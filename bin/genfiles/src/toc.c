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
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp189="is_nullable";
struct _tagged_string _temp190; _temp190.curr= _temp189; _temp190.base= _temp189;
_temp190.last_plus_one= _temp189 + 12; _temp190;})); return 0; _LL164:;} static
struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x, struct
_tagged_string tag){ return({ struct _tuple0* _temp191=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp191->f1=(* x).f1; _temp191->f2=({
struct _tagged_string* _temp192=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp192[ 0]= Cyc_String_strconcat(*(* x).f2, tag);
_temp192;}); _temp191;});} static void Cyc_Toc_add_tuple_decl( struct
_tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List* _temp193= 0;
goto _LL194; _LL194:{ int i= 1; for( 0; ts != 0;( ts=(( struct Cyc_List_List*)
_check_null( ts))->tl, i ++)){ _temp193=({ struct Cyc_List_List* _temp195=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp195->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp196=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp196->name=({ struct
_tagged_string* _temp197=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp197[ 0]= xprintf("f%d", i); _temp197;}); _temp196->tq=
Cyc_Toc_mt_tq; _temp196->type=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); _temp196->width= 0; _temp196->attributes= 0; _temp196;});
_temp195->tl= _temp193; _temp195;});}} _temp193=(( struct Cyc_List_List*(*)(
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
void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
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
_tagged_string) * 1); _temp246[ 0]= xprintf("_tuple%d", Cyc_Toc_tuple_type_counter
++); _temp246;}); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt,
tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({ struct Cyc_List_List*
_temp244=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp244->hd=( void*)({ struct _tuple5* _temp245=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp245->f1= x; _temp245->f2= ts; _temp245;});
_temp244->tl= Cyc_Toc_tuple_types; _temp244;}); return x;}} static struct Cyc_List_List*
Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter= 0; static
void* Cyc_Toc_tagged_string_type_v=( void*) 0u; static void* Cyc_Toc_tagged_string_type(){
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
struct _tagged_string) * 1); _temp266[ 0]= xprintf("_tagged_ptr%d", Cyc_Toc_tagged_ptr_counter
++); _temp266;}); Cyc_Toc_add_tagged_ptr_decl( x, t); Cyc_Toc_tagged_ptr_types=({
struct Cyc_List_List* _temp264=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp264->hd=( void*)({ struct _tuple6* _temp265=(
struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp265->f1= x; _temp265->f2=
t; _temp265;}); _temp264->tl= Cyc_Toc_tagged_ptr_types; _temp264;}); return Cyc_Absyn_strct(
x);}} static int Cyc_Toc_temp_var_counter= 0; struct _tuple0* Cyc_Toc_temp_var(){
return({ struct _tuple0* _temp267=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp267->f1=( void*) Cyc_Absyn_Loc_n; _temp267->f2=({ struct
_tagged_string* _temp268=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp268[ 0]= xprintf("_temp%d", Cyc_Toc_temp_var_counter
++); _temp268;}); _temp267;});} static int Cyc_Toc_fresh_label_counter= 0;
static struct _tagged_string* Cyc_Toc_fresh_label(){ return({ struct
_tagged_string* _temp269=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp269[ 0]= xprintf("_LL%d", Cyc_Toc_temp_var_counter ++);
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
unsigned char* _temp483="unresolved TunionType"; struct _tagged_string _temp484;
_temp484.curr= _temp483; _temp484.base= _temp483; _temp484.last_plus_one=
_temp483 + 22; _temp484;}))); _LL329: if( _temp405->typs == 0){ if( _temp407->is_xtunion){
return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);}
else{ return Cyc_Absyn_uint_t;}} else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp405->name,({ unsigned char* _temp485="_struct"; struct _tagged_string
_temp486; _temp486.curr= _temp485; _temp486.base= _temp485; _temp486.last_plus_one=
_temp485 + 8; _temp486;})));} _LL331:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp487="unresolved TunionFieldType";
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
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp529="anonymous union";
struct _tagged_string _temp530; _temp530.curr= _temp529; _temp530.base= _temp529;
_temp530.last_plus_one= _temp529 + 16; _temp530;}));} return Cyc_Absyn_unionq_typ((
struct _tuple0*) _check_null( _temp459)); _LL353: if( _temp465 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp531="anonymous struct";
struct _tagged_string _temp532; _temp532.curr= _temp531; _temp532.base= _temp531;
_temp532.last_plus_one= _temp531 + 17; _temp532;}));} return Cyc_Absyn_strctq((
struct _tuple0*) _check_null( _temp465)); _LL355: return t; _LL357: if( _temp471
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp533="Toc::typ_to_c: unresolved TypedefType"; struct
_tagged_string _temp534; _temp534.curr= _temp533; _temp534.base= _temp533;
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
unsigned char* _temp538="Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp539; _temp539.curr= _temp538; _temp539.base= _temp538;
_temp539.last_plus_one= _temp538 + 55; _temp539;})); _LL363: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp540="Toc::typ_to_c: type translation passed the access eff";
struct _tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 54; _temp541;})); _LL365: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp542="Toc::typ_to_c: type translation passed the join eff";
struct _tagged_string _temp543; _temp543.curr= _temp542; _temp543.base= _temp542;
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
_LL589: return 0; _LL591:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp640= Cyc_Absynpp_typ2string( t); xprintf("atomic_typ:  bad type %.*s",
_temp640.last_plus_one - _temp640.curr, _temp640.curr);})); return 0; _LL557:;}
static int Cyc_Toc_is_void_star( void* t){ void* _temp641= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp647; struct Cyc_Absyn_Conref* _temp649; struct
Cyc_Absyn_Tqual _temp651; struct Cyc_Absyn_Conref* _temp653; void* _temp655;
void* _temp657; _LL643: if(( unsigned int) _temp641 > 4u?*(( int*) _temp641) ==
Cyc_Absyn_PointerType: 0){ _LL648: _temp647=(( struct Cyc_Absyn_PointerType_struct*)
_temp641)->f1; _LL658: _temp657=( void*) _temp647.elt_typ; goto _LL656; _LL656:
_temp655=( void*) _temp647.rgn_typ; goto _LL654; _LL654: _temp653= _temp647.nullable;
goto _LL652; _LL652: _temp651= _temp647.tq; goto _LL650; _LL650: _temp649=
_temp647.bounds; goto _LL644;} else{ goto _LL645;} _LL645: goto _LL646; _LL644: {
void* _temp659= Cyc_Tcutil_compress( _temp657); _LL661: if( _temp659 ==( void*)
Cyc_Absyn_VoidType){ goto _LL662;} else{ goto _LL663;} _LL663: goto _LL664;
_LL662: return 1; _LL664: return 0; _LL660:;} _LL646: return 0; _LL642:;} static
int Cyc_Toc_is_poly_field( void* t, struct _tagged_string* f){ void* _temp665=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl** _temp677; struct Cyc_Absyn_Structdecl*
_temp679; struct Cyc_List_List* _temp680; struct _tuple0* _temp682; struct Cyc_List_List*
_temp684; struct Cyc_List_List* _temp686; struct Cyc_Absyn_Uniondecl** _temp688;
struct Cyc_List_List* _temp690; struct _tuple0* _temp692; _LL667: if((
unsigned int) _temp665 > 4u?*(( int*) _temp665) == Cyc_Absyn_StructType: 0){
_LL683: _temp682=(( struct Cyc_Absyn_StructType_struct*) _temp665)->f1; goto
_LL681; _LL681: _temp680=(( struct Cyc_Absyn_StructType_struct*) _temp665)->f2;
goto _LL678; _LL678: _temp677=(( struct Cyc_Absyn_StructType_struct*) _temp665)->f3;
if( _temp677 == 0){ goto _LL669;} else{ _temp679=* _temp677; goto _LL668;}}
else{ goto _LL669;} _LL669: if(( unsigned int) _temp665 > 4u?*(( int*) _temp665)
== Cyc_Absyn_AnonStructType: 0){ _LL685: _temp684=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp665)->f1; goto _LL670;} else{ goto _LL671;} _LL671: if(( unsigned int)
_temp665 > 4u?*(( int*) _temp665) == Cyc_Absyn_AnonUnionType: 0){ _LL687:
_temp686=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp665)->f1; goto _LL672;}
else{ goto _LL673;} _LL673: if(( unsigned int) _temp665 > 4u?*(( int*) _temp665)
== Cyc_Absyn_UnionType: 0){ _LL693: _temp692=(( struct Cyc_Absyn_UnionType_struct*)
_temp665)->f1; goto _LL691; _LL691: _temp690=(( struct Cyc_Absyn_UnionType_struct*)
_temp665)->f2; goto _LL689; _LL689: _temp688=(( struct Cyc_Absyn_UnionType_struct*)
_temp665)->f3; goto _LL674;} else{ goto _LL675;} _LL675: goto _LL676; _LL668:
if( _temp679->fields == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp694="is_poly_field: struct missing fields";
struct _tagged_string _temp695; _temp695.curr= _temp694; _temp695.base= _temp694;
_temp695.last_plus_one= _temp694 + 37; _temp695;})));} _temp684=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp679->fields))->v; goto _LL670; _LL670: {
struct Cyc_Absyn_Structfield* _temp696= Cyc_Absyn_lookup_field( _temp684, f);
goto _LL697; _LL697: if( _temp696 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp698=*(( struct Cyc_Absyn_Structfield*)
_check_null( _temp696))->name; xprintf("is_poly_field: bad field %.*s", _temp698.last_plus_one
- _temp698.curr, _temp698.curr);}));} return Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp696))->type);} _LL672: return 0;
_LL674: return 0; _LL676:( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp699= Cyc_Absynpp_typ2string( t);
xprintf("is_poly_field: bad type %.*s", _temp699.last_plus_one - _temp699.curr,
_temp699.curr);}))); _LL666:;} static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp*
e){ void* _temp700=( void*) e->r; struct _tagged_string* _temp708; struct Cyc_Absyn_Exp*
_temp710; struct _tagged_string* _temp712; struct Cyc_Absyn_Exp* _temp714;
_LL702: if(*(( int*) _temp700) == Cyc_Absyn_StructMember_e){ _LL711: _temp710=((
struct Cyc_Absyn_StructMember_e_struct*) _temp700)->f1; goto _LL709; _LL709:
_temp708=(( struct Cyc_Absyn_StructMember_e_struct*) _temp700)->f2; goto _LL703;}
else{ goto _LL704;} _LL704: if(*(( int*) _temp700) == Cyc_Absyn_StructArrow_e){
_LL715: _temp714=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp700)->f1; goto
_LL713; _LL713: _temp712=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp700)->f2;
goto _LL705;} else{ goto _LL706;} _LL706: goto _LL707; _LL703: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp710->topt))->v, _temp708);
_LL705: { void* _temp716= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp714->topt))->v); struct Cyc_Absyn_PtrInfo _temp722; struct Cyc_Absyn_Conref*
_temp724; struct Cyc_Absyn_Tqual _temp726; struct Cyc_Absyn_Conref* _temp728;
void* _temp730; void* _temp732; _LL718: if(( unsigned int) _temp716 > 4u?*(( int*)
_temp716) == Cyc_Absyn_PointerType: 0){ _LL723: _temp722=(( struct Cyc_Absyn_PointerType_struct*)
_temp716)->f1; _LL733: _temp732=( void*) _temp722.elt_typ; goto _LL731; _LL731:
_temp730=( void*) _temp722.rgn_typ; goto _LL729; _LL729: _temp728= _temp722.nullable;
goto _LL727; _LL727: _temp726= _temp722.tq; goto _LL725; _LL725: _temp724=
_temp722.bounds; goto _LL719;} else{ goto _LL720;} _LL720: goto _LL721; _LL719:
return Cyc_Toc_is_poly_field( _temp732, _temp712); _LL721:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp734= Cyc_Absynpp_typ2string((
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
+ 1u}; static struct _tagged_string* Cyc_Toc_fieldname( int i){ return({ struct
_tagged_string* _temp744=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp744[ 0]= xprintf("f%d", i); _temp744;});{ unsigned int
fsz=({ struct _tagged_ptr2 _temp750= Cyc_Toc_field_names;( unsigned int)(
_temp750.last_plus_one - _temp750.curr);}); if( i >= fsz){ Cyc_Toc_field_names=({
unsigned int _temp745=( unsigned int)( i + 1); struct _tagged_string** _temp746=(
struct _tagged_string**) GC_malloc( sizeof( struct _tagged_string*) * _temp745);
struct _tagged_ptr2 _temp749={ _temp746, _temp746, _temp746 + _temp745};{
unsigned int _temp747= _temp745; unsigned int j; for( j= 0; j < _temp747; j ++){
_temp746[ j]= j < fsz?*(( struct _tagged_string**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Toc_field_names,
sizeof( struct _tagged_string*),( int) j):({ struct _tagged_string* _temp748=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp748[ 0]=
xprintf("f%d",( int) j); _temp748;});}}; _temp749;});} return*(( struct
_tagged_string**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Toc_field_names, sizeof( struct _tagged_string*),
i);}} static const int Cyc_Toc_NullCheck= 0; struct Cyc_Toc_NullCheck_struct{
int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; static const int Cyc_Toc_NoConv= 0; struct _tuple7{ void* f1; void* f2; } ;
static void* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp752=({ struct _tuple7
_temp751; _temp751.f1= t1; _temp751.f2= t2; _temp751;}); void* _temp758; struct
Cyc_Absyn_PtrInfo _temp760; void* _temp762; struct Cyc_Absyn_PtrInfo _temp764;
_LL754: _LL763: _temp762= _temp752.f1; if(( unsigned int) _temp762 > 4u?*(( int*)
_temp762) == Cyc_Absyn_PointerType: 0){ _LL765: _temp764=(( struct Cyc_Absyn_PointerType_struct*)
_temp762)->f1; goto _LL759;} else{ goto _LL756;} _LL759: _temp758= _temp752.f2;
if(( unsigned int) _temp758 > 4u?*(( int*) _temp758) == Cyc_Absyn_PointerType: 0){
_LL761: _temp760=(( struct Cyc_Absyn_PointerType_struct*) _temp758)->f1; goto
_LL755;} else{ goto _LL756;} _LL756: goto _LL757; _LL755: { int _temp766=(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp764.nullable); goto
_LL767; _LL767: { int _temp768=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp760.nullable); goto _LL769; _LL769: { void* _temp770=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp764.bounds); goto _LL771;
_LL771: { void* _temp772=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp760.bounds); goto _LL773; _LL773: { struct _tuple7 _temp775=({ struct
_tuple7 _temp774; _temp774.f1= _temp770; _temp774.f2= _temp772; _temp774;});
void* _temp785; struct Cyc_Absyn_Exp* _temp787; void* _temp789; struct Cyc_Absyn_Exp*
_temp791; void* _temp793; void* _temp795; void* _temp797; void* _temp799; struct
Cyc_Absyn_Exp* _temp801; void* _temp803; struct Cyc_Absyn_Exp* _temp805; void*
_temp807; _LL777: _LL790: _temp789= _temp775.f1; if(( unsigned int) _temp789 > 1u?*((
int*) _temp789) == Cyc_Absyn_Upper_b: 0){ _LL792: _temp791=(( struct Cyc_Absyn_Upper_b_struct*)
_temp789)->f1; goto _LL786;} else{ goto _LL779;} _LL786: _temp785= _temp775.f2;
if(( unsigned int) _temp785 > 1u?*(( int*) _temp785) == Cyc_Absyn_Upper_b: 0){
_LL788: _temp787=(( struct Cyc_Absyn_Upper_b_struct*) _temp785)->f1; goto _LL778;}
else{ goto _LL779;} _LL779: _LL796: _temp795= _temp775.f1; if( _temp795 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL794;} else{ goto _LL781;} _LL794: _temp793=
_temp775.f2; if( _temp793 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL780;} else{
goto _LL781;} _LL781: _LL800: _temp799= _temp775.f1; if(( unsigned int) _temp799
> 1u?*(( int*) _temp799) == Cyc_Absyn_Upper_b: 0){ _LL802: _temp801=(( struct
Cyc_Absyn_Upper_b_struct*) _temp799)->f1; goto _LL798;} else{ goto _LL783;}
_LL798: _temp797= _temp775.f2; if( _temp797 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL782;} else{ goto _LL783;} _LL783: _LL808: _temp807= _temp775.f1; if(
_temp807 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL804;} else{ goto _LL776;}
_LL804: _temp803= _temp775.f2; if(( unsigned int) _temp803 > 1u?*(( int*)
_temp803) == Cyc_Absyn_Upper_b: 0){ _LL806: _temp805=(( struct Cyc_Absyn_Upper_b_struct*)
_temp803)->f1; goto _LL784;} else{ goto _LL776;} _LL778: if( _temp766? !
_temp768: 0){ return( void*)({ struct Cyc_Toc_NullCheck_struct* _temp809=(
struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp809[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp810; _temp810.tag= Cyc_Toc_NullCheck;
_temp810.f1=( void*) t2; _temp810;}); _temp809;});} return( void*) Cyc_Toc_NoConv;
_LL780: return( void*) Cyc_Toc_NoConv; _LL782: return( void*)({ struct Cyc_Toc_TagPtr_struct*
_temp811=( struct Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp811[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp812; _temp812.tag= Cyc_Toc_TagPtr;
_temp812.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp812.f2= _temp801; _temp812;});
_temp811;}); _LL784: return( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp813=(
struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp813[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp814; _temp814.tag= Cyc_Toc_UntagPtr;
_temp814.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp814.f2= _temp805; _temp814.f3=
_temp768; _temp814;}); _temp813;}); _LL776:;}}}}} _LL757: return( void*) Cyc_Toc_NoConv;
_LL753:;}} static unsigned char _temp817[ 14u]="sfile_to_file"; static struct
_tagged_string Cyc_Toc__sfile_to_file_string={ _temp817, _temp817, _temp817 + 14u};
static struct _tuple0 Cyc_Toc__sfile_to_file_var={( void*)& Cyc_Toc_abs_ns_value,&
Cyc_Toc__sfile_to_file_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv(
struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__sfile_to_file_var,
0),({ struct Cyc_List_List* _temp818=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp818->hd=( void*) e; _temp818->tl= 0; _temp818;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__throw_qv, 0),({ struct
Cyc_List_List* _temp819=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp819->hd=( void*) e; _temp819->tl= 0; _temp819;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_null_check_conv( int toplevel, void* t, struct Cyc_Absyn_Exp* e){ void*
new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ unsigned char* _temp820="can't do null-check conversion at top-level";
struct _tagged_string _temp821; _temp821.curr= _temp820; _temp821.base= _temp820;
_temp821.last_plus_one= _temp820 + 44; _temp821;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp822=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp822->hd=( void*) e; _temp822->tl= 0; _temp822;}), 0),
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
sz); void* strt_typ; void* elt_typ;{ void* _temp823= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp829; struct Cyc_Absyn_Conref* _temp831; struct Cyc_Absyn_Tqual
_temp833; struct Cyc_Absyn_Conref* _temp835; void* _temp837; void* _temp839;
_LL825: if(( unsigned int) _temp823 > 4u?*(( int*) _temp823) == Cyc_Absyn_PointerType:
0){ _LL830: _temp829=(( struct Cyc_Absyn_PointerType_struct*) _temp823)->f1;
_LL840: _temp839=( void*) _temp829.elt_typ; goto _LL838; _LL838: _temp837=( void*)
_temp829.rgn_typ; goto _LL836; _LL836: _temp835= _temp829.nullable; goto _LL834;
_LL834: _temp833= _temp829.tq; goto _LL832; _LL832: _temp831= _temp829.bounds;
goto _LL826;} else{ goto _LL827;} _LL827: goto _LL828; _LL826: elt_typ= _temp839;
strt_typ= Cyc_Toc_add_tagged_ptr_type( _temp839, Cyc_Toc_mt_tq); goto _LL824;
_LL828: return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp841="tag_ptr_conv: not a pointer"; struct _tagged_string
_temp842; _temp842.curr= _temp841; _temp842.base= _temp841; _temp842.last_plus_one=
_temp841 + 28; _temp842;})); _LL824:;} if( toplevel){ int is_string= 0;{ void*
_temp843=( void*) e->r; void* _temp849; struct _tagged_string _temp851; _LL845:
if(*(( int*) _temp843) == Cyc_Absyn_Const_e){ _LL850: _temp849=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp843)->f1; if(( unsigned int) _temp849 > 1u?*((
int*) _temp849) == Cyc_Absyn_String_c: 0){ _LL852: _temp851=(( struct Cyc_Absyn_String_c_struct*)
_temp849)->f1; goto _LL846;} else{ goto _LL847;}} else{ goto _LL847;} _LL847:
goto _LL848; _LL846: is_string= 1; goto _LL844; _LL848: goto _LL844; _LL844:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp856=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp856[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp857; _temp857.tag= Cyc_Absyn_ArrayType; _temp857.f1=(
void*) elt_typ; _temp857.f2= Cyc_Toc_mt_tq; _temp857.f3=( struct Cyc_Absyn_Exp*)
sz; _temp857;}); _temp856;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp853=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp853->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp854=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp854[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp855; _temp855.tag= Cyc_Absyn_Var_d;
_temp855.f1= vd; _temp855;}); _temp854;}), 0); _temp853->tl= Cyc_Toc_result_decls;
_temp853;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List*(* _temp858)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tuple4* _temp861=({ struct _tuple4*
_temp866=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp866->f1= 0;
_temp866->f2= xexp; _temp866;}); struct _tuple4* _temp862=({ struct _tuple4*
_temp865=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp865->f1= 0;
_temp865->f2= xexp; _temp865;}); struct _tuple4* _temp863=({ struct _tuple4*
_temp864=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp864->f1= 0;
_temp864->f2= xplussz; _temp864;}); struct _tuple4* _temp859[ 3u]={ _temp861,
_temp862, _temp863}; struct _tagged_ptr3 _temp860={ _temp859, _temp859, _temp859
+ 3u}; _temp858( _temp860);}), 0); return urm_exp;}}} else{ struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( b, 0), Cyc_Toc_last_plus_one_sp,
0), Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp(
szvalue, 0), 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( b, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_var_exp( a, 0), 0), s,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( b, 0), Cyc_Toc_curr_sp, 0), Cyc_Absyn_var_exp( a, 0), 0), s,
0); s= Cyc_Absyn_declare_stmt( b, strt_typ, 0, s, 0); s= Cyc_Absyn_declare_stmt(
a, Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq),( struct Cyc_Absyn_Exp*) e, s, 0);
return Cyc_Absyn_stmt_exp( s, 0);}} static struct Cyc_Absyn_Exp* Cyc_Toc_untag_ptr_conv(
int toplevel, void* t1, struct Cyc_Absyn_Exp* sz, int nullable, struct Cyc_Absyn_Exp*
e){ if( toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp867="can't coerce t? to t* or t@ at the top-level"; struct
_tagged_string _temp868; _temp868.curr= _temp867; _temp868.base= _temp867;
_temp868.last_plus_one= _temp867 + 45; _temp868;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
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
return({ struct Cyc_Toc_Env* _temp869=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp869->break_lab= 0; _temp869->continue_lab= 0;
_temp869->fallthru_info= 0; _temp869->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp869->toplevel= 1; _temp869;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp870=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp870->break_lab= e->break_lab;
_temp870->continue_lab= e->continue_lab; _temp870->fallthru_info= e->fallthru_info;
_temp870->varmap= e->varmap; _temp870->toplevel= e->toplevel; _temp870;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp871=(* x).f1; struct Cyc_List_List*
_temp877; _LL873: if(( unsigned int) _temp871 > 1u?*(( int*) _temp871) == Cyc_Absyn_Rel_n:
0){ _LL878: _temp877=(( struct Cyc_Absyn_Rel_n_struct*) _temp871)->f1; goto
_LL874;} else{ goto _LL875;} _LL875: goto _LL876; _LL874:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp879=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp879[ 0]=({ struct
Cyc_Core_Impossible_struct _temp880; _temp880.tag= Cyc_Core_Impossible; _temp880.f1=({
struct _tagged_string _temp881= Cyc_Absynpp_qvar2string( x); xprintf("Toc::add_varmap on Rel_n: %.*s\n",
_temp881.last_plus_one - _temp881.curr, _temp881.curr);}); _temp880;}); _temp879;}));
_LL876: goto _LL872; _LL872:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y); return
ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple8{ struct _tagged_string* f1; struct Cyc_List_List*
f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l, struct _tagged_string*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp882= 0; goto _LL883; _LL883: for( 0;
fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*) _check_null(
fallthru_binders))->tl){ _temp882=({ struct Cyc_List_List* _temp884=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp884->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp884->tl=
_temp882; _temp884;});} _temp882=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp882);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp885=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp885->v=( void*) break_l; _temp885;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp886=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp886->v=( void*)({ struct _tuple8*
_temp887=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp887->f1=
fallthru_l; _temp887->f2= _temp882; _temp887->f3= next_case_env->varmap;
_temp887;}); _temp886;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp888=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp888->v=( void*)
break_l; _temp888;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_string*
next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp889=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp889->v=( void*)({ struct _tuple8* _temp890=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp890->f1= next_l; _temp890->f2= 0;
_temp890->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp890;}); _temp889;});
return ans;} struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tagged_ptr4{ struct Cyc_List_List** curr; struct Cyc_List_List** base;
struct Cyc_List_List** last_plus_one; } ; struct _tuple10{ struct _tuple0* f1;
void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp891="expecting a literal format string";
struct _tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 34; _temp892;}));}{ struct _tagged_string
fmt_str;{ void* _temp893=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd)->r; void* _temp899; struct _tagged_string _temp901;
_LL895: if(*(( int*) _temp893) == Cyc_Absyn_Const_e){ _LL900: _temp899=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp893)->f1; if(( unsigned int) _temp899 > 1u?*((
int*) _temp899) == Cyc_Absyn_String_c: 0){ _LL902: _temp901=(( struct Cyc_Absyn_String_c_struct*)
_temp899)->f1; goto _LL896;} else{ goto _LL897;}} else{ goto _LL897;} _LL897:
goto _LL898; _LL896: fmt_str= _temp901; goto _LL894; _LL898: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp903="expecting a literal format string";
struct _tagged_string _temp904; _temp904.curr= _temp903; _temp904.base= _temp903;
_temp904.last_plus_one= _temp903 + 34; _temp904;})); _LL894:;}{ int len=( int)({
struct _tagged_string _temp1000= fmt_str;( unsigned int)( _temp1000.last_plus_one
- _temp1000.curr);}); int i= 0; for( i= 0; i < len; i ++){ if(*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( unsigned char), i) =='%'){ i ++; if( i < len?*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( unsigned char), i) =='s': 0){ break;}}} if( i == len){ if( aopt
!= 0){ args=({ struct Cyc_List_List* _temp905=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp905->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp905->tl= args; _temp905;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp906= _new_region(); struct
_RegionHandle* r=& _temp906; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str, sizeof(
unsigned char), i); rev_fmt=({ struct Cyc_List_List* _temp907=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp907->hd=( void*)(( int)
c); _temp907->tl= rev_fmt; _temp907;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt*
_temp908= Cyc_Formatstr_parse_conversionspecification( fmt_str, i + 1); goto
_LL909; _LL909: if( _temp908 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp910="bad format string"; struct _tagged_string _temp911;
_temp911.curr= _temp910; _temp911.base= _temp910; _temp911.last_plus_one=
_temp910 + 18; _temp911;}));}{ struct _tuple9 _temp914; int _temp915;
unsigned char _temp917; struct Cyc_List_List* _temp919; struct Cyc_List_List*
_temp921; struct Cyc_List_List* _temp923; struct Cyc_List_List* _temp925; struct
_tuple9* _temp912=( struct _tuple9*)(( struct Cyc_Core_Opt*) _check_null(
_temp908))->v; _temp914=* _temp912; _LL926: _temp925= _temp914.f1; goto _LL924;
_LL924: _temp923= _temp914.f2; goto _LL922; _LL922: _temp921= _temp914.f3; goto
_LL920; _LL920: _temp919= _temp914.f4; goto _LL918; _LL918: _temp917= _temp914.f5;
goto _LL916; _LL916: _temp915= _temp914.f6; goto _LL913; _LL913: i= _temp915 - 1;
if( _temp917 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp927)( struct _RegionHandle*, struct
_tagged_ptr4)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_ptr4)) Cyc_List_rlist; struct _RegionHandle* _temp934= r; struct Cyc_List_List*
_temp930= _temp925; struct Cyc_List_List* _temp931= _temp923; struct Cyc_List_List*
_temp932= _temp921; struct Cyc_List_List* _temp933= _temp919; struct Cyc_List_List*
_temp928[ 4u]={ _temp930, _temp931, _temp932, _temp933}; struct _tagged_ptr4
_temp929={ _temp928, _temp928, _temp928 + 4u}; _temp927( _temp934, _temp929);})),
rev_fmt); rev_fmt=({ struct Cyc_List_List* _temp935=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp935->hd=( void*)(( int)
_temp917); _temp935->tl= rev_fmt; _temp935;});{ struct Cyc_List_List* _temp936=
_temp923; struct Cyc_List_List _temp943; struct Cyc_List_List* _temp944; int
_temp946; _LL938: if( _temp936 == 0){ goto _LL940;} else{ _temp943=* _temp936;
_LL947: _temp946=( int) _temp943.hd; goto _LL945; _LL945: _temp944= _temp943.tl;
if( _temp944 == 0){ goto _LL942;} else{ goto _LL940;}} _LL942: if( _temp946 =='*'){
goto _LL939;} else{ goto _LL940;} _LL940: goto _LL941; _LL939: { struct _tuple0*
_temp948= Cyc_Toc_temp_var(); goto _LL949; _LL949: rev_temps=({ struct Cyc_List_List*
_temp950=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp950->hd=( void*)({ struct _tuple10* _temp951=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp951->f1= _temp948; _temp951->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp951->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp951;});
_temp950->tl= rev_temps; _temp950;}); rev_result=({ struct Cyc_List_List*
_temp952=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp952->hd=( void*) Cyc_Absyn_var_exp( _temp948, 0); _temp952->tl= rev_result;
_temp952;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL937;} _LL941: goto
_LL937; _LL937:;}{ struct Cyc_List_List* _temp953= _temp921; struct Cyc_List_List
_temp960; struct Cyc_List_List* _temp961; struct Cyc_List_List _temp963; struct
Cyc_List_List* _temp964; int _temp966; int _temp968; _LL955: if( _temp953 == 0){
goto _LL957;} else{ _temp960=* _temp953; _LL969: _temp968=( int) _temp960.hd;
goto _LL962; _LL962: _temp961= _temp960.tl; if( _temp961 == 0){ goto _LL957;}
else{ _temp963=* _temp961; _LL967: _temp966=( int) _temp963.hd; goto _LL965;
_LL965: _temp964= _temp963.tl; if( _temp964 == 0){ goto _LL959;} else{ goto
_LL957;}}} _LL959: if( _temp968 =='.'? _temp966 =='*': 0){ goto _LL956;} else{
goto _LL957;} _LL957: goto _LL958; _LL956: { struct _tuple0* _temp970= Cyc_Toc_temp_var();
goto _LL971; _LL971: rev_temps=({ struct Cyc_List_List* _temp972=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp972->hd=( void*)({
struct _tuple10* _temp973=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp973->f1= _temp970; _temp973->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp973->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp973;}); _temp972->tl= rev_temps; _temp972;});
rev_result=({ struct Cyc_List_List* _temp974=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp974->hd=( void*) Cyc_Absyn_var_exp(
_temp970, 0); _temp974->tl= rev_result; _temp974;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL954;} _LL958: goto _LL954; _LL954:;} if( _temp917 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp975= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp975.last_plus_one - _temp975.curr, _temp975.curr);}));}{ struct _tuple0*
_temp976= Cyc_Toc_temp_var(); goto _LL977; _LL977: rev_temps=({ struct Cyc_List_List*
_temp978=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp978->hd=( void*)({ struct _tuple10* _temp979=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp979->f1= _temp976; _temp979->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp979->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp979;});
_temp978->tl= rev_temps; _temp978;}); rev_result=({ struct Cyc_List_List*
_temp980=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp980->hd=( void*) Cyc_Absyn_var_exp( _temp976, 0); _temp980->tl= rev_result;
_temp980;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp981=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp981->hd=( void*)(( int)'.'); _temp981->tl= rev_fmt; _temp981;}); rev_fmt=({
struct Cyc_List_List* _temp982=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp982->hd=( void*)(( int)'*'); _temp982->tl=
rev_fmt; _temp982;}); rev_fmt=({ struct Cyc_List_List* _temp983=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp983->hd=( void*)(( int)'s');
_temp983->tl= rev_fmt; _temp983;});{ struct _tuple0* _temp984= Cyc_Toc_temp_var();
goto _LL985; _LL985: rev_temps=({ struct Cyc_List_List* _temp986=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp986->hd=( void*)({
struct _tuple10* _temp987=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp987->f1= _temp984; _temp987->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp987->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp987;}); _temp986->tl= rev_temps; _temp986;});{
struct Cyc_Absyn_Exp* _temp988= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp984, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp984, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL989; _LL989: {
struct Cyc_Absyn_Exp* _temp990= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp984, 0), Cyc_Toc_curr_sp, 0); goto _LL991; _LL991: rev_result=({ struct Cyc_List_List*
_temp992=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp992->hd=( void*) _temp988; _temp992->tl= rev_result; _temp992;});
rev_result=({ struct Cyc_List_List* _temp993=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp993->hd=( void*)
_temp990; _temp993->tl= rev_result; _temp993;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}}{
struct Cyc_List_List* _temp995=({ struct Cyc_List_List* _temp994=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp994->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp994->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( rev_result); _temp994;}); goto _LL996; _LL996: if( aopt != 0){
_temp995=({ struct Cyc_List_List* _temp997=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp997->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp997->tl= _temp995; _temp997;});}{
struct Cyc_Absyn_Exp* _temp998= Cyc_Absyn_primop_exp( p, _temp995, 0); goto
_LL999; _LL999: s= Cyc_Absyn_exp_stmt( _temp998, 0); for( 0; rev_temps != 0;
rev_temps=(( struct Cyc_List_List*) _check_null( rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f1,(*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f2,(
struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f3, s, 0);}}}}; _pop_region(& _temp906);} return
Cyc_Absyn_stmt_exp( s, 0);}}}} static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1001="Missing type in primop "; struct _tagged_string
_temp1002; _temp1002.curr= _temp1001; _temp1002.base= _temp1001; _temp1002.last_plus_one=
_temp1001 + 24; _temp1002;}));} return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1003="Missing type in primop "; struct _tagged_string
_temp1004; _temp1004.curr= _temp1003; _temp1004.base= _temp1003; _temp1004.last_plus_one=
_temp1003 + 24; _temp1004;}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp*
e){ return({ struct _tuple3* _temp1005=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp1005->f1= Cyc_Toc_mt_tq; _temp1005->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp1005;});} static
void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e); static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp1006=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1006[ 0]=({ struct
_tuple4 _temp1007; _temp1007.f1= 0; _temp1007.f2= e; _temp1007;}); _temp1006;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp1008= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); goto _LL1009; _LL1009: if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic( _temp1008): Cyc_Toc_malloc_ptr(
_temp1008), 0);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( rgnopt); Cyc_Toc_exp_to_c( nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, Cyc_Toc_rmalloc_exp( r, _temp1008), 0);}}} else{ t= struct_typ; eo= 0;}
return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, t, eo, s, 0), 0);} static
struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp1010=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL1011; _LL1011: for( 0;
_temp1010 != 0; _temp1010=(( struct Cyc_List_List*) _check_null( _temp1010))->tl){
struct _tuple4 _temp1014; struct Cyc_Absyn_Exp* _temp1015; struct Cyc_List_List*
_temp1017; struct _tuple4* _temp1012=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1010))->hd; _temp1014=* _temp1012; _LL1018: _temp1017=
_temp1014.f1; goto _LL1016; _LL1016: _temp1015= _temp1014.f2; goto _LL1013;
_LL1013: { struct Cyc_Absyn_Exp* e_index; if( _temp1017 == 0){ e_index= Cyc_Absyn_signed_int_exp(
count --, 0);} else{ if((( struct Cyc_List_List*) _check_null( _temp1017))->tl
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp1019="multiple designators in array"; struct _tagged_string _temp1020;
_temp1020.curr= _temp1019; _temp1020.base= _temp1019; _temp1020.last_plus_one=
_temp1019 + 30; _temp1020;}));}{ void* _temp1021=( void*)(( struct Cyc_List_List*)
_check_null( _temp1017))->hd; goto _LL1022; _LL1022: { void* _temp1023=
_temp1021; struct Cyc_Absyn_Exp* _temp1029; struct _tagged_string* _temp1031;
_LL1025: if(*(( int*) _temp1023) == Cyc_Absyn_ArrayElement){ _LL1030: _temp1029=((
struct Cyc_Absyn_ArrayElement_struct*) _temp1023)->f1; goto _LL1026;} else{ goto
_LL1027;} _LL1027: if(*(( int*) _temp1023) == Cyc_Absyn_FieldName){ _LL1032:
_temp1031=(( struct Cyc_Absyn_FieldName_struct*) _temp1023)->f1; goto _LL1028;}
else{ goto _LL1024;} _LL1026: Cyc_Toc_exp_to_c( nv, _temp1029); e_index=
_temp1029; goto _LL1024; _LL1028: e_index=(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp1033="field name designators in array";
struct _tagged_string _temp1034; _temp1034.curr= _temp1033; _temp1034.base=
_temp1033; _temp1034.last_plus_one= _temp1033 + 32; _temp1034;})); goto _LL1024;
_LL1024:;}}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index,
0); void* _temp1035=( void*) _temp1015->r; struct Cyc_List_List* _temp1045;
struct Cyc_Absyn_Exp* _temp1047; struct Cyc_Absyn_Exp* _temp1049; struct Cyc_Absyn_Vardecl*
_temp1051; struct Cyc_List_List* _temp1053; void* _temp1055; _LL1037: if(*(( int*)
_temp1035) == Cyc_Absyn_Array_e){ _LL1046: _temp1045=(( struct Cyc_Absyn_Array_e_struct*)
_temp1035)->f1; goto _LL1038;} else{ goto _LL1039;} _LL1039: if(*(( int*)
_temp1035) == Cyc_Absyn_Comprehension_e){ _LL1052: _temp1051=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1035)->f1; goto _LL1050; _LL1050: _temp1049=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1035)->f2; goto _LL1048; _LL1048: _temp1047=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1035)->f3; goto _LL1040;} else{ goto _LL1041;} _LL1041: if(*(( int*)
_temp1035) == Cyc_Absyn_AnonStruct_e){ _LL1056: _temp1055=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1035)->f1; goto _LL1054; _LL1054: _temp1053=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1035)->f2; goto _LL1042;} else{ goto _LL1043;} _LL1043: goto _LL1044;
_LL1038: s= Cyc_Toc_init_array( nv, lval, _temp1045, s); goto _LL1036; _LL1040:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1051, _temp1049, _temp1047, s, 0);
goto _LL1036; _LL1042: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1055,
_temp1053, s); goto _LL1036; _LL1044: Cyc_Toc_exp_to_c( nv, _temp1015); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1015, 0),
s, 0); goto _LL1036; _LL1036:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* _temp1057= vd->name; goto _LL1058;
_LL1058: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1057, Cyc_Absyn_varb_exp( _temp1057,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1081=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1081[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1082; _temp1082.tag= Cyc_Absyn_Local_b; _temp1082.f1=
vd; _temp1082;}); _temp1081;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp1057, 0),
Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( _temp1057, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp*
ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( _temp1057, 0), 0); struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( _temp1057, 0), 0); struct
Cyc_Absyn_Stmt* body;{ void* _temp1059=( void*) e2->r; struct Cyc_List_List*
_temp1069; struct Cyc_Absyn_Exp* _temp1071; struct Cyc_Absyn_Exp* _temp1073;
struct Cyc_Absyn_Vardecl* _temp1075; struct Cyc_List_List* _temp1077; void*
_temp1079; _LL1061: if(*(( int*) _temp1059) == Cyc_Absyn_Array_e){ _LL1070:
_temp1069=(( struct Cyc_Absyn_Array_e_struct*) _temp1059)->f1; goto _LL1062;}
else{ goto _LL1063;} _LL1063: if(*(( int*) _temp1059) == Cyc_Absyn_Comprehension_e){
_LL1076: _temp1075=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1059)->f1;
goto _LL1074; _LL1074: _temp1073=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1059)->f2; goto _LL1072; _LL1072: _temp1071=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1059)->f3; goto _LL1064;} else{ goto _LL1065;} _LL1065: if(*(( int*)
_temp1059) == Cyc_Absyn_AnonStruct_e){ _LL1080: _temp1079=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1059)->f1; goto _LL1078; _LL1078: _temp1077=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1059)->f2; goto _LL1066;} else{ goto _LL1067;} _LL1067: goto _LL1068;
_LL1062: body= Cyc_Toc_init_array( nv2, lval, _temp1069, Cyc_Absyn_skip_stmt( 0));
goto _LL1060; _LL1064: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1075,
_temp1073, _temp1071, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1060; _LL1066: body=
Cyc_Toc_init_anon_struct( nv, lval, _temp1079, _temp1077, Cyc_Absyn_skip_stmt( 0));
goto _LL1060; _LL1068: Cyc_Toc_exp_to_c( nv2, e2); body= Cyc_Absyn_assign_stmt(
lval, e2, 0); goto _LL1060; _LL1060:;}{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt(
ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( _temp1057, Cyc_Absyn_uint_t,
0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s){{ struct Cyc_List_List* _temp1083=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto _LL1084; _LL1084: for( 0;
_temp1083 != 0; _temp1083=(( struct Cyc_List_List*) _check_null( _temp1083))->tl){
struct _tuple4 _temp1087; struct Cyc_Absyn_Exp* _temp1088; struct Cyc_List_List*
_temp1090; struct _tuple4* _temp1085=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1083))->hd; _temp1087=* _temp1085; _LL1091: _temp1090=
_temp1087.f1; goto _LL1089; _LL1089: _temp1088= _temp1087.f2; goto _LL1086;
_LL1086: if( _temp1090 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1092="empty designator list"; struct _tagged_string
_temp1093; _temp1093.curr= _temp1092; _temp1093.base= _temp1092; _temp1093.last_plus_one=
_temp1092 + 22; _temp1093;}));} if((( struct Cyc_List_List*) _check_null(
_temp1090))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1094="too many designators in anonymous struct"; struct
_tagged_string _temp1095; _temp1095.curr= _temp1094; _temp1095.base= _temp1094;
_temp1095.last_plus_one= _temp1094 + 41; _temp1095;}));}{ void* _temp1096=( void*)((
struct Cyc_List_List*) _check_null( _temp1090))->hd; struct _tagged_string*
_temp1102; _LL1098: if(*(( int*) _temp1096) == Cyc_Absyn_FieldName){ _LL1103:
_temp1102=(( struct Cyc_Absyn_FieldName_struct*) _temp1096)->f1; goto _LL1099;}
else{ goto _LL1100;} _LL1100: goto _LL1101; _LL1099: { struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_structmember_exp( lhs, _temp1102, 0);{ void* _temp1104=( void*)
_temp1088->r; struct Cyc_List_List* _temp1114; struct Cyc_Absyn_Exp* _temp1116;
struct Cyc_Absyn_Exp* _temp1118; struct Cyc_Absyn_Vardecl* _temp1120; struct Cyc_List_List*
_temp1122; void* _temp1124; _LL1106: if(*(( int*) _temp1104) == Cyc_Absyn_Array_e){
_LL1115: _temp1114=(( struct Cyc_Absyn_Array_e_struct*) _temp1104)->f1; goto
_LL1107;} else{ goto _LL1108;} _LL1108: if(*(( int*) _temp1104) == Cyc_Absyn_Comprehension_e){
_LL1121: _temp1120=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1104)->f1;
goto _LL1119; _LL1119: _temp1118=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1104)->f2; goto _LL1117; _LL1117: _temp1116=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1104)->f3; goto _LL1109;} else{ goto _LL1110;} _LL1110: if(*(( int*)
_temp1104) == Cyc_Absyn_AnonStruct_e){ _LL1125: _temp1124=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1104)->f1; goto _LL1123; _LL1123: _temp1122=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1104)->f2; goto _LL1111;} else{ goto _LL1112;} _LL1112: goto _LL1113;
_LL1107: s= Cyc_Toc_init_array( nv, lval, _temp1114, s); goto _LL1105; _LL1109:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1120, _temp1118, _temp1116, s, 0);
goto _LL1105; _LL1111: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1124,
_temp1122, s); goto _LL1105; _LL1113: Cyc_Toc_exp_to_c( nv, _temp1088); if( Cyc_Toc_is_poly_field(
struct_type, _temp1102)){ _temp1088= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1088, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1088, 0), 0), s, 0); goto _LL1105; _LL1105:;} goto _LL1097;} _LL1101:(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1126="array designator in struct"; struct _tagged_string
_temp1127; _temp1127.curr= _temp1126; _temp1127.base= _temp1126; _temp1127.last_plus_one=
_temp1126 + 27; _temp1127;}))); _LL1097:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp1128= _new_region();
struct _RegionHandle* r=& _temp1128; _push_region( r);{ struct Cyc_List_List*
_temp1129=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(*
f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); goto _LL1130; _LL1130: { struct _tagged_string* _temp1131= Cyc_Toc_add_tuple_type(
_temp1129); goto _LL1132; _LL1132: { struct _tuple0* _temp1133= Cyc_Toc_temp_var();
goto _LL1134; _LL1134: { struct Cyc_Absyn_Exp* _temp1135= Cyc_Absyn_var_exp(
_temp1133, 0); goto _LL1136; _LL1136: { struct Cyc_Absyn_Stmt* _temp1137= Cyc_Absyn_exp_stmt(
_temp1135, 0); goto _LL1138; _LL1138: { struct Cyc_Absyn_Exp*(* _temp1139)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1140;
_LL1140: { int is_atomic= 1; struct Cyc_List_List* _temp1141=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es); goto
_LL1142; _LL1142:{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1141); for( 0; _temp1141 != 0;( _temp1141=(( struct Cyc_List_List*)
_check_null( _temp1141))->tl, -- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1141))->hd; struct Cyc_Absyn_Exp* lval=
_temp1139( _temp1135, Cyc_Toc_fieldname( i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v): 0;{ void* _temp1143=(
void*) e->r; struct Cyc_List_List* _temp1151; struct Cyc_Absyn_Exp* _temp1153;
struct Cyc_Absyn_Exp* _temp1155; struct Cyc_Absyn_Vardecl* _temp1157; _LL1145:
if(*(( int*) _temp1143) == Cyc_Absyn_Array_e){ _LL1152: _temp1151=(( struct Cyc_Absyn_Array_e_struct*)
_temp1143)->f1; goto _LL1146;} else{ goto _LL1147;} _LL1147: if(*(( int*)
_temp1143) == Cyc_Absyn_Comprehension_e){ _LL1158: _temp1157=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1143)->f1; goto _LL1156; _LL1156: _temp1155=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1143)->f2; goto _LL1154; _LL1154: _temp1153=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1143)->f3; goto _LL1148;} else{ goto _LL1149;} _LL1149: goto _LL1150;
_LL1146: _temp1137= Cyc_Toc_init_array( nv, lval, _temp1151, _temp1137); goto
_LL1144; _LL1148: _temp1137= Cyc_Toc_init_comprehension( nv, lval, _temp1157,
_temp1155, _temp1153, _temp1137, 0); goto _LL1144; _LL1150: Cyc_Toc_exp_to_c( nv,
e); _temp1137= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1139( _temp1135, Cyc_Toc_fieldname( i), 0), e, 0), 0), _temp1137, 0); goto
_LL1144; _LL1144:;}}}{ struct Cyc_Absyn_Exp* _temp1159= Cyc_Toc_make_struct( nv,
_temp1133, Cyc_Absyn_strct( _temp1131), _temp1137, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp1159;}}}}}}}}; _pop_region(& _temp1128);} static
struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env* nv, void*
struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp1160= Cyc_Toc_temp_var(); goto
_LL1161; _LL1161: { struct Cyc_Absyn_Exp* _temp1162= Cyc_Absyn_var_exp(
_temp1160, 0); goto _LL1163; _LL1163: { struct Cyc_Absyn_Stmt* _temp1164= Cyc_Absyn_exp_stmt(
_temp1162, 0); goto _LL1165; _LL1165: { struct Cyc_Absyn_Exp*(* _temp1166)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1167;
_LL1167: { int is_atomic= 1; struct _RegionHandle _temp1168= _new_region();
struct _RegionHandle* r=& _temp1168; _push_region( r);{ struct Cyc_List_List*
_temp1169=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, dles); goto _LL1170; _LL1170: for( 0; _temp1169 != 0;
_temp1169=(( struct Cyc_List_List*) _check_null( _temp1169))->tl){ struct
_tuple4 _temp1173; struct Cyc_Absyn_Exp* _temp1174; struct Cyc_List_List*
_temp1176; struct _tuple4* _temp1171=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1169))->hd; _temp1173=* _temp1171; _LL1177: _temp1176=
_temp1173.f1; goto _LL1175; _LL1175: _temp1174= _temp1173.f2; goto _LL1172;
_LL1172: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1174->topt))->v): 0; if( _temp1176 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1178="empty designator list";
struct _tagged_string _temp1179; _temp1179.curr= _temp1178; _temp1179.base=
_temp1178; _temp1179.last_plus_one= _temp1178 + 22; _temp1179;}));} if((( struct
Cyc_List_List*) _check_null( _temp1176))->tl != 0){ struct _tuple0* _temp1180=
Cyc_Toc_temp_var(); goto _LL1181; _LL1181: { struct Cyc_Absyn_Exp* _temp1182=
Cyc_Absyn_var_exp( _temp1180, 0); goto _LL1183; _LL1183: for( 0; _temp1176 != 0;
_temp1176=(( struct Cyc_List_List*) _check_null( _temp1176))->tl){ void*
_temp1184=( void*)(( struct Cyc_List_List*) _check_null( _temp1176))->hd; struct
_tagged_string* _temp1190; _LL1186: if(*(( int*) _temp1184) == Cyc_Absyn_FieldName){
_LL1191: _temp1190=(( struct Cyc_Absyn_FieldName_struct*) _temp1184)->f1; goto
_LL1187;} else{ goto _LL1188;} _LL1188: goto _LL1189; _LL1187: if( Cyc_Toc_is_poly_field(
struct_type, _temp1190)){ _temp1182= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1182, 0);} _temp1164= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1166( _temp1162, _temp1190, 0), _temp1182, 0), 0), _temp1164, 0); goto
_LL1185; _LL1189:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1192="array designator in struct"; struct _tagged_string
_temp1193; _temp1193.curr= _temp1192; _temp1193.base= _temp1192; _temp1193.last_plus_one=
_temp1192 + 27; _temp1193;}))); _LL1185:;} Cyc_Toc_exp_to_c( nv, _temp1174);
_temp1164= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1182, _temp1174, 0), 0), _temp1164, 0);}} else{ void* _temp1194=( void*)((
struct Cyc_List_List*) _check_null( _temp1176))->hd; struct _tagged_string*
_temp1200; _LL1196: if(*(( int*) _temp1194) == Cyc_Absyn_FieldName){ _LL1201:
_temp1200=(( struct Cyc_Absyn_FieldName_struct*) _temp1194)->f1; goto _LL1197;}
else{ goto _LL1198;} _LL1198: goto _LL1199; _LL1197: { struct Cyc_Absyn_Exp*
lval= _temp1166( _temp1162, _temp1200, 0);{ void* _temp1202=( void*) _temp1174->r;
struct Cyc_List_List* _temp1212; struct Cyc_Absyn_Exp* _temp1214; struct Cyc_Absyn_Exp*
_temp1216; struct Cyc_Absyn_Vardecl* _temp1218; struct Cyc_List_List* _temp1220;
void* _temp1222; _LL1204: if(*(( int*) _temp1202) == Cyc_Absyn_Array_e){ _LL1213:
_temp1212=(( struct Cyc_Absyn_Array_e_struct*) _temp1202)->f1; goto _LL1205;}
else{ goto _LL1206;} _LL1206: if(*(( int*) _temp1202) == Cyc_Absyn_Comprehension_e){
_LL1219: _temp1218=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1202)->f1;
goto _LL1217; _LL1217: _temp1216=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1202)->f2; goto _LL1215; _LL1215: _temp1214=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1202)->f3; goto _LL1207;} else{ goto _LL1208;} _LL1208: if(*(( int*)
_temp1202) == Cyc_Absyn_AnonStruct_e){ _LL1223: _temp1222=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1202)->f1; goto _LL1221; _LL1221: _temp1220=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1202)->f2; goto _LL1209;} else{ goto _LL1210;} _LL1210: goto _LL1211;
_LL1205: _temp1164= Cyc_Toc_init_array( nv, lval, _temp1212, _temp1164); goto
_LL1203; _LL1207: _temp1164= Cyc_Toc_init_comprehension( nv, lval, _temp1218,
_temp1216, _temp1214, _temp1164, 0); goto _LL1203; _LL1209: _temp1164= Cyc_Toc_init_anon_struct(
nv, lval, _temp1222, _temp1220, _temp1164); goto _LL1203; _LL1211: Cyc_Toc_exp_to_c(
nv, _temp1174); if( Cyc_Toc_is_poly_field( struct_type, _temp1200)){ _temp1174=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1174, 0);} _temp1164= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1174, 0), 0), _temp1164, 0);
goto _LL1203; _LL1203:;} goto _LL1195;} _LL1199:( void) _throw((( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1224="array designator in struct";
struct _tagged_string _temp1225; _temp1225.curr= _temp1224; _temp1225.base=
_temp1224; _temp1225.last_plus_one= _temp1224 + 27; _temp1225;}))); _LL1195:;}}}{
struct Cyc_Absyn_Exp* _temp1226= Cyc_Toc_make_struct( nv, _temp1160, Cyc_Absyn_strctq(
tdn), _temp1164, pointer, rgnopt, is_atomic); _npop_handler( 0u); return
_temp1226;}; _pop_region(& _temp1168);}}}}} struct _tuple11{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp(
el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1227=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1227[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1228; _temp1228.tag=
Cyc_Absyn_Increment_e; _temp1228.f1= e1; _temp1228.f2=( void*) incr; _temp1228;});
_temp1227;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1229=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1239; struct Cyc_Absyn_Exp* _temp1241; void* _temp1243; struct
_tagged_string* _temp1245; struct Cyc_Absyn_Exp* _temp1247; _LL1231: if(*(( int*)
_temp1229) == Cyc_Absyn_StmtExp_e){ _LL1240: _temp1239=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1229)->f1; goto _LL1232;} else{ goto _LL1233;} _LL1233: if(*(( int*)
_temp1229) == Cyc_Absyn_Cast_e){ _LL1244: _temp1243=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1229)->f1; goto _LL1242; _LL1242: _temp1241=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1229)->f2; goto _LL1234;} else{ goto _LL1235;} _LL1235: if(*(( int*)
_temp1229) == Cyc_Absyn_StructMember_e){ _LL1248: _temp1247=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1229)->f1; goto _LL1246; _LL1246: _temp1245=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1229)->f2; goto _LL1236;} else{ goto _LL1237;} _LL1237: goto _LL1238;
_LL1232:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1239, fs, f, f_env); goto _LL1230; _LL1234:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1241, fs, f, f_env); goto
_LL1230; _LL1236:( void*)( e1->r=( void*)(( void*) _temp1247->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1249=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1249->hd=( void*) _temp1245; _temp1249->tl= fs;
_temp1249;}), f, f_env); goto _LL1230; _LL1238: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1250= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1250.last_plus_one - _temp1250.curr, _temp1250.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1230;}
_LL1230:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1251=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1261; struct Cyc_Absyn_Stmt* _temp1263; struct Cyc_Absyn_Decl* _temp1265;
struct Cyc_Absyn_Stmt* _temp1267; struct Cyc_Absyn_Stmt* _temp1269; _LL1253: if((
unsigned int) _temp1251 > 1u?*(( int*) _temp1251) == Cyc_Absyn_Exp_s: 0){
_LL1262: _temp1261=(( struct Cyc_Absyn_Exp_s_struct*) _temp1251)->f1; goto
_LL1254;} else{ goto _LL1255;} _LL1255: if(( unsigned int) _temp1251 > 1u?*((
int*) _temp1251) == Cyc_Absyn_Decl_s: 0){ _LL1266: _temp1265=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1251)->f1; goto _LL1264; _LL1264: _temp1263=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1251)->f2; goto _LL1256;} else{ goto _LL1257;} _LL1257: if(( unsigned int)
_temp1251 > 1u?*(( int*) _temp1251) == Cyc_Absyn_Seq_s: 0){ _LL1270: _temp1269=((
struct Cyc_Absyn_Seq_s_struct*) _temp1251)->f1; goto _LL1268; _LL1268: _temp1267=((
struct Cyc_Absyn_Seq_s_struct*) _temp1251)->f2; goto _LL1258;} else{ goto
_LL1259;} _LL1259: goto _LL1260; _LL1254:(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1261, fs, f, f_env); goto
_LL1252; _LL1256:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1263, fs, f, f_env); goto _LL1252; _LL1258:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1267, fs, f, f_env);
goto _LL1252; _LL1260:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1271= Cyc_Absynpp_stmt2string( s); xprintf("lvalue_assign_stmt: %.*s",
_temp1271.last_plus_one - _temp1271.curr, _temp1271.curr);})); goto _LL1252;
_LL1252:;} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct _RegionHandle* r2, void*(*
f)( void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List*
_temp1272=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp1272->hd=( void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,
env); _temp1272->tl= 0; _temp1272;}); prev= result; for( x=(( struct Cyc_List_List*)
_check_null( x))->tl; x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){((
struct Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List*
_temp1273=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp1273->hd=( void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,
env); _temp1273->tl= 0; _temp1273;}); prev=(( struct Cyc_List_List*) _check_null(
prev))->tl;} return result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(*
f)( void*, void*), void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4*
Cyc_Toc_make_dle( struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1274=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1274->f1= 0;
_temp1274->f2= e; _temp1274;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void* t){ void* _temp1275= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp1281; _LL1277: if(( unsigned int) _temp1275 > 4u?*(( int*) _temp1275) ==
Cyc_Absyn_PointerType: 0){ _LL1282: _temp1281=(( struct Cyc_Absyn_PointerType_struct*)
_temp1275)->f1; goto _LL1278;} else{ goto _LL1279;} _LL1279: goto _LL1280;
_LL1278: return _temp1281; _LL1280:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp1283="get_ptr_typ:  not a pointer!";
struct _tagged_string _temp1284; _temp1284.curr= _temp1283; _temp1284.base=
_temp1283; _temp1284.last_plus_one= _temp1283 + 29; _temp1284;}))); _LL1276:;}
struct _tagged_ptr5{ struct _tuple0** curr; struct _tuple0** base; struct
_tuple0** last_plus_one; } ; struct _tagged_ptr6{ struct _tuple1** curr; struct
_tuple1** base; struct _tuple1** last_plus_one; } ; struct _tagged_ptr7{ struct
Cyc_Absyn_Exp** curr; struct Cyc_Absyn_Exp** base; struct Cyc_Absyn_Exp**
last_plus_one; } ; struct _tuple12{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp*
f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ void* _temp1285=( void*) e->r; goto _LL1286; _LL1286: if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1287= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1287.last_plus_one - _temp1287.curr, _temp1287.curr);})); return;}{ void*
old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void*
_temp1288= _temp1285; void* _temp1366; void* _temp1368; void* _temp1370; struct
_tuple0* _temp1372; struct _tuple0* _temp1374; struct Cyc_List_List* _temp1376;
void* _temp1378; void* _temp1380; struct Cyc_Absyn_Exp* _temp1382; struct Cyc_Absyn_Exp*
_temp1384; struct Cyc_Core_Opt* _temp1386; struct Cyc_Absyn_Exp* _temp1388;
struct Cyc_Absyn_Exp* _temp1390; struct Cyc_Absyn_Exp* _temp1392; struct Cyc_Absyn_Exp*
_temp1394; struct Cyc_Absyn_Exp* _temp1396; struct Cyc_Absyn_Exp* _temp1398;
struct Cyc_List_List* _temp1400; struct Cyc_Absyn_Exp* _temp1402; struct Cyc_Absyn_VarargCallInfo*
_temp1404; struct Cyc_List_List* _temp1406; struct Cyc_Absyn_Exp* _temp1408;
struct Cyc_Absyn_VarargCallInfo* _temp1410; struct Cyc_Absyn_VarargCallInfo
_temp1412; struct Cyc_Absyn_VarargInfo* _temp1413; struct Cyc_List_List*
_temp1415; int _temp1417; struct Cyc_List_List* _temp1419; struct Cyc_Absyn_Exp*
_temp1421; struct Cyc_Absyn_Exp* _temp1423; struct Cyc_Absyn_Exp* _temp1425;
struct Cyc_List_List* _temp1427; struct Cyc_Absyn_Exp* _temp1429; struct Cyc_Absyn_Exp*
_temp1431; void* _temp1433; void** _temp1435; struct Cyc_Absyn_Exp* _temp1436;
struct Cyc_Absyn_Exp* _temp1438; struct Cyc_Absyn_Exp* _temp1440; void*
_temp1442; struct Cyc_Absyn_Exp* _temp1444; struct Cyc_Absyn_Exp* _temp1446;
struct _tagged_string* _temp1448; struct Cyc_Absyn_Exp* _temp1450; struct
_tagged_string* _temp1452; struct Cyc_Absyn_Exp* _temp1454; struct Cyc_Absyn_Exp*
_temp1456; struct Cyc_Absyn_Exp* _temp1458; struct Cyc_List_List* _temp1460;
struct Cyc_List_List* _temp1462; struct _tuple1* _temp1464; struct Cyc_List_List*
_temp1466; struct Cyc_Absyn_Exp* _temp1468; struct Cyc_Absyn_Exp* _temp1470;
struct Cyc_Absyn_Vardecl* _temp1472; struct Cyc_Absyn_Structdecl* _temp1474;
struct Cyc_List_List* _temp1476; struct Cyc_Core_Opt* _temp1478; struct _tuple0*
_temp1480; struct Cyc_List_List* _temp1482; void* _temp1484; struct Cyc_Absyn_Tunionfield*
_temp1486; struct Cyc_Absyn_Tuniondecl* _temp1488; struct Cyc_List_List*
_temp1490; struct Cyc_Core_Opt* _temp1492; struct Cyc_Core_Opt* _temp1494;
struct Cyc_Absyn_Tunionfield* _temp1496; struct Cyc_Absyn_Tuniondecl* _temp1498;
struct Cyc_List_List* _temp1500; struct Cyc_Core_Opt* _temp1502; struct Cyc_Core_Opt*
_temp1504; struct Cyc_Absyn_Enumfield* _temp1506; struct Cyc_Absyn_Enumdecl*
_temp1508; struct _tuple0* _temp1510; void* _temp1512; struct Cyc_Absyn_Exp*
_temp1514; struct Cyc_Absyn_Stmt* _temp1516; struct Cyc_List_List* _temp1518;
struct Cyc_Core_Opt* _temp1520; struct Cyc_Absyn_Fndecl* _temp1522; struct Cyc_Absyn_Exp*
_temp1524; _LL1290: if(*(( int*) _temp1288) == Cyc_Absyn_Const_e){ _LL1367:
_temp1366=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1288)->f1; if(
_temp1366 ==( void*) Cyc_Absyn_Null_c){ goto _LL1291;} else{ goto _LL1292;}}
else{ goto _LL1292;} _LL1292: if(*(( int*) _temp1288) == Cyc_Absyn_Const_e){
_LL1369: _temp1368=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1288)->f1;
goto _LL1293;} else{ goto _LL1294;} _LL1294: if(*(( int*) _temp1288) == Cyc_Absyn_Var_e){
_LL1373: _temp1372=(( struct Cyc_Absyn_Var_e_struct*) _temp1288)->f1; goto
_LL1371; _LL1371: _temp1370=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1288)->f2;
goto _LL1295;} else{ goto _LL1296;} _LL1296: if(*(( int*) _temp1288) == Cyc_Absyn_UnknownId_e){
_LL1375: _temp1374=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1288)->f1; goto
_LL1297;} else{ goto _LL1298;} _LL1298: if(*(( int*) _temp1288) == Cyc_Absyn_Primop_e){
_LL1379: _temp1378=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1288)->f1;
goto _LL1377; _LL1377: _temp1376=(( struct Cyc_Absyn_Primop_e_struct*) _temp1288)->f2;
goto _LL1299;} else{ goto _LL1300;} _LL1300: if(*(( int*) _temp1288) == Cyc_Absyn_Increment_e){
_LL1383: _temp1382=(( struct Cyc_Absyn_Increment_e_struct*) _temp1288)->f1; goto
_LL1381; _LL1381: _temp1380=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1288)->f2; goto _LL1301;} else{ goto _LL1302;} _LL1302: if(*(( int*)
_temp1288) == Cyc_Absyn_AssignOp_e){ _LL1389: _temp1388=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1288)->f1; goto _LL1387; _LL1387: _temp1386=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1288)->f2; goto _LL1385; _LL1385: _temp1384=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1288)->f3; goto _LL1303;} else{ goto _LL1304;} _LL1304: if(*(( int*)
_temp1288) == Cyc_Absyn_Conditional_e){ _LL1395: _temp1394=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1288)->f1; goto _LL1393; _LL1393: _temp1392=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1288)->f2; goto _LL1391; _LL1391: _temp1390=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1288)->f3; goto _LL1305;} else{ goto _LL1306;} _LL1306: if(*(( int*)
_temp1288) == Cyc_Absyn_SeqExp_e){ _LL1399: _temp1398=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1288)->f1; goto _LL1397; _LL1397: _temp1396=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1288)->f2; goto _LL1307;} else{ goto _LL1308;} _LL1308: if(*(( int*)
_temp1288) == Cyc_Absyn_UnknownCall_e){ _LL1403: _temp1402=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1288)->f1; goto _LL1401; _LL1401: _temp1400=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1288)->f2; goto _LL1309;} else{ goto _LL1310;} _LL1310: if(*(( int*)
_temp1288) == Cyc_Absyn_FnCall_e){ _LL1409: _temp1408=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1288)->f1; goto _LL1407; _LL1407: _temp1406=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1288)->f2; goto _LL1405; _LL1405: _temp1404=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1288)->f3; if( _temp1404 == 0){ goto _LL1311;} else{ goto _LL1312;}} else{
goto _LL1312;} _LL1312: if(*(( int*) _temp1288) == Cyc_Absyn_FnCall_e){ _LL1422:
_temp1421=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1288)->f1; goto _LL1420;
_LL1420: _temp1419=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1288)->f2; goto
_LL1411; _LL1411: _temp1410=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1288)->f3;
if( _temp1410 == 0){ goto _LL1314;} else{ _temp1412=* _temp1410; _LL1418:
_temp1417= _temp1412.num_varargs; goto _LL1416; _LL1416: _temp1415= _temp1412.injectors;
goto _LL1414; _LL1414: _temp1413= _temp1412.vai; goto _LL1313;}} else{ goto
_LL1314;} _LL1314: if(*(( int*) _temp1288) == Cyc_Absyn_Throw_e){ _LL1424:
_temp1423=(( struct Cyc_Absyn_Throw_e_struct*) _temp1288)->f1; goto _LL1315;}
else{ goto _LL1316;} _LL1316: if(*(( int*) _temp1288) == Cyc_Absyn_NoInstantiate_e){
_LL1426: _temp1425=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1288)->f1;
goto _LL1317;} else{ goto _LL1318;} _LL1318: if(*(( int*) _temp1288) == Cyc_Absyn_Instantiate_e){
_LL1430: _temp1429=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1288)->f1;
goto _LL1428; _LL1428: _temp1427=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1288)->f2; goto _LL1319;} else{ goto _LL1320;} _LL1320: if(*(( int*)
_temp1288) == Cyc_Absyn_Cast_e){ _LL1434: _temp1433=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1288)->f1; _temp1435=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1288)->f1); goto _LL1432; _LL1432: _temp1431=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1288)->f2; goto _LL1321;} else{ goto _LL1322;} _LL1322: if(*(( int*)
_temp1288) == Cyc_Absyn_Address_e){ _LL1437: _temp1436=(( struct Cyc_Absyn_Address_e_struct*)
_temp1288)->f1; goto _LL1323;} else{ goto _LL1324;} _LL1324: if(*(( int*)
_temp1288) == Cyc_Absyn_New_e){ _LL1441: _temp1440=(( struct Cyc_Absyn_New_e_struct*)
_temp1288)->f1; goto _LL1439; _LL1439: _temp1438=(( struct Cyc_Absyn_New_e_struct*)
_temp1288)->f2; goto _LL1325;} else{ goto _LL1326;} _LL1326: if(*(( int*)
_temp1288) == Cyc_Absyn_Sizeoftyp_e){ _LL1443: _temp1442=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1288)->f1; goto _LL1327;} else{ goto _LL1328;} _LL1328: if(*(( int*)
_temp1288) == Cyc_Absyn_Sizeofexp_e){ _LL1445: _temp1444=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1288)->f1; goto _LL1329;} else{ goto _LL1330;} _LL1330: if(*(( int*)
_temp1288) == Cyc_Absyn_Deref_e){ _LL1447: _temp1446=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1288)->f1; goto _LL1331;} else{ goto _LL1332;} _LL1332: if(*(( int*)
_temp1288) == Cyc_Absyn_StructMember_e){ _LL1451: _temp1450=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1288)->f1; goto _LL1449; _LL1449: _temp1448=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1288)->f2; goto _LL1333;} else{ goto _LL1334;} _LL1334: if(*(( int*)
_temp1288) == Cyc_Absyn_StructArrow_e){ _LL1455: _temp1454=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1288)->f1; goto _LL1453; _LL1453: _temp1452=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1288)->f2; goto _LL1335;} else{ goto _LL1336;} _LL1336: if(*(( int*)
_temp1288) == Cyc_Absyn_Subscript_e){ _LL1459: _temp1458=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1288)->f1; goto _LL1457; _LL1457: _temp1456=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1288)->f2; goto _LL1337;} else{ goto _LL1338;} _LL1338: if(*(( int*)
_temp1288) == Cyc_Absyn_Tuple_e){ _LL1461: _temp1460=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1288)->f1; goto _LL1339;} else{ goto _LL1340;} _LL1340: if(*(( int*)
_temp1288) == Cyc_Absyn_CompoundLit_e){ _LL1465: _temp1464=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1288)->f1; goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1288)->f2; goto _LL1341;} else{ goto _LL1342;} _LL1342: if(*(( int*)
_temp1288) == Cyc_Absyn_Array_e){ _LL1467: _temp1466=(( struct Cyc_Absyn_Array_e_struct*)
_temp1288)->f1; goto _LL1343;} else{ goto _LL1344;} _LL1344: if(*(( int*)
_temp1288) == Cyc_Absyn_Comprehension_e){ _LL1473: _temp1472=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1288)->f1; goto _LL1471; _LL1471: _temp1470=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1288)->f2; goto _LL1469; _LL1469: _temp1468=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1288)->f3; goto _LL1345;} else{ goto _LL1346;} _LL1346: if(*(( int*)
_temp1288) == Cyc_Absyn_Struct_e){ _LL1481: _temp1480=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1288)->f1; goto _LL1479; _LL1479: _temp1478=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1288)->f2; goto _LL1477; _LL1477: _temp1476=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1288)->f3; goto _LL1475; _LL1475: _temp1474=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1288)->f4; goto _LL1347;} else{ goto _LL1348;} _LL1348: if(*(( int*)
_temp1288) == Cyc_Absyn_AnonStruct_e){ _LL1485: _temp1484=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1288)->f1; goto _LL1483; _LL1483: _temp1482=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1288)->f2; goto _LL1349;} else{ goto _LL1350;} _LL1350: if(*(( int*)
_temp1288) == Cyc_Absyn_Tunion_e){ _LL1495: _temp1494=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1288)->f1; goto _LL1493; _LL1493: _temp1492=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1288)->f2; goto _LL1491; _LL1491: _temp1490=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1288)->f3; if( _temp1490 == 0){ goto _LL1489;} else{ goto _LL1352;} _LL1489:
_temp1488=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1288)->f4; goto _LL1487;
_LL1487: _temp1486=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1288)->f5; goto
_LL1351;} else{ goto _LL1352;} _LL1352: if(*(( int*) _temp1288) == Cyc_Absyn_Tunion_e){
_LL1505: _temp1504=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1288)->f1; goto
_LL1503; _LL1503: _temp1502=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1288)->f2;
goto _LL1501; _LL1501: _temp1500=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1288)->f3;
goto _LL1499; _LL1499: _temp1498=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1288)->f4;
goto _LL1497; _LL1497: _temp1496=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1288)->f5;
goto _LL1353;} else{ goto _LL1354;} _LL1354: if(*(( int*) _temp1288) == Cyc_Absyn_Enum_e){
_LL1511: _temp1510=(( struct Cyc_Absyn_Enum_e_struct*) _temp1288)->f1; goto
_LL1509; _LL1509: _temp1508=(( struct Cyc_Absyn_Enum_e_struct*) _temp1288)->f2;
goto _LL1507; _LL1507: _temp1506=(( struct Cyc_Absyn_Enum_e_struct*) _temp1288)->f3;
goto _LL1355;} else{ goto _LL1356;} _LL1356: if(*(( int*) _temp1288) == Cyc_Absyn_Malloc_e){
_LL1515: _temp1514=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1288)->f1; goto
_LL1513; _LL1513: _temp1512=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1288)->f2; goto _LL1357;} else{ goto _LL1358;} _LL1358: if(*(( int*)
_temp1288) == Cyc_Absyn_StmtExp_e){ _LL1517: _temp1516=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1288)->f1; goto _LL1359;} else{ goto _LL1360;} _LL1360: if(*(( int*)
_temp1288) == Cyc_Absyn_UnresolvedMem_e){ _LL1521: _temp1520=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1288)->f1; goto _LL1519; _LL1519: _temp1518=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1288)->f2; goto _LL1361;} else{ goto _LL1362;} _LL1362: if(*(( int*)
_temp1288) == Cyc_Absyn_Codegen_e){ _LL1523: _temp1522=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1288)->f1; goto _LL1363;} else{ goto _LL1364;} _LL1364: if(*(( int*)
_temp1288) == Cyc_Absyn_Fill_e){ _LL1525: _temp1524=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1288)->f1; goto _LL1365;} else{ goto _LL1289;} _LL1291:{ void* _temp1526=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1532; struct Cyc_Absyn_Conref*
_temp1534; struct Cyc_Absyn_Tqual _temp1536; struct Cyc_Absyn_Conref* _temp1538;
void* _temp1540; void* _temp1542; _LL1528: if(( unsigned int) _temp1526 > 4u?*((
int*) _temp1526) == Cyc_Absyn_PointerType: 0){ _LL1533: _temp1532=(( struct Cyc_Absyn_PointerType_struct*)
_temp1526)->f1; _LL1543: _temp1542=( void*) _temp1532.elt_typ; goto _LL1541;
_LL1541: _temp1540=( void*) _temp1532.rgn_typ; goto _LL1539; _LL1539: _temp1538=
_temp1532.nullable; goto _LL1537; _LL1537: _temp1536= _temp1532.tq; goto _LL1535;
_LL1535: _temp1534= _temp1532.bounds; goto _LL1529;} else{ goto _LL1530;}
_LL1530: goto _LL1531; _LL1529: if(( void*) _temp1534->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1527;}{ void* _temp1544=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1534); struct Cyc_Absyn_Exp* _temp1550; _LL1546: if( _temp1544 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1547;} else{ goto _LL1548;} _LL1548: if((
unsigned int) _temp1544 > 1u?*(( int*) _temp1544) == Cyc_Absyn_Upper_b: 0){
_LL1551: _temp1550=(( struct Cyc_Absyn_Upper_b_struct*) _temp1544)->f1; goto
_LL1549;} else{ goto _LL1545;} _LL1547: { struct _tuple4* _temp1553=({ struct
_tuple4* _temp1552=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1552->f1= 0; _temp1552->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1552;});
goto _LL1554; _LL1554: { struct Cyc_Absyn_Exp* _temp1561= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List*(* _temp1555)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)(
struct _tagged_ptr3)) Cyc_List_list; struct _tuple4* _temp1558= _temp1553;
struct _tuple4* _temp1559= _temp1553; struct _tuple4* _temp1560= _temp1553;
struct _tuple4* _temp1556[ 3u]={ _temp1558, _temp1559, _temp1560}; struct
_tagged_ptr3 _temp1557={ _temp1556, _temp1556, _temp1556 + 3u}; _temp1555(
_temp1557);}), 0); goto _LL1562; _LL1562: if( nv->toplevel){( void*)( e->r=(
void*)(( void*) _temp1561->r));} else{ void* _temp1563= Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1542), Cyc_Toc_mt_tq); goto _LL1564; _LL1564: { struct
_tuple0* _temp1565= Cyc_Toc_temp_var(); goto _LL1566; _LL1566: { struct Cyc_Absyn_Exp*
_temp1567= Cyc_Absyn_var_exp( _temp1565, 0); goto _LL1568; _LL1568: { struct Cyc_Absyn_Stmt*
_temp1569= Cyc_Absyn_declare_stmt( _temp1565, _temp1563,( struct Cyc_Absyn_Exp*)
_temp1561, Cyc_Absyn_exp_stmt( _temp1567, 0), 0); goto _LL1570; _LL1570:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1569, 0))->r));}}}} goto
_LL1545;}} _LL1549:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0,
0))->r)); goto _LL1545; _LL1545:;} goto _LL1527; _LL1531:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1571= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1571.last_plus_one -
_temp1571.curr, _temp1571.curr);})); goto _LL1527; _LL1527:;} goto _LL1289;
_LL1293: goto _LL1289; _LL1295:{ struct _handler_cons _temp1572; _push_handler(&
_temp1572);{ int _temp1574= 0; if( setjmp( _temp1572.handler)){ _temp1574= 1;}
if( ! _temp1574){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1372))->r));; _pop_handler();} else{ void* _temp1573=( void*) _exn_thrown;
void* _temp1576= _temp1573; _LL1578: if( _temp1576 == Cyc_Dict_Absent){ goto
_LL1579;} else{ goto _LL1580;} _LL1580: goto _LL1581; _LL1579:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1582= Cyc_Absynpp_qvar2string(
_temp1372); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1582.last_plus_one
- _temp1582.curr, _temp1582.curr);})); return; _LL1581:( void) _throw( _temp1576);
_LL1577:;}}} goto _LL1289; _LL1297:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1583="unknownid"; struct _tagged_string _temp1584; _temp1584.curr=
_temp1583; _temp1584.base= _temp1583; _temp1584.last_plus_one= _temp1583 + 10;
_temp1584;})); goto _LL1289; _LL1299: { struct Cyc_List_List* _temp1585=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp1376); goto _LL1586; _LL1586: {
struct Cyc_List_List* _temp1587=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1376); goto _LL1588; _LL1588:(( void(*)( void(* f)( struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)(
Cyc_Toc_exp_to_c, nv, _temp1376);{ void* _temp1589= _temp1378; _LL1591: if(
_temp1589 ==( void*) Cyc_Absyn_Size){ goto _LL1592;} else{ goto _LL1593;}
_LL1593: if( _temp1589 ==( void*) Cyc_Absyn_Printf){ goto _LL1594;} else{ goto
_LL1595;} _LL1595: if( _temp1589 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1596;}
else{ goto _LL1597;} _LL1597: if( _temp1589 ==( void*) Cyc_Absyn_Fprintf){ goto
_LL1598;} else{ goto _LL1599;} _LL1599: if( _temp1589 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1600;} else{ goto _LL1601;} _LL1601: if( _temp1589 ==( void*) Cyc_Absyn_Plus){
goto _LL1602;} else{ goto _LL1603;} _LL1603: if( _temp1589 ==( void*) Cyc_Absyn_Minus){
goto _LL1604;} else{ goto _LL1605;} _LL1605: if( _temp1589 ==( void*) Cyc_Absyn_Eq){
goto _LL1606;} else{ goto _LL1607;} _LL1607: if( _temp1589 ==( void*) Cyc_Absyn_Neq){
goto _LL1608;} else{ goto _LL1609;} _LL1609: if( _temp1589 ==( void*) Cyc_Absyn_Gt){
goto _LL1610;} else{ goto _LL1611;} _LL1611: if( _temp1589 ==( void*) Cyc_Absyn_Gte){
goto _LL1612;} else{ goto _LL1613;} _LL1613: if( _temp1589 ==( void*) Cyc_Absyn_Lt){
goto _LL1614;} else{ goto _LL1615;} _LL1615: if( _temp1589 ==( void*) Cyc_Absyn_Lte){
goto _LL1616;} else{ goto _LL1617;} _LL1617: goto _LL1618; _LL1592: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1376))->hd;{
void* _temp1619=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1627; struct Cyc_Absyn_Tqual _temp1629; void*
_temp1631; struct Cyc_Absyn_PtrInfo _temp1633; struct Cyc_Absyn_Conref*
_temp1635; struct Cyc_Absyn_Tqual _temp1637; struct Cyc_Absyn_Conref* _temp1639;
void* _temp1641; void* _temp1643; _LL1621: if(( unsigned int) _temp1619 > 4u?*((
int*) _temp1619) == Cyc_Absyn_ArrayType: 0){ _LL1632: _temp1631=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1619)->f1; goto _LL1630; _LL1630:
_temp1629=(( struct Cyc_Absyn_ArrayType_struct*) _temp1619)->f2; goto _LL1628;
_LL1628: _temp1627=(( struct Cyc_Absyn_ArrayType_struct*) _temp1619)->f3; goto
_LL1622;} else{ goto _LL1623;} _LL1623: if(( unsigned int) _temp1619 > 4u?*((
int*) _temp1619) == Cyc_Absyn_PointerType: 0){ _LL1634: _temp1633=(( struct Cyc_Absyn_PointerType_struct*)
_temp1619)->f1; _LL1644: _temp1643=( void*) _temp1633.elt_typ; goto _LL1642;
_LL1642: _temp1641=( void*) _temp1633.rgn_typ; goto _LL1640; _LL1640: _temp1639=
_temp1633.nullable; goto _LL1638; _LL1638: _temp1637= _temp1633.tq; goto _LL1636;
_LL1636: _temp1635= _temp1633.bounds; goto _LL1624;} else{ goto _LL1625;}
_LL1625: goto _LL1626; _LL1622:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1627))->r)); goto _LL1620; _LL1624: { struct _tuple0*
_temp1645= Cyc_Toc_temp_var(); goto _LL1646; _LL1646: { void* _temp1647= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); goto _LL1648;
_LL1648: { struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1645, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1645, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4=
Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp( e2, e3, 0), 0);
struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1645, _temp1647,(
struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1620;}}} _LL1626:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1649= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*) _check_null(
_temp1587))->hd); struct _tagged_string _temp1650= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1649.last_plus_one - _temp1649.curr, _temp1649.curr, _temp1650.last_plus_one
- _temp1650.curr, _temp1650.curr);})); return; _LL1620:;} goto _LL1590;} _LL1594:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1378, 0, _temp1376,
_temp1587))->r)); goto _LL1590; _LL1596:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1378, 0, _temp1376, _temp1587))->r)); goto _LL1590; _LL1598: if( _temp1376
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char*
_temp1651="fprintf without arguments"; struct _tagged_string _temp1652;
_temp1652.curr= _temp1651; _temp1652.base= _temp1651; _temp1652.last_plus_one=
_temp1651 + 26; _temp1652;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1378,({ struct Cyc_Core_Opt* _temp1653=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1653->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1376))->hd); _temp1653;}),(( struct Cyc_List_List*)
_check_null( _temp1376))->tl,(( struct Cyc_List_List*) _check_null( _temp1587))->tl))->r));
goto _LL1590; _LL1600: if( _temp1376 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1654="sscanf without arguments";
struct _tagged_string _temp1655; _temp1655.curr= _temp1654; _temp1655.base=
_temp1654; _temp1655.last_plus_one= _temp1654 + 25; _temp1655;}));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1376))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1376))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1376))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1590; _LL1602: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1376))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1376))->tl))->hd; struct _tuple0* _temp1656= Cyc_Toc_temp_var(); goto
_LL1657; _LL1657: { void* _temp1658= Cyc_Toc_typ_to_c( old_typ); goto _LL1659;
_LL1659: { struct Cyc_Absyn_Stmt* _temp1660= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1656, 0), 0); goto _LL1661; _LL1661: { struct Cyc_Absyn_Exp* _temp1662= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1656, 0), Cyc_Toc_curr_sp, 0); goto _LL1663; _LL1663: {
struct Cyc_Absyn_Exp* _temp1665= Cyc_Absyn_assignop_exp( _temp1662,({ struct Cyc_Core_Opt*
_temp1664=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1664->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1664;}), e2, 0); goto
_LL1666; _LL1666: _temp1660= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1665,
0), _temp1660, 0); _temp1660= Cyc_Absyn_declare_stmt( _temp1656, _temp1658,(
struct Cyc_Absyn_Exp*) e1, _temp1660, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1660, 0))->r));}}}}} goto _LL1590; _LL1604: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null( _temp1585))->hd)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1376))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1376))->tl))->hd; void*
old_typ_e2=( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1585))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* _temp1667= Cyc_Toc_temp_var(); goto _LL1668;
_LL1668: { void* _temp1669= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_List_List*)
_check_null( _temp1585))->hd); goto _LL1670; _LL1670: { struct Cyc_Absyn_Stmt*
_temp1671= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1667, 0), 0); goto
_LL1672; _LL1672: { struct Cyc_Absyn_Exp* _temp1673= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1667, 0), Cyc_Toc_curr_sp, 0); goto _LL1674; _LL1674: {
struct Cyc_Absyn_Exp* _temp1676= Cyc_Absyn_assignop_exp( _temp1673,({ struct Cyc_Core_Opt*
_temp1675=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1675->v=( void*)(( void*) Cyc_Absyn_Minus); _temp1675;}), e2, 0); goto
_LL1677; _LL1677: _temp1671= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1676,
0), _temp1671, 0); _temp1671= Cyc_Absyn_declare_stmt( _temp1667, _temp1669,(
struct Cyc_Absyn_Exp*) e1, _temp1671, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1671, 0))->r));}}}}}} goto _LL1590; _LL1606: goto _LL1608; _LL1608: goto
_LL1610; _LL1610: goto _LL1612; _LL1612: goto _LL1614; _LL1614: goto _LL1616;
_LL1616: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1376))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1376))->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp1585))->hd; void* t2=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1585))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1590;} _LL1618: goto
_LL1590; _LL1590:;} goto _LL1289;}} _LL1301: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1382->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1382); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1380 ==(
void*) Cyc_Absyn_PreInc? 1: _temp1380 ==( void*) Cyc_Absyn_PreDec){ struct
_tuple0* _temp1678= Cyc_Toc_temp_var(); goto _LL1679; _LL1679: { void* _temp1680=
Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1681; _LL1681: { struct Cyc_Absyn_Stmt*
_temp1682= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1678,
0), 0), 0); goto _LL1683; _LL1683: { struct Cyc_Absyn_Exp* _temp1684= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1678, 0), Cyc_Toc_curr_sp, 0),
_temp1380, 0); goto _LL1685; _LL1685: _temp1682= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1684, 0), _temp1682, 0); _temp1682= Cyc_Absyn_declare_stmt( _temp1678,
_temp1680,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1382, 0),
_temp1682, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1682, 0))->r));}}}}
else{ struct _tuple0* _temp1686= Cyc_Toc_temp_var(); goto _LL1687; _LL1687: {
struct _tuple0* _temp1688= Cyc_Toc_temp_var(); goto _LL1689; _LL1689: { void*
_temp1690= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1691; _LL1691: { void*
_temp1692= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1693; _LL1693: { struct Cyc_Absyn_Stmt*
_temp1694= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1688, 0), 0); goto
_LL1695; _LL1695: { struct Cyc_Absyn_Exp* _temp1696= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1686, 0), Cyc_Toc_curr_sp, 0),
_temp1380, 0); goto _LL1697; _LL1697: _temp1694= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1696, 0), _temp1694, 0); _temp1694= Cyc_Absyn_declare_stmt( _temp1688,
_temp1690,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1686, 0), 0), _temp1694, 0); _temp1694= Cyc_Absyn_declare_stmt( _temp1686,
_temp1692,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1382, 0),
_temp1694, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1694, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1382)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1382, 0, Cyc_Toc_incr_lvalue,
_temp1380);( void*)( e->r=( void*)(( void*) _temp1382->r));}} goto _LL1289;}
_LL1303: { int e1_poly= Cyc_Toc_is_poly_project( _temp1388); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1388->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1384->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1388); Cyc_Toc_exp_to_c( nv, _temp1384); if(
_temp1386 != 0?( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1386))->v ==(
void*) Cyc_Absyn_Plus? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1386))->v
==( void*) Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){(
void*)( _temp1388->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1388->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1384->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1384->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1384->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1384->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1388)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1388, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1698=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1698[ 0]=({ struct _tuple11 _temp1699; _temp1699.f1=
_temp1386; _temp1699.f2= _temp1384; _temp1699;}); _temp1698;}));( void*)( e->r=(
void*)(( void*) _temp1388->r));} goto _LL1289;} _LL1305: Cyc_Toc_exp_to_c( nv,
_temp1394); Cyc_Toc_exp_to_c( nv, _temp1392); Cyc_Toc_exp_to_c( nv, _temp1390);
goto _LL1289; _LL1307: Cyc_Toc_exp_to_c( nv, _temp1398); Cyc_Toc_exp_to_c( nv,
_temp1396); goto _LL1289; _LL1309: _temp1408= _temp1402; _temp1406= _temp1400;
goto _LL1311; _LL1311: Cyc_Toc_exp_to_c( nv, _temp1408);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1406); goto _LL1289; _LL1313:{
struct _RegionHandle _temp1700= _new_region(); struct _RegionHandle* r=&
_temp1700; _push_region( r);{ void* _temp1701=( void*) _temp1413->type; goto
_LL1702; _LL1702: { void* _temp1703= Cyc_Toc_typ_to_c( _temp1701); goto _LL1704;
_LL1704: { void* _temp1705= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1421->topt))->v); goto _LL1706; _LL1706: { struct Cyc_Absyn_Exp*
_temp1707= Cyc_Absyn_uint_exp(( unsigned int) _temp1417, 0); goto _LL1708;
_LL1708: { void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1780=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1780[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1781; _temp1781.tag= Cyc_Absyn_ArrayType;
_temp1781.f1=( void*) _temp1701; _temp1781.f2= Cyc_Toc_mt_tq; _temp1781.f3=(
struct Cyc_Absyn_Exp*) _temp1707; _temp1781;}); _temp1780;})); struct _tuple0*
_temp1709= Cyc_Toc_temp_var(); goto _LL1710; _LL1710: { struct _tuple0*
_temp1711= Cyc_Toc_temp_var(); goto _LL1712; _LL1712: { struct Cyc_Absyn_Exp*
_temp1713= Cyc_Absyn_var_exp( _temp1711, 0); goto _LL1714; _LL1714: { struct
_tuple0* _temp1715= Cyc_Toc_temp_var(); goto _LL1716; _LL1716: { int _temp1717=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1419); goto _LL1718;
_LL1718: { int _temp1719= _temp1717 - _temp1417; goto _LL1720; _LL1720: { struct
_tagged_ptr5 vs=({ unsigned int _temp1776=( unsigned int) _temp1417; struct
_tuple0** _temp1777=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1776); struct _tagged_ptr5 _temp1779={ _temp1777, _temp1777,
_temp1777 + _temp1776};{ unsigned int _temp1778= _temp1776; unsigned int i; for(
i= 0; i < _temp1778; i ++){ _temp1777[ i]= Cyc_Toc_temp_var();}}; _temp1779;});
struct _tagged_ptr5 xs=({ unsigned int _temp1772=( unsigned int) _temp1719;
struct _tuple0** _temp1773=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1772); struct _tagged_ptr5 _temp1775={ _temp1773, _temp1773,
_temp1773 + _temp1772};{ unsigned int _temp1774= _temp1772; unsigned int i; for(
i= 0; i < _temp1774; i ++){ _temp1773[ i]= Cyc_Toc_temp_var();}}; _temp1775;});
struct Cyc_List_List* xexps=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(*
f)( struct _tuple0*, struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,( struct Cyc_Position_Segment*)
0,(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct _tagged_ptr5 arr))
Cyc_List_rfrom_array)( r, xs)); xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List*
_temp1721=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1721->hd=( void*) Cyc_Absyn_var_exp( _temp1715, 0); _temp1721->tl= 0;
_temp1721;}));{ struct Cyc_Absyn_Stmt* _temp1722= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( _temp1709, 0), xexps, 0), 0); goto _LL1723; _LL1723: { struct
Cyc_Absyn_Exp* _temp1724= Cyc_Absyn_add_exp( _temp1713, _temp1707, 0); goto
_LL1725; _LL1725: { struct Cyc_List_List* _temp1735=({ struct Cyc_List_List*(*
_temp1726)( struct _tagged_ptr3)=( struct Cyc_List_List*(*)( struct _tagged_ptr3))
Cyc_List_list; struct _tuple4* _temp1729=({ struct _tuple4* _temp1734=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1734->f1= 0; _temp1734->f2=
_temp1713; _temp1734;}); struct _tuple4* _temp1730=({ struct _tuple4* _temp1733=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1733->f1= 0;
_temp1733->f2= _temp1713; _temp1733;}); struct _tuple4* _temp1731=({ struct
_tuple4* _temp1732=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1732->f1= 0; _temp1732->f2= _temp1724; _temp1732;}); struct _tuple4*
_temp1727[ 3u]={ _temp1729, _temp1730, _temp1731}; struct _tagged_ptr3 _temp1728={
_temp1727, _temp1727, _temp1727 + 3u}; _temp1726( _temp1728);}); goto _LL1736;
_LL1736: _temp1722= Cyc_Absyn_declare_stmt( _temp1715, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp1701,( void*) _temp1413->rgn, Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp(
0, _temp1735, 0), _temp1722, 0);{ struct Cyc_List_List* _temp1737=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_ptr5 arr)) Cyc_List_rfrom_array)( r, vs));
goto _LL1738; _LL1738: { struct Cyc_List_List* _temp1739=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_make_dle, _temp1737); goto _LL1740; _LL1740: _temp1722= Cyc_Absyn_declare_stmt(
_temp1711, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1739, 0), _temp1722, 0);{ struct Cyc_List_List* _temp1741=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1419);
goto _LL1742; _LL1742: { struct Cyc_List_List* _temp1743=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1415);
goto _LL1744; _LL1744:{ int i= _temp1417 - 1; for( 0; i >= 0;( i --, _temp1741=((
struct Cyc_List_List*) _check_null( _temp1741))->tl)){ struct Cyc_Absyn_Exp* arg=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1741))->hd;
void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c(
nv, arg); if( _temp1413->inject){ void* _temp1747; struct Cyc_Position_Segment*
_temp1749; struct Cyc_List_List* _temp1751; struct Cyc_List_List* _temp1753;
struct _tuple0* _temp1755; struct Cyc_Absyn_Tunionfield _temp1745=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1743))->hd); _LL1756: _temp1755=
_temp1745.name; goto _LL1754; _LL1754: _temp1753= _temp1745.tvs; goto _LL1752;
_LL1752: _temp1751= _temp1745.typs; goto _LL1750; _LL1750: _temp1749= _temp1745.loc;
goto _LL1748; _LL1748: _temp1747=( void*) _temp1745.sc; goto _LL1746; _LL1746:
_temp1743=(( struct Cyc_List_List*) _check_null( _temp1743))->tl;{ struct
_tuple0* _temp1757= Cyc_Toc_temp_var(); goto _LL1758; _LL1758: { struct Cyc_Absyn_Exp*
_temp1759= Cyc_Absyn_var_exp( _temp1757, 0); goto _LL1760; _LL1760: { void*
_temp1763= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1755,({
unsigned char* _temp1761="_struct"; struct _tagged_string _temp1762; _temp1762.curr=
_temp1761; _temp1762.base= _temp1761; _temp1762.last_plus_one= _temp1761 + 8;
_temp1762;}))); goto _LL1764; _LL1764: _temp1722= Cyc_Absyn_declare_stmt(*((
struct _tuple0**(*)( struct _tagged_ptr5, unsigned int, unsigned int))
_check_unknown_subscript)( vs, sizeof( struct _tuple0*), i), _temp1703,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1703, Cyc_Absyn_address_exp( _temp1759,
0), 0), _temp1722, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp1751))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1768= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1759,({
struct _tagged_string* _temp1765=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp1765[ 0]=({ unsigned char* _temp1766="f1"; struct
_tagged_string _temp1767; _temp1767.curr= _temp1766; _temp1767.base= _temp1766;
_temp1767.last_plus_one= _temp1766 + 3; _temp1767;}); _temp1765;}), 0), arg, 0);
goto _LL1769; _LL1769: _temp1722= Cyc_Absyn_seq_stmt( _temp1768, _temp1722, 0);{
struct Cyc_Absyn_Stmt* _temp1770= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1759, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1755, 0), 0); goto
_LL1771; _LL1771: _temp1722= Cyc_Absyn_seq_stmt( _temp1770, _temp1722, 0);
_temp1722= Cyc_Absyn_declare_stmt( _temp1757, _temp1763, 0, _temp1722, 0);}}}}}}}
else{ _temp1722= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr5, unsigned int, unsigned int)) _check_unknown_subscript)( vs,
sizeof( struct _tuple0*), i), _temp1703,( struct Cyc_Absyn_Exp*) arg, _temp1722,
0);}}}{ int i= _temp1719 - 1; for( 0; i >= 0;( i --, _temp1741=(( struct Cyc_List_List*)
_check_null( _temp1741))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1741))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1722= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr5, unsigned int, unsigned int)) _check_unknown_subscript)( xs,
sizeof( struct _tuple0*), i), argtype,( struct Cyc_Absyn_Exp*) arg, _temp1722, 0);}}
Cyc_Toc_exp_to_c( nv, _temp1421); _temp1722= Cyc_Absyn_declare_stmt( _temp1709,
_temp1705,( struct Cyc_Absyn_Exp*) _temp1421, _temp1722, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1722, 0))->r));}}}}}}}}}}}}}}}}}};
_pop_region(& _temp1700);} goto _LL1289; _LL1315: Cyc_Toc_exp_to_c( nv,
_temp1423);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1423), 0))->r)); goto _LL1289; _LL1317: Cyc_Toc_exp_to_c(
nv, _temp1425); goto _LL1289; _LL1319: Cyc_Toc_exp_to_c( nv, _temp1429);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
for( 0; _temp1427 != 0; _temp1427=(( struct Cyc_List_List*) _check_null(
_temp1427))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*)
_check_null( _temp1427))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*)
Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1429, 0))->r)); break;}} goto _LL1289;} _LL1321: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1431->topt))->v; void* new_typ=*
_temp1435;* _temp1435= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1431);{ int noconv= 0;{ void* _temp1782= Cyc_Toc_conversion( old_t2,
new_typ); void* _temp1792; struct Cyc_Absyn_Exp* _temp1794; void* _temp1796; int
_temp1798; struct Cyc_Absyn_Exp* _temp1800; void* _temp1802; _LL1784: if(
_temp1782 ==( void*) Cyc_Toc_NoConv){ goto _LL1785;} else{ goto _LL1786;}
_LL1786: if(( unsigned int) _temp1782 > 1u?*(( int*) _temp1782) == Cyc_Toc_NullCheck:
0){ _LL1793: _temp1792=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp1782)->f1;
goto _LL1787;} else{ goto _LL1788;} _LL1788: if(( unsigned int) _temp1782 > 1u?*((
int*) _temp1782) == Cyc_Toc_TagPtr: 0){ _LL1797: _temp1796=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp1782)->f1; goto _LL1795; _LL1795: _temp1794=(( struct Cyc_Toc_TagPtr_struct*)
_temp1782)->f2; goto _LL1789;} else{ goto _LL1790;} _LL1790: if(( unsigned int)
_temp1782 > 1u?*(( int*) _temp1782) == Cyc_Toc_UntagPtr: 0){ _LL1803: _temp1802=(
void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp1782)->f1; goto _LL1801; _LL1801:
_temp1800=(( struct Cyc_Toc_UntagPtr_struct*) _temp1782)->f2; goto _LL1799;
_LL1799: _temp1798=(( struct Cyc_Toc_UntagPtr_struct*) _temp1782)->f3; goto
_LL1791;} else{ goto _LL1783;} _LL1785: goto _LL1783; _LL1787:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_null_check_conv( nv->toplevel, _temp1792, _temp1431))->r));
goto _LL1783; _LL1789:( void*)( e->r=( void*)(( void*)( Cyc_Toc_tag_ptr_conv( nv->toplevel,
_temp1796, _temp1794, _temp1431))->r)); goto _LL1783; _LL1791:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_untag_ptr_conv( nv->toplevel, _temp1802, _temp1800,
_temp1798, _temp1431))->r)); goto _LL1783; _LL1783:;} goto _LL1289;}} _LL1323:{
void* _temp1804=( void*) _temp1436->r; struct Cyc_Absyn_Structdecl* _temp1812;
struct Cyc_List_List* _temp1814; struct Cyc_Core_Opt* _temp1816; struct _tuple0*
_temp1818; struct Cyc_List_List* _temp1820; _LL1806: if(*(( int*) _temp1804) ==
Cyc_Absyn_Struct_e){ _LL1819: _temp1818=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1804)->f1; goto _LL1817; _LL1817: _temp1816=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1804)->f2; goto _LL1815; _LL1815: _temp1814=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1804)->f3; goto _LL1813; _LL1813: _temp1812=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1804)->f4; goto _LL1807;} else{ goto _LL1808;} _LL1808: if(*(( int*)
_temp1804) == Cyc_Absyn_Tuple_e){ _LL1821: _temp1820=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1804)->f1; goto _LL1809;} else{ goto _LL1810;} _LL1810: goto _LL1811;
_LL1807: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1822= Cyc_Position_string_of_segment( _temp1436->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1822.last_plus_one -
_temp1822.curr, _temp1822.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1436->topt))->v, 1, 0,
_temp1814, _temp1818))->r)); goto _LL1805; _LL1809: if( nv->toplevel){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1823= Cyc_Position_string_of_segment(
_temp1436->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1823.last_plus_one - _temp1823.curr, _temp1823.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1820))->r)); goto _LL1805;
_LL1811: Cyc_Toc_exp_to_c( nv, _temp1436); if( ! Cyc_Absyn_is_lvalue( _temp1436)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1436,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1436, 0))->r));} goto _LL1805; _LL1805:;} goto _LL1289; _LL1325: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string
_temp1824= Cyc_Position_string_of_segment( _temp1438->loc); xprintf("%.*s: new at top-level",
_temp1824.last_plus_one - _temp1824.curr, _temp1824.curr);}));}{ void* _temp1825=(
void*) _temp1438->r; struct Cyc_List_List* _temp1839; void* _temp1841; struct
_tagged_string _temp1843; struct Cyc_Absyn_Exp* _temp1845; struct Cyc_Absyn_Exp*
_temp1847; struct Cyc_Absyn_Vardecl* _temp1849; struct Cyc_Absyn_Structdecl*
_temp1851; struct Cyc_List_List* _temp1853; struct Cyc_Core_Opt* _temp1855;
struct _tuple0* _temp1857; struct Cyc_List_List* _temp1859; _LL1827: if(*(( int*)
_temp1825) == Cyc_Absyn_Array_e){ _LL1840: _temp1839=(( struct Cyc_Absyn_Array_e_struct*)
_temp1825)->f1; goto _LL1828;} else{ goto _LL1829;} _LL1829: if(*(( int*)
_temp1825) == Cyc_Absyn_Const_e){ _LL1842: _temp1841=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1825)->f1; if(( unsigned int) _temp1841 > 1u?*(( int*) _temp1841) == Cyc_Absyn_String_c:
0){ _LL1844: _temp1843=(( struct Cyc_Absyn_String_c_struct*) _temp1841)->f1;
goto _LL1830;} else{ goto _LL1831;}} else{ goto _LL1831;} _LL1831: if(*(( int*)
_temp1825) == Cyc_Absyn_Comprehension_e){ _LL1850: _temp1849=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1825)->f1; goto _LL1848; _LL1848: _temp1847=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1825)->f2; goto _LL1846; _LL1846: _temp1845=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1825)->f3; goto _LL1832;} else{ goto _LL1833;} _LL1833: if(*(( int*)
_temp1825) == Cyc_Absyn_Struct_e){ _LL1858: _temp1857=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1825)->f1; goto _LL1856; _LL1856: _temp1855=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1825)->f2; goto _LL1854; _LL1854: _temp1853=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1825)->f3; goto _LL1852; _LL1852: _temp1851=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1825)->f4; goto _LL1834;} else{ goto _LL1835;} _LL1835: if(*(( int*)
_temp1825) == Cyc_Absyn_Tuple_e){ _LL1860: _temp1859=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1825)->f1; goto _LL1836;} else{ goto _LL1837;} _LL1837: goto _LL1838;
_LL1828: { struct _tuple0* _temp1861= Cyc_Toc_temp_var(); goto _LL1862; _LL1862: {
struct Cyc_Absyn_Exp* _temp1863= Cyc_Absyn_var_exp( _temp1861, 0); goto _LL1864;
_LL1864: { struct Cyc_Absyn_Stmt* _temp1865= Cyc_Toc_init_array( nv, _temp1863,
_temp1839, Cyc_Absyn_exp_stmt( _temp1863, 0)); goto _LL1866; _LL1866: { void*
old_elt_typ;{ void* _temp1867= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1873; struct Cyc_Absyn_Conref* _temp1875; struct Cyc_Absyn_Tqual _temp1877;
struct Cyc_Absyn_Conref* _temp1879; void* _temp1881; void* _temp1883; _LL1869:
if(( unsigned int) _temp1867 > 4u?*(( int*) _temp1867) == Cyc_Absyn_PointerType:
0){ _LL1874: _temp1873=(( struct Cyc_Absyn_PointerType_struct*) _temp1867)->f1;
_LL1884: _temp1883=( void*) _temp1873.elt_typ; goto _LL1882; _LL1882: _temp1881=(
void*) _temp1873.rgn_typ; goto _LL1880; _LL1880: _temp1879= _temp1873.nullable;
goto _LL1878; _LL1878: _temp1877= _temp1873.tq; goto _LL1876; _LL1876: _temp1875=
_temp1873.bounds; goto _LL1870;} else{ goto _LL1871;} _LL1871: goto _LL1872;
_LL1870: old_elt_typ= _temp1883; goto _LL1868; _LL1872: old_elt_typ=(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1885="exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1886; _temp1886.curr= _temp1885; _temp1886.base=
_temp1885; _temp1886.last_plus_one= _temp1885 + 52; _temp1886;})); goto _LL1868;
_LL1868:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1887= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1888; _LL1888: { struct Cyc_Absyn_Exp*
_temp1889= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1839), 0), 0); goto
_LL1890; _LL1890: { struct Cyc_Absyn_Exp* e1; if( _temp1440 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1889);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1440); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1889);}{ struct Cyc_Absyn_Exp* _temp1891= Cyc_Absyn_cast_exp( _temp1887, e1,
0); goto _LL1892; _LL1892:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1861, _temp1887,( struct Cyc_Absyn_Exp*) _temp1891,
_temp1865, 0), 0))->r)); goto _LL1826;}}}}}}}} _LL1830: if( _temp1440 == 0){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple0* _temp1893=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1893->f1= Cyc_Toc_abs_ns; _temp1893->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1893;}), 0),({ struct Cyc_List_List* _temp1894=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1894->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1895=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1895[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1896; _temp1896.tag= Cyc_Absyn_Const_e; _temp1896.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1897=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1897[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1898; _temp1898.tag= Cyc_Absyn_String_c;
_temp1898.f1= _temp1843; _temp1898;}); _temp1897;})); _temp1896;}); _temp1895;}),
0); _temp1894->tl= 0; _temp1894;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( _temp1440); Cyc_Toc_exp_to_c( nv, r);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({ struct
_tuple0* _temp1899=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp1899->f1= Cyc_Toc_abs_ns; _temp1899->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1899;}), 0),({ struct Cyc_List_List* _temp1900=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1900->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1901=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1901[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1902; _temp1902.tag= Cyc_Absyn_Const_e; _temp1902.f1=(
void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1903=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp1903[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp1904; _temp1904.tag= Cyc_Absyn_String_c;
_temp1904.f1= _temp1843; _temp1904;}); _temp1903;})); _temp1902;}); _temp1901;}),
0); _temp1900->tl= 0; _temp1900;}), 0))->r));} goto _LL1826; _LL1832: { int
is_tagged_ptr= 0;{ void* _temp1905= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1911; struct Cyc_Absyn_Conref* _temp1913; struct Cyc_Absyn_Tqual _temp1915;
struct Cyc_Absyn_Conref* _temp1917; void* _temp1919; void* _temp1921; _LL1907:
if(( unsigned int) _temp1905 > 4u?*(( int*) _temp1905) == Cyc_Absyn_PointerType:
0){ _LL1912: _temp1911=(( struct Cyc_Absyn_PointerType_struct*) _temp1905)->f1;
_LL1922: _temp1921=( void*) _temp1911.elt_typ; goto _LL1920; _LL1920: _temp1919=(
void*) _temp1911.rgn_typ; goto _LL1918; _LL1918: _temp1917= _temp1911.nullable;
goto _LL1916; _LL1916: _temp1915= _temp1911.tq; goto _LL1914; _LL1914: _temp1913=
_temp1911.bounds; goto _LL1908;} else{ goto _LL1909;} _LL1909: goto _LL1910;
_LL1908:{ void* _temp1923=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1913))->v; void* _temp1929; _LL1925: if((
unsigned int) _temp1923 > 1u?*(( int*) _temp1923) == Cyc_Absyn_Eq_constr: 0){
_LL1930: _temp1929=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1923)->f1;
if( _temp1929 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1926;} else{ goto _LL1927;}}
else{ goto _LL1927;} _LL1927: goto _LL1928; _LL1926: is_tagged_ptr= 1; goto
_LL1924; _LL1928: goto _LL1924; _LL1924:;} goto _LL1906; _LL1910:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1931="exp_to_c: comprehension not an array type";
struct _tagged_string _temp1932; _temp1932.curr= _temp1931; _temp1932.base=
_temp1931; _temp1932.last_plus_one= _temp1931 + 42; _temp1932;})); goto _LL1906;
_LL1906:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1845->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1847);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1849, Cyc_Absyn_var_exp(
max, 0), _temp1845, Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1933=
_new_region(); struct _RegionHandle* r=& _temp1933; _push_region( r);{ struct
Cyc_List_List* decls=({ struct Cyc_List_List* _temp1963=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1963->hd=( void*)({
struct _tuple10* _temp1964=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1964->f1= max; _temp1964->f2= Cyc_Absyn_uint_t; _temp1964->f3=(
struct Cyc_Absyn_Exp*) _temp1847; _temp1964;}); _temp1963->tl= 0; _temp1963;});
struct Cyc_Absyn_Exp* ai; if( _temp1440 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1440); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1934=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1934->hd=( void*)({ struct _tuple10*
_temp1935=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1935->f1= a; _temp1935->f2= ptr_typ; _temp1935->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1935;}); _temp1934->tl= decls; _temp1934;}); if( is_tagged_ptr){
struct _tuple0* _temp1936= Cyc_Toc_temp_var(); goto _LL1937; _LL1937: { void*
_temp1938= Cyc_Toc_typ_to_c( old_typ); goto _LL1939; _LL1939: { struct Cyc_Absyn_Exp*
_temp1949= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(* _temp1940)(
struct _tagged_ptr3)=( struct Cyc_List_List*(*)( struct _tagged_ptr3)) Cyc_List_list;
struct _tuple4* _temp1943=({ struct _tuple4* _temp1948=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1948->f1= 0; _temp1948->f2= Cyc_Absyn_var_exp(
a, 0); _temp1948;}); struct _tuple4* _temp1944=({ struct _tuple4* _temp1947=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1947->f1= 0;
_temp1947->f2= Cyc_Absyn_var_exp( a, 0); _temp1947;}); struct _tuple4* _temp1945=({
struct _tuple4* _temp1946=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1946->f1= 0; _temp1946->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1946;}); struct _tuple4* _temp1941[ 3u]={
_temp1943, _temp1944, _temp1945}; struct _tagged_ptr3 _temp1942={ _temp1941,
_temp1941, _temp1941 + 3u}; _temp1940( _temp1942);}), 0); goto _LL1950; _LL1950:
decls=({ struct Cyc_List_List* _temp1951=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1951->hd=( void*)({ struct _tuple10*
_temp1952=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1952->f1= _temp1936; _temp1952->f2= _temp1938; _temp1952->f3=( struct Cyc_Absyn_Exp*)
_temp1949; _temp1952;}); _temp1951->tl= decls; _temp1951;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1936, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
_temp1953= decls; goto _LL1954; _LL1954: for( 0; _temp1953 != 0; _temp1953=((
struct Cyc_List_List*) _check_null( _temp1953))->tl){ struct Cyc_Absyn_Exp*
_temp1957; void* _temp1959; struct _tuple0* _temp1961; struct _tuple10 _temp1955=*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1953))->hd);
_LL1962: _temp1961= _temp1955.f1; goto _LL1960; _LL1960: _temp1959= _temp1955.f2;
goto _LL1958; _LL1958: _temp1957= _temp1955.f3; goto _LL1956; _LL1956: s= Cyc_Absyn_declare_stmt(
_temp1961, _temp1959, _temp1957, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region(& _temp1933);} goto _LL1826;}}} _LL1834:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1438->topt))->v, 1, _temp1440, _temp1853, _temp1857))->r));
goto _LL1826; _LL1836:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv,
1, _temp1440, _temp1859))->r)); goto _LL1826; _LL1838: { void* _temp1965=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1438->topt))->v; goto _LL1966; _LL1966: {
struct _tuple0* _temp1967= Cyc_Toc_temp_var(); goto _LL1968; _LL1968: { struct
Cyc_Absyn_Stmt* _temp1969= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1967, 0),
0); goto _LL1970; _LL1970: { struct Cyc_Absyn_Exp* _temp1971= Cyc_Absyn_signed_int_exp(
0, 0); goto _LL1972; _LL1972: Cyc_Toc_exp_to_c( nv, _temp1438); _temp1969= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1967, 0),
_temp1971, 0), _temp1438, 0), _temp1969, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
_temp1965); void* _temp1973= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto
_LL1974; _LL1974: { struct Cyc_Absyn_Exp* _temp1975= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0); goto _LL1976; _LL1976: { struct Cyc_Absyn_Exp* e1; if( _temp1440 ==
0){ e1= Cyc_Toc_malloc_exp( _temp1965, _temp1975);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp1440); Cyc_Toc_exp_to_c( nv, r); e1=
Cyc_Toc_rmalloc_exp( r, _temp1975);}{ struct Cyc_Absyn_Exp* _temp1977= Cyc_Absyn_cast_exp(
_temp1973, e1, 0); goto _LL1978; _LL1978:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1967, _temp1973,( struct Cyc_Absyn_Exp*) _temp1977,
_temp1969, 0), 0))->r)); goto _LL1826;}}}}}}}} _LL1826:;} goto _LL1289; _LL1327:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp1979=(
struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1979[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1980; _temp1980.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1980.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1442);
_temp1980;}); _temp1979;}))); goto _LL1289; _LL1329: Cyc_Toc_exp_to_c( nv,
_temp1444); goto _LL1289; _LL1331: { void* e1_typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1446->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1446);{ void* _temp1981= e1_typ; struct Cyc_Absyn_PtrInfo _temp1987; struct
Cyc_Absyn_Conref* _temp1989; struct Cyc_Absyn_Tqual _temp1991; struct Cyc_Absyn_Conref*
_temp1993; void* _temp1995; void* _temp1997; _LL1983: if(( unsigned int)
_temp1981 > 4u?*(( int*) _temp1981) == Cyc_Absyn_PointerType: 0){ _LL1988:
_temp1987=(( struct Cyc_Absyn_PointerType_struct*) _temp1981)->f1; _LL1998:
_temp1997=( void*) _temp1987.elt_typ; goto _LL1996; _LL1996: _temp1995=( void*)
_temp1987.rgn_typ; goto _LL1994; _LL1994: _temp1993= _temp1987.nullable; goto
_LL1992; _LL1992: _temp1991= _temp1987.tq; goto _LL1990; _LL1990: _temp1989=
_temp1987.bounds; goto _LL1984;} else{ goto _LL1985;} _LL1985: goto _LL1986;
_LL1984:{ void* _temp1999=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1989); struct Cyc_Absyn_Exp* _temp2005; _LL2001: if( _temp1999 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL2002;} else{ goto _LL2003;} _LL2003: if((
unsigned int) _temp1999 > 1u?*(( int*) _temp1999) == Cyc_Absyn_Upper_b: 0){
_LL2006: _temp2005=(( struct Cyc_Absyn_Upper_b_struct*) _temp1999)->f1; goto
_LL2004;} else{ goto _LL2000;} _LL2002: { struct _tuple0* x= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* _temp2007= Cyc_Toc_typ_to_c_array(
_temp1997); goto _LL2008; _LL2008: { void* tx= Cyc_Toc_add_tagged_ptr_type(
_temp2007, _temp1991); void* ta2= Cyc_Absyn_cstar_typ( _temp2007, _temp1991);
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
struct Cyc_Absyn_Exp*) _temp1446, s1, 0), 0))->r)); goto _LL2000;}} _LL2004: if(
Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1446->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp2009=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2009->hd=(
void*) _temp1446; _temp2009->tl= 0; _temp2009;}), 0), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, e1_typ,(
struct Cyc_Absyn_Exp*) _temp1446, s3, 0), 0))->r));}} goto _LL2000; _LL2000:;}
goto _LL1982; _LL1986:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2010="exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp2011; _temp2011.curr= _temp2010; _temp2011.base= _temp2010; _temp2011.last_plus_one=
_temp2010 + 29; _temp2011;})); goto _LL1982; _LL1982:;} goto _LL1289;} _LL1333:
Cyc_Toc_exp_to_c( nv, _temp1450); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL1289; _LL1335: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1454->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1454);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp2014; struct Cyc_Absyn_Tqual _temp2016; struct Cyc_Absyn_Conref* _temp2018;
void* _temp2020; void* _temp2022; struct Cyc_Absyn_PtrInfo _temp2012= Cyc_Toc_get_ptr_type(
e1typ); _LL2023: _temp2022=( void*) _temp2012.elt_typ; goto _LL2021; _LL2021:
_temp2020=( void*) _temp2012.rgn_typ; goto _LL2019; _LL2019: _temp2018=
_temp2012.nullable; goto _LL2017; _LL2017: _temp2016= _temp2012.tq; goto _LL2015;
_LL2015: _temp2014= _temp2012.bounds; goto _LL2013; _LL2013:{ void* _temp2024=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2014); struct
Cyc_Absyn_Exp* _temp2030; _LL2026: if(( unsigned int) _temp2024 > 1u?*(( int*)
_temp2024) == Cyc_Absyn_Upper_b: 0){ _LL2031: _temp2030=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2024)->f1; goto _LL2027;} else{ goto _LL2028;} _LL2028: if( _temp2024 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL2029;} else{ goto _LL2025;} _LL2027: if(
Cyc_Evexp_eval_const_uint_exp( _temp2030) < 1){(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp2032="exp_to_c:  StructArrow_e on pointer of size 0";
struct _tagged_string _temp2033; _temp2033.curr= _temp2032; _temp2033.base=
_temp2032; _temp2033.last_plus_one= _temp2032 + 46; _temp2033;}));}{ int
possibly_null;{ void* _temp2034=( void*)((( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2018))->v; int _temp2040;
_LL2036: if(( unsigned int) _temp2034 > 1u?*(( int*) _temp2034) == Cyc_Absyn_Eq_constr:
0){ _LL2041: _temp2040=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2034)->f1;
goto _LL2037;} else{ goto _LL2038;} _LL2038: goto _LL2039; _LL2037:
possibly_null= _temp2040; goto _LL2035; _LL2039: possibly_null= 0; goto _LL2035;
_LL2035:;} if( Cyc_Toc_is_nullable(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1454->topt))->v)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1454->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp2042=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2042->hd=(
void*) _temp1454; _temp2042->tl= 0; _temp2042;}), 0), 0), _temp1452, 0))->r));}
else{ struct _tuple0* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1454->topt))->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1452, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1454, s3, 0), 0))->r));}} goto _LL2025;} _LL2029: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2022); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2016); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2052=({ struct Cyc_List_List*(* _temp2043)( struct
_tagged_ptr6)=( struct Cyc_List_List*(*)( struct _tagged_ptr6)) Cyc_List_list;
struct _tuple1* _temp2046=({ struct _tuple1* _temp2051=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2051->f1= 0; _temp2051->f2= Cyc_Toc_mt_tq;
_temp2051->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1454->topt))->v); _temp2051;}); struct _tuple1* _temp2047=({ struct _tuple1*
_temp2050=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2050->f1=
0; _temp2050->f2= Cyc_Toc_mt_tq; _temp2050->f3= Cyc_Absyn_uint_t; _temp2050;});
struct _tuple1* _temp2048=({ struct _tuple1* _temp2049=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2049->f1= 0; _temp2049->f2= Cyc_Toc_mt_tq;
_temp2049->f3= Cyc_Absyn_uint_t; _temp2049;}); struct _tuple1* _temp2044[ 3u]={
_temp2046, _temp2047, _temp2048}; struct _tagged_ptr6 _temp2045={ _temp2044,
_temp2044, _temp2044 + 3u}; _temp2043( _temp2045);}); goto _LL2053; _LL2053: {
void* _temp2057= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2054=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2054[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2055; _temp2055.tag= Cyc_Absyn_FnType;
_temp2055.f1=({ struct Cyc_Absyn_FnInfo _temp2056; _temp2056.tvars= 0; _temp2056.effect=
0; _temp2056.ret_typ=( void*) ta2; _temp2056.args= _temp2052; _temp2056.c_varargs=
0; _temp2056.cyc_varargs= 0; _temp2056.attributes= 0; _temp2056;}); _temp2055;});
_temp2054;}), Cyc_Toc_mt_tq); goto _LL2058; _LL2058: { struct Cyc_Absyn_Exp*
fnexp= Cyc_Absyn_cast_exp( _temp2057, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv,
0), 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_uint_exp( 0, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_fncall_exp( fnexp,({
struct Cyc_List_List*(* _temp2059)( struct _tagged_ptr7)=( struct Cyc_List_List*(*)(
struct _tagged_ptr7)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2062= _temp1454;
struct Cyc_Absyn_Exp* _temp2063= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2064= e2; struct Cyc_Absyn_Exp* _temp2060[ 3u]={ _temp2062, _temp2063,
_temp2064}; struct _tagged_ptr7 _temp2061={ _temp2060, _temp2060, _temp2060 + 3u};
_temp2059( _temp2061);}), 0), _temp1452, 0))->r));}}} else{ struct _tuple0* x=
Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2016);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( a, 0), _temp1452, 0), 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt( 0), 0); struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt* s1=
Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,(
struct Cyc_Absyn_Exp*) _temp1454, s1, 0), 0))->r));} goto _LL2025;} _LL2025:;}
if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1289;}} _LL1337: { void* _temp2065= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1458->topt))->v); goto _LL2066;
_LL2066: Cyc_Toc_exp_to_c( nv, _temp1458); Cyc_Toc_exp_to_c( nv, _temp1456);{
void* _temp2067= _temp2065; struct Cyc_List_List* _temp2075; struct Cyc_Absyn_PtrInfo
_temp2077; struct Cyc_Absyn_Conref* _temp2079; struct Cyc_Absyn_Tqual _temp2081;
struct Cyc_Absyn_Conref* _temp2083; void* _temp2085; void* _temp2087; _LL2069:
if(( unsigned int) _temp2067 > 4u?*(( int*) _temp2067) == Cyc_Absyn_TupleType: 0){
_LL2076: _temp2075=(( struct Cyc_Absyn_TupleType_struct*) _temp2067)->f1; goto
_LL2070;} else{ goto _LL2071;} _LL2071: if(( unsigned int) _temp2067 > 4u?*((
int*) _temp2067) == Cyc_Absyn_PointerType: 0){ _LL2078: _temp2077=(( struct Cyc_Absyn_PointerType_struct*)
_temp2067)->f1; _LL2088: _temp2087=( void*) _temp2077.elt_typ; goto _LL2086;
_LL2086: _temp2085=( void*) _temp2077.rgn_typ; goto _LL2084; _LL2084: _temp2083=
_temp2077.nullable; goto _LL2082; _LL2082: _temp2081= _temp2077.tq; goto _LL2080;
_LL2080: _temp2079= _temp2077.bounds; goto _LL2072;} else{ goto _LL2073;}
_LL2073: goto _LL2074; _LL2070: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1456) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1458, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL2068;} _LL2072:{ void*
_temp2089=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp2079); struct Cyc_Absyn_Exp* _temp2095; _LL2091: if(( unsigned int)
_temp2089 > 1u?*(( int*) _temp2089) == Cyc_Absyn_Upper_b: 0){ _LL2096: _temp2095=((
struct Cyc_Absyn_Upper_b_struct*) _temp2089)->f1; goto _LL2092;} else{ goto
_LL2093;} _LL2093: if( _temp2089 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2094;}
else{ goto _LL2090;} _LL2092: _temp2095= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp2095), 0);{ int possibly_null;{ void* _temp2097=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2083))->v; int
_temp2103; _LL2099: if(( unsigned int) _temp2097 > 1u?*(( int*) _temp2097) ==
Cyc_Absyn_Eq_constr: 0){ _LL2104: _temp2103=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2097)->f1; goto _LL2100;} else{ goto _LL2101;} _LL2101: goto _LL2102;
_LL2100: possibly_null= _temp2103; goto _LL2098; _LL2102: possibly_null= 0; goto
_LL2098; _LL2098:;}{ void* ta1= Cyc_Toc_typ_to_c( _temp2087); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp2081); if( Cyc_Toc_use_runtimecheck_funs){ if( possibly_null){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv, 0),({ struct Cyc_List_List*(*
_temp2105)( struct _tagged_ptr7)=( struct Cyc_List_List*(*)( struct _tagged_ptr7))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp2108= _temp1458; struct Cyc_Absyn_Exp*
_temp2109= _temp2095; struct Cyc_Absyn_Exp* _temp2110= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp2111= _temp1456; struct Cyc_Absyn_Exp*
_temp2106[ 4u]={ _temp2108, _temp2109, _temp2110, _temp2111}; struct
_tagged_ptr7 _temp2107={ _temp2106, _temp2106, _temp2106 + 4u}; _temp2105(
_temp2107);}), 0), 0), 0))->r));} else{( void*)( _temp1456->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List*(* _temp2112)( struct _tagged_ptr7)=( struct Cyc_List_List*(*)(
struct _tagged_ptr7)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2115= _temp2095;
struct Cyc_Absyn_Exp* _temp2116= Cyc_Absyn_copy_exp( _temp1456); struct Cyc_Absyn_Exp*
_temp2113[ 2u]={ _temp2115, _temp2116}; struct _tagged_ptr7 _temp2114={
_temp2113, _temp2113, _temp2113 + 2u}; _temp2112( _temp2114);}), 0))->r));}}
else{ struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp(
i, 0), _temp2095, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp((
void*) e3->r, 0), 0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1456, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1458, s1, 0), 0))->r));}} goto _LL2090;}} _LL2094: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2087); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2081); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2126=({ struct Cyc_List_List*(* _temp2117)( struct
_tagged_ptr6)=( struct Cyc_List_List*(*)( struct _tagged_ptr6)) Cyc_List_list;
struct _tuple1* _temp2120=({ struct _tuple1* _temp2125=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2125->f1= 0; _temp2125->f2= Cyc_Toc_mt_tq;
_temp2125->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1458->topt))->v); _temp2125;}); struct _tuple1* _temp2121=({ struct _tuple1*
_temp2124=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2124->f1=
0; _temp2124->f2= Cyc_Toc_mt_tq; _temp2124->f3= Cyc_Absyn_uint_t; _temp2124;});
struct _tuple1* _temp2122=({ struct _tuple1* _temp2123=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2123->f1= 0; _temp2123->f2= Cyc_Toc_mt_tq;
_temp2123->f3= Cyc_Absyn_uint_t; _temp2123;}); struct _tuple1* _temp2118[ 3u]={
_temp2120, _temp2121, _temp2122}; struct _tagged_ptr6 _temp2119={ _temp2118,
_temp2118, _temp2118 + 3u}; _temp2117( _temp2119);}); goto _LL2127; _LL2127: {
void* _temp2131= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2128=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2128[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2129; _temp2129.tag= Cyc_Absyn_FnType;
_temp2129.f1=({ struct Cyc_Absyn_FnInfo _temp2130; _temp2130.tvars= 0; _temp2130.effect=
0; _temp2130.ret_typ=( void*) ta2; _temp2130.args= _temp2126; _temp2130.c_varargs=
0; _temp2130.cyc_varargs= 0; _temp2130.attributes= 0; _temp2130;}); _temp2129;});
_temp2128;}), Cyc_Toc_mt_tq); goto _LL2132; _LL2132: { struct Cyc_Absyn_Exp*
fnexp= Cyc_Absyn_cast_exp( _temp2131, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv,
0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp(
fnexp,({ struct Cyc_List_List*(* _temp2133)( struct _tagged_ptr7)=( struct Cyc_List_List*(*)(
struct _tagged_ptr7)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2136= _temp1458;
struct Cyc_Absyn_Exp* _temp2137= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2138= _temp1456; struct Cyc_Absyn_Exp* _temp2134[ 3u]={ _temp2136,
_temp2137, _temp2138}; struct _tagged_ptr7 _temp2135={ _temp2134, _temp2134,
_temp2134 + 3u}; _temp2133( _temp2135);}), 0), 0))->r));}}} else{ struct _tuple0*
x= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2081);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1456, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1458, s1, 0), 0))->r));} goto _LL2090;} _LL2090:;}
goto _LL2068; _LL2074:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2139="exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp2140; _temp2140.curr= _temp2139; _temp2140.base=
_temp2139; _temp2140.last_plus_one= _temp2139 + 49; _temp2140;})); goto _LL2068;
_LL2068:;} goto _LL1289;} _LL1339: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1460))->r));} else{ struct Cyc_List_List*
_temp2141=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1460); goto
_LL2142; _LL2142: { struct _tagged_string* _temp2143= Cyc_Toc_add_tuple_type(
_temp2141); goto _LL2144; _LL2144: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1460 != 0;( _temp1460=(( struct Cyc_List_List*) _check_null(
_temp1460))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1460))->hd); dles=({ struct Cyc_List_List*
_temp2145=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2145->hd=( void*)({ struct _tuple4* _temp2146=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2146->f1= 0; _temp2146->f2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1460))->hd; _temp2146;}); _temp2145->tl=
dles; _temp2145;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));}}} goto _LL1289; _LL1341:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ unsigned char* _temp2147="compoundlit"; struct _tagged_string
_temp2148; _temp2148.curr= _temp2147; _temp2148.base= _temp2147; _temp2148.last_plus_one=
_temp2147 + 12; _temp2148;})); goto _LL1289; _LL1343:( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1466, 0))->r));{ struct Cyc_List_List*
_temp2149= _temp1466; goto _LL2150; _LL2150: for( 0; _temp2149 != 0; _temp2149=((
struct Cyc_List_List*) _check_null( _temp2149))->tl){ struct _tuple4 _temp2153;
struct Cyc_Absyn_Exp* _temp2154; struct Cyc_List_List* _temp2156; struct _tuple4*
_temp2151=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp2149))->hd;
_temp2153=* _temp2151; _LL2157: _temp2156= _temp2153.f1; goto _LL2155; _LL2155:
_temp2154= _temp2153.f2; goto _LL2152; _LL2152: Cyc_Toc_exp_to_c( nv, _temp2154);}}
goto _LL1289; _LL1345: { unsigned int _temp2158= Cyc_Evexp_eval_const_uint_exp(
_temp1470); goto _LL2159; _LL2159: Cyc_Toc_exp_to_c( nv, _temp1468);{ struct Cyc_List_List*
es= 0; if( ! Cyc_Toc_is_zero( _temp1468)){ unsigned int i= 0; for( 0; i <
_temp2158; i ++){ es=({ struct Cyc_List_List* _temp2160=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2160->hd=( void*)({ struct
_tuple4* _temp2161=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp2161->f1= 0; _temp2161->f2= _temp1468; _temp2161;}); _temp2160->tl= es;
_temp2160;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
es, 0))->r)); goto _LL1289;}} _LL1347: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1476, _temp1480))->r));}
else{ if( _temp1474 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2162="Struct_e: missing structdecl pointer"; struct
_tagged_string _temp2163; _temp2163.curr= _temp2162; _temp2163.base= _temp2162;
_temp2163.last_plus_one= _temp2162 + 37; _temp2163;}));}{ struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*) _check_null( _temp1474); struct
_RegionHandle _temp2164= _new_region(); struct _RegionHandle* rgn=& _temp2164;
_push_region( rgn);{ struct Cyc_List_List* _temp2165=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1476,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd2->fields))->v); goto _LL2166; _LL2166: { struct Cyc_List_List* _temp2167= 0;
goto _LL2168; _LL2168:{ struct Cyc_List_List* _temp2169=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto _LL2170; _LL2170: for(
0; _temp2169 != 0; _temp2169=(( struct Cyc_List_List*) _check_null( _temp2169))->tl){
struct Cyc_List_List* _temp2171= _temp2165; goto _LL2172; _LL2172: for( 0;
_temp2171 != 0; _temp2171=(( struct Cyc_List_List*) _check_null( _temp2171))->tl){
if((*(( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2171))->hd)).f1
==( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2169))->hd){ struct _tuple12 _temp2175; struct Cyc_Absyn_Exp* _temp2176;
struct Cyc_Absyn_Structfield* _temp2178; struct _tuple12* _temp2173=( struct
_tuple12*)(( struct Cyc_List_List*) _check_null( _temp2171))->hd; _temp2175=*
_temp2173; _LL2179: _temp2178= _temp2175.f1; goto _LL2177; _LL2177: _temp2176=
_temp2175.f2; goto _LL2174; _LL2174: { void* _temp2180=( void*) _temp2178->type;
goto _LL2181; _LL2181: Cyc_Toc_exp_to_c( nv, _temp2176); if( Cyc_Toc_is_void_star(
_temp2180)){( void*)( _temp2176->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp2176->r, 0), 0))->r));} _temp2167=({ struct Cyc_List_List*
_temp2182=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2182->hd=( void*)({ struct _tuple4* _temp2183=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2183->f1= 0; _temp2183->f2= _temp2176; _temp2183;});
_temp2182->tl= _temp2167; _temp2182;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp2167), 0))->r));}}; _pop_region(& _temp2164);}} goto
_LL1289; _LL1349: { struct Cyc_List_List* fs;{ void* _temp2184= Cyc_Tcutil_compress(
_temp1484); struct Cyc_List_List* _temp2190; _LL2186: if(( unsigned int)
_temp2184 > 4u?*(( int*) _temp2184) == Cyc_Absyn_AnonStructType: 0){ _LL2191:
_temp2190=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2184)->f1; goto
_LL2187;} else{ goto _LL2188;} _LL2188: goto _LL2189; _LL2187: fs= _temp2190;
goto _LL2185; _LL2189: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp2192= Cyc_Absynpp_typ2string(
_temp1484); xprintf("anon struct has type %.*s", _temp2192.last_plus_one -
_temp2192.curr, _temp2192.curr);})); goto _LL2185; _LL2185:;}{ struct
_RegionHandle _temp2193= _new_region(); struct _RegionHandle* rgn=& _temp2193;
_push_region( rgn);{ struct Cyc_List_List* _temp2194=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1482, fs); goto _LL2195; _LL2195: for( 0; _temp2194 != 0; _temp2194=((
struct Cyc_List_List*) _check_null( _temp2194))->tl){ struct _tuple12 _temp2198;
struct Cyc_Absyn_Exp* _temp2199; struct Cyc_Absyn_Structfield* _temp2201; struct
_tuple12* _temp2196=( struct _tuple12*)(( struct Cyc_List_List*) _check_null(
_temp2194))->hd; _temp2198=* _temp2196; _LL2202: _temp2201= _temp2198.f1; goto
_LL2200; _LL2200: _temp2199= _temp2198.f2; goto _LL2197; _LL2197: { void*
_temp2203=( void*) _temp2201->type; goto _LL2204; _LL2204: Cyc_Toc_exp_to_c( nv,
_temp2199); if( Cyc_Toc_is_void_star( _temp2203)){( void*)( _temp2199->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp2199->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1482, 0))->r));}; _pop_region(& _temp2193);} goto _LL1289;} _LL1351: {
struct _tuple0* qv= _temp1486->name; if( _temp1488->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2205= _temp1488->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1488->fields))->v;
goto _LL2206; _LL2206: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2205))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2205))->hd)->typs
== 0){ tag_count ++;} _temp2205=(( struct Cyc_List_List*) _check_null( _temp2205))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1289;} _LL1353: { struct _tuple0* _temp2207= _temp1496->name; goto
_LL2208; _LL2208: { struct Cyc_List_List* _temp2209= _temp1496->typs; goto
_LL2210; _LL2210: { struct _tuple0* _temp2211= Cyc_Toc_temp_var(); goto _LL2212;
_LL2212: { struct Cyc_Absyn_Exp* _temp2213= Cyc_Absyn_var_exp( _temp2211, 0);
goto _LL2214; _LL2214: { void* _temp2217= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2207,({ unsigned char* _temp2215="_struct"; struct _tagged_string _temp2216;
_temp2216.curr= _temp2215; _temp2216.base= _temp2215; _temp2216.last_plus_one=
_temp2215 + 8; _temp2216;}))); goto _LL2218; _LL2218: if( nv->toplevel){ struct
Cyc_Absyn_Exp* tag_exp; if( _temp1498->is_xtunion){ tag_exp= Cyc_Absyn_var_exp(
_temp2207, 0);} else{ int tag_count= 0; struct Cyc_List_List* _temp2219=
_temp1498->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1498->fields))->v; goto _LL2220; _LL2220: while( Cyc_Absyn_qvar_cmp(
_temp2207,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2219))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2219))->hd)->typs != 0){ tag_count ++;}
_temp2219=(( struct Cyc_List_List*) _check_null( _temp2219))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2221= 0; goto _LL2222;
_LL2222: for( 0; _temp1500 != 0;( _temp1500=(( struct Cyc_List_List*)
_check_null( _temp1500))->tl, _temp2209=(( struct Cyc_List_List*) _check_null(
_temp2209))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1500))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2209))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2221=({ struct Cyc_List_List* _temp2223=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2223[ 0]=({ struct Cyc_List_List
_temp2224; _temp2224.hd=( void*)({ struct _tuple4* _temp2225=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2225[ 0]=({ struct _tuple4
_temp2226; _temp2226.f1= 0; _temp2226.f2= cur_e; _temp2226;}); _temp2225;});
_temp2224.tl= _temp2221; _temp2224;}); _temp2223;});} _temp2221=({ struct Cyc_List_List*
_temp2227=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2227[ 0]=({ struct Cyc_List_List _temp2228; _temp2228.hd=( void*)({ struct
_tuple4* _temp2229=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2229[ 0]=({ struct _tuple4 _temp2230; _temp2230.f1= 0; _temp2230.f2=
tag_exp; _temp2230;}); _temp2229;}); _temp2228.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2221); _temp2228;}); _temp2227;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2231=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2231->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2207,({ unsigned char*
_temp2232="_struct"; struct _tagged_string _temp2233; _temp2233.curr= _temp2232;
_temp2233.base= _temp2232; _temp2233.last_plus_one= _temp2232 + 8; _temp2233;}));
_temp2231;}), _temp2221, 0))->r));}} else{ struct Cyc_List_List* _temp2235=({
struct Cyc_List_List* _temp2234=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2234->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp2213, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp2207, 0), 0); _temp2234->tl=
0; _temp2234;}); goto _LL2236; _LL2236:{ int i= 1; for( 0; _temp1500 != 0;(((
_temp1500=(( struct Cyc_List_List*) _check_null( _temp1500))->tl, i ++)),
_temp2209=(( struct Cyc_List_List*) _check_null( _temp2209))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1500))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2209))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp2237= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp2213, Cyc_Toc_fieldname(
i), 0), cur_e, 0); goto _LL2238; _LL2238: _temp2235=({ struct Cyc_List_List*
_temp2239=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2239->hd=( void*) _temp2237; _temp2239->tl= _temp2235; _temp2239;});}}}{
struct Cyc_Absyn_Stmt* _temp2240= Cyc_Absyn_exp_stmt( _temp2213, 0); goto
_LL2241; _LL2241: { struct Cyc_Absyn_Stmt* _temp2243= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp2242=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2242->hd=( void*) _temp2240; _temp2242->tl= _temp2235; _temp2242;})), 0);
goto _LL2244; _LL2244:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp2211, _temp2217, 0, _temp2243, 0), 0))->r));}}} goto _LL1289;}}}}} _LL1355:
goto _LL1289; _LL1357: { void* t_c= Cyc_Toc_typ_to_c( _temp1512); if( _temp1514
!= 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1514);
Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp(
rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} else{( void*)( e->r=( void*)((
void*)( Cyc_Toc_malloc_exp( _temp1512, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
goto _LL1289;} _LL1359: Cyc_Toc_stmt_to_c( nv, _temp1516); goto _LL1289; _LL1361:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char*
_temp2245="UnresolvedMem"; struct _tagged_string _temp2246; _temp2246.curr=
_temp2245; _temp2246.base= _temp2245; _temp2246.last_plus_one= _temp2245 + 14;
_temp2246;})); goto _LL1289; _LL1363:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2247="codegen"; struct _tagged_string _temp2248; _temp2248.curr=
_temp2247; _temp2248.base= _temp2247; _temp2248.last_plus_one= _temp2247 + 8;
_temp2248;})); goto _LL1289; _LL1365:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2249="fill"; struct _tagged_string _temp2250; _temp2250.curr=
_temp2249; _temp2250.base= _temp2249; _temp2250.last_plus_one= _temp2249 + 5;
_temp2250;})); goto _LL1289; _LL1289:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
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
_temp2251=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp2287; struct Cyc_Absyn_Vardecl
_temp2289; struct Cyc_List_List* _temp2290; struct Cyc_Core_Opt* _temp2292;
struct Cyc_Absyn_Exp* _temp2294; void* _temp2296; struct Cyc_Absyn_Tqual
_temp2298; struct _tuple0* _temp2300; void* _temp2302; struct Cyc_Absyn_Vardecl*
_temp2304; int _temp2306; void* _temp2308; unsigned char _temp2310; struct
_tagged_string _temp2312; struct Cyc_Absyn_Enumfield* _temp2314; struct Cyc_Absyn_Enumdecl*
_temp2316; struct Cyc_List_List* _temp2318; struct Cyc_List_List* _temp2320;
struct Cyc_Absyn_Tunionfield* _temp2322; struct Cyc_Absyn_Tuniondecl* _temp2324;
struct Cyc_Absyn_Pat* _temp2327; struct Cyc_Absyn_Pat _temp2329; struct Cyc_Position_Segment*
_temp2330; struct Cyc_Core_Opt* _temp2332; void* _temp2334; struct Cyc_List_List*
_temp2336; struct Cyc_List_List* _temp2338; struct Cyc_Absyn_Tunionfield*
_temp2340; struct Cyc_Absyn_Tuniondecl* _temp2342; struct Cyc_List_List*
_temp2344; struct Cyc_List_List* _temp2346; struct Cyc_Absyn_Tunionfield*
_temp2348; struct Cyc_Absyn_Tuniondecl* _temp2350; struct Cyc_List_List*
_temp2352; struct Cyc_List_List* _temp2354; struct Cyc_List_List* _temp2356;
struct Cyc_Core_Opt* _temp2358; struct Cyc_Absyn_Structdecl* _temp2360; struct
Cyc_Absyn_Pat* _temp2362; struct _tuple0* _temp2364; struct Cyc_List_List*
_temp2366; struct Cyc_List_List* _temp2368; struct _tuple0* _temp2370; struct
Cyc_List_List* _temp2372; struct Cyc_List_List* _temp2374; struct _tuple0*
_temp2376; _LL2253: if(( unsigned int) _temp2251 > 2u?*(( int*) _temp2251) ==
Cyc_Absyn_Var_p: 0){ _LL2288: _temp2287=(( struct Cyc_Absyn_Var_p_struct*)
_temp2251)->f1; _temp2289=* _temp2287; _LL2303: _temp2302=( void*) _temp2289.sc;
goto _LL2301; _LL2301: _temp2300= _temp2289.name; goto _LL2299; _LL2299:
_temp2298= _temp2289.tq; goto _LL2297; _LL2297: _temp2296=( void*) _temp2289.type;
goto _LL2295; _LL2295: _temp2294= _temp2289.initializer; goto _LL2293; _LL2293:
_temp2292= _temp2289.rgn; goto _LL2291; _LL2291: _temp2290= _temp2289.attributes;
goto _LL2254;} else{ goto _LL2255;} _LL2255: if( _temp2251 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2256;} else{ goto _LL2257;} _LL2257: if(( unsigned int) _temp2251 > 2u?*((
int*) _temp2251) == Cyc_Absyn_Reference_p: 0){ _LL2305: _temp2304=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2251)->f1; goto _LL2258;} else{ goto _LL2259;} _LL2259: if( _temp2251 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2260;} else{ goto _LL2261;} _LL2261: if((
unsigned int) _temp2251 > 2u?*(( int*) _temp2251) == Cyc_Absyn_Int_p: 0){
_LL2309: _temp2308=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2251)->f1;
goto _LL2307; _LL2307: _temp2306=(( struct Cyc_Absyn_Int_p_struct*) _temp2251)->f2;
goto _LL2262;} else{ goto _LL2263;} _LL2263: if(( unsigned int) _temp2251 > 2u?*((
int*) _temp2251) == Cyc_Absyn_Char_p: 0){ _LL2311: _temp2310=(( struct Cyc_Absyn_Char_p_struct*)
_temp2251)->f1; goto _LL2264;} else{ goto _LL2265;} _LL2265: if(( unsigned int)
_temp2251 > 2u?*(( int*) _temp2251) == Cyc_Absyn_Float_p: 0){ _LL2313: _temp2312=((
struct Cyc_Absyn_Float_p_struct*) _temp2251)->f1; goto _LL2266;} else{ goto
_LL2267;} _LL2267: if(( unsigned int) _temp2251 > 2u?*(( int*) _temp2251) == Cyc_Absyn_Enum_p:
0){ _LL2317: _temp2316=(( struct Cyc_Absyn_Enum_p_struct*) _temp2251)->f1; goto
_LL2315; _LL2315: _temp2314=(( struct Cyc_Absyn_Enum_p_struct*) _temp2251)->f2;
goto _LL2268;} else{ goto _LL2269;} _LL2269: if(( unsigned int) _temp2251 > 2u?*((
int*) _temp2251) == Cyc_Absyn_Tunion_p: 0){ _LL2325: _temp2324=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2251)->f1; goto _LL2323; _LL2323: _temp2322=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2251)->f2; goto _LL2321; _LL2321: _temp2320=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2251)->f3; goto _LL2319; _LL2319: _temp2318=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2251)->f4; if( _temp2318 == 0){ goto _LL2270;} else{ goto _LL2271;}} else{
goto _LL2271;} _LL2271: if(( unsigned int) _temp2251 > 2u?*(( int*) _temp2251)
== Cyc_Absyn_Pointer_p: 0){ _LL2328: _temp2327=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2251)->f1; _temp2329=* _temp2327; _LL2335: _temp2334=( void*) _temp2329.r;
if(( unsigned int) _temp2334 > 2u?*(( int*) _temp2334) == Cyc_Absyn_Tunion_p: 0){
_LL2343: _temp2342=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2334)->f1; goto
_LL2341; _LL2341: _temp2340=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2334)->f2;
goto _LL2339; _LL2339: _temp2338=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2334)->f3;
goto _LL2337; _LL2337: _temp2336=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2334)->f4;
goto _LL2333;} else{ goto _LL2273;} _LL2333: _temp2332= _temp2329.topt; goto
_LL2331; _LL2331: _temp2330= _temp2329.loc; goto _LL2326;} else{ goto _LL2273;}
_LL2326: if( _temp2336 != 0){ goto _LL2272;} else{ goto _LL2273;} _LL2273: if((
unsigned int) _temp2251 > 2u?*(( int*) _temp2251) == Cyc_Absyn_Tunion_p: 0){
_LL2351: _temp2350=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2251)->f1; goto
_LL2349; _LL2349: _temp2348=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2251)->f2;
goto _LL2347; _LL2347: _temp2346=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2251)->f3;
goto _LL2345; _LL2345: _temp2344=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2251)->f4;
goto _LL2274;} else{ goto _LL2275;} _LL2275: if(( unsigned int) _temp2251 > 2u?*((
int*) _temp2251) == Cyc_Absyn_Tuple_p: 0){ _LL2353: _temp2352=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2251)->f1; goto _LL2276;} else{ goto _LL2277;} _LL2277: if(( unsigned int)
_temp2251 > 2u?*(( int*) _temp2251) == Cyc_Absyn_Struct_p: 0){ _LL2361:
_temp2360=(( struct Cyc_Absyn_Struct_p_struct*) _temp2251)->f1; goto _LL2359;
_LL2359: _temp2358=(( struct Cyc_Absyn_Struct_p_struct*) _temp2251)->f2; goto
_LL2357; _LL2357: _temp2356=(( struct Cyc_Absyn_Struct_p_struct*) _temp2251)->f3;
goto _LL2355; _LL2355: _temp2354=(( struct Cyc_Absyn_Struct_p_struct*) _temp2251)->f4;
goto _LL2278;} else{ goto _LL2279;} _LL2279: if(( unsigned int) _temp2251 > 2u?*((
int*) _temp2251) == Cyc_Absyn_Pointer_p: 0){ _LL2363: _temp2362=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2251)->f1; goto _LL2280;} else{ goto _LL2281;} _LL2281: if(( unsigned int)
_temp2251 > 2u?*(( int*) _temp2251) == Cyc_Absyn_UnknownId_p: 0){ _LL2365:
_temp2364=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2251)->f1; goto _LL2282;}
else{ goto _LL2283;} _LL2283: if(( unsigned int) _temp2251 > 2u?*(( int*)
_temp2251) == Cyc_Absyn_UnknownCall_p: 0){ _LL2371: _temp2370=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2251)->f1; goto _LL2369; _LL2369: _temp2368=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2251)->f2; goto _LL2367; _LL2367: _temp2366=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2251)->f3; goto _LL2284;} else{ goto _LL2285;} _LL2285: if(( unsigned int)
_temp2251 > 2u?*(( int*) _temp2251) == Cyc_Absyn_UnknownFields_p: 0){ _LL2377:
_temp2376=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2251)->f1; goto
_LL2375; _LL2375: _temp2374=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2251)->f2; goto _LL2373; _LL2373: _temp2372=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2251)->f3; goto _LL2286;} else{ goto _LL2252;} _LL2254: nv= Cyc_Toc_add_varmap(
nv, _temp2300, r); goto _LL2256; _LL2256: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2252; _LL2258: { struct _tuple0* _temp2378= Cyc_Toc_temp_var(); goto
_LL2379; _LL2379: decls=({ struct Cyc_List_List* _temp2380=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2380->hd=( void*)({
struct _tuple14* _temp2381=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2381->f1= _temp2378; _temp2381->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2381;}); _temp2380->tl= decls;
_temp2380;}); nv= Cyc_Toc_add_varmap( nv, _temp2304->name, Cyc_Absyn_var_exp(
_temp2378, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2378, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2252;} _LL2260: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2252; _LL2262: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2308, _temp2306, 0), succ_lab, fail_lab); goto _LL2252;
_LL2264: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2310, 0),
succ_lab, fail_lab); goto _LL2252; _LL2266: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2312, 0), succ_lab, fail_lab); goto _LL2252; _LL2268: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2382=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2382[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2383; _temp2383.tag= Cyc_Absyn_Enum_e;
_temp2383.f1= _temp2314->name; _temp2383.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2316; _temp2383.f3=( struct Cyc_Absyn_Enumfield*) _temp2314; _temp2383;});
_temp2382;}), 0), succ_lab, fail_lab); goto _LL2252; _LL2270: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2322->name, 0); if( ! _temp2324->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2252;} _LL2272: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2336);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2340->name,({
unsigned char* _temp2418="_struct"; struct _tagged_string _temp2419; _temp2419.curr=
_temp2418; _temp2419.base= _temp2418; _temp2419.last_plus_one= _temp2418 + 8;
_temp2419;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct Cyc_List_List*
_temp2384=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2340->typs); goto _LL2385; _LL2385: { struct Cyc_List_List*
_temp2386=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2336); goto _LL2387; _LL2387: for( 0; _temp2386 !=
0;((( _temp2386=(( struct Cyc_List_List*) _check_null( _temp2386))->tl,
_temp2384=(( struct Cyc_List_List*) _check_null( _temp2384))->tl)), cnt --)){
struct Cyc_Absyn_Pat* _temp2388=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2386))->hd; goto _LL2389; _LL2389: { void* _temp2390=(*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2384))->hd)).f2;
goto _LL2391; _LL2391: { struct _tuple0* _temp2392= Cyc_Toc_temp_var(); goto
_LL2393; _LL2393: { void* _temp2394=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp2388->topt))->v; goto _LL2395; _LL2395: { void* _temp2396= Cyc_Toc_typ_to_c(
_temp2394); goto _LL2397; _LL2397: { struct _tagged_string* _temp2398= Cyc_Toc_fresh_label();
goto _LL2399; _LL2399: { struct Cyc_Absyn_Exp* _temp2401= Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2400=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2400[ 0]= xprintf("f%d", cnt);
_temp2400;}), 0); goto _LL2402; _LL2402: if( Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c(
_temp2390))){ _temp2401= Cyc_Absyn_cast_exp( _temp2396, _temp2401, 0);} decls=({
struct Cyc_List_List* _temp2403=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2403->hd=( void*)({ struct _tuple14*
_temp2404=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2404->f1= _temp2392; _temp2404->f2= _temp2396; _temp2404;}); _temp2403->tl=
decls; _temp2403;});{ struct _tuple13 _temp2405= Cyc_Toc_xlate_pat( nv, rgn,
_temp2394, Cyc_Absyn_var_exp( _temp2392, 0), _temp2401, _temp2388, succ_lab,
fail_lab, decls); goto _LL2406; _LL2406: nv= _temp2405.f1; decls= _temp2405.f2;{
struct Cyc_Absyn_Stmt* _temp2407= _temp2405.f3; goto _LL2408; _LL2408: { struct
Cyc_Absyn_Stmt* _temp2409= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2392,
0), _temp2401, 0); goto _LL2410; _LL2410: { struct Cyc_Absyn_Stmt* _temp2411=
Cyc_Absyn_seq_stmt( _temp2409, _temp2407, 0); goto _LL2412; _LL2412: ss=({
struct Cyc_List_List* _temp2413=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2413->hd=( void*) Cyc_Absyn_label_stmt( _temp2398,
_temp2411, 0); _temp2413->tl= ss; _temp2413;}); succ_lab= _temp2398;}}}}}}}}}}}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2342->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2340->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2414=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2342->fields))->v; goto _LL2415; _LL2415: for( 0; _temp2414 !=
0; _temp2414=(( struct Cyc_List_List*) _check_null( _temp2414))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2416=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2414))->hd; goto _LL2417; _LL2417: if( _temp2416->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2340->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2252;}}} _LL2274: _temp2352=
_temp2344; goto _LL2276; _LL2276: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2352);{ struct Cyc_List_List*
_temp2420=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2352); goto _LL2421; _LL2421: for( 0; _temp2420 !=
0;( _temp2420=(( struct Cyc_List_List*) _check_null( _temp2420))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2422=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2420))->hd; goto _LL2423; _LL2423: { struct _tuple0* _temp2424=
Cyc_Toc_temp_var(); goto _LL2425; _LL2425: { void* _temp2426=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2422->topt))->v; goto _LL2427; _LL2427: {
struct _tagged_string* _temp2428= Cyc_Toc_fresh_label(); goto _LL2429; _LL2429:
decls=({ struct Cyc_List_List* _temp2430=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2430->hd=( void*)({ struct _tuple14*
_temp2431=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2431->f1= _temp2424; _temp2431->f2= Cyc_Toc_typ_to_c( _temp2426); _temp2431;});
_temp2430->tl= decls; _temp2430;});{ struct _tuple13 _temp2432= Cyc_Toc_xlate_pat(
nv, rgn, _temp2426, Cyc_Absyn_var_exp( _temp2424, 0), Cyc_Absyn_structmember_exp(
path, Cyc_Toc_fieldname( cnt), 0), _temp2422, succ_lab, fail_lab, decls); goto
_LL2433; _LL2433: nv= _temp2432.f1; decls= _temp2432.f2;{ struct Cyc_Absyn_Stmt*
_temp2434= _temp2432.f3; goto _LL2435; _LL2435: { struct Cyc_Absyn_Stmt*
_temp2436= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2424, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2434, 0); goto _LL2437; _LL2437: ss=({ struct Cyc_List_List* _temp2438=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2438->hd=(
void*) Cyc_Absyn_label_stmt( _temp2428, _temp2436, 0); _temp2438->tl= ss;
_temp2438;}); succ_lab= _temp2428;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2252;} _LL2278: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2439=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2354); goto _LL2440; _LL2440: for( 0; _temp2439 !=
0; _temp2439=(( struct Cyc_List_List*) _check_null( _temp2439))->tl){ struct
_tuple15* _temp2441=( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2439))->hd; goto _LL2442; _LL2442: { struct Cyc_Absyn_Pat* _temp2443=(*
_temp2441).f2; goto _LL2444; _LL2444: { struct _tagged_string* f;{ void*
_temp2445=( void*)(( struct Cyc_List_List*) _check_null((* _temp2441).f1))->hd;
struct _tagged_string* _temp2451; _LL2447: if(*(( int*) _temp2445) == Cyc_Absyn_FieldName){
_LL2452: _temp2451=(( struct Cyc_Absyn_FieldName_struct*) _temp2445)->f1; goto
_LL2448;} else{ goto _LL2449;} _LL2449: goto _LL2450; _LL2448: f= _temp2451;
goto _LL2446; _LL2450:( void) _throw(( void*) Cyc_Toc_Match_error); _LL2446:;}{
struct _tuple0* _temp2453= Cyc_Toc_temp_var(); goto _LL2454; _LL2454: { void*
_temp2455=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2443->topt))->v;
goto _LL2456; _LL2456: { void* _temp2457= Cyc_Toc_typ_to_c( _temp2455); goto
_LL2458; _LL2458: { struct _tagged_string* _temp2459= Cyc_Toc_fresh_label();
goto _LL2460; _LL2460: decls=({ struct Cyc_List_List* _temp2461=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2461->hd=( void*)({
struct _tuple14* _temp2462=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2462->f1= _temp2453; _temp2462->f2= _temp2457; _temp2462;});
_temp2461->tl= decls; _temp2461;});{ struct _tuple13 _temp2463= Cyc_Toc_xlate_pat(
nv, rgn, _temp2455, Cyc_Absyn_var_exp( _temp2453, 0), Cyc_Absyn_structmember_exp(
path, f, 0), _temp2443, succ_lab, fail_lab, decls); goto _LL2464; _LL2464: nv=
_temp2463.f1; decls= _temp2463.f2;{ struct Cyc_Absyn_Exp* _temp2465= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2466; _LL2466: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2360->fields))->v, f)))->type)){ _temp2465= Cyc_Absyn_cast_exp(
_temp2457, _temp2465, 0);}{ struct Cyc_Absyn_Stmt* _temp2467= _temp2463.f3; goto
_LL2468; _LL2468: { struct Cyc_Absyn_Stmt* _temp2469= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2453, 0), _temp2465, 0), _temp2467, 0); goto _LL2470;
_LL2470: ss=({ struct Cyc_List_List* _temp2471=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2471->hd=( void*) Cyc_Absyn_label_stmt(
_temp2459, _temp2469, 0); _temp2471->tl= ss; _temp2471;}); succ_lab= _temp2459;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2252;} _LL2280: { struct _tuple0*
_temp2472= Cyc_Toc_temp_var(); goto _LL2473; _LL2473: { void* _temp2474=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2362->topt))->v; goto _LL2475; _LL2475:
decls=({ struct Cyc_List_List* _temp2476=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2476->hd=( void*)({ struct _tuple14*
_temp2477=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2477->f1= _temp2472; _temp2477->f2= Cyc_Toc_typ_to_c( _temp2474); _temp2477;});
_temp2476->tl= decls; _temp2476;});{ struct _tuple13 _temp2478= Cyc_Toc_xlate_pat(
nv, rgn, _temp2474, Cyc_Absyn_var_exp( _temp2472, 0), Cyc_Absyn_deref_exp( path,
0), _temp2362, succ_lab, fail_lab, decls); goto _LL2479; _LL2479: nv= _temp2478.f1;
decls= _temp2478.f2;{ struct Cyc_Absyn_Stmt* _temp2480= _temp2478.f3; goto
_LL2481; _LL2481: { struct Cyc_Absyn_Stmt* _temp2482= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2472, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2480, 0);
goto _LL2483; _LL2483: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2482, 0);} else{ s= _temp2482;} goto _LL2252;}}}}} _LL2282: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2484="unknownid"; struct _tagged_string _temp2485; _temp2485.curr=
_temp2484; _temp2485.base= _temp2484; _temp2485.last_plus_one= _temp2484 + 10;
_temp2485;})); goto _LL2252; _LL2284: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2486="unknowncall";
struct _tagged_string _temp2487; _temp2487.curr= _temp2486; _temp2487.base=
_temp2486; _temp2487.last_plus_one= _temp2486 + 12; _temp2487;})); goto _LL2252;
_LL2286: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2488="unknownfields"; struct _tagged_string _temp2489;
_temp2489.curr= _temp2488; _temp2489.base= _temp2488; _temp2489.last_plus_one=
_temp2488 + 14; _temp2489;})); goto _LL2252; _LL2252:;} return({ struct _tuple13
_temp2490; _temp2490.f1= nv; _temp2490.f2= decls; _temp2490.f3= s; _temp2490;});}
struct _tuple16{ struct _tagged_string* f1; struct _tagged_string* f2; struct
Cyc_Absyn_Switch_clause* f3; } ; static struct _tuple16* Cyc_Toc_gen_label(
struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause* sc){ return({ struct
_tuple16* _temp2491=( struct _tuple16*) _region_malloc( r, sizeof( struct
_tuple16)); _temp2491->f1= Cyc_Toc_fresh_label(); _temp2491->f2= Cyc_Toc_fresh_label();
_temp2491->f3= sc; _temp2491;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp2492=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2493; _LL2493: { int leave_as_switch;{ void*
_temp2494= Cyc_Tcutil_compress( _temp2492); void* _temp2500; void* _temp2502;
_LL2496: if(( unsigned int) _temp2494 > 4u?*(( int*) _temp2494) == Cyc_Absyn_IntType:
0){ _LL2503: _temp2502=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2494)->f1;
goto _LL2501; _LL2501: _temp2500=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2494)->f2; goto _LL2497;} else{ goto _LL2498;} _LL2498: goto _LL2499;
_LL2497: leave_as_switch= 1; goto _LL2495; _LL2499: leave_as_switch= 0; goto
_LL2495; _LL2495:;}{ struct Cyc_List_List* _temp2504= scs; goto _LL2505; _LL2505:
for( 0; _temp2504 != 0; _temp2504=(( struct Cyc_List_List*) _check_null(
_temp2504))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2504))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2504))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2506= scs; goto
_LL2507; _LL2507: for( 0; _temp2506 != 0; _temp2506=(( struct Cyc_List_List*)
_check_null( _temp2506))->tl){ struct Cyc_Absyn_Stmt* _temp2508=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2506))->hd)->body; goto _LL2509;
_LL2509:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2506))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2508, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2508);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2510=
_new_region(); struct _RegionHandle* rgn=& _temp2510; _push_region( rgn);{
struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2511= lscs; goto
_LL2512; _LL2512: for( 0; _temp2511 != 0; _temp2511=(( struct Cyc_List_List*)
_check_null( _temp2511))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct
_tuple16*)(( struct Cyc_List_List*) _check_null( _temp2511))->hd)).f3; struct
_tagged_string* fail_lab=(( struct Cyc_List_List*) _check_null( _temp2511))->tl
== 0? end_l:(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp2511))->tl))->hd)).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2511))->hd)).f2; if( sc->where_clause == 0){ struct _tuple13
_temp2513= Cyc_Toc_xlate_pat( nv, rgn, _temp2492, r, path, sc->pattern, succ_lab,
fail_lab, decls); goto _LL2514; _LL2514: nvs=({ struct Cyc_List_List* _temp2515=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2515->hd=( void*) _temp2513.f1; _temp2515->tl= nvs; _temp2515;}); decls=
_temp2513.f2; test_stmts=({ struct Cyc_List_List* _temp2516=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2516->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2511))->hd)).f1,
_temp2513.f3, 0); _temp2516->tl= test_stmts; _temp2516;});} else{ struct Cyc_Absyn_Exp*
_temp2517=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2518;
_LL2518: { struct _tagged_string* _temp2519= Cyc_Toc_fresh_label(); goto _LL2520;
_LL2520: { struct _tuple13 _temp2521= Cyc_Toc_xlate_pat( nv, rgn, _temp2492, r,
path, sc->pattern, _temp2519, fail_lab, decls); goto _LL2522; _LL2522: Cyc_Toc_exp_to_c(
_temp2521.f1, _temp2517); nvs=({ struct Cyc_List_List* _temp2523=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2523->hd=( void*)
_temp2521.f1; _temp2523->tl= nvs; _temp2523;}); decls= _temp2521.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2517, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2519, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2524=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2524->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2511))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2521.f3, s3, 0), 0); _temp2524->tl= test_stmts;
_temp2524;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2525= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2526; _LL2526: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2527=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2528; _LL2528: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2527, end_l,(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2529=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2530; _LL2530: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2529, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2525, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple14 _temp2533;
void* _temp2534; struct _tuple0* _temp2536; struct _tuple14* _temp2531=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2533=*
_temp2531; _LL2537: _temp2536= _temp2533.f1; goto _LL2535; _LL2535: _temp2534=
_temp2533.f2; goto _LL2532; _LL2532: res= Cyc_Absyn_declare_stmt( _temp2536,
_temp2534, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region(& _temp2510);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2538=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2538[ 0]=({ struct Cyc_List_List _temp2539; _temp2539.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2539.tl= 0; _temp2539;}); _temp2538;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2540=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2582; struct Cyc_Absyn_Stmt* _temp2584; struct Cyc_Absyn_Stmt*
_temp2586; struct Cyc_Absyn_Exp* _temp2588; struct Cyc_Absyn_Stmt* _temp2590;
struct Cyc_Absyn_Stmt* _temp2592; struct Cyc_Absyn_Exp* _temp2594; struct Cyc_Absyn_Stmt*
_temp2596; struct _tuple2 _temp2598; struct Cyc_Absyn_Stmt* _temp2600; struct
Cyc_Absyn_Exp* _temp2602; struct Cyc_Absyn_Stmt* _temp2604; struct Cyc_Absyn_Stmt*
_temp2606; struct Cyc_Absyn_Stmt* _temp2608; struct _tagged_string* _temp2610;
struct Cyc_Absyn_Stmt* _temp2612; struct _tuple2 _temp2614; struct Cyc_Absyn_Stmt*
_temp2616; struct Cyc_Absyn_Exp* _temp2618; struct _tuple2 _temp2620; struct Cyc_Absyn_Stmt*
_temp2622; struct Cyc_Absyn_Exp* _temp2624; struct Cyc_Absyn_Exp* _temp2626;
struct Cyc_List_List* _temp2628; struct Cyc_Absyn_Exp* _temp2630; struct Cyc_Absyn_Switch_clause**
_temp2632; struct Cyc_List_List* _temp2634; struct Cyc_Absyn_Stmt* _temp2636;
struct Cyc_Absyn_Decl* _temp2638; struct Cyc_Absyn_Stmt* _temp2640; struct
_tagged_string* _temp2642; struct _tuple2 _temp2644; struct Cyc_Absyn_Stmt*
_temp2646; struct Cyc_Absyn_Exp* _temp2648; struct Cyc_Absyn_Stmt* _temp2650;
struct Cyc_List_List* _temp2652; struct Cyc_Absyn_Stmt* _temp2654; struct Cyc_Absyn_Stmt*
_temp2656; struct Cyc_Absyn_Vardecl* _temp2658; struct Cyc_Absyn_Tvar* _temp2660;
struct Cyc_List_List* _temp2662; struct Cyc_Absyn_Exp* _temp2664; struct Cyc_Absyn_Stmt*
_temp2666; struct Cyc_Absyn_Stmt* _temp2668; _LL2542: if( _temp2540 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2543;} else{ goto _LL2544;} _LL2544: if((
unsigned int) _temp2540 > 1u?*(( int*) _temp2540) == Cyc_Absyn_Exp_s: 0){
_LL2583: _temp2582=(( struct Cyc_Absyn_Exp_s_struct*) _temp2540)->f1; goto
_LL2545;} else{ goto _LL2546;} _LL2546: if(( unsigned int) _temp2540 > 1u?*((
int*) _temp2540) == Cyc_Absyn_Seq_s: 0){ _LL2587: _temp2586=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2540)->f1; goto _LL2585; _LL2585: _temp2584=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2540)->f2; goto _LL2547;} else{ goto _LL2548;} _LL2548: if(( unsigned int)
_temp2540 > 1u?*(( int*) _temp2540) == Cyc_Absyn_Return_s: 0){ _LL2589:
_temp2588=(( struct Cyc_Absyn_Return_s_struct*) _temp2540)->f1; goto _LL2549;}
else{ goto _LL2550;} _LL2550: if(( unsigned int) _temp2540 > 1u?*(( int*)
_temp2540) == Cyc_Absyn_IfThenElse_s: 0){ _LL2595: _temp2594=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2540)->f1; goto _LL2593; _LL2593: _temp2592=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2540)->f2; goto _LL2591; _LL2591: _temp2590=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2540)->f3; goto _LL2551;} else{ goto _LL2552;} _LL2552: if(( unsigned int)
_temp2540 > 1u?*(( int*) _temp2540) == Cyc_Absyn_While_s: 0){ _LL2599: _temp2598=((
struct Cyc_Absyn_While_s_struct*) _temp2540)->f1; _LL2603: _temp2602= _temp2598.f1;
goto _LL2601; _LL2601: _temp2600= _temp2598.f2; goto _LL2597; _LL2597: _temp2596=((
struct Cyc_Absyn_While_s_struct*) _temp2540)->f2; goto _LL2553;} else{ goto
_LL2554;} _LL2554: if(( unsigned int) _temp2540 > 1u?*(( int*) _temp2540) == Cyc_Absyn_Break_s:
0){ _LL2605: _temp2604=(( struct Cyc_Absyn_Break_s_struct*) _temp2540)->f1; goto
_LL2555;} else{ goto _LL2556;} _LL2556: if(( unsigned int) _temp2540 > 1u?*((
int*) _temp2540) == Cyc_Absyn_Continue_s: 0){ _LL2607: _temp2606=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2540)->f1; goto _LL2557;} else{ goto _LL2558;} _LL2558: if(( unsigned int)
_temp2540 > 1u?*(( int*) _temp2540) == Cyc_Absyn_Goto_s: 0){ _LL2611: _temp2610=((
struct Cyc_Absyn_Goto_s_struct*) _temp2540)->f1; goto _LL2609; _LL2609:
_temp2608=(( struct Cyc_Absyn_Goto_s_struct*) _temp2540)->f2; goto _LL2559;}
else{ goto _LL2560;} _LL2560: if(( unsigned int) _temp2540 > 1u?*(( int*)
_temp2540) == Cyc_Absyn_For_s: 0){ _LL2627: _temp2626=(( struct Cyc_Absyn_For_s_struct*)
_temp2540)->f1; goto _LL2621; _LL2621: _temp2620=(( struct Cyc_Absyn_For_s_struct*)
_temp2540)->f2; _LL2625: _temp2624= _temp2620.f1; goto _LL2623; _LL2623:
_temp2622= _temp2620.f2; goto _LL2615; _LL2615: _temp2614=(( struct Cyc_Absyn_For_s_struct*)
_temp2540)->f3; _LL2619: _temp2618= _temp2614.f1; goto _LL2617; _LL2617:
_temp2616= _temp2614.f2; goto _LL2613; _LL2613: _temp2612=(( struct Cyc_Absyn_For_s_struct*)
_temp2540)->f4; goto _LL2561;} else{ goto _LL2562;} _LL2562: if(( unsigned int)
_temp2540 > 1u?*(( int*) _temp2540) == Cyc_Absyn_Switch_s: 0){ _LL2631:
_temp2630=(( struct Cyc_Absyn_Switch_s_struct*) _temp2540)->f1; goto _LL2629;
_LL2629: _temp2628=(( struct Cyc_Absyn_Switch_s_struct*) _temp2540)->f2; goto
_LL2563;} else{ goto _LL2564;} _LL2564: if(( unsigned int) _temp2540 > 1u?*((
int*) _temp2540) == Cyc_Absyn_Fallthru_s: 0){ _LL2635: _temp2634=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2540)->f1; goto _LL2633; _LL2633: _temp2632=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2540)->f2; goto _LL2565;} else{ goto _LL2566;} _LL2566: if(( unsigned int)
_temp2540 > 1u?*(( int*) _temp2540) == Cyc_Absyn_Decl_s: 0){ _LL2639: _temp2638=((
struct Cyc_Absyn_Decl_s_struct*) _temp2540)->f1; goto _LL2637; _LL2637:
_temp2636=(( struct Cyc_Absyn_Decl_s_struct*) _temp2540)->f2; goto _LL2567;}
else{ goto _LL2568;} _LL2568: if(( unsigned int) _temp2540 > 1u?*(( int*)
_temp2540) == Cyc_Absyn_Label_s: 0){ _LL2643: _temp2642=(( struct Cyc_Absyn_Label_s_struct*)
_temp2540)->f1; goto _LL2641; _LL2641: _temp2640=(( struct Cyc_Absyn_Label_s_struct*)
_temp2540)->f2; goto _LL2569;} else{ goto _LL2570;} _LL2570: if(( unsigned int)
_temp2540 > 1u?*(( int*) _temp2540) == Cyc_Absyn_Do_s: 0){ _LL2651: _temp2650=((
struct Cyc_Absyn_Do_s_struct*) _temp2540)->f1; goto _LL2645; _LL2645: _temp2644=((
struct Cyc_Absyn_Do_s_struct*) _temp2540)->f2; _LL2649: _temp2648= _temp2644.f1;
goto _LL2647; _LL2647: _temp2646= _temp2644.f2; goto _LL2571;} else{ goto
_LL2572;} _LL2572: if(( unsigned int) _temp2540 > 1u?*(( int*) _temp2540) == Cyc_Absyn_TryCatch_s:
0){ _LL2655: _temp2654=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2540)->f1;
goto _LL2653; _LL2653: _temp2652=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2540)->f2; goto _LL2573;} else{ goto _LL2574;} _LL2574: if(( unsigned int)
_temp2540 > 1u?*(( int*) _temp2540) == Cyc_Absyn_Region_s: 0){ _LL2661:
_temp2660=(( struct Cyc_Absyn_Region_s_struct*) _temp2540)->f1; goto _LL2659;
_LL2659: _temp2658=(( struct Cyc_Absyn_Region_s_struct*) _temp2540)->f2; goto
_LL2657; _LL2657: _temp2656=(( struct Cyc_Absyn_Region_s_struct*) _temp2540)->f3;
goto _LL2575;} else{ goto _LL2576;} _LL2576: if(( unsigned int) _temp2540 > 1u?*((
int*) _temp2540) == Cyc_Absyn_SwitchC_s: 0){ _LL2665: _temp2664=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2540)->f1; goto _LL2663; _LL2663: _temp2662=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2540)->f2; goto _LL2577;} else{ goto _LL2578;} _LL2578: if(( unsigned int)
_temp2540 > 1u?*(( int*) _temp2540) == Cyc_Absyn_Cut_s: 0){ _LL2667: _temp2666=((
struct Cyc_Absyn_Cut_s_struct*) _temp2540)->f1; goto _LL2579;} else{ goto
_LL2580;} _LL2580: if(( unsigned int) _temp2540 > 1u?*(( int*) _temp2540) == Cyc_Absyn_Splice_s:
0){ _LL2669: _temp2668=(( struct Cyc_Absyn_Splice_s_struct*) _temp2540)->f1;
goto _LL2581;} else{ goto _LL2541;} _LL2543: return; _LL2545: Cyc_Toc_exp_to_c(
nv, _temp2582); return; _LL2547: Cyc_Toc_stmt_to_c( nv, _temp2586); s= _temp2584;
continue; _LL2549: { struct Cyc_Core_Opt* topt= 0; if( _temp2588 != 0){ topt=({
struct Cyc_Core_Opt* _temp2670=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2670->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2588))->topt))->v);
_temp2670;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2588));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2671=
Cyc_Toc_temp_var(); goto _LL2672; _LL2672: { struct Cyc_Absyn_Stmt* _temp2673=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2671, 0),
0); goto _LL2674; _LL2674:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2671,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2588,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2673, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2551: Cyc_Toc_exp_to_c(
nv, _temp2594); Cyc_Toc_stmt_to_c( nv, _temp2592); s= _temp2590; continue;
_LL2553: Cyc_Toc_exp_to_c( nv, _temp2602); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2596); return; _LL2555: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2604 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2604))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2557: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -(( struct Cyc_Absyn_Stmt*) _check_null( _temp2606))->try_depth, s);
return; _LL2559: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2608))->try_depth, s); return; _LL2561: Cyc_Toc_exp_to_c( nv,
_temp2626); Cyc_Toc_exp_to_c( nv, _temp2624); Cyc_Toc_exp_to_c( nv, _temp2618);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2612); return; _LL2563: Cyc_Toc_xlate_switch(
nv, s, _temp2630, _temp2628); return; _LL2565: if( nv->fallthru_info == 0){(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2675="fallthru in unexpected place"; struct _tagged_string
_temp2676; _temp2676.curr= _temp2675; _temp2676.base= _temp2675; _temp2676.last_plus_one=
_temp2675 + 29; _temp2676;})));}{ struct _tuple8 _temp2679; struct Cyc_Dict_Dict*
_temp2680; struct Cyc_List_List* _temp2682; struct _tagged_string* _temp2684;
struct _tuple8* _temp2677=( struct _tuple8*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp2679=* _temp2677; _LL2685: _temp2684= _temp2679.f1;
goto _LL2683; _LL2683: _temp2682= _temp2679.f2; goto _LL2681; _LL2681: _temp2680=
_temp2679.f3; goto _LL2678; _LL2678: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp2684, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp2632)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2686=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2682); goto _LL2687; _LL2687: { struct Cyc_List_List* _temp2688=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp2634); goto
_LL2689; _LL2689: for( 0; _temp2686 != 0;( _temp2686=(( struct Cyc_List_List*)
_check_null( _temp2686))->tl, _temp2688=(( struct Cyc_List_List*) _check_null(
_temp2688))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp2688))->hd); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2680,( struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp2686))->hd),(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp2688))->hd, 0),
s2, 0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}}} _LL2567:{ void*
_temp2690=( void*) _temp2638->r; int _temp2700; struct Cyc_Absyn_Exp* _temp2702;
struct Cyc_Core_Opt* _temp2704; struct Cyc_Core_Opt* _temp2706; struct Cyc_Absyn_Pat*
_temp2708; struct Cyc_List_List* _temp2710; struct Cyc_Absyn_Vardecl* _temp2712;
_LL2692: if(*(( int*) _temp2690) == Cyc_Absyn_Let_d){ _LL2709: _temp2708=((
struct Cyc_Absyn_Let_d_struct*) _temp2690)->f1; goto _LL2707; _LL2707: _temp2706=((
struct Cyc_Absyn_Let_d_struct*) _temp2690)->f2; goto _LL2705; _LL2705: _temp2704=((
struct Cyc_Absyn_Let_d_struct*) _temp2690)->f3; goto _LL2703; _LL2703: _temp2702=((
struct Cyc_Absyn_Let_d_struct*) _temp2690)->f4; goto _LL2701; _LL2701: _temp2700=((
struct Cyc_Absyn_Let_d_struct*) _temp2690)->f5; goto _LL2693;} else{ goto
_LL2694;} _LL2694: if(*(( int*) _temp2690) == Cyc_Absyn_Letv_d){ _LL2711:
_temp2710=(( struct Cyc_Absyn_Letv_d_struct*) _temp2690)->f1; goto _LL2695;}
else{ goto _LL2696;} _LL2696: if(*(( int*) _temp2690) == Cyc_Absyn_Var_d){
_LL2713: _temp2712=(( struct Cyc_Absyn_Var_d_struct*) _temp2690)->f1; goto
_LL2697;} else{ goto _LL2698;} _LL2698: goto _LL2699; _LL2693:( void*)( s->r=(
void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2708, _temp2704, _temp2702,
_temp2700, _temp2636))->r)); goto _LL2691; _LL2695: { struct Cyc_List_List*
_temp2714=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2710); goto _LL2715; _LL2715: if( _temp2714 == 0){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2716="empty Letv_d";
struct _tagged_string _temp2717; _temp2717.curr= _temp2716; _temp2717.base=
_temp2716; _temp2717.last_plus_one= _temp2716 + 13; _temp2717;}));}( void*)(
_temp2638->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2718=(
struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2718[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2719; _temp2719.tag= Cyc_Absyn_Var_d;
_temp2719.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2714))->hd; _temp2719;}); _temp2718;}))); _temp2714=(( struct Cyc_List_List*)
_check_null( _temp2714))->tl; for( 0; _temp2714 != 0; _temp2714=(( struct Cyc_List_List*)
_check_null( _temp2714))->tl){ struct Cyc_Absyn_Decl* _temp2722= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2720=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2720[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2721; _temp2721.tag= Cyc_Absyn_Var_d; _temp2721.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2714))->hd; _temp2721;}); _temp2720;}),
0); goto _LL2723; _LL2723:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp2722, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c(
nv, s); goto _LL2691;} _LL2697: { struct _tuple0* _temp2724= _temp2712->name;
goto _LL2725; _LL2725:( void*)( _temp2712->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2712->type));{ struct Cyc_Toc_Env* _temp2728= Cyc_Toc_add_varmap( nv,
_temp2724, Cyc_Absyn_varb_exp( _temp2724,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2726=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2726[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2727; _temp2727.tag= Cyc_Absyn_Local_b;
_temp2727.f1= _temp2712; _temp2727;}); _temp2726;}), 0)); goto _LL2729; _LL2729:
Cyc_Toc_stmt_to_c( _temp2728, _temp2636); if( _temp2712->initializer != 0){
struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*) _check_null( _temp2712->initializer);
void* _temp2730=( void*) init->r; struct Cyc_Absyn_Exp* _temp2736; struct Cyc_Absyn_Exp*
_temp2738; struct Cyc_Absyn_Vardecl* _temp2740; _LL2732: if(*(( int*) _temp2730)
== Cyc_Absyn_Comprehension_e){ _LL2741: _temp2740=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2730)->f1; goto _LL2739; _LL2739: _temp2738=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2730)->f2; goto _LL2737; _LL2737: _temp2736=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2730)->f3; goto _LL2733;} else{ goto _LL2734;} _LL2734: goto _LL2735;
_LL2733: _temp2712->initializer= 0;( void*)( _temp2636->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2724, 0), _temp2740, _temp2738, _temp2736, Cyc_Absyn_new_stmt((
void*) _temp2636->r, 0), 0))->r)); goto _LL2731; _LL2735: Cyc_Toc_exp_to_c( nv,
init); goto _LL2731; _LL2731:;} goto _LL2691;}} _LL2699:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2742="bad nested declaration within function";
struct _tagged_string _temp2743; _temp2743.curr= _temp2742; _temp2743.base=
_temp2742; _temp2743.last_plus_one= _temp2742 + 39; _temp2743;})); goto _LL2691;
_LL2691:;} return; _LL2569: s= _temp2640; continue; _LL2571: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp2650); Cyc_Toc_exp_to_c( nv, _temp2648); return;
_LL2573: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__push_handler_qv,
0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp( Cyc_Toc_setjmp_qv, 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__pop_handler_qv,
0); struct Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp( Cyc_Toc__exn_thrown_qv,
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2744=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2744->v=( void*) e_typ; _temp2744;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2654);{
struct Cyc_Absyn_Stmt* _temp2745= Cyc_Absyn_seq_stmt( _temp2654, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); goto _LL2746; _LL2746: {
struct _tuple0* _temp2747= Cyc_Toc_temp_var(); goto _LL2748; _LL2748: { struct
Cyc_Absyn_Exp* _temp2749= Cyc_Absyn_var_exp( _temp2747, 0); goto _LL2750;
_LL2750: { struct Cyc_Absyn_Vardecl* _temp2751= Cyc_Absyn_new_vardecl( _temp2747,
Cyc_Absyn_exn_typ, 0); goto _LL2752; _LL2752: _temp2749->topt=({ struct Cyc_Core_Opt*
_temp2753=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2753->v=( void*) Cyc_Absyn_exn_typ; _temp2753;});{ struct Cyc_Absyn_Pat*
_temp2758=({ struct Cyc_Absyn_Pat* _temp2754=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2754->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2756=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2756[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2757; _temp2757.tag= Cyc_Absyn_Var_p;
_temp2757.f1= _temp2751; _temp2757;}); _temp2756;})); _temp2754->topt=({ struct
Cyc_Core_Opt* _temp2755=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2755->v=( void*) Cyc_Absyn_exn_typ; _temp2755;}); _temp2754->loc= 0;
_temp2754;}); goto _LL2759; _LL2759: { struct Cyc_Absyn_Exp* _temp2760= Cyc_Absyn_throw_exp(
_temp2749, 0); goto _LL2761; _LL2761: _temp2760->topt=({ struct Cyc_Core_Opt*
_temp2762=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2762->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2762;});{ struct Cyc_Absyn_Stmt*
_temp2763= Cyc_Absyn_exp_stmt( _temp2760, 0); goto _LL2764; _LL2764: { struct
Cyc_Absyn_Switch_clause* _temp2768=({ struct Cyc_Absyn_Switch_clause* _temp2765=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2765->pattern= _temp2758; _temp2765->pat_vars=({ struct Cyc_Core_Opt*
_temp2766=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2766->v=( void*)({ struct Cyc_List_List* _temp2767=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2767->hd=( void*) _temp2751;
_temp2767->tl= 0; _temp2767;}); _temp2766;}); _temp2765->where_clause= 0;
_temp2765->body= _temp2763; _temp2765->loc= 0; _temp2765;}); goto _LL2769;
_LL2769: { struct Cyc_Absyn_Stmt* _temp2771= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2652,({ struct Cyc_List_List* _temp2770=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2770->hd=( void*) _temp2768; _temp2770->tl=
0; _temp2770;})), 0); goto _LL2772; _LL2772: Cyc_Toc_stmt_to_c( nv, _temp2771);{
struct Cyc_Absyn_Exp* _temp2774= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2773=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2773->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2773->tl= 0; _temp2773;}), 0); goto _LL2775; _LL2775: { struct Cyc_Absyn_Stmt*
_temp2777= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2776=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2776->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2776->tl= 0;
_temp2776;}), 0), 0); goto _LL2778; _LL2778: { struct Cyc_Absyn_Exp* _temp2779=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2780; _LL2780: {
struct Cyc_Absyn_Exp* _temp2781= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2782; _LL2782:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2777, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2779, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2774, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2781, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2745, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2771, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2575: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2658->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2788=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2788[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2789; _temp2789.tag= Cyc_Absyn_Local_b; _temp2789.f1=
_temp2658; _temp2789;}); _temp2788;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2787=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2787->f1= Cyc_Toc_abs_ns; _temp2787->f2= Cyc_Toc__push_region_sp;
_temp2787;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2786=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2786->f1= Cyc_Toc_abs_ns; _temp2786->f2= Cyc_Toc__pop_region_sp; _temp2786;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2785=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2785->f1=
Cyc_Toc_abs_ns; _temp2785->f2= Cyc_Toc__new_region_sp; _temp2785;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2784=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2784->hd=( void*) addr_rh_exp; _temp2784->tl= 0; _temp2784;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2783=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2783->hd=( void*) x_exp; _temp2783->tl= 0; _temp2783;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2656);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2656, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2577:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2790="switchC_s";
struct _tagged_string _temp2791; _temp2791.curr= _temp2790; _temp2791.base=
_temp2790; _temp2791.last_plus_one= _temp2790 + 10; _temp2791;})); return;
_LL2579:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp2792="cut"; struct _tagged_string _temp2793; _temp2793.curr= _temp2792;
_temp2793.base= _temp2792; _temp2793.last_plus_one= _temp2792 + 4; _temp2793;}));
return; _LL2581:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2794="splice"; struct _tagged_string _temp2795; _temp2795.curr=
_temp2794; _temp2795.base= _temp2794; _temp2795.last_plus_one= _temp2794 + 7;
_temp2795;})); return; _LL2541:;}} struct _tuple17{ struct _tagged_string* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0;( void*)(
f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{ struct Cyc_List_List*
_temp2796= f->args; goto _LL2797; _LL2797: for( 0; _temp2796 != 0; _temp2796=((
struct Cyc_List_List*) _check_null( _temp2796))->tl){(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp2796))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp2796))->hd)).f3);{
struct _tuple0* _temp2799=({ struct _tuple0* _temp2798=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2798->f1=( void*) Cyc_Absyn_Loc_n;
_temp2798->f2=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2796))->hd)).f1; _temp2798;}); goto _LL2800; _LL2800: nv= Cyc_Toc_add_varmap(
nv, _temp2799, Cyc_Absyn_var_exp( _temp2799, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2803; void* _temp2805; void* _temp2807; struct Cyc_Absyn_Tqual _temp2809;
struct Cyc_Core_Opt* _temp2811; struct Cyc_Absyn_VarargInfo _temp2801=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2812: _temp2811=
_temp2801.name; goto _LL2810; _LL2810: _temp2809= _temp2801.tq; goto _LL2808;
_LL2808: _temp2807=( void*) _temp2801.type; goto _LL2806; _LL2806: _temp2805=(
void*) _temp2801.rgn; goto _LL2804; _LL2804: _temp2803= _temp2801.inject; goto
_LL2802; _LL2802: { void* _temp2813= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2807, _temp2805, _temp2809)); goto _LL2814; _LL2814: { struct _tuple0*
_temp2816=({ struct _tuple0* _temp2815=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2815->f1=( void*) Cyc_Absyn_Loc_n; _temp2815->f2=( struct
_tagged_string*)(( struct Cyc_Core_Opt*) _check_null( _temp2811))->v; _temp2815;});
goto _LL2817; _LL2817: f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2818=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2818->hd=( void*)({ struct _tuple17* _temp2819=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp2819->f1=( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( _temp2811))->v; _temp2819->f2= _temp2809;
_temp2819->f3= _temp2813; _temp2819;}); _temp2818->tl= 0; _temp2818;})); nv= Cyc_Toc_add_varmap(
nv, _temp2816, Cyc_Absyn_var_exp( _temp2816, 0)); f->cyc_varargs= 0;}}}{ struct
Cyc_List_List* _temp2820=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; goto _LL2821; _LL2821: for( 0; _temp2820 !=
0; _temp2820=(( struct Cyc_List_List*) _check_null( _temp2820))->tl){( void*)(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp2820))->hd)->type=(
void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp2820))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2822= s;
_LL2824: if( _temp2822 ==( void*) Cyc_Absyn_Abstract){ goto _LL2825;} else{ goto
_LL2826;} _LL2826: if( _temp2822 ==( void*) Cyc_Absyn_ExternC){ goto _LL2827;}
else{ goto _LL2828;} _LL2828: goto _LL2829; _LL2825: return( void*) Cyc_Absyn_Public;
_LL2827: return( void*) Cyc_Absyn_Extern; _LL2829: return s; _LL2823:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2830=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2831; _LL2831: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2832=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2832->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2832;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2833=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2830).f2); goto _LL2834;
_LL2834: if( _temp2833 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2830).f2, s));}
else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null(
_temp2833))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2830).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2835=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2835->v=( void*)
_temp2830; _temp2835;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2836=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2837; _LL2837: for( 0; _temp2836 != 0;
_temp2836=(( struct Cyc_List_List*) _check_null( _temp2836))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2836))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2836))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2838=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2839; _LL2839: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2840=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2840->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2840;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2841=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2838).f2); goto _LL2842; _LL2842:
if( _temp2841 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2838).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2841))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2838).f2, u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)(
u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==(
void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*)
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2843=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2843->v=( void*) _temp2838; _temp2843;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2844=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( u->fields))->v; goto
_LL2845; _LL2845: for( 0; _temp2844 != 0; _temp2844=(( struct Cyc_List_List*)
_check_null( _temp2844))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2844))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2844))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2846=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2846->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2846;});}{ struct
_tuple0* _temp2847= tud->name; goto _LL2848; _LL2848: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2847)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2847));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2849=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2850; _LL2850: for( 0;
_temp2849 != 0; _temp2849=(( struct Cyc_List_List*) _check_null( _temp2849))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2849))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2851; _temp2851.q_const= 1; _temp2851.q_volatile= 0;
_temp2851.q_restrict= 0; _temp2851;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2852=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2852->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2853=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2853[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2854; _temp2854.tag= Cyc_Absyn_Var_d;
_temp2854.f1= vd; _temp2854;}); _temp2853;}), 0); _temp2852->tl= Cyc_Toc_result_decls;
_temp2852;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2855; _temp2855.q_const=
1; _temp2855.q_volatile= 0; _temp2855.q_restrict= 0; _temp2855;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2856=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2856->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2857=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2857[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2858; _temp2858.tag= Cyc_Absyn_Var_d; _temp2858.f1= vd; _temp2858;});
_temp2857;}), 0); _temp2856->tl= Cyc_Toc_result_decls; _temp2856;});{ struct Cyc_List_List*
_temp2859= 0; goto _LL2860; _LL2860: { int i= 1;{ struct Cyc_List_List*
_temp2861= f->typs; goto _LL2862; _LL2862: for( 0; _temp2861 != 0;( _temp2861=((
struct Cyc_List_List*) _check_null( _temp2861))->tl, i ++)){ struct
_tagged_string* _temp2863= Cyc_Toc_fieldname( i); goto _LL2864; _LL2864: {
struct Cyc_Absyn_Structfield* _temp2866=({ struct Cyc_Absyn_Structfield*
_temp2865=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2865->name= _temp2863; _temp2865->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2861))->hd)).f1; _temp2865->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2861))->hd)).f2);
_temp2865->width= 0; _temp2865->attributes= 0; _temp2865;}); goto _LL2867;
_LL2867: _temp2859=({ struct Cyc_List_List* _temp2868=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2868->hd=( void*) _temp2866;
_temp2868->tl= _temp2859; _temp2868;});}}} _temp2859=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2859); _temp2859=({ struct Cyc_List_List*
_temp2869=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2869->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2870=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2870->name= Cyc_Toc_tag_sp;
_temp2870->tq= Cyc_Toc_mt_tq; _temp2870->type=( void*) Cyc_Absyn_sint_t;
_temp2870->width= 0; _temp2870->attributes= 0; _temp2870;}); _temp2869->tl=
_temp2859; _temp2869;});{ struct Cyc_Absyn_Structdecl* _temp2876=({ struct Cyc_Absyn_Structdecl*
_temp2871=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2871->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2871->name=({ struct Cyc_Core_Opt*
_temp2873=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2873->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2874="_struct"; struct _tagged_string _temp2875; _temp2875.curr= _temp2874;
_temp2875.base= _temp2874; _temp2875.last_plus_one= _temp2874 + 8; _temp2875;}));
_temp2873;}); _temp2871->tvs= 0; _temp2871->fields=({ struct Cyc_Core_Opt*
_temp2872=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2872->v=( void*) _temp2859; _temp2872;}); _temp2871->attributes= 0;
_temp2871;}); goto _LL2877; _LL2877: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2878=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2878->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2879=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2879[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2880; _temp2880.tag= Cyc_Absyn_Struct_d;
_temp2880.f1= _temp2876; _temp2880;}); _temp2879;}), 0); _temp2878->tl= Cyc_Toc_result_decls;
_temp2878;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2881=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2881->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2881;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2882= xd->name; goto _LL2883; _LL2883: {
struct Cyc_List_List* _temp2884=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2885; _LL2885: for( 0; _temp2884 != 0;
_temp2884=(( struct Cyc_List_List*) _check_null( _temp2884))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2884))->hd; struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2887= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2886=* fn;(
unsigned int)( _temp2886.last_plus_one - _temp2886.curr);}) + 4, 0); goto
_LL2888; _LL2888: { struct Cyc_Absyn_ArrayType_struct* _temp2891=({ struct Cyc_Absyn_ArrayType_struct*
_temp2889=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2889[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2890; _temp2890.tag= Cyc_Absyn_ArrayType;
_temp2890.f1=( void*) Cyc_Absyn_uchar_t; _temp2890.f2= Cyc_Toc_mt_tq; _temp2890.f3=(
struct Cyc_Absyn_Exp*) _temp2887; _temp2890;}); _temp2889;}); goto _LL2892;
_LL2892: { struct Cyc_Core_Opt* _temp2893=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2901; int _temp2902; _LL2895: if( _temp2893 == 0){ goto
_LL2896;} else{ goto _LL2897;} _LL2897: if( _temp2893 == 0){ goto _LL2899;}
else{ _temp2901=* _temp2893; _LL2903: _temp2902=( int) _temp2901.v; if(
_temp2902 == 0){ goto _LL2898;} else{ goto _LL2899;}} _LL2899: goto _LL2900;
_LL2896: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(({ int _temp2904= 0; int
_temp2905= 0; int _temp2906= 0; int _temp2907= 0; struct _tagged_string
_temp2908=* fn; xprintf("%c%c%c%c%.*s", _temp2904, _temp2905, _temp2906,
_temp2907, _temp2908.last_plus_one - _temp2908.curr, _temp2908.curr);}), 0);}{
struct Cyc_Absyn_Vardecl* _temp2909= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2891, initopt); goto _LL2910; _LL2910:( void*)( _temp2909->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2911=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2911->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2912=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2912[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2913; _temp2913.tag= Cyc_Absyn_Var_d;
_temp2913.f1= _temp2909; _temp2913;}); _temp2912;}), 0); _temp2911->tl= Cyc_Toc_result_decls;
_temp2911;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2914= f->typs; goto _LL2915; _LL2915: for( 0;
_temp2914 != 0;( _temp2914=(( struct Cyc_List_List*) _check_null( _temp2914))->tl,
i ++)){ struct _tagged_string* _temp2917=( struct _tagged_string*)({ struct
_tagged_string* _temp2916=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2916[ 0]= xprintf("f%d", i); _temp2916;}); goto
_LL2918; _LL2918: { struct Cyc_Absyn_Structfield* _temp2920=({ struct Cyc_Absyn_Structfield*
_temp2919=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2919->name= _temp2917; _temp2919->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2914))->hd)).f1; _temp2919->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2914))->hd)).f2);
_temp2919->width= 0; _temp2919->attributes= 0; _temp2919;}); goto _LL2921;
_LL2921: fields=({ struct Cyc_List_List* _temp2922=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2922->hd=( void*) _temp2920;
_temp2922->tl= fields; _temp2922;});}}} fields=({ struct Cyc_List_List*
_temp2923=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2923->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2924=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2924->name= Cyc_Toc_tag_sp;
_temp2924->tq= Cyc_Toc_mt_tq; _temp2924->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2924->width= 0; _temp2924->attributes= 0; _temp2924;});
_temp2923->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2923;});{ struct Cyc_Absyn_Structdecl* _temp2930=({ struct Cyc_Absyn_Structdecl*
_temp2925=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2925->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2925->name=({ struct Cyc_Core_Opt*
_temp2927=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2927->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2928="_struct"; struct _tagged_string _temp2929; _temp2929.curr= _temp2928;
_temp2929.base= _temp2928; _temp2929.last_plus_one= _temp2928 + 8; _temp2929;}));
_temp2927;}); _temp2925->tvs= 0; _temp2925->fields=({ struct Cyc_Core_Opt*
_temp2926=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2926->v=( void*) fields; _temp2926;}); _temp2925->attributes= 0; _temp2925;});
goto _LL2931; _LL2931: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2932=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2932->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2933=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2933[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2934; _temp2934.tag= Cyc_Absyn_Struct_d;
_temp2934.f1= _temp2930; _temp2934;}); _temp2933;}), 0); _temp2932->tl= Cyc_Toc_result_decls;
_temp2932;});}} goto _LL2894;}} _LL2898: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2940= Cyc_Absyn_string_exp(({ int _temp2935= 0; int
_temp2936= 0; int _temp2937= 0; int _temp2938= 0; struct _tagged_string
_temp2939=* fn; xprintf("%c%c%c%c%.*s", _temp2935, _temp2936, _temp2937,
_temp2938, _temp2939.last_plus_one - _temp2939.curr, _temp2939.curr);}), 0);
goto _LL2941; _LL2941: { struct Cyc_Absyn_Vardecl* _temp2942= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2891,( struct Cyc_Absyn_Exp*) _temp2940); goto _LL2943;
_LL2943:( void*)( _temp2942->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2944=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2944->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2945=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2945[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2946; _temp2946.tag= Cyc_Absyn_Var_d; _temp2946.f1= _temp2942; _temp2946;});
_temp2945;}), 0); _temp2944->tl= Cyc_Toc_result_decls; _temp2944;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2894; _LL2900: goto
_LL2894; _LL2894:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2947= ed->name; goto
_LL2948; _LL2948:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2949=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2950; _LL2950: for( 0; _temp2949 != 0;
_temp2949=(( struct Cyc_List_List*) _check_null( _temp2949))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2949))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2949))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2949))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct _RegionHandle
_temp2951= _new_region(); struct _RegionHandle* prgn=& _temp2951; _push_region(
prgn);{ struct Cyc_Absyn_Stmt* _temp2954; struct Cyc_List_List* _temp2956;
struct Cyc_Toc_Env* _temp2958; struct _tuple13 _temp2952= Cyc_Toc_xlate_pat( nv,
prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2959: _temp2958= _temp2952.f1; goto _LL2957; _LL2957: _temp2956=
_temp2952.f2; goto _LL2955; _LL2955: _temp2954= _temp2952.f3; goto _LL2953;
_LL2953: Cyc_Toc_stmt_to_c( _temp2958, s);{ struct Cyc_Absyn_Stmt* _temp2960=
Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL2961; _LL2961: if( exhaust){ s=
Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2954, _temp2960, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2963= Cyc_Toc_newthrow_exp(
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2962=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2962->f1= Cyc_Toc_abs_ns; _temp2962->f2= Cyc_Toc_Match_Exception_sp;
_temp2962;}), 0)); goto _LL2964; _LL2964: { struct Cyc_Absyn_Stmt* _temp2965=
Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2963, 0), 0); goto
_LL2966; _LL2966: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2954, Cyc_Absyn_seq_stmt( _temp2965,
_temp2960, 0), 0), 0);}} for( 0; _temp2956 != 0; _temp2956=(( struct Cyc_List_List*)
_check_null( _temp2956))->tl){ struct _tuple14 _temp2969; void* _temp2970;
struct _tuple0* _temp2972; struct _tuple14* _temp2967=( struct _tuple14*)((
struct Cyc_List_List*) _check_null( _temp2956))->hd; _temp2969=* _temp2967;
_LL2973: _temp2972= _temp2969.f1; goto _LL2971; _LL2971: _temp2970= _temp2969.f2;
goto _LL2968; _LL2968: s= Cyc_Absyn_declare_stmt( _temp2972, _temp2970, 0, s, 0);}}};
_pop_region(& _temp2951);} return s;}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=((
struct Cyc_List_List*) _check_null( ds))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2974=(
void*) d->r; struct Cyc_Absyn_Vardecl* _temp3000; struct Cyc_Absyn_Fndecl*
_temp3002; int _temp3004; struct Cyc_Absyn_Exp* _temp3006; struct Cyc_Core_Opt*
_temp3008; struct Cyc_Core_Opt* _temp3010; struct Cyc_Absyn_Pat* _temp3012;
struct Cyc_List_List* _temp3014; struct Cyc_Absyn_Structdecl* _temp3016; struct
Cyc_Absyn_Uniondecl* _temp3018; struct Cyc_Absyn_Tuniondecl* _temp3020; struct
Cyc_Absyn_Enumdecl* _temp3022; struct Cyc_Absyn_Typedefdecl* _temp3024; struct
Cyc_List_List* _temp3026; struct _tagged_string* _temp3028; struct Cyc_List_List*
_temp3030; struct _tuple0* _temp3032; struct Cyc_List_List* _temp3034; _LL2976:
if(*(( int*) _temp2974) == Cyc_Absyn_Var_d){ _LL3001: _temp3000=(( struct Cyc_Absyn_Var_d_struct*)
_temp2974)->f1; goto _LL2977;} else{ goto _LL2978;} _LL2978: if(*(( int*)
_temp2974) == Cyc_Absyn_Fn_d){ _LL3003: _temp3002=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2974)->f1; goto _LL2979;} else{ goto _LL2980;} _LL2980: if(*(( int*)
_temp2974) == Cyc_Absyn_Let_d){ _LL3013: _temp3012=(( struct Cyc_Absyn_Let_d_struct*)
_temp2974)->f1; goto _LL3011; _LL3011: _temp3010=(( struct Cyc_Absyn_Let_d_struct*)
_temp2974)->f2; goto _LL3009; _LL3009: _temp3008=(( struct Cyc_Absyn_Let_d_struct*)
_temp2974)->f3; goto _LL3007; _LL3007: _temp3006=(( struct Cyc_Absyn_Let_d_struct*)
_temp2974)->f4; goto _LL3005; _LL3005: _temp3004=(( struct Cyc_Absyn_Let_d_struct*)
_temp2974)->f5; goto _LL2981;} else{ goto _LL2982;} _LL2982: if(*(( int*)
_temp2974) == Cyc_Absyn_Letv_d){ _LL3015: _temp3014=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2974)->f1; goto _LL2983;} else{ goto _LL2984;} _LL2984: if(*(( int*)
_temp2974) == Cyc_Absyn_Struct_d){ _LL3017: _temp3016=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2974)->f1; goto _LL2985;} else{ goto _LL2986;} _LL2986: if(*(( int*)
_temp2974) == Cyc_Absyn_Union_d){ _LL3019: _temp3018=(( struct Cyc_Absyn_Union_d_struct*)
_temp2974)->f1; goto _LL2987;} else{ goto _LL2988;} _LL2988: if(*(( int*)
_temp2974) == Cyc_Absyn_Tunion_d){ _LL3021: _temp3020=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2974)->f1; goto _LL2989;} else{ goto _LL2990;} _LL2990: if(*(( int*)
_temp2974) == Cyc_Absyn_Enum_d){ _LL3023: _temp3022=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2974)->f1; goto _LL2991;} else{ goto _LL2992;} _LL2992: if(*(( int*)
_temp2974) == Cyc_Absyn_Typedef_d){ _LL3025: _temp3024=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2974)->f1; goto _LL2993;} else{ goto _LL2994;} _LL2994: if(*(( int*)
_temp2974) == Cyc_Absyn_Namespace_d){ _LL3029: _temp3028=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2974)->f1; goto _LL3027; _LL3027: _temp3026=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2974)->f2; goto _LL2995;} else{ goto _LL2996;} _LL2996: if(*(( int*)
_temp2974) == Cyc_Absyn_Using_d){ _LL3033: _temp3032=(( struct Cyc_Absyn_Using_d_struct*)
_temp2974)->f1; goto _LL3031; _LL3031: _temp3030=(( struct Cyc_Absyn_Using_d_struct*)
_temp2974)->f2; goto _LL2997;} else{ goto _LL2998;} _LL2998: if(*(( int*)
_temp2974) == Cyc_Absyn_ExternC_d){ _LL3035: _temp3034=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2974)->f1; goto _LL2999;} else{ goto _LL2975;} _LL2977: { struct _tuple0*
_temp3036= _temp3000->name; goto _LL3037; _LL3037: if(( void*) _temp3000->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3036=({ struct _tuple0* _temp3038=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3038->f1= Cyc_Toc_abs_ns;
_temp3038->f2=(* _temp3036).f2; _temp3038;});} if( _temp3000->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null( _temp3000->initializer));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp3039="decls_to_c: not at toplevel!"; struct _tagged_string
_temp3040; _temp3040.curr= _temp3039; _temp3040.base= _temp3039; _temp3040.last_plus_one=
_temp3039 + 29; _temp3040;}));} nv= Cyc_Toc_add_varmap( nv, _temp3000->name, Cyc_Absyn_varb_exp(
_temp3036,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp3041=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp3041[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp3042; _temp3042.tag= Cyc_Absyn_Global_b;
_temp3042.f1= _temp3000; _temp3042;}); _temp3041;}), 0)); _temp3000->name=
_temp3036;( void*)( _temp3000->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp3000->sc));(
void*)( _temp3000->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp3000->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3043=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3043->hd=( void*) d; _temp3043->tl=
Cyc_Toc_result_decls; _temp3043;}); goto _LL2975;} _LL2979: { struct _tuple0*
_temp3044= _temp3002->name; goto _LL3045; _LL3045: if(( void*) _temp3002->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3044=({ struct _tuple0* _temp3046=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3046->f1= Cyc_Toc_abs_ns;
_temp3046->f2=(* _temp3044).f2; _temp3046;});} nv= Cyc_Toc_add_varmap( nv,
_temp3002->name, Cyc_Absyn_var_exp( _temp3044, 0)); _temp3002->name= _temp3044;
Cyc_Toc_fndecl_to_c( nv, _temp3002); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3047=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3047->hd=( void*) d; _temp3047->tl= Cyc_Toc_result_decls; _temp3047;});
goto _LL2975;} _LL2981:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp3048="letdecl"; struct _tagged_string _temp3049; _temp3049.curr=
_temp3048; _temp3049.base= _temp3048; _temp3049.last_plus_one= _temp3048 + 8;
_temp3049;})); goto _LL2975; _LL2983:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp3050="letdecl"; struct _tagged_string _temp3051; _temp3051.curr=
_temp3050; _temp3051.base= _temp3050; _temp3051.last_plus_one= _temp3050 + 8;
_temp3051;})); goto _LL2975; _LL2985: Cyc_Toc_structdecl_to_c( _temp3016); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3052=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3052->hd=( void*) d; _temp3052->tl= Cyc_Toc_result_decls;
_temp3052;}); goto _LL2975; _LL2987: Cyc_Toc_uniondecl_to_c( _temp3018); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3053=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3053->hd=( void*) d; _temp3053->tl= Cyc_Toc_result_decls;
_temp3053;}); goto _LL2975; _LL2989: if( _temp3020->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp3020);} else{ Cyc_Toc_tuniondecl_to_c( _temp3020);} goto _LL2975; _LL2991:
Cyc_Toc_enumdecl_to_c( nv, _temp3022); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3054=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3054->hd=( void*) d; _temp3054->tl= Cyc_Toc_result_decls; _temp3054;});
goto _LL2975; _LL2993: _temp3024->name= _temp3024->name; _temp3024->tvs= 0;(
void*)( _temp3024->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp3024->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3055=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3055->hd=( void*) d; _temp3055->tl=
Cyc_Toc_result_decls; _temp3055;}); goto _LL2975; _LL2995: _temp3030= _temp3026;
goto _LL2997; _LL2997: _temp3034= _temp3030; goto _LL2999; _LL2999: nv= Cyc_Toc_decls_to_c(
nv, _temp3034, top); goto _LL2975; _LL2975:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}