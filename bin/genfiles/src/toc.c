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
goto _LL166;} else{ goto _LL167;} _LL167: goto _LL168; _LL166: return(( int(*)(
int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp175); _LL168:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp181=(
unsigned char*)"is_nullable"; struct _tagged_string _temp182; _temp182.curr=
_temp181; _temp182.base= _temp181; _temp182.last_plus_one= _temp181 + 12;
_temp182;})); return 0; _LL164:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag(
struct _tuple0* x, struct _tagged_string tag){ return({ struct _tuple0* _temp183=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp183->f1=(* x).f1;
_temp183->f2=({ struct _tagged_string* _temp184=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp184[ 0]=( struct
_tagged_string) Cyc_String_strconcat(*(* x).f2, tag); _temp184;}); _temp183;});}
static void Cyc_Toc_add_tuple_decl( struct _tagged_string* x, struct Cyc_List_List*
ts){ struct Cyc_List_List* _temp185= 0; goto _LL186; _LL186:{ int i= 1; for( 0;
ts != 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, i ++)){ _temp185=({
struct Cyc_List_List* _temp187=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp187->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp188=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp188->name=({ struct _tagged_string* _temp189=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp189[ 0]=( struct
_tagged_string) xprintf("f%d", i); _temp189;}); _temp188->tq= Cyc_Toc_mt_tq;
_temp188->type=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp188->width= 0; _temp188->attributes= 0; _temp188;}); _temp187->tl= _temp185;
_temp187;});}} _temp185=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp185);{ struct Cyc_Absyn_Structdecl* _temp194=({ struct
Cyc_Absyn_Structdecl* _temp190=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp190->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp190->name=({ struct Cyc_Core_Opt* _temp192=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp192->v=( void*)({ struct _tuple0*
_temp193=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp193->f1=
Cyc_Toc_abs_ns; _temp193->f2= x; _temp193;}); _temp192;}); _temp190->tvs= 0;
_temp190->fields=({ struct Cyc_Core_Opt* _temp191=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp191->v=( void*) _temp185;
_temp191;}); _temp190->attributes= 0; _temp190;}); goto _LL195; _LL195: Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp196=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp196->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp197=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp197[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp198; _temp198.tag= Cyc_Absyn_Struct_d; _temp198.f1=
_temp194; _temp198;}); _temp197;}), 0); _temp196->tl= Cyc_Toc_result_decls;
_temp196;});}} struct _tagged_ptr2{ struct Cyc_Absyn_Structfield** curr; struct
Cyc_Absyn_Structfield** base; struct Cyc_Absyn_Structfield** last_plus_one; } ;
static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x, void* t){
void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Structfield* _temp200=({ struct Cyc_Absyn_Structfield* _temp199=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp199->name= Cyc_Toc_curr_sp; _temp199->tq= Cyc_Toc_mt_tq; _temp199->type=(
void*) tptr; _temp199->width= 0; _temp199->attributes= 0; _temp199;}); goto
_LL201; _LL201: { struct Cyc_Absyn_Structfield* _temp203=({ struct Cyc_Absyn_Structfield*
_temp202=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp202->name= Cyc_Toc_base_sp; _temp202->tq= Cyc_Toc_mt_tq; _temp202->type=(
void*) tptr; _temp202->width= 0; _temp202->attributes= 0; _temp202;}); goto
_LL204; _LL204: { struct Cyc_Absyn_Structfield* _temp206=({ struct Cyc_Absyn_Structfield*
_temp205=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp205->name= Cyc_Toc_last_plus_one_sp; _temp205->tq= Cyc_Toc_mt_tq; _temp205->type=(
void*) tptr; _temp205->width= 0; _temp205->attributes= 0; _temp205;}); goto
_LL207; _LL207: { struct Cyc_List_List* _temp214=({ struct Cyc_List_List*(*
_temp208)( struct _tagged_ptr2)=( struct Cyc_List_List*(*)( struct _tagged_ptr2))
Cyc_List_list; struct Cyc_Absyn_Structfield* _temp211= _temp200; struct Cyc_Absyn_Structfield*
_temp212= _temp203; struct Cyc_Absyn_Structfield* _temp213= _temp206; struct Cyc_Absyn_Structfield*
_temp209[ 3u]={ _temp211, _temp212, _temp213}; struct _tagged_ptr2 _temp210={
_temp209, _temp209, _temp209 + 3u}; _temp208( _temp210);}); goto _LL215; _LL215: {
struct Cyc_Absyn_Structdecl* _temp220=({ struct Cyc_Absyn_Structdecl* _temp216=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp216->sc=( void*)(( void*) Cyc_Absyn_Public); _temp216->name=({ struct Cyc_Core_Opt*
_temp218=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp218->v=( void*)({ struct _tuple0* _temp219=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp219->f1= Cyc_Toc_abs_ns; _temp219->f2= x;
_temp219;}); _temp218;}); _temp216->tvs= 0; _temp216->fields=({ struct Cyc_Core_Opt*
_temp217=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp217->v=( void*)(( struct Cyc_List_List*) _temp214); _temp217;}); _temp216->attributes=
0; _temp216;}); goto _LL221; _LL221: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp222=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp222->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp223=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp223[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp224; _temp224.tag= Cyc_Absyn_Struct_d;
_temp224.f1= _temp220; _temp224;}); _temp223;}), 0); _temp222->tl= Cyc_Toc_result_decls;
_temp222;});}}}}} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static
int Cyc_Toc_tuple_type_counter= 0; struct _tuple5{ struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static struct _tagged_string* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp225= Cyc_Toc_tuple_types;
goto _LL226; _LL226: for( 0; _temp225 != 0; _temp225=(( struct Cyc_List_List*)
_check_null( _temp225))->tl){ struct _tuple5 _temp229; struct Cyc_List_List*
_temp230; struct _tagged_string* _temp232; struct _tuple5* _temp227=( struct
_tuple5*)(( struct Cyc_List_List*) _check_null( _temp225))->hd; _temp229=*
_temp227; _LL233: _temp232= _temp229.f1; goto _LL231; _LL231: _temp230= _temp229.f2;
goto _LL228; _LL228: { int okay= 1; struct Cyc_List_List* _temp234= tqs0; goto
_LL235; _LL235: for( 0; _temp234 != 0? _temp230 != 0: 0;( _temp234=(( struct Cyc_List_List*)
_check_null( _temp234))->tl, _temp230=(( struct Cyc_List_List*) _check_null(
_temp230))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp234))->hd)).f2,( void*)(( struct Cyc_List_List*) _check_null(
_temp230))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp234 != 0)? 1: _temp230
!= 0){ continue;} return _temp232;}}}{ struct _tagged_string* x=({ struct
_tagged_string* _temp238=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp238[ 0]=( struct _tagged_string) xprintf("_tuple%d",
Cyc_Toc_tuple_type_counter ++); _temp238;}); struct Cyc_List_List* ts=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_snd_tqt, tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({
struct Cyc_List_List* _temp236=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp236->hd=( void*)({ struct _tuple5* _temp237=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp237->f1= x; _temp237->f2=
ts; _temp237;}); _temp236->tl= Cyc_Toc_tuple_types; _temp236;}); return x;}}
static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter=
0; static void* Cyc_Toc_tagged_string_type_v=( void*) 0u; static void* Cyc_Toc_tagged_string_type(){
if( Cyc_Toc_tagged_string_type_v ==( void*) Cyc_Absyn_VoidType){ Cyc_Toc_tagged_string_type_v=
Cyc_Absyn_strct( Cyc_Toc__tagged_string_sp);} return Cyc_Toc_tagged_string_type_v;}
struct _tuple6{ struct _tagged_string* f1; void* f2; } ; static void* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual tq){{ void* _temp239= t; void* _temp245; void*
_temp247; _LL241: if(( unsigned int) _temp239 > 4u?*(( int*) _temp239) == Cyc_Absyn_IntType:
0){ _LL248: _temp247=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp239)->f1;
if( _temp247 ==( void*) Cyc_Absyn_Unsigned){ goto _LL246;} else{ goto _LL243;}
_LL246: _temp245=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp239)->f2; if(
_temp245 ==( void*) Cyc_Absyn_B1){ goto _LL242;} else{ goto _LL243;}} else{ goto
_LL243;} _LL243: goto _LL244; _LL242: return Cyc_Toc_tagged_string_type();
_LL244: goto _LL240; _LL240:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=(( struct Cyc_List_List*) _check_null( tts))->tl){ struct
_tuple6 _temp251; void* _temp252; struct _tagged_string* _temp254; struct
_tuple6* _temp249=( struct _tuple6*)(( struct Cyc_List_List*) _check_null( tts))->hd;
_temp251=* _temp249; _LL255: _temp254= _temp251.f1; goto _LL253; _LL253:
_temp252= _temp251.f2; goto _LL250; _LL250: if( ! Cyc_Tcutil_unify( t, _temp252)){
continue;} return Cyc_Absyn_strct( _temp254);}}{ struct _tagged_string* x=({
struct _tagged_string* _temp258=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp258[ 0]=( struct _tagged_string) xprintf("_tagged_ptr%d",
Cyc_Toc_tagged_ptr_counter ++); _temp258;}); Cyc_Toc_add_tagged_ptr_decl( x, t);
Cyc_Toc_tagged_ptr_types=({ struct Cyc_List_List* _temp256=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp256->hd=( void*)({ struct
_tuple6* _temp257=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp257->f1= x; _temp257->f2= t; _temp257;}); _temp256->tl= Cyc_Toc_tagged_ptr_types;
_temp256;}); return Cyc_Absyn_strct( x);}} static int Cyc_Toc_temp_var_counter=
0; struct _tuple0* Cyc_Toc_temp_var(){ return({ struct _tuple0* _temp259=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp259->f1=( void*) Cyc_Absyn_Loc_n;
_temp259->f2=({ struct _tagged_string* _temp260=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp260[ 0]=( struct
_tagged_string) xprintf("_temp%d", Cyc_Toc_temp_var_counter ++); _temp260;});
_temp259;});} static int Cyc_Toc_fresh_label_counter= 0; static struct
_tagged_string* Cyc_Toc_fresh_label(){ return({ struct _tagged_string* _temp261=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp261[
0]=( struct _tagged_string) xprintf("_LL%d", Cyc_Toc_temp_var_counter ++);
_temp261;});} static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple1*
Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp264; struct Cyc_Absyn_Tqual
_temp266; struct Cyc_Core_Opt* _temp268; struct _tuple1 _temp262=* a; _LL269:
_temp268= _temp262.f1; goto _LL267; _LL267: _temp266= _temp262.f2; goto _LL265;
_LL265: _temp264= _temp262.f3; goto _LL263; _LL263: return({ struct _tuple1*
_temp270=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp270->f1=
_temp268; _temp270->f2= _temp266; _temp270->f3= Cyc_Toc_typ_to_c( _temp264);
_temp270;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp273; struct Cyc_Absyn_Tqual _temp275; struct _tuple3 _temp271=* x;
_LL276: _temp275= _temp271.f1; goto _LL274; _LL274: _temp273= _temp271.f2; goto
_LL272; _LL272: return({ struct _tuple3* _temp277=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp277->f1= _temp275; _temp277->f2= Cyc_Toc_typ_to_c(
_temp273); _temp277;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp278= t; struct Cyc_Absyn_Exp* _temp286; struct Cyc_Absyn_Tqual _temp288;
void* _temp290; struct Cyc_Core_Opt* _temp292; int _temp294; struct Cyc_Core_Opt*
_temp296; struct Cyc_Core_Opt _temp298; void* _temp299; struct Cyc_Core_Opt*
_temp301; _LL280: if(( unsigned int) _temp278 > 4u?*(( int*) _temp278) == Cyc_Absyn_ArrayType:
0){ _LL291: _temp290=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp278)->f1;
goto _LL289; _LL289: _temp288=(( struct Cyc_Absyn_ArrayType_struct*) _temp278)->f2;
goto _LL287; _LL287: _temp286=(( struct Cyc_Absyn_ArrayType_struct*) _temp278)->f3;
goto _LL281;} else{ goto _LL282;} _LL282: if(( unsigned int) _temp278 > 4u?*((
int*) _temp278) == Cyc_Absyn_Evar: 0){ _LL302: _temp301=(( struct Cyc_Absyn_Evar_struct*)
_temp278)->f1; goto _LL297; _LL297: _temp296=(( struct Cyc_Absyn_Evar_struct*)
_temp278)->f2; if( _temp296 == 0){ goto _LL284;} else{ _temp298=* _temp296;
_LL300: _temp299=( void*) _temp298.v; goto _LL295;} _LL295: _temp294=(( struct
Cyc_Absyn_Evar_struct*) _temp278)->f3; goto _LL293; _LL293: _temp292=(( struct
Cyc_Absyn_Evar_struct*) _temp278)->f4; goto _LL283;} else{ goto _LL284;} _LL284:
goto _LL285; _LL281: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp303=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp303[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp304; _temp304.tag= Cyc_Absyn_ArrayType;
_temp304.f1=( void*) Cyc_Toc_typ_to_c_array( _temp290); _temp304.f2= _temp288;
_temp304.f3= _temp286; _temp304;}); _temp303;}); _LL283: return Cyc_Toc_typ_to_c_array(
_temp299); _LL285: return Cyc_Toc_typ_to_c( t); _LL279:;} static void* Cyc_Toc_typ_to_c(
void* t); static struct Cyc_Absyn_Structfield* Cyc_Toc_structfield_to_c( struct
Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield* _temp305=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp305->name= f->name; _temp305->tq= f->tq; _temp305->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp305->width= f->width; _temp305->attributes= f->attributes;
_temp305;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp306= t; struct
Cyc_Core_Opt* _temp358; int _temp360; struct Cyc_Core_Opt* _temp362; struct Cyc_Core_Opt*
_temp364; struct Cyc_Core_Opt* _temp366; int _temp368; struct Cyc_Core_Opt*
_temp370; struct Cyc_Core_Opt _temp372; void* _temp373; struct Cyc_Core_Opt*
_temp375; struct Cyc_Absyn_Tvar* _temp377; struct Cyc_Absyn_TunionInfo _temp379;
void* _temp381; struct Cyc_List_List* _temp383; void* _temp385; struct Cyc_Absyn_Tuniondecl*
_temp387; struct Cyc_Absyn_TunionInfo _temp389; struct Cyc_Absyn_TunionFieldInfo
_temp391; struct Cyc_List_List* _temp393; void* _temp395; struct Cyc_Absyn_Tunionfield*
_temp397; struct Cyc_Absyn_Tuniondecl* _temp399; struct Cyc_Absyn_TunionFieldInfo
_temp401; struct Cyc_Absyn_PtrInfo _temp403; struct Cyc_Absyn_Conref* _temp405;
struct Cyc_Absyn_Tqual _temp407; struct Cyc_Absyn_Conref* _temp409; void*
_temp411; void* _temp413; void* _temp415; void* _temp417; struct Cyc_Absyn_Exp*
_temp419; struct Cyc_Absyn_Tqual _temp421; void* _temp423; struct Cyc_Absyn_FnInfo
_temp425; struct Cyc_List_List* _temp427; struct Cyc_List_List* _temp429; struct
Cyc_Absyn_VarargInfo* _temp431; int _temp433; struct Cyc_List_List* _temp435;
void* _temp437; struct Cyc_Core_Opt* _temp439; struct Cyc_List_List* _temp441;
struct Cyc_List_List* _temp443; struct Cyc_List_List* _temp445; struct Cyc_List_List*
_temp447; struct Cyc_Absyn_Uniondecl** _temp449; struct Cyc_List_List* _temp451;
struct _tuple0* _temp453; struct Cyc_Absyn_Structdecl** _temp455; struct Cyc_List_List*
_temp457; struct _tuple0* _temp459; struct Cyc_Absyn_Enumdecl* _temp461; struct
_tuple0* _temp463; struct Cyc_Core_Opt* _temp465; struct Cyc_List_List* _temp467;
struct _tuple0* _temp469; void* _temp471; void* _temp473; struct Cyc_List_List*
_temp475; _LL308: if( _temp306 ==( void*) Cyc_Absyn_VoidType){ goto _LL309;}
else{ goto _LL310;} _LL310: if(( unsigned int) _temp306 > 4u?*(( int*) _temp306)
== Cyc_Absyn_Evar: 0){ _LL365: _temp364=(( struct Cyc_Absyn_Evar_struct*)
_temp306)->f1; goto _LL363; _LL363: _temp362=(( struct Cyc_Absyn_Evar_struct*)
_temp306)->f2; if( _temp362 == 0){ goto _LL361;} else{ goto _LL312;} _LL361:
_temp360=(( struct Cyc_Absyn_Evar_struct*) _temp306)->f3; goto _LL359; _LL359:
_temp358=(( struct Cyc_Absyn_Evar_struct*) _temp306)->f4; goto _LL311;} else{
goto _LL312;} _LL312: if(( unsigned int) _temp306 > 4u?*(( int*) _temp306) ==
Cyc_Absyn_Evar: 0){ _LL376: _temp375=(( struct Cyc_Absyn_Evar_struct*) _temp306)->f1;
goto _LL371; _LL371: _temp370=(( struct Cyc_Absyn_Evar_struct*) _temp306)->f2;
if( _temp370 == 0){ goto _LL314;} else{ _temp372=* _temp370; _LL374: _temp373=(
void*) _temp372.v; goto _LL369;} _LL369: _temp368=(( struct Cyc_Absyn_Evar_struct*)
_temp306)->f3; goto _LL367; _LL367: _temp366=(( struct Cyc_Absyn_Evar_struct*)
_temp306)->f4; goto _LL313;} else{ goto _LL314;} _LL314: if(( unsigned int)
_temp306 > 4u?*(( int*) _temp306) == Cyc_Absyn_VarType: 0){ _LL378: _temp377=((
struct Cyc_Absyn_VarType_struct*) _temp306)->f1; goto _LL315;} else{ goto _LL316;}
_LL316: if(( unsigned int) _temp306 > 4u?*(( int*) _temp306) == Cyc_Absyn_TunionType:
0){ _LL380: _temp379=(( struct Cyc_Absyn_TunionType_struct*) _temp306)->f1;
_LL386: _temp385=( void*) _temp379.tunion_info; if(*(( int*) _temp385) == Cyc_Absyn_KnownTunion){
_LL388: _temp387=(( struct Cyc_Absyn_KnownTunion_struct*) _temp385)->f1; goto
_LL384;} else{ goto _LL318;} _LL384: _temp383= _temp379.targs; goto _LL382;
_LL382: _temp381=( void*) _temp379.rgn; goto _LL317;} else{ goto _LL318;} _LL318:
if(( unsigned int) _temp306 > 4u?*(( int*) _temp306) == Cyc_Absyn_TunionType: 0){
_LL390: _temp389=(( struct Cyc_Absyn_TunionType_struct*) _temp306)->f1; goto
_LL319;} else{ goto _LL320;} _LL320: if(( unsigned int) _temp306 > 4u?*(( int*)
_temp306) == Cyc_Absyn_TunionFieldType: 0){ _LL392: _temp391=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp306)->f1; _LL396: _temp395=( void*) _temp391.field_info; if(*(( int*)
_temp395) == Cyc_Absyn_KnownTunionfield){ _LL400: _temp399=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp395)->f1; goto _LL398; _LL398: _temp397=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp395)->f2; goto _LL394;} else{ goto _LL322;} _LL394: _temp393= _temp391.targs;
goto _LL321;} else{ goto _LL322;} _LL322: if(( unsigned int) _temp306 > 4u?*((
int*) _temp306) == Cyc_Absyn_TunionFieldType: 0){ _LL402: _temp401=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp306)->f1; goto _LL323;} else{ goto _LL324;} _LL324: if(( unsigned int)
_temp306 > 4u?*(( int*) _temp306) == Cyc_Absyn_PointerType: 0){ _LL404: _temp403=((
struct Cyc_Absyn_PointerType_struct*) _temp306)->f1; _LL414: _temp413=( void*)
_temp403.elt_typ; goto _LL412; _LL412: _temp411=( void*) _temp403.rgn_typ; goto
_LL410; _LL410: _temp409= _temp403.nullable; goto _LL408; _LL408: _temp407=
_temp403.tq; goto _LL406; _LL406: _temp405= _temp403.bounds; goto _LL325;} else{
goto _LL326;} _LL326: if(( unsigned int) _temp306 > 4u?*(( int*) _temp306) ==
Cyc_Absyn_IntType: 0){ _LL418: _temp417=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp306)->f1; goto _LL416; _LL416: _temp415=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp306)->f2; goto _LL327;} else{ goto _LL328;} _LL328: if( _temp306 ==( void*)
Cyc_Absyn_FloatType){ goto _LL329;} else{ goto _LL330;} _LL330: if( _temp306 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL331;} else{ goto _LL332;} _LL332: if((
unsigned int) _temp306 > 4u?*(( int*) _temp306) == Cyc_Absyn_ArrayType: 0){
_LL424: _temp423=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp306)->f1;
goto _LL422; _LL422: _temp421=(( struct Cyc_Absyn_ArrayType_struct*) _temp306)->f2;
goto _LL420; _LL420: _temp419=(( struct Cyc_Absyn_ArrayType_struct*) _temp306)->f3;
goto _LL333;} else{ goto _LL334;} _LL334: if(( unsigned int) _temp306 > 4u?*((
int*) _temp306) == Cyc_Absyn_FnType: 0){ _LL426: _temp425=(( struct Cyc_Absyn_FnType_struct*)
_temp306)->f1; _LL442: _temp441= _temp425.tvars; goto _LL440; _LL440: _temp439=
_temp425.effect; goto _LL438; _LL438: _temp437=( void*) _temp425.ret_typ; goto
_LL436; _LL436: _temp435= _temp425.args; goto _LL434; _LL434: _temp433= _temp425.c_varargs;
goto _LL432; _LL432: _temp431= _temp425.cyc_varargs; goto _LL430; _LL430:
_temp429= _temp425.rgn_po; goto _LL428; _LL428: _temp427= _temp425.attributes;
goto _LL335;} else{ goto _LL336;} _LL336: if(( unsigned int) _temp306 > 4u?*((
int*) _temp306) == Cyc_Absyn_TupleType: 0){ _LL444: _temp443=(( struct Cyc_Absyn_TupleType_struct*)
_temp306)->f1; goto _LL337;} else{ goto _LL338;} _LL338: if(( unsigned int)
_temp306 > 4u?*(( int*) _temp306) == Cyc_Absyn_AnonStructType: 0){ _LL446:
_temp445=(( struct Cyc_Absyn_AnonStructType_struct*) _temp306)->f1; goto _LL339;}
else{ goto _LL340;} _LL340: if(( unsigned int) _temp306 > 4u?*(( int*) _temp306)
== Cyc_Absyn_AnonUnionType: 0){ _LL448: _temp447=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp306)->f1; goto _LL341;} else{ goto _LL342;} _LL342: if(( unsigned int)
_temp306 > 4u?*(( int*) _temp306) == Cyc_Absyn_UnionType: 0){ _LL454: _temp453=((
struct Cyc_Absyn_UnionType_struct*) _temp306)->f1; goto _LL452; _LL452: _temp451=((
struct Cyc_Absyn_UnionType_struct*) _temp306)->f2; goto _LL450; _LL450: _temp449=((
struct Cyc_Absyn_UnionType_struct*) _temp306)->f3; goto _LL343;} else{ goto
_LL344;} _LL344: if(( unsigned int) _temp306 > 4u?*(( int*) _temp306) == Cyc_Absyn_StructType:
0){ _LL460: _temp459=(( struct Cyc_Absyn_StructType_struct*) _temp306)->f1; goto
_LL458; _LL458: _temp457=(( struct Cyc_Absyn_StructType_struct*) _temp306)->f2;
goto _LL456; _LL456: _temp455=(( struct Cyc_Absyn_StructType_struct*) _temp306)->f3;
goto _LL345;} else{ goto _LL346;} _LL346: if(( unsigned int) _temp306 > 4u?*((
int*) _temp306) == Cyc_Absyn_EnumType: 0){ _LL464: _temp463=(( struct Cyc_Absyn_EnumType_struct*)
_temp306)->f1; goto _LL462; _LL462: _temp461=(( struct Cyc_Absyn_EnumType_struct*)
_temp306)->f2; goto _LL347;} else{ goto _LL348;} _LL348: if(( unsigned int)
_temp306 > 4u?*(( int*) _temp306) == Cyc_Absyn_TypedefType: 0){ _LL470: _temp469=((
struct Cyc_Absyn_TypedefType_struct*) _temp306)->f1; goto _LL468; _LL468:
_temp467=(( struct Cyc_Absyn_TypedefType_struct*) _temp306)->f2; goto _LL466;
_LL466: _temp465=(( struct Cyc_Absyn_TypedefType_struct*) _temp306)->f3; goto
_LL349;} else{ goto _LL350;} _LL350: if(( unsigned int) _temp306 > 4u?*(( int*)
_temp306) == Cyc_Absyn_RgnHandleType: 0){ _LL472: _temp471=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp306)->f1; goto _LL351;} else{ goto _LL352;} _LL352: if( _temp306 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL353;} else{ goto _LL354;} _LL354: if(( unsigned int)
_temp306 > 4u?*(( int*) _temp306) == Cyc_Absyn_AccessEff: 0){ _LL474: _temp473=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp306)->f1; goto _LL355;} else{
goto _LL356;} _LL356: if(( unsigned int) _temp306 > 4u?*(( int*) _temp306) ==
Cyc_Absyn_JoinEff: 0){ _LL476: _temp475=(( struct Cyc_Absyn_JoinEff_struct*)
_temp306)->f1; goto _LL357;} else{ goto _LL307;} _LL309: return t; _LL311:
return( void*) Cyc_Absyn_VoidType; _LL313: return Cyc_Toc_typ_to_c( _temp373);
_LL315: return Cyc_Absyn_void_star_typ(); _LL317: return Cyc_Absyn_void_star_typ();
_LL319:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp477=( unsigned char*)"unresolved TunionType"; struct
_tagged_string _temp478; _temp478.curr= _temp477; _temp478.base= _temp477;
_temp478.last_plus_one= _temp477 + 22; _temp478;}))); _LL321: if( _temp397->typs
== 0){ if( _temp399->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp397->name,({
unsigned char* _temp479=( unsigned char*)"_struct"; struct _tagged_string
_temp480; _temp480.curr= _temp479; _temp480.base= _temp479; _temp480.last_plus_one=
_temp479 + 8; _temp480;})));} _LL323:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp481=( unsigned char*)"unresolved TunionFieldType";
struct _tagged_string _temp482; _temp482.curr= _temp481; _temp482.base= _temp481;
_temp482.last_plus_one= _temp481 + 27; _temp482;}))); _LL325: _temp413= Cyc_Toc_typ_to_c_array(
_temp413);{ void* _temp483=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp405))->v; void* _temp489; _LL485: if((
unsigned int) _temp483 > 1u?*(( int*) _temp483) == Cyc_Absyn_Eq_constr: 0){
_LL490: _temp489=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp483)->f1;
if( _temp489 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL486;} else{ goto _LL487;}}
else{ goto _LL487;} _LL487: goto _LL488; _LL486: return Cyc_Toc_add_tagged_ptr_type(
_temp413, _temp407); _LL488: return Cyc_Absyn_star_typ( _temp413,( void*) Cyc_Absyn_HeapRgn,
_temp407); _LL484:;} _LL327: return t; _LL329: return t; _LL331: return t;
_LL333: return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp491=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp491[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp492; _temp492.tag= Cyc_Absyn_ArrayType;
_temp492.f1=( void*) Cyc_Toc_typ_to_c_array( _temp423); _temp492.f2= _temp421;
_temp492.f3= _temp419; _temp492;}); _temp491;}); _LL335: { struct Cyc_List_List*
_temp493= _temp427; goto _LL494; _LL494: { struct Cyc_List_List* _temp495= 0;
goto _LL496; _LL496: for( 0; _temp427 != 0; _temp427=(( struct Cyc_List_List*)
_check_null( _temp427))->tl){ void* _temp497=( void*)(( struct Cyc_List_List*)
_check_null( _temp427))->hd; _LL499: if( _temp497 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL500;} else{ goto _LL501;} _LL501: if( _temp497 ==( void*) Cyc_Absyn_Const_att){
goto _LL502;} else{ goto _LL503;} _LL503: goto _LL504; _LL500: goto _LL502;
_LL502: continue; _LL504: _temp495=({ struct Cyc_List_List* _temp505=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp505->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( _temp427))->hd); _temp505->tl=
_temp495; _temp505;}); goto _LL498; _LL498:;}{ struct Cyc_List_List* _temp506=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp435); goto _LL507; _LL507: if(
_temp431 != 0){ void* _temp508= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp431))->type,( void*)(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp431))->rgn, Cyc_Toc_mt_tq)); goto
_LL509; _LL509: { struct _tuple1* _temp511=({ struct _tuple1* _temp510=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp510->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp431))->name; _temp510->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp431))->tq; _temp510->f3= _temp508; _temp510;}); goto _LL512;
_LL512: _temp506=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_imp_append)( _temp506,({ struct Cyc_List_List*
_temp513=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp513->hd=( void*) _temp511; _temp513->tl= 0; _temp513;}));}} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp514=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp514[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp515; _temp515.tag= Cyc_Absyn_FnType; _temp515.f1=({ struct Cyc_Absyn_FnInfo
_temp516; _temp516.tvars= 0; _temp516.effect= 0; _temp516.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp437); _temp516.args= _temp506; _temp516.c_varargs= _temp433; _temp516.cyc_varargs=
0; _temp516.rgn_po= 0; _temp516.attributes= _temp495; _temp516;}); _temp515;});
_temp514;});}}} _LL337: _temp443=(( struct Cyc_List_List*(*)( struct _tuple3*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp443);{ struct _tagged_string* _temp517= Cyc_Toc_add_tuple_type( _temp443);
goto _LL518; _LL518: return Cyc_Absyn_strct( _temp517);} _LL339: return( void*)({
struct Cyc_Absyn_AnonStructType_struct* _temp519=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp519[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp520; _temp520.tag= Cyc_Absyn_AnonStructType;
_temp520.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp445); _temp520;}); _temp519;}); _LL341: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp521=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp521[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp522; _temp522.tag=
Cyc_Absyn_AnonUnionType; _temp522.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp447); _temp522;}); _temp521;}); _LL343: if( _temp453 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp523=(
unsigned char*)"anonymous union"; struct _tagged_string _temp524; _temp524.curr=
_temp523; _temp524.base= _temp523; _temp524.last_plus_one= _temp523 + 16;
_temp524;}));} return Cyc_Absyn_unionq_typ(( struct _tuple0*) _check_null(
_temp453)); _LL345: if( _temp459 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp525=( unsigned char*)"anonymous struct"; struct
_tagged_string _temp526; _temp526.curr= _temp525; _temp526.base= _temp525;
_temp526.last_plus_one= _temp525 + 17; _temp526;}));} return Cyc_Absyn_strctq((
struct _tuple0*) _check_null( _temp459)); _LL347: return t; _LL349: if( _temp465
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp527=( unsigned char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp528; _temp528.curr= _temp527; _temp528.base= _temp527;
_temp528.last_plus_one= _temp527 + 38; _temp528;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp529=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp529[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp530; _temp530.tag= Cyc_Absyn_TypedefType;
_temp530.f1= _temp469; _temp530.f2= 0; _temp530.f3=({ struct Cyc_Core_Opt*
_temp531=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp531->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp465))->v); _temp531;}); _temp530;}); _temp529;}); _LL351:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL353: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp532=( unsigned char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp533; _temp533.curr= _temp532; _temp533.base= _temp532;
_temp533.last_plus_one= _temp532 + 55; _temp533;})); _LL355: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp534=(
unsigned char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp535; _temp535.curr= _temp534; _temp535.base= _temp534;
_temp535.last_plus_one= _temp534 + 54; _temp535;})); _LL357: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp536=(
unsigned char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp537; _temp537.curr= _temp536; _temp537.base= _temp536;
_temp537.last_plus_one= _temp536 + 52; _temp537;})); _LL307:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp538= t; struct Cyc_Absyn_Exp* _temp544; struct Cyc_Absyn_Tqual
_temp546; void* _temp548; _LL540: if(( unsigned int) _temp538 > 4u?*(( int*)
_temp538) == Cyc_Absyn_ArrayType: 0){ _LL549: _temp548=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp538)->f1; goto _LL547; _LL547: _temp546=(( struct Cyc_Absyn_ArrayType_struct*)
_temp538)->f2; goto _LL545; _LL545: _temp544=(( struct Cyc_Absyn_ArrayType_struct*)
_temp538)->f3; goto _LL541;} else{ goto _LL542;} _LL542: goto _LL543; _LL541:
return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ( _temp548,( void*) Cyc_Absyn_HeapRgn,
_temp546), e, l); _LL543: return Cyc_Absyn_cast_exp( t, e, l); _LL539:;} static
int Cyc_Toc_atomic_typ( void* t){ void* _temp550= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp586; void* _temp588; void* _temp590; struct Cyc_Absyn_Exp*
_temp592; struct Cyc_Absyn_Tqual _temp594; void* _temp596; struct Cyc_Absyn_FnInfo
_temp598; struct Cyc_List_List* _temp600; struct Cyc_Absyn_Structdecl** _temp602;
struct Cyc_List_List* _temp604; struct _tuple0* _temp606; struct Cyc_Absyn_TunionInfo
_temp608; struct Cyc_List_List* _temp610; struct Cyc_List_List* _temp612; struct
Cyc_Absyn_TunionFieldInfo _temp614; struct Cyc_List_List* _temp616; void*
_temp618; struct Cyc_Absyn_Tunionfield* _temp620; struct Cyc_Absyn_Tuniondecl*
_temp622; struct Cyc_Absyn_Uniondecl** _temp624; struct Cyc_List_List* _temp626;
struct _tuple0* _temp628; struct Cyc_Absyn_PtrInfo _temp630; void* _temp632;
_LL552: if( _temp550 ==( void*) Cyc_Absyn_VoidType){ goto _LL553;} else{ goto
_LL554;} _LL554: if(( unsigned int) _temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_VarType:
0){ _LL587: _temp586=(( struct Cyc_Absyn_VarType_struct*) _temp550)->f1; goto
_LL555;} else{ goto _LL556;} _LL556: if(( unsigned int) _temp550 > 4u?*(( int*)
_temp550) == Cyc_Absyn_IntType: 0){ _LL591: _temp590=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp550)->f1; goto _LL589; _LL589: _temp588=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp550)->f2; goto _LL557;} else{ goto _LL558;} _LL558: if( _temp550 ==( void*)
Cyc_Absyn_FloatType){ goto _LL559;} else{ goto _LL560;} _LL560: if( _temp550 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL561;} else{ goto _LL562;} _LL562: if((
unsigned int) _temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_ArrayType: 0){
_LL597: _temp596=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp550)->f1;
goto _LL595; _LL595: _temp594=(( struct Cyc_Absyn_ArrayType_struct*) _temp550)->f2;
goto _LL593; _LL593: _temp592=(( struct Cyc_Absyn_ArrayType_struct*) _temp550)->f3;
goto _LL563;} else{ goto _LL564;} _LL564: if(( unsigned int) _temp550 > 4u?*((
int*) _temp550) == Cyc_Absyn_FnType: 0){ _LL599: _temp598=(( struct Cyc_Absyn_FnType_struct*)
_temp550)->f1; goto _LL565;} else{ goto _LL566;} _LL566: if(( unsigned int)
_temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_TupleType: 0){ _LL601: _temp600=((
struct Cyc_Absyn_TupleType_struct*) _temp550)->f1; goto _LL567;} else{ goto
_LL568;} _LL568: if(( unsigned int) _temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_StructType:
0){ _LL607: _temp606=(( struct Cyc_Absyn_StructType_struct*) _temp550)->f1; goto
_LL605; _LL605: _temp604=(( struct Cyc_Absyn_StructType_struct*) _temp550)->f2;
goto _LL603; _LL603: _temp602=(( struct Cyc_Absyn_StructType_struct*) _temp550)->f3;
goto _LL569;} else{ goto _LL570;} _LL570: if(( unsigned int) _temp550 > 4u?*((
int*) _temp550) == Cyc_Absyn_TunionType: 0){ _LL609: _temp608=(( struct Cyc_Absyn_TunionType_struct*)
_temp550)->f1; goto _LL571;} else{ goto _LL572;} _LL572: if(( unsigned int)
_temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_AnonStructType: 0){ _LL611:
_temp610=(( struct Cyc_Absyn_AnonStructType_struct*) _temp550)->f1; goto _LL573;}
else{ goto _LL574;} _LL574: if(( unsigned int) _temp550 > 4u?*(( int*) _temp550)
== Cyc_Absyn_AnonUnionType: 0){ _LL613: _temp612=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp550)->f1; goto _LL575;} else{ goto _LL576;} _LL576: if(( unsigned int)
_temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_TunionFieldType: 0){ _LL615:
_temp614=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp550)->f1; _LL619:
_temp618=( void*) _temp614.field_info; if(*(( int*) _temp618) == Cyc_Absyn_KnownTunionfield){
_LL623: _temp622=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp618)->f1;
goto _LL621; _LL621: _temp620=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp618)->f2; goto _LL617;} else{ goto _LL578;} _LL617: _temp616= _temp614.targs;
goto _LL577;} else{ goto _LL578;} _LL578: if(( unsigned int) _temp550 > 4u?*((
int*) _temp550) == Cyc_Absyn_UnionType: 0){ _LL629: _temp628=(( struct Cyc_Absyn_UnionType_struct*)
_temp550)->f1; goto _LL627; _LL627: _temp626=(( struct Cyc_Absyn_UnionType_struct*)
_temp550)->f2; goto _LL625; _LL625: _temp624=(( struct Cyc_Absyn_UnionType_struct*)
_temp550)->f3; goto _LL579;} else{ goto _LL580;} _LL580: if(( unsigned int)
_temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_PointerType: 0){ _LL631: _temp630=((
struct Cyc_Absyn_PointerType_struct*) _temp550)->f1; goto _LL581;} else{ goto
_LL582;} _LL582: if(( unsigned int) _temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_RgnHandleType:
0){ _LL633: _temp632=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp550)->f1;
goto _LL583;} else{ goto _LL584;} _LL584: goto _LL585; _LL553: return 1; _LL555:
return 0; _LL557: return 1; _LL559: return 1; _LL561: return 0; _LL563: return
Cyc_Toc_atomic_typ( _temp596); _LL565: return 1; _LL567: for( 0; _temp600 != 0;
_temp600=(( struct Cyc_List_List*) _check_null( _temp600))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp600))->hd)).f2)){
return 0;}} return 1; _LL569: return 0; _LL571: return 0; _LL573: _temp612=
_temp610; goto _LL575; _LL575: for( 0; _temp612 != 0; _temp612=(( struct Cyc_List_List*)
_check_null( _temp612))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp612))->hd)->type)){ return 0;}} return
1; _LL577: return _temp620->typs == 0; _LL579: return 0; _LL581: return 0;
_LL583: return 0; _LL585:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp634= Cyc_Absynpp_typ2string(
t); xprintf("atomic_typ:  bad type %.*s", _temp634.last_plus_one - _temp634.curr,
_temp634.curr);})); return 0; _LL551:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp635= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp641;
struct Cyc_Absyn_Conref* _temp643; struct Cyc_Absyn_Tqual _temp645; struct Cyc_Absyn_Conref*
_temp647; void* _temp649; void* _temp651; _LL637: if(( unsigned int) _temp635 >
4u?*(( int*) _temp635) == Cyc_Absyn_PointerType: 0){ _LL642: _temp641=(( struct
Cyc_Absyn_PointerType_struct*) _temp635)->f1; _LL652: _temp651=( void*) _temp641.elt_typ;
goto _LL650; _LL650: _temp649=( void*) _temp641.rgn_typ; goto _LL648; _LL648:
_temp647= _temp641.nullable; goto _LL646; _LL646: _temp645= _temp641.tq; goto
_LL644; _LL644: _temp643= _temp641.bounds; goto _LL638;} else{ goto _LL639;}
_LL639: goto _LL640; _LL638: { void* _temp653= Cyc_Tcutil_compress( _temp651);
_LL655: if( _temp653 ==( void*) Cyc_Absyn_VoidType){ goto _LL656;} else{ goto
_LL657;} _LL657: goto _LL658; _LL656: return 1; _LL658: return 0; _LL654:;}
_LL640: return 0; _LL636:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_string* f){ void* _temp659= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp671; struct Cyc_Absyn_Structdecl* _temp673; struct Cyc_List_List* _temp674;
struct _tuple0* _temp676; struct Cyc_List_List* _temp678; struct Cyc_List_List*
_temp680; struct Cyc_Absyn_Uniondecl** _temp682; struct Cyc_List_List* _temp684;
struct _tuple0* _temp686; _LL661: if(( unsigned int) _temp659 > 4u?*(( int*)
_temp659) == Cyc_Absyn_StructType: 0){ _LL677: _temp676=(( struct Cyc_Absyn_StructType_struct*)
_temp659)->f1; goto _LL675; _LL675: _temp674=(( struct Cyc_Absyn_StructType_struct*)
_temp659)->f2; goto _LL672; _LL672: _temp671=(( struct Cyc_Absyn_StructType_struct*)
_temp659)->f3; if( _temp671 == 0){ goto _LL663;} else{ _temp673=* _temp671; goto
_LL662;}} else{ goto _LL663;} _LL663: if(( unsigned int) _temp659 > 4u?*(( int*)
_temp659) == Cyc_Absyn_AnonStructType: 0){ _LL679: _temp678=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp659)->f1; goto _LL664;} else{ goto _LL665;} _LL665: if(( unsigned int)
_temp659 > 4u?*(( int*) _temp659) == Cyc_Absyn_AnonUnionType: 0){ _LL681:
_temp680=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp659)->f1; goto _LL666;}
else{ goto _LL667;} _LL667: if(( unsigned int) _temp659 > 4u?*(( int*) _temp659)
== Cyc_Absyn_UnionType: 0){ _LL687: _temp686=(( struct Cyc_Absyn_UnionType_struct*)
_temp659)->f1; goto _LL685; _LL685: _temp684=(( struct Cyc_Absyn_UnionType_struct*)
_temp659)->f2; goto _LL683; _LL683: _temp682=(( struct Cyc_Absyn_UnionType_struct*)
_temp659)->f3; goto _LL668;} else{ goto _LL669;} _LL669: goto _LL670; _LL662:
if( _temp673->fields == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp688=( unsigned char*)"is_poly_field: struct missing fields";
struct _tagged_string _temp689; _temp689.curr= _temp688; _temp689.base= _temp688;
_temp689.last_plus_one= _temp688 + 37; _temp689;})));} _temp678=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp673->fields))->v; goto _LL664; _LL664: {
struct Cyc_Absyn_Structfield* _temp690= Cyc_Absyn_lookup_field( _temp678, f);
goto _LL691; _LL691: if( _temp690 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct _tagged_string _temp692=*((
struct Cyc_Absyn_Structfield*) _check_null( _temp690))->name; xprintf("is_poly_field: bad field %.*s",
_temp692.last_plus_one - _temp692.curr, _temp692.curr);}));} return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp690))->type);} _LL666:
return 0; _LL668: return 0; _LL670:( void) _throw((( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp693= Cyc_Absynpp_typ2string( t); xprintf("is_poly_field: bad type %.*s",
_temp693.last_plus_one - _temp693.curr, _temp693.curr);}))); _LL660:;} static
int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp694=( void*) e->r;
struct _tagged_string* _temp702; struct Cyc_Absyn_Exp* _temp704; struct
_tagged_string* _temp706; struct Cyc_Absyn_Exp* _temp708; _LL696: if(*(( int*)
_temp694) == Cyc_Absyn_StructMember_e){ _LL705: _temp704=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp694)->f1; goto _LL703; _LL703: _temp702=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp694)->f2; goto _LL697;} else{ goto _LL698;} _LL698: if(*(( int*) _temp694)
== Cyc_Absyn_StructArrow_e){ _LL709: _temp708=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp694)->f1; goto _LL707; _LL707: _temp706=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp694)->f2; goto _LL699;} else{ goto _LL700;} _LL700: goto _LL701; _LL697:
return Cyc_Toc_is_poly_field(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp704->topt))->v, _temp702); _LL699: { void* _temp710= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp708->topt))->v); struct Cyc_Absyn_PtrInfo
_temp716; struct Cyc_Absyn_Conref* _temp718; struct Cyc_Absyn_Tqual _temp720;
struct Cyc_Absyn_Conref* _temp722; void* _temp724; void* _temp726; _LL712: if((
unsigned int) _temp710 > 4u?*(( int*) _temp710) == Cyc_Absyn_PointerType: 0){
_LL717: _temp716=(( struct Cyc_Absyn_PointerType_struct*) _temp710)->f1; _LL727:
_temp726=( void*) _temp716.elt_typ; goto _LL725; _LL725: _temp724=( void*)
_temp716.rgn_typ; goto _LL723; _LL723: _temp722= _temp716.nullable; goto _LL721;
_LL721: _temp720= _temp716.tq; goto _LL719; _LL719: _temp718= _temp716.bounds;
goto _LL713;} else{ goto _LL714;} _LL714: goto _LL715; _LL713: return Cyc_Toc_is_poly_field(
_temp726, _temp706); _LL715:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp728= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp708->topt))->v); xprintf("is_poly_project:  bad type %.*s",
_temp728.last_plus_one - _temp728.curr, _temp728.curr);})); return 0; _LL711:;}
_LL701: return 0; _LL695:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_qv,
0),({ struct Cyc_List_List* _temp729=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp729->hd=( void*) s; _temp729->tl= 0; _temp729;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_atomic_qv, 0),({
struct Cyc_List_List* _temp730=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp730->hd=( void*) s; _temp730->tl= 0; _temp730;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__region_malloc_qv,
0),({ struct Cyc_List_List* _temp731=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp731->hd=( void*) rgn; _temp731->tl=({ struct Cyc_List_List*
_temp732=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp732->hd=( void*) s; _temp732->tl= 0; _temp732;}); _temp731;}), 0);} static
unsigned char _temp735[ 3u]="f0"; static struct _tagged_string Cyc_Toc_f0={
_temp735, _temp735, _temp735 + 3u}; static struct _tagged_string* Cyc_Toc_field_names_v[
1u]={& Cyc_Toc_f0}; struct _tagged_ptr3{ struct _tagged_string** curr; struct
_tagged_string** base; struct _tagged_string** last_plus_one; } ; static struct
_tagged_ptr3 Cyc_Toc_field_names={( struct _tagged_string**) Cyc_Toc_field_names_v,(
struct _tagged_string**) Cyc_Toc_field_names_v,( struct _tagged_string**) Cyc_Toc_field_names_v
+ 1u}; static struct _tagged_string* Cyc_Toc_fieldname( int i){ unsigned int fsz=({
struct _tagged_ptr3 _temp743= Cyc_Toc_field_names;( unsigned int)( _temp743.last_plus_one
- _temp743.curr);}); if( i >= fsz){ Cyc_Toc_field_names=({ unsigned int _temp738=(
unsigned int)( i + 1); struct _tagged_string** _temp739=( struct _tagged_string**)
GC_malloc( sizeof( struct _tagged_string*) * _temp738); struct _tagged_ptr3
_temp742={ _temp739, _temp739, _temp739 + _temp738};{ unsigned int _temp740=
_temp738; unsigned int j; for( j= 0; j < _temp740; j ++){ _temp739[ j]= j < fsz?*((
struct _tagged_string**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Toc_field_names, sizeof( struct _tagged_string*),(
int) j):({ struct _tagged_string* _temp741=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string)); _temp741[ 0]=( struct _tagged_string) xprintf("f%d",(
int) j); _temp741;});}}; _temp742;});} return*(( struct _tagged_string**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Toc_field_names,
sizeof( struct _tagged_string*), i);} static const int Cyc_Toc_NullCheck= 0;
struct Cyc_Toc_NullCheck_struct{ int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3; } ; static const int Cyc_Toc_UntagPtrNocheck= 2; struct Cyc_Toc_UntagPtrNocheck_struct{
int tag; void* f1; } ; static const int Cyc_Toc_TagPtr= 3; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Toc_NoConv=
0; struct _tuple7{ void* f1; void* f2; } ; static void* Cyc_Toc_conversion( void*
t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp745=({ struct _tuple7 _temp744; _temp744.f1= t1; _temp744.f2=
t2; _temp744;}); void* _temp753; struct Cyc_Absyn_PtrInfo _temp755; void*
_temp757; struct Cyc_Absyn_PtrInfo _temp759; void* _temp761; void* _temp763;
void* _temp765; void* _temp767; struct Cyc_Absyn_PtrInfo _temp769; _LL747:
_LL758: _temp757= _temp745.f1; if(( unsigned int) _temp757 > 4u?*(( int*)
_temp757) == Cyc_Absyn_PointerType: 0){ _LL760: _temp759=(( struct Cyc_Absyn_PointerType_struct*)
_temp757)->f1; goto _LL754;} else{ goto _LL749;} _LL754: _temp753= _temp745.f2;
if(( unsigned int) _temp753 > 4u?*(( int*) _temp753) == Cyc_Absyn_PointerType: 0){
_LL756: _temp755=(( struct Cyc_Absyn_PointerType_struct*) _temp753)->f1; goto
_LL748;} else{ goto _LL749;} _LL749: _LL768: _temp767= _temp745.f1; if((
unsigned int) _temp767 > 4u?*(( int*) _temp767) == Cyc_Absyn_PointerType: 0){
_LL770: _temp769=(( struct Cyc_Absyn_PointerType_struct*) _temp767)->f1; goto
_LL762;} else{ goto _LL751;} _LL762: _temp761= _temp745.f2; if(( unsigned int)
_temp761 > 4u?*(( int*) _temp761) == Cyc_Absyn_IntType: 0){ _LL766: _temp765=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp761)->f1; goto _LL764; _LL764:
_temp763=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp761)->f2; goto _LL750;}
else{ goto _LL751;} _LL751: goto _LL752; _LL748: { int _temp771=(( int(*)( int,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp759.nullable); goto
_LL772; _LL772: { int _temp773=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp755.nullable); goto _LL774; _LL774: { void* _temp775=(( void*(*)( void*,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp759.bounds); goto _LL776; _LL776: { void* _temp777=(( void*(*)( void*,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp755.bounds); goto _LL778; _LL778: { struct _tuple7 _temp780=({ struct
_tuple7 _temp779; _temp779.f1= _temp775; _temp779.f2= _temp777; _temp779;});
void* _temp790; struct Cyc_Absyn_Exp* _temp792; void* _temp794; struct Cyc_Absyn_Exp*
_temp796; void* _temp798; void* _temp800; void* _temp802; void* _temp804; struct
Cyc_Absyn_Exp* _temp806; void* _temp808; struct Cyc_Absyn_Exp* _temp810; void*
_temp812; _LL782: _LL795: _temp794= _temp780.f1; if(( unsigned int) _temp794 > 1u?*((
int*) _temp794) == Cyc_Absyn_Upper_b: 0){ _LL797: _temp796=(( struct Cyc_Absyn_Upper_b_struct*)
_temp794)->f1; goto _LL791;} else{ goto _LL784;} _LL791: _temp790= _temp780.f2;
if(( unsigned int) _temp790 > 1u?*(( int*) _temp790) == Cyc_Absyn_Upper_b: 0){
_LL793: _temp792=(( struct Cyc_Absyn_Upper_b_struct*) _temp790)->f1; goto _LL783;}
else{ goto _LL784;} _LL784: _LL801: _temp800= _temp780.f1; if( _temp800 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL799;} else{ goto _LL786;} _LL799: _temp798=
_temp780.f2; if( _temp798 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL785;} else{
goto _LL786;} _LL786: _LL805: _temp804= _temp780.f1; if(( unsigned int) _temp804
> 1u?*(( int*) _temp804) == Cyc_Absyn_Upper_b: 0){ _LL807: _temp806=(( struct
Cyc_Absyn_Upper_b_struct*) _temp804)->f1; goto _LL803;} else{ goto _LL788;}
_LL803: _temp802= _temp780.f2; if( _temp802 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL787;} else{ goto _LL788;} _LL788: _LL813: _temp812= _temp780.f1; if(
_temp812 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL809;} else{ goto _LL781;}
_LL809: _temp808= _temp780.f2; if(( unsigned int) _temp808 > 1u?*(( int*)
_temp808) == Cyc_Absyn_Upper_b: 0){ _LL811: _temp810=(( struct Cyc_Absyn_Upper_b_struct*)
_temp808)->f1; goto _LL789;} else{ goto _LL781;} _LL783: if( _temp771? !
_temp773: 0){ return( void*)({ struct Cyc_Toc_NullCheck_struct* _temp814=(
struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp814[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp815; _temp815.tag= Cyc_Toc_NullCheck;
_temp815.f1=( void*) t2; _temp815;}); _temp814;});} return( void*) Cyc_Toc_NoConv;
_LL785: return( void*) Cyc_Toc_NoConv; _LL787: return( void*)({ struct Cyc_Toc_TagPtr_struct*
_temp816=( struct Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp816[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp817; _temp817.tag= Cyc_Toc_TagPtr;
_temp817.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp817.f2= _temp806; _temp817;});
_temp816;}); _LL789: return( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp818=(
struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp818[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp819; _temp819.tag= Cyc_Toc_UntagPtr;
_temp819.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp819.f2= _temp810; _temp819.f3=
_temp773; _temp819;}); _temp818;}); _LL781:;}}}}} _LL750: { void* _temp820=((
void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp769.bounds); goto _LL821; _LL821: { void* _temp822= _temp820; struct Cyc_Absyn_Exp*
_temp828; _LL824: if(( unsigned int) _temp822 > 1u?*(( int*) _temp822) == Cyc_Absyn_Upper_b:
0){ _LL829: _temp828=(( struct Cyc_Absyn_Upper_b_struct*) _temp822)->f1; goto
_LL825;} else{ goto _LL826;} _LL826: if( _temp822 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL827;} else{ goto _LL823;} _LL825: return( void*) Cyc_Toc_NoConv; _LL827:
return( void*)({ struct Cyc_Toc_UntagPtrNocheck_struct* _temp830=( struct Cyc_Toc_UntagPtrNocheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_UntagPtrNocheck_struct)); _temp830[ 0]=({
struct Cyc_Toc_UntagPtrNocheck_struct _temp831; _temp831.tag= Cyc_Toc_UntagPtrNocheck;
_temp831.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp831;}); _temp830;}); _LL823:;}}
_LL752: return( void*) Cyc_Toc_NoConv; _LL746:;}} static unsigned char _temp834[
14u]="sfile_to_file"; static struct _tagged_string Cyc_Toc__sfile_to_file_string={
_temp834, _temp834, _temp834 + 14u}; static struct _tuple0 Cyc_Toc__sfile_to_file_var={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__sfile_to_file_string}; static struct Cyc_Absyn_Exp*
Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(&
Cyc_Toc__sfile_to_file_var, 0),({ struct Cyc_List_List* _temp835=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp835->hd=( void*) e; _temp835->tl=
0; _temp835;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct
Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__throw_qv,
0),({ struct Cyc_List_List* _temp836=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp836->hd=( void*) e; _temp836->tl= 0; _temp836;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp837=(
unsigned char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp838; _temp838.curr= _temp837; _temp838.base= _temp837;
_temp838.last_plus_one= _temp837 + 44; _temp838;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp839=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp839->hd=( void*) e; _temp839->tl= 0; _temp839;}), 0),
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
_temp840= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp846; struct Cyc_Absyn_Conref*
_temp848; struct Cyc_Absyn_Tqual _temp850; struct Cyc_Absyn_Conref* _temp852;
void* _temp854; void* _temp856; _LL842: if(( unsigned int) _temp840 > 4u?*(( int*)
_temp840) == Cyc_Absyn_PointerType: 0){ _LL847: _temp846=(( struct Cyc_Absyn_PointerType_struct*)
_temp840)->f1; _LL857: _temp856=( void*) _temp846.elt_typ; goto _LL855; _LL855:
_temp854=( void*) _temp846.rgn_typ; goto _LL853; _LL853: _temp852= _temp846.nullable;
goto _LL851; _LL851: _temp850= _temp846.tq; goto _LL849; _LL849: _temp848=
_temp846.bounds; goto _LL843;} else{ goto _LL844;} _LL844: goto _LL845; _LL843:
elt_typ= _temp856; elt_tqual= _temp850; strt_typ= Cyc_Toc_add_tagged_ptr_type(
_temp856, _temp850); goto _LL841; _LL845: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp858=(
unsigned char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp859;
_temp859.curr= _temp858; _temp859.base= _temp858; _temp859.last_plus_one=
_temp858 + 28; _temp859;})); _LL841:;} if( toplevel){ int is_string= 0;{ void*
_temp860=( void*) e->r; void* _temp866; struct _tagged_string _temp868; _LL862:
if(*(( int*) _temp860) == Cyc_Absyn_Const_e){ _LL867: _temp866=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp860)->f1; if(( unsigned int) _temp866 > 1u?*((
int*) _temp866) == Cyc_Absyn_String_c: 0){ _LL869: _temp868=(( struct Cyc_Absyn_String_c_struct*)
_temp866)->f1; goto _LL863;} else{ goto _LL864;}} else{ goto _LL864;} _LL864:
goto _LL865; _LL863: is_string= 1; goto _LL861; _LL865: goto _LL861; _LL861:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp873=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp873[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp874; _temp874.tag= Cyc_Absyn_ArrayType; _temp874.f1=(
void*) elt_typ; _temp874.f2= Cyc_Toc_mt_tq; _temp874.f3=( struct Cyc_Absyn_Exp*)
sz; _temp874;}); _temp873;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp870=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp870->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp871=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp871[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp872; _temp872.tag= Cyc_Absyn_Var_d;
_temp872.f1= vd; _temp872;}); _temp871;}), 0); _temp870->tl= Cyc_Toc_result_decls;
_temp870;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ if( elt_tqual.q_const){ xexp=
Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq), e, 0);} else{
xexp= e;}}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp( xexp, sz, 0);
struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(*
_temp875)( struct _tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4))
Cyc_List_list; struct _tuple4* _temp878=({ struct _tuple4* _temp883=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp883->f1= 0; _temp883->f2=
xexp; _temp883;}); struct _tuple4* _temp879=({ struct _tuple4* _temp882=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp882->f1= 0; _temp882->f2=
xexp; _temp882;}); struct _tuple4* _temp880=({ struct _tuple4* _temp881=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp881->f1= 0; _temp881->f2=
xplussz; _temp881;}); struct _tuple4* _temp876[ 3u]={ _temp878, _temp879,
_temp880}; struct _tagged_ptr4 _temp877={ _temp876, _temp876, _temp876 + 3u};
_temp875( _temp877);}), 0); return urm_exp;}}} else{ struct Cyc_Absyn_Stmt* s=
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
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp884=( unsigned char*)"can't coerce t? to t* or t@ at the top-level";
struct _tagged_string _temp885; _temp885.curr= _temp884; _temp885.base= _temp884;
_temp885.last_plus_one= _temp884 + 45; _temp885;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
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
return({ struct Cyc_Toc_Env* _temp886=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp886->break_lab= 0; _temp886->continue_lab= 0;
_temp886->fallthru_info= 0; _temp886->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp886->toplevel= 1; _temp886;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp887=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp887->break_lab= e->break_lab;
_temp887->continue_lab= e->continue_lab; _temp887->fallthru_info= e->fallthru_info;
_temp887->varmap= e->varmap; _temp887->toplevel= e->toplevel; _temp887;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp888=(* x).f1; struct Cyc_List_List*
_temp894; _LL890: if(( unsigned int) _temp888 > 1u?*(( int*) _temp888) == Cyc_Absyn_Rel_n:
0){ _LL895: _temp894=(( struct Cyc_Absyn_Rel_n_struct*) _temp888)->f1; goto
_LL891;} else{ goto _LL892;} _LL892: goto _LL893; _LL891:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp896=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp896[ 0]=({ struct
Cyc_Core_Impossible_struct _temp897; _temp897.tag= Cyc_Core_Impossible; _temp897.f1=(
struct _tagged_string)({ struct _tagged_string _temp898= Cyc_Absynpp_qvar2string(
x); xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp898.last_plus_one -
_temp898.curr, _temp898.curr);}); _temp897;}); _temp896;})); _LL893: goto _LL889;
_LL889:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp*
data)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env*
Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab= 0; ans->continue_lab= 0; return ans;} struct _tuple8{ struct
_tagged_string* f1; struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ;
static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_string* break_l, struct _tagged_string* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
_temp899= 0; goto _LL900; _LL900: for( 0; fallthru_binders != 0;
fallthru_binders=(( struct Cyc_List_List*) _check_null( fallthru_binders))->tl){
_temp899=({ struct Cyc_List_List* _temp901=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp901->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp901->tl=
_temp899; _temp901;});} _temp899=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp899);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp902=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp902->v=( void*) break_l; _temp902;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp903=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp903->v=( void*)({ struct _tuple8*
_temp904=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp904->f1=
fallthru_l; _temp904->f2= _temp899; _temp904->f3= next_case_env->varmap;
_temp904;}); _temp903;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp905=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp905->v=( void*)
break_l; _temp905;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_string*
next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp906=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp906->v=( void*)({ struct _tuple8* _temp907=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp907->f1= next_l; _temp907->f2= 0;
_temp907->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp907;}); _temp906;});
return ans;} struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tagged_ptr5{ struct Cyc_List_List** curr; struct Cyc_List_List** base;
struct Cyc_List_List** last_plus_one; } ; struct _tuple10{ struct _tuple0* f1;
void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp908=( unsigned char*)"expecting a literal format string";
struct _tagged_string _temp909; _temp909.curr= _temp908; _temp909.base= _temp908;
_temp909.last_plus_one= _temp908 + 34; _temp909;}));}{ struct _tagged_string
fmt_str;{ void* _temp910=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd)->r; void* _temp916; struct _tagged_string _temp918;
_LL912: if(*(( int*) _temp910) == Cyc_Absyn_Const_e){ _LL917: _temp916=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp910)->f1; if(( unsigned int) _temp916 > 1u?*((
int*) _temp916) == Cyc_Absyn_String_c: 0){ _LL919: _temp918=(( struct Cyc_Absyn_String_c_struct*)
_temp916)->f1; goto _LL913;} else{ goto _LL914;}} else{ goto _LL914;} _LL914:
goto _LL915; _LL913: fmt_str= _temp918; goto _LL911; _LL915: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp920=(
unsigned char*)"expecting a literal format string"; struct _tagged_string
_temp921; _temp921.curr= _temp920; _temp921.base= _temp920; _temp921.last_plus_one=
_temp920 + 34; _temp921;})); _LL911:;}{ int len=( int)({ struct _tagged_string
_temp1017= fmt_str;( unsigned int)( _temp1017.last_plus_one - _temp1017.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str,
sizeof( unsigned char), i) =='%'){ i ++; if( i < len?*(( const unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( unsigned char), i) =='s': 0){ break;}}} if( i == len){ if( aopt
!= 0){ args=({ struct Cyc_List_List* _temp922=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp922->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp922->tl= args; _temp922;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp923= _new_region(); struct
_RegionHandle* r=& _temp923; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str,
sizeof( unsigned char), i); rev_fmt=({ struct Cyc_List_List* _temp924=( struct
Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List)); _temp924->hd=(
void*)(( int) c); _temp924->tl= rev_fmt; _temp924;}); if( c !='%'){ continue;}{
struct Cyc_Core_Opt* _temp925= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); goto _LL926; _LL926: if( _temp925 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp927=( unsigned char*)"bad format string";
struct _tagged_string _temp928; _temp928.curr= _temp927; _temp928.base= _temp927;
_temp928.last_plus_one= _temp927 + 18; _temp928;}));}{ struct _tuple9 _temp931;
int _temp932; unsigned char _temp934; struct Cyc_List_List* _temp936; struct Cyc_List_List*
_temp938; struct Cyc_List_List* _temp940; struct Cyc_List_List* _temp942; struct
_tuple9* _temp929=( struct _tuple9*)(( struct Cyc_Core_Opt*) _check_null(
_temp925))->v; _temp931=* _temp929; _LL943: _temp942= _temp931.f1; goto _LL941;
_LL941: _temp940= _temp931.f2; goto _LL939; _LL939: _temp938= _temp931.f3; goto
_LL937; _LL937: _temp936= _temp931.f4; goto _LL935; _LL935: _temp934= _temp931.f5;
goto _LL933; _LL933: _temp932= _temp931.f6; goto _LL930; _LL930: i= _temp932 - 1;
if( _temp934 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp944)( struct _RegionHandle*, struct
_tagged_ptr5)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_ptr5)) Cyc_List_rlist; struct _RegionHandle* _temp951= r; struct Cyc_List_List*
_temp947= _temp942; struct Cyc_List_List* _temp948= _temp940; struct Cyc_List_List*
_temp949= _temp938; struct Cyc_List_List* _temp950= _temp936; struct Cyc_List_List*
_temp945[ 4u]={ _temp947, _temp948, _temp949, _temp950}; struct _tagged_ptr5
_temp946={ _temp945, _temp945, _temp945 + 4u}; _temp944( _temp951, _temp946);})),
rev_fmt); rev_fmt=({ struct Cyc_List_List* _temp952=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp952->hd=( void*)(( int)
_temp934); _temp952->tl= rev_fmt; _temp952;});{ struct Cyc_List_List* _temp953=
_temp940; struct Cyc_List_List _temp960; struct Cyc_List_List* _temp961; int
_temp963; _LL955: if( _temp953 == 0){ goto _LL957;} else{ _temp960=* _temp953;
_LL964: _temp963=( int) _temp960.hd; goto _LL962; _LL962: _temp961= _temp960.tl;
if( _temp961 == 0){ goto _LL959;} else{ goto _LL957;}} _LL959: if( _temp963 =='*'){
goto _LL956;} else{ goto _LL957;} _LL957: goto _LL958; _LL956: { struct _tuple0*
_temp965= Cyc_Toc_temp_var(); goto _LL966; _LL966: rev_temps=({ struct Cyc_List_List*
_temp967=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp967->hd=( void*)({ struct _tuple10* _temp968=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp968->f1= _temp965; _temp968->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp968->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp968;});
_temp967->tl= rev_temps; _temp967;}); rev_result=({ struct Cyc_List_List*
_temp969=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp969->hd=( void*) Cyc_Absyn_var_exp( _temp965, 0); _temp969->tl= rev_result;
_temp969;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL954;} _LL958: goto
_LL954; _LL954:;}{ struct Cyc_List_List* _temp970= _temp938; struct Cyc_List_List
_temp977; struct Cyc_List_List* _temp978; struct Cyc_List_List _temp980; struct
Cyc_List_List* _temp981; int _temp983; int _temp985; _LL972: if( _temp970 == 0){
goto _LL974;} else{ _temp977=* _temp970; _LL986: _temp985=( int) _temp977.hd;
goto _LL979; _LL979: _temp978= _temp977.tl; if( _temp978 == 0){ goto _LL974;}
else{ _temp980=* _temp978; _LL984: _temp983=( int) _temp980.hd; goto _LL982;
_LL982: _temp981= _temp980.tl; if( _temp981 == 0){ goto _LL976;} else{ goto
_LL974;}}} _LL976: if( _temp985 =='.'? _temp983 =='*': 0){ goto _LL973;} else{
goto _LL974;} _LL974: goto _LL975; _LL973: { struct _tuple0* _temp987= Cyc_Toc_temp_var();
goto _LL988; _LL988: rev_temps=({ struct Cyc_List_List* _temp989=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp989->hd=( void*)({
struct _tuple10* _temp990=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp990->f1= _temp987; _temp990->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp990->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp990;}); _temp989->tl= rev_temps; _temp989;});
rev_result=({ struct Cyc_List_List* _temp991=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp991->hd=( void*) Cyc_Absyn_var_exp(
_temp987, 0); _temp991->tl= rev_result; _temp991;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL971;} _LL975: goto _LL971; _LL971:;} if( _temp934 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ struct _tagged_string _temp992= Cyc_Absynpp_prim2string( p);
xprintf("missing arg to %.*s", _temp992.last_plus_one - _temp992.curr, _temp992.curr);}));}{
struct _tuple0* _temp993= Cyc_Toc_temp_var(); goto _LL994; _LL994: rev_temps=({
struct Cyc_List_List* _temp995=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp995->hd=( void*)({ struct _tuple10*
_temp996=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp996->f1= _temp993; _temp996->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp996->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp996;}); _temp995->tl= rev_temps; _temp995;});
rev_result=({ struct Cyc_List_List* _temp997=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp997->hd=( void*) Cyc_Absyn_var_exp(
_temp993, 0); _temp997->tl= rev_result; _temp997;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp998=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp998->hd=( void*)(( int)'.');
_temp998->tl= rev_fmt; _temp998;}); rev_fmt=({ struct Cyc_List_List* _temp999=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp999->hd=( void*)(( int)'*'); _temp999->tl= rev_fmt; _temp999;}); rev_fmt=({
struct Cyc_List_List* _temp1000=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp1000->hd=( void*)(( int)'s'); _temp1000->tl=
rev_fmt; _temp1000;});{ struct _tuple0* _temp1001= Cyc_Toc_temp_var(); goto
_LL1002; _LL1002: rev_temps=({ struct Cyc_List_List* _temp1003=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1003->hd=( void*)({
struct _tuple10* _temp1004=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1004->f1= _temp1001; _temp1004->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp1004->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp1004;}); _temp1003->tl= rev_temps; _temp1003;});{
struct Cyc_Absyn_Exp* _temp1005= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1001, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1001, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL1006; _LL1006: {
struct Cyc_Absyn_Exp* _temp1007= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1001, 0), Cyc_Toc_curr_sp, 0); goto _LL1008; _LL1008: rev_result=({ struct
Cyc_List_List* _temp1009=( struct Cyc_List_List*) _region_malloc( r, sizeof(
struct Cyc_List_List)); _temp1009->hd=( void*) _temp1005; _temp1009->tl=
rev_result; _temp1009;}); rev_result=({ struct Cyc_List_List* _temp1010=( struct
Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List)); _temp1010->hd=(
void*) _temp1007; _temp1010->tl= rev_result; _temp1010;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}}{
struct Cyc_List_List* _temp1012=({ struct Cyc_List_List* _temp1011=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1011->hd=( void*) Cyc_Absyn_string_exp((
struct _tagged_string) Cyc_String_implode((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0); _temp1011->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_result); _temp1011;}); goto _LL1013;
_LL1013: if( aopt != 0){ _temp1012=({ struct Cyc_List_List* _temp1014=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1014->hd=( void*)((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( aopt))->v);
_temp1014->tl= _temp1012; _temp1014;});}{ struct Cyc_Absyn_Exp* _temp1015= Cyc_Absyn_primop_exp(
p, _temp1012, 0); goto _LL1016; _LL1016: s= Cyc_Absyn_exp_stmt( _temp1015, 0);
for( 0; rev_temps != 0; rev_temps=(( struct Cyc_List_List*) _check_null(
rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f1,(*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f2,( struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)((
struct Cyc_List_List*) _check_null( rev_temps))->hd)).f3, s, 0);}}}};
_pop_region(& _temp923);} return Cyc_Absyn_stmt_exp( s, 0);}}}} static void* Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1018=( unsigned char*)"Missing type in primop ";
struct _tagged_string _temp1019; _temp1019.curr= _temp1018; _temp1019.base=
_temp1018; _temp1019.last_plus_one= _temp1018 + 24; _temp1019;}));} return Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1020=( unsigned char*)"Missing type in primop ";
struct _tagged_string _temp1021; _temp1021.curr= _temp1020; _temp1021.base=
_temp1020; _temp1021.last_plus_one= _temp1020 + 24; _temp1021;}));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1022=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1022->f1= Cyc_Toc_mt_tq; _temp1022->f2=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp1022;});} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp1023=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1023[
0]=({ struct _tuple4 _temp1024; _temp1024.f1= 0; _temp1024.f2= e; _temp1024;});
_temp1023;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
_temp1025= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); goto _LL1026; _LL1026: if(
rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic(
_temp1025): Cyc_Toc_malloc_ptr( _temp1025), 0);} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c( nv, r); eo=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp( r, _temp1025),
0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp1027=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL1028; _LL1028: for( 0;
_temp1027 != 0; _temp1027=(( struct Cyc_List_List*) _check_null( _temp1027))->tl){
struct _tuple4 _temp1031; struct Cyc_Absyn_Exp* _temp1032; struct Cyc_List_List*
_temp1034; struct _tuple4* _temp1029=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1027))->hd; _temp1031=* _temp1029; _LL1035: _temp1034=
_temp1031.f1; goto _LL1033; _LL1033: _temp1032= _temp1031.f2; goto _LL1030;
_LL1030: { struct Cyc_Absyn_Exp* e_index; if( _temp1034 == 0){ e_index= Cyc_Absyn_signed_int_exp(
count --, 0);} else{ if((( struct Cyc_List_List*) _check_null( _temp1034))->tl
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp1036=( unsigned char*)"multiple designators in array"; struct
_tagged_string _temp1037; _temp1037.curr= _temp1036; _temp1037.base= _temp1036;
_temp1037.last_plus_one= _temp1036 + 30; _temp1037;}));}{ void* _temp1038=( void*)((
struct Cyc_List_List*) _check_null( _temp1034))->hd; goto _LL1039; _LL1039: {
void* _temp1040= _temp1038; struct Cyc_Absyn_Exp* _temp1046; struct
_tagged_string* _temp1048; _LL1042: if(*(( int*) _temp1040) == Cyc_Absyn_ArrayElement){
_LL1047: _temp1046=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1040)->f1;
goto _LL1043;} else{ goto _LL1044;} _LL1044: if(*(( int*) _temp1040) == Cyc_Absyn_FieldName){
_LL1049: _temp1048=(( struct Cyc_Absyn_FieldName_struct*) _temp1040)->f1; goto
_LL1045;} else{ goto _LL1041;} _LL1043: Cyc_Toc_exp_to_c( nv, _temp1046);
e_index= _temp1046; goto _LL1041; _LL1045: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp1050=(
unsigned char*)"field name designators in array"; struct _tagged_string
_temp1051; _temp1051.curr= _temp1050; _temp1051.base= _temp1050; _temp1051.last_plus_one=
_temp1050 + 32; _temp1051;})); goto _LL1041; _LL1041:;}}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp1052=( void*)
_temp1032->r; struct Cyc_List_List* _temp1062; struct Cyc_Absyn_Exp* _temp1064;
struct Cyc_Absyn_Exp* _temp1066; struct Cyc_Absyn_Vardecl* _temp1068; struct Cyc_List_List*
_temp1070; void* _temp1072; _LL1054: if(*(( int*) _temp1052) == Cyc_Absyn_Array_e){
_LL1063: _temp1062=(( struct Cyc_Absyn_Array_e_struct*) _temp1052)->f1; goto
_LL1055;} else{ goto _LL1056;} _LL1056: if(*(( int*) _temp1052) == Cyc_Absyn_Comprehension_e){
_LL1069: _temp1068=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1052)->f1;
goto _LL1067; _LL1067: _temp1066=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1052)->f2; goto _LL1065; _LL1065: _temp1064=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1052)->f3; goto _LL1057;} else{ goto _LL1058;} _LL1058: if(*(( int*)
_temp1052) == Cyc_Absyn_AnonStruct_e){ _LL1073: _temp1072=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1052)->f1; goto _LL1071; _LL1071: _temp1070=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1052)->f2; goto _LL1059;} else{ goto _LL1060;} _LL1060: goto _LL1061;
_LL1055: s= Cyc_Toc_init_array( nv, lval, _temp1062, s); goto _LL1053; _LL1057:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1068, _temp1066, _temp1064, s, 0);
goto _LL1053; _LL1059: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1072,
_temp1070, s); goto _LL1053; _LL1061: Cyc_Toc_exp_to_c( nv, _temp1032); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1032, 0),
s, 0); goto _LL1053; _LL1053:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* _temp1074= vd->name; goto _LL1075;
_LL1075: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1074, Cyc_Absyn_varb_exp( _temp1074,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1098=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1098[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1099; _temp1099.tag= Cyc_Absyn_Local_b; _temp1099.f1=
vd; _temp1099;}); _temp1098;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp1074, 0),
Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( _temp1074, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp*
ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( _temp1074, 0), 0); struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( _temp1074, 0), 0); struct
Cyc_Absyn_Stmt* body;{ void* _temp1076=( void*) e2->r; struct Cyc_List_List*
_temp1086; struct Cyc_Absyn_Exp* _temp1088; struct Cyc_Absyn_Exp* _temp1090;
struct Cyc_Absyn_Vardecl* _temp1092; struct Cyc_List_List* _temp1094; void*
_temp1096; _LL1078: if(*(( int*) _temp1076) == Cyc_Absyn_Array_e){ _LL1087:
_temp1086=(( struct Cyc_Absyn_Array_e_struct*) _temp1076)->f1; goto _LL1079;}
else{ goto _LL1080;} _LL1080: if(*(( int*) _temp1076) == Cyc_Absyn_Comprehension_e){
_LL1093: _temp1092=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1076)->f1;
goto _LL1091; _LL1091: _temp1090=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1076)->f2; goto _LL1089; _LL1089: _temp1088=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1076)->f3; goto _LL1081;} else{ goto _LL1082;} _LL1082: if(*(( int*)
_temp1076) == Cyc_Absyn_AnonStruct_e){ _LL1097: _temp1096=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1076)->f1; goto _LL1095; _LL1095: _temp1094=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1076)->f2; goto _LL1083;} else{ goto _LL1084;} _LL1084: goto _LL1085;
_LL1079: body= Cyc_Toc_init_array( nv2, lval, _temp1086, Cyc_Absyn_skip_stmt( 0));
goto _LL1077; _LL1081: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1092,
_temp1090, _temp1088, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1077; _LL1083: body=
Cyc_Toc_init_anon_struct( nv, lval, _temp1096, _temp1094, Cyc_Absyn_skip_stmt( 0));
goto _LL1077; _LL1085: Cyc_Toc_exp_to_c( nv2, e2); body= Cyc_Absyn_assign_stmt(
lval, e2, 0); goto _LL1077; _LL1077:;}{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt(
ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( _temp1074, Cyc_Absyn_uint_t,
0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s){{ struct Cyc_List_List* _temp1100=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto _LL1101; _LL1101: for( 0;
_temp1100 != 0; _temp1100=(( struct Cyc_List_List*) _check_null( _temp1100))->tl){
struct _tuple4 _temp1104; struct Cyc_Absyn_Exp* _temp1105; struct Cyc_List_List*
_temp1107; struct _tuple4* _temp1102=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1100))->hd; _temp1104=* _temp1102; _LL1108: _temp1107=
_temp1104.f1; goto _LL1106; _LL1106: _temp1105= _temp1104.f2; goto _LL1103;
_LL1103: if( _temp1107 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1109=( unsigned char*)"empty designator list"; struct
_tagged_string _temp1110; _temp1110.curr= _temp1109; _temp1110.base= _temp1109;
_temp1110.last_plus_one= _temp1109 + 22; _temp1110;}));} if((( struct Cyc_List_List*)
_check_null( _temp1107))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1111=( unsigned char*)"too many designators in anonymous struct";
struct _tagged_string _temp1112; _temp1112.curr= _temp1111; _temp1112.base=
_temp1111; _temp1112.last_plus_one= _temp1111 + 41; _temp1112;}));}{ void*
_temp1113=( void*)(( struct Cyc_List_List*) _check_null( _temp1107))->hd; struct
_tagged_string* _temp1119; _LL1115: if(*(( int*) _temp1113) == Cyc_Absyn_FieldName){
_LL1120: _temp1119=(( struct Cyc_Absyn_FieldName_struct*) _temp1113)->f1; goto
_LL1116;} else{ goto _LL1117;} _LL1117: goto _LL1118; _LL1116: { struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_structmember_exp( lhs, _temp1119, 0);{ void* _temp1121=( void*)
_temp1105->r; struct Cyc_List_List* _temp1131; struct Cyc_Absyn_Exp* _temp1133;
struct Cyc_Absyn_Exp* _temp1135; struct Cyc_Absyn_Vardecl* _temp1137; struct Cyc_List_List*
_temp1139; void* _temp1141; _LL1123: if(*(( int*) _temp1121) == Cyc_Absyn_Array_e){
_LL1132: _temp1131=(( struct Cyc_Absyn_Array_e_struct*) _temp1121)->f1; goto
_LL1124;} else{ goto _LL1125;} _LL1125: if(*(( int*) _temp1121) == Cyc_Absyn_Comprehension_e){
_LL1138: _temp1137=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1121)->f1;
goto _LL1136; _LL1136: _temp1135=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1121)->f2; goto _LL1134; _LL1134: _temp1133=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1121)->f3; goto _LL1126;} else{ goto _LL1127;} _LL1127: if(*(( int*)
_temp1121) == Cyc_Absyn_AnonStruct_e){ _LL1142: _temp1141=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1121)->f1; goto _LL1140; _LL1140: _temp1139=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1121)->f2; goto _LL1128;} else{ goto _LL1129;} _LL1129: goto _LL1130;
_LL1124: s= Cyc_Toc_init_array( nv, lval, _temp1131, s); goto _LL1122; _LL1126:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1137, _temp1135, _temp1133, s, 0);
goto _LL1122; _LL1128: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1141,
_temp1139, s); goto _LL1122; _LL1130: Cyc_Toc_exp_to_c( nv, _temp1105); if( Cyc_Toc_is_poly_field(
struct_type, _temp1119)){ _temp1105= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1105, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1105, 0), 0), s, 0); goto _LL1122; _LL1122:;} goto _LL1114;} _LL1118:(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1143=( unsigned char*)"array designator in struct"; struct
_tagged_string _temp1144; _temp1144.curr= _temp1143; _temp1144.base= _temp1143;
_temp1144.last_plus_one= _temp1143 + 27; _temp1144;}))); _LL1114:;}}} return s;}
static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int
pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
_RegionHandle _temp1145= _new_region(); struct _RegionHandle* r=& _temp1145;
_push_region( r);{ struct Cyc_List_List* _temp1146=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c, es); goto _LL1147; _LL1147: { struct
_tagged_string* _temp1148= Cyc_Toc_add_tuple_type( _temp1146); goto _LL1149;
_LL1149: { struct _tuple0* _temp1150= Cyc_Toc_temp_var(); goto _LL1151; _LL1151: {
struct Cyc_Absyn_Exp* _temp1152= Cyc_Absyn_var_exp( _temp1150, 0); goto _LL1153;
_LL1153: { struct Cyc_Absyn_Stmt* _temp1154= Cyc_Absyn_exp_stmt( _temp1152, 0);
goto _LL1155; _LL1155: { struct Cyc_Absyn_Exp*(* _temp1156)( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; goto _LL1157; _LL1157: { int is_atomic= 1; struct
Cyc_List_List* _temp1158=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( r, es); goto _LL1159; _LL1159:{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1158); for( 0;
_temp1158 != 0;( _temp1158=(( struct Cyc_List_List*) _check_null( _temp1158))->tl,
-- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1158))->hd; struct Cyc_Absyn_Exp* lval= _temp1156( _temp1152,
Cyc_Toc_fieldname( i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v): 0;{ void* _temp1160=( void*) e->r;
struct Cyc_List_List* _temp1168; struct Cyc_Absyn_Exp* _temp1170; struct Cyc_Absyn_Exp*
_temp1172; struct Cyc_Absyn_Vardecl* _temp1174; _LL1162: if(*(( int*) _temp1160)
== Cyc_Absyn_Array_e){ _LL1169: _temp1168=(( struct Cyc_Absyn_Array_e_struct*)
_temp1160)->f1; goto _LL1163;} else{ goto _LL1164;} _LL1164: if(*(( int*)
_temp1160) == Cyc_Absyn_Comprehension_e){ _LL1175: _temp1174=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1160)->f1; goto _LL1173; _LL1173: _temp1172=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1160)->f2; goto _LL1171; _LL1171: _temp1170=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1160)->f3; goto _LL1165;} else{ goto _LL1166;} _LL1166: goto _LL1167;
_LL1163: _temp1154= Cyc_Toc_init_array( nv, lval, _temp1168, _temp1154); goto
_LL1161; _LL1165: _temp1154= Cyc_Toc_init_comprehension( nv, lval, _temp1174,
_temp1172, _temp1170, _temp1154, 0); goto _LL1161; _LL1167: Cyc_Toc_exp_to_c( nv,
e); _temp1154= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1156( _temp1152, Cyc_Toc_fieldname( i), 0), e, 0), 0), _temp1154, 0); goto
_LL1161; _LL1161:;}}}{ struct Cyc_Absyn_Exp* _temp1176= Cyc_Toc_make_struct( nv,
_temp1150, Cyc_Absyn_strct( _temp1148), _temp1154, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp1176;}}}}}}}}; _pop_region(& _temp1145);} static
struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env* nv, void*
struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp1177= Cyc_Toc_temp_var(); goto
_LL1178; _LL1178: { struct Cyc_Absyn_Exp* _temp1179= Cyc_Absyn_var_exp(
_temp1177, 0); goto _LL1180; _LL1180: { struct Cyc_Absyn_Stmt* _temp1181= Cyc_Absyn_exp_stmt(
_temp1179, 0); goto _LL1182; _LL1182: { struct Cyc_Absyn_Exp*(* _temp1183)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1184;
_LL1184: { int is_atomic= 1; struct _RegionHandle _temp1185= _new_region();
struct _RegionHandle* r=& _temp1185; _push_region( r);{ struct Cyc_List_List*
_temp1186=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, dles); goto _LL1187; _LL1187: for( 0; _temp1186 != 0;
_temp1186=(( struct Cyc_List_List*) _check_null( _temp1186))->tl){ struct
_tuple4 _temp1190; struct Cyc_Absyn_Exp* _temp1191; struct Cyc_List_List*
_temp1193; struct _tuple4* _temp1188=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1186))->hd; _temp1190=* _temp1188; _LL1194: _temp1193=
_temp1190.f1; goto _LL1192; _LL1192: _temp1191= _temp1190.f2; goto _LL1189;
_LL1189: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1191->topt))->v): 0; if( _temp1193 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1195=(
unsigned char*)"empty designator list"; struct _tagged_string _temp1196;
_temp1196.curr= _temp1195; _temp1196.base= _temp1195; _temp1196.last_plus_one=
_temp1195 + 22; _temp1196;}));} if((( struct Cyc_List_List*) _check_null(
_temp1193))->tl != 0){ struct _tuple0* _temp1197= Cyc_Toc_temp_var(); goto
_LL1198; _LL1198: { struct Cyc_Absyn_Exp* _temp1199= Cyc_Absyn_var_exp(
_temp1197, 0); goto _LL1200; _LL1200: for( 0; _temp1193 != 0; _temp1193=((
struct Cyc_List_List*) _check_null( _temp1193))->tl){ void* _temp1201=( void*)((
struct Cyc_List_List*) _check_null( _temp1193))->hd; struct _tagged_string*
_temp1207; _LL1203: if(*(( int*) _temp1201) == Cyc_Absyn_FieldName){ _LL1208:
_temp1207=(( struct Cyc_Absyn_FieldName_struct*) _temp1201)->f1; goto _LL1204;}
else{ goto _LL1205;} _LL1205: goto _LL1206; _LL1204: if( Cyc_Toc_is_poly_field(
struct_type, _temp1207)){ _temp1199= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1199, 0);} _temp1181= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1183( _temp1179, _temp1207, 0), _temp1199, 0), 0), _temp1181, 0); goto
_LL1202; _LL1206:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1209=( unsigned char*)"array designator in struct"; struct
_tagged_string _temp1210; _temp1210.curr= _temp1209; _temp1210.base= _temp1209;
_temp1210.last_plus_one= _temp1209 + 27; _temp1210;}))); _LL1202:;} Cyc_Toc_exp_to_c(
nv, _temp1191); _temp1181= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1199, _temp1191, 0), 0), _temp1181, 0);}} else{ void* _temp1211=( void*)((
struct Cyc_List_List*) _check_null( _temp1193))->hd; struct _tagged_string*
_temp1217; _LL1213: if(*(( int*) _temp1211) == Cyc_Absyn_FieldName){ _LL1218:
_temp1217=(( struct Cyc_Absyn_FieldName_struct*) _temp1211)->f1; goto _LL1214;}
else{ goto _LL1215;} _LL1215: goto _LL1216; _LL1214: { struct Cyc_Absyn_Exp*
lval= _temp1183( _temp1179, _temp1217, 0);{ void* _temp1219=( void*) _temp1191->r;
struct Cyc_List_List* _temp1229; struct Cyc_Absyn_Exp* _temp1231; struct Cyc_Absyn_Exp*
_temp1233; struct Cyc_Absyn_Vardecl* _temp1235; struct Cyc_List_List* _temp1237;
void* _temp1239; _LL1221: if(*(( int*) _temp1219) == Cyc_Absyn_Array_e){ _LL1230:
_temp1229=(( struct Cyc_Absyn_Array_e_struct*) _temp1219)->f1; goto _LL1222;}
else{ goto _LL1223;} _LL1223: if(*(( int*) _temp1219) == Cyc_Absyn_Comprehension_e){
_LL1236: _temp1235=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1219)->f1;
goto _LL1234; _LL1234: _temp1233=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1219)->f2; goto _LL1232; _LL1232: _temp1231=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1219)->f3; goto _LL1224;} else{ goto _LL1225;} _LL1225: if(*(( int*)
_temp1219) == Cyc_Absyn_AnonStruct_e){ _LL1240: _temp1239=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1219)->f1; goto _LL1238; _LL1238: _temp1237=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1219)->f2; goto _LL1226;} else{ goto _LL1227;} _LL1227: goto _LL1228;
_LL1222: _temp1181= Cyc_Toc_init_array( nv, lval, _temp1229, _temp1181); goto
_LL1220; _LL1224: _temp1181= Cyc_Toc_init_comprehension( nv, lval, _temp1235,
_temp1233, _temp1231, _temp1181, 0); goto _LL1220; _LL1226: _temp1181= Cyc_Toc_init_anon_struct(
nv, lval, _temp1239, _temp1237, _temp1181); goto _LL1220; _LL1228: Cyc_Toc_exp_to_c(
nv, _temp1191); if( Cyc_Toc_is_poly_field( struct_type, _temp1217)){ _temp1191=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1191, 0);} _temp1181= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1191, 0), 0), _temp1181, 0);
goto _LL1220; _LL1220:;} goto _LL1212;} _LL1216:( void) _throw((( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1241=(
unsigned char*)"array designator in struct"; struct _tagged_string _temp1242;
_temp1242.curr= _temp1241; _temp1242.base= _temp1241; _temp1242.last_plus_one=
_temp1241 + 27; _temp1242;}))); _LL1212:;}}}{ struct Cyc_Absyn_Exp* _temp1243=
Cyc_Toc_make_struct( nv, _temp1177, Cyc_Absyn_strctq( tdn), _temp1181, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp1243;}; _pop_region(&
_temp1185);}}}}} struct _tuple11{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp*
f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp*
el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2,
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp*
e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp1244=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp1244[ 0]=({
struct Cyc_Absyn_Increment_e_struct _temp1245; _temp1245.tag= Cyc_Absyn_Increment_e;
_temp1245.f1= e1; _temp1245.f2=( void*) incr; _temp1245;}); _temp1244;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp1246=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp1256; struct Cyc_Absyn_Exp*
_temp1258; void* _temp1260; struct _tagged_string* _temp1262; struct Cyc_Absyn_Exp*
_temp1264; _LL1248: if(*(( int*) _temp1246) == Cyc_Absyn_StmtExp_e){ _LL1257:
_temp1256=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1246)->f1; goto _LL1249;}
else{ goto _LL1250;} _LL1250: if(*(( int*) _temp1246) == Cyc_Absyn_Cast_e){
_LL1261: _temp1260=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1246)->f1;
goto _LL1259; _LL1259: _temp1258=(( struct Cyc_Absyn_Cast_e_struct*) _temp1246)->f2;
goto _LL1251;} else{ goto _LL1252;} _LL1252: if(*(( int*) _temp1246) == Cyc_Absyn_StructMember_e){
_LL1265: _temp1264=(( struct Cyc_Absyn_StructMember_e_struct*) _temp1246)->f1;
goto _LL1263; _LL1263: _temp1262=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1246)->f2; goto _LL1253;} else{ goto _LL1254;} _LL1254: goto _LL1255;
_LL1249:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1256, fs, f, f_env); goto _LL1247; _LL1251:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1258, fs, f, f_env); goto
_LL1247; _LL1253:( void*)( e1->r=( void*)(( void*) _temp1264->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1266=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1266->hd=( void*) _temp1262; _temp1266->tl= fs;
_temp1266;}), f, f_env); goto _LL1247; _LL1255: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
struct _tagged_string _temp1267= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1267.last_plus_one - _temp1267.curr, _temp1267.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1247;}
_LL1247:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1268=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1278; struct Cyc_Absyn_Stmt* _temp1280; struct Cyc_Absyn_Decl* _temp1282;
struct Cyc_Absyn_Stmt* _temp1284; struct Cyc_Absyn_Stmt* _temp1286; _LL1270: if((
unsigned int) _temp1268 > 1u?*(( int*) _temp1268) == Cyc_Absyn_Exp_s: 0){
_LL1279: _temp1278=(( struct Cyc_Absyn_Exp_s_struct*) _temp1268)->f1; goto
_LL1271;} else{ goto _LL1272;} _LL1272: if(( unsigned int) _temp1268 > 1u?*((
int*) _temp1268) == Cyc_Absyn_Decl_s: 0){ _LL1283: _temp1282=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1268)->f1; goto _LL1281; _LL1281: _temp1280=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1268)->f2; goto _LL1273;} else{ goto _LL1274;} _LL1274: if(( unsigned int)
_temp1268 > 1u?*(( int*) _temp1268) == Cyc_Absyn_Seq_s: 0){ _LL1287: _temp1286=((
struct Cyc_Absyn_Seq_s_struct*) _temp1268)->f1; goto _LL1285; _LL1285: _temp1284=((
struct Cyc_Absyn_Seq_s_struct*) _temp1268)->f2; goto _LL1275;} else{ goto
_LL1276;} _LL1276: goto _LL1277; _LL1271:(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1278, fs, f, f_env); goto
_LL1269; _LL1273:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1280, fs, f, f_env); goto _LL1269; _LL1275:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1284, fs, f, f_env);
goto _LL1269; _LL1277:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp1288= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1288.last_plus_one - _temp1288.curr,
_temp1288.curr);})); goto _LL1269; _LL1269:;} static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s); static struct Cyc_List_List*
Cyc_Toc_rmap_2c( struct _RegionHandle* r2, void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List* _temp1289=( struct
Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1289->hd=(
void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1289->tl=
0; _temp1289;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp1290=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1290->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1290->tl= 0;
_temp1290;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x))
Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1291=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1291->f1= 0; _temp1291->f2= e;
_temp1291;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){
void* _temp1292= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp1298;
_LL1294: if(( unsigned int) _temp1292 > 4u?*(( int*) _temp1292) == Cyc_Absyn_PointerType:
0){ _LL1299: _temp1298=(( struct Cyc_Absyn_PointerType_struct*) _temp1292)->f1;
goto _LL1295;} else{ goto _LL1296;} _LL1296: goto _LL1297; _LL1295: return
_temp1298; _LL1297:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp1300=( unsigned char*)"get_ptr_typ:  not a pointer!"; struct
_tagged_string _temp1301; _temp1301.curr= _temp1300; _temp1301.base= _temp1300;
_temp1301.last_plus_one= _temp1300 + 29; _temp1301;}))); _LL1293:;} struct
_tagged_ptr6{ struct _tuple0** curr; struct _tuple0** base; struct _tuple0**
last_plus_one; } ; struct _tagged_ptr7{ struct _tuple1** curr; struct _tuple1**
base; struct _tuple1** last_plus_one; } ; struct _tagged_ptr8{ struct Cyc_Absyn_Exp**
curr; struct Cyc_Absyn_Exp** base; struct Cyc_Absyn_Exp** last_plus_one; } ;
struct _tuple12{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){
void* _temp1302=( void*) e->r; goto _LL1303; _LL1303: if( e->topt == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1304= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1304.last_plus_one - _temp1304.curr, _temp1304.curr);})); return;}{ void*
old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void*
_temp1305= _temp1302; void* _temp1385; void* _temp1387; void* _temp1389; struct
_tuple0* _temp1391; struct _tuple0* _temp1393; struct Cyc_List_List* _temp1395;
void* _temp1397; void* _temp1399; struct Cyc_Absyn_Exp* _temp1401; struct Cyc_Absyn_Exp*
_temp1403; struct Cyc_Core_Opt* _temp1405; struct Cyc_Absyn_Exp* _temp1407;
struct Cyc_Absyn_Exp* _temp1409; struct Cyc_Absyn_Exp* _temp1411; struct Cyc_Absyn_Exp*
_temp1413; struct Cyc_Absyn_Exp* _temp1415; struct Cyc_Absyn_Exp* _temp1417;
struct Cyc_List_List* _temp1419; struct Cyc_Absyn_Exp* _temp1421; struct Cyc_Absyn_VarargCallInfo*
_temp1423; struct Cyc_List_List* _temp1425; struct Cyc_Absyn_Exp* _temp1427;
struct Cyc_Absyn_VarargCallInfo* _temp1429; struct Cyc_Absyn_VarargCallInfo
_temp1431; struct Cyc_Absyn_VarargInfo* _temp1432; struct Cyc_List_List*
_temp1434; int _temp1436; struct Cyc_List_List* _temp1438; struct Cyc_Absyn_Exp*
_temp1440; struct Cyc_Absyn_Exp* _temp1442; struct Cyc_Absyn_Exp* _temp1444;
struct Cyc_List_List* _temp1446; struct Cyc_Absyn_Exp* _temp1448; struct Cyc_Absyn_Exp*
_temp1450; void* _temp1452; void** _temp1454; struct Cyc_Absyn_Exp* _temp1455;
struct Cyc_Absyn_Exp* _temp1457; struct Cyc_Absyn_Exp* _temp1459; struct Cyc_Absyn_Exp*
_temp1461; void* _temp1463; struct _tagged_string* _temp1465; void* _temp1467;
struct Cyc_Absyn_Exp* _temp1469; struct _tagged_string* _temp1471; struct Cyc_Absyn_Exp*
_temp1473; struct _tagged_string* _temp1475; struct Cyc_Absyn_Exp* _temp1477;
struct Cyc_Absyn_Exp* _temp1479; struct Cyc_Absyn_Exp* _temp1481; struct Cyc_List_List*
_temp1483; struct Cyc_List_List* _temp1485; struct _tuple1* _temp1487; struct
Cyc_List_List* _temp1489; struct Cyc_Absyn_Exp* _temp1491; struct Cyc_Absyn_Exp*
_temp1493; struct Cyc_Absyn_Vardecl* _temp1495; struct Cyc_Absyn_Structdecl*
_temp1497; struct Cyc_List_List* _temp1499; struct Cyc_Core_Opt* _temp1501;
struct _tuple0* _temp1503; struct Cyc_List_List* _temp1505; void* _temp1507;
struct Cyc_Absyn_Tunionfield* _temp1509; struct Cyc_Absyn_Tuniondecl* _temp1511;
struct Cyc_List_List* _temp1513; struct Cyc_Core_Opt* _temp1515; struct Cyc_Core_Opt*
_temp1517; struct Cyc_Absyn_Tunionfield* _temp1519; struct Cyc_Absyn_Tuniondecl*
_temp1521; struct Cyc_List_List* _temp1523; struct Cyc_Core_Opt* _temp1525;
struct Cyc_Core_Opt* _temp1527; struct Cyc_Absyn_Enumfield* _temp1529; struct
Cyc_Absyn_Enumdecl* _temp1531; struct _tuple0* _temp1533; void* _temp1535;
struct Cyc_Absyn_Exp* _temp1537; struct Cyc_Absyn_Stmt* _temp1539; struct Cyc_List_List*
_temp1541; struct Cyc_Core_Opt* _temp1543; struct Cyc_Absyn_Fndecl* _temp1545;
struct Cyc_Absyn_Exp* _temp1547; _LL1307: if(*(( int*) _temp1305) == Cyc_Absyn_Const_e){
_LL1386: _temp1385=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1305)->f1;
if( _temp1385 ==( void*) Cyc_Absyn_Null_c){ goto _LL1308;} else{ goto _LL1309;}}
else{ goto _LL1309;} _LL1309: if(*(( int*) _temp1305) == Cyc_Absyn_Const_e){
_LL1388: _temp1387=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1305)->f1;
goto _LL1310;} else{ goto _LL1311;} _LL1311: if(*(( int*) _temp1305) == Cyc_Absyn_Var_e){
_LL1392: _temp1391=(( struct Cyc_Absyn_Var_e_struct*) _temp1305)->f1; goto
_LL1390; _LL1390: _temp1389=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1305)->f2;
goto _LL1312;} else{ goto _LL1313;} _LL1313: if(*(( int*) _temp1305) == Cyc_Absyn_UnknownId_e){
_LL1394: _temp1393=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1305)->f1; goto
_LL1314;} else{ goto _LL1315;} _LL1315: if(*(( int*) _temp1305) == Cyc_Absyn_Primop_e){
_LL1398: _temp1397=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1305)->f1;
goto _LL1396; _LL1396: _temp1395=(( struct Cyc_Absyn_Primop_e_struct*) _temp1305)->f2;
goto _LL1316;} else{ goto _LL1317;} _LL1317: if(*(( int*) _temp1305) == Cyc_Absyn_Increment_e){
_LL1402: _temp1401=(( struct Cyc_Absyn_Increment_e_struct*) _temp1305)->f1; goto
_LL1400; _LL1400: _temp1399=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1305)->f2; goto _LL1318;} else{ goto _LL1319;} _LL1319: if(*(( int*)
_temp1305) == Cyc_Absyn_AssignOp_e){ _LL1408: _temp1407=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1305)->f1; goto _LL1406; _LL1406: _temp1405=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1305)->f2; goto _LL1404; _LL1404: _temp1403=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1305)->f3; goto _LL1320;} else{ goto _LL1321;} _LL1321: if(*(( int*)
_temp1305) == Cyc_Absyn_Conditional_e){ _LL1414: _temp1413=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1305)->f1; goto _LL1412; _LL1412: _temp1411=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1305)->f2; goto _LL1410; _LL1410: _temp1409=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1305)->f3; goto _LL1322;} else{ goto _LL1323;} _LL1323: if(*(( int*)
_temp1305) == Cyc_Absyn_SeqExp_e){ _LL1418: _temp1417=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1305)->f1; goto _LL1416; _LL1416: _temp1415=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1305)->f2; goto _LL1324;} else{ goto _LL1325;} _LL1325: if(*(( int*)
_temp1305) == Cyc_Absyn_UnknownCall_e){ _LL1422: _temp1421=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1305)->f1; goto _LL1420; _LL1420: _temp1419=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1305)->f2; goto _LL1326;} else{ goto _LL1327;} _LL1327: if(*(( int*)
_temp1305) == Cyc_Absyn_FnCall_e){ _LL1428: _temp1427=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1305)->f1; goto _LL1426; _LL1426: _temp1425=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1305)->f2; goto _LL1424; _LL1424: _temp1423=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1305)->f3; if( _temp1423 == 0){ goto _LL1328;} else{ goto _LL1329;}} else{
goto _LL1329;} _LL1329: if(*(( int*) _temp1305) == Cyc_Absyn_FnCall_e){ _LL1441:
_temp1440=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1305)->f1; goto _LL1439;
_LL1439: _temp1438=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1305)->f2; goto
_LL1430; _LL1430: _temp1429=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1305)->f3;
if( _temp1429 == 0){ goto _LL1331;} else{ _temp1431=* _temp1429; _LL1437:
_temp1436= _temp1431.num_varargs; goto _LL1435; _LL1435: _temp1434= _temp1431.injectors;
goto _LL1433; _LL1433: _temp1432= _temp1431.vai; goto _LL1330;}} else{ goto
_LL1331;} _LL1331: if(*(( int*) _temp1305) == Cyc_Absyn_Throw_e){ _LL1443:
_temp1442=(( struct Cyc_Absyn_Throw_e_struct*) _temp1305)->f1; goto _LL1332;}
else{ goto _LL1333;} _LL1333: if(*(( int*) _temp1305) == Cyc_Absyn_NoInstantiate_e){
_LL1445: _temp1444=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1305)->f1;
goto _LL1334;} else{ goto _LL1335;} _LL1335: if(*(( int*) _temp1305) == Cyc_Absyn_Instantiate_e){
_LL1449: _temp1448=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1305)->f1;
goto _LL1447; _LL1447: _temp1446=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1305)->f2; goto _LL1336;} else{ goto _LL1337;} _LL1337: if(*(( int*)
_temp1305) == Cyc_Absyn_Cast_e){ _LL1453: _temp1452=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1305)->f1; _temp1454=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1305)->f1); goto _LL1451; _LL1451: _temp1450=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1305)->f2; goto _LL1338;} else{ goto _LL1339;} _LL1339: if(*(( int*)
_temp1305) == Cyc_Absyn_Address_e){ _LL1456: _temp1455=(( struct Cyc_Absyn_Address_e_struct*)
_temp1305)->f1; goto _LL1340;} else{ goto _LL1341;} _LL1341: if(*(( int*)
_temp1305) == Cyc_Absyn_New_e){ _LL1460: _temp1459=(( struct Cyc_Absyn_New_e_struct*)
_temp1305)->f1; goto _LL1458; _LL1458: _temp1457=(( struct Cyc_Absyn_New_e_struct*)
_temp1305)->f2; goto _LL1342;} else{ goto _LL1343;} _LL1343: if(*(( int*)
_temp1305) == Cyc_Absyn_Sizeofexp_e){ _LL1462: _temp1461=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1305)->f1; goto _LL1344;} else{ goto _LL1345;} _LL1345: if(*(( int*)
_temp1305) == Cyc_Absyn_Sizeoftyp_e){ _LL1464: _temp1463=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1305)->f1; goto _LL1346;} else{ goto _LL1347;} _LL1347: if(*(( int*)
_temp1305) == Cyc_Absyn_Offsetof_e){ _LL1468: _temp1467=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1305)->f1; goto _LL1466; _LL1466: _temp1465=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1305)->f2; goto _LL1348;} else{ goto _LL1349;} _LL1349: if(*(( int*)
_temp1305) == Cyc_Absyn_Deref_e){ _LL1470: _temp1469=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1305)->f1; goto _LL1350;} else{ goto _LL1351;} _LL1351: if(*(( int*)
_temp1305) == Cyc_Absyn_StructMember_e){ _LL1474: _temp1473=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1305)->f1; goto _LL1472; _LL1472: _temp1471=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1305)->f2; goto _LL1352;} else{ goto _LL1353;} _LL1353: if(*(( int*)
_temp1305) == Cyc_Absyn_StructArrow_e){ _LL1478: _temp1477=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1305)->f1; goto _LL1476; _LL1476: _temp1475=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1305)->f2; goto _LL1354;} else{ goto _LL1355;} _LL1355: if(*(( int*)
_temp1305) == Cyc_Absyn_Subscript_e){ _LL1482: _temp1481=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1305)->f1; goto _LL1480; _LL1480: _temp1479=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1305)->f2; goto _LL1356;} else{ goto _LL1357;} _LL1357: if(*(( int*)
_temp1305) == Cyc_Absyn_Tuple_e){ _LL1484: _temp1483=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1305)->f1; goto _LL1358;} else{ goto _LL1359;} _LL1359: if(*(( int*)
_temp1305) == Cyc_Absyn_CompoundLit_e){ _LL1488: _temp1487=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1305)->f1; goto _LL1486; _LL1486: _temp1485=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1305)->f2; goto _LL1360;} else{ goto _LL1361;} _LL1361: if(*(( int*)
_temp1305) == Cyc_Absyn_Array_e){ _LL1490: _temp1489=(( struct Cyc_Absyn_Array_e_struct*)
_temp1305)->f1; goto _LL1362;} else{ goto _LL1363;} _LL1363: if(*(( int*)
_temp1305) == Cyc_Absyn_Comprehension_e){ _LL1496: _temp1495=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1305)->f1; goto _LL1494; _LL1494: _temp1493=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1305)->f2; goto _LL1492; _LL1492: _temp1491=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1305)->f3; goto _LL1364;} else{ goto _LL1365;} _LL1365: if(*(( int*)
_temp1305) == Cyc_Absyn_Struct_e){ _LL1504: _temp1503=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1305)->f1; goto _LL1502; _LL1502: _temp1501=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1305)->f2; goto _LL1500; _LL1500: _temp1499=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1305)->f3; goto _LL1498; _LL1498: _temp1497=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1305)->f4; goto _LL1366;} else{ goto _LL1367;} _LL1367: if(*(( int*)
_temp1305) == Cyc_Absyn_AnonStruct_e){ _LL1508: _temp1507=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1305)->f1; goto _LL1506; _LL1506: _temp1505=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1305)->f2; goto _LL1368;} else{ goto _LL1369;} _LL1369: if(*(( int*)
_temp1305) == Cyc_Absyn_Tunion_e){ _LL1518: _temp1517=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1305)->f1; goto _LL1516; _LL1516: _temp1515=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1305)->f2; goto _LL1514; _LL1514: _temp1513=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1305)->f3; if( _temp1513 == 0){ goto _LL1512;} else{ goto _LL1371;} _LL1512:
_temp1511=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1305)->f4; goto _LL1510;
_LL1510: _temp1509=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1305)->f5; goto
_LL1370;} else{ goto _LL1371;} _LL1371: if(*(( int*) _temp1305) == Cyc_Absyn_Tunion_e){
_LL1528: _temp1527=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1305)->f1; goto
_LL1526; _LL1526: _temp1525=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1305)->f2;
goto _LL1524; _LL1524: _temp1523=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1305)->f3;
goto _LL1522; _LL1522: _temp1521=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1305)->f4;
goto _LL1520; _LL1520: _temp1519=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1305)->f5;
goto _LL1372;} else{ goto _LL1373;} _LL1373: if(*(( int*) _temp1305) == Cyc_Absyn_Enum_e){
_LL1534: _temp1533=(( struct Cyc_Absyn_Enum_e_struct*) _temp1305)->f1; goto
_LL1532; _LL1532: _temp1531=(( struct Cyc_Absyn_Enum_e_struct*) _temp1305)->f2;
goto _LL1530; _LL1530: _temp1529=(( struct Cyc_Absyn_Enum_e_struct*) _temp1305)->f3;
goto _LL1374;} else{ goto _LL1375;} _LL1375: if(*(( int*) _temp1305) == Cyc_Absyn_Malloc_e){
_LL1538: _temp1537=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1305)->f1; goto
_LL1536; _LL1536: _temp1535=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1305)->f2; goto _LL1376;} else{ goto _LL1377;} _LL1377: if(*(( int*)
_temp1305) == Cyc_Absyn_StmtExp_e){ _LL1540: _temp1539=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1305)->f1; goto _LL1378;} else{ goto _LL1379;} _LL1379: if(*(( int*)
_temp1305) == Cyc_Absyn_UnresolvedMem_e){ _LL1544: _temp1543=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1305)->f1; goto _LL1542; _LL1542: _temp1541=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1305)->f2; goto _LL1380;} else{ goto _LL1381;} _LL1381: if(*(( int*)
_temp1305) == Cyc_Absyn_Codegen_e){ _LL1546: _temp1545=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1305)->f1; goto _LL1382;} else{ goto _LL1383;} _LL1383: if(*(( int*)
_temp1305) == Cyc_Absyn_Fill_e){ _LL1548: _temp1547=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1305)->f1; goto _LL1384;} else{ goto _LL1306;} _LL1308:{ void* _temp1549=
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
_LL1550;}{ void* _temp1567=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1557); struct Cyc_Absyn_Exp* _temp1573; _LL1569: if(
_temp1567 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1570;} else{ goto _LL1571;}
_LL1571: if(( unsigned int) _temp1567 > 1u?*(( int*) _temp1567) == Cyc_Absyn_Upper_b:
0){ _LL1574: _temp1573=(( struct Cyc_Absyn_Upper_b_struct*) _temp1567)->f1; goto
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
_LL1550:;} goto _LL1306; _LL1310: goto _LL1306; _LL1312:{ struct _handler_cons
_temp1595; _push_handler(& _temp1595);{ int _temp1597= 0; if( setjmp( _temp1595.handler)){
_temp1597= 1;} if( ! _temp1597){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1391))->r));; _pop_handler();} else{ void* _temp1596=( void*) _exn_thrown;
void* _temp1599= _temp1596; _LL1601: if( _temp1599 == Cyc_Dict_Absent){ goto
_LL1602;} else{ goto _LL1603;} _LL1603: goto _LL1604; _LL1602:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1605= Cyc_Absynpp_qvar2string( _temp1391); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1605.last_plus_one - _temp1605.curr, _temp1605.curr);})); return; _LL1604:(
void) _throw( _temp1599); _LL1600:;}}} goto _LL1306; _LL1314:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1606=(
unsigned char*)"unknownid"; struct _tagged_string _temp1607; _temp1607.curr=
_temp1606; _temp1607.base= _temp1606; _temp1607.last_plus_one= _temp1606 + 10;
_temp1607;})); goto _LL1306; _LL1316: { struct Cyc_List_List* _temp1608=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp1395); goto _LL1609; _LL1609: {
struct Cyc_List_List* _temp1610=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1395); goto _LL1611; _LL1611:(( void(*)( void(* f)( struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)(
Cyc_Toc_exp_to_c, nv, _temp1395);{ void* _temp1612= _temp1397; _LL1614: if(
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
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1395))->hd;{
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
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1397, 0, _temp1395,
_temp1610))->r)); goto _LL1613; _LL1619:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1397, 0, _temp1395, _temp1610))->r)); goto _LL1613; _LL1621: if( _temp1395
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char*
_temp1674=( unsigned char*)"fprintf without arguments"; struct _tagged_string
_temp1675; _temp1675.curr= _temp1674; _temp1675.base= _temp1674; _temp1675.last_plus_one=
_temp1674 + 26; _temp1675;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1397,({ struct Cyc_Core_Opt* _temp1676=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1676->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1395))->hd); _temp1676;}),(( struct Cyc_List_List*)
_check_null( _temp1395))->tl,(( struct Cyc_List_List*) _check_null( _temp1610))->tl))->r));
goto _LL1613; _LL1623: if( _temp1395 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1677=( unsigned char*)"sscanf without arguments";
struct _tagged_string _temp1678; _temp1678.curr= _temp1677; _temp1678.base=
_temp1677; _temp1678.last_plus_one= _temp1677 + 25; _temp1678;}));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1395))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1395))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1395))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1613; _LL1625: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1395))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1395))->tl))->hd; struct _tuple0* _temp1679= Cyc_Toc_temp_var(); goto
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
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1395))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1395))->tl))->hd; void*
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
_check_null( _temp1395))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1395))->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp1608))->hd; void* t2=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1608))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1613;} _LL1641: goto
_LL1613; _LL1613:;} goto _LL1306;}} _LL1318: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1401->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1401); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1399 ==(
void*) Cyc_Absyn_PreInc? 1: _temp1399 ==( void*) Cyc_Absyn_PreDec){ struct
_tuple0* _temp1701= Cyc_Toc_temp_var(); goto _LL1702; _LL1702: { void* _temp1703=
Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1704; _LL1704: { struct Cyc_Absyn_Stmt*
_temp1705= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1701,
0), 0), 0); goto _LL1706; _LL1706: { struct Cyc_Absyn_Exp* _temp1707= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1701, 0), Cyc_Toc_curr_sp, 0),
_temp1399, 0); goto _LL1708; _LL1708: _temp1705= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1707, 0), _temp1705, 0); _temp1705= Cyc_Absyn_declare_stmt( _temp1701,
_temp1703,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1401, 0),
_temp1705, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1705, 0))->r));}}}}
else{ struct _tuple0* _temp1709= Cyc_Toc_temp_var(); goto _LL1710; _LL1710: {
struct _tuple0* _temp1711= Cyc_Toc_temp_var(); goto _LL1712; _LL1712: { void*
_temp1713= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1714; _LL1714: { void*
_temp1715= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1716; _LL1716: { struct Cyc_Absyn_Stmt*
_temp1717= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1711, 0), 0); goto
_LL1718; _LL1718: { struct Cyc_Absyn_Exp* _temp1719= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1709, 0), Cyc_Toc_curr_sp, 0),
_temp1399, 0); goto _LL1720; _LL1720: _temp1717= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1719, 0), _temp1717, 0); _temp1717= Cyc_Absyn_declare_stmt( _temp1711,
_temp1713,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1709, 0), 0), _temp1717, 0); _temp1717= Cyc_Absyn_declare_stmt( _temp1709,
_temp1715,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1401, 0),
_temp1717, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1717, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1401)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1401, 0, Cyc_Toc_incr_lvalue,
_temp1399);( void*)( e->r=( void*)(( void*) _temp1401->r));}} goto _LL1306;}
_LL1320: { int e1_poly= Cyc_Toc_is_poly_project( _temp1407); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1407->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1403->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1407); Cyc_Toc_exp_to_c( nv, _temp1403); if(
_temp1405 != 0?( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1405))->v ==(
void*) Cyc_Absyn_Plus? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1405))->v
==( void*) Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){(
void*)( _temp1407->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1407->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1403->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1403->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1403->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1403->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1407)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1407, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1721=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1721[ 0]=({ struct _tuple11 _temp1722; _temp1722.f1=
_temp1405; _temp1722.f2= _temp1403; _temp1722;}); _temp1721;}));( void*)( e->r=(
void*)(( void*) _temp1407->r));} goto _LL1306;} _LL1322: Cyc_Toc_exp_to_c( nv,
_temp1413); Cyc_Toc_exp_to_c( nv, _temp1411); Cyc_Toc_exp_to_c( nv, _temp1409);
goto _LL1306; _LL1324: Cyc_Toc_exp_to_c( nv, _temp1417); Cyc_Toc_exp_to_c( nv,
_temp1415); goto _LL1306; _LL1326: _temp1427= _temp1421; _temp1425= _temp1419;
goto _LL1328; _LL1328: Cyc_Toc_exp_to_c( nv, _temp1427);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1425); goto _LL1306; _LL1330:{
struct _RegionHandle _temp1723= _new_region(); struct _RegionHandle* r=&
_temp1723; _push_region( r);{ void* _temp1724=( void*) _temp1432->type; goto
_LL1725; _LL1725: { void* _temp1726= Cyc_Toc_typ_to_c( _temp1724); goto _LL1727;
_LL1727: { void* _temp1728= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1440->topt))->v); goto _LL1729; _LL1729: { struct Cyc_Absyn_Exp*
_temp1730= Cyc_Absyn_uint_exp(( unsigned int) _temp1436, 0); goto _LL1731;
_LL1731: { void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1800=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1800[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1801; _temp1801.tag= Cyc_Absyn_ArrayType;
_temp1801.f1=( void*) _temp1724; _temp1801.f2= Cyc_Toc_mt_tq; _temp1801.f3=(
struct Cyc_Absyn_Exp*) _temp1730; _temp1801;}); _temp1800;})); struct _tuple0*
_temp1732= Cyc_Toc_temp_var(); goto _LL1733; _LL1733: { struct _tuple0*
_temp1734= Cyc_Toc_temp_var(); goto _LL1735; _LL1735: { struct Cyc_Absyn_Exp*
_temp1736= Cyc_Absyn_var_exp( _temp1734, 0); goto _LL1737; _LL1737: { struct
_tuple0* _temp1738= Cyc_Toc_temp_var(); goto _LL1739; _LL1739: { int _temp1740=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1438); goto _LL1741;
_LL1741: { int _temp1742= _temp1740 - _temp1436; goto _LL1743; _LL1743: { struct
_tagged_ptr6 vs=({ unsigned int _temp1796=( unsigned int) _temp1436; struct
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
_temp1724,( void*) _temp1432->rgn, Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp(
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
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1438);
goto _LL1765; _LL1765: { struct Cyc_List_List* _temp1766=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1434);
goto _LL1767; _LL1767:{ int i= _temp1436 - 1; for( 0; i >= 0;( i --, _temp1764=((
struct Cyc_List_List*) _check_null( _temp1764))->tl)){ struct Cyc_Absyn_Exp* arg=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1764))->hd;
void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c(
nv, arg); if( _temp1432->inject){ void* _temp1770; struct Cyc_Position_Segment*
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
Cyc_Toc_exp_to_c( nv, _temp1440); _temp1745= Cyc_Absyn_declare_stmt( _temp1732,
_temp1728,( struct Cyc_Absyn_Exp*) _temp1440, _temp1745, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1745, 0))->r));}}}}}}}}}}}}}}}}}};
_pop_region(& _temp1723);} goto _LL1306; _LL1332: Cyc_Toc_exp_to_c( nv,
_temp1442);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1442), 0))->r)); goto _LL1306; _LL1334: Cyc_Toc_exp_to_c(
nv, _temp1444); goto _LL1306; _LL1336: Cyc_Toc_exp_to_c( nv, _temp1448);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
for( 0; _temp1446 != 0; _temp1446=(( struct Cyc_List_List*) _check_null(
_temp1446))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*)
_check_null( _temp1446))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*)
Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1448, 0))->r)); break;}} goto _LL1306;} _LL1338: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1450->topt))->v; void* new_typ=*
_temp1454;* _temp1454= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1450);{ int noconv= 0;{ void* _temp1802= Cyc_Toc_conversion( old_t2,
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
Cyc_Toc_null_check_conv( nv->toplevel, _temp1814, _temp1450))->r)); goto _LL1803;
_LL1809:( void*)( e->r=( void*)(( void*)( Cyc_Toc_tag_ptr_conv( nv->toplevel,
_temp1818, _temp1816, _temp1450))->r)); goto _LL1803; _LL1811:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_untag_ptr_conv( nv->toplevel, _temp1824, _temp1822,
_temp1820, _temp1450))->r)); goto _LL1803; _LL1813:( void*)( _temp1450->r=( void*)((
void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp1450->r,
_temp1450->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1803; _LL1803:;} goto
_LL1306;}} _LL1340:{ void* _temp1828=( void*) _temp1455->r; struct Cyc_Absyn_Structdecl*
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
Cyc_Position_string_of_segment( _temp1455->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1846.last_plus_one - _temp1846.curr, _temp1846.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1455->topt))->v, 1, 0, _temp1838, _temp1842))->r)); goto
_LL1829; _LL1833: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ struct _tagged_string _temp1847=( struct _tagged_string)
Cyc_Position_string_of_segment( _temp1455->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1847.last_plus_one - _temp1847.curr, _temp1847.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1844))->r)); goto _LL1829;
_LL1835: Cyc_Toc_exp_to_c( nv, _temp1455); if( ! Cyc_Absyn_is_lvalue( _temp1455)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1455,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1455, 0))->r));} goto _LL1829; _LL1829:;} goto _LL1306; _LL1342: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
struct _tagged_string _temp1848=( struct _tagged_string) Cyc_Position_string_of_segment(
_temp1457->loc); xprintf("%.*s: new at top-level", _temp1848.last_plus_one -
_temp1848.curr, _temp1848.curr);}));}{ void* _temp1849=( void*) _temp1457->r;
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
_LL1908; _LL1908: { struct Cyc_Absyn_Exp* e1; if( _temp1459 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1907);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1459); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
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
_LL1914: is_tagged_ptr=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1919) ==( void*) Cyc_Absyn_Unknown_b; goto _LL1912;
_LL1916:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1929=( unsigned char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1930; _temp1930.curr= _temp1929; _temp1930.base=
_temp1929; _temp1930.last_plus_one= _temp1929 + 42; _temp1930;})); goto _LL1912;
_LL1912:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1863->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1865);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1867, Cyc_Absyn_var_exp(
max, 0), _temp1863, Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1931=
_new_region(); struct _RegionHandle* r=& _temp1931; _push_region( r);{ struct
Cyc_List_List* decls=({ struct Cyc_List_List* _temp1961=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1961->hd=( void*)({
struct _tuple10* _temp1962=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1962->f1= max; _temp1962->f2= Cyc_Absyn_uint_t; _temp1962->f3=(
struct Cyc_Absyn_Exp*) _temp1865; _temp1962;}); _temp1961->tl= 0; _temp1961;});
struct Cyc_Absyn_Exp* ai; if( _temp1459 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1459); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1932=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1932->hd=( void*)({ struct _tuple10*
_temp1933=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1933->f1= a; _temp1933->f2= ptr_typ; _temp1933->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1933;}); _temp1932->tl= decls; _temp1932;}); if( is_tagged_ptr){
struct _tuple0* _temp1934= Cyc_Toc_temp_var(); goto _LL1935; _LL1935: { void*
_temp1936= Cyc_Toc_typ_to_c( old_typ); goto _LL1937; _LL1937: { struct Cyc_Absyn_Exp*
_temp1947= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(* _temp1938)(
struct _tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4)) Cyc_List_list;
struct _tuple4* _temp1941=({ struct _tuple4* _temp1946=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1946->f1= 0; _temp1946->f2= Cyc_Absyn_var_exp(
a, 0); _temp1946;}); struct _tuple4* _temp1942=({ struct _tuple4* _temp1945=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1945->f1= 0;
_temp1945->f2= Cyc_Absyn_var_exp( a, 0); _temp1945;}); struct _tuple4* _temp1943=({
struct _tuple4* _temp1944=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1944->f1= 0; _temp1944->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1944;}); struct _tuple4* _temp1939[ 3u]={
_temp1941, _temp1942, _temp1943}; struct _tagged_ptr4 _temp1940={ _temp1939,
_temp1939, _temp1939 + 3u}; _temp1938( _temp1940);}), 0); goto _LL1948; _LL1948:
decls=({ struct Cyc_List_List* _temp1949=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1949->hd=( void*)({ struct _tuple10*
_temp1950=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1950->f1= _temp1934; _temp1950->f2= _temp1936; _temp1950->f3=( struct Cyc_Absyn_Exp*)
_temp1947; _temp1950;}); _temp1949->tl= decls; _temp1949;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1934, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
_temp1951= decls; goto _LL1952; _LL1952: for( 0; _temp1951 != 0; _temp1951=((
struct Cyc_List_List*) _check_null( _temp1951))->tl){ struct Cyc_Absyn_Exp*
_temp1955; void* _temp1957; struct _tuple0* _temp1959; struct _tuple10 _temp1953=*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1951))->hd);
_LL1960: _temp1959= _temp1953.f1; goto _LL1958; _LL1958: _temp1957= _temp1953.f2;
goto _LL1956; _LL1956: _temp1955= _temp1953.f3; goto _LL1954; _LL1954: s= Cyc_Absyn_declare_stmt(
_temp1959, _temp1957, _temp1955, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region(& _temp1931);} goto _LL1850;}}} _LL1856:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1457->topt))->v, 1, _temp1459, _temp1871, _temp1875))->r));
goto _LL1850; _LL1858:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv,
1, _temp1459, _temp1877))->r)); goto _LL1850; _LL1860: { void* _temp1963=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1457->topt))->v; goto _LL1964; _LL1964: {
struct _tuple0* _temp1965= Cyc_Toc_temp_var(); goto _LL1966; _LL1966: { struct
Cyc_Absyn_Stmt* _temp1967= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1965, 0),
0); goto _LL1968; _LL1968: { struct Cyc_Absyn_Exp* _temp1969= Cyc_Absyn_signed_int_exp(
0, 0); goto _LL1970; _LL1970: Cyc_Toc_exp_to_c( nv, _temp1457); _temp1967= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1965, 0),
_temp1969, 0), _temp1457, 0), _temp1967, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
_temp1963); void* _temp1971= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto
_LL1972; _LL1972: { struct Cyc_Absyn_Exp* _temp1973= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0); goto _LL1974; _LL1974: { struct Cyc_Absyn_Exp* e1; if( _temp1459 ==
0){ e1= Cyc_Toc_malloc_exp( _temp1963, _temp1973);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp1459); Cyc_Toc_exp_to_c( nv, r); e1=
Cyc_Toc_rmalloc_exp( r, _temp1973);}{ struct Cyc_Absyn_Exp* _temp1975= Cyc_Absyn_cast_exp(
_temp1971, e1, 0); goto _LL1976; _LL1976:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1965, _temp1971,( struct Cyc_Absyn_Exp*) _temp1975,
_temp1967, 0), 0))->r)); goto _LL1850;}}}}}}}} _LL1850:;} goto _LL1306; _LL1344:
Cyc_Toc_exp_to_c( nv, _temp1461); goto _LL1306; _LL1346:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp1977=( struct Cyc_Absyn_Sizeoftyp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct)); _temp1977[ 0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _temp1978; _temp1978.tag= Cyc_Absyn_Sizeoftyp_e;
_temp1978.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1463); _temp1978;});
_temp1977;}))); goto _LL1306; _LL1348:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Offsetof_e_struct* _temp1979=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1979[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1980; _temp1980.tag= Cyc_Absyn_Offsetof_e;
_temp1980.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1467); _temp1980.f2=
_temp1465; _temp1980;}); _temp1979;}))); goto _LL1306; _LL1350: { void* e1_typ=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1469->topt))->v);
Cyc_Toc_exp_to_c( nv, _temp1469);{ void* _temp1981= e1_typ; struct Cyc_Absyn_PtrInfo
_temp1987; struct Cyc_Absyn_Conref* _temp1989; struct Cyc_Absyn_Tqual _temp1991;
struct Cyc_Absyn_Conref* _temp1993; void* _temp1995; void* _temp1997; _LL1983:
if(( unsigned int) _temp1981 > 4u?*(( int*) _temp1981) == Cyc_Absyn_PointerType:
0){ _LL1988: _temp1987=(( struct Cyc_Absyn_PointerType_struct*) _temp1981)->f1;
_LL1998: _temp1997=( void*) _temp1987.elt_typ; goto _LL1996; _LL1996: _temp1995=(
void*) _temp1987.rgn_typ; goto _LL1994; _LL1994: _temp1993= _temp1987.nullable;
goto _LL1992; _LL1992: _temp1991= _temp1987.tq; goto _LL1990; _LL1990: _temp1989=
_temp1987.bounds; goto _LL1984;} else{ goto _LL1985;} _LL1985: goto _LL1986;
_LL1984:{ void* _temp1999=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1989); struct Cyc_Absyn_Exp* _temp2005; _LL2001: if(
_temp1999 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2002;} else{ goto _LL2003;}
_LL2003: if(( unsigned int) _temp1999 > 1u?*(( int*) _temp1999) == Cyc_Absyn_Upper_b:
0){ _LL2006: _temp2005=(( struct Cyc_Absyn_Upper_b_struct*) _temp1999)->f1; goto
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
struct Cyc_Absyn_Exp*) _temp1469, s1, 0), 0))->r)); goto _LL2000;}} _LL2004: if(
Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1469->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp2009=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2009->hd=(
void*) _temp1469; _temp2009->tl= 0; _temp2009;}), 0), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, e1_typ,(
struct Cyc_Absyn_Exp*) _temp1469, s3, 0), 0))->r));}} goto _LL2000; _LL2000:;}
goto _LL1982; _LL1986:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2010=( unsigned char*)"exp_to_c: Deref: non-pointer"; struct
_tagged_string _temp2011; _temp2011.curr= _temp2010; _temp2011.base= _temp2010;
_temp2011.last_plus_one= _temp2010 + 29; _temp2011;})); goto _LL1982; _LL1982:;}
goto _LL1306;} _LL1352: Cyc_Toc_exp_to_c( nv, _temp1473); if( Cyc_Toc_is_poly_project(
e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1306; _LL1354: { void* e1typ= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1477->topt))->v); Cyc_Toc_exp_to_c(
nv, _temp1477);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp2014; struct Cyc_Absyn_Tqual _temp2016; struct Cyc_Absyn_Conref* _temp2018;
void* _temp2020; void* _temp2022; struct Cyc_Absyn_PtrInfo _temp2012= Cyc_Toc_get_ptr_type(
e1typ); _LL2023: _temp2022=( void*) _temp2012.elt_typ; goto _LL2021; _LL2021:
_temp2020=( void*) _temp2012.rgn_typ; goto _LL2019; _LL2019: _temp2018=
_temp2012.nullable; goto _LL2017; _LL2017: _temp2016= _temp2012.tq; goto _LL2015;
_LL2015: _temp2014= _temp2012.bounds; goto _LL2013; _LL2013:{ void* _temp2024=((
void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp2014); struct Cyc_Absyn_Exp* _temp2030; _LL2026: if(( unsigned int)
_temp2024 > 1u?*(( int*) _temp2024) == Cyc_Absyn_Upper_b: 0){ _LL2031: _temp2030=((
struct Cyc_Absyn_Upper_b_struct*) _temp2024)->f1; goto _LL2027;} else{ goto
_LL2028;} _LL2028: if( _temp2024 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2029;}
else{ goto _LL2025;} _LL2027: if( Cyc_Evexp_eval_const_uint_exp( _temp2030) < 1){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2032=(
unsigned char*)"exp_to_c:  StructArrow_e on pointer of size 0"; struct
_tagged_string _temp2033; _temp2033.curr= _temp2032; _temp2033.base= _temp2032;
_temp2033.last_plus_one= _temp2032 + 46; _temp2033;}));}{ int possibly_null=((
int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp2018);
if( Cyc_Toc_is_nullable(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1477->topt))->v)){
if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1477->topt))->v), Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp2034=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2034->hd=( void*) _temp1477; _temp2034->tl=
0; _temp2034;}), 0), 0), _temp1475, 0))->r));} else{ struct _tuple0* x= Cyc_Toc_temp_var();
void* tx= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1477->topt))->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0),
_temp1475, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1477, s3, 0), 0))->r));}} goto _LL2025;}
_LL2029: { void* ta1= Cyc_Toc_typ_to_c_array( _temp2022); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp2016); if( Cyc_Toc_use_runtimecheck_funs){ struct Cyc_List_List*
_temp2044=({ struct Cyc_List_List*(* _temp2035)( struct _tagged_ptr7)=( struct
Cyc_List_List*(*)( struct _tagged_ptr7)) Cyc_List_list; struct _tuple1*
_temp2038=({ struct _tuple1* _temp2043=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2043->f1= 0; _temp2043->f2= Cyc_Toc_mt_tq; _temp2043->f3=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1477->topt))->v);
_temp2043;}); struct _tuple1* _temp2039=({ struct _tuple1* _temp2042=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2042->f1= 0; _temp2042->f2=
Cyc_Toc_mt_tq; _temp2042->f3= Cyc_Absyn_uint_t; _temp2042;}); struct _tuple1*
_temp2040=({ struct _tuple1* _temp2041=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2041->f1= 0; _temp2041->f2= Cyc_Toc_mt_tq; _temp2041->f3=
Cyc_Absyn_uint_t; _temp2041;}); struct _tuple1* _temp2036[ 3u]={ _temp2038,
_temp2039, _temp2040}; struct _tagged_ptr7 _temp2037={ _temp2036, _temp2036,
_temp2036 + 3u}; _temp2035( _temp2037);}); goto _LL2045; _LL2045: { void*
_temp2049= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2046=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2046[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2047; _temp2047.tag= Cyc_Absyn_FnType;
_temp2047.f1=({ struct Cyc_Absyn_FnInfo _temp2048; _temp2048.tvars= 0; _temp2048.effect=
0; _temp2048.ret_typ=( void*) ta2; _temp2048.args= _temp2044; _temp2048.c_varargs=
0; _temp2048.cyc_varargs= 0; _temp2048.rgn_po= 0; _temp2048.attributes= 0;
_temp2048;}); _temp2047;}); _temp2046;}), Cyc_Toc_mt_tq); goto _LL2050; _LL2050: {
struct Cyc_Absyn_Exp* fnexp= Cyc_Absyn_cast_exp( _temp2049, Cyc_Absyn_var_exp(
Cyc_Toc__check_unknown_subscript_qv, 0), 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_uint_exp(
0, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_fncall_exp(
fnexp,({ struct Cyc_List_List*(* _temp2051)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)(
struct _tagged_ptr8)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2054= _temp1477;
struct Cyc_Absyn_Exp* _temp2055= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2056= e2; struct Cyc_Absyn_Exp* _temp2052[ 3u]={ _temp2054, _temp2055,
_temp2056}; struct _tagged_ptr8 _temp2053={ _temp2052, _temp2052, _temp2052 + 3u};
_temp2051( _temp2053);}), 0), _temp1475, 0))->r));}}} else{ struct _tuple0* x=
Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2016);
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
struct Cyc_Absyn_Exp*) _temp1477, s1, 0), 0))->r));} goto _LL2025;} _LL2025:;}
if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1306;}} _LL1356: { void* _temp2057= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1481->topt))->v); goto _LL2058;
_LL2058: Cyc_Toc_exp_to_c( nv, _temp1481); Cyc_Toc_exp_to_c( nv, _temp1479);{
void* _temp2059= _temp2057; struct Cyc_List_List* _temp2067; struct Cyc_Absyn_PtrInfo
_temp2069; struct Cyc_Absyn_Conref* _temp2071; struct Cyc_Absyn_Tqual _temp2073;
struct Cyc_Absyn_Conref* _temp2075; void* _temp2077; void* _temp2079; _LL2061:
if(( unsigned int) _temp2059 > 4u?*(( int*) _temp2059) == Cyc_Absyn_TupleType: 0){
_LL2068: _temp2067=(( struct Cyc_Absyn_TupleType_struct*) _temp2059)->f1; goto
_LL2062;} else{ goto _LL2063;} _LL2063: if(( unsigned int) _temp2059 > 4u?*((
int*) _temp2059) == Cyc_Absyn_PointerType: 0){ _LL2070: _temp2069=(( struct Cyc_Absyn_PointerType_struct*)
_temp2059)->f1; _LL2080: _temp2079=( void*) _temp2069.elt_typ; goto _LL2078;
_LL2078: _temp2077=( void*) _temp2069.rgn_typ; goto _LL2076; _LL2076: _temp2075=
_temp2069.nullable; goto _LL2074; _LL2074: _temp2073= _temp2069.tq; goto _LL2072;
_LL2072: _temp2071= _temp2069.bounds; goto _LL2064;} else{ goto _LL2065;}
_LL2065: goto _LL2066; _LL2062: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1479) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1481, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL2060;} _LL2064:{ void*
_temp2081=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp2071); struct Cyc_Absyn_Exp* _temp2087; _LL2083: if((
unsigned int) _temp2081 > 1u?*(( int*) _temp2081) == Cyc_Absyn_Upper_b: 0){
_LL2088: _temp2087=(( struct Cyc_Absyn_Upper_b_struct*) _temp2081)->f1; goto
_LL2084;} else{ goto _LL2085;} _LL2085: if( _temp2081 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL2086;} else{ goto _LL2082;} _LL2084: _temp2087= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp2087), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp2075); void* ta1= Cyc_Toc_typ_to_c( _temp2079);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2073); if( Cyc_Toc_use_runtimecheck_funs){
if( possibly_null){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp(
ta2, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv,
0),({ struct Cyc_List_List*(* _temp2089)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)(
struct _tagged_ptr8)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2092= _temp1481;
struct Cyc_Absyn_Exp* _temp2093= _temp2087; struct Cyc_Absyn_Exp* _temp2094= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp2095= _temp1479; struct Cyc_Absyn_Exp*
_temp2090[ 4u]={ _temp2092, _temp2093, _temp2094, _temp2095}; struct
_tagged_ptr8 _temp2091={ _temp2090, _temp2090, _temp2090 + 4u}; _temp2089(
_temp2091);}), 0), 0), 0))->r));} else{( void*)( _temp1479->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List*(* _temp2096)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)(
struct _tagged_ptr8)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2099= _temp2087;
struct Cyc_Absyn_Exp* _temp2100= Cyc_Absyn_copy_exp( _temp1479); struct Cyc_Absyn_Exp*
_temp2097[ 2u]={ _temp2099, _temp2100}; struct _tagged_ptr8 _temp2098={
_temp2097, _temp2097, _temp2097 + 2u}; _temp2096( _temp2098);}), 0))->r));}}
else{ struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp(
i, 0), _temp2087, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp((
void*) e3->r, 0), 0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1479, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1481, s1, 0), 0))->r));}} goto _LL2082;} _LL2086: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2079); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2073); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2110=({ struct Cyc_List_List*(* _temp2101)( struct
_tagged_ptr7)=( struct Cyc_List_List*(*)( struct _tagged_ptr7)) Cyc_List_list;
struct _tuple1* _temp2104=({ struct _tuple1* _temp2109=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2109->f1= 0; _temp2109->f2= Cyc_Toc_mt_tq;
_temp2109->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1481->topt))->v); _temp2109;}); struct _tuple1* _temp2105=({ struct _tuple1*
_temp2108=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2108->f1=
0; _temp2108->f2= Cyc_Toc_mt_tq; _temp2108->f3= Cyc_Absyn_uint_t; _temp2108;});
struct _tuple1* _temp2106=({ struct _tuple1* _temp2107=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2107->f1= 0; _temp2107->f2= Cyc_Toc_mt_tq;
_temp2107->f3= Cyc_Absyn_uint_t; _temp2107;}); struct _tuple1* _temp2102[ 3u]={
_temp2104, _temp2105, _temp2106}; struct _tagged_ptr7 _temp2103={ _temp2102,
_temp2102, _temp2102 + 3u}; _temp2101( _temp2103);}); goto _LL2111; _LL2111: {
void* _temp2115= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2112=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2112[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2113; _temp2113.tag= Cyc_Absyn_FnType;
_temp2113.f1=({ struct Cyc_Absyn_FnInfo _temp2114; _temp2114.tvars= 0; _temp2114.effect=
0; _temp2114.ret_typ=( void*) ta2; _temp2114.args= _temp2110; _temp2114.c_varargs=
0; _temp2114.cyc_varargs= 0; _temp2114.rgn_po= 0; _temp2114.attributes= 0;
_temp2114;}); _temp2113;}); _temp2112;}), Cyc_Toc_mt_tq); goto _LL2116; _LL2116: {
struct Cyc_Absyn_Exp* fnexp= Cyc_Absyn_cast_exp( _temp2115, Cyc_Absyn_var_exp(
Cyc_Toc__check_unknown_subscript_qv, 0), 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp( fnexp,({ struct Cyc_List_List*(*
_temp2117)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)( struct _tagged_ptr8))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp2120= _temp1481; struct Cyc_Absyn_Exp*
_temp2121= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp* _temp2122=
_temp1479; struct Cyc_Absyn_Exp* _temp2118[ 3u]={ _temp2120, _temp2121,
_temp2122}; struct _tagged_ptr8 _temp2119={ _temp2118, _temp2118, _temp2118 + 3u};
_temp2117( _temp2119);}), 0), 0))->r));}}} else{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2073); struct Cyc_Absyn_Exp*
test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_or_exp(
Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp1479, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1481, s1, 0), 0))->r));} goto _LL2082;} _LL2082:;}
goto _LL2060; _LL2066:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2123=( unsigned char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp2124; _temp2124.curr= _temp2123; _temp2124.base=
_temp2123; _temp2124.last_plus_one= _temp2123 + 49; _temp2124;})); goto _LL2060;
_LL2060:;} goto _LL1306;} _LL1358: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1483))->r));} else{ struct Cyc_List_List*
_temp2125=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1483); goto
_LL2126; _LL2126: { struct _tagged_string* _temp2127= Cyc_Toc_add_tuple_type(
_temp2125); goto _LL2128; _LL2128: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1483 != 0;( _temp1483=(( struct Cyc_List_List*) _check_null(
_temp1483))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1483))->hd); dles=({ struct Cyc_List_List*
_temp2129=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2129->hd=( void*)({ struct _tuple4* _temp2130=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2130->f1= 0; _temp2130->f2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1483))->hd; _temp2130;}); _temp2129->tl=
dles; _temp2129;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));}}} goto _LL1306; _LL1360:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ unsigned char* _temp2131=( unsigned char*)"compoundlit";
struct _tagged_string _temp2132; _temp2132.curr= _temp2131; _temp2132.base=
_temp2131; _temp2132.last_plus_one= _temp2131 + 12; _temp2132;})); goto _LL1306;
_LL1362:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1489, 0))->r));{ struct Cyc_List_List* _temp2133= _temp1489; goto _LL2134;
_LL2134: for( 0; _temp2133 != 0; _temp2133=(( struct Cyc_List_List*) _check_null(
_temp2133))->tl){ struct _tuple4 _temp2137; struct Cyc_Absyn_Exp* _temp2138;
struct Cyc_List_List* _temp2140; struct _tuple4* _temp2135=( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp2133))->hd; _temp2137=* _temp2135;
_LL2141: _temp2140= _temp2137.f1; goto _LL2139; _LL2139: _temp2138= _temp2137.f2;
goto _LL2136; _LL2136: Cyc_Toc_exp_to_c( nv, _temp2138);}} goto _LL1306; _LL1364: {
unsigned int _temp2142= Cyc_Evexp_eval_const_uint_exp( _temp1493); goto _LL2143;
_LL2143: Cyc_Toc_exp_to_c( nv, _temp1491);{ struct Cyc_List_List* es= 0; if( !
Cyc_Toc_is_zero( _temp1491)){ unsigned int i= 0; for( 0; i < _temp2142; i ++){
es=({ struct Cyc_List_List* _temp2144=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2144->hd=( void*)({ struct _tuple4*
_temp2145=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp2145->f1=
0; _temp2145->f2= _temp1491; _temp2145;}); _temp2144->tl= es; _temp2144;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL1306;}} _LL1366: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1499, _temp1503))->r));} else{ if(
_temp1497 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2146=( unsigned char*)"Struct_e: missing structdecl pointer";
struct _tagged_string _temp2147; _temp2147.curr= _temp2146; _temp2147.base=
_temp2146; _temp2147.last_plus_one= _temp2146 + 37; _temp2147;}));}{ struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*) _check_null( _temp1497); struct
_RegionHandle _temp2148= _new_region(); struct _RegionHandle* rgn=& _temp2148;
_push_region( rgn);{ struct Cyc_List_List* _temp2149=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1499,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd2->fields))->v); goto _LL2150; _LL2150: { struct Cyc_List_List* _temp2151= 0;
goto _LL2152; _LL2152:{ struct Cyc_List_List* _temp2153=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto _LL2154; _LL2154: for(
0; _temp2153 != 0; _temp2153=(( struct Cyc_List_List*) _check_null( _temp2153))->tl){
struct Cyc_List_List* _temp2155= _temp2149; goto _LL2156; _LL2156: for( 0;
_temp2155 != 0; _temp2155=(( struct Cyc_List_List*) _check_null( _temp2155))->tl){
if((*(( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2155))->hd)).f1
==( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2153))->hd){ struct _tuple12 _temp2159; struct Cyc_Absyn_Exp* _temp2160;
struct Cyc_Absyn_Structfield* _temp2162; struct _tuple12* _temp2157=( struct
_tuple12*)(( struct Cyc_List_List*) _check_null( _temp2155))->hd; _temp2159=*
_temp2157; _LL2163: _temp2162= _temp2159.f1; goto _LL2161; _LL2161: _temp2160=
_temp2159.f2; goto _LL2158; _LL2158: { void* _temp2164=( void*) _temp2162->type;
goto _LL2165; _LL2165: Cyc_Toc_exp_to_c( nv, _temp2160); if( Cyc_Toc_is_void_star(
_temp2164)){( void*)( _temp2160->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp2160->r, 0), 0))->r));} _temp2151=({ struct Cyc_List_List*
_temp2166=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2166->hd=( void*)({ struct _tuple4* _temp2167=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2167->f1= 0; _temp2167->f2= _temp2160; _temp2167;});
_temp2166->tl= _temp2151; _temp2166;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp2151), 0))->r));}}; _pop_region(& _temp2148);}} goto
_LL1306; _LL1368: { struct Cyc_List_List* fs;{ void* _temp2168= Cyc_Tcutil_compress(
_temp1507); struct Cyc_List_List* _temp2174; _LL2170: if(( unsigned int)
_temp2168 > 4u?*(( int*) _temp2168) == Cyc_Absyn_AnonStructType: 0){ _LL2175:
_temp2174=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2168)->f1; goto
_LL2171;} else{ goto _LL2172;} _LL2172: goto _LL2173; _LL2171: fs= _temp2174;
goto _LL2169; _LL2173: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct _tagged_string _temp2176=
Cyc_Absynpp_typ2string( _temp1507); xprintf("anon struct has type %.*s",
_temp2176.last_plus_one - _temp2176.curr, _temp2176.curr);})); goto _LL2169;
_LL2169:;}{ struct _RegionHandle _temp2177= _new_region(); struct _RegionHandle*
rgn=& _temp2177; _push_region( rgn);{ struct Cyc_List_List* _temp2178=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1505, fs); goto _LL2179; _LL2179: for( 0; _temp2178 != 0;
_temp2178=(( struct Cyc_List_List*) _check_null( _temp2178))->tl){ struct
_tuple12 _temp2182; struct Cyc_Absyn_Exp* _temp2183; struct Cyc_Absyn_Structfield*
_temp2185; struct _tuple12* _temp2180=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp2178))->hd; _temp2182=* _temp2180; _LL2186: _temp2185=
_temp2182.f1; goto _LL2184; _LL2184: _temp2183= _temp2182.f2; goto _LL2181;
_LL2181: { void* _temp2187=( void*) _temp2185->type; goto _LL2188; _LL2188: Cyc_Toc_exp_to_c(
nv, _temp2183); if( Cyc_Toc_is_void_star( _temp2187)){( void*)( _temp2183->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp2183->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1505, 0))->r));}; _pop_region(& _temp2177);} goto _LL1306;} _LL1370: {
struct _tuple0* qv= _temp1509->name; if( _temp1511->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2189= _temp1511->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1511->fields))->v;
goto _LL2190; _LL2190: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2189))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2189))->hd)->typs
== 0){ tag_count ++;} _temp2189=(( struct Cyc_List_List*) _check_null( _temp2189))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1306;} _LL1372: { struct _tuple0* _temp2191= _temp1519->name; goto
_LL2192; _LL2192: { struct Cyc_List_List* _temp2193= _temp1519->typs; goto
_LL2194; _LL2194: { struct _tuple0* _temp2195= Cyc_Toc_temp_var(); goto _LL2196;
_LL2196: { struct Cyc_Absyn_Exp* _temp2197= Cyc_Absyn_var_exp( _temp2195, 0);
goto _LL2198; _LL2198: { void* _temp2201= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2191,({ unsigned char* _temp2199=( unsigned char*)"_struct"; struct
_tagged_string _temp2200; _temp2200.curr= _temp2199; _temp2200.base= _temp2199;
_temp2200.last_plus_one= _temp2199 + 8; _temp2200;}))); goto _LL2202; _LL2202:
if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp; if( _temp1521->is_xtunion){
tag_exp= Cyc_Absyn_var_exp( _temp2191, 0);} else{ int tag_count= 0; struct Cyc_List_List*
_temp2203= _temp1521->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1521->fields))->v; goto _LL2204; _LL2204: while( Cyc_Absyn_qvar_cmp(
_temp2191,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2203))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2203))->hd)->typs != 0){ tag_count ++;}
_temp2203=(( struct Cyc_List_List*) _check_null( _temp2203))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2205= 0; goto _LL2206;
_LL2206: for( 0; _temp1523 != 0;( _temp1523=(( struct Cyc_List_List*)
_check_null( _temp1523))->tl, _temp2193=(( struct Cyc_List_List*) _check_null(
_temp2193))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1523))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2193))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2205=({ struct Cyc_List_List* _temp2207=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2207[ 0]=({ struct Cyc_List_List
_temp2208; _temp2208.hd=( void*)({ struct _tuple4* _temp2209=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2209[ 0]=({ struct _tuple4
_temp2210; _temp2210.f1= 0; _temp2210.f2= cur_e; _temp2210;}); _temp2209;});
_temp2208.tl= _temp2205; _temp2208;}); _temp2207;});} _temp2205=({ struct Cyc_List_List*
_temp2211=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2211[ 0]=({ struct Cyc_List_List _temp2212; _temp2212.hd=( void*)({ struct
_tuple4* _temp2213=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2213[ 0]=({ struct _tuple4 _temp2214; _temp2214.f1= 0; _temp2214.f2=
tag_exp; _temp2214;}); _temp2213;}); _temp2212.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2205); _temp2212;}); _temp2211;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2215=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2215->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2191,({ unsigned char*
_temp2216=( unsigned char*)"_struct"; struct _tagged_string _temp2217; _temp2217.curr=
_temp2216; _temp2217.base= _temp2216; _temp2217.last_plus_one= _temp2216 + 8;
_temp2217;})); _temp2215;}), _temp2205, 0))->r));}} else{ struct Cyc_List_List*
_temp2219=({ struct Cyc_List_List* _temp2218=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2218->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2197, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp2191, 0), 0); _temp2218->tl= 0; _temp2218;}); goto _LL2220; _LL2220:{ int i=
1; for( 0; _temp1523 != 0;((( _temp1523=(( struct Cyc_List_List*) _check_null(
_temp1523))->tl, i ++)), _temp2193=(( struct Cyc_List_List*) _check_null(
_temp2193))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1523))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2193))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp2221= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2197, Cyc_Toc_fieldname( i), 0), cur_e, 0);
goto _LL2222; _LL2222: _temp2219=({ struct Cyc_List_List* _temp2223=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2223->hd=( void*) _temp2221;
_temp2223->tl= _temp2219; _temp2223;});}}}{ struct Cyc_Absyn_Stmt* _temp2224=
Cyc_Absyn_exp_stmt( _temp2197, 0); goto _LL2225; _LL2225: { struct Cyc_Absyn_Stmt*
_temp2227= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2226=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2226->hd=( void*) _temp2224;
_temp2226->tl= _temp2219; _temp2226;})), 0); goto _LL2228; _LL2228:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp2195,
_temp2201, 0, _temp2227, 0), 0))->r));}}} goto _LL1306;}}}}} _LL1374: goto
_LL1306; _LL1376: { void* t_c= Cyc_Toc_typ_to_c( _temp1535); if( _temp1537 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1537); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1535, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL1306;} _LL1378: Cyc_Toc_stmt_to_c(
nv, _temp1539); goto _LL1306; _LL1380:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2229=( unsigned char*)"UnresolvedMem"; struct _tagged_string
_temp2230; _temp2230.curr= _temp2229; _temp2230.base= _temp2229; _temp2230.last_plus_one=
_temp2229 + 14; _temp2230;})); goto _LL1306; _LL1382:(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp2231=( unsigned char*)"codegen";
struct _tagged_string _temp2232; _temp2232.curr= _temp2231; _temp2232.base=
_temp2231; _temp2232.last_plus_one= _temp2231 + 8; _temp2232;})); goto _LL1306;
_LL1384:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp2233=( unsigned char*)"fill"; struct _tagged_string _temp2234; _temp2234.curr=
_temp2233; _temp2234.base= _temp2233; _temp2234.last_plus_one= _temp2233 + 5;
_temp2234;})); goto _LL1306; _LL1306:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
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
_temp2235=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp2271; struct Cyc_Absyn_Vardecl
_temp2273; struct Cyc_List_List* _temp2274; struct Cyc_Core_Opt* _temp2276;
struct Cyc_Absyn_Exp* _temp2278; void* _temp2280; struct Cyc_Absyn_Tqual
_temp2282; struct _tuple0* _temp2284; void* _temp2286; struct Cyc_Absyn_Vardecl*
_temp2288; int _temp2290; void* _temp2292; unsigned char _temp2294; struct
_tagged_string _temp2296; struct Cyc_Absyn_Enumfield* _temp2298; struct Cyc_Absyn_Enumdecl*
_temp2300; struct Cyc_List_List* _temp2302; struct Cyc_List_List* _temp2304;
struct Cyc_Absyn_Tunionfield* _temp2306; struct Cyc_Absyn_Tuniondecl* _temp2308;
struct Cyc_Absyn_Pat* _temp2311; struct Cyc_Absyn_Pat _temp2313; struct Cyc_Position_Segment*
_temp2314; struct Cyc_Core_Opt* _temp2316; void* _temp2318; struct Cyc_List_List*
_temp2320; struct Cyc_List_List* _temp2322; struct Cyc_Absyn_Tunionfield*
_temp2324; struct Cyc_Absyn_Tuniondecl* _temp2326; struct Cyc_List_List*
_temp2328; struct Cyc_List_List* _temp2330; struct Cyc_Absyn_Tunionfield*
_temp2332; struct Cyc_Absyn_Tuniondecl* _temp2334; struct Cyc_List_List*
_temp2336; struct Cyc_List_List* _temp2338; struct Cyc_List_List* _temp2340;
struct Cyc_Core_Opt* _temp2342; struct Cyc_Absyn_Structdecl* _temp2344; struct
Cyc_Absyn_Pat* _temp2346; struct _tuple0* _temp2348; struct Cyc_List_List*
_temp2350; struct Cyc_List_List* _temp2352; struct _tuple0* _temp2354; struct
Cyc_List_List* _temp2356; struct Cyc_List_List* _temp2358; struct _tuple0*
_temp2360; _LL2237: if(( unsigned int) _temp2235 > 2u?*(( int*) _temp2235) ==
Cyc_Absyn_Var_p: 0){ _LL2272: _temp2271=(( struct Cyc_Absyn_Var_p_struct*)
_temp2235)->f1; _temp2273=* _temp2271; _LL2287: _temp2286=( void*) _temp2273.sc;
goto _LL2285; _LL2285: _temp2284= _temp2273.name; goto _LL2283; _LL2283:
_temp2282= _temp2273.tq; goto _LL2281; _LL2281: _temp2280=( void*) _temp2273.type;
goto _LL2279; _LL2279: _temp2278= _temp2273.initializer; goto _LL2277; _LL2277:
_temp2276= _temp2273.rgn; goto _LL2275; _LL2275: _temp2274= _temp2273.attributes;
goto _LL2238;} else{ goto _LL2239;} _LL2239: if( _temp2235 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2240;} else{ goto _LL2241;} _LL2241: if(( unsigned int) _temp2235 > 2u?*((
int*) _temp2235) == Cyc_Absyn_Reference_p: 0){ _LL2289: _temp2288=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2235)->f1; goto _LL2242;} else{ goto _LL2243;} _LL2243: if( _temp2235 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2244;} else{ goto _LL2245;} _LL2245: if((
unsigned int) _temp2235 > 2u?*(( int*) _temp2235) == Cyc_Absyn_Int_p: 0){
_LL2293: _temp2292=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2235)->f1;
goto _LL2291; _LL2291: _temp2290=(( struct Cyc_Absyn_Int_p_struct*) _temp2235)->f2;
goto _LL2246;} else{ goto _LL2247;} _LL2247: if(( unsigned int) _temp2235 > 2u?*((
int*) _temp2235) == Cyc_Absyn_Char_p: 0){ _LL2295: _temp2294=(( struct Cyc_Absyn_Char_p_struct*)
_temp2235)->f1; goto _LL2248;} else{ goto _LL2249;} _LL2249: if(( unsigned int)
_temp2235 > 2u?*(( int*) _temp2235) == Cyc_Absyn_Float_p: 0){ _LL2297: _temp2296=((
struct Cyc_Absyn_Float_p_struct*) _temp2235)->f1; goto _LL2250;} else{ goto
_LL2251;} _LL2251: if(( unsigned int) _temp2235 > 2u?*(( int*) _temp2235) == Cyc_Absyn_Enum_p:
0){ _LL2301: _temp2300=(( struct Cyc_Absyn_Enum_p_struct*) _temp2235)->f1; goto
_LL2299; _LL2299: _temp2298=(( struct Cyc_Absyn_Enum_p_struct*) _temp2235)->f2;
goto _LL2252;} else{ goto _LL2253;} _LL2253: if(( unsigned int) _temp2235 > 2u?*((
int*) _temp2235) == Cyc_Absyn_Tunion_p: 0){ _LL2309: _temp2308=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2235)->f1; goto _LL2307; _LL2307: _temp2306=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2235)->f2; goto _LL2305; _LL2305: _temp2304=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2235)->f3; goto _LL2303; _LL2303: _temp2302=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2235)->f4; if( _temp2302 == 0){ goto _LL2254;} else{ goto _LL2255;}} else{
goto _LL2255;} _LL2255: if(( unsigned int) _temp2235 > 2u?*(( int*) _temp2235)
== Cyc_Absyn_Pointer_p: 0){ _LL2312: _temp2311=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2235)->f1; _temp2313=* _temp2311; _LL2319: _temp2318=( void*) _temp2313.r;
if(( unsigned int) _temp2318 > 2u?*(( int*) _temp2318) == Cyc_Absyn_Tunion_p: 0){
_LL2327: _temp2326=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2318)->f1; goto
_LL2325; _LL2325: _temp2324=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2318)->f2;
goto _LL2323; _LL2323: _temp2322=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2318)->f3;
goto _LL2321; _LL2321: _temp2320=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2318)->f4;
goto _LL2317;} else{ goto _LL2257;} _LL2317: _temp2316= _temp2313.topt; goto
_LL2315; _LL2315: _temp2314= _temp2313.loc; goto _LL2310;} else{ goto _LL2257;}
_LL2310: if( _temp2320 != 0){ goto _LL2256;} else{ goto _LL2257;} _LL2257: if((
unsigned int) _temp2235 > 2u?*(( int*) _temp2235) == Cyc_Absyn_Tunion_p: 0){
_LL2335: _temp2334=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2235)->f1; goto
_LL2333; _LL2333: _temp2332=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2235)->f2;
goto _LL2331; _LL2331: _temp2330=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2235)->f3;
goto _LL2329; _LL2329: _temp2328=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2235)->f4;
goto _LL2258;} else{ goto _LL2259;} _LL2259: if(( unsigned int) _temp2235 > 2u?*((
int*) _temp2235) == Cyc_Absyn_Tuple_p: 0){ _LL2337: _temp2336=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2235)->f1; goto _LL2260;} else{ goto _LL2261;} _LL2261: if(( unsigned int)
_temp2235 > 2u?*(( int*) _temp2235) == Cyc_Absyn_Struct_p: 0){ _LL2345:
_temp2344=(( struct Cyc_Absyn_Struct_p_struct*) _temp2235)->f1; goto _LL2343;
_LL2343: _temp2342=(( struct Cyc_Absyn_Struct_p_struct*) _temp2235)->f2; goto
_LL2341; _LL2341: _temp2340=(( struct Cyc_Absyn_Struct_p_struct*) _temp2235)->f3;
goto _LL2339; _LL2339: _temp2338=(( struct Cyc_Absyn_Struct_p_struct*) _temp2235)->f4;
goto _LL2262;} else{ goto _LL2263;} _LL2263: if(( unsigned int) _temp2235 > 2u?*((
int*) _temp2235) == Cyc_Absyn_Pointer_p: 0){ _LL2347: _temp2346=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2235)->f1; goto _LL2264;} else{ goto _LL2265;} _LL2265: if(( unsigned int)
_temp2235 > 2u?*(( int*) _temp2235) == Cyc_Absyn_UnknownId_p: 0){ _LL2349:
_temp2348=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2235)->f1; goto _LL2266;}
else{ goto _LL2267;} _LL2267: if(( unsigned int) _temp2235 > 2u?*(( int*)
_temp2235) == Cyc_Absyn_UnknownCall_p: 0){ _LL2355: _temp2354=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2235)->f1; goto _LL2353; _LL2353: _temp2352=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2235)->f2; goto _LL2351; _LL2351: _temp2350=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2235)->f3; goto _LL2268;} else{ goto _LL2269;} _LL2269: if(( unsigned int)
_temp2235 > 2u?*(( int*) _temp2235) == Cyc_Absyn_UnknownFields_p: 0){ _LL2361:
_temp2360=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2235)->f1; goto
_LL2359; _LL2359: _temp2358=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2235)->f2; goto _LL2357; _LL2357: _temp2356=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2235)->f3; goto _LL2270;} else{ goto _LL2236;} _LL2238: nv= Cyc_Toc_add_varmap(
nv, _temp2284, r); goto _LL2240; _LL2240: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2236; _LL2242: { struct _tuple0* _temp2362= Cyc_Toc_temp_var(); goto
_LL2363; _LL2363: decls=({ struct Cyc_List_List* _temp2364=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2364->hd=( void*)({
struct _tuple14* _temp2365=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2365->f1= _temp2362; _temp2365->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2365;}); _temp2364->tl= decls;
_temp2364;}); nv= Cyc_Toc_add_varmap( nv, _temp2288->name, Cyc_Absyn_var_exp(
_temp2362, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2362, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2236;} _LL2244: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2236; _LL2246: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2292, _temp2290, 0), succ_lab, fail_lab); goto _LL2236;
_LL2248: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2294, 0),
succ_lab, fail_lab); goto _LL2236; _LL2250: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2296, 0), succ_lab, fail_lab); goto _LL2236; _LL2252: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2366=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2366[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2367; _temp2367.tag= Cyc_Absyn_Enum_e;
_temp2367.f1= _temp2298->name; _temp2367.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2300; _temp2367.f3=( struct Cyc_Absyn_Enumfield*) _temp2298; _temp2367;});
_temp2366;}), 0), succ_lab, fail_lab); goto _LL2236; _LL2254: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2306->name, 0); if( ! _temp2308->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2236;} _LL2256: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2320);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2324->name,({
unsigned char* _temp2401=( unsigned char*)"_struct"; struct _tagged_string
_temp2402; _temp2402.curr= _temp2401; _temp2402.base= _temp2401; _temp2402.last_plus_one=
_temp2401 + 8; _temp2402;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp2368=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2324->typs); goto _LL2369;
_LL2369: { struct Cyc_List_List* _temp2370=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2320); goto
_LL2371; _LL2371: for( 0; _temp2370 != 0;((( _temp2370=(( struct Cyc_List_List*)
_check_null( _temp2370))->tl, _temp2368=(( struct Cyc_List_List*) _check_null(
_temp2368))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp2372=( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp2370))->hd; goto _LL2373; _LL2373: {
void* _temp2374=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2368))->hd)).f2; goto _LL2375; _LL2375: { struct _tuple0* _temp2376= Cyc_Toc_temp_var();
goto _LL2377; _LL2377: { void* _temp2378=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2372->topt))->v; goto _LL2379; _LL2379: { void* _temp2380= Cyc_Toc_typ_to_c(
_temp2378); goto _LL2381; _LL2381: { struct _tagged_string* _temp2382= Cyc_Toc_fresh_label();
goto _LL2383; _LL2383: { struct Cyc_Absyn_Exp* _temp2384= Cyc_Absyn_structarrow_exp(
rcast, Cyc_Toc_fieldname( cnt), 0); goto _LL2385; _LL2385: if( Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c( _temp2374))){ _temp2384= Cyc_Absyn_cast_exp( _temp2380,
_temp2384, 0);} decls=({ struct Cyc_List_List* _temp2386=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2386->hd=( void*)({
struct _tuple14* _temp2387=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2387->f1= _temp2376; _temp2387->f2= _temp2380; _temp2387;});
_temp2386->tl= decls; _temp2386;});{ struct _tuple13 _temp2388= Cyc_Toc_xlate_pat(
nv, rgn, _temp2378, Cyc_Absyn_var_exp( _temp2376, 0), _temp2384, _temp2372,
succ_lab, fail_lab, decls); goto _LL2389; _LL2389: nv= _temp2388.f1; decls=
_temp2388.f2;{ struct Cyc_Absyn_Stmt* _temp2390= _temp2388.f3; goto _LL2391;
_LL2391: { struct Cyc_Absyn_Stmt* _temp2392= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2376, 0), _temp2384, 0); goto _LL2393; _LL2393: { struct Cyc_Absyn_Stmt*
_temp2394= Cyc_Absyn_seq_stmt( _temp2392, _temp2390, 0); goto _LL2395; _LL2395:
ss=({ struct Cyc_List_List* _temp2396=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2396->hd=( void*) Cyc_Absyn_label_stmt(
_temp2382, _temp2394, 0); _temp2396->tl= ss; _temp2396;}); succ_lab= _temp2382;}}}}}}}}}}}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2326->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2324->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2397=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2326->fields))->v; goto _LL2398; _LL2398: for( 0; _temp2397 !=
0; _temp2397=(( struct Cyc_List_List*) _check_null( _temp2397))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2399=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2397))->hd; goto _LL2400; _LL2400: if( _temp2399->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2324->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2236;}}} _LL2258: _temp2336=
_temp2328; goto _LL2260; _LL2260: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2336);{ struct Cyc_List_List*
_temp2403=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2336); goto _LL2404; _LL2404: for( 0; _temp2403 !=
0;( _temp2403=(( struct Cyc_List_List*) _check_null( _temp2403))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2405=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2403))->hd; goto _LL2406; _LL2406: { struct _tuple0* _temp2407=
Cyc_Toc_temp_var(); goto _LL2408; _LL2408: { void* _temp2409=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2405->topt))->v; goto _LL2410; _LL2410: {
struct _tagged_string* _temp2411= Cyc_Toc_fresh_label(); goto _LL2412; _LL2412:
decls=({ struct Cyc_List_List* _temp2413=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2413->hd=( void*)({ struct _tuple14*
_temp2414=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2414->f1= _temp2407; _temp2414->f2= Cyc_Toc_typ_to_c( _temp2409); _temp2414;});
_temp2413->tl= decls; _temp2413;});{ struct _tuple13 _temp2415= Cyc_Toc_xlate_pat(
nv, rgn, _temp2409, Cyc_Absyn_var_exp( _temp2407, 0), Cyc_Absyn_structmember_exp(
path, Cyc_Toc_fieldname( cnt), 0), _temp2405, succ_lab, fail_lab, decls); goto
_LL2416; _LL2416: nv= _temp2415.f1; decls= _temp2415.f2;{ struct Cyc_Absyn_Stmt*
_temp2417= _temp2415.f3; goto _LL2418; _LL2418: { struct Cyc_Absyn_Stmt*
_temp2419= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2407, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2417, 0); goto _LL2420; _LL2420: ss=({ struct Cyc_List_List* _temp2421=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2421->hd=(
void*) Cyc_Absyn_label_stmt( _temp2411, _temp2419, 0); _temp2421->tl= ss;
_temp2421;}); succ_lab= _temp2411;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2236;} _LL2262: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2422=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2338); goto _LL2423; _LL2423: for( 0; _temp2422 !=
0; _temp2422=(( struct Cyc_List_List*) _check_null( _temp2422))->tl){ struct
_tuple15* _temp2424=( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2422))->hd; goto _LL2425; _LL2425: { struct Cyc_Absyn_Pat* _temp2426=(*
_temp2424).f2; goto _LL2427; _LL2427: { struct _tagged_string* f;{ void*
_temp2428=( void*)(( struct Cyc_List_List*) _check_null((* _temp2424).f1))->hd;
struct _tagged_string* _temp2434; _LL2430: if(*(( int*) _temp2428) == Cyc_Absyn_FieldName){
_LL2435: _temp2434=(( struct Cyc_Absyn_FieldName_struct*) _temp2428)->f1; goto
_LL2431;} else{ goto _LL2432;} _LL2432: goto _LL2433; _LL2431: f= _temp2434;
goto _LL2429; _LL2433:( void) _throw(( void*) Cyc_Toc_Match_error); _LL2429:;}{
struct _tuple0* _temp2436= Cyc_Toc_temp_var(); goto _LL2437; _LL2437: { void*
_temp2438=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2426->topt))->v;
goto _LL2439; _LL2439: { void* _temp2440= Cyc_Toc_typ_to_c( _temp2438); goto
_LL2441; _LL2441: { struct _tagged_string* _temp2442= Cyc_Toc_fresh_label();
goto _LL2443; _LL2443: decls=({ struct Cyc_List_List* _temp2444=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2444->hd=( void*)({
struct _tuple14* _temp2445=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2445->f1= _temp2436; _temp2445->f2= _temp2440; _temp2445;});
_temp2444->tl= decls; _temp2444;});{ struct _tuple13 _temp2446= Cyc_Toc_xlate_pat(
nv, rgn, _temp2438, Cyc_Absyn_var_exp( _temp2436, 0), Cyc_Absyn_structmember_exp(
path, f, 0), _temp2426, succ_lab, fail_lab, decls); goto _LL2447; _LL2447: nv=
_temp2446.f1; decls= _temp2446.f2;{ struct Cyc_Absyn_Exp* _temp2448= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2449; _LL2449: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2344->fields))->v, f)))->type)){ _temp2448= Cyc_Absyn_cast_exp(
_temp2440, _temp2448, 0);}{ struct Cyc_Absyn_Stmt* _temp2450= _temp2446.f3; goto
_LL2451; _LL2451: { struct Cyc_Absyn_Stmt* _temp2452= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2436, 0), _temp2448, 0), _temp2450, 0); goto _LL2453;
_LL2453: ss=({ struct Cyc_List_List* _temp2454=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2454->hd=( void*) Cyc_Absyn_label_stmt(
_temp2442, _temp2452, 0); _temp2454->tl= ss; _temp2454;}); succ_lab= _temp2442;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2236;} _LL2264: { struct _tuple0*
_temp2455= Cyc_Toc_temp_var(); goto _LL2456; _LL2456: { void* _temp2457=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2346->topt))->v; goto _LL2458; _LL2458:
decls=({ struct Cyc_List_List* _temp2459=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2459->hd=( void*)({ struct _tuple14*
_temp2460=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2460->f1= _temp2455; _temp2460->f2= Cyc_Toc_typ_to_c( _temp2457); _temp2460;});
_temp2459->tl= decls; _temp2459;});{ struct _tuple13 _temp2461= Cyc_Toc_xlate_pat(
nv, rgn, _temp2457, Cyc_Absyn_var_exp( _temp2455, 0), Cyc_Absyn_deref_exp( path,
0), _temp2346, succ_lab, fail_lab, decls); goto _LL2462; _LL2462: nv= _temp2461.f1;
decls= _temp2461.f2;{ struct Cyc_Absyn_Stmt* _temp2463= _temp2461.f3; goto
_LL2464; _LL2464: { struct Cyc_Absyn_Stmt* _temp2465= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2455, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2463, 0);
goto _LL2466; _LL2466: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2465, 0);} else{ s= _temp2465;} goto _LL2236;}}}}} _LL2266: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2467=( unsigned char*)"unknownid"; struct _tagged_string
_temp2468; _temp2468.curr= _temp2467; _temp2468.base= _temp2467; _temp2468.last_plus_one=
_temp2467 + 10; _temp2468;})); goto _LL2236; _LL2268: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2469=(
unsigned char*)"unknowncall"; struct _tagged_string _temp2470; _temp2470.curr=
_temp2469; _temp2470.base= _temp2469; _temp2470.last_plus_one= _temp2469 + 12;
_temp2470;})); goto _LL2236; _LL2270: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2471=(
unsigned char*)"unknownfields"; struct _tagged_string _temp2472; _temp2472.curr=
_temp2471; _temp2472.base= _temp2471; _temp2472.last_plus_one= _temp2471 + 14;
_temp2472;})); goto _LL2236; _LL2236:;} return({ struct _tuple13 _temp2473;
_temp2473.f1= nv; _temp2473.f2= decls; _temp2473.f3= s; _temp2473;});} struct
_tuple16{ struct _tagged_string* f1; struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple16* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple16* _temp2474=(
struct _tuple16*) _region_malloc( r, sizeof( struct _tuple16)); _temp2474->f1=
Cyc_Toc_fresh_label(); _temp2474->f2= Cyc_Toc_fresh_label(); _temp2474->f3= sc;
_temp2474;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp2475=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2476; _LL2476: { int leave_as_switch;{ void*
_temp2477= Cyc_Tcutil_compress( _temp2475); void* _temp2483; void* _temp2485;
_LL2479: if(( unsigned int) _temp2477 > 4u?*(( int*) _temp2477) == Cyc_Absyn_IntType:
0){ _LL2486: _temp2485=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2477)->f1;
goto _LL2484; _LL2484: _temp2483=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2477)->f2; goto _LL2480;} else{ goto _LL2481;} _LL2481: goto _LL2482;
_LL2480: leave_as_switch= 1; goto _LL2478; _LL2482: leave_as_switch= 0; goto
_LL2478; _LL2478:;}{ struct Cyc_List_List* _temp2487= scs; goto _LL2488; _LL2488:
for( 0; _temp2487 != 0; _temp2487=(( struct Cyc_List_List*) _check_null(
_temp2487))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2487))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2487))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2489= scs; goto
_LL2490; _LL2490: for( 0; _temp2489 != 0; _temp2489=(( struct Cyc_List_List*)
_check_null( _temp2489))->tl){ struct Cyc_Absyn_Stmt* _temp2491=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2489))->hd)->body; goto _LL2492;
_LL2492:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2489))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2491, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2491);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2493=
_new_region(); struct _RegionHandle* rgn=& _temp2493; _push_region( rgn);{
struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2494= lscs; goto
_LL2495; _LL2495: for( 0; _temp2494 != 0; _temp2494=(( struct Cyc_List_List*)
_check_null( _temp2494))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct
_tuple16*)(( struct Cyc_List_List*) _check_null( _temp2494))->hd)).f3; struct
_tagged_string* fail_lab=(( struct Cyc_List_List*) _check_null( _temp2494))->tl
== 0? end_l:(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp2494))->tl))->hd)).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2494))->hd)).f2; if( sc->where_clause == 0){ struct _tuple13
_temp2496= Cyc_Toc_xlate_pat( nv, rgn, _temp2475, r, path, sc->pattern, succ_lab,
fail_lab, decls); goto _LL2497; _LL2497: nvs=({ struct Cyc_List_List* _temp2498=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2498->hd=( void*) _temp2496.f1; _temp2498->tl= nvs; _temp2498;}); decls=
_temp2496.f2; test_stmts=({ struct Cyc_List_List* _temp2499=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2499->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2494))->hd)).f1,
_temp2496.f3, 0); _temp2499->tl= test_stmts; _temp2499;});} else{ struct Cyc_Absyn_Exp*
_temp2500=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2501;
_LL2501: { struct _tagged_string* _temp2502= Cyc_Toc_fresh_label(); goto _LL2503;
_LL2503: { struct _tuple13 _temp2504= Cyc_Toc_xlate_pat( nv, rgn, _temp2475, r,
path, sc->pattern, _temp2502, fail_lab, decls); goto _LL2505; _LL2505: Cyc_Toc_exp_to_c(
_temp2504.f1, _temp2500); nvs=({ struct Cyc_List_List* _temp2506=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2506->hd=( void*)
_temp2504.f1; _temp2506->tl= nvs; _temp2506;}); decls= _temp2504.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2500, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2502, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2507=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2507->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2494))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2504.f3, s3, 0), 0); _temp2507->tl= test_stmts;
_temp2507;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2508= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2509; _LL2509: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2510=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2511; _LL2511: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2510, end_l,(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2512=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2513; _LL2513: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2512, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2508, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple14 _temp2516;
void* _temp2517; struct _tuple0* _temp2519; struct _tuple14* _temp2514=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2516=*
_temp2514; _LL2520: _temp2519= _temp2516.f1; goto _LL2518; _LL2518: _temp2517=
_temp2516.f2; goto _LL2515; _LL2515: res= Cyc_Absyn_declare_stmt( _temp2519,
_temp2517, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region(& _temp2493);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2521=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2521[ 0]=({ struct Cyc_List_List _temp2522; _temp2522.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2522.tl= 0; _temp2522;}); _temp2521;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2523=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2565; struct Cyc_Absyn_Stmt* _temp2567; struct Cyc_Absyn_Stmt*
_temp2569; struct Cyc_Absyn_Exp* _temp2571; struct Cyc_Absyn_Stmt* _temp2573;
struct Cyc_Absyn_Stmt* _temp2575; struct Cyc_Absyn_Exp* _temp2577; struct Cyc_Absyn_Stmt*
_temp2579; struct _tuple2 _temp2581; struct Cyc_Absyn_Stmt* _temp2583; struct
Cyc_Absyn_Exp* _temp2585; struct Cyc_Absyn_Stmt* _temp2587; struct Cyc_Absyn_Stmt*
_temp2589; struct Cyc_Absyn_Stmt* _temp2591; struct _tagged_string* _temp2593;
struct Cyc_Absyn_Stmt* _temp2595; struct _tuple2 _temp2597; struct Cyc_Absyn_Stmt*
_temp2599; struct Cyc_Absyn_Exp* _temp2601; struct _tuple2 _temp2603; struct Cyc_Absyn_Stmt*
_temp2605; struct Cyc_Absyn_Exp* _temp2607; struct Cyc_Absyn_Exp* _temp2609;
struct Cyc_List_List* _temp2611; struct Cyc_Absyn_Exp* _temp2613; struct Cyc_Absyn_Switch_clause**
_temp2615; struct Cyc_List_List* _temp2617; struct Cyc_Absyn_Stmt* _temp2619;
struct Cyc_Absyn_Decl* _temp2621; struct Cyc_Absyn_Stmt* _temp2623; struct
_tagged_string* _temp2625; struct _tuple2 _temp2627; struct Cyc_Absyn_Stmt*
_temp2629; struct Cyc_Absyn_Exp* _temp2631; struct Cyc_Absyn_Stmt* _temp2633;
struct Cyc_List_List* _temp2635; struct Cyc_Absyn_Stmt* _temp2637; struct Cyc_Absyn_Stmt*
_temp2639; struct Cyc_Absyn_Vardecl* _temp2641; struct Cyc_Absyn_Tvar* _temp2643;
struct Cyc_List_List* _temp2645; struct Cyc_Absyn_Exp* _temp2647; struct Cyc_Absyn_Stmt*
_temp2649; struct Cyc_Absyn_Stmt* _temp2651; _LL2525: if( _temp2523 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2526;} else{ goto _LL2527;} _LL2527: if((
unsigned int) _temp2523 > 1u?*(( int*) _temp2523) == Cyc_Absyn_Exp_s: 0){
_LL2566: _temp2565=(( struct Cyc_Absyn_Exp_s_struct*) _temp2523)->f1; goto
_LL2528;} else{ goto _LL2529;} _LL2529: if(( unsigned int) _temp2523 > 1u?*((
int*) _temp2523) == Cyc_Absyn_Seq_s: 0){ _LL2570: _temp2569=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2523)->f1; goto _LL2568; _LL2568: _temp2567=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2523)->f2; goto _LL2530;} else{ goto _LL2531;} _LL2531: if(( unsigned int)
_temp2523 > 1u?*(( int*) _temp2523) == Cyc_Absyn_Return_s: 0){ _LL2572:
_temp2571=(( struct Cyc_Absyn_Return_s_struct*) _temp2523)->f1; goto _LL2532;}
else{ goto _LL2533;} _LL2533: if(( unsigned int) _temp2523 > 1u?*(( int*)
_temp2523) == Cyc_Absyn_IfThenElse_s: 0){ _LL2578: _temp2577=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2523)->f1; goto _LL2576; _LL2576: _temp2575=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2523)->f2; goto _LL2574; _LL2574: _temp2573=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2523)->f3; goto _LL2534;} else{ goto _LL2535;} _LL2535: if(( unsigned int)
_temp2523 > 1u?*(( int*) _temp2523) == Cyc_Absyn_While_s: 0){ _LL2582: _temp2581=((
struct Cyc_Absyn_While_s_struct*) _temp2523)->f1; _LL2586: _temp2585= _temp2581.f1;
goto _LL2584; _LL2584: _temp2583= _temp2581.f2; goto _LL2580; _LL2580: _temp2579=((
struct Cyc_Absyn_While_s_struct*) _temp2523)->f2; goto _LL2536;} else{ goto
_LL2537;} _LL2537: if(( unsigned int) _temp2523 > 1u?*(( int*) _temp2523) == Cyc_Absyn_Break_s:
0){ _LL2588: _temp2587=(( struct Cyc_Absyn_Break_s_struct*) _temp2523)->f1; goto
_LL2538;} else{ goto _LL2539;} _LL2539: if(( unsigned int) _temp2523 > 1u?*((
int*) _temp2523) == Cyc_Absyn_Continue_s: 0){ _LL2590: _temp2589=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2523)->f1; goto _LL2540;} else{ goto _LL2541;} _LL2541: if(( unsigned int)
_temp2523 > 1u?*(( int*) _temp2523) == Cyc_Absyn_Goto_s: 0){ _LL2594: _temp2593=((
struct Cyc_Absyn_Goto_s_struct*) _temp2523)->f1; goto _LL2592; _LL2592:
_temp2591=(( struct Cyc_Absyn_Goto_s_struct*) _temp2523)->f2; goto _LL2542;}
else{ goto _LL2543;} _LL2543: if(( unsigned int) _temp2523 > 1u?*(( int*)
_temp2523) == Cyc_Absyn_For_s: 0){ _LL2610: _temp2609=(( struct Cyc_Absyn_For_s_struct*)
_temp2523)->f1; goto _LL2604; _LL2604: _temp2603=(( struct Cyc_Absyn_For_s_struct*)
_temp2523)->f2; _LL2608: _temp2607= _temp2603.f1; goto _LL2606; _LL2606:
_temp2605= _temp2603.f2; goto _LL2598; _LL2598: _temp2597=(( struct Cyc_Absyn_For_s_struct*)
_temp2523)->f3; _LL2602: _temp2601= _temp2597.f1; goto _LL2600; _LL2600:
_temp2599= _temp2597.f2; goto _LL2596; _LL2596: _temp2595=(( struct Cyc_Absyn_For_s_struct*)
_temp2523)->f4; goto _LL2544;} else{ goto _LL2545;} _LL2545: if(( unsigned int)
_temp2523 > 1u?*(( int*) _temp2523) == Cyc_Absyn_Switch_s: 0){ _LL2614:
_temp2613=(( struct Cyc_Absyn_Switch_s_struct*) _temp2523)->f1; goto _LL2612;
_LL2612: _temp2611=(( struct Cyc_Absyn_Switch_s_struct*) _temp2523)->f2; goto
_LL2546;} else{ goto _LL2547;} _LL2547: if(( unsigned int) _temp2523 > 1u?*((
int*) _temp2523) == Cyc_Absyn_Fallthru_s: 0){ _LL2618: _temp2617=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2523)->f1; goto _LL2616; _LL2616: _temp2615=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2523)->f2; goto _LL2548;} else{ goto _LL2549;} _LL2549: if(( unsigned int)
_temp2523 > 1u?*(( int*) _temp2523) == Cyc_Absyn_Decl_s: 0){ _LL2622: _temp2621=((
struct Cyc_Absyn_Decl_s_struct*) _temp2523)->f1; goto _LL2620; _LL2620:
_temp2619=(( struct Cyc_Absyn_Decl_s_struct*) _temp2523)->f2; goto _LL2550;}
else{ goto _LL2551;} _LL2551: if(( unsigned int) _temp2523 > 1u?*(( int*)
_temp2523) == Cyc_Absyn_Label_s: 0){ _LL2626: _temp2625=(( struct Cyc_Absyn_Label_s_struct*)
_temp2523)->f1; goto _LL2624; _LL2624: _temp2623=(( struct Cyc_Absyn_Label_s_struct*)
_temp2523)->f2; goto _LL2552;} else{ goto _LL2553;} _LL2553: if(( unsigned int)
_temp2523 > 1u?*(( int*) _temp2523) == Cyc_Absyn_Do_s: 0){ _LL2634: _temp2633=((
struct Cyc_Absyn_Do_s_struct*) _temp2523)->f1; goto _LL2628; _LL2628: _temp2627=((
struct Cyc_Absyn_Do_s_struct*) _temp2523)->f2; _LL2632: _temp2631= _temp2627.f1;
goto _LL2630; _LL2630: _temp2629= _temp2627.f2; goto _LL2554;} else{ goto
_LL2555;} _LL2555: if(( unsigned int) _temp2523 > 1u?*(( int*) _temp2523) == Cyc_Absyn_TryCatch_s:
0){ _LL2638: _temp2637=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2523)->f1;
goto _LL2636; _LL2636: _temp2635=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2523)->f2; goto _LL2556;} else{ goto _LL2557;} _LL2557: if(( unsigned int)
_temp2523 > 1u?*(( int*) _temp2523) == Cyc_Absyn_Region_s: 0){ _LL2644:
_temp2643=(( struct Cyc_Absyn_Region_s_struct*) _temp2523)->f1; goto _LL2642;
_LL2642: _temp2641=(( struct Cyc_Absyn_Region_s_struct*) _temp2523)->f2; goto
_LL2640; _LL2640: _temp2639=(( struct Cyc_Absyn_Region_s_struct*) _temp2523)->f3;
goto _LL2558;} else{ goto _LL2559;} _LL2559: if(( unsigned int) _temp2523 > 1u?*((
int*) _temp2523) == Cyc_Absyn_SwitchC_s: 0){ _LL2648: _temp2647=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2523)->f1; goto _LL2646; _LL2646: _temp2645=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2523)->f2; goto _LL2560;} else{ goto _LL2561;} _LL2561: if(( unsigned int)
_temp2523 > 1u?*(( int*) _temp2523) == Cyc_Absyn_Cut_s: 0){ _LL2650: _temp2649=((
struct Cyc_Absyn_Cut_s_struct*) _temp2523)->f1; goto _LL2562;} else{ goto
_LL2563;} _LL2563: if(( unsigned int) _temp2523 > 1u?*(( int*) _temp2523) == Cyc_Absyn_Splice_s:
0){ _LL2652: _temp2651=(( struct Cyc_Absyn_Splice_s_struct*) _temp2523)->f1;
goto _LL2564;} else{ goto _LL2524;} _LL2526: return; _LL2528: Cyc_Toc_exp_to_c(
nv, _temp2565); return; _LL2530: Cyc_Toc_stmt_to_c( nv, _temp2569); s= _temp2567;
continue; _LL2532: { struct Cyc_Core_Opt* topt= 0; if( _temp2571 != 0){ topt=({
struct Cyc_Core_Opt* _temp2653=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2653->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2571))->topt))->v);
_temp2653;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2571));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2654=
Cyc_Toc_temp_var(); goto _LL2655; _LL2655: { struct Cyc_Absyn_Stmt* _temp2656=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2654, 0),
0); goto _LL2657; _LL2657:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2654,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2571,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2656, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2534: Cyc_Toc_exp_to_c(
nv, _temp2577); Cyc_Toc_stmt_to_c( nv, _temp2575); s= _temp2573; continue;
_LL2536: Cyc_Toc_exp_to_c( nv, _temp2585); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2579); return; _LL2538: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2587 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2587))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2540: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -(( struct Cyc_Absyn_Stmt*) _check_null( _temp2589))->try_depth, s);
return; _LL2542: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2591))->try_depth, s); return; _LL2544: Cyc_Toc_exp_to_c( nv,
_temp2609); Cyc_Toc_exp_to_c( nv, _temp2607); Cyc_Toc_exp_to_c( nv, _temp2601);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2595); return; _LL2546: Cyc_Toc_xlate_switch(
nv, s, _temp2613, _temp2611); return; _LL2548: if( nv->fallthru_info == 0){(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2658=( unsigned char*)"fallthru in unexpected place"; struct
_tagged_string _temp2659; _temp2659.curr= _temp2658; _temp2659.base= _temp2658;
_temp2659.last_plus_one= _temp2658 + 29; _temp2659;})));}{ struct _tuple8
_temp2662; struct Cyc_Dict_Dict* _temp2663; struct Cyc_List_List* _temp2665;
struct _tagged_string* _temp2667; struct _tuple8* _temp2660=( struct _tuple8*)((
struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp2662=* _temp2660;
_LL2668: _temp2667= _temp2662.f1; goto _LL2666; _LL2666: _temp2665= _temp2662.f2;
goto _LL2664; _LL2664: _temp2663= _temp2662.f3; goto _LL2661; _LL2661: { struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2667, 0); Cyc_Toc_do_npop_before(
s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp2615)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp2669=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2665); goto _LL2670; _LL2670: { struct Cyc_List_List*
_temp2671=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2617); goto _LL2672; _LL2672: for( 0; _temp2669 != 0;( _temp2669=(( struct
Cyc_List_List*) _check_null( _temp2669))->tl, _temp2671=(( struct Cyc_List_List*)
_check_null( _temp2671))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp2671))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp2663,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp2669))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp2671))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}}} _LL2550:{ void* _temp2673=( void*) _temp2621->r; int
_temp2683; struct Cyc_Absyn_Exp* _temp2685; struct Cyc_Core_Opt* _temp2687;
struct Cyc_Core_Opt* _temp2689; struct Cyc_Absyn_Pat* _temp2691; struct Cyc_List_List*
_temp2693; struct Cyc_Absyn_Vardecl* _temp2695; _LL2675: if(*(( int*) _temp2673)
== Cyc_Absyn_Let_d){ _LL2692: _temp2691=(( struct Cyc_Absyn_Let_d_struct*)
_temp2673)->f1; goto _LL2690; _LL2690: _temp2689=(( struct Cyc_Absyn_Let_d_struct*)
_temp2673)->f2; goto _LL2688; _LL2688: _temp2687=(( struct Cyc_Absyn_Let_d_struct*)
_temp2673)->f3; goto _LL2686; _LL2686: _temp2685=(( struct Cyc_Absyn_Let_d_struct*)
_temp2673)->f4; goto _LL2684; _LL2684: _temp2683=(( struct Cyc_Absyn_Let_d_struct*)
_temp2673)->f5; goto _LL2676;} else{ goto _LL2677;} _LL2677: if(*(( int*)
_temp2673) == Cyc_Absyn_Letv_d){ _LL2694: _temp2693=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2673)->f1; goto _LL2678;} else{ goto _LL2679;} _LL2679: if(*(( int*)
_temp2673) == Cyc_Absyn_Var_d){ _LL2696: _temp2695=(( struct Cyc_Absyn_Var_d_struct*)
_temp2673)->f1; goto _LL2680;} else{ goto _LL2681;} _LL2681: goto _LL2682;
_LL2676:( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2691,
_temp2687, _temp2685, _temp2683, _temp2619))->r)); goto _LL2674; _LL2678: {
struct Cyc_List_List* _temp2697=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2693); goto _LL2698; _LL2698: if( _temp2697 == 0){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2699=(
unsigned char*)"empty Letv_d"; struct _tagged_string _temp2700; _temp2700.curr=
_temp2699; _temp2700.base= _temp2699; _temp2700.last_plus_one= _temp2699 + 13;
_temp2700;}));}( void*)( _temp2621->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2701=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2701[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2702; _temp2702.tag= Cyc_Absyn_Var_d;
_temp2702.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2697))->hd; _temp2702;}); _temp2701;}))); _temp2697=(( struct Cyc_List_List*)
_check_null( _temp2697))->tl; for( 0; _temp2697 != 0; _temp2697=(( struct Cyc_List_List*)
_check_null( _temp2697))->tl){ struct Cyc_Absyn_Decl* _temp2705= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2703=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2703[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2704; _temp2704.tag= Cyc_Absyn_Var_d; _temp2704.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2697))->hd; _temp2704;}); _temp2703;}),
0); goto _LL2706; _LL2706:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp2705, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c(
nv, s); goto _LL2674;} _LL2680: { struct _tuple0* _temp2707= _temp2695->name;
goto _LL2708; _LL2708:( void*)( _temp2695->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2695->type));{ struct Cyc_Toc_Env* _temp2711= Cyc_Toc_add_varmap( nv,
_temp2707, Cyc_Absyn_varb_exp( _temp2707,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2709=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2709[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2710; _temp2710.tag= Cyc_Absyn_Local_b;
_temp2710.f1= _temp2695; _temp2710;}); _temp2709;}), 0)); goto _LL2712; _LL2712:
Cyc_Toc_stmt_to_c( _temp2711, _temp2619); if( _temp2695->initializer != 0){
struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*) _check_null( _temp2695->initializer);
void* _temp2713=( void*) init->r; struct Cyc_Absyn_Exp* _temp2719; struct Cyc_Absyn_Exp*
_temp2721; struct Cyc_Absyn_Vardecl* _temp2723; _LL2715: if(*(( int*) _temp2713)
== Cyc_Absyn_Comprehension_e){ _LL2724: _temp2723=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2713)->f1; goto _LL2722; _LL2722: _temp2721=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2713)->f2; goto _LL2720; _LL2720: _temp2719=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2713)->f3; goto _LL2716;} else{ goto _LL2717;} _LL2717: goto _LL2718;
_LL2716: _temp2695->initializer= 0;( void*)( _temp2619->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2707, 0), _temp2723, _temp2721, _temp2719, Cyc_Absyn_new_stmt((
void*) _temp2619->r, 0), 0))->r)); goto _LL2714; _LL2718: Cyc_Toc_exp_to_c( nv,
init); goto _LL2714; _LL2714:;} goto _LL2674;}} _LL2682:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2725=( unsigned char*)"bad nested declaration within function";
struct _tagged_string _temp2726; _temp2726.curr= _temp2725; _temp2726.base=
_temp2725; _temp2726.last_plus_one= _temp2725 + 39; _temp2726;})); goto _LL2674;
_LL2674:;} return; _LL2552: s= _temp2623; continue; _LL2554: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp2633); Cyc_Toc_exp_to_c( nv, _temp2631); return;
_LL2556: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__push_handler_qv,
0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp( Cyc_Toc_setjmp_qv, 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__pop_handler_qv,
0); struct Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp( Cyc_Toc__exn_thrown_qv,
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2727=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2727->v=( void*) e_typ; _temp2727;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2637);{
struct Cyc_Absyn_Stmt* _temp2728= Cyc_Absyn_seq_stmt( _temp2637, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); goto _LL2729; _LL2729: {
struct _tuple0* _temp2730= Cyc_Toc_temp_var(); goto _LL2731; _LL2731: { struct
Cyc_Absyn_Exp* _temp2732= Cyc_Absyn_var_exp( _temp2730, 0); goto _LL2733;
_LL2733: { struct Cyc_Absyn_Vardecl* _temp2734= Cyc_Absyn_new_vardecl( _temp2730,
Cyc_Absyn_exn_typ, 0); goto _LL2735; _LL2735: _temp2732->topt=({ struct Cyc_Core_Opt*
_temp2736=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2736->v=( void*) Cyc_Absyn_exn_typ; _temp2736;});{ struct Cyc_Absyn_Pat*
_temp2741=({ struct Cyc_Absyn_Pat* _temp2737=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2737->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2739=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2739[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2740; _temp2740.tag= Cyc_Absyn_Var_p;
_temp2740.f1= _temp2734; _temp2740;}); _temp2739;})); _temp2737->topt=({ struct
Cyc_Core_Opt* _temp2738=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2738->v=( void*) Cyc_Absyn_exn_typ; _temp2738;}); _temp2737->loc= 0;
_temp2737;}); goto _LL2742; _LL2742: { struct Cyc_Absyn_Exp* _temp2743= Cyc_Absyn_throw_exp(
_temp2732, 0); goto _LL2744; _LL2744: _temp2743->topt=({ struct Cyc_Core_Opt*
_temp2745=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2745->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2745;});{ struct Cyc_Absyn_Stmt*
_temp2746= Cyc_Absyn_exp_stmt( _temp2743, 0); goto _LL2747; _LL2747: { struct
Cyc_Absyn_Switch_clause* _temp2751=({ struct Cyc_Absyn_Switch_clause* _temp2748=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2748->pattern= _temp2741; _temp2748->pat_vars=({ struct Cyc_Core_Opt*
_temp2749=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2749->v=( void*)({ struct Cyc_List_List* _temp2750=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2750->hd=( void*) _temp2734;
_temp2750->tl= 0; _temp2750;}); _temp2749;}); _temp2748->where_clause= 0;
_temp2748->body= _temp2746; _temp2748->loc= 0; _temp2748;}); goto _LL2752;
_LL2752: { struct Cyc_Absyn_Stmt* _temp2754= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2635,({ struct Cyc_List_List* _temp2753=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2753->hd=( void*) _temp2751; _temp2753->tl=
0; _temp2753;})), 0); goto _LL2755; _LL2755: Cyc_Toc_stmt_to_c( nv, _temp2754);{
struct Cyc_Absyn_Exp* _temp2757= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2756=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2756->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2756->tl= 0; _temp2756;}), 0); goto _LL2758; _LL2758: { struct Cyc_Absyn_Stmt*
_temp2760= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2759=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2759->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2759->tl= 0;
_temp2759;}), 0), 0); goto _LL2761; _LL2761: { struct Cyc_Absyn_Exp* _temp2762=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2763; _LL2763: {
struct Cyc_Absyn_Exp* _temp2764= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2765; _LL2765:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2760, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2762, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2757, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2764, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2728, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2754, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2558: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2641->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2771=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2771[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2772; _temp2772.tag= Cyc_Absyn_Local_b; _temp2772.f1=
_temp2641; _temp2772;}); _temp2771;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2770=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2770->f1= Cyc_Toc_abs_ns; _temp2770->f2= Cyc_Toc__push_region_sp;
_temp2770;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2769=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2769->f1= Cyc_Toc_abs_ns; _temp2769->f2= Cyc_Toc__pop_region_sp; _temp2769;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2768=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2768->f1=
Cyc_Toc_abs_ns; _temp2768->f2= Cyc_Toc__new_region_sp; _temp2768;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2767=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2767->hd=( void*) addr_rh_exp; _temp2767->tl= 0; _temp2767;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2766=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2766->hd=( void*) x_exp; _temp2766->tl= 0; _temp2766;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2639);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2639, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2560:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2773=(
unsigned char*)"switchC_s"; struct _tagged_string _temp2774; _temp2774.curr=
_temp2773; _temp2774.base= _temp2773; _temp2774.last_plus_one= _temp2773 + 10;
_temp2774;})); return; _LL2562:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2775=( unsigned char*)"cut"; struct _tagged_string _temp2776;
_temp2776.curr= _temp2775; _temp2776.base= _temp2775; _temp2776.last_plus_one=
_temp2775 + 4; _temp2776;})); return; _LL2564:(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(({ unsigned char* _temp2777=( unsigned char*)"splice"; struct
_tagged_string _temp2778; _temp2778.curr= _temp2777; _temp2778.base= _temp2777;
_temp2778.last_plus_one= _temp2777 + 7; _temp2778;})); return; _LL2524:;}}
struct _tuple17{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp2779= f->args; goto _LL2780;
_LL2780: for( 0; _temp2779 != 0; _temp2779=(( struct Cyc_List_List*) _check_null(
_temp2779))->tl){(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2779))->hd)).f3= Cyc_Toc_typ_to_c((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp2779))->hd)).f3);{ struct _tuple0* _temp2782=({ struct _tuple0*
_temp2781=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2781->f1=(
void*) Cyc_Absyn_Loc_n; _temp2781->f2=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp2779))->hd)).f1; _temp2781;}); goto _LL2783; _LL2783: nv= Cyc_Toc_add_varmap(
nv, _temp2782, Cyc_Absyn_var_exp( _temp2782, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2786; void* _temp2788; void* _temp2790; struct Cyc_Absyn_Tqual _temp2792;
struct Cyc_Core_Opt* _temp2794; struct Cyc_Absyn_VarargInfo _temp2784=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2795: _temp2794=
_temp2784.name; goto _LL2793; _LL2793: _temp2792= _temp2784.tq; goto _LL2791;
_LL2791: _temp2790=( void*) _temp2784.type; goto _LL2789; _LL2789: _temp2788=(
void*) _temp2784.rgn; goto _LL2787; _LL2787: _temp2786= _temp2784.inject; goto
_LL2785; _LL2785: { void* _temp2796= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2790, _temp2788, _temp2792)); goto _LL2797; _LL2797: { struct _tuple0*
_temp2799=({ struct _tuple0* _temp2798=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2798->f1=( void*) Cyc_Absyn_Loc_n; _temp2798->f2=( struct
_tagged_string*)(( struct Cyc_Core_Opt*) _check_null( _temp2794))->v; _temp2798;});
goto _LL2800; _LL2800: f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2801=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2801->hd=( void*)({ struct _tuple17* _temp2802=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp2802->f1=( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( _temp2794))->v; _temp2802->f2= _temp2792;
_temp2802->f3= _temp2796; _temp2802;}); _temp2801->tl= 0; _temp2801;})); nv= Cyc_Toc_add_varmap(
nv, _temp2799, Cyc_Absyn_var_exp( _temp2799, 0)); f->cyc_varargs= 0;}}}{ struct
Cyc_List_List* _temp2803=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; goto _LL2804; _LL2804: for( 0; _temp2803 !=
0; _temp2803=(( struct Cyc_List_List*) _check_null( _temp2803))->tl){( void*)(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp2803))->hd)->type=(
void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp2803))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2805= s;
_LL2807: if( _temp2805 ==( void*) Cyc_Absyn_Abstract){ goto _LL2808;} else{ goto
_LL2809;} _LL2809: if( _temp2805 ==( void*) Cyc_Absyn_ExternC){ goto _LL2810;}
else{ goto _LL2811;} _LL2811: goto _LL2812; _LL2808: return( void*) Cyc_Absyn_Public;
_LL2810: return( void*) Cyc_Absyn_Extern; _LL2812: return s; _LL2806:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2813=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2814; _LL2814: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2815=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2815->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2815;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2816=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2813).f2); goto _LL2817;
_LL2817: if( _temp2816 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2813).f2, s));}
else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null(
_temp2816))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2813).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2818=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2818->v=( void*)
_temp2813; _temp2818;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2819=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2820; _LL2820: for( 0; _temp2819 != 0;
_temp2819=(( struct Cyc_List_List*) _check_null( _temp2819))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2819))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2819))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2821=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2822; _LL2822: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2823=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2823->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2823;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2824=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2821).f2); goto _LL2825; _LL2825:
if( _temp2824 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2821).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2824))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2821).f2, u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)(
u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==(
void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*)
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2826=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2826->v=( void*) _temp2821; _temp2826;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2827=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( u->fields))->v; goto
_LL2828; _LL2828: for( 0; _temp2827 != 0; _temp2827=(( struct Cyc_List_List*)
_check_null( _temp2827))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2827))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2827))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2829=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2829->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2829;});}{ struct
_tuple0* _temp2830= tud->name; goto _LL2831; _LL2831: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2830)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2830));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2832=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2833; _LL2833: for( 0;
_temp2832 != 0; _temp2832=(( struct Cyc_List_List*) _check_null( _temp2832))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2832))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2834; _temp2834.q_const= 1; _temp2834.q_volatile= 0;
_temp2834.q_restrict= 0; _temp2834;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2835=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2835->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2836=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2836[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2837; _temp2837.tag= Cyc_Absyn_Var_d;
_temp2837.f1= vd; _temp2837;}); _temp2836;}), 0); _temp2835->tl= Cyc_Toc_result_decls;
_temp2835;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2838; _temp2838.q_const=
1; _temp2838.q_volatile= 0; _temp2838.q_restrict= 0; _temp2838;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2839=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2839->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2840=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2840[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2841; _temp2841.tag= Cyc_Absyn_Var_d; _temp2841.f1= vd; _temp2841;});
_temp2840;}), 0); _temp2839->tl= Cyc_Toc_result_decls; _temp2839;});{ struct Cyc_List_List*
_temp2842= 0; goto _LL2843; _LL2843: { int i= 1;{ struct Cyc_List_List*
_temp2844= f->typs; goto _LL2845; _LL2845: for( 0; _temp2844 != 0;( _temp2844=((
struct Cyc_List_List*) _check_null( _temp2844))->tl, i ++)){ struct
_tagged_string* _temp2846= Cyc_Toc_fieldname( i); goto _LL2847; _LL2847: {
struct Cyc_Absyn_Structfield* _temp2849=({ struct Cyc_Absyn_Structfield*
_temp2848=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2848->name= _temp2846; _temp2848->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2844))->hd)).f1; _temp2848->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2844))->hd)).f2);
_temp2848->width= 0; _temp2848->attributes= 0; _temp2848;}); goto _LL2850;
_LL2850: _temp2842=({ struct Cyc_List_List* _temp2851=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2851->hd=( void*) _temp2849;
_temp2851->tl= _temp2842; _temp2851;});}}} _temp2842=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2842); _temp2842=({ struct Cyc_List_List*
_temp2852=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2852->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2853=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2853->name= Cyc_Toc_tag_sp;
_temp2853->tq= Cyc_Toc_mt_tq; _temp2853->type=( void*) Cyc_Absyn_sint_t;
_temp2853->width= 0; _temp2853->attributes= 0; _temp2853;}); _temp2852->tl=
_temp2842; _temp2852;});{ struct Cyc_Absyn_Structdecl* _temp2859=({ struct Cyc_Absyn_Structdecl*
_temp2854=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2854->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2854->name=({ struct Cyc_Core_Opt*
_temp2856=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2856->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2857=( unsigned char*)"_struct"; struct _tagged_string _temp2858; _temp2858.curr=
_temp2857; _temp2858.base= _temp2857; _temp2858.last_plus_one= _temp2857 + 8;
_temp2858;})); _temp2856;}); _temp2854->tvs= 0; _temp2854->fields=({ struct Cyc_Core_Opt*
_temp2855=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2855->v=( void*) _temp2842; _temp2855;}); _temp2854->attributes= 0;
_temp2854;}); goto _LL2860; _LL2860: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2861=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2861->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2862=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2862[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2863; _temp2863.tag= Cyc_Absyn_Struct_d;
_temp2863.f1= _temp2859; _temp2863;}); _temp2862;}), 0); _temp2861->tl= Cyc_Toc_result_decls;
_temp2861;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2864=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2864->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2864;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2865= xd->name; goto _LL2866; _LL2866: {
struct Cyc_List_List* _temp2867=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2868; _LL2868: for( 0; _temp2867 != 0;
_temp2867=(( struct Cyc_List_List*) _check_null( _temp2867))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2867))->hd; struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2870= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2869=* fn;(
unsigned int)( _temp2869.last_plus_one - _temp2869.curr);}) + 4, 0); goto
_LL2871; _LL2871: { struct Cyc_Absyn_ArrayType_struct* _temp2874=({ struct Cyc_Absyn_ArrayType_struct*
_temp2872=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2872[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2873; _temp2873.tag= Cyc_Absyn_ArrayType;
_temp2873.f1=( void*) Cyc_Absyn_uchar_t; _temp2873.f2= Cyc_Toc_mt_tq; _temp2873.f3=(
struct Cyc_Absyn_Exp*) _temp2870; _temp2873;}); _temp2872;}); goto _LL2875;
_LL2875: { struct Cyc_Core_Opt* _temp2876=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2884; int _temp2885; _LL2878: if( _temp2876 == 0){ goto
_LL2879;} else{ goto _LL2880;} _LL2880: if( _temp2876 == 0){ goto _LL2882;}
else{ _temp2884=* _temp2876; _LL2886: _temp2885=( int) _temp2884.v; if(
_temp2885 == 0){ goto _LL2881;} else{ goto _LL2882;}} _LL2882: goto _LL2883;
_LL2879: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_string)({
int _temp2887= 0; int _temp2888= 0; int _temp2889= 0; int _temp2890= 0; struct
_tagged_string _temp2891=* fn; xprintf("%c%c%c%c%.*s", _temp2887, _temp2888,
_temp2889, _temp2890, _temp2891.last_plus_one - _temp2891.curr, _temp2891.curr);}),
0);}{ struct Cyc_Absyn_Vardecl* _temp2892= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2874, initopt); goto _LL2893; _LL2893:( void*)( _temp2892->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2894=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2894->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2895=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2895[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2896; _temp2896.tag= Cyc_Absyn_Var_d;
_temp2896.f1= _temp2892; _temp2896;}); _temp2895;}), 0); _temp2894->tl= Cyc_Toc_result_decls;
_temp2894;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2897= f->typs; goto _LL2898; _LL2898: for( 0;
_temp2897 != 0;( _temp2897=(( struct Cyc_List_List*) _check_null( _temp2897))->tl,
i ++)){ struct _tagged_string* _temp2900=({ struct _tagged_string* _temp2899=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp2899[ 0]=(
struct _tagged_string) xprintf("f%d", i); _temp2899;}); goto _LL2901; _LL2901: {
struct Cyc_Absyn_Structfield* _temp2903=({ struct Cyc_Absyn_Structfield*
_temp2902=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2902->name= _temp2900; _temp2902->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2897))->hd)).f1; _temp2902->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2897))->hd)).f2);
_temp2902->width= 0; _temp2902->attributes= 0; _temp2902;}); goto _LL2904;
_LL2904: fields=({ struct Cyc_List_List* _temp2905=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2905->hd=( void*) _temp2903;
_temp2905->tl= fields; _temp2905;});}}} fields=({ struct Cyc_List_List*
_temp2906=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2906->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2907=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2907->name= Cyc_Toc_tag_sp;
_temp2907->tq= Cyc_Toc_mt_tq; _temp2907->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2907->width= 0; _temp2907->attributes= 0; _temp2907;});
_temp2906->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2906;});{ struct Cyc_Absyn_Structdecl* _temp2913=({ struct Cyc_Absyn_Structdecl*
_temp2908=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2908->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2908->name=({ struct Cyc_Core_Opt*
_temp2910=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2910->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2911=( unsigned char*)"_struct"; struct _tagged_string _temp2912; _temp2912.curr=
_temp2911; _temp2912.base= _temp2911; _temp2912.last_plus_one= _temp2911 + 8;
_temp2912;})); _temp2910;}); _temp2908->tvs= 0; _temp2908->fields=({ struct Cyc_Core_Opt*
_temp2909=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2909->v=( void*) fields; _temp2909;}); _temp2908->attributes= 0; _temp2908;});
goto _LL2914; _LL2914: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2915=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2915->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2916=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2916[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2917; _temp2917.tag= Cyc_Absyn_Struct_d;
_temp2917.f1= _temp2913; _temp2917;}); _temp2916;}), 0); _temp2915->tl= Cyc_Toc_result_decls;
_temp2915;});}} goto _LL2877;}} _LL2881: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2923= Cyc_Absyn_string_exp(( struct _tagged_string)({
int _temp2918= 0; int _temp2919= 0; int _temp2920= 0; int _temp2921= 0; struct
_tagged_string _temp2922=* fn; xprintf("%c%c%c%c%.*s", _temp2918, _temp2919,
_temp2920, _temp2921, _temp2922.last_plus_one - _temp2922.curr, _temp2922.curr);}),
0); goto _LL2924; _LL2924: { struct Cyc_Absyn_Vardecl* _temp2925= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2874,( struct Cyc_Absyn_Exp*) _temp2923); goto _LL2926;
_LL2926:( void*)( _temp2925->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2927=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2927->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2928=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2928[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2929; _temp2929.tag= Cyc_Absyn_Var_d; _temp2929.f1= _temp2925; _temp2929;});
_temp2928;}), 0); _temp2927->tl= Cyc_Toc_result_decls; _temp2927;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2877; _LL2883: goto
_LL2877; _LL2877:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2930= ed->name; goto
_LL2931; _LL2931:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2932=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2933; _LL2933: for( 0; _temp2932 != 0;
_temp2932=(( struct Cyc_List_List*) _check_null( _temp2932))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2932))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2932))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2932))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct _RegionHandle
_temp2934= _new_region(); struct _RegionHandle* prgn=& _temp2934; _push_region(
prgn);{ struct Cyc_Absyn_Stmt* _temp2937; struct Cyc_List_List* _temp2939;
struct Cyc_Toc_Env* _temp2941; struct _tuple13 _temp2935= Cyc_Toc_xlate_pat( nv,
prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2942: _temp2941= _temp2935.f1; goto _LL2940; _LL2940: _temp2939=
_temp2935.f2; goto _LL2938; _LL2938: _temp2937= _temp2935.f3; goto _LL2936;
_LL2936: Cyc_Toc_stmt_to_c( _temp2941, s);{ struct Cyc_Absyn_Stmt* _temp2943=
Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL2944; _LL2944: if( exhaust){ s=
Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2937, _temp2943, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2946= Cyc_Toc_newthrow_exp(
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2945=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2945->f1= Cyc_Toc_abs_ns; _temp2945->f2= Cyc_Toc_Match_Exception_sp;
_temp2945;}), 0)); goto _LL2947; _LL2947: { struct Cyc_Absyn_Stmt* _temp2948=
Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2946, 0), 0); goto
_LL2949; _LL2949: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2937, Cyc_Absyn_seq_stmt( _temp2948,
_temp2943, 0), 0), 0);}} for( 0; _temp2939 != 0; _temp2939=(( struct Cyc_List_List*)
_check_null( _temp2939))->tl){ struct _tuple14 _temp2952; void* _temp2953;
struct _tuple0* _temp2955; struct _tuple14* _temp2950=( struct _tuple14*)((
struct Cyc_List_List*) _check_null( _temp2939))->hd; _temp2952=* _temp2950;
_LL2956: _temp2955= _temp2952.f1; goto _LL2954; _LL2954: _temp2953= _temp2952.f2;
goto _LL2951; _LL2951: s= Cyc_Absyn_declare_stmt( _temp2955, _temp2953, 0, s, 0);}}};
_pop_region(& _temp2934);} return s;}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=((
struct Cyc_List_List*) _check_null( ds))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2957=(
void*) d->r; struct Cyc_Absyn_Vardecl* _temp2983; struct Cyc_Absyn_Fndecl*
_temp2985; int _temp2987; struct Cyc_Absyn_Exp* _temp2989; struct Cyc_Core_Opt*
_temp2991; struct Cyc_Core_Opt* _temp2993; struct Cyc_Absyn_Pat* _temp2995;
struct Cyc_List_List* _temp2997; struct Cyc_Absyn_Structdecl* _temp2999; struct
Cyc_Absyn_Uniondecl* _temp3001; struct Cyc_Absyn_Tuniondecl* _temp3003; struct
Cyc_Absyn_Enumdecl* _temp3005; struct Cyc_Absyn_Typedefdecl* _temp3007; struct
Cyc_List_List* _temp3009; struct _tagged_string* _temp3011; struct Cyc_List_List*
_temp3013; struct _tuple0* _temp3015; struct Cyc_List_List* _temp3017; _LL2959:
if(*(( int*) _temp2957) == Cyc_Absyn_Var_d){ _LL2984: _temp2983=(( struct Cyc_Absyn_Var_d_struct*)
_temp2957)->f1; goto _LL2960;} else{ goto _LL2961;} _LL2961: if(*(( int*)
_temp2957) == Cyc_Absyn_Fn_d){ _LL2986: _temp2985=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2957)->f1; goto _LL2962;} else{ goto _LL2963;} _LL2963: if(*(( int*)
_temp2957) == Cyc_Absyn_Let_d){ _LL2996: _temp2995=(( struct Cyc_Absyn_Let_d_struct*)
_temp2957)->f1; goto _LL2994; _LL2994: _temp2993=(( struct Cyc_Absyn_Let_d_struct*)
_temp2957)->f2; goto _LL2992; _LL2992: _temp2991=(( struct Cyc_Absyn_Let_d_struct*)
_temp2957)->f3; goto _LL2990; _LL2990: _temp2989=(( struct Cyc_Absyn_Let_d_struct*)
_temp2957)->f4; goto _LL2988; _LL2988: _temp2987=(( struct Cyc_Absyn_Let_d_struct*)
_temp2957)->f5; goto _LL2964;} else{ goto _LL2965;} _LL2965: if(*(( int*)
_temp2957) == Cyc_Absyn_Letv_d){ _LL2998: _temp2997=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2957)->f1; goto _LL2966;} else{ goto _LL2967;} _LL2967: if(*(( int*)
_temp2957) == Cyc_Absyn_Struct_d){ _LL3000: _temp2999=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2957)->f1; goto _LL2968;} else{ goto _LL2969;} _LL2969: if(*(( int*)
_temp2957) == Cyc_Absyn_Union_d){ _LL3002: _temp3001=(( struct Cyc_Absyn_Union_d_struct*)
_temp2957)->f1; goto _LL2970;} else{ goto _LL2971;} _LL2971: if(*(( int*)
_temp2957) == Cyc_Absyn_Tunion_d){ _LL3004: _temp3003=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2957)->f1; goto _LL2972;} else{ goto _LL2973;} _LL2973: if(*(( int*)
_temp2957) == Cyc_Absyn_Enum_d){ _LL3006: _temp3005=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2957)->f1; goto _LL2974;} else{ goto _LL2975;} _LL2975: if(*(( int*)
_temp2957) == Cyc_Absyn_Typedef_d){ _LL3008: _temp3007=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2957)->f1; goto _LL2976;} else{ goto _LL2977;} _LL2977: if(*(( int*)
_temp2957) == Cyc_Absyn_Namespace_d){ _LL3012: _temp3011=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2957)->f1; goto _LL3010; _LL3010: _temp3009=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2957)->f2; goto _LL2978;} else{ goto _LL2979;} _LL2979: if(*(( int*)
_temp2957) == Cyc_Absyn_Using_d){ _LL3016: _temp3015=(( struct Cyc_Absyn_Using_d_struct*)
_temp2957)->f1; goto _LL3014; _LL3014: _temp3013=(( struct Cyc_Absyn_Using_d_struct*)
_temp2957)->f2; goto _LL2980;} else{ goto _LL2981;} _LL2981: if(*(( int*)
_temp2957) == Cyc_Absyn_ExternC_d){ _LL3018: _temp3017=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2957)->f1; goto _LL2982;} else{ goto _LL2958;} _LL2960: { struct _tuple0*
_temp3019= _temp2983->name; goto _LL3020; _LL3020: if(( void*) _temp2983->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3019=({ struct _tuple0* _temp3021=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3021->f1= Cyc_Toc_abs_ns;
_temp3021->f2=(* _temp3019).f2; _temp3021;});} if( _temp2983->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null( _temp2983->initializer));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp3022=( unsigned char*)"decls_to_c: not at toplevel!"; struct
_tagged_string _temp3023; _temp3023.curr= _temp3022; _temp3023.base= _temp3022;
_temp3023.last_plus_one= _temp3022 + 29; _temp3023;}));} nv= Cyc_Toc_add_varmap(
nv, _temp2983->name, Cyc_Absyn_varb_exp( _temp3019,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp3024=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp3024[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp3025; _temp3025.tag= Cyc_Absyn_Global_b;
_temp3025.f1= _temp2983; _temp3025;}); _temp3024;}), 0)); _temp2983->name=
_temp3019;( void*)( _temp2983->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2983->sc));(
void*)( _temp2983->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2983->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3026=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3026->hd=( void*) d; _temp3026->tl=
Cyc_Toc_result_decls; _temp3026;}); goto _LL2958;} _LL2962: { struct _tuple0*
_temp3027= _temp2985->name; goto _LL3028; _LL3028: if(( void*) _temp2985->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3027=({ struct _tuple0* _temp3029=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3029->f1= Cyc_Toc_abs_ns;
_temp3029->f2=(* _temp3027).f2; _temp3029;});} nv= Cyc_Toc_add_varmap( nv,
_temp2985->name, Cyc_Absyn_var_exp( _temp3027, 0)); _temp2985->name= _temp3027;
Cyc_Toc_fndecl_to_c( nv, _temp2985); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3030=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3030->hd=( void*) d; _temp3030->tl= Cyc_Toc_result_decls; _temp3030;});
goto _LL2958;} _LL2964:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp3031=( unsigned char*)"letdecl"; struct _tagged_string
_temp3032; _temp3032.curr= _temp3031; _temp3032.base= _temp3031; _temp3032.last_plus_one=
_temp3031 + 8; _temp3032;})); goto _LL2958; _LL2966:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp3033=(
unsigned char*)"letdecl"; struct _tagged_string _temp3034; _temp3034.curr=
_temp3033; _temp3034.base= _temp3033; _temp3034.last_plus_one= _temp3033 + 8;
_temp3034;})); goto _LL2958; _LL2968: Cyc_Toc_structdecl_to_c( _temp2999); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3035=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3035->hd=( void*) d; _temp3035->tl= Cyc_Toc_result_decls;
_temp3035;}); goto _LL2958; _LL2970: Cyc_Toc_uniondecl_to_c( _temp3001); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3036=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3036->hd=( void*) d; _temp3036->tl= Cyc_Toc_result_decls;
_temp3036;}); goto _LL2958; _LL2972: if( _temp3003->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp3003);} else{ Cyc_Toc_tuniondecl_to_c( _temp3003);} goto _LL2958; _LL2974:
Cyc_Toc_enumdecl_to_c( nv, _temp3005); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3037=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3037->hd=( void*) d; _temp3037->tl= Cyc_Toc_result_decls; _temp3037;});
goto _LL2958; _LL2976: _temp3007->name= _temp3007->name; _temp3007->tvs= 0;(
void*)( _temp3007->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp3007->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3038=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3038->hd=( void*) d; _temp3038->tl=
Cyc_Toc_result_decls; _temp3038;}); goto _LL2958; _LL2978: _temp3013= _temp3009;
goto _LL2980; _LL2980: _temp3017= _temp3013; goto _LL2982; _LL2982: nv= Cyc_Toc_decls_to_c(
nv, _temp3017, top); goto _LL2958; _LL2958:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}