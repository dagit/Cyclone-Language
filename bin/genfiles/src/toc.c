 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
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
extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
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
extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void** elt_typ_dest);
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
_get_arr_size( _temp0, 1u), _temp0.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Toc_Toc_Unimplemented);} static
void* Cyc_Toc_toc_impos( struct _tagged_arr s){({ struct _tagged_arr _temp1= s;
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s\n", _get_arr_size( _temp1, 1u),
_temp1.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);(
int) _throw(( void*) Cyc_Toc_Toc_Impossible);} unsigned char Cyc_Toc_Match_error[
16u]="\000\000\000\000Match_error"; static struct Cyc_Absyn_Rel_n_struct Cyc_Toc_rel_ns_value={
0u, 0}; static void* Cyc_Toc_rel_ns=( void*)& Cyc_Toc_rel_ns_value; static
unsigned char _temp3[ 5u]="curr"; static struct _tagged_arr Cyc_Toc_curr_string={
_temp3, _temp3, _temp3 + 5u}; static struct _tagged_arr* Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static unsigned char _temp4[ 4u]="tag"; static struct _tagged_arr Cyc_Toc_tag_string={
_temp4, _temp4, _temp4 + 4u}; static struct _tagged_arr* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static unsigned char _temp5[ 14u]="_handler_cons"; static struct _tagged_arr Cyc_Toc__handler_cons_string={
_temp5, _temp5, _temp5 + 14u}; static struct _tagged_arr* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static unsigned char _temp6[ 8u]="handler"; static
struct _tagged_arr Cyc_Toc_handler_string={ _temp6, _temp6, _temp6 + 8u}; static
struct _tagged_arr* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static
unsigned char _temp7[ 14u]="_RegionHandle"; static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_temp7, _temp7, _temp7 + 14u}; static struct _tagged_arr* Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string; static unsigned char _temp8[ 7u]="_throw"; static
struct _tagged_arr Cyc_Toc__throw_str={ _temp8, _temp8, _temp8 + 7u}; static
struct _tuple0 Cyc_Toc__throw_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__throw_str};
static struct _tuple0* Cyc_Toc__throw_qv=& Cyc_Toc__throw_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__throw_re={ 1u,& Cyc_Toc__throw_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__throw_ev={ 0,( void*)(( void*)& Cyc_Toc__throw_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__throw_e=& Cyc_Toc__throw_ev; static
unsigned char _temp10[ 7u]="setjmp"; static struct _tagged_arr Cyc_Toc_setjmp_str={
_temp10, _temp10, _temp10 + 7u}; static struct _tuple0 Cyc_Toc_setjmp_pr={( void*)&
Cyc_Toc_rel_ns_value,& Cyc_Toc_setjmp_str}; static struct _tuple0* Cyc_Toc_setjmp_qv=&
Cyc_Toc_setjmp_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={ 1u,&
Cyc_Toc_setjmp_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,( void*)(( void*)& Cyc_Toc_setjmp_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc_setjmp_e=&
Cyc_Toc_setjmp_ev; static unsigned char _temp12[ 14u]="_push_handler"; static
struct _tagged_arr Cyc_Toc__push_handler_str={ _temp12, _temp12, _temp12 + 14u};
static struct _tuple0 Cyc_Toc__push_handler_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__push_handler_str}; static struct _tuple0* Cyc_Toc__push_handler_qv=&
Cyc_Toc__push_handler_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1u,& Cyc_Toc__push_handler_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__push_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__push_handler_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;
static unsigned char _temp14[ 13u]="_pop_handler"; static struct _tagged_arr Cyc_Toc__pop_handler_str={
_temp14, _temp14, _temp14 + 13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__pop_handler_str}; static struct _tuple0*
Cyc_Toc__pop_handler_qv=& Cyc_Toc__pop_handler_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_handler_re={ 1u,& Cyc_Toc__pop_handler_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_handler_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;
static unsigned char _temp16[ 12u]="_exn_thrown"; static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_temp16, _temp16, _temp16 + 12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__exn_thrown_str}; static struct _tuple0*
Cyc_Toc__exn_thrown_qv=& Cyc_Toc__exn_thrown_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={ 1u,& Cyc_Toc__exn_thrown_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={ 0,( void*)(( void*)& Cyc_Toc__exn_thrown_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;
static unsigned char _temp18[ 14u]="_npop_handler"; static struct _tagged_arr
Cyc_Toc__npop_handler_str={ _temp18, _temp18, _temp18 + 14u}; static struct
_tuple0 Cyc_Toc__npop_handler_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__npop_handler_str};
static struct _tuple0* Cyc_Toc__npop_handler_qv=& Cyc_Toc__npop_handler_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={ 1u,& Cyc_Toc__npop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__npop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev; static unsigned char _temp20[ 12u]="_check_null";
static struct _tagged_arr Cyc_Toc__check_null_str={ _temp20, _temp20, _temp20 +
12u}; static struct _tuple0 Cyc_Toc__check_null_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__check_null_str}; static struct _tuple0* Cyc_Toc__check_null_qv=& Cyc_Toc__check_null_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={ 1u,& Cyc_Toc__check_null_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={ 0,(
void*)(( void*)& Cyc_Toc__check_null_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev; static unsigned char _temp22[ 28u]="_check_known_subscript_null";
static struct _tagged_arr Cyc_Toc__check_known_subscript_null_str={ _temp22,
_temp22, _temp22 + 28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__check_known_subscript_null_str}; static
struct _tuple0* Cyc_Toc__check_known_subscript_null_qv=& Cyc_Toc__check_known_subscript_null_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={ 1u,&
Cyc_Toc__check_known_subscript_null_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_null_ev={ 0,( void*)(( void*)& Cyc_Toc__check_known_subscript_null_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static unsigned char _temp24[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={ _temp24, _temp24,
_temp24 + 31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct _tuple0* Cyc_Toc__check_known_subscript_notnull_qv=& Cyc_Toc__check_known_subscript_notnull_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={
1u,& Cyc_Toc__check_known_subscript_notnull_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={ 0,( void*)((
void*)& Cyc_Toc__check_known_subscript_notnull_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static unsigned char _temp26[ 25u]="_check_unknown_subscript"; static struct
_tagged_arr Cyc_Toc__check_unknown_subscript_str={ _temp26, _temp26, _temp26 +
25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__check_unknown_subscript_str}; static struct _tuple0* Cyc_Toc__check_unknown_subscript_qv=&
Cyc_Toc__check_unknown_subscript_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1u,& Cyc_Toc__check_unknown_subscript_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__check_unknown_subscript_ev={ 0,( void*)(( void*)& Cyc_Toc__check_unknown_subscript_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_unknown_subscript_e=& Cyc_Toc__check_unknown_subscript_ev;
static unsigned char _temp28[ 12u]="_tagged_arr"; static struct _tagged_arr Cyc_Toc__tagged_arr_str={
_temp28, _temp28, _temp28 + 12u}; static struct _tuple0 Cyc_Toc__tagged_arr_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__tagged_arr_str}; static struct _tuple0*
Cyc_Toc__tagged_arr_qv=& Cyc_Toc__tagged_arr_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_re={ 1u,& Cyc_Toc__tagged_arr_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_e=& Cyc_Toc__tagged_arr_ev;
static unsigned char _temp30[ 9u]="_tag_arr"; static struct _tagged_arr Cyc_Toc__tag_arr_str={
_temp30, _temp30, _temp30 + 9u}; static struct _tuple0 Cyc_Toc__tag_arr_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__tag_arr_str}; static struct _tuple0* Cyc_Toc__tag_arr_qv=&
Cyc_Toc__tag_arr_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={
1u,& Cyc_Toc__tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,( void*)(( void*)& Cyc_Toc__tag_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tag_arr_e=&
Cyc_Toc__tag_arr_ev; static unsigned char _temp32[ 14u]="_init_tag_arr"; static
struct _tagged_arr Cyc_Toc__init_tag_arr_str={ _temp32, _temp32, _temp32 + 14u};
static struct _tuple0 Cyc_Toc__init_tag_arr_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__init_tag_arr_str}; static struct _tuple0* Cyc_Toc__init_tag_arr_qv=&
Cyc_Toc__init_tag_arr_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_tag_arr_re={
1u,& Cyc_Toc__init_tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__init_tag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__init_tag_arr_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__init_tag_arr_e=& Cyc_Toc__init_tag_arr_ev;
static unsigned char _temp34[ 11u]="_untag_arr"; static struct _tagged_arr Cyc_Toc__untag_arr_str={
_temp34, _temp34, _temp34 + 11u}; static struct _tuple0 Cyc_Toc__untag_arr_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__untag_arr_str}; static struct _tuple0*
Cyc_Toc__untag_arr_qv=& Cyc_Toc__untag_arr_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__untag_arr_re={ 1u,& Cyc_Toc__untag_arr_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__untag_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;
static unsigned char _temp36[ 14u]="_get_arr_size"; static struct _tagged_arr
Cyc_Toc__get_arr_size_str={ _temp36, _temp36, _temp36 + 14u}; static struct
_tuple0 Cyc_Toc__get_arr_size_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__get_arr_size_str};
static struct _tuple0* Cyc_Toc__get_arr_size_qv=& Cyc_Toc__get_arr_size_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={ 1u,& Cyc_Toc__get_arr_size_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={ 0,(
void*)(( void*)& Cyc_Toc__get_arr_size_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__get_arr_size_e=&
Cyc_Toc__get_arr_size_ev; static unsigned char _temp38[ 17u]="_tagged_arr_plus";
static struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={ _temp38, _temp38,
_temp38 + 17u}; static struct _tuple0 Cyc_Toc__tagged_arr_plus_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__tagged_arr_plus_str}; static struct _tuple0* Cyc_Toc__tagged_arr_plus_qv=&
Cyc_Toc__tagged_arr_plus_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_plus_re={
1u,& Cyc_Toc__tagged_arr_plus_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_plus_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_plus_e=& Cyc_Toc__tagged_arr_plus_ev;
static unsigned char _temp40[ 25u]="_tagged_arr_inplace_plus"; static struct
_tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={ _temp40, _temp40, _temp40 +
25u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={( void*)& Cyc_Toc_rel_ns_value,&
Cyc_Toc__tagged_arr_inplace_plus_str}; static struct _tuple0* Cyc_Toc__tagged_arr_inplace_plus_qv=&
Cyc_Toc__tagged_arr_inplace_plus_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_inplace_plus_e=& Cyc_Toc__tagged_arr_inplace_plus_ev;
static unsigned char _temp42[ 30u]="_tagged_arr_inplace_plus_post"; static
struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={ _temp42, _temp42,
_temp42 + 30u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct _tuple0* Cyc_Toc__tagged_arr_inplace_plus_post_qv=& Cyc_Toc__tagged_arr_inplace_plus_post_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _temp44[ 10u]="GC_malloc"; static struct _tagged_arr Cyc_Toc_GC_malloc_str={
_temp44, _temp44, _temp44 + 10u}; static struct _tuple0 Cyc_Toc_GC_malloc_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc_GC_malloc_str}; static struct _tuple0*
Cyc_Toc_GC_malloc_qv=& Cyc_Toc_GC_malloc_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_GC_malloc_re={ 1u,& Cyc_Toc_GC_malloc_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc_GC_malloc_ev={ 0,( void*)(( void*)& Cyc_Toc_GC_malloc_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc_GC_malloc_e=& Cyc_Toc_GC_malloc_ev;
static unsigned char _temp46[ 17u]="GC_malloc_atomic"; static struct _tagged_arr
Cyc_Toc_GC_malloc_atomic_str={ _temp46, _temp46, _temp46 + 17u}; static struct
_tuple0 Cyc_Toc_GC_malloc_atomic_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc_GC_malloc_atomic_str};
static struct _tuple0* Cyc_Toc_GC_malloc_atomic_qv=& Cyc_Toc_GC_malloc_atomic_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_GC_malloc_atomic_re={ 1u,& Cyc_Toc_GC_malloc_atomic_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_GC_malloc_atomic_ev={
0,( void*)(( void*)& Cyc_Toc_GC_malloc_atomic_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc_GC_malloc_atomic_e=& Cyc_Toc_GC_malloc_atomic_ev; static unsigned char
_temp48[ 15u]="_region_malloc"; static struct _tagged_arr Cyc_Toc__region_malloc_str={
_temp48, _temp48, _temp48 + 15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__region_malloc_str}; static struct
_tuple0* Cyc_Toc__region_malloc_qv=& Cyc_Toc__region_malloc_pr; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={ 1u,& Cyc_Toc__region_malloc_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={ 0,(
void*)(( void*)& Cyc_Toc__region_malloc_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev; static unsigned char
_temp50[ 12u]="_new_region"; static struct _tagged_arr Cyc_Toc__new_region_str={
_temp50, _temp50, _temp50 + 12u}; static struct _tuple0 Cyc_Toc__new_region_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__new_region_str}; static struct _tuple0*
Cyc_Toc__new_region_qv=& Cyc_Toc__new_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__new_region_re={ 1u,& Cyc_Toc__new_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={ 0,( void*)(( void*)& Cyc_Toc__new_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;
static unsigned char _temp52[ 13u]="_push_region"; static struct _tagged_arr Cyc_Toc__push_region_str={
_temp52, _temp52, _temp52 + 13u}; static struct _tuple0 Cyc_Toc__push_region_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__push_region_str}; static struct _tuple0*
Cyc_Toc__push_region_qv=& Cyc_Toc__push_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={ 1u,& Cyc_Toc__push_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={ 0,( void*)(( void*)& Cyc_Toc__push_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;
static unsigned char _temp54[ 12u]="_pop_region"; static struct _tagged_arr Cyc_Toc__pop_region_str={
_temp54, _temp54, _temp54 + 12u}; static struct _tuple0 Cyc_Toc__pop_region_pr={(
void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__pop_region_str}; static struct _tuple0*
Cyc_Toc__pop_region_qv=& Cyc_Toc__pop_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_region_re={ 1u,& Cyc_Toc__pop_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;
static unsigned char _temp56[ 15u]="_sfile_to_file"; static struct _tagged_arr
Cyc_Toc__sfile_to_file_str={ _temp56, _temp56, _temp56 + 15u}; static struct
_tuple0 Cyc_Toc__sfile_to_file_pr={( void*)& Cyc_Toc_rel_ns_value,& Cyc_Toc__sfile_to_file_str};
static struct _tuple0* Cyc_Toc__sfile_to_file_qv=& Cyc_Toc__sfile_to_file_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__sfile_to_file_re={ 1u,& Cyc_Toc__sfile_to_file_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__sfile_to_file_ev={ 0,(
void*)(( void*)& Cyc_Toc__sfile_to_file_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__sfile_to_file_e=& Cyc_Toc__sfile_to_file_ev; static struct Cyc_Absyn_StructType_struct
Cyc_Toc_tagged_arr_typ_v={ 9u,( struct _tuple0*)& Cyc_Toc__tagged_arr_pr, 0, 0};
static void* Cyc_Toc_tagged_arr_typ=( void*)& Cyc_Toc_tagged_arr_typ_v; static
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={ 0, 0, 0}; static unsigned char _temp59[ 3u]="f0";
static struct _tagged_arr Cyc_Toc_f0={ _temp59, _temp59, _temp59 + 3u}; static
struct _tagged_arr* Cyc_Toc_field_names_v[ 1u]={& Cyc_Toc_f0}; static struct
_tagged_arr Cyc_Toc_field_names={( void*)(( struct _tagged_arr**) Cyc_Toc_field_names_v),(
void*)(( struct _tagged_arr**) Cyc_Toc_field_names_v),( void*)(( struct
_tagged_arr**) Cyc_Toc_field_names_v) + 1u}; static struct _tagged_arr* Cyc_Toc_fieldname(
int i){ unsigned int fsz= _get_arr_size( Cyc_Toc_field_names, sizeof( struct
_tagged_arr*)); if( i >= fsz){ Cyc_Toc_field_names=({ unsigned int _temp60=(
unsigned int)( i + 1); struct _tagged_arr** _temp61=( struct _tagged_arr**)
GC_malloc( sizeof( struct _tagged_arr*) * _temp60); struct _tagged_arr _temp64=
_tag_arr( _temp61, sizeof( struct _tagged_arr*),( unsigned int)( i + 1));{
unsigned int _temp62= _temp60; unsigned int j; for( j= 0; j < _temp62; j ++){
_temp61[ j]= j < fsz?*(( struct _tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names,
sizeof( struct _tagged_arr*),( int) j)):({ struct _tagged_arr* _temp63=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp63[ 0]=( struct
_tagged_arr) xprintf("f%d",( int) j); _temp63;});}}; _temp64;});} return*((
struct _tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names, sizeof(
struct _tagged_arr*), i));} struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp65=(
void*) e->r; void* _temp91; unsigned char _temp93; void* _temp95; void* _temp97;
short _temp99; void* _temp101; void* _temp103; int _temp105; void* _temp107;
void* _temp109; long long _temp111; void* _temp113; void* _temp115; struct Cyc_Absyn_Exp*
_temp117; void* _temp119; struct Cyc_List_List* _temp121; struct Cyc_List_List*
_temp123; struct Cyc_Absyn_Structdecl* _temp125; struct Cyc_List_List* _temp127;
struct Cyc_Core_Opt* _temp129; struct _tuple0* _temp131; struct Cyc_List_List*
_temp133; struct _tuple1* _temp135; struct Cyc_List_List* _temp137; struct Cyc_Core_Opt*
_temp139; _LL67: if(*(( int*) _temp65) == Cyc_Absyn_Const_e){ _LL92: _temp91=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp65)->f1; if(( unsigned int)
_temp91 > 1u?*(( int*) _temp91) == Cyc_Absyn_Char_c: 0){ _LL96: _temp95=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp91)->f1; goto _LL94; _LL94: _temp93=((
struct Cyc_Absyn_Char_c_struct*) _temp91)->f2; goto _LL68;} else{ goto _LL69;}}
else{ goto _LL69;} _LL69: if(*(( int*) _temp65) == Cyc_Absyn_Const_e){ _LL98:
_temp97=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp65)->f1; if((
unsigned int) _temp97 > 1u?*(( int*) _temp97) == Cyc_Absyn_Short_c: 0){ _LL102:
_temp101=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp97)->f1; goto _LL100;
_LL100: _temp99=(( struct Cyc_Absyn_Short_c_struct*) _temp97)->f2; goto _LL70;}
else{ goto _LL71;}} else{ goto _LL71;} _LL71: if(*(( int*) _temp65) == Cyc_Absyn_Const_e){
_LL104: _temp103=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp65)->f1; if((
unsigned int) _temp103 > 1u?*(( int*) _temp103) == Cyc_Absyn_Int_c: 0){ _LL108:
_temp107=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp103)->f1; goto _LL106;
_LL106: _temp105=(( struct Cyc_Absyn_Int_c_struct*) _temp103)->f2; goto _LL72;}
else{ goto _LL73;}} else{ goto _LL73;} _LL73: if(*(( int*) _temp65) == Cyc_Absyn_Const_e){
_LL110: _temp109=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp65)->f1; if((
unsigned int) _temp109 > 1u?*(( int*) _temp109) == Cyc_Absyn_LongLong_c: 0){
_LL114: _temp113=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp109)->f1;
goto _LL112; _LL112: _temp111=(( struct Cyc_Absyn_LongLong_c_struct*) _temp109)->f2;
goto _LL74;} else{ goto _LL75;}} else{ goto _LL75;} _LL75: if(*(( int*) _temp65)
== Cyc_Absyn_Const_e){ _LL116: _temp115=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp65)->f1; if( _temp115 ==( void*) Cyc_Absyn_Null_c){ goto _LL76;} else{ goto
_LL77;}} else{ goto _LL77;} _LL77: if(*(( int*) _temp65) == Cyc_Absyn_Cast_e){
_LL120: _temp119=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp65)->f1; goto
_LL118; _LL118: _temp117=(( struct Cyc_Absyn_Cast_e_struct*) _temp65)->f2; goto
_LL78;} else{ goto _LL79;} _LL79: if(*(( int*) _temp65) == Cyc_Absyn_Tuple_e){
_LL122: _temp121=(( struct Cyc_Absyn_Tuple_e_struct*) _temp65)->f1; goto _LL80;}
else{ goto _LL81;} _LL81: if(*(( int*) _temp65) == Cyc_Absyn_Array_e){ _LL124:
_temp123=(( struct Cyc_Absyn_Array_e_struct*) _temp65)->f1; goto _LL82;} else{
goto _LL83;} _LL83: if(*(( int*) _temp65) == Cyc_Absyn_Struct_e){ _LL132:
_temp131=(( struct Cyc_Absyn_Struct_e_struct*) _temp65)->f1; goto _LL130; _LL130:
_temp129=(( struct Cyc_Absyn_Struct_e_struct*) _temp65)->f2; goto _LL128; _LL128:
_temp127=(( struct Cyc_Absyn_Struct_e_struct*) _temp65)->f3; goto _LL126; _LL126:
_temp125=(( struct Cyc_Absyn_Struct_e_struct*) _temp65)->f4; goto _LL84;} else{
goto _LL85;} _LL85: if(*(( int*) _temp65) == Cyc_Absyn_CompoundLit_e){ _LL136:
_temp135=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp65)->f1; goto _LL134;
_LL134: _temp133=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp65)->f2; goto
_LL86;} else{ goto _LL87;} _LL87: if(*(( int*) _temp65) == Cyc_Absyn_UnresolvedMem_e){
_LL140: _temp139=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp65)->f1; goto
_LL138; _LL138: _temp137=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp65)->f2;
goto _LL88;} else{ goto _LL89;} _LL89: goto _LL90; _LL68: return _temp93 =='\000';
_LL70: return _temp99 == 0; _LL72: return _temp105 == 0; _LL74: return _temp111
== 0; _LL76: return 1; _LL78: return Cyc_Toc_is_zero( _temp117); _LL80: return((
int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp121); _LL82: _temp127= _temp123; goto _LL84; _LL84:
_temp133= _temp127; goto _LL86; _LL86: _temp137= _temp133; goto _LL88; _LL88:
for( 0; _temp137 != 0; _temp137=(( struct Cyc_List_List*) _check_null( _temp137))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp137))->hd)).f2)){ return 0;}} return 1; _LL90: return 0; _LL66:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp141= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp147; struct Cyc_Absyn_Conref* _temp149; struct Cyc_Absyn_Tqual
_temp151; struct Cyc_Absyn_Conref* _temp153; void* _temp155; void* _temp157;
_LL143: if(( unsigned int) _temp141 > 4u?*(( int*) _temp141) == Cyc_Absyn_PointerType:
0){ _LL148: _temp147=(( struct Cyc_Absyn_PointerType_struct*) _temp141)->f1;
_LL158: _temp157=( void*) _temp147.elt_typ; goto _LL156; _LL156: _temp155=( void*)
_temp147.rgn_typ; goto _LL154; _LL154: _temp153= _temp147.nullable; goto _LL152;
_LL152: _temp151= _temp147.tq; goto _LL150; _LL150: _temp149= _temp147.bounds;
goto _LL144;} else{ goto _LL145;} _LL145: goto _LL146; _LL144: return(( int(*)(
int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp153); _LL146:((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("is_nullable",
sizeof( unsigned char), 12u)); return 0; _LL142:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag(
struct _tuple0* x, struct _tagged_arr tag){ return({ struct _tuple0* _temp159=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp159->f1=(* x).f1;
_temp159->f2=({ struct _tagged_arr* _temp160=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp160[ 0]=( struct _tagged_arr) Cyc_String_strconcat(*(*
x).f2, tag); _temp160;}); _temp159;});} struct _tuple5{ struct _tagged_arr* f1;
struct Cyc_List_List* f2; } ; static struct _tagged_arr* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp161= Cyc_Toc_tuple_types;
goto _LL162; _LL162: for( 0; _temp161 != 0; _temp161=(( struct Cyc_List_List*)
_check_null( _temp161))->tl){ struct _tuple5 _temp165; struct Cyc_List_List*
_temp166; struct _tagged_arr* _temp168; struct _tuple5* _temp163=( struct
_tuple5*)(( struct Cyc_List_List*) _check_null( _temp161))->hd; _temp165=*
_temp163; _LL169: _temp168= _temp165.f1; goto _LL167; _LL167: _temp166= _temp165.f2;
goto _LL164; _LL164: { struct Cyc_List_List* _temp170= tqs0; goto _LL171; _LL171:
for( 0; _temp170 != 0? _temp166 != 0: 0;( _temp170=(( struct Cyc_List_List*)
_check_null( _temp170))->tl, _temp166=(( struct Cyc_List_List*) _check_null(
_temp166))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp170))->hd)).f2,( void*)(( struct Cyc_List_List*) _check_null(
_temp166))->hd)){ break;}} if( _temp170 == 0? _temp166 == 0: 0){ return _temp168;}}}}{
struct _tagged_arr* x=({ struct _tagged_arr* _temp187=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp187[ 0]=( struct _tagged_arr)
xprintf("_tuple%d", Cyc_Toc_tuple_type_counter ++); _temp187;}); struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); struct Cyc_List_List* _temp172= 0;
goto _LL173; _LL173: { struct Cyc_List_List* ts2= ts;{ int i= 1; for( 0; ts2 !=
0;( ts2=(( struct Cyc_List_List*) _check_null( ts2))->tl, i ++)){ _temp172=({
struct Cyc_List_List* _temp174=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp174->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp175=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp175->name= Cyc_Toc_fieldname( i); _temp175->tq= Cyc_Toc_mt_tq; _temp175->type=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( ts2))->hd); _temp175->width=
0; _temp175->attributes= 0; _temp175;}); _temp174->tl= _temp172; _temp174;});}}
_temp172=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp172);{ struct Cyc_Absyn_Structdecl* _temp180=({ struct Cyc_Absyn_Structdecl*
_temp176=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp176->sc=( void*)(( void*) Cyc_Absyn_Public); _temp176->name=({ struct Cyc_Core_Opt*
_temp178=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp178->v=( void*)({ struct _tuple0* _temp179=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp179->f1= Cyc_Toc_rel_ns; _temp179->f2= x;
_temp179;}); _temp178;}); _temp176->tvs= 0; _temp176->fields=({ struct Cyc_Core_Opt*
_temp177=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp177->v=( void*) _temp172; _temp177;}); _temp176->attributes= 0; _temp176;});
goto _LL181; _LL181: Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp182=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp182->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp183=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp183[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp184; _temp184.tag= Cyc_Absyn_Struct_d;
_temp184.f1= _temp180; _temp184;}); _temp183;}), 0); _temp182->tl= Cyc_Toc_result_decls;
_temp182;}); Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp185=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp185->hd=( void*)({ struct
_tuple5* _temp186=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp186->f1= x; _temp186->f2= ts; _temp186;}); _temp185->tl= Cyc_Toc_tuple_types;
_temp185;}); return x;}}}} struct _tuple0* Cyc_Toc_temp_var(){ return({ struct
_tuple0* _temp188=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp188->f1=( void*) Cyc_Absyn_Loc_n; _temp188->f2=({ struct _tagged_arr*
_temp189=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp189[ 0]=( struct _tagged_arr) xprintf("_temp%d", Cyc_Toc_temp_var_counter
++); _temp189;}); _temp188;});} static struct _tagged_arr* Cyc_Toc_fresh_label(){
return({ struct _tagged_arr* _temp190=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)); _temp190[ 0]=( struct _tagged_arr) xprintf("_LL%d", Cyc_Toc_temp_var_counter
++); _temp190;});} static void* Cyc_Toc_typ_to_c( void* t); static struct
_tuple1* Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp193; struct Cyc_Absyn_Tqual
_temp195; struct Cyc_Core_Opt* _temp197; struct _tuple1 _temp191=* a; _LL198:
_temp197= _temp191.f1; goto _LL196; _LL196: _temp195= _temp191.f2; goto _LL194;
_LL194: _temp193= _temp191.f3; goto _LL192; _LL192: return({ struct _tuple1*
_temp199=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp199->f1=
_temp197; _temp199->f2= _temp195; _temp199->f3= Cyc_Toc_typ_to_c( _temp193);
_temp199;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp202; struct Cyc_Absyn_Tqual _temp204; struct _tuple3 _temp200=* x;
_LL205: _temp204= _temp200.f1; goto _LL203; _LL203: _temp202= _temp200.f2; goto
_LL201; _LL201: return({ struct _tuple3* _temp206=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp206->f1= _temp204; _temp206->f2= Cyc_Toc_typ_to_c(
_temp202); _temp206;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp207= t; struct Cyc_Absyn_Exp* _temp215; struct Cyc_Absyn_Tqual _temp217;
void* _temp219; struct Cyc_Core_Opt* _temp221; int _temp223; struct Cyc_Core_Opt*
_temp225; struct Cyc_Core_Opt _temp227; void* _temp228; struct Cyc_Core_Opt*
_temp230; _LL209: if(( unsigned int) _temp207 > 4u?*(( int*) _temp207) == Cyc_Absyn_ArrayType:
0){ _LL220: _temp219=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp207)->f1;
goto _LL218; _LL218: _temp217=(( struct Cyc_Absyn_ArrayType_struct*) _temp207)->f2;
goto _LL216; _LL216: _temp215=(( struct Cyc_Absyn_ArrayType_struct*) _temp207)->f3;
goto _LL210;} else{ goto _LL211;} _LL211: if(( unsigned int) _temp207 > 4u?*((
int*) _temp207) == Cyc_Absyn_Evar: 0){ _LL231: _temp230=(( struct Cyc_Absyn_Evar_struct*)
_temp207)->f1; goto _LL226; _LL226: _temp225=(( struct Cyc_Absyn_Evar_struct*)
_temp207)->f2; if( _temp225 == 0){ goto _LL213;} else{ _temp227=* _temp225;
_LL229: _temp228=( void*) _temp227.v; goto _LL224;} _LL224: _temp223=(( struct
Cyc_Absyn_Evar_struct*) _temp207)->f3; goto _LL222; _LL222: _temp221=(( struct
Cyc_Absyn_Evar_struct*) _temp207)->f4; goto _LL212;} else{ goto _LL213;} _LL213:
goto _LL214; _LL210: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp232=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp232[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp233; _temp233.tag= Cyc_Absyn_ArrayType;
_temp233.f1=( void*) Cyc_Toc_typ_to_c_array( _temp219); _temp233.f2= _temp217;
_temp233.f3= _temp215; _temp233;}); _temp232;}); _LL212: return Cyc_Toc_typ_to_c_array(
_temp228); _LL214: return Cyc_Toc_typ_to_c( t); _LL208:;} static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c( struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield*
_temp234=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp234->name= f->name; _temp234->tq= f->tq; _temp234->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp234->width= f->width; _temp234->attributes= f->attributes;
_temp234;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp235= t; struct
Cyc_Core_Opt* _temp287; int _temp289; struct Cyc_Core_Opt* _temp291; struct Cyc_Core_Opt*
_temp293; struct Cyc_Core_Opt* _temp295; int _temp297; struct Cyc_Core_Opt*
_temp299; struct Cyc_Core_Opt _temp301; void* _temp302; struct Cyc_Core_Opt*
_temp304; struct Cyc_Absyn_Tvar* _temp306; struct Cyc_Absyn_TunionInfo _temp308;
void* _temp310; struct Cyc_List_List* _temp312; void* _temp314; struct Cyc_Absyn_Tuniondecl*
_temp316; struct Cyc_Absyn_TunionInfo _temp318; struct Cyc_Absyn_TunionFieldInfo
_temp320; struct Cyc_List_List* _temp322; void* _temp324; struct Cyc_Absyn_Tunionfield*
_temp326; struct Cyc_Absyn_Tuniondecl* _temp328; struct Cyc_Absyn_TunionFieldInfo
_temp330; struct Cyc_Absyn_PtrInfo _temp332; struct Cyc_Absyn_Conref* _temp334;
struct Cyc_Absyn_Tqual _temp336; struct Cyc_Absyn_Conref* _temp338; void*
_temp340; void* _temp342; void* _temp344; void* _temp346; struct Cyc_Absyn_Exp*
_temp348; struct Cyc_Absyn_Tqual _temp350; void* _temp352; struct Cyc_Absyn_FnInfo
_temp354; struct Cyc_List_List* _temp356; struct Cyc_List_List* _temp358; struct
Cyc_Absyn_VarargInfo* _temp360; int _temp362; struct Cyc_List_List* _temp364;
void* _temp366; struct Cyc_Core_Opt* _temp368; struct Cyc_List_List* _temp370;
struct Cyc_List_List* _temp372; struct Cyc_List_List* _temp374; struct Cyc_List_List*
_temp376; struct Cyc_Absyn_Uniondecl** _temp378; struct Cyc_List_List* _temp380;
struct _tuple0* _temp382; struct Cyc_Absyn_Structdecl** _temp384; struct Cyc_List_List*
_temp386; struct _tuple0* _temp388; struct Cyc_Absyn_Enumdecl* _temp390; struct
_tuple0* _temp392; struct Cyc_Core_Opt* _temp394; struct Cyc_List_List* _temp396;
struct _tuple0* _temp398; void* _temp400; void* _temp402; struct Cyc_List_List*
_temp404; _LL237: if( _temp235 ==( void*) Cyc_Absyn_VoidType){ goto _LL238;}
else{ goto _LL239;} _LL239: if(( unsigned int) _temp235 > 4u?*(( int*) _temp235)
== Cyc_Absyn_Evar: 0){ _LL294: _temp293=(( struct Cyc_Absyn_Evar_struct*)
_temp235)->f1; goto _LL292; _LL292: _temp291=(( struct Cyc_Absyn_Evar_struct*)
_temp235)->f2; if( _temp291 == 0){ goto _LL290;} else{ goto _LL241;} _LL290:
_temp289=(( struct Cyc_Absyn_Evar_struct*) _temp235)->f3; goto _LL288; _LL288:
_temp287=(( struct Cyc_Absyn_Evar_struct*) _temp235)->f4; goto _LL240;} else{
goto _LL241;} _LL241: if(( unsigned int) _temp235 > 4u?*(( int*) _temp235) ==
Cyc_Absyn_Evar: 0){ _LL305: _temp304=(( struct Cyc_Absyn_Evar_struct*) _temp235)->f1;
goto _LL300; _LL300: _temp299=(( struct Cyc_Absyn_Evar_struct*) _temp235)->f2;
if( _temp299 == 0){ goto _LL243;} else{ _temp301=* _temp299; _LL303: _temp302=(
void*) _temp301.v; goto _LL298;} _LL298: _temp297=(( struct Cyc_Absyn_Evar_struct*)
_temp235)->f3; goto _LL296; _LL296: _temp295=(( struct Cyc_Absyn_Evar_struct*)
_temp235)->f4; goto _LL242;} else{ goto _LL243;} _LL243: if(( unsigned int)
_temp235 > 4u?*(( int*) _temp235) == Cyc_Absyn_VarType: 0){ _LL307: _temp306=((
struct Cyc_Absyn_VarType_struct*) _temp235)->f1; goto _LL244;} else{ goto _LL245;}
_LL245: if(( unsigned int) _temp235 > 4u?*(( int*) _temp235) == Cyc_Absyn_TunionType:
0){ _LL309: _temp308=(( struct Cyc_Absyn_TunionType_struct*) _temp235)->f1;
_LL315: _temp314=( void*) _temp308.tunion_info; if(*(( int*) _temp314) == Cyc_Absyn_KnownTunion){
_LL317: _temp316=(( struct Cyc_Absyn_KnownTunion_struct*) _temp314)->f1; goto
_LL313;} else{ goto _LL247;} _LL313: _temp312= _temp308.targs; goto _LL311;
_LL311: _temp310=( void*) _temp308.rgn; goto _LL246;} else{ goto _LL247;} _LL247:
if(( unsigned int) _temp235 > 4u?*(( int*) _temp235) == Cyc_Absyn_TunionType: 0){
_LL319: _temp318=(( struct Cyc_Absyn_TunionType_struct*) _temp235)->f1; goto
_LL248;} else{ goto _LL249;} _LL249: if(( unsigned int) _temp235 > 4u?*(( int*)
_temp235) == Cyc_Absyn_TunionFieldType: 0){ _LL321: _temp320=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp235)->f1; _LL325: _temp324=( void*) _temp320.field_info; if(*(( int*)
_temp324) == Cyc_Absyn_KnownTunionfield){ _LL329: _temp328=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp324)->f1; goto _LL327; _LL327: _temp326=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp324)->f2; goto _LL323;} else{ goto _LL251;} _LL323: _temp322= _temp320.targs;
goto _LL250;} else{ goto _LL251;} _LL251: if(( unsigned int) _temp235 > 4u?*((
int*) _temp235) == Cyc_Absyn_TunionFieldType: 0){ _LL331: _temp330=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp235)->f1; goto _LL252;} else{ goto _LL253;} _LL253: if(( unsigned int)
_temp235 > 4u?*(( int*) _temp235) == Cyc_Absyn_PointerType: 0){ _LL333: _temp332=((
struct Cyc_Absyn_PointerType_struct*) _temp235)->f1; _LL343: _temp342=( void*)
_temp332.elt_typ; goto _LL341; _LL341: _temp340=( void*) _temp332.rgn_typ; goto
_LL339; _LL339: _temp338= _temp332.nullable; goto _LL337; _LL337: _temp336=
_temp332.tq; goto _LL335; _LL335: _temp334= _temp332.bounds; goto _LL254;} else{
goto _LL255;} _LL255: if(( unsigned int) _temp235 > 4u?*(( int*) _temp235) ==
Cyc_Absyn_IntType: 0){ _LL347: _temp346=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp235)->f1; goto _LL345; _LL345: _temp344=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp235)->f2; goto _LL256;} else{ goto _LL257;} _LL257: if( _temp235 ==( void*)
Cyc_Absyn_FloatType){ goto _LL258;} else{ goto _LL259;} _LL259: if( _temp235 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL260;} else{ goto _LL261;} _LL261: if((
unsigned int) _temp235 > 4u?*(( int*) _temp235) == Cyc_Absyn_ArrayType: 0){
_LL353: _temp352=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp235)->f1;
goto _LL351; _LL351: _temp350=(( struct Cyc_Absyn_ArrayType_struct*) _temp235)->f2;
goto _LL349; _LL349: _temp348=(( struct Cyc_Absyn_ArrayType_struct*) _temp235)->f3;
goto _LL262;} else{ goto _LL263;} _LL263: if(( unsigned int) _temp235 > 4u?*((
int*) _temp235) == Cyc_Absyn_FnType: 0){ _LL355: _temp354=(( struct Cyc_Absyn_FnType_struct*)
_temp235)->f1; _LL371: _temp370= _temp354.tvars; goto _LL369; _LL369: _temp368=
_temp354.effect; goto _LL367; _LL367: _temp366=( void*) _temp354.ret_typ; goto
_LL365; _LL365: _temp364= _temp354.args; goto _LL363; _LL363: _temp362= _temp354.c_varargs;
goto _LL361; _LL361: _temp360= _temp354.cyc_varargs; goto _LL359; _LL359:
_temp358= _temp354.rgn_po; goto _LL357; _LL357: _temp356= _temp354.attributes;
goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int) _temp235 > 4u?*((
int*) _temp235) == Cyc_Absyn_TupleType: 0){ _LL373: _temp372=(( struct Cyc_Absyn_TupleType_struct*)
_temp235)->f1; goto _LL266;} else{ goto _LL267;} _LL267: if(( unsigned int)
_temp235 > 4u?*(( int*) _temp235) == Cyc_Absyn_AnonStructType: 0){ _LL375:
_temp374=(( struct Cyc_Absyn_AnonStructType_struct*) _temp235)->f1; goto _LL268;}
else{ goto _LL269;} _LL269: if(( unsigned int) _temp235 > 4u?*(( int*) _temp235)
== Cyc_Absyn_AnonUnionType: 0){ _LL377: _temp376=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp235)->f1; goto _LL270;} else{ goto _LL271;} _LL271: if(( unsigned int)
_temp235 > 4u?*(( int*) _temp235) == Cyc_Absyn_UnionType: 0){ _LL383: _temp382=((
struct Cyc_Absyn_UnionType_struct*) _temp235)->f1; goto _LL381; _LL381: _temp380=((
struct Cyc_Absyn_UnionType_struct*) _temp235)->f2; goto _LL379; _LL379: _temp378=((
struct Cyc_Absyn_UnionType_struct*) _temp235)->f3; goto _LL272;} else{ goto
_LL273;} _LL273: if(( unsigned int) _temp235 > 4u?*(( int*) _temp235) == Cyc_Absyn_StructType:
0){ _LL389: _temp388=(( struct Cyc_Absyn_StructType_struct*) _temp235)->f1; goto
_LL387; _LL387: _temp386=(( struct Cyc_Absyn_StructType_struct*) _temp235)->f2;
goto _LL385; _LL385: _temp384=(( struct Cyc_Absyn_StructType_struct*) _temp235)->f3;
goto _LL274;} else{ goto _LL275;} _LL275: if(( unsigned int) _temp235 > 4u?*((
int*) _temp235) == Cyc_Absyn_EnumType: 0){ _LL393: _temp392=(( struct Cyc_Absyn_EnumType_struct*)
_temp235)->f1; goto _LL391; _LL391: _temp390=(( struct Cyc_Absyn_EnumType_struct*)
_temp235)->f2; goto _LL276;} else{ goto _LL277;} _LL277: if(( unsigned int)
_temp235 > 4u?*(( int*) _temp235) == Cyc_Absyn_TypedefType: 0){ _LL399: _temp398=((
struct Cyc_Absyn_TypedefType_struct*) _temp235)->f1; goto _LL397; _LL397:
_temp396=(( struct Cyc_Absyn_TypedefType_struct*) _temp235)->f2; goto _LL395;
_LL395: _temp394=(( struct Cyc_Absyn_TypedefType_struct*) _temp235)->f3; goto
_LL278;} else{ goto _LL279;} _LL279: if(( unsigned int) _temp235 > 4u?*(( int*)
_temp235) == Cyc_Absyn_RgnHandleType: 0){ _LL401: _temp400=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp235)->f1; goto _LL280;} else{ goto _LL281;} _LL281: if( _temp235 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL282;} else{ goto _LL283;} _LL283: if(( unsigned int)
_temp235 > 4u?*(( int*) _temp235) == Cyc_Absyn_AccessEff: 0){ _LL403: _temp402=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp235)->f1; goto _LL284;} else{
goto _LL285;} _LL285: if(( unsigned int) _temp235 > 4u?*(( int*) _temp235) ==
Cyc_Absyn_JoinEff: 0){ _LL405: _temp404=(( struct Cyc_Absyn_JoinEff_struct*)
_temp235)->f1; goto _LL286;} else{ goto _LL236;} _LL238: return t; _LL240:
return Cyc_Absyn_sint_t; _LL242: return Cyc_Toc_typ_to_c( _temp302); _LL244:
return Cyc_Absyn_void_star_typ(); _LL246: return Cyc_Absyn_void_star_typ();
_LL248:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("unresolved TunionType", sizeof( unsigned char), 22u))); _LL250: if(
_temp326->typs == 0){ if( _temp328->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp326->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL252:( int) _throw((( void*(*)(
struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u))); _LL254: _temp342= Cyc_Toc_typ_to_c_array(
_temp342);{ void* _temp406=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp334))->v; void* _temp412; _LL408: if((
unsigned int) _temp406 > 1u?*(( int*) _temp406) == Cyc_Absyn_Eq_constr: 0){
_LL413: _temp412=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp406)->f1;
if( _temp412 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL409;} else{ goto _LL410;}}
else{ goto _LL410;} _LL410: goto _LL411; _LL409: return Cyc_Toc_tagged_arr_typ;
_LL411: return Cyc_Absyn_star_typ( _temp342,( void*) Cyc_Absyn_HeapRgn, _temp336);
_LL407:;} _LL256: return t; _LL258: return t; _LL260: return t; _LL262: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp414=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp414[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp415; _temp415.tag= Cyc_Absyn_ArrayType; _temp415.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp352); _temp415.f2= _temp350; _temp415.f3=
_temp348; _temp415;}); _temp414;}); _LL264: { struct Cyc_List_List* _temp416=
_temp356; goto _LL417; _LL417: { struct Cyc_List_List* _temp418= 0; goto _LL419;
_LL419: for( 0; _temp356 != 0; _temp356=(( struct Cyc_List_List*) _check_null(
_temp356))->tl){ void* _temp420=( void*)(( struct Cyc_List_List*) _check_null(
_temp356))->hd; _LL422: if( _temp420 ==( void*) Cyc_Absyn_Noreturn_att){ goto
_LL423;} else{ goto _LL424;} _LL424: if( _temp420 ==( void*) Cyc_Absyn_Const_att){
goto _LL425;} else{ goto _LL426;} _LL426: goto _LL427; _LL423: goto _LL425;
_LL425: continue; _LL427: _temp418=({ struct Cyc_List_List* _temp428=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp428->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( _temp356))->hd); _temp428->tl=
_temp418; _temp428;}); goto _LL421; _LL421:;}{ struct Cyc_List_List* _temp429=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp364); goto _LL430; _LL430: if(
_temp360 != 0){ void* _temp431= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp360))->type,( void*)(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp360))->rgn, Cyc_Toc_mt_tq)); goto
_LL432; _LL432: { struct _tuple1* _temp434=({ struct _tuple1* _temp433=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp433->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp360))->name; _temp433->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp360))->tq; _temp433->f3= _temp431; _temp433;}); goto _LL435;
_LL435: _temp429=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_imp_append)( _temp429,({ struct Cyc_List_List*
_temp436=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp436->hd=( void*) _temp434; _temp436->tl= 0; _temp436;}));}} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp437=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp437[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp438; _temp438.tag= Cyc_Absyn_FnType; _temp438.f1=({ struct Cyc_Absyn_FnInfo
_temp439; _temp439.tvars= 0; _temp439.effect= 0; _temp439.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp366); _temp439.args= _temp429; _temp439.c_varargs= _temp362; _temp439.cyc_varargs=
0; _temp439.rgn_po= 0; _temp439.attributes= _temp418; _temp439;}); _temp438;});
_temp437;});}}} _LL266: _temp372=(( struct Cyc_List_List*(*)( struct _tuple3*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp372);{ struct _tagged_arr* _temp440= Cyc_Toc_add_tuple_type( _temp372);
goto _LL441; _LL441: return Cyc_Absyn_strct( _temp440);} _LL268: return( void*)({
struct Cyc_Absyn_AnonStructType_struct* _temp442=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp442[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp443; _temp443.tag= Cyc_Absyn_AnonStructType;
_temp443.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp374); _temp443;}); _temp442;}); _LL270: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp444=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp444[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp445; _temp445.tag=
Cyc_Absyn_AnonUnionType; _temp445.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp376); _temp445;}); _temp444;}); _LL272: if( _temp382 == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous union", sizeof(
unsigned char), 16u));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp382)); _LL274: if( _temp388 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous struct", sizeof(
unsigned char), 17u));} return Cyc_Absyn_strctq(( struct _tuple0*) _check_null(
_temp388)); _LL276: return t; _LL278: if( _temp394 == 0){ return(( void*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: unresolved TypedefType",
sizeof( unsigned char), 38u));} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp446=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp446[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp447; _temp447.tag= Cyc_Absyn_TypedefType;
_temp447.f1= _temp398; _temp447.f2= 0; _temp447.f3=({ struct Cyc_Core_Opt*
_temp448=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp448->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp394))->v); _temp448;}); _temp447;}); _temp446;}); _LL280:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL282: return(( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u)); _LL284: return(( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u)); _LL286: return(( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u)); _LL236:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp449= t; struct Cyc_Absyn_Exp* _temp455; struct Cyc_Absyn_Tqual _temp457;
void* _temp459; _LL451: if(( unsigned int) _temp449 > 4u?*(( int*) _temp449) ==
Cyc_Absyn_ArrayType: 0){ _LL460: _temp459=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp449)->f1; goto _LL458; _LL458: _temp457=(( struct Cyc_Absyn_ArrayType_struct*)
_temp449)->f2; goto _LL456; _LL456: _temp455=(( struct Cyc_Absyn_ArrayType_struct*)
_temp449)->f3; goto _LL452;} else{ goto _LL453;} _LL453: goto _LL454; _LL452:
return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ( _temp459,( void*) Cyc_Absyn_HeapRgn,
_temp457), e, l); _LL454: return Cyc_Absyn_cast_exp( t, e, l); _LL450:;} static
int Cyc_Toc_atomic_typ( void* t){ void* _temp461= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp497; void* _temp499; void* _temp501; struct Cyc_Absyn_FnInfo
_temp503; struct Cyc_Absyn_Exp* _temp505; struct Cyc_Absyn_Tqual _temp507; void*
_temp509; struct Cyc_List_List* _temp511; struct Cyc_Absyn_Structdecl** _temp513;
struct Cyc_List_List* _temp515; struct _tuple0* _temp517; struct Cyc_Absyn_TunionInfo
_temp519; struct Cyc_List_List* _temp521; struct Cyc_List_List* _temp523; struct
Cyc_Absyn_TunionFieldInfo _temp525; struct Cyc_List_List* _temp527; void*
_temp529; struct Cyc_Absyn_Tunionfield* _temp531; struct Cyc_Absyn_Tuniondecl*
_temp533; struct Cyc_Absyn_Uniondecl** _temp535; struct Cyc_List_List* _temp537;
struct _tuple0* _temp539; struct Cyc_Absyn_PtrInfo _temp541; void* _temp543;
_LL463: if( _temp461 ==( void*) Cyc_Absyn_VoidType){ goto _LL464;} else{ goto
_LL465;} _LL465: if(( unsigned int) _temp461 > 4u?*(( int*) _temp461) == Cyc_Absyn_VarType:
0){ _LL498: _temp497=(( struct Cyc_Absyn_VarType_struct*) _temp461)->f1; goto
_LL466;} else{ goto _LL467;} _LL467: if(( unsigned int) _temp461 > 4u?*(( int*)
_temp461) == Cyc_Absyn_IntType: 0){ _LL502: _temp501=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp461)->f1; goto _LL500; _LL500: _temp499=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp461)->f2; goto _LL468;} else{ goto _LL469;} _LL469: if( _temp461 ==( void*)
Cyc_Absyn_FloatType){ goto _LL470;} else{ goto _LL471;} _LL471: if( _temp461 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL472;} else{ goto _LL473;} _LL473: if((
unsigned int) _temp461 > 4u?*(( int*) _temp461) == Cyc_Absyn_FnType: 0){ _LL504:
_temp503=(( struct Cyc_Absyn_FnType_struct*) _temp461)->f1; goto _LL474;} else{
goto _LL475;} _LL475: if(( unsigned int) _temp461 > 4u?*(( int*) _temp461) ==
Cyc_Absyn_ArrayType: 0){ _LL510: _temp509=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp461)->f1; goto _LL508; _LL508: _temp507=(( struct Cyc_Absyn_ArrayType_struct*)
_temp461)->f2; goto _LL506; _LL506: _temp505=(( struct Cyc_Absyn_ArrayType_struct*)
_temp461)->f3; goto _LL476;} else{ goto _LL477;} _LL477: if(( unsigned int)
_temp461 > 4u?*(( int*) _temp461) == Cyc_Absyn_TupleType: 0){ _LL512: _temp511=((
struct Cyc_Absyn_TupleType_struct*) _temp461)->f1; goto _LL478;} else{ goto
_LL479;} _LL479: if(( unsigned int) _temp461 > 4u?*(( int*) _temp461) == Cyc_Absyn_StructType:
0){ _LL518: _temp517=(( struct Cyc_Absyn_StructType_struct*) _temp461)->f1; goto
_LL516; _LL516: _temp515=(( struct Cyc_Absyn_StructType_struct*) _temp461)->f2;
goto _LL514; _LL514: _temp513=(( struct Cyc_Absyn_StructType_struct*) _temp461)->f3;
goto _LL480;} else{ goto _LL481;} _LL481: if(( unsigned int) _temp461 > 4u?*((
int*) _temp461) == Cyc_Absyn_TunionType: 0){ _LL520: _temp519=(( struct Cyc_Absyn_TunionType_struct*)
_temp461)->f1; goto _LL482;} else{ goto _LL483;} _LL483: if(( unsigned int)
_temp461 > 4u?*(( int*) _temp461) == Cyc_Absyn_AnonStructType: 0){ _LL522:
_temp521=(( struct Cyc_Absyn_AnonStructType_struct*) _temp461)->f1; goto _LL484;}
else{ goto _LL485;} _LL485: if(( unsigned int) _temp461 > 4u?*(( int*) _temp461)
== Cyc_Absyn_AnonUnionType: 0){ _LL524: _temp523=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp461)->f1; goto _LL486;} else{ goto _LL487;} _LL487: if(( unsigned int)
_temp461 > 4u?*(( int*) _temp461) == Cyc_Absyn_TunionFieldType: 0){ _LL526:
_temp525=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp461)->f1; _LL530:
_temp529=( void*) _temp525.field_info; if(*(( int*) _temp529) == Cyc_Absyn_KnownTunionfield){
_LL534: _temp533=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp529)->f1;
goto _LL532; _LL532: _temp531=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp529)->f2; goto _LL528;} else{ goto _LL489;} _LL528: _temp527= _temp525.targs;
goto _LL488;} else{ goto _LL489;} _LL489: if(( unsigned int) _temp461 > 4u?*((
int*) _temp461) == Cyc_Absyn_UnionType: 0){ _LL540: _temp539=(( struct Cyc_Absyn_UnionType_struct*)
_temp461)->f1; goto _LL538; _LL538: _temp537=(( struct Cyc_Absyn_UnionType_struct*)
_temp461)->f2; goto _LL536; _LL536: _temp535=(( struct Cyc_Absyn_UnionType_struct*)
_temp461)->f3; goto _LL490;} else{ goto _LL491;} _LL491: if(( unsigned int)
_temp461 > 4u?*(( int*) _temp461) == Cyc_Absyn_PointerType: 0){ _LL542: _temp541=((
struct Cyc_Absyn_PointerType_struct*) _temp461)->f1; goto _LL492;} else{ goto
_LL493;} _LL493: if(( unsigned int) _temp461 > 4u?*(( int*) _temp461) == Cyc_Absyn_RgnHandleType:
0){ _LL544: _temp543=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp461)->f1;
goto _LL494;} else{ goto _LL495;} _LL495: goto _LL496; _LL464: return 1; _LL466:
return 0; _LL468: return 1; _LL470: return 1; _LL472: return 1; _LL474: return 1;
_LL476: return Cyc_Toc_atomic_typ( _temp509); _LL478: for( 0; _temp511 != 0;
_temp511=(( struct Cyc_List_List*) _check_null( _temp511))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp511))->hd)).f2)){
return 0;}} return 1; _LL480: return 0; _LL482: return 0; _LL484: _temp523=
_temp521; goto _LL486; _LL486: for( 0; _temp523 != 0; _temp523=(( struct Cyc_List_List*)
_check_null( _temp523))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp523))->hd)->type)){ return 0;}} return
1; _LL488: return _temp531->typs == 0; _LL490: return 0; _LL492: return 0;
_LL494: return 0; _LL496:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp545= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _get_arr_size( _temp545, 1u), _temp545.curr);}));
return 0; _LL462:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp546=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp552; struct Cyc_Absyn_Conref*
_temp554; struct Cyc_Absyn_Tqual _temp556; struct Cyc_Absyn_Conref* _temp558;
void* _temp560; void* _temp562; _LL548: if(( unsigned int) _temp546 > 4u?*(( int*)
_temp546) == Cyc_Absyn_PointerType: 0){ _LL553: _temp552=(( struct Cyc_Absyn_PointerType_struct*)
_temp546)->f1; _LL563: _temp562=( void*) _temp552.elt_typ; goto _LL561; _LL561:
_temp560=( void*) _temp552.rgn_typ; goto _LL559; _LL559: _temp558= _temp552.nullable;
goto _LL557; _LL557: _temp556= _temp552.tq; goto _LL555; _LL555: _temp554=
_temp552.bounds; goto _LL549;} else{ goto _LL550;} _LL550: goto _LL551; _LL549: {
void* _temp564= Cyc_Tcutil_compress( _temp562); _LL566: if( _temp564 ==( void*)
Cyc_Absyn_VoidType){ goto _LL567;} else{ goto _LL568;} _LL568: goto _LL569;
_LL567: return 1; _LL569: return 0; _LL565:;} _LL551: return 0; _LL547:;} static
int Cyc_Toc_is_poly_field( void* t, struct _tagged_arr* f){ void* _temp570= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Structdecl** _temp582; struct Cyc_Absyn_Structdecl*
_temp584; struct Cyc_List_List* _temp585; struct _tuple0* _temp587; struct Cyc_List_List*
_temp589; struct Cyc_List_List* _temp591; struct Cyc_Absyn_Uniondecl** _temp593;
struct Cyc_List_List* _temp595; struct _tuple0* _temp597; _LL572: if((
unsigned int) _temp570 > 4u?*(( int*) _temp570) == Cyc_Absyn_StructType: 0){
_LL588: _temp587=(( struct Cyc_Absyn_StructType_struct*) _temp570)->f1; goto
_LL586; _LL586: _temp585=(( struct Cyc_Absyn_StructType_struct*) _temp570)->f2;
goto _LL583; _LL583: _temp582=(( struct Cyc_Absyn_StructType_struct*) _temp570)->f3;
if( _temp582 == 0){ goto _LL574;} else{ _temp584=* _temp582; goto _LL573;}}
else{ goto _LL574;} _LL574: if(( unsigned int) _temp570 > 4u?*(( int*) _temp570)
== Cyc_Absyn_AnonStructType: 0){ _LL590: _temp589=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp570)->f1; goto _LL575;} else{ goto _LL576;} _LL576: if(( unsigned int)
_temp570 > 4u?*(( int*) _temp570) == Cyc_Absyn_AnonUnionType: 0){ _LL592:
_temp591=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp570)->f1; goto _LL577;}
else{ goto _LL578;} _LL578: if(( unsigned int) _temp570 > 4u?*(( int*) _temp570)
== Cyc_Absyn_UnionType: 0){ _LL598: _temp597=(( struct Cyc_Absyn_UnionType_struct*)
_temp570)->f1; goto _LL596; _LL596: _temp595=(( struct Cyc_Absyn_UnionType_struct*)
_temp570)->f2; goto _LL594; _LL594: _temp593=(( struct Cyc_Absyn_UnionType_struct*)
_temp570)->f3; goto _LL579;} else{ goto _LL580;} _LL580: goto _LL581; _LL573:
if( _temp584->fields == 0){( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: struct missing fields", sizeof( unsigned char), 37u)));}
_temp589=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp584->fields))->v;
goto _LL575; _LL575: { struct Cyc_Absyn_Structfield* _temp599= Cyc_Absyn_lookup_field(
_temp589, f); goto _LL600; _LL600: if( _temp599 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp601=*(( struct Cyc_Absyn_Structfield*) _check_null( _temp599))->name;
xprintf("is_poly_field: bad field %.*s", _get_arr_size( _temp601, 1u), _temp601.curr);}));}
return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp599))->type);} _LL577: return 0; _LL579: return 0; _LL581:(
int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct
_tagged_arr)({ struct _tagged_arr _temp602= Cyc_Absynpp_typ2string( t); xprintf("is_poly_field: bad type %.*s",
_get_arr_size( _temp602, 1u), _temp602.curr);}))); _LL571:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp603=( void*) e->r; struct _tagged_arr*
_temp611; struct Cyc_Absyn_Exp* _temp613; struct _tagged_arr* _temp615; struct
Cyc_Absyn_Exp* _temp617; _LL605: if(*(( int*) _temp603) == Cyc_Absyn_StructMember_e){
_LL614: _temp613=(( struct Cyc_Absyn_StructMember_e_struct*) _temp603)->f1; goto
_LL612; _LL612: _temp611=(( struct Cyc_Absyn_StructMember_e_struct*) _temp603)->f2;
goto _LL606;} else{ goto _LL607;} _LL607: if(*(( int*) _temp603) == Cyc_Absyn_StructArrow_e){
_LL618: _temp617=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp603)->f1; goto
_LL616; _LL616: _temp615=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp603)->f2;
goto _LL608;} else{ goto _LL609;} _LL609: goto _LL610; _LL606: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp613->topt))->v, _temp611);
_LL608: { void* _temp619= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp617->topt))->v); struct Cyc_Absyn_PtrInfo _temp625; struct Cyc_Absyn_Conref*
_temp627; struct Cyc_Absyn_Tqual _temp629; struct Cyc_Absyn_Conref* _temp631;
void* _temp633; void* _temp635; _LL621: if(( unsigned int) _temp619 > 4u?*(( int*)
_temp619) == Cyc_Absyn_PointerType: 0){ _LL626: _temp625=(( struct Cyc_Absyn_PointerType_struct*)
_temp619)->f1; _LL636: _temp635=( void*) _temp625.elt_typ; goto _LL634; _LL634:
_temp633=( void*) _temp625.rgn_typ; goto _LL632; _LL632: _temp631= _temp625.nullable;
goto _LL630; _LL630: _temp629= _temp625.tq; goto _LL628; _LL628: _temp627=
_temp625.bounds; goto _LL622;} else{ goto _LL623;} _LL623: goto _LL624; _LL622:
return Cyc_Toc_is_poly_field( _temp635, _temp615); _LL624:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp637= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp617->topt))->v); xprintf("is_poly_project:  bad type %.*s", _get_arr_size(
_temp637, 1u), _temp637.curr);})); return 0; _LL620:;} _LL610: return 0; _LL604:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_e,({ struct Cyc_List_List*
_temp638=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp638->hd=( void*) s; _temp638->tl= 0; _temp638;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc_GC_malloc_atomic_e,({ struct Cyc_List_List* _temp639=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp639->hd=( void*) s; _temp639->tl=
0; _temp639;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t,
struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic(
s);} return Cyc_Toc_malloc_ptr( s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({ struct Cyc_List_List*(* _temp640)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp643= rgn; struct Cyc_Absyn_Exp* _temp644= s; struct
Cyc_Absyn_Exp* _temp641[ 2u]={ _temp643, _temp644}; struct _tagged_arr _temp642={(
void*) _temp641,( void*) _temp641,( void*)( _temp641 + 2u)}; _temp640( _temp642);}),
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Toc__sfile_to_file_e,({ struct Cyc_List_List*
_temp645=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp645->hd=( void*) e; _temp645->tl= 0; _temp645;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({
struct Cyc_List_List* _temp646=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp646->hd=( void*) e; _temp646->tl= 0; _temp646;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_toplevel_tagged_arr( void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ int is_string= 0;{ void* _temp647=(
void*) e->r; void* _temp653; struct _tagged_arr _temp655; _LL649: if(*(( int*)
_temp647) == Cyc_Absyn_Const_e){ _LL654: _temp653=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp647)->f1; if(( unsigned int) _temp653 > 1u?*(( int*) _temp653) == Cyc_Absyn_String_c:
0){ _LL656: _temp655=(( struct Cyc_Absyn_String_c_struct*) _temp653)->f1; goto
_LL650;} else{ goto _LL651;}} else{ goto _LL651;} _LL651: goto _LL652; _LL650:
is_string= 1; goto _LL648; _LL652: goto _LL648; _LL648:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp660=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp660[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp661; _temp661.tag= Cyc_Absyn_ArrayType; _temp661.f1=(
void*) Cyc_Absyn_uchar_t; _temp661.f2= Cyc_Toc_mt_tq; _temp661.f3=( struct Cyc_Absyn_Exp*)
sz; _temp661;}); _temp660;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp657=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp657->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp658=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp658[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp659; _temp659.tag= Cyc_Absyn_Var_d;
_temp659.f1= vd; _temp659;}); _temp658;}), 0); _temp657->tl= Cyc_Toc_result_decls;
_temp657;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct Cyc_List_List*(* _temp662)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct _tuple4* _temp665=({ struct _tuple4*
_temp670=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp670->f1= 0;
_temp670->f2= xexp; _temp670;}); struct _tuple4* _temp666=({ struct _tuple4*
_temp669=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp669->f1= 0;
_temp669->f2= xexp; _temp669;}); struct _tuple4* _temp667=({ struct _tuple4*
_temp668=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp668->f1= 0;
_temp668->f2= xplussz; _temp668;}); struct _tuple4* _temp663[ 3u]={ _temp665,
_temp666, _temp667}; struct _tagged_arr _temp664={( void*) _temp663,( void*)
_temp663,( void*)( _temp663 + 3u)}; _temp662( _temp664);}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp671=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp671->break_lab= 0; _temp671->continue_lab= 0; _temp671->fallthru_info=
0; _temp671->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp671->toplevel= 1;
_temp671;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp672=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp672->break_lab= e->break_lab; _temp672->continue_lab=
e->continue_lab; _temp672->fallthru_info= e->fallthru_info; _temp672->varmap= e->varmap;
_temp672->toplevel= e->toplevel; _temp672;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp673=(* x).f1; struct
Cyc_List_List* _temp679; _LL675: if(( unsigned int) _temp673 > 1u?*(( int*)
_temp673) == Cyc_Absyn_Rel_n: 0){ _LL680: _temp679=(( struct Cyc_Absyn_Rel_n_struct*)
_temp673)->f1; goto _LL676;} else{ goto _LL677;} _LL677: goto _LL678; _LL676:(
int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct
_tagged_arr)({ struct _tagged_arr _temp681= Cyc_Absynpp_qvar2string( x); xprintf("Toc::add_varmap on Rel_n: %.*s\n",
_get_arr_size( _temp681, 1u), _temp681.curr);}))); _LL678: goto _LL674; _LL674:;}{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)(
ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=
0; ans->continue_lab= 0; return ans;} struct _tuple6{ struct _tagged_arr* f1;
struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_arr*
break_l, struct _tagged_arr* fallthru_l, struct Cyc_List_List* fallthru_binders,
struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List* _temp682= 0; goto
_LL683; _LL683: for( 0; fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*)
_check_null( fallthru_binders))->tl){ _temp682=({ struct Cyc_List_List* _temp684=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp684->hd=(
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
fallthru_binders))->hd)->name; _temp684->tl= _temp682; _temp684;});} _temp682=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp682);{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt*
_temp685=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp685->v=( void*) break_l; _temp685;}); ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp686=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp686->v=( void*)({ struct _tuple6* _temp687=( struct _tuple6*) GC_malloc(
sizeof( struct _tuple6)); _temp687->f1= fallthru_l; _temp687->f2= _temp682;
_temp687->f3= next_case_env->varmap; _temp687;}); _temp686;}); return ans;}}
static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e,
struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp688=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp688->v=( void*) break_l; _temp688;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_arr* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp689=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp689->v=( void*)({
struct _tuple6* _temp690=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp690->f1= next_l; _temp690->f2= 0; _temp690->f3=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp690;}); _temp689;}); return ans;} struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4;
unsigned char f5; int f6; } ; struct _tuple8{ struct _tuple0* f1; void* f2;
struct Cyc_Absyn_Exp* f3; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("expecting a literal format string", sizeof(
unsigned char), 34u));}{ struct _tagged_arr fmt_str;{ void* _temp691=( void*)((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd)->r;
void* _temp697; struct _tagged_arr _temp699; _LL693: if(*(( int*) _temp691) ==
Cyc_Absyn_Const_e){ _LL698: _temp697=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp691)->f1; if(( unsigned int) _temp697 > 1u?*(( int*) _temp697) == Cyc_Absyn_String_c:
0){ _LL700: _temp699=(( struct Cyc_Absyn_String_c_struct*) _temp697)->f1; goto
_LL694;} else{ goto _LL695;}} else{ goto _LL695;} _LL695: goto _LL696; _LL694:
fmt_str= _temp699; goto _LL692; _LL696: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("expecting a literal format string",
sizeof( unsigned char), 34u)); _LL692:;}{ int len=( int) _get_arr_size( fmt_str,
sizeof( unsigned char)); int i= 0; for( i= 0; i < len; i ++){ if(*(( const
unsigned char*) _check_unknown_subscript( fmt_str, sizeof( unsigned char), i))
=='%'){ i ++; if( i < len?*(( const unsigned char*) _check_unknown_subscript(
fmt_str, sizeof( unsigned char), i)) =='s': 0){ break;}}} if( i == len){ if(
aopt != 0){ args=({ struct Cyc_List_List* _temp701=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp701->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp701->tl= args; _temp701;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp702= _new_region(); struct
_RegionHandle* r=& _temp702; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*)
_check_unknown_subscript( fmt_str, sizeof( unsigned char), i)); rev_fmt=({
struct Cyc_List_List* _temp703=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp703->hd=( void*)(( int) c); _temp703->tl=
rev_fmt; _temp703;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* _temp704=
Cyc_Formatstr_parse_conversionspecification( fmt_str, i + 1); goto _LL705;
_LL705: if( _temp704 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("bad format string", sizeof( unsigned char), 18u));}{ struct _tuple7
_temp708; int _temp709; unsigned char _temp711; struct Cyc_List_List* _temp713;
struct Cyc_List_List* _temp715; struct Cyc_List_List* _temp717; struct Cyc_List_List*
_temp719; struct _tuple7* _temp706=( struct _tuple7*)(( struct Cyc_Core_Opt*)
_check_null( _temp704))->v; _temp708=* _temp706; _LL720: _temp719= _temp708.f1;
goto _LL718; _LL718: _temp717= _temp708.f2; goto _LL716; _LL716: _temp715=
_temp708.f3; goto _LL714; _LL714: _temp713= _temp708.f4; goto _LL712; _LL712:
_temp711= _temp708.f5; goto _LL710; _LL710: _temp709= _temp708.f6; goto _LL707;
_LL707: i= _temp709 - 1; if( _temp711 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)(
r,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x))
Cyc_List_rflatten)( r,({ struct Cyc_List_List*(* _temp721)( struct _RegionHandle*,
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_arr)) Cyc_List_rlist; struct _RegionHandle* _temp728= r; struct Cyc_List_List*
_temp724= _temp719; struct Cyc_List_List* _temp725= _temp717; struct Cyc_List_List*
_temp726= _temp715; struct Cyc_List_List* _temp727= _temp713; struct Cyc_List_List*
_temp722[ 4u]={ _temp724, _temp725, _temp726, _temp727}; struct _tagged_arr
_temp723={( void*) _temp722,( void*) _temp722,( void*)( _temp722 + 4u)};
_temp721( _temp728, _temp723);})), rev_fmt); rev_fmt=({ struct Cyc_List_List*
_temp729=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp729->hd=( void*)(( int) _temp711); _temp729->tl= rev_fmt; _temp729;});{
struct Cyc_List_List* _temp730= _temp717; struct Cyc_List_List _temp737; struct
Cyc_List_List* _temp738; int _temp740; _LL732: if( _temp730 == 0){ goto _LL734;}
else{ _temp737=* _temp730; _LL741: _temp740=( int) _temp737.hd; goto _LL739;
_LL739: _temp738= _temp737.tl; if( _temp738 == 0){ goto _LL736;} else{ goto
_LL734;}} _LL736: if( _temp740 =='*'){ goto _LL733;} else{ goto _LL734;} _LL734:
goto _LL735; _LL733: { struct _tuple0* _temp742= Cyc_Toc_temp_var(); goto _LL743;
_LL743: rev_temps=({ struct Cyc_List_List* _temp744=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp744->hd=( void*)({
struct _tuple8* _temp745=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp745->f1= _temp742; _temp745->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp745->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp745;}); _temp744->tl= rev_temps; _temp744;});
rev_result=({ struct Cyc_List_List* _temp746=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp746->hd=( void*) Cyc_Absyn_var_exp(
_temp742, 0); _temp746->tl= rev_result; _temp746;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL731;} _LL735: goto _LL731; _LL731:;}{ struct Cyc_List_List* _temp747=
_temp715; struct Cyc_List_List _temp754; struct Cyc_List_List* _temp755; struct
Cyc_List_List _temp757; struct Cyc_List_List* _temp758; int _temp760; int
_temp762; _LL749: if( _temp747 == 0){ goto _LL751;} else{ _temp754=* _temp747;
_LL763: _temp762=( int) _temp754.hd; goto _LL756; _LL756: _temp755= _temp754.tl;
if( _temp755 == 0){ goto _LL751;} else{ _temp757=* _temp755; _LL761: _temp760=(
int) _temp757.hd; goto _LL759; _LL759: _temp758= _temp757.tl; if( _temp758 == 0){
goto _LL753;} else{ goto _LL751;}}} _LL753: if( _temp762 =='.'? _temp760 =='*':
0){ goto _LL750;} else{ goto _LL751;} _LL751: goto _LL752; _LL750: { struct
_tuple0* _temp764= Cyc_Toc_temp_var(); goto _LL765; _LL765: rev_temps=({ struct
Cyc_List_List* _temp766=( struct Cyc_List_List*) _region_malloc( r, sizeof(
struct Cyc_List_List)); _temp766->hd=( void*)({ struct _tuple8* _temp767=(
struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8)); _temp767->f1=
_temp764; _temp767->f2=( void*)(( struct Cyc_List_List*) _check_null( typs))->hd;
_temp767->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd;
_temp767;}); _temp766->tl= rev_temps; _temp766;}); rev_result=({ struct Cyc_List_List*
_temp768=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp768->hd=( void*) Cyc_Absyn_var_exp( _temp764, 0); _temp768->tl= rev_result;
_temp768;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL748;} _LL752: goto
_LL748; _LL748:;} if( _temp711 !='%'){ if( args == 0? 1: typs == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp769= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_get_arr_size( _temp769, 1u), _temp769.curr);}));}{ struct _tuple0* _temp770=
Cyc_Toc_temp_var(); goto _LL771; _LL771: rev_temps=({ struct Cyc_List_List*
_temp772=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp772->hd=( void*)({ struct _tuple8* _temp773=( struct _tuple8*)
_region_malloc( r, sizeof( struct _tuple8)); _temp773->f1= _temp770; _temp773->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp773->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp773;});
_temp772->tl= rev_temps; _temp772;}); rev_result=({ struct Cyc_List_List*
_temp774=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp774->hd=( void*) Cyc_Absyn_var_exp( _temp770, 0); _temp774->tl= rev_result;
_temp774;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp775=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp775->hd=( void*)(( int)'.'); _temp775->tl= rev_fmt; _temp775;}); rev_fmt=({
struct Cyc_List_List* _temp776=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp776->hd=( void*)(( int)'*'); _temp776->tl=
rev_fmt; _temp776;}); rev_fmt=({ struct Cyc_List_List* _temp777=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp777->hd=( void*)(( int)'s');
_temp777->tl= rev_fmt; _temp777;});{ struct _tuple0* _temp778= Cyc_Toc_temp_var();
goto _LL779; _LL779: { struct Cyc_Absyn_Exp* _temp780= Cyc_Absyn_var_exp(
_temp778, 0); goto _LL781; _LL781: rev_temps=({ struct Cyc_List_List* _temp782=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp782->hd=( void*)({ struct _tuple8* _temp783=( struct _tuple8*)
_region_malloc( r, sizeof( struct _tuple8)); _temp783->f1= _temp778; _temp783->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp783->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp783;});
_temp782->tl= rev_temps; _temp782;});{ struct Cyc_Absyn_Exp* _temp789= Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp784)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp787= _temp780; struct Cyc_Absyn_Exp* _temp788= Cyc_Absyn_uint_exp( 1, 0);
struct Cyc_Absyn_Exp* _temp785[ 2u]={ _temp787, _temp788}; struct _tagged_arr
_temp786={( void*) _temp785,( void*) _temp785,( void*)( _temp785 + 2u)};
_temp784( _temp786);}), 0); goto _LL790; _LL790: { struct Cyc_Absyn_Exp*
_temp791= Cyc_Absyn_structmember_exp( _temp780, Cyc_Toc_curr_sp, 0); goto _LL792;
_LL792: rev_result=({ struct Cyc_List_List* _temp793=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp793->hd=( void*)
_temp789; _temp793->tl= rev_result; _temp793;}); rev_result=({ struct Cyc_List_List*
_temp794=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp794->hd=( void*) _temp791; _temp794->tl= rev_result; _temp794;}); args=((
struct Cyc_List_List*) _check_null( args))->tl; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl;}}}}}}}}{ struct Cyc_List_List* _temp796=({ struct Cyc_List_List*
_temp795=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp795->hd=( void*) Cyc_Absyn_string_exp(( struct _tagged_arr) Cyc_String_implode(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0);
_temp795->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_result); _temp795;}); goto _LL797; _LL797: if( aopt != 0){ _temp796=({
struct Cyc_List_List* _temp798=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp798->hd=( void*)(( struct Cyc_Absyn_Exp*)(( struct
Cyc_Core_Opt*) _check_null( aopt))->v); _temp798->tl= _temp796; _temp798;});}{
struct Cyc_Absyn_Exp* _temp799= Cyc_Absyn_primop_exp( p, _temp796, 0); goto
_LL800; _LL800: s= Cyc_Absyn_exp_stmt( _temp799, 0); for( 0; rev_temps != 0;
rev_temps=(( struct Cyc_List_List*) _check_null( rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f1,(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f2,(
struct Cyc_Absyn_Exp*)(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
rev_temps))->hd)).f3, s, 0);}}}}; _pop_region( r);} return Cyc_Absyn_stmt_exp( s,
0);}}}} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u));} return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Missing type in primop ", sizeof( unsigned char), 24u));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp801=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp801->f1= Cyc_Toc_mt_tq; _temp801->f2=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp801;});} static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp802=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp802->f1= 0;
_temp802->f2= e; _temp802;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct(
struct Cyc_Toc_Env* nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt*
s, int pointer, struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp*
eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp* _temp803= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); goto
_LL804; _LL804: if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp803): Cyc_Toc_malloc_ptr( _temp803), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp803), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp805=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); goto _LL806; _LL806: for( 0; _temp805
!= 0; _temp805=(( struct Cyc_List_List*) _check_null( _temp805))->tl){ struct
_tuple4 _temp809; struct Cyc_Absyn_Exp* _temp810; struct Cyc_List_List* _temp812;
struct _tuple4* _temp807=( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp805))->hd; _temp809=* _temp807; _LL813: _temp812= _temp809.f1; goto _LL811;
_LL811: _temp810= _temp809.f2; goto _LL808; _LL808: { struct Cyc_Absyn_Exp*
e_index; if( _temp812 == 0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);}
else{ if((( struct Cyc_List_List*) _check_null( _temp812))->tl != 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u));}{ void* _temp814=( void*)(( struct Cyc_List_List*)
_check_null( _temp812))->hd; goto _LL815; _LL815: { void* _temp816= _temp814;
struct Cyc_Absyn_Exp* _temp822; struct _tagged_arr* _temp824; _LL818: if(*(( int*)
_temp816) == Cyc_Absyn_ArrayElement){ _LL823: _temp822=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp816)->f1; goto _LL819;} else{ goto _LL820;} _LL820: if(*(( int*) _temp816)
== Cyc_Absyn_FieldName){ _LL825: _temp824=(( struct Cyc_Absyn_FieldName_struct*)
_temp816)->f1; goto _LL821;} else{ goto _LL817;} _LL819: Cyc_Toc_exp_to_c( nv,
_temp822); e_index= _temp822; goto _LL817; _LL821: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("field name designators in array",
sizeof( unsigned char), 32u)); goto _LL817; _LL817:;}}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp826=( void*)
_temp810->r; struct Cyc_List_List* _temp836; struct Cyc_Absyn_Exp* _temp838;
struct Cyc_Absyn_Exp* _temp840; struct Cyc_Absyn_Vardecl* _temp842; struct Cyc_List_List*
_temp844; void* _temp846; _LL828: if(*(( int*) _temp826) == Cyc_Absyn_Array_e){
_LL837: _temp836=(( struct Cyc_Absyn_Array_e_struct*) _temp826)->f1; goto _LL829;}
else{ goto _LL830;} _LL830: if(*(( int*) _temp826) == Cyc_Absyn_Comprehension_e){
_LL843: _temp842=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp826)->f1;
goto _LL841; _LL841: _temp840=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp826)->f2; goto _LL839; _LL839: _temp838=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp826)->f3; goto _LL831;} else{ goto _LL832;} _LL832: if(*(( int*) _temp826)
== Cyc_Absyn_AnonStruct_e){ _LL847: _temp846=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp826)->f1; goto _LL845; _LL845: _temp844=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp826)->f2; goto _LL833;} else{ goto _LL834;} _LL834: goto _LL835; _LL829: s=
Cyc_Toc_init_array( nv, lval, _temp836, s); goto _LL827; _LL831: s= Cyc_Toc_init_comprehension(
nv, lval, _temp842, _temp840, _temp838, s, 0); goto _LL827; _LL833: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp846, _temp844, s); goto _LL827; _LL835: Cyc_Toc_exp_to_c( nv,
_temp810); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp810, 0), s, 0); goto _LL827; _LL827:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp848= vd->name; goto _LL849; _LL849:
if( ! e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, _temp848, Cyc_Absyn_varb_exp( _temp848,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp872=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp872[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp873; _temp873.tag= Cyc_Absyn_Local_b; _temp873.f1= vd; _temp873;});
_temp872;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp848, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp848, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp848, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp848, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp850=( void*) e2->r; struct Cyc_List_List* _temp860; struct Cyc_Absyn_Exp*
_temp862; struct Cyc_Absyn_Exp* _temp864; struct Cyc_Absyn_Vardecl* _temp866;
struct Cyc_List_List* _temp868; void* _temp870; _LL852: if(*(( int*) _temp850)
== Cyc_Absyn_Array_e){ _LL861: _temp860=(( struct Cyc_Absyn_Array_e_struct*)
_temp850)->f1; goto _LL853;} else{ goto _LL854;} _LL854: if(*(( int*) _temp850)
== Cyc_Absyn_Comprehension_e){ _LL867: _temp866=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp850)->f1; goto _LL865; _LL865: _temp864=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp850)->f2; goto _LL863; _LL863: _temp862=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp850)->f3; goto _LL855;} else{ goto _LL856;} _LL856: if(*(( int*) _temp850)
== Cyc_Absyn_AnonStruct_e){ _LL871: _temp870=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp850)->f1; goto _LL869; _LL869: _temp868=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp850)->f2; goto _LL857;} else{ goto _LL858;} _LL858: goto _LL859; _LL853:
body= Cyc_Toc_init_array( nv2, lval, _temp860, Cyc_Absyn_skip_stmt( 0)); goto
_LL851; _LL855: body= Cyc_Toc_init_comprehension( nv2, lval, _temp866, _temp864,
_temp862, Cyc_Absyn_skip_stmt( 0), 0); goto _LL851; _LL857: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp870, _temp868, Cyc_Absyn_skip_stmt( 0)); goto _LL851; _LL859: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL851; _LL851:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp848, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp874=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); goto _LL875; _LL875: for( 0; _temp874 != 0; _temp874=((
struct Cyc_List_List*) _check_null( _temp874))->tl){ struct _tuple4 _temp878;
struct Cyc_Absyn_Exp* _temp879; struct Cyc_List_List* _temp881; struct _tuple4*
_temp876=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp874))->hd;
_temp878=* _temp876; _LL882: _temp881= _temp878.f1; goto _LL880; _LL880:
_temp879= _temp878.f2; goto _LL877; _LL877: if( _temp881 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list", sizeof(
unsigned char), 22u));} if((( struct Cyc_List_List*) _check_null( _temp881))->tl
!= 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u));}{ void* _temp883=( void*)(( struct Cyc_List_List*)
_check_null( _temp881))->hd; struct _tagged_arr* _temp889; _LL885: if(*(( int*)
_temp883) == Cyc_Absyn_FieldName){ _LL890: _temp889=(( struct Cyc_Absyn_FieldName_struct*)
_temp883)->f1; goto _LL886;} else{ goto _LL887;} _LL887: goto _LL888; _LL886: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp889, 0);{ void*
_temp891=( void*) _temp879->r; struct Cyc_List_List* _temp901; struct Cyc_Absyn_Exp*
_temp903; struct Cyc_Absyn_Exp* _temp905; struct Cyc_Absyn_Vardecl* _temp907;
struct Cyc_List_List* _temp909; void* _temp911; _LL893: if(*(( int*) _temp891)
== Cyc_Absyn_Array_e){ _LL902: _temp901=(( struct Cyc_Absyn_Array_e_struct*)
_temp891)->f1; goto _LL894;} else{ goto _LL895;} _LL895: if(*(( int*) _temp891)
== Cyc_Absyn_Comprehension_e){ _LL908: _temp907=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp891)->f1; goto _LL906; _LL906: _temp905=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp891)->f2; goto _LL904; _LL904: _temp903=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp891)->f3; goto _LL896;} else{ goto _LL897;} _LL897: if(*(( int*) _temp891)
== Cyc_Absyn_AnonStruct_e){ _LL912: _temp911=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp891)->f1; goto _LL910; _LL910: _temp909=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp891)->f2; goto _LL898;} else{ goto _LL899;} _LL899: goto _LL900; _LL894: s=
Cyc_Toc_init_array( nv, lval, _temp901, s); goto _LL892; _LL896: s= Cyc_Toc_init_comprehension(
nv, lval, _temp907, _temp905, _temp903, s, 0); goto _LL892; _LL898: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp911, _temp909, s); goto _LL892; _LL900: Cyc_Toc_exp_to_c( nv,
_temp879); if( Cyc_Toc_is_poly_field( struct_type, _temp889)){ _temp879= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp879, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp879, 0), 0), s, 0); goto _LL892; _LL892:;} goto
_LL884;} _LL888:( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u))); _LL884:;}}}
return s;} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env*
nv, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
_RegionHandle _temp913= _new_region(); struct _RegionHandle* r=& _temp913;
_push_region( r);{ struct Cyc_List_List* _temp914=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c, es); goto _LL915; _LL915: { struct
_tagged_arr* _temp916= Cyc_Toc_add_tuple_type( _temp914); goto _LL917; _LL917: {
struct _tuple0* _temp918= Cyc_Toc_temp_var(); goto _LL919; _LL919: { struct Cyc_Absyn_Exp*
_temp920= Cyc_Absyn_var_exp( _temp918, 0); goto _LL921; _LL921: { struct Cyc_Absyn_Stmt*
_temp922= Cyc_Absyn_exp_stmt( _temp920, 0); goto _LL923; _LL923: { struct Cyc_Absyn_Exp*(*
_temp924)( struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; goto _LL925;
_LL925: { int is_atomic= 1; struct Cyc_List_List* _temp926=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es); goto
_LL927; _LL927:{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp926); for( 0; _temp926 != 0;( _temp926=(( struct Cyc_List_List*)
_check_null( _temp926))->tl, -- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp926))->hd; struct Cyc_Absyn_Exp* lval=
_temp924( _temp920, Cyc_Toc_fieldname( i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v): 0;{ void* _temp928=(
void*) e->r; struct Cyc_List_List* _temp936; struct Cyc_Absyn_Exp* _temp938;
struct Cyc_Absyn_Exp* _temp940; struct Cyc_Absyn_Vardecl* _temp942; _LL930: if(*((
int*) _temp928) == Cyc_Absyn_Array_e){ _LL937: _temp936=(( struct Cyc_Absyn_Array_e_struct*)
_temp928)->f1; goto _LL931;} else{ goto _LL932;} _LL932: if(*(( int*) _temp928)
== Cyc_Absyn_Comprehension_e){ _LL943: _temp942=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp928)->f1; goto _LL941; _LL941: _temp940=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp928)->f2; goto _LL939; _LL939: _temp938=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp928)->f3; goto _LL933;} else{ goto _LL934;} _LL934: goto _LL935; _LL931:
_temp922= Cyc_Toc_init_array( nv, lval, _temp936, _temp922); goto _LL929; _LL933:
_temp922= Cyc_Toc_init_comprehension( nv, lval, _temp942, _temp940, _temp938,
_temp922, 0); goto _LL929; _LL935: Cyc_Toc_exp_to_c( nv, e); _temp922= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp924( _temp920, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp922, 0); goto _LL929; _LL929:;}}}{ struct Cyc_Absyn_Exp*
_temp944= Cyc_Toc_make_struct( nv, _temp918, Cyc_Absyn_strct( _temp916),
_temp922, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp944;}}}}}}}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp945= Cyc_Toc_temp_var(); goto
_LL946; _LL946: { struct Cyc_Absyn_Exp* _temp947= Cyc_Absyn_var_exp( _temp945, 0);
goto _LL948; _LL948: { struct Cyc_Absyn_Stmt* _temp949= Cyc_Absyn_exp_stmt(
_temp947, 0); goto _LL950; _LL950: { struct Cyc_Absyn_Exp*(* _temp951)( struct
Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; goto _LL952; _LL952: { int is_atomic= 1; struct
_RegionHandle _temp953= _new_region(); struct _RegionHandle* r=& _temp953;
_push_region( r);{ struct Cyc_List_List* _temp954=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); goto
_LL955; _LL955: for( 0; _temp954 != 0; _temp954=(( struct Cyc_List_List*)
_check_null( _temp954))->tl){ struct _tuple4 _temp958; struct Cyc_Absyn_Exp*
_temp959; struct Cyc_List_List* _temp961; struct _tuple4* _temp956=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp954))->hd; _temp958=*
_temp956; _LL962: _temp961= _temp958.f1; goto _LL960; _LL960: _temp959= _temp958.f2;
goto _LL957; _LL957: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp959->topt))->v): 0; if( _temp961 == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u));} if((( struct Cyc_List_List*) _check_null(
_temp961))->tl != 0){ struct _tuple0* _temp963= Cyc_Toc_temp_var(); goto _LL964;
_LL964: { struct Cyc_Absyn_Exp* _temp965= Cyc_Absyn_var_exp( _temp963, 0); goto
_LL966; _LL966: for( 0; _temp961 != 0; _temp961=(( struct Cyc_List_List*)
_check_null( _temp961))->tl){ void* _temp967=( void*)(( struct Cyc_List_List*)
_check_null( _temp961))->hd; struct _tagged_arr* _temp973; _LL969: if(*(( int*)
_temp967) == Cyc_Absyn_FieldName){ _LL974: _temp973=(( struct Cyc_Absyn_FieldName_struct*)
_temp967)->f1; goto _LL970;} else{ goto _LL971;} _LL971: goto _LL972; _LL970:
if( Cyc_Toc_is_poly_field( struct_type, _temp973)){ _temp965= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp965, 0);} _temp949= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp951( _temp947, _temp973, 0), _temp965, 0), 0),
_temp949, 0); goto _LL968; _LL972:( int) _throw((( void*(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)( _tag_arr("array designator in struct", sizeof(
unsigned char), 27u))); _LL968:;} Cyc_Toc_exp_to_c( nv, _temp959); _temp949= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp965, _temp959, 0), 0), _temp949,
0);}} else{ void* _temp975=( void*)(( struct Cyc_List_List*) _check_null(
_temp961))->hd; struct _tagged_arr* _temp981; _LL977: if(*(( int*) _temp975) ==
Cyc_Absyn_FieldName){ _LL982: _temp981=(( struct Cyc_Absyn_FieldName_struct*)
_temp975)->f1; goto _LL978;} else{ goto _LL979;} _LL979: goto _LL980; _LL978: {
struct Cyc_Absyn_Exp* lval= _temp951( _temp947, _temp981, 0);{ void* _temp983=(
void*) _temp959->r; struct Cyc_List_List* _temp993; struct Cyc_Absyn_Exp*
_temp995; struct Cyc_Absyn_Exp* _temp997; struct Cyc_Absyn_Vardecl* _temp999;
struct Cyc_List_List* _temp1001; void* _temp1003; _LL985: if(*(( int*) _temp983)
== Cyc_Absyn_Array_e){ _LL994: _temp993=(( struct Cyc_Absyn_Array_e_struct*)
_temp983)->f1; goto _LL986;} else{ goto _LL987;} _LL987: if(*(( int*) _temp983)
== Cyc_Absyn_Comprehension_e){ _LL1000: _temp999=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp983)->f1; goto _LL998; _LL998: _temp997=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp983)->f2; goto _LL996; _LL996: _temp995=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp983)->f3; goto _LL988;} else{ goto _LL989;} _LL989: if(*(( int*) _temp983)
== Cyc_Absyn_AnonStruct_e){ _LL1004: _temp1003=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp983)->f1; goto _LL1002; _LL1002: _temp1001=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp983)->f2; goto _LL990;} else{ goto _LL991;} _LL991: goto _LL992; _LL986:
_temp949= Cyc_Toc_init_array( nv, lval, _temp993, _temp949); goto _LL984; _LL988:
_temp949= Cyc_Toc_init_comprehension( nv, lval, _temp999, _temp997, _temp995,
_temp949, 0); goto _LL984; _LL990: _temp949= Cyc_Toc_init_anon_struct( nv, lval,
_temp1003, _temp1001, _temp949); goto _LL984; _LL992: Cyc_Toc_exp_to_c( nv,
_temp959); if( Cyc_Toc_is_poly_field( struct_type, _temp981)){ _temp959= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp959, 0);} _temp949= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp959, 0), 0), _temp949, 0); goto _LL984; _LL984:;}
goto _LL976;} _LL980:( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u))); _LL976:;}}}{
struct Cyc_Absyn_Exp* _temp1005= Cyc_Toc_make_struct( nv, _temp945, Cyc_Absyn_strctq(
tdn), _temp949, pointer, rgnopt, is_atomic); _npop_handler( 0u); return
_temp1005;}; _pop_region( r);}}}}} struct _tuple9{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple9* pr){ return Cyc_Absyn_assignop_exp( el,(*
pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1006=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1006[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1007; _temp1007.tag=
Cyc_Absyn_Increment_e; _temp1007.f1= e1; _temp1007.f2=( void*) incr; _temp1007;});
_temp1006;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1008=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1018; struct Cyc_Absyn_Exp* _temp1020; void* _temp1022; struct _tagged_arr*
_temp1024; struct Cyc_Absyn_Exp* _temp1026; _LL1010: if(*(( int*) _temp1008) ==
Cyc_Absyn_StmtExp_e){ _LL1019: _temp1018=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1008)->f1; goto _LL1011;} else{ goto _LL1012;} _LL1012: if(*(( int*)
_temp1008) == Cyc_Absyn_Cast_e){ _LL1023: _temp1022=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1008)->f1; goto _LL1021; _LL1021: _temp1020=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1008)->f2; goto _LL1013;} else{ goto _LL1014;} _LL1014: if(*(( int*)
_temp1008) == Cyc_Absyn_StructMember_e){ _LL1027: _temp1026=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1008)->f1; goto _LL1025; _LL1025: _temp1024=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1008)->f2; goto _LL1015;} else{ goto _LL1016;} _LL1016: goto _LL1017;
_LL1011:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1018, fs, f, f_env); goto _LL1009; _LL1013:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1020, fs, f, f_env); goto
_LL1009; _LL1015:( void*)( e1->r=( void*)(( void*) _temp1026->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1028=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1028->hd=( void*) _temp1024; _temp1028->tl= fs;
_temp1028;}), f, f_env); goto _LL1009; _LL1017: if( ! Cyc_Absyn_is_lvalue( e1)){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp1029= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_get_arr_size( _temp1029, 1u), _temp1029.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL1009;}
_LL1009:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp1030=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1040; struct Cyc_Absyn_Stmt* _temp1042; struct Cyc_Absyn_Decl* _temp1044;
struct Cyc_Absyn_Stmt* _temp1046; struct Cyc_Absyn_Stmt* _temp1048; _LL1032: if((
unsigned int) _temp1030 > 1u?*(( int*) _temp1030) == Cyc_Absyn_Exp_s: 0){
_LL1041: _temp1040=(( struct Cyc_Absyn_Exp_s_struct*) _temp1030)->f1; goto
_LL1033;} else{ goto _LL1034;} _LL1034: if(( unsigned int) _temp1030 > 1u?*((
int*) _temp1030) == Cyc_Absyn_Decl_s: 0){ _LL1045: _temp1044=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1030)->f1; goto _LL1043; _LL1043: _temp1042=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1030)->f2; goto _LL1035;} else{ goto _LL1036;} _LL1036: if(( unsigned int)
_temp1030 > 1u?*(( int*) _temp1030) == Cyc_Absyn_Seq_s: 0){ _LL1049: _temp1048=((
struct Cyc_Absyn_Seq_s_struct*) _temp1030)->f1; goto _LL1047; _LL1047: _temp1046=((
struct Cyc_Absyn_Seq_s_struct*) _temp1030)->f2; goto _LL1037;} else{ goto
_LL1038;} _LL1038: goto _LL1039; _LL1033:(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1040, fs, f, f_env); goto
_LL1031; _LL1035:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1042, fs, f, f_env); goto _LL1031; _LL1037:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1046, fs, f, f_env);
goto _LL1031; _LL1039:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1050= Cyc_Absynpp_stmt2string( s);
xprintf("lvalue_assign_stmt: %.*s", _get_arr_size( _temp1050, 1u), _temp1050.curr);}));
goto _LL1031; _LL1031:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){ return 0;}
result=({ struct Cyc_List_List* _temp1051=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1051->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1051->tl= 0;
_temp1051;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp1052=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1052->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1052->tl= 0;
_temp1052;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x))
Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1053=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1053->f1= 0; _temp1053->f2= e;
_temp1053;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){
void* _temp1054= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp1060;
_LL1056: if(( unsigned int) _temp1054 > 4u?*(( int*) _temp1054) == Cyc_Absyn_PointerType:
0){ _LL1061: _temp1060=(( struct Cyc_Absyn_PointerType_struct*) _temp1054)->f1;
goto _LL1057;} else{ goto _LL1058;} _LL1058: goto _LL1059; _LL1057: return
_temp1060; _LL1059:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("get_ptr_typ:  not a pointer!", sizeof( unsigned char), 29u))); _LL1055:;}
struct _tuple10{ void* f1; void* f2; } ; struct _tuple11{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp1062=( void*) e->r; goto _LL1063;
_LL1063: if( e->topt == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1064= Cyc_Absynpp_exp2string( e);
xprintf("exp_to_c: no type for %.*s", _get_arr_size( _temp1064, 1u), _temp1064.curr);}));
return;}{ void* old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
void* _temp1065= _temp1062; void* _temp1145; void* _temp1147; void* _temp1149;
struct _tuple0* _temp1151; struct _tuple0* _temp1153; struct Cyc_List_List*
_temp1155; void* _temp1157; void* _temp1159; struct Cyc_Absyn_Exp* _temp1161;
struct Cyc_Absyn_Exp* _temp1163; struct Cyc_Core_Opt* _temp1165; struct Cyc_Absyn_Exp*
_temp1167; struct Cyc_Absyn_Exp* _temp1169; struct Cyc_Absyn_Exp* _temp1171;
struct Cyc_Absyn_Exp* _temp1173; struct Cyc_Absyn_Exp* _temp1175; struct Cyc_Absyn_Exp*
_temp1177; struct Cyc_List_List* _temp1179; struct Cyc_Absyn_Exp* _temp1181;
struct Cyc_Absyn_VarargCallInfo* _temp1183; struct Cyc_List_List* _temp1185;
struct Cyc_Absyn_Exp* _temp1187; struct Cyc_Absyn_VarargCallInfo* _temp1189;
struct Cyc_Absyn_VarargCallInfo _temp1191; struct Cyc_Absyn_VarargInfo*
_temp1192; struct Cyc_List_List* _temp1194; int _temp1196; struct Cyc_List_List*
_temp1198; struct Cyc_Absyn_Exp* _temp1200; struct Cyc_Absyn_Exp* _temp1202;
struct Cyc_Absyn_Exp* _temp1204; struct Cyc_List_List* _temp1206; struct Cyc_Absyn_Exp*
_temp1208; struct Cyc_Absyn_Exp* _temp1210; void* _temp1212; void** _temp1214;
struct Cyc_Absyn_Exp* _temp1215; struct Cyc_Absyn_Exp* _temp1217; struct Cyc_Absyn_Exp*
_temp1219; struct Cyc_Absyn_Exp* _temp1221; void* _temp1223; struct _tagged_arr*
_temp1225; void* _temp1227; struct Cyc_Absyn_Exp* _temp1229; struct _tagged_arr*
_temp1231; struct Cyc_Absyn_Exp* _temp1233; struct _tagged_arr* _temp1235;
struct Cyc_Absyn_Exp* _temp1237; struct Cyc_Absyn_Exp* _temp1239; struct Cyc_Absyn_Exp*
_temp1241; struct Cyc_List_List* _temp1243; struct Cyc_List_List* _temp1245;
struct Cyc_Absyn_Exp* _temp1247; struct Cyc_Absyn_Exp* _temp1249; struct Cyc_Absyn_Vardecl*
_temp1251; struct Cyc_Absyn_Structdecl* _temp1253; struct Cyc_List_List*
_temp1255; struct Cyc_Core_Opt* _temp1257; struct _tuple0* _temp1259; struct Cyc_List_List*
_temp1261; void* _temp1263; struct Cyc_Absyn_Tunionfield* _temp1265; struct Cyc_Absyn_Tuniondecl*
_temp1267; struct Cyc_List_List* _temp1269; struct Cyc_Core_Opt* _temp1271;
struct Cyc_Core_Opt* _temp1273; struct Cyc_Absyn_Tunionfield* _temp1275; struct
Cyc_Absyn_Tuniondecl* _temp1277; struct Cyc_List_List* _temp1279; struct Cyc_Core_Opt*
_temp1281; struct Cyc_Core_Opt* _temp1283; struct Cyc_Absyn_Enumfield* _temp1285;
struct Cyc_Absyn_Enumdecl* _temp1287; struct _tuple0* _temp1289; void* _temp1291;
struct Cyc_Absyn_Exp* _temp1293; struct Cyc_Absyn_Stmt* _temp1295; struct Cyc_List_List*
_temp1297; struct Cyc_Core_Opt* _temp1299; struct Cyc_List_List* _temp1301;
struct _tuple1* _temp1303; struct Cyc_Absyn_Fndecl* _temp1305; struct Cyc_Absyn_Exp*
_temp1307; _LL1067: if(*(( int*) _temp1065) == Cyc_Absyn_Const_e){ _LL1146:
_temp1145=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1065)->f1; if(
_temp1145 ==( void*) Cyc_Absyn_Null_c){ goto _LL1068;} else{ goto _LL1069;}}
else{ goto _LL1069;} _LL1069: if(*(( int*) _temp1065) == Cyc_Absyn_Const_e){
_LL1148: _temp1147=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1065)->f1;
goto _LL1070;} else{ goto _LL1071;} _LL1071: if(*(( int*) _temp1065) == Cyc_Absyn_Var_e){
_LL1152: _temp1151=(( struct Cyc_Absyn_Var_e_struct*) _temp1065)->f1; goto
_LL1150; _LL1150: _temp1149=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1065)->f2;
goto _LL1072;} else{ goto _LL1073;} _LL1073: if(*(( int*) _temp1065) == Cyc_Absyn_UnknownId_e){
_LL1154: _temp1153=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1065)->f1; goto
_LL1074;} else{ goto _LL1075;} _LL1075: if(*(( int*) _temp1065) == Cyc_Absyn_Primop_e){
_LL1158: _temp1157=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1065)->f1;
goto _LL1156; _LL1156: _temp1155=(( struct Cyc_Absyn_Primop_e_struct*) _temp1065)->f2;
goto _LL1076;} else{ goto _LL1077;} _LL1077: if(*(( int*) _temp1065) == Cyc_Absyn_Increment_e){
_LL1162: _temp1161=(( struct Cyc_Absyn_Increment_e_struct*) _temp1065)->f1; goto
_LL1160; _LL1160: _temp1159=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1065)->f2; goto _LL1078;} else{ goto _LL1079;} _LL1079: if(*(( int*)
_temp1065) == Cyc_Absyn_AssignOp_e){ _LL1168: _temp1167=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1065)->f1; goto _LL1166; _LL1166: _temp1165=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1065)->f2; goto _LL1164; _LL1164: _temp1163=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1065)->f3; goto _LL1080;} else{ goto _LL1081;} _LL1081: if(*(( int*)
_temp1065) == Cyc_Absyn_Conditional_e){ _LL1174: _temp1173=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1065)->f1; goto _LL1172; _LL1172: _temp1171=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1065)->f2; goto _LL1170; _LL1170: _temp1169=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1065)->f3; goto _LL1082;} else{ goto _LL1083;} _LL1083: if(*(( int*)
_temp1065) == Cyc_Absyn_SeqExp_e){ _LL1178: _temp1177=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1065)->f1; goto _LL1176; _LL1176: _temp1175=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1065)->f2; goto _LL1084;} else{ goto _LL1085;} _LL1085: if(*(( int*)
_temp1065) == Cyc_Absyn_UnknownCall_e){ _LL1182: _temp1181=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1065)->f1; goto _LL1180; _LL1180: _temp1179=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1065)->f2; goto _LL1086;} else{ goto _LL1087;} _LL1087: if(*(( int*)
_temp1065) == Cyc_Absyn_FnCall_e){ _LL1188: _temp1187=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1065)->f1; goto _LL1186; _LL1186: _temp1185=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1065)->f2; goto _LL1184; _LL1184: _temp1183=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1065)->f3; if( _temp1183 == 0){ goto _LL1088;} else{ goto _LL1089;}} else{
goto _LL1089;} _LL1089: if(*(( int*) _temp1065) == Cyc_Absyn_FnCall_e){ _LL1201:
_temp1200=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1065)->f1; goto _LL1199;
_LL1199: _temp1198=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1065)->f2; goto
_LL1190; _LL1190: _temp1189=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1065)->f3;
if( _temp1189 == 0){ goto _LL1091;} else{ _temp1191=* _temp1189; _LL1197:
_temp1196= _temp1191.num_varargs; goto _LL1195; _LL1195: _temp1194= _temp1191.injectors;
goto _LL1193; _LL1193: _temp1192= _temp1191.vai; goto _LL1090;}} else{ goto
_LL1091;} _LL1091: if(*(( int*) _temp1065) == Cyc_Absyn_Throw_e){ _LL1203:
_temp1202=(( struct Cyc_Absyn_Throw_e_struct*) _temp1065)->f1; goto _LL1092;}
else{ goto _LL1093;} _LL1093: if(*(( int*) _temp1065) == Cyc_Absyn_NoInstantiate_e){
_LL1205: _temp1204=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1065)->f1;
goto _LL1094;} else{ goto _LL1095;} _LL1095: if(*(( int*) _temp1065) == Cyc_Absyn_Instantiate_e){
_LL1209: _temp1208=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1065)->f1;
goto _LL1207; _LL1207: _temp1206=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1065)->f2; goto _LL1096;} else{ goto _LL1097;} _LL1097: if(*(( int*)
_temp1065) == Cyc_Absyn_Cast_e){ _LL1213: _temp1212=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1065)->f1; _temp1214=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1065)->f1); goto _LL1211; _LL1211: _temp1210=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1065)->f2; goto _LL1098;} else{ goto _LL1099;} _LL1099: if(*(( int*)
_temp1065) == Cyc_Absyn_Address_e){ _LL1216: _temp1215=(( struct Cyc_Absyn_Address_e_struct*)
_temp1065)->f1; goto _LL1100;} else{ goto _LL1101;} _LL1101: if(*(( int*)
_temp1065) == Cyc_Absyn_New_e){ _LL1220: _temp1219=(( struct Cyc_Absyn_New_e_struct*)
_temp1065)->f1; goto _LL1218; _LL1218: _temp1217=(( struct Cyc_Absyn_New_e_struct*)
_temp1065)->f2; goto _LL1102;} else{ goto _LL1103;} _LL1103: if(*(( int*)
_temp1065) == Cyc_Absyn_Sizeofexp_e){ _LL1222: _temp1221=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1065)->f1; goto _LL1104;} else{ goto _LL1105;} _LL1105: if(*(( int*)
_temp1065) == Cyc_Absyn_Sizeoftyp_e){ _LL1224: _temp1223=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1065)->f1; goto _LL1106;} else{ goto _LL1107;} _LL1107: if(*(( int*)
_temp1065) == Cyc_Absyn_Offsetof_e){ _LL1228: _temp1227=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1065)->f1; goto _LL1226; _LL1226: _temp1225=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1065)->f2; goto _LL1108;} else{ goto _LL1109;} _LL1109: if(*(( int*)
_temp1065) == Cyc_Absyn_Deref_e){ _LL1230: _temp1229=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1065)->f1; goto _LL1110;} else{ goto _LL1111;} _LL1111: if(*(( int*)
_temp1065) == Cyc_Absyn_StructMember_e){ _LL1234: _temp1233=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1065)->f1; goto _LL1232; _LL1232: _temp1231=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1065)->f2; goto _LL1112;} else{ goto _LL1113;} _LL1113: if(*(( int*)
_temp1065) == Cyc_Absyn_StructArrow_e){ _LL1238: _temp1237=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1065)->f1; goto _LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1065)->f2; goto _LL1114;} else{ goto _LL1115;} _LL1115: if(*(( int*)
_temp1065) == Cyc_Absyn_Subscript_e){ _LL1242: _temp1241=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1065)->f1; goto _LL1240; _LL1240: _temp1239=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1065)->f2; goto _LL1116;} else{ goto _LL1117;} _LL1117: if(*(( int*)
_temp1065) == Cyc_Absyn_Tuple_e){ _LL1244: _temp1243=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1065)->f1; goto _LL1118;} else{ goto _LL1119;} _LL1119: if(*(( int*)
_temp1065) == Cyc_Absyn_Array_e){ _LL1246: _temp1245=(( struct Cyc_Absyn_Array_e_struct*)
_temp1065)->f1; goto _LL1120;} else{ goto _LL1121;} _LL1121: if(*(( int*)
_temp1065) == Cyc_Absyn_Comprehension_e){ _LL1252: _temp1251=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1065)->f1; goto _LL1250; _LL1250: _temp1249=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1065)->f2; goto _LL1248; _LL1248: _temp1247=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1065)->f3; goto _LL1122;} else{ goto _LL1123;} _LL1123: if(*(( int*)
_temp1065) == Cyc_Absyn_Struct_e){ _LL1260: _temp1259=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1065)->f1; goto _LL1258; _LL1258: _temp1257=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1065)->f2; goto _LL1256; _LL1256: _temp1255=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1065)->f3; goto _LL1254; _LL1254: _temp1253=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1065)->f4; goto _LL1124;} else{ goto _LL1125;} _LL1125: if(*(( int*)
_temp1065) == Cyc_Absyn_AnonStruct_e){ _LL1264: _temp1263=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1065)->f1; goto _LL1262; _LL1262: _temp1261=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1065)->f2; goto _LL1126;} else{ goto _LL1127;} _LL1127: if(*(( int*)
_temp1065) == Cyc_Absyn_Tunion_e){ _LL1274: _temp1273=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1065)->f1; goto _LL1272; _LL1272: _temp1271=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1065)->f2; goto _LL1270; _LL1270: _temp1269=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1065)->f3; if( _temp1269 == 0){ goto _LL1268;} else{ goto _LL1129;} _LL1268:
_temp1267=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1065)->f4; goto _LL1266;
_LL1266: _temp1265=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1065)->f5; goto
_LL1128;} else{ goto _LL1129;} _LL1129: if(*(( int*) _temp1065) == Cyc_Absyn_Tunion_e){
_LL1284: _temp1283=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1065)->f1; goto
_LL1282; _LL1282: _temp1281=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1065)->f2;
goto _LL1280; _LL1280: _temp1279=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1065)->f3;
goto _LL1278; _LL1278: _temp1277=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1065)->f4;
goto _LL1276; _LL1276: _temp1275=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1065)->f5;
goto _LL1130;} else{ goto _LL1131;} _LL1131: if(*(( int*) _temp1065) == Cyc_Absyn_Enum_e){
_LL1290: _temp1289=(( struct Cyc_Absyn_Enum_e_struct*) _temp1065)->f1; goto
_LL1288; _LL1288: _temp1287=(( struct Cyc_Absyn_Enum_e_struct*) _temp1065)->f2;
goto _LL1286; _LL1286: _temp1285=(( struct Cyc_Absyn_Enum_e_struct*) _temp1065)->f3;
goto _LL1132;} else{ goto _LL1133;} _LL1133: if(*(( int*) _temp1065) == Cyc_Absyn_Malloc_e){
_LL1294: _temp1293=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1065)->f1; goto
_LL1292; _LL1292: _temp1291=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1065)->f2; goto _LL1134;} else{ goto _LL1135;} _LL1135: if(*(( int*)
_temp1065) == Cyc_Absyn_StmtExp_e){ _LL1296: _temp1295=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1065)->f1; goto _LL1136;} else{ goto _LL1137;} _LL1137: if(*(( int*)
_temp1065) == Cyc_Absyn_UnresolvedMem_e){ _LL1300: _temp1299=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1065)->f1; goto _LL1298; _LL1298: _temp1297=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1065)->f2; goto _LL1138;} else{ goto _LL1139;} _LL1139: if(*(( int*)
_temp1065) == Cyc_Absyn_CompoundLit_e){ _LL1304: _temp1303=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1065)->f1; goto _LL1302; _LL1302: _temp1301=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1065)->f2; goto _LL1140;} else{ goto _LL1141;} _LL1141: if(*(( int*)
_temp1065) == Cyc_Absyn_Codegen_e){ _LL1306: _temp1305=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1065)->f1; goto _LL1142;} else{ goto _LL1143;} _LL1143: if(*(( int*)
_temp1065) == Cyc_Absyn_Fill_e){ _LL1308: _temp1307=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1065)->f1; goto _LL1144;} else{ goto _LL1066;} _LL1068: { struct Cyc_Absyn_Exp*
_temp1309= Cyc_Absyn_uint_exp( 0, 0); goto _LL1310; _LL1310: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ if( nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr(
old_typ, _temp1309, _temp1309))->r));} else{( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1311)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1314= _temp1309; struct Cyc_Absyn_Exp* _temp1315=
_temp1309; struct Cyc_Absyn_Exp* _temp1316= _temp1309; struct Cyc_Absyn_Exp*
_temp1312[ 3u]={ _temp1314, _temp1315, _temp1316}; struct _tagged_arr _temp1313={(
void*) _temp1312,( void*) _temp1312,( void*)( _temp1312 + 3u)}; _temp1311(
_temp1313);}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL1066;} _LL1070: goto _LL1066; _LL1072:{ struct
_handler_cons _temp1317; _push_handler(& _temp1317);{ int _temp1319= 0; if(
setjmp( _temp1317.handler)){ _temp1319= 1;} if( ! _temp1319){( void*)( e->r=(
void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key)) Cyc_Dict_lookup)( nv->varmap, _temp1151))->r));; _pop_handler();}
else{ void* _temp1318=( void*) _exn_thrown; void* _temp1321= _temp1318; _LL1323:
if( _temp1321 == Cyc_Dict_Absent){ goto _LL1324;} else{ goto _LL1325;} _LL1325:
goto _LL1326; _LL1324:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1327= Cyc_Absynpp_qvar2string(
_temp1151); xprintf("Can't find %.*s in exp_to_c, Var\n", _get_arr_size(
_temp1327, 1u), _temp1327.curr);})); return; _LL1326:( void) _throw( _temp1321);
_LL1322:;}}} goto _LL1066; _LL1074:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("unknownid", sizeof( unsigned char), 10u)); goto _LL1066; _LL1076: {
struct Cyc_List_List* _temp1328=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1155); goto _LL1329; _LL1329: { struct Cyc_List_List* _temp1330=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Toc_get_c_typ, _temp1155); goto _LL1331; _LL1331:(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1155);{
void* _temp1332= _temp1157; _LL1334: if( _temp1332 ==( void*) Cyc_Absyn_Size){
goto _LL1335;} else{ goto _LL1336;} _LL1336: if( _temp1332 ==( void*) Cyc_Absyn_Printf){
goto _LL1337;} else{ goto _LL1338;} _LL1338: if( _temp1332 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1339;} else{ goto _LL1340;} _LL1340: if( _temp1332 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1341;} else{ goto _LL1342;} _LL1342: if( _temp1332 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1343;} else{ goto _LL1344;} _LL1344: if( _temp1332 ==( void*) Cyc_Absyn_Plus){
goto _LL1345;} else{ goto _LL1346;} _LL1346: if( _temp1332 ==( void*) Cyc_Absyn_Minus){
goto _LL1347;} else{ goto _LL1348;} _LL1348: if( _temp1332 ==( void*) Cyc_Absyn_Eq){
goto _LL1349;} else{ goto _LL1350;} _LL1350: if( _temp1332 ==( void*) Cyc_Absyn_Neq){
goto _LL1351;} else{ goto _LL1352;} _LL1352: if( _temp1332 ==( void*) Cyc_Absyn_Gt){
goto _LL1353;} else{ goto _LL1354;} _LL1354: if( _temp1332 ==( void*) Cyc_Absyn_Gte){
goto _LL1355;} else{ goto _LL1356;} _LL1356: if( _temp1332 ==( void*) Cyc_Absyn_Lt){
goto _LL1357;} else{ goto _LL1358;} _LL1358: if( _temp1332 ==( void*) Cyc_Absyn_Lte){
goto _LL1359;} else{ goto _LL1360;} _LL1360: goto _LL1361; _LL1335: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1155))->hd;{
void* _temp1362=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1370; struct Cyc_Absyn_Tqual _temp1372; void*
_temp1374; struct Cyc_Absyn_PtrInfo _temp1376; struct Cyc_Absyn_Conref*
_temp1378; struct Cyc_Absyn_Tqual _temp1380; struct Cyc_Absyn_Conref* _temp1382;
void* _temp1384; void* _temp1386; _LL1364: if(( unsigned int) _temp1362 > 4u?*((
int*) _temp1362) == Cyc_Absyn_ArrayType: 0){ _LL1375: _temp1374=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1362)->f1; goto _LL1373; _LL1373:
_temp1372=(( struct Cyc_Absyn_ArrayType_struct*) _temp1362)->f2; goto _LL1371;
_LL1371: _temp1370=(( struct Cyc_Absyn_ArrayType_struct*) _temp1362)->f3; goto
_LL1365;} else{ goto _LL1366;} _LL1366: if(( unsigned int) _temp1362 > 4u?*((
int*) _temp1362) == Cyc_Absyn_PointerType: 0){ _LL1377: _temp1376=(( struct Cyc_Absyn_PointerType_struct*)
_temp1362)->f1; _LL1387: _temp1386=( void*) _temp1376.elt_typ; goto _LL1385;
_LL1385: _temp1384=( void*) _temp1376.rgn_typ; goto _LL1383; _LL1383: _temp1382=
_temp1376.nullable; goto _LL1381; _LL1381: _temp1380= _temp1376.tq; goto _LL1379;
_LL1379: _temp1378= _temp1376.bounds; goto _LL1367;} else{ goto _LL1368;}
_LL1368: goto _LL1369; _LL1365:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1370))->r)); goto _LL1363; _LL1367:{ void* _temp1388=(( void*(*)(
void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1378); struct Cyc_Absyn_Exp* _temp1394; _LL1390: if( _temp1388 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1391;} else{ goto _LL1392;} _LL1392: if((
unsigned int) _temp1388 > 1u?*(( int*) _temp1388) == Cyc_Absyn_Upper_b: 0){
_LL1395: _temp1394=(( struct Cyc_Absyn_Upper_b_struct*) _temp1388)->f1; goto
_LL1393;} else{ goto _LL1389;} _LL1391:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp1396)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1399=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1155))->hd; struct Cyc_Absyn_Exp* _temp1400= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( _temp1386), 0); struct Cyc_Absyn_Exp* _temp1397[ 2u]={
_temp1399, _temp1400}; struct _tagged_arr _temp1398={( void*) _temp1397,( void*)
_temp1397,( void*)( _temp1397 + 2u)}; _temp1396( _temp1398);}), 0))->r)); goto
_LL1389; _LL1393:( void*)( e->r=( void*)(( void*) _temp1394->r)); goto _LL1389;
_LL1389:;} goto _LL1363; _LL1369:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1401= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_List_List*) _check_null( _temp1330))->hd); struct
_tagged_arr _temp1402= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_get_arr_size( _temp1401, 1u), _temp1401.curr, _get_arr_size( _temp1402, 1u),
_temp1402.curr);})); return; _LL1363:;} goto _LL1333;} _LL1337:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1157, 0, _temp1155, _temp1330))->r));
goto _LL1333; _LL1339:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1157, 0, _temp1155, _temp1330))->r)); goto _LL1333; _LL1341: if( _temp1155
== 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("fprintf without arguments",
sizeof( unsigned char), 26u));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1157,({ struct Cyc_Core_Opt* _temp1403=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1403->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1155))->hd); _temp1403;}),(( struct Cyc_List_List*)
_check_null( _temp1155))->tl,(( struct Cyc_List_List*) _check_null( _temp1330))->tl))->r));
goto _LL1333; _LL1343: if( _temp1155 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("sscanf without arguments", sizeof( unsigned char), 25u));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1155))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1155))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1155))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1333; _LL1345: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1328))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1155))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1155))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1404)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1407= e1; struct Cyc_Absyn_Exp* _temp1408= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1409= e2; struct Cyc_Absyn_Exp*
_temp1405[ 3u]={ _temp1407, _temp1408, _temp1409}; struct _tagged_arr _temp1406={(
void*) _temp1405,( void*) _temp1405,( void*)( _temp1405 + 3u)}; _temp1404(
_temp1406);}), 0))->r));} goto _LL1333;} _LL1347: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1328))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1155))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1155))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1328))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1410)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1413= e1; struct Cyc_Absyn_Exp* _temp1414= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1415= Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Minus, e2, 0); struct Cyc_Absyn_Exp* _temp1411[ 3u]={ _temp1413,
_temp1414, _temp1415}; struct _tagged_arr _temp1412={( void*) _temp1411,( void*)
_temp1411,( void*)( _temp1411 + 3u)}; _temp1410( _temp1412);}), 0))->r));}} goto
_LL1333;} _LL1349: goto _LL1351; _LL1351: goto _LL1353; _LL1353: goto _LL1355;
_LL1355: goto _LL1357; _LL1357: goto _LL1359; _LL1359: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1155))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1155))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1328))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1328))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1333;} _LL1361: goto
_LL1333; _LL1333:;} goto _LL1066;}} _LL1078: { void* e2_cyc_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1161->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1161);{ void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1416=( _temp1159 ==( void*) Cyc_Absyn_PostInc?
1: _temp1159 ==( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; goto _LL1417; _LL1417: { int _temp1418=(
_temp1159 ==( void*) Cyc_Absyn_PreInc? 1: _temp1159 ==( void*) Cyc_Absyn_PostInc)?
1: - 1; goto _LL1419; _LL1419:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
_temp1416,({ struct Cyc_List_List*(* _temp1420)( struct _tagged_arr)=( struct
Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1423= Cyc_Absyn_address_exp( _temp1161, 0); struct Cyc_Absyn_Exp* _temp1424=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1425= Cyc_Absyn_signed_int_exp( _temp1418, 0); struct Cyc_Absyn_Exp*
_temp1421[ 3u]={ _temp1423, _temp1424, _temp1425}; struct _tagged_arr _temp1422={(
void*) _temp1421,( void*) _temp1421,( void*)( _temp1421 + 3u)}; _temp1420(
_temp1422);}), 0))->r));}} if( elt_typ ==( void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue(
_temp1161): 0){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1161, 0, Cyc_Toc_incr_lvalue, _temp1159);( void*)( e->r=( void*)(( void*)
_temp1161->r));} goto _LL1066;}} _LL1080: { int e1_poly= Cyc_Toc_is_poly_project(
_temp1167); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1167->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1163->topt))->v; Cyc_Toc_exp_to_c( nv, _temp1167); Cyc_Toc_exp_to_c(
nv, _temp1163);{ int done= 0; if( _temp1165 != 0){ void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt( old_typ,& elt_typ)){ struct Cyc_Absyn_Exp*
change;{ void* _temp1426=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1165))->v;
_LL1428: if( _temp1426 ==( void*) Cyc_Absyn_Plus){ goto _LL1429;} else{ goto
_LL1430;} _LL1430: if( _temp1426 ==( void*) Cyc_Absyn_Minus){ goto _LL1431;}
else{ goto _LL1432;} _LL1432: goto _LL1433; _LL1429: change= _temp1163; goto
_LL1427; _LL1431: change= Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus,
_temp1163, 0); goto _LL1427; _LL1433:( int) _throw((( void*(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("bad t ? pointer arithmetic", sizeof(
unsigned char), 27u))); goto _LL1427; _LL1427:;} done= 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_inplace_plus_e,({ struct Cyc_List_List*(*
_temp1434)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp1437= Cyc_Absyn_address_exp( _temp1167,
0); struct Cyc_Absyn_Exp* _temp1438= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); struct Cyc_Absyn_Exp* _temp1439= change; struct Cyc_Absyn_Exp*
_temp1435[ 3u]={ _temp1437, _temp1438, _temp1439}; struct _tagged_arr _temp1436={(
void*) _temp1435,( void*) _temp1435,( void*)( _temp1435 + 3u)}; _temp1434(
_temp1436);}), 0))->r));}} if( ! done){ if( e1_poly){( void*)( _temp1163->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp1163->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp1167)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, struct _tuple9*), struct _tuple9* f_env)) Cyc_Toc_lvalue_assign)(
_temp1167, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple9* _temp1440=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9) * 1); _temp1440[ 0]=({ struct
_tuple9 _temp1441; _temp1441.f1= _temp1165; _temp1441.f2= _temp1163; _temp1441;});
_temp1440;}));( void*)( e->r=( void*)(( void*) _temp1167->r));}} goto _LL1066;}}
_LL1082: Cyc_Toc_exp_to_c( nv, _temp1173); Cyc_Toc_exp_to_c( nv, _temp1171); Cyc_Toc_exp_to_c(
nv, _temp1169); goto _LL1066; _LL1084: Cyc_Toc_exp_to_c( nv, _temp1177); Cyc_Toc_exp_to_c(
nv, _temp1175); goto _LL1066; _LL1086: _temp1187= _temp1181; _temp1185=
_temp1179; goto _LL1088; _LL1088: Cyc_Toc_exp_to_c( nv, _temp1187);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1185);
goto _LL1066; _LL1090:{ struct _RegionHandle _temp1442= _new_region(); struct
_RegionHandle* r=& _temp1442; _push_region( r);{ void* _temp1443=( void*)
_temp1192->type; goto _LL1444; _LL1444: { void* _temp1445= Cyc_Toc_typ_to_c(
_temp1443); goto _LL1446; _LL1446: { void* _temp1447= Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1200->topt))->v); goto _LL1448; _LL1448: {
struct Cyc_Absyn_Exp* _temp1449= Cyc_Absyn_uint_exp(( unsigned int) _temp1196, 0);
goto _LL1450; _LL1450: { void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1517=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1517[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1518; _temp1518.tag= Cyc_Absyn_ArrayType;
_temp1518.f1=( void*) _temp1443; _temp1518.f2= Cyc_Toc_mt_tq; _temp1518.f3=(
struct Cyc_Absyn_Exp*) _temp1449; _temp1518;}); _temp1517;})); struct _tuple0*
_temp1451= Cyc_Toc_temp_var(); goto _LL1452; _LL1452: { struct _tuple0*
_temp1453= Cyc_Toc_temp_var(); goto _LL1454; _LL1454: { struct Cyc_Absyn_Exp*
_temp1455= Cyc_Absyn_var_exp( _temp1453, 0); goto _LL1456; _LL1456: { struct
_tuple0* _temp1457= Cyc_Toc_temp_var(); goto _LL1458; _LL1458: { int _temp1459=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1198); goto _LL1460;
_LL1460: { int _temp1461= _temp1459 - _temp1196; goto _LL1462; _LL1462: { struct
_tagged_arr vs=({ unsigned int _temp1513=( unsigned int) _temp1196; struct
_tuple0** _temp1514=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1513); struct _tagged_arr _temp1516= _tag_arr( _temp1514,
sizeof( struct _tuple0*),( unsigned int) _temp1196);{ unsigned int _temp1515=
_temp1513; unsigned int i; for( i= 0; i < _temp1515; i ++){ _temp1514[ i]= Cyc_Toc_temp_var();}};
_temp1516;}); struct _tagged_arr xs=({ unsigned int _temp1509=( unsigned int)
_temp1461; struct _tuple0** _temp1510=( struct _tuple0**) _region_malloc( r,
sizeof( struct _tuple0*) * _temp1509); struct _tagged_arr _temp1512= _tag_arr(
_temp1510, sizeof( struct _tuple0*),( unsigned int) _temp1461);{ unsigned int
_temp1511= _temp1509; unsigned int i; for( i= 0; i < _temp1511; i ++){ _temp1510[
i]= Cyc_Toc_temp_var();}}; _temp1512;}); struct Cyc_List_List* xexps=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,(
struct Cyc_Position_Segment*) 0,(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, xs));
xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List* _temp1463=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1463->hd=( void*) Cyc_Absyn_var_exp(
_temp1457, 0); _temp1463->tl= 0; _temp1463;}));{ struct Cyc_Absyn_Stmt*
_temp1464= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
_temp1451, 0), xexps, 0), 0); goto _LL1465; _LL1465: { struct Cyc_Absyn_Exp*
_temp1466= Cyc_Absyn_add_exp( _temp1455, _temp1449, 0); goto _LL1467; _LL1467: {
struct Cyc_List_List* _temp1477=({ struct Cyc_List_List*(* _temp1468)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct _tuple4* _temp1471=({ struct _tuple4* _temp1476=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1476->f1= 0; _temp1476->f2= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1455, 0); _temp1476;}); struct _tuple4*
_temp1472=({ struct _tuple4* _temp1475=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1475->f1= 0; _temp1475->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1455, 0); _temp1475;}); struct _tuple4* _temp1473=({ struct _tuple4*
_temp1474=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1474->f1=
0; _temp1474->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1466, 0);
_temp1474;}); struct _tuple4* _temp1469[ 3u]={ _temp1471, _temp1472, _temp1473};
struct _tagged_arr _temp1470={( void*) _temp1469,( void*) _temp1469,( void*)(
_temp1469 + 3u)}; _temp1468( _temp1470);}); goto _LL1478; _LL1478: _temp1464=
Cyc_Absyn_declare_stmt( _temp1457, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp1443,( void*) _temp1192->rgn, Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp(
0, _temp1477, 0), _temp1464, 0);{ struct Cyc_List_List* _temp1479=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, vs));
goto _LL1480; _LL1480: { struct Cyc_List_List* _temp1481=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_make_dle, _temp1479); goto _LL1482; _LL1482: _temp1464= Cyc_Absyn_declare_stmt(
_temp1453, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1481, 0), _temp1464, 0);{ struct Cyc_List_List* _temp1483=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1198);
goto _LL1484; _LL1484: { struct Cyc_List_List* _temp1485=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1194);
goto _LL1486; _LL1486:{ int i= _temp1196 - 1; for( 0; i >= 0;( i --, _temp1483=((
struct Cyc_List_List*) _check_null( _temp1483))->tl)){ struct Cyc_Absyn_Exp* arg=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1483))->hd;
void* arg_type=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c(
nv, arg); if( _temp1192->inject){ void* _temp1489; struct Cyc_Position_Segment*
_temp1491; struct Cyc_List_List* _temp1493; struct Cyc_List_List* _temp1495;
struct _tuple0* _temp1497; struct Cyc_Absyn_Tunionfield _temp1487=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1485))->hd); _LL1498: _temp1497=
_temp1487.name; goto _LL1496; _LL1496: _temp1495= _temp1487.tvs; goto _LL1494;
_LL1494: _temp1493= _temp1487.typs; goto _LL1492; _LL1492: _temp1491= _temp1487.loc;
goto _LL1490; _LL1490: _temp1489=( void*) _temp1487.sc; goto _LL1488; _LL1488:
_temp1485=(( struct Cyc_List_List*) _check_null( _temp1485))->tl;{ struct
_tuple0* _temp1499= Cyc_Toc_temp_var(); goto _LL1500; _LL1500: { struct Cyc_Absyn_Exp*
_temp1501= Cyc_Absyn_var_exp( _temp1499, 0); goto _LL1502; _LL1502: { void*
_temp1503= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1497, _tag_arr("_struct",
sizeof( unsigned char), 8u))); goto _LL1504; _LL1504: _temp1464= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1445,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1445, Cyc_Absyn_address_exp(
_temp1501, 0), 0), _temp1464, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple3*)(( struct Cyc_List_List*) _check_null( _temp1493))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1505= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1501, Cyc_Toc_fieldname(
1), 0), arg, 0); goto _LL1506; _LL1506: _temp1464= Cyc_Absyn_seq_stmt( _temp1505,
_temp1464, 0);{ struct Cyc_Absyn_Stmt* _temp1507= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1501, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1497, 0), 0); goto
_LL1508; _LL1508: _temp1464= Cyc_Absyn_seq_stmt( _temp1507, _temp1464, 0);
_temp1464= Cyc_Absyn_declare_stmt( _temp1499, _temp1503, 0, _temp1464, 0);}}}}}}}
else{ _temp1464= Cyc_Absyn_declare_stmt(*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)), _temp1445,( struct
Cyc_Absyn_Exp*) arg, _temp1464, 0);}}}{ int i= _temp1461 - 1; for( 0; i >= 0;( i
--, _temp1483=(( struct Cyc_List_List*) _check_null( _temp1483))->tl)){ struct
Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1483))->hd; void* argtype= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); Cyc_Toc_exp_to_c( nv, arg); _temp1464= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( xs, sizeof( struct _tuple0*), i)),
argtype,( struct Cyc_Absyn_Exp*) arg, _temp1464, 0);}} Cyc_Toc_exp_to_c( nv,
_temp1200); _temp1464= Cyc_Absyn_declare_stmt( _temp1451, _temp1447,( struct Cyc_Absyn_Exp*)
_temp1200, _temp1464, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1464, 0))->r));}}}}}}}}}}}}}}}}}}; _pop_region( r);} goto _LL1066; _LL1092:
Cyc_Toc_exp_to_c( nv, _temp1202);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1202), 0))->r)); goto
_LL1066; _LL1094: Cyc_Toc_exp_to_c( nv, _temp1204); goto _LL1066; _LL1096: Cyc_Toc_exp_to_c(
nv, _temp1208);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); for( 0; _temp1206 != 0; _temp1206=(( struct Cyc_List_List*)
_check_null( _temp1206))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct
Cyc_List_List*) _check_null( _temp1206))->hd); if( k !=( void*) Cyc_Absyn_EffKind?
k !=( void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1208, 0))->r)); break;}} goto _LL1066;} _LL1098: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1210->topt))->v; void* new_typ=*
_temp1214;* _temp1214= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1210);{ struct _tuple10 _temp1520=({ struct _tuple10 _temp1519; _temp1519.f1=
Cyc_Tcutil_compress( old_t2); _temp1519.f2= Cyc_Tcutil_compress( new_typ);
_temp1519;}); void* _temp1528; struct Cyc_Absyn_PtrInfo _temp1530; void*
_temp1532; struct Cyc_Absyn_PtrInfo _temp1534; void* _temp1536; void* _temp1538;
void* _temp1540; void* _temp1542; struct Cyc_Absyn_PtrInfo _temp1544; _LL1522:
_LL1533: _temp1532= _temp1520.f1; if(( unsigned int) _temp1532 > 4u?*(( int*)
_temp1532) == Cyc_Absyn_PointerType: 0){ _LL1535: _temp1534=(( struct Cyc_Absyn_PointerType_struct*)
_temp1532)->f1; goto _LL1529;} else{ goto _LL1524;} _LL1529: _temp1528=
_temp1520.f2; if(( unsigned int) _temp1528 > 4u?*(( int*) _temp1528) == Cyc_Absyn_PointerType:
0){ _LL1531: _temp1530=(( struct Cyc_Absyn_PointerType_struct*) _temp1528)->f1;
goto _LL1523;} else{ goto _LL1524;} _LL1524: _LL1543: _temp1542= _temp1520.f1;
if(( unsigned int) _temp1542 > 4u?*(( int*) _temp1542) == Cyc_Absyn_PointerType:
0){ _LL1545: _temp1544=(( struct Cyc_Absyn_PointerType_struct*) _temp1542)->f1;
goto _LL1537;} else{ goto _LL1526;} _LL1537: _temp1536= _temp1520.f2; if((
unsigned int) _temp1536 > 4u?*(( int*) _temp1536) == Cyc_Absyn_IntType: 0){
_LL1541: _temp1540=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1536)->f1;
goto _LL1539; _LL1539: _temp1538=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1536)->f2; goto _LL1525;} else{ goto _LL1526;} _LL1526: goto _LL1527;
_LL1523: { int _temp1546=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1534.nullable); goto _LL1547; _LL1547: { int _temp1548=(( int(*)( int,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1530.nullable); goto
_LL1549; _LL1549: { void* _temp1550=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1534.bounds); goto _LL1551;
_LL1551: { void* _temp1552=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1530.bounds); goto _LL1553; _LL1553:{ struct _tuple10
_temp1555=({ struct _tuple10 _temp1554; _temp1554.f1= _temp1550; _temp1554.f2=
_temp1552; _temp1554;}); void* _temp1565; struct Cyc_Absyn_Exp* _temp1567; void*
_temp1569; struct Cyc_Absyn_Exp* _temp1571; void* _temp1573; void* _temp1575;
struct Cyc_Absyn_Exp* _temp1577; void* _temp1579; struct Cyc_Absyn_Exp*
_temp1581; void* _temp1583; _LL1557: _LL1570: _temp1569= _temp1555.f1; if((
unsigned int) _temp1569 > 1u?*(( int*) _temp1569) == Cyc_Absyn_Upper_b: 0){
_LL1572: _temp1571=(( struct Cyc_Absyn_Upper_b_struct*) _temp1569)->f1; goto
_LL1566;} else{ goto _LL1559;} _LL1566: _temp1565= _temp1555.f2; if((
unsigned int) _temp1565 > 1u?*(( int*) _temp1565) == Cyc_Absyn_Upper_b: 0){
_LL1568: _temp1567=(( struct Cyc_Absyn_Upper_b_struct*) _temp1565)->f1; goto
_LL1558;} else{ goto _LL1559;} _LL1559: _LL1576: _temp1575= _temp1555.f1; if((
unsigned int) _temp1575 > 1u?*(( int*) _temp1575) == Cyc_Absyn_Upper_b: 0){
_LL1578: _temp1577=(( struct Cyc_Absyn_Upper_b_struct*) _temp1575)->f1; goto
_LL1574;} else{ goto _LL1561;} _LL1574: _temp1573= _temp1555.f2; if( _temp1573
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1560;} else{ goto _LL1561;} _LL1561:
_LL1584: _temp1583= _temp1555.f1; if( _temp1583 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1580;} else{ goto _LL1563;} _LL1580: _temp1579= _temp1555.f2; if((
unsigned int) _temp1579 > 1u?*(( int*) _temp1579) == Cyc_Absyn_Upper_b: 0){
_LL1582: _temp1581=(( struct Cyc_Absyn_Upper_b_struct*) _temp1579)->f1; goto
_LL1562;} else{ goto _LL1563;} _LL1563: goto _LL1564; _LL1558: if( _temp1546? !
_temp1548: 0){ if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't do null-check conversion at top-level", sizeof( unsigned char),
44u));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp1214, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1585=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1585->hd=( void*) _temp1210;
_temp1585->tl= 0; _temp1585;}), 0), 0))->r));} goto _LL1556; _LL1560: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1577, _temp1210))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1586)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1589= _temp1210; struct Cyc_Absyn_Exp* _temp1590= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1530.elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1591= _temp1577; struct Cyc_Absyn_Exp* _temp1587[ 3u]={ _temp1589,
_temp1590, _temp1591}; struct _tagged_arr _temp1588={( void*) _temp1587,( void*)
_temp1587,( void*)( _temp1587 + 3u)}; _temp1586( _temp1588);}), 0))->r));} goto
_LL1556; _LL1562: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u));}{ struct Cyc_Absyn_Exp* _temp1598= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({
struct Cyc_List_List*(* _temp1592)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1595= _temp1210;
struct Cyc_Absyn_Exp* _temp1596= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c((
void*) _temp1534.elt_typ), 0); struct Cyc_Absyn_Exp* _temp1597= _temp1581;
struct Cyc_Absyn_Exp* _temp1593[ 3u]={ _temp1595, _temp1596, _temp1597}; struct
_tagged_arr _temp1594={( void*) _temp1593,( void*) _temp1593,( void*)( _temp1593
+ 3u)}; _temp1592( _temp1594);}), 0); goto _LL1599; _LL1599: if( _temp1548){(
void*)( _temp1598->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1600=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1600->hd=( void*) Cyc_Absyn_copy_exp( _temp1598);
_temp1600->tl= 0; _temp1600;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp1214, _temp1598, 0))->r)); goto _LL1556;} _LL1564: goto _LL1556; _LL1556:;}
goto _LL1521;}}}} _LL1525:{ void* _temp1601=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1544.bounds); _LL1603: if(
_temp1601 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1604;} else{ goto _LL1605;}
_LL1605: goto _LL1606; _LL1604:( void*)( _temp1210->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1210->r, _temp1210->loc), Cyc_Toc_curr_sp, 0))->r));
goto _LL1602; _LL1606: goto _LL1602; _LL1602:;} goto _LL1521; _LL1527: goto
_LL1521; _LL1521:;} goto _LL1066;} _LL1100:{ void* _temp1607=( void*) _temp1215->r;
struct Cyc_Absyn_Structdecl* _temp1615; struct Cyc_List_List* _temp1617; struct
Cyc_Core_Opt* _temp1619; struct _tuple0* _temp1621; struct Cyc_List_List*
_temp1623; _LL1609: if(*(( int*) _temp1607) == Cyc_Absyn_Struct_e){ _LL1622:
_temp1621=(( struct Cyc_Absyn_Struct_e_struct*) _temp1607)->f1; goto _LL1620;
_LL1620: _temp1619=(( struct Cyc_Absyn_Struct_e_struct*) _temp1607)->f2; goto
_LL1618; _LL1618: _temp1617=(( struct Cyc_Absyn_Struct_e_struct*) _temp1607)->f3;
goto _LL1616; _LL1616: _temp1615=(( struct Cyc_Absyn_Struct_e_struct*) _temp1607)->f4;
goto _LL1610;} else{ goto _LL1611;} _LL1611: if(*(( int*) _temp1607) == Cyc_Absyn_Tuple_e){
_LL1624: _temp1623=(( struct Cyc_Absyn_Tuple_e_struct*) _temp1607)->f1; goto
_LL1612;} else{ goto _LL1613;} _LL1613: goto _LL1614; _LL1610: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr _temp1625=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1215->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_get_arr_size( _temp1625, 1u), _temp1625.curr);}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1215->topt))->v, 1, 0, _temp1617, _temp1621))->r)); goto _LL1608; _LL1612:
if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct
_tagged_arr)({ struct _tagged_arr _temp1626=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1215->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_get_arr_size( _temp1626, 1u), _temp1626.curr);}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1623))->r)); goto _LL1608; _LL1614:
Cyc_Toc_exp_to_c( nv, _temp1215); if( ! Cyc_Absyn_is_lvalue( _temp1215)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1215, 0,
Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1215, 0))->r));} goto _LL1608; _LL1608:;} goto _LL1066; _LL1102: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr _temp1627=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1217->loc); xprintf("%.*s: new at top-level", _get_arr_size( _temp1627, 1u),
_temp1627.curr);}));}{ void* _temp1628=( void*) _temp1217->r; struct Cyc_List_List*
_temp1640; struct Cyc_Absyn_Exp* _temp1642; struct Cyc_Absyn_Exp* _temp1644;
struct Cyc_Absyn_Vardecl* _temp1646; struct Cyc_Absyn_Structdecl* _temp1648;
struct Cyc_List_List* _temp1650; struct Cyc_Core_Opt* _temp1652; struct _tuple0*
_temp1654; struct Cyc_List_List* _temp1656; _LL1630: if(*(( int*) _temp1628) ==
Cyc_Absyn_Array_e){ _LL1641: _temp1640=(( struct Cyc_Absyn_Array_e_struct*)
_temp1628)->f1; goto _LL1631;} else{ goto _LL1632;} _LL1632: if(*(( int*)
_temp1628) == Cyc_Absyn_Comprehension_e){ _LL1647: _temp1646=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1628)->f1; goto _LL1645; _LL1645: _temp1644=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1628)->f2; goto _LL1643; _LL1643: _temp1642=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1628)->f3; goto _LL1633;} else{ goto _LL1634;} _LL1634: if(*(( int*)
_temp1628) == Cyc_Absyn_Struct_e){ _LL1655: _temp1654=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1628)->f1; goto _LL1653; _LL1653: _temp1652=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1628)->f2; goto _LL1651; _LL1651: _temp1650=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1628)->f3; goto _LL1649; _LL1649: _temp1648=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1628)->f4; goto _LL1635;} else{ goto _LL1636;} _LL1636: if(*(( int*)
_temp1628) == Cyc_Absyn_Tuple_e){ _LL1657: _temp1656=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1628)->f1; goto _LL1637;} else{ goto _LL1638;} _LL1638: goto _LL1639;
_LL1631: { struct _tuple0* _temp1658= Cyc_Toc_temp_var(); goto _LL1659; _LL1659: {
struct Cyc_Absyn_Exp* _temp1660= Cyc_Absyn_var_exp( _temp1658, 0); goto _LL1661;
_LL1661: { struct Cyc_Absyn_Stmt* _temp1662= Cyc_Toc_init_array( nv, _temp1660,
_temp1640, Cyc_Absyn_exp_stmt( _temp1660, 0)); goto _LL1663; _LL1663: { void*
old_elt_typ;{ void* _temp1664= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1670; struct Cyc_Absyn_Conref* _temp1672; struct Cyc_Absyn_Tqual _temp1674;
struct Cyc_Absyn_Conref* _temp1676; void* _temp1678; void* _temp1680; _LL1666:
if(( unsigned int) _temp1664 > 4u?*(( int*) _temp1664) == Cyc_Absyn_PointerType:
0){ _LL1671: _temp1670=(( struct Cyc_Absyn_PointerType_struct*) _temp1664)->f1;
_LL1681: _temp1680=( void*) _temp1670.elt_typ; goto _LL1679; _LL1679: _temp1678=(
void*) _temp1670.rgn_typ; goto _LL1677; _LL1677: _temp1676= _temp1670.nullable;
goto _LL1675; _LL1675: _temp1674= _temp1670.tq; goto _LL1673; _LL1673: _temp1672=
_temp1670.bounds; goto _LL1667;} else{ goto _LL1668;} _LL1668: goto _LL1669;
_LL1667: old_elt_typ= _temp1680; goto _LL1665; _LL1669: old_elt_typ=(( void*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u)); goto _LL1665; _LL1665:;}{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* _temp1682= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
goto _LL1683; _LL1683: { struct Cyc_Absyn_Exp* _temp1684= Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1640), 0), 0); goto _LL1685; _LL1685: {
struct Cyc_Absyn_Exp* e1; if( _temp1219 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1684);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1219); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1684);}{ struct Cyc_Absyn_Exp* _temp1686= Cyc_Absyn_cast_exp( _temp1682, e1,
0); goto _LL1687; _LL1687:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( _temp1658, _temp1682,( struct Cyc_Absyn_Exp*) _temp1686,
_temp1662, 0), 0))->r)); goto _LL1629;}}}}}}}} _LL1633: { int is_tagged_ptr= 0;{
void* _temp1688= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1694; struct Cyc_Absyn_Conref* _temp1696; struct Cyc_Absyn_Tqual _temp1698;
struct Cyc_Absyn_Conref* _temp1700; void* _temp1702; void* _temp1704; _LL1690:
if(( unsigned int) _temp1688 > 4u?*(( int*) _temp1688) == Cyc_Absyn_PointerType:
0){ _LL1695: _temp1694=(( struct Cyc_Absyn_PointerType_struct*) _temp1688)->f1;
_LL1705: _temp1704=( void*) _temp1694.elt_typ; goto _LL1703; _LL1703: _temp1702=(
void*) _temp1694.rgn_typ; goto _LL1701; _LL1701: _temp1700= _temp1694.nullable;
goto _LL1699; _LL1699: _temp1698= _temp1694.tq; goto _LL1697; _LL1697: _temp1696=
_temp1694.bounds; goto _LL1691;} else{ goto _LL1692;} _LL1692: goto _LL1693;
_LL1691: is_tagged_ptr=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1696) ==( void*) Cyc_Absyn_Unknown_b; goto _LL1689;
_LL1693:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u)); goto _LL1689; _LL1689:;}{ struct _tuple0* max=
Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* old_elt_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1642->topt))->v; void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1644);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1646, Cyc_Absyn_var_exp( max, 0), _temp1642,
Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1706= _new_region();
struct _RegionHandle* r=& _temp1706; _push_region( r);{ struct Cyc_List_List*
decls=({ struct Cyc_List_List* _temp1733=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1733->hd=( void*)({ struct _tuple8*
_temp1734=( struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8));
_temp1734->f1= max; _temp1734->f2= Cyc_Absyn_uint_t; _temp1734->f3=( struct Cyc_Absyn_Exp*)
_temp1644; _temp1734;}); _temp1733->tl= 0; _temp1733;}); struct Cyc_Absyn_Exp*
ai; if( _temp1219 == 0){ ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{
struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1219); Cyc_Toc_exp_to_c(
nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp(
ptr_typ, ai, 0); decls=({ struct Cyc_List_List* _temp1707=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1707->hd=( void*)({
struct _tuple8* _temp1708=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1708->f1= a; _temp1708->f2= ptr_typ; _temp1708->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1708;}); _temp1707->tl= decls; _temp1707;}); if( is_tagged_ptr){
struct _tuple0* _temp1709= Cyc_Toc_temp_var(); goto _LL1710; _LL1710: { void*
_temp1711= Cyc_Toc_typ_to_c( old_typ); goto _LL1712; _LL1712: { struct Cyc_Absyn_Exp*
_temp1719= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(*
_temp1713)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp1716= Cyc_Absyn_var_exp( a, 0); struct
Cyc_Absyn_Exp* _temp1717= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); struct Cyc_Absyn_Exp*
_temp1718= _temp1644; struct Cyc_Absyn_Exp* _temp1714[ 3u]={ _temp1716,
_temp1717, _temp1718}; struct _tagged_arr _temp1715={( void*) _temp1714,( void*)
_temp1714,( void*)( _temp1714 + 3u)}; _temp1713( _temp1715);}), 0); goto _LL1720;
_LL1720: decls=({ struct Cyc_List_List* _temp1721=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1721->hd=( void*)({
struct _tuple8* _temp1722=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1722->f1= _temp1709; _temp1722->f2= _temp1711; _temp1722->f3=(
struct Cyc_Absyn_Exp*) _temp1719; _temp1722;}); _temp1721->tl= decls; _temp1721;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1709, 0),
0), 0);}}} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1723= decls; goto _LL1724; _LL1724:
for( 0; _temp1723 != 0; _temp1723=(( struct Cyc_List_List*) _check_null(
_temp1723))->tl){ struct Cyc_Absyn_Exp* _temp1727; void* _temp1729; struct
_tuple0* _temp1731; struct _tuple8 _temp1725=*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp1723))->hd); _LL1732: _temp1731= _temp1725.f1; goto _LL1730;
_LL1730: _temp1729= _temp1725.f2; goto _LL1728; _LL1728: _temp1727= _temp1725.f3;
goto _LL1726; _LL1726: s= Cyc_Absyn_declare_stmt( _temp1731, _temp1729,
_temp1727, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}};
_pop_region( r);} goto _LL1629;}}} _LL1635:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1217->topt))->v, 1,
_temp1219, _temp1650, _temp1654))->r)); goto _LL1629; _LL1637:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1219, _temp1656))->r)); goto
_LL1629; _LL1639: { void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1217->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); if( _temp1219
== 0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp1219); Cyc_Toc_exp_to_c( nv, r);
mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1735=( void*)
_temp1217->r; struct Cyc_Absyn_Exp* _temp1741; void* _temp1743; _LL1737: if(*((
int*) _temp1735) == Cyc_Absyn_Cast_e){ _LL1744: _temp1743=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1735)->f1; goto _LL1742; _LL1742: _temp1741=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1735)->f2; goto _LL1738;} else{ goto _LL1739;} _LL1739: goto _LL1740;
_LL1738:{ struct _tuple10 _temp1746=({ struct _tuple10 _temp1745; _temp1745.f1=
Cyc_Tcutil_compress( _temp1743); _temp1745.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1741->topt))->v); _temp1745;}); void*
_temp1752; struct Cyc_Absyn_PtrInfo _temp1754; struct Cyc_Absyn_Conref*
_temp1756; struct Cyc_Absyn_Tqual _temp1758; struct Cyc_Absyn_Conref* _temp1760;
void* _temp1762; void* _temp1764; void* _temp1766; struct Cyc_Absyn_PtrInfo
_temp1768; struct Cyc_Absyn_Conref* _temp1770; struct Cyc_Absyn_Tqual _temp1772;
struct Cyc_Absyn_Conref* _temp1774; void* _temp1776; void* _temp1778; _LL1748:
_LL1767: _temp1766= _temp1746.f1; if(( unsigned int) _temp1766 > 4u?*(( int*)
_temp1766) == Cyc_Absyn_PointerType: 0){ _LL1769: _temp1768=(( struct Cyc_Absyn_PointerType_struct*)
_temp1766)->f1; _LL1779: _temp1778=( void*) _temp1768.elt_typ; goto _LL1777;
_LL1777: _temp1776=( void*) _temp1768.rgn_typ; goto _LL1775; _LL1775: _temp1774=
_temp1768.nullable; goto _LL1773; _LL1773: _temp1772= _temp1768.tq; goto _LL1771;
_LL1771: _temp1770= _temp1768.bounds; goto _LL1753;} else{ goto _LL1750;}
_LL1753: _temp1752= _temp1746.f2; if(( unsigned int) _temp1752 > 4u?*(( int*)
_temp1752) == Cyc_Absyn_PointerType: 0){ _LL1755: _temp1754=(( struct Cyc_Absyn_PointerType_struct*)
_temp1752)->f1; _LL1765: _temp1764=( void*) _temp1754.elt_typ; goto _LL1763;
_LL1763: _temp1762=( void*) _temp1754.rgn_typ; goto _LL1761; _LL1761: _temp1760=
_temp1754.nullable; goto _LL1759; _LL1759: _temp1758= _temp1754.tq; goto _LL1757;
_LL1757: _temp1756= _temp1754.bounds; goto _LL1749;} else{ goto _LL1750;}
_LL1750: goto _LL1751; _LL1749:{ struct _tuple10 _temp1781=({ struct _tuple10
_temp1780; _temp1780.f1=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1770); _temp1780.f2=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1756); _temp1780;}); void*
_temp1787; struct Cyc_Absyn_Exp* _temp1789; void* _temp1791; _LL1783: _LL1792:
_temp1791= _temp1781.f1; if( _temp1791 ==( void*) Cyc_Absyn_Unknown_b){ goto
_LL1788;} else{ goto _LL1785;} _LL1788: _temp1787= _temp1781.f2; if((
unsigned int) _temp1787 > 1u?*(( int*) _temp1787) == Cyc_Absyn_Upper_b: 0){
_LL1790: _temp1789=(( struct Cyc_Absyn_Upper_b_struct*) _temp1787)->f1; goto
_LL1784;} else{ goto _LL1785;} _LL1785: goto _LL1786; _LL1784: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_List_List*(* _temp1793)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1796= mexp;
struct Cyc_Absyn_Exp* _temp1797= _temp1741; struct Cyc_Absyn_Exp* _temp1798= Cyc_Absyn_sizeoftyp_exp(
_temp1778, 0); struct Cyc_Absyn_Exp* _temp1799= _temp1789; struct Cyc_Absyn_Exp*
_temp1794[ 4u]={ _temp1796, _temp1797, _temp1798, _temp1799}; struct _tagged_arr
_temp1795={( void*) _temp1794,( void*) _temp1794,( void*)( _temp1794 + 4u)};
_temp1793( _temp1795);}), 0))->r)); goto _LL1782; _LL1786: goto _LL1782; _LL1782:;}
goto _LL1747; _LL1751: goto _LL1747; _LL1747:;} goto _LL1736; _LL1740: goto
_LL1736; _LL1736:;} if( ! done){ struct _tuple0* _temp1800= Cyc_Toc_temp_var();
goto _LL1801; _LL1801: { struct Cyc_Absyn_Stmt* _temp1802= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1800, 0), 0); goto _LL1803; _LL1803: { struct Cyc_Absyn_Exp*
_temp1804= Cyc_Absyn_signed_int_exp( 0, 0); goto _LL1805; _LL1805: Cyc_Toc_exp_to_c(
nv, _temp1217); _temp1802= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1800, 0), _temp1804, 0), _temp1217, 0), _temp1802, 0);{
void* _temp1806= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); goto _LL1807;
_LL1807:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1800, _temp1806,( struct Cyc_Absyn_Exp*) mexp, _temp1802, 0), 0))->r));}}}}
goto _LL1629;}} _LL1629:;} goto _LL1066; _LL1104: Cyc_Toc_exp_to_c( nv,
_temp1221); goto _LL1066; _LL1106:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1808=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1808[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1809; _temp1809.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1809.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1223);
_temp1809;}); _temp1808;}))); goto _LL1066; _LL1108:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1810=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1810[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1811; _temp1811.tag= Cyc_Absyn_Offsetof_e;
_temp1811.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1227); _temp1811.f2=
_temp1225; _temp1811;}); _temp1810;}))); goto _LL1066; _LL1110: { void*
_temp1812= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1229->topt))->v); goto _LL1813; _LL1813:{ void* _temp1814= _temp1812;
struct Cyc_Absyn_PtrInfo _temp1820; struct Cyc_Absyn_Conref* _temp1822; struct
Cyc_Absyn_Tqual _temp1824; struct Cyc_Absyn_Conref* _temp1826; void* _temp1828;
void* _temp1830; _LL1816: if(( unsigned int) _temp1814 > 4u?*(( int*) _temp1814)
== Cyc_Absyn_PointerType: 0){ _LL1821: _temp1820=(( struct Cyc_Absyn_PointerType_struct*)
_temp1814)->f1; _LL1831: _temp1830=( void*) _temp1820.elt_typ; goto _LL1829;
_LL1829: _temp1828=( void*) _temp1820.rgn_typ; goto _LL1827; _LL1827: _temp1826=
_temp1820.nullable; goto _LL1825; _LL1825: _temp1824= _temp1820.tq; goto _LL1823;
_LL1823: _temp1822= _temp1820.bounds; goto _LL1817;} else{ goto _LL1818;}
_LL1818: goto _LL1819; _LL1817:{ void* _temp1832=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1822); struct Cyc_Absyn_Exp*
_temp1838; _LL1834: if(( unsigned int) _temp1832 > 1u?*(( int*) _temp1832) ==
Cyc_Absyn_Upper_b: 0){ _LL1839: _temp1838=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1832)->f1; goto _LL1835;} else{ goto _LL1836;} _LL1836: if( _temp1832 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1837;} else{ goto _LL1833;} _LL1835: Cyc_Toc_exp_to_c(
nv, _temp1229); if( Cyc_Toc_is_nullable( _temp1812)){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1229->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1840=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1840->hd=( void*) _temp1229;
_temp1840->tl= 0; _temp1840;}), 0), 0), 0))->r));} goto _LL1833; _LL1837: {
struct Cyc_Absyn_Exp* _temp1841= Cyc_Absyn_uint_exp( 0, 0); goto _LL1842;
_LL1842: _temp1841->topt=({ struct Cyc_Core_Opt* _temp1843=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1843->v=( void*) Cyc_Absyn_uint_t;
_temp1843;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_subscript_exp(
_temp1229, _temp1841, 0))->r)); Cyc_Toc_exp_to_c( nv, e); goto _LL1833;} _LL1833:;}
goto _LL1815; _LL1819:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u)); goto
_LL1815; _LL1815:;} goto _LL1066;} _LL1112: Cyc_Toc_exp_to_c( nv, _temp1233);
if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1066; _LL1114: { void*
e1typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1237->topt))->v); Cyc_Toc_exp_to_c( nv, _temp1237);{ int is_poly= Cyc_Toc_is_poly_project(
e); struct Cyc_Absyn_Conref* _temp1846; struct Cyc_Absyn_Tqual _temp1848; struct
Cyc_Absyn_Conref* _temp1850; void* _temp1852; void* _temp1854; struct Cyc_Absyn_PtrInfo
_temp1844= Cyc_Toc_get_ptr_type( e1typ); _LL1855: _temp1854=( void*) _temp1844.elt_typ;
goto _LL1853; _LL1853: _temp1852=( void*) _temp1844.rgn_typ; goto _LL1851;
_LL1851: _temp1850= _temp1844.nullable; goto _LL1849; _LL1849: _temp1848=
_temp1844.tq; goto _LL1847; _LL1847: _temp1846= _temp1844.bounds; goto _LL1845;
_LL1845:{ void* _temp1856=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1846); struct Cyc_Absyn_Exp* _temp1862; _LL1858: if((
unsigned int) _temp1856 > 1u?*(( int*) _temp1856) == Cyc_Absyn_Upper_b: 0){
_LL1863: _temp1862=(( struct Cyc_Absyn_Upper_b_struct*) _temp1856)->f1; goto
_LL1859;} else{ goto _LL1860;} _LL1860: if( _temp1856 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1861;} else{ goto _LL1857;} _LL1859: if( Cyc_Evexp_eval_const_uint_exp(
_temp1862) < 1){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof( unsigned char), 46u));}{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( 0, _temp1850); if( Cyc_Toc_is_nullable(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1237->topt))->v)){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1237->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1864=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1864->hd=( void*) _temp1237;
_temp1864->tl= 0; _temp1864;}), 0), 0), _temp1235, 0))->r));} goto _LL1857;}
_LL1861: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1854);( void*)( _temp1237->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1848), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_List_List*(* _temp1865)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1868= Cyc_Absyn_copy_exp( _temp1237); struct Cyc_Absyn_Exp*
_temp1869= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp* _temp1870=
Cyc_Absyn_uint_exp( 0, 0); struct Cyc_Absyn_Exp* _temp1866[ 3u]={ _temp1868,
_temp1869, _temp1870}; struct _tagged_arr _temp1867={( void*) _temp1866,( void*)
_temp1866,( void*)( _temp1866 + 3u)}; _temp1865( _temp1867);}), 0), 0))->r));
goto _LL1857;} _LL1857:;} if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1066;}} _LL1116: { void*
_temp1871= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1241->topt))->v); goto _LL1872; _LL1872: Cyc_Toc_exp_to_c( nv, _temp1241);
Cyc_Toc_exp_to_c( nv, _temp1239);{ void* _temp1873= _temp1871; struct Cyc_List_List*
_temp1881; struct Cyc_Absyn_PtrInfo _temp1883; struct Cyc_Absyn_Conref*
_temp1885; struct Cyc_Absyn_Tqual _temp1887; struct Cyc_Absyn_Conref* _temp1889;
void* _temp1891; void* _temp1893; _LL1875: if(( unsigned int) _temp1873 > 4u?*((
int*) _temp1873) == Cyc_Absyn_TupleType: 0){ _LL1882: _temp1881=(( struct Cyc_Absyn_TupleType_struct*)
_temp1873)->f1; goto _LL1876;} else{ goto _LL1877;} _LL1877: if(( unsigned int)
_temp1873 > 4u?*(( int*) _temp1873) == Cyc_Absyn_PointerType: 0){ _LL1884:
_temp1883=(( struct Cyc_Absyn_PointerType_struct*) _temp1873)->f1; _LL1894:
_temp1893=( void*) _temp1883.elt_typ; goto _LL1892; _LL1892: _temp1891=( void*)
_temp1883.rgn_typ; goto _LL1890; _LL1890: _temp1889= _temp1883.nullable; goto
_LL1888; _LL1888: _temp1887= _temp1883.tq; goto _LL1886; _LL1886: _temp1885=
_temp1883.bounds; goto _LL1878;} else{ goto _LL1879;} _LL1879: goto _LL1880;
_LL1876: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1239) + 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1241, Cyc_Toc_fieldname((
int) i), 0))->r)); goto _LL1874;} _LL1878:{ void* _temp1895=(( void*(*)( void*,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1885); struct Cyc_Absyn_Exp* _temp1901; _LL1897: if(( unsigned int)
_temp1895 > 1u?*(( int*) _temp1895) == Cyc_Absyn_Upper_b: 0){ _LL1902: _temp1901=((
struct Cyc_Absyn_Upper_b_struct*) _temp1895)->f1; goto _LL1898;} else{ goto
_LL1899;} _LL1899: if( _temp1895 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1900;}
else{ goto _LL1896;} _LL1898: _temp1901= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1901), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1889); void* ta1= Cyc_Toc_typ_to_c( _temp1893);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1887); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_List_List*(* _temp1903)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1906= _temp1241; struct Cyc_Absyn_Exp* _temp1907=
_temp1901; struct Cyc_Absyn_Exp* _temp1908= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
struct Cyc_Absyn_Exp* _temp1909= _temp1239; struct Cyc_Absyn_Exp* _temp1904[ 4u]={
_temp1906, _temp1907, _temp1908, _temp1909}; struct _tagged_arr _temp1905={(
void*) _temp1904,( void*) _temp1904,( void*)( _temp1904 + 4u)}; _temp1903(
_temp1905);}), 0), 0), 0))->r));} else{( void*)( _temp1239->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_List_List*(*
_temp1910)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp1913= _temp1901; struct Cyc_Absyn_Exp*
_temp1914= Cyc_Absyn_copy_exp( _temp1239); struct Cyc_Absyn_Exp* _temp1911[ 2u]={
_temp1913, _temp1914}; struct _tagged_arr _temp1912={( void*) _temp1911,( void*)
_temp1911,( void*)( _temp1911 + 2u)}; _temp1910( _temp1912);}), 0))->r));} goto
_LL1896;} _LL1900: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1893);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
ta1, _temp1887), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_List_List*(* _temp1915)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1918= _temp1241;
struct Cyc_Absyn_Exp* _temp1919= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp1920= _temp1239; struct Cyc_Absyn_Exp* _temp1916[ 3u]={ _temp1918,
_temp1919, _temp1920}; struct _tagged_arr _temp1917={( void*) _temp1916,( void*)
_temp1916,( void*)( _temp1916 + 3u)}; _temp1915( _temp1917);}), 0), 0), 0))->r));
goto _LL1896;} _LL1896:;} goto _LL1874; _LL1880:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u)); goto _LL1874; _LL1874:;} goto _LL1066;} _LL1118:
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 0,
0, _temp1243))->r));} else{ struct Cyc_List_List* _temp1921=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, _temp1243); goto _LL1922; _LL1922: { struct _tagged_arr*
_temp1923= Cyc_Toc_add_tuple_type( _temp1921); goto _LL1924; _LL1924: { struct
Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1243 != 0;( _temp1243=(( struct
Cyc_List_List*) _check_null( _temp1243))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1243))->hd);
dles=({ struct Cyc_List_List* _temp1925=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1925->hd=( void*)({ struct _tuple4*
_temp1926=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1926->f1=
0; _temp1926->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1243))->hd; _temp1926;}); _temp1925->tl= dles; _temp1925;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}}}
goto _LL1066; _LL1120:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1245, 0))->r));{ struct Cyc_List_List* _temp1927= _temp1245; goto
_LL1928; _LL1928: for( 0; _temp1927 != 0; _temp1927=(( struct Cyc_List_List*)
_check_null( _temp1927))->tl){ struct _tuple4 _temp1931; struct Cyc_Absyn_Exp*
_temp1932; struct Cyc_List_List* _temp1934; struct _tuple4* _temp1929=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp1927))->hd; _temp1931=*
_temp1929; _LL1935: _temp1934= _temp1931.f1; goto _LL1933; _LL1933: _temp1932=
_temp1931.f2; goto _LL1930; _LL1930: Cyc_Toc_exp_to_c( nv, _temp1932);}} goto
_LL1066; _LL1122: { unsigned int _temp1936= Cyc_Evexp_eval_const_uint_exp(
_temp1249); goto _LL1937; _LL1937: Cyc_Toc_exp_to_c( nv, _temp1247);{ struct Cyc_List_List*
es= 0; if( ! Cyc_Toc_is_zero( _temp1247)){ unsigned int i= 0; for( 0; i <
_temp1936; i ++){ es=({ struct Cyc_List_List* _temp1938=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1938->hd=( void*)({ struct
_tuple4* _temp1939=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1939->f1= 0; _temp1939->f2= _temp1247; _temp1939;}); _temp1938->tl= es;
_temp1938;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
es, 0))->r)); goto _LL1066;}} _LL1124: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1255, _temp1259))->r));}
else{ if( _temp1253 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Struct_e: missing structdecl pointer", sizeof( unsigned char), 37u));}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp1253); struct _RegionHandle _temp1940= _new_region(); struct _RegionHandle*
rgn=& _temp1940; _push_region( rgn);{ struct Cyc_List_List* _temp1941=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1255,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); goto _LL1942; _LL1942: { struct Cyc_List_List*
_temp1943= 0; goto _LL1944; _LL1944:{ struct Cyc_List_List* _temp1945=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; goto
_LL1946; _LL1946: for( 0; _temp1945 != 0; _temp1945=(( struct Cyc_List_List*)
_check_null( _temp1945))->tl){ struct Cyc_List_List* _temp1947= _temp1941; goto
_LL1948; _LL1948: for( 0; _temp1947 != 0; _temp1947=(( struct Cyc_List_List*)
_check_null( _temp1947))->tl){ if((*(( struct _tuple11*)(( struct Cyc_List_List*)
_check_null( _temp1947))->hd)).f1 ==( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp1945))->hd){ struct _tuple11 _temp1951; struct
Cyc_Absyn_Exp* _temp1952; struct Cyc_Absyn_Structfield* _temp1954; struct
_tuple11* _temp1949=( struct _tuple11*)(( struct Cyc_List_List*) _check_null(
_temp1947))->hd; _temp1951=* _temp1949; _LL1955: _temp1954= _temp1951.f1; goto
_LL1953; _LL1953: _temp1952= _temp1951.f2; goto _LL1950; _LL1950: { void*
_temp1956=( void*) _temp1954->type; goto _LL1957; _LL1957: Cyc_Toc_exp_to_c( nv,
_temp1952); if( Cyc_Toc_is_void_star( _temp1956)){( void*)( _temp1952->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1952->r, 0), 0))->r));} _temp1943=({ struct Cyc_List_List* _temp1958=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1958->hd=(
void*)({ struct _tuple4* _temp1959=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1959->f1= 0; _temp1959->f2= _temp1952; _temp1959;}); _temp1958->tl=
_temp1943; _temp1958;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1943), 0))->r));}}; _pop_region( rgn);}} goto _LL1066; _LL1126: { struct
Cyc_List_List* fs;{ void* _temp1960= Cyc_Tcutil_compress( _temp1263); struct Cyc_List_List*
_temp1966; _LL1962: if(( unsigned int) _temp1960 > 4u?*(( int*) _temp1960) ==
Cyc_Absyn_AnonStructType: 0){ _LL1967: _temp1966=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1960)->f1; goto _LL1963;} else{ goto _LL1964;} _LL1964: goto _LL1965;
_LL1963: fs= _temp1966; goto _LL1961; _LL1965: fs=(( struct Cyc_List_List*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp1968= Cyc_Absynpp_typ2string( _temp1263); xprintf("anon struct has type %.*s",
_get_arr_size( _temp1968, 1u), _temp1968.curr);})); goto _LL1961; _LL1961:;}{
struct _RegionHandle _temp1969= _new_region(); struct _RegionHandle* rgn=&
_temp1969; _push_region( rgn);{ struct Cyc_List_List* _temp1970=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1261, fs); goto _LL1971; _LL1971: for( 0; _temp1970 != 0; _temp1970=((
struct Cyc_List_List*) _check_null( _temp1970))->tl){ struct _tuple11 _temp1974;
struct Cyc_Absyn_Exp* _temp1975; struct Cyc_Absyn_Structfield* _temp1977; struct
_tuple11* _temp1972=( struct _tuple11*)(( struct Cyc_List_List*) _check_null(
_temp1970))->hd; _temp1974=* _temp1972; _LL1978: _temp1977= _temp1974.f1; goto
_LL1976; _LL1976: _temp1975= _temp1974.f2; goto _LL1973; _LL1973: { void*
_temp1979=( void*) _temp1977->type; goto _LL1980; _LL1980: Cyc_Toc_exp_to_c( nv,
_temp1975); if( Cyc_Toc_is_void_star( _temp1979)){( void*)( _temp1975->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1975->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1261, 0))->r));}; _pop_region( rgn);} goto _LL1066;} _LL1128: { struct
_tuple0* qv= _temp1265->name; if( _temp1267->is_xtunion? 1: ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* _temp1981= _temp1267->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1267->fields))->v;
goto _LL1982; _LL1982: while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1981))->hd)->name) != 0) { if((( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp1981))->hd)->typs
== 0){ tag_count ++;} _temp1981=(( struct Cyc_List_List*) _check_null( _temp1981))->tl;}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));}
goto _LL1066;} _LL1130: { struct _tuple0* _temp1983= _temp1275->name; goto
_LL1984; _LL1984: { struct Cyc_List_List* _temp1985= _temp1275->typs; goto
_LL1986; _LL1986: { struct _tuple0* _temp1987= Cyc_Toc_temp_var(); goto _LL1988;
_LL1988: { struct Cyc_Absyn_Exp* _temp1989= Cyc_Absyn_var_exp( _temp1987, 0);
goto _LL1990; _LL1990: { void* _temp1991= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp1983, _tag_arr("_struct", sizeof( unsigned char), 8u))); goto _LL1992;
_LL1992: if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp; if( _temp1277->is_xtunion){
tag_exp= Cyc_Absyn_var_exp( _temp1983, 0);} else{ int tag_count= 0; struct Cyc_List_List*
_temp1993= _temp1277->fields == 0? 0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1277->fields))->v; goto _LL1994; _LL1994: while( Cyc_Absyn_qvar_cmp(
_temp1983,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1993))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1993))->hd)->typs != 0){ tag_count ++;}
_temp1993=(( struct Cyc_List_List*) _check_null( _temp1993))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp1995= 0; goto _LL1996;
_LL1996: for( 0; _temp1279 != 0;( _temp1279=(( struct Cyc_List_List*)
_check_null( _temp1279))->tl, _temp1985=(( struct Cyc_List_List*) _check_null(
_temp1985))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1279))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1985))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} _temp1995=({ struct Cyc_List_List* _temp1997=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1997[ 0]=({ struct Cyc_List_List
_temp1998; _temp1998.hd=( void*)({ struct _tuple4* _temp1999=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4) * 1); _temp1999[ 0]=({ struct _tuple4
_temp2000; _temp2000.f1= 0; _temp2000.f2= cur_e; _temp2000;}); _temp1999;});
_temp1998.tl= _temp1995; _temp1998;}); _temp1997;});} _temp1995=({ struct Cyc_List_List*
_temp2001=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2001[ 0]=({ struct Cyc_List_List _temp2002; _temp2002.hd=( void*)({ struct
_tuple4* _temp2003=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp2003[ 0]=({ struct _tuple4 _temp2004; _temp2004.f1= 0; _temp2004.f2=
tag_exp; _temp2004;}); _temp2003;}); _temp2002.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1995); _temp2002;}); _temp2001;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp2005=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2005->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1983, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp2005;}), _temp1995, 0))->r));}} else{ struct
Cyc_List_List* _temp2007=({ struct Cyc_List_List* _temp2006=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2006->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1989, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1983, 0), 0); _temp2006->tl= 0; _temp2006;}); goto _LL2008; _LL2008:{ int i=
1; for( 0; _temp1279 != 0;((( _temp1279=(( struct Cyc_List_List*) _check_null(
_temp1279))->tl, i ++)), _temp1985=(( struct Cyc_List_List*) _check_null(
_temp1985))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1279))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1985))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp2009= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1989, Cyc_Toc_fieldname( i), 0), cur_e, 0);
goto _LL2010; _LL2010: _temp2007=({ struct Cyc_List_List* _temp2011=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2011->hd=( void*) _temp2009;
_temp2011->tl= _temp2007; _temp2011;});}}}{ struct Cyc_Absyn_Stmt* _temp2012=
Cyc_Absyn_exp_stmt( _temp1989, 0); goto _LL2013; _LL2013: { struct Cyc_Absyn_Stmt*
_temp2015= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp2014=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2014->hd=( void*) _temp2012;
_temp2014->tl= _temp2007; _temp2014;})), 0); goto _LL2016; _LL2016:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1987,
_temp1991, 0, _temp2015, 0), 0))->r));}}} goto _LL1066;}}}}} _LL1132: goto
_LL1066; _LL1134: { void* t_c= Cyc_Toc_typ_to_c( _temp1291); if( _temp1293 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1293); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1291, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL1066;} _LL1136: Cyc_Toc_stmt_to_c(
nv, _temp1295); goto _LL1066; _LL1138:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem", sizeof( unsigned char), 14u)); goto _LL1066; _LL1140:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u)); goto _LL1066; _LL1142:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u)); goto _LL1066;
_LL1144:(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("fill",
sizeof( unsigned char), 5u)); goto _LL1066; _LL1066:;}} static struct Cyc_Absyn_Stmt*
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
_temp2017=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp2053; struct Cyc_Absyn_Vardecl
_temp2055; struct Cyc_List_List* _temp2056; struct Cyc_Core_Opt* _temp2058;
struct Cyc_Absyn_Exp* _temp2060; void* _temp2062; struct Cyc_Absyn_Tqual
_temp2064; struct _tuple0* _temp2066; void* _temp2068; struct Cyc_Absyn_Vardecl*
_temp2070; int _temp2072; void* _temp2074; unsigned char _temp2076; struct
_tagged_arr _temp2078; struct Cyc_Absyn_Enumfield* _temp2080; struct Cyc_Absyn_Enumdecl*
_temp2082; struct Cyc_List_List* _temp2084; struct Cyc_List_List* _temp2086;
struct Cyc_Absyn_Tunionfield* _temp2088; struct Cyc_Absyn_Tuniondecl* _temp2090;
struct Cyc_Absyn_Pat* _temp2093; struct Cyc_Absyn_Pat _temp2095; struct Cyc_Position_Segment*
_temp2096; struct Cyc_Core_Opt* _temp2098; void* _temp2100; struct Cyc_List_List*
_temp2102; struct Cyc_List_List* _temp2104; struct Cyc_Absyn_Tunionfield*
_temp2106; struct Cyc_Absyn_Tuniondecl* _temp2108; struct Cyc_List_List*
_temp2110; struct Cyc_List_List* _temp2112; struct Cyc_Absyn_Tunionfield*
_temp2114; struct Cyc_Absyn_Tuniondecl* _temp2116; struct Cyc_List_List*
_temp2118; struct Cyc_List_List* _temp2120; struct Cyc_List_List* _temp2122;
struct Cyc_Core_Opt* _temp2124; struct Cyc_Absyn_Structdecl* _temp2126; struct
Cyc_Absyn_Pat* _temp2128; struct _tuple0* _temp2130; struct Cyc_List_List*
_temp2132; struct Cyc_List_List* _temp2134; struct _tuple0* _temp2136; struct
Cyc_List_List* _temp2138; struct Cyc_List_List* _temp2140; struct _tuple0*
_temp2142; _LL2019: if(( unsigned int) _temp2017 > 2u?*(( int*) _temp2017) ==
Cyc_Absyn_Var_p: 0){ _LL2054: _temp2053=(( struct Cyc_Absyn_Var_p_struct*)
_temp2017)->f1; _temp2055=* _temp2053; _LL2069: _temp2068=( void*) _temp2055.sc;
goto _LL2067; _LL2067: _temp2066= _temp2055.name; goto _LL2065; _LL2065:
_temp2064= _temp2055.tq; goto _LL2063; _LL2063: _temp2062=( void*) _temp2055.type;
goto _LL2061; _LL2061: _temp2060= _temp2055.initializer; goto _LL2059; _LL2059:
_temp2058= _temp2055.rgn; goto _LL2057; _LL2057: _temp2056= _temp2055.attributes;
goto _LL2020;} else{ goto _LL2021;} _LL2021: if( _temp2017 ==( void*) Cyc_Absyn_Wild_p){
goto _LL2022;} else{ goto _LL2023;} _LL2023: if(( unsigned int) _temp2017 > 2u?*((
int*) _temp2017) == Cyc_Absyn_Reference_p: 0){ _LL2071: _temp2070=(( struct Cyc_Absyn_Reference_p_struct*)
_temp2017)->f1; goto _LL2024;} else{ goto _LL2025;} _LL2025: if( _temp2017 ==(
void*) Cyc_Absyn_Null_p){ goto _LL2026;} else{ goto _LL2027;} _LL2027: if((
unsigned int) _temp2017 > 2u?*(( int*) _temp2017) == Cyc_Absyn_Int_p: 0){
_LL2075: _temp2074=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp2017)->f1;
goto _LL2073; _LL2073: _temp2072=(( struct Cyc_Absyn_Int_p_struct*) _temp2017)->f2;
goto _LL2028;} else{ goto _LL2029;} _LL2029: if(( unsigned int) _temp2017 > 2u?*((
int*) _temp2017) == Cyc_Absyn_Char_p: 0){ _LL2077: _temp2076=(( struct Cyc_Absyn_Char_p_struct*)
_temp2017)->f1; goto _LL2030;} else{ goto _LL2031;} _LL2031: if(( unsigned int)
_temp2017 > 2u?*(( int*) _temp2017) == Cyc_Absyn_Float_p: 0){ _LL2079: _temp2078=((
struct Cyc_Absyn_Float_p_struct*) _temp2017)->f1; goto _LL2032;} else{ goto
_LL2033;} _LL2033: if(( unsigned int) _temp2017 > 2u?*(( int*) _temp2017) == Cyc_Absyn_Enum_p:
0){ _LL2083: _temp2082=(( struct Cyc_Absyn_Enum_p_struct*) _temp2017)->f1; goto
_LL2081; _LL2081: _temp2080=(( struct Cyc_Absyn_Enum_p_struct*) _temp2017)->f2;
goto _LL2034;} else{ goto _LL2035;} _LL2035: if(( unsigned int) _temp2017 > 2u?*((
int*) _temp2017) == Cyc_Absyn_Tunion_p: 0){ _LL2091: _temp2090=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2017)->f1; goto _LL2089; _LL2089: _temp2088=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2017)->f2; goto _LL2087; _LL2087: _temp2086=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2017)->f3; goto _LL2085; _LL2085: _temp2084=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp2017)->f4; if( _temp2084 == 0){ goto _LL2036;} else{ goto _LL2037;}} else{
goto _LL2037;} _LL2037: if(( unsigned int) _temp2017 > 2u?*(( int*) _temp2017)
== Cyc_Absyn_Pointer_p: 0){ _LL2094: _temp2093=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2017)->f1; _temp2095=* _temp2093; _LL2101: _temp2100=( void*) _temp2095.r;
if(( unsigned int) _temp2100 > 2u?*(( int*) _temp2100) == Cyc_Absyn_Tunion_p: 0){
_LL2109: _temp2108=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2100)->f1; goto
_LL2107; _LL2107: _temp2106=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2100)->f2;
goto _LL2105; _LL2105: _temp2104=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2100)->f3;
goto _LL2103; _LL2103: _temp2102=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2100)->f4;
goto _LL2099;} else{ goto _LL2039;} _LL2099: _temp2098= _temp2095.topt; goto
_LL2097; _LL2097: _temp2096= _temp2095.loc; goto _LL2092;} else{ goto _LL2039;}
_LL2092: if( _temp2102 != 0){ goto _LL2038;} else{ goto _LL2039;} _LL2039: if((
unsigned int) _temp2017 > 2u?*(( int*) _temp2017) == Cyc_Absyn_Tunion_p: 0){
_LL2117: _temp2116=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2017)->f1; goto
_LL2115; _LL2115: _temp2114=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2017)->f2;
goto _LL2113; _LL2113: _temp2112=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2017)->f3;
goto _LL2111; _LL2111: _temp2110=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2017)->f4;
goto _LL2040;} else{ goto _LL2041;} _LL2041: if(( unsigned int) _temp2017 > 2u?*((
int*) _temp2017) == Cyc_Absyn_Tuple_p: 0){ _LL2119: _temp2118=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp2017)->f1; goto _LL2042;} else{ goto _LL2043;} _LL2043: if(( unsigned int)
_temp2017 > 2u?*(( int*) _temp2017) == Cyc_Absyn_Struct_p: 0){ _LL2127:
_temp2126=(( struct Cyc_Absyn_Struct_p_struct*) _temp2017)->f1; goto _LL2125;
_LL2125: _temp2124=(( struct Cyc_Absyn_Struct_p_struct*) _temp2017)->f2; goto
_LL2123; _LL2123: _temp2122=(( struct Cyc_Absyn_Struct_p_struct*) _temp2017)->f3;
goto _LL2121; _LL2121: _temp2120=(( struct Cyc_Absyn_Struct_p_struct*) _temp2017)->f4;
goto _LL2044;} else{ goto _LL2045;} _LL2045: if(( unsigned int) _temp2017 > 2u?*((
int*) _temp2017) == Cyc_Absyn_Pointer_p: 0){ _LL2129: _temp2128=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2017)->f1; goto _LL2046;} else{ goto _LL2047;} _LL2047: if(( unsigned int)
_temp2017 > 2u?*(( int*) _temp2017) == Cyc_Absyn_UnknownId_p: 0){ _LL2131:
_temp2130=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp2017)->f1; goto _LL2048;}
else{ goto _LL2049;} _LL2049: if(( unsigned int) _temp2017 > 2u?*(( int*)
_temp2017) == Cyc_Absyn_UnknownCall_p: 0){ _LL2137: _temp2136=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2017)->f1; goto _LL2135; _LL2135: _temp2134=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2017)->f2; goto _LL2133; _LL2133: _temp2132=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2017)->f3; goto _LL2050;} else{ goto _LL2051;} _LL2051: if(( unsigned int)
_temp2017 > 2u?*(( int*) _temp2017) == Cyc_Absyn_UnknownFields_p: 0){ _LL2143:
_temp2142=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2017)->f1; goto
_LL2141; _LL2141: _temp2140=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2017)->f2; goto _LL2139; _LL2139: _temp2138=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2017)->f3; goto _LL2052;} else{ goto _LL2018;} _LL2020: nv= Cyc_Toc_add_varmap(
nv, _temp2066, r); goto _LL2022; _LL2022: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL2018; _LL2024: { struct _tuple0* _temp2144= Cyc_Toc_temp_var(); goto
_LL2145; _LL2145: decls=({ struct Cyc_List_List* _temp2146=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2146->hd=( void*)({
struct _tuple13* _temp2147=( struct _tuple13*) _region_malloc( rgn, sizeof(
struct _tuple13)); _temp2147->f1= _temp2144; _temp2147->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2147;}); _temp2146->tl= decls;
_temp2146;}); nv= Cyc_Toc_add_varmap( nv, _temp2070->name, Cyc_Absyn_var_exp(
_temp2144, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2144, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL2018;} _LL2026: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL2018; _LL2028: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp2074, _temp2072, 0), succ_lab, fail_lab); goto _LL2018;
_LL2030: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp2076, 0),
succ_lab, fail_lab); goto _LL2018; _LL2032: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp2078, 0), succ_lab, fail_lab); goto _LL2018; _LL2034: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2148=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp2148[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp2149; _temp2149.tag= Cyc_Absyn_Enum_e;
_temp2149.f1= _temp2080->name; _temp2149.f2=( struct Cyc_Absyn_Enumdecl*)
_temp2082; _temp2149.f3=( struct Cyc_Absyn_Enumfield*) _temp2080; _temp2149;});
_temp2148;}), 0), succ_lab, fail_lab); goto _LL2018; _LL2036: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp2088->name, 0); if( ! _temp2090->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL2018;} _LL2038: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2102);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp2106->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp2150=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2106->typs); goto _LL2151;
_LL2151: { struct Cyc_List_List* _temp2152=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2102); goto
_LL2153; _LL2153: for( 0; _temp2152 != 0;((( _temp2152=(( struct Cyc_List_List*)
_check_null( _temp2152))->tl, _temp2150=(( struct Cyc_List_List*) _check_null(
_temp2150))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp2154=( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp2152))->hd; goto _LL2155; _LL2155: {
void* _temp2156=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2150))->hd)).f2; goto _LL2157; _LL2157: { struct _tuple0* _temp2158= Cyc_Toc_temp_var();
goto _LL2159; _LL2159: { void* _temp2160=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2154->topt))->v; goto _LL2161; _LL2161: { void* _temp2162= Cyc_Toc_typ_to_c(
_temp2160); goto _LL2163; _LL2163: { struct _tagged_arr* _temp2164= Cyc_Toc_fresh_label();
goto _LL2165; _LL2165: { struct Cyc_Absyn_Exp* _temp2166= Cyc_Absyn_structarrow_exp(
rcast, Cyc_Toc_fieldname( cnt), 0); goto _LL2167; _LL2167: if( Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c( _temp2156))){ _temp2166= Cyc_Absyn_cast_exp( _temp2162,
_temp2166, 0);} decls=({ struct Cyc_List_List* _temp2168=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2168->hd=( void*)({
struct _tuple13* _temp2169=( struct _tuple13*) _region_malloc( rgn, sizeof(
struct _tuple13)); _temp2169->f1= _temp2158; _temp2169->f2= _temp2162; _temp2169;});
_temp2168->tl= decls; _temp2168;});{ struct _tuple12 _temp2170= Cyc_Toc_xlate_pat(
nv, rgn, _temp2160, Cyc_Absyn_var_exp( _temp2158, 0), _temp2166, _temp2154,
succ_lab, fail_lab, decls); goto _LL2171; _LL2171: nv= _temp2170.f1; decls=
_temp2170.f2;{ struct Cyc_Absyn_Stmt* _temp2172= _temp2170.f3; goto _LL2173;
_LL2173: { struct Cyc_Absyn_Stmt* _temp2174= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2158, 0), _temp2166, 0); goto _LL2175; _LL2175: { struct Cyc_Absyn_Stmt*
_temp2176= Cyc_Absyn_seq_stmt( _temp2174, _temp2172, 0); goto _LL2177; _LL2177:
ss=({ struct Cyc_List_List* _temp2178=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2178->hd=( void*) Cyc_Absyn_label_stmt(
_temp2164, _temp2176, 0); _temp2178->tl= ss; _temp2178;}); succ_lab= _temp2164;}}}}}}}}}}}{
struct Cyc_Absyn_Exp* test_exp; if( _temp2108->is_xtunion){ struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0); struct Cyc_Absyn_Exp*
e= Cyc_Absyn_var_exp( _temp2106->name, 0); test_exp= Cyc_Absyn_eq_exp( e1, e, 0);
s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp2179=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2108->fields))->v; goto _LL2180; _LL2180: for( 0; _temp2179 !=
0; _temp2179=(( struct Cyc_List_List*) _check_null( _temp2179))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2181=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2179))->hd; goto _LL2182; _LL2182: if( _temp2181->typs == 0){ max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r,
0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp,
0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp2106->name, 0); test_exp=
max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL2018;}}} _LL2040: _temp2118=
_temp2110; goto _LL2042; _LL2042: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2118);{ struct Cyc_List_List*
_temp2183=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2118); goto _LL2184; _LL2184: for( 0; _temp2183 !=
0;( _temp2183=(( struct Cyc_List_List*) _check_null( _temp2183))->tl, cnt --)){
struct Cyc_Absyn_Pat* _temp2185=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2183))->hd; goto _LL2186; _LL2186: { struct _tuple0* _temp2187=
Cyc_Toc_temp_var(); goto _LL2188; _LL2188: { void* _temp2189=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2185->topt))->v; goto _LL2190; _LL2190: {
struct _tagged_arr* _temp2191= Cyc_Toc_fresh_label(); goto _LL2192; _LL2192:
decls=({ struct Cyc_List_List* _temp2193=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2193->hd=( void*)({ struct _tuple13*
_temp2194=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2194->f1= _temp2187; _temp2194->f2= Cyc_Toc_typ_to_c( _temp2189); _temp2194;});
_temp2193->tl= decls; _temp2193;});{ struct _tuple12 _temp2195= Cyc_Toc_xlate_pat(
nv, rgn, _temp2189, Cyc_Absyn_var_exp( _temp2187, 0), Cyc_Absyn_structmember_exp(
path, Cyc_Toc_fieldname( cnt), 0), _temp2185, succ_lab, fail_lab, decls); goto
_LL2196; _LL2196: nv= _temp2195.f1; decls= _temp2195.f2;{ struct Cyc_Absyn_Stmt*
_temp2197= _temp2195.f3; goto _LL2198; _LL2198: { struct Cyc_Absyn_Stmt*
_temp2199= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2187, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2197, 0); goto _LL2200; _LL2200: ss=({ struct Cyc_List_List* _temp2201=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2201->hd=(
void*) Cyc_Absyn_label_stmt( _temp2191, _temp2199, 0); _temp2201->tl= ss;
_temp2201;}); succ_lab= _temp2191;}}}}}}}} s= Cyc_Absyn_seq_stmts( ss, 0); goto
_LL2018;} _LL2044: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp2202=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp2120); goto _LL2203; _LL2203: for( 0; _temp2202 !=
0; _temp2202=(( struct Cyc_List_List*) _check_null( _temp2202))->tl){ struct
_tuple14* _temp2204=( struct _tuple14*)(( struct Cyc_List_List*) _check_null(
_temp2202))->hd; goto _LL2205; _LL2205: { struct Cyc_Absyn_Pat* _temp2206=(*
_temp2204).f2; goto _LL2207; _LL2207: { struct _tagged_arr* f;{ void* _temp2208=(
void*)(( struct Cyc_List_List*) _check_null((* _temp2204).f1))->hd; struct
_tagged_arr* _temp2214; _LL2210: if(*(( int*) _temp2208) == Cyc_Absyn_FieldName){
_LL2215: _temp2214=(( struct Cyc_Absyn_FieldName_struct*) _temp2208)->f1; goto
_LL2211;} else{ goto _LL2212;} _LL2212: goto _LL2213; _LL2211: f= _temp2214;
goto _LL2209; _LL2213:( int) _throw(( void*) Cyc_Toc_Match_error); _LL2209:;}{
struct _tuple0* _temp2216= Cyc_Toc_temp_var(); goto _LL2217; _LL2217: { void*
_temp2218=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2206->topt))->v;
goto _LL2219; _LL2219: { void* _temp2220= Cyc_Toc_typ_to_c( _temp2218); goto
_LL2221; _LL2221: { struct _tagged_arr* _temp2222= Cyc_Toc_fresh_label(); goto
_LL2223; _LL2223: decls=({ struct Cyc_List_List* _temp2224=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2224->hd=( void*)({
struct _tuple13* _temp2225=( struct _tuple13*) _region_malloc( rgn, sizeof(
struct _tuple13)); _temp2225->f1= _temp2216; _temp2225->f2= _temp2220; _temp2225;});
_temp2224->tl= decls; _temp2224;});{ struct _tuple12 _temp2226= Cyc_Toc_xlate_pat(
nv, rgn, _temp2218, Cyc_Absyn_var_exp( _temp2216, 0), Cyc_Absyn_structmember_exp(
path, f, 0), _temp2206, succ_lab, fail_lab, decls); goto _LL2227; _LL2227: nv=
_temp2226.f1; decls= _temp2226.f2;{ struct Cyc_Absyn_Exp* _temp2228= Cyc_Absyn_structmember_exp(
r, f, 0); goto _LL2229; _LL2229: if( Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2126->fields))->v, f)))->type)){ _temp2228= Cyc_Absyn_cast_exp(
_temp2220, _temp2228, 0);}{ struct Cyc_Absyn_Stmt* _temp2230= _temp2226.f3; goto
_LL2231; _LL2231: { struct Cyc_Absyn_Stmt* _temp2232= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2216, 0), _temp2228, 0), _temp2230, 0); goto _LL2233;
_LL2233: ss=({ struct Cyc_List_List* _temp2234=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2234->hd=( void*) Cyc_Absyn_label_stmt(
_temp2222, _temp2232, 0); _temp2234->tl= ss; _temp2234;}); succ_lab= _temp2222;}}}}}}}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL2018;} _LL2046: { struct _tuple0*
_temp2235= Cyc_Toc_temp_var(); goto _LL2236; _LL2236: { void* _temp2237=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2128->topt))->v; goto _LL2238; _LL2238:
decls=({ struct Cyc_List_List* _temp2239=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2239->hd=( void*)({ struct _tuple13*
_temp2240=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2240->f1= _temp2235; _temp2240->f2= Cyc_Toc_typ_to_c( _temp2237); _temp2240;});
_temp2239->tl= decls; _temp2239;});{ struct _tuple12 _temp2241= Cyc_Toc_xlate_pat(
nv, rgn, _temp2237, Cyc_Absyn_var_exp( _temp2235, 0), Cyc_Absyn_deref_exp( path,
0), _temp2128, succ_lab, fail_lab, decls); goto _LL2242; _LL2242: nv= _temp2241.f1;
decls= _temp2241.f2;{ struct Cyc_Absyn_Stmt* _temp2243= _temp2241.f3; goto
_LL2244; _LL2244: { struct Cyc_Absyn_Stmt* _temp2245= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2235, 0), Cyc_Absyn_deref_exp( r, 0), 0), _temp2243, 0);
goto _LL2246; _LL2246: if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2245, 0);} else{ s= _temp2245;} goto _LL2018;}}}}} _LL2048: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid",
sizeof( unsigned char), 10u)); goto _LL2018; _LL2050: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall", sizeof(
unsigned char), 12u)); goto _LL2018; _LL2052: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields", sizeof(
unsigned char), 14u)); goto _LL2018; _LL2018:;} return({ struct _tuple12
_temp2247; _temp2247.f1= nv; _temp2247.f2= decls; _temp2247.f3= s; _temp2247;});}
struct _tuple15{ struct _tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple15* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple15* _temp2248=(
struct _tuple15*) _region_malloc( r, sizeof( struct _tuple15)); _temp2248->f1=
Cyc_Toc_fresh_label(); _temp2248->f2= Cyc_Toc_fresh_label(); _temp2248->f3= sc;
_temp2248;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp2249=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; goto _LL2250; _LL2250: { int leave_as_switch;{ void*
_temp2251= Cyc_Tcutil_compress( _temp2249); void* _temp2257; void* _temp2259;
_LL2253: if(( unsigned int) _temp2251 > 4u?*(( int*) _temp2251) == Cyc_Absyn_IntType:
0){ _LL2260: _temp2259=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2251)->f1;
goto _LL2258; _LL2258: _temp2257=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2251)->f2; goto _LL2254;} else{ goto _LL2255;} _LL2255: goto _LL2256;
_LL2254: leave_as_switch= 1; goto _LL2252; _LL2256: leave_as_switch= 0; goto
_LL2252; _LL2252:;}{ struct Cyc_List_List* _temp2261= scs; goto _LL2262; _LL2262:
for( 0; _temp2261 != 0; _temp2261=(( struct Cyc_List_List*) _check_null(
_temp2261))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2261))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2261))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2263= scs; goto _LL2264;
_LL2264: for( 0; _temp2263 != 0; _temp2263=(( struct Cyc_List_List*) _check_null(
_temp2263))->tl){ struct Cyc_Absyn_Stmt* _temp2265=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2263))->hd)->body; goto _LL2266;
_LL2266:(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp2263))->hd)->body= Cyc_Absyn_label_stmt( next_l, _temp2265, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), _temp2265);}}
return;}{ struct _tuple0* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct _tagged_arr*
end_l= Cyc_Toc_fresh_label(); struct _RegionHandle _temp2267= _new_region();
struct _RegionHandle* rgn=& _temp2267; _push_region( rgn);{ struct Cyc_List_List*
lscs=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple15*(* f)(
struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*), struct _RegionHandle*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label, rgn,
scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0; struct
Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2268= lscs; goto _LL2269;
_LL2269: for( 0; _temp2268 != 0; _temp2268=(( struct Cyc_List_List*) _check_null(
_temp2268))->tl){ struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( _temp2268))->hd)).f3; struct _tagged_arr*
fail_lab=(( struct Cyc_List_List*) _check_null( _temp2268))->tl == 0? end_l:(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp2268))->tl))->hd)).f1; struct _tagged_arr* succ_lab=(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2268))->hd)).f2;
if( sc->where_clause == 0){ struct _tuple12 _temp2270= Cyc_Toc_xlate_pat( nv,
rgn, _temp2249, r, path, sc->pattern, succ_lab, fail_lab, decls); goto _LL2271;
_LL2271: nvs=({ struct Cyc_List_List* _temp2272=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2272->hd=( void*)
_temp2270.f1; _temp2272->tl= nvs; _temp2272;}); decls= _temp2270.f2; test_stmts=({
struct Cyc_List_List* _temp2273=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2273->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2268))->hd)).f1,
_temp2270.f3, 0); _temp2273->tl= test_stmts; _temp2273;});} else{ struct Cyc_Absyn_Exp*
_temp2274=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); goto _LL2275;
_LL2275: { struct _tagged_arr* _temp2276= Cyc_Toc_fresh_label(); goto _LL2277;
_LL2277: { struct _tuple12 _temp2278= Cyc_Toc_xlate_pat( nv, rgn, _temp2249, r,
path, sc->pattern, _temp2276, fail_lab, decls); goto _LL2279; _LL2279: Cyc_Toc_exp_to_c(
_temp2278.f1, _temp2274); nvs=({ struct Cyc_List_List* _temp2280=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2280->hd=( void*)
_temp2278.f1; _temp2280->tl= nvs; _temp2280;}); decls= _temp2278.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2274, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2276, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2281=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2281->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2268))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2278.f3, s3, 0), 0); _temp2281->tl= test_stmts;
_temp2281;});}}}}}}{ struct Cyc_Absyn_Stmt* _temp2282= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); goto _LL2283; _LL2283: nvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt(
0); if( lscs != 0){ for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=
0;( lscs=(( struct Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2284=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; goto _LL2285; _LL2285: { struct
Cyc_Absyn_Stmt* s=((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
_temp2284, end_l,(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}}{ struct Cyc_Toc_Env* _temp2286=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; goto _LL2287; _LL2287: { struct Cyc_Absyn_Stmt* s=((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( _temp2286, end_l), s); clauses=
Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0), 0);}}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( _temp2282, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=((
struct Cyc_List_List*) _check_null( decls))->tl){ struct _tuple13 _temp2290;
void* _temp2291; struct _tuple0* _temp2293; struct _tuple13* _temp2288=( struct
_tuple13*)(( struct Cyc_List_List*) _check_null( decls))->hd; _temp2290=*
_temp2288; _LL2294: _temp2293= _temp2290.f1; goto _LL2292; _LL2292: _temp2291=
_temp2290.f2; goto _LL2289; _LL2289: res= Cyc_Absyn_declare_stmt( _temp2293,
_temp2291, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt* Cyc_Toc_make_npop_handler(
int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__npop_handler_e,({
struct Cyc_List_List* _temp2295=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp2295[ 0]=({ struct Cyc_List_List _temp2296;
_temp2296.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0); _temp2296.tl=
0; _temp2296;}); _temp2295;}), 0), 0);} void Cyc_Toc_do_npop_before( int n,
struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));}}
static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s){
while( 1) { void* _temp2297=( void*) s->r; struct Cyc_Absyn_Exp* _temp2339;
struct Cyc_Absyn_Stmt* _temp2341; struct Cyc_Absyn_Stmt* _temp2343; struct Cyc_Absyn_Exp*
_temp2345; struct Cyc_Absyn_Stmt* _temp2347; struct Cyc_Absyn_Stmt* _temp2349;
struct Cyc_Absyn_Exp* _temp2351; struct Cyc_Absyn_Stmt* _temp2353; struct
_tuple2 _temp2355; struct Cyc_Absyn_Stmt* _temp2357; struct Cyc_Absyn_Exp*
_temp2359; struct Cyc_Absyn_Stmt* _temp2361; struct Cyc_Absyn_Stmt* _temp2363;
struct Cyc_Absyn_Stmt* _temp2365; struct _tagged_arr* _temp2367; struct Cyc_Absyn_Stmt*
_temp2369; struct _tuple2 _temp2371; struct Cyc_Absyn_Stmt* _temp2373; struct
Cyc_Absyn_Exp* _temp2375; struct _tuple2 _temp2377; struct Cyc_Absyn_Stmt*
_temp2379; struct Cyc_Absyn_Exp* _temp2381; struct Cyc_Absyn_Exp* _temp2383;
struct Cyc_List_List* _temp2385; struct Cyc_Absyn_Exp* _temp2387; struct Cyc_Absyn_Switch_clause**
_temp2389; struct Cyc_List_List* _temp2391; struct Cyc_Absyn_Stmt* _temp2393;
struct Cyc_Absyn_Decl* _temp2395; struct Cyc_Absyn_Stmt* _temp2397; struct
_tagged_arr* _temp2399; struct _tuple2 _temp2401; struct Cyc_Absyn_Stmt*
_temp2403; struct Cyc_Absyn_Exp* _temp2405; struct Cyc_Absyn_Stmt* _temp2407;
struct Cyc_List_List* _temp2409; struct Cyc_Absyn_Stmt* _temp2411; struct Cyc_Absyn_Stmt*
_temp2413; struct Cyc_Absyn_Vardecl* _temp2415; struct Cyc_Absyn_Tvar* _temp2417;
struct Cyc_List_List* _temp2419; struct Cyc_Absyn_Exp* _temp2421; struct Cyc_Absyn_Stmt*
_temp2423; struct Cyc_Absyn_Stmt* _temp2425; _LL2299: if( _temp2297 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2300;} else{ goto _LL2301;} _LL2301: if((
unsigned int) _temp2297 > 1u?*(( int*) _temp2297) == Cyc_Absyn_Exp_s: 0){
_LL2340: _temp2339=(( struct Cyc_Absyn_Exp_s_struct*) _temp2297)->f1; goto
_LL2302;} else{ goto _LL2303;} _LL2303: if(( unsigned int) _temp2297 > 1u?*((
int*) _temp2297) == Cyc_Absyn_Seq_s: 0){ _LL2344: _temp2343=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2297)->f1; goto _LL2342; _LL2342: _temp2341=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2297)->f2; goto _LL2304;} else{ goto _LL2305;} _LL2305: if(( unsigned int)
_temp2297 > 1u?*(( int*) _temp2297) == Cyc_Absyn_Return_s: 0){ _LL2346:
_temp2345=(( struct Cyc_Absyn_Return_s_struct*) _temp2297)->f1; goto _LL2306;}
else{ goto _LL2307;} _LL2307: if(( unsigned int) _temp2297 > 1u?*(( int*)
_temp2297) == Cyc_Absyn_IfThenElse_s: 0){ _LL2352: _temp2351=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2297)->f1; goto _LL2350; _LL2350: _temp2349=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2297)->f2; goto _LL2348; _LL2348: _temp2347=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2297)->f3; goto _LL2308;} else{ goto _LL2309;} _LL2309: if(( unsigned int)
_temp2297 > 1u?*(( int*) _temp2297) == Cyc_Absyn_While_s: 0){ _LL2356: _temp2355=((
struct Cyc_Absyn_While_s_struct*) _temp2297)->f1; _LL2360: _temp2359= _temp2355.f1;
goto _LL2358; _LL2358: _temp2357= _temp2355.f2; goto _LL2354; _LL2354: _temp2353=((
struct Cyc_Absyn_While_s_struct*) _temp2297)->f2; goto _LL2310;} else{ goto
_LL2311;} _LL2311: if(( unsigned int) _temp2297 > 1u?*(( int*) _temp2297) == Cyc_Absyn_Break_s:
0){ _LL2362: _temp2361=(( struct Cyc_Absyn_Break_s_struct*) _temp2297)->f1; goto
_LL2312;} else{ goto _LL2313;} _LL2313: if(( unsigned int) _temp2297 > 1u?*((
int*) _temp2297) == Cyc_Absyn_Continue_s: 0){ _LL2364: _temp2363=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2297)->f1; goto _LL2314;} else{ goto _LL2315;} _LL2315: if(( unsigned int)
_temp2297 > 1u?*(( int*) _temp2297) == Cyc_Absyn_Goto_s: 0){ _LL2368: _temp2367=((
struct Cyc_Absyn_Goto_s_struct*) _temp2297)->f1; goto _LL2366; _LL2366:
_temp2365=(( struct Cyc_Absyn_Goto_s_struct*) _temp2297)->f2; goto _LL2316;}
else{ goto _LL2317;} _LL2317: if(( unsigned int) _temp2297 > 1u?*(( int*)
_temp2297) == Cyc_Absyn_For_s: 0){ _LL2384: _temp2383=(( struct Cyc_Absyn_For_s_struct*)
_temp2297)->f1; goto _LL2378; _LL2378: _temp2377=(( struct Cyc_Absyn_For_s_struct*)
_temp2297)->f2; _LL2382: _temp2381= _temp2377.f1; goto _LL2380; _LL2380:
_temp2379= _temp2377.f2; goto _LL2372; _LL2372: _temp2371=(( struct Cyc_Absyn_For_s_struct*)
_temp2297)->f3; _LL2376: _temp2375= _temp2371.f1; goto _LL2374; _LL2374:
_temp2373= _temp2371.f2; goto _LL2370; _LL2370: _temp2369=(( struct Cyc_Absyn_For_s_struct*)
_temp2297)->f4; goto _LL2318;} else{ goto _LL2319;} _LL2319: if(( unsigned int)
_temp2297 > 1u?*(( int*) _temp2297) == Cyc_Absyn_Switch_s: 0){ _LL2388:
_temp2387=(( struct Cyc_Absyn_Switch_s_struct*) _temp2297)->f1; goto _LL2386;
_LL2386: _temp2385=(( struct Cyc_Absyn_Switch_s_struct*) _temp2297)->f2; goto
_LL2320;} else{ goto _LL2321;} _LL2321: if(( unsigned int) _temp2297 > 1u?*((
int*) _temp2297) == Cyc_Absyn_Fallthru_s: 0){ _LL2392: _temp2391=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2297)->f1; goto _LL2390; _LL2390: _temp2389=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2297)->f2; goto _LL2322;} else{ goto _LL2323;} _LL2323: if(( unsigned int)
_temp2297 > 1u?*(( int*) _temp2297) == Cyc_Absyn_Decl_s: 0){ _LL2396: _temp2395=((
struct Cyc_Absyn_Decl_s_struct*) _temp2297)->f1; goto _LL2394; _LL2394:
_temp2393=(( struct Cyc_Absyn_Decl_s_struct*) _temp2297)->f2; goto _LL2324;}
else{ goto _LL2325;} _LL2325: if(( unsigned int) _temp2297 > 1u?*(( int*)
_temp2297) == Cyc_Absyn_Label_s: 0){ _LL2400: _temp2399=(( struct Cyc_Absyn_Label_s_struct*)
_temp2297)->f1; goto _LL2398; _LL2398: _temp2397=(( struct Cyc_Absyn_Label_s_struct*)
_temp2297)->f2; goto _LL2326;} else{ goto _LL2327;} _LL2327: if(( unsigned int)
_temp2297 > 1u?*(( int*) _temp2297) == Cyc_Absyn_Do_s: 0){ _LL2408: _temp2407=((
struct Cyc_Absyn_Do_s_struct*) _temp2297)->f1; goto _LL2402; _LL2402: _temp2401=((
struct Cyc_Absyn_Do_s_struct*) _temp2297)->f2; _LL2406: _temp2405= _temp2401.f1;
goto _LL2404; _LL2404: _temp2403= _temp2401.f2; goto _LL2328;} else{ goto
_LL2329;} _LL2329: if(( unsigned int) _temp2297 > 1u?*(( int*) _temp2297) == Cyc_Absyn_TryCatch_s:
0){ _LL2412: _temp2411=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2297)->f1;
goto _LL2410; _LL2410: _temp2409=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2297)->f2; goto _LL2330;} else{ goto _LL2331;} _LL2331: if(( unsigned int)
_temp2297 > 1u?*(( int*) _temp2297) == Cyc_Absyn_Region_s: 0){ _LL2418:
_temp2417=(( struct Cyc_Absyn_Region_s_struct*) _temp2297)->f1; goto _LL2416;
_LL2416: _temp2415=(( struct Cyc_Absyn_Region_s_struct*) _temp2297)->f2; goto
_LL2414; _LL2414: _temp2413=(( struct Cyc_Absyn_Region_s_struct*) _temp2297)->f3;
goto _LL2332;} else{ goto _LL2333;} _LL2333: if(( unsigned int) _temp2297 > 1u?*((
int*) _temp2297) == Cyc_Absyn_SwitchC_s: 0){ _LL2422: _temp2421=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2297)->f1; goto _LL2420; _LL2420: _temp2419=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2297)->f2; goto _LL2334;} else{ goto _LL2335;} _LL2335: if(( unsigned int)
_temp2297 > 1u?*(( int*) _temp2297) == Cyc_Absyn_Cut_s: 0){ _LL2424: _temp2423=((
struct Cyc_Absyn_Cut_s_struct*) _temp2297)->f1; goto _LL2336;} else{ goto
_LL2337;} _LL2337: if(( unsigned int) _temp2297 > 1u?*(( int*) _temp2297) == Cyc_Absyn_Splice_s:
0){ _LL2426: _temp2425=(( struct Cyc_Absyn_Splice_s_struct*) _temp2297)->f1;
goto _LL2338;} else{ goto _LL2298;} _LL2300: return; _LL2302: Cyc_Toc_exp_to_c(
nv, _temp2339); return; _LL2304: Cyc_Toc_stmt_to_c( nv, _temp2343); s= _temp2341;
continue; _LL2306: { struct Cyc_Core_Opt* topt= 0; if( _temp2345 != 0){ topt=({
struct Cyc_Core_Opt* _temp2427=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2427->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2345))->topt))->v);
_temp2427;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2345));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2428=
Cyc_Toc_temp_var(); goto _LL2429; _LL2429: { struct Cyc_Absyn_Stmt* _temp2430=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2428, 0),
0); goto _LL2431; _LL2431:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
_temp2428,( void*)(( struct Cyc_Core_Opt*) _check_null( topt))->v, _temp2345,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), _temp2430, 0), 0))->r));}}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2308: Cyc_Toc_exp_to_c(
nv, _temp2351); Cyc_Toc_stmt_to_c( nv, _temp2349); s= _temp2347; continue;
_LL2310: Cyc_Toc_exp_to_c( nv, _temp2359); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2353); return; _LL2312: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)(( struct Cyc_Core_Opt*)
_check_null( nv->break_lab))->v, 0))->r));}{ int dest_depth= _temp2361 == 0? 0:((
struct Cyc_Absyn_Stmt*) _check_null( _temp2361))->try_depth; Cyc_Toc_do_npop_before(
s->try_depth - dest_depth, s); return;} _LL2314: if( nv->continue_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp2365=
_temp2363; goto _LL2316; _LL2316: Cyc_Toc_do_npop_before( s->try_depth -((
struct Cyc_Absyn_Stmt*) _check_null( _temp2365))->try_depth, s); return; _LL2318:
Cyc_Toc_exp_to_c( nv, _temp2383); Cyc_Toc_exp_to_c( nv, _temp2381); Cyc_Toc_exp_to_c(
nv, _temp2375); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2369); return;
_LL2320: Cyc_Toc_xlate_switch( nv, s, _temp2387, _temp2385); return; _LL2322:
if( nv->fallthru_info == 0){( int) _throw((( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("fallthru in unexpected place", sizeof(
unsigned char), 29u)));}{ struct _tuple6 _temp2434; struct Cyc_Dict_Dict*
_temp2435; struct Cyc_List_List* _temp2437; struct _tagged_arr* _temp2439;
struct _tuple6* _temp2432=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp2434=* _temp2432; _LL2440: _temp2439= _temp2434.f1;
goto _LL2438; _LL2438: _temp2437= _temp2434.f2; goto _LL2436; _LL2436: _temp2435=
_temp2434.f3; goto _LL2433; _LL2433: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp2439, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp2389)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2441=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2437); goto _LL2442; _LL2442: { struct Cyc_List_List* _temp2443=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp2391); goto
_LL2444; _LL2444: for( 0; _temp2441 != 0;( _temp2441=(( struct Cyc_List_List*)
_check_null( _temp2441))->tl, _temp2443=(( struct Cyc_List_List*) _check_null(
_temp2443))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp2443))->hd); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
_temp2435,( struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp2441))->hd),(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp2443))->hd, 0),
s2, 0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}}} _LL2324:{ void*
_temp2445=( void*) _temp2395->r; int _temp2455; struct Cyc_Absyn_Exp* _temp2457;
struct Cyc_Core_Opt* _temp2459; struct Cyc_Core_Opt* _temp2461; struct Cyc_Absyn_Pat*
_temp2463; struct Cyc_List_List* _temp2465; struct Cyc_Absyn_Vardecl* _temp2467;
_LL2447: if(*(( int*) _temp2445) == Cyc_Absyn_Let_d){ _LL2464: _temp2463=((
struct Cyc_Absyn_Let_d_struct*) _temp2445)->f1; goto _LL2462; _LL2462: _temp2461=((
struct Cyc_Absyn_Let_d_struct*) _temp2445)->f2; goto _LL2460; _LL2460: _temp2459=((
struct Cyc_Absyn_Let_d_struct*) _temp2445)->f3; goto _LL2458; _LL2458: _temp2457=((
struct Cyc_Absyn_Let_d_struct*) _temp2445)->f4; goto _LL2456; _LL2456: _temp2455=((
struct Cyc_Absyn_Let_d_struct*) _temp2445)->f5; goto _LL2448;} else{ goto
_LL2449;} _LL2449: if(*(( int*) _temp2445) == Cyc_Absyn_Letv_d){ _LL2466:
_temp2465=(( struct Cyc_Absyn_Letv_d_struct*) _temp2445)->f1; goto _LL2450;}
else{ goto _LL2451;} _LL2451: if(*(( int*) _temp2445) == Cyc_Absyn_Var_d){
_LL2468: _temp2467=(( struct Cyc_Absyn_Var_d_struct*) _temp2445)->f1; goto
_LL2452;} else{ goto _LL2453;} _LL2453: goto _LL2454; _LL2448:( void*)( s->r=(
void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp2463, _temp2459, _temp2457,
_temp2455, _temp2393))->r)); goto _LL2446; _LL2450: { struct Cyc_List_List*
_temp2469=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2465); goto _LL2470; _LL2470: if( _temp2469 == 0){(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d", sizeof( unsigned char),
13u));}( void*)( _temp2395->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2471=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2471[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2472; _temp2472.tag= Cyc_Absyn_Var_d;
_temp2472.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2469))->hd; _temp2472;}); _temp2471;}))); _temp2469=(( struct Cyc_List_List*)
_check_null( _temp2469))->tl; for( 0; _temp2469 != 0; _temp2469=(( struct Cyc_List_List*)
_check_null( _temp2469))->tl){ struct Cyc_Absyn_Decl* _temp2475= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2473=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2473[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2474; _temp2474.tag= Cyc_Absyn_Var_d; _temp2474.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2469))->hd; _temp2474;}); _temp2473;}),
0); goto _LL2476; _LL2476:( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp2475, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c(
nv, s); goto _LL2446;} _LL2452: { struct _tuple0* _temp2477= _temp2467->name;
goto _LL2478; _LL2478:( void*)( _temp2467->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp2467->type));{ struct Cyc_Toc_Env* _temp2481= Cyc_Toc_add_varmap( nv,
_temp2477, Cyc_Absyn_varb_exp( _temp2477,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2479=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2479[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2480; _temp2480.tag= Cyc_Absyn_Local_b;
_temp2480.f1= _temp2467; _temp2480;}); _temp2479;}), 0)); goto _LL2482; _LL2482:
Cyc_Toc_stmt_to_c( _temp2481, _temp2393); if( _temp2467->initializer != 0){
struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*) _check_null( _temp2467->initializer);
void* _temp2483=( void*) init->r; struct Cyc_Absyn_Exp* _temp2489; struct Cyc_Absyn_Exp*
_temp2491; struct Cyc_Absyn_Vardecl* _temp2493; _LL2485: if(*(( int*) _temp2483)
== Cyc_Absyn_Comprehension_e){ _LL2494: _temp2493=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2483)->f1; goto _LL2492; _LL2492: _temp2491=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2483)->f2; goto _LL2490; _LL2490: _temp2489=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2483)->f3; goto _LL2486;} else{ goto _LL2487;} _LL2487: goto _LL2488;
_LL2486: _temp2467->initializer= 0;( void*)( _temp2393->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( _temp2477, 0), _temp2493, _temp2491, _temp2489, Cyc_Absyn_new_stmt((
void*) _temp2393->r, 0), 0))->r)); goto _LL2484; _LL2488: Cyc_Toc_exp_to_c( nv,
init); goto _LL2484; _LL2484:;} goto _LL2446;}} _LL2454:(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function",
sizeof( unsigned char), 39u)); goto _LL2446; _LL2446:;} return; _LL2326: s=
_temp2397; continue; _LL2328: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv),
_temp2407); Cyc_Toc_exp_to_c( nv, _temp2405); return; _LL2330: { struct _tuple0*
h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var(); struct
_tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp*
was_thrown_exp= Cyc_Absyn_var_exp( was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ);
void* was_thrown_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_sint_t); e_exp->topt=({ struct
Cyc_Core_Opt* _temp2495=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2495->v=( void*) e_typ; _temp2495;}); nv= Cyc_Toc_add_varmap( nv, e_var,
e_exp); Cyc_Toc_stmt_to_c( nv, _temp2411);{ struct Cyc_Absyn_Stmt* _temp2496=
Cyc_Absyn_seq_stmt( _temp2411, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e,
0, 0), 0), 0); goto _LL2497; _LL2497: { struct _tuple0* _temp2498= Cyc_Toc_temp_var();
goto _LL2499; _LL2499: { struct Cyc_Absyn_Exp* _temp2500= Cyc_Absyn_var_exp(
_temp2498, 0); goto _LL2501; _LL2501: { struct Cyc_Absyn_Vardecl* _temp2502= Cyc_Absyn_new_vardecl(
_temp2498, Cyc_Absyn_exn_typ, 0); goto _LL2503; _LL2503: _temp2500->topt=({
struct Cyc_Core_Opt* _temp2504=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2504->v=( void*) Cyc_Absyn_exn_typ; _temp2504;});{ struct
Cyc_Absyn_Pat* _temp2509=({ struct Cyc_Absyn_Pat* _temp2505=( struct Cyc_Absyn_Pat*)
GC_malloc( sizeof( struct Cyc_Absyn_Pat)); _temp2505->r=( void*)(( void*)({
struct Cyc_Absyn_Var_p_struct* _temp2507=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp2507[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp2508; _temp2508.tag= Cyc_Absyn_Var_p; _temp2508.f1= _temp2502; _temp2508;});
_temp2507;})); _temp2505->topt=({ struct Cyc_Core_Opt* _temp2506=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2506->v=( void*) Cyc_Absyn_exn_typ;
_temp2506;}); _temp2505->loc= 0; _temp2505;}); goto _LL2510; _LL2510: { struct
Cyc_Absyn_Exp* _temp2511= Cyc_Absyn_throw_exp( _temp2500, 0); goto _LL2512;
_LL2512: _temp2511->topt=({ struct Cyc_Core_Opt* _temp2513=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2513->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp2513;});{ struct Cyc_Absyn_Stmt* _temp2514= Cyc_Absyn_exp_stmt( _temp2511,
0); goto _LL2515; _LL2515: { struct Cyc_Absyn_Switch_clause* _temp2519=({ struct
Cyc_Absyn_Switch_clause* _temp2516=( struct Cyc_Absyn_Switch_clause*) GC_malloc(
sizeof( struct Cyc_Absyn_Switch_clause)); _temp2516->pattern= _temp2509;
_temp2516->pat_vars=({ struct Cyc_Core_Opt* _temp2517=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2517->v=( void*)({ struct Cyc_List_List*
_temp2518=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2518->hd=( void*) _temp2502; _temp2518->tl= 0; _temp2518;}); _temp2517;});
_temp2516->where_clause= 0; _temp2516->body= _temp2514; _temp2516->loc= 0;
_temp2516;}); goto _LL2520; _LL2520: { struct Cyc_Absyn_Stmt* _temp2522= Cyc_Absyn_switch_stmt(
e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp2409,({ struct Cyc_List_List* _temp2521=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2521->hd=( void*) _temp2519;
_temp2521->tl= 0; _temp2521;})), 0); goto _LL2523; _LL2523: Cyc_Toc_stmt_to_c(
nv, _temp2522);{ struct Cyc_Absyn_Exp* _temp2525= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp2524=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2524->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2524->tl= 0; _temp2524;}), 0); goto _LL2526;
_LL2526: { struct Cyc_Absyn_Stmt* _temp2528= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({ struct Cyc_List_List* _temp2527=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2527->hd=( void*) Cyc_Absyn_address_exp(
h_exp, 0); _temp2527->tl= 0; _temp2527;}), 0), 0); goto _LL2529; _LL2529: {
struct Cyc_Absyn_Exp* _temp2530= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0,
0); goto _LL2531; _LL2531: { struct Cyc_Absyn_Exp* _temp2532= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0); goto _LL2533; _LL2533:( void*)( s->r=( void*)((
void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2528,
Cyc_Absyn_declare_stmt( was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*)
_temp2530, Cyc_Absyn_seq_stmt( Cyc_Absyn_ifthenelse_stmt( _temp2525, Cyc_Absyn_assign_stmt(
was_thrown_exp, _temp2532, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2496, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp2522, 0), 0), 0), 0), 0), 0))->r)); return;}}}}}}}}}}}}}} _LL2332: {
void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp2415->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp2413);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_List_List*(* _temp2534)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp2537= x_exp; struct Cyc_Absyn_Exp* _temp2535[ 1u]={ _temp2537}; struct
_tagged_arr _temp2536={( void*) _temp2535,( void*) _temp2535,( void*)( _temp2535
+ 1u)}; _temp2534( _temp2536);}), 0), 0), Cyc_Absyn_seq_stmt( _temp2413, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({ struct Cyc_List_List*(* _temp2538)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp2541= x_exp; struct Cyc_Absyn_Exp* _temp2539[ 1u]={
_temp2541}; struct _tagged_arr _temp2540={( void*) _temp2539,( void*) _temp2539,(
void*)( _temp2539 + 1u)}; _temp2538( _temp2540);}), 0), 0), 0), 0), 0), 0))->r));
return;} _LL2334:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("switchC_s",
sizeof( unsigned char), 10u)); return; _LL2336:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("cut", sizeof( unsigned char), 4u)); return; _LL2338:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof(
unsigned char), 7u)); return; _LL2298:;}} struct _tuple16{ struct _tagged_arr*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0; f->rgn_po=
0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* _temp2542= f->args; goto _LL2543; _LL2543: for( 0;
_temp2542 != 0; _temp2542=(( struct Cyc_List_List*) _check_null( _temp2542))->tl){(*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2542))->hd)).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2542))->hd)).f3);{ struct _tuple0* _temp2545=({ struct _tuple0* _temp2544=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2544->f1=( void*) Cyc_Absyn_Loc_n;
_temp2544->f2=(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2542))->hd)).f1; _temp2544;}); goto _LL2546; _LL2546: nv= Cyc_Toc_add_varmap(
nv, _temp2545, Cyc_Absyn_var_exp( _temp2545, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2549; void* _temp2551; void* _temp2553; struct Cyc_Absyn_Tqual _temp2555;
struct Cyc_Core_Opt* _temp2557; struct Cyc_Absyn_VarargInfo _temp2547=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2558: _temp2557=
_temp2547.name; goto _LL2556; _LL2556: _temp2555= _temp2547.tq; goto _LL2554;
_LL2554: _temp2553=( void*) _temp2547.type; goto _LL2552; _LL2552: _temp2551=(
void*) _temp2547.rgn; goto _LL2550; _LL2550: _temp2549= _temp2547.inject; goto
_LL2548; _LL2548: { void* _temp2559= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2553, _temp2551, _temp2555)); goto _LL2560; _LL2560: { struct _tuple0*
_temp2562=({ struct _tuple0* _temp2561=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2561->f1=( void*) Cyc_Absyn_Loc_n; _temp2561->f2=( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp2557))->v; _temp2561;});
goto _LL2563; _LL2563: f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2564=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2564->hd=( void*)({ struct _tuple16* _temp2565=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp2565->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp2557))->v; _temp2565->f2= _temp2555;
_temp2565->f3= _temp2559; _temp2565;}); _temp2564->tl= 0; _temp2564;})); nv= Cyc_Toc_add_varmap(
nv, _temp2562, Cyc_Absyn_var_exp( _temp2562, 0)); f->cyc_varargs= 0;}}}{ struct
Cyc_List_List* _temp2566=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; goto _LL2567; _LL2567: for( 0; _temp2566 !=
0; _temp2566=(( struct Cyc_List_List*) _check_null( _temp2566))->tl){( void*)(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp2566))->hd)->type=(
void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp2566))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2568= s;
_LL2570: if( _temp2568 ==( void*) Cyc_Absyn_Abstract){ goto _LL2571;} else{ goto
_LL2572;} _LL2572: if( _temp2568 ==( void*) Cyc_Absyn_ExternC){ goto _LL2573;}
else{ goto _LL2574;} _LL2574: goto _LL2575; _LL2571: return( void*) Cyc_Absyn_Public;
_LL2573: return( void*) Cyc_Absyn_Extern; _LL2575: return s; _LL2569:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2576=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; goto _LL2577; _LL2577: if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2578=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2578->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2578;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2579=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2576).f2); goto _LL2580; _LL2580: if( _temp2579 == 0){ seen_defn_before= 0;(
struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2576).f2,
s));} else{ if((( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*)
_check_null( _temp2579))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2576).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(( void*) Cyc_Absyn_Public));
s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2581=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2581->v=( void*) _temp2576;
_temp2581;}); if( s->fields != 0){ if( seen_defn_before){ s->fields= 0;} else{
struct Cyc_List_List* _temp2582=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; goto _LL2583; _LL2583: for( 0; _temp2582 != 0;
_temp2582=(( struct Cyc_List_List*) _check_null( _temp2582))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2582))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2582))->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple0* _temp2584=( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( u->name))->v; goto _LL2585; _LL2585: if( Cyc_Toc_unions_so_far == 0){
Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2586=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2586->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2586;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2587=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2584).f2); goto _LL2588; _LL2588: if( _temp2587 == 0){ seen_defn_before= 0;(
struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp2584).f2,
u));} else{ if((( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*)
_check_null( _temp2587))->v)->fields == 0){( struct Cyc_Dict_Dict*)((( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2584).f2, u)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)(( void*) Cyc_Absyn_Public));
u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2589=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2589->v=( void*) _temp2584;
_temp2589;}); if( u->fields != 0){ if( seen_defn_before){ u->fields= 0;} else{
struct Cyc_List_List* _temp2590=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; goto _LL2591; _LL2591: for( 0; _temp2590 != 0;
_temp2590=(( struct Cyc_List_List*) _check_null( _temp2590))->tl){( void*)(((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2590))->hd)->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp2590))->hd)->type));}}}}} static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far= 0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl*
tud){ if( Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2592=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2592->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2592;});}{ struct
_tuple0* _temp2593= tud->name; goto _LL2594; _LL2594: if( tud->fields == 0){
return;} if((( int(*)( struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)((
struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2593)){ return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_tunions_so_far))->v=( void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tuple0* elt)) Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_tunions_so_far))->v, _temp2593));{ int tag_count= 0; int
box_count= 0; struct Cyc_List_List* _temp2595=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( tud->fields))->v; goto _LL2596; _LL2596: for( 0;
_temp2595 != 0; _temp2595=(( struct Cyc_List_List*) _check_null( _temp2595))->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp2595))->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count ++, 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t, eopt); vd->tq=({ struct
Cyc_Absyn_Tqual _temp2597; _temp2597.q_const= 1; _temp2597.q_volatile= 0;
_temp2597.q_restrict= 0; _temp2597;}); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2598=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2598->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2599=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2599[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2600; _temp2600.tag= Cyc_Absyn_Var_d;
_temp2600.f1= vd; _temp2600;}); _temp2599;}), 0); _temp2598->tl= Cyc_Toc_result_decls;
_temp2598;});} else{ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2601; _temp2601.q_const=
1; _temp2601.q_volatile= 0; _temp2601.q_restrict= 0; _temp2601;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2602=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2602->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2603=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2603[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2604; _temp2604.tag= Cyc_Absyn_Var_d; _temp2604.f1= vd; _temp2604;});
_temp2603;}), 0); _temp2602->tl= Cyc_Toc_result_decls; _temp2602;});{ struct Cyc_List_List*
_temp2605= 0; goto _LL2606; _LL2606: { int i= 1;{ struct Cyc_List_List*
_temp2607= f->typs; goto _LL2608; _LL2608: for( 0; _temp2607 != 0;( _temp2607=((
struct Cyc_List_List*) _check_null( _temp2607))->tl, i ++)){ struct _tagged_arr*
_temp2609= Cyc_Toc_fieldname( i); goto _LL2610; _LL2610: { struct Cyc_Absyn_Structfield*
_temp2612=({ struct Cyc_Absyn_Structfield* _temp2611=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2611->name= _temp2609;
_temp2611->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2607))->hd)).f1; _temp2611->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2607))->hd)).f2);
_temp2611->width= 0; _temp2611->attributes= 0; _temp2611;}); goto _LL2613;
_LL2613: _temp2605=({ struct Cyc_List_List* _temp2614=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2614->hd=( void*) _temp2612;
_temp2614->tl= _temp2605; _temp2614;});}}} _temp2605=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2605); _temp2605=({ struct Cyc_List_List*
_temp2615=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2615->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2616=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2616->name= Cyc_Toc_tag_sp;
_temp2616->tq= Cyc_Toc_mt_tq; _temp2616->type=( void*) Cyc_Absyn_sint_t;
_temp2616->width= 0; _temp2616->attributes= 0; _temp2616;}); _temp2615->tl=
_temp2605; _temp2615;});{ struct Cyc_Absyn_Structdecl* _temp2620=({ struct Cyc_Absyn_Structdecl*
_temp2617=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2617->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2617->name=({ struct Cyc_Core_Opt*
_temp2619=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2619->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp2619;}); _temp2617->tvs= 0; _temp2617->fields=({
struct Cyc_Core_Opt* _temp2618=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2618->v=( void*) _temp2605; _temp2618;}); _temp2617->attributes=
0; _temp2617;}); goto _LL2621; _LL2621: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2622=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2622->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2623=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2623[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2624; _temp2624.tag= Cyc_Absyn_Struct_d;
_temp2624.f1= _temp2620; _temp2624;}); _temp2623;}), 0); _temp2622->tl= Cyc_Toc_result_decls;
_temp2622;});}}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2625=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2625->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2625;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2626= xd->name; goto _LL2627; _LL2627: {
struct Cyc_List_List* _temp2628=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( xd->fields))->v; goto _LL2629; _LL2629: for( 0; _temp2628 != 0;
_temp2628=(( struct Cyc_List_List*) _check_null( _temp2628))->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2628))->hd; struct _tagged_arr* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
_temp2630= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof( unsigned char)) + 4,
0); goto _LL2631; _LL2631: { struct Cyc_Absyn_ArrayType_struct* _temp2634=({
struct Cyc_Absyn_ArrayType_struct* _temp2632=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2632[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2633; _temp2633.tag= Cyc_Absyn_ArrayType;
_temp2633.f1=( void*) Cyc_Absyn_uchar_t; _temp2633.f2= Cyc_Toc_mt_tq; _temp2633.f3=(
struct Cyc_Absyn_Exp*) _temp2630; _temp2633;}); _temp2632;}); goto _LL2635;
_LL2635: { struct Cyc_Core_Opt* _temp2636=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct
Cyc_Core_Opt _temp2644; int _temp2645; _LL2638: if( _temp2636 == 0){ goto
_LL2639;} else{ goto _LL2640;} _LL2640: if( _temp2636 == 0){ goto _LL2642;}
else{ _temp2644=* _temp2636; _LL2646: _temp2645=( int) _temp2644.v; if(
_temp2645 == 0){ goto _LL2641;} else{ goto _LL2642;}} _LL2642: goto _LL2643;
_LL2639: { struct Cyc_Absyn_Exp* initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_arr)({
int _temp2647= 0; int _temp2648= 0; int _temp2649= 0; int _temp2650= 0; struct
_tagged_arr _temp2651=* fn; xprintf("%c%c%c%c%.*s", _temp2647, _temp2648,
_temp2649, _temp2650, _get_arr_size( _temp2651, 1u), _temp2651.curr);}), 0);}{
struct Cyc_Absyn_Vardecl* _temp2652= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2634, initopt); goto _LL2653; _LL2653:( void*)( _temp2652->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2654=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2654->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2655=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2655[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2656; _temp2656.tag= Cyc_Absyn_Var_d;
_temp2656.f1= _temp2652; _temp2656;}); _temp2655;}), 0); _temp2654->tl= Cyc_Toc_result_decls;
_temp2654;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2657= f->typs; goto _LL2658; _LL2658: for( 0;
_temp2657 != 0;( _temp2657=(( struct Cyc_List_List*) _check_null( _temp2657))->tl,
i ++)){ struct _tagged_arr* _temp2660=({ struct _tagged_arr* _temp2659=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2659[ 0]=( struct
_tagged_arr) xprintf("f%d", i); _temp2659;}); goto _LL2661; _LL2661: { struct
Cyc_Absyn_Structfield* _temp2663=({ struct Cyc_Absyn_Structfield* _temp2662=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2662->name= _temp2660; _temp2662->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2657))->hd)).f1; _temp2662->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2657))->hd)).f2);
_temp2662->width= 0; _temp2662->attributes= 0; _temp2662;}); goto _LL2664;
_LL2664: fields=({ struct Cyc_List_List* _temp2665=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2665->hd=( void*) _temp2663;
_temp2665->tl= fields; _temp2665;});}}} fields=({ struct Cyc_List_List*
_temp2666=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2666->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2667=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2667->name= Cyc_Toc_tag_sp;
_temp2667->tq= Cyc_Toc_mt_tq; _temp2667->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2667->width= 0; _temp2667->attributes= 0; _temp2667;});
_temp2666->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2666;});{ struct Cyc_Absyn_Structdecl* _temp2671=({ struct Cyc_Absyn_Structdecl*
_temp2668=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2668->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2668->name=({ struct Cyc_Core_Opt*
_temp2670=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2670->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp2670;}); _temp2668->tvs= 0; _temp2668->fields=({
struct Cyc_Core_Opt* _temp2669=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2669->v=( void*) fields; _temp2669;}); _temp2668->attributes=
0; _temp2668;}); goto _LL2672; _LL2672: Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2673=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2673->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp2674=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2674[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2675; _temp2675.tag= Cyc_Absyn_Struct_d;
_temp2675.f1= _temp2671; _temp2675;}); _temp2674;}), 0); _temp2673->tl= Cyc_Toc_result_decls;
_temp2673;});}} goto _LL2637;}} _LL2641: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2681= Cyc_Absyn_string_exp(( struct _tagged_arr)({
int _temp2676= 0; int _temp2677= 0; int _temp2678= 0; int _temp2679= 0; struct
_tagged_arr _temp2680=* fn; xprintf("%c%c%c%c%.*s", _temp2676, _temp2677,
_temp2678, _temp2679, _get_arr_size( _temp2680, 1u), _temp2680.curr);}), 0);
goto _LL2682; _LL2682: { struct Cyc_Absyn_Vardecl* _temp2683= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2634,( struct Cyc_Absyn_Exp*) _temp2681); goto _LL2684;
_LL2684:( void*)( _temp2683->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2685=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2685->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2686=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2686[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2687; _temp2687.tag= Cyc_Absyn_Var_d; _temp2687.f1= _temp2683; _temp2687;});
_temp2686;}), 0); _temp2685->tl= Cyc_Toc_result_decls; _temp2685;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));}} goto _LL2637; _LL2643: goto
_LL2637; _LL2637:;}}}}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2688= ed->name; goto
_LL2689; _LL2689:( void*)( ed->sc=( void*)(( void*) Cyc_Absyn_Public)); if( ed->fields
!= 0){ struct Cyc_List_List* _temp2690=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; goto _LL2691; _LL2691: for( 0; _temp2690 != 0;
_temp2690=(( struct Cyc_List_List*) _check_null( _temp2690))->tl){ if((( struct
Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( _temp2690))->hd)->tag
!= 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp2690))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2690))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e,
int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple0* x= Cyc_Toc_temp_var();
struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_arr*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct _RegionHandle
_temp2692= _new_region(); struct _RegionHandle* prgn=& _temp2692; _push_region(
prgn);{ struct Cyc_Absyn_Stmt* _temp2695; struct Cyc_List_List* _temp2697;
struct Cyc_Toc_Env* _temp2699; struct _tuple12 _temp2693= Cyc_Toc_xlate_pat( nv,
prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2700: _temp2699= _temp2693.f1; goto _LL2698; _LL2698: _temp2697=
_temp2693.f2; goto _LL2696; _LL2696: _temp2695= _temp2693.f3; goto _LL2694;
_LL2694: Cyc_Toc_stmt_to_c( _temp2699, s);{ struct Cyc_Absyn_Stmt* _temp2701=
Cyc_Absyn_label_stmt( succ_lab, s, 0); goto _LL2702; _LL2702: if( exhaust){ s=
Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2695, _temp2701, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2703= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); goto _LL2704; _LL2704: { struct Cyc_Absyn_Stmt*
_temp2705= Cyc_Absyn_label_stmt( fail_lab, Cyc_Absyn_exp_stmt( _temp2703, 0), 0);
goto _LL2706; _LL2706: s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),(
struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2695, Cyc_Absyn_seq_stmt(
_temp2705, _temp2701, 0), 0), 0);}} for( 0; _temp2697 != 0; _temp2697=(( struct
Cyc_List_List*) _check_null( _temp2697))->tl){ struct _tuple13 _temp2709; void*
_temp2710; struct _tuple0* _temp2712; struct _tuple13* _temp2707=( struct
_tuple13*)(( struct Cyc_List_List*) _check_null( _temp2697))->hd; _temp2709=*
_temp2707; _LL2713: _temp2712= _temp2709.f1; goto _LL2711; _LL2711: _temp2710=
_temp2709.f2; goto _LL2708; _LL2708: s= Cyc_Absyn_declare_stmt( _temp2712,
_temp2710, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){
for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( ds))->hd; void*
_temp2714=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp2740; struct Cyc_Absyn_Fndecl*
_temp2742; int _temp2744; struct Cyc_Absyn_Exp* _temp2746; struct Cyc_Core_Opt*
_temp2748; struct Cyc_Core_Opt* _temp2750; struct Cyc_Absyn_Pat* _temp2752;
struct Cyc_List_List* _temp2754; struct Cyc_Absyn_Structdecl* _temp2756; struct
Cyc_Absyn_Uniondecl* _temp2758; struct Cyc_Absyn_Tuniondecl* _temp2760; struct
Cyc_Absyn_Enumdecl* _temp2762; struct Cyc_Absyn_Typedefdecl* _temp2764; struct
Cyc_List_List* _temp2766; struct _tagged_arr* _temp2768; struct Cyc_List_List*
_temp2770; struct _tuple0* _temp2772; struct Cyc_List_List* _temp2774; _LL2716:
if(*(( int*) _temp2714) == Cyc_Absyn_Var_d){ _LL2741: _temp2740=(( struct Cyc_Absyn_Var_d_struct*)
_temp2714)->f1; goto _LL2717;} else{ goto _LL2718;} _LL2718: if(*(( int*)
_temp2714) == Cyc_Absyn_Fn_d){ _LL2743: _temp2742=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2714)->f1; goto _LL2719;} else{ goto _LL2720;} _LL2720: if(*(( int*)
_temp2714) == Cyc_Absyn_Let_d){ _LL2753: _temp2752=(( struct Cyc_Absyn_Let_d_struct*)
_temp2714)->f1; goto _LL2751; _LL2751: _temp2750=(( struct Cyc_Absyn_Let_d_struct*)
_temp2714)->f2; goto _LL2749; _LL2749: _temp2748=(( struct Cyc_Absyn_Let_d_struct*)
_temp2714)->f3; goto _LL2747; _LL2747: _temp2746=(( struct Cyc_Absyn_Let_d_struct*)
_temp2714)->f4; goto _LL2745; _LL2745: _temp2744=(( struct Cyc_Absyn_Let_d_struct*)
_temp2714)->f5; goto _LL2721;} else{ goto _LL2722;} _LL2722: if(*(( int*)
_temp2714) == Cyc_Absyn_Letv_d){ _LL2755: _temp2754=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2714)->f1; goto _LL2723;} else{ goto _LL2724;} _LL2724: if(*(( int*)
_temp2714) == Cyc_Absyn_Struct_d){ _LL2757: _temp2756=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2714)->f1; goto _LL2725;} else{ goto _LL2726;} _LL2726: if(*(( int*)
_temp2714) == Cyc_Absyn_Union_d){ _LL2759: _temp2758=(( struct Cyc_Absyn_Union_d_struct*)
_temp2714)->f1; goto _LL2727;} else{ goto _LL2728;} _LL2728: if(*(( int*)
_temp2714) == Cyc_Absyn_Tunion_d){ _LL2761: _temp2760=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2714)->f1; goto _LL2729;} else{ goto _LL2730;} _LL2730: if(*(( int*)
_temp2714) == Cyc_Absyn_Enum_d){ _LL2763: _temp2762=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2714)->f1; goto _LL2731;} else{ goto _LL2732;} _LL2732: if(*(( int*)
_temp2714) == Cyc_Absyn_Typedef_d){ _LL2765: _temp2764=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2714)->f1; goto _LL2733;} else{ goto _LL2734;} _LL2734: if(*(( int*)
_temp2714) == Cyc_Absyn_Namespace_d){ _LL2769: _temp2768=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2714)->f1; goto _LL2767; _LL2767: _temp2766=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2714)->f2; goto _LL2735;} else{ goto _LL2736;} _LL2736: if(*(( int*)
_temp2714) == Cyc_Absyn_Using_d){ _LL2773: _temp2772=(( struct Cyc_Absyn_Using_d_struct*)
_temp2714)->f1; goto _LL2771; _LL2771: _temp2770=(( struct Cyc_Absyn_Using_d_struct*)
_temp2714)->f2; goto _LL2737;} else{ goto _LL2738;} _LL2738: if(*(( int*)
_temp2714) == Cyc_Absyn_ExternC_d){ _LL2775: _temp2774=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2714)->f1; goto _LL2739;} else{ goto _LL2715;} _LL2717: { struct _tuple0*
_temp2776= _temp2740->name; goto _LL2777; _LL2777: if(( void*) _temp2740->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2776=({ struct _tuple0* _temp2778=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2778->f1= Cyc_Toc_rel_ns;
_temp2778->f2=(* _temp2776).f2; _temp2778;});} if( _temp2740->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null( _temp2740->initializer));}
if( ! nv->toplevel){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u));} nv= Cyc_Toc_add_varmap( nv, _temp2740->name, Cyc_Absyn_varb_exp(
_temp2776,( void*)({ struct Cyc_Absyn_Global_b_struct* _temp2779=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp2779[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp2780; _temp2780.tag= Cyc_Absyn_Global_b;
_temp2780.f1= _temp2740; _temp2780;}); _temp2779;}), 0)); _temp2740->name=
_temp2776;( void*)( _temp2740->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2740->sc));(
void*)( _temp2740->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2740->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2781=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2781->hd=( void*) d; _temp2781->tl=
Cyc_Toc_result_decls; _temp2781;}); goto _LL2715;} _LL2719: { struct _tuple0*
_temp2782= _temp2742->name; goto _LL2783; _LL2783: if(( void*) _temp2742->sc ==(
void*) Cyc_Absyn_ExternC){ _temp2782=({ struct _tuple0* _temp2784=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2784->f1= Cyc_Toc_rel_ns;
_temp2784->f2=(* _temp2782).f2; _temp2784;});} nv= Cyc_Toc_add_varmap( nv,
_temp2742->name, Cyc_Absyn_var_exp( _temp2782, 0)); _temp2742->name= _temp2782;
Cyc_Toc_fndecl_to_c( nv, _temp2742); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2785=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2785->hd=( void*) d; _temp2785->tl= Cyc_Toc_result_decls; _temp2785;});
goto _LL2715;} _LL2721:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("letdecl", sizeof( unsigned char), 8u)); goto _LL2715; _LL2723:(( int(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("letdecl", sizeof(
unsigned char), 8u)); goto _LL2715; _LL2725: Cyc_Toc_structdecl_to_c( _temp2756);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2786=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2786->hd=( void*) d; _temp2786->tl=
Cyc_Toc_result_decls; _temp2786;}); goto _LL2715; _LL2727: Cyc_Toc_uniondecl_to_c(
_temp2758); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2787=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2787->hd=( void*) d; _temp2787->tl=
Cyc_Toc_result_decls; _temp2787;}); goto _LL2715; _LL2729: if( _temp2760->is_xtunion){
Cyc_Toc_xtuniondecl_to_c( _temp2760);} else{ Cyc_Toc_tuniondecl_to_c( _temp2760);}
goto _LL2715; _LL2731: Cyc_Toc_enumdecl_to_c( nv, _temp2762); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2788=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2788->hd=( void*) d; _temp2788->tl= Cyc_Toc_result_decls;
_temp2788;}); goto _LL2715; _LL2733: _temp2764->name= _temp2764->name; _temp2764->tvs=
0;( void*)( _temp2764->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2764->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2789=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2789->hd=( void*) d; _temp2789->tl=
Cyc_Toc_result_decls; _temp2789;}); goto _LL2715; _LL2735: _temp2770= _temp2766;
goto _LL2737; _LL2737: _temp2774= _temp2770; goto _LL2739; _LL2739: nv= Cyc_Toc_decls_to_c(
nv, _temp2774, top); goto _LL2715; _LL2715:;} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}