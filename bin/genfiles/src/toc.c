 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
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
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value; extern void* Cyc_Absyn_rel_ns_null;
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
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
16u]="\000\000\000\000Match_error"; static unsigned char _temp2[ 5u]="curr";
static struct _tagged_arr Cyc_Toc_curr_string={ _temp2, _temp2, _temp2 + 5u};
static struct _tagged_arr* Cyc_Toc_curr_sp=& Cyc_Toc_curr_string; static
unsigned char _temp3[ 4u]="tag"; static struct _tagged_arr Cyc_Toc_tag_string={
_temp3, _temp3, _temp3 + 4u}; static struct _tagged_arr* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static unsigned char _temp4[ 14u]="_handler_cons"; static struct _tagged_arr Cyc_Toc__handler_cons_string={
_temp4, _temp4, _temp4 + 14u}; static struct _tagged_arr* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static unsigned char _temp5[ 8u]="handler"; static
struct _tagged_arr Cyc_Toc_handler_string={ _temp5, _temp5, _temp5 + 8u}; static
struct _tagged_arr* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static
unsigned char _temp6[ 14u]="_RegionHandle"; static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_temp6, _temp6, _temp6 + 14u}; static struct _tagged_arr* Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string; static unsigned char _temp7[ 7u]="_throw"; static
struct _tagged_arr Cyc_Toc__throw_str={ _temp7, _temp7, _temp7 + 7u}; static
struct _tuple0 Cyc_Toc__throw_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__throw_str};
static struct _tuple0* Cyc_Toc__throw_qv=& Cyc_Toc__throw_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__throw_re={ 1u,& Cyc_Toc__throw_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__throw_ev={ 0,( void*)(( void*)& Cyc_Toc__throw_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__throw_e=& Cyc_Toc__throw_ev; static
unsigned char _temp9[ 7u]="setjmp"; static struct _tagged_arr Cyc_Toc_setjmp_str={
_temp9, _temp9, _temp9 + 7u}; static struct _tuple0 Cyc_Toc_setjmp_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str}; static struct _tuple0* Cyc_Toc_setjmp_qv=&
Cyc_Toc_setjmp_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={ 1u,&
Cyc_Toc_setjmp_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,( void*)(( void*)& Cyc_Toc_setjmp_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc_setjmp_e=&
Cyc_Toc_setjmp_ev; static unsigned char _temp11[ 14u]="_push_handler"; static
struct _tagged_arr Cyc_Toc__push_handler_str={ _temp11, _temp11, _temp11 + 14u};
static struct _tuple0 Cyc_Toc__push_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__push_handler_str}; static struct _tuple0* Cyc_Toc__push_handler_qv=&
Cyc_Toc__push_handler_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1u,& Cyc_Toc__push_handler_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__push_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__push_handler_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;
static unsigned char _temp13[ 13u]="_pop_handler"; static struct _tagged_arr Cyc_Toc__pop_handler_str={
_temp13, _temp13, _temp13 + 13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_handler_str}; static struct
_tuple0* Cyc_Toc__pop_handler_qv=& Cyc_Toc__pop_handler_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_handler_re={ 1u,& Cyc_Toc__pop_handler_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_handler_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;
static unsigned char _temp15[ 12u]="_exn_thrown"; static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_temp15, _temp15, _temp15 + 12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__exn_thrown_str}; static struct
_tuple0* Cyc_Toc__exn_thrown_qv=& Cyc_Toc__exn_thrown_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={ 1u,& Cyc_Toc__exn_thrown_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={ 0,( void*)(( void*)& Cyc_Toc__exn_thrown_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;
static unsigned char _temp17[ 14u]="_npop_handler"; static struct _tagged_arr
Cyc_Toc__npop_handler_str={ _temp17, _temp17, _temp17 + 14u}; static struct
_tuple0 Cyc_Toc__npop_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};
static struct _tuple0* Cyc_Toc__npop_handler_qv=& Cyc_Toc__npop_handler_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={ 1u,& Cyc_Toc__npop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__npop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev; static unsigned char _temp19[ 12u]="_check_null";
static struct _tagged_arr Cyc_Toc__check_null_str={ _temp19, _temp19, _temp19 +
12u}; static struct _tuple0 Cyc_Toc__check_null_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_null_str}; static struct _tuple0* Cyc_Toc__check_null_qv=& Cyc_Toc__check_null_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={ 1u,& Cyc_Toc__check_null_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={ 0,(
void*)(( void*)& Cyc_Toc__check_null_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev; static unsigned char _temp21[ 28u]="_check_known_subscript_null";
static struct _tagged_arr Cyc_Toc__check_known_subscript_null_str={ _temp21,
_temp21, _temp21 + 28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str};
static struct _tuple0* Cyc_Toc__check_known_subscript_null_qv=& Cyc_Toc__check_known_subscript_null_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={ 1u,&
Cyc_Toc__check_known_subscript_null_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_null_ev={ 0,( void*)(( void*)& Cyc_Toc__check_known_subscript_null_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static unsigned char _temp23[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={ _temp23, _temp23,
_temp23 + 31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct _tuple0* Cyc_Toc__check_known_subscript_notnull_qv=& Cyc_Toc__check_known_subscript_notnull_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={
1u,& Cyc_Toc__check_known_subscript_notnull_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={ 0,( void*)((
void*)& Cyc_Toc__check_known_subscript_notnull_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static unsigned char _temp25[ 25u]="_check_unknown_subscript"; static struct
_tagged_arr Cyc_Toc__check_unknown_subscript_str={ _temp25, _temp25, _temp25 +
25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str}; static struct _tuple0* Cyc_Toc__check_unknown_subscript_qv=&
Cyc_Toc__check_unknown_subscript_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1u,& Cyc_Toc__check_unknown_subscript_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__check_unknown_subscript_ev={ 0,( void*)(( void*)& Cyc_Toc__check_unknown_subscript_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_unknown_subscript_e=& Cyc_Toc__check_unknown_subscript_ev;
static unsigned char _temp27[ 12u]="_tagged_arr"; static struct _tagged_arr Cyc_Toc__tagged_arr_str={
_temp27, _temp27, _temp27 + 12u}; static struct _tuple0 Cyc_Toc__tagged_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str}; static struct
_tuple0* Cyc_Toc__tagged_arr_qv=& Cyc_Toc__tagged_arr_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_re={ 1u,& Cyc_Toc__tagged_arr_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_e=& Cyc_Toc__tagged_arr_ev;
static unsigned char _temp29[ 9u]="_tag_arr"; static struct _tagged_arr Cyc_Toc__tag_arr_str={
_temp29, _temp29, _temp29 + 9u}; static struct _tuple0 Cyc_Toc__tag_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tag_arr_str}; static struct
_tuple0* Cyc_Toc__tag_arr_qv=& Cyc_Toc__tag_arr_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tag_arr_re={ 1u,& Cyc_Toc__tag_arr_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__tag_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tag_arr_e=& Cyc_Toc__tag_arr_ev;
static unsigned char _temp31[ 14u]="_init_tag_arr"; static struct _tagged_arr
Cyc_Toc__init_tag_arr_str={ _temp31, _temp31, _temp31 + 14u}; static struct
_tuple0 Cyc_Toc__init_tag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_tag_arr_str};
static struct _tuple0* Cyc_Toc__init_tag_arr_qv=& Cyc_Toc__init_tag_arr_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_tag_arr_re={ 1u,& Cyc_Toc__init_tag_arr_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__init_tag_arr_ev={ 0,(
void*)(( void*)& Cyc_Toc__init_tag_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__init_tag_arr_e=&
Cyc_Toc__init_tag_arr_ev; static unsigned char _temp33[ 11u]="_untag_arr";
static struct _tagged_arr Cyc_Toc__untag_arr_str={ _temp33, _temp33, _temp33 +
11u}; static struct _tuple0 Cyc_Toc__untag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__untag_arr_str}; static struct _tuple0* Cyc_Toc__untag_arr_qv=& Cyc_Toc__untag_arr_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_arr_re={ 1u,& Cyc_Toc__untag_arr_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={ 0,(
void*)(( void*)& Cyc_Toc__untag_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__untag_arr_e=&
Cyc_Toc__untag_arr_ev; static unsigned char _temp35[ 14u]="_get_arr_size";
static struct _tagged_arr Cyc_Toc__get_arr_size_str={ _temp35, _temp35, _temp35
+ 14u}; static struct _tuple0 Cyc_Toc__get_arr_size_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__get_arr_size_str}; static struct _tuple0* Cyc_Toc__get_arr_size_qv=&
Cyc_Toc__get_arr_size_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={
1u,& Cyc_Toc__get_arr_size_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__get_arr_size_ev={ 0,( void*)(( void*)& Cyc_Toc__get_arr_size_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__get_arr_size_e=& Cyc_Toc__get_arr_size_ev;
static unsigned char _temp37[ 17u]="_tagged_arr_plus"; static struct _tagged_arr
Cyc_Toc__tagged_arr_plus_str={ _temp37, _temp37, _temp37 + 17u}; static struct
_tuple0 Cyc_Toc__tagged_arr_plus_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_plus_str};
static struct _tuple0* Cyc_Toc__tagged_arr_plus_qv=& Cyc_Toc__tagged_arr_plus_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_plus_re={ 1u,& Cyc_Toc__tagged_arr_plus_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={
0,( void*)(( void*)& Cyc_Toc__tagged_arr_plus_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__tagged_arr_plus_e=& Cyc_Toc__tagged_arr_plus_ev; static unsigned char
_temp39[ 25u]="_tagged_arr_inplace_plus"; static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={
_temp39, _temp39, _temp39 + 25u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_str};
static struct _tuple0* Cyc_Toc__tagged_arr_inplace_plus_qv=& Cyc_Toc__tagged_arr_inplace_plus_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={ 1u,&
Cyc_Toc__tagged_arr_inplace_plus_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_inplace_plus_e=& Cyc_Toc__tagged_arr_inplace_plus_ev;
static unsigned char _temp41[ 30u]="_tagged_arr_inplace_plus_post"; static
struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={ _temp41, _temp41,
_temp41 + 30u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct _tuple0* Cyc_Toc__tagged_arr_inplace_plus_post_qv=& Cyc_Toc__tagged_arr_inplace_plus_post_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _temp43[ 10u]="GC_malloc"; static struct _tagged_arr Cyc_Toc_GC_malloc_str={
_temp43, _temp43, _temp43 + 10u}; static struct _tuple0 Cyc_Toc_GC_malloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_GC_malloc_str}; static struct
_tuple0* Cyc_Toc_GC_malloc_qv=& Cyc_Toc_GC_malloc_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_GC_malloc_re={ 1u,& Cyc_Toc_GC_malloc_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc_GC_malloc_ev={ 0,( void*)(( void*)& Cyc_Toc_GC_malloc_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc_GC_malloc_e=& Cyc_Toc_GC_malloc_ev;
static unsigned char _temp45[ 17u]="GC_malloc_atomic"; static struct _tagged_arr
Cyc_Toc_GC_malloc_atomic_str={ _temp45, _temp45, _temp45 + 17u}; static struct
_tuple0 Cyc_Toc_GC_malloc_atomic_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_GC_malloc_atomic_str};
static struct _tuple0* Cyc_Toc_GC_malloc_atomic_qv=& Cyc_Toc_GC_malloc_atomic_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_GC_malloc_atomic_re={ 1u,& Cyc_Toc_GC_malloc_atomic_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_GC_malloc_atomic_ev={
0,( void*)(( void*)& Cyc_Toc_GC_malloc_atomic_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc_GC_malloc_atomic_e=& Cyc_Toc_GC_malloc_atomic_ev; static unsigned char
_temp47[ 15u]="_region_malloc"; static struct _tagged_arr Cyc_Toc__region_malloc_str={
_temp47, _temp47, _temp47 + 15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str}; static struct
_tuple0* Cyc_Toc__region_malloc_qv=& Cyc_Toc__region_malloc_pr; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={ 1u,& Cyc_Toc__region_malloc_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={ 0,(
void*)(( void*)& Cyc_Toc__region_malloc_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev; static unsigned char
_temp49[ 12u]="_new_region"; static struct _tagged_arr Cyc_Toc__new_region_str={
_temp49, _temp49, _temp49 + 12u}; static struct _tuple0 Cyc_Toc__new_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__new_region_str}; static struct
_tuple0* Cyc_Toc__new_region_qv=& Cyc_Toc__new_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__new_region_re={ 1u,& Cyc_Toc__new_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={ 0,( void*)(( void*)& Cyc_Toc__new_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;
static unsigned char _temp51[ 13u]="_push_region"; static struct _tagged_arr Cyc_Toc__push_region_str={
_temp51, _temp51, _temp51 + 13u}; static struct _tuple0 Cyc_Toc__push_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str}; static struct
_tuple0* Cyc_Toc__push_region_qv=& Cyc_Toc__push_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={ 1u,& Cyc_Toc__push_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={ 0,( void*)(( void*)& Cyc_Toc__push_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;
static unsigned char _temp53[ 12u]="_pop_region"; static struct _tagged_arr Cyc_Toc__pop_region_str={
_temp53, _temp53, _temp53 + 12u}; static struct _tuple0 Cyc_Toc__pop_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_region_str}; static struct
_tuple0* Cyc_Toc__pop_region_qv=& Cyc_Toc__pop_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_region_re={ 1u,& Cyc_Toc__pop_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;
static unsigned char _temp55[ 15u]="_sfile_to_file"; static struct _tagged_arr
Cyc_Toc__sfile_to_file_str={ _temp55, _temp55, _temp55 + 15u}; static struct
_tuple0 Cyc_Toc__sfile_to_file_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__sfile_to_file_str};
static struct _tuple0* Cyc_Toc__sfile_to_file_qv=& Cyc_Toc__sfile_to_file_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__sfile_to_file_re={ 1u,& Cyc_Toc__sfile_to_file_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__sfile_to_file_ev={ 0,(
void*)(( void*)& Cyc_Toc__sfile_to_file_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__sfile_to_file_e=& Cyc_Toc__sfile_to_file_ev; static struct Cyc_Absyn_StructType_struct
Cyc_Toc_tagged_arr_typ_v={ 9u,( struct _tuple0*)& Cyc_Toc__tagged_arr_pr, 0, 0};
static void* Cyc_Toc_tagged_arr_typ=( void*)& Cyc_Toc_tagged_arr_typ_v; static
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={ 0, 0, 0}; static unsigned char _temp58[ 3u]="f0";
static struct _tagged_arr Cyc_Toc_f0={ _temp58, _temp58, _temp58 + 3u}; static
struct _tagged_arr* Cyc_Toc_field_names_v[ 1u]={& Cyc_Toc_f0}; static struct
_tagged_arr Cyc_Toc_field_names={( void*)(( struct _tagged_arr**) Cyc_Toc_field_names_v),(
void*)(( struct _tagged_arr**) Cyc_Toc_field_names_v),( void*)(( struct
_tagged_arr**) Cyc_Toc_field_names_v) + 1u}; static struct _tagged_arr* Cyc_Toc_fieldname(
int i){ unsigned int fsz= _get_arr_size( Cyc_Toc_field_names, sizeof( struct
_tagged_arr*)); if( i >= fsz){ Cyc_Toc_field_names=({ unsigned int _temp59=(
unsigned int)( i + 1); struct _tagged_arr** _temp60=( struct _tagged_arr**)
GC_malloc( sizeof( struct _tagged_arr*) * _temp59); struct _tagged_arr _temp63=
_tag_arr( _temp60, sizeof( struct _tagged_arr*),( unsigned int)( i + 1));{
unsigned int _temp61= _temp59; unsigned int j; for( j= 0; j < _temp61; j ++){
_temp60[ j]= j < fsz?*(( struct _tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names,
sizeof( struct _tagged_arr*),( int) j)):({ struct _tagged_arr* _temp62=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp62[ 0]=( struct
_tagged_arr) xprintf("f%d",( int) j); _temp62;});}}; _temp63;});} return*((
struct _tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names, sizeof(
struct _tagged_arr*), i));} struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp64=(
void*) e->r; void* _temp90; unsigned char _temp92; void* _temp94; void* _temp96;
short _temp98; void* _temp100; void* _temp102; int _temp104; void* _temp106;
void* _temp108; long long _temp110; void* _temp112; void* _temp114; struct Cyc_Absyn_Exp*
_temp116; void* _temp118; struct Cyc_List_List* _temp120; struct Cyc_List_List*
_temp122; struct Cyc_Absyn_Structdecl* _temp124; struct Cyc_List_List* _temp126;
struct Cyc_Core_Opt* _temp128; struct _tuple0* _temp130; struct Cyc_List_List*
_temp132; struct _tuple1* _temp134; struct Cyc_List_List* _temp136; struct Cyc_Core_Opt*
_temp138; _LL66: if(*(( int*) _temp64) == Cyc_Absyn_Const_e){ _LL91: _temp90=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if(( unsigned int)
_temp90 > 1u?*(( int*) _temp90) == Cyc_Absyn_Char_c: 0){ _LL95: _temp94=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp90)->f1; goto _LL93; _LL93: _temp92=((
struct Cyc_Absyn_Char_c_struct*) _temp90)->f2; goto _LL67;} else{ goto _LL68;}}
else{ goto _LL68;} _LL68: if(*(( int*) _temp64) == Cyc_Absyn_Const_e){ _LL97:
_temp96=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp96 > 1u?*(( int*) _temp96) == Cyc_Absyn_Short_c: 0){ _LL101:
_temp100=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp96)->f1; goto _LL99;
_LL99: _temp98=(( struct Cyc_Absyn_Short_c_struct*) _temp96)->f2; goto _LL69;}
else{ goto _LL70;}} else{ goto _LL70;} _LL70: if(*(( int*) _temp64) == Cyc_Absyn_Const_e){
_LL103: _temp102=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp102 > 1u?*(( int*) _temp102) == Cyc_Absyn_Int_c: 0){ _LL107:
_temp106=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp102)->f1; goto _LL105;
_LL105: _temp104=(( struct Cyc_Absyn_Int_c_struct*) _temp102)->f2; goto _LL71;}
else{ goto _LL72;}} else{ goto _LL72;} _LL72: if(*(( int*) _temp64) == Cyc_Absyn_Const_e){
_LL109: _temp108=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp108 > 1u?*(( int*) _temp108) == Cyc_Absyn_LongLong_c: 0){
_LL113: _temp112=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp108)->f1;
goto _LL111; _LL111: _temp110=(( struct Cyc_Absyn_LongLong_c_struct*) _temp108)->f2;
goto _LL73;} else{ goto _LL74;}} else{ goto _LL74;} _LL74: if(*(( int*) _temp64)
== Cyc_Absyn_Const_e){ _LL115: _temp114=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp64)->f1; if( _temp114 ==( void*) Cyc_Absyn_Null_c){ goto _LL75;} else{ goto
_LL76;}} else{ goto _LL76;} _LL76: if(*(( int*) _temp64) == Cyc_Absyn_Cast_e){
_LL119: _temp118=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp64)->f1; goto
_LL117; _LL117: _temp116=(( struct Cyc_Absyn_Cast_e_struct*) _temp64)->f2; goto
_LL77;} else{ goto _LL78;} _LL78: if(*(( int*) _temp64) == Cyc_Absyn_Tuple_e){
_LL121: _temp120=(( struct Cyc_Absyn_Tuple_e_struct*) _temp64)->f1; goto _LL79;}
else{ goto _LL80;} _LL80: if(*(( int*) _temp64) == Cyc_Absyn_Array_e){ _LL123:
_temp122=(( struct Cyc_Absyn_Array_e_struct*) _temp64)->f1; goto _LL81;} else{
goto _LL82;} _LL82: if(*(( int*) _temp64) == Cyc_Absyn_Struct_e){ _LL131:
_temp130=(( struct Cyc_Absyn_Struct_e_struct*) _temp64)->f1; goto _LL129; _LL129:
_temp128=(( struct Cyc_Absyn_Struct_e_struct*) _temp64)->f2; goto _LL127; _LL127:
_temp126=(( struct Cyc_Absyn_Struct_e_struct*) _temp64)->f3; goto _LL125; _LL125:
_temp124=(( struct Cyc_Absyn_Struct_e_struct*) _temp64)->f4; goto _LL83;} else{
goto _LL84;} _LL84: if(*(( int*) _temp64) == Cyc_Absyn_CompoundLit_e){ _LL135:
_temp134=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp64)->f1; goto _LL133;
_LL133: _temp132=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp64)->f2; goto
_LL85;} else{ goto _LL86;} _LL86: if(*(( int*) _temp64) == Cyc_Absyn_UnresolvedMem_e){
_LL139: _temp138=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp64)->f1; goto
_LL137; _LL137: _temp136=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp64)->f2;
goto _LL87;} else{ goto _LL88;} _LL88: goto _LL89; _LL67: return _temp92 =='\000';
_LL69: return _temp98 == 0; _LL71: return _temp104 == 0; _LL73: return _temp110
== 0; _LL75: return 1; _LL77: return Cyc_Toc_is_zero( _temp116); _LL79: return((
int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp120); _LL81: _temp126= _temp122; goto _LL83; _LL83:
_temp132= _temp126; goto _LL85; _LL85: _temp136= _temp132; goto _LL87; _LL87:
for( 0; _temp136 != 0; _temp136=(( struct Cyc_List_List*) _check_null( _temp136))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp136))->hd)).f2)){ return 0;}} return 1; _LL89: return 0; _LL65:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp140= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp146; struct Cyc_Absyn_Conref* _temp148; struct Cyc_Absyn_Tqual
_temp150; struct Cyc_Absyn_Conref* _temp152; void* _temp154; void* _temp156;
_LL142: if(( unsigned int) _temp140 > 4u?*(( int*) _temp140) == Cyc_Absyn_PointerType:
0){ _LL147: _temp146=(( struct Cyc_Absyn_PointerType_struct*) _temp140)->f1;
_LL157: _temp156=( void*) _temp146.elt_typ; goto _LL155; _LL155: _temp154=( void*)
_temp146.rgn_typ; goto _LL153; _LL153: _temp152= _temp146.nullable; goto _LL151;
_LL151: _temp150= _temp146.tq; goto _LL149; _LL149: _temp148= _temp146.bounds;
goto _LL143;} else{ goto _LL144;} _LL144: goto _LL145; _LL143: return(( int(*)(
int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp152); _LL145:((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("is_nullable",
sizeof( unsigned char), 12u)); return 0; _LL141:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag(
struct _tuple0* x, struct _tagged_arr tag){ return({ struct _tuple0* _temp158=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp158->f1=(* x).f1;
_temp158->f2=({ struct _tagged_arr* _temp159=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp159[ 0]=( struct _tagged_arr) Cyc_String_strconcat(*(*
x).f2, tag); _temp159;}); _temp158;});} struct _tuple5{ struct _tagged_arr* f1;
struct Cyc_List_List* f2; } ; static struct _tagged_arr* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp160= Cyc_Toc_tuple_types;
for( 0; _temp160 != 0; _temp160=(( struct Cyc_List_List*) _check_null( _temp160))->tl){
struct _tuple5 _temp163; struct Cyc_List_List* _temp164; struct _tagged_arr*
_temp166; struct _tuple5* _temp161=( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp160))->hd; _temp163=* _temp161; _LL167: _temp166= _temp163.f1;
goto _LL165; _LL165: _temp164= _temp163.f2; goto _LL162; _LL162: { struct Cyc_List_List*
_temp168= tqs0; for( 0; _temp168 != 0? _temp164 != 0: 0;( _temp168=(( struct Cyc_List_List*)
_check_null( _temp168))->tl, _temp164=(( struct Cyc_List_List*) _check_null(
_temp164))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp168))->hd)).f2,( void*)(( struct Cyc_List_List*) _check_null(
_temp164))->hd)){ break;}} if( _temp168 == 0? _temp164 == 0: 0){ return _temp166;}}}}{
struct _tagged_arr* x=({ struct _tagged_arr* _temp182=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp182[ 0]=( struct _tagged_arr)
xprintf("_tuple%d", Cyc_Toc_tuple_type_counter ++); _temp182;}); struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); struct Cyc_List_List* _temp169= 0;
struct Cyc_List_List* ts2= ts;{ int i= 1; for( 0; ts2 != 0;( ts2=(( struct Cyc_List_List*)
_check_null( ts2))->tl, i ++)){ _temp169=({ struct Cyc_List_List* _temp170=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp170->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp171=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp171->name= Cyc_Toc_fieldname(
i); _temp171->tq= Cyc_Toc_mt_tq; _temp171->type=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( ts2))->hd); _temp171->width= 0; _temp171->attributes= 0; _temp171;});
_temp170->tl= _temp169; _temp170;});}} _temp169=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp169);{ struct Cyc_Absyn_Structdecl*
_temp172=({ struct Cyc_Absyn_Structdecl* _temp178=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp178->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp178->name=({ struct Cyc_Core_Opt* _temp180=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp180->v=( void*)({ struct _tuple0*
_temp181=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp181->f1=
Cyc_Absyn_rel_ns_null; _temp181->f2= x; _temp181;}); _temp180;}); _temp178->tvs=
0; _temp178->fields=({ struct Cyc_Core_Opt* _temp179=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp179->v=( void*) _temp169;
_temp179;}); _temp178->attributes= 0; _temp178;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp173=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp173->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp174=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp174[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp175; _temp175.tag= Cyc_Absyn_Struct_d; _temp175.f1=
_temp172; _temp175;}); _temp174;}), 0); _temp173->tl= Cyc_Toc_result_decls;
_temp173;}); Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp176=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp176->hd=( void*)({ struct
_tuple5* _temp177=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp177->f1= x; _temp177->f2= ts; _temp177;}); _temp176->tl= Cyc_Toc_tuple_types;
_temp176;}); return x;}}} struct _tuple0* Cyc_Toc_temp_var(){ return({ struct
_tuple0* _temp183=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp183->f1=( void*) Cyc_Absyn_Loc_n; _temp183->f2=({ struct _tagged_arr*
_temp184=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp184[ 0]=( struct _tagged_arr) xprintf("_temp%d", Cyc_Toc_temp_var_counter
++); _temp184;}); _temp183;});} static struct _tagged_arr* Cyc_Toc_fresh_label(){
return({ struct _tagged_arr* _temp185=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)); _temp185[ 0]=( struct _tagged_arr) xprintf("_LL%d", Cyc_Toc_temp_var_counter
++); _temp185;});} static void* Cyc_Toc_typ_to_c( void* t); static struct
_tuple1* Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp188; struct Cyc_Absyn_Tqual
_temp190; struct Cyc_Core_Opt* _temp192; struct _tuple1 _temp186=* a; _LL193:
_temp192= _temp186.f1; goto _LL191; _LL191: _temp190= _temp186.f2; goto _LL189;
_LL189: _temp188= _temp186.f3; goto _LL187; _LL187: return({ struct _tuple1*
_temp194=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp194->f1=
_temp192; _temp194->f2= _temp190; _temp194->f3= Cyc_Toc_typ_to_c( _temp188);
_temp194;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp197; struct Cyc_Absyn_Tqual _temp199; struct _tuple3 _temp195=* x;
_LL200: _temp199= _temp195.f1; goto _LL198; _LL198: _temp197= _temp195.f2; goto
_LL196; _LL196: return({ struct _tuple3* _temp201=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp201->f1= _temp199; _temp201->f2= Cyc_Toc_typ_to_c(
_temp197); _temp201;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp202= t; struct Cyc_Absyn_Exp* _temp210; struct Cyc_Absyn_Tqual _temp212;
void* _temp214; struct Cyc_Core_Opt* _temp216; int _temp218; struct Cyc_Core_Opt*
_temp220; struct Cyc_Core_Opt _temp222; void* _temp223; struct Cyc_Core_Opt*
_temp225; _LL204: if(( unsigned int) _temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_ArrayType:
0){ _LL215: _temp214=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp202)->f1;
goto _LL213; _LL213: _temp212=(( struct Cyc_Absyn_ArrayType_struct*) _temp202)->f2;
goto _LL211; _LL211: _temp210=(( struct Cyc_Absyn_ArrayType_struct*) _temp202)->f3;
goto _LL205;} else{ goto _LL206;} _LL206: if(( unsigned int) _temp202 > 4u?*((
int*) _temp202) == Cyc_Absyn_Evar: 0){ _LL226: _temp225=(( struct Cyc_Absyn_Evar_struct*)
_temp202)->f1; goto _LL221; _LL221: _temp220=(( struct Cyc_Absyn_Evar_struct*)
_temp202)->f2; if( _temp220 == 0){ goto _LL208;} else{ _temp222=* _temp220;
_LL224: _temp223=( void*) _temp222.v; goto _LL219;} _LL219: _temp218=(( struct
Cyc_Absyn_Evar_struct*) _temp202)->f3; goto _LL217; _LL217: _temp216=(( struct
Cyc_Absyn_Evar_struct*) _temp202)->f4; goto _LL207;} else{ goto _LL208;} _LL208:
goto _LL209; _LL205: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp227=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp227[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp228; _temp228.tag= Cyc_Absyn_ArrayType;
_temp228.f1=( void*) Cyc_Toc_typ_to_c_array( _temp214); _temp228.f2= _temp212;
_temp228.f3= _temp210; _temp228;}); _temp227;}); _LL207: return Cyc_Toc_typ_to_c_array(
_temp223); _LL209: return Cyc_Toc_typ_to_c( t); _LL203:;} static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c( struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield*
_temp229=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp229->name= f->name; _temp229->tq= f->tq; _temp229->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp229->width= f->width; _temp229->attributes= f->attributes;
_temp229;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp230= t; struct
Cyc_Core_Opt* _temp282; int _temp284; struct Cyc_Core_Opt* _temp286; struct Cyc_Core_Opt*
_temp288; struct Cyc_Core_Opt* _temp290; int _temp292; struct Cyc_Core_Opt*
_temp294; struct Cyc_Core_Opt _temp296; void* _temp297; struct Cyc_Core_Opt*
_temp299; struct Cyc_Absyn_Tvar* _temp301; struct Cyc_Absyn_TunionInfo _temp303;
void* _temp305; struct Cyc_List_List* _temp307; void* _temp309; struct Cyc_Absyn_Tuniondecl*
_temp311; struct Cyc_Absyn_TunionInfo _temp313; struct Cyc_Absyn_TunionFieldInfo
_temp315; struct Cyc_List_List* _temp317; void* _temp319; struct Cyc_Absyn_Tunionfield*
_temp321; struct Cyc_Absyn_Tuniondecl* _temp323; struct Cyc_Absyn_TunionFieldInfo
_temp325; struct Cyc_Absyn_PtrInfo _temp327; struct Cyc_Absyn_Conref* _temp329;
struct Cyc_Absyn_Tqual _temp331; struct Cyc_Absyn_Conref* _temp333; void*
_temp335; void* _temp337; void* _temp339; void* _temp341; struct Cyc_Absyn_Exp*
_temp343; struct Cyc_Absyn_Tqual _temp345; void* _temp347; struct Cyc_Absyn_FnInfo
_temp349; struct Cyc_List_List* _temp351; struct Cyc_List_List* _temp353; struct
Cyc_Absyn_VarargInfo* _temp355; int _temp357; struct Cyc_List_List* _temp359;
void* _temp361; struct Cyc_Core_Opt* _temp363; struct Cyc_List_List* _temp365;
struct Cyc_List_List* _temp367; struct Cyc_List_List* _temp369; struct Cyc_List_List*
_temp371; struct Cyc_Absyn_Uniondecl** _temp373; struct Cyc_List_List* _temp375;
struct _tuple0* _temp377; struct Cyc_Absyn_Structdecl** _temp379; struct Cyc_List_List*
_temp381; struct _tuple0* _temp383; struct Cyc_Absyn_Enumdecl* _temp385; struct
_tuple0* _temp387; struct Cyc_Core_Opt* _temp389; struct Cyc_List_List* _temp391;
struct _tuple0* _temp393; void* _temp395; void* _temp397; struct Cyc_List_List*
_temp399; _LL232: if( _temp230 ==( void*) Cyc_Absyn_VoidType){ goto _LL233;}
else{ goto _LL234;} _LL234: if(( unsigned int) _temp230 > 4u?*(( int*) _temp230)
== Cyc_Absyn_Evar: 0){ _LL289: _temp288=(( struct Cyc_Absyn_Evar_struct*)
_temp230)->f1; goto _LL287; _LL287: _temp286=(( struct Cyc_Absyn_Evar_struct*)
_temp230)->f2; if( _temp286 == 0){ goto _LL285;} else{ goto _LL236;} _LL285:
_temp284=(( struct Cyc_Absyn_Evar_struct*) _temp230)->f3; goto _LL283; _LL283:
_temp282=(( struct Cyc_Absyn_Evar_struct*) _temp230)->f4; goto _LL235;} else{
goto _LL236;} _LL236: if(( unsigned int) _temp230 > 4u?*(( int*) _temp230) ==
Cyc_Absyn_Evar: 0){ _LL300: _temp299=(( struct Cyc_Absyn_Evar_struct*) _temp230)->f1;
goto _LL295; _LL295: _temp294=(( struct Cyc_Absyn_Evar_struct*) _temp230)->f2;
if( _temp294 == 0){ goto _LL238;} else{ _temp296=* _temp294; _LL298: _temp297=(
void*) _temp296.v; goto _LL293;} _LL293: _temp292=(( struct Cyc_Absyn_Evar_struct*)
_temp230)->f3; goto _LL291; _LL291: _temp290=(( struct Cyc_Absyn_Evar_struct*)
_temp230)->f4; goto _LL237;} else{ goto _LL238;} _LL238: if(( unsigned int)
_temp230 > 4u?*(( int*) _temp230) == Cyc_Absyn_VarType: 0){ _LL302: _temp301=((
struct Cyc_Absyn_VarType_struct*) _temp230)->f1; goto _LL239;} else{ goto _LL240;}
_LL240: if(( unsigned int) _temp230 > 4u?*(( int*) _temp230) == Cyc_Absyn_TunionType:
0){ _LL304: _temp303=(( struct Cyc_Absyn_TunionType_struct*) _temp230)->f1;
_LL310: _temp309=( void*) _temp303.tunion_info; if(*(( int*) _temp309) == Cyc_Absyn_KnownTunion){
_LL312: _temp311=(( struct Cyc_Absyn_KnownTunion_struct*) _temp309)->f1; goto
_LL308;} else{ goto _LL242;} _LL308: _temp307= _temp303.targs; goto _LL306;
_LL306: _temp305=( void*) _temp303.rgn; goto _LL241;} else{ goto _LL242;} _LL242:
if(( unsigned int) _temp230 > 4u?*(( int*) _temp230) == Cyc_Absyn_TunionType: 0){
_LL314: _temp313=(( struct Cyc_Absyn_TunionType_struct*) _temp230)->f1; goto
_LL243;} else{ goto _LL244;} _LL244: if(( unsigned int) _temp230 > 4u?*(( int*)
_temp230) == Cyc_Absyn_TunionFieldType: 0){ _LL316: _temp315=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp230)->f1; _LL320: _temp319=( void*) _temp315.field_info; if(*(( int*)
_temp319) == Cyc_Absyn_KnownTunionfield){ _LL324: _temp323=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp319)->f1; goto _LL322; _LL322: _temp321=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp319)->f2; goto _LL318;} else{ goto _LL246;} _LL318: _temp317= _temp315.targs;
goto _LL245;} else{ goto _LL246;} _LL246: if(( unsigned int) _temp230 > 4u?*((
int*) _temp230) == Cyc_Absyn_TunionFieldType: 0){ _LL326: _temp325=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp230)->f1; goto _LL247;} else{ goto _LL248;} _LL248: if(( unsigned int)
_temp230 > 4u?*(( int*) _temp230) == Cyc_Absyn_PointerType: 0){ _LL328: _temp327=((
struct Cyc_Absyn_PointerType_struct*) _temp230)->f1; _LL338: _temp337=( void*)
_temp327.elt_typ; goto _LL336; _LL336: _temp335=( void*) _temp327.rgn_typ; goto
_LL334; _LL334: _temp333= _temp327.nullable; goto _LL332; _LL332: _temp331=
_temp327.tq; goto _LL330; _LL330: _temp329= _temp327.bounds; goto _LL249;} else{
goto _LL250;} _LL250: if(( unsigned int) _temp230 > 4u?*(( int*) _temp230) ==
Cyc_Absyn_IntType: 0){ _LL342: _temp341=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp230)->f1; goto _LL340; _LL340: _temp339=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp230)->f2; goto _LL251;} else{ goto _LL252;} _LL252: if( _temp230 ==( void*)
Cyc_Absyn_FloatType){ goto _LL253;} else{ goto _LL254;} _LL254: if( _temp230 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL255;} else{ goto _LL256;} _LL256: if((
unsigned int) _temp230 > 4u?*(( int*) _temp230) == Cyc_Absyn_ArrayType: 0){
_LL348: _temp347=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp230)->f1;
goto _LL346; _LL346: _temp345=(( struct Cyc_Absyn_ArrayType_struct*) _temp230)->f2;
goto _LL344; _LL344: _temp343=(( struct Cyc_Absyn_ArrayType_struct*) _temp230)->f3;
goto _LL257;} else{ goto _LL258;} _LL258: if(( unsigned int) _temp230 > 4u?*((
int*) _temp230) == Cyc_Absyn_FnType: 0){ _LL350: _temp349=(( struct Cyc_Absyn_FnType_struct*)
_temp230)->f1; _LL366: _temp365= _temp349.tvars; goto _LL364; _LL364: _temp363=
_temp349.effect; goto _LL362; _LL362: _temp361=( void*) _temp349.ret_typ; goto
_LL360; _LL360: _temp359= _temp349.args; goto _LL358; _LL358: _temp357= _temp349.c_varargs;
goto _LL356; _LL356: _temp355= _temp349.cyc_varargs; goto _LL354; _LL354:
_temp353= _temp349.rgn_po; goto _LL352; _LL352: _temp351= _temp349.attributes;
goto _LL259;} else{ goto _LL260;} _LL260: if(( unsigned int) _temp230 > 4u?*((
int*) _temp230) == Cyc_Absyn_TupleType: 0){ _LL368: _temp367=(( struct Cyc_Absyn_TupleType_struct*)
_temp230)->f1; goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int)
_temp230 > 4u?*(( int*) _temp230) == Cyc_Absyn_AnonStructType: 0){ _LL370:
_temp369=(( struct Cyc_Absyn_AnonStructType_struct*) _temp230)->f1; goto _LL263;}
else{ goto _LL264;} _LL264: if(( unsigned int) _temp230 > 4u?*(( int*) _temp230)
== Cyc_Absyn_AnonUnionType: 0){ _LL372: _temp371=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp230)->f1; goto _LL265;} else{ goto _LL266;} _LL266: if(( unsigned int)
_temp230 > 4u?*(( int*) _temp230) == Cyc_Absyn_UnionType: 0){ _LL378: _temp377=((
struct Cyc_Absyn_UnionType_struct*) _temp230)->f1; goto _LL376; _LL376: _temp375=((
struct Cyc_Absyn_UnionType_struct*) _temp230)->f2; goto _LL374; _LL374: _temp373=((
struct Cyc_Absyn_UnionType_struct*) _temp230)->f3; goto _LL267;} else{ goto
_LL268;} _LL268: if(( unsigned int) _temp230 > 4u?*(( int*) _temp230) == Cyc_Absyn_StructType:
0){ _LL384: _temp383=(( struct Cyc_Absyn_StructType_struct*) _temp230)->f1; goto
_LL382; _LL382: _temp381=(( struct Cyc_Absyn_StructType_struct*) _temp230)->f2;
goto _LL380; _LL380: _temp379=(( struct Cyc_Absyn_StructType_struct*) _temp230)->f3;
goto _LL269;} else{ goto _LL270;} _LL270: if(( unsigned int) _temp230 > 4u?*((
int*) _temp230) == Cyc_Absyn_EnumType: 0){ _LL388: _temp387=(( struct Cyc_Absyn_EnumType_struct*)
_temp230)->f1; goto _LL386; _LL386: _temp385=(( struct Cyc_Absyn_EnumType_struct*)
_temp230)->f2; goto _LL271;} else{ goto _LL272;} _LL272: if(( unsigned int)
_temp230 > 4u?*(( int*) _temp230) == Cyc_Absyn_TypedefType: 0){ _LL394: _temp393=((
struct Cyc_Absyn_TypedefType_struct*) _temp230)->f1; goto _LL392; _LL392:
_temp391=(( struct Cyc_Absyn_TypedefType_struct*) _temp230)->f2; goto _LL390;
_LL390: _temp389=(( struct Cyc_Absyn_TypedefType_struct*) _temp230)->f3; goto
_LL273;} else{ goto _LL274;} _LL274: if(( unsigned int) _temp230 > 4u?*(( int*)
_temp230) == Cyc_Absyn_RgnHandleType: 0){ _LL396: _temp395=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp230)->f1; goto _LL275;} else{ goto _LL276;} _LL276: if( _temp230 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL277;} else{ goto _LL278;} _LL278: if(( unsigned int)
_temp230 > 4u?*(( int*) _temp230) == Cyc_Absyn_AccessEff: 0){ _LL398: _temp397=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp230)->f1; goto _LL279;} else{
goto _LL280;} _LL280: if(( unsigned int) _temp230 > 4u?*(( int*) _temp230) ==
Cyc_Absyn_JoinEff: 0){ _LL400: _temp399=(( struct Cyc_Absyn_JoinEff_struct*)
_temp230)->f1; goto _LL281;} else{ goto _LL231;} _LL233: return t; _LL235:
return Cyc_Absyn_sint_t; _LL237: return Cyc_Toc_typ_to_c( _temp297); _LL239:
return Cyc_Absyn_void_star_typ(); _LL241: return Cyc_Absyn_void_star_typ();
_LL243:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("unresolved TunionType", sizeof( unsigned char), 22u))); _LL245: if(
_temp321->typs == 0){ if( _temp323->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp321->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL247:( int) _throw((( void*(*)(
struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u))); _LL249: _temp337= Cyc_Toc_typ_to_c_array(
_temp337);{ void* _temp401=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp329))->v; void* _temp407; _LL403: if((
unsigned int) _temp401 > 1u?*(( int*) _temp401) == Cyc_Absyn_Eq_constr: 0){
_LL408: _temp407=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp401)->f1;
if( _temp407 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL404;} else{ goto _LL405;}}
else{ goto _LL405;} _LL405: goto _LL406; _LL404: return Cyc_Toc_tagged_arr_typ;
_LL406: return Cyc_Absyn_star_typ( _temp337,( void*) Cyc_Absyn_HeapRgn, _temp331);
_LL402:;} _LL251: return t; _LL253: return t; _LL255: return t; _LL257: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp409=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp409[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp410; _temp410.tag= Cyc_Absyn_ArrayType; _temp410.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp347); _temp410.f2= _temp345; _temp410.f3=
_temp343; _temp410;}); _temp409;}); _LL259: { struct Cyc_List_List* _temp411= 0;
for( 0; _temp351 != 0; _temp351=(( struct Cyc_List_List*) _check_null( _temp351))->tl){
void* _temp412=( void*)(( struct Cyc_List_List*) _check_null( _temp351))->hd;
_LL414: if( _temp412 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL415;} else{
goto _LL416;} _LL416: if( _temp412 ==( void*) Cyc_Absyn_Const_att){ goto _LL417;}
else{ goto _LL418;} _LL418: goto _LL419; _LL415: goto _LL417; _LL417: continue;
_LL419: _temp411=({ struct Cyc_List_List* _temp420=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp420->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( _temp351))->hd); _temp420->tl= _temp411;
_temp420;}); goto _LL413; _LL413:;}{ struct Cyc_List_List* _temp421=(( struct
Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp359); if( _temp355 != 0){ void*
_temp422= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp355))->type,( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp355))->rgn, Cyc_Toc_mt_tq)); struct _tuple1* _temp423=({
struct _tuple1* _temp425=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp425->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp355))->name;
_temp425->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp355))->tq;
_temp425->f3= _temp422; _temp425;}); _temp421=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp421,({ struct Cyc_List_List* _temp424=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp424->hd=( void*) _temp423; _temp424->tl= 0;
_temp424;}));} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp426=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp426[ 0]=({ struct Cyc_Absyn_FnType_struct _temp427; _temp427.tag= Cyc_Absyn_FnType;
_temp427.f1=({ struct Cyc_Absyn_FnInfo _temp428; _temp428.tvars= 0; _temp428.effect=
0; _temp428.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp361); _temp428.args=
_temp421; _temp428.c_varargs= _temp357; _temp428.cyc_varargs= 0; _temp428.rgn_po=
0; _temp428.attributes= _temp411; _temp428;}); _temp427;}); _temp426;});}}
_LL261: _temp367=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp367);{
struct _tagged_arr* _temp429= Cyc_Toc_add_tuple_type( _temp367); return Cyc_Absyn_strct(
_temp429);} _LL263: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp430=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp430[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp431; _temp431.tag= Cyc_Absyn_AnonStructType; _temp431.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp369); _temp431;}); _temp430;});
_LL265: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp432=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp432[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp433; _temp433.tag=
Cyc_Absyn_AnonUnionType; _temp433.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp371); _temp433;}); _temp432;}); _LL267: if( _temp377 == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous union", sizeof(
unsigned char), 16u));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp377)); _LL269: if( _temp383 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous struct", sizeof(
unsigned char), 17u));} return Cyc_Absyn_strctq(( struct _tuple0*) _check_null(
_temp383)); _LL271: return t; _LL273: if( _temp389 == 0){ return(( void*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: unresolved TypedefType",
sizeof( unsigned char), 38u));} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp434=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp434[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp435; _temp435.tag= Cyc_Absyn_TypedefType;
_temp435.f1= _temp393; _temp435.f2= 0; _temp435.f3=({ struct Cyc_Core_Opt*
_temp436=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp436->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp389))->v); _temp436;}); _temp435;}); _temp434;}); _LL275:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL277: return(( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u)); _LL279: return(( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u)); _LL281: return(( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u)); _LL231:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp437= t; struct Cyc_Absyn_Exp* _temp443; struct Cyc_Absyn_Tqual _temp445;
void* _temp447; _LL439: if(( unsigned int) _temp437 > 4u?*(( int*) _temp437) ==
Cyc_Absyn_ArrayType: 0){ _LL448: _temp447=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp437)->f1; goto _LL446; _LL446: _temp445=(( struct Cyc_Absyn_ArrayType_struct*)
_temp437)->f2; goto _LL444; _LL444: _temp443=(( struct Cyc_Absyn_ArrayType_struct*)
_temp437)->f3; goto _LL440;} else{ goto _LL441;} _LL441: goto _LL442; _LL440:
return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ( _temp447,( void*) Cyc_Absyn_HeapRgn,
_temp445), e, l); _LL442: return Cyc_Absyn_cast_exp( t, e, l); _LL438:;} static
int Cyc_Toc_atomic_typ( void* t){ void* _temp449= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp485; void* _temp487; void* _temp489; struct Cyc_Absyn_FnInfo
_temp491; struct Cyc_Absyn_Exp* _temp493; struct Cyc_Absyn_Tqual _temp495; void*
_temp497; struct Cyc_List_List* _temp499; struct Cyc_Absyn_Structdecl** _temp501;
struct Cyc_List_List* _temp503; struct _tuple0* _temp505; struct Cyc_Absyn_TunionInfo
_temp507; struct Cyc_List_List* _temp509; struct Cyc_List_List* _temp511; struct
Cyc_Absyn_TunionFieldInfo _temp513; struct Cyc_List_List* _temp515; void*
_temp517; struct Cyc_Absyn_Tunionfield* _temp519; struct Cyc_Absyn_Tuniondecl*
_temp521; struct Cyc_Absyn_Uniondecl** _temp523; struct Cyc_List_List* _temp525;
struct _tuple0* _temp527; struct Cyc_Absyn_PtrInfo _temp529; void* _temp531;
_LL451: if( _temp449 ==( void*) Cyc_Absyn_VoidType){ goto _LL452;} else{ goto
_LL453;} _LL453: if(( unsigned int) _temp449 > 4u?*(( int*) _temp449) == Cyc_Absyn_VarType:
0){ _LL486: _temp485=(( struct Cyc_Absyn_VarType_struct*) _temp449)->f1; goto
_LL454;} else{ goto _LL455;} _LL455: if(( unsigned int) _temp449 > 4u?*(( int*)
_temp449) == Cyc_Absyn_IntType: 0){ _LL490: _temp489=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp449)->f1; goto _LL488; _LL488: _temp487=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp449)->f2; goto _LL456;} else{ goto _LL457;} _LL457: if( _temp449 ==( void*)
Cyc_Absyn_FloatType){ goto _LL458;} else{ goto _LL459;} _LL459: if( _temp449 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL460;} else{ goto _LL461;} _LL461: if((
unsigned int) _temp449 > 4u?*(( int*) _temp449) == Cyc_Absyn_FnType: 0){ _LL492:
_temp491=(( struct Cyc_Absyn_FnType_struct*) _temp449)->f1; goto _LL462;} else{
goto _LL463;} _LL463: if(( unsigned int) _temp449 > 4u?*(( int*) _temp449) ==
Cyc_Absyn_ArrayType: 0){ _LL498: _temp497=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp449)->f1; goto _LL496; _LL496: _temp495=(( struct Cyc_Absyn_ArrayType_struct*)
_temp449)->f2; goto _LL494; _LL494: _temp493=(( struct Cyc_Absyn_ArrayType_struct*)
_temp449)->f3; goto _LL464;} else{ goto _LL465;} _LL465: if(( unsigned int)
_temp449 > 4u?*(( int*) _temp449) == Cyc_Absyn_TupleType: 0){ _LL500: _temp499=((
struct Cyc_Absyn_TupleType_struct*) _temp449)->f1; goto _LL466;} else{ goto
_LL467;} _LL467: if(( unsigned int) _temp449 > 4u?*(( int*) _temp449) == Cyc_Absyn_StructType:
0){ _LL506: _temp505=(( struct Cyc_Absyn_StructType_struct*) _temp449)->f1; goto
_LL504; _LL504: _temp503=(( struct Cyc_Absyn_StructType_struct*) _temp449)->f2;
goto _LL502; _LL502: _temp501=(( struct Cyc_Absyn_StructType_struct*) _temp449)->f3;
goto _LL468;} else{ goto _LL469;} _LL469: if(( unsigned int) _temp449 > 4u?*((
int*) _temp449) == Cyc_Absyn_TunionType: 0){ _LL508: _temp507=(( struct Cyc_Absyn_TunionType_struct*)
_temp449)->f1; goto _LL470;} else{ goto _LL471;} _LL471: if(( unsigned int)
_temp449 > 4u?*(( int*) _temp449) == Cyc_Absyn_AnonStructType: 0){ _LL510:
_temp509=(( struct Cyc_Absyn_AnonStructType_struct*) _temp449)->f1; goto _LL472;}
else{ goto _LL473;} _LL473: if(( unsigned int) _temp449 > 4u?*(( int*) _temp449)
== Cyc_Absyn_AnonUnionType: 0){ _LL512: _temp511=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp449)->f1; goto _LL474;} else{ goto _LL475;} _LL475: if(( unsigned int)
_temp449 > 4u?*(( int*) _temp449) == Cyc_Absyn_TunionFieldType: 0){ _LL514:
_temp513=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp449)->f1; _LL518:
_temp517=( void*) _temp513.field_info; if(*(( int*) _temp517) == Cyc_Absyn_KnownTunionfield){
_LL522: _temp521=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp517)->f1;
goto _LL520; _LL520: _temp519=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp517)->f2; goto _LL516;} else{ goto _LL477;} _LL516: _temp515= _temp513.targs;
goto _LL476;} else{ goto _LL477;} _LL477: if(( unsigned int) _temp449 > 4u?*((
int*) _temp449) == Cyc_Absyn_UnionType: 0){ _LL528: _temp527=(( struct Cyc_Absyn_UnionType_struct*)
_temp449)->f1; goto _LL526; _LL526: _temp525=(( struct Cyc_Absyn_UnionType_struct*)
_temp449)->f2; goto _LL524; _LL524: _temp523=(( struct Cyc_Absyn_UnionType_struct*)
_temp449)->f3; goto _LL478;} else{ goto _LL479;} _LL479: if(( unsigned int)
_temp449 > 4u?*(( int*) _temp449) == Cyc_Absyn_PointerType: 0){ _LL530: _temp529=((
struct Cyc_Absyn_PointerType_struct*) _temp449)->f1; goto _LL480;} else{ goto
_LL481;} _LL481: if(( unsigned int) _temp449 > 4u?*(( int*) _temp449) == Cyc_Absyn_RgnHandleType:
0){ _LL532: _temp531=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp449)->f1;
goto _LL482;} else{ goto _LL483;} _LL483: goto _LL484; _LL452: return 1; _LL454:
return 0; _LL456: return 1; _LL458: return 1; _LL460: return 1; _LL462: return 1;
_LL464: return Cyc_Toc_atomic_typ( _temp497); _LL466: for( 0; _temp499 != 0;
_temp499=(( struct Cyc_List_List*) _check_null( _temp499))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp499))->hd)).f2)){
return 0;}} return 1; _LL468: return 0; _LL470: return 0; _LL472: _temp511=
_temp509; goto _LL474; _LL474: for( 0; _temp511 != 0; _temp511=(( struct Cyc_List_List*)
_check_null( _temp511))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp511))->hd)->type)){ return 0;}} return
1; _LL476: return _temp519->typs == 0; _LL478: return 0; _LL480: return 0;
_LL482: return 0; _LL484:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp533= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _get_arr_size( _temp533, 1u), _temp533.curr);}));
return 0; _LL450:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp534=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp540; struct Cyc_Absyn_Conref*
_temp542; struct Cyc_Absyn_Tqual _temp544; struct Cyc_Absyn_Conref* _temp546;
void* _temp548; void* _temp550; _LL536: if(( unsigned int) _temp534 > 4u?*(( int*)
_temp534) == Cyc_Absyn_PointerType: 0){ _LL541: _temp540=(( struct Cyc_Absyn_PointerType_struct*)
_temp534)->f1; _LL551: _temp550=( void*) _temp540.elt_typ; goto _LL549; _LL549:
_temp548=( void*) _temp540.rgn_typ; goto _LL547; _LL547: _temp546= _temp540.nullable;
goto _LL545; _LL545: _temp544= _temp540.tq; goto _LL543; _LL543: _temp542=
_temp540.bounds; goto _LL537;} else{ goto _LL538;} _LL538: goto _LL539; _LL537: {
void* _temp552= Cyc_Tcutil_compress( _temp550); _LL554: if( _temp552 ==( void*)
Cyc_Absyn_VoidType){ goto _LL555;} else{ goto _LL556;} _LL556: goto _LL557;
_LL555: return 1; _LL557: return 0; _LL553:;} _LL539: return 0; _LL535:;} static
int Cyc_Toc_is_poly_field( void* t, struct _tagged_arr* f){ void* _temp558= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Structdecl** _temp570; struct Cyc_Absyn_Structdecl*
_temp572; struct Cyc_List_List* _temp573; struct _tuple0* _temp575; struct Cyc_List_List*
_temp577; struct Cyc_List_List* _temp579; struct Cyc_Absyn_Uniondecl** _temp581;
struct Cyc_List_List* _temp583; struct _tuple0* _temp585; _LL560: if((
unsigned int) _temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_StructType: 0){
_LL576: _temp575=(( struct Cyc_Absyn_StructType_struct*) _temp558)->f1; goto
_LL574; _LL574: _temp573=(( struct Cyc_Absyn_StructType_struct*) _temp558)->f2;
goto _LL571; _LL571: _temp570=(( struct Cyc_Absyn_StructType_struct*) _temp558)->f3;
if( _temp570 == 0){ goto _LL562;} else{ _temp572=* _temp570; goto _LL561;}}
else{ goto _LL562;} _LL562: if(( unsigned int) _temp558 > 4u?*(( int*) _temp558)
== Cyc_Absyn_AnonStructType: 0){ _LL578: _temp577=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp558)->f1; goto _LL563;} else{ goto _LL564;} _LL564: if(( unsigned int)
_temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_AnonUnionType: 0){ _LL580:
_temp579=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp558)->f1; goto _LL565;}
else{ goto _LL566;} _LL566: if(( unsigned int) _temp558 > 4u?*(( int*) _temp558)
== Cyc_Absyn_UnionType: 0){ _LL586: _temp585=(( struct Cyc_Absyn_UnionType_struct*)
_temp558)->f1; goto _LL584; _LL584: _temp583=(( struct Cyc_Absyn_UnionType_struct*)
_temp558)->f2; goto _LL582; _LL582: _temp581=(( struct Cyc_Absyn_UnionType_struct*)
_temp558)->f3; goto _LL567;} else{ goto _LL568;} _LL568: goto _LL569; _LL561:
if( _temp572->fields == 0){( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: struct missing fields", sizeof( unsigned char), 37u)));}
_temp577=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp572->fields))->v;
goto _LL563; _LL563: { struct Cyc_Absyn_Structfield* _temp587= Cyc_Absyn_lookup_field(
_temp577, f); if( _temp587 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp588=*(( struct Cyc_Absyn_Structfield*)
_check_null( _temp587))->name; xprintf("is_poly_field: bad field %.*s",
_get_arr_size( _temp588, 1u), _temp588.curr);}));} return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp587))->type);} _LL565:
return 0; _LL567: return 0; _LL569:( int) _throw((( void*(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr _temp589= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field: bad type %.*s", _get_arr_size( _temp589, 1u),
_temp589.curr);}))); _LL559:;} static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp*
e){ void* _temp590=( void*) e->r; struct _tagged_arr* _temp598; struct Cyc_Absyn_Exp*
_temp600; struct _tagged_arr* _temp602; struct Cyc_Absyn_Exp* _temp604; _LL592:
if(*(( int*) _temp590) == Cyc_Absyn_StructMember_e){ _LL601: _temp600=(( struct
Cyc_Absyn_StructMember_e_struct*) _temp590)->f1; goto _LL599; _LL599: _temp598=((
struct Cyc_Absyn_StructMember_e_struct*) _temp590)->f2; goto _LL593;} else{ goto
_LL594;} _LL594: if(*(( int*) _temp590) == Cyc_Absyn_StructArrow_e){ _LL605:
_temp604=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp590)->f1; goto _LL603;
_LL603: _temp602=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp590)->f2; goto
_LL595;} else{ goto _LL596;} _LL596: goto _LL597; _LL593: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp600->topt))->v, _temp598);
_LL595: { void* _temp606= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp604->topt))->v); struct Cyc_Absyn_PtrInfo _temp612; struct Cyc_Absyn_Conref*
_temp614; struct Cyc_Absyn_Tqual _temp616; struct Cyc_Absyn_Conref* _temp618;
void* _temp620; void* _temp622; _LL608: if(( unsigned int) _temp606 > 4u?*(( int*)
_temp606) == Cyc_Absyn_PointerType: 0){ _LL613: _temp612=(( struct Cyc_Absyn_PointerType_struct*)
_temp606)->f1; _LL623: _temp622=( void*) _temp612.elt_typ; goto _LL621; _LL621:
_temp620=( void*) _temp612.rgn_typ; goto _LL619; _LL619: _temp618= _temp612.nullable;
goto _LL617; _LL617: _temp616= _temp612.tq; goto _LL615; _LL615: _temp614=
_temp612.bounds; goto _LL609;} else{ goto _LL610;} _LL610: goto _LL611; _LL609:
return Cyc_Toc_is_poly_field( _temp622, _temp602); _LL611:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp624= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp604->topt))->v); xprintf("is_poly_project:  bad type %.*s", _get_arr_size(
_temp624, 1u), _temp624.curr);})); return 0; _LL607:;} _LL597: return 0; _LL591:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_e,({ struct Cyc_List_List*
_temp625=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp625->hd=( void*) s; _temp625->tl= 0; _temp625;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc_GC_malloc_atomic_e,({ struct Cyc_List_List* _temp626=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp626->hd=( void*) s; _temp626->tl=
0; _temp626;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t,
struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic(
s);} return Cyc_Toc_malloc_ptr( s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({ struct Cyc_List_List*(* _temp627)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp630= rgn; struct Cyc_Absyn_Exp* _temp631= s; struct
Cyc_Absyn_Exp* _temp628[ 2u]={ _temp630, _temp631}; struct _tagged_arr _temp629={(
void*) _temp628,( void*) _temp628,( void*)( _temp628 + 2u)}; _temp627( _temp629);}),
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Toc__sfile_to_file_e,({ struct Cyc_List_List*
_temp632=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp632->hd=( void*) e; _temp632->tl= 0; _temp632;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({
struct Cyc_List_List* _temp633=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp633->hd=( void*) e; _temp633->tl= 0; _temp633;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_toplevel_tagged_arr( void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ int is_string= 0;{ void* _temp634=(
void*) e->r; void* _temp640; struct _tagged_arr _temp642; _LL636: if(*(( int*)
_temp634) == Cyc_Absyn_Const_e){ _LL641: _temp640=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp634)->f1; if(( unsigned int) _temp640 > 1u?*(( int*) _temp640) == Cyc_Absyn_String_c:
0){ _LL643: _temp642=(( struct Cyc_Absyn_String_c_struct*) _temp640)->f1; goto
_LL637;} else{ goto _LL638;}} else{ goto _LL638;} _LL638: goto _LL639; _LL637:
is_string= 1; goto _LL635; _LL639: goto _LL635; _LL635:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp647=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp647[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp648; _temp648.tag= Cyc_Absyn_ArrayType; _temp648.f1=(
void*) Cyc_Absyn_uchar_t; _temp648.f2= Cyc_Toc_mt_tq; _temp648.f3=( struct Cyc_Absyn_Exp*)
sz; _temp648;}); _temp647;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp644=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp644->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp645=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp645[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp646; _temp646.tag= Cyc_Absyn_Var_d;
_temp646.f1= vd; _temp646;}); _temp645;}), 0); _temp644->tl= Cyc_Toc_result_decls;
_temp644;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct Cyc_List_List*(* _temp649)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct _tuple4* _temp652=({ struct _tuple4*
_temp657=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp657->f1= 0;
_temp657->f2= xexp; _temp657;}); struct _tuple4* _temp653=({ struct _tuple4*
_temp656=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp656->f1= 0;
_temp656->f2= xexp; _temp656;}); struct _tuple4* _temp654=({ struct _tuple4*
_temp655=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp655->f1= 0;
_temp655->f2= xplussz; _temp655;}); struct _tuple4* _temp650[ 3u]={ _temp652,
_temp653, _temp654}; struct _tagged_arr _temp651={( void*) _temp650,( void*)
_temp650,( void*)( _temp650 + 3u)}; _temp649( _temp651);}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp658=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp658->break_lab= 0; _temp658->continue_lab= 0; _temp658->fallthru_info=
0; _temp658->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp658->toplevel= 1;
_temp658;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp659=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp659->break_lab= e->break_lab; _temp659->continue_lab=
e->continue_lab; _temp659->fallthru_info= e->fallthru_info; _temp659->varmap= e->varmap;
_temp659->toplevel= e->toplevel; _temp659;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp660=(* x).f1; struct
Cyc_List_List* _temp666; _LL662: if(( unsigned int) _temp660 > 1u?*(( int*)
_temp660) == Cyc_Absyn_Rel_n: 0){ _LL667: _temp666=(( struct Cyc_Absyn_Rel_n_struct*)
_temp660)->f1; goto _LL663;} else{ goto _LL664;} _LL664: goto _LL665; _LL663:(
int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct
_tagged_arr)({ struct _tagged_arr _temp668= Cyc_Absynpp_qvar2string( x); xprintf("Toc::add_varmap on Rel_n: %.*s\n",
_get_arr_size( _temp668, 1u), _temp668.curr);}))); _LL665: goto _LL661; _LL661:;}{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)(
ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=
0; ans->continue_lab= 0; return ans;} struct _tuple6{ struct _tagged_arr* f1;
struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_arr*
break_l, struct _tagged_arr* fallthru_l, struct Cyc_List_List* fallthru_binders,
struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List* _temp669= 0; for( 0;
fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*) _check_null(
fallthru_binders))->tl){ _temp669=({ struct Cyc_List_List* _temp670=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp670->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp670->tl=
_temp669; _temp670;});} _temp669=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp669);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp671=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp671->v=( void*) break_l; _temp671;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp672=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp672->v=( void*)({ struct _tuple6*
_temp673=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp673->f1=
fallthru_l; _temp673->f2= _temp669; _temp673->f3= next_case_env->varmap;
_temp673;}); _temp672;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp674=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp674->v=( void*)
break_l; _temp674;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_arr* next_l){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp675=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp675->v=( void*)({ struct _tuple6* _temp676=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp676->f1= next_l; _temp676->f2= 0;
_temp676->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp676;}); _temp675;});
return ans;} struct _tuple7{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tuple8{ struct _tuple0* f1; void* f2; struct Cyc_Absyn_Exp* f3; } ;
static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c( void* p, struct Cyc_Core_Opt*
aopt, struct Cyc_List_List* args, struct Cyc_List_List* typs){ if( args == 0){
return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("expecting a literal format string", sizeof( unsigned char), 34u));}{
struct _tagged_arr fmt_str;{ void* _temp677=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd)->r; void* _temp683; struct
_tagged_arr _temp685; _LL679: if(*(( int*) _temp677) == Cyc_Absyn_Const_e){
_LL684: _temp683=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp677)->f1; if((
unsigned int) _temp683 > 1u?*(( int*) _temp683) == Cyc_Absyn_String_c: 0){
_LL686: _temp685=(( struct Cyc_Absyn_String_c_struct*) _temp683)->f1; goto
_LL680;} else{ goto _LL681;}} else{ goto _LL681;} _LL681: goto _LL682; _LL680:
fmt_str= _temp685; goto _LL678; _LL682: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("expecting a literal format string",
sizeof( unsigned char), 34u)); _LL678:;}{ int len=( int) _get_arr_size( fmt_str,
sizeof( unsigned char)); int i= 0; for( i= 0; i < len; i ++){ if(*(( const
unsigned char*) _check_unknown_subscript( fmt_str, sizeof( unsigned char), i))
=='%'){ i ++; if( i < len?*(( const unsigned char*) _check_unknown_subscript(
fmt_str, sizeof( unsigned char), i)) =='s': 0){ break;}}} if( i == len){ if(
aopt != 0){ args=({ struct Cyc_List_List* _temp687=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp687->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp687->tl= args; _temp687;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp688= _new_region(); struct
_RegionHandle* r=& _temp688; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*)
_check_unknown_subscript( fmt_str, sizeof( unsigned char), i)); rev_fmt=({
struct Cyc_List_List* _temp689=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp689->hd=( void*)(( int) c); _temp689->tl=
rev_fmt; _temp689;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* _temp690=
Cyc_Formatstr_parse_conversionspecification( fmt_str, i + 1); if( _temp690 == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("bad format string",
sizeof( unsigned char), 18u));}{ struct _tuple7 _temp693; int _temp694;
unsigned char _temp696; struct Cyc_List_List* _temp698; struct Cyc_List_List*
_temp700; struct Cyc_List_List* _temp702; struct Cyc_List_List* _temp704; struct
_tuple7* _temp691=( struct _tuple7*)(( struct Cyc_Core_Opt*) _check_null(
_temp690))->v; _temp693=* _temp691; _LL705: _temp704= _temp693.f1; goto _LL703;
_LL703: _temp702= _temp693.f2; goto _LL701; _LL701: _temp700= _temp693.f3; goto
_LL699; _LL699: _temp698= _temp693.f4; goto _LL697; _LL697: _temp696= _temp693.f5;
goto _LL695; _LL695: _temp694= _temp693.f6; goto _LL692; _LL692: i= _temp694 - 1;
if( _temp696 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp706)( struct _RegionHandle*, struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_arr)) Cyc_List_rlist; struct _RegionHandle* _temp713= r; struct Cyc_List_List*
_temp709= _temp704; struct Cyc_List_List* _temp710= _temp702; struct Cyc_List_List*
_temp711= _temp700; struct Cyc_List_List* _temp712= _temp698; struct Cyc_List_List*
_temp707[ 4u]={ _temp709, _temp710, _temp711, _temp712}; struct _tagged_arr
_temp708={( void*) _temp707,( void*) _temp707,( void*)( _temp707 + 4u)};
_temp706( _temp713, _temp708);})), rev_fmt); rev_fmt=({ struct Cyc_List_List*
_temp714=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp714->hd=( void*)(( int) _temp696); _temp714->tl= rev_fmt; _temp714;});{
struct Cyc_List_List* _temp715= _temp702; struct Cyc_List_List _temp722; struct
Cyc_List_List* _temp723; int _temp725; _LL717: if( _temp715 == 0){ goto _LL719;}
else{ _temp722=* _temp715; _LL726: _temp725=( int) _temp722.hd; goto _LL724;
_LL724: _temp723= _temp722.tl; if( _temp723 == 0){ goto _LL721;} else{ goto
_LL719;}} _LL721: if( _temp725 =='*'){ goto _LL718;} else{ goto _LL719;} _LL719:
goto _LL720; _LL718: { struct _tuple0* _temp727= Cyc_Toc_temp_var(); rev_temps=({
struct Cyc_List_List* _temp728=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp728->hd=( void*)({ struct _tuple8* _temp729=(
struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8)); _temp729->f1=
_temp727; _temp729->f2=( void*)(( struct Cyc_List_List*) _check_null( typs))->hd;
_temp729->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd;
_temp729;}); _temp728->tl= rev_temps; _temp728;}); rev_result=({ struct Cyc_List_List*
_temp730=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp730->hd=( void*) Cyc_Absyn_var_exp( _temp727, 0); _temp730->tl= rev_result;
_temp730;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL716;} _LL720: goto
_LL716; _LL716:;}{ struct Cyc_List_List* _temp731= _temp700; struct Cyc_List_List
_temp738; struct Cyc_List_List* _temp739; struct Cyc_List_List _temp741; struct
Cyc_List_List* _temp742; int _temp744; int _temp746; _LL733: if( _temp731 == 0){
goto _LL735;} else{ _temp738=* _temp731; _LL747: _temp746=( int) _temp738.hd;
goto _LL740; _LL740: _temp739= _temp738.tl; if( _temp739 == 0){ goto _LL735;}
else{ _temp741=* _temp739; _LL745: _temp744=( int) _temp741.hd; goto _LL743;
_LL743: _temp742= _temp741.tl; if( _temp742 == 0){ goto _LL737;} else{ goto
_LL735;}}} _LL737: if( _temp746 =='.'? _temp744 =='*': 0){ goto _LL734;} else{
goto _LL735;} _LL735: goto _LL736; _LL734: { struct _tuple0* _temp748= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp749=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp749->hd=( void*)({
struct _tuple8* _temp750=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp750->f1= _temp748; _temp750->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp750->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp750;}); _temp749->tl= rev_temps; _temp749;});
rev_result=({ struct Cyc_List_List* _temp751=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp751->hd=( void*) Cyc_Absyn_var_exp(
_temp748, 0); _temp751->tl= rev_result; _temp751;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL732;} _LL736: goto _LL732; _LL732:;} if( _temp696 !='%'){ if( args == 0?
1: typs == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct
_tagged_arr)({ struct _tagged_arr _temp752= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_get_arr_size( _temp752, 1u), _temp752.curr);}));}{ struct _tuple0* _temp753=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp754=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp754->hd=( void*)({
struct _tuple8* _temp755=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp755->f1= _temp753; _temp755->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp755->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp755;}); _temp754->tl= rev_temps; _temp754;});
rev_result=({ struct Cyc_List_List* _temp756=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp756->hd=( void*) Cyc_Absyn_var_exp(
_temp753, 0); _temp756->tl= rev_result; _temp756;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp757=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp757->hd=( void*)(( int)'.');
_temp757->tl= rev_fmt; _temp757;}); rev_fmt=({ struct Cyc_List_List* _temp758=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp758->hd=( void*)(( int)'*'); _temp758->tl= rev_fmt; _temp758;}); rev_fmt=({
struct Cyc_List_List* _temp759=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp759->hd=( void*)(( int)'s'); _temp759->tl=
rev_fmt; _temp759;});{ struct _tuple0* _temp760= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp761= Cyc_Absyn_var_exp( _temp760, 0); rev_temps=({ struct Cyc_List_List*
_temp762=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp762->hd=( void*)({ struct _tuple8* _temp763=( struct _tuple8*)
_region_malloc( r, sizeof( struct _tuple8)); _temp763->f1= _temp760; _temp763->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp763->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp763;});
_temp762->tl= rev_temps; _temp762;});{ struct Cyc_Absyn_Exp* _temp764= Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp768)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp771= _temp761; struct Cyc_Absyn_Exp* _temp772= Cyc_Absyn_uint_exp( 1, 0);
struct Cyc_Absyn_Exp* _temp769[ 2u]={ _temp771, _temp772}; struct _tagged_arr
_temp770={( void*) _temp769,( void*) _temp769,( void*)( _temp769 + 2u)};
_temp768( _temp770);}), 0); struct Cyc_Absyn_Exp* _temp765= Cyc_Absyn_structmember_exp(
_temp761, Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List* _temp766=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp766->hd=( void*) _temp764; _temp766->tl= rev_result; _temp766;});
rev_result=({ struct Cyc_List_List* _temp767=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp767->hd=( void*)
_temp765; _temp767->tl= rev_result; _temp767;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}{
struct Cyc_List_List* _temp773=({ struct Cyc_List_List* _temp776=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp776->hd=( void*) Cyc_Absyn_string_exp((
struct _tagged_arr) Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_fmt)), 0); _temp776->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_result); _temp776;}); if( aopt != 0){
_temp773=({ struct Cyc_List_List* _temp774=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp774->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp774->tl= _temp773; _temp774;});}{
struct Cyc_Absyn_Exp* _temp775= Cyc_Absyn_primop_exp( p, _temp773, 0); s= Cyc_Absyn_exp_stmt(
_temp775, 0); for( 0; rev_temps != 0; rev_temps=(( struct Cyc_List_List*)
_check_null( rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( rev_temps))->hd)).f1,(*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( rev_temps))->hd)).f2,( struct Cyc_Absyn_Exp*)(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f3, s,
0);}}}}; _pop_region( r);} return Cyc_Absyn_stmt_exp( s, 0);}}}} static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e); static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s); static void* Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ", sizeof( unsigned char),
24u));} return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp*
e){ return({ struct _tuple3* _temp777=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp777->f1= Cyc_Toc_mt_tq; _temp777->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp777;});} static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp778=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp778->f1= 0; _temp778->f2= e; _temp778;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp779= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp779): Cyc_Toc_malloc_ptr( _temp779), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp779), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp780=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp780 != 0; _temp780=((
struct Cyc_List_List*) _check_null( _temp780))->tl){ struct _tuple4 _temp783;
struct Cyc_Absyn_Exp* _temp784; struct Cyc_List_List* _temp786; struct _tuple4*
_temp781=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp780))->hd;
_temp783=* _temp781; _LL787: _temp786= _temp783.f1; goto _LL785; _LL785:
_temp784= _temp783.f2; goto _LL782; _LL782: { struct Cyc_Absyn_Exp* e_index; if(
_temp786 == 0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp786))->tl != 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u));}{ void* _temp788=( void*)(( struct Cyc_List_List*)
_check_null( _temp786))->hd; void* _temp789= _temp788; struct Cyc_Absyn_Exp*
_temp795; struct _tagged_arr* _temp797; _LL791: if(*(( int*) _temp789) == Cyc_Absyn_ArrayElement){
_LL796: _temp795=(( struct Cyc_Absyn_ArrayElement_struct*) _temp789)->f1; goto
_LL792;} else{ goto _LL793;} _LL793: if(*(( int*) _temp789) == Cyc_Absyn_FieldName){
_LL798: _temp797=(( struct Cyc_Absyn_FieldName_struct*) _temp789)->f1; goto
_LL794;} else{ goto _LL790;} _LL792: Cyc_Toc_exp_to_c( nv, _temp795); e_index=
_temp795; goto _LL790; _LL794: e_index=(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("field name designators in array",
sizeof( unsigned char), 32u)); goto _LL790; _LL790:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp799=( void*)
_temp784->r; struct Cyc_List_List* _temp809; struct Cyc_Absyn_Exp* _temp811;
struct Cyc_Absyn_Exp* _temp813; struct Cyc_Absyn_Vardecl* _temp815; struct Cyc_List_List*
_temp817; void* _temp819; _LL801: if(*(( int*) _temp799) == Cyc_Absyn_Array_e){
_LL810: _temp809=(( struct Cyc_Absyn_Array_e_struct*) _temp799)->f1; goto _LL802;}
else{ goto _LL803;} _LL803: if(*(( int*) _temp799) == Cyc_Absyn_Comprehension_e){
_LL816: _temp815=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp799)->f1;
goto _LL814; _LL814: _temp813=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp799)->f2; goto _LL812; _LL812: _temp811=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp799)->f3; goto _LL804;} else{ goto _LL805;} _LL805: if(*(( int*) _temp799)
== Cyc_Absyn_AnonStruct_e){ _LL820: _temp819=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp799)->f1; goto _LL818; _LL818: _temp817=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp799)->f2; goto _LL806;} else{ goto _LL807;} _LL807: goto _LL808; _LL802: s=
Cyc_Toc_init_array( nv, lval, _temp809, s); goto _LL800; _LL804: s= Cyc_Toc_init_comprehension(
nv, lval, _temp815, _temp813, _temp811, s, 0); goto _LL800; _LL806: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp819, _temp817, s); goto _LL800; _LL808: Cyc_Toc_exp_to_c( nv,
_temp784); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp784, 0), s, 0); goto _LL800; _LL800:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp821= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp821, Cyc_Absyn_varb_exp( _temp821,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp844=( struct Cyc_Absyn_Local_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp844[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp845; _temp845.tag= Cyc_Absyn_Local_b; _temp845.f1= vd; _temp845;});
_temp844;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp821, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp821, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp821, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp821, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp822=( void*) e2->r; struct Cyc_List_List* _temp832; struct Cyc_Absyn_Exp*
_temp834; struct Cyc_Absyn_Exp* _temp836; struct Cyc_Absyn_Vardecl* _temp838;
struct Cyc_List_List* _temp840; void* _temp842; _LL824: if(*(( int*) _temp822)
== Cyc_Absyn_Array_e){ _LL833: _temp832=(( struct Cyc_Absyn_Array_e_struct*)
_temp822)->f1; goto _LL825;} else{ goto _LL826;} _LL826: if(*(( int*) _temp822)
== Cyc_Absyn_Comprehension_e){ _LL839: _temp838=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp822)->f1; goto _LL837; _LL837: _temp836=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp822)->f2; goto _LL835; _LL835: _temp834=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp822)->f3; goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp822)
== Cyc_Absyn_AnonStruct_e){ _LL843: _temp842=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp822)->f1; goto _LL841; _LL841: _temp840=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp822)->f2; goto _LL829;} else{ goto _LL830;} _LL830: goto _LL831; _LL825:
body= Cyc_Toc_init_array( nv2, lval, _temp832, Cyc_Absyn_skip_stmt( 0)); goto
_LL823; _LL827: body= Cyc_Toc_init_comprehension( nv2, lval, _temp838, _temp836,
_temp834, Cyc_Absyn_skip_stmt( 0), 0); goto _LL823; _LL829: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp842, _temp840, Cyc_Absyn_skip_stmt( 0)); goto _LL823; _LL831: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL823; _LL823:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp821, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp846=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp846 != 0; _temp846=(( struct Cyc_List_List*)
_check_null( _temp846))->tl){ struct _tuple4 _temp849; struct Cyc_Absyn_Exp*
_temp850; struct Cyc_List_List* _temp852; struct _tuple4* _temp847=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp846))->hd; _temp849=*
_temp847; _LL853: _temp852= _temp849.f1; goto _LL851; _LL851: _temp850= _temp849.f2;
goto _LL848; _LL848: if( _temp852 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("empty designator list", sizeof( unsigned char), 22u));} if((( struct
Cyc_List_List*) _check_null( _temp852))->tl != 0){(( int(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u));}{ void* _temp854=( void*)(( struct Cyc_List_List*)
_check_null( _temp852))->hd; struct _tagged_arr* _temp860; _LL856: if(*(( int*)
_temp854) == Cyc_Absyn_FieldName){ _LL861: _temp860=(( struct Cyc_Absyn_FieldName_struct*)
_temp854)->f1; goto _LL857;} else{ goto _LL858;} _LL858: goto _LL859; _LL857: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp860, 0);{ void*
_temp862=( void*) _temp850->r; struct Cyc_List_List* _temp872; struct Cyc_Absyn_Exp*
_temp874; struct Cyc_Absyn_Exp* _temp876; struct Cyc_Absyn_Vardecl* _temp878;
struct Cyc_List_List* _temp880; void* _temp882; _LL864: if(*(( int*) _temp862)
== Cyc_Absyn_Array_e){ _LL873: _temp872=(( struct Cyc_Absyn_Array_e_struct*)
_temp862)->f1; goto _LL865;} else{ goto _LL866;} _LL866: if(*(( int*) _temp862)
== Cyc_Absyn_Comprehension_e){ _LL879: _temp878=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp862)->f1; goto _LL877; _LL877: _temp876=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp862)->f2; goto _LL875; _LL875: _temp874=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp862)->f3; goto _LL867;} else{ goto _LL868;} _LL868: if(*(( int*) _temp862)
== Cyc_Absyn_AnonStruct_e){ _LL883: _temp882=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp862)->f1; goto _LL881; _LL881: _temp880=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp862)->f2; goto _LL869;} else{ goto _LL870;} _LL870: goto _LL871; _LL865: s=
Cyc_Toc_init_array( nv, lval, _temp872, s); goto _LL863; _LL867: s= Cyc_Toc_init_comprehension(
nv, lval, _temp878, _temp876, _temp874, s, 0); goto _LL863; _LL869: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp882, _temp880, s); goto _LL863; _LL871: Cyc_Toc_exp_to_c( nv,
_temp850); if( Cyc_Toc_is_poly_field( struct_type, _temp860)){ _temp850= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp850, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp850, 0), 0), s, 0); goto _LL863; _LL863:;} goto
_LL855;} _LL859:( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u))); _LL855:;}}}
return s;} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env*
nv, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
_RegionHandle _temp884= _new_region(); struct _RegionHandle* r=& _temp884;
_push_region( r);{ struct Cyc_List_List* _temp885=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c, es); struct _tagged_arr* _temp886= Cyc_Toc_add_tuple_type(
_temp885); struct _tuple0* _temp887= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp888= Cyc_Absyn_var_exp( _temp887, 0); struct Cyc_Absyn_Stmt* _temp889= Cyc_Absyn_exp_stmt(
_temp888, 0); struct Cyc_Absyn_Exp*(* _temp890)( struct Cyc_Absyn_Exp*, struct
_tagged_arr*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1; struct Cyc_List_List* _temp891=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)(
r, es);{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp891);
for( 0; _temp891 != 0;( _temp891=(( struct Cyc_List_List*) _check_null( _temp891))->tl,
-- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp891))->hd; struct Cyc_Absyn_Exp* lval= _temp890( _temp888, Cyc_Toc_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp892=( void*) e->r; struct Cyc_List_List*
_temp900; struct Cyc_Absyn_Exp* _temp902; struct Cyc_Absyn_Exp* _temp904; struct
Cyc_Absyn_Vardecl* _temp906; _LL894: if(*(( int*) _temp892) == Cyc_Absyn_Array_e){
_LL901: _temp900=(( struct Cyc_Absyn_Array_e_struct*) _temp892)->f1; goto _LL895;}
else{ goto _LL896;} _LL896: if(*(( int*) _temp892) == Cyc_Absyn_Comprehension_e){
_LL907: _temp906=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp892)->f1;
goto _LL905; _LL905: _temp904=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp892)->f2; goto _LL903; _LL903: _temp902=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp892)->f3; goto _LL897;} else{ goto _LL898;} _LL898: goto _LL899; _LL895:
_temp889= Cyc_Toc_init_array( nv, lval, _temp900, _temp889); goto _LL893; _LL897:
_temp889= Cyc_Toc_init_comprehension( nv, lval, _temp906, _temp904, _temp902,
_temp889, 0); goto _LL893; _LL899: Cyc_Toc_exp_to_c( nv, e); _temp889= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp890( _temp888, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp889, 0); goto _LL893; _LL893:;}}}{ struct Cyc_Absyn_Exp*
_temp908= Cyc_Toc_make_struct( nv, _temp887, Cyc_Absyn_strct( _temp886),
_temp889, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp908;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp909= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp910= Cyc_Absyn_var_exp( _temp909, 0); struct Cyc_Absyn_Stmt*
_temp911= Cyc_Absyn_exp_stmt( _temp910, 0); struct Cyc_Absyn_Exp*(* _temp912)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp913= _new_region(); struct _RegionHandle* r=& _temp913;
_push_region( r);{ struct Cyc_List_List* _temp914=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp914 != 0; _temp914=(( struct Cyc_List_List*) _check_null( _temp914))->tl){
struct _tuple4 _temp917; struct Cyc_Absyn_Exp* _temp918; struct Cyc_List_List*
_temp920; struct _tuple4* _temp915=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp914))->hd; _temp917=* _temp915; _LL921: _temp920= _temp917.f1;
goto _LL919; _LL919: _temp918= _temp917.f2; goto _LL916; _LL916: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp918->topt))->v): 0; if( _temp920 == 0){(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("empty designator list", sizeof( unsigned char), 22u));}
if((( struct Cyc_List_List*) _check_null( _temp920))->tl != 0){ struct _tuple0*
_temp922= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp923= Cyc_Absyn_var_exp(
_temp922, 0); for( 0; _temp920 != 0; _temp920=(( struct Cyc_List_List*)
_check_null( _temp920))->tl){ void* _temp924=( void*)(( struct Cyc_List_List*)
_check_null( _temp920))->hd; struct _tagged_arr* _temp930; _LL926: if(*(( int*)
_temp924) == Cyc_Absyn_FieldName){ _LL931: _temp930=(( struct Cyc_Absyn_FieldName_struct*)
_temp924)->f1; goto _LL927;} else{ goto _LL928;} _LL928: goto _LL929; _LL927:
if( Cyc_Toc_is_poly_field( struct_type, _temp930)){ _temp923= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp923, 0);} _temp911= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp912( _temp910, _temp930, 0), _temp923, 0), 0),
_temp911, 0); goto _LL925; _LL929:( int) _throw((( void*(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)( _tag_arr("array designator in struct", sizeof(
unsigned char), 27u))); _LL925:;} Cyc_Toc_exp_to_c( nv, _temp918); _temp911= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp923, _temp918, 0), 0), _temp911,
0);} else{ void* _temp932=( void*)(( struct Cyc_List_List*) _check_null(
_temp920))->hd; struct _tagged_arr* _temp938; _LL934: if(*(( int*) _temp932) ==
Cyc_Absyn_FieldName){ _LL939: _temp938=(( struct Cyc_Absyn_FieldName_struct*)
_temp932)->f1; goto _LL935;} else{ goto _LL936;} _LL936: goto _LL937; _LL935: {
struct Cyc_Absyn_Exp* lval= _temp912( _temp910, _temp938, 0);{ void* _temp940=(
void*) _temp918->r; struct Cyc_List_List* _temp950; struct Cyc_Absyn_Exp*
_temp952; struct Cyc_Absyn_Exp* _temp954; struct Cyc_Absyn_Vardecl* _temp956;
struct Cyc_List_List* _temp958; void* _temp960; _LL942: if(*(( int*) _temp940)
== Cyc_Absyn_Array_e){ _LL951: _temp950=(( struct Cyc_Absyn_Array_e_struct*)
_temp940)->f1; goto _LL943;} else{ goto _LL944;} _LL944: if(*(( int*) _temp940)
== Cyc_Absyn_Comprehension_e){ _LL957: _temp956=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp940)->f1; goto _LL955; _LL955: _temp954=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp940)->f2; goto _LL953; _LL953: _temp952=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp940)->f3; goto _LL945;} else{ goto _LL946;} _LL946: if(*(( int*) _temp940)
== Cyc_Absyn_AnonStruct_e){ _LL961: _temp960=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp940)->f1; goto _LL959; _LL959: _temp958=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp940)->f2; goto _LL947;} else{ goto _LL948;} _LL948: goto _LL949; _LL943:
_temp911= Cyc_Toc_init_array( nv, lval, _temp950, _temp911); goto _LL941; _LL945:
_temp911= Cyc_Toc_init_comprehension( nv, lval, _temp956, _temp954, _temp952,
_temp911, 0); goto _LL941; _LL947: _temp911= Cyc_Toc_init_anon_struct( nv, lval,
_temp960, _temp958, _temp911); goto _LL941; _LL949: Cyc_Toc_exp_to_c( nv,
_temp918); if( Cyc_Toc_is_poly_field( struct_type, _temp938)){ _temp918= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp918, 0);} _temp911= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp918, 0), 0), _temp911, 0); goto _LL941; _LL941:;}
goto _LL933;} _LL937:( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u))); _LL933:;}}}{
struct Cyc_Absyn_Exp* _temp962= Cyc_Toc_make_struct( nv, _temp909, Cyc_Absyn_strctq(
tdn), _temp911, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp962;};
_pop_region( r);} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp*
f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp*
el, struct _tuple9* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2,
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp*
e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp963=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp963[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp964; _temp964.tag= Cyc_Absyn_Increment_e;
_temp964.f1= e1; _temp964.f2=( void*) incr; _temp964;}); _temp963;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp965=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp975; struct Cyc_Absyn_Exp*
_temp977; void* _temp979; struct _tagged_arr* _temp981; struct Cyc_Absyn_Exp*
_temp983; _LL967: if(*(( int*) _temp965) == Cyc_Absyn_StmtExp_e){ _LL976:
_temp975=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp965)->f1; goto _LL968;}
else{ goto _LL969;} _LL969: if(*(( int*) _temp965) == Cyc_Absyn_Cast_e){ _LL980:
_temp979=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp965)->f1; goto _LL978;
_LL978: _temp977=(( struct Cyc_Absyn_Cast_e_struct*) _temp965)->f2; goto _LL970;}
else{ goto _LL971;} _LL971: if(*(( int*) _temp965) == Cyc_Absyn_StructMember_e){
_LL984: _temp983=(( struct Cyc_Absyn_StructMember_e_struct*) _temp965)->f1; goto
_LL982; _LL982: _temp981=(( struct Cyc_Absyn_StructMember_e_struct*) _temp965)->f2;
goto _LL972;} else{ goto _LL973;} _LL973: goto _LL974; _LL968:(( void(*)( struct
Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp975, fs,
f, f_env); goto _LL966; _LL970:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp977, fs, f, f_env); goto _LL966; _LL972:( void*)( e1->r=( void*)(( void*)
_temp983->r));(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
e1,({ struct Cyc_List_List* _temp985=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp985->hd=( void*) _temp981; _temp985->tl= fs;
_temp985;}), f, f_env); goto _LL966; _LL974: if( ! Cyc_Absyn_is_lvalue( e1)){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp986= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_get_arr_size( _temp986, 1u), _temp986.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL966;} _LL966:;}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp987=( void*) s->r; struct Cyc_Absyn_Exp* _temp997; struct Cyc_Absyn_Stmt*
_temp999; struct Cyc_Absyn_Decl* _temp1001; struct Cyc_Absyn_Stmt* _temp1003;
struct Cyc_Absyn_Stmt* _temp1005; _LL989: if(( unsigned int) _temp987 > 1u?*((
int*) _temp987) == Cyc_Absyn_Exp_s: 0){ _LL998: _temp997=(( struct Cyc_Absyn_Exp_s_struct*)
_temp987)->f1; goto _LL990;} else{ goto _LL991;} _LL991: if(( unsigned int)
_temp987 > 1u?*(( int*) _temp987) == Cyc_Absyn_Decl_s: 0){ _LL1002: _temp1001=((
struct Cyc_Absyn_Decl_s_struct*) _temp987)->f1; goto _LL1000; _LL1000: _temp999=((
struct Cyc_Absyn_Decl_s_struct*) _temp987)->f2; goto _LL992;} else{ goto _LL993;}
_LL993: if(( unsigned int) _temp987 > 1u?*(( int*) _temp987) == Cyc_Absyn_Seq_s:
0){ _LL1006: _temp1005=(( struct Cyc_Absyn_Seq_s_struct*) _temp987)->f1; goto
_LL1004; _LL1004: _temp1003=(( struct Cyc_Absyn_Seq_s_struct*) _temp987)->f2;
goto _LL994;} else{ goto _LL995;} _LL995: goto _LL996; _LL990:(( void(*)( struct
Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp997, fs, f,
f_env); goto _LL988; _LL992:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp999, fs, f, f_env); goto _LL988; _LL994:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1003, fs, f, f_env);
goto _LL988; _LL996:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1007= Cyc_Absynpp_stmt2string( s);
xprintf("lvalue_assign_stmt: %.*s", _get_arr_size( _temp1007, 1u), _temp1007.curr);}));
goto _LL988; _LL988:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){ return 0;}
result=({ struct Cyc_List_List* _temp1008=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1008->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1008->tl= 0;
_temp1008;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp1009=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1009->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1009->tl= 0;
_temp1009;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x))
Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1010=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1010->f1= 0; _temp1010->f2= e;
_temp1010;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){
void* _temp1011= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp1017;
_LL1013: if(( unsigned int) _temp1011 > 4u?*(( int*) _temp1011) == Cyc_Absyn_PointerType:
0){ _LL1018: _temp1017=(( struct Cyc_Absyn_PointerType_struct*) _temp1011)->f1;
goto _LL1014;} else{ goto _LL1015;} _LL1015: goto _LL1016; _LL1014: return
_temp1017; _LL1016:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("get_ptr_typ:  not a pointer!", sizeof( unsigned char), 29u))); _LL1012:;}
struct _tuple10{ void* f1; void* f2; } ; struct _tuple11{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp1019=( void*) e->r; if( e->topt == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp1020= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_get_arr_size( _temp1020, 1u), _temp1020.curr);})); return;}{ void* old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void* _temp1021=
_temp1019; void* _temp1101; void* _temp1103; void* _temp1105; struct _tuple0*
_temp1107; struct _tuple0* _temp1109; struct Cyc_List_List* _temp1111; void*
_temp1113; void* _temp1115; struct Cyc_Absyn_Exp* _temp1117; struct Cyc_Absyn_Exp*
_temp1119; struct Cyc_Core_Opt* _temp1121; struct Cyc_Absyn_Exp* _temp1123;
struct Cyc_Absyn_Exp* _temp1125; struct Cyc_Absyn_Exp* _temp1127; struct Cyc_Absyn_Exp*
_temp1129; struct Cyc_Absyn_Exp* _temp1131; struct Cyc_Absyn_Exp* _temp1133;
struct Cyc_List_List* _temp1135; struct Cyc_Absyn_Exp* _temp1137; struct Cyc_Absyn_VarargCallInfo*
_temp1139; struct Cyc_List_List* _temp1141; struct Cyc_Absyn_Exp* _temp1143;
struct Cyc_Absyn_VarargCallInfo* _temp1145; struct Cyc_Absyn_VarargCallInfo
_temp1147; struct Cyc_Absyn_VarargInfo* _temp1148; struct Cyc_List_List*
_temp1150; int _temp1152; struct Cyc_List_List* _temp1154; struct Cyc_Absyn_Exp*
_temp1156; struct Cyc_Absyn_Exp* _temp1158; struct Cyc_Absyn_Exp* _temp1160;
struct Cyc_List_List* _temp1162; struct Cyc_Absyn_Exp* _temp1164; struct Cyc_Absyn_Exp*
_temp1166; void* _temp1168; void** _temp1170; struct Cyc_Absyn_Exp* _temp1171;
struct Cyc_Absyn_Exp* _temp1173; struct Cyc_Absyn_Exp* _temp1175; struct Cyc_Absyn_Exp*
_temp1177; void* _temp1179; struct _tagged_arr* _temp1181; void* _temp1183;
struct Cyc_Absyn_Exp* _temp1185; struct _tagged_arr* _temp1187; struct Cyc_Absyn_Exp*
_temp1189; struct _tagged_arr* _temp1191; struct Cyc_Absyn_Exp* _temp1193;
struct Cyc_Absyn_Exp* _temp1195; struct Cyc_Absyn_Exp* _temp1197; struct Cyc_List_List*
_temp1199; struct Cyc_List_List* _temp1201; struct Cyc_Absyn_Exp* _temp1203;
struct Cyc_Absyn_Exp* _temp1205; struct Cyc_Absyn_Vardecl* _temp1207; struct Cyc_Absyn_Structdecl*
_temp1209; struct Cyc_List_List* _temp1211; struct Cyc_Core_Opt* _temp1213;
struct _tuple0* _temp1215; struct Cyc_List_List* _temp1217; void* _temp1219;
struct Cyc_Absyn_Tunionfield* _temp1221; struct Cyc_Absyn_Tuniondecl* _temp1223;
struct Cyc_List_List* _temp1225; struct Cyc_Core_Opt* _temp1227; struct Cyc_Core_Opt*
_temp1229; struct Cyc_Absyn_Tunionfield* _temp1231; struct Cyc_Absyn_Tuniondecl*
_temp1233; struct Cyc_List_List* _temp1235; struct Cyc_Core_Opt* _temp1237;
struct Cyc_Core_Opt* _temp1239; struct Cyc_Absyn_Enumfield* _temp1241; struct
Cyc_Absyn_Enumdecl* _temp1243; struct _tuple0* _temp1245; void* _temp1247;
struct Cyc_Absyn_Exp* _temp1249; struct Cyc_Absyn_Stmt* _temp1251; struct Cyc_List_List*
_temp1253; struct Cyc_Core_Opt* _temp1255; struct Cyc_List_List* _temp1257;
struct _tuple1* _temp1259; struct Cyc_Absyn_Fndecl* _temp1261; struct Cyc_Absyn_Exp*
_temp1263; _LL1023: if(*(( int*) _temp1021) == Cyc_Absyn_Const_e){ _LL1102:
_temp1101=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1021)->f1; if(
_temp1101 ==( void*) Cyc_Absyn_Null_c){ goto _LL1024;} else{ goto _LL1025;}}
else{ goto _LL1025;} _LL1025: if(*(( int*) _temp1021) == Cyc_Absyn_Const_e){
_LL1104: _temp1103=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1021)->f1;
goto _LL1026;} else{ goto _LL1027;} _LL1027: if(*(( int*) _temp1021) == Cyc_Absyn_Var_e){
_LL1108: _temp1107=(( struct Cyc_Absyn_Var_e_struct*) _temp1021)->f1; goto
_LL1106; _LL1106: _temp1105=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1021)->f2;
goto _LL1028;} else{ goto _LL1029;} _LL1029: if(*(( int*) _temp1021) == Cyc_Absyn_UnknownId_e){
_LL1110: _temp1109=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1021)->f1; goto
_LL1030;} else{ goto _LL1031;} _LL1031: if(*(( int*) _temp1021) == Cyc_Absyn_Primop_e){
_LL1114: _temp1113=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1021)->f1;
goto _LL1112; _LL1112: _temp1111=(( struct Cyc_Absyn_Primop_e_struct*) _temp1021)->f2;
goto _LL1032;} else{ goto _LL1033;} _LL1033: if(*(( int*) _temp1021) == Cyc_Absyn_Increment_e){
_LL1118: _temp1117=(( struct Cyc_Absyn_Increment_e_struct*) _temp1021)->f1; goto
_LL1116; _LL1116: _temp1115=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1021)->f2; goto _LL1034;} else{ goto _LL1035;} _LL1035: if(*(( int*)
_temp1021) == Cyc_Absyn_AssignOp_e){ _LL1124: _temp1123=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1021)->f1; goto _LL1122; _LL1122: _temp1121=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1021)->f2; goto _LL1120; _LL1120: _temp1119=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1021)->f3; goto _LL1036;} else{ goto _LL1037;} _LL1037: if(*(( int*)
_temp1021) == Cyc_Absyn_Conditional_e){ _LL1130: _temp1129=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1021)->f1; goto _LL1128; _LL1128: _temp1127=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1021)->f2; goto _LL1126; _LL1126: _temp1125=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1021)->f3; goto _LL1038;} else{ goto _LL1039;} _LL1039: if(*(( int*)
_temp1021) == Cyc_Absyn_SeqExp_e){ _LL1134: _temp1133=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1021)->f1; goto _LL1132; _LL1132: _temp1131=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1021)->f2; goto _LL1040;} else{ goto _LL1041;} _LL1041: if(*(( int*)
_temp1021) == Cyc_Absyn_UnknownCall_e){ _LL1138: _temp1137=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1021)->f1; goto _LL1136; _LL1136: _temp1135=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1021)->f2; goto _LL1042;} else{ goto _LL1043;} _LL1043: if(*(( int*)
_temp1021) == Cyc_Absyn_FnCall_e){ _LL1144: _temp1143=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1021)->f1; goto _LL1142; _LL1142: _temp1141=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1021)->f2; goto _LL1140; _LL1140: _temp1139=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1021)->f3; if( _temp1139 == 0){ goto _LL1044;} else{ goto _LL1045;}} else{
goto _LL1045;} _LL1045: if(*(( int*) _temp1021) == Cyc_Absyn_FnCall_e){ _LL1157:
_temp1156=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1021)->f1; goto _LL1155;
_LL1155: _temp1154=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1021)->f2; goto
_LL1146; _LL1146: _temp1145=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1021)->f3;
if( _temp1145 == 0){ goto _LL1047;} else{ _temp1147=* _temp1145; _LL1153:
_temp1152= _temp1147.num_varargs; goto _LL1151; _LL1151: _temp1150= _temp1147.injectors;
goto _LL1149; _LL1149: _temp1148= _temp1147.vai; goto _LL1046;}} else{ goto
_LL1047;} _LL1047: if(*(( int*) _temp1021) == Cyc_Absyn_Throw_e){ _LL1159:
_temp1158=(( struct Cyc_Absyn_Throw_e_struct*) _temp1021)->f1; goto _LL1048;}
else{ goto _LL1049;} _LL1049: if(*(( int*) _temp1021) == Cyc_Absyn_NoInstantiate_e){
_LL1161: _temp1160=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1021)->f1;
goto _LL1050;} else{ goto _LL1051;} _LL1051: if(*(( int*) _temp1021) == Cyc_Absyn_Instantiate_e){
_LL1165: _temp1164=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1021)->f1;
goto _LL1163; _LL1163: _temp1162=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1021)->f2; goto _LL1052;} else{ goto _LL1053;} _LL1053: if(*(( int*)
_temp1021) == Cyc_Absyn_Cast_e){ _LL1169: _temp1168=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1021)->f1; _temp1170=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1021)->f1); goto _LL1167; _LL1167: _temp1166=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1021)->f2; goto _LL1054;} else{ goto _LL1055;} _LL1055: if(*(( int*)
_temp1021) == Cyc_Absyn_Address_e){ _LL1172: _temp1171=(( struct Cyc_Absyn_Address_e_struct*)
_temp1021)->f1; goto _LL1056;} else{ goto _LL1057;} _LL1057: if(*(( int*)
_temp1021) == Cyc_Absyn_New_e){ _LL1176: _temp1175=(( struct Cyc_Absyn_New_e_struct*)
_temp1021)->f1; goto _LL1174; _LL1174: _temp1173=(( struct Cyc_Absyn_New_e_struct*)
_temp1021)->f2; goto _LL1058;} else{ goto _LL1059;} _LL1059: if(*(( int*)
_temp1021) == Cyc_Absyn_Sizeofexp_e){ _LL1178: _temp1177=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1021)->f1; goto _LL1060;} else{ goto _LL1061;} _LL1061: if(*(( int*)
_temp1021) == Cyc_Absyn_Sizeoftyp_e){ _LL1180: _temp1179=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1021)->f1; goto _LL1062;} else{ goto _LL1063;} _LL1063: if(*(( int*)
_temp1021) == Cyc_Absyn_Offsetof_e){ _LL1184: _temp1183=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1021)->f1; goto _LL1182; _LL1182: _temp1181=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1021)->f2; goto _LL1064;} else{ goto _LL1065;} _LL1065: if(*(( int*)
_temp1021) == Cyc_Absyn_Deref_e){ _LL1186: _temp1185=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1021)->f1; goto _LL1066;} else{ goto _LL1067;} _LL1067: if(*(( int*)
_temp1021) == Cyc_Absyn_StructMember_e){ _LL1190: _temp1189=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1021)->f1; goto _LL1188; _LL1188: _temp1187=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1021)->f2; goto _LL1068;} else{ goto _LL1069;} _LL1069: if(*(( int*)
_temp1021) == Cyc_Absyn_StructArrow_e){ _LL1194: _temp1193=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1021)->f1; goto _LL1192; _LL1192: _temp1191=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1021)->f2; goto _LL1070;} else{ goto _LL1071;} _LL1071: if(*(( int*)
_temp1021) == Cyc_Absyn_Subscript_e){ _LL1198: _temp1197=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1021)->f1; goto _LL1196; _LL1196: _temp1195=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1021)->f2; goto _LL1072;} else{ goto _LL1073;} _LL1073: if(*(( int*)
_temp1021) == Cyc_Absyn_Tuple_e){ _LL1200: _temp1199=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1021)->f1; goto _LL1074;} else{ goto _LL1075;} _LL1075: if(*(( int*)
_temp1021) == Cyc_Absyn_Array_e){ _LL1202: _temp1201=(( struct Cyc_Absyn_Array_e_struct*)
_temp1021)->f1; goto _LL1076;} else{ goto _LL1077;} _LL1077: if(*(( int*)
_temp1021) == Cyc_Absyn_Comprehension_e){ _LL1208: _temp1207=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1021)->f1; goto _LL1206; _LL1206: _temp1205=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1021)->f2; goto _LL1204; _LL1204: _temp1203=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1021)->f3; goto _LL1078;} else{ goto _LL1079;} _LL1079: if(*(( int*)
_temp1021) == Cyc_Absyn_Struct_e){ _LL1216: _temp1215=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1021)->f1; goto _LL1214; _LL1214: _temp1213=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1021)->f2; goto _LL1212; _LL1212: _temp1211=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1021)->f3; goto _LL1210; _LL1210: _temp1209=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1021)->f4; goto _LL1080;} else{ goto _LL1081;} _LL1081: if(*(( int*)
_temp1021) == Cyc_Absyn_AnonStruct_e){ _LL1220: _temp1219=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1021)->f1; goto _LL1218; _LL1218: _temp1217=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1021)->f2; goto _LL1082;} else{ goto _LL1083;} _LL1083: if(*(( int*)
_temp1021) == Cyc_Absyn_Tunion_e){ _LL1230: _temp1229=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1021)->f1; goto _LL1228; _LL1228: _temp1227=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1021)->f2; goto _LL1226; _LL1226: _temp1225=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1021)->f3; if( _temp1225 == 0){ goto _LL1224;} else{ goto _LL1085;} _LL1224:
_temp1223=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1021)->f4; goto _LL1222;
_LL1222: _temp1221=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1021)->f5; goto
_LL1084;} else{ goto _LL1085;} _LL1085: if(*(( int*) _temp1021) == Cyc_Absyn_Tunion_e){
_LL1240: _temp1239=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1021)->f1; goto
_LL1238; _LL1238: _temp1237=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1021)->f2;
goto _LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1021)->f3;
goto _LL1234; _LL1234: _temp1233=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1021)->f4;
goto _LL1232; _LL1232: _temp1231=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1021)->f5;
goto _LL1086;} else{ goto _LL1087;} _LL1087: if(*(( int*) _temp1021) == Cyc_Absyn_Enum_e){
_LL1246: _temp1245=(( struct Cyc_Absyn_Enum_e_struct*) _temp1021)->f1; goto
_LL1244; _LL1244: _temp1243=(( struct Cyc_Absyn_Enum_e_struct*) _temp1021)->f2;
goto _LL1242; _LL1242: _temp1241=(( struct Cyc_Absyn_Enum_e_struct*) _temp1021)->f3;
goto _LL1088;} else{ goto _LL1089;} _LL1089: if(*(( int*) _temp1021) == Cyc_Absyn_Malloc_e){
_LL1250: _temp1249=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1021)->f1; goto
_LL1248; _LL1248: _temp1247=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1021)->f2; goto _LL1090;} else{ goto _LL1091;} _LL1091: if(*(( int*)
_temp1021) == Cyc_Absyn_StmtExp_e){ _LL1252: _temp1251=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1021)->f1; goto _LL1092;} else{ goto _LL1093;} _LL1093: if(*(( int*)
_temp1021) == Cyc_Absyn_UnresolvedMem_e){ _LL1256: _temp1255=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1021)->f1; goto _LL1254; _LL1254: _temp1253=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1021)->f2; goto _LL1094;} else{ goto _LL1095;} _LL1095: if(*(( int*)
_temp1021) == Cyc_Absyn_CompoundLit_e){ _LL1260: _temp1259=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1021)->f1; goto _LL1258; _LL1258: _temp1257=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1021)->f2; goto _LL1096;} else{ goto _LL1097;} _LL1097: if(*(( int*)
_temp1021) == Cyc_Absyn_Codegen_e){ _LL1262: _temp1261=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1021)->f1; goto _LL1098;} else{ goto _LL1099;} _LL1099: if(*(( int*)
_temp1021) == Cyc_Absyn_Fill_e){ _LL1264: _temp1263=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1021)->f1; goto _LL1100;} else{ goto _LL1022;} _LL1024: { struct Cyc_Absyn_Exp*
_temp1265= Cyc_Absyn_uint_exp( 0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ if( nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr(
old_typ, _temp1265, _temp1265))->r));} else{( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1266)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1269= _temp1265; struct Cyc_Absyn_Exp* _temp1270=
_temp1265; struct Cyc_Absyn_Exp* _temp1271= _temp1265; struct Cyc_Absyn_Exp*
_temp1267[ 3u]={ _temp1269, _temp1270, _temp1271}; struct _tagged_arr _temp1268={(
void*) _temp1267,( void*) _temp1267,( void*)( _temp1267 + 3u)}; _temp1266(
_temp1268);}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL1022;} _LL1026: goto _LL1022; _LL1028:{ struct
_handler_cons _temp1272; _push_handler(& _temp1272);{ int _temp1274= 0; if(
setjmp( _temp1272.handler)){ _temp1274= 1;} if( ! _temp1274){( void*)( e->r=(
void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key)) Cyc_Dict_lookup)( nv->varmap, _temp1107))->r));; _pop_handler();}
else{ void* _temp1273=( void*) _exn_thrown; void* _temp1276= _temp1273; _LL1278:
if( _temp1276 == Cyc_Dict_Absent){ goto _LL1279;} else{ goto _LL1280;} _LL1280:
goto _LL1281; _LL1279:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1282= Cyc_Absynpp_qvar2string(
_temp1107); xprintf("Can't find %.*s in exp_to_c, Var\n", _get_arr_size(
_temp1282, 1u), _temp1282.curr);})); return; _LL1281:( void) _throw( _temp1276);
_LL1277:;}}} goto _LL1022; _LL1030:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("unknownid", sizeof( unsigned char), 10u)); goto _LL1022; _LL1032: {
struct Cyc_List_List* _temp1283=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1111); struct Cyc_List_List* _temp1284=(( struct Cyc_List_List*(*)( void*(*
f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1111);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1111);{ void* _temp1285= _temp1113; _LL1287: if( _temp1285 ==( void*)
Cyc_Absyn_Size){ goto _LL1288;} else{ goto _LL1289;} _LL1289: if( _temp1285 ==(
void*) Cyc_Absyn_Printf){ goto _LL1290;} else{ goto _LL1291;} _LL1291: if(
_temp1285 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1292;} else{ goto _LL1293;}
_LL1293: if( _temp1285 ==( void*) Cyc_Absyn_Fprintf){ goto _LL1294;} else{ goto
_LL1295;} _LL1295: if( _temp1285 ==( void*) Cyc_Absyn_Sscanf){ goto _LL1296;}
else{ goto _LL1297;} _LL1297: if( _temp1285 ==( void*) Cyc_Absyn_Plus){ goto
_LL1298;} else{ goto _LL1299;} _LL1299: if( _temp1285 ==( void*) Cyc_Absyn_Minus){
goto _LL1300;} else{ goto _LL1301;} _LL1301: if( _temp1285 ==( void*) Cyc_Absyn_Eq){
goto _LL1302;} else{ goto _LL1303;} _LL1303: if( _temp1285 ==( void*) Cyc_Absyn_Neq){
goto _LL1304;} else{ goto _LL1305;} _LL1305: if( _temp1285 ==( void*) Cyc_Absyn_Gt){
goto _LL1306;} else{ goto _LL1307;} _LL1307: if( _temp1285 ==( void*) Cyc_Absyn_Gte){
goto _LL1308;} else{ goto _LL1309;} _LL1309: if( _temp1285 ==( void*) Cyc_Absyn_Lt){
goto _LL1310;} else{ goto _LL1311;} _LL1311: if( _temp1285 ==( void*) Cyc_Absyn_Lte){
goto _LL1312;} else{ goto _LL1313;} _LL1313: goto _LL1314; _LL1288: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1111))->hd;{
void* _temp1315=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1323; struct Cyc_Absyn_Tqual _temp1325; void*
_temp1327; struct Cyc_Absyn_PtrInfo _temp1329; struct Cyc_Absyn_Conref*
_temp1331; struct Cyc_Absyn_Tqual _temp1333; struct Cyc_Absyn_Conref* _temp1335;
void* _temp1337; void* _temp1339; _LL1317: if(( unsigned int) _temp1315 > 4u?*((
int*) _temp1315) == Cyc_Absyn_ArrayType: 0){ _LL1328: _temp1327=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1315)->f1; goto _LL1326; _LL1326:
_temp1325=(( struct Cyc_Absyn_ArrayType_struct*) _temp1315)->f2; goto _LL1324;
_LL1324: _temp1323=(( struct Cyc_Absyn_ArrayType_struct*) _temp1315)->f3; goto
_LL1318;} else{ goto _LL1319;} _LL1319: if(( unsigned int) _temp1315 > 4u?*((
int*) _temp1315) == Cyc_Absyn_PointerType: 0){ _LL1330: _temp1329=(( struct Cyc_Absyn_PointerType_struct*)
_temp1315)->f1; _LL1340: _temp1339=( void*) _temp1329.elt_typ; goto _LL1338;
_LL1338: _temp1337=( void*) _temp1329.rgn_typ; goto _LL1336; _LL1336: _temp1335=
_temp1329.nullable; goto _LL1334; _LL1334: _temp1333= _temp1329.tq; goto _LL1332;
_LL1332: _temp1331= _temp1329.bounds; goto _LL1320;} else{ goto _LL1321;}
_LL1321: goto _LL1322; _LL1318:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1323))->r)); goto _LL1316; _LL1320:{ void* _temp1341=(( void*(*)(
void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1331); struct Cyc_Absyn_Exp* _temp1347; _LL1343: if( _temp1341 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1344;} else{ goto _LL1345;} _LL1345: if((
unsigned int) _temp1341 > 1u?*(( int*) _temp1341) == Cyc_Absyn_Upper_b: 0){
_LL1348: _temp1347=(( struct Cyc_Absyn_Upper_b_struct*) _temp1341)->f1; goto
_LL1346;} else{ goto _LL1342;} _LL1344:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp1349)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1352=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1111))->hd; struct Cyc_Absyn_Exp* _temp1353= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( _temp1339), 0); struct Cyc_Absyn_Exp* _temp1350[ 2u]={
_temp1352, _temp1353}; struct _tagged_arr _temp1351={( void*) _temp1350,( void*)
_temp1350,( void*)( _temp1350 + 2u)}; _temp1349( _temp1351);}), 0))->r)); goto
_LL1342; _LL1346:( void*)( e->r=( void*)(( void*) _temp1347->r)); goto _LL1342;
_LL1342:;} goto _LL1316; _LL1322:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1354= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_List_List*) _check_null( _temp1284))->hd); struct
_tagged_arr _temp1355= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_get_arr_size( _temp1354, 1u), _temp1354.curr, _get_arr_size( _temp1355, 1u),
_temp1355.curr);})); return; _LL1316:;} goto _LL1286;} _LL1290:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1113, 0, _temp1111, _temp1284))->r));
goto _LL1286; _LL1292:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1113, 0, _temp1111, _temp1284))->r)); goto _LL1286; _LL1294: if( _temp1111
== 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("fprintf without arguments",
sizeof( unsigned char), 26u));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1113,({ struct Cyc_Core_Opt* _temp1356=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1356->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1111))->hd); _temp1356;}),(( struct Cyc_List_List*)
_check_null( _temp1111))->tl,(( struct Cyc_List_List*) _check_null( _temp1284))->tl))->r));
goto _LL1286; _LL1296: if( _temp1111 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("sscanf without arguments", sizeof( unsigned char), 25u));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1111))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1111))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1111))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1286; _LL1298: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1283))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1111))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1111))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1357)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1360= e1; struct Cyc_Absyn_Exp* _temp1361= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1362= e2; struct Cyc_Absyn_Exp*
_temp1358[ 3u]={ _temp1360, _temp1361, _temp1362}; struct _tagged_arr _temp1359={(
void*) _temp1358,( void*) _temp1358,( void*)( _temp1358 + 3u)}; _temp1357(
_temp1359);}), 0))->r));} goto _LL1286;} _LL1300: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1283))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1111))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1111))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1283))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1363)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1366= e1; struct Cyc_Absyn_Exp* _temp1367= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1368= Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Minus, e2, 0); struct Cyc_Absyn_Exp* _temp1364[ 3u]={ _temp1366,
_temp1367, _temp1368}; struct _tagged_arr _temp1365={( void*) _temp1364,( void*)
_temp1364,( void*)( _temp1364 + 3u)}; _temp1363( _temp1365);}), 0))->r));}} goto
_LL1286;} _LL1302: goto _LL1304; _LL1304: goto _LL1306; _LL1306: goto _LL1308;
_LL1308: goto _LL1310; _LL1310: goto _LL1312; _LL1312: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1111))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1111))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1283))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1283))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1286;} _LL1314: goto
_LL1286; _LL1286:;} goto _LL1022;} _LL1034: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1117->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1117);{ void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1369=( _temp1115 ==( void*) Cyc_Absyn_PostInc?
1: _temp1115 ==( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1370=( _temp1115 ==( void*) Cyc_Absyn_PreInc?
1: _temp1115 ==( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1369,({ struct Cyc_List_List*(* _temp1371)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1374= Cyc_Absyn_address_exp( _temp1117, 0); struct
Cyc_Absyn_Exp* _temp1375= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
struct Cyc_Absyn_Exp* _temp1376= Cyc_Absyn_signed_int_exp( _temp1370, 0); struct
Cyc_Absyn_Exp* _temp1372[ 3u]={ _temp1374, _temp1375, _temp1376}; struct
_tagged_arr _temp1373={( void*) _temp1372,( void*) _temp1372,( void*)( _temp1372
+ 3u)}; _temp1371( _temp1373);}), 0))->r));} if( elt_typ ==( void*) Cyc_Absyn_VoidType?
! Cyc_Absyn_is_lvalue( _temp1117): 0){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1117, 0, Cyc_Toc_incr_lvalue,
_temp1115);( void*)( e->r=( void*)(( void*) _temp1117->r));} goto _LL1022;}}
_LL1036: { int e1_poly= Cyc_Toc_is_poly_project( _temp1123); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1123->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1119->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1123); Cyc_Toc_exp_to_c( nv, _temp1119);{ int done= 0;
if( _temp1121 != 0){ void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* change;{ void* _temp1377=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1121))->v; _LL1379: if( _temp1377 ==(
void*) Cyc_Absyn_Plus){ goto _LL1380;} else{ goto _LL1381;} _LL1381: if(
_temp1377 ==( void*) Cyc_Absyn_Minus){ goto _LL1382;} else{ goto _LL1383;}
_LL1383: goto _LL1384; _LL1380: change= _temp1119; goto _LL1378; _LL1382: change=
Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp1119, 0); goto _LL1378;
_LL1384:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("bad t ? pointer arithmetic", sizeof( unsigned char), 27u))); goto
_LL1378; _LL1378:;} done= 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_inplace_plus_e,({ struct Cyc_List_List*(* _temp1385)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1388= Cyc_Absyn_address_exp( _temp1123, 0); struct
Cyc_Absyn_Exp* _temp1389= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
struct Cyc_Absyn_Exp* _temp1390= change; struct Cyc_Absyn_Exp* _temp1386[ 3u]={
_temp1388, _temp1389, _temp1390}; struct _tagged_arr _temp1387={( void*)
_temp1386,( void*) _temp1386,( void*)( _temp1386 + 3u)}; _temp1385( _temp1387);}),
0))->r));}} if( ! done){ if( e1_poly){( void*)( _temp1119->r=( void*)(( void*)(
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1119->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp1123)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, struct _tuple9*), struct _tuple9* f_env)) Cyc_Toc_lvalue_assign)(
_temp1123, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple9* _temp1391=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9) * 1); _temp1391[ 0]=({ struct
_tuple9 _temp1392; _temp1392.f1= _temp1121; _temp1392.f2= _temp1119; _temp1392;});
_temp1391;}));( void*)( e->r=( void*)(( void*) _temp1123->r));}} goto _LL1022;}}
_LL1038: Cyc_Toc_exp_to_c( nv, _temp1129); Cyc_Toc_exp_to_c( nv, _temp1127); Cyc_Toc_exp_to_c(
nv, _temp1125); goto _LL1022; _LL1040: Cyc_Toc_exp_to_c( nv, _temp1133); Cyc_Toc_exp_to_c(
nv, _temp1131); goto _LL1022; _LL1042: _temp1143= _temp1137; _temp1141=
_temp1135; goto _LL1044; _LL1044: Cyc_Toc_exp_to_c( nv, _temp1143);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1141);
goto _LL1022; _LL1046:{ struct _RegionHandle _temp1393= _new_region(); struct
_RegionHandle* r=& _temp1393; _push_region( r);{ void* _temp1394=( void*)
_temp1148->type; void* _temp1395= Cyc_Toc_typ_to_c( _temp1394); void* _temp1396=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1156->topt))->v);
struct Cyc_Absyn_Exp* _temp1397= Cyc_Absyn_uint_exp(( unsigned int) _temp1152, 0);
void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1446=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1446[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1447; _temp1447.tag= Cyc_Absyn_ArrayType;
_temp1447.f1=( void*) _temp1394; _temp1447.f2= Cyc_Toc_mt_tq; _temp1447.f3=(
struct Cyc_Absyn_Exp*) _temp1397; _temp1447;}); _temp1446;})); struct _tuple0*
_temp1398= Cyc_Toc_temp_var(); struct _tuple0* _temp1399= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1400= Cyc_Absyn_var_exp( _temp1399, 0); struct
_tuple0* _temp1401= Cyc_Toc_temp_var(); int _temp1402=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1154); int _temp1403= _temp1402 - _temp1152; struct
_tagged_arr vs=({ unsigned int _temp1442=( unsigned int) _temp1152; struct
_tuple0** _temp1443=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1442); struct _tagged_arr _temp1445= _tag_arr( _temp1443,
sizeof( struct _tuple0*),( unsigned int) _temp1152);{ unsigned int _temp1444=
_temp1442; unsigned int i; for( i= 0; i < _temp1444; i ++){ _temp1443[ i]= Cyc_Toc_temp_var();}};
_temp1445;}); struct _tagged_arr xs=({ unsigned int _temp1438=( unsigned int)
_temp1403; struct _tuple0** _temp1439=( struct _tuple0**) _region_malloc( r,
sizeof( struct _tuple0*) * _temp1438); struct _tagged_arr _temp1441= _tag_arr(
_temp1439, sizeof( struct _tuple0*),( unsigned int) _temp1403);{ unsigned int
_temp1440= _temp1438; unsigned int i; for( i= 0; i < _temp1440; i ++){ _temp1439[
i]= Cyc_Toc_temp_var();}}; _temp1441;}); struct Cyc_List_List* xexps=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,(
struct Cyc_Position_Segment*) 0,(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, xs));
xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List* _temp1404=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1404->hd=( void*) Cyc_Absyn_var_exp(
_temp1401, 0); _temp1404->tl= 0; _temp1404;}));{ struct Cyc_Absyn_Stmt*
_temp1405= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
_temp1398, 0), xexps, 0), 0); struct Cyc_Absyn_Exp* _temp1406= Cyc_Absyn_add_exp(
_temp1400, _temp1397, 0); struct Cyc_List_List* _temp1407=({ struct Cyc_List_List*(*
_temp1429)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct _tuple4* _temp1432=({ struct _tuple4* _temp1437=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1437->f1= 0; _temp1437->f2=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1400, 0); _temp1437;});
struct _tuple4* _temp1433=({ struct _tuple4* _temp1436=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1436->f1= 0; _temp1436->f2= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1400, 0); _temp1436;}); struct _tuple4*
_temp1434=({ struct _tuple4* _temp1435=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1435->f1= 0; _temp1435->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1406, 0); _temp1435;}); struct _tuple4* _temp1430[ 3u]={ _temp1432,
_temp1433, _temp1434}; struct _tagged_arr _temp1431={( void*) _temp1430,( void*)
_temp1430,( void*)( _temp1430 + 3u)}; _temp1429( _temp1431);}); _temp1405= Cyc_Absyn_declare_stmt(
_temp1401, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp1394,( void*) _temp1148->rgn,
Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1407, 0), _temp1405, 0);{ struct Cyc_List_List* _temp1408=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, vs));
struct Cyc_List_List* _temp1409=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_make_dle,
_temp1408); _temp1405= Cyc_Absyn_declare_stmt( _temp1399, arr_type,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp( 0, _temp1409, 0), _temp1405, 0);{ struct Cyc_List_List*
_temp1410=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, _temp1154); struct Cyc_List_List* _temp1411=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1150);{
int i= _temp1152 - 1; for( 0; i >= 0;( i --, _temp1410=(( struct Cyc_List_List*)
_check_null( _temp1410))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1410))->hd; void* arg_type=( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c( nv, arg);
if( _temp1148->inject){ void* _temp1414; struct Cyc_Position_Segment* _temp1416;
struct Cyc_List_List* _temp1418; struct Cyc_List_List* _temp1420; struct _tuple0*
_temp1422; struct Cyc_Absyn_Tunionfield _temp1412=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1411))->hd); _LL1423: _temp1422=
_temp1412.name; goto _LL1421; _LL1421: _temp1420= _temp1412.tvs; goto _LL1419;
_LL1419: _temp1418= _temp1412.typs; goto _LL1417; _LL1417: _temp1416= _temp1412.loc;
goto _LL1415; _LL1415: _temp1414=( void*) _temp1412.sc; goto _LL1413; _LL1413:
_temp1411=(( struct Cyc_List_List*) _check_null( _temp1411))->tl;{ struct
_tuple0* _temp1424= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1425= Cyc_Absyn_var_exp(
_temp1424, 0); void* _temp1426= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp1422, _tag_arr("_struct", sizeof( unsigned char), 8u))); _temp1405= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1395,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1395, Cyc_Absyn_address_exp(
_temp1425, 0), 0), _temp1405, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple3*)(( struct Cyc_List_List*) _check_null( _temp1418))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1427= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1425, Cyc_Toc_fieldname(
1), 0), arg, 0); _temp1405= Cyc_Absyn_seq_stmt( _temp1427, _temp1405, 0);{
struct Cyc_Absyn_Stmt* _temp1428= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1425, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1422, 0), 0); _temp1405=
Cyc_Absyn_seq_stmt( _temp1428, _temp1405, 0); _temp1405= Cyc_Absyn_declare_stmt(
_temp1424, _temp1426, 0, _temp1405, 0);}}}}} else{ _temp1405= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1395,( struct Cyc_Absyn_Exp*) arg, _temp1405, 0);}}}{ int i= _temp1403 - 1;
for( 0; i >= 0;( i --, _temp1410=(( struct Cyc_List_List*) _check_null(
_temp1410))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1410))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1405= Cyc_Absyn_declare_stmt(*(( struct _tuple0**)
_check_unknown_subscript( xs, sizeof( struct _tuple0*), i)), argtype,( struct
Cyc_Absyn_Exp*) arg, _temp1405, 0);}} Cyc_Toc_exp_to_c( nv, _temp1156);
_temp1405= Cyc_Absyn_declare_stmt( _temp1398, _temp1396,( struct Cyc_Absyn_Exp*)
_temp1156, _temp1405, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1405, 0))->r));}}}}; _pop_region( r);} goto _LL1022; _LL1048: Cyc_Toc_exp_to_c(
nv, _temp1158);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1158), 0))->r)); goto _LL1022; _LL1050: Cyc_Toc_exp_to_c(
nv, _temp1160); goto _LL1022; _LL1052: Cyc_Toc_exp_to_c( nv, _temp1164);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
for( 0; _temp1162 != 0; _temp1162=(( struct Cyc_List_List*) _check_null(
_temp1162))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*)
_check_null( _temp1162))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*)
Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1164, 0))->r)); break;}} goto _LL1022;} _LL1054: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1166->topt))->v; void* new_typ=*
_temp1170;* _temp1170= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1166);{ struct _tuple10 _temp1449=({ struct _tuple10 _temp1448; _temp1448.f1=
Cyc_Tcutil_compress( old_t2); _temp1448.f2= Cyc_Tcutil_compress( new_typ);
_temp1448;}); void* _temp1457; struct Cyc_Absyn_PtrInfo _temp1459; void*
_temp1461; struct Cyc_Absyn_PtrInfo _temp1463; void* _temp1465; void* _temp1467;
void* _temp1469; void* _temp1471; struct Cyc_Absyn_PtrInfo _temp1473; _LL1451:
_LL1462: _temp1461= _temp1449.f1; if(( unsigned int) _temp1461 > 4u?*(( int*)
_temp1461) == Cyc_Absyn_PointerType: 0){ _LL1464: _temp1463=(( struct Cyc_Absyn_PointerType_struct*)
_temp1461)->f1; goto _LL1458;} else{ goto _LL1453;} _LL1458: _temp1457=
_temp1449.f2; if(( unsigned int) _temp1457 > 4u?*(( int*) _temp1457) == Cyc_Absyn_PointerType:
0){ _LL1460: _temp1459=(( struct Cyc_Absyn_PointerType_struct*) _temp1457)->f1;
goto _LL1452;} else{ goto _LL1453;} _LL1453: _LL1472: _temp1471= _temp1449.f1;
if(( unsigned int) _temp1471 > 4u?*(( int*) _temp1471) == Cyc_Absyn_PointerType:
0){ _LL1474: _temp1473=(( struct Cyc_Absyn_PointerType_struct*) _temp1471)->f1;
goto _LL1466;} else{ goto _LL1455;} _LL1466: _temp1465= _temp1449.f2; if((
unsigned int) _temp1465 > 4u?*(( int*) _temp1465) == Cyc_Absyn_IntType: 0){
_LL1470: _temp1469=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1465)->f1;
goto _LL1468; _LL1468: _temp1467=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1465)->f2; goto _LL1454;} else{ goto _LL1455;} _LL1455: goto _LL1456;
_LL1452: { int _temp1475=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1463.nullable); int _temp1476=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1459.nullable); void* _temp1477=(( void*(*)( void*,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1463.bounds); void* _temp1478=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1459.bounds);{ struct
_tuple10 _temp1480=({ struct _tuple10 _temp1479; _temp1479.f1= _temp1477;
_temp1479.f2= _temp1478; _temp1479;}); void* _temp1490; struct Cyc_Absyn_Exp*
_temp1492; void* _temp1494; struct Cyc_Absyn_Exp* _temp1496; void* _temp1498;
void* _temp1500; struct Cyc_Absyn_Exp* _temp1502; void* _temp1504; struct Cyc_Absyn_Exp*
_temp1506; void* _temp1508; _LL1482: _LL1495: _temp1494= _temp1480.f1; if((
unsigned int) _temp1494 > 1u?*(( int*) _temp1494) == Cyc_Absyn_Upper_b: 0){
_LL1497: _temp1496=(( struct Cyc_Absyn_Upper_b_struct*) _temp1494)->f1; goto
_LL1491;} else{ goto _LL1484;} _LL1491: _temp1490= _temp1480.f2; if((
unsigned int) _temp1490 > 1u?*(( int*) _temp1490) == Cyc_Absyn_Upper_b: 0){
_LL1493: _temp1492=(( struct Cyc_Absyn_Upper_b_struct*) _temp1490)->f1; goto
_LL1483;} else{ goto _LL1484;} _LL1484: _LL1501: _temp1500= _temp1480.f1; if((
unsigned int) _temp1500 > 1u?*(( int*) _temp1500) == Cyc_Absyn_Upper_b: 0){
_LL1503: _temp1502=(( struct Cyc_Absyn_Upper_b_struct*) _temp1500)->f1; goto
_LL1499;} else{ goto _LL1486;} _LL1499: _temp1498= _temp1480.f2; if( _temp1498
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1485;} else{ goto _LL1486;} _LL1486:
_LL1509: _temp1508= _temp1480.f1; if( _temp1508 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1505;} else{ goto _LL1488;} _LL1505: _temp1504= _temp1480.f2; if((
unsigned int) _temp1504 > 1u?*(( int*) _temp1504) == Cyc_Absyn_Upper_b: 0){
_LL1507: _temp1506=(( struct Cyc_Absyn_Upper_b_struct*) _temp1504)->f1; goto
_LL1487;} else{ goto _LL1488;} _LL1488: goto _LL1489; _LL1483: if( _temp1475? !
_temp1476: 0){ if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't do null-check conversion at top-level", sizeof( unsigned char),
44u));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp1170, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1510=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1510->hd=( void*) _temp1166;
_temp1510->tl= 0; _temp1510;}), 0), 0))->r));} goto _LL1481; _LL1485: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1502, _temp1166))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1511)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1514= _temp1166; struct Cyc_Absyn_Exp* _temp1515= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1459.elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1516= _temp1502; struct Cyc_Absyn_Exp* _temp1512[ 3u]={ _temp1514,
_temp1515, _temp1516}; struct _tagged_arr _temp1513={( void*) _temp1512,( void*)
_temp1512,( void*)( _temp1512 + 3u)}; _temp1511( _temp1513);}), 0))->r));} goto
_LL1481; _LL1487: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u));}{ struct Cyc_Absyn_Exp* _temp1517= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({
struct Cyc_List_List*(* _temp1519)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1522= _temp1166;
struct Cyc_Absyn_Exp* _temp1523= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c((
void*) _temp1463.elt_typ), 0); struct Cyc_Absyn_Exp* _temp1524= _temp1506;
struct Cyc_Absyn_Exp* _temp1520[ 3u]={ _temp1522, _temp1523, _temp1524}; struct
_tagged_arr _temp1521={( void*) _temp1520,( void*) _temp1520,( void*)( _temp1520
+ 3u)}; _temp1519( _temp1521);}), 0); if( _temp1476){( void*)( _temp1517->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({ struct Cyc_List_List*
_temp1518=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1518->hd=( void*) Cyc_Absyn_copy_exp( _temp1517); _temp1518->tl= 0;
_temp1518;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp1170, _temp1517, 0))->r)); goto _LL1481;} _LL1489: goto _LL1481; _LL1481:;}
goto _LL1450;} _LL1454:{ void* _temp1525=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1473.bounds); _LL1527: if(
_temp1525 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1528;} else{ goto _LL1529;}
_LL1529: goto _LL1530; _LL1528:( void*)( _temp1166->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1166->r, _temp1166->loc), Cyc_Toc_curr_sp, 0))->r));
goto _LL1526; _LL1530: goto _LL1526; _LL1526:;} goto _LL1450; _LL1456: goto
_LL1450; _LL1450:;} goto _LL1022;} _LL1056:{ void* _temp1531=( void*) _temp1171->r;
struct Cyc_Absyn_Structdecl* _temp1539; struct Cyc_List_List* _temp1541; struct
Cyc_Core_Opt* _temp1543; struct _tuple0* _temp1545; struct Cyc_List_List*
_temp1547; _LL1533: if(*(( int*) _temp1531) == Cyc_Absyn_Struct_e){ _LL1546:
_temp1545=(( struct Cyc_Absyn_Struct_e_struct*) _temp1531)->f1; goto _LL1544;
_LL1544: _temp1543=(( struct Cyc_Absyn_Struct_e_struct*) _temp1531)->f2; goto
_LL1542; _LL1542: _temp1541=(( struct Cyc_Absyn_Struct_e_struct*) _temp1531)->f3;
goto _LL1540; _LL1540: _temp1539=(( struct Cyc_Absyn_Struct_e_struct*) _temp1531)->f4;
goto _LL1534;} else{ goto _LL1535;} _LL1535: if(*(( int*) _temp1531) == Cyc_Absyn_Tuple_e){
_LL1548: _temp1547=(( struct Cyc_Absyn_Tuple_e_struct*) _temp1531)->f1; goto
_LL1536;} else{ goto _LL1537;} _LL1537: goto _LL1538; _LL1534: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr _temp1549=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1171->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_get_arr_size( _temp1549, 1u), _temp1549.curr);}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1171->topt))->v, 1, 0, _temp1541, _temp1545))->r)); goto _LL1532; _LL1536:
if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct
_tagged_arr)({ struct _tagged_arr _temp1550=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1171->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_get_arr_size( _temp1550, 1u), _temp1550.curr);}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1547))->r)); goto _LL1532; _LL1538:
Cyc_Toc_exp_to_c( nv, _temp1171); if( ! Cyc_Absyn_is_lvalue( _temp1171)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1171, 0,
Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1171, 0))->r));} goto _LL1532; _LL1532:;} goto _LL1022; _LL1058: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr _temp1551=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1173->loc); xprintf("%.*s: new at top-level", _get_arr_size( _temp1551, 1u),
_temp1551.curr);}));}{ void* _temp1552=( void*) _temp1173->r; struct Cyc_List_List*
_temp1564; struct Cyc_Absyn_Exp* _temp1566; struct Cyc_Absyn_Exp* _temp1568;
struct Cyc_Absyn_Vardecl* _temp1570; struct Cyc_Absyn_Structdecl* _temp1572;
struct Cyc_List_List* _temp1574; struct Cyc_Core_Opt* _temp1576; struct _tuple0*
_temp1578; struct Cyc_List_List* _temp1580; _LL1554: if(*(( int*) _temp1552) ==
Cyc_Absyn_Array_e){ _LL1565: _temp1564=(( struct Cyc_Absyn_Array_e_struct*)
_temp1552)->f1; goto _LL1555;} else{ goto _LL1556;} _LL1556: if(*(( int*)
_temp1552) == Cyc_Absyn_Comprehension_e){ _LL1571: _temp1570=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1552)->f1; goto _LL1569; _LL1569: _temp1568=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1552)->f2; goto _LL1567; _LL1567: _temp1566=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1552)->f3; goto _LL1557;} else{ goto _LL1558;} _LL1558: if(*(( int*)
_temp1552) == Cyc_Absyn_Struct_e){ _LL1579: _temp1578=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1552)->f1; goto _LL1577; _LL1577: _temp1576=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1552)->f2; goto _LL1575; _LL1575: _temp1574=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1552)->f3; goto _LL1573; _LL1573: _temp1572=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1552)->f4; goto _LL1559;} else{ goto _LL1560;} _LL1560: if(*(( int*)
_temp1552) == Cyc_Absyn_Tuple_e){ _LL1581: _temp1580=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1552)->f1; goto _LL1561;} else{ goto _LL1562;} _LL1562: goto _LL1563;
_LL1555: { struct _tuple0* _temp1582= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1583= Cyc_Absyn_var_exp( _temp1582, 0); struct Cyc_Absyn_Stmt* _temp1584=
Cyc_Toc_init_array( nv, _temp1583, _temp1564, Cyc_Absyn_exp_stmt( _temp1583, 0));
void* old_elt_typ;{ void* _temp1585= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1591; struct Cyc_Absyn_Conref* _temp1593; struct Cyc_Absyn_Tqual _temp1595;
struct Cyc_Absyn_Conref* _temp1597; void* _temp1599; void* _temp1601; _LL1587:
if(( unsigned int) _temp1585 > 4u?*(( int*) _temp1585) == Cyc_Absyn_PointerType:
0){ _LL1592: _temp1591=(( struct Cyc_Absyn_PointerType_struct*) _temp1585)->f1;
_LL1602: _temp1601=( void*) _temp1591.elt_typ; goto _LL1600; _LL1600: _temp1599=(
void*) _temp1591.rgn_typ; goto _LL1598; _LL1598: _temp1597= _temp1591.nullable;
goto _LL1596; _LL1596: _temp1595= _temp1591.tq; goto _LL1594; _LL1594: _temp1593=
_temp1591.bounds; goto _LL1588;} else{ goto _LL1589;} _LL1589: goto _LL1590;
_LL1588: old_elt_typ= _temp1601; goto _LL1586; _LL1590: old_elt_typ=(( void*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u)); goto _LL1586; _LL1586:;}{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* _temp1603= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* _temp1604= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1564), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1175 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1604);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1175); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1604);}{ struct Cyc_Absyn_Exp* _temp1605= Cyc_Absyn_cast_exp( _temp1603, e1,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1582, _temp1603,( struct Cyc_Absyn_Exp*) _temp1605, _temp1584, 0), 0))->r));
goto _LL1553;}}} _LL1557: { int is_tagged_ptr= 0;{ void* _temp1606= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1612; struct Cyc_Absyn_Conref* _temp1614;
struct Cyc_Absyn_Tqual _temp1616; struct Cyc_Absyn_Conref* _temp1618; void*
_temp1620; void* _temp1622; _LL1608: if(( unsigned int) _temp1606 > 4u?*(( int*)
_temp1606) == Cyc_Absyn_PointerType: 0){ _LL1613: _temp1612=(( struct Cyc_Absyn_PointerType_struct*)
_temp1606)->f1; _LL1623: _temp1622=( void*) _temp1612.elt_typ; goto _LL1621;
_LL1621: _temp1620=( void*) _temp1612.rgn_typ; goto _LL1619; _LL1619: _temp1618=
_temp1612.nullable; goto _LL1617; _LL1617: _temp1616= _temp1612.tq; goto _LL1615;
_LL1615: _temp1614= _temp1612.bounds; goto _LL1609;} else{ goto _LL1610;}
_LL1610: goto _LL1611; _LL1609: is_tagged_ptr=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1614) ==( void*) Cyc_Absyn_Unknown_b;
goto _LL1607; _LL1611:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: comprehension not an array type", sizeof( unsigned char), 42u));
goto _LL1607; _LL1607:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct
_tuple0* a= Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1566->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1568);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1570, Cyc_Absyn_var_exp( max, 0), _temp1566, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1624= _new_region(); struct _RegionHandle* r=&
_temp1624; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1647=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1647->hd=( void*)({ struct _tuple8* _temp1648=( struct _tuple8*)
_region_malloc( r, sizeof( struct _tuple8)); _temp1648->f1= max; _temp1648->f2=
Cyc_Absyn_uint_t; _temp1648->f3=( struct Cyc_Absyn_Exp*) _temp1568; _temp1648;});
_temp1647->tl= 0; _temp1647;}); struct Cyc_Absyn_Exp* ai; if( _temp1175 == 0){
ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( _temp1175); Cyc_Toc_exp_to_c( nv, r); ai=
Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0),
Cyc_Absyn_var_exp( max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp(
ptr_typ, ai, 0); decls=({ struct Cyc_List_List* _temp1625=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1625->hd=( void*)({
struct _tuple8* _temp1626=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1626->f1= a; _temp1626->f2= ptr_typ; _temp1626->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1626;}); _temp1625->tl= decls; _temp1625;}); if( is_tagged_ptr){
struct _tuple0* _temp1627= Cyc_Toc_temp_var(); void* _temp1628= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Exp* _temp1629= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_List_List*(* _temp1632)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1635= Cyc_Absyn_var_exp(
a, 0); struct Cyc_Absyn_Exp* _temp1636= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
struct Cyc_Absyn_Exp* _temp1637= _temp1568; struct Cyc_Absyn_Exp* _temp1633[ 3u]={
_temp1635, _temp1636, _temp1637}; struct _tagged_arr _temp1634={( void*)
_temp1633,( void*) _temp1633,( void*)( _temp1633 + 3u)}; _temp1632( _temp1634);}),
0); decls=({ struct Cyc_List_List* _temp1630=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1630->hd=( void*)({
struct _tuple8* _temp1631=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1631->f1= _temp1627; _temp1631->f2= _temp1628; _temp1631->f3=(
struct Cyc_Absyn_Exp*) _temp1629; _temp1631;}); _temp1630->tl= decls; _temp1630;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1627, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1638= decls; for( 0; _temp1638 != 0;
_temp1638=(( struct Cyc_List_List*) _check_null( _temp1638))->tl){ struct Cyc_Absyn_Exp*
_temp1641; void* _temp1643; struct _tuple0* _temp1645; struct _tuple8 _temp1639=*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1638))->hd); _LL1646:
_temp1645= _temp1639.f1; goto _LL1644; _LL1644: _temp1643= _temp1639.f2; goto
_LL1642; _LL1642: _temp1641= _temp1639.f3; goto _LL1640; _LL1640: s= Cyc_Absyn_declare_stmt(
_temp1645, _temp1643, _temp1641, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL1553;}}} _LL1559:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1173->topt))->v, 1, _temp1175, _temp1574, _temp1578))->r)); goto _LL1553;
_LL1561:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1175,
_temp1580))->r)); goto _LL1553; _LL1563: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1173->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp1175 == 0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1175); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1649=( void*)
_temp1173->r; struct Cyc_Absyn_Exp* _temp1655; void* _temp1657; _LL1651: if(*((
int*) _temp1649) == Cyc_Absyn_Cast_e){ _LL1658: _temp1657=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1649)->f1; goto _LL1656; _LL1656: _temp1655=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1649)->f2; goto _LL1652;} else{ goto _LL1653;} _LL1653: goto _LL1654;
_LL1652:{ struct _tuple10 _temp1660=({ struct _tuple10 _temp1659; _temp1659.f1=
Cyc_Tcutil_compress( _temp1657); _temp1659.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1655->topt))->v); _temp1659;}); void*
_temp1666; struct Cyc_Absyn_PtrInfo _temp1668; struct Cyc_Absyn_Conref*
_temp1670; struct Cyc_Absyn_Tqual _temp1672; struct Cyc_Absyn_Conref* _temp1674;
void* _temp1676; void* _temp1678; void* _temp1680; struct Cyc_Absyn_PtrInfo
_temp1682; struct Cyc_Absyn_Conref* _temp1684; struct Cyc_Absyn_Tqual _temp1686;
struct Cyc_Absyn_Conref* _temp1688; void* _temp1690; void* _temp1692; _LL1662:
_LL1681: _temp1680= _temp1660.f1; if(( unsigned int) _temp1680 > 4u?*(( int*)
_temp1680) == Cyc_Absyn_PointerType: 0){ _LL1683: _temp1682=(( struct Cyc_Absyn_PointerType_struct*)
_temp1680)->f1; _LL1693: _temp1692=( void*) _temp1682.elt_typ; goto _LL1691;
_LL1691: _temp1690=( void*) _temp1682.rgn_typ; goto _LL1689; _LL1689: _temp1688=
_temp1682.nullable; goto _LL1687; _LL1687: _temp1686= _temp1682.tq; goto _LL1685;
_LL1685: _temp1684= _temp1682.bounds; goto _LL1667;} else{ goto _LL1664;}
_LL1667: _temp1666= _temp1660.f2; if(( unsigned int) _temp1666 > 4u?*(( int*)
_temp1666) == Cyc_Absyn_PointerType: 0){ _LL1669: _temp1668=(( struct Cyc_Absyn_PointerType_struct*)
_temp1666)->f1; _LL1679: _temp1678=( void*) _temp1668.elt_typ; goto _LL1677;
_LL1677: _temp1676=( void*) _temp1668.rgn_typ; goto _LL1675; _LL1675: _temp1674=
_temp1668.nullable; goto _LL1673; _LL1673: _temp1672= _temp1668.tq; goto _LL1671;
_LL1671: _temp1670= _temp1668.bounds; goto _LL1663;} else{ goto _LL1664;}
_LL1664: goto _LL1665; _LL1663:{ struct _tuple10 _temp1695=({ struct _tuple10
_temp1694; _temp1694.f1=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1684); _temp1694.f2=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1670); _temp1694;}); void*
_temp1701; struct Cyc_Absyn_Exp* _temp1703; void* _temp1705; _LL1697: _LL1706:
_temp1705= _temp1695.f1; if( _temp1705 ==( void*) Cyc_Absyn_Unknown_b){ goto
_LL1702;} else{ goto _LL1699;} _LL1702: _temp1701= _temp1695.f2; if((
unsigned int) _temp1701 > 1u?*(( int*) _temp1701) == Cyc_Absyn_Upper_b: 0){
_LL1704: _temp1703=(( struct Cyc_Absyn_Upper_b_struct*) _temp1701)->f1; goto
_LL1698;} else{ goto _LL1699;} _LL1699: goto _LL1700; _LL1698: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_List_List*(* _temp1707)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1710= mexp;
struct Cyc_Absyn_Exp* _temp1711= _temp1655; struct Cyc_Absyn_Exp* _temp1712= Cyc_Absyn_sizeoftyp_exp(
_temp1692, 0); struct Cyc_Absyn_Exp* _temp1713= _temp1703; struct Cyc_Absyn_Exp*
_temp1708[ 4u]={ _temp1710, _temp1711, _temp1712, _temp1713}; struct _tagged_arr
_temp1709={( void*) _temp1708,( void*) _temp1708,( void*)( _temp1708 + 4u)};
_temp1707( _temp1709);}), 0))->r)); goto _LL1696; _LL1700: goto _LL1696; _LL1696:;}
goto _LL1661; _LL1665: goto _LL1661; _LL1661:;} goto _LL1650; _LL1654: goto
_LL1650; _LL1650:;} if( ! done){ struct _tuple0* _temp1714= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* _temp1715= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1714, 0), 0); struct Cyc_Absyn_Exp* _temp1716= Cyc_Absyn_signed_int_exp( 0,
0); Cyc_Toc_exp_to_c( nv, _temp1173); _temp1715= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1714, 0), _temp1716, 0),
_temp1173, 0), _temp1715, 0);{ void* _temp1717= Cyc_Absyn_cstar_typ( elt_typ,
Cyc_Toc_mt_tq);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1714, _temp1717,( struct Cyc_Absyn_Exp*) mexp, _temp1715, 0), 0))->r));}}
goto _LL1553;}} _LL1553:;} goto _LL1022; _LL1060: Cyc_Toc_exp_to_c( nv,
_temp1177); goto _LL1022; _LL1062:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1718=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1718[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1719; _temp1719.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1719.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1179);
_temp1719;}); _temp1718;}))); goto _LL1022; _LL1064:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1720=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1720[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1721; _temp1721.tag= Cyc_Absyn_Offsetof_e;
_temp1721.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1183); _temp1721.f2=
_temp1181; _temp1721;}); _temp1720;}))); goto _LL1022; _LL1066: { void*
_temp1722= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1185->topt))->v);{ void* _temp1723= _temp1722; struct Cyc_Absyn_PtrInfo
_temp1729; struct Cyc_Absyn_Conref* _temp1731; struct Cyc_Absyn_Tqual _temp1733;
struct Cyc_Absyn_Conref* _temp1735; void* _temp1737; void* _temp1739; _LL1725:
if(( unsigned int) _temp1723 > 4u?*(( int*) _temp1723) == Cyc_Absyn_PointerType:
0){ _LL1730: _temp1729=(( struct Cyc_Absyn_PointerType_struct*) _temp1723)->f1;
_LL1740: _temp1739=( void*) _temp1729.elt_typ; goto _LL1738; _LL1738: _temp1737=(
void*) _temp1729.rgn_typ; goto _LL1736; _LL1736: _temp1735= _temp1729.nullable;
goto _LL1734; _LL1734: _temp1733= _temp1729.tq; goto _LL1732; _LL1732: _temp1731=
_temp1729.bounds; goto _LL1726;} else{ goto _LL1727;} _LL1727: goto _LL1728;
_LL1726:{ void* _temp1741=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1731); struct Cyc_Absyn_Exp* _temp1747; _LL1743: if((
unsigned int) _temp1741 > 1u?*(( int*) _temp1741) == Cyc_Absyn_Upper_b: 0){
_LL1748: _temp1747=(( struct Cyc_Absyn_Upper_b_struct*) _temp1741)->f1; goto
_LL1744;} else{ goto _LL1745;} _LL1745: if( _temp1741 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1746;} else{ goto _LL1742;} _LL1744: Cyc_Toc_exp_to_c( nv, _temp1185);
if( Cyc_Toc_is_nullable( _temp1722)){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1185->topt))->v), Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1749=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1749->hd=( void*) _temp1185; _temp1749->tl= 0;
_temp1749;}), 0), 0), 0))->r));} goto _LL1742; _LL1746: { struct Cyc_Absyn_Exp*
_temp1750= Cyc_Absyn_uint_exp( 0, 0); _temp1750->topt=({ struct Cyc_Core_Opt*
_temp1751=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1751->v=( void*) Cyc_Absyn_uint_t; _temp1751;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_subscript_exp( _temp1185, _temp1750, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1742;} _LL1742:;} goto _LL1724; _LL1728:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Deref: non-pointer",
sizeof( unsigned char), 29u)); goto _LL1724; _LL1724:;} goto _LL1022;} _LL1068:
Cyc_Toc_exp_to_c( nv, _temp1189); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL1022; _LL1070: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1193->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1193);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1754; struct Cyc_Absyn_Tqual _temp1756; struct Cyc_Absyn_Conref* _temp1758;
void* _temp1760; void* _temp1762; struct Cyc_Absyn_PtrInfo _temp1752= Cyc_Toc_get_ptr_type(
e1typ); _LL1763: _temp1762=( void*) _temp1752.elt_typ; goto _LL1761; _LL1761:
_temp1760=( void*) _temp1752.rgn_typ; goto _LL1759; _LL1759: _temp1758=
_temp1752.nullable; goto _LL1757; _LL1757: _temp1756= _temp1752.tq; goto _LL1755;
_LL1755: _temp1754= _temp1752.bounds; goto _LL1753; _LL1753:{ void* _temp1764=((
void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1754); struct Cyc_Absyn_Exp* _temp1770; _LL1766: if(( unsigned int)
_temp1764 > 1u?*(( int*) _temp1764) == Cyc_Absyn_Upper_b: 0){ _LL1771: _temp1770=((
struct Cyc_Absyn_Upper_b_struct*) _temp1764)->f1; goto _LL1767;} else{ goto
_LL1768;} _LL1768: if( _temp1764 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1769;}
else{ goto _LL1765;} _LL1767: if( Cyc_Evexp_eval_const_uint_exp( _temp1770) < 1){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof( unsigned char), 46u));}{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( 0, _temp1758); if( Cyc_Toc_is_nullable(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1193->topt))->v)){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1193->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1772=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1772->hd=( void*) _temp1193;
_temp1772->tl= 0; _temp1772;}), 0), 0), _temp1191, 0))->r));} goto _LL1765;}
_LL1769: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1762);( void*)( _temp1193->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1756), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_List_List*(* _temp1773)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1776= Cyc_Absyn_copy_exp( _temp1193); struct Cyc_Absyn_Exp*
_temp1777= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp* _temp1778=
Cyc_Absyn_uint_exp( 0, 0); struct Cyc_Absyn_Exp* _temp1774[ 3u]={ _temp1776,
_temp1777, _temp1778}; struct _tagged_arr _temp1775={( void*) _temp1774,( void*)
_temp1774,( void*)( _temp1774 + 3u)}; _temp1773( _temp1775);}), 0), 0))->r));
goto _LL1765;} _LL1765:;} if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1022;}} _LL1072: { void*
_temp1779= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1197->topt))->v); Cyc_Toc_exp_to_c( nv, _temp1197); Cyc_Toc_exp_to_c( nv,
_temp1195);{ void* _temp1780= _temp1779; struct Cyc_List_List* _temp1788; struct
Cyc_Absyn_PtrInfo _temp1790; struct Cyc_Absyn_Conref* _temp1792; struct Cyc_Absyn_Tqual
_temp1794; struct Cyc_Absyn_Conref* _temp1796; void* _temp1798; void* _temp1800;
_LL1782: if(( unsigned int) _temp1780 > 4u?*(( int*) _temp1780) == Cyc_Absyn_TupleType:
0){ _LL1789: _temp1788=(( struct Cyc_Absyn_TupleType_struct*) _temp1780)->f1;
goto _LL1783;} else{ goto _LL1784;} _LL1784: if(( unsigned int) _temp1780 > 4u?*((
int*) _temp1780) == Cyc_Absyn_PointerType: 0){ _LL1791: _temp1790=(( struct Cyc_Absyn_PointerType_struct*)
_temp1780)->f1; _LL1801: _temp1800=( void*) _temp1790.elt_typ; goto _LL1799;
_LL1799: _temp1798=( void*) _temp1790.rgn_typ; goto _LL1797; _LL1797: _temp1796=
_temp1790.nullable; goto _LL1795; _LL1795: _temp1794= _temp1790.tq; goto _LL1793;
_LL1793: _temp1792= _temp1790.bounds; goto _LL1785;} else{ goto _LL1786;}
_LL1786: goto _LL1787; _LL1783: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1195) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1197, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL1781;} _LL1785:{ void*
_temp1802=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1792); struct Cyc_Absyn_Exp* _temp1808; _LL1804: if((
unsigned int) _temp1802 > 1u?*(( int*) _temp1802) == Cyc_Absyn_Upper_b: 0){
_LL1809: _temp1808=(( struct Cyc_Absyn_Upper_b_struct*) _temp1802)->f1; goto
_LL1805;} else{ goto _LL1806;} _LL1806: if( _temp1802 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1807;} else{ goto _LL1803;} _LL1805: _temp1808= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1808), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1796); void* ta1= Cyc_Toc_typ_to_c( _temp1800);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1794); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_List_List*(* _temp1810)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1813= _temp1197; struct Cyc_Absyn_Exp* _temp1814=
_temp1808; struct Cyc_Absyn_Exp* _temp1815= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
struct Cyc_Absyn_Exp* _temp1816= _temp1195; struct Cyc_Absyn_Exp* _temp1811[ 4u]={
_temp1813, _temp1814, _temp1815, _temp1816}; struct _tagged_arr _temp1812={(
void*) _temp1811,( void*) _temp1811,( void*)( _temp1811 + 4u)}; _temp1810(
_temp1812);}), 0), 0), 0))->r));} else{( void*)( _temp1195->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_List_List*(*
_temp1817)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp1820= _temp1808; struct Cyc_Absyn_Exp*
_temp1821= Cyc_Absyn_copy_exp( _temp1195); struct Cyc_Absyn_Exp* _temp1818[ 2u]={
_temp1820, _temp1821}; struct _tagged_arr _temp1819={( void*) _temp1818,( void*)
_temp1818,( void*)( _temp1818 + 2u)}; _temp1817( _temp1819);}), 0))->r));} goto
_LL1803;} _LL1807: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1800);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
ta1, _temp1794), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_List_List*(* _temp1822)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1825= _temp1197;
struct Cyc_Absyn_Exp* _temp1826= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp1827= _temp1195; struct Cyc_Absyn_Exp* _temp1823[ 3u]={ _temp1825,
_temp1826, _temp1827}; struct _tagged_arr _temp1824={( void*) _temp1823,( void*)
_temp1823,( void*)( _temp1823 + 3u)}; _temp1822( _temp1824);}), 0), 0), 0))->r));
goto _LL1803;} _LL1803:;} goto _LL1781; _LL1787:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u)); goto _LL1781; _LL1781:;} goto _LL1022;} _LL1074:
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 0,
0, _temp1199))->r));} else{ struct Cyc_List_List* _temp1828=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, _temp1199); struct _tagged_arr* _temp1829= Cyc_Toc_add_tuple_type(
_temp1828); struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1199 != 0;(
_temp1199=(( struct Cyc_List_List*) _check_null( _temp1199))->tl, i ++)){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1199))->hd);
dles=({ struct Cyc_List_List* _temp1830=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1830->hd=( void*)({ struct _tuple4*
_temp1831=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1831->f1=
0; _temp1831->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1199))->hd; _temp1831;}); _temp1830->tl= dles; _temp1830;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL1022; _LL1076:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1201, 0))->r));{ struct Cyc_List_List* _temp1832= _temp1201; for( 0;
_temp1832 != 0; _temp1832=(( struct Cyc_List_List*) _check_null( _temp1832))->tl){
struct _tuple4 _temp1835; struct Cyc_Absyn_Exp* _temp1836; struct Cyc_List_List*
_temp1838; struct _tuple4* _temp1833=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1832))->hd; _temp1835=* _temp1833; _LL1839: _temp1838=
_temp1835.f1; goto _LL1837; _LL1837: _temp1836= _temp1835.f2; goto _LL1834;
_LL1834: Cyc_Toc_exp_to_c( nv, _temp1836);}} goto _LL1022; _LL1078: {
unsigned int _temp1840= Cyc_Evexp_eval_const_uint_exp( _temp1205); Cyc_Toc_exp_to_c(
nv, _temp1203);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1203)){
unsigned int i= 0; for( 0; i < _temp1840; i ++){ es=({ struct Cyc_List_List*
_temp1841=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1841->hd=( void*)({ struct _tuple4* _temp1842=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1842->f1= 0; _temp1842->f2= _temp1203; _temp1842;});
_temp1841->tl= es; _temp1841;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL1022;}} _LL1080: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1211, _temp1215))->r));}
else{ if( _temp1209 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Struct_e: missing structdecl pointer", sizeof( unsigned char), 37u));}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp1209); struct _RegionHandle _temp1843= _new_region(); struct _RegionHandle*
rgn=& _temp1843; _push_region( rgn);{ struct Cyc_List_List* _temp1844=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1211,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1845= 0;{ struct Cyc_List_List*
_temp1846=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1846 != 0; _temp1846=(( struct Cyc_List_List*) _check_null(
_temp1846))->tl){ struct Cyc_List_List* _temp1847= _temp1844; for( 0; _temp1847
!= 0; _temp1847=(( struct Cyc_List_List*) _check_null( _temp1847))->tl){ if((*((
struct _tuple11*)(( struct Cyc_List_List*) _check_null( _temp1847))->hd)).f1 ==(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1846))->hd){
struct _tuple11 _temp1850; struct Cyc_Absyn_Exp* _temp1851; struct Cyc_Absyn_Structfield*
_temp1853; struct _tuple11* _temp1848=( struct _tuple11*)(( struct Cyc_List_List*)
_check_null( _temp1847))->hd; _temp1850=* _temp1848; _LL1854: _temp1853=
_temp1850.f1; goto _LL1852; _LL1852: _temp1851= _temp1850.f2; goto _LL1849;
_LL1849: { void* _temp1855=( void*) _temp1853->type; Cyc_Toc_exp_to_c( nv,
_temp1851); if( Cyc_Toc_is_void_star( _temp1855)){( void*)( _temp1851->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1851->r, 0), 0))->r));} _temp1845=({ struct Cyc_List_List* _temp1856=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1856->hd=(
void*)({ struct _tuple4* _temp1857=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1857->f1= 0; _temp1857->f2= _temp1851; _temp1857;}); _temp1856->tl=
_temp1845; _temp1856;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1845), 0))->r));}; _pop_region( rgn);}} goto _LL1022; _LL1082: { struct Cyc_List_List*
fs;{ void* _temp1858= Cyc_Tcutil_compress( _temp1219); struct Cyc_List_List*
_temp1864; _LL1860: if(( unsigned int) _temp1858 > 4u?*(( int*) _temp1858) ==
Cyc_Absyn_AnonStructType: 0){ _LL1865: _temp1864=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1858)->f1; goto _LL1861;} else{ goto _LL1862;} _LL1862: goto _LL1863;
_LL1861: fs= _temp1864; goto _LL1859; _LL1863: fs=(( struct Cyc_List_List*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp1866= Cyc_Absynpp_typ2string( _temp1219); xprintf("anon struct has type %.*s",
_get_arr_size( _temp1866, 1u), _temp1866.curr);})); goto _LL1859; _LL1859:;}{
struct _RegionHandle _temp1867= _new_region(); struct _RegionHandle* rgn=&
_temp1867; _push_region( rgn);{ struct Cyc_List_List* _temp1868=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1217, fs); for( 0; _temp1868 != 0; _temp1868=(( struct Cyc_List_List*)
_check_null( _temp1868))->tl){ struct _tuple11 _temp1871; struct Cyc_Absyn_Exp*
_temp1872; struct Cyc_Absyn_Structfield* _temp1874; struct _tuple11* _temp1869=(
struct _tuple11*)(( struct Cyc_List_List*) _check_null( _temp1868))->hd;
_temp1871=* _temp1869; _LL1875: _temp1874= _temp1871.f1; goto _LL1873; _LL1873:
_temp1872= _temp1871.f2; goto _LL1870; _LL1870: { void* _temp1876=( void*)
_temp1874->type; Cyc_Toc_exp_to_c( nv, _temp1872); if( Cyc_Toc_is_void_star(
_temp1876)){( void*)( _temp1872->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1872->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1217, 0))->r));}; _pop_region( rgn);}
goto _LL1022;} _LL1084: { struct _tuple0* qv= _temp1221->name; if( _temp1223->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1877= _temp1223->fields == 0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1223->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1877))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1877))->hd)->typs == 0){ tag_count ++;}
_temp1877=(( struct Cyc_List_List*) _check_null( _temp1877))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL1022;} _LL1086: { struct _tuple0* _temp1878= _temp1231->name; struct Cyc_List_List*
_temp1879= _temp1231->typs; struct _tuple0* _temp1880= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1881= Cyc_Absyn_var_exp( _temp1880, 0); void*
_temp1882= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1878, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1233->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1878, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1883= _temp1233->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1233->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1878,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1883))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1883))->hd)->typs != 0){ tag_count ++;}
_temp1883=(( struct Cyc_List_List*) _check_null( _temp1883))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp1884= 0; for( 0;
_temp1235 != 0;( _temp1235=(( struct Cyc_List_List*) _check_null( _temp1235))->tl,
_temp1879=(( struct Cyc_List_List*) _check_null( _temp1879))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1235))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1879))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} _temp1884=({
struct Cyc_List_List* _temp1885=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1885[ 0]=({ struct Cyc_List_List _temp1886;
_temp1886.hd=( void*)({ struct _tuple4* _temp1887=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4) * 1); _temp1887[ 0]=({ struct _tuple4 _temp1888;
_temp1888.f1= 0; _temp1888.f2= cur_e; _temp1888;}); _temp1887;}); _temp1886.tl=
_temp1884; _temp1886;}); _temp1885;});} _temp1884=({ struct Cyc_List_List*
_temp1889=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1889[ 0]=({ struct Cyc_List_List _temp1890; _temp1890.hd=( void*)({ struct
_tuple4* _temp1891=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp1891[ 0]=({ struct _tuple4 _temp1892; _temp1892.f1= 0; _temp1892.f2=
tag_exp; _temp1892;}); _temp1891;}); _temp1890.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1884); _temp1890;}); _temp1889;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1893=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1893->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1878, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp1893;}), _temp1884, 0))->r));}} else{ struct
Cyc_List_List* _temp1894=({ struct Cyc_List_List* _temp1900=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1900->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1881, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1878, 0), 0); _temp1900->tl= 0; _temp1900;});{ int i= 1; for( 0; _temp1235
!= 0;((( _temp1235=(( struct Cyc_List_List*) _check_null( _temp1235))->tl, i ++)),
_temp1879=(( struct Cyc_List_List*) _check_null( _temp1879))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1235))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1879))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp1895= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1881, Cyc_Toc_fieldname(
i), 0), cur_e, 0); _temp1894=({ struct Cyc_List_List* _temp1896=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1896->hd=( void*) _temp1895;
_temp1896->tl= _temp1894; _temp1896;});}}}{ struct Cyc_Absyn_Stmt* _temp1897=
Cyc_Absyn_exp_stmt( _temp1881, 0); struct Cyc_Absyn_Stmt* _temp1898= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1899=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1899->hd=( void*) _temp1897; _temp1899->tl= _temp1894; _temp1899;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1880, _temp1882, 0, _temp1898, 0), 0))->r));}} goto _LL1022;} _LL1088: goto
_LL1022; _LL1090: { void* t_c= Cyc_Toc_typ_to_c( _temp1247); if( _temp1249 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1249); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1247, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL1022;} _LL1092: Cyc_Toc_stmt_to_c(
nv, _temp1251); goto _LL1022; _LL1094:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem", sizeof( unsigned char), 14u)); goto _LL1022; _LL1096:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u)); goto _LL1022; _LL1098:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u)); goto _LL1022;
_LL1100:(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("fill",
sizeof( unsigned char), 5u)); goto _LL1022; _LL1022:;}} static struct Cyc_Absyn_Stmt*
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
_temp1901=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp1937; struct Cyc_Absyn_Vardecl
_temp1939; struct Cyc_List_List* _temp1940; struct Cyc_Core_Opt* _temp1942;
struct Cyc_Absyn_Exp* _temp1944; void* _temp1946; struct Cyc_Absyn_Tqual
_temp1948; struct _tuple0* _temp1950; void* _temp1952; struct Cyc_Absyn_Vardecl*
_temp1954; int _temp1956; void* _temp1958; unsigned char _temp1960; struct
_tagged_arr _temp1962; struct Cyc_Absyn_Enumfield* _temp1964; struct Cyc_Absyn_Enumdecl*
_temp1966; struct Cyc_List_List* _temp1968; struct Cyc_List_List* _temp1970;
struct Cyc_Absyn_Tunionfield* _temp1972; struct Cyc_Absyn_Tuniondecl* _temp1974;
struct Cyc_Absyn_Pat* _temp1977; struct Cyc_Absyn_Pat _temp1979; struct Cyc_Position_Segment*
_temp1980; struct Cyc_Core_Opt* _temp1982; void* _temp1984; struct Cyc_List_List*
_temp1986; struct Cyc_List_List* _temp1988; struct Cyc_Absyn_Tunionfield*
_temp1990; struct Cyc_Absyn_Tuniondecl* _temp1992; struct Cyc_List_List*
_temp1994; struct Cyc_List_List* _temp1996; struct Cyc_Absyn_Tunionfield*
_temp1998; struct Cyc_Absyn_Tuniondecl* _temp2000; struct Cyc_List_List*
_temp2002; struct Cyc_List_List* _temp2004; struct Cyc_List_List* _temp2006;
struct Cyc_Core_Opt* _temp2008; struct Cyc_Absyn_Structdecl* _temp2010; struct
Cyc_Absyn_Pat* _temp2012; struct _tuple0* _temp2014; struct Cyc_List_List*
_temp2016; struct Cyc_List_List* _temp2018; struct _tuple0* _temp2020; struct
Cyc_List_List* _temp2022; struct Cyc_List_List* _temp2024; struct _tuple0*
_temp2026; _LL1903: if(( unsigned int) _temp1901 > 2u?*(( int*) _temp1901) ==
Cyc_Absyn_Var_p: 0){ _LL1938: _temp1937=(( struct Cyc_Absyn_Var_p_struct*)
_temp1901)->f1; _temp1939=* _temp1937; _LL1953: _temp1952=( void*) _temp1939.sc;
goto _LL1951; _LL1951: _temp1950= _temp1939.name; goto _LL1949; _LL1949:
_temp1948= _temp1939.tq; goto _LL1947; _LL1947: _temp1946=( void*) _temp1939.type;
goto _LL1945; _LL1945: _temp1944= _temp1939.initializer; goto _LL1943; _LL1943:
_temp1942= _temp1939.rgn; goto _LL1941; _LL1941: _temp1940= _temp1939.attributes;
goto _LL1904;} else{ goto _LL1905;} _LL1905: if( _temp1901 ==( void*) Cyc_Absyn_Wild_p){
goto _LL1906;} else{ goto _LL1907;} _LL1907: if(( unsigned int) _temp1901 > 2u?*((
int*) _temp1901) == Cyc_Absyn_Reference_p: 0){ _LL1955: _temp1954=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1901)->f1; goto _LL1908;} else{ goto _LL1909;} _LL1909: if( _temp1901 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1910;} else{ goto _LL1911;} _LL1911: if((
unsigned int) _temp1901 > 2u?*(( int*) _temp1901) == Cyc_Absyn_Int_p: 0){
_LL1959: _temp1958=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1901)->f1;
goto _LL1957; _LL1957: _temp1956=(( struct Cyc_Absyn_Int_p_struct*) _temp1901)->f2;
goto _LL1912;} else{ goto _LL1913;} _LL1913: if(( unsigned int) _temp1901 > 2u?*((
int*) _temp1901) == Cyc_Absyn_Char_p: 0){ _LL1961: _temp1960=(( struct Cyc_Absyn_Char_p_struct*)
_temp1901)->f1; goto _LL1914;} else{ goto _LL1915;} _LL1915: if(( unsigned int)
_temp1901 > 2u?*(( int*) _temp1901) == Cyc_Absyn_Float_p: 0){ _LL1963: _temp1962=((
struct Cyc_Absyn_Float_p_struct*) _temp1901)->f1; goto _LL1916;} else{ goto
_LL1917;} _LL1917: if(( unsigned int) _temp1901 > 2u?*(( int*) _temp1901) == Cyc_Absyn_Enum_p:
0){ _LL1967: _temp1966=(( struct Cyc_Absyn_Enum_p_struct*) _temp1901)->f1; goto
_LL1965; _LL1965: _temp1964=(( struct Cyc_Absyn_Enum_p_struct*) _temp1901)->f2;
goto _LL1918;} else{ goto _LL1919;} _LL1919: if(( unsigned int) _temp1901 > 2u?*((
int*) _temp1901) == Cyc_Absyn_Tunion_p: 0){ _LL1975: _temp1974=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1901)->f1; goto _LL1973; _LL1973: _temp1972=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1901)->f2; goto _LL1971; _LL1971: _temp1970=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1901)->f3; goto _LL1969; _LL1969: _temp1968=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1901)->f4; if( _temp1968 == 0){ goto _LL1920;} else{ goto _LL1921;}} else{
goto _LL1921;} _LL1921: if(( unsigned int) _temp1901 > 2u?*(( int*) _temp1901)
== Cyc_Absyn_Pointer_p: 0){ _LL1978: _temp1977=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1901)->f1; _temp1979=* _temp1977; _LL1985: _temp1984=( void*) _temp1979.r;
if(( unsigned int) _temp1984 > 2u?*(( int*) _temp1984) == Cyc_Absyn_Tunion_p: 0){
_LL1993: _temp1992=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1984)->f1; goto
_LL1991; _LL1991: _temp1990=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1984)->f2;
goto _LL1989; _LL1989: _temp1988=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1984)->f3;
goto _LL1987; _LL1987: _temp1986=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1984)->f4;
goto _LL1983;} else{ goto _LL1923;} _LL1983: _temp1982= _temp1979.topt; goto
_LL1981; _LL1981: _temp1980= _temp1979.loc; goto _LL1976;} else{ goto _LL1923;}
_LL1976: if( _temp1986 != 0){ goto _LL1922;} else{ goto _LL1923;} _LL1923: if((
unsigned int) _temp1901 > 2u?*(( int*) _temp1901) == Cyc_Absyn_Tunion_p: 0){
_LL2001: _temp2000=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1901)->f1; goto
_LL1999; _LL1999: _temp1998=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1901)->f2;
goto _LL1997; _LL1997: _temp1996=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1901)->f3;
goto _LL1995; _LL1995: _temp1994=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1901)->f4;
goto _LL1924;} else{ goto _LL1925;} _LL1925: if(( unsigned int) _temp1901 > 2u?*((
int*) _temp1901) == Cyc_Absyn_Tuple_p: 0){ _LL2003: _temp2002=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1901)->f1; goto _LL1926;} else{ goto _LL1927;} _LL1927: if(( unsigned int)
_temp1901 > 2u?*(( int*) _temp1901) == Cyc_Absyn_Struct_p: 0){ _LL2011:
_temp2010=(( struct Cyc_Absyn_Struct_p_struct*) _temp1901)->f1; goto _LL2009;
_LL2009: _temp2008=(( struct Cyc_Absyn_Struct_p_struct*) _temp1901)->f2; goto
_LL2007; _LL2007: _temp2006=(( struct Cyc_Absyn_Struct_p_struct*) _temp1901)->f3;
goto _LL2005; _LL2005: _temp2004=(( struct Cyc_Absyn_Struct_p_struct*) _temp1901)->f4;
goto _LL1928;} else{ goto _LL1929;} _LL1929: if(( unsigned int) _temp1901 > 2u?*((
int*) _temp1901) == Cyc_Absyn_Pointer_p: 0){ _LL2013: _temp2012=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1901)->f1; goto _LL1930;} else{ goto _LL1931;} _LL1931: if(( unsigned int)
_temp1901 > 2u?*(( int*) _temp1901) == Cyc_Absyn_UnknownId_p: 0){ _LL2015:
_temp2014=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1901)->f1; goto _LL1932;}
else{ goto _LL1933;} _LL1933: if(( unsigned int) _temp1901 > 2u?*(( int*)
_temp1901) == Cyc_Absyn_UnknownCall_p: 0){ _LL2021: _temp2020=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1901)->f1; goto _LL2019; _LL2019: _temp2018=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1901)->f2; goto _LL2017; _LL2017: _temp2016=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1901)->f3; goto _LL1934;} else{ goto _LL1935;} _LL1935: if(( unsigned int)
_temp1901 > 2u?*(( int*) _temp1901) == Cyc_Absyn_UnknownFields_p: 0){ _LL2027:
_temp2026=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1901)->f1; goto
_LL2025; _LL2025: _temp2024=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1901)->f2; goto _LL2023; _LL2023: _temp2022=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1901)->f3; goto _LL1936;} else{ goto _LL1902;} _LL1904: nv= Cyc_Toc_add_varmap(
nv, _temp1950, r); goto _LL1906; _LL1906: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL1902; _LL1908: { struct _tuple0* _temp2028= Cyc_Toc_temp_var(); decls=({
struct Cyc_List_List* _temp2029=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2029->hd=( void*)({ struct _tuple13*
_temp2030=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2030->f1= _temp2028; _temp2030->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp2030;}); _temp2029->tl= decls; _temp2029;}); nv= Cyc_Toc_add_varmap(
nv, _temp1954->name, Cyc_Absyn_var_exp( _temp2028, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2028, 0), Cyc_Absyn_address_exp(
path, 0), 0), Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1902;} _LL1910: s=
Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab);
goto _LL1902; _LL1912: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp(
_temp1958, _temp1956, 0), succ_lab, fail_lab); goto _LL1902; _LL1914: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_char_exp( _temp1960, 0), succ_lab, fail_lab); goto _LL1902; _LL1916:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp( _temp1962, 0), succ_lab,
fail_lab); goto _LL1902; _LL1918: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2031=( struct Cyc_Absyn_Enum_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct)); _temp2031[ 0]=({ struct Cyc_Absyn_Enum_e_struct
_temp2032; _temp2032.tag= Cyc_Absyn_Enum_e; _temp2032.f1= _temp1964->name;
_temp2032.f2=( struct Cyc_Absyn_Enumdecl*) _temp1966; _temp2032.f3=( struct Cyc_Absyn_Enumfield*)
_temp1964; _temp2032;}); _temp2031;}), 0), succ_lab, fail_lab); goto _LL1902;
_LL1920: { struct Cyc_Absyn_Exp* cmp_exp= Cyc_Absyn_var_exp( _temp1972->name, 0);
if( ! _temp1974->is_xtunion){ cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt( r, cmp_exp, succ_lab, fail_lab); goto
_LL1902;} _LL1922: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1986); struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag(
_temp1990->name, _tag_arr("_struct", sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq),
r, 0); struct Cyc_List_List* _temp2033=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1990->typs);
struct Cyc_List_List* _temp2034=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1986); for(
0; _temp2034 != 0;((( _temp2034=(( struct Cyc_List_List*) _check_null( _temp2034))->tl,
_temp2033=(( struct Cyc_List_List*) _check_null( _temp2033))->tl)), cnt --)){
struct Cyc_Absyn_Pat* _temp2035=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2034))->hd; void* _temp2036=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2033))->hd)).f2; struct _tuple0* _temp2037= Cyc_Toc_temp_var();
void* _temp2038=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2035->topt))->v;
void* _temp2039= Cyc_Toc_typ_to_c( _temp2038); struct _tagged_arr* _temp2040=
Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp* _temp2041= Cyc_Absyn_structarrow_exp(
rcast, Cyc_Toc_fieldname( cnt), 0); if( Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c(
_temp2036))){ _temp2041= Cyc_Absyn_cast_exp( _temp2039, _temp2041, 0);} decls=({
struct Cyc_List_List* _temp2042=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2042->hd=( void*)({ struct _tuple13*
_temp2043=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2043->f1= _temp2037; _temp2043->f2= _temp2039; _temp2043;}); _temp2042->tl=
decls; _temp2042;});{ struct _tuple12 _temp2044= Cyc_Toc_xlate_pat( nv, rgn,
_temp2038, Cyc_Absyn_var_exp( _temp2037, 0), _temp2041, _temp2035, succ_lab,
fail_lab, decls); nv= _temp2044.f1; decls= _temp2044.f2;{ struct Cyc_Absyn_Stmt*
_temp2045= _temp2044.f3; struct Cyc_Absyn_Stmt* _temp2046= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2037, 0), _temp2041, 0); struct Cyc_Absyn_Stmt*
_temp2047= Cyc_Absyn_seq_stmt( _temp2046, _temp2045, 0); ss=({ struct Cyc_List_List*
_temp2048=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2048->hd=( void*) Cyc_Absyn_label_stmt( _temp2040, _temp2047, 0); _temp2048->tl=
ss; _temp2048;}); succ_lab= _temp2040;}}}{ struct Cyc_Absyn_Exp* test_exp; if(
_temp1992->is_xtunion){ struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp(
e2, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1990->name, 0);
test_exp= Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e, 0), Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);}
else{ unsigned int max_tag= 0;{ struct Cyc_List_List* _temp2049=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1992->fields))->v; for( 0; _temp2049 !=
0; _temp2049=(( struct Cyc_List_List*) _check_null( _temp2049))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2050=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2049))->hd; if( _temp2050->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1990->name, 0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1902;}} _LL1924: _temp2002= _temp1994; goto _LL1926; _LL1926: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2002);{ struct Cyc_List_List* _temp2051=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2002); for(
0; _temp2051 != 0;( _temp2051=(( struct Cyc_List_List*) _check_null( _temp2051))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp2052=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2051))->hd; struct _tuple0* _temp2053= Cyc_Toc_temp_var();
void* _temp2054=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2052->topt))->v;
struct _tagged_arr* _temp2055= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2056=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2056->hd=( void*)({ struct _tuple13* _temp2057=( struct _tuple13*)
_region_malloc( rgn, sizeof( struct _tuple13)); _temp2057->f1= _temp2053;
_temp2057->f2= Cyc_Toc_typ_to_c( _temp2054); _temp2057;}); _temp2056->tl= decls;
_temp2056;});{ struct _tuple12 _temp2058= Cyc_Toc_xlate_pat( nv, rgn, _temp2054,
Cyc_Absyn_var_exp( _temp2053, 0), Cyc_Absyn_structmember_exp( path, Cyc_Toc_fieldname(
cnt), 0), _temp2052, succ_lab, fail_lab, decls); nv= _temp2058.f1; decls=
_temp2058.f2;{ struct Cyc_Absyn_Stmt* _temp2059= _temp2058.f3; struct Cyc_Absyn_Stmt*
_temp2060= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2053, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2059, 0); ss=({ struct Cyc_List_List* _temp2061=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2061->hd=( void*) Cyc_Absyn_label_stmt(
_temp2055, _temp2060, 0); _temp2061->tl= ss; _temp2061;}); succ_lab= _temp2055;}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL1902;} _LL1928: { struct Cyc_List_List*
ss= 0;{ struct Cyc_List_List* _temp2062=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2004); for(
0; _temp2062 != 0; _temp2062=(( struct Cyc_List_List*) _check_null( _temp2062))->tl){
struct _tuple14* _temp2063=( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp2062))->hd; struct Cyc_Absyn_Pat* _temp2064=(* _temp2063).f2;
struct _tagged_arr* f;{ void* _temp2065=( void*)(( struct Cyc_List_List*)
_check_null((* _temp2063).f1))->hd; struct _tagged_arr* _temp2071; _LL2067: if(*((
int*) _temp2065) == Cyc_Absyn_FieldName){ _LL2072: _temp2071=(( struct Cyc_Absyn_FieldName_struct*)
_temp2065)->f1; goto _LL2068;} else{ goto _LL2069;} _LL2069: goto _LL2070;
_LL2068: f= _temp2071; goto _LL2066; _LL2070:( int) _throw(( void*) Cyc_Toc_Match_error);
_LL2066:;}{ struct _tuple0* _temp2073= Cyc_Toc_temp_var(); void* _temp2074=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp2064->topt))->v; void*
_temp2075= Cyc_Toc_typ_to_c( _temp2074); struct _tagged_arr* _temp2076= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp2077=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2077->hd=( void*)({ struct _tuple13*
_temp2078=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2078->f1= _temp2073; _temp2078->f2= _temp2075; _temp2078;}); _temp2077->tl=
decls; _temp2077;});{ struct _tuple12 _temp2079= Cyc_Toc_xlate_pat( nv, rgn,
_temp2074, Cyc_Absyn_var_exp( _temp2073, 0), Cyc_Absyn_structmember_exp( path, f,
0), _temp2064, succ_lab, fail_lab, decls); nv= _temp2079.f1; decls= _temp2079.f2;{
struct Cyc_Absyn_Exp* _temp2080= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2010->fields))->v,
f)))->type)){ _temp2080= Cyc_Absyn_cast_exp( _temp2075, _temp2080, 0);}{ struct
Cyc_Absyn_Stmt* _temp2081= _temp2079.f3; struct Cyc_Absyn_Stmt* _temp2082= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2073, 0), _temp2080, 0),
_temp2081, 0); ss=({ struct Cyc_List_List* _temp2083=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2083->hd=( void*) Cyc_Absyn_label_stmt(
_temp2076, _temp2082, 0); _temp2083->tl= ss; _temp2083;}); succ_lab= _temp2076;}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL1902;} _LL1930: { struct _tuple0*
_temp2084= Cyc_Toc_temp_var(); void* _temp2085=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2012->topt))->v; decls=({ struct Cyc_List_List* _temp2086=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2086->hd=( void*)({ struct _tuple13* _temp2087=( struct _tuple13*)
_region_malloc( rgn, sizeof( struct _tuple13)); _temp2087->f1= _temp2084;
_temp2087->f2= Cyc_Toc_typ_to_c( _temp2085); _temp2087;}); _temp2086->tl= decls;
_temp2086;});{ struct _tuple12 _temp2088= Cyc_Toc_xlate_pat( nv, rgn, _temp2085,
Cyc_Absyn_var_exp( _temp2084, 0), Cyc_Absyn_deref_exp( path, 0), _temp2012,
succ_lab, fail_lab, decls); nv= _temp2088.f1; decls= _temp2088.f2;{ struct Cyc_Absyn_Stmt*
_temp2089= _temp2088.f3; struct Cyc_Absyn_Stmt* _temp2090= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2084, 0), Cyc_Absyn_deref_exp( r,
0), 0), _temp2089, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2090, 0);} else{ s= _temp2090;} goto _LL1902;}}} _LL1932: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid",
sizeof( unsigned char), 10u)); goto _LL1902; _LL1934: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall", sizeof(
unsigned char), 12u)); goto _LL1902; _LL1936: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields", sizeof(
unsigned char), 14u)); goto _LL1902; _LL1902:;} return({ struct _tuple12
_temp2091; _temp2091.f1= nv; _temp2091.f2= decls; _temp2091.f3= s; _temp2091;});}
struct _tuple15{ struct _tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple15* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple15* _temp2092=(
struct _tuple15*) _region_malloc( r, sizeof( struct _tuple15)); _temp2092->f1=
Cyc_Toc_fresh_label(); _temp2092->f2= Cyc_Toc_fresh_label(); _temp2092->f3= sc;
_temp2092;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp2093=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; int leave_as_switch;{ void* _temp2094= Cyc_Tcutil_compress(
_temp2093); void* _temp2100; void* _temp2102; _LL2096: if(( unsigned int)
_temp2094 > 4u?*(( int*) _temp2094) == Cyc_Absyn_IntType: 0){ _LL2103: _temp2102=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2094)->f1; goto _LL2101; _LL2101:
_temp2100=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2094)->f2; goto
_LL2097;} else{ goto _LL2098;} _LL2098: goto _LL2099; _LL2097: leave_as_switch=
1; goto _LL2095; _LL2099: leave_as_switch= 0; goto _LL2095; _LL2095:;}{ struct
Cyc_List_List* _temp2104= scs; for( 0; _temp2104 != 0; _temp2104=(( struct Cyc_List_List*)
_check_null( _temp2104))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2104))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2104))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2105= scs; for( 0; _temp2105
!= 0; _temp2105=(( struct Cyc_List_List*) _check_null( _temp2105))->tl){ struct
Cyc_Absyn_Stmt* _temp2106=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2105))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp2105))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp2106, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp2106);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp2107= _new_region(); struct _RegionHandle* rgn=&
_temp2107; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple15*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2108= lscs; for( 0;
_temp2108 != 0; _temp2108=(( struct Cyc_List_List*) _check_null( _temp2108))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp2108))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp2108))->tl == 0? end_l:(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp2108))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp2108))->hd)).f2; if( sc->where_clause == 0){ struct _tuple12
_temp2109= Cyc_Toc_xlate_pat( nv, rgn, _temp2093, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp2110=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2110->hd=( void*)
_temp2109.f1; _temp2110->tl= nvs; _temp2110;}); decls= _temp2109.f2; test_stmts=({
struct Cyc_List_List* _temp2111=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2111->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2108))->hd)).f1,
_temp2109.f3, 0); _temp2111->tl= test_stmts; _temp2111;});} else{ struct Cyc_Absyn_Exp*
_temp2112=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp2113= Cyc_Toc_fresh_label(); struct _tuple12 _temp2114= Cyc_Toc_xlate_pat(
nv, rgn, _temp2093, r, path, sc->pattern, _temp2113, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp2114.f1, _temp2112); nvs=({ struct Cyc_List_List* _temp2115=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2115->hd=( void*)
_temp2114.f1; _temp2115->tl= nvs; _temp2115;}); decls= _temp2114.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2112, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2113, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2116=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2116->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2108))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2114.f3, s3, 0), 0); _temp2116->tl= test_stmts;
_temp2116;});}}}}{ struct Cyc_Absyn_Stmt* _temp2117= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl != 0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2118=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp2118, end_l,(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp2119=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp2119, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp2117, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls != 0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple13 _temp2122; void* _temp2123; struct
_tuple0* _temp2125; struct _tuple13* _temp2120=( struct _tuple13*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp2122=* _temp2120; _LL2126: _temp2125= _temp2122.f1;
goto _LL2124; _LL2124: _temp2123= _temp2122.f2; goto _LL2121; _LL2121: res= Cyc_Absyn_declare_stmt(
_temp2125, _temp2123, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp2127=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2127[ 0]=({ struct Cyc_List_List
_temp2128; _temp2128.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp2128.tl= 0; _temp2128;}); _temp2127;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2129=( void*) s->r; struct Cyc_Absyn_Exp*
_temp2171; struct Cyc_Absyn_Stmt* _temp2173; struct Cyc_Absyn_Stmt* _temp2175;
struct Cyc_Absyn_Exp* _temp2177; struct Cyc_Absyn_Stmt* _temp2179; struct Cyc_Absyn_Stmt*
_temp2181; struct Cyc_Absyn_Exp* _temp2183; struct Cyc_Absyn_Stmt* _temp2185;
struct _tuple2 _temp2187; struct Cyc_Absyn_Stmt* _temp2189; struct Cyc_Absyn_Exp*
_temp2191; struct Cyc_Absyn_Stmt* _temp2193; struct Cyc_Absyn_Stmt* _temp2195;
struct Cyc_Absyn_Stmt* _temp2197; struct _tagged_arr* _temp2199; struct Cyc_Absyn_Stmt*
_temp2201; struct _tuple2 _temp2203; struct Cyc_Absyn_Stmt* _temp2205; struct
Cyc_Absyn_Exp* _temp2207; struct _tuple2 _temp2209; struct Cyc_Absyn_Stmt*
_temp2211; struct Cyc_Absyn_Exp* _temp2213; struct Cyc_Absyn_Exp* _temp2215;
struct Cyc_List_List* _temp2217; struct Cyc_Absyn_Exp* _temp2219; struct Cyc_Absyn_Switch_clause**
_temp2221; struct Cyc_List_List* _temp2223; struct Cyc_Absyn_Stmt* _temp2225;
struct Cyc_Absyn_Decl* _temp2227; struct Cyc_Absyn_Stmt* _temp2229; struct
_tagged_arr* _temp2231; struct _tuple2 _temp2233; struct Cyc_Absyn_Stmt*
_temp2235; struct Cyc_Absyn_Exp* _temp2237; struct Cyc_Absyn_Stmt* _temp2239;
struct Cyc_List_List* _temp2241; struct Cyc_Absyn_Stmt* _temp2243; struct Cyc_Absyn_Stmt*
_temp2245; struct Cyc_Absyn_Vardecl* _temp2247; struct Cyc_Absyn_Tvar* _temp2249;
struct Cyc_List_List* _temp2251; struct Cyc_Absyn_Exp* _temp2253; struct Cyc_Absyn_Stmt*
_temp2255; struct Cyc_Absyn_Stmt* _temp2257; _LL2131: if( _temp2129 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2132;} else{ goto _LL2133;} _LL2133: if((
unsigned int) _temp2129 > 1u?*(( int*) _temp2129) == Cyc_Absyn_Exp_s: 0){
_LL2172: _temp2171=(( struct Cyc_Absyn_Exp_s_struct*) _temp2129)->f1; goto
_LL2134;} else{ goto _LL2135;} _LL2135: if(( unsigned int) _temp2129 > 1u?*((
int*) _temp2129) == Cyc_Absyn_Seq_s: 0){ _LL2176: _temp2175=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2129)->f1; goto _LL2174; _LL2174: _temp2173=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2129)->f2; goto _LL2136;} else{ goto _LL2137;} _LL2137: if(( unsigned int)
_temp2129 > 1u?*(( int*) _temp2129) == Cyc_Absyn_Return_s: 0){ _LL2178:
_temp2177=(( struct Cyc_Absyn_Return_s_struct*) _temp2129)->f1; goto _LL2138;}
else{ goto _LL2139;} _LL2139: if(( unsigned int) _temp2129 > 1u?*(( int*)
_temp2129) == Cyc_Absyn_IfThenElse_s: 0){ _LL2184: _temp2183=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2129)->f1; goto _LL2182; _LL2182: _temp2181=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2129)->f2; goto _LL2180; _LL2180: _temp2179=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2129)->f3; goto _LL2140;} else{ goto _LL2141;} _LL2141: if(( unsigned int)
_temp2129 > 1u?*(( int*) _temp2129) == Cyc_Absyn_While_s: 0){ _LL2188: _temp2187=((
struct Cyc_Absyn_While_s_struct*) _temp2129)->f1; _LL2192: _temp2191= _temp2187.f1;
goto _LL2190; _LL2190: _temp2189= _temp2187.f2; goto _LL2186; _LL2186: _temp2185=((
struct Cyc_Absyn_While_s_struct*) _temp2129)->f2; goto _LL2142;} else{ goto
_LL2143;} _LL2143: if(( unsigned int) _temp2129 > 1u?*(( int*) _temp2129) == Cyc_Absyn_Break_s:
0){ _LL2194: _temp2193=(( struct Cyc_Absyn_Break_s_struct*) _temp2129)->f1; goto
_LL2144;} else{ goto _LL2145;} _LL2145: if(( unsigned int) _temp2129 > 1u?*((
int*) _temp2129) == Cyc_Absyn_Continue_s: 0){ _LL2196: _temp2195=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2129)->f1; goto _LL2146;} else{ goto _LL2147;} _LL2147: if(( unsigned int)
_temp2129 > 1u?*(( int*) _temp2129) == Cyc_Absyn_Goto_s: 0){ _LL2200: _temp2199=((
struct Cyc_Absyn_Goto_s_struct*) _temp2129)->f1; goto _LL2198; _LL2198:
_temp2197=(( struct Cyc_Absyn_Goto_s_struct*) _temp2129)->f2; goto _LL2148;}
else{ goto _LL2149;} _LL2149: if(( unsigned int) _temp2129 > 1u?*(( int*)
_temp2129) == Cyc_Absyn_For_s: 0){ _LL2216: _temp2215=(( struct Cyc_Absyn_For_s_struct*)
_temp2129)->f1; goto _LL2210; _LL2210: _temp2209=(( struct Cyc_Absyn_For_s_struct*)
_temp2129)->f2; _LL2214: _temp2213= _temp2209.f1; goto _LL2212; _LL2212:
_temp2211= _temp2209.f2; goto _LL2204; _LL2204: _temp2203=(( struct Cyc_Absyn_For_s_struct*)
_temp2129)->f3; _LL2208: _temp2207= _temp2203.f1; goto _LL2206; _LL2206:
_temp2205= _temp2203.f2; goto _LL2202; _LL2202: _temp2201=(( struct Cyc_Absyn_For_s_struct*)
_temp2129)->f4; goto _LL2150;} else{ goto _LL2151;} _LL2151: if(( unsigned int)
_temp2129 > 1u?*(( int*) _temp2129) == Cyc_Absyn_Switch_s: 0){ _LL2220:
_temp2219=(( struct Cyc_Absyn_Switch_s_struct*) _temp2129)->f1; goto _LL2218;
_LL2218: _temp2217=(( struct Cyc_Absyn_Switch_s_struct*) _temp2129)->f2; goto
_LL2152;} else{ goto _LL2153;} _LL2153: if(( unsigned int) _temp2129 > 1u?*((
int*) _temp2129) == Cyc_Absyn_Fallthru_s: 0){ _LL2224: _temp2223=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2129)->f1; goto _LL2222; _LL2222: _temp2221=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2129)->f2; goto _LL2154;} else{ goto _LL2155;} _LL2155: if(( unsigned int)
_temp2129 > 1u?*(( int*) _temp2129) == Cyc_Absyn_Decl_s: 0){ _LL2228: _temp2227=((
struct Cyc_Absyn_Decl_s_struct*) _temp2129)->f1; goto _LL2226; _LL2226:
_temp2225=(( struct Cyc_Absyn_Decl_s_struct*) _temp2129)->f2; goto _LL2156;}
else{ goto _LL2157;} _LL2157: if(( unsigned int) _temp2129 > 1u?*(( int*)
_temp2129) == Cyc_Absyn_Label_s: 0){ _LL2232: _temp2231=(( struct Cyc_Absyn_Label_s_struct*)
_temp2129)->f1; goto _LL2230; _LL2230: _temp2229=(( struct Cyc_Absyn_Label_s_struct*)
_temp2129)->f2; goto _LL2158;} else{ goto _LL2159;} _LL2159: if(( unsigned int)
_temp2129 > 1u?*(( int*) _temp2129) == Cyc_Absyn_Do_s: 0){ _LL2240: _temp2239=((
struct Cyc_Absyn_Do_s_struct*) _temp2129)->f1; goto _LL2234; _LL2234: _temp2233=((
struct Cyc_Absyn_Do_s_struct*) _temp2129)->f2; _LL2238: _temp2237= _temp2233.f1;
goto _LL2236; _LL2236: _temp2235= _temp2233.f2; goto _LL2160;} else{ goto
_LL2161;} _LL2161: if(( unsigned int) _temp2129 > 1u?*(( int*) _temp2129) == Cyc_Absyn_TryCatch_s:
0){ _LL2244: _temp2243=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2129)->f1;
goto _LL2242; _LL2242: _temp2241=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2129)->f2; goto _LL2162;} else{ goto _LL2163;} _LL2163: if(( unsigned int)
_temp2129 > 1u?*(( int*) _temp2129) == Cyc_Absyn_Region_s: 0){ _LL2250:
_temp2249=(( struct Cyc_Absyn_Region_s_struct*) _temp2129)->f1; goto _LL2248;
_LL2248: _temp2247=(( struct Cyc_Absyn_Region_s_struct*) _temp2129)->f2; goto
_LL2246; _LL2246: _temp2245=(( struct Cyc_Absyn_Region_s_struct*) _temp2129)->f3;
goto _LL2164;} else{ goto _LL2165;} _LL2165: if(( unsigned int) _temp2129 > 1u?*((
int*) _temp2129) == Cyc_Absyn_SwitchC_s: 0){ _LL2254: _temp2253=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2129)->f1; goto _LL2252; _LL2252: _temp2251=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2129)->f2; goto _LL2166;} else{ goto _LL2167;} _LL2167: if(( unsigned int)
_temp2129 > 1u?*(( int*) _temp2129) == Cyc_Absyn_Cut_s: 0){ _LL2256: _temp2255=((
struct Cyc_Absyn_Cut_s_struct*) _temp2129)->f1; goto _LL2168;} else{ goto
_LL2169;} _LL2169: if(( unsigned int) _temp2129 > 1u?*(( int*) _temp2129) == Cyc_Absyn_Splice_s:
0){ _LL2258: _temp2257=(( struct Cyc_Absyn_Splice_s_struct*) _temp2129)->f1;
goto _LL2170;} else{ goto _LL2130;} _LL2132: return; _LL2134: Cyc_Toc_exp_to_c(
nv, _temp2171); return; _LL2136: Cyc_Toc_stmt_to_c( nv, _temp2175); s= _temp2173;
continue; _LL2138: { struct Cyc_Core_Opt* topt= 0; if( _temp2177 != 0){ topt=({
struct Cyc_Core_Opt* _temp2259=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2259->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2177))->topt))->v);
_temp2259;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2177));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2260=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp2261= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2260, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp2260,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp2177, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp2261, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL2140: Cyc_Toc_exp_to_c( nv, _temp2183); Cyc_Toc_stmt_to_c( nv,
_temp2181); s= _temp2179; continue; _LL2142: Cyc_Toc_exp_to_c( nv, _temp2191);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2185); return; _LL2144: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{ int
dest_depth= _temp2193 == 0? 0:(( struct Cyc_Absyn_Stmt*) _check_null( _temp2193))->try_depth;
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL2146: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp2197=
_temp2195; goto _LL2148; _LL2148: Cyc_Toc_do_npop_before( s->try_depth -((
struct Cyc_Absyn_Stmt*) _check_null( _temp2197))->try_depth, s); return; _LL2150:
Cyc_Toc_exp_to_c( nv, _temp2215); Cyc_Toc_exp_to_c( nv, _temp2213); Cyc_Toc_exp_to_c(
nv, _temp2207); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2201); return;
_LL2152: Cyc_Toc_xlate_switch( nv, s, _temp2219, _temp2217); return; _LL2154:
if( nv->fallthru_info == 0){( int) _throw((( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("fallthru in unexpected place", sizeof(
unsigned char), 29u)));}{ struct _tuple6 _temp2264; struct Cyc_Dict_Dict*
_temp2265; struct Cyc_List_List* _temp2267; struct _tagged_arr* _temp2269;
struct _tuple6* _temp2262=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp2264=* _temp2262; _LL2270: _temp2269= _temp2264.f1;
goto _LL2268; _LL2268: _temp2267= _temp2264.f2; goto _LL2266; _LL2266: _temp2265=
_temp2264.f3; goto _LL2263; _LL2263: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp2269, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp2221)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2271=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2267); struct Cyc_List_List* _temp2272=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp2223); for( 0; _temp2271 != 0;( _temp2271=((
struct Cyc_List_List*) _check_null( _temp2271))->tl, _temp2272=(( struct Cyc_List_List*)
_check_null( _temp2272))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp2272))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp2265,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp2271))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp2272))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}} _LL2156:{ void* _temp2273=( void*) _temp2227->r;
struct Cyc_Absyn_Vardecl* _temp2283; int _temp2285; struct Cyc_Absyn_Exp*
_temp2287; struct Cyc_Core_Opt* _temp2289; struct Cyc_Core_Opt* _temp2291;
struct Cyc_Absyn_Pat* _temp2293; struct Cyc_List_List* _temp2295; _LL2275: if(*((
int*) _temp2273) == Cyc_Absyn_Var_d){ _LL2284: _temp2283=(( struct Cyc_Absyn_Var_d_struct*)
_temp2273)->f1; goto _LL2276;} else{ goto _LL2277;} _LL2277: if(*(( int*)
_temp2273) == Cyc_Absyn_Let_d){ _LL2294: _temp2293=(( struct Cyc_Absyn_Let_d_struct*)
_temp2273)->f1; goto _LL2292; _LL2292: _temp2291=(( struct Cyc_Absyn_Let_d_struct*)
_temp2273)->f2; goto _LL2290; _LL2290: _temp2289=(( struct Cyc_Absyn_Let_d_struct*)
_temp2273)->f3; goto _LL2288; _LL2288: _temp2287=(( struct Cyc_Absyn_Let_d_struct*)
_temp2273)->f4; goto _LL2286; _LL2286: _temp2285=(( struct Cyc_Absyn_Let_d_struct*)
_temp2273)->f5; goto _LL2278;} else{ goto _LL2279;} _LL2279: if(*(( int*)
_temp2273) == Cyc_Absyn_Letv_d){ _LL2296: _temp2295=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2273)->f1; goto _LL2280;} else{ goto _LL2281;} _LL2281: goto _LL2282;
_LL2276: { struct Cyc_Toc_Env* _temp2297= Cyc_Toc_add_varmap( nv, _temp2283->name,
Cyc_Absyn_varb_exp( _temp2283->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2298=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2298[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2299; _temp2299.tag= Cyc_Absyn_Local_b;
_temp2299.f1= _temp2283; _temp2299;}); _temp2298;}), 0)); Cyc_Toc_local_decl_to_c(
_temp2297, _temp2297, _temp2283, _temp2225); goto _LL2274;} _LL2278:{ void*
_temp2300=( void*) _temp2293->r; struct Cyc_Absyn_Vardecl* _temp2306; _LL2302:
if(( unsigned int) _temp2300 > 2u?*(( int*) _temp2300) == Cyc_Absyn_Var_p: 0){
_LL2307: _temp2306=(( struct Cyc_Absyn_Var_p_struct*) _temp2300)->f1; goto
_LL2303;} else{ goto _LL2304;} _LL2304: goto _LL2305; _LL2303: { struct _tuple0*
old_name= _temp2306->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp2306->name= new_name; _temp2306->initializer=( struct Cyc_Absyn_Exp*)
_temp2287;( void*)( _temp2227->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2308=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2308[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2309; _temp2309.tag= Cyc_Absyn_Var_d;
_temp2309.f1= _temp2306; _temp2309;}); _temp2308;})));{ struct Cyc_Toc_Env*
_temp2310= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2311=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2311[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2312; _temp2312.tag= Cyc_Absyn_Local_b; _temp2312.f1=
_temp2306; _temp2312;}); _temp2311;}), 0)); Cyc_Toc_local_decl_to_c( _temp2310,
nv, _temp2306, _temp2225); goto _LL2301;}} _LL2305:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp2293, _temp2289, _temp2287, _temp2285,
_temp2225))->r)); goto _LL2301; _LL2301:;} goto _LL2274; _LL2280: { struct Cyc_List_List*
_temp2313=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2295); if( _temp2313 == 0){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("empty Letv_d", sizeof( unsigned char), 13u));}( void*)( _temp2227->r=(
void*)(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2314=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2314[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2315; _temp2315.tag= Cyc_Absyn_Var_d; _temp2315.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2313))->hd; _temp2315;}); _temp2314;})));
_temp2313=(( struct Cyc_List_List*) _check_null( _temp2313))->tl; for( 0;
_temp2313 != 0; _temp2313=(( struct Cyc_List_List*) _check_null( _temp2313))->tl){
struct Cyc_Absyn_Decl* _temp2316= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2317=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2317[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2318; _temp2318.tag= Cyc_Absyn_Var_d;
_temp2318.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2313))->hd; _temp2318;}); _temp2317;}), 0);( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_decl_stmt( _temp2316, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));}
Cyc_Toc_stmt_to_c( nv, s); goto _LL2274;} _LL2282:(( int(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u)); goto _LL2274; _LL2274:;} return; _LL2158: s= _temp2229;
continue; _LL2160: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2239); Cyc_Toc_exp_to_c(
nv, _temp2237); return; _LL2162: { struct _tuple0* h_var= Cyc_Toc_temp_var();
struct _tuple0* e_var= Cyc_Toc_temp_var(); struct _tuple0* was_thrown_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp2319=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2319->v=( void*) e_typ; _temp2319;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2243);{
struct Cyc_Absyn_Stmt* _temp2320= Cyc_Absyn_seq_stmt( _temp2243, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp2321= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp2322= Cyc_Absyn_var_exp(
_temp2321, 0); struct Cyc_Absyn_Vardecl* _temp2323= Cyc_Absyn_new_vardecl(
_temp2321, Cyc_Absyn_exn_typ, 0); _temp2322->topt=({ struct Cyc_Core_Opt*
_temp2324=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2324->v=( void*) Cyc_Absyn_exn_typ; _temp2324;});{ struct Cyc_Absyn_Pat*
_temp2325=({ struct Cyc_Absyn_Pat* _temp2341=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2341->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2343=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2343[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2344; _temp2344.tag= Cyc_Absyn_Var_p;
_temp2344.f1= _temp2323; _temp2344;}); _temp2343;})); _temp2341->topt=({ struct
Cyc_Core_Opt* _temp2342=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2342->v=( void*) Cyc_Absyn_exn_typ; _temp2342;}); _temp2341->loc= 0;
_temp2341;}); struct Cyc_Absyn_Exp* _temp2326= Cyc_Absyn_throw_exp( _temp2322, 0);
_temp2326->topt=({ struct Cyc_Core_Opt* _temp2327=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2327->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp2327;});{ struct Cyc_Absyn_Stmt* _temp2328= Cyc_Absyn_exp_stmt( _temp2326,
0); struct Cyc_Absyn_Switch_clause* _temp2329=({ struct Cyc_Absyn_Switch_clause*
_temp2338=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2338->pattern= _temp2325; _temp2338->pat_vars=({ struct Cyc_Core_Opt*
_temp2339=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2339->v=( void*)({ struct Cyc_List_List* _temp2340=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2340->hd=( void*) _temp2323;
_temp2340->tl= 0; _temp2340;}); _temp2339;}); _temp2338->where_clause= 0;
_temp2338->body= _temp2328; _temp2338->loc= 0; _temp2338;}); struct Cyc_Absyn_Stmt*
_temp2330= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2241,({ struct Cyc_List_List*
_temp2337=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2337->hd=( void*) _temp2329; _temp2337->tl= 0; _temp2337;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp2330);{ struct Cyc_Absyn_Exp* _temp2331= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp2336=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2336->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2336->tl= 0; _temp2336;}), 0); struct Cyc_Absyn_Stmt*
_temp2332= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp2335=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2335->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp2335->tl= 0; _temp2335;}), 0), 0); struct Cyc_Absyn_Exp* _temp2333= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp2334= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2332, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2333, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2331, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2334, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2320, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp2330, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL2164: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp2247->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp2245);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_List_List*(* _temp2345)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp2348= x_exp; struct Cyc_Absyn_Exp* _temp2346[ 1u]={ _temp2348}; struct
_tagged_arr _temp2347={( void*) _temp2346,( void*) _temp2346,( void*)( _temp2346
+ 1u)}; _temp2345( _temp2347);}), 0), 0), Cyc_Absyn_seq_stmt( _temp2245, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({ struct Cyc_List_List*(* _temp2349)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp2352= x_exp; struct Cyc_Absyn_Exp* _temp2350[ 1u]={
_temp2352}; struct _tagged_arr _temp2351={( void*) _temp2350,( void*) _temp2350,(
void*)( _temp2350 + 1u)}; _temp2349( _temp2351);}), 0), 0), 0), 0), 0), 0))->r));
return;} _LL2166:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("switchC_s",
sizeof( unsigned char), 10u)); return; _LL2168:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("cut", sizeof( unsigned char), 4u)); return; _LL2170:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof(
unsigned char), 7u)); return; _LL2130:;}} struct _tuple16{ struct _tagged_arr*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0; f->rgn_po=
0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* _temp2353= f->args; for( 0; _temp2353 != 0; _temp2353=((
struct Cyc_List_List*) _check_null( _temp2353))->tl){(*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( _temp2353))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2353))->hd)).f3);{
struct _tuple0* _temp2354=({ struct _tuple0* _temp2355=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2355->f1=( void*) Cyc_Absyn_Loc_n;
_temp2355->f2=(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2353))->hd)).f1; _temp2355;}); nv= Cyc_Toc_add_varmap( nv, _temp2354, Cyc_Absyn_var_exp(
_temp2354, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name != 0: 0){ int _temp2358; void* _temp2360;
void* _temp2362; struct Cyc_Absyn_Tqual _temp2364; struct Cyc_Core_Opt*
_temp2366; struct Cyc_Absyn_VarargInfo _temp2356=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs)); _LL2367: _temp2366= _temp2356.name; goto _LL2365;
_LL2365: _temp2364= _temp2356.tq; goto _LL2363; _LL2363: _temp2362=( void*)
_temp2356.type; goto _LL2361; _LL2361: _temp2360=( void*) _temp2356.rgn; goto
_LL2359; _LL2359: _temp2358= _temp2356.inject; goto _LL2357; _LL2357: { void*
_temp2368= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp2362, _temp2360,
_temp2364)); struct _tuple0* _temp2369=({ struct _tuple0* _temp2372=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2372->f1=( void*) Cyc_Absyn_Loc_n;
_temp2372->f2=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
_temp2366))->v; _temp2372;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2370=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2370->hd=( void*)({ struct _tuple16* _temp2371=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp2371->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp2366))->v; _temp2371->f2= _temp2364;
_temp2371->f3= _temp2368; _temp2371;}); _temp2370->tl= 0; _temp2370;})); nv= Cyc_Toc_add_varmap(
nv, _temp2369, Cyc_Absyn_var_exp( _temp2369, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp2373=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp2373 != 0; _temp2373=(( struct
Cyc_List_List*) _check_null( _temp2373))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2373))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2373))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2374= s; _LL2376: if(
_temp2374 ==( void*) Cyc_Absyn_Abstract){ goto _LL2377;} else{ goto _LL2378;}
_LL2378: if( _temp2374 ==( void*) Cyc_Absyn_ExternC){ goto _LL2379;} else{ goto
_LL2380;} _LL2380: goto _LL2381; _LL2377: return( void*) Cyc_Absyn_Public;
_LL2379: return( void*) Cyc_Absyn_Extern; _LL2381: return s; _LL2375:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2382=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 0){
Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2383=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2383->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2383;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2384=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2382).f2); if( _temp2384 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2382).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2384))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2382).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2385=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2385->v=(
void*) _temp2382; _temp2385;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2386=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp2386 != 0; _temp2386=(( struct Cyc_List_List*)
_check_null( _temp2386))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2386))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2386))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp2387=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far == 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp2388=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2388->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2388;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2389=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2387).f2); if( _temp2389 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2387).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2389))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp2387).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2390=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2390->v=(
void*) _temp2387; _temp2390;}); if( u->fields != 0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp2391=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2391 != 0; _temp2391=(( struct Cyc_List_List*)
_check_null( _temp2391))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2391))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2391))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2392=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2392->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2392;});}{ struct
_tuple0* _temp2393= tud->name; if( tud->fields == 0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp2393)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2393));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp2394=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp2394 != 0; _temp2394=(( struct Cyc_List_List*) _check_null(
_temp2394))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2394))->hd; if( f->typs == 0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2395; _temp2395.q_const= 1;
_temp2395.q_volatile= 0; _temp2395.q_restrict= 0; _temp2395;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2396=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2396->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2397=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2397[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2398; _temp2398.tag= Cyc_Absyn_Var_d; _temp2398.f1= vd; _temp2398;});
_temp2397;}), 0); _temp2396->tl= Cyc_Toc_result_decls; _temp2396;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2399; _temp2399.q_const=
1; _temp2399.q_volatile= 0; _temp2399.q_restrict= 0; _temp2399;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2400=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2400->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2401=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2401[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2402; _temp2402.tag= Cyc_Absyn_Var_d; _temp2402.f1= vd; _temp2402;});
_temp2401;}), 0); _temp2400->tl= Cyc_Toc_result_decls; _temp2400;});{ struct Cyc_List_List*
_temp2403= 0; int i= 1;{ struct Cyc_List_List* _temp2404= f->typs; for( 0;
_temp2404 != 0;( _temp2404=(( struct Cyc_List_List*) _check_null( _temp2404))->tl,
i ++)){ struct _tagged_arr* _temp2405= Cyc_Toc_fieldname( i); struct Cyc_Absyn_Structfield*
_temp2406=({ struct Cyc_Absyn_Structfield* _temp2408=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2408->name= _temp2405;
_temp2408->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2404))->hd)).f1; _temp2408->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2404))->hd)).f2);
_temp2408->width= 0; _temp2408->attributes= 0; _temp2408;}); _temp2403=({ struct
Cyc_List_List* _temp2407=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2407->hd=( void*) _temp2406; _temp2407->tl= _temp2403; _temp2407;});}}
_temp2403=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2403); _temp2403=({ struct Cyc_List_List* _temp2409=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2409->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2410=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2410->name= Cyc_Toc_tag_sp; _temp2410->tq= Cyc_Toc_mt_tq; _temp2410->type=(
void*) Cyc_Absyn_sint_t; _temp2410->width= 0; _temp2410->attributes= 0;
_temp2410;}); _temp2409->tl= _temp2403; _temp2409;});{ struct Cyc_Absyn_Structdecl*
_temp2411=({ struct Cyc_Absyn_Structdecl* _temp2415=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2415->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2415->name=({ struct Cyc_Core_Opt* _temp2417=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2417->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2417;}); _temp2415->tvs= 0; _temp2415->fields=({ struct Cyc_Core_Opt*
_temp2416=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2416->v=( void*) _temp2403; _temp2416;}); _temp2415->attributes= 0;
_temp2415;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2412=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2412->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2413=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2413[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2414; _temp2414.tag= Cyc_Absyn_Struct_d;
_temp2414.f1= _temp2411; _temp2414;}); _temp2413;}), 0); _temp2412->tl= Cyc_Toc_result_decls;
_temp2412;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2418=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2418->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2418;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2419= xd->name; struct Cyc_List_List*
_temp2420=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2420 != 0; _temp2420=(( struct Cyc_List_List*) _check_null(
_temp2420))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2420))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2421= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) + 4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2422=({ struct
Cyc_Absyn_ArrayType_struct* _temp2468=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2468[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2469; _temp2469.tag= Cyc_Absyn_ArrayType;
_temp2469.f1=( void*) Cyc_Absyn_uchar_t; _temp2469.f2= Cyc_Toc_mt_tq; _temp2469.f3=(
struct Cyc_Absyn_Exp*) _temp2421; _temp2469;}); _temp2468;}); struct Cyc_Core_Opt*
_temp2423=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2431; int _temp2432; _LL2425: if( _temp2423 == 0){ goto _LL2426;} else{
goto _LL2427;} _LL2427: if( _temp2423 == 0){ goto _LL2429;} else{ _temp2431=*
_temp2423; _LL2433: _temp2432=( int) _temp2431.v; if( _temp2432 == 0){ goto
_LL2428;} else{ goto _LL2429;}} _LL2429: goto _LL2430; _LL2426: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp(( struct _tagged_arr)({ int _temp2434= 0; int _temp2435= 0;
int _temp2436= 0; int _temp2437= 0; struct _tagged_arr _temp2438=* fn; xprintf("%c%c%c%c%.*s",
_temp2434, _temp2435, _temp2436, _temp2437, _get_arr_size( _temp2438, 1u),
_temp2438.curr);}), 0);}{ struct Cyc_Absyn_Vardecl* _temp2439= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2422, initopt);( void*)( _temp2439->sc=( void*)(( void*) f->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2440=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2440->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2441=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2441[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2442; _temp2442.tag= Cyc_Absyn_Var_d; _temp2442.f1= _temp2439; _temp2442;});
_temp2441;}), 0); _temp2440->tl= Cyc_Toc_result_decls; _temp2440;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*) Cyc_Absyn_Extern));
if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2443= f->typs; for( 0; _temp2443 != 0;( _temp2443=(( struct Cyc_List_List*)
_check_null( _temp2443))->tl, i ++)){ struct _tagged_arr* _temp2444=({ struct
_tagged_arr* _temp2448=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2448[ 0]=( struct _tagged_arr) xprintf("f%d", i); _temp2448;});
struct Cyc_Absyn_Structfield* _temp2445=({ struct Cyc_Absyn_Structfield*
_temp2447=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2447->name= _temp2444; _temp2447->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2443))->hd)).f1; _temp2447->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2443))->hd)).f2);
_temp2447->width= 0; _temp2447->attributes= 0; _temp2447;}); fields=({ struct
Cyc_List_List* _temp2446=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2446->hd=( void*) _temp2445; _temp2446->tl= fields; _temp2446;});}} fields=({
struct Cyc_List_List* _temp2449=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2449->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2450=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2450->name= Cyc_Toc_tag_sp; _temp2450->tq= Cyc_Toc_mt_tq; _temp2450->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2450->width=
0; _temp2450->attributes= 0; _temp2450;}); _temp2449->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2449;});{ struct Cyc_Absyn_Structdecl*
_temp2451=({ struct Cyc_Absyn_Structdecl* _temp2455=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2455->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2455->name=({ struct Cyc_Core_Opt* _temp2457=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2457->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2457;}); _temp2455->tvs= 0; _temp2455->fields=({ struct Cyc_Core_Opt*
_temp2456=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2456->v=( void*) fields; _temp2456;}); _temp2455->attributes= 0; _temp2455;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2452=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2452->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2453=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2453[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2454; _temp2454.tag= Cyc_Absyn_Struct_d;
_temp2454.f1= _temp2451; _temp2454;}); _temp2453;}), 0); _temp2452->tl= Cyc_Toc_result_decls;
_temp2452;});}} goto _LL2424;}} _LL2428: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2458= Cyc_Absyn_string_exp(( struct _tagged_arr)({
int _temp2463= 0; int _temp2464= 0; int _temp2465= 0; int _temp2466= 0; struct
_tagged_arr _temp2467=* fn; xprintf("%c%c%c%c%.*s", _temp2463, _temp2464,
_temp2465, _temp2466, _get_arr_size( _temp2467, 1u), _temp2467.curr);}), 0);
struct Cyc_Absyn_Vardecl* _temp2459= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2422,( struct Cyc_Absyn_Exp*) _temp2458);( void*)( _temp2459->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2460=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2460->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2461=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2461[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2462; _temp2462.tag= Cyc_Absyn_Var_d;
_temp2462.f1= _temp2459; _temp2462;}); _temp2461;}), 0); _temp2460->tl= Cyc_Toc_result_decls;
_temp2460;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2424;
_LL2430: goto _LL2424; _LL2424:;}}} static void Cyc_Toc_enumdecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2470= ed->name;(
void*)( ed->sc=( void*)(( void*) Cyc_Absyn_Public)); if( ed->fields != 0){
struct Cyc_List_List* _temp2471=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; _temp2471 != 0; _temp2471=(( struct Cyc_List_List*)
_check_null( _temp2471))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2471))->hd)->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2471))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2471))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer != 0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2472=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2478; struct Cyc_Absyn_Exp* _temp2480; struct Cyc_Absyn_Vardecl* _temp2482;
_LL2474: if(*(( int*) _temp2472) == Cyc_Absyn_Comprehension_e){ _LL2483:
_temp2482=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2472)->f1; goto
_LL2481; _LL2481: _temp2480=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2472)->f2; goto _LL2479; _LL2479: _temp2478=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2472)->f3; goto _LL2475;} else{ goto _LL2476;} _LL2476: goto _LL2477;
_LL2475: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2482, _temp2480, _temp2478, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2473; _LL2477: Cyc_Toc_exp_to_c( init_nv,
init); goto _LL2473; _LL2473:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2484= _new_region(); struct _RegionHandle* prgn=& _temp2484;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2487; struct Cyc_List_List*
_temp2489; struct Cyc_Toc_Env* _temp2491; struct _tuple12 _temp2485= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2492: _temp2491= _temp2485.f1; goto _LL2490; _LL2490: _temp2489=
_temp2485.f2; goto _LL2488; _LL2488: _temp2487= _temp2485.f3; goto _LL2486;
_LL2486: Cyc_Toc_stmt_to_c( _temp2491, s);{ struct Cyc_Absyn_Stmt* _temp2493=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2487, _temp2493, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2494= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2495= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2494, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2487,
Cyc_Absyn_seq_stmt( _temp2495, _temp2493, 0), 0), 0);} for( 0; _temp2489 != 0;
_temp2489=(( struct Cyc_List_List*) _check_null( _temp2489))->tl){ struct
_tuple13 _temp2498; void* _temp2499; struct _tuple0* _temp2501; struct _tuple13*
_temp2496=( struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp2489))->hd;
_temp2498=* _temp2496; _LL2502: _temp2501= _temp2498.f1; goto _LL2500; _LL2500:
_temp2499= _temp2498.f2; goto _LL2497; _LL2497: s= Cyc_Absyn_declare_stmt(
_temp2501, _temp2499, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u));}{ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2503=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2529; struct Cyc_Absyn_Fndecl* _temp2531; int
_temp2533; struct Cyc_Absyn_Exp* _temp2535; struct Cyc_Core_Opt* _temp2537;
struct Cyc_Core_Opt* _temp2539; struct Cyc_Absyn_Pat* _temp2541; struct Cyc_List_List*
_temp2543; struct Cyc_Absyn_Structdecl* _temp2545; struct Cyc_Absyn_Uniondecl*
_temp2547; struct Cyc_Absyn_Tuniondecl* _temp2549; struct Cyc_Absyn_Enumdecl*
_temp2551; struct Cyc_Absyn_Typedefdecl* _temp2553; struct Cyc_List_List*
_temp2555; struct _tagged_arr* _temp2557; struct Cyc_List_List* _temp2559;
struct _tuple0* _temp2561; struct Cyc_List_List* _temp2563; _LL2505: if(*(( int*)
_temp2503) == Cyc_Absyn_Var_d){ _LL2530: _temp2529=(( struct Cyc_Absyn_Var_d_struct*)
_temp2503)->f1; goto _LL2506;} else{ goto _LL2507;} _LL2507: if(*(( int*)
_temp2503) == Cyc_Absyn_Fn_d){ _LL2532: _temp2531=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2503)->f1; goto _LL2508;} else{ goto _LL2509;} _LL2509: if(*(( int*)
_temp2503) == Cyc_Absyn_Let_d){ _LL2542: _temp2541=(( struct Cyc_Absyn_Let_d_struct*)
_temp2503)->f1; goto _LL2540; _LL2540: _temp2539=(( struct Cyc_Absyn_Let_d_struct*)
_temp2503)->f2; goto _LL2538; _LL2538: _temp2537=(( struct Cyc_Absyn_Let_d_struct*)
_temp2503)->f3; goto _LL2536; _LL2536: _temp2535=(( struct Cyc_Absyn_Let_d_struct*)
_temp2503)->f4; goto _LL2534; _LL2534: _temp2533=(( struct Cyc_Absyn_Let_d_struct*)
_temp2503)->f5; goto _LL2510;} else{ goto _LL2511;} _LL2511: if(*(( int*)
_temp2503) == Cyc_Absyn_Letv_d){ _LL2544: _temp2543=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2503)->f1; goto _LL2512;} else{ goto _LL2513;} _LL2513: if(*(( int*)
_temp2503) == Cyc_Absyn_Struct_d){ _LL2546: _temp2545=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2503)->f1; goto _LL2514;} else{ goto _LL2515;} _LL2515: if(*(( int*)
_temp2503) == Cyc_Absyn_Union_d){ _LL2548: _temp2547=(( struct Cyc_Absyn_Union_d_struct*)
_temp2503)->f1; goto _LL2516;} else{ goto _LL2517;} _LL2517: if(*(( int*)
_temp2503) == Cyc_Absyn_Tunion_d){ _LL2550: _temp2549=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2503)->f1; goto _LL2518;} else{ goto _LL2519;} _LL2519: if(*(( int*)
_temp2503) == Cyc_Absyn_Enum_d){ _LL2552: _temp2551=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2503)->f1; goto _LL2520;} else{ goto _LL2521;} _LL2521: if(*(( int*)
_temp2503) == Cyc_Absyn_Typedef_d){ _LL2554: _temp2553=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2503)->f1; goto _LL2522;} else{ goto _LL2523;} _LL2523: if(*(( int*)
_temp2503) == Cyc_Absyn_Namespace_d){ _LL2558: _temp2557=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2503)->f1; goto _LL2556; _LL2556: _temp2555=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2503)->f2; goto _LL2524;} else{ goto _LL2525;} _LL2525: if(*(( int*)
_temp2503) == Cyc_Absyn_Using_d){ _LL2562: _temp2561=(( struct Cyc_Absyn_Using_d_struct*)
_temp2503)->f1; goto _LL2560; _LL2560: _temp2559=(( struct Cyc_Absyn_Using_d_struct*)
_temp2503)->f2; goto _LL2526;} else{ goto _LL2527;} _LL2527: if(*(( int*)
_temp2503) == Cyc_Absyn_ExternC_d){ _LL2564: _temp2563=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2503)->f1; goto _LL2528;} else{ goto _LL2504;} _LL2506: { struct _tuple0*
_temp2565= _temp2529->name; if(( void*) _temp2529->sc ==( void*) Cyc_Absyn_ExternC){
_temp2565=({ struct _tuple0* _temp2566=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2566->f1= Cyc_Absyn_rel_ns_null; _temp2566->f2=(*
_temp2565).f2; _temp2566;});} if( _temp2529->initializer != 0){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) _check_null( _temp2529->initializer));} nv= Cyc_Toc_add_varmap(
nv, _temp2529->name, Cyc_Absyn_varb_exp( _temp2565,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2567=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2567[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2568; _temp2568.tag= Cyc_Absyn_Global_b;
_temp2568.f1= _temp2529; _temp2568;}); _temp2567;}), 0)); _temp2529->name=
_temp2565;( void*)( _temp2529->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2529->sc));(
void*)( _temp2529->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2529->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2569=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2569->hd=( void*) d; _temp2569->tl=
Cyc_Toc_result_decls; _temp2569;}); goto _LL2504;} _LL2508: { struct _tuple0*
_temp2570= _temp2531->name; if(( void*) _temp2531->sc ==( void*) Cyc_Absyn_ExternC){
_temp2570=({ struct _tuple0* _temp2571=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2571->f1= Cyc_Absyn_rel_ns_null; _temp2571->f2=(*
_temp2570).f2; _temp2571;});} nv= Cyc_Toc_add_varmap( nv, _temp2531->name, Cyc_Absyn_var_exp(
_temp2570, 0)); _temp2531->name= _temp2570; Cyc_Toc_fndecl_to_c( nv, _temp2531);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2572=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2572->hd=( void*) d; _temp2572->tl=
Cyc_Toc_result_decls; _temp2572;}); goto _LL2504;} _LL2510: goto _LL2512;
_LL2512:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel",
sizeof( unsigned char), 20u)); goto _LL2504; _LL2514: Cyc_Toc_structdecl_to_c(
_temp2545); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2573=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2573->hd=( void*) d; _temp2573->tl=
Cyc_Toc_result_decls; _temp2573;}); goto _LL2504; _LL2516: Cyc_Toc_uniondecl_to_c(
_temp2547); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2574=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2574->hd=( void*) d; _temp2574->tl=
Cyc_Toc_result_decls; _temp2574;}); goto _LL2504; _LL2518: if( _temp2549->is_xtunion){
Cyc_Toc_xtuniondecl_to_c( _temp2549);} else{ Cyc_Toc_tuniondecl_to_c( _temp2549);}
goto _LL2504; _LL2520: Cyc_Toc_enumdecl_to_c( nv, _temp2551); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2575=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2575->hd=( void*) d; _temp2575->tl= Cyc_Toc_result_decls;
_temp2575;}); goto _LL2504; _LL2522: _temp2553->name= _temp2553->name; _temp2553->tvs=
0;( void*)( _temp2553->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2553->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2576=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2576->hd=( void*) d; _temp2576->tl=
Cyc_Toc_result_decls; _temp2576;}); goto _LL2504; _LL2524: _temp2559= _temp2555;
goto _LL2526; _LL2526: _temp2563= _temp2559; goto _LL2528; _LL2528: nv= Cyc_Toc_decls_to_c(
nv, _temp2563, top); goto _LL2504; _LL2504:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}