 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern int
Cyc_Core_intcmp( int, int); extern unsigned char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr f1; }
; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
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
void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t;
extern void* Cyc_Absyn_bounds_one; extern void* Cyc_Absyn_atb_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq, void* b); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
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
loc, struct _tagged_arr s){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc, s));} void* Cyc_Tcutil_impos( struct _tagged_arr msg){({ struct _tagged_arr
_temp0= msg; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"Error: %.*s\n", _temp0.last_plus_one
- _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Tcutil_TypeErr);} static struct
_tagged_arr Cyc_Tcutil_tvar2string( struct Cyc_Absyn_Tvar* tv){ return* tv->name;}
void Cyc_Tcutil_print_tvars( struct Cyc_List_List* tvs){ for( 0; tvs != 0; tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl){({ struct _tagged_arr _temp1= Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd); struct
_tagged_arr _temp2= Cyc_Absynpp_ckind2string((( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs))->hd)->kind); fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s::%.*s ",
_temp1.last_plus_one - _temp1.curr, _temp1.curr, _temp2.last_plus_one - _temp2.curr,
_temp2.curr);});} fprintf( _sfile_to_file( Cyc_Stdio_stderr),"\n"); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);} static struct Cyc_List_List* Cyc_Tcutil_warning_segs=
0; static struct Cyc_List_List* Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn(
struct Cyc_Position_Segment* sg, struct _tagged_arr msg){ Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp3=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp3->hd=( void*) sg; _temp3->tl= Cyc_Tcutil_warning_segs;
_temp3;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp4=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4->hd=( void*)({ struct
_tagged_arr* _temp5=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)
* 1); _temp5[ 0]= msg; _temp5;}); _temp4->tl= Cyc_Tcutil_warning_msgs; _temp4;});}
void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs == 0){ return;}
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"***Warnings***\n");{ struct Cyc_List_List*
_temp6= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs); goto _LL7;
_LL7: Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs != 0) {({ struct _tagged_arr _temp8=( struct _tagged_arr)*((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp6))->hd); struct
_tagged_arr _temp9=*(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
Cyc_Tcutil_warning_msgs))->hd); fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s\n",
_temp8.last_plus_one - _temp8.curr, _temp8.curr, _temp9.last_plus_one - _temp9.curr,
_temp9.curr);}); _temp6=(( struct Cyc_List_List*) _check_null( _temp6))->tl; Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->tl;} fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"**************\n"); Cyc_Stdio_fflush(( struct
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
_LL348: return(( void*(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("typ_kind: Unresolved TunionFieldType",
sizeof( unsigned char), 37u)); _LL350: return( void*) Cyc_Absyn_AnyKind; _LL352:
return( void*) Cyc_Absyn_AnyKind; _LL354: return( void*) Cyc_Absyn_MemKind;
_LL356: return( void*) Cyc_Absyn_MemKind; _LL358: return( void*) Cyc_Absyn_MemKind;
_LL360: return( void*) Cyc_Absyn_MemKind; _LL362: return( void*) Cyc_Absyn_BoxKind;
_LL364: { void* _temp465=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp445.bounds))->v; void* _temp475; void*
_temp477; struct Cyc_Absyn_Exp* _temp479; struct Cyc_Absyn_Conref* _temp481;
_LL467: if(( unsigned int) _temp465 > 1u?*(( int*) _temp465) == Cyc_Absyn_Eq_constr:
0){ _LL476: _temp475=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp465)->f1;
if( _temp475 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL468;} else{ goto _LL469;}}
else{ goto _LL469;} _LL469: if(( unsigned int) _temp465 > 1u?*(( int*) _temp465)
== Cyc_Absyn_Eq_constr: 0){ _LL478: _temp477=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp465)->f1; if(( unsigned int) _temp477 > 1u?*(( int*) _temp477) == Cyc_Absyn_Upper_b:
0){ _LL480: _temp479=(( struct Cyc_Absyn_Upper_b_struct*) _temp477)->f1; goto
_LL470;} else{ goto _LL471;}} else{ goto _LL471;} _LL471: if( _temp465 ==( void*)
Cyc_Absyn_No_constr){ goto _LL472;} else{ goto _LL473;} _LL473: if((
unsigned int) _temp465 > 1u?*(( int*) _temp465) == Cyc_Absyn_Forward_constr: 0){
_LL482: _temp481=(( struct Cyc_Absyn_Forward_constr_struct*) _temp465)->f1; goto
_LL474;} else{ goto _LL466;} _LL468: return( void*) Cyc_Absyn_MemKind; _LL470:
return( void*) Cyc_Absyn_BoxKind; _LL472: return( void*) Cyc_Absyn_MemKind;
_LL474: return(( void*(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof( unsigned char), 43u)); _LL466:;} _LL366: return( void*) Cyc_Absyn_MemKind;
_LL368: return( void*) Cyc_Absyn_MemKind; _LL370: return(( void*(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp483= Cyc_Absynpp_typ2string( t); xprintf("typ_kind: typedef found: %.*s",
_temp483.last_plus_one - _temp483.curr, _temp483.curr);})); _LL372: return( void*)
Cyc_Absyn_EffKind; _LL374: return( void*) Cyc_Absyn_EffKind; _LL324:;}
unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp484; _push_handler(& _temp484);{
int _temp486= 0; if( setjmp( _temp484.handler)){ _temp486= 1;} if( ! _temp486){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp487= 1; _npop_handler( 0u); return
_temp487;}; _pop_handler();} else{ void* _temp485=( void*) _exn_thrown; void*
_temp489= _temp485; _LL491: if( _temp489 == Cyc_Tcutil_Unify){ goto _LL492;}
else{ goto _LL493;} _LL493: goto _LL494; _LL492: return 0; _LL494:( void) _throw(
_temp489); _LL490:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs( void* evar,
struct Cyc_List_List* env, void* t){ void* _temp495= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp529; struct Cyc_Core_Opt* _temp531; struct Cyc_Core_Opt**
_temp533; int _temp534; struct Cyc_Core_Opt* _temp536; struct Cyc_Core_Opt*
_temp538; struct Cyc_Absyn_PtrInfo _temp540; struct Cyc_Absyn_Exp* _temp542;
struct Cyc_Absyn_Tqual _temp544; void* _temp546; struct Cyc_Absyn_FnInfo
_temp548; struct Cyc_List_List* _temp550; struct Cyc_List_List* _temp552; struct
Cyc_Absyn_VarargInfo* _temp554; int _temp556; struct Cyc_List_List* _temp558;
void* _temp560; struct Cyc_Core_Opt* _temp562; struct Cyc_List_List* _temp564;
struct Cyc_List_List* _temp566; void* _temp568; struct Cyc_Absyn_TunionInfo
_temp570; void* _temp572; struct Cyc_List_List* _temp574; void* _temp576; struct
Cyc_Core_Opt* _temp578; struct Cyc_List_List* _temp580; struct _tuple1* _temp582;
struct Cyc_Absyn_TunionFieldInfo _temp584; struct Cyc_List_List* _temp586; void*
_temp588; struct Cyc_Absyn_Structdecl** _temp590; struct Cyc_List_List* _temp592;
struct _tuple1* _temp594; struct Cyc_List_List* _temp596; struct Cyc_List_List*
_temp598; void* _temp600; struct Cyc_List_List* _temp602; _LL497: if((
unsigned int) _temp495 > 4u?*(( int*) _temp495) == Cyc_Absyn_VarType: 0){ _LL530:
_temp529=(( struct Cyc_Absyn_VarType_struct*) _temp495)->f1; goto _LL498;} else{
goto _LL499;} _LL499: if(( unsigned int) _temp495 > 4u?*(( int*) _temp495) ==
Cyc_Absyn_Evar: 0){ _LL539: _temp538=(( struct Cyc_Absyn_Evar_struct*) _temp495)->f1;
goto _LL537; _LL537: _temp536=(( struct Cyc_Absyn_Evar_struct*) _temp495)->f2;
goto _LL535; _LL535: _temp534=(( struct Cyc_Absyn_Evar_struct*) _temp495)->f3;
goto _LL532; _LL532: _temp531=(( struct Cyc_Absyn_Evar_struct*) _temp495)->f4;
_temp533=&(( struct Cyc_Absyn_Evar_struct*) _temp495)->f4; goto _LL500;} else{
goto _LL501;} _LL501: if(( unsigned int) _temp495 > 4u?*(( int*) _temp495) ==
Cyc_Absyn_PointerType: 0){ _LL541: _temp540=(( struct Cyc_Absyn_PointerType_struct*)
_temp495)->f1; goto _LL502;} else{ goto _LL503;} _LL503: if(( unsigned int)
_temp495 > 4u?*(( int*) _temp495) == Cyc_Absyn_ArrayType: 0){ _LL547: _temp546=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp495)->f1; goto _LL545; _LL545:
_temp544=(( struct Cyc_Absyn_ArrayType_struct*) _temp495)->f2; goto _LL543;
_LL543: _temp542=(( struct Cyc_Absyn_ArrayType_struct*) _temp495)->f3; goto
_LL504;} else{ goto _LL505;} _LL505: if(( unsigned int) _temp495 > 4u?*(( int*)
_temp495) == Cyc_Absyn_FnType: 0){ _LL549: _temp548=(( struct Cyc_Absyn_FnType_struct*)
_temp495)->f1; _LL565: _temp564= _temp548.tvars; goto _LL563; _LL563: _temp562=
_temp548.effect; goto _LL561; _LL561: _temp560=( void*) _temp548.ret_typ; goto
_LL559; _LL559: _temp558= _temp548.args; goto _LL557; _LL557: _temp556= _temp548.c_varargs;
goto _LL555; _LL555: _temp554= _temp548.cyc_varargs; goto _LL553; _LL553:
_temp552= _temp548.rgn_po; goto _LL551; _LL551: _temp550= _temp548.attributes;
goto _LL506;} else{ goto _LL507;} _LL507: if(( unsigned int) _temp495 > 4u?*((
int*) _temp495) == Cyc_Absyn_TupleType: 0){ _LL567: _temp566=(( struct Cyc_Absyn_TupleType_struct*)
_temp495)->f1; goto _LL508;} else{ goto _LL509;} _LL509: if(( unsigned int)
_temp495 > 4u?*(( int*) _temp495) == Cyc_Absyn_RgnHandleType: 0){ _LL569:
_temp568=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp495)->f1; goto
_LL510;} else{ goto _LL511;} _LL511: if(( unsigned int) _temp495 > 4u?*(( int*)
_temp495) == Cyc_Absyn_TunionType: 0){ _LL571: _temp570=(( struct Cyc_Absyn_TunionType_struct*)
_temp495)->f1; _LL577: _temp576=( void*) _temp570.tunion_info; goto _LL575;
_LL575: _temp574= _temp570.targs; goto _LL573; _LL573: _temp572=( void*)
_temp570.rgn; goto _LL512;} else{ goto _LL513;} _LL513: if(( unsigned int)
_temp495 > 4u?*(( int*) _temp495) == Cyc_Absyn_TypedefType: 0){ _LL583: _temp582=((
struct Cyc_Absyn_TypedefType_struct*) _temp495)->f1; goto _LL581; _LL581:
_temp580=(( struct Cyc_Absyn_TypedefType_struct*) _temp495)->f2; goto _LL579;
_LL579: _temp578=(( struct Cyc_Absyn_TypedefType_struct*) _temp495)->f3; goto
_LL514;} else{ goto _LL515;} _LL515: if(( unsigned int) _temp495 > 4u?*(( int*)
_temp495) == Cyc_Absyn_TunionFieldType: 0){ _LL585: _temp584=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp495)->f1; _LL589: _temp588=( void*) _temp584.field_info; goto _LL587;
_LL587: _temp586= _temp584.targs; goto _LL516;} else{ goto _LL517;} _LL517: if((
unsigned int) _temp495 > 4u?*(( int*) _temp495) == Cyc_Absyn_StructType: 0){
_LL595: _temp594=(( struct Cyc_Absyn_StructType_struct*) _temp495)->f1; goto
_LL593; _LL593: _temp592=(( struct Cyc_Absyn_StructType_struct*) _temp495)->f2;
goto _LL591; _LL591: _temp590=(( struct Cyc_Absyn_StructType_struct*) _temp495)->f3;
goto _LL518;} else{ goto _LL519;} _LL519: if(( unsigned int) _temp495 > 4u?*((
int*) _temp495) == Cyc_Absyn_AnonStructType: 0){ _LL597: _temp596=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp495)->f1; goto _LL520;} else{ goto _LL521;} _LL521: if(( unsigned int)
_temp495 > 4u?*(( int*) _temp495) == Cyc_Absyn_AnonUnionType: 0){ _LL599:
_temp598=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp495)->f1; goto _LL522;}
else{ goto _LL523;} _LL523: if(( unsigned int) _temp495 > 4u?*(( int*) _temp495)
== Cyc_Absyn_AccessEff: 0){ _LL601: _temp600=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp495)->f1; goto _LL524;} else{ goto _LL525;} _LL525: if(( unsigned int)
_temp495 > 4u?*(( int*) _temp495) == Cyc_Absyn_JoinEff: 0){ _LL603: _temp602=((
struct Cyc_Absyn_JoinEff_struct*) _temp495)->f1; goto _LL526;} else{ goto _LL527;}
_LL527: goto _LL528; _LL498: if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env, _temp529)){( int) _throw(( void*) Cyc_Tcutil_Unify);}
goto _LL496; _LL500: if( t == evar){( int) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp536 != 0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp536))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp533))->v; for(
0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp604= 0; goto _LL605;
_LL605:{ struct Cyc_List_List* s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(* _temp533))->v; for( 0; s != 0; s=(( struct Cyc_List_List*)
_check_null( s))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s))->hd)){ _temp604=({ struct Cyc_List_List* _temp606=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp606->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp606->tl= _temp604; _temp606;});}}}*
_temp533=({ struct Cyc_Core_Opt* _temp607=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp607->v=( void*) _temp604; _temp607;});}}}
goto _LL496; _LL502: Cyc_Tcutil_occurs( evar, env,( void*) _temp540.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp540.rgn_typ); goto _LL496; _LL504:
Cyc_Tcutil_occurs( evar, env, _temp546); goto _LL496; _LL506: if( _temp562 != 0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp562))->v);} Cyc_Tcutil_occurs( evar, env, _temp560); for( 0; _temp558 != 0;
_temp558=(( struct Cyc_List_List*) _check_null( _temp558))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp558))->hd)).f3);}
if( _temp554 != 0){ int _temp610; void* _temp612; void* _temp614; struct Cyc_Absyn_Tqual
_temp616; struct Cyc_Core_Opt* _temp618; struct Cyc_Absyn_VarargInfo _temp608=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp554)); _LL619: _temp618=
_temp608.name; goto _LL617; _LL617: _temp616= _temp608.tq; goto _LL615; _LL615:
_temp614=( void*) _temp608.type; goto _LL613; _LL613: _temp612=( void*) _temp608.rgn;
goto _LL611; _LL611: _temp610= _temp608.inject; goto _LL609; _LL609: Cyc_Tcutil_occurs(
evar, env, _temp614); Cyc_Tcutil_occurs( evar, env, _temp612);} for( 0; _temp552
!= 0; _temp552=(( struct Cyc_List_List*) _check_null( _temp552))->tl){ struct
_tuple8 _temp622; void* _temp623; void* _temp625; struct _tuple8* _temp620=(
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp552))->hd; _temp622=*
_temp620; _LL626: _temp625= _temp622.f1; goto _LL624; _LL624: _temp623= _temp622.f2;
goto _LL621; _LL621: Cyc_Tcutil_occurs( evar, env, _temp625); Cyc_Tcutil_occurs(
evar, env, _temp623);} goto _LL496; _LL508: for( 0; _temp566 != 0; _temp566=((
struct Cyc_List_List*) _check_null( _temp566))->tl){ Cyc_Tcutil_occurs( evar,
env,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp566))->hd)).f2);}
goto _LL496; _LL510: Cyc_Tcutil_occurs( evar, env, _temp568); goto _LL496;
_LL512: Cyc_Tcutil_occurs( evar, env, _temp572); Cyc_Tcutil_occurslist( evar,
env, _temp574); goto _LL496; _LL514: _temp586= _temp580; goto _LL516; _LL516:
_temp592= _temp586; goto _LL518; _LL518: Cyc_Tcutil_occurslist( evar, env,
_temp592); goto _LL496; _LL520: _temp598= _temp596; goto _LL522; _LL522: for( 0;
_temp598 != 0; _temp598=(( struct Cyc_List_List*) _check_null( _temp598))->tl){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp598))->hd)->type);} goto _LL496; _LL524: Cyc_Tcutil_occurs(
evar, env, _temp600); goto _LL496; _LL526: Cyc_Tcutil_occurslist( evar, env,
_temp602); goto _LL496; _LL528: goto _LL496; _LL496:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* env, struct Cyc_List_List* ts){ for( 0; ts !=
0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs( evar,
env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static void Cyc_Tcutil_unify_list(
struct Cyc_List_List* t1, struct Cyc_List_List* t2){ struct _handler_cons
_temp627; _push_handler(& _temp627);{ int _temp629= 0; if( setjmp( _temp627.handler)){
_temp629= 1;} if( ! _temp629){(( void(*)( void(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_iter2)( Cyc_Tcutil_unify_it, t1, t2);;
_pop_handler();} else{ void* _temp628=( void*) _exn_thrown; void* _temp631=
_temp628; _LL633: if( _temp631 == Cyc_List_List_mismatch){ goto _LL634;} else{
goto _LL635;} _LL635: goto _LL636; _LL634:( int) _throw(( void*) Cyc_Tcutil_Unify);
_LL636:( void) _throw( _temp631); _LL632:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != tq2.q_const?
1: tq1.q_volatile != tq2.q_volatile)? 1: tq1.q_restrict != tq2.q_restrict){( int)
_throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile
== tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp637=( void*) x->v; struct Cyc_Absyn_Conref*
_temp645; void* _temp647; _LL639: if( _temp637 ==( void*) Cyc_Absyn_No_constr){
goto _LL640;} else{ goto _LL641;} _LL641: if(( unsigned int) _temp637 > 1u?*((
int*) _temp637) == Cyc_Absyn_Forward_constr: 0){ _LL646: _temp645=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp637)->f1; goto _LL642;} else{ goto _LL643;} _LL643: if(( unsigned int)
_temp637 > 1u?*(( int*) _temp637) == Cyc_Absyn_Eq_constr: 0){ _LL648: _temp647=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp637)->f1; goto _LL644;} else{
goto _LL638;} _LL640:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp649=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp649[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp650; _temp650.tag= Cyc_Absyn_Forward_constr; _temp650.f1= y; _temp650;});
_temp649;}))); return; _LL642:( int) _throw((( void*(*)( struct _tagged_arr msg))
Cyc_Tcutil_impos)( _tag_arr("unify_conref: forward after compress", sizeof(
unsigned char), 37u))); _LL644: { void* _temp651=( void*) y->v; struct Cyc_Absyn_Conref*
_temp659; void* _temp661; _LL653: if( _temp651 ==( void*) Cyc_Absyn_No_constr){
goto _LL654;} else{ goto _LL655;} _LL655: if(( unsigned int) _temp651 > 1u?*((
int*) _temp651) == Cyc_Absyn_Forward_constr: 0){ _LL660: _temp659=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp651)->f1; goto _LL656;} else{ goto _LL657;} _LL657: if(( unsigned int)
_temp651 > 1u?*(( int*) _temp651) == Cyc_Absyn_Eq_constr: 0){ _LL662: _temp661=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp651)->f1; goto _LL658;} else{
goto _LL652;} _LL654:( void*)( y->v=( void*)(( void*) x->v)); return; _LL656:(
int) _throw((( void*(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("unify_conref: forward after compress(2)",
sizeof( unsigned char), 40u))); _LL658: if( cmp( _temp647, _temp661) != 0){( int)
_throw(( void*) Cyc_Tcutil_Unify);} return; _LL652:;} _LL638:;}} static int Cyc_Tcutil_unify_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ struct _handler_cons _temp663; _push_handler(& _temp663);{ int _temp665= 0;
if( setjmp( _temp663.handler)){ _temp665= 1;} if( ! _temp665){(( void(*)( int(*
cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int _temp666= 1; _npop_handler( 0u);
return _temp666;}; _pop_handler();} else{ void* _temp664=( void*) _exn_thrown;
void* _temp668= _temp664; _LL670: if( _temp668 == Cyc_Tcutil_Unify){ goto _LL671;}
else{ goto _LL672;} _LL672: goto _LL673; _LL671: return 0; _LL673:( void) _throw(
_temp668); _LL669:;}}} static int Cyc_Tcutil_boundscmp( void* b1, void* b2){
struct _tuple8 _temp675=({ struct _tuple8 _temp674; _temp674.f1= b1; _temp674.f2=
b2; _temp674;}); void* _temp685; void* _temp687; void* _temp689; void* _temp691;
void* _temp693; void* _temp695; void* _temp697; struct Cyc_Absyn_Exp* _temp699;
void* _temp701; struct Cyc_Absyn_Exp* _temp703; _LL677: _LL688: _temp687=
_temp675.f1; if( _temp687 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL686;} else{
goto _LL679;} _LL686: _temp685= _temp675.f2; if( _temp685 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL678;} else{ goto _LL679;} _LL679: _LL692: _temp691= _temp675.f1; if(
_temp691 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL690;} else{ goto _LL681;}
_LL690: _temp689= _temp675.f2; goto _LL680; _LL681: _LL696: _temp695= _temp675.f1;
goto _LL694; _LL694: _temp693= _temp675.f2; if( _temp693 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL682;} else{ goto _LL683;} _LL683: _LL702: _temp701= _temp675.f1; if((
unsigned int) _temp701 > 1u?*(( int*) _temp701) == Cyc_Absyn_Upper_b: 0){ _LL704:
_temp703=(( struct Cyc_Absyn_Upper_b_struct*) _temp701)->f1; goto _LL698;} else{
goto _LL676;} _LL698: _temp697= _temp675.f2; if(( unsigned int) _temp697 > 1u?*((
int*) _temp697) == Cyc_Absyn_Upper_b: 0){ _LL700: _temp699=(( struct Cyc_Absyn_Upper_b_struct*)
_temp697)->f1; goto _LL684;} else{ goto _LL676;} _LL678: return 0; _LL680:
return - 1; _LL682: return 1; _LL684: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp703); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp699); if( i1 == i2){
return 0;} if( i1 < i2){ return - 1;} return 1;} _LL676:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 == a2){ return 1;}{ struct _tuple8 _temp706=({
struct _tuple8 _temp705; _temp705.f1= a1; _temp705.f2= a2; _temp705;}); void*
_temp716; int _temp718; void* _temp720; int _temp722; void* _temp724; int
_temp726; void* _temp728; int _temp730; void* _temp732; struct _tagged_arr
_temp734; void* _temp736; struct _tagged_arr _temp738; _LL708: _LL721: _temp720=
_temp706.f1; if(( unsigned int) _temp720 > 16u?*(( int*) _temp720) == Cyc_Absyn_Regparm_att:
0){ _LL723: _temp722=(( struct Cyc_Absyn_Regparm_att_struct*) _temp720)->f1;
goto _LL717;} else{ goto _LL710;} _LL717: _temp716= _temp706.f2; if((
unsigned int) _temp716 > 16u?*(( int*) _temp716) == Cyc_Absyn_Regparm_att: 0){
_LL719: _temp718=(( struct Cyc_Absyn_Regparm_att_struct*) _temp716)->f1; goto
_LL709;} else{ goto _LL710;} _LL710: _LL729: _temp728= _temp706.f1; if((
unsigned int) _temp728 > 16u?*(( int*) _temp728) == Cyc_Absyn_Aligned_att: 0){
_LL731: _temp730=(( struct Cyc_Absyn_Aligned_att_struct*) _temp728)->f1; goto
_LL725;} else{ goto _LL712;} _LL725: _temp724= _temp706.f2; if(( unsigned int)
_temp724 > 16u?*(( int*) _temp724) == Cyc_Absyn_Aligned_att: 0){ _LL727:
_temp726=(( struct Cyc_Absyn_Aligned_att_struct*) _temp724)->f1; goto _LL711;}
else{ goto _LL712;} _LL712: _LL737: _temp736= _temp706.f1; if(( unsigned int)
_temp736 > 16u?*(( int*) _temp736) == Cyc_Absyn_Section_att: 0){ _LL739:
_temp738=(( struct Cyc_Absyn_Section_att_struct*) _temp736)->f1; goto _LL733;}
else{ goto _LL714;} _LL733: _temp732= _temp706.f2; if(( unsigned int) _temp732 >
16u?*(( int*) _temp732) == Cyc_Absyn_Section_att: 0){ _LL735: _temp734=(( struct
Cyc_Absyn_Section_att_struct*) _temp732)->f1; goto _LL713;} else{ goto _LL714;}
_LL714: goto _LL715; _LL709: _temp730= _temp722; _temp726= _temp718; goto _LL711;
_LL711: return _temp730 == _temp726; _LL713: return Cyc_String_strcmp( _temp738,
_temp734) == 0; _LL715: return 0; _LL707:;}} int Cyc_Tcutil_same_atts( struct
Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for(
0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( !(( int(*)(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){
return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( !(( int(*)( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a1)){ return 0;}}} return 1;}
static void Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp740= e; struct Cyc_List_List* _temp746; struct Cyc_List_List**
_temp748; _LL742: if(( unsigned int) _temp740 > 4u?*(( int*) _temp740) == Cyc_Absyn_JoinEff:
0){ _LL747: _temp746=(( struct Cyc_Absyn_JoinEff_struct*) _temp740)->f1;
_temp748=&(( struct Cyc_Absyn_JoinEff_struct*) _temp740)->f1; goto _LL743;}
else{ goto _LL744;} _LL744: goto _LL745; _LL743: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp748; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp749=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; goto _LL750; _LL750: Cyc_Tcutil_normalize_effect(
_temp749);{ void* _temp751= Cyc_Tcutil_compress( _temp749); struct Cyc_List_List*
_temp757; _LL753: if(( unsigned int) _temp751 > 4u?*(( int*) _temp751) == Cyc_Absyn_JoinEff:
0){ _LL758: _temp757=(( struct Cyc_Absyn_JoinEff_struct*) _temp751)->f1; goto
_LL754;} else{ goto _LL755;} _LL755: goto _LL756; _LL754: nested_join= 1; goto
_LL752; _LL756: goto _LL752; _LL752:;}}} if( ! nested_join){ return;}{ struct
Cyc_List_List* effects= 0;{ struct Cyc_List_List* effs=* _temp748; for( 0; effs
!= 0; effs=(( struct Cyc_List_List*) _check_null( effs))->tl){ void* _temp759=
Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null( effs))->hd);
struct Cyc_List_List* _temp765; _LL761: if(( unsigned int) _temp759 > 4u?*(( int*)
_temp759) == Cyc_Absyn_JoinEff: 0){ _LL766: _temp765=(( struct Cyc_Absyn_JoinEff_struct*)
_temp759)->f1; goto _LL762;} else{ goto _LL763;} _LL763: goto _LL764; _LL762:
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_revappend)( _temp765, effects); goto _LL760; _LL764: effects=({
struct Cyc_List_List* _temp767=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp767->hd=( void*) _temp759; _temp767->tl= effects;
_temp767;}); goto _LL760; _LL760:;}}* _temp748=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto _LL741;}} _LL745:
goto _LL741; _LL741:;}} struct _tuple9{ void* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp768= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp776; struct Cyc_List_List _temp778; struct Cyc_List_List* _temp779; void*
_temp781; struct Cyc_Core_Opt* _temp783; int _temp785; struct Cyc_Core_Opt*
_temp787; struct Cyc_Core_Opt* _temp789; _LL770: if(( unsigned int) _temp768 > 4u?*((
int*) _temp768) == Cyc_Absyn_JoinEff: 0){ _LL777: _temp776=(( struct Cyc_Absyn_JoinEff_struct*)
_temp768)->f1; if( _temp776 == 0){ goto _LL772;} else{ _temp778=* _temp776;
_LL782: _temp781=( void*) _temp778.hd; goto _LL780; _LL780: _temp779= _temp778.tl;
goto _LL771;}} else{ goto _LL772;} _LL772: if(( unsigned int) _temp768 > 4u?*((
int*) _temp768) == Cyc_Absyn_Evar: 0){ _LL790: _temp789=(( struct Cyc_Absyn_Evar_struct*)
_temp768)->f1; goto _LL788; _LL788: _temp787=(( struct Cyc_Absyn_Evar_struct*)
_temp768)->f2; goto _LL786; _LL786: _temp785=(( struct Cyc_Absyn_Evar_struct*)
_temp768)->f3; goto _LL784; _LL784: _temp783=(( struct Cyc_Absyn_Evar_struct*)
_temp768)->f4; goto _LL773;} else{ goto _LL774;} _LL774: goto _LL775; _LL771: {
void* _temp791= Cyc_Tcutil_compress( _temp781); struct Cyc_Core_Opt* _temp797;
int _temp799; struct Cyc_Core_Opt* _temp801; struct Cyc_Core_Opt* _temp803;
_LL793: if(( unsigned int) _temp791 > 4u?*(( int*) _temp791) == Cyc_Absyn_Evar:
0){ _LL804: _temp803=(( struct Cyc_Absyn_Evar_struct*) _temp791)->f1; goto
_LL802; _LL802: _temp801=(( struct Cyc_Absyn_Evar_struct*) _temp791)->f2; goto
_LL800; _LL800: _temp799=(( struct Cyc_Absyn_Evar_struct*) _temp791)->f3; goto
_LL798; _LL798: _temp797=(( struct Cyc_Absyn_Evar_struct*) _temp791)->f4; goto
_LL794;} else{ goto _LL795;} _LL795: goto _LL796; _LL794: return({ struct
_tuple9* _temp805=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp805->f1= _temp781; _temp805->f2= _temp779; _temp805->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp797))->v; _temp805;}); _LL796: return 0;
_LL792:;} _LL773: if( _temp789 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp789))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));} return({ struct _tuple9* _temp806=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp806->f1= t; _temp806->f2= 0;
_temp806->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp783))->v; _temp806;}); _LL775: return 0; _LL769:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; int Cyc_Tcutil_region_in_effect( int
constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*) Cyc_Absyn_HeapRgn){
return 1;}{ void* _temp807= Cyc_Tcutil_compress( e); void* _temp817; struct Cyc_List_List*
_temp819; struct Cyc_Core_Opt* _temp821; int _temp823; struct Cyc_Core_Opt*
_temp825; struct Cyc_Core_Opt** _temp827; struct Cyc_Core_Opt* _temp828; _LL809:
if(( unsigned int) _temp807 > 4u?*(( int*) _temp807) == Cyc_Absyn_AccessEff: 0){
_LL818: _temp817=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp807)->f1;
goto _LL810;} else{ goto _LL811;} _LL811: if(( unsigned int) _temp807 > 4u?*((
int*) _temp807) == Cyc_Absyn_JoinEff: 0){ _LL820: _temp819=(( struct Cyc_Absyn_JoinEff_struct*)
_temp807)->f1; goto _LL812;} else{ goto _LL813;} _LL813: if(( unsigned int)
_temp807 > 4u?*(( int*) _temp807) == Cyc_Absyn_Evar: 0){ _LL829: _temp828=((
struct Cyc_Absyn_Evar_struct*) _temp807)->f1; goto _LL826; _LL826: _temp825=((
struct Cyc_Absyn_Evar_struct*) _temp807)->f2; _temp827=&(( struct Cyc_Absyn_Evar_struct*)
_temp807)->f2; goto _LL824; _LL824: _temp823=(( struct Cyc_Absyn_Evar_struct*)
_temp807)->f3; goto _LL822; _LL822: _temp821=(( struct Cyc_Absyn_Evar_struct*)
_temp807)->f4; goto _LL814;} else{ goto _LL815;} _LL815: goto _LL816; _LL810:
if( constrain){ return Cyc_Tcutil_unify( r, _temp817);} Cyc_Tcutil_compress(
_temp817); if( r == _temp817){ return 1;}{ struct _tuple8 _temp831=({ struct
_tuple8 _temp830; _temp830.f1= r; _temp830.f2= _temp817; _temp830;}); void*
_temp837; struct Cyc_Absyn_Tvar* _temp839; void* _temp841; struct Cyc_Absyn_Tvar*
_temp843; _LL833: _LL842: _temp841= _temp831.f1; if(( unsigned int) _temp841 > 4u?*((
int*) _temp841) == Cyc_Absyn_VarType: 0){ _LL844: _temp843=(( struct Cyc_Absyn_VarType_struct*)
_temp841)->f1; goto _LL838;} else{ goto _LL835;} _LL838: _temp837= _temp831.f2;
if(( unsigned int) _temp837 > 4u?*(( int*) _temp837) == Cyc_Absyn_VarType: 0){
_LL840: _temp839=(( struct Cyc_Absyn_VarType_struct*) _temp837)->f1; goto _LL834;}
else{ goto _LL835;} _LL835: goto _LL836; _LL834: return Cyc_Absyn_tvar_cmp(
_temp843, _temp839) == 0; _LL836: return 0; _LL832:;} _LL812: for( 0; _temp819
!= 0; _temp819=(( struct Cyc_List_List*) _check_null( _temp819))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp819))->hd)){
return 1;}} return 0; _LL814: if( _temp828 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp828))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));} if( ! constrain){ return 0;}{ void* _temp845=
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp821); goto
_LL846; _LL846: Cyc_Tcutil_occurs( _temp845,( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp821))->v, r);{ struct Cyc_Absyn_JoinEff_struct*
_temp853=({ struct Cyc_Absyn_JoinEff_struct* _temp847=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp847[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp848; _temp848.tag= Cyc_Absyn_JoinEff; _temp848.f1=({ struct Cyc_List_List*
_temp849=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp849->hd=( void*) _temp845; _temp849->tl=({ struct Cyc_List_List* _temp850=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp850->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp851=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp851[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp852; _temp852.tag= Cyc_Absyn_AccessEff; _temp852.f1=(
void*) r; _temp852;}); _temp851;})); _temp850->tl= 0; _temp850;}); _temp849;});
_temp848;}); _temp847;}); goto _LL854; _LL854:* _temp827=({ struct Cyc_Core_Opt*
_temp855=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp855->v=( void*)(( void*) _temp853); _temp855;}); return 1;}} _LL816: return
0; _LL808:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp856= e;
struct Cyc_Absyn_Tvar* _temp866; struct Cyc_List_List* _temp868; struct Cyc_Core_Opt*
_temp870; int _temp872; struct Cyc_Core_Opt* _temp874; struct Cyc_Core_Opt**
_temp876; struct Cyc_Core_Opt* _temp877; _LL858: if(( unsigned int) _temp856 > 4u?*((
int*) _temp856) == Cyc_Absyn_VarType: 0){ _LL867: _temp866=(( struct Cyc_Absyn_VarType_struct*)
_temp856)->f1; goto _LL859;} else{ goto _LL860;} _LL860: if(( unsigned int)
_temp856 > 4u?*(( int*) _temp856) == Cyc_Absyn_JoinEff: 0){ _LL869: _temp868=((
struct Cyc_Absyn_JoinEff_struct*) _temp856)->f1; goto _LL861;} else{ goto _LL862;}
_LL862: if(( unsigned int) _temp856 > 4u?*(( int*) _temp856) == Cyc_Absyn_Evar:
0){ _LL878: _temp877=(( struct Cyc_Absyn_Evar_struct*) _temp856)->f1; goto
_LL875; _LL875: _temp874=(( struct Cyc_Absyn_Evar_struct*) _temp856)->f2;
_temp876=&(( struct Cyc_Absyn_Evar_struct*) _temp856)->f2; goto _LL873; _LL873:
_temp872=(( struct Cyc_Absyn_Evar_struct*) _temp856)->f3; goto _LL871; _LL871:
_temp870=(( struct Cyc_Absyn_Evar_struct*) _temp856)->f4; goto _LL863;} else{
goto _LL864;} _LL864: goto _LL865; _LL859: return Cyc_Absyn_tvar_cmp( v,
_temp866) == 0; _LL861: for( 0; _temp868 != 0; _temp868=(( struct Cyc_List_List*)
_check_null( _temp868))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp868))->hd)){ return 1;}}
return 0; _LL863: if( _temp877 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp877))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));}{ void* _temp879= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp870); goto _LL880; _LL880: if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp870))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp887=({ struct Cyc_Absyn_JoinEff_struct* _temp881=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp881[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp882; _temp882.tag= Cyc_Absyn_JoinEff; _temp882.f1=({ struct Cyc_List_List*
_temp883=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp883->hd=( void*) _temp879; _temp883->tl=({ struct Cyc_List_List* _temp884=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp884->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp885=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp885[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp886; _temp886.tag= Cyc_Absyn_VarType; _temp886.f1= v; _temp886;}); _temp885;}));
_temp884->tl= 0; _temp884;}); _temp883;}); _temp882;}); _temp881;}); goto _LL888;
_LL888:* _temp876=({ struct Cyc_Core_Opt* _temp889=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp889->v=( void*)(( void*) _temp887);
_temp889;}); return 1;}} _LL865: return 0; _LL857:;}} static int Cyc_Tcutil_evar_in_effect(
void* evar, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp890= e; struct Cyc_List_List*
_temp898; struct Cyc_Core_Opt* _temp900; int _temp902; struct Cyc_Core_Opt*
_temp904; struct Cyc_Core_Opt* _temp906; _LL892: if(( unsigned int) _temp890 > 4u?*((
int*) _temp890) == Cyc_Absyn_JoinEff: 0){ _LL899: _temp898=(( struct Cyc_Absyn_JoinEff_struct*)
_temp890)->f1; goto _LL893;} else{ goto _LL894;} _LL894: if(( unsigned int)
_temp890 > 4u?*(( int*) _temp890) == Cyc_Absyn_Evar: 0){ _LL907: _temp906=((
struct Cyc_Absyn_Evar_struct*) _temp890)->f1; goto _LL905; _LL905: _temp904=((
struct Cyc_Absyn_Evar_struct*) _temp890)->f2; goto _LL903; _LL903: _temp902=((
struct Cyc_Absyn_Evar_struct*) _temp890)->f3; goto _LL901; _LL901: _temp900=((
struct Cyc_Absyn_Evar_struct*) _temp890)->f4; goto _LL895;} else{ goto _LL896;}
_LL896: goto _LL897; _LL893: for( 0; _temp898 != 0; _temp898=(( struct Cyc_List_List*)
_check_null( _temp898))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp898))->hd)){ return 1;}} return 0;
_LL895: return evar == e; _LL897: return 0; _LL891:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){{ struct _tuple9* _temp908= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple9 _temp914; struct Cyc_List_List* _temp915; struct Cyc_List_List*
_temp917; void* _temp919; _LL910: if( _temp908 == 0){ goto _LL912;} else{
_temp914=* _temp908; _LL920: _temp919= _temp914.f1; goto _LL918; _LL918:
_temp917= _temp914.f2; goto _LL916; _LL916: _temp915= _temp914.f3; goto _LL911;}
_LL912: goto _LL913; _LL911: { void* _temp922= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp921=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp921->v=( void*) _temp915; _temp921;})); goto
_LL923; _LL923: { struct Cyc_Absyn_JoinEff_struct* _temp928=({ struct Cyc_Absyn_JoinEff_struct*
_temp924=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp924[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp925; _temp925.tag= Cyc_Absyn_JoinEff;
_temp925.f1=({ struct Cyc_List_List* _temp926=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp926->hd=( void*) _temp922; _temp926->tl=({
struct Cyc_List_List* _temp927=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp927->hd=( void*) e1; _temp927->tl= 0; _temp927;});
_temp926;}); _temp925;}); _temp924;}); goto _LL929; _LL929: return Cyc_Tcutil_unify(
_temp919,( void*) _temp928);}} _LL913: goto _LL909; _LL909:;}{ void* _temp930=
Cyc_Tcutil_compress( e1); struct Cyc_List_List* _temp942; void* _temp944; struct
Cyc_Absyn_Tvar* _temp946; struct Cyc_Core_Opt* _temp948; int _temp950; struct
Cyc_Core_Opt* _temp952; struct Cyc_Core_Opt** _temp954; struct Cyc_Core_Opt*
_temp955; _LL932: if(( unsigned int) _temp930 > 4u?*(( int*) _temp930) == Cyc_Absyn_JoinEff:
0){ _LL943: _temp942=(( struct Cyc_Absyn_JoinEff_struct*) _temp930)->f1; goto
_LL933;} else{ goto _LL934;} _LL934: if(( unsigned int) _temp930 > 4u?*(( int*)
_temp930) == Cyc_Absyn_AccessEff: 0){ _LL945: _temp944=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp930)->f1; goto _LL935;} else{ goto _LL936;} _LL936: if(( unsigned int)
_temp930 > 4u?*(( int*) _temp930) == Cyc_Absyn_VarType: 0){ _LL947: _temp946=((
struct Cyc_Absyn_VarType_struct*) _temp930)->f1; goto _LL937;} else{ goto _LL938;}
_LL938: if(( unsigned int) _temp930 > 4u?*(( int*) _temp930) == Cyc_Absyn_Evar:
0){ _LL956: _temp955=(( struct Cyc_Absyn_Evar_struct*) _temp930)->f1; goto
_LL953; _LL953: _temp952=(( struct Cyc_Absyn_Evar_struct*) _temp930)->f2;
_temp954=&(( struct Cyc_Absyn_Evar_struct*) _temp930)->f2; goto _LL951; _LL951:
_temp950=(( struct Cyc_Absyn_Evar_struct*) _temp930)->f3; goto _LL949; _LL949:
_temp948=(( struct Cyc_Absyn_Evar_struct*) _temp930)->f4; goto _LL939;} else{
goto _LL940;} _LL940: goto _LL941; _LL933: for( 0; _temp942 != 0; _temp942=((
struct Cyc_List_List*) _check_null( _temp942))->tl){ if( ! Cyc_Tcutil_subset_effect(
set_to_empty,( void*)(( struct Cyc_List_List*) _check_null( _temp942))->hd, e2)){
return 0;}} return 1; _LL935: return( Cyc_Tcutil_region_in_effect( 0, _temp944,
e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp944, e2))? 1: Cyc_Tcutil_unify(
_temp944,( void*) Cyc_Absyn_HeapRgn); _LL937: return Cyc_Tcutil_variable_in_effect(
0, _temp946, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp946, e2); _LL939:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp954=({
struct Cyc_Core_Opt* _temp957=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp957->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp958=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp958[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp959; _temp959.tag= Cyc_Absyn_JoinEff;
_temp959.f1= 0; _temp959;}); _temp958;})); _temp957;});} else{ Cyc_Tcutil_occurs(
e1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp948))->v,
e2);* _temp954=({ struct Cyc_Core_Opt* _temp960=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp960->v=( void*) e2; _temp960;});}}
return 1; _LL941: return(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp961= Cyc_Absynpp_typ2string( e1);
xprintf("subset_effect: bad effect: %.*s", _temp961.last_plus_one - _temp961.curr,
_temp961.curr);})); _LL931:;}} struct _tuple10{ struct _tuple9* f1; struct
_tuple9* f2; } ; static int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1=
Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2); Cyc_Tcutil_normalize_effect(
e1); Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp963=({ struct
_tuple10 _temp962; _temp962.f1= Cyc_Tcutil_get_effect_evar( e1); _temp962.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp962;}); struct _tuple9* _temp969; struct
_tuple9 _temp971; struct Cyc_List_List* _temp972; struct Cyc_List_List* _temp974;
void* _temp976; struct _tuple9* _temp978; struct _tuple9 _temp980; struct Cyc_List_List*
_temp981; struct Cyc_List_List* _temp983; void* _temp985; _LL965: _LL979:
_temp978= _temp963.f1; if( _temp978 == 0){ goto _LL967;} else{ _temp980=*
_temp978; _LL986: _temp985= _temp980.f1; goto _LL984; _LL984: _temp983= _temp980.f2;
goto _LL982; _LL982: _temp981= _temp980.f3; goto _LL970;} _LL970: _temp969=
_temp963.f2; if( _temp969 == 0){ goto _LL967;} else{ _temp971=* _temp969; _LL977:
_temp976= _temp971.f1; goto _LL975; _LL975: _temp974= _temp971.f2; goto _LL973;
_LL973: _temp972= _temp971.f3; goto _LL966;} _LL967: goto _LL968; _LL966: {
struct Cyc_List_List* _temp987= 0; goto _LL988; _LL988: for( 0; _temp981 != 0;
_temp981=(( struct Cyc_List_List*) _check_null( _temp981))->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, _temp972,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp981))->hd)){
_temp987=({ struct Cyc_List_List* _temp989=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp989->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp981))->hd); _temp989->tl= _temp987;
_temp989;});}}{ void* _temp991= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,({
struct Cyc_Core_Opt* _temp990=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp990->v=( void*) _temp987; _temp990;})); goto _LL992; _LL992: {
struct Cyc_Absyn_JoinEff_struct* _temp996=({ struct Cyc_Absyn_JoinEff_struct*
_temp993=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp993[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp994; _temp994.tag= Cyc_Absyn_JoinEff;
_temp994.f1=({ struct Cyc_List_List* _temp995=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp995->hd=( void*) _temp991; _temp995->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp983, _temp974); _temp995;}); _temp994;}); _temp993;}); goto _LL997; _LL997:
return Cyc_Tcutil_unify( _temp985,( void*) _temp996)? Cyc_Tcutil_unify( _temp976,(
void*) _temp996): 0;}}} _LL968: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect(
0, e2, e1): 0; _LL964:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1
!= 0; r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8
_temp1000; void* _temp1001; void* _temp1003; struct _tuple8* _temp998=( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( r1))->hd; _temp1000=* _temp998;
_LL1004: _temp1003= _temp1000.f1; goto _LL1002; _LL1002: _temp1001= _temp1000.f2;
goto _LL999; _LL999: { int found= _temp1003 ==( void*) Cyc_Absyn_HeapRgn;{
struct Cyc_List_List* r2= rpo2; for( 0; r2 != 0? ! found: 0; r2=(( struct Cyc_List_List*)
_check_null( r2))->tl){ struct _tuple8 _temp1007; void* _temp1008; void*
_temp1010; struct _tuple8* _temp1005=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( r2))->hd; _temp1007=* _temp1005; _LL1011: _temp1010= _temp1007.f1;
goto _LL1009; _LL1009: _temp1008= _temp1007.f2; goto _LL1006; _LL1006: if( Cyc_Tcutil_unify(
_temp1003, _temp1010)? Cyc_Tcutil_unify( _temp1001, _temp1008): 0){ found= 1;
break;}}} if( ! found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List* rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo(
rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo( rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo*
f1; struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){
return;}{ void* _temp1012= t1; struct Cyc_Core_Opt* _temp1018; int _temp1020;
struct Cyc_Core_Opt* _temp1022; struct Cyc_Core_Opt** _temp1024; struct Cyc_Core_Opt*
_temp1025; _LL1014: if(( unsigned int) _temp1012 > 4u?*(( int*) _temp1012) ==
Cyc_Absyn_Evar: 0){ _LL1026: _temp1025=(( struct Cyc_Absyn_Evar_struct*)
_temp1012)->f1; goto _LL1023; _LL1023: _temp1022=(( struct Cyc_Absyn_Evar_struct*)
_temp1012)->f2; _temp1024=&(( struct Cyc_Absyn_Evar_struct*) _temp1012)->f2;
goto _LL1021; _LL1021: _temp1020=(( struct Cyc_Absyn_Evar_struct*) _temp1012)->f3;
goto _LL1019; _LL1019: _temp1018=(( struct Cyc_Absyn_Evar_struct*) _temp1012)->f4;
goto _LL1015;} else{ goto _LL1016;} _LL1016: goto _LL1017; _LL1015: Cyc_Tcutil_occurs(
t1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1018))->v,
t2);{ void* _temp1027= Cyc_Tcutil_typ_kind( t2); goto _LL1028; _LL1028: if( Cyc_Tcutil_kind_leq(
_temp1027,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1025))->v)){*
_temp1024=({ struct Cyc_Core_Opt* _temp1029=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1029->v=( void*) t2; _temp1029;}); return;}
else{{ void* _temp1030= t2; struct Cyc_Core_Opt* _temp1038; int _temp1040;
struct Cyc_Core_Opt* _temp1042; struct Cyc_Core_Opt** _temp1044; struct Cyc_Core_Opt*
_temp1045; struct Cyc_Absyn_PtrInfo _temp1048; _LL1032: if(( unsigned int)
_temp1030 > 4u?*(( int*) _temp1030) == Cyc_Absyn_Evar: 0){ _LL1046: _temp1045=((
struct Cyc_Absyn_Evar_struct*) _temp1030)->f1; goto _LL1043; _LL1043: _temp1042=((
struct Cyc_Absyn_Evar_struct*) _temp1030)->f2; _temp1044=&(( struct Cyc_Absyn_Evar_struct*)
_temp1030)->f2; goto _LL1041; _LL1041: _temp1040=(( struct Cyc_Absyn_Evar_struct*)
_temp1030)->f3; goto _LL1039; _LL1039: _temp1038=(( struct Cyc_Absyn_Evar_struct*)
_temp1030)->f4; goto _LL1033;} else{ goto _LL1034;} _LL1034: if(( unsigned int)
_temp1030 > 4u?*(( int*) _temp1030) == Cyc_Absyn_PointerType: 0){ _LL1049:
_temp1048=(( struct Cyc_Absyn_PointerType_struct*) _temp1030)->f1; goto _LL1047;}
else{ goto _LL1036;} _LL1047: if(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1025))->v ==( void*) Cyc_Absyn_BoxKind){ goto _LL1035;} else{ goto _LL1036;}
_LL1036: goto _LL1037; _LL1033: { struct Cyc_List_List* _temp1050=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1018))->v; goto _LL1051; _LL1051:{
struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1038))->v; for( 0; s2 != 0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp1050,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1025))->v, _temp1027)){*
_temp1044=({ struct Cyc_Core_Opt* _temp1052=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1052->v=( void*) t1; _temp1052;}); return;}
goto _LL1031;} _LL1035: { struct Cyc_Absyn_Conref* _temp1053=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1048.bounds); goto
_LL1054; _LL1054:{ void* _temp1055=( void*) _temp1053->v; _LL1057: if( _temp1055
==( void*) Cyc_Absyn_No_constr){ goto _LL1058;} else{ goto _LL1059;} _LL1059:
goto _LL1060; _LL1058:( void*)( _temp1053->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp1061=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp1061[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp1062; _temp1062.tag= Cyc_Absyn_Eq_constr;
_temp1062.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1063=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1063[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1064; _temp1064.tag= Cyc_Absyn_Upper_b;
_temp1064.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1064;}); _temp1063;}));
_temp1062;}); _temp1061;})));* _temp1024=({ struct Cyc_Core_Opt* _temp1065=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1065->v=(
void*) t2; _temp1065;}); return; _LL1060: goto _LL1056; _LL1056:;} goto _LL1031;}
_LL1037: goto _LL1031; _LL1031:;}( int) _throw(( void*) Cyc_Tcutil_Unify);}}
_LL1017: goto _LL1013; _LL1013:;}{ struct _tuple8 _temp1067=({ struct _tuple8
_temp1066; _temp1066.f1= t2; _temp1066.f2= t1; _temp1066;}); void* _temp1117;
void* _temp1119; struct Cyc_Core_Opt* _temp1121; int _temp1123; struct Cyc_Core_Opt*
_temp1125; struct Cyc_Core_Opt* _temp1127; void* _temp1129; void* _temp1131;
void* _temp1133; struct Cyc_Absyn_Tvar* _temp1135; void* _temp1137; struct Cyc_Absyn_Tvar*
_temp1139; void* _temp1141; struct Cyc_Absyn_Structdecl** _temp1143; struct Cyc_List_List*
_temp1145; struct _tuple1* _temp1147; void* _temp1149; struct Cyc_Absyn_Structdecl**
_temp1151; struct Cyc_List_List* _temp1153; struct _tuple1* _temp1155; void*
_temp1157; struct Cyc_Absyn_Enumdecl* _temp1159; struct _tuple1* _temp1161; void*
_temp1163; struct Cyc_Absyn_Enumdecl* _temp1165; struct _tuple1* _temp1167; void*
_temp1169; struct Cyc_Absyn_Uniondecl** _temp1171; struct Cyc_List_List*
_temp1173; struct _tuple1* _temp1175; void* _temp1177; struct Cyc_Absyn_Uniondecl**
_temp1179; struct Cyc_List_List* _temp1181; struct _tuple1* _temp1183; void*
_temp1185; struct Cyc_Absyn_TunionInfo _temp1187; void* _temp1189; struct Cyc_List_List*
_temp1191; void* _temp1193; struct Cyc_Absyn_Tuniondecl* _temp1195; void*
_temp1197; struct Cyc_Absyn_TunionInfo _temp1199; void* _temp1201; struct Cyc_List_List*
_temp1203; void* _temp1205; struct Cyc_Absyn_Tuniondecl* _temp1207; void*
_temp1209; struct Cyc_Absyn_TunionFieldInfo _temp1211; struct Cyc_List_List*
_temp1213; void* _temp1215; struct Cyc_Absyn_Tunionfield* _temp1217; struct Cyc_Absyn_Tuniondecl*
_temp1219; void* _temp1221; struct Cyc_Absyn_TunionFieldInfo _temp1223; struct
Cyc_List_List* _temp1225; void* _temp1227; struct Cyc_Absyn_Tunionfield*
_temp1229; struct Cyc_Absyn_Tuniondecl* _temp1231; void* _temp1233; struct Cyc_Absyn_PtrInfo
_temp1235; struct Cyc_Absyn_Conref* _temp1237; struct Cyc_Absyn_Tqual _temp1239;
struct Cyc_Absyn_Conref* _temp1241; void* _temp1243; void* _temp1245; void*
_temp1247; struct Cyc_Absyn_PtrInfo _temp1249; struct Cyc_Absyn_Conref*
_temp1251; struct Cyc_Absyn_Tqual _temp1253; struct Cyc_Absyn_Conref* _temp1255;
void* _temp1257; void* _temp1259; void* _temp1261; void* _temp1263; void*
_temp1265; void* _temp1267; void* _temp1269; void* _temp1271; void* _temp1273;
void* _temp1275; void* _temp1277; void* _temp1279; void* _temp1281; struct Cyc_Absyn_Exp*
_temp1283; struct Cyc_Absyn_Tqual _temp1285; void* _temp1287; void* _temp1289;
struct Cyc_Absyn_Exp* _temp1291; struct Cyc_Absyn_Tqual _temp1293; void*
_temp1295; void* _temp1297; struct Cyc_Absyn_FnInfo _temp1299; struct Cyc_List_List*
_temp1301; struct Cyc_List_List* _temp1303; struct Cyc_Absyn_VarargInfo*
_temp1305; int _temp1307; struct Cyc_List_List* _temp1309; void* _temp1311;
struct Cyc_Core_Opt* _temp1313; struct Cyc_List_List* _temp1315; void* _temp1317;
struct Cyc_Absyn_FnInfo _temp1319; struct Cyc_List_List* _temp1321; struct Cyc_List_List*
_temp1323; struct Cyc_Absyn_VarargInfo* _temp1325; int _temp1327; struct Cyc_List_List*
_temp1329; void* _temp1331; struct Cyc_Core_Opt* _temp1333; struct Cyc_List_List*
_temp1335; void* _temp1337; struct Cyc_List_List* _temp1339; void* _temp1341;
struct Cyc_List_List* _temp1343; void* _temp1345; struct Cyc_List_List*
_temp1347; void* _temp1349; struct Cyc_List_List* _temp1351; void* _temp1353;
struct Cyc_List_List* _temp1355; void* _temp1357; struct Cyc_List_List*
_temp1359; void* _temp1361; void* _temp1363; void* _temp1365; void* _temp1367;
void* _temp1369; void* _temp1371; void* _temp1373; void* _temp1375; struct Cyc_List_List*
_temp1377; void* _temp1379; struct Cyc_List_List* _temp1381; void* _temp1383;
void* _temp1385; void* _temp1387; void* _temp1389; void* _temp1391; void*
_temp1393; void* _temp1395; _LL1069: _LL1120: _temp1119= _temp1067.f1; if((
unsigned int) _temp1119 > 4u?*(( int*) _temp1119) == Cyc_Absyn_Evar: 0){ _LL1128:
_temp1127=(( struct Cyc_Absyn_Evar_struct*) _temp1119)->f1; goto _LL1126;
_LL1126: _temp1125=(( struct Cyc_Absyn_Evar_struct*) _temp1119)->f2; goto
_LL1124; _LL1124: _temp1123=(( struct Cyc_Absyn_Evar_struct*) _temp1119)->f3;
goto _LL1122; _LL1122: _temp1121=(( struct Cyc_Absyn_Evar_struct*) _temp1119)->f4;
goto _LL1118;} else{ goto _LL1071;} _LL1118: _temp1117= _temp1067.f2; goto
_LL1070; _LL1071: _LL1132: _temp1131= _temp1067.f1; if( _temp1131 ==( void*) Cyc_Absyn_VoidType){
goto _LL1130;} else{ goto _LL1073;} _LL1130: _temp1129= _temp1067.f2; if(
_temp1129 ==( void*) Cyc_Absyn_VoidType){ goto _LL1072;} else{ goto _LL1073;}
_LL1073: _LL1138: _temp1137= _temp1067.f1; if(( unsigned int) _temp1137 > 4u?*((
int*) _temp1137) == Cyc_Absyn_VarType: 0){ _LL1140: _temp1139=(( struct Cyc_Absyn_VarType_struct*)
_temp1137)->f1; goto _LL1134;} else{ goto _LL1075;} _LL1134: _temp1133=
_temp1067.f2; if(( unsigned int) _temp1133 > 4u?*(( int*) _temp1133) == Cyc_Absyn_VarType:
0){ _LL1136: _temp1135=(( struct Cyc_Absyn_VarType_struct*) _temp1133)->f1; goto
_LL1074;} else{ goto _LL1075;} _LL1075: _LL1150: _temp1149= _temp1067.f1; if((
unsigned int) _temp1149 > 4u?*(( int*) _temp1149) == Cyc_Absyn_StructType: 0){
_LL1156: _temp1155=(( struct Cyc_Absyn_StructType_struct*) _temp1149)->f1; goto
_LL1154; _LL1154: _temp1153=(( struct Cyc_Absyn_StructType_struct*) _temp1149)->f2;
goto _LL1152; _LL1152: _temp1151=(( struct Cyc_Absyn_StructType_struct*)
_temp1149)->f3; goto _LL1142;} else{ goto _LL1077;} _LL1142: _temp1141=
_temp1067.f2; if(( unsigned int) _temp1141 > 4u?*(( int*) _temp1141) == Cyc_Absyn_StructType:
0){ _LL1148: _temp1147=(( struct Cyc_Absyn_StructType_struct*) _temp1141)->f1;
goto _LL1146; _LL1146: _temp1145=(( struct Cyc_Absyn_StructType_struct*)
_temp1141)->f2; goto _LL1144; _LL1144: _temp1143=(( struct Cyc_Absyn_StructType_struct*)
_temp1141)->f3; goto _LL1076;} else{ goto _LL1077;} _LL1077: _LL1164: _temp1163=
_temp1067.f1; if(( unsigned int) _temp1163 > 4u?*(( int*) _temp1163) == Cyc_Absyn_EnumType:
0){ _LL1168: _temp1167=(( struct Cyc_Absyn_EnumType_struct*) _temp1163)->f1;
goto _LL1166; _LL1166: _temp1165=(( struct Cyc_Absyn_EnumType_struct*) _temp1163)->f2;
goto _LL1158;} else{ goto _LL1079;} _LL1158: _temp1157= _temp1067.f2; if((
unsigned int) _temp1157 > 4u?*(( int*) _temp1157) == Cyc_Absyn_EnumType: 0){
_LL1162: _temp1161=(( struct Cyc_Absyn_EnumType_struct*) _temp1157)->f1; goto
_LL1160; _LL1160: _temp1159=(( struct Cyc_Absyn_EnumType_struct*) _temp1157)->f2;
goto _LL1078;} else{ goto _LL1079;} _LL1079: _LL1178: _temp1177= _temp1067.f1;
if(( unsigned int) _temp1177 > 4u?*(( int*) _temp1177) == Cyc_Absyn_UnionType: 0){
_LL1184: _temp1183=(( struct Cyc_Absyn_UnionType_struct*) _temp1177)->f1; goto
_LL1182; _LL1182: _temp1181=(( struct Cyc_Absyn_UnionType_struct*) _temp1177)->f2;
goto _LL1180; _LL1180: _temp1179=(( struct Cyc_Absyn_UnionType_struct*)
_temp1177)->f3; goto _LL1170;} else{ goto _LL1081;} _LL1170: _temp1169=
_temp1067.f2; if(( unsigned int) _temp1169 > 4u?*(( int*) _temp1169) == Cyc_Absyn_UnionType:
0){ _LL1176: _temp1175=(( struct Cyc_Absyn_UnionType_struct*) _temp1169)->f1;
goto _LL1174; _LL1174: _temp1173=(( struct Cyc_Absyn_UnionType_struct*)
_temp1169)->f2; goto _LL1172; _LL1172: _temp1171=(( struct Cyc_Absyn_UnionType_struct*)
_temp1169)->f3; goto _LL1080;} else{ goto _LL1081;} _LL1081: _LL1198: _temp1197=
_temp1067.f1; if(( unsigned int) _temp1197 > 4u?*(( int*) _temp1197) == Cyc_Absyn_TunionType:
0){ _LL1200: _temp1199=(( struct Cyc_Absyn_TunionType_struct*) _temp1197)->f1;
_LL1206: _temp1205=( void*) _temp1199.tunion_info; if(*(( int*) _temp1205) ==
Cyc_Absyn_KnownTunion){ _LL1208: _temp1207=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1205)->f1; goto _LL1204;} else{ goto _LL1083;} _LL1204: _temp1203=
_temp1199.targs; goto _LL1202; _LL1202: _temp1201=( void*) _temp1199.rgn; goto
_LL1186;} else{ goto _LL1083;} _LL1186: _temp1185= _temp1067.f2; if((
unsigned int) _temp1185 > 4u?*(( int*) _temp1185) == Cyc_Absyn_TunionType: 0){
_LL1188: _temp1187=(( struct Cyc_Absyn_TunionType_struct*) _temp1185)->f1;
_LL1194: _temp1193=( void*) _temp1187.tunion_info; if(*(( int*) _temp1193) ==
Cyc_Absyn_KnownTunion){ _LL1196: _temp1195=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1193)->f1; goto _LL1192;} else{ goto _LL1083;} _LL1192: _temp1191=
_temp1187.targs; goto _LL1190; _LL1190: _temp1189=( void*) _temp1187.rgn; goto
_LL1082;} else{ goto _LL1083;} _LL1083: _LL1222: _temp1221= _temp1067.f1; if((
unsigned int) _temp1221 > 4u?*(( int*) _temp1221) == Cyc_Absyn_TunionFieldType:
0){ _LL1224: _temp1223=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1221)->f1;
_LL1228: _temp1227=( void*) _temp1223.field_info; if(*(( int*) _temp1227) == Cyc_Absyn_KnownTunionfield){
_LL1232: _temp1231=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1227)->f1;
goto _LL1230; _LL1230: _temp1229=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1227)->f2; goto _LL1226;} else{ goto _LL1085;} _LL1226: _temp1225=
_temp1223.targs; goto _LL1210;} else{ goto _LL1085;} _LL1210: _temp1209=
_temp1067.f2; if(( unsigned int) _temp1209 > 4u?*(( int*) _temp1209) == Cyc_Absyn_TunionFieldType:
0){ _LL1212: _temp1211=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1209)->f1;
_LL1216: _temp1215=( void*) _temp1211.field_info; if(*(( int*) _temp1215) == Cyc_Absyn_KnownTunionfield){
_LL1220: _temp1219=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1215)->f1;
goto _LL1218; _LL1218: _temp1217=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1215)->f2; goto _LL1214;} else{ goto _LL1085;} _LL1214: _temp1213=
_temp1211.targs; goto _LL1084;} else{ goto _LL1085;} _LL1085: _LL1248: _temp1247=
_temp1067.f1; if(( unsigned int) _temp1247 > 4u?*(( int*) _temp1247) == Cyc_Absyn_PointerType:
0){ _LL1250: _temp1249=(( struct Cyc_Absyn_PointerType_struct*) _temp1247)->f1;
_LL1260: _temp1259=( void*) _temp1249.elt_typ; goto _LL1258; _LL1258: _temp1257=(
void*) _temp1249.rgn_typ; goto _LL1256; _LL1256: _temp1255= _temp1249.nullable;
goto _LL1254; _LL1254: _temp1253= _temp1249.tq; goto _LL1252; _LL1252: _temp1251=
_temp1249.bounds; goto _LL1234;} else{ goto _LL1087;} _LL1234: _temp1233=
_temp1067.f2; if(( unsigned int) _temp1233 > 4u?*(( int*) _temp1233) == Cyc_Absyn_PointerType:
0){ _LL1236: _temp1235=(( struct Cyc_Absyn_PointerType_struct*) _temp1233)->f1;
_LL1246: _temp1245=( void*) _temp1235.elt_typ; goto _LL1244; _LL1244: _temp1243=(
void*) _temp1235.rgn_typ; goto _LL1242; _LL1242: _temp1241= _temp1235.nullable;
goto _LL1240; _LL1240: _temp1239= _temp1235.tq; goto _LL1238; _LL1238: _temp1237=
_temp1235.bounds; goto _LL1086;} else{ goto _LL1087;} _LL1087: _LL1268:
_temp1267= _temp1067.f1; if(( unsigned int) _temp1267 > 4u?*(( int*) _temp1267)
== Cyc_Absyn_IntType: 0){ _LL1272: _temp1271=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1267)->f1; goto _LL1270; _LL1270: _temp1269=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1267)->f2; goto _LL1262;} else{ goto _LL1089;} _LL1262: _temp1261=
_temp1067.f2; if(( unsigned int) _temp1261 > 4u?*(( int*) _temp1261) == Cyc_Absyn_IntType:
0){ _LL1266: _temp1265=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1261)->f1;
goto _LL1264; _LL1264: _temp1263=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1261)->f2; goto _LL1088;} else{ goto _LL1089;} _LL1089: _LL1276: _temp1275=
_temp1067.f1; if( _temp1275 ==( void*) Cyc_Absyn_FloatType){ goto _LL1274;}
else{ goto _LL1091;} _LL1274: _temp1273= _temp1067.f2; if( _temp1273 ==( void*)
Cyc_Absyn_FloatType){ goto _LL1090;} else{ goto _LL1091;} _LL1091: _LL1280:
_temp1279= _temp1067.f1; if( _temp1279 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1278;} else{ goto _LL1093;} _LL1278: _temp1277= _temp1067.f2; if( _temp1277
==( void*) Cyc_Absyn_DoubleType){ goto _LL1092;} else{ goto _LL1093;} _LL1093:
_LL1290: _temp1289= _temp1067.f1; if(( unsigned int) _temp1289 > 4u?*(( int*)
_temp1289) == Cyc_Absyn_ArrayType: 0){ _LL1296: _temp1295=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1289)->f1; goto _LL1294; _LL1294: _temp1293=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1289)->f2; goto _LL1292; _LL1292: _temp1291=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1289)->f3; goto _LL1282;} else{ goto _LL1095;} _LL1282: _temp1281=
_temp1067.f2; if(( unsigned int) _temp1281 > 4u?*(( int*) _temp1281) == Cyc_Absyn_ArrayType:
0){ _LL1288: _temp1287=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1281)->f1;
goto _LL1286; _LL1286: _temp1285=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1281)->f2; goto _LL1284; _LL1284: _temp1283=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1281)->f3; goto _LL1094;} else{ goto _LL1095;} _LL1095: _LL1318: _temp1317=
_temp1067.f1; if(( unsigned int) _temp1317 > 4u?*(( int*) _temp1317) == Cyc_Absyn_FnType:
0){ _LL1320: _temp1319=(( struct Cyc_Absyn_FnType_struct*) _temp1317)->f1;
_LL1336: _temp1335= _temp1319.tvars; goto _LL1334; _LL1334: _temp1333= _temp1319.effect;
goto _LL1332; _LL1332: _temp1331=( void*) _temp1319.ret_typ; goto _LL1330;
_LL1330: _temp1329= _temp1319.args; goto _LL1328; _LL1328: _temp1327= _temp1319.c_varargs;
goto _LL1326; _LL1326: _temp1325= _temp1319.cyc_varargs; goto _LL1324; _LL1324:
_temp1323= _temp1319.rgn_po; goto _LL1322; _LL1322: _temp1321= _temp1319.attributes;
goto _LL1298;} else{ goto _LL1097;} _LL1298: _temp1297= _temp1067.f2; if((
unsigned int) _temp1297 > 4u?*(( int*) _temp1297) == Cyc_Absyn_FnType: 0){
_LL1300: _temp1299=(( struct Cyc_Absyn_FnType_struct*) _temp1297)->f1; _LL1316:
_temp1315= _temp1299.tvars; goto _LL1314; _LL1314: _temp1313= _temp1299.effect;
goto _LL1312; _LL1312: _temp1311=( void*) _temp1299.ret_typ; goto _LL1310;
_LL1310: _temp1309= _temp1299.args; goto _LL1308; _LL1308: _temp1307= _temp1299.c_varargs;
goto _LL1306; _LL1306: _temp1305= _temp1299.cyc_varargs; goto _LL1304; _LL1304:
_temp1303= _temp1299.rgn_po; goto _LL1302; _LL1302: _temp1301= _temp1299.attributes;
goto _LL1096;} else{ goto _LL1097;} _LL1097: _LL1342: _temp1341= _temp1067.f1;
if(( unsigned int) _temp1341 > 4u?*(( int*) _temp1341) == Cyc_Absyn_TupleType: 0){
_LL1344: _temp1343=(( struct Cyc_Absyn_TupleType_struct*) _temp1341)->f1; goto
_LL1338;} else{ goto _LL1099;} _LL1338: _temp1337= _temp1067.f2; if((
unsigned int) _temp1337 > 4u?*(( int*) _temp1337) == Cyc_Absyn_TupleType: 0){
_LL1340: _temp1339=(( struct Cyc_Absyn_TupleType_struct*) _temp1337)->f1; goto
_LL1098;} else{ goto _LL1099;} _LL1099: _LL1350: _temp1349= _temp1067.f1; if((
unsigned int) _temp1349 > 4u?*(( int*) _temp1349) == Cyc_Absyn_AnonStructType: 0){
_LL1352: _temp1351=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1349)->f1;
goto _LL1346;} else{ goto _LL1101;} _LL1346: _temp1345= _temp1067.f2; if((
unsigned int) _temp1345 > 4u?*(( int*) _temp1345) == Cyc_Absyn_AnonStructType: 0){
_LL1348: _temp1347=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1345)->f1;
goto _LL1100;} else{ goto _LL1101;} _LL1101: _LL1358: _temp1357= _temp1067.f1;
if(( unsigned int) _temp1357 > 4u?*(( int*) _temp1357) == Cyc_Absyn_AnonUnionType:
0){ _LL1360: _temp1359=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1357)->f1;
goto _LL1354;} else{ goto _LL1103;} _LL1354: _temp1353= _temp1067.f2; if((
unsigned int) _temp1353 > 4u?*(( int*) _temp1353) == Cyc_Absyn_AnonUnionType: 0){
_LL1356: _temp1355=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1353)->f1;
goto _LL1102;} else{ goto _LL1103;} _LL1103: _LL1364: _temp1363= _temp1067.f1;
if( _temp1363 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1362;} else{ goto _LL1105;}
_LL1362: _temp1361= _temp1067.f2; if( _temp1361 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL1104;} else{ goto _LL1105;} _LL1105: _LL1370: _temp1369= _temp1067.f1;
if(( unsigned int) _temp1369 > 4u?*(( int*) _temp1369) == Cyc_Absyn_RgnHandleType:
0){ _LL1372: _temp1371=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1369)->f1; goto _LL1366;} else{ goto _LL1107;} _LL1366: _temp1365=
_temp1067.f2; if(( unsigned int) _temp1365 > 4u?*(( int*) _temp1365) == Cyc_Absyn_RgnHandleType:
0){ _LL1368: _temp1367=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1365)->f1; goto _LL1106;} else{ goto _LL1107;} _LL1107: _LL1376: _temp1375=
_temp1067.f1; if(( unsigned int) _temp1375 > 4u?*(( int*) _temp1375) == Cyc_Absyn_JoinEff:
0){ _LL1378: _temp1377=(( struct Cyc_Absyn_JoinEff_struct*) _temp1375)->f1; goto
_LL1374;} else{ goto _LL1109;} _LL1374: _temp1373= _temp1067.f2; goto _LL1108;
_LL1109: _LL1384: _temp1383= _temp1067.f1; goto _LL1380; _LL1380: _temp1379=
_temp1067.f2; if(( unsigned int) _temp1379 > 4u?*(( int*) _temp1379) == Cyc_Absyn_JoinEff:
0){ _LL1382: _temp1381=(( struct Cyc_Absyn_JoinEff_struct*) _temp1379)->f1; goto
_LL1110;} else{ goto _LL1111;} _LL1111: _LL1388: _temp1387= _temp1067.f1; if((
unsigned int) _temp1387 > 4u?*(( int*) _temp1387) == Cyc_Absyn_AccessEff: 0){
_LL1390: _temp1389=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1387)->f1;
goto _LL1386;} else{ goto _LL1113;} _LL1386: _temp1385= _temp1067.f2; goto
_LL1112; _LL1113: _LL1396: _temp1395= _temp1067.f1; goto _LL1392; _LL1392:
_temp1391= _temp1067.f2; if(( unsigned int) _temp1391 > 4u?*(( int*) _temp1391)
== Cyc_Absyn_AccessEff: 0){ _LL1394: _temp1393=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1391)->f1; goto _LL1114;} else{ goto _LL1115;} _LL1115: goto _LL1116;
_LL1070: Cyc_Tcutil_unify_it( t2, t1); return; _LL1072: return; _LL1074: {
struct _tagged_arr* _temp1397= _temp1139->name; goto _LL1398; _LL1398: { struct
_tagged_arr* _temp1399= _temp1135->name; goto _LL1400; _LL1400: { int _temp1401=*((
int*) _check_null( _temp1139->identity)); goto _LL1402; _LL1402: { int _temp1403=*((
int*) _check_null( _temp1135->identity)); goto _LL1404; _LL1404: { void*
_temp1405=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1139->kind); goto _LL1406; _LL1406: { void* _temp1407=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1135->kind); goto _LL1408;
_LL1408: if( _temp1403 == _temp1401? Cyc_String_zstrptrcmp( _temp1397, _temp1399)
== 0: 0){ if( _temp1405 != _temp1407){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1409=* _temp1397; struct
_tagged_arr _temp1410= Cyc_Absynpp_kind2string( _temp1405); struct _tagged_arr
_temp1411= Cyc_Absynpp_kind2string( _temp1407); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1409.last_plus_one - _temp1409.curr, _temp1409.curr, _temp1410.last_plus_one
- _temp1410.curr, _temp1410.curr, _temp1411.last_plus_one - _temp1411.curr,
_temp1411.curr);}));} return;} goto _LL1068;}}}}}} _LL1076: if((( _temp1147 != 0?
_temp1155 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp1147),(
struct _tuple1*) _check_null( _temp1155)) == 0: 0)? 1:( _temp1147 == 0?
_temp1155 == 0: 0)){ Cyc_Tcutil_unify_list( _temp1145, _temp1153); return;} goto
_LL1068; _LL1078: if( Cyc_Absyn_qvar_cmp( _temp1167, _temp1161) == 0){ return;}
goto _LL1068; _LL1080: if((( _temp1175 != 0? _temp1183 != 0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1175),( struct _tuple1*) _check_null(
_temp1183)) == 0: 0)? 1:( _temp1175 == 0? _temp1183 == 0: 0)){ Cyc_Tcutil_unify_list(
_temp1173, _temp1181); return;} goto _LL1068; _LL1082: if( _temp1207 ==
_temp1195? 1: Cyc_Absyn_qvar_cmp( _temp1207->name, _temp1195->name) == 0){ Cyc_Tcutil_unify_it(
_temp1189, _temp1201); Cyc_Tcutil_unify_list( _temp1191, _temp1203); return;}
goto _LL1068; _LL1084: if(( _temp1231 == _temp1219? 1: Cyc_Absyn_qvar_cmp(
_temp1231->name, _temp1219->name) == 0)? _temp1229 == _temp1217? 1: Cyc_Absyn_qvar_cmp(
_temp1229->name, _temp1217->name) == 0: 0){ Cyc_Tcutil_unify_list( _temp1213,
_temp1225); return;} goto _LL1068; _LL1086: Cyc_Tcutil_unify_it( _temp1245,
_temp1259); Cyc_Tcutil_unify_it( _temp1257, _temp1243); Cyc_Tcutil_unify_tqual(
_temp1239, _temp1253);(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp,
_temp1237, _temp1251);{ void* _temp1412=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1237))->v; void*
_temp1418; _LL1414: if(( unsigned int) _temp1412 > 1u?*(( int*) _temp1412) ==
Cyc_Absyn_Eq_constr: 0){ _LL1419: _temp1418=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1412)->f1; if( _temp1418 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1415;}
else{ goto _LL1416;}} else{ goto _LL1416;} _LL1416: goto _LL1417; _LL1415:
return; _LL1417: goto _LL1413; _LL1413:;}(( void(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp1241, _temp1255); return; _LL1088: if( _temp1265 ==
_temp1271? _temp1263 == _temp1269: 0){ return;} goto _LL1068; _LL1090: return;
_LL1092: return; _LL1094: Cyc_Tcutil_unify_it( _temp1287, _temp1295); Cyc_Tcutil_unify_tqual(
_temp1285, _temp1293); if( _temp1291 == _temp1283){ return;} if( _temp1291 == 0?
1: _temp1283 == 0){ goto _LL1068;} if( Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*) _check_null( _temp1291)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1283))){ return;} goto _LL1068; _LL1096: {
int done= 0;{ struct _RegionHandle _temp1420= _new_region(); struct
_RegionHandle* rgn=& _temp1420; _push_region( rgn);{ struct Cyc_List_List* inst=
0; while( _temp1315 != 0) { if( _temp1335 == 0){ break;} inst=({ struct Cyc_List_List*
_temp1421=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1421->hd=( void*)({ struct _tuple5* _temp1422=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1422->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp1335))->hd; _temp1422->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp1423=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1423[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1424; _temp1424.tag= Cyc_Absyn_VarType; _temp1424.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp1315))->hd;
_temp1424;}); _temp1423;}); _temp1422;}); _temp1421->tl= inst; _temp1421;});
_temp1315=(( struct Cyc_List_List*) _check_null( _temp1315))->tl; _temp1335=((
struct Cyc_List_List*) _check_null( _temp1335))->tl;} if( _temp1335 != 0){
_npop_handler( 0u); goto _LL1068;} if( inst != 0){ Cyc_Tcutil_unify_it(( void*)({
struct Cyc_Absyn_FnType_struct* _temp1425=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1425[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1426; _temp1426.tag= Cyc_Absyn_FnType; _temp1426.f1=({ struct Cyc_Absyn_FnInfo
_temp1427; _temp1427.tvars= 0; _temp1427.effect= _temp1313; _temp1427.ret_typ=(
void*) _temp1311; _temp1427.args= _temp1309; _temp1427.c_varargs= _temp1307;
_temp1427.cyc_varargs= _temp1305; _temp1427.rgn_po= _temp1303; _temp1427.attributes=
_temp1301; _temp1427;}); _temp1426;}); _temp1425;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1428=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1428[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1429; _temp1429.tag= Cyc_Absyn_FnType; _temp1429.f1=({ struct Cyc_Absyn_FnInfo
_temp1430; _temp1430.tvars= 0; _temp1430.effect= _temp1333; _temp1430.ret_typ=(
void*) _temp1331; _temp1430.args= _temp1329; _temp1430.c_varargs= _temp1327;
_temp1430.cyc_varargs= _temp1325; _temp1430.rgn_po= _temp1323; _temp1430.attributes=
_temp1321; _temp1430;}); _temp1429;}); _temp1428;}))); done= 1;}}; _pop_region(&
_temp1420);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1311, _temp1331);
for( 0; _temp1309 != 0? _temp1329 != 0: 0;( _temp1309=(( struct Cyc_List_List*)
_check_null( _temp1309))->tl, _temp1329=(( struct Cyc_List_List*) _check_null(
_temp1329))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1309))->hd)).f2,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1329))->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1309))->hd)).f3,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1329))->hd)).f3);} if( _temp1309 != 0?
1: _temp1329 != 0){ goto _LL1068;} if( _temp1307 != _temp1327){ goto _LL1068;}{
int bad_cyc_vararg= 0;{ struct _tuple11 _temp1432=({ struct _tuple11 _temp1431;
_temp1431.f1= _temp1305; _temp1431.f2= _temp1325; _temp1431;}); struct Cyc_Absyn_VarargInfo*
_temp1442; struct Cyc_Absyn_VarargInfo* _temp1444; struct Cyc_Absyn_VarargInfo*
_temp1446; struct Cyc_Absyn_VarargInfo* _temp1448; struct Cyc_Absyn_VarargInfo*
_temp1450; struct Cyc_Absyn_VarargInfo* _temp1452; struct Cyc_Absyn_VarargInfo*
_temp1454; struct Cyc_Absyn_VarargInfo _temp1456; int _temp1457; void* _temp1459;
void* _temp1461; struct Cyc_Absyn_Tqual _temp1463; struct Cyc_Core_Opt*
_temp1465; struct Cyc_Absyn_VarargInfo* _temp1467; struct Cyc_Absyn_VarargInfo
_temp1469; int _temp1470; void* _temp1472; void* _temp1474; struct Cyc_Absyn_Tqual
_temp1476; struct Cyc_Core_Opt* _temp1478; _LL1434: _LL1445: _temp1444=
_temp1432.f1; if( _temp1444 == 0){ goto _LL1443;} else{ goto _LL1436;} _LL1443:
_temp1442= _temp1432.f2; if( _temp1442 == 0){ goto _LL1435;} else{ goto _LL1436;}
_LL1436: _LL1449: _temp1448= _temp1432.f1; if( _temp1448 == 0){ goto _LL1447;}
else{ goto _LL1438;} _LL1447: _temp1446= _temp1432.f2; goto _LL1437; _LL1438:
_LL1453: _temp1452= _temp1432.f1; goto _LL1451; _LL1451: _temp1450= _temp1432.f2;
if( _temp1450 == 0){ goto _LL1439;} else{ goto _LL1440;} _LL1440: _LL1468:
_temp1467= _temp1432.f1; if( _temp1467 == 0){ goto _LL1433;} else{ _temp1469=*
_temp1467; _LL1479: _temp1478= _temp1469.name; goto _LL1477; _LL1477: _temp1476=
_temp1469.tq; goto _LL1475; _LL1475: _temp1474=( void*) _temp1469.type; goto
_LL1473; _LL1473: _temp1472=( void*) _temp1469.rgn; goto _LL1471; _LL1471:
_temp1470= _temp1469.inject; goto _LL1455;} _LL1455: _temp1454= _temp1432.f2;
if( _temp1454 == 0){ goto _LL1433;} else{ _temp1456=* _temp1454; _LL1466:
_temp1465= _temp1456.name; goto _LL1464; _LL1464: _temp1463= _temp1456.tq; goto
_LL1462; _LL1462: _temp1461=( void*) _temp1456.type; goto _LL1460; _LL1460:
_temp1459=( void*) _temp1456.rgn; goto _LL1458; _LL1458: _temp1457= _temp1456.inject;
goto _LL1441;} _LL1435: goto _LL1433; _LL1437: goto _LL1439; _LL1439:
bad_cyc_vararg= 1; goto _LL1433; _LL1441: Cyc_Tcutil_unify_tqual( _temp1476,
_temp1463); Cyc_Tcutil_unify_it( _temp1474, _temp1461); Cyc_Tcutil_unify_it(
_temp1472, _temp1459); if( _temp1470 != _temp1457){ bad_cyc_vararg= 1;} goto
_LL1433; _LL1433:;} if( bad_cyc_vararg){ goto _LL1068;}{ int bad_effect= 0;{
struct _tuple12 _temp1481=({ struct _tuple12 _temp1480; _temp1480.f1= _temp1313;
_temp1480.f2= _temp1333; _temp1480;}); struct Cyc_Core_Opt* _temp1491; struct
Cyc_Core_Opt* _temp1493; struct Cyc_Core_Opt* _temp1495; struct Cyc_Core_Opt*
_temp1497; struct Cyc_Core_Opt* _temp1499; struct Cyc_Core_Opt* _temp1501;
_LL1483: _LL1494: _temp1493= _temp1481.f1; if( _temp1493 == 0){ goto _LL1492;}
else{ goto _LL1485;} _LL1492: _temp1491= _temp1481.f2; if( _temp1491 == 0){ goto
_LL1484;} else{ goto _LL1485;} _LL1485: _LL1498: _temp1497= _temp1481.f1; if(
_temp1497 == 0){ goto _LL1496;} else{ goto _LL1487;} _LL1496: _temp1495=
_temp1481.f2; goto _LL1486; _LL1487: _LL1502: _temp1501= _temp1481.f1; goto
_LL1500; _LL1500: _temp1499= _temp1481.f2; if( _temp1499 == 0){ goto _LL1488;}
else{ goto _LL1489;} _LL1489: goto _LL1490; _LL1484: goto _LL1482; _LL1486: goto
_LL1488; _LL1488: bad_effect= 1; goto _LL1482; _LL1490: bad_effect= ! Cyc_Tcutil_unify_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1313))->v,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1333))->v); goto _LL1482; _LL1482:;} if( bad_effect){ goto
_LL1068;} if( ! Cyc_Tcutil_same_atts( _temp1321, _temp1301)){ goto _LL1068;} if(
! Cyc_Tcutil_same_rgn_po( _temp1323, _temp1303)){ goto _LL1068;} return;}}}
_LL1098: for( 0; _temp1339 != 0? _temp1343 != 0: 0;( _temp1339=(( struct Cyc_List_List*)
_check_null( _temp1339))->tl, _temp1343=(( struct Cyc_List_List*) _check_null(
_temp1343))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1339))->hd)).f1,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1343))->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1339))->hd)).f2,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1343))->hd)).f2);} if( _temp1339 == 0?
_temp1343 == 0: 0){ return;} goto _LL1068; _LL1100: _temp1359= _temp1351;
_temp1355= _temp1347; goto _LL1102; _LL1102: for( 0; _temp1355 != 0? _temp1359
!= 0: 0;( _temp1355=(( struct Cyc_List_List*) _check_null( _temp1355))->tl,
_temp1359=(( struct Cyc_List_List*) _check_null( _temp1359))->tl)){ struct Cyc_Absyn_Structfield*
_temp1503=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1355))->hd; goto _LL1504; _LL1504: { struct Cyc_Absyn_Structfield*
_temp1505=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1359))->hd; goto _LL1506; _LL1506: if( Cyc_String_zstrptrcmp( _temp1503->name,
_temp1505->name) != 0){( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1503->tq, _temp1505->tq); Cyc_Tcutil_unify_it(( void*) _temp1503->type,(
void*) _temp1505->type); if((( ! Cyc_Tcutil_same_atts( _temp1503->attributes,
_temp1505->attributes)? 1:( _temp1503->width != 0? _temp1505->width == 0: 0))? 1:(
_temp1505->width != 0? _temp1503->width == 0: 0))? 1:(( _temp1503->width != 0?
_temp1505->width != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp1503->width))->v) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1505->width))->v):
0)){( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if( _temp1355 == 0? _temp1359
== 0: 0){ return;} goto _LL1068; _LL1104: return; _LL1106: Cyc_Tcutil_unify_it(
_temp1371, _temp1367); return; _LL1108: goto _LL1110; _LL1110: goto _LL1112;
_LL1112: goto _LL1114; _LL1114: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;}
else{ goto _LL1068;} _LL1116: goto _LL1068; _LL1068:;}( int) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1507= Cyc_Tcutil_compress(
t); void* _temp1519; void* _temp1521; struct Cyc_Absyn_Enumdecl* _temp1523;
struct _tuple1* _temp1525; _LL1509: if(( unsigned int) _temp1507 > 4u?*(( int*)
_temp1507) == Cyc_Absyn_IntType: 0){ _LL1522: _temp1521=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1507)->f1; goto _LL1520; _LL1520: _temp1519=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1507)->f2; goto _LL1510;} else{ goto _LL1511;} _LL1511: if( _temp1507 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1512;} else{ goto _LL1513;} _LL1513: if(
_temp1507 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1514;} else{ goto _LL1515;}
_LL1515: if(( unsigned int) _temp1507 > 4u?*(( int*) _temp1507) == Cyc_Absyn_EnumType:
0){ _LL1526: _temp1525=(( struct Cyc_Absyn_EnumType_struct*) _temp1507)->f1;
goto _LL1524; _LL1524: _temp1523=(( struct Cyc_Absyn_EnumType_struct*) _temp1507)->f2;
goto _LL1516;} else{ goto _LL1517;} _LL1517: goto _LL1518; _LL1510: return 1;
_LL1512: return 1; _LL1514: return 1; _LL1516: return 1; _LL1518: return 0;
_LL1508:;} int Cyc_Tcutil_will_lose_precision( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp1528=({ struct _tuple8
_temp1527; _temp1527.f1= t1; _temp1527.f2= t2; _temp1527;}); void* _temp1550;
void* _temp1552; void* _temp1554; void* _temp1556; void* _temp1558; void*
_temp1560; void* _temp1562; void* _temp1564; void* _temp1566; void* _temp1568;
void* _temp1570; void* _temp1572; void* _temp1574; void* _temp1576; void*
_temp1578; void* _temp1580; void* _temp1582; void* _temp1584; void* _temp1586;
void* _temp1588; void* _temp1590; void* _temp1592; void* _temp1594; void*
_temp1596; void* _temp1598; void* _temp1600; void* _temp1602; void* _temp1604;
void* _temp1606; void* _temp1608; void* _temp1610; void* _temp1612; void*
_temp1614; void* _temp1616; void* _temp1618; void* _temp1620; void* _temp1622;
void* _temp1624; void* _temp1626; void* _temp1628; void* _temp1630; void*
_temp1632; _LL1530: _LL1553: _temp1552= _temp1528.f1; if( _temp1552 ==( void*)
Cyc_Absyn_DoubleType){ goto _LL1551;} else{ goto _LL1532;} _LL1551: _temp1550=
_temp1528.f2; if( _temp1550 ==( void*) Cyc_Absyn_FloatType){ goto _LL1531;}
else{ goto _LL1532;} _LL1532: _LL1561: _temp1560= _temp1528.f1; if( _temp1560 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL1555;} else{ goto _LL1534;} _LL1555:
_temp1554= _temp1528.f2; if(( unsigned int) _temp1554 > 4u?*(( int*) _temp1554)
== Cyc_Absyn_IntType: 0){ _LL1559: _temp1558=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1554)->f1; goto _LL1557; _LL1557: _temp1556=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1554)->f2; goto _LL1533;} else{ goto _LL1534;} _LL1534: _LL1569: _temp1568=
_temp1528.f1; if( _temp1568 ==( void*) Cyc_Absyn_FloatType){ goto _LL1563;}
else{ goto _LL1536;} _LL1563: _temp1562= _temp1528.f2; if(( unsigned int)
_temp1562 > 4u?*(( int*) _temp1562) == Cyc_Absyn_IntType: 0){ _LL1567: _temp1566=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1562)->f1; goto _LL1565; _LL1565:
_temp1564=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1562)->f2; goto
_LL1535;} else{ goto _LL1536;} _LL1536: _LL1577: _temp1576= _temp1528.f1; if((
unsigned int) _temp1576 > 4u?*(( int*) _temp1576) == Cyc_Absyn_IntType: 0){
_LL1581: _temp1580=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1576)->f1;
goto _LL1579; _LL1579: _temp1578=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1576)->f2; if( _temp1578 ==( void*) Cyc_Absyn_B8){ goto _LL1571;} else{
goto _LL1538;}} else{ goto _LL1538;} _LL1571: _temp1570= _temp1528.f2; if((
unsigned int) _temp1570 > 4u?*(( int*) _temp1570) == Cyc_Absyn_IntType: 0){
_LL1575: _temp1574=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1570)->f1;
goto _LL1573; _LL1573: _temp1572=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1570)->f2; if( _temp1572 ==( void*) Cyc_Absyn_B8){ goto _LL1537;} else{
goto _LL1538;}} else{ goto _LL1538;} _LL1538: _LL1585: _temp1584= _temp1528.f1;
if(( unsigned int) _temp1584 > 4u?*(( int*) _temp1584) == Cyc_Absyn_IntType: 0){
_LL1589: _temp1588=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1584)->f1;
goto _LL1587; _LL1587: _temp1586=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1584)->f2; if( _temp1586 ==( void*) Cyc_Absyn_B8){ goto _LL1583;} else{
goto _LL1540;}} else{ goto _LL1540;} _LL1583: _temp1582= _temp1528.f2; goto
_LL1539; _LL1540: _LL1593: _temp1592= _temp1528.f1; if(( unsigned int) _temp1592
> 4u?*(( int*) _temp1592) == Cyc_Absyn_IntType: 0){ _LL1597: _temp1596=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1592)->f1; goto _LL1595; _LL1595:
_temp1594=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1592)->f2; if(
_temp1594 ==( void*) Cyc_Absyn_B4){ goto _LL1591;} else{ goto _LL1542;}} else{
goto _LL1542;} _LL1591: _temp1590= _temp1528.f2; if( _temp1590 ==( void*) Cyc_Absyn_FloatType){
goto _LL1541;} else{ goto _LL1542;} _LL1542: _LL1605: _temp1604= _temp1528.f1;
if(( unsigned int) _temp1604 > 4u?*(( int*) _temp1604) == Cyc_Absyn_IntType: 0){
_LL1609: _temp1608=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1604)->f1;
goto _LL1607; _LL1607: _temp1606=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1604)->f2; if( _temp1606 ==( void*) Cyc_Absyn_B4){ goto _LL1599;} else{
goto _LL1544;}} else{ goto _LL1544;} _LL1599: _temp1598= _temp1528.f2; if((
unsigned int) _temp1598 > 4u?*(( int*) _temp1598) == Cyc_Absyn_IntType: 0){
_LL1603: _temp1602=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1598)->f1;
goto _LL1601; _LL1601: _temp1600=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1598)->f2; if( _temp1600 ==( void*) Cyc_Absyn_B2){ goto _LL1543;} else{
goto _LL1544;}} else{ goto _LL1544;} _LL1544: _LL1617: _temp1616= _temp1528.f1;
if(( unsigned int) _temp1616 > 4u?*(( int*) _temp1616) == Cyc_Absyn_IntType: 0){
_LL1621: _temp1620=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1616)->f1;
goto _LL1619; _LL1619: _temp1618=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1616)->f2; if( _temp1618 ==( void*) Cyc_Absyn_B4){ goto _LL1611;} else{
goto _LL1546;}} else{ goto _LL1546;} _LL1611: _temp1610= _temp1528.f2; if((
unsigned int) _temp1610 > 4u?*(( int*) _temp1610) == Cyc_Absyn_IntType: 0){
_LL1615: _temp1614=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1610)->f1;
goto _LL1613; _LL1613: _temp1612=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1610)->f2; if( _temp1612 ==( void*) Cyc_Absyn_B1){ goto _LL1545;} else{
goto _LL1546;}} else{ goto _LL1546;} _LL1546: _LL1629: _temp1628= _temp1528.f1;
if(( unsigned int) _temp1628 > 4u?*(( int*) _temp1628) == Cyc_Absyn_IntType: 0){
_LL1633: _temp1632=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1628)->f1;
goto _LL1631; _LL1631: _temp1630=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1628)->f2; if( _temp1630 ==( void*) Cyc_Absyn_B2){ goto _LL1623;} else{
goto _LL1548;}} else{ goto _LL1548;} _LL1623: _temp1622= _temp1528.f2; if((
unsigned int) _temp1622 > 4u?*(( int*) _temp1622) == Cyc_Absyn_IntType: 0){
_LL1627: _temp1626=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1622)->f1;
goto _LL1625; _LL1625: _temp1624=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1622)->f2; if( _temp1624 ==( void*) Cyc_Absyn_B1){ goto _LL1547;} else{
goto _LL1548;}} else{ goto _LL1548;} _LL1548: goto _LL1549; _LL1531: return 1;
_LL1533: return 1; _LL1535: return 1; _LL1537: return 0; _LL1539: return 1;
_LL1541: return 1; _LL1543: return 1; _LL1545: return 1; _LL1547: return 1;
_LL1549: return 0; _LL1529:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1634=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1634->v=( void*) t1; _temp1634;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
el))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr _temp1635= Cyc_Absynpp_typ2string(
t); struct _tagged_arr _temp1636= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
xprintf("type mismatch: expecting %.*s but found %.*s", _temp1635.last_plus_one
- _temp1635.curr, _temp1635.curr, _temp1636.last_plus_one - _temp1636.curr,
_temp1636.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1637= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1643; _LL1639: if((
unsigned int) _temp1637 > 4u?*(( int*) _temp1637) == Cyc_Absyn_PointerType: 0){
_LL1644: _temp1643=(( struct Cyc_Absyn_PointerType_struct*) _temp1637)->f1; goto
_LL1640;} else{ goto _LL1641;} _LL1641: goto _LL1642; _LL1640: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1638; _LL1642: return 0; _LL1638:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1645= Cyc_Tcutil_compress(
t); void* _temp1653; void* _temp1655; struct Cyc_Absyn_Enumdecl* _temp1657;
struct _tuple1* _temp1659; _LL1647: if(( unsigned int) _temp1645 > 4u?*(( int*)
_temp1645) == Cyc_Absyn_IntType: 0){ _LL1656: _temp1655=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1645)->f1; goto _LL1654; _LL1654: _temp1653=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1645)->f2; goto _LL1648;} else{ goto _LL1649;} _LL1649: if(( unsigned int)
_temp1645 > 4u?*(( int*) _temp1645) == Cyc_Absyn_EnumType: 0){ _LL1660:
_temp1659=(( struct Cyc_Absyn_EnumType_struct*) _temp1645)->f1; goto _LL1658;
_LL1658: _temp1657=(( struct Cyc_Absyn_EnumType_struct*) _temp1645)->f2; goto
_LL1650;} else{ goto _LL1651;} _LL1651: goto _LL1652; _LL1648: return 1; _LL1650:
return 1; _LL1652: return 0; _LL1646:;} int Cyc_Tcutil_coerce_uint_typ( struct
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
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp1662=({ struct _tuple8
_temp1661; _temp1661.f1= t1; _temp1661.f2= t2; _temp1661;}); void* _temp1674;
struct Cyc_Absyn_PtrInfo _temp1676; void* _temp1678; struct Cyc_Absyn_PtrInfo
_temp1680; void* _temp1682; struct Cyc_Absyn_Exp* _temp1684; struct Cyc_Absyn_Tqual
_temp1686; void* _temp1688; void* _temp1690; struct Cyc_Absyn_Exp* _temp1692;
struct Cyc_Absyn_Tqual _temp1694; void* _temp1696; void* _temp1698; struct Cyc_Absyn_TunionInfo
_temp1700; void* _temp1702; struct Cyc_List_List* _temp1704; void* _temp1706;
struct Cyc_Absyn_Tuniondecl* _temp1708; void* _temp1710; struct Cyc_Absyn_TunionFieldInfo
_temp1712; struct Cyc_List_List* _temp1714; void* _temp1716; struct Cyc_Absyn_Tunionfield*
_temp1718; struct Cyc_Absyn_Tuniondecl* _temp1720; void* _temp1722; struct Cyc_Absyn_TunionInfo
_temp1724; void* _temp1726; struct Cyc_List_List* _temp1728; void* _temp1730;
struct Cyc_Absyn_Tuniondecl* _temp1732; void* _temp1734; struct Cyc_Absyn_PtrInfo
_temp1736; struct Cyc_Absyn_Conref* _temp1738; struct Cyc_Absyn_Tqual _temp1740;
struct Cyc_Absyn_Conref* _temp1742; void* _temp1744; void* _temp1746; _LL1664:
_LL1679: _temp1678= _temp1662.f1; if(( unsigned int) _temp1678 > 4u?*(( int*)
_temp1678) == Cyc_Absyn_PointerType: 0){ _LL1681: _temp1680=(( struct Cyc_Absyn_PointerType_struct*)
_temp1678)->f1; goto _LL1675;} else{ goto _LL1666;} _LL1675: _temp1674=
_temp1662.f2; if(( unsigned int) _temp1674 > 4u?*(( int*) _temp1674) == Cyc_Absyn_PointerType:
0){ _LL1677: _temp1676=(( struct Cyc_Absyn_PointerType_struct*) _temp1674)->f1;
goto _LL1665;} else{ goto _LL1666;} _LL1666: _LL1691: _temp1690= _temp1662.f1;
if(( unsigned int) _temp1690 > 4u?*(( int*) _temp1690) == Cyc_Absyn_ArrayType: 0){
_LL1697: _temp1696=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1690)->f1;
goto _LL1695; _LL1695: _temp1694=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1690)->f2; goto _LL1693; _LL1693: _temp1692=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1690)->f3; goto _LL1683;} else{ goto _LL1668;} _LL1683: _temp1682=
_temp1662.f2; if(( unsigned int) _temp1682 > 4u?*(( int*) _temp1682) == Cyc_Absyn_ArrayType:
0){ _LL1689: _temp1688=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1682)->f1;
goto _LL1687; _LL1687: _temp1686=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1682)->f2; goto _LL1685; _LL1685: _temp1684=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1682)->f3; goto _LL1667;} else{ goto _LL1668;} _LL1668: _LL1711: _temp1710=
_temp1662.f1; if(( unsigned int) _temp1710 > 4u?*(( int*) _temp1710) == Cyc_Absyn_TunionFieldType:
0){ _LL1713: _temp1712=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1710)->f1;
_LL1717: _temp1716=( void*) _temp1712.field_info; if(*(( int*) _temp1716) == Cyc_Absyn_KnownTunionfield){
_LL1721: _temp1720=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1716)->f1;
goto _LL1719; _LL1719: _temp1718=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1716)->f2; goto _LL1715;} else{ goto _LL1670;} _LL1715: _temp1714=
_temp1712.targs; goto _LL1699;} else{ goto _LL1670;} _LL1699: _temp1698=
_temp1662.f2; if(( unsigned int) _temp1698 > 4u?*(( int*) _temp1698) == Cyc_Absyn_TunionType:
0){ _LL1701: _temp1700=(( struct Cyc_Absyn_TunionType_struct*) _temp1698)->f1;
_LL1707: _temp1706=( void*) _temp1700.tunion_info; if(*(( int*) _temp1706) ==
Cyc_Absyn_KnownTunion){ _LL1709: _temp1708=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1706)->f1; goto _LL1705;} else{ goto _LL1670;} _LL1705: _temp1704=
_temp1700.targs; goto _LL1703; _LL1703: _temp1702=( void*) _temp1700.rgn; goto
_LL1669;} else{ goto _LL1670;} _LL1670: _LL1735: _temp1734= _temp1662.f1; if((
unsigned int) _temp1734 > 4u?*(( int*) _temp1734) == Cyc_Absyn_PointerType: 0){
_LL1737: _temp1736=(( struct Cyc_Absyn_PointerType_struct*) _temp1734)->f1;
_LL1747: _temp1746=( void*) _temp1736.elt_typ; goto _LL1745; _LL1745: _temp1744=(
void*) _temp1736.rgn_typ; goto _LL1743; _LL1743: _temp1742= _temp1736.nullable;
goto _LL1741; _LL1741: _temp1740= _temp1736.tq; goto _LL1739; _LL1739: _temp1738=
_temp1736.bounds; goto _LL1723;} else{ goto _LL1672;} _LL1723: _temp1722=
_temp1662.f2; if(( unsigned int) _temp1722 > 4u?*(( int*) _temp1722) == Cyc_Absyn_TunionType:
0){ _LL1725: _temp1724=(( struct Cyc_Absyn_TunionType_struct*) _temp1722)->f1;
_LL1731: _temp1730=( void*) _temp1724.tunion_info; if(*(( int*) _temp1730) ==
Cyc_Absyn_KnownTunion){ _LL1733: _temp1732=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1730)->f1; goto _LL1729;} else{ goto _LL1672;} _LL1729: _temp1728=
_temp1724.targs; goto _LL1727; _LL1727: _temp1726=( void*) _temp1724.rgn; goto
_LL1671;} else{ goto _LL1672;} _LL1672: goto _LL1673; _LL1665: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1680.nullable, _temp1676.nullable)){
void* _temp1748=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1680.nullable))->v; int _temp1754; _LL1750:
if(( unsigned int) _temp1748 > 1u?*(( int*) _temp1748) == Cyc_Absyn_Eq_constr: 0){
_LL1755: _temp1754=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1748)->f1;
goto _LL1751;} else{ goto _LL1752;} _LL1752: goto _LL1753; _LL1751: okay= !
_temp1754; goto _LL1749; _LL1753:( int) _throw((( void*(*)( struct _tagged_arr
msg)) Cyc_Tcutil_impos)( _tag_arr("silent_castable conref not eq", sizeof(
unsigned char), 30u))); _LL1749:;} if( !(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1680.bounds, _temp1676.bounds)){ struct _tuple8
_temp1757=({ struct _tuple8 _temp1756; _temp1756.f1=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1680.bounds))->v;
_temp1756.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1676.bounds))->v; _temp1756;}); void* _temp1767;
void* _temp1769; void* _temp1771; void* _temp1773; struct Cyc_Absyn_Exp*
_temp1775; void* _temp1777; void* _temp1779; struct Cyc_Absyn_Exp* _temp1781;
void* _temp1783; void* _temp1785; struct Cyc_Absyn_Exp* _temp1787; void*
_temp1789; void* _temp1791; void* _temp1793; void* _temp1795; _LL1759: _LL1772:
_temp1771= _temp1757.f1; if(( unsigned int) _temp1771 > 1u?*(( int*) _temp1771)
== Cyc_Absyn_Eq_constr: 0){ _LL1774: _temp1773=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1771)->f1; if(( unsigned int) _temp1773 > 1u?*(( int*) _temp1773) == Cyc_Absyn_Upper_b:
0){ _LL1776: _temp1775=(( struct Cyc_Absyn_Upper_b_struct*) _temp1773)->f1; goto
_LL1768;} else{ goto _LL1761;}} else{ goto _LL1761;} _LL1768: _temp1767=
_temp1757.f2; if(( unsigned int) _temp1767 > 1u?*(( int*) _temp1767) == Cyc_Absyn_Eq_constr:
0){ _LL1770: _temp1769=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1767)->f1;
if( _temp1769 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1760;} else{ goto _LL1761;}}
else{ goto _LL1761;} _LL1761: _LL1784: _temp1783= _temp1757.f1; if((
unsigned int) _temp1783 > 1u?*(( int*) _temp1783) == Cyc_Absyn_Eq_constr: 0){
_LL1786: _temp1785=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1783)->f1;
if(( unsigned int) _temp1785 > 1u?*(( int*) _temp1785) == Cyc_Absyn_Upper_b: 0){
_LL1788: _temp1787=(( struct Cyc_Absyn_Upper_b_struct*) _temp1785)->f1; goto
_LL1778;} else{ goto _LL1763;}} else{ goto _LL1763;} _LL1778: _temp1777=
_temp1757.f2; if(( unsigned int) _temp1777 > 1u?*(( int*) _temp1777) == Cyc_Absyn_Eq_constr:
0){ _LL1780: _temp1779=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1777)->f1;
if(( unsigned int) _temp1779 > 1u?*(( int*) _temp1779) == Cyc_Absyn_Upper_b: 0){
_LL1782: _temp1781=(( struct Cyc_Absyn_Upper_b_struct*) _temp1779)->f1; goto
_LL1762;} else{ goto _LL1763;}} else{ goto _LL1763;} _LL1763: _LL1794: _temp1793=
_temp1757.f1; if(( unsigned int) _temp1793 > 1u?*(( int*) _temp1793) == Cyc_Absyn_Eq_constr:
0){ _LL1796: _temp1795=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1793)->f1;
if( _temp1795 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1790;} else{ goto _LL1765;}}
else{ goto _LL1765;} _LL1790: _temp1789= _temp1757.f2; if(( unsigned int)
_temp1789 > 1u?*(( int*) _temp1789) == Cyc_Absyn_Eq_constr: 0){ _LL1792:
_temp1791=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1789)->f1; if(
_temp1791 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1764;} else{ goto _LL1765;}}
else{ goto _LL1765;} _LL1765: goto _LL1766; _LL1760: okay= 1; goto _LL1758;
_LL1762: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1787) >= Cyc_Evexp_eval_const_uint_exp(
_temp1781): 0; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u)); goto _LL1758; _LL1764: okay= 1; goto _LL1758;
_LL1766: okay= 0; goto _LL1758; _LL1758:;} okay= okay? Cyc_Tcutil_unify(( void*)
_temp1680.elt_typ,( void*) _temp1676.elt_typ): 0; okay= okay? Cyc_Tcutil_unify((
void*) _temp1680.rgn_typ,( void*) _temp1676.rgn_typ): 0; okay= okay? !(
_temp1680.tq).q_const? 1:( _temp1676.tq).q_const: 0; return okay;} _LL1667: {
int okay; okay=( _temp1692 != 0? _temp1684 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1692)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1684)): 0; return( okay? Cyc_Tcutil_unify(
_temp1696, _temp1688): 0)? ! _temp1694.q_const? 1: _temp1686.q_const: 0;}
_LL1669: if(( _temp1720 == _temp1708? 1: Cyc_Absyn_qvar_cmp( _temp1720->name,
_temp1708->name) == 0)? _temp1718->typs == 0: 0){ for( 0; _temp1714 != 0?
_temp1704 != 0: 0;( _temp1714=(( struct Cyc_List_List*) _check_null( _temp1714))->tl,
_temp1704=(( struct Cyc_List_List*) _check_null( _temp1704))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1714))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1704))->hd)){ break;}} if( _temp1714 == 0?
_temp1704 == 0: 0){ return 1;}} return 0; _LL1671:{ void* _temp1797= Cyc_Tcutil_compress(
_temp1746); struct Cyc_Absyn_TunionFieldInfo _temp1803; struct Cyc_List_List*
_temp1805; void* _temp1807; struct Cyc_Absyn_Tunionfield* _temp1809; struct Cyc_Absyn_Tuniondecl*
_temp1811; _LL1799: if(( unsigned int) _temp1797 > 4u?*(( int*) _temp1797) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1804: _temp1803=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1797)->f1; _LL1808: _temp1807=( void*) _temp1803.field_info; if(*(( int*)
_temp1807) == Cyc_Absyn_KnownTunionfield){ _LL1812: _temp1811=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1807)->f1; goto _LL1810; _LL1810: _temp1809=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1807)->f2; goto _LL1806;} else{ goto _LL1801;} _LL1806: _temp1805=
_temp1803.targs; goto _LL1800;} else{ goto _LL1801;} _LL1801: goto _LL1802;
_LL1800: if( ! Cyc_Tcutil_unify( _temp1744, _temp1726)? _temp1744 !=( void*) Cyc_Absyn_HeapRgn:
0){ return 0;} if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1742,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0))){
return 0;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1738,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_Absyn_bounds_one))){
return 0;} if( Cyc_Absyn_qvar_cmp( _temp1732->name, _temp1811->name) == 0?
_temp1809->typs != 0: 0){ int okay= 1; for( 0; _temp1805 != 0? _temp1728 != 0: 0;(
_temp1805=(( struct Cyc_List_List*) _check_null( _temp1805))->tl, _temp1728=((
struct Cyc_List_List*) _check_null( _temp1728))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1805))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1728))->hd)){ okay= 0; break;}} if(( ! okay? 1:
_temp1805 != 0)? 1: _temp1728 != 0){ return 0;} return 1;} goto _LL1798; _LL1802:
goto _LL1798; _LL1798:;} return 0; _LL1673: return Cyc_Tcutil_unify( t1, t2);
_LL1663:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u));} Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;}
else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1813= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp1814= Cyc_Absynpp_typ2string(
t2); xprintf("implicit cast from %.*s to %.*s", _temp1813.last_plus_one -
_temp1813.curr, _temp1813.curr, _temp1814.last_plus_one - _temp1814.curr,
_temp1814.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1815= old_typ; struct Cyc_Absyn_Enumdecl*
_temp1827; struct _tuple1* _temp1829; void* _temp1831; void* _temp1833; _LL1817:
if(( unsigned int) _temp1815 > 4u?*(( int*) _temp1815) == Cyc_Absyn_EnumType: 0){
_LL1830: _temp1829=(( struct Cyc_Absyn_EnumType_struct*) _temp1815)->f1; goto
_LL1828; _LL1828: _temp1827=(( struct Cyc_Absyn_EnumType_struct*) _temp1815)->f2;
goto _LL1818;} else{ goto _LL1819;} _LL1819: if(( unsigned int) _temp1815 > 4u?*((
int*) _temp1815) == Cyc_Absyn_IntType: 0){ _LL1834: _temp1833=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1815)->f1; goto _LL1832; _LL1832: _temp1831=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1815)->f2; goto _LL1820;} else{
goto _LL1821;} _LL1821: if( _temp1815 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1822;} else{ goto _LL1823;} _LL1823: if( _temp1815 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1824;} else{ goto _LL1825;} _LL1825: goto _LL1826; _LL1818: t= Cyc_Absyn_sint_t;
goto _LL1816; _LL1820:{ void* _temp1835= _temp1831; _LL1837: if( _temp1835 ==(
void*) Cyc_Absyn_B1){ goto _LL1838;} else{ goto _LL1839;} _LL1839: if( _temp1835
==( void*) Cyc_Absyn_B2){ goto _LL1840;} else{ goto _LL1841;} _LL1841: if(
_temp1835 ==( void*) Cyc_Absyn_B4){ goto _LL1842;} else{ goto _LL1843;} _LL1843:
if( _temp1835 ==( void*) Cyc_Absyn_B8){ goto _LL1844;} else{ goto _LL1836;}
_LL1838: t= Cyc_Absyn_sint_t; goto _LL1836; _LL1840: t= Cyc_Absyn_sint_t; goto
_LL1836; _LL1842: t= _temp1833 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1836; _LL1844: t= _temp1833 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1836; _LL1836:;} goto _LL1816;
_LL1822: t= Cyc_Absyn_float_t; goto _LL1816; _LL1824: t= Cyc_Absyn_double_t;
goto _LL1816; _LL1826: return 0; _LL1816:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote(
te, e); if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,
t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1845= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp1846= Cyc_Absynpp_typ2string(
t2); xprintf("implicit cast from %.*s to %.*s", _temp1845.last_plus_one -
_temp1845.curr, _temp1845.curr, _temp1846.last_plus_one - _temp1846.curr,
_temp1846.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1847= Cyc_Tcutil_compress( t); void* _temp1857; void*
_temp1859; _LL1849: if(( unsigned int) _temp1847 > 4u?*(( int*) _temp1847) ==
Cyc_Absyn_IntType: 0){ _LL1860: _temp1859=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1847)->f1; goto _LL1858; _LL1858: _temp1857=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1847)->f2; goto _LL1850;} else{ goto _LL1851;} _LL1851: if( _temp1847 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1852;} else{ goto _LL1853;} _LL1853: if(
_temp1847 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1854;} else{ goto _LL1855;}
_LL1855: goto _LL1856; _LL1850: return 1; _LL1852: return 1; _LL1854: return 1;
_LL1856: return 0; _LL1848:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp1861=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1861->f1= x->tq; _temp1861->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp1861;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp1862= t1; struct Cyc_List_List*
_temp1874; struct Cyc_Absyn_Structdecl** _temp1876; struct Cyc_List_List*
_temp1878; struct _tuple1* _temp1880; struct Cyc_List_List* _temp1882; _LL1864:
if( _temp1862 ==( void*) Cyc_Absyn_VoidType){ goto _LL1865;} else{ goto _LL1866;}
_LL1866: if(( unsigned int) _temp1862 > 4u?*(( int*) _temp1862) == Cyc_Absyn_TupleType:
0){ _LL1875: _temp1874=(( struct Cyc_Absyn_TupleType_struct*) _temp1862)->f1;
goto _LL1867;} else{ goto _LL1868;} _LL1868: if(( unsigned int) _temp1862 > 4u?*((
int*) _temp1862) == Cyc_Absyn_StructType: 0){ _LL1881: _temp1880=(( struct Cyc_Absyn_StructType_struct*)
_temp1862)->f1; goto _LL1879; _LL1879: _temp1878=(( struct Cyc_Absyn_StructType_struct*)
_temp1862)->f2; goto _LL1877; _LL1877: _temp1876=(( struct Cyc_Absyn_StructType_struct*)
_temp1862)->f3; goto _LL1869;} else{ goto _LL1870;} _LL1870: if(( unsigned int)
_temp1862 > 4u?*(( int*) _temp1862) == Cyc_Absyn_AnonStructType: 0){ _LL1883:
_temp1882=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1862)->f1; goto
_LL1871;} else{ goto _LL1872;} _LL1872: goto _LL1873; _LL1865: return 0; _LL1867:
return _temp1874; _LL1869: if( _temp1876 == 0? 1: _temp1880 == 0){ return({
struct Cyc_List_List* _temp1884=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1884->hd=( void*)({ struct _tuple4* _temp1885=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1885->f1= Cyc_Absyn_empty_tqual();
_temp1885->f2= t1; _temp1885;}); _temp1884->tl= 0; _temp1884;});}{ struct Cyc_Absyn_Structdecl*
_temp1886=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1876)); goto
_LL1887; _LL1887: { struct _tuple1 _temp1888=*(( struct _tuple1*) _check_null(
_temp1880)); goto _LL1889; _LL1889: { struct Cyc_List_List* _temp1890=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1886->tvs, _temp1878); goto _LL1891; _LL1891: if( _temp1886->fields == 0){
return({ struct Cyc_List_List* _temp1892=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1892->hd=( void*)({ struct _tuple4*
_temp1893=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1893->f1=
Cyc_Absyn_empty_tqual(); _temp1893->f2= t1; _temp1893;}); _temp1892->tl= 0;
_temp1892;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1890,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1886->fields))->v);}}}
_LL1871: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1882); _LL1873: return({
struct Cyc_List_List* _temp1894=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1894->hd=( void*)({ struct _tuple4* _temp1895=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1895->f1= Cyc_Absyn_empty_tqual();
_temp1895->f2= t1; _temp1895;}); _temp1894->tl= 0; _temp1894;}); _LL1863:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1897=({ struct _tuple8 _temp1896; _temp1896.f1= t1; _temp1896.f2=
t2; _temp1896;}); void* _temp1903; struct Cyc_Absyn_PtrInfo _temp1905; struct
Cyc_Absyn_Conref* _temp1907; struct Cyc_Absyn_Tqual _temp1909; struct Cyc_Absyn_Conref*
_temp1911; void* _temp1913; void* _temp1915; void* _temp1917; struct Cyc_Absyn_PtrInfo
_temp1919; struct Cyc_Absyn_Conref* _temp1921; struct Cyc_Absyn_Tqual _temp1923;
struct Cyc_Absyn_Conref* _temp1925; void* _temp1927; void* _temp1929; _LL1899:
_LL1918: _temp1917= _temp1897.f1; if(( unsigned int) _temp1917 > 4u?*(( int*)
_temp1917) == Cyc_Absyn_PointerType: 0){ _LL1920: _temp1919=(( struct Cyc_Absyn_PointerType_struct*)
_temp1917)->f1; _LL1930: _temp1929=( void*) _temp1919.elt_typ; goto _LL1928;
_LL1928: _temp1927=( void*) _temp1919.rgn_typ; goto _LL1926; _LL1926: _temp1925=
_temp1919.nullable; goto _LL1924; _LL1924: _temp1923= _temp1919.tq; goto _LL1922;
_LL1922: _temp1921= _temp1919.bounds; goto _LL1904;} else{ goto _LL1901;}
_LL1904: _temp1903= _temp1897.f2; if(( unsigned int) _temp1903 > 4u?*(( int*)
_temp1903) == Cyc_Absyn_PointerType: 0){ _LL1906: _temp1905=(( struct Cyc_Absyn_PointerType_struct*)
_temp1903)->f1; _LL1916: _temp1915=( void*) _temp1905.elt_typ; goto _LL1914;
_LL1914: _temp1913=( void*) _temp1905.rgn_typ; goto _LL1912; _LL1912: _temp1911=
_temp1905.nullable; goto _LL1910; _LL1910: _temp1909= _temp1905.tq; goto _LL1908;
_LL1908: _temp1907= _temp1905.bounds; goto _LL1900;} else{ goto _LL1901;}
_LL1901: goto _LL1902; _LL1900: if( _temp1923.q_const? ! _temp1909.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1925, _temp1911)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1925): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1911): 0){ return 0;}{ void* _temp1931= _temp1927; _LL1933: if( _temp1931
==( void*) Cyc_Absyn_HeapRgn){ goto _LL1934;} else{ goto _LL1935;} _LL1935: goto
_LL1936; _LL1934: goto _LL1932; _LL1936: Cyc_Tcutil_unify( _temp1927, _temp1913);
goto _LL1932; _LL1932:;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1921, _temp1907)){ struct _tuple8 _temp1938=({ struct _tuple8 _temp1937;
_temp1937.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1921); _temp1937.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1907); _temp1937;}); void* _temp1946; void* _temp1948; struct Cyc_Absyn_Exp*
_temp1950; void* _temp1952; struct Cyc_Absyn_Exp* _temp1954; void* _temp1956;
struct Cyc_Absyn_Exp* _temp1958; _LL1940: _LL1949: _temp1948= _temp1938.f1; if((
unsigned int) _temp1948 > 1u?*(( int*) _temp1948) == Cyc_Absyn_Upper_b: 0){
_LL1951: _temp1950=(( struct Cyc_Absyn_Upper_b_struct*) _temp1948)->f1; goto
_LL1947;} else{ goto _LL1942;} _LL1947: _temp1946= _temp1938.f2; if( _temp1946
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1941;} else{ goto _LL1942;} _LL1942:
_LL1957: _temp1956= _temp1938.f1; if(( unsigned int) _temp1956 > 1u?*(( int*)
_temp1956) == Cyc_Absyn_Upper_b: 0){ _LL1959: _temp1958=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1956)->f1; goto _LL1953;} else{ goto _LL1944;} _LL1953: _temp1952=
_temp1938.f2; if(( unsigned int) _temp1952 > 1u?*(( int*) _temp1952) == Cyc_Absyn_Upper_b:
0){ _LL1955: _temp1954=(( struct Cyc_Absyn_Upper_b_struct*) _temp1952)->f1; goto
_LL1943;} else{ goto _LL1944;} _LL1944: goto _LL1945; _LL1941: goto _LL1939;
_LL1943: if( Cyc_Evexp_eval_const_uint_exp( _temp1958) < Cyc_Evexp_eval_const_uint_exp(
_temp1954)){ return 0;} goto _LL1939; _LL1945: return 0; _LL1939:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1960=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1960->hd=( void*)({ struct _tuple8*
_temp1961=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1961->f1=
t1; _temp1961->f2= t2; _temp1961;}); _temp1960->tl= assume; _temp1960;}),
_temp1929, _temp1915); _LL1902: return 0; _LL1898:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2){
struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1); struct Cyc_List_List*
tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;( tqs2=(( struct Cyc_List_List*)
_check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*) _check_null( tqs1))->tl)){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1964; void* _temp1965; struct
Cyc_Absyn_Tqual _temp1967; struct _tuple4* _temp1962=( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( tqs1))->hd; _temp1964=* _temp1962; _LL1968:
_temp1967= _temp1964.f1; goto _LL1966; _LL1966: _temp1965= _temp1964.f2; goto
_LL1963; _LL1963: { struct _tuple4 _temp1971; void* _temp1972; struct Cyc_Absyn_Tqual
_temp1974; struct _tuple4* _temp1969=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs2))->hd; _temp1971=* _temp1969; _LL1975: _temp1974= _temp1971.f1;
goto _LL1973; _LL1973: _temp1972= _temp1971.f2; goto _LL1970; _LL1970: if(
_temp1974.q_const? Cyc_Tcutil_subtype( te, assume, _temp1965, _temp1972): 0){
continue;} else{ if( Cyc_Tcutil_unify( _temp1965, _temp1972)){ continue;} else{
return 0;}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1976= t2; struct Cyc_Absyn_Uniondecl** _temp1984; struct Cyc_List_List*
_temp1986; struct _tuple1* _temp1988; void* _temp1990; void* _temp1992; _LL1978:
if(( unsigned int) _temp1976 > 4u?*(( int*) _temp1976) == Cyc_Absyn_UnionType: 0){
_LL1989: _temp1988=(( struct Cyc_Absyn_UnionType_struct*) _temp1976)->f1; goto
_LL1987; _LL1987: _temp1986=(( struct Cyc_Absyn_UnionType_struct*) _temp1976)->f2;
goto _LL1985; _LL1985: _temp1984=(( struct Cyc_Absyn_UnionType_struct*)
_temp1976)->f3; goto _LL1979;} else{ goto _LL1980;} _LL1980: if(( unsigned int)
_temp1976 > 4u?*(( int*) _temp1976) == Cyc_Absyn_IntType: 0){ _LL1993: _temp1992=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1976)->f1; goto _LL1991; _LL1991:
_temp1990=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1976)->f2; if(
_temp1990 ==( void*) Cyc_Absyn_B4){ goto _LL1981;} else{ goto _LL1982;}} else{
goto _LL1982;} _LL1982: goto _LL1983; _LL1979: if( _temp1984 == 0){ goto _LL1977;}{
struct Cyc_Absyn_Uniondecl* _temp1994=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1984)); goto _LL1995; _LL1995: if( _temp1994->fields == 0){
goto _LL1977;}{ struct Cyc_List_List* _temp1996=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1994->tvs,
_temp1986); goto _LL1997; _LL1997:{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1994->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1996,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL1977;}} _LL1981: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1977; _LL1983: goto
_LL1977; _LL1977:;}{ void* _temp1998= t1; struct Cyc_Absyn_PtrInfo _temp2014;
struct Cyc_Absyn_Conref* _temp2016; struct Cyc_Absyn_Tqual _temp2018; struct Cyc_Absyn_Conref*
_temp2020; void* _temp2022; void* _temp2024; struct Cyc_Absyn_Exp* _temp2026;
struct Cyc_Absyn_Tqual _temp2028; void* _temp2030; struct Cyc_Absyn_Enumdecl*
_temp2032; struct _tuple1* _temp2034; void* _temp2036; void* _temp2038; _LL2000:
if(( unsigned int) _temp1998 > 4u?*(( int*) _temp1998) == Cyc_Absyn_PointerType:
0){ _LL2015: _temp2014=(( struct Cyc_Absyn_PointerType_struct*) _temp1998)->f1;
_LL2025: _temp2024=( void*) _temp2014.elt_typ; goto _LL2023; _LL2023: _temp2022=(
void*) _temp2014.rgn_typ; goto _LL2021; _LL2021: _temp2020= _temp2014.nullable;
goto _LL2019; _LL2019: _temp2018= _temp2014.tq; goto _LL2017; _LL2017: _temp2016=
_temp2014.bounds; goto _LL2001;} else{ goto _LL2002;} _LL2002: if(( unsigned int)
_temp1998 > 4u?*(( int*) _temp1998) == Cyc_Absyn_ArrayType: 0){ _LL2031:
_temp2030=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1998)->f1; goto
_LL2029; _LL2029: _temp2028=(( struct Cyc_Absyn_ArrayType_struct*) _temp1998)->f2;
goto _LL2027; _LL2027: _temp2026=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1998)->f3; goto _LL2003;} else{ goto _LL2004;} _LL2004: if(( unsigned int)
_temp1998 > 4u?*(( int*) _temp1998) == Cyc_Absyn_EnumType: 0){ _LL2035:
_temp2034=(( struct Cyc_Absyn_EnumType_struct*) _temp1998)->f1; goto _LL2033;
_LL2033: _temp2032=(( struct Cyc_Absyn_EnumType_struct*) _temp1998)->f2; goto
_LL2005;} else{ goto _LL2006;} _LL2006: if(( unsigned int) _temp1998 > 4u?*((
int*) _temp1998) == Cyc_Absyn_IntType: 0){ _LL2039: _temp2038=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1998)->f1; goto _LL2037; _LL2037: _temp2036=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1998)->f2; goto _LL2007;} else{
goto _LL2008;} _LL2008: if( _temp1998 ==( void*) Cyc_Absyn_FloatType){ goto
_LL2009;} else{ goto _LL2010;} _LL2010: if( _temp1998 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2011;} else{ goto _LL2012;} _LL2012: goto _LL2013; _LL2001:{ void*
_temp2040= t2; struct Cyc_Absyn_PtrInfo _temp2046; struct Cyc_Absyn_Conref*
_temp2048; struct Cyc_Absyn_Tqual _temp2050; struct Cyc_Absyn_Conref* _temp2052;
void* _temp2054; void* _temp2056; _LL2042: if(( unsigned int) _temp2040 > 4u?*((
int*) _temp2040) == Cyc_Absyn_PointerType: 0){ _LL2047: _temp2046=(( struct Cyc_Absyn_PointerType_struct*)
_temp2040)->f1; _LL2057: _temp2056=( void*) _temp2046.elt_typ; goto _LL2055;
_LL2055: _temp2054=( void*) _temp2046.rgn_typ; goto _LL2053; _LL2053: _temp2052=
_temp2046.nullable; goto _LL2051; _LL2051: _temp2050= _temp2046.tq; goto _LL2049;
_LL2049: _temp2048= _temp2046.bounds; goto _LL2043;} else{ goto _LL2044;}
_LL2044: goto _LL2045; _LL2043: { struct Cyc_List_List* _temp2060=({ struct Cyc_List_List*
_temp2058=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2058->hd=( void*)({ struct _tuple8* _temp2059=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp2059->f1= t1; _temp2059->f2= t2; _temp2059;});
_temp2058->tl= 0; _temp2058;}); goto _LL2061; _LL2061: { int _temp2062= Cyc_Tcutil_ptrsubtype(
te, _temp2060, _temp2024, _temp2056)? ! _temp2018.q_const? 1: _temp2050.q_const:
0; goto _LL2063; _LL2063: { int bounds_ok=(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp2016, _temp2048); if( ! bounds_ok){ struct _tuple8
_temp2065=({ struct _tuple8 _temp2064; _temp2064.f1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp2016); _temp2064.f2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp2048); _temp2064;}); void* _temp2071; struct Cyc_Absyn_Exp*
_temp2073; void* _temp2075; struct Cyc_Absyn_Exp* _temp2077; _LL2067: _LL2076:
_temp2075= _temp2065.f1; if(( unsigned int) _temp2075 > 1u?*(( int*) _temp2075)
== Cyc_Absyn_Upper_b: 0){ _LL2078: _temp2077=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2075)->f1; goto _LL2072;} else{ goto _LL2069;} _LL2072: _temp2071=
_temp2065.f2; if(( unsigned int) _temp2071 > 1u?*(( int*) _temp2071) == Cyc_Absyn_Upper_b:
0){ _LL2074: _temp2073=(( struct Cyc_Absyn_Upper_b_struct*) _temp2071)->f1; goto
_LL2068;} else{ goto _LL2069;} _LL2069: goto _LL2070; _LL2068: if( Cyc_Evexp_eval_const_uint_exp(
_temp2077) >= Cyc_Evexp_eval_const_uint_exp( _temp2073)){ bounds_ok= 1;} goto
_LL2066; _LL2070: bounds_ok= 1; goto _LL2066; _LL2066:;} return( bounds_ok?
_temp2062: 0)? Cyc_Tcutil_unify( _temp2022, _temp2054)? 1: Cyc_Tcenv_region_outlives(
te, _temp2022, _temp2054): 0;}}} _LL2045: goto _LL2041; _LL2041:;} return 0;
_LL2003:{ void* _temp2079= t2; struct Cyc_Absyn_Exp* _temp2085; struct Cyc_Absyn_Tqual
_temp2087; void* _temp2089; _LL2081: if(( unsigned int) _temp2079 > 4u?*(( int*)
_temp2079) == Cyc_Absyn_ArrayType: 0){ _LL2090: _temp2089=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2079)->f1; goto _LL2088; _LL2088: _temp2087=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2079)->f2; goto _LL2086; _LL2086: _temp2085=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2079)->f3; goto _LL2082;} else{ goto _LL2083;} _LL2083: goto _LL2084;
_LL2082: { int okay; okay=( _temp2026 != 0? _temp2085 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2026)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2085)): 0; return( okay? Cyc_Tcutil_unify(
_temp2030, _temp2089): 0)? ! _temp2028.q_const? 1: _temp2087.q_const: 0;}
_LL2084: return 0; _LL2080:;} return 0; _LL2005:{ void* _temp2091= t2; struct
Cyc_Absyn_Enumdecl* _temp2097; struct _tuple1* _temp2099; _LL2093: if((
unsigned int) _temp2091 > 4u?*(( int*) _temp2091) == Cyc_Absyn_EnumType: 0){
_LL2100: _temp2099=(( struct Cyc_Absyn_EnumType_struct*) _temp2091)->f1; goto
_LL2098; _LL2098: _temp2097=(( struct Cyc_Absyn_EnumType_struct*) _temp2091)->f2;
goto _LL2094;} else{ goto _LL2095;} _LL2095: goto _LL2096; _LL2094: if((((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp2032))->fields != 0?(( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp2097))->fields != 0: 0)?(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp2032))->fields))->v) >=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2097))->fields))->v):
0){ return 1;} goto _LL2092; _LL2096: goto _LL2092; _LL2092:;} goto _LL2007;
_LL2007: goto _LL2009; _LL2009: goto _LL2011; _LL2011: return Cyc_Tcutil_coerceable(
t2); _LL2013: return 0; _LL1999:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2101=
Cyc_Absyn_copy_exp( e); goto _LL2102; _LL2102:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Cast_e_struct* _temp2103=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2103[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2104; _temp2104.tag= Cyc_Absyn_Cast_e; _temp2104.f1=( void*) t; _temp2104.f2=
_temp2101; _temp2104;}); _temp2103;}))); e->topt=({ struct Cyc_Core_Opt*
_temp2105=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2105->v=( void*) t; _temp2105;});}} int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*
e){ void* _temp2106= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_Enumdecl* _temp2116; struct _tuple1*
_temp2118; void* _temp2120; void* _temp2122; struct Cyc_Core_Opt* _temp2124; int
_temp2126; struct Cyc_Core_Opt* _temp2128; struct Cyc_Core_Opt* _temp2130;
_LL2108: if(( unsigned int) _temp2106 > 4u?*(( int*) _temp2106) == Cyc_Absyn_EnumType:
0){ _LL2119: _temp2118=(( struct Cyc_Absyn_EnumType_struct*) _temp2106)->f1;
goto _LL2117; _LL2117: _temp2116=(( struct Cyc_Absyn_EnumType_struct*) _temp2106)->f2;
goto _LL2109;} else{ goto _LL2110;} _LL2110: if(( unsigned int) _temp2106 > 4u?*((
int*) _temp2106) == Cyc_Absyn_IntType: 0){ _LL2123: _temp2122=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2106)->f1; goto _LL2121; _LL2121: _temp2120=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2106)->f2; goto _LL2111;} else{
goto _LL2112;} _LL2112: if(( unsigned int) _temp2106 > 4u?*(( int*) _temp2106)
== Cyc_Absyn_Evar: 0){ _LL2131: _temp2130=(( struct Cyc_Absyn_Evar_struct*)
_temp2106)->f1; goto _LL2129; _LL2129: _temp2128=(( struct Cyc_Absyn_Evar_struct*)
_temp2106)->f2; goto _LL2127; _LL2127: _temp2126=(( struct Cyc_Absyn_Evar_struct*)
_temp2106)->f3; goto _LL2125; _LL2125: _temp2124=(( struct Cyc_Absyn_Evar_struct*)
_temp2106)->f4; goto _LL2113;} else{ goto _LL2114;} _LL2114: goto _LL2115;
_LL2109: goto _LL2111; _LL2111: return 1; _LL2113: if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
return 0; _LL2115: return 0; _LL2107:;} int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*
e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void* _temp2132= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2134: if(
_temp2132 ==( void*) Cyc_Absyn_FloatType){ goto _LL2135;} else{ goto _LL2136;}
_LL2136: if( _temp2132 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2137;} else{
goto _LL2138;} _LL2138: goto _LL2139; _LL2135: goto _LL2137; _LL2137: return 1;
_LL2139: return 0; _LL2133:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp2140= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2146; _LL2142:
if(( unsigned int) _temp2140 > 4u?*(( int*) _temp2140) == Cyc_Absyn_FnType: 0){
_LL2147: _temp2146=(( struct Cyc_Absyn_FnType_struct*) _temp2140)->f1; goto
_LL2143;} else{ goto _LL2144;} _LL2144: goto _LL2145; _LL2143: return 1; _LL2145:
return 0; _LL2141:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){
struct _tuple8 _temp2149=({ struct _tuple8 _temp2148; _temp2148.f1= t1;
_temp2148.f2= t2; _temp2148;}); void* _temp2173; void* _temp2175; void*
_temp2177; void* _temp2179; void* _temp2181; void* _temp2183; void* _temp2185;
void* _temp2187; void* _temp2189; void* _temp2191; void* _temp2193; void*
_temp2195; void* _temp2197; void* _temp2199; void* _temp2201; void* _temp2203;
void* _temp2205; void* _temp2207; void* _temp2209; void* _temp2211; void*
_temp2213; void* _temp2215; void* _temp2217; void* _temp2219; void* _temp2221;
void* _temp2223; void* _temp2225; void* _temp2227; void* _temp2229; void*
_temp2231; void* _temp2233; void* _temp2235; _LL2151: _LL2176: _temp2175=
_temp2149.f1; if( _temp2175 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2174;}
else{ goto _LL2153;} _LL2174: _temp2173= _temp2149.f2; goto _LL2152; _LL2153:
_LL2180: _temp2179= _temp2149.f1; goto _LL2178; _LL2178: _temp2177= _temp2149.f2;
if( _temp2177 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2154;} else{ goto
_LL2155;} _LL2155: _LL2184: _temp2183= _temp2149.f1; if( _temp2183 ==( void*)
Cyc_Absyn_FloatType){ goto _LL2182;} else{ goto _LL2157;} _LL2182: _temp2181=
_temp2149.f2; goto _LL2156; _LL2157: _LL2188: _temp2187= _temp2149.f1; goto
_LL2186; _LL2186: _temp2185= _temp2149.f2; if( _temp2185 ==( void*) Cyc_Absyn_FloatType){
goto _LL2158;} else{ goto _LL2159;} _LL2159: _LL2192: _temp2191= _temp2149.f1;
if(( unsigned int) _temp2191 > 4u?*(( int*) _temp2191) == Cyc_Absyn_IntType: 0){
_LL2196: _temp2195=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2191)->f1;
if( _temp2195 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2194;} else{ goto _LL2161;}
_LL2194: _temp2193=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2191)->f2;
if( _temp2193 ==( void*) Cyc_Absyn_B8){ goto _LL2190;} else{ goto _LL2161;}}
else{ goto _LL2161;} _LL2190: _temp2189= _temp2149.f2; goto _LL2160; _LL2161:
_LL2204: _temp2203= _temp2149.f1; goto _LL2198; _LL2198: _temp2197= _temp2149.f2;
if(( unsigned int) _temp2197 > 4u?*(( int*) _temp2197) == Cyc_Absyn_IntType: 0){
_LL2202: _temp2201=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2197)->f1;
if( _temp2201 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2200;} else{ goto _LL2163;}
_LL2200: _temp2199=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2197)->f2;
if( _temp2199 ==( void*) Cyc_Absyn_B8){ goto _LL2162;} else{ goto _LL2163;}}
else{ goto _LL2163;} _LL2163: _LL2208: _temp2207= _temp2149.f1; if((
unsigned int) _temp2207 > 4u?*(( int*) _temp2207) == Cyc_Absyn_IntType: 0){
_LL2212: _temp2211=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2207)->f1;
if( _temp2211 ==( void*) Cyc_Absyn_Signed){ goto _LL2210;} else{ goto _LL2165;}
_LL2210: _temp2209=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2207)->f2;
if( _temp2209 ==( void*) Cyc_Absyn_B8){ goto _LL2206;} else{ goto _LL2165;}}
else{ goto _LL2165;} _LL2206: _temp2205= _temp2149.f2; goto _LL2164; _LL2165:
_LL2220: _temp2219= _temp2149.f1; goto _LL2214; _LL2214: _temp2213= _temp2149.f2;
if(( unsigned int) _temp2213 > 4u?*(( int*) _temp2213) == Cyc_Absyn_IntType: 0){
_LL2218: _temp2217=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2213)->f1;
if( _temp2217 ==( void*) Cyc_Absyn_Signed){ goto _LL2216;} else{ goto _LL2167;}
_LL2216: _temp2215=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2213)->f2;
if( _temp2215 ==( void*) Cyc_Absyn_B8){ goto _LL2166;} else{ goto _LL2167;}}
else{ goto _LL2167;} _LL2167: _LL2224: _temp2223= _temp2149.f1; if((
unsigned int) _temp2223 > 4u?*(( int*) _temp2223) == Cyc_Absyn_IntType: 0){
_LL2228: _temp2227=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2223)->f1;
if( _temp2227 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2226;} else{ goto _LL2169;}
_LL2226: _temp2225=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2223)->f2;
if( _temp2225 ==( void*) Cyc_Absyn_B4){ goto _LL2222;} else{ goto _LL2169;}}
else{ goto _LL2169;} _LL2222: _temp2221= _temp2149.f2; goto _LL2168; _LL2169:
_LL2236: _temp2235= _temp2149.f1; goto _LL2230; _LL2230: _temp2229= _temp2149.f2;
if(( unsigned int) _temp2229 > 4u?*(( int*) _temp2229) == Cyc_Absyn_IntType: 0){
_LL2234: _temp2233=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2229)->f1;
if( _temp2233 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2232;} else{ goto _LL2171;}
_LL2232: _temp2231=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2229)->f2;
if( _temp2231 ==( void*) Cyc_Absyn_B4){ goto _LL2170;} else{ goto _LL2171;}}
else{ goto _LL2171;} _LL2171: goto _LL2172; _LL2152: goto _LL2154; _LL2154:
return( void*) Cyc_Absyn_DoubleType; _LL2156: goto _LL2158; _LL2158: return(
void*) Cyc_Absyn_FloatType; _LL2160: goto _LL2162; _LL2162: return Cyc_Absyn_ulong_t;
_LL2164: goto _LL2166; _LL2166: return Cyc_Absyn_slong_t; _LL2168: goto _LL2170;
_LL2170: return Cyc_Absyn_uint_t; _LL2172: return Cyc_Absyn_sint_t; _LL2150:;}
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void* _temp2237=(
void*) e->r; struct Cyc_Absyn_Exp* _temp2243; struct Cyc_Core_Opt* _temp2245;
struct Cyc_Absyn_Exp* _temp2247; _LL2239: if(*(( int*) _temp2237) == Cyc_Absyn_AssignOp_e){
_LL2248: _temp2247=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp2237)->f1; goto
_LL2246; _LL2246: _temp2245=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp2237)->f2;
if( _temp2245 == 0){ goto _LL2244;} else{ goto _LL2241;} _LL2244: _temp2243=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp2237)->f3; goto _LL2240;} else{ goto
_LL2241;} _LL2241: goto _LL2242; _LL2240: Cyc_Tcutil_warn( e->loc, _tag_arr("assignment in test",
sizeof( unsigned char), 19u)); goto _LL2238; _LL2242: goto _LL2238; _LL2238:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
c2); if( c1 == c2){ return 1;} if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2249=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2249[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2250; _temp2250.tag= Cyc_Absyn_Forward_constr; _temp2250.f1= c2; _temp2250;});
_temp2249;}))); return 1;} else{ if(( void*) c2->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2251=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2251[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2252; _temp2252.tag= Cyc_Absyn_Forward_constr; _temp2252.f1= c1; _temp2252;});
_temp2251;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2253=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2253[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2254; _temp2254.tag= Cyc_Absyn_Forward_constr;
_temp2254.f1= c1; _temp2254;}); _temp2253;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2255=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2255[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2256; _temp2256.tag= Cyc_Absyn_Forward_constr; _temp2256.f1= c2; _temp2256;});
_temp2255;}))); return 1;} else{ return 0;}}}}} static int Cyc_Tcutil_tvar_id_counter=
0; int* Cyc_Tcutil_new_tvar_id(){ return({ int* _temp2257=( int*)
GC_malloc_atomic( sizeof( int)); _temp2257[ 0]= Cyc_Tcutil_tvar_id_counter ++;
_temp2257;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct _tagged_arr s=( struct
_tagged_arr) xprintf("#%d", i); return({ struct Cyc_Absyn_Tvar* _temp2258=(
struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2258->name=({
struct _tagged_arr* _temp2259=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2259[ 0]= s; _temp2259;}); _temp2258->identity= 0;
_temp2258->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
k); _temp2258;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp2260=* t->name; goto _LL2261; _LL2261: return*(( const
unsigned char*) _check_unknown_subscript( _temp2260, sizeof( unsigned char), 0))
=='#';} void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar(
t)){ return;}{ struct _tagged_arr _temp2262= Cyc_String_strconcat( _tag_arr("`",
sizeof( unsigned char), 2u),* t->name); goto _LL2263; _LL2263:*(( unsigned char*)
_check_unknown_subscript( _temp2262, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2264=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2264[ 0]=( struct _tagged_arr) _temp2262; _temp2264;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2265=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2265->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2266=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2266->v=( void*)(*
x).f1; _temp2266;}); _temp2265->f2=(* x).f2; _temp2265->f3=(* x).f3; _temp2265;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ ==
0){ struct Cyc_List_List* _temp2267= 0; goto _LL2268; _LL2268:{ struct Cyc_List_List*
atts= fd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ if( Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd)){ _temp2267=({ struct Cyc_List_List* _temp2269=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2269->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); _temp2269->tl=
_temp2267; _temp2269;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2270=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2270[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2271; _temp2271.tag= Cyc_Absyn_FnType;
_temp2271.f1=({ struct Cyc_Absyn_FnInfo _temp2272; _temp2272.tvars= fd->tvs;
_temp2272.effect= fd->effect; _temp2272.ret_typ=( void*)(( void*) fd->ret_type);
_temp2272.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp2272.c_varargs= fd->c_varargs; _temp2272.cyc_varargs= fd->cyc_varargs;
_temp2272.rgn_po= fd->rgn_po; _temp2272.attributes= _temp2267; _temp2272;});
_temp2271;}); _temp2270;});} return( void*)(( struct Cyc_Core_Opt*) _check_null(
fd->cached_typ))->v;} struct _tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void*
f3; } ; static void* Cyc_Tcutil_fst_fdarg( struct _tuple14* t){ return(* t).f1;}
void* Cyc_Tcutil_snd_tqt( struct _tuple4* t){ return(* t).f2;} static struct
_tuple4* Cyc_Tcutil_map2_tq( struct _tuple4* pr, void* t){ return({ struct
_tuple4* _temp2273=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp2273->f1=(* pr).f1; _temp2273->f2= t; _temp2273;});} void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl*
fd){ void* t= Cyc_Tcutil_fndecl2typ( fd); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp2274= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp2280; struct Cyc_List_List* _temp2282; struct Cyc_List_List* _temp2284;
struct Cyc_Absyn_VarargInfo* _temp2286; int _temp2288; struct Cyc_List_List*
_temp2290; void* _temp2292; struct Cyc_Core_Opt* _temp2294; struct Cyc_List_List*
_temp2296; _LL2276: if(( unsigned int) _temp2274 > 4u?*(( int*) _temp2274) ==
Cyc_Absyn_FnType: 0){ _LL2281: _temp2280=(( struct Cyc_Absyn_FnType_struct*)
_temp2274)->f1; _LL2297: _temp2296= _temp2280.tvars; goto _LL2295; _LL2295:
_temp2294= _temp2280.effect; goto _LL2293; _LL2293: _temp2292=( void*) _temp2280.ret_typ;
goto _LL2291; _LL2291: _temp2290= _temp2280.args; goto _LL2289; _LL2289:
_temp2288= _temp2280.c_varargs; goto _LL2287; _LL2287: _temp2286= _temp2280.cyc_varargs;
goto _LL2285; _LL2285: _temp2284= _temp2280.rgn_po; goto _LL2283; _LL2283:
_temp2282= _temp2280.attributes; goto _LL2277;} else{ goto _LL2278;} _LL2278:
goto _LL2279; _LL2277: fd->tvs= _temp2296; fd->effect= _temp2294; goto _LL2275;
_LL2279:(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u)); return; _LL2275:;}{ struct _RegionHandle
_temp2298= _new_region(); struct _RegionHandle* r=& _temp2298; _push_region( r);
Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tagged_arr*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)(
r,( struct _tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args),
loc, _tag_arr("function declaration has repeated parameter", sizeof(
unsigned char), 44u));; _pop_region(& _temp2298);} fd->cached_typ=({ struct Cyc_Core_Opt*
_temp2299=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2299->v=( void*) t; _temp2299;});} struct _tuple15{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Tqual f2; } ; struct _tuple16{ struct _tuple15* f1; void* f2; }
; static struct _tuple16* Cyc_Tcutil_substitute_f1( struct _RegionHandle* rgn,
struct _tuple2* y){ return({ struct _tuple16* _temp2300=( struct _tuple16*)
_region_malloc( rgn, sizeof( struct _tuple16)); _temp2300->f1=({ struct _tuple15*
_temp2301=( struct _tuple15*) _region_malloc( rgn, sizeof( struct _tuple15));
_temp2301->f1=(* y).f1; _temp2301->f2=(* y).f2; _temp2301;}); _temp2300->f2=(* y).f3;
_temp2300;});} static struct _tuple2* Cyc_Tcutil_substitute_f2( struct _tuple16*
w){ void* _temp2304; struct _tuple15* _temp2306; struct _tuple16 _temp2302=* w;
_LL2307: _temp2306= _temp2302.f1; goto _LL2305; _LL2305: _temp2304= _temp2302.f2;
goto _LL2303; _LL2303: { struct Cyc_Absyn_Tqual _temp2310; struct Cyc_Core_Opt*
_temp2312; struct _tuple15 _temp2308=* _temp2306; _LL2313: _temp2312= _temp2308.f1;
goto _LL2311; _LL2311: _temp2310= _temp2308.f2; goto _LL2309; _LL2309: return({
struct _tuple2* _temp2314=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2314->f1= _temp2312; _temp2314->f2= _temp2310; _temp2314->f3= _temp2304;
_temp2314;});}} static void* Cyc_Tcutil_field_type( struct Cyc_Absyn_Structfield*
f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield* Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Structfield* f, void* t){ return({ struct Cyc_Absyn_Structfield*
_temp2315=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2315->name= f->name; _temp2315->tq= f->tq; _temp2315->type=( void*) t;
_temp2315->width= f->width; _temp2315->attributes= f->attributes; _temp2315;});}
static struct Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, struct Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle* rgn, struct Cyc_List_List* inst, void* t){ void* _temp2316=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp2362; struct Cyc_Absyn_Structdecl**
_temp2364; struct Cyc_List_List* _temp2366; struct _tuple1* _temp2368; struct
Cyc_Absyn_Uniondecl** _temp2370; struct Cyc_List_List* _temp2372; struct _tuple1*
_temp2374; struct Cyc_Absyn_TunionInfo _temp2376; void* _temp2378; struct Cyc_List_List*
_temp2380; void* _temp2382; struct Cyc_Absyn_TunionFieldInfo _temp2384; struct
Cyc_List_List* _temp2386; void* _temp2388; struct Cyc_Core_Opt* _temp2390;
struct Cyc_List_List* _temp2392; struct _tuple1* _temp2394; struct Cyc_Absyn_Exp*
_temp2396; struct Cyc_Absyn_Tqual _temp2398; void* _temp2400; struct Cyc_Absyn_PtrInfo
_temp2402; struct Cyc_Absyn_Conref* _temp2404; struct Cyc_Absyn_Tqual _temp2406;
struct Cyc_Absyn_Conref* _temp2408; void* _temp2410; void* _temp2412; struct Cyc_Absyn_FnInfo
_temp2414; struct Cyc_List_List* _temp2416; struct Cyc_List_List* _temp2418;
struct Cyc_Absyn_VarargInfo* _temp2420; int _temp2422; struct Cyc_List_List*
_temp2424; void* _temp2426; struct Cyc_Core_Opt* _temp2428; struct Cyc_List_List*
_temp2430; struct Cyc_List_List* _temp2432; struct Cyc_List_List* _temp2434;
struct Cyc_List_List* _temp2436; struct Cyc_Core_Opt* _temp2438; int _temp2440;
struct Cyc_Core_Opt* _temp2442; struct Cyc_Core_Opt* _temp2444; void* _temp2446;
struct Cyc_Absyn_Enumdecl* _temp2448; struct _tuple1* _temp2450; void* _temp2452;
void* _temp2454; void* _temp2456; struct Cyc_List_List* _temp2458; _LL2318: if((
unsigned int) _temp2316 > 4u?*(( int*) _temp2316) == Cyc_Absyn_VarType: 0){
_LL2363: _temp2362=(( struct Cyc_Absyn_VarType_struct*) _temp2316)->f1; goto
_LL2319;} else{ goto _LL2320;} _LL2320: if(( unsigned int) _temp2316 > 4u?*((
int*) _temp2316) == Cyc_Absyn_StructType: 0){ _LL2369: _temp2368=(( struct Cyc_Absyn_StructType_struct*)
_temp2316)->f1; goto _LL2367; _LL2367: _temp2366=(( struct Cyc_Absyn_StructType_struct*)
_temp2316)->f2; goto _LL2365; _LL2365: _temp2364=(( struct Cyc_Absyn_StructType_struct*)
_temp2316)->f3; goto _LL2321;} else{ goto _LL2322;} _LL2322: if(( unsigned int)
_temp2316 > 4u?*(( int*) _temp2316) == Cyc_Absyn_UnionType: 0){ _LL2375:
_temp2374=(( struct Cyc_Absyn_UnionType_struct*) _temp2316)->f1; goto _LL2373;
_LL2373: _temp2372=(( struct Cyc_Absyn_UnionType_struct*) _temp2316)->f2; goto
_LL2371; _LL2371: _temp2370=(( struct Cyc_Absyn_UnionType_struct*) _temp2316)->f3;
goto _LL2323;} else{ goto _LL2324;} _LL2324: if(( unsigned int) _temp2316 > 4u?*((
int*) _temp2316) == Cyc_Absyn_TunionType: 0){ _LL2377: _temp2376=(( struct Cyc_Absyn_TunionType_struct*)
_temp2316)->f1; _LL2383: _temp2382=( void*) _temp2376.tunion_info; goto _LL2381;
_LL2381: _temp2380= _temp2376.targs; goto _LL2379; _LL2379: _temp2378=( void*)
_temp2376.rgn; goto _LL2325;} else{ goto _LL2326;} _LL2326: if(( unsigned int)
_temp2316 > 4u?*(( int*) _temp2316) == Cyc_Absyn_TunionFieldType: 0){ _LL2385:
_temp2384=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2316)->f1; _LL2389:
_temp2388=( void*) _temp2384.field_info; goto _LL2387; _LL2387: _temp2386=
_temp2384.targs; goto _LL2327;} else{ goto _LL2328;} _LL2328: if(( unsigned int)
_temp2316 > 4u?*(( int*) _temp2316) == Cyc_Absyn_TypedefType: 0){ _LL2395:
_temp2394=(( struct Cyc_Absyn_TypedefType_struct*) _temp2316)->f1; goto _LL2393;
_LL2393: _temp2392=(( struct Cyc_Absyn_TypedefType_struct*) _temp2316)->f2; goto
_LL2391; _LL2391: _temp2390=(( struct Cyc_Absyn_TypedefType_struct*) _temp2316)->f3;
goto _LL2329;} else{ goto _LL2330;} _LL2330: if(( unsigned int) _temp2316 > 4u?*((
int*) _temp2316) == Cyc_Absyn_ArrayType: 0){ _LL2401: _temp2400=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2316)->f1; goto _LL2399; _LL2399:
_temp2398=(( struct Cyc_Absyn_ArrayType_struct*) _temp2316)->f2; goto _LL2397;
_LL2397: _temp2396=(( struct Cyc_Absyn_ArrayType_struct*) _temp2316)->f3; goto
_LL2331;} else{ goto _LL2332;} _LL2332: if(( unsigned int) _temp2316 > 4u?*((
int*) _temp2316) == Cyc_Absyn_PointerType: 0){ _LL2403: _temp2402=(( struct Cyc_Absyn_PointerType_struct*)
_temp2316)->f1; _LL2413: _temp2412=( void*) _temp2402.elt_typ; goto _LL2411;
_LL2411: _temp2410=( void*) _temp2402.rgn_typ; goto _LL2409; _LL2409: _temp2408=
_temp2402.nullable; goto _LL2407; _LL2407: _temp2406= _temp2402.tq; goto _LL2405;
_LL2405: _temp2404= _temp2402.bounds; goto _LL2333;} else{ goto _LL2334;}
_LL2334: if(( unsigned int) _temp2316 > 4u?*(( int*) _temp2316) == Cyc_Absyn_FnType:
0){ _LL2415: _temp2414=(( struct Cyc_Absyn_FnType_struct*) _temp2316)->f1;
_LL2431: _temp2430= _temp2414.tvars; goto _LL2429; _LL2429: _temp2428= _temp2414.effect;
goto _LL2427; _LL2427: _temp2426=( void*) _temp2414.ret_typ; goto _LL2425;
_LL2425: _temp2424= _temp2414.args; goto _LL2423; _LL2423: _temp2422= _temp2414.c_varargs;
goto _LL2421; _LL2421: _temp2420= _temp2414.cyc_varargs; goto _LL2419; _LL2419:
_temp2418= _temp2414.rgn_po; goto _LL2417; _LL2417: _temp2416= _temp2414.attributes;
goto _LL2335;} else{ goto _LL2336;} _LL2336: if(( unsigned int) _temp2316 > 4u?*((
int*) _temp2316) == Cyc_Absyn_TupleType: 0){ _LL2433: _temp2432=(( struct Cyc_Absyn_TupleType_struct*)
_temp2316)->f1; goto _LL2337;} else{ goto _LL2338;} _LL2338: if(( unsigned int)
_temp2316 > 4u?*(( int*) _temp2316) == Cyc_Absyn_AnonStructType: 0){ _LL2435:
_temp2434=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2316)->f1; goto
_LL2339;} else{ goto _LL2340;} _LL2340: if(( unsigned int) _temp2316 > 4u?*((
int*) _temp2316) == Cyc_Absyn_AnonUnionType: 0){ _LL2437: _temp2436=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2316)->f1; goto _LL2341;} else{ goto
_LL2342;} _LL2342: if(( unsigned int) _temp2316 > 4u?*(( int*) _temp2316) == Cyc_Absyn_Evar:
0){ _LL2445: _temp2444=(( struct Cyc_Absyn_Evar_struct*) _temp2316)->f1; goto
_LL2443; _LL2443: _temp2442=(( struct Cyc_Absyn_Evar_struct*) _temp2316)->f2;
goto _LL2441; _LL2441: _temp2440=(( struct Cyc_Absyn_Evar_struct*) _temp2316)->f3;
goto _LL2439; _LL2439: _temp2438=(( struct Cyc_Absyn_Evar_struct*) _temp2316)->f4;
goto _LL2343;} else{ goto _LL2344;} _LL2344: if(( unsigned int) _temp2316 > 4u?*((
int*) _temp2316) == Cyc_Absyn_RgnHandleType: 0){ _LL2447: _temp2446=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2316)->f1; goto _LL2345;} else{
goto _LL2346;} _LL2346: if(( unsigned int) _temp2316 > 4u?*(( int*) _temp2316)
== Cyc_Absyn_EnumType: 0){ _LL2451: _temp2450=(( struct Cyc_Absyn_EnumType_struct*)
_temp2316)->f1; goto _LL2449; _LL2449: _temp2448=(( struct Cyc_Absyn_EnumType_struct*)
_temp2316)->f2; goto _LL2347;} else{ goto _LL2348;} _LL2348: if( _temp2316 ==(
void*) Cyc_Absyn_VoidType){ goto _LL2349;} else{ goto _LL2350;} _LL2350: if((
unsigned int) _temp2316 > 4u?*(( int*) _temp2316) == Cyc_Absyn_IntType: 0){
_LL2455: _temp2454=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2316)->f1;
goto _LL2453; _LL2453: _temp2452=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2316)->f2; goto _LL2351;} else{ goto _LL2352;} _LL2352: if( _temp2316 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2353;} else{ goto _LL2354;} _LL2354: if(
_temp2316 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2355;} else{ goto _LL2356;}
_LL2356: if( _temp2316 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2357;} else{ goto
_LL2358;} _LL2358: if(( unsigned int) _temp2316 > 4u?*(( int*) _temp2316) == Cyc_Absyn_AccessEff:
0){ _LL2457: _temp2456=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2316)->f1;
goto _LL2359;} else{ goto _LL2360;} _LL2360: if(( unsigned int) _temp2316 > 4u?*((
int*) _temp2316) == Cyc_Absyn_JoinEff: 0){ _LL2459: _temp2458=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2316)->f1; goto _LL2361;} else{ goto _LL2317;} _LL2319: { struct
_handler_cons _temp2460; _push_handler(& _temp2460);{ int _temp2462= 0; if(
setjmp( _temp2460.handler)){ _temp2462= 1;} if( ! _temp2462){{ void* _temp2463=((
void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2362); _npop_handler( 0u); return _temp2463;}; _pop_handler();} else{
void* _temp2461=( void*) _exn_thrown; void* _temp2465= _temp2461; _LL2467: if(
_temp2465 == Cyc_Core_Not_found){ goto _LL2468;} else{ goto _LL2469;} _LL2469:
goto _LL2470; _LL2468: return t; _LL2470:( void) _throw( _temp2465); _LL2466:;}}}
_LL2321: { struct Cyc_List_List* _temp2471= Cyc_Tcutil_substs( rgn, inst,
_temp2366); goto _LL2472; _LL2472: return _temp2471 == _temp2366? t:( void*)({
struct Cyc_Absyn_StructType_struct* _temp2473=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2473[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2474; _temp2474.tag= Cyc_Absyn_StructType;
_temp2474.f1= _temp2368; _temp2474.f2= _temp2471; _temp2474.f3= _temp2364;
_temp2474;}); _temp2473;});} _LL2323: { struct Cyc_List_List* _temp2475= Cyc_Tcutil_substs(
rgn, inst, _temp2372); goto _LL2476; _LL2476: return _temp2475 == _temp2372? t:(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp2477=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2477[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2478; _temp2478.tag= Cyc_Absyn_UnionType;
_temp2478.f1= _temp2374; _temp2478.f2= _temp2475; _temp2478.f3= _temp2370;
_temp2478;}); _temp2477;});} _LL2325: { struct Cyc_List_List* _temp2479= Cyc_Tcutil_substs(
rgn, inst, _temp2380); goto _LL2480; _LL2480: { void* _temp2481= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2378); goto _LL2482; _LL2482: return( _temp2479 == _temp2380?
_temp2481 == _temp2378: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2483=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2483[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2484; _temp2484.tag=
Cyc_Absyn_TunionType; _temp2484.f1=({ struct Cyc_Absyn_TunionInfo _temp2485;
_temp2485.tunion_info=( void*) _temp2382; _temp2485.targs= _temp2479; _temp2485.rgn=(
void*) _temp2481; _temp2485;}); _temp2484;}); _temp2483;});}} _LL2327: { struct
Cyc_List_List* _temp2486= Cyc_Tcutil_substs( rgn, inst, _temp2386); goto _LL2487;
_LL2487: return _temp2486 == _temp2386? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2488=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2488[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2489; _temp2489.tag= Cyc_Absyn_TunionFieldType; _temp2489.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2490; _temp2490.field_info=( void*) _temp2388; _temp2490.targs= _temp2486;
_temp2490;}); _temp2489;}); _temp2488;});} _LL2329: { struct Cyc_List_List*
_temp2491= Cyc_Tcutil_substs( rgn, inst, _temp2392); goto _LL2492; _LL2492:
return _temp2491 == _temp2392? t:( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2493=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2493[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2494; _temp2494.tag=
Cyc_Absyn_TypedefType; _temp2494.f1= _temp2394; _temp2494.f2= _temp2491;
_temp2494.f3= _temp2390; _temp2494;}); _temp2493;});} _LL2331: { void* _temp2495=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2400); goto _LL2496; _LL2496: return
_temp2495 == _temp2400? t:( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp2497=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2497[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2498; _temp2498.tag= Cyc_Absyn_ArrayType;
_temp2498.f1=( void*) _temp2495; _temp2498.f2= _temp2398; _temp2498.f3=
_temp2396; _temp2498;}); _temp2497;});} _LL2333: { void* _temp2499= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2412); goto _LL2500; _LL2500: { void* _temp2501= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2410); goto _LL2502; _LL2502: if( _temp2499 == _temp2412?
_temp2501 == _temp2410: 0){ return t;} return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp2503=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp2503[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp2504; _temp2504.tag=
Cyc_Absyn_PointerType; _temp2504.f1=({ struct Cyc_Absyn_PtrInfo _temp2505;
_temp2505.elt_typ=( void*) _temp2499; _temp2505.rgn_typ=( void*) _temp2501;
_temp2505.nullable= _temp2408; _temp2505.tq= _temp2406; _temp2505.bounds=
_temp2404; _temp2505;}); _temp2504;}); _temp2503;});}} _LL2335:{ struct Cyc_List_List*
_temp2506= _temp2430; goto _LL2507; _LL2507: for( 0; _temp2506 != 0; _temp2506=((
struct Cyc_List_List*) _check_null( _temp2506))->tl){ inst=({ struct Cyc_List_List*
_temp2508=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2508->hd=( void*)({ struct _tuple5* _temp2509=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp2509->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp2506))->hd; _temp2509->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp2510=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2510[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2511; _temp2511.tag= Cyc_Absyn_VarType; _temp2511.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp2506))->hd;
_temp2511;}); _temp2510;}); _temp2509;}); _temp2508->tl= inst; _temp2508;});}}{
struct Cyc_List_List* _temp2514; struct Cyc_List_List* _temp2516; struct _tuple0
_temp2512=(( struct _tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple16*(* f)( struct _RegionHandle*, struct
_tuple2*), struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2424)); _LL2517: _temp2516= _temp2512.f1;
goto _LL2515; _LL2515: _temp2514= _temp2512.f2; goto _LL2513; _LL2513: { struct
Cyc_List_List* _temp2518= Cyc_Tcutil_substs( rgn, inst, _temp2514); goto _LL2519;
_LL2519: { struct Cyc_List_List* _temp2520=(( struct Cyc_List_List*(*)( struct
_tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp2516, _temp2518)); goto _LL2521; _LL2521: { struct Cyc_Core_Opt* eff2; if(
_temp2428 == 0){ eff2= 0;} else{ void* _temp2522= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2428))->v); goto _LL2523;
_LL2523: if( _temp2522 ==( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2428))->v){
eff2= _temp2428;} else{ eff2=({ struct Cyc_Core_Opt* _temp2524=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2524->v=( void*) _temp2522;
_temp2524;});}}{ struct Cyc_Absyn_VarargInfo* cyc_varargs2; if( _temp2420 == 0){
cyc_varargs2= 0;} else{ int _temp2527; void* _temp2529; void* _temp2531; struct
Cyc_Absyn_Tqual _temp2533; struct Cyc_Core_Opt* _temp2535; struct Cyc_Absyn_VarargInfo
_temp2525=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2420)); _LL2536:
_temp2535= _temp2525.name; goto _LL2534; _LL2534: _temp2533= _temp2525.tq; goto
_LL2532; _LL2532: _temp2531=( void*) _temp2525.type; goto _LL2530; _LL2530:
_temp2529=( void*) _temp2525.rgn; goto _LL2528; _LL2528: _temp2527= _temp2525.inject;
goto _LL2526; _LL2526: { void* _temp2537= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2531); goto _LL2538; _LL2538: { void* _temp2539= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2529); goto _LL2540; _LL2540: if( _temp2537 == _temp2531?
_temp2529 == _temp2539: 0){ cyc_varargs2= _temp2420;} else{ cyc_varargs2=({
struct Cyc_Absyn_VarargInfo* _temp2541=( struct Cyc_Absyn_VarargInfo*) GC_malloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2541->name= _temp2535; _temp2541->tq=
_temp2533; _temp2541->type=( void*) _temp2537; _temp2541->rgn=( void*) _temp2539;
_temp2541->inject= _temp2527; _temp2541;});}}}}{ struct Cyc_List_List* rgn_po2;
struct Cyc_List_List* _temp2544; struct Cyc_List_List* _temp2546; struct _tuple0
_temp2542=(( struct _tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn, rgn, _temp2418); _LL2547:
_temp2546= _temp2542.f1; goto _LL2545; _LL2545: _temp2544= _temp2542.f2; goto
_LL2543; _LL2543: { struct Cyc_List_List* _temp2548= Cyc_Tcutil_substs( rgn,
inst, _temp2546); goto _LL2549; _LL2549: { struct Cyc_List_List* _temp2550= Cyc_Tcutil_substs(
rgn, inst, _temp2544); goto _LL2551; _LL2551: if( _temp2548 == _temp2546?
_temp2550 == _temp2544: 0){ rgn_po2= _temp2418;} else{ rgn_po2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp2548,
_temp2550);} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp2552=( struct
Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2552[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2553; _temp2553.tag= Cyc_Absyn_FnType;
_temp2553.f1=({ struct Cyc_Absyn_FnInfo _temp2554; _temp2554.tvars= _temp2430;
_temp2554.effect= eff2; _temp2554.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2426); _temp2554.args= _temp2520; _temp2554.c_varargs= _temp2422;
_temp2554.cyc_varargs= cyc_varargs2; _temp2554.rgn_po= rgn_po2; _temp2554.attributes=
_temp2416; _temp2554;}); _temp2553;}); _temp2552;});}}}}}}}} _LL2337: { struct
Cyc_List_List* _temp2555=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2432); goto _LL2556; _LL2556: { struct Cyc_List_List* _temp2557= Cyc_Tcutil_substs(
rgn, inst, _temp2555); goto _LL2558; _LL2558: if( _temp2557 == _temp2555){
return t;}{ struct Cyc_List_List* _temp2559=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct _tuple4*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq, _temp2432, _temp2557); goto _LL2560;
_LL2560: return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp2561=( struct
Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2561[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2562; _temp2562.tag= Cyc_Absyn_TupleType;
_temp2562.f1= _temp2559; _temp2562;}); _temp2561;});}}} _LL2339: { struct Cyc_List_List*
_temp2563=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2434); goto _LL2564; _LL2564: { struct Cyc_List_List* _temp2565= Cyc_Tcutil_substs(
rgn, inst, _temp2563); goto _LL2566; _LL2566: if( _temp2565 == _temp2563){
return t;}{ struct Cyc_List_List* _temp2567=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type,
_temp2434, _temp2565); goto _LL2568; _LL2568: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2569=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2569[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2570; _temp2570.tag= Cyc_Absyn_AnonStructType; _temp2570.f1= _temp2567;
_temp2570;}); _temp2569;});}}} _LL2341: { struct Cyc_List_List* _temp2571=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2436);
goto _LL2572; _LL2572: { struct Cyc_List_List* _temp2573= Cyc_Tcutil_substs( rgn,
inst, _temp2571); goto _LL2574; _LL2574: if( _temp2573 == _temp2571){ return t;}{
struct Cyc_List_List* _temp2575=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2436, _temp2573); goto
_LL2576; _LL2576: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2577=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2577[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2578; _temp2578.tag= Cyc_Absyn_AnonStructType; _temp2578.f1= _temp2575;
_temp2578;}); _temp2577;});}}} _LL2343: if( _temp2442 != 0){ return Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2442))->v);} else{
return t;} _LL2345: { void* _temp2579= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2446); goto _LL2580; _LL2580: return _temp2579 == _temp2446? t:( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2581=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2581[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2582; _temp2582.tag= Cyc_Absyn_RgnHandleType;
_temp2582.f1=( void*) _temp2579; _temp2582;}); _temp2581;});} _LL2347: return t;
_LL2349: return t; _LL2351: return t; _LL2353: return t; _LL2355: return t;
_LL2357: return t; _LL2359: { void* _temp2583= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2456); goto _LL2584; _LL2584: return _temp2583 == _temp2456? t:( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2585=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2585[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2586; _temp2586.tag= Cyc_Absyn_AccessEff;
_temp2586.f1=( void*) _temp2583; _temp2586;}); _temp2585;});} _LL2361: { struct
Cyc_List_List* _temp2587= Cyc_Tcutil_substs( rgn, inst, _temp2458); goto _LL2588;
_LL2588: return _temp2587 == _temp2458? t:( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2589=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2589[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2590; _temp2590.tag= Cyc_Absyn_JoinEff;
_temp2590.f1= _temp2587; _temp2590;}); _temp2589;});} _LL2317:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2591=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; goto _LL2592; _LL2592: {
struct Cyc_List_List* _temp2593=(( struct Cyc_List_List*) _check_null( ts))->tl;
goto _LL2594; _LL2594: { void* _temp2595= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2591); goto _LL2596; _LL2596: { struct Cyc_List_List* _temp2597= Cyc_Tcutil_substs(
rgn, inst, _temp2593); goto _LL2598; _LL2598: if( _temp2591 == _temp2595?
_temp2593 == _temp2597: 0){ return ts;} return( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp2599=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2599->hd=( void*) _temp2595; _temp2599->tl= _temp2597; _temp2599;});}}}}}
extern void* Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ return
Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region, inst, t);} struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_Position_Segment*
loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2645=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2645[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2646; _temp2646.tag= Cyc_Absyn_Const_e;
_temp2646.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2646;}); _temp2645;}),
loc);{ void* _temp2600= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2614; void* _temp2616; void* _temp2618; void* _temp2620; void* _temp2622;
_LL2602: if(( unsigned int) _temp2600 > 4u?*(( int*) _temp2600) == Cyc_Absyn_PointerType:
0){ _LL2615: _temp2614=(( struct Cyc_Absyn_PointerType_struct*) _temp2600)->f1;
goto _LL2603;} else{ goto _LL2604;} _LL2604: if(( unsigned int) _temp2600 > 4u?*((
int*) _temp2600) == Cyc_Absyn_IntType: 0){ _LL2619: _temp2618=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2600)->f1; goto _LL2617; _LL2617: _temp2616=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2600)->f2; if( _temp2616 ==(
void*) Cyc_Absyn_B1){ goto _LL2605;} else{ goto _LL2606;}} else{ goto _LL2606;}
_LL2606: if(( unsigned int) _temp2600 > 4u?*(( int*) _temp2600) == Cyc_Absyn_IntType:
0){ _LL2623: _temp2622=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2600)->f1;
goto _LL2621; _LL2621: _temp2620=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2600)->f2; goto _LL2607;} else{ goto _LL2608;} _LL2608: if( _temp2600 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2609;} else{ goto _LL2610;} _LL2610: if(
_temp2600 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2611;} else{ goto _LL2612;}
_LL2612: goto _LL2613; _LL2603: goto _LL2601; _LL2605:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2624=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2624[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2625; _temp2625.tag= Cyc_Absyn_Const_e; _temp2625.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2626=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2626[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2627; _temp2627.tag= Cyc_Absyn_Char_c; _temp2627.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2627.f2='\000'; _temp2627;}); _temp2626;})); _temp2625;}); _temp2624;})));
goto _LL2601; _LL2607:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2628=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2628[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2629; _temp2629.tag= Cyc_Absyn_Const_e;
_temp2629.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2630=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2630[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2631; _temp2631.tag= Cyc_Absyn_Int_c;
_temp2631.f1=( void*) _temp2622; _temp2631.f2= 0; _temp2631;}); _temp2630;}));
_temp2629;}); _temp2628;}))); if( _temp2620 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2632=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2632[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2633; _temp2633.tag= Cyc_Absyn_Cast_e; _temp2633.f1=( void*) t; _temp2633.f2=
e; _temp2633;}); _temp2632;}), loc);} goto _LL2601; _LL2609:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2634=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2634[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2635; _temp2635.tag= Cyc_Absyn_Const_e; _temp2635.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2636=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2636[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2637; _temp2637.tag= Cyc_Absyn_Float_c; _temp2637.f1=
_tag_arr("0.0", sizeof( unsigned char), 4u); _temp2637;}); _temp2636;}));
_temp2635;}); _temp2634;}))); goto _LL2601; _LL2611:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2638=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2638[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2639; _temp2639.tag= Cyc_Absyn_Cast_e; _temp2639.f1=( void*) t; _temp2639.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2640=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2640[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2641; _temp2641.tag= Cyc_Absyn_Const_e;
_temp2641.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2642=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2642[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2643; _temp2643.tag= Cyc_Absyn_Float_c;
_temp2643.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2643;});
_temp2642;})); _temp2641;}); _temp2640;}), loc); _temp2639;}); _temp2638;})));
goto _LL2601; _LL2613: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp2644= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_temp2644.last_plus_one - _temp2644.curr, _temp2644.curr);})); goto _LL2601;
_LL2601:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_List_List*
s, struct Cyc_Absyn_Tvar* tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( tv->kind); return({ struct _tuple5* _temp2647=( struct
_tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp2647->f1= tv; _temp2647->f2=
Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2648=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2648->v=( void*) k; _temp2648;}),({
struct Cyc_Core_Opt* _temp2649=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2649->v=( void*) s; _temp2649;})); _temp2647;});} struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple6* env, struct Cyc_Absyn_Tvar*
tv){ struct _tuple6 _temp2652; struct _RegionHandle* _temp2653; struct Cyc_List_List*
_temp2655; struct _tuple6* _temp2650= env; _temp2652=* _temp2650; _LL2656:
_temp2655= _temp2652.f1; goto _LL2654; _LL2654: _temp2653= _temp2652.f2; goto
_LL2651; _LL2651: { void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
tv->kind); return({ struct _tuple5* _temp2657=( struct _tuple5*) _region_malloc(
_temp2653, sizeof( struct _tuple5)); _temp2657->f1= tv; _temp2657->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2658=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2658->v=( void*) k; _temp2658;}),({ struct Cyc_Core_Opt*
_temp2659=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2659->v=( void*) _temp2655; _temp2659;})); _temp2657;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) == 0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp2660=* tv->name;
struct _tagged_arr _temp2661= Cyc_Absynpp_ckind2string( k1); struct _tagged_arr
_temp2662= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp2660.last_plus_one - _temp2660.curr, _temp2660.curr, _temp2661.last_plus_one
- _temp2661.curr, _temp2661.curr, _temp2662.last_plus_one - _temp2662.curr,
_temp2662.curr);}));} if( tv->identity == 0){ tv->identity=(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity;} else{ if(*(( int*)
_check_null( tv->identity)) !=*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity))){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)(( struct _tagged_arr) xprintf("same type variable has different identity!"));}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2663=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2663->hd=( void*) tv; _temp2663->tl= tvs; _temp2663;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv", sizeof( unsigned char), 39u));}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*)
_check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tvs2", sizeof( unsigned char), 41u));}
if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity)) ==*(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp2664=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2664->hd=( void*) tv; _temp2664->tl=
tvs; _temp2664;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(( struct _tagged_arr)({
struct _tagged_arr _temp2665= Cyc_Tcutil_tvar2string( tv); xprintf("bound tvar id for %.*s is null",
_temp2665.last_plus_one - _temp2665.curr, _temp2665.curr);}));} return({ struct
Cyc_List_List* _temp2666=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2666->hd=( void*) tv; _temp2666->tl= tvs; _temp2666;});} static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar( struct Cyc_List_List* es, void* e){ void* _temp2667=
Cyc_Tcutil_compress( e); struct Cyc_Core_Opt* _temp2673; int _temp2675; struct
Cyc_Core_Opt* _temp2677; struct Cyc_Core_Opt* _temp2679; _LL2669: if((
unsigned int) _temp2667 > 4u?*(( int*) _temp2667) == Cyc_Absyn_Evar: 0){ _LL2680:
_temp2679=(( struct Cyc_Absyn_Evar_struct*) _temp2667)->f1; goto _LL2678;
_LL2678: _temp2677=(( struct Cyc_Absyn_Evar_struct*) _temp2667)->f2; goto
_LL2676; _LL2676: _temp2675=(( struct Cyc_Absyn_Evar_struct*) _temp2667)->f3;
goto _LL2674; _LL2674: _temp2673=(( struct Cyc_Absyn_Evar_struct*) _temp2667)->f4;
goto _LL2670;} else{ goto _LL2671;} _LL2671: goto _LL2672; _LL2670:{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
void* _temp2681= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( es2))->hd); struct Cyc_Core_Opt* _temp2687; int _temp2689; struct
Cyc_Core_Opt* _temp2691; struct Cyc_Core_Opt* _temp2693; _LL2683: if((
unsigned int) _temp2681 > 4u?*(( int*) _temp2681) == Cyc_Absyn_Evar: 0){ _LL2694:
_temp2693=(( struct Cyc_Absyn_Evar_struct*) _temp2681)->f1; goto _LL2692;
_LL2692: _temp2691=(( struct Cyc_Absyn_Evar_struct*) _temp2681)->f2; goto
_LL2690; _LL2690: _temp2689=(( struct Cyc_Absyn_Evar_struct*) _temp2681)->f3;
goto _LL2688; _LL2688: _temp2687=(( struct Cyc_Absyn_Evar_struct*) _temp2681)->f4;
goto _LL2684;} else{ goto _LL2685;} _LL2685: goto _LL2686; _LL2684: if(
_temp2675 == _temp2689){ return es;} goto _LL2682; _LL2686: goto _LL2682;
_LL2682:;}} return({ struct Cyc_List_List* _temp2695=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2695->hd=( void*) e; _temp2695->tl=
es; _temp2695;}); _LL2672: return es; _LL2668:;} static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs, struct Cyc_List_List*
btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ int present= 0;{ struct Cyc_List_List* b= btvs; for( 0;
b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){ if(*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->identity))
==*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->identity))){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp2696=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2696->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd); _temp2696->tl= r; _temp2696;});}}
r=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List* kind_env; struct Cyc_List_List*
free_vars; struct Cyc_List_List* free_evars; } ; static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcutil_CVTEnv cvtenv, void* expected_kind, void* t){{ void*
_temp2697= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp2743; int
_temp2745; struct Cyc_Core_Opt* _temp2747; struct Cyc_Core_Opt* _temp2749;
struct Cyc_Core_Opt** _temp2751; struct Cyc_Absyn_Tvar* _temp2752; struct Cyc_Absyn_Enumdecl*
_temp2754; struct Cyc_Absyn_Enumdecl** _temp2756; struct _tuple1* _temp2757;
struct Cyc_Absyn_TunionInfo _temp2759; void* _temp2761; struct Cyc_List_List*
_temp2763; void* _temp2765; void** _temp2767; struct Cyc_Absyn_TunionFieldInfo
_temp2768; struct Cyc_List_List* _temp2770; void* _temp2772; void** _temp2774;
struct Cyc_Absyn_PtrInfo _temp2775; struct Cyc_Absyn_Conref* _temp2777; struct
Cyc_Absyn_Tqual _temp2779; struct Cyc_Absyn_Conref* _temp2781; void* _temp2783;
void* _temp2785; void* _temp2787; void* _temp2789; struct Cyc_Absyn_Exp*
_temp2791; struct Cyc_Absyn_Tqual _temp2793; void* _temp2795; struct Cyc_Absyn_FnInfo
_temp2797; struct Cyc_List_List* _temp2799; struct Cyc_List_List* _temp2801;
struct Cyc_Absyn_VarargInfo* _temp2803; int _temp2805; struct Cyc_List_List*
_temp2807; void* _temp2809; struct Cyc_Core_Opt* _temp2811; struct Cyc_Core_Opt**
_temp2813; struct Cyc_List_List* _temp2814; struct Cyc_List_List** _temp2816;
struct Cyc_List_List* _temp2817; struct Cyc_List_List* _temp2819; struct Cyc_List_List*
_temp2821; struct Cyc_Absyn_Structdecl** _temp2823; struct Cyc_Absyn_Structdecl***
_temp2825; struct Cyc_List_List* _temp2826; struct _tuple1* _temp2828; struct
Cyc_Absyn_Uniondecl** _temp2830; struct Cyc_Absyn_Uniondecl*** _temp2832; struct
Cyc_List_List* _temp2833; struct _tuple1* _temp2835; struct Cyc_Core_Opt*
_temp2837; struct Cyc_Core_Opt** _temp2839; struct Cyc_List_List* _temp2840;
struct _tuple1* _temp2842; void* _temp2844; void* _temp2846; struct Cyc_List_List*
_temp2848; _LL2699: if( _temp2697 ==( void*) Cyc_Absyn_VoidType){ goto _LL2700;}
else{ goto _LL2701;} _LL2701: if(( unsigned int) _temp2697 > 4u?*(( int*)
_temp2697) == Cyc_Absyn_Evar: 0){ _LL2750: _temp2749=(( struct Cyc_Absyn_Evar_struct*)
_temp2697)->f1; _temp2751=&(( struct Cyc_Absyn_Evar_struct*) _temp2697)->f1;
goto _LL2748; _LL2748: _temp2747=(( struct Cyc_Absyn_Evar_struct*) _temp2697)->f2;
goto _LL2746; _LL2746: _temp2745=(( struct Cyc_Absyn_Evar_struct*) _temp2697)->f3;
goto _LL2744; _LL2744: _temp2743=(( struct Cyc_Absyn_Evar_struct*) _temp2697)->f4;
goto _LL2702;} else{ goto _LL2703;} _LL2703: if(( unsigned int) _temp2697 > 4u?*((
int*) _temp2697) == Cyc_Absyn_VarType: 0){ _LL2753: _temp2752=(( struct Cyc_Absyn_VarType_struct*)
_temp2697)->f1; goto _LL2704;} else{ goto _LL2705;} _LL2705: if(( unsigned int)
_temp2697 > 4u?*(( int*) _temp2697) == Cyc_Absyn_EnumType: 0){ _LL2758:
_temp2757=(( struct Cyc_Absyn_EnumType_struct*) _temp2697)->f1; goto _LL2755;
_LL2755: _temp2754=(( struct Cyc_Absyn_EnumType_struct*) _temp2697)->f2;
_temp2756=&(( struct Cyc_Absyn_EnumType_struct*) _temp2697)->f2; goto _LL2706;}
else{ goto _LL2707;} _LL2707: if(( unsigned int) _temp2697 > 4u?*(( int*)
_temp2697) == Cyc_Absyn_TunionType: 0){ _LL2760: _temp2759=(( struct Cyc_Absyn_TunionType_struct*)
_temp2697)->f1; _LL2766: _temp2765=( void*) _temp2759.tunion_info; _temp2767=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2697)->f1).tunion_info; goto _LL2764;
_LL2764: _temp2763= _temp2759.targs; goto _LL2762; _LL2762: _temp2761=( void*)
_temp2759.rgn; goto _LL2708;} else{ goto _LL2709;} _LL2709: if(( unsigned int)
_temp2697 > 4u?*(( int*) _temp2697) == Cyc_Absyn_TunionFieldType: 0){ _LL2769:
_temp2768=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2697)->f1; _LL2773:
_temp2772=( void*) _temp2768.field_info; _temp2774=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2697)->f1).field_info; goto _LL2771; _LL2771: _temp2770= _temp2768.targs;
goto _LL2710;} else{ goto _LL2711;} _LL2711: if(( unsigned int) _temp2697 > 4u?*((
int*) _temp2697) == Cyc_Absyn_PointerType: 0){ _LL2776: _temp2775=(( struct Cyc_Absyn_PointerType_struct*)
_temp2697)->f1; _LL2786: _temp2785=( void*) _temp2775.elt_typ; goto _LL2784;
_LL2784: _temp2783=( void*) _temp2775.rgn_typ; goto _LL2782; _LL2782: _temp2781=
_temp2775.nullable; goto _LL2780; _LL2780: _temp2779= _temp2775.tq; goto _LL2778;
_LL2778: _temp2777= _temp2775.bounds; goto _LL2712;} else{ goto _LL2713;}
_LL2713: if(( unsigned int) _temp2697 > 4u?*(( int*) _temp2697) == Cyc_Absyn_IntType:
0){ _LL2790: _temp2789=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2697)->f1;
goto _LL2788; _LL2788: _temp2787=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2697)->f2; goto _LL2714;} else{ goto _LL2715;} _LL2715: if( _temp2697 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2716;} else{ goto _LL2717;} _LL2717: if(
_temp2697 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2718;} else{ goto _LL2719;}
_LL2719: if(( unsigned int) _temp2697 > 4u?*(( int*) _temp2697) == Cyc_Absyn_ArrayType:
0){ _LL2796: _temp2795=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2697)->f1;
goto _LL2794; _LL2794: _temp2793=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2697)->f2; goto _LL2792; _LL2792: _temp2791=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2697)->f3; goto _LL2720;} else{ goto _LL2721;} _LL2721: if(( unsigned int)
_temp2697 > 4u?*(( int*) _temp2697) == Cyc_Absyn_FnType: 0){ _LL2798: _temp2797=((
struct Cyc_Absyn_FnType_struct*) _temp2697)->f1; _LL2815: _temp2814= _temp2797.tvars;
_temp2816=&((( struct Cyc_Absyn_FnType_struct*) _temp2697)->f1).tvars; goto
_LL2812; _LL2812: _temp2811= _temp2797.effect; _temp2813=&((( struct Cyc_Absyn_FnType_struct*)
_temp2697)->f1).effect; goto _LL2810; _LL2810: _temp2809=( void*) _temp2797.ret_typ;
goto _LL2808; _LL2808: _temp2807= _temp2797.args; goto _LL2806; _LL2806:
_temp2805= _temp2797.c_varargs; goto _LL2804; _LL2804: _temp2803= _temp2797.cyc_varargs;
goto _LL2802; _LL2802: _temp2801= _temp2797.rgn_po; goto _LL2800; _LL2800:
_temp2799= _temp2797.attributes; goto _LL2722;} else{ goto _LL2723;} _LL2723:
if(( unsigned int) _temp2697 > 4u?*(( int*) _temp2697) == Cyc_Absyn_TupleType: 0){
_LL2818: _temp2817=(( struct Cyc_Absyn_TupleType_struct*) _temp2697)->f1; goto
_LL2724;} else{ goto _LL2725;} _LL2725: if(( unsigned int) _temp2697 > 4u?*((
int*) _temp2697) == Cyc_Absyn_AnonStructType: 0){ _LL2820: _temp2819=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2697)->f1; goto _LL2726;} else{ goto
_LL2727;} _LL2727: if(( unsigned int) _temp2697 > 4u?*(( int*) _temp2697) == Cyc_Absyn_AnonUnionType:
0){ _LL2822: _temp2821=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2697)->f1;
goto _LL2728;} else{ goto _LL2729;} _LL2729: if(( unsigned int) _temp2697 > 4u?*((
int*) _temp2697) == Cyc_Absyn_StructType: 0){ _LL2829: _temp2828=(( struct Cyc_Absyn_StructType_struct*)
_temp2697)->f1; goto _LL2827; _LL2827: _temp2826=(( struct Cyc_Absyn_StructType_struct*)
_temp2697)->f2; goto _LL2824; _LL2824: _temp2823=(( struct Cyc_Absyn_StructType_struct*)
_temp2697)->f3; _temp2825=&(( struct Cyc_Absyn_StructType_struct*) _temp2697)->f3;
goto _LL2730;} else{ goto _LL2731;} _LL2731: if(( unsigned int) _temp2697 > 4u?*((
int*) _temp2697) == Cyc_Absyn_UnionType: 0){ _LL2836: _temp2835=(( struct Cyc_Absyn_UnionType_struct*)
_temp2697)->f1; goto _LL2834; _LL2834: _temp2833=(( struct Cyc_Absyn_UnionType_struct*)
_temp2697)->f2; goto _LL2831; _LL2831: _temp2830=(( struct Cyc_Absyn_UnionType_struct*)
_temp2697)->f3; _temp2832=&(( struct Cyc_Absyn_UnionType_struct*) _temp2697)->f3;
goto _LL2732;} else{ goto _LL2733;} _LL2733: if(( unsigned int) _temp2697 > 4u?*((
int*) _temp2697) == Cyc_Absyn_TypedefType: 0){ _LL2843: _temp2842=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2697)->f1; goto _LL2841; _LL2841: _temp2840=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2697)->f2; goto _LL2838; _LL2838: _temp2837=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2697)->f3; _temp2839=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2697)->f3;
goto _LL2734;} else{ goto _LL2735;} _LL2735: if( _temp2697 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2736;} else{ goto _LL2737;} _LL2737: if(( unsigned int) _temp2697 > 4u?*((
int*) _temp2697) == Cyc_Absyn_RgnHandleType: 0){ _LL2845: _temp2844=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2697)->f1; goto _LL2738;} else{
goto _LL2739;} _LL2739: if(( unsigned int) _temp2697 > 4u?*(( int*) _temp2697)
== Cyc_Absyn_AccessEff: 0){ _LL2847: _temp2846=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2697)->f1; goto _LL2740;} else{ goto _LL2741;} _LL2741: if(( unsigned int)
_temp2697 > 4u?*(( int*) _temp2697) == Cyc_Absyn_JoinEff: 0){ _LL2849: _temp2848=((
struct Cyc_Absyn_JoinEff_struct*) _temp2697)->f1; goto _LL2742;} else{ goto
_LL2698;} _LL2700: goto _LL2698; _LL2702: if(* _temp2751 == 0){* _temp2751=({
struct Cyc_Core_Opt* _temp2850=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2850->v=( void*) expected_kind; _temp2850;});} cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars, t); goto _LL2698; _LL2704: { struct
Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp2752->kind); if(( void*) c->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2851=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2851[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2852; _temp2852.tag= Cyc_Absyn_Eq_constr;
_temp2852.f1=( void*) expected_kind; _temp2852;}); _temp2851;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2752); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2752); goto _LL2698;}
_LL2706: if(* _temp2756 == 0){ struct _handler_cons _temp2853; _push_handler(&
_temp2853);{ int _temp2855= 0; if( setjmp( _temp2853.handler)){ _temp2855= 1;}
if( ! _temp2855){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2757);* _temp2756=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2854=( void*) _exn_thrown; void* _temp2857= _temp2854; _LL2859:
if( _temp2857 == Cyc_Dict_Absent){ goto _LL2860;} else{ goto _LL2861;} _LL2861:
goto _LL2862; _LL2860: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp2863= Cyc_Absynpp_qvar2string( _temp2757); xprintf("unbound type enum %.*s",
_temp2863.last_plus_one - _temp2863.curr, _temp2863.curr);})); return cvtenv;
_LL2862:( void) _throw( _temp2857); _LL2858:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2756);* _temp2757=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2698;} _LL2708:{ void*
_temp2864=* _temp2767; struct Cyc_Absyn_UnknownTunionInfo _temp2870; int
_temp2872; struct _tuple1* _temp2874; struct Cyc_Absyn_Tuniondecl* _temp2876;
_LL2866: if(*(( int*) _temp2864) == Cyc_Absyn_UnknownTunion){ _LL2871: _temp2870=((
struct Cyc_Absyn_UnknownTunion_struct*) _temp2864)->f1; _LL2875: _temp2874=
_temp2870.name; goto _LL2873; _LL2873: _temp2872= _temp2870.is_xtunion; goto
_LL2867;} else{ goto _LL2868;} _LL2868: if(*(( int*) _temp2864) == Cyc_Absyn_KnownTunion){
_LL2877: _temp2876=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2864)->f1; goto
_LL2869;} else{ goto _LL2865;} _LL2867: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2878; _push_handler(& _temp2878);{ int _temp2880= 0;
if( setjmp( _temp2878.handler)){ _temp2880= 1;} if( ! _temp2880){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2874);; _pop_handler();} else{ void* _temp2879=( void*)
_exn_thrown; void* _temp2882= _temp2879; _LL2884: if( _temp2882 == Cyc_Dict_Absent){
goto _LL2885;} else{ goto _LL2886;} _LL2886: goto _LL2887; _LL2885: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2888= Cyc_Absynpp_qvar2string(
_temp2874); xprintf("unbound type [x]tunion %.*s", _temp2888.last_plus_one -
_temp2888.curr, _temp2888.curr);})); return cvtenv; _LL2887:( void) _throw(
_temp2882); _LL2883:;}}} if( tud->is_xtunion != _temp2872){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp2889= Cyc_Absynpp_qvar2string(
_temp2874); xprintf("[x]tunion is different from type declaration %.*s",
_temp2889.last_plus_one - _temp2889.curr, _temp2889.curr);}));}* _temp2767=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2890=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2890[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2891; _temp2891.tag= Cyc_Absyn_KnownTunion;
_temp2891.f1= tud; _temp2891;}); _temp2890;}); _temp2876= tud; goto _LL2869;}
_LL2869: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2761);{ struct Cyc_List_List* tvs= _temp2876->tvs; for( 0; _temp2763 != 0?
tvs != 0: 0;( _temp2763=(( struct Cyc_List_List*) _check_null( _temp2763))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2763))->hd; void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} if( _temp2763 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp2892= Cyc_Absynpp_qvar2string( _temp2876->name);
xprintf("too many type arguments for tunion %.*s", _temp2892.last_plus_one -
_temp2892.curr, _temp2892.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp2893= Cyc_Absynpp_qvar2string(
_temp2876->name); xprintf("too few type arguments for tunion %.*s", _temp2893.last_plus_one
- _temp2893.curr, _temp2893.curr);}));} goto _LL2865;} _LL2865:;} goto _LL2698;
_LL2710:{ void* _temp2894=* _temp2774; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2900; int _temp2902; struct _tuple1* _temp2904; struct _tuple1* _temp2906;
struct Cyc_Absyn_Tunionfield* _temp2908; struct Cyc_Absyn_Tuniondecl* _temp2910;
_LL2896: if(*(( int*) _temp2894) == Cyc_Absyn_UnknownTunionfield){ _LL2901:
_temp2900=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2894)->f1;
_LL2907: _temp2906= _temp2900.tunion_name; goto _LL2905; _LL2905: _temp2904=
_temp2900.field_name; goto _LL2903; _LL2903: _temp2902= _temp2900.is_xtunion;
goto _LL2897;} else{ goto _LL2898;} _LL2898: if(*(( int*) _temp2894) == Cyc_Absyn_KnownTunionfield){
_LL2911: _temp2910=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2894)->f1;
goto _LL2909; _LL2909: _temp2908=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2894)->f2; goto _LL2899;} else{ goto _LL2895;} _LL2897: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2912;
_push_handler(& _temp2912);{ int _temp2914= 0; if( setjmp( _temp2912.handler)){
_temp2914= 1;} if( ! _temp2914){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2906);; _pop_handler();} else{ void* _temp2913=( void*) _exn_thrown; void*
_temp2916= _temp2913; _LL2918: if( _temp2916 == Cyc_Dict_Absent){ goto _LL2919;}
else{ goto _LL2920;} _LL2920: goto _LL2921; _LL2919: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp2922= Cyc_Absynpp_qvar2string(
_temp2906); xprintf("unbound type tunion %.*s", _temp2922.last_plus_one -
_temp2922.curr, _temp2922.curr);})); return cvtenv; _LL2921:( void) _throw(
_temp2916); _LL2917:;}}}{ struct _handler_cons _temp2923; _push_handler(&
_temp2923);{ int _temp2925= 0; if( setjmp( _temp2923.handler)){ _temp2925= 1;}
if( ! _temp2925){{ void* _temp2926= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2904); struct Cyc_Absyn_Tunionfield* _temp2932; struct Cyc_Absyn_Tuniondecl*
_temp2934; _LL2928: if(*(( int*) _temp2926) == Cyc_Tcenv_TunionRes){ _LL2935:
_temp2934=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2926)->f1; goto _LL2933;
_LL2933: _temp2932=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2926)->f2; goto
_LL2929;} else{ goto _LL2930;} _LL2930: goto _LL2931; _LL2929: tuf= _temp2932;
tud= _temp2934; if( tud->is_xtunion != _temp2902){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp2936= Cyc_Absynpp_qvar2string( _temp2906);
xprintf("[x]tunion is different from type declaration %.*s", _temp2936.last_plus_one
- _temp2936.curr, _temp2936.curr);}));} goto _LL2927; _LL2931: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2937= Cyc_Absynpp_qvar2string(
_temp2904); struct _tagged_arr _temp2938= Cyc_Absynpp_qvar2string( _temp2906);
xprintf("unbound field %.*s in type tunion %.*s", _temp2937.last_plus_one -
_temp2937.curr, _temp2937.curr, _temp2938.last_plus_one - _temp2938.curr,
_temp2938.curr);}));{ struct Cyc_Tcutil_CVTEnv _temp2939= cvtenv; _npop_handler(
0u); return _temp2939;} _LL2927:;}; _pop_handler();} else{ void* _temp2924=(
void*) _exn_thrown; void* _temp2941= _temp2924; _LL2943: if( _temp2941 == Cyc_Dict_Absent){
goto _LL2944;} else{ goto _LL2945;} _LL2945: goto _LL2946; _LL2944: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2947= Cyc_Absynpp_qvar2string(
_temp2904); struct _tagged_arr _temp2948= Cyc_Absynpp_qvar2string( _temp2906);
xprintf("unbound field %.*s in type tunion %.*s", _temp2947.last_plus_one -
_temp2947.curr, _temp2947.curr, _temp2948.last_plus_one - _temp2948.curr,
_temp2948.curr);})); return cvtenv; _LL2946:( void) _throw( _temp2941); _LL2942:;}}}*
_temp2774=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct* _temp2949=(
struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunionfield_struct));
_temp2949[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct _temp2950; _temp2950.tag=
Cyc_Absyn_KnownTunionfield; _temp2950.f1= tud; _temp2950.f2= tuf; _temp2950;});
_temp2949;}); _temp2910= tud; _temp2908= tuf; goto _LL2899;} _LL2899: { struct
Cyc_List_List* tvs= _temp2910->tvs; for( 0; _temp2770 != 0? tvs != 0: 0;(
_temp2770=(( struct Cyc_List_List*) _check_null( _temp2770))->tl, tvs=(( struct
Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)(( struct Cyc_List_List*)
_check_null( _temp2770))->hd; void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k1, t1);} if( _temp2770 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp2951= Cyc_Absynpp_qvar2string( _temp2910->name); struct
_tagged_arr _temp2952= Cyc_Absynpp_qvar2string( _temp2908->name); xprintf("too many type arguments for tunion %.*s.%.*s",
_temp2951.last_plus_one - _temp2951.curr, _temp2951.curr, _temp2952.last_plus_one
- _temp2952.curr, _temp2952.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp2953= Cyc_Absynpp_qvar2string(
_temp2910->name); struct _tagged_arr _temp2954= Cyc_Absynpp_qvar2string(
_temp2908->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2953.last_plus_one - _temp2953.curr, _temp2953.curr, _temp2954.last_plus_one
- _temp2954.curr, _temp2954.curr);}));} goto _LL2895;} _LL2895:;} goto _LL2698;
_LL2712: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2785); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2783);{ void* _temp2955=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2777))->v; void* _temp2961; struct Cyc_Absyn_Exp*
_temp2963; _LL2957: if(( unsigned int) _temp2955 > 1u?*(( int*) _temp2955) ==
Cyc_Absyn_Eq_constr: 0){ _LL2962: _temp2961=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2955)->f1; if(( unsigned int) _temp2961 > 1u?*(( int*) _temp2961) == Cyc_Absyn_Upper_b:
0){ _LL2964: _temp2963=(( struct Cyc_Absyn_Upper_b_struct*) _temp2961)->f1; goto
_LL2958;} else{ goto _LL2959;}} else{ goto _LL2959;} _LL2959: goto _LL2960;
_LL2958: if( ! Cyc_Tcutil_is_const_exp( te, _temp2963)){ Cyc_Tcutil_terr( loc,
_tag_arr("pointer bounds expression is not a constant", sizeof( unsigned char),
44u));} Cyc_Tcexp_tcExp( te, 0, _temp2963); if( ! Cyc_Tcutil_coerce_uint_typ( te,
_temp2963)){ Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u));} Cyc_Evexp_eval_const_uint_exp( _temp2963); goto
_LL2956; _LL2960: goto _LL2956; _LL2956:;} goto _LL2698; _LL2714: goto _LL2698;
_LL2716: goto _LL2698; _LL2718: goto _LL2698; _LL2720: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp2795); if( _temp2791 == 0? 1: !
Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null( _temp2791))){
Cyc_Tcutil_terr( loc, _tag_arr("array bounds expression is not constant",
sizeof( unsigned char), 40u));} Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_check_null( _temp2791)); if( ! Cyc_Tcutil_coerce_uint_typ( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2791))){ Cyc_Tcutil_terr( loc, _tag_arr("array bounds expression is not an unsigned int",
sizeof( unsigned char), 47u));} Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp2791)); goto _LL2698; _LL2722: { int num_convs= 0; int
seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; for( 0; _temp2799 != 0;
_temp2799=(( struct Cyc_List_List*) _check_null( _temp2799))->tl){ if( ! Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( _temp2799))->hd)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2965= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2799))->hd); xprintf("bad function type attribute %.*s",
_temp2965.last_plus_one - _temp2965.curr, _temp2965.curr);}));}{ void* _temp2966=(
void*)(( struct Cyc_List_List*) _check_null( _temp2799))->hd; _LL2968: if(
_temp2966 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL2969;} else{ goto _LL2970;}
_LL2970: if( _temp2966 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL2971;} else{
goto _LL2972;} _LL2972: if( _temp2966 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL2973;} else{ goto _LL2974;} _LL2974: goto _LL2975; _LL2969: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2967; _LL2971: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2967; _LL2973: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2967; _LL2975: goto
_LL2967; _LL2967:;}} if( num_convs > 1){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr) xprintf("function can't have multiple calling conventions"));} Cyc_Tcutil_check_unique_tvars(
loc,* _temp2816);{ struct Cyc_List_List* b=* _temp2816; for( 0; b != 0; b=((
struct Cyc_List_List*) _check_null( b))->tl){(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( b))->hd)->identity= Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd);{ void* _temp2976=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->kind))->v;
void* _temp2982; _LL2978: if(( unsigned int) _temp2976 > 1u?*(( int*) _temp2976)
== Cyc_Absyn_Eq_constr: 0){ _LL2983: _temp2982=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2976)->f1; if( _temp2982 ==( void*) Cyc_Absyn_MemKind){ goto _LL2979;}
else{ goto _LL2980;}} else{ goto _LL2980;} _LL2980: goto _LL2981; _LL2979: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2984=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_temp2984.last_plus_one - _temp2984.curr, _temp2984.curr);})); goto _LL2977;
_LL2981: goto _LL2977; _LL2977:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2986=({
struct Cyc_Tcutil_CVTEnv _temp2985; _temp2985.kind_env= cvtenv.kind_env;
_temp2985.free_vars= 0; _temp2985.free_evars= 0; _temp2985;}); goto _LL2987;
_LL2987: _temp2986= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2986,( void*)
Cyc_Absyn_MemKind, _temp2809); for( 0; _temp2807 != 0; _temp2807=(( struct Cyc_List_List*)
_check_null( _temp2807))->tl){ _temp2986= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2986,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp2807))->hd)).f3);} if( _temp2803 != 0){ if( _temp2805){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("both c_vararg and cyc_vararg",
sizeof( unsigned char), 29u));}{ int _temp2990; void* _temp2992; void* _temp2994;
struct Cyc_Absyn_Tqual _temp2996; struct Cyc_Core_Opt* _temp2998; struct Cyc_Absyn_VarargInfo
_temp2988=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2803)); _LL2999:
_temp2998= _temp2988.name; goto _LL2997; _LL2997: _temp2996= _temp2988.tq; goto
_LL2995; _LL2995: _temp2994=( void*) _temp2988.type; goto _LL2993; _LL2993:
_temp2992=( void*) _temp2988.rgn; goto _LL2991; _LL2991: _temp2990= _temp2988.inject;
goto _LL2989; _LL2989: _temp2986= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2986,( void*) Cyc_Absyn_MemKind, _temp2994);{ void* _temp3000= Cyc_Tcutil_compress(
_temp2992); struct Cyc_Core_Opt* _temp3010; int _temp3012; struct Cyc_Core_Opt*
_temp3014; struct Cyc_Core_Opt* _temp3016; struct Cyc_Absyn_Tvar* _temp3018;
_LL3002: if(( unsigned int) _temp3000 > 4u?*(( int*) _temp3000) == Cyc_Absyn_Evar:
0){ _LL3017: _temp3016=(( struct Cyc_Absyn_Evar_struct*) _temp3000)->f1; goto
_LL3015; _LL3015: _temp3014=(( struct Cyc_Absyn_Evar_struct*) _temp3000)->f2;
goto _LL3013; _LL3013: _temp3012=(( struct Cyc_Absyn_Evar_struct*) _temp3000)->f3;
goto _LL3011; _LL3011: _temp3010=(( struct Cyc_Absyn_Evar_struct*) _temp3000)->f4;
goto _LL3003;} else{ goto _LL3004;} _LL3004: if(( unsigned int) _temp3000 > 4u?*((
int*) _temp3000) == Cyc_Absyn_VarType: 0){ _LL3019: _temp3018=(( struct Cyc_Absyn_VarType_struct*)
_temp3000)->f1; goto _LL3005;} else{ goto _LL3006;} _LL3006: if( _temp3000 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL3007;} else{ goto _LL3008;} _LL3008: goto
_LL3009; _LL3003: { void* vr_tvar=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3020=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3020[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3021; _temp3021.tag= Cyc_Absyn_VarType;
_temp3021.f1= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp3021;});
_temp3020;}); Cyc_Tcutil_unify( _temp2992, vr_tvar); goto _LL3001;} _LL3005:
goto _LL3001; _LL3007: Cyc_Tcutil_terr( loc, _tag_arr("varargs should be in a region, not heap-allocated",
sizeof( unsigned char), 50u)); goto _LL3001; _LL3009: goto _LL3001; _LL3001:;}
_temp2986= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2986,( void*) Cyc_Absyn_RgnKind,
_temp2992); if( _temp2990){ void* _temp3022= Cyc_Tcutil_compress( _temp2994);
struct Cyc_Absyn_TunionInfo _temp3028; void* _temp3030; struct Cyc_List_List*
_temp3032; void* _temp3034; struct Cyc_Absyn_Tuniondecl* _temp3036; _LL3024: if((
unsigned int) _temp3022 > 4u?*(( int*) _temp3022) == Cyc_Absyn_TunionType: 0){
_LL3029: _temp3028=(( struct Cyc_Absyn_TunionType_struct*) _temp3022)->f1;
_LL3035: _temp3034=( void*) _temp3028.tunion_info; if(*(( int*) _temp3034) ==
Cyc_Absyn_KnownTunion){ _LL3037: _temp3036=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp3034)->f1; goto _LL3033;} else{ goto _LL3026;} _LL3033: _temp3032=
_temp3028.targs; goto _LL3031; _LL3031: _temp3030=( void*) _temp3028.rgn; goto
_LL3025;} else{ goto _LL3026;} _LL3026: goto _LL3027; _LL3025:{ void* _temp3038=
Cyc_Tcutil_compress( _temp3030); struct Cyc_Core_Opt* _temp3048; int _temp3050;
struct Cyc_Core_Opt* _temp3052; struct Cyc_Core_Opt* _temp3054; struct Cyc_Absyn_Tvar*
_temp3056; _LL3040: if(( unsigned int) _temp3038 > 4u?*(( int*) _temp3038) ==
Cyc_Absyn_Evar: 0){ _LL3055: _temp3054=(( struct Cyc_Absyn_Evar_struct*)
_temp3038)->f1; goto _LL3053; _LL3053: _temp3052=(( struct Cyc_Absyn_Evar_struct*)
_temp3038)->f2; goto _LL3051; _LL3051: _temp3050=(( struct Cyc_Absyn_Evar_struct*)
_temp3038)->f3; goto _LL3049; _LL3049: _temp3048=(( struct Cyc_Absyn_Evar_struct*)
_temp3038)->f4; goto _LL3041;} else{ goto _LL3042;} _LL3042: if(( unsigned int)
_temp3038 > 4u?*(( int*) _temp3038) == Cyc_Absyn_VarType: 0){ _LL3057: _temp3056=((
struct Cyc_Absyn_VarType_struct*) _temp3038)->f1; goto _LL3043;} else{ goto
_LL3044;} _LL3044: if( _temp3038 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL3045;}
else{ goto _LL3046;} _LL3046: goto _LL3047; _LL3041: Cyc_Tcutil_unify( _temp2992,
_temp3030); goto _LL3039; _LL3043: goto _LL3039; _LL3045: Cyc_Tcutil_terr( loc,
_tag_arr("injected vararg [x]tunion should be in a region, not heap-allocated",
sizeof( unsigned char), 68u)); goto _LL3039; _LL3047: goto _LL3039; _LL3039:;}
goto _LL3023; _LL3027: Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u)); goto _LL3023; _LL3023:;}}}{ struct Cyc_List_List*
rpo= _temp2801; for( 0; rpo != 0; rpo=(( struct Cyc_List_List*) _check_null( rpo))->tl){
struct _tuple8 _temp3060; void* _temp3061; void* _temp3063; struct _tuple8*
_temp3058=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( rpo))->hd;
_temp3060=* _temp3058; _LL3064: _temp3063= _temp3060.f1; goto _LL3062; _LL3062:
_temp3061= _temp3060.f2; goto _LL3059; _LL3059: _temp2986= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2986,( void*) Cyc_Absyn_RgnKind, _temp3063); _temp2986= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2986,( void*) Cyc_Absyn_RgnKind, _temp3061);}} if(* _temp2813 != 0){
_temp2986= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2986,( void*) Cyc_Absyn_EffKind,(
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp2813))->v);} else{ struct Cyc_Absyn_Tvar*
e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind); _temp2986.free_vars= Cyc_Tcutil_add_free_tvar(
loc, _temp2986.free_vars, e);{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= _temp2986.free_vars; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ void* _temp3065=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp3073; void*
_temp3075; _LL3067: if(( unsigned int) _temp3065 > 1u?*(( int*) _temp3065) ==
Cyc_Absyn_Eq_constr: 0){ _LL3074: _temp3073=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3065)->f1; if( _temp3073 ==( void*) Cyc_Absyn_RgnKind){ goto _LL3068;}
else{ goto _LL3069;}} else{ goto _LL3069;} _LL3069: if(( unsigned int) _temp3065
> 1u?*(( int*) _temp3065) == Cyc_Absyn_Eq_constr: 0){ _LL3076: _temp3075=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3065)->f1; if( _temp3075 ==( void*) Cyc_Absyn_EffKind){
goto _LL3070;} else{ goto _LL3071;}} else{ goto _LL3071;} _LL3071: goto _LL3072;
_LL3068: effect=({ struct Cyc_List_List* _temp3077=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3077->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp3078=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp3078[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp3079; _temp3079.tag= Cyc_Absyn_AccessEff;
_temp3079.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3080=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3080[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3081; _temp3081.tag= Cyc_Absyn_VarType;
_temp3081.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3081;}); _temp3080;})); _temp3079;}); _temp3078;})); _temp3077->tl= effect;
_temp3077;}); goto _LL3066; _LL3070: effect=({ struct Cyc_List_List* _temp3082=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3082->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3083=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3083[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3084; _temp3084.tag= Cyc_Absyn_VarType; _temp3084.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3084;}); _temp3083;})); _temp3082->tl= effect; _temp3082;}); goto _LL3066;
_LL3072: goto _LL3066; _LL3066:;}}{ struct Cyc_List_List* ts= _temp2986.free_evars;
for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ void*
_temp3085= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_LL3087: if( _temp3085 ==( void*) Cyc_Absyn_RgnKind){ goto _LL3088;} else{ goto
_LL3089;} _LL3089: if( _temp3085 ==( void*) Cyc_Absyn_EffKind){ goto _LL3090;}
else{ goto _LL3091;} _LL3091: goto _LL3092; _LL3088: effect=({ struct Cyc_List_List*
_temp3093=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3093->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3094=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3094[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3095; _temp3095.tag= Cyc_Absyn_AccessEff;
_temp3095.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp3095;}); _temp3094;})); _temp3093->tl= effect; _temp3093;}); goto _LL3086;
_LL3090: effect=({ struct Cyc_List_List* _temp3096=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3096->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp3096->tl= effect; _temp3096;});
goto _LL3086; _LL3092: goto _LL3086; _LL3086:;}}* _temp2813=({ struct Cyc_Core_Opt*
_temp3097=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3097->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3098=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3098[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3099; _temp3099.tag= Cyc_Absyn_JoinEff;
_temp3099.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effect); _temp3099;}); _temp3098;})); _temp3097;});}} if(* _temp2816 != 0){
struct Cyc_List_List* bs=* _temp2816; for( 0; bs != 0; bs=(( struct Cyc_List_List*)
_check_null( bs))->tl){ struct Cyc_Absyn_Conref* _temp3100=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->kind); goto _LL3101; _LL3101: {
void* _temp3102=( void*) _temp3100->v; void* _temp3110; _LL3104: if( _temp3102
==( void*) Cyc_Absyn_No_constr){ goto _LL3105;} else{ goto _LL3106;} _LL3106:
if(( unsigned int) _temp3102 > 1u?*(( int*) _temp3102) == Cyc_Absyn_Eq_constr: 0){
_LL3111: _temp3110=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3102)->f1;
if( _temp3110 ==( void*) Cyc_Absyn_MemKind){ goto _LL3107;} else{ goto _LL3108;}}
else{ goto _LL3108;} _LL3108: goto _LL3109; _LL3105: Cyc_Tcutil_warn( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp3112=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp3112.last_plus_one - _temp3112.curr, _temp3112.curr);})); goto _LL3107;
_LL3107:( void*)( _temp3100->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3113=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3113[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3114; _temp3114.tag= Cyc_Absyn_Eq_constr;
_temp3114.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3114;}); _temp3113;})));
goto _LL3103; _LL3109: goto _LL3103; _LL3103:;}}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2986.kind_env,* _temp2816); _temp2986.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2986.free_vars,* _temp2816);{ struct Cyc_List_List* tvs= _temp2986.free_vars;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2986.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2698;}}
_LL2724: for( 0; _temp2817 != 0; _temp2817=(( struct Cyc_List_List*) _check_null(
_temp2817))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp2817))->hd)).f2);} goto _LL2698; _LL2726:{ struct _RegionHandle _temp3115=
_new_region(); struct _RegionHandle* sprev_rgn=& _temp3115; _push_region(
sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2819 != 0;
_temp2819=(( struct Cyc_List_List*) _check_null( _temp2819))->tl){ struct Cyc_Absyn_Structfield
_temp3118; struct Cyc_List_List* _temp3119; struct Cyc_Core_Opt* _temp3121; void*
_temp3123; struct Cyc_Absyn_Tqual _temp3125; struct _tagged_arr* _temp3127;
struct Cyc_Absyn_Structfield* _temp3116=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2819))->hd; _temp3118=* _temp3116;
_LL3128: _temp3127= _temp3118.name; goto _LL3126; _LL3126: _temp3125= _temp3118.tq;
goto _LL3124; _LL3124: _temp3123=( void*) _temp3118.type; goto _LL3122; _LL3122:
_temp3121= _temp3118.width; goto _LL3120; _LL3120: _temp3119= _temp3118.attributes;
goto _LL3117; _LL3117: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp3127)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp3129=* _temp3127; xprintf("duplicate field %.*s in struct",
_temp3129.last_plus_one - _temp3129.curr, _temp3129.curr);}));} if( Cyc_String_strcmp(*
_temp3127, _tag_arr("", sizeof( unsigned char), 1u)) != 0){ prev_fields=({
struct Cyc_List_List* _temp3130=( struct Cyc_List_List*) _region_malloc(
sprev_rgn, sizeof( struct Cyc_List_List)); _temp3130->hd=( void*) _temp3127;
_temp3130->tl= prev_fields; _temp3130;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3123); if( _temp3121 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp3121))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp3131=* _temp3127; xprintf("bitfield %.*s does not have constant width",
_temp3131.last_plus_one - _temp3131.curr, _temp3131.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3121))->v);}{
void* _temp3132= Cyc_Tcutil_compress( _temp3123); void* _temp3138; void*
_temp3140; _LL3134: if(( unsigned int) _temp3132 > 4u?*(( int*) _temp3132) ==
Cyc_Absyn_IntType: 0){ _LL3141: _temp3140=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3132)->f1; goto _LL3139; _LL3139: _temp3138=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3132)->f2; goto _LL3135;} else{ goto _LL3136;} _LL3136: goto _LL3137;
_LL3135:{ void* _temp3142= _temp3138; _LL3144: if( _temp3142 ==( void*) Cyc_Absyn_B1){
goto _LL3145;} else{ goto _LL3146;} _LL3146: if( _temp3142 ==( void*) Cyc_Absyn_B2){
goto _LL3147;} else{ goto _LL3148;} _LL3148: if( _temp3142 ==( void*) Cyc_Absyn_B4){
goto _LL3149;} else{ goto _LL3150;} _LL3150: if( _temp3142 ==( void*) Cyc_Absyn_B8){
goto _LL3151;} else{ goto _LL3143;} _LL3145: if( w > 8){ Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL3143; _LL3147: if( w > 16){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL3143; _LL3149: if( w > 32){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL3143; _LL3151: if( w > 64){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL3143; _LL3143:;} goto _LL3133; _LL3137:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3152=*
_temp3127; struct _tagged_arr _temp3153= Cyc_Absynpp_typ2string( _temp3123);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp3152.last_plus_one
- _temp3152.curr, _temp3152.curr, _temp3153.last_plus_one - _temp3153.curr,
_temp3153.curr);})); goto _LL3133; _LL3133:;}} for( 0; _temp3119 != 0; _temp3119=((
struct Cyc_List_List*) _check_null( _temp3119))->tl){ void* _temp3154=( void*)((
struct Cyc_List_List*) _check_null( _temp3119))->hd; int _temp3162; _LL3156: if(
_temp3154 ==( void*) Cyc_Absyn_Packed_att){ goto _LL3157;} else{ goto _LL3158;}
_LL3158: if(( unsigned int) _temp3154 > 16u?*(( int*) _temp3154) == Cyc_Absyn_Aligned_att:
0){ _LL3163: _temp3162=(( struct Cyc_Absyn_Aligned_att_struct*) _temp3154)->f1;
goto _LL3159;} else{ goto _LL3160;} _LL3160: goto _LL3161; _LL3157: continue;
_LL3159: continue; _LL3161: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp3164= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp3119))->hd); struct _tagged_arr _temp3165=* _temp3127; xprintf("bad attribute %.*s on struct member %.*s",
_temp3164.last_plus_one - _temp3164.curr, _temp3164.curr, _temp3165.last_plus_one
- _temp3165.curr, _temp3165.curr);})); goto _LL3155; _LL3155:;}}}; _pop_region(&
_temp3115);} goto _LL2698; _LL2728:{ struct _RegionHandle _temp3166= _new_region();
struct _RegionHandle* uprev_rgn=& _temp3166; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; for( 0; _temp2821 != 0; _temp2821=(( struct Cyc_List_List*)
_check_null( _temp2821))->tl){ struct Cyc_Absyn_Structfield _temp3169; struct
Cyc_List_List* _temp3170; struct Cyc_Core_Opt* _temp3172; void* _temp3174;
struct Cyc_Absyn_Tqual _temp3176; struct _tagged_arr* _temp3178; struct Cyc_Absyn_Structfield*
_temp3167=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2821))->hd; _temp3169=* _temp3167; _LL3179: _temp3178= _temp3169.name; goto
_LL3177; _LL3177: _temp3176= _temp3169.tq; goto _LL3175; _LL3175: _temp3174=(
void*) _temp3169.type; goto _LL3173; _LL3173: _temp3172= _temp3169.width; goto
_LL3171; _LL3171: _temp3170= _temp3169.attributes; goto _LL3168; _LL3168: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp3178)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp3180=* _temp3178; xprintf("duplicate field %.*s in struct", _temp3180.last_plus_one
- _temp3180.curr, _temp3180.curr);}));} if( Cyc_String_strcmp(* _temp3178,
_tag_arr("", sizeof( unsigned char), 1u)) != 0){ prev_fields=({ struct Cyc_List_List*
_temp3181=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp3181->hd=( void*) _temp3178; _temp3181->tl= prev_fields; _temp3181;});}
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp3174); if( ! Cyc_Tcutil_bits_only( _temp3174)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp3182=* _temp3178; xprintf("%.*s has a type that is possibly incompatible with other members of union",
_temp3182.last_plus_one - _temp3182.curr, _temp3182.curr);}));} if( _temp3172 !=
0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp3172))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp3183=* _temp3178; xprintf("bitfield %.*s does not have constant width",
_temp3183.last_plus_one - _temp3183.curr, _temp3183.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3172))->v);}{
void* _temp3184= Cyc_Tcutil_compress( _temp3174); void* _temp3190; void*
_temp3192; _LL3186: if(( unsigned int) _temp3184 > 4u?*(( int*) _temp3184) ==
Cyc_Absyn_IntType: 0){ _LL3193: _temp3192=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3184)->f1; goto _LL3191; _LL3191: _temp3190=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3184)->f2; goto _LL3187;} else{ goto _LL3188;} _LL3188: goto _LL3189;
_LL3187:{ void* _temp3194= _temp3190; _LL3196: if( _temp3194 ==( void*) Cyc_Absyn_B1){
goto _LL3197;} else{ goto _LL3198;} _LL3198: if( _temp3194 ==( void*) Cyc_Absyn_B2){
goto _LL3199;} else{ goto _LL3200;} _LL3200: if( _temp3194 ==( void*) Cyc_Absyn_B4){
goto _LL3201;} else{ goto _LL3202;} _LL3202: if( _temp3194 ==( void*) Cyc_Absyn_B8){
goto _LL3203;} else{ goto _LL3195;} _LL3197: if( w > 8){ Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL3195; _LL3199: if( w > 16){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL3195; _LL3201: if( w > 32){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL3195; _LL3203: if( w > 64){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL3195; _LL3195:;} goto _LL3185; _LL3189:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3204=*
_temp3178; struct _tagged_arr _temp3205= Cyc_Absynpp_typ2string( _temp3174);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp3204.last_plus_one
- _temp3204.curr, _temp3204.curr, _temp3205.last_plus_one - _temp3205.curr,
_temp3205.curr);})); goto _LL3185; _LL3185:;}} for( 0; _temp3170 != 0; _temp3170=((
struct Cyc_List_List*) _check_null( _temp3170))->tl){ void* _temp3206=( void*)((
struct Cyc_List_List*) _check_null( _temp3170))->hd; int _temp3214; _LL3208: if(
_temp3206 ==( void*) Cyc_Absyn_Packed_att){ goto _LL3209;} else{ goto _LL3210;}
_LL3210: if(( unsigned int) _temp3206 > 16u?*(( int*) _temp3206) == Cyc_Absyn_Aligned_att:
0){ _LL3215: _temp3214=(( struct Cyc_Absyn_Aligned_att_struct*) _temp3206)->f1;
goto _LL3211;} else{ goto _LL3212;} _LL3212: goto _LL3213; _LL3209: continue;
_LL3211: continue; _LL3213: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp3216= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp3170))->hd); struct _tagged_arr _temp3217=* _temp3178; xprintf("bad attribute %.*s on struct member %.*s",
_temp3216.last_plus_one - _temp3216.curr, _temp3216.curr, _temp3217.last_plus_one
- _temp3217.curr, _temp3217.curr);})); goto _LL3207; _LL3207:;}}}; _pop_region(&
_temp3166);} goto _LL2698; _LL2730: if( _temp2828 == 0){ if(* _temp2825 == 0){
Cyc_Tcutil_terr( loc,( struct _tagged_arr) xprintf("bad anonymous struct"));
return cvtenv;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null(* _temp2825));;}} if(* _temp2825 == 0){ struct _handler_cons
_temp3218; _push_handler(& _temp3218);{ int _temp3220= 0; if( setjmp( _temp3218.handler)){
_temp3220= 1;} if( ! _temp3220){* _temp2825=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2828));; _pop_handler();} else{
void* _temp3219=( void*) _exn_thrown; void* _temp3222= _temp3219; _LL3224: if(
_temp3222 == Cyc_Dict_Absent){ goto _LL3225;} else{ goto _LL3226;} _LL3226: goto
_LL3227; _LL3225: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp3228= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp2828)); xprintf("unbound type struct %.*s", _temp3228.last_plus_one -
_temp3228.curr, _temp3228.curr);})); return cvtenv; _LL3227:( void) _throw(
_temp3222); _LL3223:;}}}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null(* _temp2825)); if( sd->name != 0){*(( struct _tuple1*) _check_null(
_temp2828))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2826); if( lvs != largs){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3229= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2828)); int _temp3230= lvs; int _temp3231=
largs; xprintf("struct %.*s expects %d type arguments but was given %d",
_temp3229.last_plus_one - _temp3229.curr, _temp3229.curr, _temp3230, _temp3231);}));}
for( 0; _temp2826 != 0;( _temp2826=(( struct Cyc_List_List*) _check_null(
_temp2826))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2826))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2698;}} _LL2732: if( _temp2835 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("missing union name", sizeof( unsigned char), 19u)); return cvtenv;}
if(* _temp2832 == 0){ struct _handler_cons _temp3232; _push_handler(& _temp3232);{
int _temp3234= 0; if( setjmp( _temp3232.handler)){ _temp3234= 1;} if( !
_temp3234){* _temp2832=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*) _check_null( _temp2835));; _pop_handler();} else{
void* _temp3233=( void*) _exn_thrown; void* _temp3236= _temp3233; _LL3238: if(
_temp3236 == Cyc_Dict_Absent){ goto _LL3239;} else{ goto _LL3240;} _LL3240: goto
_LL3241; _LL3239: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp3242= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp2835)); xprintf("unbound type union %.*s", _temp3242.last_plus_one -
_temp3242.curr, _temp3242.curr);})); return cvtenv; _LL3241:( void) _throw(
_temp3236); _LL3237:;}}}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null(* _temp2832)); if( ud->name != 0){*(( struct _tuple1*) _check_null(
_temp2835))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2833); if( lvs != largs){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3243= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2835)); int _temp3244= lvs; int _temp3245=
largs; xprintf("union %.*s expects %d type arguments but was given %d",
_temp3243.last_plus_one - _temp3243.curr, _temp3243.curr, _temp3244, _temp3245);}));}
for( 0; _temp2833 != 0;( _temp2833=(( struct Cyc_List_List*) _check_null(
_temp2833))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2833))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2698;}} _LL2734: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp3246; _push_handler(& _temp3246);{ int _temp3248=
0; if( setjmp( _temp3246.handler)){ _temp3248= 1;} if( ! _temp3248){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2842);; _pop_handler();} else{ void* _temp3247=( void*)
_exn_thrown; void* _temp3250= _temp3247; _LL3252: if( _temp3250 == Cyc_Dict_Absent){
goto _LL3253;} else{ goto _LL3254;} _LL3254: goto _LL3255; _LL3253: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp3256= Cyc_Absynpp_qvar2string(
_temp2842); xprintf("unbound typedef name %.*s", _temp3256.last_plus_one -
_temp3256.curr, _temp3256.curr);})); return cvtenv; _LL3255:( void) _throw(
_temp3250); _LL3251:;}}} _temp2842[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2840; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); inst=({
struct Cyc_List_List* _temp3257=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3257->hd=( void*)({ struct _tuple5* _temp3258=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp3258->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; _temp3258->f2=( void*)(( struct
Cyc_List_List*) _check_null( ts))->hd; _temp3258;}); _temp3257->tl= inst;
_temp3257;});} if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp3259= Cyc_Absynpp_qvar2string( _temp2842);
xprintf("typedef %.*s expects a different number of arguments", _temp3259.last_plus_one
- _temp3259.curr, _temp3259.curr);})); return cvtenv;}{ void* new_typ= Cyc_Tcutil_substitute(
inst,( void*) td->defn);* _temp2839=({ struct Cyc_Core_Opt* _temp3260=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3260->v=( void*)
new_typ; _temp3260;}); goto _LL2698;}}} _LL2736: goto _LL2698; _LL2738:
_temp2846= _temp2844; goto _LL2740; _LL2740: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2846); goto _LL2698; _LL2742:
for( 0; _temp2848 != 0; _temp2848=(( struct Cyc_List_List*) _check_null(
_temp2848))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*) _check_null( _temp2848))->hd);}
goto _LL2698; _LL2698:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t),
expected_kind)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp3261= Cyc_Absynpp_typ2string( t); struct _tagged_arr _temp3262= Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind( t)); struct _tagged_arr _temp3263= Cyc_Absynpp_kind2string(
expected_kind); xprintf("type %.*s has kind %.*s but as used here needs kind %.*s",
_temp3261.last_plus_one - _temp3261.curr, _temp3261.curr, _temp3262.last_plus_one
- _temp3262.curr, _temp3262.curr, _temp3263.last_plus_one - _temp3263.curr,
_temp3263.curr);}));} return cvtenv;} struct Cyc_List_List* Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, void* t){ struct Cyc_Tcutil_CVTEnv _temp3265= Cyc_Tcutil_i_check_valid_type(
loc, te,({ struct Cyc_Tcutil_CVTEnv _temp3264; _temp3264.kind_env= kind_env;
_temp3264.free_vars= 0; _temp3264.free_evars= 0; _temp3264;}), expected_kind, t);
goto _LL3266; _LL3266:{ struct Cyc_List_List* vs= _temp3265.free_vars; for( 0;
vs != 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ Cyc_Tcutil_fast_add_free_tvar(
kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( vs))->hd);}}{
struct Cyc_List_List* evs= _temp3265.free_evars; for( 0; evs != 0; evs=(( struct
Cyc_List_List*) _check_null( evs))->tl){ void* _temp3267= Cyc_Tcutil_compress((
void*)(( struct Cyc_List_List*) _check_null( evs))->hd); struct Cyc_Core_Opt*
_temp3273; struct Cyc_Core_Opt** _temp3275; int _temp3276; struct Cyc_Core_Opt*
_temp3278; struct Cyc_Core_Opt* _temp3280; _LL3269: if(( unsigned int) _temp3267
> 4u?*(( int*) _temp3267) == Cyc_Absyn_Evar: 0){ _LL3281: _temp3280=(( struct
Cyc_Absyn_Evar_struct*) _temp3267)->f1; goto _LL3279; _LL3279: _temp3278=((
struct Cyc_Absyn_Evar_struct*) _temp3267)->f2; goto _LL3277; _LL3277: _temp3276=((
struct Cyc_Absyn_Evar_struct*) _temp3267)->f3; goto _LL3274; _LL3274: _temp3273=((
struct Cyc_Absyn_Evar_struct*) _temp3267)->f4; _temp3275=&(( struct Cyc_Absyn_Evar_struct*)
_temp3267)->f4; goto _LL3270;} else{ goto _LL3271;} _LL3271: goto _LL3272;
_LL3270: if(* _temp3275 == 0){* _temp3275=({ struct Cyc_Core_Opt* _temp3282=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3282->v=(
void*) kind_env; _temp3282;});} else{ struct Cyc_List_List* _temp3283=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp3275))->v; goto
_LL3284; _LL3284: { struct Cyc_List_List* _temp3285= 0; goto _LL3286; _LL3286:
for( 0; _temp3283 != 0; _temp3283=(( struct Cyc_List_List*) _check_null(
_temp3283))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct
Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3283))->hd)){ _temp3285=({ struct Cyc_List_List* _temp3287=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3287->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3283))->hd);
_temp3287->tl= _temp3285; _temp3287;});}}* _temp3275=({ struct Cyc_Core_Opt*
_temp3288=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3288->v=( void*) _temp3285; _temp3288;});}} goto _LL3268; _LL3272: goto
_LL3268; _LL3268:;}} return _temp3265.free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* _temp3289= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t); goto _LL3290; _LL3290:{ struct Cyc_List_List* x= _temp3289; for( 0; x != 0;
x=(( struct Cyc_List_List*) _check_null( x))->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( x))->hd)->kind);
void* _temp3291=( void*) c->v; void* _temp3299; _LL3293: if( _temp3291 ==( void*)
Cyc_Absyn_No_constr){ goto _LL3294;} else{ goto _LL3295;} _LL3295: if((
unsigned int) _temp3291 > 1u?*(( int*) _temp3291) == Cyc_Absyn_Eq_constr: 0){
_LL3300: _temp3299=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3291)->f1;
if( _temp3299 ==( void*) Cyc_Absyn_MemKind){ goto _LL3296;} else{ goto _LL3297;}}
else{ goto _LL3297;} _LL3297: goto _LL3298; _LL3294: goto _LL3296; _LL3296:(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3301=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3301[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3302; _temp3302.tag= Cyc_Absyn_Eq_constr;
_temp3302.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3302;}); _temp3301;})));
goto _LL3292; _LL3298: goto _LL3292; _LL3292:;}} if( _temp3289 != 0){ void*
_temp3303= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp3309; struct
Cyc_List_List* _temp3311; struct Cyc_List_List* _temp3313; struct Cyc_Absyn_VarargInfo*
_temp3315; int _temp3317; struct Cyc_List_List* _temp3319; void* _temp3321;
struct Cyc_Core_Opt* _temp3323; struct Cyc_List_List* _temp3325; struct Cyc_List_List**
_temp3327; _LL3305: if(( unsigned int) _temp3303 > 4u?*(( int*) _temp3303) ==
Cyc_Absyn_FnType: 0){ _LL3310: _temp3309=(( struct Cyc_Absyn_FnType_struct*)
_temp3303)->f1; _LL3326: _temp3325= _temp3309.tvars; _temp3327=&((( struct Cyc_Absyn_FnType_struct*)
_temp3303)->f1).tvars; goto _LL3324; _LL3324: _temp3323= _temp3309.effect; goto
_LL3322; _LL3322: _temp3321=( void*) _temp3309.ret_typ; goto _LL3320; _LL3320:
_temp3319= _temp3309.args; goto _LL3318; _LL3318: _temp3317= _temp3309.c_varargs;
goto _LL3316; _LL3316: _temp3315= _temp3309.cyc_varargs; goto _LL3314; _LL3314:
_temp3313= _temp3309.rgn_po; goto _LL3312; _LL3312: _temp3311= _temp3309.attributes;
goto _LL3306;} else{ goto _LL3307;} _LL3307: goto _LL3308; _LL3306: if(*
_temp3327 == 0){* _temp3327= _temp3289;} else{ if( _temp3289 != 0){ struct
_tagged_arr _temp3328=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3289))->hd)->name; goto _LL3329; _LL3329: if(*(( const
unsigned char*) _check_unknown_subscript( _temp3328, sizeof( unsigned char), 0))
=='?'){ Cyc_Tcutil_terr( loc,( struct _tagged_arr) xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3330=
_temp3328; xprintf("unbound type variable %.*s", _temp3330.last_plus_one -
_temp3330.curr, _temp3330.curr);}));}}} goto _LL3304; _LL3308: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp3331=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp3289))->hd)->name; xprintf("unbound type variable %.*s",
_temp3331.last_plus_one - _temp3331.curr, _temp3331.curr);})); return; _LL3304:;}}
void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar* tv){ if( tv->identity
== 0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
_temp3332= Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, t);
goto _LL3333; _LL3333: { struct Cyc_List_List* _temp3334= Cyc_Tcutil_remove_bound_tvars(
_temp3332, bound_tvars); goto _LL3335; _LL3335: { struct Cyc_List_List* fs=
_temp3334; for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct _tagged_arr* _temp3336=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->name; goto _LL3337; _LL3337: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp3338=* _temp3336; struct
_tagged_arr _temp3339= Cyc_Absynpp_typ2string( t); xprintf("unbound type variable %.*s in type %.*s",
_temp3338.last_plus_one - _temp3338.curr, _temp3338.curr, _temp3339.last_plus_one
- _temp3339.curr, _temp3339.curr);}));}}}} static void Cyc_Tcutil_check_unique_unsorted(
int(* cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr(* a2string)( void*), struct _tagged_arr msg){ for( 0; vs
!= 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=((
struct Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3340=
msg; struct _tagged_arr _temp3341= a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd); xprintf("%.*s: %.*s", _temp3340.last_plus_one -
_temp3340.curr, _temp3340.curr, _temp3341.last_plus_one - _temp3341.curr,
_temp3341.curr);}));}}}} static struct _tagged_arr Cyc_Tcutil_strptr2string(
struct _tagged_arr* s){ return* s;} void Cyc_Tcutil_check_unique_vars( struct
Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr msg){((
void(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
vs, struct Cyc_Position_Segment* loc, struct _tagged_arr(* a2string)( struct
_tagged_arr*), struct _tagged_arr msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_String_zstrptrcmp,
vs, loc, Cyc_Tcutil_strptr2string, msg);} void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs){(( void(*)( int(*
cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_arr(* a2string)( struct Cyc_Absyn_Tvar*),
struct _tagged_arr msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp,
tvs, loc, Cyc_Tcutil_tvar2string, _tag_arr("duplicate type variable", sizeof(
unsigned char), 24u));} struct _tuple17{ struct Cyc_Absyn_Structfield* f1; int
f2; } ; struct _tuple18{ struct Cyc_List_List* f1; void* f2; } ; struct _tuple19{
struct Cyc_Absyn_Structfield* f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_String_strcmp(*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( sd_fields))->hd)->name,
_tag_arr("", sizeof( unsigned char), 1u)) != 0){ fields=({ struct Cyc_List_List*
_temp3342=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3342->hd=( void*)({ struct _tuple17* _temp3343=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp3343->f1=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( sd_fields))->hd; _temp3343->f2= 0; _temp3343;});
_temp3342->tl= fields; _temp3342;});}}} fields=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans=
0; for( 0; des != 0; des=(( struct Cyc_List_List*) _check_null( des))->tl){
struct _tuple18 _temp3346; void* _temp3347; struct Cyc_List_List* _temp3349;
struct _tuple18* _temp3344=( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd; _temp3346=* _temp3344; _LL3350: _temp3349= _temp3346.f1;
goto _LL3348; _LL3348: _temp3347= _temp3346.f2; goto _LL3345; _LL3345: if(
_temp3349 == 0){ struct Cyc_List_List* _temp3351= fields; goto _LL3352; _LL3352:
for( 0; _temp3351 != 0; _temp3351=(( struct Cyc_List_List*) _check_null(
_temp3351))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3351))->hd)).f2){(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3351))->hd)).f2= 1;(*(( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1=({ struct Cyc_List_List* _temp3353=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3353->hd=( void*)(( void*)({
struct Cyc_Absyn_FieldName_struct* _temp3354=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp3354[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp3355; _temp3355.tag= Cyc_Absyn_FieldName;
_temp3355.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3351))->hd)).f1)->name; _temp3355;}); _temp3354;})); _temp3353->tl= 0;
_temp3353;}); ans=({ struct Cyc_List_List* _temp3356=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3356->hd=( void*)({
struct _tuple19* _temp3357=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3357->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3351))->hd)).f1; _temp3357->f2= _temp3347; _temp3357;});
_temp3356->tl= ans; _temp3356;}); break;}} if( _temp3351 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments to struct", sizeof( unsigned char), 29u));}}
else{ if((( struct Cyc_List_List*) _check_null( _temp3349))->tl != 0){ Cyc_Tcutil_terr(
loc, _tag_arr("multiple designators are not supported", sizeof( unsigned char),
39u));} else{ void* _temp3358=( void*)(( struct Cyc_List_List*) _check_null(
_temp3349))->hd; struct Cyc_Absyn_Exp* _temp3364; struct _tagged_arr* _temp3366;
_LL3360: if(*(( int*) _temp3358) == Cyc_Absyn_ArrayElement){ _LL3365: _temp3364=((
struct Cyc_Absyn_ArrayElement_struct*) _temp3358)->f1; goto _LL3361;} else{ goto
_LL3362;} _LL3362: if(*(( int*) _temp3358) == Cyc_Absyn_FieldName){ _LL3367:
_temp3366=(( struct Cyc_Absyn_FieldName_struct*) _temp3358)->f1; goto _LL3363;}
else{ goto _LL3359;} _LL3361: Cyc_Tcutil_terr( loc, _tag_arr("array designator used in argument to struct",
sizeof( unsigned char), 44u)); goto _LL3359; _LL3363: { struct Cyc_List_List*
_temp3368= fields; goto _LL3369; _LL3369: for( 0; _temp3368 != 0; _temp3368=((
struct Cyc_List_List*) _check_null( _temp3368))->tl){ if( Cyc_String_zstrptrcmp(
_temp3366,((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3368))->hd)).f1)->name) == 0){ if((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3368))->hd)).f2){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp3370=* _temp3366; xprintf("field %.*s has already been used as an argument",
_temp3370.last_plus_one - _temp3370.curr, _temp3370.curr);}));}(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp3368))->hd)).f2= 1; ans=({
struct Cyc_List_List* _temp3371=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp3371->hd=( void*)({ struct _tuple19*
_temp3372=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3372->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3368))->hd)).f1; _temp3372->f2= _temp3347; _temp3372;}); _temp3371->tl= ans;
_temp3371;}); break;}} if( _temp3368 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp3373=* _temp3366; xprintf("bad field designator %.*s",
_temp3373.last_plus_one - _temp3373.curr, _temp3373.curr);}));} goto _LL3359;}
_LL3359:;}}} for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
fields))->hd)).f2){ Cyc_Tcutil_terr( loc, _tag_arr("too few arguments to struct",
sizeof( unsigned char), 28u)); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ(
void* t){ void* _temp3374= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3380; struct Cyc_Absyn_Conref* _temp3382; struct Cyc_Absyn_Tqual _temp3384;
struct Cyc_Absyn_Conref* _temp3386; void* _temp3388; void* _temp3390; _LL3376:
if(( unsigned int) _temp3374 > 4u?*(( int*) _temp3374) == Cyc_Absyn_PointerType:
0){ _LL3381: _temp3380=(( struct Cyc_Absyn_PointerType_struct*) _temp3374)->f1;
_LL3391: _temp3390=( void*) _temp3380.elt_typ; goto _LL3389; _LL3389: _temp3388=(
void*) _temp3380.rgn_typ; goto _LL3387; _LL3387: _temp3386= _temp3380.nullable;
goto _LL3385; _LL3385: _temp3384= _temp3380.tq; goto _LL3383; _LL3383: _temp3382=
_temp3380.bounds; goto _LL3377;} else{ goto _LL3378;} _LL3378: goto _LL3379;
_LL3377: { struct Cyc_Absyn_Conref* _temp3392=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3382); goto
_LL3393; _LL3393: { void* _temp3394=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3392))->v; void*
_temp3402; _LL3396: if(( unsigned int) _temp3394 > 1u?*(( int*) _temp3394) ==
Cyc_Absyn_Eq_constr: 0){ _LL3403: _temp3402=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3394)->f1; if( _temp3402 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3397;}
else{ goto _LL3398;}} else{ goto _LL3398;} _LL3398: if( _temp3394 ==( void*) Cyc_Absyn_No_constr){
goto _LL3399;} else{ goto _LL3400;} _LL3400: goto _LL3401; _LL3397: return 1;
_LL3399:( void*)( _temp3392->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3404=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3404[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3405; _temp3405.tag= Cyc_Absyn_Eq_constr;
_temp3405.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3405;}); _temp3404;})));
return 1; _LL3401: return 0; _LL3395:;}} _LL3379: return 0; _LL3375:;} struct
_tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3642; _temp3642.f1= 0;
_temp3642.f2=( void*) Cyc_Absyn_HeapRgn; _temp3642;}); void* _temp3406=( void*)
e->r; void* _temp3420; struct _tuple1* _temp3422; struct _tagged_arr* _temp3424;
struct Cyc_Absyn_Exp* _temp3426; struct _tagged_arr* _temp3428; struct Cyc_Absyn_Exp*
_temp3430; struct Cyc_Absyn_Exp* _temp3432; struct Cyc_Absyn_Exp* _temp3434;
struct Cyc_Absyn_Exp* _temp3436; _LL3408: if(*(( int*) _temp3406) == Cyc_Absyn_Var_e){
_LL3423: _temp3422=(( struct Cyc_Absyn_Var_e_struct*) _temp3406)->f1; goto
_LL3421; _LL3421: _temp3420=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp3406)->f2;
goto _LL3409;} else{ goto _LL3410;} _LL3410: if(*(( int*) _temp3406) == Cyc_Absyn_StructMember_e){
_LL3427: _temp3426=(( struct Cyc_Absyn_StructMember_e_struct*) _temp3406)->f1;
goto _LL3425; _LL3425: _temp3424=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3406)->f2; goto _LL3411;} else{ goto _LL3412;} _LL3412: if(*(( int*)
_temp3406) == Cyc_Absyn_StructArrow_e){ _LL3431: _temp3430=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3406)->f1; goto _LL3429; _LL3429: _temp3428=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3406)->f2; goto _LL3413;} else{ goto _LL3414;} _LL3414: if(*(( int*)
_temp3406) == Cyc_Absyn_Deref_e){ _LL3433: _temp3432=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3406)->f1; goto _LL3415;} else{ goto _LL3416;} _LL3416: if(*(( int*)
_temp3406) == Cyc_Absyn_Subscript_e){ _LL3437: _temp3436=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3406)->f1; goto _LL3435; _LL3435: _temp3434=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3406)->f2; goto _LL3417;} else{ goto _LL3418;} _LL3418: goto _LL3419;
_LL3409: { void* _temp3438= _temp3420; struct Cyc_Absyn_Fndecl* _temp3452;
struct Cyc_Absyn_Vardecl* _temp3454; struct Cyc_Absyn_Vardecl* _temp3456; struct
Cyc_Absyn_Vardecl* _temp3458; struct Cyc_Absyn_Vardecl* _temp3460; _LL3440: if(
_temp3438 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL3441;} else{ goto _LL3442;}
_LL3442: if(( unsigned int) _temp3438 > 1u?*(( int*) _temp3438) == Cyc_Absyn_Funname_b:
0){ _LL3453: _temp3452=(( struct Cyc_Absyn_Funname_b_struct*) _temp3438)->f1;
goto _LL3443;} else{ goto _LL3444;} _LL3444: if(( unsigned int) _temp3438 > 1u?*((
int*) _temp3438) == Cyc_Absyn_Global_b: 0){ _LL3455: _temp3454=(( struct Cyc_Absyn_Global_b_struct*)
_temp3438)->f1; goto _LL3445;} else{ goto _LL3446;} _LL3446: if(( unsigned int)
_temp3438 > 1u?*(( int*) _temp3438) == Cyc_Absyn_Local_b: 0){ _LL3457: _temp3456=((
struct Cyc_Absyn_Local_b_struct*) _temp3438)->f1; goto _LL3447;} else{ goto
_LL3448;} _LL3448: if(( unsigned int) _temp3438 > 1u?*(( int*) _temp3438) == Cyc_Absyn_Pat_b:
0){ _LL3459: _temp3458=(( struct Cyc_Absyn_Pat_b_struct*) _temp3438)->f1; goto
_LL3449;} else{ goto _LL3450;} _LL3450: if(( unsigned int) _temp3438 > 1u?*((
int*) _temp3438) == Cyc_Absyn_Param_b: 0){ _LL3461: _temp3460=(( struct Cyc_Absyn_Param_b_struct*)
_temp3438)->f1; goto _LL3451;} else{ goto _LL3439;} _LL3441: return bogus_ans;
_LL3443: return({ struct _tuple7 _temp3462; _temp3462.f1= 1; _temp3462.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3462;}); _LL3445: { void* _temp3463= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3469; struct Cyc_Absyn_Tqual _temp3471; void* _temp3473; _LL3465: if((
unsigned int) _temp3463 > 4u?*(( int*) _temp3463) == Cyc_Absyn_ArrayType: 0){
_LL3474: _temp3473=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3463)->f1;
goto _LL3472; _LL3472: _temp3471=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3463)->f2; goto _LL3470; _LL3470: _temp3469=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3463)->f3; goto _LL3466;} else{ goto _LL3467;} _LL3467: goto _LL3468;
_LL3466: return({ struct _tuple7 _temp3475; _temp3475.f1= 1; _temp3475.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3475;}); _LL3468: return({ struct _tuple7 _temp3476;
_temp3476.f1=( _temp3454->tq).q_const; _temp3476.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3476;}); _LL3464:;} _LL3447: { void* _temp3477= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3483; struct Cyc_Absyn_Tqual _temp3485; void* _temp3487; _LL3479: if((
unsigned int) _temp3477 > 4u?*(( int*) _temp3477) == Cyc_Absyn_ArrayType: 0){
_LL3488: _temp3487=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3477)->f1;
goto _LL3486; _LL3486: _temp3485=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3477)->f2; goto _LL3484; _LL3484: _temp3483=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3477)->f3; goto _LL3480;} else{ goto _LL3481;} _LL3481: goto _LL3482;
_LL3480: return({ struct _tuple7 _temp3489; _temp3489.f1= 1; _temp3489.f2=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3456->rgn))->v; _temp3489;}); _LL3482:
return({ struct _tuple7 _temp3490; _temp3490.f1=( _temp3456->tq).q_const;
_temp3490.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3456->rgn))->v;
_temp3490;}); _LL3478:;} _LL3449: _temp3460= _temp3458; goto _LL3451; _LL3451:
return({ struct _tuple7 _temp3491; _temp3491.f1=( _temp3460->tq).q_const;
_temp3491.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3460->rgn))->v;
_temp3491;}); _LL3439:;} _LL3411: { void* _temp3492= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3426->topt))->v); struct Cyc_List_List*
_temp3504; struct Cyc_List_List* _temp3506; struct Cyc_Absyn_Structdecl**
_temp3508; struct Cyc_Absyn_Structdecl* _temp3510; struct Cyc_List_List*
_temp3511; struct _tuple1* _temp3513; struct Cyc_Absyn_Uniondecl** _temp3515;
struct Cyc_Absyn_Uniondecl* _temp3517; struct Cyc_List_List* _temp3518; struct
_tuple1* _temp3520; _LL3494: if(( unsigned int) _temp3492 > 4u?*(( int*)
_temp3492) == Cyc_Absyn_AnonStructType: 0){ _LL3505: _temp3504=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3492)->f1; goto _LL3495;} else{ goto _LL3496;} _LL3496: if(( unsigned int)
_temp3492 > 4u?*(( int*) _temp3492) == Cyc_Absyn_AnonUnionType: 0){ _LL3507:
_temp3506=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3492)->f1; goto
_LL3497;} else{ goto _LL3498;} _LL3498: if(( unsigned int) _temp3492 > 4u?*((
int*) _temp3492) == Cyc_Absyn_StructType: 0){ _LL3514: _temp3513=(( struct Cyc_Absyn_StructType_struct*)
_temp3492)->f1; goto _LL3512; _LL3512: _temp3511=(( struct Cyc_Absyn_StructType_struct*)
_temp3492)->f2; goto _LL3509; _LL3509: _temp3508=(( struct Cyc_Absyn_StructType_struct*)
_temp3492)->f3; if( _temp3508 == 0){ goto _LL3500;} else{ _temp3510=* _temp3508;
goto _LL3499;}} else{ goto _LL3500;} _LL3500: if(( unsigned int) _temp3492 > 4u?*((
int*) _temp3492) == Cyc_Absyn_UnionType: 0){ _LL3521: _temp3520=(( struct Cyc_Absyn_UnionType_struct*)
_temp3492)->f1; goto _LL3519; _LL3519: _temp3518=(( struct Cyc_Absyn_UnionType_struct*)
_temp3492)->f2; goto _LL3516; _LL3516: _temp3515=(( struct Cyc_Absyn_UnionType_struct*)
_temp3492)->f3; if( _temp3515 == 0){ goto _LL3502;} else{ _temp3517=* _temp3515;
goto _LL3501;}} else{ goto _LL3502;} _LL3502: goto _LL3503; _LL3495: _temp3506=
_temp3504; goto _LL3497; _LL3497: { struct Cyc_Absyn_Structfield* _temp3522= Cyc_Absyn_lookup_field(
_temp3506, _temp3424); goto _LL3523; _LL3523: if( _temp3522 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3522))->width != 0: 0){ return({ struct _tuple7 _temp3524;
_temp3524.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3522))->tq).q_const;
_temp3524.f2=( Cyc_Tcutil_addressof_props( te, _temp3426)).f2; _temp3524;});}
return bogus_ans;} _LL3499: { struct Cyc_Absyn_Structfield* _temp3525= Cyc_Absyn_lookup_struct_field(
_temp3510, _temp3424); goto _LL3526; _LL3526: if( _temp3525 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3525))->width != 0: 0){ return({ struct _tuple7 _temp3527;
_temp3527.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3525))->tq).q_const;
_temp3527.f2=( Cyc_Tcutil_addressof_props( te, _temp3426)).f2; _temp3527;});}
return bogus_ans;} _LL3501: { struct Cyc_Absyn_Structfield* _temp3528= Cyc_Absyn_lookup_union_field(
_temp3517, _temp3424); goto _LL3529; _LL3529: if( _temp3528 != 0){ return({
struct _tuple7 _temp3530; _temp3530.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3528))->tq).q_const; _temp3530.f2=( Cyc_Tcutil_addressof_props(
te, _temp3426)).f2; _temp3530;});} goto _LL3503;} _LL3503: return bogus_ans;
_LL3493:;} _LL3413: { void* _temp3531= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3430->topt))->v); struct Cyc_Absyn_PtrInfo _temp3537; struct
Cyc_Absyn_Conref* _temp3539; struct Cyc_Absyn_Tqual _temp3541; struct Cyc_Absyn_Conref*
_temp3543; void* _temp3545; void* _temp3547; _LL3533: if(( unsigned int)
_temp3531 > 4u?*(( int*) _temp3531) == Cyc_Absyn_PointerType: 0){ _LL3538:
_temp3537=(( struct Cyc_Absyn_PointerType_struct*) _temp3531)->f1; _LL3548:
_temp3547=( void*) _temp3537.elt_typ; goto _LL3546; _LL3546: _temp3545=( void*)
_temp3537.rgn_typ; goto _LL3544; _LL3544: _temp3543= _temp3537.nullable; goto
_LL3542; _LL3542: _temp3541= _temp3537.tq; goto _LL3540; _LL3540: _temp3539=
_temp3537.bounds; goto _LL3534;} else{ goto _LL3535;} _LL3535: goto _LL3536;
_LL3534: { void* _temp3549= Cyc_Tcutil_compress( _temp3547); struct Cyc_List_List*
_temp3561; struct Cyc_List_List* _temp3563; struct Cyc_Absyn_Structdecl**
_temp3565; struct Cyc_Absyn_Structdecl* _temp3567; struct Cyc_List_List*
_temp3568; struct _tuple1* _temp3570; struct Cyc_Absyn_Uniondecl** _temp3572;
struct Cyc_Absyn_Uniondecl* _temp3574; struct Cyc_List_List* _temp3575; struct
_tuple1* _temp3577; _LL3551: if(( unsigned int) _temp3549 > 4u?*(( int*)
_temp3549) == Cyc_Absyn_AnonStructType: 0){ _LL3562: _temp3561=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3549)->f1; goto _LL3552;} else{ goto _LL3553;} _LL3553: if(( unsigned int)
_temp3549 > 4u?*(( int*) _temp3549) == Cyc_Absyn_AnonUnionType: 0){ _LL3564:
_temp3563=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3549)->f1; goto
_LL3554;} else{ goto _LL3555;} _LL3555: if(( unsigned int) _temp3549 > 4u?*((
int*) _temp3549) == Cyc_Absyn_StructType: 0){ _LL3571: _temp3570=(( struct Cyc_Absyn_StructType_struct*)
_temp3549)->f1; goto _LL3569; _LL3569: _temp3568=(( struct Cyc_Absyn_StructType_struct*)
_temp3549)->f2; goto _LL3566; _LL3566: _temp3565=(( struct Cyc_Absyn_StructType_struct*)
_temp3549)->f3; if( _temp3565 == 0){ goto _LL3557;} else{ _temp3567=* _temp3565;
goto _LL3556;}} else{ goto _LL3557;} _LL3557: if(( unsigned int) _temp3549 > 4u?*((
int*) _temp3549) == Cyc_Absyn_UnionType: 0){ _LL3578: _temp3577=(( struct Cyc_Absyn_UnionType_struct*)
_temp3549)->f1; goto _LL3576; _LL3576: _temp3575=(( struct Cyc_Absyn_UnionType_struct*)
_temp3549)->f2; goto _LL3573; _LL3573: _temp3572=(( struct Cyc_Absyn_UnionType_struct*)
_temp3549)->f3; if( _temp3572 == 0){ goto _LL3559;} else{ _temp3574=* _temp3572;
goto _LL3558;}} else{ goto _LL3559;} _LL3559: goto _LL3560; _LL3552: _temp3563=
_temp3561; goto _LL3554; _LL3554: { struct Cyc_Absyn_Structfield* _temp3579= Cyc_Absyn_lookup_field(
_temp3563, _temp3428); goto _LL3580; _LL3580: if( _temp3579 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3579))->width != 0: 0){ return({ struct _tuple7 _temp3581;
_temp3581.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3579))->tq).q_const;
_temp3581.f2= _temp3545; _temp3581;});} return bogus_ans;} _LL3556: { struct Cyc_Absyn_Structfield*
_temp3582= Cyc_Absyn_lookup_struct_field( _temp3567, _temp3428); goto _LL3583;
_LL3583: if( _temp3582 != 0?(( struct Cyc_Absyn_Structfield*) _check_null(
_temp3582))->width != 0: 0){ return({ struct _tuple7 _temp3584; _temp3584.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3582))->tq).q_const; _temp3584.f2=
_temp3545; _temp3584;});} return bogus_ans;} _LL3558: { struct Cyc_Absyn_Structfield*
_temp3585= Cyc_Absyn_lookup_union_field( _temp3574, _temp3428); goto _LL3586;
_LL3586: if( _temp3585 != 0){ return({ struct _tuple7 _temp3587; _temp3587.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3585))->tq).q_const; _temp3587.f2=
_temp3545; _temp3587;});} return bogus_ans;} _LL3560: return bogus_ans; _LL3550:;}
_LL3536: return bogus_ans; _LL3532:;} _LL3415: { void* _temp3588= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp3432->topt))->v); struct Cyc_Absyn_PtrInfo
_temp3594; struct Cyc_Absyn_Conref* _temp3596; struct Cyc_Absyn_Tqual _temp3598;
struct Cyc_Absyn_Conref* _temp3600; void* _temp3602; void* _temp3604; _LL3590:
if(( unsigned int) _temp3588 > 4u?*(( int*) _temp3588) == Cyc_Absyn_PointerType:
0){ _LL3595: _temp3594=(( struct Cyc_Absyn_PointerType_struct*) _temp3588)->f1;
_LL3605: _temp3604=( void*) _temp3594.elt_typ; goto _LL3603; _LL3603: _temp3602=(
void*) _temp3594.rgn_typ; goto _LL3601; _LL3601: _temp3600= _temp3594.nullable;
goto _LL3599; _LL3599: _temp3598= _temp3594.tq; goto _LL3597; _LL3597: _temp3596=
_temp3594.bounds; goto _LL3591;} else{ goto _LL3592;} _LL3592: goto _LL3593;
_LL3591: return({ struct _tuple7 _temp3606; _temp3606.f1= _temp3598.q_const;
_temp3606.f2= _temp3602; _temp3606;}); _LL3593: return bogus_ans; _LL3589:;}
_LL3417: { void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3436->topt))->v); void* _temp3607= t; struct Cyc_Absyn_Exp*
_temp3617; struct Cyc_Absyn_Tqual _temp3619; void* _temp3621; struct Cyc_List_List*
_temp3623; struct Cyc_Absyn_PtrInfo _temp3625; struct Cyc_Absyn_Conref*
_temp3627; struct Cyc_Absyn_Tqual _temp3629; struct Cyc_Absyn_Conref* _temp3631;
void* _temp3633; void* _temp3635; _LL3609: if(( unsigned int) _temp3607 > 4u?*((
int*) _temp3607) == Cyc_Absyn_ArrayType: 0){ _LL3622: _temp3621=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3607)->f1; goto _LL3620; _LL3620:
_temp3619=(( struct Cyc_Absyn_ArrayType_struct*) _temp3607)->f2; goto _LL3618;
_LL3618: _temp3617=(( struct Cyc_Absyn_ArrayType_struct*) _temp3607)->f3; goto
_LL3610;} else{ goto _LL3611;} _LL3611: if(( unsigned int) _temp3607 > 4u?*((
int*) _temp3607) == Cyc_Absyn_TupleType: 0){ _LL3624: _temp3623=(( struct Cyc_Absyn_TupleType_struct*)
_temp3607)->f1; goto _LL3612;} else{ goto _LL3613;} _LL3613: if(( unsigned int)
_temp3607 > 4u?*(( int*) _temp3607) == Cyc_Absyn_PointerType: 0){ _LL3626:
_temp3625=(( struct Cyc_Absyn_PointerType_struct*) _temp3607)->f1; _LL3636:
_temp3635=( void*) _temp3625.elt_typ; goto _LL3634; _LL3634: _temp3633=( void*)
_temp3625.rgn_typ; goto _LL3632; _LL3632: _temp3631= _temp3625.nullable; goto
_LL3630; _LL3630: _temp3629= _temp3625.tq; goto _LL3628; _LL3628: _temp3627=
_temp3625.bounds; goto _LL3614;} else{ goto _LL3615;} _LL3615: goto _LL3616;
_LL3610: return({ struct _tuple7 _temp3637; _temp3637.f1= _temp3619.q_const;
_temp3637.f2=( Cyc_Tcutil_addressof_props( te, _temp3436)).f2; _temp3637;});
_LL3612: { struct _tuple4* _temp3638= Cyc_Absyn_lookup_tuple_field( _temp3623,(
int) Cyc_Evexp_eval_const_uint_exp( _temp3434)); goto _LL3639; _LL3639: if(
_temp3638 != 0){ return({ struct _tuple7 _temp3640; _temp3640.f1=((*(( struct
_tuple4*) _check_null( _temp3638))).f1).q_const; _temp3640.f2=( Cyc_Tcutil_addressof_props(
te, _temp3436)).f2; _temp3640;});} return bogus_ans;} _LL3614: return({ struct
_tuple7 _temp3641; _temp3641.f1= _temp3629.q_const; _temp3641.f2= _temp3633;
_temp3641;}); _LL3616: return bogus_ans; _LL3608:;} _LL3419: Cyc_Tcutil_terr( e->loc,
_tag_arr("unary & applied to non-lvalue", sizeof( unsigned char), 30u)); return
bogus_ans; _LL3407:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3643= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Exp* _temp3649; struct Cyc_Absyn_Tqual _temp3651; void*
_temp3653; _LL3645: if(( unsigned int) _temp3643 > 4u?*(( int*) _temp3643) ==
Cyc_Absyn_ArrayType: 0){ _LL3654: _temp3653=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3643)->f1; goto _LL3652; _LL3652: _temp3651=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3643)->f2; goto _LL3650; _LL3650: _temp3649=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3643)->f3; goto _LL3646;} else{ goto _LL3647;} _LL3647: goto _LL3648;
_LL3646: { void* _temp3657; int _temp3659; struct _tuple7 _temp3655= Cyc_Tcutil_addressof_props(
te, e); _LL3660: _temp3659= _temp3655.f1; goto _LL3658; _LL3658: _temp3657=
_temp3655.f2; goto _LL3656; _LL3656: return Cyc_Absyn_atb_typ( _temp3653,
_temp3657, _temp3651,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3661=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3661[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3662; _temp3662.tag= Cyc_Absyn_Upper_b;
_temp3662.f1= e; _temp3662;}); _temp3661;}));} _LL3648: return e_typ; _LL3644:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b);{ void* _temp3663=( void*) b->v; void*
_temp3671; void* _temp3673; struct Cyc_Absyn_Exp* _temp3675; _LL3665: if((
unsigned int) _temp3663 > 1u?*(( int*) _temp3663) == Cyc_Absyn_Eq_constr: 0){
_LL3672: _temp3671=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3663)->f1;
if( _temp3671 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3666;} else{ goto _LL3667;}}
else{ goto _LL3667;} _LL3667: if(( unsigned int) _temp3663 > 1u?*(( int*)
_temp3663) == Cyc_Absyn_Eq_constr: 0){ _LL3674: _temp3673=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3663)->f1; if(( unsigned int) _temp3673 > 1u?*(( int*) _temp3673) == Cyc_Absyn_Upper_b:
0){ _LL3676: _temp3675=(( struct Cyc_Absyn_Upper_b_struct*) _temp3673)->f1; goto
_LL3668;} else{ goto _LL3669;}} else{ goto _LL3669;} _LL3669: goto _LL3670;
_LL3666: return; _LL3668: if( Cyc_Evexp_eval_const_uint_exp( _temp3675) <= i){
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u));} return; _LL3670:( void*)( b->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp3677=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3677[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3678; _temp3678.tag= Cyc_Absyn_Eq_constr;
_temp3678.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3679=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3679[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3680; _temp3680.tag= Cyc_Absyn_Upper_b;
_temp3680.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3680;}); _temp3679;}));
_temp3678;}); _temp3677;}))); return; _LL3664:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3681= Cyc_Tcutil_compress(
t); void* _temp3707; void* _temp3709; struct Cyc_Absyn_Enumdecl* _temp3711;
struct _tuple1* _temp3713; struct Cyc_Absyn_Exp* _temp3715; struct Cyc_Absyn_Tqual
_temp3717; void* _temp3719; struct Cyc_List_List* _temp3721; struct Cyc_Absyn_Structdecl**
_temp3723; struct Cyc_List_List* _temp3725; struct _tuple1* _temp3727; struct
Cyc_Absyn_Uniondecl** _temp3729; struct Cyc_List_List* _temp3731; struct _tuple1*
_temp3733; struct Cyc_List_List* _temp3735; struct Cyc_List_List* _temp3737;
_LL3683: if( _temp3681 ==( void*) Cyc_Absyn_VoidType){ goto _LL3684;} else{ goto
_LL3685;} _LL3685: if(( unsigned int) _temp3681 > 4u?*(( int*) _temp3681) == Cyc_Absyn_IntType:
0){ _LL3710: _temp3709=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3681)->f1;
goto _LL3708; _LL3708: _temp3707=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3681)->f2; goto _LL3686;} else{ goto _LL3687;} _LL3687: if( _temp3681 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3688;} else{ goto _LL3689;} _LL3689: if(
_temp3681 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3690;} else{ goto _LL3691;}
_LL3691: if(( unsigned int) _temp3681 > 4u?*(( int*) _temp3681) == Cyc_Absyn_EnumType:
0){ _LL3714: _temp3713=(( struct Cyc_Absyn_EnumType_struct*) _temp3681)->f1;
goto _LL3712; _LL3712: _temp3711=(( struct Cyc_Absyn_EnumType_struct*) _temp3681)->f2;
goto _LL3692;} else{ goto _LL3693;} _LL3693: if(( unsigned int) _temp3681 > 4u?*((
int*) _temp3681) == Cyc_Absyn_ArrayType: 0){ _LL3720: _temp3719=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3681)->f1; goto _LL3718; _LL3718:
_temp3717=(( struct Cyc_Absyn_ArrayType_struct*) _temp3681)->f2; goto _LL3716;
_LL3716: _temp3715=(( struct Cyc_Absyn_ArrayType_struct*) _temp3681)->f3; goto
_LL3694;} else{ goto _LL3695;} _LL3695: if(( unsigned int) _temp3681 > 4u?*((
int*) _temp3681) == Cyc_Absyn_TupleType: 0){ _LL3722: _temp3721=(( struct Cyc_Absyn_TupleType_struct*)
_temp3681)->f1; goto _LL3696;} else{ goto _LL3697;} _LL3697: if(( unsigned int)
_temp3681 > 4u?*(( int*) _temp3681) == Cyc_Absyn_StructType: 0){ _LL3728:
_temp3727=(( struct Cyc_Absyn_StructType_struct*) _temp3681)->f1; goto _LL3726;
_LL3726: _temp3725=(( struct Cyc_Absyn_StructType_struct*) _temp3681)->f2; goto
_LL3724; _LL3724: _temp3723=(( struct Cyc_Absyn_StructType_struct*) _temp3681)->f3;
goto _LL3698;} else{ goto _LL3699;} _LL3699: if(( unsigned int) _temp3681 > 4u?*((
int*) _temp3681) == Cyc_Absyn_UnionType: 0){ _LL3734: _temp3733=(( struct Cyc_Absyn_UnionType_struct*)
_temp3681)->f1; goto _LL3732; _LL3732: _temp3731=(( struct Cyc_Absyn_UnionType_struct*)
_temp3681)->f2; goto _LL3730; _LL3730: _temp3729=(( struct Cyc_Absyn_UnionType_struct*)
_temp3681)->f3; goto _LL3700;} else{ goto _LL3701;} _LL3701: if(( unsigned int)
_temp3681 > 4u?*(( int*) _temp3681) == Cyc_Absyn_AnonStructType: 0){ _LL3736:
_temp3735=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3681)->f1; goto
_LL3702;} else{ goto _LL3703;} _LL3703: if(( unsigned int) _temp3681 > 4u?*((
int*) _temp3681) == Cyc_Absyn_AnonUnionType: 0){ _LL3738: _temp3737=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3681)->f1; goto _LL3704;} else{ goto
_LL3705;} _LL3705: goto _LL3706; _LL3684: return 1; _LL3686: return 1; _LL3688:
return 1; _LL3690: return 1; _LL3692: return 0; _LL3694: return Cyc_Tcutil_bits_only(
_temp3719); _LL3696: for( 0; _temp3721 != 0; _temp3721=(( struct Cyc_List_List*)
_check_null( _temp3721))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3721))->hd)).f2)){ return 0;}} return 1;
_LL3698: if( _temp3723 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3739=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3723)); goto _LL3740; _LL3740:
if( _temp3739->fields == 0){ return 0;}{ struct _RegionHandle _temp3741=
_new_region(); struct _RegionHandle* rgn=& _temp3741; _push_region( rgn);{
struct Cyc_List_List* _temp3742=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3739->tvs, _temp3725); goto _LL3743; _LL3743:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3739->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3742,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3744= 0; _npop_handler( 0u); return
_temp3744;}}}{ int _temp3745= 1; _npop_handler( 0u); return _temp3745;}};
_pop_region(& _temp3741);}} _LL3700: if( _temp3729 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3746=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3729)); goto
_LL3747; _LL3747: if( _temp3746->fields == 0){ return 0;}{ struct _RegionHandle
_temp3748= _new_region(); struct _RegionHandle* rgn=& _temp3748; _push_region(
rgn);{ struct Cyc_List_List* _temp3749=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3746->tvs, _temp3731); goto _LL3750; _LL3750:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3746->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3749,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3751= 0; _npop_handler( 0u); return
_temp3751;}}}{ int _temp3752= 1; _npop_handler( 0u); return _temp3752;}};
_pop_region(& _temp3748);}} _LL3702: _temp3737= _temp3735; goto _LL3704; _LL3704:
for( 0; _temp3737 != 0; _temp3737=(( struct Cyc_List_List*) _check_null(
_temp3737))->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp3737))->hd)->type)){ return 0;}} return
1; _LL3706: return 0; _LL3682:;} struct _tuple20{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3753=( void*) e->r; void*
_temp3793; void* _temp3795; struct Cyc_Absyn_Exp* _temp3797; struct Cyc_Absyn_Enumfield*
_temp3799; struct Cyc_Absyn_Enumdecl* _temp3801; struct _tuple1* _temp3803; void*
_temp3805; struct _tuple1* _temp3807; struct Cyc_Absyn_Exp* _temp3809; struct
Cyc_Absyn_Exp* _temp3811; struct Cyc_Absyn_Exp* _temp3813; struct Cyc_Absyn_Exp*
_temp3815; struct Cyc_Absyn_Exp* _temp3817; struct Cyc_Absyn_Exp* _temp3819;
struct Cyc_List_List* _temp3821; struct Cyc_Absyn_Exp* _temp3823; struct Cyc_Absyn_Exp*
_temp3825; void* _temp3827; struct Cyc_Absyn_Exp* _temp3829; struct Cyc_Absyn_Exp*
_temp3831; struct Cyc_Absyn_Exp* _temp3833; struct Cyc_Absyn_Vardecl* _temp3835;
struct Cyc_List_List* _temp3837; struct Cyc_List_List* _temp3839; void*
_temp3841; struct Cyc_Absyn_Structdecl* _temp3843; struct Cyc_List_List*
_temp3845; struct Cyc_Core_Opt* _temp3847; struct _tuple1* _temp3849; struct Cyc_List_List*
_temp3851; void* _temp3853; struct Cyc_List_List* _temp3855; struct Cyc_Absyn_Tunionfield*
_temp3857; struct Cyc_Absyn_Tuniondecl* _temp3859; struct Cyc_List_List*
_temp3861; struct Cyc_Core_Opt* _temp3863; struct Cyc_Core_Opt* _temp3865;
_LL3755: if(*(( int*) _temp3753) == Cyc_Absyn_Const_e){ _LL3794: _temp3793=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp3753)->f1; goto _LL3756;} else{
goto _LL3757;} _LL3757: if(*(( int*) _temp3753) == Cyc_Absyn_Sizeoftyp_e){
_LL3796: _temp3795=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp3753)->f1;
goto _LL3758;} else{ goto _LL3759;} _LL3759: if(*(( int*) _temp3753) == Cyc_Absyn_Sizeofexp_e){
_LL3798: _temp3797=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp3753)->f1; goto
_LL3760;} else{ goto _LL3761;} _LL3761: if(*(( int*) _temp3753) == Cyc_Absyn_Enum_e){
_LL3804: _temp3803=(( struct Cyc_Absyn_Enum_e_struct*) _temp3753)->f1; goto
_LL3802; _LL3802: _temp3801=(( struct Cyc_Absyn_Enum_e_struct*) _temp3753)->f2;
goto _LL3800; _LL3800: _temp3799=(( struct Cyc_Absyn_Enum_e_struct*) _temp3753)->f3;
goto _LL3762;} else{ goto _LL3763;} _LL3763: if(*(( int*) _temp3753) == Cyc_Absyn_Var_e){
_LL3808: _temp3807=(( struct Cyc_Absyn_Var_e_struct*) _temp3753)->f1; goto
_LL3806; _LL3806: _temp3805=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp3753)->f2;
goto _LL3764;} else{ goto _LL3765;} _LL3765: if(*(( int*) _temp3753) == Cyc_Absyn_Conditional_e){
_LL3814: _temp3813=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3753)->f1;
goto _LL3812; _LL3812: _temp3811=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3753)->f2; goto _LL3810; _LL3810: _temp3809=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3753)->f3; goto _LL3766;} else{ goto _LL3767;} _LL3767: if(*(( int*)
_temp3753) == Cyc_Absyn_SeqExp_e){ _LL3818: _temp3817=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3753)->f1; goto _LL3816; _LL3816: _temp3815=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3753)->f2; goto _LL3768;} else{ goto _LL3769;} _LL3769: if(*(( int*)
_temp3753) == Cyc_Absyn_NoInstantiate_e){ _LL3820: _temp3819=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3753)->f1; goto _LL3770;} else{ goto _LL3771;} _LL3771: if(*(( int*)
_temp3753) == Cyc_Absyn_Instantiate_e){ _LL3824: _temp3823=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3753)->f1; goto _LL3822; _LL3822: _temp3821=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3753)->f2; goto _LL3772;} else{ goto _LL3773;} _LL3773: if(*(( int*)
_temp3753) == Cyc_Absyn_Cast_e){ _LL3828: _temp3827=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3753)->f1; goto _LL3826; _LL3826: _temp3825=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3753)->f2; goto _LL3774;} else{ goto _LL3775;} _LL3775: if(*(( int*)
_temp3753) == Cyc_Absyn_Address_e){ _LL3830: _temp3829=(( struct Cyc_Absyn_Address_e_struct*)
_temp3753)->f1; goto _LL3776;} else{ goto _LL3777;} _LL3777: if(*(( int*)
_temp3753) == Cyc_Absyn_Comprehension_e){ _LL3836: _temp3835=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3753)->f1; goto _LL3834; _LL3834: _temp3833=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3753)->f2; goto _LL3832; _LL3832: _temp3831=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3753)->f3; goto _LL3778;} else{ goto _LL3779;} _LL3779: if(*(( int*)
_temp3753) == Cyc_Absyn_Array_e){ _LL3838: _temp3837=(( struct Cyc_Absyn_Array_e_struct*)
_temp3753)->f1; goto _LL3780;} else{ goto _LL3781;} _LL3781: if(*(( int*)
_temp3753) == Cyc_Absyn_AnonStruct_e){ _LL3842: _temp3841=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3753)->f1; goto _LL3840; _LL3840: _temp3839=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3753)->f2; goto _LL3782;} else{ goto _LL3783;} _LL3783: if(*(( int*)
_temp3753) == Cyc_Absyn_Struct_e){ _LL3850: _temp3849=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3753)->f1; goto _LL3848; _LL3848: _temp3847=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3753)->f2; goto _LL3846; _LL3846: _temp3845=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3753)->f3; goto _LL3844; _LL3844: _temp3843=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3753)->f4; goto _LL3784;} else{ goto _LL3785;} _LL3785: if(*(( int*)
_temp3753) == Cyc_Absyn_Primop_e){ _LL3854: _temp3853=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3753)->f1; goto _LL3852; _LL3852: _temp3851=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3753)->f2; goto _LL3786;} else{ goto _LL3787;} _LL3787: if(*(( int*)
_temp3753) == Cyc_Absyn_Tuple_e){ _LL3856: _temp3855=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3753)->f1; goto _LL3788;} else{ goto _LL3789;} _LL3789: if(*(( int*)
_temp3753) == Cyc_Absyn_Tunion_e){ _LL3866: _temp3865=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3753)->f1; goto _LL3864; _LL3864: _temp3863=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3753)->f2; goto _LL3862; _LL3862: _temp3861=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3753)->f3; goto _LL3860; _LL3860: _temp3859=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3753)->f4; goto _LL3858; _LL3858: _temp3857=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3753)->f5; goto _LL3790;} else{ goto _LL3791;} _LL3791: goto _LL3792;
_LL3756: return 1; _LL3758: return 1; _LL3760: return 1; _LL3762: return 1;
_LL3764: { struct _handler_cons _temp3867; _push_handler(& _temp3867);{ int
_temp3869= 0; if( setjmp( _temp3867.handler)){ _temp3869= 1;} if( ! _temp3869){{
void* _temp3870= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3807); void*
_temp3878; struct Cyc_Absyn_Fndecl* _temp3880; void* _temp3882; struct Cyc_Absyn_Vardecl*
_temp3884; _LL3872: if(*(( int*) _temp3870) == Cyc_Tcenv_VarRes){ _LL3879:
_temp3878=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3870)->f1; if((
unsigned int) _temp3878 > 1u?*(( int*) _temp3878) == Cyc_Absyn_Funname_b: 0){
_LL3881: _temp3880=(( struct Cyc_Absyn_Funname_b_struct*) _temp3878)->f1; goto
_LL3873;} else{ goto _LL3874;}} else{ goto _LL3874;} _LL3874: if(*(( int*)
_temp3870) == Cyc_Tcenv_VarRes){ _LL3883: _temp3882=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3870)->f1; if(( unsigned int) _temp3882 > 1u?*(( int*) _temp3882) == Cyc_Absyn_Global_b:
0){ _LL3885: _temp3884=(( struct Cyc_Absyn_Global_b_struct*) _temp3882)->f1;
goto _LL3875;} else{ goto _LL3876;}} else{ goto _LL3876;} _LL3876: goto _LL3877;
_LL3873: { int _temp3886= 1; _npop_handler( 0u); return _temp3886;} _LL3875: {
void* _temp3887= Cyc_Tcutil_compress(( void*) _temp3884->type); struct Cyc_Absyn_Exp*
_temp3893; struct Cyc_Absyn_Tqual _temp3895; void* _temp3897; _LL3889: if((
unsigned int) _temp3887 > 4u?*(( int*) _temp3887) == Cyc_Absyn_ArrayType: 0){
_LL3898: _temp3897=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3887)->f1;
goto _LL3896; _LL3896: _temp3895=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3887)->f2; goto _LL3894; _LL3894: _temp3893=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3887)->f3; goto _LL3890;} else{ goto _LL3891;} _LL3891: goto _LL3892;
_LL3890: { int _temp3899= 1; _npop_handler( 0u); return _temp3899;} _LL3892: {
int _temp3900= var_okay; _npop_handler( 0u); return _temp3900;} _LL3888:;}
_LL3877: { int _temp3901= var_okay; _npop_handler( 0u); return _temp3901;}
_LL3871:;}; _pop_handler();} else{ void* _temp3868=( void*) _exn_thrown; void*
_temp3903= _temp3868; _LL3905: if( _temp3903 == Cyc_Dict_Absent){ goto _LL3906;}
else{ goto _LL3907;} _LL3907: goto _LL3908; _LL3906: return 0; _LL3908:( void)
_throw( _temp3903); _LL3904:;}}} _LL3766: return( Cyc_Tcutil_cnst_exp( te, 0,
_temp3813)? Cyc_Tcutil_cnst_exp( te, 0, _temp3811): 0)? Cyc_Tcutil_cnst_exp( te,
0, _temp3809): 0; _LL3768: return Cyc_Tcutil_cnst_exp( te, 0, _temp3817)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3815): 0; _LL3770: _temp3823= _temp3819; goto _LL3772; _LL3772:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3823); _LL3774: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3825); _LL3776: return Cyc_Tcutil_cnst_exp( te, 1, _temp3829);
_LL3778: return Cyc_Tcutil_cnst_exp( te, 0, _temp3833)? Cyc_Tcutil_cnst_exp( te,
0, _temp3831): 0; _LL3780: _temp3839= _temp3837; goto _LL3782; _LL3782:
_temp3845= _temp3839; goto _LL3784; _LL3784: for( 0; _temp3845 != 0; _temp3845=((
struct Cyc_List_List*) _check_null( _temp3845))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3845))->hd)).f2)){
return 0;}} return 1; _LL3786: _temp3855= _temp3851; goto _LL3788; _LL3788:
_temp3861= _temp3855; goto _LL3790; _LL3790: for( 0; _temp3861 != 0; _temp3861=((
struct Cyc_List_List*) _check_null( _temp3861))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3861))->hd)){
return 0;}} return 1; _LL3792: return 0; _LL3754:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3909= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp3947; int _temp3949; struct Cyc_Core_Opt* _temp3951; struct Cyc_Core_Opt*
_temp3953; struct Cyc_Absyn_Tvar* _temp3955; struct Cyc_Absyn_TunionInfo
_temp3957; struct Cyc_Absyn_TunionFieldInfo _temp3959; struct Cyc_Absyn_PtrInfo
_temp3961; struct Cyc_Absyn_Conref* _temp3963; struct Cyc_Absyn_Tqual _temp3965;
struct Cyc_Absyn_Conref* _temp3967; void* _temp3969; void* _temp3971; void*
_temp3973; void* _temp3975; struct Cyc_Absyn_Exp* _temp3977; struct Cyc_Absyn_Tqual
_temp3979; void* _temp3981; struct Cyc_Absyn_FnInfo _temp3983; struct Cyc_List_List*
_temp3985; struct Cyc_Absyn_Structdecl** _temp3987; struct Cyc_List_List*
_temp3989; struct _tuple1* _temp3991; struct Cyc_Absyn_Uniondecl** _temp3993;
struct Cyc_List_List* _temp3995; struct _tuple1* _temp3997; struct Cyc_List_List*
_temp3999; struct Cyc_List_List* _temp4001; struct Cyc_Absyn_Enumdecl* _temp4003;
struct _tuple1* _temp4005; _LL3911: if( _temp3909 ==( void*) Cyc_Absyn_VoidType){
goto _LL3912;} else{ goto _LL3913;} _LL3913: if(( unsigned int) _temp3909 > 4u?*((
int*) _temp3909) == Cyc_Absyn_Evar: 0){ _LL3954: _temp3953=(( struct Cyc_Absyn_Evar_struct*)
_temp3909)->f1; goto _LL3952; _LL3952: _temp3951=(( struct Cyc_Absyn_Evar_struct*)
_temp3909)->f2; goto _LL3950; _LL3950: _temp3949=(( struct Cyc_Absyn_Evar_struct*)
_temp3909)->f3; goto _LL3948; _LL3948: _temp3947=(( struct Cyc_Absyn_Evar_struct*)
_temp3909)->f4; goto _LL3914;} else{ goto _LL3915;} _LL3915: if(( unsigned int)
_temp3909 > 4u?*(( int*) _temp3909) == Cyc_Absyn_VarType: 0){ _LL3956: _temp3955=((
struct Cyc_Absyn_VarType_struct*) _temp3909)->f1; goto _LL3916;} else{ goto
_LL3917;} _LL3917: if(( unsigned int) _temp3909 > 4u?*(( int*) _temp3909) == Cyc_Absyn_TunionType:
0){ _LL3958: _temp3957=(( struct Cyc_Absyn_TunionType_struct*) _temp3909)->f1;
goto _LL3918;} else{ goto _LL3919;} _LL3919: if(( unsigned int) _temp3909 > 4u?*((
int*) _temp3909) == Cyc_Absyn_TunionFieldType: 0){ _LL3960: _temp3959=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp3909)->f1; goto _LL3920;} else{ goto
_LL3921;} _LL3921: if(( unsigned int) _temp3909 > 4u?*(( int*) _temp3909) == Cyc_Absyn_PointerType:
0){ _LL3962: _temp3961=(( struct Cyc_Absyn_PointerType_struct*) _temp3909)->f1;
_LL3972: _temp3971=( void*) _temp3961.elt_typ; goto _LL3970; _LL3970: _temp3969=(
void*) _temp3961.rgn_typ; goto _LL3968; _LL3968: _temp3967= _temp3961.nullable;
goto _LL3966; _LL3966: _temp3965= _temp3961.tq; goto _LL3964; _LL3964: _temp3963=
_temp3961.bounds; goto _LL3922;} else{ goto _LL3923;} _LL3923: if(( unsigned int)
_temp3909 > 4u?*(( int*) _temp3909) == Cyc_Absyn_IntType: 0){ _LL3976: _temp3975=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp3909)->f1; goto _LL3974; _LL3974:
_temp3973=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3909)->f2; goto
_LL3924;} else{ goto _LL3925;} _LL3925: if( _temp3909 ==( void*) Cyc_Absyn_FloatType){
goto _LL3926;} else{ goto _LL3927;} _LL3927: if( _temp3909 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3928;} else{ goto _LL3929;} _LL3929: if(( unsigned int) _temp3909 > 4u?*((
int*) _temp3909) == Cyc_Absyn_ArrayType: 0){ _LL3982: _temp3981=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3909)->f1; goto _LL3980; _LL3980:
_temp3979=(( struct Cyc_Absyn_ArrayType_struct*) _temp3909)->f2; goto _LL3978;
_LL3978: _temp3977=(( struct Cyc_Absyn_ArrayType_struct*) _temp3909)->f3; goto
_LL3930;} else{ goto _LL3931;} _LL3931: if(( unsigned int) _temp3909 > 4u?*((
int*) _temp3909) == Cyc_Absyn_FnType: 0){ _LL3984: _temp3983=(( struct Cyc_Absyn_FnType_struct*)
_temp3909)->f1; goto _LL3932;} else{ goto _LL3933;} _LL3933: if(( unsigned int)
_temp3909 > 4u?*(( int*) _temp3909) == Cyc_Absyn_TupleType: 0){ _LL3986:
_temp3985=(( struct Cyc_Absyn_TupleType_struct*) _temp3909)->f1; goto _LL3934;}
else{ goto _LL3935;} _LL3935: if(( unsigned int) _temp3909 > 4u?*(( int*)
_temp3909) == Cyc_Absyn_StructType: 0){ _LL3992: _temp3991=(( struct Cyc_Absyn_StructType_struct*)
_temp3909)->f1; goto _LL3990; _LL3990: _temp3989=(( struct Cyc_Absyn_StructType_struct*)
_temp3909)->f2; goto _LL3988; _LL3988: _temp3987=(( struct Cyc_Absyn_StructType_struct*)
_temp3909)->f3; goto _LL3936;} else{ goto _LL3937;} _LL3937: if(( unsigned int)
_temp3909 > 4u?*(( int*) _temp3909) == Cyc_Absyn_UnionType: 0){ _LL3998:
_temp3997=(( struct Cyc_Absyn_UnionType_struct*) _temp3909)->f1; goto _LL3996;
_LL3996: _temp3995=(( struct Cyc_Absyn_UnionType_struct*) _temp3909)->f2; goto
_LL3994; _LL3994: _temp3993=(( struct Cyc_Absyn_UnionType_struct*) _temp3909)->f3;
goto _LL3938;} else{ goto _LL3939;} _LL3939: if(( unsigned int) _temp3909 > 4u?*((
int*) _temp3909) == Cyc_Absyn_AnonStructType: 0){ _LL4000: _temp3999=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3909)->f1; goto _LL3940;} else{ goto
_LL3941;} _LL3941: if(( unsigned int) _temp3909 > 4u?*(( int*) _temp3909) == Cyc_Absyn_AnonUnionType:
0){ _LL4002: _temp4001=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3909)->f1;
goto _LL3942;} else{ goto _LL3943;} _LL3943: if(( unsigned int) _temp3909 > 4u?*((
int*) _temp3909) == Cyc_Absyn_EnumType: 0){ _LL4006: _temp4005=(( struct Cyc_Absyn_EnumType_struct*)
_temp3909)->f1; goto _LL4004; _LL4004: _temp4003=(( struct Cyc_Absyn_EnumType_struct*)
_temp3909)->f2; goto _LL3944;} else{ goto _LL3945;} _LL3945: goto _LL3946;
_LL3912: return 1; _LL3914: return 0; _LL3916: return 0; _LL3918: return 0;
_LL3920: return 0; _LL3922: { void* _temp4007=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3963))->v; void*
_temp4015; void* _temp4017; struct Cyc_Absyn_Exp* _temp4019; _LL4009: if((
unsigned int) _temp4007 > 1u?*(( int*) _temp4007) == Cyc_Absyn_Eq_constr: 0){
_LL4016: _temp4015=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp4007)->f1;
if( _temp4015 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL4010;} else{ goto _LL4011;}}
else{ goto _LL4011;} _LL4011: if(( unsigned int) _temp4007 > 1u?*(( int*)
_temp4007) == Cyc_Absyn_Eq_constr: 0){ _LL4018: _temp4017=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4007)->f1; if(( unsigned int) _temp4017 > 1u?*(( int*) _temp4017) == Cyc_Absyn_Upper_b:
0){ _LL4020: _temp4019=(( struct Cyc_Absyn_Upper_b_struct*) _temp4017)->f1; goto
_LL4012;} else{ goto _LL4013;}} else{ goto _LL4013;} _LL4013: goto _LL4014;
_LL4010: return 1; _LL4012: { void* _temp4021=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3967))->v; int
_temp4027; _LL4023: if(( unsigned int) _temp4021 > 1u?*(( int*) _temp4021) ==
Cyc_Absyn_Eq_constr: 0){ _LL4028: _temp4027=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4021)->f1; goto _LL4024;} else{ goto _LL4025;} _LL4025: goto _LL4026;
_LL4024: return _temp4027; _LL4026: return 0; _LL4022:;} _LL4014: return 0;
_LL4008:;} _LL3924: return 1; _LL3926: return 1; _LL3928: return 1; _LL3930:
return Cyc_Tcutil_supports_default( _temp3981); _LL3932: return 0; _LL3934: for(
0; _temp3985 != 0; _temp3985=(( struct Cyc_List_List*) _check_null( _temp3985))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3985))->hd)).f2)){ return 0;}} return 1; _LL3936: if(
_temp3987 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3987)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3989,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3938: if( _temp3993 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3993)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3995,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3940: _temp4001= _temp3999; goto _LL3942; _LL3942: return Cyc_Tcutil_fields_support_default(
0, 0, _temp4001); _LL3944: return 1; _LL3946: return 0; _LL3910:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp4029= _new_region();
struct _RegionHandle* rgn=& _temp4029; _push_region( rgn);{ struct Cyc_List_List*
_temp4030=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); goto _LL4031; _LL4031: for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp4030,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fs))->hd)->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp4032= 0; _npop_handler( 0u);
return _temp4032;}}}; _pop_region(& _temp4029);} return 1;}