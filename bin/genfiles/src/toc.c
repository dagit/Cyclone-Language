 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
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
Cyc_Toc__handler_cons_string; static unsigned char _temp9[ 8u]="handler"; static
struct _tagged_arr Cyc_Toc_handler_string={ _temp9, _temp9, _temp9 + 8u}; static
struct _tagged_arr* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static
unsigned char _temp10[ 14u]="_RegionHandle"; static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_temp10, _temp10, _temp10 + 14u}; static struct _tagged_arr* Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string; static unsigned char _temp11[ 7u]="_throw"; static
struct _tagged_arr Cyc_Toc__throw_str={ _temp11, _temp11, _temp11 + 7u}; static
struct _tuple0 Cyc_Toc__throw_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__throw_str};
static struct _tuple0* Cyc_Toc__throw_qv=& Cyc_Toc__throw_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__throw_re={ 1u,& Cyc_Toc__throw_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__throw_ev={ 0,( void*)(( void*)& Cyc_Toc__throw_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__throw_e=& Cyc_Toc__throw_ev; static
unsigned char _temp13[ 7u]="setjmp"; static struct _tagged_arr Cyc_Toc_setjmp_str={
_temp13, _temp13, _temp13 + 7u}; static struct _tuple0 Cyc_Toc_setjmp_pr={( void*)&
Cyc_Toc_rel_ns_value,& Cyc_Toc_setjmp_str}; static struct _tuple0* Cyc_Toc_setjmp_qv=&
Cyc_Toc_setjmp_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={ 1u,&
Cyc_Toc_setjmp_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,( void*)(( void*)& Cyc_Toc_setjmp_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc_setjmp_e=&
Cyc_Toc_setjmp_ev; static unsigned char _temp15[ 14u]="_push_handler"; static
struct _tagged_arr Cyc_Toc__push_handler_str={ _temp15, _temp15, _temp15 + 14u};
static struct _tuple0 Cyc_Toc__push_handler_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__push_handler_str}; static struct _tuple0* Cyc_Toc__push_handler_qv=&
Cyc_Toc__push_handler_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1u,& Cyc_Toc__push_handler_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__push_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__push_handler_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;
static unsigned char _temp17[ 13u]="_pop_handler"; static struct _tagged_arr Cyc_Toc__pop_handler_str={
_temp17, _temp17, _temp17 + 13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__pop_handler_str}; static struct _tuple0*
Cyc_Toc__pop_handler_qv=& Cyc_Toc__pop_handler_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_handler_re={ 1u,& Cyc_Toc__pop_handler_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_handler_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;
static unsigned char _temp19[ 12u]="_exn_thrown"; static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_temp19, _temp19, _temp19 + 12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__exn_thrown_str}; static struct _tuple0*
Cyc_Toc__exn_thrown_qv=& Cyc_Toc__exn_thrown_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={ 1u,& Cyc_Toc__exn_thrown_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={ 0,( void*)(( void*)& Cyc_Toc__exn_thrown_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;
static unsigned char _temp21[ 14u]="_npop_handler"; static struct _tagged_arr
Cyc_Toc__npop_handler_str={ _temp21, _temp21, _temp21 + 14u}; static struct
_tuple0 Cyc_Toc__npop_handler_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__npop_handler_str};
static struct _tuple0* Cyc_Toc__npop_handler_qv=& Cyc_Toc__npop_handler_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={ 1u,& Cyc_Toc__npop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__npop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev; static unsigned char _temp23[ 12u]="_check_null";
static struct _tagged_arr Cyc_Toc__check_null_str={ _temp23, _temp23, _temp23 +
12u}; static struct _tuple0 Cyc_Toc__check_null_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__check_null_str}; static struct _tuple0* Cyc_Toc__check_null_qv=& Cyc_Toc__check_null_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={ 1u,& Cyc_Toc__check_null_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={ 0,(
void*)(( void*)& Cyc_Toc__check_null_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev; static unsigned char _temp25[ 28u]="_check_known_subscript_null";
static struct _tagged_arr Cyc_Toc__check_known_subscript_null_str={ _temp25,
_temp25, _temp25 + 28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__check_known_subscript_null_str}; static
struct _tuple0* Cyc_Toc__check_known_subscript_null_qv=& Cyc_Toc__check_known_subscript_null_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={ 1u,&
Cyc_Toc__check_known_subscript_null_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_null_ev={ 0,( void*)(( void*)& Cyc_Toc__check_known_subscript_null_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static unsigned char _temp27[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={ _temp27, _temp27,
_temp27 + 31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct _tuple0* Cyc_Toc__check_known_subscript_notnull_qv=& Cyc_Toc__check_known_subscript_notnull_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={
1u,& Cyc_Toc__check_known_subscript_notnull_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={ 0,( void*)((
void*)& Cyc_Toc__check_known_subscript_notnull_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static unsigned char _temp29[ 25u]="_check_unknown_subscript"; static struct
_tagged_arr Cyc_Toc__check_unknown_subscript_str={ _temp29, _temp29, _temp29 +
25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__check_unknown_subscript_str}; static struct _tuple0* Cyc_Toc__check_unknown_subscript_qv=&
Cyc_Toc__check_unknown_subscript_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1u,& Cyc_Toc__check_unknown_subscript_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__check_unknown_subscript_ev={ 0,( void*)(( void*)& Cyc_Toc__check_unknown_subscript_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_unknown_subscript_e=& Cyc_Toc__check_unknown_subscript_ev;
static unsigned char _temp31[ 12u]="_tagged_arr"; static struct _tagged_arr Cyc_Toc__tagged_arr_str={
_temp31, _temp31, _temp31 + 12u}; static struct _tuple0 Cyc_Toc__tagged_arr_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__tagged_arr_str}; static struct _tuple0*
Cyc_Toc__tagged_arr_qv=& Cyc_Toc__tagged_arr_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_re={ 1u,& Cyc_Toc__tagged_arr_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_e=& Cyc_Toc__tagged_arr_ev;
static unsigned char _temp33[ 9u]="_tag_arr"; static struct _tagged_arr Cyc_Toc__tag_arr_str={
_temp33, _temp33, _temp33 + 9u}; static struct _tuple0 Cyc_Toc__tag_arr_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__tag_arr_str}; static struct _tuple0* Cyc_Toc__tag_arr_qv=&
Cyc_Toc__tag_arr_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={
1u,& Cyc_Toc__tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,( void*)(( void*)& Cyc_Toc__tag_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tag_arr_e=&
Cyc_Toc__tag_arr_ev; static unsigned char _temp35[ 14u]="_init_tag_arr"; static
struct _tagged_arr Cyc_Toc__init_tag_arr_str={ _temp35, _temp35, _temp35 + 14u};
static struct _tuple0 Cyc_Toc__init_tag_arr_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__init_tag_arr_str}; static struct _tuple0* Cyc_Toc__init_tag_arr_qv=&
Cyc_Toc__init_tag_arr_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_tag_arr_re={
1u,& Cyc_Toc__init_tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__init_tag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__init_tag_arr_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__init_tag_arr_e=& Cyc_Toc__init_tag_arr_ev;
static unsigned char _temp37[ 11u]="_untag_arr"; static struct _tagged_arr Cyc_Toc__untag_arr_str={
_temp37, _temp37, _temp37 + 11u}; static struct _tuple0 Cyc_Toc__untag_arr_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__untag_arr_str}; static struct _tuple0*
Cyc_Toc__untag_arr_qv=& Cyc_Toc__untag_arr_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__untag_arr_re={ 1u,& Cyc_Toc__untag_arr_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__untag_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;
static unsigned char _temp39[ 14u]="_get_arr_size"; static struct _tagged_arr
Cyc_Toc__get_arr_size_str={ _temp39, _temp39, _temp39 + 14u}; static struct
_tuple0 Cyc_Toc__get_arr_size_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__get_arr_size_str};
static struct _tuple0* Cyc_Toc__get_arr_size_qv=& Cyc_Toc__get_arr_size_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={ 1u,& Cyc_Toc__get_arr_size_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={ 0,(
void*)(( void*)& Cyc_Toc__get_arr_size_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__get_arr_size_e=&
Cyc_Toc__get_arr_size_ev; static unsigned char _temp41[ 17u]="_tagged_arr_plus";
static struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={ _temp41, _temp41,
_temp41 + 17u}; static struct _tuple0 Cyc_Toc__tagged_arr_plus_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__tagged_arr_plus_str}; static struct _tuple0* Cyc_Toc__tagged_arr_plus_qv=&
Cyc_Toc__tagged_arr_plus_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_plus_re={
1u,& Cyc_Toc__tagged_arr_plus_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_plus_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_plus_e=& Cyc_Toc__tagged_arr_plus_ev;
static unsigned char _temp43[ 25u]="_tagged_arr_inplace_plus"; static struct
_tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={ _temp43, _temp43, _temp43 +
25u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__tagged_arr_inplace_plus_str}; static struct _tuple0* Cyc_Toc__tagged_arr_inplace_plus_qv=&
Cyc_Toc__tagged_arr_inplace_plus_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_inplace_plus_e=& Cyc_Toc__tagged_arr_inplace_plus_ev;
static unsigned char _temp45[ 30u]="_tagged_arr_inplace_plus_post"; static
struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={ _temp45, _temp45,
_temp45 + 30u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct _tuple0* Cyc_Toc__tagged_arr_inplace_plus_post_qv=& Cyc_Toc__tagged_arr_inplace_plus_post_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _temp47[ 10u]="GC_malloc"; static struct _tagged_arr Cyc_Toc_GC_malloc_str={
_temp47, _temp47, _temp47 + 10u}; static struct _tuple0 Cyc_Toc_GC_malloc_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc_GC_malloc_str}; static struct _tuple0*
Cyc_Toc_GC_malloc_qv=& Cyc_Toc_GC_malloc_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_GC_malloc_re={ 1u,& Cyc_Toc_GC_malloc_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc_GC_malloc_ev={ 0,( void*)(( void*)& Cyc_Toc_GC_malloc_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc_GC_malloc_e=& Cyc_Toc_GC_malloc_ev;
static unsigned char _temp49[ 17u]="GC_malloc_atomic"; static struct _tagged_arr
Cyc_Toc_GC_malloc_atomic_str={ _temp49, _temp49, _temp49 + 17u}; static struct
_tuple0 Cyc_Toc_GC_malloc_atomic_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc_GC_malloc_atomic_str};
static struct _tuple0* Cyc_Toc_GC_malloc_atomic_qv=& Cyc_Toc_GC_malloc_atomic_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_GC_malloc_atomic_re={ 1u,& Cyc_Toc_GC_malloc_atomic_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_GC_malloc_atomic_ev={
0,( void*)(( void*)& Cyc_Toc_GC_malloc_atomic_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc_GC_malloc_atomic_e=& Cyc_Toc_GC_malloc_atomic_ev; static unsigned char
_temp51[ 15u]="_region_malloc"; static struct _tagged_arr Cyc_Toc__region_malloc_str={
_temp51, _temp51, _temp51 + 15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__region_malloc_str}; static struct
_tuple0* Cyc_Toc__region_malloc_qv=& Cyc_Toc__region_malloc_pr; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={ 1u,& Cyc_Toc__region_malloc_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={ 0,(
void*)(( void*)& Cyc_Toc__region_malloc_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev; static unsigned char
_temp53[ 12u]="_new_region"; static struct _tagged_arr Cyc_Toc__new_region_str={
_temp53, _temp53, _temp53 + 12u}; static struct _tuple0 Cyc_Toc__new_region_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__new_region_str}; static struct _tuple0*
Cyc_Toc__new_region_qv=& Cyc_Toc__new_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__new_region_re={ 1u,& Cyc_Toc__new_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={ 0,( void*)(( void*)& Cyc_Toc__new_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;
static unsigned char _temp55[ 13u]="_push_region"; static struct _tagged_arr Cyc_Toc__push_region_str={
_temp55, _temp55, _temp55 + 13u}; static struct _tuple0 Cyc_Toc__push_region_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__push_region_str}; static struct _tuple0*
Cyc_Toc__push_region_qv=& Cyc_Toc__push_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={ 1u,& Cyc_Toc__push_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={ 0,( void*)(( void*)& Cyc_Toc__push_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;
static unsigned char _temp57[ 12u]="_pop_region"; static struct _tagged_arr Cyc_Toc__pop_region_str={
_temp57, _temp57, _temp57 + 12u}; static struct _tuple0 Cyc_Toc__pop_region_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__pop_region_str}; static struct _tuple0*
Cyc_Toc__pop_region_qv=& Cyc_Toc__pop_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_region_re={ 1u,& Cyc_Toc__pop_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;
static unsigned char _temp59[ 15u]="_sfile_to_file"; static struct _tagged_arr
Cyc_Toc__sfile_to_file_str={ _temp59, _temp59, _temp59 + 15u}; static struct
_tuple0 Cyc_Toc__sfile_to_file_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__sfile_to_file_str};
static struct _tuple0* Cyc_Toc__sfile_to_file_qv=& Cyc_Toc__sfile_to_file_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__sfile_to_file_re={ 1u,& Cyc_Toc__sfile_to_file_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__sfile_to_file_ev={ 0,(
void*)(( void*)& Cyc_Toc__sfile_to_file_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__sfile_to_file_e=& Cyc_Toc__sfile_to_file_ev; static struct Cyc_Absyn_StructType_struct
Cyc_Toc_tagged_arr_typ_v={ 9u,( struct _tuple0*)& Cyc_Toc__tagged_arr_pr, 0, 0};
static void* Cyc_Toc_tagged_arr_typ=( void*)& Cyc_Toc_tagged_arr_typ_v; static
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={ 0, 0, 0}; struct _tuple4{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp*
e){ void* _temp62=( void*) e->r; void* _temp88; unsigned char _temp90; void*
_temp92; void* _temp94; short _temp96; void* _temp98; void* _temp100; int
_temp102; void* _temp104; void* _temp106; long long _temp108; void* _temp110;
void* _temp112; struct Cyc_Absyn_Exp* _temp114; void* _temp116; struct Cyc_List_List*
_temp118; struct Cyc_List_List* _temp120; struct Cyc_Absyn_Structdecl* _temp122;
struct Cyc_List_List* _temp124; struct Cyc_Core_Opt* _temp126; struct _tuple0*
_temp128; struct Cyc_List_List* _temp130; struct _tuple1* _temp132; struct Cyc_List_List*
_temp134; struct Cyc_Core_Opt* _temp136; _LL64: if(*(( int*) _temp62) == Cyc_Absyn_Const_e){
_LL89: _temp88=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp62)->f1; if((
unsigned int) _temp88 > 1u?*(( int*) _temp88) == Cyc_Absyn_Char_c: 0){ _LL93:
_temp92=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp88)->f1; goto _LL91;
_LL91: _temp90=(( struct Cyc_Absyn_Char_c_struct*) _temp88)->f2; goto _LL65;}
else{ goto _LL66;}} else{ goto _LL66;} _LL66: if(*(( int*) _temp62) == Cyc_Absyn_Const_e){
_LL95: _temp94=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp62)->f1; if((
unsigned int) _temp94 > 1u?*(( int*) _temp94) == Cyc_Absyn_Short_c: 0){ _LL99:
_temp98=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp94)->f1; goto _LL97;
_LL97: _temp96=(( struct Cyc_Absyn_Short_c_struct*) _temp94)->f2; goto _LL67;}
else{ goto _LL68;}} else{ goto _LL68;} _LL68: if(*(( int*) _temp62) == Cyc_Absyn_Const_e){
_LL101: _temp100=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp62)->f1; if((
unsigned int) _temp100 > 1u?*(( int*) _temp100) == Cyc_Absyn_Int_c: 0){ _LL105:
_temp104=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp100)->f1; goto _LL103;
_LL103: _temp102=(( struct Cyc_Absyn_Int_c_struct*) _temp100)->f2; goto _LL69;}
else{ goto _LL70;}} else{ goto _LL70;} _LL70: if(*(( int*) _temp62) == Cyc_Absyn_Const_e){
_LL107: _temp106=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp62)->f1; if((
unsigned int) _temp106 > 1u?*(( int*) _temp106) == Cyc_Absyn_LongLong_c: 0){
_LL111: _temp110=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp106)->f1;
goto _LL109; _LL109: _temp108=(( struct Cyc_Absyn_LongLong_c_struct*) _temp106)->f2;
goto _LL71;} else{ goto _LL72;}} else{ goto _LL72;} _LL72: if(*(( int*) _temp62)
== Cyc_Absyn_Const_e){ _LL113: _temp112=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp62)->f1; if( _temp112 ==( void*) Cyc_Absyn_Null_c){ goto _LL73;} else{ goto
_LL74;}} else{ goto _LL74;} _LL74: if(*(( int*) _temp62) == Cyc_Absyn_Cast_e){
_LL117: _temp116=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp62)->f1; goto
_LL115; _LL115: _temp114=(( struct Cyc_Absyn_Cast_e_struct*) _temp62)->f2; goto
_LL75;} else{ goto _LL76;} _LL76: if(*(( int*) _temp62) == Cyc_Absyn_Tuple_e){
_LL119: _temp118=(( struct Cyc_Absyn_Tuple_e_struct*) _temp62)->f1; goto _LL77;}
else{ goto _LL78;} _LL78: if(*(( int*) _temp62) == Cyc_Absyn_Array_e){ _LL121:
_temp120=(( struct Cyc_Absyn_Array_e_struct*) _temp62)->f1; goto _LL79;} else{
goto _LL80;} _LL80: if(*(( int*) _temp62) == Cyc_Absyn_Struct_e){ _LL129:
_temp128=(( struct Cyc_Absyn_Struct_e_struct*) _temp62)->f1; goto _LL127; _LL127:
_temp126=(( struct Cyc_Absyn_Struct_e_struct*) _temp62)->f2; goto _LL125; _LL125:
_temp124=(( struct Cyc_Absyn_Struct_e_struct*) _temp62)->f3; goto _LL123; _LL123:
_temp122=(( struct Cyc_Absyn_Struct_e_struct*) _temp62)->f4; goto _LL81;} else{
goto _LL82;} _LL82: if(*(( int*) _temp62) == Cyc_Absyn_CompoundLit_e){ _LL133:
_temp132=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp62)->f1; goto _LL131;
_LL131: _temp130=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp62)->f2; goto
_LL83;} else{ goto _LL84;} _LL84: if(*(( int*) _temp62) == Cyc_Absyn_UnresolvedMem_e){
_LL137: _temp136=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp62)->f1; goto
_LL135; _LL135: _temp134=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp62)->f2;
goto _LL85;} else{ goto _LL86;} _LL86: goto _LL87; _LL65: return _temp90 =='\000';
_LL67: return _temp96 == 0; _LL69: return _temp102 == 0; _LL71: return _temp108
== 0; _LL73: return 1; _LL75: return Cyc_Toc_is_zero( _temp114); _LL77: return((
int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp118); _LL79: _temp124= _temp120; goto _LL81; _LL81:
_temp130= _temp124; goto _LL83; _LL83: _temp134= _temp130; goto _LL85; _LL85:
for( 0; _temp134 != 0; _temp134=(( struct Cyc_List_List*) _check_null( _temp134))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp134))->hd)).f2)){ return 0;}} return 1; _LL87: return 0; _LL63:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp138= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp144; struct Cyc_Absyn_Conref* _temp146; struct Cyc_Absyn_Tqual
_temp148; struct Cyc_Absyn_Conref* _temp150; void* _temp152; void* _temp154;
_LL140: if(( unsigned int) _temp138 > 4u?*(( int*) _temp138) == Cyc_Absyn_PointerType:
0){ _LL145: _temp144=(( struct Cyc_Absyn_PointerType_struct*) _temp138)->f1;
_LL155: _temp154=( void*) _temp144.elt_typ; goto _LL153; _LL153: _temp152=( void*)
_temp144.rgn_typ; goto _LL151; _LL151: _temp150= _temp144.nullable; goto _LL149;
_LL149: _temp148= _temp144.tq; goto _LL147; _LL147: _temp146= _temp144.bounds;
goto _LL141;} else{ goto _LL142;} _LL142: goto _LL143; _LL141: return(( int(*)(
int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp150); _LL143:((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("is_nullable",
sizeof( unsigned char), 12u)); return 0; _LL139:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag(
struct _tuple0* x, struct _tagged_arr tag){ return({ struct _tuple0* _temp156=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp156->f1=(* x).f1;
_temp156->f2=({ struct _tagged_arr* _temp157=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp157[ 0]=( struct _tagged_arr) Cyc_String_strconcat(*(*
x).f2, tag); _temp157;}); _temp156;});} static void Cyc_Toc_add_tuple_decl(
struct _tagged_arr* x, struct Cyc_List_List* ts){ struct Cyc_List_List* _temp158=
0; goto _LL159; _LL159:{ int i= 1; for( 0; ts != 0;( ts=(( struct Cyc_List_List*)
_check_null( ts))->tl, i ++)){ _temp158=({ struct Cyc_List_List* _temp160=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp160->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp161=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp161->name=({ struct
_tagged_arr* _temp162=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp162[ 0]=( struct _tagged_arr) xprintf("f%d", i);
_temp162;}); _temp161->tq= Cyc_Toc_mt_tq; _temp161->type=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp161->width= 0; _temp161->attributes=
0; _temp161;}); _temp160->tl= _temp158; _temp160;});}} _temp158=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp158);{ struct Cyc_Absyn_Structdecl*
_temp167=({ struct Cyc_Absyn_Structdecl* _temp163=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp163->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp163->name=({ struct Cyc_Core_Opt* _temp165=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp165->v=( void*)({ struct _tuple0*
_temp166=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp166->f1=
Cyc_Toc_rel_ns; _temp166->f2= x; _temp166;}); _temp165;}); _temp163->tvs= 0;
_temp163->fields=({ struct Cyc_Core_Opt* _temp164=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp164->v=( void*) _temp158;
_temp164;}); _temp163->attributes= 0; _temp163;}); goto _LL168; _LL168: Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp169=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp169->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp170=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp170[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp171; _temp171.tag= Cyc_Absyn_Struct_d; _temp171.f1=
_temp167; _temp171;}); _temp170;}), 0); _temp169->tl= Cyc_Toc_result_decls;
_temp169;});}} struct _tuple5{ struct _tagged_arr* f1; struct Cyc_List_List* f2;
} ; static struct _tagged_arr* Cyc_Toc_add_tuple_type( struct Cyc_List_List*
tqs0){{ struct Cyc_List_List* _temp172= Cyc_Toc_tuple_types; goto _LL173; _LL173:
for( 0; _temp172 != 0; _temp172=(( struct Cyc_List_List*) _check_null( _temp172))->tl){
struct _tuple5 _temp176; struct Cyc_List_List* _temp177; struct _tagged_arr*
_temp179; struct _tuple5* _temp174=( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp172))->hd; _temp176=* _temp174; _LL180: _temp179= _temp176.f1;
goto _LL178; _LL178: _temp177= _temp176.f2; goto _LL175; _LL175: { int okay= 1;
struct Cyc_List_List* _temp181= tqs0; goto _LL182; _LL182: for( 0; _temp181 != 0?
_temp177 != 0: 0;( _temp181=(( struct Cyc_List_List*) _check_null( _temp181))->tl,
_temp177=(( struct Cyc_List_List*) _check_null( _temp177))->tl)){ if( ! Cyc_Tcutil_unify((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp181))->hd)).f2,(
void*)(( struct Cyc_List_List*) _check_null( _temp177))->hd)){ okay= 0; break;}}
if(( ! okay? 1: _temp181 != 0)? 1: _temp177 != 0){ continue;} return _temp179;}}}{
struct _tagged_arr* x=({ struct _tagged_arr* _temp185=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp185[ 0]=( struct _tagged_arr)
xprintf("_tuple%d", Cyc_Toc_tuple_type_counter ++); _temp185;}); struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); Cyc_Toc_add_tuple_decl( x, ts);
Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp183=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp183->hd=( void*)({ struct
_tuple5* _temp184=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp184->f1= x; _temp184->f2= ts; _temp184;}); _temp183->tl= Cyc_Toc_tuple_types;
_temp183;}); return x;}} struct _tuple0* Cyc_Toc_temp_var(){ return({ struct
_tuple0* _temp186=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp186->f1=( void*) Cyc_Absyn_Loc_n; _temp186->f2=({ struct _tagged_arr*
_temp187=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp187[ 0]=( struct _tagged_arr) xprintf("_temp%d", Cyc_Toc_temp_var_counter
++); _temp187;}); _temp186;});} static struct _tagged_arr* Cyc_Toc_fresh_label(){
return({ struct _tagged_arr* _temp188=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)); _temp188[ 0]=( struct _tagged_arr) xprintf("_LL%d", Cyc_Toc_temp_var_counter
++); _temp188;});} static void* Cyc_Toc_typ_to_c( void* t); static struct
_tuple1* Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp191; struct Cyc_Absyn_Tqual
_temp193; struct Cyc_Core_Opt* _temp195; struct _tuple1 _temp189=* a; _LL196:
_temp195= _temp189.f1; goto _LL194; _LL194: _temp193= _temp189.f2; goto _LL192;
_LL192: _temp191= _temp189.f3; goto _LL190; _LL190: return({ struct _tuple1*
_temp197=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp197->f1=
_temp195; _temp197->f2= _temp193; _temp197->f3= Cyc_Toc_typ_to_c( _temp191);
_temp197;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp200; struct Cyc_Absyn_Tqual _temp202; struct _tuple3 _temp198=* x;
_LL203: _temp202= _temp198.f1; goto _LL201; _LL201: _temp200= _temp198.f2; goto
_LL199; _LL199: return({ struct _tuple3* _temp204=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp204->f1= _temp202; _temp204->f2= Cyc_Toc_typ_to_c(
_temp200); _temp204;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp205= t; struct Cyc_Absyn_Exp* _temp213; struct Cyc_Absyn_Tqual _temp215;
void* _temp217; struct Cyc_Core_Opt* _temp219; int _temp221; struct Cyc_Core_Opt*
_temp223; struct Cyc_Core_Opt _temp225; void* _temp226; struct Cyc_Core_Opt*
_temp228; _LL207: if(( unsigned int) _temp205 > 4u?*(( int*) _temp205) == Cyc_Absyn_ArrayType:
0){ _LL218: _temp217=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp205)->f1;
goto _LL216; _LL216: _temp215=(( struct Cyc_Absyn_ArrayType_struct*) _temp205)->f2;
goto _LL214; _LL214: _temp213=(( struct Cyc_Absyn_ArrayType_struct*) _temp205)->f3;
goto _LL208;} else{ goto _LL209;} _LL209: if(( unsigned int) _temp205 > 4u?*((
int*) _temp205) == Cyc_Absyn_Evar: 0){ _LL229: _temp228=(( struct Cyc_Absyn_Evar_struct*)
_temp205)->f1; goto _LL224; _LL224: _temp223=(( struct Cyc_Absyn_Evar_struct*)
_temp205)->f2; if( _temp223 == 0){ goto _LL211;} else{ _temp225=* _temp223;
_LL227: _temp226=( void*) _temp225.v; goto _LL222;} _LL222: _temp221=(( struct
Cyc_Absyn_Evar_struct*) _temp205)->f3; goto _LL220; _LL220: _temp219=(( struct
Cyc_Absyn_Evar_struct*) _temp205)->f4; goto _LL210;} else{ goto _LL211;} _LL211:
goto _LL212; _LL208: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp230=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp230[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp231; _temp231.tag= Cyc_Absyn_ArrayType;
_temp231.f1=( void*) Cyc_Toc_typ_to_c_array( _temp217); _temp231.f2= _temp215;
_temp231.f3= _temp213; _temp231;}); _temp230;}); _LL210: return Cyc_Toc_typ_to_c_array(
_temp226); _LL212: return Cyc_Toc_typ_to_c( t); _LL206:;} static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c( struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield*
_temp232=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp232->name= f->name; _temp232->tq= f->tq; _temp232->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp232->width= f->width; _temp232->attributes= f->attributes;
_temp232;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp233= t; struct
Cyc_Core_Opt* _temp285; int _temp287; struct Cyc_Core_Opt* _temp289; struct Cyc_Core_Opt*
_temp291; struct Cyc_Core_Opt* _temp293; int _temp295; struct Cyc_Core_Opt*
_temp297; struct Cyc_Core_Opt _temp299; void* _temp300; struct Cyc_Core_Opt*
_temp302; struct Cyc_Absyn_Tvar* _temp304; struct Cyc_Absyn_TunionInfo _temp306;
void* _temp308; struct Cyc_List_List* _temp310; void* _temp312; struct Cyc_Absyn_Tuniondecl*
_temp314; struct Cyc_Absyn_TunionInfo _temp316; struct Cyc_Absyn_TunionFieldInfo
_temp318; struct Cyc_List_List* _temp320; void* _temp322; struct Cyc_Absyn_Tunionfield*
_temp324; struct Cyc_Absyn_Tuniondecl* _temp326; struct Cyc_Absyn_TunionFieldInfo
_temp328; struct Cyc_Absyn_PtrInfo _temp330; struct Cyc_Absyn_Conref* _temp332;
struct Cyc_Absyn_Tqual _temp334; struct Cyc_Absyn_Conref* _temp336; void*
_temp338; void* _temp340; void* _temp342; void* _temp344; struct Cyc_Absyn_Exp*
_temp346; struct Cyc_Absyn_Tqual _temp348; void* _temp350; struct Cyc_Absyn_FnInfo
_temp352; struct Cyc_List_List* _temp354; struct Cyc_List_List* _temp356; struct
Cyc_Absyn_VarargInfo* _temp358; int _temp360; struct Cyc_List_List* _temp362;
void* _temp364; struct Cyc_Core_Opt* _temp366; struct Cyc_List_List* _temp368;
struct Cyc_List_List* _temp370; struct Cyc_List_List* _temp372; struct Cyc_List_List*
_temp374; struct Cyc_Absyn_Uniondecl** _temp376; struct Cyc_List_List* _temp378;
struct _tuple0* _temp380; struct Cyc_Absyn_Structdecl** _temp382; struct Cyc_List_List*
_temp384; struct _tuple0* _temp386; struct Cyc_Absyn_Enumdecl* _temp388; struct
_tuple0* _temp390; struct Cyc_Core_Opt* _temp392; struct Cyc_List_List* _temp394;
struct _tuple0* _temp396; void* _temp398; void* _temp400; struct Cyc_List_List*
_temp402; _LL235: if( _temp233 ==( void*) Cyc_Absyn_VoidType){ goto _LL236;}
else{ goto _LL237;} _LL237: if(( unsigned int) _temp233 > 4u?*(( int*) _temp233)
== Cyc_Absyn_Evar: 0){ _LL292: _temp291=(( struct Cyc_Absyn_Evar_struct*)
_temp233)->f1; goto _LL290; _LL290: _temp289=(( struct Cyc_Absyn_Evar_struct*)
_temp233)->f2; if( _temp289 == 0){ goto _LL288;} else{ goto _LL239;} _LL288:
_temp287=(( struct Cyc_Absyn_Evar_struct*) _temp233)->f3; goto _LL286; _LL286:
_temp285=(( struct Cyc_Absyn_Evar_struct*) _temp233)->f4; goto _LL238;} else{
goto _LL239;} _LL239: if(( unsigned int) _temp233 > 4u?*(( int*) _temp233) ==
Cyc_Absyn_Evar: 0){ _LL303: _temp302=(( struct Cyc_Absyn_Evar_struct*) _temp233)->f1;
goto _LL298; _LL298: _temp297=(( struct Cyc_Absyn_Evar_struct*) _temp233)->f2;
if( _temp297 == 0){ goto _LL241;} else{ _temp299=* _temp297; _LL301: _temp300=(
void*) _temp299.v; goto _LL296;} _LL296: _temp295=(( struct Cyc_Absyn_Evar_struct*)
_temp233)->f3; goto _LL294; _LL294: _temp293=(( struct Cyc_Absyn_Evar_struct*)
_temp233)->f4; goto _LL240;} else{ goto _LL241;} _LL241: if(( unsigned int)
_temp233 > 4u?*(( int*) _temp233) == Cyc_Absyn_VarType: 0){ _LL305: _temp304=((
struct Cyc_Absyn_VarType_struct*) _temp233)->f1; goto _LL242;} else{ goto _LL243;}
_LL243: if(( unsigned int) _temp233 > 4u?*(( int*) _temp233) == Cyc_Absyn_TunionType:
0){ _LL307: _temp306=(( struct Cyc_Absyn_TunionType_struct*) _temp233)->f1;
_LL313: _temp312=( void*) _temp306.tunion_info; if(*(( int*) _temp312) == Cyc_Absyn_KnownTunion){
_LL315: _temp314=(( struct Cyc_Absyn_KnownTunion_struct*) _temp312)->f1; goto
_LL311;} else{ goto _LL245;} _LL311: _temp310= _temp306.targs; goto _LL309;
_LL309: _temp308=( void*) _temp306.rgn; goto _LL244;} else{ goto _LL245;} _LL245:
if(( unsigned int) _temp233 > 4u?*(( int*) _temp233) == Cyc_Absyn_TunionType: 0){
_LL317: _temp316=(( struct Cyc_Absyn_TunionType_struct*) _temp233)->f1; goto
_LL246;} else{ goto _LL247;} _LL247: if(( unsigned int) _temp233 > 4u?*(( int*)
_temp233) == Cyc_Absyn_TunionFieldType: 0){ _LL319: _temp318=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp233)->f1; _LL323: _temp322=( void*) _temp318.field_info; if(*(( int*)
_temp322) == Cyc_Absyn_KnownTunionfield){ _LL327: _temp326=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp322)->f1; goto _LL325; _LL325: _temp324=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp322)->f2; goto _LL321;} else{ goto _LL249;} _LL321: _temp320= _temp318.targs;
goto _LL248;} else{ goto _LL249;} _LL249: if(( unsigned int) _temp233 > 4u?*((
int*) _temp233) == Cyc_Absyn_TunionFieldType: 0){ _LL329: _temp328=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp233)->f1; goto _LL250;} else{ goto _LL251;} _LL251: if(( unsigned int)
_temp233 > 4u?*(( int*) _temp233) == Cyc_Absyn_PointerType: 0){ _LL331: _temp330=((
struct Cyc_Absyn_PointerType_struct*) _temp233)->f1; _LL341: _temp340=( void*)
_temp330.elt_typ; goto _LL339; _LL339: _temp338=( void*) _temp330.rgn_typ; goto
_LL337; _LL337: _temp336= _temp330.nullable; goto _LL335; _LL335: _temp334=
_temp330.tq; goto _LL333; _LL333: _temp332= _temp330.bounds; goto _LL252;} else{
goto _LL253;} _LL253: if(( unsigned int) _temp233 > 4u?*(( int*) _temp233) ==
Cyc_Absyn_IntType: 0){ _LL345: _temp344=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp233)->f1; goto _LL343; _LL343: _temp342=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp233)->f2; goto _LL254;} else{ goto _LL255;} _LL255: if( _temp233 ==( void*)
Cyc_Absyn_FloatType){ goto _LL256;} else{ goto _LL257;} _LL257: if( _temp233 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL258;} else{ goto _LL259;} _LL259: if((
unsigned int) _temp233 > 4u?*(( int*) _temp233) == Cyc_Absyn_ArrayType: 0){
_LL351: _temp350=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp233)->f1;
goto _LL349; _LL349: _temp348=(( struct Cyc_Absyn_ArrayType_struct*) _temp233)->f2;
goto _LL347; _LL347: _temp346=(( struct Cyc_Absyn_ArrayType_struct*) _temp233)->f3;
goto _LL260;} else{ goto _LL261;} _LL261: if(( unsigned int) _temp233 > 4u?*((
int*) _temp233) == Cyc_Absyn_FnType: 0){ _LL353: _temp352=(( struct Cyc_Absyn_FnType_struct*)
_temp233)->f1; _LL369: _temp368= _temp352.tvars; goto _LL367; _LL367: _temp366=
_temp352.effect; goto _LL365; _LL365: _temp364=( void*) _temp352.ret_typ; goto
_LL363; _LL363: _temp362= _temp352.args; goto _LL361; _LL361: _temp360= _temp352.c_varargs;
goto _LL359; _LL359: _temp358= _temp352.cyc_varargs; goto _LL357; _LL357:
_temp356= _temp352.rgn_po; goto _LL355; _LL355: _temp354= _temp352.attributes;
goto _LL262;} else{ goto _LL263;} _LL263: if(( unsigned int) _temp233 > 4u?*((
int*) _temp233) == Cyc_Absyn_TupleType: 0){ _LL371: _temp370=(( struct Cyc_Absyn_TupleType_struct*)
_temp233)->f1; goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int)
_temp233 > 4u?*(( int*) _temp233) == Cyc_Absyn_AnonStructType: 0){ _LL373:
_temp372=(( struct Cyc_Absyn_AnonStructType_struct*) _temp233)->f1; goto _LL266;}
else{ goto _LL267;} _LL267: if(( unsigned int) _temp233 > 4u?*(( int*) _temp233)
== Cyc_Absyn_AnonUnionType: 0){ _LL375: _temp374=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp233)->f1; goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int)
_temp233 > 4u?*(( int*) _temp233) == Cyc_Absyn_UnionType: 0){ _LL381: _temp380=((
struct Cyc_Absyn_UnionType_struct*) _temp233)->f1; goto _LL379; _LL379: _temp378=((
struct Cyc_Absyn_UnionType_struct*) _temp233)->f2; goto _LL377; _LL377: _temp376=((
struct Cyc_Absyn_UnionType_struct*) _temp233)->f3; goto _LL270;} else{ goto
_LL271;} _LL271: if(( unsigned int) _temp233 > 4u?*(( int*) _temp233) == Cyc_Absyn_StructType:
0){ _LL387: _temp386=(( struct Cyc_Absyn_StructType_struct*) _temp233)->f1; goto
_LL385; _LL385: _temp384=(( struct Cyc_Absyn_StructType_struct*) _temp233)->f2;
goto _LL383; _LL383: _temp382=(( struct Cyc_Absyn_StructType_struct*) _temp233)->f3;
goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int) _temp233 > 4u?*((
int*) _temp233) == Cyc_Absyn_EnumType: 0){ _LL391: _temp390=(( struct Cyc_Absyn_EnumType_struct*)
_temp233)->f1; goto _LL389; _LL389: _temp388=(( struct Cyc_Absyn_EnumType_struct*)
_temp233)->f2; goto _LL274;} else{ goto _LL275;} _LL275: if(( unsigned int)
_temp233 > 4u?*(( int*) _temp233) == Cyc_Absyn_TypedefType: 0){ _LL397: _temp396=((
struct Cyc_Absyn_TypedefType_struct*) _temp233)->f1; goto _LL395; _LL395:
_temp394=(( struct Cyc_Absyn_TypedefType_struct*) _temp233)->f2; goto _LL393;
_LL393: _temp392=(( struct Cyc_Absyn_TypedefType_struct*) _temp233)->f3; goto
_LL276;} else{ goto _LL277;} _LL277: if(( unsigned int) _temp233 > 4u?*(( int*)
_temp233) == Cyc_Absyn_RgnHandleType: 0){ _LL399: _temp398=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp233)->f1; goto _LL278;} else{ goto _LL279;} _LL279: if( _temp233 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL280;} else{ goto _LL281;} _LL281: if(( unsigned int)
_temp233 > 4u?*(( int*) _temp233) == Cyc_Absyn_AccessEff: 0){ _LL401: _temp400=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp233)->f1; goto _LL282;} else{
goto _LL283;} _LL283: if(( unsigned int) _temp233 > 4u?*(( int*) _temp233) ==
Cyc_Absyn_JoinEff: 0){ _LL403: _temp402=(( struct Cyc_Absyn_JoinEff_struct*)
_temp233)->f1; goto _LL284;} else{ goto _LL234;} _LL236: return t; _LL238:
return Cyc_Absyn_sint_t; _LL240: return Cyc_Toc_typ_to_c( _temp300); _LL242:
return Cyc_Absyn_void_star_typ(); _LL244: return Cyc_Absyn_void_star_typ();
_LL246:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("unresolved TunionType", sizeof( unsigned char), 22u))); _LL248: if(
_temp324->typs == 0){ if( _temp326->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp324->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL250:( int) _throw((( void*(*)(
struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u))); _LL252: _temp340= Cyc_Toc_typ_to_c_array(
_temp340);{ void* _temp404=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp332))->v; void* _temp410; _LL406: if((
unsigned int) _temp404 > 1u?*(( int*) _temp404) == Cyc_Absyn_Eq_constr: 0){
_LL411: _temp410=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp404)->f1;
if( _temp410 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL407;} else{ goto _LL408;}}
else{ goto _LL408;} _LL408: goto _LL409; _LL407: return Cyc_Toc_tagged_arr_typ;
_LL409: return Cyc_Absyn_star_typ( _temp340,( void*) Cyc_Absyn_HeapRgn, _temp334);
_LL405:;} _LL254: return t; _LL256: return t; _LL258: return t; _LL260: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp412=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp412[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp413; _temp413.tag= Cyc_Absyn_ArrayType; _temp413.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp350); _temp413.f2= _temp348; _temp413.f3=
_temp346; _temp413;}); _temp412;}); _LL262: { struct Cyc_List_List* _temp414=
_temp354; goto _LL415; _LL415: { struct Cyc_List_List* _temp416= 0; goto _LL417;
_LL417: for( 0; _temp354 != 0; _temp354=(( struct Cyc_List_List*) _check_null(
_temp354))->tl){ void* _temp418=( void*)(( struct Cyc_List_List*) _check_null(
_temp354))->hd; _LL420: if( _temp418 ==( void*) Cyc_Absyn_Noreturn_att){ goto
_LL421;} else{ goto _LL422;} _LL422: if( _temp418 ==( void*) Cyc_Absyn_Const_att){
goto _LL423;} else{ goto _LL424;} _LL424: goto _LL425; _LL421: goto _LL423;
_LL423: continue; _LL425: _temp416=({ struct Cyc_List_List* _temp426=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp426->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( _temp354))->hd); _temp426->tl=
_temp416; _temp426;}); goto _LL419; _LL419:;}{ struct Cyc_List_List* _temp427=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp362); goto _LL428; _LL428: if(
_temp358 != 0){ void* _temp429= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp358))->type,( void*)(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp358))->rgn, Cyc_Toc_mt_tq)); goto
_LL430; _LL430: { struct _tuple1* _temp432=({ struct _tuple1* _temp431=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp431->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp358))->name; _temp431->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp358))->tq; _temp431->f3= _temp429; _temp431;}); goto _LL433;
_LL433: _temp427=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_imp_append)( _temp427,({ struct Cyc_List_List*
_temp434=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp434->hd=( void*) _temp432; _temp434->tl= 0; _temp434;}));}} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp435=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp435[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp436; _temp436.tag= Cyc_Absyn_FnType; _temp436.f1=({ struct Cyc_Absyn_FnInfo
_temp437; _temp437.tvars= 0; _temp437.effect= 0; _temp437.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp364); _temp437.args= _temp427; _temp437.c_varargs= _temp360; _temp437.cyc_varargs=
0; _temp437.rgn_po= 0; _temp437.attributes= _temp416; _temp437;}); _temp436;});
_temp435;});}}} _LL264: _temp370=(( struct Cyc_List_List*(*)( struct _tuple3*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp370);{ struct _tagged_arr* _temp438= Cyc_Toc_add_tuple_type( _temp370);
goto _LL439; _LL439: return Cyc_Absyn_strct( _temp438);} _LL266: return( void*)({
struct Cyc_Absyn_AnonStructType_struct* _temp440=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp440[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp441; _temp441.tag= Cyc_Absyn_AnonStructType;
_temp441.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp372); _temp441;}); _temp440;}); _LL268: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp442=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp442[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp443; _temp443.tag=
Cyc_Absyn_AnonUnionType; _temp443.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp374); _temp443;}); _temp442;}); _LL270: if( _temp380 == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous union", sizeof(
unsigned char), 16u));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp380)); _LL272: if( _temp386 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous struct", sizeof(
unsigned char), 17u));} return Cyc_Absyn_strctq(( struct _tuple0*) _check_null(
_temp386)); _LL274: return t; _LL276: if( _temp392 == 0){ return(( void*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: unresolved TypedefType",
sizeof( unsigned char), 38u));} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp444=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp444[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp445; _temp445.tag= Cyc_Absyn_TypedefType;
_temp445.f1= _temp396; _temp445.f2= 0; _temp445.f3=({ struct Cyc_Core_Opt*
_temp446=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp446->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp392))->v); _temp446;}); _temp445;}); _temp444;}); _LL278:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL280: return(( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u)); _LL282: return(( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u)); _LL284: return(( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u)); _LL234:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp447= t; struct Cyc_Absyn_Exp* _temp453; struct Cyc_Absyn_Tqual _temp455;
void* _temp457; _LL449: if(( unsigned int) _temp447 > 4u?*(( int*) _temp447) ==
Cyc_Absyn_ArrayType: 0){ _LL458: _temp457=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp447)->f1; goto _LL456; _LL456: _temp455=(( struct Cyc_Absyn_ArrayType_struct*)
_temp447)->f2; goto _LL454; _LL454: _temp453=(( struct Cyc_Absyn_ArrayType_struct*)
_temp447)->f3; goto _LL450;} else{ goto _LL451;} _LL451: goto _LL452; _LL450:
return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ( _temp457,( void*) Cyc_Absyn_HeapRgn,
_temp455), e, l); _LL452: return Cyc_Absyn_cast_exp( t, e, l); _LL448:;} static
int Cyc_Toc_atomic_typ( void* t){ void* _temp459= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp495; void* _temp497; void* _temp499; struct Cyc_Absyn_FnInfo
_temp501; struct Cyc_Absyn_Exp* _temp503; struct Cyc_Absyn_Tqual _temp505; void*
_temp507; struct Cyc_List_List* _temp509; struct Cyc_Absyn_Structdecl** _temp511;
struct Cyc_List_List* _temp513; struct _tuple0* _temp515; struct Cyc_Absyn_TunionInfo
_temp517; struct Cyc_List_List* _temp519; struct Cyc_List_List* _temp521; struct
Cyc_Absyn_TunionFieldInfo _temp523; struct Cyc_List_List* _temp525; void*
_temp527; struct Cyc_Absyn_Tunionfield* _temp529; struct Cyc_Absyn_Tuniondecl*
_temp531; struct Cyc_Absyn_Uniondecl** _temp533; struct Cyc_List_List* _temp535;
struct _tuple0* _temp537; struct Cyc_Absyn_PtrInfo _temp539; void* _temp541;
_LL461: if( _temp459 ==( void*) Cyc_Absyn_VoidType){ goto _LL462;} else{ goto
_LL463;} _LL463: if(( unsigned int) _temp459 > 4u?*(( int*) _temp459) == Cyc_Absyn_VarType:
0){ _LL496: _temp495=(( struct Cyc_Absyn_VarType_struct*) _temp459)->f1; goto
_LL464;} else{ goto _LL465;} _LL465: if(( unsigned int) _temp459 > 4u?*(( int*)
_temp459) == Cyc_Absyn_IntType: 0){ _LL500: _temp499=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp459)->f1; goto _LL498; _LL498: _temp497=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp459)->f2; goto _LL466;} else{ goto _LL467;} _LL467: if( _temp459 ==( void*)
Cyc_Absyn_FloatType){ goto _LL468;} else{ goto _LL469;} _LL469: if( _temp459 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL470;} else{ goto _LL471;} _LL471: if((
unsigned int) _temp459 > 4u?*(( int*) _temp459) == Cyc_Absyn_FnType: 0){ _LL502:
_temp501=(( struct Cyc_Absyn_FnType_struct*) _temp459)->f1; goto _LL472;} else{
goto _LL473;} _LL473: if(( unsigned int) _temp459 > 4u?*(( int*) _temp459) ==
Cyc_Absyn_ArrayType: 0){ _LL508: _temp507=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp459)->f1; goto _LL506; _LL506: _temp505=(( struct Cyc_Absyn_ArrayType_struct*)
_temp459)->f2; goto _LL504; _LL504: _temp503=(( struct Cyc_Absyn_ArrayType_struct*)
_temp459)->f3; goto _LL474;} else{ goto _LL475;} _LL475: if(( unsigned int)
_temp459 > 4u?*(( int*) _temp459) == Cyc_Absyn_TupleType: 0){ _LL510: _temp509=((
struct Cyc_Absyn_TupleType_struct*) _temp459)->f1; goto _LL476;} else{ goto
_LL477;} _LL477: if(( unsigned int) _temp459 > 4u?*(( int*) _temp459) == Cyc_Absyn_StructType:
0){ _LL516: _temp515=(( struct Cyc_Absyn_StructType_struct*) _temp459)->f1; goto
_LL514; _LL514: _temp513=(( struct Cyc_Absyn_StructType_struct*) _temp459)->f2;
goto _LL512; _LL512: _temp511=(( struct Cyc_Absyn_StructType_struct*) _temp459)->f3;
goto _LL478;} else{ goto _LL479;} _LL479: if(( unsigned int) _temp459 > 4u?*((
int*) _temp459) == Cyc_Absyn_TunionType: 0){ _LL518: _temp517=(( struct Cyc_Absyn_TunionType_struct*)
_temp459)->f1; goto _LL480;} else{ goto _LL481;} _LL481: if(( unsigned int)
_temp459 > 4u?*(( int*) _temp459) == Cyc_Absyn_AnonStructType: 0){ _LL520:
_temp519=(( struct Cyc_Absyn_AnonStructType_struct*) _temp459)->f1; goto _LL482;}
else{ goto _LL483;} _LL483: if(( unsigned int) _temp459 > 4u?*(( int*) _temp459)
== Cyc_Absyn_AnonUnionType: 0){ _LL522: _temp521=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp459)->f1; goto _LL484;} else{ goto _LL485;} _LL485: if(( unsigned int)
_temp459 > 4u?*(( int*) _temp459) == Cyc_Absyn_TunionFieldType: 0){ _LL524:
_temp523=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp459)->f1; _LL528:
_temp527=( void*) _temp523.field_info; if(*(( int*) _temp527) == Cyc_Absyn_KnownTunionfield){
_LL532: _temp531=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp527)->f1;
goto _LL530; _LL530: _temp529=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp527)->f2; goto _LL526;} else{ goto _LL487;} _LL526: _temp525= _temp523.targs;
goto _LL486;} else{ goto _LL487;} _LL487: if(( unsigned int) _temp459 > 4u?*((
int*) _temp459) == Cyc_Absyn_UnionType: 0){ _LL538: _temp537=(( struct Cyc_Absyn_UnionType_struct*)
_temp459)->f1; goto _LL536; _LL536: _temp535=(( struct Cyc_Absyn_UnionType_struct*)
_temp459)->f2; goto _LL534; _LL534: _temp533=(( struct Cyc_Absyn_UnionType_struct*)
_temp459)->f3; goto _LL488;} else{ goto _LL489;} _LL489: if(( unsigned int)
_temp459 > 4u?*(( int*) _temp459) == Cyc_Absyn_PointerType: 0){ _LL540: _temp539=((
struct Cyc_Absyn_PointerType_struct*) _temp459)->f1; goto _LL490;} else{ goto
_LL491;} _LL491: if(( unsigned int) _temp459 > 4u?*(( int*) _temp459) == Cyc_Absyn_RgnHandleType:
0){ _LL542: _temp541=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp459)->f1;
goto _LL492;} else{ goto _LL493;} _LL493: goto _LL494; _LL462: return 1; _LL464:
return 0; _LL466: return 1; _LL468: return 1; _LL470: return 1; _LL472: return 1;
_LL474: return Cyc_Toc_atomic_typ( _temp507); _LL476: for( 0; _temp509 != 0;
_temp509=(( struct Cyc_List_List*) _check_null( _temp509))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp509))->hd)).f2)){
return 0;}} return 1; _LL478: return 0; _LL480: return 0; _LL482: _temp521=
_temp519; goto _LL484; _LL484: for( 0; _temp521 != 0; _temp521=(( struct Cyc_List_List*)
_check_null( _temp521))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp521))->hd)->type)){ return 0;}} return
1; _LL486: return _temp529->typs == 0; _LL488: return 0; _LL490: return 0;
_LL492: return 0; _LL494:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp543= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _temp543.last_plus_one - _temp543.curr,
_temp543.curr);})); return 0; _LL460:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp544= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp550;
struct Cyc_Absyn_Conref* _temp552; struct Cyc_Absyn_Tqual _temp554; struct Cyc_Absyn_Conref*
_temp556; void* _temp558; void* _temp560; _LL546: if(( unsigned int) _temp544 >
4u?*(( int*) _temp544) == Cyc_Absyn_PointerType: 0){ _LL551: _temp550=(( struct
Cyc_Absyn_PointerType_struct*) _temp544)->f1; _LL561: _temp560=( void*) _temp550.elt_typ;
goto _LL559; _LL559: _temp558=( void*) _temp550.rgn_typ; goto _LL557; _LL557:
_temp556= _temp550.nullable; goto _LL555; _LL555: _temp554= _temp550.tq; goto
_LL553; _LL553: _temp552= _temp550.bounds; goto _LL547;} else{ goto _LL548;}
_LL548: goto _LL549; _LL547: { void* _temp562= Cyc_Tcutil_compress( _temp560);
_LL564: if( _temp562 ==( void*) Cyc_Absyn_VoidType){ goto _LL565;} else{ goto
_LL566;} _LL566: goto _LL567; _LL565: return 1; _LL567: return 0; _LL563:;}
_LL549: return 0; _LL545:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_arr* f){ void* _temp568= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp580; struct Cyc_Absyn_Structdecl* _temp582; struct Cyc_List_List* _temp583;
struct _tuple0* _temp585; struct Cyc_List_List* _temp587; struct Cyc_List_List*
_temp589; struct Cyc_Absyn_Uniondecl** _temp591; struct Cyc_List_List* _temp593;
struct _tuple0* _temp595; _LL570: if(( unsigned int) _temp568 > 4u?*(( int*)
_temp568) == Cyc_Absyn_StructType: 0){ _LL586: _temp585=(( struct Cyc_Absyn_StructType_struct*)
_temp568)->f1; goto _LL584; _LL584: _temp583=(( struct Cyc_Absyn_StructType_struct*)
_temp568)->f2; goto _LL581; _LL581: _temp580=(( struct Cyc_Absyn_StructType_struct*)
_temp568)->f3; if( _temp580 == 0){ goto _LL572;} else{ _temp582=* _temp580; goto
_LL571;}} else{ goto _LL572;} _LL572: if(( unsigned int) _temp568 > 4u?*(( int*)
_temp568) == Cyc_Absyn_AnonStructType: 0){ _LL588: _temp587=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp568)->f1; goto _LL573;} else{ goto _LL574;} _LL574: if(( unsigned int)
_temp568 > 4u?*(( int*) _temp568) == Cyc_Absyn_AnonUnionType: 0){ _LL590:
_temp589=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp568)->f1; goto _LL575;}
else{ goto _LL576;} _LL576: if(( unsigned int) _temp568 > 4u?*(( int*) _temp568)
== Cyc_Absyn_UnionType: 0){ _LL596: _temp595=(( struct Cyc_Absyn_UnionType_struct*)
_temp568)->f1; goto _LL594; _LL594: _temp593=(( struct Cyc_Absyn_UnionType_struct*)
_temp568)->f2; goto _LL592; _LL592: _temp591=(( struct Cyc_Absyn_UnionType_struct*)
_temp568)->f3; goto _LL577;} else{ goto _LL578;} _LL578: goto _LL579; _LL571:
if( _temp582->fields == 0){( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: struct missing fields", sizeof( unsigned char), 37u)));}
_temp587=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp582->fields))->v;
goto _LL573; _LL573: { struct Cyc_Absyn_Structfield* _temp597= Cyc_Absyn_lookup_field(
_temp587, f); goto _LL598; _LL598: if( _temp597 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp599=*(( struct Cyc_Absyn_Structfield*) _check_null( _temp597))->name;
xprintf("is_poly_field: bad field %.*s", _temp599.last_plus_one - _temp599.curr,
_temp599.curr);}));} return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp597))->type);} _LL575: return 0; _LL577: return 0; _LL579:(
int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct
_tagged_arr)({ struct _tagged_arr _temp600= Cyc_Absynpp_typ2string( t); xprintf("is_poly_field: bad type %.*s",
_temp600.last_plus_one - _temp600.curr, _temp600.curr);}))); _LL569:;} static
int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp601=( void*) e->r;
struct _tagged_arr* _temp609; struct Cyc_Absyn_Exp* _temp611; struct _tagged_arr*
_temp613; struct Cyc_Absyn_Exp* _temp615; _LL603: if(*(( int*) _temp601) == Cyc_Absyn_StructMember_e){
_LL612: _temp611=(( struct Cyc_Absyn_StructMember_e_struct*) _temp601)->f1; goto
_LL610; _LL610: _temp609=(( struct Cyc_Absyn_StructMember_e_struct*) _temp601)->f2;
goto _LL604;} else{ goto _LL605;} _LL605: if(*(( int*) _temp601) == Cyc_Absyn_StructArrow_e){
_LL616: _temp615=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp601)->f1; goto
_LL614; _LL614: _temp613=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp601)->f2;
goto _LL606;} else{ goto _LL607;} _LL607: goto _LL608; _LL604: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp611->topt))->v, _temp609);
_LL606: { void* _temp617= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp615->topt))->v); struct Cyc_Absyn_PtrInfo _temp623; struct Cyc_Absyn_Conref*
_temp625; struct Cyc_Absyn_Tqual _temp627; struct Cyc_Absyn_Conref* _temp629;
void* _temp631; void* _temp633; _LL619: if(( unsigned int) _temp617 > 4u?*(( int*)
_temp617) == Cyc_Absyn_PointerType: 0){ _LL624: _temp623=(( struct Cyc_Absyn_PointerType_struct*)
_temp617)->f1; _LL634: _temp633=( void*) _temp623.elt_typ; goto _LL632; _LL632:
_temp631=( void*) _temp623.rgn_typ; goto _LL630; _LL630: _temp629= _temp623.nullable;
goto _LL628; _LL628: _temp627= _temp623.tq; goto _LL626; _LL626: _temp625=
_temp623.bounds; goto _LL620;} else{ goto _LL621;} _LL621: goto _LL622; _LL620:
return Cyc_Toc_is_poly_field( _temp633, _temp613); _LL622:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp635= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp615->topt))->v); xprintf("is_poly_project:  bad type %.*s", _temp635.last_plus_one
- _temp635.curr, _temp635.curr);})); return 0; _LL618:;} _LL608: return 0;
_LL602:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp*
s){ return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_e,({ struct Cyc_List_List*
_temp636=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp636->hd=( void*) s; _temp636->tl= 0; _temp636;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc_GC_malloc_atomic_e,({ struct Cyc_List_List* _temp637=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp637->hd=( void*) s; _temp637->tl=
0; _temp637;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t,
struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic(
s);} return Cyc_Toc_malloc_ptr( s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({ struct Cyc_List_List*(* _temp638)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp641= rgn; struct Cyc_Absyn_Exp* _temp642= s; struct
Cyc_Absyn_Exp* _temp639[ 2u]={ _temp641, _temp642}; struct _tagged_arr _temp640={(
void*) _temp639,( void*) _temp639,( void*)( _temp639 + 2u)}; _temp638( _temp640);}),
0);} static unsigned char _temp643[ 3u]="f0"; static struct _tagged_arr Cyc_Toc_f0={
_temp643, _temp643, _temp643 + 3u}; static struct _tagged_arr* Cyc_Toc_field_names_v[
1u]={& Cyc_Toc_f0}; static struct _tagged_arr Cyc_Toc_field_names={( void*)((
struct _tagged_arr**) Cyc_Toc_field_names_v),( void*)(( struct _tagged_arr**)
Cyc_Toc_field_names_v),( void*)(( struct _tagged_arr**) Cyc_Toc_field_names_v) +
1u}; static struct _tagged_arr* Cyc_Toc_fieldname( int i){ unsigned int fsz=
_get_arr_size( Cyc_Toc_field_names, sizeof( struct _tagged_arr*)); if( i >= fsz){
Cyc_Toc_field_names=({ unsigned int _temp644=( unsigned int)( i + 1); struct
_tagged_arr** _temp645=( struct _tagged_arr**) GC_malloc( sizeof( struct
_tagged_arr*) * _temp644); struct _tagged_arr _temp648= _tag_arr( _temp645,
sizeof( struct _tagged_arr*),( unsigned int)( i + 1));{ unsigned int _temp646=
_temp644; unsigned int j; for( j= 0; j < _temp646; j ++){ _temp645[ j]= j < fsz?*((
struct _tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names, sizeof(
struct _tagged_arr*),( int) j)):({ struct _tagged_arr* _temp647=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp647[ 0]=( struct
_tagged_arr) xprintf("f%d",( int) j); _temp647;});}}; _temp648;});} return*((
struct _tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names, sizeof(
struct _tagged_arr*), i));} static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv( struct
Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__sfile_to_file_e,({
struct Cyc_List_List* _temp649=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp649->hd=( void*) e; _temp649->tl= 0; _temp649;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({ struct Cyc_List_List* _temp650=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp650->hd=(
void*) e; _temp650->tl= 0; _temp650;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_make_toplevel_tagged_arr(
void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ void* elt_typ;
struct Cyc_Absyn_Tqual elt_tqual;{ void* _temp651= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp657; struct Cyc_Absyn_Conref* _temp659; struct Cyc_Absyn_Tqual
_temp661; struct Cyc_Absyn_Conref* _temp663; void* _temp665; void* _temp667;
_LL653: if(( unsigned int) _temp651 > 4u?*(( int*) _temp651) == Cyc_Absyn_PointerType:
0){ _LL658: _temp657=(( struct Cyc_Absyn_PointerType_struct*) _temp651)->f1;
_LL668: _temp667=( void*) _temp657.elt_typ; goto _LL666; _LL666: _temp665=( void*)
_temp657.rgn_typ; goto _LL664; _LL664: _temp663= _temp657.nullable; goto _LL662;
_LL662: _temp661= _temp657.tq; goto _LL660; _LL660: _temp659= _temp657.bounds;
goto _LL654;} else{ goto _LL655;} _LL655: goto _LL656; _LL654: elt_typ= Cyc_Toc_typ_to_c(
_temp667); elt_tqual= _temp661; goto _LL652; _LL656: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("make_toplevel_tagged_arr: not a pointer",
sizeof( unsigned char), 40u)); _LL652:;}{ int is_string= 0;{ void* _temp669=(
void*) e->r; void* _temp675; struct _tagged_arr _temp677; _LL671: if(*(( int*)
_temp669) == Cyc_Absyn_Const_e){ _LL676: _temp675=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp669)->f1; if(( unsigned int) _temp675 > 1u?*(( int*) _temp675) == Cyc_Absyn_String_c:
0){ _LL678: _temp677=(( struct Cyc_Absyn_String_c_struct*) _temp675)->f1; goto
_LL672;} else{ goto _LL673;}} else{ goto _LL673;} _LL673: goto _LL674; _LL672:
is_string= 1; goto _LL670; _LL674: goto _LL670; _LL670:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp682=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp682[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp683; _temp683.tag= Cyc_Absyn_ArrayType; _temp683.f1=(
void*) elt_typ; _temp683.f2= Cyc_Toc_mt_tq; _temp683.f3=( struct Cyc_Absyn_Exp*)
sz; _temp683;}); _temp682;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp679=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp679->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp680=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp680[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp681; _temp681.tag= Cyc_Absyn_Var_d;
_temp681.f1= vd; _temp681;}); _temp680;}), 0); _temp679->tl= Cyc_Toc_result_decls;
_temp679;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct Cyc_List_List*(* _temp684)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct _tuple4* _temp687=({ struct _tuple4*
_temp692=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp692->f1= 0;
_temp692->f2= xexp; _temp692;}); struct _tuple4* _temp688=({ struct _tuple4*
_temp691=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp691->f1= 0;
_temp691->f2= xexp; _temp691;}); struct _tuple4* _temp689=({ struct _tuple4*
_temp690=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp690->f1= 0;
_temp690->f2= xplussz; _temp690;}); struct _tuple4* _temp685[ 3u]={ _temp687,
_temp688, _temp689}; struct _tagged_arr _temp686={( void*) _temp685,( void*)
_temp685,( void*)( _temp685 + 3u)}; _temp684( _temp686);}), 0); return urm_exp;}}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp693=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp693->break_lab= 0; _temp693->continue_lab= 0; _temp693->fallthru_info=
0; _temp693->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp693->toplevel= 1;
_temp693;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp694=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp694->break_lab= e->break_lab; _temp694->continue_lab=
e->continue_lab; _temp694->fallthru_info= e->fallthru_info; _temp694->varmap= e->varmap;
_temp694->toplevel= e->toplevel; _temp694;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp695=(* x).f1; struct
Cyc_List_List* _temp701; _LL697: if(( unsigned int) _temp695 > 1u?*(( int*)
_temp695) == Cyc_Absyn_Rel_n: 0){ _LL702: _temp701=(( struct Cyc_Absyn_Rel_n_struct*)
_temp695)->f1; goto _LL698;} else{ goto _LL699;} _LL699: goto _LL700; _LL698:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp703=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp703[ 0]=({ struct
Cyc_Core_Impossible_struct _temp704; _temp704.tag= Cyc_Core_Impossible; _temp704.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp705= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp705.last_plus_one - _temp705.curr,
_temp705.curr);}); _temp704;}); _temp703;})); _LL700: goto _LL696; _LL696:;}{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)(
ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=
0; ans->continue_lab= 0; return ans;} struct _tuple6{ struct _tagged_arr* f1;
struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_arr*
break_l, struct _tagged_arr* fallthru_l, struct Cyc_List_List* fallthru_binders,
struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List* _temp706= 0; goto
_LL707; _LL707: for( 0; fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*)
_check_null( fallthru_binders))->tl){ _temp706=({ struct Cyc_List_List* _temp708=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp708->hd=(
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
fallthru_binders))->hd)->name; _temp708->tl= _temp706; _temp708;});} _temp706=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp706);{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt*
_temp709=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp709->v=( void*) break_l; _temp709;}); ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp710=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp710->v=( void*)({ struct _tuple6* _temp711=( struct _tuple6*) GC_malloc(
sizeof( struct _tuple6)); _temp711->f1= fallthru_l; _temp711->f2= _temp706;
_temp711->f3= next_case_env->varmap; _temp711;}); _temp710;}); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e,
struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp712=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp712->v=( void*) break_l; _temp712;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp713=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp713->v=( void*)({
struct _tuple6* _temp714=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp714->f1= next_l; _temp714->f2= 0; _temp714->f3=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp714;}); _temp713;}); return ans;} struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4;
unsigned char f5; int f6; } ; struct _tuple8{ struct _tuple0* f1; void* f2;
struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("expecting a literal format string", sizeof(
unsigned char), 34u));}{ struct _tagged_arr fmt_str;{ void* _temp715=( void*)((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd)->r;
void* _temp721; struct _tagged_arr _temp723; _LL717: if(*(( int*) _temp715) ==
Cyc_Absyn_Const_e){ _LL722: _temp721=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp715)->f1; if(( unsigned int) _temp721 > 1u?*(( int*) _temp721) == Cyc_Absyn_String_c:
0){ _LL724: _temp723=(( struct Cyc_Absyn_String_c_struct*) _temp721)->f1; goto
_LL718;} else{ goto _LL719;}} else{ goto _LL719;} _LL719: goto _LL720; _LL718:
fmt_str= _temp723; goto _LL716; _LL720: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("expecting a literal format string",
sizeof( unsigned char), 34u)); _LL716:;}{ int len=( int) _get_arr_size( fmt_str,
sizeof( unsigned char)); int i= 0; for( i= 0; i < len; i ++){ if(*(( const
unsigned char*) _check_unknown_subscript( fmt_str, sizeof( unsigned char), i))
=='%'){ i ++; if( i < len?*(( const unsigned char*) _check_unknown_subscript(
fmt_str, sizeof( unsigned char), i)) =='s': 0){ break;}}} if( i == len){ if(
aopt != 0){ args=({ struct Cyc_List_List* _temp725=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp725->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp725->tl= args; _temp725;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp726= _new_region(); struct
_RegionHandle* r=& _temp726; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*)
_check_unknown_subscript( fmt_str, sizeof( unsigned char), i)); rev_fmt=({
struct Cyc_List_List* _temp727=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp727->hd=( void*)(( int) c); _temp727->tl=
rev_fmt; _temp727;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* _temp728=
Cyc_Formatstr_parse_conversionspecification( fmt_str, i + 1); goto _LL729;
_LL729: if( _temp728 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("bad format string", sizeof( unsigned char), 18u));}{ struct _tuple7
_temp732; int _temp733; unsigned char _temp735; struct Cyc_List_List* _temp737;
struct Cyc_List_List* _temp739; struct Cyc_List_List* _temp741; struct Cyc_List_List*
_temp743; struct _tuple7* _temp730=( struct _tuple7*)(( struct Cyc_Core_Opt*)
_check_null( _temp728))->v; _temp732=* _temp730; _LL744: _temp743= _temp732.f1;
goto _LL742; _LL742: _temp741= _temp732.f2; goto _LL740; _LL740: _temp739=
_temp732.f3; goto _LL738; _LL738: _temp737= _temp732.f4; goto _LL736; _LL736:
_temp735= _temp732.f5; goto _LL734; _LL734: _temp733= _temp732.f6; goto _LL731;
_LL731: i= _temp733 - 1; if( _temp735 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)(
r,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x))
Cyc_List_rflatten)( r,({ struct Cyc_List_List*(* _temp745)( struct _RegionHandle*,
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_arr)) Cyc_List_rlist; struct _RegionHandle* _temp752= r; struct Cyc_List_List*
_temp748= _temp743; struct Cyc_List_List* _temp749= _temp741; struct Cyc_List_List*
_temp750= _temp739; struct Cyc_List_List* _temp751= _temp737; struct Cyc_List_List*
_temp746[ 4u]={ _temp748, _temp749, _temp750, _temp751}; struct _tagged_arr
_temp747={( void*) _temp746,( void*) _temp746,( void*)( _temp746 + 4u)};
_temp745( _temp752, _temp747);})), rev_fmt); rev_fmt=({ struct Cyc_List_List*
_temp753=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp753->hd=( void*)(( int) _temp735); _temp753->tl= rev_fmt; _temp753;});{
struct Cyc_List_List* _temp754= _temp741; struct Cyc_List_List _temp761; struct
Cyc_List_List* _temp762; int _temp764; _LL756: if( _temp754 == 0){ goto _LL758;}
else{ _temp761=* _temp754; _LL765: _temp764=( int) _temp761.hd; goto _LL763;
_LL763: _temp762= _temp761.tl; if( _temp762 == 0){ goto _LL760;} else{ goto
_LL758;}} _LL760: if( _temp764 =='*'){ goto _LL757;} else{ goto _LL758;} _LL758:
goto _LL759; _LL757: { struct _tuple0* _temp766= Cyc_Toc_temp_var(); goto _LL767;
_LL767: rev_temps=({ struct Cyc_List_List* _temp768=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp768->hd=( void*)({
struct _tuple8* _temp769=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp769->f1= _temp766; _temp769->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp769->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp769;}); _temp768->tl= rev_temps; _temp768;});
rev_result=({ struct Cyc_List_List* _temp770=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp770->hd=( void*) Cyc_Absyn_var_exp(
_temp766, 0); _temp770->tl= rev_result; _temp770;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL755;} _LL759: goto _LL755; _LL755:;}{ struct Cyc_List_List* _temp771=
_temp739; struct Cyc_List_List _temp778; struct Cyc_List_List* _temp779; struct
Cyc_List_List _temp781; struct Cyc_List_List* _temp782; int _temp784; int
_temp786; _LL773: if( _temp771 == 0){ goto _LL775;} else{ _temp778=* _temp771;
_LL787: _temp786=( int) _temp778.hd; goto _LL780; _LL780: _temp779= _temp778.tl;
if( _temp779 == 0){ goto _LL775;} else{ _temp781=* _temp779; _LL785: _temp784=(
int) _temp781.hd; goto _LL783; _LL783: _temp782= _temp781.tl; if( _temp782 == 0){
goto _LL777;} else{ goto _LL775;}}} _LL777: if( _temp786 =='.'? _temp784 =='*':
0){ goto _LL774;} else{ goto _LL775;} _LL775: goto _LL776; _LL774: { struct
_tuple0* _temp788= Cyc_Toc_temp_var(); goto _LL789; _LL789: rev_temps=({ struct
Cyc_List_List* _temp790=( struct Cyc_List_List*) _region_malloc( r, sizeof(
struct Cyc_List_List)); _temp790->hd=( void*)({ struct _tuple8* _temp791=(
struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8)); _temp791->f1=
_temp788; _temp791->f2=( void*)(( struct Cyc_List_List*) _check_null( typs))->hd;
_temp791->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd;
_temp791;}); _temp790->tl= rev_temps; _temp790;}); rev_result=({ struct Cyc_List_List*
_temp792=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp792->hd=( void*) Cyc_Absyn_var_exp( _temp788, 0); _temp792->tl= rev_result;
_temp792;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL772;} _LL776: goto
_LL772; _LL772:;} if( _temp735 !='%'){ if( args == 0? 1: typs == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp793= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp793.last_plus_one - _temp793.curr, _temp793.curr);}));}{ struct _tuple0*
_temp794= Cyc_Toc_temp_var(); goto _LL795; _LL795: rev_temps=({ struct Cyc_List_List*
_temp796=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp796->hd=( void*)({ struct _tuple8* _temp797=( struct _tuple8*)
_region_malloc( r, sizeof( struct _tuple8)); _temp797->f1= _temp794; _temp797->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp797->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp797;});
_temp796->tl= rev_temps; _temp796;}); rev_result=({ struct Cyc_List_List*
_temp798=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp798->hd=( void*) Cyc_Absyn_var_exp( _temp794, 0); _temp798->tl= rev_result;
_temp798;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp799=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp799->hd=( void*)(( int)'.'); _temp799->tl= rev_fmt; _temp799;}); rev_fmt=({
struct Cyc_List_List* _temp800=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp800->hd=( void*)(( int)'*'); _temp800->tl=
rev_fmt; _temp800;}); rev_fmt=({ struct Cyc_List_List* _temp801=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp801->hd=( void*)(( int)'s');
_temp801->tl= rev_fmt; _temp801;});{ struct _tuple0* _temp802= Cyc_Toc_temp_var();
goto _LL803; _LL803: rev_temps=({ struct Cyc_List_List* _temp804=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp804->hd=( void*)({
struct _tuple8* _temp805=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp805->f1= _temp802; _temp805->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp805->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp805;}); _temp804->tl= rev_temps; _temp804;});{
struct Cyc_Absyn_Exp* _temp806= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp802, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( _temp802, 0), Cyc_Toc_curr_sp, 0), 0); goto _LL807; _LL807: {
struct Cyc_Absyn_Exp* _temp808= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
_temp802, 0), Cyc_Toc_curr_sp, 0); goto _LL809; _LL809: rev_result=({ struct Cyc_List_List*
_temp810=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp810->hd=( void*) _temp806; _temp810->tl= rev_result; _temp810;});
rev_result=({ struct Cyc_List_List* _temp811=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp811->hd=( void*)
_temp808; _temp811->tl= rev_result; _temp811;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}}{
struct Cyc_List_List* _temp813=({ struct Cyc_List_List* _temp812=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp812->hd=( void*) Cyc_Absyn_string_exp((
struct _tagged_arr) Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_fmt)), 0); _temp812->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_result); _temp812;}); goto _LL814;
_LL814: if( aopt != 0){ _temp813=({ struct Cyc_List_List* _temp815=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp815->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp815->tl= _temp813; _temp815;});}{
struct Cyc_Absyn_Exp* _temp816= Cyc_Absyn_primop_exp( p, _temp813, 0); goto
_LL817; _LL817: s= Cyc_Absyn_exp_stmt( _temp816, 0); for( 0; rev_temps != 0;
rev_temps=(( struct Cyc_List_List*) _check_null( rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f1,(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f2,(
struct Cyc_Absyn_Exp*)(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
rev_temps))->hd)).f3, s, 0);}}}}; _pop_region(& _temp726);} return Cyc_Absyn_stmt_exp(
s, 0);}}}} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u));} return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Missing type in primop ", sizeof( unsigned char), 24u));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp818=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp818->f1= Cyc_Toc_mt_tq; _temp818->f2=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp818;});} static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp819=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1); _temp819[ 0]=({
struct _tuple4 _temp820; _temp820.f1= 0; _temp820.f2= e; _temp820;}); _temp819;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp821= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); goto _LL822; _LL822: if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic( _temp821): Cyc_Toc_malloc_ptr(
_temp821), 0);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( rgnopt); Cyc_Toc_exp_to_c( nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, Cyc_Toc_rmalloc_exp( r, _temp821), 0);}}} else{ t= struct_typ; eo= 0;} return
Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, t, eo, s, 0), 0);} static struct
Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Absyn_Stmt* s, int e1_already_translated); static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s); static
struct Cyc_Absyn_Stmt* Cyc_Toc_init_array( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
lhs, struct Cyc_List_List* dles0, struct Cyc_Absyn_Stmt* s){ int count=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( dles0) - 1;{ struct Cyc_List_List*
_temp823=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
dles0); goto _LL824; _LL824: for( 0; _temp823 != 0; _temp823=(( struct Cyc_List_List*)
_check_null( _temp823))->tl){ struct _tuple4 _temp827; struct Cyc_Absyn_Exp*
_temp828; struct Cyc_List_List* _temp830; struct _tuple4* _temp825=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp823))->hd; _temp827=*
_temp825; _LL831: _temp830= _temp827.f1; goto _LL829; _LL829: _temp828= _temp827.f2;
goto _LL826; _LL826: { struct Cyc_Absyn_Exp* e_index; if( _temp830 == 0){
e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if((( struct Cyc_List_List*)
_check_null( _temp830))->tl != 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("multiple designators in array", sizeof( unsigned char), 30u));}{ void*
_temp832=( void*)(( struct Cyc_List_List*) _check_null( _temp830))->hd; goto
_LL833; _LL833: { void* _temp834= _temp832; struct Cyc_Absyn_Exp* _temp840;
struct _tagged_arr* _temp842; _LL836: if(*(( int*) _temp834) == Cyc_Absyn_ArrayElement){
_LL841: _temp840=(( struct Cyc_Absyn_ArrayElement_struct*) _temp834)->f1; goto
_LL837;} else{ goto _LL838;} _LL838: if(*(( int*) _temp834) == Cyc_Absyn_FieldName){
_LL843: _temp842=(( struct Cyc_Absyn_FieldName_struct*) _temp834)->f1; goto
_LL839;} else{ goto _LL835;} _LL837: Cyc_Toc_exp_to_c( nv, _temp840); e_index=
_temp840; goto _LL835; _LL839: e_index=(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("field name designators in array",
sizeof( unsigned char), 32u)); goto _LL835; _LL835:;}}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp844=( void*)
_temp828->r; struct Cyc_List_List* _temp854; struct Cyc_Absyn_Exp* _temp856;
struct Cyc_Absyn_Exp* _temp858; struct Cyc_Absyn_Vardecl* _temp860; struct Cyc_List_List*
_temp862; void* _temp864; _LL846: if(*(( int*) _temp844) == Cyc_Absyn_Array_e){
_LL855: _temp854=(( struct Cyc_Absyn_Array_e_struct*) _temp844)->f1; goto _LL847;}
else{ goto _LL848;} _LL848: if(*(( int*) _temp844) == Cyc_Absyn_Comprehension_e){
_LL861: _temp860=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp844)->f1;
goto _LL859; _LL859: _temp858=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp844)->f2; goto _LL857; _LL857: _temp856=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp844)->f3; goto _LL849;} else{ goto _LL850;} _LL850: if(*(( int*) _temp844)
== Cyc_Absyn_AnonStruct_e){ _LL865: _temp864=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp844)->f1; goto _LL863; _LL863: _temp862=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp844)->f2; goto _LL851;} else{ goto _LL852;} _LL852: goto _LL853; _LL847: s=
Cyc_Toc_init_array( nv, lval, _temp854, s); goto _LL845; _LL849: s= Cyc_Toc_init_comprehension(
nv, lval, _temp860, _temp858, _temp856, s, 0); goto _LL845; _LL851: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp864, _temp862, s); goto _LL845; _LL853: Cyc_Toc_exp_to_c( nv,
_temp828); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp828, 0), s, 0); goto _LL845; _LL845:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp866= vd->name; goto _LL867; _LL867:
if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp866, Cyc_Absyn_varb_exp( _temp866,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp890=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp890[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp891; _temp891.tag= Cyc_Absyn_Local_b; _temp891.f1= vd; _temp891;});
_temp890;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp866, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp866, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp866, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp866, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp868=( void*) e2->r; struct Cyc_List_List* _temp878; struct Cyc_Absyn_Exp*
_temp880; struct Cyc_Absyn_Exp* _temp882; struct Cyc_Absyn_Vardecl* _temp884;
struct Cyc_List_List* _temp886; void* _temp888; _LL870: if(*(( int*) _temp868)
== Cyc_Absyn_Array_e){ _LL879: _temp878=(( struct Cyc_Absyn_Array_e_struct*)
_temp868)->f1; goto _LL871;} else{ goto _LL872;} _LL872: if(*(( int*) _temp868)
== Cyc_Absyn_Comprehension_e){ _LL885: _temp884=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp868)->f1; goto _LL883; _LL883: _temp882=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp868)->f2; goto _LL881; _LL881: _temp880=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp868)->f3; goto _LL873;} else{ goto _LL874;} _LL874: if(*(( int*) _temp868)
== Cyc_Absyn_AnonStruct_e){ _LL889: _temp888=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp868)->f1; goto _LL887; _LL887: _temp886=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp868)->f2; goto _LL875;} else{ goto _LL876;} _LL876: goto _LL877; _LL871:
body= Cyc_Toc_init_array( nv2, lval, _temp878, Cyc_Absyn_skip_stmt( 0)); goto
_LL869; _LL873: body= Cyc_Toc_init_comprehension( nv2, lval, _temp884, _temp882,
_temp880, Cyc_Absyn_skip_stmt( 0), 0); goto _LL869; _LL875: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp888, _temp886, Cyc_Absyn_skip_stmt( 0)); goto _LL869; _LL877: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL869; _LL869:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp866, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp892=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); goto _LL893; _LL893: for( 0; _temp892 != 0; _temp892=((
struct Cyc_List_List*) _check_null( _temp892))->tl){ struct _tuple4 _temp896;
struct Cyc_Absyn_Exp* _temp897; struct Cyc_List_List* _temp899; struct _tuple4*
_temp894=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp892))->hd;
_temp896=* _temp894; _LL900: _temp899= _temp896.f1; goto _LL898; _LL898:
_temp897= _temp896.f2; goto _LL895; _LL895: if( _temp899 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list", sizeof(
unsigned char), 22u));} if((( struct Cyc_List_List*) _check_null( _temp899))->tl
!= 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u));}{ void* _temp901=( void*)(( struct Cyc_List_List*)
_check_null( _temp899))->hd; struct _tagged_arr* _temp907; _LL903: if(*(( int*)
_temp901) == Cyc_Absyn_FieldName){ _LL908: _temp907=(( struct Cyc_Absyn_FieldName_struct*)
_temp901)->f1; goto _LL904;} else{ goto _LL905;} _LL905: goto _LL906; _LL904: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp907, 0);{ void*
_temp909=( void*) _temp897->r; struct Cyc_List_List* _temp919; struct Cyc_Absyn_Exp*
_temp921; struct Cyc_Absyn_Exp* _temp923; struct Cyc_Absyn_Vardecl* _temp925;
struct Cyc_List_List* _temp927; void* _temp929; _LL911: if(*(( int*) _temp909)
== Cyc_Absyn_Array_e){ _LL920: _temp919=(( struct Cyc_Absyn_Array_e_struct*)
_temp909)->f1; goto _LL912;} else{ goto _LL913;} _LL913: if(*(( int*) _temp909)
== Cyc_Absyn_Comprehension_e){ _LL926: _temp925=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp909)->f1; goto _LL924; _LL924: _temp923=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp909)->f2; goto _LL922; _LL922: _temp921=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp909)->f3; goto _LL914;} else{ goto _LL915;} _LL915: if(*(( int*) _temp909)
== Cyc_Absyn_AnonStruct_e){ _LL930: _temp929=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp909)->f1; goto _LL928; _LL928: _temp927=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp909)->f2; goto _LL916;} else{ goto _LL917;} _LL917: goto _LL918; _LL912: s=
Cyc_Toc_init_array( nv, lval, _temp919, s); goto _LL910; _LL914: s= Cyc_Toc_init_comprehension(
nv, lval, _temp925, _temp923, _temp921, s, 0); goto _LL910; _LL916: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp929, _temp927, s); goto _LL910; _LL918: Cyc_Toc_exp_to_c( nv,
_temp897); if( Cyc_Toc_is_poly_field( struct_type, _temp907)){ _temp897= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp897, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp897, 0), 0), s, 0); goto _LL910; _LL910:;} goto
_LL902;} _LL906:( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u))); _LL902:;}}}
return s;} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env*
nv, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
_RegionHandle _temp931= _new_region(); struct _RegionHandle* r=& _temp931;
_push_region( r);{ struct Cyc_List_List* _temp932=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c, es); goto _LL933; _LL933: { struct
_tagged_arr* _temp934= Cyc_Toc_add_tuple_type( _temp932); goto _LL935; _LL935: {
struct _tuple0* _temp936= Cyc_Toc_temp_var(); goto _LL937; _LL937: { struct Cyc_Absyn_Exp*
_temp938= Cyc_Absyn_var_exp( _temp936, 0); goto _LL939; _LL939: { struct Cyc_Absyn_Stmt*
_temp940= Cyc_Absyn_exp_stmt( _temp938, 0); goto _LL941; _LL941: { struct Cyc_Absyn_Exp*(*
_temp942)( struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL943;
_LL943: { int is_atomic= 1; struct Cyc_List_List* _temp944=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es); goto
_LL945; _LL945:{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp944); for( 0; _temp944 != 0;( _temp944=(( struct Cyc_List_List*)
_check_null( _temp944))->tl, -- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp944))->hd; struct Cyc_Absyn_Exp* lval=
_temp942( _temp938, Cyc_Toc_fieldname( i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v): 0;{ void* _temp946=(
void*) e->r; struct Cyc_List_List* _temp954; struct Cyc_Absyn_Exp* _temp956;
struct Cyc_Absyn_Exp* _temp958; struct Cyc_Absyn_Vardecl* _temp960; _LL948: if(*((
int*) _temp946) == Cyc_Absyn_Array_e){ _LL955: _temp954=(( struct Cyc_Absyn_Array_e_struct*)
_temp946)->f1; goto _LL949;} else{ goto _LL950;} _LL950: if(*(( int*) _temp946)
== Cyc_Absyn_Comprehension_e){ _LL961: _temp960=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp946)->f1; goto _LL959; _LL959: _temp958=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp946)->f2; goto _LL957; _LL957: _temp956=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp946)->f3; goto _LL951;} else{ goto _LL952;} _LL952: goto _LL953; _LL949:
_temp940= Cyc_Toc_init_array( nv, lval, _temp954, _temp940); goto _LL947; _LL951:
_temp940= Cyc_Toc_init_comprehension( nv, lval, _temp960, _temp958, _temp956,
_temp940, 0); goto _LL947; _LL953: Cyc_Toc_exp_to_c( nv, e); _temp940= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp942( _temp938, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp940, 0); goto _LL947; _LL947:;}}}{ struct Cyc_Absyn_Exp*
_temp962= Cyc_Toc_make_struct( nv, _temp936, Cyc_Absyn_strct( _temp934),
_temp940, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp962;}}}}}}}};
_pop_region(& _temp931);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct(
struct Cyc_Toc_Env* nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* dles, struct _tuple0* tdn){ struct _tuple0*
_temp963= Cyc_Toc_temp_var(); goto _LL964; _LL964: { struct Cyc_Absyn_Exp*
_temp965= Cyc_Absyn_var_exp( _temp963, 0); goto _LL966; _LL966: { struct Cyc_Absyn_Stmt*
_temp967= Cyc_Absyn_exp_stmt( _temp965, 0); goto _LL968; _LL968: { struct Cyc_Absyn_Exp*(*
_temp969)( struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL970;
_LL970: { int is_atomic= 1; struct _RegionHandle _temp971= _new_region(); struct
_RegionHandle* r=& _temp971; _push_region( r);{ struct Cyc_List_List* _temp972=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)(
r, dles); goto _LL973; _LL973: for( 0; _temp972 != 0; _temp972=(( struct Cyc_List_List*)
_check_null( _temp972))->tl){ struct _tuple4 _temp976; struct Cyc_Absyn_Exp*
_temp977; struct Cyc_List_List* _temp979; struct _tuple4* _temp974=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp972))->hd; _temp976=*
_temp974; _LL980: _temp979= _temp976.f1; goto _LL978; _LL978: _temp977= _temp976.f2;
goto _LL975; _LL975: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp977->topt))->v): 0; if( _temp979 == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u));} if((( struct Cyc_List_List*) _check_null(
_temp979))->tl != 0){ struct _tuple0* _temp981= Cyc_Toc_temp_var(); goto _LL982;
_LL982: { struct Cyc_Absyn_Exp* _temp983= Cyc_Absyn_var_exp( _temp981, 0); goto
_LL984; _LL984: for( 0; _temp979 != 0; _temp979=(( struct Cyc_List_List*)
_check_null( _temp979))->tl){ void* _temp985=( void*)(( struct Cyc_List_List*)
_check_null( _temp979))->hd; struct _tagged_arr* _temp991; _LL987: if(*(( int*)
_temp985) == Cyc_Absyn_FieldName){ _LL992: _temp991=(( struct Cyc_Absyn_FieldName_struct*)
_temp985)->f1; goto _LL988;} else{ goto _LL989;} _LL989: goto _LL990; _LL988:
if( Cyc_Toc_is_poly_field( struct_type, _temp991)){ _temp983= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp983, 0);} _temp967= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp969( _temp965, _temp991, 0), _temp983, 0), 0),
_temp967, 0); goto _LL986; _LL990:( int) _throw((( void*(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)( _tag_arr("array designator in struct", sizeof(
unsigned char), 27u))); _LL986:;} Cyc_Toc_exp_to_c( nv, _temp977); _temp967= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp983, _temp977, 0), 0), _temp967,
0);}} else{ void* _temp993=( void*)(( struct Cyc_List_List*) _check_null(
_temp979))->hd; struct _tagged_arr* _temp999; _LL995: if(*(( int*) _temp993) ==
Cyc_Absyn_FieldName){ _LL1000: _temp999=(( struct Cyc_Absyn_FieldName_struct*)
_temp993)->f1; goto _LL996;} else{ goto _LL997;} _LL997: goto _LL998; _LL996: {
struct Cyc_Absyn_Exp* lval= _temp969( _temp965, _temp999, 0);{ void* _temp1001=(
void*) _temp977->r; struct Cyc_List_List* _temp1011; struct Cyc_Absyn_Exp*
_temp1013; struct Cyc_Absyn_Exp* _temp1015; struct Cyc_Absyn_Vardecl* _temp1017;
struct Cyc_List_List* _temp1019; void* _temp1021; _LL1003: if(*(( int*)
_temp1001) == Cyc_Absyn_Array_e){ _LL1012: _temp1011=(( struct Cyc_Absyn_Array_e_struct*)
_temp1001)->f1; goto _LL1004;} else{ goto _LL1005;} _LL1005: if(*(( int*)
_temp1001) == Cyc_Absyn_Comprehension_e){ _LL1018: _temp1017=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1001)->f1; goto _LL1016; _LL1016: _temp1015=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1001)->f2; goto _LL1014; _LL1014: _temp1013=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1001)->f3; goto _LL1006;} else{ goto _LL1007;} _LL1007: if(*(( int*)
_temp1001) == Cyc_Absyn_AnonStruct_e){ _LL1022: _temp1021=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1001)->f1; goto _LL1020; _LL1020: _temp1019=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1001)->f2; goto _LL1008;} else{ goto _LL1009;} _LL1009: goto _LL1010;
_LL1004: _temp967= Cyc_Toc_init_array( nv, lval, _temp1011, _temp967); goto
_LL1002; _LL1006: _temp967= Cyc_Toc_init_comprehension( nv, lval, _temp1017,
_temp1015, _temp1013, _temp967, 0); goto _LL1002; _LL1008: _temp967= Cyc_Toc_init_anon_struct(
nv, lval, _temp1021, _temp1019, _temp967); goto _LL1002; _LL1010: Cyc_Toc_exp_to_c(
nv, _temp977); if( Cyc_Toc_is_poly_field( struct_type, _temp999)){ _temp977= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp977, 0);} _temp967= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp977, 0), 0), _temp967, 0); goto _LL1002;
_LL1002:;} goto _LL994;} _LL998:( int) _throw((( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("array designator in struct", sizeof( unsigned char),
27u))); _LL994:;}}}{ struct Cyc_Absyn_Exp* _temp1023= Cyc_Toc_make_struct( nv,
_temp963, Cyc_Absyn_strctq( tdn), _temp967, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp1023;}; _pop_region(& _temp971);}}}}} struct
_tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static struct
Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct _tuple9*
pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static struct
Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int ignore){
return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp1024=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp1024[ 0]=({
struct Cyc_Absyn_Increment_e_struct _temp1025; _temp1025.tag= Cyc_Absyn_Increment_e;
_temp1025.f1= e1; _temp1025.f2=( void*) incr; _temp1025;}); _temp1024;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp1026=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp1036; struct Cyc_Absyn_Exp*
_temp1038; void* _temp1040; struct _tagged_arr* _temp1042; struct Cyc_Absyn_Exp*
_temp1044; _LL1028: if(*(( int*) _temp1026) == Cyc_Absyn_StmtExp_e){ _LL1037:
_temp1036=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1026)->f1; goto _LL1029;}
else{ goto _LL1030;} _LL1030: if(*(( int*) _temp1026) == Cyc_Absyn_Cast_e){
_LL1041: _temp1040=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1026)->f1;
goto _LL1039; _LL1039: _temp1038=(( struct Cyc_Absyn_Cast_e_struct*) _temp1026)->f2;
goto _LL1031;} else{ goto _LL1032;} _LL1032: if(*(( int*) _temp1026) == Cyc_Absyn_StructMember_e){
_LL1045: _temp1044=(( struct Cyc_Absyn_StructMember_e_struct*) _temp1026)->f1;
goto _LL1043; _LL1043: _temp1042=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1026)->f2; goto _LL1033;} else{ goto _LL1034;} _LL1034: goto _LL1035;
_LL1029:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1036, fs, f, f_env); goto _LL1027; _LL1031:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1038, fs, f, f_env); goto
_LL1027; _LL1033:( void*)( e1->r=( void*)(( void*) _temp1044->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1046=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1046->hd=( void*) _temp1042; _temp1046->tl= fs;
_temp1046;}), f, f_env); goto _LL1027; _LL1035: if( ! Cyc_Absyn_is_lvalue( e1)){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp1047= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1047.last_plus_one - _temp1047.curr, _temp1047.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1027;}
_LL1027:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1048=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1058; struct Cyc_Absyn_Stmt* _temp1060; struct Cyc_Absyn_Decl* _temp1062;
struct Cyc_Absyn_Stmt* _temp1064; struct Cyc_Absyn_Stmt* _temp1066; _LL1050: if((
unsigned int) _temp1048 > 1u?*(( int*) _temp1048) == Cyc_Absyn_Exp_s: 0){
_LL1059: _temp1058=(( struct Cyc_Absyn_Exp_s_struct*) _temp1048)->f1; goto
_LL1051;} else{ goto _LL1052;} _LL1052: if(( unsigned int) _temp1048 > 1u?*((
int*) _temp1048) == Cyc_Absyn_Decl_s: 0){ _LL1063: _temp1062=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1048)->f1; goto _LL1061; _LL1061: _temp1060=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1048)->f2; goto _LL1053;} else{ goto _LL1054;} _LL1054: if(( unsigned int)
_temp1048 > 1u?*(( int*) _temp1048) == Cyc_Absyn_Seq_s: 0){ _LL1067: _temp1066=((
struct Cyc_Absyn_Seq_s_struct*) _temp1048)->f1; goto _LL1065; _LL1065: _temp1064=((
struct Cyc_Absyn_Seq_s_struct*) _temp1048)->f2; goto _LL1055;} else{ goto
_LL1056;} _LL1056: goto _LL1057; _LL1051:(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1058, fs, f, f_env); goto
_LL1049; _LL1053:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1060, fs, f, f_env); goto _LL1049; _LL1055:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1064, fs, f, f_env);
goto _LL1049; _LL1057:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1068= Cyc_Absynpp_stmt2string( s);
xprintf("lvalue_assign_stmt: %.*s", _temp1068.last_plus_one - _temp1068.curr,
_temp1068.curr);})); goto _LL1049; _LL1049:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c(
struct _RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x){ struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){
return 0;} result=({ struct Cyc_List_List* _temp1069=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1069->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1069->tl= 0;
_temp1069;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp1070=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1070->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1070->tl= 0;
_temp1070;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x))
Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1071=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1071->f1= 0; _temp1071->f2= e;
_temp1071;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){
void* _temp1072= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp1078;
_LL1074: if(( unsigned int) _temp1072 > 4u?*(( int*) _temp1072) == Cyc_Absyn_PointerType:
0){ _LL1079: _temp1078=(( struct Cyc_Absyn_PointerType_struct*) _temp1072)->f1;
goto _LL1075;} else{ goto _LL1076;} _LL1076: goto _LL1077; _LL1075: return
_temp1078; _LL1077:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("get_ptr_typ:  not a pointer!", sizeof( unsigned char), 29u))); _LL1073:;}
struct _tuple10{ void* f1; void* f2; } ; struct _tuple11{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp1080=( void*) e->r; goto _LL1081;
_LL1081: if( e->topt == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1082= Cyc_Absynpp_exp2string( e);
xprintf("exp_to_c: no type for %.*s", _temp1082.last_plus_one - _temp1082.curr,
_temp1082.curr);})); return;}{ void* old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; void* _temp1083= _temp1080; void* _temp1163; void*
_temp1165; void* _temp1167; struct _tuple0* _temp1169; struct _tuple0* _temp1171;
struct Cyc_List_List* _temp1173; void* _temp1175; void* _temp1177; struct Cyc_Absyn_Exp*
_temp1179; struct Cyc_Absyn_Exp* _temp1181; struct Cyc_Core_Opt* _temp1183;
struct Cyc_Absyn_Exp* _temp1185; struct Cyc_Absyn_Exp* _temp1187; struct Cyc_Absyn_Exp*
_temp1189; struct Cyc_Absyn_Exp* _temp1191; struct Cyc_Absyn_Exp* _temp1193;
struct Cyc_Absyn_Exp* _temp1195; struct Cyc_List_List* _temp1197; struct Cyc_Absyn_Exp*
_temp1199; struct Cyc_Absyn_VarargCallInfo* _temp1201; struct Cyc_List_List*
_temp1203; struct Cyc_Absyn_Exp* _temp1205; struct Cyc_Absyn_VarargCallInfo*
_temp1207; struct Cyc_Absyn_VarargCallInfo _temp1209; struct Cyc_Absyn_VarargInfo*
_temp1210; struct Cyc_List_List* _temp1212; int _temp1214; struct Cyc_List_List*
_temp1216; struct Cyc_Absyn_Exp* _temp1218; struct Cyc_Absyn_Exp* _temp1220;
struct Cyc_Absyn_Exp* _temp1222; struct Cyc_List_List* _temp1224; struct Cyc_Absyn_Exp*
_temp1226; struct Cyc_Absyn_Exp* _temp1228; void* _temp1230; void** _temp1232;
struct Cyc_Absyn_Exp* _temp1233; struct Cyc_Absyn_Exp* _temp1235; struct Cyc_Absyn_Exp*
_temp1237; struct Cyc_Absyn_Exp* _temp1239; void* _temp1241; struct _tagged_arr*
_temp1243; void* _temp1245; struct Cyc_Absyn_Exp* _temp1247; struct _tagged_arr*
_temp1249; struct Cyc_Absyn_Exp* _temp1251; struct _tagged_arr* _temp1253;
struct Cyc_Absyn_Exp* _temp1255; struct Cyc_Absyn_Exp* _temp1257; struct Cyc_Absyn_Exp*
_temp1259; struct Cyc_List_List* _temp1261; struct Cyc_List_List* _temp1263;
struct Cyc_Absyn_Exp* _temp1265; struct Cyc_Absyn_Exp* _temp1267; struct Cyc_Absyn_Vardecl*
_temp1269; struct Cyc_Absyn_Structdecl* _temp1271; struct Cyc_List_List*
_temp1273; struct Cyc_Core_Opt* _temp1275; struct _tuple0* _temp1277; struct Cyc_List_List*
_temp1279; void* _temp1281; struct Cyc_Absyn_Tunionfield* _temp1283; struct Cyc_Absyn_Tuniondecl*
_temp1285; struct Cyc_List_List* _temp1287; struct Cyc_Core_Opt* _temp1289;
struct Cyc_Core_Opt* _temp1291; struct Cyc_Absyn_Tunionfield* _temp1293; struct
Cyc_Absyn_Tuniondecl* _temp1295; struct Cyc_List_List* _temp1297; struct Cyc_Core_Opt*
_temp1299; struct Cyc_Core_Opt* _temp1301; struct Cyc_Absyn_Enumfield* _temp1303;
struct Cyc_Absyn_Enumdecl* _temp1305; struct _tuple0* _temp1307; void* _temp1309;
struct Cyc_Absyn_Exp* _temp1311; struct Cyc_Absyn_Stmt* _temp1313; struct Cyc_List_List*
_temp1315; struct Cyc_Core_Opt* _temp1317; struct Cyc_List_List* _temp1319;
struct _tuple1* _temp1321; struct Cyc_Absyn_Fndecl* _temp1323; struct Cyc_Absyn_Exp*
_temp1325; _LL1085: if(*(( int*) _temp1083) == Cyc_Absyn_Const_e){ _LL1164:
_temp1163=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1083)->f1; if(
_temp1163 ==( void*) Cyc_Absyn_Null_c){ goto _LL1086;} else{ goto _LL1087;}}
else{ goto _LL1087;} _LL1087: if(*(( int*) _temp1083) == Cyc_Absyn_Const_e){
_LL1166: _temp1165=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1083)->f1;
goto _LL1088;} else{ goto _LL1089;} _LL1089: if(*(( int*) _temp1083) == Cyc_Absyn_Var_e){
_LL1170: _temp1169=(( struct Cyc_Absyn_Var_e_struct*) _temp1083)->f1; goto
_LL1168; _LL1168: _temp1167=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1083)->f2;
goto _LL1090;} else{ goto _LL1091;} _LL1091: if(*(( int*) _temp1083) == Cyc_Absyn_UnknownId_e){
_LL1172: _temp1171=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1083)->f1; goto
_LL1092;} else{ goto _LL1093;} _LL1093: if(*(( int*) _temp1083) == Cyc_Absyn_Primop_e){
_LL1176: _temp1175=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1083)->f1;
goto _LL1174; _LL1174: _temp1173=(( struct Cyc_Absyn_Primop_e_struct*) _temp1083)->f2;
goto _LL1094;} else{ goto _LL1095;} _LL1095: if(*(( int*) _temp1083) == Cyc_Absyn_Increment_e){
_LL1180: _temp1179=(( struct Cyc_Absyn_Increment_e_struct*) _temp1083)->f1; goto
_LL1178; _LL1178: _temp1177=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1083)->f2; goto _LL1096;} else{ goto _LL1097;} _LL1097: if(*(( int*)
_temp1083) == Cyc_Absyn_AssignOp_e){ _LL1186: _temp1185=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1083)->f1; goto _LL1184; _LL1184: _temp1183=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1083)->f2; goto _LL1182; _LL1182: _temp1181=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1083)->f3; goto _LL1098;} else{ goto _LL1099;} _LL1099: if(*(( int*)
_temp1083) == Cyc_Absyn_Conditional_e){ _LL1192: _temp1191=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1083)->f1; goto _LL1190; _LL1190: _temp1189=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1083)->f2; goto _LL1188; _LL1188: _temp1187=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1083)->f3; goto _LL1100;} else{ goto _LL1101;} _LL1101: if(*(( int*)
_temp1083) == Cyc_Absyn_SeqExp_e){ _LL1196: _temp1195=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1083)->f1; goto _LL1194; _LL1194: _temp1193=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1083)->f2; goto _LL1102;} else{ goto _LL1103;} _LL1103: if(*(( int*)
_temp1083) == Cyc_Absyn_UnknownCall_e){ _LL1200: _temp1199=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1083)->f1; goto _LL1198; _LL1198: _temp1197=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1083)->f2; goto _LL1104;} else{ goto _LL1105;} _LL1105: if(*(( int*)
_temp1083) == Cyc_Absyn_FnCall_e){ _LL1206: _temp1205=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1083)->f1; goto _LL1204; _LL1204: _temp1203=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1083)->f2; goto _LL1202; _LL1202: _temp1201=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1083)->f3; if( _temp1201 == 0){ goto _LL1106;} else{ goto _LL1107;}} else{
goto _LL1107;} _LL1107: if(*(( int*) _temp1083) == Cyc_Absyn_FnCall_e){ _LL1219:
_temp1218=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1083)->f1; goto _LL1217;
_LL1217: _temp1216=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1083)->f2; goto
_LL1208; _LL1208: _temp1207=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1083)->f3;
if( _temp1207 == 0){ goto _LL1109;} else{ _temp1209=* _temp1207; _LL1215:
_temp1214= _temp1209.num_varargs; goto _LL1213; _LL1213: _temp1212= _temp1209.injectors;
goto _LL1211; _LL1211: _temp1210= _temp1209.vai; goto _LL1108;}} else{ goto
_LL1109;} _LL1109: if(*(( int*) _temp1083) == Cyc_Absyn_Throw_e){ _LL1221:
_temp1220=(( struct Cyc_Absyn_Throw_e_struct*) _temp1083)->f1; goto _LL1110;}
else{ goto _LL1111;} _LL1111: if(*(( int*) _temp1083) == Cyc_Absyn_NoInstantiate_e){
_LL1223: _temp1222=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1083)->f1;
goto _LL1112;} else{ goto _LL1113;} _LL1113: if(*(( int*) _temp1083) == Cyc_Absyn_Instantiate_e){
_LL1227: _temp1226=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1083)->f1;
goto _LL1225; _LL1225: _temp1224=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1083)->f2; goto _LL1114;} else{ goto _LL1115;} _LL1115: if(*(( int*)
_temp1083) == Cyc_Absyn_Cast_e){ _LL1231: _temp1230=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1083)->f1; _temp1232=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1083)->f1); goto _LL1229; _LL1229: _temp1228=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1083)->f2; goto _LL1116;} else{ goto _LL1117;} _LL1117: if(*(( int*)
_temp1083) == Cyc_Absyn_Address_e){ _LL1234: _temp1233=(( struct Cyc_Absyn_Address_e_struct*)
_temp1083)->f1; goto _LL1118;} else{ goto _LL1119;} _LL1119: if(*(( int*)
_temp1083) == Cyc_Absyn_New_e){ _LL1238: _temp1237=(( struct Cyc_Absyn_New_e_struct*)
_temp1083)->f1; goto _LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_New_e_struct*)
_temp1083)->f2; goto _LL1120;} else{ goto _LL1121;} _LL1121: if(*(( int*)
_temp1083) == Cyc_Absyn_Sizeofexp_e){ _LL1240: _temp1239=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1083)->f1; goto _LL1122;} else{ goto _LL1123;} _LL1123: if(*(( int*)
_temp1083) == Cyc_Absyn_Sizeoftyp_e){ _LL1242: _temp1241=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1083)->f1; goto _LL1124;} else{ goto _LL1125;} _LL1125: if(*(( int*)
_temp1083) == Cyc_Absyn_Offsetof_e){ _LL1246: _temp1245=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1083)->f1; goto _LL1244; _LL1244: _temp1243=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1083)->f2; goto _LL1126;} else{ goto _LL1127;} _LL1127: if(*(( int*)
_temp1083) == Cyc_Absyn_Deref_e){ _LL1248: _temp1247=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1083)->f1; goto _LL1128;} else{ goto _LL1129;} _LL1129: if(*(( int*)
_temp1083) == Cyc_Absyn_StructMember_e){ _LL1252: _temp1251=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1083)->f1; goto _LL1250; _LL1250: _temp1249=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1083)->f2; goto _LL1130;} else{ goto _LL1131;} _LL1131: if(*(( int*)
_temp1083) == Cyc_Absyn_StructArrow_e){ _LL1256: _temp1255=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1083)->f1; goto _LL1254; _LL1254: _temp1253=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1083)->f2; goto _LL1132;} else{ goto _LL1133;} _LL1133: if(*(( int*)
_temp1083) == Cyc_Absyn_Subscript_e){ _LL1260: _temp1259=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1083)->f1; goto _LL1258; _LL1258: _temp1257=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1083)->f2; goto _LL1134;} else{ goto _LL1135;} _LL1135: if(*(( int*)
_temp1083) == Cyc_Absyn_Tuple_e){ _LL1262: _temp1261=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1083)->f1; goto _LL1136;} else{ goto _LL1137;} _LL1137: if(*(( int*)
_temp1083) == Cyc_Absyn_Array_e){ _LL1264: _temp1263=(( struct Cyc_Absyn_Array_e_struct*)
_temp1083)->f1; goto _LL1138;} else{ goto _LL1139;} _LL1139: if(*(( int*)
_temp1083) == Cyc_Absyn_Comprehension_e){ _LL1270: _temp1269=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1083)->f1; goto _LL1268; _LL1268: _temp1267=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1083)->f2; goto _LL1266; _LL1266: _temp1265=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1083)->f3; goto _LL1140;} else{ goto _LL1141;} _LL1141: if(*(( int*)
_temp1083) == Cyc_Absyn_Struct_e){ _LL1278: _temp1277=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1083)->f1; goto _LL1276; _LL1276: _temp1275=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1083)->f2; goto _LL1274; _LL1274: _temp1273=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1083)->f3; goto _LL1272; _LL1272: _temp1271=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1083)->f4; goto _LL1142;} else{ goto _LL1143;} _LL1143: if(*(( int*)
_temp1083) == Cyc_Absyn_AnonStruct_e){ _LL1282: _temp1281=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1083)->f1; goto _LL1280; _LL1280: _temp1279=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1083)->f2; goto _LL1144;} else{ goto _LL1145;} _LL1145: if(*(( int*)
_temp1083) == Cyc_Absyn_Tunion_e){ _LL1292: _temp1291=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1083)->f1; goto _LL1290; _LL1290: _temp1289=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1083)->f2; goto _LL1288; _LL1288: _temp1287=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1083)->f3; if( _temp1287 == 0){ goto _LL1286;} else{ goto _LL1147;} _LL1286:
_temp1285=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1083)->f4; goto _LL1284;
_LL1284: _temp1283=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1083)->f5; goto
_LL1146;} else{ goto _LL1147;} _LL1147: if(*(( int*) _temp1083) == Cyc_Absyn_Tunion_e){
_LL1302: _temp1301=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1083)->f1; goto
_LL1300; _LL1300: _temp1299=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1083)->f2;
goto _LL1298; _LL1298: _temp1297=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1083)->f3;
goto _LL1296; _LL1296: _temp1295=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1083)->f4;
goto _LL1294; _LL1294: _temp1293=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1083)->f5;
goto _LL1148;} else{ goto _LL1149;} _LL1149: if(*(( int*) _temp1083) == Cyc_Absyn_Enum_e){
_LL1308: _temp1307=(( struct Cyc_Absyn_Enum_e_struct*) _temp1083)->f1; goto
_LL1306; _LL1306: _temp1305=(( struct Cyc_Absyn_Enum_e_struct*) _temp1083)->f2;
goto _LL1304; _LL1304: _temp1303=(( struct Cyc_Absyn_Enum_e_struct*) _temp1083)->f3;
goto _LL1150;} else{ goto _LL1151;} _LL1151: if(*(( int*) _temp1083) == Cyc_Absyn_Malloc_e){
_LL1312: _temp1311=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1083)->f1; goto
_LL1310; _LL1310: _temp1309=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1083)->f2; goto _LL1152;} else{ goto _LL1153;} _LL1153: if(*(( int*)
_temp1083) == Cyc_Absyn_StmtExp_e){ _LL1314: _temp1313=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1083)->f1; goto _LL1154;} else{ goto _LL1155;} _LL1155: if(*(( int*)
_temp1083) == Cyc_Absyn_UnresolvedMem_e){ _LL1318: _temp1317=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1083)->f1; goto _LL1316; _LL1316: _temp1315=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1083)->f2; goto _LL1156;} else{ goto _LL1157;} _LL1157: if(*(( int*)
_temp1083) == Cyc_Absyn_CompoundLit_e){ _LL1322: _temp1321=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1083)->f1; goto _LL1320; _LL1320: _temp1319=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1083)->f2; goto _LL1158;} else{ goto _LL1159;} _LL1159: if(*(( int*)
_temp1083) == Cyc_Absyn_Codegen_e){ _LL1324: _temp1323=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1083)->f1; goto _LL1160;} else{ goto _LL1161;} _LL1161: if(*(( int*)
_temp1083) == Cyc_Absyn_Fill_e){ _LL1326: _temp1325=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1083)->f1; goto _LL1162;} else{ goto _LL1084;} _LL1086:{ void* _temp1327=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1333; struct Cyc_Absyn_Conref*
_temp1335; struct Cyc_Absyn_Tqual _temp1337; struct Cyc_Absyn_Conref* _temp1339;
void* _temp1341; void* _temp1343; _LL1329: if(( unsigned int) _temp1327 > 4u?*((
int*) _temp1327) == Cyc_Absyn_PointerType: 0){ _LL1334: _temp1333=(( struct Cyc_Absyn_PointerType_struct*)
_temp1327)->f1; _LL1344: _temp1343=( void*) _temp1333.elt_typ; goto _LL1342;
_LL1342: _temp1341=( void*) _temp1333.rgn_typ; goto _LL1340; _LL1340: _temp1339=
_temp1333.nullable; goto _LL1338; _LL1338: _temp1337= _temp1333.tq; goto _LL1336;
_LL1336: _temp1335= _temp1333.bounds; goto _LL1330;} else{ goto _LL1331;}
_LL1331: goto _LL1332; _LL1330:{ void* _temp1345=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1335); struct Cyc_Absyn_Exp*
_temp1351; _LL1347: if( _temp1345 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1348;}
else{ goto _LL1349;} _LL1349: if(( unsigned int) _temp1345 > 1u?*(( int*)
_temp1345) == Cyc_Absyn_Upper_b: 0){ _LL1352: _temp1351=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1345)->f1; goto _LL1350;} else{ goto _LL1346;} _LL1348: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, Cyc_Absyn_uint_exp(
0, 0), Cyc_Absyn_uint_exp( 0, 0)))->r));} else{( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1353)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1356= Cyc_Absyn_uint_exp( 0, 0); struct Cyc_Absyn_Exp*
_temp1357= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1343), 0); struct Cyc_Absyn_Exp*
_temp1358= Cyc_Absyn_uint_exp( 0, 0); struct Cyc_Absyn_Exp* _temp1354[ 3u]={
_temp1356, _temp1357, _temp1358}; struct _tagged_arr _temp1355={( void*)
_temp1354,( void*) _temp1354,( void*)( _temp1354 + 3u)}; _temp1353( _temp1355);}),
0))->r));} goto _LL1346; _LL1350:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1346; _LL1346:;} goto _LL1328; _LL1332:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp1359= Cyc_Absynpp_typ2string( old_typ); xprintf("exp_to_c: null given type %.*s",
_temp1359.last_plus_one - _temp1359.curr, _temp1359.curr);})); goto _LL1328;
_LL1328:;} goto _LL1084; _LL1088: goto _LL1084; _LL1090:{ struct _handler_cons
_temp1360; _push_handler(& _temp1360);{ int _temp1362= 0; if( setjmp( _temp1360.handler)){
_temp1362= 1;} if( ! _temp1362){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1169))->r));; _pop_handler();} else{ void* _temp1361=( void*) _exn_thrown;
void* _temp1364= _temp1361; _LL1366: if( _temp1364 == Cyc_Dict_Absent){ goto
_LL1367;} else{ goto _LL1368;} _LL1368: goto _LL1369; _LL1367:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp1370= Cyc_Absynpp_qvar2string( _temp1169); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1370.last_plus_one - _temp1370.curr, _temp1370.curr);})); return; _LL1369:(
void) _throw( _temp1364); _LL1365:;}}} goto _LL1084; _LL1092:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char),
10u)); goto _LL1084; _LL1094: { struct Cyc_List_List* _temp1371=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1173); goto _LL1372; _LL1372: { struct Cyc_List_List* _temp1373=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Toc_get_c_typ, _temp1173); goto _LL1374; _LL1374:(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1173);{
void* _temp1375= _temp1175; _LL1377: if( _temp1375 ==( void*) Cyc_Absyn_Size){
goto _LL1378;} else{ goto _LL1379;} _LL1379: if( _temp1375 ==( void*) Cyc_Absyn_Printf){
goto _LL1380;} else{ goto _LL1381;} _LL1381: if( _temp1375 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1382;} else{ goto _LL1383;} _LL1383: if( _temp1375 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1384;} else{ goto _LL1385;} _LL1385: if( _temp1375 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1386;} else{ goto _LL1387;} _LL1387: if( _temp1375 ==( void*) Cyc_Absyn_Plus){
goto _LL1388;} else{ goto _LL1389;} _LL1389: if( _temp1375 ==( void*) Cyc_Absyn_Minus){
goto _LL1390;} else{ goto _LL1391;} _LL1391: if( _temp1375 ==( void*) Cyc_Absyn_Eq){
goto _LL1392;} else{ goto _LL1393;} _LL1393: if( _temp1375 ==( void*) Cyc_Absyn_Neq){
goto _LL1394;} else{ goto _LL1395;} _LL1395: if( _temp1375 ==( void*) Cyc_Absyn_Gt){
goto _LL1396;} else{ goto _LL1397;} _LL1397: if( _temp1375 ==( void*) Cyc_Absyn_Gte){
goto _LL1398;} else{ goto _LL1399;} _LL1399: if( _temp1375 ==( void*) Cyc_Absyn_Lt){
goto _LL1400;} else{ goto _LL1401;} _LL1401: if( _temp1375 ==( void*) Cyc_Absyn_Lte){
goto _LL1402;} else{ goto _LL1403;} _LL1403: goto _LL1404; _LL1378: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1173))->hd;{
void* _temp1405=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1413; struct Cyc_Absyn_Tqual _temp1415; void*
_temp1417; struct Cyc_Absyn_PtrInfo _temp1419; struct Cyc_Absyn_Conref*
_temp1421; struct Cyc_Absyn_Tqual _temp1423; struct Cyc_Absyn_Conref* _temp1425;
void* _temp1427; void* _temp1429; _LL1407: if(( unsigned int) _temp1405 > 4u?*((
int*) _temp1405) == Cyc_Absyn_ArrayType: 0){ _LL1418: _temp1417=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1405)->f1; goto _LL1416; _LL1416:
_temp1415=(( struct Cyc_Absyn_ArrayType_struct*) _temp1405)->f2; goto _LL1414;
_LL1414: _temp1413=(( struct Cyc_Absyn_ArrayType_struct*) _temp1405)->f3; goto
_LL1408;} else{ goto _LL1409;} _LL1409: if(( unsigned int) _temp1405 > 4u?*((
int*) _temp1405) == Cyc_Absyn_PointerType: 0){ _LL1420: _temp1419=(( struct Cyc_Absyn_PointerType_struct*)
_temp1405)->f1; _LL1430: _temp1429=( void*) _temp1419.elt_typ; goto _LL1428;
_LL1428: _temp1427=( void*) _temp1419.rgn_typ; goto _LL1426; _LL1426: _temp1425=
_temp1419.nullable; goto _LL1424; _LL1424: _temp1423= _temp1419.tq; goto _LL1422;
_LL1422: _temp1421= _temp1419.bounds; goto _LL1410;} else{ goto _LL1411;}
_LL1411: goto _LL1412; _LL1408:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1413))->r)); goto _LL1406; _LL1410:{ void* _temp1431=(( void*(*)(
void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1421); struct Cyc_Absyn_Exp* _temp1437; _LL1433: if( _temp1431 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1434;} else{ goto _LL1435;} _LL1435: if((
unsigned int) _temp1431 > 1u?*(( int*) _temp1431) == Cyc_Absyn_Upper_b: 0){
_LL1438: _temp1437=(( struct Cyc_Absyn_Upper_b_struct*) _temp1431)->f1; goto
_LL1436;} else{ goto _LL1432;} _LL1434:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp1439)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1442=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1173))->hd; struct Cyc_Absyn_Exp* _temp1443= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( _temp1429), 0); struct Cyc_Absyn_Exp* _temp1440[ 2u]={
_temp1442, _temp1443}; struct _tagged_arr _temp1441={( void*) _temp1440,( void*)
_temp1440,( void*)( _temp1440 + 2u)}; _temp1439( _temp1441);}), 0))->r)); goto
_LL1432; _LL1436:( void*)( e->r=( void*)(( void*) _temp1437->r)); goto _LL1432;
_LL1432:;} goto _LL1406; _LL1412:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1444= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_List_List*) _check_null( _temp1373))->hd); struct
_tagged_arr _temp1445= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_temp1444.last_plus_one - _temp1444.curr, _temp1444.curr, _temp1445.last_plus_one
- _temp1445.curr, _temp1445.curr);})); return; _LL1406:;} goto _LL1376;} _LL1380:(
void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1175, 0, _temp1173,
_temp1373))->r)); goto _LL1376; _LL1382:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1175, 0, _temp1173, _temp1373))->r)); goto _LL1376; _LL1384: if( _temp1173
== 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("fprintf without arguments",
sizeof( unsigned char), 26u));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1175,({ struct Cyc_Core_Opt* _temp1446=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1446->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1173))->hd); _temp1446;}),(( struct Cyc_List_List*)
_check_null( _temp1173))->tl,(( struct Cyc_List_List*) _check_null( _temp1373))->tl))->r));
goto _LL1376; _LL1386: if( _temp1173 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("sscanf without arguments", sizeof( unsigned char), 25u));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1173))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1173))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1173))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1376; _LL1388:{ void* _temp1447= Cyc_Tcutil_compress((
void*)(( struct Cyc_List_List*) _check_null( _temp1371))->hd); struct Cyc_Absyn_PtrInfo
_temp1453; struct Cyc_Absyn_Conref* _temp1455; struct Cyc_Absyn_Tqual _temp1457;
struct Cyc_Absyn_Conref* _temp1459; void* _temp1461; void* _temp1463; _LL1449:
if(( unsigned int) _temp1447 > 4u?*(( int*) _temp1447) == Cyc_Absyn_PointerType:
0){ _LL1454: _temp1453=(( struct Cyc_Absyn_PointerType_struct*) _temp1447)->f1;
_LL1464: _temp1463=( void*) _temp1453.elt_typ; goto _LL1462; _LL1462: _temp1461=(
void*) _temp1453.rgn_typ; goto _LL1460; _LL1460: _temp1459= _temp1453.nullable;
goto _LL1458; _LL1458: _temp1457= _temp1453.tq; goto _LL1456; _LL1456: _temp1455=
_temp1453.bounds; goto _LL1450;} else{ goto _LL1451;} _LL1451: goto _LL1452;
_LL1450:{ void* _temp1465=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1455); _LL1467: if( _temp1465 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1468;} else{ goto _LL1469;} _LL1469: goto _LL1470; _LL1468: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1173))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1173))->tl))->hd;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_List_List*(* _temp1471)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1474= e1; struct
Cyc_Absyn_Exp* _temp1475= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1463),
0); struct Cyc_Absyn_Exp* _temp1476= e2; struct Cyc_Absyn_Exp* _temp1472[ 3u]={
_temp1474, _temp1475, _temp1476}; struct _tagged_arr _temp1473={( void*)
_temp1472,( void*) _temp1472,( void*)( _temp1472 + 3u)}; _temp1471( _temp1473);}),
0))->r)); goto _LL1466;} _LL1470: goto _LL1466; _LL1466:;} goto _LL1448; _LL1452:
goto _LL1448; _LL1448:;} goto _LL1376; _LL1390:{ void* _temp1477= Cyc_Tcutil_compress((
void*)(( struct Cyc_List_List*) _check_null( _temp1371))->hd); struct Cyc_Absyn_PtrInfo
_temp1483; struct Cyc_Absyn_Conref* _temp1485; struct Cyc_Absyn_Tqual _temp1487;
struct Cyc_Absyn_Conref* _temp1489; void* _temp1491; void* _temp1493; _LL1479:
if(( unsigned int) _temp1477 > 4u?*(( int*) _temp1477) == Cyc_Absyn_PointerType:
0){ _LL1484: _temp1483=(( struct Cyc_Absyn_PointerType_struct*) _temp1477)->f1;
_LL1494: _temp1493=( void*) _temp1483.elt_typ; goto _LL1492; _LL1492: _temp1491=(
void*) _temp1483.rgn_typ; goto _LL1490; _LL1490: _temp1489= _temp1483.nullable;
goto _LL1488; _LL1488: _temp1487= _temp1483.tq; goto _LL1486; _LL1486: _temp1485=
_temp1483.bounds; goto _LL1480;} else{ goto _LL1481;} _LL1481: goto _LL1482;
_LL1480:{ void* _temp1495=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1485); _LL1497: if( _temp1495 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1498;} else{ goto _LL1499;} _LL1499: goto _LL1500; _LL1498: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1173))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1173))->tl))->hd; if(
Cyc_Tcutil_is_tagged_pointer_typ(( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1371))->tl))->hd)){( void*)( e1->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e1->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} else{( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_List_List*(* _temp1501)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1504= e1; struct
Cyc_Absyn_Exp* _temp1505= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1493),
0); struct Cyc_Absyn_Exp* _temp1506= Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus,
e2, 0); struct Cyc_Absyn_Exp* _temp1502[ 3u]={ _temp1504, _temp1505, _temp1506};
struct _tagged_arr _temp1503={( void*) _temp1502,( void*) _temp1502,( void*)(
_temp1502 + 3u)}; _temp1501( _temp1503);}), 0))->r));} goto _LL1496;} _LL1500:
goto _LL1496; _LL1496:;} goto _LL1478; _LL1482: goto _LL1478; _LL1478:;} goto
_LL1376; _LL1392: goto _LL1394; _LL1394: goto _LL1396; _LL1396: goto _LL1398;
_LL1398: goto _LL1400; _LL1400: goto _LL1402; _LL1402: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1173))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1173))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1371))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1371))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1376;} _LL1404: goto
_LL1376; _LL1376:;} goto _LL1084;}} _LL1096: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1179->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1179);{ void* _temp1507= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1513; struct Cyc_Absyn_Conref* _temp1515; struct Cyc_Absyn_Tqual _temp1517;
struct Cyc_Absyn_Conref* _temp1519; void* _temp1521; void* _temp1523; _LL1509:
if(( unsigned int) _temp1507 > 4u?*(( int*) _temp1507) == Cyc_Absyn_PointerType:
0){ _LL1514: _temp1513=(( struct Cyc_Absyn_PointerType_struct*) _temp1507)->f1;
_LL1524: _temp1523=( void*) _temp1513.elt_typ; goto _LL1522; _LL1522: _temp1521=(
void*) _temp1513.rgn_typ; goto _LL1520; _LL1520: _temp1519= _temp1513.nullable;
goto _LL1518; _LL1518: _temp1517= _temp1513.tq; goto _LL1516; _LL1516: _temp1515=
_temp1513.bounds; goto _LL1510;} else{ goto _LL1511;} _LL1511: goto _LL1512;
_LL1510:{ void* _temp1525=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1515); _LL1527: if( _temp1525 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1528;} else{ goto _LL1529;} _LL1529: goto _LL1530; _LL1528: { struct Cyc_Absyn_Exp*
_temp1531=( _temp1177 ==( void*) Cyc_Absyn_PostInc? 1: _temp1177 ==( void*) Cyc_Absyn_PostDec)?
Cyc_Toc__tagged_arr_inplace_plus_post_e: Cyc_Toc__tagged_arr_inplace_plus_e;
goto _LL1532; _LL1532: { int _temp1533=( _temp1177 ==( void*) Cyc_Absyn_PreInc?
1: _temp1177 ==( void*) Cyc_Absyn_PostInc)? 1: - 1; goto _LL1534; _LL1534:( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( _temp1531,({ struct Cyc_List_List*(*
_temp1535)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp1538= Cyc_Absyn_address_exp( _temp1179,
0); struct Cyc_Absyn_Exp* _temp1539= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
_temp1523), 0); struct Cyc_Absyn_Exp* _temp1540= Cyc_Absyn_signed_int_exp(
_temp1533, 0); struct Cyc_Absyn_Exp* _temp1536[ 3u]={ _temp1538, _temp1539,
_temp1540}; struct _tagged_arr _temp1537={( void*) _temp1536,( void*) _temp1536,(
void*)( _temp1536 + 3u)}; _temp1535( _temp1537);}), 0))->r)); goto _LL1526;}}
_LL1530: goto _LL1526; _LL1526:;} goto _LL1508; _LL1512: if( ! Cyc_Absyn_is_lvalue(
_temp1179)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1179, 0, Cyc_Toc_incr_lvalue, _temp1177);( void*)( e->r=( void*)(( void*)
_temp1179->r));} goto _LL1508; _LL1508:;} goto _LL1084;} _LL1098: { int e1_poly=
Cyc_Toc_is_poly_project( _temp1185); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1185->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1181->topt))->v; Cyc_Toc_exp_to_c( nv, _temp1185); Cyc_Toc_exp_to_c(
nv, _temp1181);{ int done= 0; if( _temp1183 != 0){ void* _temp1541= Cyc_Tcutil_compress(
e1_old_typ); struct Cyc_Absyn_PtrInfo _temp1547; struct Cyc_Absyn_Conref*
_temp1549; struct Cyc_Absyn_Tqual _temp1551; struct Cyc_Absyn_Conref* _temp1553;
void* _temp1555; void* _temp1557; _LL1543: if(( unsigned int) _temp1541 > 4u?*((
int*) _temp1541) == Cyc_Absyn_PointerType: 0){ _LL1548: _temp1547=(( struct Cyc_Absyn_PointerType_struct*)
_temp1541)->f1; _LL1558: _temp1557=( void*) _temp1547.elt_typ; goto _LL1556;
_LL1556: _temp1555=( void*) _temp1547.rgn_typ; goto _LL1554; _LL1554: _temp1553=
_temp1547.nullable; goto _LL1552; _LL1552: _temp1551= _temp1547.tq; goto _LL1550;
_LL1550: _temp1549= _temp1547.bounds; goto _LL1544;} else{ goto _LL1545;}
_LL1545: goto _LL1546; _LL1544:{ void* _temp1559=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1549); _LL1561: if(
_temp1559 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1562;} else{ goto _LL1563;}
_LL1563: goto _LL1564; _LL1562: { struct Cyc_Absyn_Exp* change;{ void* _temp1565=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1183))->v; _LL1567: if(
_temp1565 ==( void*) Cyc_Absyn_Plus){ goto _LL1568;} else{ goto _LL1569;}
_LL1569: if( _temp1565 ==( void*) Cyc_Absyn_Minus){ goto _LL1570;} else{ goto
_LL1571;} _LL1571: goto _LL1572; _LL1568: change= _temp1181; goto _LL1566;
_LL1570: change= Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp1181, 0);
goto _LL1566; _LL1572:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("bad t ? pointer arithmetic", sizeof( unsigned char), 27u))); goto
_LL1566; _LL1566:;} done= 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_inplace_plus_e,({ struct Cyc_List_List*(* _temp1573)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1576= Cyc_Absyn_address_exp( _temp1185, 0); struct
Cyc_Absyn_Exp* _temp1577= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1557),
0); struct Cyc_Absyn_Exp* _temp1578= change; struct Cyc_Absyn_Exp* _temp1574[ 3u]={
_temp1576, _temp1577, _temp1578}; struct _tagged_arr _temp1575={( void*)
_temp1574,( void*) _temp1574,( void*)( _temp1574 + 3u)}; _temp1573( _temp1575);}),
0))->r)); goto _LL1560;} _LL1564: goto _LL1560; _LL1560:;} goto _LL1542; _LL1546:
goto _LL1542; _LL1542:;} if( ! done){ if( e1_poly){( void*)( _temp1181->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1181->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp1185)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, struct _tuple9*), struct _tuple9* f_env)) Cyc_Toc_lvalue_assign)(
_temp1185, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple9* _temp1579=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9) * 1); _temp1579[ 0]=({ struct
_tuple9 _temp1580; _temp1580.f1= _temp1183; _temp1580.f2= _temp1181; _temp1580;});
_temp1579;}));( void*)( e->r=( void*)(( void*) _temp1185->r));}} goto _LL1084;}}
_LL1100: Cyc_Toc_exp_to_c( nv, _temp1191); Cyc_Toc_exp_to_c( nv, _temp1189); Cyc_Toc_exp_to_c(
nv, _temp1187); goto _LL1084; _LL1102: Cyc_Toc_exp_to_c( nv, _temp1195); Cyc_Toc_exp_to_c(
nv, _temp1193); goto _LL1084; _LL1104: _temp1205= _temp1199; _temp1203=
_temp1197; goto _LL1106; _LL1106: Cyc_Toc_exp_to_c( nv, _temp1205);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1203);
goto _LL1084; _LL1108:{ struct _RegionHandle _temp1581= _new_region(); struct
_RegionHandle* r=& _temp1581; _push_region( r);{ void* _temp1582=( void*)
_temp1210->type; goto _LL1583; _LL1583: { void* _temp1584= Cyc_Toc_typ_to_c(
_temp1582); goto _LL1585; _LL1585: { void* _temp1586= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1218->topt))->v); goto _LL1587; _LL1587: {
struct Cyc_Absyn_Exp* _temp1588= Cyc_Absyn_uint_exp(( unsigned int) _temp1214, 0);
goto _LL1589; _LL1589: { void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1656=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1656[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1657; _temp1657.tag= Cyc_Absyn_ArrayType;
_temp1657.f1=( void*) _temp1582; _temp1657.f2= Cyc_Toc_mt_tq; _temp1657.f3=(
struct Cyc_Absyn_Exp*) _temp1588; _temp1657;}); _temp1656;})); struct _tuple0*
_temp1590= Cyc_Toc_temp_var(); goto _LL1591; _LL1591: { struct _tuple0*
_temp1592= Cyc_Toc_temp_var(); goto _LL1593; _LL1593: { struct Cyc_Absyn_Exp*
_temp1594= Cyc_Absyn_var_exp( _temp1592, 0); goto _LL1595; _LL1595: { struct
_tuple0* _temp1596= Cyc_Toc_temp_var(); goto _LL1597; _LL1597: { int _temp1598=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1216); goto _LL1599;
_LL1599: { int _temp1600= _temp1598 - _temp1214; goto _LL1601; _LL1601: { struct
_tagged_arr vs=({ unsigned int _temp1652=( unsigned int) _temp1214; struct
_tuple0** _temp1653=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1652); struct _tagged_arr _temp1655= _tag_arr( _temp1653,
sizeof( struct _tuple0*),( unsigned int) _temp1214);{ unsigned int _temp1654=
_temp1652; unsigned int i; for( i= 0; i < _temp1654; i ++){ _temp1653[ i]= Cyc_Toc_temp_var();}};
_temp1655;}); struct _tagged_arr xs=({ unsigned int _temp1648=( unsigned int)
_temp1600; struct _tuple0** _temp1649=( struct _tuple0**) _region_malloc( r,
sizeof( struct _tuple0*) * _temp1648); struct _tagged_arr _temp1651= _tag_arr(
_temp1649, sizeof( struct _tuple0*),( unsigned int) _temp1600);{ unsigned int
_temp1650= _temp1648; unsigned int i; for( i= 0; i < _temp1650; i ++){ _temp1649[
i]= Cyc_Toc_temp_var();}}; _temp1651;}); struct Cyc_List_List* xexps=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,(
struct Cyc_Position_Segment*) 0,(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, xs));
xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List* _temp1602=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1602->hd=( void*) Cyc_Absyn_var_exp(
_temp1596, 0); _temp1602->tl= 0; _temp1602;}));{ struct Cyc_Absyn_Stmt*
_temp1603= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
_temp1590, 0), xexps, 0), 0); goto _LL1604; _LL1604: { struct Cyc_Absyn_Exp*
_temp1605= Cyc_Absyn_add_exp( _temp1594, _temp1588, 0); goto _LL1606; _LL1606: {
struct Cyc_List_List* _temp1616=({ struct Cyc_List_List*(* _temp1607)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct _tuple4* _temp1610=({ struct _tuple4* _temp1615=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1615->f1= 0; _temp1615->f2= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1594, 0); _temp1615;}); struct _tuple4*
_temp1611=({ struct _tuple4* _temp1614=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1614->f1= 0; _temp1614->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1594, 0); _temp1614;}); struct _tuple4* _temp1612=({ struct _tuple4*
_temp1613=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1613->f1=
0; _temp1613->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1605, 0);
_temp1613;}); struct _tuple4* _temp1608[ 3u]={ _temp1610, _temp1611, _temp1612};
struct _tagged_arr _temp1609={( void*) _temp1608,( void*) _temp1608,( void*)(
_temp1608 + 3u)}; _temp1607( _temp1609);}); goto _LL1617; _LL1617: _temp1603=
Cyc_Absyn_declare_stmt( _temp1596, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp1582,( void*) _temp1210->rgn, Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp(
0, _temp1616, 0), _temp1603, 0);{ struct Cyc_List_List* _temp1618=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, vs));
goto _LL1619; _LL1619: { struct Cyc_List_List* _temp1620=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_make_dle, _temp1618); goto _LL1621; _LL1621: _temp1603= Cyc_Absyn_declare_stmt(
_temp1592, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1620, 0), _temp1603, 0);{ struct Cyc_List_List* _temp1622=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1216);
goto _LL1623; _LL1623: { struct Cyc_List_List* _temp1624=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1212);
goto _LL1625; _LL1625:{ int i= _temp1214 - 1; for( 0; i >= 0;( i --, _temp1622=((
struct Cyc_List_List*) _check_null( _temp1622))->tl)){ struct Cyc_Absyn_Exp* arg=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1622))->hd;
void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c(
nv, arg); if( _temp1210->inject){ void* _temp1628; struct Cyc_Position_Segment*
_temp1630; struct Cyc_List_List* _temp1632; struct Cyc_List_List* _temp1634;
struct _tuple0* _temp1636; struct Cyc_Absyn_Tunionfield _temp1626=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1624))->hd); _LL1637: _temp1636=
_temp1626.name; goto _LL1635; _LL1635: _temp1634= _temp1626.tvs; goto _LL1633;
_LL1633: _temp1632= _temp1626.typs; goto _LL1631; _LL1631: _temp1630= _temp1626.loc;
goto _LL1629; _LL1629: _temp1628=( void*) _temp1626.sc; goto _LL1627; _LL1627:
_temp1624=(( struct Cyc_List_List*) _check_null( _temp1624))->tl;{ struct
_tuple0* _temp1638= Cyc_Toc_temp_var(); goto _LL1639; _LL1639: { struct Cyc_Absyn_Exp*
_temp1640= Cyc_Absyn_var_exp( _temp1638, 0); goto _LL1641; _LL1641: { void*
_temp1642= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1636, _tag_arr("_struct",
sizeof( unsigned char), 8u))); goto _LL1643; _LL1643: _temp1603= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1584,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1584, Cyc_Absyn_address_exp(
_temp1640, 0), 0), _temp1603, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple3*)(( struct Cyc_List_List*) _check_null( _temp1632))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1644= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1640, Cyc_Toc_fieldname(
1), 0), arg, 0); goto _LL1645; _LL1645: _temp1603= Cyc_Absyn_seq_stmt( _temp1644,
_temp1603, 0);{ struct Cyc_Absyn_Stmt* _temp1646= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1640, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1636, 0), 0); goto
_LL1647; _LL1647: _temp1603= Cyc_Absyn_seq_stmt( _temp1646, _temp1603, 0);
_temp1603= Cyc_Absyn_declare_stmt( _temp1638, _temp1642, 0, _temp1603, 0);}}}}}}}
else{ _temp1603= Cyc_Absyn_declare_stmt(*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)), _temp1584,( struct
Cyc_Absyn_Exp*) arg, _temp1603, 0);}}}{ int i= _temp1600 - 1; for( 0; i >= 0;( i
--, _temp1622=(( struct Cyc_List_List*) _check_null( _temp1622))->tl)){ struct
Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1622))->hd; void* argtype= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); Cyc_Toc_exp_to_c( nv, arg); _temp1603= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( xs, sizeof( struct _tuple0*), i)),
argtype,( struct Cyc_Absyn_Exp*) arg, _temp1603, 0);}} Cyc_Toc_exp_to_c( nv,
_temp1218); _temp1603= Cyc_Absyn_declare_stmt( _temp1590, _temp1586,( struct Cyc_Absyn_Exp*)
_temp1218, _temp1603, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1603, 0))->r));}}}}}}}}}}}}}}}}}}; _pop_region(& _temp1581);} goto _LL1084;
_LL1110: Cyc_Toc_exp_to_c( nv, _temp1220);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1220), 0))->r)); goto
_LL1084; _LL1112: Cyc_Toc_exp_to_c( nv, _temp1222); goto _LL1084; _LL1114: Cyc_Toc_exp_to_c(
nv, _temp1226);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); for( 0; _temp1224 != 0; _temp1224=(( struct Cyc_List_List*)
_check_null( _temp1224))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct
Cyc_List_List*) _check_null( _temp1224))->hd); if( k !=( void*) Cyc_Absyn_EffKind?
k !=( void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1226, 0))->r)); break;}} goto _LL1084;} _LL1116: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1228->topt))->v; void* new_typ=*
_temp1232;* _temp1232= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1228);{ struct _tuple10 _temp1659=({ struct _tuple10 _temp1658; _temp1658.f1=
Cyc_Tcutil_compress( old_t2); _temp1658.f2= Cyc_Tcutil_compress( new_typ);
_temp1658;}); void* _temp1667; struct Cyc_Absyn_PtrInfo _temp1669; void*
_temp1671; struct Cyc_Absyn_PtrInfo _temp1673; void* _temp1675; void* _temp1677;
void* _temp1679; void* _temp1681; struct Cyc_Absyn_PtrInfo _temp1683; _LL1661:
_LL1672: _temp1671= _temp1659.f1; if(( unsigned int) _temp1671 > 4u?*(( int*)
_temp1671) == Cyc_Absyn_PointerType: 0){ _LL1674: _temp1673=(( struct Cyc_Absyn_PointerType_struct*)
_temp1671)->f1; goto _LL1668;} else{ goto _LL1663;} _LL1668: _temp1667=
_temp1659.f2; if(( unsigned int) _temp1667 > 4u?*(( int*) _temp1667) == Cyc_Absyn_PointerType:
0){ _LL1670: _temp1669=(( struct Cyc_Absyn_PointerType_struct*) _temp1667)->f1;
goto _LL1662;} else{ goto _LL1663;} _LL1663: _LL1682: _temp1681= _temp1659.f1;
if(( unsigned int) _temp1681 > 4u?*(( int*) _temp1681) == Cyc_Absyn_PointerType:
0){ _LL1684: _temp1683=(( struct Cyc_Absyn_PointerType_struct*) _temp1681)->f1;
goto _LL1676;} else{ goto _LL1665;} _LL1676: _temp1675= _temp1659.f2; if((
unsigned int) _temp1675 > 4u?*(( int*) _temp1675) == Cyc_Absyn_IntType: 0){
_LL1680: _temp1679=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1675)->f1;
goto _LL1678; _LL1678: _temp1677=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1675)->f2; goto _LL1664;} else{ goto _LL1665;} _LL1665: goto _LL1666;
_LL1662: { int _temp1685=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1673.nullable); goto _LL1686; _LL1686: { int _temp1687=(( int(*)( int,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1669.nullable); goto
_LL1688; _LL1688: { void* _temp1689=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1673.bounds); goto _LL1690;
_LL1690: { void* _temp1691=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1669.bounds); goto _LL1692; _LL1692:{ struct _tuple10
_temp1694=({ struct _tuple10 _temp1693; _temp1693.f1= _temp1689; _temp1693.f2=
_temp1691; _temp1693;}); void* _temp1704; struct Cyc_Absyn_Exp* _temp1706; void*
_temp1708; struct Cyc_Absyn_Exp* _temp1710; void* _temp1712; void* _temp1714;
struct Cyc_Absyn_Exp* _temp1716; void* _temp1718; struct Cyc_Absyn_Exp*
_temp1720; void* _temp1722; _LL1696: _LL1709: _temp1708= _temp1694.f1; if((
unsigned int) _temp1708 > 1u?*(( int*) _temp1708) == Cyc_Absyn_Upper_b: 0){
_LL1711: _temp1710=(( struct Cyc_Absyn_Upper_b_struct*) _temp1708)->f1; goto
_LL1705;} else{ goto _LL1698;} _LL1705: _temp1704= _temp1694.f2; if((
unsigned int) _temp1704 > 1u?*(( int*) _temp1704) == Cyc_Absyn_Upper_b: 0){
_LL1707: _temp1706=(( struct Cyc_Absyn_Upper_b_struct*) _temp1704)->f1; goto
_LL1697;} else{ goto _LL1698;} _LL1698: _LL1715: _temp1714= _temp1694.f1; if((
unsigned int) _temp1714 > 1u?*(( int*) _temp1714) == Cyc_Absyn_Upper_b: 0){
_LL1717: _temp1716=(( struct Cyc_Absyn_Upper_b_struct*) _temp1714)->f1; goto
_LL1713;} else{ goto _LL1700;} _LL1713: _temp1712= _temp1694.f2; if( _temp1712
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1699;} else{ goto _LL1700;} _LL1700:
_LL1723: _temp1722= _temp1694.f1; if( _temp1722 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1719;} else{ goto _LL1702;} _LL1719: _temp1718= _temp1694.f2; if((
unsigned int) _temp1718 > 1u?*(( int*) _temp1718) == Cyc_Absyn_Upper_b: 0){
_LL1721: _temp1720=(( struct Cyc_Absyn_Upper_b_struct*) _temp1718)->f1; goto
_LL1701;} else{ goto _LL1702;} _LL1702: goto _LL1703; _LL1697: if( _temp1685? !
_temp1687: 0){ if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't do null-check conversion at top-level", sizeof( unsigned char),
44u));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp1232, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1724=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1724->hd=( void*) _temp1228;
_temp1724->tl= 0; _temp1724;}), 0), 0))->r));} goto _LL1695; _LL1699: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1716, _temp1228))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1725)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1728= _temp1228; struct Cyc_Absyn_Exp* _temp1729= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1669.elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1730= _temp1716; struct Cyc_Absyn_Exp* _temp1726[ 3u]={ _temp1728,
_temp1729, _temp1730}; struct _tagged_arr _temp1727={( void*) _temp1726,( void*)
_temp1726,( void*)( _temp1726 + 3u)}; _temp1725( _temp1727);}), 0))->r));} goto
_LL1695; _LL1701: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u));}{ struct Cyc_Absyn_Exp* _temp1737= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({
struct Cyc_List_List*(* _temp1731)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1734= _temp1228;
struct Cyc_Absyn_Exp* _temp1735= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c((
void*) _temp1673.elt_typ), 0); struct Cyc_Absyn_Exp* _temp1736= _temp1720;
struct Cyc_Absyn_Exp* _temp1732[ 3u]={ _temp1734, _temp1735, _temp1736}; struct
_tagged_arr _temp1733={( void*) _temp1732,( void*) _temp1732,( void*)( _temp1732
+ 3u)}; _temp1731( _temp1733);}), 0); goto _LL1738; _LL1738: if( _temp1687){(
void*)( _temp1737->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1739=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1739->hd=( void*) Cyc_Absyn_copy_exp( _temp1737);
_temp1739->tl= 0; _temp1739;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp1232, _temp1737, 0))->r)); goto _LL1695;} _LL1703: goto _LL1695; _LL1695:;}
goto _LL1660;}}}} _LL1664:{ void* _temp1740=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1683.bounds); _LL1742: if(
_temp1740 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1743;} else{ goto _LL1744;}
_LL1744: goto _LL1745; _LL1743:( void*)( _temp1228->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1228->r, _temp1228->loc), Cyc_Toc_curr_sp, 0))->r));
goto _LL1741; _LL1745: goto _LL1741; _LL1741:;} goto _LL1660; _LL1666: goto
_LL1660; _LL1660:;} goto _LL1084;} _LL1118:{ void* _temp1746=( void*) _temp1233->r;
struct Cyc_Absyn_Structdecl* _temp1754; struct Cyc_List_List* _temp1756; struct
Cyc_Core_Opt* _temp1758; struct _tuple0* _temp1760; struct Cyc_List_List*
_temp1762; _LL1748: if(*(( int*) _temp1746) == Cyc_Absyn_Struct_e){ _LL1761:
_temp1760=(( struct Cyc_Absyn_Struct_e_struct*) _temp1746)->f1; goto _LL1759;
_LL1759: _temp1758=(( struct Cyc_Absyn_Struct_e_struct*) _temp1746)->f2; goto
_LL1757; _LL1757: _temp1756=(( struct Cyc_Absyn_Struct_e_struct*) _temp1746)->f3;
goto _LL1755; _LL1755: _temp1754=(( struct Cyc_Absyn_Struct_e_struct*) _temp1746)->f4;
goto _LL1749;} else{ goto _LL1750;} _LL1750: if(*(( int*) _temp1746) == Cyc_Absyn_Tuple_e){
_LL1763: _temp1762=(( struct Cyc_Absyn_Tuple_e_struct*) _temp1746)->f1; goto
_LL1751;} else{ goto _LL1752;} _LL1752: goto _LL1753; _LL1749: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr _temp1764=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1233->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1764.last_plus_one - _temp1764.curr, _temp1764.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1233->topt))->v, 1, 0, _temp1756, _temp1760))->r)); goto
_LL1747; _LL1751: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)((
struct _tagged_arr)({ struct _tagged_arr _temp1765=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1233->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1765.last_plus_one - _temp1765.curr, _temp1765.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1762))->r)); goto _LL1747;
_LL1753: Cyc_Toc_exp_to_c( nv, _temp1233); if( ! Cyc_Absyn_is_lvalue( _temp1233)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1233,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1233, 0))->r));} goto _LL1747; _LL1747:;} goto _LL1084; _LL1120: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr _temp1766=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1235->loc); xprintf("%.*s: new at top-level", _temp1766.last_plus_one -
_temp1766.curr, _temp1766.curr);}));}{ void* _temp1767=( void*) _temp1235->r;
struct Cyc_List_List* _temp1779; struct Cyc_Absyn_Exp* _temp1781; struct Cyc_Absyn_Exp*
_temp1783; struct Cyc_Absyn_Vardecl* _temp1785; struct Cyc_Absyn_Structdecl*
_temp1787; struct Cyc_List_List* _temp1789; struct Cyc_Core_Opt* _temp1791;
struct _tuple0* _temp1793; struct Cyc_List_List* _temp1795; _LL1769: if(*(( int*)
_temp1767) == Cyc_Absyn_Array_e){ _LL1780: _temp1779=(( struct Cyc_Absyn_Array_e_struct*)
_temp1767)->f1; goto _LL1770;} else{ goto _LL1771;} _LL1771: if(*(( int*)
_temp1767) == Cyc_Absyn_Comprehension_e){ _LL1786: _temp1785=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1767)->f1; goto _LL1784; _LL1784: _temp1783=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1767)->f2; goto _LL1782; _LL1782: _temp1781=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1767)->f3; goto _LL1772;} else{ goto _LL1773;} _LL1773: if(*(( int*)
_temp1767) == Cyc_Absyn_Struct_e){ _LL1794: _temp1793=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1767)->f1; goto _LL1792; _LL1792: _temp1791=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1767)->f2; goto _LL1790; _LL1790: _temp1789=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1767)->f3; goto _LL1788; _LL1788: _temp1787=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1767)->f4; goto _LL1774;} else{ goto _LL1775;} _LL1775: if(*(( int*)
_temp1767) == Cyc_Absyn_Tuple_e){ _LL1796: _temp1795=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1767)->f1; goto _LL1776;} else{ goto _LL1777;} _LL1777: goto _LL1778;
_LL1770: { struct _tuple0* _temp1797= Cyc_Toc_temp_var(); goto _LL1798; _LL1798: {
struct Cyc_Absyn_Exp* _temp1799= Cyc_Absyn_var_exp( _temp1797, 0); goto _LL1800;
_LL1800: { struct Cyc_Absyn_Stmt* _temp1801= Cyc_Toc_init_array( nv, _temp1799,
_temp1779, Cyc_Absyn_exp_stmt( _temp1799, 0)); goto _LL1802; _LL1802: { void*
old_elt_typ;{ void* _temp1803= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1809; struct Cyc_Absyn_Conref* _temp1811; struct Cyc_Absyn_Tqual _temp1813;
struct Cyc_Absyn_Conref* _temp1815; void* _temp1817; void* _temp1819; _LL1805:
if(( unsigned int) _temp1803 > 4u?*(( int*) _temp1803) == Cyc_Absyn_PointerType:
0){ _LL1810: _temp1809=(( struct Cyc_Absyn_PointerType_struct*) _temp1803)->f1;
_LL1820: _temp1819=( void*) _temp1809.elt_typ; goto _LL1818; _LL1818: _temp1817=(
void*) _temp1809.rgn_typ; goto _LL1816; _LL1816: _temp1815= _temp1809.nullable;
goto _LL1814; _LL1814: _temp1813= _temp1809.tq; goto _LL1812; _LL1812: _temp1811=
_temp1809.bounds; goto _LL1806;} else{ goto _LL1807;} _LL1807: goto _LL1808;
_LL1806: old_elt_typ= _temp1819; goto _LL1804; _LL1808: old_elt_typ=(( void*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u)); goto _LL1804; _LL1804:;}{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* _temp1821= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
goto _LL1822; _LL1822: { struct Cyc_Absyn_Exp* _temp1823= Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1779), 0), 0); goto _LL1824; _LL1824: {
struct Cyc_Absyn_Exp* e1; if( _temp1237 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1823);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1237); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1823);}{ struct Cyc_Absyn_Exp* _temp1825= Cyc_Absyn_cast_exp( _temp1821, e1,
0); goto _LL1826; _LL1826:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1797, _temp1821,( struct Cyc_Absyn_Exp*) _temp1825,
_temp1801, 0), 0))->r)); goto _LL1768;}}}}}}}} _LL1772: { int is_tagged_ptr= 0;{
void* _temp1827= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1833; struct Cyc_Absyn_Conref* _temp1835; struct Cyc_Absyn_Tqual _temp1837;
struct Cyc_Absyn_Conref* _temp1839; void* _temp1841; void* _temp1843; _LL1829:
if(( unsigned int) _temp1827 > 4u?*(( int*) _temp1827) == Cyc_Absyn_PointerType:
0){ _LL1834: _temp1833=(( struct Cyc_Absyn_PointerType_struct*) _temp1827)->f1;
_LL1844: _temp1843=( void*) _temp1833.elt_typ; goto _LL1842; _LL1842: _temp1841=(
void*) _temp1833.rgn_typ; goto _LL1840; _LL1840: _temp1839= _temp1833.nullable;
goto _LL1838; _LL1838: _temp1837= _temp1833.tq; goto _LL1836; _LL1836: _temp1835=
_temp1833.bounds; goto _LL1830;} else{ goto _LL1831;} _LL1831: goto _LL1832;
_LL1830: is_tagged_ptr=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1835) ==( void*) Cyc_Absyn_Unknown_b; goto _LL1828;
_LL1832:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u)); goto _LL1828; _LL1828:;}{ struct _tuple0* max=
Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* old_elt_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1781->topt))->v; void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1783);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1785, Cyc_Absyn_var_exp( max, 0), _temp1781,
Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1845= _new_region();
struct _RegionHandle* r=& _temp1845; _push_region( r);{ struct Cyc_List_List*
decls=({ struct Cyc_List_List* _temp1872=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1872->hd=( void*)({ struct _tuple8*
_temp1873=( struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8));
_temp1873->f1= max; _temp1873->f2= Cyc_Absyn_uint_t; _temp1873->f3=( struct Cyc_Absyn_Exp*)
_temp1783; _temp1873;}); _temp1872->tl= 0; _temp1872;}); struct Cyc_Absyn_Exp*
ai; if( _temp1237 == 0){ ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{
struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1237); Cyc_Toc_exp_to_c(
nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp(
ptr_typ, ai, 0); decls=({ struct Cyc_List_List* _temp1846=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1846->hd=( void*)({
struct _tuple8* _temp1847=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1847->f1= a; _temp1847->f2= ptr_typ; _temp1847->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1847;}); _temp1846->tl= decls; _temp1846;}); if( is_tagged_ptr){
struct _tuple0* _temp1848= Cyc_Toc_temp_var(); goto _LL1849; _LL1849: { void*
_temp1850= Cyc_Toc_typ_to_c( old_typ); goto _LL1851; _LL1851: { struct Cyc_Absyn_Exp*
_temp1858= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(*
_temp1852)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp1855= Cyc_Absyn_var_exp( a, 0); struct
Cyc_Absyn_Exp* _temp1856= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); struct Cyc_Absyn_Exp*
_temp1857= _temp1783; struct Cyc_Absyn_Exp* _temp1853[ 3u]={ _temp1855,
_temp1856, _temp1857}; struct _tagged_arr _temp1854={( void*) _temp1853,( void*)
_temp1853,( void*)( _temp1853 + 3u)}; _temp1852( _temp1854);}), 0); goto _LL1859;
_LL1859: decls=({ struct Cyc_List_List* _temp1860=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1860->hd=( void*)({
struct _tuple8* _temp1861=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1861->f1= _temp1848; _temp1861->f2= _temp1850; _temp1861->f3=(
struct Cyc_Absyn_Exp*) _temp1858; _temp1861;}); _temp1860->tl= decls; _temp1860;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1848, 0),
0), 0);}}} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1862= decls; goto _LL1863; _LL1863:
for( 0; _temp1862 != 0; _temp1862=(( struct Cyc_List_List*) _check_null(
_temp1862))->tl){ struct Cyc_Absyn_Exp* _temp1866; void* _temp1868; struct
_tuple0* _temp1870; struct _tuple8 _temp1864=*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp1862))->hd); _LL1871: _temp1870= _temp1864.f1; goto _LL1869;
_LL1869: _temp1868= _temp1864.f2; goto _LL1867; _LL1867: _temp1866= _temp1864.f3;
goto _LL1865; _LL1865: s= Cyc_Absyn_declare_stmt( _temp1870, _temp1868,
_temp1866, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}};
_pop_region(& _temp1845);} goto _LL1768;}}} _LL1774:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1235->topt))->v, 1, _temp1237, _temp1789, _temp1793))->r)); goto _LL1768;
_LL1776:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1237,
_temp1795))->r)); goto _LL1768; _LL1778: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1235->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp1237 == 0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1237); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1874=( void*)
_temp1235->r; struct Cyc_Absyn_Exp* _temp1880; void* _temp1882; _LL1876: if(*((
int*) _temp1874) == Cyc_Absyn_Cast_e){ _LL1883: _temp1882=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1874)->f1; goto _LL1881; _LL1881: _temp1880=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1874)->f2; goto _LL1877;} else{ goto _LL1878;} _LL1878: goto _LL1879;
_LL1877:{ struct _tuple10 _temp1885=({ struct _tuple10 _temp1884; _temp1884.f1=
Cyc_Tcutil_compress( _temp1882); _temp1884.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1880->topt))->v); _temp1884;}); void*
_temp1891; struct Cyc_Absyn_PtrInfo _temp1893; struct Cyc_Absyn_Conref*
_temp1895; struct Cyc_Absyn_Tqual _temp1897; struct Cyc_Absyn_Conref* _temp1899;
void* _temp1901; void* _temp1903; void* _temp1905; struct Cyc_Absyn_PtrInfo
_temp1907; struct Cyc_Absyn_Conref* _temp1909; struct Cyc_Absyn_Tqual _temp1911;
struct Cyc_Absyn_Conref* _temp1913; void* _temp1915; void* _temp1917; _LL1887:
_LL1906: _temp1905= _temp1885.f1; if(( unsigned int) _temp1905 > 4u?*(( int*)
_temp1905) == Cyc_Absyn_PointerType: 0){ _LL1908: _temp1907=(( struct Cyc_Absyn_PointerType_struct*)
_temp1905)->f1; _LL1918: _temp1917=( void*) _temp1907.elt_typ; goto _LL1916;
_LL1916: _temp1915=( void*) _temp1907.rgn_typ; goto _LL1914; _LL1914: _temp1913=
_temp1907.nullable; goto _LL1912; _LL1912: _temp1911= _temp1907.tq; goto _LL1910;
_LL1910: _temp1909= _temp1907.bounds; goto _LL1892;} else{ goto _LL1889;}
_LL1892: _temp1891= _temp1885.f2; if(( unsigned int) _temp1891 > 4u?*(( int*)
_temp1891) == Cyc_Absyn_PointerType: 0){ _LL1894: _temp1893=(( struct Cyc_Absyn_PointerType_struct*)
_temp1891)->f1; _LL1904: _temp1903=( void*) _temp1893.elt_typ; goto _LL1902;
_LL1902: _temp1901=( void*) _temp1893.rgn_typ; goto _LL1900; _LL1900: _temp1899=
_temp1893.nullable; goto _LL1898; _LL1898: _temp1897= _temp1893.tq; goto _LL1896;
_LL1896: _temp1895= _temp1893.bounds; goto _LL1888;} else{ goto _LL1889;}
_LL1889: goto _LL1890; _LL1888:{ struct _tuple10 _temp1920=({ struct _tuple10
_temp1919; _temp1919.f1=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1909); _temp1919.f2=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1895); _temp1919;}); void*
_temp1926; struct Cyc_Absyn_Exp* _temp1928; void* _temp1930; _LL1922: _LL1931:
_temp1930= _temp1920.f1; if( _temp1930 ==( void*) Cyc_Absyn_Unknown_b){ goto
_LL1927;} else{ goto _LL1924;} _LL1927: _temp1926= _temp1920.f2; if((
unsigned int) _temp1926 > 1u?*(( int*) _temp1926) == Cyc_Absyn_Upper_b: 0){
_LL1929: _temp1928=(( struct Cyc_Absyn_Upper_b_struct*) _temp1926)->f1; goto
_LL1923;} else{ goto _LL1924;} _LL1924: goto _LL1925; _LL1923: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_List_List*(* _temp1932)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1935= mexp;
struct Cyc_Absyn_Exp* _temp1936= _temp1880; struct Cyc_Absyn_Exp* _temp1937= Cyc_Absyn_sizeoftyp_exp(
_temp1917, 0); struct Cyc_Absyn_Exp* _temp1938= _temp1928; struct Cyc_Absyn_Exp*
_temp1933[ 4u]={ _temp1935, _temp1936, _temp1937, _temp1938}; struct _tagged_arr
_temp1934={( void*) _temp1933,( void*) _temp1933,( void*)( _temp1933 + 4u)};
_temp1932( _temp1934);}), 0))->r)); goto _LL1921; _LL1925: goto _LL1921; _LL1921:;}
goto _LL1886; _LL1890: goto _LL1886; _LL1886:;} goto _LL1875; _LL1879: goto
_LL1875; _LL1875:;} if( ! done){ struct _tuple0* _temp1939= Cyc_Toc_temp_var();
goto _LL1940; _LL1940: { struct Cyc_Absyn_Stmt* _temp1941= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1939, 0), 0); goto _LL1942; _LL1942: { struct Cyc_Absyn_Exp*
_temp1943= Cyc_Absyn_signed_int_exp( 0, 0); goto _LL1944; _LL1944: Cyc_Toc_exp_to_c(
nv, _temp1235); _temp1941= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1939, 0), _temp1943, 0), _temp1235, 0), _temp1941, 0);{
void* _temp1945= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto _LL1946;
_LL1946:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1939, _temp1945,( struct Cyc_Absyn_Exp*) mexp, _temp1941, 0), 0))->r));}}}}
goto _LL1768;}} _LL1768:;} goto _LL1084; _LL1122: Cyc_Toc_exp_to_c( nv,
_temp1239); goto _LL1084; _LL1124:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1947=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1947[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1948; _temp1948.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1948.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1241);
_temp1948;}); _temp1947;}))); goto _LL1084; _LL1126:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1949=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1949[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1950; _temp1950.tag= Cyc_Absyn_Offsetof_e;
_temp1950.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1245); _temp1950.f2=
_temp1243; _temp1950;}); _temp1949;}))); goto _LL1084; _LL1128: { void*
_temp1951= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1247->topt))->v); goto _LL1952; _LL1952:{ void* _temp1953= _temp1951;
struct Cyc_Absyn_PtrInfo _temp1959; struct Cyc_Absyn_Conref* _temp1961; struct
Cyc_Absyn_Tqual _temp1963; struct Cyc_Absyn_Conref* _temp1965; void* _temp1967;
void* _temp1969; _LL1955: if(( unsigned int) _temp1953 > 4u?*(( int*) _temp1953)
== Cyc_Absyn_PointerType: 0){ _LL1960: _temp1959=(( struct Cyc_Absyn_PointerType_struct*)
_temp1953)->f1; _LL1970: _temp1969=( void*) _temp1959.elt_typ; goto _LL1968;
_LL1968: _temp1967=( void*) _temp1959.rgn_typ; goto _LL1966; _LL1966: _temp1965=
_temp1959.nullable; goto _LL1964; _LL1964: _temp1963= _temp1959.tq; goto _LL1962;
_LL1962: _temp1961= _temp1959.bounds; goto _LL1956;} else{ goto _LL1957;}
_LL1957: goto _LL1958; _LL1956:{ void* _temp1971=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1961); struct Cyc_Absyn_Exp*
_temp1977; _LL1973: if(( unsigned int) _temp1971 > 1u?*(( int*) _temp1971) ==
Cyc_Absyn_Upper_b: 0){ _LL1978: _temp1977=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1971)->f1; goto _LL1974;} else{ goto _LL1975;} _LL1975: if( _temp1971 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1976;} else{ goto _LL1972;} _LL1974: Cyc_Toc_exp_to_c(
nv, _temp1247); if( Cyc_Toc_is_nullable( _temp1951)){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1247->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1979=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1979->hd=( void*) _temp1247;
_temp1979->tl= 0; _temp1979;}), 0), 0), 0))->r));} goto _LL1972; _LL1976: {
struct Cyc_Absyn_Exp* _temp1980= Cyc_Absyn_uint_exp( 0, 0); goto _LL1981;
_LL1981: _temp1980->topt=({ struct Cyc_Core_Opt* _temp1982=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1982->v=( void*) Cyc_Absyn_uint_t;
_temp1982;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_subscript_exp(
_temp1247, _temp1980, 0))->r)); Cyc_Toc_exp_to_c( nv, e); goto _LL1972;} _LL1972:;}
goto _LL1954; _LL1958:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u)); goto
_LL1954; _LL1954:;} goto _LL1084;} _LL1130: Cyc_Toc_exp_to_c( nv, _temp1251);
if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1084; _LL1132: { void*
e1typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1255->topt))->v); Cyc_Toc_exp_to_c( nv, _temp1255);{ int is_poly= Cyc_Toc_is_poly_project(
e); struct Cyc_Absyn_Conref* _temp1985; struct Cyc_Absyn_Tqual _temp1987; struct
Cyc_Absyn_Conref* _temp1989; void* _temp1991; void* _temp1993; struct Cyc_Absyn_PtrInfo
_temp1983= Cyc_Toc_get_ptr_type( e1typ); _LL1994: _temp1993=( void*) _temp1983.elt_typ;
goto _LL1992; _LL1992: _temp1991=( void*) _temp1983.rgn_typ; goto _LL1990;
_LL1990: _temp1989= _temp1983.nullable; goto _LL1988; _LL1988: _temp1987=
_temp1983.tq; goto _LL1986; _LL1986: _temp1985= _temp1983.bounds; goto _LL1984;
_LL1984:{ void* _temp1995=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1985); struct Cyc_Absyn_Exp* _temp2001; _LL1997: if((
unsigned int) _temp1995 > 1u?*(( int*) _temp1995) == Cyc_Absyn_Upper_b: 0){
_LL2002: _temp2001=(( struct Cyc_Absyn_Upper_b_struct*) _temp1995)->f1; goto
_LL1998;} else{ goto _LL1999;} _LL1999: if( _temp1995 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL2000;} else{ goto _LL1996;} _LL1998: if( Cyc_Evexp_eval_const_uint_exp(
_temp2001) < 1){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof( unsigned char), 46u));}{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( 0, _temp1989); if( Cyc_Toc_is_nullable(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1255->topt))->v)){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1255->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp2003=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2003->hd=( void*) _temp1255;
_temp2003->tl= 0; _temp2003;}), 0), 0), _temp1253, 0))->r));} goto _LL1996;}
_LL2000: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1993);( void*)( _temp1255->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1987), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_List_List*(* _temp2004)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp2007= Cyc_Absyn_copy_exp( _temp1255); struct Cyc_Absyn_Exp*
_temp2008= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp* _temp2009=
Cyc_Absyn_uint_exp( 0, 0); struct Cyc_Absyn_Exp* _temp2005[ 3u]={ _temp2007,
_temp2008, _temp2009}; struct _tagged_arr _temp2006={( void*) _temp2005,( void*)
_temp2005,( void*)( _temp2005 + 3u)}; _temp2004( _temp2006);}), 0), 0))->r));
goto _LL1996;} _LL1996:;} if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1084;}} _LL1134: { void*
_temp2010= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1259->topt))->v); goto _LL2011; _LL2011: Cyc_Toc_exp_to_c( nv, _temp1259);
Cyc_Toc_exp_to_c( nv, _temp1257);{ void* _temp2012= _temp2010; struct Cyc_List_List*
_temp2020; struct Cyc_Absyn_PtrInfo _temp2022; struct Cyc_Absyn_Conref*
_temp2024; struct Cyc_Absyn_Tqual _temp2026; struct Cyc_Absyn_Conref* _temp2028;
void* _temp2030; void* _temp2032; _LL2014: if(( unsigned int) _temp2012 > 4u?*((
int*) _temp2012) == Cyc_Absyn_TupleType: 0){ _LL2021: _temp2020=(( struct Cyc_Absyn_TupleType_struct*)
_temp2012)->f1; goto _LL2015;} else{ goto _LL2016;} _LL2016: if(( unsigned int)
_temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_PointerType: 0){ _LL2023:
_temp2022=(( struct Cyc_Absyn_PointerType_struct*) _temp2012)->f1; _LL2033:
_temp2032=( void*) _temp2022.elt_typ; goto _LL2031; _LL2031: _temp2030=( void*)
_temp2022.rgn_typ; goto _LL2029; _LL2029: _temp2028= _temp2022.nullable; goto
_LL2027; _LL2027: _temp2026= _temp2022.tq; goto _LL2025; _LL2025: _temp2024=
_temp2022.bounds; goto _LL2017;} else{ goto _LL2018;} _LL2018: goto _LL2019;
_LL2015: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1257) + 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1259, Cyc_Toc_fieldname((
int) i), 0))->r)); goto _LL2013;} _LL2017:{ void* _temp2034=(( void*(*)( void*,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp2024); struct Cyc_Absyn_Exp* _temp2040; _LL2036: if(( unsigned int)
_temp2034 > 1u?*(( int*) _temp2034) == Cyc_Absyn_Upper_b: 0){ _LL2041: _temp2040=((
struct Cyc_Absyn_Upper_b_struct*) _temp2034)->f1; goto _LL2037;} else{ goto
_LL2038;} _LL2038: if( _temp2034 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2039;}
else{ goto _LL2035;} _LL2037: _temp2040= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp2040), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp2028); void* ta1= Cyc_Toc_typ_to_c( _temp2032);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp2026); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_List_List*(* _temp2042)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp2045= _temp1259; struct Cyc_Absyn_Exp* _temp2046=
_temp2040; struct Cyc_Absyn_Exp* _temp2047= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
struct Cyc_Absyn_Exp* _temp2048= _temp1257; struct Cyc_Absyn_Exp* _temp2043[ 4u]={
_temp2045, _temp2046, _temp2047, _temp2048}; struct _tagged_arr _temp2044={(
void*) _temp2043,( void*) _temp2043,( void*)( _temp2043 + 4u)}; _temp2042(
_temp2044);}), 0), 0), 0))->r));} else{( void*)( _temp1257->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_List_List*(*
_temp2049)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp2052= _temp2040; struct Cyc_Absyn_Exp*
_temp2053= Cyc_Absyn_copy_exp( _temp1257); struct Cyc_Absyn_Exp* _temp2050[ 2u]={
_temp2052, _temp2053}; struct _tagged_arr _temp2051={( void*) _temp2050,( void*)
_temp2050,( void*)( _temp2050 + 2u)}; _temp2049( _temp2051);}), 0))->r));} goto
_LL2035;} _LL2039: { void* ta1= Cyc_Toc_typ_to_c_array( _temp2032);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
ta1, _temp2026), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_List_List*(* _temp2054)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp2057= _temp1259;
struct Cyc_Absyn_Exp* _temp2058= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp2059= _temp1257; struct Cyc_Absyn_Exp* _temp2055[ 3u]={ _temp2057,
_temp2058, _temp2059}; struct _tagged_arr _temp2056={( void*) _temp2055,( void*)
_temp2055,( void*)( _temp2055 + 3u)}; _temp2054( _temp2056);}), 0), 0), 0))->r));
goto _LL2035;} _LL2035:;} goto _LL2013; _LL2019:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u)); goto _LL2013; _LL2013:;} goto _LL1084;} _LL1136:
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 0,
0, _temp1261))->r));} else{ struct Cyc_List_List* _temp2060=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, _temp1261); goto _LL2061; _LL2061: { struct _tagged_arr*
_temp2062= Cyc_Toc_add_tuple_type( _temp2060); goto _LL2063; _LL2063: { struct
Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1261 != 0;( _temp1261=(( struct
Cyc_List_List*) _check_null( _temp1261))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1261))->hd);
dles=({ struct Cyc_List_List* _temp2064=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2064->hd=( void*)({ struct _tuple4*
_temp2065=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp2065->f1=
0; _temp2065->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1261))->hd; _temp2065;}); _temp2064->tl= dles; _temp2064;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}}}
goto _LL1084; _LL1138:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1263, 0))->r));{ struct Cyc_List_List* _temp2066= _temp1263; goto
_LL2067; _LL2067: for( 0; _temp2066 != 0; _temp2066=(( struct Cyc_List_List*)
_check_null( _temp2066))->tl){ struct _tuple4 _temp2070; struct Cyc_Absyn_Exp*
_temp2071; struct Cyc_List_List* _temp2073; struct _tuple4* _temp2068=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp2066))->hd; _temp2070=*
_temp2068; _LL2074: _temp2073= _temp2070.f1; goto _LL2072; _LL2072: _temp2071=
_temp2070.f2; goto _LL2069; _LL2069: Cyc_Toc_exp_to_c( nv, _temp2071);}} goto
_LL1084; _LL1140: { unsigned int _temp2075= Cyc_Evexp_eval_const_uint_exp(
_temp1267); goto _LL2076; _LL2076: Cyc_Toc_exp_to_c( nv, _temp1265);{ struct Cyc_List_List*
es= 0; if( ! Cyc_Toc_is_zero( _temp1265)){ unsigned int i= 0; for( 0; i <
_temp2075; i ++){ es=({ struct Cyc_List_List* _temp2077=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2077->hd=( void*)({ struct
_tuple4* _temp2078=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp2078->f1= 0; _temp2078->f2= _temp1265; _temp2078;}); _temp2077->tl= es;
_temp2077;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
es, 0))->r)); goto _LL1084;}} _LL1142: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1273, _temp1277))->r));}
else{ if( _temp1271 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Struct_e: missing structdecl pointer", sizeof( unsigned char), 37u));}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp1271); struct _RegionHandle _temp2079= _new_region(); struct _RegionHandle*
rgn=& _temp2079; _push_region( rgn);{ struct Cyc_List_List* _temp2080=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1273,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); goto _LL2081; _LL2081: { struct Cyc_List_List*
_temp2082= 0; goto _LL2083; _LL2083:{ struct Cyc_List_List* _temp2084=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto
_LL2085; _LL2085: for( 0; _temp2084 != 0; _temp2084=(( struct Cyc_List_List*)
_check_null( _temp2084))->tl){ struct Cyc_List_List* _temp2086= _temp2080; goto
_LL2087; _LL2087: for( 0; _temp2086 != 0; _temp2086=(( struct Cyc_List_List*)
_check_null( _temp2086))->tl){ if((*(( struct _tuple11*)(( struct Cyc_List_List*)
_check_null( _temp2086))->hd)).f1 ==( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2084))->hd){ struct _tuple11 _temp2090; struct
Cyc_Absyn_Exp* _temp2091; struct Cyc_Absyn_Structfield* _temp2093; struct
_tuple11* _temp2088=( struct _tuple11*)(( struct Cyc_List_List*) _check_null(
_temp2086))->hd; _temp2090=* _temp2088; _LL2094: _temp2093= _temp2090.f1; goto
_LL2092; _LL2092: _temp2091= _temp2090.f2; goto _LL2089; _LL2089: { void*
_temp2095=( void*) _temp2093->type; goto _LL2096; _LL2096: Cyc_Toc_exp_to_c( nv,
_temp2091); if( Cyc_Toc_is_void_star( _temp2095)){( void*)( _temp2091->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp2091->r, 0), 0))->r));} _temp2082=({ struct Cyc_List_List* _temp2097=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2097->hd=(
void*)({ struct _tuple4* _temp2098=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp2098->f1= 0; _temp2098->f2= _temp2091; _temp2098;}); _temp2097->tl=
_temp2082; _temp2097;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2082), 0))->r));}}; _pop_region(& _temp2079);}} goto _LL1084; _LL1144: {
struct Cyc_List_List* fs;{ void* _temp2099= Cyc_Tcutil_compress( _temp1281);
struct Cyc_List_List* _temp2105; _LL2101: if(( unsigned int) _temp2099 > 4u?*((
int*) _temp2099) == Cyc_Absyn_AnonStructType: 0){ _LL2106: _temp2105=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2099)->f1; goto _LL2102;} else{ goto
_LL2103;} _LL2103: goto _LL2104; _LL2102: fs= _temp2105; goto _LL2100; _LL2104:
fs=(( struct Cyc_List_List*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp2107= Cyc_Absynpp_typ2string(
_temp1281); xprintf("anon struct has type %.*s", _temp2107.last_plus_one -
_temp2107.curr, _temp2107.curr);})); goto _LL2100; _LL2100:;}{ struct
_RegionHandle _temp2108= _new_region(); struct _RegionHandle* rgn=& _temp2108;
_push_region( rgn);{ struct Cyc_List_List* _temp2109=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1279, fs); goto _LL2110; _LL2110: for( 0; _temp2109 != 0; _temp2109=((
struct Cyc_List_List*) _check_null( _temp2109))->tl){ struct _tuple11 _temp2113;
struct Cyc_Absyn_Exp* _temp2114; struct Cyc_Absyn_Structfield* _temp2116; struct
_tuple11* _temp2111=( struct _tuple11*)(( struct Cyc_List_List*) _check_null(
_temp2109))->hd; _temp2113=* _temp2111; _LL2117: _temp2116= _temp2113.f1; goto
_LL2115; _LL2115: _temp2114= _temp2113.f2; goto _LL2112; _LL2112: { void*
_temp2118=( void*) _temp2116->type; goto _LL2119; _LL2119: Cyc_Toc_exp_to_c( nv,
_temp2114); if( Cyc_Toc_is_void_star( _temp2118)){( void*)( _temp2114->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp2114->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1279, 0))->r));}; _pop_region(& _temp2108);} goto _LL1084;} _LL1146: {
struct _tuple0* qv= _temp1283->name; if( _temp1285->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp2120= _temp1285->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1285->fields))->v;
goto _LL2121; _LL2121: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2120))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp2120))->hd)->typs
== 0){ tag_count ++;} _temp2120=(( struct Cyc_List_List*) _check_null( _temp2120))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1084;} _LL1148: { struct _tuple0* _temp2122= _temp1293->name; goto
_LL2123; _LL2123: { struct Cyc_List_List* _temp2124= _temp1293->typs; goto
_LL2125; _LL2125: { struct _tuple0* _temp2126= Cyc_Toc_temp_var(); goto _LL2127;
_LL2127: { struct Cyc_Absyn_Exp* _temp2128= Cyc_Absyn_var_exp( _temp2126, 0);
goto _LL2129; _LL2129: { void* _temp2130= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp2122, _tag_arr("_struct", sizeof( unsigned char), 8u))); goto _LL2131;
_LL2131: if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp; if( _temp1295->is_xtunion){
tag_exp= Cyc_Absyn_var_exp( _temp2122, 0);} else{ int tag_count= 0; struct Cyc_List_List*
_temp2132= _temp1295->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1295->fields))->v; goto _LL2133; _LL2133: while( Cyc_Absyn_qvar_cmp(
_temp2122,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2132))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp2132))->hd)->typs != 0){ tag_count ++;}
_temp2132=(( struct Cyc_List_List*) _check_null( _temp2132))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp2134= 0; goto _LL2135;
_LL2135: for( 0; _temp1297 != 0;( _temp1297=(( struct Cyc_List_List*)
_check_null( _temp1297))->tl, _temp2124=(( struct Cyc_List_List*) _check_null(
_temp2124))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1297))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2124))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp2134=({ struct Cyc_List_List* _temp2136=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2136[ 0]=({ struct Cyc_List_List
_temp2137; _temp2137.hd=( void*)({ struct _tuple4* _temp2138=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp2138[ 0]=({ struct _tuple4
_temp2139; _temp2139.f1= 0; _temp2139.f2= cur_e; _temp2139;}); _temp2138;});
_temp2137.tl= _temp2134; _temp2137;}); _temp2136;});} _temp2134=({ struct Cyc_List_List*
_temp2140=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2140[ 0]=({ struct Cyc_List_List _temp2141; _temp2141.hd=( void*)({ struct
_tuple4* _temp2142=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2142[ 0]=({ struct _tuple4 _temp2143; _temp2143.f1= 0; _temp2143.f2=
tag_exp; _temp2143;}); _temp2142;}); _temp2141.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2134); _temp2141;}); _temp2140;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2144=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2144->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp2122, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp2144;}), _temp2134, 0))->r));}} else{ struct
Cyc_List_List* _temp2146=({ struct Cyc_List_List* _temp2145=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2145->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2128, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp2122, 0), 0); _temp2145->tl= 0; _temp2145;}); goto _LL2147; _LL2147:{ int i=
1; for( 0; _temp1297 != 0;((( _temp1297=(( struct Cyc_List_List*) _check_null(
_temp1297))->tl, i ++)), _temp2124=(( struct Cyc_List_List*) _check_null(
_temp2124))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1297))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2124))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp2148= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp2128, Cyc_Toc_fieldname( i), 0), cur_e, 0);
goto _LL2149; _LL2149: _temp2146=({ struct Cyc_List_List* _temp2150=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2150->hd=( void*) _temp2148;
_temp2150->tl= _temp2146; _temp2150;});}}}{ struct Cyc_Absyn_Stmt* _temp2151=
Cyc_Absyn_exp_stmt( _temp2128, 0); goto _LL2152; _LL2152: { struct Cyc_Absyn_Stmt*
_temp2154= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2153=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2153->hd=( void*) _temp2151;
_temp2153->tl= _temp2146; _temp2153;})), 0); goto _LL2155; _LL2155:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp2126,
_temp2130, 0, _temp2154, 0), 0))->r));}}} goto _LL1084;}}}}} _LL1150: goto
_LL1084; _LL1152: { void* t_c= Cyc_Toc_typ_to_c( _temp1309); if( _temp1311 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1311); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1309, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL1084;} _LL1154: Cyc_Toc_stmt_to_c(
nv, _temp1313); goto _LL1084; _LL1156:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem", sizeof( unsigned char), 14u)); goto _LL1084; _LL1158:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u)); goto _LL1084; _LL1160:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u)); goto _LL1084;
_LL1162:(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("fill",
sizeof( unsigned char), 5u)); goto _LL1084; _LL1084:;}} static struct Cyc_Absyn_Stmt*
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
_temp2156=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp2192; struct Cyc_Absyn_Vardecl
_temp2194; struct Cyc_List_List* _temp2195; struct Cyc_Core_Opt* _temp2197;
struct Cyc_Absyn_Exp* _temp2199; void* _temp2201; struct Cyc_Absyn_Tqual
_temp2203; struct _tuple0* _temp2205; void* _temp2207; struct Cyc_Absyn_Vardecl*
_temp2209; int _temp2211; void* _temp2213; unsigned char _temp2215; struct
_tagged_arr _temp2217; struct Cyc_Absyn_Enumfield* _temp2219; struct Cyc_Absyn_Enumdecl*
_temp2221; struct Cyc_List_List* _temp2223; struct Cyc_List_List* _temp2225;
struct Cyc_Absyn_Tunionfield* _temp2227; struct Cyc_Absyn_Tuniondecl* _temp2229;
struct Cyc_Absyn_Pat* _temp2232; struct Cyc_Absyn_Pat _temp2234; struct Cyc_Position_Segment*
_temp2235; struct Cyc_Core_Opt* _temp2237; void* _temp2239; struct Cyc_List_List*
_temp2241; struct Cyc_List_List* _temp2243; struct Cyc_Absyn_Tunionfield*
_temp2245; struct Cyc_Absyn_Tuniondecl* _temp2247; struct Cyc_List_List*
_temp2249; struct Cyc_List_List* _temp2251; struct Cyc_Absyn_Tunionfield*
_temp2253; struct Cyc_Absyn_Tuniondecl* _temp2255; struct Cyc_List_List*
_temp2257; struct Cyc_List_List* _temp2259; struct Cyc_List_List* _temp2261;
struct Cyc_Core_Opt* _temp2263; struct Cyc_Absyn_Structdecl* _temp2265; struct
Cyc_Absyn_Pat* _temp2267; struct _tuple0* _temp2269; struct Cyc_List_List*
_temp2271; struct Cyc_List_List* _temp2273; struct _tuple0* _temp2275; struct
Cyc_List_List* _temp2277; struct Cyc_List_List* _temp2279; struct _tuple0*
_temp2281; _LL2158: if(( unsigned int) _temp2156 > 2u?*(( int*) _temp2156) ==
Cyc_Absyn_Var_p: 0){ _LL2193: _temp2192=(( struct Cyc_Absyn_Var_p_struct*)
_temp2156)->f1; _temp2194=* _temp2192; _LL2208: _temp2207=( void*) _temp2194.sc;
goto _LL2206; _LL2206: _temp2205= _temp2194.name; goto _LL2204; _LL2204:
_temp2203= _temp2194.tq; goto _LL2202; _LL2202: _temp2201=( void*) _temp2194.type;
goto _LL2200; _LL2200: _temp2199= _temp2194.initializer; goto _LL2198; _LL2198:
_temp2197= _temp2194.rgn; goto _LL2196; _LL2196: _temp2195= _temp2194.attributes;
goto _LL2159;} else{ goto _LL2160;} _LL2160: if( _temp2156 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2161;} else{ goto _LL2162;} _LL2162: if(( unsigned int) _temp2156 > 2u?*((
int*) _temp2156) == Cyc_Absyn_Reference_p: 0){ _LL2210: _temp2209=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2156)->f1; goto _LL2163;} else{ goto _LL2164;} _LL2164: if( _temp2156 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2165;} else{ goto _LL2166;} _LL2166: if((
unsigned int) _temp2156 > 2u?*(( int*) _temp2156) == Cyc_Absyn_Int_p: 0){
_LL2214: _temp2213=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2156)->f1;
goto _LL2212; _LL2212: _temp2211=(( struct Cyc_Absyn_Int_p_struct*) _temp2156)->f2;
goto _LL2167;} else{ goto _LL2168;} _LL2168: if(( unsigned int) _temp2156 > 2u?*((
int*) _temp2156) == Cyc_Absyn_Char_p: 0){ _LL2216: _temp2215=(( struct Cyc_Absyn_Char_p_struct*)
_temp2156)->f1; goto _LL2169;} else{ goto _LL2170;} _LL2170: if(( unsigned int)
_temp2156 > 2u?*(( int*) _temp2156) == Cyc_Absyn_Float_p: 0){ _LL2218: _temp2217=((
struct Cyc_Absyn_Float_p_struct*) _temp2156)->f1; goto _LL2171;} else{ goto
_LL2172;} _LL2172: if(( unsigned int) _temp2156 > 2u?*(( int*) _temp2156) == Cyc_Absyn_Enum_p:
0){ _LL2222: _temp2221=(( struct Cyc_Absyn_Enum_p_struct*) _temp2156)->f1; goto
_LL2220; _LL2220: _temp2219=(( struct Cyc_Absyn_Enum_p_struct*) _temp2156)->f2;
goto _LL2173;} else{ goto _LL2174;} _LL2174: if(( unsigned int) _temp2156 > 2u?*((
int*) _temp2156) == Cyc_Absyn_Tunion_p: 0){ _LL2230: _temp2229=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2156)->f1; goto _LL2228; _LL2228: _temp2227=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2156)->f2; goto _LL2226; _LL2226: _temp2225=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2156)->f3; goto _LL2224; _LL2224: _temp2223=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2156)->f4; if( _temp2223 == 0){ goto _LL2175;} else{ goto _LL2176;}} else{
goto _LL2176;} _LL2176: if(( unsigned int) _temp2156 > 2u?*(( int*) _temp2156)
== Cyc_Absyn_Pointer_p: 0){ _LL2233: _temp2232=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2156)->f1; _temp2234=* _temp2232; _LL2240: _temp2239=( void*) _temp2234.r;
if(( unsigned int) _temp2239 > 2u?*(( int*) _temp2239) == Cyc_Absyn_Tunion_p: 0){
_LL2248: _temp2247=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2239)->f1; goto
_LL2246; _LL2246: _temp2245=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2239)->f2;
goto _LL2244; _LL2244: _temp2243=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2239)->f3;
goto _LL2242; _LL2242: _temp2241=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2239)->f4;
goto _LL2238;} else{ goto _LL2178;} _LL2238: _temp2237= _temp2234.topt; goto
_LL2236; _LL2236: _temp2235= _temp2234.loc; goto _LL2231;} else{ goto _LL2178;}
_LL2231: if( _temp2241 != 0){ goto _LL2177;} else{ goto _LL2178;} _LL2178: if((
unsigned int) _temp2156 > 2u?*(( int*) _temp2156) == Cyc_Absyn_Tunion_p: 0){
_LL2256: _temp2255=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2156)->f1; goto
_LL2254; _LL2254: _temp2253=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2156)->f2;
goto _LL2252; _LL2252: _temp2251=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2156)->f3;
goto _LL2250; _LL2250: _temp2249=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2156)->f4;
goto _LL2179;} else{ goto _LL2180;} _LL2180: if(( unsigned int) _temp2156 > 2u?*((
int*) _temp2156) == Cyc_Absyn_Tuple_p: 0){ _LL2258: _temp2257=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2156)->f1; goto _LL2181;} else{ goto _LL2182;} _LL2182: if(( unsigned int)
_temp2156 > 2u?*(( int*) _temp2156) == Cyc_Absyn_Struct_p: 0){ _LL2266:
_temp2265=(( struct Cyc_Absyn_Struct_p_struct*) _temp2156)->f1; goto _LL2264;
_LL2264: _temp2263=(( struct Cyc_Absyn_Struct_p_struct*) _temp2156)->f2; goto
_LL2262; _LL2262: _temp2261=(( struct Cyc_Absyn_Struct_p_struct*) _temp2156)->f3;
goto _LL2260; _LL2260: _temp2259=(( struct Cyc_Absyn_Struct_p_struct*) _temp2156)->f4;
goto _LL2183;} else{ goto _LL2184;} _LL2184: if(( unsigned int) _temp2156 > 2u?*((
int*) _temp2156) == Cyc_Absyn_Pointer_p: 0){ _LL2268: _temp2267=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2156)->f1; goto _LL2185;} else{ goto _LL2186;} _LL2186: if(( unsigned int)
_temp2156 > 2u?*(( int*) _temp2156) == Cyc_Absyn_UnknownId_p: 0){ _LL2270:
_temp2269=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2156)->f1; goto _LL2187;}
else{ goto _LL2188;} _LL2188: if(( unsigned int) _temp2156 > 2u?*(( int*)
_temp2156) == Cyc_Absyn_UnknownCall_p: 0){ _LL2276: _temp2275=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2156)->f1; goto _LL2274; _LL2274: _temp2273=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2156)->f2; goto _LL2272; _LL2272: _temp2271=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2156)->f3; goto _LL2189;} else{ goto _LL2190;} _LL2190: if(( unsigned int)
_temp2156 > 2u?*(( int*) _temp2156) == Cyc_Absyn_UnknownFields_p: 0){ _LL2282:
_temp2281=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2156)->f1; goto
_LL2280; _LL2280: _temp2279=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2156)->f2; goto _LL2278; _LL2278: _temp2277=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2156)->f3; goto _LL2191;} else{ goto _LL2157;} _LL2159: nv= Cyc_Toc_add_varmap(
nv, _temp2205, r); goto _LL2161; _LL2161: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2157; _LL2163: { struct _tuple0* _temp2283= Cyc_Toc_temp_var(); goto
_LL2284; _LL2284: decls=({ struct Cyc_List_List* _temp2285=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2285->hd=( void*)({
struct _tuple13* _temp2286=( struct _tuple13*) _region_malloc( rgn, sizeof(
struct _tuple13)); _temp2286->f1= _temp2283; _temp2286->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2286;}); _temp2285->tl= decls;
_temp2285;}); nv= Cyc_Toc_add_varmap( nv, _temp2209->name, Cyc_Absyn_var_exp(
_temp2283, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2283, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2157;} _LL2165: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2157; _LL2167: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2213, _temp2211, 0), succ_lab, fail_lab); goto _LL2157;
_LL2169: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2215, 0),
succ_lab, fail_lab); goto _LL2157; _LL2171: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2217, 0), succ_lab, fail_lab); goto _LL2157; _LL2173: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2287=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2287[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2288; _temp2288.tag= Cyc_Absyn_Enum_e;
_temp2288.f1= _temp2219->name; _temp2288.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2221; _temp2288.f3=( struct Cyc_Absyn_Enumfield*) _temp2219; _temp2288;});
_temp2287;}), 0), succ_lab, fail_lab); goto _LL2157; _LL2175: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2227->name, 0); if( ! _temp2229->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2157;} _LL2177: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2241);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2245->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp2289=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2245->typs); goto _LL2290;
_LL2290: { struct Cyc_List_List* _temp2291=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2241); goto
_LL2292; _LL2292: for( 0; _temp2291 != 0;((( _temp2291=(( struct Cyc_List_List*)
_check_null( _temp2291))->tl, _temp2289=(( struct Cyc_List_List*) _check_null(
_temp2289))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp2293=( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp2291))->hd; goto _LL2294; _LL2294: {
void* _temp2295=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2289))->hd)).f2; goto _LL2296; _LL2296: { struct _tuple0* _temp2297= Cyc_Toc_temp_var();
goto _LL2298; _LL2298: { void* _temp2299=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2293->topt))->v; goto _LL2300; _LL2300: { void* _temp2301= Cyc_Toc_typ_to_c(
_temp2299); goto _LL2302; _LL2302: { struct _tagged_arr* _temp2303= Cyc_Toc_fresh_label();
goto _LL2304; _LL2304: { struct Cyc_Absyn_Exp* _temp2305= Cyc_Absyn_structarrow_exp(
rcast, Cyc_Toc_fieldname( cnt), 0); goto _LL2306; _LL2306: if( Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c( _temp2295))){ _temp2305= Cyc_Absyn_cast_exp( _temp2301,
_temp2305, 0);} decls=({ struct Cyc_List_List* _temp2307=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2307->hd=( void*)({
struct _tuple13* _temp2308=( struct _tuple13*) _region_malloc( rgn, sizeof(
struct _tuple13)); _temp2308->f1= _temp2297; _temp2308->f2= _temp2301; _temp2308;});
_temp2307->tl= decls; _temp2307;});{ struct _tuple12 _temp2309= Cyc_Toc_xlate_pat(
nv, rgn, _temp2299, Cyc_Absyn_var_exp( _temp2297, 0), _temp2305, _temp2293,
succ_lab, fail_lab, decls); goto _LL2310; _LL2310: nv= _temp2309.f1; decls=
_temp2309.f2;{ struct Cyc_Absyn_Stmt* _temp2311= _temp2309.f3; goto _LL2312;
_LL2312: { struct Cyc_Absyn_Stmt* _temp2313= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2297, 0), _temp2305, 0); goto _LL2314; _LL2314: { struct Cyc_Absyn_Stmt*
_temp2315= Cyc_Absyn_seq_stmt( _temp2313, _temp2311, 0); goto _LL2316; _LL2316:
ss=({ struct Cyc_List_List* _temp2317=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2317->hd=( void*) Cyc_Absyn_label_stmt(
_temp2303, _temp2315, 0); _temp2317->tl= ss; _temp2317;}); succ_lab= _temp2303;}}}}}}}}}}}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2247->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2245->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2318=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2247->fields))->v; goto _LL2319; _LL2319: for( 0; _temp2318 !=
0; _temp2318=(( struct Cyc_List_List*) _check_null( _temp2318))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2320=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2318))->hd; goto _LL2321; _LL2321: if( _temp2320->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2245->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2157;}}} _LL2179: _temp2257=
_temp2249; goto _LL2181; _LL2181: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2257);{ struct Cyc_List_List*
_temp2322=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2257); goto _LL2323; _LL2323: for( 0; _temp2322 !=
0;( _temp2322=(( struct Cyc_List_List*) _check_null( _temp2322))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2324=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2322))->hd; goto _LL2325; _LL2325: { struct _tuple0* _temp2326=
Cyc_Toc_temp_var(); goto _LL2327; _LL2327: { void* _temp2328=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2324->topt))->v; goto _LL2329; _LL2329: {
struct _tagged_arr* _temp2330= Cyc_Toc_fresh_label(); goto _LL2331; _LL2331:
decls=({ struct Cyc_List_List* _temp2332=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2332->hd=( void*)({ struct _tuple13*
_temp2333=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2333->f1= _temp2326; _temp2333->f2= Cyc_Toc_typ_to_c( _temp2328); _temp2333;});
_temp2332->tl= decls; _temp2332;});{ struct _tuple12 _temp2334= Cyc_Toc_xlate_pat(
nv, rgn, _temp2328, Cyc_Absyn_var_exp( _temp2326, 0), Cyc_Absyn_structmember_exp(
path, Cyc_Toc_fieldname( cnt), 0), _temp2324, succ_lab, fail_lab, decls); goto
_LL2335; _LL2335: nv= _temp2334.f1; decls= _temp2334.f2;{ struct Cyc_Absyn_Stmt*
_temp2336= _temp2334.f3; goto _LL2337; _LL2337: { struct Cyc_Absyn_Stmt*
_temp2338= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2326, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2336, 0); goto _LL2339; _LL2339: ss=({ struct Cyc_List_List* _temp2340=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2340->hd=(
void*) Cyc_Absyn_label_stmt( _temp2330, _temp2338, 0); _temp2340->tl= ss;
_temp2340;}); succ_lab= _temp2330;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2157;} _LL2183: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2341=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2259); goto _LL2342; _LL2342: for( 0; _temp2341 !=
0; _temp2341=(( struct Cyc_List_List*) _check_null( _temp2341))->tl){ struct
_tuple14* _temp2343=( struct _tuple14*)(( struct Cyc_List_List*) _check_null(
_temp2341))->hd; goto _LL2344; _LL2344: { struct Cyc_Absyn_Pat* _temp2345=(*
_temp2343).f2; goto _LL2346; _LL2346: { struct _tagged_arr* f;{ void* _temp2347=(
void*)(( struct Cyc_List_List*) _check_null((* _temp2343).f1))->hd; struct
_tagged_arr* _temp2353; _LL2349: if(*(( int*) _temp2347) == Cyc_Absyn_FieldName){
_LL2354: _temp2353=(( struct Cyc_Absyn_FieldName_struct*) _temp2347)->f1; goto
_LL2350;} else{ goto _LL2351;} _LL2351: goto _LL2352; _LL2350: f= _temp2353;
goto _LL2348; _LL2352:( int) _throw(( void*) Cyc_Toc_Match_error); _LL2348:;}{
struct _tuple0* _temp2355= Cyc_Toc_temp_var(); goto _LL2356; _LL2356: { void*
_temp2357=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2345->topt))->v;
goto _LL2358; _LL2358: { void* _temp2359= Cyc_Toc_typ_to_c( _temp2357); goto
_LL2360; _LL2360: { struct _tagged_arr* _temp2361= Cyc_Toc_fresh_label(); goto
_LL2362; _LL2362: decls=({ struct Cyc_List_List* _temp2363=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2363->hd=( void*)({
struct _tuple13* _temp2364=( struct _tuple13*) _region_malloc( rgn, sizeof(
struct _tuple13)); _temp2364->f1= _temp2355; _temp2364->f2= _temp2359; _temp2364;});
_temp2363->tl= decls; _temp2363;});{ struct _tuple12 _temp2365= Cyc_Toc_xlate_pat(
nv, rgn, _temp2357, Cyc_Absyn_var_exp( _temp2355, 0), Cyc_Absyn_structmember_exp(
path, f, 0), _temp2345, succ_lab, fail_lab, decls); goto _LL2366; _LL2366: nv=
_temp2365.f1; decls= _temp2365.f2;{ struct Cyc_Absyn_Exp* _temp2367= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2368; _LL2368: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2265->fields))->v, f)))->type)){ _temp2367= Cyc_Absyn_cast_exp(
_temp2359, _temp2367, 0);}{ struct Cyc_Absyn_Stmt* _temp2369= _temp2365.f3; goto
_LL2370; _LL2370: { struct Cyc_Absyn_Stmt* _temp2371= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2355, 0), _temp2367, 0), _temp2369, 0); goto _LL2372;
_LL2372: ss=({ struct Cyc_List_List* _temp2373=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2373->hd=( void*) Cyc_Absyn_label_stmt(
_temp2361, _temp2371, 0); _temp2373->tl= ss; _temp2373;}); succ_lab= _temp2361;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2157;} _LL2185: { struct _tuple0*
_temp2374= Cyc_Toc_temp_var(); goto _LL2375; _LL2375: { void* _temp2376=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2267->topt))->v; goto _LL2377; _LL2377:
decls=({ struct Cyc_List_List* _temp2378=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2378->hd=( void*)({ struct _tuple13*
_temp2379=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2379->f1= _temp2374; _temp2379->f2= Cyc_Toc_typ_to_c( _temp2376); _temp2379;});
_temp2378->tl= decls; _temp2378;});{ struct _tuple12 _temp2380= Cyc_Toc_xlate_pat(
nv, rgn, _temp2376, Cyc_Absyn_var_exp( _temp2374, 0), Cyc_Absyn_deref_exp( path,
0), _temp2267, succ_lab, fail_lab, decls); goto _LL2381; _LL2381: nv= _temp2380.f1;
decls= _temp2380.f2;{ struct Cyc_Absyn_Stmt* _temp2382= _temp2380.f3; goto
_LL2383; _LL2383: { struct Cyc_Absyn_Stmt* _temp2384= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2374, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2382, 0);
goto _LL2385; _LL2385: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2384, 0);} else{ s= _temp2384;} goto _LL2157;}}}}} _LL2187: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid",
sizeof( unsigned char), 10u)); goto _LL2157; _LL2189: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall", sizeof(
unsigned char), 12u)); goto _LL2157; _LL2191: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields", sizeof(
unsigned char), 14u)); goto _LL2157; _LL2157:;} return({ struct _tuple12
_temp2386; _temp2386.f1= nv; _temp2386.f2= decls; _temp2386.f3= s; _temp2386;});}
struct _tuple15{ struct _tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple15* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple15* _temp2387=(
struct _tuple15*) _region_malloc( r, sizeof( struct _tuple15)); _temp2387->f1=
Cyc_Toc_fresh_label(); _temp2387->f2= Cyc_Toc_fresh_label(); _temp2387->f3= sc;
_temp2387;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp2388=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2389; _LL2389: { int leave_as_switch;{ void*
_temp2390= Cyc_Tcutil_compress( _temp2388); void* _temp2396; void* _temp2398;
_LL2392: if(( unsigned int) _temp2390 > 4u?*(( int*) _temp2390) == Cyc_Absyn_IntType:
0){ _LL2399: _temp2398=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2390)->f1;
goto _LL2397; _LL2397: _temp2396=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2390)->f2; goto _LL2393;} else{ goto _LL2394;} _LL2394: goto _LL2395;
_LL2393: leave_as_switch= 1; goto _LL2391; _LL2395: leave_as_switch= 0; goto
_LL2391; _LL2391:;}{ struct Cyc_List_List* _temp2400= scs; goto _LL2401; _LL2401:
for( 0; _temp2400 != 0; _temp2400=(( struct Cyc_List_List*) _check_null(
_temp2400))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2400))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2400))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2402= scs; goto _LL2403;
_LL2403: for( 0; _temp2402 != 0; _temp2402=(( struct Cyc_List_List*) _check_null(
_temp2402))->tl){ struct Cyc_Absyn_Stmt* _temp2404=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2402))->hd)->body; goto _LL2405;
_LL2405:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2402))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2404, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2404);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct _tagged_arr*
end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2406= _new_region();
struct _RegionHandle* rgn=& _temp2406; _push_region( rgn);{ struct Cyc_List_List*
lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple15*(* f)(
struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*), struct _RegionHandle*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label, rgn,
scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0; struct
Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2407= lscs; goto _LL2408;
_LL2408: for( 0; _temp2407 != 0; _temp2407=(( struct Cyc_List_List*) _check_null(
_temp2407))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( _temp2407))->hd)).f3; struct _tagged_arr*
fail_lab=(( struct Cyc_List_List*) _check_null( _temp2407))->tl == 0? end_l:(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp2407))->tl))->hd)).f1; struct _tagged_arr* succ_lab=(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2407))->hd)).f2;
if( sc->where_clause == 0){ struct _tuple12 _temp2409= Cyc_Toc_xlate_pat( nv,
rgn, _temp2388, r, path, sc->pattern, succ_lab, fail_lab, decls); goto _LL2410;
_LL2410: nvs=({ struct Cyc_List_List* _temp2411=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2411->hd=( void*)
_temp2409.f1; _temp2411->tl= nvs; _temp2411;}); decls= _temp2409.f2; test_stmts=({
struct Cyc_List_List* _temp2412=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2412->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2407))->hd)).f1,
_temp2409.f3, 0); _temp2412->tl= test_stmts; _temp2412;});} else{ struct Cyc_Absyn_Exp*
_temp2413=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2414;
_LL2414: { struct _tagged_arr* _temp2415= Cyc_Toc_fresh_label(); goto _LL2416;
_LL2416: { struct _tuple12 _temp2417= Cyc_Toc_xlate_pat( nv, rgn, _temp2388, r,
path, sc->pattern, _temp2415, fail_lab, decls); goto _LL2418; _LL2418: Cyc_Toc_exp_to_c(
_temp2417.f1, _temp2413); nvs=({ struct Cyc_List_List* _temp2419=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2419->hd=( void*)
_temp2417.f1; _temp2419->tl= nvs; _temp2419;}); decls= _temp2417.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2413, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2415, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2420=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2420->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2407))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2417.f3, s3, 0), 0); _temp2420->tl= test_stmts;
_temp2420;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2421= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2422; _LL2422: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2423=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2424; _LL2424: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2423, end_l,(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2425=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2426; _LL2426: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2425, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2421, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple13 _temp2429;
void* _temp2430; struct _tuple0* _temp2432; struct _tuple13* _temp2427=( struct
_tuple13*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2429=*
_temp2427; _LL2433: _temp2432= _temp2429.f1; goto _LL2431; _LL2431: _temp2430=
_temp2429.f2; goto _LL2428; _LL2428: res= Cyc_Absyn_declare_stmt( _temp2432,
_temp2430, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region(& _temp2406);}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp2434=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2434[ 0]=({ struct Cyc_List_List
_temp2435; _temp2435.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp2435.tl= 0; _temp2435;}); _temp2434;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2436=( void*) s->r; struct Cyc_Absyn_Exp*
_temp2478; struct Cyc_Absyn_Stmt* _temp2480; struct Cyc_Absyn_Stmt* _temp2482;
struct Cyc_Absyn_Exp* _temp2484; struct Cyc_Absyn_Stmt* _temp2486; struct Cyc_Absyn_Stmt*
_temp2488; struct Cyc_Absyn_Exp* _temp2490; struct Cyc_Absyn_Stmt* _temp2492;
struct _tuple2 _temp2494; struct Cyc_Absyn_Stmt* _temp2496; struct Cyc_Absyn_Exp*
_temp2498; struct Cyc_Absyn_Stmt* _temp2500; struct Cyc_Absyn_Stmt* _temp2502;
struct Cyc_Absyn_Stmt* _temp2504; struct _tagged_arr* _temp2506; struct Cyc_Absyn_Stmt*
_temp2508; struct _tuple2 _temp2510; struct Cyc_Absyn_Stmt* _temp2512; struct
Cyc_Absyn_Exp* _temp2514; struct _tuple2 _temp2516; struct Cyc_Absyn_Stmt*
_temp2518; struct Cyc_Absyn_Exp* _temp2520; struct Cyc_Absyn_Exp* _temp2522;
struct Cyc_List_List* _temp2524; struct Cyc_Absyn_Exp* _temp2526; struct Cyc_Absyn_Switch_clause**
_temp2528; struct Cyc_List_List* _temp2530; struct Cyc_Absyn_Stmt* _temp2532;
struct Cyc_Absyn_Decl* _temp2534; struct Cyc_Absyn_Stmt* _temp2536; struct
_tagged_arr* _temp2538; struct _tuple2 _temp2540; struct Cyc_Absyn_Stmt*
_temp2542; struct Cyc_Absyn_Exp* _temp2544; struct Cyc_Absyn_Stmt* _temp2546;
struct Cyc_List_List* _temp2548; struct Cyc_Absyn_Stmt* _temp2550; struct Cyc_Absyn_Stmt*
_temp2552; struct Cyc_Absyn_Vardecl* _temp2554; struct Cyc_Absyn_Tvar* _temp2556;
struct Cyc_List_List* _temp2558; struct Cyc_Absyn_Exp* _temp2560; struct Cyc_Absyn_Stmt*
_temp2562; struct Cyc_Absyn_Stmt* _temp2564; _LL2438: if( _temp2436 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2439;} else{ goto _LL2440;} _LL2440: if((
unsigned int) _temp2436 > 1u?*(( int*) _temp2436) == Cyc_Absyn_Exp_s: 0){
_LL2479: _temp2478=(( struct Cyc_Absyn_Exp_s_struct*) _temp2436)->f1; goto
_LL2441;} else{ goto _LL2442;} _LL2442: if(( unsigned int) _temp2436 > 1u?*((
int*) _temp2436) == Cyc_Absyn_Seq_s: 0){ _LL2483: _temp2482=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2436)->f1; goto _LL2481; _LL2481: _temp2480=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2436)->f2; goto _LL2443;} else{ goto _LL2444;} _LL2444: if(( unsigned int)
_temp2436 > 1u?*(( int*) _temp2436) == Cyc_Absyn_Return_s: 0){ _LL2485:
_temp2484=(( struct Cyc_Absyn_Return_s_struct*) _temp2436)->f1; goto _LL2445;}
else{ goto _LL2446;} _LL2446: if(( unsigned int) _temp2436 > 1u?*(( int*)
_temp2436) == Cyc_Absyn_IfThenElse_s: 0){ _LL2491: _temp2490=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2436)->f1; goto _LL2489; _LL2489: _temp2488=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2436)->f2; goto _LL2487; _LL2487: _temp2486=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2436)->f3; goto _LL2447;} else{ goto _LL2448;} _LL2448: if(( unsigned int)
_temp2436 > 1u?*(( int*) _temp2436) == Cyc_Absyn_While_s: 0){ _LL2495: _temp2494=((
struct Cyc_Absyn_While_s_struct*) _temp2436)->f1; _LL2499: _temp2498= _temp2494.f1;
goto _LL2497; _LL2497: _temp2496= _temp2494.f2; goto _LL2493; _LL2493: _temp2492=((
struct Cyc_Absyn_While_s_struct*) _temp2436)->f2; goto _LL2449;} else{ goto
_LL2450;} _LL2450: if(( unsigned int) _temp2436 > 1u?*(( int*) _temp2436) == Cyc_Absyn_Break_s:
0){ _LL2501: _temp2500=(( struct Cyc_Absyn_Break_s_struct*) _temp2436)->f1; goto
_LL2451;} else{ goto _LL2452;} _LL2452: if(( unsigned int) _temp2436 > 1u?*((
int*) _temp2436) == Cyc_Absyn_Continue_s: 0){ _LL2503: _temp2502=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2436)->f1; goto _LL2453;} else{ goto _LL2454;} _LL2454: if(( unsigned int)
_temp2436 > 1u?*(( int*) _temp2436) == Cyc_Absyn_Goto_s: 0){ _LL2507: _temp2506=((
struct Cyc_Absyn_Goto_s_struct*) _temp2436)->f1; goto _LL2505; _LL2505:
_temp2504=(( struct Cyc_Absyn_Goto_s_struct*) _temp2436)->f2; goto _LL2455;}
else{ goto _LL2456;} _LL2456: if(( unsigned int) _temp2436 > 1u?*(( int*)
_temp2436) == Cyc_Absyn_For_s: 0){ _LL2523: _temp2522=(( struct Cyc_Absyn_For_s_struct*)
_temp2436)->f1; goto _LL2517; _LL2517: _temp2516=(( struct Cyc_Absyn_For_s_struct*)
_temp2436)->f2; _LL2521: _temp2520= _temp2516.f1; goto _LL2519; _LL2519:
_temp2518= _temp2516.f2; goto _LL2511; _LL2511: _temp2510=(( struct Cyc_Absyn_For_s_struct*)
_temp2436)->f3; _LL2515: _temp2514= _temp2510.f1; goto _LL2513; _LL2513:
_temp2512= _temp2510.f2; goto _LL2509; _LL2509: _temp2508=(( struct Cyc_Absyn_For_s_struct*)
_temp2436)->f4; goto _LL2457;} else{ goto _LL2458;} _LL2458: if(( unsigned int)
_temp2436 > 1u?*(( int*) _temp2436) == Cyc_Absyn_Switch_s: 0){ _LL2527:
_temp2526=(( struct Cyc_Absyn_Switch_s_struct*) _temp2436)->f1; goto _LL2525;
_LL2525: _temp2524=(( struct Cyc_Absyn_Switch_s_struct*) _temp2436)->f2; goto
_LL2459;} else{ goto _LL2460;} _LL2460: if(( unsigned int) _temp2436 > 1u?*((
int*) _temp2436) == Cyc_Absyn_Fallthru_s: 0){ _LL2531: _temp2530=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2436)->f1; goto _LL2529; _LL2529: _temp2528=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2436)->f2; goto _LL2461;} else{ goto _LL2462;} _LL2462: if(( unsigned int)
_temp2436 > 1u?*(( int*) _temp2436) == Cyc_Absyn_Decl_s: 0){ _LL2535: _temp2534=((
struct Cyc_Absyn_Decl_s_struct*) _temp2436)->f1; goto _LL2533; _LL2533:
_temp2532=(( struct Cyc_Absyn_Decl_s_struct*) _temp2436)->f2; goto _LL2463;}
else{ goto _LL2464;} _LL2464: if(( unsigned int) _temp2436 > 1u?*(( int*)
_temp2436) == Cyc_Absyn_Label_s: 0){ _LL2539: _temp2538=(( struct Cyc_Absyn_Label_s_struct*)
_temp2436)->f1; goto _LL2537; _LL2537: _temp2536=(( struct Cyc_Absyn_Label_s_struct*)
_temp2436)->f2; goto _LL2465;} else{ goto _LL2466;} _LL2466: if(( unsigned int)
_temp2436 > 1u?*(( int*) _temp2436) == Cyc_Absyn_Do_s: 0){ _LL2547: _temp2546=((
struct Cyc_Absyn_Do_s_struct*) _temp2436)->f1; goto _LL2541; _LL2541: _temp2540=((
struct Cyc_Absyn_Do_s_struct*) _temp2436)->f2; _LL2545: _temp2544= _temp2540.f1;
goto _LL2543; _LL2543: _temp2542= _temp2540.f2; goto _LL2467;} else{ goto
_LL2468;} _LL2468: if(( unsigned int) _temp2436 > 1u?*(( int*) _temp2436) == Cyc_Absyn_TryCatch_s:
0){ _LL2551: _temp2550=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2436)->f1;
goto _LL2549; _LL2549: _temp2548=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2436)->f2; goto _LL2469;} else{ goto _LL2470;} _LL2470: if(( unsigned int)
_temp2436 > 1u?*(( int*) _temp2436) == Cyc_Absyn_Region_s: 0){ _LL2557:
_temp2556=(( struct Cyc_Absyn_Region_s_struct*) _temp2436)->f1; goto _LL2555;
_LL2555: _temp2554=(( struct Cyc_Absyn_Region_s_struct*) _temp2436)->f2; goto
_LL2553; _LL2553: _temp2552=(( struct Cyc_Absyn_Region_s_struct*) _temp2436)->f3;
goto _LL2471;} else{ goto _LL2472;} _LL2472: if(( unsigned int) _temp2436 > 1u?*((
int*) _temp2436) == Cyc_Absyn_SwitchC_s: 0){ _LL2561: _temp2560=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2436)->f1; goto _LL2559; _LL2559: _temp2558=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2436)->f2; goto _LL2473;} else{ goto _LL2474;} _LL2474: if(( unsigned int)
_temp2436 > 1u?*(( int*) _temp2436) == Cyc_Absyn_Cut_s: 0){ _LL2563: _temp2562=((
struct Cyc_Absyn_Cut_s_struct*) _temp2436)->f1; goto _LL2475;} else{ goto
_LL2476;} _LL2476: if(( unsigned int) _temp2436 > 1u?*(( int*) _temp2436) == Cyc_Absyn_Splice_s:
0){ _LL2565: _temp2564=(( struct Cyc_Absyn_Splice_s_struct*) _temp2436)->f1;
goto _LL2477;} else{ goto _LL2437;} _LL2439: return; _LL2441: Cyc_Toc_exp_to_c(
nv, _temp2478); return; _LL2443: Cyc_Toc_stmt_to_c( nv, _temp2482); s= _temp2480;
continue; _LL2445: { struct Cyc_Core_Opt* topt= 0; if( _temp2484 != 0){ topt=({
struct Cyc_Core_Opt* _temp2566=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2566->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2484))->topt))->v);
_temp2566;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2484));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2567=
Cyc_Toc_temp_var(); goto _LL2568; _LL2568: { struct Cyc_Absyn_Stmt* _temp2569=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2567, 0),
0); goto _LL2570; _LL2570:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2567,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2484,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2569, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2447: Cyc_Toc_exp_to_c(
nv, _temp2490); Cyc_Toc_stmt_to_c( nv, _temp2488); s= _temp2486; continue;
_LL2449: Cyc_Toc_exp_to_c( nv, _temp2498); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2492); return; _LL2451: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2500 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2500))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2453: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -(( struct Cyc_Absyn_Stmt*) _check_null( _temp2502))->try_depth, s);
return; _LL2455: Cyc_Toc_do_npop_before( s->try_depth -(( struct Cyc_Absyn_Stmt*)
_check_null( _temp2504))->try_depth, s); return; _LL2457: Cyc_Toc_exp_to_c( nv,
_temp2522); Cyc_Toc_exp_to_c( nv, _temp2520); Cyc_Toc_exp_to_c( nv, _temp2514);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2508); return; _LL2459: Cyc_Toc_xlate_switch(
nv, s, _temp2526, _temp2524); return; _LL2461: if( nv->fallthru_info == 0){( int)
_throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("fallthru in unexpected place",
sizeof( unsigned char), 29u)));}{ struct _tuple6 _temp2573; struct Cyc_Dict_Dict*
_temp2574; struct Cyc_List_List* _temp2576; struct _tagged_arr* _temp2578;
struct _tuple6* _temp2571=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp2573=* _temp2571; _LL2579: _temp2578= _temp2573.f1;
goto _LL2577; _LL2577: _temp2576= _temp2573.f2; goto _LL2575; _LL2575: _temp2574=
_temp2573.f3; goto _LL2572; _LL2572: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp2578, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp2528)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2580=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2576); goto _LL2581; _LL2581: { struct Cyc_List_List* _temp2582=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp2530); goto
_LL2583; _LL2583: for( 0; _temp2580 != 0;( _temp2580=(( struct Cyc_List_List*)
_check_null( _temp2580))->tl, _temp2582=(( struct Cyc_List_List*) _check_null(
_temp2582))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp2582))->hd); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2574,( struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp2580))->hd),(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp2582))->hd, 0),
s2, 0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}}} _LL2463:{ void*
_temp2584=( void*) _temp2534->r; int _temp2594; struct Cyc_Absyn_Exp* _temp2596;
struct Cyc_Core_Opt* _temp2598; struct Cyc_Core_Opt* _temp2600; struct Cyc_Absyn_Pat*
_temp2602; struct Cyc_List_List* _temp2604; struct Cyc_Absyn_Vardecl* _temp2606;
_LL2586: if(*(( int*) _temp2584) == Cyc_Absyn_Let_d){ _LL2603: _temp2602=((
struct Cyc_Absyn_Let_d_struct*) _temp2584)->f1; goto _LL2601; _LL2601: _temp2600=((
struct Cyc_Absyn_Let_d_struct*) _temp2584)->f2; goto _LL2599; _LL2599: _temp2598=((
struct Cyc_Absyn_Let_d_struct*) _temp2584)->f3; goto _LL2597; _LL2597: _temp2596=((
struct Cyc_Absyn_Let_d_struct*) _temp2584)->f4; goto _LL2595; _LL2595: _temp2594=((
struct Cyc_Absyn_Let_d_struct*) _temp2584)->f5; goto _LL2587;} else{ goto
_LL2588;} _LL2588: if(*(( int*) _temp2584) == Cyc_Absyn_Letv_d){ _LL2605:
_temp2604=(( struct Cyc_Absyn_Letv_d_struct*) _temp2584)->f1; goto _LL2589;}
else{ goto _LL2590;} _LL2590: if(*(( int*) _temp2584) == Cyc_Absyn_Var_d){
_LL2607: _temp2606=(( struct Cyc_Absyn_Var_d_struct*) _temp2584)->f1; goto
_LL2591;} else{ goto _LL2592;} _LL2592: goto _LL2593; _LL2587:( void*)( s->r=(
void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2602, _temp2598, _temp2596,
_temp2594, _temp2532))->r)); goto _LL2585; _LL2589: { struct Cyc_List_List*
_temp2608=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2604); goto _LL2609; _LL2609: if( _temp2608 == 0){(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d", sizeof( unsigned char),
13u));}( void*)( _temp2534->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2610=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2610[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2611; _temp2611.tag= Cyc_Absyn_Var_d;
_temp2611.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2608))->hd; _temp2611;}); _temp2610;}))); _temp2608=(( struct Cyc_List_List*)
_check_null( _temp2608))->tl; for( 0; _temp2608 != 0; _temp2608=(( struct Cyc_List_List*)
_check_null( _temp2608))->tl){ struct Cyc_Absyn_Decl* _temp2614= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2612=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2612[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2613; _temp2613.tag= Cyc_Absyn_Var_d; _temp2613.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2608))->hd; _temp2613;}); _temp2612;}),
0); goto _LL2615; _LL2615:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp2614, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c(
nv, s); goto _LL2585;} _LL2591: { struct _tuple0* _temp2616= _temp2606->name;
goto _LL2617; _LL2617:( void*)( _temp2606->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2606->type));{ struct Cyc_Toc_Env* _temp2620= Cyc_Toc_add_varmap( nv,
_temp2616, Cyc_Absyn_varb_exp( _temp2616,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2618=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2618[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2619; _temp2619.tag= Cyc_Absyn_Local_b;
_temp2619.f1= _temp2606; _temp2619;}); _temp2618;}), 0)); goto _LL2621; _LL2621:
Cyc_Toc_stmt_to_c( _temp2620, _temp2532); if( _temp2606->initializer != 0){
struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*) _check_null( _temp2606->initializer);
void* _temp2622=( void*) init->r; struct Cyc_Absyn_Exp* _temp2628; struct Cyc_Absyn_Exp*
_temp2630; struct Cyc_Absyn_Vardecl* _temp2632; _LL2624: if(*(( int*) _temp2622)
== Cyc_Absyn_Comprehension_e){ _LL2633: _temp2632=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2622)->f1; goto _LL2631; _LL2631: _temp2630=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2622)->f2; goto _LL2629; _LL2629: _temp2628=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2622)->f3; goto _LL2625;} else{ goto _LL2626;} _LL2626: goto _LL2627;
_LL2625: _temp2606->initializer= 0;( void*)( _temp2532->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2616, 0), _temp2632, _temp2630, _temp2628, Cyc_Absyn_new_stmt((
void*) _temp2532->r, 0), 0))->r)); goto _LL2623; _LL2627: Cyc_Toc_exp_to_c( nv,
init); goto _LL2623; _LL2623:;} goto _LL2585;}} _LL2593:(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function",
sizeof( unsigned char), 39u)); goto _LL2585; _LL2585:;} return; _LL2465: s=
_temp2536; continue; _LL2467: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv),
_temp2546); Cyc_Toc_exp_to_c( nv, _temp2544); return; _LL2469: { struct _tuple0*
h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var(); struct
_tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp*
was_thrown_exp= Cyc_Absyn_var_exp( was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ);
void* was_thrown_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_sint_t); e_exp->topt=({ struct
Cyc_Core_Opt* _temp2634=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2634->v=( void*) e_typ; _temp2634;}); nv= Cyc_Toc_add_varmap( nv, e_var,
e_exp); Cyc_Toc_stmt_to_c( nv, _temp2550);{ struct Cyc_Absyn_Stmt* _temp2635=
Cyc_Absyn_seq_stmt( _temp2550, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e,
0, 0), 0), 0); goto _LL2636; _LL2636: { struct _tuple0* _temp2637= Cyc_Toc_temp_var();
goto _LL2638; _LL2638: { struct Cyc_Absyn_Exp* _temp2639= Cyc_Absyn_var_exp(
_temp2637, 0); goto _LL2640; _LL2640: { struct Cyc_Absyn_Vardecl* _temp2641= Cyc_Absyn_new_vardecl(
_temp2637, Cyc_Absyn_exn_typ, 0); goto _LL2642; _LL2642: _temp2639->topt=({
struct Cyc_Core_Opt* _temp2643=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2643->v=( void*) Cyc_Absyn_exn_typ; _temp2643;});{ struct
Cyc_Absyn_Pat* _temp2648=({ struct Cyc_Absyn_Pat* _temp2644=( struct Cyc_Absyn_Pat*)
GC_malloc( sizeof( struct Cyc_Absyn_Pat)); _temp2644->r=( void*)(( void*)({
struct Cyc_Absyn_Var_p_struct* _temp2646=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp2646[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp2647; _temp2647.tag= Cyc_Absyn_Var_p; _temp2647.f1= _temp2641; _temp2647;});
_temp2646;})); _temp2644->topt=({ struct Cyc_Core_Opt* _temp2645=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2645->v=( void*) Cyc_Absyn_exn_typ;
_temp2645;}); _temp2644->loc= 0; _temp2644;}); goto _LL2649; _LL2649: { struct
Cyc_Absyn_Exp* _temp2650= Cyc_Absyn_throw_exp( _temp2639, 0); goto _LL2651;
_LL2651: _temp2650->topt=({ struct Cyc_Core_Opt* _temp2652=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2652->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp2652;});{ struct Cyc_Absyn_Stmt* _temp2653= Cyc_Absyn_exp_stmt( _temp2650,
0); goto _LL2654; _LL2654: { struct Cyc_Absyn_Switch_clause* _temp2658=({ struct
Cyc_Absyn_Switch_clause* _temp2655=( struct Cyc_Absyn_Switch_clause*) GC_malloc(
sizeof( struct Cyc_Absyn_Switch_clause)); _temp2655->pattern= _temp2648;
_temp2655->pat_vars=({ struct Cyc_Core_Opt* _temp2656=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2656->v=( void*)({ struct Cyc_List_List*
_temp2657=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2657->hd=( void*) _temp2641; _temp2657->tl= 0; _temp2657;}); _temp2656;});
_temp2655->where_clause= 0; _temp2655->body= _temp2653; _temp2655->loc= 0;
_temp2655;}); goto _LL2659; _LL2659: { struct Cyc_Absyn_Stmt* _temp2661= Cyc_Absyn_switch_stmt(
e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp2548,({ struct Cyc_List_List* _temp2660=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2660->hd=( void*) _temp2658;
_temp2660->tl= 0; _temp2660;})), 0); goto _LL2662; _LL2662: Cyc_Toc_stmt_to_c(
nv, _temp2661);{ struct Cyc_Absyn_Exp* _temp2664= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp2663=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2663->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2663->tl= 0; _temp2663;}), 0); goto _LL2665;
_LL2665: { struct Cyc_Absyn_Stmt* _temp2667= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({ struct Cyc_List_List* _temp2666=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2666->hd=( void*) Cyc_Absyn_address_exp(
h_exp, 0); _temp2666->tl= 0; _temp2666;}), 0), 0); goto _LL2668; _LL2668: {
struct Cyc_Absyn_Exp* _temp2669= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0,
0); goto _LL2670; _LL2670: { struct Cyc_Absyn_Exp* _temp2671= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0); goto _LL2672; _LL2672:( void*)( s->r=( void*)((
void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2667,
Cyc_Absyn_declare_stmt( was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*)
_temp2669, Cyc_Absyn_seq_stmt( Cyc_Absyn_ifthenelse_stmt( _temp2664, Cyc_Absyn_assign_stmt(
was_thrown_exp, _temp2671, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2635, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp2661, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2471: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); struct _tuple0* rh_var= Cyc_Toc_temp_var();
struct _tuple0* x_var= _temp2554->name; struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2675=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2675[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2676; _temp2676.tag= Cyc_Absyn_Local_b; _temp2676.f1=
_temp2554; _temp2676;}); _temp2675;}), 0); struct Cyc_Absyn_Exp* addr_rh_exp=
Cyc_Absyn_address_exp( rh_exp, 0); struct Cyc_Absyn_Exp* newregion_call= Cyc_Absyn_fncall_exp(
Cyc_Toc__new_region_e, 0, 0); struct Cyc_Absyn_Exp* popregion_call= Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({ struct Cyc_List_List* _temp2674=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2674->hd=( void*) addr_rh_exp;
_temp2674->tl= 0; _temp2674;}), 0); struct Cyc_Absyn_Exp* pushregion_call= Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_List_List* _temp2673=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2673->hd=( void*) x_exp;
_temp2673->tl= 0; _temp2673;}), 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp2552);{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt(
_temp2552, Cyc_Absyn_exp_stmt( popregion_call, 0), 0); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( pushregion_call, 0), s2, 0);( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, s2, 0), 0))->r)); return;}} _LL2473:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("switchC_s", sizeof( unsigned char), 10u)); return;
_LL2475:(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("cut",
sizeof( unsigned char), 4u)); return; _LL2477:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("splice", sizeof( unsigned char), 7u)); return; _LL2437:;}}
struct _tuple16{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp2677= f->args; goto _LL2678;
_LL2678: for( 0; _temp2677 != 0; _temp2677=(( struct Cyc_List_List*) _check_null(
_temp2677))->tl){(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2677))->hd)).f3= Cyc_Toc_typ_to_c((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2677))->hd)).f3);{ struct _tuple0* _temp2680=({ struct _tuple0*
_temp2679=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2679->f1=(
void*) Cyc_Absyn_Loc_n; _temp2679->f2=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2677))->hd)).f1; _temp2679;}); goto _LL2681; _LL2681: nv= Cyc_Toc_add_varmap(
nv, _temp2680, Cyc_Absyn_var_exp( _temp2680, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2684; void* _temp2686; void* _temp2688; struct Cyc_Absyn_Tqual _temp2690;
struct Cyc_Core_Opt* _temp2692; struct Cyc_Absyn_VarargInfo _temp2682=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2693: _temp2692=
_temp2682.name; goto _LL2691; _LL2691: _temp2690= _temp2682.tq; goto _LL2689;
_LL2689: _temp2688=( void*) _temp2682.type; goto _LL2687; _LL2687: _temp2686=(
void*) _temp2682.rgn; goto _LL2685; _LL2685: _temp2684= _temp2682.inject; goto
_LL2683; _LL2683: { void* _temp2694= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2688, _temp2686, _temp2690)); goto _LL2695; _LL2695: { struct _tuple0*
_temp2697=({ struct _tuple0* _temp2696=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2696->f1=( void*) Cyc_Absyn_Loc_n; _temp2696->f2=( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp2692))->v; _temp2696;});
goto _LL2698; _LL2698: f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2699=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2699->hd=( void*)({ struct _tuple16* _temp2700=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp2700->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp2692))->v; _temp2700->f2= _temp2690;
_temp2700->f3= _temp2694; _temp2700;}); _temp2699->tl= 0; _temp2699;})); nv= Cyc_Toc_add_varmap(
nv, _temp2697, Cyc_Absyn_var_exp( _temp2697, 0)); f->cyc_varargs= 0;}}}{ struct
Cyc_List_List* _temp2701=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; goto _LL2702; _LL2702: for( 0; _temp2701 !=
0; _temp2701=(( struct Cyc_List_List*) _check_null( _temp2701))->tl){( void*)(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp2701))->hd)->type=(
void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp2701))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2703= s;
_LL2705: if( _temp2703 ==( void*) Cyc_Absyn_Abstract){ goto _LL2706;} else{ goto
_LL2707;} _LL2707: if( _temp2703 ==( void*) Cyc_Absyn_ExternC){ goto _LL2708;}
else{ goto _LL2709;} _LL2709: goto _LL2710; _LL2706: return( void*) Cyc_Absyn_Public;
_LL2708: return( void*) Cyc_Absyn_Extern; _LL2710: return s; _LL2704:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2711=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2712; _LL2712: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2713=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2713->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2713;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2714=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2711).f2); goto _LL2715; _LL2715: if( _temp2714 == 0){ seen_defn_before= 0;(
struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2711).f2,
s));} else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*)
_check_null( _temp2714))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2711).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2716=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2716->v=( void*)
_temp2711; _temp2716;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2717=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2718; _LL2718: for( 0; _temp2717 != 0;
_temp2717=(( struct Cyc_List_List*) _check_null( _temp2717))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2717))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2717))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2719=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2720; _LL2720: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2721=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2721->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2721;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2722=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2719).f2); goto _LL2723; _LL2723: if( _temp2722 == 0){ seen_defn_before= 0;(
struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp2719).f2,
u));} else{ if((( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*)
_check_null( _temp2722))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2719).f2, u)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)(((( void*) u->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
u->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) u->sc))); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2724=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2724->v=( void*)
_temp2719; _temp2724;}); if( u->fields != 0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp2725=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; goto _LL2726; _LL2726: for( 0; _temp2725 != 0;
_temp2725=(( struct Cyc_List_List*) _check_null( _temp2725))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2725))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2725))->hd)->type));}}}}} static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far= 0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl*
tud){ if( Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2727=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2727->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2727;});}{ struct
_tuple0* _temp2728= tud->name; goto _LL2729; _LL2729: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2728)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2728));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2730=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2731; _LL2731: for( 0;
_temp2730 != 0; _temp2730=(( struct Cyc_List_List*) _check_null( _temp2730))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2730))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2732; _temp2732.q_const= 1; _temp2732.q_volatile= 0;
_temp2732.q_restrict= 0; _temp2732;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2733=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2733->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2734=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2734[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2735; _temp2735.tag= Cyc_Absyn_Var_d;
_temp2735.f1= vd; _temp2735;}); _temp2734;}), 0); _temp2733->tl= Cyc_Toc_result_decls;
_temp2733;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2736; _temp2736.q_const=
1; _temp2736.q_volatile= 0; _temp2736.q_restrict= 0; _temp2736;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2737=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2737->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2738=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2738[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2739; _temp2739.tag= Cyc_Absyn_Var_d; _temp2739.f1= vd; _temp2739;});
_temp2738;}), 0); _temp2737->tl= Cyc_Toc_result_decls; _temp2737;});{ struct Cyc_List_List*
_temp2740= 0; goto _LL2741; _LL2741: { int i= 1;{ struct Cyc_List_List*
_temp2742= f->typs; goto _LL2743; _LL2743: for( 0; _temp2742 != 0;( _temp2742=((
struct Cyc_List_List*) _check_null( _temp2742))->tl, i ++)){ struct _tagged_arr*
_temp2744= Cyc_Toc_fieldname( i); goto _LL2745; _LL2745: { struct Cyc_Absyn_Structfield*
_temp2747=({ struct Cyc_Absyn_Structfield* _temp2746=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2746->name= _temp2744;
_temp2746->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2742))->hd)).f1; _temp2746->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2742))->hd)).f2);
_temp2746->width= 0; _temp2746->attributes= 0; _temp2746;}); goto _LL2748;
_LL2748: _temp2740=({ struct Cyc_List_List* _temp2749=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2749->hd=( void*) _temp2747;
_temp2749->tl= _temp2740; _temp2749;});}}} _temp2740=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2740); _temp2740=({ struct Cyc_List_List*
_temp2750=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2750->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2751=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2751->name= Cyc_Toc_tag_sp;
_temp2751->tq= Cyc_Toc_mt_tq; _temp2751->type=( void*) Cyc_Absyn_sint_t;
_temp2751->width= 0; _temp2751->attributes= 0; _temp2751;}); _temp2750->tl=
_temp2740; _temp2750;});{ struct Cyc_Absyn_Structdecl* _temp2755=({ struct Cyc_Absyn_Structdecl*
_temp2752=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2752->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2752->name=({ struct Cyc_Core_Opt*
_temp2754=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2754->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp2754;}); _temp2752->tvs= 0; _temp2752->fields=({
struct Cyc_Core_Opt* _temp2753=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2753->v=( void*) _temp2740; _temp2753;}); _temp2752->attributes=
0; _temp2752;}); goto _LL2756; _LL2756: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2757=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2757->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2758=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2758[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2759; _temp2759.tag= Cyc_Absyn_Struct_d;
_temp2759.f1= _temp2755; _temp2759;}); _temp2758;}), 0); _temp2757->tl= Cyc_Toc_result_decls;
_temp2757;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2760=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2760->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2760;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2761= xd->name; goto _LL2762; _LL2762: {
struct Cyc_List_List* _temp2763=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2764; _LL2764: for( 0; _temp2763 != 0;
_temp2763=(( struct Cyc_List_List*) _check_null( _temp2763))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2763))->hd; struct _tagged_arr* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2765= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof( unsigned char)) + 4,
0); goto _LL2766; _LL2766: { struct Cyc_Absyn_ArrayType_struct* _temp2769=({
struct Cyc_Absyn_ArrayType_struct* _temp2767=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2767[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2768; _temp2768.tag= Cyc_Absyn_ArrayType;
_temp2768.f1=( void*) Cyc_Absyn_uchar_t; _temp2768.f2= Cyc_Toc_mt_tq; _temp2768.f3=(
struct Cyc_Absyn_Exp*) _temp2765; _temp2768;}); _temp2767;}); goto _LL2770;
_LL2770: { struct Cyc_Core_Opt* _temp2771=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2779; int _temp2780; _LL2773: if( _temp2771 == 0){ goto
_LL2774;} else{ goto _LL2775;} _LL2775: if( _temp2771 == 0){ goto _LL2777;}
else{ _temp2779=* _temp2771; _LL2781: _temp2780=( int) _temp2779.v; if(
_temp2780 == 0){ goto _LL2776;} else{ goto _LL2777;}} _LL2777: goto _LL2778;
_LL2774: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_arr)({
int _temp2782= 0; int _temp2783= 0; int _temp2784= 0; int _temp2785= 0; struct
_tagged_arr _temp2786=* fn; xprintf("%c%c%c%c%.*s", _temp2782, _temp2783,
_temp2784, _temp2785, _temp2786.last_plus_one - _temp2786.curr, _temp2786.curr);}),
0);}{ struct Cyc_Absyn_Vardecl* _temp2787= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2769, initopt); goto _LL2788; _LL2788:( void*)( _temp2787->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2789=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2789->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2790=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2790[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2791; _temp2791.tag= Cyc_Absyn_Var_d;
_temp2791.f1= _temp2787; _temp2791;}); _temp2790;}), 0); _temp2789->tl= Cyc_Toc_result_decls;
_temp2789;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2792= f->typs; goto _LL2793; _LL2793: for( 0;
_temp2792 != 0;( _temp2792=(( struct Cyc_List_List*) _check_null( _temp2792))->tl,
i ++)){ struct _tagged_arr* _temp2795=({ struct _tagged_arr* _temp2794=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2794[ 0]=( struct
_tagged_arr) xprintf("f%d", i); _temp2794;}); goto _LL2796; _LL2796: { struct
Cyc_Absyn_Structfield* _temp2798=({ struct Cyc_Absyn_Structfield* _temp2797=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2797->name= _temp2795; _temp2797->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2792))->hd)).f1; _temp2797->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2792))->hd)).f2);
_temp2797->width= 0; _temp2797->attributes= 0; _temp2797;}); goto _LL2799;
_LL2799: fields=({ struct Cyc_List_List* _temp2800=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2800->hd=( void*) _temp2798;
_temp2800->tl= fields; _temp2800;});}}} fields=({ struct Cyc_List_List*
_temp2801=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2801->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2802=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2802->name= Cyc_Toc_tag_sp;
_temp2802->tq= Cyc_Toc_mt_tq; _temp2802->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2802->width= 0; _temp2802->attributes= 0; _temp2802;});
_temp2801->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2801;});{ struct Cyc_Absyn_Structdecl* _temp2806=({ struct Cyc_Absyn_Structdecl*
_temp2803=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2803->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2803->name=({ struct Cyc_Core_Opt*
_temp2805=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2805->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp2805;}); _temp2803->tvs= 0; _temp2803->fields=({
struct Cyc_Core_Opt* _temp2804=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2804->v=( void*) fields; _temp2804;}); _temp2803->attributes=
0; _temp2803;}); goto _LL2807; _LL2807: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2808=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2808->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2809=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2809[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2810; _temp2810.tag= Cyc_Absyn_Struct_d;
_temp2810.f1= _temp2806; _temp2810;}); _temp2809;}), 0); _temp2808->tl= Cyc_Toc_result_decls;
_temp2808;});}} goto _LL2772;}} _LL2776: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2816= Cyc_Absyn_string_exp(( struct _tagged_arr)({
int _temp2811= 0; int _temp2812= 0; int _temp2813= 0; int _temp2814= 0; struct
_tagged_arr _temp2815=* fn; xprintf("%c%c%c%c%.*s", _temp2811, _temp2812,
_temp2813, _temp2814, _temp2815.last_plus_one - _temp2815.curr, _temp2815.curr);}),
0); goto _LL2817; _LL2817: { struct Cyc_Absyn_Vardecl* _temp2818= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2769,( struct Cyc_Absyn_Exp*) _temp2816); goto _LL2819;
_LL2819:( void*)( _temp2818->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2820=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2820->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2821=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2821[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2822; _temp2822.tag= Cyc_Absyn_Var_d; _temp2822.f1= _temp2818; _temp2822;});
_temp2821;}), 0); _temp2820->tl= Cyc_Toc_result_decls; _temp2820;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2772; _LL2778: goto
_LL2772; _LL2772:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2823= ed->name; goto
_LL2824; _LL2824:( void*)( ed->sc=( void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) ed->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) ed->sc))); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2825=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2826; _LL2826: for( 0; _temp2825 != 0;
_temp2825=(( struct Cyc_List_List*) _check_null( _temp2825))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2825))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2825))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2825))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_arr*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct _RegionHandle
_temp2827= _new_region(); struct _RegionHandle* prgn=& _temp2827; _push_region(
prgn);{ struct Cyc_Absyn_Stmt* _temp2830; struct Cyc_List_List* _temp2832;
struct Cyc_Toc_Env* _temp2834; struct _tuple12 _temp2828= Cyc_Toc_xlate_pat( nv,
prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2835: _temp2834= _temp2828.f1; goto _LL2833; _LL2833: _temp2832=
_temp2828.f2; goto _LL2831; _LL2831: _temp2830= _temp2828.f3; goto _LL2829;
_LL2829: Cyc_Toc_stmt_to_c( _temp2834, s);{ struct Cyc_Absyn_Stmt* _temp2836=
Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL2837; _LL2837: if( exhaust){ s=
Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2830, _temp2836, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2838= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); goto _LL2839; _LL2839: { struct Cyc_Absyn_Stmt*
_temp2840= Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2838, 0), 0);
goto _LL2841; _LL2841: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),(
struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2830, Cyc_Absyn_seq_stmt(
_temp2840, _temp2836, 0), 0), 0);}} for( 0; _temp2832 != 0; _temp2832=(( struct
Cyc_List_List*) _check_null( _temp2832))->tl){ struct _tuple13 _temp2844; void*
_temp2845; struct _tuple0* _temp2847; struct _tuple13* _temp2842=( struct
_tuple13*)(( struct Cyc_List_List*) _check_null( _temp2832))->hd; _temp2844=*
_temp2842; _LL2848: _temp2847= _temp2844.f1; goto _LL2846; _LL2846: _temp2845=
_temp2844.f2; goto _LL2843; _LL2843: s= Cyc_Absyn_declare_stmt( _temp2847,
_temp2845, 0, s, 0);}}}; _pop_region(& _temp2827);} return s;}} static struct
Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( ds))->hd; void* _temp2849=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2875; struct Cyc_Absyn_Fndecl* _temp2877; int _temp2879; struct Cyc_Absyn_Exp*
_temp2881; struct Cyc_Core_Opt* _temp2883; struct Cyc_Core_Opt* _temp2885;
struct Cyc_Absyn_Pat* _temp2887; struct Cyc_List_List* _temp2889; struct Cyc_Absyn_Structdecl*
_temp2891; struct Cyc_Absyn_Uniondecl* _temp2893; struct Cyc_Absyn_Tuniondecl*
_temp2895; struct Cyc_Absyn_Enumdecl* _temp2897; struct Cyc_Absyn_Typedefdecl*
_temp2899; struct Cyc_List_List* _temp2901; struct _tagged_arr* _temp2903;
struct Cyc_List_List* _temp2905; struct _tuple0* _temp2907; struct Cyc_List_List*
_temp2909; _LL2851: if(*(( int*) _temp2849) == Cyc_Absyn_Var_d){ _LL2876:
_temp2875=(( struct Cyc_Absyn_Var_d_struct*) _temp2849)->f1; goto _LL2852;}
else{ goto _LL2853;} _LL2853: if(*(( int*) _temp2849) == Cyc_Absyn_Fn_d){
_LL2878: _temp2877=(( struct Cyc_Absyn_Fn_d_struct*) _temp2849)->f1; goto
_LL2854;} else{ goto _LL2855;} _LL2855: if(*(( int*) _temp2849) == Cyc_Absyn_Let_d){
_LL2888: _temp2887=(( struct Cyc_Absyn_Let_d_struct*) _temp2849)->f1; goto
_LL2886; _LL2886: _temp2885=(( struct Cyc_Absyn_Let_d_struct*) _temp2849)->f2;
goto _LL2884; _LL2884: _temp2883=(( struct Cyc_Absyn_Let_d_struct*) _temp2849)->f3;
goto _LL2882; _LL2882: _temp2881=(( struct Cyc_Absyn_Let_d_struct*) _temp2849)->f4;
goto _LL2880; _LL2880: _temp2879=(( struct Cyc_Absyn_Let_d_struct*) _temp2849)->f5;
goto _LL2856;} else{ goto _LL2857;} _LL2857: if(*(( int*) _temp2849) == Cyc_Absyn_Letv_d){
_LL2890: _temp2889=(( struct Cyc_Absyn_Letv_d_struct*) _temp2849)->f1; goto
_LL2858;} else{ goto _LL2859;} _LL2859: if(*(( int*) _temp2849) == Cyc_Absyn_Struct_d){
_LL2892: _temp2891=(( struct Cyc_Absyn_Struct_d_struct*) _temp2849)->f1; goto
_LL2860;} else{ goto _LL2861;} _LL2861: if(*(( int*) _temp2849) == Cyc_Absyn_Union_d){
_LL2894: _temp2893=(( struct Cyc_Absyn_Union_d_struct*) _temp2849)->f1; goto
_LL2862;} else{ goto _LL2863;} _LL2863: if(*(( int*) _temp2849) == Cyc_Absyn_Tunion_d){
_LL2896: _temp2895=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2849)->f1; goto
_LL2864;} else{ goto _LL2865;} _LL2865: if(*(( int*) _temp2849) == Cyc_Absyn_Enum_d){
_LL2898: _temp2897=(( struct Cyc_Absyn_Enum_d_struct*) _temp2849)->f1; goto
_LL2866;} else{ goto _LL2867;} _LL2867: if(*(( int*) _temp2849) == Cyc_Absyn_Typedef_d){
_LL2900: _temp2899=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2849)->f1; goto
_LL2868;} else{ goto _LL2869;} _LL2869: if(*(( int*) _temp2849) == Cyc_Absyn_Namespace_d){
_LL2904: _temp2903=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2849)->f1; goto
_LL2902; _LL2902: _temp2901=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2849)->f2;
goto _LL2870;} else{ goto _LL2871;} _LL2871: if(*(( int*) _temp2849) == Cyc_Absyn_Using_d){
_LL2908: _temp2907=(( struct Cyc_Absyn_Using_d_struct*) _temp2849)->f1; goto
_LL2906; _LL2906: _temp2905=(( struct Cyc_Absyn_Using_d_struct*) _temp2849)->f2;
goto _LL2872;} else{ goto _LL2873;} _LL2873: if(*(( int*) _temp2849) == Cyc_Absyn_ExternC_d){
_LL2910: _temp2909=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2849)->f1; goto
_LL2874;} else{ goto _LL2850;} _LL2852: { struct _tuple0* _temp2911= _temp2875->name;
goto _LL2912; _LL2912: if(( void*) _temp2875->sc ==( void*) Cyc_Absyn_ExternC){
_temp2911=({ struct _tuple0* _temp2913=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2913->f1= Cyc_Toc_rel_ns; _temp2913->f2=(* _temp2911).f2;
_temp2913;});} if( _temp2875->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*) _check_null( _temp2875->initializer));} if( ! nv->toplevel){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u));} nv= Cyc_Toc_add_varmap( nv, _temp2875->name, Cyc_Absyn_varb_exp(
_temp2911,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp2914=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp2914[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp2915; _temp2915.tag= Cyc_Absyn_Global_b;
_temp2915.f1= _temp2875; _temp2915;}); _temp2914;}), 0)); _temp2875->name=
_temp2911;( void*)( _temp2875->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2875->sc));(
void*)( _temp2875->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2875->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2916=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2916->hd=( void*) d; _temp2916->tl=
Cyc_Toc_result_decls; _temp2916;}); goto _LL2850;} _LL2854: { struct _tuple0*
_temp2917= _temp2877->name; goto _LL2918; _LL2918: if(( void*) _temp2877->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2917=({ struct _tuple0* _temp2919=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2919->f1= Cyc_Toc_rel_ns;
_temp2919->f2=(* _temp2917).f2; _temp2919;});} nv= Cyc_Toc_add_varmap( nv,
_temp2877->name, Cyc_Absyn_var_exp( _temp2917, 0)); _temp2877->name= _temp2917;
Cyc_Toc_fndecl_to_c( nv, _temp2877); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2920=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2920->hd=( void*) d; _temp2920->tl= Cyc_Toc_result_decls; _temp2920;});
goto _LL2850;} _LL2856:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("letdecl", sizeof( unsigned char), 8u)); goto _LL2850; _LL2858:(( int(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("letdecl", sizeof(
unsigned char), 8u)); goto _LL2850; _LL2860: Cyc_Toc_structdecl_to_c( _temp2891);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2921=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2921->hd=( void*) d; _temp2921->tl=
Cyc_Toc_result_decls; _temp2921;}); goto _LL2850; _LL2862: Cyc_Toc_uniondecl_to_c(
_temp2893); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2922=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2922->hd=( void*) d; _temp2922->tl=
Cyc_Toc_result_decls; _temp2922;}); goto _LL2850; _LL2864: if( _temp2895->is_xtunion){
Cyc_Toc_xtuniondecl_to_c( _temp2895);} else{ Cyc_Toc_tuniondecl_to_c( _temp2895);}
goto _LL2850; _LL2866: Cyc_Toc_enumdecl_to_c( nv, _temp2897); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2923=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2923->hd=( void*) d; _temp2923->tl= Cyc_Toc_result_decls;
_temp2923;}); goto _LL2850; _LL2868: _temp2899->name= _temp2899->name; _temp2899->tvs=
0;( void*)( _temp2899->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2899->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2924=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2924->hd=( void*) d; _temp2924->tl=
Cyc_Toc_result_decls; _temp2924;}); goto _LL2850; _LL2870: _temp2905= _temp2901;
goto _LL2872; _LL2872: _temp2909= _temp2905; goto _LL2874; _LL2874: nv= Cyc_Toc_decls_to_c(
nv, _temp2909, top); goto _LL2850; _LL2850:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}