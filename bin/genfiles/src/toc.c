 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr;
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern unsigned char
Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u];
struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern struct
Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern struct Cyc_List_List*
Cyc_List_rlist( struct _RegionHandle*, struct _tagged_arr); extern int Cyc_List_length(
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
struct _tagged_arr arr); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strconcat( struct
_tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_String_implode(
struct Cyc_List_List* c); struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty(
int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct
Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set* s,
void* elt); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key);
extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void*
key); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; extern struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*); static const int Cyc_Position_Lex= 0; static
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x); extern
void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_bounds_one; extern void*
Cyc_Absyn_star_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void*
Cyc_Absyn_cstar_typ( void* t, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ();
extern void* Cyc_Absyn_strct( struct _tagged_arr* name); extern void* Cyc_Absyn_strctq(
struct _tuple0* name); extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_arr f, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_arr s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*, struct Cyc_List_List*
es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*);
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
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt( struct _tagged_arr* v,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp*
init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl( struct _tuple0*
x, void* t, struct Cyc_Absyn_Exp* init); extern int Cyc_Absyn_is_lvalue( struct
Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field(
struct Cyc_List_List*, struct _tagged_arr*); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int
qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_arr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*); extern struct _tagged_arr Cyc_Absynpp_prim2string( void* p);
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
struct _tagged_arr); extern void* Cyc_Tcutil_typ_kind( void* t); extern void*
Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify( void*, void*);
extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*);
struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern void* Cyc_Tcutil_snd_tqt(
struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_arr s, int i); extern struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds); extern struct _tuple0* Cyc_Toc_temp_var(); static struct Cyc_List_List*
Cyc_Toc_result_decls= 0; static struct Cyc_List_List* Cyc_Toc_tuple_types= 0;
static int Cyc_Toc_tuple_type_counter= 0; static int Cyc_Toc_temp_var_counter= 0;
static int Cyc_Toc_fresh_label_counter= 0; unsigned char Cyc_Toc_Toc_Unimplemented[
22u]="\000\000\000\000Toc_Unimplemented"; unsigned char Cyc_Toc_Toc_Impossible[
19u]="\000\000\000\000Toc_Impossible"; static void* Cyc_Toc_unimp( struct
_tagged_arr s){({ struct _tagged_arr _temp0= s; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Toc_Toc_Unimplemented);}
static void* Cyc_Toc_toc_impos( struct _tagged_arr s){({ struct _tagged_arr
_temp1= s; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s\n", _temp1.last_plus_one
- _temp1.curr, _temp1.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Toc_Toc_Impossible);} unsigned char
Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error"; static struct Cyc_Absyn_Rel_n_struct
Cyc_Toc_rel_ns_value={ 0u, 0}; static void* Cyc_Toc_rel_ns=( void*)& Cyc_Toc_rel_ns_value;
static unsigned char _temp3[ 5u]="curr"; static struct _tagged_arr Cyc_Toc_curr_string={
_temp3, _temp3, _temp3 + 5u}; static struct _tagged_arr* Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static unsigned char _temp4[ 14u]="last_plus_one"; static struct _tagged_arr Cyc_Toc_last_plus_one_string={
_temp4, _temp4, _temp4 + 14u}; static struct _tagged_arr* Cyc_Toc_last_plus_one_sp=&
Cyc_Toc_last_plus_one_string; static unsigned char _temp5[ 5u]="base"; static
struct _tagged_arr Cyc_Toc_base_string={ _temp5, _temp5, _temp5 + 5u}; static
struct _tagged_arr* Cyc_Toc_base_sp=& Cyc_Toc_base_string; static unsigned char
_temp6[ 8u]="xtunion"; static struct _tagged_arr Cyc_Toc_xtunion_string={ _temp6,
_temp6, _temp6 + 8u}; static struct _tagged_arr* Cyc_Toc_xtunion_sp=& Cyc_Toc_xtunion_string;
static unsigned char _temp7[ 4u]="tag"; static struct _tagged_arr Cyc_Toc_tag_string={
_temp7, _temp7, _temp7 + 4u}; static struct _tagged_arr* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static unsigned char _temp8[ 14u]="_handler_cons"; static struct _tagged_arr Cyc_Toc__handler_cons_string={
_temp8, _temp8, _temp8 + 14u}; static struct _tagged_arr* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static unsigned char _temp9[ 13u]="_push_region";
static struct _tagged_arr Cyc_Toc__push_region_string={ _temp9, _temp9, _temp9 +
13u}; static struct _tagged_arr* Cyc_Toc__push_region_sp=& Cyc_Toc__push_region_string;
static unsigned char _temp10[ 12u]="_pop_region"; static struct _tagged_arr Cyc_Toc__pop_region_string={
_temp10, _temp10, _temp10 + 12u}; static struct _tagged_arr* Cyc_Toc__pop_region_sp=&
Cyc_Toc__pop_region_string; static unsigned char _temp11[ 8u]="handler"; static
struct _tagged_arr Cyc_Toc_handler_string={ _temp11, _temp11, _temp11 + 8u};
static struct _tagged_arr* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static
unsigned char _temp12[ 14u]="_RegionHandle"; static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_temp12, _temp12, _temp12 + 14u}; static struct _tagged_arr* Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string; static unsigned char _temp13[ 12u]="_new_region";
static struct _tagged_arr Cyc_Toc__new_region_string={ _temp13, _temp13, _temp13
+ 12u}; static struct _tagged_arr* Cyc_Toc__new_region_sp=& Cyc_Toc__new_region_string;
static unsigned char _temp14[ 7u]="_throw"; static struct _tagged_arr Cyc_Toc__throw_string={
_temp14, _temp14, _temp14 + 7u}; static struct _tuple0 Cyc_Toc__throw_pr={( void*)&
Cyc_Toc_rel_ns_value,& Cyc_Toc__throw_string}; static struct _tuple0* Cyc_Toc__throw_qv=&
Cyc_Toc__throw_pr; static unsigned char _temp15[ 7u]="setjmp"; static struct
_tagged_arr Cyc_Toc_setjmp_string={ _temp15, _temp15, _temp15 + 7u}; static
struct _tuple0 Cyc_Toc_setjmp_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc_setjmp_string};
static struct _tuple0* Cyc_Toc_setjmp_qv=& Cyc_Toc_setjmp_pr; static
unsigned char _temp16[ 14u]="_push_handler"; static struct _tagged_arr Cyc_Toc__push_handler_string={
_temp16, _temp16, _temp16 + 14u}; static struct _tuple0 Cyc_Toc__push_handler_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__push_handler_string}; static struct
_tuple0* Cyc_Toc__push_handler_qv=& Cyc_Toc__push_handler_pr; static
unsigned char _temp17[ 13u]="_pop_handler"; static struct _tagged_arr Cyc_Toc__pop_handler_string={
_temp17, _temp17, _temp17 + 13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__pop_handler_string}; static struct
_tuple0* Cyc_Toc__pop_handler_qv=& Cyc_Toc__pop_handler_pr; static unsigned char
_temp18[ 12u]="_exn_thrown"; static struct _tagged_arr Cyc_Toc__exn_thrown_string={
_temp18, _temp18, _temp18 + 12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__exn_thrown_string}; static struct
_tuple0* Cyc_Toc__exn_thrown_qv=& Cyc_Toc__exn_thrown_pr; static unsigned char
_temp19[ 14u]="_npop_handler"; static struct _tagged_arr Cyc_Toc__npop_handler_string={
_temp19, _temp19, _temp19 + 14u}; static struct _tuple0 Cyc_Toc__npop_handler_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__npop_handler_string}; static struct
_tuple0* Cyc_Toc__npop_handler_qv=& Cyc_Toc__npop_handler_pr; static
unsigned char _temp20[ 12u]="_check_null"; static struct _tagged_arr Cyc_Toc__check_null_string={
_temp20, _temp20, _temp20 + 12u}; static struct _tuple0 Cyc_Toc__check_null_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__check_null_string}; static struct
_tuple0* Cyc_Toc__check_null_qv=& Cyc_Toc__check_null_pr; static unsigned char
_temp21[ 28u]="_check_known_subscript_null"; static struct _tagged_arr Cyc_Toc__check_known_subscript_null_string={
_temp21, _temp21, _temp21 + 28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__check_known_subscript_null_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_null_qv=& Cyc_Toc__check_known_subscript_null_pr;
static unsigned char _temp22[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_string={ _temp22,
_temp22, _temp22 + 31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__check_known_subscript_notnull_string};
static struct _tuple0* Cyc_Toc__check_known_subscript_notnull_qv=& Cyc_Toc__check_known_subscript_notnull_pr;
static unsigned char _temp23[ 25u]="_check_unknown_subscript"; static struct
_tagged_arr Cyc_Toc__check_unknown_subscript_string={ _temp23, _temp23, _temp23
+ 25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__check_unknown_subscript_string}; static struct _tuple0* Cyc_Toc__check_unknown_subscript_qv=&
Cyc_Toc__check_unknown_subscript_pr; static unsigned char _temp24[ 12u]="_tagged_arr";
static struct _tagged_arr Cyc_Toc__tagged_arr_string={ _temp24, _temp24, _temp24
+ 12u}; static struct _tuple0 Cyc_Toc__tagged_arr_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__tagged_arr_string}; static struct _tuple0* Cyc_Toc__tagged_arr_qv=& Cyc_Toc__tagged_arr_pr;
static unsigned char _temp25[ 9u]="_tag_arr"; static struct _tagged_arr Cyc_Toc__tag_arr_string={
_temp25, _temp25, _temp25 + 9u}; static struct _tuple0 Cyc_Toc__tag_arr_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__tag_arr_string}; static struct _tuple0*
Cyc_Toc__tag_arr_qv=& Cyc_Toc__tag_arr_pr; static unsigned char _temp26[ 14u]="_init_tag_arr";
static struct _tagged_arr Cyc_Toc__init_tag_arr_string={ _temp26, _temp26,
_temp26 + 14u}; static struct _tuple0 Cyc_Toc__init_tag_arr_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__init_tag_arr_string}; static struct _tuple0* Cyc_Toc__init_tag_arr_qv=&
Cyc_Toc__init_tag_arr_pr; static unsigned char _temp27[ 11u]="_untag_arr";
static struct _tagged_arr Cyc_Toc__untag_arr_string={ _temp27, _temp27, _temp27
+ 11u}; static struct _tuple0 Cyc_Toc__untag_arr_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__untag_arr_string}; static struct _tuple0* Cyc_Toc__untag_arr_qv=& Cyc_Toc__untag_arr_pr;
static unsigned char _temp28[ 14u]="_get_arr_size"; static struct _tagged_arr
Cyc_Toc__get_arr_size_string={ _temp28, _temp28, _temp28 + 14u}; static struct
_tuple0 Cyc_Toc__get_arr_size_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__get_arr_size_string};
static struct _tuple0* Cyc_Toc__get_arr_size_qv=& Cyc_Toc__get_arr_size_pr;
static unsigned char _temp29[ 17u]="_tagged_arr_plus"; static struct _tagged_arr
Cyc_Toc__tagged_arr_plus_string={ _temp29, _temp29, _temp29 + 17u}; static
struct _tuple0 Cyc_Toc__tagged_arr_plus_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__tagged_arr_plus_string};
static struct _tuple0* Cyc_Toc__tagged_arr_plus_qv=& Cyc_Toc__tagged_arr_plus_pr;
static unsigned char _temp30[ 25u]="_tagged_arr_inplace_plus"; static struct
_tagged_arr Cyc_Toc__tagged_arr_inplace_plus_string={ _temp30, _temp30, _temp30
+ 25u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__tagged_arr_inplace_plus_string}; static struct _tuple0* Cyc_Toc__tagged_arr_inplace_plus_qv=&
Cyc_Toc__tagged_arr_inplace_plus_pr; static unsigned char _temp31[ 30u]="_tagged_arr_inplace_plus_post";
static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_string={ _temp31,
_temp31, _temp31 + 30u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__tagged_arr_inplace_plus_post_string};
static struct _tuple0* Cyc_Toc__tagged_arr_inplace_plus_post_qv=& Cyc_Toc__tagged_arr_inplace_plus_post_pr;
static unsigned char _temp32[ 10u]="GC_malloc"; static struct _tagged_arr Cyc_Toc_GC_malloc_string={
_temp32, _temp32, _temp32 + 10u}; static struct _tuple0 Cyc_Toc_GC_malloc_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc_GC_malloc_string}; static struct _tuple0*
Cyc_Toc_GC_malloc_qv=& Cyc_Toc_GC_malloc_pr; static unsigned char _temp33[ 17u]="GC_malloc_atomic";
static struct _tagged_arr Cyc_Toc_GC_malloc_atomic_string={ _temp33, _temp33,
_temp33 + 17u}; static struct _tuple0 Cyc_Toc_GC_malloc_atomic_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc_GC_malloc_atomic_string}; static struct _tuple0* Cyc_Toc_GC_malloc_atomic_qv=&
Cyc_Toc_GC_malloc_atomic_pr; static unsigned char _temp34[ 15u]="_region_malloc";
static struct _tagged_arr Cyc_Toc__region_malloc_string={ _temp34, _temp34,
_temp34 + 15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__region_malloc_string}; static struct _tuple0* Cyc_Toc__region_malloc_qv=&
Cyc_Toc__region_malloc_pr; static unsigned char _temp35[ 15u]="_sfile_to_file";
static struct _tagged_arr Cyc_Toc__sfile_to_file_string={ _temp35, _temp35,
_temp35 + 15u}; static struct _tuple0 Cyc_Toc__sfile_to_file_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__sfile_to_file_string}; static struct _tuple0* Cyc_Toc__sfile_to_file_qv=&
Cyc_Toc__sfile_to_file_pr; static struct Cyc_Absyn_StructType_struct Cyc_Toc_tagged_arr_typ_v={
9u,( struct _tuple0*)& Cyc_Toc__tagged_arr_pr, 0, 0}; static void* Cyc_Toc_tagged_arr_typ=(
void*)& Cyc_Toc_tagged_arr_typ_v; static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={
0, 0, 0}; struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; }
; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp37=( void*) e->r;
void* _temp63; unsigned char _temp65; void* _temp67; void* _temp69; short
_temp71; void* _temp73; void* _temp75; int _temp77; void* _temp79; void* _temp81;
long long _temp83; void* _temp85; void* _temp87; struct Cyc_Absyn_Exp* _temp89;
void* _temp91; struct Cyc_List_List* _temp93; struct Cyc_List_List* _temp95;
struct Cyc_Absyn_Structdecl* _temp97; struct Cyc_List_List* _temp99; struct Cyc_Core_Opt*
_temp101; struct _tuple0* _temp103; struct Cyc_List_List* _temp105; struct
_tuple1* _temp107; struct Cyc_List_List* _temp109; struct Cyc_Core_Opt* _temp111;
_LL39: if(*(( int*) _temp37) == Cyc_Absyn_Const_e){ _LL64: _temp63=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp37)->f1; if(( unsigned int) _temp63 > 1u?*((
int*) _temp63) == Cyc_Absyn_Char_c: 0){ _LL68: _temp67=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp63)->f1; goto _LL66; _LL66: _temp65=(( struct Cyc_Absyn_Char_c_struct*)
_temp63)->f2; goto _LL40;} else{ goto _LL41;}} else{ goto _LL41;} _LL41: if(*((
int*) _temp37) == Cyc_Absyn_Const_e){ _LL70: _temp69=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp37)->f1; if(( unsigned int) _temp69 > 1u?*(( int*) _temp69) == Cyc_Absyn_Short_c:
0){ _LL74: _temp73=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp69)->f1;
goto _LL72; _LL72: _temp71=(( struct Cyc_Absyn_Short_c_struct*) _temp69)->f2;
goto _LL42;} else{ goto _LL43;}} else{ goto _LL43;} _LL43: if(*(( int*) _temp37)
== Cyc_Absyn_Const_e){ _LL76: _temp75=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp37)->f1; if(( unsigned int) _temp75 > 1u?*(( int*) _temp75) == Cyc_Absyn_Int_c:
0){ _LL80: _temp79=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp75)->f1; goto
_LL78; _LL78: _temp77=(( struct Cyc_Absyn_Int_c_struct*) _temp75)->f2; goto
_LL44;} else{ goto _LL45;}} else{ goto _LL45;} _LL45: if(*(( int*) _temp37) ==
Cyc_Absyn_Const_e){ _LL82: _temp81=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp37)->f1; if(( unsigned int) _temp81 > 1u?*(( int*) _temp81) == Cyc_Absyn_LongLong_c:
0){ _LL86: _temp85=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp81)->f1;
goto _LL84; _LL84: _temp83=(( struct Cyc_Absyn_LongLong_c_struct*) _temp81)->f2;
goto _LL46;} else{ goto _LL47;}} else{ goto _LL47;} _LL47: if(*(( int*) _temp37)
== Cyc_Absyn_Const_e){ _LL88: _temp87=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp37)->f1; if( _temp87 ==( void*) Cyc_Absyn_Null_c){ goto _LL48;} else{ goto
_LL49;}} else{ goto _LL49;} _LL49: if(*(( int*) _temp37) == Cyc_Absyn_Cast_e){
_LL92: _temp91=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp37)->f1; goto
_LL90; _LL90: _temp89=(( struct Cyc_Absyn_Cast_e_struct*) _temp37)->f2; goto
_LL50;} else{ goto _LL51;} _LL51: if(*(( int*) _temp37) == Cyc_Absyn_Tuple_e){
_LL94: _temp93=(( struct Cyc_Absyn_Tuple_e_struct*) _temp37)->f1; goto _LL52;}
else{ goto _LL53;} _LL53: if(*(( int*) _temp37) == Cyc_Absyn_Array_e){ _LL96:
_temp95=(( struct Cyc_Absyn_Array_e_struct*) _temp37)->f1; goto _LL54;} else{
goto _LL55;} _LL55: if(*(( int*) _temp37) == Cyc_Absyn_Struct_e){ _LL104:
_temp103=(( struct Cyc_Absyn_Struct_e_struct*) _temp37)->f1; goto _LL102; _LL102:
_temp101=(( struct Cyc_Absyn_Struct_e_struct*) _temp37)->f2; goto _LL100; _LL100:
_temp99=(( struct Cyc_Absyn_Struct_e_struct*) _temp37)->f3; goto _LL98; _LL98:
_temp97=(( struct Cyc_Absyn_Struct_e_struct*) _temp37)->f4; goto _LL56;} else{
goto _LL57;} _LL57: if(*(( int*) _temp37) == Cyc_Absyn_CompoundLit_e){ _LL108:
_temp107=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp37)->f1; goto _LL106;
_LL106: _temp105=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp37)->f2; goto
_LL58;} else{ goto _LL59;} _LL59: if(*(( int*) _temp37) == Cyc_Absyn_UnresolvedMem_e){
_LL112: _temp111=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp37)->f1; goto
_LL110; _LL110: _temp109=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp37)->f2;
goto _LL60;} else{ goto _LL61;} _LL61: goto _LL62; _LL40: return _temp65 =='\000';
_LL42: return _temp71 == 0; _LL44: return _temp77 == 0; _LL46: return _temp83 ==
0; _LL48: return 1; _LL50: return Cyc_Toc_is_zero( _temp89); _LL52: return(( int(*)(
int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp93); _LL54: _temp99= _temp95; goto _LL56; _LL56: _temp105=
_temp99; goto _LL58; _LL58: _temp109= _temp105; goto _LL60; _LL60: for( 0;
_temp109 != 0; _temp109=(( struct Cyc_List_List*) _check_null( _temp109))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp109))->hd)).f2)){ return 0;}} return 1; _LL62: return 0; _LL38:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp113= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp119; struct Cyc_Absyn_Conref* _temp121; struct Cyc_Absyn_Tqual
_temp123; struct Cyc_Absyn_Conref* _temp125; void* _temp127; void* _temp129;
_LL115: if(( unsigned int) _temp113 > 4u?*(( int*) _temp113) == Cyc_Absyn_PointerType:
0){ _LL120: _temp119=(( struct Cyc_Absyn_PointerType_struct*) _temp113)->f1;
_LL130: _temp129=( void*) _temp119.elt_typ; goto _LL128; _LL128: _temp127=( void*)
_temp119.rgn_typ; goto _LL126; _LL126: _temp125= _temp119.nullable; goto _LL124;
_LL124: _temp123= _temp119.tq; goto _LL122; _LL122: _temp121= _temp119.bounds;
goto _LL116;} else{ goto _LL117;} _LL117: goto _LL118; _LL116: return(( int(*)(
int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp125); _LL118:((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("is_nullable",
sizeof( unsigned char), 12u)); return 0; _LL114:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag(
struct _tuple0* x, struct _tagged_arr tag){ return({ struct _tuple0* _temp131=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp131->f1=(* x).f1;
_temp131->f2=({ struct _tagged_arr* _temp132=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp132[ 0]=( struct _tagged_arr) Cyc_String_strconcat(*(*
x).f2, tag); _temp132;}); _temp131;});} static void Cyc_Toc_add_tuple_decl(
struct _tagged_arr* x, struct Cyc_List_List* ts){ struct Cyc_List_List* _temp133=
0; goto _LL134; _LL134:{ int i= 1; for( 0; ts != 0;( ts=(( struct Cyc_List_List*)
_check_null( ts))->tl, i ++)){ _temp133=({ struct Cyc_List_List* _temp135=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp135->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp136=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp136->name=({ struct
_tagged_arr* _temp137=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp137[ 0]=( struct _tagged_arr) xprintf("f%d", i);
_temp137;}); _temp136->tq= Cyc_Toc_mt_tq; _temp136->type=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp136->width= 0; _temp136->attributes=
0; _temp136;}); _temp135->tl= _temp133; _temp135;});}} _temp133=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp133);{ struct Cyc_Absyn_Structdecl*
_temp142=({ struct Cyc_Absyn_Structdecl* _temp138=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp138->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp138->name=({ struct Cyc_Core_Opt* _temp140=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp140->v=( void*)({ struct _tuple0*
_temp141=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp141->f1=
Cyc_Toc_rel_ns; _temp141->f2= x; _temp141;}); _temp140;}); _temp138->tvs= 0;
_temp138->fields=({ struct Cyc_Core_Opt* _temp139=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp139->v=( void*) _temp133;
_temp139;}); _temp138->attributes= 0; _temp138;}); goto _LL143; _LL143: Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp144=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp144->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp145=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp145[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp146; _temp146.tag= Cyc_Absyn_Struct_d; _temp146.f1=
_temp142; _temp146;}); _temp145;}), 0); _temp144->tl= Cyc_Toc_result_decls;
_temp144;});}} struct _tuple5{ struct _tagged_arr* f1; struct Cyc_List_List* f2;
} ; static struct _tagged_arr* Cyc_Toc_add_tuple_type( struct Cyc_List_List*
tqs0){{ struct Cyc_List_List* _temp147= Cyc_Toc_tuple_types; goto _LL148; _LL148:
for( 0; _temp147 != 0; _temp147=(( struct Cyc_List_List*) _check_null( _temp147))->tl){
struct _tuple5 _temp151; struct Cyc_List_List* _temp152; struct _tagged_arr*
_temp154; struct _tuple5* _temp149=( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp147))->hd; _temp151=* _temp149; _LL155: _temp154= _temp151.f1;
goto _LL153; _LL153: _temp152= _temp151.f2; goto _LL150; _LL150: { int okay= 1;
struct Cyc_List_List* _temp156= tqs0; goto _LL157; _LL157: for( 0; _temp156 != 0?
_temp152 != 0: 0;( _temp156=(( struct Cyc_List_List*) _check_null( _temp156))->tl,
_temp152=(( struct Cyc_List_List*) _check_null( _temp152))->tl)){ if( ! Cyc_Tcutil_unify((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp156))->hd)).f2,(
void*)(( struct Cyc_List_List*) _check_null( _temp152))->hd)){ okay= 0; break;}}
if(( ! okay? 1: _temp156 != 0)? 1: _temp152 != 0){ continue;} return _temp154;}}}{
struct _tagged_arr* x=({ struct _tagged_arr* _temp160=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp160[ 0]=( struct _tagged_arr)
xprintf("_tuple%d", Cyc_Toc_tuple_type_counter ++); _temp160;}); struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); Cyc_Toc_add_tuple_decl( x, ts);
Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp158=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp158->hd=( void*)({ struct
_tuple5* _temp159=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp159->f1= x; _temp159->f2= ts; _temp159;}); _temp158->tl= Cyc_Toc_tuple_types;
_temp158;}); return x;}} struct _tuple0* Cyc_Toc_temp_var(){ return({ struct
_tuple0* _temp161=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp161->f1=( void*) Cyc_Absyn_Loc_n; _temp161->f2=({ struct _tagged_arr*
_temp162=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp162[ 0]=( struct _tagged_arr) xprintf("_temp%d", Cyc_Toc_temp_var_counter
++); _temp162;}); _temp161;});} static struct _tagged_arr* Cyc_Toc_fresh_label(){
return({ struct _tagged_arr* _temp163=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)); _temp163[ 0]=( struct _tagged_arr) xprintf("_LL%d", Cyc_Toc_temp_var_counter
++); _temp163;});} static void* Cyc_Toc_typ_to_c( void* t); static struct
_tuple1* Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp166; struct Cyc_Absyn_Tqual
_temp168; struct Cyc_Core_Opt* _temp170; struct _tuple1 _temp164=* a; _LL171:
_temp170= _temp164.f1; goto _LL169; _LL169: _temp168= _temp164.f2; goto _LL167;
_LL167: _temp166= _temp164.f3; goto _LL165; _LL165: return({ struct _tuple1*
_temp172=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp172->f1=
_temp170; _temp172->f2= _temp168; _temp172->f3= Cyc_Toc_typ_to_c( _temp166);
_temp172;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp175; struct Cyc_Absyn_Tqual _temp177; struct _tuple3 _temp173=* x;
_LL178: _temp177= _temp173.f1; goto _LL176; _LL176: _temp175= _temp173.f2; goto
_LL174; _LL174: return({ struct _tuple3* _temp179=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp179->f1= _temp177; _temp179->f2= Cyc_Toc_typ_to_c(
_temp175); _temp179;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp180= t; struct Cyc_Absyn_Exp* _temp188; struct Cyc_Absyn_Tqual _temp190;
void* _temp192; struct Cyc_Core_Opt* _temp194; int _temp196; struct Cyc_Core_Opt*
_temp198; struct Cyc_Core_Opt _temp200; void* _temp201; struct Cyc_Core_Opt*
_temp203; _LL182: if(( unsigned int) _temp180 > 4u?*(( int*) _temp180) == Cyc_Absyn_ArrayType:
0){ _LL193: _temp192=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp180)->f1;
goto _LL191; _LL191: _temp190=(( struct Cyc_Absyn_ArrayType_struct*) _temp180)->f2;
goto _LL189; _LL189: _temp188=(( struct Cyc_Absyn_ArrayType_struct*) _temp180)->f3;
goto _LL183;} else{ goto _LL184;} _LL184: if(( unsigned int) _temp180 > 4u?*((
int*) _temp180) == Cyc_Absyn_Evar: 0){ _LL204: _temp203=(( struct Cyc_Absyn_Evar_struct*)
_temp180)->f1; goto _LL199; _LL199: _temp198=(( struct Cyc_Absyn_Evar_struct*)
_temp180)->f2; if( _temp198 == 0){ goto _LL186;} else{ _temp200=* _temp198;
_LL202: _temp201=( void*) _temp200.v; goto _LL197;} _LL197: _temp196=(( struct
Cyc_Absyn_Evar_struct*) _temp180)->f3; goto _LL195; _LL195: _temp194=(( struct
Cyc_Absyn_Evar_struct*) _temp180)->f4; goto _LL185;} else{ goto _LL186;} _LL186:
goto _LL187; _LL183: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp205=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp205[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp206; _temp206.tag= Cyc_Absyn_ArrayType;
_temp206.f1=( void*) Cyc_Toc_typ_to_c_array( _temp192); _temp206.f2= _temp190;
_temp206.f3= _temp188; _temp206;}); _temp205;}); _LL185: return Cyc_Toc_typ_to_c_array(
_temp201); _LL187: return Cyc_Toc_typ_to_c( t); _LL181:;} static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c( struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield*
_temp207=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp207->name= f->name; _temp207->tq= f->tq; _temp207->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp207->width= f->width; _temp207->attributes= f->attributes;
_temp207;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp208= t; struct
Cyc_Core_Opt* _temp260; int _temp262; struct Cyc_Core_Opt* _temp264; struct Cyc_Core_Opt*
_temp266; struct Cyc_Core_Opt* _temp268; int _temp270; struct Cyc_Core_Opt*
_temp272; struct Cyc_Core_Opt _temp274; void* _temp275; struct Cyc_Core_Opt*
_temp277; struct Cyc_Absyn_Tvar* _temp279; struct Cyc_Absyn_TunionInfo _temp281;
void* _temp283; struct Cyc_List_List* _temp285; void* _temp287; struct Cyc_Absyn_Tuniondecl*
_temp289; struct Cyc_Absyn_TunionInfo _temp291; struct Cyc_Absyn_TunionFieldInfo
_temp293; struct Cyc_List_List* _temp295; void* _temp297; struct Cyc_Absyn_Tunionfield*
_temp299; struct Cyc_Absyn_Tuniondecl* _temp301; struct Cyc_Absyn_TunionFieldInfo
_temp303; struct Cyc_Absyn_PtrInfo _temp305; struct Cyc_Absyn_Conref* _temp307;
struct Cyc_Absyn_Tqual _temp309; struct Cyc_Absyn_Conref* _temp311; void*
_temp313; void* _temp315; void* _temp317; void* _temp319; struct Cyc_Absyn_Exp*
_temp321; struct Cyc_Absyn_Tqual _temp323; void* _temp325; struct Cyc_Absyn_FnInfo
_temp327; struct Cyc_List_List* _temp329; struct Cyc_List_List* _temp331; struct
Cyc_Absyn_VarargInfo* _temp333; int _temp335; struct Cyc_List_List* _temp337;
void* _temp339; struct Cyc_Core_Opt* _temp341; struct Cyc_List_List* _temp343;
struct Cyc_List_List* _temp345; struct Cyc_List_List* _temp347; struct Cyc_List_List*
_temp349; struct Cyc_Absyn_Uniondecl** _temp351; struct Cyc_List_List* _temp353;
struct _tuple0* _temp355; struct Cyc_Absyn_Structdecl** _temp357; struct Cyc_List_List*
_temp359; struct _tuple0* _temp361; struct Cyc_Absyn_Enumdecl* _temp363; struct
_tuple0* _temp365; struct Cyc_Core_Opt* _temp367; struct Cyc_List_List* _temp369;
struct _tuple0* _temp371; void* _temp373; void* _temp375; struct Cyc_List_List*
_temp377; _LL210: if( _temp208 ==( void*) Cyc_Absyn_VoidType){ goto _LL211;}
else{ goto _LL212;} _LL212: if(( unsigned int) _temp208 > 4u?*(( int*) _temp208)
== Cyc_Absyn_Evar: 0){ _LL267: _temp266=(( struct Cyc_Absyn_Evar_struct*)
_temp208)->f1; goto _LL265; _LL265: _temp264=(( struct Cyc_Absyn_Evar_struct*)
_temp208)->f2; if( _temp264 == 0){ goto _LL263;} else{ goto _LL214;} _LL263:
_temp262=(( struct Cyc_Absyn_Evar_struct*) _temp208)->f3; goto _LL261; _LL261:
_temp260=(( struct Cyc_Absyn_Evar_struct*) _temp208)->f4; goto _LL213;} else{
goto _LL214;} _LL214: if(( unsigned int) _temp208 > 4u?*(( int*) _temp208) ==
Cyc_Absyn_Evar: 0){ _LL278: _temp277=(( struct Cyc_Absyn_Evar_struct*) _temp208)->f1;
goto _LL273; _LL273: _temp272=(( struct Cyc_Absyn_Evar_struct*) _temp208)->f2;
if( _temp272 == 0){ goto _LL216;} else{ _temp274=* _temp272; _LL276: _temp275=(
void*) _temp274.v; goto _LL271;} _LL271: _temp270=(( struct Cyc_Absyn_Evar_struct*)
_temp208)->f3; goto _LL269; _LL269: _temp268=(( struct Cyc_Absyn_Evar_struct*)
_temp208)->f4; goto _LL215;} else{ goto _LL216;} _LL216: if(( unsigned int)
_temp208 > 4u?*(( int*) _temp208) == Cyc_Absyn_VarType: 0){ _LL280: _temp279=((
struct Cyc_Absyn_VarType_struct*) _temp208)->f1; goto _LL217;} else{ goto _LL218;}
_LL218: if(( unsigned int) _temp208 > 4u?*(( int*) _temp208) == Cyc_Absyn_TunionType:
0){ _LL282: _temp281=(( struct Cyc_Absyn_TunionType_struct*) _temp208)->f1;
_LL288: _temp287=( void*) _temp281.tunion_info; if(*(( int*) _temp287) == Cyc_Absyn_KnownTunion){
_LL290: _temp289=(( struct Cyc_Absyn_KnownTunion_struct*) _temp287)->f1; goto
_LL286;} else{ goto _LL220;} _LL286: _temp285= _temp281.targs; goto _LL284;
_LL284: _temp283=( void*) _temp281.rgn; goto _LL219;} else{ goto _LL220;} _LL220:
if(( unsigned int) _temp208 > 4u?*(( int*) _temp208) == Cyc_Absyn_TunionType: 0){
_LL292: _temp291=(( struct Cyc_Absyn_TunionType_struct*) _temp208)->f1; goto
_LL221;} else{ goto _LL222;} _LL222: if(( unsigned int) _temp208 > 4u?*(( int*)
_temp208) == Cyc_Absyn_TunionFieldType: 0){ _LL294: _temp293=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp208)->f1; _LL298: _temp297=( void*) _temp293.field_info; if(*(( int*)
_temp297) == Cyc_Absyn_KnownTunionfield){ _LL302: _temp301=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp297)->f1; goto _LL300; _LL300: _temp299=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp297)->f2; goto _LL296;} else{ goto _LL224;} _LL296: _temp295= _temp293.targs;
goto _LL223;} else{ goto _LL224;} _LL224: if(( unsigned int) _temp208 > 4u?*((
int*) _temp208) == Cyc_Absyn_TunionFieldType: 0){ _LL304: _temp303=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp208)->f1; goto _LL225;} else{ goto _LL226;} _LL226: if(( unsigned int)
_temp208 > 4u?*(( int*) _temp208) == Cyc_Absyn_PointerType: 0){ _LL306: _temp305=((
struct Cyc_Absyn_PointerType_struct*) _temp208)->f1; _LL316: _temp315=( void*)
_temp305.elt_typ; goto _LL314; _LL314: _temp313=( void*) _temp305.rgn_typ; goto
_LL312; _LL312: _temp311= _temp305.nullable; goto _LL310; _LL310: _temp309=
_temp305.tq; goto _LL308; _LL308: _temp307= _temp305.bounds; goto _LL227;} else{
goto _LL228;} _LL228: if(( unsigned int) _temp208 > 4u?*(( int*) _temp208) ==
Cyc_Absyn_IntType: 0){ _LL320: _temp319=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp208)->f1; goto _LL318; _LL318: _temp317=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp208)->f2; goto _LL229;} else{ goto _LL230;} _LL230: if( _temp208 ==( void*)
Cyc_Absyn_FloatType){ goto _LL231;} else{ goto _LL232;} _LL232: if( _temp208 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL233;} else{ goto _LL234;} _LL234: if((
unsigned int) _temp208 > 4u?*(( int*) _temp208) == Cyc_Absyn_ArrayType: 0){
_LL326: _temp325=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp208)->f1;
goto _LL324; _LL324: _temp323=(( struct Cyc_Absyn_ArrayType_struct*) _temp208)->f2;
goto _LL322; _LL322: _temp321=(( struct Cyc_Absyn_ArrayType_struct*) _temp208)->f3;
goto _LL235;} else{ goto _LL236;} _LL236: if(( unsigned int) _temp208 > 4u?*((
int*) _temp208) == Cyc_Absyn_FnType: 0){ _LL328: _temp327=(( struct Cyc_Absyn_FnType_struct*)
_temp208)->f1; _LL344: _temp343= _temp327.tvars; goto _LL342; _LL342: _temp341=
_temp327.effect; goto _LL340; _LL340: _temp339=( void*) _temp327.ret_typ; goto
_LL338; _LL338: _temp337= _temp327.args; goto _LL336; _LL336: _temp335= _temp327.c_varargs;
goto _LL334; _LL334: _temp333= _temp327.cyc_varargs; goto _LL332; _LL332:
_temp331= _temp327.rgn_po; goto _LL330; _LL330: _temp329= _temp327.attributes;
goto _LL237;} else{ goto _LL238;} _LL238: if(( unsigned int) _temp208 > 4u?*((
int*) _temp208) == Cyc_Absyn_TupleType: 0){ _LL346: _temp345=(( struct Cyc_Absyn_TupleType_struct*)
_temp208)->f1; goto _LL239;} else{ goto _LL240;} _LL240: if(( unsigned int)
_temp208 > 4u?*(( int*) _temp208) == Cyc_Absyn_AnonStructType: 0){ _LL348:
_temp347=(( struct Cyc_Absyn_AnonStructType_struct*) _temp208)->f1; goto _LL241;}
else{ goto _LL242;} _LL242: if(( unsigned int) _temp208 > 4u?*(( int*) _temp208)
== Cyc_Absyn_AnonUnionType: 0){ _LL350: _temp349=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp208)->f1; goto _LL243;} else{ goto _LL244;} _LL244: if(( unsigned int)
_temp208 > 4u?*(( int*) _temp208) == Cyc_Absyn_UnionType: 0){ _LL356: _temp355=((
struct Cyc_Absyn_UnionType_struct*) _temp208)->f1; goto _LL354; _LL354: _temp353=((
struct Cyc_Absyn_UnionType_struct*) _temp208)->f2; goto _LL352; _LL352: _temp351=((
struct Cyc_Absyn_UnionType_struct*) _temp208)->f3; goto _LL245;} else{ goto
_LL246;} _LL246: if(( unsigned int) _temp208 > 4u?*(( int*) _temp208) == Cyc_Absyn_StructType:
0){ _LL362: _temp361=(( struct Cyc_Absyn_StructType_struct*) _temp208)->f1; goto
_LL360; _LL360: _temp359=(( struct Cyc_Absyn_StructType_struct*) _temp208)->f2;
goto _LL358; _LL358: _temp357=(( struct Cyc_Absyn_StructType_struct*) _temp208)->f3;
goto _LL247;} else{ goto _LL248;} _LL248: if(( unsigned int) _temp208 > 4u?*((
int*) _temp208) == Cyc_Absyn_EnumType: 0){ _LL366: _temp365=(( struct Cyc_Absyn_EnumType_struct*)
_temp208)->f1; goto _LL364; _LL364: _temp363=(( struct Cyc_Absyn_EnumType_struct*)
_temp208)->f2; goto _LL249;} else{ goto _LL250;} _LL250: if(( unsigned int)
_temp208 > 4u?*(( int*) _temp208) == Cyc_Absyn_TypedefType: 0){ _LL372: _temp371=((
struct Cyc_Absyn_TypedefType_struct*) _temp208)->f1; goto _LL370; _LL370:
_temp369=(( struct Cyc_Absyn_TypedefType_struct*) _temp208)->f2; goto _LL368;
_LL368: _temp367=(( struct Cyc_Absyn_TypedefType_struct*) _temp208)->f3; goto
_LL251;} else{ goto _LL252;} _LL252: if(( unsigned int) _temp208 > 4u?*(( int*)
_temp208) == Cyc_Absyn_RgnHandleType: 0){ _LL374: _temp373=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp208)->f1; goto _LL253;} else{ goto _LL254;} _LL254: if( _temp208 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL255;} else{ goto _LL256;} _LL256: if(( unsigned int)
_temp208 > 4u?*(( int*) _temp208) == Cyc_Absyn_AccessEff: 0){ _LL376: _temp375=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp208)->f1; goto _LL257;} else{
goto _LL258;} _LL258: if(( unsigned int) _temp208 > 4u?*(( int*) _temp208) ==
Cyc_Absyn_JoinEff: 0){ _LL378: _temp377=(( struct Cyc_Absyn_JoinEff_struct*)
_temp208)->f1; goto _LL259;} else{ goto _LL209;} _LL211: return t; _LL213:
return Cyc_Absyn_sint_t; _LL215: return Cyc_Toc_typ_to_c( _temp275); _LL217:
return Cyc_Absyn_void_star_typ(); _LL219: return Cyc_Absyn_void_star_typ();
_LL221:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("unresolved TunionType", sizeof( unsigned char), 22u))); _LL223: if(
_temp299->typs == 0){ if( _temp301->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp299->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL225:( int) _throw((( void*(*)(
struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u))); _LL227: _temp315= Cyc_Toc_typ_to_c_array(
_temp315);{ void* _temp379=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp307))->v; void* _temp385; _LL381: if((
unsigned int) _temp379 > 1u?*(( int*) _temp379) == Cyc_Absyn_Eq_constr: 0){
_LL386: _temp385=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp379)->f1;
if( _temp385 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL382;} else{ goto _LL383;}}
else{ goto _LL383;} _LL383: goto _LL384; _LL382: return Cyc_Toc_tagged_arr_typ;
_LL384: return Cyc_Absyn_star_typ( _temp315,( void*) Cyc_Absyn_HeapRgn, _temp309);
_LL380:;} _LL229: return t; _LL231: return t; _LL233: return t; _LL235: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp387=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp387[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp388; _temp388.tag= Cyc_Absyn_ArrayType; _temp388.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp325); _temp388.f2= _temp323; _temp388.f3=
_temp321; _temp388;}); _temp387;}); _LL237: { struct Cyc_List_List* _temp389=
_temp329; goto _LL390; _LL390: { struct Cyc_List_List* _temp391= 0; goto _LL392;
_LL392: for( 0; _temp329 != 0; _temp329=(( struct Cyc_List_List*) _check_null(
_temp329))->tl){ void* _temp393=( void*)(( struct Cyc_List_List*) _check_null(
_temp329))->hd; _LL395: if( _temp393 ==( void*) Cyc_Absyn_Noreturn_att){ goto
_LL396;} else{ goto _LL397;} _LL397: if( _temp393 ==( void*) Cyc_Absyn_Const_att){
goto _LL398;} else{ goto _LL399;} _LL399: goto _LL400; _LL396: goto _LL398;
_LL398: continue; _LL400: _temp391=({ struct Cyc_List_List* _temp401=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp401->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( _temp329))->hd); _temp401->tl=
_temp391; _temp401;}); goto _LL394; _LL394:;}{ struct Cyc_List_List* _temp402=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp337); goto _LL403; _LL403: if(
_temp333 != 0){ void* _temp404= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp333))->type,( void*)(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp333))->rgn, Cyc_Toc_mt_tq)); goto
_LL405; _LL405: { struct _tuple1* _temp407=({ struct _tuple1* _temp406=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp406->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp333))->name; _temp406->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp333))->tq; _temp406->f3= _temp404; _temp406;}); goto _LL408;
_LL408: _temp402=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_imp_append)( _temp402,({ struct Cyc_List_List*
_temp409=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp409->hd=( void*) _temp407; _temp409->tl= 0; _temp409;}));}} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp410=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp410[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp411; _temp411.tag= Cyc_Absyn_FnType; _temp411.f1=({ struct Cyc_Absyn_FnInfo
_temp412; _temp412.tvars= 0; _temp412.effect= 0; _temp412.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp339); _temp412.args= _temp402; _temp412.c_varargs= _temp335; _temp412.cyc_varargs=
0; _temp412.rgn_po= 0; _temp412.attributes= _temp391; _temp412;}); _temp411;});
_temp410;});}}} _LL239: _temp345=(( struct Cyc_List_List*(*)( struct _tuple3*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp345);{ struct _tagged_arr* _temp413= Cyc_Toc_add_tuple_type( _temp345);
goto _LL414; _LL414: return Cyc_Absyn_strct( _temp413);} _LL241: return( void*)({
struct Cyc_Absyn_AnonStructType_struct* _temp415=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp415[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp416; _temp416.tag= Cyc_Absyn_AnonStructType;
_temp416.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp347); _temp416;}); _temp415;}); _LL243: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp417=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp417[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp418; _temp418.tag=
Cyc_Absyn_AnonUnionType; _temp418.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp349); _temp418;}); _temp417;}); _LL245: if( _temp355 == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous union", sizeof(
unsigned char), 16u));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp355)); _LL247: if( _temp361 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous struct", sizeof(
unsigned char), 17u));} return Cyc_Absyn_strctq(( struct _tuple0*) _check_null(
_temp361)); _LL249: return t; _LL251: if( _temp367 == 0){ return(( void*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: unresolved TypedefType",
sizeof( unsigned char), 38u));} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp419=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp419[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp420; _temp420.tag= Cyc_Absyn_TypedefType;
_temp420.f1= _temp371; _temp420.f2= 0; _temp420.f3=({ struct Cyc_Core_Opt*
_temp421=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp421->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp367))->v); _temp421;}); _temp420;}); _temp419;}); _LL253:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL255: return(( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u)); _LL257: return(( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u)); _LL259: return(( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u)); _LL209:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp422= t; struct Cyc_Absyn_Exp* _temp428; struct Cyc_Absyn_Tqual _temp430;
void* _temp432; _LL424: if(( unsigned int) _temp422 > 4u?*(( int*) _temp422) ==
Cyc_Absyn_ArrayType: 0){ _LL433: _temp432=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp422)->f1; goto _LL431; _LL431: _temp430=(( struct Cyc_Absyn_ArrayType_struct*)
_temp422)->f2; goto _LL429; _LL429: _temp428=(( struct Cyc_Absyn_ArrayType_struct*)
_temp422)->f3; goto _LL425;} else{ goto _LL426;} _LL426: goto _LL427; _LL425:
return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ( _temp432,( void*) Cyc_Absyn_HeapRgn,
_temp430), e, l); _LL427: return Cyc_Absyn_cast_exp( t, e, l); _LL423:;} static
int Cyc_Toc_atomic_typ( void* t){ void* _temp434= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp470; void* _temp472; void* _temp474; struct Cyc_Absyn_FnInfo
_temp476; struct Cyc_Absyn_Exp* _temp478; struct Cyc_Absyn_Tqual _temp480; void*
_temp482; struct Cyc_List_List* _temp484; struct Cyc_Absyn_Structdecl** _temp486;
struct Cyc_List_List* _temp488; struct _tuple0* _temp490; struct Cyc_Absyn_TunionInfo
_temp492; struct Cyc_List_List* _temp494; struct Cyc_List_List* _temp496; struct
Cyc_Absyn_TunionFieldInfo _temp498; struct Cyc_List_List* _temp500; void*
_temp502; struct Cyc_Absyn_Tunionfield* _temp504; struct Cyc_Absyn_Tuniondecl*
_temp506; struct Cyc_Absyn_Uniondecl** _temp508; struct Cyc_List_List* _temp510;
struct _tuple0* _temp512; struct Cyc_Absyn_PtrInfo _temp514; void* _temp516;
_LL436: if( _temp434 ==( void*) Cyc_Absyn_VoidType){ goto _LL437;} else{ goto
_LL438;} _LL438: if(( unsigned int) _temp434 > 4u?*(( int*) _temp434) == Cyc_Absyn_VarType:
0){ _LL471: _temp470=(( struct Cyc_Absyn_VarType_struct*) _temp434)->f1; goto
_LL439;} else{ goto _LL440;} _LL440: if(( unsigned int) _temp434 > 4u?*(( int*)
_temp434) == Cyc_Absyn_IntType: 0){ _LL475: _temp474=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp434)->f1; goto _LL473; _LL473: _temp472=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp434)->f2; goto _LL441;} else{ goto _LL442;} _LL442: if( _temp434 ==( void*)
Cyc_Absyn_FloatType){ goto _LL443;} else{ goto _LL444;} _LL444: if( _temp434 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL445;} else{ goto _LL446;} _LL446: if((
unsigned int) _temp434 > 4u?*(( int*) _temp434) == Cyc_Absyn_FnType: 0){ _LL477:
_temp476=(( struct Cyc_Absyn_FnType_struct*) _temp434)->f1; goto _LL447;} else{
goto _LL448;} _LL448: if(( unsigned int) _temp434 > 4u?*(( int*) _temp434) ==
Cyc_Absyn_ArrayType: 0){ _LL483: _temp482=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp434)->f1; goto _LL481; _LL481: _temp480=(( struct Cyc_Absyn_ArrayType_struct*)
_temp434)->f2; goto _LL479; _LL479: _temp478=(( struct Cyc_Absyn_ArrayType_struct*)
_temp434)->f3; goto _LL449;} else{ goto _LL450;} _LL450: if(( unsigned int)
_temp434 > 4u?*(( int*) _temp434) == Cyc_Absyn_TupleType: 0){ _LL485: _temp484=((
struct Cyc_Absyn_TupleType_struct*) _temp434)->f1; goto _LL451;} else{ goto
_LL452;} _LL452: if(( unsigned int) _temp434 > 4u?*(( int*) _temp434) == Cyc_Absyn_StructType:
0){ _LL491: _temp490=(( struct Cyc_Absyn_StructType_struct*) _temp434)->f1; goto
_LL489; _LL489: _temp488=(( struct Cyc_Absyn_StructType_struct*) _temp434)->f2;
goto _LL487; _LL487: _temp486=(( struct Cyc_Absyn_StructType_struct*) _temp434)->f3;
goto _LL453;} else{ goto _LL454;} _LL454: if(( unsigned int) _temp434 > 4u?*((
int*) _temp434) == Cyc_Absyn_TunionType: 0){ _LL493: _temp492=(( struct Cyc_Absyn_TunionType_struct*)
_temp434)->f1; goto _LL455;} else{ goto _LL456;} _LL456: if(( unsigned int)
_temp434 > 4u?*(( int*) _temp434) == Cyc_Absyn_AnonStructType: 0){ _LL495:
_temp494=(( struct Cyc_Absyn_AnonStructType_struct*) _temp434)->f1; goto _LL457;}
else{ goto _LL458;} _LL458: if(( unsigned int) _temp434 > 4u?*(( int*) _temp434)
== Cyc_Absyn_AnonUnionType: 0){ _LL497: _temp496=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp434)->f1; goto _LL459;} else{ goto _LL460;} _LL460: if(( unsigned int)
_temp434 > 4u?*(( int*) _temp434) == Cyc_Absyn_TunionFieldType: 0){ _LL499:
_temp498=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp434)->f1; _LL503:
_temp502=( void*) _temp498.field_info; if(*(( int*) _temp502) == Cyc_Absyn_KnownTunionfield){
_LL507: _temp506=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp502)->f1;
goto _LL505; _LL505: _temp504=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp502)->f2; goto _LL501;} else{ goto _LL462;} _LL501: _temp500= _temp498.targs;
goto _LL461;} else{ goto _LL462;} _LL462: if(( unsigned int) _temp434 > 4u?*((
int*) _temp434) == Cyc_Absyn_UnionType: 0){ _LL513: _temp512=(( struct Cyc_Absyn_UnionType_struct*)
_temp434)->f1; goto _LL511; _LL511: _temp510=(( struct Cyc_Absyn_UnionType_struct*)
_temp434)->f2; goto _LL509; _LL509: _temp508=(( struct Cyc_Absyn_UnionType_struct*)
_temp434)->f3; goto _LL463;} else{ goto _LL464;} _LL464: if(( unsigned int)
_temp434 > 4u?*(( int*) _temp434) == Cyc_Absyn_PointerType: 0){ _LL515: _temp514=((
struct Cyc_Absyn_PointerType_struct*) _temp434)->f1; goto _LL465;} else{ goto
_LL466;} _LL466: if(( unsigned int) _temp434 > 4u?*(( int*) _temp434) == Cyc_Absyn_RgnHandleType:
0){ _LL517: _temp516=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp434)->f1;
goto _LL467;} else{ goto _LL468;} _LL468: goto _LL469; _LL437: return 1; _LL439:
return 0; _LL441: return 1; _LL443: return 1; _LL445: return 1; _LL447: return 1;
_LL449: return Cyc_Toc_atomic_typ( _temp482); _LL451: for( 0; _temp484 != 0;
_temp484=(( struct Cyc_List_List*) _check_null( _temp484))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp484))->hd)).f2)){
return 0;}} return 1; _LL453: return 0; _LL455: return 0; _LL457: _temp496=
_temp494; goto _LL459; _LL459: for( 0; _temp496 != 0; _temp496=(( struct Cyc_List_List*)
_check_null( _temp496))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp496))->hd)->type)){ return 0;}} return
1; _LL461: return _temp504->typs == 0; _LL463: return 0; _LL465: return 0;
_LL467: return 0; _LL469:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp518= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _temp518.last_plus_one - _temp518.curr,
_temp518.curr);})); return 0; _LL435:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp519= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp525;
struct Cyc_Absyn_Conref* _temp527; struct Cyc_Absyn_Tqual _temp529; struct Cyc_Absyn_Conref*
_temp531; void* _temp533; void* _temp535; _LL521: if(( unsigned int) _temp519 >
4u?*(( int*) _temp519) == Cyc_Absyn_PointerType: 0){ _LL526: _temp525=(( struct
Cyc_Absyn_PointerType_struct*) _temp519)->f1; _LL536: _temp535=( void*) _temp525.elt_typ;
goto _LL534; _LL534: _temp533=( void*) _temp525.rgn_typ; goto _LL532; _LL532:
_temp531= _temp525.nullable; goto _LL530; _LL530: _temp529= _temp525.tq; goto
_LL528; _LL528: _temp527= _temp525.bounds; goto _LL522;} else{ goto _LL523;}
_LL523: goto _LL524; _LL522: { void* _temp537= Cyc_Tcutil_compress( _temp535);
_LL539: if( _temp537 ==( void*) Cyc_Absyn_VoidType){ goto _LL540;} else{ goto
_LL541;} _LL541: goto _LL542; _LL540: return 1; _LL542: return 0; _LL538:;}
_LL524: return 0; _LL520:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_arr* f){ void* _temp543= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp555; struct Cyc_Absyn_Structdecl* _temp557; struct Cyc_List_List* _temp558;
struct _tuple0* _temp560; struct Cyc_List_List* _temp562; struct Cyc_List_List*
_temp564; struct Cyc_Absyn_Uniondecl** _temp566; struct Cyc_List_List* _temp568;
struct _tuple0* _temp570; _LL545: if(( unsigned int) _temp543 > 4u?*(( int*)
_temp543) == Cyc_Absyn_StructType: 0){ _LL561: _temp560=(( struct Cyc_Absyn_StructType_struct*)
_temp543)->f1; goto _LL559; _LL559: _temp558=(( struct Cyc_Absyn_StructType_struct*)
_temp543)->f2; goto _LL556; _LL556: _temp555=(( struct Cyc_Absyn_StructType_struct*)
_temp543)->f3; if( _temp555 == 0){ goto _LL547;} else{ _temp557=* _temp555; goto
_LL546;}} else{ goto _LL547;} _LL547: if(( unsigned int) _temp543 > 4u?*(( int*)
_temp543) == Cyc_Absyn_AnonStructType: 0){ _LL563: _temp562=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp543)->f1; goto _LL548;} else{ goto _LL549;} _LL549: if(( unsigned int)
_temp543 > 4u?*(( int*) _temp543) == Cyc_Absyn_AnonUnionType: 0){ _LL565:
_temp564=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp543)->f1; goto _LL550;}
else{ goto _LL551;} _LL551: if(( unsigned int) _temp543 > 4u?*(( int*) _temp543)
== Cyc_Absyn_UnionType: 0){ _LL571: _temp570=(( struct Cyc_Absyn_UnionType_struct*)
_temp543)->f1; goto _LL569; _LL569: _temp568=(( struct Cyc_Absyn_UnionType_struct*)
_temp543)->f2; goto _LL567; _LL567: _temp566=(( struct Cyc_Absyn_UnionType_struct*)
_temp543)->f3; goto _LL552;} else{ goto _LL553;} _LL553: goto _LL554; _LL546:
if( _temp557->fields == 0){( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: struct missing fields", sizeof( unsigned char), 37u)));}
_temp562=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp557->fields))->v;
goto _LL548; _LL548: { struct Cyc_Absyn_Structfield* _temp572= Cyc_Absyn_lookup_field(
_temp562, f); goto _LL573; _LL573: if( _temp572 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp574=*(( struct Cyc_Absyn_Structfield*) _check_null( _temp572))->name;
xprintf("is_poly_field: bad field %.*s", _temp574.last_plus_one - _temp574.curr,
_temp574.curr);}));} return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp572))->type);} _LL550: return 0; _LL552: return 0; _LL554:(
int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct
_tagged_arr)({ struct _tagged_arr _temp575= Cyc_Absynpp_typ2string( t); xprintf("is_poly_field: bad type %.*s",
_temp575.last_plus_one - _temp575.curr, _temp575.curr);}))); _LL544:;} static
int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp576=( void*) e->r;
struct _tagged_arr* _temp584; struct Cyc_Absyn_Exp* _temp586; struct _tagged_arr*
_temp588; struct Cyc_Absyn_Exp* _temp590; _LL578: if(*(( int*) _temp576) == Cyc_Absyn_StructMember_e){
_LL587: _temp586=(( struct Cyc_Absyn_StructMember_e_struct*) _temp576)->f1; goto
_LL585; _LL585: _temp584=(( struct Cyc_Absyn_StructMember_e_struct*) _temp576)->f2;
goto _LL579;} else{ goto _LL580;} _LL580: if(*(( int*) _temp576) == Cyc_Absyn_StructArrow_e){
_LL591: _temp590=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp576)->f1; goto
_LL589; _LL589: _temp588=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp576)->f2;
goto _LL581;} else{ goto _LL582;} _LL582: goto _LL583; _LL579: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp586->topt))->v, _temp584);
_LL581: { void* _temp592= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp590->topt))->v); struct Cyc_Absyn_PtrInfo _temp598; struct Cyc_Absyn_Conref*
_temp600; struct Cyc_Absyn_Tqual _temp602; struct Cyc_Absyn_Conref* _temp604;
void* _temp606; void* _temp608; _LL594: if(( unsigned int) _temp592 > 4u?*(( int*)
_temp592) == Cyc_Absyn_PointerType: 0){ _LL599: _temp598=(( struct Cyc_Absyn_PointerType_struct*)
_temp592)->f1; _LL609: _temp608=( void*) _temp598.elt_typ; goto _LL607; _LL607:
_temp606=( void*) _temp598.rgn_typ; goto _LL605; _LL605: _temp604= _temp598.nullable;
goto _LL603; _LL603: _temp602= _temp598.tq; goto _LL601; _LL601: _temp600=
_temp598.bounds; goto _LL595;} else{ goto _LL596;} _LL596: goto _LL597; _LL595:
return Cyc_Toc_is_poly_field( _temp608, _temp588); _LL597:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp610= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp590->topt))->v); xprintf("is_poly_project:  bad type %.*s", _temp610.last_plus_one
- _temp610.curr, _temp610.curr);})); return 0; _LL593:;} _LL583: return 0;
_LL577:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp*
s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_qv, 0),({
struct Cyc_List_List* _temp611=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp611->hd=( void*) s; _temp611->tl= 0; _temp611;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc_GC_malloc_atomic_qv, 0),({
struct Cyc_List_List* _temp612=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp612->hd=( void*) s; _temp612->tl= 0; _temp612;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__region_malloc_qv,
0),({ struct Cyc_List_List* _temp613=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp613->hd=( void*) rgn; _temp613->tl=({ struct Cyc_List_List*
_temp614=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp614->hd=( void*) s; _temp614->tl= 0; _temp614;}); _temp613;}), 0);} static
unsigned char _temp615[ 3u]="f0"; static struct _tagged_arr Cyc_Toc_f0={
_temp615, _temp615, _temp615 + 3u}; static struct _tagged_arr* Cyc_Toc_field_names_v[
1u]={& Cyc_Toc_f0}; static struct _tagged_arr Cyc_Toc_field_names={( struct
_tagged_arr**) Cyc_Toc_field_names_v,( struct _tagged_arr**) Cyc_Toc_field_names_v,(
struct _tagged_arr**) Cyc_Toc_field_names_v + 1u}; static struct _tagged_arr*
Cyc_Toc_fieldname( int i){ unsigned int fsz= _get_arr_size( Cyc_Toc_field_names,
sizeof( struct _tagged_arr*)); if( i >= fsz){ Cyc_Toc_field_names=({
unsigned int _temp616=( unsigned int)( i + 1); struct _tagged_arr** _temp617=(
struct _tagged_arr**) GC_malloc( sizeof( struct _tagged_arr*) * _temp616);
struct _tagged_arr _temp620= _tag_arr( _temp617, sizeof( struct _tagged_arr*),(
unsigned int)( i + 1));{ unsigned int _temp618= _temp616; unsigned int j; for( j=
0; j < _temp618; j ++){ _temp617[ j]= j < fsz?*(( struct _tagged_arr**)
_check_unknown_subscript( Cyc_Toc_field_names, sizeof( struct _tagged_arr*),(
int) j)):({ struct _tagged_arr* _temp619=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp619[ 0]=( struct _tagged_arr) xprintf("f%d",(
int) j); _temp619;});}}; _temp620;});} return*(( struct _tagged_arr**)
_check_unknown_subscript( Cyc_Toc_field_names, sizeof( struct _tagged_arr*), i));}
static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){ return
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__sfile_to_file_qv, 0),({ struct
Cyc_List_List* _temp621=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp621->hd=( void*) e; _temp621->tl= 0; _temp621;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
Cyc_Toc__throw_qv, 0),({ struct Cyc_List_List* _temp622=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp622->hd=( void*) e; _temp622->tl=
0; _temp622;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_make_toplevel_tagged_arr(
void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ void* elt_typ;
struct Cyc_Absyn_Tqual elt_tqual;{ void* _temp623= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp629; struct Cyc_Absyn_Conref* _temp631; struct Cyc_Absyn_Tqual
_temp633; struct Cyc_Absyn_Conref* _temp635; void* _temp637; void* _temp639;
_LL625: if(( unsigned int) _temp623 > 4u?*(( int*) _temp623) == Cyc_Absyn_PointerType:
0){ _LL630: _temp629=(( struct Cyc_Absyn_PointerType_struct*) _temp623)->f1;
_LL640: _temp639=( void*) _temp629.elt_typ; goto _LL638; _LL638: _temp637=( void*)
_temp629.rgn_typ; goto _LL636; _LL636: _temp635= _temp629.nullable; goto _LL634;
_LL634: _temp633= _temp629.tq; goto _LL632; _LL632: _temp631= _temp629.bounds;
goto _LL626;} else{ goto _LL627;} _LL627: goto _LL628; _LL626: elt_typ= Cyc_Toc_typ_to_c(
_temp639); elt_tqual= _temp633; goto _LL624; _LL628: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("make_toplevel_tagged_arr: not a pointer",
sizeof( unsigned char), 40u)); _LL624:;}{ int is_string= 0;{ void* _temp641=(
void*) e->r; void* _temp647; struct _tagged_arr _temp649; _LL643: if(*(( int*)
_temp641) == Cyc_Absyn_Const_e){ _LL648: _temp647=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp641)->f1; if(( unsigned int) _temp647 > 1u?*(( int*) _temp647) == Cyc_Absyn_String_c:
0){ _LL650: _temp649=(( struct Cyc_Absyn_String_c_struct*) _temp647)->f1; goto
_LL644;} else{ goto _LL645;}} else{ goto _LL645;} _LL645: goto _LL646; _LL644:
is_string= 1; goto _LL642; _LL646: goto _LL642; _LL642:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp654=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp654[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp655; _temp655.tag= Cyc_Absyn_ArrayType; _temp655.f1=(
void*) elt_typ; _temp655.f2= Cyc_Toc_mt_tq; _temp655.f3=( struct Cyc_Absyn_Exp*)
sz; _temp655;}); _temp654;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp651=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp651->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp652=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp652[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp653; _temp653.tag= Cyc_Absyn_Var_d;
_temp653.f1= vd; _temp653;}); _temp652;}), 0); _temp651->tl= Cyc_Toc_result_decls;
_temp651;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ if( elt_tqual.q_const){ xexp=
Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq), e, 0);} else{
xexp= e;}}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp( xexp, sz, 0);
struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*(*
_temp656)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct _tuple4* _temp659=({ struct _tuple4* _temp664=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp664->f1= 0; _temp664->f2=
xexp; _temp664;}); struct _tuple4* _temp660=({ struct _tuple4* _temp663=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp663->f1= 0; _temp663->f2=
xexp; _temp663;}); struct _tuple4* _temp661=({ struct _tuple4* _temp662=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp662->f1= 0; _temp662->f2=
xplussz; _temp662;}); struct _tuple4* _temp657[ 3u]={ _temp659, _temp660,
_temp661}; struct _tagged_arr _temp658={( void*) _temp657,( void*) _temp657,(
void*)( _temp657 + 3u)}; _temp656( _temp658);}), 0); return urm_exp;}}}} struct
Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab;
struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap; int toplevel;
} ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({ struct Cyc_Toc_Env*
_temp665=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp665->break_lab= 0; _temp665->continue_lab= 0; _temp665->fallthru_info= 0;
_temp665->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp665->toplevel= 1;
_temp665;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp666=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp666->break_lab= e->break_lab; _temp666->continue_lab=
e->continue_lab; _temp666->fallthru_info= e->fallthru_info; _temp666->varmap= e->varmap;
_temp666->toplevel= e->toplevel; _temp666;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp667=(* x).f1; struct
Cyc_List_List* _temp673; _LL669: if(( unsigned int) _temp667 > 1u?*(( int*)
_temp667) == Cyc_Absyn_Rel_n: 0){ _LL674: _temp673=(( struct Cyc_Absyn_Rel_n_struct*)
_temp667)->f1; goto _LL670;} else{ goto _LL671;} _LL671: goto _LL672; _LL670:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp675=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp675[ 0]=({ struct
Cyc_Core_Impossible_struct _temp676; _temp676.tag= Cyc_Core_Impossible; _temp676.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp677= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp677.last_plus_one - _temp677.curr,
_temp677.curr);}); _temp676;}); _temp675;})); _LL672: goto _LL668; _LL668:;}{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)(
ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=
0; ans->continue_lab= 0; return ans;} struct _tuple6{ struct _tagged_arr* f1;
struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_arr*
break_l, struct _tagged_arr* fallthru_l, struct Cyc_List_List* fallthru_binders,
struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List* _temp678= 0; goto
_LL679; _LL679: for( 0; fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*)
_check_null( fallthru_binders))->tl){ _temp678=({ struct Cyc_List_List* _temp680=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp680->hd=(
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
fallthru_binders))->hd)->name; _temp680->tl= _temp678; _temp680;});} _temp678=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp678);{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt*
_temp681=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp681->v=( void*) break_l; _temp681;}); ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp682=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp682->v=( void*)({ struct _tuple6* _temp683=( struct _tuple6*) GC_malloc(
sizeof( struct _tuple6)); _temp683->f1= fallthru_l; _temp683->f2= _temp678;
_temp683->f3= next_case_env->varmap; _temp683;}); _temp682;}); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e,
struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp684=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp684->v=( void*) break_l; _temp684;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp685=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp685->v=( void*)({
struct _tuple6* _temp686=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp686->f1= next_l; _temp686->f2= 0; _temp686->f3=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp686;}); _temp685;}); return ans;} struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4;
unsigned char f5; int f6; } ; struct _tuple8{ struct _tuple0* f1; void* f2;
struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("expecting a literal format string", sizeof(
unsigned char), 34u));}{ struct _tagged_arr fmt_str;{ void* _temp687=( void*)((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd)->r;
void* _temp693; struct _tagged_arr _temp695; _LL689: if(*(( int*) _temp687) ==
Cyc_Absyn_Const_e){ _LL694: _temp693=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp687)->f1; if(( unsigned int) _temp693 > 1u?*(( int*) _temp693) == Cyc_Absyn_String_c:
0){ _LL696: _temp695=(( struct Cyc_Absyn_String_c_struct*) _temp693)->f1; goto
_LL690;} else{ goto _LL691;}} else{ goto _LL691;} _LL691: goto _LL692; _LL690:
fmt_str= _temp695; goto _LL688; _LL692: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("expecting a literal format string",
sizeof( unsigned char), 34u)); _LL688:;}{ int len=( int) _get_arr_size( fmt_str,
sizeof( unsigned char)); int i= 0; for( i= 0; i < len; i ++){ if(*(( const
unsigned char*) _check_unknown_subscript( fmt_str, sizeof( unsigned char), i))
=='%'){ i ++; if( i < len?*(( const unsigned char*) _check_unknown_subscript(
fmt_str, sizeof( unsigned char), i)) =='s': 0){ break;}}} if( i == len){ if(
aopt != 0){ args=({ struct Cyc_List_List* _temp697=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp697->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp697->tl= args; _temp697;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp698= _new_region(); struct
_RegionHandle* r=& _temp698; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*)
_check_unknown_subscript( fmt_str, sizeof( unsigned char), i)); rev_fmt=({
struct Cyc_List_List* _temp699=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp699->hd=( void*)(( int) c); _temp699->tl=
rev_fmt; _temp699;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* _temp700=
Cyc_Formatstr_parse_conversionspecification( fmt_str, i + 1); goto _LL701;
_LL701: if( _temp700 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("bad format string", sizeof( unsigned char), 18u));}{ struct _tuple7
_temp704; int _temp705; unsigned char _temp707; struct Cyc_List_List* _temp709;
struct Cyc_List_List* _temp711; struct Cyc_List_List* _temp713; struct Cyc_List_List*
_temp715; struct _tuple7* _temp702=( struct _tuple7*)(( struct Cyc_Core_Opt*)
_check_null( _temp700))->v; _temp704=* _temp702; _LL716: _temp715= _temp704.f1;
goto _LL714; _LL714: _temp713= _temp704.f2; goto _LL712; _LL712: _temp711=
_temp704.f3; goto _LL710; _LL710: _temp709= _temp704.f4; goto _LL708; _LL708:
_temp707= _temp704.f5; goto _LL706; _LL706: _temp705= _temp704.f6; goto _LL703;
_LL703: i= _temp705 - 1; if( _temp707 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)(
r,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x))
Cyc_List_rflatten)( r,({ struct Cyc_List_List*(* _temp717)( struct _RegionHandle*,
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_arr)) Cyc_List_rlist; struct _RegionHandle* _temp724= r; struct Cyc_List_List*
_temp720= _temp715; struct Cyc_List_List* _temp721= _temp713; struct Cyc_List_List*
_temp722= _temp711; struct Cyc_List_List* _temp723= _temp709; struct Cyc_List_List*
_temp718[ 4u]={ _temp720, _temp721, _temp722, _temp723}; struct _tagged_arr
_temp719={( void*) _temp718,( void*) _temp718,( void*)( _temp718 + 4u)};
_temp717( _temp724, _temp719);})), rev_fmt); rev_fmt=({ struct Cyc_List_List*
_temp725=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp725->hd=( void*)(( int) _temp707); _temp725->tl= rev_fmt; _temp725;});{
struct Cyc_List_List* _temp726= _temp713; struct Cyc_List_List _temp733; struct
Cyc_List_List* _temp734; int _temp736; _LL728: if( _temp726 == 0){ goto _LL730;}
else{ _temp733=* _temp726; _LL737: _temp736=( int) _temp733.hd; goto _LL735;
_LL735: _temp734= _temp733.tl; if( _temp734 == 0){ goto _LL732;} else{ goto
_LL730;}} _LL732: if( _temp736 =='*'){ goto _LL729;} else{ goto _LL730;} _LL730:
goto _LL731; _LL729: { struct _tuple0* _temp738= Cyc_Toc_temp_var(); goto _LL739;
_LL739: rev_temps=({ struct Cyc_List_List* _temp740=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp740->hd=( void*)({
struct _tuple8* _temp741=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp741->f1= _temp738; _temp741->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp741->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp741;}); _temp740->tl= rev_temps; _temp740;});
rev_result=({ struct Cyc_List_List* _temp742=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp742->hd=( void*) Cyc_Absyn_var_exp(
_temp738, 0); _temp742->tl= rev_result; _temp742;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL727;} _LL731: goto _LL727; _LL727:;}{ struct Cyc_List_List* _temp743=
_temp711; struct Cyc_List_List _temp750; struct Cyc_List_List* _temp751; struct
Cyc_List_List _temp753; struct Cyc_List_List* _temp754; int _temp756; int
_temp758; _LL745: if( _temp743 == 0){ goto _LL747;} else{ _temp750=* _temp743;
_LL759: _temp758=( int) _temp750.hd; goto _LL752; _LL752: _temp751= _temp750.tl;
if( _temp751 == 0){ goto _LL747;} else{ _temp753=* _temp751; _LL757: _temp756=(
int) _temp753.hd; goto _LL755; _LL755: _temp754= _temp753.tl; if( _temp754 == 0){
goto _LL749;} else{ goto _LL747;}}} _LL749: if( _temp758 =='.'? _temp756 =='*':
0){ goto _LL746;} else{ goto _LL747;} _LL747: goto _LL748; _LL746: { struct
_tuple0* _temp760= Cyc_Toc_temp_var(); goto _LL761; _LL761: rev_temps=({ struct
Cyc_List_List* _temp762=( struct Cyc_List_List*) _region_malloc( r, sizeof(
struct Cyc_List_List)); _temp762->hd=( void*)({ struct _tuple8* _temp763=(
struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8)); _temp763->f1=
_temp760; _temp763->f2=( void*)(( struct Cyc_List_List*) _check_null( typs))->hd;
_temp763->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd;
_temp763;}); _temp762->tl= rev_temps; _temp762;}); rev_result=({ struct Cyc_List_List*
_temp764=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp764->hd=( void*) Cyc_Absyn_var_exp( _temp760, 0); _temp764->tl= rev_result;
_temp764;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL744;} _LL748: goto
_LL744; _LL744:;} if( _temp707 !='%'){ if( args == 0? 1: typs == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp765= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp765.last_plus_one - _temp765.curr, _temp765.curr);}));}{ struct _tuple0*
_temp766= Cyc_Toc_temp_var(); goto _LL767; _LL767: rev_temps=({ struct Cyc_List_List*
_temp768=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp768->hd=( void*)({ struct _tuple8* _temp769=( struct _tuple8*)
_region_malloc( r, sizeof( struct _tuple8)); _temp769->f1= _temp766; _temp769->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp769->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp769;});
_temp768->tl= rev_temps; _temp768;}); rev_result=({ struct Cyc_List_List*
_temp770=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp770->hd=( void*) Cyc_Absyn_var_exp( _temp766, 0); _temp770->tl= rev_result;
_temp770;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp771=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp771->hd=( void*)(( int)'.'); _temp771->tl= rev_fmt; _temp771;}); rev_fmt=({
struct Cyc_List_List* _temp772=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp772->hd=( void*)(( int)'*'); _temp772->tl=
rev_fmt; _temp772;}); rev_fmt=({ struct Cyc_List_List* _temp773=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp773->hd=( void*)(( int)'s');
_temp773->tl= rev_fmt; _temp773;});{ struct _tuple0* _temp774= Cyc_Toc_temp_var();
goto _LL775; _LL775: rev_temps=({ struct Cyc_List_List* _temp776=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp776->hd=( void*)({
struct _tuple8* _temp777=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp777->f1= _temp774; _temp777->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp777->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp777;}); _temp776->tl= rev_temps; _temp776;});{
struct Cyc_Absyn_Exp* _temp778= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp774, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp774, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL779; _LL779: {
struct Cyc_Absyn_Exp* _temp780= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp774, 0), Cyc_Toc_curr_sp, 0); goto _LL781; _LL781: rev_result=({ struct Cyc_List_List*
_temp782=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp782->hd=( void*) _temp778; _temp782->tl= rev_result; _temp782;});
rev_result=({ struct Cyc_List_List* _temp783=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp783->hd=( void*)
_temp780; _temp783->tl= rev_result; _temp783;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}}{
struct Cyc_List_List* _temp785=({ struct Cyc_List_List* _temp784=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp784->hd=( void*) Cyc_Absyn_string_exp((
struct _tagged_arr) Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_fmt)), 0); _temp784->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_result); _temp784;}); goto _LL786;
_LL786: if( aopt != 0){ _temp785=({ struct Cyc_List_List* _temp787=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp787->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp787->tl= _temp785; _temp787;});}{
struct Cyc_Absyn_Exp* _temp788= Cyc_Absyn_primop_exp( p, _temp785, 0); goto
_LL789; _LL789: s= Cyc_Absyn_exp_stmt( _temp788, 0); for( 0; rev_temps != 0;
rev_temps=(( struct Cyc_List_List*) _check_null( rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f1,(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f2,(
struct Cyc_Absyn_Exp*)(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
rev_temps))->hd)).f3, s, 0);}}}}; _pop_region(& _temp698);} return Cyc_Absyn_stmt_exp(
s, 0);}}}} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u));} return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Missing type in primop ", sizeof( unsigned char), 24u));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp790=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp790->f1= Cyc_Toc_mt_tq; _temp790->f2=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp790;});} static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp791=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp791[ 0]=({
struct _tuple4 _temp792; _temp792.f1= 0; _temp792.f2= e; _temp792;}); _temp791;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp793= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); goto _LL794; _LL794: if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic( _temp793): Cyc_Toc_malloc_ptr(
_temp793), 0);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( rgnopt); Cyc_Toc_exp_to_c( nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, Cyc_Toc_rmalloc_exp( r, _temp793), 0);}}} else{ t= struct_typ; eo= 0;} return
Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, t, eo, s, 0), 0);} static struct
Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Absyn_Stmt* s, int e1_already_translated); static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s); static
struct Cyc_Absyn_Stmt* Cyc_Toc_init_array( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
lhs, struct Cyc_List_List* dles0, struct Cyc_Absyn_Stmt* s){ int count=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( dles0) - 1;{ struct Cyc_List_List*
_temp795=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
dles0); goto _LL796; _LL796: for( 0; _temp795 != 0; _temp795=(( struct Cyc_List_List*)
_check_null( _temp795))->tl){ struct _tuple4 _temp799; struct Cyc_Absyn_Exp*
_temp800; struct Cyc_List_List* _temp802; struct _tuple4* _temp797=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp795))->hd; _temp799=*
_temp797; _LL803: _temp802= _temp799.f1; goto _LL801; _LL801: _temp800= _temp799.f2;
goto _LL798; _LL798: { struct Cyc_Absyn_Exp* e_index; if( _temp802 == 0){
e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if((( struct Cyc_List_List*)
_check_null( _temp802))->tl != 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("multiple designators in array", sizeof( unsigned char), 30u));}{ void*
_temp804=( void*)(( struct Cyc_List_List*) _check_null( _temp802))->hd; goto
_LL805; _LL805: { void* _temp806= _temp804; struct Cyc_Absyn_Exp* _temp812;
struct _tagged_arr* _temp814; _LL808: if(*(( int*) _temp806) == Cyc_Absyn_ArrayElement){
_LL813: _temp812=(( struct Cyc_Absyn_ArrayElement_struct*) _temp806)->f1; goto
_LL809;} else{ goto _LL810;} _LL810: if(*(( int*) _temp806) == Cyc_Absyn_FieldName){
_LL815: _temp814=(( struct Cyc_Absyn_FieldName_struct*) _temp806)->f1; goto
_LL811;} else{ goto _LL807;} _LL809: Cyc_Toc_exp_to_c( nv, _temp812); e_index=
_temp812; goto _LL807; _LL811: e_index=(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("field name designators in array",
sizeof( unsigned char), 32u)); goto _LL807; _LL807:;}}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp816=( void*)
_temp800->r; struct Cyc_List_List* _temp826; struct Cyc_Absyn_Exp* _temp828;
struct Cyc_Absyn_Exp* _temp830; struct Cyc_Absyn_Vardecl* _temp832; struct Cyc_List_List*
_temp834; void* _temp836; _LL818: if(*(( int*) _temp816) == Cyc_Absyn_Array_e){
_LL827: _temp826=(( struct Cyc_Absyn_Array_e_struct*) _temp816)->f1; goto _LL819;}
else{ goto _LL820;} _LL820: if(*(( int*) _temp816) == Cyc_Absyn_Comprehension_e){
_LL833: _temp832=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp816)->f1;
goto _LL831; _LL831: _temp830=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp816)->f2; goto _LL829; _LL829: _temp828=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp816)->f3; goto _LL821;} else{ goto _LL822;} _LL822: if(*(( int*) _temp816)
== Cyc_Absyn_AnonStruct_e){ _LL837: _temp836=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp816)->f1; goto _LL835; _LL835: _temp834=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp816)->f2; goto _LL823;} else{ goto _LL824;} _LL824: goto _LL825; _LL819: s=
Cyc_Toc_init_array( nv, lval, _temp826, s); goto _LL817; _LL821: s= Cyc_Toc_init_comprehension(
nv, lval, _temp832, _temp830, _temp828, s, 0); goto _LL817; _LL823: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp836, _temp834, s); goto _LL817; _LL825: Cyc_Toc_exp_to_c( nv,
_temp800); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp800, 0), s, 0); goto _LL817; _LL817:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp838= vd->name; goto _LL839; _LL839:
if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp838, Cyc_Absyn_varb_exp( _temp838,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp862=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp862[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp863; _temp863.tag= Cyc_Absyn_Local_b; _temp863.f1= vd; _temp863;});
_temp862;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp838, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp838, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp838, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp838, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp840=( void*) e2->r; struct Cyc_List_List* _temp850; struct Cyc_Absyn_Exp*
_temp852; struct Cyc_Absyn_Exp* _temp854; struct Cyc_Absyn_Vardecl* _temp856;
struct Cyc_List_List* _temp858; void* _temp860; _LL842: if(*(( int*) _temp840)
== Cyc_Absyn_Array_e){ _LL851: _temp850=(( struct Cyc_Absyn_Array_e_struct*)
_temp840)->f1; goto _LL843;} else{ goto _LL844;} _LL844: if(*(( int*) _temp840)
== Cyc_Absyn_Comprehension_e){ _LL857: _temp856=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp840)->f1; goto _LL855; _LL855: _temp854=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp840)->f2; goto _LL853; _LL853: _temp852=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp840)->f3; goto _LL845;} else{ goto _LL846;} _LL846: if(*(( int*) _temp840)
== Cyc_Absyn_AnonStruct_e){ _LL861: _temp860=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp840)->f1; goto _LL859; _LL859: _temp858=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp840)->f2; goto _LL847;} else{ goto _LL848;} _LL848: goto _LL849; _LL843:
body= Cyc_Toc_init_array( nv2, lval, _temp850, Cyc_Absyn_skip_stmt( 0)); goto
_LL841; _LL845: body= Cyc_Toc_init_comprehension( nv2, lval, _temp856, _temp854,
_temp852, Cyc_Absyn_skip_stmt( 0), 0); goto _LL841; _LL847: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp860, _temp858, Cyc_Absyn_skip_stmt( 0)); goto _LL841; _LL849: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL841; _LL841:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp838, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp864=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); goto _LL865; _LL865: for( 0; _temp864 != 0; _temp864=((
struct Cyc_List_List*) _check_null( _temp864))->tl){ struct _tuple4 _temp868;
struct Cyc_Absyn_Exp* _temp869; struct Cyc_List_List* _temp871; struct _tuple4*
_temp866=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp864))->hd;
_temp868=* _temp866; _LL872: _temp871= _temp868.f1; goto _LL870; _LL870:
_temp869= _temp868.f2; goto _LL867; _LL867: if( _temp871 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list", sizeof(
unsigned char), 22u));} if((( struct Cyc_List_List*) _check_null( _temp871))->tl
!= 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u));}{ void* _temp873=( void*)(( struct Cyc_List_List*)
_check_null( _temp871))->hd; struct _tagged_arr* _temp879; _LL875: if(*(( int*)
_temp873) == Cyc_Absyn_FieldName){ _LL880: _temp879=(( struct Cyc_Absyn_FieldName_struct*)
_temp873)->f1; goto _LL876;} else{ goto _LL877;} _LL877: goto _LL878; _LL876: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp879, 0);{ void*
_temp881=( void*) _temp869->r; struct Cyc_List_List* _temp891; struct Cyc_Absyn_Exp*
_temp893; struct Cyc_Absyn_Exp* _temp895; struct Cyc_Absyn_Vardecl* _temp897;
struct Cyc_List_List* _temp899; void* _temp901; _LL883: if(*(( int*) _temp881)
== Cyc_Absyn_Array_e){ _LL892: _temp891=(( struct Cyc_Absyn_Array_e_struct*)
_temp881)->f1; goto _LL884;} else{ goto _LL885;} _LL885: if(*(( int*) _temp881)
== Cyc_Absyn_Comprehension_e){ _LL898: _temp897=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp881)->f1; goto _LL896; _LL896: _temp895=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp881)->f2; goto _LL894; _LL894: _temp893=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp881)->f3; goto _LL886;} else{ goto _LL887;} _LL887: if(*(( int*) _temp881)
== Cyc_Absyn_AnonStruct_e){ _LL902: _temp901=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp881)->f1; goto _LL900; _LL900: _temp899=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp881)->f2; goto _LL888;} else{ goto _LL889;} _LL889: goto _LL890; _LL884: s=
Cyc_Toc_init_array( nv, lval, _temp891, s); goto _LL882; _LL886: s= Cyc_Toc_init_comprehension(
nv, lval, _temp897, _temp895, _temp893, s, 0); goto _LL882; _LL888: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp901, _temp899, s); goto _LL882; _LL890: Cyc_Toc_exp_to_c( nv,
_temp869); if( Cyc_Toc_is_poly_field( struct_type, _temp879)){ _temp869= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp869, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp869, 0), 0), s, 0); goto _LL882; _LL882:;} goto
_LL874;} _LL878:( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u))); _LL874:;}}}
return s;} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env*
nv, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
_RegionHandle _temp903= _new_region(); struct _RegionHandle* r=& _temp903;
_push_region( r);{ struct Cyc_List_List* _temp904=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c, es); goto _LL905; _LL905: { struct
_tagged_arr* _temp906= Cyc_Toc_add_tuple_type( _temp904); goto _LL907; _LL907: {
struct _tuple0* _temp908= Cyc_Toc_temp_var(); goto _LL909; _LL909: { struct Cyc_Absyn_Exp*
_temp910= Cyc_Absyn_var_exp( _temp908, 0); goto _LL911; _LL911: { struct Cyc_Absyn_Stmt*
_temp912= Cyc_Absyn_exp_stmt( _temp910, 0); goto _LL913; _LL913: { struct Cyc_Absyn_Exp*(*
_temp914)( struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL915;
_LL915: { int is_atomic= 1; struct Cyc_List_List* _temp916=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es); goto
_LL917; _LL917:{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp916); for( 0; _temp916 != 0;( _temp916=(( struct Cyc_List_List*)
_check_null( _temp916))->tl, -- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp916))->hd; struct Cyc_Absyn_Exp* lval=
_temp914( _temp910, Cyc_Toc_fieldname( i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v): 0;{ void* _temp918=(
void*) e->r; struct Cyc_List_List* _temp926; struct Cyc_Absyn_Exp* _temp928;
struct Cyc_Absyn_Exp* _temp930; struct Cyc_Absyn_Vardecl* _temp932; _LL920: if(*((
int*) _temp918) == Cyc_Absyn_Array_e){ _LL927: _temp926=(( struct Cyc_Absyn_Array_e_struct*)
_temp918)->f1; goto _LL921;} else{ goto _LL922;} _LL922: if(*(( int*) _temp918)
== Cyc_Absyn_Comprehension_e){ _LL933: _temp932=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp918)->f1; goto _LL931; _LL931: _temp930=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp918)->f2; goto _LL929; _LL929: _temp928=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp918)->f3; goto _LL923;} else{ goto _LL924;} _LL924: goto _LL925; _LL921:
_temp912= Cyc_Toc_init_array( nv, lval, _temp926, _temp912); goto _LL919; _LL923:
_temp912= Cyc_Toc_init_comprehension( nv, lval, _temp932, _temp930, _temp928,
_temp912, 0); goto _LL919; _LL925: Cyc_Toc_exp_to_c( nv, e); _temp912= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp914( _temp910, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp912, 0); goto _LL919; _LL919:;}}}{ struct Cyc_Absyn_Exp*
_temp934= Cyc_Toc_make_struct( nv, _temp908, Cyc_Absyn_strct( _temp906),
_temp912, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp934;}}}}}}}};
_pop_region(& _temp903);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct(
struct Cyc_Toc_Env* nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* dles, struct _tuple0* tdn){ struct _tuple0*
_temp935= Cyc_Toc_temp_var(); goto _LL936; _LL936: { struct Cyc_Absyn_Exp*
_temp937= Cyc_Absyn_var_exp( _temp935, 0); goto _LL938; _LL938: { struct Cyc_Absyn_Stmt*
_temp939= Cyc_Absyn_exp_stmt( _temp937, 0); goto _LL940; _LL940: { struct Cyc_Absyn_Exp*(*
_temp941)( struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL942;
_LL942: { int is_atomic= 1; struct _RegionHandle _temp943= _new_region(); struct
_RegionHandle* r=& _temp943; _push_region( r);{ struct Cyc_List_List* _temp944=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)(
r, dles); goto _LL945; _LL945: for( 0; _temp944 != 0; _temp944=(( struct Cyc_List_List*)
_check_null( _temp944))->tl){ struct _tuple4 _temp948; struct Cyc_Absyn_Exp*
_temp949; struct Cyc_List_List* _temp951; struct _tuple4* _temp946=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp944))->hd; _temp948=*
_temp946; _LL952: _temp951= _temp948.f1; goto _LL950; _LL950: _temp949= _temp948.f2;
goto _LL947; _LL947: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp949->topt))->v): 0; if( _temp951 == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u));} if((( struct Cyc_List_List*) _check_null(
_temp951))->tl != 0){ struct _tuple0* _temp953= Cyc_Toc_temp_var(); goto _LL954;
_LL954: { struct Cyc_Absyn_Exp* _temp955= Cyc_Absyn_var_exp( _temp953, 0); goto
_LL956; _LL956: for( 0; _temp951 != 0; _temp951=(( struct Cyc_List_List*)
_check_null( _temp951))->tl){ void* _temp957=( void*)(( struct Cyc_List_List*)
_check_null( _temp951))->hd; struct _tagged_arr* _temp963; _LL959: if(*(( int*)
_temp957) == Cyc_Absyn_FieldName){ _LL964: _temp963=(( struct Cyc_Absyn_FieldName_struct*)
_temp957)->f1; goto _LL960;} else{ goto _LL961;} _LL961: goto _LL962; _LL960:
if( Cyc_Toc_is_poly_field( struct_type, _temp963)){ _temp955= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp955, 0);} _temp939= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp941( _temp937, _temp963, 0), _temp955, 0), 0),
_temp939, 0); goto _LL958; _LL962:( int) _throw((( void*(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)( _tag_arr("array designator in struct", sizeof(
unsigned char), 27u))); _LL958:;} Cyc_Toc_exp_to_c( nv, _temp949); _temp939= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp955, _temp949, 0), 0), _temp939,
0);}} else{ void* _temp965=( void*)(( struct Cyc_List_List*) _check_null(
_temp951))->hd; struct _tagged_arr* _temp971; _LL967: if(*(( int*) _temp965) ==
Cyc_Absyn_FieldName){ _LL972: _temp971=(( struct Cyc_Absyn_FieldName_struct*)
_temp965)->f1; goto _LL968;} else{ goto _LL969;} _LL969: goto _LL970; _LL968: {
struct Cyc_Absyn_Exp* lval= _temp941( _temp937, _temp971, 0);{ void* _temp973=(
void*) _temp949->r; struct Cyc_List_List* _temp983; struct Cyc_Absyn_Exp*
_temp985; struct Cyc_Absyn_Exp* _temp987; struct Cyc_Absyn_Vardecl* _temp989;
struct Cyc_List_List* _temp991; void* _temp993; _LL975: if(*(( int*) _temp973)
== Cyc_Absyn_Array_e){ _LL984: _temp983=(( struct Cyc_Absyn_Array_e_struct*)
_temp973)->f1; goto _LL976;} else{ goto _LL977;} _LL977: if(*(( int*) _temp973)
== Cyc_Absyn_Comprehension_e){ _LL990: _temp989=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp973)->f1; goto _LL988; _LL988: _temp987=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp973)->f2; goto _LL986; _LL986: _temp985=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp973)->f3; goto _LL978;} else{ goto _LL979;} _LL979: if(*(( int*) _temp973)
== Cyc_Absyn_AnonStruct_e){ _LL994: _temp993=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp973)->f1; goto _LL992; _LL992: _temp991=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp973)->f2; goto _LL980;} else{ goto _LL981;} _LL981: goto _LL982; _LL976:
_temp939= Cyc_Toc_init_array( nv, lval, _temp983, _temp939); goto _LL974; _LL978:
_temp939= Cyc_Toc_init_comprehension( nv, lval, _temp989, _temp987, _temp985,
_temp939, 0); goto _LL974; _LL980: _temp939= Cyc_Toc_init_anon_struct( nv, lval,
_temp993, _temp991, _temp939); goto _LL974; _LL982: Cyc_Toc_exp_to_c( nv,
_temp949); if( Cyc_Toc_is_poly_field( struct_type, _temp971)){ _temp949= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp949, 0);} _temp939= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp949, 0), 0), _temp939, 0); goto _LL974; _LL974:;}
goto _LL966;} _LL970:( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u))); _LL966:;}}}{
struct Cyc_Absyn_Exp* _temp995= Cyc_Toc_make_struct( nv, _temp935, Cyc_Absyn_strctq(
tdn), _temp939, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp995;};
_pop_region(& _temp943);}}}}} struct _tuple9{ struct Cyc_Core_Opt* f1; struct
Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple9* pr){ return Cyc_Absyn_assignop_exp( el,(*
pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp996=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp996[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp997; _temp997.tag= Cyc_Absyn_Increment_e;
_temp997.f1= e1; _temp997.f2=( void*) incr; _temp997;}); _temp996;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp998=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp1008; struct Cyc_Absyn_Exp*
_temp1010; void* _temp1012; struct _tagged_arr* _temp1014; struct Cyc_Absyn_Exp*
_temp1016; _LL1000: if(*(( int*) _temp998) == Cyc_Absyn_StmtExp_e){ _LL1009:
_temp1008=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp998)->f1; goto _LL1001;}
else{ goto _LL1002;} _LL1002: if(*(( int*) _temp998) == Cyc_Absyn_Cast_e){
_LL1013: _temp1012=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp998)->f1;
goto _LL1011; _LL1011: _temp1010=(( struct Cyc_Absyn_Cast_e_struct*) _temp998)->f2;
goto _LL1003;} else{ goto _LL1004;} _LL1004: if(*(( int*) _temp998) == Cyc_Absyn_StructMember_e){
_LL1017: _temp1016=(( struct Cyc_Absyn_StructMember_e_struct*) _temp998)->f1;
goto _LL1015; _LL1015: _temp1014=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp998)->f2; goto _LL1005;} else{ goto _LL1006;} _LL1006: goto _LL1007;
_LL1001:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1008, fs, f, f_env); goto _LL999; _LL1003:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1010, fs, f, f_env); goto
_LL999; _LL1005:( void*)( e1->r=( void*)(( void*) _temp1016->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1018=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1018->hd=( void*) _temp1014; _temp1018->tl= fs;
_temp1018;}), f, f_env); goto _LL999; _LL1007: if( ! Cyc_Absyn_is_lvalue( e1)){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp1019= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1019.last_plus_one - _temp1019.curr, _temp1019.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL999;} _LL999:;}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp1020=( void*) s->r; struct Cyc_Absyn_Exp* _temp1030; struct Cyc_Absyn_Stmt*
_temp1032; struct Cyc_Absyn_Decl* _temp1034; struct Cyc_Absyn_Stmt* _temp1036;
struct Cyc_Absyn_Stmt* _temp1038; _LL1022: if(( unsigned int) _temp1020 > 1u?*((
int*) _temp1020) == Cyc_Absyn_Exp_s: 0){ _LL1031: _temp1030=(( struct Cyc_Absyn_Exp_s_struct*)
_temp1020)->f1; goto _LL1023;} else{ goto _LL1024;} _LL1024: if(( unsigned int)
_temp1020 > 1u?*(( int*) _temp1020) == Cyc_Absyn_Decl_s: 0){ _LL1035: _temp1034=((
struct Cyc_Absyn_Decl_s_struct*) _temp1020)->f1; goto _LL1033; _LL1033:
_temp1032=(( struct Cyc_Absyn_Decl_s_struct*) _temp1020)->f2; goto _LL1025;}
else{ goto _LL1026;} _LL1026: if(( unsigned int) _temp1020 > 1u?*(( int*)
_temp1020) == Cyc_Absyn_Seq_s: 0){ _LL1039: _temp1038=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1020)->f1; goto _LL1037; _LL1037: _temp1036=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1020)->f2; goto _LL1027;} else{ goto _LL1028;} _LL1028: goto _LL1029;
_LL1023:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1030, fs, f, f_env); goto _LL1021; _LL1025:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1032, fs, f, f_env);
goto _LL1021; _LL1027:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1036, fs, f, f_env); goto _LL1021; _LL1029:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr _temp1040= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1040.last_plus_one - _temp1040.curr,
_temp1040.curr);})); goto _LL1021; _LL1021:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c(
struct _RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x){ struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){
return 0;} result=({ struct Cyc_List_List* _temp1041=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1041->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1041->tl= 0;
_temp1041;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp1042=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1042->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1042->tl= 0;
_temp1042;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x))
Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1043=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1043->f1= 0; _temp1043->f2= e;
_temp1043;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){
void* _temp1044= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp1050;
_LL1046: if(( unsigned int) _temp1044 > 4u?*(( int*) _temp1044) == Cyc_Absyn_PointerType:
0){ _LL1051: _temp1050=(( struct Cyc_Absyn_PointerType_struct*) _temp1044)->f1;
goto _LL1047;} else{ goto _LL1048;} _LL1048: goto _LL1049; _LL1047: return
_temp1050; _LL1049:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("get_ptr_typ:  not a pointer!", sizeof( unsigned char), 29u))); _LL1045:;}
struct _tuple10{ void* f1; void* f2; } ; struct _tuple11{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp1052=( void*) e->r; goto _LL1053;
_LL1053: if( e->topt == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1054= Cyc_Absynpp_exp2string( e);
xprintf("exp_to_c: no type for %.*s", _temp1054.last_plus_one - _temp1054.curr,
_temp1054.curr);})); return;}{ void* old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; void* _temp1055= _temp1052; void* _temp1135; void*
_temp1137; void* _temp1139; struct _tuple0* _temp1141; struct _tuple0* _temp1143;
struct Cyc_List_List* _temp1145; void* _temp1147; void* _temp1149; struct Cyc_Absyn_Exp*
_temp1151; struct Cyc_Absyn_Exp* _temp1153; struct Cyc_Core_Opt* _temp1155;
struct Cyc_Absyn_Exp* _temp1157; struct Cyc_Absyn_Exp* _temp1159; struct Cyc_Absyn_Exp*
_temp1161; struct Cyc_Absyn_Exp* _temp1163; struct Cyc_Absyn_Exp* _temp1165;
struct Cyc_Absyn_Exp* _temp1167; struct Cyc_List_List* _temp1169; struct Cyc_Absyn_Exp*
_temp1171; struct Cyc_Absyn_VarargCallInfo* _temp1173; struct Cyc_List_List*
_temp1175; struct Cyc_Absyn_Exp* _temp1177; struct Cyc_Absyn_VarargCallInfo*
_temp1179; struct Cyc_Absyn_VarargCallInfo _temp1181; struct Cyc_Absyn_VarargInfo*
_temp1182; struct Cyc_List_List* _temp1184; int _temp1186; struct Cyc_List_List*
_temp1188; struct Cyc_Absyn_Exp* _temp1190; struct Cyc_Absyn_Exp* _temp1192;
struct Cyc_Absyn_Exp* _temp1194; struct Cyc_List_List* _temp1196; struct Cyc_Absyn_Exp*
_temp1198; struct Cyc_Absyn_Exp* _temp1200; void* _temp1202; void** _temp1204;
struct Cyc_Absyn_Exp* _temp1205; struct Cyc_Absyn_Exp* _temp1207; struct Cyc_Absyn_Exp*
_temp1209; struct Cyc_Absyn_Exp* _temp1211; void* _temp1213; struct _tagged_arr*
_temp1215; void* _temp1217; struct Cyc_Absyn_Exp* _temp1219; struct _tagged_arr*
_temp1221; struct Cyc_Absyn_Exp* _temp1223; struct _tagged_arr* _temp1225;
struct Cyc_Absyn_Exp* _temp1227; struct Cyc_Absyn_Exp* _temp1229; struct Cyc_Absyn_Exp*
_temp1231; struct Cyc_List_List* _temp1233; struct Cyc_List_List* _temp1235;
struct Cyc_Absyn_Exp* _temp1237; struct Cyc_Absyn_Exp* _temp1239; struct Cyc_Absyn_Vardecl*
_temp1241; struct Cyc_Absyn_Structdecl* _temp1243; struct Cyc_List_List*
_temp1245; struct Cyc_Core_Opt* _temp1247; struct _tuple0* _temp1249; struct Cyc_List_List*
_temp1251; void* _temp1253; struct Cyc_Absyn_Tunionfield* _temp1255; struct Cyc_Absyn_Tuniondecl*
_temp1257; struct Cyc_List_List* _temp1259; struct Cyc_Core_Opt* _temp1261;
struct Cyc_Core_Opt* _temp1263; struct Cyc_Absyn_Tunionfield* _temp1265; struct
Cyc_Absyn_Tuniondecl* _temp1267; struct Cyc_List_List* _temp1269; struct Cyc_Core_Opt*
_temp1271; struct Cyc_Core_Opt* _temp1273; struct Cyc_Absyn_Enumfield* _temp1275;
struct Cyc_Absyn_Enumdecl* _temp1277; struct _tuple0* _temp1279; void* _temp1281;
struct Cyc_Absyn_Exp* _temp1283; struct Cyc_Absyn_Stmt* _temp1285; struct Cyc_List_List*
_temp1287; struct Cyc_Core_Opt* _temp1289; struct Cyc_List_List* _temp1291;
struct _tuple1* _temp1293; struct Cyc_Absyn_Fndecl* _temp1295; struct Cyc_Absyn_Exp*
_temp1297; _LL1057: if(*(( int*) _temp1055) == Cyc_Absyn_Const_e){ _LL1136:
_temp1135=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1055)->f1; if(
_temp1135 ==( void*) Cyc_Absyn_Null_c){ goto _LL1058;} else{ goto _LL1059;}}
else{ goto _LL1059;} _LL1059: if(*(( int*) _temp1055) == Cyc_Absyn_Const_e){
_LL1138: _temp1137=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1055)->f1;
goto _LL1060;} else{ goto _LL1061;} _LL1061: if(*(( int*) _temp1055) == Cyc_Absyn_Var_e){
_LL1142: _temp1141=(( struct Cyc_Absyn_Var_e_struct*) _temp1055)->f1; goto
_LL1140; _LL1140: _temp1139=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1055)->f2;
goto _LL1062;} else{ goto _LL1063;} _LL1063: if(*(( int*) _temp1055) == Cyc_Absyn_UnknownId_e){
_LL1144: _temp1143=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1055)->f1; goto
_LL1064;} else{ goto _LL1065;} _LL1065: if(*(( int*) _temp1055) == Cyc_Absyn_Primop_e){
_LL1148: _temp1147=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1055)->f1;
goto _LL1146; _LL1146: _temp1145=(( struct Cyc_Absyn_Primop_e_struct*) _temp1055)->f2;
goto _LL1066;} else{ goto _LL1067;} _LL1067: if(*(( int*) _temp1055) == Cyc_Absyn_Increment_e){
_LL1152: _temp1151=(( struct Cyc_Absyn_Increment_e_struct*) _temp1055)->f1; goto
_LL1150; _LL1150: _temp1149=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1055)->f2; goto _LL1068;} else{ goto _LL1069;} _LL1069: if(*(( int*)
_temp1055) == Cyc_Absyn_AssignOp_e){ _LL1158: _temp1157=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1055)->f1; goto _LL1156; _LL1156: _temp1155=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1055)->f2; goto _LL1154; _LL1154: _temp1153=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1055)->f3; goto _LL1070;} else{ goto _LL1071;} _LL1071: if(*(( int*)
_temp1055) == Cyc_Absyn_Conditional_e){ _LL1164: _temp1163=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1055)->f1; goto _LL1162; _LL1162: _temp1161=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1055)->f2; goto _LL1160; _LL1160: _temp1159=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1055)->f3; goto _LL1072;} else{ goto _LL1073;} _LL1073: if(*(( int*)
_temp1055) == Cyc_Absyn_SeqExp_e){ _LL1168: _temp1167=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1055)->f1; goto _LL1166; _LL1166: _temp1165=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1055)->f2; goto _LL1074;} else{ goto _LL1075;} _LL1075: if(*(( int*)
_temp1055) == Cyc_Absyn_UnknownCall_e){ _LL1172: _temp1171=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1055)->f1; goto _LL1170; _LL1170: _temp1169=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1055)->f2; goto _LL1076;} else{ goto _LL1077;} _LL1077: if(*(( int*)
_temp1055) == Cyc_Absyn_FnCall_e){ _LL1178: _temp1177=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1055)->f1; goto _LL1176; _LL1176: _temp1175=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1055)->f2; goto _LL1174; _LL1174: _temp1173=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1055)->f3; if( _temp1173 == 0){ goto _LL1078;} else{ goto _LL1079;}} else{
goto _LL1079;} _LL1079: if(*(( int*) _temp1055) == Cyc_Absyn_FnCall_e){ _LL1191:
_temp1190=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1055)->f1; goto _LL1189;
_LL1189: _temp1188=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1055)->f2; goto
_LL1180; _LL1180: _temp1179=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1055)->f3;
if( _temp1179 == 0){ goto _LL1081;} else{ _temp1181=* _temp1179; _LL1187:
_temp1186= _temp1181.num_varargs; goto _LL1185; _LL1185: _temp1184= _temp1181.injectors;
goto _LL1183; _LL1183: _temp1182= _temp1181.vai; goto _LL1080;}} else{ goto
_LL1081;} _LL1081: if(*(( int*) _temp1055) == Cyc_Absyn_Throw_e){ _LL1193:
_temp1192=(( struct Cyc_Absyn_Throw_e_struct*) _temp1055)->f1; goto _LL1082;}
else{ goto _LL1083;} _LL1083: if(*(( int*) _temp1055) == Cyc_Absyn_NoInstantiate_e){
_LL1195: _temp1194=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1055)->f1;
goto _LL1084;} else{ goto _LL1085;} _LL1085: if(*(( int*) _temp1055) == Cyc_Absyn_Instantiate_e){
_LL1199: _temp1198=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1055)->f1;
goto _LL1197; _LL1197: _temp1196=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1055)->f2; goto _LL1086;} else{ goto _LL1087;} _LL1087: if(*(( int*)
_temp1055) == Cyc_Absyn_Cast_e){ _LL1203: _temp1202=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1055)->f1; _temp1204=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1055)->f1); goto _LL1201; _LL1201: _temp1200=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1055)->f2; goto _LL1088;} else{ goto _LL1089;} _LL1089: if(*(( int*)
_temp1055) == Cyc_Absyn_Address_e){ _LL1206: _temp1205=(( struct Cyc_Absyn_Address_e_struct*)
_temp1055)->f1; goto _LL1090;} else{ goto _LL1091;} _LL1091: if(*(( int*)
_temp1055) == Cyc_Absyn_New_e){ _LL1210: _temp1209=(( struct Cyc_Absyn_New_e_struct*)
_temp1055)->f1; goto _LL1208; _LL1208: _temp1207=(( struct Cyc_Absyn_New_e_struct*)
_temp1055)->f2; goto _LL1092;} else{ goto _LL1093;} _LL1093: if(*(( int*)
_temp1055) == Cyc_Absyn_Sizeofexp_e){ _LL1212: _temp1211=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1055)->f1; goto _LL1094;} else{ goto _LL1095;} _LL1095: if(*(( int*)
_temp1055) == Cyc_Absyn_Sizeoftyp_e){ _LL1214: _temp1213=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1055)->f1; goto _LL1096;} else{ goto _LL1097;} _LL1097: if(*(( int*)
_temp1055) == Cyc_Absyn_Offsetof_e){ _LL1218: _temp1217=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1055)->f1; goto _LL1216; _LL1216: _temp1215=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1055)->f2; goto _LL1098;} else{ goto _LL1099;} _LL1099: if(*(( int*)
_temp1055) == Cyc_Absyn_Deref_e){ _LL1220: _temp1219=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1055)->f1; goto _LL1100;} else{ goto _LL1101;} _LL1101: if(*(( int*)
_temp1055) == Cyc_Absyn_StructMember_e){ _LL1224: _temp1223=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1055)->f1; goto _LL1222; _LL1222: _temp1221=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1055)->f2; goto _LL1102;} else{ goto _LL1103;} _LL1103: if(*(( int*)
_temp1055) == Cyc_Absyn_StructArrow_e){ _LL1228: _temp1227=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1055)->f1; goto _LL1226; _LL1226: _temp1225=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1055)->f2; goto _LL1104;} else{ goto _LL1105;} _LL1105: if(*(( int*)
_temp1055) == Cyc_Absyn_Subscript_e){ _LL1232: _temp1231=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1055)->f1; goto _LL1230; _LL1230: _temp1229=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1055)->f2; goto _LL1106;} else{ goto _LL1107;} _LL1107: if(*(( int*)
_temp1055) == Cyc_Absyn_Tuple_e){ _LL1234: _temp1233=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1055)->f1; goto _LL1108;} else{ goto _LL1109;} _LL1109: if(*(( int*)
_temp1055) == Cyc_Absyn_Array_e){ _LL1236: _temp1235=(( struct Cyc_Absyn_Array_e_struct*)
_temp1055)->f1; goto _LL1110;} else{ goto _LL1111;} _LL1111: if(*(( int*)
_temp1055) == Cyc_Absyn_Comprehension_e){ _LL1242: _temp1241=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1055)->f1; goto _LL1240; _LL1240: _temp1239=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1055)->f2; goto _LL1238; _LL1238: _temp1237=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1055)->f3; goto _LL1112;} else{ goto _LL1113;} _LL1113: if(*(( int*)
_temp1055) == Cyc_Absyn_Struct_e){ _LL1250: _temp1249=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1055)->f1; goto _LL1248; _LL1248: _temp1247=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1055)->f2; goto _LL1246; _LL1246: _temp1245=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1055)->f3; goto _LL1244; _LL1244: _temp1243=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1055)->f4; goto _LL1114;} else{ goto _LL1115;} _LL1115: if(*(( int*)
_temp1055) == Cyc_Absyn_AnonStruct_e){ _LL1254: _temp1253=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1055)->f1; goto _LL1252; _LL1252: _temp1251=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1055)->f2; goto _LL1116;} else{ goto _LL1117;} _LL1117: if(*(( int*)
_temp1055) == Cyc_Absyn_Tunion_e){ _LL1264: _temp1263=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1055)->f1; goto _LL1262; _LL1262: _temp1261=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1055)->f2; goto _LL1260; _LL1260: _temp1259=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1055)->f3; if( _temp1259 == 0){ goto _LL1258;} else{ goto _LL1119;} _LL1258:
_temp1257=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1055)->f4; goto _LL1256;
_LL1256: _temp1255=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1055)->f5; goto
_LL1118;} else{ goto _LL1119;} _LL1119: if(*(( int*) _temp1055) == Cyc_Absyn_Tunion_e){
_LL1274: _temp1273=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1055)->f1; goto
_LL1272; _LL1272: _temp1271=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1055)->f2;
goto _LL1270; _LL1270: _temp1269=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1055)->f3;
goto _LL1268; _LL1268: _temp1267=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1055)->f4;
goto _LL1266; _LL1266: _temp1265=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1055)->f5;
goto _LL1120;} else{ goto _LL1121;} _LL1121: if(*(( int*) _temp1055) == Cyc_Absyn_Enum_e){
_LL1280: _temp1279=(( struct Cyc_Absyn_Enum_e_struct*) _temp1055)->f1; goto
_LL1278; _LL1278: _temp1277=(( struct Cyc_Absyn_Enum_e_struct*) _temp1055)->f2;
goto _LL1276; _LL1276: _temp1275=(( struct Cyc_Absyn_Enum_e_struct*) _temp1055)->f3;
goto _LL1122;} else{ goto _LL1123;} _LL1123: if(*(( int*) _temp1055) == Cyc_Absyn_Malloc_e){
_LL1284: _temp1283=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1055)->f1; goto
_LL1282; _LL1282: _temp1281=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1055)->f2; goto _LL1124;} else{ goto _LL1125;} _LL1125: if(*(( int*)
_temp1055) == Cyc_Absyn_StmtExp_e){ _LL1286: _temp1285=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1055)->f1; goto _LL1126;} else{ goto _LL1127;} _LL1127: if(*(( int*)
_temp1055) == Cyc_Absyn_UnresolvedMem_e){ _LL1290: _temp1289=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1055)->f1; goto _LL1288; _LL1288: _temp1287=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1055)->f2; goto _LL1128;} else{ goto _LL1129;} _LL1129: if(*(( int*)
_temp1055) == Cyc_Absyn_CompoundLit_e){ _LL1294: _temp1293=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1055)->f1; goto _LL1292; _LL1292: _temp1291=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1055)->f2; goto _LL1130;} else{ goto _LL1131;} _LL1131: if(*(( int*)
_temp1055) == Cyc_Absyn_Codegen_e){ _LL1296: _temp1295=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1055)->f1; goto _LL1132;} else{ goto _LL1133;} _LL1133: if(*(( int*)
_temp1055) == Cyc_Absyn_Fill_e){ _LL1298: _temp1297=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1055)->f1; goto _LL1134;} else{ goto _LL1056;} _LL1058:{ void* _temp1299=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1305; struct Cyc_Absyn_Conref*
_temp1307; struct Cyc_Absyn_Tqual _temp1309; struct Cyc_Absyn_Conref* _temp1311;
void* _temp1313; void* _temp1315; _LL1301: if(( unsigned int) _temp1299 > 4u?*((
int*) _temp1299) == Cyc_Absyn_PointerType: 0){ _LL1306: _temp1305=(( struct Cyc_Absyn_PointerType_struct*)
_temp1299)->f1; _LL1316: _temp1315=( void*) _temp1305.elt_typ; goto _LL1314;
_LL1314: _temp1313=( void*) _temp1305.rgn_typ; goto _LL1312; _LL1312: _temp1311=
_temp1305.nullable; goto _LL1310; _LL1310: _temp1309= _temp1305.tq; goto _LL1308;
_LL1308: _temp1307= _temp1305.bounds; goto _LL1302;} else{ goto _LL1303;}
_LL1303: goto _LL1304; _LL1302:{ void* _temp1317=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1307); struct Cyc_Absyn_Exp*
_temp1323; _LL1319: if( _temp1317 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1320;}
else{ goto _LL1321;} _LL1321: if(( unsigned int) _temp1317 > 1u?*(( int*)
_temp1317) == Cyc_Absyn_Upper_b: 0){ _LL1324: _temp1323=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1317)->f1; goto _LL1322;} else{ goto _LL1318;} _LL1320: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, Cyc_Absyn_uint_exp(
0, 0), Cyc_Absyn_uint_exp( 0, 0)))->r));} else{( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__tag_arr_qv, 0),({ struct Cyc_List_List*(*
_temp1325)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp1328= Cyc_Absyn_uint_exp( 0, 0);
struct Cyc_Absyn_Exp* _temp1329= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
_temp1315), 0); struct Cyc_Absyn_Exp* _temp1330= Cyc_Absyn_uint_exp( 0, 0);
struct Cyc_Absyn_Exp* _temp1326[ 3u]={ _temp1328, _temp1329, _temp1330}; struct
_tagged_arr _temp1327={( void*) _temp1326,( void*) _temp1326,( void*)( _temp1326
+ 3u)}; _temp1325( _temp1327);}), 0))->r));} goto _LL1318; _LL1322:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto _LL1318; _LL1318:;}
goto _LL1300; _LL1304:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1331= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1331.last_plus_one -
_temp1331.curr, _temp1331.curr);})); goto _LL1300; _LL1300:;} goto _LL1056;
_LL1060: goto _LL1056; _LL1062:{ struct _handler_cons _temp1332; _push_handler(&
_temp1332);{ int _temp1334= 0; if( setjmp( _temp1332.handler)){ _temp1334= 1;}
if( ! _temp1334){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1141))->r));; _pop_handler();} else{ void* _temp1333=( void*) _exn_thrown;
void* _temp1336= _temp1333; _LL1338: if( _temp1336 == Cyc_Dict_Absent){ goto
_LL1339;} else{ goto _LL1340;} _LL1340: goto _LL1341; _LL1339:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp1342= Cyc_Absynpp_qvar2string( _temp1141); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1342.last_plus_one - _temp1342.curr, _temp1342.curr);})); return; _LL1341:(
void) _throw( _temp1336); _LL1337:;}}} goto _LL1056; _LL1064:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char),
10u)); goto _LL1056; _LL1066: { struct Cyc_List_List* _temp1343=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1145); goto _LL1344; _LL1344: { struct Cyc_List_List* _temp1345=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Toc_get_c_typ, _temp1145); goto _LL1346; _LL1346:(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1145);{
void* _temp1347= _temp1147; _LL1349: if( _temp1347 ==( void*) Cyc_Absyn_Size){
goto _LL1350;} else{ goto _LL1351;} _LL1351: if( _temp1347 ==( void*) Cyc_Absyn_Printf){
goto _LL1352;} else{ goto _LL1353;} _LL1353: if( _temp1347 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1354;} else{ goto _LL1355;} _LL1355: if( _temp1347 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1356;} else{ goto _LL1357;} _LL1357: if( _temp1347 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1358;} else{ goto _LL1359;} _LL1359: if( _temp1347 ==( void*) Cyc_Absyn_Plus){
goto _LL1360;} else{ goto _LL1361;} _LL1361: if( _temp1347 ==( void*) Cyc_Absyn_Minus){
goto _LL1362;} else{ goto _LL1363;} _LL1363: if( _temp1347 ==( void*) Cyc_Absyn_Eq){
goto _LL1364;} else{ goto _LL1365;} _LL1365: if( _temp1347 ==( void*) Cyc_Absyn_Neq){
goto _LL1366;} else{ goto _LL1367;} _LL1367: if( _temp1347 ==( void*) Cyc_Absyn_Gt){
goto _LL1368;} else{ goto _LL1369;} _LL1369: if( _temp1347 ==( void*) Cyc_Absyn_Gte){
goto _LL1370;} else{ goto _LL1371;} _LL1371: if( _temp1347 ==( void*) Cyc_Absyn_Lt){
goto _LL1372;} else{ goto _LL1373;} _LL1373: if( _temp1347 ==( void*) Cyc_Absyn_Lte){
goto _LL1374;} else{ goto _LL1375;} _LL1375: goto _LL1376; _LL1350: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1145))->hd;{
void* _temp1377=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1385; struct Cyc_Absyn_Tqual _temp1387; void*
_temp1389; struct Cyc_Absyn_PtrInfo _temp1391; struct Cyc_Absyn_Conref*
_temp1393; struct Cyc_Absyn_Tqual _temp1395; struct Cyc_Absyn_Conref* _temp1397;
void* _temp1399; void* _temp1401; _LL1379: if(( unsigned int) _temp1377 > 4u?*((
int*) _temp1377) == Cyc_Absyn_ArrayType: 0){ _LL1390: _temp1389=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1377)->f1; goto _LL1388; _LL1388:
_temp1387=(( struct Cyc_Absyn_ArrayType_struct*) _temp1377)->f2; goto _LL1386;
_LL1386: _temp1385=(( struct Cyc_Absyn_ArrayType_struct*) _temp1377)->f3; goto
_LL1380;} else{ goto _LL1381;} _LL1381: if(( unsigned int) _temp1377 > 4u?*((
int*) _temp1377) == Cyc_Absyn_PointerType: 0){ _LL1392: _temp1391=(( struct Cyc_Absyn_PointerType_struct*)
_temp1377)->f1; _LL1402: _temp1401=( void*) _temp1391.elt_typ; goto _LL1400;
_LL1400: _temp1399=( void*) _temp1391.rgn_typ; goto _LL1398; _LL1398: _temp1397=
_temp1391.nullable; goto _LL1396; _LL1396: _temp1395= _temp1391.tq; goto _LL1394;
_LL1394: _temp1393= _temp1391.bounds; goto _LL1382;} else{ goto _LL1383;}
_LL1383: goto _LL1384; _LL1380:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1385))->r)); goto _LL1378; _LL1382:{ void* _temp1403=(( void*(*)(
void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1393); struct Cyc_Absyn_Exp* _temp1409; _LL1405: if( _temp1403 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1406;} else{ goto _LL1407;} _LL1407: if((
unsigned int) _temp1403 > 1u?*(( int*) _temp1403) == Cyc_Absyn_Upper_b: 0){
_LL1410: _temp1409=(( struct Cyc_Absyn_Upper_b_struct*) _temp1403)->f1; goto
_LL1408;} else{ goto _LL1404;} _LL1406:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__get_arr_size_qv, 0),({ struct Cyc_List_List*(*
_temp1411)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp1414=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1145))->hd; struct Cyc_Absyn_Exp* _temp1415=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1401), 0); struct Cyc_Absyn_Exp*
_temp1412[ 2u]={ _temp1414, _temp1415}; struct _tagged_arr _temp1413={( void*)
_temp1412,( void*) _temp1412,( void*)( _temp1412 + 2u)}; _temp1411( _temp1413);}),
0))->r)); goto _LL1404; _LL1408:( void*)( e->r=( void*)(( void*) _temp1409->r));
goto _LL1404; _LL1404:;} goto _LL1378; _LL1384:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr _temp1416= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_List_List*) _check_null( _temp1345))->hd); struct
_tagged_arr _temp1417= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1416.last_plus_one - _temp1416.curr, _temp1416.curr, _temp1417.last_plus_one
- _temp1417.curr, _temp1417.curr);})); return; _LL1378:;} goto _LL1348;} _LL1352:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1147, 0, _temp1145,
_temp1345))->r)); goto _LL1348; _LL1354:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1147, 0, _temp1145, _temp1345))->r)); goto _LL1348; _LL1356: if( _temp1145
== 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("fprintf without arguments",
sizeof( unsigned char), 26u));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1147,({ struct Cyc_Core_Opt* _temp1418=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1418->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1145))->hd); _temp1418;}),(( struct Cyc_List_List*)
_check_null( _temp1145))->tl,(( struct Cyc_List_List*) _check_null( _temp1345))->tl))->r));
goto _LL1348; _LL1358: if( _temp1145 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("sscanf without arguments", sizeof( unsigned char), 25u));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1145))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1145))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1145))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1348; _LL1360:{ void* _temp1419= Cyc_Tcutil_compress((
void*)(( struct Cyc_List_List*) _check_null( _temp1343))->hd); struct Cyc_Absyn_PtrInfo
_temp1425; struct Cyc_Absyn_Conref* _temp1427; struct Cyc_Absyn_Tqual _temp1429;
struct Cyc_Absyn_Conref* _temp1431; void* _temp1433; void* _temp1435; _LL1421:
if(( unsigned int) _temp1419 > 4u?*(( int*) _temp1419) == Cyc_Absyn_PointerType:
0){ _LL1426: _temp1425=(( struct Cyc_Absyn_PointerType_struct*) _temp1419)->f1;
_LL1436: _temp1435=( void*) _temp1425.elt_typ; goto _LL1434; _LL1434: _temp1433=(
void*) _temp1425.rgn_typ; goto _LL1432; _LL1432: _temp1431= _temp1425.nullable;
goto _LL1430; _LL1430: _temp1429= _temp1425.tq; goto _LL1428; _LL1428: _temp1427=
_temp1425.bounds; goto _LL1422;} else{ goto _LL1423;} _LL1423: goto _LL1424;
_LL1422:{ void* _temp1437=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1427); _LL1439: if( _temp1437 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1440;} else{ goto _LL1441;} _LL1441: goto _LL1442; _LL1440: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1145))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1145))->tl))->hd;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__tagged_arr_plus_qv,
0),({ struct Cyc_List_List*(* _temp1443)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1446= e1; struct
Cyc_Absyn_Exp* _temp1447= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1435),
0); struct Cyc_Absyn_Exp* _temp1448= e2; struct Cyc_Absyn_Exp* _temp1444[ 3u]={
_temp1446, _temp1447, _temp1448}; struct _tagged_arr _temp1445={( void*)
_temp1444,( void*) _temp1444,( void*)( _temp1444 + 3u)}; _temp1443( _temp1445);}),
0))->r)); goto _LL1438;} _LL1442: goto _LL1438; _LL1438:;} goto _LL1420; _LL1424:
goto _LL1420; _LL1420:;} goto _LL1348; _LL1362:{ void* _temp1449= Cyc_Tcutil_compress((
void*)(( struct Cyc_List_List*) _check_null( _temp1343))->hd); struct Cyc_Absyn_PtrInfo
_temp1455; struct Cyc_Absyn_Conref* _temp1457; struct Cyc_Absyn_Tqual _temp1459;
struct Cyc_Absyn_Conref* _temp1461; void* _temp1463; void* _temp1465; _LL1451:
if(( unsigned int) _temp1449 > 4u?*(( int*) _temp1449) == Cyc_Absyn_PointerType:
0){ _LL1456: _temp1455=(( struct Cyc_Absyn_PointerType_struct*) _temp1449)->f1;
_LL1466: _temp1465=( void*) _temp1455.elt_typ; goto _LL1464; _LL1464: _temp1463=(
void*) _temp1455.rgn_typ; goto _LL1462; _LL1462: _temp1461= _temp1455.nullable;
goto _LL1460; _LL1460: _temp1459= _temp1455.tq; goto _LL1458; _LL1458: _temp1457=
_temp1455.bounds; goto _LL1452;} else{ goto _LL1453;} _LL1453: goto _LL1454;
_LL1452:{ void* _temp1467=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1457); _LL1469: if( _temp1467 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1470;} else{ goto _LL1471;} _LL1471: goto _LL1472; _LL1470: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1145))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1145))->tl))->hd; if(
Cyc_Tcutil_is_tagged_pointer_typ(( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1343))->tl))->hd)){( void*)( e1->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e1->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} else{( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__tagged_arr_plus_qv,
0),({ struct Cyc_List_List*(* _temp1473)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1476= e1; struct
Cyc_Absyn_Exp* _temp1477= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1465),
0); struct Cyc_Absyn_Exp* _temp1478= Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus,
e2, 0); struct Cyc_Absyn_Exp* _temp1474[ 3u]={ _temp1476, _temp1477, _temp1478};
struct _tagged_arr _temp1475={( void*) _temp1474,( void*) _temp1474,( void*)(
_temp1474 + 3u)}; _temp1473( _temp1475);}), 0))->r));} goto _LL1468;} _LL1472:
goto _LL1468; _LL1468:;} goto _LL1450; _LL1454: goto _LL1450; _LL1450:;} goto
_LL1348; _LL1364: goto _LL1366; _LL1366: goto _LL1368; _LL1368: goto _LL1370;
_LL1370: goto _LL1372; _LL1372: goto _LL1374; _LL1374: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1145))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1145))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1343))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1343))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1348;} _LL1376: goto
_LL1348; _LL1348:;} goto _LL1056;}} _LL1068: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1151->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1151);{ void* _temp1479= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1485; struct Cyc_Absyn_Conref* _temp1487; struct Cyc_Absyn_Tqual _temp1489;
struct Cyc_Absyn_Conref* _temp1491; void* _temp1493; void* _temp1495; _LL1481:
if(( unsigned int) _temp1479 > 4u?*(( int*) _temp1479) == Cyc_Absyn_PointerType:
0){ _LL1486: _temp1485=(( struct Cyc_Absyn_PointerType_struct*) _temp1479)->f1;
_LL1496: _temp1495=( void*) _temp1485.elt_typ; goto _LL1494; _LL1494: _temp1493=(
void*) _temp1485.rgn_typ; goto _LL1492; _LL1492: _temp1491= _temp1485.nullable;
goto _LL1490; _LL1490: _temp1489= _temp1485.tq; goto _LL1488; _LL1488: _temp1487=
_temp1485.bounds; goto _LL1482;} else{ goto _LL1483;} _LL1483: goto _LL1484;
_LL1482:{ void* _temp1497=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1487); _LL1499: if( _temp1497 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1500;} else{ goto _LL1501;} _LL1501: goto _LL1502; _LL1500: { struct
_tuple0* _temp1503=( _temp1149 ==( void*) Cyc_Absyn_PostInc? 1: _temp1149 ==(
void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_qv: Cyc_Toc__tagged_arr_inplace_plus_qv;
goto _LL1504; _LL1504: { int _temp1505=( _temp1149 ==( void*) Cyc_Absyn_PreInc?
1: _temp1149 ==( void*) Cyc_Absyn_PostInc)? 1: - 1; goto _LL1506; _LL1506:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( _temp1503, 0),({
struct Cyc_List_List*(* _temp1507)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1510= Cyc_Absyn_address_exp(
_temp1151, 0); struct Cyc_Absyn_Exp* _temp1511= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
_temp1495), 0); struct Cyc_Absyn_Exp* _temp1512= Cyc_Absyn_signed_int_exp(
_temp1505, 0); struct Cyc_Absyn_Exp* _temp1508[ 3u]={ _temp1510, _temp1511,
_temp1512}; struct _tagged_arr _temp1509={( void*) _temp1508,( void*) _temp1508,(
void*)( _temp1508 + 3u)}; _temp1507( _temp1509);}), 0))->r)); goto _LL1498;}}
_LL1502: goto _LL1498; _LL1498:;} goto _LL1480; _LL1484: if( ! Cyc_Absyn_is_lvalue(
_temp1151)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1151, 0, Cyc_Toc_incr_lvalue, _temp1149);( void*)( e->r=( void*)(( void*)
_temp1151->r));} goto _LL1480; _LL1480:;} goto _LL1056;} _LL1070: { int e1_poly=
Cyc_Toc_is_poly_project( _temp1157); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1157->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1153->topt))->v; Cyc_Toc_exp_to_c( nv, _temp1157); Cyc_Toc_exp_to_c(
nv, _temp1153);{ int done= 0; if( _temp1155 != 0){ void* _temp1513= Cyc_Tcutil_compress(
e1_old_typ); struct Cyc_Absyn_PtrInfo _temp1519; struct Cyc_Absyn_Conref*
_temp1521; struct Cyc_Absyn_Tqual _temp1523; struct Cyc_Absyn_Conref* _temp1525;
void* _temp1527; void* _temp1529; _LL1515: if(( unsigned int) _temp1513 > 4u?*((
int*) _temp1513) == Cyc_Absyn_PointerType: 0){ _LL1520: _temp1519=(( struct Cyc_Absyn_PointerType_struct*)
_temp1513)->f1; _LL1530: _temp1529=( void*) _temp1519.elt_typ; goto _LL1528;
_LL1528: _temp1527=( void*) _temp1519.rgn_typ; goto _LL1526; _LL1526: _temp1525=
_temp1519.nullable; goto _LL1524; _LL1524: _temp1523= _temp1519.tq; goto _LL1522;
_LL1522: _temp1521= _temp1519.bounds; goto _LL1516;} else{ goto _LL1517;}
_LL1517: goto _LL1518; _LL1516:{ void* _temp1531=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1521); _LL1533: if(
_temp1531 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1534;} else{ goto _LL1535;}
_LL1535: goto _LL1536; _LL1534: { struct Cyc_Absyn_Exp* change;{ void* _temp1537=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1155))->v; _LL1539: if(
_temp1537 ==( void*) Cyc_Absyn_Plus){ goto _LL1540;} else{ goto _LL1541;}
_LL1541: if( _temp1537 ==( void*) Cyc_Absyn_Minus){ goto _LL1542;} else{ goto
_LL1543;} _LL1543: goto _LL1544; _LL1540: change= _temp1153; goto _LL1538;
_LL1542: change= Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp1153, 0);
goto _LL1538; _LL1544:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("bad t ? pointer arithmetic", sizeof( unsigned char), 27u))); goto
_LL1538; _LL1538:;} done= 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__tagged_arr_inplace_plus_qv, 0),({ struct Cyc_List_List*(*
_temp1545)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp1548= Cyc_Absyn_address_exp( _temp1157,
0); struct Cyc_Absyn_Exp* _temp1549= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
_temp1529), 0); struct Cyc_Absyn_Exp* _temp1550= change; struct Cyc_Absyn_Exp*
_temp1546[ 3u]={ _temp1548, _temp1549, _temp1550}; struct _tagged_arr _temp1547={(
void*) _temp1546,( void*) _temp1546,( void*)( _temp1546 + 3u)}; _temp1545(
_temp1547);}), 0))->r)); goto _LL1532;} _LL1536: goto _LL1532; _LL1532:;} goto
_LL1514; _LL1518: goto _LL1514; _LL1514:;} if( ! done){ if( e1_poly){( void*)(
_temp1153->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1153->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1157)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple9*), struct
_tuple9* f_env)) Cyc_Toc_lvalue_assign)( _temp1157, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple9* _temp1551=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9)
* 1); _temp1551[ 0]=({ struct _tuple9 _temp1552; _temp1552.f1= _temp1155;
_temp1552.f2= _temp1153; _temp1552;}); _temp1551;}));( void*)( e->r=( void*)((
void*) _temp1157->r));}} goto _LL1056;}} _LL1072: Cyc_Toc_exp_to_c( nv,
_temp1163); Cyc_Toc_exp_to_c( nv, _temp1161); Cyc_Toc_exp_to_c( nv, _temp1159);
goto _LL1056; _LL1074: Cyc_Toc_exp_to_c( nv, _temp1167); Cyc_Toc_exp_to_c( nv,
_temp1165); goto _LL1056; _LL1076: _temp1177= _temp1171; _temp1175= _temp1169;
goto _LL1078; _LL1078: Cyc_Toc_exp_to_c( nv, _temp1177);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1175); goto _LL1056; _LL1080:{
struct _RegionHandle _temp1553= _new_region(); struct _RegionHandle* r=&
_temp1553; _push_region( r);{ void* _temp1554=( void*) _temp1182->type; goto
_LL1555; _LL1555: { void* _temp1556= Cyc_Toc_typ_to_c( _temp1554); goto _LL1557;
_LL1557: { void* _temp1558= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1190->topt))->v); goto _LL1559; _LL1559: { struct Cyc_Absyn_Exp*
_temp1560= Cyc_Absyn_uint_exp(( unsigned int) _temp1186, 0); goto _LL1561;
_LL1561: { void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1628=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1628[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1629; _temp1629.tag= Cyc_Absyn_ArrayType;
_temp1629.f1=( void*) _temp1554; _temp1629.f2= Cyc_Toc_mt_tq; _temp1629.f3=(
struct Cyc_Absyn_Exp*) _temp1560; _temp1629;}); _temp1628;})); struct _tuple0*
_temp1562= Cyc_Toc_temp_var(); goto _LL1563; _LL1563: { struct _tuple0*
_temp1564= Cyc_Toc_temp_var(); goto _LL1565; _LL1565: { struct Cyc_Absyn_Exp*
_temp1566= Cyc_Absyn_var_exp( _temp1564, 0); goto _LL1567; _LL1567: { struct
_tuple0* _temp1568= Cyc_Toc_temp_var(); goto _LL1569; _LL1569: { int _temp1570=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1188); goto _LL1571;
_LL1571: { int _temp1572= _temp1570 - _temp1186; goto _LL1573; _LL1573: { struct
_tagged_arr vs=({ unsigned int _temp1624=( unsigned int) _temp1186; struct
_tuple0** _temp1625=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1624); struct _tagged_arr _temp1627= _tag_arr( _temp1625,
sizeof( struct _tuple0*),( unsigned int) _temp1186);{ unsigned int _temp1626=
_temp1624; unsigned int i; for( i= 0; i < _temp1626; i ++){ _temp1625[ i]= Cyc_Toc_temp_var();}};
_temp1627;}); struct _tagged_arr xs=({ unsigned int _temp1620=( unsigned int)
_temp1572; struct _tuple0** _temp1621=( struct _tuple0**) _region_malloc( r,
sizeof( struct _tuple0*) * _temp1620); struct _tagged_arr _temp1623= _tag_arr(
_temp1621, sizeof( struct _tuple0*),( unsigned int) _temp1572);{ unsigned int
_temp1622= _temp1620; unsigned int i; for( i= 0; i < _temp1622; i ++){ _temp1621[
i]= Cyc_Toc_temp_var();}}; _temp1623;}); struct Cyc_List_List* xexps=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,(
struct Cyc_Position_Segment*) 0,(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, xs));
xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List* _temp1574=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1574->hd=( void*) Cyc_Absyn_var_exp(
_temp1568, 0); _temp1574->tl= 0; _temp1574;}));{ struct Cyc_Absyn_Stmt*
_temp1575= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
_temp1562, 0), xexps, 0), 0); goto _LL1576; _LL1576: { struct Cyc_Absyn_Exp*
_temp1577= Cyc_Absyn_add_exp( _temp1566, _temp1560, 0); goto _LL1578; _LL1578: {
struct Cyc_List_List* _temp1588=({ struct Cyc_List_List*(* _temp1579)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct _tuple4* _temp1582=({ struct _tuple4* _temp1587=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1587->f1= 0; _temp1587->f2= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1566, 0); _temp1587;}); struct _tuple4*
_temp1583=({ struct _tuple4* _temp1586=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1586->f1= 0; _temp1586->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1566, 0); _temp1586;}); struct _tuple4* _temp1584=({ struct _tuple4*
_temp1585=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1585->f1=
0; _temp1585->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1577, 0);
_temp1585;}); struct _tuple4* _temp1580[ 3u]={ _temp1582, _temp1583, _temp1584};
struct _tagged_arr _temp1581={( void*) _temp1580,( void*) _temp1580,( void*)(
_temp1580 + 3u)}; _temp1579( _temp1581);}); goto _LL1589; _LL1589: _temp1575=
Cyc_Absyn_declare_stmt( _temp1568, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp1554,( void*) _temp1182->rgn, Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp(
0, _temp1588, 0), _temp1575, 0);{ struct Cyc_List_List* _temp1590=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, vs));
goto _LL1591; _LL1591: { struct Cyc_List_List* _temp1592=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_make_dle, _temp1590); goto _LL1593; _LL1593: _temp1575= Cyc_Absyn_declare_stmt(
_temp1564, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1592, 0), _temp1575, 0);{ struct Cyc_List_List* _temp1594=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1188);
goto _LL1595; _LL1595: { struct Cyc_List_List* _temp1596=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1184);
goto _LL1597; _LL1597:{ int i= _temp1186 - 1; for( 0; i >= 0;( i --, _temp1594=((
struct Cyc_List_List*) _check_null( _temp1594))->tl)){ struct Cyc_Absyn_Exp* arg=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1594))->hd;
void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c(
nv, arg); if( _temp1182->inject){ void* _temp1600; struct Cyc_Position_Segment*
_temp1602; struct Cyc_List_List* _temp1604; struct Cyc_List_List* _temp1606;
struct _tuple0* _temp1608; struct Cyc_Absyn_Tunionfield _temp1598=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1596))->hd); _LL1609: _temp1608=
_temp1598.name; goto _LL1607; _LL1607: _temp1606= _temp1598.tvs; goto _LL1605;
_LL1605: _temp1604= _temp1598.typs; goto _LL1603; _LL1603: _temp1602= _temp1598.loc;
goto _LL1601; _LL1601: _temp1600=( void*) _temp1598.sc; goto _LL1599; _LL1599:
_temp1596=(( struct Cyc_List_List*) _check_null( _temp1596))->tl;{ struct
_tuple0* _temp1610= Cyc_Toc_temp_var(); goto _LL1611; _LL1611: { struct Cyc_Absyn_Exp*
_temp1612= Cyc_Absyn_var_exp( _temp1610, 0); goto _LL1613; _LL1613: { void*
_temp1614= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1608, _tag_arr("_struct",
sizeof( unsigned char), 8u))); goto _LL1615; _LL1615: _temp1575= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1556,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1556, Cyc_Absyn_address_exp(
_temp1612, 0), 0), _temp1575, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple3*)(( struct Cyc_List_List*) _check_null( _temp1604))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1616= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1612, Cyc_Toc_fieldname(
1), 0), arg, 0); goto _LL1617; _LL1617: _temp1575= Cyc_Absyn_seq_stmt( _temp1616,
_temp1575, 0);{ struct Cyc_Absyn_Stmt* _temp1618= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1612, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1608, 0), 0); goto
_LL1619; _LL1619: _temp1575= Cyc_Absyn_seq_stmt( _temp1618, _temp1575, 0);
_temp1575= Cyc_Absyn_declare_stmt( _temp1610, _temp1614, 0, _temp1575, 0);}}}}}}}
else{ _temp1575= Cyc_Absyn_declare_stmt(*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)), _temp1556,( struct
Cyc_Absyn_Exp*) arg, _temp1575, 0);}}}{ int i= _temp1572 - 1; for( 0; i >= 0;( i
--, _temp1594=(( struct Cyc_List_List*) _check_null( _temp1594))->tl)){ struct
Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1594))->hd; void* argtype= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); Cyc_Toc_exp_to_c( nv, arg); _temp1575= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( xs, sizeof( struct _tuple0*), i)),
argtype,( struct Cyc_Absyn_Exp*) arg, _temp1575, 0);}} Cyc_Toc_exp_to_c( nv,
_temp1190); _temp1575= Cyc_Absyn_declare_stmt( _temp1562, _temp1558,( struct Cyc_Absyn_Exp*)
_temp1190, _temp1575, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1575, 0))->r));}}}}}}}}}}}}}}}}}}; _pop_region(& _temp1553);} goto _LL1056;
_LL1082: Cyc_Toc_exp_to_c( nv, _temp1192);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1192), 0))->r)); goto
_LL1056; _LL1084: Cyc_Toc_exp_to_c( nv, _temp1194); goto _LL1056; _LL1086: Cyc_Toc_exp_to_c(
nv, _temp1198);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); for( 0; _temp1196 != 0; _temp1196=(( struct Cyc_List_List*)
_check_null( _temp1196))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct
Cyc_List_List*) _check_null( _temp1196))->hd); if( k !=( void*) Cyc_Absyn_EffKind?
k !=( void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1198, 0))->r)); break;}} goto _LL1056;} _LL1088: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1200->topt))->v; void* new_typ=*
_temp1204;* _temp1204= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1200);{ struct _tuple10 _temp1631=({ struct _tuple10 _temp1630; _temp1630.f1=
Cyc_Tcutil_compress( old_t2); _temp1630.f2= Cyc_Tcutil_compress( new_typ);
_temp1630;}); void* _temp1639; struct Cyc_Absyn_PtrInfo _temp1641; void*
_temp1643; struct Cyc_Absyn_PtrInfo _temp1645; void* _temp1647; void* _temp1649;
void* _temp1651; void* _temp1653; struct Cyc_Absyn_PtrInfo _temp1655; _LL1633:
_LL1644: _temp1643= _temp1631.f1; if(( unsigned int) _temp1643 > 4u?*(( int*)
_temp1643) == Cyc_Absyn_PointerType: 0){ _LL1646: _temp1645=(( struct Cyc_Absyn_PointerType_struct*)
_temp1643)->f1; goto _LL1640;} else{ goto _LL1635;} _LL1640: _temp1639=
_temp1631.f2; if(( unsigned int) _temp1639 > 4u?*(( int*) _temp1639) == Cyc_Absyn_PointerType:
0){ _LL1642: _temp1641=(( struct Cyc_Absyn_PointerType_struct*) _temp1639)->f1;
goto _LL1634;} else{ goto _LL1635;} _LL1635: _LL1654: _temp1653= _temp1631.f1;
if(( unsigned int) _temp1653 > 4u?*(( int*) _temp1653) == Cyc_Absyn_PointerType:
0){ _LL1656: _temp1655=(( struct Cyc_Absyn_PointerType_struct*) _temp1653)->f1;
goto _LL1648;} else{ goto _LL1637;} _LL1648: _temp1647= _temp1631.f2; if((
unsigned int) _temp1647 > 4u?*(( int*) _temp1647) == Cyc_Absyn_IntType: 0){
_LL1652: _temp1651=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1647)->f1;
goto _LL1650; _LL1650: _temp1649=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1647)->f2; goto _LL1636;} else{ goto _LL1637;} _LL1637: goto _LL1638;
_LL1634: { int _temp1657=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1645.nullable); goto _LL1658; _LL1658: { int _temp1659=(( int(*)( int,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1641.nullable); goto
_LL1660; _LL1660: { void* _temp1661=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1645.bounds); goto _LL1662;
_LL1662: { void* _temp1663=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1641.bounds); goto _LL1664; _LL1664:{ struct _tuple10
_temp1666=({ struct _tuple10 _temp1665; _temp1665.f1= _temp1661; _temp1665.f2=
_temp1663; _temp1665;}); void* _temp1676; struct Cyc_Absyn_Exp* _temp1678; void*
_temp1680; struct Cyc_Absyn_Exp* _temp1682; void* _temp1684; void* _temp1686;
struct Cyc_Absyn_Exp* _temp1688; void* _temp1690; struct Cyc_Absyn_Exp*
_temp1692; void* _temp1694; _LL1668: _LL1681: _temp1680= _temp1666.f1; if((
unsigned int) _temp1680 > 1u?*(( int*) _temp1680) == Cyc_Absyn_Upper_b: 0){
_LL1683: _temp1682=(( struct Cyc_Absyn_Upper_b_struct*) _temp1680)->f1; goto
_LL1677;} else{ goto _LL1670;} _LL1677: _temp1676= _temp1666.f2; if((
unsigned int) _temp1676 > 1u?*(( int*) _temp1676) == Cyc_Absyn_Upper_b: 0){
_LL1679: _temp1678=(( struct Cyc_Absyn_Upper_b_struct*) _temp1676)->f1; goto
_LL1669;} else{ goto _LL1670;} _LL1670: _LL1687: _temp1686= _temp1666.f1; if((
unsigned int) _temp1686 > 1u?*(( int*) _temp1686) == Cyc_Absyn_Upper_b: 0){
_LL1689: _temp1688=(( struct Cyc_Absyn_Upper_b_struct*) _temp1686)->f1; goto
_LL1685;} else{ goto _LL1672;} _LL1685: _temp1684= _temp1666.f2; if( _temp1684
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1671;} else{ goto _LL1672;} _LL1672:
_LL1695: _temp1694= _temp1666.f1; if( _temp1694 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1691;} else{ goto _LL1674;} _LL1691: _temp1690= _temp1666.f2; if((
unsigned int) _temp1690 > 1u?*(( int*) _temp1690) == Cyc_Absyn_Upper_b: 0){
_LL1693: _temp1692=(( struct Cyc_Absyn_Upper_b_struct*) _temp1690)->f1; goto
_LL1673;} else{ goto _LL1674;} _LL1674: goto _LL1675; _LL1669: if( _temp1657? !
_temp1659: 0){ if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't do null-check conversion at top-level", sizeof( unsigned char),
44u));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp1204, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1696=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1696->hd=(
void*) _temp1200; _temp1696->tl= 0; _temp1696;}), 0), 0))->r));} goto _LL1667;
_LL1671: if( nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr(
old_t2, _temp1688, _temp1200))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__tag_arr_qv, 0),({ struct Cyc_List_List*(* _temp1697)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1700= _temp1200; struct Cyc_Absyn_Exp* _temp1701= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1641.elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1702= _temp1688; struct Cyc_Absyn_Exp* _temp1698[ 3u]={ _temp1700,
_temp1701, _temp1702}; struct _tagged_arr _temp1699={( void*) _temp1698,( void*)
_temp1698,( void*)( _temp1698 + 3u)}; _temp1697( _temp1699);}), 0))->r));} goto
_LL1667; _LL1673: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u));}{ struct Cyc_Absyn_Exp* _temp1709= Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
Cyc_Toc__untag_arr_qv, 0),({ struct Cyc_List_List*(* _temp1703)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1706= _temp1200; struct Cyc_Absyn_Exp* _temp1707= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1641.elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1708= _temp1692; struct Cyc_Absyn_Exp* _temp1704[ 3u]={ _temp1706,
_temp1707, _temp1708}; struct _tagged_arr _temp1705={( void*) _temp1704,( void*)
_temp1704,( void*)( _temp1704 + 3u)}; _temp1703( _temp1705);}), 0); goto _LL1710;
_LL1710: if( _temp1659){( void*)( _temp1709->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1711=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1711->hd=(
void*) _temp1709; _temp1711->tl= 0; _temp1711;}), 0))->r));}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_cast_exp(* _temp1204, _temp1709, 0))->r)); goto
_LL1667;} _LL1675: goto _LL1667; _LL1667:;} goto _LL1632;}}}} _LL1636:{ void*
_temp1712=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1655.bounds); _LL1714: if( _temp1712 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1715;} else{ goto _LL1716;} _LL1716: goto _LL1717; _LL1715:( void*)(
_temp1200->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1200->r, _temp1200->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1713;
_LL1717: goto _LL1713; _LL1713:;} goto _LL1632; _LL1638: goto _LL1632; _LL1632:;}
goto _LL1056;} _LL1090:{ void* _temp1718=( void*) _temp1205->r; struct Cyc_Absyn_Structdecl*
_temp1726; struct Cyc_List_List* _temp1728; struct Cyc_Core_Opt* _temp1730;
struct _tuple0* _temp1732; struct Cyc_List_List* _temp1734; _LL1720: if(*(( int*)
_temp1718) == Cyc_Absyn_Struct_e){ _LL1733: _temp1732=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1718)->f1; goto _LL1731; _LL1731: _temp1730=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1718)->f2; goto _LL1729; _LL1729: _temp1728=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1718)->f3; goto _LL1727; _LL1727: _temp1726=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1718)->f4; goto _LL1721;} else{ goto _LL1722;} _LL1722: if(*(( int*)
_temp1718) == Cyc_Absyn_Tuple_e){ _LL1735: _temp1734=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1718)->f1; goto _LL1723;} else{ goto _LL1724;} _LL1724: goto _LL1725;
_LL1721: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)((
struct _tagged_arr)({ struct _tagged_arr _temp1736=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1205->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1736.last_plus_one - _temp1736.curr, _temp1736.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1205->topt))->v, 1, 0, _temp1728, _temp1732))->r)); goto
_LL1719; _LL1723: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)((
struct _tagged_arr)({ struct _tagged_arr _temp1737=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1205->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1737.last_plus_one - _temp1737.curr, _temp1737.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1734))->r)); goto _LL1719;
_LL1725: Cyc_Toc_exp_to_c( nv, _temp1205); if( ! Cyc_Absyn_is_lvalue( _temp1205)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1205,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1205, 0))->r));} goto _LL1719; _LL1719:;} goto _LL1056; _LL1092: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr _temp1738=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1207->loc); xprintf("%.*s: new at top-level", _temp1738.last_plus_one -
_temp1738.curr, _temp1738.curr);}));}{ void* _temp1739=( void*) _temp1207->r;
struct Cyc_List_List* _temp1751; struct Cyc_Absyn_Exp* _temp1753; struct Cyc_Absyn_Exp*
_temp1755; struct Cyc_Absyn_Vardecl* _temp1757; struct Cyc_Absyn_Structdecl*
_temp1759; struct Cyc_List_List* _temp1761; struct Cyc_Core_Opt* _temp1763;
struct _tuple0* _temp1765; struct Cyc_List_List* _temp1767; _LL1741: if(*(( int*)
_temp1739) == Cyc_Absyn_Array_e){ _LL1752: _temp1751=(( struct Cyc_Absyn_Array_e_struct*)
_temp1739)->f1; goto _LL1742;} else{ goto _LL1743;} _LL1743: if(*(( int*)
_temp1739) == Cyc_Absyn_Comprehension_e){ _LL1758: _temp1757=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1739)->f1; goto _LL1756; _LL1756: _temp1755=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1739)->f2; goto _LL1754; _LL1754: _temp1753=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1739)->f3; goto _LL1744;} else{ goto _LL1745;} _LL1745: if(*(( int*)
_temp1739) == Cyc_Absyn_Struct_e){ _LL1766: _temp1765=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1739)->f1; goto _LL1764; _LL1764: _temp1763=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1739)->f2; goto _LL1762; _LL1762: _temp1761=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1739)->f3; goto _LL1760; _LL1760: _temp1759=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1739)->f4; goto _LL1746;} else{ goto _LL1747;} _LL1747: if(*(( int*)
_temp1739) == Cyc_Absyn_Tuple_e){ _LL1768: _temp1767=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1739)->f1; goto _LL1748;} else{ goto _LL1749;} _LL1749: goto _LL1750;
_LL1742: { struct _tuple0* _temp1769= Cyc_Toc_temp_var(); goto _LL1770; _LL1770: {
struct Cyc_Absyn_Exp* _temp1771= Cyc_Absyn_var_exp( _temp1769, 0); goto _LL1772;
_LL1772: { struct Cyc_Absyn_Stmt* _temp1773= Cyc_Toc_init_array( nv, _temp1771,
_temp1751, Cyc_Absyn_exp_stmt( _temp1771, 0)); goto _LL1774; _LL1774: { void*
old_elt_typ;{ void* _temp1775= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1781; struct Cyc_Absyn_Conref* _temp1783; struct Cyc_Absyn_Tqual _temp1785;
struct Cyc_Absyn_Conref* _temp1787; void* _temp1789; void* _temp1791; _LL1777:
if(( unsigned int) _temp1775 > 4u?*(( int*) _temp1775) == Cyc_Absyn_PointerType:
0){ _LL1782: _temp1781=(( struct Cyc_Absyn_PointerType_struct*) _temp1775)->f1;
_LL1792: _temp1791=( void*) _temp1781.elt_typ; goto _LL1790; _LL1790: _temp1789=(
void*) _temp1781.rgn_typ; goto _LL1788; _LL1788: _temp1787= _temp1781.nullable;
goto _LL1786; _LL1786: _temp1785= _temp1781.tq; goto _LL1784; _LL1784: _temp1783=
_temp1781.bounds; goto _LL1778;} else{ goto _LL1779;} _LL1779: goto _LL1780;
_LL1778: old_elt_typ= _temp1791; goto _LL1776; _LL1780: old_elt_typ=(( void*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u)); goto _LL1776; _LL1776:;}{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* _temp1793= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
goto _LL1794; _LL1794: { struct Cyc_Absyn_Exp* _temp1795= Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1751), 0), 0); goto _LL1796; _LL1796: {
struct Cyc_Absyn_Exp* e1; if( _temp1209 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1795);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1209); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1795);}{ struct Cyc_Absyn_Exp* _temp1797= Cyc_Absyn_cast_exp( _temp1793, e1,
0); goto _LL1798; _LL1798:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1769, _temp1793,( struct Cyc_Absyn_Exp*) _temp1797,
_temp1773, 0), 0))->r)); goto _LL1740;}}}}}}}} _LL1744: { int is_tagged_ptr= 0;{
void* _temp1799= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1805; struct Cyc_Absyn_Conref* _temp1807; struct Cyc_Absyn_Tqual _temp1809;
struct Cyc_Absyn_Conref* _temp1811; void* _temp1813; void* _temp1815; _LL1801:
if(( unsigned int) _temp1799 > 4u?*(( int*) _temp1799) == Cyc_Absyn_PointerType:
0){ _LL1806: _temp1805=(( struct Cyc_Absyn_PointerType_struct*) _temp1799)->f1;
_LL1816: _temp1815=( void*) _temp1805.elt_typ; goto _LL1814; _LL1814: _temp1813=(
void*) _temp1805.rgn_typ; goto _LL1812; _LL1812: _temp1811= _temp1805.nullable;
goto _LL1810; _LL1810: _temp1809= _temp1805.tq; goto _LL1808; _LL1808: _temp1807=
_temp1805.bounds; goto _LL1802;} else{ goto _LL1803;} _LL1803: goto _LL1804;
_LL1802: is_tagged_ptr=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1807) ==( void*) Cyc_Absyn_Unknown_b; goto _LL1800;
_LL1804:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u)); goto _LL1800; _LL1800:;}{ struct _tuple0* max=
Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* old_elt_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1753->topt))->v; void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1755);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1757, Cyc_Absyn_var_exp( max, 0), _temp1753,
Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1817= _new_region();
struct _RegionHandle* r=& _temp1817; _push_region( r);{ struct Cyc_List_List*
decls=({ struct Cyc_List_List* _temp1844=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1844->hd=( void*)({ struct _tuple8*
_temp1845=( struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8));
_temp1845->f1= max; _temp1845->f2= Cyc_Absyn_uint_t; _temp1845->f3=( struct Cyc_Absyn_Exp*)
_temp1755; _temp1845;}); _temp1844->tl= 0; _temp1844;}); struct Cyc_Absyn_Exp*
ai; if( _temp1209 == 0){ ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{
struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1209); Cyc_Toc_exp_to_c(
nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp(
ptr_typ, ai, 0); decls=({ struct Cyc_List_List* _temp1818=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1818->hd=( void*)({
struct _tuple8* _temp1819=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1819->f1= a; _temp1819->f2= ptr_typ; _temp1819->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1819;}); _temp1818->tl= decls; _temp1818;}); if( is_tagged_ptr){
struct _tuple0* _temp1820= Cyc_Toc_temp_var(); goto _LL1821; _LL1821: { void*
_temp1822= Cyc_Toc_typ_to_c( old_typ); goto _LL1823; _LL1823: { struct Cyc_Absyn_Exp*
_temp1830= Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__tag_arr_qv, 0),({
struct Cyc_List_List*(* _temp1824)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1827= Cyc_Absyn_var_exp(
a, 0); struct Cyc_Absyn_Exp* _temp1828= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
struct Cyc_Absyn_Exp* _temp1829= _temp1755; struct Cyc_Absyn_Exp* _temp1825[ 3u]={
_temp1827, _temp1828, _temp1829}; struct _tagged_arr _temp1826={( void*)
_temp1825,( void*) _temp1825,( void*)( _temp1825 + 3u)}; _temp1824( _temp1826);}),
0); goto _LL1831; _LL1831: decls=({ struct Cyc_List_List* _temp1832=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1832->hd=( void*)({
struct _tuple8* _temp1833=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1833->f1= _temp1820; _temp1833->f2= _temp1822; _temp1833->f3=(
struct Cyc_Absyn_Exp*) _temp1830; _temp1833;}); _temp1832->tl= decls; _temp1832;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1820, 0),
0), 0);}}} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1834= decls; goto _LL1835; _LL1835:
for( 0; _temp1834 != 0; _temp1834=(( struct Cyc_List_List*) _check_null(
_temp1834))->tl){ struct Cyc_Absyn_Exp* _temp1838; void* _temp1840; struct
_tuple0* _temp1842; struct _tuple8 _temp1836=*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp1834))->hd); _LL1843: _temp1842= _temp1836.f1; goto _LL1841;
_LL1841: _temp1840= _temp1836.f2; goto _LL1839; _LL1839: _temp1838= _temp1836.f3;
goto _LL1837; _LL1837: s= Cyc_Absyn_declare_stmt( _temp1842, _temp1840,
_temp1838, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}};
_pop_region(& _temp1817);} goto _LL1740;}}} _LL1746:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1207->topt))->v, 1, _temp1209, _temp1761, _temp1765))->r)); goto _LL1740;
_LL1748:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1209,
_temp1767))->r)); goto _LL1740; _LL1750: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1207->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp1209 == 0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1209); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1846=( void*)
_temp1207->r; struct Cyc_Absyn_Exp* _temp1852; void* _temp1854; _LL1848: if(*((
int*) _temp1846) == Cyc_Absyn_Cast_e){ _LL1855: _temp1854=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1846)->f1; goto _LL1853; _LL1853: _temp1852=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1846)->f2; goto _LL1849;} else{ goto _LL1850;} _LL1850: goto _LL1851;
_LL1849:{ struct _tuple10 _temp1857=({ struct _tuple10 _temp1856; _temp1856.f1=
Cyc_Tcutil_compress( _temp1854); _temp1856.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1852->topt))->v); _temp1856;}); void*
_temp1863; struct Cyc_Absyn_PtrInfo _temp1865; struct Cyc_Absyn_Conref*
_temp1867; struct Cyc_Absyn_Tqual _temp1869; struct Cyc_Absyn_Conref* _temp1871;
void* _temp1873; void* _temp1875; void* _temp1877; struct Cyc_Absyn_PtrInfo
_temp1879; struct Cyc_Absyn_Conref* _temp1881; struct Cyc_Absyn_Tqual _temp1883;
struct Cyc_Absyn_Conref* _temp1885; void* _temp1887; void* _temp1889; _LL1859:
_LL1878: _temp1877= _temp1857.f1; if(( unsigned int) _temp1877 > 4u?*(( int*)
_temp1877) == Cyc_Absyn_PointerType: 0){ _LL1880: _temp1879=(( struct Cyc_Absyn_PointerType_struct*)
_temp1877)->f1; _LL1890: _temp1889=( void*) _temp1879.elt_typ; goto _LL1888;
_LL1888: _temp1887=( void*) _temp1879.rgn_typ; goto _LL1886; _LL1886: _temp1885=
_temp1879.nullable; goto _LL1884; _LL1884: _temp1883= _temp1879.tq; goto _LL1882;
_LL1882: _temp1881= _temp1879.bounds; goto _LL1864;} else{ goto _LL1861;}
_LL1864: _temp1863= _temp1857.f2; if(( unsigned int) _temp1863 > 4u?*(( int*)
_temp1863) == Cyc_Absyn_PointerType: 0){ _LL1866: _temp1865=(( struct Cyc_Absyn_PointerType_struct*)
_temp1863)->f1; _LL1876: _temp1875=( void*) _temp1865.elt_typ; goto _LL1874;
_LL1874: _temp1873=( void*) _temp1865.rgn_typ; goto _LL1872; _LL1872: _temp1871=
_temp1865.nullable; goto _LL1870; _LL1870: _temp1869= _temp1865.tq; goto _LL1868;
_LL1868: _temp1867= _temp1865.bounds; goto _LL1860;} else{ goto _LL1861;}
_LL1861: goto _LL1862; _LL1860:{ struct _tuple10 _temp1892=({ struct _tuple10
_temp1891; _temp1891.f1=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1881); _temp1891.f2=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1867); _temp1891;}); void*
_temp1898; struct Cyc_Absyn_Exp* _temp1900; void* _temp1902; _LL1894: _LL1903:
_temp1902= _temp1892.f1; if( _temp1902 ==( void*) Cyc_Absyn_Unknown_b){ goto
_LL1899;} else{ goto _LL1896;} _LL1899: _temp1898= _temp1892.f2; if((
unsigned int) _temp1898 > 1u?*(( int*) _temp1898) == Cyc_Absyn_Upper_b: 0){
_LL1901: _temp1900=(( struct Cyc_Absyn_Upper_b_struct*) _temp1898)->f1; goto
_LL1895;} else{ goto _LL1896;} _LL1896: goto _LL1897; _LL1895: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__init_tag_arr_qv,
0),({ struct Cyc_List_List*(* _temp1904)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1907= mexp;
struct Cyc_Absyn_Exp* _temp1908= _temp1852; struct Cyc_Absyn_Exp* _temp1909= Cyc_Absyn_sizeoftyp_exp(
_temp1889, 0); struct Cyc_Absyn_Exp* _temp1910= _temp1900; struct Cyc_Absyn_Exp*
_temp1905[ 4u]={ _temp1907, _temp1908, _temp1909, _temp1910}; struct _tagged_arr
_temp1906={( void*) _temp1905,( void*) _temp1905,( void*)( _temp1905 + 4u)};
_temp1904( _temp1906);}), 0))->r)); goto _LL1893; _LL1897: goto _LL1893; _LL1893:;}
goto _LL1858; _LL1862: goto _LL1858; _LL1858:;} goto _LL1847; _LL1851: goto
_LL1847; _LL1847:;} if( ! done){ struct _tuple0* _temp1911= Cyc_Toc_temp_var();
goto _LL1912; _LL1912: { struct Cyc_Absyn_Stmt* _temp1913= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1911, 0), 0); goto _LL1914; _LL1914: { struct Cyc_Absyn_Exp*
_temp1915= Cyc_Absyn_signed_int_exp( 0, 0); goto _LL1916; _LL1916: Cyc_Toc_exp_to_c(
nv, _temp1207); _temp1913= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1911, 0), _temp1915, 0), _temp1207, 0), _temp1913, 0);{
void* _temp1917= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto _LL1918;
_LL1918:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1911, _temp1917,( struct Cyc_Absyn_Exp*) mexp, _temp1913, 0), 0))->r));}}}}
goto _LL1740;}} _LL1740:;} goto _LL1056; _LL1094: Cyc_Toc_exp_to_c( nv,
_temp1211); goto _LL1056; _LL1096:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1919=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1919[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1920; _temp1920.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1920.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1213);
_temp1920;}); _temp1919;}))); goto _LL1056; _LL1098:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1921=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1921[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1922; _temp1922.tag= Cyc_Absyn_Offsetof_e;
_temp1922.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1217); _temp1922.f2=
_temp1215; _temp1922;}); _temp1921;}))); goto _LL1056; _LL1100: { void*
_temp1923= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1219->topt))->v); goto _LL1924; _LL1924:{ void* _temp1925= _temp1923;
struct Cyc_Absyn_PtrInfo _temp1931; struct Cyc_Absyn_Conref* _temp1933; struct
Cyc_Absyn_Tqual _temp1935; struct Cyc_Absyn_Conref* _temp1937; void* _temp1939;
void* _temp1941; _LL1927: if(( unsigned int) _temp1925 > 4u?*(( int*) _temp1925)
== Cyc_Absyn_PointerType: 0){ _LL1932: _temp1931=(( struct Cyc_Absyn_PointerType_struct*)
_temp1925)->f1; _LL1942: _temp1941=( void*) _temp1931.elt_typ; goto _LL1940;
_LL1940: _temp1939=( void*) _temp1931.rgn_typ; goto _LL1938; _LL1938: _temp1937=
_temp1931.nullable; goto _LL1936; _LL1936: _temp1935= _temp1931.tq; goto _LL1934;
_LL1934: _temp1933= _temp1931.bounds; goto _LL1928;} else{ goto _LL1929;}
_LL1929: goto _LL1930; _LL1928:{ void* _temp1943=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1933); struct Cyc_Absyn_Exp*
_temp1949; _LL1945: if(( unsigned int) _temp1943 > 1u?*(( int*) _temp1943) ==
Cyc_Absyn_Upper_b: 0){ _LL1950: _temp1949=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1943)->f1; goto _LL1946;} else{ goto _LL1947;} _LL1947: if( _temp1943 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1948;} else{ goto _LL1944;} _LL1946: Cyc_Toc_exp_to_c(
nv, _temp1219); if( Cyc_Toc_is_nullable( _temp1923)){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1219->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1951=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1951->hd=(
void*) _temp1219; _temp1951->tl= 0; _temp1951;}), 0), 0), 0))->r));} goto
_LL1944; _LL1948: { struct Cyc_Absyn_Exp* _temp1952= Cyc_Absyn_uint_exp( 0, 0);
goto _LL1953; _LL1953: _temp1952->topt=({ struct Cyc_Core_Opt* _temp1954=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1954->v=(
void*) Cyc_Absyn_uint_t; _temp1954;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_subscript_exp(
_temp1219, _temp1952, 0))->r)); Cyc_Toc_exp_to_c( nv, e); goto _LL1944;} _LL1944:;}
goto _LL1926; _LL1930:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u)); goto
_LL1926; _LL1926:;} goto _LL1056;} _LL1102: Cyc_Toc_exp_to_c( nv, _temp1223);
if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1056; _LL1104: { void*
e1typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1227->topt))->v); Cyc_Toc_exp_to_c( nv, _temp1227);{ int is_poly= Cyc_Toc_is_poly_project(
e); struct Cyc_Absyn_Conref* _temp1957; struct Cyc_Absyn_Tqual _temp1959; struct
Cyc_Absyn_Conref* _temp1961; void* _temp1963; void* _temp1965; struct Cyc_Absyn_PtrInfo
_temp1955= Cyc_Toc_get_ptr_type( e1typ); _LL1966: _temp1965=( void*) _temp1955.elt_typ;
goto _LL1964; _LL1964: _temp1963=( void*) _temp1955.rgn_typ; goto _LL1962;
_LL1962: _temp1961= _temp1955.nullable; goto _LL1960; _LL1960: _temp1959=
_temp1955.tq; goto _LL1958; _LL1958: _temp1957= _temp1955.bounds; goto _LL1956;
_LL1956:{ void* _temp1967=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1957); struct Cyc_Absyn_Exp* _temp1973; _LL1969: if((
unsigned int) _temp1967 > 1u?*(( int*) _temp1967) == Cyc_Absyn_Upper_b: 0){
_LL1974: _temp1973=(( struct Cyc_Absyn_Upper_b_struct*) _temp1967)->f1; goto
_LL1970;} else{ goto _LL1971;} _LL1971: if( _temp1967 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1972;} else{ goto _LL1968;} _LL1970: if( Cyc_Evexp_eval_const_uint_exp(
_temp1973) < 1){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof( unsigned char), 46u));}{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( 0, _temp1961); if( Cyc_Toc_is_nullable(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1227->topt))->v)){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1227->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_null_qv, 0),({ struct Cyc_List_List* _temp1975=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1975->hd=(
void*) _temp1227; _temp1975->tl= 0; _temp1975;}), 0), 0), _temp1225, 0))->r));}
goto _LL1968;} _LL1972: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1965);( void*)(
_temp1227->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1,
_temp1959), Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_unknown_subscript_qv,
0),({ struct Cyc_List_List*(* _temp1976)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1979= Cyc_Absyn_copy_exp(
_temp1227); struct Cyc_Absyn_Exp* _temp1980= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
struct Cyc_Absyn_Exp* _temp1981= Cyc_Absyn_uint_exp( 0, 0); struct Cyc_Absyn_Exp*
_temp1977[ 3u]={ _temp1979, _temp1980, _temp1981}; struct _tagged_arr _temp1978={(
void*) _temp1977,( void*) _temp1977,( void*)( _temp1977 + 3u)}; _temp1976(
_temp1978);}), 0), 0))->r)); goto _LL1968;} _LL1968:;} if( is_poly){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL1056;}} _LL1106: { void* _temp1982= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1231->topt))->v); goto _LL1983;
_LL1983: Cyc_Toc_exp_to_c( nv, _temp1231); Cyc_Toc_exp_to_c( nv, _temp1229);{
void* _temp1984= _temp1982; struct Cyc_List_List* _temp1992; struct Cyc_Absyn_PtrInfo
_temp1994; struct Cyc_Absyn_Conref* _temp1996; struct Cyc_Absyn_Tqual _temp1998;
struct Cyc_Absyn_Conref* _temp2000; void* _temp2002; void* _temp2004; _LL1986:
if(( unsigned int) _temp1984 > 4u?*(( int*) _temp1984) == Cyc_Absyn_TupleType: 0){
_LL1993: _temp1992=(( struct Cyc_Absyn_TupleType_struct*) _temp1984)->f1; goto
_LL1987;} else{ goto _LL1988;} _LL1988: if(( unsigned int) _temp1984 > 4u?*((
int*) _temp1984) == Cyc_Absyn_PointerType: 0){ _LL1995: _temp1994=(( struct Cyc_Absyn_PointerType_struct*)
_temp1984)->f1; _LL2005: _temp2004=( void*) _temp1994.elt_typ; goto _LL2003;
_LL2003: _temp2002=( void*) _temp1994.rgn_typ; goto _LL2001; _LL2001: _temp2000=
_temp1994.nullable; goto _LL1999; _LL1999: _temp1998= _temp1994.tq; goto _LL1997;
_LL1997: _temp1996= _temp1994.bounds; goto _LL1989;} else{ goto _LL1990;}
_LL1990: goto _LL1991; _LL1987: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1229) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1231, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL1985;} _LL1989:{ void*
_temp2006=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1996); struct Cyc_Absyn_Exp* _temp2012; _LL2008: if((
unsigned int) _temp2006 > 1u?*(( int*) _temp2006) == Cyc_Absyn_Upper_b: 0){
_LL2013: _temp2012=(( struct Cyc_Absyn_Upper_b_struct*) _temp2006)->f1; goto
_LL2009;} else{ goto _LL2010;} _LL2010: if( _temp2006 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL2011;} else{ goto _LL2007;} _LL2009: _temp2012= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp2012), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp2000); void* ta1= Cyc_Toc_typ_to_c( _temp2004);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1998); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_null_qv, 0),({ struct Cyc_List_List*(*
_temp2014)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp2017= _temp1231; struct Cyc_Absyn_Exp*
_temp2018= _temp2012; struct Cyc_Absyn_Exp* _temp2019= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp2020= _temp1229; struct Cyc_Absyn_Exp*
_temp2015[ 4u]={ _temp2017, _temp2018, _temp2019, _temp2020}; struct _tagged_arr
_temp2016={( void*) _temp2015,( void*) _temp2015,( void*)( _temp2015 + 4u)};
_temp2014( _temp2016);}), 0), 0), 0))->r));} else{( void*)( _temp1229->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( Cyc_Toc__check_known_subscript_notnull_qv,
0),({ struct Cyc_List_List*(* _temp2021)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2024= _temp2012;
struct Cyc_Absyn_Exp* _temp2025= Cyc_Absyn_copy_exp( _temp1229); struct Cyc_Absyn_Exp*
_temp2022[ 2u]={ _temp2024, _temp2025}; struct _tagged_arr _temp2023={( void*)
_temp2022,( void*) _temp2022,( void*)( _temp2022 + 2u)}; _temp2021( _temp2023);}),
0))->r));} goto _LL2007;} _LL2011: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp2004);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( ta1, _temp1998), Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
Cyc_Toc__check_unknown_subscript_qv, 0),({ struct Cyc_List_List*(* _temp2026)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp2029= _temp1231; struct Cyc_Absyn_Exp* _temp2030= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp2031= _temp1229; struct Cyc_Absyn_Exp*
_temp2027[ 3u]={ _temp2029, _temp2030, _temp2031}; struct _tagged_arr _temp2028={(
void*) _temp2027,( void*) _temp2027,( void*)( _temp2027 + 3u)}; _temp2026(
_temp2028);}), 0), 0), 0))->r)); goto _LL2007;} _LL2007:;} goto _LL1985; _LL1991:((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u)); goto _LL1985; _LL1985:;} goto _LL1056;} _LL1108:
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 0,
0, _temp1233))->r));} else{ struct Cyc_List_List* _temp2032=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, _temp1233); goto _LL2033; _LL2033: { struct _tagged_arr*
_temp2034= Cyc_Toc_add_tuple_type( _temp2032); goto _LL2035; _LL2035: { struct
Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1233 != 0;( _temp1233=(( struct
Cyc_List_List*) _check_null( _temp1233))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1233))->hd);
dles=({ struct Cyc_List_List* _temp2036=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2036->hd=( void*)({ struct _tuple4*
_temp2037=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp2037->f1=
0; _temp2037->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1233))->hd; _temp2037;}); _temp2036->tl= dles; _temp2036;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}}}
goto _LL1056; _LL1110:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1235, 0))->r));{ struct Cyc_List_List* _temp2038= _temp1235; goto
_LL2039; _LL2039: for( 0; _temp2038 != 0; _temp2038=(( struct Cyc_List_List*)
_check_null( _temp2038))->tl){ struct _tuple4 _temp2042; struct Cyc_Absyn_Exp*
_temp2043; struct Cyc_List_List* _temp2045; struct _tuple4* _temp2040=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp2038))->hd; _temp2042=*
_temp2040; _LL2046: _temp2045= _temp2042.f1; goto _LL2044; _LL2044: _temp2043=
_temp2042.f2; goto _LL2041; _LL2041: Cyc_Toc_exp_to_c( nv, _temp2043);}} goto
_LL1056; _LL1112: { unsigned int _temp2047= Cyc_Evexp_eval_const_uint_exp(
_temp1239); goto _LL2048; _LL2048: Cyc_Toc_exp_to_c( nv, _temp1237);{ struct Cyc_List_List*
es= 0; if( ! Cyc_Toc_is_zero( _temp1237)){ unsigned int i= 0; for( 0; i <
_temp2047; i ++){ es=({ struct Cyc_List_List* _temp2049=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2049->hd=( void*)({ struct
_tuple4* _temp2050=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp2050->f1= 0; _temp2050->f2= _temp1237; _temp2050;}); _temp2049->tl= es;
_temp2049;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
es, 0))->r)); goto _LL1056;}} _LL1114: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1245, _temp1249))->r));}
else{ if( _temp1243 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Struct_e: missing structdecl pointer", sizeof( unsigned char), 37u));}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp1243); struct _RegionHandle _temp2051= _new_region(); struct _RegionHandle*
rgn=& _temp2051; _push_region( rgn);{ struct Cyc_List_List* _temp2052=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1245,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); goto _LL2053; _LL2053: { struct Cyc_List_List*
_temp2054= 0; goto _LL2055; _LL2055:{ struct Cyc_List_List* _temp2056=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto
_LL2057; _LL2057: for( 0; _temp2056 != 0; _temp2056=(( struct Cyc_List_List*)
_check_null( _temp2056))->tl){ struct Cyc_List_List* _temp2058= _temp2052; goto
_LL2059; _LL2059: for( 0; _temp2058 != 0; _temp2058=(( struct Cyc_List_List*)
_check_null( _temp2058))->tl){ if((*(( struct _tuple11*)(( struct Cyc_List_List*)
_check_null( _temp2058))->hd)).f1 ==( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2056))->hd){ struct _tuple11 _temp2062; struct
Cyc_Absyn_Exp* _temp2063; struct Cyc_Absyn_Structfield* _temp2065; struct
_tuple11* _temp2060=( struct _tuple11*)(( struct Cyc_List_List*) _check_null(
_temp2058))->hd; _temp2062=* _temp2060; _LL2066: _temp2065= _temp2062.f1; goto
_LL2064; _LL2064: _temp2063= _temp2062.f2; goto _LL2061; _LL2061: { void*
_temp2067=( void*) _temp2065->type; goto _LL2068; _LL2068: Cyc_Toc_exp_to_c( nv,
_temp2063); if( Cyc_Toc_is_void_star( _temp2067)){( void*)( _temp2063->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp2063->r, 0), 0))->r));} _temp2054=({ struct Cyc_List_List* _temp2069=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2069->hd=(
void*)({ struct _tuple4* _temp2070=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp2070->f1= 0; _temp2070->f2= _temp2063; _temp2070;}); _temp2069->tl=
_temp2054; _temp2069;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2054), 0))->r));}}; _pop_region(& _temp2051);}} goto _LL1056; _LL1116: {
struct Cyc_List_List* fs;{ void* _temp2071= Cyc_Tcutil_compress( _temp1253);
struct Cyc_List_List* _temp2077; _LL2073: if(( unsigned int) _temp2071 > 4u?*((
int*) _temp2071) == Cyc_Absyn_AnonStructType: 0){ _LL2078: _temp2077=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2071)->f1; goto _LL2074;} else{ goto
_LL2075;} _LL2075: goto _LL2076; _LL2074: fs= _temp2077; goto _LL2072; _LL2076:
fs=(( struct Cyc_List_List*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp2079= Cyc_Absynpp_typ2string(
_temp1253); xprintf("anon struct has type %.*s", _temp2079.last_plus_one -
_temp2079.curr, _temp2079.curr);})); goto _LL2072; _LL2072:;}{ struct
_RegionHandle _temp2080= _new_region(); struct _RegionHandle* rgn=& _temp2080;
_push_region( rgn);{ struct Cyc_List_List* _temp2081=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1251, fs); goto _LL2082; _LL2082: for( 0; _temp2081 != 0; _temp2081=((
struct Cyc_List_List*) _check_null( _temp2081))->tl){ struct _tuple11 _temp2085;
struct Cyc_Absyn_Exp* _temp2086; struct Cyc_Absyn_Structfield* _temp2088; struct
_tuple11* _temp2083=( struct _tuple11*)(( struct Cyc_List_List*) _check_null(
_temp2081))->hd; _temp2085=* _temp2083; _LL2089: _temp2088= _temp2085.f1; goto
_LL2087; _LL2087: _temp2086= _temp2085.f2; goto _LL2084; _LL2084: { void*
_temp2090=( void*) _temp2088->type; goto _LL2091; _LL2091: Cyc_Toc_exp_to_c( nv,
_temp2086); if( Cyc_Toc_is_void_star( _temp2090)){( void*)( _temp2086->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp2086->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1251, 0))->r));}; _pop_region(& _temp2080);} goto _LL1056;} _LL1118: {
struct _tuple0* qv= _temp1255->name; if( _temp1257->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2092= _temp1257->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1257->fields))->v;
goto _LL2093; _LL2093: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2092))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2092))->hd)->typs
== 0){ tag_count ++;} _temp2092=(( struct Cyc_List_List*) _check_null( _temp2092))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1056;} _LL1120: { struct _tuple0* _temp2094= _temp1265->name; goto
_LL2095; _LL2095: { struct Cyc_List_List* _temp2096= _temp1265->typs; goto
_LL2097; _LL2097: { struct _tuple0* _temp2098= Cyc_Toc_temp_var(); goto _LL2099;
_LL2099: { struct Cyc_Absyn_Exp* _temp2100= Cyc_Absyn_var_exp( _temp2098, 0);
goto _LL2101; _LL2101: { void* _temp2102= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2094, _tag_arr("_struct", sizeof( unsigned char), 8u))); goto _LL2103;
_LL2103: if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp; if( _temp1267->is_xtunion){
tag_exp= Cyc_Absyn_var_exp( _temp2094, 0);} else{ int tag_count= 0; struct Cyc_List_List*
_temp2104= _temp1267->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1267->fields))->v; goto _LL2105; _LL2105: while( Cyc_Absyn_qvar_cmp(
_temp2094,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2104))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2104))->hd)->typs != 0){ tag_count ++;}
_temp2104=(( struct Cyc_List_List*) _check_null( _temp2104))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2106= 0; goto _LL2107;
_LL2107: for( 0; _temp1269 != 0;( _temp1269=(( struct Cyc_List_List*)
_check_null( _temp1269))->tl, _temp2096=(( struct Cyc_List_List*) _check_null(
_temp2096))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1269))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2096))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2106=({ struct Cyc_List_List* _temp2108=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2108[ 0]=({ struct Cyc_List_List
_temp2109; _temp2109.hd=( void*)({ struct _tuple4* _temp2110=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2110[ 0]=({ struct _tuple4
_temp2111; _temp2111.f1= 0; _temp2111.f2= cur_e; _temp2111;}); _temp2110;});
_temp2109.tl= _temp2106; _temp2109;}); _temp2108;});} _temp2106=({ struct Cyc_List_List*
_temp2112=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2112[ 0]=({ struct Cyc_List_List _temp2113; _temp2113.hd=( void*)({ struct
_tuple4* _temp2114=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2114[ 0]=({ struct _tuple4 _temp2115; _temp2115.f1= 0; _temp2115.f2=
tag_exp; _temp2115;}); _temp2114;}); _temp2113.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2106); _temp2113;}); _temp2112;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2116=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2116->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2094, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp2116;}), _temp2106, 0))->r));}} else{ struct
Cyc_List_List* _temp2118=({ struct Cyc_List_List* _temp2117=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2117->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2100, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp2094, 0), 0); _temp2117->tl= 0; _temp2117;}); goto _LL2119; _LL2119:{ int i=
1; for( 0; _temp1269 != 0;((( _temp1269=(( struct Cyc_List_List*) _check_null(
_temp1269))->tl, i ++)), _temp2096=(( struct Cyc_List_List*) _check_null(
_temp2096))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1269))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2096))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp2120= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2100, Cyc_Toc_fieldname( i), 0), cur_e, 0);
goto _LL2121; _LL2121: _temp2118=({ struct Cyc_List_List* _temp2122=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2122->hd=( void*) _temp2120;
_temp2122->tl= _temp2118; _temp2122;});}}}{ struct Cyc_Absyn_Stmt* _temp2123=
Cyc_Absyn_exp_stmt( _temp2100, 0); goto _LL2124; _LL2124: { struct Cyc_Absyn_Stmt*
_temp2126= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2125=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2125->hd=( void*) _temp2123;
_temp2125->tl= _temp2118; _temp2125;})), 0); goto _LL2127; _LL2127:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp2098,
_temp2102, 0, _temp2126, 0), 0))->r));}}} goto _LL1056;}}}}} _LL1122: goto
_LL1056; _LL1124: { void* t_c= Cyc_Toc_typ_to_c( _temp1281); if( _temp1283 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1283); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1281, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL1056;} _LL1126: Cyc_Toc_stmt_to_c(
nv, _temp1285); goto _LL1056; _LL1128:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem", sizeof( unsigned char), 14u)); goto _LL1056; _LL1130:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u)); goto _LL1056; _LL1132:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u)); goto _LL1056;
_LL1134:(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("fill",
sizeof( unsigned char), 5u)); goto _LL1056; _LL1056:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple12{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple13{ struct _tuple0* f1; void* f2;
} ; struct _tuple14{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple12 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct
_RegionHandle* rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path,
struct Cyc_Absyn_Pat* p, struct _tagged_arr* succ_lab, struct _tagged_arr*
fail_lab, struct Cyc_List_List* decls){ struct Cyc_Absyn_Stmt* s;{ void*
_temp2128=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp2164; struct Cyc_Absyn_Vardecl
_temp2166; struct Cyc_List_List* _temp2167; struct Cyc_Core_Opt* _temp2169;
struct Cyc_Absyn_Exp* _temp2171; void* _temp2173; struct Cyc_Absyn_Tqual
_temp2175; struct _tuple0* _temp2177; void* _temp2179; struct Cyc_Absyn_Vardecl*
_temp2181; int _temp2183; void* _temp2185; unsigned char _temp2187; struct
_tagged_arr _temp2189; struct Cyc_Absyn_Enumfield* _temp2191; struct Cyc_Absyn_Enumdecl*
_temp2193; struct Cyc_List_List* _temp2195; struct Cyc_List_List* _temp2197;
struct Cyc_Absyn_Tunionfield* _temp2199; struct Cyc_Absyn_Tuniondecl* _temp2201;
struct Cyc_Absyn_Pat* _temp2204; struct Cyc_Absyn_Pat _temp2206; struct Cyc_Position_Segment*
_temp2207; struct Cyc_Core_Opt* _temp2209; void* _temp2211; struct Cyc_List_List*
_temp2213; struct Cyc_List_List* _temp2215; struct Cyc_Absyn_Tunionfield*
_temp2217; struct Cyc_Absyn_Tuniondecl* _temp2219; struct Cyc_List_List*
_temp2221; struct Cyc_List_List* _temp2223; struct Cyc_Absyn_Tunionfield*
_temp2225; struct Cyc_Absyn_Tuniondecl* _temp2227; struct Cyc_List_List*
_temp2229; struct Cyc_List_List* _temp2231; struct Cyc_List_List* _temp2233;
struct Cyc_Core_Opt* _temp2235; struct Cyc_Absyn_Structdecl* _temp2237; struct
Cyc_Absyn_Pat* _temp2239; struct _tuple0* _temp2241; struct Cyc_List_List*
_temp2243; struct Cyc_List_List* _temp2245; struct _tuple0* _temp2247; struct
Cyc_List_List* _temp2249; struct Cyc_List_List* _temp2251; struct _tuple0*
_temp2253; _LL2130: if(( unsigned int) _temp2128 > 2u?*(( int*) _temp2128) ==
Cyc_Absyn_Var_p: 0){ _LL2165: _temp2164=(( struct Cyc_Absyn_Var_p_struct*)
_temp2128)->f1; _temp2166=* _temp2164; _LL2180: _temp2179=( void*) _temp2166.sc;
goto _LL2178; _LL2178: _temp2177= _temp2166.name; goto _LL2176; _LL2176:
_temp2175= _temp2166.tq; goto _LL2174; _LL2174: _temp2173=( void*) _temp2166.type;
goto _LL2172; _LL2172: _temp2171= _temp2166.initializer; goto _LL2170; _LL2170:
_temp2169= _temp2166.rgn; goto _LL2168; _LL2168: _temp2167= _temp2166.attributes;
goto _LL2131;} else{ goto _LL2132;} _LL2132: if( _temp2128 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2133;} else{ goto _LL2134;} _LL2134: if(( unsigned int) _temp2128 > 2u?*((
int*) _temp2128) == Cyc_Absyn_Reference_p: 0){ _LL2182: _temp2181=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2128)->f1; goto _LL2135;} else{ goto _LL2136;} _LL2136: if( _temp2128 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2137;} else{ goto _LL2138;} _LL2138: if((
unsigned int) _temp2128 > 2u?*(( int*) _temp2128) == Cyc_Absyn_Int_p: 0){
_LL2186: _temp2185=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2128)->f1;
goto _LL2184; _LL2184: _temp2183=(( struct Cyc_Absyn_Int_p_struct*) _temp2128)->f2;
goto _LL2139;} else{ goto _LL2140;} _LL2140: if(( unsigned int) _temp2128 > 2u?*((
int*) _temp2128) == Cyc_Absyn_Char_p: 0){ _LL2188: _temp2187=(( struct Cyc_Absyn_Char_p_struct*)
_temp2128)->f1; goto _LL2141;} else{ goto _LL2142;} _LL2142: if(( unsigned int)
_temp2128 > 2u?*(( int*) _temp2128) == Cyc_Absyn_Float_p: 0){ _LL2190: _temp2189=((
struct Cyc_Absyn_Float_p_struct*) _temp2128)->f1; goto _LL2143;} else{ goto
_LL2144;} _LL2144: if(( unsigned int) _temp2128 > 2u?*(( int*) _temp2128) == Cyc_Absyn_Enum_p:
0){ _LL2194: _temp2193=(( struct Cyc_Absyn_Enum_p_struct*) _temp2128)->f1; goto
_LL2192; _LL2192: _temp2191=(( struct Cyc_Absyn_Enum_p_struct*) _temp2128)->f2;
goto _LL2145;} else{ goto _LL2146;} _LL2146: if(( unsigned int) _temp2128 > 2u?*((
int*) _temp2128) == Cyc_Absyn_Tunion_p: 0){ _LL2202: _temp2201=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2128)->f1; goto _LL2200; _LL2200: _temp2199=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2128)->f2; goto _LL2198; _LL2198: _temp2197=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2128)->f3; goto _LL2196; _LL2196: _temp2195=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2128)->f4; if( _temp2195 == 0){ goto _LL2147;} else{ goto _LL2148;}} else{
goto _LL2148;} _LL2148: if(( unsigned int) _temp2128 > 2u?*(( int*) _temp2128)
== Cyc_Absyn_Pointer_p: 0){ _LL2205: _temp2204=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2128)->f1; _temp2206=* _temp2204; _LL2212: _temp2211=( void*) _temp2206.r;
if(( unsigned int) _temp2211 > 2u?*(( int*) _temp2211) == Cyc_Absyn_Tunion_p: 0){
_LL2220: _temp2219=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2211)->f1; goto
_LL2218; _LL2218: _temp2217=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2211)->f2;
goto _LL2216; _LL2216: _temp2215=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2211)->f3;
goto _LL2214; _LL2214: _temp2213=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2211)->f4;
goto _LL2210;} else{ goto _LL2150;} _LL2210: _temp2209= _temp2206.topt; goto
_LL2208; _LL2208: _temp2207= _temp2206.loc; goto _LL2203;} else{ goto _LL2150;}
_LL2203: if( _temp2213 != 0){ goto _LL2149;} else{ goto _LL2150;} _LL2150: if((
unsigned int) _temp2128 > 2u?*(( int*) _temp2128) == Cyc_Absyn_Tunion_p: 0){
_LL2228: _temp2227=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2128)->f1; goto
_LL2226; _LL2226: _temp2225=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2128)->f2;
goto _LL2224; _LL2224: _temp2223=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2128)->f3;
goto _LL2222; _LL2222: _temp2221=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2128)->f4;
goto _LL2151;} else{ goto _LL2152;} _LL2152: if(( unsigned int) _temp2128 > 2u?*((
int*) _temp2128) == Cyc_Absyn_Tuple_p: 0){ _LL2230: _temp2229=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2128)->f1; goto _LL2153;} else{ goto _LL2154;} _LL2154: if(( unsigned int)
_temp2128 > 2u?*(( int*) _temp2128) == Cyc_Absyn_Struct_p: 0){ _LL2238:
_temp2237=(( struct Cyc_Absyn_Struct_p_struct*) _temp2128)->f1; goto _LL2236;
_LL2236: _temp2235=(( struct Cyc_Absyn_Struct_p_struct*) _temp2128)->f2; goto
_LL2234; _LL2234: _temp2233=(( struct Cyc_Absyn_Struct_p_struct*) _temp2128)->f3;
goto _LL2232; _LL2232: _temp2231=(( struct Cyc_Absyn_Struct_p_struct*) _temp2128)->f4;
goto _LL2155;} else{ goto _LL2156;} _LL2156: if(( unsigned int) _temp2128 > 2u?*((
int*) _temp2128) == Cyc_Absyn_Pointer_p: 0){ _LL2240: _temp2239=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2128)->f1; goto _LL2157;} else{ goto _LL2158;} _LL2158: if(( unsigned int)
_temp2128 > 2u?*(( int*) _temp2128) == Cyc_Absyn_UnknownId_p: 0){ _LL2242:
_temp2241=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2128)->f1; goto _LL2159;}
else{ goto _LL2160;} _LL2160: if(( unsigned int) _temp2128 > 2u?*(( int*)
_temp2128) == Cyc_Absyn_UnknownCall_p: 0){ _LL2248: _temp2247=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2128)->f1; goto _LL2246; _LL2246: _temp2245=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2128)->f2; goto _LL2244; _LL2244: _temp2243=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2128)->f3; goto _LL2161;} else{ goto _LL2162;} _LL2162: if(( unsigned int)
_temp2128 > 2u?*(( int*) _temp2128) == Cyc_Absyn_UnknownFields_p: 0){ _LL2254:
_temp2253=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2128)->f1; goto
_LL2252; _LL2252: _temp2251=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2128)->f2; goto _LL2250; _LL2250: _temp2249=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2128)->f3; goto _LL2163;} else{ goto _LL2129;} _LL2131: nv= Cyc_Toc_add_varmap(
nv, _temp2177, r); goto _LL2133; _LL2133: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2129; _LL2135: { struct _tuple0* _temp2255= Cyc_Toc_temp_var(); goto
_LL2256; _LL2256: decls=({ struct Cyc_List_List* _temp2257=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2257->hd=( void*)({
struct _tuple13* _temp2258=( struct _tuple13*) _region_malloc( rgn, sizeof(
struct _tuple13)); _temp2258->f1= _temp2255; _temp2258->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2258;}); _temp2257->tl= decls;
_temp2257;}); nv= Cyc_Toc_add_varmap( nv, _temp2181->name, Cyc_Absyn_var_exp(
_temp2255, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2255, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2129;} _LL2137: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2129; _LL2139: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2185, _temp2183, 0), succ_lab, fail_lab); goto _LL2129;
_LL2141: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2187, 0),
succ_lab, fail_lab); goto _LL2129; _LL2143: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2189, 0), succ_lab, fail_lab); goto _LL2129; _LL2145: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2259=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2259[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2260; _temp2260.tag= Cyc_Absyn_Enum_e;
_temp2260.f1= _temp2191->name; _temp2260.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2193; _temp2260.f3=( struct Cyc_Absyn_Enumfield*) _temp2191; _temp2260;});
_temp2259;}), 0), succ_lab, fail_lab); goto _LL2129; _LL2147: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2199->name, 0); if( ! _temp2201->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2129;} _LL2149: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2213);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2217->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp2261=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2217->typs); goto _LL2262;
_LL2262: { struct Cyc_List_List* _temp2263=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2213); goto
_LL2264; _LL2264: for( 0; _temp2263 != 0;((( _temp2263=(( struct Cyc_List_List*)
_check_null( _temp2263))->tl, _temp2261=(( struct Cyc_List_List*) _check_null(
_temp2261))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp2265=( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp2263))->hd; goto _LL2266; _LL2266: {
void* _temp2267=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2261))->hd)).f2; goto _LL2268; _LL2268: { struct _tuple0* _temp2269= Cyc_Toc_temp_var();
goto _LL2270; _LL2270: { void* _temp2271=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2265->topt))->v; goto _LL2272; _LL2272: { void* _temp2273= Cyc_Toc_typ_to_c(
_temp2271); goto _LL2274; _LL2274: { struct _tagged_arr* _temp2275= Cyc_Toc_fresh_label();
goto _LL2276; _LL2276: { struct Cyc_Absyn_Exp* _temp2277= Cyc_Absyn_structarrow_exp(
rcast, Cyc_Toc_fieldname( cnt), 0); goto _LL2278; _LL2278: if( Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c( _temp2267))){ _temp2277= Cyc_Absyn_cast_exp( _temp2273,
_temp2277, 0);} decls=({ struct Cyc_List_List* _temp2279=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2279->hd=( void*)({
struct _tuple13* _temp2280=( struct _tuple13*) _region_malloc( rgn, sizeof(
struct _tuple13)); _temp2280->f1= _temp2269; _temp2280->f2= _temp2273; _temp2280;});
_temp2279->tl= decls; _temp2279;});{ struct _tuple12 _temp2281= Cyc_Toc_xlate_pat(
nv, rgn, _temp2271, Cyc_Absyn_var_exp( _temp2269, 0), _temp2277, _temp2265,
succ_lab, fail_lab, decls); goto _LL2282; _LL2282: nv= _temp2281.f1; decls=
_temp2281.f2;{ struct Cyc_Absyn_Stmt* _temp2283= _temp2281.f3; goto _LL2284;
_LL2284: { struct Cyc_Absyn_Stmt* _temp2285= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2269, 0), _temp2277, 0); goto _LL2286; _LL2286: { struct Cyc_Absyn_Stmt*
_temp2287= Cyc_Absyn_seq_stmt( _temp2285, _temp2283, 0); goto _LL2288; _LL2288:
ss=({ struct Cyc_List_List* _temp2289=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2289->hd=( void*) Cyc_Absyn_label_stmt(
_temp2275, _temp2287, 0); _temp2289->tl= ss; _temp2289;}); succ_lab= _temp2275;}}}}}}}}}}}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2219->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2217->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2290=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2219->fields))->v; goto _LL2291; _LL2291: for( 0; _temp2290 !=
0; _temp2290=(( struct Cyc_List_List*) _check_null( _temp2290))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2292=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2290))->hd; goto _LL2293; _LL2293: if( _temp2292->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2217->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2129;}}} _LL2151: _temp2229=
_temp2221; goto _LL2153; _LL2153: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2229);{ struct Cyc_List_List*
_temp2294=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2229); goto _LL2295; _LL2295: for( 0; _temp2294 !=
0;( _temp2294=(( struct Cyc_List_List*) _check_null( _temp2294))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2296=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2294))->hd; goto _LL2297; _LL2297: { struct _tuple0* _temp2298=
Cyc_Toc_temp_var(); goto _LL2299; _LL2299: { void* _temp2300=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2296->topt))->v; goto _LL2301; _LL2301: {
struct _tagged_arr* _temp2302= Cyc_Toc_fresh_label(); goto _LL2303; _LL2303:
decls=({ struct Cyc_List_List* _temp2304=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2304->hd=( void*)({ struct _tuple13*
_temp2305=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2305->f1= _temp2298; _temp2305->f2= Cyc_Toc_typ_to_c( _temp2300); _temp2305;});
_temp2304->tl= decls; _temp2304;});{ struct _tuple12 _temp2306= Cyc_Toc_xlate_pat(
nv, rgn, _temp2300, Cyc_Absyn_var_exp( _temp2298, 0), Cyc_Absyn_structmember_exp(
path, Cyc_Toc_fieldname( cnt), 0), _temp2296, succ_lab, fail_lab, decls); goto
_LL2307; _LL2307: nv= _temp2306.f1; decls= _temp2306.f2;{ struct Cyc_Absyn_Stmt*
_temp2308= _temp2306.f3; goto _LL2309; _LL2309: { struct Cyc_Absyn_Stmt*
_temp2310= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2298, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2308, 0); goto _LL2311; _LL2311: ss=({ struct Cyc_List_List* _temp2312=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2312->hd=(
void*) Cyc_Absyn_label_stmt( _temp2302, _temp2310, 0); _temp2312->tl= ss;
_temp2312;}); succ_lab= _temp2302;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2129;} _LL2155: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2313=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2231); goto _LL2314; _LL2314: for( 0; _temp2313 !=
0; _temp2313=(( struct Cyc_List_List*) _check_null( _temp2313))->tl){ struct
_tuple14* _temp2315=( struct _tuple14*)(( struct Cyc_List_List*) _check_null(
_temp2313))->hd; goto _LL2316; _LL2316: { struct Cyc_Absyn_Pat* _temp2317=(*
_temp2315).f2; goto _LL2318; _LL2318: { struct _tagged_arr* f;{ void* _temp2319=(
void*)(( struct Cyc_List_List*) _check_null((* _temp2315).f1))->hd; struct
_tagged_arr* _temp2325; _LL2321: if(*(( int*) _temp2319) == Cyc_Absyn_FieldName){
_LL2326: _temp2325=(( struct Cyc_Absyn_FieldName_struct*) _temp2319)->f1; goto
_LL2322;} else{ goto _LL2323;} _LL2323: goto _LL2324; _LL2322: f= _temp2325;
goto _LL2320; _LL2324:( int) _throw(( void*) Cyc_Toc_Match_error); _LL2320:;}{
struct _tuple0* _temp2327= Cyc_Toc_temp_var(); goto _LL2328; _LL2328: { void*
_temp2329=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2317->topt))->v;
goto _LL2330; _LL2330: { void* _temp2331= Cyc_Toc_typ_to_c( _temp2329); goto
_LL2332; _LL2332: { struct _tagged_arr* _temp2333= Cyc_Toc_fresh_label(); goto
_LL2334; _LL2334: decls=({ struct Cyc_List_List* _temp2335=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2335->hd=( void*)({
struct _tuple13* _temp2336=( struct _tuple13*) _region_malloc( rgn, sizeof(
struct _tuple13)); _temp2336->f1= _temp2327; _temp2336->f2= _temp2331; _temp2336;});
_temp2335->tl= decls; _temp2335;});{ struct _tuple12 _temp2337= Cyc_Toc_xlate_pat(
nv, rgn, _temp2329, Cyc_Absyn_var_exp( _temp2327, 0), Cyc_Absyn_structmember_exp(
path, f, 0), _temp2317, succ_lab, fail_lab, decls); goto _LL2338; _LL2338: nv=
_temp2337.f1; decls= _temp2337.f2;{ struct Cyc_Absyn_Exp* _temp2339= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2340; _LL2340: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2237->fields))->v, f)))->type)){ _temp2339= Cyc_Absyn_cast_exp(
_temp2331, _temp2339, 0);}{ struct Cyc_Absyn_Stmt* _temp2341= _temp2337.f3; goto
_LL2342; _LL2342: { struct Cyc_Absyn_Stmt* _temp2343= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2327, 0), _temp2339, 0), _temp2341, 0); goto _LL2344;
_LL2344: ss=({ struct Cyc_List_List* _temp2345=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2345->hd=( void*) Cyc_Absyn_label_stmt(
_temp2333, _temp2343, 0); _temp2345->tl= ss; _temp2345;}); succ_lab= _temp2333;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2129;} _LL2157: { struct _tuple0*
_temp2346= Cyc_Toc_temp_var(); goto _LL2347; _LL2347: { void* _temp2348=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2239->topt))->v; goto _LL2349; _LL2349:
decls=({ struct Cyc_List_List* _temp2350=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2350->hd=( void*)({ struct _tuple13*
_temp2351=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2351->f1= _temp2346; _temp2351->f2= Cyc_Toc_typ_to_c( _temp2348); _temp2351;});
_temp2350->tl= decls; _temp2350;});{ struct _tuple12 _temp2352= Cyc_Toc_xlate_pat(
nv, rgn, _temp2348, Cyc_Absyn_var_exp( _temp2346, 0), Cyc_Absyn_deref_exp( path,
0), _temp2239, succ_lab, fail_lab, decls); goto _LL2353; _LL2353: nv= _temp2352.f1;
decls= _temp2352.f2;{ struct Cyc_Absyn_Stmt* _temp2354= _temp2352.f3; goto
_LL2355; _LL2355: { struct Cyc_Absyn_Stmt* _temp2356= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2346, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2354, 0);
goto _LL2357; _LL2357: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2356, 0);} else{ s= _temp2356;} goto _LL2129;}}}}} _LL2159: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid",
sizeof( unsigned char), 10u)); goto _LL2129; _LL2161: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall", sizeof(
unsigned char), 12u)); goto _LL2129; _LL2163: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields", sizeof(
unsigned char), 14u)); goto _LL2129; _LL2129:;} return({ struct _tuple12
_temp2358; _temp2358.f1= nv; _temp2358.f2= decls; _temp2358.f3= s; _temp2358;});}
struct _tuple15{ struct _tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple15* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple15* _temp2359=(
struct _tuple15*) _region_malloc( r, sizeof( struct _tuple15)); _temp2359->f1=
Cyc_Toc_fresh_label(); _temp2359->f2= Cyc_Toc_fresh_label(); _temp2359->f3= sc;
_temp2359;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp2360=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2361; _LL2361: { int leave_as_switch;{ void*
_temp2362= Cyc_Tcutil_compress( _temp2360); void* _temp2368; void* _temp2370;
_LL2364: if(( unsigned int) _temp2362 > 4u?*(( int*) _temp2362) == Cyc_Absyn_IntType:
0){ _LL2371: _temp2370=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2362)->f1;
goto _LL2369; _LL2369: _temp2368=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2362)->f2; goto _LL2365;} else{ goto _LL2366;} _LL2366: goto _LL2367;
_LL2365: leave_as_switch= 1; goto _LL2363; _LL2367: leave_as_switch= 0; goto
_LL2363; _LL2363:;}{ struct Cyc_List_List* _temp2372= scs; goto _LL2373; _LL2373:
for( 0; _temp2372 != 0; _temp2372=(( struct Cyc_List_List*) _check_null(
_temp2372))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2372))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2372))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2374= scs; goto _LL2375;
_LL2375: for( 0; _temp2374 != 0; _temp2374=(( struct Cyc_List_List*) _check_null(
_temp2374))->tl){ struct Cyc_Absyn_Stmt* _temp2376=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2374))->hd)->body; goto _LL2377;
_LL2377:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2374))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2376, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2376);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct _tagged_arr*
end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2378= _new_region();
struct _RegionHandle* rgn=& _temp2378; _push_region( rgn);{ struct Cyc_List_List*
lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple15*(* f)(
struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*), struct _RegionHandle*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label, rgn,
scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0; struct
Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2379= lscs; goto _LL2380;
_LL2380: for( 0; _temp2379 != 0; _temp2379=(( struct Cyc_List_List*) _check_null(
_temp2379))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( _temp2379))->hd)).f3; struct _tagged_arr*
fail_lab=(( struct Cyc_List_List*) _check_null( _temp2379))->tl == 0? end_l:(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp2379))->tl))->hd)).f1; struct _tagged_arr* succ_lab=(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2379))->hd)).f2;
if( sc->where_clause == 0){ struct _tuple12 _temp2381= Cyc_Toc_xlate_pat( nv,
rgn, _temp2360, r, path, sc->pattern, succ_lab, fail_lab, decls); goto _LL2382;
_LL2382: nvs=({ struct Cyc_List_List* _temp2383=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2383->hd=( void*)
_temp2381.f1; _temp2383->tl= nvs; _temp2383;}); decls= _temp2381.f2; test_stmts=({
struct Cyc_List_List* _temp2384=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2384->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2379))->hd)).f1,
_temp2381.f3, 0); _temp2384->tl= test_stmts; _temp2384;});} else{ struct Cyc_Absyn_Exp*
_temp2385=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2386;
_LL2386: { struct _tagged_arr* _temp2387= Cyc_Toc_fresh_label(); goto _LL2388;
_LL2388: { struct _tuple12 _temp2389= Cyc_Toc_xlate_pat( nv, rgn, _temp2360, r,
path, sc->pattern, _temp2387, fail_lab, decls); goto _LL2390; _LL2390: Cyc_Toc_exp_to_c(
_temp2389.f1, _temp2385); nvs=({ struct Cyc_List_List* _temp2391=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2391->hd=( void*)
_temp2389.f1; _temp2391->tl= nvs; _temp2391;}); decls= _temp2389.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2385, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2387, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2392=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2392->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2379))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2389.f3, s3, 0), 0); _temp2392->tl= test_stmts;
_temp2392;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2393= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2394; _LL2394: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2395=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2396; _LL2396: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2395, end_l,(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2397=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2398; _LL2398: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2397, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2393, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple13 _temp2401;
void* _temp2402; struct _tuple0* _temp2404; struct _tuple13* _temp2399=( struct
_tuple13*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2401=*
_temp2399; _LL2405: _temp2404= _temp2401.f1; goto _LL2403; _LL2403: _temp2402=
_temp2401.f2; goto _LL2400; _LL2400: res= Cyc_Absyn_declare_stmt( _temp2404,
_temp2402, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region(& _temp2378);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2406=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2406[ 0]=({ struct Cyc_List_List _temp2407; _temp2407.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2407.tl= 0; _temp2407;}); _temp2406;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2408=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2450; struct Cyc_Absyn_Stmt* _temp2452; struct Cyc_Absyn_Stmt*
_temp2454; struct Cyc_Absyn_Exp* _temp2456; struct Cyc_Absyn_Stmt* _temp2458;
struct Cyc_Absyn_Stmt* _temp2460; struct Cyc_Absyn_Exp* _temp2462; struct Cyc_Absyn_Stmt*
_temp2464; struct _tuple2 _temp2466; struct Cyc_Absyn_Stmt* _temp2468; struct
Cyc_Absyn_Exp* _temp2470; struct Cyc_Absyn_Stmt* _temp2472; struct Cyc_Absyn_Stmt*
_temp2474; struct Cyc_Absyn_Stmt* _temp2476; struct _tagged_arr* _temp2478;
struct Cyc_Absyn_Stmt* _temp2480; struct _tuple2 _temp2482; struct Cyc_Absyn_Stmt*
_temp2484; struct Cyc_Absyn_Exp* _temp2486; struct _tuple2 _temp2488; struct Cyc_Absyn_Stmt*
_temp2490; struct Cyc_Absyn_Exp* _temp2492; struct Cyc_Absyn_Exp* _temp2494;
struct Cyc_List_List* _temp2496; struct Cyc_Absyn_Exp* _temp2498; struct Cyc_Absyn_Switch_clause**
_temp2500; struct Cyc_List_List* _temp2502; struct Cyc_Absyn_Stmt* _temp2504;
struct Cyc_Absyn_Decl* _temp2506; struct Cyc_Absyn_Stmt* _temp2508; struct
_tagged_arr* _temp2510; struct _tuple2 _temp2512; struct Cyc_Absyn_Stmt*
_temp2514; struct Cyc_Absyn_Exp* _temp2516; struct Cyc_Absyn_Stmt* _temp2518;
struct Cyc_List_List* _temp2520; struct Cyc_Absyn_Stmt* _temp2522; struct Cyc_Absyn_Stmt*
_temp2524; struct Cyc_Absyn_Vardecl* _temp2526; struct Cyc_Absyn_Tvar* _temp2528;
struct Cyc_List_List* _temp2530; struct Cyc_Absyn_Exp* _temp2532; struct Cyc_Absyn_Stmt*
_temp2534; struct Cyc_Absyn_Stmt* _temp2536; _LL2410: if( _temp2408 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2411;} else{ goto _LL2412;} _LL2412: if((
unsigned int) _temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_Exp_s: 0){
_LL2451: _temp2450=(( struct Cyc_Absyn_Exp_s_struct*) _temp2408)->f1; goto
_LL2413;} else{ goto _LL2414;} _LL2414: if(( unsigned int) _temp2408 > 1u?*((
int*) _temp2408) == Cyc_Absyn_Seq_s: 0){ _LL2455: _temp2454=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2408)->f1; goto _LL2453; _LL2453: _temp2452=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2408)->f2; goto _LL2415;} else{ goto _LL2416;} _LL2416: if(( unsigned int)
_temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_Return_s: 0){ _LL2457:
_temp2456=(( struct Cyc_Absyn_Return_s_struct*) _temp2408)->f1; goto _LL2417;}
else{ goto _LL2418;} _LL2418: if(( unsigned int) _temp2408 > 1u?*(( int*)
_temp2408) == Cyc_Absyn_IfThenElse_s: 0){ _LL2463: _temp2462=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2408)->f1; goto _LL2461; _LL2461: _temp2460=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2408)->f2; goto _LL2459; _LL2459: _temp2458=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2408)->f3; goto _LL2419;} else{ goto _LL2420;} _LL2420: if(( unsigned int)
_temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_While_s: 0){ _LL2467: _temp2466=((
struct Cyc_Absyn_While_s_struct*) _temp2408)->f1; _LL2471: _temp2470= _temp2466.f1;
goto _LL2469; _LL2469: _temp2468= _temp2466.f2; goto _LL2465; _LL2465: _temp2464=((
struct Cyc_Absyn_While_s_struct*) _temp2408)->f2; goto _LL2421;} else{ goto
_LL2422;} _LL2422: if(( unsigned int) _temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_Break_s:
0){ _LL2473: _temp2472=(( struct Cyc_Absyn_Break_s_struct*) _temp2408)->f1; goto
_LL2423;} else{ goto _LL2424;} _LL2424: if(( unsigned int) _temp2408 > 1u?*((
int*) _temp2408) == Cyc_Absyn_Continue_s: 0){ _LL2475: _temp2474=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2408)->f1; goto _LL2425;} else{ goto _LL2426;} _LL2426: if(( unsigned int)
_temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_Goto_s: 0){ _LL2479: _temp2478=((
struct Cyc_Absyn_Goto_s_struct*) _temp2408)->f1; goto _LL2477; _LL2477:
_temp2476=(( struct Cyc_Absyn_Goto_s_struct*) _temp2408)->f2; goto _LL2427;}
else{ goto _LL2428;} _LL2428: if(( unsigned int) _temp2408 > 1u?*(( int*)
_temp2408) == Cyc_Absyn_For_s: 0){ _LL2495: _temp2494=(( struct Cyc_Absyn_For_s_struct*)
_temp2408)->f1; goto _LL2489; _LL2489: _temp2488=(( struct Cyc_Absyn_For_s_struct*)
_temp2408)->f2; _LL2493: _temp2492= _temp2488.f1; goto _LL2491; _LL2491:
_temp2490= _temp2488.f2; goto _LL2483; _LL2483: _temp2482=(( struct Cyc_Absyn_For_s_struct*)
_temp2408)->f3; _LL2487: _temp2486= _temp2482.f1; goto _LL2485; _LL2485:
_temp2484= _temp2482.f2; goto _LL2481; _LL2481: _temp2480=(( struct Cyc_Absyn_For_s_struct*)
_temp2408)->f4; goto _LL2429;} else{ goto _LL2430;} _LL2430: if(( unsigned int)
_temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_Switch_s: 0){ _LL2499:
_temp2498=(( struct Cyc_Absyn_Switch_s_struct*) _temp2408)->f1; goto _LL2497;
_LL2497: _temp2496=(( struct Cyc_Absyn_Switch_s_struct*) _temp2408)->f2; goto
_LL2431;} else{ goto _LL2432;} _LL2432: if(( unsigned int) _temp2408 > 1u?*((
int*) _temp2408) == Cyc_Absyn_Fallthru_s: 0){ _LL2503: _temp2502=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2408)->f1; goto _LL2501; _LL2501: _temp2500=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2408)->f2; goto _LL2433;} else{ goto _LL2434;} _LL2434: if(( unsigned int)
_temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_Decl_s: 0){ _LL2507: _temp2506=((
struct Cyc_Absyn_Decl_s_struct*) _temp2408)->f1; goto _LL2505; _LL2505:
_temp2504=(( struct Cyc_Absyn_Decl_s_struct*) _temp2408)->f2; goto _LL2435;}
else{ goto _LL2436;} _LL2436: if(( unsigned int) _temp2408 > 1u?*(( int*)
_temp2408) == Cyc_Absyn_Label_s: 0){ _LL2511: _temp2510=(( struct Cyc_Absyn_Label_s_struct*)
_temp2408)->f1; goto _LL2509; _LL2509: _temp2508=(( struct Cyc_Absyn_Label_s_struct*)
_temp2408)->f2; goto _LL2437;} else{ goto _LL2438;} _LL2438: if(( unsigned int)
_temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_Do_s: 0){ _LL2519: _temp2518=((
struct Cyc_Absyn_Do_s_struct*) _temp2408)->f1; goto _LL2513; _LL2513: _temp2512=((
struct Cyc_Absyn_Do_s_struct*) _temp2408)->f2; _LL2517: _temp2516= _temp2512.f1;
goto _LL2515; _LL2515: _temp2514= _temp2512.f2; goto _LL2439;} else{ goto
_LL2440;} _LL2440: if(( unsigned int) _temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_TryCatch_s:
0){ _LL2523: _temp2522=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2408)->f1;
goto _LL2521; _LL2521: _temp2520=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2408)->f2; goto _LL2441;} else{ goto _LL2442;} _LL2442: if(( unsigned int)
_temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_Region_s: 0){ _LL2529:
_temp2528=(( struct Cyc_Absyn_Region_s_struct*) _temp2408)->f1; goto _LL2527;
_LL2527: _temp2526=(( struct Cyc_Absyn_Region_s_struct*) _temp2408)->f2; goto
_LL2525; _LL2525: _temp2524=(( struct Cyc_Absyn_Region_s_struct*) _temp2408)->f3;
goto _LL2443;} else{ goto _LL2444;} _LL2444: if(( unsigned int) _temp2408 > 1u?*((
int*) _temp2408) == Cyc_Absyn_SwitchC_s: 0){ _LL2533: _temp2532=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2408)->f1; goto _LL2531; _LL2531: _temp2530=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2408)->f2; goto _LL2445;} else{ goto _LL2446;} _LL2446: if(( unsigned int)
_temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_Cut_s: 0){ _LL2535: _temp2534=((
struct Cyc_Absyn_Cut_s_struct*) _temp2408)->f1; goto _LL2447;} else{ goto
_LL2448;} _LL2448: if(( unsigned int) _temp2408 > 1u?*(( int*) _temp2408) == Cyc_Absyn_Splice_s:
0){ _LL2537: _temp2536=(( struct Cyc_Absyn_Splice_s_struct*) _temp2408)->f1;
goto _LL2449;} else{ goto _LL2409;} _LL2411: return; _LL2413: Cyc_Toc_exp_to_c(
nv, _temp2450); return; _LL2415: Cyc_Toc_stmt_to_c( nv, _temp2454); s= _temp2452;
continue; _LL2417: { struct Cyc_Core_Opt* topt= 0; if( _temp2456 != 0){ topt=({
struct Cyc_Core_Opt* _temp2538=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2538->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2456))->topt))->v);
_temp2538;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2456));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2539=
Cyc_Toc_temp_var(); goto _LL2540; _LL2540: { struct Cyc_Absyn_Stmt* _temp2541=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2539, 0),
0); goto _LL2542; _LL2542:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2539,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2456,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2541, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2419: Cyc_Toc_exp_to_c(
nv, _temp2462); Cyc_Toc_stmt_to_c( nv, _temp2460); s= _temp2458; continue;
_LL2421: Cyc_Toc_exp_to_c( nv, _temp2470); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2464); return; _LL2423: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2472 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2472))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2425: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -(( struct Cyc_Absyn_Stmt*) _check_null( _temp2474))->try_depth, s);
return; _LL2427: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2476))->try_depth, s); return; _LL2429: Cyc_Toc_exp_to_c( nv,
_temp2494); Cyc_Toc_exp_to_c( nv, _temp2492); Cyc_Toc_exp_to_c( nv, _temp2486);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2480); return; _LL2431: Cyc_Toc_xlate_switch(
nv, s, _temp2498, _temp2496); return; _LL2433: if( nv->fallthru_info == 0){( int)
_throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("fallthru in unexpected place",
sizeof( unsigned char), 29u)));}{ struct _tuple6 _temp2545; struct Cyc_Dict_Dict*
_temp2546; struct Cyc_List_List* _temp2548; struct _tagged_arr* _temp2550;
struct _tuple6* _temp2543=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp2545=* _temp2543; _LL2551: _temp2550= _temp2545.f1;
goto _LL2549; _LL2549: _temp2548= _temp2545.f2; goto _LL2547; _LL2547: _temp2546=
_temp2545.f3; goto _LL2544; _LL2544: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp2550, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp2500)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2552=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2548); goto _LL2553; _LL2553: { struct Cyc_List_List* _temp2554=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp2502); goto
_LL2555; _LL2555: for( 0; _temp2552 != 0;( _temp2552=(( struct Cyc_List_List*)
_check_null( _temp2552))->tl, _temp2554=(( struct Cyc_List_List*) _check_null(
_temp2554))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp2554))->hd); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2546,( struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp2552))->hd),(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp2554))->hd, 0),
s2, 0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}}} _LL2435:{ void*
_temp2556=( void*) _temp2506->r; int _temp2566; struct Cyc_Absyn_Exp* _temp2568;
struct Cyc_Core_Opt* _temp2570; struct Cyc_Core_Opt* _temp2572; struct Cyc_Absyn_Pat*
_temp2574; struct Cyc_List_List* _temp2576; struct Cyc_Absyn_Vardecl* _temp2578;
_LL2558: if(*(( int*) _temp2556) == Cyc_Absyn_Let_d){ _LL2575: _temp2574=((
struct Cyc_Absyn_Let_d_struct*) _temp2556)->f1; goto _LL2573; _LL2573: _temp2572=((
struct Cyc_Absyn_Let_d_struct*) _temp2556)->f2; goto _LL2571; _LL2571: _temp2570=((
struct Cyc_Absyn_Let_d_struct*) _temp2556)->f3; goto _LL2569; _LL2569: _temp2568=((
struct Cyc_Absyn_Let_d_struct*) _temp2556)->f4; goto _LL2567; _LL2567: _temp2566=((
struct Cyc_Absyn_Let_d_struct*) _temp2556)->f5; goto _LL2559;} else{ goto
_LL2560;} _LL2560: if(*(( int*) _temp2556) == Cyc_Absyn_Letv_d){ _LL2577:
_temp2576=(( struct Cyc_Absyn_Letv_d_struct*) _temp2556)->f1; goto _LL2561;}
else{ goto _LL2562;} _LL2562: if(*(( int*) _temp2556) == Cyc_Absyn_Var_d){
_LL2579: _temp2578=(( struct Cyc_Absyn_Var_d_struct*) _temp2556)->f1; goto
_LL2563;} else{ goto _LL2564;} _LL2564: goto _LL2565; _LL2559:( void*)( s->r=(
void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2574, _temp2570, _temp2568,
_temp2566, _temp2504))->r)); goto _LL2557; _LL2561: { struct Cyc_List_List*
_temp2580=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2576); goto _LL2581; _LL2581: if( _temp2580 == 0){(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d", sizeof( unsigned char),
13u));}( void*)( _temp2506->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2582=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2582[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2583; _temp2583.tag= Cyc_Absyn_Var_d;
_temp2583.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2580))->hd; _temp2583;}); _temp2582;}))); _temp2580=(( struct Cyc_List_List*)
_check_null( _temp2580))->tl; for( 0; _temp2580 != 0; _temp2580=(( struct Cyc_List_List*)
_check_null( _temp2580))->tl){ struct Cyc_Absyn_Decl* _temp2586= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2584=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2584[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2585; _temp2585.tag= Cyc_Absyn_Var_d; _temp2585.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2580))->hd; _temp2585;}); _temp2584;}),
0); goto _LL2587; _LL2587:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp2586, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c(
nv, s); goto _LL2557;} _LL2563: { struct _tuple0* _temp2588= _temp2578->name;
goto _LL2589; _LL2589:( void*)( _temp2578->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2578->type));{ struct Cyc_Toc_Env* _temp2592= Cyc_Toc_add_varmap( nv,
_temp2588, Cyc_Absyn_varb_exp( _temp2588,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2590=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2590[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2591; _temp2591.tag= Cyc_Absyn_Local_b;
_temp2591.f1= _temp2578; _temp2591;}); _temp2590;}), 0)); goto _LL2593; _LL2593:
Cyc_Toc_stmt_to_c( _temp2592, _temp2504); if( _temp2578->initializer != 0){
struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*) _check_null( _temp2578->initializer);
void* _temp2594=( void*) init->r; struct Cyc_Absyn_Exp* _temp2600; struct Cyc_Absyn_Exp*
_temp2602; struct Cyc_Absyn_Vardecl* _temp2604; _LL2596: if(*(( int*) _temp2594)
== Cyc_Absyn_Comprehension_e){ _LL2605: _temp2604=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2594)->f1; goto _LL2603; _LL2603: _temp2602=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2594)->f2; goto _LL2601; _LL2601: _temp2600=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2594)->f3; goto _LL2597;} else{ goto _LL2598;} _LL2598: goto _LL2599;
_LL2597: _temp2578->initializer= 0;( void*)( _temp2504->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2588, 0), _temp2604, _temp2602, _temp2600, Cyc_Absyn_new_stmt((
void*) _temp2504->r, 0), 0))->r)); goto _LL2595; _LL2599: Cyc_Toc_exp_to_c( nv,
init); goto _LL2595; _LL2595:;} goto _LL2557;}} _LL2565:(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function",
sizeof( unsigned char), 39u)); goto _LL2557; _LL2557:;} return; _LL2437: s=
_temp2508; continue; _LL2439: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv),
_temp2518); Cyc_Toc_exp_to_c( nv, _temp2516); return; _LL2441: { struct _tuple0*
h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var(); struct
_tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp*
was_thrown_exp= Cyc_Absyn_var_exp( was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ);
void* was_thrown_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_sint_t); struct Cyc_Absyn_Exp*
pushhandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__push_handler_qv, 0); struct Cyc_Absyn_Exp*
setjmp_exp= Cyc_Absyn_var_exp( Cyc_Toc_setjmp_qv, 0); struct Cyc_Absyn_Exp*
pophandler_exp= Cyc_Absyn_var_exp( Cyc_Toc__pop_handler_qv, 0); struct Cyc_Absyn_Exp*
_exn_thrown_exp= Cyc_Absyn_var_exp( Cyc_Toc__exn_thrown_qv, 0); e_exp->topt=({
struct Cyc_Core_Opt* _temp2606=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2606->v=( void*) e_typ; _temp2606;}); nv= Cyc_Toc_add_varmap(
nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2522);{ struct Cyc_Absyn_Stmt*
_temp2607= Cyc_Absyn_seq_stmt( _temp2522, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0); goto _LL2608; _LL2608: { struct _tuple0*
_temp2609= Cyc_Toc_temp_var(); goto _LL2610; _LL2610: { struct Cyc_Absyn_Exp*
_temp2611= Cyc_Absyn_var_exp( _temp2609, 0); goto _LL2612; _LL2612: { struct Cyc_Absyn_Vardecl*
_temp2613= Cyc_Absyn_new_vardecl( _temp2609, Cyc_Absyn_exn_typ, 0); goto _LL2614;
_LL2614: _temp2611->topt=({ struct Cyc_Core_Opt* _temp2615=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2615->v=( void*) Cyc_Absyn_exn_typ;
_temp2615;});{ struct Cyc_Absyn_Pat* _temp2620=({ struct Cyc_Absyn_Pat*
_temp2616=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat));
_temp2616->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp2618=(
struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2618[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2619; _temp2619.tag= Cyc_Absyn_Var_p;
_temp2619.f1= _temp2613; _temp2619;}); _temp2618;})); _temp2616->topt=({ struct
Cyc_Core_Opt* _temp2617=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2617->v=( void*) Cyc_Absyn_exn_typ; _temp2617;}); _temp2616->loc= 0;
_temp2616;}); goto _LL2621; _LL2621: { struct Cyc_Absyn_Exp* _temp2622= Cyc_Absyn_throw_exp(
_temp2611, 0); goto _LL2623; _LL2623: _temp2622->topt=({ struct Cyc_Core_Opt*
_temp2624=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2624->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2624;});{ struct Cyc_Absyn_Stmt*
_temp2625= Cyc_Absyn_exp_stmt( _temp2622, 0); goto _LL2626; _LL2626: { struct
Cyc_Absyn_Switch_clause* _temp2630=({ struct Cyc_Absyn_Switch_clause* _temp2627=(
struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2627->pattern= _temp2620; _temp2627->pat_vars=({ struct Cyc_Core_Opt*
_temp2628=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2628->v=( void*)({ struct Cyc_List_List* _temp2629=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2629->hd=( void*) _temp2613;
_temp2629->tl= 0; _temp2629;}); _temp2628;}); _temp2627->where_clause= 0;
_temp2627->body= _temp2625; _temp2627->loc= 0; _temp2627;}); goto _LL2631;
_LL2631: { struct Cyc_Absyn_Stmt* _temp2633= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2520,({ struct Cyc_List_List* _temp2632=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2632->hd=( void*) _temp2630; _temp2632->tl=
0; _temp2632;})), 0); goto _LL2634; _LL2634: Cyc_Toc_stmt_to_c( nv, _temp2633);{
struct Cyc_Absyn_Exp* _temp2636= Cyc_Absyn_fncall_exp( setjmp_exp,({ struct Cyc_List_List*
_temp2635=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2635->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2635->tl= 0; _temp2635;}), 0); goto _LL2637; _LL2637: { struct Cyc_Absyn_Stmt*
_temp2639= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct
Cyc_List_List* _temp2638=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2638->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2638->tl= 0;
_temp2638;}), 0), 0); goto _LL2640; _LL2640: { struct Cyc_Absyn_Exp* _temp2641=
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0, 0); goto _LL2642; _LL2642: {
struct Cyc_Absyn_Exp* _temp2643= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 1,
0); goto _LL2644; _LL2644:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2639, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2641, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2636, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2643, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2607, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, _exn_thrown_exp,
0), _temp2633, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2443: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2526->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2650=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2650[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2651; _temp2651.tag= Cyc_Absyn_Local_b; _temp2651.f1=
_temp2526; _temp2651;}); _temp2650;}), 0); struct Cyc_Absyn_Exp* pushregion_exp=
Cyc_Absyn_var_exp(({ struct _tuple0* _temp2649=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp2649->f1= Cyc_Toc_rel_ns; _temp2649->f2= Cyc_Toc__push_region_sp;
_temp2649;}), 0); struct Cyc_Absyn_Exp* popregion_exp= Cyc_Absyn_var_exp(({
struct _tuple0* _temp2648=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2648->f1= Cyc_Toc_rel_ns; _temp2648->f2= Cyc_Toc__pop_region_sp; _temp2648;}),
0); struct Cyc_Absyn_Exp* newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple0*
_temp2647=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2647->f1=
Cyc_Toc_rel_ns; _temp2647->f2= Cyc_Toc__new_region_sp; _temp2647;}), 0); struct
Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp*
newregion_call= Cyc_Absyn_fncall_exp( newregion_exp, 0, 0); struct Cyc_Absyn_Exp*
popregion_call= Cyc_Absyn_fncall_exp( popregion_exp,({ struct Cyc_List_List*
_temp2646=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2646->hd=( void*) addr_rh_exp; _temp2646->tl= 0; _temp2646;}), 0); struct
Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp( pushregion_exp,({ struct
Cyc_List_List* _temp2645=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2645->hd=( void*) x_exp; _temp2645->tl= 0; _temp2645;}), 0); Cyc_Toc_stmt_to_c(
Cyc_Toc_add_varmap( nv, x_var, x_exp), _temp2524);{ struct Cyc_Absyn_Stmt* s2=
Cyc_Absyn_seq_stmt( _temp2524, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2445:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("switchC_s", sizeof( unsigned char), 10u)); return;
_LL2447:(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("cut",
sizeof( unsigned char), 4u)); return; _LL2449:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("splice", sizeof( unsigned char), 7u)); return; _LL2409:;}}
struct _tuple16{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp2652= f->args; goto _LL2653;
_LL2653: for( 0; _temp2652 != 0; _temp2652=(( struct Cyc_List_List*) _check_null(
_temp2652))->tl){(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2652))->hd)).f3= Cyc_Toc_typ_to_c((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2652))->hd)).f3);{ struct _tuple0* _temp2655=({ struct _tuple0*
_temp2654=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2654->f1=(
void*) Cyc_Absyn_Loc_n; _temp2654->f2=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2652))->hd)).f1; _temp2654;}); goto _LL2656; _LL2656: nv= Cyc_Toc_add_varmap(
nv, _temp2655, Cyc_Absyn_var_exp( _temp2655, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2659; void* _temp2661; void* _temp2663; struct Cyc_Absyn_Tqual _temp2665;
struct Cyc_Core_Opt* _temp2667; struct Cyc_Absyn_VarargInfo _temp2657=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2668: _temp2667=
_temp2657.name; goto _LL2666; _LL2666: _temp2665= _temp2657.tq; goto _LL2664;
_LL2664: _temp2663=( void*) _temp2657.type; goto _LL2662; _LL2662: _temp2661=(
void*) _temp2657.rgn; goto _LL2660; _LL2660: _temp2659= _temp2657.inject; goto
_LL2658; _LL2658: { void* _temp2669= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2663, _temp2661, _temp2665)); goto _LL2670; _LL2670: { struct _tuple0*
_temp2672=({ struct _tuple0* _temp2671=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2671->f1=( void*) Cyc_Absyn_Loc_n; _temp2671->f2=( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp2667))->v; _temp2671;});
goto _LL2673; _LL2673: f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2674=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2674->hd=( void*)({ struct _tuple16* _temp2675=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp2675->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp2667))->v; _temp2675->f2= _temp2665;
_temp2675->f3= _temp2669; _temp2675;}); _temp2674->tl= 0; _temp2674;})); nv= Cyc_Toc_add_varmap(
nv, _temp2672, Cyc_Absyn_var_exp( _temp2672, 0)); f->cyc_varargs= 0;}}}{ struct
Cyc_List_List* _temp2676=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; goto _LL2677; _LL2677: for( 0; _temp2676 !=
0; _temp2676=(( struct Cyc_List_List*) _check_null( _temp2676))->tl){( void*)(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp2676))->hd)->type=(
void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp2676))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2678= s;
_LL2680: if( _temp2678 ==( void*) Cyc_Absyn_Abstract){ goto _LL2681;} else{ goto
_LL2682;} _LL2682: if( _temp2678 ==( void*) Cyc_Absyn_ExternC){ goto _LL2683;}
else{ goto _LL2684;} _LL2684: goto _LL2685; _LL2681: return( void*) Cyc_Absyn_Public;
_LL2683: return( void*) Cyc_Absyn_Extern; _LL2685: return s; _LL2679:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2686=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2687; _LL2687: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2688=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2688->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2688;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2689=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2686).f2); goto _LL2690; _LL2690: if( _temp2689 == 0){ seen_defn_before= 0;(
struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2686).f2,
s));} else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*)
_check_null( _temp2689))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2686).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2691=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2691->v=( void*)
_temp2686; _temp2691;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2692=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2693; _LL2693: for( 0; _temp2692 != 0;
_temp2692=(( struct Cyc_List_List*) _check_null( _temp2692))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2692))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2692))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2694=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2695; _LL2695: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2696=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2696->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2696;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2697=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2694).f2); goto _LL2698; _LL2698: if( _temp2697 == 0){ seen_defn_before= 0;(
struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp2694).f2,
u));} else{ if((( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*)
_check_null( _temp2697))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2694).f2, u)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)(((( void*) u->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
u->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) u->sc))); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2699=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2699->v=( void*)
_temp2694; _temp2699;}); if( u->fields != 0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp2700=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; goto _LL2701; _LL2701: for( 0; _temp2700 != 0;
_temp2700=(( struct Cyc_List_List*) _check_null( _temp2700))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2700))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2700))->hd)->type));}}}}} static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far= 0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl*
tud){ if( Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2702=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2702->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2702;});}{ struct
_tuple0* _temp2703= tud->name; goto _LL2704; _LL2704: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2703)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2703));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2705=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2706; _LL2706: for( 0;
_temp2705 != 0; _temp2705=(( struct Cyc_List_List*) _check_null( _temp2705))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2705))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2707; _temp2707.q_const= 1; _temp2707.q_volatile= 0;
_temp2707.q_restrict= 0; _temp2707;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2708=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2708->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2709=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2709[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2710; _temp2710.tag= Cyc_Absyn_Var_d;
_temp2710.f1= vd; _temp2710;}); _temp2709;}), 0); _temp2708->tl= Cyc_Toc_result_decls;
_temp2708;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2711; _temp2711.q_const=
1; _temp2711.q_volatile= 0; _temp2711.q_restrict= 0; _temp2711;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2712=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2712->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2713=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2713[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2714; _temp2714.tag= Cyc_Absyn_Var_d; _temp2714.f1= vd; _temp2714;});
_temp2713;}), 0); _temp2712->tl= Cyc_Toc_result_decls; _temp2712;});{ struct Cyc_List_List*
_temp2715= 0; goto _LL2716; _LL2716: { int i= 1;{ struct Cyc_List_List*
_temp2717= f->typs; goto _LL2718; _LL2718: for( 0; _temp2717 != 0;( _temp2717=((
struct Cyc_List_List*) _check_null( _temp2717))->tl, i ++)){ struct _tagged_arr*
_temp2719= Cyc_Toc_fieldname( i); goto _LL2720; _LL2720: { struct Cyc_Absyn_Structfield*
_temp2722=({ struct Cyc_Absyn_Structfield* _temp2721=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2721->name= _temp2719;
_temp2721->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2717))->hd)).f1; _temp2721->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2717))->hd)).f2);
_temp2721->width= 0; _temp2721->attributes= 0; _temp2721;}); goto _LL2723;
_LL2723: _temp2715=({ struct Cyc_List_List* _temp2724=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2724->hd=( void*) _temp2722;
_temp2724->tl= _temp2715; _temp2724;});}}} _temp2715=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2715); _temp2715=({ struct Cyc_List_List*
_temp2725=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2725->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2726=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2726->name= Cyc_Toc_tag_sp;
_temp2726->tq= Cyc_Toc_mt_tq; _temp2726->type=( void*) Cyc_Absyn_sint_t;
_temp2726->width= 0; _temp2726->attributes= 0; _temp2726;}); _temp2725->tl=
_temp2715; _temp2725;});{ struct Cyc_Absyn_Structdecl* _temp2730=({ struct Cyc_Absyn_Structdecl*
_temp2727=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2727->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2727->name=({ struct Cyc_Core_Opt*
_temp2729=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2729->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp2729;}); _temp2727->tvs= 0; _temp2727->fields=({
struct Cyc_Core_Opt* _temp2728=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2728->v=( void*) _temp2715; _temp2728;}); _temp2727->attributes=
0; _temp2727;}); goto _LL2731; _LL2731: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2732=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2732->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2733=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2733[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2734; _temp2734.tag= Cyc_Absyn_Struct_d;
_temp2734.f1= _temp2730; _temp2734;}); _temp2733;}), 0); _temp2732->tl= Cyc_Toc_result_decls;
_temp2732;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2735=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2735->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2735;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2736= xd->name; goto _LL2737; _LL2737: {
struct Cyc_List_List* _temp2738=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2739; _LL2739: for( 0; _temp2738 != 0;
_temp2738=(( struct Cyc_List_List*) _check_null( _temp2738))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2738))->hd; struct _tagged_arr* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2740= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof( unsigned char)) + 4,
0); goto _LL2741; _LL2741: { struct Cyc_Absyn_ArrayType_struct* _temp2744=({
struct Cyc_Absyn_ArrayType_struct* _temp2742=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2742[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2743; _temp2743.tag= Cyc_Absyn_ArrayType;
_temp2743.f1=( void*) Cyc_Absyn_uchar_t; _temp2743.f2= Cyc_Toc_mt_tq; _temp2743.f3=(
struct Cyc_Absyn_Exp*) _temp2740; _temp2743;}); _temp2742;}); goto _LL2745;
_LL2745: { struct Cyc_Core_Opt* _temp2746=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2754; int _temp2755; _LL2748: if( _temp2746 == 0){ goto
_LL2749;} else{ goto _LL2750;} _LL2750: if( _temp2746 == 0){ goto _LL2752;}
else{ _temp2754=* _temp2746; _LL2756: _temp2755=( int) _temp2754.v; if(
_temp2755 == 0){ goto _LL2751;} else{ goto _LL2752;}} _LL2752: goto _LL2753;
_LL2749: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_arr)({
int _temp2757= 0; int _temp2758= 0; int _temp2759= 0; int _temp2760= 0; struct
_tagged_arr _temp2761=* fn; xprintf("%c%c%c%c%.*s", _temp2757, _temp2758,
_temp2759, _temp2760, _temp2761.last_plus_one - _temp2761.curr, _temp2761.curr);}),
0);}{ struct Cyc_Absyn_Vardecl* _temp2762= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2744, initopt); goto _LL2763; _LL2763:( void*)( _temp2762->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2764=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2764->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2765=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2765[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2766; _temp2766.tag= Cyc_Absyn_Var_d;
_temp2766.f1= _temp2762; _temp2766;}); _temp2765;}), 0); _temp2764->tl= Cyc_Toc_result_decls;
_temp2764;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2767= f->typs; goto _LL2768; _LL2768: for( 0;
_temp2767 != 0;( _temp2767=(( struct Cyc_List_List*) _check_null( _temp2767))->tl,
i ++)){ struct _tagged_arr* _temp2770=({ struct _tagged_arr* _temp2769=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2769[ 0]=( struct
_tagged_arr) xprintf("f%d", i); _temp2769;}); goto _LL2771; _LL2771: { struct
Cyc_Absyn_Structfield* _temp2773=({ struct Cyc_Absyn_Structfield* _temp2772=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2772->name= _temp2770; _temp2772->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2767))->hd)).f1; _temp2772->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2767))->hd)).f2);
_temp2772->width= 0; _temp2772->attributes= 0; _temp2772;}); goto _LL2774;
_LL2774: fields=({ struct Cyc_List_List* _temp2775=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2775->hd=( void*) _temp2773;
_temp2775->tl= fields; _temp2775;});}}} fields=({ struct Cyc_List_List*
_temp2776=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2776->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2777=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2777->name= Cyc_Toc_tag_sp;
_temp2777->tq= Cyc_Toc_mt_tq; _temp2777->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2777->width= 0; _temp2777->attributes= 0; _temp2777;});
_temp2776->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2776;});{ struct Cyc_Absyn_Structdecl* _temp2781=({ struct Cyc_Absyn_Structdecl*
_temp2778=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2778->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2778->name=({ struct Cyc_Core_Opt*
_temp2780=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2780->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp2780;}); _temp2778->tvs= 0; _temp2778->fields=({
struct Cyc_Core_Opt* _temp2779=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2779->v=( void*) fields; _temp2779;}); _temp2778->attributes=
0; _temp2778;}); goto _LL2782; _LL2782: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2783=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2783->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2784=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2784[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2785; _temp2785.tag= Cyc_Absyn_Struct_d;
_temp2785.f1= _temp2781; _temp2785;}); _temp2784;}), 0); _temp2783->tl= Cyc_Toc_result_decls;
_temp2783;});}} goto _LL2747;}} _LL2751: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2791= Cyc_Absyn_string_exp(( struct _tagged_arr)({
int _temp2786= 0; int _temp2787= 0; int _temp2788= 0; int _temp2789= 0; struct
_tagged_arr _temp2790=* fn; xprintf("%c%c%c%c%.*s", _temp2786, _temp2787,
_temp2788, _temp2789, _temp2790.last_plus_one - _temp2790.curr, _temp2790.curr);}),
0); goto _LL2792; _LL2792: { struct Cyc_Absyn_Vardecl* _temp2793= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2744,( struct Cyc_Absyn_Exp*) _temp2791); goto _LL2794;
_LL2794:( void*)( _temp2793->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2795=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2795->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2796=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2796[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2797; _temp2797.tag= Cyc_Absyn_Var_d; _temp2797.f1= _temp2793; _temp2797;});
_temp2796;}), 0); _temp2795->tl= Cyc_Toc_result_decls; _temp2795;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2747; _LL2753: goto
_LL2747; _LL2747:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2798= ed->name; goto
_LL2799; _LL2799:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2800=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2801; _LL2801: for( 0; _temp2800 != 0;
_temp2800=(( struct Cyc_List_List*) _check_null( _temp2800))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2800))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2800))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2800))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_arr*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct _RegionHandle
_temp2802= _new_region(); struct _RegionHandle* prgn=& _temp2802; _push_region(
prgn);{ struct Cyc_Absyn_Stmt* _temp2805; struct Cyc_List_List* _temp2807;
struct Cyc_Toc_Env* _temp2809; struct _tuple12 _temp2803= Cyc_Toc_xlate_pat( nv,
prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2810: _temp2809= _temp2803.f1; goto _LL2808; _LL2808: _temp2807=
_temp2803.f2; goto _LL2806; _LL2806: _temp2805= _temp2803.f3; goto _LL2804;
_LL2804: Cyc_Toc_stmt_to_c( _temp2809, s);{ struct Cyc_Absyn_Stmt* _temp2811=
Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL2812; _LL2812: if( exhaust){ s=
Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2805, _temp2811, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2813= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); goto _LL2814; _LL2814: { struct Cyc_Absyn_Stmt*
_temp2815= Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2813, 0), 0);
goto _LL2816; _LL2816: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),(
struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2805, Cyc_Absyn_seq_stmt(
_temp2815, _temp2811, 0), 0), 0);}} for( 0; _temp2807 != 0; _temp2807=(( struct
Cyc_List_List*) _check_null( _temp2807))->tl){ struct _tuple13 _temp2819; void*
_temp2820; struct _tuple0* _temp2822; struct _tuple13* _temp2817=( struct
_tuple13*)(( struct Cyc_List_List*) _check_null( _temp2807))->hd; _temp2819=*
_temp2817; _LL2823: _temp2822= _temp2819.f1; goto _LL2821; _LL2821: _temp2820=
_temp2819.f2; goto _LL2818; _LL2818: s= Cyc_Absyn_declare_stmt( _temp2822,
_temp2820, 0, s, 0);}}}; _pop_region(& _temp2802);} return s;}} static struct
Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( ds))->hd; void* _temp2824=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2850; struct Cyc_Absyn_Fndecl* _temp2852; int _temp2854; struct Cyc_Absyn_Exp*
_temp2856; struct Cyc_Core_Opt* _temp2858; struct Cyc_Core_Opt* _temp2860;
struct Cyc_Absyn_Pat* _temp2862; struct Cyc_List_List* _temp2864; struct Cyc_Absyn_Structdecl*
_temp2866; struct Cyc_Absyn_Uniondecl* _temp2868; struct Cyc_Absyn_Tuniondecl*
_temp2870; struct Cyc_Absyn_Enumdecl* _temp2872; struct Cyc_Absyn_Typedefdecl*
_temp2874; struct Cyc_List_List* _temp2876; struct _tagged_arr* _temp2878;
struct Cyc_List_List* _temp2880; struct _tuple0* _temp2882; struct Cyc_List_List*
_temp2884; _LL2826: if(*(( int*) _temp2824) == Cyc_Absyn_Var_d){ _LL2851:
_temp2850=(( struct Cyc_Absyn_Var_d_struct*) _temp2824)->f1; goto _LL2827;}
else{ goto _LL2828;} _LL2828: if(*(( int*) _temp2824) == Cyc_Absyn_Fn_d){
_LL2853: _temp2852=(( struct Cyc_Absyn_Fn_d_struct*) _temp2824)->f1; goto
_LL2829;} else{ goto _LL2830;} _LL2830: if(*(( int*) _temp2824) == Cyc_Absyn_Let_d){
_LL2863: _temp2862=(( struct Cyc_Absyn_Let_d_struct*) _temp2824)->f1; goto
_LL2861; _LL2861: _temp2860=(( struct Cyc_Absyn_Let_d_struct*) _temp2824)->f2;
goto _LL2859; _LL2859: _temp2858=(( struct Cyc_Absyn_Let_d_struct*) _temp2824)->f3;
goto _LL2857; _LL2857: _temp2856=(( struct Cyc_Absyn_Let_d_struct*) _temp2824)->f4;
goto _LL2855; _LL2855: _temp2854=(( struct Cyc_Absyn_Let_d_struct*) _temp2824)->f5;
goto _LL2831;} else{ goto _LL2832;} _LL2832: if(*(( int*) _temp2824) == Cyc_Absyn_Letv_d){
_LL2865: _temp2864=(( struct Cyc_Absyn_Letv_d_struct*) _temp2824)->f1; goto
_LL2833;} else{ goto _LL2834;} _LL2834: if(*(( int*) _temp2824) == Cyc_Absyn_Struct_d){
_LL2867: _temp2866=(( struct Cyc_Absyn_Struct_d_struct*) _temp2824)->f1; goto
_LL2835;} else{ goto _LL2836;} _LL2836: if(*(( int*) _temp2824) == Cyc_Absyn_Union_d){
_LL2869: _temp2868=(( struct Cyc_Absyn_Union_d_struct*) _temp2824)->f1; goto
_LL2837;} else{ goto _LL2838;} _LL2838: if(*(( int*) _temp2824) == Cyc_Absyn_Tunion_d){
_LL2871: _temp2870=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2824)->f1; goto
_LL2839;} else{ goto _LL2840;} _LL2840: if(*(( int*) _temp2824) == Cyc_Absyn_Enum_d){
_LL2873: _temp2872=(( struct Cyc_Absyn_Enum_d_struct*) _temp2824)->f1; goto
_LL2841;} else{ goto _LL2842;} _LL2842: if(*(( int*) _temp2824) == Cyc_Absyn_Typedef_d){
_LL2875: _temp2874=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2824)->f1; goto
_LL2843;} else{ goto _LL2844;} _LL2844: if(*(( int*) _temp2824) == Cyc_Absyn_Namespace_d){
_LL2879: _temp2878=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2824)->f1; goto
_LL2877; _LL2877: _temp2876=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2824)->f2;
goto _LL2845;} else{ goto _LL2846;} _LL2846: if(*(( int*) _temp2824) == Cyc_Absyn_Using_d){
_LL2883: _temp2882=(( struct Cyc_Absyn_Using_d_struct*) _temp2824)->f1; goto
_LL2881; _LL2881: _temp2880=(( struct Cyc_Absyn_Using_d_struct*) _temp2824)->f2;
goto _LL2847;} else{ goto _LL2848;} _LL2848: if(*(( int*) _temp2824) == Cyc_Absyn_ExternC_d){
_LL2885: _temp2884=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2824)->f1; goto
_LL2849;} else{ goto _LL2825;} _LL2827: { struct _tuple0* _temp2886= _temp2850->name;
goto _LL2887; _LL2887: if(( void*) _temp2850->sc ==( void*) Cyc_Absyn_ExternC){
_temp2886=({ struct _tuple0* _temp2888=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2888->f1= Cyc_Toc_rel_ns; _temp2888->f2=(* _temp2886).f2;
_temp2888;});} if( _temp2850->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*) _check_null( _temp2850->initializer));} if( ! nv->toplevel){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u));} nv= Cyc_Toc_add_varmap( nv, _temp2850->name, Cyc_Absyn_varb_exp(
_temp2886,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp2889=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp2889[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp2890; _temp2890.tag= Cyc_Absyn_Global_b;
_temp2890.f1= _temp2850; _temp2890;}); _temp2889;}), 0)); _temp2850->name=
_temp2886;( void*)( _temp2850->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2850->sc));(
void*)( _temp2850->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2850->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2891=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2891->hd=( void*) d; _temp2891->tl=
Cyc_Toc_result_decls; _temp2891;}); goto _LL2825;} _LL2829: { struct _tuple0*
_temp2892= _temp2852->name; goto _LL2893; _LL2893: if(( void*) _temp2852->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2892=({ struct _tuple0* _temp2894=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2894->f1= Cyc_Toc_rel_ns;
_temp2894->f2=(* _temp2892).f2; _temp2894;});} nv= Cyc_Toc_add_varmap( nv,
_temp2852->name, Cyc_Absyn_var_exp( _temp2892, 0)); _temp2852->name= _temp2892;
Cyc_Toc_fndecl_to_c( nv, _temp2852); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2895=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2895->hd=( void*) d; _temp2895->tl= Cyc_Toc_result_decls; _temp2895;});
goto _LL2825;} _LL2831:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("letdecl", sizeof( unsigned char), 8u)); goto _LL2825; _LL2833:(( int(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("letdecl", sizeof(
unsigned char), 8u)); goto _LL2825; _LL2835: Cyc_Toc_structdecl_to_c( _temp2866);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2896=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2896->hd=( void*) d; _temp2896->tl=
Cyc_Toc_result_decls; _temp2896;}); goto _LL2825; _LL2837: Cyc_Toc_uniondecl_to_c(
_temp2868); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2897=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2897->hd=( void*) d; _temp2897->tl=
Cyc_Toc_result_decls; _temp2897;}); goto _LL2825; _LL2839: if( _temp2870->is_xtunion){
Cyc_Toc_xtuniondecl_to_c( _temp2870);} else{ Cyc_Toc_tuniondecl_to_c( _temp2870);}
goto _LL2825; _LL2841: Cyc_Toc_enumdecl_to_c( nv, _temp2872); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2898=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2898->hd=( void*) d; _temp2898->tl= Cyc_Toc_result_decls;
_temp2898;}); goto _LL2825; _LL2843: _temp2874->name= _temp2874->name; _temp2874->tvs=
0;( void*)( _temp2874->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2874->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2899=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2899->hd=( void*) d; _temp2899->tl=
Cyc_Toc_result_decls; _temp2899;}); goto _LL2825; _LL2845: _temp2880= _temp2876;
goto _LL2847; _LL2847: _temp2884= _temp2880; goto _LL2849; _LL2849: nv= Cyc_Toc_decls_to_c(
nv, _temp2884, top); goto _LL2825; _LL2825:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}