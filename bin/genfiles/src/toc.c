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
Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp( struct
Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp(
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
void*) e->r; void* _temp90; unsigned char _temp92; void* _temp94; short _temp96;
void* _temp98; int _temp100; void* _temp102; long long _temp104; void* _temp106;
struct Cyc_Absyn_Exp* _temp108; struct Cyc_List_List* _temp110; struct Cyc_List_List*
_temp112; struct Cyc_List_List* _temp114; struct Cyc_List_List* _temp116; struct
Cyc_List_List* _temp118; _LL66: if(*(( int*) _temp64) == Cyc_Absyn_Const_e){
_LL91: _temp90=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp90 > 1u?*(( int*) _temp90) == Cyc_Absyn_Char_c: 0){ _LL93:
_temp92=(( struct Cyc_Absyn_Char_c_struct*) _temp90)->f2; goto _LL67;} else{
goto _LL68;}} else{ goto _LL68;} _LL68: if(*(( int*) _temp64) == Cyc_Absyn_Const_e){
_LL95: _temp94=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp94 > 1u?*(( int*) _temp94) == Cyc_Absyn_Short_c: 0){ _LL97:
_temp96=(( struct Cyc_Absyn_Short_c_struct*) _temp94)->f2; goto _LL69;} else{
goto _LL70;}} else{ goto _LL70;} _LL70: if(*(( int*) _temp64) == Cyc_Absyn_Const_e){
_LL99: _temp98=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp98 > 1u?*(( int*) _temp98) == Cyc_Absyn_Int_c: 0){ _LL101:
_temp100=(( struct Cyc_Absyn_Int_c_struct*) _temp98)->f2; goto _LL71;} else{
goto _LL72;}} else{ goto _LL72;} _LL72: if(*(( int*) _temp64) == Cyc_Absyn_Const_e){
_LL103: _temp102=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp102 > 1u?*(( int*) _temp102) == Cyc_Absyn_LongLong_c: 0){
_LL105: _temp104=(( struct Cyc_Absyn_LongLong_c_struct*) _temp102)->f2; goto
_LL73;} else{ goto _LL74;}} else{ goto _LL74;} _LL74: if(*(( int*) _temp64) ==
Cyc_Absyn_Const_e){ _LL107: _temp106=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp64)->f1; if( _temp106 ==( void*) Cyc_Absyn_Null_c){ goto _LL75;} else{ goto
_LL76;}} else{ goto _LL76;} _LL76: if(*(( int*) _temp64) == Cyc_Absyn_Cast_e){
_LL109: _temp108=(( struct Cyc_Absyn_Cast_e_struct*) _temp64)->f2; goto _LL77;}
else{ goto _LL78;} _LL78: if(*(( int*) _temp64) == Cyc_Absyn_Tuple_e){ _LL111:
_temp110=(( struct Cyc_Absyn_Tuple_e_struct*) _temp64)->f1; goto _LL79;} else{
goto _LL80;} _LL80: if(*(( int*) _temp64) == Cyc_Absyn_Array_e){ _LL113:
_temp112=(( struct Cyc_Absyn_Array_e_struct*) _temp64)->f1; goto _LL81;} else{
goto _LL82;} _LL82: if(*(( int*) _temp64) == Cyc_Absyn_Struct_e){ _LL115:
_temp114=(( struct Cyc_Absyn_Struct_e_struct*) _temp64)->f3; goto _LL83;} else{
goto _LL84;} _LL84: if(*(( int*) _temp64) == Cyc_Absyn_CompoundLit_e){ _LL117:
_temp116=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp64)->f2; goto _LL85;}
else{ goto _LL86;} _LL86: if(*(( int*) _temp64) == Cyc_Absyn_UnresolvedMem_e){
_LL119: _temp118=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp64)->f2; goto
_LL87;} else{ goto _LL88;} _LL88: goto _LL89; _LL67: return _temp92 =='\000';
_LL69: return _temp96 == 0; _LL71: return _temp100 == 0; _LL73: return _temp104
== 0; _LL75: return 1; _LL77: return Cyc_Toc_is_zero( _temp108); _LL79: return((
int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp110); _LL81: _temp114= _temp112; goto _LL83; _LL83:
_temp116= _temp114; goto _LL85; _LL85: _temp118= _temp116; goto _LL87; _LL87:
for( 0; _temp118 != 0; _temp118=(( struct Cyc_List_List*) _check_null( _temp118))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp118))->hd)).f2)){ return 0;}} return 1; _LL89: return 0; _LL65:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp120= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp126; struct Cyc_Absyn_Conref* _temp128; _LL122:
if(( unsigned int) _temp120 > 4u?*(( int*) _temp120) == Cyc_Absyn_PointerType: 0){
_LL127: _temp126=(( struct Cyc_Absyn_PointerType_struct*) _temp120)->f1; _LL129:
_temp128= _temp126.nullable; goto _LL123;} else{ goto _LL124;} _LL124: goto
_LL125; _LL123: return(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp128); _LL125:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("is_nullable", sizeof( unsigned char), 12u)); return 0; _LL121:;}
static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x, struct
_tagged_arr tag){ return({ struct _tuple0* _temp130=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp130->f1=(* x).f1; _temp130->f2=({ struct
_tagged_arr* _temp131=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp131[ 0]=( struct _tagged_arr) Cyc_String_strconcat(*(* x).f2,
tag); _temp131;}); _temp130;});} struct _tuple5{ struct _tagged_arr* f1; struct
Cyc_List_List* f2; } ; static struct _tagged_arr* Cyc_Toc_add_tuple_type( struct
Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp132= Cyc_Toc_tuple_types; for(
0; _temp132 != 0; _temp132=(( struct Cyc_List_List*) _check_null( _temp132))->tl){
struct _tuple5 _temp135; struct Cyc_List_List* _temp136; struct _tagged_arr*
_temp138; struct _tuple5* _temp133=( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp132))->hd; _temp135=* _temp133; _LL139: _temp138= _temp135.f1;
goto _LL137; _LL137: _temp136= _temp135.f2; goto _LL134; _LL134: { struct Cyc_List_List*
_temp140= tqs0; for( 0; _temp140 != 0? _temp136 != 0: 0;( _temp140=(( struct Cyc_List_List*)
_check_null( _temp140))->tl, _temp136=(( struct Cyc_List_List*) _check_null(
_temp136))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp140))->hd)).f2,( void*)(( struct Cyc_List_List*) _check_null(
_temp136))->hd)){ break;}} if( _temp140 == 0? _temp136 == 0: 0){ return _temp138;}}}}{
struct _tagged_arr* x=({ struct _tagged_arr* _temp154=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp154[ 0]=( struct _tagged_arr)
xprintf("_tuple%d", Cyc_Toc_tuple_type_counter ++); _temp154;}); struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); struct Cyc_List_List* _temp141= 0;
struct Cyc_List_List* ts2= ts;{ int i= 1; for( 0; ts2 != 0;( ts2=(( struct Cyc_List_List*)
_check_null( ts2))->tl, i ++)){ _temp141=({ struct Cyc_List_List* _temp142=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp142->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp143=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp143->name= Cyc_Toc_fieldname(
i); _temp143->tq= Cyc_Toc_mt_tq; _temp143->type=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( ts2))->hd); _temp143->width= 0; _temp143->attributes= 0; _temp143;});
_temp142->tl= _temp141; _temp142;});}} _temp141=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp141);{ struct Cyc_Absyn_Structdecl*
_temp144=({ struct Cyc_Absyn_Structdecl* _temp150=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp150->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp150->name=({ struct Cyc_Core_Opt* _temp152=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp152->v=( void*)({ struct _tuple0*
_temp153=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp153->f1=
Cyc_Absyn_rel_ns_null; _temp153->f2= x; _temp153;}); _temp152;}); _temp150->tvs=
0; _temp150->fields=({ struct Cyc_Core_Opt* _temp151=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp151->v=( void*) _temp141;
_temp151;}); _temp150->attributes= 0; _temp150;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp145=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp145->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp146=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp146[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp147; _temp147.tag= Cyc_Absyn_Struct_d; _temp147.f1=
_temp144; _temp147;}); _temp146;}), 0); _temp145->tl= Cyc_Toc_result_decls;
_temp145;}); Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp148=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp148->hd=( void*)({ struct
_tuple5* _temp149=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp149->f1= x; _temp149->f2= ts; _temp149;}); _temp148->tl= Cyc_Toc_tuple_types;
_temp148;}); return x;}}} struct _tuple0* Cyc_Toc_temp_var(){ return({ struct
_tuple0* _temp155=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp155->f1=( void*) Cyc_Absyn_Loc_n; _temp155->f2=({ struct _tagged_arr*
_temp156=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp156[ 0]=( struct _tagged_arr) xprintf("_temp%d", Cyc_Toc_temp_var_counter
++); _temp156;}); _temp155;});} static struct _tagged_arr* Cyc_Toc_fresh_label(){
return({ struct _tagged_arr* _temp157=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)); _temp157[ 0]=( struct _tagged_arr) xprintf("_LL%d", Cyc_Toc_temp_var_counter
++); _temp157;});} static void* Cyc_Toc_typ_to_c( void* t); static struct
_tuple1* Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp160; struct Cyc_Absyn_Tqual
_temp162; struct Cyc_Core_Opt* _temp164; struct _tuple1 _temp158=* a; _LL165:
_temp164= _temp158.f1; goto _LL163; _LL163: _temp162= _temp158.f2; goto _LL161;
_LL161: _temp160= _temp158.f3; goto _LL159; _LL159: return({ struct _tuple1*
_temp166=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp166->f1=
_temp164; _temp166->f2= _temp162; _temp166->f3= Cyc_Toc_typ_to_c( _temp160);
_temp166;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp169; struct Cyc_Absyn_Tqual _temp171; struct _tuple3 _temp167=* x;
_LL172: _temp171= _temp167.f1; goto _LL170; _LL170: _temp169= _temp167.f2; goto
_LL168; _LL168: return({ struct _tuple3* _temp173=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp173->f1= _temp171; _temp173->f2= Cyc_Toc_typ_to_c(
_temp169); _temp173;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp174= t; struct Cyc_Absyn_Exp* _temp182; struct Cyc_Absyn_Tqual _temp184;
void* _temp186; struct Cyc_Core_Opt* _temp188; struct Cyc_Core_Opt _temp190;
void* _temp191; _LL176: if(( unsigned int) _temp174 > 4u?*(( int*) _temp174) ==
Cyc_Absyn_ArrayType: 0){ _LL187: _temp186=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp174)->f1; goto _LL185; _LL185: _temp184=(( struct Cyc_Absyn_ArrayType_struct*)
_temp174)->f2; goto _LL183; _LL183: _temp182=(( struct Cyc_Absyn_ArrayType_struct*)
_temp174)->f3; goto _LL177;} else{ goto _LL178;} _LL178: if(( unsigned int)
_temp174 > 4u?*(( int*) _temp174) == Cyc_Absyn_Evar: 0){ _LL189: _temp188=((
struct Cyc_Absyn_Evar_struct*) _temp174)->f2; if( _temp188 == 0){ goto _LL180;}
else{ _temp190=* _temp188; _LL192: _temp191=( void*) _temp190.v; goto _LL179;}}
else{ goto _LL180;} _LL180: goto _LL181; _LL177: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp193=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp193[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp194; _temp194.tag= Cyc_Absyn_ArrayType;
_temp194.f1=( void*) Cyc_Toc_typ_to_c_array( _temp186); _temp194.f2= _temp184;
_temp194.f3= _temp182; _temp194;}); _temp193;}); _LL179: return Cyc_Toc_typ_to_c_array(
_temp191); _LL181: return Cyc_Toc_typ_to_c( t); _LL175:;} static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c( struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield*
_temp195=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp195->name= f->name; _temp195->tq= f->tq; _temp195->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp195->width= f->width; _temp195->attributes= f->attributes;
_temp195;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp196= t; struct
Cyc_Core_Opt* _temp248; struct Cyc_Core_Opt* _temp250; struct Cyc_Core_Opt
_temp252; void* _temp253; struct Cyc_Absyn_TunionInfo _temp255; void* _temp257;
struct Cyc_Absyn_Tuniondecl* _temp259; struct Cyc_Absyn_TunionFieldInfo _temp261;
void* _temp263; struct Cyc_Absyn_Tunionfield* _temp265; struct Cyc_Absyn_Tuniondecl*
_temp267; struct Cyc_Absyn_PtrInfo _temp269; struct Cyc_Absyn_Conref* _temp271;
struct Cyc_Absyn_Tqual _temp273; void* _temp275; struct Cyc_Absyn_Exp* _temp277;
struct Cyc_Absyn_Tqual _temp279; void* _temp281; struct Cyc_Absyn_FnInfo
_temp283; struct Cyc_List_List* _temp285; struct Cyc_Absyn_VarargInfo* _temp287;
int _temp289; struct Cyc_List_List* _temp291; void* _temp293; struct Cyc_List_List*
_temp295; struct Cyc_List_List* _temp297; struct Cyc_List_List* _temp299; struct
_tuple0* _temp301; struct _tuple0* _temp303; struct _tuple0* _temp305; struct
Cyc_Core_Opt* _temp307; struct Cyc_List_List* _temp309; struct _tuple0* _temp311;
void* _temp313; _LL198: if( _temp196 ==( void*) Cyc_Absyn_VoidType){ goto _LL199;}
else{ goto _LL200;} _LL200: if(( unsigned int) _temp196 > 4u?*(( int*) _temp196)
== Cyc_Absyn_Evar: 0){ _LL249: _temp248=(( struct Cyc_Absyn_Evar_struct*)
_temp196)->f2; if( _temp248 == 0){ goto _LL201;} else{ goto _LL202;}} else{ goto
_LL202;} _LL202: if(( unsigned int) _temp196 > 4u?*(( int*) _temp196) == Cyc_Absyn_Evar:
0){ _LL251: _temp250=(( struct Cyc_Absyn_Evar_struct*) _temp196)->f2; if(
_temp250 == 0){ goto _LL204;} else{ _temp252=* _temp250; _LL254: _temp253=( void*)
_temp252.v; goto _LL203;}} else{ goto _LL204;} _LL204: if(( unsigned int)
_temp196 > 4u?*(( int*) _temp196) == Cyc_Absyn_VarType: 0){ goto _LL205;} else{
goto _LL206;} _LL206: if(( unsigned int) _temp196 > 4u?*(( int*) _temp196) ==
Cyc_Absyn_TunionType: 0){ _LL256: _temp255=(( struct Cyc_Absyn_TunionType_struct*)
_temp196)->f1; _LL258: _temp257=( void*) _temp255.tunion_info; if(*(( int*)
_temp257) == Cyc_Absyn_KnownTunion){ _LL260: _temp259=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp257)->f1; goto _LL207;} else{ goto _LL208;}} else{ goto _LL208;} _LL208:
if(( unsigned int) _temp196 > 4u?*(( int*) _temp196) == Cyc_Absyn_TunionType: 0){
goto _LL209;} else{ goto _LL210;} _LL210: if(( unsigned int) _temp196 > 4u?*((
int*) _temp196) == Cyc_Absyn_TunionFieldType: 0){ _LL262: _temp261=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp196)->f1; _LL264: _temp263=( void*) _temp261.field_info; if(*(( int*)
_temp263) == Cyc_Absyn_KnownTunionfield){ _LL268: _temp267=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp263)->f1; goto _LL266; _LL266: _temp265=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp263)->f2; goto _LL211;} else{ goto _LL212;}} else{ goto _LL212;} _LL212:
if(( unsigned int) _temp196 > 4u?*(( int*) _temp196) == Cyc_Absyn_TunionFieldType:
0){ goto _LL213;} else{ goto _LL214;} _LL214: if(( unsigned int) _temp196 > 4u?*((
int*) _temp196) == Cyc_Absyn_PointerType: 0){ _LL270: _temp269=(( struct Cyc_Absyn_PointerType_struct*)
_temp196)->f1; _LL276: _temp275=( void*) _temp269.elt_typ; goto _LL274; _LL274:
_temp273= _temp269.tq; goto _LL272; _LL272: _temp271= _temp269.bounds; goto
_LL215;} else{ goto _LL216;} _LL216: if(( unsigned int) _temp196 > 4u?*(( int*)
_temp196) == Cyc_Absyn_IntType: 0){ goto _LL217;} else{ goto _LL218;} _LL218:
if( _temp196 ==( void*) Cyc_Absyn_FloatType){ goto _LL219;} else{ goto _LL220;}
_LL220: if( _temp196 ==( void*) Cyc_Absyn_DoubleType){ goto _LL221;} else{ goto
_LL222;} _LL222: if(( unsigned int) _temp196 > 4u?*(( int*) _temp196) == Cyc_Absyn_ArrayType:
0){ _LL282: _temp281=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp196)->f1;
goto _LL280; _LL280: _temp279=(( struct Cyc_Absyn_ArrayType_struct*) _temp196)->f2;
goto _LL278; _LL278: _temp277=(( struct Cyc_Absyn_ArrayType_struct*) _temp196)->f3;
goto _LL223;} else{ goto _LL224;} _LL224: if(( unsigned int) _temp196 > 4u?*((
int*) _temp196) == Cyc_Absyn_FnType: 0){ _LL284: _temp283=(( struct Cyc_Absyn_FnType_struct*)
_temp196)->f1; _LL294: _temp293=( void*) _temp283.ret_typ; goto _LL292; _LL292:
_temp291= _temp283.args; goto _LL290; _LL290: _temp289= _temp283.c_varargs; goto
_LL288; _LL288: _temp287= _temp283.cyc_varargs; goto _LL286; _LL286: _temp285=
_temp283.attributes; goto _LL225;} else{ goto _LL226;} _LL226: if(( unsigned int)
_temp196 > 4u?*(( int*) _temp196) == Cyc_Absyn_TupleType: 0){ _LL296: _temp295=((
struct Cyc_Absyn_TupleType_struct*) _temp196)->f1; goto _LL227;} else{ goto
_LL228;} _LL228: if(( unsigned int) _temp196 > 4u?*(( int*) _temp196) == Cyc_Absyn_AnonStructType:
0){ _LL298: _temp297=(( struct Cyc_Absyn_AnonStructType_struct*) _temp196)->f1;
goto _LL229;} else{ goto _LL230;} _LL230: if(( unsigned int) _temp196 > 4u?*((
int*) _temp196) == Cyc_Absyn_AnonUnionType: 0){ _LL300: _temp299=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp196)->f1; goto _LL231;} else{ goto _LL232;} _LL232: if(( unsigned int)
_temp196 > 4u?*(( int*) _temp196) == Cyc_Absyn_UnionType: 0){ _LL302: _temp301=((
struct Cyc_Absyn_UnionType_struct*) _temp196)->f1; goto _LL233;} else{ goto
_LL234;} _LL234: if(( unsigned int) _temp196 > 4u?*(( int*) _temp196) == Cyc_Absyn_StructType:
0){ _LL304: _temp303=(( struct Cyc_Absyn_StructType_struct*) _temp196)->f1; goto
_LL235;} else{ goto _LL236;} _LL236: if(( unsigned int) _temp196 > 4u?*(( int*)
_temp196) == Cyc_Absyn_EnumType: 0){ _LL306: _temp305=(( struct Cyc_Absyn_EnumType_struct*)
_temp196)->f1; goto _LL237;} else{ goto _LL238;} _LL238: if(( unsigned int)
_temp196 > 4u?*(( int*) _temp196) == Cyc_Absyn_TypedefType: 0){ _LL312: _temp311=((
struct Cyc_Absyn_TypedefType_struct*) _temp196)->f1; goto _LL310; _LL310:
_temp309=(( struct Cyc_Absyn_TypedefType_struct*) _temp196)->f2; goto _LL308;
_LL308: _temp307=(( struct Cyc_Absyn_TypedefType_struct*) _temp196)->f3; goto
_LL239;} else{ goto _LL240;} _LL240: if(( unsigned int) _temp196 > 4u?*(( int*)
_temp196) == Cyc_Absyn_RgnHandleType: 0){ _LL314: _temp313=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp196)->f1; goto _LL241;} else{ goto _LL242;} _LL242: if( _temp196 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL243;} else{ goto _LL244;} _LL244: if(( unsigned int)
_temp196 > 4u?*(( int*) _temp196) == Cyc_Absyn_AccessEff: 0){ goto _LL245;}
else{ goto _LL246;} _LL246: if(( unsigned int) _temp196 > 4u?*(( int*) _temp196)
== Cyc_Absyn_JoinEff: 0){ goto _LL247;} else{ goto _LL197;} _LL199: return t;
_LL201: return Cyc_Absyn_sint_t; _LL203: return Cyc_Toc_typ_to_c( _temp253);
_LL205: return Cyc_Absyn_void_star_typ(); _LL207: return Cyc_Absyn_void_star_typ();
_LL209:( int) _throw( Cyc_Tcutil_impos( _tag_arr("unresolved TunionType",
sizeof( unsigned char), 22u))); _LL211: if( _temp265->typs == 0){ if( _temp267->is_xtunion){
return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);}
else{ return Cyc_Absyn_uint_t;}} else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp265->name, _tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL213:( int)
_throw( Cyc_Tcutil_impos( _tag_arr("unresolved TunionFieldType", sizeof(
unsigned char), 27u))); _LL215: _temp275= Cyc_Toc_typ_to_c_array( _temp275);{
void* _temp315=( void*)( Cyc_Absyn_compress_conref( _temp271))->v; void*
_temp321; _LL317: if(( unsigned int) _temp315 > 1u?*(( int*) _temp315) == Cyc_Absyn_Eq_constr:
0){ _LL322: _temp321=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp315)->f1;
if( _temp321 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL318;} else{ goto _LL319;}}
else{ goto _LL319;} _LL319: goto _LL320; _LL318: return Cyc_Toc_tagged_arr_typ;
_LL320: return Cyc_Absyn_star_typ( _temp275,( void*) Cyc_Absyn_HeapRgn, _temp273);
_LL316:;} _LL217: return t; _LL219: return t; _LL221: return t; _LL223: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp323=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp323[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp324; _temp324.tag= Cyc_Absyn_ArrayType; _temp324.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp281); _temp324.f2= _temp279; _temp324.f3=
_temp277; _temp324;}); _temp323;}); _LL225: { struct Cyc_List_List* _temp325= 0;
for( 0; _temp285 != 0; _temp285=(( struct Cyc_List_List*) _check_null( _temp285))->tl){
void* _temp326=( void*)(( struct Cyc_List_List*) _check_null( _temp285))->hd;
_LL328: if( _temp326 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL329;} else{
goto _LL330;} _LL330: if( _temp326 ==( void*) Cyc_Absyn_Const_att){ goto _LL331;}
else{ goto _LL332;} _LL332: goto _LL333; _LL329: goto _LL331; _LL331: continue;
_LL333: _temp325=({ struct Cyc_List_List* _temp334=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp334->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( _temp285))->hd); _temp334->tl= _temp325;
_temp334;}); goto _LL327; _LL327:;}{ struct Cyc_List_List* _temp335=(( struct
Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp291); if( _temp287 != 0){ void*
_temp336= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp287))->type,( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp287))->rgn, Cyc_Toc_mt_tq)); struct _tuple1* _temp337=({
struct _tuple1* _temp339=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp339->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp287))->name;
_temp339->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp287))->tq;
_temp339->f3= _temp336; _temp339;}); _temp335=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp335,({ struct Cyc_List_List* _temp338=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp338->hd=( void*) _temp337; _temp338->tl= 0;
_temp338;}));} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp340=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp340[ 0]=({ struct Cyc_Absyn_FnType_struct _temp341; _temp341.tag= Cyc_Absyn_FnType;
_temp341.f1=({ struct Cyc_Absyn_FnInfo _temp342; _temp342.tvars= 0; _temp342.effect=
0; _temp342.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp293); _temp342.args=
_temp335; _temp342.c_varargs= _temp289; _temp342.cyc_varargs= 0; _temp342.rgn_po=
0; _temp342.attributes= _temp325; _temp342;}); _temp341;}); _temp340;});}}
_LL227: _temp295=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp295);{
struct _tagged_arr* _temp343= Cyc_Toc_add_tuple_type( _temp295); return Cyc_Absyn_strct(
_temp343);} _LL229: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp344=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp344[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp345; _temp345.tag= Cyc_Absyn_AnonStructType; _temp345.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp297); _temp345;}); _temp344;});
_LL231: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp346=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp346[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp347; _temp347.tag=
Cyc_Absyn_AnonUnionType; _temp347.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp299); _temp347;}); _temp346;}); _LL233: if( _temp301 == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous union", sizeof(
unsigned char), 16u));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp301)); _LL235: if( _temp303 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous struct", sizeof(
unsigned char), 17u));} return Cyc_Absyn_strctq(( struct _tuple0*) _check_null(
_temp303)); _LL237: return t; _LL239: if( _temp307 == 0){ return Cyc_Toc_toc_impos(
_tag_arr("Toc::typ_to_c: unresolved TypedefType", sizeof( unsigned char), 38u));}
return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp348=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp348[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp349; _temp349.tag= Cyc_Absyn_TypedefType;
_temp349.f1= _temp311; _temp349.f2= 0; _temp349.f3=({ struct Cyc_Core_Opt*
_temp350=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp350->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp307))->v); _temp350;}); _temp349;}); _temp348;}); _LL241:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL243: return Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u)); _LL245: return Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u)); _LL247: return Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u)); _LL197:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp351= t; struct Cyc_Absyn_Tqual _temp357; void* _temp359; _LL353: if((
unsigned int) _temp351 > 4u?*(( int*) _temp351) == Cyc_Absyn_ArrayType: 0){
_LL360: _temp359=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp351)->f1;
goto _LL358; _LL358: _temp357=(( struct Cyc_Absyn_ArrayType_struct*) _temp351)->f2;
goto _LL354;} else{ goto _LL355;} _LL355: goto _LL356; _LL354: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp359,( void*) Cyc_Absyn_HeapRgn, _temp357), e, l);
_LL356: return Cyc_Absyn_cast_exp( t, e, l); _LL352:;} static int Cyc_Toc_atomic_typ(
void* t){ void* _temp361= Cyc_Tcutil_compress( t); void* _temp397; struct Cyc_Absyn_Structdecl**
_temp399; struct Cyc_List_List* _temp401; struct Cyc_List_List* _temp403; struct
Cyc_Absyn_TunionFieldInfo _temp405; void* _temp407; struct Cyc_Absyn_Tunionfield*
_temp409; struct Cyc_Absyn_Tuniondecl* _temp411; struct Cyc_List_List* _temp413;
_LL363: if( _temp361 ==( void*) Cyc_Absyn_VoidType){ goto _LL364;} else{ goto
_LL365;} _LL365: if(( unsigned int) _temp361 > 4u?*(( int*) _temp361) == Cyc_Absyn_VarType:
0){ goto _LL366;} else{ goto _LL367;} _LL367: if(( unsigned int) _temp361 > 4u?*((
int*) _temp361) == Cyc_Absyn_IntType: 0){ goto _LL368;} else{ goto _LL369;}
_LL369: if( _temp361 ==( void*) Cyc_Absyn_FloatType){ goto _LL370;} else{ goto
_LL371;} _LL371: if( _temp361 ==( void*) Cyc_Absyn_DoubleType){ goto _LL372;}
else{ goto _LL373;} _LL373: if(( unsigned int) _temp361 > 4u?*(( int*) _temp361)
== Cyc_Absyn_FnType: 0){ goto _LL374;} else{ goto _LL375;} _LL375: if((
unsigned int) _temp361 > 4u?*(( int*) _temp361) == Cyc_Absyn_ArrayType: 0){
_LL398: _temp397=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp361)->f1;
goto _LL376;} else{ goto _LL377;} _LL377: if(( unsigned int) _temp361 > 4u?*((
int*) _temp361) == Cyc_Absyn_StructType: 0){ _LL400: _temp399=(( struct Cyc_Absyn_StructType_struct*)
_temp361)->f3; goto _LL378;} else{ goto _LL379;} _LL379: if(( unsigned int)
_temp361 > 4u?*(( int*) _temp361) == Cyc_Absyn_AnonStructType: 0){ _LL402:
_temp401=(( struct Cyc_Absyn_AnonStructType_struct*) _temp361)->f1; goto _LL380;}
else{ goto _LL381;} _LL381: if(( unsigned int) _temp361 > 4u?*(( int*) _temp361)
== Cyc_Absyn_AnonUnionType: 0){ _LL404: _temp403=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp361)->f1; goto _LL382;} else{ goto _LL383;} _LL383: if(( unsigned int)
_temp361 > 4u?*(( int*) _temp361) == Cyc_Absyn_TunionFieldType: 0){ _LL406:
_temp405=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp361)->f1; _LL408:
_temp407=( void*) _temp405.field_info; if(*(( int*) _temp407) == Cyc_Absyn_KnownTunionfield){
_LL412: _temp411=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp407)->f1;
goto _LL410; _LL410: _temp409=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp407)->f2; goto _LL384;} else{ goto _LL385;}} else{ goto _LL385;} _LL385:
if(( unsigned int) _temp361 > 4u?*(( int*) _temp361) == Cyc_Absyn_TupleType: 0){
_LL414: _temp413=(( struct Cyc_Absyn_TupleType_struct*) _temp361)->f1; goto
_LL386;} else{ goto _LL387;} _LL387: if(( unsigned int) _temp361 > 4u?*(( int*)
_temp361) == Cyc_Absyn_TunionType: 0){ goto _LL388;} else{ goto _LL389;} _LL389:
if(( unsigned int) _temp361 > 4u?*(( int*) _temp361) == Cyc_Absyn_UnionType: 0){
goto _LL390;} else{ goto _LL391;} _LL391: if(( unsigned int) _temp361 > 4u?*((
int*) _temp361) == Cyc_Absyn_PointerType: 0){ goto _LL392;} else{ goto _LL393;}
_LL393: if(( unsigned int) _temp361 > 4u?*(( int*) _temp361) == Cyc_Absyn_RgnHandleType:
0){ goto _LL394;} else{ goto _LL395;} _LL395: goto _LL396; _LL364: return 1;
_LL366: return 0; _LL368: return 1; _LL370: return 1; _LL372: return 1; _LL374:
return 1; _LL376: return Cyc_Toc_atomic_typ( _temp397); _LL378: if( _temp399 ==
0? 1:(*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp399)))->fields == 0){
return 0;}{ struct Cyc_List_List* _temp415=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp399)))->fields))->v;
for( 0; _temp415 != 0; _temp415=(( struct Cyc_List_List*) _check_null( _temp415))->tl){
if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp415))->hd)->type)){ return 0;}}} return 1; _LL380: _temp403=
_temp401; goto _LL382; _LL382: for( 0; _temp403 != 0; _temp403=(( struct Cyc_List_List*)
_check_null( _temp403))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp403))->hd)->type)){ return 0;}} return
1; _LL384: _temp413= _temp409->typs; goto _LL386; _LL386: for( 0; _temp413 != 0;
_temp413=(( struct Cyc_List_List*) _check_null( _temp413))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp413))->hd)).f2)){
return 0;}} return 1; _LL388: return 0; _LL390: return 0; _LL392: return 0;
_LL394: return 0; _LL396:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp416= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _get_arr_size( _temp416, 1u), _temp416.curr);}));
return 0; _LL362:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp417=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp423; void* _temp425;
_LL419: if(( unsigned int) _temp417 > 4u?*(( int*) _temp417) == Cyc_Absyn_PointerType:
0){ _LL424: _temp423=(( struct Cyc_Absyn_PointerType_struct*) _temp417)->f1;
_LL426: _temp425=( void*) _temp423.elt_typ; goto _LL420;} else{ goto _LL421;}
_LL421: goto _LL422; _LL420: { void* _temp427= Cyc_Tcutil_compress( _temp425);
_LL429: if( _temp427 ==( void*) Cyc_Absyn_VoidType){ goto _LL430;} else{ goto
_LL431;} _LL431: goto _LL432; _LL430: return 1; _LL432: return 0; _LL428:;}
_LL422: return 0; _LL418:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_arr* f){ void* _temp433= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp445; struct Cyc_Absyn_Structdecl* _temp447; struct Cyc_List_List* _temp448;
_LL435: if(( unsigned int) _temp433 > 4u?*(( int*) _temp433) == Cyc_Absyn_StructType:
0){ _LL446: _temp445=(( struct Cyc_Absyn_StructType_struct*) _temp433)->f3; if(
_temp445 == 0){ goto _LL437;} else{ _temp447=* _temp445; goto _LL436;}} else{
goto _LL437;} _LL437: if(( unsigned int) _temp433 > 4u?*(( int*) _temp433) ==
Cyc_Absyn_AnonStructType: 0){ _LL449: _temp448=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp433)->f1; goto _LL438;} else{ goto _LL439;} _LL439: if(( unsigned int)
_temp433 > 4u?*(( int*) _temp433) == Cyc_Absyn_AnonUnionType: 0){ goto _LL440;}
else{ goto _LL441;} _LL441: if(( unsigned int) _temp433 > 4u?*(( int*) _temp433)
== Cyc_Absyn_UnionType: 0){ goto _LL442;} else{ goto _LL443;} _LL443: goto
_LL444; _LL436: if( _temp447->fields == 0){( int) _throw( Cyc_Toc_toc_impos(
_tag_arr("is_poly_field: struct missing fields", sizeof( unsigned char), 37u)));}
_temp448=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp447->fields))->v;
goto _LL438; _LL438: { struct Cyc_Absyn_Structfield* _temp450= Cyc_Absyn_lookup_field(
_temp448, f); if( _temp450 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp451=*(( struct Cyc_Absyn_Structfield*)
_check_null( _temp450))->name; xprintf("is_poly_field: bad field %.*s",
_get_arr_size( _temp451, 1u), _temp451.curr);}));} return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp450))->type);} _LL440:
return 0; _LL442: return 0; _LL444:( int) _throw( Cyc_Toc_toc_impos(( struct
_tagged_arr)({ struct _tagged_arr _temp452= Cyc_Absynpp_typ2string( t); xprintf("is_poly_field: bad type %.*s",
_get_arr_size( _temp452, 1u), _temp452.curr);}))); _LL434:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp453=( void*) e->r; struct _tagged_arr*
_temp461; struct Cyc_Absyn_Exp* _temp463; struct _tagged_arr* _temp465; struct
Cyc_Absyn_Exp* _temp467; _LL455: if(*(( int*) _temp453) == Cyc_Absyn_StructMember_e){
_LL464: _temp463=(( struct Cyc_Absyn_StructMember_e_struct*) _temp453)->f1; goto
_LL462; _LL462: _temp461=(( struct Cyc_Absyn_StructMember_e_struct*) _temp453)->f2;
goto _LL456;} else{ goto _LL457;} _LL457: if(*(( int*) _temp453) == Cyc_Absyn_StructArrow_e){
_LL468: _temp467=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp453)->f1; goto
_LL466; _LL466: _temp465=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp453)->f2;
goto _LL458;} else{ goto _LL459;} _LL459: goto _LL460; _LL456: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp463->topt))->v, _temp461);
_LL458: { void* _temp469= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp467->topt))->v); struct Cyc_Absyn_PtrInfo _temp475; void*
_temp477; _LL471: if(( unsigned int) _temp469 > 4u?*(( int*) _temp469) == Cyc_Absyn_PointerType:
0){ _LL476: _temp475=(( struct Cyc_Absyn_PointerType_struct*) _temp469)->f1;
_LL478: _temp477=( void*) _temp475.elt_typ; goto _LL472;} else{ goto _LL473;}
_LL473: goto _LL474; _LL472: return Cyc_Toc_is_poly_field( _temp477, _temp465);
_LL474:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({
struct _tagged_arr _temp479= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp467->topt))->v); xprintf("is_poly_project:  bad type %.*s",
_get_arr_size( _temp479, 1u), _temp479.curr);})); return 0; _LL470:;} _LL460:
return 0; _LL454:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp*
s){ return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_e,({ struct Cyc_List_List*
_temp480=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp480->hd=( void*) s; _temp480->tl= 0; _temp480;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc_GC_malloc_atomic_e,({ struct Cyc_List_List* _temp481=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp481->hd=( void*) s; _temp481->tl=
0; _temp481;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t,
struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic(
s);} return Cyc_Toc_malloc_ptr( s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({ struct Cyc_List_List*(* _temp482)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp485= rgn; struct Cyc_Absyn_Exp* _temp486= s; struct
Cyc_Absyn_Exp* _temp483[ 2u]={ _temp485, _temp486}; struct _tagged_arr _temp484={(
void*) _temp483,( void*) _temp483,( void*)( _temp483 + 2u)}; _temp482( _temp484);}),
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Toc__sfile_to_file_e,({ struct Cyc_List_List*
_temp487=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp487->hd=( void*) e; _temp487->tl= 0; _temp487;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({
struct Cyc_List_List* _temp488=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp488->hd=( void*) e; _temp488->tl= 0; _temp488;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_toplevel_tagged_arr( void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ int is_string= 0;{ void* _temp489=(
void*) e->r; void* _temp495; _LL491: if(*(( int*) _temp489) == Cyc_Absyn_Const_e){
_LL496: _temp495=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp489)->f1; if((
unsigned int) _temp495 > 1u?*(( int*) _temp495) == Cyc_Absyn_String_c: 0){ goto
_LL492;} else{ goto _LL493;}} else{ goto _LL493;} _LL493: goto _LL494; _LL492:
is_string= 1; goto _LL490; _LL494: goto _LL490; _LL490:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp500=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp500[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp501; _temp501.tag= Cyc_Absyn_ArrayType; _temp501.f1=(
void*) Cyc_Absyn_uchar_t; _temp501.f2= Cyc_Toc_mt_tq; _temp501.f3=( struct Cyc_Absyn_Exp*)
sz; _temp501;}); _temp500;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp497=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp497->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp498=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp498[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp499; _temp499.tag= Cyc_Absyn_Var_d;
_temp499.f1= vd; _temp499;}); _temp498;}), 0); _temp497->tl= Cyc_Toc_result_decls;
_temp497;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct Cyc_List_List*(* _temp502)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct _tuple4* _temp505=({ struct _tuple4*
_temp510=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp510->f1= 0;
_temp510->f2= xexp; _temp510;}); struct _tuple4* _temp506=({ struct _tuple4*
_temp509=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp509->f1= 0;
_temp509->f2= xexp; _temp509;}); struct _tuple4* _temp507=({ struct _tuple4*
_temp508=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp508->f1= 0;
_temp508->f2= xplussz; _temp508;}); struct _tuple4* _temp503[ 3u]={ _temp505,
_temp506, _temp507}; struct _tagged_arr _temp504={( void*) _temp503,( void*)
_temp503,( void*)( _temp503 + 3u)}; _temp502( _temp504);}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp511=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp511->break_lab= 0; _temp511->continue_lab= 0; _temp511->fallthru_info=
0; _temp511->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp511->toplevel= 1;
_temp511;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp512=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp512->break_lab= e->break_lab; _temp512->continue_lab=
e->continue_lab; _temp512->fallthru_info= e->fallthru_info; _temp512->varmap= e->varmap;
_temp512->toplevel= e->toplevel; _temp512;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp513=(* x).f1; _LL515:
if(( unsigned int) _temp513 > 1u?*(( int*) _temp513) == Cyc_Absyn_Rel_n: 0){
goto _LL516;} else{ goto _LL517;} _LL517: goto _LL518; _LL516:( int) _throw( Cyc_Toc_toc_impos((
struct _tagged_arr)({ struct _tagged_arr _temp519= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _get_arr_size( _temp519, 1u),
_temp519.curr);}))); _LL518: goto _LL514; _LL514:;}{ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap,
x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env*
e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2;
struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l, struct _tagged_arr*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp520= 0; for( 0; fallthru_binders != 0;
fallthru_binders=(( struct Cyc_List_List*) _check_null( fallthru_binders))->tl){
_temp520=({ struct Cyc_List_List* _temp521=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp521->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp521->tl=
_temp520; _temp521;});} _temp520=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp520);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp522=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp522->v=( void*) break_l; _temp522;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp523=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp523->v=( void*)({ struct _tuple6*
_temp524=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp524->f1=
fallthru_l; _temp524->f2= _temp520; _temp524->f3= next_case_env->varmap;
_temp524;}); _temp523;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp525=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp525->v=( void*)
break_l; _temp525;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_arr* next_l){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp526=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp526->v=( void*)({ struct _tuple6* _temp527=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp527->f1= next_l; _temp527->f2= 0;
_temp527->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp527;}); _temp526;});
return ans;} struct _tuple7{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tuple8{ struct _tuple0* f1; void* f2; struct Cyc_Absyn_Exp* f3; } ;
static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c( void* p, struct Cyc_Core_Opt*
aopt, struct Cyc_List_List* args, struct Cyc_List_List* typs){ if( args == 0){
return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("expecting a literal format string", sizeof( unsigned char), 34u));}{
struct _tagged_arr fmt_str;{ void* _temp528=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd)->r; void* _temp534; struct
_tagged_arr _temp536; _LL530: if(*(( int*) _temp528) == Cyc_Absyn_Const_e){
_LL535: _temp534=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp528)->f1; if((
unsigned int) _temp534 > 1u?*(( int*) _temp534) == Cyc_Absyn_String_c: 0){
_LL537: _temp536=(( struct Cyc_Absyn_String_c_struct*) _temp534)->f1; goto
_LL531;} else{ goto _LL532;}} else{ goto _LL532;} _LL532: goto _LL533; _LL531:
fmt_str= _temp536; goto _LL529; _LL533: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("expecting a literal format string",
sizeof( unsigned char), 34u)); _LL529:;}{ int len=( int) _get_arr_size( fmt_str,
sizeof( unsigned char)); int i= 0; for( i= 0; i < len; i ++){ if(*(( const
unsigned char*) _check_unknown_subscript( fmt_str, sizeof( unsigned char), i))
=='%'){ i ++; if( i < len?*(( const unsigned char*) _check_unknown_subscript(
fmt_str, sizeof( unsigned char), i)) =='s': 0){ break;}}} if( i == len){ if(
aopt != 0){ args=({ struct Cyc_List_List* _temp538=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp538->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp538->tl= args; _temp538;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp539= _new_region(); struct
_RegionHandle* r=& _temp539; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*)
_check_unknown_subscript( fmt_str, sizeof( unsigned char), i)); rev_fmt=({
struct Cyc_List_List* _temp540=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp540->hd=( void*)(( int) c); _temp540->tl=
rev_fmt; _temp540;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* _temp541=
Cyc_Formatstr_parse_conversionspecification( fmt_str, i + 1); if( _temp541 == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("bad format string",
sizeof( unsigned char), 18u));}{ struct _tuple7 _temp544; int _temp545;
unsigned char _temp547; struct Cyc_List_List* _temp549; struct Cyc_List_List*
_temp551; struct Cyc_List_List* _temp553; struct Cyc_List_List* _temp555; struct
_tuple7* _temp542=( struct _tuple7*)(( struct Cyc_Core_Opt*) _check_null(
_temp541))->v; _temp544=* _temp542; _LL556: _temp555= _temp544.f1; goto _LL554;
_LL554: _temp553= _temp544.f2; goto _LL552; _LL552: _temp551= _temp544.f3; goto
_LL550; _LL550: _temp549= _temp544.f4; goto _LL548; _LL548: _temp547= _temp544.f5;
goto _LL546; _LL546: _temp545= _temp544.f6; goto _LL543; _LL543: i= _temp545 - 1;
if( _temp547 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp557)( struct _RegionHandle*, struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_arr)) Cyc_List_rlist; struct _RegionHandle* _temp564= r; struct Cyc_List_List*
_temp560= _temp555; struct Cyc_List_List* _temp561= _temp553; struct Cyc_List_List*
_temp562= _temp551; struct Cyc_List_List* _temp563= _temp549; struct Cyc_List_List*
_temp558[ 4u]={ _temp560, _temp561, _temp562, _temp563}; struct _tagged_arr
_temp559={( void*) _temp558,( void*) _temp558,( void*)( _temp558 + 4u)};
_temp557( _temp564, _temp559);})), rev_fmt); rev_fmt=({ struct Cyc_List_List*
_temp565=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp565->hd=( void*)(( int) _temp547); _temp565->tl= rev_fmt; _temp565;});{
struct Cyc_List_List* _temp566= _temp553; struct Cyc_List_List _temp573; struct
Cyc_List_List* _temp574; int _temp576; _LL568: if( _temp566 == 0){ goto _LL570;}
else{ _temp573=* _temp566; _LL577: _temp576=( int) _temp573.hd; goto _LL575;
_LL575: _temp574= _temp573.tl; if( _temp574 == 0){ goto _LL572;} else{ goto
_LL570;}} _LL572: if( _temp576 =='*'){ goto _LL569;} else{ goto _LL570;} _LL570:
goto _LL571; _LL569: { struct _tuple0* _temp578= Cyc_Toc_temp_var(); rev_temps=({
struct Cyc_List_List* _temp579=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp579->hd=( void*)({ struct _tuple8* _temp580=(
struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8)); _temp580->f1=
_temp578; _temp580->f2=( void*)(( struct Cyc_List_List*) _check_null( typs))->hd;
_temp580->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd;
_temp580;}); _temp579->tl= rev_temps; _temp579;}); rev_result=({ struct Cyc_List_List*
_temp581=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp581->hd=( void*) Cyc_Absyn_var_exp( _temp578, 0); _temp581->tl= rev_result;
_temp581;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL567;} _LL571: goto
_LL567; _LL567:;}{ struct Cyc_List_List* _temp582= _temp551; struct Cyc_List_List
_temp589; struct Cyc_List_List* _temp590; struct Cyc_List_List _temp592; struct
Cyc_List_List* _temp593; int _temp595; int _temp597; _LL584: if( _temp582 == 0){
goto _LL586;} else{ _temp589=* _temp582; _LL598: _temp597=( int) _temp589.hd;
goto _LL591; _LL591: _temp590= _temp589.tl; if( _temp590 == 0){ goto _LL586;}
else{ _temp592=* _temp590; _LL596: _temp595=( int) _temp592.hd; goto _LL594;
_LL594: _temp593= _temp592.tl; if( _temp593 == 0){ goto _LL588;} else{ goto
_LL586;}}} _LL588: if( _temp597 =='.'? _temp595 =='*': 0){ goto _LL585;} else{
goto _LL586;} _LL586: goto _LL587; _LL585: { struct _tuple0* _temp599= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp600=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp600->hd=( void*)({
struct _tuple8* _temp601=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp601->f1= _temp599; _temp601->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp601->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp601;}); _temp600->tl= rev_temps; _temp600;});
rev_result=({ struct Cyc_List_List* _temp602=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp602->hd=( void*) Cyc_Absyn_var_exp(
_temp599, 0); _temp602->tl= rev_result; _temp602;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL583;} _LL587: goto _LL583; _LL583:;} if( _temp547 !='%'){ if( args == 0?
1: typs == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct
_tagged_arr)({ struct _tagged_arr _temp603= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_get_arr_size( _temp603, 1u), _temp603.curr);}));}{ struct _tuple0* _temp604=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp605=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp605->hd=( void*)({
struct _tuple8* _temp606=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp606->f1= _temp604; _temp606->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp606->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp606;}); _temp605->tl= rev_temps; _temp605;});
rev_result=({ struct Cyc_List_List* _temp607=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp607->hd=( void*) Cyc_Absyn_var_exp(
_temp604, 0); _temp607->tl= rev_result; _temp607;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp608=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp608->hd=( void*)(( int)'.');
_temp608->tl= rev_fmt; _temp608;}); rev_fmt=({ struct Cyc_List_List* _temp609=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp609->hd=( void*)(( int)'*'); _temp609->tl= rev_fmt; _temp609;}); rev_fmt=({
struct Cyc_List_List* _temp610=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp610->hd=( void*)(( int)'s'); _temp610->tl=
rev_fmt; _temp610;});{ struct _tuple0* _temp611= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp612= Cyc_Absyn_var_exp( _temp611, 0); rev_temps=({ struct Cyc_List_List*
_temp613=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp613->hd=( void*)({ struct _tuple8* _temp614=( struct _tuple8*)
_region_malloc( r, sizeof( struct _tuple8)); _temp614->f1= _temp611; _temp614->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp614->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp614;});
_temp613->tl= rev_temps; _temp613;});{ struct Cyc_Absyn_Exp* _temp615= Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp619)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp622= _temp612; struct Cyc_Absyn_Exp* _temp623= Cyc_Absyn_uint_exp( 1, 0);
struct Cyc_Absyn_Exp* _temp620[ 2u]={ _temp622, _temp623}; struct _tagged_arr
_temp621={( void*) _temp620,( void*) _temp620,( void*)( _temp620 + 2u)};
_temp619( _temp621);}), 0); struct Cyc_Absyn_Exp* _temp616= Cyc_Absyn_structmember_exp(
_temp612, Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List* _temp617=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp617->hd=( void*) _temp615; _temp617->tl= rev_result; _temp617;});
rev_result=({ struct Cyc_List_List* _temp618=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp618->hd=( void*)
_temp616; _temp618->tl= rev_result; _temp618;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}{
struct Cyc_List_List* _temp624=({ struct Cyc_List_List* _temp627=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp627->hd=( void*) Cyc_Absyn_string_exp((
struct _tagged_arr) Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_fmt)), 0); _temp627->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_result); _temp627;}); if( aopt != 0){
_temp624=({ struct Cyc_List_List* _temp625=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp625->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp625->tl= _temp624; _temp625;});}{
struct Cyc_Absyn_Exp* _temp626= Cyc_Absyn_primop_exp( p, _temp624, 0); s= Cyc_Absyn_exp_stmt(
_temp626, 0); for( 0; rev_temps != 0; rev_temps=(( struct Cyc_List_List*)
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
e){ return({ struct _tuple3* _temp628=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp628->f1= Cyc_Toc_mt_tq; _temp628->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp628;});} static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp629=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp629->f1= 0; _temp629->f2= e; _temp629;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp630= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp630): Cyc_Toc_malloc_ptr( _temp630), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp630), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp631=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp631 != 0; _temp631=((
struct Cyc_List_List*) _check_null( _temp631))->tl){ struct _tuple4 _temp634;
struct Cyc_Absyn_Exp* _temp635; struct Cyc_List_List* _temp637; struct _tuple4*
_temp632=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp631))->hd;
_temp634=* _temp632; _LL638: _temp637= _temp634.f1; goto _LL636; _LL636:
_temp635= _temp634.f2; goto _LL633; _LL633: { struct Cyc_Absyn_Exp* e_index; if(
_temp637 == 0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp637))->tl != 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u));}{ void* _temp639=( void*)(( struct Cyc_List_List*)
_check_null( _temp637))->hd; void* _temp640= _temp639; struct Cyc_Absyn_Exp*
_temp646; _LL642: if(*(( int*) _temp640) == Cyc_Absyn_ArrayElement){ _LL647:
_temp646=(( struct Cyc_Absyn_ArrayElement_struct*) _temp640)->f1; goto _LL643;}
else{ goto _LL644;} _LL644: if(*(( int*) _temp640) == Cyc_Absyn_FieldName){ goto
_LL645;} else{ goto _LL641;} _LL643: Cyc_Toc_exp_to_c( nv, _temp646); e_index=
_temp646; goto _LL641; _LL645: e_index=(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("field name designators in array",
sizeof( unsigned char), 32u)); goto _LL641; _LL641:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp648=( void*)
_temp635->r; struct Cyc_List_List* _temp658; struct Cyc_Absyn_Exp* _temp660;
struct Cyc_Absyn_Exp* _temp662; struct Cyc_Absyn_Vardecl* _temp664; struct Cyc_List_List*
_temp666; void* _temp668; _LL650: if(*(( int*) _temp648) == Cyc_Absyn_Array_e){
_LL659: _temp658=(( struct Cyc_Absyn_Array_e_struct*) _temp648)->f1; goto _LL651;}
else{ goto _LL652;} _LL652: if(*(( int*) _temp648) == Cyc_Absyn_Comprehension_e){
_LL665: _temp664=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp648)->f1;
goto _LL663; _LL663: _temp662=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp648)->f2; goto _LL661; _LL661: _temp660=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp648)->f3; goto _LL653;} else{ goto _LL654;} _LL654: if(*(( int*) _temp648)
== Cyc_Absyn_AnonStruct_e){ _LL669: _temp668=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp648)->f1; goto _LL667; _LL667: _temp666=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp648)->f2; goto _LL655;} else{ goto _LL656;} _LL656: goto _LL657; _LL651: s=
Cyc_Toc_init_array( nv, lval, _temp658, s); goto _LL649; _LL653: s= Cyc_Toc_init_comprehension(
nv, lval, _temp664, _temp662, _temp660, s, 0); goto _LL649; _LL655: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp668, _temp666, s); goto _LL649; _LL657: Cyc_Toc_exp_to_c( nv,
_temp635); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp635, 0), s, 0); goto _LL649; _LL649:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp670= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp670, Cyc_Absyn_varb_exp( _temp670,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp693=( struct Cyc_Absyn_Local_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp693[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp694; _temp694.tag= Cyc_Absyn_Local_b; _temp694.f1= vd; _temp694;});
_temp693;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp670, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp670, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp670, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp670, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp671=( void*) e2->r; struct Cyc_List_List* _temp681; struct Cyc_Absyn_Exp*
_temp683; struct Cyc_Absyn_Exp* _temp685; struct Cyc_Absyn_Vardecl* _temp687;
struct Cyc_List_List* _temp689; void* _temp691; _LL673: if(*(( int*) _temp671)
== Cyc_Absyn_Array_e){ _LL682: _temp681=(( struct Cyc_Absyn_Array_e_struct*)
_temp671)->f1; goto _LL674;} else{ goto _LL675;} _LL675: if(*(( int*) _temp671)
== Cyc_Absyn_Comprehension_e){ _LL688: _temp687=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp671)->f1; goto _LL686; _LL686: _temp685=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp671)->f2; goto _LL684; _LL684: _temp683=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp671)->f3; goto _LL676;} else{ goto _LL677;} _LL677: if(*(( int*) _temp671)
== Cyc_Absyn_AnonStruct_e){ _LL692: _temp691=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp671)->f1; goto _LL690; _LL690: _temp689=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp671)->f2; goto _LL678;} else{ goto _LL679;} _LL679: goto _LL680; _LL674:
body= Cyc_Toc_init_array( nv2, lval, _temp681, Cyc_Absyn_skip_stmt( 0)); goto
_LL672; _LL676: body= Cyc_Toc_init_comprehension( nv2, lval, _temp687, _temp685,
_temp683, Cyc_Absyn_skip_stmt( 0), 0); goto _LL672; _LL678: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp691, _temp689, Cyc_Absyn_skip_stmt( 0)); goto _LL672; _LL680: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL672; _LL672:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp670, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp695=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp695 != 0; _temp695=(( struct Cyc_List_List*)
_check_null( _temp695))->tl){ struct _tuple4 _temp698; struct Cyc_Absyn_Exp*
_temp699; struct Cyc_List_List* _temp701; struct _tuple4* _temp696=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp695))->hd; _temp698=*
_temp696; _LL702: _temp701= _temp698.f1; goto _LL700; _LL700: _temp699= _temp698.f2;
goto _LL697; _LL697: if( _temp701 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("empty designator list", sizeof( unsigned char), 22u));} if((( struct
Cyc_List_List*) _check_null( _temp701))->tl != 0){(( int(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u));}{ void* _temp703=( void*)(( struct Cyc_List_List*)
_check_null( _temp701))->hd; struct _tagged_arr* _temp709; _LL705: if(*(( int*)
_temp703) == Cyc_Absyn_FieldName){ _LL710: _temp709=(( struct Cyc_Absyn_FieldName_struct*)
_temp703)->f1; goto _LL706;} else{ goto _LL707;} _LL707: goto _LL708; _LL706: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp709, 0);{ void*
_temp711=( void*) _temp699->r; struct Cyc_List_List* _temp721; struct Cyc_Absyn_Exp*
_temp723; struct Cyc_Absyn_Exp* _temp725; struct Cyc_Absyn_Vardecl* _temp727;
struct Cyc_List_List* _temp729; void* _temp731; _LL713: if(*(( int*) _temp711)
== Cyc_Absyn_Array_e){ _LL722: _temp721=(( struct Cyc_Absyn_Array_e_struct*)
_temp711)->f1; goto _LL714;} else{ goto _LL715;} _LL715: if(*(( int*) _temp711)
== Cyc_Absyn_Comprehension_e){ _LL728: _temp727=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp711)->f1; goto _LL726; _LL726: _temp725=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp711)->f2; goto _LL724; _LL724: _temp723=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp711)->f3; goto _LL716;} else{ goto _LL717;} _LL717: if(*(( int*) _temp711)
== Cyc_Absyn_AnonStruct_e){ _LL732: _temp731=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp711)->f1; goto _LL730; _LL730: _temp729=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp711)->f2; goto _LL718;} else{ goto _LL719;} _LL719: goto _LL720; _LL714: s=
Cyc_Toc_init_array( nv, lval, _temp721, s); goto _LL712; _LL716: s= Cyc_Toc_init_comprehension(
nv, lval, _temp727, _temp725, _temp723, s, 0); goto _LL712; _LL718: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp731, _temp729, s); goto _LL712; _LL720: Cyc_Toc_exp_to_c( nv,
_temp699); if( Cyc_Toc_is_poly_field( struct_type, _temp709)){ _temp699= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp699, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp699, 0), 0), s, 0); goto _LL712; _LL712:;} goto
_LL704;} _LL708:( int) _throw( Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u))); _LL704:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp733= _new_region();
struct _RegionHandle* r=& _temp733; _push_region( r);{ struct Cyc_List_List*
_temp734=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp735= Cyc_Toc_add_tuple_type( _temp734); struct
_tuple0* _temp736= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp737= Cyc_Absyn_var_exp(
_temp736, 0); struct Cyc_Absyn_Stmt* _temp738= Cyc_Absyn_exp_stmt( _temp737, 0);
struct Cyc_Absyn_Exp*(* _temp739)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp740=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp740); for( 0; _temp740
!= 0;( _temp740=(( struct Cyc_List_List*) _check_null( _temp740))->tl, -- i)){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp740))->hd; struct Cyc_Absyn_Exp* lval= _temp739( _temp737, Cyc_Toc_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp741=( void*) e->r; struct Cyc_List_List*
_temp749; struct Cyc_Absyn_Exp* _temp751; struct Cyc_Absyn_Exp* _temp753; struct
Cyc_Absyn_Vardecl* _temp755; _LL743: if(*(( int*) _temp741) == Cyc_Absyn_Array_e){
_LL750: _temp749=(( struct Cyc_Absyn_Array_e_struct*) _temp741)->f1; goto _LL744;}
else{ goto _LL745;} _LL745: if(*(( int*) _temp741) == Cyc_Absyn_Comprehension_e){
_LL756: _temp755=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp741)->f1;
goto _LL754; _LL754: _temp753=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp741)->f2; goto _LL752; _LL752: _temp751=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp741)->f3; goto _LL746;} else{ goto _LL747;} _LL747: goto _LL748; _LL744:
_temp738= Cyc_Toc_init_array( nv, lval, _temp749, _temp738); goto _LL742; _LL746:
_temp738= Cyc_Toc_init_comprehension( nv, lval, _temp755, _temp753, _temp751,
_temp738, 0); goto _LL742; _LL748: Cyc_Toc_exp_to_c( nv, e); _temp738= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp739( _temp737, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp738, 0); goto _LL742; _LL742:;}}}{ struct Cyc_Absyn_Exp*
_temp757= Cyc_Toc_make_struct( nv, _temp736, Cyc_Absyn_strct( _temp735),
_temp738, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp757;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp758= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp759= Cyc_Absyn_var_exp( _temp758, 0); struct Cyc_Absyn_Stmt*
_temp760= Cyc_Absyn_exp_stmt( _temp759, 0); struct Cyc_Absyn_Exp*(* _temp761)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp762= _new_region(); struct _RegionHandle* r=& _temp762;
_push_region( r);{ struct Cyc_List_List* _temp763=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp763 != 0; _temp763=(( struct Cyc_List_List*) _check_null( _temp763))->tl){
struct _tuple4 _temp766; struct Cyc_Absyn_Exp* _temp767; struct Cyc_List_List*
_temp769; struct _tuple4* _temp764=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp763))->hd; _temp766=* _temp764; _LL770: _temp769= _temp766.f1;
goto _LL768; _LL768: _temp767= _temp766.f2; goto _LL765; _LL765: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp767->topt))->v): 0; if( _temp769 == 0){(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("empty designator list", sizeof( unsigned char), 22u));}
if((( struct Cyc_List_List*) _check_null( _temp769))->tl != 0){ struct _tuple0*
_temp771= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp772= Cyc_Absyn_var_exp(
_temp771, 0); for( 0; _temp769 != 0; _temp769=(( struct Cyc_List_List*)
_check_null( _temp769))->tl){ void* _temp773=( void*)(( struct Cyc_List_List*)
_check_null( _temp769))->hd; struct _tagged_arr* _temp779; _LL775: if(*(( int*)
_temp773) == Cyc_Absyn_FieldName){ _LL780: _temp779=(( struct Cyc_Absyn_FieldName_struct*)
_temp773)->f1; goto _LL776;} else{ goto _LL777;} _LL777: goto _LL778; _LL776:
if( Cyc_Toc_is_poly_field( struct_type, _temp779)){ _temp772= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp772, 0);} _temp760= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp761( _temp759, _temp779, 0), _temp772, 0), 0),
_temp760, 0); goto _LL774; _LL778:( int) _throw( Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u))); _LL774:;} Cyc_Toc_exp_to_c( nv, _temp767);
_temp760= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp772,
_temp767, 0), 0), _temp760, 0);} else{ void* _temp781=( void*)(( struct Cyc_List_List*)
_check_null( _temp769))->hd; struct _tagged_arr* _temp787; _LL783: if(*(( int*)
_temp781) == Cyc_Absyn_FieldName){ _LL788: _temp787=(( struct Cyc_Absyn_FieldName_struct*)
_temp781)->f1; goto _LL784;} else{ goto _LL785;} _LL785: goto _LL786; _LL784: {
struct Cyc_Absyn_Exp* lval= _temp761( _temp759, _temp787, 0);{ void* _temp789=(
void*) _temp767->r; struct Cyc_List_List* _temp799; struct Cyc_Absyn_Exp*
_temp801; struct Cyc_Absyn_Exp* _temp803; struct Cyc_Absyn_Vardecl* _temp805;
struct Cyc_List_List* _temp807; void* _temp809; _LL791: if(*(( int*) _temp789)
== Cyc_Absyn_Array_e){ _LL800: _temp799=(( struct Cyc_Absyn_Array_e_struct*)
_temp789)->f1; goto _LL792;} else{ goto _LL793;} _LL793: if(*(( int*) _temp789)
== Cyc_Absyn_Comprehension_e){ _LL806: _temp805=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp789)->f1; goto _LL804; _LL804: _temp803=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp789)->f2; goto _LL802; _LL802: _temp801=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp789)->f3; goto _LL794;} else{ goto _LL795;} _LL795: if(*(( int*) _temp789)
== Cyc_Absyn_AnonStruct_e){ _LL810: _temp809=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp789)->f1; goto _LL808; _LL808: _temp807=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp789)->f2; goto _LL796;} else{ goto _LL797;} _LL797: goto _LL798; _LL792:
_temp760= Cyc_Toc_init_array( nv, lval, _temp799, _temp760); goto _LL790; _LL794:
_temp760= Cyc_Toc_init_comprehension( nv, lval, _temp805, _temp803, _temp801,
_temp760, 0); goto _LL790; _LL796: _temp760= Cyc_Toc_init_anon_struct( nv, lval,
_temp809, _temp807, _temp760); goto _LL790; _LL798: Cyc_Toc_exp_to_c( nv,
_temp767); if( Cyc_Toc_is_poly_field( struct_type, _temp787)){ _temp767= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp767, 0);} _temp760= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp767, 0), 0), _temp760, 0); goto _LL790; _LL790:;}
goto _LL782;} _LL786:( int) _throw( Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u))); _LL782:;}}}{ struct Cyc_Absyn_Exp* _temp811= Cyc_Toc_make_struct(
nv, _temp758, Cyc_Absyn_strctq( tdn), _temp760, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp811;}; _pop_region( r);} struct _tuple9{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple9* pr){ return Cyc_Absyn_assignop_exp( el,(*
pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp812=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp812[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp813; _temp813.tag= Cyc_Absyn_Increment_e;
_temp813.f1= e1; _temp813.f2=( void*) incr; _temp813;}); _temp812;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp814=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp824; struct Cyc_Absyn_Exp*
_temp826; void* _temp828; struct _tagged_arr* _temp830; struct Cyc_Absyn_Exp*
_temp832; _LL816: if(*(( int*) _temp814) == Cyc_Absyn_StmtExp_e){ _LL825:
_temp824=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp814)->f1; goto _LL817;}
else{ goto _LL818;} _LL818: if(*(( int*) _temp814) == Cyc_Absyn_Cast_e){ _LL829:
_temp828=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp814)->f1; goto _LL827;
_LL827: _temp826=(( struct Cyc_Absyn_Cast_e_struct*) _temp814)->f2; goto _LL819;}
else{ goto _LL820;} _LL820: if(*(( int*) _temp814) == Cyc_Absyn_StructMember_e){
_LL833: _temp832=(( struct Cyc_Absyn_StructMember_e_struct*) _temp814)->f1; goto
_LL831; _LL831: _temp830=(( struct Cyc_Absyn_StructMember_e_struct*) _temp814)->f2;
goto _LL821;} else{ goto _LL822;} _LL822: goto _LL823; _LL817: Cyc_Toc_lvalue_assign_stmt(
_temp824, fs, f, f_env); goto _LL815; _LL819: Cyc_Toc_lvalue_assign( _temp826,
fs, f, f_env); goto _LL815; _LL821:( void*)( e1->r=( void*)(( void*) _temp832->r));
Cyc_Toc_lvalue_assign( e1,({ struct Cyc_List_List* _temp834=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp834->hd=( void*) _temp830;
_temp834->tl= fs; _temp834;}), f, f_env); goto _LL815; _LL823: if( ! Cyc_Absyn_is_lvalue(
e1)){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({
struct _tagged_arr _temp835= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_get_arr_size( _temp835, 1u), _temp835.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL815;} _LL815:;}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp836=( void*) s->r; struct Cyc_Absyn_Exp* _temp846; struct Cyc_Absyn_Stmt*
_temp848; struct Cyc_Absyn_Decl* _temp850; struct Cyc_Absyn_Stmt* _temp852;
_LL838: if(( unsigned int) _temp836 > 1u?*(( int*) _temp836) == Cyc_Absyn_Exp_s:
0){ _LL847: _temp846=(( struct Cyc_Absyn_Exp_s_struct*) _temp836)->f1; goto
_LL839;} else{ goto _LL840;} _LL840: if(( unsigned int) _temp836 > 1u?*(( int*)
_temp836) == Cyc_Absyn_Decl_s: 0){ _LL851: _temp850=(( struct Cyc_Absyn_Decl_s_struct*)
_temp836)->f1; goto _LL849; _LL849: _temp848=(( struct Cyc_Absyn_Decl_s_struct*)
_temp836)->f2; goto _LL841;} else{ goto _LL842;} _LL842: if(( unsigned int)
_temp836 > 1u?*(( int*) _temp836) == Cyc_Absyn_Seq_s: 0){ _LL853: _temp852=((
struct Cyc_Absyn_Seq_s_struct*) _temp836)->f2; goto _LL843;} else{ goto _LL844;}
_LL844: goto _LL845; _LL839: Cyc_Toc_lvalue_assign( _temp846, fs, f, f_env);
goto _LL837; _LL841: Cyc_Toc_lvalue_assign_stmt( _temp848, fs, f, f_env); goto
_LL837; _LL843: Cyc_Toc_lvalue_assign_stmt( _temp852, fs, f, f_env); goto _LL837;
_LL845:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({
struct _tagged_arr _temp854= Cyc_Absynpp_stmt2string( s); xprintf("lvalue_assign_stmt: %.*s",
_get_arr_size( _temp854, 1u), _temp854.curr);})); goto _LL837; _LL837:;} static
struct Cyc_List_List* Cyc_Toc_rmap_2c( struct _RegionHandle* r2, void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List* result;
struct Cyc_List_List* prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List*
_temp855=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp855->hd=( void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,
env); _temp855->tl= 0; _temp855;}); prev= result; for( x=(( struct Cyc_List_List*)
_check_null( x))->tl; x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){((
struct Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List* _temp856=(
struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp856->hd=( void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,
env); _temp856->tl= 0; _temp856;}); prev=(( struct Cyc_List_List*) _check_null(
prev))->tl;} return result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(*
f)( void*, void*), void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp857=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp857->f1= 0; _temp857->f2= e; _temp857;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){ void* _temp858=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp864; _LL860: if((
unsigned int) _temp858 > 4u?*(( int*) _temp858) == Cyc_Absyn_PointerType: 0){
_LL865: _temp864=(( struct Cyc_Absyn_PointerType_struct*) _temp858)->f1; goto
_LL861;} else{ goto _LL862;} _LL862: goto _LL863; _LL861: return _temp864;
_LL863:( int) _throw( Cyc_Tcutil_impos( _tag_arr("get_ptr_typ:  not a pointer!",
sizeof( unsigned char), 29u))); _LL859:;} struct _tuple10{ void* f1; void* f2; }
; struct _tuple11{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; }
; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){
void* _temp866=( void*) e->r; if( e->topt == 0){(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr _temp867= Cyc_Absynpp_exp2string(
e); xprintf("exp_to_c: no type for %.*s", _get_arr_size( _temp867, 1u), _temp867.curr);}));
return;}{ void* old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
void* _temp868= _temp866; void* _temp948; void* _temp950; struct _tuple0*
_temp952; struct _tuple0* _temp954; struct Cyc_List_List* _temp956; void*
_temp958; void* _temp960; struct Cyc_Absyn_Exp* _temp962; struct Cyc_Absyn_Exp*
_temp964; struct Cyc_Core_Opt* _temp966; struct Cyc_Absyn_Exp* _temp968; struct
Cyc_Absyn_Exp* _temp970; struct Cyc_Absyn_Exp* _temp972; struct Cyc_Absyn_Exp*
_temp974; struct Cyc_Absyn_Exp* _temp976; struct Cyc_Absyn_Exp* _temp978; struct
Cyc_List_List* _temp980; struct Cyc_Absyn_Exp* _temp982; struct Cyc_Absyn_VarargCallInfo*
_temp984; struct Cyc_List_List* _temp986; struct Cyc_Absyn_Exp* _temp988; struct
Cyc_Absyn_VarargCallInfo* _temp990; struct Cyc_Absyn_VarargCallInfo _temp992;
struct Cyc_Absyn_VarargInfo* _temp993; struct Cyc_List_List* _temp995; int
_temp997; struct Cyc_List_List* _temp999; struct Cyc_Absyn_Exp* _temp1001;
struct Cyc_Absyn_Exp* _temp1003; struct Cyc_Absyn_Exp* _temp1005; struct Cyc_List_List*
_temp1007; struct Cyc_Absyn_Exp* _temp1009; struct Cyc_Absyn_Exp* _temp1011;
void* _temp1013; void** _temp1015; struct Cyc_Absyn_Exp* _temp1016; struct Cyc_Absyn_Exp*
_temp1018; struct Cyc_Absyn_Exp* _temp1020; struct Cyc_Absyn_Exp* _temp1022;
void* _temp1024; struct _tagged_arr* _temp1026; void* _temp1028; struct Cyc_Absyn_Exp*
_temp1030; struct _tagged_arr* _temp1032; struct Cyc_Absyn_Exp* _temp1034;
struct _tagged_arr* _temp1036; struct Cyc_Absyn_Exp* _temp1038; struct Cyc_Absyn_Exp*
_temp1040; struct Cyc_Absyn_Exp* _temp1042; struct Cyc_List_List* _temp1044;
struct Cyc_List_List* _temp1046; struct Cyc_Absyn_Exp* _temp1048; struct Cyc_Absyn_Exp*
_temp1050; struct Cyc_Absyn_Vardecl* _temp1052; struct Cyc_Absyn_Structdecl*
_temp1054; struct Cyc_List_List* _temp1056; struct Cyc_Core_Opt* _temp1058;
struct _tuple0* _temp1060; struct Cyc_List_List* _temp1062; void* _temp1064;
struct Cyc_Absyn_Tunionfield* _temp1066; struct Cyc_Absyn_Tuniondecl* _temp1068;
struct Cyc_List_List* _temp1070; struct Cyc_Absyn_Tunionfield* _temp1072; struct
Cyc_Absyn_Tuniondecl* _temp1074; struct Cyc_List_List* _temp1076; void*
_temp1078; struct Cyc_Absyn_Exp* _temp1080; struct Cyc_Absyn_Stmt* _temp1082;
struct Cyc_Absyn_Fndecl* _temp1084; _LL870: if(*(( int*) _temp868) == Cyc_Absyn_Const_e){
_LL949: _temp948=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp868)->f1; if(
_temp948 ==( void*) Cyc_Absyn_Null_c){ goto _LL871;} else{ goto _LL872;}} else{
goto _LL872;} _LL872: if(*(( int*) _temp868) == Cyc_Absyn_Const_e){ goto _LL873;}
else{ goto _LL874;} _LL874: if(*(( int*) _temp868) == Cyc_Absyn_Var_e){ _LL953:
_temp952=(( struct Cyc_Absyn_Var_e_struct*) _temp868)->f1; goto _LL951; _LL951:
_temp950=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp868)->f2; goto _LL875;}
else{ goto _LL876;} _LL876: if(*(( int*) _temp868) == Cyc_Absyn_UnknownId_e){
_LL955: _temp954=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp868)->f1; goto
_LL877;} else{ goto _LL878;} _LL878: if(*(( int*) _temp868) == Cyc_Absyn_Primop_e){
_LL959: _temp958=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp868)->f1;
goto _LL957; _LL957: _temp956=(( struct Cyc_Absyn_Primop_e_struct*) _temp868)->f2;
goto _LL879;} else{ goto _LL880;} _LL880: if(*(( int*) _temp868) == Cyc_Absyn_Increment_e){
_LL963: _temp962=(( struct Cyc_Absyn_Increment_e_struct*) _temp868)->f1; goto
_LL961; _LL961: _temp960=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp868)->f2; goto _LL881;} else{ goto _LL882;} _LL882: if(*(( int*) _temp868)
== Cyc_Absyn_AssignOp_e){ _LL969: _temp968=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp868)->f1; goto _LL967; _LL967: _temp966=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp868)->f2; goto _LL965; _LL965: _temp964=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp868)->f3; goto _LL883;} else{ goto _LL884;} _LL884: if(*(( int*) _temp868)
== Cyc_Absyn_Conditional_e){ _LL975: _temp974=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp868)->f1; goto _LL973; _LL973: _temp972=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp868)->f2; goto _LL971; _LL971: _temp970=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp868)->f3; goto _LL885;} else{ goto _LL886;} _LL886: if(*(( int*) _temp868)
== Cyc_Absyn_SeqExp_e){ _LL979: _temp978=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp868)->f1; goto _LL977; _LL977: _temp976=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp868)->f2; goto _LL887;} else{ goto _LL888;} _LL888: if(*(( int*) _temp868)
== Cyc_Absyn_UnknownCall_e){ _LL983: _temp982=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp868)->f1; goto _LL981; _LL981: _temp980=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp868)->f2; goto _LL889;} else{ goto _LL890;} _LL890: if(*(( int*) _temp868)
== Cyc_Absyn_FnCall_e){ _LL989: _temp988=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp868)->f1; goto _LL987; _LL987: _temp986=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp868)->f2; goto _LL985; _LL985: _temp984=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp868)->f3; if( _temp984 == 0){ goto _LL891;} else{ goto _LL892;}} else{ goto
_LL892;} _LL892: if(*(( int*) _temp868) == Cyc_Absyn_FnCall_e){ _LL1002:
_temp1001=(( struct Cyc_Absyn_FnCall_e_struct*) _temp868)->f1; goto _LL1000;
_LL1000: _temp999=(( struct Cyc_Absyn_FnCall_e_struct*) _temp868)->f2; goto
_LL991; _LL991: _temp990=(( struct Cyc_Absyn_FnCall_e_struct*) _temp868)->f3;
if( _temp990 == 0){ goto _LL894;} else{ _temp992=* _temp990; _LL998: _temp997=
_temp992.num_varargs; goto _LL996; _LL996: _temp995= _temp992.injectors; goto
_LL994; _LL994: _temp993= _temp992.vai; goto _LL893;}} else{ goto _LL894;}
_LL894: if(*(( int*) _temp868) == Cyc_Absyn_Throw_e){ _LL1004: _temp1003=((
struct Cyc_Absyn_Throw_e_struct*) _temp868)->f1; goto _LL895;} else{ goto _LL896;}
_LL896: if(*(( int*) _temp868) == Cyc_Absyn_NoInstantiate_e){ _LL1006: _temp1005=((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp868)->f1; goto _LL897;} else{
goto _LL898;} _LL898: if(*(( int*) _temp868) == Cyc_Absyn_Instantiate_e){
_LL1010: _temp1009=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp868)->f1;
goto _LL1008; _LL1008: _temp1007=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp868)->f2; goto _LL899;} else{ goto _LL900;} _LL900: if(*(( int*) _temp868)
== Cyc_Absyn_Cast_e){ _LL1014: _temp1013=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp868)->f1; _temp1015=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp868)->f1);
goto _LL1012; _LL1012: _temp1011=(( struct Cyc_Absyn_Cast_e_struct*) _temp868)->f2;
goto _LL901;} else{ goto _LL902;} _LL902: if(*(( int*) _temp868) == Cyc_Absyn_Address_e){
_LL1017: _temp1016=(( struct Cyc_Absyn_Address_e_struct*) _temp868)->f1; goto
_LL903;} else{ goto _LL904;} _LL904: if(*(( int*) _temp868) == Cyc_Absyn_New_e){
_LL1021: _temp1020=(( struct Cyc_Absyn_New_e_struct*) _temp868)->f1; goto
_LL1019; _LL1019: _temp1018=(( struct Cyc_Absyn_New_e_struct*) _temp868)->f2;
goto _LL905;} else{ goto _LL906;} _LL906: if(*(( int*) _temp868) == Cyc_Absyn_Sizeofexp_e){
_LL1023: _temp1022=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp868)->f1; goto
_LL907;} else{ goto _LL908;} _LL908: if(*(( int*) _temp868) == Cyc_Absyn_Sizeoftyp_e){
_LL1025: _temp1024=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp868)->f1;
goto _LL909;} else{ goto _LL910;} _LL910: if(*(( int*) _temp868) == Cyc_Absyn_Offsetof_e){
_LL1029: _temp1028=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp868)->f1;
goto _LL1027; _LL1027: _temp1026=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp868)->f2; goto _LL911;} else{ goto _LL912;} _LL912: if(*(( int*) _temp868)
== Cyc_Absyn_Deref_e){ _LL1031: _temp1030=(( struct Cyc_Absyn_Deref_e_struct*)
_temp868)->f1; goto _LL913;} else{ goto _LL914;} _LL914: if(*(( int*) _temp868)
== Cyc_Absyn_StructMember_e){ _LL1035: _temp1034=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp868)->f1; goto _LL1033; _LL1033: _temp1032=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp868)->f2; goto _LL915;} else{ goto _LL916;} _LL916: if(*(( int*) _temp868)
== Cyc_Absyn_StructArrow_e){ _LL1039: _temp1038=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp868)->f1; goto _LL1037; _LL1037: _temp1036=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp868)->f2; goto _LL917;} else{ goto _LL918;} _LL918: if(*(( int*) _temp868)
== Cyc_Absyn_Subscript_e){ _LL1043: _temp1042=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp868)->f1; goto _LL1041; _LL1041: _temp1040=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp868)->f2; goto _LL919;} else{ goto _LL920;} _LL920: if(*(( int*) _temp868)
== Cyc_Absyn_Tuple_e){ _LL1045: _temp1044=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp868)->f1; goto _LL921;} else{ goto _LL922;} _LL922: if(*(( int*) _temp868)
== Cyc_Absyn_Array_e){ _LL1047: _temp1046=(( struct Cyc_Absyn_Array_e_struct*)
_temp868)->f1; goto _LL923;} else{ goto _LL924;} _LL924: if(*(( int*) _temp868)
== Cyc_Absyn_Comprehension_e){ _LL1053: _temp1052=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp868)->f1; goto _LL1051; _LL1051: _temp1050=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp868)->f2; goto _LL1049; _LL1049: _temp1048=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp868)->f3; goto _LL925;} else{ goto _LL926;} _LL926: if(*(( int*) _temp868)
== Cyc_Absyn_Struct_e){ _LL1061: _temp1060=(( struct Cyc_Absyn_Struct_e_struct*)
_temp868)->f1; goto _LL1059; _LL1059: _temp1058=(( struct Cyc_Absyn_Struct_e_struct*)
_temp868)->f2; goto _LL1057; _LL1057: _temp1056=(( struct Cyc_Absyn_Struct_e_struct*)
_temp868)->f3; goto _LL1055; _LL1055: _temp1054=(( struct Cyc_Absyn_Struct_e_struct*)
_temp868)->f4; goto _LL927;} else{ goto _LL928;} _LL928: if(*(( int*) _temp868)
== Cyc_Absyn_AnonStruct_e){ _LL1065: _temp1064=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp868)->f1; goto _LL1063; _LL1063: _temp1062=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp868)->f2; goto _LL929;} else{ goto _LL930;} _LL930: if(*(( int*) _temp868)
== Cyc_Absyn_Tunion_e){ _LL1071: _temp1070=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp868)->f3; if( _temp1070 == 0){ goto _LL1069;} else{ goto _LL932;} _LL1069:
_temp1068=(( struct Cyc_Absyn_Tunion_e_struct*) _temp868)->f4; goto _LL1067;
_LL1067: _temp1066=(( struct Cyc_Absyn_Tunion_e_struct*) _temp868)->f5; goto
_LL931;} else{ goto _LL932;} _LL932: if(*(( int*) _temp868) == Cyc_Absyn_Tunion_e){
_LL1077: _temp1076=(( struct Cyc_Absyn_Tunion_e_struct*) _temp868)->f3; goto
_LL1075; _LL1075: _temp1074=(( struct Cyc_Absyn_Tunion_e_struct*) _temp868)->f4;
goto _LL1073; _LL1073: _temp1072=(( struct Cyc_Absyn_Tunion_e_struct*) _temp868)->f5;
goto _LL933;} else{ goto _LL934;} _LL934: if(*(( int*) _temp868) == Cyc_Absyn_Enum_e){
goto _LL935;} else{ goto _LL936;} _LL936: if(*(( int*) _temp868) == Cyc_Absyn_Malloc_e){
_LL1081: _temp1080=(( struct Cyc_Absyn_Malloc_e_struct*) _temp868)->f1; goto
_LL1079; _LL1079: _temp1078=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp868)->f2; goto _LL937;} else{ goto _LL938;} _LL938: if(*(( int*) _temp868)
== Cyc_Absyn_StmtExp_e){ _LL1083: _temp1082=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp868)->f1; goto _LL939;} else{ goto _LL940;} _LL940: if(*(( int*) _temp868)
== Cyc_Absyn_UnresolvedMem_e){ goto _LL941;} else{ goto _LL942;} _LL942: if(*((
int*) _temp868) == Cyc_Absyn_CompoundLit_e){ goto _LL943;} else{ goto _LL944;}
_LL944: if(*(( int*) _temp868) == Cyc_Absyn_Codegen_e){ _LL1085: _temp1084=((
struct Cyc_Absyn_Codegen_e_struct*) _temp868)->f1; goto _LL945;} else{ goto
_LL946;} _LL946: if(*(( int*) _temp868) == Cyc_Absyn_Fill_e){ goto _LL947;}
else{ goto _LL869;} _LL871: { struct Cyc_Absyn_Exp* _temp1086= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1086,
_temp1086))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1087)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1090= _temp1086; struct Cyc_Absyn_Exp* _temp1091= _temp1086; struct Cyc_Absyn_Exp*
_temp1092= _temp1086; struct Cyc_Absyn_Exp* _temp1088[ 3u]={ _temp1090,
_temp1091, _temp1092}; struct _tagged_arr _temp1089={( void*) _temp1088,( void*)
_temp1088,( void*)( _temp1088 + 3u)}; _temp1087( _temp1089);}), 0))->r));}}
else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r));}
goto _LL869;} _LL873: goto _LL869; _LL875:{ struct _handler_cons _temp1093;
_push_handler(& _temp1093);{ int _temp1095= 0; if( setjmp( _temp1093.handler)){
_temp1095= 1;} if( ! _temp1095){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp952))->r));; _pop_handler();} else{ void* _temp1094=( void*) _exn_thrown;
void* _temp1097= _temp1094; _LL1099: if( _temp1097 == Cyc_Dict_Absent){ goto
_LL1100;} else{ goto _LL1101;} _LL1101: goto _LL1102; _LL1100:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp1103= Cyc_Absynpp_qvar2string( _temp952); xprintf("Can't find %.*s in exp_to_c, Var\n",
_get_arr_size( _temp1103, 1u), _temp1103.curr);})); return; _LL1102:( void)
_throw( _temp1097); _LL1098:;}}} goto _LL869; _LL877:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char),
10u)); goto _LL869; _LL879: { struct Cyc_List_List* _temp1104=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp956); struct Cyc_List_List* _temp1105=(( struct Cyc_List_List*(*)( void*(*
f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp956);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp956);{ void* _temp1106= _temp958; _LL1108: if( _temp1106 ==( void*) Cyc_Absyn_Size){
goto _LL1109;} else{ goto _LL1110;} _LL1110: if( _temp1106 ==( void*) Cyc_Absyn_Printf){
goto _LL1111;} else{ goto _LL1112;} _LL1112: if( _temp1106 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1113;} else{ goto _LL1114;} _LL1114: if( _temp1106 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1115;} else{ goto _LL1116;} _LL1116: if( _temp1106 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1117;} else{ goto _LL1118;} _LL1118: if( _temp1106 ==( void*) Cyc_Absyn_Plus){
goto _LL1119;} else{ goto _LL1120;} _LL1120: if( _temp1106 ==( void*) Cyc_Absyn_Minus){
goto _LL1121;} else{ goto _LL1122;} _LL1122: if( _temp1106 ==( void*) Cyc_Absyn_Eq){
goto _LL1123;} else{ goto _LL1124;} _LL1124: if( _temp1106 ==( void*) Cyc_Absyn_Neq){
goto _LL1125;} else{ goto _LL1126;} _LL1126: if( _temp1106 ==( void*) Cyc_Absyn_Gt){
goto _LL1127;} else{ goto _LL1128;} _LL1128: if( _temp1106 ==( void*) Cyc_Absyn_Gte){
goto _LL1129;} else{ goto _LL1130;} _LL1130: if( _temp1106 ==( void*) Cyc_Absyn_Lt){
goto _LL1131;} else{ goto _LL1132;} _LL1132: if( _temp1106 ==( void*) Cyc_Absyn_Lte){
goto _LL1133;} else{ goto _LL1134;} _LL1134: goto _LL1135; _LL1109: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp956))->hd;{
void* _temp1136=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1144; struct Cyc_Absyn_PtrInfo _temp1146; struct Cyc_Absyn_Conref*
_temp1148; void* _temp1150; _LL1138: if(( unsigned int) _temp1136 > 4u?*(( int*)
_temp1136) == Cyc_Absyn_ArrayType: 0){ _LL1145: _temp1144=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1136)->f3; goto _LL1139;} else{ goto _LL1140;} _LL1140: if(( unsigned int)
_temp1136 > 4u?*(( int*) _temp1136) == Cyc_Absyn_PointerType: 0){ _LL1147:
_temp1146=(( struct Cyc_Absyn_PointerType_struct*) _temp1136)->f1; _LL1151:
_temp1150=( void*) _temp1146.elt_typ; goto _LL1149; _LL1149: _temp1148=
_temp1146.bounds; goto _LL1141;} else{ goto _LL1142;} _LL1142: goto _LL1143;
_LL1139:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*) _check_null(
_temp1144))->r)); goto _LL1137; _LL1141:{ void* _temp1152= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1148); struct Cyc_Absyn_Exp* _temp1158; _LL1154: if(
_temp1152 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1155;} else{ goto _LL1156;}
_LL1156: if(( unsigned int) _temp1152 > 1u?*(( int*) _temp1152) == Cyc_Absyn_Upper_b:
0){ _LL1159: _temp1158=(( struct Cyc_Absyn_Upper_b_struct*) _temp1152)->f1; goto
_LL1157;} else{ goto _LL1153;} _LL1155:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp1160)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1163=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp956))->hd; struct Cyc_Absyn_Exp* _temp1164= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( _temp1150), 0); struct Cyc_Absyn_Exp* _temp1161[ 2u]={
_temp1163, _temp1164}; struct _tagged_arr _temp1162={( void*) _temp1161,( void*)
_temp1161,( void*)( _temp1161 + 2u)}; _temp1160( _temp1162);}), 0))->r)); goto
_LL1153; _LL1157:( void*)( e->r=( void*)(( void*) _temp1158->r)); goto _LL1153;
_LL1153:;} goto _LL1137; _LL1143:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1165= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_List_List*) _check_null( _temp1105))->hd); struct
_tagged_arr _temp1166= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_get_arr_size( _temp1165, 1u), _temp1165.curr, _get_arr_size( _temp1166, 1u),
_temp1166.curr);})); return; _LL1137:;} goto _LL1107;} _LL1111:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp958, 0, _temp956, _temp1105))->r));
goto _LL1107; _LL1113:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp958, 0, _temp956, _temp1105))->r)); goto _LL1107; _LL1115: if( _temp956 ==
0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("fprintf without arguments",
sizeof( unsigned char), 26u));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp958,({ struct Cyc_Core_Opt* _temp1167=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1167->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp956))->hd); _temp1167;}),(( struct Cyc_List_List*)
_check_null( _temp956))->tl,(( struct Cyc_List_List*) _check_null( _temp1105))->tl))->r));
goto _LL1107; _LL1117: if( _temp956 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("sscanf without arguments", sizeof( unsigned char), 25u));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp956))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp956))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp956))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1107; _LL1119: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1104))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp956))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp956))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1168)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1171= e1; struct Cyc_Absyn_Exp* _temp1172= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1173= e2; struct Cyc_Absyn_Exp*
_temp1169[ 3u]={ _temp1171, _temp1172, _temp1173}; struct _tagged_arr _temp1170={(
void*) _temp1169,( void*) _temp1169,( void*)( _temp1169 + 3u)}; _temp1168(
_temp1170);}), 0))->r));} goto _LL1107;} _LL1121: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1104))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp956))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp956))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1104))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)( Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Div, Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0), 0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1174)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1177= e1; struct Cyc_Absyn_Exp* _temp1178= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1179= Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Minus, e2, 0); struct Cyc_Absyn_Exp* _temp1175[ 3u]={ _temp1177,
_temp1178, _temp1179}; struct _tagged_arr _temp1176={( void*) _temp1175,( void*)
_temp1175,( void*)( _temp1175 + 3u)}; _temp1174( _temp1176);}), 0))->r));}} goto
_LL1107;} _LL1123: goto _LL1125; _LL1125: goto _LL1127; _LL1127: goto _LL1129;
_LL1129: goto _LL1131; _LL1131: goto _LL1133; _LL1133: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp956))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp956))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1104))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1104))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1107;} _LL1135: goto
_LL1107; _LL1107:;} goto _LL869;} _LL881: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp962->topt))->v; Cyc_Toc_exp_to_c( nv, _temp962);{
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1180=( _temp960 ==( void*) Cyc_Absyn_PostInc?
1: _temp960 ==( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1181=( _temp960 ==( void*) Cyc_Absyn_PreInc?
1: _temp960 ==( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( _temp1180,({ struct Cyc_List_List*(* _temp1182)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1185= Cyc_Absyn_address_exp( _temp962, 0); struct Cyc_Absyn_Exp*
_temp1186= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1187= Cyc_Absyn_signed_int_exp( _temp1181, 0); struct Cyc_Absyn_Exp*
_temp1183[ 3u]={ _temp1185, _temp1186, _temp1187}; struct _tagged_arr _temp1184={(
void*) _temp1183,( void*) _temp1183,( void*)( _temp1183 + 3u)}; _temp1182(
_temp1184);}), 0))->r));} if( elt_typ ==( void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue(
_temp962): 0){ Cyc_Toc_lvalue_assign( _temp962, 0, Cyc_Toc_incr_lvalue, _temp960);(
void*)( e->r=( void*)(( void*) _temp962->r));} goto _LL869;}} _LL883: { int
e1_poly= Cyc_Toc_is_poly_project( _temp968); void* e1_old_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp968->topt))->v; void* e2_old_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp964->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp968); Cyc_Toc_exp_to_c( nv, _temp964);{ int done= 0; if( _temp966 != 0){
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* change;{ void* _temp1188=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp966))->v; _LL1190: if( _temp1188 ==(
void*) Cyc_Absyn_Plus){ goto _LL1191;} else{ goto _LL1192;} _LL1192: if(
_temp1188 ==( void*) Cyc_Absyn_Minus){ goto _LL1193;} else{ goto _LL1194;}
_LL1194: goto _LL1195; _LL1191: change= _temp964; goto _LL1189; _LL1193: change=
Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp964, 0); goto _LL1189;
_LL1195:( int) _throw( Cyc_Tcutil_impos( _tag_arr("bad t ? pointer arithmetic",
sizeof( unsigned char), 27u))); goto _LL1189; _LL1189:;} done= 1;( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_List_List*(* _temp1196)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1199= Cyc_Absyn_address_exp(
_temp968, 0); struct Cyc_Absyn_Exp* _temp1200= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); struct Cyc_Absyn_Exp* _temp1201= change; struct Cyc_Absyn_Exp*
_temp1197[ 3u]={ _temp1199, _temp1200, _temp1201}; struct _tagged_arr _temp1198={(
void*) _temp1197,( void*) _temp1197,( void*)( _temp1197 + 3u)}; _temp1196(
_temp1198);}), 0))->r));}} if( ! done){ if( e1_poly){( void*)( _temp964->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp964->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp968)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, struct _tuple9*), struct _tuple9* f_env)) Cyc_Toc_lvalue_assign)(
_temp968, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple9* _temp1202=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9) * 1); _temp1202[ 0]=({ struct
_tuple9 _temp1203; _temp1203.f1= _temp966; _temp1203.f2= _temp964; _temp1203;});
_temp1202;}));( void*)( e->r=( void*)(( void*) _temp968->r));}} goto _LL869;}}
_LL885: Cyc_Toc_exp_to_c( nv, _temp974); Cyc_Toc_exp_to_c( nv, _temp972); Cyc_Toc_exp_to_c(
nv, _temp970); goto _LL869; _LL887: Cyc_Toc_exp_to_c( nv, _temp978); Cyc_Toc_exp_to_c(
nv, _temp976); goto _LL869; _LL889: _temp988= _temp982; _temp986= _temp980; goto
_LL891; _LL891: Cyc_Toc_exp_to_c( nv, _temp988);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp986); goto _LL869; _LL893:{
struct _RegionHandle _temp1204= _new_region(); struct _RegionHandle* r=&
_temp1204; _push_region( r);{ void* _temp1205=( void*) _temp993->type; void*
_temp1206= Cyc_Toc_typ_to_c( _temp1205); void* _temp1207= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1001->topt))->v); struct Cyc_Absyn_Exp*
_temp1208= Cyc_Absyn_uint_exp(( unsigned int) _temp997, 0); void* arr_type= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1257=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1257[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1258; _temp1258.tag= Cyc_Absyn_ArrayType;
_temp1258.f1=( void*) _temp1205; _temp1258.f2= Cyc_Toc_mt_tq; _temp1258.f3=(
struct Cyc_Absyn_Exp*) _temp1208; _temp1258;}); _temp1257;})); struct _tuple0*
_temp1209= Cyc_Toc_temp_var(); struct _tuple0* _temp1210= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1211= Cyc_Absyn_var_exp( _temp1210, 0); struct
_tuple0* _temp1212= Cyc_Toc_temp_var(); int _temp1213=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp999); int _temp1214= _temp1213 - _temp997; struct
_tagged_arr vs=({ unsigned int _temp1253=( unsigned int) _temp997; struct
_tuple0** _temp1254=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1253); struct _tagged_arr _temp1256= _tag_arr( _temp1254,
sizeof( struct _tuple0*),( unsigned int) _temp997);{ unsigned int _temp1255=
_temp1253; unsigned int i; for( i= 0; i < _temp1255; i ++){ _temp1254[ i]= Cyc_Toc_temp_var();}};
_temp1256;}); struct _tagged_arr xs=({ unsigned int _temp1249=( unsigned int)
_temp1214; struct _tuple0** _temp1250=( struct _tuple0**) _region_malloc( r,
sizeof( struct _tuple0*) * _temp1249); struct _tagged_arr _temp1252= _tag_arr(
_temp1250, sizeof( struct _tuple0*),( unsigned int) _temp1214);{ unsigned int
_temp1251= _temp1249; unsigned int i; for( i= 0; i < _temp1251; i ++){ _temp1250[
i]= Cyc_Toc_temp_var();}}; _temp1252;}); struct Cyc_List_List* xexps=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,(
struct Cyc_Position_Segment*) 0,(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, xs));
xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List* _temp1215=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1215->hd=( void*) Cyc_Absyn_var_exp(
_temp1212, 0); _temp1215->tl= 0; _temp1215;}));{ struct Cyc_Absyn_Stmt*
_temp1216= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
_temp1209, 0), xexps, 0), 0); struct Cyc_Absyn_Exp* _temp1217= Cyc_Absyn_add_exp(
_temp1211, _temp1208, 0); struct Cyc_List_List* _temp1218=({ struct Cyc_List_List*(*
_temp1240)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct _tuple4* _temp1243=({ struct _tuple4* _temp1248=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1248->f1= 0; _temp1248->f2=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1211, 0); _temp1248;});
struct _tuple4* _temp1244=({ struct _tuple4* _temp1247=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1247->f1= 0; _temp1247->f2= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1211, 0); _temp1247;}); struct _tuple4*
_temp1245=({ struct _tuple4* _temp1246=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1246->f1= 0; _temp1246->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1217, 0); _temp1246;}); struct _tuple4* _temp1241[ 3u]={ _temp1243,
_temp1244, _temp1245}; struct _tagged_arr _temp1242={( void*) _temp1241,( void*)
_temp1241,( void*)( _temp1241 + 3u)}; _temp1240( _temp1242);}); _temp1216= Cyc_Absyn_declare_stmt(
_temp1212, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp1205,( void*) _temp993->rgn,
Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1218, 0), _temp1216, 0);{ struct Cyc_List_List* _temp1219=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, vs));
struct Cyc_List_List* _temp1220=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_make_dle,
_temp1219); _temp1216= Cyc_Absyn_declare_stmt( _temp1210, arr_type,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp( 0, _temp1220, 0), _temp1216, 0);{ struct Cyc_List_List*
_temp1221=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, _temp999); struct Cyc_List_List* _temp1222=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp995);{
int i= _temp997 - 1; for( 0; i >= 0;( i --, _temp1221=(( struct Cyc_List_List*)
_check_null( _temp1221))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1221))->hd; void* arg_type=( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c( nv, arg);
if( _temp993->inject){ void* _temp1225; struct Cyc_Position_Segment* _temp1227;
struct Cyc_List_List* _temp1229; struct Cyc_List_List* _temp1231; struct _tuple0*
_temp1233; struct Cyc_Absyn_Tunionfield _temp1223=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1222))->hd); _LL1234: _temp1233=
_temp1223.name; goto _LL1232; _LL1232: _temp1231= _temp1223.tvs; goto _LL1230;
_LL1230: _temp1229= _temp1223.typs; goto _LL1228; _LL1228: _temp1227= _temp1223.loc;
goto _LL1226; _LL1226: _temp1225=( void*) _temp1223.sc; goto _LL1224; _LL1224:
_temp1222=(( struct Cyc_List_List*) _check_null( _temp1222))->tl;{ struct
_tuple0* _temp1235= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1236= Cyc_Absyn_var_exp(
_temp1235, 0); void* _temp1237= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp1233, _tag_arr("_struct", sizeof( unsigned char), 8u))); _temp1216= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1206,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1206, Cyc_Absyn_address_exp(
_temp1236, 0), 0), _temp1216, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple3*)(( struct Cyc_List_List*) _check_null( _temp1229))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1238= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1236, Cyc_Toc_fieldname(
1), 0), arg, 0); _temp1216= Cyc_Absyn_seq_stmt( _temp1238, _temp1216, 0);{
struct Cyc_Absyn_Stmt* _temp1239= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1236, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1233, 0), 0); _temp1216=
Cyc_Absyn_seq_stmt( _temp1239, _temp1216, 0); _temp1216= Cyc_Absyn_declare_stmt(
_temp1235, _temp1237, 0, _temp1216, 0);}}}}} else{ _temp1216= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1206,( struct Cyc_Absyn_Exp*) arg, _temp1216, 0);}}}{ int i= _temp1214 - 1;
for( 0; i >= 0;( i --, _temp1221=(( struct Cyc_List_List*) _check_null(
_temp1221))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1221))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1216= Cyc_Absyn_declare_stmt(*(( struct _tuple0**)
_check_unknown_subscript( xs, sizeof( struct _tuple0*), i)), argtype,( struct
Cyc_Absyn_Exp*) arg, _temp1216, 0);}} Cyc_Toc_exp_to_c( nv, _temp1001);
_temp1216= Cyc_Absyn_declare_stmt( _temp1209, _temp1207,( struct Cyc_Absyn_Exp*)
_temp1001, _temp1216, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1216, 0))->r));}}}}; _pop_region( r);} goto _LL869; _LL895: Cyc_Toc_exp_to_c(
nv, _temp1003);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1003), 0))->r)); goto _LL869; _LL897: Cyc_Toc_exp_to_c(
nv, _temp1005); goto _LL869; _LL899: Cyc_Toc_exp_to_c( nv, _temp1009); for( 0;
_temp1007 != 0; _temp1007=(( struct Cyc_List_List*) _check_null( _temp1007))->tl){
void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null(
_temp1007))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1259= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( _temp1007))->hd); _LL1261: if(( unsigned int) _temp1259 > 4u?*((
int*) _temp1259) == Cyc_Absyn_VarType: 0){ goto _LL1262;} else{ goto _LL1263;}
_LL1263: if(( unsigned int) _temp1259 > 4u?*(( int*) _temp1259) == Cyc_Absyn_TunionType:
0){ goto _LL1264;} else{ goto _LL1265;} _LL1265: goto _LL1266; _LL1262:
continue; _LL1264: continue; _LL1266:( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1009, 0))->r)); goto _LL1260; _LL1260:;} break;}} goto _LL869; _LL901: {
void* old_t2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1011->topt))->v;
void* new_typ=* _temp1015;* _temp1015= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv, _temp1011);{ struct _tuple10 _temp1268=({ struct _tuple10 _temp1267;
_temp1267.f1= Cyc_Tcutil_compress( old_t2); _temp1267.f2= Cyc_Tcutil_compress(
new_typ); _temp1267;}); void* _temp1276; struct Cyc_Absyn_PtrInfo _temp1278;
void* _temp1280; struct Cyc_Absyn_PtrInfo _temp1282; void* _temp1284; void*
_temp1286; struct Cyc_Absyn_PtrInfo _temp1288; _LL1270: _LL1281: _temp1280=
_temp1268.f1; if(( unsigned int) _temp1280 > 4u?*(( int*) _temp1280) == Cyc_Absyn_PointerType:
0){ _LL1283: _temp1282=(( struct Cyc_Absyn_PointerType_struct*) _temp1280)->f1;
goto _LL1277;} else{ goto _LL1272;} _LL1277: _temp1276= _temp1268.f2; if((
unsigned int) _temp1276 > 4u?*(( int*) _temp1276) == Cyc_Absyn_PointerType: 0){
_LL1279: _temp1278=(( struct Cyc_Absyn_PointerType_struct*) _temp1276)->f1; goto
_LL1271;} else{ goto _LL1272;} _LL1272: _LL1287: _temp1286= _temp1268.f1; if((
unsigned int) _temp1286 > 4u?*(( int*) _temp1286) == Cyc_Absyn_PointerType: 0){
_LL1289: _temp1288=(( struct Cyc_Absyn_PointerType_struct*) _temp1286)->f1; goto
_LL1285;} else{ goto _LL1274;} _LL1285: _temp1284= _temp1268.f2; if((
unsigned int) _temp1284 > 4u?*(( int*) _temp1284) == Cyc_Absyn_IntType: 0){ goto
_LL1273;} else{ goto _LL1274;} _LL1274: goto _LL1275; _LL1271: { int _temp1290=((
int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1282.nullable);
int _temp1291=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1278.nullable); void* _temp1292= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1282.bounds); void* _temp1293= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1278.bounds);{ struct _tuple10 _temp1295=({ struct _tuple10 _temp1294;
_temp1294.f1= _temp1292; _temp1294.f2= _temp1293; _temp1294;}); void* _temp1305;
void* _temp1307; void* _temp1309; void* _temp1311; struct Cyc_Absyn_Exp*
_temp1313; void* _temp1315; struct Cyc_Absyn_Exp* _temp1317; void* _temp1319;
_LL1297: _LL1308: _temp1307= _temp1295.f1; if(( unsigned int) _temp1307 > 1u?*((
int*) _temp1307) == Cyc_Absyn_Upper_b: 0){ goto _LL1306;} else{ goto _LL1299;}
_LL1306: _temp1305= _temp1295.f2; if(( unsigned int) _temp1305 > 1u?*(( int*)
_temp1305) == Cyc_Absyn_Upper_b: 0){ goto _LL1298;} else{ goto _LL1299;} _LL1299:
_LL1312: _temp1311= _temp1295.f1; if(( unsigned int) _temp1311 > 1u?*(( int*)
_temp1311) == Cyc_Absyn_Upper_b: 0){ _LL1314: _temp1313=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1311)->f1; goto _LL1310;} else{ goto _LL1301;} _LL1310: _temp1309=
_temp1295.f2; if( _temp1309 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1300;}
else{ goto _LL1301;} _LL1301: _LL1320: _temp1319= _temp1295.f1; if( _temp1319 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1316;} else{ goto _LL1303;} _LL1316:
_temp1315= _temp1295.f2; if(( unsigned int) _temp1315 > 1u?*(( int*) _temp1315)
== Cyc_Absyn_Upper_b: 0){ _LL1318: _temp1317=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1315)->f1; goto _LL1302;} else{ goto _LL1303;} _LL1303: goto _LL1304;
_LL1298: if( _temp1290? ! _temp1291: 0){ if( nv->toplevel){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("can't do null-check conversion at top-level",
sizeof( unsigned char), 44u));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp1015, Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({ struct Cyc_List_List*
_temp1321=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1321->hd=( void*) _temp1011; _temp1321->tl= 0; _temp1321;}), 0), 0))->r));}
goto _LL1296; _LL1300: if( nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr(
old_t2, _temp1313, _temp1011))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1322)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1325= _temp1011; struct Cyc_Absyn_Exp* _temp1326= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1278.elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1327= _temp1313; struct Cyc_Absyn_Exp* _temp1323[ 3u]={ _temp1325,
_temp1326, _temp1327}; struct _tagged_arr _temp1324={( void*) _temp1323,( void*)
_temp1323,( void*)( _temp1323 + 3u)}; _temp1322( _temp1324);}), 0))->r));} goto
_LL1296; _LL1302: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u));}{ struct Cyc_Absyn_Exp* _temp1328= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({
struct Cyc_List_List*(* _temp1330)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1333= _temp1011;
struct Cyc_Absyn_Exp* _temp1334= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c((
void*) _temp1282.elt_typ), 0); struct Cyc_Absyn_Exp* _temp1335= _temp1317;
struct Cyc_Absyn_Exp* _temp1331[ 3u]={ _temp1333, _temp1334, _temp1335}; struct
_tagged_arr _temp1332={( void*) _temp1331,( void*) _temp1331,( void*)( _temp1331
+ 3u)}; _temp1330( _temp1332);}), 0); if( _temp1291){( void*)( _temp1328->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({ struct Cyc_List_List*
_temp1329=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1329->hd=( void*) Cyc_Absyn_copy_exp( _temp1328); _temp1329->tl= 0;
_temp1329;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp1015, _temp1328, 0))->r)); goto _LL1296;} _LL1304: goto _LL1296; _LL1296:;}
goto _LL1269;} _LL1273:{ void* _temp1336= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1288.bounds); _LL1338: if( _temp1336 ==( void*) Cyc_Absyn_Unknown_b){ goto
_LL1339;} else{ goto _LL1340;} _LL1340: goto _LL1341; _LL1339:( void*)(
_temp1011->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1011->r, _temp1011->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1337;
_LL1341: goto _LL1337; _LL1337:;} goto _LL1269; _LL1275: goto _LL1269; _LL1269:;}
goto _LL869;} _LL903:{ void* _temp1342=( void*) _temp1016->r; struct Cyc_List_List*
_temp1350; struct _tuple0* _temp1352; struct Cyc_List_List* _temp1354; _LL1344:
if(*(( int*) _temp1342) == Cyc_Absyn_Struct_e){ _LL1353: _temp1352=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1342)->f1; goto _LL1351; _LL1351: _temp1350=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1342)->f3; goto _LL1345;} else{ goto _LL1346;} _LL1346: if(*(( int*)
_temp1342) == Cyc_Absyn_Tuple_e){ _LL1355: _temp1354=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1342)->f1; goto _LL1347;} else{ goto _LL1348;} _LL1348: goto _LL1349;
_LL1345: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)((
struct _tagged_arr)({ struct _tagged_arr _temp1356=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1016->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_get_arr_size( _temp1356, 1u), _temp1356.curr);}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1016->topt))->v, 1, 0, _temp1350, _temp1352))->r)); goto _LL1343; _LL1347:
if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct
_tagged_arr)({ struct _tagged_arr _temp1357=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1016->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_get_arr_size( _temp1357, 1u), _temp1357.curr);}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1354))->r)); goto _LL1343; _LL1349:
Cyc_Toc_exp_to_c( nv, _temp1016); if( ! Cyc_Absyn_is_lvalue( _temp1016)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1016, 0,
Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1016, 0))->r));} goto _LL1343; _LL1343:;} goto _LL869; _LL905: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr _temp1358=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1018->loc); xprintf("%.*s: new at top-level", _get_arr_size( _temp1358, 1u),
_temp1358.curr);}));}{ void* _temp1359=( void*) _temp1018->r; struct Cyc_List_List*
_temp1371; struct Cyc_Absyn_Exp* _temp1373; struct Cyc_Absyn_Exp* _temp1375;
struct Cyc_Absyn_Vardecl* _temp1377; struct Cyc_List_List* _temp1379; struct
_tuple0* _temp1381; struct Cyc_List_List* _temp1383; _LL1361: if(*(( int*)
_temp1359) == Cyc_Absyn_Array_e){ _LL1372: _temp1371=(( struct Cyc_Absyn_Array_e_struct*)
_temp1359)->f1; goto _LL1362;} else{ goto _LL1363;} _LL1363: if(*(( int*)
_temp1359) == Cyc_Absyn_Comprehension_e){ _LL1378: _temp1377=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1359)->f1; goto _LL1376; _LL1376: _temp1375=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1359)->f2; goto _LL1374; _LL1374: _temp1373=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1359)->f3; goto _LL1364;} else{ goto _LL1365;} _LL1365: if(*(( int*)
_temp1359) == Cyc_Absyn_Struct_e){ _LL1382: _temp1381=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1359)->f1; goto _LL1380; _LL1380: _temp1379=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1359)->f3; goto _LL1366;} else{ goto _LL1367;} _LL1367: if(*(( int*)
_temp1359) == Cyc_Absyn_Tuple_e){ _LL1384: _temp1383=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1359)->f1; goto _LL1368;} else{ goto _LL1369;} _LL1369: goto _LL1370;
_LL1362: { struct _tuple0* _temp1385= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1386= Cyc_Absyn_var_exp( _temp1385, 0); struct Cyc_Absyn_Stmt* _temp1387=
Cyc_Toc_init_array( nv, _temp1386, _temp1371, Cyc_Absyn_exp_stmt( _temp1386, 0));
void* old_elt_typ;{ void* _temp1388= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1394; struct Cyc_Absyn_Tqual _temp1396; void* _temp1398; _LL1390: if((
unsigned int) _temp1388 > 4u?*(( int*) _temp1388) == Cyc_Absyn_PointerType: 0){
_LL1395: _temp1394=(( struct Cyc_Absyn_PointerType_struct*) _temp1388)->f1;
_LL1399: _temp1398=( void*) _temp1394.elt_typ; goto _LL1397; _LL1397: _temp1396=
_temp1394.tq; goto _LL1391;} else{ goto _LL1392;} _LL1392: goto _LL1393; _LL1391:
old_elt_typ= _temp1398; goto _LL1389; _LL1393: old_elt_typ= Cyc_Toc_toc_impos(
_tag_arr("exp_to_c:new array expression doesn't have ptr type", sizeof(
unsigned char), 52u)); goto _LL1389; _LL1389:;}{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* _temp1400= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* _temp1401= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1371), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1020 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1401);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1020); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1401);}{ struct Cyc_Absyn_Exp* _temp1402= Cyc_Absyn_cast_exp( _temp1400, e1,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1385, _temp1400,( struct Cyc_Absyn_Exp*) _temp1402, _temp1387, 0), 0))->r));
goto _LL1360;}}} _LL1364: { int is_tagged_ptr= 0;{ void* _temp1403= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1409; struct Cyc_Absyn_Conref* _temp1411;
struct Cyc_Absyn_Tqual _temp1413; void* _temp1415; _LL1405: if(( unsigned int)
_temp1403 > 4u?*(( int*) _temp1403) == Cyc_Absyn_PointerType: 0){ _LL1410:
_temp1409=(( struct Cyc_Absyn_PointerType_struct*) _temp1403)->f1; _LL1416:
_temp1415=( void*) _temp1409.elt_typ; goto _LL1414; _LL1414: _temp1413=
_temp1409.tq; goto _LL1412; _LL1412: _temp1411= _temp1409.bounds; goto _LL1406;}
else{ goto _LL1407;} _LL1407: goto _LL1408; _LL1406: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1411) ==( void*) Cyc_Absyn_Unknown_b; goto _LL1404;
_LL1408:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u)); goto _LL1404; _LL1404:;}{ struct _tuple0* max=
Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* old_elt_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1373->topt))->v; void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1375);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1377, Cyc_Absyn_var_exp( max, 0), _temp1373,
Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1417= _new_region();
struct _RegionHandle* r=& _temp1417; _push_region( r);{ struct Cyc_List_List*
decls=({ struct Cyc_List_List* _temp1440=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1440->hd=( void*)({ struct _tuple8*
_temp1441=( struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8));
_temp1441->f1= max; _temp1441->f2= Cyc_Absyn_uint_t; _temp1441->f3=( struct Cyc_Absyn_Exp*)
_temp1375; _temp1441;}); _temp1440->tl= 0; _temp1440;}); struct Cyc_Absyn_Exp*
ai; if( _temp1020 == 0){ ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{
struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1020); Cyc_Toc_exp_to_c(
nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp(
ptr_typ, ai, 0); decls=({ struct Cyc_List_List* _temp1418=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1418->hd=( void*)({
struct _tuple8* _temp1419=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1419->f1= a; _temp1419->f2= ptr_typ; _temp1419->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1419;}); _temp1418->tl= decls; _temp1418;}); if( is_tagged_ptr){
struct _tuple0* _temp1420= Cyc_Toc_temp_var(); void* _temp1421= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Exp* _temp1422= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_List_List*(* _temp1425)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1428= Cyc_Absyn_var_exp(
a, 0); struct Cyc_Absyn_Exp* _temp1429= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
struct Cyc_Absyn_Exp* _temp1430= _temp1375; struct Cyc_Absyn_Exp* _temp1426[ 3u]={
_temp1428, _temp1429, _temp1430}; struct _tagged_arr _temp1427={( void*)
_temp1426,( void*) _temp1426,( void*)( _temp1426 + 3u)}; _temp1425( _temp1427);}),
0); decls=({ struct Cyc_List_List* _temp1423=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1423->hd=( void*)({
struct _tuple8* _temp1424=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1424->f1= _temp1420; _temp1424->f2= _temp1421; _temp1424->f3=(
struct Cyc_Absyn_Exp*) _temp1422; _temp1424;}); _temp1423->tl= decls; _temp1423;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1420, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1431= decls; for( 0; _temp1431 != 0;
_temp1431=(( struct Cyc_List_List*) _check_null( _temp1431))->tl){ struct Cyc_Absyn_Exp*
_temp1434; void* _temp1436; struct _tuple0* _temp1438; struct _tuple8 _temp1432=*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1431))->hd); _LL1439:
_temp1438= _temp1432.f1; goto _LL1437; _LL1437: _temp1436= _temp1432.f2; goto
_LL1435; _LL1435: _temp1434= _temp1432.f3; goto _LL1433; _LL1433: s= Cyc_Absyn_declare_stmt(
_temp1438, _temp1436, _temp1434, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL1360;}}} _LL1366:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1018->topt))->v, 1, _temp1020, _temp1379, _temp1381))->r)); goto _LL1360;
_LL1368:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1020,
_temp1383))->r)); goto _LL1360; _LL1370: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1018->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp1020 == 0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1020); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1442=( void*)
_temp1018->r; struct Cyc_Absyn_Exp* _temp1448; void* _temp1450; _LL1444: if(*((
int*) _temp1442) == Cyc_Absyn_Cast_e){ _LL1451: _temp1450=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1442)->f1; goto _LL1449; _LL1449: _temp1448=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1442)->f2; goto _LL1445;} else{ goto _LL1446;} _LL1446: goto _LL1447;
_LL1445:{ struct _tuple10 _temp1453=({ struct _tuple10 _temp1452; _temp1452.f1=
Cyc_Tcutil_compress( _temp1450); _temp1452.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1448->topt))->v); _temp1452;}); void*
_temp1459; struct Cyc_Absyn_PtrInfo _temp1461; struct Cyc_Absyn_Conref*
_temp1463; void* _temp1465; struct Cyc_Absyn_PtrInfo _temp1467; struct Cyc_Absyn_Conref*
_temp1469; void* _temp1471; _LL1455: _LL1466: _temp1465= _temp1453.f1; if((
unsigned int) _temp1465 > 4u?*(( int*) _temp1465) == Cyc_Absyn_PointerType: 0){
_LL1468: _temp1467=(( struct Cyc_Absyn_PointerType_struct*) _temp1465)->f1;
_LL1472: _temp1471=( void*) _temp1467.elt_typ; goto _LL1470; _LL1470: _temp1469=
_temp1467.bounds; goto _LL1460;} else{ goto _LL1457;} _LL1460: _temp1459=
_temp1453.f2; if(( unsigned int) _temp1459 > 4u?*(( int*) _temp1459) == Cyc_Absyn_PointerType:
0){ _LL1462: _temp1461=(( struct Cyc_Absyn_PointerType_struct*) _temp1459)->f1;
_LL1464: _temp1463= _temp1461.bounds; goto _LL1456;} else{ goto _LL1457;}
_LL1457: goto _LL1458; _LL1456:{ struct _tuple10 _temp1474=({ struct _tuple10
_temp1473; _temp1473.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1469);
_temp1473.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1463); _temp1473;});
void* _temp1480; struct Cyc_Absyn_Exp* _temp1482; void* _temp1484; _LL1476:
_LL1485: _temp1484= _temp1474.f1; if( _temp1484 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1481;} else{ goto _LL1478;} _LL1481: _temp1480= _temp1474.f2; if((
unsigned int) _temp1480 > 1u?*(( int*) _temp1480) == Cyc_Absyn_Upper_b: 0){
_LL1483: _temp1482=(( struct Cyc_Absyn_Upper_b_struct*) _temp1480)->f1; goto
_LL1477;} else{ goto _LL1478;} _LL1478: goto _LL1479; _LL1477: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_List_List*(* _temp1486)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1489= mexp;
struct Cyc_Absyn_Exp* _temp1490= _temp1448; struct Cyc_Absyn_Exp* _temp1491= Cyc_Absyn_sizeoftyp_exp(
_temp1471, 0); struct Cyc_Absyn_Exp* _temp1492= _temp1482; struct Cyc_Absyn_Exp*
_temp1487[ 4u]={ _temp1489, _temp1490, _temp1491, _temp1492}; struct _tagged_arr
_temp1488={( void*) _temp1487,( void*) _temp1487,( void*)( _temp1487 + 4u)};
_temp1486( _temp1488);}), 0))->r)); goto _LL1475; _LL1479: goto _LL1475; _LL1475:;}
goto _LL1454; _LL1458: goto _LL1454; _LL1454:;} goto _LL1443; _LL1447: goto
_LL1443; _LL1443:;} if( ! done){ struct _tuple0* _temp1493= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* _temp1494= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1493, 0), 0); struct Cyc_Absyn_Exp* _temp1495= Cyc_Absyn_signed_int_exp( 0,
0); Cyc_Toc_exp_to_c( nv, _temp1018); _temp1494= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1493, 0), _temp1495, 0),
_temp1018, 0), _temp1494, 0);{ void* _temp1496= Cyc_Absyn_cstar_typ( elt_typ,
Cyc_Toc_mt_tq);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1493, _temp1496,( struct Cyc_Absyn_Exp*) mexp, _temp1494, 0), 0))->r));}}
goto _LL1360;}} _LL1360:;} goto _LL869; _LL907: Cyc_Toc_exp_to_c( nv, _temp1022);
goto _LL869; _LL909:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1497=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1497[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1498; _temp1498.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1498.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1024);
_temp1498;}); _temp1497;}))); goto _LL869; _LL911:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Offsetof_e_struct* _temp1499=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1499[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1500; _temp1500.tag= Cyc_Absyn_Offsetof_e;
_temp1500.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1028); _temp1500.f2=
_temp1026; _temp1500;}); _temp1499;}))); goto _LL869; _LL913: { void* _temp1501=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1030->topt))->v);{
void* _temp1502= _temp1501; struct Cyc_Absyn_PtrInfo _temp1508; struct Cyc_Absyn_Conref*
_temp1510; struct Cyc_Absyn_Tqual _temp1512; struct Cyc_Absyn_Conref* _temp1514;
void* _temp1516; void* _temp1518; _LL1504: if(( unsigned int) _temp1502 > 4u?*((
int*) _temp1502) == Cyc_Absyn_PointerType: 0){ _LL1509: _temp1508=(( struct Cyc_Absyn_PointerType_struct*)
_temp1502)->f1; _LL1519: _temp1518=( void*) _temp1508.elt_typ; goto _LL1517;
_LL1517: _temp1516=( void*) _temp1508.rgn_typ; goto _LL1515; _LL1515: _temp1514=
_temp1508.nullable; goto _LL1513; _LL1513: _temp1512= _temp1508.tq; goto _LL1511;
_LL1511: _temp1510= _temp1508.bounds; goto _LL1505;} else{ goto _LL1506;}
_LL1506: goto _LL1507; _LL1505:{ void* _temp1520= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1510); _LL1522: if(( unsigned int) _temp1520 > 1u?*(( int*) _temp1520) ==
Cyc_Absyn_Upper_b: 0){ goto _LL1523;} else{ goto _LL1524;} _LL1524: if(
_temp1520 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1525;} else{ goto _LL1521;}
_LL1523: Cyc_Toc_exp_to_c( nv, _temp1030); if( Cyc_Toc_is_nullable( _temp1501)){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1030->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1526=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1526->hd=( void*) _temp1030;
_temp1526->tl= 0; _temp1526;}), 0), 0), 0))->r));} goto _LL1521; _LL1525: {
struct Cyc_Absyn_Exp* _temp1527= Cyc_Absyn_uint_exp( 0, 0); _temp1527->topt=({
struct Cyc_Core_Opt* _temp1528=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1528->v=( void*) Cyc_Absyn_uint_t; _temp1528;});( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_subscript_exp( _temp1030, _temp1527, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1521;} _LL1521:;} goto _LL1503; _LL1507:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Deref: non-pointer",
sizeof( unsigned char), 29u)); goto _LL1503; _LL1503:;} goto _LL869;} _LL915:
Cyc_Toc_exp_to_c( nv, _temp1034); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL869; _LL917: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1038->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1038);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1531; struct Cyc_Absyn_Tqual _temp1533; struct Cyc_Absyn_Conref* _temp1535;
void* _temp1537; void* _temp1539; struct Cyc_Absyn_PtrInfo _temp1529= Cyc_Toc_get_ptr_type(
e1typ); _LL1540: _temp1539=( void*) _temp1529.elt_typ; goto _LL1538; _LL1538:
_temp1537=( void*) _temp1529.rgn_typ; goto _LL1536; _LL1536: _temp1535=
_temp1529.nullable; goto _LL1534; _LL1534: _temp1533= _temp1529.tq; goto _LL1532;
_LL1532: _temp1531= _temp1529.bounds; goto _LL1530; _LL1530:{ void* _temp1541=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1531); struct Cyc_Absyn_Exp*
_temp1547; _LL1543: if(( unsigned int) _temp1541 > 1u?*(( int*) _temp1541) ==
Cyc_Absyn_Upper_b: 0){ _LL1548: _temp1547=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1541)->f1; goto _LL1544;} else{ goto _LL1545;} _LL1545: if( _temp1541 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1546;} else{ goto _LL1542;} _LL1544: if(
Cyc_Evexp_eval_const_uint_exp( _temp1547) < 1){(( int(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof( unsigned char), 46u));}{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( 0, _temp1535); if( Cyc_Toc_is_nullable(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1038->topt))->v)){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1038->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1549=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1549->hd=( void*) _temp1038;
_temp1549->tl= 0; _temp1549;}), 0), 0), _temp1036, 0))->r));} goto _LL1542;}
_LL1546: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1539);( void*)( _temp1038->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1533), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_List_List*(* _temp1550)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1553= Cyc_Absyn_copy_exp( _temp1038); struct Cyc_Absyn_Exp*
_temp1554= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp* _temp1555=
Cyc_Absyn_uint_exp( 0, 0); struct Cyc_Absyn_Exp* _temp1551[ 3u]={ _temp1553,
_temp1554, _temp1555}; struct _tagged_arr _temp1552={( void*) _temp1551,( void*)
_temp1551,( void*)( _temp1551 + 3u)}; _temp1550( _temp1552);}), 0), 0))->r));
goto _LL1542;} _LL1542:;} if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL869;}} _LL919: { void*
_temp1556= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1042->topt))->v); Cyc_Toc_exp_to_c( nv, _temp1042); Cyc_Toc_exp_to_c( nv,
_temp1040);{ void* _temp1557= _temp1556; struct Cyc_List_List* _temp1565; struct
Cyc_Absyn_PtrInfo _temp1567; struct Cyc_Absyn_Conref* _temp1569; struct Cyc_Absyn_Tqual
_temp1571; struct Cyc_Absyn_Conref* _temp1573; void* _temp1575; void* _temp1577;
_LL1559: if(( unsigned int) _temp1557 > 4u?*(( int*) _temp1557) == Cyc_Absyn_TupleType:
0){ _LL1566: _temp1565=(( struct Cyc_Absyn_TupleType_struct*) _temp1557)->f1;
goto _LL1560;} else{ goto _LL1561;} _LL1561: if(( unsigned int) _temp1557 > 4u?*((
int*) _temp1557) == Cyc_Absyn_PointerType: 0){ _LL1568: _temp1567=(( struct Cyc_Absyn_PointerType_struct*)
_temp1557)->f1; _LL1578: _temp1577=( void*) _temp1567.elt_typ; goto _LL1576;
_LL1576: _temp1575=( void*) _temp1567.rgn_typ; goto _LL1574; _LL1574: _temp1573=
_temp1567.nullable; goto _LL1572; _LL1572: _temp1571= _temp1567.tq; goto _LL1570;
_LL1570: _temp1569= _temp1567.bounds; goto _LL1562;} else{ goto _LL1563;}
_LL1563: goto _LL1564; _LL1560: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1040) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1042, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL1558;} _LL1562:{ void*
_temp1579= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1569); struct Cyc_Absyn_Exp*
_temp1585; _LL1581: if(( unsigned int) _temp1579 > 1u?*(( int*) _temp1579) ==
Cyc_Absyn_Upper_b: 0){ _LL1586: _temp1585=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1579)->f1; goto _LL1582;} else{ goto _LL1583;} _LL1583: if( _temp1579 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1584;} else{ goto _LL1580;} _LL1582:
_temp1585= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp( _temp1585), 0);{
int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1573); void* ta1= Cyc_Toc_typ_to_c( _temp1577); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1571); if( possibly_null){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_List_List*(* _temp1587)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1590= _temp1042;
struct Cyc_Absyn_Exp* _temp1591= _temp1585; struct Cyc_Absyn_Exp* _temp1592= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp1593= _temp1040; struct Cyc_Absyn_Exp*
_temp1588[ 4u]={ _temp1590, _temp1591, _temp1592, _temp1593}; struct _tagged_arr
_temp1589={( void*) _temp1588,( void*) _temp1588,( void*)( _temp1588 + 4u)};
_temp1587( _temp1589);}), 0), 0), 0))->r));} else{( void*)( _temp1040->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_notnull_e,({ struct
Cyc_List_List*(* _temp1594)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1597= _temp1585;
struct Cyc_Absyn_Exp* _temp1598= Cyc_Absyn_copy_exp( _temp1040); struct Cyc_Absyn_Exp*
_temp1595[ 2u]={ _temp1597, _temp1598}; struct _tagged_arr _temp1596={( void*)
_temp1595,( void*) _temp1595,( void*)( _temp1595 + 2u)}; _temp1594( _temp1596);}),
0))->r));} goto _LL1580;} _LL1584: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp1577);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( ta1, _temp1571), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_List_List*(* _temp1599)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1602= _temp1042;
struct Cyc_Absyn_Exp* _temp1603= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp1604= _temp1040; struct Cyc_Absyn_Exp* _temp1600[ 3u]={ _temp1602,
_temp1603, _temp1604}; struct _tagged_arr _temp1601={( void*) _temp1600,( void*)
_temp1600,( void*)( _temp1600 + 3u)}; _temp1599( _temp1601);}), 0), 0), 0))->r));
goto _LL1580;} _LL1580:;} goto _LL1558; _LL1564:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u)); goto _LL1558; _LL1558:;} goto _LL869;} _LL921:
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 0,
0, _temp1044))->r));} else{ struct Cyc_List_List* _temp1605=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, _temp1044); struct _tagged_arr* _temp1606= Cyc_Toc_add_tuple_type(
_temp1605); struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1044 != 0;(
_temp1044=(( struct Cyc_List_List*) _check_null( _temp1044))->tl, i ++)){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1044))->hd);
dles=({ struct Cyc_List_List* _temp1607=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1607->hd=( void*)({ struct _tuple4*
_temp1608=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1608->f1=
0; _temp1608->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1044))->hd; _temp1608;}); _temp1607->tl= dles; _temp1607;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL869; _LL923:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1046, 0))->r));{ struct Cyc_List_List* _temp1609= _temp1046; for( 0;
_temp1609 != 0; _temp1609=(( struct Cyc_List_List*) _check_null( _temp1609))->tl){
struct _tuple4 _temp1612; struct Cyc_Absyn_Exp* _temp1613; struct _tuple4*
_temp1610=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1609))->hd;
_temp1612=* _temp1610; _LL1614: _temp1613= _temp1612.f2; goto _LL1611; _LL1611:
Cyc_Toc_exp_to_c( nv, _temp1613);}} goto _LL869; _LL925: { unsigned int
_temp1615= Cyc_Evexp_eval_const_uint_exp( _temp1050); Cyc_Toc_exp_to_c( nv,
_temp1048);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1048)){
unsigned int i= 0; for( 0; i < _temp1615; i ++){ es=({ struct Cyc_List_List*
_temp1616=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1616->hd=( void*)({ struct _tuple4* _temp1617=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1617->f1= 0; _temp1617->f2= _temp1048; _temp1617;});
_temp1616->tl= es; _temp1616;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL869;}} _LL927: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1056, _temp1060))->r));}
else{ if( _temp1054 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Struct_e: missing structdecl pointer", sizeof( unsigned char), 37u));}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp1054); struct _RegionHandle _temp1618= _new_region(); struct _RegionHandle*
rgn=& _temp1618; _push_region( rgn);{ struct Cyc_List_List* _temp1619=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1056,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1620= 0;{ struct Cyc_List_List*
_temp1621=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1621 != 0; _temp1621=(( struct Cyc_List_List*) _check_null(
_temp1621))->tl){ struct Cyc_List_List* _temp1622= _temp1619; for( 0; _temp1622
!= 0; _temp1622=(( struct Cyc_List_List*) _check_null( _temp1622))->tl){ if((*((
struct _tuple11*)(( struct Cyc_List_List*) _check_null( _temp1622))->hd)).f1 ==(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1621))->hd){
struct _tuple11 _temp1625; struct Cyc_Absyn_Exp* _temp1626; struct Cyc_Absyn_Structfield*
_temp1628; struct _tuple11* _temp1623=( struct _tuple11*)(( struct Cyc_List_List*)
_check_null( _temp1622))->hd; _temp1625=* _temp1623; _LL1629: _temp1628=
_temp1625.f1; goto _LL1627; _LL1627: _temp1626= _temp1625.f2; goto _LL1624;
_LL1624: { void* _temp1630=( void*) _temp1628->type; Cyc_Toc_exp_to_c( nv,
_temp1626); if( Cyc_Toc_is_void_star( _temp1630)){( void*)( _temp1626->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1626->r, 0), 0))->r));} _temp1620=({ struct Cyc_List_List* _temp1631=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1631->hd=(
void*)({ struct _tuple4* _temp1632=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1632->f1= 0; _temp1632->f2= _temp1626; _temp1632;}); _temp1631->tl=
_temp1620; _temp1631;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1620), 0))->r));}; _pop_region( rgn);}} goto _LL869; _LL929: { struct Cyc_List_List*
fs;{ void* _temp1633= Cyc_Tcutil_compress( _temp1064); struct Cyc_List_List*
_temp1639; _LL1635: if(( unsigned int) _temp1633 > 4u?*(( int*) _temp1633) ==
Cyc_Absyn_AnonStructType: 0){ _LL1640: _temp1639=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1633)->f1; goto _LL1636;} else{ goto _LL1637;} _LL1637: goto _LL1638;
_LL1636: fs= _temp1639; goto _LL1634; _LL1638: fs=(( struct Cyc_List_List*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp1641= Cyc_Absynpp_typ2string( _temp1064); xprintf("anon struct has type %.*s",
_get_arr_size( _temp1641, 1u), _temp1641.curr);})); goto _LL1634; _LL1634:;}{
struct _RegionHandle _temp1642= _new_region(); struct _RegionHandle* rgn=&
_temp1642; _push_region( rgn);{ struct Cyc_List_List* _temp1643=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1062, fs); for( 0; _temp1643 != 0; _temp1643=(( struct Cyc_List_List*)
_check_null( _temp1643))->tl){ struct _tuple11 _temp1646; struct Cyc_Absyn_Exp*
_temp1647; struct Cyc_Absyn_Structfield* _temp1649; struct _tuple11* _temp1644=(
struct _tuple11*)(( struct Cyc_List_List*) _check_null( _temp1643))->hd;
_temp1646=* _temp1644; _LL1650: _temp1649= _temp1646.f1; goto _LL1648; _LL1648:
_temp1647= _temp1646.f2; goto _LL1645; _LL1645: { void* _temp1651=( void*)
_temp1649->type; Cyc_Toc_exp_to_c( nv, _temp1647); if( Cyc_Toc_is_void_star(
_temp1651)){( void*)( _temp1647->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1647->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1062, 0))->r));}; _pop_region( rgn);}
goto _LL869;} _LL931: { struct _tuple0* qv= _temp1066->name; if( _temp1068->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1652= _temp1068->fields == 0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1068->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1652))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1652))->hd)->typs == 0){ tag_count ++;}
_temp1652=(( struct Cyc_List_List*) _check_null( _temp1652))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL869;} _LL933: { struct _tuple0* _temp1653= _temp1072->name; struct Cyc_List_List*
_temp1654= _temp1072->typs; struct _tuple0* _temp1655= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1656= Cyc_Absyn_var_exp( _temp1655, 0); void*
_temp1657= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1653, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1074->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1653, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1658= _temp1074->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1074->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1653,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1658))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1658))->hd)->typs != 0){ tag_count ++;}
_temp1658=(( struct Cyc_List_List*) _check_null( _temp1658))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp1659= 0; for( 0;
_temp1076 != 0;( _temp1076=(( struct Cyc_List_List*) _check_null( _temp1076))->tl,
_temp1654=(( struct Cyc_List_List*) _check_null( _temp1654))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1076))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1654))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} _temp1659=({
struct Cyc_List_List* _temp1660=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1660->hd=( void*)({ struct _tuple4* _temp1661=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1661->f1= 0;
_temp1661->f2= cur_e; _temp1661;}); _temp1660->tl= _temp1659; _temp1660;});}
_temp1659=({ struct Cyc_List_List* _temp1662=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1662->hd=( void*)({ struct _tuple4*
_temp1663=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1663->f1=
0; _temp1663->f2= tag_exp; _temp1663;}); _temp1662->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1659); _temp1662;});( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1664=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1664->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1653, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp1664;}), _temp1659, 0))->r));}} else{ struct
Cyc_List_List* _temp1665=({ struct Cyc_List_List* _temp1671=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1671->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1656, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1653, 0), 0); _temp1671->tl= 0; _temp1671;});{ int i= 1; for( 0; _temp1076
!= 0;((( _temp1076=(( struct Cyc_List_List*) _check_null( _temp1076))->tl, i ++)),
_temp1654=(( struct Cyc_List_List*) _check_null( _temp1654))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1076))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1654))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp1666= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1656, Cyc_Toc_fieldname(
i), 0), cur_e, 0); _temp1665=({ struct Cyc_List_List* _temp1667=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1667->hd=( void*) _temp1666;
_temp1667->tl= _temp1665; _temp1667;});}}}{ struct Cyc_Absyn_Stmt* _temp1668=
Cyc_Absyn_exp_stmt( _temp1656, 0); struct Cyc_Absyn_Stmt* _temp1669= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1670=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1670->hd=( void*) _temp1668; _temp1670->tl= _temp1665; _temp1670;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1655, _temp1657, 0, _temp1669, 0), 0))->r));}} goto _LL869;} _LL935: goto
_LL869; _LL937: { void* t_c= Cyc_Toc_typ_to_c( _temp1078); if( _temp1080 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1080); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1078, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL869;} _LL939: Cyc_Toc_stmt_to_c(
nv, _temp1082); goto _LL869; _LL941:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem", sizeof( unsigned char), 14u)); goto _LL869; _LL943:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u)); goto _LL869; _LL945:(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("codegen", sizeof( unsigned char), 8u)); goto _LL869; _LL947:(( int(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("fill", sizeof( unsigned char),
5u)); goto _LL869; _LL869:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_arr* succ_lab,
struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
0);} struct _tuple12{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct
Cyc_Absyn_Stmt* f3; } ; struct _tuple13{ struct _tuple0* f1; void* f2; } ;
struct _tuple14{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static
struct _tuple12 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct _RegionHandle*
rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1672=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1708; struct Cyc_Absyn_Vardecl _temp1710; struct _tuple0* _temp1711; struct
Cyc_Absyn_Vardecl* _temp1713; int _temp1715; void* _temp1717; unsigned char
_temp1719; struct _tagged_arr _temp1721; struct Cyc_Absyn_Enumfield* _temp1723;
struct Cyc_Absyn_Enumdecl* _temp1725; struct Cyc_List_List* _temp1727; struct
Cyc_List_List* _temp1729; struct Cyc_Absyn_Tunionfield* _temp1731; struct Cyc_Absyn_Tuniondecl*
_temp1733; struct Cyc_Absyn_Pat* _temp1736; struct Cyc_Absyn_Pat _temp1738; void*
_temp1739; struct Cyc_List_List* _temp1741; struct Cyc_List_List* _temp1743;
struct Cyc_Absyn_Tunionfield* _temp1745; struct Cyc_Absyn_Tuniondecl* _temp1747;
struct Cyc_List_List* _temp1749; struct Cyc_List_List* _temp1751; struct Cyc_List_List*
_temp1753; struct Cyc_List_List* _temp1755; struct Cyc_Core_Opt* _temp1757;
struct Cyc_Absyn_Structdecl* _temp1759; struct Cyc_Absyn_Pat* _temp1761; _LL1674:
if(( unsigned int) _temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_Var_p: 0){
_LL1709: _temp1708=(( struct Cyc_Absyn_Var_p_struct*) _temp1672)->f1; _temp1710=*
_temp1708; _LL1712: _temp1711= _temp1710.name; goto _LL1675;} else{ goto _LL1676;}
_LL1676: if( _temp1672 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1677;} else{ goto
_LL1678;} _LL1678: if(( unsigned int) _temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_Reference_p:
0){ _LL1714: _temp1713=(( struct Cyc_Absyn_Reference_p_struct*) _temp1672)->f1;
goto _LL1679;} else{ goto _LL1680;} _LL1680: if( _temp1672 ==( void*) Cyc_Absyn_Null_p){
goto _LL1681;} else{ goto _LL1682;} _LL1682: if(( unsigned int) _temp1672 > 2u?*((
int*) _temp1672) == Cyc_Absyn_Int_p: 0){ _LL1718: _temp1717=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1672)->f1; goto _LL1716; _LL1716: _temp1715=((
struct Cyc_Absyn_Int_p_struct*) _temp1672)->f2; goto _LL1683;} else{ goto
_LL1684;} _LL1684: if(( unsigned int) _temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_Char_p:
0){ _LL1720: _temp1719=(( struct Cyc_Absyn_Char_p_struct*) _temp1672)->f1; goto
_LL1685;} else{ goto _LL1686;} _LL1686: if(( unsigned int) _temp1672 > 2u?*((
int*) _temp1672) == Cyc_Absyn_Float_p: 0){ _LL1722: _temp1721=(( struct Cyc_Absyn_Float_p_struct*)
_temp1672)->f1; goto _LL1687;} else{ goto _LL1688;} _LL1688: if(( unsigned int)
_temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_Enum_p: 0){ _LL1726: _temp1725=((
struct Cyc_Absyn_Enum_p_struct*) _temp1672)->f1; goto _LL1724; _LL1724:
_temp1723=(( struct Cyc_Absyn_Enum_p_struct*) _temp1672)->f2; goto _LL1689;}
else{ goto _LL1690;} _LL1690: if(( unsigned int) _temp1672 > 2u?*(( int*)
_temp1672) == Cyc_Absyn_Tunion_p: 0){ _LL1734: _temp1733=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1672)->f1; goto _LL1732; _LL1732: _temp1731=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1672)->f2; goto _LL1730; _LL1730: _temp1729=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1672)->f3; goto _LL1728; _LL1728: _temp1727=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1672)->f4; if( _temp1727 == 0){ goto _LL1691;} else{ goto _LL1692;}} else{
goto _LL1692;} _LL1692: if(( unsigned int) _temp1672 > 2u?*(( int*) _temp1672)
== Cyc_Absyn_Pointer_p: 0){ _LL1737: _temp1736=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1672)->f1; _temp1738=* _temp1736; _LL1740: _temp1739=( void*) _temp1738.r;
if(( unsigned int) _temp1739 > 2u?*(( int*) _temp1739) == Cyc_Absyn_Tunion_p: 0){
_LL1748: _temp1747=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1739)->f1; goto
_LL1746; _LL1746: _temp1745=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1739)->f2;
goto _LL1744; _LL1744: _temp1743=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1739)->f3;
goto _LL1742; _LL1742: _temp1741=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1739)->f4;
goto _LL1735;} else{ goto _LL1694;}} else{ goto _LL1694;} _LL1735: if( _temp1741
!= 0){ goto _LL1693;} else{ goto _LL1694;} _LL1694: if(( unsigned int) _temp1672
> 2u?*(( int*) _temp1672) == Cyc_Absyn_Tunion_p: 0){ _LL1750: _temp1749=((
struct Cyc_Absyn_Tunion_p_struct*) _temp1672)->f4; goto _LL1695;} else{ goto
_LL1696;} _LL1696: if(( unsigned int) _temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_Tuple_p:
0){ _LL1752: _temp1751=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1672)->f1; goto
_LL1697;} else{ goto _LL1698;} _LL1698: if(( unsigned int) _temp1672 > 2u?*((
int*) _temp1672) == Cyc_Absyn_Struct_p: 0){ _LL1760: _temp1759=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1672)->f1; goto _LL1758; _LL1758: _temp1757=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1672)->f2; goto _LL1756; _LL1756: _temp1755=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1672)->f3; goto _LL1754; _LL1754: _temp1753=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1672)->f4; goto _LL1699;} else{ goto _LL1700;} _LL1700: if(( unsigned int)
_temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_Pointer_p: 0){ _LL1762:
_temp1761=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1672)->f1; goto _LL1701;}
else{ goto _LL1702;} _LL1702: if(( unsigned int) _temp1672 > 2u?*(( int*)
_temp1672) == Cyc_Absyn_UnknownId_p: 0){ goto _LL1703;} else{ goto _LL1704;}
_LL1704: if(( unsigned int) _temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_UnknownCall_p:
0){ goto _LL1705;} else{ goto _LL1706;} _LL1706: if(( unsigned int) _temp1672 >
2u?*(( int*) _temp1672) == Cyc_Absyn_UnknownFields_p: 0){ goto _LL1707;} else{
goto _LL1673;} _LL1675: nv= Cyc_Toc_add_varmap( nv, _temp1711, r); goto _LL1677;
_LL1677: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1673; _LL1679: { struct
_tuple0* _temp1763= Cyc_Toc_temp_var(); decls=({ struct Cyc_List_List* _temp1764=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1764->hd=( void*)({ struct _tuple13* _temp1765=( struct _tuple13*)
_region_malloc( rgn, sizeof( struct _tuple13)); _temp1765->f1= _temp1763;
_temp1765->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq);
_temp1765;}); _temp1764->tl= decls; _temp1764;}); nv= Cyc_Toc_add_varmap( nv,
_temp1713->name, Cyc_Absyn_var_exp( _temp1763, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1763, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1673;} _LL1681: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1673; _LL1683: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp1717, _temp1715, 0), succ_lab, fail_lab); goto _LL1673;
_LL1685: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp1719, 0),
succ_lab, fail_lab); goto _LL1673; _LL1687: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp1721, 0), succ_lab, fail_lab); goto _LL1673; _LL1689: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp1766=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1766[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1767; _temp1767.tag= Cyc_Absyn_Enum_e;
_temp1767.f1= _temp1723->name; _temp1767.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1725; _temp1767.f3=( struct Cyc_Absyn_Enumfield*) _temp1723; _temp1767;});
_temp1766;}), 0), succ_lab, fail_lab); goto _LL1673; _LL1691: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1731->name, 0); if( ! _temp1733->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1673;} _LL1693: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1741);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1745->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1768=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1745->typs); struct Cyc_List_List*
_temp1769=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1741); for( 0; _temp1769 != 0;((( _temp1769=((
struct Cyc_List_List*) _check_null( _temp1769))->tl, _temp1768=(( struct Cyc_List_List*)
_check_null( _temp1768))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp1770=(
struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp1769))->hd;
if(( void*) _temp1770->r ==( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1771=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1768))->hd)).f2;
struct _tuple0* _temp1772= Cyc_Toc_temp_var(); void* _temp1773=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1770->topt))->v; void* _temp1774= Cyc_Toc_typ_to_c(
_temp1773); struct _tagged_arr* _temp1775= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1776= Cyc_Absyn_structarrow_exp( rcast, Cyc_Toc_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1771))){ _temp1776= Cyc_Absyn_cast_exp(
_temp1774, _temp1776, 0);} decls=({ struct Cyc_List_List* _temp1777=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1777->hd=( void*)({
struct _tuple13* _temp1778=( struct _tuple13*) _region_malloc( rgn, sizeof(
struct _tuple13)); _temp1778->f1= _temp1772; _temp1778->f2= _temp1774; _temp1778;});
_temp1777->tl= decls; _temp1777;});{ struct _tuple12 _temp1779= Cyc_Toc_xlate_pat(
nv, rgn, _temp1773, Cyc_Absyn_var_exp( _temp1772, 0), _temp1776, _temp1770,
succ_lab, fail_lab, decls); nv= _temp1779.f1; decls= _temp1779.f2;{ struct Cyc_Absyn_Stmt*
_temp1780= _temp1779.f3; struct Cyc_Absyn_Stmt* _temp1781= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1772, 0), _temp1776, 0); struct Cyc_Absyn_Stmt*
_temp1782= Cyc_Absyn_seq_stmt( _temp1781, _temp1780, 0); ss=({ struct Cyc_List_List*
_temp1783=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1783->hd=( void*) Cyc_Absyn_label_stmt( _temp1775, _temp1782, 0); _temp1783->tl=
ss; _temp1783;}); succ_lab= _temp1775;}}}} if( ss == 0){ ss=({ struct Cyc_List_List*
_temp1784=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1784->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1784->tl= 0;
_temp1784;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1747->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1745->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1785=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1747->fields))->v; for( 0; _temp1785 != 0; _temp1785=(( struct
Cyc_List_List*) _check_null( _temp1785))->tl){ struct Cyc_Absyn_Tunionfield*
_temp1786=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1785))->hd; if( _temp1786->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1745->name, 0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1673;}} _LL1695: _temp1751= _temp1749; goto _LL1697; _LL1697: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1751);{ struct Cyc_List_List* _temp1787=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1751); for(
0; _temp1787 != 0;( _temp1787=(( struct Cyc_List_List*) _check_null( _temp1787))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp1788=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp1787))->hd; if(( void*) _temp1788->r ==( void*) Cyc_Absyn_Wild_p){
continue;}{ struct _tuple0* _temp1789= Cyc_Toc_temp_var(); void* _temp1790=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1788->topt))->v; struct
_tagged_arr* _temp1791= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1792=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1792->hd=( void*)({ struct _tuple13* _temp1793=( struct _tuple13*)
_region_malloc( rgn, sizeof( struct _tuple13)); _temp1793->f1= _temp1789;
_temp1793->f2= Cyc_Toc_typ_to_c( _temp1790); _temp1793;}); _temp1792->tl= decls;
_temp1792;});{ struct _tuple12 _temp1794= Cyc_Toc_xlate_pat( nv, rgn, _temp1790,
Cyc_Absyn_var_exp( _temp1789, 0), Cyc_Absyn_structmember_exp( path, Cyc_Toc_fieldname(
cnt), 0), _temp1788, succ_lab, fail_lab, decls); nv= _temp1794.f1; decls=
_temp1794.f2;{ struct Cyc_Absyn_Stmt* _temp1795= _temp1794.f3; struct Cyc_Absyn_Stmt*
_temp1796= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1789, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp1795, 0); ss=({ struct Cyc_List_List* _temp1797=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1797->hd=( void*) Cyc_Absyn_label_stmt(
_temp1791, _temp1796, 0); _temp1797->tl= ss; _temp1797;}); succ_lab= _temp1791;}}}}}
if( ss == 0){ ss=({ struct Cyc_List_List* _temp1798=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1798->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1798->tl= 0; _temp1798;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1673;} _LL1699: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1799=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1753); for( 0; _temp1799 != 0; _temp1799=(( struct
Cyc_List_List*) _check_null( _temp1799))->tl){ struct _tuple14* _temp1800=(
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1799))->hd; struct
Cyc_Absyn_Pat* _temp1801=(* _temp1800).f2; if(( void*) _temp1801->r ==( void*)
Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1802=( void*)((
struct Cyc_List_List*) _check_null((* _temp1800).f1))->hd; struct _tagged_arr*
_temp1808; _LL1804: if(*(( int*) _temp1802) == Cyc_Absyn_FieldName){ _LL1809:
_temp1808=(( struct Cyc_Absyn_FieldName_struct*) _temp1802)->f1; goto _LL1805;}
else{ goto _LL1806;} _LL1806: goto _LL1807; _LL1805: f= _temp1808; goto _LL1803;
_LL1807:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1803:;}{ struct _tuple0*
_temp1810= Cyc_Toc_temp_var(); void* _temp1811=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1801->topt))->v; void* _temp1812= Cyc_Toc_typ_to_c( _temp1811);
struct _tagged_arr* _temp1813= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1814=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1814->hd=( void*)({ struct _tuple13* _temp1815=( struct _tuple13*)
_region_malloc( rgn, sizeof( struct _tuple13)); _temp1815->f1= _temp1810;
_temp1815->f2= _temp1812; _temp1815;}); _temp1814->tl= decls; _temp1814;});{
struct _tuple12 _temp1816= Cyc_Toc_xlate_pat( nv, rgn, _temp1811, Cyc_Absyn_var_exp(
_temp1810, 0), Cyc_Absyn_structmember_exp( path, f, 0), _temp1801, succ_lab,
fail_lab, decls); nv= _temp1816.f1; decls= _temp1816.f2;{ struct Cyc_Absyn_Exp*
_temp1817= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1759->fields))->v, f)))->type)){
_temp1817= Cyc_Absyn_cast_exp( _temp1812, _temp1817, 0);}{ struct Cyc_Absyn_Stmt*
_temp1818= _temp1816.f3; struct Cyc_Absyn_Stmt* _temp1819= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1810, 0), _temp1817, 0),
_temp1818, 0); ss=({ struct Cyc_List_List* _temp1820=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1820->hd=( void*) Cyc_Absyn_label_stmt(
_temp1813, _temp1819, 0); _temp1820->tl= ss; _temp1820;}); succ_lab= _temp1813;}}}}}}}
if( ss == 0){ ss=({ struct Cyc_List_List* _temp1821=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1821->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1821->tl= 0; _temp1821;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1673;} _LL1701: { struct _tuple0* _temp1822= Cyc_Toc_temp_var(); void*
_temp1823=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1761->topt))->v;
decls=({ struct Cyc_List_List* _temp1824=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1824->hd=( void*)({ struct _tuple13*
_temp1825=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp1825->f1= _temp1822; _temp1825->f2= Cyc_Toc_typ_to_c( _temp1823); _temp1825;});
_temp1824->tl= decls; _temp1824;});{ struct _tuple12 _temp1826= Cyc_Toc_xlate_pat(
nv, rgn, _temp1823, Cyc_Absyn_var_exp( _temp1822, 0), Cyc_Absyn_deref_exp( path,
0), _temp1761, succ_lab, fail_lab, decls); nv= _temp1826.f1; decls= _temp1826.f2;{
struct Cyc_Absyn_Stmt* _temp1827= _temp1826.f3; struct Cyc_Absyn_Stmt* _temp1828=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1822, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1827, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1828, 0);} else{ s= _temp1828;} goto _LL1673;}}} _LL1703: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid",
sizeof( unsigned char), 10u)); goto _LL1673; _LL1705: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall", sizeof(
unsigned char), 12u)); goto _LL1673; _LL1707: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields", sizeof(
unsigned char), 14u)); goto _LL1673; _LL1673:;} return({ struct _tuple12
_temp1829; _temp1829.f1= nv; _temp1829.f2= decls; _temp1829.f3= s; _temp1829;});}
struct _tuple15{ struct _tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple15* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple15* _temp1830=(
struct _tuple15*) _region_malloc( r, sizeof( struct _tuple15)); _temp1830->f1=
Cyc_Toc_fresh_label(); _temp1830->f2= Cyc_Toc_fresh_label(); _temp1830->f3= sc;
_temp1830;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp1831=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; int leave_as_switch;{ void* _temp1832= Cyc_Tcutil_compress(
_temp1831); _LL1834: if(( unsigned int) _temp1832 > 4u?*(( int*) _temp1832) ==
Cyc_Absyn_IntType: 0){ goto _LL1835;} else{ goto _LL1836;} _LL1836: goto _LL1837;
_LL1835: leave_as_switch= 1; goto _LL1833; _LL1837: leave_as_switch= 0; goto
_LL1833; _LL1833:;}{ struct Cyc_List_List* _temp1838= scs; for( 0; _temp1838 !=
0; _temp1838=(( struct Cyc_List_List*) _check_null( _temp1838))->tl){ if((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1838))->hd)->pat_vars))->v != 0? 1:((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp1838))->hd)->where_clause
!= 0){ leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp1839= scs; for( 0;
_temp1839 != 0; _temp1839=(( struct Cyc_List_List*) _check_null( _temp1839))->tl){
struct Cyc_Absyn_Stmt* _temp1840=(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp1839))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1839))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp1840, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp1840);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp1841= _new_region(); struct _RegionHandle* rgn=&
_temp1841; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple15*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1842= lscs; for( 0;
_temp1842 != 0; _temp1842=(( struct Cyc_List_List*) _check_null( _temp1842))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp1842))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp1842))->tl == 0? end_l:(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1842))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp1842))->hd)).f2; if( sc->where_clause == 0){ struct _tuple12
_temp1843= Cyc_Toc_xlate_pat( nv, rgn, _temp1831, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1844=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1844->hd=( void*)
_temp1843.f1; _temp1844->tl= nvs; _temp1844;}); decls= _temp1843.f2; test_stmts=({
struct Cyc_List_List* _temp1845=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1845->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp1842))->hd)).f1,
_temp1843.f3, 0); _temp1845->tl= test_stmts; _temp1845;});} else{ struct Cyc_Absyn_Exp*
_temp1846=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1847= Cyc_Toc_fresh_label(); struct _tuple12 _temp1848= Cyc_Toc_xlate_pat(
nv, rgn, _temp1831, r, path, sc->pattern, _temp1847, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1848.f1, _temp1846); nvs=({ struct Cyc_List_List* _temp1849=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1849->hd=( void*)
_temp1848.f1; _temp1849->tl= nvs; _temp1849;}); decls= _temp1848.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1846, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1847, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1850=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1850->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp1842))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp1848.f3, s3, 0), 0); _temp1850->tl= test_stmts;
_temp1850;});}}}}{ struct Cyc_Absyn_Stmt* _temp1851= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl != 0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp1852=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1852, end_l,(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp1853=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp1853, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp1851, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls != 0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple13 _temp1856; void* _temp1857; struct
_tuple0* _temp1859; struct _tuple13* _temp1854=( struct _tuple13*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp1856=* _temp1854; _LL1860: _temp1859= _temp1856.f1;
goto _LL1858; _LL1858: _temp1857= _temp1856.f2; goto _LL1855; _LL1855: res= Cyc_Absyn_declare_stmt(
_temp1859, _temp1857, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1861=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1861[ 0]=({ struct Cyc_List_List
_temp1862; _temp1862.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp1862.tl= 0; _temp1862;}); _temp1861;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1863=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1905; struct Cyc_Absyn_Stmt* _temp1907; struct Cyc_Absyn_Stmt* _temp1909;
struct Cyc_Absyn_Exp* _temp1911; struct Cyc_Absyn_Stmt* _temp1913; struct Cyc_Absyn_Stmt*
_temp1915; struct Cyc_Absyn_Exp* _temp1917; struct Cyc_Absyn_Stmt* _temp1919;
struct _tuple2 _temp1921; struct Cyc_Absyn_Exp* _temp1923; struct Cyc_Absyn_Stmt*
_temp1925; struct Cyc_Absyn_Stmt* _temp1927; struct Cyc_Absyn_Stmt* _temp1929;
struct Cyc_Absyn_Stmt* _temp1931; struct _tuple2 _temp1933; struct Cyc_Absyn_Exp*
_temp1935; struct _tuple2 _temp1937; struct Cyc_Absyn_Exp* _temp1939; struct Cyc_Absyn_Exp*
_temp1941; struct Cyc_List_List* _temp1943; struct Cyc_Absyn_Exp* _temp1945;
struct Cyc_Absyn_Switch_clause** _temp1947; struct Cyc_List_List* _temp1949;
struct Cyc_Absyn_Stmt* _temp1951; struct Cyc_Absyn_Decl* _temp1953; struct Cyc_Absyn_Stmt*
_temp1955; struct _tagged_arr* _temp1957; struct _tuple2 _temp1959; struct Cyc_Absyn_Exp*
_temp1961; struct Cyc_Absyn_Stmt* _temp1963; struct Cyc_List_List* _temp1965;
struct Cyc_Absyn_Stmt* _temp1967; struct Cyc_Absyn_Stmt* _temp1969; struct Cyc_Absyn_Vardecl*
_temp1971; struct Cyc_Absyn_Tvar* _temp1973; _LL1865: if( _temp1863 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1866;} else{ goto _LL1867;} _LL1867: if((
unsigned int) _temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Exp_s: 0){
_LL1906: _temp1905=(( struct Cyc_Absyn_Exp_s_struct*) _temp1863)->f1; goto
_LL1868;} else{ goto _LL1869;} _LL1869: if(( unsigned int) _temp1863 > 1u?*((
int*) _temp1863) == Cyc_Absyn_Seq_s: 0){ _LL1910: _temp1909=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1863)->f1; goto _LL1908; _LL1908: _temp1907=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1863)->f2; goto _LL1870;} else{ goto _LL1871;} _LL1871: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Return_s: 0){ _LL1912:
_temp1911=(( struct Cyc_Absyn_Return_s_struct*) _temp1863)->f1; goto _LL1872;}
else{ goto _LL1873;} _LL1873: if(( unsigned int) _temp1863 > 1u?*(( int*)
_temp1863) == Cyc_Absyn_IfThenElse_s: 0){ _LL1918: _temp1917=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1863)->f1; goto _LL1916; _LL1916: _temp1915=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1863)->f2; goto _LL1914; _LL1914: _temp1913=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1863)->f3; goto _LL1874;} else{ goto _LL1875;} _LL1875: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_While_s: 0){ _LL1922: _temp1921=((
struct Cyc_Absyn_While_s_struct*) _temp1863)->f1; _LL1924: _temp1923= _temp1921.f1;
goto _LL1920; _LL1920: _temp1919=(( struct Cyc_Absyn_While_s_struct*) _temp1863)->f2;
goto _LL1876;} else{ goto _LL1877;} _LL1877: if(( unsigned int) _temp1863 > 1u?*((
int*) _temp1863) == Cyc_Absyn_Break_s: 0){ _LL1926: _temp1925=(( struct Cyc_Absyn_Break_s_struct*)
_temp1863)->f1; goto _LL1878;} else{ goto _LL1879;} _LL1879: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Continue_s: 0){ _LL1928:
_temp1927=(( struct Cyc_Absyn_Continue_s_struct*) _temp1863)->f1; goto _LL1880;}
else{ goto _LL1881;} _LL1881: if(( unsigned int) _temp1863 > 1u?*(( int*)
_temp1863) == Cyc_Absyn_Goto_s: 0){ _LL1930: _temp1929=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1863)->f2; goto _LL1882;} else{ goto _LL1883;} _LL1883: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_For_s: 0){ _LL1942: _temp1941=((
struct Cyc_Absyn_For_s_struct*) _temp1863)->f1; goto _LL1938; _LL1938: _temp1937=((
struct Cyc_Absyn_For_s_struct*) _temp1863)->f2; _LL1940: _temp1939= _temp1937.f1;
goto _LL1934; _LL1934: _temp1933=(( struct Cyc_Absyn_For_s_struct*) _temp1863)->f3;
_LL1936: _temp1935= _temp1933.f1; goto _LL1932; _LL1932: _temp1931=(( struct Cyc_Absyn_For_s_struct*)
_temp1863)->f4; goto _LL1884;} else{ goto _LL1885;} _LL1885: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Switch_s: 0){ _LL1946:
_temp1945=(( struct Cyc_Absyn_Switch_s_struct*) _temp1863)->f1; goto _LL1944;
_LL1944: _temp1943=(( struct Cyc_Absyn_Switch_s_struct*) _temp1863)->f2; goto
_LL1886;} else{ goto _LL1887;} _LL1887: if(( unsigned int) _temp1863 > 1u?*((
int*) _temp1863) == Cyc_Absyn_Fallthru_s: 0){ _LL1950: _temp1949=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1863)->f1; goto _LL1948; _LL1948: _temp1947=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1863)->f2; goto _LL1888;} else{ goto _LL1889;} _LL1889: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Decl_s: 0){ _LL1954: _temp1953=((
struct Cyc_Absyn_Decl_s_struct*) _temp1863)->f1; goto _LL1952; _LL1952:
_temp1951=(( struct Cyc_Absyn_Decl_s_struct*) _temp1863)->f2; goto _LL1890;}
else{ goto _LL1891;} _LL1891: if(( unsigned int) _temp1863 > 1u?*(( int*)
_temp1863) == Cyc_Absyn_Label_s: 0){ _LL1958: _temp1957=(( struct Cyc_Absyn_Label_s_struct*)
_temp1863)->f1; goto _LL1956; _LL1956: _temp1955=(( struct Cyc_Absyn_Label_s_struct*)
_temp1863)->f2; goto _LL1892;} else{ goto _LL1893;} _LL1893: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Do_s: 0){ _LL1964: _temp1963=((
struct Cyc_Absyn_Do_s_struct*) _temp1863)->f1; goto _LL1960; _LL1960: _temp1959=((
struct Cyc_Absyn_Do_s_struct*) _temp1863)->f2; _LL1962: _temp1961= _temp1959.f1;
goto _LL1894;} else{ goto _LL1895;} _LL1895: if(( unsigned int) _temp1863 > 1u?*((
int*) _temp1863) == Cyc_Absyn_TryCatch_s: 0){ _LL1968: _temp1967=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1863)->f1; goto _LL1966; _LL1966: _temp1965=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1863)->f2; goto _LL1896;} else{ goto _LL1897;} _LL1897: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Region_s: 0){ _LL1974:
_temp1973=(( struct Cyc_Absyn_Region_s_struct*) _temp1863)->f1; goto _LL1972;
_LL1972: _temp1971=(( struct Cyc_Absyn_Region_s_struct*) _temp1863)->f2; goto
_LL1970; _LL1970: _temp1969=(( struct Cyc_Absyn_Region_s_struct*) _temp1863)->f3;
goto _LL1898;} else{ goto _LL1899;} _LL1899: if(( unsigned int) _temp1863 > 1u?*((
int*) _temp1863) == Cyc_Absyn_SwitchC_s: 0){ goto _LL1900;} else{ goto _LL1901;}
_LL1901: if(( unsigned int) _temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Cut_s:
0){ goto _LL1902;} else{ goto _LL1903;} _LL1903: if(( unsigned int) _temp1863 >
1u?*(( int*) _temp1863) == Cyc_Absyn_Splice_s: 0){ goto _LL1904;} else{ goto
_LL1864;} _LL1866: return; _LL1868: Cyc_Toc_exp_to_c( nv, _temp1905); return;
_LL1870: Cyc_Toc_stmt_to_c( nv, _temp1909); s= _temp1907; continue; _LL1872: {
struct Cyc_Core_Opt* topt= 0; if( _temp1911 != 0){ topt=({ struct Cyc_Core_Opt*
_temp1975=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1975->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp1911))->topt))->v);
_temp1975;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp1911));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp1976=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp1977= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp1976, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp1976,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp1911, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp1977, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1874: Cyc_Toc_exp_to_c( nv, _temp1917); Cyc_Toc_stmt_to_c( nv,
_temp1915); s= _temp1913; continue; _LL1876: Cyc_Toc_exp_to_c( nv, _temp1923);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1919); return; _LL1878: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{ int
dest_depth= _temp1925 == 0? 0:(( struct Cyc_Absyn_Stmt*) _check_null( _temp1925))->try_depth;
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL1880: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp1929=
_temp1927; goto _LL1882; _LL1882: Cyc_Toc_do_npop_before( s->try_depth -((
struct Cyc_Absyn_Stmt*) _check_null( _temp1929))->try_depth, s); return; _LL1884:
Cyc_Toc_exp_to_c( nv, _temp1941); Cyc_Toc_exp_to_c( nv, _temp1939); Cyc_Toc_exp_to_c(
nv, _temp1935); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1931); return;
_LL1886: Cyc_Toc_xlate_switch( nv, s, _temp1945, _temp1943); return; _LL1888:
if( nv->fallthru_info == 0){( int) _throw( Cyc_Toc_toc_impos( _tag_arr("fallthru in unexpected place",
sizeof( unsigned char), 29u)));}{ struct _tuple6 _temp1980; struct Cyc_Dict_Dict*
_temp1981; struct Cyc_List_List* _temp1983; struct _tagged_arr* _temp1985;
struct _tuple6* _temp1978=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp1980=* _temp1978; _LL1986: _temp1985= _temp1980.f1;
goto _LL1984; _LL1984: _temp1983= _temp1980.f2; goto _LL1982; _LL1982: _temp1981=
_temp1980.f3; goto _LL1979; _LL1979: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp1985, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp1947)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp1987=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1983); struct Cyc_List_List* _temp1988=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp1949); for( 0; _temp1987 != 0;( _temp1987=((
struct Cyc_List_List*) _check_null( _temp1987))->tl, _temp1988=(( struct Cyc_List_List*)
_check_null( _temp1988))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1988))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp1981,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp1987))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1988))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}} _LL1890:{ void* _temp1989=( void*) _temp1953->r;
struct Cyc_Absyn_Vardecl* _temp1999; int _temp2001; struct Cyc_Absyn_Exp*
_temp2003; struct Cyc_Core_Opt* _temp2005; struct Cyc_Absyn_Pat* _temp2007;
struct Cyc_List_List* _temp2009; _LL1991: if(*(( int*) _temp1989) == Cyc_Absyn_Var_d){
_LL2000: _temp1999=(( struct Cyc_Absyn_Var_d_struct*) _temp1989)->f1; goto
_LL1992;} else{ goto _LL1993;} _LL1993: if(*(( int*) _temp1989) == Cyc_Absyn_Let_d){
_LL2008: _temp2007=(( struct Cyc_Absyn_Let_d_struct*) _temp1989)->f1; goto
_LL2006; _LL2006: _temp2005=(( struct Cyc_Absyn_Let_d_struct*) _temp1989)->f3;
goto _LL2004; _LL2004: _temp2003=(( struct Cyc_Absyn_Let_d_struct*) _temp1989)->f4;
goto _LL2002; _LL2002: _temp2001=(( struct Cyc_Absyn_Let_d_struct*) _temp1989)->f5;
goto _LL1994;} else{ goto _LL1995;} _LL1995: if(*(( int*) _temp1989) == Cyc_Absyn_Letv_d){
_LL2010: _temp2009=(( struct Cyc_Absyn_Letv_d_struct*) _temp1989)->f1; goto
_LL1996;} else{ goto _LL1997;} _LL1997: goto _LL1998; _LL1992: { struct Cyc_Toc_Env*
_temp2011= Cyc_Toc_add_varmap( nv, _temp1999->name, Cyc_Absyn_varb_exp(
_temp1999->name,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp2012=( struct
Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2012[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2013; _temp2013.tag= Cyc_Absyn_Local_b;
_temp2013.f1= _temp1999; _temp2013;}); _temp2012;}), 0)); Cyc_Toc_local_decl_to_c(
_temp2011, _temp2011, _temp1999, _temp1951); goto _LL1990;} _LL1994:{ void*
_temp2014=( void*) _temp2007->r; struct Cyc_Absyn_Vardecl* _temp2020; _LL2016:
if(( unsigned int) _temp2014 > 2u?*(( int*) _temp2014) == Cyc_Absyn_Var_p: 0){
_LL2021: _temp2020=(( struct Cyc_Absyn_Var_p_struct*) _temp2014)->f1; goto
_LL2017;} else{ goto _LL2018;} _LL2018: goto _LL2019; _LL2017: { struct _tuple0*
old_name= _temp2020->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp2020->name= new_name; _temp2020->initializer=( struct Cyc_Absyn_Exp*)
_temp2003;( void*)( _temp1953->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2022=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2022[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2023; _temp2023.tag= Cyc_Absyn_Var_d;
_temp2023.f1= _temp2020; _temp2023;}); _temp2022;})));{ struct Cyc_Toc_Env*
_temp2024= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2025=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2025[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2026; _temp2026.tag= Cyc_Absyn_Local_b; _temp2026.f1=
_temp2020; _temp2026;}); _temp2025;}), 0)); Cyc_Toc_local_decl_to_c( _temp2024,
nv, _temp2020, _temp1951); goto _LL2015;}} _LL2019:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp2007, _temp2005, _temp2003, _temp2001,
_temp1951))->r)); goto _LL2015; _LL2015:;} goto _LL1990; _LL1996: { struct Cyc_List_List*
_temp2027=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2009); if( _temp2027 == 0){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("empty Letv_d", sizeof( unsigned char), 13u));}( void*)( _temp1953->r=(
void*)(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2028=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2028[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2029; _temp2029.tag= Cyc_Absyn_Var_d; _temp2029.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2027))->hd; _temp2029;}); _temp2028;})));
_temp2027=(( struct Cyc_List_List*) _check_null( _temp2027))->tl; for( 0;
_temp2027 != 0; _temp2027=(( struct Cyc_List_List*) _check_null( _temp2027))->tl){
struct Cyc_Absyn_Decl* _temp2030= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2031=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2031[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2032; _temp2032.tag= Cyc_Absyn_Var_d;
_temp2032.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2027))->hd; _temp2032;}); _temp2031;}), 0);( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_decl_stmt( _temp2030, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));}
Cyc_Toc_stmt_to_c( nv, s); goto _LL1990;} _LL1998:(( int(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u)); goto _LL1990; _LL1990:;} return; _LL1892: s= _temp1955;
continue; _LL1894: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1963); Cyc_Toc_exp_to_c(
nv, _temp1961); return; _LL1896: { struct _tuple0* h_var= Cyc_Toc_temp_var();
struct _tuple0* e_var= Cyc_Toc_temp_var(); struct _tuple0* was_thrown_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp2033=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2033->v=( void*) e_typ; _temp2033;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1967);{
struct Cyc_Absyn_Stmt* _temp2034= Cyc_Absyn_seq_stmt( _temp1967, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp2035= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp2036= Cyc_Absyn_var_exp(
_temp2035, 0); struct Cyc_Absyn_Vardecl* _temp2037= Cyc_Absyn_new_vardecl(
_temp2035, Cyc_Absyn_exn_typ, 0); _temp2036->topt=({ struct Cyc_Core_Opt*
_temp2038=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2038->v=( void*) Cyc_Absyn_exn_typ; _temp2038;});{ struct Cyc_Absyn_Pat*
_temp2039=({ struct Cyc_Absyn_Pat* _temp2055=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2055->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2057=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2057[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2058; _temp2058.tag= Cyc_Absyn_Var_p;
_temp2058.f1= _temp2037; _temp2058;}); _temp2057;})); _temp2055->topt=({ struct
Cyc_Core_Opt* _temp2056=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2056->v=( void*) Cyc_Absyn_exn_typ; _temp2056;}); _temp2055->loc= 0;
_temp2055;}); struct Cyc_Absyn_Exp* _temp2040= Cyc_Absyn_throw_exp( _temp2036, 0);
_temp2040->topt=({ struct Cyc_Core_Opt* _temp2041=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2041->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp2041;});{ struct Cyc_Absyn_Stmt* _temp2042= Cyc_Absyn_exp_stmt( _temp2040,
0); struct Cyc_Absyn_Switch_clause* _temp2043=({ struct Cyc_Absyn_Switch_clause*
_temp2052=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2052->pattern= _temp2039; _temp2052->pat_vars=({ struct Cyc_Core_Opt*
_temp2053=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2053->v=( void*)({ struct Cyc_List_List* _temp2054=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2054->hd=( void*) _temp2037;
_temp2054->tl= 0; _temp2054;}); _temp2053;}); _temp2052->where_clause= 0;
_temp2052->body= _temp2042; _temp2052->loc= 0; _temp2052;}); struct Cyc_Absyn_Stmt*
_temp2044= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1965,({ struct Cyc_List_List*
_temp2051=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2051->hd=( void*) _temp2043; _temp2051->tl= 0; _temp2051;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp2044);{ struct Cyc_Absyn_Exp* _temp2045= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp2050=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2050->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2050->tl= 0; _temp2050;}), 0); struct Cyc_Absyn_Stmt*
_temp2046= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp2049=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2049->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp2049->tl= 0; _temp2049;}), 0), 0); struct Cyc_Absyn_Exp* _temp2047= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp2048= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2046, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2047, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2045, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2048, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2034, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp2044, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL1898: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp1971->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp1969);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_List_List*(* _temp2059)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp2062= x_exp; struct Cyc_Absyn_Exp* _temp2060[ 1u]={ _temp2062}; struct
_tagged_arr _temp2061={( void*) _temp2060,( void*) _temp2060,( void*)( _temp2060
+ 1u)}; _temp2059( _temp2061);}), 0), 0), Cyc_Absyn_seq_stmt( _temp1969, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({ struct Cyc_List_List*(* _temp2063)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp2066= x_exp; struct Cyc_Absyn_Exp* _temp2064[ 1u]={
_temp2066}; struct _tagged_arr _temp2065={( void*) _temp2064,( void*) _temp2064,(
void*)( _temp2064 + 1u)}; _temp2063( _temp2065);}), 0), 0), 0), 0), 0), 0))->r));
return;} _LL1900:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("switchC_s",
sizeof( unsigned char), 10u)); return; _LL1902:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("cut", sizeof( unsigned char), 4u)); return; _LL1904:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof(
unsigned char), 7u)); return; _LL1864:;}} struct _tuple16{ struct _tagged_arr*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0; f->rgn_po=
0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* _temp2067= f->args; for( 0; _temp2067 != 0; _temp2067=((
struct Cyc_List_List*) _check_null( _temp2067))->tl){(*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( _temp2067))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2067))->hd)).f3);{
struct _tuple0* _temp2068=({ struct _tuple0* _temp2069=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2069->f1=( void*) Cyc_Absyn_Loc_n;
_temp2069->f2=(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2067))->hd)).f1; _temp2069;}); nv= Cyc_Toc_add_varmap( nv, _temp2068, Cyc_Absyn_var_exp(
_temp2068, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name != 0: 0){ int _temp2072; void* _temp2074;
void* _temp2076; struct Cyc_Absyn_Tqual _temp2078; struct Cyc_Core_Opt*
_temp2080; struct Cyc_Absyn_VarargInfo _temp2070=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs)); _LL2081: _temp2080= _temp2070.name; goto _LL2079;
_LL2079: _temp2078= _temp2070.tq; goto _LL2077; _LL2077: _temp2076=( void*)
_temp2070.type; goto _LL2075; _LL2075: _temp2074=( void*) _temp2070.rgn; goto
_LL2073; _LL2073: _temp2072= _temp2070.inject; goto _LL2071; _LL2071: { void*
_temp2082= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp2076, _temp2074,
_temp2078)); struct _tuple0* _temp2083=({ struct _tuple0* _temp2086=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2086->f1=( void*) Cyc_Absyn_Loc_n;
_temp2086->f2=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
_temp2080))->v; _temp2086;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2084=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2084->hd=( void*)({ struct _tuple16* _temp2085=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp2085->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp2080))->v; _temp2085->f2= _temp2078;
_temp2085->f3= _temp2082; _temp2085;}); _temp2084->tl= 0; _temp2084;})); nv= Cyc_Toc_add_varmap(
nv, _temp2083, Cyc_Absyn_var_exp( _temp2083, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp2087=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp2087 != 0; _temp2087=(( struct
Cyc_List_List*) _check_null( _temp2087))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2087))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2087))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2088= s; _LL2090: if(
_temp2088 ==( void*) Cyc_Absyn_Abstract){ goto _LL2091;} else{ goto _LL2092;}
_LL2092: if( _temp2088 ==( void*) Cyc_Absyn_ExternC){ goto _LL2093;} else{ goto
_LL2094;} _LL2094: goto _LL2095; _LL2091: return( void*) Cyc_Absyn_Public;
_LL2093: return( void*) Cyc_Absyn_Extern; _LL2095: return s; _LL2089:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2096=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 0){
Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2097=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2097->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2097;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2098=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2096).f2); if( _temp2098 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2096).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2098))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2096).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2099=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2099->v=(
void*) _temp2096; _temp2099;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2100=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp2100 != 0; _temp2100=(( struct Cyc_List_List*)
_check_null( _temp2100))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2100))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2100))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp2101=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far == 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp2102=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2102->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2102;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2103=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2101).f2); if( _temp2103 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2101).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2103))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp2101).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2104=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2104->v=(
void*) _temp2101; _temp2104;}); if( u->fields != 0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp2105=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2105 != 0; _temp2105=(( struct Cyc_List_List*)
_check_null( _temp2105))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2105))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2105))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2106=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2106->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2106;});}{ struct
_tuple0* _temp2107= tud->name; if( tud->fields == 0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp2107)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2107));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp2108=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp2108 != 0; _temp2108=(( struct Cyc_List_List*) _check_null(
_temp2108))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2108))->hd; if( f->typs == 0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2109; _temp2109.q_const= 1;
_temp2109.q_volatile= 0; _temp2109.q_restrict= 0; _temp2109;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2110=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2110->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2111=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2111[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2112; _temp2112.tag= Cyc_Absyn_Var_d; _temp2112.f1= vd; _temp2112;});
_temp2111;}), 0); _temp2110->tl= Cyc_Toc_result_decls; _temp2110;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2113; _temp2113.q_const=
1; _temp2113.q_volatile= 0; _temp2113.q_restrict= 0; _temp2113;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2114=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2114->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2115=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2115[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2116; _temp2116.tag= Cyc_Absyn_Var_d; _temp2116.f1= vd; _temp2116;});
_temp2115;}), 0); _temp2114->tl= Cyc_Toc_result_decls; _temp2114;});{ struct Cyc_List_List*
_temp2117= 0; int i= 1;{ struct Cyc_List_List* _temp2118= f->typs; for( 0;
_temp2118 != 0;( _temp2118=(( struct Cyc_List_List*) _check_null( _temp2118))->tl,
i ++)){ struct _tagged_arr* _temp2119= Cyc_Toc_fieldname( i); struct Cyc_Absyn_Structfield*
_temp2120=({ struct Cyc_Absyn_Structfield* _temp2122=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2122->name= _temp2119;
_temp2122->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2118))->hd)).f1; _temp2122->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2118))->hd)).f2);
_temp2122->width= 0; _temp2122->attributes= 0; _temp2122;}); _temp2117=({ struct
Cyc_List_List* _temp2121=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2121->hd=( void*) _temp2120; _temp2121->tl= _temp2117; _temp2121;});}}
_temp2117=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2117); _temp2117=({ struct Cyc_List_List* _temp2123=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2123->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2124=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2124->name= Cyc_Toc_tag_sp; _temp2124->tq= Cyc_Toc_mt_tq; _temp2124->type=(
void*) Cyc_Absyn_sint_t; _temp2124->width= 0; _temp2124->attributes= 0;
_temp2124;}); _temp2123->tl= _temp2117; _temp2123;});{ struct Cyc_Absyn_Structdecl*
_temp2125=({ struct Cyc_Absyn_Structdecl* _temp2129=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2129->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2129->name=({ struct Cyc_Core_Opt* _temp2131=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2131->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2131;}); _temp2129->tvs= 0; _temp2129->fields=({ struct Cyc_Core_Opt*
_temp2130=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2130->v=( void*) _temp2117; _temp2130;}); _temp2129->attributes= 0;
_temp2129;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2126=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2126->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2127=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2127[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2128; _temp2128.tag= Cyc_Absyn_Struct_d;
_temp2128.f1= _temp2125; _temp2128;}); _temp2127;}), 0); _temp2126->tl= Cyc_Toc_result_decls;
_temp2126;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2132=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2132->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2132;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2133= xd->name; struct Cyc_List_List*
_temp2134=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2134 != 0; _temp2134=(( struct Cyc_List_List*) _check_null(
_temp2134))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2134))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2135= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) + 4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2136=({ struct
Cyc_Absyn_ArrayType_struct* _temp2182=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2182[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2183; _temp2183.tag= Cyc_Absyn_ArrayType;
_temp2183.f1=( void*) Cyc_Absyn_uchar_t; _temp2183.f2= Cyc_Toc_mt_tq; _temp2183.f3=(
struct Cyc_Absyn_Exp*) _temp2135; _temp2183;}); _temp2182;}); struct Cyc_Core_Opt*
_temp2137=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2145; int _temp2146; _LL2139: if( _temp2137 == 0){ goto _LL2140;} else{
goto _LL2141;} _LL2141: if( _temp2137 == 0){ goto _LL2143;} else{ _temp2145=*
_temp2137; _LL2147: _temp2146=( int) _temp2145.v; if( _temp2146 == 0){ goto
_LL2142;} else{ goto _LL2143;}} _LL2143: goto _LL2144; _LL2140: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp(( struct _tagged_arr)({ int _temp2148= 0; int _temp2149= 0;
int _temp2150= 0; int _temp2151= 0; struct _tagged_arr _temp2152=* fn; xprintf("%c%c%c%c%.*s",
_temp2148, _temp2149, _temp2150, _temp2151, _get_arr_size( _temp2152, 1u),
_temp2152.curr);}), 0);}{ struct Cyc_Absyn_Vardecl* _temp2153= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2136, initopt);( void*)( _temp2153->sc=( void*)(( void*) f->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2154=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2154->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2155=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2155[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2156; _temp2156.tag= Cyc_Absyn_Var_d; _temp2156.f1= _temp2153; _temp2156;});
_temp2155;}), 0); _temp2154->tl= Cyc_Toc_result_decls; _temp2154;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*) Cyc_Absyn_Extern));
if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2157= f->typs; for( 0; _temp2157 != 0;( _temp2157=(( struct Cyc_List_List*)
_check_null( _temp2157))->tl, i ++)){ struct _tagged_arr* _temp2158=({ struct
_tagged_arr* _temp2162=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2162[ 0]=( struct _tagged_arr) xprintf("f%d", i); _temp2162;});
struct Cyc_Absyn_Structfield* _temp2159=({ struct Cyc_Absyn_Structfield*
_temp2161=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2161->name= _temp2158; _temp2161->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2157))->hd)).f1; _temp2161->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2157))->hd)).f2);
_temp2161->width= 0; _temp2161->attributes= 0; _temp2161;}); fields=({ struct
Cyc_List_List* _temp2160=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2160->hd=( void*) _temp2159; _temp2160->tl= fields; _temp2160;});}} fields=({
struct Cyc_List_List* _temp2163=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2163->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2164=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2164->name= Cyc_Toc_tag_sp; _temp2164->tq= Cyc_Toc_mt_tq; _temp2164->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2164->width=
0; _temp2164->attributes= 0; _temp2164;}); _temp2163->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2163;});{ struct Cyc_Absyn_Structdecl*
_temp2165=({ struct Cyc_Absyn_Structdecl* _temp2169=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2169->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2169->name=({ struct Cyc_Core_Opt* _temp2171=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2171->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2171;}); _temp2169->tvs= 0; _temp2169->fields=({ struct Cyc_Core_Opt*
_temp2170=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2170->v=( void*) fields; _temp2170;}); _temp2169->attributes= 0; _temp2169;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2166=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2166->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2167=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2167[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2168; _temp2168.tag= Cyc_Absyn_Struct_d;
_temp2168.f1= _temp2165; _temp2168;}); _temp2167;}), 0); _temp2166->tl= Cyc_Toc_result_decls;
_temp2166;});}} goto _LL2138;}} _LL2142: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2172= Cyc_Absyn_string_exp(( struct _tagged_arr)({
int _temp2177= 0; int _temp2178= 0; int _temp2179= 0; int _temp2180= 0; struct
_tagged_arr _temp2181=* fn; xprintf("%c%c%c%c%.*s", _temp2177, _temp2178,
_temp2179, _temp2180, _get_arr_size( _temp2181, 1u), _temp2181.curr);}), 0);
struct Cyc_Absyn_Vardecl* _temp2173= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2136,( struct Cyc_Absyn_Exp*) _temp2172);( void*)( _temp2173->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2174=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2174->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2175=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2175[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2176; _temp2176.tag= Cyc_Absyn_Var_d;
_temp2176.f1= _temp2173; _temp2176;}); _temp2175;}), 0); _temp2174->tl= Cyc_Toc_result_decls;
_temp2174;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2138;
_LL2144: goto _LL2138; _LL2138:;}}} static void Cyc_Toc_enumdecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2184= ed->name;(
void*)( ed->sc=( void*)(( void*) Cyc_Absyn_Public)); if( ed->fields != 0){
struct Cyc_List_List* _temp2185=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; _temp2185 != 0; _temp2185=(( struct Cyc_List_List*)
_check_null( _temp2185))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2185))->hd)->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2185))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2185))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer != 0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2186=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2192; struct Cyc_Absyn_Exp* _temp2194; struct Cyc_Absyn_Vardecl* _temp2196;
_LL2188: if(*(( int*) _temp2186) == Cyc_Absyn_Comprehension_e){ _LL2197:
_temp2196=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2186)->f1; goto
_LL2195; _LL2195: _temp2194=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2186)->f2; goto _LL2193; _LL2193: _temp2192=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2186)->f3; goto _LL2189;} else{ goto _LL2190;} _LL2190: goto _LL2191;
_LL2189: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2196, _temp2194, _temp2192, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2187; _LL2191: Cyc_Toc_exp_to_c( init_nv,
init); goto _LL2187; _LL2187:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2198= _new_region(); struct _RegionHandle* prgn=& _temp2198;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2201; struct Cyc_List_List*
_temp2203; struct Cyc_Toc_Env* _temp2205; struct _tuple12 _temp2199= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2206: _temp2205= _temp2199.f1; goto _LL2204; _LL2204: _temp2203=
_temp2199.f2; goto _LL2202; _LL2202: _temp2201= _temp2199.f3; goto _LL2200;
_LL2200: Cyc_Toc_stmt_to_c( _temp2205, s);{ struct Cyc_Absyn_Stmt* _temp2207=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2201, _temp2207, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2208= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2209= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2208, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2201,
Cyc_Absyn_seq_stmt( _temp2209, _temp2207, 0), 0), 0);} for( 0; _temp2203 != 0;
_temp2203=(( struct Cyc_List_List*) _check_null( _temp2203))->tl){ struct
_tuple13 _temp2212; void* _temp2213; struct _tuple0* _temp2215; struct _tuple13*
_temp2210=( struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp2203))->hd;
_temp2212=* _temp2210; _LL2216: _temp2215= _temp2212.f1; goto _LL2214; _LL2214:
_temp2213= _temp2212.f2; goto _LL2211; _LL2211: s= Cyc_Absyn_declare_stmt(
_temp2215, _temp2213, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u));}{ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2217=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2243; struct Cyc_Absyn_Fndecl* _temp2245; struct
Cyc_Absyn_Structdecl* _temp2247; struct Cyc_Absyn_Uniondecl* _temp2249; struct
Cyc_Absyn_Tuniondecl* _temp2251; struct Cyc_Absyn_Enumdecl* _temp2253; struct
Cyc_Absyn_Typedefdecl* _temp2255; struct Cyc_List_List* _temp2257; struct Cyc_List_List*
_temp2259; struct Cyc_List_List* _temp2261; _LL2219: if(*(( int*) _temp2217) ==
Cyc_Absyn_Var_d){ _LL2244: _temp2243=(( struct Cyc_Absyn_Var_d_struct*)
_temp2217)->f1; goto _LL2220;} else{ goto _LL2221;} _LL2221: if(*(( int*)
_temp2217) == Cyc_Absyn_Fn_d){ _LL2246: _temp2245=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2217)->f1; goto _LL2222;} else{ goto _LL2223;} _LL2223: if(*(( int*)
_temp2217) == Cyc_Absyn_Let_d){ goto _LL2224;} else{ goto _LL2225;} _LL2225: if(*((
int*) _temp2217) == Cyc_Absyn_Letv_d){ goto _LL2226;} else{ goto _LL2227;}
_LL2227: if(*(( int*) _temp2217) == Cyc_Absyn_Struct_d){ _LL2248: _temp2247=((
struct Cyc_Absyn_Struct_d_struct*) _temp2217)->f1; goto _LL2228;} else{ goto
_LL2229;} _LL2229: if(*(( int*) _temp2217) == Cyc_Absyn_Union_d){ _LL2250:
_temp2249=(( struct Cyc_Absyn_Union_d_struct*) _temp2217)->f1; goto _LL2230;}
else{ goto _LL2231;} _LL2231: if(*(( int*) _temp2217) == Cyc_Absyn_Tunion_d){
_LL2252: _temp2251=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2217)->f1; goto
_LL2232;} else{ goto _LL2233;} _LL2233: if(*(( int*) _temp2217) == Cyc_Absyn_Enum_d){
_LL2254: _temp2253=(( struct Cyc_Absyn_Enum_d_struct*) _temp2217)->f1; goto
_LL2234;} else{ goto _LL2235;} _LL2235: if(*(( int*) _temp2217) == Cyc_Absyn_Typedef_d){
_LL2256: _temp2255=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2217)->f1; goto
_LL2236;} else{ goto _LL2237;} _LL2237: if(*(( int*) _temp2217) == Cyc_Absyn_Namespace_d){
_LL2258: _temp2257=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2217)->f2; goto
_LL2238;} else{ goto _LL2239;} _LL2239: if(*(( int*) _temp2217) == Cyc_Absyn_Using_d){
_LL2260: _temp2259=(( struct Cyc_Absyn_Using_d_struct*) _temp2217)->f2; goto
_LL2240;} else{ goto _LL2241;} _LL2241: if(*(( int*) _temp2217) == Cyc_Absyn_ExternC_d){
_LL2262: _temp2261=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2217)->f1; goto
_LL2242;} else{ goto _LL2218;} _LL2220: { struct _tuple0* _temp2263= _temp2243->name;
if(( void*) _temp2243->sc ==( void*) Cyc_Absyn_ExternC){ _temp2263=({ struct
_tuple0* _temp2264=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2264->f1= Cyc_Absyn_rel_ns_null; _temp2264->f2=(* _temp2263).f2; _temp2264;});}
if( _temp2243->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2243->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2243->name,
Cyc_Absyn_varb_exp( _temp2263,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2265=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2265[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2266; _temp2266.tag= Cyc_Absyn_Global_b;
_temp2266.f1= _temp2243; _temp2266;}); _temp2265;}), 0)); _temp2243->name=
_temp2263;( void*)( _temp2243->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2243->sc));(
void*)( _temp2243->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2243->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2267=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2267->hd=( void*) d; _temp2267->tl=
Cyc_Toc_result_decls; _temp2267;}); goto _LL2218;} _LL2222: { struct _tuple0*
_temp2268= _temp2245->name; if(( void*) _temp2245->sc ==( void*) Cyc_Absyn_ExternC){
_temp2268=({ struct _tuple0* _temp2269=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2269->f1= Cyc_Absyn_rel_ns_null; _temp2269->f2=(*
_temp2268).f2; _temp2269;});} nv= Cyc_Toc_add_varmap( nv, _temp2245->name, Cyc_Absyn_var_exp(
_temp2268, 0)); _temp2245->name= _temp2268; Cyc_Toc_fndecl_to_c( nv, _temp2245);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2270=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2270->hd=( void*) d; _temp2270->tl=
Cyc_Toc_result_decls; _temp2270;}); goto _LL2218;} _LL2224: goto _LL2226;
_LL2226:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel",
sizeof( unsigned char), 20u)); goto _LL2218; _LL2228: Cyc_Toc_structdecl_to_c(
_temp2247); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2271=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2271->hd=( void*) d; _temp2271->tl=
Cyc_Toc_result_decls; _temp2271;}); goto _LL2218; _LL2230: Cyc_Toc_uniondecl_to_c(
_temp2249); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2272=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2272->hd=( void*) d; _temp2272->tl=
Cyc_Toc_result_decls; _temp2272;}); goto _LL2218; _LL2232: if( _temp2251->is_xtunion){
Cyc_Toc_xtuniondecl_to_c( _temp2251);} else{ Cyc_Toc_tuniondecl_to_c( _temp2251);}
goto _LL2218; _LL2234: Cyc_Toc_enumdecl_to_c( nv, _temp2253); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2273=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2273->hd=( void*) d; _temp2273->tl= Cyc_Toc_result_decls;
_temp2273;}); goto _LL2218; _LL2236: _temp2255->name= _temp2255->name; _temp2255->tvs=
0;( void*)( _temp2255->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2255->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2274=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2274->hd=( void*) d; _temp2274->tl=
Cyc_Toc_result_decls; _temp2274;}); goto _LL2218; _LL2238: _temp2259= _temp2257;
goto _LL2240; _LL2240: _temp2261= _temp2259; goto _LL2242; _LL2242: nv= Cyc_Toc_decls_to_c(
nv, _temp2261, top); goto _LL2218; _LL2218:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}