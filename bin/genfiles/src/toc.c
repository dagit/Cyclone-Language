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
extern struct _tagged_string Cstring_to_string( unsigned char*); struct
_tagged_ptr0{ struct _tagged_string* curr; struct _tagged_string* base; struct
_tagged_string* last_plus_one; } ; extern struct _tagged_ptr0 ntCsl_to_ntsl(
unsigned char**); extern int system( unsigned char*); extern int* __errno();
struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr;
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern unsigned char
Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u];
struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct _tagged_string
f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; struct
_tagged_ptr1{ void** curr; void** base; void** last_plus_one; } ; extern struct
Cyc_List_List* Cyc_List_list( struct _tagged_ptr1); extern struct Cyc_List_List*
Cyc_List_rlist( struct _RegionHandle*, struct _tagged_ptr1); extern int Cyc_List_length(
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
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
struct _tagged_ptr1 arr); extern int Cyc_String_zstrptrcmp( struct
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
_temp204;});}} struct _tagged_ptr2{ struct Cyc_Absyn_Structfield** curr; struct
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
_temp216)( struct _tagged_ptr2)=( struct Cyc_List_List*(*)( struct _tagged_ptr2))
Cyc_List_list; struct Cyc_Absyn_Structfield* _temp219= _temp208; struct Cyc_Absyn_Structfield*
_temp220= _temp211; struct Cyc_Absyn_Structfield* _temp221= _temp214; struct Cyc_Absyn_Structfield*
_temp217[ 3u]={ _temp219, _temp220, _temp221}; struct _tagged_ptr2 _temp218={
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
1u]={& Cyc_Toc_f0}; struct _tagged_ptr3{ struct _tagged_string** curr; struct
_tagged_string** base; struct _tagged_string** last_plus_one; } ; static struct
_tagged_ptr3 Cyc_Toc_field_names={( struct _tagged_string**) Cyc_Toc_field_names_v,(
struct _tagged_string**) Cyc_Toc_field_names_v,( struct _tagged_string**) Cyc_Toc_field_names_v
+ 1u}; static struct _tagged_string* Cyc_Toc_fieldname( int i){ unsigned int fsz=({
struct _tagged_ptr3 _temp749= Cyc_Toc_field_names;( unsigned int)( _temp749.last_plus_one
- _temp749.curr);}); if( i >= fsz){ Cyc_Toc_field_names=({ unsigned int _temp744=(
unsigned int)( i + 1); struct _tagged_string** _temp745=( struct _tagged_string**)
GC_malloc( sizeof( struct _tagged_string*) * _temp744); struct _tagged_ptr3
_temp748={ _temp745, _temp745, _temp745 + _temp744};{ unsigned int _temp746=
_temp744; unsigned int j; for( j= 0; j < _temp746; j ++){ _temp745[ j]= j < fsz?*((
struct _tagged_string**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Toc_field_names, sizeof( struct _tagged_string*),(
int) j):({ struct _tagged_string* _temp747=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string)); _temp747[ 0]=( struct _tagged_string) xprintf("f%d",(
int) j); _temp747;});}}; _temp748;});} return*(( struct _tagged_string**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Toc_field_names,
sizeof( struct _tagged_string*), i);} static const int Cyc_Toc_NullCheck= 0;
struct Cyc_Toc_NullCheck_struct{ int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3; } ; static const int Cyc_Toc_UntagPtrNocheck= 2; struct Cyc_Toc_UntagPtrNocheck_struct{
int tag; void* f1; } ; static const int Cyc_Toc_TagPtr= 3; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Toc_NoConv=
0; struct _tuple7{ void* f1; void* f2; } ; static void* Cyc_Toc_conversion( void*
t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp751=({ struct _tuple7 _temp750; _temp750.f1= t1; _temp750.f2=
t2; _temp750;}); void* _temp759; struct Cyc_Absyn_PtrInfo _temp761; void*
_temp763; struct Cyc_Absyn_PtrInfo _temp765; void* _temp767; void* _temp769;
void* _temp771; void* _temp773; struct Cyc_Absyn_PtrInfo _temp775; _LL753:
_LL764: _temp763= _temp751.f1; if(( unsigned int) _temp763 > 4u?*(( int*)
_temp763) == Cyc_Absyn_PointerType: 0){ _LL766: _temp765=(( struct Cyc_Absyn_PointerType_struct*)
_temp763)->f1; goto _LL760;} else{ goto _LL755;} _LL760: _temp759= _temp751.f2;
if(( unsigned int) _temp759 > 4u?*(( int*) _temp759) == Cyc_Absyn_PointerType: 0){
_LL762: _temp761=(( struct Cyc_Absyn_PointerType_struct*) _temp759)->f1; goto
_LL754;} else{ goto _LL755;} _LL755: _LL774: _temp773= _temp751.f1; if((
unsigned int) _temp773 > 4u?*(( int*) _temp773) == Cyc_Absyn_PointerType: 0){
_LL776: _temp775=(( struct Cyc_Absyn_PointerType_struct*) _temp773)->f1; goto
_LL768;} else{ goto _LL757;} _LL768: _temp767= _temp751.f2; if(( unsigned int)
_temp767 > 4u?*(( int*) _temp767) == Cyc_Absyn_IntType: 0){ _LL772: _temp771=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp767)->f1; goto _LL770; _LL770:
_temp769=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp767)->f2; goto _LL756;}
else{ goto _LL757;} _LL757: goto _LL758; _LL754: { int _temp777=(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp765.nullable); goto
_LL778; _LL778: { int _temp779=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp761.nullable); goto _LL780; _LL780: { void* _temp781=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp765.bounds); goto _LL782;
_LL782: { void* _temp783=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp761.bounds); goto _LL784; _LL784: { struct _tuple7 _temp786=({ struct
_tuple7 _temp785; _temp785.f1= _temp781; _temp785.f2= _temp783; _temp785;});
void* _temp796; struct Cyc_Absyn_Exp* _temp798; void* _temp800; struct Cyc_Absyn_Exp*
_temp802; void* _temp804; void* _temp806; void* _temp808; void* _temp810; struct
Cyc_Absyn_Exp* _temp812; void* _temp814; struct Cyc_Absyn_Exp* _temp816; void*
_temp818; _LL788: _LL801: _temp800= _temp786.f1; if(( unsigned int) _temp800 > 1u?*((
int*) _temp800) == Cyc_Absyn_Upper_b: 0){ _LL803: _temp802=(( struct Cyc_Absyn_Upper_b_struct*)
_temp800)->f1; goto _LL797;} else{ goto _LL790;} _LL797: _temp796= _temp786.f2;
if(( unsigned int) _temp796 > 1u?*(( int*) _temp796) == Cyc_Absyn_Upper_b: 0){
_LL799: _temp798=(( struct Cyc_Absyn_Upper_b_struct*) _temp796)->f1; goto _LL789;}
else{ goto _LL790;} _LL790: _LL807: _temp806= _temp786.f1; if( _temp806 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL805;} else{ goto _LL792;} _LL805: _temp804=
_temp786.f2; if( _temp804 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL791;} else{
goto _LL792;} _LL792: _LL811: _temp810= _temp786.f1; if(( unsigned int) _temp810
> 1u?*(( int*) _temp810) == Cyc_Absyn_Upper_b: 0){ _LL813: _temp812=(( struct
Cyc_Absyn_Upper_b_struct*) _temp810)->f1; goto _LL809;} else{ goto _LL794;}
_LL809: _temp808= _temp786.f2; if( _temp808 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL793;} else{ goto _LL794;} _LL794: _LL819: _temp818= _temp786.f1; if(
_temp818 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL815;} else{ goto _LL787;}
_LL815: _temp814= _temp786.f2; if(( unsigned int) _temp814 > 1u?*(( int*)
_temp814) == Cyc_Absyn_Upper_b: 0){ _LL817: _temp816=(( struct Cyc_Absyn_Upper_b_struct*)
_temp814)->f1; goto _LL795;} else{ goto _LL787;} _LL789: if( _temp777? !
_temp779: 0){ return( void*)({ struct Cyc_Toc_NullCheck_struct* _temp820=(
struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp820[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp821; _temp821.tag= Cyc_Toc_NullCheck;
_temp821.f1=( void*) t2; _temp821;}); _temp820;});} return( void*) Cyc_Toc_NoConv;
_LL791: return( void*) Cyc_Toc_NoConv; _LL793: return( void*)({ struct Cyc_Toc_TagPtr_struct*
_temp822=( struct Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp822[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp823; _temp823.tag= Cyc_Toc_TagPtr;
_temp823.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp823.f2= _temp812; _temp823;});
_temp822;}); _LL795: return( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp824=(
struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp824[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp825; _temp825.tag= Cyc_Toc_UntagPtr;
_temp825.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp825.f2= _temp816; _temp825.f3=
_temp779; _temp825;}); _temp824;}); _LL787:;}}}}} _LL756: { void* _temp826=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp775.bounds);
goto _LL827; _LL827: { void* _temp828= _temp826; struct Cyc_Absyn_Exp* _temp834;
_LL830: if(( unsigned int) _temp828 > 1u?*(( int*) _temp828) == Cyc_Absyn_Upper_b:
0){ _LL835: _temp834=(( struct Cyc_Absyn_Upper_b_struct*) _temp828)->f1; goto
_LL831;} else{ goto _LL832;} _LL832: if( _temp828 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL833;} else{ goto _LL829;} _LL831: return( void*) Cyc_Toc_NoConv; _LL833:
return( void*)({ struct Cyc_Toc_UntagPtrNocheck_struct* _temp836=( struct Cyc_Toc_UntagPtrNocheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_UntagPtrNocheck_struct)); _temp836[ 0]=({
struct Cyc_Toc_UntagPtrNocheck_struct _temp837; _temp837.tag= Cyc_Toc_UntagPtrNocheck;
_temp837.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp837;}); _temp836;}); _LL829:;}}
_LL758: return( void*) Cyc_Toc_NoConv; _LL752:;}} static unsigned char _temp840[
14u]="sfile_to_file"; static struct _tagged_string Cyc_Toc__sfile_to_file_string={
_temp840, _temp840, _temp840 + 14u}; static struct _tuple0 Cyc_Toc__sfile_to_file_var={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__sfile_to_file_string}; static struct Cyc_Absyn_Exp*
Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(&
Cyc_Toc__sfile_to_file_var, 0),({ struct Cyc_List_List* _temp841=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp841->hd=( void*) e; _temp841->tl=
0; _temp841;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct
Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__throw_qv,
0),({ struct Cyc_List_List* _temp842=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp842->hd=( void*) e; _temp842->tl= 0; _temp842;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp843=(
unsigned char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp844; _temp844.curr= _temp843; _temp844.base= _temp843;
_temp844.last_plus_one= _temp843 + 44; _temp844;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp845=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp845->hd=( void*) e; _temp845->tl= 0; _temp845;}), 0),
0);} else{ struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0),
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0),
Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
x, 0), 0), 0); return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, new_typ,(
struct Cyc_Absyn_Exp*) e, s, 0), 0);}} struct _tagged_ptr4{ struct _tuple4**
curr; struct _tuple4** base; struct _tuple4** last_plus_one; } ; static struct
Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct Cyc_Absyn_Exp*
sz, struct Cyc_Absyn_Exp* e){ struct _tuple0* a= Cyc_Toc_temp_var(); struct
_tuple0* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ; struct Cyc_Absyn_Tqual elt_tqual;{ void*
_temp846= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp852; struct Cyc_Absyn_Conref*
_temp854; struct Cyc_Absyn_Tqual _temp856; struct Cyc_Absyn_Conref* _temp858;
void* _temp860; void* _temp862; _LL848: if(( unsigned int) _temp846 > 4u?*(( int*)
_temp846) == Cyc_Absyn_PointerType: 0){ _LL853: _temp852=(( struct Cyc_Absyn_PointerType_struct*)
_temp846)->f1; _LL863: _temp862=( void*) _temp852.elt_typ; goto _LL861; _LL861:
_temp860=( void*) _temp852.rgn_typ; goto _LL859; _LL859: _temp858= _temp852.nullable;
goto _LL857; _LL857: _temp856= _temp852.tq; goto _LL855; _LL855: _temp854=
_temp852.bounds; goto _LL849;} else{ goto _LL850;} _LL850: goto _LL851; _LL849:
elt_typ= _temp862; elt_tqual= _temp856; strt_typ= Cyc_Toc_add_tagged_ptr_type(
_temp862, _temp856); goto _LL847; _LL851: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp864=(
unsigned char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp865;
_temp865.curr= _temp864; _temp865.base= _temp864; _temp865.last_plus_one=
_temp864 + 28; _temp865;})); _LL847:;} if( toplevel){ int is_string= 0;{ void*
_temp866=( void*) e->r; void* _temp872; struct _tagged_string _temp874; _LL868:
if(*(( int*) _temp866) == Cyc_Absyn_Const_e){ _LL873: _temp872=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp866)->f1; if(( unsigned int) _temp872 > 1u?*((
int*) _temp872) == Cyc_Absyn_String_c: 0){ _LL875: _temp874=(( struct Cyc_Absyn_String_c_struct*)
_temp872)->f1; goto _LL869;} else{ goto _LL870;}} else{ goto _LL870;} _LL870:
goto _LL871; _LL869: is_string= 1; goto _LL867; _LL871: goto _LL867; _LL867:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp879=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp879[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp880; _temp880.tag= Cyc_Absyn_ArrayType; _temp880.f1=(
void*) elt_typ; _temp880.f2= Cyc_Toc_mt_tq; _temp880.f3=( struct Cyc_Absyn_Exp*)
sz; _temp880;}); _temp879;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp876=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp876->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp877=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp877[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp878; _temp878.tag= Cyc_Absyn_Var_d;
_temp878.f1= vd; _temp878;}); _temp877;}), 0); _temp876->tl= Cyc_Toc_result_decls;
_temp876;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ if( elt_tqual.q_const){ xexp=
Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq), e, 0);} else{
xexp= e;}}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp( xexp, sz, 0);
struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(*
_temp881)( struct _tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4))
Cyc_List_list; struct _tuple4* _temp884=({ struct _tuple4* _temp889=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp889->f1= 0; _temp889->f2=
xexp; _temp889;}); struct _tuple4* _temp885=({ struct _tuple4* _temp888=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp888->f1= 0; _temp888->f2=
xexp; _temp888;}); struct _tuple4* _temp886=({ struct _tuple4* _temp887=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp887->f1= 0; _temp887->f2=
xplussz; _temp887;}); struct _tuple4* _temp882[ 3u]={ _temp884, _temp885,
_temp886}; struct _tagged_ptr4 _temp883={ _temp882, _temp882, _temp882 + 3u};
_temp881( _temp883);}), 0); return urm_exp;}}} else{ struct Cyc_Absyn_Stmt* s=
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
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp890=( unsigned char*)"can't coerce t? to t* or t@ at the top-level";
struct _tagged_string _temp891; _temp891.curr= _temp890; _temp891.base= _temp890;
_temp891.last_plus_one= _temp890 + 45; _temp891;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
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
return({ struct Cyc_Toc_Env* _temp892=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp892->break_lab= 0; _temp892->continue_lab= 0;
_temp892->fallthru_info= 0; _temp892->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp892->toplevel= 1; _temp892;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp893=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp893->break_lab= e->break_lab;
_temp893->continue_lab= e->continue_lab; _temp893->fallthru_info= e->fallthru_info;
_temp893->varmap= e->varmap; _temp893->toplevel= e->toplevel; _temp893;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp894=(* x).f1; struct Cyc_List_List*
_temp900; _LL896: if(( unsigned int) _temp894 > 1u?*(( int*) _temp894) == Cyc_Absyn_Rel_n:
0){ _LL901: _temp900=(( struct Cyc_Absyn_Rel_n_struct*) _temp894)->f1; goto
_LL897;} else{ goto _LL898;} _LL898: goto _LL899; _LL897:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp902=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp902[ 0]=({ struct
Cyc_Core_Impossible_struct _temp903; _temp903.tag= Cyc_Core_Impossible; _temp903.f1=(
struct _tagged_string)({ struct _tagged_string _temp904= Cyc_Absynpp_qvar2string(
x); xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp904.last_plus_one -
_temp904.curr, _temp904.curr);}); _temp903;}); _temp902;})); _LL899: goto _LL895;
_LL895:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp*
data)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env*
Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab= 0; ans->continue_lab= 0; return ans;} struct _tuple8{ struct
_tagged_string* f1; struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ;
static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_string* break_l, struct _tagged_string* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
_temp905= 0; goto _LL906; _LL906: for( 0; fallthru_binders != 0;
fallthru_binders=(( struct Cyc_List_List*) _check_null( fallthru_binders))->tl){
_temp905=({ struct Cyc_List_List* _temp907=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp907->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp907->tl=
_temp905; _temp907;});} _temp905=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp905);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp908=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp908->v=( void*) break_l; _temp908;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp909=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp909->v=( void*)({ struct _tuple8*
_temp910=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp910->f1=
fallthru_l; _temp910->f2= _temp905; _temp910->f3= next_case_env->varmap;
_temp910;}); _temp909;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp911=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp911->v=( void*)
break_l; _temp911;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_string*
next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp912=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp912->v=( void*)({ struct _tuple8* _temp913=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp913->f1= next_l; _temp913->f2= 0;
_temp913->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp913;}); _temp912;});
return ans;} struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tagged_ptr5{ struct Cyc_List_List** curr; struct Cyc_List_List** base;
struct Cyc_List_List** last_plus_one; } ; struct _tuple10{ struct _tuple0* f1;
void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp914=( unsigned char*)"expecting a literal format string";
struct _tagged_string _temp915; _temp915.curr= _temp914; _temp915.base= _temp914;
_temp915.last_plus_one= _temp914 + 34; _temp915;}));}{ struct _tagged_string
fmt_str;{ void* _temp916=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd)->r; void* _temp922; struct _tagged_string _temp924;
_LL918: if(*(( int*) _temp916) == Cyc_Absyn_Const_e){ _LL923: _temp922=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp916)->f1; if(( unsigned int) _temp922 > 1u?*((
int*) _temp922) == Cyc_Absyn_String_c: 0){ _LL925: _temp924=(( struct Cyc_Absyn_String_c_struct*)
_temp922)->f1; goto _LL919;} else{ goto _LL920;}} else{ goto _LL920;} _LL920:
goto _LL921; _LL919: fmt_str= _temp924; goto _LL917; _LL921: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp926=(
unsigned char*)"expecting a literal format string"; struct _tagged_string
_temp927; _temp927.curr= _temp926; _temp927.base= _temp926; _temp927.last_plus_one=
_temp926 + 34; _temp927;})); _LL917:;}{ int len=( int)({ struct _tagged_string
_temp1023= fmt_str;( unsigned int)( _temp1023.last_plus_one - _temp1023.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str,
sizeof( unsigned char), i) =='%'){ i ++; if( i < len?*(( const unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( unsigned char), i) =='s': 0){ break;}}} if( i == len){ if( aopt
!= 0){ args=({ struct Cyc_List_List* _temp928=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp928->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp928->tl= args; _temp928;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp929= _new_region(); struct
_RegionHandle* r=& _temp929; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str,
sizeof( unsigned char), i); rev_fmt=({ struct Cyc_List_List* _temp930=( struct
Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List)); _temp930->hd=(
void*)(( int) c); _temp930->tl= rev_fmt; _temp930;}); if( c !='%'){ continue;}{
struct Cyc_Core_Opt* _temp931= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); goto _LL932; _LL932: if( _temp931 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp933=( unsigned char*)"bad format string";
struct _tagged_string _temp934; _temp934.curr= _temp933; _temp934.base= _temp933;
_temp934.last_plus_one= _temp933 + 18; _temp934;}));}{ struct _tuple9 _temp937;
int _temp938; unsigned char _temp940; struct Cyc_List_List* _temp942; struct Cyc_List_List*
_temp944; struct Cyc_List_List* _temp946; struct Cyc_List_List* _temp948; struct
_tuple9* _temp935=( struct _tuple9*)(( struct Cyc_Core_Opt*) _check_null(
_temp931))->v; _temp937=* _temp935; _LL949: _temp948= _temp937.f1; goto _LL947;
_LL947: _temp946= _temp937.f2; goto _LL945; _LL945: _temp944= _temp937.f3; goto
_LL943; _LL943: _temp942= _temp937.f4; goto _LL941; _LL941: _temp940= _temp937.f5;
goto _LL939; _LL939: _temp938= _temp937.f6; goto _LL936; _LL936: i= _temp938 - 1;
if( _temp940 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp950)( struct _RegionHandle*, struct
_tagged_ptr5)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_ptr5)) Cyc_List_rlist; struct _RegionHandle* _temp957= r; struct Cyc_List_List*
_temp953= _temp948; struct Cyc_List_List* _temp954= _temp946; struct Cyc_List_List*
_temp955= _temp944; struct Cyc_List_List* _temp956= _temp942; struct Cyc_List_List*
_temp951[ 4u]={ _temp953, _temp954, _temp955, _temp956}; struct _tagged_ptr5
_temp952={ _temp951, _temp951, _temp951 + 4u}; _temp950( _temp957, _temp952);})),
rev_fmt); rev_fmt=({ struct Cyc_List_List* _temp958=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp958->hd=( void*)(( int)
_temp940); _temp958->tl= rev_fmt; _temp958;});{ struct Cyc_List_List* _temp959=
_temp946; struct Cyc_List_List _temp966; struct Cyc_List_List* _temp967; int
_temp969; _LL961: if( _temp959 == 0){ goto _LL963;} else{ _temp966=* _temp959;
_LL970: _temp969=( int) _temp966.hd; goto _LL968; _LL968: _temp967= _temp966.tl;
if( _temp967 == 0){ goto _LL965;} else{ goto _LL963;}} _LL965: if( _temp969 =='*'){
goto _LL962;} else{ goto _LL963;} _LL963: goto _LL964; _LL962: { struct _tuple0*
_temp971= Cyc_Toc_temp_var(); goto _LL972; _LL972: rev_temps=({ struct Cyc_List_List*
_temp973=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp973->hd=( void*)({ struct _tuple10* _temp974=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp974->f1= _temp971; _temp974->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp974->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp974;});
_temp973->tl= rev_temps; _temp973;}); rev_result=({ struct Cyc_List_List*
_temp975=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp975->hd=( void*) Cyc_Absyn_var_exp( _temp971, 0); _temp975->tl= rev_result;
_temp975;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL960;} _LL964: goto
_LL960; _LL960:;}{ struct Cyc_List_List* _temp976= _temp944; struct Cyc_List_List
_temp983; struct Cyc_List_List* _temp984; struct Cyc_List_List _temp986; struct
Cyc_List_List* _temp987; int _temp989; int _temp991; _LL978: if( _temp976 == 0){
goto _LL980;} else{ _temp983=* _temp976; _LL992: _temp991=( int) _temp983.hd;
goto _LL985; _LL985: _temp984= _temp983.tl; if( _temp984 == 0){ goto _LL980;}
else{ _temp986=* _temp984; _LL990: _temp989=( int) _temp986.hd; goto _LL988;
_LL988: _temp987= _temp986.tl; if( _temp987 == 0){ goto _LL982;} else{ goto
_LL980;}}} _LL982: if( _temp991 =='.'? _temp989 =='*': 0){ goto _LL979;} else{
goto _LL980;} _LL980: goto _LL981; _LL979: { struct _tuple0* _temp993= Cyc_Toc_temp_var();
goto _LL994; _LL994: rev_temps=({ struct Cyc_List_List* _temp995=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp995->hd=( void*)({
struct _tuple10* _temp996=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp996->f1= _temp993; _temp996->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp996->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp996;}); _temp995->tl= rev_temps; _temp995;});
rev_result=({ struct Cyc_List_List* _temp997=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp997->hd=( void*) Cyc_Absyn_var_exp(
_temp993, 0); _temp997->tl= rev_result; _temp997;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL977;} _LL981: goto _LL977; _LL977:;} if( _temp940 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ struct _tagged_string _temp998= Cyc_Absynpp_prim2string( p);
xprintf("missing arg to %.*s", _temp998.last_plus_one - _temp998.curr, _temp998.curr);}));}{
struct _tuple0* _temp999= Cyc_Toc_temp_var(); goto _LL1000; _LL1000: rev_temps=({
struct Cyc_List_List* _temp1001=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp1001->hd=( void*)({ struct _tuple10*
_temp1002=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1002->f1= _temp999; _temp1002->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp1002->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp1002;}); _temp1001->tl= rev_temps; _temp1001;});
rev_result=({ struct Cyc_List_List* _temp1003=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1003->hd=( void*) Cyc_Absyn_var_exp(
_temp999, 0); _temp1003->tl= rev_result; _temp1003;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp1004=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1004->hd=( void*)(( int)'.');
_temp1004->tl= rev_fmt; _temp1004;}); rev_fmt=({ struct Cyc_List_List* _temp1005=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1005->hd=( void*)(( int)'*'); _temp1005->tl= rev_fmt; _temp1005;}); rev_fmt=({
struct Cyc_List_List* _temp1006=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp1006->hd=( void*)(( int)'s'); _temp1006->tl=
rev_fmt; _temp1006;});{ struct _tuple0* _temp1007= Cyc_Toc_temp_var(); goto
_LL1008; _LL1008: rev_temps=({ struct Cyc_List_List* _temp1009=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1009->hd=( void*)({
struct _tuple10* _temp1010=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1010->f1= _temp1007; _temp1010->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp1010->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp1010;}); _temp1009->tl= rev_temps; _temp1009;});{
struct Cyc_Absyn_Exp* _temp1011= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1007, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1007, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL1012; _LL1012: {
struct Cyc_Absyn_Exp* _temp1013= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1007, 0), Cyc_Toc_curr_sp, 0); goto _LL1014; _LL1014: rev_result=({ struct
Cyc_List_List* _temp1015=( struct Cyc_List_List*) _region_malloc( r, sizeof(
struct Cyc_List_List)); _temp1015->hd=( void*) _temp1011; _temp1015->tl=
rev_result; _temp1015;}); rev_result=({ struct Cyc_List_List* _temp1016=( struct
Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List)); _temp1016->hd=(
void*) _temp1013; _temp1016->tl= rev_result; _temp1016;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}}{
struct Cyc_List_List* _temp1018=({ struct Cyc_List_List* _temp1017=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1017->hd=( void*) Cyc_Absyn_string_exp((
struct _tagged_string) Cyc_String_implode((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0); _temp1017->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_result); _temp1017;}); goto _LL1019;
_LL1019: if( aopt != 0){ _temp1018=({ struct Cyc_List_List* _temp1020=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1020->hd=( void*)((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( aopt))->v);
_temp1020->tl= _temp1018; _temp1020;});}{ struct Cyc_Absyn_Exp* _temp1021= Cyc_Absyn_primop_exp(
p, _temp1018, 0); goto _LL1022; _LL1022: s= Cyc_Absyn_exp_stmt( _temp1021, 0);
for( 0; rev_temps != 0; rev_temps=(( struct Cyc_List_List*) _check_null(
rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f1,(*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f2,( struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)((
struct Cyc_List_List*) _check_null( rev_temps))->hd)).f3, s, 0);}}}};
_pop_region(& _temp929);} return Cyc_Absyn_stmt_exp( s, 0);}}}} static void* Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1024=( unsigned char*)"Missing type in primop ";
struct _tagged_string _temp1025; _temp1025.curr= _temp1024; _temp1025.base=
_temp1024; _temp1025.last_plus_one= _temp1024 + 24; _temp1025;}));} return Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1026=( unsigned char*)"Missing type in primop ";
struct _tagged_string _temp1027; _temp1027.curr= _temp1026; _temp1027.base=
_temp1026; _temp1027.last_plus_one= _temp1026 + 24; _temp1027;}));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1028=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1028->f1= Cyc_Toc_mt_tq; _temp1028->f2=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp1028;});} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp1029=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1029[
0]=({ struct _tuple4 _temp1030; _temp1030.f1= 0; _temp1030.f2= e; _temp1030;});
_temp1029;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
_temp1031= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); goto _LL1032; _LL1032: if(
rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic(
_temp1031): Cyc_Toc_malloc_ptr( _temp1031), 0);} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c( nv, r); eo=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp( r, _temp1031),
0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp1033=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL1034; _LL1034: for( 0;
_temp1033 != 0; _temp1033=(( struct Cyc_List_List*) _check_null( _temp1033))->tl){
struct _tuple4 _temp1037; struct Cyc_Absyn_Exp* _temp1038; struct Cyc_List_List*
_temp1040; struct _tuple4* _temp1035=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1033))->hd; _temp1037=* _temp1035; _LL1041: _temp1040=
_temp1037.f1; goto _LL1039; _LL1039: _temp1038= _temp1037.f2; goto _LL1036;
_LL1036: { struct Cyc_Absyn_Exp* e_index; if( _temp1040 == 0){ e_index= Cyc_Absyn_signed_int_exp(
count --, 0);} else{ if((( struct Cyc_List_List*) _check_null( _temp1040))->tl
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp1042=( unsigned char*)"multiple designators in array"; struct
_tagged_string _temp1043; _temp1043.curr= _temp1042; _temp1043.base= _temp1042;
_temp1043.last_plus_one= _temp1042 + 30; _temp1043;}));}{ void* _temp1044=( void*)((
struct Cyc_List_List*) _check_null( _temp1040))->hd; goto _LL1045; _LL1045: {
void* _temp1046= _temp1044; struct Cyc_Absyn_Exp* _temp1052; struct
_tagged_string* _temp1054; _LL1048: if(*(( int*) _temp1046) == Cyc_Absyn_ArrayElement){
_LL1053: _temp1052=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1046)->f1;
goto _LL1049;} else{ goto _LL1050;} _LL1050: if(*(( int*) _temp1046) == Cyc_Absyn_FieldName){
_LL1055: _temp1054=(( struct Cyc_Absyn_FieldName_struct*) _temp1046)->f1; goto
_LL1051;} else{ goto _LL1047;} _LL1049: Cyc_Toc_exp_to_c( nv, _temp1052);
e_index= _temp1052; goto _LL1047; _LL1051: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp1056=(
unsigned char*)"field name designators in array"; struct _tagged_string
_temp1057; _temp1057.curr= _temp1056; _temp1057.base= _temp1056; _temp1057.last_plus_one=
_temp1056 + 32; _temp1057;})); goto _LL1047; _LL1047:;}}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp1058=( void*)
_temp1038->r; struct Cyc_List_List* _temp1068; struct Cyc_Absyn_Exp* _temp1070;
struct Cyc_Absyn_Exp* _temp1072; struct Cyc_Absyn_Vardecl* _temp1074; struct Cyc_List_List*
_temp1076; void* _temp1078; _LL1060: if(*(( int*) _temp1058) == Cyc_Absyn_Array_e){
_LL1069: _temp1068=(( struct Cyc_Absyn_Array_e_struct*) _temp1058)->f1; goto
_LL1061;} else{ goto _LL1062;} _LL1062: if(*(( int*) _temp1058) == Cyc_Absyn_Comprehension_e){
_LL1075: _temp1074=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1058)->f1;
goto _LL1073; _LL1073: _temp1072=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1058)->f2; goto _LL1071; _LL1071: _temp1070=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1058)->f3; goto _LL1063;} else{ goto _LL1064;} _LL1064: if(*(( int*)
_temp1058) == Cyc_Absyn_AnonStruct_e){ _LL1079: _temp1078=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1058)->f1; goto _LL1077; _LL1077: _temp1076=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1058)->f2; goto _LL1065;} else{ goto _LL1066;} _LL1066: goto _LL1067;
_LL1061: s= Cyc_Toc_init_array( nv, lval, _temp1068, s); goto _LL1059; _LL1063:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1074, _temp1072, _temp1070, s, 0);
goto _LL1059; _LL1065: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1078,
_temp1076, s); goto _LL1059; _LL1067: Cyc_Toc_exp_to_c( nv, _temp1038); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1038, 0),
s, 0); goto _LL1059; _LL1059:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* _temp1080= vd->name; goto _LL1081;
_LL1081: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1080, Cyc_Absyn_varb_exp( _temp1080,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1104=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1104[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1105; _temp1105.tag= Cyc_Absyn_Local_b; _temp1105.f1=
vd; _temp1105;}); _temp1104;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp1080, 0),
Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( _temp1080, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp*
ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( _temp1080, 0), 0); struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( _temp1080, 0), 0); struct
Cyc_Absyn_Stmt* body;{ void* _temp1082=( void*) e2->r; struct Cyc_List_List*
_temp1092; struct Cyc_Absyn_Exp* _temp1094; struct Cyc_Absyn_Exp* _temp1096;
struct Cyc_Absyn_Vardecl* _temp1098; struct Cyc_List_List* _temp1100; void*
_temp1102; _LL1084: if(*(( int*) _temp1082) == Cyc_Absyn_Array_e){ _LL1093:
_temp1092=(( struct Cyc_Absyn_Array_e_struct*) _temp1082)->f1; goto _LL1085;}
else{ goto _LL1086;} _LL1086: if(*(( int*) _temp1082) == Cyc_Absyn_Comprehension_e){
_LL1099: _temp1098=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1082)->f1;
goto _LL1097; _LL1097: _temp1096=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1082)->f2; goto _LL1095; _LL1095: _temp1094=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1082)->f3; goto _LL1087;} else{ goto _LL1088;} _LL1088: if(*(( int*)
_temp1082) == Cyc_Absyn_AnonStruct_e){ _LL1103: _temp1102=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1082)->f1; goto _LL1101; _LL1101: _temp1100=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1082)->f2; goto _LL1089;} else{ goto _LL1090;} _LL1090: goto _LL1091;
_LL1085: body= Cyc_Toc_init_array( nv2, lval, _temp1092, Cyc_Absyn_skip_stmt( 0));
goto _LL1083; _LL1087: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1098,
_temp1096, _temp1094, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1083; _LL1089: body=
Cyc_Toc_init_anon_struct( nv, lval, _temp1102, _temp1100, Cyc_Absyn_skip_stmt( 0));
goto _LL1083; _LL1091: Cyc_Toc_exp_to_c( nv2, e2); body= Cyc_Absyn_assign_stmt(
lval, e2, 0); goto _LL1083; _LL1083:;}{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt(
ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( _temp1080, Cyc_Absyn_uint_t,
0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s){{ struct Cyc_List_List* _temp1106=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto _LL1107; _LL1107: for( 0;
_temp1106 != 0; _temp1106=(( struct Cyc_List_List*) _check_null( _temp1106))->tl){
struct _tuple4 _temp1110; struct Cyc_Absyn_Exp* _temp1111; struct Cyc_List_List*
_temp1113; struct _tuple4* _temp1108=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1106))->hd; _temp1110=* _temp1108; _LL1114: _temp1113=
_temp1110.f1; goto _LL1112; _LL1112: _temp1111= _temp1110.f2; goto _LL1109;
_LL1109: if( _temp1113 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1115=( unsigned char*)"empty designator list"; struct
_tagged_string _temp1116; _temp1116.curr= _temp1115; _temp1116.base= _temp1115;
_temp1116.last_plus_one= _temp1115 + 22; _temp1116;}));} if((( struct Cyc_List_List*)
_check_null( _temp1113))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1117=( unsigned char*)"too many designators in anonymous struct";
struct _tagged_string _temp1118; _temp1118.curr= _temp1117; _temp1118.base=
_temp1117; _temp1118.last_plus_one= _temp1117 + 41; _temp1118;}));}{ void*
_temp1119=( void*)(( struct Cyc_List_List*) _check_null( _temp1113))->hd; struct
_tagged_string* _temp1125; _LL1121: if(*(( int*) _temp1119) == Cyc_Absyn_FieldName){
_LL1126: _temp1125=(( struct Cyc_Absyn_FieldName_struct*) _temp1119)->f1; goto
_LL1122;} else{ goto _LL1123;} _LL1123: goto _LL1124; _LL1122: { struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_structmember_exp( lhs, _temp1125, 0);{ void* _temp1127=( void*)
_temp1111->r; struct Cyc_List_List* _temp1137; struct Cyc_Absyn_Exp* _temp1139;
struct Cyc_Absyn_Exp* _temp1141; struct Cyc_Absyn_Vardecl* _temp1143; struct Cyc_List_List*
_temp1145; void* _temp1147; _LL1129: if(*(( int*) _temp1127) == Cyc_Absyn_Array_e){
_LL1138: _temp1137=(( struct Cyc_Absyn_Array_e_struct*) _temp1127)->f1; goto
_LL1130;} else{ goto _LL1131;} _LL1131: if(*(( int*) _temp1127) == Cyc_Absyn_Comprehension_e){
_LL1144: _temp1143=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1127)->f1;
goto _LL1142; _LL1142: _temp1141=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1127)->f2; goto _LL1140; _LL1140: _temp1139=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1127)->f3; goto _LL1132;} else{ goto _LL1133;} _LL1133: if(*(( int*)
_temp1127) == Cyc_Absyn_AnonStruct_e){ _LL1148: _temp1147=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1127)->f1; goto _LL1146; _LL1146: _temp1145=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1127)->f2; goto _LL1134;} else{ goto _LL1135;} _LL1135: goto _LL1136;
_LL1130: s= Cyc_Toc_init_array( nv, lval, _temp1137, s); goto _LL1128; _LL1132:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1143, _temp1141, _temp1139, s, 0);
goto _LL1128; _LL1134: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1147,
_temp1145, s); goto _LL1128; _LL1136: Cyc_Toc_exp_to_c( nv, _temp1111); if( Cyc_Toc_is_poly_field(
struct_type, _temp1125)){ _temp1111= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1111, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1111, 0), 0), s, 0); goto _LL1128; _LL1128:;} goto _LL1120;} _LL1124:(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1149=( unsigned char*)"array designator in struct"; struct
_tagged_string _temp1150; _temp1150.curr= _temp1149; _temp1150.base= _temp1149;
_temp1150.last_plus_one= _temp1149 + 27; _temp1150;}))); _LL1120:;}}} return s;}
static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int
pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
_RegionHandle _temp1151= _new_region(); struct _RegionHandle* r=& _temp1151;
_push_region( r);{ struct Cyc_List_List* _temp1152=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c, es); goto _LL1153; _LL1153: { struct
_tagged_string* _temp1154= Cyc_Toc_add_tuple_type( _temp1152); goto _LL1155;
_LL1155: { struct _tuple0* _temp1156= Cyc_Toc_temp_var(); goto _LL1157; _LL1157: {
struct Cyc_Absyn_Exp* _temp1158= Cyc_Absyn_var_exp( _temp1156, 0); goto _LL1159;
_LL1159: { struct Cyc_Absyn_Stmt* _temp1160= Cyc_Absyn_exp_stmt( _temp1158, 0);
goto _LL1161; _LL1161: { struct Cyc_Absyn_Exp*(* _temp1162)( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; goto _LL1163; _LL1163: { int is_atomic= 1; struct
Cyc_List_List* _temp1164=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( r, es); goto _LL1165; _LL1165:{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1164); for( 0;
_temp1164 != 0;( _temp1164=(( struct Cyc_List_List*) _check_null( _temp1164))->tl,
-- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1164))->hd; struct Cyc_Absyn_Exp* lval= _temp1162( _temp1158,
Cyc_Toc_fieldname( i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v): 0;{ void* _temp1166=( void*) e->r;
struct Cyc_List_List* _temp1174; struct Cyc_Absyn_Exp* _temp1176; struct Cyc_Absyn_Exp*
_temp1178; struct Cyc_Absyn_Vardecl* _temp1180; _LL1168: if(*(( int*) _temp1166)
== Cyc_Absyn_Array_e){ _LL1175: _temp1174=(( struct Cyc_Absyn_Array_e_struct*)
_temp1166)->f1; goto _LL1169;} else{ goto _LL1170;} _LL1170: if(*(( int*)
_temp1166) == Cyc_Absyn_Comprehension_e){ _LL1181: _temp1180=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1166)->f1; goto _LL1179; _LL1179: _temp1178=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1166)->f2; goto _LL1177; _LL1177: _temp1176=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1166)->f3; goto _LL1171;} else{ goto _LL1172;} _LL1172: goto _LL1173;
_LL1169: _temp1160= Cyc_Toc_init_array( nv, lval, _temp1174, _temp1160); goto
_LL1167; _LL1171: _temp1160= Cyc_Toc_init_comprehension( nv, lval, _temp1180,
_temp1178, _temp1176, _temp1160, 0); goto _LL1167; _LL1173: Cyc_Toc_exp_to_c( nv,
e); _temp1160= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1162( _temp1158, Cyc_Toc_fieldname( i), 0), e, 0), 0), _temp1160, 0); goto
_LL1167; _LL1167:;}}}{ struct Cyc_Absyn_Exp* _temp1182= Cyc_Toc_make_struct( nv,
_temp1156, Cyc_Absyn_strct( _temp1154), _temp1160, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp1182;}}}}}}}}; _pop_region(& _temp1151);} static
struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env* nv, void*
struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp1183= Cyc_Toc_temp_var(); goto
_LL1184; _LL1184: { struct Cyc_Absyn_Exp* _temp1185= Cyc_Absyn_var_exp(
_temp1183, 0); goto _LL1186; _LL1186: { struct Cyc_Absyn_Stmt* _temp1187= Cyc_Absyn_exp_stmt(
_temp1185, 0); goto _LL1188; _LL1188: { struct Cyc_Absyn_Exp*(* _temp1189)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1190;
_LL1190: { int is_atomic= 1; struct _RegionHandle _temp1191= _new_region();
struct _RegionHandle* r=& _temp1191; _push_region( r);{ struct Cyc_List_List*
_temp1192=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, dles); goto _LL1193; _LL1193: for( 0; _temp1192 != 0;
_temp1192=(( struct Cyc_List_List*) _check_null( _temp1192))->tl){ struct
_tuple4 _temp1196; struct Cyc_Absyn_Exp* _temp1197; struct Cyc_List_List*
_temp1199; struct _tuple4* _temp1194=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1192))->hd; _temp1196=* _temp1194; _LL1200: _temp1199=
_temp1196.f1; goto _LL1198; _LL1198: _temp1197= _temp1196.f2; goto _LL1195;
_LL1195: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1197->topt))->v): 0; if( _temp1199 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1201=(
unsigned char*)"empty designator list"; struct _tagged_string _temp1202;
_temp1202.curr= _temp1201; _temp1202.base= _temp1201; _temp1202.last_plus_one=
_temp1201 + 22; _temp1202;}));} if((( struct Cyc_List_List*) _check_null(
_temp1199))->tl != 0){ struct _tuple0* _temp1203= Cyc_Toc_temp_var(); goto
_LL1204; _LL1204: { struct Cyc_Absyn_Exp* _temp1205= Cyc_Absyn_var_exp(
_temp1203, 0); goto _LL1206; _LL1206: for( 0; _temp1199 != 0; _temp1199=((
struct Cyc_List_List*) _check_null( _temp1199))->tl){ void* _temp1207=( void*)((
struct Cyc_List_List*) _check_null( _temp1199))->hd; struct _tagged_string*
_temp1213; _LL1209: if(*(( int*) _temp1207) == Cyc_Absyn_FieldName){ _LL1214:
_temp1213=(( struct Cyc_Absyn_FieldName_struct*) _temp1207)->f1; goto _LL1210;}
else{ goto _LL1211;} _LL1211: goto _LL1212; _LL1210: if( Cyc_Toc_is_poly_field(
struct_type, _temp1213)){ _temp1205= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1205, 0);} _temp1187= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1189( _temp1185, _temp1213, 0), _temp1205, 0), 0), _temp1187, 0); goto
_LL1208; _LL1212:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1215=( unsigned char*)"array designator in struct"; struct
_tagged_string _temp1216; _temp1216.curr= _temp1215; _temp1216.base= _temp1215;
_temp1216.last_plus_one= _temp1215 + 27; _temp1216;}))); _LL1208:;} Cyc_Toc_exp_to_c(
nv, _temp1197); _temp1187= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1205, _temp1197, 0), 0), _temp1187, 0);}} else{ void* _temp1217=( void*)((
struct Cyc_List_List*) _check_null( _temp1199))->hd; struct _tagged_string*
_temp1223; _LL1219: if(*(( int*) _temp1217) == Cyc_Absyn_FieldName){ _LL1224:
_temp1223=(( struct Cyc_Absyn_FieldName_struct*) _temp1217)->f1; goto _LL1220;}
else{ goto _LL1221;} _LL1221: goto _LL1222; _LL1220: { struct Cyc_Absyn_Exp*
lval= _temp1189( _temp1185, _temp1223, 0);{ void* _temp1225=( void*) _temp1197->r;
struct Cyc_List_List* _temp1235; struct Cyc_Absyn_Exp* _temp1237; struct Cyc_Absyn_Exp*
_temp1239; struct Cyc_Absyn_Vardecl* _temp1241; struct Cyc_List_List* _temp1243;
void* _temp1245; _LL1227: if(*(( int*) _temp1225) == Cyc_Absyn_Array_e){ _LL1236:
_temp1235=(( struct Cyc_Absyn_Array_e_struct*) _temp1225)->f1; goto _LL1228;}
else{ goto _LL1229;} _LL1229: if(*(( int*) _temp1225) == Cyc_Absyn_Comprehension_e){
_LL1242: _temp1241=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1225)->f1;
goto _LL1240; _LL1240: _temp1239=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1225)->f2; goto _LL1238; _LL1238: _temp1237=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1225)->f3; goto _LL1230;} else{ goto _LL1231;} _LL1231: if(*(( int*)
_temp1225) == Cyc_Absyn_AnonStruct_e){ _LL1246: _temp1245=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1225)->f1; goto _LL1244; _LL1244: _temp1243=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1225)->f2; goto _LL1232;} else{ goto _LL1233;} _LL1233: goto _LL1234;
_LL1228: _temp1187= Cyc_Toc_init_array( nv, lval, _temp1235, _temp1187); goto
_LL1226; _LL1230: _temp1187= Cyc_Toc_init_comprehension( nv, lval, _temp1241,
_temp1239, _temp1237, _temp1187, 0); goto _LL1226; _LL1232: _temp1187= Cyc_Toc_init_anon_struct(
nv, lval, _temp1245, _temp1243, _temp1187); goto _LL1226; _LL1234: Cyc_Toc_exp_to_c(
nv, _temp1197); if( Cyc_Toc_is_poly_field( struct_type, _temp1223)){ _temp1197=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1197, 0);} _temp1187= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1197, 0), 0), _temp1187, 0);
goto _LL1226; _LL1226:;} goto _LL1218;} _LL1222:( void) _throw((( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1247=(
unsigned char*)"array designator in struct"; struct _tagged_string _temp1248;
_temp1248.curr= _temp1247; _temp1248.base= _temp1247; _temp1248.last_plus_one=
_temp1247 + 27; _temp1248;}))); _LL1218:;}}}{ struct Cyc_Absyn_Exp* _temp1249=
Cyc_Toc_make_struct( nv, _temp1183, Cyc_Absyn_strctq( tdn), _temp1187, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp1249;}; _pop_region(&
_temp1191);}}}}} struct _tuple11{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp*
f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp*
el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2,
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp*
e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp1250=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp1250[ 0]=({
struct Cyc_Absyn_Increment_e_struct _temp1251; _temp1251.tag= Cyc_Absyn_Increment_e;
_temp1251.f1= e1; _temp1251.f2=( void*) incr; _temp1251;}); _temp1250;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp1252=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp1262; struct Cyc_Absyn_Exp*
_temp1264; void* _temp1266; struct _tagged_string* _temp1268; struct Cyc_Absyn_Exp*
_temp1270; _LL1254: if(*(( int*) _temp1252) == Cyc_Absyn_StmtExp_e){ _LL1263:
_temp1262=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1252)->f1; goto _LL1255;}
else{ goto _LL1256;} _LL1256: if(*(( int*) _temp1252) == Cyc_Absyn_Cast_e){
_LL1267: _temp1266=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1252)->f1;
goto _LL1265; _LL1265: _temp1264=(( struct Cyc_Absyn_Cast_e_struct*) _temp1252)->f2;
goto _LL1257;} else{ goto _LL1258;} _LL1258: if(*(( int*) _temp1252) == Cyc_Absyn_StructMember_e){
_LL1271: _temp1270=(( struct Cyc_Absyn_StructMember_e_struct*) _temp1252)->f1;
goto _LL1269; _LL1269: _temp1268=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1252)->f2; goto _LL1259;} else{ goto _LL1260;} _LL1260: goto _LL1261;
_LL1255:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1262, fs, f, f_env); goto _LL1253; _LL1257:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1264, fs, f, f_env); goto
_LL1253; _LL1259:( void*)( e1->r=( void*)(( void*) _temp1270->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1272=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1272->hd=( void*) _temp1268; _temp1272->tl= fs;
_temp1272;}), f, f_env); goto _LL1253; _LL1261: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
struct _tagged_string _temp1273= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1273.last_plus_one - _temp1273.curr, _temp1273.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1253;}
_LL1253:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1274=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1284; struct Cyc_Absyn_Stmt* _temp1286; struct Cyc_Absyn_Decl* _temp1288;
struct Cyc_Absyn_Stmt* _temp1290; struct Cyc_Absyn_Stmt* _temp1292; _LL1276: if((
unsigned int) _temp1274 > 1u?*(( int*) _temp1274) == Cyc_Absyn_Exp_s: 0){
_LL1285: _temp1284=(( struct Cyc_Absyn_Exp_s_struct*) _temp1274)->f1; goto
_LL1277;} else{ goto _LL1278;} _LL1278: if(( unsigned int) _temp1274 > 1u?*((
int*) _temp1274) == Cyc_Absyn_Decl_s: 0){ _LL1289: _temp1288=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1274)->f1; goto _LL1287; _LL1287: _temp1286=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1274)->f2; goto _LL1279;} else{ goto _LL1280;} _LL1280: if(( unsigned int)
_temp1274 > 1u?*(( int*) _temp1274) == Cyc_Absyn_Seq_s: 0){ _LL1293: _temp1292=((
struct Cyc_Absyn_Seq_s_struct*) _temp1274)->f1; goto _LL1291; _LL1291: _temp1290=((
struct Cyc_Absyn_Seq_s_struct*) _temp1274)->f2; goto _LL1281;} else{ goto
_LL1282;} _LL1282: goto _LL1283; _LL1277:(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1284, fs, f, f_env); goto
_LL1275; _LL1279:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1286, fs, f, f_env); goto _LL1275; _LL1281:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1290, fs, f, f_env);
goto _LL1275; _LL1283:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp1294= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1294.last_plus_one - _temp1294.curr,
_temp1294.curr);})); goto _LL1275; _LL1275:;} static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s); static struct Cyc_List_List*
Cyc_Toc_rmap_2c( struct _RegionHandle* r2, void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List* _temp1295=( struct
Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1295->hd=(
void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1295->tl=
0; _temp1295;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp1296=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1296->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1296->tl= 0;
_temp1296;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x))
Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1297=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1297->f1= 0; _temp1297->f2= e;
_temp1297;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){
void* _temp1298= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp1304;
_LL1300: if(( unsigned int) _temp1298 > 4u?*(( int*) _temp1298) == Cyc_Absyn_PointerType:
0){ _LL1305: _temp1304=(( struct Cyc_Absyn_PointerType_struct*) _temp1298)->f1;
goto _LL1301;} else{ goto _LL1302;} _LL1302: goto _LL1303; _LL1301: return
_temp1304; _LL1303:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp1306=( unsigned char*)"get_ptr_typ:  not a pointer!"; struct
_tagged_string _temp1307; _temp1307.curr= _temp1306; _temp1307.base= _temp1306;
_temp1307.last_plus_one= _temp1306 + 29; _temp1307;}))); _LL1299:;} struct
_tagged_ptr6{ struct _tuple0** curr; struct _tuple0** base; struct _tuple0**
last_plus_one; } ; struct _tagged_ptr7{ struct _tuple1** curr; struct _tuple1**
base; struct _tuple1** last_plus_one; } ; struct _tagged_ptr8{ struct Cyc_Absyn_Exp**
curr; struct Cyc_Absyn_Exp** base; struct Cyc_Absyn_Exp** last_plus_one; } ;
struct _tuple12{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){
void* _temp1308=( void*) e->r; goto _LL1309; _LL1309: if( e->topt == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1310= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1310.last_plus_one - _temp1310.curr, _temp1310.curr);})); return;}{ void*
old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void*
_temp1311= _temp1308; void* _temp1389; void* _temp1391; void* _temp1393; struct
_tuple0* _temp1395; struct _tuple0* _temp1397; struct Cyc_List_List* _temp1399;
void* _temp1401; void* _temp1403; struct Cyc_Absyn_Exp* _temp1405; struct Cyc_Absyn_Exp*
_temp1407; struct Cyc_Core_Opt* _temp1409; struct Cyc_Absyn_Exp* _temp1411;
struct Cyc_Absyn_Exp* _temp1413; struct Cyc_Absyn_Exp* _temp1415; struct Cyc_Absyn_Exp*
_temp1417; struct Cyc_Absyn_Exp* _temp1419; struct Cyc_Absyn_Exp* _temp1421;
struct Cyc_List_List* _temp1423; struct Cyc_Absyn_Exp* _temp1425; struct Cyc_Absyn_VarargCallInfo*
_temp1427; struct Cyc_List_List* _temp1429; struct Cyc_Absyn_Exp* _temp1431;
struct Cyc_Absyn_VarargCallInfo* _temp1433; struct Cyc_Absyn_VarargCallInfo
_temp1435; struct Cyc_Absyn_VarargInfo* _temp1436; struct Cyc_List_List*
_temp1438; int _temp1440; struct Cyc_List_List* _temp1442; struct Cyc_Absyn_Exp*
_temp1444; struct Cyc_Absyn_Exp* _temp1446; struct Cyc_Absyn_Exp* _temp1448;
struct Cyc_List_List* _temp1450; struct Cyc_Absyn_Exp* _temp1452; struct Cyc_Absyn_Exp*
_temp1454; void* _temp1456; void** _temp1458; struct Cyc_Absyn_Exp* _temp1459;
struct Cyc_Absyn_Exp* _temp1461; struct Cyc_Absyn_Exp* _temp1463; void*
_temp1465; struct Cyc_Absyn_Exp* _temp1467; struct Cyc_Absyn_Exp* _temp1469;
struct _tagged_string* _temp1471; struct Cyc_Absyn_Exp* _temp1473; struct
_tagged_string* _temp1475; struct Cyc_Absyn_Exp* _temp1477; struct Cyc_Absyn_Exp*
_temp1479; struct Cyc_Absyn_Exp* _temp1481; struct Cyc_List_List* _temp1483;
struct Cyc_List_List* _temp1485; struct _tuple1* _temp1487; struct Cyc_List_List*
_temp1489; struct Cyc_Absyn_Exp* _temp1491; struct Cyc_Absyn_Exp* _temp1493;
struct Cyc_Absyn_Vardecl* _temp1495; struct Cyc_Absyn_Structdecl* _temp1497;
struct Cyc_List_List* _temp1499; struct Cyc_Core_Opt* _temp1501; struct _tuple0*
_temp1503; struct Cyc_List_List* _temp1505; void* _temp1507; struct Cyc_Absyn_Tunionfield*
_temp1509; struct Cyc_Absyn_Tuniondecl* _temp1511; struct Cyc_List_List*
_temp1513; struct Cyc_Core_Opt* _temp1515; struct Cyc_Core_Opt* _temp1517;
struct Cyc_Absyn_Tunionfield* _temp1519; struct Cyc_Absyn_Tuniondecl* _temp1521;
struct Cyc_List_List* _temp1523; struct Cyc_Core_Opt* _temp1525; struct Cyc_Core_Opt*
_temp1527; struct Cyc_Absyn_Enumfield* _temp1529; struct Cyc_Absyn_Enumdecl*
_temp1531; struct _tuple0* _temp1533; void* _temp1535; struct Cyc_Absyn_Exp*
_temp1537; struct Cyc_Absyn_Stmt* _temp1539; struct Cyc_List_List* _temp1541;
struct Cyc_Core_Opt* _temp1543; struct Cyc_Absyn_Fndecl* _temp1545; struct Cyc_Absyn_Exp*
_temp1547; _LL1313: if(*(( int*) _temp1311) == Cyc_Absyn_Const_e){ _LL1390:
_temp1389=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1311)->f1; if(
_temp1389 ==( void*) Cyc_Absyn_Null_c){ goto _LL1314;} else{ goto _LL1315;}}
else{ goto _LL1315;} _LL1315: if(*(( int*) _temp1311) == Cyc_Absyn_Const_e){
_LL1392: _temp1391=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1311)->f1;
goto _LL1316;} else{ goto _LL1317;} _LL1317: if(*(( int*) _temp1311) == Cyc_Absyn_Var_e){
_LL1396: _temp1395=(( struct Cyc_Absyn_Var_e_struct*) _temp1311)->f1; goto
_LL1394; _LL1394: _temp1393=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1311)->f2;
goto _LL1318;} else{ goto _LL1319;} _LL1319: if(*(( int*) _temp1311) == Cyc_Absyn_UnknownId_e){
_LL1398: _temp1397=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1311)->f1; goto
_LL1320;} else{ goto _LL1321;} _LL1321: if(*(( int*) _temp1311) == Cyc_Absyn_Primop_e){
_LL1402: _temp1401=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1311)->f1;
goto _LL1400; _LL1400: _temp1399=(( struct Cyc_Absyn_Primop_e_struct*) _temp1311)->f2;
goto _LL1322;} else{ goto _LL1323;} _LL1323: if(*(( int*) _temp1311) == Cyc_Absyn_Increment_e){
_LL1406: _temp1405=(( struct Cyc_Absyn_Increment_e_struct*) _temp1311)->f1; goto
_LL1404; _LL1404: _temp1403=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1311)->f2; goto _LL1324;} else{ goto _LL1325;} _LL1325: if(*(( int*)
_temp1311) == Cyc_Absyn_AssignOp_e){ _LL1412: _temp1411=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1311)->f1; goto _LL1410; _LL1410: _temp1409=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1311)->f2; goto _LL1408; _LL1408: _temp1407=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1311)->f3; goto _LL1326;} else{ goto _LL1327;} _LL1327: if(*(( int*)
_temp1311) == Cyc_Absyn_Conditional_e){ _LL1418: _temp1417=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1311)->f1; goto _LL1416; _LL1416: _temp1415=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1311)->f2; goto _LL1414; _LL1414: _temp1413=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1311)->f3; goto _LL1328;} else{ goto _LL1329;} _LL1329: if(*(( int*)
_temp1311) == Cyc_Absyn_SeqExp_e){ _LL1422: _temp1421=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1311)->f1; goto _LL1420; _LL1420: _temp1419=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1311)->f2; goto _LL1330;} else{ goto _LL1331;} _LL1331: if(*(( int*)
_temp1311) == Cyc_Absyn_UnknownCall_e){ _LL1426: _temp1425=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1311)->f1; goto _LL1424; _LL1424: _temp1423=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1311)->f2; goto _LL1332;} else{ goto _LL1333;} _LL1333: if(*(( int*)
_temp1311) == Cyc_Absyn_FnCall_e){ _LL1432: _temp1431=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1311)->f1; goto _LL1430; _LL1430: _temp1429=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1311)->f2; goto _LL1428; _LL1428: _temp1427=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1311)->f3; if( _temp1427 == 0){ goto _LL1334;} else{ goto _LL1335;}} else{
goto _LL1335;} _LL1335: if(*(( int*) _temp1311) == Cyc_Absyn_FnCall_e){ _LL1445:
_temp1444=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1311)->f1; goto _LL1443;
_LL1443: _temp1442=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1311)->f2; goto
_LL1434; _LL1434: _temp1433=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1311)->f3;
if( _temp1433 == 0){ goto _LL1337;} else{ _temp1435=* _temp1433; _LL1441:
_temp1440= _temp1435.num_varargs; goto _LL1439; _LL1439: _temp1438= _temp1435.injectors;
goto _LL1437; _LL1437: _temp1436= _temp1435.vai; goto _LL1336;}} else{ goto
_LL1337;} _LL1337: if(*(( int*) _temp1311) == Cyc_Absyn_Throw_e){ _LL1447:
_temp1446=(( struct Cyc_Absyn_Throw_e_struct*) _temp1311)->f1; goto _LL1338;}
else{ goto _LL1339;} _LL1339: if(*(( int*) _temp1311) == Cyc_Absyn_NoInstantiate_e){
_LL1449: _temp1448=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1311)->f1;
goto _LL1340;} else{ goto _LL1341;} _LL1341: if(*(( int*) _temp1311) == Cyc_Absyn_Instantiate_e){
_LL1453: _temp1452=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1311)->f1;
goto _LL1451; _LL1451: _temp1450=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1311)->f2; goto _LL1342;} else{ goto _LL1343;} _LL1343: if(*(( int*)
_temp1311) == Cyc_Absyn_Cast_e){ _LL1457: _temp1456=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1311)->f1; _temp1458=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1311)->f1); goto _LL1455; _LL1455: _temp1454=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1311)->f2; goto _LL1344;} else{ goto _LL1345;} _LL1345: if(*(( int*)
_temp1311) == Cyc_Absyn_Address_e){ _LL1460: _temp1459=(( struct Cyc_Absyn_Address_e_struct*)
_temp1311)->f1; goto _LL1346;} else{ goto _LL1347;} _LL1347: if(*(( int*)
_temp1311) == Cyc_Absyn_New_e){ _LL1464: _temp1463=(( struct Cyc_Absyn_New_e_struct*)
_temp1311)->f1; goto _LL1462; _LL1462: _temp1461=(( struct Cyc_Absyn_New_e_struct*)
_temp1311)->f2; goto _LL1348;} else{ goto _LL1349;} _LL1349: if(*(( int*)
_temp1311) == Cyc_Absyn_Sizeoftyp_e){ _LL1466: _temp1465=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1311)->f1; goto _LL1350;} else{ goto _LL1351;} _LL1351: if(*(( int*)
_temp1311) == Cyc_Absyn_Sizeofexp_e){ _LL1468: _temp1467=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1311)->f1; goto _LL1352;} else{ goto _LL1353;} _LL1353: if(*(( int*)
_temp1311) == Cyc_Absyn_Deref_e){ _LL1470: _temp1469=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1311)->f1; goto _LL1354;} else{ goto _LL1355;} _LL1355: if(*(( int*)
_temp1311) == Cyc_Absyn_StructMember_e){ _LL1474: _temp1473=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1311)->f1; goto _LL1472; _LL1472: _temp1471=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1311)->f2; goto _LL1356;} else{ goto _LL1357;} _LL1357: if(*(( int*)
_temp1311) == Cyc_Absyn_StructArrow_e){ _LL1478: _temp1477=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1311)->f1; goto _LL1476; _LL1476: _temp1475=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1311)->f2; goto _LL1358;} else{ goto _LL1359;} _LL1359: if(*(( int*)
_temp1311) == Cyc_Absyn_Subscript_e){ _LL1482: _temp1481=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1311)->f1; goto _LL1480; _LL1480: _temp1479=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1311)->f2; goto _LL1360;} else{ goto _LL1361;} _LL1361: if(*(( int*)
_temp1311) == Cyc_Absyn_Tuple_e){ _LL1484: _temp1483=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1311)->f1; goto _LL1362;} else{ goto _LL1363;} _LL1363: if(*(( int*)
_temp1311) == Cyc_Absyn_CompoundLit_e){ _LL1488: _temp1487=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1311)->f1; goto _LL1486; _LL1486: _temp1485=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1311)->f2; goto _LL1364;} else{ goto _LL1365;} _LL1365: if(*(( int*)
_temp1311) == Cyc_Absyn_Array_e){ _LL1490: _temp1489=(( struct Cyc_Absyn_Array_e_struct*)
_temp1311)->f1; goto _LL1366;} else{ goto _LL1367;} _LL1367: if(*(( int*)
_temp1311) == Cyc_Absyn_Comprehension_e){ _LL1496: _temp1495=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1311)->f1; goto _LL1494; _LL1494: _temp1493=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1311)->f2; goto _LL1492; _LL1492: _temp1491=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1311)->f3; goto _LL1368;} else{ goto _LL1369;} _LL1369: if(*(( int*)
_temp1311) == Cyc_Absyn_Struct_e){ _LL1504: _temp1503=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1311)->f1; goto _LL1502; _LL1502: _temp1501=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1311)->f2; goto _LL1500; _LL1500: _temp1499=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1311)->f3; goto _LL1498; _LL1498: _temp1497=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1311)->f4; goto _LL1370;} else{ goto _LL1371;} _LL1371: if(*(( int*)
_temp1311) == Cyc_Absyn_AnonStruct_e){ _LL1508: _temp1507=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1311)->f1; goto _LL1506; _LL1506: _temp1505=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1311)->f2; goto _LL1372;} else{ goto _LL1373;} _LL1373: if(*(( int*)
_temp1311) == Cyc_Absyn_Tunion_e){ _LL1518: _temp1517=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1311)->f1; goto _LL1516; _LL1516: _temp1515=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1311)->f2; goto _LL1514; _LL1514: _temp1513=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1311)->f3; if( _temp1513 == 0){ goto _LL1512;} else{ goto _LL1375;} _LL1512:
_temp1511=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f4; goto _LL1510;
_LL1510: _temp1509=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f5; goto
_LL1374;} else{ goto _LL1375;} _LL1375: if(*(( int*) _temp1311) == Cyc_Absyn_Tunion_e){
_LL1528: _temp1527=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f1; goto
_LL1526; _LL1526: _temp1525=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f2;
goto _LL1524; _LL1524: _temp1523=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f3;
goto _LL1522; _LL1522: _temp1521=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f4;
goto _LL1520; _LL1520: _temp1519=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1311)->f5;
goto _LL1376;} else{ goto _LL1377;} _LL1377: if(*(( int*) _temp1311) == Cyc_Absyn_Enum_e){
_LL1534: _temp1533=(( struct Cyc_Absyn_Enum_e_struct*) _temp1311)->f1; goto
_LL1532; _LL1532: _temp1531=(( struct Cyc_Absyn_Enum_e_struct*) _temp1311)->f2;
goto _LL1530; _LL1530: _temp1529=(( struct Cyc_Absyn_Enum_e_struct*) _temp1311)->f3;
goto _LL1378;} else{ goto _LL1379;} _LL1379: if(*(( int*) _temp1311) == Cyc_Absyn_Malloc_e){
_LL1538: _temp1537=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1311)->f1; goto
_LL1536; _LL1536: _temp1535=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1311)->f2; goto _LL1380;} else{ goto _LL1381;} _LL1381: if(*(( int*)
_temp1311) == Cyc_Absyn_StmtExp_e){ _LL1540: _temp1539=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1311)->f1; goto _LL1382;} else{ goto _LL1383;} _LL1383: if(*(( int*)
_temp1311) == Cyc_Absyn_UnresolvedMem_e){ _LL1544: _temp1543=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1311)->f1; goto _LL1542; _LL1542: _temp1541=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1311)->f2; goto _LL1384;} else{ goto _LL1385;} _LL1385: if(*(( int*)
_temp1311) == Cyc_Absyn_Codegen_e){ _LL1546: _temp1545=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1311)->f1; goto _LL1386;} else{ goto _LL1387;} _LL1387: if(*(( int*)
_temp1311) == Cyc_Absyn_Fill_e){ _LL1548: _temp1547=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1311)->f1; goto _LL1388;} else{ goto _LL1312;} _LL1314:{ void* _temp1549=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1555; struct Cyc_Absyn_Conref*
_temp1557; struct Cyc_Absyn_Tqual _temp1559; struct Cyc_Absyn_Conref* _temp1561;
void* _temp1563; void* _temp1565; _LL1551: if(( unsigned int) _temp1549 > 4u?*((
int*) _temp1549) == Cyc_Absyn_PointerType: 0){ _LL1556: _temp1555=(( struct Cyc_Absyn_PointerType_struct*)
_temp1549)->f1; _LL1566: _temp1565=( void*) _temp1555.elt_typ; goto _LL1564;
_LL1564: _temp1563=( void*) _temp1555.rgn_typ; goto _LL1562; _LL1562: _temp1561=
_temp1555.nullable; goto _LL1560; _LL1560: _temp1559= _temp1555.tq; goto _LL1558;
_LL1558: _temp1557= _temp1555.bounds; goto _LL1552;} else{ goto _LL1553;}
_LL1553: goto _LL1554; _LL1552: if(( void*) _temp1557->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1550;}{ void* _temp1567=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1557); struct Cyc_Absyn_Exp* _temp1573; _LL1569: if( _temp1567 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1570;} else{ goto _LL1571;} _LL1571: if((
unsigned int) _temp1567 > 1u?*(( int*) _temp1567) == Cyc_Absyn_Upper_b: 0){
_LL1574: _temp1573=(( struct Cyc_Absyn_Upper_b_struct*) _temp1567)->f1; goto
_LL1572;} else{ goto _LL1568;} _LL1570: { struct _tuple4* _temp1576=({ struct
_tuple4* _temp1575=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1575->f1= 0; _temp1575->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1575;});
goto _LL1577; _LL1577: { struct Cyc_Absyn_Exp* _temp1584= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List*(* _temp1578)( struct _tagged_ptr4)=( struct Cyc_List_List*(*)(
struct _tagged_ptr4)) Cyc_List_list; struct _tuple4* _temp1581= _temp1576;
struct _tuple4* _temp1582= _temp1576; struct _tuple4* _temp1583= _temp1576;
struct _tuple4* _temp1579[ 3u]={ _temp1581, _temp1582, _temp1583}; struct
_tagged_ptr4 _temp1580={ _temp1579, _temp1579, _temp1579 + 3u}; _temp1578(
_temp1580);}), 0); goto _LL1585; _LL1585: if( nv->toplevel){( void*)( e->r=(
void*)(( void*) _temp1584->r));} else{ void* _temp1586= Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1565), Cyc_Toc_mt_tq); goto _LL1587; _LL1587: { struct
_tuple0* _temp1588= Cyc_Toc_temp_var(); goto _LL1589; _LL1589: { struct Cyc_Absyn_Exp*
_temp1590= Cyc_Absyn_var_exp( _temp1588, 0); goto _LL1591; _LL1591: { struct Cyc_Absyn_Stmt*
_temp1592= Cyc_Absyn_declare_stmt( _temp1588, _temp1586,( struct Cyc_Absyn_Exp*)
_temp1584, Cyc_Absyn_exp_stmt( _temp1590, 0), 0); goto _LL1593; _LL1593:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1592, 0))->r));}}}} goto
_LL1568;}} _LL1572:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0,
0))->r)); goto _LL1568; _LL1568:;} goto _LL1550; _LL1554:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1594= Cyc_Absynpp_typ2string( old_typ); xprintf("exp_to_c: null given type %.*s",
_temp1594.last_plus_one - _temp1594.curr, _temp1594.curr);})); goto _LL1550;
_LL1550:;} goto _LL1312; _LL1316: goto _LL1312; _LL1318:{ struct _handler_cons
_temp1595; _push_handler(& _temp1595);{ int _temp1597= 0; if( setjmp( _temp1595.handler)){
_temp1597= 1;} if( ! _temp1597){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1395))->r));; _pop_handler();} else{ void* _temp1596=( void*) _exn_thrown;
void* _temp1599= _temp1596; _LL1601: if( _temp1599 == Cyc_Dict_Absent){ goto
_LL1602;} else{ goto _LL1603;} _LL1603: goto _LL1604; _LL1602:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1605= Cyc_Absynpp_qvar2string( _temp1395); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1605.last_plus_one - _temp1605.curr, _temp1605.curr);})); return; _LL1604:(
void) _throw( _temp1599); _LL1600:;}}} goto _LL1312; _LL1320:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1606=(
unsigned char*)"unknownid"; struct _tagged_string _temp1607; _temp1607.curr=
_temp1606; _temp1607.base= _temp1606; _temp1607.last_plus_one= _temp1606 + 10;
_temp1607;})); goto _LL1312; _LL1322: { struct Cyc_List_List* _temp1608=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp1399); goto _LL1609; _LL1609: {
struct Cyc_List_List* _temp1610=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1399); goto _LL1611; _LL1611:(( void(*)( void(* f)( struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)(
Cyc_Toc_exp_to_c, nv, _temp1399);{ void* _temp1612= _temp1401; _LL1614: if(
_temp1612 ==( void*) Cyc_Absyn_Size){ goto _LL1615;} else{ goto _LL1616;}
_LL1616: if( _temp1612 ==( void*) Cyc_Absyn_Printf){ goto _LL1617;} else{ goto
_LL1618;} _LL1618: if( _temp1612 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1619;}
else{ goto _LL1620;} _LL1620: if( _temp1612 ==( void*) Cyc_Absyn_Fprintf){ goto
_LL1621;} else{ goto _LL1622;} _LL1622: if( _temp1612 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1623;} else{ goto _LL1624;} _LL1624: if( _temp1612 ==( void*) Cyc_Absyn_Plus){
goto _LL1625;} else{ goto _LL1626;} _LL1626: if( _temp1612 ==( void*) Cyc_Absyn_Minus){
goto _LL1627;} else{ goto _LL1628;} _LL1628: if( _temp1612 ==( void*) Cyc_Absyn_Eq){
goto _LL1629;} else{ goto _LL1630;} _LL1630: if( _temp1612 ==( void*) Cyc_Absyn_Neq){
goto _LL1631;} else{ goto _LL1632;} _LL1632: if( _temp1612 ==( void*) Cyc_Absyn_Gt){
goto _LL1633;} else{ goto _LL1634;} _LL1634: if( _temp1612 ==( void*) Cyc_Absyn_Gte){
goto _LL1635;} else{ goto _LL1636;} _LL1636: if( _temp1612 ==( void*) Cyc_Absyn_Lt){
goto _LL1637;} else{ goto _LL1638;} _LL1638: if( _temp1612 ==( void*) Cyc_Absyn_Lte){
goto _LL1639;} else{ goto _LL1640;} _LL1640: goto _LL1641; _LL1615: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1399))->hd;{
void* _temp1642=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1650; struct Cyc_Absyn_Tqual _temp1652; void*
_temp1654; struct Cyc_Absyn_PtrInfo _temp1656; struct Cyc_Absyn_Conref*
_temp1658; struct Cyc_Absyn_Tqual _temp1660; struct Cyc_Absyn_Conref* _temp1662;
void* _temp1664; void* _temp1666; _LL1644: if(( unsigned int) _temp1642 > 4u?*((
int*) _temp1642) == Cyc_Absyn_ArrayType: 0){ _LL1655: _temp1654=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1642)->f1; goto _LL1653; _LL1653:
_temp1652=(( struct Cyc_Absyn_ArrayType_struct*) _temp1642)->f2; goto _LL1651;
_LL1651: _temp1650=(( struct Cyc_Absyn_ArrayType_struct*) _temp1642)->f3; goto
_LL1645;} else{ goto _LL1646;} _LL1646: if(( unsigned int) _temp1642 > 4u?*((
int*) _temp1642) == Cyc_Absyn_PointerType: 0){ _LL1657: _temp1656=(( struct Cyc_Absyn_PointerType_struct*)
_temp1642)->f1; _LL1667: _temp1666=( void*) _temp1656.elt_typ; goto _LL1665;
_LL1665: _temp1664=( void*) _temp1656.rgn_typ; goto _LL1663; _LL1663: _temp1662=
_temp1656.nullable; goto _LL1661; _LL1661: _temp1660= _temp1656.tq; goto _LL1659;
_LL1659: _temp1658= _temp1656.bounds; goto _LL1647;} else{ goto _LL1648;}
_LL1648: goto _LL1649; _LL1645:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1650))->r)); goto _LL1643; _LL1647: { struct _tuple0*
_temp1668= Cyc_Toc_temp_var(); goto _LL1669; _LL1669: { void* _temp1670= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); goto _LL1671;
_LL1671: { struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1668, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1668, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4=
Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp( e2, e3, 0), 0);
struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1668, _temp1670,(
struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1643;}}} _LL1649:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
struct _tagged_string _temp1672= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp1610))->hd); struct _tagged_string _temp1673= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1672.last_plus_one - _temp1672.curr, _temp1672.curr, _temp1673.last_plus_one
- _temp1673.curr, _temp1673.curr);})); return; _LL1643:;} goto _LL1613;} _LL1617:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1401, 0, _temp1399,
_temp1610))->r)); goto _LL1613; _LL1619:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1401, 0, _temp1399, _temp1610))->r)); goto _LL1613; _LL1621: if( _temp1399
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char*
_temp1674=( unsigned char*)"fprintf without arguments"; struct _tagged_string
_temp1675; _temp1675.curr= _temp1674; _temp1675.base= _temp1674; _temp1675.last_plus_one=
_temp1674 + 26; _temp1675;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1401,({ struct Cyc_Core_Opt* _temp1676=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1676->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1399))->hd); _temp1676;}),(( struct Cyc_List_List*)
_check_null( _temp1399))->tl,(( struct Cyc_List_List*) _check_null( _temp1610))->tl))->r));
goto _LL1613; _LL1623: if( _temp1399 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1677=( unsigned char*)"sscanf without arguments";
struct _tagged_string _temp1678; _temp1678.curr= _temp1677; _temp1678.base=
_temp1677; _temp1678.last_plus_one= _temp1677 + 25; _temp1678;}));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1399))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1399))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1399))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1613; _LL1625: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1399))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1399))->tl))->hd; struct _tuple0* _temp1679= Cyc_Toc_temp_var(); goto
_LL1680; _LL1680: { void* _temp1681= Cyc_Toc_typ_to_c( old_typ); goto _LL1682;
_LL1682: { struct Cyc_Absyn_Stmt* _temp1683= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1679, 0), 0); goto _LL1684; _LL1684: { struct Cyc_Absyn_Exp* _temp1685= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1679, 0), Cyc_Toc_curr_sp, 0); goto _LL1686; _LL1686: {
struct Cyc_Absyn_Exp* _temp1688= Cyc_Absyn_assignop_exp( _temp1685,({ struct Cyc_Core_Opt*
_temp1687=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1687->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1687;}), e2, 0); goto
_LL1689; _LL1689: _temp1683= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1688,
0), _temp1683, 0); _temp1683= Cyc_Absyn_declare_stmt( _temp1679, _temp1681,(
struct Cyc_Absyn_Exp*) e1, _temp1683, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1683, 0))->r));}}}}} goto _LL1613; _LL1627: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null( _temp1608))->hd)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1399))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1399))->tl))->hd; void*
old_typ_e2=( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1608))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* _temp1690= Cyc_Toc_temp_var(); goto _LL1691;
_LL1691: { void* _temp1692= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_List_List*)
_check_null( _temp1608))->hd); goto _LL1693; _LL1693: { struct Cyc_Absyn_Stmt*
_temp1694= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1690, 0), 0); goto
_LL1695; _LL1695: { struct Cyc_Absyn_Exp* _temp1696= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1690, 0), Cyc_Toc_curr_sp, 0); goto _LL1697; _LL1697: {
struct Cyc_Absyn_Exp* _temp1699= Cyc_Absyn_assignop_exp( _temp1696,({ struct Cyc_Core_Opt*
_temp1698=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1698->v=( void*)(( void*) Cyc_Absyn_Minus); _temp1698;}), e2, 0); goto
_LL1700; _LL1700: _temp1694= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1699,
0), _temp1694, 0); _temp1694= Cyc_Absyn_declare_stmt( _temp1690, _temp1692,(
struct Cyc_Absyn_Exp*) e1, _temp1694, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1694, 0))->r));}}}}}} goto _LL1613; _LL1629: goto _LL1631; _LL1631: goto
_LL1633; _LL1633: goto _LL1635; _LL1635: goto _LL1637; _LL1637: goto _LL1639;
_LL1639: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1399))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1399))->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp1608))->hd; void* t2=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1608))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1613;} _LL1641: goto
_LL1613; _LL1613:;} goto _LL1312;}} _LL1324: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1405->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1405); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1403 ==(
void*) Cyc_Absyn_PreInc? 1: _temp1403 ==( void*) Cyc_Absyn_PreDec){ struct
_tuple0* _temp1701= Cyc_Toc_temp_var(); goto _LL1702; _LL1702: { void* _temp1703=
Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1704; _LL1704: { struct Cyc_Absyn_Stmt*
_temp1705= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1701,
0), 0), 0); goto _LL1706; _LL1706: { struct Cyc_Absyn_Exp* _temp1707= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1701, 0), Cyc_Toc_curr_sp, 0),
_temp1403, 0); goto _LL1708; _LL1708: _temp1705= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1707, 0), _temp1705, 0); _temp1705= Cyc_Absyn_declare_stmt( _temp1701,
_temp1703,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1405, 0),
_temp1705, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1705, 0))->r));}}}}
else{ struct _tuple0* _temp1709= Cyc_Toc_temp_var(); goto _LL1710; _LL1710: {
struct _tuple0* _temp1711= Cyc_Toc_temp_var(); goto _LL1712; _LL1712: { void*
_temp1713= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1714; _LL1714: { void*
_temp1715= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1716; _LL1716: { struct Cyc_Absyn_Stmt*
_temp1717= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1711, 0), 0); goto
_LL1718; _LL1718: { struct Cyc_Absyn_Exp* _temp1719= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1709, 0), Cyc_Toc_curr_sp, 0),
_temp1403, 0); goto _LL1720; _LL1720: _temp1717= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1719, 0), _temp1717, 0); _temp1717= Cyc_Absyn_declare_stmt( _temp1711,
_temp1713,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1709, 0), 0), _temp1717, 0); _temp1717= Cyc_Absyn_declare_stmt( _temp1709,
_temp1715,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1405, 0),
_temp1717, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1717, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1405)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1405, 0, Cyc_Toc_incr_lvalue,
_temp1403);( void*)( e->r=( void*)(( void*) _temp1405->r));}} goto _LL1312;}
_LL1326: { int e1_poly= Cyc_Toc_is_poly_project( _temp1411); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1411->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1407->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1411); Cyc_Toc_exp_to_c( nv, _temp1407); if(
_temp1409 != 0?( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1409))->v ==(
void*) Cyc_Absyn_Plus? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1409))->v
==( void*) Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){(
void*)( _temp1411->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1411->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1407->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1407->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1407->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1407->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1411)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1411, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1721=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1721[ 0]=({ struct _tuple11 _temp1722; _temp1722.f1=
_temp1409; _temp1722.f2= _temp1407; _temp1722;}); _temp1721;}));( void*)( e->r=(
void*)(( void*) _temp1411->r));} goto _LL1312;} _LL1328: Cyc_Toc_exp_to_c( nv,
_temp1417); Cyc_Toc_exp_to_c( nv, _temp1415); Cyc_Toc_exp_to_c( nv, _temp1413);
goto _LL1312; _LL1330: Cyc_Toc_exp_to_c( nv, _temp1421); Cyc_Toc_exp_to_c( nv,
_temp1419); goto _LL1312; _LL1332: _temp1431= _temp1425; _temp1429= _temp1423;
goto _LL1334; _LL1334: Cyc_Toc_exp_to_c( nv, _temp1431);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1429); goto _LL1312; _LL1336:{
struct _RegionHandle _temp1723= _new_region(); struct _RegionHandle* r=&
_temp1723; _push_region( r);{ void* _temp1724=( void*) _temp1436->type; goto
_LL1725; _LL1725: { void* _temp1726= Cyc_Toc_typ_to_c( _temp1724); goto _LL1727;
_LL1727: { void* _temp1728= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1444->topt))->v); goto _LL1729; _LL1729: { struct Cyc_Absyn_Exp*
_temp1730= Cyc_Absyn_uint_exp(( unsigned int) _temp1440, 0); goto _LL1731;
_LL1731: { void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1800=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1800[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1801; _temp1801.tag= Cyc_Absyn_ArrayType;
_temp1801.f1=( void*) _temp1724; _temp1801.f2= Cyc_Toc_mt_tq; _temp1801.f3=(
struct Cyc_Absyn_Exp*) _temp1730; _temp1801;}); _temp1800;})); struct _tuple0*
_temp1732= Cyc_Toc_temp_var(); goto _LL1733; _LL1733: { struct _tuple0*
_temp1734= Cyc_Toc_temp_var(); goto _LL1735; _LL1735: { struct Cyc_Absyn_Exp*
_temp1736= Cyc_Absyn_var_exp( _temp1734, 0); goto _LL1737; _LL1737: { struct
_tuple0* _temp1738= Cyc_Toc_temp_var(); goto _LL1739; _LL1739: { int _temp1740=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1442); goto _LL1741;
_LL1741: { int _temp1742= _temp1740 - _temp1440; goto _LL1743; _LL1743: { struct
_tagged_ptr6 vs=({ unsigned int _temp1796=( unsigned int) _temp1440; struct
_tuple0** _temp1797=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1796); struct _tagged_ptr6 _temp1799={ _temp1797, _temp1797,
_temp1797 + _temp1796};{ unsigned int _temp1798= _temp1796; unsigned int i; for(
i= 0; i < _temp1798; i ++){ _temp1797[ i]= Cyc_Toc_temp_var();}}; _temp1799;});
struct _tagged_ptr6 xs=({ unsigned int _temp1792=( unsigned int) _temp1742;
struct _tuple0** _temp1793=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1792); struct _tagged_ptr6 _temp1795={ _temp1793, _temp1793,
_temp1793 + _temp1792};{ unsigned int _temp1794= _temp1792; unsigned int i; for(
i= 0; i < _temp1794; i ++){ _temp1793[ i]= Cyc_Toc_temp_var();}}; _temp1795;});
struct Cyc_List_List* xexps=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(*
f)( struct _tuple0*, struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,( struct Cyc_Position_Segment*)
0,(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct _tagged_ptr6 arr))
Cyc_List_rfrom_array)( r, xs)); xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List*
_temp1744=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1744->hd=( void*) Cyc_Absyn_var_exp( _temp1738, 0); _temp1744->tl= 0;
_temp1744;}));{ struct Cyc_Absyn_Stmt* _temp1745= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( _temp1732, 0), xexps, 0), 0); goto _LL1746; _LL1746: { struct
Cyc_Absyn_Exp* _temp1747= Cyc_Absyn_add_exp( _temp1736, _temp1730, 0); goto
_LL1748; _LL1748: { struct Cyc_List_List* _temp1758=({ struct Cyc_List_List*(*
_temp1749)( struct _tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4))
Cyc_List_list; struct _tuple4* _temp1752=({ struct _tuple4* _temp1757=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1757->f1= 0; _temp1757->f2=
_temp1736; _temp1757;}); struct _tuple4* _temp1753=({ struct _tuple4* _temp1756=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1756->f1= 0;
_temp1756->f2= _temp1736; _temp1756;}); struct _tuple4* _temp1754=({ struct
_tuple4* _temp1755=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1755->f1= 0; _temp1755->f2= _temp1747; _temp1755;}); struct _tuple4*
_temp1750[ 3u]={ _temp1752, _temp1753, _temp1754}; struct _tagged_ptr4 _temp1751={
_temp1750, _temp1750, _temp1750 + 3u}; _temp1749( _temp1751);}); goto _LL1759;
_LL1759: _temp1745= Cyc_Absyn_declare_stmt( _temp1738, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp1724,( void*) _temp1436->rgn, Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp(
0, _temp1758, 0), _temp1745, 0);{ struct Cyc_List_List* _temp1760=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_ptr6 arr)) Cyc_List_rfrom_array)( r, vs));
goto _LL1761; _LL1761: { struct Cyc_List_List* _temp1762=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_make_dle, _temp1760); goto _LL1763; _LL1763: _temp1745= Cyc_Absyn_declare_stmt(
_temp1734, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1762, 0), _temp1745, 0);{ struct Cyc_List_List* _temp1764=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1442);
goto _LL1765; _LL1765: { struct Cyc_List_List* _temp1766=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1438);
goto _LL1767; _LL1767:{ int i= _temp1440 - 1; for( 0; i >= 0;( i --, _temp1764=((
struct Cyc_List_List*) _check_null( _temp1764))->tl)){ struct Cyc_Absyn_Exp* arg=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1764))->hd;
void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c(
nv, arg); if( _temp1436->inject){ void* _temp1770; struct Cyc_Position_Segment*
_temp1772; struct Cyc_List_List* _temp1774; struct Cyc_List_List* _temp1776;
struct _tuple0* _temp1778; struct Cyc_Absyn_Tunionfield _temp1768=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1766))->hd); _LL1779: _temp1778=
_temp1768.name; goto _LL1777; _LL1777: _temp1776= _temp1768.tvs; goto _LL1775;
_LL1775: _temp1774= _temp1768.typs; goto _LL1773; _LL1773: _temp1772= _temp1768.loc;
goto _LL1771; _LL1771: _temp1770=( void*) _temp1768.sc; goto _LL1769; _LL1769:
_temp1766=(( struct Cyc_List_List*) _check_null( _temp1766))->tl;{ struct
_tuple0* _temp1780= Cyc_Toc_temp_var(); goto _LL1781; _LL1781: { struct Cyc_Absyn_Exp*
_temp1782= Cyc_Absyn_var_exp( _temp1780, 0); goto _LL1783; _LL1783: { void*
_temp1786= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1778,({
unsigned char* _temp1784=( unsigned char*)"_struct"; struct _tagged_string
_temp1785; _temp1785.curr= _temp1784; _temp1785.base= _temp1784; _temp1785.last_plus_one=
_temp1784 + 8; _temp1785;}))); goto _LL1787; _LL1787: _temp1745= Cyc_Absyn_declare_stmt(*((
struct _tuple0**(*)( struct _tagged_ptr6, unsigned int, unsigned int))
_check_unknown_subscript)( vs, sizeof( struct _tuple0*), i), _temp1726,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1726, Cyc_Absyn_address_exp( _temp1782,
0), 0), _temp1745, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp1774))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1788= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1782, Cyc_Toc_fieldname(
1), 0), arg, 0); goto _LL1789; _LL1789: _temp1745= Cyc_Absyn_seq_stmt( _temp1788,
_temp1745, 0);{ struct Cyc_Absyn_Stmt* _temp1790= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1782, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1778, 0), 0); goto
_LL1791; _LL1791: _temp1745= Cyc_Absyn_seq_stmt( _temp1790, _temp1745, 0);
_temp1745= Cyc_Absyn_declare_stmt( _temp1780, _temp1786, 0, _temp1745, 0);}}}}}}}
else{ _temp1745= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr6, unsigned int, unsigned int)) _check_unknown_subscript)( vs,
sizeof( struct _tuple0*), i), _temp1726,( struct Cyc_Absyn_Exp*) arg, _temp1745,
0);}}}{ int i= _temp1742 - 1; for( 0; i >= 0;( i --, _temp1764=(( struct Cyc_List_List*)
_check_null( _temp1764))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1764))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1745= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr6, unsigned int, unsigned int)) _check_unknown_subscript)( xs,
sizeof( struct _tuple0*), i), argtype,( struct Cyc_Absyn_Exp*) arg, _temp1745, 0);}}
Cyc_Toc_exp_to_c( nv, _temp1444); _temp1745= Cyc_Absyn_declare_stmt( _temp1732,
_temp1728,( struct Cyc_Absyn_Exp*) _temp1444, _temp1745, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1745, 0))->r));}}}}}}}}}}}}}}}}}};
_pop_region(& _temp1723);} goto _LL1312; _LL1338: Cyc_Toc_exp_to_c( nv,
_temp1446);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1446), 0))->r)); goto _LL1312; _LL1340: Cyc_Toc_exp_to_c(
nv, _temp1448); goto _LL1312; _LL1342: Cyc_Toc_exp_to_c( nv, _temp1452);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
for( 0; _temp1450 != 0; _temp1450=(( struct Cyc_List_List*) _check_null(
_temp1450))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*)
_check_null( _temp1450))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*)
Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1452, 0))->r)); break;}} goto _LL1312;} _LL1344: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1454->topt))->v; void* new_typ=*
_temp1458;* _temp1458= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1454);{ int noconv= 0;{ void* _temp1802= Cyc_Toc_conversion( old_t2,
new_typ); void* _temp1814; struct Cyc_Absyn_Exp* _temp1816; void* _temp1818; int
_temp1820; struct Cyc_Absyn_Exp* _temp1822; void* _temp1824; void* _temp1826;
_LL1804: if( _temp1802 ==( void*) Cyc_Toc_NoConv){ goto _LL1805;} else{ goto
_LL1806;} _LL1806: if(( unsigned int) _temp1802 > 1u?*(( int*) _temp1802) == Cyc_Toc_NullCheck:
0){ _LL1815: _temp1814=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp1802)->f1;
goto _LL1807;} else{ goto _LL1808;} _LL1808: if(( unsigned int) _temp1802 > 1u?*((
int*) _temp1802) == Cyc_Toc_TagPtr: 0){ _LL1819: _temp1818=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp1802)->f1; goto _LL1817; _LL1817: _temp1816=(( struct Cyc_Toc_TagPtr_struct*)
_temp1802)->f2; goto _LL1809;} else{ goto _LL1810;} _LL1810: if(( unsigned int)
_temp1802 > 1u?*(( int*) _temp1802) == Cyc_Toc_UntagPtr: 0){ _LL1825: _temp1824=(
void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp1802)->f1; goto _LL1823; _LL1823:
_temp1822=(( struct Cyc_Toc_UntagPtr_struct*) _temp1802)->f2; goto _LL1821;
_LL1821: _temp1820=(( struct Cyc_Toc_UntagPtr_struct*) _temp1802)->f3; goto
_LL1811;} else{ goto _LL1812;} _LL1812: if(( unsigned int) _temp1802 > 1u?*((
int*) _temp1802) == Cyc_Toc_UntagPtrNocheck: 0){ _LL1827: _temp1826=( void*)((
struct Cyc_Toc_UntagPtrNocheck_struct*) _temp1802)->f1; goto _LL1813;} else{
goto _LL1803;} _LL1805: goto _LL1803; _LL1807:( void*)( e->r=( void*)(( void*)(
Cyc_Toc_null_check_conv( nv->toplevel, _temp1814, _temp1454))->r)); goto _LL1803;
_LL1809:( void*)( e->r=( void*)(( void*)( Cyc_Toc_tag_ptr_conv( nv->toplevel,
_temp1818, _temp1816, _temp1454))->r)); goto _LL1803; _LL1811:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_untag_ptr_conv( nv->toplevel, _temp1824, _temp1822,
_temp1820, _temp1454))->r)); goto _LL1803; _LL1813:( void*)( _temp1454->r=( void*)((
void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp1454->r,
_temp1454->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1803; _LL1803:;} goto
_LL1312;}} _LL1346:{ void* _temp1828=( void*) _temp1459->r; struct Cyc_Absyn_Structdecl*
_temp1836; struct Cyc_List_List* _temp1838; struct Cyc_Core_Opt* _temp1840;
struct _tuple0* _temp1842; struct Cyc_List_List* _temp1844; _LL1830: if(*(( int*)
_temp1828) == Cyc_Absyn_Struct_e){ _LL1843: _temp1842=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1828)->f1; goto _LL1841; _LL1841: _temp1840=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1828)->f2; goto _LL1839; _LL1839: _temp1838=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1828)->f3; goto _LL1837; _LL1837: _temp1836=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1828)->f4; goto _LL1831;} else{ goto _LL1832;} _LL1832: if(*(( int*)
_temp1828) == Cyc_Absyn_Tuple_e){ _LL1845: _temp1844=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1828)->f1; goto _LL1833;} else{ goto _LL1834;} _LL1834: goto _LL1835;
_LL1831: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ struct _tagged_string _temp1846=( struct _tagged_string)
Cyc_Position_string_of_segment( _temp1459->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1846.last_plus_one - _temp1846.curr, _temp1846.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1459->topt))->v, 1, 0, _temp1838, _temp1842))->r)); goto
_LL1829; _LL1833: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ struct _tagged_string _temp1847=( struct _tagged_string)
Cyc_Position_string_of_segment( _temp1459->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1847.last_plus_one - _temp1847.curr, _temp1847.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1844))->r)); goto _LL1829;
_LL1835: Cyc_Toc_exp_to_c( nv, _temp1459); if( ! Cyc_Absyn_is_lvalue( _temp1459)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1459,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1459, 0))->r));} goto _LL1829; _LL1829:;} goto _LL1312; _LL1348: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
struct _tagged_string _temp1848=( struct _tagged_string) Cyc_Position_string_of_segment(
_temp1461->loc); xprintf("%.*s: new at top-level", _temp1848.last_plus_one -
_temp1848.curr, _temp1848.curr);}));}{ void* _temp1849=( void*) _temp1461->r;
struct Cyc_List_List* _temp1861; struct Cyc_Absyn_Exp* _temp1863; struct Cyc_Absyn_Exp*
_temp1865; struct Cyc_Absyn_Vardecl* _temp1867; struct Cyc_Absyn_Structdecl*
_temp1869; struct Cyc_List_List* _temp1871; struct Cyc_Core_Opt* _temp1873;
struct _tuple0* _temp1875; struct Cyc_List_List* _temp1877; _LL1851: if(*(( int*)
_temp1849) == Cyc_Absyn_Array_e){ _LL1862: _temp1861=(( struct Cyc_Absyn_Array_e_struct*)
_temp1849)->f1; goto _LL1852;} else{ goto _LL1853;} _LL1853: if(*(( int*)
_temp1849) == Cyc_Absyn_Comprehension_e){ _LL1868: _temp1867=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1849)->f1; goto _LL1866; _LL1866: _temp1865=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1849)->f2; goto _LL1864; _LL1864: _temp1863=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1849)->f3; goto _LL1854;} else{ goto _LL1855;} _LL1855: if(*(( int*)
_temp1849) == Cyc_Absyn_Struct_e){ _LL1876: _temp1875=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1849)->f1; goto _LL1874; _LL1874: _temp1873=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1849)->f2; goto _LL1872; _LL1872: _temp1871=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1849)->f3; goto _LL1870; _LL1870: _temp1869=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1849)->f4; goto _LL1856;} else{ goto _LL1857;} _LL1857: if(*(( int*)
_temp1849) == Cyc_Absyn_Tuple_e){ _LL1878: _temp1877=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1849)->f1; goto _LL1858;} else{ goto _LL1859;} _LL1859: goto _LL1860;
_LL1852: { struct _tuple0* _temp1879= Cyc_Toc_temp_var(); goto _LL1880; _LL1880: {
struct Cyc_Absyn_Exp* _temp1881= Cyc_Absyn_var_exp( _temp1879, 0); goto _LL1882;
_LL1882: { struct Cyc_Absyn_Stmt* _temp1883= Cyc_Toc_init_array( nv, _temp1881,
_temp1861, Cyc_Absyn_exp_stmt( _temp1881, 0)); goto _LL1884; _LL1884: { void*
old_elt_typ;{ void* _temp1885= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1891; struct Cyc_Absyn_Conref* _temp1893; struct Cyc_Absyn_Tqual _temp1895;
struct Cyc_Absyn_Conref* _temp1897; void* _temp1899; void* _temp1901; _LL1887:
if(( unsigned int) _temp1885 > 4u?*(( int*) _temp1885) == Cyc_Absyn_PointerType:
0){ _LL1892: _temp1891=(( struct Cyc_Absyn_PointerType_struct*) _temp1885)->f1;
_LL1902: _temp1901=( void*) _temp1891.elt_typ; goto _LL1900; _LL1900: _temp1899=(
void*) _temp1891.rgn_typ; goto _LL1898; _LL1898: _temp1897= _temp1891.nullable;
goto _LL1896; _LL1896: _temp1895= _temp1891.tq; goto _LL1894; _LL1894: _temp1893=
_temp1891.bounds; goto _LL1888;} else{ goto _LL1889;} _LL1889: goto _LL1890;
_LL1888: old_elt_typ= _temp1901; goto _LL1886; _LL1890: old_elt_typ=(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1903=(
unsigned char*)"exp_to_c:new array expression doesn't have ptr type"; struct
_tagged_string _temp1904; _temp1904.curr= _temp1903; _temp1904.base= _temp1903;
_temp1904.last_plus_one= _temp1903 + 52; _temp1904;})); goto _LL1886; _LL1886:;}{
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1905= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1906; _LL1906: { struct Cyc_Absyn_Exp*
_temp1907= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1861), 0), 0); goto
_LL1908; _LL1908: { struct Cyc_Absyn_Exp* e1; if( _temp1463 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1907);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1463); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1907);}{ struct Cyc_Absyn_Exp* _temp1909= Cyc_Absyn_cast_exp( _temp1905, e1,
0); goto _LL1910; _LL1910:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1879, _temp1905,( struct Cyc_Absyn_Exp*) _temp1909,
_temp1883, 0), 0))->r)); goto _LL1850;}}}}}}}} _LL1854: { int is_tagged_ptr= 0;{
void* _temp1911= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1917; struct Cyc_Absyn_Conref* _temp1919; struct Cyc_Absyn_Tqual _temp1921;
struct Cyc_Absyn_Conref* _temp1923; void* _temp1925; void* _temp1927; _LL1913:
if(( unsigned int) _temp1911 > 4u?*(( int*) _temp1911) == Cyc_Absyn_PointerType:
0){ _LL1918: _temp1917=(( struct Cyc_Absyn_PointerType_struct*) _temp1911)->f1;
_LL1928: _temp1927=( void*) _temp1917.elt_typ; goto _LL1926; _LL1926: _temp1925=(
void*) _temp1917.rgn_typ; goto _LL1924; _LL1924: _temp1923= _temp1917.nullable;
goto _LL1922; _LL1922: _temp1921= _temp1917.tq; goto _LL1920; _LL1920: _temp1919=
_temp1917.bounds; goto _LL1914;} else{ goto _LL1915;} _LL1915: goto _LL1916;
_LL1914:{ void* _temp1929=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1919))->v; void* _temp1935; _LL1931: if((
unsigned int) _temp1929 > 1u?*(( int*) _temp1929) == Cyc_Absyn_Eq_constr: 0){
_LL1936: _temp1935=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1929)->f1;
if( _temp1935 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1932;} else{ goto _LL1933;}}
else{ goto _LL1933;} _LL1933: goto _LL1934; _LL1932: is_tagged_ptr= 1; goto
_LL1930; _LL1934: goto _LL1930; _LL1930:;} goto _LL1912; _LL1916:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1937=(
unsigned char*)"exp_to_c: comprehension not an array type"; struct
_tagged_string _temp1938; _temp1938.curr= _temp1937; _temp1938.base= _temp1937;
_temp1938.last_plus_one= _temp1937 + 42; _temp1938;})); goto _LL1912; _LL1912:;}{
struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1863->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1865);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1867, Cyc_Absyn_var_exp(
max, 0), _temp1863, Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1939=
_new_region(); struct _RegionHandle* r=& _temp1939; _push_region( r);{ struct
Cyc_List_List* decls=({ struct Cyc_List_List* _temp1969=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1969->hd=( void*)({
struct _tuple10* _temp1970=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1970->f1= max; _temp1970->f2= Cyc_Absyn_uint_t; _temp1970->f3=(
struct Cyc_Absyn_Exp*) _temp1865; _temp1970;}); _temp1969->tl= 0; _temp1969;});
struct Cyc_Absyn_Exp* ai; if( _temp1463 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1463); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1940=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1940->hd=( void*)({ struct _tuple10*
_temp1941=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1941->f1= a; _temp1941->f2= ptr_typ; _temp1941->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1941;}); _temp1940->tl= decls; _temp1940;}); if( is_tagged_ptr){
struct _tuple0* _temp1942= Cyc_Toc_temp_var(); goto _LL1943; _LL1943: { void*
_temp1944= Cyc_Toc_typ_to_c( old_typ); goto _LL1945; _LL1945: { struct Cyc_Absyn_Exp*
_temp1955= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(* _temp1946)(
struct _tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4)) Cyc_List_list;
struct _tuple4* _temp1949=({ struct _tuple4* _temp1954=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1954->f1= 0; _temp1954->f2= Cyc_Absyn_var_exp(
a, 0); _temp1954;}); struct _tuple4* _temp1950=({ struct _tuple4* _temp1953=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1953->f1= 0;
_temp1953->f2= Cyc_Absyn_var_exp( a, 0); _temp1953;}); struct _tuple4* _temp1951=({
struct _tuple4* _temp1952=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1952->f1= 0; _temp1952->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1952;}); struct _tuple4* _temp1947[ 3u]={
_temp1949, _temp1950, _temp1951}; struct _tagged_ptr4 _temp1948={ _temp1947,
_temp1947, _temp1947 + 3u}; _temp1946( _temp1948);}), 0); goto _LL1956; _LL1956:
decls=({ struct Cyc_List_List* _temp1957=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1957->hd=( void*)({ struct _tuple10*
_temp1958=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1958->f1= _temp1942; _temp1958->f2= _temp1944; _temp1958->f3=( struct Cyc_Absyn_Exp*)
_temp1955; _temp1958;}); _temp1957->tl= decls; _temp1957;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1942, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
_temp1959= decls; goto _LL1960; _LL1960: for( 0; _temp1959 != 0; _temp1959=((
struct Cyc_List_List*) _check_null( _temp1959))->tl){ struct Cyc_Absyn_Exp*
_temp1963; void* _temp1965; struct _tuple0* _temp1967; struct _tuple10 _temp1961=*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1959))->hd);
_LL1968: _temp1967= _temp1961.f1; goto _LL1966; _LL1966: _temp1965= _temp1961.f2;
goto _LL1964; _LL1964: _temp1963= _temp1961.f3; goto _LL1962; _LL1962: s= Cyc_Absyn_declare_stmt(
_temp1967, _temp1965, _temp1963, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region(& _temp1939);} goto _LL1850;}}} _LL1856:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1461->topt))->v, 1, _temp1463, _temp1871, _temp1875))->r));
goto _LL1850; _LL1858:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv,
1, _temp1463, _temp1877))->r)); goto _LL1850; _LL1860: { void* _temp1971=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1461->topt))->v; goto _LL1972; _LL1972: {
struct _tuple0* _temp1973= Cyc_Toc_temp_var(); goto _LL1974; _LL1974: { struct
Cyc_Absyn_Stmt* _temp1975= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1973, 0),
0); goto _LL1976; _LL1976: { struct Cyc_Absyn_Exp* _temp1977= Cyc_Absyn_signed_int_exp(
0, 0); goto _LL1978; _LL1978: Cyc_Toc_exp_to_c( nv, _temp1461); _temp1975= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1973, 0),
_temp1977, 0), _temp1461, 0), _temp1975, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
_temp1971); void* _temp1979= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto
_LL1980; _LL1980: { struct Cyc_Absyn_Exp* _temp1981= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0); goto _LL1982; _LL1982: { struct Cyc_Absyn_Exp* e1; if( _temp1463 ==
0){ e1= Cyc_Toc_malloc_exp( _temp1971, _temp1981);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp1463); Cyc_Toc_exp_to_c( nv, r); e1=
Cyc_Toc_rmalloc_exp( r, _temp1981);}{ struct Cyc_Absyn_Exp* _temp1983= Cyc_Absyn_cast_exp(
_temp1979, e1, 0); goto _LL1984; _LL1984:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1973, _temp1979,( struct Cyc_Absyn_Exp*) _temp1983,
_temp1975, 0), 0))->r)); goto _LL1850;}}}}}}}} _LL1850:;} goto _LL1312; _LL1350:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp1985=(
struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1985[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1986; _temp1986.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1986.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1465);
_temp1986;}); _temp1985;}))); goto _LL1312; _LL1352: Cyc_Toc_exp_to_c( nv,
_temp1467); goto _LL1312; _LL1354: { void* e1_typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1469->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1469);{ void* _temp1987= e1_typ; struct Cyc_Absyn_PtrInfo _temp1993; struct
Cyc_Absyn_Conref* _temp1995; struct Cyc_Absyn_Tqual _temp1997; struct Cyc_Absyn_Conref*
_temp1999; void* _temp2001; void* _temp2003; _LL1989: if(( unsigned int)
_temp1987 > 4u?*(( int*) _temp1987) == Cyc_Absyn_PointerType: 0){ _LL1994:
_temp1993=(( struct Cyc_Absyn_PointerType_struct*) _temp1987)->f1; _LL2004:
_temp2003=( void*) _temp1993.elt_typ; goto _LL2002; _LL2002: _temp2001=( void*)
_temp1993.rgn_typ; goto _LL2000; _LL2000: _temp1999= _temp1993.nullable; goto
_LL1998; _LL1998: _temp1997= _temp1993.tq; goto _LL1996; _LL1996: _temp1995=
_temp1993.bounds; goto _LL1990;} else{ goto _LL1991;} _LL1991: goto _LL1992;
_LL1990:{ void* _temp2005=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1995); struct Cyc_Absyn_Exp* _temp2011; _LL2007: if( _temp2005 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL2008;} else{ goto _LL2009;} _LL2009: if((
unsigned int) _temp2005 > 1u?*(( int*) _temp2005) == Cyc_Absyn_Upper_b: 0){
_LL2012: _temp2011=(( struct Cyc_Absyn_Upper_b_struct*) _temp2005)->f1; goto
_LL2010;} else{ goto _LL2006;} _LL2008: { struct _tuple0* x= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* _temp2013= Cyc_Toc_typ_to_c_array(
_temp2003); goto _LL2014; _LL2014: { void* tx= Cyc_Toc_add_tagged_ptr_type(
_temp2013, _temp1997); void* ta2= Cyc_Absyn_cstar_typ( _temp2013, _temp1997);
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
struct Cyc_Absyn_Exp*) _temp1469, s1, 0), 0))->r)); goto _LL2006;}} _LL2010: if(
Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1469->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp2015=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2015->hd=(
void*) _temp1469; _temp2015->tl= 0; _temp2015;}), 0), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, e1_typ,(
struct Cyc_Absyn_Exp*) _temp1469, s3, 0), 0))->r));}} goto _LL2006; _LL2006:;}
goto _LL1988; _LL1992:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2016=( unsigned char*)"exp_to_c: Deref: non-pointer"; struct
_tagged_string _temp2017; _temp2017.curr= _temp2016; _temp2017.base= _temp2016;
_temp2017.last_plus_one= _temp2016 + 29; _temp2017;})); goto _LL1988; _LL1988:;}
goto _LL1312;} _LL1356: Cyc_Toc_exp_to_c( nv, _temp1473); if( Cyc_Toc_is_poly_project(
e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1312; _LL1358: { void* e1typ= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1477->topt))->v); Cyc_Toc_exp_to_c(
nv, _temp1477);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp2020; struct Cyc_Absyn_Tqual _temp2022; struct Cyc_Absyn_Conref* _temp2024;
void* _temp2026; void* _temp2028; struct Cyc_Absyn_PtrInfo _temp2018= Cyc_Toc_get_ptr_type(
e1typ); _LL2029: _temp2028=( void*) _temp2018.elt_typ; goto _LL2027; _LL2027:
_temp2026=( void*) _temp2018.rgn_typ; goto _LL2025; _LL2025: _temp2024=
_temp2018.nullable; goto _LL2023; _LL2023: _temp2022= _temp2018.tq; goto _LL2021;
_LL2021: _temp2020= _temp2018.bounds; goto _LL2019; _LL2019:{ void* _temp2030=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2020); struct
Cyc_Absyn_Exp* _temp2036; _LL2032: if(( unsigned int) _temp2030 > 1u?*(( int*)
_temp2030) == Cyc_Absyn_Upper_b: 0){ _LL2037: _temp2036=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2030)->f1; goto _LL2033;} else{ goto _LL2034;} _LL2034: if( _temp2030 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL2035;} else{ goto _LL2031;} _LL2033: if(
Cyc_Evexp_eval_const_uint_exp( _temp2036) < 1){(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp2038=( unsigned char*)"exp_to_c:  StructArrow_e on pointer of size 0";
struct _tagged_string _temp2039; _temp2039.curr= _temp2038; _temp2039.base=
_temp2038; _temp2039.last_plus_one= _temp2038 + 46; _temp2039;}));}{ int
possibly_null;{ void* _temp2040=( void*)((( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2024))->v; int _temp2046;
_LL2042: if(( unsigned int) _temp2040 > 1u?*(( int*) _temp2040) == Cyc_Absyn_Eq_constr:
0){ _LL2047: _temp2046=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2040)->f1;
goto _LL2043;} else{ goto _LL2044;} _LL2044: goto _LL2045; _LL2043:
possibly_null= _temp2046; goto _LL2041; _LL2045: possibly_null= 0; goto _LL2041;
_LL2041:;} if( Cyc_Toc_is_nullable(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1477->topt))->v)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1477->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp2048=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2048->hd=(
void*) _temp1477; _temp2048->tl= 0; _temp2048;}), 0), 0), _temp1475, 0))->r));}
else{ struct _tuple0* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1477->topt))->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1475, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1477, s3, 0), 0))->r));}} goto _LL2031;} _LL2035: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2028); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2022); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2058=({ struct Cyc_List_List*(* _temp2049)( struct
_tagged_ptr7)=( struct Cyc_List_List*(*)( struct _tagged_ptr7)) Cyc_List_list;
struct _tuple1* _temp2052=({ struct _tuple1* _temp2057=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2057->f1= 0; _temp2057->f2= Cyc_Toc_mt_tq;
_temp2057->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1477->topt))->v); _temp2057;}); struct _tuple1* _temp2053=({ struct _tuple1*
_temp2056=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2056->f1=
0; _temp2056->f2= Cyc_Toc_mt_tq; _temp2056->f3= Cyc_Absyn_uint_t; _temp2056;});
struct _tuple1* _temp2054=({ struct _tuple1* _temp2055=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2055->f1= 0; _temp2055->f2= Cyc_Toc_mt_tq;
_temp2055->f3= Cyc_Absyn_uint_t; _temp2055;}); struct _tuple1* _temp2050[ 3u]={
_temp2052, _temp2053, _temp2054}; struct _tagged_ptr7 _temp2051={ _temp2050,
_temp2050, _temp2050 + 3u}; _temp2049( _temp2051);}); goto _LL2059; _LL2059: {
void* _temp2063= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2060=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2060[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2061; _temp2061.tag= Cyc_Absyn_FnType;
_temp2061.f1=({ struct Cyc_Absyn_FnInfo _temp2062; _temp2062.tvars= 0; _temp2062.effect=
0; _temp2062.ret_typ=( void*) ta2; _temp2062.args= _temp2058; _temp2062.c_varargs=
0; _temp2062.cyc_varargs= 0; _temp2062.attributes= 0; _temp2062;}); _temp2061;});
_temp2060;}), Cyc_Toc_mt_tq); goto _LL2064; _LL2064: { struct Cyc_Absyn_Exp*
fnexp= Cyc_Absyn_cast_exp( _temp2063, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv,
0), 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_uint_exp( 0, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_fncall_exp( fnexp,({
struct Cyc_List_List*(* _temp2065)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)(
struct _tagged_ptr8)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2068= _temp1477;
struct Cyc_Absyn_Exp* _temp2069= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2070= e2; struct Cyc_Absyn_Exp* _temp2066[ 3u]={ _temp2068, _temp2069,
_temp2070}; struct _tagged_ptr8 _temp2067={ _temp2066, _temp2066, _temp2066 + 3u};
_temp2065( _temp2067);}), 0), _temp1475, 0))->r));}}} else{ struct _tuple0* x=
Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2022);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( a, 0), _temp1475, 0), 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt( 0), 0); struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt* s1=
Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,(
struct Cyc_Absyn_Exp*) _temp1477, s1, 0), 0))->r));} goto _LL2031;} _LL2031:;}
if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1312;}} _LL1360: { void* _temp2071= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1481->topt))->v); goto _LL2072;
_LL2072: Cyc_Toc_exp_to_c( nv, _temp1481); Cyc_Toc_exp_to_c( nv, _temp1479);{
void* _temp2073= _temp2071; struct Cyc_List_List* _temp2081; struct Cyc_Absyn_PtrInfo
_temp2083; struct Cyc_Absyn_Conref* _temp2085; struct Cyc_Absyn_Tqual _temp2087;
struct Cyc_Absyn_Conref* _temp2089; void* _temp2091; void* _temp2093; _LL2075:
if(( unsigned int) _temp2073 > 4u?*(( int*) _temp2073) == Cyc_Absyn_TupleType: 0){
_LL2082: _temp2081=(( struct Cyc_Absyn_TupleType_struct*) _temp2073)->f1; goto
_LL2076;} else{ goto _LL2077;} _LL2077: if(( unsigned int) _temp2073 > 4u?*((
int*) _temp2073) == Cyc_Absyn_PointerType: 0){ _LL2084: _temp2083=(( struct Cyc_Absyn_PointerType_struct*)
_temp2073)->f1; _LL2094: _temp2093=( void*) _temp2083.elt_typ; goto _LL2092;
_LL2092: _temp2091=( void*) _temp2083.rgn_typ; goto _LL2090; _LL2090: _temp2089=
_temp2083.nullable; goto _LL2088; _LL2088: _temp2087= _temp2083.tq; goto _LL2086;
_LL2086: _temp2085= _temp2083.bounds; goto _LL2078;} else{ goto _LL2079;}
_LL2079: goto _LL2080; _LL2076: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1479) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1481, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL2074;} _LL2078:{ void*
_temp2095=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp2085); struct Cyc_Absyn_Exp* _temp2101; _LL2097: if(( unsigned int)
_temp2095 > 1u?*(( int*) _temp2095) == Cyc_Absyn_Upper_b: 0){ _LL2102: _temp2101=((
struct Cyc_Absyn_Upper_b_struct*) _temp2095)->f1; goto _LL2098;} else{ goto
_LL2099;} _LL2099: if( _temp2095 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2100;}
else{ goto _LL2096;} _LL2098: _temp2101= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp2101), 0);{ int possibly_null;{ void* _temp2103=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2089))->v; int
_temp2109; _LL2105: if(( unsigned int) _temp2103 > 1u?*(( int*) _temp2103) ==
Cyc_Absyn_Eq_constr: 0){ _LL2110: _temp2109=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2103)->f1; goto _LL2106;} else{ goto _LL2107;} _LL2107: goto _LL2108;
_LL2106: possibly_null= _temp2109; goto _LL2104; _LL2108: possibly_null= 0; goto
_LL2104; _LL2104:;}{ void* ta1= Cyc_Toc_typ_to_c( _temp2093); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp2087); if( Cyc_Toc_use_runtimecheck_funs){ if( possibly_null){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv, 0),({ struct Cyc_List_List*(*
_temp2111)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)( struct _tagged_ptr8))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp2114= _temp1481; struct Cyc_Absyn_Exp*
_temp2115= _temp2101; struct Cyc_Absyn_Exp* _temp2116= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp2117= _temp1479; struct Cyc_Absyn_Exp*
_temp2112[ 4u]={ _temp2114, _temp2115, _temp2116, _temp2117}; struct
_tagged_ptr8 _temp2113={ _temp2112, _temp2112, _temp2112 + 4u}; _temp2111(
_temp2113);}), 0), 0), 0))->r));} else{( void*)( _temp1479->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List*(* _temp2118)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)(
struct _tagged_ptr8)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2121= _temp2101;
struct Cyc_Absyn_Exp* _temp2122= Cyc_Absyn_copy_exp( _temp1479); struct Cyc_Absyn_Exp*
_temp2119[ 2u]={ _temp2121, _temp2122}; struct _tagged_ptr8 _temp2120={
_temp2119, _temp2119, _temp2119 + 2u}; _temp2118( _temp2120);}), 0))->r));}}
else{ struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp(
i, 0), _temp2101, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp((
void*) e3->r, 0), 0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1479, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1481, s1, 0), 0))->r));}} goto _LL2096;}} _LL2100: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2093); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2087); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2132=({ struct Cyc_List_List*(* _temp2123)( struct
_tagged_ptr7)=( struct Cyc_List_List*(*)( struct _tagged_ptr7)) Cyc_List_list;
struct _tuple1* _temp2126=({ struct _tuple1* _temp2131=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2131->f1= 0; _temp2131->f2= Cyc_Toc_mt_tq;
_temp2131->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1481->topt))->v); _temp2131;}); struct _tuple1* _temp2127=({ struct _tuple1*
_temp2130=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2130->f1=
0; _temp2130->f2= Cyc_Toc_mt_tq; _temp2130->f3= Cyc_Absyn_uint_t; _temp2130;});
struct _tuple1* _temp2128=({ struct _tuple1* _temp2129=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2129->f1= 0; _temp2129->f2= Cyc_Toc_mt_tq;
_temp2129->f3= Cyc_Absyn_uint_t; _temp2129;}); struct _tuple1* _temp2124[ 3u]={
_temp2126, _temp2127, _temp2128}; struct _tagged_ptr7 _temp2125={ _temp2124,
_temp2124, _temp2124 + 3u}; _temp2123( _temp2125);}); goto _LL2133; _LL2133: {
void* _temp2137= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2134=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2134[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2135; _temp2135.tag= Cyc_Absyn_FnType;
_temp2135.f1=({ struct Cyc_Absyn_FnInfo _temp2136; _temp2136.tvars= 0; _temp2136.effect=
0; _temp2136.ret_typ=( void*) ta2; _temp2136.args= _temp2132; _temp2136.c_varargs=
0; _temp2136.cyc_varargs= 0; _temp2136.attributes= 0; _temp2136;}); _temp2135;});
_temp2134;}), Cyc_Toc_mt_tq); goto _LL2138; _LL2138: { struct Cyc_Absyn_Exp*
fnexp= Cyc_Absyn_cast_exp( _temp2137, Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv,
0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp(
fnexp,({ struct Cyc_List_List*(* _temp2139)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)(
struct _tagged_ptr8)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2142= _temp1481;
struct Cyc_Absyn_Exp* _temp2143= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2144= _temp1479; struct Cyc_Absyn_Exp* _temp2140[ 3u]={ _temp2142,
_temp2143, _temp2144}; struct _tagged_ptr8 _temp2141={ _temp2140, _temp2140,
_temp2140 + 3u}; _temp2139( _temp2141);}), 0), 0))->r));}}} else{ struct _tuple0*
x= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2087);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0), _temp1479, 0); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0);
struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1481, s1, 0), 0))->r));} goto _LL2096;} _LL2096:;}
goto _LL2074; _LL2080:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2145=( unsigned char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp2146; _temp2146.curr= _temp2145; _temp2146.base=
_temp2145; _temp2146.last_plus_one= _temp2145 + 49; _temp2146;})); goto _LL2074;
_LL2074:;} goto _LL1312;} _LL1362: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1483))->r));} else{ struct Cyc_List_List*
_temp2147=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1483); goto
_LL2148; _LL2148: { struct _tagged_string* _temp2149= Cyc_Toc_add_tuple_type(
_temp2147); goto _LL2150; _LL2150: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1483 != 0;( _temp1483=(( struct Cyc_List_List*) _check_null(
_temp1483))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1483))->hd); dles=({ struct Cyc_List_List*
_temp2151=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2151->hd=( void*)({ struct _tuple4* _temp2152=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2152->f1= 0; _temp2152->f2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1483))->hd; _temp2152;}); _temp2151->tl=
dles; _temp2151;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));}}} goto _LL1312; _LL1364:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ unsigned char* _temp2153=( unsigned char*)"compoundlit";
struct _tagged_string _temp2154; _temp2154.curr= _temp2153; _temp2154.base=
_temp2153; _temp2154.last_plus_one= _temp2153 + 12; _temp2154;})); goto _LL1312;
_LL1366:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1489, 0))->r));{ struct Cyc_List_List* _temp2155= _temp1489; goto _LL2156;
_LL2156: for( 0; _temp2155 != 0; _temp2155=(( struct Cyc_List_List*) _check_null(
_temp2155))->tl){ struct _tuple4 _temp2159; struct Cyc_Absyn_Exp* _temp2160;
struct Cyc_List_List* _temp2162; struct _tuple4* _temp2157=( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp2155))->hd; _temp2159=* _temp2157;
_LL2163: _temp2162= _temp2159.f1; goto _LL2161; _LL2161: _temp2160= _temp2159.f2;
goto _LL2158; _LL2158: Cyc_Toc_exp_to_c( nv, _temp2160);}} goto _LL1312; _LL1368: {
unsigned int _temp2164= Cyc_Evexp_eval_const_uint_exp( _temp1493); goto _LL2165;
_LL2165: Cyc_Toc_exp_to_c( nv, _temp1491);{ struct Cyc_List_List* es= 0; if( !
Cyc_Toc_is_zero( _temp1491)){ unsigned int i= 0; for( 0; i < _temp2164; i ++){
es=({ struct Cyc_List_List* _temp2166=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2166->hd=( void*)({ struct _tuple4*
_temp2167=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp2167->f1=
0; _temp2167->f2= _temp1491; _temp2167;}); _temp2166->tl= es; _temp2166;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL1312;}} _LL1370: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1499, _temp1503))->r));} else{ if(
_temp1497 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2168=( unsigned char*)"Struct_e: missing structdecl pointer";
struct _tagged_string _temp2169; _temp2169.curr= _temp2168; _temp2169.base=
_temp2168; _temp2169.last_plus_one= _temp2168 + 37; _temp2169;}));}{ struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*) _check_null( _temp1497); struct
_RegionHandle _temp2170= _new_region(); struct _RegionHandle* rgn=& _temp2170;
_push_region( rgn);{ struct Cyc_List_List* _temp2171=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1499,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd2->fields))->v); goto _LL2172; _LL2172: { struct Cyc_List_List* _temp2173= 0;
goto _LL2174; _LL2174:{ struct Cyc_List_List* _temp2175=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto _LL2176; _LL2176: for(
0; _temp2175 != 0; _temp2175=(( struct Cyc_List_List*) _check_null( _temp2175))->tl){
struct Cyc_List_List* _temp2177= _temp2171; goto _LL2178; _LL2178: for( 0;
_temp2177 != 0; _temp2177=(( struct Cyc_List_List*) _check_null( _temp2177))->tl){
if((*(( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2177))->hd)).f1
==( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2175))->hd){ struct _tuple12 _temp2181; struct Cyc_Absyn_Exp* _temp2182;
struct Cyc_Absyn_Structfield* _temp2184; struct _tuple12* _temp2179=( struct
_tuple12*)(( struct Cyc_List_List*) _check_null( _temp2177))->hd; _temp2181=*
_temp2179; _LL2185: _temp2184= _temp2181.f1; goto _LL2183; _LL2183: _temp2182=
_temp2181.f2; goto _LL2180; _LL2180: { void* _temp2186=( void*) _temp2184->type;
goto _LL2187; _LL2187: Cyc_Toc_exp_to_c( nv, _temp2182); if( Cyc_Toc_is_void_star(
_temp2186)){( void*)( _temp2182->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp2182->r, 0), 0))->r));} _temp2173=({ struct Cyc_List_List*
_temp2188=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2188->hd=( void*)({ struct _tuple4* _temp2189=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2189->f1= 0; _temp2189->f2= _temp2182; _temp2189;});
_temp2188->tl= _temp2173; _temp2188;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp2173), 0))->r));}}; _pop_region(& _temp2170);}} goto
_LL1312; _LL1372: { struct Cyc_List_List* fs;{ void* _temp2190= Cyc_Tcutil_compress(
_temp1507); struct Cyc_List_List* _temp2196; _LL2192: if(( unsigned int)
_temp2190 > 4u?*(( int*) _temp2190) == Cyc_Absyn_AnonStructType: 0){ _LL2197:
_temp2196=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2190)->f1; goto
_LL2193;} else{ goto _LL2194;} _LL2194: goto _LL2195; _LL2193: fs= _temp2196;
goto _LL2191; _LL2195: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct _tagged_string _temp2198=
Cyc_Absynpp_typ2string( _temp1507); xprintf("anon struct has type %.*s",
_temp2198.last_plus_one - _temp2198.curr, _temp2198.curr);})); goto _LL2191;
_LL2191:;}{ struct _RegionHandle _temp2199= _new_region(); struct _RegionHandle*
rgn=& _temp2199; _push_region( rgn);{ struct Cyc_List_List* _temp2200=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1505, fs); goto _LL2201; _LL2201: for( 0; _temp2200 != 0;
_temp2200=(( struct Cyc_List_List*) _check_null( _temp2200))->tl){ struct
_tuple12 _temp2204; struct Cyc_Absyn_Exp* _temp2205; struct Cyc_Absyn_Structfield*
_temp2207; struct _tuple12* _temp2202=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp2200))->hd; _temp2204=* _temp2202; _LL2208: _temp2207=
_temp2204.f1; goto _LL2206; _LL2206: _temp2205= _temp2204.f2; goto _LL2203;
_LL2203: { void* _temp2209=( void*) _temp2207->type; goto _LL2210; _LL2210: Cyc_Toc_exp_to_c(
nv, _temp2205); if( Cyc_Toc_is_void_star( _temp2209)){( void*)( _temp2205->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp2205->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1505, 0))->r));}; _pop_region(& _temp2199);} goto _LL1312;} _LL1374: {
struct _tuple0* qv= _temp1509->name; if( _temp1511->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2211= _temp1511->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1511->fields))->v;
goto _LL2212; _LL2212: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2211))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2211))->hd)->typs
== 0){ tag_count ++;} _temp2211=(( struct Cyc_List_List*) _check_null( _temp2211))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1312;} _LL1376: { struct _tuple0* _temp2213= _temp1519->name; goto
_LL2214; _LL2214: { struct Cyc_List_List* _temp2215= _temp1519->typs; goto
_LL2216; _LL2216: { struct _tuple0* _temp2217= Cyc_Toc_temp_var(); goto _LL2218;
_LL2218: { struct Cyc_Absyn_Exp* _temp2219= Cyc_Absyn_var_exp( _temp2217, 0);
goto _LL2220; _LL2220: { void* _temp2223= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2213,({ unsigned char* _temp2221=( unsigned char*)"_struct"; struct
_tagged_string _temp2222; _temp2222.curr= _temp2221; _temp2222.base= _temp2221;
_temp2222.last_plus_one= _temp2221 + 8; _temp2222;}))); goto _LL2224; _LL2224:
if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp; if( _temp1521->is_xtunion){
tag_exp= Cyc_Absyn_var_exp( _temp2213, 0);} else{ int tag_count= 0; struct Cyc_List_List*
_temp2225= _temp1521->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1521->fields))->v; goto _LL2226; _LL2226: while( Cyc_Absyn_qvar_cmp(
_temp2213,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2225))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2225))->hd)->typs != 0){ tag_count ++;}
_temp2225=(( struct Cyc_List_List*) _check_null( _temp2225))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2227= 0; goto _LL2228;
_LL2228: for( 0; _temp1523 != 0;( _temp1523=(( struct Cyc_List_List*)
_check_null( _temp1523))->tl, _temp2215=(( struct Cyc_List_List*) _check_null(
_temp2215))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1523))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2215))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2227=({ struct Cyc_List_List* _temp2229=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2229[ 0]=({ struct Cyc_List_List
_temp2230; _temp2230.hd=( void*)({ struct _tuple4* _temp2231=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2231[ 0]=({ struct _tuple4
_temp2232; _temp2232.f1= 0; _temp2232.f2= cur_e; _temp2232;}); _temp2231;});
_temp2230.tl= _temp2227; _temp2230;}); _temp2229;});} _temp2227=({ struct Cyc_List_List*
_temp2233=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2233[ 0]=({ struct Cyc_List_List _temp2234; _temp2234.hd=( void*)({ struct
_tuple4* _temp2235=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2235[ 0]=({ struct _tuple4 _temp2236; _temp2236.f1= 0; _temp2236.f2=
tag_exp; _temp2236;}); _temp2235;}); _temp2234.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2227); _temp2234;}); _temp2233;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2237=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2237->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2213,({ unsigned char*
_temp2238=( unsigned char*)"_struct"; struct _tagged_string _temp2239; _temp2239.curr=
_temp2238; _temp2239.base= _temp2238; _temp2239.last_plus_one= _temp2238 + 8;
_temp2239;})); _temp2237;}), _temp2227, 0))->r));}} else{ struct Cyc_List_List*
_temp2241=({ struct Cyc_List_List* _temp2240=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2240->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2219, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp2213, 0), 0); _temp2240->tl= 0; _temp2240;}); goto _LL2242; _LL2242:{ int i=
1; for( 0; _temp1523 != 0;((( _temp1523=(( struct Cyc_List_List*) _check_null(
_temp1523))->tl, i ++)), _temp2215=(( struct Cyc_List_List*) _check_null(
_temp2215))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1523))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2215))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp2243= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2219, Cyc_Toc_fieldname( i), 0), cur_e, 0);
goto _LL2244; _LL2244: _temp2241=({ struct Cyc_List_List* _temp2245=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2245->hd=( void*) _temp2243;
_temp2245->tl= _temp2241; _temp2245;});}}}{ struct Cyc_Absyn_Stmt* _temp2246=
Cyc_Absyn_exp_stmt( _temp2219, 0); goto _LL2247; _LL2247: { struct Cyc_Absyn_Stmt*
_temp2249= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2248=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2248->hd=( void*) _temp2246;
_temp2248->tl= _temp2241; _temp2248;})), 0); goto _LL2250; _LL2250:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp2217,
_temp2223, 0, _temp2249, 0), 0))->r));}}} goto _LL1312;}}}}} _LL1378: goto
_LL1312; _LL1380: { void* t_c= Cyc_Toc_typ_to_c( _temp1535); if( _temp1537 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1537); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1535, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL1312;} _LL1382: Cyc_Toc_stmt_to_c(
nv, _temp1539); goto _LL1312; _LL1384:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2251=( unsigned char*)"UnresolvedMem"; struct _tagged_string
_temp2252; _temp2252.curr= _temp2251; _temp2252.base= _temp2251; _temp2252.last_plus_one=
_temp2251 + 14; _temp2252;})); goto _LL1312; _LL1386:(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp2253=( unsigned char*)"codegen";
struct _tagged_string _temp2254; _temp2254.curr= _temp2253; _temp2254.base=
_temp2253; _temp2254.last_plus_one= _temp2253 + 8; _temp2254;})); goto _LL1312;
_LL1388:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp2255=( unsigned char*)"fill"; struct _tagged_string _temp2256; _temp2256.curr=
_temp2255; _temp2256.base= _temp2255; _temp2256.last_plus_one= _temp2255 + 5;
_temp2256;})); goto _LL1312; _LL1312:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
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
_temp2257=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp2293; struct Cyc_Absyn_Vardecl
_temp2295; struct Cyc_List_List* _temp2296; struct Cyc_Core_Opt* _temp2298;
struct Cyc_Absyn_Exp* _temp2300; void* _temp2302; struct Cyc_Absyn_Tqual
_temp2304; struct _tuple0* _temp2306; void* _temp2308; struct Cyc_Absyn_Vardecl*
_temp2310; int _temp2312; void* _temp2314; unsigned char _temp2316; struct
_tagged_string _temp2318; struct Cyc_Absyn_Enumfield* _temp2320; struct Cyc_Absyn_Enumdecl*
_temp2322; struct Cyc_List_List* _temp2324; struct Cyc_List_List* _temp2326;
struct Cyc_Absyn_Tunionfield* _temp2328; struct Cyc_Absyn_Tuniondecl* _temp2330;
struct Cyc_Absyn_Pat* _temp2333; struct Cyc_Absyn_Pat _temp2335; struct Cyc_Position_Segment*
_temp2336; struct Cyc_Core_Opt* _temp2338; void* _temp2340; struct Cyc_List_List*
_temp2342; struct Cyc_List_List* _temp2344; struct Cyc_Absyn_Tunionfield*
_temp2346; struct Cyc_Absyn_Tuniondecl* _temp2348; struct Cyc_List_List*
_temp2350; struct Cyc_List_List* _temp2352; struct Cyc_Absyn_Tunionfield*
_temp2354; struct Cyc_Absyn_Tuniondecl* _temp2356; struct Cyc_List_List*
_temp2358; struct Cyc_List_List* _temp2360; struct Cyc_List_List* _temp2362;
struct Cyc_Core_Opt* _temp2364; struct Cyc_Absyn_Structdecl* _temp2366; struct
Cyc_Absyn_Pat* _temp2368; struct _tuple0* _temp2370; struct Cyc_List_List*
_temp2372; struct Cyc_List_List* _temp2374; struct _tuple0* _temp2376; struct
Cyc_List_List* _temp2378; struct Cyc_List_List* _temp2380; struct _tuple0*
_temp2382; _LL2259: if(( unsigned int) _temp2257 > 2u?*(( int*) _temp2257) ==
Cyc_Absyn_Var_p: 0){ _LL2294: _temp2293=(( struct Cyc_Absyn_Var_p_struct*)
_temp2257)->f1; _temp2295=* _temp2293; _LL2309: _temp2308=( void*) _temp2295.sc;
goto _LL2307; _LL2307: _temp2306= _temp2295.name; goto _LL2305; _LL2305:
_temp2304= _temp2295.tq; goto _LL2303; _LL2303: _temp2302=( void*) _temp2295.type;
goto _LL2301; _LL2301: _temp2300= _temp2295.initializer; goto _LL2299; _LL2299:
_temp2298= _temp2295.rgn; goto _LL2297; _LL2297: _temp2296= _temp2295.attributes;
goto _LL2260;} else{ goto _LL2261;} _LL2261: if( _temp2257 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2262;} else{ goto _LL2263;} _LL2263: if(( unsigned int) _temp2257 > 2u?*((
int*) _temp2257) == Cyc_Absyn_Reference_p: 0){ _LL2311: _temp2310=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2257)->f1; goto _LL2264;} else{ goto _LL2265;} _LL2265: if( _temp2257 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2266;} else{ goto _LL2267;} _LL2267: if((
unsigned int) _temp2257 > 2u?*(( int*) _temp2257) == Cyc_Absyn_Int_p: 0){
_LL2315: _temp2314=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2257)->f1;
goto _LL2313; _LL2313: _temp2312=(( struct Cyc_Absyn_Int_p_struct*) _temp2257)->f2;
goto _LL2268;} else{ goto _LL2269;} _LL2269: if(( unsigned int) _temp2257 > 2u?*((
int*) _temp2257) == Cyc_Absyn_Char_p: 0){ _LL2317: _temp2316=(( struct Cyc_Absyn_Char_p_struct*)
_temp2257)->f1; goto _LL2270;} else{ goto _LL2271;} _LL2271: if(( unsigned int)
_temp2257 > 2u?*(( int*) _temp2257) == Cyc_Absyn_Float_p: 0){ _LL2319: _temp2318=((
struct Cyc_Absyn_Float_p_struct*) _temp2257)->f1; goto _LL2272;} else{ goto
_LL2273;} _LL2273: if(( unsigned int) _temp2257 > 2u?*(( int*) _temp2257) == Cyc_Absyn_Enum_p:
0){ _LL2323: _temp2322=(( struct Cyc_Absyn_Enum_p_struct*) _temp2257)->f1; goto
_LL2321; _LL2321: _temp2320=(( struct Cyc_Absyn_Enum_p_struct*) _temp2257)->f2;
goto _LL2274;} else{ goto _LL2275;} _LL2275: if(( unsigned int) _temp2257 > 2u?*((
int*) _temp2257) == Cyc_Absyn_Tunion_p: 0){ _LL2331: _temp2330=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2257)->f1; goto _LL2329; _LL2329: _temp2328=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2257)->f2; goto _LL2327; _LL2327: _temp2326=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2257)->f3; goto _LL2325; _LL2325: _temp2324=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2257)->f4; if( _temp2324 == 0){ goto _LL2276;} else{ goto _LL2277;}} else{
goto _LL2277;} _LL2277: if(( unsigned int) _temp2257 > 2u?*(( int*) _temp2257)
== Cyc_Absyn_Pointer_p: 0){ _LL2334: _temp2333=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2257)->f1; _temp2335=* _temp2333; _LL2341: _temp2340=( void*) _temp2335.r;
if(( unsigned int) _temp2340 > 2u?*(( int*) _temp2340) == Cyc_Absyn_Tunion_p: 0){
_LL2349: _temp2348=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2340)->f1; goto
_LL2347; _LL2347: _temp2346=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2340)->f2;
goto _LL2345; _LL2345: _temp2344=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2340)->f3;
goto _LL2343; _LL2343: _temp2342=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2340)->f4;
goto _LL2339;} else{ goto _LL2279;} _LL2339: _temp2338= _temp2335.topt; goto
_LL2337; _LL2337: _temp2336= _temp2335.loc; goto _LL2332;} else{ goto _LL2279;}
_LL2332: if( _temp2342 != 0){ goto _LL2278;} else{ goto _LL2279;} _LL2279: if((
unsigned int) _temp2257 > 2u?*(( int*) _temp2257) == Cyc_Absyn_Tunion_p: 0){
_LL2357: _temp2356=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2257)->f1; goto
_LL2355; _LL2355: _temp2354=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2257)->f2;
goto _LL2353; _LL2353: _temp2352=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2257)->f3;
goto _LL2351; _LL2351: _temp2350=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2257)->f4;
goto _LL2280;} else{ goto _LL2281;} _LL2281: if(( unsigned int) _temp2257 > 2u?*((
int*) _temp2257) == Cyc_Absyn_Tuple_p: 0){ _LL2359: _temp2358=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2257)->f1; goto _LL2282;} else{ goto _LL2283;} _LL2283: if(( unsigned int)
_temp2257 > 2u?*(( int*) _temp2257) == Cyc_Absyn_Struct_p: 0){ _LL2367:
_temp2366=(( struct Cyc_Absyn_Struct_p_struct*) _temp2257)->f1; goto _LL2365;
_LL2365: _temp2364=(( struct Cyc_Absyn_Struct_p_struct*) _temp2257)->f2; goto
_LL2363; _LL2363: _temp2362=(( struct Cyc_Absyn_Struct_p_struct*) _temp2257)->f3;
goto _LL2361; _LL2361: _temp2360=(( struct Cyc_Absyn_Struct_p_struct*) _temp2257)->f4;
goto _LL2284;} else{ goto _LL2285;} _LL2285: if(( unsigned int) _temp2257 > 2u?*((
int*) _temp2257) == Cyc_Absyn_Pointer_p: 0){ _LL2369: _temp2368=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2257)->f1; goto _LL2286;} else{ goto _LL2287;} _LL2287: if(( unsigned int)
_temp2257 > 2u?*(( int*) _temp2257) == Cyc_Absyn_UnknownId_p: 0){ _LL2371:
_temp2370=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2257)->f1; goto _LL2288;}
else{ goto _LL2289;} _LL2289: if(( unsigned int) _temp2257 > 2u?*(( int*)
_temp2257) == Cyc_Absyn_UnknownCall_p: 0){ _LL2377: _temp2376=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2257)->f1; goto _LL2375; _LL2375: _temp2374=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2257)->f2; goto _LL2373; _LL2373: _temp2372=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2257)->f3; goto _LL2290;} else{ goto _LL2291;} _LL2291: if(( unsigned int)
_temp2257 > 2u?*(( int*) _temp2257) == Cyc_Absyn_UnknownFields_p: 0){ _LL2383:
_temp2382=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2257)->f1; goto
_LL2381; _LL2381: _temp2380=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2257)->f2; goto _LL2379; _LL2379: _temp2378=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2257)->f3; goto _LL2292;} else{ goto _LL2258;} _LL2260: nv= Cyc_Toc_add_varmap(
nv, _temp2306, r); goto _LL2262; _LL2262: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2258; _LL2264: { struct _tuple0* _temp2384= Cyc_Toc_temp_var(); goto
_LL2385; _LL2385: decls=({ struct Cyc_List_List* _temp2386=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2386->hd=( void*)({
struct _tuple14* _temp2387=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2387->f1= _temp2384; _temp2387->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2387;}); _temp2386->tl= decls;
_temp2386;}); nv= Cyc_Toc_add_varmap( nv, _temp2310->name, Cyc_Absyn_var_exp(
_temp2384, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2384, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2258;} _LL2266: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2258; _LL2268: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2314, _temp2312, 0), succ_lab, fail_lab); goto _LL2258;
_LL2270: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2316, 0),
succ_lab, fail_lab); goto _LL2258; _LL2272: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2318, 0), succ_lab, fail_lab); goto _LL2258; _LL2274: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2388=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2388[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2389; _temp2389.tag= Cyc_Absyn_Enum_e;
_temp2389.f1= _temp2320->name; _temp2389.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2322; _temp2389.f3=( struct Cyc_Absyn_Enumfield*) _temp2320; _temp2389;});
_temp2388;}), 0), succ_lab, fail_lab); goto _LL2258; _LL2276: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2328->name, 0); if( ! _temp2330->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2258;} _LL2278: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2342);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2346->name,({
unsigned char* _temp2423=( unsigned char*)"_struct"; struct _tagged_string
_temp2424; _temp2424.curr= _temp2423; _temp2424.base= _temp2423; _temp2424.last_plus_one=
_temp2423 + 8; _temp2424;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp2390=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2346->typs); goto _LL2391;
_LL2391: { struct Cyc_List_List* _temp2392=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2342); goto
_LL2393; _LL2393: for( 0; _temp2392 != 0;((( _temp2392=(( struct Cyc_List_List*)
_check_null( _temp2392))->tl, _temp2390=(( struct Cyc_List_List*) _check_null(
_temp2390))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp2394=( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp2392))->hd; goto _LL2395; _LL2395: {
void* _temp2396=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2390))->hd)).f2; goto _LL2397; _LL2397: { struct _tuple0* _temp2398= Cyc_Toc_temp_var();
goto _LL2399; _LL2399: { void* _temp2400=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2394->topt))->v; goto _LL2401; _LL2401: { void* _temp2402= Cyc_Toc_typ_to_c(
_temp2400); goto _LL2403; _LL2403: { struct _tagged_string* _temp2404= Cyc_Toc_fresh_label();
goto _LL2405; _LL2405: { struct Cyc_Absyn_Exp* _temp2406= Cyc_Absyn_structarrow_exp(
rcast, Cyc_Toc_fieldname( cnt), 0); goto _LL2407; _LL2407: if( Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c( _temp2396))){ _temp2406= Cyc_Absyn_cast_exp( _temp2402,
_temp2406, 0);} decls=({ struct Cyc_List_List* _temp2408=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2408->hd=( void*)({
struct _tuple14* _temp2409=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2409->f1= _temp2398; _temp2409->f2= _temp2402; _temp2409;});
_temp2408->tl= decls; _temp2408;});{ struct _tuple13 _temp2410= Cyc_Toc_xlate_pat(
nv, rgn, _temp2400, Cyc_Absyn_var_exp( _temp2398, 0), _temp2406, _temp2394,
succ_lab, fail_lab, decls); goto _LL2411; _LL2411: nv= _temp2410.f1; decls=
_temp2410.f2;{ struct Cyc_Absyn_Stmt* _temp2412= _temp2410.f3; goto _LL2413;
_LL2413: { struct Cyc_Absyn_Stmt* _temp2414= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2398, 0), _temp2406, 0); goto _LL2415; _LL2415: { struct Cyc_Absyn_Stmt*
_temp2416= Cyc_Absyn_seq_stmt( _temp2414, _temp2412, 0); goto _LL2417; _LL2417:
ss=({ struct Cyc_List_List* _temp2418=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2418->hd=( void*) Cyc_Absyn_label_stmt(
_temp2404, _temp2416, 0); _temp2418->tl= ss; _temp2418;}); succ_lab= _temp2404;}}}}}}}}}}}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2348->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2346->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2419=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2348->fields))->v; goto _LL2420; _LL2420: for( 0; _temp2419 !=
0; _temp2419=(( struct Cyc_List_List*) _check_null( _temp2419))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2421=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2419))->hd; goto _LL2422; _LL2422: if( _temp2421->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2346->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2258;}}} _LL2280: _temp2358=
_temp2350; goto _LL2282; _LL2282: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2358);{ struct Cyc_List_List*
_temp2425=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2358); goto _LL2426; _LL2426: for( 0; _temp2425 !=
0;( _temp2425=(( struct Cyc_List_List*) _check_null( _temp2425))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2427=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2425))->hd; goto _LL2428; _LL2428: { struct _tuple0* _temp2429=
Cyc_Toc_temp_var(); goto _LL2430; _LL2430: { void* _temp2431=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2427->topt))->v; goto _LL2432; _LL2432: {
struct _tagged_string* _temp2433= Cyc_Toc_fresh_label(); goto _LL2434; _LL2434:
decls=({ struct Cyc_List_List* _temp2435=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2435->hd=( void*)({ struct _tuple14*
_temp2436=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2436->f1= _temp2429; _temp2436->f2= Cyc_Toc_typ_to_c( _temp2431); _temp2436;});
_temp2435->tl= decls; _temp2435;});{ struct _tuple13 _temp2437= Cyc_Toc_xlate_pat(
nv, rgn, _temp2431, Cyc_Absyn_var_exp( _temp2429, 0), Cyc_Absyn_structmember_exp(
path, Cyc_Toc_fieldname( cnt), 0), _temp2427, succ_lab, fail_lab, decls); goto
_LL2438; _LL2438: nv= _temp2437.f1; decls= _temp2437.f2;{ struct Cyc_Absyn_Stmt*
_temp2439= _temp2437.f3; goto _LL2440; _LL2440: { struct Cyc_Absyn_Stmt*
_temp2441= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2429, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2439, 0); goto _LL2442; _LL2442: ss=({ struct Cyc_List_List* _temp2443=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2443->hd=(
void*) Cyc_Absyn_label_stmt( _temp2433, _temp2441, 0); _temp2443->tl= ss;
_temp2443;}); succ_lab= _temp2433;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2258;} _LL2284: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2444=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2360); goto _LL2445; _LL2445: for( 0; _temp2444 !=
0; _temp2444=(( struct Cyc_List_List*) _check_null( _temp2444))->tl){ struct
_tuple15* _temp2446=( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2444))->hd; goto _LL2447; _LL2447: { struct Cyc_Absyn_Pat* _temp2448=(*
_temp2446).f2; goto _LL2449; _LL2449: { struct _tagged_string* f;{ void*
_temp2450=( void*)(( struct Cyc_List_List*) _check_null((* _temp2446).f1))->hd;
struct _tagged_string* _temp2456; _LL2452: if(*(( int*) _temp2450) == Cyc_Absyn_FieldName){
_LL2457: _temp2456=(( struct Cyc_Absyn_FieldName_struct*) _temp2450)->f1; goto
_LL2453;} else{ goto _LL2454;} _LL2454: goto _LL2455; _LL2453: f= _temp2456;
goto _LL2451; _LL2455:( void) _throw(( void*) Cyc_Toc_Match_error); _LL2451:;}{
struct _tuple0* _temp2458= Cyc_Toc_temp_var(); goto _LL2459; _LL2459: { void*
_temp2460=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2448->topt))->v;
goto _LL2461; _LL2461: { void* _temp2462= Cyc_Toc_typ_to_c( _temp2460); goto
_LL2463; _LL2463: { struct _tagged_string* _temp2464= Cyc_Toc_fresh_label();
goto _LL2465; _LL2465: decls=({ struct Cyc_List_List* _temp2466=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2466->hd=( void*)({
struct _tuple14* _temp2467=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2467->f1= _temp2458; _temp2467->f2= _temp2462; _temp2467;});
_temp2466->tl= decls; _temp2466;});{ struct _tuple13 _temp2468= Cyc_Toc_xlate_pat(
nv, rgn, _temp2460, Cyc_Absyn_var_exp( _temp2458, 0), Cyc_Absyn_structmember_exp(
path, f, 0), _temp2448, succ_lab, fail_lab, decls); goto _LL2469; _LL2469: nv=
_temp2468.f1; decls= _temp2468.f2;{ struct Cyc_Absyn_Exp* _temp2470= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2471; _LL2471: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2366->fields))->v, f)))->type)){ _temp2470= Cyc_Absyn_cast_exp(
_temp2462, _temp2470, 0);}{ struct Cyc_Absyn_Stmt* _temp2472= _temp2468.f3; goto
_LL2473; _LL2473: { struct Cyc_Absyn_Stmt* _temp2474= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2458, 0), _temp2470, 0), _temp2472, 0); goto _LL2475;
_LL2475: ss=({ struct Cyc_List_List* _temp2476=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2476->hd=( void*) Cyc_Absyn_label_stmt(
_temp2464, _temp2474, 0); _temp2476->tl= ss; _temp2476;}); succ_lab= _temp2464;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2258;} _LL2286: { struct _tuple0*
_temp2477= Cyc_Toc_temp_var(); goto _LL2478; _LL2478: { void* _temp2479=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2368->topt))->v; goto _LL2480; _LL2480:
decls=({ struct Cyc_List_List* _temp2481=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2481->hd=( void*)({ struct _tuple14*
_temp2482=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2482->f1= _temp2477; _temp2482->f2= Cyc_Toc_typ_to_c( _temp2479); _temp2482;});
_temp2481->tl= decls; _temp2481;});{ struct _tuple13 _temp2483= Cyc_Toc_xlate_pat(
nv, rgn, _temp2479, Cyc_Absyn_var_exp( _temp2477, 0), Cyc_Absyn_deref_exp( path,
0), _temp2368, succ_lab, fail_lab, decls); goto _LL2484; _LL2484: nv= _temp2483.f1;
decls= _temp2483.f2;{ struct Cyc_Absyn_Stmt* _temp2485= _temp2483.f3; goto
_LL2486; _LL2486: { struct Cyc_Absyn_Stmt* _temp2487= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2477, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2485, 0);
goto _LL2488; _LL2488: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2487, 0);} else{ s= _temp2487;} goto _LL2258;}}}}} _LL2288: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2489=( unsigned char*)"unknownid"; struct _tagged_string
_temp2490; _temp2490.curr= _temp2489; _temp2490.base= _temp2489; _temp2490.last_plus_one=
_temp2489 + 10; _temp2490;})); goto _LL2258; _LL2290: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2491=(
unsigned char*)"unknowncall"; struct _tagged_string _temp2492; _temp2492.curr=
_temp2491; _temp2492.base= _temp2491; _temp2492.last_plus_one= _temp2491 + 12;
_temp2492;})); goto _LL2258; _LL2292: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2493=(
unsigned char*)"unknownfields"; struct _tagged_string _temp2494; _temp2494.curr=
_temp2493; _temp2494.base= _temp2493; _temp2494.last_plus_one= _temp2493 + 14;
_temp2494;})); goto _LL2258; _LL2258:;} return({ struct _tuple13 _temp2495;
_temp2495.f1= nv; _temp2495.f2= decls; _temp2495.f3= s; _temp2495;});} struct
_tuple16{ struct _tagged_string* f1; struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple16* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple16* _temp2496=(
struct _tuple16*) _region_malloc( r, sizeof( struct _tuple16)); _temp2496->f1=
Cyc_Toc_fresh_label(); _temp2496->f2= Cyc_Toc_fresh_label(); _temp2496->f3= sc;
_temp2496;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp2497=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2498; _LL2498: { int leave_as_switch;{ void*
_temp2499= Cyc_Tcutil_compress( _temp2497); void* _temp2505; void* _temp2507;
_LL2501: if(( unsigned int) _temp2499 > 4u?*(( int*) _temp2499) == Cyc_Absyn_IntType:
0){ _LL2508: _temp2507=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2499)->f1;
goto _LL2506; _LL2506: _temp2505=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2499)->f2; goto _LL2502;} else{ goto _LL2503;} _LL2503: goto _LL2504;
_LL2502: leave_as_switch= 1; goto _LL2500; _LL2504: leave_as_switch= 0; goto
_LL2500; _LL2500:;}{ struct Cyc_List_List* _temp2509= scs; goto _LL2510; _LL2510:
for( 0; _temp2509 != 0; _temp2509=(( struct Cyc_List_List*) _check_null(
_temp2509))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2509))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2509))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2511= scs; goto
_LL2512; _LL2512: for( 0; _temp2511 != 0; _temp2511=(( struct Cyc_List_List*)
_check_null( _temp2511))->tl){ struct Cyc_Absyn_Stmt* _temp2513=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2511))->hd)->body; goto _LL2514;
_LL2514:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2511))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2513, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2513);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2515=
_new_region(); struct _RegionHandle* rgn=& _temp2515; _push_region( rgn);{
struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2516= lscs; goto
_LL2517; _LL2517: for( 0; _temp2516 != 0; _temp2516=(( struct Cyc_List_List*)
_check_null( _temp2516))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct
_tuple16*)(( struct Cyc_List_List*) _check_null( _temp2516))->hd)).f3; struct
_tagged_string* fail_lab=(( struct Cyc_List_List*) _check_null( _temp2516))->tl
== 0? end_l:(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp2516))->tl))->hd)).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2516))->hd)).f2; if( sc->where_clause == 0){ struct _tuple13
_temp2518= Cyc_Toc_xlate_pat( nv, rgn, _temp2497, r, path, sc->pattern, succ_lab,
fail_lab, decls); goto _LL2519; _LL2519: nvs=({ struct Cyc_List_List* _temp2520=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2520->hd=( void*) _temp2518.f1; _temp2520->tl= nvs; _temp2520;}); decls=
_temp2518.f2; test_stmts=({ struct Cyc_List_List* _temp2521=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2521->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2516))->hd)).f1,
_temp2518.f3, 0); _temp2521->tl= test_stmts; _temp2521;});} else{ struct Cyc_Absyn_Exp*
_temp2522=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2523;
_LL2523: { struct _tagged_string* _temp2524= Cyc_Toc_fresh_label(); goto _LL2525;
_LL2525: { struct _tuple13 _temp2526= Cyc_Toc_xlate_pat( nv, rgn, _temp2497, r,
path, sc->pattern, _temp2524, fail_lab, decls); goto _LL2527; _LL2527: Cyc_Toc_exp_to_c(
_temp2526.f1, _temp2522); nvs=({ struct Cyc_List_List* _temp2528=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2528->hd=( void*)
_temp2526.f1; _temp2528->tl= nvs; _temp2528;}); decls= _temp2526.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2522, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2524, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2529=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2529->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2516))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2526.f3, s3, 0), 0); _temp2529->tl= test_stmts;
_temp2529;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2530= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2531; _LL2531: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2532=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2533; _LL2533: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2532, end_l,(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2534=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2535; _LL2535: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2534, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2530, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple14 _temp2538;
void* _temp2539; struct _tuple0* _temp2541; struct _tuple14* _temp2536=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2538=*
_temp2536; _LL2542: _temp2541= _temp2538.f1; goto _LL2540; _LL2540: _temp2539=
_temp2538.f2; goto _LL2537; _LL2537: res= Cyc_Absyn_declare_stmt( _temp2541,
_temp2539, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region(& _temp2515);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2543=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2543[ 0]=({ struct Cyc_List_List _temp2544; _temp2544.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2544.tl= 0; _temp2544;}); _temp2543;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2545=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2587; struct Cyc_Absyn_Stmt* _temp2589; struct Cyc_Absyn_Stmt*
_temp2591; struct Cyc_Absyn_Exp* _temp2593; struct Cyc_Absyn_Stmt* _temp2595;
struct Cyc_Absyn_Stmt* _temp2597; struct Cyc_Absyn_Exp* _temp2599; struct Cyc_Absyn_Stmt*
_temp2601; struct _tuple2 _temp2603; struct Cyc_Absyn_Stmt* _temp2605; struct
Cyc_Absyn_Exp* _temp2607; struct Cyc_Absyn_Stmt* _temp2609; struct Cyc_Absyn_Stmt*
_temp2611; struct Cyc_Absyn_Stmt* _temp2613; struct _tagged_string* _temp2615;
struct Cyc_Absyn_Stmt* _temp2617; struct _tuple2 _temp2619; struct Cyc_Absyn_Stmt*
_temp2621; struct Cyc_Absyn_Exp* _temp2623; struct _tuple2 _temp2625; struct Cyc_Absyn_Stmt*
_temp2627; struct Cyc_Absyn_Exp* _temp2629; struct Cyc_Absyn_Exp* _temp2631;
struct Cyc_List_List* _temp2633; struct Cyc_Absyn_Exp* _temp2635; struct Cyc_Absyn_Switch_clause**
_temp2637; struct Cyc_List_List* _temp2639; struct Cyc_Absyn_Stmt* _temp2641;
struct Cyc_Absyn_Decl* _temp2643; struct Cyc_Absyn_Stmt* _temp2645; struct
_tagged_string* _temp2647; struct _tuple2 _temp2649; struct Cyc_Absyn_Stmt*
_temp2651; struct Cyc_Absyn_Exp* _temp2653; struct Cyc_Absyn_Stmt* _temp2655;
struct Cyc_List_List* _temp2657; struct Cyc_Absyn_Stmt* _temp2659; struct Cyc_Absyn_Stmt*
_temp2661; struct Cyc_Absyn_Vardecl* _temp2663; struct Cyc_Absyn_Tvar* _temp2665;
struct Cyc_List_List* _temp2667; struct Cyc_Absyn_Exp* _temp2669; struct Cyc_Absyn_Stmt*
_temp2671; struct Cyc_Absyn_Stmt* _temp2673; _LL2547: if( _temp2545 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2548;} else{ goto _LL2549;} _LL2549: if((
unsigned int) _temp2545 > 1u?*(( int*) _temp2545) == Cyc_Absyn_Exp_s: 0){
_LL2588: _temp2587=(( struct Cyc_Absyn_Exp_s_struct*) _temp2545)->f1; goto
_LL2550;} else{ goto _LL2551;} _LL2551: if(( unsigned int) _temp2545 > 1u?*((
int*) _temp2545) == Cyc_Absyn_Seq_s: 0){ _LL2592: _temp2591=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2545)->f1; goto _LL2590; _LL2590: _temp2589=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2545)->f2; goto _LL2552;} else{ goto _LL2553;} _LL2553: if(( unsigned int)
_temp2545 > 1u?*(( int*) _temp2545) == Cyc_Absyn_Return_s: 0){ _LL2594:
_temp2593=(( struct Cyc_Absyn_Return_s_struct*) _temp2545)->f1; goto _LL2554;}
else{ goto _LL2555;} _LL2555: if(( unsigned int) _temp2545 > 1u?*(( int*)
_temp2545) == Cyc_Absyn_IfThenElse_s: 0){ _LL2600: _temp2599=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2545)->f1; goto _LL2598; _LL2598: _temp2597=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2545)->f2; goto _LL2596; _LL2596: _temp2595=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2545)->f3; goto _LL2556;} else{ goto _LL2557;} _LL2557: if(( unsigned int)
_temp2545 > 1u?*(( int*) _temp2545) == Cyc_Absyn_While_s: 0){ _LL2604: _temp2603=((
struct Cyc_Absyn_While_s_struct*) _temp2545)->f1; _LL2608: _temp2607= _temp2603.f1;
goto _LL2606; _LL2606: _temp2605= _temp2603.f2; goto _LL2602; _LL2602: _temp2601=((
struct Cyc_Absyn_While_s_struct*) _temp2545)->f2; goto _LL2558;} else{ goto
_LL2559;} _LL2559: if(( unsigned int) _temp2545 > 1u?*(( int*) _temp2545) == Cyc_Absyn_Break_s:
0){ _LL2610: _temp2609=(( struct Cyc_Absyn_Break_s_struct*) _temp2545)->f1; goto
_LL2560;} else{ goto _LL2561;} _LL2561: if(( unsigned int) _temp2545 > 1u?*((
int*) _temp2545) == Cyc_Absyn_Continue_s: 0){ _LL2612: _temp2611=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2545)->f1; goto _LL2562;} else{ goto _LL2563;} _LL2563: if(( unsigned int)
_temp2545 > 1u?*(( int*) _temp2545) == Cyc_Absyn_Goto_s: 0){ _LL2616: _temp2615=((
struct Cyc_Absyn_Goto_s_struct*) _temp2545)->f1; goto _LL2614; _LL2614:
_temp2613=(( struct Cyc_Absyn_Goto_s_struct*) _temp2545)->f2; goto _LL2564;}
else{ goto _LL2565;} _LL2565: if(( unsigned int) _temp2545 > 1u?*(( int*)
_temp2545) == Cyc_Absyn_For_s: 0){ _LL2632: _temp2631=(( struct Cyc_Absyn_For_s_struct*)
_temp2545)->f1; goto _LL2626; _LL2626: _temp2625=(( struct Cyc_Absyn_For_s_struct*)
_temp2545)->f2; _LL2630: _temp2629= _temp2625.f1; goto _LL2628; _LL2628:
_temp2627= _temp2625.f2; goto _LL2620; _LL2620: _temp2619=(( struct Cyc_Absyn_For_s_struct*)
_temp2545)->f3; _LL2624: _temp2623= _temp2619.f1; goto _LL2622; _LL2622:
_temp2621= _temp2619.f2; goto _LL2618; _LL2618: _temp2617=(( struct Cyc_Absyn_For_s_struct*)
_temp2545)->f4; goto _LL2566;} else{ goto _LL2567;} _LL2567: if(( unsigned int)
_temp2545 > 1u?*(( int*) _temp2545) == Cyc_Absyn_Switch_s: 0){ _LL2636:
_temp2635=(( struct Cyc_Absyn_Switch_s_struct*) _temp2545)->f1; goto _LL2634;
_LL2634: _temp2633=(( struct Cyc_Absyn_Switch_s_struct*) _temp2545)->f2; goto
_LL2568;} else{ goto _LL2569;} _LL2569: if(( unsigned int) _temp2545 > 1u?*((
int*) _temp2545) == Cyc_Absyn_Fallthru_s: 0){ _LL2640: _temp2639=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2545)->f1; goto _LL2638; _LL2638: _temp2637=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2545)->f2; goto _LL2570;} else{ goto _LL2571;} _LL2571: if(( unsigned int)
_temp2545 > 1u?*(( int*) _temp2545) == Cyc_Absyn_Decl_s: 0){ _LL2644: _temp2643=((
struct Cyc_Absyn_Decl_s_struct*) _temp2545)->f1; goto _LL2642; _LL2642:
_temp2641=(( struct Cyc_Absyn_Decl_s_struct*) _temp2545)->f2; goto _LL2572;}
else{ goto _LL2573;} _LL2573: if(( unsigned int) _temp2545 > 1u?*(( int*)
_temp2545) == Cyc_Absyn_Label_s: 0){ _LL2648: _temp2647=(( struct Cyc_Absyn_Label_s_struct*)
_temp2545)->f1; goto _LL2646; _LL2646: _temp2645=(( struct Cyc_Absyn_Label_s_struct*)
_temp2545)->f2; goto _LL2574;} else{ goto _LL2575;} _LL2575: if(( unsigned int)
_temp2545 > 1u?*(( int*) _temp2545) == Cyc_Absyn_Do_s: 0){ _LL2656: _temp2655=((
struct Cyc_Absyn_Do_s_struct*) _temp2545)->f1; goto _LL2650; _LL2650: _temp2649=((
struct Cyc_Absyn_Do_s_struct*) _temp2545)->f2; _LL2654: _temp2653= _temp2649.f1;
goto _LL2652; _LL2652: _temp2651= _temp2649.f2; goto _LL2576;} else{ goto
_LL2577;} _LL2577: if(( unsigned int) _temp2545 > 1u?*(( int*) _temp2545) == Cyc_Absyn_TryCatch_s:
0){ _LL2660: _temp2659=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2545)->f1;
goto _LL2658; _LL2658: _temp2657=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2545)->f2; goto _LL2578;} else{ goto _LL2579;} _LL2579: if(( unsigned int)
_temp2545 > 1u?*(( int*) _temp2545) == Cyc_Absyn_Region_s: 0){ _LL2666:
_temp2665=(( struct Cyc_Absyn_Region_s_struct*) _temp2545)->f1; goto _LL2664;
_LL2664: _temp2663=(( struct Cyc_Absyn_Region_s_struct*) _temp2545)->f2; goto
_LL2662; _LL2662: _temp2661=(( struct Cyc_Absyn_Region_s_struct*) _temp2545)->f3;
goto _LL2580;} else{ goto _LL2581;} _LL2581: if(( unsigned int) _temp2545 > 1u?*((
int*) _temp2545) == Cyc_Absyn_SwitchC_s: 0){ _LL2670: _temp2669=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2545)->f1; goto _LL2668; _LL2668: _temp2667=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2545)->f2; goto _LL2582;} else{ goto _LL2583;} _LL2583: if(( unsigned int)
_temp2545 > 1u?*(( int*) _temp2545) == Cyc_Absyn_Cut_s: 0){ _LL2672: _temp2671=((
struct Cyc_Absyn_Cut_s_struct*) _temp2545)->f1; goto _LL2584;} else{ goto
_LL2585;} _LL2585: if(( unsigned int) _temp2545 > 1u?*(( int*) _temp2545) == Cyc_Absyn_Splice_s:
0){ _LL2674: _temp2673=(( struct Cyc_Absyn_Splice_s_struct*) _temp2545)->f1;
goto _LL2586;} else{ goto _LL2546;} _LL2548: return; _LL2550: Cyc_Toc_exp_to_c(
nv, _temp2587); return; _LL2552: Cyc_Toc_stmt_to_c( nv, _temp2591); s= _temp2589;
continue; _LL2554: { struct Cyc_Core_Opt* topt= 0; if( _temp2593 != 0){ topt=({
struct Cyc_Core_Opt* _temp2675=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2675->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2593))->topt))->v);
_temp2675;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2593));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2676=
Cyc_Toc_temp_var(); goto _LL2677; _LL2677: { struct Cyc_Absyn_Stmt* _temp2678=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2676, 0),
0); goto _LL2679; _LL2679:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2676,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2593,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2678, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2556: Cyc_Toc_exp_to_c(
nv, _temp2599); Cyc_Toc_stmt_to_c( nv, _temp2597); s= _temp2595; continue;
_LL2558: Cyc_Toc_exp_to_c( nv, _temp2607); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2601); return; _LL2560: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2609 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2609))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2562: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -(( struct Cyc_Absyn_Stmt*) _check_null( _temp2611))->try_depth, s);
return; _LL2564: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2613))->try_depth, s); return; _LL2566: Cyc_Toc_exp_to_c( nv,
_temp2631); Cyc_Toc_exp_to_c( nv, _temp2629); Cyc_Toc_exp_to_c( nv, _temp2623);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2617); return; _LL2568: Cyc_Toc_xlate_switch(
nv, s, _temp2635, _temp2633); return; _LL2570: if( nv->fallthru_info == 0){(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2680=( unsigned char*)"fallthru in unexpected place"; struct
_tagged_string _temp2681; _temp2681.curr= _temp2680; _temp2681.base= _temp2680;
_temp2681.last_plus_one= _temp2680 + 29; _temp2681;})));}{ struct _tuple8
_temp2684; struct Cyc_Dict_Dict* _temp2685; struct Cyc_List_List* _temp2687;
struct _tagged_string* _temp2689; struct _tuple8* _temp2682=( struct _tuple8*)((
struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp2684=* _temp2682;
_LL2690: _temp2689= _temp2684.f1; goto _LL2688; _LL2688: _temp2687= _temp2684.f2;
goto _LL2686; _LL2686: _temp2685= _temp2684.f3; goto _LL2683; _LL2683: { struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2689, 0); Cyc_Toc_do_npop_before(
s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp2637)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp2691=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2687); goto _LL2692; _LL2692: { struct Cyc_List_List*
_temp2693=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2639); goto _LL2694; _LL2694: for( 0; _temp2691 != 0;( _temp2691=(( struct
Cyc_List_List*) _check_null( _temp2691))->tl, _temp2693=(( struct Cyc_List_List*)
_check_null( _temp2693))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp2693))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp2685,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp2691))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp2693))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}}} _LL2572:{ void* _temp2695=( void*) _temp2643->r; int
_temp2705; struct Cyc_Absyn_Exp* _temp2707; struct Cyc_Core_Opt* _temp2709;
struct Cyc_Core_Opt* _temp2711; struct Cyc_Absyn_Pat* _temp2713; struct Cyc_List_List*
_temp2715; struct Cyc_Absyn_Vardecl* _temp2717; _LL2697: if(*(( int*) _temp2695)
== Cyc_Absyn_Let_d){ _LL2714: _temp2713=(( struct Cyc_Absyn_Let_d_struct*)
_temp2695)->f1; goto _LL2712; _LL2712: _temp2711=(( struct Cyc_Absyn_Let_d_struct*)
_temp2695)->f2; goto _LL2710; _LL2710: _temp2709=(( struct Cyc_Absyn_Let_d_struct*)
_temp2695)->f3; goto _LL2708; _LL2708: _temp2707=(( struct Cyc_Absyn_Let_d_struct*)
_temp2695)->f4; goto _LL2706; _LL2706: _temp2705=(( struct Cyc_Absyn_Let_d_struct*)
_temp2695)->f5; goto _LL2698;} else{ goto _LL2699;} _LL2699: if(*(( int*)
_temp2695) == Cyc_Absyn_Letv_d){ _LL2716: _temp2715=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2695)->f1; goto _LL2700;} else{ goto _LL2701;} _LL2701: if(*(( int*)
_temp2695) == Cyc_Absyn_Var_d){ _LL2718: _temp2717=(( struct Cyc_Absyn_Var_d_struct*)
_temp2695)->f1; goto _LL2702;} else{ goto _LL2703;} _LL2703: goto _LL2704;
_LL2698:( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2713,
_temp2709, _temp2707, _temp2705, _temp2641))->r)); goto _LL2696; _LL2700: {
struct Cyc_List_List* _temp2719=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2715); goto _LL2720; _LL2720: if( _temp2719 == 0){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2721=(
unsigned char*)"empty Letv_d"; struct _tagged_string _temp2722; _temp2722.curr=
_temp2721; _temp2722.base= _temp2721; _temp2722.last_plus_one= _temp2721 + 13;
_temp2722;}));}( void*)( _temp2643->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2723=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2723[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2724; _temp2724.tag= Cyc_Absyn_Var_d;
_temp2724.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2719))->hd; _temp2724;}); _temp2723;}))); _temp2719=(( struct Cyc_List_List*)
_check_null( _temp2719))->tl; for( 0; _temp2719 != 0; _temp2719=(( struct Cyc_List_List*)
_check_null( _temp2719))->tl){ struct Cyc_Absyn_Decl* _temp2727= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2725=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2725[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2726; _temp2726.tag= Cyc_Absyn_Var_d; _temp2726.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2719))->hd; _temp2726;}); _temp2725;}),
0); goto _LL2728; _LL2728:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp2727, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c(
nv, s); goto _LL2696;} _LL2702: { struct _tuple0* _temp2729= _temp2717->name;
goto _LL2730; _LL2730:( void*)( _temp2717->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2717->type));{ struct Cyc_Toc_Env* _temp2733= Cyc_Toc_add_varmap( nv,
_temp2729, Cyc_Absyn_varb_exp( _temp2729,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2731=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2731[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2732; _temp2732.tag= Cyc_Absyn_Local_b;
_temp2732.f1= _temp2717; _temp2732;}); _temp2731;}), 0)); goto _LL2734; _LL2734:
Cyc_Toc_stmt_to_c( _temp2733, _temp2641); if( _temp2717->initializer != 0){
struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*) _check_null( _temp2717->initializer);
void* _temp2735=( void*) init->r; struct Cyc_Absyn_Exp* _temp2741; struct Cyc_Absyn_Exp*
_temp2743; struct Cyc_Absyn_Vardecl* _temp2745; _LL2737: if(*(( int*) _temp2735)
== Cyc_Absyn_Comprehension_e){ _LL2746: _temp2745=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2735)->f1; goto _LL2744; _LL2744: _temp2743=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2735)->f2; goto _LL2742; _LL2742: _temp2741=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2735)->f3; goto _LL2738;} else{ goto _LL2739;} _LL2739: goto _LL2740;
_LL2738: _temp2717->initializer= 0;( void*)( _temp2641->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2729, 0), _temp2745, _temp2743, _temp2741, Cyc_Absyn_new_stmt((
void*) _temp2641->r, 0), 0))->r)); goto _LL2736; _LL2740: Cyc_Toc_exp_to_c( nv,
init); goto _LL2736; _LL2736:;} goto _LL2696;}} _LL2704:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2747=( unsigned char*)"bad nested declaration within function";
struct _tagged_string _temp2748; _temp2748.curr= _temp2747; _temp2748.base=
_temp2747; _temp2748.last_plus_one= _temp2747 + 39; _temp2748;})); goto _LL2696;
_LL2696:;} return; _LL2574: s= _temp2645; continue; _LL2576: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp2655); Cyc_Toc_exp_to_c( nv, _temp2653); return;
_LL2578: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__push_handler_qv,
0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp( Cyc_Toc_setjmp_qv, 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__pop_handler_qv,
0); struct Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp( Cyc_Toc__exn_thrown_qv,
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2749=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2749->v=( void*) e_typ; _temp2749;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2659);{
struct Cyc_Absyn_Stmt* _temp2750= Cyc_Absyn_seq_stmt( _temp2659, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); goto _LL2751; _LL2751: {
struct _tuple0* _temp2752= Cyc_Toc_temp_var(); goto _LL2753; _LL2753: { struct
Cyc_Absyn_Exp* _temp2754= Cyc_Absyn_var_exp( _temp2752, 0); goto _LL2755;
_LL2755: { struct Cyc_Absyn_Vardecl* _temp2756= Cyc_Absyn_new_vardecl( _temp2752,
Cyc_Absyn_exn_typ, 0); goto _LL2757; _LL2757: _temp2754->topt=({ struct Cyc_Core_Opt*
_temp2758=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2758->v=( void*) Cyc_Absyn_exn_typ; _temp2758;});{ struct Cyc_Absyn_Pat*
_temp2763=({ struct Cyc_Absyn_Pat* _temp2759=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2759->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2761=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2761[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2762; _temp2762.tag= Cyc_Absyn_Var_p;
_temp2762.f1= _temp2756; _temp2762;}); _temp2761;})); _temp2759->topt=({ struct
Cyc_Core_Opt* _temp2760=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2760->v=( void*) Cyc_Absyn_exn_typ; _temp2760;}); _temp2759->loc= 0;
_temp2759;}); goto _LL2764; _LL2764: { struct Cyc_Absyn_Exp* _temp2765= Cyc_Absyn_throw_exp(
_temp2754, 0); goto _LL2766; _LL2766: _temp2765->topt=({ struct Cyc_Core_Opt*
_temp2767=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2767->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2767;});{ struct Cyc_Absyn_Stmt*
_temp2768= Cyc_Absyn_exp_stmt( _temp2765, 0); goto _LL2769; _LL2769: { struct
Cyc_Absyn_Switch_clause* _temp2773=({ struct Cyc_Absyn_Switch_clause* _temp2770=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2770->pattern= _temp2763; _temp2770->pat_vars=({ struct Cyc_Core_Opt*
_temp2771=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2771->v=( void*)({ struct Cyc_List_List* _temp2772=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2772->hd=( void*) _temp2756;
_temp2772->tl= 0; _temp2772;}); _temp2771;}); _temp2770->where_clause= 0;
_temp2770->body= _temp2768; _temp2770->loc= 0; _temp2770;}); goto _LL2774;
_LL2774: { struct Cyc_Absyn_Stmt* _temp2776= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2657,({ struct Cyc_List_List* _temp2775=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2775->hd=( void*) _temp2773; _temp2775->tl=
0; _temp2775;})), 0); goto _LL2777; _LL2777: Cyc_Toc_stmt_to_c( nv, _temp2776);{
struct Cyc_Absyn_Exp* _temp2779= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2778=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2778->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2778->tl= 0; _temp2778;}), 0); goto _LL2780; _LL2780: { struct Cyc_Absyn_Stmt*
_temp2782= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2781=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2781->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2781->tl= 0;
_temp2781;}), 0), 0); goto _LL2783; _LL2783: { struct Cyc_Absyn_Exp* _temp2784=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2785; _LL2785: {
struct Cyc_Absyn_Exp* _temp2786= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2787; _LL2787:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2782, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2784, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2779, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2786, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2750, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2776, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2580: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2663->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2793=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2793[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2794; _temp2794.tag= Cyc_Absyn_Local_b; _temp2794.f1=
_temp2663; _temp2794;}); _temp2793;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2792=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2792->f1= Cyc_Toc_abs_ns; _temp2792->f2= Cyc_Toc__push_region_sp;
_temp2792;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2791=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2791->f1= Cyc_Toc_abs_ns; _temp2791->f2= Cyc_Toc__pop_region_sp; _temp2791;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2790=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2790->f1=
Cyc_Toc_abs_ns; _temp2790->f2= Cyc_Toc__new_region_sp; _temp2790;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2789=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2789->hd=( void*) addr_rh_exp; _temp2789->tl= 0; _temp2789;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2788=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2788->hd=( void*) x_exp; _temp2788->tl= 0; _temp2788;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2661);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2661, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2582:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2795=(
unsigned char*)"switchC_s"; struct _tagged_string _temp2796; _temp2796.curr=
_temp2795; _temp2796.base= _temp2795; _temp2796.last_plus_one= _temp2795 + 10;
_temp2796;})); return; _LL2584:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2797=( unsigned char*)"cut"; struct _tagged_string _temp2798;
_temp2798.curr= _temp2797; _temp2798.base= _temp2797; _temp2798.last_plus_one=
_temp2797 + 4; _temp2798;})); return; _LL2586:(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(({ unsigned char* _temp2799=( unsigned char*)"splice"; struct
_tagged_string _temp2800; _temp2800.curr= _temp2799; _temp2800.base= _temp2799;
_temp2800.last_plus_one= _temp2799 + 7; _temp2800;})); return; _LL2546:;}}
struct _tuple17{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp2801= f->args; goto _LL2802;
_LL2802: for( 0; _temp2801 != 0; _temp2801=(( struct Cyc_List_List*) _check_null(
_temp2801))->tl){(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2801))->hd)).f3= Cyc_Toc_typ_to_c((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp2801))->hd)).f3);{ struct _tuple0* _temp2804=({ struct _tuple0*
_temp2803=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2803->f1=(
void*) Cyc_Absyn_Loc_n; _temp2803->f2=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp2801))->hd)).f1; _temp2803;}); goto _LL2805; _LL2805: nv= Cyc_Toc_add_varmap(
nv, _temp2804, Cyc_Absyn_var_exp( _temp2804, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2808; void* _temp2810; void* _temp2812; struct Cyc_Absyn_Tqual _temp2814;
struct Cyc_Core_Opt* _temp2816; struct Cyc_Absyn_VarargInfo _temp2806=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2817: _temp2816=
_temp2806.name; goto _LL2815; _LL2815: _temp2814= _temp2806.tq; goto _LL2813;
_LL2813: _temp2812=( void*) _temp2806.type; goto _LL2811; _LL2811: _temp2810=(
void*) _temp2806.rgn; goto _LL2809; _LL2809: _temp2808= _temp2806.inject; goto
_LL2807; _LL2807: { void* _temp2818= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2812, _temp2810, _temp2814)); goto _LL2819; _LL2819: { struct _tuple0*
_temp2821=({ struct _tuple0* _temp2820=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2820->f1=( void*) Cyc_Absyn_Loc_n; _temp2820->f2=( struct
_tagged_string*)(( struct Cyc_Core_Opt*) _check_null( _temp2816))->v; _temp2820;});
goto _LL2822; _LL2822: f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2823=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2823->hd=( void*)({ struct _tuple17* _temp2824=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp2824->f1=( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( _temp2816))->v; _temp2824->f2= _temp2814;
_temp2824->f3= _temp2818; _temp2824;}); _temp2823->tl= 0; _temp2823;})); nv= Cyc_Toc_add_varmap(
nv, _temp2821, Cyc_Absyn_var_exp( _temp2821, 0)); f->cyc_varargs= 0;}}}{ struct
Cyc_List_List* _temp2825=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; goto _LL2826; _LL2826: for( 0; _temp2825 !=
0; _temp2825=(( struct Cyc_List_List*) _check_null( _temp2825))->tl){( void*)(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp2825))->hd)->type=(
void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp2825))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2827= s;
_LL2829: if( _temp2827 ==( void*) Cyc_Absyn_Abstract){ goto _LL2830;} else{ goto
_LL2831;} _LL2831: if( _temp2827 ==( void*) Cyc_Absyn_ExternC){ goto _LL2832;}
else{ goto _LL2833;} _LL2833: goto _LL2834; _LL2830: return( void*) Cyc_Absyn_Public;
_LL2832: return( void*) Cyc_Absyn_Extern; _LL2834: return s; _LL2828:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2835=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2836; _LL2836: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2837=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2837->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2837;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2838=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2835).f2); goto _LL2839;
_LL2839: if( _temp2838 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2835).f2, s));}
else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null(
_temp2838))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2835).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2840=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2840->v=( void*)
_temp2835; _temp2840;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2841=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2842; _LL2842: for( 0; _temp2841 != 0;
_temp2841=(( struct Cyc_List_List*) _check_null( _temp2841))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2841))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2841))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2843=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2844; _LL2844: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2845=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2845->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2845;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2846=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2843).f2); goto _LL2847; _LL2847:
if( _temp2846 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2843).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2846))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2843).f2, u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)(
u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==(
void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*)
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2848=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2848->v=( void*) _temp2843; _temp2848;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2849=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( u->fields))->v; goto
_LL2850; _LL2850: for( 0; _temp2849 != 0; _temp2849=(( struct Cyc_List_List*)
_check_null( _temp2849))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2849))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2849))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2851=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2851->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2851;});}{ struct
_tuple0* _temp2852= tud->name; goto _LL2853; _LL2853: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2852)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2852));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2854=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2855; _LL2855: for( 0;
_temp2854 != 0; _temp2854=(( struct Cyc_List_List*) _check_null( _temp2854))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2854))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2856; _temp2856.q_const= 1; _temp2856.q_volatile= 0;
_temp2856.q_restrict= 0; _temp2856;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2857=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2857->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2858=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2858[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2859; _temp2859.tag= Cyc_Absyn_Var_d;
_temp2859.f1= vd; _temp2859;}); _temp2858;}), 0); _temp2857->tl= Cyc_Toc_result_decls;
_temp2857;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2860; _temp2860.q_const=
1; _temp2860.q_volatile= 0; _temp2860.q_restrict= 0; _temp2860;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2861=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2861->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2862=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2862[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2863; _temp2863.tag= Cyc_Absyn_Var_d; _temp2863.f1= vd; _temp2863;});
_temp2862;}), 0); _temp2861->tl= Cyc_Toc_result_decls; _temp2861;});{ struct Cyc_List_List*
_temp2864= 0; goto _LL2865; _LL2865: { int i= 1;{ struct Cyc_List_List*
_temp2866= f->typs; goto _LL2867; _LL2867: for( 0; _temp2866 != 0;( _temp2866=((
struct Cyc_List_List*) _check_null( _temp2866))->tl, i ++)){ struct
_tagged_string* _temp2868= Cyc_Toc_fieldname( i); goto _LL2869; _LL2869: {
struct Cyc_Absyn_Structfield* _temp2871=({ struct Cyc_Absyn_Structfield*
_temp2870=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2870->name= _temp2868; _temp2870->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2866))->hd)).f1; _temp2870->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2866))->hd)).f2);
_temp2870->width= 0; _temp2870->attributes= 0; _temp2870;}); goto _LL2872;
_LL2872: _temp2864=({ struct Cyc_List_List* _temp2873=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2873->hd=( void*) _temp2871;
_temp2873->tl= _temp2864; _temp2873;});}}} _temp2864=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2864); _temp2864=({ struct Cyc_List_List*
_temp2874=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2874->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2875=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2875->name= Cyc_Toc_tag_sp;
_temp2875->tq= Cyc_Toc_mt_tq; _temp2875->type=( void*) Cyc_Absyn_sint_t;
_temp2875->width= 0; _temp2875->attributes= 0; _temp2875;}); _temp2874->tl=
_temp2864; _temp2874;});{ struct Cyc_Absyn_Structdecl* _temp2881=({ struct Cyc_Absyn_Structdecl*
_temp2876=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2876->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2876->name=({ struct Cyc_Core_Opt*
_temp2878=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2878->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2879=( unsigned char*)"_struct"; struct _tagged_string _temp2880; _temp2880.curr=
_temp2879; _temp2880.base= _temp2879; _temp2880.last_plus_one= _temp2879 + 8;
_temp2880;})); _temp2878;}); _temp2876->tvs= 0; _temp2876->fields=({ struct Cyc_Core_Opt*
_temp2877=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2877->v=( void*) _temp2864; _temp2877;}); _temp2876->attributes= 0;
_temp2876;}); goto _LL2882; _LL2882: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2883=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2883->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2884=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2884[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2885; _temp2885.tag= Cyc_Absyn_Struct_d;
_temp2885.f1= _temp2881; _temp2885;}); _temp2884;}), 0); _temp2883->tl= Cyc_Toc_result_decls;
_temp2883;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2886=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2886->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2886;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2887= xd->name; goto _LL2888; _LL2888: {
struct Cyc_List_List* _temp2889=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2890; _LL2890: for( 0; _temp2889 != 0;
_temp2889=(( struct Cyc_List_List*) _check_null( _temp2889))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2889))->hd; struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2892= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2891=* fn;(
unsigned int)( _temp2891.last_plus_one - _temp2891.curr);}) + 4, 0); goto
_LL2893; _LL2893: { struct Cyc_Absyn_ArrayType_struct* _temp2896=({ struct Cyc_Absyn_ArrayType_struct*
_temp2894=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2894[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2895; _temp2895.tag= Cyc_Absyn_ArrayType;
_temp2895.f1=( void*) Cyc_Absyn_uchar_t; _temp2895.f2= Cyc_Toc_mt_tq; _temp2895.f3=(
struct Cyc_Absyn_Exp*) _temp2892; _temp2895;}); _temp2894;}); goto _LL2897;
_LL2897: { struct Cyc_Core_Opt* _temp2898=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2906; int _temp2907; _LL2900: if( _temp2898 == 0){ goto
_LL2901;} else{ goto _LL2902;} _LL2902: if( _temp2898 == 0){ goto _LL2904;}
else{ _temp2906=* _temp2898; _LL2908: _temp2907=( int) _temp2906.v; if(
_temp2907 == 0){ goto _LL2903;} else{ goto _LL2904;}} _LL2904: goto _LL2905;
_LL2901: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_string)({
int _temp2909= 0; int _temp2910= 0; int _temp2911= 0; int _temp2912= 0; struct
_tagged_string _temp2913=* fn; xprintf("%c%c%c%c%.*s", _temp2909, _temp2910,
_temp2911, _temp2912, _temp2913.last_plus_one - _temp2913.curr, _temp2913.curr);}),
0);}{ struct Cyc_Absyn_Vardecl* _temp2914= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2896, initopt); goto _LL2915; _LL2915:( void*)( _temp2914->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2916=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2916->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2917=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2917[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2918; _temp2918.tag= Cyc_Absyn_Var_d;
_temp2918.f1= _temp2914; _temp2918;}); _temp2917;}), 0); _temp2916->tl= Cyc_Toc_result_decls;
_temp2916;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2919= f->typs; goto _LL2920; _LL2920: for( 0;
_temp2919 != 0;( _temp2919=(( struct Cyc_List_List*) _check_null( _temp2919))->tl,
i ++)){ struct _tagged_string* _temp2922=({ struct _tagged_string* _temp2921=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp2921[ 0]=(
struct _tagged_string) xprintf("f%d", i); _temp2921;}); goto _LL2923; _LL2923: {
struct Cyc_Absyn_Structfield* _temp2925=({ struct Cyc_Absyn_Structfield*
_temp2924=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2924->name= _temp2922; _temp2924->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2919))->hd)).f1; _temp2924->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2919))->hd)).f2);
_temp2924->width= 0; _temp2924->attributes= 0; _temp2924;}); goto _LL2926;
_LL2926: fields=({ struct Cyc_List_List* _temp2927=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2927->hd=( void*) _temp2925;
_temp2927->tl= fields; _temp2927;});}}} fields=({ struct Cyc_List_List*
_temp2928=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2928->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2929=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2929->name= Cyc_Toc_tag_sp;
_temp2929->tq= Cyc_Toc_mt_tq; _temp2929->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2929->width= 0; _temp2929->attributes= 0; _temp2929;});
_temp2928->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2928;});{ struct Cyc_Absyn_Structdecl* _temp2935=({ struct Cyc_Absyn_Structdecl*
_temp2930=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2930->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2930->name=({ struct Cyc_Core_Opt*
_temp2932=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2932->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2933=( unsigned char*)"_struct"; struct _tagged_string _temp2934; _temp2934.curr=
_temp2933; _temp2934.base= _temp2933; _temp2934.last_plus_one= _temp2933 + 8;
_temp2934;})); _temp2932;}); _temp2930->tvs= 0; _temp2930->fields=({ struct Cyc_Core_Opt*
_temp2931=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2931->v=( void*) fields; _temp2931;}); _temp2930->attributes= 0; _temp2930;});
goto _LL2936; _LL2936: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2937=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2937->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2938=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2938[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2939; _temp2939.tag= Cyc_Absyn_Struct_d;
_temp2939.f1= _temp2935; _temp2939;}); _temp2938;}), 0); _temp2937->tl= Cyc_Toc_result_decls;
_temp2937;});}} goto _LL2899;}} _LL2903: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2945= Cyc_Absyn_string_exp(( struct _tagged_string)({
int _temp2940= 0; int _temp2941= 0; int _temp2942= 0; int _temp2943= 0; struct
_tagged_string _temp2944=* fn; xprintf("%c%c%c%c%.*s", _temp2940, _temp2941,
_temp2942, _temp2943, _temp2944.last_plus_one - _temp2944.curr, _temp2944.curr);}),
0); goto _LL2946; _LL2946: { struct Cyc_Absyn_Vardecl* _temp2947= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2896,( struct Cyc_Absyn_Exp*) _temp2945); goto _LL2948;
_LL2948:( void*)( _temp2947->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2949=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2949->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2950=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2950[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2951; _temp2951.tag= Cyc_Absyn_Var_d; _temp2951.f1= _temp2947; _temp2951;});
_temp2950;}), 0); _temp2949->tl= Cyc_Toc_result_decls; _temp2949;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2899; _LL2905: goto
_LL2899; _LL2899:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2952= ed->name; goto
_LL2953; _LL2953:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2954=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2955; _LL2955: for( 0; _temp2954 != 0;
_temp2954=(( struct Cyc_List_List*) _check_null( _temp2954))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2954))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2954))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2954))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct _RegionHandle
_temp2956= _new_region(); struct _RegionHandle* prgn=& _temp2956; _push_region(
prgn);{ struct Cyc_Absyn_Stmt* _temp2959; struct Cyc_List_List* _temp2961;
struct Cyc_Toc_Env* _temp2963; struct _tuple13 _temp2957= Cyc_Toc_xlate_pat( nv,
prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2964: _temp2963= _temp2957.f1; goto _LL2962; _LL2962: _temp2961=
_temp2957.f2; goto _LL2960; _LL2960: _temp2959= _temp2957.f3; goto _LL2958;
_LL2958: Cyc_Toc_stmt_to_c( _temp2963, s);{ struct Cyc_Absyn_Stmt* _temp2965=
Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL2966; _LL2966: if( exhaust){ s=
Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2959, _temp2965, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2968= Cyc_Toc_newthrow_exp(
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2967=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2967->f1= Cyc_Toc_abs_ns; _temp2967->f2= Cyc_Toc_Match_Exception_sp;
_temp2967;}), 0)); goto _LL2969; _LL2969: { struct Cyc_Absyn_Stmt* _temp2970=
Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2968, 0), 0); goto
_LL2971; _LL2971: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2959, Cyc_Absyn_seq_stmt( _temp2970,
_temp2965, 0), 0), 0);}} for( 0; _temp2961 != 0; _temp2961=(( struct Cyc_List_List*)
_check_null( _temp2961))->tl){ struct _tuple14 _temp2974; void* _temp2975;
struct _tuple0* _temp2977; struct _tuple14* _temp2972=( struct _tuple14*)((
struct Cyc_List_List*) _check_null( _temp2961))->hd; _temp2974=* _temp2972;
_LL2978: _temp2977= _temp2974.f1; goto _LL2976; _LL2976: _temp2975= _temp2974.f2;
goto _LL2973; _LL2973: s= Cyc_Absyn_declare_stmt( _temp2977, _temp2975, 0, s, 0);}}};
_pop_region(& _temp2956);} return s;}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=((
struct Cyc_List_List*) _check_null( ds))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2979=(
void*) d->r; struct Cyc_Absyn_Vardecl* _temp3005; struct Cyc_Absyn_Fndecl*
_temp3007; int _temp3009; struct Cyc_Absyn_Exp* _temp3011; struct Cyc_Core_Opt*
_temp3013; struct Cyc_Core_Opt* _temp3015; struct Cyc_Absyn_Pat* _temp3017;
struct Cyc_List_List* _temp3019; struct Cyc_Absyn_Structdecl* _temp3021; struct
Cyc_Absyn_Uniondecl* _temp3023; struct Cyc_Absyn_Tuniondecl* _temp3025; struct
Cyc_Absyn_Enumdecl* _temp3027; struct Cyc_Absyn_Typedefdecl* _temp3029; struct
Cyc_List_List* _temp3031; struct _tagged_string* _temp3033; struct Cyc_List_List*
_temp3035; struct _tuple0* _temp3037; struct Cyc_List_List* _temp3039; _LL2981:
if(*(( int*) _temp2979) == Cyc_Absyn_Var_d){ _LL3006: _temp3005=(( struct Cyc_Absyn_Var_d_struct*)
_temp2979)->f1; goto _LL2982;} else{ goto _LL2983;} _LL2983: if(*(( int*)
_temp2979) == Cyc_Absyn_Fn_d){ _LL3008: _temp3007=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2979)->f1; goto _LL2984;} else{ goto _LL2985;} _LL2985: if(*(( int*)
_temp2979) == Cyc_Absyn_Let_d){ _LL3018: _temp3017=(( struct Cyc_Absyn_Let_d_struct*)
_temp2979)->f1; goto _LL3016; _LL3016: _temp3015=(( struct Cyc_Absyn_Let_d_struct*)
_temp2979)->f2; goto _LL3014; _LL3014: _temp3013=(( struct Cyc_Absyn_Let_d_struct*)
_temp2979)->f3; goto _LL3012; _LL3012: _temp3011=(( struct Cyc_Absyn_Let_d_struct*)
_temp2979)->f4; goto _LL3010; _LL3010: _temp3009=(( struct Cyc_Absyn_Let_d_struct*)
_temp2979)->f5; goto _LL2986;} else{ goto _LL2987;} _LL2987: if(*(( int*)
_temp2979) == Cyc_Absyn_Letv_d){ _LL3020: _temp3019=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2979)->f1; goto _LL2988;} else{ goto _LL2989;} _LL2989: if(*(( int*)
_temp2979) == Cyc_Absyn_Struct_d){ _LL3022: _temp3021=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2979)->f1; goto _LL2990;} else{ goto _LL2991;} _LL2991: if(*(( int*)
_temp2979) == Cyc_Absyn_Union_d){ _LL3024: _temp3023=(( struct Cyc_Absyn_Union_d_struct*)
_temp2979)->f1; goto _LL2992;} else{ goto _LL2993;} _LL2993: if(*(( int*)
_temp2979) == Cyc_Absyn_Tunion_d){ _LL3026: _temp3025=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2979)->f1; goto _LL2994;} else{ goto _LL2995;} _LL2995: if(*(( int*)
_temp2979) == Cyc_Absyn_Enum_d){ _LL3028: _temp3027=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2979)->f1; goto _LL2996;} else{ goto _LL2997;} _LL2997: if(*(( int*)
_temp2979) == Cyc_Absyn_Typedef_d){ _LL3030: _temp3029=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2979)->f1; goto _LL2998;} else{ goto _LL2999;} _LL2999: if(*(( int*)
_temp2979) == Cyc_Absyn_Namespace_d){ _LL3034: _temp3033=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2979)->f1; goto _LL3032; _LL3032: _temp3031=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2979)->f2; goto _LL3000;} else{ goto _LL3001;} _LL3001: if(*(( int*)
_temp2979) == Cyc_Absyn_Using_d){ _LL3038: _temp3037=(( struct Cyc_Absyn_Using_d_struct*)
_temp2979)->f1; goto _LL3036; _LL3036: _temp3035=(( struct Cyc_Absyn_Using_d_struct*)
_temp2979)->f2; goto _LL3002;} else{ goto _LL3003;} _LL3003: if(*(( int*)
_temp2979) == Cyc_Absyn_ExternC_d){ _LL3040: _temp3039=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2979)->f1; goto _LL3004;} else{ goto _LL2980;} _LL2982: { struct _tuple0*
_temp3041= _temp3005->name; goto _LL3042; _LL3042: if(( void*) _temp3005->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3041=({ struct _tuple0* _temp3043=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3043->f1= Cyc_Toc_abs_ns;
_temp3043->f2=(* _temp3041).f2; _temp3043;});} if( _temp3005->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null( _temp3005->initializer));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp3044=( unsigned char*)"decls_to_c: not at toplevel!"; struct
_tagged_string _temp3045; _temp3045.curr= _temp3044; _temp3045.base= _temp3044;
_temp3045.last_plus_one= _temp3044 + 29; _temp3045;}));} nv= Cyc_Toc_add_varmap(
nv, _temp3005->name, Cyc_Absyn_varb_exp( _temp3041,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp3046=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp3046[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp3047; _temp3047.tag= Cyc_Absyn_Global_b;
_temp3047.f1= _temp3005; _temp3047;}); _temp3046;}), 0)); _temp3005->name=
_temp3041;( void*)( _temp3005->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp3005->sc));(
void*)( _temp3005->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp3005->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3048=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3048->hd=( void*) d; _temp3048->tl=
Cyc_Toc_result_decls; _temp3048;}); goto _LL2980;} _LL2984: { struct _tuple0*
_temp3049= _temp3007->name; goto _LL3050; _LL3050: if(( void*) _temp3007->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3049=({ struct _tuple0* _temp3051=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3051->f1= Cyc_Toc_abs_ns;
_temp3051->f2=(* _temp3049).f2; _temp3051;});} nv= Cyc_Toc_add_varmap( nv,
_temp3007->name, Cyc_Absyn_var_exp( _temp3049, 0)); _temp3007->name= _temp3049;
Cyc_Toc_fndecl_to_c( nv, _temp3007); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3052=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3052->hd=( void*) d; _temp3052->tl= Cyc_Toc_result_decls; _temp3052;});
goto _LL2980;} _LL2986:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp3053=( unsigned char*)"letdecl"; struct _tagged_string
_temp3054; _temp3054.curr= _temp3053; _temp3054.base= _temp3053; _temp3054.last_plus_one=
_temp3053 + 8; _temp3054;})); goto _LL2980; _LL2988:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp3055=(
unsigned char*)"letdecl"; struct _tagged_string _temp3056; _temp3056.curr=
_temp3055; _temp3056.base= _temp3055; _temp3056.last_plus_one= _temp3055 + 8;
_temp3056;})); goto _LL2980; _LL2990: Cyc_Toc_structdecl_to_c( _temp3021); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3057=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3057->hd=( void*) d; _temp3057->tl= Cyc_Toc_result_decls;
_temp3057;}); goto _LL2980; _LL2992: Cyc_Toc_uniondecl_to_c( _temp3023); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3058=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3058->hd=( void*) d; _temp3058->tl= Cyc_Toc_result_decls;
_temp3058;}); goto _LL2980; _LL2994: if( _temp3025->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp3025);} else{ Cyc_Toc_tuniondecl_to_c( _temp3025);} goto _LL2980; _LL2996:
Cyc_Toc_enumdecl_to_c( nv, _temp3027); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3059=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3059->hd=( void*) d; _temp3059->tl= Cyc_Toc_result_decls; _temp3059;});
goto _LL2980; _LL2998: _temp3029->name= _temp3029->name; _temp3029->tvs= 0;(
void*)( _temp3029->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp3029->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3060=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3060->hd=( void*) d; _temp3060->tl=
Cyc_Toc_result_decls; _temp3060;}); goto _LL2980; _LL3000: _temp3035= _temp3031;
goto _LL3002; _LL3002: _temp3039= _temp3035; goto _LL3004; _LL3004: nv= Cyc_Toc_decls_to_c(
nv, _temp3039, top); goto _LL2980; _LL2980:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}