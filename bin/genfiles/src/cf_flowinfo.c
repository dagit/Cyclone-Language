 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern int
Cyc_Core_intcmp( int, int); extern int Cyc_Core_ptrcmp( void**, void**); extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
extern int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List* l,
void* x); extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern void Cyc_Dict_iter_c( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map_c( void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern
int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern int Cyc_String_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*, struct
_tagged_arr*); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar;
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
0; struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; struct Cyc_Absyn_Conref*
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
15u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_compress(
void* t); struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_arr* f1; } ;
static const int Cyc_CfFlowInfo_TupleF= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc=
1; static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct
Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern void* Cyc_CfFlowInfo_mkLeafPI(
void* esc, void* il); extern int Cyc_CfFlowInfo_local_root_cmp( void*, void*);
extern int Cyc_CfFlowInfo_field_cmp( void*, void*); extern int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
void* t); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d,
struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo);
extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict* pinfo_dict, void*
pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo,
struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*,
void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_noneil={
0u,( void*)(( void*) 1u),( void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_unes_thisil={ 0u,( void*)(( void*) 1u),( void*)(( void*)
1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_allil={
0u,( void*)(( void*) 1u),( void*)(( void*) 2u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_esc_noneil={ 0u,( void*)(( void*) 0u),( void*)(( void*)
0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_esc_thisil={
0u,( void*)(( void*) 0u),( void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_esc_allil={ 0u,( void*)(( void*) 0u),( void*)(( void*)
2u)}; static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_none_v={
0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_noneil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_unesc_this_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_thisil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_all_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_allil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_none_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_noneil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_this_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_thisil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_all_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_allil)};
void* Cyc_CfFlowInfo_leaf_unesc_none=( void*)& Cyc_CfFlowInfo_leaf_unesc_none_v;
void* Cyc_CfFlowInfo_leaf_unesc_this=( void*)& Cyc_CfFlowInfo_leaf_unesc_this_v;
void* Cyc_CfFlowInfo_leaf_unesc_all=( void*)& Cyc_CfFlowInfo_leaf_unesc_all_v;
void* Cyc_CfFlowInfo_leaf_esc_none=( void*)& Cyc_CfFlowInfo_leaf_esc_none_v;
void* Cyc_CfFlowInfo_leaf_esc_this=( void*)& Cyc_CfFlowInfo_leaf_esc_this_v;
void* Cyc_CfFlowInfo_leaf_esc_all=( void*)& Cyc_CfFlowInfo_leaf_esc_all_v;
struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfFlowInfo_mkLeafPI( void* esc,
void* il){ struct _tuple3 _temp13=({ struct _tuple3 _temp12; _temp12.f1= esc;
_temp12.f2= il; _temp12;}); void* _temp27; void* _temp29; void* _temp31; void*
_temp33; void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; _LL15: _LL30: _temp29=
_temp13.f1; if( _temp29 ==( void*) Cyc_CfFlowInfo_Unesc){ goto _LL28;} else{
goto _LL17;} _LL28: _temp27= _temp13.f2; if( _temp27 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL16;} else{ goto _LL17;} _LL17: _LL34: _temp33= _temp13.f1; if( _temp33
==( void*) Cyc_CfFlowInfo_Unesc){ goto _LL32;} else{ goto _LL19;} _LL32: _temp31=
_temp13.f2; if( _temp31 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL18;} else{
goto _LL19;} _LL19: _LL38: _temp37= _temp13.f1; if( _temp37 ==( void*) Cyc_CfFlowInfo_Unesc){
goto _LL36;} else{ goto _LL21;} _LL36: _temp35= _temp13.f2; if( _temp35 ==( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL20;} else{ goto _LL21;} _LL21: _LL42: _temp41=
_temp13.f1; if( _temp41 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL40;} else{ goto
_LL23;} _LL40: _temp39= _temp13.f2; if( _temp39 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL22;} else{ goto _LL23;} _LL23: _LL46: _temp45= _temp13.f1; if( _temp45
==( void*) Cyc_CfFlowInfo_Esc){ goto _LL44;} else{ goto _LL25;} _LL44: _temp43=
_temp13.f2; if( _temp43 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL24;} else{
goto _LL25;} _LL25: _LL50: _temp49= _temp13.f1; if( _temp49 ==( void*) Cyc_CfFlowInfo_Esc){
goto _LL48;} else{ goto _LL14;} _LL48: _temp47= _temp13.f2; if( _temp47 ==( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL26;} else{ goto _LL14;} _LL16: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL18: return Cyc_CfFlowInfo_leaf_unesc_this; _LL20: return Cyc_CfFlowInfo_leaf_unesc_all;
_LL22: return Cyc_CfFlowInfo_leaf_esc_none; _LL24: return Cyc_CfFlowInfo_leaf_esc_this;
_LL26: return Cyc_CfFlowInfo_leaf_esc_all; _LL14:;} int Cyc_CfFlowInfo_local_root_cmp(
void* r1, void* r2){ if( r1 == r2){ return 0;}{ struct _tuple3 _temp52=({ struct
_tuple3 _temp51; _temp51.f1= r1; _temp51.f2= r2; _temp51;}); void* _temp62;
struct Cyc_Absyn_Exp* _temp64; void* _temp66; struct Cyc_Absyn_Vardecl* _temp68;
void* _temp70; struct Cyc_Absyn_Vardecl* _temp72; void* _temp74; struct Cyc_Absyn_Exp*
_temp76; void* _temp78; struct Cyc_Absyn_Vardecl* _temp80; void* _temp82; struct
Cyc_Absyn_Vardecl* _temp84; void* _temp86; struct Cyc_Absyn_Exp* _temp88; void*
_temp90; struct Cyc_Absyn_Exp* _temp92; _LL54: _LL67: _temp66= _temp52.f1; if(*((
int*) _temp66) == Cyc_CfFlowInfo_VarRoot){ _LL69: _temp68=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp66)->f1; goto _LL63;} else{ goto _LL56;} _LL63: _temp62= _temp52.f2; if(*((
int*) _temp62) == Cyc_CfFlowInfo_MallocPt){ _LL65: _temp64=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp62)->f1; goto _LL55;} else{ goto _LL56;} _LL56: _LL75: _temp74= _temp52.f1;
if(*(( int*) _temp74) == Cyc_CfFlowInfo_MallocPt){ _LL77: _temp76=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp74)->f1; goto _LL71;} else{ goto _LL58;} _LL71: _temp70= _temp52.f2; if(*((
int*) _temp70) == Cyc_CfFlowInfo_VarRoot){ _LL73: _temp72=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp70)->f1; goto _LL57;} else{ goto _LL58;} _LL58: _LL83: _temp82= _temp52.f1;
if(*(( int*) _temp82) == Cyc_CfFlowInfo_VarRoot){ _LL85: _temp84=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp82)->f1; goto _LL79;} else{ goto _LL60;} _LL79: _temp78= _temp52.f2; if(*((
int*) _temp78) == Cyc_CfFlowInfo_VarRoot){ _LL81: _temp80=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp78)->f1; goto _LL59;} else{ goto _LL60;} _LL60: _LL91: _temp90= _temp52.f1;
if(*(( int*) _temp90) == Cyc_CfFlowInfo_MallocPt){ _LL93: _temp92=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp90)->f1; goto _LL87;} else{ goto _LL53;} _LL87: _temp86= _temp52.f2; if(*((
int*) _temp86) == Cyc_CfFlowInfo_MallocPt){ _LL89: _temp88=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp86)->f1; goto _LL61;} else{ goto _LL53;} _LL55: return 1; _LL57: return - 1;
_LL59: return(( int(*)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp)( _temp84, _temp80); _LL61: return(( int(*)( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp)( _temp92, _temp88); _LL53:;}} int Cyc_CfFlowInfo_field_cmp(
void* f1, void* f2){ if( f1 == f2){ return 0;}{ struct _tuple3 _temp95=({ struct
_tuple3 _temp94; _temp94.f1= f1; _temp94.f2= f2; _temp94;}); void* _temp105; int
_temp107; void* _temp109; struct _tagged_arr* _temp111; void* _temp113; struct
_tagged_arr* _temp115; void* _temp117; int _temp119; void* _temp121; struct
_tagged_arr* _temp123; void* _temp125; struct _tagged_arr* _temp127; void*
_temp129; int _temp131; void* _temp133; int _temp135; _LL97: _LL110: _temp109=
_temp95.f1; if(*(( int*) _temp109) == Cyc_CfFlowInfo_StructF){ _LL112: _temp111=((
struct Cyc_CfFlowInfo_StructF_struct*) _temp109)->f1; goto _LL106;} else{ goto
_LL99;} _LL106: _temp105= _temp95.f2; if(*(( int*) _temp105) == Cyc_CfFlowInfo_TupleF){
_LL108: _temp107=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp105)->f1; goto
_LL98;} else{ goto _LL99;} _LL99: _LL118: _temp117= _temp95.f1; if(*(( int*)
_temp117) == Cyc_CfFlowInfo_TupleF){ _LL120: _temp119=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp117)->f1; goto _LL114;} else{ goto _LL101;} _LL114: _temp113= _temp95.f2;
if(*(( int*) _temp113) == Cyc_CfFlowInfo_StructF){ _LL116: _temp115=(( struct
Cyc_CfFlowInfo_StructF_struct*) _temp113)->f1; goto _LL100;} else{ goto _LL101;}
_LL101: _LL126: _temp125= _temp95.f1; if(*(( int*) _temp125) == Cyc_CfFlowInfo_StructF){
_LL128: _temp127=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp125)->f1; goto
_LL122;} else{ goto _LL103;} _LL122: _temp121= _temp95.f2; if(*(( int*) _temp121)
== Cyc_CfFlowInfo_StructF){ _LL124: _temp123=(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp121)->f1; goto _LL102;} else{ goto _LL103;} _LL103: _LL134: _temp133=
_temp95.f1; if(*(( int*) _temp133) == Cyc_CfFlowInfo_TupleF){ _LL136: _temp135=((
struct Cyc_CfFlowInfo_TupleF_struct*) _temp133)->f1; goto _LL130;} else{ goto
_LL96;} _LL130: _temp129= _temp95.f2; if(*(( int*) _temp129) == Cyc_CfFlowInfo_TupleF){
_LL132: _temp131=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp129)->f1; goto
_LL104;} else{ goto _LL96;} _LL98: return 1; _LL100: return - 1; _LL102: return((
int(*)( struct _tagged_arr*, struct _tagged_arr*)) Cyc_Core_ptrcmp)( _temp127,
_temp123); _LL104: return _temp135 - _temp131; _LL96:;}} int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place* p1, struct Cyc_CfFlowInfo_Place* p2){ if( p1 == p2){
return 0;}{ int i= Cyc_CfFlowInfo_local_root_cmp(( void*) p1->root,( void*) p2->root);
if( i != 0){ return i;} return(( int(*)( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_CfFlowInfo_field_cmp, p1->fields,
p2->fields);}} struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; } ; void*
Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t){ void* _temp137= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_TunionFieldInfo _temp149; struct Cyc_List_List* _temp151;
void* _temp153; struct Cyc_Absyn_Tunionfield* _temp155; struct Cyc_Absyn_Tuniondecl*
_temp157; struct Cyc_List_List* _temp159; struct Cyc_List_List* _temp161; struct
Cyc_Absyn_Structdecl** _temp163; struct Cyc_Absyn_Structdecl* _temp165; struct
Cyc_Absyn_Structdecl _temp166; struct Cyc_List_List* _temp167; struct Cyc_Core_Opt*
_temp169; struct Cyc_Core_Opt _temp171; struct Cyc_List_List* _temp172; struct
Cyc_List_List* _temp174; struct Cyc_Core_Opt* _temp176; void* _temp178; struct
Cyc_List_List* _temp180; struct _tuple0* _temp182; _LL139: if(( unsigned int)
_temp137 > 4u?*(( int*) _temp137) == Cyc_Absyn_TunionFieldType: 0){ _LL150:
_temp149=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp137)->f1; _LL154:
_temp153=( void*) _temp149.field_info; if(*(( int*) _temp153) == Cyc_Absyn_KnownTunionfield){
_LL158: _temp157=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp153)->f1;
goto _LL156; _LL156: _temp155=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp153)->f2; goto _LL152;} else{ goto _LL141;} _LL152: _temp151= _temp149.targs;
goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int) _temp137 > 4u?*((
int*) _temp137) == Cyc_Absyn_TupleType: 0){ _LL160: _temp159=(( struct Cyc_Absyn_TupleType_struct*)
_temp137)->f1; goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp137 > 4u?*(( int*) _temp137) == Cyc_Absyn_AnonStructType: 0){ _LL162:
_temp161=(( struct Cyc_Absyn_AnonStructType_struct*) _temp137)->f1; goto _LL144;}
else{ goto _LL145;} _LL145: if(( unsigned int) _temp137 > 4u?*(( int*) _temp137)
== Cyc_Absyn_StructType: 0){ _LL183: _temp182=(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f1; goto _LL181; _LL181: _temp180=(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f2; goto _LL164; _LL164: _temp163=(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f3; if( _temp163 == 0){ goto _LL147;} else{ _temp165=* _temp163;
_temp166=* _temp165; _LL179: _temp178=( void*) _temp166.sc; goto _LL177; _LL177:
_temp176= _temp166.name; goto _LL175; _LL175: _temp174= _temp166.tvs; goto
_LL170; _LL170: _temp169= _temp166.fields; if( _temp169 == 0){ goto _LL147;}
else{ _temp171=* _temp169; _LL173: _temp172=( struct Cyc_List_List*) _temp171.v;
goto _LL168;} _LL168: _temp167= _temp166.attributes; goto _LL146;}} else{ goto
_LL147;} _LL147: goto _LL148; _LL140: if( _temp155->typs == 0){ return Cyc_CfFlowInfo_leaf_unesc_none;}
_temp159= _temp155->typs; goto _LL142; _LL142: { struct Cyc_Dict_Dict* _temp184=((
struct Cyc_Dict_Dict*(*)( int(* comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);{
int i= 0; for( 0; _temp159 != 0;( _temp159=(( struct Cyc_List_List*) _check_null(
_temp159))->tl, ++ i)){ struct _tuple4 _temp187; void* _temp188; struct Cyc_Absyn_Tqual
_temp190; struct _tuple4* _temp185=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp159))->hd; _temp187=* _temp185; _LL191: _temp190= _temp187.f1;
goto _LL189; _LL189: _temp188= _temp187.f2; goto _LL186; _LL186: _temp184=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void* data)) Cyc_Dict_insert)(
_temp184, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp188));}} return( void*)({
struct Cyc_CfFlowInfo_TuplePI_struct* _temp192=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp192[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp193; _temp193.tag= Cyc_CfFlowInfo_TuplePI;
_temp193.f1= _temp184; _temp193;}); _temp192;});} _LL144: _temp172= _temp161;
goto _LL146; _LL146: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
for( 0; _temp172 != 0; _temp172=(( struct Cyc_List_List*) _check_null( _temp172))->tl){
struct Cyc_Absyn_Structfield _temp196; struct Cyc_List_List* _temp197; struct
Cyc_Core_Opt* _temp199; void* _temp201; struct Cyc_Absyn_Tqual _temp203; struct
_tagged_arr* _temp205; struct Cyc_Absyn_Structfield* _temp194=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp172))->hd; _temp196=* _temp194; _LL206:
_temp205= _temp196.name; goto _LL204; _LL204: _temp203= _temp196.tq; goto _LL202;
_LL202: _temp201=( void*) _temp196.type; goto _LL200; _LL200: _temp199= _temp196.width;
goto _LL198; _LL198: _temp197= _temp196.attributes; goto _LL195; _LL195: if( Cyc_String_strcmp(*
_temp205, _tag_arr("", sizeof( unsigned char), 1u)) != 0){ d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)(
d, _temp205, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp201));}} return( void*)({
struct Cyc_CfFlowInfo_StructPI_struct* _temp207=( struct Cyc_CfFlowInfo_StructPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct)); _temp207[ 0]=({
struct Cyc_CfFlowInfo_StructPI_struct _temp208; _temp208.tag= Cyc_CfFlowInfo_StructPI;
_temp208.f1= d; _temp208;}); _temp207;});} _LL148: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL138:;} void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place){ struct Cyc_CfFlowInfo_Place _temp211; struct Cyc_List_List* _temp212;
void* _temp214; struct Cyc_CfFlowInfo_Place* _temp209= place; _temp211=*
_temp209; _LL215: _temp214=( void*) _temp211.root; goto _LL213; _LL213: _temp212=
_temp211.fields; goto _LL210; _LL210: { void* _temp216=(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( d, _temp214); for( 0; _temp212 != 0; _temp212=((
struct Cyc_List_List*) _check_null( _temp212))->tl){ struct _tuple3 _temp218=({
struct _tuple3 _temp217; _temp217.f1= _temp216; _temp217.f2=( void*)(( struct
Cyc_List_List*) _check_null( _temp212))->hd; _temp217;}); void* _temp226; int
_temp228; void* _temp230; struct Cyc_Dict_Dict* _temp232; void* _temp234; struct
_tagged_arr* _temp236; void* _temp238; struct Cyc_Dict_Dict* _temp240; _LL220:
_LL231: _temp230= _temp218.f1; if(*(( int*) _temp230) == Cyc_CfFlowInfo_TuplePI){
_LL233: _temp232=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp230)->f1; goto
_LL227;} else{ goto _LL222;} _LL227: _temp226= _temp218.f2; if(*(( int*)
_temp226) == Cyc_CfFlowInfo_TupleF){ _LL229: _temp228=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp226)->f1; goto _LL221;} else{ goto _LL222;} _LL222: _LL239: _temp238=
_temp218.f1; if(*(( int*) _temp238) == Cyc_CfFlowInfo_StructPI){ _LL241:
_temp240=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp238)->f1; goto _LL235;}
else{ goto _LL224;} _LL235: _temp234= _temp218.f2; if(*(( int*) _temp234) == Cyc_CfFlowInfo_StructF){
_LL237: _temp236=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp234)->f1; goto
_LL223;} else{ goto _LL224;} _LL224: goto _LL225; _LL221: _temp216=(( void*(*)(
struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp232, _temp228); goto
_LL219; _LL223: _temp216=(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp240, _temp236); goto _LL219; _LL225:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp242=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp242[ 0]=({ struct
Cyc_Core_Impossible_struct _temp243; _temp243.tag= Cyc_Core_Impossible; _temp243.f1=
_tag_arr("bad lookup_place", sizeof( unsigned char), 17u); _temp243;}); _temp242;}));
_LL219:;} return _temp216;}} struct _tuple5{ void* f1; struct Cyc_List_List* f2;
} ; void* Cyc_CfFlowInfo_insert_place_rec( void* insert_path, struct Cyc_List_List*
fields, void* pinfo){ if( fields == 0){ return pinfo;}{ struct _tuple5 _temp245=({
struct _tuple5 _temp244; _temp244.f1= insert_path; _temp244.f2= fields; _temp244;});
struct Cyc_List_List* _temp253; struct Cyc_List_List _temp255; struct Cyc_List_List*
_temp256; void* _temp258; int _temp260; void* _temp262; struct Cyc_Dict_Dict*
_temp264; struct Cyc_List_List* _temp266; struct Cyc_List_List _temp268; struct
Cyc_List_List* _temp269; void* _temp271; struct _tagged_arr* _temp273; void*
_temp275; struct Cyc_Dict_Dict* _temp277; _LL247: _LL263: _temp262= _temp245.f1;
if(*(( int*) _temp262) == Cyc_CfFlowInfo_TuplePI){ _LL265: _temp264=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp262)->f1; goto _LL254;} else{ goto _LL249;}
_LL254: _temp253= _temp245.f2; if( _temp253 == 0){ goto _LL249;} else{ _temp255=*
_temp253; _LL259: _temp258=( void*) _temp255.hd; if(*(( int*) _temp258) == Cyc_CfFlowInfo_TupleF){
_LL261: _temp260=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp258)->f1; goto
_LL257;} else{ goto _LL249;} _LL257: _temp256= _temp255.tl; goto _LL248;} _LL249:
_LL276: _temp275= _temp245.f1; if(*(( int*) _temp275) == Cyc_CfFlowInfo_StructPI){
_LL278: _temp277=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp275)->f1; goto
_LL267;} else{ goto _LL251;} _LL267: _temp266= _temp245.f2; if( _temp266 == 0){
goto _LL251;} else{ _temp268=* _temp266; _LL272: _temp271=( void*) _temp268.hd;
if(*(( int*) _temp271) == Cyc_CfFlowInfo_StructF){ _LL274: _temp273=(( struct
Cyc_CfFlowInfo_StructF_struct*) _temp271)->f1; goto _LL270;} else{ goto _LL251;}
_LL270: _temp269= _temp268.tl; goto _LL250;} _LL251: goto _LL252; _LL248: { void*
_temp279=(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)(
_temp264, _temp260); return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp280=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp280[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp281; _temp281.tag= Cyc_CfFlowInfo_TuplePI;
_temp281.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void*
data)) Cyc_Dict_insert)( _temp264, _temp260, Cyc_CfFlowInfo_insert_place_rec(
_temp279, _temp256, pinfo)); _temp281;}); _temp280;});} _LL250: { void* _temp282=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp277, _temp273); return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp283=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp283[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp284; _temp284.tag=
Cyc_CfFlowInfo_StructPI; _temp284.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)( _temp277, _temp273,
Cyc_CfFlowInfo_insert_place_rec( _temp282, _temp269, pinfo)); _temp284;});
_temp283;});} _LL252:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp285=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp285[ 0]=({ struct Cyc_Core_Impossible_struct _temp286; _temp286.tag= Cyc_Core_Impossible;
_temp286.f1= _tag_arr("bad insert_place", sizeof( unsigned char), 17u); _temp286;});
_temp285;})); _LL246:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo){
struct Cyc_CfFlowInfo_Place _temp289; struct Cyc_List_List* _temp290; void*
_temp292; struct Cyc_CfFlowInfo_Place* _temp287= place; _temp289=* _temp287;
_LL293: _temp292=( void*) _temp289.root; goto _LL291; _LL291: _temp290= _temp289.fields;
goto _LL288; _LL288: return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( d, _temp292, Cyc_CfFlowInfo_insert_place_rec(((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d, _temp292),
_temp290, pinfo));} struct _tuple6{ struct Cyc_Dict_Dict* f1; struct Cyc_List_List*
f2; } ; int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path);
int Cyc_CfFlowInfo_isAllInit_is( struct _tuple6* env, void* state){ void*
_temp294= state; void* _temp302; void* _temp304; void* _temp306; void* _temp308;
struct Cyc_CfFlowInfo_Place* _temp310; _LL296: if(*(( int*) _temp294) == Cyc_CfFlowInfo_UnknownIS){
_LL305: _temp304=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp294)->f1;
goto _LL303; _LL303: _temp302=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp294)->f2; if( _temp302 ==( void*) Cyc_CfFlowInfo_AllIL){ goto _LL297;}
else{ goto _LL298;}} else{ goto _LL298;} _LL298: if(*(( int*) _temp294) == Cyc_CfFlowInfo_UnknownIS){
_LL309: _temp308=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp294)->f1;
goto _LL307; _LL307: _temp306=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp294)->f2; goto _LL299;} else{ goto _LL300;} _LL300: if(*(( int*) _temp294)
== Cyc_CfFlowInfo_MustPointTo){ _LL311: _temp310=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp294)->f1; goto _LL301;} else{ goto _LL295;} _LL297: return 1; _LL299:
return 0; _LL301: { struct _tuple6 _temp314; struct Cyc_List_List* _temp315;
struct Cyc_List_List** _temp317; struct Cyc_Dict_Dict* _temp318; struct _tuple6*
_temp312= env; _temp314=* _temp312; _LL319: _temp318= _temp314.f1; goto _LL316;
_LL316: _temp315= _temp314.f2; _temp317=&(* _temp312).f2; goto _LL313; _LL313:
if((( int(*)( int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
_temp317, _temp310)){ return 1;}* _temp317=({ struct Cyc_List_List* _temp320=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp320->hd=(
void*) _temp310; _temp320->tl=* _temp317; _temp320;}); return Cyc_CfFlowInfo_isAllInit_pinfo(
env, Cyc_CfFlowInfo_lookup_place( _temp318, _temp310));} _LL295:;} int Cyc_CfFlowInfo_isAllInit_f(
struct _tuple6* env, void* a, void* path){ return Cyc_CfFlowInfo_isAllInit_pinfo(
env, path);} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path){
void* _temp321= path; void* _temp329; struct Cyc_Dict_Dict* _temp331; struct Cyc_Dict_Dict*
_temp333; _LL323: if(*(( int*) _temp321) == Cyc_CfFlowInfo_LeafPI){ _LL330:
_temp329=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp321)->f1; goto
_LL324;} else{ goto _LL325;} _LL325: if(*(( int*) _temp321) == Cyc_CfFlowInfo_TuplePI){
_LL332: _temp331=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp321)->f1; goto
_LL326;} else{ goto _LL327;} _LL327: if(*(( int*) _temp321) == Cyc_CfFlowInfo_StructPI){
_LL334: _temp333=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp321)->f1; goto
_LL328;} else{ goto _LL322;} _LL324: return Cyc_CfFlowInfo_isAllInit_is( env,
_temp329); _LL326: return(( int(*)( int(* f)( struct _tuple6*, int, void*),
struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)(
struct _tuple6* env, int a, void* path)) Cyc_CfFlowInfo_isAllInit_f, env,
_temp331); _LL328: return(( int(*)( int(* f)( struct _tuple6*, struct
_tagged_arr*, void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct _tuple6* env, struct _tagged_arr* a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp333); _LL322:;} int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo){ struct _tuple6 _temp335=({ struct _tuple6 _temp336;
_temp336.f1= pinfo_dict; _temp336.f2= 0; _temp336;}); return Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp335, pinfo);} static void Cyc_CfFlowInfo_add_place( struct _RegionHandle* r,
struct Cyc_List_List** places, struct Cyc_CfFlowInfo_Place* place){ if( !(( int(*)(
int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
places, place)){* places=({ struct Cyc_List_List* _temp337=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp337->hd=( void*) place;
_temp337->tl=* places; _temp337;});}} struct _tuple7{ struct Cyc_Dict_Dict* f1;
struct Cyc_List_List** f2; struct _RegionHandle* f3; } ; void* Cyc_CfFlowInfo_pinfo_getplaces_esc(
struct _tuple7* env, void* pinfo){ struct _tuple7 _temp340; struct _RegionHandle*
_temp341; struct Cyc_List_List** _temp343; struct Cyc_Dict_Dict* _temp345;
struct _tuple7* _temp338= env; _temp340=* _temp338; _LL346: _temp345= _temp340.f1;
goto _LL344; _LL344: _temp343= _temp340.f2; goto _LL342; _LL342: _temp341=
_temp340.f3; goto _LL339; _LL339: { void* _temp347= pinfo; void* _temp357;
struct Cyc_CfFlowInfo_Place* _temp359; void* _temp361; void* _temp363; void*
_temp365; struct Cyc_Dict_Dict* _temp367; struct Cyc_Dict_Dict* _temp369; _LL349:
if(*(( int*) _temp347) == Cyc_CfFlowInfo_LeafPI){ _LL358: _temp357=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp347)->f1; if(*(( int*) _temp357) ==
Cyc_CfFlowInfo_MustPointTo){ _LL360: _temp359=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp357)->f1; goto _LL350;} else{ goto _LL351;}} else{ goto _LL351;} _LL351:
if(*(( int*) _temp347) == Cyc_CfFlowInfo_LeafPI){ _LL362: _temp361=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp347)->f1; if(*(( int*) _temp361) ==
Cyc_CfFlowInfo_UnknownIS){ _LL366: _temp365=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp361)->f1; goto _LL364; _LL364: _temp363=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp361)->f2; goto _LL352;} else{ goto _LL353;}} else{ goto _LL353;} _LL353:
if(*(( int*) _temp347) == Cyc_CfFlowInfo_TuplePI){ _LL368: _temp367=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp347)->f1; goto _LL354;} else{ goto _LL355;}
_LL355: if(*(( int*) _temp347) == Cyc_CfFlowInfo_StructPI){ _LL370: _temp369=((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp347)->f1; goto _LL356;} else{ goto
_LL348;} _LL350: Cyc_CfFlowInfo_add_place( _temp341, _temp343, _temp359); if(
Cyc_CfFlowInfo_isAllInit( _temp345, Cyc_CfFlowInfo_lookup_place( _temp345,
_temp359))){ return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,( void*)
Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_ThisIL); _LL352: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Esc, _temp363); _LL354: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp371=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp371[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp372; _temp372.tag= Cyc_CfFlowInfo_TuplePI;
_temp372.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct _tuple7*, void*),
struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc,
env, _temp367); _temp372;}); _temp371;}); _LL356: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp373=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp373[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp374; _temp374.tag=
Cyc_CfFlowInfo_StructPI; _temp374.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct _tuple7*, void*), struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)(
Cyc_CfFlowInfo_pinfo_getplaces_esc, env, _temp369); _temp374;}); _temp373;});
_LL348:;}} struct _tuple8{ struct Cyc_List_List** f1; struct _RegionHandle* f2;
} ; void Cyc_CfFlowInfo_pinfo_getplaces( struct _tuple8* env, void* a, void*
pinfo){ struct _tuple8 _temp377; struct _RegionHandle* _temp378; struct Cyc_List_List**
_temp380; struct _tuple8* _temp375= env; _temp377=* _temp375; _LL381: _temp380=
_temp377.f1; goto _LL379; _LL379: _temp378= _temp377.f2; goto _LL376; _LL376: {
void* _temp382= pinfo; void* _temp392; void* _temp394; void* _temp396; void*
_temp398; struct Cyc_CfFlowInfo_Place* _temp400; struct Cyc_Dict_Dict* _temp402;
struct Cyc_Dict_Dict* _temp404; _LL384: if(*(( int*) _temp382) == Cyc_CfFlowInfo_LeafPI){
_LL393: _temp392=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp382)->f1;
if(*(( int*) _temp392) == Cyc_CfFlowInfo_UnknownIS){ _LL397: _temp396=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp392)->f1; goto _LL395; _LL395:
_temp394=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp392)->f2; goto
_LL385;} else{ goto _LL386;}} else{ goto _LL386;} _LL386: if(*(( int*) _temp382)
== Cyc_CfFlowInfo_LeafPI){ _LL399: _temp398=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp382)->f1; if(*(( int*) _temp398) == Cyc_CfFlowInfo_MustPointTo){ _LL401:
_temp400=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp398)->f1; goto
_LL387;} else{ goto _LL388;}} else{ goto _LL388;} _LL388: if(*(( int*) _temp382)
== Cyc_CfFlowInfo_TuplePI){ _LL403: _temp402=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp382)->f1; goto _LL389;} else{ goto _LL390;} _LL390: if(*(( int*) _temp382)
== Cyc_CfFlowInfo_StructPI){ _LL405: _temp404=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp382)->f1; goto _LL391;} else{ goto _LL383;} _LL385: return; _LL387: Cyc_CfFlowInfo_add_place(
_temp378, _temp380, _temp400); return; _LL389:(( void(*)( void(* f)( struct
_tuple8*, int, void*), struct _tuple8* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple8* env, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces,
env, _temp402); return; _LL391:(( void(*)( void(* f)( struct _tuple8*, struct
_tagged_arr*, void*), struct _tuple8* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple8* env, struct _tagged_arr* a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces,
env, _temp404); return; _LL383:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these(
struct _RegionHandle* rgn, struct Cyc_List_List* places, struct Cyc_Dict_Dict* d){
while( places != 0) { struct Cyc_CfFlowInfo_Place* _temp406=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*) _check_null( places))->hd; places=(( struct Cyc_List_List*)
_check_null( places))->tl;{ void* place_old_pinfo;{ struct _handler_cons
_temp407; _push_handler(& _temp407);{ int _temp409= 0; if( setjmp( _temp407.handler)){
_temp409= 1;} if( ! _temp409){ place_old_pinfo= Cyc_CfFlowInfo_lookup_place( d,
_temp406);; _pop_handler();} else{ void* _temp408=( void*) _exn_thrown; void*
_temp411= _temp408; _LL413: if( _temp411 == Cyc_Dict_Absent){ goto _LL414;}
else{ goto _LL415;} _LL415: goto _LL416; _LL414: continue; _LL416:( void) _throw(
_temp411); _LL412:;}}}{ struct _tuple7 _temp417=({ struct _tuple7 _temp419;
_temp419.f1= d; _temp419.f2=& places; _temp419.f3= rgn; _temp419;}); void*
_temp418= Cyc_CfFlowInfo_pinfo_getplaces_esc(& _temp417, place_old_pinfo); if(
place_old_pinfo != _temp418){ d= Cyc_CfFlowInfo_insert_place( d, _temp406,
_temp418);}}}} return d;} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d){ struct _RegionHandle _temp420=
_new_region(); struct _RegionHandle* r=& _temp420; _push_region( r);{ struct Cyc_List_List*
_temp421= 0; struct _tuple8 _temp422=({ struct _tuple8 _temp424; _temp424.f1=&
_temp421; _temp424.f2= r; _temp424;});(( void(*)( struct _tuple8* env, int a,
void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces)(& _temp422, 0, pinfo);{ struct Cyc_Dict_Dict*
_temp423= Cyc_CfFlowInfo_escape_these( r, _temp421, d); _npop_handler( 0u);
return _temp423;}}; _pop_region( r);} void* Cyc_CfFlowInfo_assign_unknown_dict(
void* level, void* pinfo){ void* _temp425= pinfo; void* _temp435; void* _temp437;
void* _temp439; void* _temp441; struct Cyc_Dict_Dict* _temp443; struct Cyc_Dict_Dict*
_temp445; _LL427: if(*(( int*) _temp425) == Cyc_CfFlowInfo_LeafPI){ _LL436:
_temp435=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp425)->f1; if(*((
int*) _temp435) == Cyc_CfFlowInfo_UnknownIS){ _LL440: _temp439=( void*)(( struct
Cyc_CfFlowInfo_UnknownIS_struct*) _temp435)->f1; if( _temp439 ==( void*) Cyc_CfFlowInfo_Esc){
goto _LL438;} else{ goto _LL429;} _LL438: _temp437=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp435)->f2; goto _LL428;} else{ goto _LL429;}} else{ goto _LL429;} _LL429:
if(*(( int*) _temp425) == Cyc_CfFlowInfo_LeafPI){ _LL442: _temp441=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp425)->f1; goto _LL430;} else{ goto
_LL431;} _LL431: if(*(( int*) _temp425) == Cyc_CfFlowInfo_TuplePI){ _LL444:
_temp443=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp425)->f1; goto _LL432;}
else{ goto _LL433;} _LL433: if(*(( int*) _temp425) == Cyc_CfFlowInfo_StructPI){
_LL446: _temp445=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp425)->f1; goto
_LL434;} else{ goto _LL426;} _LL428: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,
level); _LL430: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,
level); _LL432: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp447=(
struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp447[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp448; _temp448.tag= Cyc_CfFlowInfo_TuplePI;
_temp448.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp443); _temp448;}); _temp447;}); _LL434: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp449=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp449[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp450; _temp450.tag=
Cyc_CfFlowInfo_StructPI; _temp450.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
void*, void*), void* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp445); _temp450;}); _temp449;}); _LL426:;} struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2; struct Cyc_List_List**
places; struct _RegionHandle* rgn; } ; void* Cyc_CfFlowInfo_join_escape( void*
esc1, void* esc2){ struct _tuple3 _temp452=({ struct _tuple3 _temp451; _temp451.f1=
esc1; _temp451.f2= esc2; _temp451;}); void* _temp460; void* _temp462; void*
_temp464; void* _temp466; _LL454: _LL463: _temp462= _temp452.f1; if( _temp462 ==(
void*) Cyc_CfFlowInfo_Esc){ goto _LL461;} else{ goto _LL456;} _LL461: _temp460=
_temp452.f2; goto _LL455; _LL456: _LL467: _temp466= _temp452.f1; goto _LL465;
_LL465: _temp464= _temp452.f2; if( _temp464 ==( void*) Cyc_CfFlowInfo_Esc){ goto
_LL457;} else{ goto _LL458;} _LL458: goto _LL459; _LL455: goto _LL457; _LL457:
return( void*) Cyc_CfFlowInfo_Esc; _LL459: return( void*) Cyc_CfFlowInfo_Unesc;
_LL453:;} void* Cyc_CfFlowInfo_join_initlevel( void* il1, void* il2){ struct
_tuple3 _temp469=({ struct _tuple3 _temp468; _temp468.f1= il1; _temp468.f2= il2;
_temp468;}); void* _temp481; void* _temp483; void* _temp485; void* _temp487;
void* _temp489; void* _temp491; void* _temp493; void* _temp495; _LL471: _LL484:
_temp483= _temp469.f1; if( _temp483 ==( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL482;} else{ goto _LL473;} _LL482: _temp481= _temp469.f2; goto _LL472; _LL473:
_LL488: _temp487= _temp469.f1; goto _LL486; _LL486: _temp485= _temp469.f2; if(
_temp485 ==( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL474;} else{ goto _LL475;}
_LL475: _LL492: _temp491= _temp469.f1; if( _temp491 ==( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL490;} else{ goto _LL477;} _LL490: _temp489= _temp469.f2; goto _LL476;
_LL477: _LL496: _temp495= _temp469.f1; goto _LL494; _LL494: _temp493= _temp469.f2;
if( _temp493 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL478;} else{ goto _LL479;}
_LL479: goto _LL480; _LL472: goto _LL474; _LL474: return( void*) Cyc_CfFlowInfo_NoneIL;
_LL476: goto _LL478; _LL478: return( void*) Cyc_CfFlowInfo_ThisIL; _LL480:
return( void*) Cyc_CfFlowInfo_AllIL; _LL470:;} void* Cyc_CfFlowInfo_join_initstate(
struct Cyc_CfFlowInfo_JoinEnv* env, void* state1, void* state2){ struct _tuple3
_temp498=({ struct _tuple3 _temp497; _temp497.f1= state1; _temp497.f2= state2;
_temp497;}); void* _temp508; void* _temp510; void* _temp512; void* _temp514;
void* _temp516; void* _temp518; void* _temp520; struct Cyc_CfFlowInfo_Place*
_temp522; void* _temp524; void* _temp526; void* _temp528; void* _temp530; void*
_temp532; void* _temp534; void* _temp536; struct Cyc_CfFlowInfo_Place* _temp538;
void* _temp540; struct Cyc_CfFlowInfo_Place* _temp542; void* _temp544; struct
Cyc_CfFlowInfo_Place* _temp546; _LL500: _LL515: _temp514= _temp498.f1; if(*((
int*) _temp514) == Cyc_CfFlowInfo_UnknownIS){ _LL519: _temp518=( void*)(( struct
Cyc_CfFlowInfo_UnknownIS_struct*) _temp514)->f1; goto _LL517; _LL517: _temp516=(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp514)->f2; goto _LL509;}
else{ goto _LL502;} _LL509: _temp508= _temp498.f2; if(*(( int*) _temp508) == Cyc_CfFlowInfo_UnknownIS){
_LL513: _temp512=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp508)->f1;
goto _LL511; _LL511: _temp510=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp508)->f2; goto _LL501;} else{ goto _LL502;} _LL502: _LL525: _temp524=
_temp498.f1; if(*(( int*) _temp524) == Cyc_CfFlowInfo_UnknownIS){ _LL529:
_temp528=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp524)->f1; goto
_LL527; _LL527: _temp526=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp524)->f2; goto _LL521;} else{ goto _LL504;} _LL521: _temp520= _temp498.f2;
if(*(( int*) _temp520) == Cyc_CfFlowInfo_MustPointTo){ _LL523: _temp522=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp520)->f1; goto _LL503;} else{
goto _LL504;} _LL504: _LL537: _temp536= _temp498.f1; if(*(( int*) _temp536) ==
Cyc_CfFlowInfo_MustPointTo){ _LL539: _temp538=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp536)->f1; goto _LL531;} else{ goto _LL506;} _LL531: _temp530= _temp498.f2;
if(*(( int*) _temp530) == Cyc_CfFlowInfo_UnknownIS){ _LL535: _temp534=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp530)->f1; goto _LL533; _LL533:
_temp532=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp530)->f2; goto
_LL505;} else{ goto _LL506;} _LL506: _LL545: _temp544= _temp498.f1; if(*(( int*)
_temp544) == Cyc_CfFlowInfo_MustPointTo){ _LL547: _temp546=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp544)->f1; goto _LL541;} else{ goto _LL499;} _LL541: _temp540= _temp498.f2;
if(*(( int*) _temp540) == Cyc_CfFlowInfo_MustPointTo){ _LL543: _temp542=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp540)->f1; goto _LL507;} else{
goto _LL499;} _LL501: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_join_escape(
_temp518, _temp512), Cyc_CfFlowInfo_join_initlevel( _temp516, _temp510)); _LL503: {
struct _tuple6 _temp548=({ struct _tuple6 _temp549; _temp549.f1= env->d2;
_temp549.f2= 0; _temp549;}); if( _temp526 ==( void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp548, Cyc_CfFlowInfo_lookup_place( env->d2, _temp522)): 0){ Cyc_CfFlowInfo_add_place(
env->rgn, env->places, _temp522); return Cyc_CfFlowInfo_mkLeafPI( _temp528,
_temp526);} return Cyc_CfFlowInfo_mkLeafPI( _temp528, Cyc_CfFlowInfo_join_initlevel(
_temp526,( void*) Cyc_CfFlowInfo_ThisIL));} _LL505: { struct _tuple6 _temp550=({
struct _tuple6 _temp551; _temp551.f1= env->d1; _temp551.f2= 0; _temp551;}); if(
_temp532 ==( void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp550, Cyc_CfFlowInfo_lookup_place( env->d1, _temp538)): 0){ Cyc_CfFlowInfo_add_place(
env->rgn, env->places, _temp538); return Cyc_CfFlowInfo_mkLeafPI( _temp534,
_temp532);} return Cyc_CfFlowInfo_mkLeafPI( _temp534, Cyc_CfFlowInfo_join_initlevel((
void*) Cyc_CfFlowInfo_ThisIL, _temp532));} _LL507: if( Cyc_CfFlowInfo_place_cmp(
_temp546, _temp542) == 0){ return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct*
_temp552=( struct Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp552[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp553; _temp553.tag= Cyc_CfFlowInfo_LeafPI;
_temp553.f1=( void*) state1; _temp553;}); _temp552;});}{ struct _tuple6 _temp554=({
struct _tuple6 _temp557; _temp557.f1= env->d1; _temp557.f2= 0; _temp557;});
struct _tuple6 _temp555=({ struct _tuple6 _temp556; _temp556.f1= env->d2;
_temp556.f2= 0; _temp556;}); if( Cyc_CfFlowInfo_isAllInit_pinfo(& _temp554, Cyc_CfFlowInfo_lookup_place(
env->d1, _temp546))? Cyc_CfFlowInfo_isAllInit_pinfo(& _temp555, Cyc_CfFlowInfo_lookup_place(
env->d2, _temp542)): 0){ Cyc_CfFlowInfo_add_place( env->rgn, env->places,
_temp546); Cyc_CfFlowInfo_add_place( env->rgn, env->places, _temp542); return
Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,( void*) Cyc_CfFlowInfo_AllIL);}
return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,( void*) Cyc_CfFlowInfo_ThisIL);}
_LL499:;} void* Cyc_CfFlowInfo_join_pathinfo( struct Cyc_CfFlowInfo_JoinEnv* env,
void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return pinfo1;}{ struct
_tuple3 _temp559=({ struct _tuple3 _temp558; _temp558.f1= pinfo1; _temp558.f2=
pinfo2; _temp558;}); void* _temp569; void* _temp571; void* _temp573; void*
_temp575; void* _temp577; struct Cyc_Dict_Dict* _temp579; void* _temp581; struct
Cyc_Dict_Dict* _temp583; void* _temp585; struct Cyc_Dict_Dict* _temp587; void*
_temp589; struct Cyc_Dict_Dict* _temp591; _LL561: _LL574: _temp573= _temp559.f1;
if(*(( int*) _temp573) == Cyc_CfFlowInfo_LeafPI){ _LL576: _temp575=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp573)->f1; goto _LL570;} else{ goto
_LL563;} _LL570: _temp569= _temp559.f2; if(*(( int*) _temp569) == Cyc_CfFlowInfo_LeafPI){
_LL572: _temp571=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp569)->f1;
goto _LL562;} else{ goto _LL563;} _LL563: _LL582: _temp581= _temp559.f1; if(*((
int*) _temp581) == Cyc_CfFlowInfo_TuplePI){ _LL584: _temp583=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp581)->f1; goto _LL578;} else{ goto _LL565;} _LL578: _temp577= _temp559.f2;
if(*(( int*) _temp577) == Cyc_CfFlowInfo_TuplePI){ _LL580: _temp579=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp577)->f1; goto _LL564;} else{ goto _LL565;}
_LL565: _LL590: _temp589= _temp559.f1; if(*(( int*) _temp589) == Cyc_CfFlowInfo_StructPI){
_LL592: _temp591=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp589)->f1; goto
_LL586;} else{ goto _LL567;} _LL586: _temp585= _temp559.f2; if(*(( int*)
_temp585) == Cyc_CfFlowInfo_StructPI){ _LL588: _temp587=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp585)->f1; goto _LL566;} else{ goto _LL567;} _LL567: goto _LL568; _LL562:
if( _temp575 == _temp571){ return pinfo1;} return Cyc_CfFlowInfo_join_initstate(
env, _temp575, _temp571); _LL564: if( _temp583 == _temp579){ return pinfo1;}
return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp593=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp593[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp594; _temp594.tag= Cyc_CfFlowInfo_TuplePI;
_temp594.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,
env, _temp583, _temp579); _temp594;}); _temp593;}); _LL566: if( _temp591 ==
_temp587){ return pinfo1;} return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp595=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp595[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp596; _temp596.tag=
Cyc_CfFlowInfo_StructPI; _temp596.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo, env, _temp591, _temp587); _temp596;}); _temp595;});
_LL568:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp597=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp597[ 0]=({ struct Cyc_Core_Impossible_struct _temp598; _temp598.tag= Cyc_Core_Impossible;
_temp598.f1= _tag_arr("join_pathinfo", sizeof( unsigned char), 14u); _temp598;});
_temp597;})); _LL560:;}} void* Cyc_CfFlowInfo_join_flow( void* f1, void* f2){
if( f1 == f2){ return f1;}{ struct _tuple3 _temp600=({ struct _tuple3 _temp599;
_temp599.f1= f1; _temp599.f2= f2; _temp599;}); void* _temp608; void* _temp610;
void* _temp612; void* _temp614; void* _temp616; struct Cyc_Dict_Dict* _temp618;
void* _temp620; struct Cyc_Dict_Dict* _temp622; _LL602: _LL611: _temp610=
_temp600.f1; if( _temp610 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL609;}
else{ goto _LL604;} _LL609: _temp608= _temp600.f2; goto _LL603; _LL604: _LL615:
_temp614= _temp600.f1; goto _LL613; _LL613: _temp612= _temp600.f2; if( _temp612
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL605;} else{ goto _LL606;} _LL606:
_LL621: _temp620= _temp600.f1; if(( unsigned int) _temp620 > 1u?*(( int*)
_temp620) == Cyc_CfFlowInfo_InitsFL: 0){ _LL623: _temp622=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp620)->f1; goto _LL617;} else{ goto _LL601;} _LL617: _temp616= _temp600.f2;
if(( unsigned int) _temp616 > 1u?*(( int*) _temp616) == Cyc_CfFlowInfo_InitsFL:
0){ _LL619: _temp618=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp616)->f1;
goto _LL607;} else{ goto _LL601;} _LL603: return f2; _LL605: return f1; _LL607:
if( _temp622 == _temp618){ return f1;} if( Cyc_CfFlowInfo_flow_lessthan_approx(
f1, f2)){ return f2;} if( Cyc_CfFlowInfo_flow_lessthan_approx( f2, f1)){ return
f1;}{ struct _RegionHandle _temp624= _new_region(); struct _RegionHandle* r=&
_temp624; _push_region( r);{ struct Cyc_List_List* _temp625= 0; struct Cyc_CfFlowInfo_JoinEnv
_temp626=({ struct Cyc_CfFlowInfo_JoinEnv _temp631; _temp631.d1= _temp622;
_temp631.d2= _temp618; _temp631.places=& _temp625; _temp631.rgn= r; _temp631;});
struct Cyc_Dict_Dict* _temp627=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct
Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env,
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,&
_temp626, _temp622, _temp618); void* _temp630=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp628=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp628[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp629; _temp629.tag= Cyc_CfFlowInfo_InitsFL;
_temp629.f1= Cyc_CfFlowInfo_escape_these( r, _temp625, _temp627); _temp629;});
_temp628;}); _npop_handler( 0u); return _temp630;}; _pop_region( r);} _LL601:;}}
int Cyc_CfFlowInfo_initstate_lessthan_approx( void* state1, void* state2){
struct _tuple3 _temp633=({ struct _tuple3 _temp632; _temp632.f1= state1;
_temp632.f2= state2; _temp632;}); void* _temp641; void* _temp643; void* _temp645;
void* _temp647; void* _temp649; void* _temp651; void* _temp653; struct Cyc_CfFlowInfo_Place*
_temp655; void* _temp657; struct Cyc_CfFlowInfo_Place* _temp659; _LL635: _LL648:
_temp647= _temp633.f1; if(*(( int*) _temp647) == Cyc_CfFlowInfo_UnknownIS){
_LL652: _temp651=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp647)->f1;
goto _LL650; _LL650: _temp649=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp647)->f2; goto _LL642;} else{ goto _LL637;} _LL642: _temp641= _temp633.f2;
if(*(( int*) _temp641) == Cyc_CfFlowInfo_UnknownIS){ _LL646: _temp645=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp641)->f1; goto _LL644; _LL644:
_temp643=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp641)->f2; goto
_LL636;} else{ goto _LL637;} _LL637: _LL658: _temp657= _temp633.f1; if(*(( int*)
_temp657) == Cyc_CfFlowInfo_MustPointTo){ _LL660: _temp659=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp657)->f1; goto _LL654;} else{ goto _LL639;} _LL654: _temp653= _temp633.f2;
if(*(( int*) _temp653) == Cyc_CfFlowInfo_MustPointTo){ _LL656: _temp655=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp653)->f1; goto _LL638;} else{
goto _LL639;} _LL639: goto _LL640; _LL636: return Cyc_CfFlowInfo_join_escape(
_temp651, _temp645) == _temp645? Cyc_CfFlowInfo_join_initlevel( _temp649,
_temp643) == _temp643: 0; _LL638: return Cyc_CfFlowInfo_place_cmp( _temp659,
_temp655) == 0; _LL640: return 0; _LL634:;} int Cyc_CfFlowInfo_pinfo_lessthan_approx(
void* ignore, void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return 1;}{
struct _tuple3 _temp662=({ struct _tuple3 _temp661; _temp661.f1= pinfo1;
_temp661.f2= pinfo2; _temp661;}); void* _temp672; void* _temp674; void* _temp676;
void* _temp678; void* _temp680; struct Cyc_Dict_Dict* _temp682; void* _temp684;
struct Cyc_Dict_Dict* _temp686; void* _temp688; struct Cyc_Dict_Dict* _temp690;
void* _temp692; struct Cyc_Dict_Dict* _temp694; _LL664: _LL677: _temp676=
_temp662.f1; if(*(( int*) _temp676) == Cyc_CfFlowInfo_LeafPI){ _LL679: _temp678=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp676)->f1; goto _LL673;}
else{ goto _LL666;} _LL673: _temp672= _temp662.f2; if(*(( int*) _temp672) == Cyc_CfFlowInfo_LeafPI){
_LL675: _temp674=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp672)->f1;
goto _LL665;} else{ goto _LL666;} _LL666: _LL685: _temp684= _temp662.f1; if(*((
int*) _temp684) == Cyc_CfFlowInfo_TuplePI){ _LL687: _temp686=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp684)->f1; goto _LL681;} else{ goto _LL668;} _LL681: _temp680= _temp662.f2;
if(*(( int*) _temp680) == Cyc_CfFlowInfo_TuplePI){ _LL683: _temp682=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp680)->f1; goto _LL667;} else{ goto _LL668;}
_LL668: _LL693: _temp692= _temp662.f1; if(*(( int*) _temp692) == Cyc_CfFlowInfo_StructPI){
_LL695: _temp694=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp692)->f1; goto
_LL689;} else{ goto _LL670;} _LL689: _temp688= _temp662.f2; if(*(( int*)
_temp688) == Cyc_CfFlowInfo_StructPI){ _LL691: _temp690=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp688)->f1; goto _LL669;} else{ goto _LL670;} _LL670: goto _LL671; _LL665:
return _temp678 == _temp674? 1: Cyc_CfFlowInfo_initstate_lessthan_approx(
_temp678, _temp674); _LL667: return _temp686 == _temp682? 1:(( int(*)( int(* f)(
int, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( int ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp686, _temp682); _LL669: return _temp694 == _temp690? 1:(( int(*)( int(* f)(
struct _tagged_arr*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_arr* ignore, void*
pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp694, _temp690);
_LL671:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp696=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp696[ 0]=({ struct Cyc_Core_Impossible_struct _temp697; _temp697.tag= Cyc_Core_Impossible;
_temp697.f1= _tag_arr("pinfo_lessthan", sizeof( unsigned char), 15u); _temp697;});
_temp696;})); _LL663:;}} int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void*
f2){ if( f1 == f2){ return 1;}{ struct _tuple3 _temp699=({ struct _tuple3
_temp698; _temp698.f1= f1; _temp698.f2= f2; _temp698;}); void* _temp707; void*
_temp709; void* _temp711; void* _temp713; void* _temp715; struct Cyc_Dict_Dict*
_temp717; void* _temp719; struct Cyc_Dict_Dict* _temp721; _LL701: _LL710:
_temp709= _temp699.f1; if( _temp709 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL708;} else{ goto _LL703;} _LL708: _temp707= _temp699.f2; goto _LL702; _LL703:
_LL714: _temp713= _temp699.f1; goto _LL712; _LL712: _temp711= _temp699.f2; if(
_temp711 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL704;} else{ goto _LL705;}
_LL705: _LL720: _temp719= _temp699.f1; if(( unsigned int) _temp719 > 1u?*(( int*)
_temp719) == Cyc_CfFlowInfo_InitsFL: 0){ _LL722: _temp721=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp719)->f1; goto _LL716;} else{ goto _LL700;} _LL716: _temp715= _temp699.f2;
if(( unsigned int) _temp715 > 1u?*(( int*) _temp715) == Cyc_CfFlowInfo_InitsFL:
0){ _LL718: _temp717=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp715)->f1;
goto _LL706;} else{ goto _LL700;} _LL702: return 1; _LL704: return 0; _LL706:
if( _temp721 == _temp717){ return 1;} return(( int(*)( int(* f)( void*, void*,
void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( void* ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp721, _temp717); _LL700:;}}