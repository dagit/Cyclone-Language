 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern struct _RegionHandle*
Cyc_Core_heap_region; extern unsigned char* string_to_Cstring( struct
_tagged_string); extern unsigned char* underlying_Cstring( struct _tagged_string);
extern struct _tagged_string Cstring_to_string( unsigned char*); extern int
system( unsigned char*); struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; struct _tagged_ptr0{ void** curr; void** base; void** last_plus_one; } ;
extern struct Cyc_List_List* Cyc_List_list( struct _tagged_ptr0); extern struct
Cyc_List_List* Cyc_List_rlist( struct _RegionHandle*, struct _tagged_ptr0);
extern int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c(
struct _RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_rrevappend( struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_rrev( struct _RegionHandle*, struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rflatten(
struct _RegionHandle*, struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[
8u]; extern int Cyc_List_forall( int(* pred)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rfrom_array( struct _RegionHandle* r2,
struct _tagged_ptr0 arr); extern int Cyc_String_zstrptrcmp( struct
_tagged_string*, struct _tagged_string*); extern struct _tagged_string Cyc_String_strconcat(
struct _tagged_string, struct _tagged_string); extern struct _tagged_string Cyc_String_implode(
struct Cyc_List_List* c); struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty(
int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct
Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set* s,
void* elt); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key);
extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void*
key); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; extern struct _tagged_string Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*); static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar;
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp(
struct _tuple0*, struct _tuple0*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void*
Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_star_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_cstar_typ( void* t,
struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ(); extern
void* Cyc_Absyn_strct( struct _tagged_string* name); extern void* Cyc_Absyn_strctq(
struct _tuple0* name); extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_string s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*, struct Cyc_List_List*
es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp( struct
Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*, void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_pointer_exn_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt( struct _tagged_string*
v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x, void*
t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field(
struct Cyc_List_List*, struct _tagged_string*); struct Cyc_PP_Ppstate; struct
Cyc_PP_Out; struct Cyc_PP_Doc; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*);
extern struct _tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); extern
struct _tagged_string Cyc_Absynpp_prim2string( void* p); static const int Cyc_Tcenv_VarRes=
0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void* Cyc_Tcutil_typ_kind(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify(
void*, void*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern void*
Cyc_Tcutil_snd_tqt( struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_string s, int i); extern struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds); extern struct _tuple0* Cyc_Toc_temp_var(); int Cyc_Toc_use_runtimecheck_funs=
1; static struct Cyc_List_List* Cyc_Toc_result_decls= 0; struct Cyc_Absyn_Abs_n_struct
Cyc_Toc_abs_ns_value={ 1u, 0}; void* Cyc_Toc_abs_ns=( void*)& Cyc_Toc_abs_ns_value;
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={ 0, 0, 0}; unsigned char Cyc_Toc_Toc_Unimplemented[
22u]="\000\000\000\000Toc_Unimplemented"; unsigned char Cyc_Toc_Toc_Impossible[
19u]="\000\000\000\000Toc_Impossible"; static void* Cyc_Toc_unimp( struct
_tagged_string s){({ struct _tagged_string _temp1= s; fprintf( sfile_to_file(
Cyc_Stdio_stderr),"%.*s\n", _temp1.last_plus_one - _temp1.curr, _temp1.curr);});
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw((
void*) Cyc_Toc_Toc_Unimplemented);} static void* Cyc_Toc_toc_impos( struct
_tagged_string s){({ struct _tagged_string _temp2= s; fprintf( sfile_to_file(
Cyc_Stdio_stderr),"%.*s\n", _temp2.last_plus_one - _temp2.curr, _temp2.curr);});
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw((
void*) Cyc_Toc_Toc_Impossible);} unsigned char Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error";
static unsigned char _temp5[ 5u]="curr"; static struct _tagged_string Cyc_Toc_curr_string={
_temp5, _temp5, _temp5 + 5u}; static struct _tagged_string* Cyc_Toc_curr_sp=&
Cyc_Toc_curr_string; static unsigned char _temp8[ 14u]="last_plus_one"; static
struct _tagged_string Cyc_Toc_last_plus_one_string={ _temp8, _temp8, _temp8 + 14u};
static struct _tagged_string* Cyc_Toc_last_plus_one_sp=& Cyc_Toc_last_plus_one_string;
static unsigned char _temp11[ 5u]="base"; static struct _tagged_string Cyc_Toc_base_string={
_temp11, _temp11, _temp11 + 5u}; static struct _tagged_string* Cyc_Toc_base_sp=&
Cyc_Toc_base_string; static unsigned char _temp14[ 15u]="_tagged_string"; static
struct _tagged_string Cyc_Toc__tagged_string_string={ _temp14, _temp14, _temp14
+ 15u}; static struct _tagged_string* Cyc_Toc__tagged_string_sp=& Cyc_Toc__tagged_string_string;
static unsigned char _temp17[ 8u]="xtunion"; static struct _tagged_string Cyc_Toc_xtunion_string={
_temp17, _temp17, _temp17 + 8u}; static struct _tagged_string* Cyc_Toc_xtunion_sp=&
Cyc_Toc_xtunion_string; static unsigned char _temp20[ 4u]="tag"; static struct
_tagged_string Cyc_Toc_tag_string={ _temp20, _temp20, _temp20 + 4u}; static
struct _tagged_string* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string; static unsigned char
_temp23[ 14u]="_handler_cons"; static struct _tagged_string Cyc_Toc__handler_cons_string={
_temp23, _temp23, _temp23 + 14u}; static struct _tagged_string* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static unsigned char _temp26[ 13u]="_push_region";
static struct _tagged_string Cyc_Toc__push_region_string={ _temp26, _temp26,
_temp26 + 13u}; static struct _tagged_string* Cyc_Toc__push_region_sp=& Cyc_Toc__push_region_string;
static unsigned char _temp29[ 12u]="_pop_region"; static struct _tagged_string
Cyc_Toc__pop_region_string={ _temp29, _temp29, _temp29 + 12u}; static struct
_tagged_string* Cyc_Toc__pop_region_sp=& Cyc_Toc__pop_region_string; static
unsigned char _temp32[ 8u]="handler"; static struct _tagged_string Cyc_Toc_handler_string={
_temp32, _temp32, _temp32 + 8u}; static struct _tagged_string* Cyc_Toc_handler_sp=&
Cyc_Toc_handler_string; static unsigned char _temp35[ 16u]="Match_Exception";
static struct _tagged_string Cyc_Toc_Match_Exception_string={ _temp35, _temp35,
_temp35 + 16u}; static struct _tagged_string* Cyc_Toc_Match_Exception_sp=& Cyc_Toc_Match_Exception_string;
static unsigned char _temp38[ 15u]="Cyc_new_string"; static struct
_tagged_string Cyc_Toc_Cyc_new_string_string={ _temp38, _temp38, _temp38 + 15u};
static struct _tagged_string* Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string;
static unsigned char _temp41[ 16u]="Cyc_rnew_string"; static struct
_tagged_string Cyc_Toc_Cyc_rnew_string_string={ _temp41, _temp41, _temp41 + 16u};
static struct _tagged_string* Cyc_Toc_Cyc_rnew_string_sp=& Cyc_Toc_Cyc_rnew_string_string;
static unsigned char _temp44[ 14u]="_RegionHandle"; static struct _tagged_string
Cyc_Toc__RegionHandle_string={ _temp44, _temp44, _temp44 + 14u}; static struct
_tagged_string* Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string; static
unsigned char _temp47[ 12u]="_new_region"; static struct _tagged_string Cyc_Toc__new_region_string={
_temp47, _temp47, _temp47 + 12u}; static struct _tagged_string* Cyc_Toc__new_region_sp=&
Cyc_Toc__new_region_string; static unsigned char _temp50[ 7u]="_throw"; static
struct _tagged_string Cyc_Toc__throw_string={ _temp50, _temp50, _temp50 + 7u};
static struct _tuple0 Cyc_Toc__throw_pr={( void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__throw_string};
static struct _tuple0* Cyc_Toc__throw_qv=& Cyc_Toc__throw_pr; static
unsigned char _temp53[ 7u]="setjmp"; static struct _tagged_string Cyc_Toc_setjmp_string={
_temp53, _temp53, _temp53 + 7u}; static struct _tuple0 Cyc_Toc_setjmp_pr={( void*)&
Cyc_Toc_abs_ns_value,& Cyc_Toc_setjmp_string}; static struct _tuple0* Cyc_Toc_setjmp_qv=&
Cyc_Toc_setjmp_pr; static unsigned char _temp56[ 14u]="_push_handler"; static
struct _tagged_string Cyc_Toc__push_handler_string={ _temp56, _temp56, _temp56 +
14u}; static struct _tuple0 Cyc_Toc__push_handler_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__push_handler_string}; static struct _tuple0* Cyc_Toc__push_handler_qv=&
Cyc_Toc__push_handler_pr; static unsigned char _temp59[ 13u]="_pop_handler";
static struct _tagged_string Cyc_Toc__pop_handler_string={ _temp59, _temp59,
_temp59 + 13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__pop_handler_string}; static struct _tuple0* Cyc_Toc__pop_handler_qv=&
Cyc_Toc__pop_handler_pr; static unsigned char _temp62[ 12u]="_exn_thrown";
static struct _tagged_string Cyc_Toc__exn_thrown_string={ _temp62, _temp62,
_temp62 + 12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__exn_thrown_string}; static struct _tuple0* Cyc_Toc__exn_thrown_qv=& Cyc_Toc__exn_thrown_pr;
static unsigned char _temp65[ 14u]="_npop_handler"; static struct _tagged_string
Cyc_Toc__npop_handler_string={ _temp65, _temp65, _temp65 + 14u}; static struct
_tuple0 Cyc_Toc__npop_handler_pr={( void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__npop_handler_string};
static struct _tuple0* Cyc_Toc__npop_handler_qv=& Cyc_Toc__npop_handler_pr;
static unsigned char _temp68[ 12u]="_check_null"; static struct _tagged_string
Cyc_Toc__check_null_string={ _temp68, _temp68, _temp68 + 12u}; static struct
_tuple0 Cyc_Toc__check_null_pr={( void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__check_null_string};
static struct _tuple0* Cyc_Toc__check_null_qv=& Cyc_Toc__check_null_pr; static
unsigned char _temp71[ 25u]="_check_unknown_subscript"; static struct
_tagged_string Cyc_Toc__check_unknown_subscript_string={ _temp71, _temp71,
_temp71 + 25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__check_unknown_subscript_string}; static
struct _tuple0* Cyc_Toc__check_unknown_subscript_qv=& Cyc_Toc__check_unknown_subscript_pr;
static unsigned char _temp74[ 28u]="_check_known_subscript_null"; static struct
_tagged_string Cyc_Toc__check_known_subscript_null_string={ _temp74, _temp74,
_temp74 + 28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__check_known_subscript_null_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_null_qv=& Cyc_Toc__check_known_subscript_null_pr;
static unsigned char _temp77[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_string Cyc_Toc__check_known_subscript_notnull_string={ _temp77,
_temp77, _temp77 + 31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__check_known_subscript_notnull_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_notnull_qv=& Cyc_Toc__check_known_subscript_notnull_pr;
static unsigned char _temp80[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_GC_malloc_string={
_temp80, _temp80, _temp80 + 10u}; static struct _tuple0 Cyc_Toc_GC_malloc_pr={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc_GC_malloc_string}; static struct _tuple0*
Cyc_Toc_GC_malloc_qv=& Cyc_Toc_GC_malloc_pr; static unsigned char _temp83[ 17u]="GC_malloc_atomic";
static struct _tagged_string Cyc_Toc_GC_malloc_atomic_string={ _temp83, _temp83,
_temp83 + 17u}; static struct _tuple0 Cyc_Toc_GC_malloc_atomic_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc_GC_malloc_atomic_string}; static struct _tuple0* Cyc_Toc_GC_malloc_atomic_qv=&
Cyc_Toc_GC_malloc_atomic_pr; static unsigned char _temp86[ 15u]="_region_malloc";
static struct _tagged_string Cyc_Toc__region_malloc_string={ _temp86, _temp86,
_temp86 + 15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__region_malloc_string}; static struct _tuple0* Cyc_Toc__region_malloc_qv=&
Cyc_Toc__region_malloc_pr; struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp87=(
void*) e->r; void* _temp113; unsigned char _temp115; void* _temp117; void*
_temp119; short _temp121; void* _temp123; void* _temp125; int _temp127; void*
_temp129; void* _temp131; long long _temp133; void* _temp135; void* _temp137;
struct Cyc_Absyn_Exp* _temp139; void* _temp141; struct Cyc_List_List* _temp143;
struct Cyc_List_List* _temp145; struct Cyc_Absyn_Structdecl* _temp147; struct
Cyc_List_List* _temp149; struct Cyc_Core_Opt* _temp151; struct _tuple0* _temp153;
struct Cyc_List_List* _temp155; struct _tuple1* _temp157; struct Cyc_List_List*
_temp159; struct Cyc_Core_Opt* _temp161; _LL89: if(*(( int*) _temp87) == Cyc_Absyn_Const_e){
_LL114: _temp113=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp87)->f1; if((
unsigned int) _temp113 > 1u?*(( int*) _temp113) == Cyc_Absyn_Char_c: 0){ _LL118:
_temp117=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp113)->f1; goto _LL116;
_LL116: _temp115=(( struct Cyc_Absyn_Char_c_struct*) _temp113)->f2; goto _LL90;}
else{ goto _LL91;}} else{ goto _LL91;} _LL91: if(*(( int*) _temp87) == Cyc_Absyn_Const_e){
_LL120: _temp119=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp87)->f1; if((
unsigned int) _temp119 > 1u?*(( int*) _temp119) == Cyc_Absyn_Short_c: 0){ _LL124:
_temp123=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp119)->f1; goto _LL122;
_LL122: _temp121=(( struct Cyc_Absyn_Short_c_struct*) _temp119)->f2; goto _LL92;}
else{ goto _LL93;}} else{ goto _LL93;} _LL93: if(*(( int*) _temp87) == Cyc_Absyn_Const_e){
_LL126: _temp125=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp87)->f1; if((
unsigned int) _temp125 > 1u?*(( int*) _temp125) == Cyc_Absyn_Int_c: 0){ _LL130:
_temp129=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp125)->f1; goto _LL128;
_LL128: _temp127=(( struct Cyc_Absyn_Int_c_struct*) _temp125)->f2; goto _LL94;}
else{ goto _LL95;}} else{ goto _LL95;} _LL95: if(*(( int*) _temp87) == Cyc_Absyn_Const_e){
_LL132: _temp131=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp87)->f1; if((
unsigned int) _temp131 > 1u?*(( int*) _temp131) == Cyc_Absyn_LongLong_c: 0){
_LL136: _temp135=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp131)->f1;
goto _LL134; _LL134: _temp133=(( struct Cyc_Absyn_LongLong_c_struct*) _temp131)->f2;
goto _LL96;} else{ goto _LL97;}} else{ goto _LL97;} _LL97: if(*(( int*) _temp87)
== Cyc_Absyn_Const_e){ _LL138: _temp137=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp87)->f1; if( _temp137 ==( void*) Cyc_Absyn_Null_c){ goto _LL98;} else{ goto
_LL99;}} else{ goto _LL99;} _LL99: if(*(( int*) _temp87) == Cyc_Absyn_Cast_e){
_LL142: _temp141=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp87)->f1; goto
_LL140; _LL140: _temp139=(( struct Cyc_Absyn_Cast_e_struct*) _temp87)->f2; goto
_LL100;} else{ goto _LL101;} _LL101: if(*(( int*) _temp87) == Cyc_Absyn_Tuple_e){
_LL144: _temp143=(( struct Cyc_Absyn_Tuple_e_struct*) _temp87)->f1; goto _LL102;}
else{ goto _LL103;} _LL103: if(*(( int*) _temp87) == Cyc_Absyn_Array_e){ _LL146:
_temp145=(( struct Cyc_Absyn_Array_e_struct*) _temp87)->f1; goto _LL104;} else{
goto _LL105;} _LL105: if(*(( int*) _temp87) == Cyc_Absyn_Struct_e){ _LL154:
_temp153=(( struct Cyc_Absyn_Struct_e_struct*) _temp87)->f1; goto _LL152; _LL152:
_temp151=(( struct Cyc_Absyn_Struct_e_struct*) _temp87)->f2; goto _LL150; _LL150:
_temp149=(( struct Cyc_Absyn_Struct_e_struct*) _temp87)->f3; goto _LL148; _LL148:
_temp147=(( struct Cyc_Absyn_Struct_e_struct*) _temp87)->f4; goto _LL106;} else{
goto _LL107;} _LL107: if(*(( int*) _temp87) == Cyc_Absyn_CompoundLit_e){ _LL158:
_temp157=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp87)->f1; goto _LL156;
_LL156: _temp155=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp87)->f2; goto
_LL108;} else{ goto _LL109;} _LL109: if(*(( int*) _temp87) == Cyc_Absyn_UnresolvedMem_e){
_LL162: _temp161=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp87)->f1; goto
_LL160; _LL160: _temp159=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp87)->f2;
goto _LL110;} else{ goto _LL111;} _LL111: goto _LL112; _LL90: return _temp115 =='\000';
_LL92: return _temp121 == 0; _LL94: return _temp127 == 0; _LL96: return _temp133
== 0; _LL98: return 1; _LL100: return Cyc_Toc_is_zero( _temp139); _LL102: return((
int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp143); _LL104: _temp149= _temp145; goto _LL106; _LL106:
_temp155= _temp149; goto _LL108; _LL108: _temp159= _temp155; goto _LL110; _LL110:
for( 0; _temp159 != 0; _temp159=(( struct Cyc_List_List*) _check_null( _temp159))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp159))->hd)).f2)){ return 0;}} return 1; _LL112: return 0; _LL88:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp163= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp169; struct Cyc_Absyn_Conref* _temp171; struct Cyc_Absyn_Tqual
_temp173; struct Cyc_Absyn_Conref* _temp175; void* _temp177; void* _temp179;
_LL165: if(( unsigned int) _temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_PointerType:
0){ _LL170: _temp169=(( struct Cyc_Absyn_PointerType_struct*) _temp163)->f1;
_LL180: _temp179=( void*) _temp169.elt_typ; goto _LL178; _LL178: _temp177=( void*)
_temp169.rgn_typ; goto _LL176; _LL176: _temp175= _temp169.nullable; goto _LL174;
_LL174: _temp173= _temp169.tq; goto _LL172; _LL172: _temp171= _temp169.bounds;
goto _LL166;} else{ goto _LL167;} _LL167: goto _LL168; _LL166: { void* _temp181=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp175))->v; int _temp187; _LL183: if(( unsigned int) _temp181 > 1u?*(( int*)
_temp181) == Cyc_Absyn_Eq_constr: 0){ _LL188: _temp187=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp181)->f1; goto _LL184;} else{ goto _LL185;} _LL185: goto _LL186; _LL184:
return _temp187; _LL186: return 0; _LL182:;} _LL168:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp189=( unsigned char*)"is_nullable";
struct _tagged_string _temp190; _temp190.curr= _temp189; _temp190.base= _temp189;
_temp190.last_plus_one= _temp189 + 12; _temp190;})); return 0; _LL164:;} static
struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x, struct
_tagged_string tag){ return({ struct _tuple0* _temp191=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp191->f1=(* x).f1; _temp191->f2=({
struct _tagged_string* _temp192=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp192[ 0]=( struct _tagged_string) Cyc_String_strconcat(*(*
x).f2, tag); _temp192;}); _temp191;});} static void Cyc_Toc_add_tuple_decl(
struct _tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List*
_temp193= 0; goto _LL194; _LL194:{ int i= 1; for( 0; ts != 0;( ts=(( struct Cyc_List_List*)
_check_null( ts))->tl, i ++)){ _temp193=({ struct Cyc_List_List* _temp195=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp195->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp196=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp196->name=({ struct
_tagged_string* _temp197=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp197[ 0]=( struct _tagged_string) xprintf("f%d", i);
_temp197;}); _temp196->tq= Cyc_Toc_mt_tq; _temp196->type=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp196->width= 0; _temp196->attributes=
0; _temp196;}); _temp195->tl= _temp193; _temp195;});}} _temp193=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp193);{ struct Cyc_Absyn_Structdecl*
_temp202=({ struct Cyc_Absyn_Structdecl* _temp198=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp198->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp198->name=({ struct Cyc_Core_Opt* _temp200=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp200->v=( void*)({ struct _tuple0*
_temp201=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp201->f1=
Cyc_Toc_abs_ns; _temp201->f2= x; _temp201;}); _temp200;}); _temp198->tvs= 0;
_temp198->fields=({ struct Cyc_Core_Opt* _temp199=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp199->v=( void*) _temp193;
_temp199;}); _temp198->attributes= 0; _temp198;}); goto _LL203; _LL203: Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp204=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp204->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp205=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp205[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp206; _temp206.tag= Cyc_Absyn_Struct_d; _temp206.f1=
_temp202; _temp206;}); _temp205;}), 0); _temp204->tl= Cyc_Toc_result_decls;
_temp204;});}} struct _tagged_ptr1{ struct Cyc_Absyn_Structfield** curr; struct
Cyc_Absyn_Structfield** base; struct Cyc_Absyn_Structfield** last_plus_one; } ;
static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x, void* t){
void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Structfield* _temp208=({ struct Cyc_Absyn_Structfield* _temp207=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp207->name= Cyc_Toc_curr_sp; _temp207->tq= Cyc_Toc_mt_tq; _temp207->type=(
void*) tptr; _temp207->width= 0; _temp207->attributes= 0; _temp207;}); goto
_LL209; _LL209: { struct Cyc_Absyn_Structfield* _temp211=({ struct Cyc_Absyn_Structfield*
_temp210=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp210->name= Cyc_Toc_base_sp; _temp210->tq= Cyc_Toc_mt_tq; _temp210->type=(
void*) tptr; _temp210->width= 0; _temp210->attributes= 0; _temp210;}); goto
_LL212; _LL212: { struct Cyc_Absyn_Structfield* _temp214=({ struct Cyc_Absyn_Structfield*
_temp213=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp213->name= Cyc_Toc_last_plus_one_sp; _temp213->tq= Cyc_Toc_mt_tq; _temp213->type=(
void*) tptr; _temp213->width= 0; _temp213->attributes= 0; _temp213;}); goto
_LL215; _LL215: { struct Cyc_List_List* _temp222=({ struct Cyc_List_List*(*
_temp216)( struct _tagged_ptr1)=( struct Cyc_List_List*(*)( struct _tagged_ptr1))
Cyc_List_list; struct Cyc_Absyn_Structfield* _temp219= _temp208; struct Cyc_Absyn_Structfield*
_temp220= _temp211; struct Cyc_Absyn_Structfield* _temp221= _temp214; struct Cyc_Absyn_Structfield*
_temp217[ 3u]={ _temp219, _temp220, _temp221}; struct _tagged_ptr1 _temp218={
_temp217, _temp217, _temp217 + 3u}; _temp216( _temp218);}); goto _LL223; _LL223: {
struct Cyc_Absyn_Structdecl* _temp228=({ struct Cyc_Absyn_Structdecl* _temp224=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp224->sc=( void*)(( void*) Cyc_Absyn_Public); _temp224->name=({ struct Cyc_Core_Opt*
_temp226=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp226->v=( void*)({ struct _tuple0* _temp227=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp227->f1= Cyc_Toc_abs_ns; _temp227->f2= x;
_temp227;}); _temp226;}); _temp224->tvs= 0; _temp224->fields=({ struct Cyc_Core_Opt*
_temp225=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp225->v=( void*)(( struct Cyc_List_List*) _temp222); _temp225;}); _temp224->attributes=
0; _temp224;}); goto _LL229; _LL229: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp230=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp230->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp231=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp231[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp232; _temp232.tag= Cyc_Absyn_Struct_d;
_temp232.f1= _temp228; _temp232;}); _temp231;}), 0); _temp230->tl= Cyc_Toc_result_decls;
_temp230;});}}}}} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static
int Cyc_Toc_tuple_type_counter= 0; struct _tuple5{ struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static struct _tagged_string* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp233= Cyc_Toc_tuple_types;
goto _LL234; _LL234: for( 0; _temp233 != 0; _temp233=(( struct Cyc_List_List*)
_check_null( _temp233))->tl){ struct _tuple5 _temp237; struct Cyc_List_List*
_temp238; struct _tagged_string* _temp240; struct _tuple5* _temp235=( struct
_tuple5*)(( struct Cyc_List_List*) _check_null( _temp233))->hd; _temp237=*
_temp235; _LL241: _temp240= _temp237.f1; goto _LL239; _LL239: _temp238= _temp237.f2;
goto _LL236; _LL236: { int okay= 1; struct Cyc_List_List* _temp242= tqs0; goto
_LL243; _LL243: for( 0; _temp242 != 0? _temp238 != 0: 0;( _temp242=(( struct Cyc_List_List*)
_check_null( _temp242))->tl, _temp238=(( struct Cyc_List_List*) _check_null(
_temp238))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp242))->hd)).f2,( void*)(( struct Cyc_List_List*) _check_null(
_temp238))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp242 != 0)? 1: _temp238
!= 0){ continue;} return _temp240;}}}{ struct _tagged_string* x=({ struct
_tagged_string* _temp246=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp246[ 0]=( struct _tagged_string) xprintf("_tuple%d",
Cyc_Toc_tuple_type_counter ++); _temp246;}); struct Cyc_List_List* ts=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_snd_tqt, tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({
struct Cyc_List_List* _temp244=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp244->hd=( void*)({ struct _tuple5* _temp245=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp245->f1= x; _temp245->f2=
ts; _temp245;}); _temp244->tl= Cyc_Toc_tuple_types; _temp244;}); return x;}}
static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter=
0; static void* Cyc_Toc_tagged_string_type_v=( void*) 0u; static void* Cyc_Toc_tagged_string_type(){
if( Cyc_Toc_tagged_string_type_v ==( void*) Cyc_Absyn_VoidType){ Cyc_Toc_tagged_string_type_v=
Cyc_Absyn_strct( Cyc_Toc__tagged_string_sp);} return Cyc_Toc_tagged_string_type_v;}
struct _tuple6{ struct _tagged_string* f1; void* f2; } ; static void* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual tq){{ void* _temp247= t; void* _temp253; void*
_temp255; _LL249: if(( unsigned int) _temp247 > 4u?*(( int*) _temp247) == Cyc_Absyn_IntType:
0){ _LL256: _temp255=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp247)->f1;
if( _temp255 ==( void*) Cyc_Absyn_Unsigned){ goto _LL254;} else{ goto _LL251;}
_LL254: _temp253=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp247)->f2; if(
_temp253 ==( void*) Cyc_Absyn_B1){ goto _LL250;} else{ goto _LL251;}} else{ goto
_LL251;} _LL251: goto _LL252; _LL250: return Cyc_Toc_tagged_string_type();
_LL252: goto _LL248; _LL248:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=(( struct Cyc_List_List*) _check_null( tts))->tl){ struct
_tuple6 _temp259; void* _temp260; struct _tagged_string* _temp262; struct
_tuple6* _temp257=( struct _tuple6*)(( struct Cyc_List_List*) _check_null( tts))->hd;
_temp259=* _temp257; _LL263: _temp262= _temp259.f1; goto _LL261; _LL261:
_temp260= _temp259.f2; goto _LL258; _LL258: if( ! Cyc_Tcutil_unify( t, _temp260)){
continue;} return Cyc_Absyn_strct( _temp262);}}{ struct _tagged_string* x=({
struct _tagged_string* _temp266=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp266[ 0]=( struct _tagged_string) xprintf("_tagged_ptr%d",
Cyc_Toc_tagged_ptr_counter ++); _temp266;}); Cyc_Toc_add_tagged_ptr_decl( x, t);
Cyc_Toc_tagged_ptr_types=({ struct Cyc_List_List* _temp264=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp264->hd=( void*)({ struct
_tuple6* _temp265=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp265->f1= x; _temp265->f2= t; _temp265;}); _temp264->tl= Cyc_Toc_tagged_ptr_types;
_temp264;}); return Cyc_Absyn_strct( x);}} static int Cyc_Toc_temp_var_counter=
0; struct _tuple0* Cyc_Toc_temp_var(){ return({ struct _tuple0* _temp267=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp267->f1=( void*) Cyc_Absyn_Loc_n;
_temp267->f2=({ struct _tagged_string* _temp268=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp268[ 0]=( struct
_tagged_string) xprintf("_temp%d", Cyc_Toc_temp_var_counter ++); _temp268;});
_temp267;});} static int Cyc_Toc_fresh_label_counter= 0; static struct
_tagged_string* Cyc_Toc_fresh_label(){ return({ struct _tagged_string* _temp269=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp269[
0]=( struct _tagged_string) xprintf("_LL%d", Cyc_Toc_temp_var_counter ++);
_temp269;});} static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple1*
Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp272; struct Cyc_Absyn_Tqual
_temp274; struct Cyc_Core_Opt* _temp276; struct _tuple1 _temp270=* a; _LL277:
_temp276= _temp270.f1; goto _LL275; _LL275: _temp274= _temp270.f2; goto _LL273;
_LL273: _temp272= _temp270.f3; goto _LL271; _LL271: return({ struct _tuple1*
_temp278=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp278->f1=
_temp276; _temp278->f2= _temp274; _temp278->f3= Cyc_Toc_typ_to_c( _temp272);
_temp278;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp281; struct Cyc_Absyn_Tqual _temp283; struct _tuple3 _temp279=* x;
_LL284: _temp283= _temp279.f1; goto _LL282; _LL282: _temp281= _temp279.f2; goto
_LL280; _LL280: return({ struct _tuple3* _temp285=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp285->f1= _temp283; _temp285->f2= Cyc_Toc_typ_to_c(
_temp281); _temp285;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp286= t; struct Cyc_Absyn_Exp* _temp294; struct Cyc_Absyn_Tqual _temp296;
void* _temp298; struct Cyc_Core_Opt* _temp300; int _temp302; struct Cyc_Core_Opt*
_temp304; struct Cyc_Core_Opt _temp306; void* _temp307; struct Cyc_Core_Opt*
_temp309; _LL288: if(( unsigned int) _temp286 > 4u?*(( int*) _temp286) == Cyc_Absyn_ArrayType:
0){ _LL299: _temp298=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp286)->f1;
goto _LL297; _LL297: _temp296=(( struct Cyc_Absyn_ArrayType_struct*) _temp286)->f2;
goto _LL295; _LL295: _temp294=(( struct Cyc_Absyn_ArrayType_struct*) _temp286)->f3;
goto _LL289;} else{ goto _LL290;} _LL290: if(( unsigned int) _temp286 > 4u?*((
int*) _temp286) == Cyc_Absyn_Evar: 0){ _LL310: _temp309=(( struct Cyc_Absyn_Evar_struct*)
_temp286)->f1; goto _LL305; _LL305: _temp304=(( struct Cyc_Absyn_Evar_struct*)
_temp286)->f2; if( _temp304 == 0){ goto _LL292;} else{ _temp306=* _temp304;
_LL308: _temp307=( void*) _temp306.v; goto _LL303;} _LL303: _temp302=(( struct
Cyc_Absyn_Evar_struct*) _temp286)->f3; goto _LL301; _LL301: _temp300=(( struct
Cyc_Absyn_Evar_struct*) _temp286)->f4; goto _LL291;} else{ goto _LL292;} _LL292:
goto _LL293; _LL289: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp311=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp311[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp312; _temp312.tag= Cyc_Absyn_ArrayType;
_temp312.f1=( void*) Cyc_Toc_typ_to_c_array( _temp298); _temp312.f2= _temp296;
_temp312.f3= _temp294; _temp312;}); _temp311;}); _LL291: return Cyc_Toc_typ_to_c_array(
_temp307); _LL293: return Cyc_Toc_typ_to_c( t); _LL287:;} static void* Cyc_Toc_typ_to_c(
void* t); static struct Cyc_Absyn_Structfield* Cyc_Toc_structfield_to_c( struct
Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield* _temp313=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp313->name= f->name; _temp313->tq= f->tq; _temp313->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp313->width= f->width; _temp313->attributes= f->attributes;
_temp313;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp314= t; struct
Cyc_Core_Opt* _temp366; int _temp368; struct Cyc_Core_Opt* _temp370; struct Cyc_Core_Opt*
_temp372; struct Cyc_Core_Opt* _temp374; int _temp376; struct Cyc_Core_Opt*
_temp378; struct Cyc_Core_Opt _temp380; void* _temp381; struct Cyc_Core_Opt*
_temp383; struct Cyc_Absyn_Tvar* _temp385; struct Cyc_Absyn_TunionInfo _temp387;
void* _temp389; struct Cyc_List_List* _temp391; void* _temp393; struct Cyc_Absyn_Tuniondecl*
_temp395; struct Cyc_Absyn_TunionInfo _temp397; struct Cyc_Absyn_TunionFieldInfo
_temp399; struct Cyc_List_List* _temp401; void* _temp403; struct Cyc_Absyn_Tunionfield*
_temp405; struct Cyc_Absyn_Tuniondecl* _temp407; struct Cyc_Absyn_TunionFieldInfo
_temp409; struct Cyc_Absyn_PtrInfo _temp411; struct Cyc_Absyn_Conref* _temp413;
struct Cyc_Absyn_Tqual _temp415; struct Cyc_Absyn_Conref* _temp417; void*
_temp419; void* _temp421; void* _temp423; void* _temp425; struct Cyc_Absyn_Exp*
_temp427; struct Cyc_Absyn_Tqual _temp429; void* _temp431; struct Cyc_Absyn_FnInfo
_temp433; struct Cyc_List_List* _temp435; struct Cyc_Absyn_VarargInfo* _temp437;
int _temp439; struct Cyc_List_List* _temp441; void* _temp443; struct Cyc_Core_Opt*
_temp445; struct Cyc_List_List* _temp447; struct Cyc_List_List* _temp449; struct
Cyc_List_List* _temp451; struct Cyc_List_List* _temp453; struct Cyc_Absyn_Uniondecl**
_temp455; struct Cyc_List_List* _temp457; struct _tuple0* _temp459; struct Cyc_Absyn_Structdecl**
_temp461; struct Cyc_List_List* _temp463; struct _tuple0* _temp465; struct Cyc_Absyn_Enumdecl*
_temp467; struct _tuple0* _temp469; struct Cyc_Core_Opt* _temp471; struct Cyc_List_List*
_temp473; struct _tuple0* _temp475; void* _temp477; void* _temp479; struct Cyc_List_List*
_temp481; _LL316: if( _temp314 ==( void*) Cyc_Absyn_VoidType){ goto _LL317;}
else{ goto _LL318;} _LL318: if(( unsigned int) _temp314 > 4u?*(( int*) _temp314)
== Cyc_Absyn_Evar: 0){ _LL373: _temp372=(( struct Cyc_Absyn_Evar_struct*)
_temp314)->f1; goto _LL371; _LL371: _temp370=(( struct Cyc_Absyn_Evar_struct*)
_temp314)->f2; if( _temp370 == 0){ goto _LL369;} else{ goto _LL320;} _LL369:
_temp368=(( struct Cyc_Absyn_Evar_struct*) _temp314)->f3; goto _LL367; _LL367:
_temp366=(( struct Cyc_Absyn_Evar_struct*) _temp314)->f4; goto _LL319;} else{
goto _LL320;} _LL320: if(( unsigned int) _temp314 > 4u?*(( int*) _temp314) ==
Cyc_Absyn_Evar: 0){ _LL384: _temp383=(( struct Cyc_Absyn_Evar_struct*) _temp314)->f1;
goto _LL379; _LL379: _temp378=(( struct Cyc_Absyn_Evar_struct*) _temp314)->f2;
if( _temp378 == 0){ goto _LL322;} else{ _temp380=* _temp378; _LL382: _temp381=(
void*) _temp380.v; goto _LL377;} _LL377: _temp376=(( struct Cyc_Absyn_Evar_struct*)
_temp314)->f3; goto _LL375; _LL375: _temp374=(( struct Cyc_Absyn_Evar_struct*)
_temp314)->f4; goto _LL321;} else{ goto _LL322;} _LL322: if(( unsigned int)
_temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_VarType: 0){ _LL386: _temp385=((
struct Cyc_Absyn_VarType_struct*) _temp314)->f1; goto _LL323;} else{ goto _LL324;}
_LL324: if(( unsigned int) _temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_TunionType:
0){ _LL388: _temp387=(( struct Cyc_Absyn_TunionType_struct*) _temp314)->f1;
_LL394: _temp393=( void*) _temp387.tunion_info; if(*(( int*) _temp393) == Cyc_Absyn_KnownTunion){
_LL396: _temp395=(( struct Cyc_Absyn_KnownTunion_struct*) _temp393)->f1; goto
_LL392;} else{ goto _LL326;} _LL392: _temp391= _temp387.targs; goto _LL390;
_LL390: _temp389=( void*) _temp387.rgn; goto _LL325;} else{ goto _LL326;} _LL326:
if(( unsigned int) _temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_TunionType: 0){
_LL398: _temp397=(( struct Cyc_Absyn_TunionType_struct*) _temp314)->f1; goto
_LL327;} else{ goto _LL328;} _LL328: if(( unsigned int) _temp314 > 4u?*(( int*)
_temp314) == Cyc_Absyn_TunionFieldType: 0){ _LL400: _temp399=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp314)->f1; _LL404: _temp403=( void*) _temp399.field_info; if(*(( int*)
_temp403) == Cyc_Absyn_KnownTunionfield){ _LL408: _temp407=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp403)->f1; goto _LL406; _LL406: _temp405=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp403)->f2; goto _LL402;} else{ goto _LL330;} _LL402: _temp401= _temp399.targs;
goto _LL329;} else{ goto _LL330;} _LL330: if(( unsigned int) _temp314 > 4u?*((
int*) _temp314) == Cyc_Absyn_TunionFieldType: 0){ _LL410: _temp409=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp314)->f1; goto _LL331;} else{ goto _LL332;} _LL332: if(( unsigned int)
_temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_PointerType: 0){ _LL412: _temp411=((
struct Cyc_Absyn_PointerType_struct*) _temp314)->f1; _LL422: _temp421=( void*)
_temp411.elt_typ; goto _LL420; _LL420: _temp419=( void*) _temp411.rgn_typ; goto
_LL418; _LL418: _temp417= _temp411.nullable; goto _LL416; _LL416: _temp415=
_temp411.tq; goto _LL414; _LL414: _temp413= _temp411.bounds; goto _LL333;} else{
goto _LL334;} _LL334: if(( unsigned int) _temp314 > 4u?*(( int*) _temp314) ==
Cyc_Absyn_IntType: 0){ _LL426: _temp425=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp314)->f1; goto _LL424; _LL424: _temp423=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp314)->f2; goto _LL335;} else{ goto _LL336;} _LL336: if( _temp314 ==( void*)
Cyc_Absyn_FloatType){ goto _LL337;} else{ goto _LL338;} _LL338: if( _temp314 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL339;} else{ goto _LL340;} _LL340: if((
unsigned int) _temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_ArrayType: 0){
_LL432: _temp431=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp314)->f1;
goto _LL430; _LL430: _temp429=(( struct Cyc_Absyn_ArrayType_struct*) _temp314)->f2;
goto _LL428; _LL428: _temp427=(( struct Cyc_Absyn_ArrayType_struct*) _temp314)->f3;
goto _LL341;} else{ goto _LL342;} _LL342: if(( unsigned int) _temp314 > 4u?*((
int*) _temp314) == Cyc_Absyn_FnType: 0){ _LL434: _temp433=(( struct Cyc_Absyn_FnType_struct*)
_temp314)->f1; _LL448: _temp447= _temp433.tvars; goto _LL446; _LL446: _temp445=
_temp433.effect; goto _LL444; _LL444: _temp443=( void*) _temp433.ret_typ; goto
_LL442; _LL442: _temp441= _temp433.args; goto _LL440; _LL440: _temp439= _temp433.c_varargs;
goto _LL438; _LL438: _temp437= _temp433.cyc_varargs; goto _LL436; _LL436:
_temp435= _temp433.attributes; goto _LL343;} else{ goto _LL344;} _LL344: if((
unsigned int) _temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_TupleType: 0){
_LL450: _temp449=(( struct Cyc_Absyn_TupleType_struct*) _temp314)->f1; goto
_LL345;} else{ goto _LL346;} _LL346: if(( unsigned int) _temp314 > 4u?*(( int*)
_temp314) == Cyc_Absyn_AnonStructType: 0){ _LL452: _temp451=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp314)->f1; goto _LL347;} else{ goto _LL348;} _LL348: if(( unsigned int)
_temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_AnonUnionType: 0){ _LL454:
_temp453=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp314)->f1; goto _LL349;}
else{ goto _LL350;} _LL350: if(( unsigned int) _temp314 > 4u?*(( int*) _temp314)
== Cyc_Absyn_UnionType: 0){ _LL460: _temp459=(( struct Cyc_Absyn_UnionType_struct*)
_temp314)->f1; goto _LL458; _LL458: _temp457=(( struct Cyc_Absyn_UnionType_struct*)
_temp314)->f2; goto _LL456; _LL456: _temp455=(( struct Cyc_Absyn_UnionType_struct*)
_temp314)->f3; goto _LL351;} else{ goto _LL352;} _LL352: if(( unsigned int)
_temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_StructType: 0){ _LL466: _temp465=((
struct Cyc_Absyn_StructType_struct*) _temp314)->f1; goto _LL464; _LL464:
_temp463=(( struct Cyc_Absyn_StructType_struct*) _temp314)->f2; goto _LL462;
_LL462: _temp461=(( struct Cyc_Absyn_StructType_struct*) _temp314)->f3; goto
_LL353;} else{ goto _LL354;} _LL354: if(( unsigned int) _temp314 > 4u?*(( int*)
_temp314) == Cyc_Absyn_EnumType: 0){ _LL470: _temp469=(( struct Cyc_Absyn_EnumType_struct*)
_temp314)->f1; goto _LL468; _LL468: _temp467=(( struct Cyc_Absyn_EnumType_struct*)
_temp314)->f2; goto _LL355;} else{ goto _LL356;} _LL356: if(( unsigned int)
_temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_TypedefType: 0){ _LL476: _temp475=((
struct Cyc_Absyn_TypedefType_struct*) _temp314)->f1; goto _LL474; _LL474:
_temp473=(( struct Cyc_Absyn_TypedefType_struct*) _temp314)->f2; goto _LL472;
_LL472: _temp471=(( struct Cyc_Absyn_TypedefType_struct*) _temp314)->f3; goto
_LL357;} else{ goto _LL358;} _LL358: if(( unsigned int) _temp314 > 4u?*(( int*)
_temp314) == Cyc_Absyn_RgnHandleType: 0){ _LL478: _temp477=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp314)->f1; goto _LL359;} else{ goto _LL360;} _LL360: if( _temp314 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL361;} else{ goto _LL362;} _LL362: if(( unsigned int)
_temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_AccessEff: 0){ _LL480: _temp479=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp314)->f1; goto _LL363;} else{
goto _LL364;} _LL364: if(( unsigned int) _temp314 > 4u?*(( int*) _temp314) ==
Cyc_Absyn_JoinEff: 0){ _LL482: _temp481=(( struct Cyc_Absyn_JoinEff_struct*)
_temp314)->f1; goto _LL365;} else{ goto _LL315;} _LL317: return t; _LL319:
return( void*) Cyc_Absyn_VoidType; _LL321: return Cyc_Toc_typ_to_c( _temp381);
_LL323: return Cyc_Absyn_void_star_typ(); _LL325: return Cyc_Absyn_void_star_typ();
_LL327:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp483=( unsigned char*)"unresolved TunionType"; struct
_tagged_string _temp484; _temp484.curr= _temp483; _temp484.base= _temp483;
_temp484.last_plus_one= _temp483 + 22; _temp484;}))); _LL329: if( _temp405->typs
== 0){ if( _temp407->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp405->name,({
unsigned char* _temp485=( unsigned char*)"_struct"; struct _tagged_string
_temp486; _temp486.curr= _temp485; _temp486.base= _temp485; _temp486.last_plus_one=
_temp485 + 8; _temp486;})));} _LL331:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp487=( unsigned char*)"unresolved TunionFieldType";
struct _tagged_string _temp488; _temp488.curr= _temp487; _temp488.base= _temp487;
_temp488.last_plus_one= _temp487 + 27; _temp488;}))); _LL333: _temp421= Cyc_Toc_typ_to_c_array(
_temp421);{ void* _temp489=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp413))->v; void* _temp495; _LL491: if((
unsigned int) _temp489 > 1u?*(( int*) _temp489) == Cyc_Absyn_Eq_constr: 0){
_LL496: _temp495=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp489)->f1;
if( _temp495 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL492;} else{ goto _LL493;}}
else{ goto _LL493;} _LL493: goto _LL494; _LL492: return Cyc_Toc_add_tagged_ptr_type(
_temp421, _temp415); _LL494: return Cyc_Absyn_star_typ( _temp421,( void*) Cyc_Absyn_HeapRgn,
_temp415); _LL490:;} _LL335: return t; _LL337: return t; _LL339: return t;
_LL341: return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp497=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp497[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp498; _temp498.tag= Cyc_Absyn_ArrayType;
_temp498.f1=( void*) Cyc_Toc_typ_to_c_array( _temp431); _temp498.f2= _temp429;
_temp498.f3= _temp427; _temp498;}); _temp497;}); _LL343: { struct Cyc_List_List*
_temp499= _temp435; goto _LL500; _LL500: { struct Cyc_List_List* _temp501= 0;
goto _LL502; _LL502: for( 0; _temp435 != 0; _temp435=(( struct Cyc_List_List*)
_check_null( _temp435))->tl){ void* _temp503=( void*)(( struct Cyc_List_List*)
_check_null( _temp435))->hd; _LL505: if( _temp503 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL506;} else{ goto _LL507;} _LL507: if( _temp503 ==( void*) Cyc_Absyn_Const_att){
goto _LL508;} else{ goto _LL509;} _LL509: goto _LL510; _LL506: goto _LL508;
_LL508: continue; _LL510: _temp501=({ struct Cyc_List_List* _temp511=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp511->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( _temp435))->hd); _temp511->tl=
_temp501; _temp511;}); goto _LL504; _LL504:;}{ struct Cyc_List_List* _temp512=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp441); goto _LL513; _LL513: if(
_temp437 != 0){ void* _temp514= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp437))->type,( void*)(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp437))->rgn, Cyc_Toc_mt_tq)); goto
_LL515; _LL515: { struct _tuple1* _temp517=({ struct _tuple1* _temp516=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp516->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp437))->name; _temp516->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp437))->tq; _temp516->f3= _temp514; _temp516;}); goto _LL518;
_LL518: _temp512=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_imp_append)( _temp512,({ struct Cyc_List_List*
_temp519=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp519->hd=( void*) _temp517; _temp519->tl= 0; _temp519;}));}} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp520=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp520[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp521; _temp521.tag= Cyc_Absyn_FnType; _temp521.f1=({ struct Cyc_Absyn_FnInfo
_temp522; _temp522.tvars= 0; _temp522.effect= 0; _temp522.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp443); _temp522.args= _temp512; _temp522.c_varargs= _temp439; _temp522.cyc_varargs=
0; _temp522.attributes= _temp501; _temp522;}); _temp521;}); _temp520;});}}}
_LL345: _temp449=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp449);{
struct _tagged_string* _temp523= Cyc_Toc_add_tuple_type( _temp449); goto _LL524;
_LL524: return Cyc_Absyn_strct( _temp523);} _LL347: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp525=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp525[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp526; _temp526.tag= Cyc_Absyn_AnonStructType; _temp526.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp451); _temp526;}); _temp525;});
_LL349: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp527=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp527[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp528; _temp528.tag=
Cyc_Absyn_AnonUnionType; _temp528.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp453); _temp528;}); _temp527;}); _LL351: if( _temp459 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp529=(
unsigned char*)"anonymous union"; struct _tagged_string _temp530; _temp530.curr=
_temp529; _temp530.base= _temp529; _temp530.last_plus_one= _temp529 + 16;
_temp530;}));} return Cyc_Absyn_unionq_typ(( struct _tuple0*) _check_null(
_temp459)); _LL353: if( _temp465 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp531=( unsigned char*)"anonymous struct"; struct
_tagged_string _temp532; _temp532.curr= _temp531; _temp532.base= _temp531;
_temp532.last_plus_one= _temp531 + 17; _temp532;}));} return Cyc_Absyn_strctq((
struct _tuple0*) _check_null( _temp465)); _LL355: return t; _LL357: if( _temp471
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp533=( unsigned char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp534; _temp534.curr= _temp533; _temp534.base= _temp533;
_temp534.last_plus_one= _temp533 + 38; _temp534;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp535=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp535[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp536; _temp536.tag= Cyc_Absyn_TypedefType;
_temp536.f1= _temp475; _temp536.f2= 0; _temp536.f3=({ struct Cyc_Core_Opt*
_temp537=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp537->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp471))->v); _temp537;}); _temp536;}); _temp535;}); _LL359:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL361: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp538=( unsigned char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp539; _temp539.curr= _temp538; _temp539.base= _temp538;
_temp539.last_plus_one= _temp538 + 55; _temp539;})); _LL363: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp540=(
unsigned char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 54; _temp541;})); _LL365: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp542=(
unsigned char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp543; _temp543.curr= _temp542; _temp543.base= _temp542;
_temp543.last_plus_one= _temp542 + 52; _temp543;})); _LL315:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp544= t; struct Cyc_Absyn_Exp* _temp550; struct Cyc_Absyn_Tqual
_temp552; void* _temp554; _LL546: if(( unsigned int) _temp544 > 4u?*(( int*)
_temp544) == Cyc_Absyn_ArrayType: 0){ _LL555: _temp554=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp544)->f1; goto _LL553; _LL553: _temp552=(( struct Cyc_Absyn_ArrayType_struct*)
_temp544)->f2; goto _LL551; _LL551: _temp550=(( struct Cyc_Absyn_ArrayType_struct*)
_temp544)->f3; goto _LL547;} else{ goto _LL548;} _LL548: goto _LL549; _LL547:
return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ( _temp554,( void*) Cyc_Absyn_HeapRgn,
_temp552), e, l); _LL549: return Cyc_Absyn_cast_exp( t, e, l); _LL545:;} static
int Cyc_Toc_atomic_typ( void* t){ void* _temp556= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp592; void* _temp594; void* _temp596; struct Cyc_Absyn_Exp*
_temp598; struct Cyc_Absyn_Tqual _temp600; void* _temp602; struct Cyc_Absyn_FnInfo
_temp604; struct Cyc_List_List* _temp606; struct Cyc_Absyn_Structdecl** _temp608;
struct Cyc_List_List* _temp610; struct _tuple0* _temp612; struct Cyc_Absyn_TunionInfo
_temp614; struct Cyc_List_List* _temp616; struct Cyc_List_List* _temp618; struct
Cyc_Absyn_TunionFieldInfo _temp620; struct Cyc_List_List* _temp622; void*
_temp624; struct Cyc_Absyn_Tunionfield* _temp626; struct Cyc_Absyn_Tuniondecl*
_temp628; struct Cyc_Absyn_Uniondecl** _temp630; struct Cyc_List_List* _temp632;
struct _tuple0* _temp634; struct Cyc_Absyn_PtrInfo _temp636; void* _temp638;
_LL558: if( _temp556 ==( void*) Cyc_Absyn_VoidType){ goto _LL559;} else{ goto
_LL560;} _LL560: if(( unsigned int) _temp556 > 4u?*(( int*) _temp556) == Cyc_Absyn_VarType:
0){ _LL593: _temp592=(( struct Cyc_Absyn_VarType_struct*) _temp556)->f1; goto
_LL561;} else{ goto _LL562;} _LL562: if(( unsigned int) _temp556 > 4u?*(( int*)
_temp556) == Cyc_Absyn_IntType: 0){ _LL597: _temp596=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp556)->f1; goto _LL595; _LL595: _temp594=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp556)->f2; goto _LL563;} else{ goto _LL564;} _LL564: if( _temp556 ==( void*)
Cyc_Absyn_FloatType){ goto _LL565;} else{ goto _LL566;} _LL566: if( _temp556 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL567;} else{ goto _LL568;} _LL568: if((
unsigned int) _temp556 > 4u?*(( int*) _temp556) == Cyc_Absyn_ArrayType: 0){
_LL603: _temp602=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp556)->f1;
goto _LL601; _LL601: _temp600=(( struct Cyc_Absyn_ArrayType_struct*) _temp556)->f2;
goto _LL599; _LL599: _temp598=(( struct Cyc_Absyn_ArrayType_struct*) _temp556)->f3;
goto _LL569;} else{ goto _LL570;} _LL570: if(( unsigned int) _temp556 > 4u?*((
int*) _temp556) == Cyc_Absyn_FnType: 0){ _LL605: _temp604=(( struct Cyc_Absyn_FnType_struct*)
_temp556)->f1; goto _LL571;} else{ goto _LL572;} _LL572: if(( unsigned int)
_temp556 > 4u?*(( int*) _temp556) == Cyc_Absyn_TupleType: 0){ _LL607: _temp606=((
struct Cyc_Absyn_TupleType_struct*) _temp556)->f1; goto _LL573;} else{ goto
_LL574;} _LL574: if(( unsigned int) _temp556 > 4u?*(( int*) _temp556) == Cyc_Absyn_StructType:
0){ _LL613: _temp612=(( struct Cyc_Absyn_StructType_struct*) _temp556)->f1; goto
_LL611; _LL611: _temp610=(( struct Cyc_Absyn_StructType_struct*) _temp556)->f2;
goto _LL609; _LL609: _temp608=(( struct Cyc_Absyn_StructType_struct*) _temp556)->f3;
goto _LL575;} else{ goto _LL576;} _LL576: if(( unsigned int) _temp556 > 4u?*((
int*) _temp556) == Cyc_Absyn_TunionType: 0){ _LL615: _temp614=(( struct Cyc_Absyn_TunionType_struct*)
_temp556)->f1; goto _LL577;} else{ goto _LL578;} _LL578: if(( unsigned int)
_temp556 > 4u?*(( int*) _temp556) == Cyc_Absyn_AnonStructType: 0){ _LL617:
_temp616=(( struct Cyc_Absyn_AnonStructType_struct*) _temp556)->f1; goto _LL579;}
else{ goto _LL580;} _LL580: if(( unsigned int) _temp556 > 4u?*(( int*) _temp556)
== Cyc_Absyn_AnonUnionType: 0){ _LL619: _temp618=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp556)->f1; goto _LL581;} else{ goto _LL582;} _LL582: if(( unsigned int)
_temp556 > 4u?*(( int*) _temp556) == Cyc_Absyn_TunionFieldType: 0){ _LL621:
_temp620=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp556)->f1; _LL625:
_temp624=( void*) _temp620.field_info; if(*(( int*) _temp624) == Cyc_Absyn_KnownTunionfield){
_LL629: _temp628=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp624)->f1;
goto _LL627; _LL627: _temp626=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp624)->f2; goto _LL623;} else{ goto _LL584;} _LL623: _temp622= _temp620.targs;
goto _LL583;} else{ goto _LL584;} _LL584: if(( unsigned int) _temp556 > 4u?*((
int*) _temp556) == Cyc_Absyn_UnionType: 0){ _LL635: _temp634=(( struct Cyc_Absyn_UnionType_struct*)
_temp556)->f1; goto _LL633; _LL633: _temp632=(( struct Cyc_Absyn_UnionType_struct*)
_temp556)->f2; goto _LL631; _LL631: _temp630=(( struct Cyc_Absyn_UnionType_struct*)
_temp556)->f3; goto _LL585;} else{ goto _LL586;} _LL586: if(( unsigned int)
_temp556 > 4u?*(( int*) _temp556) == Cyc_Absyn_PointerType: 0){ _LL637: _temp636=((
struct Cyc_Absyn_PointerType_struct*) _temp556)->f1; goto _LL587;} else{ goto
_LL588;} _LL588: if(( unsigned int) _temp556 > 4u?*(( int*) _temp556) == Cyc_Absyn_RgnHandleType:
0){ _LL639: _temp638=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp556)->f1;
goto _LL589;} else{ goto _LL590;} _LL590: goto _LL591; _LL559: return 1; _LL561:
return 0; _LL563: return 1; _LL565: return 1; _LL567: return 0; _LL569: return
Cyc_Toc_atomic_typ( _temp602); _LL571: return 1; _LL573: for( 0; _temp606 != 0;
_temp606=(( struct Cyc_List_List*) _check_null( _temp606))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp606))->hd)).f2)){
return 0;}} return 1; _LL575: return 0; _LL577: return 0; _LL579: _temp618=
_temp616; goto _LL581; _LL581: for( 0; _temp618 != 0; _temp618=(( struct Cyc_List_List*)
_check_null( _temp618))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp618))->hd)->type)){ return 0;}} return
1; _LL583: return _temp626->typs == 0; _LL585: return 0; _LL587: return 0;
_LL589: return 0; _LL591:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp640= Cyc_Absynpp_typ2string(
t); xprintf("atomic_typ:  bad type %.*s", _temp640.last_plus_one - _temp640.curr,
_temp640.curr);})); return 0; _LL557:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp641= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp647;
struct Cyc_Absyn_Conref* _temp649; struct Cyc_Absyn_Tqual _temp651; struct Cyc_Absyn_Conref*
_temp653; void* _temp655; void* _temp657; _LL643: if(( unsigned int) _temp641 >
4u?*(( int*) _temp641) == Cyc_Absyn_PointerType: 0){ _LL648: _temp647=(( struct
Cyc_Absyn_PointerType_struct*) _temp641)->f1; _LL658: _temp657=( void*) _temp647.elt_typ;
goto _LL656; _LL656: _temp655=( void*) _temp647.rgn_typ; goto _LL654; _LL654:
_temp653= _temp647.nullable; goto _LL652; _LL652: _temp651= _temp647.tq; goto
_LL650; _LL650: _temp649= _temp647.bounds; goto _LL644;} else{ goto _LL645;}
_LL645: goto _LL646; _LL644: { void* _temp659= Cyc_Tcutil_compress( _temp657);
_LL661: if( _temp659 ==( void*) Cyc_Absyn_VoidType){ goto _LL662;} else{ goto
_LL663;} _LL663: goto _LL664; _LL662: return 1; _LL664: return 0; _LL660:;}
_LL646: return 0; _LL642:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_string* f){ void* _temp665= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp677; struct Cyc_Absyn_Structdecl* _temp679; struct Cyc_List_List* _temp680;
struct _tuple0* _temp682; struct Cyc_List_List* _temp684; struct Cyc_List_List*
_temp686; struct Cyc_Absyn_Uniondecl** _temp688; struct Cyc_List_List* _temp690;
struct _tuple0* _temp692; _LL667: if(( unsigned int) _temp665 > 4u?*(( int*)
_temp665) == Cyc_Absyn_StructType: 0){ _LL683: _temp682=(( struct Cyc_Absyn_StructType_struct*)
_temp665)->f1; goto _LL681; _LL681: _temp680=(( struct Cyc_Absyn_StructType_struct*)
_temp665)->f2; goto _LL678; _LL678: _temp677=(( struct Cyc_Absyn_StructType_struct*)
_temp665)->f3; if( _temp677 == 0){ goto _LL669;} else{ _temp679=* _temp677; goto
_LL668;}} else{ goto _LL669;} _LL669: if(( unsigned int) _temp665 > 4u?*(( int*)
_temp665) == Cyc_Absyn_AnonStructType: 0){ _LL685: _temp684=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp665)->f1; goto _LL670;} else{ goto _LL671;} _LL671: if(( unsigned int)
_temp665 > 4u?*(( int*) _temp665) == Cyc_Absyn_AnonUnionType: 0){ _LL687:
_temp686=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp665)->f1; goto _LL672;}
else{ goto _LL673;} _LL673: if(( unsigned int) _temp665 > 4u?*(( int*) _temp665)
== Cyc_Absyn_UnionType: 0){ _LL693: _temp692=(( struct Cyc_Absyn_UnionType_struct*)
_temp665)->f1; goto _LL691; _LL691: _temp690=(( struct Cyc_Absyn_UnionType_struct*)
_temp665)->f2; goto _LL689; _LL689: _temp688=(( struct Cyc_Absyn_UnionType_struct*)
_temp665)->f3; goto _LL674;} else{ goto _LL675;} _LL675: goto _LL676; _LL668:
if( _temp679->fields == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp694=( unsigned char*)"is_poly_field: struct missing fields";
struct _tagged_string _temp695; _temp695.curr= _temp694; _temp695.base= _temp694;
_temp695.last_plus_one= _temp694 + 37; _temp695;})));} _temp684=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp679->fields))->v; goto _LL670; _LL670: {
struct Cyc_Absyn_Structfield* _temp696= Cyc_Absyn_lookup_field( _temp684, f);
goto _LL697; _LL697: if( _temp696 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct _tagged_string _temp698=*((
struct Cyc_Absyn_Structfield*) _check_null( _temp696))->name; xprintf("is_poly_field: bad field %.*s",
_temp698.last_plus_one - _temp698.curr, _temp698.curr);}));} return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp696))->type);} _LL672:
return 0; _LL674: return 0; _LL676:( void) _throw((( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp699= Cyc_Absynpp_typ2string( t); xprintf("is_poly_field: bad type %.*s",
_temp699.last_plus_one - _temp699.curr, _temp699.curr);}))); _LL666:;} static
int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp700=( void*) e->r;
struct _tagged_string* _temp708; struct Cyc_Absyn_Exp* _temp710; struct
_tagged_string* _temp712; struct Cyc_Absyn_Exp* _temp714; _LL702: if(*(( int*)
_temp700) == Cyc_Absyn_StructMember_e){ _LL711: _temp710=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp700)->f1; goto _LL709; _LL709: _temp708=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp700)->f2; goto _LL703;} else{ goto _LL704;} _LL704: if(*(( int*) _temp700)
== Cyc_Absyn_StructArrow_e){ _LL715: _temp714=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp700)->f1; goto _LL713; _LL713: _temp712=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp700)->f2; goto _LL705;} else{ goto _LL706;} _LL706: goto _LL707; _LL703:
return Cyc_Toc_is_poly_field(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp710->topt))->v, _temp708); _LL705: { void* _temp716= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp714->topt))->v); struct Cyc_Absyn_PtrInfo
_temp722; struct Cyc_Absyn_Conref* _temp724; struct Cyc_Absyn_Tqual _temp726;
struct Cyc_Absyn_Conref* _temp728; void* _temp730; void* _temp732; _LL718: if((
unsigned int) _temp716 > 4u?*(( int*) _temp716) == Cyc_Absyn_PointerType: 0){
_LL723: _temp722=(( struct Cyc_Absyn_PointerType_struct*) _temp716)->f1; _LL733:
_temp732=( void*) _temp722.elt_typ; goto _LL731; _LL731: _temp730=( void*)
_temp722.rgn_typ; goto _LL729; _LL729: _temp728= _temp722.nullable; goto _LL727;
_LL727: _temp726= _temp722.tq; goto _LL725; _LL725: _temp724= _temp722.bounds;
goto _LL719;} else{ goto _LL720;} _LL720: goto _LL721; _LL719: return Cyc_Toc_is_poly_field(
_temp732, _temp712); _LL721:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp734= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp714->topt))->v); xprintf("is_poly_project:  bad type %.*s",
_temp734.last_plus_one - _temp734.curr, _temp734.curr);})); return 0; _LL717:;}
_LL707: return 0; _LL701:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_qv,
0),({ struct Cyc_List_List* _temp735=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp735->hd=( void*) s; _temp735->tl= 0; _temp735;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_atomic_qv, 0),({
struct Cyc_List_List* _temp736=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp736->hd=( void*) s; _temp736->tl= 0; _temp736;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__region_malloc_qv,
0),({ struct Cyc_List_List* _temp737=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp737->hd=( void*) rgn; _temp737->tl=({ struct Cyc_List_List*
_temp738=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp738->hd=( void*) s; _temp738->tl= 0; _temp738;}); _temp737;}), 0);} static
unsigned char _temp741[ 3u]="f0"; static struct _tagged_string Cyc_Toc_f0={
_temp741, _temp741, _temp741 + 3u}; static struct _tagged_string* Cyc_Toc_field_names_v[
1u]={& Cyc_Toc_f0}; struct _tagged_ptr2{ struct _tagged_string** curr; struct
_tagged_string** base; struct _tagged_string** last_plus_one; } ; static struct
_tagged_ptr2 Cyc_Toc_field_names={( struct _tagged_string**) Cyc_Toc_field_names_v,(
struct _tagged_string**) Cyc_Toc_field_names_v,( struct _tagged_string**) Cyc_Toc_field_names_v
+ 1u}; static struct _tagged_string* Cyc_Toc_fieldname( int i){ unsigned int fsz=({
struct _tagged_ptr2 _temp749= Cyc_Toc_field_names;( unsigned int)( _temp749.last_plus_one
- _temp749.curr);}); if( i >= fsz){ Cyc_Toc_field_names=({ unsigned int _temp744=(
unsigned int)( i + 1); struct _tagged_string** _temp745=( struct _tagged_string**)
GC_malloc( sizeof( struct _tagged_string*) * _temp744); struct _tagged_ptr2
_temp748={ _temp745, _temp745, _temp745 + _temp744};{ unsigned int _temp746=
_temp744; unsigned int j; for( j= 0; j < _temp746; j ++){ _temp745[ j]= j < fsz?*((
struct _tagged_string**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Toc_field_names, sizeof( struct _tagged_string*),(
int) j):({ struct _tagged_string* _temp747=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string)); _temp747[ 0]=( struct _tagged_string) xprintf("f%d",(
int) j); _temp747;});}}; _temp748;});} return*(( struct _tagged_string**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Toc_field_names,
sizeof( struct _tagged_string*), i);} static const int Cyc_Toc_NullCheck= 0;
struct Cyc_Toc_NullCheck_struct{ int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3; } ; static const int Cyc_Toc_TagPtr= 2; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Toc_NoConv=
0; struct _tuple7{ void* f1; void* f2; } ; static void* Cyc_Toc_conversion( void*
t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp751=({ struct _tuple7 _temp750; _temp750.f1= t1; _temp750.f2=
t2; _temp750;}); void* _temp757; struct Cyc_Absyn_PtrInfo _temp759; void*
_temp761; struct Cyc_Absyn_PtrInfo _temp763; _LL753: _LL762: _temp761= _temp751.f1;
if(( unsigned int) _temp761 > 4u?*(( int*) _temp761) == Cyc_Absyn_PointerType: 0){
_LL764: _temp763=(( struct Cyc_Absyn_PointerType_struct*) _temp761)->f1; goto
_LL758;} else{ goto _LL755;} _LL758: _temp757= _temp751.f2; if(( unsigned int)
_temp757 > 4u?*(( int*) _temp757) == Cyc_Absyn_PointerType: 0){ _LL760: _temp759=((
struct Cyc_Absyn_PointerType_struct*) _temp757)->f1; goto _LL754;} else{ goto
_LL755;} _LL755: goto _LL756; _LL754: { int _temp765=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp763.nullable); goto _LL766; _LL766: { int
_temp767=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp759.nullable);
goto _LL768; _LL768: { void* _temp769=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp763.bounds); goto _LL770; _LL770: { void* _temp771=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp759.bounds);
goto _LL772; _LL772: { struct _tuple7 _temp774=({ struct _tuple7 _temp773;
_temp773.f1= _temp769; _temp773.f2= _temp771; _temp773;}); void* _temp784;
struct Cyc_Absyn_Exp* _temp786; void* _temp788; struct Cyc_Absyn_Exp* _temp790;
void* _temp792; void* _temp794; void* _temp796; void* _temp798; struct Cyc_Absyn_Exp*
_temp800; void* _temp802; struct Cyc_Absyn_Exp* _temp804; void* _temp806; _LL776:
_LL789: _temp788= _temp774.f1; if(( unsigned int) _temp788 > 1u?*(( int*)
_temp788) == Cyc_Absyn_Upper_b: 0){ _LL791: _temp790=(( struct Cyc_Absyn_Upper_b_struct*)
_temp788)->f1; goto _LL785;} else{ goto _LL778;} _LL785: _temp784= _temp774.f2;
if(( unsigned int) _temp784 > 1u?*(( int*) _temp784) == Cyc_Absyn_Upper_b: 0){
_LL787: _temp786=(( struct Cyc_Absyn_Upper_b_struct*) _temp784)->f1; goto _LL777;}
else{ goto _LL778;} _LL778: _LL795: _temp794= _temp774.f1; if( _temp794 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL793;} else{ goto _LL780;} _LL793: _temp792=
_temp774.f2; if( _temp792 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL779;} else{
goto _LL780;} _LL780: _LL799: _temp798= _temp774.f1; if(( unsigned int) _temp798
> 1u?*(( int*) _temp798) == Cyc_Absyn_Upper_b: 0){ _LL801: _temp800=(( struct
Cyc_Absyn_Upper_b_struct*) _temp798)->f1; goto _LL797;} else{ goto _LL782;}
_LL797: _temp796= _temp774.f2; if( _temp796 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL781;} else{ goto _LL782;} _LL782: _LL807: _temp806= _temp774.f1; if(
_temp806 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL803;} else{ goto _LL775;}
_LL803: _temp802= _temp774.f2; if(( unsigned int) _temp802 > 1u?*(( int*)
_temp802) == Cyc_Absyn_Upper_b: 0){ _LL805: _temp804=(( struct Cyc_Absyn_Upper_b_struct*)
_temp802)->f1; goto _LL783;} else{ goto _LL775;} _LL777: if( _temp765? !
_temp767: 0){ return( void*)({ struct Cyc_Toc_NullCheck_struct* _temp808=(
struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp808[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp809; _temp809.tag= Cyc_Toc_NullCheck;
_temp809.f1=( void*) t2; _temp809;}); _temp808;});} return( void*) Cyc_Toc_NoConv;
_LL779: return( void*) Cyc_Toc_NoConv; _LL781: return( void*)({ struct Cyc_Toc_TagPtr_struct*
_temp810=( struct Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp810[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp811; _temp811.tag= Cyc_Toc_TagPtr;
_temp811.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp811.f2= _temp800; _temp811;});
_temp810;}); _LL783: return( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp812=(
struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp812[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp813; _temp813.tag= Cyc_Toc_UntagPtr;
_temp813.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp813.f2= _temp804; _temp813.f3=
_temp767; _temp813;}); _temp812;}); _LL775:;}}}}} _LL756: return( void*) Cyc_Toc_NoConv;
_LL752:;}} static unsigned char _temp816[ 14u]="sfile_to_file"; static struct
_tagged_string Cyc_Toc__sfile_to_file_string={ _temp816, _temp816, _temp816 + 14u};
static struct _tuple0 Cyc_Toc__sfile_to_file_var={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__sfile_to_file_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv(
struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__sfile_to_file_var,
0),({ struct Cyc_List_List* _temp817=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp817->hd=( void*) e; _temp817->tl= 0; _temp817;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__throw_qv, 0),({ struct
Cyc_List_List* _temp818=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp818->hd=( void*) e; _temp818->tl= 0; _temp818;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_null_check_conv( int toplevel, void* t, struct Cyc_Absyn_Exp* e){ void*
new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ unsigned char* _temp819=( unsigned char*)"can't do null-check conversion at top-level";
struct _tagged_string _temp820; _temp820.curr= _temp819; _temp820.base= _temp819;
_temp820.last_plus_one= _temp819 + 44; _temp820;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp821=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp821->hd=( void*) e; _temp821->tl= 0; _temp821;}), 0),
0);} else{ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0),
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0),
Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
x, 0), 0), 0); return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, new_typ,(
struct Cyc_Absyn_Exp*) e, s, 0), 0);}} struct _tagged_ptr3{ struct _tuple4**
curr; struct _tuple4** base; struct _tuple4** last_plus_one; } ; static struct
Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct Cyc_Absyn_Exp*
sz, struct Cyc_Absyn_Exp* e){ struct _tuple0* a= Cyc_Toc_temp_var(); struct
_tuple0* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ; struct Cyc_Absyn_Tqual elt_tqual;{ void*
_temp822= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp828; struct Cyc_Absyn_Conref*
_temp830; struct Cyc_Absyn_Tqual _temp832; struct Cyc_Absyn_Conref* _temp834;
void* _temp836; void* _temp838; _LL824: if(( unsigned int) _temp822 > 4u?*(( int*)
_temp822) == Cyc_Absyn_PointerType: 0){ _LL829: _temp828=(( struct Cyc_Absyn_PointerType_struct*)
_temp822)->f1; _LL839: _temp838=( void*) _temp828.elt_typ; goto _LL837; _LL837:
_temp836=( void*) _temp828.rgn_typ; goto _LL835; _LL835: _temp834= _temp828.nullable;
goto _LL833; _LL833: _temp832= _temp828.tq; goto _LL831; _LL831: _temp830=
_temp828.bounds; goto _LL825;} else{ goto _LL826;} _LL826: goto _LL827; _LL825:
elt_typ= _temp838; elt_tqual= _temp832; strt_typ= Cyc_Toc_add_tagged_ptr_type(
_temp838, _temp832); goto _LL823; _LL827: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp840=(
unsigned char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp841;
_temp841.curr= _temp840; _temp841.base= _temp840; _temp841.last_plus_one=
_temp840 + 28; _temp841;})); _LL823:;} if( toplevel){ int is_string= 0;{ void*
_temp842=( void*) e->r; void* _temp848; struct _tagged_string _temp850; _LL844:
if(*(( int*) _temp842) == Cyc_Absyn_Const_e){ _LL849: _temp848=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp842)->f1; if(( unsigned int) _temp848 > 1u?*((
int*) _temp848) == Cyc_Absyn_String_c: 0){ _LL851: _temp850=(( struct Cyc_Absyn_String_c_struct*)
_temp848)->f1; goto _LL845;} else{ goto _LL846;}} else{ goto _LL846;} _LL846:
goto _LL847; _LL845: is_string= 1; goto _LL843; _LL847: goto _LL843; _LL843:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp855=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp855[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp856; _temp856.tag= Cyc_Absyn_ArrayType; _temp856.f1=(
void*) elt_typ; _temp856.f2= Cyc_Toc_mt_tq; _temp856.f3=( struct Cyc_Absyn_Exp*)
sz; _temp856;}); _temp855;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp852=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp852->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp853=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp853[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp854; _temp854.tag= Cyc_Absyn_Var_d;
_temp854.f1= vd; _temp854;}); _temp853;}), 0); _temp852->tl= Cyc_Toc_result_decls;
_temp852;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ if( elt_tqual.q_const){ xexp=
Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq), e, 0);} else{
xexp= e;}}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp( xexp, sz, 0);
struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(*
_temp857)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)( struct _tagged_ptr3))
Cyc_List_list; struct _tuple4* _temp860=({ struct _tuple4* _temp865=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp865->f1= 0; _temp865->f2=
xexp; _temp865;}); struct _tuple4* _temp861=({ struct _tuple4* _temp864=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp864->f1= 0; _temp864->f2=
xexp; _temp864;}); struct _tuple4* _temp862=({ struct _tuple4* _temp863=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp863->f1= 0; _temp863->f2=
xplussz; _temp863;}); struct _tuple4* _temp858[ 3u]={ _temp860, _temp861,
_temp862}; struct _tagged_ptr3 _temp859={ _temp858, _temp858, _temp858 + 3u};
_temp857( _temp859);}), 0); return urm_exp;}}} else{ struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( b, 0), Cyc_Toc_last_plus_one_sp,
0), Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp(
szvalue, 0), 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( b, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_var_exp( a, 0), 0), s,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( b, 0), Cyc_Toc_curr_sp, 0), Cyc_Absyn_var_exp( a, 0), 0), s,
0); s= Cyc_Absyn_declare_stmt( b, strt_typ, 0, s, 0);{ void* atyp= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); if( elt_tqual.q_const){ e= Cyc_Absyn_cast_exp( atyp, e,
0);} s= Cyc_Absyn_declare_stmt( a, Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*) e, s, 0); return Cyc_Absyn_stmt_exp( s, 0);}}} static
struct Cyc_Absyn_Exp* Cyc_Toc_untag_ptr_conv( int toplevel, void* t1, struct Cyc_Absyn_Exp*
sz, int nullable, struct Cyc_Absyn_Exp* e){ if( toplevel){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp866=( unsigned char*)"can't coerce t? to t* or t@ at the top-level";
struct _tagged_string _temp867; _temp867.curr= _temp866; _temp867.base= _temp866;
_temp867.last_plus_one= _temp866 + 45; _temp867;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
int szvalue=( int) Cyc_Evexp_eval_const_uint_exp( sz); struct Cyc_Absyn_Exp*
xcurr= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xcurr, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_add_exp( xcurr, Cyc_Absyn_signed_int_exp( szvalue, 0), 0); struct
Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_gt_exp( e1, e2, 0); struct Cyc_Absyn_Exp*
e4= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( xcurr, Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), e3, 0); struct Cyc_Absyn_Exp* e5= ! nullable? e4:
Cyc_Absyn_and_exp( Cyc_Absyn_neq_exp( xcurr, Cyc_Absyn_uint_exp( 0, 0), 0), e4,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_ifthenelse_stmt( e5, Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt(
0), 0), s, 0); s= Cyc_Absyn_declare_stmt( x, t1,( struct Cyc_Absyn_Exp*) e, s, 0);
return Cyc_Absyn_stmt_exp( s, 0);}} struct Cyc_Toc_Env{ struct Cyc_Core_Opt*
break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt* fallthru_info;
struct Cyc_Dict_Dict* varmap; int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){
return({ struct Cyc_Toc_Env* _temp868=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp868->break_lab= 0; _temp868->continue_lab= 0;
_temp868->fallthru_info= 0; _temp868->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp868->toplevel= 1; _temp868;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp869=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp869->break_lab= e->break_lab;
_temp869->continue_lab= e->continue_lab; _temp869->fallthru_info= e->fallthru_info;
_temp869->varmap= e->varmap; _temp869->toplevel= e->toplevel; _temp869;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp870=(* x).f1; struct Cyc_List_List*
_temp876; _LL872: if(( unsigned int) _temp870 > 1u?*(( int*) _temp870) == Cyc_Absyn_Rel_n:
0){ _LL877: _temp876=(( struct Cyc_Absyn_Rel_n_struct*) _temp870)->f1; goto
_LL873;} else{ goto _LL874;} _LL874: goto _LL875; _LL873:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp878=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp878[ 0]=({ struct
Cyc_Core_Impossible_struct _temp879; _temp879.tag= Cyc_Core_Impossible; _temp879.f1=(
struct _tagged_string)({ struct _tagged_string _temp880= Cyc_Absynpp_qvar2string(
x); xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp880.last_plus_one -
_temp880.curr, _temp880.curr);}); _temp879;}); _temp878;})); _LL875: goto _LL871;
_LL871:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp*
data)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env*
Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab= 0; ans->continue_lab= 0; return ans;} struct _tuple8{ struct
_tagged_string* f1; struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ;
static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_string* break_l, struct _tagged_string* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
_temp881= 0; goto _LL882; _LL882: for( 0; fallthru_binders != 0;
fallthru_binders=(( struct Cyc_List_List*) _check_null( fallthru_binders))->tl){
_temp881=({ struct Cyc_List_List* _temp883=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp883->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp883->tl=
_temp881; _temp883;});} _temp881=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp881);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp884=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp884->v=( void*) break_l; _temp884;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp885=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp885->v=( void*)({ struct _tuple8*
_temp886=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp886->f1=
fallthru_l; _temp886->f2= _temp881; _temp886->f3= next_case_env->varmap;
_temp886;}); _temp885;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp887=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp887->v=( void*)
break_l; _temp887;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_string*
next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp888=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp888->v=( void*)({ struct _tuple8* _temp889=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp889->f1= next_l; _temp889->f2= 0;
_temp889->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp889;}); _temp888;});
return ans;} struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tagged_ptr4{ struct Cyc_List_List** curr; struct Cyc_List_List** base;
struct Cyc_List_List** last_plus_one; } ; struct _tuple10{ struct _tuple0* f1;
void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp890=( unsigned char*)"expecting a literal format string";
struct _tagged_string _temp891; _temp891.curr= _temp890; _temp891.base= _temp890;
_temp891.last_plus_one= _temp890 + 34; _temp891;}));}{ struct _tagged_string
fmt_str;{ void* _temp892=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd)->r; void* _temp898; struct _tagged_string _temp900;
_LL894: if(*(( int*) _temp892) == Cyc_Absyn_Const_e){ _LL899: _temp898=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp892)->f1; if(( unsigned int) _temp898 > 1u?*((
int*) _temp898) == Cyc_Absyn_String_c: 0){ _LL901: _temp900=(( struct Cyc_Absyn_String_c_struct*)
_temp898)->f1; goto _LL895;} else{ goto _LL896;}} else{ goto _LL896;} _LL896:
goto _LL897; _LL895: fmt_str= _temp900; goto _LL893; _LL897: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp902=(
unsigned char*)"expecting a literal format string"; struct _tagged_string
_temp903; _temp903.curr= _temp902; _temp903.base= _temp902; _temp903.last_plus_one=
_temp902 + 34; _temp903;})); _LL893:;}{ int len=( int)({ struct _tagged_string
_temp999= fmt_str;( unsigned int)( _temp999.last_plus_one - _temp999.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str,
sizeof( unsigned char), i) =='%'){ i ++; if( i < len?*(( const unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( unsigned char), i) =='s': 0){ break;}}} if( i == len){ if( aopt
!= 0){ args=({ struct Cyc_List_List* _temp904=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp904->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp904->tl= args; _temp904;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp905= _new_region(); struct
_RegionHandle* r=& _temp905; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str,
sizeof( unsigned char), i); rev_fmt=({ struct Cyc_List_List* _temp906=( struct
Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List)); _temp906->hd=(
void*)(( int) c); _temp906->tl= rev_fmt; _temp906;}); if( c !='%'){ continue;}{
struct Cyc_Core_Opt* _temp907= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); goto _LL908; _LL908: if( _temp907 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp909=( unsigned char*)"bad format string";
struct _tagged_string _temp910; _temp910.curr= _temp909; _temp910.base= _temp909;
_temp910.last_plus_one= _temp909 + 18; _temp910;}));}{ struct _tuple9 _temp913;
int _temp914; unsigned char _temp916; struct Cyc_List_List* _temp918; struct Cyc_List_List*
_temp920; struct Cyc_List_List* _temp922; struct Cyc_List_List* _temp924; struct
_tuple9* _temp911=( struct _tuple9*)(( struct Cyc_Core_Opt*) _check_null(
_temp907))->v; _temp913=* _temp911; _LL925: _temp924= _temp913.f1; goto _LL923;
_LL923: _temp922= _temp913.f2; goto _LL921; _LL921: _temp920= _temp913.f3; goto
_LL919; _LL919: _temp918= _temp913.f4; goto _LL917; _LL917: _temp916= _temp913.f5;
goto _LL915; _LL915: _temp914= _temp913.f6; goto _LL912; _LL912: i= _temp914 - 1;
if( _temp916 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp926)( struct _RegionHandle*, struct
_tagged_ptr4)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_ptr4)) Cyc_List_rlist; struct _RegionHandle* _temp933= r; struct Cyc_List_List*
_temp929= _temp924; struct Cyc_List_List* _temp930= _temp922; struct Cyc_List_List*
_temp931= _temp920; struct Cyc_List_List* _temp932= _temp918; struct Cyc_List_List*
_temp927[ 4u]={ _temp929, _temp930, _temp931, _temp932}; struct _tagged_ptr4
_temp928={ _temp927, _temp927, _temp927 + 4u}; _temp926( _temp933, _temp928);})),
rev_fmt); rev_fmt=({ struct Cyc_List_List* _temp934=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp934->hd=( void*)(( int)
_temp916); _temp934->tl= rev_fmt; _temp934;});{ struct Cyc_List_List* _temp935=
_temp922; struct Cyc_List_List _temp942; struct Cyc_List_List* _temp943; int
_temp945; _LL937: if( _temp935 == 0){ goto _LL939;} else{ _temp942=* _temp935;
_LL946: _temp945=( int) _temp942.hd; goto _LL944; _LL944: _temp943= _temp942.tl;
if( _temp943 == 0){ goto _LL941;} else{ goto _LL939;}} _LL941: if( _temp945 =='*'){
goto _LL938;} else{ goto _LL939;} _LL939: goto _LL940; _LL938: { struct _tuple0*
_temp947= Cyc_Toc_temp_var(); goto _LL948; _LL948: rev_temps=({ struct Cyc_List_List*
_temp949=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp949->hd=( void*)({ struct _tuple10* _temp950=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp950->f1= _temp947; _temp950->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp950->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp950;});
_temp949->tl= rev_temps; _temp949;}); rev_result=({ struct Cyc_List_List*
_temp951=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp951->hd=( void*) Cyc_Absyn_var_exp( _temp947, 0); _temp951->tl= rev_result;
_temp951;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL936;} _LL940: goto
_LL936; _LL936:;}{ struct Cyc_List_List* _temp952= _temp920; struct Cyc_List_List
_temp959; struct Cyc_List_List* _temp960; struct Cyc_List_List _temp962; struct
Cyc_List_List* _temp963; int _temp965; int _temp967; _LL954: if( _temp952 == 0){
goto _LL956;} else{ _temp959=* _temp952; _LL968: _temp967=( int) _temp959.hd;
goto _LL961; _LL961: _temp960= _temp959.tl; if( _temp960 == 0){ goto _LL956;}
else{ _temp962=* _temp960; _LL966: _temp965=( int) _temp962.hd; goto _LL964;
_LL964: _temp963= _temp962.tl; if( _temp963 == 0){ goto _LL958;} else{ goto
_LL956;}}} _LL958: if( _temp967 =='.'? _temp965 =='*': 0){ goto _LL955;} else{
goto _LL956;} _LL956: goto _LL957; _LL955: { struct _tuple0* _temp969= Cyc_Toc_temp_var();
goto _LL970; _LL970: rev_temps=({ struct Cyc_List_List* _temp971=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp971->hd=( void*)({
struct _tuple10* _temp972=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp972->f1= _temp969; _temp972->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp972->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp972;}); _temp971->tl= rev_temps; _temp971;});
rev_result=({ struct Cyc_List_List* _temp973=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp973->hd=( void*) Cyc_Absyn_var_exp(
_temp969, 0); _temp973->tl= rev_result; _temp973;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL953;} _LL957: goto _LL953; _LL953:;} if( _temp916 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ struct _tagged_string _temp974= Cyc_Absynpp_prim2string( p);
xprintf("missing arg to %.*s", _temp974.last_plus_one - _temp974.curr, _temp974.curr);}));}{
struct _tuple0* _temp975= Cyc_Toc_temp_var(); goto _LL976; _LL976: rev_temps=({
struct Cyc_List_List* _temp977=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp977->hd=( void*)({ struct _tuple10*
_temp978=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp978->f1= _temp975; _temp978->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp978->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp978;}); _temp977->tl= rev_temps; _temp977;});
rev_result=({ struct Cyc_List_List* _temp979=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp979->hd=( void*) Cyc_Absyn_var_exp(
_temp975, 0); _temp979->tl= rev_result; _temp979;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp980=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp980->hd=( void*)(( int)'.');
_temp980->tl= rev_fmt; _temp980;}); rev_fmt=({ struct Cyc_List_List* _temp981=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp981->hd=( void*)(( int)'*'); _temp981->tl= rev_fmt; _temp981;}); rev_fmt=({
struct Cyc_List_List* _temp982=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp982->hd=( void*)(( int)'s'); _temp982->tl=
rev_fmt; _temp982;});{ struct _tuple0* _temp983= Cyc_Toc_temp_var(); goto _LL984;
_LL984: rev_temps=({ struct Cyc_List_List* _temp985=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp985->hd=( void*)({
struct _tuple10* _temp986=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp986->f1= _temp983; _temp986->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp986->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp986;}); _temp985->tl= rev_temps; _temp985;});{
struct Cyc_Absyn_Exp* _temp987= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp983, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp983, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL988; _LL988: {
struct Cyc_Absyn_Exp* _temp989= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp983, 0), Cyc_Toc_curr_sp, 0); goto _LL990; _LL990: rev_result=({ struct Cyc_List_List*
_temp991=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp991->hd=( void*) _temp987; _temp991->tl= rev_result; _temp991;});
rev_result=({ struct Cyc_List_List* _temp992=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp992->hd=( void*)
_temp989; _temp992->tl= rev_result; _temp992;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}}{
struct Cyc_List_List* _temp994=({ struct Cyc_List_List* _temp993=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp993->hd=( void*) Cyc_Absyn_string_exp((
struct _tagged_string) Cyc_String_implode((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0); _temp993->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_result); _temp993;}); goto _LL995;
_LL995: if( aopt != 0){ _temp994=({ struct Cyc_List_List* _temp996=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp996->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp996->tl= _temp994; _temp996;});}{
struct Cyc_Absyn_Exp* _temp997= Cyc_Absyn_primop_exp( p, _temp994, 0); goto
_LL998; _LL998: s= Cyc_Absyn_exp_stmt( _temp997, 0); for( 0; rev_temps != 0;
rev_temps=(( struct Cyc_List_List*) _check_null( rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f1,(*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f2,(
struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f3, s, 0);}}}}; _pop_region(& _temp905);} return
Cyc_Absyn_stmt_exp( s, 0);}}}} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1000=( unsigned char*)"Missing type in primop "; struct
_tagged_string _temp1001; _temp1001.curr= _temp1000; _temp1001.base= _temp1000;
_temp1001.last_plus_one= _temp1000 + 24; _temp1001;}));} return Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1002=( unsigned char*)"Missing type in primop ";
struct _tagged_string _temp1003; _temp1003.curr= _temp1002; _temp1003.base=
_temp1002; _temp1003.last_plus_one= _temp1002 + 24; _temp1003;}));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1004=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1004->f1= Cyc_Toc_mt_tq; _temp1004->f2=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp1004;});} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp1005=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1005[
0]=({ struct _tuple4 _temp1006; _temp1006.f1= 0; _temp1006.f2= e; _temp1006;});
_temp1005;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
_temp1007= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); goto _LL1008; _LL1008: if(
rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic(
_temp1007): Cyc_Toc_malloc_ptr( _temp1007), 0);} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c( nv, r); eo=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp( r, _temp1007),
0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp1009=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL1010; _LL1010: for( 0;
_temp1009 != 0; _temp1009=(( struct Cyc_List_List*) _check_null( _temp1009))->tl){
struct _tuple4 _temp1013; struct Cyc_Absyn_Exp* _temp1014; struct Cyc_List_List*
_temp1016; struct _tuple4* _temp1011=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1009))->hd; _temp1013=* _temp1011; _LL1017: _temp1016=
_temp1013.f1; goto _LL1015; _LL1015: _temp1014= _temp1013.f2; goto _LL1012;
_LL1012: { struct Cyc_Absyn_Exp* e_index; if( _temp1016 == 0){ e_index= Cyc_Absyn_signed_int_exp(
count --, 0);} else{ if((( struct Cyc_List_List*) _check_null( _temp1016))->tl
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp1018=( unsigned char*)"multiple designators in array"; struct
_tagged_string _temp1019; _temp1019.curr= _temp1018; _temp1019.base= _temp1018;
_temp1019.last_plus_one= _temp1018 + 30; _temp1019;}));}{ void* _temp1020=( void*)((
struct Cyc_List_List*) _check_null( _temp1016))->hd; goto _LL1021; _LL1021: {
void* _temp1022= _temp1020; struct Cyc_Absyn_Exp* _temp1028; struct
_tagged_string* _temp1030; _LL1024: if(*(( int*) _temp1022) == Cyc_Absyn_ArrayElement){
_LL1029: _temp1028=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1022)->f1;
goto _LL1025;} else{ goto _LL1026;} _LL1026: if(*(( int*) _temp1022) == Cyc_Absyn_FieldName){
_LL1031: _temp1030=(( struct Cyc_Absyn_FieldName_struct*) _temp1022)->f1; goto
_LL1027;} else{ goto _LL1023;} _LL1025: Cyc_Toc_exp_to_c( nv, _temp1028);
e_index= _temp1028; goto _LL1023; _LL1027: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp1032=(
unsigned char*)"field name designators in array"; struct _tagged_string
_temp1033; _temp1033.curr= _temp1032; _temp1033.base= _temp1032; _temp1033.last_plus_one=
_temp1032 + 32; _temp1033;})); goto _LL1023; _LL1023:;}}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp1034=( void*)
_temp1014->r; struct Cyc_List_List* _temp1044; struct Cyc_Absyn_Exp* _temp1046;
struct Cyc_Absyn_Exp* _temp1048; struct Cyc_Absyn_Vardecl* _temp1050; struct Cyc_List_List*
_temp1052; void* _temp1054; _LL1036: if(*(( int*) _temp1034) == Cyc_Absyn_Array_e){
_LL1045: _temp1044=(( struct Cyc_Absyn_Array_e_struct*) _temp1034)->f1; goto
_LL1037;} else{ goto _LL1038;} _LL1038: if(*(( int*) _temp1034) == Cyc_Absyn_Comprehension_e){
_LL1051: _temp1050=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1034)->f1;
goto _LL1049; _LL1049: _temp1048=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1034)->f2; goto _LL1047; _LL1047: _temp1046=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1034)->f3; goto _LL1039;} else{ goto _LL1040;} _LL1040: if(*(( int*)
_temp1034) == Cyc_Absyn_AnonStruct_e){ _LL1055: _temp1054=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1034)->f1; goto _LL1053; _LL1053: _temp1052=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1034)->f2; goto _LL1041;} else{ goto _LL1042;} _LL1042: goto _LL1043;
_LL1037: s= Cyc_Toc_init_array( nv, lval, _temp1044, s); goto _LL1035; _LL1039:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1050, _temp1048, _temp1046, s, 0);
goto _LL1035; _LL1041: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1054,
_temp1052, s); goto _LL1035; _LL1043: Cyc_Toc_exp_to_c( nv, _temp1014); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1014, 0),
s, 0); goto _LL1035; _LL1035:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* _temp1056= vd->name; goto _LL1057;
_LL1057: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1056, Cyc_Absyn_varb_exp( _temp1056,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1080=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1080[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1081; _temp1081.tag= Cyc_Absyn_Local_b; _temp1081.f1=
vd; _temp1081;}); _temp1080;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp1056, 0),
Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( _temp1056, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp*
ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( _temp1056, 0), 0); struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( _temp1056, 0), 0); struct
Cyc_Absyn_Stmt* body;{ void* _temp1058=( void*) e2->r; struct Cyc_List_List*
_temp1068; struct Cyc_Absyn_Exp* _temp1070; struct Cyc_Absyn_Exp* _temp1072;
struct Cyc_Absyn_Vardecl* _temp1074; struct Cyc_List_List* _temp1076; void*
_temp1078; _LL1060: if(*(( int*) _temp1058) == Cyc_Absyn_Array_e){ _LL1069:
_temp1068=(( struct Cyc_Absyn_Array_e_struct*) _temp1058)->f1; goto _LL1061;}
else{ goto _LL1062;} _LL1062: if(*(( int*) _temp1058) == Cyc_Absyn_Comprehension_e){
_LL1075: _temp1074=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1058)->f1;
goto _LL1073; _LL1073: _temp1072=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1058)->f2; goto _LL1071; _LL1071: _temp1070=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1058)->f3; goto _LL1063;} else{ goto _LL1064;} _LL1064: if(*(( int*)
_temp1058) == Cyc_Absyn_AnonStruct_e){ _LL1079: _temp1078=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1058)->f1; goto _LL1077; _LL1077: _temp1076=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1058)->f2; goto _LL1065;} else{ goto _LL1066;} _LL1066: goto _LL1067;
_LL1061: body= Cyc_Toc_init_array( nv2, lval, _temp1068, Cyc_Absyn_skip_stmt( 0));
goto _LL1059; _LL1063: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1074,
_temp1072, _temp1070, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1059; _LL1065: body=
Cyc_Toc_init_anon_struct( nv, lval, _temp1078, _temp1076, Cyc_Absyn_skip_stmt( 0));
goto _LL1059; _LL1067: Cyc_Toc_exp_to_c( nv2, e2); body= Cyc_Absyn_assign_stmt(
lval, e2, 0); goto _LL1059; _LL1059:;}{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt(
ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( _temp1056, Cyc_Absyn_uint_t,
0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s){{ struct Cyc_List_List* _temp1082=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto _LL1083; _LL1083: for( 0;
_temp1082 != 0; _temp1082=(( struct Cyc_List_List*) _check_null( _temp1082))->tl){
struct _tuple4 _temp1086; struct Cyc_Absyn_Exp* _temp1087; struct Cyc_List_List*
_temp1089; struct _tuple4* _temp1084=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1082))->hd; _temp1086=* _temp1084; _LL1090: _temp1089=
_temp1086.f1; goto _LL1088; _LL1088: _temp1087= _temp1086.f2; goto _LL1085;
_LL1085: if( _temp1089 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1091=( unsigned char*)"empty designator list"; struct
_tagged_string _temp1092; _temp1092.curr= _temp1091; _temp1092.base= _temp1091;
_temp1092.last_plus_one= _temp1091 + 22; _temp1092;}));} if((( struct Cyc_List_List*)
_check_null( _temp1089))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1093=( unsigned char*)"too many designators in anonymous struct";
struct _tagged_string _temp1094; _temp1094.curr= _temp1093; _temp1094.base=
_temp1093; _temp1094.last_plus_one= _temp1093 + 41; _temp1094;}));}{ void*
_temp1095=( void*)(( struct Cyc_List_List*) _check_null( _temp1089))->hd; struct
_tagged_string* _temp1101; _LL1097: if(*(( int*) _temp1095) == Cyc_Absyn_FieldName){
_LL1102: _temp1101=(( struct Cyc_Absyn_FieldName_struct*) _temp1095)->f1; goto
_LL1098;} else{ goto _LL1099;} _LL1099: goto _LL1100; _LL1098: { struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_structmember_exp( lhs, _temp1101, 0);{ void* _temp1103=( void*)
_temp1087->r; struct Cyc_List_List* _temp1113; struct Cyc_Absyn_Exp* _temp1115;
struct Cyc_Absyn_Exp* _temp1117; struct Cyc_Absyn_Vardecl* _temp1119; struct Cyc_List_List*
_temp1121; void* _temp1123; _LL1105: if(*(( int*) _temp1103) == Cyc_Absyn_Array_e){
_LL1114: _temp1113=(( struct Cyc_Absyn_Array_e_struct*) _temp1103)->f1; goto
_LL1106;} else{ goto _LL1107;} _LL1107: if(*(( int*) _temp1103) == Cyc_Absyn_Comprehension_e){
_LL1120: _temp1119=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1103)->f1;
goto _LL1118; _LL1118: _temp1117=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1103)->f2; goto _LL1116; _LL1116: _temp1115=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1103)->f3; goto _LL1108;} else{ goto _LL1109;} _LL1109: if(*(( int*)
_temp1103) == Cyc_Absyn_AnonStruct_e){ _LL1124: _temp1123=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1103)->f1; goto _LL1122; _LL1122: _temp1121=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1103)->f2; goto _LL1110;} else{ goto _LL1111;} _LL1111: goto _LL1112;
_LL1106: s= Cyc_Toc_init_array( nv, lval, _temp1113, s); goto _LL1104; _LL1108:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1119, _temp1117, _temp1115, s, 0);
goto _LL1104; _LL1110: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1123,
_temp1121, s); goto _LL1104; _LL1112: Cyc_Toc_exp_to_c( nv, _temp1087); if( Cyc_Toc_is_poly_field(
struct_type, _temp1101)){ _temp1087= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1087, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1087, 0), 0), s, 0); goto _LL1104; _LL1104:;} goto _LL1096;} _LL1100:(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1125=( unsigned char*)"array designator in struct"; struct
_tagged_string _temp1126; _temp1126.curr= _temp1125; _temp1126.base= _temp1125;
_temp1126.last_plus_one= _temp1125 + 27; _temp1126;}))); _LL1096:;}}} return s;}
static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int
pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
_RegionHandle _temp1127= _new_region(); struct _RegionHandle* r=& _temp1127;
_push_region( r);{ struct Cyc_List_List* _temp1128=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c, es); goto _LL1129; _LL1129: { struct
_tagged_string* _temp1130= Cyc_Toc_add_tuple_type( _temp1128); goto _LL1131;
_LL1131: { struct _tuple0* _temp1132= Cyc_Toc_temp_var(); goto _LL1133; _LL1133: {
struct Cyc_Absyn_Exp* _temp1134= Cyc_Absyn_var_exp( _temp1132, 0); goto _LL1135;
_LL1135: { struct Cyc_Absyn_Stmt* _temp1136= Cyc_Absyn_exp_stmt( _temp1134, 0);
goto _LL1137; _LL1137: { struct Cyc_Absyn_Exp*(* _temp1138)( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; goto _LL1139; _LL1139: { int is_atomic= 1; struct
Cyc_List_List* _temp1140=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( r, es); goto _LL1141; _LL1141:{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1140); for( 0;
_temp1140 != 0;( _temp1140=(( struct Cyc_List_List*) _check_null( _temp1140))->tl,
-- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1140))->hd; struct Cyc_Absyn_Exp* lval= _temp1138( _temp1134,
Cyc_Toc_fieldname( i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v): 0;{ void* _temp1142=( void*) e->r;
struct Cyc_List_List* _temp1150; struct Cyc_Absyn_Exp* _temp1152; struct Cyc_Absyn_Exp*
_temp1154; struct Cyc_Absyn_Vardecl* _temp1156; _LL1144: if(*(( int*) _temp1142)
== Cyc_Absyn_Array_e){ _LL1151: _temp1150=(( struct Cyc_Absyn_Array_e_struct*)
_temp1142)->f1; goto _LL1145;} else{ goto _LL1146;} _LL1146: if(*(( int*)
_temp1142) == Cyc_Absyn_Comprehension_e){ _LL1157: _temp1156=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1142)->f1; goto _LL1155; _LL1155: _temp1154=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1142)->f2; goto _LL1153; _LL1153: _temp1152=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1142)->f3; goto _LL1147;} else{ goto _LL1148;} _LL1148: goto _LL1149;
_LL1145: _temp1136= Cyc_Toc_init_array( nv, lval, _temp1150, _temp1136); goto
_LL1143; _LL1147: _temp1136= Cyc_Toc_init_comprehension( nv, lval, _temp1156,
_temp1154, _temp1152, _temp1136, 0); goto _LL1143; _LL1149: Cyc_Toc_exp_to_c( nv,
e); _temp1136= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1138( _temp1134, Cyc_Toc_fieldname( i), 0), e, 0), 0), _temp1136, 0); goto
_LL1143; _LL1143:;}}}{ struct Cyc_Absyn_Exp* _temp1158= Cyc_Toc_make_struct( nv,
_temp1132, Cyc_Absyn_strct( _temp1130), _temp1136, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp1158;}}}}}}}}; _pop_region(& _temp1127);} static
struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env* nv, void*
struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp1159= Cyc_Toc_temp_var(); goto
_LL1160; _LL1160: { struct Cyc_Absyn_Exp* _temp1161= Cyc_Absyn_var_exp(
_temp1159, 0); goto _LL1162; _LL1162: { struct Cyc_Absyn_Stmt* _temp1163= Cyc_Absyn_exp_stmt(
_temp1161, 0); goto _LL1164; _LL1164: { struct Cyc_Absyn_Exp*(* _temp1165)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1166;
_LL1166: { int is_atomic= 1; struct _RegionHandle _temp1167= _new_region();
struct _RegionHandle* r=& _temp1167; _push_region( r);{ struct Cyc_List_List*
_temp1168=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, dles); goto _LL1169; _LL1169: for( 0; _temp1168 != 0;
_temp1168=(( struct Cyc_List_List*) _check_null( _temp1168))->tl){ struct
_tuple4 _temp1172; struct Cyc_Absyn_Exp* _temp1173; struct Cyc_List_List*
_temp1175; struct _tuple4* _temp1170=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1168))->hd; _temp1172=* _temp1170; _LL1176: _temp1175=
_temp1172.f1; goto _LL1174; _LL1174: _temp1173= _temp1172.f2; goto _LL1171;
_LL1171: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1173->topt))->v): 0; if( _temp1175 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1177=(
unsigned char*)"empty designator list"; struct _tagged_string _temp1178;
_temp1178.curr= _temp1177; _temp1178.base= _temp1177; _temp1178.last_plus_one=
_temp1177 + 22; _temp1178;}));} if((( struct Cyc_List_List*) _check_null(
_temp1175))->tl != 0){ struct _tuple0* _temp1179= Cyc_Toc_temp_var(); goto
_LL1180; _LL1180: { struct Cyc_Absyn_Exp* _temp1181= Cyc_Absyn_var_exp(
_temp1179, 0); goto _LL1182; _LL1182: for( 0; _temp1175 != 0; _temp1175=((
struct Cyc_List_List*) _check_null( _temp1175))->tl){ void* _temp1183=( void*)((
struct Cyc_List_List*) _check_null( _temp1175))->hd; struct _tagged_string*
_temp1189; _LL1185: if(*(( int*) _temp1183) == Cyc_Absyn_FieldName){ _LL1190:
_temp1189=(( struct Cyc_Absyn_FieldName_struct*) _temp1183)->f1; goto _LL1186;}
else{ goto _LL1187;} _LL1187: goto _LL1188; _LL1186: if( Cyc_Toc_is_poly_field(
struct_type, _temp1189)){ _temp1181= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1181, 0);} _temp1163= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1165( _temp1161, _temp1189, 0), _temp1181, 0), 0), _temp1163, 0); goto
_LL1184; _LL1188:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1191=( unsigned char*)"array designator in struct"; struct
_tagged_string _temp1192; _temp1192.curr= _temp1191; _temp1192.base= _temp1191;
_temp1192.last_plus_one= _temp1191 + 27; _temp1192;}))); _LL1184:;} Cyc_Toc_exp_to_c(
nv, _temp1173); _temp1163= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1181, _temp1173, 0), 0), _temp1163, 0);}} else{ void* _temp1193=( void*)((
struct Cyc_List_List*) _check_null( _temp1175))->hd; struct _tagged_string*
_temp1199; _LL1195: if(*(( int*) _temp1193) == Cyc_Absyn_FieldName){ _LL1200:
_temp1199=(( struct Cyc_Absyn_FieldName_struct*) _temp1193)->f1; goto _LL1196;}
else{ goto _LL1197;} _LL1197: goto _LL1198; _LL1196: { struct Cyc_Absyn_Exp*
lval= _temp1165( _temp1161, _temp1199, 0);{ void* _temp1201=( void*) _temp1173->r;
struct Cyc_List_List* _temp1211; struct Cyc_Absyn_Exp* _temp1213; struct Cyc_Absyn_Exp*
_temp1215; struct Cyc_Absyn_Vardecl* _temp1217; struct Cyc_List_List* _temp1219;
void* _temp1221; _LL1203: if(*(( int*) _temp1201) == Cyc_Absyn_Array_e){ _LL1212:
_temp1211=(( struct Cyc_Absyn_Array_e_struct*) _temp1201)->f1; goto _LL1204;}
else{ goto _LL1205;} _LL1205: if(*(( int*) _temp1201) == Cyc_Absyn_Comprehension_e){
_LL1218: _temp1217=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1201)->f1;
goto _LL1216; _LL1216: _temp1215=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1201)->f2; goto _LL1214; _LL1214: _temp1213=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1201)->f3; goto _LL1206;} else{ goto _LL1207;} _LL1207: if(*(( int*)
_temp1201) == Cyc_Absyn_AnonStruct_e){ _LL1222: _temp1221=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1201)->f1; goto _LL1220; _LL1220: _temp1219=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1201)->f2; goto _LL1208;} else{ goto _LL1209;} _LL1209: goto _LL1210;
_LL1204: _temp1163= Cyc_Toc_init_array( nv, lval, _temp1211, _temp1163); goto
_LL1202; _LL1206: _temp1163= Cyc_Toc_init_comprehension( nv, lval, _temp1217,
_temp1215, _temp1213, _temp1163, 0); goto _LL1202; _LL1208: _temp1163= Cyc_Toc_init_anon_struct(
nv, lval, _temp1221, _temp1219, _temp1163); goto _LL1202; _LL1210: Cyc_Toc_exp_to_c(
nv, _temp1173); if( Cyc_Toc_is_poly_field( struct_type, _temp1199)){ _temp1173=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1173, 0);} _temp1163= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1173, 0), 0), _temp1163, 0);
goto _LL1202; _LL1202:;} goto _LL1194;} _LL1198:( void) _throw((( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1223=(
unsigned char*)"array designator in struct"; struct _tagged_string _temp1224;
_temp1224.curr= _temp1223; _temp1224.base= _temp1223; _temp1224.last_plus_one=
_temp1223 + 27; _temp1224;}))); _LL1194:;}}}{ struct Cyc_Absyn_Exp* _temp1225=
Cyc_Toc_make_struct( nv, _temp1159, Cyc_Absyn_strctq( tdn), _temp1163, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp1225;}; _pop_region(&
_temp1167);}}}}} struct _tuple11{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp*
f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp*
el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2,
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp*
e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp1226=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp1226[ 0]=({
struct Cyc_Absyn_Increment_e_struct _temp1227; _temp1227.tag= Cyc_Absyn_Increment_e;
_temp1227.f1= e1; _temp1227.f2=( void*) incr; _temp1227;}); _temp1226;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp1228=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp1238; struct Cyc_Absyn_Exp*
_temp1240; void* _temp1242; struct _tagged_string* _temp1244; struct Cyc_Absyn_Exp*
_temp1246; _LL1230: if(*(( int*) _temp1228) == Cyc_Absyn_StmtExp_e){ _LL1239:
_temp1238=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1228)->f1; goto _LL1231;}
else{ goto _LL1232;} _LL1232: if(*(( int*) _temp1228) == Cyc_Absyn_Cast_e){
_LL1243: _temp1242=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1228)->f1;
goto _LL1241; _LL1241: _temp1240=(( struct Cyc_Absyn_Cast_e_struct*) _temp1228)->f2;
goto _LL1233;} else{ goto _LL1234;} _LL1234: if(*(( int*) _temp1228) == Cyc_Absyn_StructMember_e){
_LL1247: _temp1246=(( struct Cyc_Absyn_StructMember_e_struct*) _temp1228)->f1;
goto _LL1245; _LL1245: _temp1244=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1228)->f2; goto _LL1235;} else{ goto _LL1236;} _LL1236: goto _LL1237;
_LL1231:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1238, fs, f, f_env); goto _LL1229; _LL1233:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1240, fs, f, f_env); goto
_LL1229; _LL1235:( void*)( e1->r=( void*)(( void*) _temp1246->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1248=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1248->hd=( void*) _temp1244; _temp1248->tl= fs;
_temp1248;}), f, f_env); goto _LL1229; _LL1237: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
struct _tagged_string _temp1249= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1249.last_plus_one - _temp1249.curr, _temp1249.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1229;}
_LL1229:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1250=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1260; struct Cyc_Absyn_Stmt* _temp1262; struct Cyc_Absyn_Decl* _temp1264;
struct Cyc_Absyn_Stmt* _temp1266; struct Cyc_Absyn_Stmt* _temp1268; _LL1252: if((
unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Exp_s: 0){
_LL1261: _temp1260=(( struct Cyc_Absyn_Exp_s_struct*) _temp1250)->f1; goto
_LL1253;} else{ goto _LL1254;} _LL1254: if(( unsigned int) _temp1250 > 1u?*((
int*) _temp1250) == Cyc_Absyn_Decl_s: 0){ _LL1265: _temp1264=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1250)->f1; goto _LL1263; _LL1263: _temp1262=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1250)->f2; goto _LL1255;} else{ goto _LL1256;} _LL1256: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Seq_s: 0){ _LL1269: _temp1268=((
struct Cyc_Absyn_Seq_s_struct*) _temp1250)->f1; goto _LL1267; _LL1267: _temp1266=((
struct Cyc_Absyn_Seq_s_struct*) _temp1250)->f2; goto _LL1257;} else{ goto
_LL1258;} _LL1258: goto _LL1259; _LL1253:(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1260, fs, f, f_env); goto
_LL1251; _LL1255:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1262, fs, f, f_env); goto _LL1251; _LL1257:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1266, fs, f, f_env);
goto _LL1251; _LL1259:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp1270= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1270.last_plus_one - _temp1270.curr,
_temp1270.curr);})); goto _LL1251; _LL1251:;} static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s); static struct Cyc_List_List*
Cyc_Toc_rmap_2c( struct _RegionHandle* r2, void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List* _temp1271=( struct
Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1271->hd=(
void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1271->tl=
0; _temp1271;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp1272=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1272->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1272->tl= 0;
_temp1272;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x))
Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1273=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1273->f1= 0; _temp1273->f2= e;
_temp1273;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){
void* _temp1274= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp1280;
_LL1276: if(( unsigned int) _temp1274 > 4u?*(( int*) _temp1274) == Cyc_Absyn_PointerType:
0){ _LL1281: _temp1280=(( struct Cyc_Absyn_PointerType_struct*) _temp1274)->f1;
goto _LL1277;} else{ goto _LL1278;} _LL1278: goto _LL1279; _LL1277: return
_temp1280; _LL1279:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp1282=( unsigned char*)"get_ptr_typ:  not a pointer!"; struct
_tagged_string _temp1283; _temp1283.curr= _temp1282; _temp1283.base= _temp1282;
_temp1283.last_plus_one= _temp1282 + 29; _temp1283;}))); _LL1275:;} struct
_tagged_ptr5{ struct _tuple0** curr; struct _tuple0** base; struct _tuple0**
last_plus_one; } ; struct _tagged_ptr6{ struct _tuple1** curr; struct _tuple1**
base; struct _tuple1** last_plus_one; } ; struct _tagged_ptr7{ struct Cyc_Absyn_Exp**
curr; struct Cyc_Absyn_Exp** base; struct Cyc_Absyn_Exp** last_plus_one; } ;
struct _tuple12{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){
void* _temp1284=( void*) e->r; goto _LL1285; _LL1285: if( e->topt == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1286= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1286.last_plus_one - _temp1286.curr, _temp1286.curr);})); return;}{ void*
old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void*
_temp1287= _temp1284; void* _temp1365; void* _temp1367; void* _temp1369; struct
_tuple0* _temp1371; struct _tuple0* _temp1373; struct Cyc_List_List* _temp1375;
void* _temp1377; void* _temp1379; struct Cyc_Absyn_Exp* _temp1381; struct Cyc_Absyn_Exp*
_temp1383; struct Cyc_Core_Opt* _temp1385; struct Cyc_Absyn_Exp* _temp1387;
struct Cyc_Absyn_Exp* _temp1389; struct Cyc_Absyn_Exp* _temp1391; struct Cyc_Absyn_Exp*
_temp1393; struct Cyc_Absyn_Exp* _temp1395; struct Cyc_Absyn_Exp* _temp1397;
struct Cyc_List_List* _temp1399; struct Cyc_Absyn_Exp* _temp1401; struct Cyc_Absyn_VarargCallInfo*
_temp1403; struct Cyc_List_List* _temp1405; struct Cyc_Absyn_Exp* _temp1407;
struct Cyc_Absyn_VarargCallInfo* _temp1409; struct Cyc_Absyn_VarargCallInfo
_temp1411; struct Cyc_Absyn_VarargInfo* _temp1412; struct Cyc_List_List*
_temp1414; int _temp1416; struct Cyc_List_List* _temp1418; struct Cyc_Absyn_Exp*
_temp1420; struct Cyc_Absyn_Exp* _temp1422; struct Cyc_Absyn_Exp* _temp1424;
struct Cyc_List_List* _temp1426; struct Cyc_Absyn_Exp* _temp1428; struct Cyc_Absyn_Exp*
_temp1430; void* _temp1432; void** _temp1434; struct Cyc_Absyn_Exp* _temp1435;
struct Cyc_Absyn_Exp* _temp1437; struct Cyc_Absyn_Exp* _temp1439; void*
_temp1441; struct Cyc_Absyn_Exp* _temp1443; struct Cyc_Absyn_Exp* _temp1445;
struct _tagged_string* _temp1447; struct Cyc_Absyn_Exp* _temp1449; struct
_tagged_string* _temp1451; struct Cyc_Absyn_Exp* _temp1453; struct Cyc_Absyn_Exp*
_temp1455; struct Cyc_Absyn_Exp* _temp1457; struct Cyc_List_List* _temp1459;
struct Cyc_List_List* _temp1461; struct _tuple1* _temp1463; struct Cyc_List_List*
_temp1465; struct Cyc_Absyn_Exp* _temp1467; struct Cyc_Absyn_Exp* _temp1469;
struct Cyc_Absyn_Vardecl* _temp1471; struct Cyc_Absyn_Structdecl* _temp1473;
struct Cyc_List_List* _temp1475; struct Cyc_Core_Opt* _temp1477; struct _tuple0*
_temp1479; struct Cyc_List_List* _temp1481; void* _temp1483; struct Cyc_Absyn_Tunionfield*
_temp1485; struct Cyc_Absyn_Tuniondecl* _temp1487; struct Cyc_List_List*
_temp1489; struct Cyc_Core_Opt* _temp1491; struct Cyc_Core_Opt* _temp1493;
struct Cyc_Absyn_Tunionfield* _temp1495; struct Cyc_Absyn_Tuniondecl* _temp1497;
struct Cyc_List_List* _temp1499; struct Cyc_Core_Opt* _temp1501; struct Cyc_Core_Opt*
_temp1503; struct Cyc_Absyn_Enumfield* _temp1505; struct Cyc_Absyn_Enumdecl*
_temp1507; struct _tuple0* _temp1509; void* _temp1511; struct Cyc_Absyn_Exp*
_temp1513; struct Cyc_Absyn_Stmt* _temp1515; struct Cyc_List_List* _temp1517;
struct Cyc_Core_Opt* _temp1519; struct Cyc_Absyn_Fndecl* _temp1521; struct Cyc_Absyn_Exp*
_temp1523; _LL1289: if(*(( int*) _temp1287) == Cyc_Absyn_Const_e){ _LL1366:
_temp1365=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1287)->f1; if(
_temp1365 ==( void*) Cyc_Absyn_Null_c){ goto _LL1290;} else{ goto _LL1291;}}
else{ goto _LL1291;} _LL1291: if(*(( int*) _temp1287) == Cyc_Absyn_Const_e){
_LL1368: _temp1367=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1287)->f1;
goto _LL1292;} else{ goto _LL1293;} _LL1293: if(*(( int*) _temp1287) == Cyc_Absyn_Var_e){
_LL1372: _temp1371=(( struct Cyc_Absyn_Var_e_struct*) _temp1287)->f1; goto
_LL1370; _LL1370: _temp1369=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1287)->f2;
goto _LL1294;} else{ goto _LL1295;} _LL1295: if(*(( int*) _temp1287) == Cyc_Absyn_UnknownId_e){
_LL1374: _temp1373=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1287)->f1; goto
_LL1296;} else{ goto _LL1297;} _LL1297: if(*(( int*) _temp1287) == Cyc_Absyn_Primop_e){
_LL1378: _temp1377=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1287)->f1;
goto _LL1376; _LL1376: _temp1375=(( struct Cyc_Absyn_Primop_e_struct*) _temp1287)->f2;
goto _LL1298;} else{ goto _LL1299;} _LL1299: if(*(( int*) _temp1287) == Cyc_Absyn_Increment_e){
_LL1382: _temp1381=(( struct Cyc_Absyn_Increment_e_struct*) _temp1287)->f1; goto
_LL1380; _LL1380: _temp1379=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1287)->f2; goto _LL1300;} else{ goto _LL1301;} _LL1301: if(*(( int*)
_temp1287) == Cyc_Absyn_AssignOp_e){ _LL1388: _temp1387=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1287)->f1; goto _LL1386; _LL1386: _temp1385=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1287)->f2; goto _LL1384; _LL1384: _temp1383=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1287)->f3; goto _LL1302;} else{ goto _LL1303;} _LL1303: if(*(( int*)
_temp1287) == Cyc_Absyn_Conditional_e){ _LL1394: _temp1393=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1287)->f1; goto _LL1392; _LL1392: _temp1391=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1287)->f2; goto _LL1390; _LL1390: _temp1389=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1287)->f3; goto _LL1304;} else{ goto _LL1305;} _LL1305: if(*(( int*)
_temp1287) == Cyc_Absyn_SeqExp_e){ _LL1398: _temp1397=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1287)->f1; goto _LL1396; _LL1396: _temp1395=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1287)->f2; goto _LL1306;} else{ goto _LL1307;} _LL1307: if(*(( int*)
_temp1287) == Cyc_Absyn_UnknownCall_e){ _LL1402: _temp1401=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1287)->f1; goto _LL1400; _LL1400: _temp1399=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1287)->f2; goto _LL1308;} else{ goto _LL1309;} _LL1309: if(*(( int*)
_temp1287) == Cyc_Absyn_FnCall_e){ _LL1408: _temp1407=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1287)->f1; goto _LL1406; _LL1406: _temp1405=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1287)->f2; goto _LL1404; _LL1404: _temp1403=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1287)->f3; if( _temp1403 == 0){ goto _LL1310;} else{ goto _LL1311;}} else{
goto _LL1311;} _LL1311: if(*(( int*) _temp1287) == Cyc_Absyn_FnCall_e){ _LL1421:
_temp1420=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1287)->f1; goto _LL1419;
_LL1419: _temp1418=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1287)->f2; goto
_LL1410; _LL1410: _temp1409=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1287)->f3;
if( _temp1409 == 0){ goto _LL1313;} else{ _temp1411=* _temp1409; _LL1417:
_temp1416= _temp1411.num_varargs; goto _LL1415; _LL1415: _temp1414= _temp1411.injectors;
goto _LL1413; _LL1413: _temp1412= _temp1411.vai; goto _LL1312;}} else{ goto
_LL1313;} _LL1313: if(*(( int*) _temp1287) == Cyc_Absyn_Throw_e){ _LL1423:
_temp1422=(( struct Cyc_Absyn_Throw_e_struct*) _temp1287)->f1; goto _LL1314;}
else{ goto _LL1315;} _LL1315: if(*(( int*) _temp1287) == Cyc_Absyn_NoInstantiate_e){
_LL1425: _temp1424=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1287)->f1;
goto _LL1316;} else{ goto _LL1317;} _LL1317: if(*(( int*) _temp1287) == Cyc_Absyn_Instantiate_e){
_LL1429: _temp1428=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1287)->f1;
goto _LL1427; _LL1427: _temp1426=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1287)->f2; goto _LL1318;} else{ goto _LL1319;} _LL1319: if(*(( int*)
_temp1287) == Cyc_Absyn_Cast_e){ _LL1433: _temp1432=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1287)->f1; _temp1434=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1287)->f1); goto _LL1431; _LL1431: _temp1430=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1287)->f2; goto _LL1320;} else{ goto _LL1321;} _LL1321: if(*(( int*)
_temp1287) == Cyc_Absyn_Address_e){ _LL1436: _temp1435=(( struct Cyc_Absyn_Address_e_struct*)
_temp1287)->f1; goto _LL1322;} else{ goto _LL1323;} _LL1323: if(*(( int*)
_temp1287) == Cyc_Absyn_New_e){ _LL1440: _temp1439=(( struct Cyc_Absyn_New_e_struct*)
_temp1287)->f1; goto _LL1438; _LL1438: _temp1437=(( struct Cyc_Absyn_New_e_struct*)
_temp1287)->f2; goto _LL1324;} else{ goto _LL1325;} _LL1325: if(*(( int*)
_temp1287) == Cyc_Absyn_Sizeoftyp_e){ _LL1442: _temp1441=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1287)->f1; goto _LL1326;} else{ goto _LL1327;} _LL1327: if(*(( int*)
_temp1287) == Cyc_Absyn_Sizeofexp_e){ _LL1444: _temp1443=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1287)->f1; goto _LL1328;} else{ goto _LL1329;} _LL1329: if(*(( int*)
_temp1287) == Cyc_Absyn_Deref_e){ _LL1446: _temp1445=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1287)->f1; goto _LL1330;} else{ goto _LL1331;} _LL1331: if(*(( int*)
_temp1287) == Cyc_Absyn_StructMember_e){ _LL1450: _temp1449=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1287)->f1; goto _LL1448; _LL1448: _temp1447=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1287)->f2; goto _LL1332;} else{ goto _LL1333;} _LL1333: if(*(( int*)
_temp1287) == Cyc_Absyn_StructArrow_e){ _LL1454: _temp1453=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1287)->f1; goto _LL1452; _LL1452: _temp1451=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1287)->f2; goto _LL1334;} else{ goto _LL1335;} _LL1335: if(*(( int*)
_temp1287) == Cyc_Absyn_Subscript_e){ _LL1458: _temp1457=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1287)->f1; goto _LL1456; _LL1456: _temp1455=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1287)->f2; goto _LL1336;} else{ goto _LL1337;} _LL1337: if(*(( int*)
_temp1287) == Cyc_Absyn_Tuple_e){ _LL1460: _temp1459=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1287)->f1; goto _LL1338;} else{ goto _LL1339;} _LL1339: if(*(( int*)
_temp1287) == Cyc_Absyn_CompoundLit_e){ _LL1464: _temp1463=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1287)->f1; goto _LL1462; _LL1462: _temp1461=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1287)->f2; goto _LL1340;} else{ goto _LL1341;} _LL1341: if(*(( int*)
_temp1287) == Cyc_Absyn_Array_e){ _LL1466: _temp1465=(( struct Cyc_Absyn_Array_e_struct*)
_temp1287)->f1; goto _LL1342;} else{ goto _LL1343;} _LL1343: if(*(( int*)
_temp1287) == Cyc_Absyn_Comprehension_e){ _LL1472: _temp1471=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1287)->f1; goto _LL1470; _LL1470: _temp1469=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1287)->f2; goto _LL1468; _LL1468: _temp1467=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1287)->f3; goto _LL1344;} else{ goto _LL1345;} _LL1345: if(*(( int*)
_temp1287) == Cyc_Absyn_Struct_e){ _LL1480: _temp1479=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1287)->f1; goto _LL1478; _LL1478: _temp1477=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1287)->f2; goto _LL1476; _LL1476: _temp1475=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1287)->f3; goto _LL1474; _LL1474: _temp1473=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1287)->f4; goto _LL1346;} else{ goto _LL1347;} _LL1347: if(*(( int*)
_temp1287) == Cyc_Absyn_AnonStruct_e){ _LL1484: _temp1483=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1287)->f1; goto _LL1482; _LL1482: _temp1481=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1287)->f2; goto _LL1348;} else{ goto _LL1349;} _LL1349: if(*(( int*)
_temp1287) == Cyc_Absyn_Tunion_e){ _LL1494: _temp1493=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1287)->f1; goto _LL1492; _LL1492: _temp1491=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1287)->f2; goto _LL1490; _LL1490: _temp1489=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1287)->f3; if( _temp1489 == 0){ goto _LL1488;} else{ goto _LL1351;} _LL1488:
_temp1487=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1287)->f4; goto _LL1486;
_LL1486: _temp1485=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1287)->f5; goto
_LL1350;} else{ goto _LL1351;} _LL1351: if(*(( int*) _temp1287) == Cyc_Absyn_Tunion_e){
_LL1504: _temp1503=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1287)->f1; goto
_LL1502; _LL1502: _temp1501=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1287)->f2;
goto _LL1500; _LL1500: _temp1499=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1287)->f3;
goto _LL1498; _LL1498: _temp1497=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1287)->f4;
goto _LL1496; _LL1496: _temp1495=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1287)->f5;
goto _LL1352;} else{ goto _LL1353;} _LL1353: if(*(( int*) _temp1287) == Cyc_Absyn_Enum_e){
_LL1510: _temp1509=(( struct Cyc_Absyn_Enum_e_struct*) _temp1287)->f1; goto
_LL1508; _LL1508: _temp1507=(( struct Cyc_Absyn_Enum_e_struct*) _temp1287)->f2;
goto _LL1506; _LL1506: _temp1505=(( struct Cyc_Absyn_Enum_e_struct*) _temp1287)->f3;
goto _LL1354;} else{ goto _LL1355;} _LL1355: if(*(( int*) _temp1287) == Cyc_Absyn_Malloc_e){
_LL1514: _temp1513=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1287)->f1; goto
_LL1512; _LL1512: _temp1511=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1287)->f2; goto _LL1356;} else{ goto _LL1357;} _LL1357: if(*(( int*)
_temp1287) == Cyc_Absyn_StmtExp_e){ _LL1516: _temp1515=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1287)->f1; goto _LL1358;} else{ goto _LL1359;} _LL1359: if(*(( int*)
_temp1287) == Cyc_Absyn_UnresolvedMem_e){ _LL1520: _temp1519=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1287)->f1; goto _LL1518; _LL1518: _temp1517=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1287)->f2; goto _LL1360;} else{ goto _LL1361;} _LL1361: if(*(( int*)
_temp1287) == Cyc_Absyn_Codegen_e){ _LL1522: _temp1521=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1287)->f1; goto _LL1362;} else{ goto _LL1363;} _LL1363: if(*(( int*)
_temp1287) == Cyc_Absyn_Fill_e){ _LL1524: _temp1523=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1287)->f1; goto _LL1364;} else{ goto _LL1288;} _LL1290:{ void* _temp1525=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1531; struct Cyc_Absyn_Conref*
_temp1533; struct Cyc_Absyn_Tqual _temp1535; struct Cyc_Absyn_Conref* _temp1537;
void* _temp1539; void* _temp1541; _LL1527: if(( unsigned int) _temp1525 > 4u?*((
int*) _temp1525) == Cyc_Absyn_PointerType: 0){ _LL1532: _temp1531=(( struct Cyc_Absyn_PointerType_struct*)
_temp1525)->f1; _LL1542: _temp1541=( void*) _temp1531.elt_typ; goto _LL1540;
_LL1540: _temp1539=( void*) _temp1531.rgn_typ; goto _LL1538; _LL1538: _temp1537=
_temp1531.nullable; goto _LL1536; _LL1536: _temp1535= _temp1531.tq; goto _LL1534;
_LL1534: _temp1533= _temp1531.bounds; goto _LL1528;} else{ goto _LL1529;}
_LL1529: goto _LL1530; _LL1528: if(( void*) _temp1533->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1526;}{ void* _temp1543=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1533); struct Cyc_Absyn_Exp* _temp1549; _LL1545: if( _temp1543 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1546;} else{ goto _LL1547;} _LL1547: if((
unsigned int) _temp1543 > 1u?*(( int*) _temp1543) == Cyc_Absyn_Upper_b: 0){
_LL1550: _temp1549=(( struct Cyc_Absyn_Upper_b_struct*) _temp1543)->f1; goto
_LL1548;} else{ goto _LL1544;} _LL1546: { struct _tuple4* _temp1552=({ struct
_tuple4* _temp1551=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1551->f1= 0; _temp1551->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1551;});
goto _LL1553; _LL1553: { struct Cyc_Absyn_Exp* _temp1560= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List*(* _temp1554)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tuple4* _temp1557= _temp1552;
struct _tuple4* _temp1558= _temp1552; struct _tuple4* _temp1559= _temp1552;
struct _tuple4* _temp1555[ 3u]={ _temp1557, _temp1558, _temp1559}; struct
_tagged_ptr3 _temp1556={ _temp1555, _temp1555, _temp1555 + 3u}; _temp1554(
_temp1556);}), 0); goto _LL1561; _LL1561: if( nv->toplevel){( void*)( e->r=(
void*)(( void*) _temp1560->r));} else{ void* _temp1562= Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1541), Cyc_Toc_mt_tq); goto _LL1563; _LL1563: { struct
_tuple0* _temp1564= Cyc_Toc_temp_var(); goto _LL1565; _LL1565: { struct Cyc_Absyn_Exp*
_temp1566= Cyc_Absyn_var_exp( _temp1564, 0); goto _LL1567; _LL1567: { struct Cyc_Absyn_Stmt*
_temp1568= Cyc_Absyn_declare_stmt( _temp1564, _temp1562,( struct Cyc_Absyn_Exp*)
_temp1560, Cyc_Absyn_exp_stmt( _temp1566, 0), 0); goto _LL1569; _LL1569:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1568, 0))->r));}}}} goto
_LL1544;}} _LL1548:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0,
0))->r)); goto _LL1544; _LL1544:;} goto _LL1526; _LL1530:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1570= Cyc_Absynpp_typ2string( old_typ); xprintf("exp_to_c: null given type %.*s",
_temp1570.last_plus_one - _temp1570.curr, _temp1570.curr);})); goto _LL1526;
_LL1526:;} goto _LL1288; _LL1292: goto _LL1288; _LL1294:{ struct _handler_cons
_temp1571; _push_handler(& _temp1571);{ int _temp1573= 0; if( setjmp( _temp1571.handler)){
_temp1573= 1;} if( ! _temp1573){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1371))->r));; _pop_handler();} else{ void* _temp1572=( void*) _exn_thrown;
void* _temp1575= _temp1572; _LL1577: if( _temp1575 == Cyc_Dict_Absent){ goto
_LL1578;} else{ goto _LL1579;} _LL1579: goto _LL1580; _LL1578:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1581= Cyc_Absynpp_qvar2string( _temp1371); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1581.last_plus_one - _temp1581.curr, _temp1581.curr);})); return; _LL1580:(
void) _throw( _temp1575); _LL1576:;}}} goto _LL1288; _LL1296:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1582=(
unsigned char*)"unknownid"; struct _tagged_string _temp1583; _temp1583.curr=
_temp1582; _temp1583.base= _temp1582; _temp1583.last_plus_one= _temp1582 + 10;
_temp1583;})); goto _LL1288; _LL1298: { struct Cyc_List_List* _temp1584=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp1375); goto _LL1585; _LL1585: {
struct Cyc_List_List* _temp1586=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1375); goto _LL1587; _LL1587:(( void(*)( void(* f)( struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)(
Cyc_Toc_exp_to_c, nv, _temp1375);{ void* _temp1588= _temp1377; _LL1590: if(
_temp1588 ==( void*) Cyc_Absyn_Size){ goto _LL1591;} else{ goto _LL1592;}
_LL1592: if( _temp1588 ==( void*) Cyc_Absyn_Printf){ goto _LL1593;} else{ goto
_LL1594;} _LL1594: if( _temp1588 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1595;}
else{ goto _LL1596;} _LL1596: if( _temp1588 ==( void*) Cyc_Absyn_Fprintf){ goto
_LL1597;} else{ goto _LL1598;} _LL1598: if( _temp1588 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1599;} else{ goto _LL1600;} _LL1600: if( _temp1588 ==( void*) Cyc_Absyn_Plus){
goto _LL1601;} else{ goto _LL1602;} _LL1602: if( _temp1588 ==( void*) Cyc_Absyn_Minus){
goto _LL1603;} else{ goto _LL1604;} _LL1604: if( _temp1588 ==( void*) Cyc_Absyn_Eq){
goto _LL1605;} else{ goto _LL1606;} _LL1606: if( _temp1588 ==( void*) Cyc_Absyn_Neq){
goto _LL1607;} else{ goto _LL1608;} _LL1608: if( _temp1588 ==( void*) Cyc_Absyn_Gt){
goto _LL1609;} else{ goto _LL1610;} _LL1610: if( _temp1588 ==( void*) Cyc_Absyn_Gte){
goto _LL1611;} else{ goto _LL1612;} _LL1612: if( _temp1588 ==( void*) Cyc_Absyn_Lt){
goto _LL1613;} else{ goto _LL1614;} _LL1614: if( _temp1588 ==( void*) Cyc_Absyn_Lte){
goto _LL1615;} else{ goto _LL1616;} _LL1616: goto _LL1617; _LL1591: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1375))->hd;{
void* _temp1618=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1626; struct Cyc_Absyn_Tqual _temp1628; void*
_temp1630; struct Cyc_Absyn_PtrInfo _temp1632; struct Cyc_Absyn_Conref*
_temp1634; struct Cyc_Absyn_Tqual _temp1636; struct Cyc_Absyn_Conref* _temp1638;
void* _temp1640; void* _temp1642; _LL1620: if(( unsigned int) _temp1618 > 4u?*((
int*) _temp1618) == Cyc_Absyn_ArrayType: 0){ _LL1631: _temp1630=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1618)->f1; goto _LL1629; _LL1629:
_temp1628=(( struct Cyc_Absyn_ArrayType_struct*) _temp1618)->f2; goto _LL1627;
_LL1627: _temp1626=(( struct Cyc_Absyn_ArrayType_struct*) _temp1618)->f3; goto
_LL1621;} else{ goto _LL1622;} _LL1622: if(( unsigned int) _temp1618 > 4u?*((
int*) _temp1618) == Cyc_Absyn_PointerType: 0){ _LL1633: _temp1632=(( struct Cyc_Absyn_PointerType_struct*)
_temp1618)->f1; _LL1643: _temp1642=( void*) _temp1632.elt_typ; goto _LL1641;
_LL1641: _temp1640=( void*) _temp1632.rgn_typ; goto _LL1639; _LL1639: _temp1638=
_temp1632.nullable; goto _LL1637; _LL1637: _temp1636= _temp1632.tq; goto _LL1635;
_LL1635: _temp1634= _temp1632.bounds; goto _LL1623;} else{ goto _LL1624;}
_LL1624: goto _LL1625; _LL1621:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1626))->r)); goto _LL1619; _LL1623: { struct _tuple0*
_temp1644= Cyc_Toc_temp_var(); goto _LL1645; _LL1645: { void* _temp1646= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); goto _LL1647;
_LL1647: { struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1644, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1644, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4=
Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp( e2, e3, 0), 0);
struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1644, _temp1646,(
struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1619;}}} _LL1625:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
struct _tagged_string _temp1648= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp1586))->hd); struct _tagged_string _temp1649= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1648.last_plus_one - _temp1648.curr, _temp1648.curr, _temp1649.last_plus_one
- _temp1649.curr, _temp1649.curr);})); return; _LL1619:;} goto _LL1589;} _LL1593:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1377, 0, _temp1375,
_temp1586))->r)); goto _LL1589; _LL1595:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1377, 0, _temp1375, _temp1586))->r)); goto _LL1589; _LL1597: if( _temp1375
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char*
_temp1650=( unsigned char*)"fprintf without arguments"; struct _tagged_string
_temp1651; _temp1651.curr= _temp1650; _temp1651.base= _temp1650; _temp1651.last_plus_one=
_temp1650 + 26; _temp1651;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1377,({ struct Cyc_Core_Opt* _temp1652=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1652->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1375))->hd); _temp1652;}),(( struct Cyc_List_List*)
_check_null( _temp1375))->tl,(( struct Cyc_List_List*) _check_null( _temp1586))->tl))->r));
goto _LL1589; _LL1599: if( _temp1375 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1653=( unsigned char*)"sscanf without arguments";
struct _tagged_string _temp1654; _temp1654.curr= _temp1653; _temp1654.base=
_temp1653; _temp1654.last_plus_one= _temp1653 + 25; _temp1654;}));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1375))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1375))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1375))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1589; _LL1601: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1375))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1375))->tl))->hd; struct _tuple0* _temp1655= Cyc_Toc_temp_var(); goto
_LL1656; _LL1656: { void* _temp1657= Cyc_Toc_typ_to_c( old_typ); goto _LL1658;
_LL1658: { struct Cyc_Absyn_Stmt* _temp1659= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1655, 0), 0); goto _LL1660; _LL1660: { struct Cyc_Absyn_Exp* _temp1661= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1655, 0), Cyc_Toc_curr_sp, 0); goto _LL1662; _LL1662: {
struct Cyc_Absyn_Exp* _temp1664= Cyc_Absyn_assignop_exp( _temp1661,({ struct Cyc_Core_Opt*
_temp1663=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1663->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1663;}), e2, 0); goto
_LL1665; _LL1665: _temp1659= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1664,
0), _temp1659, 0); _temp1659= Cyc_Absyn_declare_stmt( _temp1655, _temp1657,(
struct Cyc_Absyn_Exp*) e1, _temp1659, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1659, 0))->r));}}}}} goto _LL1589; _LL1603: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null( _temp1584))->hd)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1375))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1375))->tl))->hd; void*
old_typ_e2=( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1584))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* _temp1666= Cyc_Toc_temp_var(); goto _LL1667;
_LL1667: { void* _temp1668= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_List_List*)
_check_null( _temp1584))->hd); goto _LL1669; _LL1669: { struct Cyc_Absyn_Stmt*
_temp1670= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1666, 0), 0); goto
_LL1671; _LL1671: { struct Cyc_Absyn_Exp* _temp1672= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1666, 0), Cyc_Toc_curr_sp, 0); goto _LL1673; _LL1673: {
struct Cyc_Absyn_Exp* _temp1675= Cyc_Absyn_assignop_exp( _temp1672,({ struct Cyc_Core_Opt*
_temp1674=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1674->v=( void*)(( void*) Cyc_Absyn_Minus); _temp1674;}), e2, 0); goto
_LL1676; _LL1676: _temp1670= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1675,
0), _temp1670, 0); _temp1670= Cyc_Absyn_declare_stmt( _temp1666, _temp1668,(
struct Cyc_Absyn_Exp*) e1, _temp1670, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1670, 0))->r));}}}}}} goto _LL1589; _LL1605: goto _LL1607; _LL1607: goto
_LL1609; _LL1609: goto _LL1611; _LL1611: goto _LL1613; _LL1613: goto _LL1615;
_LL1615: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1375))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1375))->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp1584))->hd; void* t2=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1584))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1589;} _LL1617: goto
_LL1589; _LL1589:;} goto _LL1288;}} _LL1300: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1381->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1381); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1379 ==(
void*) Cyc_Absyn_PreInc? 1: _temp1379 ==( void*) Cyc_Absyn_PreDec){ struct
_tuple0* _temp1677= Cyc_Toc_temp_var(); goto _LL1678; _LL1678: { void* _temp1679=
Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1680; _LL1680: { struct Cyc_Absyn_Stmt*
_temp1681= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1677,
0), 0), 0); goto _LL1682; _LL1682: { struct Cyc_Absyn_Exp* _temp1683= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1677, 0), Cyc_Toc_curr_sp, 0),
_temp1379, 0); goto _LL1684; _LL1684: _temp1681= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1683, 0), _temp1681, 0); _temp1681= Cyc_Absyn_declare_stmt( _temp1677,
_temp1679,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1381, 0),
_temp1681, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1681, 0))->r));}}}}
else{ struct _tuple0* _temp1685= Cyc_Toc_temp_var(); goto _LL1686; _LL1686: {
struct _tuple0* _temp1687= Cyc_Toc_temp_var(); goto _LL1688; _LL1688: { void*
_temp1689= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1690; _LL1690: { void*
_temp1691= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1692; _LL1692: { struct Cyc_Absyn_Stmt*
_temp1693= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1687, 0), 0); goto
_LL1694; _LL1694: { struct Cyc_Absyn_Exp* _temp1695= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1685, 0), Cyc_Toc_curr_sp, 0),
_temp1379, 0); goto _LL1696; _LL1696: _temp1693= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1695, 0), _temp1693, 0); _temp1693= Cyc_Absyn_declare_stmt( _temp1687,
_temp1689,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1685, 0), 0), _temp1693, 0); _temp1693= Cyc_Absyn_declare_stmt( _temp1685,
_temp1691,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1381, 0),
_temp1693, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1693, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1381)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1381, 0, Cyc_Toc_incr_lvalue,
_temp1379);( void*)( e->r=( void*)(( void*) _temp1381->r));}} goto _LL1288;}
_LL1302: { int e1_poly= Cyc_Toc_is_poly_project( _temp1387); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1387->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1383->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1387); Cyc_Toc_exp_to_c( nv, _temp1383); if(
_temp1385 != 0?( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1385))->v ==(
void*) Cyc_Absyn_Plus? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1385))->v
==( void*) Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){(
void*)( _temp1387->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1387->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1383->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1383->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1383->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1383->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1387)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1387, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1697=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1697[ 0]=({ struct _tuple11 _temp1698; _temp1698.f1=
_temp1385; _temp1698.f2= _temp1383; _temp1698;}); _temp1697;}));( void*)( e->r=(
void*)(( void*) _temp1387->r));} goto _LL1288;} _LL1304: Cyc_Toc_exp_to_c( nv,
_temp1393); Cyc_Toc_exp_to_c( nv, _temp1391); Cyc_Toc_exp_to_c( nv, _temp1389);
goto _LL1288; _LL1306: Cyc_Toc_exp_to_c( nv, _temp1397); Cyc_Toc_exp_to_c( nv,
_temp1395); goto _LL1288; _LL1308: _temp1407= _temp1401; _temp1405= _temp1399;
goto _LL1310; _LL1310: Cyc_Toc_exp_to_c( nv, _temp1407);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1405); goto _LL1288; _LL1312:{
struct _RegionHandle _temp1699= _new_region(); struct _RegionHandle* r=&
_temp1699; _push_region( r);{ void* _temp1700=( void*) _temp1412->type; goto
_LL1701; _LL1701: { void* _temp1702= Cyc_Toc_typ_to_c( _temp1700); goto _LL1703;
_LL1703: { void* _temp1704= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1420->topt))->v); goto _LL1705; _LL1705: { struct Cyc_Absyn_Exp*
_temp1706= Cyc_Absyn_uint_exp(( unsigned int) _temp1416, 0); goto _LL1707;
_LL1707: { void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1776=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1776[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1777; _temp1777.tag= Cyc_Absyn_ArrayType;
_temp1777.f1=( void*) _temp1700; _temp1777.f2= Cyc_Toc_mt_tq; _temp1777.f3=(
struct Cyc_Absyn_Exp*) _temp1706; _temp1777;}); _temp1776;})); struct _tuple0*
_temp1708= Cyc_Toc_temp_var(); goto _LL1709; _LL1709: { struct _tuple0*
_temp1710= Cyc_Toc_temp_var(); goto _LL1711; _LL1711: { struct Cyc_Absyn_Exp*
_temp1712= Cyc_Absyn_var_exp( _temp1710, 0); goto _LL1713; _LL1713: { struct
_tuple0* _temp1714= Cyc_Toc_temp_var(); goto _LL1715; _LL1715: { int _temp1716=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1418); goto _LL1717;
_LL1717: { int _temp1718= _temp1716 - _temp1416; goto _LL1719; _LL1719: { struct
_tagged_ptr5 vs=({ unsigned int _temp1772=( unsigned int) _temp1416; struct
_tuple0** _temp1773=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1772); struct _tagged_ptr5 _temp1775={ _temp1773, _temp1773,
_temp1773 + _temp1772};{ unsigned int _temp1774= _temp1772; unsigned int i; for(
i= 0; i < _temp1774; i ++){ _temp1773[ i]= Cyc_Toc_temp_var();}}; _temp1775;});
struct _tagged_ptr5 xs=({ unsigned int _temp1768=( unsigned int) _temp1718;
struct _tuple0** _temp1769=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1768); struct _tagged_ptr5 _temp1771={ _temp1769, _temp1769,
_temp1769 + _temp1768};{ unsigned int _temp1770= _temp1768; unsigned int i; for(
i= 0; i < _temp1770; i ++){ _temp1769[ i]= Cyc_Toc_temp_var();}}; _temp1771;});
struct Cyc_List_List* xexps=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(*
f)( struct _tuple0*, struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,( struct Cyc_Position_Segment*)
0,(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct _tagged_ptr5 arr))
Cyc_List_rfrom_array)( r, xs)); xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List*
_temp1720=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1720->hd=( void*) Cyc_Absyn_var_exp( _temp1714, 0); _temp1720->tl= 0;
_temp1720;}));{ struct Cyc_Absyn_Stmt* _temp1721= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( _temp1708, 0), xexps, 0), 0); goto _LL1722; _LL1722: { struct
Cyc_Absyn_Exp* _temp1723= Cyc_Absyn_add_exp( _temp1712, _temp1706, 0); goto
_LL1724; _LL1724: { struct Cyc_List_List* _temp1734=({ struct Cyc_List_List*(*
_temp1725)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)( struct _tagged_ptr3))
Cyc_List_list; struct _tuple4* _temp1728=({ struct _tuple4* _temp1733=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1733->f1= 0; _temp1733->f2=
_temp1712; _temp1733;}); struct _tuple4* _temp1729=({ struct _tuple4* _temp1732=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1732->f1= 0;
_temp1732->f2= _temp1712; _temp1732;}); struct _tuple4* _temp1730=({ struct
_tuple4* _temp1731=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1731->f1= 0; _temp1731->f2= _temp1723; _temp1731;}); struct _tuple4*
_temp1726[ 3u]={ _temp1728, _temp1729, _temp1730}; struct _tagged_ptr3 _temp1727={
_temp1726, _temp1726, _temp1726 + 3u}; _temp1725( _temp1727);}); goto _LL1735;
_LL1735: _temp1721= Cyc_Absyn_declare_stmt( _temp1714, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp1700,( void*) _temp1412->rgn, Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp(
0, _temp1734, 0), _temp1721, 0);{ struct Cyc_List_List* _temp1736=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_ptr5 arr)) Cyc_List_rfrom_array)( r, vs));
goto _LL1737; _LL1737: { struct Cyc_List_List* _temp1738=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_make_dle, _temp1736); goto _LL1739; _LL1739: _temp1721= Cyc_Absyn_declare_stmt(
_temp1710, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1738, 0), _temp1721, 0);{ struct Cyc_List_List* _temp1740=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1418);
goto _LL1741; _LL1741: { struct Cyc_List_List* _temp1742=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1414);
goto _LL1743; _LL1743:{ int i= _temp1416 - 1; for( 0; i >= 0;( i --, _temp1740=((
struct Cyc_List_List*) _check_null( _temp1740))->tl)){ struct Cyc_Absyn_Exp* arg=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1740))->hd;
void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c(
nv, arg); if( _temp1412->inject){ void* _temp1746; struct Cyc_Position_Segment*
_temp1748; struct Cyc_List_List* _temp1750; struct Cyc_List_List* _temp1752;
struct _tuple0* _temp1754; struct Cyc_Absyn_Tunionfield _temp1744=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1742))->hd); _LL1755: _temp1754=
_temp1744.name; goto _LL1753; _LL1753: _temp1752= _temp1744.tvs; goto _LL1751;
_LL1751: _temp1750= _temp1744.typs; goto _LL1749; _LL1749: _temp1748= _temp1744.loc;
goto _LL1747; _LL1747: _temp1746=( void*) _temp1744.sc; goto _LL1745; _LL1745:
_temp1742=(( struct Cyc_List_List*) _check_null( _temp1742))->tl;{ struct
_tuple0* _temp1756= Cyc_Toc_temp_var(); goto _LL1757; _LL1757: { struct Cyc_Absyn_Exp*
_temp1758= Cyc_Absyn_var_exp( _temp1756, 0); goto _LL1759; _LL1759: { void*
_temp1762= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1754,({
unsigned char* _temp1760=( unsigned char*)"_struct"; struct _tagged_string
_temp1761; _temp1761.curr= _temp1760; _temp1761.base= _temp1760; _temp1761.last_plus_one=
_temp1760 + 8; _temp1761;}))); goto _LL1763; _LL1763: _temp1721= Cyc_Absyn_declare_stmt(*((
struct _tuple0**(*)( struct _tagged_ptr5, unsigned int, unsigned int))
_check_unknown_subscript)( vs, sizeof( struct _tuple0*), i), _temp1702,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1702, Cyc_Absyn_address_exp( _temp1758,
0), 0), _temp1721, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp1750))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1764= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1758, Cyc_Toc_fieldname(
1), 0), arg, 0); goto _LL1765; _LL1765: _temp1721= Cyc_Absyn_seq_stmt( _temp1764,
_temp1721, 0);{ struct Cyc_Absyn_Stmt* _temp1766= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1758, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1754, 0), 0); goto
_LL1767; _LL1767: _temp1721= Cyc_Absyn_seq_stmt( _temp1766, _temp1721, 0);
_temp1721= Cyc_Absyn_declare_stmt( _temp1756, _temp1762, 0, _temp1721, 0);}}}}}}}
else{ _temp1721= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr5, unsigned int, unsigned int)) _check_unknown_subscript)( vs,
sizeof( struct _tuple0*), i), _temp1702,( struct Cyc_Absyn_Exp*) arg, _temp1721,
0);}}}{ int i= _temp1718 - 1; for( 0; i >= 0;( i --, _temp1740=(( struct Cyc_List_List*)
_check_null( _temp1740))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1740))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1721= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr5, unsigned int, unsigned int)) _check_unknown_subscript)( xs,
sizeof( struct _tuple0*), i), argtype,( struct Cyc_Absyn_Exp*) arg, _temp1721, 0);}}
Cyc_Toc_exp_to_c( nv, _temp1420); _temp1721= Cyc_Absyn_declare_stmt( _temp1708,
_temp1704,( struct Cyc_Absyn_Exp*) _temp1420, _temp1721, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1721, 0))->r));}}}}}}}}}}}}}}}}}};
_pop_region(& _temp1699);} goto _LL1288; _LL1314: Cyc_Toc_exp_to_c( nv,
_temp1422);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1422), 0))->r)); goto _LL1288; _LL1316: Cyc_Toc_exp_to_c(
nv, _temp1424); goto _LL1288; _LL1318: Cyc_Toc_exp_to_c( nv, _temp1428);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
for( 0; _temp1426 != 0; _temp1426=(( struct Cyc_List_List*) _check_null(
_temp1426))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*)
_check_null( _temp1426))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*)
Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1428, 0))->r)); break;}} goto _LL1288;} _LL1320: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1430->topt))->v; void* new_typ=*
_temp1434;* _temp1434= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1430);{ int noconv= 0;{ void* _temp1778= Cyc_Toc_conversion( old_t2,
new_typ); void* _temp1788; struct Cyc_Absyn_Exp* _temp1790; void* _temp1792; int
_temp1794; struct Cyc_Absyn_Exp* _temp1796; void* _temp1798; _LL1780: if(
_temp1778 ==( void*) Cyc_Toc_NoConv){ goto _LL1781;} else{ goto _LL1782;}
_LL1782: if(( unsigned int) _temp1778 > 1u?*(( int*) _temp1778) == Cyc_Toc_NullCheck:
0){ _LL1789: _temp1788=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp1778)->f1;
goto _LL1783;} else{ goto _LL1784;} _LL1784: if(( unsigned int) _temp1778 > 1u?*((
int*) _temp1778) == Cyc_Toc_TagPtr: 0){ _LL1793: _temp1792=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp1778)->f1; goto _LL1791; _LL1791: _temp1790=(( struct Cyc_Toc_TagPtr_struct*)
_temp1778)->f2; goto _LL1785;} else{ goto _LL1786;} _LL1786: if(( unsigned int)
_temp1778 > 1u?*(( int*) _temp1778) == Cyc_Toc_UntagPtr: 0){ _LL1799: _temp1798=(
void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp1778)->f1; goto _LL1797; _LL1797:
_temp1796=(( struct Cyc_Toc_UntagPtr_struct*) _temp1778)->f2; goto _LL1795;
_LL1795: _temp1794=(( struct Cyc_Toc_UntagPtr_struct*) _temp1778)->f3; goto
_LL1787;} else{ goto _LL1779;} _LL1781: goto _LL1779; _LL1783:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_null_check_conv( nv->toplevel, _temp1788, _temp1430))->r));
goto _LL1779; _LL1785:( void*)( e->r=( void*)(( void*)( Cyc_Toc_tag_ptr_conv( nv->toplevel,
_temp1792, _temp1790, _temp1430))->r)); goto _LL1779; _LL1787:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_untag_ptr_conv( nv->toplevel, _temp1798, _temp1796,
_temp1794, _temp1430))->r)); goto _LL1779; _LL1779:;} goto _LL1288;}} _LL1322:{
void* _temp1800=( void*) _temp1435->r; struct Cyc_Absyn_Structdecl* _temp1808;
struct Cyc_List_List* _temp1810; struct Cyc_Core_Opt* _temp1812; struct _tuple0*
_temp1814; struct Cyc_List_List* _temp1816; _LL1802: if(*(( int*) _temp1800) ==
Cyc_Absyn_Struct_e){ _LL1815: _temp1814=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1800)->f1; goto _LL1813; _LL1813: _temp1812=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1800)->f2; goto _LL1811; _LL1811: _temp1810=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1800)->f3; goto _LL1809; _LL1809: _temp1808=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1800)->f4; goto _LL1803;} else{ goto _LL1804;} _LL1804: if(*(( int*)
_temp1800) == Cyc_Absyn_Tuple_e){ _LL1817: _temp1816=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1800)->f1; goto _LL1805;} else{ goto _LL1806;} _LL1806: goto _LL1807;
_LL1803: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ struct _tagged_string _temp1818=( struct _tagged_string)
Cyc_Position_string_of_segment( _temp1435->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1818.last_plus_one - _temp1818.curr, _temp1818.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1435->topt))->v, 1, 0, _temp1810, _temp1814))->r)); goto
_LL1801; _LL1805: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ struct _tagged_string _temp1819=( struct _tagged_string)
Cyc_Position_string_of_segment( _temp1435->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1819.last_plus_one - _temp1819.curr, _temp1819.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1816))->r)); goto _LL1801;
_LL1807: Cyc_Toc_exp_to_c( nv, _temp1435); if( ! Cyc_Absyn_is_lvalue( _temp1435)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1435,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1435, 0))->r));} goto _LL1801; _LL1801:;} goto _LL1288; _LL1324: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
struct _tagged_string _temp1820=( struct _tagged_string) Cyc_Position_string_of_segment(
_temp1437->loc); xprintf("%.*s: new at top-level", _temp1820.last_plus_one -
_temp1820.curr, _temp1820.curr);}));}{ void* _temp1821=( void*) _temp1437->r;
struct Cyc_List_List* _temp1833; struct Cyc_Absyn_Exp* _temp1835; struct Cyc_Absyn_Exp*
_temp1837; struct Cyc_Absyn_Vardecl* _temp1839; struct Cyc_Absyn_Structdecl*
_temp1841; struct Cyc_List_List* _temp1843; struct Cyc_Core_Opt* _temp1845;
struct _tuple0* _temp1847; struct Cyc_List_List* _temp1849; _LL1823: if(*(( int*)
_temp1821) == Cyc_Absyn_Array_e){ _LL1834: _temp1833=(( struct Cyc_Absyn_Array_e_struct*)
_temp1821)->f1; goto _LL1824;} else{ goto _LL1825;} _LL1825: if(*(( int*)
_temp1821) == Cyc_Absyn_Comprehension_e){ _LL1840: _temp1839=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1821)->f1; goto _LL1838; _LL1838: _temp1837=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1821)->f2; goto _LL1836; _LL1836: _temp1835=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1821)->f3; goto _LL1826;} else{ goto _LL1827;} _LL1827: if(*(( int*)
_temp1821) == Cyc_Absyn_Struct_e){ _LL1848: _temp1847=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1821)->f1; goto _LL1846; _LL1846: _temp1845=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1821)->f2; goto _LL1844; _LL1844: _temp1843=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1821)->f3; goto _LL1842; _LL1842: _temp1841=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1821)->f4; goto _LL1828;} else{ goto _LL1829;} _LL1829: if(*(( int*)
_temp1821) == Cyc_Absyn_Tuple_e){ _LL1850: _temp1849=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1821)->f1; goto _LL1830;} else{ goto _LL1831;} _LL1831: goto _LL1832;
_LL1824: { struct _tuple0* _temp1851= Cyc_Toc_temp_var(); goto _LL1852; _LL1852: {
struct Cyc_Absyn_Exp* _temp1853= Cyc_Absyn_var_exp( _temp1851, 0); goto _LL1854;
_LL1854: { struct Cyc_Absyn_Stmt* _temp1855= Cyc_Toc_init_array( nv, _temp1853,
_temp1833, Cyc_Absyn_exp_stmt( _temp1853, 0)); goto _LL1856; _LL1856: { void*
old_elt_typ;{ void* _temp1857= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1863; struct Cyc_Absyn_Conref* _temp1865; struct Cyc_Absyn_Tqual _temp1867;
struct Cyc_Absyn_Conref* _temp1869; void* _temp1871; void* _temp1873; _LL1859:
if(( unsigned int) _temp1857 > 4u?*(( int*) _temp1857) == Cyc_Absyn_PointerType:
0){ _LL1864: _temp1863=(( struct Cyc_Absyn_PointerType_struct*) _temp1857)->f1;
_LL1874: _temp1873=( void*) _temp1863.elt_typ; goto _LL1872; _LL1872: _temp1871=(
void*) _temp1863.rgn_typ; goto _LL1870; _LL1870: _temp1869= _temp1863.nullable;
goto _LL1868; _LL1868: _temp1867= _temp1863.tq; goto _LL1866; _LL1866: _temp1865=
_temp1863.bounds; goto _LL1860;} else{ goto _LL1861;} _LL1861: goto _LL1862;
_LL1860: old_elt_typ= _temp1873; goto _LL1858; _LL1862: old_elt_typ=(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1875=(
unsigned char*)"exp_to_c:new array expression doesn't have ptr type"; struct
_tagged_string _temp1876; _temp1876.curr= _temp1875; _temp1876.base= _temp1875;
_temp1876.last_plus_one= _temp1875 + 52; _temp1876;})); goto _LL1858; _LL1858:;}{
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1877= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1878; _LL1878: { struct Cyc_Absyn_Exp*
_temp1879= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1833), 0), 0); goto
_LL1880; _LL1880: { struct Cyc_Absyn_Exp* e1; if( _temp1439 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1879);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1439); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1879);}{ struct Cyc_Absyn_Exp* _temp1881= Cyc_Absyn_cast_exp( _temp1877, e1,
0); goto _LL1882; _LL1882:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1851, _temp1877,( struct Cyc_Absyn_Exp*) _temp1881,
_temp1855, 0), 0))->r)); goto _LL1822;}}}}}}}} _LL1826: { int is_tagged_ptr= 0;{
void* _temp1883= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1889; struct Cyc_Absyn_Conref* _temp1891; struct Cyc_Absyn_Tqual _temp1893;
struct Cyc_Absyn_Conref* _temp1895; void* _temp1897; void* _temp1899; _LL1885:
if(( unsigned int) _temp1883 > 4u?*(( int*) _temp1883) == Cyc_Absyn_PointerType:
0){ _LL1890: _temp1889=(( struct Cyc_Absyn_PointerType_struct*) _temp1883)->f1;
_LL1900: _temp1899=( void*) _temp1889.elt_typ; goto _LL1898; _LL1898: _temp1897=(
void*) _temp1889.rgn_typ; goto _LL1896; _LL1896: _temp1895= _temp1889.nullable;
goto _LL1894; _LL1894: _temp1893= _temp1889.tq; goto _LL1892; _LL1892: _temp1891=
_temp1889.bounds; goto _LL1886;} else{ goto _LL1887;} _LL1887: goto _LL1888;
_LL1886:{ void* _temp1901=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1891))->v; void* _temp1907; _LL1903: if((
unsigned int) _temp1901 > 1u?*(( int*) _temp1901) == Cyc_Absyn_Eq_constr: 0){
_LL1908: _temp1907=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1901)->f1;
if( _temp1907 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1904;} else{ goto _LL1905;}}
else{ goto _LL1905;} _LL1905: goto _LL1906; _LL1904: is_tagged_ptr= 1; goto
_LL1902; _LL1906: goto _LL1902; _LL1902:;} goto _LL1884; _LL1888:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1909=(
unsigned char*)"exp_to_c: comprehension not an array type"; struct
_tagged_string _temp1910; _temp1910.curr= _temp1909; _temp1910.base= _temp1909;
_temp1910.last_plus_one= _temp1909 + 42; _temp1910;})); goto _LL1884; _LL1884:;}{
struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1835->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1837);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1839, Cyc_Absyn_var_exp(
max, 0), _temp1835, Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1911=
_new_region(); struct _RegionHandle* r=& _temp1911; _push_region( r);{ struct
Cyc_List_List* decls=({ struct Cyc_List_List* _temp1941=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1941->hd=( void*)({
struct _tuple10* _temp1942=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1942->f1= max; _temp1942->f2= Cyc_Absyn_uint_t; _temp1942->f3=(
struct Cyc_Absyn_Exp*) _temp1837; _temp1942;}); _temp1941->tl= 0; _temp1941;});
struct Cyc_Absyn_Exp* ai; if( _temp1439 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1439); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1912=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1912->hd=( void*)({ struct _tuple10*
_temp1913=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1913->f1= a; _temp1913->f2= ptr_typ; _temp1913->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1913;}); _temp1912->tl= decls; _temp1912;}); if( is_tagged_ptr){
struct _tuple0* _temp1914= Cyc_Toc_temp_var(); goto _LL1915; _LL1915: { void*
_temp1916= Cyc_Toc_typ_to_c( old_typ); goto _LL1917; _LL1917: { struct Cyc_Absyn_Exp*
_temp1927= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(* _temp1918)(
struct _tagged_ptr3)=( struct Cyc_List_List*(*)( struct _tagged_ptr3)) Cyc_List_list;
struct _tuple4* _temp1921=({ struct _tuple4* _temp1926=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1926->f1= 0; _temp1926->f2= Cyc_Absyn_var_exp(
a, 0); _temp1926;}); struct _tuple4* _temp1922=({ struct _tuple4* _temp1925=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1925->f1= 0;
_temp1925->f2= Cyc_Absyn_var_exp( a, 0); _temp1925;}); struct _tuple4* _temp1923=({
struct _tuple4* _temp1924=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1924->f1= 0; _temp1924->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1924;}); struct _tuple4* _temp1919[ 3u]={
_temp1921, _temp1922, _temp1923}; struct _tagged_ptr3 _temp1920={ _temp1919,
_temp1919, _temp1919 + 3u}; _temp1918( _temp1920);}), 0); goto _LL1928; _LL1928:
decls=({ struct Cyc_List_List* _temp1929=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1929->hd=( void*)({ struct _tuple10*
_temp1930=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1930->f1= _temp1914; _temp1930->f2= _temp1916; _temp1930->f3=( struct Cyc_Absyn_Exp*)
_temp1927; _temp1930;}); _temp1929->tl= decls; _temp1929;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1914, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
_temp1931= decls; goto _LL1932; _LL1932: for( 0; _temp1931 != 0; _temp1931=((
struct Cyc_List_List*) _check_null( _temp1931))->tl){ struct Cyc_Absyn_Exp*
_temp1935; void* _temp1937; struct _tuple0* _temp1939; struct _tuple10 _temp1933=*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1931))->hd);
_LL1940: _temp1939= _temp1933.f1; goto _LL1938; _LL1938: _temp1937= _temp1933.f2;
goto _LL1936; _LL1936: _temp1935= _temp1933.f3; goto _LL1934; _LL1934: s= Cyc_Absyn_declare_stmt(
_temp1939, _temp1937, _temp1935, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region(& _temp1911);} goto _LL1822;}}} _LL1828:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1437->topt))->v, 1, _temp1439, _temp1843, _temp1847))->r));
goto _LL1822; _LL1830:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv,
1, _temp1439, _temp1849))->r)); goto _LL1822; _LL1832: { void* _temp1943=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1437->topt))->v; goto _LL1944; _LL1944: {
struct _tuple0* _temp1945= Cyc_Toc_temp_var(); goto _LL1946; _LL1946: { struct
Cyc_Absyn_Stmt* _temp1947= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1945, 0),
0); goto _LL1948; _LL1948: { struct Cyc_Absyn_Exp* _temp1949= Cyc_Absyn_signed_int_exp(
0, 0); goto _LL1950; _LL1950: Cyc_Toc_exp_to_c( nv, _temp1437); _temp1947= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1945, 0),
_temp1949, 0), _temp1437, 0), _temp1947, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
_temp1943); void* _temp1951= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto
_LL1952; _LL1952: { struct Cyc_Absyn_Exp* _temp1953= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0); goto _LL1954; _LL1954: { struct Cyc_Absyn_Exp* e1; if( _temp1439 ==
0){ e1= Cyc_Toc_malloc_exp( _temp1943, _temp1953);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp1439); Cyc_Toc_exp_to_c( nv, r); e1=
Cyc_Toc_rmalloc_exp( r, _temp1953);}{ struct Cyc_Absyn_Exp* _temp1955= Cyc_Absyn_cast_exp(
_temp1951, e1, 0); goto _LL1956; _LL1956:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1945, _temp1951,( struct Cyc_Absyn_Exp*) _temp1955,
_temp1947, 0), 0))->r)); goto _LL1822;}}}}}}}} _LL1822:;} goto _LL1288; _LL1326:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp1957=(
struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1957[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1958; _temp1958.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1958.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1441);
_temp1958;}); _temp1957;}))); goto _LL1288; _LL1328: Cyc_Toc_exp_to_c( nv,
_temp1443); goto _LL1288; _LL1330: { void* e1_typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1445->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1445);{ void* _temp1959= e1_typ; struct Cyc_Absyn_PtrInfo _temp1965; struct
Cyc_Absyn_Conref* _temp1967; struct Cyc_Absyn_Tqual _temp1969; struct Cyc_Absyn_Conref*
_temp1971; void* _temp1973; void* _temp1975; _LL1961: if(( unsigned int)
_temp1959 > 4u?*(( int*) _temp1959) == Cyc_Absyn_PointerType: 0){ _LL1966:
_temp1965=(( struct Cyc_Absyn_PointerType_struct*) _temp1959)->f1; _LL1976:
_temp1975=( void*) _temp1965.elt_typ; goto _LL1974; _LL1974: _temp1973=( void*)
_temp1965.rgn_typ; goto _LL1972; _LL1972: _temp1971= _temp1965.nullable; goto
_LL1970; _LL1970: _temp1969= _temp1965.tq; goto _LL1968; _LL1968: _temp1967=
_temp1965.bounds; goto _LL1962;} else{ goto _LL1963;} _LL1963: goto _LL1964;
_LL1962:{ void* _temp1977=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1967); struct Cyc_Absyn_Exp* _temp1983; _LL1979: if( _temp1977 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1980;} else{ goto _LL1981;} _LL1981: if((
unsigned int) _temp1977 > 1u?*(( int*) _temp1977) == Cyc_Absyn_Upper_b: 0){
_LL1984: _temp1983=(( struct Cyc_Absyn_Upper_b_struct*) _temp1977)->f1; goto
_LL1982;} else{ goto _LL1978;} _LL1980: { struct _tuple0* x= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* _temp1985= Cyc_Toc_typ_to_c_array(
_temp1975); goto _LL1986; _LL1986: { void* tx= Cyc_Toc_add_tagged_ptr_type(
_temp1985, _temp1969); void* ta2= Cyc_Absyn_cstar_typ( _temp1985, _temp1969);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp(
Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt( 0), 0); struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt* s1=
Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,(
struct Cyc_Absyn_Exp*) _temp1445, s1, 0), 0))->r)); goto _LL1978;}} _LL1982: if(
Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1445->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1987=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1987->hd=(
void*) _temp1445; _temp1987->tl= 0; _temp1987;}), 0), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, e1_typ,(
struct Cyc_Absyn_Exp*) _temp1445, s3, 0), 0))->r));}} goto _LL1978; _LL1978:;}
goto _LL1960; _LL1964:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1988=( unsigned char*)"exp_to_c: Deref: non-pointer"; struct
_tagged_string _temp1989; _temp1989.curr= _temp1988; _temp1989.base= _temp1988;
_temp1989.last_plus_one= _temp1988 + 29; _temp1989;})); goto _LL1960; _LL1960:;}
goto _LL1288;} _LL1332: Cyc_Toc_exp_to_c( nv, _temp1449); if( Cyc_Toc_is_poly_project(
e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1288; _LL1334: { void* e1typ= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1453->topt))->v); Cyc_Toc_exp_to_c(
nv, _temp1453);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1992; struct Cyc_Absyn_Tqual _temp1994; struct Cyc_Absyn_Conref* _temp1996;
void* _temp1998; void* _temp2000; struct Cyc_Absyn_PtrInfo _temp1990= Cyc_Toc_get_ptr_type(
e1typ); _LL2001: _temp2000=( void*) _temp1990.elt_typ; goto _LL1999; _LL1999:
_temp1998=( void*) _temp1990.rgn_typ; goto _LL1997; _LL1997: _temp1996=
_temp1990.nullable; goto _LL1995; _LL1995: _temp1994= _temp1990.tq; goto _LL1993;
_LL1993: _temp1992= _temp1990.bounds; goto _LL1991; _LL1991:{ void* _temp2002=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1992); struct
Cyc_Absyn_Exp* _temp2008; _LL2004: if(( unsigned int) _temp2002 > 1u?*(( int*)
_temp2002) == Cyc_Absyn_Upper_b: 0){ _LL2009: _temp2008=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2002)->f1; goto _LL2005;} else{ goto _LL2006;} _LL2006: if( _temp2002 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL2007;} else{ goto _LL2003;} _LL2005: if(
Cyc_Evexp_eval_const_uint_exp( _temp2008) < 1){(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp2010=( unsigned char*)"exp_to_c:  StructArrow_e on pointer of size 0";
struct _tagged_string _temp2011; _temp2011.curr= _temp2010; _temp2011.base=
_temp2010; _temp2011.last_plus_one= _temp2010 + 46; _temp2011;}));}{ int
possibly_null;{ void* _temp2012=( void*)((( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1996))->v; int _temp2018;
_LL2014: if(( unsigned int) _temp2012 > 1u?*(( int*) _temp2012) == Cyc_Absyn_Eq_constr:
0){ _LL2019: _temp2018=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2012)->f1;
goto _LL2015;} else{ goto _LL2016;} _LL2016: goto _LL2017; _LL2015:
possibly_null= _temp2018; goto _LL2013; _LL2017: possibly_null= 0; goto _LL2013;
_LL2013:;} if( Cyc_Toc_is_nullable(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1453->topt))->v)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1453->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp2020=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2020->hd=(
void*) _temp1453; _temp2020->tl= 0; _temp2020;}), 0), 0), _temp1451, 0))->r));}
else{ struct _tuple0* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1453->topt))->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1451, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1453, s3, 0), 0))->r));}} goto _LL2003;} _LL2007: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2000); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1994); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2030=({ struct Cyc_List_List*(* _temp2021)( struct
_tagged_ptr6)=( struct Cyc_List_List*(*)( struct _tagged_ptr6)) Cyc_List_list;
struct _tuple1* _temp2024=({ struct _tuple1* _temp2029=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2029->f1= 0; _temp2029->f2= Cyc_Toc_mt_tq;
_temp2029->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1453->topt))->v); _temp2029;}); struct _tuple1* _temp2025=({ struct _tuple1*
_temp2028=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2028->f1=
0; _temp2028->f2= Cyc_Toc_mt_tq; _temp2028->f3= Cyc_Absyn_uint_t; _temp2028;});
struct _tuple1* _temp2026=({ struct _tuple1* _temp2027=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2027->f1= 0; _temp2027->f2= Cyc_Toc_mt_tq;
_temp2027->f3= Cyc_Absyn_uint_t; _temp2027;}); struct _tuple1* _temp2022[ 3u]={
_temp2024, _temp2025, _temp2026}; struct _tagged_ptr6 _temp2023={ _temp2022,
_temp2022, _temp2022 + 3u}; _temp2021( _temp2023);}); goto _LL2031; _LL2031: {
void* _temp2035= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2032=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2032[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2033; _temp2033.tag= Cyc_Absyn_FnType;
_temp2033.f1=({ struct Cyc_Absyn_FnInfo _temp2034; _temp2034.tvars= 0; _temp2034.effect=
0; _temp2034.ret_typ=( void*) ta2; _temp2034.args= _temp2030; _temp2034.c_varargs=
0; _temp2034.cyc_varargs= 0; _temp2034.attributes= 0; _temp2034;}); _temp2033;});
_temp2032;}), Cyc_Toc_mt_tq); goto _LL2036; _LL2036: { struct Cyc_Absyn_Exp*
fnexp= Cyc_Absyn_cast_exp( _temp2035, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv,
0), 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_uint_exp( 0, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_fncall_exp( fnexp,({
struct Cyc_List_List*(* _temp2037)( struct _tagged_ptr7)=( struct Cyc_List_List*(*)(
struct _tagged_ptr7)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2040= _temp1453;
struct Cyc_Absyn_Exp* _temp2041= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2042= e2; struct Cyc_Absyn_Exp* _temp2038[ 3u]={ _temp2040, _temp2041,
_temp2042}; struct _tagged_ptr7 _temp2039={ _temp2038, _temp2038, _temp2038 + 3u};
_temp2037( _temp2039);}), 0), _temp1451, 0))->r));}}} else{ struct _tuple0* x=
Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp1994);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( a, 0), _temp1451, 0), 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt( 0), 0); struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt* s1=
Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,(
struct Cyc_Absyn_Exp*) _temp1453, s1, 0), 0))->r));} goto _LL2003;} _LL2003:;}
if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1288;}} _LL1336: { void* _temp2043= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1457->topt))->v); goto _LL2044;
_LL2044: Cyc_Toc_exp_to_c( nv, _temp1457); Cyc_Toc_exp_to_c( nv, _temp1455);{
void* _temp2045= _temp2043; struct Cyc_List_List* _temp2053; struct Cyc_Absyn_PtrInfo
_temp2055; struct Cyc_Absyn_Conref* _temp2057; struct Cyc_Absyn_Tqual _temp2059;
struct Cyc_Absyn_Conref* _temp2061; void* _temp2063; void* _temp2065; _LL2047:
if(( unsigned int) _temp2045 > 4u?*(( int*) _temp2045) == Cyc_Absyn_TupleType: 0){
_LL2054: _temp2053=(( struct Cyc_Absyn_TupleType_struct*) _temp2045)->f1; goto
_LL2048;} else{ goto _LL2049;} _LL2049: if(( unsigned int) _temp2045 > 4u?*((
int*) _temp2045) == Cyc_Absyn_PointerType: 0){ _LL2056: _temp2055=(( struct Cyc_Absyn_PointerType_struct*)
_temp2045)->f1; _LL2066: _temp2065=( void*) _temp2055.elt_typ; goto _LL2064;
_LL2064: _temp2063=( void*) _temp2055.rgn_typ; goto _LL2062; _LL2062: _temp2061=
_temp2055.nullable; goto _LL2060; _LL2060: _temp2059= _temp2055.tq; goto _LL2058;
_LL2058: _temp2057= _temp2055.bounds; goto _LL2050;} else{ goto _LL2051;}
_LL2051: goto _LL2052; _LL2048: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1455) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1457, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL2046;} _LL2050:{ void*
_temp2067=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp2057); struct Cyc_Absyn_Exp* _temp2073; _LL2069: if(( unsigned int)
_temp2067 > 1u?*(( int*) _temp2067) == Cyc_Absyn_Upper_b: 0){ _LL2074: _temp2073=((
struct Cyc_Absyn_Upper_b_struct*) _temp2067)->f1; goto _LL2070;} else{ goto
_LL2071;} _LL2071: if( _temp2067 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2072;}
else{ goto _LL2068;} _LL2070: _temp2073= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp2073), 0);{ int possibly_null;{ void* _temp2075=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2061))->v; int
_temp2081; _LL2077: if(( unsigned int) _temp2075 > 1u?*(( int*) _temp2075) ==
Cyc_Absyn_Eq_constr: 0){ _LL2082: _temp2081=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2075)->f1; goto _LL2078;} else{ goto _LL2079;} _LL2079: goto _LL2080;
_LL2078: possibly_null= _temp2081; goto _LL2076; _LL2080: possibly_null= 0; goto
_LL2076; _LL2076:;}{ void* ta1= Cyc_Toc_typ_to_c( _temp2065); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp2059); if( Cyc_Toc_use_runtimecheck_funs){ if( possibly_null){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv, 0),({ struct Cyc_List_List*(*
_temp2083)( struct _tagged_ptr7)=( struct Cyc_List_List*(*)( struct _tagged_ptr7))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp2086= _temp1457; struct Cyc_Absyn_Exp*
_temp2087= _temp2073; struct Cyc_Absyn_Exp* _temp2088= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp2089= _temp1455; struct Cyc_Absyn_Exp*
_temp2084[ 4u]={ _temp2086, _temp2087, _temp2088, _temp2089}; struct
_tagged_ptr7 _temp2085={ _temp2084, _temp2084, _temp2084 + 4u}; _temp2083(
_temp2085);}), 0), 0), 0))->r));} else{( void*)( _temp1455->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List*(* _temp2090)( struct _tagged_ptr7)=( struct Cyc_List_List*(*)(
struct _tagged_ptr7)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2093= _temp2073;
struct Cyc_Absyn_Exp* _temp2094= Cyc_Absyn_copy_exp( _temp1455); struct Cyc_Absyn_Exp*
_temp2091[ 2u]={ _temp2093, _temp2094}; struct _tagged_ptr7 _temp2092={
_temp2091, _temp2091, _temp2091 + 2u}; _temp2090( _temp2092);}), 0))->r));}}
else{ struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp(
i, 0), _temp2073, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp((
void*) e3->r, 0), 0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1455, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1457, s1, 0), 0))->r));}} goto _LL2068;}} _LL2072: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2065); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2059); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2104=({ struct Cyc_List_List*(* _temp2095)( struct
_tagged_ptr6)=( struct Cyc_List_List*(*)( struct _tagged_ptr6)) Cyc_List_list;
struct _tuple1* _temp2098=({ struct _tuple1* _temp2103=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2103->f1= 0; _temp2103->f2= Cyc_Toc_mt_tq;
_temp2103->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1457->topt))->v); _temp2103;}); struct _tuple1* _temp2099=({ struct _tuple1*
_temp2102=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2102->f1=
0; _temp2102->f2= Cyc_Toc_mt_tq; _temp2102->f3= Cyc_Absyn_uint_t; _temp2102;});
struct _tuple1* _temp2100=({ struct _tuple1* _temp2101=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2101->f1= 0; _temp2101->f2= Cyc_Toc_mt_tq;
_temp2101->f3= Cyc_Absyn_uint_t; _temp2101;}); struct _tuple1* _temp2096[ 3u]={
_temp2098, _temp2099, _temp2100}; struct _tagged_ptr6 _temp2097={ _temp2096,
_temp2096, _temp2096 + 3u}; _temp2095( _temp2097);}); goto _LL2105; _LL2105: {
void* _temp2109= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2106=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2106[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2107; _temp2107.tag= Cyc_Absyn_FnType;
_temp2107.f1=({ struct Cyc_Absyn_FnInfo _temp2108; _temp2108.tvars= 0; _temp2108.effect=
0; _temp2108.ret_typ=( void*) ta2; _temp2108.args= _temp2104; _temp2108.c_varargs=
0; _temp2108.cyc_varargs= 0; _temp2108.attributes= 0; _temp2108;}); _temp2107;});
_temp2106;}), Cyc_Toc_mt_tq); goto _LL2110; _LL2110: { struct Cyc_Absyn_Exp*
fnexp= Cyc_Absyn_cast_exp( _temp2109, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv,
0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp(
fnexp,({ struct Cyc_List_List*(* _temp2111)( struct _tagged_ptr7)=( struct Cyc_List_List*(*)(
struct _tagged_ptr7)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2114= _temp1457;
struct Cyc_Absyn_Exp* _temp2115= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2116= _temp1455; struct Cyc_Absyn_Exp* _temp2112[ 3u]={ _temp2114,
_temp2115, _temp2116}; struct _tagged_ptr7 _temp2113={ _temp2112, _temp2112,
_temp2112 + 3u}; _temp2111( _temp2113);}), 0), 0))->r));}}} else{ struct _tuple0*
x= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2059);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1455, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1457, s1, 0), 0))->r));} goto _LL2068;} _LL2068:;}
goto _LL2046; _LL2052:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2117=( unsigned char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp2118; _temp2118.curr= _temp2117; _temp2118.base=
_temp2117; _temp2118.last_plus_one= _temp2117 + 49; _temp2118;})); goto _LL2046;
_LL2046:;} goto _LL1288;} _LL1338: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1459))->r));} else{ struct Cyc_List_List*
_temp2119=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1459); goto
_LL2120; _LL2120: { struct _tagged_string* _temp2121= Cyc_Toc_add_tuple_type(
_temp2119); goto _LL2122; _LL2122: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1459 != 0;( _temp1459=(( struct Cyc_List_List*) _check_null(
_temp1459))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1459))->hd); dles=({ struct Cyc_List_List*
_temp2123=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2123->hd=( void*)({ struct _tuple4* _temp2124=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2124->f1= 0; _temp2124->f2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1459))->hd; _temp2124;}); _temp2123->tl=
dles; _temp2123;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));}}} goto _LL1288; _LL1340:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ unsigned char* _temp2125=( unsigned char*)"compoundlit";
struct _tagged_string _temp2126; _temp2126.curr= _temp2125; _temp2126.base=
_temp2125; _temp2126.last_plus_one= _temp2125 + 12; _temp2126;})); goto _LL1288;
_LL1342:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1465, 0))->r));{ struct Cyc_List_List* _temp2127= _temp1465; goto _LL2128;
_LL2128: for( 0; _temp2127 != 0; _temp2127=(( struct Cyc_List_List*) _check_null(
_temp2127))->tl){ struct _tuple4 _temp2131; struct Cyc_Absyn_Exp* _temp2132;
struct Cyc_List_List* _temp2134; struct _tuple4* _temp2129=( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp2127))->hd; _temp2131=* _temp2129;
_LL2135: _temp2134= _temp2131.f1; goto _LL2133; _LL2133: _temp2132= _temp2131.f2;
goto _LL2130; _LL2130: Cyc_Toc_exp_to_c( nv, _temp2132);}} goto _LL1288; _LL1344: {
unsigned int _temp2136= Cyc_Evexp_eval_const_uint_exp( _temp1469); goto _LL2137;
_LL2137: Cyc_Toc_exp_to_c( nv, _temp1467);{ struct Cyc_List_List* es= 0; if( !
Cyc_Toc_is_zero( _temp1467)){ unsigned int i= 0; for( 0; i < _temp2136; i ++){
es=({ struct Cyc_List_List* _temp2138=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2138->hd=( void*)({ struct _tuple4*
_temp2139=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp2139->f1=
0; _temp2139->f2= _temp1467; _temp2139;}); _temp2138->tl= es; _temp2138;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL1288;}} _LL1346: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1475, _temp1479))->r));} else{ if(
_temp1473 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2140=( unsigned char*)"Struct_e: missing structdecl pointer";
struct _tagged_string _temp2141; _temp2141.curr= _temp2140; _temp2141.base=
_temp2140; _temp2141.last_plus_one= _temp2140 + 37; _temp2141;}));}{ struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*) _check_null( _temp1473); struct
_RegionHandle _temp2142= _new_region(); struct _RegionHandle* rgn=& _temp2142;
_push_region( rgn);{ struct Cyc_List_List* _temp2143=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1475,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd2->fields))->v); goto _LL2144; _LL2144: { struct Cyc_List_List* _temp2145= 0;
goto _LL2146; _LL2146:{ struct Cyc_List_List* _temp2147=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto _LL2148; _LL2148: for(
0; _temp2147 != 0; _temp2147=(( struct Cyc_List_List*) _check_null( _temp2147))->tl){
struct Cyc_List_List* _temp2149= _temp2143; goto _LL2150; _LL2150: for( 0;
_temp2149 != 0; _temp2149=(( struct Cyc_List_List*) _check_null( _temp2149))->tl){
if((*(( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2149))->hd)).f1
==( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2147))->hd){ struct _tuple12 _temp2153; struct Cyc_Absyn_Exp* _temp2154;
struct Cyc_Absyn_Structfield* _temp2156; struct _tuple12* _temp2151=( struct
_tuple12*)(( struct Cyc_List_List*) _check_null( _temp2149))->hd; _temp2153=*
_temp2151; _LL2157: _temp2156= _temp2153.f1; goto _LL2155; _LL2155: _temp2154=
_temp2153.f2; goto _LL2152; _LL2152: { void* _temp2158=( void*) _temp2156->type;
goto _LL2159; _LL2159: Cyc_Toc_exp_to_c( nv, _temp2154); if( Cyc_Toc_is_void_star(
_temp2158)){( void*)( _temp2154->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp2154->r, 0), 0))->r));} _temp2145=({ struct Cyc_List_List*
_temp2160=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2160->hd=( void*)({ struct _tuple4* _temp2161=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2161->f1= 0; _temp2161->f2= _temp2154; _temp2161;});
_temp2160->tl= _temp2145; _temp2160;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp2145), 0))->r));}}; _pop_region(& _temp2142);}} goto
_LL1288; _LL1348: { struct Cyc_List_List* fs;{ void* _temp2162= Cyc_Tcutil_compress(
_temp1483); struct Cyc_List_List* _temp2168; _LL2164: if(( unsigned int)
_temp2162 > 4u?*(( int*) _temp2162) == Cyc_Absyn_AnonStructType: 0){ _LL2169:
_temp2168=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2162)->f1; goto
_LL2165;} else{ goto _LL2166;} _LL2166: goto _LL2167; _LL2165: fs= _temp2168;
goto _LL2163; _LL2167: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct _tagged_string _temp2170=
Cyc_Absynpp_typ2string( _temp1483); xprintf("anon struct has type %.*s",
_temp2170.last_plus_one - _temp2170.curr, _temp2170.curr);})); goto _LL2163;
_LL2163:;}{ struct _RegionHandle _temp2171= _new_region(); struct _RegionHandle*
rgn=& _temp2171; _push_region( rgn);{ struct Cyc_List_List* _temp2172=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1481, fs); goto _LL2173; _LL2173: for( 0; _temp2172 != 0;
_temp2172=(( struct Cyc_List_List*) _check_null( _temp2172))->tl){ struct
_tuple12 _temp2176; struct Cyc_Absyn_Exp* _temp2177; struct Cyc_Absyn_Structfield*
_temp2179; struct _tuple12* _temp2174=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp2172))->hd; _temp2176=* _temp2174; _LL2180: _temp2179=
_temp2176.f1; goto _LL2178; _LL2178: _temp2177= _temp2176.f2; goto _LL2175;
_LL2175: { void* _temp2181=( void*) _temp2179->type; goto _LL2182; _LL2182: Cyc_Toc_exp_to_c(
nv, _temp2177); if( Cyc_Toc_is_void_star( _temp2181)){( void*)( _temp2177->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp2177->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1481, 0))->r));}; _pop_region(& _temp2171);} goto _LL1288;} _LL1350: {
struct _tuple0* qv= _temp1485->name; if( _temp1487->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2183= _temp1487->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1487->fields))->v;
goto _LL2184; _LL2184: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2183))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2183))->hd)->typs
== 0){ tag_count ++;} _temp2183=(( struct Cyc_List_List*) _check_null( _temp2183))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1288;} _LL1352: { struct _tuple0* _temp2185= _temp1495->name; goto
_LL2186; _LL2186: { struct Cyc_List_List* _temp2187= _temp1495->typs; goto
_LL2188; _LL2188: { struct _tuple0* _temp2189= Cyc_Toc_temp_var(); goto _LL2190;
_LL2190: { struct Cyc_Absyn_Exp* _temp2191= Cyc_Absyn_var_exp( _temp2189, 0);
goto _LL2192; _LL2192: { void* _temp2195= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2185,({ unsigned char* _temp2193=( unsigned char*)"_struct"; struct
_tagged_string _temp2194; _temp2194.curr= _temp2193; _temp2194.base= _temp2193;
_temp2194.last_plus_one= _temp2193 + 8; _temp2194;}))); goto _LL2196; _LL2196:
if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp; if( _temp1497->is_xtunion){
tag_exp= Cyc_Absyn_var_exp( _temp2185, 0);} else{ int tag_count= 0; struct Cyc_List_List*
_temp2197= _temp1497->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1497->fields))->v; goto _LL2198; _LL2198: while( Cyc_Absyn_qvar_cmp(
_temp2185,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2197))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2197))->hd)->typs != 0){ tag_count ++;}
_temp2197=(( struct Cyc_List_List*) _check_null( _temp2197))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2199= 0; goto _LL2200;
_LL2200: for( 0; _temp1499 != 0;( _temp1499=(( struct Cyc_List_List*)
_check_null( _temp1499))->tl, _temp2187=(( struct Cyc_List_List*) _check_null(
_temp2187))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1499))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2187))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2199=({ struct Cyc_List_List* _temp2201=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2201[ 0]=({ struct Cyc_List_List
_temp2202; _temp2202.hd=( void*)({ struct _tuple4* _temp2203=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2203[ 0]=({ struct _tuple4
_temp2204; _temp2204.f1= 0; _temp2204.f2= cur_e; _temp2204;}); _temp2203;});
_temp2202.tl= _temp2199; _temp2202;}); _temp2201;});} _temp2199=({ struct Cyc_List_List*
_temp2205=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2205[ 0]=({ struct Cyc_List_List _temp2206; _temp2206.hd=( void*)({ struct
_tuple4* _temp2207=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2207[ 0]=({ struct _tuple4 _temp2208; _temp2208.f1= 0; _temp2208.f2=
tag_exp; _temp2208;}); _temp2207;}); _temp2206.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2199); _temp2206;}); _temp2205;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2209=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2209->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2185,({ unsigned char*
_temp2210=( unsigned char*)"_struct"; struct _tagged_string _temp2211; _temp2211.curr=
_temp2210; _temp2211.base= _temp2210; _temp2211.last_plus_one= _temp2210 + 8;
_temp2211;})); _temp2209;}), _temp2199, 0))->r));}} else{ struct Cyc_List_List*
_temp2213=({ struct Cyc_List_List* _temp2212=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2212->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2191, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp2185, 0), 0); _temp2212->tl= 0; _temp2212;}); goto _LL2214; _LL2214:{ int i=
1; for( 0; _temp1499 != 0;((( _temp1499=(( struct Cyc_List_List*) _check_null(
_temp1499))->tl, i ++)), _temp2187=(( struct Cyc_List_List*) _check_null(
_temp2187))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1499))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2187))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp2215= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2191, Cyc_Toc_fieldname( i), 0), cur_e, 0);
goto _LL2216; _LL2216: _temp2213=({ struct Cyc_List_List* _temp2217=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2217->hd=( void*) _temp2215;
_temp2217->tl= _temp2213; _temp2217;});}}}{ struct Cyc_Absyn_Stmt* _temp2218=
Cyc_Absyn_exp_stmt( _temp2191, 0); goto _LL2219; _LL2219: { struct Cyc_Absyn_Stmt*
_temp2221= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2220=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2220->hd=( void*) _temp2218;
_temp2220->tl= _temp2213; _temp2220;})), 0); goto _LL2222; _LL2222:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp2189,
_temp2195, 0, _temp2221, 0), 0))->r));}}} goto _LL1288;}}}}} _LL1354: goto
_LL1288; _LL1356: { void* t_c= Cyc_Toc_typ_to_c( _temp1511); if( _temp1513 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1513); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1511, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL1288;} _LL1358: Cyc_Toc_stmt_to_c(
nv, _temp1515); goto _LL1288; _LL1360:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2223=( unsigned char*)"UnresolvedMem"; struct _tagged_string
_temp2224; _temp2224.curr= _temp2223; _temp2224.base= _temp2223; _temp2224.last_plus_one=
_temp2223 + 14; _temp2224;})); goto _LL1288; _LL1362:(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp2225=( unsigned char*)"codegen";
struct _tagged_string _temp2226; _temp2226.curr= _temp2225; _temp2226.base=
_temp2225; _temp2226.last_plus_one= _temp2225 + 8; _temp2226;})); goto _LL1288;
_LL1364:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp2227=( unsigned char*)"fill"; struct _tagged_string _temp2228; _temp2228.curr=
_temp2227; _temp2228.base= _temp2227; _temp2228.last_plus_one= _temp2227 + 5;
_temp2228;})); goto _LL1288; _LL1288:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_string*
succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple14{ struct _tuple0* f1; void* f2;
} ; struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct
_RegionHandle* rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path,
struct Cyc_Absyn_Pat* p, struct _tagged_string* succ_lab, struct _tagged_string*
fail_lab, struct Cyc_List_List* decls){ struct Cyc_Absyn_Stmt* s;{ void*
_temp2229=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp2265; struct Cyc_Absyn_Vardecl
_temp2267; struct Cyc_List_List* _temp2268; struct Cyc_Core_Opt* _temp2270;
struct Cyc_Absyn_Exp* _temp2272; void* _temp2274; struct Cyc_Absyn_Tqual
_temp2276; struct _tuple0* _temp2278; void* _temp2280; struct Cyc_Absyn_Vardecl*
_temp2282; int _temp2284; void* _temp2286; unsigned char _temp2288; struct
_tagged_string _temp2290; struct Cyc_Absyn_Enumfield* _temp2292; struct Cyc_Absyn_Enumdecl*
_temp2294; struct Cyc_List_List* _temp2296; struct Cyc_List_List* _temp2298;
struct Cyc_Absyn_Tunionfield* _temp2300; struct Cyc_Absyn_Tuniondecl* _temp2302;
struct Cyc_Absyn_Pat* _temp2305; struct Cyc_Absyn_Pat _temp2307; struct Cyc_Position_Segment*
_temp2308; struct Cyc_Core_Opt* _temp2310; void* _temp2312; struct Cyc_List_List*
_temp2314; struct Cyc_List_List* _temp2316; struct Cyc_Absyn_Tunionfield*
_temp2318; struct Cyc_Absyn_Tuniondecl* _temp2320; struct Cyc_List_List*
_temp2322; struct Cyc_List_List* _temp2324; struct Cyc_Absyn_Tunionfield*
_temp2326; struct Cyc_Absyn_Tuniondecl* _temp2328; struct Cyc_List_List*
_temp2330; struct Cyc_List_List* _temp2332; struct Cyc_List_List* _temp2334;
struct Cyc_Core_Opt* _temp2336; struct Cyc_Absyn_Structdecl* _temp2338; struct
Cyc_Absyn_Pat* _temp2340; struct _tuple0* _temp2342; struct Cyc_List_List*
_temp2344; struct Cyc_List_List* _temp2346; struct _tuple0* _temp2348; struct
Cyc_List_List* _temp2350; struct Cyc_List_List* _temp2352; struct _tuple0*
_temp2354; _LL2231: if(( unsigned int) _temp2229 > 2u?*(( int*) _temp2229) ==
Cyc_Absyn_Var_p: 0){ _LL2266: _temp2265=(( struct Cyc_Absyn_Var_p_struct*)
_temp2229)->f1; _temp2267=* _temp2265; _LL2281: _temp2280=( void*) _temp2267.sc;
goto _LL2279; _LL2279: _temp2278= _temp2267.name; goto _LL2277; _LL2277:
_temp2276= _temp2267.tq; goto _LL2275; _LL2275: _temp2274=( void*) _temp2267.type;
goto _LL2273; _LL2273: _temp2272= _temp2267.initializer; goto _LL2271; _LL2271:
_temp2270= _temp2267.rgn; goto _LL2269; _LL2269: _temp2268= _temp2267.attributes;
goto _LL2232;} else{ goto _LL2233;} _LL2233: if( _temp2229 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2234;} else{ goto _LL2235;} _LL2235: if(( unsigned int) _temp2229 > 2u?*((
int*) _temp2229) == Cyc_Absyn_Reference_p: 0){ _LL2283: _temp2282=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2229)->f1; goto _LL2236;} else{ goto _LL2237;} _LL2237: if( _temp2229 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2238;} else{ goto _LL2239;} _LL2239: if((
unsigned int) _temp2229 > 2u?*(( int*) _temp2229) == Cyc_Absyn_Int_p: 0){
_LL2287: _temp2286=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2229)->f1;
goto _LL2285; _LL2285: _temp2284=(( struct Cyc_Absyn_Int_p_struct*) _temp2229)->f2;
goto _LL2240;} else{ goto _LL2241;} _LL2241: if(( unsigned int) _temp2229 > 2u?*((
int*) _temp2229) == Cyc_Absyn_Char_p: 0){ _LL2289: _temp2288=(( struct Cyc_Absyn_Char_p_struct*)
_temp2229)->f1; goto _LL2242;} else{ goto _LL2243;} _LL2243: if(( unsigned int)
_temp2229 > 2u?*(( int*) _temp2229) == Cyc_Absyn_Float_p: 0){ _LL2291: _temp2290=((
struct Cyc_Absyn_Float_p_struct*) _temp2229)->f1; goto _LL2244;} else{ goto
_LL2245;} _LL2245: if(( unsigned int) _temp2229 > 2u?*(( int*) _temp2229) == Cyc_Absyn_Enum_p:
0){ _LL2295: _temp2294=(( struct Cyc_Absyn_Enum_p_struct*) _temp2229)->f1; goto
_LL2293; _LL2293: _temp2292=(( struct Cyc_Absyn_Enum_p_struct*) _temp2229)->f2;
goto _LL2246;} else{ goto _LL2247;} _LL2247: if(( unsigned int) _temp2229 > 2u?*((
int*) _temp2229) == Cyc_Absyn_Tunion_p: 0){ _LL2303: _temp2302=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2229)->f1; goto _LL2301; _LL2301: _temp2300=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2229)->f2; goto _LL2299; _LL2299: _temp2298=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2229)->f3; goto _LL2297; _LL2297: _temp2296=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2229)->f4; if( _temp2296 == 0){ goto _LL2248;} else{ goto _LL2249;}} else{
goto _LL2249;} _LL2249: if(( unsigned int) _temp2229 > 2u?*(( int*) _temp2229)
== Cyc_Absyn_Pointer_p: 0){ _LL2306: _temp2305=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2229)->f1; _temp2307=* _temp2305; _LL2313: _temp2312=( void*) _temp2307.r;
if(( unsigned int) _temp2312 > 2u?*(( int*) _temp2312) == Cyc_Absyn_Tunion_p: 0){
_LL2321: _temp2320=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2312)->f1; goto
_LL2319; _LL2319: _temp2318=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2312)->f2;
goto _LL2317; _LL2317: _temp2316=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2312)->f3;
goto _LL2315; _LL2315: _temp2314=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2312)->f4;
goto _LL2311;} else{ goto _LL2251;} _LL2311: _temp2310= _temp2307.topt; goto
_LL2309; _LL2309: _temp2308= _temp2307.loc; goto _LL2304;} else{ goto _LL2251;}
_LL2304: if( _temp2314 != 0){ goto _LL2250;} else{ goto _LL2251;} _LL2251: if((
unsigned int) _temp2229 > 2u?*(( int*) _temp2229) == Cyc_Absyn_Tunion_p: 0){
_LL2329: _temp2328=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2229)->f1; goto
_LL2327; _LL2327: _temp2326=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2229)->f2;
goto _LL2325; _LL2325: _temp2324=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2229)->f3;
goto _LL2323; _LL2323: _temp2322=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2229)->f4;
goto _LL2252;} else{ goto _LL2253;} _LL2253: if(( unsigned int) _temp2229 > 2u?*((
int*) _temp2229) == Cyc_Absyn_Tuple_p: 0){ _LL2331: _temp2330=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2229)->f1; goto _LL2254;} else{ goto _LL2255;} _LL2255: if(( unsigned int)
_temp2229 > 2u?*(( int*) _temp2229) == Cyc_Absyn_Struct_p: 0){ _LL2339:
_temp2338=(( struct Cyc_Absyn_Struct_p_struct*) _temp2229)->f1; goto _LL2337;
_LL2337: _temp2336=(( struct Cyc_Absyn_Struct_p_struct*) _temp2229)->f2; goto
_LL2335; _LL2335: _temp2334=(( struct Cyc_Absyn_Struct_p_struct*) _temp2229)->f3;
goto _LL2333; _LL2333: _temp2332=(( struct Cyc_Absyn_Struct_p_struct*) _temp2229)->f4;
goto _LL2256;} else{ goto _LL2257;} _LL2257: if(( unsigned int) _temp2229 > 2u?*((
int*) _temp2229) == Cyc_Absyn_Pointer_p: 0){ _LL2341: _temp2340=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2229)->f1; goto _LL2258;} else{ goto _LL2259;} _LL2259: if(( unsigned int)
_temp2229 > 2u?*(( int*) _temp2229) == Cyc_Absyn_UnknownId_p: 0){ _LL2343:
_temp2342=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2229)->f1; goto _LL2260;}
else{ goto _LL2261;} _LL2261: if(( unsigned int) _temp2229 > 2u?*(( int*)
_temp2229) == Cyc_Absyn_UnknownCall_p: 0){ _LL2349: _temp2348=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2229)->f1; goto _LL2347; _LL2347: _temp2346=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2229)->f2; goto _LL2345; _LL2345: _temp2344=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2229)->f3; goto _LL2262;} else{ goto _LL2263;} _LL2263: if(( unsigned int)
_temp2229 > 2u?*(( int*) _temp2229) == Cyc_Absyn_UnknownFields_p: 0){ _LL2355:
_temp2354=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2229)->f1; goto
_LL2353; _LL2353: _temp2352=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2229)->f2; goto _LL2351; _LL2351: _temp2350=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2229)->f3; goto _LL2264;} else{ goto _LL2230;} _LL2232: nv= Cyc_Toc_add_varmap(
nv, _temp2278, r); goto _LL2234; _LL2234: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2230; _LL2236: { struct _tuple0* _temp2356= Cyc_Toc_temp_var(); goto
_LL2357; _LL2357: decls=({ struct Cyc_List_List* _temp2358=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2358->hd=( void*)({
struct _tuple14* _temp2359=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2359->f1= _temp2356; _temp2359->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2359;}); _temp2358->tl= decls;
_temp2358;}); nv= Cyc_Toc_add_varmap( nv, _temp2282->name, Cyc_Absyn_var_exp(
_temp2356, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2356, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2230;} _LL2238: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2230; _LL2240: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2286, _temp2284, 0), succ_lab, fail_lab); goto _LL2230;
_LL2242: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2288, 0),
succ_lab, fail_lab); goto _LL2230; _LL2244: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2290, 0), succ_lab, fail_lab); goto _LL2230; _LL2246: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2360=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2360[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2361; _temp2361.tag= Cyc_Absyn_Enum_e;
_temp2361.f1= _temp2292->name; _temp2361.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2294; _temp2361.f3=( struct Cyc_Absyn_Enumfield*) _temp2292; _temp2361;});
_temp2360;}), 0), succ_lab, fail_lab); goto _LL2230; _LL2248: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2300->name, 0); if( ! _temp2302->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2230;} _LL2250: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2314);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2318->name,({
unsigned char* _temp2395=( unsigned char*)"_struct"; struct _tagged_string
_temp2396; _temp2396.curr= _temp2395; _temp2396.base= _temp2395; _temp2396.last_plus_one=
_temp2395 + 8; _temp2396;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp2362=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2318->typs); goto _LL2363;
_LL2363: { struct Cyc_List_List* _temp2364=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2314); goto
_LL2365; _LL2365: for( 0; _temp2364 != 0;((( _temp2364=(( struct Cyc_List_List*)
_check_null( _temp2364))->tl, _temp2362=(( struct Cyc_List_List*) _check_null(
_temp2362))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp2366=( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp2364))->hd; goto _LL2367; _LL2367: {
void* _temp2368=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2362))->hd)).f2; goto _LL2369; _LL2369: { struct _tuple0* _temp2370= Cyc_Toc_temp_var();
goto _LL2371; _LL2371: { void* _temp2372=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2366->topt))->v; goto _LL2373; _LL2373: { void* _temp2374= Cyc_Toc_typ_to_c(
_temp2372); goto _LL2375; _LL2375: { struct _tagged_string* _temp2376= Cyc_Toc_fresh_label();
goto _LL2377; _LL2377: { struct Cyc_Absyn_Exp* _temp2378= Cyc_Absyn_structarrow_exp(
rcast, Cyc_Toc_fieldname( cnt), 0); goto _LL2379; _LL2379: if( Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c( _temp2368))){ _temp2378= Cyc_Absyn_cast_exp( _temp2374,
_temp2378, 0);} decls=({ struct Cyc_List_List* _temp2380=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2380->hd=( void*)({
struct _tuple14* _temp2381=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2381->f1= _temp2370; _temp2381->f2= _temp2374; _temp2381;});
_temp2380->tl= decls; _temp2380;});{ struct _tuple13 _temp2382= Cyc_Toc_xlate_pat(
nv, rgn, _temp2372, Cyc_Absyn_var_exp( _temp2370, 0), _temp2378, _temp2366,
succ_lab, fail_lab, decls); goto _LL2383; _LL2383: nv= _temp2382.f1; decls=
_temp2382.f2;{ struct Cyc_Absyn_Stmt* _temp2384= _temp2382.f3; goto _LL2385;
_LL2385: { struct Cyc_Absyn_Stmt* _temp2386= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2370, 0), _temp2378, 0); goto _LL2387; _LL2387: { struct Cyc_Absyn_Stmt*
_temp2388= Cyc_Absyn_seq_stmt( _temp2386, _temp2384, 0); goto _LL2389; _LL2389:
ss=({ struct Cyc_List_List* _temp2390=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2390->hd=( void*) Cyc_Absyn_label_stmt(
_temp2376, _temp2388, 0); _temp2390->tl= ss; _temp2390;}); succ_lab= _temp2376;}}}}}}}}}}}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2320->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2318->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2391=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2320->fields))->v; goto _LL2392; _LL2392: for( 0; _temp2391 !=
0; _temp2391=(( struct Cyc_List_List*) _check_null( _temp2391))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2393=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2391))->hd; goto _LL2394; _LL2394: if( _temp2393->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2318->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2230;}}} _LL2252: _temp2330=
_temp2322; goto _LL2254; _LL2254: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2330);{ struct Cyc_List_List*
_temp2397=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2330); goto _LL2398; _LL2398: for( 0; _temp2397 !=
0;( _temp2397=(( struct Cyc_List_List*) _check_null( _temp2397))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2399=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2397))->hd; goto _LL2400; _LL2400: { struct _tuple0* _temp2401=
Cyc_Toc_temp_var(); goto _LL2402; _LL2402: { void* _temp2403=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2399->topt))->v; goto _LL2404; _LL2404: {
struct _tagged_string* _temp2405= Cyc_Toc_fresh_label(); goto _LL2406; _LL2406:
decls=({ struct Cyc_List_List* _temp2407=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2407->hd=( void*)({ struct _tuple14*
_temp2408=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2408->f1= _temp2401; _temp2408->f2= Cyc_Toc_typ_to_c( _temp2403); _temp2408;});
_temp2407->tl= decls; _temp2407;});{ struct _tuple13 _temp2409= Cyc_Toc_xlate_pat(
nv, rgn, _temp2403, Cyc_Absyn_var_exp( _temp2401, 0), Cyc_Absyn_structmember_exp(
path, Cyc_Toc_fieldname( cnt), 0), _temp2399, succ_lab, fail_lab, decls); goto
_LL2410; _LL2410: nv= _temp2409.f1; decls= _temp2409.f2;{ struct Cyc_Absyn_Stmt*
_temp2411= _temp2409.f3; goto _LL2412; _LL2412: { struct Cyc_Absyn_Stmt*
_temp2413= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2401, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2411, 0); goto _LL2414; _LL2414: ss=({ struct Cyc_List_List* _temp2415=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2415->hd=(
void*) Cyc_Absyn_label_stmt( _temp2405, _temp2413, 0); _temp2415->tl= ss;
_temp2415;}); succ_lab= _temp2405;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2230;} _LL2256: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2416=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2332); goto _LL2417; _LL2417: for( 0; _temp2416 !=
0; _temp2416=(( struct Cyc_List_List*) _check_null( _temp2416))->tl){ struct
_tuple15* _temp2418=( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2416))->hd; goto _LL2419; _LL2419: { struct Cyc_Absyn_Pat* _temp2420=(*
_temp2418).f2; goto _LL2421; _LL2421: { struct _tagged_string* f;{ void*
_temp2422=( void*)(( struct Cyc_List_List*) _check_null((* _temp2418).f1))->hd;
struct _tagged_string* _temp2428; _LL2424: if(*(( int*) _temp2422) == Cyc_Absyn_FieldName){
_LL2429: _temp2428=(( struct Cyc_Absyn_FieldName_struct*) _temp2422)->f1; goto
_LL2425;} else{ goto _LL2426;} _LL2426: goto _LL2427; _LL2425: f= _temp2428;
goto _LL2423; _LL2427:( void) _throw(( void*) Cyc_Toc_Match_error); _LL2423:;}{
struct _tuple0* _temp2430= Cyc_Toc_temp_var(); goto _LL2431; _LL2431: { void*
_temp2432=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2420->topt))->v;
goto _LL2433; _LL2433: { void* _temp2434= Cyc_Toc_typ_to_c( _temp2432); goto
_LL2435; _LL2435: { struct _tagged_string* _temp2436= Cyc_Toc_fresh_label();
goto _LL2437; _LL2437: decls=({ struct Cyc_List_List* _temp2438=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2438->hd=( void*)({
struct _tuple14* _temp2439=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2439->f1= _temp2430; _temp2439->f2= _temp2434; _temp2439;});
_temp2438->tl= decls; _temp2438;});{ struct _tuple13 _temp2440= Cyc_Toc_xlate_pat(
nv, rgn, _temp2432, Cyc_Absyn_var_exp( _temp2430, 0), Cyc_Absyn_structmember_exp(
path, f, 0), _temp2420, succ_lab, fail_lab, decls); goto _LL2441; _LL2441: nv=
_temp2440.f1; decls= _temp2440.f2;{ struct Cyc_Absyn_Exp* _temp2442= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2443; _LL2443: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2338->fields))->v, f)))->type)){ _temp2442= Cyc_Absyn_cast_exp(
_temp2434, _temp2442, 0);}{ struct Cyc_Absyn_Stmt* _temp2444= _temp2440.f3; goto
_LL2445; _LL2445: { struct Cyc_Absyn_Stmt* _temp2446= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2430, 0), _temp2442, 0), _temp2444, 0); goto _LL2447;
_LL2447: ss=({ struct Cyc_List_List* _temp2448=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2448->hd=( void*) Cyc_Absyn_label_stmt(
_temp2436, _temp2446, 0); _temp2448->tl= ss; _temp2448;}); succ_lab= _temp2436;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2230;} _LL2258: { struct _tuple0*
_temp2449= Cyc_Toc_temp_var(); goto _LL2450; _LL2450: { void* _temp2451=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2340->topt))->v; goto _LL2452; _LL2452:
decls=({ struct Cyc_List_List* _temp2453=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2453->hd=( void*)({ struct _tuple14*
_temp2454=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2454->f1= _temp2449; _temp2454->f2= Cyc_Toc_typ_to_c( _temp2451); _temp2454;});
_temp2453->tl= decls; _temp2453;});{ struct _tuple13 _temp2455= Cyc_Toc_xlate_pat(
nv, rgn, _temp2451, Cyc_Absyn_var_exp( _temp2449, 0), Cyc_Absyn_deref_exp( path,
0), _temp2340, succ_lab, fail_lab, decls); goto _LL2456; _LL2456: nv= _temp2455.f1;
decls= _temp2455.f2;{ struct Cyc_Absyn_Stmt* _temp2457= _temp2455.f3; goto
_LL2458; _LL2458: { struct Cyc_Absyn_Stmt* _temp2459= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2449, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2457, 0);
goto _LL2460; _LL2460: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2459, 0);} else{ s= _temp2459;} goto _LL2230;}}}}} _LL2260: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2461=( unsigned char*)"unknownid"; struct _tagged_string
_temp2462; _temp2462.curr= _temp2461; _temp2462.base= _temp2461; _temp2462.last_plus_one=
_temp2461 + 10; _temp2462;})); goto _LL2230; _LL2262: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2463=(
unsigned char*)"unknowncall"; struct _tagged_string _temp2464; _temp2464.curr=
_temp2463; _temp2464.base= _temp2463; _temp2464.last_plus_one= _temp2463 + 12;
_temp2464;})); goto _LL2230; _LL2264: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2465=(
unsigned char*)"unknownfields"; struct _tagged_string _temp2466; _temp2466.curr=
_temp2465; _temp2466.base= _temp2465; _temp2466.last_plus_one= _temp2465 + 14;
_temp2466;})); goto _LL2230; _LL2230:;} return({ struct _tuple13 _temp2467;
_temp2467.f1= nv; _temp2467.f2= decls; _temp2467.f3= s; _temp2467;});} struct
_tuple16{ struct _tagged_string* f1; struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple16* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple16* _temp2468=(
struct _tuple16*) _region_malloc( r, sizeof( struct _tuple16)); _temp2468->f1=
Cyc_Toc_fresh_label(); _temp2468->f2= Cyc_Toc_fresh_label(); _temp2468->f3= sc;
_temp2468;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp2469=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2470; _LL2470: { int leave_as_switch;{ void*
_temp2471= Cyc_Tcutil_compress( _temp2469); void* _temp2477; void* _temp2479;
_LL2473: if(( unsigned int) _temp2471 > 4u?*(( int*) _temp2471) == Cyc_Absyn_IntType:
0){ _LL2480: _temp2479=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2471)->f1;
goto _LL2478; _LL2478: _temp2477=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2471)->f2; goto _LL2474;} else{ goto _LL2475;} _LL2475: goto _LL2476;
_LL2474: leave_as_switch= 1; goto _LL2472; _LL2476: leave_as_switch= 0; goto
_LL2472; _LL2472:;}{ struct Cyc_List_List* _temp2481= scs; goto _LL2482; _LL2482:
for( 0; _temp2481 != 0; _temp2481=(( struct Cyc_List_List*) _check_null(
_temp2481))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2481))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2481))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2483= scs; goto
_LL2484; _LL2484: for( 0; _temp2483 != 0; _temp2483=(( struct Cyc_List_List*)
_check_null( _temp2483))->tl){ struct Cyc_Absyn_Stmt* _temp2485=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2483))->hd)->body; goto _LL2486;
_LL2486:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2483))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2485, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2485);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2487=
_new_region(); struct _RegionHandle* rgn=& _temp2487; _push_region( rgn);{
struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2488= lscs; goto
_LL2489; _LL2489: for( 0; _temp2488 != 0; _temp2488=(( struct Cyc_List_List*)
_check_null( _temp2488))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct
_tuple16*)(( struct Cyc_List_List*) _check_null( _temp2488))->hd)).f3; struct
_tagged_string* fail_lab=(( struct Cyc_List_List*) _check_null( _temp2488))->tl
== 0? end_l:(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp2488))->tl))->hd)).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2488))->hd)).f2; if( sc->where_clause == 0){ struct _tuple13
_temp2490= Cyc_Toc_xlate_pat( nv, rgn, _temp2469, r, path, sc->pattern, succ_lab,
fail_lab, decls); goto _LL2491; _LL2491: nvs=({ struct Cyc_List_List* _temp2492=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2492->hd=( void*) _temp2490.f1; _temp2492->tl= nvs; _temp2492;}); decls=
_temp2490.f2; test_stmts=({ struct Cyc_List_List* _temp2493=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2493->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2488))->hd)).f1,
_temp2490.f3, 0); _temp2493->tl= test_stmts; _temp2493;});} else{ struct Cyc_Absyn_Exp*
_temp2494=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2495;
_LL2495: { struct _tagged_string* _temp2496= Cyc_Toc_fresh_label(); goto _LL2497;
_LL2497: { struct _tuple13 _temp2498= Cyc_Toc_xlate_pat( nv, rgn, _temp2469, r,
path, sc->pattern, _temp2496, fail_lab, decls); goto _LL2499; _LL2499: Cyc_Toc_exp_to_c(
_temp2498.f1, _temp2494); nvs=({ struct Cyc_List_List* _temp2500=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2500->hd=( void*)
_temp2498.f1; _temp2500->tl= nvs; _temp2500;}); decls= _temp2498.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2494, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2496, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2501=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2501->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2488))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2498.f3, s3, 0), 0); _temp2501->tl= test_stmts;
_temp2501;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2502= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2503; _LL2503: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2504=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2505; _LL2505: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2504, end_l,(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2506=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2507; _LL2507: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2506, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2502, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple14 _temp2510;
void* _temp2511; struct _tuple0* _temp2513; struct _tuple14* _temp2508=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2510=*
_temp2508; _LL2514: _temp2513= _temp2510.f1; goto _LL2512; _LL2512: _temp2511=
_temp2510.f2; goto _LL2509; _LL2509: res= Cyc_Absyn_declare_stmt( _temp2513,
_temp2511, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region(& _temp2487);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2515=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2515[ 0]=({ struct Cyc_List_List _temp2516; _temp2516.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2516.tl= 0; _temp2516;}); _temp2515;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2517=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2559; struct Cyc_Absyn_Stmt* _temp2561; struct Cyc_Absyn_Stmt*
_temp2563; struct Cyc_Absyn_Exp* _temp2565; struct Cyc_Absyn_Stmt* _temp2567;
struct Cyc_Absyn_Stmt* _temp2569; struct Cyc_Absyn_Exp* _temp2571; struct Cyc_Absyn_Stmt*
_temp2573; struct _tuple2 _temp2575; struct Cyc_Absyn_Stmt* _temp2577; struct
Cyc_Absyn_Exp* _temp2579; struct Cyc_Absyn_Stmt* _temp2581; struct Cyc_Absyn_Stmt*
_temp2583; struct Cyc_Absyn_Stmt* _temp2585; struct _tagged_string* _temp2587;
struct Cyc_Absyn_Stmt* _temp2589; struct _tuple2 _temp2591; struct Cyc_Absyn_Stmt*
_temp2593; struct Cyc_Absyn_Exp* _temp2595; struct _tuple2 _temp2597; struct Cyc_Absyn_Stmt*
_temp2599; struct Cyc_Absyn_Exp* _temp2601; struct Cyc_Absyn_Exp* _temp2603;
struct Cyc_List_List* _temp2605; struct Cyc_Absyn_Exp* _temp2607; struct Cyc_Absyn_Switch_clause**
_temp2609; struct Cyc_List_List* _temp2611; struct Cyc_Absyn_Stmt* _temp2613;
struct Cyc_Absyn_Decl* _temp2615; struct Cyc_Absyn_Stmt* _temp2617; struct
_tagged_string* _temp2619; struct _tuple2 _temp2621; struct Cyc_Absyn_Stmt*
_temp2623; struct Cyc_Absyn_Exp* _temp2625; struct Cyc_Absyn_Stmt* _temp2627;
struct Cyc_List_List* _temp2629; struct Cyc_Absyn_Stmt* _temp2631; struct Cyc_Absyn_Stmt*
_temp2633; struct Cyc_Absyn_Vardecl* _temp2635; struct Cyc_Absyn_Tvar* _temp2637;
struct Cyc_List_List* _temp2639; struct Cyc_Absyn_Exp* _temp2641; struct Cyc_Absyn_Stmt*
_temp2643; struct Cyc_Absyn_Stmt* _temp2645; _LL2519: if( _temp2517 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2520;} else{ goto _LL2521;} _LL2521: if((
unsigned int) _temp2517 > 1u?*(( int*) _temp2517) == Cyc_Absyn_Exp_s: 0){
_LL2560: _temp2559=(( struct Cyc_Absyn_Exp_s_struct*) _temp2517)->f1; goto
_LL2522;} else{ goto _LL2523;} _LL2523: if(( unsigned int) _temp2517 > 1u?*((
int*) _temp2517) == Cyc_Absyn_Seq_s: 0){ _LL2564: _temp2563=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2517)->f1; goto _LL2562; _LL2562: _temp2561=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2517)->f2; goto _LL2524;} else{ goto _LL2525;} _LL2525: if(( unsigned int)
_temp2517 > 1u?*(( int*) _temp2517) == Cyc_Absyn_Return_s: 0){ _LL2566:
_temp2565=(( struct Cyc_Absyn_Return_s_struct*) _temp2517)->f1; goto _LL2526;}
else{ goto _LL2527;} _LL2527: if(( unsigned int) _temp2517 > 1u?*(( int*)
_temp2517) == Cyc_Absyn_IfThenElse_s: 0){ _LL2572: _temp2571=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2517)->f1; goto _LL2570; _LL2570: _temp2569=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2517)->f2; goto _LL2568; _LL2568: _temp2567=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2517)->f3; goto _LL2528;} else{ goto _LL2529;} _LL2529: if(( unsigned int)
_temp2517 > 1u?*(( int*) _temp2517) == Cyc_Absyn_While_s: 0){ _LL2576: _temp2575=((
struct Cyc_Absyn_While_s_struct*) _temp2517)->f1; _LL2580: _temp2579= _temp2575.f1;
goto _LL2578; _LL2578: _temp2577= _temp2575.f2; goto _LL2574; _LL2574: _temp2573=((
struct Cyc_Absyn_While_s_struct*) _temp2517)->f2; goto _LL2530;} else{ goto
_LL2531;} _LL2531: if(( unsigned int) _temp2517 > 1u?*(( int*) _temp2517) == Cyc_Absyn_Break_s:
0){ _LL2582: _temp2581=(( struct Cyc_Absyn_Break_s_struct*) _temp2517)->f1; goto
_LL2532;} else{ goto _LL2533;} _LL2533: if(( unsigned int) _temp2517 > 1u?*((
int*) _temp2517) == Cyc_Absyn_Continue_s: 0){ _LL2584: _temp2583=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2517)->f1; goto _LL2534;} else{ goto _LL2535;} _LL2535: if(( unsigned int)
_temp2517 > 1u?*(( int*) _temp2517) == Cyc_Absyn_Goto_s: 0){ _LL2588: _temp2587=((
struct Cyc_Absyn_Goto_s_struct*) _temp2517)->f1; goto _LL2586; _LL2586:
_temp2585=(( struct Cyc_Absyn_Goto_s_struct*) _temp2517)->f2; goto _LL2536;}
else{ goto _LL2537;} _LL2537: if(( unsigned int) _temp2517 > 1u?*(( int*)
_temp2517) == Cyc_Absyn_For_s: 0){ _LL2604: _temp2603=(( struct Cyc_Absyn_For_s_struct*)
_temp2517)->f1; goto _LL2598; _LL2598: _temp2597=(( struct Cyc_Absyn_For_s_struct*)
_temp2517)->f2; _LL2602: _temp2601= _temp2597.f1; goto _LL2600; _LL2600:
_temp2599= _temp2597.f2; goto _LL2592; _LL2592: _temp2591=(( struct Cyc_Absyn_For_s_struct*)
_temp2517)->f3; _LL2596: _temp2595= _temp2591.f1; goto _LL2594; _LL2594:
_temp2593= _temp2591.f2; goto _LL2590; _LL2590: _temp2589=(( struct Cyc_Absyn_For_s_struct*)
_temp2517)->f4; goto _LL2538;} else{ goto _LL2539;} _LL2539: if(( unsigned int)
_temp2517 > 1u?*(( int*) _temp2517) == Cyc_Absyn_Switch_s: 0){ _LL2608:
_temp2607=(( struct Cyc_Absyn_Switch_s_struct*) _temp2517)->f1; goto _LL2606;
_LL2606: _temp2605=(( struct Cyc_Absyn_Switch_s_struct*) _temp2517)->f2; goto
_LL2540;} else{ goto _LL2541;} _LL2541: if(( unsigned int) _temp2517 > 1u?*((
int*) _temp2517) == Cyc_Absyn_Fallthru_s: 0){ _LL2612: _temp2611=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2517)->f1; goto _LL2610; _LL2610: _temp2609=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2517)->f2; goto _LL2542;} else{ goto _LL2543;} _LL2543: if(( unsigned int)
_temp2517 > 1u?*(( int*) _temp2517) == Cyc_Absyn_Decl_s: 0){ _LL2616: _temp2615=((
struct Cyc_Absyn_Decl_s_struct*) _temp2517)->f1; goto _LL2614; _LL2614:
_temp2613=(( struct Cyc_Absyn_Decl_s_struct*) _temp2517)->f2; goto _LL2544;}
else{ goto _LL2545;} _LL2545: if(( unsigned int) _temp2517 > 1u?*(( int*)
_temp2517) == Cyc_Absyn_Label_s: 0){ _LL2620: _temp2619=(( struct Cyc_Absyn_Label_s_struct*)
_temp2517)->f1; goto _LL2618; _LL2618: _temp2617=(( struct Cyc_Absyn_Label_s_struct*)
_temp2517)->f2; goto _LL2546;} else{ goto _LL2547;} _LL2547: if(( unsigned int)
_temp2517 > 1u?*(( int*) _temp2517) == Cyc_Absyn_Do_s: 0){ _LL2628: _temp2627=((
struct Cyc_Absyn_Do_s_struct*) _temp2517)->f1; goto _LL2622; _LL2622: _temp2621=((
struct Cyc_Absyn_Do_s_struct*) _temp2517)->f2; _LL2626: _temp2625= _temp2621.f1;
goto _LL2624; _LL2624: _temp2623= _temp2621.f2; goto _LL2548;} else{ goto
_LL2549;} _LL2549: if(( unsigned int) _temp2517 > 1u?*(( int*) _temp2517) == Cyc_Absyn_TryCatch_s:
0){ _LL2632: _temp2631=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2517)->f1;
goto _LL2630; _LL2630: _temp2629=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2517)->f2; goto _LL2550;} else{ goto _LL2551;} _LL2551: if(( unsigned int)
_temp2517 > 1u?*(( int*) _temp2517) == Cyc_Absyn_Region_s: 0){ _LL2638:
_temp2637=(( struct Cyc_Absyn_Region_s_struct*) _temp2517)->f1; goto _LL2636;
_LL2636: _temp2635=(( struct Cyc_Absyn_Region_s_struct*) _temp2517)->f2; goto
_LL2634; _LL2634: _temp2633=(( struct Cyc_Absyn_Region_s_struct*) _temp2517)->f3;
goto _LL2552;} else{ goto _LL2553;} _LL2553: if(( unsigned int) _temp2517 > 1u?*((
int*) _temp2517) == Cyc_Absyn_SwitchC_s: 0){ _LL2642: _temp2641=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2517)->f1; goto _LL2640; _LL2640: _temp2639=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2517)->f2; goto _LL2554;} else{ goto _LL2555;} _LL2555: if(( unsigned int)
_temp2517 > 1u?*(( int*) _temp2517) == Cyc_Absyn_Cut_s: 0){ _LL2644: _temp2643=((
struct Cyc_Absyn_Cut_s_struct*) _temp2517)->f1; goto _LL2556;} else{ goto
_LL2557;} _LL2557: if(( unsigned int) _temp2517 > 1u?*(( int*) _temp2517) == Cyc_Absyn_Splice_s:
0){ _LL2646: _temp2645=(( struct Cyc_Absyn_Splice_s_struct*) _temp2517)->f1;
goto _LL2558;} else{ goto _LL2518;} _LL2520: return; _LL2522: Cyc_Toc_exp_to_c(
nv, _temp2559); return; _LL2524: Cyc_Toc_stmt_to_c( nv, _temp2563); s= _temp2561;
continue; _LL2526: { struct Cyc_Core_Opt* topt= 0; if( _temp2565 != 0){ topt=({
struct Cyc_Core_Opt* _temp2647=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2647->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2565))->topt))->v);
_temp2647;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2565));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2648=
Cyc_Toc_temp_var(); goto _LL2649; _LL2649: { struct Cyc_Absyn_Stmt* _temp2650=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2648, 0),
0); goto _LL2651; _LL2651:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2648,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2565,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2650, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2528: Cyc_Toc_exp_to_c(
nv, _temp2571); Cyc_Toc_stmt_to_c( nv, _temp2569); s= _temp2567; continue;
_LL2530: Cyc_Toc_exp_to_c( nv, _temp2579); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2573); return; _LL2532: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2581 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2581))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2534: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -(( struct Cyc_Absyn_Stmt*) _check_null( _temp2583))->try_depth, s);
return; _LL2536: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2585))->try_depth, s); return; _LL2538: Cyc_Toc_exp_to_c( nv,
_temp2603); Cyc_Toc_exp_to_c( nv, _temp2601); Cyc_Toc_exp_to_c( nv, _temp2595);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2589); return; _LL2540: Cyc_Toc_xlate_switch(
nv, s, _temp2607, _temp2605); return; _LL2542: if( nv->fallthru_info == 0){(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2652=( unsigned char*)"fallthru in unexpected place"; struct
_tagged_string _temp2653; _temp2653.curr= _temp2652; _temp2653.base= _temp2652;
_temp2653.last_plus_one= _temp2652 + 29; _temp2653;})));}{ struct _tuple8
_temp2656; struct Cyc_Dict_Dict* _temp2657; struct Cyc_List_List* _temp2659;
struct _tagged_string* _temp2661; struct _tuple8* _temp2654=( struct _tuple8*)((
struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp2656=* _temp2654;
_LL2662: _temp2661= _temp2656.f1; goto _LL2660; _LL2660: _temp2659= _temp2656.f2;
goto _LL2658; _LL2658: _temp2657= _temp2656.f3; goto _LL2655; _LL2655: { struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2661, 0); Cyc_Toc_do_npop_before(
s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp2609)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp2663=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2659); goto _LL2664; _LL2664: { struct Cyc_List_List*
_temp2665=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2611); goto _LL2666; _LL2666: for( 0; _temp2663 != 0;( _temp2663=(( struct
Cyc_List_List*) _check_null( _temp2663))->tl, _temp2665=(( struct Cyc_List_List*)
_check_null( _temp2665))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp2665))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp2657,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp2663))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp2665))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}}} _LL2544:{ void* _temp2667=( void*) _temp2615->r; int
_temp2677; struct Cyc_Absyn_Exp* _temp2679; struct Cyc_Core_Opt* _temp2681;
struct Cyc_Core_Opt* _temp2683; struct Cyc_Absyn_Pat* _temp2685; struct Cyc_List_List*
_temp2687; struct Cyc_Absyn_Vardecl* _temp2689; _LL2669: if(*(( int*) _temp2667)
== Cyc_Absyn_Let_d){ _LL2686: _temp2685=(( struct Cyc_Absyn_Let_d_struct*)
_temp2667)->f1; goto _LL2684; _LL2684: _temp2683=(( struct Cyc_Absyn_Let_d_struct*)
_temp2667)->f2; goto _LL2682; _LL2682: _temp2681=(( struct Cyc_Absyn_Let_d_struct*)
_temp2667)->f3; goto _LL2680; _LL2680: _temp2679=(( struct Cyc_Absyn_Let_d_struct*)
_temp2667)->f4; goto _LL2678; _LL2678: _temp2677=(( struct Cyc_Absyn_Let_d_struct*)
_temp2667)->f5; goto _LL2670;} else{ goto _LL2671;} _LL2671: if(*(( int*)
_temp2667) == Cyc_Absyn_Letv_d){ _LL2688: _temp2687=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2667)->f1; goto _LL2672;} else{ goto _LL2673;} _LL2673: if(*(( int*)
_temp2667) == Cyc_Absyn_Var_d){ _LL2690: _temp2689=(( struct Cyc_Absyn_Var_d_struct*)
_temp2667)->f1; goto _LL2674;} else{ goto _LL2675;} _LL2675: goto _LL2676;
_LL2670:( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2685,
_temp2681, _temp2679, _temp2677, _temp2613))->r)); goto _LL2668; _LL2672: {
struct Cyc_List_List* _temp2691=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2687); goto _LL2692; _LL2692: if( _temp2691 == 0){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2693=(
unsigned char*)"empty Letv_d"; struct _tagged_string _temp2694; _temp2694.curr=
_temp2693; _temp2694.base= _temp2693; _temp2694.last_plus_one= _temp2693 + 13;
_temp2694;}));}( void*)( _temp2615->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2695=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2695[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2696; _temp2696.tag= Cyc_Absyn_Var_d;
_temp2696.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2691))->hd; _temp2696;}); _temp2695;}))); _temp2691=(( struct Cyc_List_List*)
_check_null( _temp2691))->tl; for( 0; _temp2691 != 0; _temp2691=(( struct Cyc_List_List*)
_check_null( _temp2691))->tl){ struct Cyc_Absyn_Decl* _temp2699= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2697=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2697[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2698; _temp2698.tag= Cyc_Absyn_Var_d; _temp2698.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2691))->hd; _temp2698;}); _temp2697;}),
0); goto _LL2700; _LL2700:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp2699, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c(
nv, s); goto _LL2668;} _LL2674: { struct _tuple0* _temp2701= _temp2689->name;
goto _LL2702; _LL2702:( void*)( _temp2689->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2689->type));{ struct Cyc_Toc_Env* _temp2705= Cyc_Toc_add_varmap( nv,
_temp2701, Cyc_Absyn_varb_exp( _temp2701,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2703=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2703[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2704; _temp2704.tag= Cyc_Absyn_Local_b;
_temp2704.f1= _temp2689; _temp2704;}); _temp2703;}), 0)); goto _LL2706; _LL2706:
Cyc_Toc_stmt_to_c( _temp2705, _temp2613); if( _temp2689->initializer != 0){
struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*) _check_null( _temp2689->initializer);
void* _temp2707=( void*) init->r; struct Cyc_Absyn_Exp* _temp2713; struct Cyc_Absyn_Exp*
_temp2715; struct Cyc_Absyn_Vardecl* _temp2717; _LL2709: if(*(( int*) _temp2707)
== Cyc_Absyn_Comprehension_e){ _LL2718: _temp2717=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2707)->f1; goto _LL2716; _LL2716: _temp2715=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2707)->f2; goto _LL2714; _LL2714: _temp2713=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2707)->f3; goto _LL2710;} else{ goto _LL2711;} _LL2711: goto _LL2712;
_LL2710: _temp2689->initializer= 0;( void*)( _temp2613->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2701, 0), _temp2717, _temp2715, _temp2713, Cyc_Absyn_new_stmt((
void*) _temp2613->r, 0), 0))->r)); goto _LL2708; _LL2712: Cyc_Toc_exp_to_c( nv,
init); goto _LL2708; _LL2708:;} goto _LL2668;}} _LL2676:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2719=( unsigned char*)"bad nested declaration within function";
struct _tagged_string _temp2720; _temp2720.curr= _temp2719; _temp2720.base=
_temp2719; _temp2720.last_plus_one= _temp2719 + 39; _temp2720;})); goto _LL2668;
_LL2668:;} return; _LL2546: s= _temp2617; continue; _LL2548: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp2627); Cyc_Toc_exp_to_c( nv, _temp2625); return;
_LL2550: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__push_handler_qv,
0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp( Cyc_Toc_setjmp_qv, 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__pop_handler_qv,
0); struct Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp( Cyc_Toc__exn_thrown_qv,
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2721=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2721->v=( void*) e_typ; _temp2721;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2631);{
struct Cyc_Absyn_Stmt* _temp2722= Cyc_Absyn_seq_stmt( _temp2631, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); goto _LL2723; _LL2723: {
struct _tuple0* _temp2724= Cyc_Toc_temp_var(); goto _LL2725; _LL2725: { struct
Cyc_Absyn_Exp* _temp2726= Cyc_Absyn_var_exp( _temp2724, 0); goto _LL2727;
_LL2727: { struct Cyc_Absyn_Vardecl* _temp2728= Cyc_Absyn_new_vardecl( _temp2724,
Cyc_Absyn_exn_typ, 0); goto _LL2729; _LL2729: _temp2726->topt=({ struct Cyc_Core_Opt*
_temp2730=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2730->v=( void*) Cyc_Absyn_exn_typ; _temp2730;});{ struct Cyc_Absyn_Pat*
_temp2735=({ struct Cyc_Absyn_Pat* _temp2731=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2731->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2733=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2733[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2734; _temp2734.tag= Cyc_Absyn_Var_p;
_temp2734.f1= _temp2728; _temp2734;}); _temp2733;})); _temp2731->topt=({ struct
Cyc_Core_Opt* _temp2732=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2732->v=( void*) Cyc_Absyn_exn_typ; _temp2732;}); _temp2731->loc= 0;
_temp2731;}); goto _LL2736; _LL2736: { struct Cyc_Absyn_Exp* _temp2737= Cyc_Absyn_throw_exp(
_temp2726, 0); goto _LL2738; _LL2738: _temp2737->topt=({ struct Cyc_Core_Opt*
_temp2739=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2739->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2739;});{ struct Cyc_Absyn_Stmt*
_temp2740= Cyc_Absyn_exp_stmt( _temp2737, 0); goto _LL2741; _LL2741: { struct
Cyc_Absyn_Switch_clause* _temp2745=({ struct Cyc_Absyn_Switch_clause* _temp2742=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2742->pattern= _temp2735; _temp2742->pat_vars=({ struct Cyc_Core_Opt*
_temp2743=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2743->v=( void*)({ struct Cyc_List_List* _temp2744=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2744->hd=( void*) _temp2728;
_temp2744->tl= 0; _temp2744;}); _temp2743;}); _temp2742->where_clause= 0;
_temp2742->body= _temp2740; _temp2742->loc= 0; _temp2742;}); goto _LL2746;
_LL2746: { struct Cyc_Absyn_Stmt* _temp2748= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2629,({ struct Cyc_List_List* _temp2747=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2747->hd=( void*) _temp2745; _temp2747->tl=
0; _temp2747;})), 0); goto _LL2749; _LL2749: Cyc_Toc_stmt_to_c( nv, _temp2748);{
struct Cyc_Absyn_Exp* _temp2751= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2750=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2750->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2750->tl= 0; _temp2750;}), 0); goto _LL2752; _LL2752: { struct Cyc_Absyn_Stmt*
_temp2754= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2753=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2753->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2753->tl= 0;
_temp2753;}), 0), 0); goto _LL2755; _LL2755: { struct Cyc_Absyn_Exp* _temp2756=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2757; _LL2757: {
struct Cyc_Absyn_Exp* _temp2758= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2759; _LL2759:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2754, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2756, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2751, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2758, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2722, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2748, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2552: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2635->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2765=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2765[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2766; _temp2766.tag= Cyc_Absyn_Local_b; _temp2766.f1=
_temp2635; _temp2766;}); _temp2765;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2764=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2764->f1= Cyc_Toc_abs_ns; _temp2764->f2= Cyc_Toc__push_region_sp;
_temp2764;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2763=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2763->f1= Cyc_Toc_abs_ns; _temp2763->f2= Cyc_Toc__pop_region_sp; _temp2763;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2762=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2762->f1=
Cyc_Toc_abs_ns; _temp2762->f2= Cyc_Toc__new_region_sp; _temp2762;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2761=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2761->hd=( void*) addr_rh_exp; _temp2761->tl= 0; _temp2761;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2760=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2760->hd=( void*) x_exp; _temp2760->tl= 0; _temp2760;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2633);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2633, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2554:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2767=(
unsigned char*)"switchC_s"; struct _tagged_string _temp2768; _temp2768.curr=
_temp2767; _temp2768.base= _temp2767; _temp2768.last_plus_one= _temp2767 + 10;
_temp2768;})); return; _LL2556:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2769=( unsigned char*)"cut"; struct _tagged_string _temp2770;
_temp2770.curr= _temp2769; _temp2770.base= _temp2769; _temp2770.last_plus_one=
_temp2769 + 4; _temp2770;})); return; _LL2558:(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(({ unsigned char* _temp2771=( unsigned char*)"splice"; struct
_tagged_string _temp2772; _temp2772.curr= _temp2771; _temp2772.base= _temp2771;
_temp2772.last_plus_one= _temp2771 + 7; _temp2772;})); return; _LL2518:;}}
struct _tuple17{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp2773= f->args; goto _LL2774;
_LL2774: for( 0; _temp2773 != 0; _temp2773=(( struct Cyc_List_List*) _check_null(
_temp2773))->tl){(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2773))->hd)).f3= Cyc_Toc_typ_to_c((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp2773))->hd)).f3);{ struct _tuple0* _temp2776=({ struct _tuple0*
_temp2775=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2775->f1=(
void*) Cyc_Absyn_Loc_n; _temp2775->f2=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp2773))->hd)).f1; _temp2775;}); goto _LL2777; _LL2777: nv= Cyc_Toc_add_varmap(
nv, _temp2776, Cyc_Absyn_var_exp( _temp2776, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2780; void* _temp2782; void* _temp2784; struct Cyc_Absyn_Tqual _temp2786;
struct Cyc_Core_Opt* _temp2788; struct Cyc_Absyn_VarargInfo _temp2778=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2789: _temp2788=
_temp2778.name; goto _LL2787; _LL2787: _temp2786= _temp2778.tq; goto _LL2785;
_LL2785: _temp2784=( void*) _temp2778.type; goto _LL2783; _LL2783: _temp2782=(
void*) _temp2778.rgn; goto _LL2781; _LL2781: _temp2780= _temp2778.inject; goto
_LL2779; _LL2779: { void* _temp2790= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2784, _temp2782, _temp2786)); goto _LL2791; _LL2791: { struct _tuple0*
_temp2793=({ struct _tuple0* _temp2792=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2792->f1=( void*) Cyc_Absyn_Loc_n; _temp2792->f2=( struct
_tagged_string*)(( struct Cyc_Core_Opt*) _check_null( _temp2788))->v; _temp2792;});
goto _LL2794; _LL2794: f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2795=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2795->hd=( void*)({ struct _tuple17* _temp2796=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp2796->f1=( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( _temp2788))->v; _temp2796->f2= _temp2786;
_temp2796->f3= _temp2790; _temp2796;}); _temp2795->tl= 0; _temp2795;})); nv= Cyc_Toc_add_varmap(
nv, _temp2793, Cyc_Absyn_var_exp( _temp2793, 0)); f->cyc_varargs= 0;}}}{ struct
Cyc_List_List* _temp2797=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; goto _LL2798; _LL2798: for( 0; _temp2797 !=
0; _temp2797=(( struct Cyc_List_List*) _check_null( _temp2797))->tl){( void*)(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp2797))->hd)->type=(
void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp2797))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2799= s;
_LL2801: if( _temp2799 ==( void*) Cyc_Absyn_Abstract){ goto _LL2802;} else{ goto
_LL2803;} _LL2803: if( _temp2799 ==( void*) Cyc_Absyn_ExternC){ goto _LL2804;}
else{ goto _LL2805;} _LL2805: goto _LL2806; _LL2802: return( void*) Cyc_Absyn_Public;
_LL2804: return( void*) Cyc_Absyn_Extern; _LL2806: return s; _LL2800:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2807=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2808; _LL2808: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2809=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2809->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2809;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2810=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2807).f2); goto _LL2811;
_LL2811: if( _temp2810 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2807).f2, s));}
else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null(
_temp2810))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2807).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2812=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2812->v=( void*)
_temp2807; _temp2812;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2813=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2814; _LL2814: for( 0; _temp2813 != 0;
_temp2813=(( struct Cyc_List_List*) _check_null( _temp2813))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2813))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2813))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2815=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2816; _LL2816: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2817=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2817->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2817;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2818=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2815).f2); goto _LL2819; _LL2819:
if( _temp2818 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2815).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2818))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2815).f2, u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)(
u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==(
void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*)
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2820=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2820->v=( void*) _temp2815; _temp2820;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2821=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( u->fields))->v; goto
_LL2822; _LL2822: for( 0; _temp2821 != 0; _temp2821=(( struct Cyc_List_List*)
_check_null( _temp2821))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2821))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2821))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2823=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2823->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2823;});}{ struct
_tuple0* _temp2824= tud->name; goto _LL2825; _LL2825: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2824)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2824));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2826=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2827; _LL2827: for( 0;
_temp2826 != 0; _temp2826=(( struct Cyc_List_List*) _check_null( _temp2826))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2826))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2828; _temp2828.q_const= 1; _temp2828.q_volatile= 0;
_temp2828.q_restrict= 0; _temp2828;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2829=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2829->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2830=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2830[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2831; _temp2831.tag= Cyc_Absyn_Var_d;
_temp2831.f1= vd; _temp2831;}); _temp2830;}), 0); _temp2829->tl= Cyc_Toc_result_decls;
_temp2829;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2832; _temp2832.q_const=
1; _temp2832.q_volatile= 0; _temp2832.q_restrict= 0; _temp2832;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2833=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2833->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2834=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2834[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2835; _temp2835.tag= Cyc_Absyn_Var_d; _temp2835.f1= vd; _temp2835;});
_temp2834;}), 0); _temp2833->tl= Cyc_Toc_result_decls; _temp2833;});{ struct Cyc_List_List*
_temp2836= 0; goto _LL2837; _LL2837: { int i= 1;{ struct Cyc_List_List*
_temp2838= f->typs; goto _LL2839; _LL2839: for( 0; _temp2838 != 0;( _temp2838=((
struct Cyc_List_List*) _check_null( _temp2838))->tl, i ++)){ struct
_tagged_string* _temp2840= Cyc_Toc_fieldname( i); goto _LL2841; _LL2841: {
struct Cyc_Absyn_Structfield* _temp2843=({ struct Cyc_Absyn_Structfield*
_temp2842=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2842->name= _temp2840; _temp2842->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2838))->hd)).f1; _temp2842->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2838))->hd)).f2);
_temp2842->width= 0; _temp2842->attributes= 0; _temp2842;}); goto _LL2844;
_LL2844: _temp2836=({ struct Cyc_List_List* _temp2845=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2845->hd=( void*) _temp2843;
_temp2845->tl= _temp2836; _temp2845;});}}} _temp2836=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2836); _temp2836=({ struct Cyc_List_List*
_temp2846=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2846->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2847=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2847->name= Cyc_Toc_tag_sp;
_temp2847->tq= Cyc_Toc_mt_tq; _temp2847->type=( void*) Cyc_Absyn_sint_t;
_temp2847->width= 0; _temp2847->attributes= 0; _temp2847;}); _temp2846->tl=
_temp2836; _temp2846;});{ struct Cyc_Absyn_Structdecl* _temp2853=({ struct Cyc_Absyn_Structdecl*
_temp2848=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2848->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2848->name=({ struct Cyc_Core_Opt*
_temp2850=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2850->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2851=( unsigned char*)"_struct"; struct _tagged_string _temp2852; _temp2852.curr=
_temp2851; _temp2852.base= _temp2851; _temp2852.last_plus_one= _temp2851 + 8;
_temp2852;})); _temp2850;}); _temp2848->tvs= 0; _temp2848->fields=({ struct Cyc_Core_Opt*
_temp2849=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2849->v=( void*) _temp2836; _temp2849;}); _temp2848->attributes= 0;
_temp2848;}); goto _LL2854; _LL2854: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2855=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2855->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2856=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2856[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2857; _temp2857.tag= Cyc_Absyn_Struct_d;
_temp2857.f1= _temp2853; _temp2857;}); _temp2856;}), 0); _temp2855->tl= Cyc_Toc_result_decls;
_temp2855;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2858=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2858->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2858;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2859= xd->name; goto _LL2860; _LL2860: {
struct Cyc_List_List* _temp2861=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2862; _LL2862: for( 0; _temp2861 != 0;
_temp2861=(( struct Cyc_List_List*) _check_null( _temp2861))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2861))->hd; struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2864= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2863=* fn;(
unsigned int)( _temp2863.last_plus_one - _temp2863.curr);}) + 4, 0); goto
_LL2865; _LL2865: { struct Cyc_Absyn_ArrayType_struct* _temp2868=({ struct Cyc_Absyn_ArrayType_struct*
_temp2866=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2866[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2867; _temp2867.tag= Cyc_Absyn_ArrayType;
_temp2867.f1=( void*) Cyc_Absyn_uchar_t; _temp2867.f2= Cyc_Toc_mt_tq; _temp2867.f3=(
struct Cyc_Absyn_Exp*) _temp2864; _temp2867;}); _temp2866;}); goto _LL2869;
_LL2869: { struct Cyc_Core_Opt* _temp2870=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2878; int _temp2879; _LL2872: if( _temp2870 == 0){ goto
_LL2873;} else{ goto _LL2874;} _LL2874: if( _temp2870 == 0){ goto _LL2876;}
else{ _temp2878=* _temp2870; _LL2880: _temp2879=( int) _temp2878.v; if(
_temp2879 == 0){ goto _LL2875;} else{ goto _LL2876;}} _LL2876: goto _LL2877;
_LL2873: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_string)({
int _temp2881= 0; int _temp2882= 0; int _temp2883= 0; int _temp2884= 0; struct
_tagged_string _temp2885=* fn; xprintf("%c%c%c%c%.*s", _temp2881, _temp2882,
_temp2883, _temp2884, _temp2885.last_plus_one - _temp2885.curr, _temp2885.curr);}),
0);}{ struct Cyc_Absyn_Vardecl* _temp2886= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2868, initopt); goto _LL2887; _LL2887:( void*)( _temp2886->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2888=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2888->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2889=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2889[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2890; _temp2890.tag= Cyc_Absyn_Var_d;
_temp2890.f1= _temp2886; _temp2890;}); _temp2889;}), 0); _temp2888->tl= Cyc_Toc_result_decls;
_temp2888;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2891= f->typs; goto _LL2892; _LL2892: for( 0;
_temp2891 != 0;( _temp2891=(( struct Cyc_List_List*) _check_null( _temp2891))->tl,
i ++)){ struct _tagged_string* _temp2894=({ struct _tagged_string* _temp2893=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp2893[ 0]=(
struct _tagged_string) xprintf("f%d", i); _temp2893;}); goto _LL2895; _LL2895: {
struct Cyc_Absyn_Structfield* _temp2897=({ struct Cyc_Absyn_Structfield*
_temp2896=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2896->name= _temp2894; _temp2896->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2891))->hd)).f1; _temp2896->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2891))->hd)).f2);
_temp2896->width= 0; _temp2896->attributes= 0; _temp2896;}); goto _LL2898;
_LL2898: fields=({ struct Cyc_List_List* _temp2899=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2899->hd=( void*) _temp2897;
_temp2899->tl= fields; _temp2899;});}}} fields=({ struct Cyc_List_List*
_temp2900=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2900->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2901=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2901->name= Cyc_Toc_tag_sp;
_temp2901->tq= Cyc_Toc_mt_tq; _temp2901->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2901->width= 0; _temp2901->attributes= 0; _temp2901;});
_temp2900->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2900;});{ struct Cyc_Absyn_Structdecl* _temp2907=({ struct Cyc_Absyn_Structdecl*
_temp2902=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2902->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2902->name=({ struct Cyc_Core_Opt*
_temp2904=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2904->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2905=( unsigned char*)"_struct"; struct _tagged_string _temp2906; _temp2906.curr=
_temp2905; _temp2906.base= _temp2905; _temp2906.last_plus_one= _temp2905 + 8;
_temp2906;})); _temp2904;}); _temp2902->tvs= 0; _temp2902->fields=({ struct Cyc_Core_Opt*
_temp2903=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2903->v=( void*) fields; _temp2903;}); _temp2902->attributes= 0; _temp2902;});
goto _LL2908; _LL2908: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2909=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2909->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2910=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2910[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2911; _temp2911.tag= Cyc_Absyn_Struct_d;
_temp2911.f1= _temp2907; _temp2911;}); _temp2910;}), 0); _temp2909->tl= Cyc_Toc_result_decls;
_temp2909;});}} goto _LL2871;}} _LL2875: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2917= Cyc_Absyn_string_exp(( struct _tagged_string)({
int _temp2912= 0; int _temp2913= 0; int _temp2914= 0; int _temp2915= 0; struct
_tagged_string _temp2916=* fn; xprintf("%c%c%c%c%.*s", _temp2912, _temp2913,
_temp2914, _temp2915, _temp2916.last_plus_one - _temp2916.curr, _temp2916.curr);}),
0); goto _LL2918; _LL2918: { struct Cyc_Absyn_Vardecl* _temp2919= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2868,( struct Cyc_Absyn_Exp*) _temp2917); goto _LL2920;
_LL2920:( void*)( _temp2919->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2921=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2921->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2922=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2922[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2923; _temp2923.tag= Cyc_Absyn_Var_d; _temp2923.f1= _temp2919; _temp2923;});
_temp2922;}), 0); _temp2921->tl= Cyc_Toc_result_decls; _temp2921;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2871; _LL2877: goto
_LL2871; _LL2871:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2924= ed->name; goto
_LL2925; _LL2925:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2926=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2927; _LL2927: for( 0; _temp2926 != 0;
_temp2926=(( struct Cyc_List_List*) _check_null( _temp2926))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2926))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2926))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2926))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct _RegionHandle
_temp2928= _new_region(); struct _RegionHandle* prgn=& _temp2928; _push_region(
prgn);{ struct Cyc_Absyn_Stmt* _temp2931; struct Cyc_List_List* _temp2933;
struct Cyc_Toc_Env* _temp2935; struct _tuple13 _temp2929= Cyc_Toc_xlate_pat( nv,
prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2936: _temp2935= _temp2929.f1; goto _LL2934; _LL2934: _temp2933=
_temp2929.f2; goto _LL2932; _LL2932: _temp2931= _temp2929.f3; goto _LL2930;
_LL2930: Cyc_Toc_stmt_to_c( _temp2935, s);{ struct Cyc_Absyn_Stmt* _temp2937=
Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL2938; _LL2938: if( exhaust){ s=
Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2931, _temp2937, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2940= Cyc_Toc_newthrow_exp(
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2939=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2939->f1= Cyc_Toc_abs_ns; _temp2939->f2= Cyc_Toc_Match_Exception_sp;
_temp2939;}), 0)); goto _LL2941; _LL2941: { struct Cyc_Absyn_Stmt* _temp2942=
Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2940, 0), 0); goto
_LL2943; _LL2943: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2931, Cyc_Absyn_seq_stmt( _temp2942,
_temp2937, 0), 0), 0);}} for( 0; _temp2933 != 0; _temp2933=(( struct Cyc_List_List*)
_check_null( _temp2933))->tl){ struct _tuple14 _temp2946; void* _temp2947;
struct _tuple0* _temp2949; struct _tuple14* _temp2944=( struct _tuple14*)((
struct Cyc_List_List*) _check_null( _temp2933))->hd; _temp2946=* _temp2944;
_LL2950: _temp2949= _temp2946.f1; goto _LL2948; _LL2948: _temp2947= _temp2946.f2;
goto _LL2945; _LL2945: s= Cyc_Absyn_declare_stmt( _temp2949, _temp2947, 0, s, 0);}}};
_pop_region(& _temp2928);} return s;}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=((
struct Cyc_List_List*) _check_null( ds))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2951=(
void*) d->r; struct Cyc_Absyn_Vardecl* _temp2977; struct Cyc_Absyn_Fndecl*
_temp2979; int _temp2981; struct Cyc_Absyn_Exp* _temp2983; struct Cyc_Core_Opt*
_temp2985; struct Cyc_Core_Opt* _temp2987; struct Cyc_Absyn_Pat* _temp2989;
struct Cyc_List_List* _temp2991; struct Cyc_Absyn_Structdecl* _temp2993; struct
Cyc_Absyn_Uniondecl* _temp2995; struct Cyc_Absyn_Tuniondecl* _temp2997; struct
Cyc_Absyn_Enumdecl* _temp2999; struct Cyc_Absyn_Typedefdecl* _temp3001; struct
Cyc_List_List* _temp3003; struct _tagged_string* _temp3005; struct Cyc_List_List*
_temp3007; struct _tuple0* _temp3009; struct Cyc_List_List* _temp3011; _LL2953:
if(*(( int*) _temp2951) == Cyc_Absyn_Var_d){ _LL2978: _temp2977=(( struct Cyc_Absyn_Var_d_struct*)
_temp2951)->f1; goto _LL2954;} else{ goto _LL2955;} _LL2955: if(*(( int*)
_temp2951) == Cyc_Absyn_Fn_d){ _LL2980: _temp2979=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2951)->f1; goto _LL2956;} else{ goto _LL2957;} _LL2957: if(*(( int*)
_temp2951) == Cyc_Absyn_Let_d){ _LL2990: _temp2989=(( struct Cyc_Absyn_Let_d_struct*)
_temp2951)->f1; goto _LL2988; _LL2988: _temp2987=(( struct Cyc_Absyn_Let_d_struct*)
_temp2951)->f2; goto _LL2986; _LL2986: _temp2985=(( struct Cyc_Absyn_Let_d_struct*)
_temp2951)->f3; goto _LL2984; _LL2984: _temp2983=(( struct Cyc_Absyn_Let_d_struct*)
_temp2951)->f4; goto _LL2982; _LL2982: _temp2981=(( struct Cyc_Absyn_Let_d_struct*)
_temp2951)->f5; goto _LL2958;} else{ goto _LL2959;} _LL2959: if(*(( int*)
_temp2951) == Cyc_Absyn_Letv_d){ _LL2992: _temp2991=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2951)->f1; goto _LL2960;} else{ goto _LL2961;} _LL2961: if(*(( int*)
_temp2951) == Cyc_Absyn_Struct_d){ _LL2994: _temp2993=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2951)->f1; goto _LL2962;} else{ goto _LL2963;} _LL2963: if(*(( int*)
_temp2951) == Cyc_Absyn_Union_d){ _LL2996: _temp2995=(( struct Cyc_Absyn_Union_d_struct*)
_temp2951)->f1; goto _LL2964;} else{ goto _LL2965;} _LL2965: if(*(( int*)
_temp2951) == Cyc_Absyn_Tunion_d){ _LL2998: _temp2997=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2951)->f1; goto _LL2966;} else{ goto _LL2967;} _LL2967: if(*(( int*)
_temp2951) == Cyc_Absyn_Enum_d){ _LL3000: _temp2999=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2951)->f1; goto _LL2968;} else{ goto _LL2969;} _LL2969: if(*(( int*)
_temp2951) == Cyc_Absyn_Typedef_d){ _LL3002: _temp3001=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2951)->f1; goto _LL2970;} else{ goto _LL2971;} _LL2971: if(*(( int*)
_temp2951) == Cyc_Absyn_Namespace_d){ _LL3006: _temp3005=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2951)->f1; goto _LL3004; _LL3004: _temp3003=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2951)->f2; goto _LL2972;} else{ goto _LL2973;} _LL2973: if(*(( int*)
_temp2951) == Cyc_Absyn_Using_d){ _LL3010: _temp3009=(( struct Cyc_Absyn_Using_d_struct*)
_temp2951)->f1; goto _LL3008; _LL3008: _temp3007=(( struct Cyc_Absyn_Using_d_struct*)
_temp2951)->f2; goto _LL2974;} else{ goto _LL2975;} _LL2975: if(*(( int*)
_temp2951) == Cyc_Absyn_ExternC_d){ _LL3012: _temp3011=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2951)->f1; goto _LL2976;} else{ goto _LL2952;} _LL2954: { struct _tuple0*
_temp3013= _temp2977->name; goto _LL3014; _LL3014: if(( void*) _temp2977->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3013=({ struct _tuple0* _temp3015=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3015->f1= Cyc_Toc_abs_ns;
_temp3015->f2=(* _temp3013).f2; _temp3015;});} if( _temp2977->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null( _temp2977->initializer));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp3016=( unsigned char*)"decls_to_c: not at toplevel!"; struct
_tagged_string _temp3017; _temp3017.curr= _temp3016; _temp3017.base= _temp3016;
_temp3017.last_plus_one= _temp3016 + 29; _temp3017;}));} nv= Cyc_Toc_add_varmap(
nv, _temp2977->name, Cyc_Absyn_varb_exp( _temp3013,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp3018=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp3018[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp3019; _temp3019.tag= Cyc_Absyn_Global_b;
_temp3019.f1= _temp2977; _temp3019;}); _temp3018;}), 0)); _temp2977->name=
_temp3013;( void*)( _temp2977->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2977->sc));(
void*)( _temp2977->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2977->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3020=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3020->hd=( void*) d; _temp3020->tl=
Cyc_Toc_result_decls; _temp3020;}); goto _LL2952;} _LL2956: { struct _tuple0*
_temp3021= _temp2979->name; goto _LL3022; _LL3022: if(( void*) _temp2979->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3021=({ struct _tuple0* _temp3023=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3023->f1= Cyc_Toc_abs_ns;
_temp3023->f2=(* _temp3021).f2; _temp3023;});} nv= Cyc_Toc_add_varmap( nv,
_temp2979->name, Cyc_Absyn_var_exp( _temp3021, 0)); _temp2979->name= _temp3021;
Cyc_Toc_fndecl_to_c( nv, _temp2979); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3024=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3024->hd=( void*) d; _temp3024->tl= Cyc_Toc_result_decls; _temp3024;});
goto _LL2952;} _LL2958:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp3025=( unsigned char*)"letdecl"; struct _tagged_string
_temp3026; _temp3026.curr= _temp3025; _temp3026.base= _temp3025; _temp3026.last_plus_one=
_temp3025 + 8; _temp3026;})); goto _LL2952; _LL2960:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp3027=(
unsigned char*)"letdecl"; struct _tagged_string _temp3028; _temp3028.curr=
_temp3027; _temp3028.base= _temp3027; _temp3028.last_plus_one= _temp3027 + 8;
_temp3028;})); goto _LL2952; _LL2962: Cyc_Toc_structdecl_to_c( _temp2993); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3029=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3029->hd=( void*) d; _temp3029->tl= Cyc_Toc_result_decls;
_temp3029;}); goto _LL2952; _LL2964: Cyc_Toc_uniondecl_to_c( _temp2995); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3030=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3030->hd=( void*) d; _temp3030->tl= Cyc_Toc_result_decls;
_temp3030;}); goto _LL2952; _LL2966: if( _temp2997->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2997);} else{ Cyc_Toc_tuniondecl_to_c( _temp2997);} goto _LL2952; _LL2968:
Cyc_Toc_enumdecl_to_c( nv, _temp2999); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3031=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3031->hd=( void*) d; _temp3031->tl= Cyc_Toc_result_decls; _temp3031;});
goto _LL2952; _LL2970: _temp3001->name= _temp3001->name; _temp3001->tvs= 0;(
void*)( _temp3001->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp3001->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3032=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3032->hd=( void*) d; _temp3032->tl=
Cyc_Toc_result_decls; _temp3032;}); goto _LL2952; _LL2972: _temp3007= _temp3003;
goto _LL2974; _LL2974: _temp3011= _temp3007; goto _LL2976; _LL2976: nv= Cyc_Toc_decls_to_c(
nv, _temp3011, top); goto _LL2952; _LL2952:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}