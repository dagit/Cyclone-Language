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
_temp433; struct Cyc_List_List* _temp435; struct Cyc_List_List* _temp437; struct
Cyc_Absyn_VarargInfo* _temp439; int _temp441; struct Cyc_List_List* _temp443;
void* _temp445; struct Cyc_Core_Opt* _temp447; struct Cyc_List_List* _temp449;
struct Cyc_List_List* _temp451; struct Cyc_List_List* _temp453; struct Cyc_List_List*
_temp455; struct Cyc_Absyn_Uniondecl** _temp457; struct Cyc_List_List* _temp459;
struct _tuple0* _temp461; struct Cyc_Absyn_Structdecl** _temp463; struct Cyc_List_List*
_temp465; struct _tuple0* _temp467; struct Cyc_Absyn_Enumdecl* _temp469; struct
_tuple0* _temp471; struct Cyc_Core_Opt* _temp473; struct Cyc_List_List* _temp475;
struct _tuple0* _temp477; void* _temp479; void* _temp481; struct Cyc_List_List*
_temp483; _LL316: if( _temp314 ==( void*) Cyc_Absyn_VoidType){ goto _LL317;}
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
_temp314)->f1; _LL450: _temp449= _temp433.tvars; goto _LL448; _LL448: _temp447=
_temp433.effect; goto _LL446; _LL446: _temp445=( void*) _temp433.ret_typ; goto
_LL444; _LL444: _temp443= _temp433.args; goto _LL442; _LL442: _temp441= _temp433.c_varargs;
goto _LL440; _LL440: _temp439= _temp433.cyc_varargs; goto _LL438; _LL438:
_temp437= _temp433.rgn_po; goto _LL436; _LL436: _temp435= _temp433.attributes;
goto _LL343;} else{ goto _LL344;} _LL344: if(( unsigned int) _temp314 > 4u?*((
int*) _temp314) == Cyc_Absyn_TupleType: 0){ _LL452: _temp451=(( struct Cyc_Absyn_TupleType_struct*)
_temp314)->f1; goto _LL345;} else{ goto _LL346;} _LL346: if(( unsigned int)
_temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_AnonStructType: 0){ _LL454:
_temp453=(( struct Cyc_Absyn_AnonStructType_struct*) _temp314)->f1; goto _LL347;}
else{ goto _LL348;} _LL348: if(( unsigned int) _temp314 > 4u?*(( int*) _temp314)
== Cyc_Absyn_AnonUnionType: 0){ _LL456: _temp455=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp314)->f1; goto _LL349;} else{ goto _LL350;} _LL350: if(( unsigned int)
_temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_UnionType: 0){ _LL462: _temp461=((
struct Cyc_Absyn_UnionType_struct*) _temp314)->f1; goto _LL460; _LL460: _temp459=((
struct Cyc_Absyn_UnionType_struct*) _temp314)->f2; goto _LL458; _LL458: _temp457=((
struct Cyc_Absyn_UnionType_struct*) _temp314)->f3; goto _LL351;} else{ goto
_LL352;} _LL352: if(( unsigned int) _temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_StructType:
0){ _LL468: _temp467=(( struct Cyc_Absyn_StructType_struct*) _temp314)->f1; goto
_LL466; _LL466: _temp465=(( struct Cyc_Absyn_StructType_struct*) _temp314)->f2;
goto _LL464; _LL464: _temp463=(( struct Cyc_Absyn_StructType_struct*) _temp314)->f3;
goto _LL353;} else{ goto _LL354;} _LL354: if(( unsigned int) _temp314 > 4u?*((
int*) _temp314) == Cyc_Absyn_EnumType: 0){ _LL472: _temp471=(( struct Cyc_Absyn_EnumType_struct*)
_temp314)->f1; goto _LL470; _LL470: _temp469=(( struct Cyc_Absyn_EnumType_struct*)
_temp314)->f2; goto _LL355;} else{ goto _LL356;} _LL356: if(( unsigned int)
_temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_TypedefType: 0){ _LL478: _temp477=((
struct Cyc_Absyn_TypedefType_struct*) _temp314)->f1; goto _LL476; _LL476:
_temp475=(( struct Cyc_Absyn_TypedefType_struct*) _temp314)->f2; goto _LL474;
_LL474: _temp473=(( struct Cyc_Absyn_TypedefType_struct*) _temp314)->f3; goto
_LL357;} else{ goto _LL358;} _LL358: if(( unsigned int) _temp314 > 4u?*(( int*)
_temp314) == Cyc_Absyn_RgnHandleType: 0){ _LL480: _temp479=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp314)->f1; goto _LL359;} else{ goto _LL360;} _LL360: if( _temp314 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL361;} else{ goto _LL362;} _LL362: if(( unsigned int)
_temp314 > 4u?*(( int*) _temp314) == Cyc_Absyn_AccessEff: 0){ _LL482: _temp481=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp314)->f1; goto _LL363;} else{
goto _LL364;} _LL364: if(( unsigned int) _temp314 > 4u?*(( int*) _temp314) ==
Cyc_Absyn_JoinEff: 0){ _LL484: _temp483=(( struct Cyc_Absyn_JoinEff_struct*)
_temp314)->f1; goto _LL365;} else{ goto _LL315;} _LL317: return t; _LL319:
return( void*) Cyc_Absyn_VoidType; _LL321: return Cyc_Toc_typ_to_c( _temp381);
_LL323: return Cyc_Absyn_void_star_typ(); _LL325: return Cyc_Absyn_void_star_typ();
_LL327:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp485=( unsigned char*)"unresolved TunionType"; struct
_tagged_string _temp486; _temp486.curr= _temp485; _temp486.base= _temp485;
_temp486.last_plus_one= _temp485 + 22; _temp486;}))); _LL329: if( _temp405->typs
== 0){ if( _temp407->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp405->name,({
unsigned char* _temp487=( unsigned char*)"_struct"; struct _tagged_string
_temp488; _temp488.curr= _temp487; _temp488.base= _temp487; _temp488.last_plus_one=
_temp487 + 8; _temp488;})));} _LL331:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp489=( unsigned char*)"unresolved TunionFieldType";
struct _tagged_string _temp490; _temp490.curr= _temp489; _temp490.base= _temp489;
_temp490.last_plus_one= _temp489 + 27; _temp490;}))); _LL333: _temp421= Cyc_Toc_typ_to_c_array(
_temp421);{ void* _temp491=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp413))->v; void* _temp497; _LL493: if((
unsigned int) _temp491 > 1u?*(( int*) _temp491) == Cyc_Absyn_Eq_constr: 0){
_LL498: _temp497=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp491)->f1;
if( _temp497 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL494;} else{ goto _LL495;}}
else{ goto _LL495;} _LL495: goto _LL496; _LL494: return Cyc_Toc_add_tagged_ptr_type(
_temp421, _temp415); _LL496: return Cyc_Absyn_star_typ( _temp421,( void*) Cyc_Absyn_HeapRgn,
_temp415); _LL492:;} _LL335: return t; _LL337: return t; _LL339: return t;
_LL341: return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp499=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp499[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp500; _temp500.tag= Cyc_Absyn_ArrayType;
_temp500.f1=( void*) Cyc_Toc_typ_to_c_array( _temp431); _temp500.f2= _temp429;
_temp500.f3= _temp427; _temp500;}); _temp499;}); _LL343: { struct Cyc_List_List*
_temp501= _temp435; goto _LL502; _LL502: { struct Cyc_List_List* _temp503= 0;
goto _LL504; _LL504: for( 0; _temp435 != 0; _temp435=(( struct Cyc_List_List*)
_check_null( _temp435))->tl){ void* _temp505=( void*)(( struct Cyc_List_List*)
_check_null( _temp435))->hd; _LL507: if( _temp505 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL508;} else{ goto _LL509;} _LL509: if( _temp505 ==( void*) Cyc_Absyn_Const_att){
goto _LL510;} else{ goto _LL511;} _LL511: goto _LL512; _LL508: goto _LL510;
_LL510: continue; _LL512: _temp503=({ struct Cyc_List_List* _temp513=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp513->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( _temp435))->hd); _temp513->tl=
_temp503; _temp513;}); goto _LL506; _LL506:;}{ struct Cyc_List_List* _temp514=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp443); goto _LL515; _LL515: if(
_temp439 != 0){ void* _temp516= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp439))->type,( void*)(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp439))->rgn, Cyc_Toc_mt_tq)); goto
_LL517; _LL517: { struct _tuple1* _temp519=({ struct _tuple1* _temp518=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp518->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp439))->name; _temp518->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp439))->tq; _temp518->f3= _temp516; _temp518;}); goto _LL520;
_LL520: _temp514=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_imp_append)( _temp514,({ struct Cyc_List_List*
_temp521=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp521->hd=( void*) _temp519; _temp521->tl= 0; _temp521;}));}} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp522=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp522[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp523; _temp523.tag= Cyc_Absyn_FnType; _temp523.f1=({ struct Cyc_Absyn_FnInfo
_temp524; _temp524.tvars= 0; _temp524.effect= 0; _temp524.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp445); _temp524.args= _temp514; _temp524.c_varargs= _temp441; _temp524.cyc_varargs=
0; _temp524.rgn_po= 0; _temp524.attributes= _temp503; _temp524;}); _temp523;});
_temp522;});}}} _LL345: _temp451=(( struct Cyc_List_List*(*)( struct _tuple3*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp451);{ struct _tagged_string* _temp525= Cyc_Toc_add_tuple_type( _temp451);
goto _LL526; _LL526: return Cyc_Absyn_strct( _temp525);} _LL347: return( void*)({
struct Cyc_Absyn_AnonStructType_struct* _temp527=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp527[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp528; _temp528.tag= Cyc_Absyn_AnonStructType;
_temp528.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp453); _temp528;}); _temp527;}); _LL349: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp529=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp529[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp530; _temp530.tag=
Cyc_Absyn_AnonUnionType; _temp530.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp455); _temp530;}); _temp529;}); _LL351: if( _temp461 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp531=(
unsigned char*)"anonymous union"; struct _tagged_string _temp532; _temp532.curr=
_temp531; _temp532.base= _temp531; _temp532.last_plus_one= _temp531 + 16;
_temp532;}));} return Cyc_Absyn_unionq_typ(( struct _tuple0*) _check_null(
_temp461)); _LL353: if( _temp467 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp533=( unsigned char*)"anonymous struct"; struct
_tagged_string _temp534; _temp534.curr= _temp533; _temp534.base= _temp533;
_temp534.last_plus_one= _temp533 + 17; _temp534;}));} return Cyc_Absyn_strctq((
struct _tuple0*) _check_null( _temp467)); _LL355: return t; _LL357: if( _temp473
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp535=( unsigned char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp536; _temp536.curr= _temp535; _temp536.base= _temp535;
_temp536.last_plus_one= _temp535 + 38; _temp536;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp537=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp537[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp538; _temp538.tag= Cyc_Absyn_TypedefType;
_temp538.f1= _temp477; _temp538.f2= 0; _temp538.f3=({ struct Cyc_Core_Opt*
_temp539=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp539->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp473))->v); _temp539;}); _temp538;}); _temp537;}); _LL359:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL361: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp540=( unsigned char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 55; _temp541;})); _LL363: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp542=(
unsigned char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp543; _temp543.curr= _temp542; _temp543.base= _temp542;
_temp543.last_plus_one= _temp542 + 54; _temp543;})); _LL365: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp544=(
unsigned char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp545; _temp545.curr= _temp544; _temp545.base= _temp544;
_temp545.last_plus_one= _temp544 + 52; _temp545;})); _LL315:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp546= t; struct Cyc_Absyn_Exp* _temp552; struct Cyc_Absyn_Tqual
_temp554; void* _temp556; _LL548: if(( unsigned int) _temp546 > 4u?*(( int*)
_temp546) == Cyc_Absyn_ArrayType: 0){ _LL557: _temp556=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp546)->f1; goto _LL555; _LL555: _temp554=(( struct Cyc_Absyn_ArrayType_struct*)
_temp546)->f2; goto _LL553; _LL553: _temp552=(( struct Cyc_Absyn_ArrayType_struct*)
_temp546)->f3; goto _LL549;} else{ goto _LL550;} _LL550: goto _LL551; _LL549:
return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ( _temp556,( void*) Cyc_Absyn_HeapRgn,
_temp554), e, l); _LL551: return Cyc_Absyn_cast_exp( t, e, l); _LL547:;} static
int Cyc_Toc_atomic_typ( void* t){ void* _temp558= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp594; void* _temp596; void* _temp598; struct Cyc_Absyn_Exp*
_temp600; struct Cyc_Absyn_Tqual _temp602; void* _temp604; struct Cyc_Absyn_FnInfo
_temp606; struct Cyc_List_List* _temp608; struct Cyc_Absyn_Structdecl** _temp610;
struct Cyc_List_List* _temp612; struct _tuple0* _temp614; struct Cyc_Absyn_TunionInfo
_temp616; struct Cyc_List_List* _temp618; struct Cyc_List_List* _temp620; struct
Cyc_Absyn_TunionFieldInfo _temp622; struct Cyc_List_List* _temp624; void*
_temp626; struct Cyc_Absyn_Tunionfield* _temp628; struct Cyc_Absyn_Tuniondecl*
_temp630; struct Cyc_Absyn_Uniondecl** _temp632; struct Cyc_List_List* _temp634;
struct _tuple0* _temp636; struct Cyc_Absyn_PtrInfo _temp638; void* _temp640;
_LL560: if( _temp558 ==( void*) Cyc_Absyn_VoidType){ goto _LL561;} else{ goto
_LL562;} _LL562: if(( unsigned int) _temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_VarType:
0){ _LL595: _temp594=(( struct Cyc_Absyn_VarType_struct*) _temp558)->f1; goto
_LL563;} else{ goto _LL564;} _LL564: if(( unsigned int) _temp558 > 4u?*(( int*)
_temp558) == Cyc_Absyn_IntType: 0){ _LL599: _temp598=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp558)->f1; goto _LL597; _LL597: _temp596=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp558)->f2; goto _LL565;} else{ goto _LL566;} _LL566: if( _temp558 ==( void*)
Cyc_Absyn_FloatType){ goto _LL567;} else{ goto _LL568;} _LL568: if( _temp558 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL569;} else{ goto _LL570;} _LL570: if((
unsigned int) _temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_ArrayType: 0){
_LL605: _temp604=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp558)->f1;
goto _LL603; _LL603: _temp602=(( struct Cyc_Absyn_ArrayType_struct*) _temp558)->f2;
goto _LL601; _LL601: _temp600=(( struct Cyc_Absyn_ArrayType_struct*) _temp558)->f3;
goto _LL571;} else{ goto _LL572;} _LL572: if(( unsigned int) _temp558 > 4u?*((
int*) _temp558) == Cyc_Absyn_FnType: 0){ _LL607: _temp606=(( struct Cyc_Absyn_FnType_struct*)
_temp558)->f1; goto _LL573;} else{ goto _LL574;} _LL574: if(( unsigned int)
_temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_TupleType: 0){ _LL609: _temp608=((
struct Cyc_Absyn_TupleType_struct*) _temp558)->f1; goto _LL575;} else{ goto
_LL576;} _LL576: if(( unsigned int) _temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_StructType:
0){ _LL615: _temp614=(( struct Cyc_Absyn_StructType_struct*) _temp558)->f1; goto
_LL613; _LL613: _temp612=(( struct Cyc_Absyn_StructType_struct*) _temp558)->f2;
goto _LL611; _LL611: _temp610=(( struct Cyc_Absyn_StructType_struct*) _temp558)->f3;
goto _LL577;} else{ goto _LL578;} _LL578: if(( unsigned int) _temp558 > 4u?*((
int*) _temp558) == Cyc_Absyn_TunionType: 0){ _LL617: _temp616=(( struct Cyc_Absyn_TunionType_struct*)
_temp558)->f1; goto _LL579;} else{ goto _LL580;} _LL580: if(( unsigned int)
_temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_AnonStructType: 0){ _LL619:
_temp618=(( struct Cyc_Absyn_AnonStructType_struct*) _temp558)->f1; goto _LL581;}
else{ goto _LL582;} _LL582: if(( unsigned int) _temp558 > 4u?*(( int*) _temp558)
== Cyc_Absyn_AnonUnionType: 0){ _LL621: _temp620=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp558)->f1; goto _LL583;} else{ goto _LL584;} _LL584: if(( unsigned int)
_temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_TunionFieldType: 0){ _LL623:
_temp622=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp558)->f1; _LL627:
_temp626=( void*) _temp622.field_info; if(*(( int*) _temp626) == Cyc_Absyn_KnownTunionfield){
_LL631: _temp630=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp626)->f1;
goto _LL629; _LL629: _temp628=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp626)->f2; goto _LL625;} else{ goto _LL586;} _LL625: _temp624= _temp622.targs;
goto _LL585;} else{ goto _LL586;} _LL586: if(( unsigned int) _temp558 > 4u?*((
int*) _temp558) == Cyc_Absyn_UnionType: 0){ _LL637: _temp636=(( struct Cyc_Absyn_UnionType_struct*)
_temp558)->f1; goto _LL635; _LL635: _temp634=(( struct Cyc_Absyn_UnionType_struct*)
_temp558)->f2; goto _LL633; _LL633: _temp632=(( struct Cyc_Absyn_UnionType_struct*)
_temp558)->f3; goto _LL587;} else{ goto _LL588;} _LL588: if(( unsigned int)
_temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_PointerType: 0){ _LL639: _temp638=((
struct Cyc_Absyn_PointerType_struct*) _temp558)->f1; goto _LL589;} else{ goto
_LL590;} _LL590: if(( unsigned int) _temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_RgnHandleType:
0){ _LL641: _temp640=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp558)->f1;
goto _LL591;} else{ goto _LL592;} _LL592: goto _LL593; _LL561: return 1; _LL563:
return 0; _LL565: return 1; _LL567: return 1; _LL569: return 0; _LL571: return
Cyc_Toc_atomic_typ( _temp604); _LL573: return 1; _LL575: for( 0; _temp608 != 0;
_temp608=(( struct Cyc_List_List*) _check_null( _temp608))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp608))->hd)).f2)){
return 0;}} return 1; _LL577: return 0; _LL579: return 0; _LL581: _temp620=
_temp618; goto _LL583; _LL583: for( 0; _temp620 != 0; _temp620=(( struct Cyc_List_List*)
_check_null( _temp620))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp620))->hd)->type)){ return 0;}} return
1; _LL585: return _temp628->typs == 0; _LL587: return 0; _LL589: return 0;
_LL591: return 0; _LL593:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp642= Cyc_Absynpp_typ2string(
t); xprintf("atomic_typ:  bad type %.*s", _temp642.last_plus_one - _temp642.curr,
_temp642.curr);})); return 0; _LL559:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp643= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp649;
struct Cyc_Absyn_Conref* _temp651; struct Cyc_Absyn_Tqual _temp653; struct Cyc_Absyn_Conref*
_temp655; void* _temp657; void* _temp659; _LL645: if(( unsigned int) _temp643 >
4u?*(( int*) _temp643) == Cyc_Absyn_PointerType: 0){ _LL650: _temp649=(( struct
Cyc_Absyn_PointerType_struct*) _temp643)->f1; _LL660: _temp659=( void*) _temp649.elt_typ;
goto _LL658; _LL658: _temp657=( void*) _temp649.rgn_typ; goto _LL656; _LL656:
_temp655= _temp649.nullable; goto _LL654; _LL654: _temp653= _temp649.tq; goto
_LL652; _LL652: _temp651= _temp649.bounds; goto _LL646;} else{ goto _LL647;}
_LL647: goto _LL648; _LL646: { void* _temp661= Cyc_Tcutil_compress( _temp659);
_LL663: if( _temp661 ==( void*) Cyc_Absyn_VoidType){ goto _LL664;} else{ goto
_LL665;} _LL665: goto _LL666; _LL664: return 1; _LL666: return 0; _LL662:;}
_LL648: return 0; _LL644:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_string* f){ void* _temp667= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp679; struct Cyc_Absyn_Structdecl* _temp681; struct Cyc_List_List* _temp682;
struct _tuple0* _temp684; struct Cyc_List_List* _temp686; struct Cyc_List_List*
_temp688; struct Cyc_Absyn_Uniondecl** _temp690; struct Cyc_List_List* _temp692;
struct _tuple0* _temp694; _LL669: if(( unsigned int) _temp667 > 4u?*(( int*)
_temp667) == Cyc_Absyn_StructType: 0){ _LL685: _temp684=(( struct Cyc_Absyn_StructType_struct*)
_temp667)->f1; goto _LL683; _LL683: _temp682=(( struct Cyc_Absyn_StructType_struct*)
_temp667)->f2; goto _LL680; _LL680: _temp679=(( struct Cyc_Absyn_StructType_struct*)
_temp667)->f3; if( _temp679 == 0){ goto _LL671;} else{ _temp681=* _temp679; goto
_LL670;}} else{ goto _LL671;} _LL671: if(( unsigned int) _temp667 > 4u?*(( int*)
_temp667) == Cyc_Absyn_AnonStructType: 0){ _LL687: _temp686=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp667)->f1; goto _LL672;} else{ goto _LL673;} _LL673: if(( unsigned int)
_temp667 > 4u?*(( int*) _temp667) == Cyc_Absyn_AnonUnionType: 0){ _LL689:
_temp688=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp667)->f1; goto _LL674;}
else{ goto _LL675;} _LL675: if(( unsigned int) _temp667 > 4u?*(( int*) _temp667)
== Cyc_Absyn_UnionType: 0){ _LL695: _temp694=(( struct Cyc_Absyn_UnionType_struct*)
_temp667)->f1; goto _LL693; _LL693: _temp692=(( struct Cyc_Absyn_UnionType_struct*)
_temp667)->f2; goto _LL691; _LL691: _temp690=(( struct Cyc_Absyn_UnionType_struct*)
_temp667)->f3; goto _LL676;} else{ goto _LL677;} _LL677: goto _LL678; _LL670:
if( _temp681->fields == 0){( void) _throw((( void*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp696=( unsigned char*)"is_poly_field: struct missing fields";
struct _tagged_string _temp697; _temp697.curr= _temp696; _temp697.base= _temp696;
_temp697.last_plus_one= _temp696 + 37; _temp697;})));} _temp686=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp681->fields))->v; goto _LL672; _LL672: {
struct Cyc_Absyn_Structfield* _temp698= Cyc_Absyn_lookup_field( _temp686, f);
goto _LL699; _LL699: if( _temp698 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct _tagged_string _temp700=*((
struct Cyc_Absyn_Structfield*) _check_null( _temp698))->name; xprintf("is_poly_field: bad field %.*s",
_temp700.last_plus_one - _temp700.curr, _temp700.curr);}));} return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp698))->type);} _LL674:
return 0; _LL676: return 0; _LL678:( void) _throw((( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp701= Cyc_Absynpp_typ2string( t); xprintf("is_poly_field: bad type %.*s",
_temp701.last_plus_one - _temp701.curr, _temp701.curr);}))); _LL668:;} static
int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp702=( void*) e->r;
struct _tagged_string* _temp710; struct Cyc_Absyn_Exp* _temp712; struct
_tagged_string* _temp714; struct Cyc_Absyn_Exp* _temp716; _LL704: if(*(( int*)
_temp702) == Cyc_Absyn_StructMember_e){ _LL713: _temp712=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp702)->f1; goto _LL711; _LL711: _temp710=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp702)->f2; goto _LL705;} else{ goto _LL706;} _LL706: if(*(( int*) _temp702)
== Cyc_Absyn_StructArrow_e){ _LL717: _temp716=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp702)->f1; goto _LL715; _LL715: _temp714=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp702)->f2; goto _LL707;} else{ goto _LL708;} _LL708: goto _LL709; _LL705:
return Cyc_Toc_is_poly_field(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp712->topt))->v, _temp710); _LL707: { void* _temp718= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp716->topt))->v); struct Cyc_Absyn_PtrInfo
_temp724; struct Cyc_Absyn_Conref* _temp726; struct Cyc_Absyn_Tqual _temp728;
struct Cyc_Absyn_Conref* _temp730; void* _temp732; void* _temp734; _LL720: if((
unsigned int) _temp718 > 4u?*(( int*) _temp718) == Cyc_Absyn_PointerType: 0){
_LL725: _temp724=(( struct Cyc_Absyn_PointerType_struct*) _temp718)->f1; _LL735:
_temp734=( void*) _temp724.elt_typ; goto _LL733; _LL733: _temp732=( void*)
_temp724.rgn_typ; goto _LL731; _LL731: _temp730= _temp724.nullable; goto _LL729;
_LL729: _temp728= _temp724.tq; goto _LL727; _LL727: _temp726= _temp724.bounds;
goto _LL721;} else{ goto _LL722;} _LL722: goto _LL723; _LL721: return Cyc_Toc_is_poly_field(
_temp734, _temp714); _LL723:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp736= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp716->topt))->v); xprintf("is_poly_project:  bad type %.*s",
_temp736.last_plus_one - _temp736.curr, _temp736.curr);})); return 0; _LL719:;}
_LL709: return 0; _LL703:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_qv,
0),({ struct Cyc_List_List* _temp737=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp737->hd=( void*) s; _temp737->tl= 0; _temp737;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_atomic_qv, 0),({
struct Cyc_List_List* _temp738=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp738->hd=( void*) s; _temp738->tl= 0; _temp738;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__region_malloc_qv,
0),({ struct Cyc_List_List* _temp739=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp739->hd=( void*) rgn; _temp739->tl=({ struct Cyc_List_List*
_temp740=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp740->hd=( void*) s; _temp740->tl= 0; _temp740;}); _temp739;}), 0);} static
unsigned char _temp743[ 3u]="f0"; static struct _tagged_string Cyc_Toc_f0={
_temp743, _temp743, _temp743 + 3u}; static struct _tagged_string* Cyc_Toc_field_names_v[
1u]={& Cyc_Toc_f0}; struct _tagged_ptr3{ struct _tagged_string** curr; struct
_tagged_string** base; struct _tagged_string** last_plus_one; } ; static struct
_tagged_ptr3 Cyc_Toc_field_names={( struct _tagged_string**) Cyc_Toc_field_names_v,(
struct _tagged_string**) Cyc_Toc_field_names_v,( struct _tagged_string**) Cyc_Toc_field_names_v
+ 1u}; static struct _tagged_string* Cyc_Toc_fieldname( int i){ unsigned int fsz=({
struct _tagged_ptr3 _temp751= Cyc_Toc_field_names;( unsigned int)( _temp751.last_plus_one
- _temp751.curr);}); if( i >= fsz){ Cyc_Toc_field_names=({ unsigned int _temp746=(
unsigned int)( i + 1); struct _tagged_string** _temp747=( struct _tagged_string**)
GC_malloc( sizeof( struct _tagged_string*) * _temp746); struct _tagged_ptr3
_temp750={ _temp747, _temp747, _temp747 + _temp746};{ unsigned int _temp748=
_temp746; unsigned int j; for( j= 0; j < _temp748; j ++){ _temp747[ j]= j < fsz?*((
struct _tagged_string**(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Toc_field_names, sizeof( struct _tagged_string*),(
int) j):({ struct _tagged_string* _temp749=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string)); _temp749[ 0]=( struct _tagged_string) xprintf("f%d",(
int) j); _temp749;});}}; _temp750;});} return*(( struct _tagged_string**(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Toc_field_names,
sizeof( struct _tagged_string*), i);} static const int Cyc_Toc_NullCheck= 0;
struct Cyc_Toc_NullCheck_struct{ int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3; } ; static const int Cyc_Toc_UntagPtrNocheck= 2; struct Cyc_Toc_UntagPtrNocheck_struct{
int tag; void* f1; } ; static const int Cyc_Toc_TagPtr= 3; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Toc_NoConv=
0; struct _tuple7{ void* f1; void* f2; } ; static void* Cyc_Toc_conversion( void*
t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp753=({ struct _tuple7 _temp752; _temp752.f1= t1; _temp752.f2=
t2; _temp752;}); void* _temp761; struct Cyc_Absyn_PtrInfo _temp763; void*
_temp765; struct Cyc_Absyn_PtrInfo _temp767; void* _temp769; void* _temp771;
void* _temp773; void* _temp775; struct Cyc_Absyn_PtrInfo _temp777; _LL755:
_LL766: _temp765= _temp753.f1; if(( unsigned int) _temp765 > 4u?*(( int*)
_temp765) == Cyc_Absyn_PointerType: 0){ _LL768: _temp767=(( struct Cyc_Absyn_PointerType_struct*)
_temp765)->f1; goto _LL762;} else{ goto _LL757;} _LL762: _temp761= _temp753.f2;
if(( unsigned int) _temp761 > 4u?*(( int*) _temp761) == Cyc_Absyn_PointerType: 0){
_LL764: _temp763=(( struct Cyc_Absyn_PointerType_struct*) _temp761)->f1; goto
_LL756;} else{ goto _LL757;} _LL757: _LL776: _temp775= _temp753.f1; if((
unsigned int) _temp775 > 4u?*(( int*) _temp775) == Cyc_Absyn_PointerType: 0){
_LL778: _temp777=(( struct Cyc_Absyn_PointerType_struct*) _temp775)->f1; goto
_LL770;} else{ goto _LL759;} _LL770: _temp769= _temp753.f2; if(( unsigned int)
_temp769 > 4u?*(( int*) _temp769) == Cyc_Absyn_IntType: 0){ _LL774: _temp773=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp769)->f1; goto _LL772; _LL772:
_temp771=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp769)->f2; goto _LL758;}
else{ goto _LL759;} _LL759: goto _LL760; _LL756: { int _temp779=(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp767.nullable); goto
_LL780; _LL780: { int _temp781=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp763.nullable); goto _LL782; _LL782: { void* _temp783=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp767.bounds); goto _LL784;
_LL784: { void* _temp785=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp763.bounds); goto _LL786; _LL786: { struct _tuple7 _temp788=({ struct
_tuple7 _temp787; _temp787.f1= _temp783; _temp787.f2= _temp785; _temp787;});
void* _temp798; struct Cyc_Absyn_Exp* _temp800; void* _temp802; struct Cyc_Absyn_Exp*
_temp804; void* _temp806; void* _temp808; void* _temp810; void* _temp812; struct
Cyc_Absyn_Exp* _temp814; void* _temp816; struct Cyc_Absyn_Exp* _temp818; void*
_temp820; _LL790: _LL803: _temp802= _temp788.f1; if(( unsigned int) _temp802 > 1u?*((
int*) _temp802) == Cyc_Absyn_Upper_b: 0){ _LL805: _temp804=(( struct Cyc_Absyn_Upper_b_struct*)
_temp802)->f1; goto _LL799;} else{ goto _LL792;} _LL799: _temp798= _temp788.f2;
if(( unsigned int) _temp798 > 1u?*(( int*) _temp798) == Cyc_Absyn_Upper_b: 0){
_LL801: _temp800=(( struct Cyc_Absyn_Upper_b_struct*) _temp798)->f1; goto _LL791;}
else{ goto _LL792;} _LL792: _LL809: _temp808= _temp788.f1; if( _temp808 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL807;} else{ goto _LL794;} _LL807: _temp806=
_temp788.f2; if( _temp806 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL793;} else{
goto _LL794;} _LL794: _LL813: _temp812= _temp788.f1; if(( unsigned int) _temp812
> 1u?*(( int*) _temp812) == Cyc_Absyn_Upper_b: 0){ _LL815: _temp814=(( struct
Cyc_Absyn_Upper_b_struct*) _temp812)->f1; goto _LL811;} else{ goto _LL796;}
_LL811: _temp810= _temp788.f2; if( _temp810 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL795;} else{ goto _LL796;} _LL796: _LL821: _temp820= _temp788.f1; if(
_temp820 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL817;} else{ goto _LL789;}
_LL817: _temp816= _temp788.f2; if(( unsigned int) _temp816 > 1u?*(( int*)
_temp816) == Cyc_Absyn_Upper_b: 0){ _LL819: _temp818=(( struct Cyc_Absyn_Upper_b_struct*)
_temp816)->f1; goto _LL797;} else{ goto _LL789;} _LL791: if( _temp779? !
_temp781: 0){ return( void*)({ struct Cyc_Toc_NullCheck_struct* _temp822=(
struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp822[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp823; _temp823.tag= Cyc_Toc_NullCheck;
_temp823.f1=( void*) t2; _temp823;}); _temp822;});} return( void*) Cyc_Toc_NoConv;
_LL793: return( void*) Cyc_Toc_NoConv; _LL795: return( void*)({ struct Cyc_Toc_TagPtr_struct*
_temp824=( struct Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp824[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp825; _temp825.tag= Cyc_Toc_TagPtr;
_temp825.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp825.f2= _temp814; _temp825;});
_temp824;}); _LL797: return( void*)({ struct Cyc_Toc_UntagPtr_struct* _temp826=(
struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp826[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp827; _temp827.tag= Cyc_Toc_UntagPtr;
_temp827.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp827.f2= _temp818; _temp827.f3=
_temp781; _temp827;}); _temp826;}); _LL789:;}}}}} _LL758: { void* _temp828=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp777.bounds);
goto _LL829; _LL829: { void* _temp830= _temp828; struct Cyc_Absyn_Exp* _temp836;
_LL832: if(( unsigned int) _temp830 > 1u?*(( int*) _temp830) == Cyc_Absyn_Upper_b:
0){ _LL837: _temp836=(( struct Cyc_Absyn_Upper_b_struct*) _temp830)->f1; goto
_LL833;} else{ goto _LL834;} _LL834: if( _temp830 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL835;} else{ goto _LL831;} _LL833: return( void*) Cyc_Toc_NoConv; _LL835:
return( void*)({ struct Cyc_Toc_UntagPtrNocheck_struct* _temp838=( struct Cyc_Toc_UntagPtrNocheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_UntagPtrNocheck_struct)); _temp838[ 0]=({
struct Cyc_Toc_UntagPtrNocheck_struct _temp839; _temp839.tag= Cyc_Toc_UntagPtrNocheck;
_temp839.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp839;}); _temp838;}); _LL831:;}}
_LL760: return( void*) Cyc_Toc_NoConv; _LL754:;}} static unsigned char _temp842[
14u]="sfile_to_file"; static struct _tagged_string Cyc_Toc__sfile_to_file_string={
_temp842, _temp842, _temp842 + 14u}; static struct _tuple0 Cyc_Toc__sfile_to_file_var={(
void*)& Cyc_Toc_abs_ns_value,& Cyc_Toc__sfile_to_file_string}; static struct Cyc_Absyn_Exp*
Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(&
Cyc_Toc__sfile_to_file_var, 0),({ struct Cyc_List_List* _temp843=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp843->hd=( void*) e; _temp843->tl=
0; _temp843;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct
Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__throw_qv,
0),({ struct Cyc_List_List* _temp844=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp844->hd=( void*) e; _temp844->tl= 0; _temp844;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp845=(
unsigned char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp846; _temp846.curr= _temp845; _temp846.base= _temp845;
_temp846.last_plus_one= _temp845 + 44; _temp846;}));} if( Cyc_Toc_use_runtimecheck_funs){
return Cyc_Absyn_cast_exp( new_typ, Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv,
0),({ struct Cyc_List_List* _temp847=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp847->hd=( void*) e; _temp847->tl= 0; _temp847;}), 0),
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
_temp848= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp854; struct Cyc_Absyn_Conref*
_temp856; struct Cyc_Absyn_Tqual _temp858; struct Cyc_Absyn_Conref* _temp860;
void* _temp862; void* _temp864; _LL850: if(( unsigned int) _temp848 > 4u?*(( int*)
_temp848) == Cyc_Absyn_PointerType: 0){ _LL855: _temp854=(( struct Cyc_Absyn_PointerType_struct*)
_temp848)->f1; _LL865: _temp864=( void*) _temp854.elt_typ; goto _LL863; _LL863:
_temp862=( void*) _temp854.rgn_typ; goto _LL861; _LL861: _temp860= _temp854.nullable;
goto _LL859; _LL859: _temp858= _temp854.tq; goto _LL857; _LL857: _temp856=
_temp854.bounds; goto _LL851;} else{ goto _LL852;} _LL852: goto _LL853; _LL851:
elt_typ= _temp864; elt_tqual= _temp858; strt_typ= Cyc_Toc_add_tagged_ptr_type(
_temp864, _temp858); goto _LL849; _LL853: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp866=(
unsigned char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp867;
_temp867.curr= _temp866; _temp867.base= _temp866; _temp867.last_plus_one=
_temp866 + 28; _temp867;})); _LL849:;} if( toplevel){ int is_string= 0;{ void*
_temp868=( void*) e->r; void* _temp874; struct _tagged_string _temp876; _LL870:
if(*(( int*) _temp868) == Cyc_Absyn_Const_e){ _LL875: _temp874=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp868)->f1; if(( unsigned int) _temp874 > 1u?*((
int*) _temp874) == Cyc_Absyn_String_c: 0){ _LL877: _temp876=(( struct Cyc_Absyn_String_c_struct*)
_temp874)->f1; goto _LL871;} else{ goto _LL872;}} else{ goto _LL872;} _LL872:
goto _LL873; _LL871: is_string= 1; goto _LL869; _LL873: goto _LL869; _LL869:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp881=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp881[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp882; _temp882.tag= Cyc_Absyn_ArrayType; _temp882.f1=(
void*) elt_typ; _temp882.f2= Cyc_Toc_mt_tq; _temp882.f3=( struct Cyc_Absyn_Exp*)
sz; _temp882;}); _temp881;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp878=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp878->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp879=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp879[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp880; _temp880.tag= Cyc_Absyn_Var_d;
_temp880.f1= vd; _temp880;}); _temp879;}), 0); _temp878->tl= Cyc_Toc_result_decls;
_temp878;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ if( elt_tqual.q_const){ xexp=
Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq), e, 0);} else{
xexp= e;}}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp( xexp, sz, 0);
struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(*
_temp883)( struct _tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4))
Cyc_List_list; struct _tuple4* _temp886=({ struct _tuple4* _temp891=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp891->f1= 0; _temp891->f2=
xexp; _temp891;}); struct _tuple4* _temp887=({ struct _tuple4* _temp890=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp890->f1= 0; _temp890->f2=
xexp; _temp890;}); struct _tuple4* _temp888=({ struct _tuple4* _temp889=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp889->f1= 0; _temp889->f2=
xplussz; _temp889;}); struct _tuple4* _temp884[ 3u]={ _temp886, _temp887,
_temp888}; struct _tagged_ptr4 _temp885={ _temp884, _temp884, _temp884 + 3u};
_temp883( _temp885);}), 0); return urm_exp;}}} else{ struct Cyc_Absyn_Stmt* s=
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
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp892=( unsigned char*)"can't coerce t? to t* or t@ at the top-level";
struct _tagged_string _temp893; _temp893.curr= _temp892; _temp893.base= _temp892;
_temp893.last_plus_one= _temp892 + 45; _temp893;}));}{ struct _tuple0* x= Cyc_Toc_temp_var();
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
return({ struct Cyc_Toc_Env* _temp894=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp894->break_lab= 0; _temp894->continue_lab= 0;
_temp894->fallthru_info= 0; _temp894->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp894->toplevel= 1; _temp894;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp895=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp895->break_lab= e->break_lab;
_temp895->continue_lab= e->continue_lab; _temp895->fallthru_info= e->fallthru_info;
_temp895->varmap= e->varmap; _temp895->toplevel= e->toplevel; _temp895;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp896=(* x).f1; struct Cyc_List_List*
_temp902; _LL898: if(( unsigned int) _temp896 > 1u?*(( int*) _temp896) == Cyc_Absyn_Rel_n:
0){ _LL903: _temp902=(( struct Cyc_Absyn_Rel_n_struct*) _temp896)->f1; goto
_LL899;} else{ goto _LL900;} _LL900: goto _LL901; _LL899:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp904=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp904[ 0]=({ struct
Cyc_Core_Impossible_struct _temp905; _temp905.tag= Cyc_Core_Impossible; _temp905.f1=(
struct _tagged_string)({ struct _tagged_string _temp906= Cyc_Absynpp_qvar2string(
x); xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp906.last_plus_one -
_temp906.curr, _temp906.curr);}); _temp905;}); _temp904;})); _LL901: goto _LL897;
_LL897:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp*
data)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env*
Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab= 0; ans->continue_lab= 0; return ans;} struct _tuple8{ struct
_tagged_string* f1; struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ;
static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_string* break_l, struct _tagged_string* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
_temp907= 0; goto _LL908; _LL908: for( 0; fallthru_binders != 0;
fallthru_binders=(( struct Cyc_List_List*) _check_null( fallthru_binders))->tl){
_temp907=({ struct Cyc_List_List* _temp909=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp909->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp909->tl=
_temp907; _temp909;});} _temp907=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp907);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp910=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp910->v=( void*) break_l; _temp910;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp911=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp911->v=( void*)({ struct _tuple8*
_temp912=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp912->f1=
fallthru_l; _temp912->f2= _temp907; _temp912->f3= next_case_env->varmap;
_temp912;}); _temp911;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp913=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp913->v=( void*)
break_l; _temp913;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_string*
next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp914=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp914->v=( void*)({ struct _tuple8* _temp915=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp915->f1= next_l; _temp915->f2= 0;
_temp915->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp915;}); _temp914;});
return ans;} struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tagged_ptr5{ struct Cyc_List_List** curr; struct Cyc_List_List** base;
struct Cyc_List_List** last_plus_one; } ; struct _tuple10{ struct _tuple0* f1;
void* f2; struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp916=( unsigned char*)"expecting a literal format string";
struct _tagged_string _temp917; _temp917.curr= _temp916; _temp917.base= _temp916;
_temp917.last_plus_one= _temp916 + 34; _temp917;}));}{ struct _tagged_string
fmt_str;{ void* _temp918=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd)->r; void* _temp924; struct _tagged_string _temp926;
_LL920: if(*(( int*) _temp918) == Cyc_Absyn_Const_e){ _LL925: _temp924=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp918)->f1; if(( unsigned int) _temp924 > 1u?*((
int*) _temp924) == Cyc_Absyn_String_c: 0){ _LL927: _temp926=(( struct Cyc_Absyn_String_c_struct*)
_temp924)->f1; goto _LL921;} else{ goto _LL922;}} else{ goto _LL922;} _LL922:
goto _LL923; _LL921: fmt_str= _temp926; goto _LL919; _LL923: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp928=(
unsigned char*)"expecting a literal format string"; struct _tagged_string
_temp929; _temp929.curr= _temp928; _temp929.base= _temp928; _temp929.last_plus_one=
_temp928 + 34; _temp929;})); _LL919:;}{ int len=( int)({ struct _tagged_string
_temp1025= fmt_str;( unsigned int)( _temp1025.last_plus_one - _temp1025.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str,
sizeof( unsigned char), i) =='%'){ i ++; if( i < len?*(( const unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
fmt_str, sizeof( unsigned char), i) =='s': 0){ break;}}} if( i == len){ if( aopt
!= 0){ args=({ struct Cyc_List_List* _temp930=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp930->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp930->tl= args; _temp930;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp931= _new_region(); struct
_RegionHandle* r=& _temp931; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( fmt_str,
sizeof( unsigned char), i); rev_fmt=({ struct Cyc_List_List* _temp932=( struct
Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List)); _temp932->hd=(
void*)(( int) c); _temp932->tl= rev_fmt; _temp932;}); if( c !='%'){ continue;}{
struct Cyc_Core_Opt* _temp933= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); goto _LL934; _LL934: if( _temp933 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp935=( unsigned char*)"bad format string";
struct _tagged_string _temp936; _temp936.curr= _temp935; _temp936.base= _temp935;
_temp936.last_plus_one= _temp935 + 18; _temp936;}));}{ struct _tuple9 _temp939;
int _temp940; unsigned char _temp942; struct Cyc_List_List* _temp944; struct Cyc_List_List*
_temp946; struct Cyc_List_List* _temp948; struct Cyc_List_List* _temp950; struct
_tuple9* _temp937=( struct _tuple9*)(( struct Cyc_Core_Opt*) _check_null(
_temp933))->v; _temp939=* _temp937; _LL951: _temp950= _temp939.f1; goto _LL949;
_LL949: _temp948= _temp939.f2; goto _LL947; _LL947: _temp946= _temp939.f3; goto
_LL945; _LL945: _temp944= _temp939.f4; goto _LL943; _LL943: _temp942= _temp939.f5;
goto _LL941; _LL941: _temp940= _temp939.f6; goto _LL938; _LL938: i= _temp940 - 1;
if( _temp942 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp952)( struct _RegionHandle*, struct
_tagged_ptr5)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_ptr5)) Cyc_List_rlist; struct _RegionHandle* _temp959= r; struct Cyc_List_List*
_temp955= _temp950; struct Cyc_List_List* _temp956= _temp948; struct Cyc_List_List*
_temp957= _temp946; struct Cyc_List_List* _temp958= _temp944; struct Cyc_List_List*
_temp953[ 4u]={ _temp955, _temp956, _temp957, _temp958}; struct _tagged_ptr5
_temp954={ _temp953, _temp953, _temp953 + 4u}; _temp952( _temp959, _temp954);})),
rev_fmt); rev_fmt=({ struct Cyc_List_List* _temp960=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp960->hd=( void*)(( int)
_temp942); _temp960->tl= rev_fmt; _temp960;});{ struct Cyc_List_List* _temp961=
_temp948; struct Cyc_List_List _temp968; struct Cyc_List_List* _temp969; int
_temp971; _LL963: if( _temp961 == 0){ goto _LL965;} else{ _temp968=* _temp961;
_LL972: _temp971=( int) _temp968.hd; goto _LL970; _LL970: _temp969= _temp968.tl;
if( _temp969 == 0){ goto _LL967;} else{ goto _LL965;}} _LL967: if( _temp971 =='*'){
goto _LL964;} else{ goto _LL965;} _LL965: goto _LL966; _LL964: { struct _tuple0*
_temp973= Cyc_Toc_temp_var(); goto _LL974; _LL974: rev_temps=({ struct Cyc_List_List*
_temp975=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp975->hd=( void*)({ struct _tuple10* _temp976=( struct _tuple10*)
_region_malloc( r, sizeof( struct _tuple10)); _temp976->f1= _temp973; _temp976->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp976->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp976;});
_temp975->tl= rev_temps; _temp975;}); rev_result=({ struct Cyc_List_List*
_temp977=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp977->hd=( void*) Cyc_Absyn_var_exp( _temp973, 0); _temp977->tl= rev_result;
_temp977;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL962;} _LL966: goto
_LL962; _LL962:;}{ struct Cyc_List_List* _temp978= _temp946; struct Cyc_List_List
_temp985; struct Cyc_List_List* _temp986; struct Cyc_List_List _temp988; struct
Cyc_List_List* _temp989; int _temp991; int _temp993; _LL980: if( _temp978 == 0){
goto _LL982;} else{ _temp985=* _temp978; _LL994: _temp993=( int) _temp985.hd;
goto _LL987; _LL987: _temp986= _temp985.tl; if( _temp986 == 0){ goto _LL982;}
else{ _temp988=* _temp986; _LL992: _temp991=( int) _temp988.hd; goto _LL990;
_LL990: _temp989= _temp988.tl; if( _temp989 == 0){ goto _LL984;} else{ goto
_LL982;}}} _LL984: if( _temp993 =='.'? _temp991 =='*': 0){ goto _LL981;} else{
goto _LL982;} _LL982: goto _LL983; _LL981: { struct _tuple0* _temp995= Cyc_Toc_temp_var();
goto _LL996; _LL996: rev_temps=({ struct Cyc_List_List* _temp997=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp997->hd=( void*)({
struct _tuple10* _temp998=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp998->f1= _temp995; _temp998->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp998->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp998;}); _temp997->tl= rev_temps; _temp997;});
rev_result=({ struct Cyc_List_List* _temp999=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp999->hd=( void*) Cyc_Absyn_var_exp(
_temp995, 0); _temp999->tl= rev_result; _temp999;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL979;} _LL983: goto _LL979; _LL979:;} if( _temp942 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ struct _tagged_string _temp1000= Cyc_Absynpp_prim2string( p);
xprintf("missing arg to %.*s", _temp1000.last_plus_one - _temp1000.curr,
_temp1000.curr);}));}{ struct _tuple0* _temp1001= Cyc_Toc_temp_var(); goto
_LL1002; _LL1002: rev_temps=({ struct Cyc_List_List* _temp1003=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1003->hd=( void*)({
struct _tuple10* _temp1004=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1004->f1= _temp1001; _temp1004->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp1004->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp1004;}); _temp1003->tl= rev_temps; _temp1003;});
rev_result=({ struct Cyc_List_List* _temp1005=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1005->hd=( void*) Cyc_Absyn_var_exp(
_temp1001, 0); _temp1005->tl= rev_result; _temp1005;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp1006=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1006->hd=( void*)(( int)'.');
_temp1006->tl= rev_fmt; _temp1006;}); rev_fmt=({ struct Cyc_List_List* _temp1007=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1007->hd=( void*)(( int)'*'); _temp1007->tl= rev_fmt; _temp1007;}); rev_fmt=({
struct Cyc_List_List* _temp1008=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp1008->hd=( void*)(( int)'s'); _temp1008->tl=
rev_fmt; _temp1008;});{ struct _tuple0* _temp1009= Cyc_Toc_temp_var(); goto
_LL1010; _LL1010: rev_temps=({ struct Cyc_List_List* _temp1011=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1011->hd=( void*)({
struct _tuple10* _temp1012=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1012->f1= _temp1009; _temp1012->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp1012->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp1012;}); _temp1011->tl= rev_temps; _temp1011;});{
struct Cyc_Absyn_Exp* _temp1013= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1009, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1009, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL1014; _LL1014: {
struct Cyc_Absyn_Exp* _temp1015= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1009, 0), Cyc_Toc_curr_sp, 0); goto _LL1016; _LL1016: rev_result=({ struct
Cyc_List_List* _temp1017=( struct Cyc_List_List*) _region_malloc( r, sizeof(
struct Cyc_List_List)); _temp1017->hd=( void*) _temp1013; _temp1017->tl=
rev_result; _temp1017;}); rev_result=({ struct Cyc_List_List* _temp1018=( struct
Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List)); _temp1018->hd=(
void*) _temp1015; _temp1018->tl= rev_result; _temp1018;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}}{
struct Cyc_List_List* _temp1020=({ struct Cyc_List_List* _temp1019=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1019->hd=( void*) Cyc_Absyn_string_exp((
struct _tagged_string) Cyc_String_implode((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0); _temp1019->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_result); _temp1019;}); goto _LL1021;
_LL1021: if( aopt != 0){ _temp1020=({ struct Cyc_List_List* _temp1022=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1022->hd=( void*)((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( aopt))->v);
_temp1022->tl= _temp1020; _temp1022;});}{ struct Cyc_Absyn_Exp* _temp1023= Cyc_Absyn_primop_exp(
p, _temp1020, 0); goto _LL1024; _LL1024: s= Cyc_Absyn_exp_stmt( _temp1023, 0);
for( 0; rev_temps != 0; rev_temps=(( struct Cyc_List_List*) _check_null(
rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f1,(*(( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( rev_temps))->hd)).f2,( struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)((
struct Cyc_List_List*) _check_null( rev_temps))->hd)).f3, s, 0);}}}};
_pop_region(& _temp931);} return Cyc_Absyn_stmt_exp( s, 0);}}}} static void* Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1026=( unsigned char*)"Missing type in primop ";
struct _tagged_string _temp1027; _temp1027.curr= _temp1026; _temp1027.base=
_temp1026; _temp1027.last_plus_one= _temp1026 + 24; _temp1027;}));} return Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1028=( unsigned char*)"Missing type in primop ";
struct _tagged_string _temp1029; _temp1029.curr= _temp1028; _temp1029.base=
_temp1028; _temp1029.last_plus_one= _temp1028 + 24; _temp1029;}));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp1030=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1030->f1= Cyc_Toc_mt_tq; _temp1030->f2=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp1030;});} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp1031=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp1031[
0]=({ struct _tuple4 _temp1032; _temp1032.f1= 0; _temp1032.f2= e; _temp1032;});
_temp1031;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env*
nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer,
struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t;
if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp*
_temp1033= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); goto _LL1034; _LL1034: if(
rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic(
_temp1033): Cyc_Toc_malloc_ptr( _temp1033), 0);} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c( nv, r); eo=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp( r, _temp1033),
0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp1035=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL1036; _LL1036: for( 0;
_temp1035 != 0; _temp1035=(( struct Cyc_List_List*) _check_null( _temp1035))->tl){
struct _tuple4 _temp1039; struct Cyc_Absyn_Exp* _temp1040; struct Cyc_List_List*
_temp1042; struct _tuple4* _temp1037=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1035))->hd; _temp1039=* _temp1037; _LL1043: _temp1042=
_temp1039.f1; goto _LL1041; _LL1041: _temp1040= _temp1039.f2; goto _LL1038;
_LL1038: { struct Cyc_Absyn_Exp* e_index; if( _temp1042 == 0){ e_index= Cyc_Absyn_signed_int_exp(
count --, 0);} else{ if((( struct Cyc_List_List*) _check_null( _temp1042))->tl
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp1044=( unsigned char*)"multiple designators in array"; struct
_tagged_string _temp1045; _temp1045.curr= _temp1044; _temp1045.base= _temp1044;
_temp1045.last_plus_one= _temp1044 + 30; _temp1045;}));}{ void* _temp1046=( void*)((
struct Cyc_List_List*) _check_null( _temp1042))->hd; goto _LL1047; _LL1047: {
void* _temp1048= _temp1046; struct Cyc_Absyn_Exp* _temp1054; struct
_tagged_string* _temp1056; _LL1050: if(*(( int*) _temp1048) == Cyc_Absyn_ArrayElement){
_LL1055: _temp1054=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1048)->f1;
goto _LL1051;} else{ goto _LL1052;} _LL1052: if(*(( int*) _temp1048) == Cyc_Absyn_FieldName){
_LL1057: _temp1056=(( struct Cyc_Absyn_FieldName_struct*) _temp1048)->f1; goto
_LL1053;} else{ goto _LL1049;} _LL1051: Cyc_Toc_exp_to_c( nv, _temp1054);
e_index= _temp1054; goto _LL1049; _LL1053: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp1058=(
unsigned char*)"field name designators in array"; struct _tagged_string
_temp1059; _temp1059.curr= _temp1058; _temp1059.base= _temp1058; _temp1059.last_plus_one=
_temp1058 + 32; _temp1059;})); goto _LL1049; _LL1049:;}}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp1060=( void*)
_temp1040->r; struct Cyc_List_List* _temp1070; struct Cyc_Absyn_Exp* _temp1072;
struct Cyc_Absyn_Exp* _temp1074; struct Cyc_Absyn_Vardecl* _temp1076; struct Cyc_List_List*
_temp1078; void* _temp1080; _LL1062: if(*(( int*) _temp1060) == Cyc_Absyn_Array_e){
_LL1071: _temp1070=(( struct Cyc_Absyn_Array_e_struct*) _temp1060)->f1; goto
_LL1063;} else{ goto _LL1064;} _LL1064: if(*(( int*) _temp1060) == Cyc_Absyn_Comprehension_e){
_LL1077: _temp1076=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1060)->f1;
goto _LL1075; _LL1075: _temp1074=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1060)->f2; goto _LL1073; _LL1073: _temp1072=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1060)->f3; goto _LL1065;} else{ goto _LL1066;} _LL1066: if(*(( int*)
_temp1060) == Cyc_Absyn_AnonStruct_e){ _LL1081: _temp1080=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1060)->f1; goto _LL1079; _LL1079: _temp1078=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1060)->f2; goto _LL1067;} else{ goto _LL1068;} _LL1068: goto _LL1069;
_LL1063: s= Cyc_Toc_init_array( nv, lval, _temp1070, s); goto _LL1061; _LL1065:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1076, _temp1074, _temp1072, s, 0);
goto _LL1061; _LL1067: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1080,
_temp1078, s); goto _LL1061; _LL1069: Cyc_Toc_exp_to_c( nv, _temp1040); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( lhs, e_index, 0), _temp1040, 0),
s, 0); goto _LL1061; _LL1061:;}}}} return s;} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated){ struct _tuple0* _temp1082= vd->name; goto _LL1083;
_LL1083: if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp1082, Cyc_Absyn_varb_exp( _temp1082,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1106=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1106[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1107; _temp1107.tag= Cyc_Absyn_Local_b; _temp1107.f1=
vd; _temp1107;}); _temp1106;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp1082, 0),
Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( _temp1082, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp*
ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( _temp1082, 0), 0); struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( _temp1082, 0), 0); struct
Cyc_Absyn_Stmt* body;{ void* _temp1084=( void*) e2->r; struct Cyc_List_List*
_temp1094; struct Cyc_Absyn_Exp* _temp1096; struct Cyc_Absyn_Exp* _temp1098;
struct Cyc_Absyn_Vardecl* _temp1100; struct Cyc_List_List* _temp1102; void*
_temp1104; _LL1086: if(*(( int*) _temp1084) == Cyc_Absyn_Array_e){ _LL1095:
_temp1094=(( struct Cyc_Absyn_Array_e_struct*) _temp1084)->f1; goto _LL1087;}
else{ goto _LL1088;} _LL1088: if(*(( int*) _temp1084) == Cyc_Absyn_Comprehension_e){
_LL1101: _temp1100=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1084)->f1;
goto _LL1099; _LL1099: _temp1098=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1084)->f2; goto _LL1097; _LL1097: _temp1096=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1084)->f3; goto _LL1089;} else{ goto _LL1090;} _LL1090: if(*(( int*)
_temp1084) == Cyc_Absyn_AnonStruct_e){ _LL1105: _temp1104=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1084)->f1; goto _LL1103; _LL1103: _temp1102=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1084)->f2; goto _LL1091;} else{ goto _LL1092;} _LL1092: goto _LL1093;
_LL1087: body= Cyc_Toc_init_array( nv2, lval, _temp1094, Cyc_Absyn_skip_stmt( 0));
goto _LL1085; _LL1089: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1100,
_temp1098, _temp1096, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1085; _LL1091: body=
Cyc_Toc_init_anon_struct( nv, lval, _temp1104, _temp1102, Cyc_Absyn_skip_stmt( 0));
goto _LL1085; _LL1093: Cyc_Toc_exp_to_c( nv2, e2); body= Cyc_Absyn_assign_stmt(
lval, e2, 0); goto _LL1085; _LL1085:;}{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt(
ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt( _temp1082, Cyc_Absyn_uint_t,
0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s){{ struct Cyc_List_List* _temp1108=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( dles); goto _LL1109; _LL1109: for( 0;
_temp1108 != 0; _temp1108=(( struct Cyc_List_List*) _check_null( _temp1108))->tl){
struct _tuple4 _temp1112; struct Cyc_Absyn_Exp* _temp1113; struct Cyc_List_List*
_temp1115; struct _tuple4* _temp1110=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1108))->hd; _temp1112=* _temp1110; _LL1116: _temp1115=
_temp1112.f1; goto _LL1114; _LL1114: _temp1113= _temp1112.f2; goto _LL1111;
_LL1111: if( _temp1115 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1117=( unsigned char*)"empty designator list"; struct
_tagged_string _temp1118; _temp1118.curr= _temp1117; _temp1118.base= _temp1117;
_temp1118.last_plus_one= _temp1117 + 22; _temp1118;}));} if((( struct Cyc_List_List*)
_check_null( _temp1115))->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1119=( unsigned char*)"too many designators in anonymous struct";
struct _tagged_string _temp1120; _temp1120.curr= _temp1119; _temp1120.base=
_temp1119; _temp1120.last_plus_one= _temp1119 + 41; _temp1120;}));}{ void*
_temp1121=( void*)(( struct Cyc_List_List*) _check_null( _temp1115))->hd; struct
_tagged_string* _temp1127; _LL1123: if(*(( int*) _temp1121) == Cyc_Absyn_FieldName){
_LL1128: _temp1127=(( struct Cyc_Absyn_FieldName_struct*) _temp1121)->f1; goto
_LL1124;} else{ goto _LL1125;} _LL1125: goto _LL1126; _LL1124: { struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_structmember_exp( lhs, _temp1127, 0);{ void* _temp1129=( void*)
_temp1113->r; struct Cyc_List_List* _temp1139; struct Cyc_Absyn_Exp* _temp1141;
struct Cyc_Absyn_Exp* _temp1143; struct Cyc_Absyn_Vardecl* _temp1145; struct Cyc_List_List*
_temp1147; void* _temp1149; _LL1131: if(*(( int*) _temp1129) == Cyc_Absyn_Array_e){
_LL1140: _temp1139=(( struct Cyc_Absyn_Array_e_struct*) _temp1129)->f1; goto
_LL1132;} else{ goto _LL1133;} _LL1133: if(*(( int*) _temp1129) == Cyc_Absyn_Comprehension_e){
_LL1146: _temp1145=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1129)->f1;
goto _LL1144; _LL1144: _temp1143=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1129)->f2; goto _LL1142; _LL1142: _temp1141=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1129)->f3; goto _LL1134;} else{ goto _LL1135;} _LL1135: if(*(( int*)
_temp1129) == Cyc_Absyn_AnonStruct_e){ _LL1150: _temp1149=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1129)->f1; goto _LL1148; _LL1148: _temp1147=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1129)->f2; goto _LL1136;} else{ goto _LL1137;} _LL1137: goto _LL1138;
_LL1132: s= Cyc_Toc_init_array( nv, lval, _temp1139, s); goto _LL1130; _LL1134:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1145, _temp1143, _temp1141, s, 0);
goto _LL1130; _LL1136: s= Cyc_Toc_init_anon_struct( nv, lval, _temp1149,
_temp1147, s); goto _LL1130; _LL1138: Cyc_Toc_exp_to_c( nv, _temp1113); if( Cyc_Toc_is_poly_field(
struct_type, _temp1127)){ _temp1113= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1113, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval, _temp1113, 0), 0), s, 0); goto _LL1130; _LL1130:;} goto _LL1122;} _LL1126:(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1151=( unsigned char*)"array designator in struct"; struct
_tagged_string _temp1152; _temp1152.curr= _temp1151; _temp1152.base= _temp1151;
_temp1152.last_plus_one= _temp1151 + 27; _temp1152;}))); _LL1122:;}}} return s;}
static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int
pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
_RegionHandle _temp1153= _new_region(); struct _RegionHandle* r=& _temp1153;
_push_region( r);{ struct Cyc_List_List* _temp1154=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c, es); goto _LL1155; _LL1155: { struct
_tagged_string* _temp1156= Cyc_Toc_add_tuple_type( _temp1154); goto _LL1157;
_LL1157: { struct _tuple0* _temp1158= Cyc_Toc_temp_var(); goto _LL1159; _LL1159: {
struct Cyc_Absyn_Exp* _temp1160= Cyc_Absyn_var_exp( _temp1158, 0); goto _LL1161;
_LL1161: { struct Cyc_Absyn_Stmt* _temp1162= Cyc_Absyn_exp_stmt( _temp1160, 0);
goto _LL1163; _LL1163: { struct Cyc_Absyn_Exp*(* _temp1164)( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; goto _LL1165; _LL1165: { int is_atomic= 1; struct
Cyc_List_List* _temp1166=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( r, es); goto _LL1167; _LL1167:{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1166); for( 0;
_temp1166 != 0;( _temp1166=(( struct Cyc_List_List*) _check_null( _temp1166))->tl,
-- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1166))->hd; struct Cyc_Absyn_Exp* lval= _temp1164( _temp1160,
Cyc_Toc_fieldname( i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v): 0;{ void* _temp1168=( void*) e->r;
struct Cyc_List_List* _temp1176; struct Cyc_Absyn_Exp* _temp1178; struct Cyc_Absyn_Exp*
_temp1180; struct Cyc_Absyn_Vardecl* _temp1182; _LL1170: if(*(( int*) _temp1168)
== Cyc_Absyn_Array_e){ _LL1177: _temp1176=(( struct Cyc_Absyn_Array_e_struct*)
_temp1168)->f1; goto _LL1171;} else{ goto _LL1172;} _LL1172: if(*(( int*)
_temp1168) == Cyc_Absyn_Comprehension_e){ _LL1183: _temp1182=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1168)->f1; goto _LL1181; _LL1181: _temp1180=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1168)->f2; goto _LL1179; _LL1179: _temp1178=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1168)->f3; goto _LL1173;} else{ goto _LL1174;} _LL1174: goto _LL1175;
_LL1171: _temp1162= Cyc_Toc_init_array( nv, lval, _temp1176, _temp1162); goto
_LL1169; _LL1173: _temp1162= Cyc_Toc_init_comprehension( nv, lval, _temp1182,
_temp1180, _temp1178, _temp1162, 0); goto _LL1169; _LL1175: Cyc_Toc_exp_to_c( nv,
e); _temp1162= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1164( _temp1160, Cyc_Toc_fieldname( i), 0), e, 0), 0), _temp1162, 0); goto
_LL1169; _LL1169:;}}}{ struct Cyc_Absyn_Exp* _temp1184= Cyc_Toc_make_struct( nv,
_temp1158, Cyc_Absyn_strct( _temp1156), _temp1162, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp1184;}}}}}}}}; _pop_region(& _temp1153);} static
struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env* nv, void*
struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp1185= Cyc_Toc_temp_var(); goto
_LL1186; _LL1186: { struct Cyc_Absyn_Exp* _temp1187= Cyc_Absyn_var_exp(
_temp1185, 0); goto _LL1188; _LL1188: { struct Cyc_Absyn_Stmt* _temp1189= Cyc_Absyn_exp_stmt(
_temp1187, 0); goto _LL1190; _LL1190: { struct Cyc_Absyn_Exp*(* _temp1191)(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL1192;
_LL1192: { int is_atomic= 1; struct _RegionHandle _temp1193= _new_region();
struct _RegionHandle* r=& _temp1193; _push_region( r);{ struct Cyc_List_List*
_temp1194=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, dles); goto _LL1195; _LL1195: for( 0; _temp1194 != 0;
_temp1194=(( struct Cyc_List_List*) _check_null( _temp1194))->tl){ struct
_tuple4 _temp1198; struct Cyc_Absyn_Exp* _temp1199; struct Cyc_List_List*
_temp1201; struct _tuple4* _temp1196=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1194))->hd; _temp1198=* _temp1196; _LL1202: _temp1201=
_temp1198.f1; goto _LL1200; _LL1200: _temp1199= _temp1198.f2; goto _LL1197;
_LL1197: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1199->topt))->v): 0; if( _temp1201 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1203=(
unsigned char*)"empty designator list"; struct _tagged_string _temp1204;
_temp1204.curr= _temp1203; _temp1204.base= _temp1203; _temp1204.last_plus_one=
_temp1203 + 22; _temp1204;}));} if((( struct Cyc_List_List*) _check_null(
_temp1201))->tl != 0){ struct _tuple0* _temp1205= Cyc_Toc_temp_var(); goto
_LL1206; _LL1206: { struct Cyc_Absyn_Exp* _temp1207= Cyc_Absyn_var_exp(
_temp1205, 0); goto _LL1208; _LL1208: for( 0; _temp1201 != 0; _temp1201=((
struct Cyc_List_List*) _check_null( _temp1201))->tl){ void* _temp1209=( void*)((
struct Cyc_List_List*) _check_null( _temp1201))->hd; struct _tagged_string*
_temp1215; _LL1211: if(*(( int*) _temp1209) == Cyc_Absyn_FieldName){ _LL1216:
_temp1215=(( struct Cyc_Absyn_FieldName_struct*) _temp1209)->f1; goto _LL1212;}
else{ goto _LL1213;} _LL1213: goto _LL1214; _LL1212: if( Cyc_Toc_is_poly_field(
struct_type, _temp1215)){ _temp1207= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1207, 0);} _temp1189= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1191( _temp1187, _temp1215, 0), _temp1207, 0), 0), _temp1189, 0); goto
_LL1210; _LL1214:( void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp1217=( unsigned char*)"array designator in struct"; struct
_tagged_string _temp1218; _temp1218.curr= _temp1217; _temp1218.base= _temp1217;
_temp1218.last_plus_one= _temp1217 + 27; _temp1218;}))); _LL1210:;} Cyc_Toc_exp_to_c(
nv, _temp1199); _temp1189= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
_temp1207, _temp1199, 0), 0), _temp1189, 0);}} else{ void* _temp1219=( void*)((
struct Cyc_List_List*) _check_null( _temp1201))->hd; struct _tagged_string*
_temp1225; _LL1221: if(*(( int*) _temp1219) == Cyc_Absyn_FieldName){ _LL1226:
_temp1225=(( struct Cyc_Absyn_FieldName_struct*) _temp1219)->f1; goto _LL1222;}
else{ goto _LL1223;} _LL1223: goto _LL1224; _LL1222: { struct Cyc_Absyn_Exp*
lval= _temp1191( _temp1187, _temp1225, 0);{ void* _temp1227=( void*) _temp1199->r;
struct Cyc_List_List* _temp1237; struct Cyc_Absyn_Exp* _temp1239; struct Cyc_Absyn_Exp*
_temp1241; struct Cyc_Absyn_Vardecl* _temp1243; struct Cyc_List_List* _temp1245;
void* _temp1247; _LL1229: if(*(( int*) _temp1227) == Cyc_Absyn_Array_e){ _LL1238:
_temp1237=(( struct Cyc_Absyn_Array_e_struct*) _temp1227)->f1; goto _LL1230;}
else{ goto _LL1231;} _LL1231: if(*(( int*) _temp1227) == Cyc_Absyn_Comprehension_e){
_LL1244: _temp1243=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1227)->f1;
goto _LL1242; _LL1242: _temp1241=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1227)->f2; goto _LL1240; _LL1240: _temp1239=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1227)->f3; goto _LL1232;} else{ goto _LL1233;} _LL1233: if(*(( int*)
_temp1227) == Cyc_Absyn_AnonStruct_e){ _LL1248: _temp1247=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1227)->f1; goto _LL1246; _LL1246: _temp1245=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1227)->f2; goto _LL1234;} else{ goto _LL1235;} _LL1235: goto _LL1236;
_LL1230: _temp1189= Cyc_Toc_init_array( nv, lval, _temp1237, _temp1189); goto
_LL1228; _LL1232: _temp1189= Cyc_Toc_init_comprehension( nv, lval, _temp1243,
_temp1241, _temp1239, _temp1189, 0); goto _LL1228; _LL1234: _temp1189= Cyc_Toc_init_anon_struct(
nv, lval, _temp1247, _temp1245, _temp1189); goto _LL1228; _LL1236: Cyc_Toc_exp_to_c(
nv, _temp1199); if( Cyc_Toc_is_poly_field( struct_type, _temp1225)){ _temp1199=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1199, 0);} _temp1189= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1199, 0), 0), _temp1189, 0);
goto _LL1228; _LL1228:;} goto _LL1220;} _LL1224:( void) _throw((( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1249=(
unsigned char*)"array designator in struct"; struct _tagged_string _temp1250;
_temp1250.curr= _temp1249; _temp1250.base= _temp1249; _temp1250.last_plus_one=
_temp1249 + 27; _temp1250;}))); _LL1220:;}}}{ struct Cyc_Absyn_Exp* _temp1251=
Cyc_Toc_make_struct( nv, _temp1185, Cyc_Absyn_strctq( tdn), _temp1189, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp1251;}; _pop_region(&
_temp1193);}}}}} struct _tuple11{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp*
f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp*
el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2,
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp*
e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp1252=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp1252[ 0]=({
struct Cyc_Absyn_Increment_e_struct _temp1253; _temp1253.tag= Cyc_Absyn_Increment_e;
_temp1253.f1= e1; _temp1253.f2=( void*) incr; _temp1253;}); _temp1252;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp1254=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp1264; struct Cyc_Absyn_Exp*
_temp1266; void* _temp1268; struct _tagged_string* _temp1270; struct Cyc_Absyn_Exp*
_temp1272; _LL1256: if(*(( int*) _temp1254) == Cyc_Absyn_StmtExp_e){ _LL1265:
_temp1264=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1254)->f1; goto _LL1257;}
else{ goto _LL1258;} _LL1258: if(*(( int*) _temp1254) == Cyc_Absyn_Cast_e){
_LL1269: _temp1268=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1254)->f1;
goto _LL1267; _LL1267: _temp1266=(( struct Cyc_Absyn_Cast_e_struct*) _temp1254)->f2;
goto _LL1259;} else{ goto _LL1260;} _LL1260: if(*(( int*) _temp1254) == Cyc_Absyn_StructMember_e){
_LL1273: _temp1272=(( struct Cyc_Absyn_StructMember_e_struct*) _temp1254)->f1;
goto _LL1271; _LL1271: _temp1270=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1254)->f2; goto _LL1261;} else{ goto _LL1262;} _LL1262: goto _LL1263;
_LL1257:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1264, fs, f, f_env); goto _LL1255; _LL1259:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1266, fs, f, f_env); goto
_LL1255; _LL1261:( void*)( e1->r=( void*)(( void*) _temp1272->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1274=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1274->hd=( void*) _temp1270; _temp1274->tl= fs;
_temp1274;}), f, f_env); goto _LL1255; _LL1263: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
struct _tagged_string _temp1275= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1275.last_plus_one - _temp1275.curr, _temp1275.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1255;}
_LL1255:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1276=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1286; struct Cyc_Absyn_Stmt* _temp1288; struct Cyc_Absyn_Decl* _temp1290;
struct Cyc_Absyn_Stmt* _temp1292; struct Cyc_Absyn_Stmt* _temp1294; _LL1278: if((
unsigned int) _temp1276 > 1u?*(( int*) _temp1276) == Cyc_Absyn_Exp_s: 0){
_LL1287: _temp1286=(( struct Cyc_Absyn_Exp_s_struct*) _temp1276)->f1; goto
_LL1279;} else{ goto _LL1280;} _LL1280: if(( unsigned int) _temp1276 > 1u?*((
int*) _temp1276) == Cyc_Absyn_Decl_s: 0){ _LL1291: _temp1290=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1276)->f1; goto _LL1289; _LL1289: _temp1288=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1276)->f2; goto _LL1281;} else{ goto _LL1282;} _LL1282: if(( unsigned int)
_temp1276 > 1u?*(( int*) _temp1276) == Cyc_Absyn_Seq_s: 0){ _LL1295: _temp1294=((
struct Cyc_Absyn_Seq_s_struct*) _temp1276)->f1; goto _LL1293; _LL1293: _temp1292=((
struct Cyc_Absyn_Seq_s_struct*) _temp1276)->f2; goto _LL1283;} else{ goto
_LL1284;} _LL1284: goto _LL1285; _LL1279:(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1286, fs, f, f_env); goto
_LL1277; _LL1281:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1288, fs, f, f_env); goto _LL1277; _LL1283:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1292, fs, f, f_env);
goto _LL1277; _LL1285:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ struct _tagged_string _temp1296= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1296.last_plus_one - _temp1296.curr,
_temp1296.curr);})); goto _LL1277; _LL1277:;} static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s); static struct Cyc_List_List*
Cyc_Toc_rmap_2c( struct _RegionHandle* r2, void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List* _temp1297=( struct
Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1297->hd=(
void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1297->tl=
0; _temp1297;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp1298=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1298->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1298->tl= 0;
_temp1298;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x))
Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1299=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1299->f1= 0; _temp1299->f2= e;
_temp1299;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){
void* _temp1300= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp1306;
_LL1302: if(( unsigned int) _temp1300 > 4u?*(( int*) _temp1300) == Cyc_Absyn_PointerType:
0){ _LL1307: _temp1306=(( struct Cyc_Absyn_PointerType_struct*) _temp1300)->f1;
goto _LL1303;} else{ goto _LL1304;} _LL1304: goto _LL1305; _LL1303: return
_temp1306; _LL1305:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp1308=( unsigned char*)"get_ptr_typ:  not a pointer!"; struct
_tagged_string _temp1309; _temp1309.curr= _temp1308; _temp1309.base= _temp1308;
_temp1309.last_plus_one= _temp1308 + 29; _temp1309;}))); _LL1301:;} struct
_tagged_ptr6{ struct _tuple0** curr; struct _tuple0** base; struct _tuple0**
last_plus_one; } ; struct _tagged_ptr7{ struct _tuple1** curr; struct _tuple1**
base; struct _tuple1** last_plus_one; } ; struct _tagged_ptr8{ struct Cyc_Absyn_Exp**
curr; struct Cyc_Absyn_Exp** base; struct Cyc_Absyn_Exp** last_plus_one; } ;
struct _tuple12{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){
void* _temp1310=( void*) e->r; goto _LL1311; _LL1311: if( e->topt == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1312= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp1312.last_plus_one - _temp1312.curr, _temp1312.curr);})); return;}{ void*
old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void*
_temp1313= _temp1310; void* _temp1393; void* _temp1395; void* _temp1397; struct
_tuple0* _temp1399; struct _tuple0* _temp1401; struct Cyc_List_List* _temp1403;
void* _temp1405; void* _temp1407; struct Cyc_Absyn_Exp* _temp1409; struct Cyc_Absyn_Exp*
_temp1411; struct Cyc_Core_Opt* _temp1413; struct Cyc_Absyn_Exp* _temp1415;
struct Cyc_Absyn_Exp* _temp1417; struct Cyc_Absyn_Exp* _temp1419; struct Cyc_Absyn_Exp*
_temp1421; struct Cyc_Absyn_Exp* _temp1423; struct Cyc_Absyn_Exp* _temp1425;
struct Cyc_List_List* _temp1427; struct Cyc_Absyn_Exp* _temp1429; struct Cyc_Absyn_VarargCallInfo*
_temp1431; struct Cyc_List_List* _temp1433; struct Cyc_Absyn_Exp* _temp1435;
struct Cyc_Absyn_VarargCallInfo* _temp1437; struct Cyc_Absyn_VarargCallInfo
_temp1439; struct Cyc_Absyn_VarargInfo* _temp1440; struct Cyc_List_List*
_temp1442; int _temp1444; struct Cyc_List_List* _temp1446; struct Cyc_Absyn_Exp*
_temp1448; struct Cyc_Absyn_Exp* _temp1450; struct Cyc_Absyn_Exp* _temp1452;
struct Cyc_List_List* _temp1454; struct Cyc_Absyn_Exp* _temp1456; struct Cyc_Absyn_Exp*
_temp1458; void* _temp1460; void** _temp1462; struct Cyc_Absyn_Exp* _temp1463;
struct Cyc_Absyn_Exp* _temp1465; struct Cyc_Absyn_Exp* _temp1467; struct Cyc_Absyn_Exp*
_temp1469; void* _temp1471; struct _tagged_string* _temp1473; void* _temp1475;
struct Cyc_Absyn_Exp* _temp1477; struct _tagged_string* _temp1479; struct Cyc_Absyn_Exp*
_temp1481; struct _tagged_string* _temp1483; struct Cyc_Absyn_Exp* _temp1485;
struct Cyc_Absyn_Exp* _temp1487; struct Cyc_Absyn_Exp* _temp1489; struct Cyc_List_List*
_temp1491; struct Cyc_List_List* _temp1493; struct _tuple1* _temp1495; struct
Cyc_List_List* _temp1497; struct Cyc_Absyn_Exp* _temp1499; struct Cyc_Absyn_Exp*
_temp1501; struct Cyc_Absyn_Vardecl* _temp1503; struct Cyc_Absyn_Structdecl*
_temp1505; struct Cyc_List_List* _temp1507; struct Cyc_Core_Opt* _temp1509;
struct _tuple0* _temp1511; struct Cyc_List_List* _temp1513; void* _temp1515;
struct Cyc_Absyn_Tunionfield* _temp1517; struct Cyc_Absyn_Tuniondecl* _temp1519;
struct Cyc_List_List* _temp1521; struct Cyc_Core_Opt* _temp1523; struct Cyc_Core_Opt*
_temp1525; struct Cyc_Absyn_Tunionfield* _temp1527; struct Cyc_Absyn_Tuniondecl*
_temp1529; struct Cyc_List_List* _temp1531; struct Cyc_Core_Opt* _temp1533;
struct Cyc_Core_Opt* _temp1535; struct Cyc_Absyn_Enumfield* _temp1537; struct
Cyc_Absyn_Enumdecl* _temp1539; struct _tuple0* _temp1541; void* _temp1543;
struct Cyc_Absyn_Exp* _temp1545; struct Cyc_Absyn_Stmt* _temp1547; struct Cyc_List_List*
_temp1549; struct Cyc_Core_Opt* _temp1551; struct Cyc_Absyn_Fndecl* _temp1553;
struct Cyc_Absyn_Exp* _temp1555; _LL1315: if(*(( int*) _temp1313) == Cyc_Absyn_Const_e){
_LL1394: _temp1393=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1313)->f1;
if( _temp1393 ==( void*) Cyc_Absyn_Null_c){ goto _LL1316;} else{ goto _LL1317;}}
else{ goto _LL1317;} _LL1317: if(*(( int*) _temp1313) == Cyc_Absyn_Const_e){
_LL1396: _temp1395=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1313)->f1;
goto _LL1318;} else{ goto _LL1319;} _LL1319: if(*(( int*) _temp1313) == Cyc_Absyn_Var_e){
_LL1400: _temp1399=(( struct Cyc_Absyn_Var_e_struct*) _temp1313)->f1; goto
_LL1398; _LL1398: _temp1397=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1313)->f2;
goto _LL1320;} else{ goto _LL1321;} _LL1321: if(*(( int*) _temp1313) == Cyc_Absyn_UnknownId_e){
_LL1402: _temp1401=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1313)->f1; goto
_LL1322;} else{ goto _LL1323;} _LL1323: if(*(( int*) _temp1313) == Cyc_Absyn_Primop_e){
_LL1406: _temp1405=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1313)->f1;
goto _LL1404; _LL1404: _temp1403=(( struct Cyc_Absyn_Primop_e_struct*) _temp1313)->f2;
goto _LL1324;} else{ goto _LL1325;} _LL1325: if(*(( int*) _temp1313) == Cyc_Absyn_Increment_e){
_LL1410: _temp1409=(( struct Cyc_Absyn_Increment_e_struct*) _temp1313)->f1; goto
_LL1408; _LL1408: _temp1407=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1313)->f2; goto _LL1326;} else{ goto _LL1327;} _LL1327: if(*(( int*)
_temp1313) == Cyc_Absyn_AssignOp_e){ _LL1416: _temp1415=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1313)->f1; goto _LL1414; _LL1414: _temp1413=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1313)->f2; goto _LL1412; _LL1412: _temp1411=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1313)->f3; goto _LL1328;} else{ goto _LL1329;} _LL1329: if(*(( int*)
_temp1313) == Cyc_Absyn_Conditional_e){ _LL1422: _temp1421=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1313)->f1; goto _LL1420; _LL1420: _temp1419=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1313)->f2; goto _LL1418; _LL1418: _temp1417=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1313)->f3; goto _LL1330;} else{ goto _LL1331;} _LL1331: if(*(( int*)
_temp1313) == Cyc_Absyn_SeqExp_e){ _LL1426: _temp1425=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1313)->f1; goto _LL1424; _LL1424: _temp1423=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1313)->f2; goto _LL1332;} else{ goto _LL1333;} _LL1333: if(*(( int*)
_temp1313) == Cyc_Absyn_UnknownCall_e){ _LL1430: _temp1429=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1313)->f1; goto _LL1428; _LL1428: _temp1427=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1313)->f2; goto _LL1334;} else{ goto _LL1335;} _LL1335: if(*(( int*)
_temp1313) == Cyc_Absyn_FnCall_e){ _LL1436: _temp1435=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1313)->f1; goto _LL1434; _LL1434: _temp1433=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1313)->f2; goto _LL1432; _LL1432: _temp1431=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1313)->f3; if( _temp1431 == 0){ goto _LL1336;} else{ goto _LL1337;}} else{
goto _LL1337;} _LL1337: if(*(( int*) _temp1313) == Cyc_Absyn_FnCall_e){ _LL1449:
_temp1448=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1313)->f1; goto _LL1447;
_LL1447: _temp1446=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1313)->f2; goto
_LL1438; _LL1438: _temp1437=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1313)->f3;
if( _temp1437 == 0){ goto _LL1339;} else{ _temp1439=* _temp1437; _LL1445:
_temp1444= _temp1439.num_varargs; goto _LL1443; _LL1443: _temp1442= _temp1439.injectors;
goto _LL1441; _LL1441: _temp1440= _temp1439.vai; goto _LL1338;}} else{ goto
_LL1339;} _LL1339: if(*(( int*) _temp1313) == Cyc_Absyn_Throw_e){ _LL1451:
_temp1450=(( struct Cyc_Absyn_Throw_e_struct*) _temp1313)->f1; goto _LL1340;}
else{ goto _LL1341;} _LL1341: if(*(( int*) _temp1313) == Cyc_Absyn_NoInstantiate_e){
_LL1453: _temp1452=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1313)->f1;
goto _LL1342;} else{ goto _LL1343;} _LL1343: if(*(( int*) _temp1313) == Cyc_Absyn_Instantiate_e){
_LL1457: _temp1456=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1313)->f1;
goto _LL1455; _LL1455: _temp1454=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1313)->f2; goto _LL1344;} else{ goto _LL1345;} _LL1345: if(*(( int*)
_temp1313) == Cyc_Absyn_Cast_e){ _LL1461: _temp1460=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1313)->f1; _temp1462=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1313)->f1); goto _LL1459; _LL1459: _temp1458=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1313)->f2; goto _LL1346;} else{ goto _LL1347;} _LL1347: if(*(( int*)
_temp1313) == Cyc_Absyn_Address_e){ _LL1464: _temp1463=(( struct Cyc_Absyn_Address_e_struct*)
_temp1313)->f1; goto _LL1348;} else{ goto _LL1349;} _LL1349: if(*(( int*)
_temp1313) == Cyc_Absyn_New_e){ _LL1468: _temp1467=(( struct Cyc_Absyn_New_e_struct*)
_temp1313)->f1; goto _LL1466; _LL1466: _temp1465=(( struct Cyc_Absyn_New_e_struct*)
_temp1313)->f2; goto _LL1350;} else{ goto _LL1351;} _LL1351: if(*(( int*)
_temp1313) == Cyc_Absyn_Sizeofexp_e){ _LL1470: _temp1469=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1313)->f1; goto _LL1352;} else{ goto _LL1353;} _LL1353: if(*(( int*)
_temp1313) == Cyc_Absyn_Sizeoftyp_e){ _LL1472: _temp1471=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1313)->f1; goto _LL1354;} else{ goto _LL1355;} _LL1355: if(*(( int*)
_temp1313) == Cyc_Absyn_Offsetof_e){ _LL1476: _temp1475=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1313)->f1; goto _LL1474; _LL1474: _temp1473=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1313)->f2; goto _LL1356;} else{ goto _LL1357;} _LL1357: if(*(( int*)
_temp1313) == Cyc_Absyn_Deref_e){ _LL1478: _temp1477=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1313)->f1; goto _LL1358;} else{ goto _LL1359;} _LL1359: if(*(( int*)
_temp1313) == Cyc_Absyn_StructMember_e){ _LL1482: _temp1481=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1313)->f1; goto _LL1480; _LL1480: _temp1479=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1313)->f2; goto _LL1360;} else{ goto _LL1361;} _LL1361: if(*(( int*)
_temp1313) == Cyc_Absyn_StructArrow_e){ _LL1486: _temp1485=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1313)->f1; goto _LL1484; _LL1484: _temp1483=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1313)->f2; goto _LL1362;} else{ goto _LL1363;} _LL1363: if(*(( int*)
_temp1313) == Cyc_Absyn_Subscript_e){ _LL1490: _temp1489=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1313)->f1; goto _LL1488; _LL1488: _temp1487=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1313)->f2; goto _LL1364;} else{ goto _LL1365;} _LL1365: if(*(( int*)
_temp1313) == Cyc_Absyn_Tuple_e){ _LL1492: _temp1491=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1313)->f1; goto _LL1366;} else{ goto _LL1367;} _LL1367: if(*(( int*)
_temp1313) == Cyc_Absyn_CompoundLit_e){ _LL1496: _temp1495=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1313)->f1; goto _LL1494; _LL1494: _temp1493=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1313)->f2; goto _LL1368;} else{ goto _LL1369;} _LL1369: if(*(( int*)
_temp1313) == Cyc_Absyn_Array_e){ _LL1498: _temp1497=(( struct Cyc_Absyn_Array_e_struct*)
_temp1313)->f1; goto _LL1370;} else{ goto _LL1371;} _LL1371: if(*(( int*)
_temp1313) == Cyc_Absyn_Comprehension_e){ _LL1504: _temp1503=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1313)->f1; goto _LL1502; _LL1502: _temp1501=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1313)->f2; goto _LL1500; _LL1500: _temp1499=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1313)->f3; goto _LL1372;} else{ goto _LL1373;} _LL1373: if(*(( int*)
_temp1313) == Cyc_Absyn_Struct_e){ _LL1512: _temp1511=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1313)->f1; goto _LL1510; _LL1510: _temp1509=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1313)->f2; goto _LL1508; _LL1508: _temp1507=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1313)->f3; goto _LL1506; _LL1506: _temp1505=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1313)->f4; goto _LL1374;} else{ goto _LL1375;} _LL1375: if(*(( int*)
_temp1313) == Cyc_Absyn_AnonStruct_e){ _LL1516: _temp1515=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1313)->f1; goto _LL1514; _LL1514: _temp1513=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1313)->f2; goto _LL1376;} else{ goto _LL1377;} _LL1377: if(*(( int*)
_temp1313) == Cyc_Absyn_Tunion_e){ _LL1526: _temp1525=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1313)->f1; goto _LL1524; _LL1524: _temp1523=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1313)->f2; goto _LL1522; _LL1522: _temp1521=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1313)->f3; if( _temp1521 == 0){ goto _LL1520;} else{ goto _LL1379;} _LL1520:
_temp1519=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1313)->f4; goto _LL1518;
_LL1518: _temp1517=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1313)->f5; goto
_LL1378;} else{ goto _LL1379;} _LL1379: if(*(( int*) _temp1313) == Cyc_Absyn_Tunion_e){
_LL1536: _temp1535=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1313)->f1; goto
_LL1534; _LL1534: _temp1533=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1313)->f2;
goto _LL1532; _LL1532: _temp1531=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1313)->f3;
goto _LL1530; _LL1530: _temp1529=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1313)->f4;
goto _LL1528; _LL1528: _temp1527=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1313)->f5;
goto _LL1380;} else{ goto _LL1381;} _LL1381: if(*(( int*) _temp1313) == Cyc_Absyn_Enum_e){
_LL1542: _temp1541=(( struct Cyc_Absyn_Enum_e_struct*) _temp1313)->f1; goto
_LL1540; _LL1540: _temp1539=(( struct Cyc_Absyn_Enum_e_struct*) _temp1313)->f2;
goto _LL1538; _LL1538: _temp1537=(( struct Cyc_Absyn_Enum_e_struct*) _temp1313)->f3;
goto _LL1382;} else{ goto _LL1383;} _LL1383: if(*(( int*) _temp1313) == Cyc_Absyn_Malloc_e){
_LL1546: _temp1545=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1313)->f1; goto
_LL1544; _LL1544: _temp1543=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1313)->f2; goto _LL1384;} else{ goto _LL1385;} _LL1385: if(*(( int*)
_temp1313) == Cyc_Absyn_StmtExp_e){ _LL1548: _temp1547=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1313)->f1; goto _LL1386;} else{ goto _LL1387;} _LL1387: if(*(( int*)
_temp1313) == Cyc_Absyn_UnresolvedMem_e){ _LL1552: _temp1551=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1313)->f1; goto _LL1550; _LL1550: _temp1549=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1313)->f2; goto _LL1388;} else{ goto _LL1389;} _LL1389: if(*(( int*)
_temp1313) == Cyc_Absyn_Codegen_e){ _LL1554: _temp1553=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1313)->f1; goto _LL1390;} else{ goto _LL1391;} _LL1391: if(*(( int*)
_temp1313) == Cyc_Absyn_Fill_e){ _LL1556: _temp1555=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1313)->f1; goto _LL1392;} else{ goto _LL1314;} _LL1316:{ void* _temp1557=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1563; struct Cyc_Absyn_Conref*
_temp1565; struct Cyc_Absyn_Tqual _temp1567; struct Cyc_Absyn_Conref* _temp1569;
void* _temp1571; void* _temp1573; _LL1559: if(( unsigned int) _temp1557 > 4u?*((
int*) _temp1557) == Cyc_Absyn_PointerType: 0){ _LL1564: _temp1563=(( struct Cyc_Absyn_PointerType_struct*)
_temp1557)->f1; _LL1574: _temp1573=( void*) _temp1563.elt_typ; goto _LL1572;
_LL1572: _temp1571=( void*) _temp1563.rgn_typ; goto _LL1570; _LL1570: _temp1569=
_temp1563.nullable; goto _LL1568; _LL1568: _temp1567= _temp1563.tq; goto _LL1566;
_LL1566: _temp1565= _temp1563.bounds; goto _LL1560;} else{ goto _LL1561;}
_LL1561: goto _LL1562; _LL1560: if(( void*) _temp1565->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1558;}{ void* _temp1575=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1565); struct Cyc_Absyn_Exp* _temp1581; _LL1577: if( _temp1575 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1578;} else{ goto _LL1579;} _LL1579: if((
unsigned int) _temp1575 > 1u?*(( int*) _temp1575) == Cyc_Absyn_Upper_b: 0){
_LL1582: _temp1581=(( struct Cyc_Absyn_Upper_b_struct*) _temp1575)->f1; goto
_LL1580;} else{ goto _LL1576;} _LL1578: { struct _tuple4* _temp1584=({ struct
_tuple4* _temp1583=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1583->f1= 0; _temp1583->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1583;});
goto _LL1585; _LL1585: { struct Cyc_Absyn_Exp* _temp1592= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List*(* _temp1586)( struct _tagged_ptr4)=( struct Cyc_List_List*(*)(
struct _tagged_ptr4)) Cyc_List_list; struct _tuple4* _temp1589= _temp1584;
struct _tuple4* _temp1590= _temp1584; struct _tuple4* _temp1591= _temp1584;
struct _tuple4* _temp1587[ 3u]={ _temp1589, _temp1590, _temp1591}; struct
_tagged_ptr4 _temp1588={ _temp1587, _temp1587, _temp1587 + 3u}; _temp1586(
_temp1588);}), 0); goto _LL1593; _LL1593: if( nv->toplevel){( void*)( e->r=(
void*)(( void*) _temp1592->r));} else{ void* _temp1594= Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1573), Cyc_Toc_mt_tq); goto _LL1595; _LL1595: { struct
_tuple0* _temp1596= Cyc_Toc_temp_var(); goto _LL1597; _LL1597: { struct Cyc_Absyn_Exp*
_temp1598= Cyc_Absyn_var_exp( _temp1596, 0); goto _LL1599; _LL1599: { struct Cyc_Absyn_Stmt*
_temp1600= Cyc_Absyn_declare_stmt( _temp1596, _temp1594,( struct Cyc_Absyn_Exp*)
_temp1592, Cyc_Absyn_exp_stmt( _temp1598, 0), 0); goto _LL1601; _LL1601:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1600, 0))->r));}}}} goto
_LL1576;}} _LL1580:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0,
0))->r)); goto _LL1576; _LL1576:;} goto _LL1558; _LL1562:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1602= Cyc_Absynpp_typ2string( old_typ); xprintf("exp_to_c: null given type %.*s",
_temp1602.last_plus_one - _temp1602.curr, _temp1602.curr);})); goto _LL1558;
_LL1558:;} goto _LL1314; _LL1318: goto _LL1314; _LL1320:{ struct _handler_cons
_temp1603; _push_handler(& _temp1603);{ int _temp1605= 0; if( setjmp( _temp1603.handler)){
_temp1605= 1;} if( ! _temp1605){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1399))->r));; _pop_handler();} else{ void* _temp1604=( void*) _exn_thrown;
void* _temp1607= _temp1604; _LL1609: if( _temp1607 == Cyc_Dict_Absent){ goto
_LL1610;} else{ goto _LL1611;} _LL1611: goto _LL1612; _LL1610:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct
_tagged_string _temp1613= Cyc_Absynpp_qvar2string( _temp1399); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1613.last_plus_one - _temp1613.curr, _temp1613.curr);})); return; _LL1612:(
void) _throw( _temp1607); _LL1608:;}}} goto _LL1314; _LL1322:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1614=(
unsigned char*)"unknownid"; struct _tagged_string _temp1615; _temp1615.curr=
_temp1614; _temp1615.base= _temp1614; _temp1615.last_plus_one= _temp1614 + 10;
_temp1615;})); goto _LL1314; _LL1324: { struct Cyc_List_List* _temp1616=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp1403); goto _LL1617; _LL1617: {
struct Cyc_List_List* _temp1618=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1403); goto _LL1619; _LL1619:(( void(*)( void(* f)( struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)(
Cyc_Toc_exp_to_c, nv, _temp1403);{ void* _temp1620= _temp1405; _LL1622: if(
_temp1620 ==( void*) Cyc_Absyn_Size){ goto _LL1623;} else{ goto _LL1624;}
_LL1624: if( _temp1620 ==( void*) Cyc_Absyn_Printf){ goto _LL1625;} else{ goto
_LL1626;} _LL1626: if( _temp1620 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1627;}
else{ goto _LL1628;} _LL1628: if( _temp1620 ==( void*) Cyc_Absyn_Fprintf){ goto
_LL1629;} else{ goto _LL1630;} _LL1630: if( _temp1620 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1631;} else{ goto _LL1632;} _LL1632: if( _temp1620 ==( void*) Cyc_Absyn_Plus){
goto _LL1633;} else{ goto _LL1634;} _LL1634: if( _temp1620 ==( void*) Cyc_Absyn_Minus){
goto _LL1635;} else{ goto _LL1636;} _LL1636: if( _temp1620 ==( void*) Cyc_Absyn_Eq){
goto _LL1637;} else{ goto _LL1638;} _LL1638: if( _temp1620 ==( void*) Cyc_Absyn_Neq){
goto _LL1639;} else{ goto _LL1640;} _LL1640: if( _temp1620 ==( void*) Cyc_Absyn_Gt){
goto _LL1641;} else{ goto _LL1642;} _LL1642: if( _temp1620 ==( void*) Cyc_Absyn_Gte){
goto _LL1643;} else{ goto _LL1644;} _LL1644: if( _temp1620 ==( void*) Cyc_Absyn_Lt){
goto _LL1645;} else{ goto _LL1646;} _LL1646: if( _temp1620 ==( void*) Cyc_Absyn_Lte){
goto _LL1647;} else{ goto _LL1648;} _LL1648: goto _LL1649; _LL1623: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1403))->hd;{
void* _temp1650=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1658; struct Cyc_Absyn_Tqual _temp1660; void*
_temp1662; struct Cyc_Absyn_PtrInfo _temp1664; struct Cyc_Absyn_Conref*
_temp1666; struct Cyc_Absyn_Tqual _temp1668; struct Cyc_Absyn_Conref* _temp1670;
void* _temp1672; void* _temp1674; _LL1652: if(( unsigned int) _temp1650 > 4u?*((
int*) _temp1650) == Cyc_Absyn_ArrayType: 0){ _LL1663: _temp1662=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1650)->f1; goto _LL1661; _LL1661:
_temp1660=(( struct Cyc_Absyn_ArrayType_struct*) _temp1650)->f2; goto _LL1659;
_LL1659: _temp1658=(( struct Cyc_Absyn_ArrayType_struct*) _temp1650)->f3; goto
_LL1653;} else{ goto _LL1654;} _LL1654: if(( unsigned int) _temp1650 > 4u?*((
int*) _temp1650) == Cyc_Absyn_PointerType: 0){ _LL1665: _temp1664=(( struct Cyc_Absyn_PointerType_struct*)
_temp1650)->f1; _LL1675: _temp1674=( void*) _temp1664.elt_typ; goto _LL1673;
_LL1673: _temp1672=( void*) _temp1664.rgn_typ; goto _LL1671; _LL1671: _temp1670=
_temp1664.nullable; goto _LL1669; _LL1669: _temp1668= _temp1664.tq; goto _LL1667;
_LL1667: _temp1666= _temp1664.bounds; goto _LL1655;} else{ goto _LL1656;}
_LL1656: goto _LL1657; _LL1653:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1658))->r)); goto _LL1651; _LL1655: { struct _tuple0*
_temp1676= Cyc_Toc_temp_var(); goto _LL1677; _LL1677: { void* _temp1678= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); goto _LL1679;
_LL1679: { struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp1676, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1676, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4=
Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp( e2, e3, 0), 0);
struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1676, _temp1678,(
struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1651;}}} _LL1657:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
struct _tagged_string _temp1680= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp1618))->hd); struct _tagged_string _temp1681= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1680.last_plus_one - _temp1680.curr, _temp1680.curr, _temp1681.last_plus_one
- _temp1681.curr, _temp1681.curr);})); return; _LL1651:;} goto _LL1621;} _LL1625:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1405, 0, _temp1403,
_temp1618))->r)); goto _LL1621; _LL1627:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1405, 0, _temp1403, _temp1618))->r)); goto _LL1621; _LL1629: if( _temp1403
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char*
_temp1682=( unsigned char*)"fprintf without arguments"; struct _tagged_string
_temp1683; _temp1683.curr= _temp1682; _temp1683.base= _temp1682; _temp1683.last_plus_one=
_temp1682 + 26; _temp1683;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1405,({ struct Cyc_Core_Opt* _temp1684=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1684->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1403))->hd); _temp1684;}),(( struct Cyc_List_List*)
_check_null( _temp1403))->tl,(( struct Cyc_List_List*) _check_null( _temp1618))->tl))->r));
goto _LL1621; _LL1631: if( _temp1403 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ unsigned char* _temp1685=( unsigned char*)"sscanf without arguments";
struct _tagged_string _temp1686; _temp1686.curr= _temp1685; _temp1686.base=
_temp1685; _temp1686.last_plus_one= _temp1685 + 25; _temp1686;}));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1403))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1403))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1403))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1621; _LL1633: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1403))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1403))->tl))->hd; struct _tuple0* _temp1687= Cyc_Toc_temp_var(); goto
_LL1688; _LL1688: { void* _temp1689= Cyc_Toc_typ_to_c( old_typ); goto _LL1690;
_LL1690: { struct Cyc_Absyn_Stmt* _temp1691= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1687, 0), 0); goto _LL1692; _LL1692: { struct Cyc_Absyn_Exp* _temp1693= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1687, 0), Cyc_Toc_curr_sp, 0); goto _LL1694; _LL1694: {
struct Cyc_Absyn_Exp* _temp1696= Cyc_Absyn_assignop_exp( _temp1693,({ struct Cyc_Core_Opt*
_temp1695=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1695->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1695;}), e2, 0); goto
_LL1697; _LL1697: _temp1691= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1696,
0), _temp1691, 0); _temp1691= Cyc_Absyn_declare_stmt( _temp1687, _temp1689,(
struct Cyc_Absyn_Exp*) e1, _temp1691, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1691, 0))->r));}}}}} goto _LL1621; _LL1635: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null( _temp1616))->hd)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1403))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1403))->tl))->hd; void*
old_typ_e2=( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1616))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple0* _temp1698= Cyc_Toc_temp_var(); goto _LL1699;
_LL1699: { void* _temp1700= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_List_List*)
_check_null( _temp1616))->hd); goto _LL1701; _LL1701: { struct Cyc_Absyn_Stmt*
_temp1702= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1698, 0), 0); goto
_LL1703; _LL1703: { struct Cyc_Absyn_Exp* _temp1704= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp1698, 0), Cyc_Toc_curr_sp, 0); goto _LL1705; _LL1705: {
struct Cyc_Absyn_Exp* _temp1707= Cyc_Absyn_assignop_exp( _temp1704,({ struct Cyc_Core_Opt*
_temp1706=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1706->v=( void*)(( void*) Cyc_Absyn_Minus); _temp1706;}), e2, 0); goto
_LL1708; _LL1708: _temp1702= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( _temp1707,
0), _temp1702, 0); _temp1702= Cyc_Absyn_declare_stmt( _temp1698, _temp1700,(
struct Cyc_Absyn_Exp*) e1, _temp1702, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1702, 0))->r));}}}}}} goto _LL1621; _LL1637: goto _LL1639; _LL1639: goto
_LL1641; _LL1641: goto _LL1643; _LL1643: goto _LL1645; _LL1645: goto _LL1647;
_LL1647: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1403))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1403))->tl))->hd; void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp1616))->hd; void* t2=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1616))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1621;} _LL1649: goto
_LL1621; _LL1621:;} goto _LL1314;}} _LL1326: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1409->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1409); if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1407 ==(
void*) Cyc_Absyn_PreInc? 1: _temp1407 ==( void*) Cyc_Absyn_PreDec){ struct
_tuple0* _temp1709= Cyc_Toc_temp_var(); goto _LL1710; _LL1710: { void* _temp1711=
Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1712; _LL1712: { struct Cyc_Absyn_Stmt*
_temp1713= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( _temp1709,
0), 0), 0); goto _LL1714; _LL1714: { struct Cyc_Absyn_Exp* _temp1715= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1709, 0), Cyc_Toc_curr_sp, 0),
_temp1407, 0); goto _LL1716; _LL1716: _temp1713= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1715, 0), _temp1713, 0); _temp1713= Cyc_Absyn_declare_stmt( _temp1709,
_temp1711,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1409, 0),
_temp1713, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1713, 0))->r));}}}}
else{ struct _tuple0* _temp1717= Cyc_Toc_temp_var(); goto _LL1718; _LL1718: {
struct _tuple0* _temp1719= Cyc_Toc_temp_var(); goto _LL1720; _LL1720: { void*
_temp1721= Cyc_Toc_typ_to_c( e2_cyc_typ); goto _LL1722; _LL1722: { void*
_temp1723= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); goto _LL1724; _LL1724: { struct Cyc_Absyn_Stmt*
_temp1725= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1719, 0), 0); goto
_LL1726; _LL1726: { struct Cyc_Absyn_Exp* _temp1727= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( _temp1717, 0), Cyc_Toc_curr_sp, 0),
_temp1407, 0); goto _LL1728; _LL1728: _temp1725= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
_temp1727, 0), _temp1725, 0); _temp1725= Cyc_Absyn_declare_stmt( _temp1719,
_temp1721,( struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
_temp1717, 0), 0), _temp1725, 0); _temp1725= Cyc_Absyn_declare_stmt( _temp1717,
_temp1723,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1409, 0),
_temp1725, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1725, 0))->r));}}}}}}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1409)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1409, 0, Cyc_Toc_incr_lvalue,
_temp1407);( void*)( e->r=( void*)(( void*) _temp1409->r));}} goto _LL1314;}
_LL1328: { int e1_poly= Cyc_Toc_is_poly_project( _temp1415); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1415->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1411->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1415); Cyc_Toc_exp_to_c( nv, _temp1411); if(
_temp1413 != 0?( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1413))->v ==(
void*) Cyc_Absyn_Plus? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1413))->v
==( void*) Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){(
void*)( _temp1415->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1415->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1411->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1411->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1411->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1411->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1415)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1415, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1729=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1729[ 0]=({ struct _tuple11 _temp1730; _temp1730.f1=
_temp1413; _temp1730.f2= _temp1411; _temp1730;}); _temp1729;}));( void*)( e->r=(
void*)(( void*) _temp1415->r));} goto _LL1314;} _LL1330: Cyc_Toc_exp_to_c( nv,
_temp1421); Cyc_Toc_exp_to_c( nv, _temp1419); Cyc_Toc_exp_to_c( nv, _temp1417);
goto _LL1314; _LL1332: Cyc_Toc_exp_to_c( nv, _temp1425); Cyc_Toc_exp_to_c( nv,
_temp1423); goto _LL1314; _LL1334: _temp1435= _temp1429; _temp1433= _temp1427;
goto _LL1336; _LL1336: Cyc_Toc_exp_to_c( nv, _temp1435);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1433); goto _LL1314; _LL1338:{
struct _RegionHandle _temp1731= _new_region(); struct _RegionHandle* r=&
_temp1731; _push_region( r);{ void* _temp1732=( void*) _temp1440->type; goto
_LL1733; _LL1733: { void* _temp1734= Cyc_Toc_typ_to_c( _temp1732); goto _LL1735;
_LL1735: { void* _temp1736= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1448->topt))->v); goto _LL1737; _LL1737: { struct Cyc_Absyn_Exp*
_temp1738= Cyc_Absyn_uint_exp(( unsigned int) _temp1444, 0); goto _LL1739;
_LL1739: { void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1808=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1808[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1809; _temp1809.tag= Cyc_Absyn_ArrayType;
_temp1809.f1=( void*) _temp1732; _temp1809.f2= Cyc_Toc_mt_tq; _temp1809.f3=(
struct Cyc_Absyn_Exp*) _temp1738; _temp1809;}); _temp1808;})); struct _tuple0*
_temp1740= Cyc_Toc_temp_var(); goto _LL1741; _LL1741: { struct _tuple0*
_temp1742= Cyc_Toc_temp_var(); goto _LL1743; _LL1743: { struct Cyc_Absyn_Exp*
_temp1744= Cyc_Absyn_var_exp( _temp1742, 0); goto _LL1745; _LL1745: { struct
_tuple0* _temp1746= Cyc_Toc_temp_var(); goto _LL1747; _LL1747: { int _temp1748=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1446); goto _LL1749;
_LL1749: { int _temp1750= _temp1748 - _temp1444; goto _LL1751; _LL1751: { struct
_tagged_ptr6 vs=({ unsigned int _temp1804=( unsigned int) _temp1444; struct
_tuple0** _temp1805=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1804); struct _tagged_ptr6 _temp1807={ _temp1805, _temp1805,
_temp1805 + _temp1804};{ unsigned int _temp1806= _temp1804; unsigned int i; for(
i= 0; i < _temp1806; i ++){ _temp1805[ i]= Cyc_Toc_temp_var();}}; _temp1807;});
struct _tagged_ptr6 xs=({ unsigned int _temp1800=( unsigned int) _temp1750;
struct _tuple0** _temp1801=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1800); struct _tagged_ptr6 _temp1803={ _temp1801, _temp1801,
_temp1801 + _temp1800};{ unsigned int _temp1802= _temp1800; unsigned int i; for(
i= 0; i < _temp1802; i ++){ _temp1801[ i]= Cyc_Toc_temp_var();}}; _temp1803;});
struct Cyc_List_List* xexps=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(*
f)( struct _tuple0*, struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,( struct Cyc_Position_Segment*)
0,(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct _tagged_ptr6 arr))
Cyc_List_rfrom_array)( r, xs)); xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List*
_temp1752=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1752->hd=( void*) Cyc_Absyn_var_exp( _temp1746, 0); _temp1752->tl= 0;
_temp1752;}));{ struct Cyc_Absyn_Stmt* _temp1753= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( _temp1740, 0), xexps, 0), 0); goto _LL1754; _LL1754: { struct
Cyc_Absyn_Exp* _temp1755= Cyc_Absyn_add_exp( _temp1744, _temp1738, 0); goto
_LL1756; _LL1756: { struct Cyc_List_List* _temp1766=({ struct Cyc_List_List*(*
_temp1757)( struct _tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4))
Cyc_List_list; struct _tuple4* _temp1760=({ struct _tuple4* _temp1765=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1765->f1= 0; _temp1765->f2=
_temp1744; _temp1765;}); struct _tuple4* _temp1761=({ struct _tuple4* _temp1764=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1764->f1= 0;
_temp1764->f2= _temp1744; _temp1764;}); struct _tuple4* _temp1762=({ struct
_tuple4* _temp1763=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1763->f1= 0; _temp1763->f2= _temp1755; _temp1763;}); struct _tuple4*
_temp1758[ 3u]={ _temp1760, _temp1761, _temp1762}; struct _tagged_ptr4 _temp1759={
_temp1758, _temp1758, _temp1758 + 3u}; _temp1757( _temp1759);}); goto _LL1767;
_LL1767: _temp1753= Cyc_Absyn_declare_stmt( _temp1746, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp1732,( void*) _temp1440->rgn, Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp(
0, _temp1766, 0), _temp1753, 0);{ struct Cyc_List_List* _temp1768=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_ptr6 arr)) Cyc_List_rfrom_array)( r, vs));
goto _LL1769; _LL1769: { struct Cyc_List_List* _temp1770=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_make_dle, _temp1768); goto _LL1771; _LL1771: _temp1753= Cyc_Absyn_declare_stmt(
_temp1742, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1770, 0), _temp1753, 0);{ struct Cyc_List_List* _temp1772=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1446);
goto _LL1773; _LL1773: { struct Cyc_List_List* _temp1774=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1442);
goto _LL1775; _LL1775:{ int i= _temp1444 - 1; for( 0; i >= 0;( i --, _temp1772=((
struct Cyc_List_List*) _check_null( _temp1772))->tl)){ struct Cyc_Absyn_Exp* arg=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1772))->hd;
void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c(
nv, arg); if( _temp1440->inject){ void* _temp1778; struct Cyc_Position_Segment*
_temp1780; struct Cyc_List_List* _temp1782; struct Cyc_List_List* _temp1784;
struct _tuple0* _temp1786; struct Cyc_Absyn_Tunionfield _temp1776=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1774))->hd); _LL1787: _temp1786=
_temp1776.name; goto _LL1785; _LL1785: _temp1784= _temp1776.tvs; goto _LL1783;
_LL1783: _temp1782= _temp1776.typs; goto _LL1781; _LL1781: _temp1780= _temp1776.loc;
goto _LL1779; _LL1779: _temp1778=( void*) _temp1776.sc; goto _LL1777; _LL1777:
_temp1774=(( struct Cyc_List_List*) _check_null( _temp1774))->tl;{ struct
_tuple0* _temp1788= Cyc_Toc_temp_var(); goto _LL1789; _LL1789: { struct Cyc_Absyn_Exp*
_temp1790= Cyc_Absyn_var_exp( _temp1788, 0); goto _LL1791; _LL1791: { void*
_temp1794= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1786,({
unsigned char* _temp1792=( unsigned char*)"_struct"; struct _tagged_string
_temp1793; _temp1793.curr= _temp1792; _temp1793.base= _temp1792; _temp1793.last_plus_one=
_temp1792 + 8; _temp1793;}))); goto _LL1795; _LL1795: _temp1753= Cyc_Absyn_declare_stmt(*((
struct _tuple0**(*)( struct _tagged_ptr6, unsigned int, unsigned int))
_check_unknown_subscript)( vs, sizeof( struct _tuple0*), i), _temp1734,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1734, Cyc_Absyn_address_exp( _temp1790,
0), 0), _temp1753, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp1782))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1796= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1790, Cyc_Toc_fieldname(
1), 0), arg, 0); goto _LL1797; _LL1797: _temp1753= Cyc_Absyn_seq_stmt( _temp1796,
_temp1753, 0);{ struct Cyc_Absyn_Stmt* _temp1798= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1790, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1786, 0), 0); goto
_LL1799; _LL1799: _temp1753= Cyc_Absyn_seq_stmt( _temp1798, _temp1753, 0);
_temp1753= Cyc_Absyn_declare_stmt( _temp1788, _temp1794, 0, _temp1753, 0);}}}}}}}
else{ _temp1753= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr6, unsigned int, unsigned int)) _check_unknown_subscript)( vs,
sizeof( struct _tuple0*), i), _temp1734,( struct Cyc_Absyn_Exp*) arg, _temp1753,
0);}}}{ int i= _temp1750 - 1; for( 0; i >= 0;( i --, _temp1772=(( struct Cyc_List_List*)
_check_null( _temp1772))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1772))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1753= Cyc_Absyn_declare_stmt(*(( struct _tuple0**(*)( struct
_tagged_ptr6, unsigned int, unsigned int)) _check_unknown_subscript)( xs,
sizeof( struct _tuple0*), i), argtype,( struct Cyc_Absyn_Exp*) arg, _temp1753, 0);}}
Cyc_Toc_exp_to_c( nv, _temp1448); _temp1753= Cyc_Absyn_declare_stmt( _temp1740,
_temp1736,( struct Cyc_Absyn_Exp*) _temp1448, _temp1753, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1753, 0))->r));}}}}}}}}}}}}}}}}}};
_pop_region(& _temp1731);} goto _LL1314; _LL1340: Cyc_Toc_exp_to_c( nv,
_temp1450);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1450), 0))->r)); goto _LL1314; _LL1342: Cyc_Toc_exp_to_c(
nv, _temp1452); goto _LL1314; _LL1344: Cyc_Toc_exp_to_c( nv, _temp1456);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
for( 0; _temp1454 != 0; _temp1454=(( struct Cyc_List_List*) _check_null(
_temp1454))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*)
_check_null( _temp1454))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*)
Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1456, 0))->r)); break;}} goto _LL1314;} _LL1346: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1458->topt))->v; void* new_typ=*
_temp1462;* _temp1462= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1458);{ int noconv= 0;{ void* _temp1810= Cyc_Toc_conversion( old_t2,
new_typ); void* _temp1822; struct Cyc_Absyn_Exp* _temp1824; void* _temp1826; int
_temp1828; struct Cyc_Absyn_Exp* _temp1830; void* _temp1832; void* _temp1834;
_LL1812: if( _temp1810 ==( void*) Cyc_Toc_NoConv){ goto _LL1813;} else{ goto
_LL1814;} _LL1814: if(( unsigned int) _temp1810 > 1u?*(( int*) _temp1810) == Cyc_Toc_NullCheck:
0){ _LL1823: _temp1822=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp1810)->f1;
goto _LL1815;} else{ goto _LL1816;} _LL1816: if(( unsigned int) _temp1810 > 1u?*((
int*) _temp1810) == Cyc_Toc_TagPtr: 0){ _LL1827: _temp1826=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp1810)->f1; goto _LL1825; _LL1825: _temp1824=(( struct Cyc_Toc_TagPtr_struct*)
_temp1810)->f2; goto _LL1817;} else{ goto _LL1818;} _LL1818: if(( unsigned int)
_temp1810 > 1u?*(( int*) _temp1810) == Cyc_Toc_UntagPtr: 0){ _LL1833: _temp1832=(
void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp1810)->f1; goto _LL1831; _LL1831:
_temp1830=(( struct Cyc_Toc_UntagPtr_struct*) _temp1810)->f2; goto _LL1829;
_LL1829: _temp1828=(( struct Cyc_Toc_UntagPtr_struct*) _temp1810)->f3; goto
_LL1819;} else{ goto _LL1820;} _LL1820: if(( unsigned int) _temp1810 > 1u?*((
int*) _temp1810) == Cyc_Toc_UntagPtrNocheck: 0){ _LL1835: _temp1834=( void*)((
struct Cyc_Toc_UntagPtrNocheck_struct*) _temp1810)->f1; goto _LL1821;} else{
goto _LL1811;} _LL1813: goto _LL1811; _LL1815:( void*)( e->r=( void*)(( void*)(
Cyc_Toc_null_check_conv( nv->toplevel, _temp1822, _temp1458))->r)); goto _LL1811;
_LL1817:( void*)( e->r=( void*)(( void*)( Cyc_Toc_tag_ptr_conv( nv->toplevel,
_temp1826, _temp1824, _temp1458))->r)); goto _LL1811; _LL1819:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_untag_ptr_conv( nv->toplevel, _temp1832, _temp1830,
_temp1828, _temp1458))->r)); goto _LL1811; _LL1821:( void*)( _temp1458->r=( void*)((
void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp1458->r,
_temp1458->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1811; _LL1811:;} goto
_LL1314;}} _LL1348:{ void* _temp1836=( void*) _temp1463->r; struct Cyc_Absyn_Structdecl*
_temp1844; struct Cyc_List_List* _temp1846; struct Cyc_Core_Opt* _temp1848;
struct _tuple0* _temp1850; struct Cyc_List_List* _temp1852; _LL1838: if(*(( int*)
_temp1836) == Cyc_Absyn_Struct_e){ _LL1851: _temp1850=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1836)->f1; goto _LL1849; _LL1849: _temp1848=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1836)->f2; goto _LL1847; _LL1847: _temp1846=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1836)->f3; goto _LL1845; _LL1845: _temp1844=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1836)->f4; goto _LL1839;} else{ goto _LL1840;} _LL1840: if(*(( int*)
_temp1836) == Cyc_Absyn_Tuple_e){ _LL1853: _temp1852=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1836)->f1; goto _LL1841;} else{ goto _LL1842;} _LL1842: goto _LL1843;
_LL1839: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ struct _tagged_string _temp1854=( struct _tagged_string)
Cyc_Position_string_of_segment( _temp1463->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1854.last_plus_one - _temp1854.curr, _temp1854.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1463->topt))->v, 1, 0, _temp1846, _temp1850))->r)); goto
_LL1837; _LL1841: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ struct _tagged_string _temp1855=( struct _tagged_string)
Cyc_Position_string_of_segment( _temp1463->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1855.last_plus_one - _temp1855.curr, _temp1855.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1852))->r)); goto _LL1837;
_LL1843: Cyc_Toc_exp_to_c( nv, _temp1463); if( ! Cyc_Absyn_is_lvalue( _temp1463)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1463,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1463, 0))->r));} goto _LL1837; _LL1837:;} goto _LL1314; _LL1350: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
struct _tagged_string _temp1856=( struct _tagged_string) Cyc_Position_string_of_segment(
_temp1465->loc); xprintf("%.*s: new at top-level", _temp1856.last_plus_one -
_temp1856.curr, _temp1856.curr);}));}{ void* _temp1857=( void*) _temp1465->r;
struct Cyc_List_List* _temp1869; struct Cyc_Absyn_Exp* _temp1871; struct Cyc_Absyn_Exp*
_temp1873; struct Cyc_Absyn_Vardecl* _temp1875; struct Cyc_Absyn_Structdecl*
_temp1877; struct Cyc_List_List* _temp1879; struct Cyc_Core_Opt* _temp1881;
struct _tuple0* _temp1883; struct Cyc_List_List* _temp1885; _LL1859: if(*(( int*)
_temp1857) == Cyc_Absyn_Array_e){ _LL1870: _temp1869=(( struct Cyc_Absyn_Array_e_struct*)
_temp1857)->f1; goto _LL1860;} else{ goto _LL1861;} _LL1861: if(*(( int*)
_temp1857) == Cyc_Absyn_Comprehension_e){ _LL1876: _temp1875=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1857)->f1; goto _LL1874; _LL1874: _temp1873=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1857)->f2; goto _LL1872; _LL1872: _temp1871=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1857)->f3; goto _LL1862;} else{ goto _LL1863;} _LL1863: if(*(( int*)
_temp1857) == Cyc_Absyn_Struct_e){ _LL1884: _temp1883=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1857)->f1; goto _LL1882; _LL1882: _temp1881=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1857)->f2; goto _LL1880; _LL1880: _temp1879=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1857)->f3; goto _LL1878; _LL1878: _temp1877=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1857)->f4; goto _LL1864;} else{ goto _LL1865;} _LL1865: if(*(( int*)
_temp1857) == Cyc_Absyn_Tuple_e){ _LL1886: _temp1885=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1857)->f1; goto _LL1866;} else{ goto _LL1867;} _LL1867: goto _LL1868;
_LL1860: { struct _tuple0* _temp1887= Cyc_Toc_temp_var(); goto _LL1888; _LL1888: {
struct Cyc_Absyn_Exp* _temp1889= Cyc_Absyn_var_exp( _temp1887, 0); goto _LL1890;
_LL1890: { struct Cyc_Absyn_Stmt* _temp1891= Cyc_Toc_init_array( nv, _temp1889,
_temp1869, Cyc_Absyn_exp_stmt( _temp1889, 0)); goto _LL1892; _LL1892: { void*
old_elt_typ;{ void* _temp1893= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1899; struct Cyc_Absyn_Conref* _temp1901; struct Cyc_Absyn_Tqual _temp1903;
struct Cyc_Absyn_Conref* _temp1905; void* _temp1907; void* _temp1909; _LL1895:
if(( unsigned int) _temp1893 > 4u?*(( int*) _temp1893) == Cyc_Absyn_PointerType:
0){ _LL1900: _temp1899=(( struct Cyc_Absyn_PointerType_struct*) _temp1893)->f1;
_LL1910: _temp1909=( void*) _temp1899.elt_typ; goto _LL1908; _LL1908: _temp1907=(
void*) _temp1899.rgn_typ; goto _LL1906; _LL1906: _temp1905= _temp1899.nullable;
goto _LL1904; _LL1904: _temp1903= _temp1899.tq; goto _LL1902; _LL1902: _temp1901=
_temp1899.bounds; goto _LL1896;} else{ goto _LL1897;} _LL1897: goto _LL1898;
_LL1896: old_elt_typ= _temp1909; goto _LL1894; _LL1898: old_elt_typ=(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1911=(
unsigned char*)"exp_to_c:new array expression doesn't have ptr type"; struct
_tagged_string _temp1912; _temp1912.curr= _temp1911; _temp1912.base= _temp1911;
_temp1912.last_plus_one= _temp1911 + 52; _temp1912;})); goto _LL1894; _LL1894:;}{
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1913= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); goto _LL1914; _LL1914: { struct Cyc_Absyn_Exp*
_temp1915= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1869), 0), 0); goto
_LL1916; _LL1916: { struct Cyc_Absyn_Exp* e1; if( _temp1467 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1915);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1467); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1915);}{ struct Cyc_Absyn_Exp* _temp1917= Cyc_Absyn_cast_exp( _temp1913, e1,
0); goto _LL1918; _LL1918:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1887, _temp1913,( struct Cyc_Absyn_Exp*) _temp1917,
_temp1891, 0), 0))->r)); goto _LL1858;}}}}}}}} _LL1862: { int is_tagged_ptr= 0;{
void* _temp1919= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1925; struct Cyc_Absyn_Conref* _temp1927; struct Cyc_Absyn_Tqual _temp1929;
struct Cyc_Absyn_Conref* _temp1931; void* _temp1933; void* _temp1935; _LL1921:
if(( unsigned int) _temp1919 > 4u?*(( int*) _temp1919) == Cyc_Absyn_PointerType:
0){ _LL1926: _temp1925=(( struct Cyc_Absyn_PointerType_struct*) _temp1919)->f1;
_LL1936: _temp1935=( void*) _temp1925.elt_typ; goto _LL1934; _LL1934: _temp1933=(
void*) _temp1925.rgn_typ; goto _LL1932; _LL1932: _temp1931= _temp1925.nullable;
goto _LL1930; _LL1930: _temp1929= _temp1925.tq; goto _LL1928; _LL1928: _temp1927=
_temp1925.bounds; goto _LL1922;} else{ goto _LL1923;} _LL1923: goto _LL1924;
_LL1922:{ void* _temp1937=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1927))->v; void* _temp1943; _LL1939: if((
unsigned int) _temp1937 > 1u?*(( int*) _temp1937) == Cyc_Absyn_Eq_constr: 0){
_LL1944: _temp1943=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1937)->f1;
if( _temp1943 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1940;} else{ goto _LL1941;}}
else{ goto _LL1941;} _LL1941: goto _LL1942; _LL1940: is_tagged_ptr= 1; goto
_LL1938; _LL1942: goto _LL1938; _LL1938:;} goto _LL1920; _LL1924:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp1945=(
unsigned char*)"exp_to_c: comprehension not an array type"; struct
_tagged_string _temp1946; _temp1946.curr= _temp1945; _temp1946.base= _temp1945;
_temp1946.last_plus_one= _temp1945 + 42; _temp1946;})); goto _LL1920; _LL1920:;}{
struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1871->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1873);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1875, Cyc_Absyn_var_exp(
max, 0), _temp1871, Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1947=
_new_region(); struct _RegionHandle* r=& _temp1947; _push_region( r);{ struct
Cyc_List_List* decls=({ struct Cyc_List_List* _temp1977=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1977->hd=( void*)({
struct _tuple10* _temp1978=( struct _tuple10*) _region_malloc( r, sizeof( struct
_tuple10)); _temp1978->f1= max; _temp1978->f2= Cyc_Absyn_uint_t; _temp1978->f3=(
struct Cyc_Absyn_Exp*) _temp1873; _temp1978;}); _temp1977->tl= 0; _temp1977;});
struct Cyc_Absyn_Exp* ai; if( _temp1467 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1467); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1948=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1948->hd=( void*)({ struct _tuple10*
_temp1949=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1949->f1= a; _temp1949->f2= ptr_typ; _temp1949->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1949;}); _temp1948->tl= decls; _temp1948;}); if( is_tagged_ptr){
struct _tuple0* _temp1950= Cyc_Toc_temp_var(); goto _LL1951; _LL1951: { void*
_temp1952= Cyc_Toc_typ_to_c( old_typ); goto _LL1953; _LL1953: { struct Cyc_Absyn_Exp*
_temp1963= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(* _temp1954)(
struct _tagged_ptr4)=( struct Cyc_List_List*(*)( struct _tagged_ptr4)) Cyc_List_list;
struct _tuple4* _temp1957=({ struct _tuple4* _temp1962=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1962->f1= 0; _temp1962->f2= Cyc_Absyn_var_exp(
a, 0); _temp1962;}); struct _tuple4* _temp1958=({ struct _tuple4* _temp1961=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1961->f1= 0;
_temp1961->f2= Cyc_Absyn_var_exp( a, 0); _temp1961;}); struct _tuple4* _temp1959=({
struct _tuple4* _temp1960=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1960->f1= 0; _temp1960->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1960;}); struct _tuple4* _temp1955[ 3u]={
_temp1957, _temp1958, _temp1959}; struct _tagged_ptr4 _temp1956={ _temp1955,
_temp1955, _temp1955 + 3u}; _temp1954( _temp1956);}), 0); goto _LL1964; _LL1964:
decls=({ struct Cyc_List_List* _temp1965=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1965->hd=( void*)({ struct _tuple10*
_temp1966=( struct _tuple10*) _region_malloc( r, sizeof( struct _tuple10));
_temp1966->f1= _temp1950; _temp1966->f2= _temp1952; _temp1966->f3=( struct Cyc_Absyn_Exp*)
_temp1963; _temp1966;}); _temp1965->tl= decls; _temp1965;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1950, 0), 0), 0);}}} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
_temp1967= decls; goto _LL1968; _LL1968: for( 0; _temp1967 != 0; _temp1967=((
struct Cyc_List_List*) _check_null( _temp1967))->tl){ struct Cyc_Absyn_Exp*
_temp1971; void* _temp1973; struct _tuple0* _temp1975; struct _tuple10 _temp1969=*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1967))->hd);
_LL1976: _temp1975= _temp1969.f1; goto _LL1974; _LL1974: _temp1973= _temp1969.f2;
goto _LL1972; _LL1972: _temp1971= _temp1969.f3; goto _LL1970; _LL1970: s= Cyc_Absyn_declare_stmt(
_temp1975, _temp1973, _temp1971, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region(& _temp1947);} goto _LL1858;}}} _LL1864:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1465->topt))->v, 1, _temp1467, _temp1879, _temp1883))->r));
goto _LL1858; _LL1866:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv,
1, _temp1467, _temp1885))->r)); goto _LL1858; _LL1868: { void* _temp1979=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1465->topt))->v; goto _LL1980; _LL1980: {
struct _tuple0* _temp1981= Cyc_Toc_temp_var(); goto _LL1982; _LL1982: { struct
Cyc_Absyn_Stmt* _temp1983= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1981, 0),
0); goto _LL1984; _LL1984: { struct Cyc_Absyn_Exp* _temp1985= Cyc_Absyn_signed_int_exp(
0, 0); goto _LL1986; _LL1986: Cyc_Toc_exp_to_c( nv, _temp1465); _temp1983= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1981, 0),
_temp1985, 0), _temp1465, 0), _temp1983, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
_temp1979); void* _temp1987= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto
_LL1988; _LL1988: { struct Cyc_Absyn_Exp* _temp1989= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0); goto _LL1990; _LL1990: { struct Cyc_Absyn_Exp* e1; if( _temp1467 ==
0){ e1= Cyc_Toc_malloc_exp( _temp1979, _temp1989);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp1467); Cyc_Toc_exp_to_c( nv, r); e1=
Cyc_Toc_rmalloc_exp( r, _temp1989);}{ struct Cyc_Absyn_Exp* _temp1991= Cyc_Absyn_cast_exp(
_temp1987, e1, 0); goto _LL1992; _LL1992:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1981, _temp1987,( struct Cyc_Absyn_Exp*) _temp1991,
_temp1983, 0), 0))->r)); goto _LL1858;}}}}}}}} _LL1858:;} goto _LL1314; _LL1352:
Cyc_Toc_exp_to_c( nv, _temp1469); goto _LL1314; _LL1354:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct* _temp1993=( struct Cyc_Absyn_Sizeoftyp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct)); _temp1993[ 0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _temp1994; _temp1994.tag= Cyc_Absyn_Sizeoftyp_e;
_temp1994.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1471); _temp1994;});
_temp1993;}))); goto _LL1314; _LL1356:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Offsetof_e_struct* _temp1995=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1995[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1996; _temp1996.tag= Cyc_Absyn_Offsetof_e;
_temp1996.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1475); _temp1996.f2=
_temp1473; _temp1996;}); _temp1995;}))); goto _LL1314; _LL1358: { void* e1_typ=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1477->topt))->v);
Cyc_Toc_exp_to_c( nv, _temp1477);{ void* _temp1997= e1_typ; struct Cyc_Absyn_PtrInfo
_temp2003; struct Cyc_Absyn_Conref* _temp2005; struct Cyc_Absyn_Tqual _temp2007;
struct Cyc_Absyn_Conref* _temp2009; void* _temp2011; void* _temp2013; _LL1999:
if(( unsigned int) _temp1997 > 4u?*(( int*) _temp1997) == Cyc_Absyn_PointerType:
0){ _LL2004: _temp2003=(( struct Cyc_Absyn_PointerType_struct*) _temp1997)->f1;
_LL2014: _temp2013=( void*) _temp2003.elt_typ; goto _LL2012; _LL2012: _temp2011=(
void*) _temp2003.rgn_typ; goto _LL2010; _LL2010: _temp2009= _temp2003.nullable;
goto _LL2008; _LL2008: _temp2007= _temp2003.tq; goto _LL2006; _LL2006: _temp2005=
_temp2003.bounds; goto _LL2000;} else{ goto _LL2001;} _LL2001: goto _LL2002;
_LL2000:{ void* _temp2015=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp2005); struct Cyc_Absyn_Exp* _temp2021; _LL2017: if( _temp2015 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL2018;} else{ goto _LL2019;} _LL2019: if((
unsigned int) _temp2015 > 1u?*(( int*) _temp2015) == Cyc_Absyn_Upper_b: 0){
_LL2022: _temp2021=(( struct Cyc_Absyn_Upper_b_struct*) _temp2015)->f1; goto
_LL2020;} else{ goto _LL2016;} _LL2018: { struct _tuple0* x= Cyc_Toc_temp_var();
struct _tuple0* a= Cyc_Toc_temp_var(); void* _temp2023= Cyc_Toc_typ_to_c_array(
_temp2013); goto _LL2024; _LL2024: { void* tx= Cyc_Toc_add_tagged_ptr_type(
_temp2023, _temp2007); void* ta2= Cyc_Absyn_cstar_typ( _temp2023, _temp2007);
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
struct Cyc_Absyn_Exp*) _temp1477, s1, 0), 0))->r)); goto _LL2016;}} _LL2020: if(
Cyc_Toc_is_nullable( e1_typ)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1477->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp2025=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2025->hd=(
void*) _temp1477; _temp2025->tl= 0; _temp2025;}), 0), 0), 0))->r));} else{
struct _tuple0* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
x, 0), 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, e1_typ,(
struct Cyc_Absyn_Exp*) _temp1477, s3, 0), 0))->r));}} goto _LL2016; _LL2016:;}
goto _LL1998; _LL2002:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2026=( unsigned char*)"exp_to_c: Deref: non-pointer"; struct
_tagged_string _temp2027; _temp2027.curr= _temp2026; _temp2027.base= _temp2026;
_temp2027.last_plus_one= _temp2026 + 29; _temp2027;})); goto _LL1998; _LL1998:;}
goto _LL1314;} _LL1360: Cyc_Toc_exp_to_c( nv, _temp1481); if( Cyc_Toc_is_poly_project(
e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1314; _LL1362: { void* e1typ= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1485->topt))->v); Cyc_Toc_exp_to_c(
nv, _temp1485);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp2030; struct Cyc_Absyn_Tqual _temp2032; struct Cyc_Absyn_Conref* _temp2034;
void* _temp2036; void* _temp2038; struct Cyc_Absyn_PtrInfo _temp2028= Cyc_Toc_get_ptr_type(
e1typ); _LL2039: _temp2038=( void*) _temp2028.elt_typ; goto _LL2037; _LL2037:
_temp2036=( void*) _temp2028.rgn_typ; goto _LL2035; _LL2035: _temp2034=
_temp2028.nullable; goto _LL2033; _LL2033: _temp2032= _temp2028.tq; goto _LL2031;
_LL2031: _temp2030= _temp2028.bounds; goto _LL2029; _LL2029:{ void* _temp2040=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp2030); struct
Cyc_Absyn_Exp* _temp2046; _LL2042: if(( unsigned int) _temp2040 > 1u?*(( int*)
_temp2040) == Cyc_Absyn_Upper_b: 0){ _LL2047: _temp2046=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2040)->f1; goto _LL2043;} else{ goto _LL2044;} _LL2044: if( _temp2040 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL2045;} else{ goto _LL2041;} _LL2043: if(
Cyc_Evexp_eval_const_uint_exp( _temp2046) < 1){(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp2048=( unsigned char*)"exp_to_c:  StructArrow_e on pointer of size 0";
struct _tagged_string _temp2049; _temp2049.curr= _temp2048; _temp2049.base=
_temp2048; _temp2049.last_plus_one= _temp2048 + 46; _temp2049;}));}{ int
possibly_null;{ void* _temp2050=( void*)((( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2034))->v; int _temp2056;
_LL2052: if(( unsigned int) _temp2050 > 1u?*(( int*) _temp2050) == Cyc_Absyn_Eq_constr:
0){ _LL2057: _temp2056=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2050)->f1;
goto _LL2053;} else{ goto _LL2054;} _LL2054: goto _LL2055; _LL2053:
possibly_null= _temp2056; goto _LL2051; _LL2055: possibly_null= 0; goto _LL2051;
_LL2051:;} if( Cyc_Toc_is_nullable(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1485->topt))->v)){ if( Cyc_Toc_use_runtimecheck_funs){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1485->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp2058=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2058->hd=(
void*) _temp1485; _temp2058->tl= 0; _temp2058;}), 0), 0), _temp1483, 0))->r));}
else{ struct _tuple0* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1485->topt))->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1483, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1485, s3, 0), 0))->r));}} goto _LL2041;} _LL2045: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2038); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2032); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2068=({ struct Cyc_List_List*(* _temp2059)( struct
_tagged_ptr7)=( struct Cyc_List_List*(*)( struct _tagged_ptr7)) Cyc_List_list;
struct _tuple1* _temp2062=({ struct _tuple1* _temp2067=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2067->f1= 0; _temp2067->f2= Cyc_Toc_mt_tq;
_temp2067->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1485->topt))->v); _temp2067;}); struct _tuple1* _temp2063=({ struct _tuple1*
_temp2066=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2066->f1=
0; _temp2066->f2= Cyc_Toc_mt_tq; _temp2066->f3= Cyc_Absyn_uint_t; _temp2066;});
struct _tuple1* _temp2064=({ struct _tuple1* _temp2065=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2065->f1= 0; _temp2065->f2= Cyc_Toc_mt_tq;
_temp2065->f3= Cyc_Absyn_uint_t; _temp2065;}); struct _tuple1* _temp2060[ 3u]={
_temp2062, _temp2063, _temp2064}; struct _tagged_ptr7 _temp2061={ _temp2060,
_temp2060, _temp2060 + 3u}; _temp2059( _temp2061);}); goto _LL2069; _LL2069: {
void* _temp2073= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2070=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2070[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2071; _temp2071.tag= Cyc_Absyn_FnType;
_temp2071.f1=({ struct Cyc_Absyn_FnInfo _temp2072; _temp2072.tvars= 0; _temp2072.effect=
0; _temp2072.ret_typ=( void*) ta2; _temp2072.args= _temp2068; _temp2072.c_varargs=
0; _temp2072.cyc_varargs= 0; _temp2072.rgn_po= 0; _temp2072.attributes= 0;
_temp2072;}); _temp2071;}); _temp2070;}), Cyc_Toc_mt_tq); goto _LL2074; _LL2074: {
struct Cyc_Absyn_Exp* fnexp= Cyc_Absyn_cast_exp( _temp2073, Cyc_Absyn_var_exp(
Cyc_Toc__check_unknown_subscript_qv, 0), 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_uint_exp(
0, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_fncall_exp(
fnexp,({ struct Cyc_List_List*(* _temp2075)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)(
struct _tagged_ptr8)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2078= _temp1485;
struct Cyc_Absyn_Exp* _temp2079= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2080= e2; struct Cyc_Absyn_Exp* _temp2076[ 3u]={ _temp2078, _temp2079,
_temp2080}; struct _tagged_ptr8 _temp2077={ _temp2076, _temp2076, _temp2076 + 3u};
_temp2075( _temp2077);}), 0), _temp1483, 0))->r));}}} else{ struct _tuple0* x=
Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2032);
struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0), Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp(
a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0), 0), 0), 0); struct Cyc_Absyn_Exp* a_init= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( a, 0), _temp1483, 0), 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt( 0), 0); struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt* s1=
Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,(
struct Cyc_Absyn_Exp*) _temp1485, s1, 0), 0))->r));} goto _LL2041;} _LL2041:;}
if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL1314;}} _LL1364: { void* _temp2081= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1489->topt))->v); goto _LL2082;
_LL2082: Cyc_Toc_exp_to_c( nv, _temp1489); Cyc_Toc_exp_to_c( nv, _temp1487);{
void* _temp2083= _temp2081; struct Cyc_List_List* _temp2091; struct Cyc_Absyn_PtrInfo
_temp2093; struct Cyc_Absyn_Conref* _temp2095; struct Cyc_Absyn_Tqual _temp2097;
struct Cyc_Absyn_Conref* _temp2099; void* _temp2101; void* _temp2103; _LL2085:
if(( unsigned int) _temp2083 > 4u?*(( int*) _temp2083) == Cyc_Absyn_TupleType: 0){
_LL2092: _temp2091=(( struct Cyc_Absyn_TupleType_struct*) _temp2083)->f1; goto
_LL2086;} else{ goto _LL2087;} _LL2087: if(( unsigned int) _temp2083 > 4u?*((
int*) _temp2083) == Cyc_Absyn_PointerType: 0){ _LL2094: _temp2093=(( struct Cyc_Absyn_PointerType_struct*)
_temp2083)->f1; _LL2104: _temp2103=( void*) _temp2093.elt_typ; goto _LL2102;
_LL2102: _temp2101=( void*) _temp2093.rgn_typ; goto _LL2100; _LL2100: _temp2099=
_temp2093.nullable; goto _LL2098; _LL2098: _temp2097= _temp2093.tq; goto _LL2096;
_LL2096: _temp2095= _temp2093.bounds; goto _LL2088;} else{ goto _LL2089;}
_LL2089: goto _LL2090; _LL2086: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1487) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1489, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL2084;} _LL2088:{ void*
_temp2105=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp2095); struct Cyc_Absyn_Exp* _temp2111; _LL2107: if(( unsigned int)
_temp2105 > 1u?*(( int*) _temp2105) == Cyc_Absyn_Upper_b: 0){ _LL2112: _temp2111=((
struct Cyc_Absyn_Upper_b_struct*) _temp2105)->f1; goto _LL2108;} else{ goto
_LL2109;} _LL2109: if( _temp2105 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2110;}
else{ goto _LL2106;} _LL2108: _temp2111= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp2111), 0);{ int possibly_null;{ void* _temp2113=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2099))->v; int
_temp2119; _LL2115: if(( unsigned int) _temp2113 > 1u?*(( int*) _temp2113) ==
Cyc_Absyn_Eq_constr: 0){ _LL2120: _temp2119=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2113)->f1; goto _LL2116;} else{ goto _LL2117;} _LL2117: goto _LL2118;
_LL2116: possibly_null= _temp2119; goto _LL2114; _LL2118: possibly_null= 0; goto
_LL2114; _LL2114:;}{ void* ta1= Cyc_Toc_typ_to_c( _temp2103); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp2097); if( Cyc_Toc_use_runtimecheck_funs){ if( possibly_null){( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv, 0),({ struct Cyc_List_List*(*
_temp2121)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)( struct _tagged_ptr8))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp2124= _temp1489; struct Cyc_Absyn_Exp*
_temp2125= _temp2111; struct Cyc_Absyn_Exp* _temp2126= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp2127= _temp1487; struct Cyc_Absyn_Exp*
_temp2122[ 4u]={ _temp2124, _temp2125, _temp2126, _temp2127}; struct
_tagged_ptr8 _temp2123={ _temp2122, _temp2122, _temp2122 + 4u}; _temp2121(
_temp2123);}), 0), 0), 0))->r));} else{( void*)( _temp1487->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List*(* _temp2128)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)(
struct _tagged_ptr8)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2131= _temp2111;
struct Cyc_Absyn_Exp* _temp2132= Cyc_Absyn_copy_exp( _temp1487); struct Cyc_Absyn_Exp*
_temp2129[ 2u]={ _temp2131, _temp2132}; struct _tagged_ptr8 _temp2130={
_temp2129, _temp2129, _temp2129 + 2u}; _temp2128( _temp2130);}), 0))->r));}}
else{ struct _tuple0* a= Cyc_Toc_temp_var(); struct _tuple0* i= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte, Cyc_Absyn_var_exp(
i, 0), _temp2111, 0); if( possibly_null){ e3= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_new_exp((
void*) e3->r, 0), 0);}{ struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt*
s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt(
i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) _temp1487, s3, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*)
_temp1489, s1, 0), 0))->r));}} goto _LL2106;}} _LL2110: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2103); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2097); if( Cyc_Toc_use_runtimecheck_funs){
struct Cyc_List_List* _temp2142=({ struct Cyc_List_List*(* _temp2133)( struct
_tagged_ptr7)=( struct Cyc_List_List*(*)( struct _tagged_ptr7)) Cyc_List_list;
struct _tuple1* _temp2136=({ struct _tuple1* _temp2141=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2141->f1= 0; _temp2141->f2= Cyc_Toc_mt_tq;
_temp2141->f3= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1489->topt))->v); _temp2141;}); struct _tuple1* _temp2137=({ struct _tuple1*
_temp2140=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2140->f1=
0; _temp2140->f2= Cyc_Toc_mt_tq; _temp2140->f3= Cyc_Absyn_uint_t; _temp2140;});
struct _tuple1* _temp2138=({ struct _tuple1* _temp2139=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2139->f1= 0; _temp2139->f2= Cyc_Toc_mt_tq;
_temp2139->f3= Cyc_Absyn_uint_t; _temp2139;}); struct _tuple1* _temp2134[ 3u]={
_temp2136, _temp2137, _temp2138}; struct _tagged_ptr7 _temp2135={ _temp2134,
_temp2134, _temp2134 + 3u}; _temp2133( _temp2135);}); goto _LL2143; _LL2143: {
void* _temp2147= Cyc_Absyn_cstar_typ(( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2144=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2144[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2145; _temp2145.tag= Cyc_Absyn_FnType;
_temp2145.f1=({ struct Cyc_Absyn_FnInfo _temp2146; _temp2146.tvars= 0; _temp2146.effect=
0; _temp2146.ret_typ=( void*) ta2; _temp2146.args= _temp2142; _temp2146.c_varargs=
0; _temp2146.cyc_varargs= 0; _temp2146.rgn_po= 0; _temp2146.attributes= 0;
_temp2146;}); _temp2145;}); _temp2144;}), Cyc_Toc_mt_tq); goto _LL2148; _LL2148: {
struct Cyc_Absyn_Exp* fnexp= Cyc_Absyn_cast_exp( _temp2147, Cyc_Absyn_var_exp(
Cyc_Toc__check_unknown_subscript_qv, 0), 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_deref_exp( Cyc_Absyn_fncall_exp( fnexp,({ struct Cyc_List_List*(*
_temp2149)( struct _tagged_ptr8)=( struct Cyc_List_List*(*)( struct _tagged_ptr8))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp2152= _temp1489; struct Cyc_Absyn_Exp*
_temp2153= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp* _temp2154=
_temp1487; struct Cyc_Absyn_Exp* _temp2150[ 3u]={ _temp2152, _temp2153,
_temp2154}; struct _tagged_ptr8 _temp2151={ _temp2150, _temp2150, _temp2150 + 3u};
_temp2149( _temp2151);}), 0), 0))->r));}}} else{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tuple0* i= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var();
void* tx= Cyc_Toc_add_tagged_ptr_type( ta1, _temp2097); struct Cyc_Absyn_Exp*
test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_or_exp(
Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp1487, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1489, s1, 0), 0))->r));} goto _LL2106;} _LL2106:;}
goto _LL2084; _LL2090:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2155=( unsigned char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp2156; _temp2156.curr= _temp2155; _temp2156.base=
_temp2155; _temp2156.last_plus_one= _temp2155 + 49; _temp2156;})); goto _LL2084;
_LL2084:;} goto _LL1314;} _LL1366: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1491))->r));} else{ struct Cyc_List_List*
_temp2157=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1491); goto
_LL2158; _LL2158: { struct _tagged_string* _temp2159= Cyc_Toc_add_tuple_type(
_temp2157); goto _LL2160; _LL2160: { struct Cyc_List_List* dles= 0;{ int i= 1;
for( 0; _temp1491 != 0;( _temp1491=(( struct Cyc_List_List*) _check_null(
_temp1491))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1491))->hd); dles=({ struct Cyc_List_List*
_temp2161=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2161->hd=( void*)({ struct _tuple4* _temp2162=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2162->f1= 0; _temp2162->f2=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1491))->hd; _temp2162;}); _temp2161->tl=
dles; _temp2161;});}} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, dles, 0))->r));}}} goto _LL1314; _LL1368:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ unsigned char* _temp2163=( unsigned char*)"compoundlit";
struct _tagged_string _temp2164; _temp2164.curr= _temp2163; _temp2164.base=
_temp2163; _temp2164.last_plus_one= _temp2163 + 12; _temp2164;})); goto _LL1314;
_LL1370:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1497, 0))->r));{ struct Cyc_List_List* _temp2165= _temp1497; goto _LL2166;
_LL2166: for( 0; _temp2165 != 0; _temp2165=(( struct Cyc_List_List*) _check_null(
_temp2165))->tl){ struct _tuple4 _temp2169; struct Cyc_Absyn_Exp* _temp2170;
struct Cyc_List_List* _temp2172; struct _tuple4* _temp2167=( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp2165))->hd; _temp2169=* _temp2167;
_LL2173: _temp2172= _temp2169.f1; goto _LL2171; _LL2171: _temp2170= _temp2169.f2;
goto _LL2168; _LL2168: Cyc_Toc_exp_to_c( nv, _temp2170);}} goto _LL1314; _LL1372: {
unsigned int _temp2174= Cyc_Evexp_eval_const_uint_exp( _temp1501); goto _LL2175;
_LL2175: Cyc_Toc_exp_to_c( nv, _temp1499);{ struct Cyc_List_List* es= 0; if( !
Cyc_Toc_is_zero( _temp1499)){ unsigned int i= 0; for( 0; i < _temp2174; i ++){
es=({ struct Cyc_List_List* _temp2176=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2176->hd=( void*)({ struct _tuple4*
_temp2177=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp2177->f1=
0; _temp2177->f2= _temp1499; _temp2177;}); _temp2176->tl= es; _temp2176;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL1314;}} _LL1374: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1507, _temp1511))->r));} else{ if(
_temp1505 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2178=( unsigned char*)"Struct_e: missing structdecl pointer";
struct _tagged_string _temp2179; _temp2179.curr= _temp2178; _temp2179.base=
_temp2178; _temp2179.last_plus_one= _temp2178 + 37; _temp2179;}));}{ struct Cyc_Absyn_Structdecl*
sd2=( struct Cyc_Absyn_Structdecl*) _check_null( _temp1505); struct
_RegionHandle _temp2180= _new_region(); struct _RegionHandle* rgn=& _temp2180;
_push_region( rgn);{ struct Cyc_List_List* _temp2181=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1507,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd2->fields))->v); goto _LL2182; _LL2182: { struct Cyc_List_List* _temp2183= 0;
goto _LL2184; _LL2184:{ struct Cyc_List_List* _temp2185=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto _LL2186; _LL2186: for(
0; _temp2185 != 0; _temp2185=(( struct Cyc_List_List*) _check_null( _temp2185))->tl){
struct Cyc_List_List* _temp2187= _temp2181; goto _LL2188; _LL2188: for( 0;
_temp2187 != 0; _temp2187=(( struct Cyc_List_List*) _check_null( _temp2187))->tl){
if((*(( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2187))->hd)).f1
==( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2185))->hd){ struct _tuple12 _temp2191; struct Cyc_Absyn_Exp* _temp2192;
struct Cyc_Absyn_Structfield* _temp2194; struct _tuple12* _temp2189=( struct
_tuple12*)(( struct Cyc_List_List*) _check_null( _temp2187))->hd; _temp2191=*
_temp2189; _LL2195: _temp2194= _temp2191.f1; goto _LL2193; _LL2193: _temp2192=
_temp2191.f2; goto _LL2190; _LL2190: { void* _temp2196=( void*) _temp2194->type;
goto _LL2197; _LL2197: Cyc_Toc_exp_to_c( nv, _temp2192); if( Cyc_Toc_is_void_star(
_temp2196)){( void*)( _temp2192->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp2192->r, 0), 0))->r));} _temp2183=({ struct Cyc_List_List*
_temp2198=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2198->hd=( void*)({ struct _tuple4* _temp2199=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2199->f1= 0; _temp2199->f2= _temp2192; _temp2199;});
_temp2198->tl= _temp2183; _temp2198;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp2183), 0))->r));}}; _pop_region(& _temp2180);}} goto
_LL1314; _LL1376: { struct Cyc_List_List* fs;{ void* _temp2200= Cyc_Tcutil_compress(
_temp1515); struct Cyc_List_List* _temp2206; _LL2202: if(( unsigned int)
_temp2200 > 4u?*(( int*) _temp2200) == Cyc_Absyn_AnonStructType: 0){ _LL2207:
_temp2206=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2200)->f1; goto
_LL2203;} else{ goto _LL2204;} _LL2204: goto _LL2205; _LL2203: fs= _temp2206;
goto _LL2201; _LL2205: fs=(( struct Cyc_List_List*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ struct _tagged_string _temp2208=
Cyc_Absynpp_typ2string( _temp1515); xprintf("anon struct has type %.*s",
_temp2208.last_plus_one - _temp2208.curr, _temp2208.curr);})); goto _LL2201;
_LL2201:;}{ struct _RegionHandle _temp2209= _new_region(); struct _RegionHandle*
rgn=& _temp2209; _push_region( rgn);{ struct Cyc_List_List* _temp2210=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1513, fs); goto _LL2211; _LL2211: for( 0; _temp2210 != 0;
_temp2210=(( struct Cyc_List_List*) _check_null( _temp2210))->tl){ struct
_tuple12 _temp2214; struct Cyc_Absyn_Exp* _temp2215; struct Cyc_Absyn_Structfield*
_temp2217; struct _tuple12* _temp2212=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( _temp2210))->hd; _temp2214=* _temp2212; _LL2218: _temp2217=
_temp2214.f1; goto _LL2216; _LL2216: _temp2215= _temp2214.f2; goto _LL2213;
_LL2213: { void* _temp2219=( void*) _temp2217->type; goto _LL2220; _LL2220: Cyc_Toc_exp_to_c(
nv, _temp2215); if( Cyc_Toc_is_void_star( _temp2219)){( void*)( _temp2215->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp2215->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1513, 0))->r));}; _pop_region(& _temp2209);} goto _LL1314;} _LL1378: {
struct _tuple0* qv= _temp1517->name; if( _temp1519->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2221= _temp1519->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1519->fields))->v;
goto _LL2222; _LL2222: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2221))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2221))->hd)->typs
== 0){ tag_count ++;} _temp2221=(( struct Cyc_List_List*) _check_null( _temp2221))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1314;} _LL1380: { struct _tuple0* _temp2223= _temp1527->name; goto
_LL2224; _LL2224: { struct Cyc_List_List* _temp2225= _temp1527->typs; goto
_LL2226; _LL2226: { struct _tuple0* _temp2227= Cyc_Toc_temp_var(); goto _LL2228;
_LL2228: { struct Cyc_Absyn_Exp* _temp2229= Cyc_Absyn_var_exp( _temp2227, 0);
goto _LL2230; _LL2230: { void* _temp2233= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2223,({ unsigned char* _temp2231=( unsigned char*)"_struct"; struct
_tagged_string _temp2232; _temp2232.curr= _temp2231; _temp2232.base= _temp2231;
_temp2232.last_plus_one= _temp2231 + 8; _temp2232;}))); goto _LL2234; _LL2234:
if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp; if( _temp1529->is_xtunion){
tag_exp= Cyc_Absyn_var_exp( _temp2223, 0);} else{ int tag_count= 0; struct Cyc_List_List*
_temp2235= _temp1529->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1529->fields))->v; goto _LL2236; _LL2236: while( Cyc_Absyn_qvar_cmp(
_temp2223,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2235))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2235))->hd)->typs != 0){ tag_count ++;}
_temp2235=(( struct Cyc_List_List*) _check_null( _temp2235))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2237= 0; goto _LL2238;
_LL2238: for( 0; _temp1531 != 0;( _temp1531=(( struct Cyc_List_List*)
_check_null( _temp1531))->tl, _temp2225=(( struct Cyc_List_List*) _check_null(
_temp2225))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1531))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2225))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2237=({ struct Cyc_List_List* _temp2239=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2239[ 0]=({ struct Cyc_List_List
_temp2240; _temp2240.hd=( void*)({ struct _tuple4* _temp2241=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2241[ 0]=({ struct _tuple4
_temp2242; _temp2242.f1= 0; _temp2242.f2= cur_e; _temp2242;}); _temp2241;});
_temp2240.tl= _temp2237; _temp2240;}); _temp2239;});} _temp2237=({ struct Cyc_List_List*
_temp2243=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2243[ 0]=({ struct Cyc_List_List _temp2244; _temp2244.hd=( void*)({ struct
_tuple4* _temp2245=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2245[ 0]=({ struct _tuple4 _temp2246; _temp2246.f1= 0; _temp2246.f2=
tag_exp; _temp2246;}); _temp2245;}); _temp2244.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2237); _temp2244;}); _temp2243;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2247=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2247->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2223,({ unsigned char*
_temp2248=( unsigned char*)"_struct"; struct _tagged_string _temp2249; _temp2249.curr=
_temp2248; _temp2249.base= _temp2248; _temp2249.last_plus_one= _temp2248 + 8;
_temp2249;})); _temp2247;}), _temp2237, 0))->r));}} else{ struct Cyc_List_List*
_temp2251=({ struct Cyc_List_List* _temp2250=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2250->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2229, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp2223, 0), 0); _temp2250->tl= 0; _temp2250;}); goto _LL2252; _LL2252:{ int i=
1; for( 0; _temp1531 != 0;((( _temp1531=(( struct Cyc_List_List*) _check_null(
_temp1531))->tl, i ++)), _temp2225=(( struct Cyc_List_List*) _check_null(
_temp2225))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1531))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2225))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp2253= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2229, Cyc_Toc_fieldname( i), 0), cur_e, 0);
goto _LL2254; _LL2254: _temp2251=({ struct Cyc_List_List* _temp2255=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2255->hd=( void*) _temp2253;
_temp2255->tl= _temp2251; _temp2255;});}}}{ struct Cyc_Absyn_Stmt* _temp2256=
Cyc_Absyn_exp_stmt( _temp2229, 0); goto _LL2257; _LL2257: { struct Cyc_Absyn_Stmt*
_temp2259= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2258=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2258->hd=( void*) _temp2256;
_temp2258->tl= _temp2251; _temp2258;})), 0); goto _LL2260; _LL2260:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp2227,
_temp2233, 0, _temp2259, 0), 0))->r));}}} goto _LL1314;}}}}} _LL1382: goto
_LL1314; _LL1384: { void* t_c= Cyc_Toc_typ_to_c( _temp1543); if( _temp1545 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1545); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1543, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL1314;} _LL1386: Cyc_Toc_stmt_to_c(
nv, _temp1547); goto _LL1314; _LL1388:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2261=( unsigned char*)"UnresolvedMem"; struct _tagged_string
_temp2262; _temp2262.curr= _temp2261; _temp2262.base= _temp2261; _temp2262.last_plus_one=
_temp2261 + 14; _temp2262;})); goto _LL1314; _LL1390:(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(({ unsigned char* _temp2263=( unsigned char*)"codegen";
struct _tagged_string _temp2264; _temp2264.curr= _temp2263; _temp2264.base=
_temp2263; _temp2264.last_plus_one= _temp2263 + 8; _temp2264;})); goto _LL1314;
_LL1392:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ unsigned char*
_temp2265=( unsigned char*)"fill"; struct _tagged_string _temp2266; _temp2266.curr=
_temp2265; _temp2266.base= _temp2265; _temp2266.last_plus_one= _temp2265 + 5;
_temp2266;})); goto _LL1314; _LL1314:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
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
_temp2267=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp2303; struct Cyc_Absyn_Vardecl
_temp2305; struct Cyc_List_List* _temp2306; struct Cyc_Core_Opt* _temp2308;
struct Cyc_Absyn_Exp* _temp2310; void* _temp2312; struct Cyc_Absyn_Tqual
_temp2314; struct _tuple0* _temp2316; void* _temp2318; struct Cyc_Absyn_Vardecl*
_temp2320; int _temp2322; void* _temp2324; unsigned char _temp2326; struct
_tagged_string _temp2328; struct Cyc_Absyn_Enumfield* _temp2330; struct Cyc_Absyn_Enumdecl*
_temp2332; struct Cyc_List_List* _temp2334; struct Cyc_List_List* _temp2336;
struct Cyc_Absyn_Tunionfield* _temp2338; struct Cyc_Absyn_Tuniondecl* _temp2340;
struct Cyc_Absyn_Pat* _temp2343; struct Cyc_Absyn_Pat _temp2345; struct Cyc_Position_Segment*
_temp2346; struct Cyc_Core_Opt* _temp2348; void* _temp2350; struct Cyc_List_List*
_temp2352; struct Cyc_List_List* _temp2354; struct Cyc_Absyn_Tunionfield*
_temp2356; struct Cyc_Absyn_Tuniondecl* _temp2358; struct Cyc_List_List*
_temp2360; struct Cyc_List_List* _temp2362; struct Cyc_Absyn_Tunionfield*
_temp2364; struct Cyc_Absyn_Tuniondecl* _temp2366; struct Cyc_List_List*
_temp2368; struct Cyc_List_List* _temp2370; struct Cyc_List_List* _temp2372;
struct Cyc_Core_Opt* _temp2374; struct Cyc_Absyn_Structdecl* _temp2376; struct
Cyc_Absyn_Pat* _temp2378; struct _tuple0* _temp2380; struct Cyc_List_List*
_temp2382; struct Cyc_List_List* _temp2384; struct _tuple0* _temp2386; struct
Cyc_List_List* _temp2388; struct Cyc_List_List* _temp2390; struct _tuple0*
_temp2392; _LL2269: if(( unsigned int) _temp2267 > 2u?*(( int*) _temp2267) ==
Cyc_Absyn_Var_p: 0){ _LL2304: _temp2303=(( struct Cyc_Absyn_Var_p_struct*)
_temp2267)->f1; _temp2305=* _temp2303; _LL2319: _temp2318=( void*) _temp2305.sc;
goto _LL2317; _LL2317: _temp2316= _temp2305.name; goto _LL2315; _LL2315:
_temp2314= _temp2305.tq; goto _LL2313; _LL2313: _temp2312=( void*) _temp2305.type;
goto _LL2311; _LL2311: _temp2310= _temp2305.initializer; goto _LL2309; _LL2309:
_temp2308= _temp2305.rgn; goto _LL2307; _LL2307: _temp2306= _temp2305.attributes;
goto _LL2270;} else{ goto _LL2271;} _LL2271: if( _temp2267 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2272;} else{ goto _LL2273;} _LL2273: if(( unsigned int) _temp2267 > 2u?*((
int*) _temp2267) == Cyc_Absyn_Reference_p: 0){ _LL2321: _temp2320=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2267)->f1; goto _LL2274;} else{ goto _LL2275;} _LL2275: if( _temp2267 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2276;} else{ goto _LL2277;} _LL2277: if((
unsigned int) _temp2267 > 2u?*(( int*) _temp2267) == Cyc_Absyn_Int_p: 0){
_LL2325: _temp2324=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2267)->f1;
goto _LL2323; _LL2323: _temp2322=(( struct Cyc_Absyn_Int_p_struct*) _temp2267)->f2;
goto _LL2278;} else{ goto _LL2279;} _LL2279: if(( unsigned int) _temp2267 > 2u?*((
int*) _temp2267) == Cyc_Absyn_Char_p: 0){ _LL2327: _temp2326=(( struct Cyc_Absyn_Char_p_struct*)
_temp2267)->f1; goto _LL2280;} else{ goto _LL2281;} _LL2281: if(( unsigned int)
_temp2267 > 2u?*(( int*) _temp2267) == Cyc_Absyn_Float_p: 0){ _LL2329: _temp2328=((
struct Cyc_Absyn_Float_p_struct*) _temp2267)->f1; goto _LL2282;} else{ goto
_LL2283;} _LL2283: if(( unsigned int) _temp2267 > 2u?*(( int*) _temp2267) == Cyc_Absyn_Enum_p:
0){ _LL2333: _temp2332=(( struct Cyc_Absyn_Enum_p_struct*) _temp2267)->f1; goto
_LL2331; _LL2331: _temp2330=(( struct Cyc_Absyn_Enum_p_struct*) _temp2267)->f2;
goto _LL2284;} else{ goto _LL2285;} _LL2285: if(( unsigned int) _temp2267 > 2u?*((
int*) _temp2267) == Cyc_Absyn_Tunion_p: 0){ _LL2341: _temp2340=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2267)->f1; goto _LL2339; _LL2339: _temp2338=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2267)->f2; goto _LL2337; _LL2337: _temp2336=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2267)->f3; goto _LL2335; _LL2335: _temp2334=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2267)->f4; if( _temp2334 == 0){ goto _LL2286;} else{ goto _LL2287;}} else{
goto _LL2287;} _LL2287: if(( unsigned int) _temp2267 > 2u?*(( int*) _temp2267)
== Cyc_Absyn_Pointer_p: 0){ _LL2344: _temp2343=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2267)->f1; _temp2345=* _temp2343; _LL2351: _temp2350=( void*) _temp2345.r;
if(( unsigned int) _temp2350 > 2u?*(( int*) _temp2350) == Cyc_Absyn_Tunion_p: 0){
_LL2359: _temp2358=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2350)->f1; goto
_LL2357; _LL2357: _temp2356=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2350)->f2;
goto _LL2355; _LL2355: _temp2354=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2350)->f3;
goto _LL2353; _LL2353: _temp2352=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2350)->f4;
goto _LL2349;} else{ goto _LL2289;} _LL2349: _temp2348= _temp2345.topt; goto
_LL2347; _LL2347: _temp2346= _temp2345.loc; goto _LL2342;} else{ goto _LL2289;}
_LL2342: if( _temp2352 != 0){ goto _LL2288;} else{ goto _LL2289;} _LL2289: if((
unsigned int) _temp2267 > 2u?*(( int*) _temp2267) == Cyc_Absyn_Tunion_p: 0){
_LL2367: _temp2366=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2267)->f1; goto
_LL2365; _LL2365: _temp2364=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2267)->f2;
goto _LL2363; _LL2363: _temp2362=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2267)->f3;
goto _LL2361; _LL2361: _temp2360=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2267)->f4;
goto _LL2290;} else{ goto _LL2291;} _LL2291: if(( unsigned int) _temp2267 > 2u?*((
int*) _temp2267) == Cyc_Absyn_Tuple_p: 0){ _LL2369: _temp2368=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2267)->f1; goto _LL2292;} else{ goto _LL2293;} _LL2293: if(( unsigned int)
_temp2267 > 2u?*(( int*) _temp2267) == Cyc_Absyn_Struct_p: 0){ _LL2377:
_temp2376=(( struct Cyc_Absyn_Struct_p_struct*) _temp2267)->f1; goto _LL2375;
_LL2375: _temp2374=(( struct Cyc_Absyn_Struct_p_struct*) _temp2267)->f2; goto
_LL2373; _LL2373: _temp2372=(( struct Cyc_Absyn_Struct_p_struct*) _temp2267)->f3;
goto _LL2371; _LL2371: _temp2370=(( struct Cyc_Absyn_Struct_p_struct*) _temp2267)->f4;
goto _LL2294;} else{ goto _LL2295;} _LL2295: if(( unsigned int) _temp2267 > 2u?*((
int*) _temp2267) == Cyc_Absyn_Pointer_p: 0){ _LL2379: _temp2378=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2267)->f1; goto _LL2296;} else{ goto _LL2297;} _LL2297: if(( unsigned int)
_temp2267 > 2u?*(( int*) _temp2267) == Cyc_Absyn_UnknownId_p: 0){ _LL2381:
_temp2380=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2267)->f1; goto _LL2298;}
else{ goto _LL2299;} _LL2299: if(( unsigned int) _temp2267 > 2u?*(( int*)
_temp2267) == Cyc_Absyn_UnknownCall_p: 0){ _LL2387: _temp2386=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2267)->f1; goto _LL2385; _LL2385: _temp2384=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2267)->f2; goto _LL2383; _LL2383: _temp2382=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2267)->f3; goto _LL2300;} else{ goto _LL2301;} _LL2301: if(( unsigned int)
_temp2267 > 2u?*(( int*) _temp2267) == Cyc_Absyn_UnknownFields_p: 0){ _LL2393:
_temp2392=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2267)->f1; goto
_LL2391; _LL2391: _temp2390=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2267)->f2; goto _LL2389; _LL2389: _temp2388=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2267)->f3; goto _LL2302;} else{ goto _LL2268;} _LL2270: nv= Cyc_Toc_add_varmap(
nv, _temp2316, r); goto _LL2272; _LL2272: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2268; _LL2274: { struct _tuple0* _temp2394= Cyc_Toc_temp_var(); goto
_LL2395; _LL2395: decls=({ struct Cyc_List_List* _temp2396=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2396->hd=( void*)({
struct _tuple14* _temp2397=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2397->f1= _temp2394; _temp2397->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2397;}); _temp2396->tl= decls;
_temp2396;}); nv= Cyc_Toc_add_varmap( nv, _temp2320->name, Cyc_Absyn_var_exp(
_temp2394, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2394, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2268;} _LL2276: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2268; _LL2278: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2324, _temp2322, 0), succ_lab, fail_lab); goto _LL2268;
_LL2280: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2326, 0),
succ_lab, fail_lab); goto _LL2268; _LL2282: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2328, 0), succ_lab, fail_lab); goto _LL2268; _LL2284: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2398=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2398[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2399; _temp2399.tag= Cyc_Absyn_Enum_e;
_temp2399.f1= _temp2330->name; _temp2399.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2332; _temp2399.f3=( struct Cyc_Absyn_Enumfield*) _temp2330; _temp2399;});
_temp2398;}), 0), succ_lab, fail_lab); goto _LL2268; _LL2286: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2338->name, 0); if( ! _temp2340->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2268;} _LL2288: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2352);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2356->name,({
unsigned char* _temp2433=( unsigned char*)"_struct"; struct _tagged_string
_temp2434; _temp2434.curr= _temp2433; _temp2434.base= _temp2433; _temp2434.last_plus_one=
_temp2433 + 8; _temp2434;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp2400=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2356->typs); goto _LL2401;
_LL2401: { struct Cyc_List_List* _temp2402=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2352); goto
_LL2403; _LL2403: for( 0; _temp2402 != 0;((( _temp2402=(( struct Cyc_List_List*)
_check_null( _temp2402))->tl, _temp2400=(( struct Cyc_List_List*) _check_null(
_temp2400))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp2404=( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp2402))->hd; goto _LL2405; _LL2405: {
void* _temp2406=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2400))->hd)).f2; goto _LL2407; _LL2407: { struct _tuple0* _temp2408= Cyc_Toc_temp_var();
goto _LL2409; _LL2409: { void* _temp2410=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2404->topt))->v; goto _LL2411; _LL2411: { void* _temp2412= Cyc_Toc_typ_to_c(
_temp2410); goto _LL2413; _LL2413: { struct _tagged_string* _temp2414= Cyc_Toc_fresh_label();
goto _LL2415; _LL2415: { struct Cyc_Absyn_Exp* _temp2416= Cyc_Absyn_structarrow_exp(
rcast, Cyc_Toc_fieldname( cnt), 0); goto _LL2417; _LL2417: if( Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c( _temp2406))){ _temp2416= Cyc_Absyn_cast_exp( _temp2412,
_temp2416, 0);} decls=({ struct Cyc_List_List* _temp2418=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2418->hd=( void*)({
struct _tuple14* _temp2419=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2419->f1= _temp2408; _temp2419->f2= _temp2412; _temp2419;});
_temp2418->tl= decls; _temp2418;});{ struct _tuple13 _temp2420= Cyc_Toc_xlate_pat(
nv, rgn, _temp2410, Cyc_Absyn_var_exp( _temp2408, 0), _temp2416, _temp2404,
succ_lab, fail_lab, decls); goto _LL2421; _LL2421: nv= _temp2420.f1; decls=
_temp2420.f2;{ struct Cyc_Absyn_Stmt* _temp2422= _temp2420.f3; goto _LL2423;
_LL2423: { struct Cyc_Absyn_Stmt* _temp2424= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2408, 0), _temp2416, 0); goto _LL2425; _LL2425: { struct Cyc_Absyn_Stmt*
_temp2426= Cyc_Absyn_seq_stmt( _temp2424, _temp2422, 0); goto _LL2427; _LL2427:
ss=({ struct Cyc_List_List* _temp2428=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2428->hd=( void*) Cyc_Absyn_label_stmt(
_temp2414, _temp2426, 0); _temp2428->tl= ss; _temp2428;}); succ_lab= _temp2414;}}}}}}}}}}}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2358->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2356->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2429=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2358->fields))->v; goto _LL2430; _LL2430: for( 0; _temp2429 !=
0; _temp2429=(( struct Cyc_List_List*) _check_null( _temp2429))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2431=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2429))->hd; goto _LL2432; _LL2432: if( _temp2431->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2356->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2268;}}} _LL2290: _temp2368=
_temp2360; goto _LL2292; _LL2292: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2368);{ struct Cyc_List_List*
_temp2435=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2368); goto _LL2436; _LL2436: for( 0; _temp2435 !=
0;( _temp2435=(( struct Cyc_List_List*) _check_null( _temp2435))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2437=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2435))->hd; goto _LL2438; _LL2438: { struct _tuple0* _temp2439=
Cyc_Toc_temp_var(); goto _LL2440; _LL2440: { void* _temp2441=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2437->topt))->v; goto _LL2442; _LL2442: {
struct _tagged_string* _temp2443= Cyc_Toc_fresh_label(); goto _LL2444; _LL2444:
decls=({ struct Cyc_List_List* _temp2445=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2445->hd=( void*)({ struct _tuple14*
_temp2446=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2446->f1= _temp2439; _temp2446->f2= Cyc_Toc_typ_to_c( _temp2441); _temp2446;});
_temp2445->tl= decls; _temp2445;});{ struct _tuple13 _temp2447= Cyc_Toc_xlate_pat(
nv, rgn, _temp2441, Cyc_Absyn_var_exp( _temp2439, 0), Cyc_Absyn_structmember_exp(
path, Cyc_Toc_fieldname( cnt), 0), _temp2437, succ_lab, fail_lab, decls); goto
_LL2448; _LL2448: nv= _temp2447.f1; decls= _temp2447.f2;{ struct Cyc_Absyn_Stmt*
_temp2449= _temp2447.f3; goto _LL2450; _LL2450: { struct Cyc_Absyn_Stmt*
_temp2451= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2439, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2449, 0); goto _LL2452; _LL2452: ss=({ struct Cyc_List_List* _temp2453=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2453->hd=(
void*) Cyc_Absyn_label_stmt( _temp2443, _temp2451, 0); _temp2453->tl= ss;
_temp2453;}); succ_lab= _temp2443;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2268;} _LL2294: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2454=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2370); goto _LL2455; _LL2455: for( 0; _temp2454 !=
0; _temp2454=(( struct Cyc_List_List*) _check_null( _temp2454))->tl){ struct
_tuple15* _temp2456=( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2454))->hd; goto _LL2457; _LL2457: { struct Cyc_Absyn_Pat* _temp2458=(*
_temp2456).f2; goto _LL2459; _LL2459: { struct _tagged_string* f;{ void*
_temp2460=( void*)(( struct Cyc_List_List*) _check_null((* _temp2456).f1))->hd;
struct _tagged_string* _temp2466; _LL2462: if(*(( int*) _temp2460) == Cyc_Absyn_FieldName){
_LL2467: _temp2466=(( struct Cyc_Absyn_FieldName_struct*) _temp2460)->f1; goto
_LL2463;} else{ goto _LL2464;} _LL2464: goto _LL2465; _LL2463: f= _temp2466;
goto _LL2461; _LL2465:( void) _throw(( void*) Cyc_Toc_Match_error); _LL2461:;}{
struct _tuple0* _temp2468= Cyc_Toc_temp_var(); goto _LL2469; _LL2469: { void*
_temp2470=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2458->topt))->v;
goto _LL2471; _LL2471: { void* _temp2472= Cyc_Toc_typ_to_c( _temp2470); goto
_LL2473; _LL2473: { struct _tagged_string* _temp2474= Cyc_Toc_fresh_label();
goto _LL2475; _LL2475: decls=({ struct Cyc_List_List* _temp2476=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2476->hd=( void*)({
struct _tuple14* _temp2477=( struct _tuple14*) _region_malloc( rgn, sizeof(
struct _tuple14)); _temp2477->f1= _temp2468; _temp2477->f2= _temp2472; _temp2477;});
_temp2476->tl= decls; _temp2476;});{ struct _tuple13 _temp2478= Cyc_Toc_xlate_pat(
nv, rgn, _temp2470, Cyc_Absyn_var_exp( _temp2468, 0), Cyc_Absyn_structmember_exp(
path, f, 0), _temp2458, succ_lab, fail_lab, decls); goto _LL2479; _LL2479: nv=
_temp2478.f1; decls= _temp2478.f2;{ struct Cyc_Absyn_Exp* _temp2480= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2481; _LL2481: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2376->fields))->v, f)))->type)){ _temp2480= Cyc_Absyn_cast_exp(
_temp2472, _temp2480, 0);}{ struct Cyc_Absyn_Stmt* _temp2482= _temp2478.f3; goto
_LL2483; _LL2483: { struct Cyc_Absyn_Stmt* _temp2484= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2468, 0), _temp2480, 0), _temp2482, 0); goto _LL2485;
_LL2485: ss=({ struct Cyc_List_List* _temp2486=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2486->hd=( void*) Cyc_Absyn_label_stmt(
_temp2474, _temp2484, 0); _temp2486->tl= ss; _temp2486;}); succ_lab= _temp2474;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2268;} _LL2296: { struct _tuple0*
_temp2487= Cyc_Toc_temp_var(); goto _LL2488; _LL2488: { void* _temp2489=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2378->topt))->v; goto _LL2490; _LL2490:
decls=({ struct Cyc_List_List* _temp2491=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2491->hd=( void*)({ struct _tuple14*
_temp2492=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2492->f1= _temp2487; _temp2492->f2= Cyc_Toc_typ_to_c( _temp2489); _temp2492;});
_temp2491->tl= decls; _temp2491;});{ struct _tuple13 _temp2493= Cyc_Toc_xlate_pat(
nv, rgn, _temp2489, Cyc_Absyn_var_exp( _temp2487, 0), Cyc_Absyn_deref_exp( path,
0), _temp2378, succ_lab, fail_lab, decls); goto _LL2494; _LL2494: nv= _temp2493.f1;
decls= _temp2493.f2;{ struct Cyc_Absyn_Stmt* _temp2495= _temp2493.f3; goto
_LL2496; _LL2496: { struct Cyc_Absyn_Stmt* _temp2497= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2487, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2495, 0);
goto _LL2498; _LL2498: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2497, 0);} else{ s= _temp2497;} goto _LL2268;}}}}} _LL2298: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2499=( unsigned char*)"unknownid"; struct _tagged_string
_temp2500; _temp2500.curr= _temp2499; _temp2500.base= _temp2499; _temp2500.last_plus_one=
_temp2499 + 10; _temp2500;})); goto _LL2268; _LL2300: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2501=(
unsigned char*)"unknowncall"; struct _tagged_string _temp2502; _temp2502.curr=
_temp2501; _temp2502.base= _temp2501; _temp2502.last_plus_one= _temp2501 + 12;
_temp2502;})); goto _LL2268; _LL2302: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2503=(
unsigned char*)"unknownfields"; struct _tagged_string _temp2504; _temp2504.curr=
_temp2503; _temp2504.base= _temp2503; _temp2504.last_plus_one= _temp2503 + 14;
_temp2504;})); goto _LL2268; _LL2268:;} return({ struct _tuple13 _temp2505;
_temp2505.f1= nv; _temp2505.f2= decls; _temp2505.f3= s; _temp2505;});} struct
_tuple16{ struct _tagged_string* f1; struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple16* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple16* _temp2506=(
struct _tuple16*) _region_malloc( r, sizeof( struct _tuple16)); _temp2506->f1=
Cyc_Toc_fresh_label(); _temp2506->f2= Cyc_Toc_fresh_label(); _temp2506->f3= sc;
_temp2506;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp2507=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2508; _LL2508: { int leave_as_switch;{ void*
_temp2509= Cyc_Tcutil_compress( _temp2507); void* _temp2515; void* _temp2517;
_LL2511: if(( unsigned int) _temp2509 > 4u?*(( int*) _temp2509) == Cyc_Absyn_IntType:
0){ _LL2518: _temp2517=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2509)->f1;
goto _LL2516; _LL2516: _temp2515=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2509)->f2; goto _LL2512;} else{ goto _LL2513;} _LL2513: goto _LL2514;
_LL2512: leave_as_switch= 1; goto _LL2510; _LL2514: leave_as_switch= 0; goto
_LL2510; _LL2510:;}{ struct Cyc_List_List* _temp2519= scs; goto _LL2520; _LL2520:
for( 0; _temp2519 != 0; _temp2519=(( struct Cyc_List_List*) _check_null(
_temp2519))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2519))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2519))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2521= scs; goto
_LL2522; _LL2522: for( 0; _temp2521 != 0; _temp2521=(( struct Cyc_List_List*)
_check_null( _temp2521))->tl){ struct Cyc_Absyn_Stmt* _temp2523=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2521))->hd)->body; goto _LL2524;
_LL2524:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2521))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2523, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2523);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2525=
_new_region(); struct _RegionHandle* rgn=& _temp2525; _push_region( rgn);{
struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple16*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2526= lscs; goto
_LL2527; _LL2527: for( 0; _temp2526 != 0; _temp2526=(( struct Cyc_List_List*)
_check_null( _temp2526))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct
_tuple16*)(( struct Cyc_List_List*) _check_null( _temp2526))->hd)).f3; struct
_tagged_string* fail_lab=(( struct Cyc_List_List*) _check_null( _temp2526))->tl
== 0? end_l:(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp2526))->tl))->hd)).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2526))->hd)).f2; if( sc->where_clause == 0){ struct _tuple13
_temp2528= Cyc_Toc_xlate_pat( nv, rgn, _temp2507, r, path, sc->pattern, succ_lab,
fail_lab, decls); goto _LL2529; _LL2529: nvs=({ struct Cyc_List_List* _temp2530=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2530->hd=( void*) _temp2528.f1; _temp2530->tl= nvs; _temp2530;}); decls=
_temp2528.f2; test_stmts=({ struct Cyc_List_List* _temp2531=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2531->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2526))->hd)).f1,
_temp2528.f3, 0); _temp2531->tl= test_stmts; _temp2531;});} else{ struct Cyc_Absyn_Exp*
_temp2532=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2533;
_LL2533: { struct _tagged_string* _temp2534= Cyc_Toc_fresh_label(); goto _LL2535;
_LL2535: { struct _tuple13 _temp2536= Cyc_Toc_xlate_pat( nv, rgn, _temp2507, r,
path, sc->pattern, _temp2534, fail_lab, decls); goto _LL2537; _LL2537: Cyc_Toc_exp_to_c(
_temp2536.f1, _temp2532); nvs=({ struct Cyc_List_List* _temp2538=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2538->hd=( void*)
_temp2536.f1; _temp2538->tl= nvs; _temp2538;}); decls= _temp2536.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2532, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2534, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2539=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2539->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2526))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2536.f3, s3, 0), 0); _temp2539->tl= test_stmts;
_temp2539;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2540= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2541; _LL2541: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2542=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2543; _LL2543: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2542, end_l,(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2544=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2545; _LL2545: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2544, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2540, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple14 _temp2548;
void* _temp2549; struct _tuple0* _temp2551; struct _tuple14* _temp2546=( struct
_tuple14*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2548=*
_temp2546; _LL2552: _temp2551= _temp2548.f1; goto _LL2550; _LL2550: _temp2549=
_temp2548.f2; goto _LL2547; _LL2547: res= Cyc_Absyn_declare_stmt( _temp2551,
_temp2549, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region(& _temp2525);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2553=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2553[ 0]=({ struct Cyc_List_List _temp2554; _temp2554.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2554.tl= 0; _temp2554;}); _temp2553;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2555=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2597; struct Cyc_Absyn_Stmt* _temp2599; struct Cyc_Absyn_Stmt*
_temp2601; struct Cyc_Absyn_Exp* _temp2603; struct Cyc_Absyn_Stmt* _temp2605;
struct Cyc_Absyn_Stmt* _temp2607; struct Cyc_Absyn_Exp* _temp2609; struct Cyc_Absyn_Stmt*
_temp2611; struct _tuple2 _temp2613; struct Cyc_Absyn_Stmt* _temp2615; struct
Cyc_Absyn_Exp* _temp2617; struct Cyc_Absyn_Stmt* _temp2619; struct Cyc_Absyn_Stmt*
_temp2621; struct Cyc_Absyn_Stmt* _temp2623; struct _tagged_string* _temp2625;
struct Cyc_Absyn_Stmt* _temp2627; struct _tuple2 _temp2629; struct Cyc_Absyn_Stmt*
_temp2631; struct Cyc_Absyn_Exp* _temp2633; struct _tuple2 _temp2635; struct Cyc_Absyn_Stmt*
_temp2637; struct Cyc_Absyn_Exp* _temp2639; struct Cyc_Absyn_Exp* _temp2641;
struct Cyc_List_List* _temp2643; struct Cyc_Absyn_Exp* _temp2645; struct Cyc_Absyn_Switch_clause**
_temp2647; struct Cyc_List_List* _temp2649; struct Cyc_Absyn_Stmt* _temp2651;
struct Cyc_Absyn_Decl* _temp2653; struct Cyc_Absyn_Stmt* _temp2655; struct
_tagged_string* _temp2657; struct _tuple2 _temp2659; struct Cyc_Absyn_Stmt*
_temp2661; struct Cyc_Absyn_Exp* _temp2663; struct Cyc_Absyn_Stmt* _temp2665;
struct Cyc_List_List* _temp2667; struct Cyc_Absyn_Stmt* _temp2669; struct Cyc_Absyn_Stmt*
_temp2671; struct Cyc_Absyn_Vardecl* _temp2673; struct Cyc_Absyn_Tvar* _temp2675;
struct Cyc_List_List* _temp2677; struct Cyc_Absyn_Exp* _temp2679; struct Cyc_Absyn_Stmt*
_temp2681; struct Cyc_Absyn_Stmt* _temp2683; _LL2557: if( _temp2555 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2558;} else{ goto _LL2559;} _LL2559: if((
unsigned int) _temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_Exp_s: 0){
_LL2598: _temp2597=(( struct Cyc_Absyn_Exp_s_struct*) _temp2555)->f1; goto
_LL2560;} else{ goto _LL2561;} _LL2561: if(( unsigned int) _temp2555 > 1u?*((
int*) _temp2555) == Cyc_Absyn_Seq_s: 0){ _LL2602: _temp2601=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2555)->f1; goto _LL2600; _LL2600: _temp2599=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2555)->f2; goto _LL2562;} else{ goto _LL2563;} _LL2563: if(( unsigned int)
_temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_Return_s: 0){ _LL2604:
_temp2603=(( struct Cyc_Absyn_Return_s_struct*) _temp2555)->f1; goto _LL2564;}
else{ goto _LL2565;} _LL2565: if(( unsigned int) _temp2555 > 1u?*(( int*)
_temp2555) == Cyc_Absyn_IfThenElse_s: 0){ _LL2610: _temp2609=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2555)->f1; goto _LL2608; _LL2608: _temp2607=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2555)->f2; goto _LL2606; _LL2606: _temp2605=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2555)->f3; goto _LL2566;} else{ goto _LL2567;} _LL2567: if(( unsigned int)
_temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_While_s: 0){ _LL2614: _temp2613=((
struct Cyc_Absyn_While_s_struct*) _temp2555)->f1; _LL2618: _temp2617= _temp2613.f1;
goto _LL2616; _LL2616: _temp2615= _temp2613.f2; goto _LL2612; _LL2612: _temp2611=((
struct Cyc_Absyn_While_s_struct*) _temp2555)->f2; goto _LL2568;} else{ goto
_LL2569;} _LL2569: if(( unsigned int) _temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_Break_s:
0){ _LL2620: _temp2619=(( struct Cyc_Absyn_Break_s_struct*) _temp2555)->f1; goto
_LL2570;} else{ goto _LL2571;} _LL2571: if(( unsigned int) _temp2555 > 1u?*((
int*) _temp2555) == Cyc_Absyn_Continue_s: 0){ _LL2622: _temp2621=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2555)->f1; goto _LL2572;} else{ goto _LL2573;} _LL2573: if(( unsigned int)
_temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_Goto_s: 0){ _LL2626: _temp2625=((
struct Cyc_Absyn_Goto_s_struct*) _temp2555)->f1; goto _LL2624; _LL2624:
_temp2623=(( struct Cyc_Absyn_Goto_s_struct*) _temp2555)->f2; goto _LL2574;}
else{ goto _LL2575;} _LL2575: if(( unsigned int) _temp2555 > 1u?*(( int*)
_temp2555) == Cyc_Absyn_For_s: 0){ _LL2642: _temp2641=(( struct Cyc_Absyn_For_s_struct*)
_temp2555)->f1; goto _LL2636; _LL2636: _temp2635=(( struct Cyc_Absyn_For_s_struct*)
_temp2555)->f2; _LL2640: _temp2639= _temp2635.f1; goto _LL2638; _LL2638:
_temp2637= _temp2635.f2; goto _LL2630; _LL2630: _temp2629=(( struct Cyc_Absyn_For_s_struct*)
_temp2555)->f3; _LL2634: _temp2633= _temp2629.f1; goto _LL2632; _LL2632:
_temp2631= _temp2629.f2; goto _LL2628; _LL2628: _temp2627=(( struct Cyc_Absyn_For_s_struct*)
_temp2555)->f4; goto _LL2576;} else{ goto _LL2577;} _LL2577: if(( unsigned int)
_temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_Switch_s: 0){ _LL2646:
_temp2645=(( struct Cyc_Absyn_Switch_s_struct*) _temp2555)->f1; goto _LL2644;
_LL2644: _temp2643=(( struct Cyc_Absyn_Switch_s_struct*) _temp2555)->f2; goto
_LL2578;} else{ goto _LL2579;} _LL2579: if(( unsigned int) _temp2555 > 1u?*((
int*) _temp2555) == Cyc_Absyn_Fallthru_s: 0){ _LL2650: _temp2649=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2555)->f1; goto _LL2648; _LL2648: _temp2647=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2555)->f2; goto _LL2580;} else{ goto _LL2581;} _LL2581: if(( unsigned int)
_temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_Decl_s: 0){ _LL2654: _temp2653=((
struct Cyc_Absyn_Decl_s_struct*) _temp2555)->f1; goto _LL2652; _LL2652:
_temp2651=(( struct Cyc_Absyn_Decl_s_struct*) _temp2555)->f2; goto _LL2582;}
else{ goto _LL2583;} _LL2583: if(( unsigned int) _temp2555 > 1u?*(( int*)
_temp2555) == Cyc_Absyn_Label_s: 0){ _LL2658: _temp2657=(( struct Cyc_Absyn_Label_s_struct*)
_temp2555)->f1; goto _LL2656; _LL2656: _temp2655=(( struct Cyc_Absyn_Label_s_struct*)
_temp2555)->f2; goto _LL2584;} else{ goto _LL2585;} _LL2585: if(( unsigned int)
_temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_Do_s: 0){ _LL2666: _temp2665=((
struct Cyc_Absyn_Do_s_struct*) _temp2555)->f1; goto _LL2660; _LL2660: _temp2659=((
struct Cyc_Absyn_Do_s_struct*) _temp2555)->f2; _LL2664: _temp2663= _temp2659.f1;
goto _LL2662; _LL2662: _temp2661= _temp2659.f2; goto _LL2586;} else{ goto
_LL2587;} _LL2587: if(( unsigned int) _temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_TryCatch_s:
0){ _LL2670: _temp2669=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2555)->f1;
goto _LL2668; _LL2668: _temp2667=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2555)->f2; goto _LL2588;} else{ goto _LL2589;} _LL2589: if(( unsigned int)
_temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_Region_s: 0){ _LL2676:
_temp2675=(( struct Cyc_Absyn_Region_s_struct*) _temp2555)->f1; goto _LL2674;
_LL2674: _temp2673=(( struct Cyc_Absyn_Region_s_struct*) _temp2555)->f2; goto
_LL2672; _LL2672: _temp2671=(( struct Cyc_Absyn_Region_s_struct*) _temp2555)->f3;
goto _LL2590;} else{ goto _LL2591;} _LL2591: if(( unsigned int) _temp2555 > 1u?*((
int*) _temp2555) == Cyc_Absyn_SwitchC_s: 0){ _LL2680: _temp2679=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2555)->f1; goto _LL2678; _LL2678: _temp2677=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2555)->f2; goto _LL2592;} else{ goto _LL2593;} _LL2593: if(( unsigned int)
_temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_Cut_s: 0){ _LL2682: _temp2681=((
struct Cyc_Absyn_Cut_s_struct*) _temp2555)->f1; goto _LL2594;} else{ goto
_LL2595;} _LL2595: if(( unsigned int) _temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_Splice_s:
0){ _LL2684: _temp2683=(( struct Cyc_Absyn_Splice_s_struct*) _temp2555)->f1;
goto _LL2596;} else{ goto _LL2556;} _LL2558: return; _LL2560: Cyc_Toc_exp_to_c(
nv, _temp2597); return; _LL2562: Cyc_Toc_stmt_to_c( nv, _temp2601); s= _temp2599;
continue; _LL2564: { struct Cyc_Core_Opt* topt= 0; if( _temp2603 != 0){ topt=({
struct Cyc_Core_Opt* _temp2685=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2685->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2603))->topt))->v);
_temp2685;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2603));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2686=
Cyc_Toc_temp_var(); goto _LL2687; _LL2687: { struct Cyc_Absyn_Stmt* _temp2688=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2686, 0),
0); goto _LL2689; _LL2689:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2686,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2603,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2688, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2566: Cyc_Toc_exp_to_c(
nv, _temp2609); Cyc_Toc_stmt_to_c( nv, _temp2607); s= _temp2605; continue;
_LL2568: Cyc_Toc_exp_to_c( nv, _temp2617); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2611); return; _LL2570: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2619 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2619))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2572: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -(( struct Cyc_Absyn_Stmt*) _check_null( _temp2621))->try_depth, s);
return; _LL2574: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2623))->try_depth, s); return; _LL2576: Cyc_Toc_exp_to_c( nv,
_temp2641); Cyc_Toc_exp_to_c( nv, _temp2639); Cyc_Toc_exp_to_c( nv, _temp2633);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2627); return; _LL2578: Cyc_Toc_xlate_switch(
nv, s, _temp2645, _temp2643); return; _LL2580: if( nv->fallthru_info == 0){(
void) _throw((( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp2690=( unsigned char*)"fallthru in unexpected place"; struct
_tagged_string _temp2691; _temp2691.curr= _temp2690; _temp2691.base= _temp2690;
_temp2691.last_plus_one= _temp2690 + 29; _temp2691;})));}{ struct _tuple8
_temp2694; struct Cyc_Dict_Dict* _temp2695; struct Cyc_List_List* _temp2697;
struct _tagged_string* _temp2699; struct _tuple8* _temp2692=( struct _tuple8*)((
struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp2694=* _temp2692;
_LL2700: _temp2699= _temp2694.f1; goto _LL2698; _LL2698: _temp2697= _temp2694.f2;
goto _LL2696; _LL2696: _temp2695= _temp2694.f3; goto _LL2693; _LL2693: { struct
Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2699, 0); Cyc_Toc_do_npop_before(
s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp2647)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp2701=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2697); goto _LL2702; _LL2702: { struct Cyc_List_List*
_temp2703=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2649); goto _LL2704; _LL2704: for( 0; _temp2701 != 0;( _temp2701=(( struct
Cyc_List_List*) _check_null( _temp2701))->tl, _temp2703=(( struct Cyc_List_List*)
_check_null( _temp2703))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp2703))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp2695,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp2701))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp2703))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}}} _LL2582:{ void* _temp2705=( void*) _temp2653->r; int
_temp2715; struct Cyc_Absyn_Exp* _temp2717; struct Cyc_Core_Opt* _temp2719;
struct Cyc_Core_Opt* _temp2721; struct Cyc_Absyn_Pat* _temp2723; struct Cyc_List_List*
_temp2725; struct Cyc_Absyn_Vardecl* _temp2727; _LL2707: if(*(( int*) _temp2705)
== Cyc_Absyn_Let_d){ _LL2724: _temp2723=(( struct Cyc_Absyn_Let_d_struct*)
_temp2705)->f1; goto _LL2722; _LL2722: _temp2721=(( struct Cyc_Absyn_Let_d_struct*)
_temp2705)->f2; goto _LL2720; _LL2720: _temp2719=(( struct Cyc_Absyn_Let_d_struct*)
_temp2705)->f3; goto _LL2718; _LL2718: _temp2717=(( struct Cyc_Absyn_Let_d_struct*)
_temp2705)->f4; goto _LL2716; _LL2716: _temp2715=(( struct Cyc_Absyn_Let_d_struct*)
_temp2705)->f5; goto _LL2708;} else{ goto _LL2709;} _LL2709: if(*(( int*)
_temp2705) == Cyc_Absyn_Letv_d){ _LL2726: _temp2725=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2705)->f1; goto _LL2710;} else{ goto _LL2711;} _LL2711: if(*(( int*)
_temp2705) == Cyc_Absyn_Var_d){ _LL2728: _temp2727=(( struct Cyc_Absyn_Var_d_struct*)
_temp2705)->f1; goto _LL2712;} else{ goto _LL2713;} _LL2713: goto _LL2714;
_LL2708:( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2723,
_temp2719, _temp2717, _temp2715, _temp2651))->r)); goto _LL2706; _LL2710: {
struct Cyc_List_List* _temp2729=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2725); goto _LL2730; _LL2730: if( _temp2729 == 0){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2731=(
unsigned char*)"empty Letv_d"; struct _tagged_string _temp2732; _temp2732.curr=
_temp2731; _temp2732.base= _temp2731; _temp2732.last_plus_one= _temp2731 + 13;
_temp2732;}));}( void*)( _temp2653->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2733=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2733[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2734; _temp2734.tag= Cyc_Absyn_Var_d;
_temp2734.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2729))->hd; _temp2734;}); _temp2733;}))); _temp2729=(( struct Cyc_List_List*)
_check_null( _temp2729))->tl; for( 0; _temp2729 != 0; _temp2729=(( struct Cyc_List_List*)
_check_null( _temp2729))->tl){ struct Cyc_Absyn_Decl* _temp2737= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2735=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2735[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2736; _temp2736.tag= Cyc_Absyn_Var_d; _temp2736.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2729))->hd; _temp2736;}); _temp2735;}),
0); goto _LL2738; _LL2738:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp2737, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c(
nv, s); goto _LL2706;} _LL2712: { struct _tuple0* _temp2739= _temp2727->name;
goto _LL2740; _LL2740:( void*)( _temp2727->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2727->type));{ struct Cyc_Toc_Env* _temp2743= Cyc_Toc_add_varmap( nv,
_temp2739, Cyc_Absyn_varb_exp( _temp2739,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2741=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2741[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2742; _temp2742.tag= Cyc_Absyn_Local_b;
_temp2742.f1= _temp2727; _temp2742;}); _temp2741;}), 0)); goto _LL2744; _LL2744:
Cyc_Toc_stmt_to_c( _temp2743, _temp2651); if( _temp2727->initializer != 0){
struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*) _check_null( _temp2727->initializer);
void* _temp2745=( void*) init->r; struct Cyc_Absyn_Exp* _temp2751; struct Cyc_Absyn_Exp*
_temp2753; struct Cyc_Absyn_Vardecl* _temp2755; _LL2747: if(*(( int*) _temp2745)
== Cyc_Absyn_Comprehension_e){ _LL2756: _temp2755=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2745)->f1; goto _LL2754; _LL2754: _temp2753=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2745)->f2; goto _LL2752; _LL2752: _temp2751=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2745)->f3; goto _LL2748;} else{ goto _LL2749;} _LL2749: goto _LL2750;
_LL2748: _temp2727->initializer= 0;( void*)( _temp2651->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2739, 0), _temp2755, _temp2753, _temp2751, Cyc_Absyn_new_stmt((
void*) _temp2651->r, 0), 0))->r)); goto _LL2746; _LL2750: Cyc_Toc_exp_to_c( nv,
init); goto _LL2746; _LL2746:;} goto _LL2706;}} _LL2714:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp2757=( unsigned char*)"bad nested declaration within function";
struct _tagged_string _temp2758; _temp2758.curr= _temp2757; _temp2758.base=
_temp2757; _temp2758.last_plus_one= _temp2757 + 39; _temp2758;})); goto _LL2706;
_LL2706:;} return; _LL2584: s= _temp2655; continue; _LL2586: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp2665); Cyc_Toc_exp_to_c( nv, _temp2663); return;
_LL2588: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__push_handler_qv,
0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp( Cyc_Toc_setjmp_qv, 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__pop_handler_qv,
0); struct Cyc_Absyn_Exp* _exn_thrown_exp= Cyc_Absyn_var_exp( Cyc_Toc__exn_thrown_qv,
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2759=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2759->v=( void*) e_typ; _temp2759;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2669);{
struct Cyc_Absyn_Stmt* _temp2760= Cyc_Absyn_seq_stmt( _temp2669, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); goto _LL2761; _LL2761: {
struct _tuple0* _temp2762= Cyc_Toc_temp_var(); goto _LL2763; _LL2763: { struct
Cyc_Absyn_Exp* _temp2764= Cyc_Absyn_var_exp( _temp2762, 0); goto _LL2765;
_LL2765: { struct Cyc_Absyn_Vardecl* _temp2766= Cyc_Absyn_new_vardecl( _temp2762,
Cyc_Absyn_exn_typ, 0); goto _LL2767; _LL2767: _temp2764->topt=({ struct Cyc_Core_Opt*
_temp2768=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2768->v=( void*) Cyc_Absyn_exn_typ; _temp2768;});{ struct Cyc_Absyn_Pat*
_temp2773=({ struct Cyc_Absyn_Pat* _temp2769=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2769->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2771=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2771[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2772; _temp2772.tag= Cyc_Absyn_Var_p;
_temp2772.f1= _temp2766; _temp2772;}); _temp2771;})); _temp2769->topt=({ struct
Cyc_Core_Opt* _temp2770=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2770->v=( void*) Cyc_Absyn_exn_typ; _temp2770;}); _temp2769->loc= 0;
_temp2769;}); goto _LL2774; _LL2774: { struct Cyc_Absyn_Exp* _temp2775= Cyc_Absyn_throw_exp(
_temp2764, 0); goto _LL2776; _LL2776: _temp2775->topt=({ struct Cyc_Core_Opt*
_temp2777=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2777->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2777;});{ struct Cyc_Absyn_Stmt*
_temp2778= Cyc_Absyn_exp_stmt( _temp2775, 0); goto _LL2779; _LL2779: { struct
Cyc_Absyn_Switch_clause* _temp2783=({ struct Cyc_Absyn_Switch_clause* _temp2780=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2780->pattern= _temp2773; _temp2780->pat_vars=({ struct Cyc_Core_Opt*
_temp2781=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2781->v=( void*)({ struct Cyc_List_List* _temp2782=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2782->hd=( void*) _temp2766;
_temp2782->tl= 0; _temp2782;}); _temp2781;}); _temp2780->where_clause= 0;
_temp2780->body= _temp2778; _temp2780->loc= 0; _temp2780;}); goto _LL2784;
_LL2784: { struct Cyc_Absyn_Stmt* _temp2786= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2667,({ struct Cyc_List_List* _temp2785=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2785->hd=( void*) _temp2783; _temp2785->tl=
0; _temp2785;})), 0); goto _LL2787; _LL2787: Cyc_Toc_stmt_to_c( nv, _temp2786);{
struct Cyc_Absyn_Exp* _temp2789= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2788=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2788->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2788->tl= 0; _temp2788;}), 0); goto _LL2790; _LL2790: { struct Cyc_Absyn_Stmt*
_temp2792= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2791=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2791->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2791->tl= 0;
_temp2791;}), 0), 0); goto _LL2793; _LL2793: { struct Cyc_Absyn_Exp* _temp2794=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2795; _LL2795: {
struct Cyc_Absyn_Exp* _temp2796= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2797; _LL2797:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2792, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2794, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2789, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2796, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2760, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2786, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2590: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2673->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2803=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2803[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2804; _temp2804.tag= Cyc_Absyn_Local_b; _temp2804.f1=
_temp2673; _temp2804;}); _temp2803;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2802=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2802->f1= Cyc_Toc_abs_ns; _temp2802->f2= Cyc_Toc__push_region_sp;
_temp2802;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2801=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2801->f1= Cyc_Toc_abs_ns; _temp2801->f2= Cyc_Toc__pop_region_sp; _temp2801;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2800=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2800->f1=
Cyc_Toc_abs_ns; _temp2800->f2= Cyc_Toc__new_region_sp; _temp2800;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2799=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2799->hd=( void*) addr_rh_exp; _temp2799->tl= 0; _temp2799;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2798=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2798->hd=( void*) x_exp; _temp2798->tl= 0; _temp2798;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2671);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2671, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2592:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp2805=(
unsigned char*)"switchC_s"; struct _tagged_string _temp2806; _temp2806.curr=
_temp2805; _temp2806.base= _temp2805; _temp2806.last_plus_one= _temp2805 + 10;
_temp2806;})); return; _LL2594:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
unsigned char* _temp2807=( unsigned char*)"cut"; struct _tagged_string _temp2808;
_temp2808.curr= _temp2807; _temp2808.base= _temp2807; _temp2808.last_plus_one=
_temp2807 + 4; _temp2808;})); return; _LL2596:(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(({ unsigned char* _temp2809=( unsigned char*)"splice"; struct
_tagged_string _temp2810; _temp2810.curr= _temp2809; _temp2810.base= _temp2809;
_temp2810.last_plus_one= _temp2809 + 7; _temp2810;})); return; _LL2556:;}}
struct _tuple17{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp2811= f->args; goto _LL2812;
_LL2812: for( 0; _temp2811 != 0; _temp2811=(( struct Cyc_List_List*) _check_null(
_temp2811))->tl){(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2811))->hd)).f3= Cyc_Toc_typ_to_c((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp2811))->hd)).f3);{ struct _tuple0* _temp2814=({ struct _tuple0*
_temp2813=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2813->f1=(
void*) Cyc_Absyn_Loc_n; _temp2813->f2=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp2811))->hd)).f1; _temp2813;}); goto _LL2815; _LL2815: nv= Cyc_Toc_add_varmap(
nv, _temp2814, Cyc_Absyn_var_exp( _temp2814, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2818; void* _temp2820; void* _temp2822; struct Cyc_Absyn_Tqual _temp2824;
struct Cyc_Core_Opt* _temp2826; struct Cyc_Absyn_VarargInfo _temp2816=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2827: _temp2826=
_temp2816.name; goto _LL2825; _LL2825: _temp2824= _temp2816.tq; goto _LL2823;
_LL2823: _temp2822=( void*) _temp2816.type; goto _LL2821; _LL2821: _temp2820=(
void*) _temp2816.rgn; goto _LL2819; _LL2819: _temp2818= _temp2816.inject; goto
_LL2817; _LL2817: { void* _temp2828= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2822, _temp2820, _temp2824)); goto _LL2829; _LL2829: { struct _tuple0*
_temp2831=({ struct _tuple0* _temp2830=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2830->f1=( void*) Cyc_Absyn_Loc_n; _temp2830->f2=( struct
_tagged_string*)(( struct Cyc_Core_Opt*) _check_null( _temp2826))->v; _temp2830;});
goto _LL2832; _LL2832: f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2833=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2833->hd=( void*)({ struct _tuple17* _temp2834=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp2834->f1=( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( _temp2826))->v; _temp2834->f2= _temp2824;
_temp2834->f3= _temp2828; _temp2834;}); _temp2833->tl= 0; _temp2833;})); nv= Cyc_Toc_add_varmap(
nv, _temp2831, Cyc_Absyn_var_exp( _temp2831, 0)); f->cyc_varargs= 0;}}}{ struct
Cyc_List_List* _temp2835=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; goto _LL2836; _LL2836: for( 0; _temp2835 !=
0; _temp2835=(( struct Cyc_List_List*) _check_null( _temp2835))->tl){( void*)(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp2835))->hd)->type=(
void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp2835))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2837= s;
_LL2839: if( _temp2837 ==( void*) Cyc_Absyn_Abstract){ goto _LL2840;} else{ goto
_LL2841;} _LL2841: if( _temp2837 ==( void*) Cyc_Absyn_ExternC){ goto _LL2842;}
else{ goto _LL2843;} _LL2843: goto _LL2844; _LL2840: return( void*) Cyc_Absyn_Public;
_LL2842: return( void*) Cyc_Absyn_Extern; _LL2844: return s; _LL2838:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2845=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2846; _LL2846: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2847=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2847->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2847;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2848=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2845).f2); goto _LL2849;
_LL2849: if( _temp2848 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2845).f2, s));}
else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null(
_temp2848))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2845).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2850=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2850->v=( void*)
_temp2845; _temp2850;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2851=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2852; _LL2852: for( 0; _temp2851 != 0;
_temp2851=(( struct Cyc_List_List*) _check_null( _temp2851))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2851))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2851))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2853=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2854; _LL2854: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2855=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2855->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2855;});}{ int seen_defn_before; struct Cyc_Core_Opt*
_temp2856=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2853).f2); goto _LL2857; _LL2857:
if( _temp2856 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2853).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2856))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2853).f2, u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)(
u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==(
void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*)
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2858=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2858->v=( void*) _temp2853; _temp2858;}); if( u->fields !=
0){ if( seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* _temp2859=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( u->fields))->v; goto
_LL2860; _LL2860: for( 0; _temp2859 != 0; _temp2859=(( struct Cyc_List_List*)
_check_null( _temp2859))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2859))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2859))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2861=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2861->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2861;});}{ struct
_tuple0* _temp2862= tud->name; goto _LL2863; _LL2863: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2862)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2862));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2864=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2865; _LL2865: for( 0;
_temp2864 != 0; _temp2864=(( struct Cyc_List_List*) _check_null( _temp2864))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2864))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2866; _temp2866.q_const= 1; _temp2866.q_volatile= 0;
_temp2866.q_restrict= 0; _temp2866;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2867=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2867->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2868=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2868[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2869; _temp2869.tag= Cyc_Absyn_Var_d;
_temp2869.f1= vd; _temp2869;}); _temp2868;}), 0); _temp2867->tl= Cyc_Toc_result_decls;
_temp2867;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2870; _temp2870.q_const=
1; _temp2870.q_volatile= 0; _temp2870.q_restrict= 0; _temp2870;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2871=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2871->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2872=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2872[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2873; _temp2873.tag= Cyc_Absyn_Var_d; _temp2873.f1= vd; _temp2873;});
_temp2872;}), 0); _temp2871->tl= Cyc_Toc_result_decls; _temp2871;});{ struct Cyc_List_List*
_temp2874= 0; goto _LL2875; _LL2875: { int i= 1;{ struct Cyc_List_List*
_temp2876= f->typs; goto _LL2877; _LL2877: for( 0; _temp2876 != 0;( _temp2876=((
struct Cyc_List_List*) _check_null( _temp2876))->tl, i ++)){ struct
_tagged_string* _temp2878= Cyc_Toc_fieldname( i); goto _LL2879; _LL2879: {
struct Cyc_Absyn_Structfield* _temp2881=({ struct Cyc_Absyn_Structfield*
_temp2880=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2880->name= _temp2878; _temp2880->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2876))->hd)).f1; _temp2880->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2876))->hd)).f2);
_temp2880->width= 0; _temp2880->attributes= 0; _temp2880;}); goto _LL2882;
_LL2882: _temp2874=({ struct Cyc_List_List* _temp2883=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2883->hd=( void*) _temp2881;
_temp2883->tl= _temp2874; _temp2883;});}}} _temp2874=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2874); _temp2874=({ struct Cyc_List_List*
_temp2884=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2884->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2885=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2885->name= Cyc_Toc_tag_sp;
_temp2885->tq= Cyc_Toc_mt_tq; _temp2885->type=( void*) Cyc_Absyn_sint_t;
_temp2885->width= 0; _temp2885->attributes= 0; _temp2885;}); _temp2884->tl=
_temp2874; _temp2884;});{ struct Cyc_Absyn_Structdecl* _temp2891=({ struct Cyc_Absyn_Structdecl*
_temp2886=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2886->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2886->name=({ struct Cyc_Core_Opt*
_temp2888=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2888->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2889=( unsigned char*)"_struct"; struct _tagged_string _temp2890; _temp2890.curr=
_temp2889; _temp2890.base= _temp2889; _temp2890.last_plus_one= _temp2889 + 8;
_temp2890;})); _temp2888;}); _temp2886->tvs= 0; _temp2886->fields=({ struct Cyc_Core_Opt*
_temp2887=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2887->v=( void*) _temp2874; _temp2887;}); _temp2886->attributes= 0;
_temp2886;}); goto _LL2892; _LL2892: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2893=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2893->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2894=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2894[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2895; _temp2895.tag= Cyc_Absyn_Struct_d;
_temp2895.f1= _temp2891; _temp2895;}); _temp2894;}), 0); _temp2893->tl= Cyc_Toc_result_decls;
_temp2893;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2896=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2896->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2896;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2897= xd->name; goto _LL2898; _LL2898: {
struct Cyc_List_List* _temp2899=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2900; _LL2900: for( 0; _temp2899 != 0;
_temp2899=(( struct Cyc_List_List*) _check_null( _temp2899))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2899))->hd; struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2902= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2901=* fn;(
unsigned int)( _temp2901.last_plus_one - _temp2901.curr);}) + 4, 0); goto
_LL2903; _LL2903: { struct Cyc_Absyn_ArrayType_struct* _temp2906=({ struct Cyc_Absyn_ArrayType_struct*
_temp2904=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2904[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2905; _temp2905.tag= Cyc_Absyn_ArrayType;
_temp2905.f1=( void*) Cyc_Absyn_uchar_t; _temp2905.f2= Cyc_Toc_mt_tq; _temp2905.f3=(
struct Cyc_Absyn_Exp*) _temp2902; _temp2905;}); _temp2904;}); goto _LL2907;
_LL2907: { struct Cyc_Core_Opt* _temp2908=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2916; int _temp2917; _LL2910: if( _temp2908 == 0){ goto
_LL2911;} else{ goto _LL2912;} _LL2912: if( _temp2908 == 0){ goto _LL2914;}
else{ _temp2916=* _temp2908; _LL2918: _temp2917=( int) _temp2916.v; if(
_temp2917 == 0){ goto _LL2913;} else{ goto _LL2914;}} _LL2914: goto _LL2915;
_LL2911: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_string)({
int _temp2919= 0; int _temp2920= 0; int _temp2921= 0; int _temp2922= 0; struct
_tagged_string _temp2923=* fn; xprintf("%c%c%c%c%.*s", _temp2919, _temp2920,
_temp2921, _temp2922, _temp2923.last_plus_one - _temp2923.curr, _temp2923.curr);}),
0);}{ struct Cyc_Absyn_Vardecl* _temp2924= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2906, initopt); goto _LL2925; _LL2925:( void*)( _temp2924->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2926=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2926->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2927=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2927[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2928; _temp2928.tag= Cyc_Absyn_Var_d;
_temp2928.f1= _temp2924; _temp2928;}); _temp2927;}), 0); _temp2926->tl= Cyc_Toc_result_decls;
_temp2926;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2929= f->typs; goto _LL2930; _LL2930: for( 0;
_temp2929 != 0;( _temp2929=(( struct Cyc_List_List*) _check_null( _temp2929))->tl,
i ++)){ struct _tagged_string* _temp2932=({ struct _tagged_string* _temp2931=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp2931[ 0]=(
struct _tagged_string) xprintf("f%d", i); _temp2931;}); goto _LL2933; _LL2933: {
struct Cyc_Absyn_Structfield* _temp2935=({ struct Cyc_Absyn_Structfield*
_temp2934=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2934->name= _temp2932; _temp2934->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2929))->hd)).f1; _temp2934->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2929))->hd)).f2);
_temp2934->width= 0; _temp2934->attributes= 0; _temp2934;}); goto _LL2936;
_LL2936: fields=({ struct Cyc_List_List* _temp2937=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2937->hd=( void*) _temp2935;
_temp2937->tl= fields; _temp2937;});}}} fields=({ struct Cyc_List_List*
_temp2938=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2938->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2939=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2939->name= Cyc_Toc_tag_sp;
_temp2939->tq= Cyc_Toc_mt_tq; _temp2939->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2939->width= 0; _temp2939->attributes= 0; _temp2939;});
_temp2938->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2938;});{ struct Cyc_Absyn_Structdecl* _temp2945=({ struct Cyc_Absyn_Structdecl*
_temp2940=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2940->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2940->name=({ struct Cyc_Core_Opt*
_temp2942=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2942->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,({ unsigned char*
_temp2943=( unsigned char*)"_struct"; struct _tagged_string _temp2944; _temp2944.curr=
_temp2943; _temp2944.base= _temp2943; _temp2944.last_plus_one= _temp2943 + 8;
_temp2944;})); _temp2942;}); _temp2940->tvs= 0; _temp2940->fields=({ struct Cyc_Core_Opt*
_temp2941=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2941->v=( void*) fields; _temp2941;}); _temp2940->attributes= 0; _temp2940;});
goto _LL2946; _LL2946: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2947=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2947->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2948=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2948[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2949; _temp2949.tag= Cyc_Absyn_Struct_d;
_temp2949.f1= _temp2945; _temp2949;}); _temp2948;}), 0); _temp2947->tl= Cyc_Toc_result_decls;
_temp2947;});}} goto _LL2909;}} _LL2913: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2955= Cyc_Absyn_string_exp(( struct _tagged_string)({
int _temp2950= 0; int _temp2951= 0; int _temp2952= 0; int _temp2953= 0; struct
_tagged_string _temp2954=* fn; xprintf("%c%c%c%c%.*s", _temp2950, _temp2951,
_temp2952, _temp2953, _temp2954.last_plus_one - _temp2954.curr, _temp2954.curr);}),
0); goto _LL2956; _LL2956: { struct Cyc_Absyn_Vardecl* _temp2957= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2906,( struct Cyc_Absyn_Exp*) _temp2955); goto _LL2958;
_LL2958:( void*)( _temp2957->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2959=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2959->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2960=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2960[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2961; _temp2961.tag= Cyc_Absyn_Var_d; _temp2961.f1= _temp2957; _temp2961;});
_temp2960;}), 0); _temp2959->tl= Cyc_Toc_result_decls; _temp2959;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2909; _LL2915: goto
_LL2909; _LL2909:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2962= ed->name; goto
_LL2963; _LL2963:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2964=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2965; _LL2965: for( 0; _temp2964 != 0;
_temp2964=(( struct Cyc_List_List*) _check_null( _temp2964))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2964))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2964))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2964))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct _RegionHandle
_temp2966= _new_region(); struct _RegionHandle* prgn=& _temp2966; _push_region(
prgn);{ struct Cyc_Absyn_Stmt* _temp2969; struct Cyc_List_List* _temp2971;
struct Cyc_Toc_Env* _temp2973; struct _tuple13 _temp2967= Cyc_Toc_xlate_pat( nv,
prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2974: _temp2973= _temp2967.f1; goto _LL2972; _LL2972: _temp2971=
_temp2967.f2; goto _LL2970; _LL2970: _temp2969= _temp2967.f3; goto _LL2968;
_LL2968: Cyc_Toc_stmt_to_c( _temp2973, s);{ struct Cyc_Absyn_Stmt* _temp2975=
Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL2976; _LL2976: if( exhaust){ s=
Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2969, _temp2975, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2978= Cyc_Toc_newthrow_exp(
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2977=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2977->f1= Cyc_Toc_abs_ns; _temp2977->f2= Cyc_Toc_Match_Exception_sp;
_temp2977;}), 0)); goto _LL2979; _LL2979: { struct Cyc_Absyn_Stmt* _temp2980=
Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2978, 0), 0); goto
_LL2981; _LL2981: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2969, Cyc_Absyn_seq_stmt( _temp2980,
_temp2975, 0), 0), 0);}} for( 0; _temp2971 != 0; _temp2971=(( struct Cyc_List_List*)
_check_null( _temp2971))->tl){ struct _tuple14 _temp2984; void* _temp2985;
struct _tuple0* _temp2987; struct _tuple14* _temp2982=( struct _tuple14*)((
struct Cyc_List_List*) _check_null( _temp2971))->hd; _temp2984=* _temp2982;
_LL2988: _temp2987= _temp2984.f1; goto _LL2986; _LL2986: _temp2985= _temp2984.f2;
goto _LL2983; _LL2983: s= Cyc_Absyn_declare_stmt( _temp2987, _temp2985, 0, s, 0);}}};
_pop_region(& _temp2966);} return s;}} static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ for( 0; ds != 0; ds=((
struct Cyc_List_List*) _check_null( ds))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2989=(
void*) d->r; struct Cyc_Absyn_Vardecl* _temp3015; struct Cyc_Absyn_Fndecl*
_temp3017; int _temp3019; struct Cyc_Absyn_Exp* _temp3021; struct Cyc_Core_Opt*
_temp3023; struct Cyc_Core_Opt* _temp3025; struct Cyc_Absyn_Pat* _temp3027;
struct Cyc_List_List* _temp3029; struct Cyc_Absyn_Structdecl* _temp3031; struct
Cyc_Absyn_Uniondecl* _temp3033; struct Cyc_Absyn_Tuniondecl* _temp3035; struct
Cyc_Absyn_Enumdecl* _temp3037; struct Cyc_Absyn_Typedefdecl* _temp3039; struct
Cyc_List_List* _temp3041; struct _tagged_string* _temp3043; struct Cyc_List_List*
_temp3045; struct _tuple0* _temp3047; struct Cyc_List_List* _temp3049; _LL2991:
if(*(( int*) _temp2989) == Cyc_Absyn_Var_d){ _LL3016: _temp3015=(( struct Cyc_Absyn_Var_d_struct*)
_temp2989)->f1; goto _LL2992;} else{ goto _LL2993;} _LL2993: if(*(( int*)
_temp2989) == Cyc_Absyn_Fn_d){ _LL3018: _temp3017=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2989)->f1; goto _LL2994;} else{ goto _LL2995;} _LL2995: if(*(( int*)
_temp2989) == Cyc_Absyn_Let_d){ _LL3028: _temp3027=(( struct Cyc_Absyn_Let_d_struct*)
_temp2989)->f1; goto _LL3026; _LL3026: _temp3025=(( struct Cyc_Absyn_Let_d_struct*)
_temp2989)->f2; goto _LL3024; _LL3024: _temp3023=(( struct Cyc_Absyn_Let_d_struct*)
_temp2989)->f3; goto _LL3022; _LL3022: _temp3021=(( struct Cyc_Absyn_Let_d_struct*)
_temp2989)->f4; goto _LL3020; _LL3020: _temp3019=(( struct Cyc_Absyn_Let_d_struct*)
_temp2989)->f5; goto _LL2996;} else{ goto _LL2997;} _LL2997: if(*(( int*)
_temp2989) == Cyc_Absyn_Letv_d){ _LL3030: _temp3029=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2989)->f1; goto _LL2998;} else{ goto _LL2999;} _LL2999: if(*(( int*)
_temp2989) == Cyc_Absyn_Struct_d){ _LL3032: _temp3031=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2989)->f1; goto _LL3000;} else{ goto _LL3001;} _LL3001: if(*(( int*)
_temp2989) == Cyc_Absyn_Union_d){ _LL3034: _temp3033=(( struct Cyc_Absyn_Union_d_struct*)
_temp2989)->f1; goto _LL3002;} else{ goto _LL3003;} _LL3003: if(*(( int*)
_temp2989) == Cyc_Absyn_Tunion_d){ _LL3036: _temp3035=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2989)->f1; goto _LL3004;} else{ goto _LL3005;} _LL3005: if(*(( int*)
_temp2989) == Cyc_Absyn_Enum_d){ _LL3038: _temp3037=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2989)->f1; goto _LL3006;} else{ goto _LL3007;} _LL3007: if(*(( int*)
_temp2989) == Cyc_Absyn_Typedef_d){ _LL3040: _temp3039=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2989)->f1; goto _LL3008;} else{ goto _LL3009;} _LL3009: if(*(( int*)
_temp2989) == Cyc_Absyn_Namespace_d){ _LL3044: _temp3043=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2989)->f1; goto _LL3042; _LL3042: _temp3041=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2989)->f2; goto _LL3010;} else{ goto _LL3011;} _LL3011: if(*(( int*)
_temp2989) == Cyc_Absyn_Using_d){ _LL3048: _temp3047=(( struct Cyc_Absyn_Using_d_struct*)
_temp2989)->f1; goto _LL3046; _LL3046: _temp3045=(( struct Cyc_Absyn_Using_d_struct*)
_temp2989)->f2; goto _LL3012;} else{ goto _LL3013;} _LL3013: if(*(( int*)
_temp2989) == Cyc_Absyn_ExternC_d){ _LL3050: _temp3049=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2989)->f1; goto _LL3014;} else{ goto _LL2990;} _LL2992: { struct _tuple0*
_temp3051= _temp3015->name; goto _LL3052; _LL3052: if(( void*) _temp3015->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3051=({ struct _tuple0* _temp3053=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3053->f1= Cyc_Toc_abs_ns;
_temp3053->f2=(* _temp3051).f2; _temp3053;});} if( _temp3015->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null( _temp3015->initializer));}
if( ! nv->toplevel){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp3054=( unsigned char*)"decls_to_c: not at toplevel!"; struct
_tagged_string _temp3055; _temp3055.curr= _temp3054; _temp3055.base= _temp3054;
_temp3055.last_plus_one= _temp3054 + 29; _temp3055;}));} nv= Cyc_Toc_add_varmap(
nv, _temp3015->name, Cyc_Absyn_varb_exp( _temp3051,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp3056=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp3056[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp3057; _temp3057.tag= Cyc_Absyn_Global_b;
_temp3057.f1= _temp3015; _temp3057;}); _temp3056;}), 0)); _temp3015->name=
_temp3051;( void*)( _temp3015->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp3015->sc));(
void*)( _temp3015->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp3015->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3058=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3058->hd=( void*) d; _temp3058->tl=
Cyc_Toc_result_decls; _temp3058;}); goto _LL2990;} _LL2994: { struct _tuple0*
_temp3059= _temp3017->name; goto _LL3060; _LL3060: if(( void*) _temp3017->sc ==(
void*) Cyc_Absyn_ExternC){ _temp3059=({ struct _tuple0* _temp3061=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp3061->f1= Cyc_Toc_abs_ns;
_temp3061->f2=(* _temp3059).f2; _temp3061;});} nv= Cyc_Toc_add_varmap( nv,
_temp3017->name, Cyc_Absyn_var_exp( _temp3059, 0)); _temp3017->name= _temp3059;
Cyc_Toc_fndecl_to_c( nv, _temp3017); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3062=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3062->hd=( void*) d; _temp3062->tl= Cyc_Toc_result_decls; _temp3062;});
goto _LL2990;} _LL2996:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
unsigned char* _temp3063=( unsigned char*)"letdecl"; struct _tagged_string
_temp3064; _temp3064.curr= _temp3063; _temp3064.base= _temp3063; _temp3064.last_plus_one=
_temp3063 + 8; _temp3064;})); goto _LL2990; _LL2998:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ unsigned char* _temp3065=(
unsigned char*)"letdecl"; struct _tagged_string _temp3066; _temp3066.curr=
_temp3065; _temp3066.base= _temp3065; _temp3066.last_plus_one= _temp3065 + 8;
_temp3066;})); goto _LL2990; _LL3000: Cyc_Toc_structdecl_to_c( _temp3031); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3067=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3067->hd=( void*) d; _temp3067->tl= Cyc_Toc_result_decls;
_temp3067;}); goto _LL2990; _LL3002: Cyc_Toc_uniondecl_to_c( _temp3033); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp3068=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3068->hd=( void*) d; _temp3068->tl= Cyc_Toc_result_decls;
_temp3068;}); goto _LL2990; _LL3004: if( _temp3035->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp3035);} else{ Cyc_Toc_tuniondecl_to_c( _temp3035);} goto _LL2990; _LL3006:
Cyc_Toc_enumdecl_to_c( nv, _temp3037); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp3069=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3069->hd=( void*) d; _temp3069->tl= Cyc_Toc_result_decls; _temp3069;});
goto _LL2990; _LL3008: _temp3039->name= _temp3039->name; _temp3039->tvs= 0;(
void*)( _temp3039->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp3039->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp3070=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3070->hd=( void*) d; _temp3070->tl=
Cyc_Toc_result_decls; _temp3070;}); goto _LL2990; _LL3010: _temp3045= _temp3041;
goto _LL3012; _LL3012: _temp3049= _temp3045; goto _LL3014; _LL3014: nv= Cyc_Toc_decls_to_c(
nv, _temp3049, top); goto _LL2990; _LL2990:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far= 0; Cyc_Toc_tunions_so_far=
0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(), ds,
1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}