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
for( 0; _temp161 != 0; _temp161=(( struct Cyc_List_List*) _check_null( _temp161))->tl){
struct _tuple5 _temp164; struct Cyc_List_List* _temp165; struct _tagged_arr*
_temp167; struct _tuple5* _temp162=( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp161))->hd; _temp164=* _temp162; _LL168: _temp167= _temp164.f1;
goto _LL166; _LL166: _temp165= _temp164.f2; goto _LL163; _LL163: { struct Cyc_List_List*
_temp169= tqs0; for( 0; _temp169 != 0? _temp165 != 0: 0;( _temp169=(( struct Cyc_List_List*)
_check_null( _temp169))->tl, _temp165=(( struct Cyc_List_List*) _check_null(
_temp165))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp169))->hd)).f2,( void*)(( struct Cyc_List_List*) _check_null(
_temp165))->hd)){ break;}} if( _temp169 == 0? _temp165 == 0: 0){ return _temp167;}}}}{
struct _tagged_arr* x=({ struct _tagged_arr* _temp183=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp183[ 0]=( struct _tagged_arr)
xprintf("_tuple%d", Cyc_Toc_tuple_type_counter ++); _temp183;}); struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); struct Cyc_List_List* _temp170= 0;
struct Cyc_List_List* ts2= ts;{ int i= 1; for( 0; ts2 != 0;( ts2=(( struct Cyc_List_List*)
_check_null( ts2))->tl, i ++)){ _temp170=({ struct Cyc_List_List* _temp171=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp171->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp172=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp172->name= Cyc_Toc_fieldname(
i); _temp172->tq= Cyc_Toc_mt_tq; _temp172->type=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( ts2))->hd); _temp172->width= 0; _temp172->attributes= 0; _temp172;});
_temp171->tl= _temp170; _temp171;});}} _temp170=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp170);{ struct Cyc_Absyn_Structdecl*
_temp173=({ struct Cyc_Absyn_Structdecl* _temp179=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp179->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp179->name=({ struct Cyc_Core_Opt* _temp181=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp181->v=( void*)({ struct _tuple0*
_temp182=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp182->f1=
Cyc_Toc_rel_ns; _temp182->f2= x; _temp182;}); _temp181;}); _temp179->tvs= 0;
_temp179->fields=({ struct Cyc_Core_Opt* _temp180=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp180->v=( void*) _temp170;
_temp180;}); _temp179->attributes= 0; _temp179;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp174=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp174->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp175=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp175[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp176; _temp176.tag= Cyc_Absyn_Struct_d; _temp176.f1=
_temp173; _temp176;}); _temp175;}), 0); _temp174->tl= Cyc_Toc_result_decls;
_temp174;}); Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp177=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp177->hd=( void*)({ struct
_tuple5* _temp178=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp178->f1= x; _temp178->f2= ts; _temp178;}); _temp177->tl= Cyc_Toc_tuple_types;
_temp177;}); return x;}}} struct _tuple0* Cyc_Toc_temp_var(){ return({ struct
_tuple0* _temp184=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp184->f1=( void*) Cyc_Absyn_Loc_n; _temp184->f2=({ struct _tagged_arr*
_temp185=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp185[ 0]=( struct _tagged_arr) xprintf("_temp%d", Cyc_Toc_temp_var_counter
++); _temp185;}); _temp184;});} static struct _tagged_arr* Cyc_Toc_fresh_label(){
return({ struct _tagged_arr* _temp186=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)); _temp186[ 0]=( struct _tagged_arr) xprintf("_LL%d", Cyc_Toc_temp_var_counter
++); _temp186;});} static void* Cyc_Toc_typ_to_c( void* t); static struct
_tuple1* Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp189; struct Cyc_Absyn_Tqual
_temp191; struct Cyc_Core_Opt* _temp193; struct _tuple1 _temp187=* a; _LL194:
_temp193= _temp187.f1; goto _LL192; _LL192: _temp191= _temp187.f2; goto _LL190;
_LL190: _temp189= _temp187.f3; goto _LL188; _LL188: return({ struct _tuple1*
_temp195=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp195->f1=
_temp193; _temp195->f2= _temp191; _temp195->f3= Cyc_Toc_typ_to_c( _temp189);
_temp195;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp198; struct Cyc_Absyn_Tqual _temp200; struct _tuple3 _temp196=* x;
_LL201: _temp200= _temp196.f1; goto _LL199; _LL199: _temp198= _temp196.f2; goto
_LL197; _LL197: return({ struct _tuple3* _temp202=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp202->f1= _temp200; _temp202->f2= Cyc_Toc_typ_to_c(
_temp198); _temp202;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp203= t; struct Cyc_Absyn_Exp* _temp211; struct Cyc_Absyn_Tqual _temp213;
void* _temp215; struct Cyc_Core_Opt* _temp217; int _temp219; struct Cyc_Core_Opt*
_temp221; struct Cyc_Core_Opt _temp223; void* _temp224; struct Cyc_Core_Opt*
_temp226; _LL205: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_ArrayType:
0){ _LL216: _temp215=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp203)->f1;
goto _LL214; _LL214: _temp213=(( struct Cyc_Absyn_ArrayType_struct*) _temp203)->f2;
goto _LL212; _LL212: _temp211=(( struct Cyc_Absyn_ArrayType_struct*) _temp203)->f3;
goto _LL206;} else{ goto _LL207;} _LL207: if(( unsigned int) _temp203 > 4u?*((
int*) _temp203) == Cyc_Absyn_Evar: 0){ _LL227: _temp226=(( struct Cyc_Absyn_Evar_struct*)
_temp203)->f1; goto _LL222; _LL222: _temp221=(( struct Cyc_Absyn_Evar_struct*)
_temp203)->f2; if( _temp221 == 0){ goto _LL209;} else{ _temp223=* _temp221;
_LL225: _temp224=( void*) _temp223.v; goto _LL220;} _LL220: _temp219=(( struct
Cyc_Absyn_Evar_struct*) _temp203)->f3; goto _LL218; _LL218: _temp217=(( struct
Cyc_Absyn_Evar_struct*) _temp203)->f4; goto _LL208;} else{ goto _LL209;} _LL209:
goto _LL210; _LL206: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp228=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp228[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp229; _temp229.tag= Cyc_Absyn_ArrayType;
_temp229.f1=( void*) Cyc_Toc_typ_to_c_array( _temp215); _temp229.f2= _temp213;
_temp229.f3= _temp211; _temp229;}); _temp228;}); _LL208: return Cyc_Toc_typ_to_c_array(
_temp224); _LL210: return Cyc_Toc_typ_to_c( t); _LL204:;} static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c( struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield*
_temp230=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp230->name= f->name; _temp230->tq= f->tq; _temp230->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp230->width= f->width; _temp230->attributes= f->attributes;
_temp230;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp231= t; struct
Cyc_Core_Opt* _temp283; int _temp285; struct Cyc_Core_Opt* _temp287; struct Cyc_Core_Opt*
_temp289; struct Cyc_Core_Opt* _temp291; int _temp293; struct Cyc_Core_Opt*
_temp295; struct Cyc_Core_Opt _temp297; void* _temp298; struct Cyc_Core_Opt*
_temp300; struct Cyc_Absyn_Tvar* _temp302; struct Cyc_Absyn_TunionInfo _temp304;
void* _temp306; struct Cyc_List_List* _temp308; void* _temp310; struct Cyc_Absyn_Tuniondecl*
_temp312; struct Cyc_Absyn_TunionInfo _temp314; struct Cyc_Absyn_TunionFieldInfo
_temp316; struct Cyc_List_List* _temp318; void* _temp320; struct Cyc_Absyn_Tunionfield*
_temp322; struct Cyc_Absyn_Tuniondecl* _temp324; struct Cyc_Absyn_TunionFieldInfo
_temp326; struct Cyc_Absyn_PtrInfo _temp328; struct Cyc_Absyn_Conref* _temp330;
struct Cyc_Absyn_Tqual _temp332; struct Cyc_Absyn_Conref* _temp334; void*
_temp336; void* _temp338; void* _temp340; void* _temp342; struct Cyc_Absyn_Exp*
_temp344; struct Cyc_Absyn_Tqual _temp346; void* _temp348; struct Cyc_Absyn_FnInfo
_temp350; struct Cyc_List_List* _temp352; struct Cyc_List_List* _temp354; struct
Cyc_Absyn_VarargInfo* _temp356; int _temp358; struct Cyc_List_List* _temp360;
void* _temp362; struct Cyc_Core_Opt* _temp364; struct Cyc_List_List* _temp366;
struct Cyc_List_List* _temp368; struct Cyc_List_List* _temp370; struct Cyc_List_List*
_temp372; struct Cyc_Absyn_Uniondecl** _temp374; struct Cyc_List_List* _temp376;
struct _tuple0* _temp378; struct Cyc_Absyn_Structdecl** _temp380; struct Cyc_List_List*
_temp382; struct _tuple0* _temp384; struct Cyc_Absyn_Enumdecl* _temp386; struct
_tuple0* _temp388; struct Cyc_Core_Opt* _temp390; struct Cyc_List_List* _temp392;
struct _tuple0* _temp394; void* _temp396; void* _temp398; struct Cyc_List_List*
_temp400; _LL233: if( _temp231 ==( void*) Cyc_Absyn_VoidType){ goto _LL234;}
else{ goto _LL235;} _LL235: if(( unsigned int) _temp231 > 4u?*(( int*) _temp231)
== Cyc_Absyn_Evar: 0){ _LL290: _temp289=(( struct Cyc_Absyn_Evar_struct*)
_temp231)->f1; goto _LL288; _LL288: _temp287=(( struct Cyc_Absyn_Evar_struct*)
_temp231)->f2; if( _temp287 == 0){ goto _LL286;} else{ goto _LL237;} _LL286:
_temp285=(( struct Cyc_Absyn_Evar_struct*) _temp231)->f3; goto _LL284; _LL284:
_temp283=(( struct Cyc_Absyn_Evar_struct*) _temp231)->f4; goto _LL236;} else{
goto _LL237;} _LL237: if(( unsigned int) _temp231 > 4u?*(( int*) _temp231) ==
Cyc_Absyn_Evar: 0){ _LL301: _temp300=(( struct Cyc_Absyn_Evar_struct*) _temp231)->f1;
goto _LL296; _LL296: _temp295=(( struct Cyc_Absyn_Evar_struct*) _temp231)->f2;
if( _temp295 == 0){ goto _LL239;} else{ _temp297=* _temp295; _LL299: _temp298=(
void*) _temp297.v; goto _LL294;} _LL294: _temp293=(( struct Cyc_Absyn_Evar_struct*)
_temp231)->f3; goto _LL292; _LL292: _temp291=(( struct Cyc_Absyn_Evar_struct*)
_temp231)->f4; goto _LL238;} else{ goto _LL239;} _LL239: if(( unsigned int)
_temp231 > 4u?*(( int*) _temp231) == Cyc_Absyn_VarType: 0){ _LL303: _temp302=((
struct Cyc_Absyn_VarType_struct*) _temp231)->f1; goto _LL240;} else{ goto _LL241;}
_LL241: if(( unsigned int) _temp231 > 4u?*(( int*) _temp231) == Cyc_Absyn_TunionType:
0){ _LL305: _temp304=(( struct Cyc_Absyn_TunionType_struct*) _temp231)->f1;
_LL311: _temp310=( void*) _temp304.tunion_info; if(*(( int*) _temp310) == Cyc_Absyn_KnownTunion){
_LL313: _temp312=(( struct Cyc_Absyn_KnownTunion_struct*) _temp310)->f1; goto
_LL309;} else{ goto _LL243;} _LL309: _temp308= _temp304.targs; goto _LL307;
_LL307: _temp306=( void*) _temp304.rgn; goto _LL242;} else{ goto _LL243;} _LL243:
if(( unsigned int) _temp231 > 4u?*(( int*) _temp231) == Cyc_Absyn_TunionType: 0){
_LL315: _temp314=(( struct Cyc_Absyn_TunionType_struct*) _temp231)->f1; goto
_LL244;} else{ goto _LL245;} _LL245: if(( unsigned int) _temp231 > 4u?*(( int*)
_temp231) == Cyc_Absyn_TunionFieldType: 0){ _LL317: _temp316=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp231)->f1; _LL321: _temp320=( void*) _temp316.field_info; if(*(( int*)
_temp320) == Cyc_Absyn_KnownTunionfield){ _LL325: _temp324=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp320)->f1; goto _LL323; _LL323: _temp322=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp320)->f2; goto _LL319;} else{ goto _LL247;} _LL319: _temp318= _temp316.targs;
goto _LL246;} else{ goto _LL247;} _LL247: if(( unsigned int) _temp231 > 4u?*((
int*) _temp231) == Cyc_Absyn_TunionFieldType: 0){ _LL327: _temp326=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp231)->f1; goto _LL248;} else{ goto _LL249;} _LL249: if(( unsigned int)
_temp231 > 4u?*(( int*) _temp231) == Cyc_Absyn_PointerType: 0){ _LL329: _temp328=((
struct Cyc_Absyn_PointerType_struct*) _temp231)->f1; _LL339: _temp338=( void*)
_temp328.elt_typ; goto _LL337; _LL337: _temp336=( void*) _temp328.rgn_typ; goto
_LL335; _LL335: _temp334= _temp328.nullable; goto _LL333; _LL333: _temp332=
_temp328.tq; goto _LL331; _LL331: _temp330= _temp328.bounds; goto _LL250;} else{
goto _LL251;} _LL251: if(( unsigned int) _temp231 > 4u?*(( int*) _temp231) ==
Cyc_Absyn_IntType: 0){ _LL343: _temp342=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp231)->f1; goto _LL341; _LL341: _temp340=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp231)->f2; goto _LL252;} else{ goto _LL253;} _LL253: if( _temp231 ==( void*)
Cyc_Absyn_FloatType){ goto _LL254;} else{ goto _LL255;} _LL255: if( _temp231 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL256;} else{ goto _LL257;} _LL257: if((
unsigned int) _temp231 > 4u?*(( int*) _temp231) == Cyc_Absyn_ArrayType: 0){
_LL349: _temp348=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp231)->f1;
goto _LL347; _LL347: _temp346=(( struct Cyc_Absyn_ArrayType_struct*) _temp231)->f2;
goto _LL345; _LL345: _temp344=(( struct Cyc_Absyn_ArrayType_struct*) _temp231)->f3;
goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int) _temp231 > 4u?*((
int*) _temp231) == Cyc_Absyn_FnType: 0){ _LL351: _temp350=(( struct Cyc_Absyn_FnType_struct*)
_temp231)->f1; _LL367: _temp366= _temp350.tvars; goto _LL365; _LL365: _temp364=
_temp350.effect; goto _LL363; _LL363: _temp362=( void*) _temp350.ret_typ; goto
_LL361; _LL361: _temp360= _temp350.args; goto _LL359; _LL359: _temp358= _temp350.c_varargs;
goto _LL357; _LL357: _temp356= _temp350.cyc_varargs; goto _LL355; _LL355:
_temp354= _temp350.rgn_po; goto _LL353; _LL353: _temp352= _temp350.attributes;
goto _LL260;} else{ goto _LL261;} _LL261: if(( unsigned int) _temp231 > 4u?*((
int*) _temp231) == Cyc_Absyn_TupleType: 0){ _LL369: _temp368=(( struct Cyc_Absyn_TupleType_struct*)
_temp231)->f1; goto _LL262;} else{ goto _LL263;} _LL263: if(( unsigned int)
_temp231 > 4u?*(( int*) _temp231) == Cyc_Absyn_AnonStructType: 0){ _LL371:
_temp370=(( struct Cyc_Absyn_AnonStructType_struct*) _temp231)->f1; goto _LL264;}
else{ goto _LL265;} _LL265: if(( unsigned int) _temp231 > 4u?*(( int*) _temp231)
== Cyc_Absyn_AnonUnionType: 0){ _LL373: _temp372=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp231)->f1; goto _LL266;} else{ goto _LL267;} _LL267: if(( unsigned int)
_temp231 > 4u?*(( int*) _temp231) == Cyc_Absyn_UnionType: 0){ _LL379: _temp378=((
struct Cyc_Absyn_UnionType_struct*) _temp231)->f1; goto _LL377; _LL377: _temp376=((
struct Cyc_Absyn_UnionType_struct*) _temp231)->f2; goto _LL375; _LL375: _temp374=((
struct Cyc_Absyn_UnionType_struct*) _temp231)->f3; goto _LL268;} else{ goto
_LL269;} _LL269: if(( unsigned int) _temp231 > 4u?*(( int*) _temp231) == Cyc_Absyn_StructType:
0){ _LL385: _temp384=(( struct Cyc_Absyn_StructType_struct*) _temp231)->f1; goto
_LL383; _LL383: _temp382=(( struct Cyc_Absyn_StructType_struct*) _temp231)->f2;
goto _LL381; _LL381: _temp380=(( struct Cyc_Absyn_StructType_struct*) _temp231)->f3;
goto _LL270;} else{ goto _LL271;} _LL271: if(( unsigned int) _temp231 > 4u?*((
int*) _temp231) == Cyc_Absyn_EnumType: 0){ _LL389: _temp388=(( struct Cyc_Absyn_EnumType_struct*)
_temp231)->f1; goto _LL387; _LL387: _temp386=(( struct Cyc_Absyn_EnumType_struct*)
_temp231)->f2; goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int)
_temp231 > 4u?*(( int*) _temp231) == Cyc_Absyn_TypedefType: 0){ _LL395: _temp394=((
struct Cyc_Absyn_TypedefType_struct*) _temp231)->f1; goto _LL393; _LL393:
_temp392=(( struct Cyc_Absyn_TypedefType_struct*) _temp231)->f2; goto _LL391;
_LL391: _temp390=(( struct Cyc_Absyn_TypedefType_struct*) _temp231)->f3; goto
_LL274;} else{ goto _LL275;} _LL275: if(( unsigned int) _temp231 > 4u?*(( int*)
_temp231) == Cyc_Absyn_RgnHandleType: 0){ _LL397: _temp396=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp231)->f1; goto _LL276;} else{ goto _LL277;} _LL277: if( _temp231 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL278;} else{ goto _LL279;} _LL279: if(( unsigned int)
_temp231 > 4u?*(( int*) _temp231) == Cyc_Absyn_AccessEff: 0){ _LL399: _temp398=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp231)->f1; goto _LL280;} else{
goto _LL281;} _LL281: if(( unsigned int) _temp231 > 4u?*(( int*) _temp231) ==
Cyc_Absyn_JoinEff: 0){ _LL401: _temp400=(( struct Cyc_Absyn_JoinEff_struct*)
_temp231)->f1; goto _LL282;} else{ goto _LL232;} _LL234: return t; _LL236:
return Cyc_Absyn_sint_t; _LL238: return Cyc_Toc_typ_to_c( _temp298); _LL240:
return Cyc_Absyn_void_star_typ(); _LL242: return Cyc_Absyn_void_star_typ();
_LL244:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("unresolved TunionType", sizeof( unsigned char), 22u))); _LL246: if(
_temp322->typs == 0){ if( _temp324->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp322->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL248:( int) _throw((( void*(*)(
struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u))); _LL250: _temp338= Cyc_Toc_typ_to_c_array(
_temp338);{ void* _temp402=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp330))->v; void* _temp408; _LL404: if((
unsigned int) _temp402 > 1u?*(( int*) _temp402) == Cyc_Absyn_Eq_constr: 0){
_LL409: _temp408=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp402)->f1;
if( _temp408 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL405;} else{ goto _LL406;}}
else{ goto _LL406;} _LL406: goto _LL407; _LL405: return Cyc_Toc_tagged_arr_typ;
_LL407: return Cyc_Absyn_star_typ( _temp338,( void*) Cyc_Absyn_HeapRgn, _temp332);
_LL403:;} _LL252: return t; _LL254: return t; _LL256: return t; _LL258: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp410=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp410[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp411; _temp411.tag= Cyc_Absyn_ArrayType; _temp411.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp348); _temp411.f2= _temp346; _temp411.f3=
_temp344; _temp411;}); _temp410;}); _LL260: { struct Cyc_List_List* _temp412=
_temp352; struct Cyc_List_List* _temp413= 0; for( 0; _temp352 != 0; _temp352=((
struct Cyc_List_List*) _check_null( _temp352))->tl){ void* _temp414=( void*)((
struct Cyc_List_List*) _check_null( _temp352))->hd; _LL416: if( _temp414 ==(
void*) Cyc_Absyn_Noreturn_att){ goto _LL417;} else{ goto _LL418;} _LL418: if(
_temp414 ==( void*) Cyc_Absyn_Const_att){ goto _LL419;} else{ goto _LL420;}
_LL420: goto _LL421; _LL417: goto _LL419; _LL419: continue; _LL421: _temp413=({
struct Cyc_List_List* _temp422=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp422->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( _temp352))->hd); _temp422->tl= _temp413; _temp422;}); goto _LL415;
_LL415:;}{ struct Cyc_List_List* _temp423=(( struct Cyc_List_List*(*)( struct
_tuple1*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp360); if( _temp356 != 0){ void* _temp424= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp356))->type,( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp356))->rgn, Cyc_Toc_mt_tq));
struct _tuple1* _temp425=({ struct _tuple1* _temp427=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp427->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp356))->name; _temp427->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp356))->tq; _temp427->f3= _temp424; _temp427;}); _temp423=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp423,({ struct Cyc_List_List* _temp426=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp426->hd=( void*) _temp425; _temp426->tl= 0;
_temp426;}));} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp428=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp428[ 0]=({ struct Cyc_Absyn_FnType_struct _temp429; _temp429.tag= Cyc_Absyn_FnType;
_temp429.f1=({ struct Cyc_Absyn_FnInfo _temp430; _temp430.tvars= 0; _temp430.effect=
0; _temp430.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp362); _temp430.args=
_temp423; _temp430.c_varargs= _temp358; _temp430.cyc_varargs= 0; _temp430.rgn_po=
0; _temp430.attributes= _temp413; _temp430;}); _temp429;}); _temp428;});}}
_LL262: _temp368=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp368);{
struct _tagged_arr* _temp431= Cyc_Toc_add_tuple_type( _temp368); return Cyc_Absyn_strct(
_temp431);} _LL264: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp432=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp432[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp433; _temp433.tag= Cyc_Absyn_AnonStructType; _temp433.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp370); _temp433;}); _temp432;});
_LL266: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp434=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp434[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp435; _temp435.tag=
Cyc_Absyn_AnonUnionType; _temp435.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp372); _temp435;}); _temp434;}); _LL268: if( _temp378 == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous union", sizeof(
unsigned char), 16u));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp378)); _LL270: if( _temp384 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous struct", sizeof(
unsigned char), 17u));} return Cyc_Absyn_strctq(( struct _tuple0*) _check_null(
_temp384)); _LL272: return t; _LL274: if( _temp390 == 0){ return(( void*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: unresolved TypedefType",
sizeof( unsigned char), 38u));} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp436=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp436[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp437; _temp437.tag= Cyc_Absyn_TypedefType;
_temp437.f1= _temp394; _temp437.f2= 0; _temp437.f3=({ struct Cyc_Core_Opt*
_temp438=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp438->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp390))->v); _temp438;}); _temp437;}); _temp436;}); _LL276:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL278: return(( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u)); _LL280: return(( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u)); _LL282: return(( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u)); _LL232:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp439= t; struct Cyc_Absyn_Exp* _temp445; struct Cyc_Absyn_Tqual _temp447;
void* _temp449; _LL441: if(( unsigned int) _temp439 > 4u?*(( int*) _temp439) ==
Cyc_Absyn_ArrayType: 0){ _LL450: _temp449=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp439)->f1; goto _LL448; _LL448: _temp447=(( struct Cyc_Absyn_ArrayType_struct*)
_temp439)->f2; goto _LL446; _LL446: _temp445=(( struct Cyc_Absyn_ArrayType_struct*)
_temp439)->f3; goto _LL442;} else{ goto _LL443;} _LL443: goto _LL444; _LL442:
return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ( _temp449,( void*) Cyc_Absyn_HeapRgn,
_temp447), e, l); _LL444: return Cyc_Absyn_cast_exp( t, e, l); _LL440:;} static
int Cyc_Toc_atomic_typ( void* t){ void* _temp451= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp487; void* _temp489; void* _temp491; struct Cyc_Absyn_FnInfo
_temp493; struct Cyc_Absyn_Exp* _temp495; struct Cyc_Absyn_Tqual _temp497; void*
_temp499; struct Cyc_List_List* _temp501; struct Cyc_Absyn_Structdecl** _temp503;
struct Cyc_List_List* _temp505; struct _tuple0* _temp507; struct Cyc_Absyn_TunionInfo
_temp509; struct Cyc_List_List* _temp511; struct Cyc_List_List* _temp513; struct
Cyc_Absyn_TunionFieldInfo _temp515; struct Cyc_List_List* _temp517; void*
_temp519; struct Cyc_Absyn_Tunionfield* _temp521; struct Cyc_Absyn_Tuniondecl*
_temp523; struct Cyc_Absyn_Uniondecl** _temp525; struct Cyc_List_List* _temp527;
struct _tuple0* _temp529; struct Cyc_Absyn_PtrInfo _temp531; void* _temp533;
_LL453: if( _temp451 ==( void*) Cyc_Absyn_VoidType){ goto _LL454;} else{ goto
_LL455;} _LL455: if(( unsigned int) _temp451 > 4u?*(( int*) _temp451) == Cyc_Absyn_VarType:
0){ _LL488: _temp487=(( struct Cyc_Absyn_VarType_struct*) _temp451)->f1; goto
_LL456;} else{ goto _LL457;} _LL457: if(( unsigned int) _temp451 > 4u?*(( int*)
_temp451) == Cyc_Absyn_IntType: 0){ _LL492: _temp491=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp451)->f1; goto _LL490; _LL490: _temp489=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp451)->f2; goto _LL458;} else{ goto _LL459;} _LL459: if( _temp451 ==( void*)
Cyc_Absyn_FloatType){ goto _LL460;} else{ goto _LL461;} _LL461: if( _temp451 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL462;} else{ goto _LL463;} _LL463: if((
unsigned int) _temp451 > 4u?*(( int*) _temp451) == Cyc_Absyn_FnType: 0){ _LL494:
_temp493=(( struct Cyc_Absyn_FnType_struct*) _temp451)->f1; goto _LL464;} else{
goto _LL465;} _LL465: if(( unsigned int) _temp451 > 4u?*(( int*) _temp451) ==
Cyc_Absyn_ArrayType: 0){ _LL500: _temp499=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp451)->f1; goto _LL498; _LL498: _temp497=(( struct Cyc_Absyn_ArrayType_struct*)
_temp451)->f2; goto _LL496; _LL496: _temp495=(( struct Cyc_Absyn_ArrayType_struct*)
_temp451)->f3; goto _LL466;} else{ goto _LL467;} _LL467: if(( unsigned int)
_temp451 > 4u?*(( int*) _temp451) == Cyc_Absyn_TupleType: 0){ _LL502: _temp501=((
struct Cyc_Absyn_TupleType_struct*) _temp451)->f1; goto _LL468;} else{ goto
_LL469;} _LL469: if(( unsigned int) _temp451 > 4u?*(( int*) _temp451) == Cyc_Absyn_StructType:
0){ _LL508: _temp507=(( struct Cyc_Absyn_StructType_struct*) _temp451)->f1; goto
_LL506; _LL506: _temp505=(( struct Cyc_Absyn_StructType_struct*) _temp451)->f2;
goto _LL504; _LL504: _temp503=(( struct Cyc_Absyn_StructType_struct*) _temp451)->f3;
goto _LL470;} else{ goto _LL471;} _LL471: if(( unsigned int) _temp451 > 4u?*((
int*) _temp451) == Cyc_Absyn_TunionType: 0){ _LL510: _temp509=(( struct Cyc_Absyn_TunionType_struct*)
_temp451)->f1; goto _LL472;} else{ goto _LL473;} _LL473: if(( unsigned int)
_temp451 > 4u?*(( int*) _temp451) == Cyc_Absyn_AnonStructType: 0){ _LL512:
_temp511=(( struct Cyc_Absyn_AnonStructType_struct*) _temp451)->f1; goto _LL474;}
else{ goto _LL475;} _LL475: if(( unsigned int) _temp451 > 4u?*(( int*) _temp451)
== Cyc_Absyn_AnonUnionType: 0){ _LL514: _temp513=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp451)->f1; goto _LL476;} else{ goto _LL477;} _LL477: if(( unsigned int)
_temp451 > 4u?*(( int*) _temp451) == Cyc_Absyn_TunionFieldType: 0){ _LL516:
_temp515=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp451)->f1; _LL520:
_temp519=( void*) _temp515.field_info; if(*(( int*) _temp519) == Cyc_Absyn_KnownTunionfield){
_LL524: _temp523=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp519)->f1;
goto _LL522; _LL522: _temp521=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp519)->f2; goto _LL518;} else{ goto _LL479;} _LL518: _temp517= _temp515.targs;
goto _LL478;} else{ goto _LL479;} _LL479: if(( unsigned int) _temp451 > 4u?*((
int*) _temp451) == Cyc_Absyn_UnionType: 0){ _LL530: _temp529=(( struct Cyc_Absyn_UnionType_struct*)
_temp451)->f1; goto _LL528; _LL528: _temp527=(( struct Cyc_Absyn_UnionType_struct*)
_temp451)->f2; goto _LL526; _LL526: _temp525=(( struct Cyc_Absyn_UnionType_struct*)
_temp451)->f3; goto _LL480;} else{ goto _LL481;} _LL481: if(( unsigned int)
_temp451 > 4u?*(( int*) _temp451) == Cyc_Absyn_PointerType: 0){ _LL532: _temp531=((
struct Cyc_Absyn_PointerType_struct*) _temp451)->f1; goto _LL482;} else{ goto
_LL483;} _LL483: if(( unsigned int) _temp451 > 4u?*(( int*) _temp451) == Cyc_Absyn_RgnHandleType:
0){ _LL534: _temp533=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp451)->f1;
goto _LL484;} else{ goto _LL485;} _LL485: goto _LL486; _LL454: return 1; _LL456:
return 0; _LL458: return 1; _LL460: return 1; _LL462: return 1; _LL464: return 1;
_LL466: return Cyc_Toc_atomic_typ( _temp499); _LL468: for( 0; _temp501 != 0;
_temp501=(( struct Cyc_List_List*) _check_null( _temp501))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp501))->hd)).f2)){
return 0;}} return 1; _LL470: return 0; _LL472: return 0; _LL474: _temp513=
_temp511; goto _LL476; _LL476: for( 0; _temp513 != 0; _temp513=(( struct Cyc_List_List*)
_check_null( _temp513))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp513))->hd)->type)){ return 0;}} return
1; _LL478: return _temp521->typs == 0; _LL480: return 0; _LL482: return 0;
_LL484: return 0; _LL486:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp535= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _get_arr_size( _temp535, 1u), _temp535.curr);}));
return 0; _LL452:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp536=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp542; struct Cyc_Absyn_Conref*
_temp544; struct Cyc_Absyn_Tqual _temp546; struct Cyc_Absyn_Conref* _temp548;
void* _temp550; void* _temp552; _LL538: if(( unsigned int) _temp536 > 4u?*(( int*)
_temp536) == Cyc_Absyn_PointerType: 0){ _LL543: _temp542=(( struct Cyc_Absyn_PointerType_struct*)
_temp536)->f1; _LL553: _temp552=( void*) _temp542.elt_typ; goto _LL551; _LL551:
_temp550=( void*) _temp542.rgn_typ; goto _LL549; _LL549: _temp548= _temp542.nullable;
goto _LL547; _LL547: _temp546= _temp542.tq; goto _LL545; _LL545: _temp544=
_temp542.bounds; goto _LL539;} else{ goto _LL540;} _LL540: goto _LL541; _LL539: {
void* _temp554= Cyc_Tcutil_compress( _temp552); _LL556: if( _temp554 ==( void*)
Cyc_Absyn_VoidType){ goto _LL557;} else{ goto _LL558;} _LL558: goto _LL559;
_LL557: return 1; _LL559: return 0; _LL555:;} _LL541: return 0; _LL537:;} static
int Cyc_Toc_is_poly_field( void* t, struct _tagged_arr* f){ void* _temp560= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Structdecl** _temp572; struct Cyc_Absyn_Structdecl*
_temp574; struct Cyc_List_List* _temp575; struct _tuple0* _temp577; struct Cyc_List_List*
_temp579; struct Cyc_List_List* _temp581; struct Cyc_Absyn_Uniondecl** _temp583;
struct Cyc_List_List* _temp585; struct _tuple0* _temp587; _LL562: if((
unsigned int) _temp560 > 4u?*(( int*) _temp560) == Cyc_Absyn_StructType: 0){
_LL578: _temp577=(( struct Cyc_Absyn_StructType_struct*) _temp560)->f1; goto
_LL576; _LL576: _temp575=(( struct Cyc_Absyn_StructType_struct*) _temp560)->f2;
goto _LL573; _LL573: _temp572=(( struct Cyc_Absyn_StructType_struct*) _temp560)->f3;
if( _temp572 == 0){ goto _LL564;} else{ _temp574=* _temp572; goto _LL563;}}
else{ goto _LL564;} _LL564: if(( unsigned int) _temp560 > 4u?*(( int*) _temp560)
== Cyc_Absyn_AnonStructType: 0){ _LL580: _temp579=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp560)->f1; goto _LL565;} else{ goto _LL566;} _LL566: if(( unsigned int)
_temp560 > 4u?*(( int*) _temp560) == Cyc_Absyn_AnonUnionType: 0){ _LL582:
_temp581=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp560)->f1; goto _LL567;}
else{ goto _LL568;} _LL568: if(( unsigned int) _temp560 > 4u?*(( int*) _temp560)
== Cyc_Absyn_UnionType: 0){ _LL588: _temp587=(( struct Cyc_Absyn_UnionType_struct*)
_temp560)->f1; goto _LL586; _LL586: _temp585=(( struct Cyc_Absyn_UnionType_struct*)
_temp560)->f2; goto _LL584; _LL584: _temp583=(( struct Cyc_Absyn_UnionType_struct*)
_temp560)->f3; goto _LL569;} else{ goto _LL570;} _LL570: goto _LL571; _LL563:
if( _temp574->fields == 0){( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: struct missing fields", sizeof( unsigned char), 37u)));}
_temp579=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp574->fields))->v;
goto _LL565; _LL565: { struct Cyc_Absyn_Structfield* _temp589= Cyc_Absyn_lookup_field(
_temp579, f); if( _temp589 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp590=*(( struct Cyc_Absyn_Structfield*)
_check_null( _temp589))->name; xprintf("is_poly_field: bad field %.*s",
_get_arr_size( _temp590, 1u), _temp590.curr);}));} return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp589))->type);} _LL567:
return 0; _LL569: return 0; _LL571:( int) _throw((( void*(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr _temp591= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field: bad type %.*s", _get_arr_size( _temp591, 1u),
_temp591.curr);}))); _LL561:;} static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp*
e){ void* _temp592=( void*) e->r; struct _tagged_arr* _temp600; struct Cyc_Absyn_Exp*
_temp602; struct _tagged_arr* _temp604; struct Cyc_Absyn_Exp* _temp606; _LL594:
if(*(( int*) _temp592) == Cyc_Absyn_StructMember_e){ _LL603: _temp602=(( struct
Cyc_Absyn_StructMember_e_struct*) _temp592)->f1; goto _LL601; _LL601: _temp600=((
struct Cyc_Absyn_StructMember_e_struct*) _temp592)->f2; goto _LL595;} else{ goto
_LL596;} _LL596: if(*(( int*) _temp592) == Cyc_Absyn_StructArrow_e){ _LL607:
_temp606=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp592)->f1; goto _LL605;
_LL605: _temp604=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp592)->f2; goto
_LL597;} else{ goto _LL598;} _LL598: goto _LL599; _LL595: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp602->topt))->v, _temp600);
_LL597: { void* _temp608= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp606->topt))->v); struct Cyc_Absyn_PtrInfo _temp614; struct Cyc_Absyn_Conref*
_temp616; struct Cyc_Absyn_Tqual _temp618; struct Cyc_Absyn_Conref* _temp620;
void* _temp622; void* _temp624; _LL610: if(( unsigned int) _temp608 > 4u?*(( int*)
_temp608) == Cyc_Absyn_PointerType: 0){ _LL615: _temp614=(( struct Cyc_Absyn_PointerType_struct*)
_temp608)->f1; _LL625: _temp624=( void*) _temp614.elt_typ; goto _LL623; _LL623:
_temp622=( void*) _temp614.rgn_typ; goto _LL621; _LL621: _temp620= _temp614.nullable;
goto _LL619; _LL619: _temp618= _temp614.tq; goto _LL617; _LL617: _temp616=
_temp614.bounds; goto _LL611;} else{ goto _LL612;} _LL612: goto _LL613; _LL611:
return Cyc_Toc_is_poly_field( _temp624, _temp604); _LL613:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp626= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp606->topt))->v); xprintf("is_poly_project:  bad type %.*s", _get_arr_size(
_temp626, 1u), _temp626.curr);})); return 0; _LL609:;} _LL599: return 0; _LL593:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_e,({ struct Cyc_List_List*
_temp627=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp627->hd=( void*) s; _temp627->tl= 0; _temp627;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc_GC_malloc_atomic_e,({ struct Cyc_List_List* _temp628=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp628->hd=( void*) s; _temp628->tl=
0; _temp628;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t,
struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic(
s);} return Cyc_Toc_malloc_ptr( s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({ struct Cyc_List_List*(* _temp629)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp632= rgn; struct Cyc_Absyn_Exp* _temp633= s; struct
Cyc_Absyn_Exp* _temp630[ 2u]={ _temp632, _temp633}; struct _tagged_arr _temp631={(
void*) _temp630,( void*) _temp630,( void*)( _temp630 + 2u)}; _temp629( _temp631);}),
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Toc__sfile_to_file_e,({ struct Cyc_List_List*
_temp634=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp634->hd=( void*) e; _temp634->tl= 0; _temp634;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({
struct Cyc_List_List* _temp635=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp635->hd=( void*) e; _temp635->tl= 0; _temp635;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_toplevel_tagged_arr( void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ int is_string= 0;{ void* _temp636=(
void*) e->r; void* _temp642; struct _tagged_arr _temp644; _LL638: if(*(( int*)
_temp636) == Cyc_Absyn_Const_e){ _LL643: _temp642=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp636)->f1; if(( unsigned int) _temp642 > 1u?*(( int*) _temp642) == Cyc_Absyn_String_c:
0){ _LL645: _temp644=(( struct Cyc_Absyn_String_c_struct*) _temp642)->f1; goto
_LL639;} else{ goto _LL640;}} else{ goto _LL640;} _LL640: goto _LL641; _LL639:
is_string= 1; goto _LL637; _LL641: goto _LL637; _LL637:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp649=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp649[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp650; _temp650.tag= Cyc_Absyn_ArrayType; _temp650.f1=(
void*) Cyc_Absyn_uchar_t; _temp650.f2= Cyc_Toc_mt_tq; _temp650.f3=( struct Cyc_Absyn_Exp*)
sz; _temp650;}); _temp649;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp646=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp646->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp647=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp647[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp648; _temp648.tag= Cyc_Absyn_Var_d;
_temp648.f1= vd; _temp648;}); _temp647;}), 0); _temp646->tl= Cyc_Toc_result_decls;
_temp646;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct Cyc_List_List*(* _temp651)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct _tuple4* _temp654=({ struct _tuple4*
_temp659=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp659->f1= 0;
_temp659->f2= xexp; _temp659;}); struct _tuple4* _temp655=({ struct _tuple4*
_temp658=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp658->f1= 0;
_temp658->f2= xexp; _temp658;}); struct _tuple4* _temp656=({ struct _tuple4*
_temp657=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp657->f1= 0;
_temp657->f2= xplussz; _temp657;}); struct _tuple4* _temp652[ 3u]={ _temp654,
_temp655, _temp656}; struct _tagged_arr _temp653={( void*) _temp652,( void*)
_temp652,( void*)( _temp652 + 3u)}; _temp651( _temp653);}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp660=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp660->break_lab= 0; _temp660->continue_lab= 0; _temp660->fallthru_info=
0; _temp660->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp660->toplevel= 1;
_temp660;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp661=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp661->break_lab= e->break_lab; _temp661->continue_lab=
e->continue_lab; _temp661->fallthru_info= e->fallthru_info; _temp661->varmap= e->varmap;
_temp661->toplevel= e->toplevel; _temp661;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp662=(* x).f1; struct
Cyc_List_List* _temp668; _LL664: if(( unsigned int) _temp662 > 1u?*(( int*)
_temp662) == Cyc_Absyn_Rel_n: 0){ _LL669: _temp668=(( struct Cyc_Absyn_Rel_n_struct*)
_temp662)->f1; goto _LL665;} else{ goto _LL666;} _LL666: goto _LL667; _LL665:(
int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct
_tagged_arr)({ struct _tagged_arr _temp670= Cyc_Absynpp_qvar2string( x); xprintf("Toc::add_varmap on Rel_n: %.*s\n",
_get_arr_size( _temp670, 1u), _temp670.curr);}))); _LL667: goto _LL663; _LL663:;}{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)(
ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=
0; ans->continue_lab= 0; return ans;} struct _tuple6{ struct _tagged_arr* f1;
struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_arr*
break_l, struct _tagged_arr* fallthru_l, struct Cyc_List_List* fallthru_binders,
struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List* _temp671= 0; for( 0;
fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*) _check_null(
fallthru_binders))->tl){ _temp671=({ struct Cyc_List_List* _temp672=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp672->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp672->tl=
_temp671; _temp672;});} _temp671=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp671);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp673=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp673->v=( void*) break_l; _temp673;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp674=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp674->v=( void*)({ struct _tuple6*
_temp675=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp675->f1=
fallthru_l; _temp675->f2= _temp671; _temp675->f3= next_case_env->varmap;
_temp675;}); _temp674;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp676=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp676->v=( void*)
break_l; _temp676;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_arr* next_l){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp677=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp677->v=( void*)({ struct _tuple6* _temp678=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp678->f1= next_l; _temp678->f2= 0;
_temp678->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp678;}); _temp677;});
return ans;} struct _tuple7{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tuple8{ struct _tuple0* f1; void* f2; struct Cyc_Absyn_Exp* f3; } ;
static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c( void* p, struct Cyc_Core_Opt*
aopt, struct Cyc_List_List* args, struct Cyc_List_List* typs){ if( args == 0){
return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("expecting a literal format string", sizeof( unsigned char), 34u));}{
struct _tagged_arr fmt_str;{ void* _temp679=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd)->r; void* _temp685; struct
_tagged_arr _temp687; _LL681: if(*(( int*) _temp679) == Cyc_Absyn_Const_e){
_LL686: _temp685=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp679)->f1; if((
unsigned int) _temp685 > 1u?*(( int*) _temp685) == Cyc_Absyn_String_c: 0){
_LL688: _temp687=(( struct Cyc_Absyn_String_c_struct*) _temp685)->f1; goto
_LL682;} else{ goto _LL683;}} else{ goto _LL683;} _LL683: goto _LL684; _LL682:
fmt_str= _temp687; goto _LL680; _LL684: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("expecting a literal format string",
sizeof( unsigned char), 34u)); _LL680:;}{ int len=( int) _get_arr_size( fmt_str,
sizeof( unsigned char)); int i= 0; for( i= 0; i < len; i ++){ if(*(( const
unsigned char*) _check_unknown_subscript( fmt_str, sizeof( unsigned char), i))
=='%'){ i ++; if( i < len?*(( const unsigned char*) _check_unknown_subscript(
fmt_str, sizeof( unsigned char), i)) =='s': 0){ break;}}} if( i == len){ if(
aopt != 0){ args=({ struct Cyc_List_List* _temp689=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp689->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp689->tl= args; _temp689;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp690= _new_region(); struct
_RegionHandle* r=& _temp690; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*)
_check_unknown_subscript( fmt_str, sizeof( unsigned char), i)); rev_fmt=({
struct Cyc_List_List* _temp691=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp691->hd=( void*)(( int) c); _temp691->tl=
rev_fmt; _temp691;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* _temp692=
Cyc_Formatstr_parse_conversionspecification( fmt_str, i + 1); if( _temp692 == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("bad format string",
sizeof( unsigned char), 18u));}{ struct _tuple7 _temp695; int _temp696;
unsigned char _temp698; struct Cyc_List_List* _temp700; struct Cyc_List_List*
_temp702; struct Cyc_List_List* _temp704; struct Cyc_List_List* _temp706; struct
_tuple7* _temp693=( struct _tuple7*)(( struct Cyc_Core_Opt*) _check_null(
_temp692))->v; _temp695=* _temp693; _LL707: _temp706= _temp695.f1; goto _LL705;
_LL705: _temp704= _temp695.f2; goto _LL703; _LL703: _temp702= _temp695.f3; goto
_LL701; _LL701: _temp700= _temp695.f4; goto _LL699; _LL699: _temp698= _temp695.f5;
goto _LL697; _LL697: _temp696= _temp695.f6; goto _LL694; _LL694: i= _temp696 - 1;
if( _temp698 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp708)( struct _RegionHandle*, struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_arr)) Cyc_List_rlist; struct _RegionHandle* _temp715= r; struct Cyc_List_List*
_temp711= _temp706; struct Cyc_List_List* _temp712= _temp704; struct Cyc_List_List*
_temp713= _temp702; struct Cyc_List_List* _temp714= _temp700; struct Cyc_List_List*
_temp709[ 4u]={ _temp711, _temp712, _temp713, _temp714}; struct _tagged_arr
_temp710={( void*) _temp709,( void*) _temp709,( void*)( _temp709 + 4u)};
_temp708( _temp715, _temp710);})), rev_fmt); rev_fmt=({ struct Cyc_List_List*
_temp716=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp716->hd=( void*)(( int) _temp698); _temp716->tl= rev_fmt; _temp716;});{
struct Cyc_List_List* _temp717= _temp704; struct Cyc_List_List _temp724; struct
Cyc_List_List* _temp725; int _temp727; _LL719: if( _temp717 == 0){ goto _LL721;}
else{ _temp724=* _temp717; _LL728: _temp727=( int) _temp724.hd; goto _LL726;
_LL726: _temp725= _temp724.tl; if( _temp725 == 0){ goto _LL723;} else{ goto
_LL721;}} _LL723: if( _temp727 =='*'){ goto _LL720;} else{ goto _LL721;} _LL721:
goto _LL722; _LL720: { struct _tuple0* _temp729= Cyc_Toc_temp_var(); rev_temps=({
struct Cyc_List_List* _temp730=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp730->hd=( void*)({ struct _tuple8* _temp731=(
struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8)); _temp731->f1=
_temp729; _temp731->f2=( void*)(( struct Cyc_List_List*) _check_null( typs))->hd;
_temp731->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd;
_temp731;}); _temp730->tl= rev_temps; _temp730;}); rev_result=({ struct Cyc_List_List*
_temp732=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp732->hd=( void*) Cyc_Absyn_var_exp( _temp729, 0); _temp732->tl= rev_result;
_temp732;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL718;} _LL722: goto
_LL718; _LL718:;}{ struct Cyc_List_List* _temp733= _temp702; struct Cyc_List_List
_temp740; struct Cyc_List_List* _temp741; struct Cyc_List_List _temp743; struct
Cyc_List_List* _temp744; int _temp746; int _temp748; _LL735: if( _temp733 == 0){
goto _LL737;} else{ _temp740=* _temp733; _LL749: _temp748=( int) _temp740.hd;
goto _LL742; _LL742: _temp741= _temp740.tl; if( _temp741 == 0){ goto _LL737;}
else{ _temp743=* _temp741; _LL747: _temp746=( int) _temp743.hd; goto _LL745;
_LL745: _temp744= _temp743.tl; if( _temp744 == 0){ goto _LL739;} else{ goto
_LL737;}}} _LL739: if( _temp748 =='.'? _temp746 =='*': 0){ goto _LL736;} else{
goto _LL737;} _LL737: goto _LL738; _LL736: { struct _tuple0* _temp750= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp751=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp751->hd=( void*)({
struct _tuple8* _temp752=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp752->f1= _temp750; _temp752->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp752->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp752;}); _temp751->tl= rev_temps; _temp751;});
rev_result=({ struct Cyc_List_List* _temp753=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp753->hd=( void*) Cyc_Absyn_var_exp(
_temp750, 0); _temp753->tl= rev_result; _temp753;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL734;} _LL738: goto _LL734; _LL734:;} if( _temp698 !='%'){ if( args == 0?
1: typs == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct
_tagged_arr)({ struct _tagged_arr _temp754= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_get_arr_size( _temp754, 1u), _temp754.curr);}));}{ struct _tuple0* _temp755=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp756=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp756->hd=( void*)({
struct _tuple8* _temp757=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp757->f1= _temp755; _temp757->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp757->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp757;}); _temp756->tl= rev_temps; _temp756;});
rev_result=({ struct Cyc_List_List* _temp758=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp758->hd=( void*) Cyc_Absyn_var_exp(
_temp755, 0); _temp758->tl= rev_result; _temp758;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp759=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp759->hd=( void*)(( int)'.');
_temp759->tl= rev_fmt; _temp759;}); rev_fmt=({ struct Cyc_List_List* _temp760=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp760->hd=( void*)(( int)'*'); _temp760->tl= rev_fmt; _temp760;}); rev_fmt=({
struct Cyc_List_List* _temp761=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp761->hd=( void*)(( int)'s'); _temp761->tl=
rev_fmt; _temp761;});{ struct _tuple0* _temp762= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp763= Cyc_Absyn_var_exp( _temp762, 0); rev_temps=({ struct Cyc_List_List*
_temp764=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp764->hd=( void*)({ struct _tuple8* _temp765=( struct _tuple8*)
_region_malloc( r, sizeof( struct _tuple8)); _temp765->f1= _temp762; _temp765->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp765->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp765;});
_temp764->tl= rev_temps; _temp764;});{ struct Cyc_Absyn_Exp* _temp766= Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp770)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp773= _temp763; struct Cyc_Absyn_Exp* _temp774= Cyc_Absyn_uint_exp( 1, 0);
struct Cyc_Absyn_Exp* _temp771[ 2u]={ _temp773, _temp774}; struct _tagged_arr
_temp772={( void*) _temp771,( void*) _temp771,( void*)( _temp771 + 2u)};
_temp770( _temp772);}), 0); struct Cyc_Absyn_Exp* _temp767= Cyc_Absyn_structmember_exp(
_temp763, Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List* _temp768=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp768->hd=( void*) _temp766; _temp768->tl= rev_result; _temp768;});
rev_result=({ struct Cyc_List_List* _temp769=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp769->hd=( void*)
_temp767; _temp769->tl= rev_result; _temp769;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}{
struct Cyc_List_List* _temp775=({ struct Cyc_List_List* _temp778=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp778->hd=( void*) Cyc_Absyn_string_exp((
struct _tagged_arr) Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_fmt)), 0); _temp778->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_result); _temp778;}); if( aopt != 0){
_temp775=({ struct Cyc_List_List* _temp776=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp776->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp776->tl= _temp775; _temp776;});}{
struct Cyc_Absyn_Exp* _temp777= Cyc_Absyn_primop_exp( p, _temp775, 0); s= Cyc_Absyn_exp_stmt(
_temp777, 0); for( 0; rev_temps != 0; rev_temps=(( struct Cyc_List_List*)
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
e){ return({ struct _tuple3* _temp779=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp779->f1= Cyc_Toc_mt_tq; _temp779->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp779;});} static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp780=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp780->f1= 0; _temp780->f2= e; _temp780;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp781= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp781): Cyc_Toc_malloc_ptr( _temp781), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp781), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp782=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp782 != 0; _temp782=((
struct Cyc_List_List*) _check_null( _temp782))->tl){ struct _tuple4 _temp785;
struct Cyc_Absyn_Exp* _temp786; struct Cyc_List_List* _temp788; struct _tuple4*
_temp783=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp782))->hd;
_temp785=* _temp783; _LL789: _temp788= _temp785.f1; goto _LL787; _LL787:
_temp786= _temp785.f2; goto _LL784; _LL784: { struct Cyc_Absyn_Exp* e_index; if(
_temp788 == 0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp788))->tl != 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u));}{ void* _temp790=( void*)(( struct Cyc_List_List*)
_check_null( _temp788))->hd; void* _temp791= _temp790; struct Cyc_Absyn_Exp*
_temp797; struct _tagged_arr* _temp799; _LL793: if(*(( int*) _temp791) == Cyc_Absyn_ArrayElement){
_LL798: _temp797=(( struct Cyc_Absyn_ArrayElement_struct*) _temp791)->f1; goto
_LL794;} else{ goto _LL795;} _LL795: if(*(( int*) _temp791) == Cyc_Absyn_FieldName){
_LL800: _temp799=(( struct Cyc_Absyn_FieldName_struct*) _temp791)->f1; goto
_LL796;} else{ goto _LL792;} _LL794: Cyc_Toc_exp_to_c( nv, _temp797); e_index=
_temp797; goto _LL792; _LL796: e_index=(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("field name designators in array",
sizeof( unsigned char), 32u)); goto _LL792; _LL792:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp801=( void*)
_temp786->r; struct Cyc_List_List* _temp811; struct Cyc_Absyn_Exp* _temp813;
struct Cyc_Absyn_Exp* _temp815; struct Cyc_Absyn_Vardecl* _temp817; struct Cyc_List_List*
_temp819; void* _temp821; _LL803: if(*(( int*) _temp801) == Cyc_Absyn_Array_e){
_LL812: _temp811=(( struct Cyc_Absyn_Array_e_struct*) _temp801)->f1; goto _LL804;}
else{ goto _LL805;} _LL805: if(*(( int*) _temp801) == Cyc_Absyn_Comprehension_e){
_LL818: _temp817=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp801)->f1;
goto _LL816; _LL816: _temp815=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp801)->f2; goto _LL814; _LL814: _temp813=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp801)->f3; goto _LL806;} else{ goto _LL807;} _LL807: if(*(( int*) _temp801)
== Cyc_Absyn_AnonStruct_e){ _LL822: _temp821=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp801)->f1; goto _LL820; _LL820: _temp819=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp801)->f2; goto _LL808;} else{ goto _LL809;} _LL809: goto _LL810; _LL804: s=
Cyc_Toc_init_array( nv, lval, _temp811, s); goto _LL802; _LL806: s= Cyc_Toc_init_comprehension(
nv, lval, _temp817, _temp815, _temp813, s, 0); goto _LL802; _LL808: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp821, _temp819, s); goto _LL802; _LL810: Cyc_Toc_exp_to_c( nv,
_temp786); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp786, 0), s, 0); goto _LL802; _LL802:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp823= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp823, Cyc_Absyn_varb_exp( _temp823,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp846=( struct Cyc_Absyn_Local_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp846[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp847; _temp847.tag= Cyc_Absyn_Local_b; _temp847.f1= vd; _temp847;});
_temp846;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp823, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp823, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp823, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp823, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp824=( void*) e2->r; struct Cyc_List_List* _temp834; struct Cyc_Absyn_Exp*
_temp836; struct Cyc_Absyn_Exp* _temp838; struct Cyc_Absyn_Vardecl* _temp840;
struct Cyc_List_List* _temp842; void* _temp844; _LL826: if(*(( int*) _temp824)
== Cyc_Absyn_Array_e){ _LL835: _temp834=(( struct Cyc_Absyn_Array_e_struct*)
_temp824)->f1; goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp824)
== Cyc_Absyn_Comprehension_e){ _LL841: _temp840=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp824)->f1; goto _LL839; _LL839: _temp838=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp824)->f2; goto _LL837; _LL837: _temp836=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp824)->f3; goto _LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp824)
== Cyc_Absyn_AnonStruct_e){ _LL845: _temp844=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp824)->f1; goto _LL843; _LL843: _temp842=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp824)->f2; goto _LL831;} else{ goto _LL832;} _LL832: goto _LL833; _LL827:
body= Cyc_Toc_init_array( nv2, lval, _temp834, Cyc_Absyn_skip_stmt( 0)); goto
_LL825; _LL829: body= Cyc_Toc_init_comprehension( nv2, lval, _temp840, _temp838,
_temp836, Cyc_Absyn_skip_stmt( 0), 0); goto _LL825; _LL831: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp844, _temp842, Cyc_Absyn_skip_stmt( 0)); goto _LL825; _LL833: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL825; _LL825:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp823, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp848=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp848 != 0; _temp848=(( struct Cyc_List_List*)
_check_null( _temp848))->tl){ struct _tuple4 _temp851; struct Cyc_Absyn_Exp*
_temp852; struct Cyc_List_List* _temp854; struct _tuple4* _temp849=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp848))->hd; _temp851=*
_temp849; _LL855: _temp854= _temp851.f1; goto _LL853; _LL853: _temp852= _temp851.f2;
goto _LL850; _LL850: if( _temp854 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("empty designator list", sizeof( unsigned char), 22u));} if((( struct
Cyc_List_List*) _check_null( _temp854))->tl != 0){(( int(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u));}{ void* _temp856=( void*)(( struct Cyc_List_List*)
_check_null( _temp854))->hd; struct _tagged_arr* _temp862; _LL858: if(*(( int*)
_temp856) == Cyc_Absyn_FieldName){ _LL863: _temp862=(( struct Cyc_Absyn_FieldName_struct*)
_temp856)->f1; goto _LL859;} else{ goto _LL860;} _LL860: goto _LL861; _LL859: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp862, 0);{ void*
_temp864=( void*) _temp852->r; struct Cyc_List_List* _temp874; struct Cyc_Absyn_Exp*
_temp876; struct Cyc_Absyn_Exp* _temp878; struct Cyc_Absyn_Vardecl* _temp880;
struct Cyc_List_List* _temp882; void* _temp884; _LL866: if(*(( int*) _temp864)
== Cyc_Absyn_Array_e){ _LL875: _temp874=(( struct Cyc_Absyn_Array_e_struct*)
_temp864)->f1; goto _LL867;} else{ goto _LL868;} _LL868: if(*(( int*) _temp864)
== Cyc_Absyn_Comprehension_e){ _LL881: _temp880=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp864)->f1; goto _LL879; _LL879: _temp878=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp864)->f2; goto _LL877; _LL877: _temp876=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp864)->f3; goto _LL869;} else{ goto _LL870;} _LL870: if(*(( int*) _temp864)
== Cyc_Absyn_AnonStruct_e){ _LL885: _temp884=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp864)->f1; goto _LL883; _LL883: _temp882=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp864)->f2; goto _LL871;} else{ goto _LL872;} _LL872: goto _LL873; _LL867: s=
Cyc_Toc_init_array( nv, lval, _temp874, s); goto _LL865; _LL869: s= Cyc_Toc_init_comprehension(
nv, lval, _temp880, _temp878, _temp876, s, 0); goto _LL865; _LL871: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp884, _temp882, s); goto _LL865; _LL873: Cyc_Toc_exp_to_c( nv,
_temp852); if( Cyc_Toc_is_poly_field( struct_type, _temp862)){ _temp852= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp852, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp852, 0), 0), s, 0); goto _LL865; _LL865:;} goto
_LL857;} _LL861:( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u))); _LL857:;}}}
return s;} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env*
nv, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
_RegionHandle _temp886= _new_region(); struct _RegionHandle* r=& _temp886;
_push_region( r);{ struct Cyc_List_List* _temp887=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c, es); struct _tagged_arr* _temp888= Cyc_Toc_add_tuple_type(
_temp887); struct _tuple0* _temp889= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp890= Cyc_Absyn_var_exp( _temp889, 0); struct Cyc_Absyn_Stmt* _temp891= Cyc_Absyn_exp_stmt(
_temp890, 0); struct Cyc_Absyn_Exp*(* _temp892)( struct Cyc_Absyn_Exp*, struct
_tagged_arr*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1; struct Cyc_List_List* _temp893=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)(
r, es);{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp893);
for( 0; _temp893 != 0;( _temp893=(( struct Cyc_List_List*) _check_null( _temp893))->tl,
-- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp893))->hd; struct Cyc_Absyn_Exp* lval= _temp892( _temp890, Cyc_Toc_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp894=( void*) e->r; struct Cyc_List_List*
_temp902; struct Cyc_Absyn_Exp* _temp904; struct Cyc_Absyn_Exp* _temp906; struct
Cyc_Absyn_Vardecl* _temp908; _LL896: if(*(( int*) _temp894) == Cyc_Absyn_Array_e){
_LL903: _temp902=(( struct Cyc_Absyn_Array_e_struct*) _temp894)->f1; goto _LL897;}
else{ goto _LL898;} _LL898: if(*(( int*) _temp894) == Cyc_Absyn_Comprehension_e){
_LL909: _temp908=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp894)->f1;
goto _LL907; _LL907: _temp906=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp894)->f2; goto _LL905; _LL905: _temp904=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp894)->f3; goto _LL899;} else{ goto _LL900;} _LL900: goto _LL901; _LL897:
_temp891= Cyc_Toc_init_array( nv, lval, _temp902, _temp891); goto _LL895; _LL899:
_temp891= Cyc_Toc_init_comprehension( nv, lval, _temp908, _temp906, _temp904,
_temp891, 0); goto _LL895; _LL901: Cyc_Toc_exp_to_c( nv, e); _temp891= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp892( _temp890, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp891, 0); goto _LL895; _LL895:;}}}{ struct Cyc_Absyn_Exp*
_temp910= Cyc_Toc_make_struct( nv, _temp889, Cyc_Absyn_strct( _temp888),
_temp891, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp910;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp911= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp912= Cyc_Absyn_var_exp( _temp911, 0); struct Cyc_Absyn_Stmt*
_temp913= Cyc_Absyn_exp_stmt( _temp912, 0); struct Cyc_Absyn_Exp*(* _temp914)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp915= _new_region(); struct _RegionHandle* r=& _temp915;
_push_region( r);{ struct Cyc_List_List* _temp916=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp916 != 0; _temp916=(( struct Cyc_List_List*) _check_null( _temp916))->tl){
struct _tuple4 _temp919; struct Cyc_Absyn_Exp* _temp920; struct Cyc_List_List*
_temp922; struct _tuple4* _temp917=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp916))->hd; _temp919=* _temp917; _LL923: _temp922= _temp919.f1;
goto _LL921; _LL921: _temp920= _temp919.f2; goto _LL918; _LL918: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp920->topt))->v): 0; if( _temp922 == 0){(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("empty designator list", sizeof( unsigned char), 22u));}
if((( struct Cyc_List_List*) _check_null( _temp922))->tl != 0){ struct _tuple0*
_temp924= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp925= Cyc_Absyn_var_exp(
_temp924, 0); for( 0; _temp922 != 0; _temp922=(( struct Cyc_List_List*)
_check_null( _temp922))->tl){ void* _temp926=( void*)(( struct Cyc_List_List*)
_check_null( _temp922))->hd; struct _tagged_arr* _temp932; _LL928: if(*(( int*)
_temp926) == Cyc_Absyn_FieldName){ _LL933: _temp932=(( struct Cyc_Absyn_FieldName_struct*)
_temp926)->f1; goto _LL929;} else{ goto _LL930;} _LL930: goto _LL931; _LL929:
if( Cyc_Toc_is_poly_field( struct_type, _temp932)){ _temp925= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp925, 0);} _temp913= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp914( _temp912, _temp932, 0), _temp925, 0), 0),
_temp913, 0); goto _LL927; _LL931:( int) _throw((( void*(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)( _tag_arr("array designator in struct", sizeof(
unsigned char), 27u))); _LL927:;} Cyc_Toc_exp_to_c( nv, _temp920); _temp913= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp925, _temp920, 0), 0), _temp913,
0);} else{ void* _temp934=( void*)(( struct Cyc_List_List*) _check_null(
_temp922))->hd; struct _tagged_arr* _temp940; _LL936: if(*(( int*) _temp934) ==
Cyc_Absyn_FieldName){ _LL941: _temp940=(( struct Cyc_Absyn_FieldName_struct*)
_temp934)->f1; goto _LL937;} else{ goto _LL938;} _LL938: goto _LL939; _LL937: {
struct Cyc_Absyn_Exp* lval= _temp914( _temp912, _temp940, 0);{ void* _temp942=(
void*) _temp920->r; struct Cyc_List_List* _temp952; struct Cyc_Absyn_Exp*
_temp954; struct Cyc_Absyn_Exp* _temp956; struct Cyc_Absyn_Vardecl* _temp958;
struct Cyc_List_List* _temp960; void* _temp962; _LL944: if(*(( int*) _temp942)
== Cyc_Absyn_Array_e){ _LL953: _temp952=(( struct Cyc_Absyn_Array_e_struct*)
_temp942)->f1; goto _LL945;} else{ goto _LL946;} _LL946: if(*(( int*) _temp942)
== Cyc_Absyn_Comprehension_e){ _LL959: _temp958=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp942)->f1; goto _LL957; _LL957: _temp956=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp942)->f2; goto _LL955; _LL955: _temp954=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp942)->f3; goto _LL947;} else{ goto _LL948;} _LL948: if(*(( int*) _temp942)
== Cyc_Absyn_AnonStruct_e){ _LL963: _temp962=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp942)->f1; goto _LL961; _LL961: _temp960=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp942)->f2; goto _LL949;} else{ goto _LL950;} _LL950: goto _LL951; _LL945:
_temp913= Cyc_Toc_init_array( nv, lval, _temp952, _temp913); goto _LL943; _LL947:
_temp913= Cyc_Toc_init_comprehension( nv, lval, _temp958, _temp956, _temp954,
_temp913, 0); goto _LL943; _LL949: _temp913= Cyc_Toc_init_anon_struct( nv, lval,
_temp962, _temp960, _temp913); goto _LL943; _LL951: Cyc_Toc_exp_to_c( nv,
_temp920); if( Cyc_Toc_is_poly_field( struct_type, _temp940)){ _temp920= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp920, 0);} _temp913= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp920, 0), 0), _temp913, 0); goto _LL943; _LL943:;}
goto _LL935;} _LL939:( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u))); _LL935:;}}}{
struct Cyc_Absyn_Exp* _temp964= Cyc_Toc_make_struct( nv, _temp911, Cyc_Absyn_strctq(
tdn), _temp913, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp964;};
_pop_region( r);} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp*
f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp*
el, struct _tuple9* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2,
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp*
e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp965=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp965[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp966; _temp966.tag= Cyc_Absyn_Increment_e;
_temp966.f1= e1; _temp966.f2=( void*) incr; _temp966;}); _temp965;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp967=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp977; struct Cyc_Absyn_Exp*
_temp979; void* _temp981; struct _tagged_arr* _temp983; struct Cyc_Absyn_Exp*
_temp985; _LL969: if(*(( int*) _temp967) == Cyc_Absyn_StmtExp_e){ _LL978:
_temp977=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp967)->f1; goto _LL970;}
else{ goto _LL971;} _LL971: if(*(( int*) _temp967) == Cyc_Absyn_Cast_e){ _LL982:
_temp981=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp967)->f1; goto _LL980;
_LL980: _temp979=(( struct Cyc_Absyn_Cast_e_struct*) _temp967)->f2; goto _LL972;}
else{ goto _LL973;} _LL973: if(*(( int*) _temp967) == Cyc_Absyn_StructMember_e){
_LL986: _temp985=(( struct Cyc_Absyn_StructMember_e_struct*) _temp967)->f1; goto
_LL984; _LL984: _temp983=(( struct Cyc_Absyn_StructMember_e_struct*) _temp967)->f2;
goto _LL974;} else{ goto _LL975;} _LL975: goto _LL976; _LL970:(( void(*)( struct
Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp977, fs,
f, f_env); goto _LL968; _LL972:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp979, fs, f, f_env); goto _LL968; _LL974:( void*)( e1->r=( void*)(( void*)
_temp985->r));(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
e1,({ struct Cyc_List_List* _temp987=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp987->hd=( void*) _temp983; _temp987->tl= fs;
_temp987;}), f, f_env); goto _LL968; _LL976: if( ! Cyc_Absyn_is_lvalue( e1)){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp988= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_get_arr_size( _temp988, 1u), _temp988.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL968;} _LL968:;}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp989=( void*) s->r; struct Cyc_Absyn_Exp* _temp999; struct Cyc_Absyn_Stmt*
_temp1001; struct Cyc_Absyn_Decl* _temp1003; struct Cyc_Absyn_Stmt* _temp1005;
struct Cyc_Absyn_Stmt* _temp1007; _LL991: if(( unsigned int) _temp989 > 1u?*((
int*) _temp989) == Cyc_Absyn_Exp_s: 0){ _LL1000: _temp999=(( struct Cyc_Absyn_Exp_s_struct*)
_temp989)->f1; goto _LL992;} else{ goto _LL993;} _LL993: if(( unsigned int)
_temp989 > 1u?*(( int*) _temp989) == Cyc_Absyn_Decl_s: 0){ _LL1004: _temp1003=((
struct Cyc_Absyn_Decl_s_struct*) _temp989)->f1; goto _LL1002; _LL1002: _temp1001=((
struct Cyc_Absyn_Decl_s_struct*) _temp989)->f2; goto _LL994;} else{ goto _LL995;}
_LL995: if(( unsigned int) _temp989 > 1u?*(( int*) _temp989) == Cyc_Absyn_Seq_s:
0){ _LL1008: _temp1007=(( struct Cyc_Absyn_Seq_s_struct*) _temp989)->f1; goto
_LL1006; _LL1006: _temp1005=(( struct Cyc_Absyn_Seq_s_struct*) _temp989)->f2;
goto _LL996;} else{ goto _LL997;} _LL997: goto _LL998; _LL992:(( void(*)( struct
Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp999, fs, f,
f_env); goto _LL990; _LL994:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1001, fs, f, f_env); goto _LL990; _LL996:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1005, fs, f, f_env);
goto _LL990; _LL998:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1009= Cyc_Absynpp_stmt2string( s);
xprintf("lvalue_assign_stmt: %.*s", _get_arr_size( _temp1009, 1u), _temp1009.curr);}));
goto _LL990; _LL990:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){ return 0;}
result=({ struct Cyc_List_List* _temp1010=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1010->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1010->tl= 0;
_temp1010;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp1011=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1011->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1011->tl= 0;
_temp1011;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x))
Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1012=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1012->f1= 0; _temp1012->f2= e;
_temp1012;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){
void* _temp1013= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp1019;
_LL1015: if(( unsigned int) _temp1013 > 4u?*(( int*) _temp1013) == Cyc_Absyn_PointerType:
0){ _LL1020: _temp1019=(( struct Cyc_Absyn_PointerType_struct*) _temp1013)->f1;
goto _LL1016;} else{ goto _LL1017;} _LL1017: goto _LL1018; _LL1016: return
_temp1019; _LL1018:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("get_ptr_typ:  not a pointer!", sizeof( unsigned char), 29u))); _LL1014:;}
struct _tuple10{ void* f1; void* f2; } ; struct _tuple11{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp1021=( void*) e->r; if( e->topt == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp1022= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_get_arr_size( _temp1022, 1u), _temp1022.curr);})); return;}{ void* old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void* _temp1023=
_temp1021; void* _temp1103; void* _temp1105; void* _temp1107; struct _tuple0*
_temp1109; struct _tuple0* _temp1111; struct Cyc_List_List* _temp1113; void*
_temp1115; void* _temp1117; struct Cyc_Absyn_Exp* _temp1119; struct Cyc_Absyn_Exp*
_temp1121; struct Cyc_Core_Opt* _temp1123; struct Cyc_Absyn_Exp* _temp1125;
struct Cyc_Absyn_Exp* _temp1127; struct Cyc_Absyn_Exp* _temp1129; struct Cyc_Absyn_Exp*
_temp1131; struct Cyc_Absyn_Exp* _temp1133; struct Cyc_Absyn_Exp* _temp1135;
struct Cyc_List_List* _temp1137; struct Cyc_Absyn_Exp* _temp1139; struct Cyc_Absyn_VarargCallInfo*
_temp1141; struct Cyc_List_List* _temp1143; struct Cyc_Absyn_Exp* _temp1145;
struct Cyc_Absyn_VarargCallInfo* _temp1147; struct Cyc_Absyn_VarargCallInfo
_temp1149; struct Cyc_Absyn_VarargInfo* _temp1150; struct Cyc_List_List*
_temp1152; int _temp1154; struct Cyc_List_List* _temp1156; struct Cyc_Absyn_Exp*
_temp1158; struct Cyc_Absyn_Exp* _temp1160; struct Cyc_Absyn_Exp* _temp1162;
struct Cyc_List_List* _temp1164; struct Cyc_Absyn_Exp* _temp1166; struct Cyc_Absyn_Exp*
_temp1168; void* _temp1170; void** _temp1172; struct Cyc_Absyn_Exp* _temp1173;
struct Cyc_Absyn_Exp* _temp1175; struct Cyc_Absyn_Exp* _temp1177; struct Cyc_Absyn_Exp*
_temp1179; void* _temp1181; struct _tagged_arr* _temp1183; void* _temp1185;
struct Cyc_Absyn_Exp* _temp1187; struct _tagged_arr* _temp1189; struct Cyc_Absyn_Exp*
_temp1191; struct _tagged_arr* _temp1193; struct Cyc_Absyn_Exp* _temp1195;
struct Cyc_Absyn_Exp* _temp1197; struct Cyc_Absyn_Exp* _temp1199; struct Cyc_List_List*
_temp1201; struct Cyc_List_List* _temp1203; struct Cyc_Absyn_Exp* _temp1205;
struct Cyc_Absyn_Exp* _temp1207; struct Cyc_Absyn_Vardecl* _temp1209; struct Cyc_Absyn_Structdecl*
_temp1211; struct Cyc_List_List* _temp1213; struct Cyc_Core_Opt* _temp1215;
struct _tuple0* _temp1217; struct Cyc_List_List* _temp1219; void* _temp1221;
struct Cyc_Absyn_Tunionfield* _temp1223; struct Cyc_Absyn_Tuniondecl* _temp1225;
struct Cyc_List_List* _temp1227; struct Cyc_Core_Opt* _temp1229; struct Cyc_Core_Opt*
_temp1231; struct Cyc_Absyn_Tunionfield* _temp1233; struct Cyc_Absyn_Tuniondecl*
_temp1235; struct Cyc_List_List* _temp1237; struct Cyc_Core_Opt* _temp1239;
struct Cyc_Core_Opt* _temp1241; struct Cyc_Absyn_Enumfield* _temp1243; struct
Cyc_Absyn_Enumdecl* _temp1245; struct _tuple0* _temp1247; void* _temp1249;
struct Cyc_Absyn_Exp* _temp1251; struct Cyc_Absyn_Stmt* _temp1253; struct Cyc_List_List*
_temp1255; struct Cyc_Core_Opt* _temp1257; struct Cyc_List_List* _temp1259;
struct _tuple1* _temp1261; struct Cyc_Absyn_Fndecl* _temp1263; struct Cyc_Absyn_Exp*
_temp1265; _LL1025: if(*(( int*) _temp1023) == Cyc_Absyn_Const_e){ _LL1104:
_temp1103=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1023)->f1; if(
_temp1103 ==( void*) Cyc_Absyn_Null_c){ goto _LL1026;} else{ goto _LL1027;}}
else{ goto _LL1027;} _LL1027: if(*(( int*) _temp1023) == Cyc_Absyn_Const_e){
_LL1106: _temp1105=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1023)->f1;
goto _LL1028;} else{ goto _LL1029;} _LL1029: if(*(( int*) _temp1023) == Cyc_Absyn_Var_e){
_LL1110: _temp1109=(( struct Cyc_Absyn_Var_e_struct*) _temp1023)->f1; goto
_LL1108; _LL1108: _temp1107=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1023)->f2;
goto _LL1030;} else{ goto _LL1031;} _LL1031: if(*(( int*) _temp1023) == Cyc_Absyn_UnknownId_e){
_LL1112: _temp1111=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1023)->f1; goto
_LL1032;} else{ goto _LL1033;} _LL1033: if(*(( int*) _temp1023) == Cyc_Absyn_Primop_e){
_LL1116: _temp1115=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1023)->f1;
goto _LL1114; _LL1114: _temp1113=(( struct Cyc_Absyn_Primop_e_struct*) _temp1023)->f2;
goto _LL1034;} else{ goto _LL1035;} _LL1035: if(*(( int*) _temp1023) == Cyc_Absyn_Increment_e){
_LL1120: _temp1119=(( struct Cyc_Absyn_Increment_e_struct*) _temp1023)->f1; goto
_LL1118; _LL1118: _temp1117=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1023)->f2; goto _LL1036;} else{ goto _LL1037;} _LL1037: if(*(( int*)
_temp1023) == Cyc_Absyn_AssignOp_e){ _LL1126: _temp1125=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1023)->f1; goto _LL1124; _LL1124: _temp1123=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1023)->f2; goto _LL1122; _LL1122: _temp1121=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1023)->f3; goto _LL1038;} else{ goto _LL1039;} _LL1039: if(*(( int*)
_temp1023) == Cyc_Absyn_Conditional_e){ _LL1132: _temp1131=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1023)->f1; goto _LL1130; _LL1130: _temp1129=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1023)->f2; goto _LL1128; _LL1128: _temp1127=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1023)->f3; goto _LL1040;} else{ goto _LL1041;} _LL1041: if(*(( int*)
_temp1023) == Cyc_Absyn_SeqExp_e){ _LL1136: _temp1135=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1023)->f1; goto _LL1134; _LL1134: _temp1133=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1023)->f2; goto _LL1042;} else{ goto _LL1043;} _LL1043: if(*(( int*)
_temp1023) == Cyc_Absyn_UnknownCall_e){ _LL1140: _temp1139=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1023)->f1; goto _LL1138; _LL1138: _temp1137=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1023)->f2; goto _LL1044;} else{ goto _LL1045;} _LL1045: if(*(( int*)
_temp1023) == Cyc_Absyn_FnCall_e){ _LL1146: _temp1145=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1023)->f1; goto _LL1144; _LL1144: _temp1143=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1023)->f2; goto _LL1142; _LL1142: _temp1141=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1023)->f3; if( _temp1141 == 0){ goto _LL1046;} else{ goto _LL1047;}} else{
goto _LL1047;} _LL1047: if(*(( int*) _temp1023) == Cyc_Absyn_FnCall_e){ _LL1159:
_temp1158=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1023)->f1; goto _LL1157;
_LL1157: _temp1156=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1023)->f2; goto
_LL1148; _LL1148: _temp1147=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1023)->f3;
if( _temp1147 == 0){ goto _LL1049;} else{ _temp1149=* _temp1147; _LL1155:
_temp1154= _temp1149.num_varargs; goto _LL1153; _LL1153: _temp1152= _temp1149.injectors;
goto _LL1151; _LL1151: _temp1150= _temp1149.vai; goto _LL1048;}} else{ goto
_LL1049;} _LL1049: if(*(( int*) _temp1023) == Cyc_Absyn_Throw_e){ _LL1161:
_temp1160=(( struct Cyc_Absyn_Throw_e_struct*) _temp1023)->f1; goto _LL1050;}
else{ goto _LL1051;} _LL1051: if(*(( int*) _temp1023) == Cyc_Absyn_NoInstantiate_e){
_LL1163: _temp1162=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1023)->f1;
goto _LL1052;} else{ goto _LL1053;} _LL1053: if(*(( int*) _temp1023) == Cyc_Absyn_Instantiate_e){
_LL1167: _temp1166=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1023)->f1;
goto _LL1165; _LL1165: _temp1164=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1023)->f2; goto _LL1054;} else{ goto _LL1055;} _LL1055: if(*(( int*)
_temp1023) == Cyc_Absyn_Cast_e){ _LL1171: _temp1170=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1023)->f1; _temp1172=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1023)->f1); goto _LL1169; _LL1169: _temp1168=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1023)->f2; goto _LL1056;} else{ goto _LL1057;} _LL1057: if(*(( int*)
_temp1023) == Cyc_Absyn_Address_e){ _LL1174: _temp1173=(( struct Cyc_Absyn_Address_e_struct*)
_temp1023)->f1; goto _LL1058;} else{ goto _LL1059;} _LL1059: if(*(( int*)
_temp1023) == Cyc_Absyn_New_e){ _LL1178: _temp1177=(( struct Cyc_Absyn_New_e_struct*)
_temp1023)->f1; goto _LL1176; _LL1176: _temp1175=(( struct Cyc_Absyn_New_e_struct*)
_temp1023)->f2; goto _LL1060;} else{ goto _LL1061;} _LL1061: if(*(( int*)
_temp1023) == Cyc_Absyn_Sizeofexp_e){ _LL1180: _temp1179=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1023)->f1; goto _LL1062;} else{ goto _LL1063;} _LL1063: if(*(( int*)
_temp1023) == Cyc_Absyn_Sizeoftyp_e){ _LL1182: _temp1181=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1023)->f1; goto _LL1064;} else{ goto _LL1065;} _LL1065: if(*(( int*)
_temp1023) == Cyc_Absyn_Offsetof_e){ _LL1186: _temp1185=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1023)->f1; goto _LL1184; _LL1184: _temp1183=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1023)->f2; goto _LL1066;} else{ goto _LL1067;} _LL1067: if(*(( int*)
_temp1023) == Cyc_Absyn_Deref_e){ _LL1188: _temp1187=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1023)->f1; goto _LL1068;} else{ goto _LL1069;} _LL1069: if(*(( int*)
_temp1023) == Cyc_Absyn_StructMember_e){ _LL1192: _temp1191=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1023)->f1; goto _LL1190; _LL1190: _temp1189=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1023)->f2; goto _LL1070;} else{ goto _LL1071;} _LL1071: if(*(( int*)
_temp1023) == Cyc_Absyn_StructArrow_e){ _LL1196: _temp1195=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1023)->f1; goto _LL1194; _LL1194: _temp1193=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1023)->f2; goto _LL1072;} else{ goto _LL1073;} _LL1073: if(*(( int*)
_temp1023) == Cyc_Absyn_Subscript_e){ _LL1200: _temp1199=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1023)->f1; goto _LL1198; _LL1198: _temp1197=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1023)->f2; goto _LL1074;} else{ goto _LL1075;} _LL1075: if(*(( int*)
_temp1023) == Cyc_Absyn_Tuple_e){ _LL1202: _temp1201=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1023)->f1; goto _LL1076;} else{ goto _LL1077;} _LL1077: if(*(( int*)
_temp1023) == Cyc_Absyn_Array_e){ _LL1204: _temp1203=(( struct Cyc_Absyn_Array_e_struct*)
_temp1023)->f1; goto _LL1078;} else{ goto _LL1079;} _LL1079: if(*(( int*)
_temp1023) == Cyc_Absyn_Comprehension_e){ _LL1210: _temp1209=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1023)->f1; goto _LL1208; _LL1208: _temp1207=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1023)->f2; goto _LL1206; _LL1206: _temp1205=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1023)->f3; goto _LL1080;} else{ goto _LL1081;} _LL1081: if(*(( int*)
_temp1023) == Cyc_Absyn_Struct_e){ _LL1218: _temp1217=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1023)->f1; goto _LL1216; _LL1216: _temp1215=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1023)->f2; goto _LL1214; _LL1214: _temp1213=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1023)->f3; goto _LL1212; _LL1212: _temp1211=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1023)->f4; goto _LL1082;} else{ goto _LL1083;} _LL1083: if(*(( int*)
_temp1023) == Cyc_Absyn_AnonStruct_e){ _LL1222: _temp1221=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1023)->f1; goto _LL1220; _LL1220: _temp1219=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1023)->f2; goto _LL1084;} else{ goto _LL1085;} _LL1085: if(*(( int*)
_temp1023) == Cyc_Absyn_Tunion_e){ _LL1232: _temp1231=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1023)->f1; goto _LL1230; _LL1230: _temp1229=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1023)->f2; goto _LL1228; _LL1228: _temp1227=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1023)->f3; if( _temp1227 == 0){ goto _LL1226;} else{ goto _LL1087;} _LL1226:
_temp1225=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1023)->f4; goto _LL1224;
_LL1224: _temp1223=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1023)->f5; goto
_LL1086;} else{ goto _LL1087;} _LL1087: if(*(( int*) _temp1023) == Cyc_Absyn_Tunion_e){
_LL1242: _temp1241=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1023)->f1; goto
_LL1240; _LL1240: _temp1239=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1023)->f2;
goto _LL1238; _LL1238: _temp1237=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1023)->f3;
goto _LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1023)->f4;
goto _LL1234; _LL1234: _temp1233=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1023)->f5;
goto _LL1088;} else{ goto _LL1089;} _LL1089: if(*(( int*) _temp1023) == Cyc_Absyn_Enum_e){
_LL1248: _temp1247=(( struct Cyc_Absyn_Enum_e_struct*) _temp1023)->f1; goto
_LL1246; _LL1246: _temp1245=(( struct Cyc_Absyn_Enum_e_struct*) _temp1023)->f2;
goto _LL1244; _LL1244: _temp1243=(( struct Cyc_Absyn_Enum_e_struct*) _temp1023)->f3;
goto _LL1090;} else{ goto _LL1091;} _LL1091: if(*(( int*) _temp1023) == Cyc_Absyn_Malloc_e){
_LL1252: _temp1251=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1023)->f1; goto
_LL1250; _LL1250: _temp1249=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1023)->f2; goto _LL1092;} else{ goto _LL1093;} _LL1093: if(*(( int*)
_temp1023) == Cyc_Absyn_StmtExp_e){ _LL1254: _temp1253=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1023)->f1; goto _LL1094;} else{ goto _LL1095;} _LL1095: if(*(( int*)
_temp1023) == Cyc_Absyn_UnresolvedMem_e){ _LL1258: _temp1257=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1023)->f1; goto _LL1256; _LL1256: _temp1255=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1023)->f2; goto _LL1096;} else{ goto _LL1097;} _LL1097: if(*(( int*)
_temp1023) == Cyc_Absyn_CompoundLit_e){ _LL1262: _temp1261=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1023)->f1; goto _LL1260; _LL1260: _temp1259=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1023)->f2; goto _LL1098;} else{ goto _LL1099;} _LL1099: if(*(( int*)
_temp1023) == Cyc_Absyn_Codegen_e){ _LL1264: _temp1263=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1023)->f1; goto _LL1100;} else{ goto _LL1101;} _LL1101: if(*(( int*)
_temp1023) == Cyc_Absyn_Fill_e){ _LL1266: _temp1265=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1023)->f1; goto _LL1102;} else{ goto _LL1024;} _LL1026: { struct Cyc_Absyn_Exp*
_temp1267= Cyc_Absyn_uint_exp( 0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ if( nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr(
old_typ, _temp1267, _temp1267))->r));} else{( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1268)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1271= _temp1267; struct Cyc_Absyn_Exp* _temp1272=
_temp1267; struct Cyc_Absyn_Exp* _temp1273= _temp1267; struct Cyc_Absyn_Exp*
_temp1269[ 3u]={ _temp1271, _temp1272, _temp1273}; struct _tagged_arr _temp1270={(
void*) _temp1269,( void*) _temp1269,( void*)( _temp1269 + 3u)}; _temp1268(
_temp1270);}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL1024;} _LL1028: goto _LL1024; _LL1030:{ struct
_handler_cons _temp1274; _push_handler(& _temp1274);{ int _temp1276= 0; if(
setjmp( _temp1274.handler)){ _temp1276= 1;} if( ! _temp1276){( void*)( e->r=(
void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key)) Cyc_Dict_lookup)( nv->varmap, _temp1109))->r));; _pop_handler();}
else{ void* _temp1275=( void*) _exn_thrown; void* _temp1278= _temp1275; _LL1280:
if( _temp1278 == Cyc_Dict_Absent){ goto _LL1281;} else{ goto _LL1282;} _LL1282:
goto _LL1283; _LL1281:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1284= Cyc_Absynpp_qvar2string(
_temp1109); xprintf("Can't find %.*s in exp_to_c, Var\n", _get_arr_size(
_temp1284, 1u), _temp1284.curr);})); return; _LL1283:( void) _throw( _temp1278);
_LL1279:;}}} goto _LL1024; _LL1032:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("unknownid", sizeof( unsigned char), 10u)); goto _LL1024; _LL1034: {
struct Cyc_List_List* _temp1285=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1113); struct Cyc_List_List* _temp1286=(( struct Cyc_List_List*(*)( void*(*
f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1113);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1113);{ void* _temp1287= _temp1115; _LL1289: if( _temp1287 ==( void*)
Cyc_Absyn_Size){ goto _LL1290;} else{ goto _LL1291;} _LL1291: if( _temp1287 ==(
void*) Cyc_Absyn_Printf){ goto _LL1292;} else{ goto _LL1293;} _LL1293: if(
_temp1287 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1294;} else{ goto _LL1295;}
_LL1295: if( _temp1287 ==( void*) Cyc_Absyn_Fprintf){ goto _LL1296;} else{ goto
_LL1297;} _LL1297: if( _temp1287 ==( void*) Cyc_Absyn_Sscanf){ goto _LL1298;}
else{ goto _LL1299;} _LL1299: if( _temp1287 ==( void*) Cyc_Absyn_Plus){ goto
_LL1300;} else{ goto _LL1301;} _LL1301: if( _temp1287 ==( void*) Cyc_Absyn_Minus){
goto _LL1302;} else{ goto _LL1303;} _LL1303: if( _temp1287 ==( void*) Cyc_Absyn_Eq){
goto _LL1304;} else{ goto _LL1305;} _LL1305: if( _temp1287 ==( void*) Cyc_Absyn_Neq){
goto _LL1306;} else{ goto _LL1307;} _LL1307: if( _temp1287 ==( void*) Cyc_Absyn_Gt){
goto _LL1308;} else{ goto _LL1309;} _LL1309: if( _temp1287 ==( void*) Cyc_Absyn_Gte){
goto _LL1310;} else{ goto _LL1311;} _LL1311: if( _temp1287 ==( void*) Cyc_Absyn_Lt){
goto _LL1312;} else{ goto _LL1313;} _LL1313: if( _temp1287 ==( void*) Cyc_Absyn_Lte){
goto _LL1314;} else{ goto _LL1315;} _LL1315: goto _LL1316; _LL1290: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1113))->hd;{
void* _temp1317=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1325; struct Cyc_Absyn_Tqual _temp1327; void*
_temp1329; struct Cyc_Absyn_PtrInfo _temp1331; struct Cyc_Absyn_Conref*
_temp1333; struct Cyc_Absyn_Tqual _temp1335; struct Cyc_Absyn_Conref* _temp1337;
void* _temp1339; void* _temp1341; _LL1319: if(( unsigned int) _temp1317 > 4u?*((
int*) _temp1317) == Cyc_Absyn_ArrayType: 0){ _LL1330: _temp1329=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1317)->f1; goto _LL1328; _LL1328:
_temp1327=(( struct Cyc_Absyn_ArrayType_struct*) _temp1317)->f2; goto _LL1326;
_LL1326: _temp1325=(( struct Cyc_Absyn_ArrayType_struct*) _temp1317)->f3; goto
_LL1320;} else{ goto _LL1321;} _LL1321: if(( unsigned int) _temp1317 > 4u?*((
int*) _temp1317) == Cyc_Absyn_PointerType: 0){ _LL1332: _temp1331=(( struct Cyc_Absyn_PointerType_struct*)
_temp1317)->f1; _LL1342: _temp1341=( void*) _temp1331.elt_typ; goto _LL1340;
_LL1340: _temp1339=( void*) _temp1331.rgn_typ; goto _LL1338; _LL1338: _temp1337=
_temp1331.nullable; goto _LL1336; _LL1336: _temp1335= _temp1331.tq; goto _LL1334;
_LL1334: _temp1333= _temp1331.bounds; goto _LL1322;} else{ goto _LL1323;}
_LL1323: goto _LL1324; _LL1320:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1325))->r)); goto _LL1318; _LL1322:{ void* _temp1343=(( void*(*)(
void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1333); struct Cyc_Absyn_Exp* _temp1349; _LL1345: if( _temp1343 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1346;} else{ goto _LL1347;} _LL1347: if((
unsigned int) _temp1343 > 1u?*(( int*) _temp1343) == Cyc_Absyn_Upper_b: 0){
_LL1350: _temp1349=(( struct Cyc_Absyn_Upper_b_struct*) _temp1343)->f1; goto
_LL1348;} else{ goto _LL1344;} _LL1346:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp1351)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1354=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1113))->hd; struct Cyc_Absyn_Exp* _temp1355= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( _temp1341), 0); struct Cyc_Absyn_Exp* _temp1352[ 2u]={
_temp1354, _temp1355}; struct _tagged_arr _temp1353={( void*) _temp1352,( void*)
_temp1352,( void*)( _temp1352 + 2u)}; _temp1351( _temp1353);}), 0))->r)); goto
_LL1344; _LL1348:( void*)( e->r=( void*)(( void*) _temp1349->r)); goto _LL1344;
_LL1344:;} goto _LL1318; _LL1324:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1356= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_List_List*) _check_null( _temp1286))->hd); struct
_tagged_arr _temp1357= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_get_arr_size( _temp1356, 1u), _temp1356.curr, _get_arr_size( _temp1357, 1u),
_temp1357.curr);})); return; _LL1318:;} goto _LL1288;} _LL1292:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1115, 0, _temp1113, _temp1286))->r));
goto _LL1288; _LL1294:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1115, 0, _temp1113, _temp1286))->r)); goto _LL1288; _LL1296: if( _temp1113
== 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("fprintf without arguments",
sizeof( unsigned char), 26u));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1115,({ struct Cyc_Core_Opt* _temp1358=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1358->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1113))->hd); _temp1358;}),(( struct Cyc_List_List*)
_check_null( _temp1113))->tl,(( struct Cyc_List_List*) _check_null( _temp1286))->tl))->r));
goto _LL1288; _LL1298: if( _temp1113 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("sscanf without arguments", sizeof( unsigned char), 25u));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1113))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1113))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1113))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1288; _LL1300: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1285))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1113))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1113))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1359)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1362= e1; struct Cyc_Absyn_Exp* _temp1363= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1364= e2; struct Cyc_Absyn_Exp*
_temp1360[ 3u]={ _temp1362, _temp1363, _temp1364}; struct _tagged_arr _temp1361={(
void*) _temp1360,( void*) _temp1360,( void*)( _temp1360 + 3u)}; _temp1359(
_temp1361);}), 0))->r));} goto _LL1288;} _LL1302: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1285))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1113))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1113))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1285))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1365)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1368= e1; struct Cyc_Absyn_Exp* _temp1369= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1370= Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Minus, e2, 0); struct Cyc_Absyn_Exp* _temp1366[ 3u]={ _temp1368,
_temp1369, _temp1370}; struct _tagged_arr _temp1367={( void*) _temp1366,( void*)
_temp1366,( void*)( _temp1366 + 3u)}; _temp1365( _temp1367);}), 0))->r));}} goto
_LL1288;} _LL1304: goto _LL1306; _LL1306: goto _LL1308; _LL1308: goto _LL1310;
_LL1310: goto _LL1312; _LL1312: goto _LL1314; _LL1314: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1113))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1113))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1285))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1285))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1288;} _LL1316: goto
_LL1288; _LL1288:;} goto _LL1024;} _LL1036: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1119->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1119);{ void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1371=( _temp1117 ==( void*) Cyc_Absyn_PostInc?
1: _temp1117 ==( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1372=( _temp1117 ==( void*) Cyc_Absyn_PreInc?
1: _temp1117 ==( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1371,({ struct Cyc_List_List*(* _temp1373)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1376= Cyc_Absyn_address_exp( _temp1119, 0); struct
Cyc_Absyn_Exp* _temp1377= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
struct Cyc_Absyn_Exp* _temp1378= Cyc_Absyn_signed_int_exp( _temp1372, 0); struct
Cyc_Absyn_Exp* _temp1374[ 3u]={ _temp1376, _temp1377, _temp1378}; struct
_tagged_arr _temp1375={( void*) _temp1374,( void*) _temp1374,( void*)( _temp1374
+ 3u)}; _temp1373( _temp1375);}), 0))->r));} if( elt_typ ==( void*) Cyc_Absyn_VoidType?
! Cyc_Absyn_is_lvalue( _temp1119): 0){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1119, 0, Cyc_Toc_incr_lvalue,
_temp1117);( void*)( e->r=( void*)(( void*) _temp1119->r));} goto _LL1024;}}
_LL1038: { int e1_poly= Cyc_Toc_is_poly_project( _temp1125); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1125->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1121->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1125); Cyc_Toc_exp_to_c( nv, _temp1121);{ int done= 0;
if( _temp1123 != 0){ void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* change;{ void* _temp1379=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1123))->v; _LL1381: if( _temp1379 ==(
void*) Cyc_Absyn_Plus){ goto _LL1382;} else{ goto _LL1383;} _LL1383: if(
_temp1379 ==( void*) Cyc_Absyn_Minus){ goto _LL1384;} else{ goto _LL1385;}
_LL1385: goto _LL1386; _LL1382: change= _temp1121; goto _LL1380; _LL1384: change=
Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp1121, 0); goto _LL1380;
_LL1386:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("bad t ? pointer arithmetic", sizeof( unsigned char), 27u))); goto
_LL1380; _LL1380:;} done= 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_inplace_plus_e,({ struct Cyc_List_List*(* _temp1387)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1390= Cyc_Absyn_address_exp( _temp1125, 0); struct
Cyc_Absyn_Exp* _temp1391= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
struct Cyc_Absyn_Exp* _temp1392= change; struct Cyc_Absyn_Exp* _temp1388[ 3u]={
_temp1390, _temp1391, _temp1392}; struct _tagged_arr _temp1389={( void*)
_temp1388,( void*) _temp1388,( void*)( _temp1388 + 3u)}; _temp1387( _temp1389);}),
0))->r));}} if( ! done){ if( e1_poly){( void*)( _temp1121->r=( void*)(( void*)(
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1121->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp1125)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, struct _tuple9*), struct _tuple9* f_env)) Cyc_Toc_lvalue_assign)(
_temp1125, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple9* _temp1393=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9) * 1); _temp1393[ 0]=({ struct
_tuple9 _temp1394; _temp1394.f1= _temp1123; _temp1394.f2= _temp1121; _temp1394;});
_temp1393;}));( void*)( e->r=( void*)(( void*) _temp1125->r));}} goto _LL1024;}}
_LL1040: Cyc_Toc_exp_to_c( nv, _temp1131); Cyc_Toc_exp_to_c( nv, _temp1129); Cyc_Toc_exp_to_c(
nv, _temp1127); goto _LL1024; _LL1042: Cyc_Toc_exp_to_c( nv, _temp1135); Cyc_Toc_exp_to_c(
nv, _temp1133); goto _LL1024; _LL1044: _temp1145= _temp1139; _temp1143=
_temp1137; goto _LL1046; _LL1046: Cyc_Toc_exp_to_c( nv, _temp1145);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1143);
goto _LL1024; _LL1048:{ struct _RegionHandle _temp1395= _new_region(); struct
_RegionHandle* r=& _temp1395; _push_region( r);{ void* _temp1396=( void*)
_temp1150->type; void* _temp1397= Cyc_Toc_typ_to_c( _temp1396); void* _temp1398=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1158->topt))->v);
struct Cyc_Absyn_Exp* _temp1399= Cyc_Absyn_uint_exp(( unsigned int) _temp1154, 0);
void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1448=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1448[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1449; _temp1449.tag= Cyc_Absyn_ArrayType;
_temp1449.f1=( void*) _temp1396; _temp1449.f2= Cyc_Toc_mt_tq; _temp1449.f3=(
struct Cyc_Absyn_Exp*) _temp1399; _temp1449;}); _temp1448;})); struct _tuple0*
_temp1400= Cyc_Toc_temp_var(); struct _tuple0* _temp1401= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1402= Cyc_Absyn_var_exp( _temp1401, 0); struct
_tuple0* _temp1403= Cyc_Toc_temp_var(); int _temp1404=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1156); int _temp1405= _temp1404 - _temp1154; struct
_tagged_arr vs=({ unsigned int _temp1444=( unsigned int) _temp1154; struct
_tuple0** _temp1445=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1444); struct _tagged_arr _temp1447= _tag_arr( _temp1445,
sizeof( struct _tuple0*),( unsigned int) _temp1154);{ unsigned int _temp1446=
_temp1444; unsigned int i; for( i= 0; i < _temp1446; i ++){ _temp1445[ i]= Cyc_Toc_temp_var();}};
_temp1447;}); struct _tagged_arr xs=({ unsigned int _temp1440=( unsigned int)
_temp1405; struct _tuple0** _temp1441=( struct _tuple0**) _region_malloc( r,
sizeof( struct _tuple0*) * _temp1440); struct _tagged_arr _temp1443= _tag_arr(
_temp1441, sizeof( struct _tuple0*),( unsigned int) _temp1405);{ unsigned int
_temp1442= _temp1440; unsigned int i; for( i= 0; i < _temp1442; i ++){ _temp1441[
i]= Cyc_Toc_temp_var();}}; _temp1443;}); struct Cyc_List_List* xexps=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,(
struct Cyc_Position_Segment*) 0,(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, xs));
xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List* _temp1406=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1406->hd=( void*) Cyc_Absyn_var_exp(
_temp1403, 0); _temp1406->tl= 0; _temp1406;}));{ struct Cyc_Absyn_Stmt*
_temp1407= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
_temp1400, 0), xexps, 0), 0); struct Cyc_Absyn_Exp* _temp1408= Cyc_Absyn_add_exp(
_temp1402, _temp1399, 0); struct Cyc_List_List* _temp1409=({ struct Cyc_List_List*(*
_temp1431)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct _tuple4* _temp1434=({ struct _tuple4* _temp1439=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1439->f1= 0; _temp1439->f2=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1402, 0); _temp1439;});
struct _tuple4* _temp1435=({ struct _tuple4* _temp1438=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1438->f1= 0; _temp1438->f2= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1402, 0); _temp1438;}); struct _tuple4*
_temp1436=({ struct _tuple4* _temp1437=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1437->f1= 0; _temp1437->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1408, 0); _temp1437;}); struct _tuple4* _temp1432[ 3u]={ _temp1434,
_temp1435, _temp1436}; struct _tagged_arr _temp1433={( void*) _temp1432,( void*)
_temp1432,( void*)( _temp1432 + 3u)}; _temp1431( _temp1433);}); _temp1407= Cyc_Absyn_declare_stmt(
_temp1403, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp1396,( void*) _temp1150->rgn,
Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1409, 0), _temp1407, 0);{ struct Cyc_List_List* _temp1410=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, vs));
struct Cyc_List_List* _temp1411=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_make_dle,
_temp1410); _temp1407= Cyc_Absyn_declare_stmt( _temp1401, arr_type,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp( 0, _temp1411, 0), _temp1407, 0);{ struct Cyc_List_List*
_temp1412=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, _temp1156); struct Cyc_List_List* _temp1413=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1152);{
int i= _temp1154 - 1; for( 0; i >= 0;( i --, _temp1412=(( struct Cyc_List_List*)
_check_null( _temp1412))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1412))->hd; void* arg_type=( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c( nv, arg);
if( _temp1150->inject){ void* _temp1416; struct Cyc_Position_Segment* _temp1418;
struct Cyc_List_List* _temp1420; struct Cyc_List_List* _temp1422; struct _tuple0*
_temp1424; struct Cyc_Absyn_Tunionfield _temp1414=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1413))->hd); _LL1425: _temp1424=
_temp1414.name; goto _LL1423; _LL1423: _temp1422= _temp1414.tvs; goto _LL1421;
_LL1421: _temp1420= _temp1414.typs; goto _LL1419; _LL1419: _temp1418= _temp1414.loc;
goto _LL1417; _LL1417: _temp1416=( void*) _temp1414.sc; goto _LL1415; _LL1415:
_temp1413=(( struct Cyc_List_List*) _check_null( _temp1413))->tl;{ struct
_tuple0* _temp1426= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1427= Cyc_Absyn_var_exp(
_temp1426, 0); void* _temp1428= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp1424, _tag_arr("_struct", sizeof( unsigned char), 8u))); _temp1407= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1397,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1397, Cyc_Absyn_address_exp(
_temp1427, 0), 0), _temp1407, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple3*)(( struct Cyc_List_List*) _check_null( _temp1420))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1429= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1427, Cyc_Toc_fieldname(
1), 0), arg, 0); _temp1407= Cyc_Absyn_seq_stmt( _temp1429, _temp1407, 0);{
struct Cyc_Absyn_Stmt* _temp1430= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1427, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1424, 0), 0); _temp1407=
Cyc_Absyn_seq_stmt( _temp1430, _temp1407, 0); _temp1407= Cyc_Absyn_declare_stmt(
_temp1426, _temp1428, 0, _temp1407, 0);}}}}} else{ _temp1407= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1397,( struct Cyc_Absyn_Exp*) arg, _temp1407, 0);}}}{ int i= _temp1405 - 1;
for( 0; i >= 0;( i --, _temp1412=(( struct Cyc_List_List*) _check_null(
_temp1412))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1412))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1407= Cyc_Absyn_declare_stmt(*(( struct _tuple0**)
_check_unknown_subscript( xs, sizeof( struct _tuple0*), i)), argtype,( struct
Cyc_Absyn_Exp*) arg, _temp1407, 0);}} Cyc_Toc_exp_to_c( nv, _temp1158);
_temp1407= Cyc_Absyn_declare_stmt( _temp1400, _temp1398,( struct Cyc_Absyn_Exp*)
_temp1158, _temp1407, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1407, 0))->r));}}}}; _pop_region( r);} goto _LL1024; _LL1050: Cyc_Toc_exp_to_c(
nv, _temp1160);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1160), 0))->r)); goto _LL1024; _LL1052: Cyc_Toc_exp_to_c(
nv, _temp1162); goto _LL1024; _LL1054: Cyc_Toc_exp_to_c( nv, _temp1166);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
for( 0; _temp1164 != 0; _temp1164=(( struct Cyc_List_List*) _check_null(
_temp1164))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*)
_check_null( _temp1164))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*)
Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1166, 0))->r)); break;}} goto _LL1024;} _LL1056: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1168->topt))->v; void* new_typ=*
_temp1172;* _temp1172= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1168);{ struct _tuple10 _temp1451=({ struct _tuple10 _temp1450; _temp1450.f1=
Cyc_Tcutil_compress( old_t2); _temp1450.f2= Cyc_Tcutil_compress( new_typ);
_temp1450;}); void* _temp1459; struct Cyc_Absyn_PtrInfo _temp1461; void*
_temp1463; struct Cyc_Absyn_PtrInfo _temp1465; void* _temp1467; void* _temp1469;
void* _temp1471; void* _temp1473; struct Cyc_Absyn_PtrInfo _temp1475; _LL1453:
_LL1464: _temp1463= _temp1451.f1; if(( unsigned int) _temp1463 > 4u?*(( int*)
_temp1463) == Cyc_Absyn_PointerType: 0){ _LL1466: _temp1465=(( struct Cyc_Absyn_PointerType_struct*)
_temp1463)->f1; goto _LL1460;} else{ goto _LL1455;} _LL1460: _temp1459=
_temp1451.f2; if(( unsigned int) _temp1459 > 4u?*(( int*) _temp1459) == Cyc_Absyn_PointerType:
0){ _LL1462: _temp1461=(( struct Cyc_Absyn_PointerType_struct*) _temp1459)->f1;
goto _LL1454;} else{ goto _LL1455;} _LL1455: _LL1474: _temp1473= _temp1451.f1;
if(( unsigned int) _temp1473 > 4u?*(( int*) _temp1473) == Cyc_Absyn_PointerType:
0){ _LL1476: _temp1475=(( struct Cyc_Absyn_PointerType_struct*) _temp1473)->f1;
goto _LL1468;} else{ goto _LL1457;} _LL1468: _temp1467= _temp1451.f2; if((
unsigned int) _temp1467 > 4u?*(( int*) _temp1467) == Cyc_Absyn_IntType: 0){
_LL1472: _temp1471=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1467)->f1;
goto _LL1470; _LL1470: _temp1469=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1467)->f2; goto _LL1456;} else{ goto _LL1457;} _LL1457: goto _LL1458;
_LL1454: { int _temp1477=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1465.nullable); int _temp1478=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1461.nullable); void* _temp1479=(( void*(*)( void*,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1465.bounds); void* _temp1480=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1461.bounds);{ struct
_tuple10 _temp1482=({ struct _tuple10 _temp1481; _temp1481.f1= _temp1479;
_temp1481.f2= _temp1480; _temp1481;}); void* _temp1492; struct Cyc_Absyn_Exp*
_temp1494; void* _temp1496; struct Cyc_Absyn_Exp* _temp1498; void* _temp1500;
void* _temp1502; struct Cyc_Absyn_Exp* _temp1504; void* _temp1506; struct Cyc_Absyn_Exp*
_temp1508; void* _temp1510; _LL1484: _LL1497: _temp1496= _temp1482.f1; if((
unsigned int) _temp1496 > 1u?*(( int*) _temp1496) == Cyc_Absyn_Upper_b: 0){
_LL1499: _temp1498=(( struct Cyc_Absyn_Upper_b_struct*) _temp1496)->f1; goto
_LL1493;} else{ goto _LL1486;} _LL1493: _temp1492= _temp1482.f2; if((
unsigned int) _temp1492 > 1u?*(( int*) _temp1492) == Cyc_Absyn_Upper_b: 0){
_LL1495: _temp1494=(( struct Cyc_Absyn_Upper_b_struct*) _temp1492)->f1; goto
_LL1485;} else{ goto _LL1486;} _LL1486: _LL1503: _temp1502= _temp1482.f1; if((
unsigned int) _temp1502 > 1u?*(( int*) _temp1502) == Cyc_Absyn_Upper_b: 0){
_LL1505: _temp1504=(( struct Cyc_Absyn_Upper_b_struct*) _temp1502)->f1; goto
_LL1501;} else{ goto _LL1488;} _LL1501: _temp1500= _temp1482.f2; if( _temp1500
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1487;} else{ goto _LL1488;} _LL1488:
_LL1511: _temp1510= _temp1482.f1; if( _temp1510 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1507;} else{ goto _LL1490;} _LL1507: _temp1506= _temp1482.f2; if((
unsigned int) _temp1506 > 1u?*(( int*) _temp1506) == Cyc_Absyn_Upper_b: 0){
_LL1509: _temp1508=(( struct Cyc_Absyn_Upper_b_struct*) _temp1506)->f1; goto
_LL1489;} else{ goto _LL1490;} _LL1490: goto _LL1491; _LL1485: if( _temp1477? !
_temp1478: 0){ if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't do null-check conversion at top-level", sizeof( unsigned char),
44u));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp1172, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1512=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1512->hd=( void*) _temp1168;
_temp1512->tl= 0; _temp1512;}), 0), 0))->r));} goto _LL1483; _LL1487: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1504, _temp1168))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1513)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1516= _temp1168; struct Cyc_Absyn_Exp* _temp1517= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1461.elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1518= _temp1504; struct Cyc_Absyn_Exp* _temp1514[ 3u]={ _temp1516,
_temp1517, _temp1518}; struct _tagged_arr _temp1515={( void*) _temp1514,( void*)
_temp1514,( void*)( _temp1514 + 3u)}; _temp1513( _temp1515);}), 0))->r));} goto
_LL1483; _LL1489: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u));}{ struct Cyc_Absyn_Exp* _temp1519= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({
struct Cyc_List_List*(* _temp1521)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1524= _temp1168;
struct Cyc_Absyn_Exp* _temp1525= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c((
void*) _temp1465.elt_typ), 0); struct Cyc_Absyn_Exp* _temp1526= _temp1508;
struct Cyc_Absyn_Exp* _temp1522[ 3u]={ _temp1524, _temp1525, _temp1526}; struct
_tagged_arr _temp1523={( void*) _temp1522,( void*) _temp1522,( void*)( _temp1522
+ 3u)}; _temp1521( _temp1523);}), 0); if( _temp1478){( void*)( _temp1519->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({ struct Cyc_List_List*
_temp1520=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1520->hd=( void*) Cyc_Absyn_copy_exp( _temp1519); _temp1520->tl= 0;
_temp1520;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp1172, _temp1519, 0))->r)); goto _LL1483;} _LL1491: goto _LL1483; _LL1483:;}
goto _LL1452;} _LL1456:{ void* _temp1527=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1475.bounds); _LL1529: if(
_temp1527 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1530;} else{ goto _LL1531;}
_LL1531: goto _LL1532; _LL1530:( void*)( _temp1168->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1168->r, _temp1168->loc), Cyc_Toc_curr_sp, 0))->r));
goto _LL1528; _LL1532: goto _LL1528; _LL1528:;} goto _LL1452; _LL1458: goto
_LL1452; _LL1452:;} goto _LL1024;} _LL1058:{ void* _temp1533=( void*) _temp1173->r;
struct Cyc_Absyn_Structdecl* _temp1541; struct Cyc_List_List* _temp1543; struct
Cyc_Core_Opt* _temp1545; struct _tuple0* _temp1547; struct Cyc_List_List*
_temp1549; _LL1535: if(*(( int*) _temp1533) == Cyc_Absyn_Struct_e){ _LL1548:
_temp1547=(( struct Cyc_Absyn_Struct_e_struct*) _temp1533)->f1; goto _LL1546;
_LL1546: _temp1545=(( struct Cyc_Absyn_Struct_e_struct*) _temp1533)->f2; goto
_LL1544; _LL1544: _temp1543=(( struct Cyc_Absyn_Struct_e_struct*) _temp1533)->f3;
goto _LL1542; _LL1542: _temp1541=(( struct Cyc_Absyn_Struct_e_struct*) _temp1533)->f4;
goto _LL1536;} else{ goto _LL1537;} _LL1537: if(*(( int*) _temp1533) == Cyc_Absyn_Tuple_e){
_LL1550: _temp1549=(( struct Cyc_Absyn_Tuple_e_struct*) _temp1533)->f1; goto
_LL1538;} else{ goto _LL1539;} _LL1539: goto _LL1540; _LL1536: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr _temp1551=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1173->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_get_arr_size( _temp1551, 1u), _temp1551.curr);}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1173->topt))->v, 1, 0, _temp1543, _temp1547))->r)); goto _LL1534; _LL1538:
if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct
_tagged_arr)({ struct _tagged_arr _temp1552=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1173->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_get_arr_size( _temp1552, 1u), _temp1552.curr);}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1549))->r)); goto _LL1534; _LL1540:
Cyc_Toc_exp_to_c( nv, _temp1173); if( ! Cyc_Absyn_is_lvalue( _temp1173)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1173, 0,
Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1173, 0))->r));} goto _LL1534; _LL1534:;} goto _LL1024; _LL1060: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr _temp1553=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1175->loc); xprintf("%.*s: new at top-level", _get_arr_size( _temp1553, 1u),
_temp1553.curr);}));}{ void* _temp1554=( void*) _temp1175->r; struct Cyc_List_List*
_temp1566; struct Cyc_Absyn_Exp* _temp1568; struct Cyc_Absyn_Exp* _temp1570;
struct Cyc_Absyn_Vardecl* _temp1572; struct Cyc_Absyn_Structdecl* _temp1574;
struct Cyc_List_List* _temp1576; struct Cyc_Core_Opt* _temp1578; struct _tuple0*
_temp1580; struct Cyc_List_List* _temp1582; _LL1556: if(*(( int*) _temp1554) ==
Cyc_Absyn_Array_e){ _LL1567: _temp1566=(( struct Cyc_Absyn_Array_e_struct*)
_temp1554)->f1; goto _LL1557;} else{ goto _LL1558;} _LL1558: if(*(( int*)
_temp1554) == Cyc_Absyn_Comprehension_e){ _LL1573: _temp1572=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1554)->f1; goto _LL1571; _LL1571: _temp1570=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1554)->f2; goto _LL1569; _LL1569: _temp1568=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1554)->f3; goto _LL1559;} else{ goto _LL1560;} _LL1560: if(*(( int*)
_temp1554) == Cyc_Absyn_Struct_e){ _LL1581: _temp1580=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1554)->f1; goto _LL1579; _LL1579: _temp1578=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1554)->f2; goto _LL1577; _LL1577: _temp1576=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1554)->f3; goto _LL1575; _LL1575: _temp1574=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1554)->f4; goto _LL1561;} else{ goto _LL1562;} _LL1562: if(*(( int*)
_temp1554) == Cyc_Absyn_Tuple_e){ _LL1583: _temp1582=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1554)->f1; goto _LL1563;} else{ goto _LL1564;} _LL1564: goto _LL1565;
_LL1557: { struct _tuple0* _temp1584= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1585= Cyc_Absyn_var_exp( _temp1584, 0); struct Cyc_Absyn_Stmt* _temp1586=
Cyc_Toc_init_array( nv, _temp1585, _temp1566, Cyc_Absyn_exp_stmt( _temp1585, 0));
void* old_elt_typ;{ void* _temp1587= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1593; struct Cyc_Absyn_Conref* _temp1595; struct Cyc_Absyn_Tqual _temp1597;
struct Cyc_Absyn_Conref* _temp1599; void* _temp1601; void* _temp1603; _LL1589:
if(( unsigned int) _temp1587 > 4u?*(( int*) _temp1587) == Cyc_Absyn_PointerType:
0){ _LL1594: _temp1593=(( struct Cyc_Absyn_PointerType_struct*) _temp1587)->f1;
_LL1604: _temp1603=( void*) _temp1593.elt_typ; goto _LL1602; _LL1602: _temp1601=(
void*) _temp1593.rgn_typ; goto _LL1600; _LL1600: _temp1599= _temp1593.nullable;
goto _LL1598; _LL1598: _temp1597= _temp1593.tq; goto _LL1596; _LL1596: _temp1595=
_temp1593.bounds; goto _LL1590;} else{ goto _LL1591;} _LL1591: goto _LL1592;
_LL1590: old_elt_typ= _temp1603; goto _LL1588; _LL1592: old_elt_typ=(( void*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u)); goto _LL1588; _LL1588:;}{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* _temp1605= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* _temp1606= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1566), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1177 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1606);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1177); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1606);}{ struct Cyc_Absyn_Exp* _temp1607= Cyc_Absyn_cast_exp( _temp1605, e1,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1584, _temp1605,( struct Cyc_Absyn_Exp*) _temp1607, _temp1586, 0), 0))->r));
goto _LL1555;}}} _LL1559: { int is_tagged_ptr= 0;{ void* _temp1608= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1614; struct Cyc_Absyn_Conref* _temp1616;
struct Cyc_Absyn_Tqual _temp1618; struct Cyc_Absyn_Conref* _temp1620; void*
_temp1622; void* _temp1624; _LL1610: if(( unsigned int) _temp1608 > 4u?*(( int*)
_temp1608) == Cyc_Absyn_PointerType: 0){ _LL1615: _temp1614=(( struct Cyc_Absyn_PointerType_struct*)
_temp1608)->f1; _LL1625: _temp1624=( void*) _temp1614.elt_typ; goto _LL1623;
_LL1623: _temp1622=( void*) _temp1614.rgn_typ; goto _LL1621; _LL1621: _temp1620=
_temp1614.nullable; goto _LL1619; _LL1619: _temp1618= _temp1614.tq; goto _LL1617;
_LL1617: _temp1616= _temp1614.bounds; goto _LL1611;} else{ goto _LL1612;}
_LL1612: goto _LL1613; _LL1611: is_tagged_ptr=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1616) ==( void*) Cyc_Absyn_Unknown_b;
goto _LL1609; _LL1613:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: comprehension not an array type", sizeof( unsigned char), 42u));
goto _LL1609; _LL1609:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct
_tuple0* a= Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1568->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1570);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1572, Cyc_Absyn_var_exp( max, 0), _temp1568, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1626= _new_region(); struct _RegionHandle* r=&
_temp1626; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1649=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1649->hd=( void*)({ struct _tuple8* _temp1650=( struct _tuple8*)
_region_malloc( r, sizeof( struct _tuple8)); _temp1650->f1= max; _temp1650->f2=
Cyc_Absyn_uint_t; _temp1650->f3=( struct Cyc_Absyn_Exp*) _temp1570; _temp1650;});
_temp1649->tl= 0; _temp1649;}); struct Cyc_Absyn_Exp* ai; if( _temp1177 == 0){
ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( _temp1177); Cyc_Toc_exp_to_c( nv, r); ai=
Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0),
Cyc_Absyn_var_exp( max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp(
ptr_typ, ai, 0); decls=({ struct Cyc_List_List* _temp1627=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1627->hd=( void*)({
struct _tuple8* _temp1628=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1628->f1= a; _temp1628->f2= ptr_typ; _temp1628->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1628;}); _temp1627->tl= decls; _temp1627;}); if( is_tagged_ptr){
struct _tuple0* _temp1629= Cyc_Toc_temp_var(); void* _temp1630= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Exp* _temp1631= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_List_List*(* _temp1634)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1637= Cyc_Absyn_var_exp(
a, 0); struct Cyc_Absyn_Exp* _temp1638= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
struct Cyc_Absyn_Exp* _temp1639= _temp1570; struct Cyc_Absyn_Exp* _temp1635[ 3u]={
_temp1637, _temp1638, _temp1639}; struct _tagged_arr _temp1636={( void*)
_temp1635,( void*) _temp1635,( void*)( _temp1635 + 3u)}; _temp1634( _temp1636);}),
0); decls=({ struct Cyc_List_List* _temp1632=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1632->hd=( void*)({
struct _tuple8* _temp1633=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1633->f1= _temp1629; _temp1633->f2= _temp1630; _temp1633->f3=(
struct Cyc_Absyn_Exp*) _temp1631; _temp1633;}); _temp1632->tl= decls; _temp1632;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1629, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1640= decls; for( 0; _temp1640 != 0;
_temp1640=(( struct Cyc_List_List*) _check_null( _temp1640))->tl){ struct Cyc_Absyn_Exp*
_temp1643; void* _temp1645; struct _tuple0* _temp1647; struct _tuple8 _temp1641=*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1640))->hd); _LL1648:
_temp1647= _temp1641.f1; goto _LL1646; _LL1646: _temp1645= _temp1641.f2; goto
_LL1644; _LL1644: _temp1643= _temp1641.f3; goto _LL1642; _LL1642: s= Cyc_Absyn_declare_stmt(
_temp1647, _temp1645, _temp1643, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL1555;}}} _LL1561:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1175->topt))->v, 1, _temp1177, _temp1576, _temp1580))->r)); goto _LL1555;
_LL1563:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1177,
_temp1582))->r)); goto _LL1555; _LL1565: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1175->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp1177 == 0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1177); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1651=( void*)
_temp1175->r; struct Cyc_Absyn_Exp* _temp1657; void* _temp1659; _LL1653: if(*((
int*) _temp1651) == Cyc_Absyn_Cast_e){ _LL1660: _temp1659=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1651)->f1; goto _LL1658; _LL1658: _temp1657=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1651)->f2; goto _LL1654;} else{ goto _LL1655;} _LL1655: goto _LL1656;
_LL1654:{ struct _tuple10 _temp1662=({ struct _tuple10 _temp1661; _temp1661.f1=
Cyc_Tcutil_compress( _temp1659); _temp1661.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1657->topt))->v); _temp1661;}); void*
_temp1668; struct Cyc_Absyn_PtrInfo _temp1670; struct Cyc_Absyn_Conref*
_temp1672; struct Cyc_Absyn_Tqual _temp1674; struct Cyc_Absyn_Conref* _temp1676;
void* _temp1678; void* _temp1680; void* _temp1682; struct Cyc_Absyn_PtrInfo
_temp1684; struct Cyc_Absyn_Conref* _temp1686; struct Cyc_Absyn_Tqual _temp1688;
struct Cyc_Absyn_Conref* _temp1690; void* _temp1692; void* _temp1694; _LL1664:
_LL1683: _temp1682= _temp1662.f1; if(( unsigned int) _temp1682 > 4u?*(( int*)
_temp1682) == Cyc_Absyn_PointerType: 0){ _LL1685: _temp1684=(( struct Cyc_Absyn_PointerType_struct*)
_temp1682)->f1; _LL1695: _temp1694=( void*) _temp1684.elt_typ; goto _LL1693;
_LL1693: _temp1692=( void*) _temp1684.rgn_typ; goto _LL1691; _LL1691: _temp1690=
_temp1684.nullable; goto _LL1689; _LL1689: _temp1688= _temp1684.tq; goto _LL1687;
_LL1687: _temp1686= _temp1684.bounds; goto _LL1669;} else{ goto _LL1666;}
_LL1669: _temp1668= _temp1662.f2; if(( unsigned int) _temp1668 > 4u?*(( int*)
_temp1668) == Cyc_Absyn_PointerType: 0){ _LL1671: _temp1670=(( struct Cyc_Absyn_PointerType_struct*)
_temp1668)->f1; _LL1681: _temp1680=( void*) _temp1670.elt_typ; goto _LL1679;
_LL1679: _temp1678=( void*) _temp1670.rgn_typ; goto _LL1677; _LL1677: _temp1676=
_temp1670.nullable; goto _LL1675; _LL1675: _temp1674= _temp1670.tq; goto _LL1673;
_LL1673: _temp1672= _temp1670.bounds; goto _LL1665;} else{ goto _LL1666;}
_LL1666: goto _LL1667; _LL1665:{ struct _tuple10 _temp1697=({ struct _tuple10
_temp1696; _temp1696.f1=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1686); _temp1696.f2=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1672); _temp1696;}); void*
_temp1703; struct Cyc_Absyn_Exp* _temp1705; void* _temp1707; _LL1699: _LL1708:
_temp1707= _temp1697.f1; if( _temp1707 ==( void*) Cyc_Absyn_Unknown_b){ goto
_LL1704;} else{ goto _LL1701;} _LL1704: _temp1703= _temp1697.f2; if((
unsigned int) _temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_Upper_b: 0){
_LL1706: _temp1705=(( struct Cyc_Absyn_Upper_b_struct*) _temp1703)->f1; goto
_LL1700;} else{ goto _LL1701;} _LL1701: goto _LL1702; _LL1700: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_List_List*(* _temp1709)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1712= mexp;
struct Cyc_Absyn_Exp* _temp1713= _temp1657; struct Cyc_Absyn_Exp* _temp1714= Cyc_Absyn_sizeoftyp_exp(
_temp1694, 0); struct Cyc_Absyn_Exp* _temp1715= _temp1705; struct Cyc_Absyn_Exp*
_temp1710[ 4u]={ _temp1712, _temp1713, _temp1714, _temp1715}; struct _tagged_arr
_temp1711={( void*) _temp1710,( void*) _temp1710,( void*)( _temp1710 + 4u)};
_temp1709( _temp1711);}), 0))->r)); goto _LL1698; _LL1702: goto _LL1698; _LL1698:;}
goto _LL1663; _LL1667: goto _LL1663; _LL1663:;} goto _LL1652; _LL1656: goto
_LL1652; _LL1652:;} if( ! done){ struct _tuple0* _temp1716= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* _temp1717= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1716, 0), 0); struct Cyc_Absyn_Exp* _temp1718= Cyc_Absyn_signed_int_exp( 0,
0); Cyc_Toc_exp_to_c( nv, _temp1175); _temp1717= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1716, 0), _temp1718, 0),
_temp1175, 0), _temp1717, 0);{ void* _temp1719= Cyc_Absyn_cstar_typ( elt_typ,
Cyc_Toc_mt_tq);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1716, _temp1719,( struct Cyc_Absyn_Exp*) mexp, _temp1717, 0), 0))->r));}}
goto _LL1555;}} _LL1555:;} goto _LL1024; _LL1062: Cyc_Toc_exp_to_c( nv,
_temp1179); goto _LL1024; _LL1064:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1720=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1720[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1721; _temp1721.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1721.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1181);
_temp1721;}); _temp1720;}))); goto _LL1024; _LL1066:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1722=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1722[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1723; _temp1723.tag= Cyc_Absyn_Offsetof_e;
_temp1723.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1185); _temp1723.f2=
_temp1183; _temp1723;}); _temp1722;}))); goto _LL1024; _LL1068: { void*
_temp1724= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1187->topt))->v);{ void* _temp1725= _temp1724; struct Cyc_Absyn_PtrInfo
_temp1731; struct Cyc_Absyn_Conref* _temp1733; struct Cyc_Absyn_Tqual _temp1735;
struct Cyc_Absyn_Conref* _temp1737; void* _temp1739; void* _temp1741; _LL1727:
if(( unsigned int) _temp1725 > 4u?*(( int*) _temp1725) == Cyc_Absyn_PointerType:
0){ _LL1732: _temp1731=(( struct Cyc_Absyn_PointerType_struct*) _temp1725)->f1;
_LL1742: _temp1741=( void*) _temp1731.elt_typ; goto _LL1740; _LL1740: _temp1739=(
void*) _temp1731.rgn_typ; goto _LL1738; _LL1738: _temp1737= _temp1731.nullable;
goto _LL1736; _LL1736: _temp1735= _temp1731.tq; goto _LL1734; _LL1734: _temp1733=
_temp1731.bounds; goto _LL1728;} else{ goto _LL1729;} _LL1729: goto _LL1730;
_LL1728:{ void* _temp1743=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1733); struct Cyc_Absyn_Exp* _temp1749; _LL1745: if((
unsigned int) _temp1743 > 1u?*(( int*) _temp1743) == Cyc_Absyn_Upper_b: 0){
_LL1750: _temp1749=(( struct Cyc_Absyn_Upper_b_struct*) _temp1743)->f1; goto
_LL1746;} else{ goto _LL1747;} _LL1747: if( _temp1743 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1748;} else{ goto _LL1744;} _LL1746: Cyc_Toc_exp_to_c( nv, _temp1187);
if( Cyc_Toc_is_nullable( _temp1724)){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1187->topt))->v), Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1751=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1751->hd=( void*) _temp1187; _temp1751->tl= 0;
_temp1751;}), 0), 0), 0))->r));} goto _LL1744; _LL1748: { struct Cyc_Absyn_Exp*
_temp1752= Cyc_Absyn_uint_exp( 0, 0); _temp1752->topt=({ struct Cyc_Core_Opt*
_temp1753=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1753->v=( void*) Cyc_Absyn_uint_t; _temp1753;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_subscript_exp( _temp1187, _temp1752, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1744;} _LL1744:;} goto _LL1726; _LL1730:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Deref: non-pointer",
sizeof( unsigned char), 29u)); goto _LL1726; _LL1726:;} goto _LL1024;} _LL1070:
Cyc_Toc_exp_to_c( nv, _temp1191); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL1024; _LL1072: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1195->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1195);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1756; struct Cyc_Absyn_Tqual _temp1758; struct Cyc_Absyn_Conref* _temp1760;
void* _temp1762; void* _temp1764; struct Cyc_Absyn_PtrInfo _temp1754= Cyc_Toc_get_ptr_type(
e1typ); _LL1765: _temp1764=( void*) _temp1754.elt_typ; goto _LL1763; _LL1763:
_temp1762=( void*) _temp1754.rgn_typ; goto _LL1761; _LL1761: _temp1760=
_temp1754.nullable; goto _LL1759; _LL1759: _temp1758= _temp1754.tq; goto _LL1757;
_LL1757: _temp1756= _temp1754.bounds; goto _LL1755; _LL1755:{ void* _temp1766=((
void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1756); struct Cyc_Absyn_Exp* _temp1772; _LL1768: if(( unsigned int)
_temp1766 > 1u?*(( int*) _temp1766) == Cyc_Absyn_Upper_b: 0){ _LL1773: _temp1772=((
struct Cyc_Absyn_Upper_b_struct*) _temp1766)->f1; goto _LL1769;} else{ goto
_LL1770;} _LL1770: if( _temp1766 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1771;}
else{ goto _LL1767;} _LL1769: if( Cyc_Evexp_eval_const_uint_exp( _temp1772) < 1){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof( unsigned char), 46u));}{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( 0, _temp1760); if( Cyc_Toc_is_nullable(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1195->topt))->v)){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1195->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1774=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1774->hd=( void*) _temp1195;
_temp1774->tl= 0; _temp1774;}), 0), 0), _temp1193, 0))->r));} goto _LL1767;}
_LL1771: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1764);( void*)( _temp1195->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1758), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_List_List*(* _temp1775)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1778= Cyc_Absyn_copy_exp( _temp1195); struct Cyc_Absyn_Exp*
_temp1779= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp* _temp1780=
Cyc_Absyn_uint_exp( 0, 0); struct Cyc_Absyn_Exp* _temp1776[ 3u]={ _temp1778,
_temp1779, _temp1780}; struct _tagged_arr _temp1777={( void*) _temp1776,( void*)
_temp1776,( void*)( _temp1776 + 3u)}; _temp1775( _temp1777);}), 0), 0))->r));
goto _LL1767;} _LL1767:;} if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1024;}} _LL1074: { void*
_temp1781= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1199->topt))->v); Cyc_Toc_exp_to_c( nv, _temp1199); Cyc_Toc_exp_to_c( nv,
_temp1197);{ void* _temp1782= _temp1781; struct Cyc_List_List* _temp1790; struct
Cyc_Absyn_PtrInfo _temp1792; struct Cyc_Absyn_Conref* _temp1794; struct Cyc_Absyn_Tqual
_temp1796; struct Cyc_Absyn_Conref* _temp1798; void* _temp1800; void* _temp1802;
_LL1784: if(( unsigned int) _temp1782 > 4u?*(( int*) _temp1782) == Cyc_Absyn_TupleType:
0){ _LL1791: _temp1790=(( struct Cyc_Absyn_TupleType_struct*) _temp1782)->f1;
goto _LL1785;} else{ goto _LL1786;} _LL1786: if(( unsigned int) _temp1782 > 4u?*((
int*) _temp1782) == Cyc_Absyn_PointerType: 0){ _LL1793: _temp1792=(( struct Cyc_Absyn_PointerType_struct*)
_temp1782)->f1; _LL1803: _temp1802=( void*) _temp1792.elt_typ; goto _LL1801;
_LL1801: _temp1800=( void*) _temp1792.rgn_typ; goto _LL1799; _LL1799: _temp1798=
_temp1792.nullable; goto _LL1797; _LL1797: _temp1796= _temp1792.tq; goto _LL1795;
_LL1795: _temp1794= _temp1792.bounds; goto _LL1787;} else{ goto _LL1788;}
_LL1788: goto _LL1789; _LL1785: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1197) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1199, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL1783;} _LL1787:{ void*
_temp1804=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1794); struct Cyc_Absyn_Exp* _temp1810; _LL1806: if((
unsigned int) _temp1804 > 1u?*(( int*) _temp1804) == Cyc_Absyn_Upper_b: 0){
_LL1811: _temp1810=(( struct Cyc_Absyn_Upper_b_struct*) _temp1804)->f1; goto
_LL1807;} else{ goto _LL1808;} _LL1808: if( _temp1804 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1809;} else{ goto _LL1805;} _LL1807: _temp1810= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1810), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1798); void* ta1= Cyc_Toc_typ_to_c( _temp1802);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1796); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_List_List*(* _temp1812)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1815= _temp1199; struct Cyc_Absyn_Exp* _temp1816=
_temp1810; struct Cyc_Absyn_Exp* _temp1817= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
struct Cyc_Absyn_Exp* _temp1818= _temp1197; struct Cyc_Absyn_Exp* _temp1813[ 4u]={
_temp1815, _temp1816, _temp1817, _temp1818}; struct _tagged_arr _temp1814={(
void*) _temp1813,( void*) _temp1813,( void*)( _temp1813 + 4u)}; _temp1812(
_temp1814);}), 0), 0), 0))->r));} else{( void*)( _temp1197->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_List_List*(*
_temp1819)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp1822= _temp1810; struct Cyc_Absyn_Exp*
_temp1823= Cyc_Absyn_copy_exp( _temp1197); struct Cyc_Absyn_Exp* _temp1820[ 2u]={
_temp1822, _temp1823}; struct _tagged_arr _temp1821={( void*) _temp1820,( void*)
_temp1820,( void*)( _temp1820 + 2u)}; _temp1819( _temp1821);}), 0))->r));} goto
_LL1805;} _LL1809: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1802);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
ta1, _temp1796), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_List_List*(* _temp1824)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1827= _temp1199;
struct Cyc_Absyn_Exp* _temp1828= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp1829= _temp1197; struct Cyc_Absyn_Exp* _temp1825[ 3u]={ _temp1827,
_temp1828, _temp1829}; struct _tagged_arr _temp1826={( void*) _temp1825,( void*)
_temp1825,( void*)( _temp1825 + 3u)}; _temp1824( _temp1826);}), 0), 0), 0))->r));
goto _LL1805;} _LL1805:;} goto _LL1783; _LL1789:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u)); goto _LL1783; _LL1783:;} goto _LL1024;} _LL1076:
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 0,
0, _temp1201))->r));} else{ struct Cyc_List_List* _temp1830=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, _temp1201); struct _tagged_arr* _temp1831= Cyc_Toc_add_tuple_type(
_temp1830); struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1201 != 0;(
_temp1201=(( struct Cyc_List_List*) _check_null( _temp1201))->tl, i ++)){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1201))->hd);
dles=({ struct Cyc_List_List* _temp1832=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1832->hd=( void*)({ struct _tuple4*
_temp1833=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1833->f1=
0; _temp1833->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1201))->hd; _temp1833;}); _temp1832->tl= dles; _temp1832;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL1024; _LL1078:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1203, 0))->r));{ struct Cyc_List_List* _temp1834= _temp1203; for( 0;
_temp1834 != 0; _temp1834=(( struct Cyc_List_List*) _check_null( _temp1834))->tl){
struct _tuple4 _temp1837; struct Cyc_Absyn_Exp* _temp1838; struct Cyc_List_List*
_temp1840; struct _tuple4* _temp1835=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1834))->hd; _temp1837=* _temp1835; _LL1841: _temp1840=
_temp1837.f1; goto _LL1839; _LL1839: _temp1838= _temp1837.f2; goto _LL1836;
_LL1836: Cyc_Toc_exp_to_c( nv, _temp1838);}} goto _LL1024; _LL1080: {
unsigned int _temp1842= Cyc_Evexp_eval_const_uint_exp( _temp1207); Cyc_Toc_exp_to_c(
nv, _temp1205);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1205)){
unsigned int i= 0; for( 0; i < _temp1842; i ++){ es=({ struct Cyc_List_List*
_temp1843=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1843->hd=( void*)({ struct _tuple4* _temp1844=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1844->f1= 0; _temp1844->f2= _temp1205; _temp1844;});
_temp1843->tl= es; _temp1843;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL1024;}} _LL1082: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1213, _temp1217))->r));}
else{ if( _temp1211 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Struct_e: missing structdecl pointer", sizeof( unsigned char), 37u));}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp1211); struct _RegionHandle _temp1845= _new_region(); struct _RegionHandle*
rgn=& _temp1845; _push_region( rgn);{ struct Cyc_List_List* _temp1846=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1213,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1847= 0;{ struct Cyc_List_List*
_temp1848=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1848 != 0; _temp1848=(( struct Cyc_List_List*) _check_null(
_temp1848))->tl){ struct Cyc_List_List* _temp1849= _temp1846; for( 0; _temp1849
!= 0; _temp1849=(( struct Cyc_List_List*) _check_null( _temp1849))->tl){ if((*((
struct _tuple11*)(( struct Cyc_List_List*) _check_null( _temp1849))->hd)).f1 ==(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1848))->hd){
struct _tuple11 _temp1852; struct Cyc_Absyn_Exp* _temp1853; struct Cyc_Absyn_Structfield*
_temp1855; struct _tuple11* _temp1850=( struct _tuple11*)(( struct Cyc_List_List*)
_check_null( _temp1849))->hd; _temp1852=* _temp1850; _LL1856: _temp1855=
_temp1852.f1; goto _LL1854; _LL1854: _temp1853= _temp1852.f2; goto _LL1851;
_LL1851: { void* _temp1857=( void*) _temp1855->type; Cyc_Toc_exp_to_c( nv,
_temp1853); if( Cyc_Toc_is_void_star( _temp1857)){( void*)( _temp1853->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1853->r, 0), 0))->r));} _temp1847=({ struct Cyc_List_List* _temp1858=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1858->hd=(
void*)({ struct _tuple4* _temp1859=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1859->f1= 0; _temp1859->f2= _temp1853; _temp1859;}); _temp1858->tl=
_temp1847; _temp1858;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1847), 0))->r));}; _pop_region( rgn);}} goto _LL1024; _LL1084: { struct Cyc_List_List*
fs;{ void* _temp1860= Cyc_Tcutil_compress( _temp1221); struct Cyc_List_List*
_temp1866; _LL1862: if(( unsigned int) _temp1860 > 4u?*(( int*) _temp1860) ==
Cyc_Absyn_AnonStructType: 0){ _LL1867: _temp1866=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1860)->f1; goto _LL1863;} else{ goto _LL1864;} _LL1864: goto _LL1865;
_LL1863: fs= _temp1866; goto _LL1861; _LL1865: fs=(( struct Cyc_List_List*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp1868= Cyc_Absynpp_typ2string( _temp1221); xprintf("anon struct has type %.*s",
_get_arr_size( _temp1868, 1u), _temp1868.curr);})); goto _LL1861; _LL1861:;}{
struct _RegionHandle _temp1869= _new_region(); struct _RegionHandle* rgn=&
_temp1869; _push_region( rgn);{ struct Cyc_List_List* _temp1870=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1219, fs); for( 0; _temp1870 != 0; _temp1870=(( struct Cyc_List_List*)
_check_null( _temp1870))->tl){ struct _tuple11 _temp1873; struct Cyc_Absyn_Exp*
_temp1874; struct Cyc_Absyn_Structfield* _temp1876; struct _tuple11* _temp1871=(
struct _tuple11*)(( struct Cyc_List_List*) _check_null( _temp1870))->hd;
_temp1873=* _temp1871; _LL1877: _temp1876= _temp1873.f1; goto _LL1875; _LL1875:
_temp1874= _temp1873.f2; goto _LL1872; _LL1872: { void* _temp1878=( void*)
_temp1876->type; Cyc_Toc_exp_to_c( nv, _temp1874); if( Cyc_Toc_is_void_star(
_temp1878)){( void*)( _temp1874->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1874->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1219, 0))->r));}; _pop_region( rgn);}
goto _LL1024;} _LL1086: { struct _tuple0* qv= _temp1223->name; if( _temp1225->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1879= _temp1225->fields == 0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1225->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1879))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1879))->hd)->typs == 0){ tag_count ++;}
_temp1879=(( struct Cyc_List_List*) _check_null( _temp1879))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL1024;} _LL1088: { struct _tuple0* _temp1880= _temp1233->name; struct Cyc_List_List*
_temp1881= _temp1233->typs; struct _tuple0* _temp1882= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1883= Cyc_Absyn_var_exp( _temp1882, 0); void*
_temp1884= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1880, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1235->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1880, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1885= _temp1235->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1235->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1880,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1885))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1885))->hd)->typs != 0){ tag_count ++;}
_temp1885=(( struct Cyc_List_List*) _check_null( _temp1885))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp1886= 0; for( 0;
_temp1237 != 0;( _temp1237=(( struct Cyc_List_List*) _check_null( _temp1237))->tl,
_temp1881=(( struct Cyc_List_List*) _check_null( _temp1881))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1237))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1881))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} _temp1886=({
struct Cyc_List_List* _temp1887=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1887[ 0]=({ struct Cyc_List_List _temp1888;
_temp1888.hd=( void*)({ struct _tuple4* _temp1889=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4) * 1); _temp1889[ 0]=({ struct _tuple4 _temp1890;
_temp1890.f1= 0; _temp1890.f2= cur_e; _temp1890;}); _temp1889;}); _temp1888.tl=
_temp1886; _temp1888;}); _temp1887;});} _temp1886=({ struct Cyc_List_List*
_temp1891=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1891[ 0]=({ struct Cyc_List_List _temp1892; _temp1892.hd=( void*)({ struct
_tuple4* _temp1893=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp1893[ 0]=({ struct _tuple4 _temp1894; _temp1894.f1= 0; _temp1894.f2=
tag_exp; _temp1894;}); _temp1893;}); _temp1892.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1886); _temp1892;}); _temp1891;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1895=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1895->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1880, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp1895;}), _temp1886, 0))->r));}} else{ struct
Cyc_List_List* _temp1896=({ struct Cyc_List_List* _temp1902=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1902->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1883, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1880, 0), 0); _temp1902->tl= 0; _temp1902;});{ int i= 1; for( 0; _temp1237
!= 0;((( _temp1237=(( struct Cyc_List_List*) _check_null( _temp1237))->tl, i ++)),
_temp1881=(( struct Cyc_List_List*) _check_null( _temp1881))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1237))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1881))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp1897= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1883, Cyc_Toc_fieldname(
i), 0), cur_e, 0); _temp1896=({ struct Cyc_List_List* _temp1898=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1898->hd=( void*) _temp1897;
_temp1898->tl= _temp1896; _temp1898;});}}}{ struct Cyc_Absyn_Stmt* _temp1899=
Cyc_Absyn_exp_stmt( _temp1883, 0); struct Cyc_Absyn_Stmt* _temp1900= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1901=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1901->hd=( void*) _temp1899; _temp1901->tl= _temp1896; _temp1901;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1882, _temp1884, 0, _temp1900, 0), 0))->r));}} goto _LL1024;} _LL1090: goto
_LL1024; _LL1092: { void* t_c= Cyc_Toc_typ_to_c( _temp1249); if( _temp1251 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1251); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1249, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL1024;} _LL1094: Cyc_Toc_stmt_to_c(
nv, _temp1253); goto _LL1024; _LL1096:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem", sizeof( unsigned char), 14u)); goto _LL1024; _LL1098:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u)); goto _LL1024; _LL1100:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u)); goto _LL1024;
_LL1102:(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("fill",
sizeof( unsigned char), 5u)); goto _LL1024; _LL1024:;}} static struct Cyc_Absyn_Stmt*
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
_temp1903=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp1939; struct Cyc_Absyn_Vardecl
_temp1941; struct Cyc_List_List* _temp1942; struct Cyc_Core_Opt* _temp1944;
struct Cyc_Absyn_Exp* _temp1946; void* _temp1948; struct Cyc_Absyn_Tqual
_temp1950; struct _tuple0* _temp1952; void* _temp1954; struct Cyc_Absyn_Vardecl*
_temp1956; int _temp1958; void* _temp1960; unsigned char _temp1962; struct
_tagged_arr _temp1964; struct Cyc_Absyn_Enumfield* _temp1966; struct Cyc_Absyn_Enumdecl*
_temp1968; struct Cyc_List_List* _temp1970; struct Cyc_List_List* _temp1972;
struct Cyc_Absyn_Tunionfield* _temp1974; struct Cyc_Absyn_Tuniondecl* _temp1976;
struct Cyc_Absyn_Pat* _temp1979; struct Cyc_Absyn_Pat _temp1981; struct Cyc_Position_Segment*
_temp1982; struct Cyc_Core_Opt* _temp1984; void* _temp1986; struct Cyc_List_List*
_temp1988; struct Cyc_List_List* _temp1990; struct Cyc_Absyn_Tunionfield*
_temp1992; struct Cyc_Absyn_Tuniondecl* _temp1994; struct Cyc_List_List*
_temp1996; struct Cyc_List_List* _temp1998; struct Cyc_Absyn_Tunionfield*
_temp2000; struct Cyc_Absyn_Tuniondecl* _temp2002; struct Cyc_List_List*
_temp2004; struct Cyc_List_List* _temp2006; struct Cyc_List_List* _temp2008;
struct Cyc_Core_Opt* _temp2010; struct Cyc_Absyn_Structdecl* _temp2012; struct
Cyc_Absyn_Pat* _temp2014; struct _tuple0* _temp2016; struct Cyc_List_List*
_temp2018; struct Cyc_List_List* _temp2020; struct _tuple0* _temp2022; struct
Cyc_List_List* _temp2024; struct Cyc_List_List* _temp2026; struct _tuple0*
_temp2028; _LL1905: if(( unsigned int) _temp1903 > 2u?*(( int*) _temp1903) ==
Cyc_Absyn_Var_p: 0){ _LL1940: _temp1939=(( struct Cyc_Absyn_Var_p_struct*)
_temp1903)->f1; _temp1941=* _temp1939; _LL1955: _temp1954=( void*) _temp1941.sc;
goto _LL1953; _LL1953: _temp1952= _temp1941.name; goto _LL1951; _LL1951:
_temp1950= _temp1941.tq; goto _LL1949; _LL1949: _temp1948=( void*) _temp1941.type;
goto _LL1947; _LL1947: _temp1946= _temp1941.initializer; goto _LL1945; _LL1945:
_temp1944= _temp1941.rgn; goto _LL1943; _LL1943: _temp1942= _temp1941.attributes;
goto _LL1906;} else{ goto _LL1907;} _LL1907: if( _temp1903 ==( void*) Cyc_Absyn_Wild_p){
goto _LL1908;} else{ goto _LL1909;} _LL1909: if(( unsigned int) _temp1903 > 2u?*((
int*) _temp1903) == Cyc_Absyn_Reference_p: 0){ _LL1957: _temp1956=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1903)->f1; goto _LL1910;} else{ goto _LL1911;} _LL1911: if( _temp1903 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1912;} else{ goto _LL1913;} _LL1913: if((
unsigned int) _temp1903 > 2u?*(( int*) _temp1903) == Cyc_Absyn_Int_p: 0){
_LL1961: _temp1960=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1903)->f1;
goto _LL1959; _LL1959: _temp1958=(( struct Cyc_Absyn_Int_p_struct*) _temp1903)->f2;
goto _LL1914;} else{ goto _LL1915;} _LL1915: if(( unsigned int) _temp1903 > 2u?*((
int*) _temp1903) == Cyc_Absyn_Char_p: 0){ _LL1963: _temp1962=(( struct Cyc_Absyn_Char_p_struct*)
_temp1903)->f1; goto _LL1916;} else{ goto _LL1917;} _LL1917: if(( unsigned int)
_temp1903 > 2u?*(( int*) _temp1903) == Cyc_Absyn_Float_p: 0){ _LL1965: _temp1964=((
struct Cyc_Absyn_Float_p_struct*) _temp1903)->f1; goto _LL1918;} else{ goto
_LL1919;} _LL1919: if(( unsigned int) _temp1903 > 2u?*(( int*) _temp1903) == Cyc_Absyn_Enum_p:
0){ _LL1969: _temp1968=(( struct Cyc_Absyn_Enum_p_struct*) _temp1903)->f1; goto
_LL1967; _LL1967: _temp1966=(( struct Cyc_Absyn_Enum_p_struct*) _temp1903)->f2;
goto _LL1920;} else{ goto _LL1921;} _LL1921: if(( unsigned int) _temp1903 > 2u?*((
int*) _temp1903) == Cyc_Absyn_Tunion_p: 0){ _LL1977: _temp1976=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1903)->f1; goto _LL1975; _LL1975: _temp1974=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1903)->f2; goto _LL1973; _LL1973: _temp1972=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1903)->f3; goto _LL1971; _LL1971: _temp1970=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1903)->f4; if( _temp1970 == 0){ goto _LL1922;} else{ goto _LL1923;}} else{
goto _LL1923;} _LL1923: if(( unsigned int) _temp1903 > 2u?*(( int*) _temp1903)
== Cyc_Absyn_Pointer_p: 0){ _LL1980: _temp1979=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1903)->f1; _temp1981=* _temp1979; _LL1987: _temp1986=( void*) _temp1981.r;
if(( unsigned int) _temp1986 > 2u?*(( int*) _temp1986) == Cyc_Absyn_Tunion_p: 0){
_LL1995: _temp1994=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1986)->f1; goto
_LL1993; _LL1993: _temp1992=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1986)->f2;
goto _LL1991; _LL1991: _temp1990=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1986)->f3;
goto _LL1989; _LL1989: _temp1988=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1986)->f4;
goto _LL1985;} else{ goto _LL1925;} _LL1985: _temp1984= _temp1981.topt; goto
_LL1983; _LL1983: _temp1982= _temp1981.loc; goto _LL1978;} else{ goto _LL1925;}
_LL1978: if( _temp1988 != 0){ goto _LL1924;} else{ goto _LL1925;} _LL1925: if((
unsigned int) _temp1903 > 2u?*(( int*) _temp1903) == Cyc_Absyn_Tunion_p: 0){
_LL2003: _temp2002=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1903)->f1; goto
_LL2001; _LL2001: _temp2000=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1903)->f2;
goto _LL1999; _LL1999: _temp1998=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1903)->f3;
goto _LL1997; _LL1997: _temp1996=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1903)->f4;
goto _LL1926;} else{ goto _LL1927;} _LL1927: if(( unsigned int) _temp1903 > 2u?*((
int*) _temp1903) == Cyc_Absyn_Tuple_p: 0){ _LL2005: _temp2004=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1903)->f1; goto _LL1928;} else{ goto _LL1929;} _LL1929: if(( unsigned int)
_temp1903 > 2u?*(( int*) _temp1903) == Cyc_Absyn_Struct_p: 0){ _LL2013:
_temp2012=(( struct Cyc_Absyn_Struct_p_struct*) _temp1903)->f1; goto _LL2011;
_LL2011: _temp2010=(( struct Cyc_Absyn_Struct_p_struct*) _temp1903)->f2; goto
_LL2009; _LL2009: _temp2008=(( struct Cyc_Absyn_Struct_p_struct*) _temp1903)->f3;
goto _LL2007; _LL2007: _temp2006=(( struct Cyc_Absyn_Struct_p_struct*) _temp1903)->f4;
goto _LL1930;} else{ goto _LL1931;} _LL1931: if(( unsigned int) _temp1903 > 2u?*((
int*) _temp1903) == Cyc_Absyn_Pointer_p: 0){ _LL2015: _temp2014=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1903)->f1; goto _LL1932;} else{ goto _LL1933;} _LL1933: if(( unsigned int)
_temp1903 > 2u?*(( int*) _temp1903) == Cyc_Absyn_UnknownId_p: 0){ _LL2017:
_temp2016=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1903)->f1; goto _LL1934;}
else{ goto _LL1935;} _LL1935: if(( unsigned int) _temp1903 > 2u?*(( int*)
_temp1903) == Cyc_Absyn_UnknownCall_p: 0){ _LL2023: _temp2022=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1903)->f1; goto _LL2021; _LL2021: _temp2020=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1903)->f2; goto _LL2019; _LL2019: _temp2018=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1903)->f3; goto _LL1936;} else{ goto _LL1937;} _LL1937: if(( unsigned int)
_temp1903 > 2u?*(( int*) _temp1903) == Cyc_Absyn_UnknownFields_p: 0){ _LL2029:
_temp2028=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1903)->f1; goto
_LL2027; _LL2027: _temp2026=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1903)->f2; goto _LL2025; _LL2025: _temp2024=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1903)->f3; goto _LL1938;} else{ goto _LL1904;} _LL1906: nv= Cyc_Toc_add_varmap(
nv, _temp1952, r); goto _LL1908; _LL1908: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL1904; _LL1910: { struct _tuple0* _temp2030= Cyc_Toc_temp_var(); decls=({
struct Cyc_List_List* _temp2031=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2031->hd=( void*)({ struct _tuple13*
_temp2032=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2032->f1= _temp2030; _temp2032->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp2032;}); _temp2031->tl= decls; _temp2031;}); nv= Cyc_Toc_add_varmap(
nv, _temp1956->name, Cyc_Absyn_var_exp( _temp2030, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2030, 0), Cyc_Absyn_address_exp(
path, 0), 0), Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1904;} _LL1912: s=
Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab);
goto _LL1904; _LL1914: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp(
_temp1960, _temp1958, 0), succ_lab, fail_lab); goto _LL1904; _LL1916: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_char_exp( _temp1962, 0), succ_lab, fail_lab); goto _LL1904; _LL1918:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp( _temp1964, 0), succ_lab,
fail_lab); goto _LL1904; _LL1920: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2033=( struct Cyc_Absyn_Enum_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct)); _temp2033[ 0]=({ struct Cyc_Absyn_Enum_e_struct
_temp2034; _temp2034.tag= Cyc_Absyn_Enum_e; _temp2034.f1= _temp1966->name;
_temp2034.f2=( struct Cyc_Absyn_Enumdecl*) _temp1968; _temp2034.f3=( struct Cyc_Absyn_Enumfield*)
_temp1966; _temp2034;}); _temp2033;}), 0), succ_lab, fail_lab); goto _LL1904;
_LL1922: { struct Cyc_Absyn_Exp* cmp_exp= Cyc_Absyn_var_exp( _temp1974->name, 0);
if( ! _temp1976->is_xtunion){ cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt( r, cmp_exp, succ_lab, fail_lab); goto
_LL1904;} _LL1924: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1988); struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag(
_temp1992->name, _tag_arr("_struct", sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq),
r, 0); struct Cyc_List_List* _temp2035=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1992->typs);
struct Cyc_List_List* _temp2036=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1988); for(
0; _temp2036 != 0;((( _temp2036=(( struct Cyc_List_List*) _check_null( _temp2036))->tl,
_temp2035=(( struct Cyc_List_List*) _check_null( _temp2035))->tl)), cnt --)){
struct Cyc_Absyn_Pat* _temp2037=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2036))->hd; void* _temp2038=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2035))->hd)).f2; struct _tuple0* _temp2039= Cyc_Toc_temp_var();
void* _temp2040=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2037->topt))->v;
void* _temp2041= Cyc_Toc_typ_to_c( _temp2040); struct _tagged_arr* _temp2042=
Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp* _temp2043= Cyc_Absyn_structarrow_exp(
rcast, Cyc_Toc_fieldname( cnt), 0); if( Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c(
_temp2038))){ _temp2043= Cyc_Absyn_cast_exp( _temp2041, _temp2043, 0);} decls=({
struct Cyc_List_List* _temp2044=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2044->hd=( void*)({ struct _tuple13*
_temp2045=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2045->f1= _temp2039; _temp2045->f2= _temp2041; _temp2045;}); _temp2044->tl=
decls; _temp2044;});{ struct _tuple12 _temp2046= Cyc_Toc_xlate_pat( nv, rgn,
_temp2040, Cyc_Absyn_var_exp( _temp2039, 0), _temp2043, _temp2037, succ_lab,
fail_lab, decls); nv= _temp2046.f1; decls= _temp2046.f2;{ struct Cyc_Absyn_Stmt*
_temp2047= _temp2046.f3; struct Cyc_Absyn_Stmt* _temp2048= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2039, 0), _temp2043, 0); struct Cyc_Absyn_Stmt*
_temp2049= Cyc_Absyn_seq_stmt( _temp2048, _temp2047, 0); ss=({ struct Cyc_List_List*
_temp2050=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2050->hd=( void*) Cyc_Absyn_label_stmt( _temp2042, _temp2049, 0); _temp2050->tl=
ss; _temp2050;}); succ_lab= _temp2042;}}}{ struct Cyc_Absyn_Exp* test_exp; if(
_temp1994->is_xtunion){ struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp(
e2, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1992->name, 0);
test_exp= Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e, 0), Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);}
else{ unsigned int max_tag= 0;{ struct Cyc_List_List* _temp2051=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1994->fields))->v; for( 0; _temp2051 !=
0; _temp2051=(( struct Cyc_List_List*) _check_null( _temp2051))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2052=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2051))->hd; if( _temp2052->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1992->name, 0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1904;}} _LL1926: _temp2004= _temp1996; goto _LL1928; _LL1928: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2004);{ struct Cyc_List_List* _temp2053=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2004); for(
0; _temp2053 != 0;( _temp2053=(( struct Cyc_List_List*) _check_null( _temp2053))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp2054=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2053))->hd; struct _tuple0* _temp2055= Cyc_Toc_temp_var();
void* _temp2056=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2054->topt))->v;
struct _tagged_arr* _temp2057= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2058=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2058->hd=( void*)({ struct _tuple13* _temp2059=( struct _tuple13*)
_region_malloc( rgn, sizeof( struct _tuple13)); _temp2059->f1= _temp2055;
_temp2059->f2= Cyc_Toc_typ_to_c( _temp2056); _temp2059;}); _temp2058->tl= decls;
_temp2058;});{ struct _tuple12 _temp2060= Cyc_Toc_xlate_pat( nv, rgn, _temp2056,
Cyc_Absyn_var_exp( _temp2055, 0), Cyc_Absyn_structmember_exp( path, Cyc_Toc_fieldname(
cnt), 0), _temp2054, succ_lab, fail_lab, decls); nv= _temp2060.f1; decls=
_temp2060.f2;{ struct Cyc_Absyn_Stmt* _temp2061= _temp2060.f3; struct Cyc_Absyn_Stmt*
_temp2062= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2055, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2061, 0); ss=({ struct Cyc_List_List* _temp2063=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2063->hd=( void*) Cyc_Absyn_label_stmt(
_temp2057, _temp2062, 0); _temp2063->tl= ss; _temp2063;}); succ_lab= _temp2057;}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL1904;} _LL1930: { struct Cyc_List_List*
ss= 0;{ struct Cyc_List_List* _temp2064=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2006); for(
0; _temp2064 != 0; _temp2064=(( struct Cyc_List_List*) _check_null( _temp2064))->tl){
struct _tuple14* _temp2065=( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp2064))->hd; struct Cyc_Absyn_Pat* _temp2066=(* _temp2065).f2;
struct _tagged_arr* f;{ void* _temp2067=( void*)(( struct Cyc_List_List*)
_check_null((* _temp2065).f1))->hd; struct _tagged_arr* _temp2073; _LL2069: if(*((
int*) _temp2067) == Cyc_Absyn_FieldName){ _LL2074: _temp2073=(( struct Cyc_Absyn_FieldName_struct*)
_temp2067)->f1; goto _LL2070;} else{ goto _LL2071;} _LL2071: goto _LL2072;
_LL2070: f= _temp2073; goto _LL2068; _LL2072:( int) _throw(( void*) Cyc_Toc_Match_error);
_LL2068:;}{ struct _tuple0* _temp2075= Cyc_Toc_temp_var(); void* _temp2076=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp2066->topt))->v; void*
_temp2077= Cyc_Toc_typ_to_c( _temp2076); struct _tagged_arr* _temp2078= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp2079=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2079->hd=( void*)({ struct _tuple13*
_temp2080=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2080->f1= _temp2075; _temp2080->f2= _temp2077; _temp2080;}); _temp2079->tl=
decls; _temp2079;});{ struct _tuple12 _temp2081= Cyc_Toc_xlate_pat( nv, rgn,
_temp2076, Cyc_Absyn_var_exp( _temp2075, 0), Cyc_Absyn_structmember_exp( path, f,
0), _temp2066, succ_lab, fail_lab, decls); nv= _temp2081.f1; decls= _temp2081.f2;{
struct Cyc_Absyn_Exp* _temp2082= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2012->fields))->v,
f)))->type)){ _temp2082= Cyc_Absyn_cast_exp( _temp2077, _temp2082, 0);}{ struct
Cyc_Absyn_Stmt* _temp2083= _temp2081.f3; struct Cyc_Absyn_Stmt* _temp2084= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2075, 0), _temp2082, 0),
_temp2083, 0); ss=({ struct Cyc_List_List* _temp2085=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2085->hd=( void*) Cyc_Absyn_label_stmt(
_temp2078, _temp2084, 0); _temp2085->tl= ss; _temp2085;}); succ_lab= _temp2078;}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL1904;} _LL1932: { struct _tuple0*
_temp2086= Cyc_Toc_temp_var(); void* _temp2087=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2014->topt))->v; decls=({ struct Cyc_List_List* _temp2088=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2088->hd=( void*)({ struct _tuple13* _temp2089=( struct _tuple13*)
_region_malloc( rgn, sizeof( struct _tuple13)); _temp2089->f1= _temp2086;
_temp2089->f2= Cyc_Toc_typ_to_c( _temp2087); _temp2089;}); _temp2088->tl= decls;
_temp2088;});{ struct _tuple12 _temp2090= Cyc_Toc_xlate_pat( nv, rgn, _temp2087,
Cyc_Absyn_var_exp( _temp2086, 0), Cyc_Absyn_deref_exp( path, 0), _temp2014,
succ_lab, fail_lab, decls); nv= _temp2090.f1; decls= _temp2090.f2;{ struct Cyc_Absyn_Stmt*
_temp2091= _temp2090.f3; struct Cyc_Absyn_Stmt* _temp2092= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2086, 0), Cyc_Absyn_deref_exp( r,
0), 0), _temp2091, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2092, 0);} else{ s= _temp2092;} goto _LL1904;}}} _LL1934: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid",
sizeof( unsigned char), 10u)); goto _LL1904; _LL1936: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall", sizeof(
unsigned char), 12u)); goto _LL1904; _LL1938: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields", sizeof(
unsigned char), 14u)); goto _LL1904; _LL1904:;} return({ struct _tuple12
_temp2093; _temp2093.f1= nv; _temp2093.f2= decls; _temp2093.f3= s; _temp2093;});}
struct _tuple15{ struct _tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple15* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple15* _temp2094=(
struct _tuple15*) _region_malloc( r, sizeof( struct _tuple15)); _temp2094->f1=
Cyc_Toc_fresh_label(); _temp2094->f2= Cyc_Toc_fresh_label(); _temp2094->f3= sc;
_temp2094;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp2095=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; int leave_as_switch;{ void* _temp2096= Cyc_Tcutil_compress(
_temp2095); void* _temp2102; void* _temp2104; _LL2098: if(( unsigned int)
_temp2096 > 4u?*(( int*) _temp2096) == Cyc_Absyn_IntType: 0){ _LL2105: _temp2104=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2096)->f1; goto _LL2103; _LL2103:
_temp2102=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2096)->f2; goto
_LL2099;} else{ goto _LL2100;} _LL2100: goto _LL2101; _LL2099: leave_as_switch=
1; goto _LL2097; _LL2101: leave_as_switch= 0; goto _LL2097; _LL2097:;}{ struct
Cyc_List_List* _temp2106= scs; for( 0; _temp2106 != 0; _temp2106=(( struct Cyc_List_List*)
_check_null( _temp2106))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2106))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2106))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2107= scs; for( 0; _temp2107
!= 0; _temp2107=(( struct Cyc_List_List*) _check_null( _temp2107))->tl){ struct
Cyc_Absyn_Stmt* _temp2108=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2107))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp2107))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp2108, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp2108);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp2109= _new_region(); struct _RegionHandle* rgn=&
_temp2109; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple15*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2110= lscs; for( 0;
_temp2110 != 0; _temp2110=(( struct Cyc_List_List*) _check_null( _temp2110))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp2110))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp2110))->tl == 0? end_l:(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp2110))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp2110))->hd)).f2; if( sc->where_clause == 0){ struct _tuple12
_temp2111= Cyc_Toc_xlate_pat( nv, rgn, _temp2095, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp2112=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2112->hd=( void*)
_temp2111.f1; _temp2112->tl= nvs; _temp2112;}); decls= _temp2111.f2; test_stmts=({
struct Cyc_List_List* _temp2113=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2113->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2110))->hd)).f1,
_temp2111.f3, 0); _temp2113->tl= test_stmts; _temp2113;});} else{ struct Cyc_Absyn_Exp*
_temp2114=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp2115= Cyc_Toc_fresh_label(); struct _tuple12 _temp2116= Cyc_Toc_xlate_pat(
nv, rgn, _temp2095, r, path, sc->pattern, _temp2115, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp2116.f1, _temp2114); nvs=({ struct Cyc_List_List* _temp2117=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2117->hd=( void*)
_temp2116.f1; _temp2117->tl= nvs; _temp2117;}); decls= _temp2116.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2114, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2115, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2118=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2118->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2110))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2116.f3, s3, 0), 0); _temp2118->tl= test_stmts;
_temp2118;});}}}}{ struct Cyc_Absyn_Stmt* _temp2119= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl != 0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2120=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp2120, end_l,(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp2121=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp2121, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp2119, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls != 0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple13 _temp2124; void* _temp2125; struct
_tuple0* _temp2127; struct _tuple13* _temp2122=( struct _tuple13*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp2124=* _temp2122; _LL2128: _temp2127= _temp2124.f1;
goto _LL2126; _LL2126: _temp2125= _temp2124.f2; goto _LL2123; _LL2123: res= Cyc_Absyn_declare_stmt(
_temp2127, _temp2125, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp2129=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2129[ 0]=({ struct Cyc_List_List
_temp2130; _temp2130.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp2130.tl= 0; _temp2130;}); _temp2129;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2131=( void*) s->r; struct Cyc_Absyn_Exp*
_temp2173; struct Cyc_Absyn_Stmt* _temp2175; struct Cyc_Absyn_Stmt* _temp2177;
struct Cyc_Absyn_Exp* _temp2179; struct Cyc_Absyn_Stmt* _temp2181; struct Cyc_Absyn_Stmt*
_temp2183; struct Cyc_Absyn_Exp* _temp2185; struct Cyc_Absyn_Stmt* _temp2187;
struct _tuple2 _temp2189; struct Cyc_Absyn_Stmt* _temp2191; struct Cyc_Absyn_Exp*
_temp2193; struct Cyc_Absyn_Stmt* _temp2195; struct Cyc_Absyn_Stmt* _temp2197;
struct Cyc_Absyn_Stmt* _temp2199; struct _tagged_arr* _temp2201; struct Cyc_Absyn_Stmt*
_temp2203; struct _tuple2 _temp2205; struct Cyc_Absyn_Stmt* _temp2207; struct
Cyc_Absyn_Exp* _temp2209; struct _tuple2 _temp2211; struct Cyc_Absyn_Stmt*
_temp2213; struct Cyc_Absyn_Exp* _temp2215; struct Cyc_Absyn_Exp* _temp2217;
struct Cyc_List_List* _temp2219; struct Cyc_Absyn_Exp* _temp2221; struct Cyc_Absyn_Switch_clause**
_temp2223; struct Cyc_List_List* _temp2225; struct Cyc_Absyn_Stmt* _temp2227;
struct Cyc_Absyn_Decl* _temp2229; struct Cyc_Absyn_Stmt* _temp2231; struct
_tagged_arr* _temp2233; struct _tuple2 _temp2235; struct Cyc_Absyn_Stmt*
_temp2237; struct Cyc_Absyn_Exp* _temp2239; struct Cyc_Absyn_Stmt* _temp2241;
struct Cyc_List_List* _temp2243; struct Cyc_Absyn_Stmt* _temp2245; struct Cyc_Absyn_Stmt*
_temp2247; struct Cyc_Absyn_Vardecl* _temp2249; struct Cyc_Absyn_Tvar* _temp2251;
struct Cyc_List_List* _temp2253; struct Cyc_Absyn_Exp* _temp2255; struct Cyc_Absyn_Stmt*
_temp2257; struct Cyc_Absyn_Stmt* _temp2259; _LL2133: if( _temp2131 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2134;} else{ goto _LL2135;} _LL2135: if((
unsigned int) _temp2131 > 1u?*(( int*) _temp2131) == Cyc_Absyn_Exp_s: 0){
_LL2174: _temp2173=(( struct Cyc_Absyn_Exp_s_struct*) _temp2131)->f1; goto
_LL2136;} else{ goto _LL2137;} _LL2137: if(( unsigned int) _temp2131 > 1u?*((
int*) _temp2131) == Cyc_Absyn_Seq_s: 0){ _LL2178: _temp2177=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2131)->f1; goto _LL2176; _LL2176: _temp2175=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2131)->f2; goto _LL2138;} else{ goto _LL2139;} _LL2139: if(( unsigned int)
_temp2131 > 1u?*(( int*) _temp2131) == Cyc_Absyn_Return_s: 0){ _LL2180:
_temp2179=(( struct Cyc_Absyn_Return_s_struct*) _temp2131)->f1; goto _LL2140;}
else{ goto _LL2141;} _LL2141: if(( unsigned int) _temp2131 > 1u?*(( int*)
_temp2131) == Cyc_Absyn_IfThenElse_s: 0){ _LL2186: _temp2185=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2131)->f1; goto _LL2184; _LL2184: _temp2183=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2131)->f2; goto _LL2182; _LL2182: _temp2181=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2131)->f3; goto _LL2142;} else{ goto _LL2143;} _LL2143: if(( unsigned int)
_temp2131 > 1u?*(( int*) _temp2131) == Cyc_Absyn_While_s: 0){ _LL2190: _temp2189=((
struct Cyc_Absyn_While_s_struct*) _temp2131)->f1; _LL2194: _temp2193= _temp2189.f1;
goto _LL2192; _LL2192: _temp2191= _temp2189.f2; goto _LL2188; _LL2188: _temp2187=((
struct Cyc_Absyn_While_s_struct*) _temp2131)->f2; goto _LL2144;} else{ goto
_LL2145;} _LL2145: if(( unsigned int) _temp2131 > 1u?*(( int*) _temp2131) == Cyc_Absyn_Break_s:
0){ _LL2196: _temp2195=(( struct Cyc_Absyn_Break_s_struct*) _temp2131)->f1; goto
_LL2146;} else{ goto _LL2147;} _LL2147: if(( unsigned int) _temp2131 > 1u?*((
int*) _temp2131) == Cyc_Absyn_Continue_s: 0){ _LL2198: _temp2197=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2131)->f1; goto _LL2148;} else{ goto _LL2149;} _LL2149: if(( unsigned int)
_temp2131 > 1u?*(( int*) _temp2131) == Cyc_Absyn_Goto_s: 0){ _LL2202: _temp2201=((
struct Cyc_Absyn_Goto_s_struct*) _temp2131)->f1; goto _LL2200; _LL2200:
_temp2199=(( struct Cyc_Absyn_Goto_s_struct*) _temp2131)->f2; goto _LL2150;}
else{ goto _LL2151;} _LL2151: if(( unsigned int) _temp2131 > 1u?*(( int*)
_temp2131) == Cyc_Absyn_For_s: 0){ _LL2218: _temp2217=(( struct Cyc_Absyn_For_s_struct*)
_temp2131)->f1; goto _LL2212; _LL2212: _temp2211=(( struct Cyc_Absyn_For_s_struct*)
_temp2131)->f2; _LL2216: _temp2215= _temp2211.f1; goto _LL2214; _LL2214:
_temp2213= _temp2211.f2; goto _LL2206; _LL2206: _temp2205=(( struct Cyc_Absyn_For_s_struct*)
_temp2131)->f3; _LL2210: _temp2209= _temp2205.f1; goto _LL2208; _LL2208:
_temp2207= _temp2205.f2; goto _LL2204; _LL2204: _temp2203=(( struct Cyc_Absyn_For_s_struct*)
_temp2131)->f4; goto _LL2152;} else{ goto _LL2153;} _LL2153: if(( unsigned int)
_temp2131 > 1u?*(( int*) _temp2131) == Cyc_Absyn_Switch_s: 0){ _LL2222:
_temp2221=(( struct Cyc_Absyn_Switch_s_struct*) _temp2131)->f1; goto _LL2220;
_LL2220: _temp2219=(( struct Cyc_Absyn_Switch_s_struct*) _temp2131)->f2; goto
_LL2154;} else{ goto _LL2155;} _LL2155: if(( unsigned int) _temp2131 > 1u?*((
int*) _temp2131) == Cyc_Absyn_Fallthru_s: 0){ _LL2226: _temp2225=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2131)->f1; goto _LL2224; _LL2224: _temp2223=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2131)->f2; goto _LL2156;} else{ goto _LL2157;} _LL2157: if(( unsigned int)
_temp2131 > 1u?*(( int*) _temp2131) == Cyc_Absyn_Decl_s: 0){ _LL2230: _temp2229=((
struct Cyc_Absyn_Decl_s_struct*) _temp2131)->f1; goto _LL2228; _LL2228:
_temp2227=(( struct Cyc_Absyn_Decl_s_struct*) _temp2131)->f2; goto _LL2158;}
else{ goto _LL2159;} _LL2159: if(( unsigned int) _temp2131 > 1u?*(( int*)
_temp2131) == Cyc_Absyn_Label_s: 0){ _LL2234: _temp2233=(( struct Cyc_Absyn_Label_s_struct*)
_temp2131)->f1; goto _LL2232; _LL2232: _temp2231=(( struct Cyc_Absyn_Label_s_struct*)
_temp2131)->f2; goto _LL2160;} else{ goto _LL2161;} _LL2161: if(( unsigned int)
_temp2131 > 1u?*(( int*) _temp2131) == Cyc_Absyn_Do_s: 0){ _LL2242: _temp2241=((
struct Cyc_Absyn_Do_s_struct*) _temp2131)->f1; goto _LL2236; _LL2236: _temp2235=((
struct Cyc_Absyn_Do_s_struct*) _temp2131)->f2; _LL2240: _temp2239= _temp2235.f1;
goto _LL2238; _LL2238: _temp2237= _temp2235.f2; goto _LL2162;} else{ goto
_LL2163;} _LL2163: if(( unsigned int) _temp2131 > 1u?*(( int*) _temp2131) == Cyc_Absyn_TryCatch_s:
0){ _LL2246: _temp2245=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2131)->f1;
goto _LL2244; _LL2244: _temp2243=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2131)->f2; goto _LL2164;} else{ goto _LL2165;} _LL2165: if(( unsigned int)
_temp2131 > 1u?*(( int*) _temp2131) == Cyc_Absyn_Region_s: 0){ _LL2252:
_temp2251=(( struct Cyc_Absyn_Region_s_struct*) _temp2131)->f1; goto _LL2250;
_LL2250: _temp2249=(( struct Cyc_Absyn_Region_s_struct*) _temp2131)->f2; goto
_LL2248; _LL2248: _temp2247=(( struct Cyc_Absyn_Region_s_struct*) _temp2131)->f3;
goto _LL2166;} else{ goto _LL2167;} _LL2167: if(( unsigned int) _temp2131 > 1u?*((
int*) _temp2131) == Cyc_Absyn_SwitchC_s: 0){ _LL2256: _temp2255=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2131)->f1; goto _LL2254; _LL2254: _temp2253=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2131)->f2; goto _LL2168;} else{ goto _LL2169;} _LL2169: if(( unsigned int)
_temp2131 > 1u?*(( int*) _temp2131) == Cyc_Absyn_Cut_s: 0){ _LL2258: _temp2257=((
struct Cyc_Absyn_Cut_s_struct*) _temp2131)->f1; goto _LL2170;} else{ goto
_LL2171;} _LL2171: if(( unsigned int) _temp2131 > 1u?*(( int*) _temp2131) == Cyc_Absyn_Splice_s:
0){ _LL2260: _temp2259=(( struct Cyc_Absyn_Splice_s_struct*) _temp2131)->f1;
goto _LL2172;} else{ goto _LL2132;} _LL2134: return; _LL2136: Cyc_Toc_exp_to_c(
nv, _temp2173); return; _LL2138: Cyc_Toc_stmt_to_c( nv, _temp2177); s= _temp2175;
continue; _LL2140: { struct Cyc_Core_Opt* topt= 0; if( _temp2179 != 0){ topt=({
struct Cyc_Core_Opt* _temp2261=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2261->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2179))->topt))->v);
_temp2261;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2179));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2262=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp2263= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2262, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp2262,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp2179, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp2263, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL2142: Cyc_Toc_exp_to_c( nv, _temp2185); Cyc_Toc_stmt_to_c( nv,
_temp2183); s= _temp2181; continue; _LL2144: Cyc_Toc_exp_to_c( nv, _temp2193);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2187); return; _LL2146: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{ int
dest_depth= _temp2195 == 0? 0:(( struct Cyc_Absyn_Stmt*) _check_null( _temp2195))->try_depth;
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL2148: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp2199=
_temp2197; goto _LL2150; _LL2150: Cyc_Toc_do_npop_before( s->try_depth -((
struct Cyc_Absyn_Stmt*) _check_null( _temp2199))->try_depth, s); return; _LL2152:
Cyc_Toc_exp_to_c( nv, _temp2217); Cyc_Toc_exp_to_c( nv, _temp2215); Cyc_Toc_exp_to_c(
nv, _temp2209); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2203); return;
_LL2154: Cyc_Toc_xlate_switch( nv, s, _temp2221, _temp2219); return; _LL2156:
if( nv->fallthru_info == 0){( int) _throw((( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("fallthru in unexpected place", sizeof(
unsigned char), 29u)));}{ struct _tuple6 _temp2266; struct Cyc_Dict_Dict*
_temp2267; struct Cyc_List_List* _temp2269; struct _tagged_arr* _temp2271;
struct _tuple6* _temp2264=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp2266=* _temp2264; _LL2272: _temp2271= _temp2266.f1;
goto _LL2270; _LL2270: _temp2269= _temp2266.f2; goto _LL2268; _LL2268: _temp2267=
_temp2266.f3; goto _LL2265; _LL2265: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp2271, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp2223)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2273=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2269); struct Cyc_List_List* _temp2274=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp2225); for( 0; _temp2273 != 0;( _temp2273=((
struct Cyc_List_List*) _check_null( _temp2273))->tl, _temp2274=(( struct Cyc_List_List*)
_check_null( _temp2274))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp2274))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp2267,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp2273))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp2274))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}} _LL2158:{ void* _temp2275=( void*) _temp2229->r;
struct Cyc_Absyn_Vardecl* _temp2285; int _temp2287; struct Cyc_Absyn_Exp*
_temp2289; struct Cyc_Core_Opt* _temp2291; struct Cyc_Core_Opt* _temp2293;
struct Cyc_Absyn_Pat* _temp2295; struct Cyc_List_List* _temp2297; _LL2277: if(*((
int*) _temp2275) == Cyc_Absyn_Var_d){ _LL2286: _temp2285=(( struct Cyc_Absyn_Var_d_struct*)
_temp2275)->f1; goto _LL2278;} else{ goto _LL2279;} _LL2279: if(*(( int*)
_temp2275) == Cyc_Absyn_Let_d){ _LL2296: _temp2295=(( struct Cyc_Absyn_Let_d_struct*)
_temp2275)->f1; goto _LL2294; _LL2294: _temp2293=(( struct Cyc_Absyn_Let_d_struct*)
_temp2275)->f2; goto _LL2292; _LL2292: _temp2291=(( struct Cyc_Absyn_Let_d_struct*)
_temp2275)->f3; goto _LL2290; _LL2290: _temp2289=(( struct Cyc_Absyn_Let_d_struct*)
_temp2275)->f4; goto _LL2288; _LL2288: _temp2287=(( struct Cyc_Absyn_Let_d_struct*)
_temp2275)->f5; goto _LL2280;} else{ goto _LL2281;} _LL2281: if(*(( int*)
_temp2275) == Cyc_Absyn_Letv_d){ _LL2298: _temp2297=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2275)->f1; goto _LL2282;} else{ goto _LL2283;} _LL2283: goto _LL2284;
_LL2278: { struct Cyc_Toc_Env* _temp2299= Cyc_Toc_add_varmap( nv, _temp2285->name,
Cyc_Absyn_varb_exp( _temp2285->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2300=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2300[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2301; _temp2301.tag= Cyc_Absyn_Local_b;
_temp2301.f1= _temp2285; _temp2301;}); _temp2300;}), 0)); Cyc_Toc_local_decl_to_c(
_temp2299, _temp2299, _temp2285, _temp2227); goto _LL2276;} _LL2280:{ void*
_temp2302=( void*) _temp2295->r; struct Cyc_Absyn_Vardecl* _temp2308; _LL2304:
if(( unsigned int) _temp2302 > 2u?*(( int*) _temp2302) == Cyc_Absyn_Var_p: 0){
_LL2309: _temp2308=(( struct Cyc_Absyn_Var_p_struct*) _temp2302)->f1; goto
_LL2305;} else{ goto _LL2306;} _LL2306: goto _LL2307; _LL2305: { struct _tuple0*
old_name= _temp2308->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp2308->name= new_name; _temp2308->initializer=( struct Cyc_Absyn_Exp*)
_temp2289;( void*)( _temp2229->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2310=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2310[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2311; _temp2311.tag= Cyc_Absyn_Var_d;
_temp2311.f1= _temp2308; _temp2311;}); _temp2310;})));{ struct Cyc_Toc_Env*
_temp2312= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2313=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2313[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2314; _temp2314.tag= Cyc_Absyn_Local_b; _temp2314.f1=
_temp2308; _temp2314;}); _temp2313;}), 0)); Cyc_Toc_local_decl_to_c( _temp2312,
nv, _temp2308, _temp2227); goto _LL2303;}} _LL2307:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp2295, _temp2291, _temp2289, _temp2287,
_temp2227))->r)); goto _LL2303; _LL2303:;} goto _LL2276; _LL2282: { struct Cyc_List_List*
_temp2315=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2297); if( _temp2315 == 0){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("empty Letv_d", sizeof( unsigned char), 13u));}( void*)( _temp2229->r=(
void*)(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2316=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2316[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2317; _temp2317.tag= Cyc_Absyn_Var_d; _temp2317.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2315))->hd; _temp2317;}); _temp2316;})));
_temp2315=(( struct Cyc_List_List*) _check_null( _temp2315))->tl; for( 0;
_temp2315 != 0; _temp2315=(( struct Cyc_List_List*) _check_null( _temp2315))->tl){
struct Cyc_Absyn_Decl* _temp2318= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2319=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2319[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2320; _temp2320.tag= Cyc_Absyn_Var_d;
_temp2320.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2315))->hd; _temp2320;}); _temp2319;}), 0);( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_decl_stmt( _temp2318, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));}
Cyc_Toc_stmt_to_c( nv, s); goto _LL2276;} _LL2284:(( int(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u)); goto _LL2276; _LL2276:;} return; _LL2160: s= _temp2231;
continue; _LL2162: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2241); Cyc_Toc_exp_to_c(
nv, _temp2239); return; _LL2164: { struct _tuple0* h_var= Cyc_Toc_temp_var();
struct _tuple0* e_var= Cyc_Toc_temp_var(); struct _tuple0* was_thrown_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp2321=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2321->v=( void*) e_typ; _temp2321;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2245);{
struct Cyc_Absyn_Stmt* _temp2322= Cyc_Absyn_seq_stmt( _temp2245, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp2323= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp2324= Cyc_Absyn_var_exp(
_temp2323, 0); struct Cyc_Absyn_Vardecl* _temp2325= Cyc_Absyn_new_vardecl(
_temp2323, Cyc_Absyn_exn_typ, 0); _temp2324->topt=({ struct Cyc_Core_Opt*
_temp2326=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2326->v=( void*) Cyc_Absyn_exn_typ; _temp2326;});{ struct Cyc_Absyn_Pat*
_temp2327=({ struct Cyc_Absyn_Pat* _temp2343=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2343->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2345=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2345[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2346; _temp2346.tag= Cyc_Absyn_Var_p;
_temp2346.f1= _temp2325; _temp2346;}); _temp2345;})); _temp2343->topt=({ struct
Cyc_Core_Opt* _temp2344=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2344->v=( void*) Cyc_Absyn_exn_typ; _temp2344;}); _temp2343->loc= 0;
_temp2343;}); struct Cyc_Absyn_Exp* _temp2328= Cyc_Absyn_throw_exp( _temp2324, 0);
_temp2328->topt=({ struct Cyc_Core_Opt* _temp2329=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2329->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp2329;});{ struct Cyc_Absyn_Stmt* _temp2330= Cyc_Absyn_exp_stmt( _temp2328,
0); struct Cyc_Absyn_Switch_clause* _temp2331=({ struct Cyc_Absyn_Switch_clause*
_temp2340=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2340->pattern= _temp2327; _temp2340->pat_vars=({ struct Cyc_Core_Opt*
_temp2341=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2341->v=( void*)({ struct Cyc_List_List* _temp2342=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2342->hd=( void*) _temp2325;
_temp2342->tl= 0; _temp2342;}); _temp2341;}); _temp2340->where_clause= 0;
_temp2340->body= _temp2330; _temp2340->loc= 0; _temp2340;}); struct Cyc_Absyn_Stmt*
_temp2332= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2243,({ struct Cyc_List_List*
_temp2339=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2339->hd=( void*) _temp2331; _temp2339->tl= 0; _temp2339;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp2332);{ struct Cyc_Absyn_Exp* _temp2333= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp2338=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2338->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2338->tl= 0; _temp2338;}), 0); struct Cyc_Absyn_Stmt*
_temp2334= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp2337=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2337->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp2337->tl= 0; _temp2337;}), 0), 0); struct Cyc_Absyn_Exp* _temp2335= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp2336= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2334, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2335, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2333, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2336, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2322, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp2332, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL2166: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp2249->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp2247);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_List_List*(* _temp2347)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp2350= x_exp; struct Cyc_Absyn_Exp* _temp2348[ 1u]={ _temp2350}; struct
_tagged_arr _temp2349={( void*) _temp2348,( void*) _temp2348,( void*)( _temp2348
+ 1u)}; _temp2347( _temp2349);}), 0), 0), Cyc_Absyn_seq_stmt( _temp2247, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({ struct Cyc_List_List*(* _temp2351)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp2354= x_exp; struct Cyc_Absyn_Exp* _temp2352[ 1u]={
_temp2354}; struct _tagged_arr _temp2353={( void*) _temp2352,( void*) _temp2352,(
void*)( _temp2352 + 1u)}; _temp2351( _temp2353);}), 0), 0), 0), 0), 0), 0))->r));
return;} _LL2168:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("switchC_s",
sizeof( unsigned char), 10u)); return; _LL2170:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("cut", sizeof( unsigned char), 4u)); return; _LL2172:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof(
unsigned char), 7u)); return; _LL2132:;}} struct _tuple16{ struct _tagged_arr*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0; f->rgn_po=
0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* _temp2355= f->args; for( 0; _temp2355 != 0; _temp2355=((
struct Cyc_List_List*) _check_null( _temp2355))->tl){(*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( _temp2355))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2355))->hd)).f3);{
struct _tuple0* _temp2356=({ struct _tuple0* _temp2357=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2357->f1=( void*) Cyc_Absyn_Loc_n;
_temp2357->f2=(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2355))->hd)).f1; _temp2357;}); nv= Cyc_Toc_add_varmap( nv, _temp2356, Cyc_Absyn_var_exp(
_temp2356, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name != 0: 0){ int _temp2360; void* _temp2362;
void* _temp2364; struct Cyc_Absyn_Tqual _temp2366; struct Cyc_Core_Opt*
_temp2368; struct Cyc_Absyn_VarargInfo _temp2358=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs)); _LL2369: _temp2368= _temp2358.name; goto _LL2367;
_LL2367: _temp2366= _temp2358.tq; goto _LL2365; _LL2365: _temp2364=( void*)
_temp2358.type; goto _LL2363; _LL2363: _temp2362=( void*) _temp2358.rgn; goto
_LL2361; _LL2361: _temp2360= _temp2358.inject; goto _LL2359; _LL2359: { void*
_temp2370= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp2364, _temp2362,
_temp2366)); struct _tuple0* _temp2371=({ struct _tuple0* _temp2374=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2374->f1=( void*) Cyc_Absyn_Loc_n;
_temp2374->f2=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
_temp2368))->v; _temp2374;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2372=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2372->hd=( void*)({ struct _tuple16* _temp2373=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp2373->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp2368))->v; _temp2373->f2= _temp2366;
_temp2373->f3= _temp2370; _temp2373;}); _temp2372->tl= 0; _temp2372;})); nv= Cyc_Toc_add_varmap(
nv, _temp2371, Cyc_Absyn_var_exp( _temp2371, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp2375=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp2375 != 0; _temp2375=(( struct
Cyc_List_List*) _check_null( _temp2375))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2375))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2375))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2376= s; _LL2378: if(
_temp2376 ==( void*) Cyc_Absyn_Abstract){ goto _LL2379;} else{ goto _LL2380;}
_LL2380: if( _temp2376 ==( void*) Cyc_Absyn_ExternC){ goto _LL2381;} else{ goto
_LL2382;} _LL2382: goto _LL2383; _LL2379: return( void*) Cyc_Absyn_Public;
_LL2381: return( void*) Cyc_Absyn_Extern; _LL2383: return s; _LL2377:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2384=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 0){
Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2385=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2385->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2385;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2386=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2384).f2); if( _temp2386 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2384).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2386))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2384).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2387=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2387->v=(
void*) _temp2384; _temp2387;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2388=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp2388 != 0; _temp2388=(( struct Cyc_List_List*)
_check_null( _temp2388))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2388))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2388))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp2389=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far == 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp2390=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2390->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2390;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2391=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2389).f2); if( _temp2391 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2389).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2391))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp2389).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2392=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2392->v=(
void*) _temp2389; _temp2392;}); if( u->fields != 0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp2393=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2393 != 0; _temp2393=(( struct Cyc_List_List*)
_check_null( _temp2393))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2393))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2393))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2394=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2394->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2394;});}{ struct
_tuple0* _temp2395= tud->name; if( tud->fields == 0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp2395)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2395));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp2396=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp2396 != 0; _temp2396=(( struct Cyc_List_List*) _check_null(
_temp2396))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2396))->hd; if( f->typs == 0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2397; _temp2397.q_const= 1;
_temp2397.q_volatile= 0; _temp2397.q_restrict= 0; _temp2397;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2398=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2398->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2399=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2399[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2400; _temp2400.tag= Cyc_Absyn_Var_d; _temp2400.f1= vd; _temp2400;});
_temp2399;}), 0); _temp2398->tl= Cyc_Toc_result_decls; _temp2398;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2401; _temp2401.q_const=
1; _temp2401.q_volatile= 0; _temp2401.q_restrict= 0; _temp2401;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2402=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2402->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2403=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2403[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2404; _temp2404.tag= Cyc_Absyn_Var_d; _temp2404.f1= vd; _temp2404;});
_temp2403;}), 0); _temp2402->tl= Cyc_Toc_result_decls; _temp2402;});{ struct Cyc_List_List*
_temp2405= 0; int i= 1;{ struct Cyc_List_List* _temp2406= f->typs; for( 0;
_temp2406 != 0;( _temp2406=(( struct Cyc_List_List*) _check_null( _temp2406))->tl,
i ++)){ struct _tagged_arr* _temp2407= Cyc_Toc_fieldname( i); struct Cyc_Absyn_Structfield*
_temp2408=({ struct Cyc_Absyn_Structfield* _temp2410=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2410->name= _temp2407;
_temp2410->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2406))->hd)).f1; _temp2410->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2406))->hd)).f2);
_temp2410->width= 0; _temp2410->attributes= 0; _temp2410;}); _temp2405=({ struct
Cyc_List_List* _temp2409=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2409->hd=( void*) _temp2408; _temp2409->tl= _temp2405; _temp2409;});}}
_temp2405=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2405); _temp2405=({ struct Cyc_List_List* _temp2411=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2411->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2412=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2412->name= Cyc_Toc_tag_sp; _temp2412->tq= Cyc_Toc_mt_tq; _temp2412->type=(
void*) Cyc_Absyn_sint_t; _temp2412->width= 0; _temp2412->attributes= 0;
_temp2412;}); _temp2411->tl= _temp2405; _temp2411;});{ struct Cyc_Absyn_Structdecl*
_temp2413=({ struct Cyc_Absyn_Structdecl* _temp2417=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2417->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2417->name=({ struct Cyc_Core_Opt* _temp2419=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2419->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2419;}); _temp2417->tvs= 0; _temp2417->fields=({ struct Cyc_Core_Opt*
_temp2418=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2418->v=( void*) _temp2405; _temp2418;}); _temp2417->attributes= 0;
_temp2417;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2414=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2414->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2415=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2415[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2416; _temp2416.tag= Cyc_Absyn_Struct_d;
_temp2416.f1= _temp2413; _temp2416;}); _temp2415;}), 0); _temp2414->tl= Cyc_Toc_result_decls;
_temp2414;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2420=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2420->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2420;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2421= xd->name; struct Cyc_List_List*
_temp2422=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2422 != 0; _temp2422=(( struct Cyc_List_List*) _check_null(
_temp2422))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2422))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2423= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) + 4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2424=({ struct
Cyc_Absyn_ArrayType_struct* _temp2470=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2470[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2471; _temp2471.tag= Cyc_Absyn_ArrayType;
_temp2471.f1=( void*) Cyc_Absyn_uchar_t; _temp2471.f2= Cyc_Toc_mt_tq; _temp2471.f3=(
struct Cyc_Absyn_Exp*) _temp2423; _temp2471;}); _temp2470;}); struct Cyc_Core_Opt*
_temp2425=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2433; int _temp2434; _LL2427: if( _temp2425 == 0){ goto _LL2428;} else{
goto _LL2429;} _LL2429: if( _temp2425 == 0){ goto _LL2431;} else{ _temp2433=*
_temp2425; _LL2435: _temp2434=( int) _temp2433.v; if( _temp2434 == 0){ goto
_LL2430;} else{ goto _LL2431;}} _LL2431: goto _LL2432; _LL2428: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp(( struct _tagged_arr)({ int _temp2436= 0; int _temp2437= 0;
int _temp2438= 0; int _temp2439= 0; struct _tagged_arr _temp2440=* fn; xprintf("%c%c%c%c%.*s",
_temp2436, _temp2437, _temp2438, _temp2439, _get_arr_size( _temp2440, 1u),
_temp2440.curr);}), 0);}{ struct Cyc_Absyn_Vardecl* _temp2441= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2424, initopt);( void*)( _temp2441->sc=( void*)(( void*) f->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2442=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2442->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2443=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2443[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2444; _temp2444.tag= Cyc_Absyn_Var_d; _temp2444.f1= _temp2441; _temp2444;});
_temp2443;}), 0); _temp2442->tl= Cyc_Toc_result_decls; _temp2442;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*) Cyc_Absyn_Extern));
if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2445= f->typs; for( 0; _temp2445 != 0;( _temp2445=(( struct Cyc_List_List*)
_check_null( _temp2445))->tl, i ++)){ struct _tagged_arr* _temp2446=({ struct
_tagged_arr* _temp2450=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2450[ 0]=( struct _tagged_arr) xprintf("f%d", i); _temp2450;});
struct Cyc_Absyn_Structfield* _temp2447=({ struct Cyc_Absyn_Structfield*
_temp2449=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2449->name= _temp2446; _temp2449->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2445))->hd)).f1; _temp2449->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2445))->hd)).f2);
_temp2449->width= 0; _temp2449->attributes= 0; _temp2449;}); fields=({ struct
Cyc_List_List* _temp2448=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2448->hd=( void*) _temp2447; _temp2448->tl= fields; _temp2448;});}} fields=({
struct Cyc_List_List* _temp2451=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2451->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2452=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2452->name= Cyc_Toc_tag_sp; _temp2452->tq= Cyc_Toc_mt_tq; _temp2452->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2452->width=
0; _temp2452->attributes= 0; _temp2452;}); _temp2451->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2451;});{ struct Cyc_Absyn_Structdecl*
_temp2453=({ struct Cyc_Absyn_Structdecl* _temp2457=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2457->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2457->name=({ struct Cyc_Core_Opt* _temp2459=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2459->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2459;}); _temp2457->tvs= 0; _temp2457->fields=({ struct Cyc_Core_Opt*
_temp2458=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2458->v=( void*) fields; _temp2458;}); _temp2457->attributes= 0; _temp2457;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2454=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2454->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2455=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2455[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2456; _temp2456.tag= Cyc_Absyn_Struct_d;
_temp2456.f1= _temp2453; _temp2456;}); _temp2455;}), 0); _temp2454->tl= Cyc_Toc_result_decls;
_temp2454;});}} goto _LL2426;}} _LL2430: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2460= Cyc_Absyn_string_exp(( struct _tagged_arr)({
int _temp2465= 0; int _temp2466= 0; int _temp2467= 0; int _temp2468= 0; struct
_tagged_arr _temp2469=* fn; xprintf("%c%c%c%c%.*s", _temp2465, _temp2466,
_temp2467, _temp2468, _get_arr_size( _temp2469, 1u), _temp2469.curr);}), 0);
struct Cyc_Absyn_Vardecl* _temp2461= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2424,( struct Cyc_Absyn_Exp*) _temp2460);( void*)( _temp2461->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2462=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2462->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2463=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2463[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2464; _temp2464.tag= Cyc_Absyn_Var_d;
_temp2464.f1= _temp2461; _temp2464;}); _temp2463;}), 0); _temp2462->tl= Cyc_Toc_result_decls;
_temp2462;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2426;
_LL2432: goto _LL2426; _LL2426:;}}} static void Cyc_Toc_enumdecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2472= ed->name;(
void*)( ed->sc=( void*)(( void*) Cyc_Absyn_Public)); if( ed->fields != 0){
struct Cyc_List_List* _temp2473=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; _temp2473 != 0; _temp2473=(( struct Cyc_List_List*)
_check_null( _temp2473))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2473))->hd)->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2473))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2473))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer != 0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2474=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2480; struct Cyc_Absyn_Exp* _temp2482; struct Cyc_Absyn_Vardecl* _temp2484;
_LL2476: if(*(( int*) _temp2474) == Cyc_Absyn_Comprehension_e){ _LL2485:
_temp2484=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2474)->f1; goto
_LL2483; _LL2483: _temp2482=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2474)->f2; goto _LL2481; _LL2481: _temp2480=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2474)->f3; goto _LL2477;} else{ goto _LL2478;} _LL2478: goto _LL2479;
_LL2477: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2484, _temp2482, _temp2480, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2475; _LL2479: Cyc_Toc_exp_to_c( init_nv,
init); goto _LL2475; _LL2475:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2486= _new_region(); struct _RegionHandle* prgn=& _temp2486;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2489; struct Cyc_List_List*
_temp2491; struct Cyc_Toc_Env* _temp2493; struct _tuple12 _temp2487= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2494: _temp2493= _temp2487.f1; goto _LL2492; _LL2492: _temp2491=
_temp2487.f2; goto _LL2490; _LL2490: _temp2489= _temp2487.f3; goto _LL2488;
_LL2488: Cyc_Toc_stmt_to_c( _temp2493, s);{ struct Cyc_Absyn_Stmt* _temp2495=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2489, _temp2495, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2496= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2497= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2496, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2489,
Cyc_Absyn_seq_stmt( _temp2497, _temp2495, 0), 0), 0);} for( 0; _temp2491 != 0;
_temp2491=(( struct Cyc_List_List*) _check_null( _temp2491))->tl){ struct
_tuple13 _temp2500; void* _temp2501; struct _tuple0* _temp2503; struct _tuple13*
_temp2498=( struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp2491))->hd;
_temp2500=* _temp2498; _LL2504: _temp2503= _temp2500.f1; goto _LL2502; _LL2502:
_temp2501= _temp2500.f2; goto _LL2499; _LL2499: s= Cyc_Absyn_declare_stmt(
_temp2503, _temp2501, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u));}{ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2505=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2531; struct Cyc_Absyn_Fndecl* _temp2533; int
_temp2535; struct Cyc_Absyn_Exp* _temp2537; struct Cyc_Core_Opt* _temp2539;
struct Cyc_Core_Opt* _temp2541; struct Cyc_Absyn_Pat* _temp2543; struct Cyc_List_List*
_temp2545; struct Cyc_Absyn_Structdecl* _temp2547; struct Cyc_Absyn_Uniondecl*
_temp2549; struct Cyc_Absyn_Tuniondecl* _temp2551; struct Cyc_Absyn_Enumdecl*
_temp2553; struct Cyc_Absyn_Typedefdecl* _temp2555; struct Cyc_List_List*
_temp2557; struct _tagged_arr* _temp2559; struct Cyc_List_List* _temp2561;
struct _tuple0* _temp2563; struct Cyc_List_List* _temp2565; _LL2507: if(*(( int*)
_temp2505) == Cyc_Absyn_Var_d){ _LL2532: _temp2531=(( struct Cyc_Absyn_Var_d_struct*)
_temp2505)->f1; goto _LL2508;} else{ goto _LL2509;} _LL2509: if(*(( int*)
_temp2505) == Cyc_Absyn_Fn_d){ _LL2534: _temp2533=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2505)->f1; goto _LL2510;} else{ goto _LL2511;} _LL2511: if(*(( int*)
_temp2505) == Cyc_Absyn_Let_d){ _LL2544: _temp2543=(( struct Cyc_Absyn_Let_d_struct*)
_temp2505)->f1; goto _LL2542; _LL2542: _temp2541=(( struct Cyc_Absyn_Let_d_struct*)
_temp2505)->f2; goto _LL2540; _LL2540: _temp2539=(( struct Cyc_Absyn_Let_d_struct*)
_temp2505)->f3; goto _LL2538; _LL2538: _temp2537=(( struct Cyc_Absyn_Let_d_struct*)
_temp2505)->f4; goto _LL2536; _LL2536: _temp2535=(( struct Cyc_Absyn_Let_d_struct*)
_temp2505)->f5; goto _LL2512;} else{ goto _LL2513;} _LL2513: if(*(( int*)
_temp2505) == Cyc_Absyn_Letv_d){ _LL2546: _temp2545=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2505)->f1; goto _LL2514;} else{ goto _LL2515;} _LL2515: if(*(( int*)
_temp2505) == Cyc_Absyn_Struct_d){ _LL2548: _temp2547=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2505)->f1; goto _LL2516;} else{ goto _LL2517;} _LL2517: if(*(( int*)
_temp2505) == Cyc_Absyn_Union_d){ _LL2550: _temp2549=(( struct Cyc_Absyn_Union_d_struct*)
_temp2505)->f1; goto _LL2518;} else{ goto _LL2519;} _LL2519: if(*(( int*)
_temp2505) == Cyc_Absyn_Tunion_d){ _LL2552: _temp2551=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2505)->f1; goto _LL2520;} else{ goto _LL2521;} _LL2521: if(*(( int*)
_temp2505) == Cyc_Absyn_Enum_d){ _LL2554: _temp2553=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2505)->f1; goto _LL2522;} else{ goto _LL2523;} _LL2523: if(*(( int*)
_temp2505) == Cyc_Absyn_Typedef_d){ _LL2556: _temp2555=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2505)->f1; goto _LL2524;} else{ goto _LL2525;} _LL2525: if(*(( int*)
_temp2505) == Cyc_Absyn_Namespace_d){ _LL2560: _temp2559=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2505)->f1; goto _LL2558; _LL2558: _temp2557=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2505)->f2; goto _LL2526;} else{ goto _LL2527;} _LL2527: if(*(( int*)
_temp2505) == Cyc_Absyn_Using_d){ _LL2564: _temp2563=(( struct Cyc_Absyn_Using_d_struct*)
_temp2505)->f1; goto _LL2562; _LL2562: _temp2561=(( struct Cyc_Absyn_Using_d_struct*)
_temp2505)->f2; goto _LL2528;} else{ goto _LL2529;} _LL2529: if(*(( int*)
_temp2505) == Cyc_Absyn_ExternC_d){ _LL2566: _temp2565=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2505)->f1; goto _LL2530;} else{ goto _LL2506;} _LL2508: { struct _tuple0*
_temp2567= _temp2531->name; if(( void*) _temp2531->sc ==( void*) Cyc_Absyn_ExternC){
_temp2567=({ struct _tuple0* _temp2568=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2568->f1= Cyc_Toc_rel_ns; _temp2568->f2=(* _temp2567).f2;
_temp2568;});} if( _temp2531->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*) _check_null( _temp2531->initializer));} nv= Cyc_Toc_add_varmap(
nv, _temp2531->name, Cyc_Absyn_varb_exp( _temp2567,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2569=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2569[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2570; _temp2570.tag= Cyc_Absyn_Global_b;
_temp2570.f1= _temp2531; _temp2570;}); _temp2569;}), 0)); _temp2531->name=
_temp2567;( void*)( _temp2531->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2531->sc));(
void*)( _temp2531->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2531->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2571=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2571->hd=( void*) d; _temp2571->tl=
Cyc_Toc_result_decls; _temp2571;}); goto _LL2506;} _LL2510: { struct _tuple0*
_temp2572= _temp2533->name; if(( void*) _temp2533->sc ==( void*) Cyc_Absyn_ExternC){
_temp2572=({ struct _tuple0* _temp2573=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2573->f1= Cyc_Toc_rel_ns; _temp2573->f2=(* _temp2572).f2;
_temp2573;});} nv= Cyc_Toc_add_varmap( nv, _temp2533->name, Cyc_Absyn_var_exp(
_temp2572, 0)); _temp2533->name= _temp2572; Cyc_Toc_fndecl_to_c( nv, _temp2533);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2574=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2574->hd=( void*) d; _temp2574->tl=
Cyc_Toc_result_decls; _temp2574;}); goto _LL2506;} _LL2512: goto _LL2514;
_LL2514:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel",
sizeof( unsigned char), 20u)); goto _LL2506; _LL2516: Cyc_Toc_structdecl_to_c(
_temp2547); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2575=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2575->hd=( void*) d; _temp2575->tl=
Cyc_Toc_result_decls; _temp2575;}); goto _LL2506; _LL2518: Cyc_Toc_uniondecl_to_c(
_temp2549); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2576=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2576->hd=( void*) d; _temp2576->tl=
Cyc_Toc_result_decls; _temp2576;}); goto _LL2506; _LL2520: if( _temp2551->is_xtunion){
Cyc_Toc_xtuniondecl_to_c( _temp2551);} else{ Cyc_Toc_tuniondecl_to_c( _temp2551);}
goto _LL2506; _LL2522: Cyc_Toc_enumdecl_to_c( nv, _temp2553); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2577=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2577->hd=( void*) d; _temp2577->tl= Cyc_Toc_result_decls;
_temp2577;}); goto _LL2506; _LL2524: _temp2555->name= _temp2555->name; _temp2555->tvs=
0;( void*)( _temp2555->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2555->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2578=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2578->hd=( void*) d; _temp2578->tl=
Cyc_Toc_result_decls; _temp2578;}); goto _LL2506; _LL2526: _temp2561= _temp2557;
goto _LL2528; _LL2528: _temp2565= _temp2561; goto _LL2530; _LL2530: nv= Cyc_Toc_decls_to_c(
nv, _temp2565, top); goto _LL2506; _LL2506:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}