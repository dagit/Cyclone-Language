 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
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
static const int Cyc_Absyn_Offsetof_e= 18; struct Cyc_Absyn_Offsetof_e_struct{
int tag; void* f1; struct _tagged_string* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; static const
int Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e=
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
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp(
struct _tuple0*, struct _tuple0*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_bounds_one; extern void*
Cyc_Absyn_star_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void*
Cyc_Absyn_cstar_typ( void* t, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ();
extern void* Cyc_Absyn_strct( struct _tagged_string* name); extern void* Cyc_Absyn_strctq(
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
extern struct Cyc_Absyn_Exp* Cyc_Absyn_match_exn_exp( struct Cyc_Position_Segment*);
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
Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1;
int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1;
int rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_string Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0*); extern struct _tagged_string Cyc_Absynpp_prim2string( void* p);
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void* Cyc_Tcutil_typ_kind( void* t); extern void*
Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify( void*, void*);
extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*);
struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern void* Cyc_Tcutil_snd_tqt(
struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_string s, int i); extern struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds); extern struct _tuple0* Cyc_Toc_temp_var(); int Cyc_Toc_use_runtimecheck_funs=
1; static struct Cyc_List_List* Cyc_Toc_result_decls= 0; struct Cyc_Absyn_Rel_n_struct
Cyc_Toc_rel_ns_value={ 0u, 0}; void* Cyc_Toc_rel_ns=( void*)& Cyc_Toc_rel_ns_value;
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
Cyc_Toc_handler_string; static unsigned char _temp35[ 14u]="_RegionHandle";
static struct _tagged_string Cyc_Toc__RegionHandle_string={ _temp35, _temp35,
_temp35 + 14u}; static struct _tagged_string* Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static unsigned char _temp38[ 12u]="_new_region"; static struct _tagged_string
Cyc_Toc__new_region_string={ _temp38, _temp38, _temp38 + 12u}; static struct
_tagged_string* Cyc_Toc__new_region_sp=& Cyc_Toc__new_region_string; static
unsigned char _temp41[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string={
_temp41, _temp41, _temp41 + 7u}; static struct _tuple0 Cyc_Toc__throw_pr={( void*)&
Cyc_Toc_rel_ns_value,& Cyc_Toc__throw_string}; static struct _tuple0* Cyc_Toc__throw_qv=&
Cyc_Toc__throw_pr; static unsigned char _temp44[ 7u]="setjmp"; static struct
_tagged_string Cyc_Toc_setjmp_string={ _temp44, _temp44, _temp44 + 7u}; static
struct _tuple0 Cyc_Toc_setjmp_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc_setjmp_string};
static struct _tuple0* Cyc_Toc_setjmp_qv=& Cyc_Toc_setjmp_pr; static
unsigned char _temp47[ 14u]="_push_handler"; static struct _tagged_string Cyc_Toc__push_handler_string={
_temp47, _temp47, _temp47 + 14u}; static struct _tuple0 Cyc_Toc__push_handler_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__push_handler_string}; static struct
_tuple0* Cyc_Toc__push_handler_qv=& Cyc_Toc__push_handler_pr; static
unsigned char _temp50[ 13u]="_pop_handler"; static struct _tagged_string Cyc_Toc__pop_handler_string={
_temp50, _temp50, _temp50 + 13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__pop_handler_string}; static struct
_tuple0* Cyc_Toc__pop_handler_qv=& Cyc_Toc__pop_handler_pr; static unsigned char
_temp53[ 12u]="_exn_thrown"; static struct _tagged_string Cyc_Toc__exn_thrown_string={
_temp53, _temp53, _temp53 + 12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__exn_thrown_string}; static struct
_tuple0* Cyc_Toc__exn_thrown_qv=& Cyc_Toc__exn_thrown_pr; static unsigned char
_temp56[ 14u]="_npop_handler"; static struct _tagged_string Cyc_Toc__npop_handler_string={
_temp56, _temp56, _temp56 + 14u}; static struct _tuple0 Cyc_Toc__npop_handler_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__npop_handler_string}; static struct
_tuple0* Cyc_Toc__npop_handler_qv=& Cyc_Toc__npop_handler_pr; static
unsigned char _temp59[ 12u]="_check_null"; static struct _tagged_string Cyc_Toc__check_null_string={
_temp59, _temp59, _temp59 + 12u}; static struct _tuple0 Cyc_Toc__check_null_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__check_null_string}; static struct
_tuple0* Cyc_Toc__check_null_qv=& Cyc_Toc__check_null_pr; static unsigned char
_temp62[ 25u]="_check_unknown_subscript"; static struct _tagged_string Cyc_Toc__check_unknown_subscript_string={
_temp62, _temp62, _temp62 + 25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__check_unknown_subscript_string}; static
struct _tuple0* Cyc_Toc__check_unknown_subscript_qv=& Cyc_Toc__check_unknown_subscript_pr;
static unsigned char _temp65[ 28u]="_check_known_subscript_null"; static struct
_tagged_string Cyc_Toc__check_known_subscript_null_string={ _temp65, _temp65,
_temp65 + 28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__check_known_subscript_null_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_null_qv=& Cyc_Toc__check_known_subscript_null_pr;
static unsigned char _temp68[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_string Cyc_Toc__check_known_subscript_notnull_string={ _temp68,
_temp68, _temp68 + 31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__check_known_subscript_notnull_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_notnull_qv=& Cyc_Toc__check_known_subscript_notnull_pr;
static unsigned char _temp71[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_GC_malloc_string={
_temp71, _temp71, _temp71 + 10u}; static struct _tuple0 Cyc_Toc_GC_malloc_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc_GC_malloc_string}; static struct _tuple0*
Cyc_Toc_GC_malloc_qv=& Cyc_Toc_GC_malloc_pr; static unsigned char _temp74[ 17u]="GC_malloc_atomic";
static struct _tagged_string Cyc_Toc_GC_malloc_atomic_string={ _temp74, _temp74,
_temp74 + 17u}; static struct _tuple0 Cyc_Toc_GC_malloc_atomic_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc_GC_malloc_atomic_string}; static struct _tuple0* Cyc_Toc_GC_malloc_atomic_qv=&
Cyc_Toc_GC_malloc_atomic_pr; static unsigned char _temp77[ 15u]="_region_malloc";
static struct _tagged_string Cyc_Toc__region_malloc_string={ _temp77, _temp77,
_temp77 + 15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__region_malloc_string}; static struct _tuple0* Cyc_Toc__region_malloc_qv=&
Cyc_Toc__region_malloc_pr; struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp78=(
void*) e->r; void* _temp104; unsigned char _temp106; void* _temp108; void*
_temp110; short _temp112; void* _temp114; void* _temp116; int _temp118; void*
_temp120; void* _temp122; long long _temp124; void* _temp126; void* _temp128;
struct Cyc_Absyn_Exp* _temp130; void* _temp132; struct Cyc_List_List* _temp134;
struct Cyc_List_List* _temp136; struct Cyc_Absyn_Structdecl* _temp138; struct
Cyc_List_List* _temp140; struct Cyc_Core_Opt* _temp142; struct _tuple0* _temp144;
struct Cyc_List_List* _temp146; struct _tuple1* _temp148; struct Cyc_List_List*
_temp150; struct Cyc_Core_Opt* _temp152; _LL80: if(*(( int*) _temp78) == Cyc_Absyn_Const_e){
_LL105: _temp104=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp78)->f1; if((
unsigned int) _temp104 > 1u?*(( int*) _temp104) == Cyc_Absyn_Char_c: 0){ _LL109:
_temp108=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp104)->f1; goto _LL107;
_LL107: _temp106=(( struct Cyc_Absyn_Char_c_struct*) _temp104)->f2; goto _LL81;}
else{ goto _LL82;}} else{ goto _LL82;} _LL82: if(*(( int*) _temp78) == Cyc_Absyn_Const_e){
_LL111: _temp110=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp78)->f1; if((
unsigned int) _temp110 > 1u?*(( int*) _temp110) == Cyc_Absyn_Short_c: 0){ _LL115:
_temp114=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp110)->f1; goto _LL113;
_LL113: _temp112=(( struct Cyc_Absyn_Short_c_struct*) _temp110)->f2; goto _LL83;}
else{ goto _LL84;}} else{ goto _LL84;} _LL84: if(*(( int*) _temp78) == Cyc_Absyn_Const_e){
_LL117: _temp116=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp78)->f1; if((
unsigned int) _temp116 > 1u?*(( int*) _temp116) == Cyc_Absyn_Int_c: 0){ _LL121:
_temp120=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp116)->f1; goto _LL119;
_LL119: _temp118=(( struct Cyc_Absyn_Int_c_struct*) _temp116)->f2; goto _LL85;}
else{ goto _LL86;}} else{ goto _LL86;} _LL86: if(*(( int*) _temp78) == Cyc_Absyn_Const_e){
_LL123: _temp122=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp78)->f1; if((
unsigned int) _temp122 > 1u?*(( int*) _temp122) == Cyc_Absyn_LongLong_c: 0){
_LL127: _temp126=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp122)->f1;
goto _LL125; _LL125: _temp124=(( struct Cyc_Absyn_LongLong_c_struct*) _temp122)->f2;
goto _LL87;} else{ goto _LL88;}} else{ goto _LL88;} _LL88: if(*(( int*) _temp78)
== Cyc_Absyn_Const_e){ _LL129: _temp128=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp78)->f1; if( _temp128 ==( void*) Cyc_Absyn_Null_c){ goto _LL89;} else{ goto
_LL90;}} else{ goto _LL90;} _LL90: if(*(( int*) _temp78) == Cyc_Absyn_Cast_e){
_LL133: _temp132=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp78)->f1; goto
_LL131; _LL131: _temp130=(( struct Cyc_Absyn_Cast_e_struct*) _temp78)->f2; goto
_LL91;} else{ goto _LL92;} _LL92: if(*(( int*) _temp78) == Cyc_Absyn_Tuple_e){
_LL135: _temp134=(( struct Cyc_Absyn_Tuple_e_struct*) _temp78)->f1; goto _LL93;}
else{ goto _LL94;} _LL94: if(*(( int*) _temp78) == Cyc_Absyn_Array_e){ _LL137:
_temp136=(( struct Cyc_Absyn_Array_e_struct*) _temp78)->f1; goto _LL95;} else{
goto _LL96;} _LL96: if(*(( int*) _temp78) == Cyc_Absyn_Struct_e){ _LL145:
_temp144=(( struct Cyc_Absyn_Struct_e_struct*) _temp78)->f1; goto _LL143; _LL143:
_temp142=(( struct Cyc_Absyn_Struct_e_struct*) _temp78)->f2; goto _LL141; _LL141:
_temp140=(( struct Cyc_Absyn_Struct_e_struct*) _temp78)->f3; goto _LL139; _LL139:
_temp138=(( struct Cyc_Absyn_Struct_e_struct*) _temp78)->f4; goto _LL97;} else{
goto _LL98;} _LL98: if(*(( int*) _temp78) == Cyc_Absyn_CompoundLit_e){ _LL149:
_temp148=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp78)->f1; goto _LL147;
_LL147: _temp146=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp78)->f2; goto
_LL99;} else{ goto _LL100;} _LL100: if(*(( int*) _temp78) == Cyc_Absyn_UnresolvedMem_e){
_LL153: _temp152=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp78)->f1; goto
_LL151; _LL151: _temp150=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp78)->f2;
goto _LL101;} else{ goto _LL102;} _LL102: goto _LL103; _LL81: return _temp106 =='\000';
_LL83: return _temp112 == 0; _LL85: return _temp118 == 0; _LL87: return _temp124
== 0; _LL89: return 1; _LL91: return Cyc_Toc_is_zero( _temp130); _LL93: return((
int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp134); _LL95: _temp140= _temp136; goto _LL97; _LL97:
_temp146= _temp140; goto _LL99; _LL99: _temp150= _temp146; goto _LL101; _LL101:
for( 0; _temp150 != 0; _temp150=(( struct Cyc_List_List*) _check_null( _temp150))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp150))->hd)).f2)){ return 0;}} return 1; _LL103: return 0; _LL79:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp154= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp160; struct Cyc_Absyn_Conref* _temp162; struct Cyc_Absyn_Tqual
_temp164; struct Cyc_Absyn_Conref* _temp166; void* _temp168; void* _temp170;
_LL156: if(( unsigned int) _temp154 > 4u?*(( int*) _temp154) == Cyc_Absyn_PointerType:
0){ _LL161: _temp160=(( struct Cyc_Absyn_PointerType_struct*) _temp154)->f1;
_LL171: _temp170=( void*) _temp160.elt_typ; goto _LL169; _LL169: _temp168=( void*)
_temp160.rgn_typ; goto _LL167; _LL167: _temp166= _temp160.nullable; goto _LL165;
_LL165: _temp164= _temp160.tq; goto _LL163; _LL163: _temp162= _temp160.bounds;
goto _LL157;} else{ goto _LL158;} _LL158: goto _LL159; _LL157: return(( int(*)(
int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp166); _LL159:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp172=(
unsigned char*)"is_nullable"; struct _tagged_string _temp173; _temp173.curr=
_temp172; _temp173.base= _temp172; _temp173.last_plus_one= _temp172 + 12;
_temp173;})); return 0; _LL155:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag(
struct _tuple0* x, struct _tagged_string tag){ return({ struct _tuple0* _temp174=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp174->f1=(* x).f1;
_temp174->f2=({ struct _tagged_string* _temp175=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp175[ 0]=( struct
_tagged_string) Cyc_String_strconcat(*(* x).f2, tag); _temp175;}); _temp174;});}
static void Cyc_Toc_add_tuple_decl( struct _tagged_string* x, struct Cyc_List_List*
ts){ struct Cyc_List_List* _temp176= 0; goto _LL177; _LL177:{ int i= 1; for( 0;
ts != 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, i ++)){ _temp176=({
struct Cyc_List_List* _temp178=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp178->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp179=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp179->name=({ struct _tagged_string* _temp180=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp180[ 0]=( struct
_tagged_string) xprintf("f%d", i); _temp180;}); _temp179->tq= Cyc_Toc_mt_tq;
_temp179->type=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp179->width= 0; _temp179->attributes= 0; _temp179;}); _temp178->tl= _temp176;
_temp178;});}} _temp176=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp176);{ struct Cyc_Absyn_Structdecl* _temp185=({ struct
Cyc_Absyn_Structdecl* _temp181=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp181->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp181->name=({ struct Cyc_Core_Opt* _temp183=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp183->v=( void*)({ struct _tuple0*
_temp184=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp184->f1=
Cyc_Toc_rel_ns; _temp184->f2= x; _temp184;}); _temp183;}); _temp181->tvs= 0;
_temp181->fields=({ struct Cyc_Core_Opt* _temp182=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp182->v=( void*) _temp176;
_temp182;}); _temp181->attributes= 0; _temp181;}); goto _LL186; _LL186: Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp187=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp187->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp188=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp188[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp189; _temp189.tag= Cyc_Absyn_Struct_d; _temp189.f1=
_temp185; _temp189;}); _temp188;}), 0); _temp187->tl= Cyc_Toc_result_decls;
_temp187;});}} struct _tagged_ptr2{ struct Cyc_Absyn_Structfield** curr; struct
Cyc_Absyn_Structfield** base; struct Cyc_Absyn_Structfield** last_plus_one; } ;
static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x, void* t){
void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Structfield* _temp191=({ struct Cyc_Absyn_Structfield* _temp190=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp190->name= Cyc_Toc_curr_sp; _temp190->tq= Cyc_Toc_mt_tq; _temp190->type=(
void*) tptr; _temp190->width= 0; _temp190->attributes= 0; _temp190;}); goto
_LL192; _LL192: { struct Cyc_Absyn_Structfield* _temp194=({ struct Cyc_Absyn_Structfield*
_temp193=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp193->name= Cyc_Toc_base_sp; _temp193->tq= Cyc_Toc_mt_tq; _temp193->type=(
void*) tptr; _temp193->width= 0; _temp193->attributes= 0; _temp193;}); goto
_LL195; _LL195: { struct Cyc_Absyn_Structfield* _temp197=({ struct Cyc_Absyn_Structfield*
_temp196=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp196->name= Cyc_Toc_last_plus_one_sp; _temp196->tq= Cyc_Toc_mt_tq; _temp196->type=(
void*) tptr; _temp196->width= 0; _temp196->attributes= 0; _temp196;}); goto
_LL198; _LL198: { struct Cyc_List_List* _temp205=({ struct Cyc_List_List*(*
_temp199)( struct _tagged_ptr2)=( struct Cyc_List_List*(*)( struct _tagged_ptr2))
Cyc_List_list; struct Cyc_Absyn_Structfield* _temp202= _temp191; struct Cyc_Absyn_Structfield*
_temp203= _temp194; struct Cyc_Absyn_Structfield* _temp204= _temp197; struct Cyc_Absyn_Structfield*
_temp200[ 3u]={ _temp202, _temp203, _temp204}; struct _tagged_ptr2 _temp201={
_temp200, _temp200, _temp200 + 3u}; _temp199( _temp201);}); goto _LL206; _LL206: {
struct Cyc_Absyn_Structdecl* _temp211=({ struct Cyc_Absyn_Structdecl* _temp207=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp207->sc=( void*)(( void*) Cyc_Absyn_Public); _temp207->name=({ struct Cyc_Core_Opt*
_temp209=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp209->v=( void*)({ struct _tuple0* _temp210=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp210->f1= Cyc_Toc_rel_ns; _temp210->f2= x;
_temp210;}); _temp209;}); _temp207->tvs= 0; _temp207->fields=({ struct Cyc_Core_Opt*
_temp208=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp208->v=( void*)(( struct Cyc_List_List*) _temp205); _temp208;}); _temp207->attributes=
0; _temp207;}); goto _LL212; _LL212: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp213=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp213->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp214=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp214[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp215; _temp215.tag= Cyc_Absyn_Struct_d;
_temp215.f1= _temp211; _temp215;}); _temp214;}), 0); _temp213->tl= Cyc_Toc_result_decls;
_temp213;});}}}}} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static
int Cyc_Toc_tuple_type_counter= 0; struct _tuple5{ struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static struct _tagged_string* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp216= Cyc_Toc_tuple_types;
goto _LL217; _LL217: for( 0; _temp216 != 0; _temp216=(( struct Cyc_List_List*)
_check_null( _temp216))->tl){ struct _tuple5 _temp220; struct Cyc_List_List*
_temp221; struct _tagged_string* _temp223; struct _tuple5* _temp218=( struct
_tuple5*)(( struct Cyc_List_List*) _check_null( _temp216))->hd; _temp220=*
_temp218; _LL224: _temp223= _temp220.f1; goto _LL222; _LL222: _temp221= _temp220.f2;
goto _LL219; _LL219: { int okay= 1; struct Cyc_List_List* _temp225= tqs0; goto
_LL226; _LL226: for( 0; _temp225 != 0? _temp221 != 0: 0;( _temp225=(( struct Cyc_List_List*)
_check_null( _temp225))->tl, _temp221=(( struct Cyc_List_List*) _check_null(
_temp221))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp225))->hd)).f2,( void*)(( struct Cyc_List_List*) _check_null(
_temp221))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp225 != 0)? 1: _temp221
!= 0){ continue;} return _temp223;}}}{ struct _tagged_string* x=({ struct
_tagged_string* _temp229=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp229[ 0]=( struct _tagged_string) xprintf("_tuple%d",
Cyc_Toc_tuple_type_counter ++); _temp229;}); struct Cyc_List_List* ts=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_snd_tqt, tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({
struct Cyc_List_List* _temp227=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp227->hd=( void*)({ struct _tuple5* _temp228=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp228->f1= x; _temp228->f2=
ts; _temp228;}); _temp227->tl= Cyc_Toc_tuple_types; _temp227;}); return x;}}
static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter=
0; static void* Cyc_Toc_tagged_string_type_v=( void*) 0u; static void* Cyc_Toc_tagged_string_type(){
if( Cyc_Toc_tagged_string_type_v ==( void*) Cyc_Absyn_VoidType){ Cyc_Toc_tagged_string_type_v=
Cyc_Absyn_strct( Cyc_Toc__tagged_string_sp);} return Cyc_Toc_tagged_string_type_v;}
struct _tuple6{ struct _tagged_string* f1; void* f2; } ; static void* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual tq){{ void* _temp230= Cyc_Tcutil_compress( t);
void* _temp236; void* _temp238; _LL232: if(( unsigned int) _temp230 > 4u?*(( int*)
_temp230) == Cyc_Absyn_IntType: 0){ _LL239: _temp238=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp230)->f1; if( _temp238 ==( void*) Cyc_Absyn_Unsigned){ goto _LL237;} else{
goto _LL234;} _LL237: _temp236=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp230)->f2; if( _temp236 ==( void*) Cyc_Absyn_B1){ goto _LL233;} else{ goto
_LL234;}} else{ goto _LL234;} _LL234: goto _LL235; _LL233: return Cyc_Toc_tagged_string_type();
_LL235: goto _LL231; _LL231:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=(( struct Cyc_List_List*) _check_null( tts))->tl){ struct
_tuple6 _temp242; void* _temp243; struct _tagged_string* _temp245; struct
_tuple6* _temp240=( struct _tuple6*)(( struct Cyc_List_List*) _check_null( tts))->hd;
_temp242=* _temp240; _LL246: _temp245= _temp242.f1; goto _LL244; _LL244:
_temp243= _temp242.f2; goto _LL241; _LL241: if( ! Cyc_Tcutil_unify( t, _temp243)){
continue;} return Cyc_Absyn_strct( _temp245);}}{ struct _tagged_string* x=({
struct _tagged_string* _temp249=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp249[ 0]=( struct _tagged_string) xprintf("_tagged_ptr%d",
Cyc_Toc_tagged_ptr_counter ++); _temp249;}); Cyc_Toc_add_tagged_ptr_decl( x, t);
Cyc_Toc_tagged_ptr_types=({ struct Cyc_List_List* _temp247=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp247->hd=( void*)({ struct
_tuple6* _temp248=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp248->f1= x; _temp248->f2= t; _temp248;}); _temp247->tl= Cyc_Toc_tagged_ptr_types;
_temp247;}); return Cyc_Absyn_strct( x);}} static int Cyc_Toc_temp_var_counter=
0; struct _tuple0* Cyc_Toc_temp_var(){ return({ struct _tuple0* _temp250=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp250->f1=( void*) Cyc_Absyn_Loc_n;
_temp250->f2=({ struct _tagged_string* _temp251=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp251[ 0]=( struct
_tagged_string) xprintf("_temp%d", Cyc_Toc_temp_var_counter ++); _temp251;});
_temp250;});} static int Cyc_Toc_fresh_label_counter= 0; static struct
_tagged_string* Cyc_Toc_fresh_label(){ return({ struct _tagged_string* _temp252=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp252[
0]=( struct _tagged_string) xprintf("_LL%d", Cyc_Toc_temp_var_counter ++);
_temp252;});} static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple1*
Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp255; struct Cyc_Absyn_Tqual
_temp257; struct Cyc_Core_Opt* _temp259; struct _tuple1 _temp253=* a; _LL260:
_temp259= _temp253.f1; goto _LL258; _LL258: _temp257= _temp253.f2; goto _LL256;
_LL256: _temp255= _temp253.f3; goto _LL254; _LL254: return({ struct _tuple1*
_temp261=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp261->f1=
_temp259; _temp261->f2= _temp257; _temp261->f3= Cyc_Toc_typ_to_c( _temp255);
_temp261;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp264; struct Cyc_Absyn_Tqual _temp266; struct _tuple3 _temp262=* x;
_LL267: _temp266= _temp262.f1; goto _LL265; _LL265: _temp264= _temp262.f2; goto
_LL263; _LL263: return({ struct _tuple3* _temp268=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp268->f1= _temp266; _temp268->f2= Cyc_Toc_typ_to_c(
_temp264); _temp268;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp269= t; struct Cyc_Absyn_Exp* _temp277; struct Cyc_Absyn_Tqual _temp279;
void* _temp281; struct Cyc_Core_Opt* _temp283; int _temp285; struct Cyc_Core_Opt*
_temp287; struct Cyc_Core_Opt _temp289; void* _temp290; struct Cyc_Core_Opt*
_temp292; _LL271: if(( unsigned int) _temp269 > 4u?*(( int*) _temp269) == Cyc_Absyn_ArrayType:
0){ _LL282: _temp281=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp269)->f1;
goto _LL280; _LL280: _temp279=(( struct Cyc_Absyn_ArrayType_struct*) _temp269)->f2;
goto _LL278; _LL278: _temp277=(( struct Cyc_Absyn_ArrayType_struct*) _temp269)->f3;
goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int) _temp269 > 4u?*((
int*) _temp269) == Cyc_Absyn_Evar: 0){ _LL293: _temp292=(( struct Cyc_Absyn_Evar_struct*)
_temp269)->f1; goto _LL288; _LL288: _temp287=(( struct Cyc_Absyn_Evar_struct*)
_temp269)->f2; if( _temp287 == 0){ goto _LL275;} else{ _temp289=* _temp287;
_LL291: _temp290=( void*) _temp289.v; goto _LL286;} _LL286: _temp285=(( struct
Cyc_Absyn_Evar_struct*) _temp269)->f3; goto _LL284; _LL284: _temp283=(( struct
Cyc_Absyn_Evar_struct*) _temp269)->f4; goto _LL274;} else{ goto _LL275;} _LL275:
goto _LL276; _LL272: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp294=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp294[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp295; _temp295.tag= Cyc_Absyn_ArrayType;
_temp295.f1=( void*) Cyc_Toc_typ_to_c_array( _temp281); _temp295.f2= _temp279;
_temp295.f3= _temp277; _temp295;}); _temp294;}); _LL274: return Cyc_Toc_typ_to_c_array(
_temp290); _LL276: return Cyc_Toc_typ_to_c( t); _LL270:;} static void* Cyc_Toc_typ_to_c(
void* t); static struct Cyc_Absyn_Structfield* Cyc_Toc_structfield_to_c( struct
Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield* _temp296=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp296->name= f->name; _temp296->tq= f->tq; _temp296->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp296->width= f->width; _temp296->attributes= f->attributes;
_temp296;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp297= t; struct
Cyc_Core_Opt* _temp349; int _temp351; struct Cyc_Core_Opt* _temp353; struct Cyc_Core_Opt*
_temp355; struct Cyc_Core_Opt* _temp357; int _temp359; struct Cyc_Core_Opt*
_temp361; struct Cyc_Core_Opt _temp363; void* _temp364; struct Cyc_Core_Opt*
_temp366; struct Cyc_Absyn_Tvar* _temp368; struct Cyc_Absyn_TunionInfo _temp370;
void* _temp372; struct Cyc_List_List* _temp374; void* _temp376; struct Cyc_Absyn_Tuniondecl*
_temp378; struct Cyc_Absyn_TunionInfo _temp380; struct Cyc_Absyn_TunionFieldInfo
_temp382; struct Cyc_List_List* _temp384; void* _temp386; struct Cyc_Absyn_Tunionfield*
_temp388; struct Cyc_Absyn_Tuniondecl* _temp390; struct Cyc_Absyn_TunionFieldInfo
_temp392; struct Cyc_Absyn_PtrInfo _temp394; struct Cyc_Absyn_Conref* _temp396;
struct Cyc_Absyn_Tqual _temp398; struct Cyc_Absyn_Conref* _temp400; void*
_temp402; void* _temp404; void* _temp406; void* _temp408; struct Cyc_Absyn_Exp*
_temp410; struct Cyc_Absyn_Tqual _temp412; void* _temp414; struct Cyc_Absyn_FnInfo
_temp416; struct Cyc_List_List* _temp418; struct Cyc_List_List* _temp420; struct
Cyc_Absyn_VarargInfo* _temp422; int _temp424; struct Cyc_List_List* _temp426;
void* _temp428; struct Cyc_Core_Opt* _temp430; struct Cyc_List_List* _temp432;
struct Cyc_List_List* _temp434; struct Cyc_List_List* _temp436; struct Cyc_List_List*
_temp438; struct Cyc_Absyn_Uniondecl** _temp440; struct Cyc_List_List* _temp442;
struct _tuple0* _temp444; struct Cyc_Absyn_Structdecl** _temp446; struct Cyc_List_List*
_temp448; struct _tuple0* _temp450; struct Cyc_Absyn_Enumdecl* _temp452; struct
_tuple0* _temp454; struct Cyc_Core_Opt* _temp456; struct Cyc_List_List* _temp458;
struct _tuple0* _temp460; void* _temp462; void* _temp464; struct Cyc_List_List*
_temp466; _LL299: if( _temp297 ==( void*) Cyc_Absyn_VoidType){ goto _LL300;}
else{ goto _LL301;} _LL301: if(( unsigned int) _temp297 > 4u?*(( int*) _temp297)
== Cyc_Absyn_Evar: 0){ _LL356: _temp355=(( struct Cyc_Absyn_Evar_struct*)
_temp297)->f1; goto _LL354; _LL354: _temp353=(( struct Cyc_Absyn_Evar_struct*)
_temp297)->f2; if( _temp353 == 0){ goto _LL352;} else{ goto _LL303;} _LL352:
_temp351=(( struct Cyc_Absyn_Evar_struct*) _temp297)->f3; goto _LL350; _LL350:
_temp349=(( struct Cyc_Absyn_Evar_struct*) _temp297)->f4; goto _LL302;} else{
goto _LL303;} _LL303: if(( unsigned int) _temp297 > 4u?*(( int*) _temp297) ==
Cyc_Absyn_Evar: 0){ _LL367: _temp366=(( struct Cyc_Absyn_Evar_struct*) _temp297)->f1;
goto _LL362; _LL362: _temp361=(( struct Cyc_Absyn_Evar_struct*) _temp297)->f2;
if( _temp361 == 0){ goto _LL305;} else{ _temp363=* _temp361; _LL365: _temp364=(
void*) _temp363.v; goto _LL360;} _LL360: _temp359=(( struct Cyc_Absyn_Evar_struct*)
_temp297)->f3; goto _LL358; _LL358: _temp357=(( struct Cyc_Absyn_Evar_struct*)
_temp297)->f4; goto _LL304;} else{ goto _LL305;} _LL305: if(( unsigned int)
_temp297 > 4u?*(( int*) _temp297) == Cyc_Absyn_VarType: 0){ _LL369: _temp368=((
struct Cyc_Absyn_VarType_struct*) _temp297)->f1; goto _LL306;} else{ goto _LL307;}
_LL307: if(( unsigned int) _temp297 > 4u?*(( int*) _temp297) == Cyc_Absyn_TunionType:
0){ _LL371: _temp370=(( struct Cyc_Absyn_TunionType_struct*) _temp297)->f1;
_LL377: _temp376=( void*) _temp370.tunion_info; if(*(( int*) _temp376) == Cyc_Absyn_KnownTunion){
_LL379: _temp378=(( struct Cyc_Absyn_KnownTunion_struct*) _temp376)->f1; goto
_LL375;} else{ goto _LL309;} _LL375: _temp374= _temp370.targs; goto _LL373;
_LL373: _temp372=( void*) _temp370.rgn; goto _LL308;} else{ goto _LL309;} _LL309:
if(( unsigned int) _temp297 > 4u?*(( int*) _temp297) == Cyc_Absyn_TunionType: 0){
_LL381: _temp380=(( struct Cyc_Absyn_TunionType_struct*) _temp297)->f1; goto
_LL310;} else{ goto _LL311;} _LL311: if(( unsigned int) _temp297 > 4u?*(( int*)
_temp297) == Cyc_Absyn_TunionFieldType: 0){ _LL383: _temp382=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp297)->f1; _LL387: _temp386=( void*) _temp382.field_info; if(*(( int*)
_temp386) == Cyc_Absyn_KnownTunionfield){ _LL391: _temp390=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp386)->f1; goto _LL389; _LL389: _temp388=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp386)->f2; goto _LL385;} else{ goto _LL313;} _LL385: _temp384= _temp382.targs;
goto _LL312;} else{ goto _LL313;} _LL313: if(( unsigned int) _temp297 > 4u?*((
int*) _temp297) == Cyc_Absyn_TunionFieldType: 0){ _LL393: _temp392=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp297)->f1; goto _LL314;} else{ goto _LL315;} _LL315: if(( unsigned int)
_temp297 > 4u?*(( int*) _temp297) == Cyc_Absyn_PointerType: 0){ _LL395: _temp394=((
struct Cyc_Absyn_PointerType_struct*) _temp297)->f1; _LL405: _temp404=( void*)
_temp394.elt_typ; goto _LL403; _LL403: _temp402=( void*) _temp394.rgn_typ; goto
_LL401; _LL401: _temp400= _temp394.nullable; goto _LL399; _LL399: _temp398=
_temp394.tq; goto _LL397; _LL397: _temp396= _temp394.bounds; goto _LL316;} else{
goto _LL317;} _LL317: if(( unsigned int) _temp297 > 4u?*(( int*) _temp297) ==
Cyc_Absyn_IntType: 0){ _LL409: _temp408=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp297)->f1; goto _LL407; _LL407: _temp406=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp297)->f2; goto _LL318;} else{ goto _LL319;} _LL319: if( _temp297 ==( void*)
Cyc_Absyn_FloatType){ goto _LL320;} else{ goto _LL321;} _LL321: if( _temp297 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL322;} else{ goto _LL323;} _LL323: if((
unsigned int) _temp297 > 4u?*(( int*) _temp297) == Cyc_Absyn_ArrayType: 0){
_LL415: _temp414=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp297)->f1;
goto _LL413; _LL413: _temp412=(( struct Cyc_Absyn_ArrayType_struct*) _temp297)->f2;
goto _LL411; _LL411: _temp410=(( struct Cyc_Absyn_ArrayType_struct*) _temp297)->f3;
goto _LL324;} else{ goto _LL325;} _LL325: if(( unsigned int) _temp297 > 4u?*((
int*) _temp297) == Cyc_Absyn_FnType: 0){ _LL417: _temp416=(( struct Cyc_Absyn_FnType_struct*)
_temp297)->f1; _LL433: _temp432= _temp416.tvars; goto _LL431; _LL431: _temp430=
_temp416.effect; goto _LL429; _LL429: _temp428=( void*) _temp416.ret_typ; goto
_LL427; _LL427: _temp426= _temp416.args; goto _LL425; _LL425: _temp424= _temp416.c_varargs;
goto _LL423; _LL423: _temp422= _temp416.cyc_varargs; goto _LL421; _LL421:
_temp420= _temp416.rgn_po; goto _LL419; _LL419: _temp418= _temp416.attributes;
goto _LL326;} else{ goto _LL327;} _LL327: if(( unsigned int) _temp297 > 4u?*((
int*) _temp297) == Cyc_Absyn_TupleType: 0){ _LL435: _temp434=(( struct Cyc_Absyn_TupleType_struct*)
_temp297)->f1; goto _LL328;} else{ goto _LL329;} _LL329: if(( unsigned int)
_temp297 > 4u?*(( int*) _temp297) == Cyc_Absyn_AnonStructType: 0){ _LL437:
_temp436=(( struct Cyc_Absyn_AnonStructType_struct*) _temp297)->f1; goto _LL330;}
else{ goto _LL331;} _LL331: if(( unsigned int) _temp297 > 4u?*(( int*) _temp297)
== Cyc_Absyn_AnonUnionType: 0){ _LL439: _temp438=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp297)->f1; goto _LL332;} else{ goto _LL333;} _LL333: if(( unsigned int)
_temp297 > 4u?*(( int*) _temp297) == Cyc_Absyn_UnionType: 0){ _LL445: _temp444=((
struct Cyc_Absyn_UnionType_struct*) _temp297)->f1; goto _LL443; _LL443: _temp442=((
struct Cyc_Absyn_UnionType_struct*) _temp297)->f2; goto _LL441; _LL441: _temp440=((
struct Cyc_Absyn_UnionType_struct*) _temp297)->f3; goto _LL334;} else{ goto
_LL335;} _LL335: if(( unsigned int) _temp297 > 4u?*(( int*) _temp297) == Cyc_Absyn_StructType:
0){ _LL451: _temp450=(( struct Cyc_Absyn_StructType_struct*) _temp297)->f1; goto
_LL449; _LL449: _temp448=(( struct Cyc_Absyn_StructType_struct*) _temp297)->f2;
goto _LL447; _LL447: _temp446=(( struct Cyc_Absyn_StructType_struct*) _temp297)->f3;
goto _LL336;} else{ goto _LL337;} _LL337: if(( unsigned int) _temp297 > 4u?*((
int*) _temp297) == Cyc_Absyn_EnumType: 0){ _LL455: _temp454=(( struct Cyc_Absyn_EnumType_struct*)
_temp297)->f1; goto _LL453; _LL453: _temp452=(( struct Cyc_Absyn_EnumType_struct*)
_temp297)->f2; goto _LL338;} else{ goto _LL339;} _LL339: if(( unsigned int)
_temp297 > 4u?*(( int*) _temp297) == Cyc_Absyn_TypedefType: 0){ _LL461: _temp460=((
struct Cyc_Absyn_TypedefType_struct*) _temp297)->f1; goto _LL459; _LL459:
_temp458=(( struct Cyc_Absyn_TypedefType_struct*) _temp297)->f2; goto _LL457;
_LL457: _temp456=(( struct Cyc_Absyn_TypedefType_struct*) _temp297)->f3; goto
_LL340;} else{ goto _LL341;} _LL341: if(( unsigned int) _temp297 > 4u?*(( int*)
_temp297) == Cyc_Absyn_RgnHandleType: 0){ _LL463: _temp462=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp297)->f1; goto _LL342;} else{ goto _LL343;} _LL343: if( _temp297 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL344;} else{ goto _LL345;} _LL345: if(( unsigned int)
_temp297 > 4u?*(( int*) _temp297) == Cyc_Absyn_AccessEff: 0){ _LL465: _temp464=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp297)->f1; goto _LL346;} else{
goto _LL347;} _LL347: if(( unsigned int) _temp297 > 4u?*(( int*) _temp297) ==
Cyc_Absyn_JoinEff: 0){ _LL467: _temp466=(( struct Cyc_Absyn_JoinEff_struct*)
_temp297)->f1; goto _LL348;} else{ goto _LL298;} _LL300: return t; _LL302:
return( void*) Cyc_Absyn_VoidType; _LL304: return Cyc_Toc_typ_to_c( _temp364);
_LL306: return Cyc_Absyn_void_star_typ(); _LL308: return Cyc_Absyn_void_star_typ();
_LL310:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp468=( unsigned char*)"unresolved TunionType"; struct
_tagged_string _temp469; _temp469.curr= _temp468; _temp469.base= _temp468;
_temp469.last_plus_one= _temp468 + 22; _temp469;}))); _LL312: if( _temp388->typs
== 0){ if( _temp390->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp388->name,({
unsigned char* _temp470=( unsigned char*)"_struct"; struct _tagged_string
_temp471; _temp471.curr= _temp470; _temp471.base= _temp470; _temp471.last_plus_one=
_temp470 + 8; _temp471;})));} _LL314:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp472=( unsigned char*)"unresolved TunionFieldType";
struct _tagged_string _temp473; _temp473.curr= _temp472; _temp473.base= _temp472;
_temp473.last_plus_one= _temp472 + 27; _temp473;}))); _LL316: _temp404= Cyc_Toc_typ_to_c_array(
_temp404);{ void* _temp474=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp396))->v; void* _temp480; _LL476: if((
unsigned int) _temp474 > 1u?*(( int*) _temp474) == Cyc_Absyn_Eq_constr: 0){
_LL481: _temp480=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp474)->f1;
if( _temp480 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL477;} else{ goto _LL478;}}
else{ goto _LL478;} _LL478: goto _LL479; _LL477: return Cyc_Toc_add_tagged_ptr_type(
_temp404, _temp398); _LL479: return Cyc_Absyn_star_typ( _temp404,( void*) Cyc_Absyn_HeapRgn,
_temp398); _LL475:;} _LL318: return t; _LL320: return t; _LL322: return t;
_LL324: return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp482=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp482[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp483; _temp483.tag= Cyc_Absyn_ArrayType;
_temp483.f1=( void*) Cyc_Toc_typ_to_c_array( _temp414); _temp483.f2= _temp412;
_temp483.f3= _temp410; _temp483;}); _temp482;}); _LL326: { struct Cyc_List_List*
_temp484= _temp418; goto _LL485; _LL485: { struct Cyc_List_List* _temp486= 0;
goto _LL487; _LL487: for( 0; _temp418 != 0; _temp418=(( struct Cyc_List_List*)
_check_null( _temp418))->tl){ void* _temp488=( void*)(( struct Cyc_List_List*)
_check_null( _temp418))->hd; _LL490: if( _temp488 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL491;} else{ goto _LL492;} _LL492: if( _temp488 ==( void*) Cyc_Absyn_Const_att){
goto _LL493;} else{ goto _LL494;} _LL494: goto _LL495; _LL491: goto _LL493;
_LL493: continue; _LL495: _temp486=({ struct Cyc_List_List* _temp496=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp496->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( _temp418))->hd); _temp496->tl=
_temp486; _temp496;}); goto _LL489; _LL489:;}{ struct Cyc_List_List* _temp497=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp426); goto _LL498; _LL498: if(
_temp422 != 0){ void* _temp499= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp422))->type,( void*)(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp422))->rgn, Cyc_Toc_mt_tq)); goto
_LL500; _LL500: { struct _tuple1* _temp502=({ struct _tuple1* _temp501=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp501->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp422))->name; _temp501->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp422))->tq; _temp501->f3= _temp499; _temp501;}); goto _LL503;
_LL503: _temp497=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_imp_append)( _temp497,({ struct Cyc_List_List*
_temp504=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp504->hd=( void*) _temp502; _temp504->tl= 0; _temp504;}));}} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp505=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp505[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp506; _temp506.tag= Cyc_Absyn_FnType; _temp506.f1=({ struct Cyc_Absyn_FnInfo
_temp507; _temp507.tvars= 0; _temp507.effect= 0; _temp507.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp428); _temp507.args= _temp497; _temp507.c_varargs= _temp424; _temp507.cyc_varargs=
0; _temp507.rgn_po= 0; _temp507.attributes= _temp486; _temp507;}); _temp506;});
_temp505;});}}} _LL328: _temp434=(( struct Cyc_List_List*(*)( struct _tuple3*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp434);{ struct _tagged_string* _temp508= Cyc_Toc_add_tuple_type( _temp434);
goto _LL509; _LL509: return Cyc_Absyn_strct( _temp508);} _LL330: return( void*)({
struct Cyc_Absyn_AnonStructType_struct* _temp510=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp510[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp511; _temp511.tag= Cyc_Absyn_AnonStructType;
_temp511.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp436); _temp511;}); _temp510;}); _LL332: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp512=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp512[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp513; _temp513.tag=
Cyc_Absyn_AnonUnionType; _temp513.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp438); _temp513;}); _temp512;}); _LL334: if( _temp444 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp514=(
unsigned char*)"anonymous union"; struct _tagged_string _temp515; _temp515.curr=
_temp514; _temp515.base= _temp514; _temp515.last_plus_one= _temp514 + 16;
_temp515;}));} return Cyc_Absyn_unionq_typ(( struct _tuple0*) _check_null(
_temp444)); _LL336: if( _temp450 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp516=( unsigned char*)"anonymous struct"; struct
_tagged_string _temp517; _temp517.curr= _temp516; _temp517.base= _temp516;
_temp517.last_plus_one= _temp516 + 17; _temp517;}));} return Cyc_Absyn_strctq((
struct _tuple0*) _check_null( _temp450)); _LL338: return t; _LL340: if( _temp456
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp518=( unsigned char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp519; _temp519.curr= _temp518; _temp519.base= _temp518;
_temp519.last_plus_one= _temp518 + 38; _temp519;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp520=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp520[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp521; _temp521.tag= Cyc_Absyn_TypedefType;
_temp521.f1= _temp460; _temp521.f2= 0; _temp521.f3=({ struct Cyc_Core_Opt*
_temp522=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp522->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp456))->v); _temp522;}); _temp521;}); _temp520;}); _LL342:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL344: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp523=( unsigned char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 55; _temp524;})); _LL346: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp525=(
unsigned char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp526; _temp526.curr= _temp525; _temp526.base= _temp525;
_temp526.last_plus_one= _temp525 + 54; _temp526;})); _LL348: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp527=(
unsigned char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp528; _temp528.curr= _temp527; _temp528.base= _temp527;
_temp528.last_plus_one= _temp527 + 52; _temp528;})); _LL298:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp529= t; struct Cyc_Absyn_Exp* _temp535; struct Cyc_Absyn_Tqual
_temp537; void* _temp539; _LL531: if(( unsigned int) _temp529 > 4u?*(( int*)
_temp529) == Cyc_Absyn_ArrayType: 0){ _LL540: _temp539=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp529)->f1; goto _LL538; _LL538: _temp537=(( struct Cyc_Absyn_ArrayType_struct*)
_temp529)->f2; goto _LL536; _LL536: _temp535=(( struct Cyc_Absyn_ArrayType_struct*)
_temp529)->f3; goto _LL532;} else{ goto _LL533;} _LL533: goto _LL534; _LL532:
return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ( _temp539,( void*) Cyc_Absyn_HeapRgn,
_temp537), e, l); _LL534: return Cyc_Absyn_cast_exp( t, e, l); _LL530:;} static
int Cyc_Toc_atomic_typ( void* t){ void* _temp541= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp577; void* _temp579; void* _temp581; struct Cyc_Absyn_Exp*
_temp583; struct Cyc_Absyn_Tqual _temp585; void* _temp587; struct Cyc_Absyn_FnInfo
_temp589; struct Cyc_List_List* _temp591; struct Cyc_Absyn_Structdecl** _temp593;
struct Cyc_List_List* _temp595; struct _tuple0* _temp597; struct Cyc_Absyn_TunionInfo
_temp599; struct Cyc_List_List* _temp601; struct Cyc_List_List* _temp603; struct
Cyc_Absyn_TunionFieldInfo _temp605; struct Cyc_List_List* _temp607; void*
_temp609; struct Cyc_Absyn_Tunionfield* _temp611; struct Cyc_Absyn_Tuniondecl*
_temp613; struct Cyc_Absyn_Uniondecl** _temp615; struct Cyc_List_List* _temp617;
struct _tuple0* _temp619; struct Cyc_Absyn_PtrInfo _temp621; void* _temp623;
_LL543: if( _temp541 ==( void*) Cyc_Absyn_VoidType){ goto _LL544;} else{ goto
_LL545;} _LL545: if(( unsigned int) _temp541 > 4u?*(( int*) _temp541) == Cyc_Absyn_VarType:
0){ _LL578: _temp577=(( struct Cyc_Absyn_VarType_struct*) _temp541)->f1; goto
_LL546;} else{ goto _LL547;} _LL547: if(( unsigned int) _temp541 > 4u?*(( int*)
_temp541) == Cyc_Absyn_IntType: 0){ _LL582: _temp581=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp541)->f1; goto _LL580; _LL580: _temp579=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp541)->f2; goto _LL548;} else{ goto _LL549;} _LL549: if( _temp541 ==( void*)
Cyc_Absyn_FloatType){ goto _LL550;} else{ goto _LL551;} _LL551: if( _temp541 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL552;} else{ goto _LL553;} _LL553: if((
unsigned int) _temp541 > 4u?*(( int*) _temp541) == Cyc_Absyn_ArrayType: 0){
_LL588: _temp587=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp541)->f1;
goto _LL586; _LL586: _temp585=(( struct Cyc_Absyn_ArrayType_struct*) _temp541)->f2;
goto _LL584; _LL584: _temp583=(( struct Cyc_Absyn_ArrayType_struct*) _temp541)->f3;
goto _LL554;} else{ goto _LL555;} _LL555: if(( unsigned int) _temp541 > 4u?*((
int*) _temp541) == Cyc_Absyn_FnType: 0){ _LL590: _temp589=(( struct Cyc_Absyn_FnType_struct*)
_temp541)->f1; goto _LL556;} else{ goto _LL557;} _LL557: if(( unsigned int)
_temp541 > 4u?*(( int*) _temp541) == Cyc_Absyn_TupleType: 0){ _LL592: _temp591=((
struct Cyc_Absyn_TupleType_struct*) _temp541)->f1; goto _LL558;} else{ goto
_LL559;} _LL559: if(( unsigned int) _temp541 > 4u?*(( int*) _temp541) == Cyc_Absyn_StructType:
0){ _LL598: _temp597=(( struct Cyc_Absyn_StructType_struct*) _temp541)->f1; goto
_LL596; _LL596: _temp595=(( struct Cyc_Absyn_StructType_struct*) _temp541)->f2;
goto _LL594; _LL594: _temp593=(( struct Cyc_Absyn_StructType_struct*) _temp541)->f3;
goto _LL560;} else{ goto _LL561;} _LL561: if(( unsigned int) _temp541 > 4u?*((
int*) _temp541) == Cyc_Absyn_TunionType: 0){ _LL600: _temp599=(( struct Cyc_Absyn_TunionType_struct*)
_temp541)->f1; goto _LL562;} else{ goto _LL563;} _LL563: if(( unsigned int)
_temp541 > 4u?*(( int*) _temp541) == Cyc_Absyn_AnonStructType: 0){ _LL602:
_temp601=(( struct Cyc_Absyn_AnonStructType_struct*) _temp541)->f1; goto _LL564;}
else{ goto _LL565;} _LL565: if(( unsigned int) _temp541 > 4u?*(( int*) _temp541)
== Cyc_Absyn_AnonUnionType: 0){ _LL604: _temp603=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp541)->f1; goto _LL566;} else{ goto _LL567;} _LL567: if(( unsigned int)
_temp541 > 4u?*(( int*) _temp541) == Cyc_Absyn_TunionFieldType: 0){ _LL606:
_temp605=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp541)->f1; _LL610:
_temp609=( void*) _temp605.field_info; if(*(( int*) _temp609) == Cyc_Absyn_KnownTunionfield){
_LL614: _temp613=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp609)->f1;
goto _LL612; _LL612: _temp611=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp609)->f2; goto _LL608;} else{ goto _LL569;} _LL608: _temp607= _temp605.targs;
goto _LL568;} else{ goto _LL569;} _LL569: if(( unsigned int) _temp541 > 4u?*((
int*) _temp541) == Cyc_Absyn_UnionType: 0){ _LL620: _temp619=(( struct Cyc_Absyn_UnionType_struct*)
_temp541)->f1; goto _LL618; _LL618: _temp617=(( struct Cyc_Absyn_UnionType_struct*)
_temp541)->f2; goto _LL616; _LL616: _temp615=(( struct Cyc_Absyn_UnionType_struct*)
_temp541)->f3; goto _LL570;} else{ goto _LL571;} _LL571: if(( unsigned int)
_temp541 > 4u?*(( int*) _temp541) == Cyc_Absyn_PointerType: 0){ _LL622: _temp621=((
struct Cyc_Absyn_PointerType_struct*) _temp541)->f1; goto _LL572;} else{ goto
_LL573;} _LL573: if(( unsigned int) _temp541 > 4u?*(( int*) _temp541) == Cyc_Absyn_RgnHandleType:
0){ _LL624: _temp623=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp541)->f1;
goto _LL574;} else{ goto _LL575;} _LL575: goto _LL576; _LL544: return 1; _LL546:
return 0; _LL548: return 1; _LL550: return 1; _LL552: return 0; _LL554: return
Cyc_Toc_atomic_typ( _temp587); _LL556: return 1; _LL558: for( 0; _temp591 != 0;
_temp591=(( struct Cyc_List_List*) _check_null( _temp591))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp591))->hd)).f2)){
return 0;}} return 1; _LL560: return 0; _LL562: return 0; _LL564: _temp603=
_temp601; goto _LL566; _LL566: for( 0; _temp603 != 0; _temp603=(( struct Cyc_List_List*)
_check_null( _temp603))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp603))->hd)->type)){ return 0;}} return
1; _LL568: return _temp611->typs == 0; _LL570: return 0; _LL572: return 0;
_LL574: return 0; _LL576:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp625= Cyc_Absynpp_typ2string(
t); xprintf("atomic_typ:  bad type %.*s", _temp625.last_plus_one - _temp625.curr,
_temp625.curr);})); return 0; _LL542:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp626= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp632;
struct Cyc_Absyn_Conref* _temp634; struct Cyc_Absyn_Tqual _temp636; struct Cyc_Absyn_Conref*
_temp638; void* _temp640; void* _temp642; _LL628: if(( unsigned int) _temp626 >
4u?*(( int*) _temp626) == Cyc_Absyn_PointerType: 0){ _LL633: _temp632=(( struct
Cyc_Absyn_PointerType_struct*) _temp626)->f1; _LL643: _temp642=( void*) _temp632.elt_typ;
goto _LL641; _LL641: _temp640=( void*) _temp632.rgn_typ; goto _LL639; _LL639:
_temp638= _temp632.nullable; goto _LL637; _LL637: _temp636= _temp632.tq; goto
_LL635; _LL635: _temp634= _temp632.bounds; goto _LL629;} else{ goto _LL630;}
_LL630: goto _LL631; _LL629: { void* _temp644= Cyc_Tcutil_compress( _temp642);
_LL646: if( _temp644 ==( void*) Cyc_Absyn_VoidType){ goto _LL647;} else{ goto
_LL648;} _LL648: goto _LL649; _LL647: return 1; _LL649: return 0; _LL645:;}
_LL631: return 0; _LL627:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_string* f){ void* _temp650= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp662; struct Cyc_Absyn_Structdecl* _temp664; struct Cyc_List_List* _temp665;
struct _tuple0* _temp667; struct Cyc_List_List* _temp669; struct Cyc_List_List*
_temp671; struct Cyc_Absyn_Uniondecl** _temp673; struct Cyc_List_List* _temp675;
struct _tuple0* _temp677; _LL652: if(( unsigned int) _temp650 > 4u?*(( int*)
_temp650) == Cyc_Absyn_StructType: 0){ _LL668: _temp667=(( struct Cyc_Absyn_StructType_struct*)
_temp650)->f1; goto _LL666; _LL666: _temp665=(( struct Cyc_Absyn_StructType_struct*)
_temp650)->f2; goto _LL663; _LL663: _temp662=(( struct Cyc_Absyn_StructType_struct*)
_temp650)->f3; if( _temp662 == 0){ goto _LL654;} else{ _temp664=* _temp662; goto
_LL653;}} else{ goto _LL654;} _LL654: if(( unsigned int) _temp650 > 4u?*(( int*)
_temp650) == Cyc_Absyn_AnonStructType: 0){ _LL670: _temp669=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp650)->f1; goto _LL655;} else{ goto _LL656;} _LL656: if(( unsigned int)
_temp650 > 4u?*(( int*) _temp650) == Cyc_Absyn_AnonUnionType: 0){ _LL672:
_temp671=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp650)->f1; goto _LL657;}
else{ goto _LL658;} _LL658: if(( unsigned int) _temp650 > 4u?*(( int*) _temp650)
== Cyc_Absyn_UnionType: 0){ _LL678: _temp677=(( struct Cyc_Absyn_UnionType_struct*)
_temp650)->f1; goto _LL676; _LL676: _temp675=(( struct Cyc_Absyn_UnionType_struct*)
_temp650)->f2; goto _LL674; _LL674: _temp673=(( struct Cyc_Absyn_UnionType_struct*)
_temp650)->f3; goto _LL659;} else{ goto _LL660;} _LL660: goto _LL661; _LL653:
if( _temp664->fields == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp679=( unsigned char*)"is_poly_field: struct missing fields";
struct _tagged_string _temp680; _temp680.curr= _temp679; _temp680.base= _temp679;
_temp680.last_plus_one= _temp679 + 37; _temp680;})));} _temp669=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp664->fields))->v; goto _LL655; _LL655: {
struct Cyc_Absyn_Structfield* _temp681= Cyc_Absyn_lookup_field( _temp669, f);
goto _LL682; _LL682: if( _temp681 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct _tagged_string _temp683=*((
struct Cyc_Absyn_Structfield*) _check_null( _temp681))->name; xprintf("is_poly_field: bad field %.*s",
_temp683.last_plus_one - _temp683.curr, _temp683.curr);}));} return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp681))->type);} _LL657:
return 0; _LL659: return 0; _LL661:( void) _throw((( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp684= Cyc_Absynpp_typ2string( t); xprintf("is_poly_field: bad type %.*s",
_temp684.last_plus_one - _temp684.curr, _temp684.curr);}))); _LL651:;} static
int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp685=( void*) e->r;
struct _tagged_string* _temp693; struct Cyc_Absyn_Exp* _temp695; struct
_tagged_string* _temp697; struct Cyc_Absyn_Exp* _temp699; _LL687: if(*(( int*)
_temp685) == Cyc_Absyn_StructMember_e){ _LL696: _temp695=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp685)->f1; goto _LL694; _LL694: _temp693=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp685)->f2; goto _LL688;} else{ goto _LL689;} _LL689: if(*(( int*) _temp685)
== Cyc_Absyn_StructArrow_e){ _LL700: _temp699=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp685)->f1; goto _LL698; _LL698: _temp697=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp685)->f2; goto _LL690;} else{ goto _LL691;} _LL691: goto _LL692; _LL688:
return Cyc_Toc_is_poly_field(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp695->topt))->v, _temp693); _LL690: { void* _temp701= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp699->topt))->v); struct Cyc_Absyn_PtrInfo
_temp707; struct Cyc_Absyn_Conref* _temp709; struct Cyc_Absyn_Tqual _temp711;
struct Cyc_Absyn_Conref* _temp713; void* _temp715; void* _temp717; _LL703: if((
unsigned int) _temp701 > 4u?*(( int*) _temp701) == Cyc_Absyn_PointerType: 0){
_LL708: _temp707=(( struct Cyc_Absyn_PointerType_struct*) _temp701)->f1; _LL718:
_temp717=( void*) _temp707.elt_typ; goto _LL716; _LL716: _temp715=( void*)
_temp707.rgn_typ; goto _LL714; _LL714: _temp713= _temp707.nullable; goto _LL712;
_LL712: _temp711= _temp707.tq; goto _LL710; _LL710: _temp709= _temp707.bounds;
goto _LL704;} else{ goto _LL705;} _LL705: goto _LL706; _LL704: return Cyc_Toc_is_poly_field(
_temp717, _temp697); _LL706:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp719= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp699->topt))->v); xprintf("is_poly_project:  bad type %.*s",
_temp719.last_plus_one - _temp719.curr, _temp719.curr);})); return 0; _LL702:;}
_LL692: return 0; _LL686:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_qv,
0),({ struct Cyc_List_List* _temp720=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp720->hd=( void*) s; _temp720->tl= 0; _temp720;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_atomic_qv, 0),({
struct Cyc_List_List* _temp721=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp721->hd=( void*) s; _temp721->tl= 0; _temp721;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__region_malloc_qv,
0),({ struct Cyc_List_List* _temp722=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp722->hd=( void*) rgn; _temp722->tl=({ struct Cyc_List_List*
_temp723=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp723->hd=( void*) s; _temp723->tl= 0; _temp723;}); _temp722;}), 0);} static
unsigned char _temp726[ 3u]="f0"; static struct _tagged_string Cyc_Toc_f0={
_temp726, _temp726, _temp726 + 3u}; static struct _tagged_string* Cyc_Toc_field_names_v[
1u]={& Cyc_Toc_f0}; struct _tagged_ptr3{ struct _tagged_string** curr; struct
_tagged_string** base; struct _tagged_string** last_plus_one; } ; static struct
_tagged_ptr3 Cyc_Toc_field_names={( struct _tagged_string**) Cyc_Toc_field_names_v,(
struct _tagged_string**) Cyc_Toc_field_names_v,( struct _tagged_string**) Cyc_Toc_field_names_v
+ 1u}; static struct _tagged_string* Cyc_Toc_fieldname( int i){ unsigned int fsz=({
struct _tagged_ptr3 _temp734= Cyc_Toc_field_names;( unsigned int)( _temp734.last_plus_one
- _temp734.curr);}); if( i >= fsz){ Cyc_Toc_field_names=({ unsigned int _temp729=(
unsigned int)( i + 1); struct _tagged_string** _temp730=( struct _tagged_string**)
GC_malloc( sizeof( struct _tagged_string*) * _temp729); struct _tagged_ptr3
_temp733={ _temp730, _temp730, _temp730 + _temp729};{ unsigned int _temp731=
_temp729; unsigned int j; for( j= 0; j < _temp731; j ++){ _temp730[ j]= j < fsz?*((
struct _tagged_string**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Toc_field_names, sizeof( struct _tagged_string*),(
int) j):({ struct _tagged_string* _temp732=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string)); _temp732[ 0]=( struct _tagged_string) xprintf("f%d",(
int) j); _temp732;});}}; _temp733;});} return*(( struct _tagged_string**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Toc_field_names,
sizeof( struct _tagged_string*), i);} static const int Cyc_Toc_NullCheck= 0;
struct Cyc_Toc_NullCheck_struct{ int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3; } ; static const int Cyc_Toc_UntagPtrNocheck= 2; struct Cyc_Toc_UntagPtrNocheck_struct{
int tag; void* f1; } ; static const int Cyc_Toc_TagPtr= 3; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Toc_NoConv=
0; struct _tuple7{ void* f1; void* f2; } ; static void* Cyc_Toc_conversion( void*
t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp736=({ struct _tuple7 _temp735; _temp735.f1= t1; _temp735.f2=
t2; _temp735;}); void* _temp744; struct Cyc_Absyn_PtrInfo _temp746; void*
_temp748; struct Cyc_Absyn_PtrInfo _temp750; void* _temp752; void* _temp754;
void* _temp756; void* _temp758; struct Cyc_Absyn_PtrInfo _temp760; _LL738:
_LL749: _temp748= _temp736.f1; if(( unsigned int) _temp748 > 4u?*(( int*)
_temp748) == Cyc_Absyn_PointerType: 0){ _LL751: _temp750=(( struct Cyc_Absyn_PointerType_struct*)
_temp748)->f1; goto _LL745;} else{ goto _LL740;} _LL745: _temp744= _temp736.f2;
if(( unsigned int) _temp744 > 4u?*(( int*) _temp744) == Cyc_Absyn_PointerType: 0){
_LL747: _temp746=(( struct Cyc_Absyn_PointerType_struct*) _temp744)->f1; goto
_LL739;} else{ goto _LL740;} _LL740: _LL759: _temp758= _temp736.f1; if((
unsigned int) _temp758 > 4u?*(( int*) _temp758) == Cyc_Absyn_PointerType: 0){
_LL761: _temp760=(( struct Cyc_Absyn_PointerType_struct*) _temp758)->f1; goto
_LL753;} else{ goto _LL742;} _LL753: _temp752= _temp736.f2; if(( unsigned int)
_temp752 > 4u?*(( int*) _temp752) == Cyc_Absyn_IntType: 0){ _LL757: _temp756=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp752)->f1; goto _LL755; _LL755:
_temp754=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp752)->f2; goto _LL741;}
else{ goto _LL742;} _LL742: goto _LL743; _LL739: { int _temp762=(( int(*)( int,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp750.nullable); goto
_LL763; _LL763: { int _temp764=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp746.nullable); goto _LL765; _LL765: { void* _temp766=(( void*(*)( void*,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp750.bounds); goto _LL767; _LL767: { void* _temp768=(( void*(*)( void*,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp746.bounds); goto _LL769; _LL769: { struct _tuple7 _temp771=({ struct
_tuple7 _temp770; _temp770.f1= _temp766; _temp770.f2= _temp768; _temp770;});
void* _temp781; struct Cyc_Absyn_Exp* _temp783; void* _temp785; struct Cyc_Absyn_Exp*
_temp787; void* _temp789; void* _temp791; void* _temp793; void* _temp795; struct
Cyc_Absyn_Exp* _temp797; void* _temp799; struct Cyc_Absyn_Exp* _temp801; void*
_temp803; _LL773: _LL786: _temp785= _temp771.f1; if(( unsigned int) _temp785 > 1u?*((
int*) _temp785) == Cyc_Absyn_Upper_b: 0){ _LL788: _temp787=(( struct Cyc_Absyn_Upper_b_struct*)
_temp785)->f1; goto _LL782;} else{ goto _LL775;} _LL782: _temp781= _temp771.f2;
if(( unsigned int) _temp781 > 1u?*(( int*) _temp781) == Cyc_Absyn_Upper_b: 0){
_LL784: _temp783=(( struct Cyc_Absyn_Upper_b_struct*) _temp781)->f1; goto _LL774;}
else{ goto _LL775;} _LL775: _LL792: _temp791= _temp771.f1; if( _temp791 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL790;} else{ goto _LL777;} _LL790: _temp789=
_temp771.f2; if( _temp789 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL776;} else{
goto _LL777;} _LL777: _LL796: _temp795= _temp771.f1; if(( unsigned int) _temp795
> 1u?*(( int*) _temp795) == Cyc_Absyn_Upper_b: 0){ _LL798: _temp797=(( struct
Cyc_Absyn_Upper_b_struct*) _temp795)->f1; goto _LL794;} else{ goto _LL779;}
_LL794: _temp793= _temp771.f2; if( _temp793 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL778;} else{ goto _LL779;} _LL779: _LL804: _temp803= _temp771.f1; if(
_temp803 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL800;} else{ goto _LL772;}
_LL800: _temp799= _temp771.f2; if(( unsigned int) _temp799 > 1u?*(( int*)
_temp799) == Cyc_Absyn_Upper_b: 0){ _LL802: _temp801=(( struct Cyc_Absyn_Upper_b_struct*)
_temp799)->f1; goto _LL780;} else{ goto _LL772;} _LL774: if( _temp762? !
_temp764: 0){ return( void*)({ struct Cyc_Toc_NullCheck_struct* _temp805=(
struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp805[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp806; _temp806.tag= Cyc_Toc_NullCheck;
_temp806.f1=( void*) t2; _temp806;}); _temp805;});} return( void*) Cyc_Toc_NoConv;
_LL776: return( void*) Cyc_Toc_NoConv; _LL778: return( void*)({ struct Cyc_Toc_TagPtr_struct*
_temp807=( struct Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp807[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp808; _temp808.tag= Cyc_Toc_TagPtr;
_temp808.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp808.f2= _temp797; _temp808;});
_temp807;}); _LL780: return( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp809=(
struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp809[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp810; _temp810.tag= Cyc_Toc_UntagPtr;
_temp810.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp810.f2= _temp801; _temp810.f3=
_temp764; _temp810;}); _temp809;}); _LL772:;}}}}} _LL741: { void* _temp811=((
void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp760.bounds); goto _LL812; _LL812: { void* _temp813= _temp811; struct Cyc_Absyn_Exp*
_temp819; _LL815: if(( unsigned int) _temp813 > 1u?*(( int*) _temp813) == Cyc_Absyn_Upper_b:
0){ _LL820: _temp819=(( struct Cyc_Absyn_Upper_b_struct*) _temp813)->f1; goto
_LL816;} else{ goto _LL817;} _LL817: if( _temp813 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL818;} else{ goto _LL814;} _LL816: return( void*) Cyc_Toc_NoConv; _LL818:
return( void*)({ struct Cyc_Toc_UntagPtrNocheck_struct* _temp821=( struct Cyc_Toc_UntagPtrNocheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_UntagPtrNocheck_struct)); _temp821[ 0]=({
struct Cyc_Toc_UntagPtrNocheck_struct _temp822; _temp822.tag= Cyc_Toc_UntagPtrNocheck;
_temp822.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp822;}); _temp821;}); _LL814:;}}
_LL743: return( void*) Cyc_Toc_NoConv; _LL737:;}} static unsigned char _temp825[
14u]="sfile_to_file"; static struct _tagged_string Cyc_Toc__sfile_to_file_string={
_temp825, _temp825, _temp825 + 14u}; static struct _tuple0 Cyc_Toc__sfile_to_file_var={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__sfile_to_file_string}; static struct Cyc_Absyn_Exp*
Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(&
Cyc_Toc__sfile_to_file_var, 0),({ struct Cyc_List_List* _temp826=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp826->hd=( void*) e; _temp826->tl=
0; _temp826;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct
Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__throw_qv,
0),({ struct Cyc_List_List* _temp827=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp827->hd=( void*) e; _temp827->tl= 0; _temp827;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp828=(
unsigned char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp829; _temp829.curr= _temp828; _temp829.base= _temp828;
_temp829.last_plus_one= _temp828 + 44; _temp829;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp830=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp830->hd=( void*) e; _temp830->tl= 0; _temp830;}), 0),
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
_temp831= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp837; struct Cyc_Absyn_Conref*
_temp839; struct Cyc_Absyn_Tqual _temp841; struct Cyc_Absyn_Conref* _temp843;
void* _temp845; void* _temp847; _LL833: if(( unsigned int) _temp831 > 4u?*(( int*)
_temp831) == Cyc_Absyn_PointerType: 0){ _LL838: _temp837=(( struct Cyc_Absyn_PointerType_struct*)
_temp831)->f1; _LL848: _temp847=( void*) _temp837.elt_typ; goto _LL846; _LL846:
_temp845=( void*) _temp837.rgn_typ; goto _LL844; _LL844: _temp843= _temp837.nullable;
goto _LL842; _LL842: _temp841= _temp837.tq; goto _LL840; _LL840: _temp839=
_temp837.bounds; goto _LL834;} else{ goto _LL835;} _LL835: goto _LL836; _LL834:
elt_typ= _temp847; elt_tqual= _temp841; strt_typ= Cyc_Toc_add_tagged_ptr_type(
_temp847, _temp841); goto _LL832; _LL836: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp849=(
unsigned char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp850;
_temp850.curr= _temp849; _temp850.base= _temp849; _temp850.last_plus_one=
_temp849 + 28; _temp850;})); _LL832:;} if( toplevel){ int is_string= 0;{ void*
_temp851=( void*) e->r; void* _temp857; struct _tagged_string _temp859; _LL853:
if(*(( int*) _temp851) == Cyc_Absyn_Const_e){ _LL858: _temp857=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp851)->f1; if(( unsigned int) _temp857 > 1u?*((
int*) _temp857) == Cyc_Absyn_String_c: 0){ _LL860: _temp859=(( struct Cyc_Absyn_String_c_struct*)
_temp857)->f1; goto _LL854;} else{ goto _LL855;}} else{ goto _LL855;} _LL855:
goto _LL856; _LL854: is_string= 1; goto _LL852; _LL856: goto _LL852; _LL852:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp864=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp864[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp865; _temp865.tag= Cyc_Absyn_ArrayType; _temp865.f1=(
void*) elt_typ; _temp865.f2= Cyc_Toc_mt_tq; _temp865.f3=( struct Cyc_Absyn_Exp*)
sz; _temp865;}); _temp864;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp861=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp861->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp862=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp862[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp863; _temp863.tag= Cyc_Absyn_Var_d;
_temp863.f1= vd; _temp863;}); _temp862;}), 0); _temp861->tl= Cyc_Toc_result_decls;
_temp861;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ if( elt_tqual.q_const){ xexp=
Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq), e, 0);} else{
xexp= e;}}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp( xexp, sz, 0);
struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(*
_temp866)( struct _tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4))
Cyc_List_list; struct _tuple4* _temp869=({ struct _tuple4* _temp874=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp874->f1= 0; _temp874->f2=
xexp; _temp874;}); struct _tuple4* _temp870=({ struct _tuple4* _temp873=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp873->f1= 0; _temp873->f2=
xexp; _temp873;}); struct _tuple4* _temp871=({ struct _tuple4* _temp872=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp872->f1= 0; _temp872->f2=
xplussz; _temp872;}); struct _tuple4* _temp867[ 3u]={ _temp869, _temp870,
_temp871}; struct _tagged_ptr4 _temp868={ _temp867, _temp867, _temp867 + 3u};
_temp866( _temp868);}), 0); return urm_exp;}}} else{ struct Cyc_Absyn_Stmt* s=
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
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp875=( unsigned char*)"can't coerce t? to t* or t@ at the top-level";
struct _tagged_string _temp876; _temp876.curr= _temp875; _temp876.base= _temp875;
_temp876.last_plus_one= _temp875 + 45; _temp876;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
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
return({ struct Cyc_Toc_Env* _temp877=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp877->break_lab= 0; _temp877->continue_lab= 0;
_temp877->fallthru_info= 0; _temp877->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp877->toplevel= 1; _temp877;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp878=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp878->break_lab= e->break_lab;
_temp878->continue_lab= e->continue_lab; _temp878->fallthru_info= e->fallthru_info;
_temp878->varmap= e->varmap; _temp878->toplevel= e->toplevel; _temp878;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp879=(* x).f1; struct Cyc_List_List*
_temp885; _LL881: if(( unsigned int) _temp879 > 1u?*(( int*) _temp879) == Cyc_Absyn_Rel_n:
0){ _LL886: _temp885=(( struct Cyc_Absyn_Rel_n_struct*) _temp879)->f1; goto
_LL882;} else{ goto _LL883;} _LL883: goto _LL884; _LL882:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp887=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp887[ 0]=({ struct
Cyc_Core_Impossible_struct _temp888; _temp888.tag= Cyc_Core_Impossible; _temp888.f1=(
struct _tagged_string)({ struct _tagged_string _temp889= Cyc_Absynpp_qvar2string(
x); xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp889.last_plus_one -
_temp889.curr, _temp889.curr);}); _temp888;}); _temp887;})); _LL884: goto _LL880;
_LL880:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp*
data)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env*
Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab= 0; ans->continue_lab= 0; return ans;} struct _tuple8{ struct
_tagged_string* f1; struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ;
static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_string* break_l, struct _tagged_string* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
_temp890= 0; goto _LL891; _LL891: for( 0; fallthru_binders != 0;
fallthru_binders=(( struct Cyc_List_List*) _check_null( fallthru_binders))->tl){
_temp890=({ struct Cyc_List_List* _temp892=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp892->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp892->tl=
_temp890; _temp892;});} _temp890=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp890);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp893=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp893->v=( void*) break_l; _temp893;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp894=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp894->v=( void*)({ struct _tuple8*
_temp895=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp895->f1=
fallthru_l; _temp895->f2= _temp890; _temp895->f3= next_case_env->varmap;
_temp895;}); _temp894;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp896=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp896->v=( void*)
break_l; _temp896;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_string*
next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp897=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp897->v=( void*)({ struct _tuple8* _temp898=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp898->f1= next_l; _temp898->f2= 0;
_temp898->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp898;}); _temp897;});
return ans;} struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tagged_ptr5{ struct Cyc_List_List** curr; struct Cyc_List_List** base;
struct Cyc_List_List** last_plus_one; } ; struct _tuple10{ struct _tuple0* f1;
void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp899=( unsigned char*)"expecting a literal format string";
struct _tagged_string _temp900; _temp900.curr= _temp899; _temp900.base= _temp899;
_temp900.last_plus_one= _temp899 + 34; _temp900;}));}{ struct _tagged_string
fmt_str;{ void* _temp901=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd)->r; void* _temp907; struct _tagged_string _temp909;
_LL903: if(*(( int*) _temp901) == Cyc_Absyn_Const_e){ _LL908: _temp907=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp901)->f1; if(( unsigned int) _temp907 > 1u?*((
int*) _temp907) == Cyc_Absyn_String_c: 0){ _LL910: _temp909=(( struct Cyc_Absyn_String_c_struct*)
_temp907)->f1; goto _LL904;} else{ goto _LL905;}} else{ goto _LL905;} _LL905:
goto _LL906; _LL904: fmt_str= _temp909; goto _LL902; _LL906: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp911=(
unsigned char*)"expecting a literal format string"; struct _tagged_string
_temp912; _temp912.curr= _temp911; _temp912.base= _temp911; _temp912.last_plus_one=
_temp911 + 34; _temp912;})); _LL902:;}{ int len=( int)({ struct _tagged_string
_temp1008= fmt_str;( unsigned int)( _temp1008.last_plus_one - _temp1008.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str,
sizeof( unsigned char), i) =='%'){ i ++; if( i < len?*(( const unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( unsigned char), i) =='s': 0){ break;}}} if( i == len){ if( aopt
!= 0){ args=({ struct Cyc_List_List* _temp913=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp913->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp913->tl= args; _temp913;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp914= _new_region(); struct
_RegionHandle* r=& _temp914; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str,
sizeof( unsigned char), i); rev_fmt=({ struct Cyc_List_List* _temp915=( struct
Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List)); _temp915->hd=(
void*)(( int) c); _temp915->tl= rev_fmt; _temp915;}); if( c !='%'){ continue;}{
struct Cyc_Core_Opt* _temp916= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); goto _LL917; _LL917: if( _temp916 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp918=( unsigned char*)"bad format string";
struct _tagged_string _temp919; _temp919.curr= _temp918; _temp919.base= _temp918;
_temp919.last_plus_one= _temp918 + 18; _temp919;}));}{ struct _tuple9 _temp922;
int _temp923; unsigned char _temp925; struct Cyc_List_List* _temp927; struct Cyc_List_List*
_temp929; struct Cyc_List_List* _temp931; struct Cyc_List_List* _temp933; struct
_tuple9* _temp920=( struct _tuple9*)(( struct Cyc_Core_Opt*) _check_null(
_temp916))->v; _temp922=* _temp920; _LL934: _temp933= _temp922.f1; goto _LL932;
_LL932: _temp931= _temp922.f2; goto _LL930; _LL930: _temp929= _temp922.f3; goto
_LL928; _LL928: _temp927= _temp922.f4; goto _LL926; _LL926: _temp925= _temp922.f5;
goto _LL924; _LL924: _temp923= _temp922.f6; goto _LL921; _LL921: i= _temp923 - 1;
if( _temp925 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp935)( struct _RegionHandle*, struct
_tagged_ptr5)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_ptr5)) Cyc_List_rlist; struct _RegionHandle* _temp942= r; struct Cyc_List_List*
_temp938= _temp933; struct Cyc_List_List* _temp939= _temp931; struct Cyc_List_List*
_temp940= _temp929; struct Cyc_List_List* _temp941= _temp927; struct Cyc_List_List*
_temp936[ 4u]={ _temp938, _temp939, _temp940, _temp941}; struct _tagged_ptr5
_temp937={ _temp936, _temp936, _temp936 + 4u}; _temp935( _temp942, _temp937);})),
rev_fmt); rev_fmt=({ struct Cyc_List_List* _temp943=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp943->hd=( void*)(( int)
_temp925); _temp943->tl= rev_fmt; _temp943;});{ struct Cyc_List_List* _temp944=
_temp931; struct Cyc_List_List _temp951; struct Cyc_List_List* _temp952; int
_temp954; _LL946: if( _temp944 == 0){ goto _LL948;} else{ _temp951=* _temp944;
_LL955: _temp954=( int) _temp951.hd; goto _LL953; _LL953: _temp952= _temp951.tl;
if( _temp952 == 0){ goto _LL950;} else{ goto _LL948;}} _LL950: if( _temp954 =='*'){
goto _LL947;} else{ goto _LL948;} _LL948: goto _LL949; _LL947: { struct _tuple0*
_temp956= Cyc_Toc_temp_var(); goto _LL957; _LL957: rev_temps=({ struct Cyc_List_List*
_temp958=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp958->hd=( void*)({ struct _tuple10* _temp959=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp959->f1= _temp956; _temp959->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp959->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp959;});
_temp958->tl= rev_temps; _temp958;}); rev_result=({ struct Cyc_List_List*
_temp960=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp960->hd=( void*) Cyc_Absyn_var_exp( _temp956, 0); _temp960->tl= rev_result;
_temp960;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL945;} _LL949: goto
_LL945; _LL945:;}{ struct Cyc_List_List* _temp961= _temp929; struct Cyc_List_List
_temp968; struct Cyc_List_List* _temp969; struct Cyc_List_List _temp971; struct
Cyc_List_List* _temp972; int _temp974; int _temp976; _LL963: if( _temp961 == 0){
goto _LL965;} else{ _temp968=* _temp961; _LL977: _temp976=( int) _temp968.hd;
goto _LL970; _LL970: _temp969= _temp968.tl; if( _temp969 == 0){ goto _LL965;}
else{ _temp971=* _temp969; _LL975: _temp974=( int) _temp971.hd; goto _LL973;
_LL973: _temp972= _temp971.tl; if( _temp972 == 0){ goto _LL967;} else{ goto
_LL965;}}} _LL967: if( _temp976 =='.'? _temp974 =='*': 0){ goto _LL964;} else{
goto _LL965;} _LL965: goto _LL966; _LL964: { struct _tuple0* _temp978= Cyc_Toc_temp_var();
goto _LL979; _LL979: rev_temps=({ struct Cyc_List_List* _temp980=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp980->hd=( void*)({
struct _tuple10* _temp981=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp981->f1= _temp978; _temp981->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp981->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp981;}); _temp980->tl= rev_temps; _temp980;});
rev_result=({ struct Cyc_List_List* _temp982=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp982->hd=( void*) Cyc_Absyn_var_exp(
_temp978, 0); _temp982->tl= rev_result; _temp982;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL962;} _LL966: goto _LL962; _LL962:;} if( _temp925 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ struct _tagged_string _temp983= Cyc_Absynpp_prim2string( p);
xprintf("missing arg to %.*s", _temp983.last_plus_one - _temp983.curr, _temp983.curr);}));}{
struct _tuple0* _temp984= Cyc_Toc_temp_var(); goto _LL985; _LL985: rev_temps=({
struct Cyc_List_List* _temp986=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp986->hd=( void*)({ struct _tuple10*
_temp987=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp987->f1= _temp984; _temp987->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp987->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp987;}); _temp986->tl= rev_temps; _temp986;});
rev_result=({ struct Cyc_List_List* _temp988=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp988->hd=( void*) Cyc_Absyn_var_exp(
_temp984, 0); _temp988->tl= rev_result; _temp988;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp989=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp989->hd=( void*)(( int)'.');
_temp989->tl= rev_fmt; _temp989;}); rev_fmt=({ struct Cyc_List_List* _temp990=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp990->hd=( void*)(( int)'*'); _temp990->tl= rev_fmt; _temp990;}); rev_fmt=({
struct Cyc_List_List* _temp991=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp991->hd=( void*)(( int)'s'); _temp991->tl=
rev_fmt; _temp991;});{ struct _tuple0* _temp992= Cyc_Toc_temp_var(); goto _LL993;
_LL993: rev_temps=({ struct Cyc_List_List* _temp994=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp994->hd=( void*)({
struct _tuple10* _temp995=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp995->f1= _temp992; _temp995->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp995->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp995;}); _temp994->tl= rev_temps; _temp994;});{
struct Cyc_Absyn_Exp* _temp996= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp992, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp992, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL997; _LL997: {
struct Cyc_Absyn_Exp* _temp998= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp992, 0), Cyc_Toc_curr_sp, 0); goto _LL999; _LL999: rev_result=({ struct Cyc_List_List*
_temp1000=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1000->hd=( void*) _temp996; _temp1000->tl= rev_result; _temp1000;});
rev_result=({ struct Cyc_List_List* _temp1001=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1001->hd=( void*)
_temp998; _temp1001->tl= rev_result; _temp1001;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}}{
struct Cyc_List_List* _temp1003=({ struct Cyc_List_List* _temp1002=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1002->hd=( void*) Cyc_Absyn_string_exp((
struct _tagged_string) Cyc_String_implode((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0); _temp1002->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_result); _temp1002;}); goto _LL1004;
_LL1004: if( aopt != 0){ _temp1003=({ struct Cyc_List_List* _temp1005=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1005->hd=( void*)((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( aopt))->v);
_temp1005->tl= _temp1003; _temp1005;});}{ struct Cyc_Absyn_Exp* _temp1006= Cyc_Absyn_primop_exp(
p, _temp1003, 0); goto _LL1007; _LL1007: s= Cyc_Absyn_exp_stmt( _temp1006, 0);
for( 0; rev_temps != 0; rev_temps=(( struct Cyc_List_List*) _check_null(
rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f1,(*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f2,( struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)((
struct Cyc_List_List*) _check_null( rev_temps))->hd)).f3, s, 0);}}}};
_pop_region(& _temp914);} return Cyc_Absyn_stmt_exp( s, 0);}}}} static void* Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1009=( unsigned char*)"Missing type in primop ";
struct _tagged_string _temp1010; _temp1010.curr= _temp1009; _temp1010.base=
_temp1009; _temp1010.last_plus_one= _temp1009 + 24; _temp1010;}));} return Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1011=( unsigned char*)"Missing type in primop ";
struct _tagged_string _temp1012; _temp1012.curr= _temp1011; _temp1012.base=
_temp1011; _temp1012.last_plus_one= _temp1011 + 24; _temp1012;}));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1013=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1013->f1= Cyc_Toc_mt_tq; _temp1013->f2=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp1013;});} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp1014=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1014[
0]=({ struct _tuple4 _temp1015; _temp1015.f1= 0; _temp1015.f2= e; _temp1015;});
_temp1014;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
_temp1016= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); goto _LL1017; _LL1017: if(
rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic(
_temp1016): Cyc_Toc_malloc_ptr( _temp1016), 0);} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c( nv, r); eo=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp( r, _temp1016),
0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp1018=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL1019; _LL1019: for( 0;
_temp1018 != 0; _temp1018=(( struct Cyc_List_List*) _check_null( _temp1018))->tl){
struct _tuple4 _temp1022; struct Cyc_Absyn_Exp* _temp1023; struct Cyc_List_List*
_temp1025; struct _tuple4* _temp1020=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1018))->hd; _temp1022=* _temp1020; _LL1026: _temp1025=
_temp1022.f1; goto _LL1024; _LL1024: _temp1023= _temp1022.f2; goto _LL1021;
_LL1021: { struct Cyc_Absyn_Exp* e_index; if( _temp1025 == 0){ e_index= Cyc_Absyn_signed_int_exp(
count --, 0);} else{ if((( struct Cyc_List_List*) _check_null( _temp1025))->tl
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp1027=( unsigned char*)"multiple designators in array"; struct
_tagged_string _temp1028; _temp1028.curr= _temp1027; _temp1028.base= _temp1027;
_temp1028.last_plus_one= _temp1027 + 30; _temp1028;}));}{ void* _temp1029=( void*)((
struct Cyc_List_List*) _check_null( _temp1025))->hd; goto _LL1030; _LL1030: {
void* _temp1031= _temp1029; struct Cyc_Absyn_Exp* _temp1037; struct
_tagged_string* _temp1039; _LL1033: if(*(( int*) _temp1031) == Cyc_Absyn_ArrayElement){
_LL1038: _temp1037=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1031)->f1;
goto _LL1034;} else{ goto _LL1035;} _LL1035: if(*(( int*) _temp1031) == Cyc_Absyn_FieldName){
_LL1040: _temp1039=(( struct Cyc_Absyn_FieldName_struct*) _temp1031)->f1; goto
_LL1036;} else{ goto _LL1032;} _LL1034: Cyc_Toc_exp_to_c( nv, _temp1037);
e_index= _temp1037; goto _LL1032; _LL1036: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp1041=(
unsigned char*)"field name designators in array"; struct _tagged_string
_temp1042; _temp1042.curr= _temp1041; _temp1042.base= _temp1041; _temp1042.last_plus_one=
_temp1041 + 32; _temp1042;})); goto _LL1032; _LL1032:;}}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp1043=( void*)
_temp1023->r; struct Cyc_List_List* _temp1053; struct Cyc_Absyn_Exp* _temp1055;
struct Cyc_Absyn_Exp* _temp1057; struct Cyc_Absyn_Vardecl* _temp1059; struct Cyc_List_List*
_temp1061; void* _temp1063; _LL1045: if(*(( int*) _temp1043) == Cyc_Absyn_Array_e){
_LL1054: _temp1053=(( struct Cyc_Absyn_Array_e_struct*) _temp1043)->f1; goto
_LL1046;} else{ goto _LL1047;} _LL1047: if(*(( int*) _temp1043) == Cyc_Absyn_Comprehension_e){
_LL1060: _temp1059=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1043)->f1;
goto _LL1058; _LL1058: _temp1057=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1043)->f2; goto _LL1056; _LL1056: _temp1055=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1043)->f3; goto _LL1048;} else{ goto _LL1049;} _LL1049: if(*(( int*)
_temp1043) == Cyc_Absyn_AnonStruct_e){ _LL1064: _temp1063=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1043)->f1; goto _LL1062; _LL1062: _temp1061=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1043)->f2; goto _LL1050;} else{ goto _LL1051;} _LL1051: goto _LL1052;
_LL1046: s= Cyc_Toc_init_array( nv, lval, _temp1053, s); goto _LL1044; _LL1048:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1059, _temp1057, _temp1055, s, 0);
goto _LL1044; _LL1050: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1063,
_temp1061, s); goto _LL1044; _LL1052: Cyc_Toc_exp_to_c( nv, _temp1023); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1023, 0),
s, 0); goto _LL1044; _LL1044:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* _temp1065= vd->name; goto _LL1066;
_LL1066: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1065, Cyc_Absyn_varb_exp( _temp1065,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1089=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1089[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1090; _temp1090.tag= Cyc_Absyn_Local_b; _temp1090.f1=
vd; _temp1090;}); _temp1089;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp1065, 0),
Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( _temp1065, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp*
ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( _temp1065, 0), 0); struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( _temp1065, 0), 0); struct
Cyc_Absyn_Stmt* body;{ void* _temp1067=( void*) e2->r; struct Cyc_List_List*
_temp1077; struct Cyc_Absyn_Exp* _temp1079; struct Cyc_Absyn_Exp* _temp1081;
struct Cyc_Absyn_Vardecl* _temp1083; struct Cyc_List_List* _temp1085; void*
_temp1087; _LL1069: if(*(( int*) _temp1067) == Cyc_Absyn_Array_e){ _LL1078:
_temp1077=(( struct Cyc_Absyn_Array_e_struct*) _temp1067)->f1; goto _LL1070;}
else{ goto _LL1071;} _LL1071: if(*(( int*) _temp1067) == Cyc_Absyn_Comprehension_e){
_LL1084: _temp1083=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1067)->f1;
goto _LL1082; _LL1082: _temp1081=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1067)->f2; goto _LL1080; _LL1080: _temp1079=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1067)->f3; goto _LL1072;} else{ goto _LL1073;} _LL1073: if(*(( int*)
_temp1067) == Cyc_Absyn_AnonStruct_e){ _LL1088: _temp1087=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1067)->f1; goto _LL1086; _LL1086: _temp1085=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1067)->f2; goto _LL1074;} else{ goto _LL1075;} _LL1075: goto _LL1076;
_LL1070: body= Cyc_Toc_init_array( nv2, lval, _temp1077, Cyc_Absyn_skip_stmt( 0));
goto _LL1068; _LL1072: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1083,
_temp1081, _temp1079, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1068; _LL1074: body=
Cyc_Toc_init_anon_struct( nv, lval, _temp1087, _temp1085, Cyc_Absyn_skip_stmt( 0));
goto _LL1068; _LL1076: Cyc_Toc_exp_to_c( nv2, e2); body= Cyc_Absyn_assign_stmt(
lval, e2, 0); goto _LL1068; _LL1068:;}{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt(
ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( _temp1065, Cyc_Absyn_uint_t,
0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s){{ struct Cyc_List_List* _temp1091=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto _LL1092; _LL1092: for( 0;
_temp1091 != 0; _temp1091=(( struct Cyc_List_List*) _check_null( _temp1091))->tl){
struct _tuple4 _temp1095; struct Cyc_Absyn_Exp* _temp1096; struct Cyc_List_List*
_temp1098; struct _tuple4* _temp1093=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1091))->hd; _temp1095=* _temp1093; _LL1099: _temp1098=
_temp1095.f1; goto _LL1097; _LL1097: _temp1096= _temp1095.f2; goto _LL1094;
_LL1094: if( _temp1098 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1100=( unsigned char*)"empty designator list"; struct
_tagged_string _temp1101; _temp1101.curr= _temp1100; _temp1101.base= _temp1100;
_temp1101.last_plus_one= _temp1100 + 22; _temp1101;}));} if((( struct Cyc_List_List*)
_check_null( _temp1098))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1102=( unsigned char*)"too many designators in anonymous struct";
struct _tagged_string _temp1103; _temp1103.curr= _temp1102; _temp1103.base=
_temp1102; _temp1103.last_plus_one= _temp1102 + 41; _temp1103;}));}{ void*
_temp1104=( void*)(( struct Cyc_List_List*) _check_null( _temp1098))->hd; struct
_tagged_string* _temp1110; _LL1106: if(*(( int*) _temp1104) == Cyc_Absyn_FieldName){
_LL1111: _temp1110=(( struct Cyc_Absyn_FieldName_struct*) _temp1104)->f1; goto
_LL1107;} else{ goto _LL1108;} _LL1108: goto _LL1109; _LL1107: { struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_structmember_exp( lhs, _temp1110, 0);{ void* _temp1112=( void*)
_temp1096->r; struct Cyc_List_List* _temp1122; struct Cyc_Absyn_Exp* _temp1124;
struct Cyc_Absyn_Exp* _temp1126; struct Cyc_Absyn_Vardecl* _temp1128; struct Cyc_List_List*
_temp1130; void* _temp1132; _LL1114: if(*(( int*) _temp1112) == Cyc_Absyn_Array_e){
_LL1123: _temp1122=(( struct Cyc_Absyn_Array_e_struct*) _temp1112)->f1; goto
_LL1115;} else{ goto _LL1116;} _LL1116: if(*(( int*) _temp1112) == Cyc_Absyn_Comprehension_e){
_LL1129: _temp1128=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1112)->f1;
goto _LL1127; _LL1127: _temp1126=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1112)->f2; goto _LL1125; _LL1125: _temp1124=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1112)->f3; goto _LL1117;} else{ goto _LL1118;} _LL1118: if(*(( int*)
_temp1112) == Cyc_Absyn_AnonStruct_e){ _LL1133: _temp1132=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1112)->f1; goto _LL1131; _LL1131: _temp1130=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1112)->f2; goto _LL1119;} else{ goto _LL1120;} _LL1120: goto _LL1121;
_LL1115: s= Cyc_Toc_init_array( nv, lval, _temp1122, s); goto _LL1113; _LL1117:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1128, _temp1126, _temp1124, s, 0);
goto _LL1113; _LL1119: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1132,
_temp1130, s); goto _LL1113; _LL1121: Cyc_Toc_exp_to_c( nv, _temp1096); if( Cyc_Toc_is_poly_field(
struct_type, _temp1110)){ _temp1096= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1096, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1096, 0), 0), s, 0); goto _LL1113; _LL1113:;} goto _LL1105;} _LL1109:(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1134=( unsigned char*)"array designator in struct"; struct
_tagged_string _temp1135; _temp1135.curr= _temp1134; _temp1135.base= _temp1134;
_temp1135.last_plus_one= _temp1134 + 27; _temp1135;}))); _LL1105:;}}} return s;}
static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int
pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
_RegionHandle _temp1136= _new_region(); struct _RegionHandle* r=& _temp1136;
_push_region( r);{ struct Cyc_List_List* _temp1137=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c, es); goto _LL1138; _LL1138: { struct
_tagged_string* _temp1139= Cyc_Toc_add_tuple_type( _temp1137); goto _LL1140;
_LL1140: { struct _tuple0* _temp1141= Cyc_Toc_temp_var(); goto _LL1142; _LL1142: {
struct Cyc_Absyn_Exp* _temp1143= Cyc_Absyn_var_exp( _temp1141, 0); goto _LL1144;
_LL1144: { struct Cyc_Absyn_Stmt* _temp1145= Cyc_Absyn_exp_stmt( _temp1143, 0);
goto _LL1146; _LL1146: { struct Cyc_Absyn_Exp*(* _temp1147)( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; goto _LL1148; _LL1148: { int is_atomic= 1; struct
Cyc_List_List* _temp1149=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( r, es); goto _LL1150; _LL1150:{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1149); for( 0;
_temp1149 != 0;( _temp1149=(( struct Cyc_List_List*) _check_null( _temp1149))->tl,
-- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1149))->hd; struct Cyc_Absyn_Exp* lval= _temp1147( _temp1143,
Cyc_Toc_fieldname( i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v): 0;{ void* _temp1151=( void*) e->r;
struct Cyc_List_List* _temp1159; struct Cyc_Absyn_Exp* _temp1161; struct Cyc_Absyn_Exp*
_temp1163; struct Cyc_Absyn_Vardecl* _temp1165; _LL1153: if(*(( int*) _temp1151)
== Cyc_Absyn_Array_e){ _LL1160: _temp1159=(( struct Cyc_Absyn_Array_e_struct*)
_temp1151)->f1; goto _LL1154;} else{ goto _LL1155;} _LL1155: if(*(( int*)
_temp1151) == Cyc_Absyn_Comprehension_e){ _LL1166: _temp1165=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1151)->f1; goto _LL1164; _LL1164: _temp1163=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1151)->f2; goto _LL1162; _LL1162: _temp1161=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1151)->f3; goto _LL1156;} else{ goto _LL1157;} _LL1157: goto _LL1158;
_LL1154: _temp1145= Cyc_Toc_init_array( nv, lval, _temp1159, _temp1145); goto
_LL1152; _LL1156: _temp1145= Cyc_Toc_init_comprehension( nv, lval, _temp1165,
_temp1163, _temp1161, _temp1145, 0); goto _LL1152; _LL1158: Cyc_Toc_exp_to_c( nv,
e); _temp1145= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1147( _temp1143, Cyc_Toc_fieldname( i), 0), e, 0), 0), _temp1145, 0); goto
_LL1152; _LL1152:;}}}{ struct Cyc_Absyn_Exp* _temp1167= Cyc_Toc_make_struct( nv,
_temp1141, Cyc_Absyn_strct( _temp1139), _temp1145, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp1167;}}}}}}}}; _pop_region(& _temp1136);} static
struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env* nv, void*
struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp1168= Cyc_Toc_temp_var(); goto
_LL1169; _LL1169: { struct Cyc_Absyn_Exp* _temp1170= Cyc_Absyn_var_exp(
_temp1168, 0); goto _LL1171; _LL1171: { struct Cyc_Absyn_Stmt* _temp1172= Cyc_Absyn_exp_stmt(
_temp1170, 0); goto _LL1173; _LL1173: { struct Cyc_Absyn_Exp*(* _temp1174)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1175;
_LL1175: { int is_atomic= 1; struct _RegionHandle _temp1176= _new_region();
struct _RegionHandle* r=& _temp1176; _push_region( r);{ struct Cyc_List_List*
_temp1177=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, dles); goto _LL1178; _LL1178: for( 0; _temp1177 != 0;
_temp1177=(( struct Cyc_List_List*) _check_null( _temp1177))->tl){ struct
_tuple4 _temp1181; struct Cyc_Absyn_Exp* _temp1182; struct Cyc_List_List*
_temp1184; struct _tuple4* _temp1179=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1177))->hd; _temp1181=* _temp1179; _LL1185: _temp1184=
_temp1181.f1; goto _LL1183; _LL1183: _temp1182= _temp1181.f2; goto _LL1180;
_LL1180: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1182->topt))->v): 0; if( _temp1184 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1186=(
unsigned char*)"empty designator list"; struct _tagged_string _temp1187;
_temp1187.curr= _temp1186; _temp1187.base= _temp1186; _temp1187.last_plus_one=
_temp1186 + 22; _temp1187;}));} if((( struct Cyc_List_List*) _check_null(
_temp1184))->tl != 0){ struct _tuple0* _temp1188= Cyc_Toc_temp_var(); goto
_LL1189; _LL1189: { struct Cyc_Absyn_Exp* _temp1190= Cyc_Absyn_var_exp(
_temp1188, 0); goto _LL1191; _LL1191: for( 0; _temp1184 != 0; _temp1184=((
struct Cyc_List_List*) _check_null( _temp1184))->tl){ void* _temp1192=( void*)((
struct Cyc_List_List*) _check_null( _temp1184))->hd; struct _tagged_string*
_temp1198; _LL1194: if(*(( int*) _temp1192) == Cyc_Absyn_FieldName){ _LL1199:
_temp1198=(( struct Cyc_Absyn_FieldName_struct*) _temp1192)->f1; goto _LL1195;}
else{ goto _LL1196;} _LL1196: goto _LL1197; _LL1195: if( Cyc_Toc_is_poly_field(
struct_type, _temp1198)){ _temp1190= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1190, 0);} _temp1172= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1174( _temp1170, _temp1198, 0), _temp1190, 0), 0), _temp1172, 0); goto
_LL1193; _LL1197:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1200=( unsigned char*)"array designator in struct"; struct
_tagged_string _temp1201; _temp1201.curr= _temp1200; _temp1201.base= _temp1200;
_temp1201.last_plus_one= _temp1200 + 27; _temp1201;}))); _LL1193:;} Cyc_Toc_exp_to_c(
nv, _temp1182); _temp1172= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1190, _temp1182, 0), 0), _temp1172, 0);}} else{ void* _temp1202=( void*)((
struct Cyc_List_List*) _check_null( _temp1184))->hd; struct _tagged_string*
_temp1208; _LL1204: if(*(( int*) _temp1202) == Cyc_Absyn_FieldName){ _LL1209:
_temp1208=(( struct Cyc_Absyn_FieldName_struct*) _temp1202)->f1; goto _LL1205;}
else{ goto _LL1206;} _LL1206: goto _LL1207; _LL1205: { struct Cyc_Absyn_Exp*
lval= _temp1174( _temp1170, _temp1208, 0);{ void* _temp1210=( void*) _temp1182->r;
struct Cyc_List_List* _temp1220; struct Cyc_Absyn_Exp* _temp1222; struct Cyc_Absyn_Exp*
_temp1224; struct Cyc_Absyn_Vardecl* _temp1226; struct Cyc_List_List* _temp1228;
void* _temp1230; _LL1212: if(*(( int*) _temp1210) == Cyc_Absyn_Array_e){ _LL1221:
_temp1220=(( struct Cyc_Absyn_Array_e_struct*) _temp1210)->f1; goto _LL1213;}
else{ goto _LL1214;} _LL1214: if(*(( int*) _temp1210) == Cyc_Absyn_Comprehension_e){
_LL1227: _temp1226=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1210)->f1;
goto _LL1225; _LL1225: _temp1224=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1210)->f2; goto _LL1223; _LL1223: _temp1222=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1210)->f3; goto _LL1215;} else{ goto _LL1216;} _LL1216: if(*(( int*)
_temp1210) == Cyc_Absyn_AnonStruct_e){ _LL1231: _temp1230=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1210)->f1; goto _LL1229; _LL1229: _temp1228=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1210)->f2; goto _LL1217;} else{ goto _LL1218;} _LL1218: goto _LL1219;
_LL1213: _temp1172= Cyc_Toc_init_array( nv, lval, _temp1220, _temp1172); goto
_LL1211; _LL1215: _temp1172= Cyc_Toc_init_comprehension( nv, lval, _temp1226,
_temp1224, _temp1222, _temp1172, 0); goto _LL1211; _LL1217: _temp1172= Cyc_Toc_init_anon_struct(
nv, lval, _temp1230, _temp1228, _temp1172); goto _LL1211; _LL1219: Cyc_Toc_exp_to_c(
nv, _temp1182); if( Cyc_Toc_is_poly_field( struct_type, _temp1208)){ _temp1182=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1182, 0);} _temp1172= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1182, 0), 0), _temp1172, 0);
goto _LL1211; _LL1211:;} goto _LL1203;} _LL1207:( void) _throw((( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1232=(
unsigned char*)"array designator in struct"; struct _tagged_string _temp1233;
_temp1233.curr= _temp1232; _temp1233.base= _temp1232; _temp1233.last_plus_one=
_temp1232 + 27; _temp1233;}))); _LL1203:;}}}{ struct Cyc_Absyn_Exp* _temp1234=
Cyc_Toc_make_struct( nv, _temp1168, Cyc_Absyn_strctq( tdn), _temp1172, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp1234;}; _pop_region(&
_temp1176);}}}}} struct _tuple11{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp*
f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp*
el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2,
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp*
e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp1235=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp1235[ 0]=({
struct Cyc_Absyn_Increment_e_struct _temp1236; _temp1236.tag= Cyc_Absyn_Increment_e;
_temp1236.f1= e1; _temp1236.f2=( void*) incr; _temp1236;}); _temp1235;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp1237=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp1247; struct Cyc_Absyn_Exp*
_temp1249; void* _temp1251; struct _tagged_string* _temp1253; struct Cyc_Absyn_Exp*
_temp1255; _LL1239: if(*(( int*) _temp1237) == Cyc_Absyn_StmtExp_e){ _LL1248:
_temp1247=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1237)->f1; goto _LL1240;}
else{ goto _LL1241;} _LL1241: if(*(( int*) _temp1237) == Cyc_Absyn_Cast_e){
_LL1252: _temp1251=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1237)->f1;
goto _LL1250; _LL1250: _temp1249=(( struct Cyc_Absyn_Cast_e_struct*) _temp1237)->f2;
goto _LL1242;} else{ goto _LL1243;} _LL1243: if(*(( int*) _temp1237) == Cyc_Absyn_StructMember_e){
_LL1256: _temp1255=(( struct Cyc_Absyn_StructMember_e_struct*) _temp1237)->f1;
goto _LL1254; _LL1254: _temp1253=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1237)->f2; goto _LL1244;} else{ goto _LL1245;} _LL1245: goto _LL1246;
_LL1240:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1247, fs, f, f_env); goto _LL1238; _LL1242:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1249, fs, f, f_env); goto
_LL1238; _LL1244:( void*)( e1->r=( void*)(( void*) _temp1255->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1257=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1257->hd=( void*) _temp1253; _temp1257->tl= fs;
_temp1257;}), f, f_env); goto _LL1238; _LL1246: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
struct _tagged_string _temp1258= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1258.last_plus_one - _temp1258.curr, _temp1258.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1238;}
_LL1238:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1259=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1269; struct Cyc_Absyn_Stmt* _temp1271; struct Cyc_Absyn_Decl* _temp1273;
struct Cyc_Absyn_Stmt* _temp1275; struct Cyc_Absyn_Stmt* _temp1277; _LL1261: if((
unsigned int) _temp1259 > 1u?*(( int*) _temp1259) == Cyc_Absyn_Exp_s: 0){
_LL1270: _temp1269=(( struct Cyc_Absyn_Exp_s_struct*) _temp1259)->f1; goto
_LL1262;} else{ goto _LL1263;} _LL1263: if(( unsigned int) _temp1259 > 1u?*((
int*) _temp1259) == Cyc_Absyn_Decl_s: 0){ _LL1274: _temp1273=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1259)->f1; goto _LL1272; _LL1272: _temp1271=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1259)->f2; goto _LL1264;} else{ goto _LL1265;} _LL1265: if(( unsigned int)
_temp1259 > 1u?*(( int*) _temp1259) == Cyc_Absyn_Seq_s: 0){ _LL1278: _temp1277=((
struct Cyc_Absyn_Seq_s_struct*) _temp1259)->f1; goto _LL1276; _LL1276: _temp1275=((
struct Cyc_Absyn_Seq_s_struct*) _temp1259)->f2; goto _LL1266;} else{ goto
_LL1267;} _LL1267: goto _LL1268; _LL1262:(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1269, fs, f, f_env); goto
_LL1260; _LL1264:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1271, fs, f, f_env); goto _LL1260; _LL1266:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1275, fs, f, f_env);
goto _LL1260; _LL1268:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp1279= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1279.last_plus_one - _temp1279.curr,
_temp1279.curr);})); goto _LL1260; _LL1260:;} static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s); static struct Cyc_List_List*
Cyc_Toc_rmap_2c( struct _RegionHandle* r2, void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List* _temp1280=( struct
Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1280->hd=(
void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1280->tl=
0; _temp1280;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp1281=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1281->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1281->tl= 0;
_temp1281;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x))
Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1282=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1282->f1= 0; _temp1282->f2= e;
_temp1282;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){
void* _temp1283= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp1289;
_LL1285: if(( unsigned int) _temp1283 > 4u?*(( int*) _temp1283) == Cyc_Absyn_PointerType:
0){ _LL1290: _temp1289=(( struct Cyc_Absyn_PointerType_struct*) _temp1283)->f1;
goto _LL1286;} else{ goto _LL1287;} _LL1287: goto _LL1288; _LL1286: return
_temp1289; _LL1288:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp1291=( unsigned char*)"get_ptr_typ:  not a pointer!"; struct
_tagged_string _temp1292; _temp1292.curr= _temp1291; _temp1292.base= _temp1291;
_temp1292.last_plus_one= _temp1291 + 29; _temp1292;}))); _LL1284:;} struct
_tagged_ptr6{ struct _tuple0** curr; struct _tuple0** base; struct _tuple0**
last_plus_one; } ; struct _tagged_ptr7{ struct _tuple1** curr; struct _tuple1**
base; struct _tuple1** last_plus_one; } ; struct _tagged_ptr8{ struct Cyc_Absyn_Exp**
curr; struct Cyc_Absyn_Exp** base; struct Cyc_Absyn_Exp** last_plus_one; } ;
struct _tuple12{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){
void* _temp1293=( void*) e->r; goto _LL1294; _LL1294: if( e->topt == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1295= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1295.last_plus_one - _temp1295.curr, _temp1295.curr);})); return;}{ void*
old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void*
_temp1296= _temp1293; void* _temp1376; void* _temp1378; void* _temp1380; struct
_tuple0* _temp1382; struct _tuple0* _temp1384; struct Cyc_List_List* _temp1386;
void* _temp1388; void* _temp1390; struct Cyc_Absyn_Exp* _temp1392; struct Cyc_Absyn_Exp*
_temp1394; struct Cyc_Core_Opt* _temp1396; struct Cyc_Absyn_Exp* _temp1398;
struct Cyc_Absyn_Exp* _temp1400; struct Cyc_Absyn_Exp* _temp1402; struct Cyc_Absyn_Exp*
_temp1404; struct Cyc_Absyn_Exp* _temp1406; struct Cyc_Absyn_Exp* _temp1408;
struct Cyc_List_List* _temp1410; struct Cyc_Absyn_Exp* _temp1412; struct Cyc_Absyn_VarargCallInfo*
_temp1414; struct Cyc_List_List* _temp1416; struct Cyc_Absyn_Exp* _temp1418;
struct Cyc_Absyn_VarargCallInfo* _temp1420; struct Cyc_Absyn_VarargCallInfo
_temp1422; struct Cyc_Absyn_VarargInfo* _temp1423; struct Cyc_List_List*
_temp1425; int _temp1427; struct Cyc_List_List* _temp1429; struct Cyc_Absyn_Exp*
_temp1431; struct Cyc_Absyn_Exp* _temp1433; struct Cyc_Absyn_Exp* _temp1435;
struct Cyc_List_List* _temp1437; struct Cyc_Absyn_Exp* _temp1439; struct Cyc_Absyn_Exp*
_temp1441; void* _temp1443; void** _temp1445; struct Cyc_Absyn_Exp* _temp1446;
struct Cyc_Absyn_Exp* _temp1448; struct Cyc_Absyn_Exp* _temp1450; struct Cyc_Absyn_Exp*
_temp1452; void* _temp1454; struct _tagged_string* _temp1456; void* _temp1458;
struct Cyc_Absyn_Exp* _temp1460; struct _tagged_string* _temp1462; struct Cyc_Absyn_Exp*
_temp1464; struct _tagged_string* _temp1466; struct Cyc_Absyn_Exp* _temp1468;
struct Cyc_Absyn_Exp* _temp1470; struct Cyc_Absyn_Exp* _temp1472; struct Cyc_List_List*
_temp1474; struct Cyc_List_List* _temp1476; struct _tuple1* _temp1478; struct
Cyc_List_List* _temp1480; struct Cyc_Absyn_Exp* _temp1482; struct Cyc_Absyn_Exp*
_temp1484; struct Cyc_Absyn_Vardecl* _temp1486; struct Cyc_Absyn_Structdecl*
_temp1488; struct Cyc_List_List* _temp1490; struct Cyc_Core_Opt* _temp1492;
struct _tuple0* _temp1494; struct Cyc_List_List* _temp1496; void* _temp1498;
struct Cyc_Absyn_Tunionfield* _temp1500; struct Cyc_Absyn_Tuniondecl* _temp1502;
struct Cyc_List_List* _temp1504; struct Cyc_Core_Opt* _temp1506; struct Cyc_Core_Opt*
_temp1508; struct Cyc_Absyn_Tunionfield* _temp1510; struct Cyc_Absyn_Tuniondecl*
_temp1512; struct Cyc_List_List* _temp1514; struct Cyc_Core_Opt* _temp1516;
struct Cyc_Core_Opt* _temp1518; struct Cyc_Absyn_Enumfield* _temp1520; struct
Cyc_Absyn_Enumdecl* _temp1522; struct _tuple0* _temp1524; void* _temp1526;
struct Cyc_Absyn_Exp* _temp1528; struct Cyc_Absyn_Stmt* _temp1530; struct Cyc_List_List*
_temp1532; struct Cyc_Core_Opt* _temp1534; struct Cyc_Absyn_Fndecl* _temp1536;
struct Cyc_Absyn_Exp* _temp1538; _LL1298: if(*(( int*) _temp1296) == Cyc_Absyn_Const_e){
_LL1377: _temp1376=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1296)->f1;
if( _temp1376 ==( void*) Cyc_Absyn_Null_c){ goto _LL1299;} else{ goto _LL1300;}}
else{ goto _LL1300;} _LL1300: if(*(( int*) _temp1296) == Cyc_Absyn_Const_e){
_LL1379: _temp1378=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1296)->f1;
goto _LL1301;} else{ goto _LL1302;} _LL1302: if(*(( int*) _temp1296) == Cyc_Absyn_Var_e){
_LL1383: _temp1382=(( struct Cyc_Absyn_Var_e_struct*) _temp1296)->f1; goto
_LL1381; _LL1381: _temp1380=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1296)->f2;
goto _LL1303;} else{ goto _LL1304;} _LL1304: if(*(( int*) _temp1296) == Cyc_Absyn_UnknownId_e){
_LL1385: _temp1384=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1296)->f1; goto
_LL1305;} else{ goto _LL1306;} _LL1306: if(*(( int*) _temp1296) == Cyc_Absyn_Primop_e){
_LL1389: _temp1388=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1296)->f1;
goto _LL1387; _LL1387: _temp1386=(( struct Cyc_Absyn_Primop_e_struct*) _temp1296)->f2;
goto _LL1307;} else{ goto _LL1308;} _LL1308: if(*(( int*) _temp1296) == Cyc_Absyn_Increment_e){
_LL1393: _temp1392=(( struct Cyc_Absyn_Increment_e_struct*) _temp1296)->f1; goto
_LL1391; _LL1391: _temp1390=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1296)->f2; goto _LL1309;} else{ goto _LL1310;} _LL1310: if(*(( int*)
_temp1296) == Cyc_Absyn_AssignOp_e){ _LL1399: _temp1398=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1296)->f1; goto _LL1397; _LL1397: _temp1396=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1296)->f2; goto _LL1395; _LL1395: _temp1394=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1296)->f3; goto _LL1311;} else{ goto _LL1312;} _LL1312: if(*(( int*)
_temp1296) == Cyc_Absyn_Conditional_e){ _LL1405: _temp1404=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1296)->f1; goto _LL1403; _LL1403: _temp1402=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1296)->f2; goto _LL1401; _LL1401: _temp1400=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1296)->f3; goto _LL1313;} else{ goto _LL1314;} _LL1314: if(*(( int*)
_temp1296) == Cyc_Absyn_SeqExp_e){ _LL1409: _temp1408=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1296)->f1; goto _LL1407; _LL1407: _temp1406=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1296)->f2; goto _LL1315;} else{ goto _LL1316;} _LL1316: if(*(( int*)
_temp1296) == Cyc_Absyn_UnknownCall_e){ _LL1413: _temp1412=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1296)->f1; goto _LL1411; _LL1411: _temp1410=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1296)->f2; goto _LL1317;} else{ goto _LL1318;} _LL1318: if(*(( int*)
_temp1296) == Cyc_Absyn_FnCall_e){ _LL1419: _temp1418=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1296)->f1; goto _LL1417; _LL1417: _temp1416=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1296)->f2; goto _LL1415; _LL1415: _temp1414=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1296)->f3; if( _temp1414 == 0){ goto _LL1319;} else{ goto _LL1320;}} else{
goto _LL1320;} _LL1320: if(*(( int*) _temp1296) == Cyc_Absyn_FnCall_e){ _LL1432:
_temp1431=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1296)->f1; goto _LL1430;
_LL1430: _temp1429=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1296)->f2; goto
_LL1421; _LL1421: _temp1420=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1296)->f3;
if( _temp1420 == 0){ goto _LL1322;} else{ _temp1422=* _temp1420; _LL1428:
_temp1427= _temp1422.num_varargs; goto _LL1426; _LL1426: _temp1425= _temp1422.injectors;
goto _LL1424; _LL1424: _temp1423= _temp1422.vai; goto _LL1321;}} else{ goto
_LL1322;} _LL1322: if(*(( int*) _temp1296) == Cyc_Absyn_Throw_e){ _LL1434:
_temp1433=(( struct Cyc_Absyn_Throw_e_struct*) _temp1296)->f1; goto _LL1323;}
else{ goto _LL1324;} _LL1324: if(*(( int*) _temp1296) == Cyc_Absyn_NoInstantiate_e){
_LL1436: _temp1435=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1296)->f1;
goto _LL1325;} else{ goto _LL1326;} _LL1326: if(*(( int*) _temp1296) == Cyc_Absyn_Instantiate_e){
_LL1440: _temp1439=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1296)->f1;
goto _LL1438; _LL1438: _temp1437=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1296)->f2; goto _LL1327;} else{ goto _LL1328;} _LL1328: if(*(( int*)
_temp1296) == Cyc_Absyn_Cast_e){ _LL1444: _temp1443=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1296)->f1; _temp1445=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1296)->f1); goto _LL1442; _LL1442: _temp1441=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1296)->f2; goto _LL1329;} else{ goto _LL1330;} _LL1330: if(*(( int*)
_temp1296) == Cyc_Absyn_Address_e){ _LL1447: _temp1446=(( struct Cyc_Absyn_Address_e_struct*)
_temp1296)->f1; goto _LL1331;} else{ goto _LL1332;} _LL1332: if(*(( int*)
_temp1296) == Cyc_Absyn_New_e){ _LL1451: _temp1450=(( struct Cyc_Absyn_New_e_struct*)
_temp1296)->f1; goto _LL1449; _LL1449: _temp1448=(( struct Cyc_Absyn_New_e_struct*)
_temp1296)->f2; goto _LL1333;} else{ goto _LL1334;} _LL1334: if(*(( int*)
_temp1296) == Cyc_Absyn_Sizeofexp_e){ _LL1453: _temp1452=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1296)->f1; goto _LL1335;} else{ goto _LL1336;} _LL1336: if(*(( int*)
_temp1296) == Cyc_Absyn_Sizeoftyp_e){ _LL1455: _temp1454=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1296)->f1; goto _LL1337;} else{ goto _LL1338;} _LL1338: if(*(( int*)
_temp1296) == Cyc_Absyn_Offsetof_e){ _LL1459: _temp1458=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1296)->f1; goto _LL1457; _LL1457: _temp1456=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1296)->f2; goto _LL1339;} else{ goto _LL1340;} _LL1340: if(*(( int*)
_temp1296) == Cyc_Absyn_Deref_e){ _LL1461: _temp1460=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1296)->f1; goto _LL1341;} else{ goto _LL1342;} _LL1342: if(*(( int*)
_temp1296) == Cyc_Absyn_StructMember_e){ _LL1465: _temp1464=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1296)->f1; goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1296)->f2; goto _LL1343;} else{ goto _LL1344;} _LL1344: if(*(( int*)
_temp1296) == Cyc_Absyn_StructArrow_e){ _LL1469: _temp1468=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1296)->f1; goto _LL1467; _LL1467: _temp1466=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1296)->f2; goto _LL1345;} else{ goto _LL1346;} _LL1346: if(*(( int*)
_temp1296) == Cyc_Absyn_Subscript_e){ _LL1473: _temp1472=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1296)->f1; goto _LL1471; _LL1471: _temp1470=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1296)->f2; goto _LL1347;} else{ goto _LL1348;} _LL1348: if(*(( int*)
_temp1296) == Cyc_Absyn_Tuple_e){ _LL1475: _temp1474=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1296)->f1; goto _LL1349;} else{ goto _LL1350;} _LL1350: if(*(( int*)
_temp1296) == Cyc_Absyn_CompoundLit_e){ _LL1479: _temp1478=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1296)->f1; goto _LL1477; _LL1477: _temp1476=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1296)->f2; goto _LL1351;} else{ goto _LL1352;} _LL1352: if(*(( int*)
_temp1296) == Cyc_Absyn_Array_e){ _LL1481: _temp1480=(( struct Cyc_Absyn_Array_e_struct*)
_temp1296)->f1; goto _LL1353;} else{ goto _LL1354;} _LL1354: if(*(( int*)
_temp1296) == Cyc_Absyn_Comprehension_e){ _LL1487: _temp1486=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1296)->f1; goto _LL1485; _LL1485: _temp1484=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1296)->f2; goto _LL1483; _LL1483: _temp1482=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1296)->f3; goto _LL1355;} else{ goto _LL1356;} _LL1356: if(*(( int*)
_temp1296) == Cyc_Absyn_Struct_e){ _LL1495: _temp1494=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1296)->f1; goto _LL1493; _LL1493: _temp1492=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1296)->f2; goto _LL1491; _LL1491: _temp1490=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1296)->f3; goto _LL1489; _LL1489: _temp1488=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1296)->f4; goto _LL1357;} else{ goto _LL1358;} _LL1358: if(*(( int*)
_temp1296) == Cyc_Absyn_AnonStruct_e){ _LL1499: _temp1498=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1296)->f1; goto _LL1497; _LL1497: _temp1496=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1296)->f2; goto _LL1359;} else{ goto _LL1360;} _LL1360: if(*(( int*)
_temp1296) == Cyc_Absyn_Tunion_e){ _LL1509: _temp1508=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1296)->f1; goto _LL1507; _LL1507: _temp1506=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1296)->f2; goto _LL1505; _LL1505: _temp1504=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1296)->f3; if( _temp1504 == 0){ goto _LL1503;} else{ goto _LL1362;} _LL1503:
_temp1502=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1296)->f4; goto _LL1501;
_LL1501: _temp1500=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1296)->f5; goto
_LL1361;} else{ goto _LL1362;} _LL1362: if(*(( int*) _temp1296) == Cyc_Absyn_Tunion_e){
_LL1519: _temp1518=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1296)->f1; goto
_LL1517; _LL1517: _temp1516=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1296)->f2;
goto _LL1515; _LL1515: _temp1514=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1296)->f3;
goto _LL1513; _LL1513: _temp1512=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1296)->f4;
goto _LL1511; _LL1511: _temp1510=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1296)->f5;
goto _LL1363;} else{ goto _LL1364;} _LL1364: if(*(( int*) _temp1296) == Cyc_Absyn_Enum_e){
_LL1525: _temp1524=(( struct Cyc_Absyn_Enum_e_struct*) _temp1296)->f1; goto
_LL1523; _LL1523: _temp1522=(( struct Cyc_Absyn_Enum_e_struct*) _temp1296)->f2;
goto _LL1521; _LL1521: _temp1520=(( struct Cyc_Absyn_Enum_e_struct*) _temp1296)->f3;
goto _LL1365;} else{ goto _LL1366;} _LL1366: if(*(( int*) _temp1296) == Cyc_Absyn_Malloc_e){
_LL1529: _temp1528=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1296)->f1; goto
_LL1527; _LL1527: _temp1526=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1296)->f2; goto _LL1367;} else{ goto _LL1368;} _LL1368: if(*(( int*)
_temp1296) == Cyc_Absyn_StmtExp_e){ _LL1531: _temp1530=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1296)->f1; goto _LL1369;} else{ goto _LL1370;} _LL1370: if(*(( int*)
_temp1296) == Cyc_Absyn_UnresolvedMem_e){ _LL1535: _temp1534=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1296)->f1; goto _LL1533; _LL1533: _temp1532=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1296)->f2; goto _LL1371;} else{ goto _LL1372;} _LL1372: if(*(( int*)
_temp1296) == Cyc_Absyn_Codegen_e){ _LL1537: _temp1536=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1296)->f1; goto _LL1373;} else{ goto _LL1374;} _LL1374: if(*(( int*)
_temp1296) == Cyc_Absyn_Fill_e){ _LL1539: _temp1538=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1296)->f1; goto _LL1375;} else{ goto _LL1297;} _LL1299:{ void* _temp1540=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1546; struct Cyc_Absyn_Conref*
_temp1548; struct Cyc_Absyn_Tqual _temp1550; struct Cyc_Absyn_Conref* _temp1552;
void* _temp1554; void* _temp1556; _LL1542: if(( unsigned int) _temp1540 > 4u?*((
int*) _temp1540) == Cyc_Absyn_PointerType: 0){ _LL1547: _temp1546=(( struct Cyc_Absyn_PointerType_struct*)
_temp1540)->f1; _LL1557: _temp1556=( void*) _temp1546.elt_typ; goto _LL1555;
_LL1555: _temp1554=( void*) _temp1546.rgn_typ; goto _LL1553; _LL1553: _temp1552=
_temp1546.nullable; goto _LL1551; _LL1551: _temp1550= _temp1546.tq; goto _LL1549;
_LL1549: _temp1548= _temp1546.bounds; goto _LL1543;} else{ goto _LL1544;}
_LL1544: goto _LL1545; _LL1543: if(( void*) _temp1548->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1541;}{ void* _temp1558=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1548); struct Cyc_Absyn_Exp* _temp1564; _LL1560: if(
_temp1558 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1561;} else{ goto _LL1562;}
_LL1562: if(( unsigned int) _temp1558 > 1u?*(( int*) _temp1558) == Cyc_Absyn_Upper_b:
0){ _LL1565: _temp1564=(( struct Cyc_Absyn_Upper_b_struct*) _temp1558)->f1; goto
_LL1563;} else{ goto _LL1559;} _LL1561: { struct _tuple4* _temp1567=({ struct
_tuple4* _temp1566=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1566->f1= 0; _temp1566->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1566;});
goto _LL1568; _LL1568: { struct Cyc_Absyn_Exp* _temp1575= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List*(* _temp1569)( struct _tagged_ptr4)=( struct Cyc_List_List*(*)(
struct _tagged_ptr4)) Cyc_List_list; struct _tuple4* _temp1572= _temp1567;
struct _tuple4* _temp1573= _temp1567; struct _tuple4* _temp1574= _temp1567;
struct _tuple4* _temp1570[ 3u]={ _temp1572, _temp1573, _temp1574}; struct
_tagged_ptr4 _temp1571={ _temp1570, _temp1570, _temp1570 + 3u}; _temp1569(
_temp1571);}), 0); goto _LL1576; _LL1576: if( nv->toplevel){( void*)( e->r=(
void*)(( void*) _temp1575->r));} else{ void* _temp1577= Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1556), Cyc_Toc_mt_tq); goto _LL1578; _LL1578: { struct
_tuple0* _temp1579= Cyc_Toc_temp_var(); goto _LL1580; _LL1580: { struct Cyc_Absyn_Exp*
_temp1581= Cyc_Absyn_var_exp( _temp1579, 0); goto _LL1582; _LL1582: { struct Cyc_Absyn_Stmt*
_temp1583= Cyc_Absyn_declare_stmt( _temp1579, _temp1577,( struct Cyc_Absyn_Exp*)
_temp1575, Cyc_Absyn_exp_stmt( _temp1581, 0), 0); goto _LL1584; _LL1584:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1583, 0))->r));}}}} goto
_LL1559;}} _LL1563:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0,
0))->r)); goto _LL1559; _LL1559:;} goto _LL1541; _LL1545:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1585= Cyc_Absynpp_typ2string( old_typ); xprintf("exp_to_c: null given type %.*s",
_temp1585.last_plus_one - _temp1585.curr, _temp1585.curr);})); goto _LL1541;
_LL1541:;} goto _LL1297; _LL1301: goto _LL1297; _LL1303:{ struct _handler_cons
_temp1586; _push_handler(& _temp1586);{ int _temp1588= 0; if( setjmp( _temp1586.handler)){
_temp1588= 1;} if( ! _temp1588){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1382))->r));; _pop_handler();} else{ void* _temp1587=( void*) _exn_thrown;
void* _temp1590= _temp1587; _LL1592: if( _temp1590 == Cyc_Dict_Absent){ goto
_LL1593;} else{ goto _LL1594;} _LL1594: goto _LL1595; _LL1593:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1596= Cyc_Absynpp_qvar2string( _temp1382); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1596.last_plus_one - _temp1596.curr, _temp1596.curr);})); return; _LL1595:(
void) _throw( _temp1590); _LL1591:;}}} goto _LL1297; _LL1305:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1597=(
unsigned char*)"unknownid"; struct _tagged_string _temp1598; _temp1598.curr=
_temp1597; _temp1598.base= _temp1597; _temp1598.last_plus_one= _temp1597 + 10;
_temp1598;})); goto _LL1297; _LL1307: { struct Cyc_List_List* _temp1599=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp1386); goto _LL1600; _LL1600: {
struct Cyc_List_List* _temp1601=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1386); goto _LL1602; _LL1602:(( void(*)( void(* f)( struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)(
Cyc_Toc_exp_to_c, nv, _temp1386);{ void* _temp1603= _temp1388; _LL1605: if(
_temp1603 ==( void*) Cyc_Absyn_Size){ goto _LL1606;} else{ goto _LL1607;}
_LL1607: if( _temp1603 ==( void*) Cyc_Absyn_Printf){ goto _LL1608;} else{ goto
_LL1609;} _LL1609: if( _temp1603 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1610;}
else{ goto _LL1611;} _LL1611: if( _temp1603 ==( void*) Cyc_Absyn_Fprintf){ goto
_LL1612;} else{ goto _LL1613;} _LL1613: if( _temp1603 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1614;} else{ goto _LL1615;} _LL1615: if( _temp1603 ==( void*) Cyc_Absyn_Plus){
goto _LL1616;} else{ goto _LL1617;} _LL1617: if( _temp1603 ==( void*) Cyc_Absyn_Minus){
goto _LL1618;} else{ goto _LL1619;} _LL1619: if( _temp1603 ==( void*) Cyc_Absyn_Eq){
goto _LL1620;} else{ goto _LL1621;} _LL1621: if( _temp1603 ==( void*) Cyc_Absyn_Neq){
goto _LL1622;} else{ goto _LL1623;} _LL1623: if( _temp1603 ==( void*) Cyc_Absyn_Gt){
goto _LL1624;} else{ goto _LL1625;} _LL1625: if( _temp1603 ==( void*) Cyc_Absyn_Gte){
goto _LL1626;} else{ goto _LL1627;} _LL1627: if( _temp1603 ==( void*) Cyc_Absyn_Lt){
goto _LL1628;} else{ goto _LL1629;} _LL1629: if( _temp1603 ==( void*) Cyc_Absyn_Lte){
goto _LL1630;} else{ goto _LL1631;} _LL1631: goto _LL1632; _LL1606: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1386))->hd;{
void* _temp1633=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1641; struct Cyc_Absyn_Tqual _temp1643; void*
_temp1645; struct Cyc_Absyn_PtrInfo _temp1647; struct Cyc_Absyn_Conref*
_temp1649; struct Cyc_Absyn_Tqual _temp1651; struct Cyc_Absyn_Conref* _temp1653;
void* _temp1655; void* _temp1657; _LL1635: if(( unsigned int) _temp1633 > 4u?*((
int*) _temp1633) == Cyc_Absyn_ArrayType: 0){ _LL1646: _temp1645=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1633)->f1; goto _LL1644; _LL1644:
_temp1643=(( struct Cyc_Absyn_ArrayType_struct*) _temp1633)->f2; goto _LL1642;
_LL1642: _temp1641=(( struct Cyc_Absyn_ArrayType_struct*) _temp1633)->f3; goto
_LL1636;} else{ goto _LL1637;} _LL1637: if(( unsigned int) _temp1633 > 4u?*((
int*) _temp1633) == Cyc_Absyn_PointerType: 0){ _LL1648: _temp1647=(( struct Cyc_Absyn_PointerType_struct*)
_temp1633)->f1; _LL1658: _temp1657=( void*) _temp1647.elt_typ; goto _LL1656;
_LL1656: _temp1655=( void*) _temp1647.rgn_typ; goto _LL1654; _LL1654: _temp1653=
_temp1647.nullable; goto _LL1652; _LL1652: _temp1651= _temp1647.tq; goto _LL1650;
_LL1650: _temp1649= _temp1647.bounds; goto _LL1638;} else{ goto _LL1639;}
_LL1639: goto _LL1640; _LL1636:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1641))->r)); goto _LL1634; _LL1638: { struct _tuple0*
_temp1659= Cyc_Toc_temp_var(); goto _LL1660; _LL1660: { void* _temp1661= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); goto _LL1662;
_LL1662: { struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1659, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1659, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4=
Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp( e2, e3, 0), 0);
struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1659, _temp1661,(
struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1634;}}} _LL1640:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
struct _tagged_string _temp1663= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp1601))->hd); struct _tagged_string _temp1664= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1663.last_plus_one - _temp1663.curr, _temp1663.curr, _temp1664.last_plus_one
- _temp1664.curr, _temp1664.curr);})); return; _LL1634:;} goto _LL1604;} _LL1608:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1388, 0, _temp1386,
_temp1601))->r)); goto _LL1604; _LL1610:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1388, 0, _temp1386, _temp1601))->r)); goto _LL1604; _LL1612: if( _temp1386
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char*
_temp1665=( unsigned char*)"fprintf without arguments"; struct _tagged_string
_temp1666; _temp1666.curr= _temp1665; _temp1666.base= _temp1665; _temp1666.last_plus_one=
_temp1665 + 26; _temp1666;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1388,({ struct Cyc_Core_Opt* _temp1667=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1667->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1386))->hd); _temp1667;}),(( struct Cyc_List_List*)
_check_null( _temp1386))->tl,(( struct Cyc_List_List*) _check_null( _temp1601))->tl))->r));
goto _LL1604; _LL1614: if( _temp1386 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1668=( unsigned char*)"sscanf without arguments";
struct _tagged_string _temp1669; _temp1669.curr= _temp1668; _temp1669.base=
_temp1668; _temp1669.last_plus_one= _temp1668 + 25; _temp1669;}));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1386))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1386))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1386))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1604; _LL1616: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1386))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1386))->tl))->hd; struct _tuple0* _temp1670= Cyc_Toc_temp_var(); goto
_LL1671; _LL1671: { void* _temp1672= Cyc_Toc_typ_to_c( old_typ); goto _LL1673;
_LL1673: { struct Cyc_Absyn_Stmt* _temp1674= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1670, 0), 0); goto _LL1675; _LL1675: { struct Cyc_Absyn_Exp* _temp1676= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1670, 0), Cyc_Toc_curr_sp, 0); goto _LL1677; _LL1677: {
struct Cyc_Absyn_Exp* _temp1679= Cyc_Absyn_assignop_exp( _temp1676,({ struct Cyc_Core_Opt*
_temp1678=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1678->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1678;}), e2, 0); goto
_LL1680; _LL1680: _temp1674= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1679,
0), _temp1674, 0); _temp1674= Cyc_Absyn_declare_stmt( _temp1670, _temp1672,(
struct Cyc_Absyn_Exp*) e1, _temp1674, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1674, 0))->r));}}}}} goto _LL1604; _LL1618: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null( _temp1599))->hd)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1386))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1386))->tl))->hd; void*
old_typ_e2=( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1599))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* _temp1681= Cyc_Toc_temp_var(); goto _LL1682;
_LL1682: { void* _temp1683= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_List_List*)
_check_null( _temp1599))->hd); goto _LL1684; _LL1684: { struct Cyc_Absyn_Stmt*
_temp1685= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1681, 0), 0); goto
_LL1686; _LL1686: { struct Cyc_Absyn_Exp* _temp1687= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1681, 0), Cyc_Toc_curr_sp, 0); goto _LL1688; _LL1688: {
struct Cyc_Absyn_Exp* _temp1690= Cyc_Absyn_assignop_exp( _temp1687,({ struct Cyc_Core_Opt*
_temp1689=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1689->v=( void*)(( void*) Cyc_Absyn_Minus); _temp1689;}), e2, 0); goto
_LL1691; _LL1691: _temp1685= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1690,
0), _temp1685, 0); _temp1685= Cyc_Absyn_declare_stmt( _temp1681, _temp1683,(
struct Cyc_Absyn_Exp*) e1, _temp1685, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1685, 0))->r));}}}}}} goto _LL1604; _LL1620: goto _LL1622; _LL1622: goto
_LL1624; _LL1624: goto _LL1626; _LL1626: goto _LL1628; _LL1628: goto _LL1630;
_LL1630: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1386))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1386))->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp1599))->hd; void* t2=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1599))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1604;} _LL1632: goto
_LL1604; _LL1604:;} goto _LL1297;}} _LL1309: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1392->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1392); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1390 ==(
void*) Cyc_Absyn_PreInc? 1: _temp1390 ==( void*) Cyc_Absyn_PreDec){ struct
_tuple0* _temp1692= Cyc_Toc_temp_var(); goto _LL1693; _LL1693: { void* _temp1694=
Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1695; _LL1695: { struct Cyc_Absyn_Stmt*
_temp1696= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1692,
0), 0), 0); goto _LL1697; _LL1697: { struct Cyc_Absyn_Exp* _temp1698= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1692, 0), Cyc_Toc_curr_sp, 0),
_temp1390, 0); goto _LL1699; _LL1699: _temp1696= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1698, 0), _temp1696, 0); _temp1696= Cyc_Absyn_declare_stmt( _temp1692,
_temp1694,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1392, 0),
_temp1696, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1696, 0))->r));}}}}
else{ struct _tuple0* _temp1700= Cyc_Toc_temp_var(); goto _LL1701; _LL1701: {
struct _tuple0* _temp1702= Cyc_Toc_temp_var(); goto _LL1703; _LL1703: { void*
_temp1704= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1705; _LL1705: { void*
_temp1706= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1707; _LL1707: { struct Cyc_Absyn_Stmt*
_temp1708= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1702, 0), 0); goto
_LL1709; _LL1709: { struct Cyc_Absyn_Exp* _temp1710= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1700, 0), Cyc_Toc_curr_sp, 0),
_temp1390, 0); goto _LL1711; _LL1711: _temp1708= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1710, 0), _temp1708, 0); _temp1708= Cyc_Absyn_declare_stmt( _temp1702,
_temp1704,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1700, 0), 0), _temp1708, 0); _temp1708= Cyc_Absyn_declare_stmt( _temp1700,
_temp1706,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1392, 0),
_temp1708, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1708, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1392)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1392, 0, Cyc_Toc_incr_lvalue,
_temp1390);( void*)( e->r=( void*)(( void*) _temp1392->r));}} goto _LL1297;}
_LL1311: { int e1_poly= Cyc_Toc_is_poly_project( _temp1398); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1398->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1394->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1398); Cyc_Toc_exp_to_c( nv, _temp1394); if(
_temp1396 != 0?( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1396))->v ==(
void*) Cyc_Absyn_Plus? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1396))->v
==( void*) Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){(
void*)( _temp1398->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1398->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1394->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1394->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1394->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1394->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1398)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1398, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1712=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1712[ 0]=({ struct _tuple11 _temp1713; _temp1713.f1=
_temp1396; _temp1713.f2= _temp1394; _temp1713;}); _temp1712;}));( void*)( e->r=(
void*)(( void*) _temp1398->r));} goto _LL1297;} _LL1313: Cyc_Toc_exp_to_c( nv,
_temp1404); Cyc_Toc_exp_to_c( nv, _temp1402); Cyc_Toc_exp_to_c( nv, _temp1400);
goto _LL1297; _LL1315: Cyc_Toc_exp_to_c( nv, _temp1408); Cyc_Toc_exp_to_c( nv,
_temp1406); goto _LL1297; _LL1317: _temp1418= _temp1412; _temp1416= _temp1410;
goto _LL1319; _LL1319: Cyc_Toc_exp_to_c( nv, _temp1418);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1416); goto _LL1297; _LL1321:{
struct _RegionHandle _temp1714= _new_region(); struct _RegionHandle* r=&
_temp1714; _push_region( r);{ void* _temp1715=( void*) _temp1423->type; goto
_LL1716; _LL1716: { void* _temp1717= Cyc_Toc_typ_to_c( _temp1715); goto _LL1718;
_LL1718: { void* _temp1719= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1431->topt))->v); goto _LL1720; _LL1720: { struct Cyc_Absyn_Exp*
_temp1721= Cyc_Absyn_uint_exp(( unsigned int) _temp1427, 0); goto _LL1722;
_LL1722: { void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1791=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1791[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1792; _temp1792.tag= Cyc_Absyn_ArrayType;
_temp1792.f1=( void*) _temp1715; _temp1792.f2= Cyc_Toc_mt_tq; _temp1792.f3=(
struct Cyc_Absyn_Exp*) _temp1721; _temp1792;}); _temp1791;})); struct _tuple0*
_temp1723= Cyc_Toc_temp_var(); goto _LL1724; _LL1724: { struct _tuple0*
_temp1725= Cyc_Toc_temp_var(); goto _LL1726; _LL1726: { struct Cyc_Absyn_Exp*
_temp1727= Cyc_Absyn_var_exp( _temp1725, 0); goto _LL1728; _LL1728: { struct
_tuple0* _temp1729= Cyc_Toc_temp_var(); goto _LL1730; _LL1730: { int _temp1731=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1429); goto _LL1732;
_LL1732: { int _temp1733= _temp1731 - _temp1427; goto _LL1734; _LL1734: { struct
_tagged_ptr6 vs=({ unsigned int _temp1787=( unsigned int) _temp1427; struct
_tuple0** _temp1788=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1787); struct _tagged_ptr6 _temp1790={ _temp1788, _temp1788,
_temp1788 + _temp1787};{ unsigned int _temp1789= _temp1787; unsigned int i; for(
i= 0; i < _temp1789; i ++){ _temp1788[ i]= Cyc_Toc_temp_var();}}; _temp1790;});
struct _tagged_ptr6 xs=({ unsigned int _temp1783=( unsigned int) _temp1733;
struct _tuple0** _temp1784=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1783); struct _tagged_ptr6 _temp1786={ _temp1784, _temp1784,
_temp1784 + _temp1783};{ unsigned int _temp1785= _temp1783; unsigned int i; for(
i= 0; i < _temp1785; i ++){ _temp1784[ i]= Cyc_Toc_temp_var();}}; _temp1786;});
struct Cyc_List_List* xexps=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(*
f)( struct _tuple0*, struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,( struct Cyc_Position_Segment*)
0,(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct _tagged_ptr6 arr))
Cyc_List_rfrom_array)( r, xs)); xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List*
_temp1735=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1735->hd=( void*) Cyc_Absyn_var_exp( _temp1729, 0); _temp1735->tl= 0;
_temp1735;}));{ struct Cyc_Absyn_Stmt* _temp1736= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( _temp1723, 0), xexps, 0), 0); goto _LL1737; _LL1737: { struct
Cyc_Absyn_Exp* _temp1738= Cyc_Absyn_add_exp( _temp1727, _temp1721, 0); goto
_LL1739; _LL1739: { struct Cyc_List_List* _temp1749=({ struct Cyc_List_List*(*
_temp1740)( struct _tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4))
Cyc_List_list; struct _tuple4* _temp1743=({ struct _tuple4* _temp1748=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1748->f1= 0; _temp1748->f2=
_temp1727; _temp1748;}); struct _tuple4* _temp1744=({ struct _tuple4* _temp1747=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1747->f1= 0;
_temp1747->f2= _temp1727; _temp1747;}); struct _tuple4* _temp1745=({ struct
_tuple4* _temp1746=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1746->f1= 0; _temp1746->f2= _temp1738; _temp1746;}); struct _tuple4*
_temp1741[ 3u]={ _temp1743, _temp1744, _temp1745}; struct _tagged_ptr4 _temp1742={
_temp1741, _temp1741, _temp1741 + 3u}; _temp1740( _temp1742);}); goto _LL1750;
_LL1750: _temp1736= Cyc_Absyn_declare_stmt( _temp1729, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp1715,( void*) _temp1423->rgn, Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp(
0, _temp1749, 0), _temp1736, 0);{ struct Cyc_List_List* _temp1751=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_ptr6 arr)) Cyc_List_rfrom_array)( r, vs));
goto _LL1752; _LL1752: { struct Cyc_List_List* _temp1753=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_make_dle, _temp1751); goto _LL1754; _LL1754: _temp1736= Cyc_Absyn_declare_stmt(
_temp1725, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1753, 0), _temp1736, 0);{ struct Cyc_List_List* _temp1755=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1429);
goto _LL1756; _LL1756: { struct Cyc_List_List* _temp1757=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1425);
goto _LL1758; _LL1758:{ int i= _temp1427 - 1; for( 0; i >= 0;( i --, _temp1755=((
struct Cyc_List_List*) _check_null( _temp1755))->tl)){ struct Cyc_Absyn_Exp* arg=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1755))->hd;
void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c(
nv, arg); if( _temp1423->inject){ void* _temp1761; struct Cyc_Position_Segment*
_temp1763; struct Cyc_List_List* _temp1765; struct Cyc_List_List* _temp1767;
struct _tuple0* _temp1769; struct Cyc_Absyn_Tunionfield _temp1759=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1757))->hd); _LL1770: _temp1769=
_temp1759.name; goto _LL1768; _LL1768: _temp1767= _temp1759.tvs; goto _LL1766;
_LL1766: _temp1765= _temp1759.typs; goto _LL1764; _LL1764: _temp1763= _temp1759.loc;
goto _LL1762; _LL1762: _temp1761=( void*) _temp1759.sc; goto _LL1760; _LL1760:
_temp1757=(( struct Cyc_List_List*) _check_null( _temp1757))->tl;{ struct
_tuple0* _temp1771= Cyc_Toc_temp_var(); goto _LL1772; _LL1772: { struct Cyc_Absyn_Exp*
_temp1773= Cyc_Absyn_var_exp( _temp1771, 0); goto _LL1774; _LL1774: { void*
_temp1777= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1769,({
unsigned char* _temp1775=( unsigned char*)"_struct"; struct _tagged_string
_temp1776; _temp1776.curr= _temp1775; _temp1776.base= _temp1775; _temp1776.last_plus_one=
_temp1775 + 8; _temp1776;}))); goto _LL1778; _LL1778: _temp1736= Cyc_Absyn_declare_stmt(*((
struct _tuple0**(*)( struct _tagged_ptr6, unsigned int, unsigned int))
_check_unknown_subscript)( vs, sizeof( struct _tuple0*), i), _temp1717,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1717, Cyc_Absyn_address_exp( _temp1773,
0), 0), _temp1736, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp1765))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1779= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1773, Cyc_Toc_fieldname(
1), 0), arg, 0); goto _LL1780; _LL1780: _temp1736= Cyc_Absyn_seq_stmt( _temp1779,
_temp1736, 0);{ struct Cyc_Absyn_Stmt* _temp1781= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1773, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1769, 0), 0); goto
_LL1782; _LL1782: _temp1736= Cyc_Absyn_seq_stmt( _temp1781, _temp1736, 0);
_temp1736= Cyc_Absyn_declare_stmt( _temp1771, _temp1777, 0, _temp1736, 0);}}}}}}}
else{ _temp1736= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr6, unsigned int, unsigned int)) _check_unknown_subscript)( vs,
sizeof( struct _tuple0*), i), _temp1717,( struct Cyc_Absyn_Exp*) arg, _temp1736,
0);}}}{ int i= _temp1733 - 1; for( 0; i >= 0;( i --, _temp1755=(( struct Cyc_List_List*)
_check_null( _temp1755))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1755))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1736= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr6, unsigned int, unsigned int)) _check_unknown_subscript)( xs,
sizeof( struct _tuple0*), i), argtype,( struct Cyc_Absyn_Exp*) arg, _temp1736, 0);}}
Cyc_Toc_exp_to_c( nv, _temp1431); _temp1736= Cyc_Absyn_declare_stmt( _temp1723,
_temp1719,( struct Cyc_Absyn_Exp*) _temp1431, _temp1736, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1736, 0))->r));}}}}}}}}}}}}}}}}}};
_pop_region(& _temp1714);} goto _LL1297; _LL1323: Cyc_Toc_exp_to_c( nv,
_temp1433);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1433), 0))->r)); goto _LL1297; _LL1325: Cyc_Toc_exp_to_c(
nv, _temp1435); goto _LL1297; _LL1327: Cyc_Toc_exp_to_c( nv, _temp1439);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
for( 0; _temp1437 != 0; _temp1437=(( struct Cyc_List_List*) _check_null(
_temp1437))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*)
_check_null( _temp1437))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*)
Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1439, 0))->r)); break;}} goto _LL1297;} _LL1329: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1441->topt))->v; void* new_typ=*
_temp1445;* _temp1445= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1441);{ int noconv= 0;{ void* _temp1793= Cyc_Toc_conversion( old_t2,
new_typ); void* _temp1805; struct Cyc_Absyn_Exp* _temp1807; void* _temp1809; int
_temp1811; struct Cyc_Absyn_Exp* _temp1813; void* _temp1815; void* _temp1817;
_LL1795: if( _temp1793 ==( void*) Cyc_Toc_NoConv){ goto _LL1796;} else{ goto
_LL1797;} _LL1797: if(( unsigned int) _temp1793 > 1u?*(( int*) _temp1793) == Cyc_Toc_NullCheck:
0){ _LL1806: _temp1805=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp1793)->f1;
goto _LL1798;} else{ goto _LL1799;} _LL1799: if(( unsigned int) _temp1793 > 1u?*((
int*) _temp1793) == Cyc_Toc_TagPtr: 0){ _LL1810: _temp1809=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp1793)->f1; goto _LL1808; _LL1808: _temp1807=(( struct Cyc_Toc_TagPtr_struct*)
_temp1793)->f2; goto _LL1800;} else{ goto _LL1801;} _LL1801: if(( unsigned int)
_temp1793 > 1u?*(( int*) _temp1793) == Cyc_Toc_UntagPtr: 0){ _LL1816: _temp1815=(
void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp1793)->f1; goto _LL1814; _LL1814:
_temp1813=(( struct Cyc_Toc_UntagPtr_struct*) _temp1793)->f2; goto _LL1812;
_LL1812: _temp1811=(( struct Cyc_Toc_UntagPtr_struct*) _temp1793)->f3; goto
_LL1802;} else{ goto _LL1803;} _LL1803: if(( unsigned int) _temp1793 > 1u?*((
int*) _temp1793) == Cyc_Toc_UntagPtrNocheck: 0){ _LL1818: _temp1817=( void*)((
struct Cyc_Toc_UntagPtrNocheck_struct*) _temp1793)->f1; goto _LL1804;} else{
goto _LL1794;} _LL1796: goto _LL1794; _LL1798:( void*)( e->r=( void*)(( void*)(
Cyc_Toc_null_check_conv( nv->toplevel, _temp1805, _temp1441))->r)); goto _LL1794;
_LL1800:( void*)( e->r=( void*)(( void*)( Cyc_Toc_tag_ptr_conv( nv->toplevel,
_temp1809, _temp1807, _temp1441))->r)); goto _LL1794; _LL1802:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_untag_ptr_conv( nv->toplevel, _temp1815, _temp1813,
_temp1811, _temp1441))->r)); goto _LL1794; _LL1804:( void*)( _temp1441->r=( void*)((
void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp1441->r,
_temp1441->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1794; _LL1794:;} goto
_LL1297;}} _LL1331:{ void* _temp1819=( void*) _temp1446->r; struct Cyc_Absyn_Structdecl*
_temp1827; struct Cyc_List_List* _temp1829; struct Cyc_Core_Opt* _temp1831;
struct _tuple0* _temp1833; struct Cyc_List_List* _temp1835; _LL1821: if(*(( int*)
_temp1819) == Cyc_Absyn_Struct_e){ _LL1834: _temp1833=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1819)->f1; goto _LL1832; _LL1832: _temp1831=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1819)->f2; goto _LL1830; _LL1830: _temp1829=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1819)->f3; goto _LL1828; _LL1828: _temp1827=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1819)->f4; goto _LL1822;} else{ goto _LL1823;} _LL1823: if(*(( int*)
_temp1819) == Cyc_Absyn_Tuple_e){ _LL1836: _temp1835=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1819)->f1; goto _LL1824;} else{ goto _LL1825;} _LL1825: goto _LL1826;
_LL1822: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ struct _tagged_string _temp1837=( struct _tagged_string)
Cyc_Position_string_of_segment( _temp1446->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1837.last_plus_one - _temp1837.curr, _temp1837.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1446->topt))->v, 1, 0, _temp1829, _temp1833))->r)); goto
_LL1820; _LL1824: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ struct _tagged_string _temp1838=( struct _tagged_string)
Cyc_Position_string_of_segment( _temp1446->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1838.last_plus_one - _temp1838.curr, _temp1838.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1835))->r)); goto _LL1820;
_LL1826: Cyc_Toc_exp_to_c( nv, _temp1446); if( ! Cyc_Absyn_is_lvalue( _temp1446)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1446,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1446, 0))->r));} goto _LL1820; _LL1820:;} goto _LL1297; _LL1333: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
struct _tagged_string _temp1839=( struct _tagged_string) Cyc_Position_string_of_segment(
_temp1448->loc); xprintf("%.*s: new at top-level", _temp1839.last_plus_one -
_temp1839.curr, _temp1839.curr);}));}{ void* _temp1840=( void*) _temp1448->r;
struct Cyc_List_List* _temp1852; struct Cyc_Absyn_Exp* _temp1854; struct Cyc_Absyn_Exp*
_temp1856; struct Cyc_Absyn_Vardecl* _temp1858; struct Cyc_Absyn_Structdecl*
_temp1860; struct Cyc_List_List* _temp1862; struct Cyc_Core_Opt* _temp1864;
struct _tuple0* _temp1866; struct Cyc_List_List* _temp1868; _LL1842: if(*(( int*)
_temp1840) == Cyc_Absyn_Array_e){ _LL1853: _temp1852=(( struct Cyc_Absyn_Array_e_struct*)
_temp1840)->f1; goto _LL1843;} else{ goto _LL1844;} _LL1844: if(*(( int*)
_temp1840) == Cyc_Absyn_Comprehension_e){ _LL1859: _temp1858=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1840)->f1; goto _LL1857; _LL1857: _temp1856=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1840)->f2; goto _LL1855; _LL1855: _temp1854=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1840)->f3; goto _LL1845;} else{ goto _LL1846;} _LL1846: if(*(( int*)
_temp1840) == Cyc_Absyn_Struct_e){ _LL1867: _temp1866=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1840)->f1; goto _LL1865; _LL1865: _temp1864=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1840)->f2; goto _LL1863; _LL1863: _temp1862=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1840)->f3; goto _LL1861; _LL1861: _temp1860=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1840)->f4; goto _LL1847;} else{ goto _LL1848;} _LL1848: if(*(( int*)
_temp1840) == Cyc_Absyn_Tuple_e){ _LL1869: _temp1868=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1840)->f1; goto _LL1849;} else{ goto _LL1850;} _LL1850: goto _LL1851;
_LL1843: { struct _tuple0* _temp1870= Cyc_Toc_temp_var(); goto _LL1871; _LL1871: {
struct Cyc_Absyn_Exp* _temp1872= Cyc_Absyn_var_exp( _temp1870, 0); goto _LL1873;
_LL1873: { struct Cyc_Absyn_Stmt* _temp1874= Cyc_Toc_init_array( nv, _temp1872,
_temp1852, Cyc_Absyn_exp_stmt( _temp1872, 0)); goto _LL1875; _LL1875: { void*
old_elt_typ;{ void* _temp1876= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1882; struct Cyc_Absyn_Conref* _temp1884; struct Cyc_Absyn_Tqual _temp1886;
struct Cyc_Absyn_Conref* _temp1888; void* _temp1890; void* _temp1892; _LL1878:
if(( unsigned int) _temp1876 > 4u?*(( int*) _temp1876) == Cyc_Absyn_PointerType:
0){ _LL1883: _temp1882=(( struct Cyc_Absyn_PointerType_struct*) _temp1876)->f1;
_LL1893: _temp1892=( void*) _temp1882.elt_typ; goto _LL1891; _LL1891: _temp1890=(
void*) _temp1882.rgn_typ; goto _LL1889; _LL1889: _temp1888= _temp1882.nullable;
goto _LL1887; _LL1887: _temp1886= _temp1882.tq; goto _LL1885; _LL1885: _temp1884=
_temp1882.bounds; goto _LL1879;} else{ goto _LL1880;} _LL1880: goto _LL1881;
_LL1879: old_elt_typ= _temp1892; goto _LL1877; _LL1881: old_elt_typ=(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1894=(
unsigned char*)"exp_to_c:new array expression doesn't have ptr type"; struct
_tagged_string _temp1895; _temp1895.curr= _temp1894; _temp1895.base= _temp1894;
_temp1895.last_plus_one= _temp1894 + 52; _temp1895;})); goto _LL1877; _LL1877:;}{
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1896= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1897; _LL1897: { struct Cyc_Absyn_Exp*
_temp1898= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1852), 0), 0); goto
_LL1899; _LL1899: { struct Cyc_Absyn_Exp* e1; if( _temp1450 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1898);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1450); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1898);}{ struct Cyc_Absyn_Exp* _temp1900= Cyc_Absyn_cast_exp( _temp1896, e1,
0); goto _LL1901; _LL1901:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1870, _temp1896,( struct Cyc_Absyn_Exp*) _temp1900,
_temp1874, 0), 0))->r)); goto _LL1841;}}}}}}}} _LL1845: { int is_tagged_ptr= 0;{
void* _temp1902= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1908; struct Cyc_Absyn_Conref* _temp1910; struct Cyc_Absyn_Tqual _temp1912;
struct Cyc_Absyn_Conref* _temp1914; void* _temp1916; void* _temp1918; _LL1904:
if(( unsigned int) _temp1902 > 4u?*(( int*) _temp1902) == Cyc_Absyn_PointerType:
0){ _LL1909: _temp1908=(( struct Cyc_Absyn_PointerType_struct*) _temp1902)->f1;
_LL1919: _temp1918=( void*) _temp1908.elt_typ; goto _LL1917; _LL1917: _temp1916=(
void*) _temp1908.rgn_typ; goto _LL1915; _LL1915: _temp1914= _temp1908.nullable;
goto _LL1913; _LL1913: _temp1912= _temp1908.tq; goto _LL1911; _LL1911: _temp1910=
_temp1908.bounds; goto _LL1905;} else{ goto _LL1906;} _LL1906: goto _LL1907;
_LL1905: is_tagged_ptr=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1910) ==( void*) Cyc_Absyn_Unknown_b; goto _LL1903;
_LL1907:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1920=( unsigned char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1921; _temp1921.curr= _temp1920; _temp1921.base=
_temp1920; _temp1921.last_plus_one= _temp1920 + 42; _temp1921;})); goto _LL1903;
_LL1903:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1854->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1856);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1858, Cyc_Absyn_var_exp(
max, 0), _temp1854, Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1922=
_new_region(); struct _RegionHandle* r=& _temp1922; _push_region( r);{ struct
Cyc_List_List* decls=({ struct Cyc_List_List* _temp1952=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1952->hd=( void*)({
struct _tuple10* _temp1953=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1953->f1= max; _temp1953->f2= Cyc_Absyn_uint_t; _temp1953->f3=(
struct Cyc_Absyn_Exp*) _temp1856; _temp1953;}); _temp1952->tl= 0; _temp1952;});
struct Cyc_Absyn_Exp* ai; if( _temp1450 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1450); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1923=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1923->hd=( void*)({ struct _tuple10*
_temp1924=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1924->f1= a; _temp1924->f2= ptr_typ; _temp1924->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1924;}); _temp1923->tl= decls; _temp1923;}); if( is_tagged_ptr){
struct _tuple0* _temp1925= Cyc_Toc_temp_var(); goto _LL1926; _LL1926: { void*
_temp1927= Cyc_Toc_typ_to_c( old_typ); goto _LL1928; _LL1928: { struct Cyc_Absyn_Exp*
_temp1938= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(* _temp1929)(
struct _tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4)) Cyc_List_list;
struct _tuple4* _temp1932=({ struct _tuple4* _temp1937=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1937->f1= 0; _temp1937->f2= Cyc_Absyn_var_exp(
a, 0); _temp1937;}); struct _tuple4* _temp1933=({ struct _tuple4* _temp1936=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1936->f1= 0;
_temp1936->f2= Cyc_Absyn_var_exp( a, 0); _temp1936;}); struct _tuple4* _temp1934=({
struct _tuple4* _temp1935=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1935->f1= 0; _temp1935->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1935;}); struct _tuple4* _temp1930[ 3u]={
_temp1932, _temp1933, _temp1934}; struct _tagged_ptr4 _temp1931={ _temp1930,
_temp1930, _temp1930 + 3u}; _temp1929( _temp1931);}), 0); goto _LL1939; _LL1939:
decls=({ struct Cyc_List_List* _temp1940=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1940->hd=( void*)({ struct _tuple10*
_temp1941=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1941->f1= _temp1925; _temp1941->f2= _temp1927; _temp1941->f3=( struct Cyc_Absyn_Exp*)
_temp1938; _temp1941;}); _temp1940->tl= decls; _temp1940;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1925, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
_temp1942= decls; goto _LL1943; _LL1943: for( 0; _temp1942 != 0; _temp1942=((
struct Cyc_List_List*) _check_null( _temp1942))->tl){ struct Cyc_Absyn_Exp*
_temp1946; void* _temp1948; struct _tuple0* _temp1950; struct _tuple10 _temp1944=*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1942))->hd);
_LL1951: _temp1950= _temp1944.f1; goto _LL1949; _LL1949: _temp1948= _temp1944.f2;
goto _LL1947; _LL1947: _temp1946= _temp1944.f3; goto _LL1945; _LL1945: s= Cyc_Absyn_declare_stmt(
_temp1950, _temp1948, _temp1946, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region(& _temp1922);} goto _LL1841;}}} _LL1847:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1448->topt))->v, 1, _temp1450, _temp1862, _temp1866))->r));
goto _LL1841; _LL1849:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv,
1, _temp1450, _temp1868))->r)); goto _LL1841; _LL1851: { void* _temp1954=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1448->topt))->v; goto _LL1955; _LL1955: {
struct _tuple0* _temp1956= Cyc_Toc_temp_var(); goto _LL1957; _LL1957: { struct
Cyc_Absyn_Stmt* _temp1958= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1956, 0),
0); goto _LL1959; _LL1959: { struct Cyc_Absyn_Exp* _temp1960= Cyc_Absyn_signed_int_exp(
0, 0); goto _LL1961; _LL1961: Cyc_Toc_exp_to_c( nv, _temp1448); _temp1958= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1956, 0),
_temp1960, 0), _temp1448, 0), _temp1958, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
_temp1954); void* _temp1962= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto
_LL1963; _LL1963: { struct Cyc_Absyn_Exp* _temp1964= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0); goto _LL1965; _LL1965: { struct Cyc_Absyn_Exp* e1; if( _temp1450 ==
0){ e1= Cyc_Toc_malloc_exp( _temp1954, _temp1964);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp1450); Cyc_Toc_exp_to_c( nv, r); e1=
Cyc_Toc_rmalloc_exp( r, _temp1964);}{ struct Cyc_Absyn_Exp* _temp1966= Cyc_Absyn_cast_exp(
_temp1962, e1, 0); goto _LL1967; _LL1967:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1956, _temp1962,( struct Cyc_Absyn_Exp*) _temp1966,
_temp1958, 0), 0))->r)); goto _LL1841;}}}}}}}} _LL1841:;} goto _LL1297; _LL1335:
Cyc_Toc_exp_to_c( nv, _temp1452); goto _LL1297; _LL1337:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp1968=( struct Cyc_Absyn_Sizeoftyp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct)); _temp1968[ 0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _temp1969; _temp1969.tag= Cyc_Absyn_Sizeoftyp_e;
_temp1969.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1454); _temp1969;});
_temp1968;}))); goto _LL1297; _LL1339:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Offsetof_e_struct* _temp1970=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1970[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1971; _temp1971.tag= Cyc_Absyn_Offsetof_e;
_temp1971.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1458); _temp1971.f2=
_temp1456; _temp1971;}); _temp1970;}))); goto _LL1297; _LL1341: { void* e1_typ=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1460->topt))->v);
Cyc_Toc_exp_to_c( nv, _temp1460);{ void* _temp1972= e1_typ; struct Cyc_Absyn_PtrInfo
_temp1978; struct Cyc_Absyn_Conref* _temp1980; struct Cyc_Absyn_Tqual _temp1982;
struct Cyc_Absyn_Conref* _temp1984; void* _temp1986; void* _temp1988; _LL1974:
if(( unsigned int) _temp1972 > 4u?*(( int*) _temp1972) == Cyc_Absyn_PointerType:
0){ _LL1979: _temp1978=(( struct Cyc_Absyn_PointerType_struct*) _temp1972)->f1;
_LL1989: _temp1988=( void*) _temp1978.elt_typ; goto _LL1987; _LL1987: _temp1986=(
void*) _temp1978.rgn_typ; goto _LL1985; _LL1985: _temp1984= _temp1978.nullable;
goto _LL1983; _LL1983: _temp1982= _temp1978.tq; goto _LL1981; _LL1981: _temp1980=
_temp1978.bounds; goto _LL1975;} else{ goto _LL1976;} _LL1976: goto _LL1977;
_LL1975:{ void* _temp1990=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1980); struct Cyc_Absyn_Exp* _temp1996; _LL1992: if(
_temp1990 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1993;} else{ goto _LL1994;}
_LL1994: if(( unsigned int) _temp1990 > 1u?*(( int*) _temp1990) == Cyc_Absyn_Upper_b:
0){ _LL1997: _temp1996=(( struct Cyc_Absyn_Upper_b_struct*) _temp1990)->f1; goto
_LL1995;} else{ goto _LL1991;} _LL1993: { struct _tuple0* x= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* _temp1998= Cyc_Toc_typ_to_c_array(
_temp1988); goto _LL1999; _LL1999: { void* tx= Cyc_Toc_add_tagged_ptr_type(
_temp1998, _temp1982); void* ta2= Cyc_Absyn_cstar_typ( _temp1998, _temp1982);
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
struct Cyc_Absyn_Exp*) _temp1460, s1, 0), 0))->r)); goto _LL1991;}} _LL1995: if(
Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1460->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp2000=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2000->hd=(
void*) _temp1460; _temp2000->tl= 0; _temp2000;}), 0), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, e1_typ,(
struct Cyc_Absyn_Exp*) _temp1460, s3, 0), 0))->r));}} goto _LL1991; _LL1991:;}
goto _LL1973; _LL1977:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2001=( unsigned char*)"exp_to_c: Deref: non-pointer"; struct
_tagged_string _temp2002; _temp2002.curr= _temp2001; _temp2002.base= _temp2001;
_temp2002.last_plus_one= _temp2001 + 29; _temp2002;})); goto _LL1973; _LL1973:;}
goto _LL1297;} _LL1343: Cyc_Toc_exp_to_c( nv, _temp1464); if( Cyc_Toc_is_poly_project(
e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1297; _LL1345: { void* e1typ= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1468->topt))->v); Cyc_Toc_exp_to_c(
nv, _temp1468);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp2005; struct Cyc_Absyn_Tqual _temp2007; struct Cyc_Absyn_Conref* _temp2009;
void* _temp2011; void* _temp2013; struct Cyc_Absyn_PtrInfo _temp2003= Cyc_Toc_get_ptr_type(
e1typ); _LL2014: _temp2013=( void*) _temp2003.elt_typ; goto _LL2012; _LL2012:
_temp2011=( void*) _temp2003.rgn_typ; goto _LL2010; _LL2010: _temp2009=
_temp2003.nullable; goto _LL2008; _LL2008: _temp2007= _temp2003.tq; goto _LL2006;
_LL2006: _temp2005= _temp2003.bounds; goto _LL2004; _LL2004:{ void* _temp2015=((
void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp2005); struct Cyc_Absyn_Exp* _temp2021; _LL2017: if(( unsigned int)
_temp2015 > 1u?*(( int*) _temp2015) == Cyc_Absyn_Upper_b: 0){ _LL2022: _temp2021=((
struct Cyc_Absyn_Upper_b_struct*) _temp2015)->f1; goto _LL2018;} else{ goto
_LL2019;} _LL2019: if( _temp2015 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2020;}
else{ goto _LL2016;} _LL2018: if( Cyc_Evexp_eval_const_uint_exp( _temp2021) < 1){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2023=(
unsigned char*)"exp_to_c:  StructArrow_e on pointer of size 0"; struct
_tagged_string _temp2024; _temp2024.curr= _temp2023; _temp2024.base= _temp2023;
_temp2024.last_plus_one= _temp2023 + 46; _temp2024;}));}{ int possibly_null=((
int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp2009);
if( Cyc_Toc_is_nullable(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1468->topt))->v)){
if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1468->topt))->v), Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp2025=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2025->hd=( void*) _temp1468; _temp2025->tl=
0; _temp2025;}), 0), 0), _temp1466, 0))->r));} else{ struct _tuple0* x= Cyc_Toc_temp_var();
void* tx= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1468->topt))->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0),
_temp1466, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1468, s3, 0), 0))->r));}} goto _LL2016;}
_LL2020: { void* ta1= Cyc_Toc_typ_to_c_array( _temp2013); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp2007); if( Cyc_Toc_use_runtimecheck_funs){ struct Cyc_List_List*
_temp2035=({ struct Cyc_List_List*(* _temp2026)( struct _tagged_ptr7)=( struct
Cyc_List_List*(*)( struct _tagged_ptr7)) Cyc_List_list; struct _tuple1*
_temp2029=({ struct _tuple1* _temp2034=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2034->f1= 0; _temp2034->f2= Cyc_Toc_mt_tq; _temp2034->f3=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1468->topt))->v);
_temp2034;}); struct _tuple1* _temp2030=({ struct _tuple1* _temp2033=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2033->f1= 0; _temp2033->f2=
Cyc_Toc_mt_tq; _temp2033->f3= Cyc_Absyn_uint_t; _temp2033;}); struct _tuple1*
_temp2031=({ struct _tuple1* _temp2032=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2032->f1= 0; _temp2032->f2= Cyc_Toc_mt_tq; _temp2032->f3=
Cyc_Absyn_uint_t; _temp2032;}); struct _tuple1* _temp2027[ 3u]={ _temp2029,
_temp2030, _temp2031}; struct _tagged_ptr7 _temp2028={ _temp2027, _temp2027,
_temp2027 + 3u}; _temp2026( _temp2028);}); goto _LL2036; _LL2036: { void*
_temp2040= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2037=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2037[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2038; _temp2038.tag= Cyc_Absyn_FnType;
_temp2038.f1=({ struct Cyc_Absyn_FnInfo _temp2039; _temp2039.tvars= 0; _temp2039.effect=
0; _temp2039.ret_typ=( void*) ta2; _temp2039.args= _temp2035; _temp2039.c_varargs=
0; _temp2039.cyc_varargs= 0; _temp2039.rgn_po= 0; _temp2039.attributes= 0;
_temp2039;}); _temp2038;}); _temp2037;}), Cyc_Toc_mt_tq); goto _LL2041; _LL2041: {
struct Cyc_Absyn_Exp* fnexp= Cyc_Absyn_cast_exp( _temp2040, Cyc_Absyn_var_exp(
Cyc_Toc__check_unknown_subscript_qv, 0), 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_uint_exp(
0, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_fncall_exp(
fnexp,({ struct Cyc_List_List*(* _temp2042)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)(
struct _tagged_ptr8)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2045= _temp1468;
struct Cyc_Absyn_Exp* _temp2046= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2047= e2; struct Cyc_Absyn_Exp* _temp2043[ 3u]={ _temp2045, _temp2046,
_temp2047}; struct _tagged_ptr8 _temp2044={ _temp2043, _temp2043, _temp2043 + 3u};
_temp2042( _temp2044);}), 0), _temp1466, 0))->r));}}} else{ struct _tuple0* x=
Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2007);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( a, 0), _temp1466, 0), 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt( 0), 0); struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt* s1=
Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,(
struct Cyc_Absyn_Exp*) _temp1468, s1, 0), 0))->r));} goto _LL2016;} _LL2016:;}
if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1297;}} _LL1347: { void* _temp2048= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1472->topt))->v); goto _LL2049;
_LL2049: Cyc_Toc_exp_to_c( nv, _temp1472); Cyc_Toc_exp_to_c( nv, _temp1470);{
void* _temp2050= _temp2048; struct Cyc_List_List* _temp2058; struct Cyc_Absyn_PtrInfo
_temp2060; struct Cyc_Absyn_Conref* _temp2062; struct Cyc_Absyn_Tqual _temp2064;
struct Cyc_Absyn_Conref* _temp2066; void* _temp2068; void* _temp2070; _LL2052:
if(( unsigned int) _temp2050 > 4u?*(( int*) _temp2050) == Cyc_Absyn_TupleType: 0){
_LL2059: _temp2058=(( struct Cyc_Absyn_TupleType_struct*) _temp2050)->f1; goto
_LL2053;} else{ goto _LL2054;} _LL2054: if(( unsigned int) _temp2050 > 4u?*((
int*) _temp2050) == Cyc_Absyn_PointerType: 0){ _LL2061: _temp2060=(( struct Cyc_Absyn_PointerType_struct*)
_temp2050)->f1; _LL2071: _temp2070=( void*) _temp2060.elt_typ; goto _LL2069;
_LL2069: _temp2068=( void*) _temp2060.rgn_typ; goto _LL2067; _LL2067: _temp2066=
_temp2060.nullable; goto _LL2065; _LL2065: _temp2064= _temp2060.tq; goto _LL2063;
_LL2063: _temp2062= _temp2060.bounds; goto _LL2055;} else{ goto _LL2056;}
_LL2056: goto _LL2057; _LL2053: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1470) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1472, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL2051;} _LL2055:{ void*
_temp2072=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp2062); struct Cyc_Absyn_Exp* _temp2078; _LL2074: if((
unsigned int) _temp2072 > 1u?*(( int*) _temp2072) == Cyc_Absyn_Upper_b: 0){
_LL2079: _temp2078=(( struct Cyc_Absyn_Upper_b_struct*) _temp2072)->f1; goto
_LL2075;} else{ goto _LL2076;} _LL2076: if( _temp2072 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL2077;} else{ goto _LL2073;} _LL2075: _temp2078= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp2078), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp2066); void* ta1= Cyc_Toc_typ_to_c( _temp2070);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2064); if( Cyc_Toc_use_runtimecheck_funs){
if( possibly_null){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp(
ta2, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv,
0),({ struct Cyc_List_List*(* _temp2080)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)(
struct _tagged_ptr8)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2083= _temp1472;
struct Cyc_Absyn_Exp* _temp2084= _temp2078; struct Cyc_Absyn_Exp* _temp2085= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp2086= _temp1470; struct Cyc_Absyn_Exp*
_temp2081[ 4u]={ _temp2083, _temp2084, _temp2085, _temp2086}; struct
_tagged_ptr8 _temp2082={ _temp2081, _temp2081, _temp2081 + 4u}; _temp2080(
_temp2082);}), 0), 0), 0))->r));} else{( void*)( _temp1470->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List*(* _temp2087)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)(
struct _tagged_ptr8)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2090= _temp2078;
struct Cyc_Absyn_Exp* _temp2091= Cyc_Absyn_copy_exp( _temp1470); struct Cyc_Absyn_Exp*
_temp2088[ 2u]={ _temp2090, _temp2091}; struct _tagged_ptr8 _temp2089={
_temp2088, _temp2088, _temp2088 + 2u}; _temp2087( _temp2089);}), 0))->r));}}
else{ struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp(
i, 0), _temp2078, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp((
void*) e3->r, 0), 0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1470, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1472, s1, 0), 0))->r));}} goto _LL2073;} _LL2077: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2070); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2064); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2101=({ struct Cyc_List_List*(* _temp2092)( struct
_tagged_ptr7)=( struct Cyc_List_List*(*)( struct _tagged_ptr7)) Cyc_List_list;
struct _tuple1* _temp2095=({ struct _tuple1* _temp2100=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2100->f1= 0; _temp2100->f2= Cyc_Toc_mt_tq;
_temp2100->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1472->topt))->v); _temp2100;}); struct _tuple1* _temp2096=({ struct _tuple1*
_temp2099=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2099->f1=
0; _temp2099->f2= Cyc_Toc_mt_tq; _temp2099->f3= Cyc_Absyn_uint_t; _temp2099;});
struct _tuple1* _temp2097=({ struct _tuple1* _temp2098=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2098->f1= 0; _temp2098->f2= Cyc_Toc_mt_tq;
_temp2098->f3= Cyc_Absyn_uint_t; _temp2098;}); struct _tuple1* _temp2093[ 3u]={
_temp2095, _temp2096, _temp2097}; struct _tagged_ptr7 _temp2094={ _temp2093,
_temp2093, _temp2093 + 3u}; _temp2092( _temp2094);}); goto _LL2102; _LL2102: {
void* _temp2106= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2103=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2103[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2104; _temp2104.tag= Cyc_Absyn_FnType;
_temp2104.f1=({ struct Cyc_Absyn_FnInfo _temp2105; _temp2105.tvars= 0; _temp2105.effect=
0; _temp2105.ret_typ=( void*) ta2; _temp2105.args= _temp2101; _temp2105.c_varargs=
0; _temp2105.cyc_varargs= 0; _temp2105.rgn_po= 0; _temp2105.attributes= 0;
_temp2105;}); _temp2104;}); _temp2103;}), Cyc_Toc_mt_tq); goto _LL2107; _LL2107: {
struct Cyc_Absyn_Exp* fnexp= Cyc_Absyn_cast_exp( _temp2106, Cyc_Absyn_var_exp(
Cyc_Toc__check_unknown_subscript_qv, 0), 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp( fnexp,({ struct Cyc_List_List*(*
_temp2108)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)( struct _tagged_ptr8))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp2111= _temp1472; struct Cyc_Absyn_Exp*
_temp2112= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp* _temp2113=
_temp1470; struct Cyc_Absyn_Exp* _temp2109[ 3u]={ _temp2111, _temp2112,
_temp2113}; struct _tagged_ptr8 _temp2110={ _temp2109, _temp2109, _temp2109 + 3u};
_temp2108( _temp2110);}), 0), 0))->r));}}} else{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2064); struct Cyc_Absyn_Exp*
test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_or_exp(
Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp1470, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1472, s1, 0), 0))->r));} goto _LL2073;} _LL2073:;}
goto _LL2051; _LL2057:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2114=( unsigned char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp2115; _temp2115.curr= _temp2114; _temp2115.base=
_temp2114; _temp2115.last_plus_one= _temp2114 + 49; _temp2115;})); goto _LL2051;
_LL2051:;} goto _LL1297;} _LL1349: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1474))->r));} else{ struct Cyc_List_List*
_temp2116=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1474); goto
_LL2117; _LL2117: { struct _tagged_string* _temp2118= Cyc_Toc_add_tuple_type(
_temp2116); goto _LL2119; _LL2119: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1474 != 0;( _temp1474=(( struct Cyc_List_List*) _check_null(
_temp1474))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1474))->hd); dles=({ struct Cyc_List_List*
_temp2120=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2120->hd=( void*)({ struct _tuple4* _temp2121=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2121->f1= 0; _temp2121->f2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1474))->hd; _temp2121;}); _temp2120->tl=
dles; _temp2120;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));}}} goto _LL1297; _LL1351:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ unsigned char* _temp2122=( unsigned char*)"compoundlit";
struct _tagged_string _temp2123; _temp2123.curr= _temp2122; _temp2123.base=
_temp2122; _temp2123.last_plus_one= _temp2122 + 12; _temp2123;})); goto _LL1297;
_LL1353:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1480, 0))->r));{ struct Cyc_List_List* _temp2124= _temp1480; goto _LL2125;
_LL2125: for( 0; _temp2124 != 0; _temp2124=(( struct Cyc_List_List*) _check_null(
_temp2124))->tl){ struct _tuple4 _temp2128; struct Cyc_Absyn_Exp* _temp2129;
struct Cyc_List_List* _temp2131; struct _tuple4* _temp2126=( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp2124))->hd; _temp2128=* _temp2126;
_LL2132: _temp2131= _temp2128.f1; goto _LL2130; _LL2130: _temp2129= _temp2128.f2;
goto _LL2127; _LL2127: Cyc_Toc_exp_to_c( nv, _temp2129);}} goto _LL1297; _LL1355: {
unsigned int _temp2133= Cyc_Evexp_eval_const_uint_exp( _temp1484); goto _LL2134;
_LL2134: Cyc_Toc_exp_to_c( nv, _temp1482);{ struct Cyc_List_List* es= 0; if( !
Cyc_Toc_is_zero( _temp1482)){ unsigned int i= 0; for( 0; i < _temp2133; i ++){
es=({ struct Cyc_List_List* _temp2135=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2135->hd=( void*)({ struct _tuple4*
_temp2136=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp2136->f1=
0; _temp2136->f2= _temp1482; _temp2136;}); _temp2135->tl= es; _temp2135;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL1297;}} _LL1357: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1490, _temp1494))->r));} else{ if(
_temp1488 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2137=( unsigned char*)"Struct_e: missing structdecl pointer";
struct _tagged_string _temp2138; _temp2138.curr= _temp2137; _temp2138.base=
_temp2137; _temp2138.last_plus_one= _temp2137 + 37; _temp2138;}));}{ struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*) _check_null( _temp1488); struct
_RegionHandle _temp2139= _new_region(); struct _RegionHandle* rgn=& _temp2139;
_push_region( rgn);{ struct Cyc_List_List* _temp2140=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1490,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd2->fields))->v); goto _LL2141; _LL2141: { struct Cyc_List_List* _temp2142= 0;
goto _LL2143; _LL2143:{ struct Cyc_List_List* _temp2144=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto _LL2145; _LL2145: for(
0; _temp2144 != 0; _temp2144=(( struct Cyc_List_List*) _check_null( _temp2144))->tl){
struct Cyc_List_List* _temp2146= _temp2140; goto _LL2147; _LL2147: for( 0;
_temp2146 != 0; _temp2146=(( struct Cyc_List_List*) _check_null( _temp2146))->tl){
if((*(( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2146))->hd)).f1
==( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2144))->hd){ struct _tuple12 _temp2150; struct Cyc_Absyn_Exp* _temp2151;
struct Cyc_Absyn_Structfield* _temp2153; struct _tuple12* _temp2148=( struct
_tuple12*)(( struct Cyc_List_List*) _check_null( _temp2146))->hd; _temp2150=*
_temp2148; _LL2154: _temp2153= _temp2150.f1; goto _LL2152; _LL2152: _temp2151=
_temp2150.f2; goto _LL2149; _LL2149: { void* _temp2155=( void*) _temp2153->type;
goto _LL2156; _LL2156: Cyc_Toc_exp_to_c( nv, _temp2151); if( Cyc_Toc_is_void_star(
_temp2155)){( void*)( _temp2151->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp2151->r, 0), 0))->r));} _temp2142=({ struct Cyc_List_List*
_temp2157=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2157->hd=( void*)({ struct _tuple4* _temp2158=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2158->f1= 0; _temp2158->f2= _temp2151; _temp2158;});
_temp2157->tl= _temp2142; _temp2157;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp2142), 0))->r));}}; _pop_region(& _temp2139);}} goto
_LL1297; _LL1359: { struct Cyc_List_List* fs;{ void* _temp2159= Cyc_Tcutil_compress(
_temp1498); struct Cyc_List_List* _temp2165; _LL2161: if(( unsigned int)
_temp2159 > 4u?*(( int*) _temp2159) == Cyc_Absyn_AnonStructType: 0){ _LL2166:
_temp2165=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2159)->f1; goto
_LL2162;} else{ goto _LL2163;} _LL2163: goto _LL2164; _LL2162: fs= _temp2165;
goto _LL2160; _LL2164: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct _tagged_string _temp2167=
Cyc_Absynpp_typ2string( _temp1498); xprintf("anon struct has type %.*s",
_temp2167.last_plus_one - _temp2167.curr, _temp2167.curr);})); goto _LL2160;
_LL2160:;}{ struct _RegionHandle _temp2168= _new_region(); struct _RegionHandle*
rgn=& _temp2168; _push_region( rgn);{ struct Cyc_List_List* _temp2169=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1496, fs); goto _LL2170; _LL2170: for( 0; _temp2169 != 0;
_temp2169=(( struct Cyc_List_List*) _check_null( _temp2169))->tl){ struct
_tuple12 _temp2173; struct Cyc_Absyn_Exp* _temp2174; struct Cyc_Absyn_Structfield*
_temp2176; struct _tuple12* _temp2171=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp2169))->hd; _temp2173=* _temp2171; _LL2177: _temp2176=
_temp2173.f1; goto _LL2175; _LL2175: _temp2174= _temp2173.f2; goto _LL2172;
_LL2172: { void* _temp2178=( void*) _temp2176->type; goto _LL2179; _LL2179: Cyc_Toc_exp_to_c(
nv, _temp2174); if( Cyc_Toc_is_void_star( _temp2178)){( void*)( _temp2174->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp2174->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1496, 0))->r));}; _pop_region(& _temp2168);} goto _LL1297;} _LL1361: {
struct _tuple0* qv= _temp1500->name; if( _temp1502->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2180= _temp1502->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1502->fields))->v;
goto _LL2181; _LL2181: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2180))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2180))->hd)->typs
== 0){ tag_count ++;} _temp2180=(( struct Cyc_List_List*) _check_null( _temp2180))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1297;} _LL1363: { struct _tuple0* _temp2182= _temp1510->name; goto
_LL2183; _LL2183: { struct Cyc_List_List* _temp2184= _temp1510->typs; goto
_LL2185; _LL2185: { struct _tuple0* _temp2186= Cyc_Toc_temp_var(); goto _LL2187;
_LL2187: { struct Cyc_Absyn_Exp* _temp2188= Cyc_Absyn_var_exp( _temp2186, 0);
goto _LL2189; _LL2189: { void* _temp2192= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2182,({ unsigned char* _temp2190=( unsigned char*)"_struct"; struct
_tagged_string _temp2191; _temp2191.curr= _temp2190; _temp2191.base= _temp2190;
_temp2191.last_plus_one= _temp2190 + 8; _temp2191;}))); goto _LL2193; _LL2193:
if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp; if( _temp1512->is_xtunion){
tag_exp= Cyc_Absyn_var_exp( _temp2182, 0);} else{ int tag_count= 0; struct Cyc_List_List*
_temp2194= _temp1512->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1512->fields))->v; goto _LL2195; _LL2195: while( Cyc_Absyn_qvar_cmp(
_temp2182,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2194))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2194))->hd)->typs != 0){ tag_count ++;}
_temp2194=(( struct Cyc_List_List*) _check_null( _temp2194))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2196= 0; goto _LL2197;
_LL2197: for( 0; _temp1514 != 0;( _temp1514=(( struct Cyc_List_List*)
_check_null( _temp1514))->tl, _temp2184=(( struct Cyc_List_List*) _check_null(
_temp2184))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1514))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2184))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2196=({ struct Cyc_List_List* _temp2198=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2198[ 0]=({ struct Cyc_List_List
_temp2199; _temp2199.hd=( void*)({ struct _tuple4* _temp2200=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2200[ 0]=({ struct _tuple4
_temp2201; _temp2201.f1= 0; _temp2201.f2= cur_e; _temp2201;}); _temp2200;});
_temp2199.tl= _temp2196; _temp2199;}); _temp2198;});} _temp2196=({ struct Cyc_List_List*
_temp2202=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2202[ 0]=({ struct Cyc_List_List _temp2203; _temp2203.hd=( void*)({ struct
_tuple4* _temp2204=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2204[ 0]=({ struct _tuple4 _temp2205; _temp2205.f1= 0; _temp2205.f2=
tag_exp; _temp2205;}); _temp2204;}); _temp2203.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2196); _temp2203;}); _temp2202;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2206=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2206->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2182,({ unsigned char*
_temp2207=( unsigned char*)"_struct"; struct _tagged_string _temp2208; _temp2208.curr=
_temp2207; _temp2208.base= _temp2207; _temp2208.last_plus_one= _temp2207 + 8;
_temp2208;})); _temp2206;}), _temp2196, 0))->r));}} else{ struct Cyc_List_List*
_temp2210=({ struct Cyc_List_List* _temp2209=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2209->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2188, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp2182, 0), 0); _temp2209->tl= 0; _temp2209;}); goto _LL2211; _LL2211:{ int i=
1; for( 0; _temp1514 != 0;((( _temp1514=(( struct Cyc_List_List*) _check_null(
_temp1514))->tl, i ++)), _temp2184=(( struct Cyc_List_List*) _check_null(
_temp2184))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1514))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2184))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp2212= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2188, Cyc_Toc_fieldname( i), 0), cur_e, 0);
goto _LL2213; _LL2213: _temp2210=({ struct Cyc_List_List* _temp2214=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2214->hd=( void*) _temp2212;
_temp2214->tl= _temp2210; _temp2214;});}}}{ struct Cyc_Absyn_Stmt* _temp2215=
Cyc_Absyn_exp_stmt( _temp2188, 0); goto _LL2216; _LL2216: { struct Cyc_Absyn_Stmt*
_temp2218= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2217=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2217->hd=( void*) _temp2215;
_temp2217->tl= _temp2210; _temp2217;})), 0); goto _LL2219; _LL2219:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp2186,
_temp2192, 0, _temp2218, 0), 0))->r));}}} goto _LL1297;}}}}} _LL1365: goto
_LL1297; _LL1367: { void* t_c= Cyc_Toc_typ_to_c( _temp1526); if( _temp1528 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1528); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1526, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL1297;} _LL1369: Cyc_Toc_stmt_to_c(
nv, _temp1530); goto _LL1297; _LL1371:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2220=( unsigned char*)"UnresolvedMem"; struct _tagged_string
_temp2221; _temp2221.curr= _temp2220; _temp2221.base= _temp2220; _temp2221.last_plus_one=
_temp2220 + 14; _temp2221;})); goto _LL1297; _LL1373:(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp2222=( unsigned char*)"codegen";
struct _tagged_string _temp2223; _temp2223.curr= _temp2222; _temp2223.base=
_temp2222; _temp2223.last_plus_one= _temp2222 + 8; _temp2223;})); goto _LL1297;
_LL1375:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp2224=( unsigned char*)"fill"; struct _tagged_string _temp2225; _temp2225.curr=
_temp2224; _temp2225.base= _temp2224; _temp2225.last_plus_one= _temp2224 + 5;
_temp2225;})); goto _LL1297; _LL1297:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
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
_temp2226=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp2262; struct Cyc_Absyn_Vardecl
_temp2264; struct Cyc_List_List* _temp2265; struct Cyc_Core_Opt* _temp2267;
struct Cyc_Absyn_Exp* _temp2269; void* _temp2271; struct Cyc_Absyn_Tqual
_temp2273; struct _tuple0* _temp2275; void* _temp2277; struct Cyc_Absyn_Vardecl*
_temp2279; int _temp2281; void* _temp2283; unsigned char _temp2285; struct
_tagged_string _temp2287; struct Cyc_Absyn_Enumfield* _temp2289; struct Cyc_Absyn_Enumdecl*
_temp2291; struct Cyc_List_List* _temp2293; struct Cyc_List_List* _temp2295;
struct Cyc_Absyn_Tunionfield* _temp2297; struct Cyc_Absyn_Tuniondecl* _temp2299;
struct Cyc_Absyn_Pat* _temp2302; struct Cyc_Absyn_Pat _temp2304; struct Cyc_Position_Segment*
_temp2305; struct Cyc_Core_Opt* _temp2307; void* _temp2309; struct Cyc_List_List*
_temp2311; struct Cyc_List_List* _temp2313; struct Cyc_Absyn_Tunionfield*
_temp2315; struct Cyc_Absyn_Tuniondecl* _temp2317; struct Cyc_List_List*
_temp2319; struct Cyc_List_List* _temp2321; struct Cyc_Absyn_Tunionfield*
_temp2323; struct Cyc_Absyn_Tuniondecl* _temp2325; struct Cyc_List_List*
_temp2327; struct Cyc_List_List* _temp2329; struct Cyc_List_List* _temp2331;
struct Cyc_Core_Opt* _temp2333; struct Cyc_Absyn_Structdecl* _temp2335; struct
Cyc_Absyn_Pat* _temp2337; struct _tuple0* _temp2339; struct Cyc_List_List*
_temp2341; struct Cyc_List_List* _temp2343; struct _tuple0* _temp2345; struct
Cyc_List_List* _temp2347; struct Cyc_List_List* _temp2349; struct _tuple0*
_temp2351; _LL2228: if(( unsigned int) _temp2226 > 2u?*(( int*) _temp2226) ==
Cyc_Absyn_Var_p: 0){ _LL2263: _temp2262=(( struct Cyc_Absyn_Var_p_struct*)
_temp2226)->f1; _temp2264=* _temp2262; _LL2278: _temp2277=( void*) _temp2264.sc;
goto _LL2276; _LL2276: _temp2275= _temp2264.name; goto _LL2274; _LL2274:
_temp2273= _temp2264.tq; goto _LL2272; _LL2272: _temp2271=( void*) _temp2264.type;
goto _LL2270; _LL2270: _temp2269= _temp2264.initializer; goto _LL2268; _LL2268:
_temp2267= _temp2264.rgn; goto _LL2266; _LL2266: _temp2265= _temp2264.attributes;
goto _LL2229;} else{ goto _LL2230;} _LL2230: if( _temp2226 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2231;} else{ goto _LL2232;} _LL2232: if(( unsigned int) _temp2226 > 2u?*((
int*) _temp2226) == Cyc_Absyn_Reference_p: 0){ _LL2280: _temp2279=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2226)->f1; goto _LL2233;} else{ goto _LL2234;} _LL2234: if( _temp2226 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2235;} else{ goto _LL2236;} _LL2236: if((
unsigned int) _temp2226 > 2u?*(( int*) _temp2226) == Cyc_Absyn_Int_p: 0){
_LL2284: _temp2283=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2226)->f1;
goto _LL2282; _LL2282: _temp2281=(( struct Cyc_Absyn_Int_p_struct*) _temp2226)->f2;
goto _LL2237;} else{ goto _LL2238;} _LL2238: if(( unsigned int) _temp2226 > 2u?*((
int*) _temp2226) == Cyc_Absyn_Char_p: 0){ _LL2286: _temp2285=(( struct Cyc_Absyn_Char_p_struct*)
_temp2226)->f1; goto _LL2239;} else{ goto _LL2240;} _LL2240: if(( unsigned int)
_temp2226 > 2u?*(( int*) _temp2226) == Cyc_Absyn_Float_p: 0){ _LL2288: _temp2287=((
struct Cyc_Absyn_Float_p_struct*) _temp2226)->f1; goto _LL2241;} else{ goto
_LL2242;} _LL2242: if(( unsigned int) _temp2226 > 2u?*(( int*) _temp2226) == Cyc_Absyn_Enum_p:
0){ _LL2292: _temp2291=(( struct Cyc_Absyn_Enum_p_struct*) _temp2226)->f1; goto
_LL2290; _LL2290: _temp2289=(( struct Cyc_Absyn_Enum_p_struct*) _temp2226)->f2;
goto _LL2243;} else{ goto _LL2244;} _LL2244: if(( unsigned int) _temp2226 > 2u?*((
int*) _temp2226) == Cyc_Absyn_Tunion_p: 0){ _LL2300: _temp2299=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2226)->f1; goto _LL2298; _LL2298: _temp2297=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2226)->f2; goto _LL2296; _LL2296: _temp2295=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2226)->f3; goto _LL2294; _LL2294: _temp2293=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2226)->f4; if( _temp2293 == 0){ goto _LL2245;} else{ goto _LL2246;}} else{
goto _LL2246;} _LL2246: if(( unsigned int) _temp2226 > 2u?*(( int*) _temp2226)
== Cyc_Absyn_Pointer_p: 0){ _LL2303: _temp2302=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2226)->f1; _temp2304=* _temp2302; _LL2310: _temp2309=( void*) _temp2304.r;
if(( unsigned int) _temp2309 > 2u?*(( int*) _temp2309) == Cyc_Absyn_Tunion_p: 0){
_LL2318: _temp2317=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2309)->f1; goto
_LL2316; _LL2316: _temp2315=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2309)->f2;
goto _LL2314; _LL2314: _temp2313=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2309)->f3;
goto _LL2312; _LL2312: _temp2311=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2309)->f4;
goto _LL2308;} else{ goto _LL2248;} _LL2308: _temp2307= _temp2304.topt; goto
_LL2306; _LL2306: _temp2305= _temp2304.loc; goto _LL2301;} else{ goto _LL2248;}
_LL2301: if( _temp2311 != 0){ goto _LL2247;} else{ goto _LL2248;} _LL2248: if((
unsigned int) _temp2226 > 2u?*(( int*) _temp2226) == Cyc_Absyn_Tunion_p: 0){
_LL2326: _temp2325=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2226)->f1; goto
_LL2324; _LL2324: _temp2323=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2226)->f2;
goto _LL2322; _LL2322: _temp2321=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2226)->f3;
goto _LL2320; _LL2320: _temp2319=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2226)->f4;
goto _LL2249;} else{ goto _LL2250;} _LL2250: if(( unsigned int) _temp2226 > 2u?*((
int*) _temp2226) == Cyc_Absyn_Tuple_p: 0){ _LL2328: _temp2327=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2226)->f1; goto _LL2251;} else{ goto _LL2252;} _LL2252: if(( unsigned int)
_temp2226 > 2u?*(( int*) _temp2226) == Cyc_Absyn_Struct_p: 0){ _LL2336:
_temp2335=(( struct Cyc_Absyn_Struct_p_struct*) _temp2226)->f1; goto _LL2334;
_LL2334: _temp2333=(( struct Cyc_Absyn_Struct_p_struct*) _temp2226)->f2; goto
_LL2332; _LL2332: _temp2331=(( struct Cyc_Absyn_Struct_p_struct*) _temp2226)->f3;
goto _LL2330; _LL2330: _temp2329=(( struct Cyc_Absyn_Struct_p_struct*) _temp2226)->f4;
goto _LL2253;} else{ goto _LL2254;} _LL2254: if(( unsigned int) _temp2226 > 2u?*((
int*) _temp2226) == Cyc_Absyn_Pointer_p: 0){ _LL2338: _temp2337=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2226)->f1; goto _LL2255;} else{ goto _LL2256;} _LL2256: if(( unsigned int)
_temp2226 > 2u?*(( int*) _temp2226) == Cyc_Absyn_UnknownId_p: 0){ _LL2340:
_temp2339=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2226)->f1; goto _LL2257;}
else{ goto _LL2258;} _LL2258: if(( unsigned int) _temp2226 > 2u?*(( int*)
_temp2226) == Cyc_Absyn_UnknownCall_p: 0){ _LL2346: _temp2345=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2226)->f1; goto _LL2344; _LL2344: _temp2343=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2226)->f2; goto _LL2342; _LL2342: _temp2341=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2226)->f3; goto _LL2259;} else{ goto _LL2260;} _LL2260: if(( unsigned int)
_temp2226 > 2u?*(( int*) _temp2226) == Cyc_Absyn_UnknownFields_p: 0){ _LL2352:
_temp2351=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2226)->f1; goto
_LL2350; _LL2350: _temp2349=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2226)->f2; goto _LL2348; _LL2348: _temp2347=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2226)->f3; goto _LL2261;} else{ goto _LL2227;} _LL2229: nv= Cyc_Toc_add_varmap(
nv, _temp2275, r); goto _LL2231; _LL2231: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2227; _LL2233: { struct _tuple0* _temp2353= Cyc_Toc_temp_var(); goto
_LL2354; _LL2354: decls=({ struct Cyc_List_List* _temp2355=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2355->hd=( void*)({
struct _tuple14* _temp2356=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2356->f1= _temp2353; _temp2356->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2356;}); _temp2355->tl= decls;
_temp2355;}); nv= Cyc_Toc_add_varmap( nv, _temp2279->name, Cyc_Absyn_var_exp(
_temp2353, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2353, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2227;} _LL2235: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2227; _LL2237: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2283, _temp2281, 0), succ_lab, fail_lab); goto _LL2227;
_LL2239: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2285, 0),
succ_lab, fail_lab); goto _LL2227; _LL2241: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2287, 0), succ_lab, fail_lab); goto _LL2227; _LL2243: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2357=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2357[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2358; _temp2358.tag= Cyc_Absyn_Enum_e;
_temp2358.f1= _temp2289->name; _temp2358.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2291; _temp2358.f3=( struct Cyc_Absyn_Enumfield*) _temp2289; _temp2358;});
_temp2357;}), 0), succ_lab, fail_lab); goto _LL2227; _LL2245: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2297->name, 0); if( ! _temp2299->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2227;} _LL2247: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2311);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2315->name,({
unsigned char* _temp2392=( unsigned char*)"_struct"; struct _tagged_string
_temp2393; _temp2393.curr= _temp2392; _temp2393.base= _temp2392; _temp2393.last_plus_one=
_temp2392 + 8; _temp2393;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp2359=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2315->typs); goto _LL2360;
_LL2360: { struct Cyc_List_List* _temp2361=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2311); goto
_LL2362; _LL2362: for( 0; _temp2361 != 0;((( _temp2361=(( struct Cyc_List_List*)
_check_null( _temp2361))->tl, _temp2359=(( struct Cyc_List_List*) _check_null(
_temp2359))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp2363=( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp2361))->hd; goto _LL2364; _LL2364: {
void* _temp2365=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2359))->hd)).f2; goto _LL2366; _LL2366: { struct _tuple0* _temp2367= Cyc_Toc_temp_var();
goto _LL2368; _LL2368: { void* _temp2369=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2363->topt))->v; goto _LL2370; _LL2370: { void* _temp2371= Cyc_Toc_typ_to_c(
_temp2369); goto _LL2372; _LL2372: { struct _tagged_string* _temp2373= Cyc_Toc_fresh_label();
goto _LL2374; _LL2374: { struct Cyc_Absyn_Exp* _temp2375= Cyc_Absyn_structarrow_exp(
rcast, Cyc_Toc_fieldname( cnt), 0); goto _LL2376; _LL2376: if( Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c( _temp2365))){ _temp2375= Cyc_Absyn_cast_exp( _temp2371,
_temp2375, 0);} decls=({ struct Cyc_List_List* _temp2377=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2377->hd=( void*)({
struct _tuple14* _temp2378=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2378->f1= _temp2367; _temp2378->f2= _temp2371; _temp2378;});
_temp2377->tl= decls; _temp2377;});{ struct _tuple13 _temp2379= Cyc_Toc_xlate_pat(
nv, rgn, _temp2369, Cyc_Absyn_var_exp( _temp2367, 0), _temp2375, _temp2363,
succ_lab, fail_lab, decls); goto _LL2380; _LL2380: nv= _temp2379.f1; decls=
_temp2379.f2;{ struct Cyc_Absyn_Stmt* _temp2381= _temp2379.f3; goto _LL2382;
_LL2382: { struct Cyc_Absyn_Stmt* _temp2383= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2367, 0), _temp2375, 0); goto _LL2384; _LL2384: { struct Cyc_Absyn_Stmt*
_temp2385= Cyc_Absyn_seq_stmt( _temp2383, _temp2381, 0); goto _LL2386; _LL2386:
ss=({ struct Cyc_List_List* _temp2387=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2387->hd=( void*) Cyc_Absyn_label_stmt(
_temp2373, _temp2385, 0); _temp2387->tl= ss; _temp2387;}); succ_lab= _temp2373;}}}}}}}}}}}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2317->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2315->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2388=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2317->fields))->v; goto _LL2389; _LL2389: for( 0; _temp2388 !=
0; _temp2388=(( struct Cyc_List_List*) _check_null( _temp2388))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2390=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2388))->hd; goto _LL2391; _LL2391: if( _temp2390->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2315->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2227;}}} _LL2249: _temp2327=
_temp2319; goto _LL2251; _LL2251: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2327);{ struct Cyc_List_List*
_temp2394=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2327); goto _LL2395; _LL2395: for( 0; _temp2394 !=
0;( _temp2394=(( struct Cyc_List_List*) _check_null( _temp2394))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2396=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2394))->hd; goto _LL2397; _LL2397: { struct _tuple0* _temp2398=
Cyc_Toc_temp_var(); goto _LL2399; _LL2399: { void* _temp2400=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2396->topt))->v; goto _LL2401; _LL2401: {
struct _tagged_string* _temp2402= Cyc_Toc_fresh_label(); goto _LL2403; _LL2403:
decls=({ struct Cyc_List_List* _temp2404=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2404->hd=( void*)({ struct _tuple14*
_temp2405=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2405->f1= _temp2398; _temp2405->f2= Cyc_Toc_typ_to_c( _temp2400); _temp2405;});
_temp2404->tl= decls; _temp2404;});{ struct _tuple13 _temp2406= Cyc_Toc_xlate_pat(
nv, rgn, _temp2400, Cyc_Absyn_var_exp( _temp2398, 0), Cyc_Absyn_structmember_exp(
path, Cyc_Toc_fieldname( cnt), 0), _temp2396, succ_lab, fail_lab, decls); goto
_LL2407; _LL2407: nv= _temp2406.f1; decls= _temp2406.f2;{ struct Cyc_Absyn_Stmt*
_temp2408= _temp2406.f3; goto _LL2409; _LL2409: { struct Cyc_Absyn_Stmt*
_temp2410= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2398, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2408, 0); goto _LL2411; _LL2411: ss=({ struct Cyc_List_List* _temp2412=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2412->hd=(
void*) Cyc_Absyn_label_stmt( _temp2402, _temp2410, 0); _temp2412->tl= ss;
_temp2412;}); succ_lab= _temp2402;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2227;} _LL2253: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2413=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2329); goto _LL2414; _LL2414: for( 0; _temp2413 !=
0; _temp2413=(( struct Cyc_List_List*) _check_null( _temp2413))->tl){ struct
_tuple15* _temp2415=( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2413))->hd; goto _LL2416; _LL2416: { struct Cyc_Absyn_Pat* _temp2417=(*
_temp2415).f2; goto _LL2418; _LL2418: { struct _tagged_string* f;{ void*
_temp2419=( void*)(( struct Cyc_List_List*) _check_null((* _temp2415).f1))->hd;
struct _tagged_string* _temp2425; _LL2421: if(*(( int*) _temp2419) == Cyc_Absyn_FieldName){
_LL2426: _temp2425=(( struct Cyc_Absyn_FieldName_struct*) _temp2419)->f1; goto
_LL2422;} else{ goto _LL2423;} _LL2423: goto _LL2424; _LL2422: f= _temp2425;
goto _LL2420; _LL2424:( void) _throw(( void*) Cyc_Toc_Match_error); _LL2420:;}{
struct _tuple0* _temp2427= Cyc_Toc_temp_var(); goto _LL2428; _LL2428: { void*
_temp2429=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2417->topt))->v;
goto _LL2430; _LL2430: { void* _temp2431= Cyc_Toc_typ_to_c( _temp2429); goto
_LL2432; _LL2432: { struct _tagged_string* _temp2433= Cyc_Toc_fresh_label();
goto _LL2434; _LL2434: decls=({ struct Cyc_List_List* _temp2435=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2435->hd=( void*)({
struct _tuple14* _temp2436=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2436->f1= _temp2427; _temp2436->f2= _temp2431; _temp2436;});
_temp2435->tl= decls; _temp2435;});{ struct _tuple13 _temp2437= Cyc_Toc_xlate_pat(
nv, rgn, _temp2429, Cyc_Absyn_var_exp( _temp2427, 0), Cyc_Absyn_structmember_exp(
path, f, 0), _temp2417, succ_lab, fail_lab, decls); goto _LL2438; _LL2438: nv=
_temp2437.f1; decls= _temp2437.f2;{ struct Cyc_Absyn_Exp* _temp2439= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2440; _LL2440: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2335->fields))->v, f)))->type)){ _temp2439= Cyc_Absyn_cast_exp(
_temp2431, _temp2439, 0);}{ struct Cyc_Absyn_Stmt* _temp2441= _temp2437.f3; goto
_LL2442; _LL2442: { struct Cyc_Absyn_Stmt* _temp2443= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2427, 0), _temp2439, 0), _temp2441, 0); goto _LL2444;
_LL2444: ss=({ struct Cyc_List_List* _temp2445=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2445->hd=( void*) Cyc_Absyn_label_stmt(
_temp2433, _temp2443, 0); _temp2445->tl= ss; _temp2445;}); succ_lab= _temp2433;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2227;} _LL2255: { struct _tuple0*
_temp2446= Cyc_Toc_temp_var(); goto _LL2447; _LL2447: { void* _temp2448=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2337->topt))->v; goto _LL2449; _LL2449:
decls=({ struct Cyc_List_List* _temp2450=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2450->hd=( void*)({ struct _tuple14*
_temp2451=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2451->f1= _temp2446; _temp2451->f2= Cyc_Toc_typ_to_c( _temp2448); _temp2451;});
_temp2450->tl= decls; _temp2450;});{ struct _tuple13 _temp2452= Cyc_Toc_xlate_pat(
nv, rgn, _temp2448, Cyc_Absyn_var_exp( _temp2446, 0), Cyc_Absyn_deref_exp( path,
0), _temp2337, succ_lab, fail_lab, decls); goto _LL2453; _LL2453: nv= _temp2452.f1;
decls= _temp2452.f2;{ struct Cyc_Absyn_Stmt* _temp2454= _temp2452.f3; goto
_LL2455; _LL2455: { struct Cyc_Absyn_Stmt* _temp2456= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2446, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2454, 0);
goto _LL2457; _LL2457: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2456, 0);} else{ s= _temp2456;} goto _LL2227;}}}}} _LL2257: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2458=( unsigned char*)"unknownid"; struct _tagged_string
_temp2459; _temp2459.curr= _temp2458; _temp2459.base= _temp2458; _temp2459.last_plus_one=
_temp2458 + 10; _temp2459;})); goto _LL2227; _LL2259: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2460=(
unsigned char*)"unknowncall"; struct _tagged_string _temp2461; _temp2461.curr=
_temp2460; _temp2461.base= _temp2460; _temp2461.last_plus_one= _temp2460 + 12;
_temp2461;})); goto _LL2227; _LL2261: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2462=(
unsigned char*)"unknownfields"; struct _tagged_string _temp2463; _temp2463.curr=
_temp2462; _temp2463.base= _temp2462; _temp2463.last_plus_one= _temp2462 + 14;
_temp2463;})); goto _LL2227; _LL2227:;} return({ struct _tuple13 _temp2464;
_temp2464.f1= nv; _temp2464.f2= decls; _temp2464.f3= s; _temp2464;});} struct
_tuple16{ struct _tagged_string* f1; struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple16* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple16* _temp2465=(
struct _tuple16*) _region_malloc( r, sizeof( struct _tuple16)); _temp2465->f1=
Cyc_Toc_fresh_label(); _temp2465->f2= Cyc_Toc_fresh_label(); _temp2465->f3= sc;
_temp2465;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp2466=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2467; _LL2467: { int leave_as_switch;{ void*
_temp2468= Cyc_Tcutil_compress( _temp2466); void* _temp2474; void* _temp2476;
_LL2470: if(( unsigned int) _temp2468 > 4u?*(( int*) _temp2468) == Cyc_Absyn_IntType:
0){ _LL2477: _temp2476=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2468)->f1;
goto _LL2475; _LL2475: _temp2474=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2468)->f2; goto _LL2471;} else{ goto _LL2472;} _LL2472: goto _LL2473;
_LL2471: leave_as_switch= 1; goto _LL2469; _LL2473: leave_as_switch= 0; goto
_LL2469; _LL2469:;}{ struct Cyc_List_List* _temp2478= scs; goto _LL2479; _LL2479:
for( 0; _temp2478 != 0; _temp2478=(( struct Cyc_List_List*) _check_null(
_temp2478))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2478))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2478))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2480= scs; goto
_LL2481; _LL2481: for( 0; _temp2480 != 0; _temp2480=(( struct Cyc_List_List*)
_check_null( _temp2480))->tl){ struct Cyc_Absyn_Stmt* _temp2482=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2480))->hd)->body; goto _LL2483;
_LL2483:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2480))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2482, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2482);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2484=
_new_region(); struct _RegionHandle* rgn=& _temp2484; _push_region( rgn);{
struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2485= lscs; goto
_LL2486; _LL2486: for( 0; _temp2485 != 0; _temp2485=(( struct Cyc_List_List*)
_check_null( _temp2485))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct
_tuple16*)(( struct Cyc_List_List*) _check_null( _temp2485))->hd)).f3; struct
_tagged_string* fail_lab=(( struct Cyc_List_List*) _check_null( _temp2485))->tl
== 0? end_l:(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp2485))->tl))->hd)).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2485))->hd)).f2; if( sc->where_clause == 0){ struct _tuple13
_temp2487= Cyc_Toc_xlate_pat( nv, rgn, _temp2466, r, path, sc->pattern, succ_lab,
fail_lab, decls); goto _LL2488; _LL2488: nvs=({ struct Cyc_List_List* _temp2489=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2489->hd=( void*) _temp2487.f1; _temp2489->tl= nvs; _temp2489;}); decls=
_temp2487.f2; test_stmts=({ struct Cyc_List_List* _temp2490=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2490->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2485))->hd)).f1,
_temp2487.f3, 0); _temp2490->tl= test_stmts; _temp2490;});} else{ struct Cyc_Absyn_Exp*
_temp2491=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2492;
_LL2492: { struct _tagged_string* _temp2493= Cyc_Toc_fresh_label(); goto _LL2494;
_LL2494: { struct _tuple13 _temp2495= Cyc_Toc_xlate_pat( nv, rgn, _temp2466, r,
path, sc->pattern, _temp2493, fail_lab, decls); goto _LL2496; _LL2496: Cyc_Toc_exp_to_c(
_temp2495.f1, _temp2491); nvs=({ struct Cyc_List_List* _temp2497=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2497->hd=( void*)
_temp2495.f1; _temp2497->tl= nvs; _temp2497;}); decls= _temp2495.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2491, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2493, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2498=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2498->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2485))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2495.f3, s3, 0), 0); _temp2498->tl= test_stmts;
_temp2498;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2499= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2500; _LL2500: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2501=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2502; _LL2502: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2501, end_l,(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2503=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2504; _LL2504: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2503, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2499, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple14 _temp2507;
void* _temp2508; struct _tuple0* _temp2510; struct _tuple14* _temp2505=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2507=*
_temp2505; _LL2511: _temp2510= _temp2507.f1; goto _LL2509; _LL2509: _temp2508=
_temp2507.f2; goto _LL2506; _LL2506: res= Cyc_Absyn_declare_stmt( _temp2510,
_temp2508, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region(& _temp2484);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2512=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2512[ 0]=({ struct Cyc_List_List _temp2513; _temp2513.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2513.tl= 0; _temp2513;}); _temp2512;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2514=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2556; struct Cyc_Absyn_Stmt* _temp2558; struct Cyc_Absyn_Stmt*
_temp2560; struct Cyc_Absyn_Exp* _temp2562; struct Cyc_Absyn_Stmt* _temp2564;
struct Cyc_Absyn_Stmt* _temp2566; struct Cyc_Absyn_Exp* _temp2568; struct Cyc_Absyn_Stmt*
_temp2570; struct _tuple2 _temp2572; struct Cyc_Absyn_Stmt* _temp2574; struct
Cyc_Absyn_Exp* _temp2576; struct Cyc_Absyn_Stmt* _temp2578; struct Cyc_Absyn_Stmt*
_temp2580; struct Cyc_Absyn_Stmt* _temp2582; struct _tagged_string* _temp2584;
struct Cyc_Absyn_Stmt* _temp2586; struct _tuple2 _temp2588; struct Cyc_Absyn_Stmt*
_temp2590; struct Cyc_Absyn_Exp* _temp2592; struct _tuple2 _temp2594; struct Cyc_Absyn_Stmt*
_temp2596; struct Cyc_Absyn_Exp* _temp2598; struct Cyc_Absyn_Exp* _temp2600;
struct Cyc_List_List* _temp2602; struct Cyc_Absyn_Exp* _temp2604; struct Cyc_Absyn_Switch_clause**
_temp2606; struct Cyc_List_List* _temp2608; struct Cyc_Absyn_Stmt* _temp2610;
struct Cyc_Absyn_Decl* _temp2612; struct Cyc_Absyn_Stmt* _temp2614; struct
_tagged_string* _temp2616; struct _tuple2 _temp2618; struct Cyc_Absyn_Stmt*
_temp2620; struct Cyc_Absyn_Exp* _temp2622; struct Cyc_Absyn_Stmt* _temp2624;
struct Cyc_List_List* _temp2626; struct Cyc_Absyn_Stmt* _temp2628; struct Cyc_Absyn_Stmt*
_temp2630; struct Cyc_Absyn_Vardecl* _temp2632; struct Cyc_Absyn_Tvar* _temp2634;
struct Cyc_List_List* _temp2636; struct Cyc_Absyn_Exp* _temp2638; struct Cyc_Absyn_Stmt*
_temp2640; struct Cyc_Absyn_Stmt* _temp2642; _LL2516: if( _temp2514 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2517;} else{ goto _LL2518;} _LL2518: if((
unsigned int) _temp2514 > 1u?*(( int*) _temp2514) == Cyc_Absyn_Exp_s: 0){
_LL2557: _temp2556=(( struct Cyc_Absyn_Exp_s_struct*) _temp2514)->f1; goto
_LL2519;} else{ goto _LL2520;} _LL2520: if(( unsigned int) _temp2514 > 1u?*((
int*) _temp2514) == Cyc_Absyn_Seq_s: 0){ _LL2561: _temp2560=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2514)->f1; goto _LL2559; _LL2559: _temp2558=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2514)->f2; goto _LL2521;} else{ goto _LL2522;} _LL2522: if(( unsigned int)
_temp2514 > 1u?*(( int*) _temp2514) == Cyc_Absyn_Return_s: 0){ _LL2563:
_temp2562=(( struct Cyc_Absyn_Return_s_struct*) _temp2514)->f1; goto _LL2523;}
else{ goto _LL2524;} _LL2524: if(( unsigned int) _temp2514 > 1u?*(( int*)
_temp2514) == Cyc_Absyn_IfThenElse_s: 0){ _LL2569: _temp2568=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2514)->f1; goto _LL2567; _LL2567: _temp2566=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2514)->f2; goto _LL2565; _LL2565: _temp2564=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2514)->f3; goto _LL2525;} else{ goto _LL2526;} _LL2526: if(( unsigned int)
_temp2514 > 1u?*(( int*) _temp2514) == Cyc_Absyn_While_s: 0){ _LL2573: _temp2572=((
struct Cyc_Absyn_While_s_struct*) _temp2514)->f1; _LL2577: _temp2576= _temp2572.f1;
goto _LL2575; _LL2575: _temp2574= _temp2572.f2; goto _LL2571; _LL2571: _temp2570=((
struct Cyc_Absyn_While_s_struct*) _temp2514)->f2; goto _LL2527;} else{ goto
_LL2528;} _LL2528: if(( unsigned int) _temp2514 > 1u?*(( int*) _temp2514) == Cyc_Absyn_Break_s:
0){ _LL2579: _temp2578=(( struct Cyc_Absyn_Break_s_struct*) _temp2514)->f1; goto
_LL2529;} else{ goto _LL2530;} _LL2530: if(( unsigned int) _temp2514 > 1u?*((
int*) _temp2514) == Cyc_Absyn_Continue_s: 0){ _LL2581: _temp2580=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2514)->f1; goto _LL2531;} else{ goto _LL2532;} _LL2532: if(( unsigned int)
_temp2514 > 1u?*(( int*) _temp2514) == Cyc_Absyn_Goto_s: 0){ _LL2585: _temp2584=((
struct Cyc_Absyn_Goto_s_struct*) _temp2514)->f1; goto _LL2583; _LL2583:
_temp2582=(( struct Cyc_Absyn_Goto_s_struct*) _temp2514)->f2; goto _LL2533;}
else{ goto _LL2534;} _LL2534: if(( unsigned int) _temp2514 > 1u?*(( int*)
_temp2514) == Cyc_Absyn_For_s: 0){ _LL2601: _temp2600=(( struct Cyc_Absyn_For_s_struct*)
_temp2514)->f1; goto _LL2595; _LL2595: _temp2594=(( struct Cyc_Absyn_For_s_struct*)
_temp2514)->f2; _LL2599: _temp2598= _temp2594.f1; goto _LL2597; _LL2597:
_temp2596= _temp2594.f2; goto _LL2589; _LL2589: _temp2588=(( struct Cyc_Absyn_For_s_struct*)
_temp2514)->f3; _LL2593: _temp2592= _temp2588.f1; goto _LL2591; _LL2591:
_temp2590= _temp2588.f2; goto _LL2587; _LL2587: _temp2586=(( struct Cyc_Absyn_For_s_struct*)
_temp2514)->f4; goto _LL2535;} else{ goto _LL2536;} _LL2536: if(( unsigned int)
_temp2514 > 1u?*(( int*) _temp2514) == Cyc_Absyn_Switch_s: 0){ _LL2605:
_temp2604=(( struct Cyc_Absyn_Switch_s_struct*) _temp2514)->f1; goto _LL2603;
_LL2603: _temp2602=(( struct Cyc_Absyn_Switch_s_struct*) _temp2514)->f2; goto
_LL2537;} else{ goto _LL2538;} _LL2538: if(( unsigned int) _temp2514 > 1u?*((
int*) _temp2514) == Cyc_Absyn_Fallthru_s: 0){ _LL2609: _temp2608=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2514)->f1; goto _LL2607; _LL2607: _temp2606=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2514)->f2; goto _LL2539;} else{ goto _LL2540;} _LL2540: if(( unsigned int)
_temp2514 > 1u?*(( int*) _temp2514) == Cyc_Absyn_Decl_s: 0){ _LL2613: _temp2612=((
struct Cyc_Absyn_Decl_s_struct*) _temp2514)->f1; goto _LL2611; _LL2611:
_temp2610=(( struct Cyc_Absyn_Decl_s_struct*) _temp2514)->f2; goto _LL2541;}
else{ goto _LL2542;} _LL2542: if(( unsigned int) _temp2514 > 1u?*(( int*)
_temp2514) == Cyc_Absyn_Label_s: 0){ _LL2617: _temp2616=(( struct Cyc_Absyn_Label_s_struct*)
_temp2514)->f1; goto _LL2615; _LL2615: _temp2614=(( struct Cyc_Absyn_Label_s_struct*)
_temp2514)->f2; goto _LL2543;} else{ goto _LL2544;} _LL2544: if(( unsigned int)
_temp2514 > 1u?*(( int*) _temp2514) == Cyc_Absyn_Do_s: 0){ _LL2625: _temp2624=((
struct Cyc_Absyn_Do_s_struct*) _temp2514)->f1; goto _LL2619; _LL2619: _temp2618=((
struct Cyc_Absyn_Do_s_struct*) _temp2514)->f2; _LL2623: _temp2622= _temp2618.f1;
goto _LL2621; _LL2621: _temp2620= _temp2618.f2; goto _LL2545;} else{ goto
_LL2546;} _LL2546: if(( unsigned int) _temp2514 > 1u?*(( int*) _temp2514) == Cyc_Absyn_TryCatch_s:
0){ _LL2629: _temp2628=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2514)->f1;
goto _LL2627; _LL2627: _temp2626=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2514)->f2; goto _LL2547;} else{ goto _LL2548;} _LL2548: if(( unsigned int)
_temp2514 > 1u?*(( int*) _temp2514) == Cyc_Absyn_Region_s: 0){ _LL2635:
_temp2634=(( struct Cyc_Absyn_Region_s_struct*) _temp2514)->f1; goto _LL2633;
_LL2633: _temp2632=(( struct Cyc_Absyn_Region_s_struct*) _temp2514)->f2; goto
_LL2631; _LL2631: _temp2630=(( struct Cyc_Absyn_Region_s_struct*) _temp2514)->f3;
goto _LL2549;} else{ goto _LL2550;} _LL2550: if(( unsigned int) _temp2514 > 1u?*((
int*) _temp2514) == Cyc_Absyn_SwitchC_s: 0){ _LL2639: _temp2638=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2514)->f1; goto _LL2637; _LL2637: _temp2636=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2514)->f2; goto _LL2551;} else{ goto _LL2552;} _LL2552: if(( unsigned int)
_temp2514 > 1u?*(( int*) _temp2514) == Cyc_Absyn_Cut_s: 0){ _LL2641: _temp2640=((
struct Cyc_Absyn_Cut_s_struct*) _temp2514)->f1; goto _LL2553;} else{ goto
_LL2554;} _LL2554: if(( unsigned int) _temp2514 > 1u?*(( int*) _temp2514) == Cyc_Absyn_Splice_s:
0){ _LL2643: _temp2642=(( struct Cyc_Absyn_Splice_s_struct*) _temp2514)->f1;
goto _LL2555;} else{ goto _LL2515;} _LL2517: return; _LL2519: Cyc_Toc_exp_to_c(
nv, _temp2556); return; _LL2521: Cyc_Toc_stmt_to_c( nv, _temp2560); s= _temp2558;
continue; _LL2523: { struct Cyc_Core_Opt* topt= 0; if( _temp2562 != 0){ topt=({
struct Cyc_Core_Opt* _temp2644=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2644->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2562))->topt))->v);
_temp2644;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2562));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2645=
Cyc_Toc_temp_var(); goto _LL2646; _LL2646: { struct Cyc_Absyn_Stmt* _temp2647=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2645, 0),
0); goto _LL2648; _LL2648:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2645,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2562,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2647, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2525: Cyc_Toc_exp_to_c(
nv, _temp2568); Cyc_Toc_stmt_to_c( nv, _temp2566); s= _temp2564; continue;
_LL2527: Cyc_Toc_exp_to_c( nv, _temp2576); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2570); return; _LL2529: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2578 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2578))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2531: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -(( struct Cyc_Absyn_Stmt*) _check_null( _temp2580))->try_depth, s);
return; _LL2533: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2582))->try_depth, s); return; _LL2535: Cyc_Toc_exp_to_c( nv,
_temp2600); Cyc_Toc_exp_to_c( nv, _temp2598); Cyc_Toc_exp_to_c( nv, _temp2592);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2586); return; _LL2537: Cyc_Toc_xlate_switch(
nv, s, _temp2604, _temp2602); return; _LL2539: if( nv->fallthru_info == 0){(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2649=( unsigned char*)"fallthru in unexpected place"; struct
_tagged_string _temp2650; _temp2650.curr= _temp2649; _temp2650.base= _temp2649;
_temp2650.last_plus_one= _temp2649 + 29; _temp2650;})));}{ struct _tuple8
_temp2653; struct Cyc_Dict_Dict* _temp2654; struct Cyc_List_List* _temp2656;
struct _tagged_string* _temp2658; struct _tuple8* _temp2651=( struct _tuple8*)((
struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp2653=* _temp2651;
_LL2659: _temp2658= _temp2653.f1; goto _LL2657; _LL2657: _temp2656= _temp2653.f2;
goto _LL2655; _LL2655: _temp2654= _temp2653.f3; goto _LL2652; _LL2652: { struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2658, 0); Cyc_Toc_do_npop_before(
s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp2606)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp2660=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2656); goto _LL2661; _LL2661: { struct Cyc_List_List*
_temp2662=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2608); goto _LL2663; _LL2663: for( 0; _temp2660 != 0;( _temp2660=(( struct
Cyc_List_List*) _check_null( _temp2660))->tl, _temp2662=(( struct Cyc_List_List*)
_check_null( _temp2662))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp2662))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp2654,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp2660))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp2662))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}}} _LL2541:{ void* _temp2664=( void*) _temp2612->r; int
_temp2674; struct Cyc_Absyn_Exp* _temp2676; struct Cyc_Core_Opt* _temp2678;
struct Cyc_Core_Opt* _temp2680; struct Cyc_Absyn_Pat* _temp2682; struct Cyc_List_List*
_temp2684; struct Cyc_Absyn_Vardecl* _temp2686; _LL2666: if(*(( int*) _temp2664)
== Cyc_Absyn_Let_d){ _LL2683: _temp2682=(( struct Cyc_Absyn_Let_d_struct*)
_temp2664)->f1; goto _LL2681; _LL2681: _temp2680=(( struct Cyc_Absyn_Let_d_struct*)
_temp2664)->f2; goto _LL2679; _LL2679: _temp2678=(( struct Cyc_Absyn_Let_d_struct*)
_temp2664)->f3; goto _LL2677; _LL2677: _temp2676=(( struct Cyc_Absyn_Let_d_struct*)
_temp2664)->f4; goto _LL2675; _LL2675: _temp2674=(( struct Cyc_Absyn_Let_d_struct*)
_temp2664)->f5; goto _LL2667;} else{ goto _LL2668;} _LL2668: if(*(( int*)
_temp2664) == Cyc_Absyn_Letv_d){ _LL2685: _temp2684=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2664)->f1; goto _LL2669;} else{ goto _LL2670;} _LL2670: if(*(( int*)
_temp2664) == Cyc_Absyn_Var_d){ _LL2687: _temp2686=(( struct Cyc_Absyn_Var_d_struct*)
_temp2664)->f1; goto _LL2671;} else{ goto _LL2672;} _LL2672: goto _LL2673;
_LL2667:( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2682,
_temp2678, _temp2676, _temp2674, _temp2610))->r)); goto _LL2665; _LL2669: {
struct Cyc_List_List* _temp2688=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2684); goto _LL2689; _LL2689: if( _temp2688 == 0){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2690=(
unsigned char*)"empty Letv_d"; struct _tagged_string _temp2691; _temp2691.curr=
_temp2690; _temp2691.base= _temp2690; _temp2691.last_plus_one= _temp2690 + 13;
_temp2691;}));}( void*)( _temp2612->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2692=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2692[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2693; _temp2693.tag= Cyc_Absyn_Var_d;
_temp2693.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2688))->hd; _temp2693;}); _temp2692;}))); _temp2688=(( struct Cyc_List_List*)
_check_null( _temp2688))->tl; for( 0; _temp2688 != 0; _temp2688=(( struct Cyc_List_List*)
_check_null( _temp2688))->tl){ struct Cyc_Absyn_Decl* _temp2696= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2694=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2694[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2695; _temp2695.tag= Cyc_Absyn_Var_d; _temp2695.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2688))->hd; _temp2695;}); _temp2694;}),
0); goto _LL2697; _LL2697:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp2696, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c(
nv, s); goto _LL2665;} _LL2671: { struct _tuple0* _temp2698= _temp2686->name;
goto _LL2699; _LL2699:( void*)( _temp2686->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2686->type));{ struct Cyc_Toc_Env* _temp2702= Cyc_Toc_add_varmap( nv,
_temp2698, Cyc_Absyn_varb_exp( _temp2698,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2700=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2700[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2701; _temp2701.tag= Cyc_Absyn_Local_b;
_temp2701.f1= _temp2686; _temp2701;}); _temp2700;}), 0)); goto _LL2703; _LL2703:
Cyc_Toc_stmt_to_c( _temp2702, _temp2610); if( _temp2686->initializer != 0){
struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*) _check_null( _temp2686->initializer);
void* _temp2704=( void*) init->r; struct Cyc_Absyn_Exp* _temp2710; struct Cyc_Absyn_Exp*
_temp2712; struct Cyc_Absyn_Vardecl* _temp2714; _LL2706: if(*(( int*) _temp2704)
== Cyc_Absyn_Comprehension_e){ _LL2715: _temp2714=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2704)->f1; goto _LL2713; _LL2713: _temp2712=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2704)->f2; goto _LL2711; _LL2711: _temp2710=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2704)->f3; goto _LL2707;} else{ goto _LL2708;} _LL2708: goto _LL2709;
_LL2707: _temp2686->initializer= 0;( void*)( _temp2610->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2698, 0), _temp2714, _temp2712, _temp2710, Cyc_Absyn_new_stmt((
void*) _temp2610->r, 0), 0))->r)); goto _LL2705; _LL2709: Cyc_Toc_exp_to_c( nv,
init); goto _LL2705; _LL2705:;} goto _LL2665;}} _LL2673:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2716=( unsigned char*)"bad nested declaration within function";
struct _tagged_string _temp2717; _temp2717.curr= _temp2716; _temp2717.base=
_temp2716; _temp2717.last_plus_one= _temp2716 + 39; _temp2717;})); goto _LL2665;
_LL2665:;} return; _LL2543: s= _temp2614; continue; _LL2545: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp2624); Cyc_Toc_exp_to_c( nv, _temp2622); return;
_LL2547: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__push_handler_qv,
0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp( Cyc_Toc_setjmp_qv, 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__pop_handler_qv,
0); struct Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp( Cyc_Toc__exn_thrown_qv,
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2718=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2718->v=( void*) e_typ; _temp2718;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2628);{
struct Cyc_Absyn_Stmt* _temp2719= Cyc_Absyn_seq_stmt( _temp2628, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); goto _LL2720; _LL2720: {
struct _tuple0* _temp2721= Cyc_Toc_temp_var(); goto _LL2722; _LL2722: { struct
Cyc_Absyn_Exp* _temp2723= Cyc_Absyn_var_exp( _temp2721, 0); goto _LL2724;
_LL2724: { struct Cyc_Absyn_Vardecl* _temp2725= Cyc_Absyn_new_vardecl( _temp2721,
Cyc_Absyn_exn_typ, 0); goto _LL2726; _LL2726: _temp2723->topt=({ struct Cyc_Core_Opt*
_temp2727=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2727->v=( void*) Cyc_Absyn_exn_typ; _temp2727;});{ struct Cyc_Absyn_Pat*
_temp2732=({ struct Cyc_Absyn_Pat* _temp2728=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2728->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2730=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2730[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2731; _temp2731.tag= Cyc_Absyn_Var_p;
_temp2731.f1= _temp2725; _temp2731;}); _temp2730;})); _temp2728->topt=({ struct
Cyc_Core_Opt* _temp2729=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2729->v=( void*) Cyc_Absyn_exn_typ; _temp2729;}); _temp2728->loc= 0;
_temp2728;}); goto _LL2733; _LL2733: { struct Cyc_Absyn_Exp* _temp2734= Cyc_Absyn_throw_exp(
_temp2723, 0); goto _LL2735; _LL2735: _temp2734->topt=({ struct Cyc_Core_Opt*
_temp2736=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2736->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2736;});{ struct Cyc_Absyn_Stmt*
_temp2737= Cyc_Absyn_exp_stmt( _temp2734, 0); goto _LL2738; _LL2738: { struct
Cyc_Absyn_Switch_clause* _temp2742=({ struct Cyc_Absyn_Switch_clause* _temp2739=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2739->pattern= _temp2732; _temp2739->pat_vars=({ struct Cyc_Core_Opt*
_temp2740=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2740->v=( void*)({ struct Cyc_List_List* _temp2741=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2741->hd=( void*) _temp2725;
_temp2741->tl= 0; _temp2741;}); _temp2740;}); _temp2739->where_clause= 0;
_temp2739->body= _temp2737; _temp2739->loc= 0; _temp2739;}); goto _LL2743;
_LL2743: { struct Cyc_Absyn_Stmt* _temp2745= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2626,({ struct Cyc_List_List* _temp2744=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2744->hd=( void*) _temp2742; _temp2744->tl=
0; _temp2744;})), 0); goto _LL2746; _LL2746: Cyc_Toc_stmt_to_c( nv, _temp2745);{
struct Cyc_Absyn_Exp* _temp2748= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2747=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2747->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2747->tl= 0; _temp2747;}), 0); goto _LL2749; _LL2749: { struct Cyc_Absyn_Stmt*
_temp2751= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2750=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2750->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2750->tl= 0;
_temp2750;}), 0), 0); goto _LL2752; _LL2752: { struct Cyc_Absyn_Exp* _temp2753=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2754; _LL2754: {
struct Cyc_Absyn_Exp* _temp2755= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2756; _LL2756:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2751, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2753, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2748, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2755, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2719, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2745, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2549: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2632->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2762=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2762[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2763; _temp2763.tag= Cyc_Absyn_Local_b; _temp2763.f1=
_temp2632; _temp2763;}); _temp2762;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2761=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2761->f1= Cyc_Toc_rel_ns; _temp2761->f2= Cyc_Toc__push_region_sp;
_temp2761;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2760=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2760->f1= Cyc_Toc_rel_ns; _temp2760->f2= Cyc_Toc__pop_region_sp; _temp2760;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2759=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2759->f1=
Cyc_Toc_rel_ns; _temp2759->f2= Cyc_Toc__new_region_sp; _temp2759;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2758=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2758->hd=( void*) addr_rh_exp; _temp2758->tl= 0; _temp2758;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2757=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2757->hd=( void*) x_exp; _temp2757->tl= 0; _temp2757;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2630);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2630, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2551:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2764=(
unsigned char*)"switchC_s"; struct _tagged_string _temp2765; _temp2765.curr=
_temp2764; _temp2765.base= _temp2764; _temp2765.last_plus_one= _temp2764 + 10;
_temp2765;})); return; _LL2553:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2766=( unsigned char*)"cut"; struct _tagged_string _temp2767;
_temp2767.curr= _temp2766; _temp2767.base= _temp2766; _temp2767.last_plus_one=
_temp2766 + 4; _temp2767;})); return; _LL2555:(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(({ unsigned char* _temp2768=( unsigned char*)"splice"; struct
_tagged_string _temp2769; _temp2769.curr= _temp2768; _temp2769.base= _temp2768;
_temp2769.last_plus_one= _temp2768 + 7; _temp2769;})); return; _LL2515:;}}
struct _tuple17{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp2770= f->args; goto _LL2771;
_LL2771: for( 0; _temp2770 != 0; _temp2770=(( struct Cyc_List_List*) _check_null(
_temp2770))->tl){(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2770))->hd)).f3= Cyc_Toc_typ_to_c((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp2770))->hd)).f3);{ struct _tuple0* _temp2773=({ struct _tuple0*
_temp2772=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2772->f1=(
void*) Cyc_Absyn_Loc_n; _temp2772->f2=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp2770))->hd)).f1; _temp2772;}); goto _LL2774; _LL2774: nv= Cyc_Toc_add_varmap(
nv, _temp2773, Cyc_Absyn_var_exp( _temp2773, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2777; void* _temp2779; void* _temp2781; struct Cyc_Absyn_Tqual _temp2783;
struct Cyc_Core_Opt* _temp2785; struct Cyc_Absyn_VarargInfo _temp2775=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2786: _temp2785=
_temp2775.name; goto _LL2784; _LL2784: _temp2783= _temp2775.tq; goto _LL2782;
_LL2782: _temp2781=( void*) _temp2775.type; goto _LL2780; _LL2780: _temp2779=(
void*) _temp2775.rgn; goto _LL2778; _LL2778: _temp2777= _temp2775.inject; goto
_LL2776; _LL2776: { void* _temp2787= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2781, _temp2779, _temp2783)); goto _LL2788; _LL2788: { struct _tuple0*
_temp2790=({ struct _tuple0* _temp2789=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2789->f1=( void*) Cyc_Absyn_Loc_n; _temp2789->f2=( struct
_tagged_string*)(( struct Cyc_Core_Opt*) _check_null( _temp2785))->v; _temp2789;});
goto _LL2791; _LL2791: f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2792=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2792->hd=( void*)({ struct _tuple17* _temp2793=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp2793->f1=( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( _temp2785))->v; _temp2793->f2= _temp2783;
_temp2793->f3= _temp2787; _temp2793;}); _temp2792->tl= 0; _temp2792;})); nv= Cyc_Toc_add_varmap(
nv, _temp2790, Cyc_Absyn_var_exp( _temp2790, 0)); f->cyc_varargs= 0;}}}{ struct
Cyc_List_List* _temp2794=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; goto _LL2795; _LL2795: for( 0; _temp2794 !=
0; _temp2794=(( struct Cyc_List_List*) _check_null( _temp2794))->tl){( void*)(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp2794))->hd)->type=(
void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp2794))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2796= s;
_LL2798: if( _temp2796 ==( void*) Cyc_Absyn_Abstract){ goto _LL2799;} else{ goto
_LL2800;} _LL2800: if( _temp2796 ==( void*) Cyc_Absyn_ExternC){ goto _LL2801;}
else{ goto _LL2802;} _LL2802: goto _LL2803; _LL2799: return( void*) Cyc_Absyn_Public;
_LL2801: return( void*) Cyc_Absyn_Extern; _LL2803: return s; _LL2797:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2804=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2805; _LL2805: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2806=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2806->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2806;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2807=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2804).f2); goto _LL2808;
_LL2808: if( _temp2807 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2804).f2, s));}
else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null(
_temp2807))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2804).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2809=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2809->v=( void*)
_temp2804; _temp2809;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2810=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2811; _LL2811: for( 0; _temp2810 != 0;
_temp2810=(( struct Cyc_List_List*) _check_null( _temp2810))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2810))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2810))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2812=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2813; _LL2813: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2814=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2814->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2814;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2815=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2812).f2); goto _LL2816; _LL2816:
if( _temp2815 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2812).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2815))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2812).f2, u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)(
u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==(
void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*)
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2817=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2817->v=( void*) _temp2812; _temp2817;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2818=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( u->fields))->v; goto
_LL2819; _LL2819: for( 0; _temp2818 != 0; _temp2818=(( struct Cyc_List_List*)
_check_null( _temp2818))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2818))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2818))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2820=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2820->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2820;});}{ struct
_tuple0* _temp2821= tud->name; goto _LL2822; _LL2822: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2821)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2821));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2823=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2824; _LL2824: for( 0;
_temp2823 != 0; _temp2823=(( struct Cyc_List_List*) _check_null( _temp2823))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2823))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2825; _temp2825.q_const= 1; _temp2825.q_volatile= 0;
_temp2825.q_restrict= 0; _temp2825;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2826=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2826->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2827=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2827[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2828; _temp2828.tag= Cyc_Absyn_Var_d;
_temp2828.f1= vd; _temp2828;}); _temp2827;}), 0); _temp2826->tl= Cyc_Toc_result_decls;
_temp2826;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2829; _temp2829.q_const=
1; _temp2829.q_volatile= 0; _temp2829.q_restrict= 0; _temp2829;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2830=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2830->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2831=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2831[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2832; _temp2832.tag= Cyc_Absyn_Var_d; _temp2832.f1= vd; _temp2832;});
_temp2831;}), 0); _temp2830->tl= Cyc_Toc_result_decls; _temp2830;});{ struct Cyc_List_List*
_temp2833= 0; goto _LL2834; _LL2834: { int i= 1;{ struct Cyc_List_List*
_temp2835= f->typs; goto _LL2836; _LL2836: for( 0; _temp2835 != 0;( _temp2835=((
struct Cyc_List_List*) _check_null( _temp2835))->tl, i ++)){ struct
_tagged_string* _temp2837= Cyc_Toc_fieldname( i); goto _LL2838; _LL2838: {
struct Cyc_Absyn_Structfield* _temp2840=({ struct Cyc_Absyn_Structfield*
_temp2839=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2839->name= _temp2837; _temp2839->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2835))->hd)).f1; _temp2839->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2835))->hd)).f2);
_temp2839->width= 0; _temp2839->attributes= 0; _temp2839;}); goto _LL2841;
_LL2841: _temp2833=({ struct Cyc_List_List* _temp2842=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2842->hd=( void*) _temp2840;
_temp2842->tl= _temp2833; _temp2842;});}}} _temp2833=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2833); _temp2833=({ struct Cyc_List_List*
_temp2843=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2843->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2844=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2844->name= Cyc_Toc_tag_sp;
_temp2844->tq= Cyc_Toc_mt_tq; _temp2844->type=( void*) Cyc_Absyn_sint_t;
_temp2844->width= 0; _temp2844->attributes= 0; _temp2844;}); _temp2843->tl=
_temp2833; _temp2843;});{ struct Cyc_Absyn_Structdecl* _temp2850=({ struct Cyc_Absyn_Structdecl*
_temp2845=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2845->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2845->name=({ struct Cyc_Core_Opt*
_temp2847=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2847->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2848=( unsigned char*)"_struct"; struct _tagged_string _temp2849; _temp2849.curr=
_temp2848; _temp2849.base= _temp2848; _temp2849.last_plus_one= _temp2848 + 8;
_temp2849;})); _temp2847;}); _temp2845->tvs= 0; _temp2845->fields=({ struct Cyc_Core_Opt*
_temp2846=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2846->v=( void*) _temp2833; _temp2846;}); _temp2845->attributes= 0;
_temp2845;}); goto _LL2851; _LL2851: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2852=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2852->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2853=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2853[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2854; _temp2854.tag= Cyc_Absyn_Struct_d;
_temp2854.f1= _temp2850; _temp2854;}); _temp2853;}), 0); _temp2852->tl= Cyc_Toc_result_decls;
_temp2852;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2855=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2855->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2855;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2856= xd->name; goto _LL2857; _LL2857: {
struct Cyc_List_List* _temp2858=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2859; _LL2859: for( 0; _temp2858 != 0;
_temp2858=(( struct Cyc_List_List*) _check_null( _temp2858))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2858))->hd; struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2861= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2860=* fn;(
unsigned int)( _temp2860.last_plus_one - _temp2860.curr);}) + 4, 0); goto
_LL2862; _LL2862: { struct Cyc_Absyn_ArrayType_struct* _temp2865=({ struct Cyc_Absyn_ArrayType_struct*
_temp2863=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2863[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2864; _temp2864.tag= Cyc_Absyn_ArrayType;
_temp2864.f1=( void*) Cyc_Absyn_uchar_t; _temp2864.f2= Cyc_Toc_mt_tq; _temp2864.f3=(
struct Cyc_Absyn_Exp*) _temp2861; _temp2864;}); _temp2863;}); goto _LL2866;
_LL2866: { struct Cyc_Core_Opt* _temp2867=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2875; int _temp2876; _LL2869: if( _temp2867 == 0){ goto
_LL2870;} else{ goto _LL2871;} _LL2871: if( _temp2867 == 0){ goto _LL2873;}
else{ _temp2875=* _temp2867; _LL2877: _temp2876=( int) _temp2875.v; if(
_temp2876 == 0){ goto _LL2872;} else{ goto _LL2873;}} _LL2873: goto _LL2874;
_LL2870: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_string)({
int _temp2878= 0; int _temp2879= 0; int _temp2880= 0; int _temp2881= 0; struct
_tagged_string _temp2882=* fn; xprintf("%c%c%c%c%.*s", _temp2878, _temp2879,
_temp2880, _temp2881, _temp2882.last_plus_one - _temp2882.curr, _temp2882.curr);}),
0);}{ struct Cyc_Absyn_Vardecl* _temp2883= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2865, initopt); goto _LL2884; _LL2884:( void*)( _temp2883->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2885=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2885->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2886=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2886[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2887; _temp2887.tag= Cyc_Absyn_Var_d;
_temp2887.f1= _temp2883; _temp2887;}); _temp2886;}), 0); _temp2885->tl= Cyc_Toc_result_decls;
_temp2885;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2888= f->typs; goto _LL2889; _LL2889: for( 0;
_temp2888 != 0;( _temp2888=(( struct Cyc_List_List*) _check_null( _temp2888))->tl,
i ++)){ struct _tagged_string* _temp2891=({ struct _tagged_string* _temp2890=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp2890[ 0]=(
struct _tagged_string) xprintf("f%d", i); _temp2890;}); goto _LL2892; _LL2892: {
struct Cyc_Absyn_Structfield* _temp2894=({ struct Cyc_Absyn_Structfield*
_temp2893=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2893->name= _temp2891; _temp2893->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2888))->hd)).f1; _temp2893->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2888))->hd)).f2);
_temp2893->width= 0; _temp2893->attributes= 0; _temp2893;}); goto _LL2895;
_LL2895: fields=({ struct Cyc_List_List* _temp2896=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2896->hd=( void*) _temp2894;
_temp2896->tl= fields; _temp2896;});}}} fields=({ struct Cyc_List_List*
_temp2897=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2897->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2898=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2898->name= Cyc_Toc_tag_sp;
_temp2898->tq= Cyc_Toc_mt_tq; _temp2898->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2898->width= 0; _temp2898->attributes= 0; _temp2898;});
_temp2897->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2897;});{ struct Cyc_Absyn_Structdecl* _temp2904=({ struct Cyc_Absyn_Structdecl*
_temp2899=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2899->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2899->name=({ struct Cyc_Core_Opt*
_temp2901=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2901->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2902=( unsigned char*)"_struct"; struct _tagged_string _temp2903; _temp2903.curr=
_temp2902; _temp2903.base= _temp2902; _temp2903.last_plus_one= _temp2902 + 8;
_temp2903;})); _temp2901;}); _temp2899->tvs= 0; _temp2899->fields=({ struct Cyc_Core_Opt*
_temp2900=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2900->v=( void*) fields; _temp2900;}); _temp2899->attributes= 0; _temp2899;});
goto _LL2905; _LL2905: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2906=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2906->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2907=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2907[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2908; _temp2908.tag= Cyc_Absyn_Struct_d;
_temp2908.f1= _temp2904; _temp2908;}); _temp2907;}), 0); _temp2906->tl= Cyc_Toc_result_decls;
_temp2906;});}} goto _LL2868;}} _LL2872: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2914= Cyc_Absyn_string_exp(( struct _tagged_string)({
int _temp2909= 0; int _temp2910= 0; int _temp2911= 0; int _temp2912= 0; struct
_tagged_string _temp2913=* fn; xprintf("%c%c%c%c%.*s", _temp2909, _temp2910,
_temp2911, _temp2912, _temp2913.last_plus_one - _temp2913.curr, _temp2913.curr);}),
0); goto _LL2915; _LL2915: { struct Cyc_Absyn_Vardecl* _temp2916= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2865,( struct Cyc_Absyn_Exp*) _temp2914); goto _LL2917;
_LL2917:( void*)( _temp2916->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2918=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2918->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2919=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2919[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2920; _temp2920.tag= Cyc_Absyn_Var_d; _temp2920.f1= _temp2916; _temp2920;});
_temp2919;}), 0); _temp2918->tl= Cyc_Toc_result_decls; _temp2918;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2868; _LL2874: goto
_LL2868; _LL2868:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2921= ed->name; goto
_LL2922; _LL2922:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2923=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2924; _LL2924: for( 0; _temp2923 != 0;
_temp2923=(( struct Cyc_List_List*) _check_null( _temp2923))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2923))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2923))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2923))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct _RegionHandle
_temp2925= _new_region(); struct _RegionHandle* prgn=& _temp2925; _push_region(
prgn);{ struct Cyc_Absyn_Stmt* _temp2928; struct Cyc_List_List* _temp2930;
struct Cyc_Toc_Env* _temp2932; struct _tuple13 _temp2926= Cyc_Toc_xlate_pat( nv,
prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2933: _temp2932= _temp2926.f1; goto _LL2931; _LL2931: _temp2930=
_temp2926.f2; goto _LL2929; _LL2929: _temp2928= _temp2926.f3; goto _LL2927;
_LL2927: Cyc_Toc_stmt_to_c( _temp2932, s);{ struct Cyc_Absyn_Stmt* _temp2934=
Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL2935; _LL2935: if( exhaust){ s=
Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2928, _temp2934, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2936= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); goto _LL2937; _LL2937: { struct Cyc_Absyn_Stmt*
_temp2938= Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2936, 0), 0);
goto _LL2939; _LL2939: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),(
struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2928, Cyc_Absyn_seq_stmt(
_temp2938, _temp2934, 0), 0), 0);}} for( 0; _temp2930 != 0; _temp2930=(( struct
Cyc_List_List*) _check_null( _temp2930))->tl){ struct _tuple14 _temp2942; void*
_temp2943; struct _tuple0* _temp2945; struct _tuple14* _temp2940=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( _temp2930))->hd; _temp2942=*
_temp2940; _LL2946: _temp2945= _temp2942.f1; goto _LL2944; _LL2944: _temp2943=
_temp2942.f2; goto _LL2941; _LL2941: s= Cyc_Absyn_declare_stmt( _temp2945,
_temp2943, 0, s, 0);}}}; _pop_region(& _temp2925);} return s;}} static struct
Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( ds))->hd; void* _temp2947=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2973; struct Cyc_Absyn_Fndecl* _temp2975; int _temp2977; struct Cyc_Absyn_Exp*
_temp2979; struct Cyc_Core_Opt* _temp2981; struct Cyc_Core_Opt* _temp2983;
struct Cyc_Absyn_Pat* _temp2985; struct Cyc_List_List* _temp2987; struct Cyc_Absyn_Structdecl*
_temp2989; struct Cyc_Absyn_Uniondecl* _temp2991; struct Cyc_Absyn_Tuniondecl*
_temp2993; struct Cyc_Absyn_Enumdecl* _temp2995; struct Cyc_Absyn_Typedefdecl*
_temp2997; struct Cyc_List_List* _temp2999; struct _tagged_string* _temp3001;
struct Cyc_List_List* _temp3003; struct _tuple0* _temp3005; struct Cyc_List_List*
_temp3007; _LL2949: if(*(( int*) _temp2947) == Cyc_Absyn_Var_d){ _LL2974:
_temp2973=(( struct Cyc_Absyn_Var_d_struct*) _temp2947)->f1; goto _LL2950;}
else{ goto _LL2951;} _LL2951: if(*(( int*) _temp2947) == Cyc_Absyn_Fn_d){
_LL2976: _temp2975=(( struct Cyc_Absyn_Fn_d_struct*) _temp2947)->f1; goto
_LL2952;} else{ goto _LL2953;} _LL2953: if(*(( int*) _temp2947) == Cyc_Absyn_Let_d){
_LL2986: _temp2985=(( struct Cyc_Absyn_Let_d_struct*) _temp2947)->f1; goto
_LL2984; _LL2984: _temp2983=(( struct Cyc_Absyn_Let_d_struct*) _temp2947)->f2;
goto _LL2982; _LL2982: _temp2981=(( struct Cyc_Absyn_Let_d_struct*) _temp2947)->f3;
goto _LL2980; _LL2980: _temp2979=(( struct Cyc_Absyn_Let_d_struct*) _temp2947)->f4;
goto _LL2978; _LL2978: _temp2977=(( struct Cyc_Absyn_Let_d_struct*) _temp2947)->f5;
goto _LL2954;} else{ goto _LL2955;} _LL2955: if(*(( int*) _temp2947) == Cyc_Absyn_Letv_d){
_LL2988: _temp2987=(( struct Cyc_Absyn_Letv_d_struct*) _temp2947)->f1; goto
_LL2956;} else{ goto _LL2957;} _LL2957: if(*(( int*) _temp2947) == Cyc_Absyn_Struct_d){
_LL2990: _temp2989=(( struct Cyc_Absyn_Struct_d_struct*) _temp2947)->f1; goto
_LL2958;} else{ goto _LL2959;} _LL2959: if(*(( int*) _temp2947) == Cyc_Absyn_Union_d){
_LL2992: _temp2991=(( struct Cyc_Absyn_Union_d_struct*) _temp2947)->f1; goto
_LL2960;} else{ goto _LL2961;} _LL2961: if(*(( int*) _temp2947) == Cyc_Absyn_Tunion_d){
_LL2994: _temp2993=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2947)->f1; goto
_LL2962;} else{ goto _LL2963;} _LL2963: if(*(( int*) _temp2947) == Cyc_Absyn_Enum_d){
_LL2996: _temp2995=(( struct Cyc_Absyn_Enum_d_struct*) _temp2947)->f1; goto
_LL2964;} else{ goto _LL2965;} _LL2965: if(*(( int*) _temp2947) == Cyc_Absyn_Typedef_d){
_LL2998: _temp2997=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2947)->f1; goto
_LL2966;} else{ goto _LL2967;} _LL2967: if(*(( int*) _temp2947) == Cyc_Absyn_Namespace_d){
_LL3002: _temp3001=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2947)->f1; goto
_LL3000; _LL3000: _temp2999=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2947)->f2;
goto _LL2968;} else{ goto _LL2969;} _LL2969: if(*(( int*) _temp2947) == Cyc_Absyn_Using_d){
_LL3006: _temp3005=(( struct Cyc_Absyn_Using_d_struct*) _temp2947)->f1; goto
_LL3004; _LL3004: _temp3003=(( struct Cyc_Absyn_Using_d_struct*) _temp2947)->f2;
goto _LL2970;} else{ goto _LL2971;} _LL2971: if(*(( int*) _temp2947) == Cyc_Absyn_ExternC_d){
_LL3008: _temp3007=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2947)->f1; goto
_LL2972;} else{ goto _LL2948;} _LL2950: { struct _tuple0* _temp3009= _temp2973->name;
goto _LL3010; _LL3010: if(( void*) _temp2973->sc ==( void*) Cyc_Absyn_ExternC){
_temp3009=({ struct _tuple0* _temp3011=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp3011->f1= Cyc_Toc_rel_ns; _temp3011->f2=(* _temp3009).f2;
_temp3011;});} if( _temp2973->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*) _check_null( _temp2973->initializer));} if( ! nv->toplevel){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp3012=(
unsigned char*)"decls_to_c: not at toplevel!"; struct _tagged_string _temp3013;
_temp3013.curr= _temp3012; _temp3013.base= _temp3012; _temp3013.last_plus_one=
_temp3012 + 29; _temp3013;}));} nv= Cyc_Toc_add_varmap( nv, _temp2973->name, Cyc_Absyn_varb_exp(
_temp3009,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp3014=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp3014[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp3015; _temp3015.tag= Cyc_Absyn_Global_b;
_temp3015.f1= _temp2973; _temp3015;}); _temp3014;}), 0)); _temp2973->name=
_temp3009;( void*)( _temp2973->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2973->sc));(
void*)( _temp2973->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2973->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3016=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3016->hd=( void*) d; _temp3016->tl=
Cyc_Toc_result_decls; _temp3016;}); goto _LL2948;} _LL2952: { struct _tuple0*
_temp3017= _temp2975->name; goto _LL3018; _LL3018: if(( void*) _temp2975->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3017=({ struct _tuple0* _temp3019=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3019->f1= Cyc_Toc_rel_ns;
_temp3019->f2=(* _temp3017).f2; _temp3019;});} nv= Cyc_Toc_add_varmap( nv,
_temp2975->name, Cyc_Absyn_var_exp( _temp3017, 0)); _temp2975->name= _temp3017;
Cyc_Toc_fndecl_to_c( nv, _temp2975); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3020=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3020->hd=( void*) d; _temp3020->tl= Cyc_Toc_result_decls; _temp3020;});
goto _LL2948;} _LL2954:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp3021=( unsigned char*)"letdecl"; struct _tagged_string
_temp3022; _temp3022.curr= _temp3021; _temp3022.base= _temp3021; _temp3022.last_plus_one=
_temp3021 + 8; _temp3022;})); goto _LL2948; _LL2956:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp3023=(
unsigned char*)"letdecl"; struct _tagged_string _temp3024; _temp3024.curr=
_temp3023; _temp3024.base= _temp3023; _temp3024.last_plus_one= _temp3023 + 8;
_temp3024;})); goto _LL2948; _LL2958: Cyc_Toc_structdecl_to_c( _temp2989); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3025=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3025->hd=( void*) d; _temp3025->tl= Cyc_Toc_result_decls;
_temp3025;}); goto _LL2948; _LL2960: Cyc_Toc_uniondecl_to_c( _temp2991); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3026=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3026->hd=( void*) d; _temp3026->tl= Cyc_Toc_result_decls;
_temp3026;}); goto _LL2948; _LL2962: if( _temp2993->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2993);} else{ Cyc_Toc_tuniondecl_to_c( _temp2993);} goto _LL2948; _LL2964:
Cyc_Toc_enumdecl_to_c( nv, _temp2995); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3027=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3027->hd=( void*) d; _temp3027->tl= Cyc_Toc_result_decls; _temp3027;});
goto _LL2948; _LL2966: _temp2997->name= _temp2997->name; _temp2997->tvs= 0;(
void*)( _temp2997->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2997->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3028=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3028->hd=( void*) d; _temp3028->tl=
Cyc_Toc_result_decls; _temp3028;}); goto _LL2948; _LL2968: _temp3003= _temp2999;
goto _LL2970; _LL2970: _temp3007= _temp3003; goto _LL2972; _LL2972: nv= Cyc_Toc_decls_to_c(
nv, _temp3007, top); goto _LL2948; _LL2948:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}