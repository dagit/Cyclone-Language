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
_temp344; _temp411;}); _temp410;}); _LL260: { struct Cyc_List_List* _temp412= 0;
for( 0; _temp352 != 0; _temp352=(( struct Cyc_List_List*) _check_null( _temp352))->tl){
void* _temp413=( void*)(( struct Cyc_List_List*) _check_null( _temp352))->hd;
_LL415: if( _temp413 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL416;} else{
goto _LL417;} _LL417: if( _temp413 ==( void*) Cyc_Absyn_Const_att){ goto _LL418;}
else{ goto _LL419;} _LL419: goto _LL420; _LL416: goto _LL418; _LL418: continue;
_LL420: _temp412=({ struct Cyc_List_List* _temp421=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp421->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( _temp352))->hd); _temp421->tl= _temp412;
_temp421;}); goto _LL414; _LL414:;}{ struct Cyc_List_List* _temp422=(( struct
Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp360); if( _temp356 != 0){ void*
_temp423= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp356))->type,( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp356))->rgn, Cyc_Toc_mt_tq)); struct _tuple1* _temp424=({
struct _tuple1* _temp426=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp426->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp356))->name;
_temp426->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp356))->tq;
_temp426->f3= _temp423; _temp426;}); _temp422=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp422,({ struct Cyc_List_List* _temp425=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp425->hd=( void*) _temp424; _temp425->tl= 0;
_temp425;}));} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp427=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp427[ 0]=({ struct Cyc_Absyn_FnType_struct _temp428; _temp428.tag= Cyc_Absyn_FnType;
_temp428.f1=({ struct Cyc_Absyn_FnInfo _temp429; _temp429.tvars= 0; _temp429.effect=
0; _temp429.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp362); _temp429.args=
_temp422; _temp429.c_varargs= _temp358; _temp429.cyc_varargs= 0; _temp429.rgn_po=
0; _temp429.attributes= _temp412; _temp429;}); _temp428;}); _temp427;});}}
_LL262: _temp368=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp368);{
struct _tagged_arr* _temp430= Cyc_Toc_add_tuple_type( _temp368); return Cyc_Absyn_strct(
_temp430);} _LL264: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp431=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp431[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp432; _temp432.tag= Cyc_Absyn_AnonStructType; _temp432.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp370); _temp432;}); _temp431;});
_LL266: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp433=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp433[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp434; _temp434.tag=
Cyc_Absyn_AnonUnionType; _temp434.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp372); _temp434;}); _temp433;}); _LL268: if( _temp378 == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous union", sizeof(
unsigned char), 16u));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp378)); _LL270: if( _temp384 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous struct", sizeof(
unsigned char), 17u));} return Cyc_Absyn_strctq(( struct _tuple0*) _check_null(
_temp384)); _LL272: return t; _LL274: if( _temp390 == 0){ return(( void*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: unresolved TypedefType",
sizeof( unsigned char), 38u));} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp435=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp435[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp436; _temp436.tag= Cyc_Absyn_TypedefType;
_temp436.f1= _temp394; _temp436.f2= 0; _temp436.f3=({ struct Cyc_Core_Opt*
_temp437=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp437->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp390))->v); _temp437;}); _temp436;}); _temp435;}); _LL276:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL278: return(( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u)); _LL280: return(( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u)); _LL282: return(( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u)); _LL232:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp438= t; struct Cyc_Absyn_Exp* _temp444; struct Cyc_Absyn_Tqual _temp446;
void* _temp448; _LL440: if(( unsigned int) _temp438 > 4u?*(( int*) _temp438) ==
Cyc_Absyn_ArrayType: 0){ _LL449: _temp448=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp438)->f1; goto _LL447; _LL447: _temp446=(( struct Cyc_Absyn_ArrayType_struct*)
_temp438)->f2; goto _LL445; _LL445: _temp444=(( struct Cyc_Absyn_ArrayType_struct*)
_temp438)->f3; goto _LL441;} else{ goto _LL442;} _LL442: goto _LL443; _LL441:
return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ( _temp448,( void*) Cyc_Absyn_HeapRgn,
_temp446), e, l); _LL443: return Cyc_Absyn_cast_exp( t, e, l); _LL439:;} static
int Cyc_Toc_atomic_typ( void* t){ void* _temp450= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp486; void* _temp488; void* _temp490; struct Cyc_Absyn_FnInfo
_temp492; struct Cyc_Absyn_Exp* _temp494; struct Cyc_Absyn_Tqual _temp496; void*
_temp498; struct Cyc_List_List* _temp500; struct Cyc_Absyn_Structdecl** _temp502;
struct Cyc_List_List* _temp504; struct _tuple0* _temp506; struct Cyc_Absyn_TunionInfo
_temp508; struct Cyc_List_List* _temp510; struct Cyc_List_List* _temp512; struct
Cyc_Absyn_TunionFieldInfo _temp514; struct Cyc_List_List* _temp516; void*
_temp518; struct Cyc_Absyn_Tunionfield* _temp520; struct Cyc_Absyn_Tuniondecl*
_temp522; struct Cyc_Absyn_Uniondecl** _temp524; struct Cyc_List_List* _temp526;
struct _tuple0* _temp528; struct Cyc_Absyn_PtrInfo _temp530; void* _temp532;
_LL452: if( _temp450 ==( void*) Cyc_Absyn_VoidType){ goto _LL453;} else{ goto
_LL454;} _LL454: if(( unsigned int) _temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_VarType:
0){ _LL487: _temp486=(( struct Cyc_Absyn_VarType_struct*) _temp450)->f1; goto
_LL455;} else{ goto _LL456;} _LL456: if(( unsigned int) _temp450 > 4u?*(( int*)
_temp450) == Cyc_Absyn_IntType: 0){ _LL491: _temp490=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp450)->f1; goto _LL489; _LL489: _temp488=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp450)->f2; goto _LL457;} else{ goto _LL458;} _LL458: if( _temp450 ==( void*)
Cyc_Absyn_FloatType){ goto _LL459;} else{ goto _LL460;} _LL460: if( _temp450 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL461;} else{ goto _LL462;} _LL462: if((
unsigned int) _temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_FnType: 0){ _LL493:
_temp492=(( struct Cyc_Absyn_FnType_struct*) _temp450)->f1; goto _LL463;} else{
goto _LL464;} _LL464: if(( unsigned int) _temp450 > 4u?*(( int*) _temp450) ==
Cyc_Absyn_ArrayType: 0){ _LL499: _temp498=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp450)->f1; goto _LL497; _LL497: _temp496=(( struct Cyc_Absyn_ArrayType_struct*)
_temp450)->f2; goto _LL495; _LL495: _temp494=(( struct Cyc_Absyn_ArrayType_struct*)
_temp450)->f3; goto _LL465;} else{ goto _LL466;} _LL466: if(( unsigned int)
_temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_TupleType: 0){ _LL501: _temp500=((
struct Cyc_Absyn_TupleType_struct*) _temp450)->f1; goto _LL467;} else{ goto
_LL468;} _LL468: if(( unsigned int) _temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_StructType:
0){ _LL507: _temp506=(( struct Cyc_Absyn_StructType_struct*) _temp450)->f1; goto
_LL505; _LL505: _temp504=(( struct Cyc_Absyn_StructType_struct*) _temp450)->f2;
goto _LL503; _LL503: _temp502=(( struct Cyc_Absyn_StructType_struct*) _temp450)->f3;
goto _LL469;} else{ goto _LL470;} _LL470: if(( unsigned int) _temp450 > 4u?*((
int*) _temp450) == Cyc_Absyn_TunionType: 0){ _LL509: _temp508=(( struct Cyc_Absyn_TunionType_struct*)
_temp450)->f1; goto _LL471;} else{ goto _LL472;} _LL472: if(( unsigned int)
_temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_AnonStructType: 0){ _LL511:
_temp510=(( struct Cyc_Absyn_AnonStructType_struct*) _temp450)->f1; goto _LL473;}
else{ goto _LL474;} _LL474: if(( unsigned int) _temp450 > 4u?*(( int*) _temp450)
== Cyc_Absyn_AnonUnionType: 0){ _LL513: _temp512=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp450)->f1; goto _LL475;} else{ goto _LL476;} _LL476: if(( unsigned int)
_temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_TunionFieldType: 0){ _LL515:
_temp514=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp450)->f1; _LL519:
_temp518=( void*) _temp514.field_info; if(*(( int*) _temp518) == Cyc_Absyn_KnownTunionfield){
_LL523: _temp522=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp518)->f1;
goto _LL521; _LL521: _temp520=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp518)->f2; goto _LL517;} else{ goto _LL478;} _LL517: _temp516= _temp514.targs;
goto _LL477;} else{ goto _LL478;} _LL478: if(( unsigned int) _temp450 > 4u?*((
int*) _temp450) == Cyc_Absyn_UnionType: 0){ _LL529: _temp528=(( struct Cyc_Absyn_UnionType_struct*)
_temp450)->f1; goto _LL527; _LL527: _temp526=(( struct Cyc_Absyn_UnionType_struct*)
_temp450)->f2; goto _LL525; _LL525: _temp524=(( struct Cyc_Absyn_UnionType_struct*)
_temp450)->f3; goto _LL479;} else{ goto _LL480;} _LL480: if(( unsigned int)
_temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_PointerType: 0){ _LL531: _temp530=((
struct Cyc_Absyn_PointerType_struct*) _temp450)->f1; goto _LL481;} else{ goto
_LL482;} _LL482: if(( unsigned int) _temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_RgnHandleType:
0){ _LL533: _temp532=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp450)->f1;
goto _LL483;} else{ goto _LL484;} _LL484: goto _LL485; _LL453: return 1; _LL455:
return 0; _LL457: return 1; _LL459: return 1; _LL461: return 1; _LL463: return 1;
_LL465: return Cyc_Toc_atomic_typ( _temp498); _LL467: for( 0; _temp500 != 0;
_temp500=(( struct Cyc_List_List*) _check_null( _temp500))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp500))->hd)).f2)){
return 0;}} return 1; _LL469: return 0; _LL471: return 0; _LL473: _temp512=
_temp510; goto _LL475; _LL475: for( 0; _temp512 != 0; _temp512=(( struct Cyc_List_List*)
_check_null( _temp512))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp512))->hd)->type)){ return 0;}} return
1; _LL477: return _temp520->typs == 0; _LL479: return 0; _LL481: return 0;
_LL483: return 0; _LL485:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp534= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _get_arr_size( _temp534, 1u), _temp534.curr);}));
return 0; _LL451:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp535=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp541; struct Cyc_Absyn_Conref*
_temp543; struct Cyc_Absyn_Tqual _temp545; struct Cyc_Absyn_Conref* _temp547;
void* _temp549; void* _temp551; _LL537: if(( unsigned int) _temp535 > 4u?*(( int*)
_temp535) == Cyc_Absyn_PointerType: 0){ _LL542: _temp541=(( struct Cyc_Absyn_PointerType_struct*)
_temp535)->f1; _LL552: _temp551=( void*) _temp541.elt_typ; goto _LL550; _LL550:
_temp549=( void*) _temp541.rgn_typ; goto _LL548; _LL548: _temp547= _temp541.nullable;
goto _LL546; _LL546: _temp545= _temp541.tq; goto _LL544; _LL544: _temp543=
_temp541.bounds; goto _LL538;} else{ goto _LL539;} _LL539: goto _LL540; _LL538: {
void* _temp553= Cyc_Tcutil_compress( _temp551); _LL555: if( _temp553 ==( void*)
Cyc_Absyn_VoidType){ goto _LL556;} else{ goto _LL557;} _LL557: goto _LL558;
_LL556: return 1; _LL558: return 0; _LL554:;} _LL540: return 0; _LL536:;} static
int Cyc_Toc_is_poly_field( void* t, struct _tagged_arr* f){ void* _temp559= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Structdecl** _temp571; struct Cyc_Absyn_Structdecl*
_temp573; struct Cyc_List_List* _temp574; struct _tuple0* _temp576; struct Cyc_List_List*
_temp578; struct Cyc_List_List* _temp580; struct Cyc_Absyn_Uniondecl** _temp582;
struct Cyc_List_List* _temp584; struct _tuple0* _temp586; _LL561: if((
unsigned int) _temp559 > 4u?*(( int*) _temp559) == Cyc_Absyn_StructType: 0){
_LL577: _temp576=(( struct Cyc_Absyn_StructType_struct*) _temp559)->f1; goto
_LL575; _LL575: _temp574=(( struct Cyc_Absyn_StructType_struct*) _temp559)->f2;
goto _LL572; _LL572: _temp571=(( struct Cyc_Absyn_StructType_struct*) _temp559)->f3;
if( _temp571 == 0){ goto _LL563;} else{ _temp573=* _temp571; goto _LL562;}}
else{ goto _LL563;} _LL563: if(( unsigned int) _temp559 > 4u?*(( int*) _temp559)
== Cyc_Absyn_AnonStructType: 0){ _LL579: _temp578=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp559)->f1; goto _LL564;} else{ goto _LL565;} _LL565: if(( unsigned int)
_temp559 > 4u?*(( int*) _temp559) == Cyc_Absyn_AnonUnionType: 0){ _LL581:
_temp580=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp559)->f1; goto _LL566;}
else{ goto _LL567;} _LL567: if(( unsigned int) _temp559 > 4u?*(( int*) _temp559)
== Cyc_Absyn_UnionType: 0){ _LL587: _temp586=(( struct Cyc_Absyn_UnionType_struct*)
_temp559)->f1; goto _LL585; _LL585: _temp584=(( struct Cyc_Absyn_UnionType_struct*)
_temp559)->f2; goto _LL583; _LL583: _temp582=(( struct Cyc_Absyn_UnionType_struct*)
_temp559)->f3; goto _LL568;} else{ goto _LL569;} _LL569: goto _LL570; _LL562:
if( _temp573->fields == 0){( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: struct missing fields", sizeof( unsigned char), 37u)));}
_temp578=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp573->fields))->v;
goto _LL564; _LL564: { struct Cyc_Absyn_Structfield* _temp588= Cyc_Absyn_lookup_field(
_temp578, f); if( _temp588 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp589=*(( struct Cyc_Absyn_Structfield*)
_check_null( _temp588))->name; xprintf("is_poly_field: bad field %.*s",
_get_arr_size( _temp589, 1u), _temp589.curr);}));} return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp588))->type);} _LL566:
return 0; _LL568: return 0; _LL570:( int) _throw((( void*(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr _temp590= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field: bad type %.*s", _get_arr_size( _temp590, 1u),
_temp590.curr);}))); _LL560:;} static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp*
e){ void* _temp591=( void*) e->r; struct _tagged_arr* _temp599; struct Cyc_Absyn_Exp*
_temp601; struct _tagged_arr* _temp603; struct Cyc_Absyn_Exp* _temp605; _LL593:
if(*(( int*) _temp591) == Cyc_Absyn_StructMember_e){ _LL602: _temp601=(( struct
Cyc_Absyn_StructMember_e_struct*) _temp591)->f1; goto _LL600; _LL600: _temp599=((
struct Cyc_Absyn_StructMember_e_struct*) _temp591)->f2; goto _LL594;} else{ goto
_LL595;} _LL595: if(*(( int*) _temp591) == Cyc_Absyn_StructArrow_e){ _LL606:
_temp605=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp591)->f1; goto _LL604;
_LL604: _temp603=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp591)->f2; goto
_LL596;} else{ goto _LL597;} _LL597: goto _LL598; _LL594: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp601->topt))->v, _temp599);
_LL596: { void* _temp607= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp605->topt))->v); struct Cyc_Absyn_PtrInfo _temp613; struct Cyc_Absyn_Conref*
_temp615; struct Cyc_Absyn_Tqual _temp617; struct Cyc_Absyn_Conref* _temp619;
void* _temp621; void* _temp623; _LL609: if(( unsigned int) _temp607 > 4u?*(( int*)
_temp607) == Cyc_Absyn_PointerType: 0){ _LL614: _temp613=(( struct Cyc_Absyn_PointerType_struct*)
_temp607)->f1; _LL624: _temp623=( void*) _temp613.elt_typ; goto _LL622; _LL622:
_temp621=( void*) _temp613.rgn_typ; goto _LL620; _LL620: _temp619= _temp613.nullable;
goto _LL618; _LL618: _temp617= _temp613.tq; goto _LL616; _LL616: _temp615=
_temp613.bounds; goto _LL610;} else{ goto _LL611;} _LL611: goto _LL612; _LL610:
return Cyc_Toc_is_poly_field( _temp623, _temp603); _LL612:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp625= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp605->topt))->v); xprintf("is_poly_project:  bad type %.*s", _get_arr_size(
_temp625, 1u), _temp625.curr);})); return 0; _LL608:;} _LL598: return 0; _LL592:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_e,({ struct Cyc_List_List*
_temp626=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp626->hd=( void*) s; _temp626->tl= 0; _temp626;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc_GC_malloc_atomic_e,({ struct Cyc_List_List* _temp627=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp627->hd=( void*) s; _temp627->tl=
0; _temp627;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t,
struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic(
s);} return Cyc_Toc_malloc_ptr( s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({ struct Cyc_List_List*(* _temp628)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp631= rgn; struct Cyc_Absyn_Exp* _temp632= s; struct
Cyc_Absyn_Exp* _temp629[ 2u]={ _temp631, _temp632}; struct _tagged_arr _temp630={(
void*) _temp629,( void*) _temp629,( void*)( _temp629 + 2u)}; _temp628( _temp630);}),
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Toc__sfile_to_file_e,({ struct Cyc_List_List*
_temp633=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp633->hd=( void*) e; _temp633->tl= 0; _temp633;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({
struct Cyc_List_List* _temp634=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp634->hd=( void*) e; _temp634->tl= 0; _temp634;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_toplevel_tagged_arr( void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ int is_string= 0;{ void* _temp635=(
void*) e->r; void* _temp641; struct _tagged_arr _temp643; _LL637: if(*(( int*)
_temp635) == Cyc_Absyn_Const_e){ _LL642: _temp641=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp635)->f1; if(( unsigned int) _temp641 > 1u?*(( int*) _temp641) == Cyc_Absyn_String_c:
0){ _LL644: _temp643=(( struct Cyc_Absyn_String_c_struct*) _temp641)->f1; goto
_LL638;} else{ goto _LL639;}} else{ goto _LL639;} _LL639: goto _LL640; _LL638:
is_string= 1; goto _LL636; _LL640: goto _LL636; _LL636:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp648=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp648[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp649; _temp649.tag= Cyc_Absyn_ArrayType; _temp649.f1=(
void*) Cyc_Absyn_uchar_t; _temp649.f2= Cyc_Toc_mt_tq; _temp649.f3=( struct Cyc_Absyn_Exp*)
sz; _temp649;}); _temp648;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp645=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp645->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp646=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp646[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp647; _temp647.tag= Cyc_Absyn_Var_d;
_temp647.f1= vd; _temp647;}); _temp646;}), 0); _temp645->tl= Cyc_Toc_result_decls;
_temp645;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct Cyc_List_List*(* _temp650)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct _tuple4* _temp653=({ struct _tuple4*
_temp658=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp658->f1= 0;
_temp658->f2= xexp; _temp658;}); struct _tuple4* _temp654=({ struct _tuple4*
_temp657=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp657->f1= 0;
_temp657->f2= xexp; _temp657;}); struct _tuple4* _temp655=({ struct _tuple4*
_temp656=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp656->f1= 0;
_temp656->f2= xplussz; _temp656;}); struct _tuple4* _temp651[ 3u]={ _temp653,
_temp654, _temp655}; struct _tagged_arr _temp652={( void*) _temp651,( void*)
_temp651,( void*)( _temp651 + 3u)}; _temp650( _temp652);}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp659=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp659->break_lab= 0; _temp659->continue_lab= 0; _temp659->fallthru_info=
0; _temp659->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp659->toplevel= 1;
_temp659;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp660=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp660->break_lab= e->break_lab; _temp660->continue_lab=
e->continue_lab; _temp660->fallthru_info= e->fallthru_info; _temp660->varmap= e->varmap;
_temp660->toplevel= e->toplevel; _temp660;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp661=(* x).f1; struct
Cyc_List_List* _temp667; _LL663: if(( unsigned int) _temp661 > 1u?*(( int*)
_temp661) == Cyc_Absyn_Rel_n: 0){ _LL668: _temp667=(( struct Cyc_Absyn_Rel_n_struct*)
_temp661)->f1; goto _LL664;} else{ goto _LL665;} _LL665: goto _LL666; _LL664:(
int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct
_tagged_arr)({ struct _tagged_arr _temp669= Cyc_Absynpp_qvar2string( x); xprintf("Toc::add_varmap on Rel_n: %.*s\n",
_get_arr_size( _temp669, 1u), _temp669.curr);}))); _LL666: goto _LL662; _LL662:;}{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)(
ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=
0; ans->continue_lab= 0; return ans;} struct _tuple6{ struct _tagged_arr* f1;
struct Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_arr*
break_l, struct _tagged_arr* fallthru_l, struct Cyc_List_List* fallthru_binders,
struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List* _temp670= 0; for( 0;
fallthru_binders != 0; fallthru_binders=(( struct Cyc_List_List*) _check_null(
fallthru_binders))->tl){ _temp670=({ struct Cyc_List_List* _temp671=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp671->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp671->tl=
_temp670; _temp671;});} _temp670=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp670);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp672=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp672->v=( void*) break_l; _temp672;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp673=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp673->v=( void*)({ struct _tuple6*
_temp674=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp674->f1=
fallthru_l; _temp674->f2= _temp670; _temp674->f3= next_case_env->varmap;
_temp674;}); _temp673;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp675=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp675->v=( void*)
break_l; _temp675;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_arr* next_l){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp676=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp676->v=( void*)({ struct _tuple6* _temp677=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp677->f1= next_l; _temp677->f2= 0;
_temp677->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp677;}); _temp676;});
return ans;} struct _tuple7{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tuple8{ struct _tuple0* f1; void* f2; struct Cyc_Absyn_Exp* f3; } ;
static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c( void* p, struct Cyc_Core_Opt*
aopt, struct Cyc_List_List* args, struct Cyc_List_List* typs){ if( args == 0){
return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("expecting a literal format string", sizeof( unsigned char), 34u));}{
struct _tagged_arr fmt_str;{ void* _temp678=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd)->r; void* _temp684; struct
_tagged_arr _temp686; _LL680: if(*(( int*) _temp678) == Cyc_Absyn_Const_e){
_LL685: _temp684=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp678)->f1; if((
unsigned int) _temp684 > 1u?*(( int*) _temp684) == Cyc_Absyn_String_c: 0){
_LL687: _temp686=(( struct Cyc_Absyn_String_c_struct*) _temp684)->f1; goto
_LL681;} else{ goto _LL682;}} else{ goto _LL682;} _LL682: goto _LL683; _LL681:
fmt_str= _temp686; goto _LL679; _LL683: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("expecting a literal format string",
sizeof( unsigned char), 34u)); _LL679:;}{ int len=( int) _get_arr_size( fmt_str,
sizeof( unsigned char)); int i= 0; for( i= 0; i < len; i ++){ if(*(( const
unsigned char*) _check_unknown_subscript( fmt_str, sizeof( unsigned char), i))
=='%'){ i ++; if( i < len?*(( const unsigned char*) _check_unknown_subscript(
fmt_str, sizeof( unsigned char), i)) =='s': 0){ break;}}} if( i == len){ if(
aopt != 0){ args=({ struct Cyc_List_List* _temp688=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp688->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp688->tl= args; _temp688;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp689= _new_region(); struct
_RegionHandle* r=& _temp689; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*)
_check_unknown_subscript( fmt_str, sizeof( unsigned char), i)); rev_fmt=({
struct Cyc_List_List* _temp690=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp690->hd=( void*)(( int) c); _temp690->tl=
rev_fmt; _temp690;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* _temp691=
Cyc_Formatstr_parse_conversionspecification( fmt_str, i + 1); if( _temp691 == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("bad format string",
sizeof( unsigned char), 18u));}{ struct _tuple7 _temp694; int _temp695;
unsigned char _temp697; struct Cyc_List_List* _temp699; struct Cyc_List_List*
_temp701; struct Cyc_List_List* _temp703; struct Cyc_List_List* _temp705; struct
_tuple7* _temp692=( struct _tuple7*)(( struct Cyc_Core_Opt*) _check_null(
_temp691))->v; _temp694=* _temp692; _LL706: _temp705= _temp694.f1; goto _LL704;
_LL704: _temp703= _temp694.f2; goto _LL702; _LL702: _temp701= _temp694.f3; goto
_LL700; _LL700: _temp699= _temp694.f4; goto _LL698; _LL698: _temp697= _temp694.f5;
goto _LL696; _LL696: _temp695= _temp694.f6; goto _LL693; _LL693: i= _temp695 - 1;
if( _temp697 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp707)( struct _RegionHandle*, struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_arr)) Cyc_List_rlist; struct _RegionHandle* _temp714= r; struct Cyc_List_List*
_temp710= _temp705; struct Cyc_List_List* _temp711= _temp703; struct Cyc_List_List*
_temp712= _temp701; struct Cyc_List_List* _temp713= _temp699; struct Cyc_List_List*
_temp708[ 4u]={ _temp710, _temp711, _temp712, _temp713}; struct _tagged_arr
_temp709={( void*) _temp708,( void*) _temp708,( void*)( _temp708 + 4u)};
_temp707( _temp714, _temp709);})), rev_fmt); rev_fmt=({ struct Cyc_List_List*
_temp715=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp715->hd=( void*)(( int) _temp697); _temp715->tl= rev_fmt; _temp715;});{
struct Cyc_List_List* _temp716= _temp703; struct Cyc_List_List _temp723; struct
Cyc_List_List* _temp724; int _temp726; _LL718: if( _temp716 == 0){ goto _LL720;}
else{ _temp723=* _temp716; _LL727: _temp726=( int) _temp723.hd; goto _LL725;
_LL725: _temp724= _temp723.tl; if( _temp724 == 0){ goto _LL722;} else{ goto
_LL720;}} _LL722: if( _temp726 =='*'){ goto _LL719;} else{ goto _LL720;} _LL720:
goto _LL721; _LL719: { struct _tuple0* _temp728= Cyc_Toc_temp_var(); rev_temps=({
struct Cyc_List_List* _temp729=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp729->hd=( void*)({ struct _tuple8* _temp730=(
struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8)); _temp730->f1=
_temp728; _temp730->f2=( void*)(( struct Cyc_List_List*) _check_null( typs))->hd;
_temp730->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd;
_temp730;}); _temp729->tl= rev_temps; _temp729;}); rev_result=({ struct Cyc_List_List*
_temp731=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp731->hd=( void*) Cyc_Absyn_var_exp( _temp728, 0); _temp731->tl= rev_result;
_temp731;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL717;} _LL721: goto
_LL717; _LL717:;}{ struct Cyc_List_List* _temp732= _temp701; struct Cyc_List_List
_temp739; struct Cyc_List_List* _temp740; struct Cyc_List_List _temp742; struct
Cyc_List_List* _temp743; int _temp745; int _temp747; _LL734: if( _temp732 == 0){
goto _LL736;} else{ _temp739=* _temp732; _LL748: _temp747=( int) _temp739.hd;
goto _LL741; _LL741: _temp740= _temp739.tl; if( _temp740 == 0){ goto _LL736;}
else{ _temp742=* _temp740; _LL746: _temp745=( int) _temp742.hd; goto _LL744;
_LL744: _temp743= _temp742.tl; if( _temp743 == 0){ goto _LL738;} else{ goto
_LL736;}}} _LL738: if( _temp747 =='.'? _temp745 =='*': 0){ goto _LL735;} else{
goto _LL736;} _LL736: goto _LL737; _LL735: { struct _tuple0* _temp749= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp750=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp750->hd=( void*)({
struct _tuple8* _temp751=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp751->f1= _temp749; _temp751->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp751->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp751;}); _temp750->tl= rev_temps; _temp750;});
rev_result=({ struct Cyc_List_List* _temp752=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp752->hd=( void*) Cyc_Absyn_var_exp(
_temp749, 0); _temp752->tl= rev_result; _temp752;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL733;} _LL737: goto _LL733; _LL733:;} if( _temp697 !='%'){ if( args == 0?
1: typs == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct
_tagged_arr)({ struct _tagged_arr _temp753= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_get_arr_size( _temp753, 1u), _temp753.curr);}));}{ struct _tuple0* _temp754=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp755=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp755->hd=( void*)({
struct _tuple8* _temp756=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp756->f1= _temp754; _temp756->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp756->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp756;}); _temp755->tl= rev_temps; _temp755;});
rev_result=({ struct Cyc_List_List* _temp757=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp757->hd=( void*) Cyc_Absyn_var_exp(
_temp754, 0); _temp757->tl= rev_result; _temp757;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp758=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp758->hd=( void*)(( int)'.');
_temp758->tl= rev_fmt; _temp758;}); rev_fmt=({ struct Cyc_List_List* _temp759=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp759->hd=( void*)(( int)'*'); _temp759->tl= rev_fmt; _temp759;}); rev_fmt=({
struct Cyc_List_List* _temp760=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp760->hd=( void*)(( int)'s'); _temp760->tl=
rev_fmt; _temp760;});{ struct _tuple0* _temp761= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp762= Cyc_Absyn_var_exp( _temp761, 0); rev_temps=({ struct Cyc_List_List*
_temp763=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp763->hd=( void*)({ struct _tuple8* _temp764=( struct _tuple8*)
_region_malloc( r, sizeof( struct _tuple8)); _temp764->f1= _temp761; _temp764->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp764->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp764;});
_temp763->tl= rev_temps; _temp763;});{ struct Cyc_Absyn_Exp* _temp765= Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp769)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp772= _temp762; struct Cyc_Absyn_Exp* _temp773= Cyc_Absyn_uint_exp( 1, 0);
struct Cyc_Absyn_Exp* _temp770[ 2u]={ _temp772, _temp773}; struct _tagged_arr
_temp771={( void*) _temp770,( void*) _temp770,( void*)( _temp770 + 2u)};
_temp769( _temp771);}), 0); struct Cyc_Absyn_Exp* _temp766= Cyc_Absyn_structmember_exp(
_temp762, Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List* _temp767=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp767->hd=( void*) _temp765; _temp767->tl= rev_result; _temp767;});
rev_result=({ struct Cyc_List_List* _temp768=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp768->hd=( void*)
_temp766; _temp768->tl= rev_result; _temp768;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}{
struct Cyc_List_List* _temp774=({ struct Cyc_List_List* _temp777=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp777->hd=( void*) Cyc_Absyn_string_exp((
struct _tagged_arr) Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_fmt)), 0); _temp777->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_result); _temp777;}); if( aopt != 0){
_temp774=({ struct Cyc_List_List* _temp775=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp775->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp775->tl= _temp774; _temp775;});}{
struct Cyc_Absyn_Exp* _temp776= Cyc_Absyn_primop_exp( p, _temp774, 0); s= Cyc_Absyn_exp_stmt(
_temp776, 0); for( 0; rev_temps != 0; rev_temps=(( struct Cyc_List_List*)
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
e){ return({ struct _tuple3* _temp778=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp778->f1= Cyc_Toc_mt_tq; _temp778->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp778;});} static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp779=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp779->f1= 0; _temp779->f2= e; _temp779;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp780= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp780): Cyc_Toc_malloc_ptr( _temp780), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp780), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp781=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp781 != 0; _temp781=((
struct Cyc_List_List*) _check_null( _temp781))->tl){ struct _tuple4 _temp784;
struct Cyc_Absyn_Exp* _temp785; struct Cyc_List_List* _temp787; struct _tuple4*
_temp782=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp781))->hd;
_temp784=* _temp782; _LL788: _temp787= _temp784.f1; goto _LL786; _LL786:
_temp785= _temp784.f2; goto _LL783; _LL783: { struct Cyc_Absyn_Exp* e_index; if(
_temp787 == 0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp787))->tl != 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u));}{ void* _temp789=( void*)(( struct Cyc_List_List*)
_check_null( _temp787))->hd; void* _temp790= _temp789; struct Cyc_Absyn_Exp*
_temp796; struct _tagged_arr* _temp798; _LL792: if(*(( int*) _temp790) == Cyc_Absyn_ArrayElement){
_LL797: _temp796=(( struct Cyc_Absyn_ArrayElement_struct*) _temp790)->f1; goto
_LL793;} else{ goto _LL794;} _LL794: if(*(( int*) _temp790) == Cyc_Absyn_FieldName){
_LL799: _temp798=(( struct Cyc_Absyn_FieldName_struct*) _temp790)->f1; goto
_LL795;} else{ goto _LL791;} _LL793: Cyc_Toc_exp_to_c( nv, _temp796); e_index=
_temp796; goto _LL791; _LL795: e_index=(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("field name designators in array",
sizeof( unsigned char), 32u)); goto _LL791; _LL791:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp800=( void*)
_temp785->r; struct Cyc_List_List* _temp810; struct Cyc_Absyn_Exp* _temp812;
struct Cyc_Absyn_Exp* _temp814; struct Cyc_Absyn_Vardecl* _temp816; struct Cyc_List_List*
_temp818; void* _temp820; _LL802: if(*(( int*) _temp800) == Cyc_Absyn_Array_e){
_LL811: _temp810=(( struct Cyc_Absyn_Array_e_struct*) _temp800)->f1; goto _LL803;}
else{ goto _LL804;} _LL804: if(*(( int*) _temp800) == Cyc_Absyn_Comprehension_e){
_LL817: _temp816=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp800)->f1;
goto _LL815; _LL815: _temp814=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp800)->f2; goto _LL813; _LL813: _temp812=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp800)->f3; goto _LL805;} else{ goto _LL806;} _LL806: if(*(( int*) _temp800)
== Cyc_Absyn_AnonStruct_e){ _LL821: _temp820=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp800)->f1; goto _LL819; _LL819: _temp818=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp800)->f2; goto _LL807;} else{ goto _LL808;} _LL808: goto _LL809; _LL803: s=
Cyc_Toc_init_array( nv, lval, _temp810, s); goto _LL801; _LL805: s= Cyc_Toc_init_comprehension(
nv, lval, _temp816, _temp814, _temp812, s, 0); goto _LL801; _LL807: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp820, _temp818, s); goto _LL801; _LL809: Cyc_Toc_exp_to_c( nv,
_temp785); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp785, 0), s, 0); goto _LL801; _LL801:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp822= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp822, Cyc_Absyn_varb_exp( _temp822,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp845=( struct Cyc_Absyn_Local_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp845[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp846; _temp846.tag= Cyc_Absyn_Local_b; _temp846.f1= vd; _temp846;});
_temp845;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp822, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp822, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp822, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp822, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp823=( void*) e2->r; struct Cyc_List_List* _temp833; struct Cyc_Absyn_Exp*
_temp835; struct Cyc_Absyn_Exp* _temp837; struct Cyc_Absyn_Vardecl* _temp839;
struct Cyc_List_List* _temp841; void* _temp843; _LL825: if(*(( int*) _temp823)
== Cyc_Absyn_Array_e){ _LL834: _temp833=(( struct Cyc_Absyn_Array_e_struct*)
_temp823)->f1; goto _LL826;} else{ goto _LL827;} _LL827: if(*(( int*) _temp823)
== Cyc_Absyn_Comprehension_e){ _LL840: _temp839=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp823)->f1; goto _LL838; _LL838: _temp837=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp823)->f2; goto _LL836; _LL836: _temp835=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp823)->f3; goto _LL828;} else{ goto _LL829;} _LL829: if(*(( int*) _temp823)
== Cyc_Absyn_AnonStruct_e){ _LL844: _temp843=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp823)->f1; goto _LL842; _LL842: _temp841=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp823)->f2; goto _LL830;} else{ goto _LL831;} _LL831: goto _LL832; _LL826:
body= Cyc_Toc_init_array( nv2, lval, _temp833, Cyc_Absyn_skip_stmt( 0)); goto
_LL824; _LL828: body= Cyc_Toc_init_comprehension( nv2, lval, _temp839, _temp837,
_temp835, Cyc_Absyn_skip_stmt( 0), 0); goto _LL824; _LL830: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp843, _temp841, Cyc_Absyn_skip_stmt( 0)); goto _LL824; _LL832: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL824; _LL824:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp822, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp847=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp847 != 0; _temp847=(( struct Cyc_List_List*)
_check_null( _temp847))->tl){ struct _tuple4 _temp850; struct Cyc_Absyn_Exp*
_temp851; struct Cyc_List_List* _temp853; struct _tuple4* _temp848=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp847))->hd; _temp850=*
_temp848; _LL854: _temp853= _temp850.f1; goto _LL852; _LL852: _temp851= _temp850.f2;
goto _LL849; _LL849: if( _temp853 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("empty designator list", sizeof( unsigned char), 22u));} if((( struct
Cyc_List_List*) _check_null( _temp853))->tl != 0){(( int(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u));}{ void* _temp855=( void*)(( struct Cyc_List_List*)
_check_null( _temp853))->hd; struct _tagged_arr* _temp861; _LL857: if(*(( int*)
_temp855) == Cyc_Absyn_FieldName){ _LL862: _temp861=(( struct Cyc_Absyn_FieldName_struct*)
_temp855)->f1; goto _LL858;} else{ goto _LL859;} _LL859: goto _LL860; _LL858: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp861, 0);{ void*
_temp863=( void*) _temp851->r; struct Cyc_List_List* _temp873; struct Cyc_Absyn_Exp*
_temp875; struct Cyc_Absyn_Exp* _temp877; struct Cyc_Absyn_Vardecl* _temp879;
struct Cyc_List_List* _temp881; void* _temp883; _LL865: if(*(( int*) _temp863)
== Cyc_Absyn_Array_e){ _LL874: _temp873=(( struct Cyc_Absyn_Array_e_struct*)
_temp863)->f1; goto _LL866;} else{ goto _LL867;} _LL867: if(*(( int*) _temp863)
== Cyc_Absyn_Comprehension_e){ _LL880: _temp879=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp863)->f1; goto _LL878; _LL878: _temp877=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp863)->f2; goto _LL876; _LL876: _temp875=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp863)->f3; goto _LL868;} else{ goto _LL869;} _LL869: if(*(( int*) _temp863)
== Cyc_Absyn_AnonStruct_e){ _LL884: _temp883=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp863)->f1; goto _LL882; _LL882: _temp881=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp863)->f2; goto _LL870;} else{ goto _LL871;} _LL871: goto _LL872; _LL866: s=
Cyc_Toc_init_array( nv, lval, _temp873, s); goto _LL864; _LL868: s= Cyc_Toc_init_comprehension(
nv, lval, _temp879, _temp877, _temp875, s, 0); goto _LL864; _LL870: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp883, _temp881, s); goto _LL864; _LL872: Cyc_Toc_exp_to_c( nv,
_temp851); if( Cyc_Toc_is_poly_field( struct_type, _temp861)){ _temp851= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp851, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp851, 0), 0), s, 0); goto _LL864; _LL864:;} goto
_LL856;} _LL860:( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u))); _LL856:;}}}
return s;} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env*
nv, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct
_RegionHandle _temp885= _new_region(); struct _RegionHandle* r=& _temp885;
_push_region( r);{ struct Cyc_List_List* _temp886=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c, es); struct _tagged_arr* _temp887= Cyc_Toc_add_tuple_type(
_temp886); struct _tuple0* _temp888= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp889= Cyc_Absyn_var_exp( _temp888, 0); struct Cyc_Absyn_Stmt* _temp890= Cyc_Absyn_exp_stmt(
_temp889, 0); struct Cyc_Absyn_Exp*(* _temp891)( struct Cyc_Absyn_Exp*, struct
_tagged_arr*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1; struct Cyc_List_List* _temp892=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)(
r, es);{ int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp892);
for( 0; _temp892 != 0;( _temp892=(( struct Cyc_List_List*) _check_null( _temp892))->tl,
-- i)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp892))->hd; struct Cyc_Absyn_Exp* lval= _temp891( _temp889, Cyc_Toc_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp893=( void*) e->r; struct Cyc_List_List*
_temp901; struct Cyc_Absyn_Exp* _temp903; struct Cyc_Absyn_Exp* _temp905; struct
Cyc_Absyn_Vardecl* _temp907; _LL895: if(*(( int*) _temp893) == Cyc_Absyn_Array_e){
_LL902: _temp901=(( struct Cyc_Absyn_Array_e_struct*) _temp893)->f1; goto _LL896;}
else{ goto _LL897;} _LL897: if(*(( int*) _temp893) == Cyc_Absyn_Comprehension_e){
_LL908: _temp907=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp893)->f1;
goto _LL906; _LL906: _temp905=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp893)->f2; goto _LL904; _LL904: _temp903=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp893)->f3; goto _LL898;} else{ goto _LL899;} _LL899: goto _LL900; _LL896:
_temp890= Cyc_Toc_init_array( nv, lval, _temp901, _temp890); goto _LL894; _LL898:
_temp890= Cyc_Toc_init_comprehension( nv, lval, _temp907, _temp905, _temp903,
_temp890, 0); goto _LL894; _LL900: Cyc_Toc_exp_to_c( nv, e); _temp890= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp891( _temp889, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp890, 0); goto _LL894; _LL894:;}}}{ struct Cyc_Absyn_Exp*
_temp909= Cyc_Toc_make_struct( nv, _temp888, Cyc_Absyn_strct( _temp887),
_temp890, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp909;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp910= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp911= Cyc_Absyn_var_exp( _temp910, 0); struct Cyc_Absyn_Stmt*
_temp912= Cyc_Absyn_exp_stmt( _temp911, 0); struct Cyc_Absyn_Exp*(* _temp913)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp914= _new_region(); struct _RegionHandle* r=& _temp914;
_push_region( r);{ struct Cyc_List_List* _temp915=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp915 != 0; _temp915=(( struct Cyc_List_List*) _check_null( _temp915))->tl){
struct _tuple4 _temp918; struct Cyc_Absyn_Exp* _temp919; struct Cyc_List_List*
_temp921; struct _tuple4* _temp916=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp915))->hd; _temp918=* _temp916; _LL922: _temp921= _temp918.f1;
goto _LL920; _LL920: _temp919= _temp918.f2; goto _LL917; _LL917: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp919->topt))->v): 0; if( _temp921 == 0){(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("empty designator list", sizeof( unsigned char), 22u));}
if((( struct Cyc_List_List*) _check_null( _temp921))->tl != 0){ struct _tuple0*
_temp923= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp924= Cyc_Absyn_var_exp(
_temp923, 0); for( 0; _temp921 != 0; _temp921=(( struct Cyc_List_List*)
_check_null( _temp921))->tl){ void* _temp925=( void*)(( struct Cyc_List_List*)
_check_null( _temp921))->hd; struct _tagged_arr* _temp931; _LL927: if(*(( int*)
_temp925) == Cyc_Absyn_FieldName){ _LL932: _temp931=(( struct Cyc_Absyn_FieldName_struct*)
_temp925)->f1; goto _LL928;} else{ goto _LL929;} _LL929: goto _LL930; _LL928:
if( Cyc_Toc_is_poly_field( struct_type, _temp931)){ _temp924= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp924, 0);} _temp912= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp913( _temp911, _temp931, 0), _temp924, 0), 0),
_temp912, 0); goto _LL926; _LL930:( int) _throw((( void*(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)( _tag_arr("array designator in struct", sizeof(
unsigned char), 27u))); _LL926:;} Cyc_Toc_exp_to_c( nv, _temp919); _temp912= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp924, _temp919, 0), 0), _temp912,
0);} else{ void* _temp933=( void*)(( struct Cyc_List_List*) _check_null(
_temp921))->hd; struct _tagged_arr* _temp939; _LL935: if(*(( int*) _temp933) ==
Cyc_Absyn_FieldName){ _LL940: _temp939=(( struct Cyc_Absyn_FieldName_struct*)
_temp933)->f1; goto _LL936;} else{ goto _LL937;} _LL937: goto _LL938; _LL936: {
struct Cyc_Absyn_Exp* lval= _temp913( _temp911, _temp939, 0);{ void* _temp941=(
void*) _temp919->r; struct Cyc_List_List* _temp951; struct Cyc_Absyn_Exp*
_temp953; struct Cyc_Absyn_Exp* _temp955; struct Cyc_Absyn_Vardecl* _temp957;
struct Cyc_List_List* _temp959; void* _temp961; _LL943: if(*(( int*) _temp941)
== Cyc_Absyn_Array_e){ _LL952: _temp951=(( struct Cyc_Absyn_Array_e_struct*)
_temp941)->f1; goto _LL944;} else{ goto _LL945;} _LL945: if(*(( int*) _temp941)
== Cyc_Absyn_Comprehension_e){ _LL958: _temp957=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp941)->f1; goto _LL956; _LL956: _temp955=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp941)->f2; goto _LL954; _LL954: _temp953=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp941)->f3; goto _LL946;} else{ goto _LL947;} _LL947: if(*(( int*) _temp941)
== Cyc_Absyn_AnonStruct_e){ _LL962: _temp961=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp941)->f1; goto _LL960; _LL960: _temp959=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp941)->f2; goto _LL948;} else{ goto _LL949;} _LL949: goto _LL950; _LL944:
_temp912= Cyc_Toc_init_array( nv, lval, _temp951, _temp912); goto _LL942; _LL946:
_temp912= Cyc_Toc_init_comprehension( nv, lval, _temp957, _temp955, _temp953,
_temp912, 0); goto _LL942; _LL948: _temp912= Cyc_Toc_init_anon_struct( nv, lval,
_temp961, _temp959, _temp912); goto _LL942; _LL950: Cyc_Toc_exp_to_c( nv,
_temp919); if( Cyc_Toc_is_poly_field( struct_type, _temp939)){ _temp919= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp919, 0);} _temp912= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp919, 0), 0), _temp912, 0); goto _LL942; _LL942:;}
goto _LL934;} _LL938:( int) _throw((( void*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u))); _LL934:;}}}{
struct Cyc_Absyn_Exp* _temp963= Cyc_Toc_make_struct( nv, _temp910, Cyc_Absyn_strctq(
tdn), _temp912, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp963;};
_pop_region( r);} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp*
f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp*
el, struct _tuple9* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2,
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp*
e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp964=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp964[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp965; _temp965.tag= Cyc_Absyn_Increment_e;
_temp965.f1= e1; _temp965.f2=( void*) incr; _temp965;}); _temp964;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp966=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp976; struct Cyc_Absyn_Exp*
_temp978; void* _temp980; struct _tagged_arr* _temp982; struct Cyc_Absyn_Exp*
_temp984; _LL968: if(*(( int*) _temp966) == Cyc_Absyn_StmtExp_e){ _LL977:
_temp976=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp966)->f1; goto _LL969;}
else{ goto _LL970;} _LL970: if(*(( int*) _temp966) == Cyc_Absyn_Cast_e){ _LL981:
_temp980=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp966)->f1; goto _LL979;
_LL979: _temp978=(( struct Cyc_Absyn_Cast_e_struct*) _temp966)->f2; goto _LL971;}
else{ goto _LL972;} _LL972: if(*(( int*) _temp966) == Cyc_Absyn_StructMember_e){
_LL985: _temp984=(( struct Cyc_Absyn_StructMember_e_struct*) _temp966)->f1; goto
_LL983; _LL983: _temp982=(( struct Cyc_Absyn_StructMember_e_struct*) _temp966)->f2;
goto _LL973;} else{ goto _LL974;} _LL974: goto _LL975; _LL969:(( void(*)( struct
Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp976, fs,
f, f_env); goto _LL967; _LL971:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp978, fs, f, f_env); goto _LL967; _LL973:( void*)( e1->r=( void*)(( void*)
_temp984->r));(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
e1,({ struct Cyc_List_List* _temp986=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp986->hd=( void*) _temp982; _temp986->tl= fs;
_temp986;}), f, f_env); goto _LL967; _LL975: if( ! Cyc_Absyn_is_lvalue( e1)){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp987= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_get_arr_size( _temp987, 1u), _temp987.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL967;} _LL967:;}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp988=( void*) s->r; struct Cyc_Absyn_Exp* _temp998; struct Cyc_Absyn_Stmt*
_temp1000; struct Cyc_Absyn_Decl* _temp1002; struct Cyc_Absyn_Stmt* _temp1004;
struct Cyc_Absyn_Stmt* _temp1006; _LL990: if(( unsigned int) _temp988 > 1u?*((
int*) _temp988) == Cyc_Absyn_Exp_s: 0){ _LL999: _temp998=(( struct Cyc_Absyn_Exp_s_struct*)
_temp988)->f1; goto _LL991;} else{ goto _LL992;} _LL992: if(( unsigned int)
_temp988 > 1u?*(( int*) _temp988) == Cyc_Absyn_Decl_s: 0){ _LL1003: _temp1002=((
struct Cyc_Absyn_Decl_s_struct*) _temp988)->f1; goto _LL1001; _LL1001: _temp1000=((
struct Cyc_Absyn_Decl_s_struct*) _temp988)->f2; goto _LL993;} else{ goto _LL994;}
_LL994: if(( unsigned int) _temp988 > 1u?*(( int*) _temp988) == Cyc_Absyn_Seq_s:
0){ _LL1007: _temp1006=(( struct Cyc_Absyn_Seq_s_struct*) _temp988)->f1; goto
_LL1005; _LL1005: _temp1004=(( struct Cyc_Absyn_Seq_s_struct*) _temp988)->f2;
goto _LL995;} else{ goto _LL996;} _LL996: goto _LL997; _LL991:(( void(*)( struct
Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp998, fs, f,
f_env); goto _LL989; _LL993:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1000, fs, f, f_env); goto _LL989; _LL995:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1004, fs, f, f_env);
goto _LL989; _LL997:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1008= Cyc_Absynpp_stmt2string( s);
xprintf("lvalue_assign_stmt: %.*s", _get_arr_size( _temp1008, 1u), _temp1008.curr);}));
goto _LL989; _LL989:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){ return 0;}
result=({ struct Cyc_List_List* _temp1009=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1009->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1009->tl= 0;
_temp1009;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp1010=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp1010->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp1010->tl= 0;
_temp1010;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x))
Cyc_Toc_rmap_2c)( Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp1011=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1011->f1= 0; _temp1011->f2= e;
_temp1011;});} static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){
void* _temp1012= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp1018;
_LL1014: if(( unsigned int) _temp1012 > 4u?*(( int*) _temp1012) == Cyc_Absyn_PointerType:
0){ _LL1019: _temp1018=(( struct Cyc_Absyn_PointerType_struct*) _temp1012)->f1;
goto _LL1015;} else{ goto _LL1016;} _LL1016: goto _LL1017; _LL1015: return
_temp1018; _LL1017:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("get_ptr_typ:  not a pointer!", sizeof( unsigned char), 29u))); _LL1013:;}
struct _tuple10{ void* f1; void* f2; } ; struct _tuple11{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp1020=( void*) e->r; if( e->topt == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp1021= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_get_arr_size( _temp1021, 1u), _temp1021.curr);})); return;}{ void* old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; void* _temp1022=
_temp1020; void* _temp1102; void* _temp1104; void* _temp1106; struct _tuple0*
_temp1108; struct _tuple0* _temp1110; struct Cyc_List_List* _temp1112; void*
_temp1114; void* _temp1116; struct Cyc_Absyn_Exp* _temp1118; struct Cyc_Absyn_Exp*
_temp1120; struct Cyc_Core_Opt* _temp1122; struct Cyc_Absyn_Exp* _temp1124;
struct Cyc_Absyn_Exp* _temp1126; struct Cyc_Absyn_Exp* _temp1128; struct Cyc_Absyn_Exp*
_temp1130; struct Cyc_Absyn_Exp* _temp1132; struct Cyc_Absyn_Exp* _temp1134;
struct Cyc_List_List* _temp1136; struct Cyc_Absyn_Exp* _temp1138; struct Cyc_Absyn_VarargCallInfo*
_temp1140; struct Cyc_List_List* _temp1142; struct Cyc_Absyn_Exp* _temp1144;
struct Cyc_Absyn_VarargCallInfo* _temp1146; struct Cyc_Absyn_VarargCallInfo
_temp1148; struct Cyc_Absyn_VarargInfo* _temp1149; struct Cyc_List_List*
_temp1151; int _temp1153; struct Cyc_List_List* _temp1155; struct Cyc_Absyn_Exp*
_temp1157; struct Cyc_Absyn_Exp* _temp1159; struct Cyc_Absyn_Exp* _temp1161;
struct Cyc_List_List* _temp1163; struct Cyc_Absyn_Exp* _temp1165; struct Cyc_Absyn_Exp*
_temp1167; void* _temp1169; void** _temp1171; struct Cyc_Absyn_Exp* _temp1172;
struct Cyc_Absyn_Exp* _temp1174; struct Cyc_Absyn_Exp* _temp1176; struct Cyc_Absyn_Exp*
_temp1178; void* _temp1180; struct _tagged_arr* _temp1182; void* _temp1184;
struct Cyc_Absyn_Exp* _temp1186; struct _tagged_arr* _temp1188; struct Cyc_Absyn_Exp*
_temp1190; struct _tagged_arr* _temp1192; struct Cyc_Absyn_Exp* _temp1194;
struct Cyc_Absyn_Exp* _temp1196; struct Cyc_Absyn_Exp* _temp1198; struct Cyc_List_List*
_temp1200; struct Cyc_List_List* _temp1202; struct Cyc_Absyn_Exp* _temp1204;
struct Cyc_Absyn_Exp* _temp1206; struct Cyc_Absyn_Vardecl* _temp1208; struct Cyc_Absyn_Structdecl*
_temp1210; struct Cyc_List_List* _temp1212; struct Cyc_Core_Opt* _temp1214;
struct _tuple0* _temp1216; struct Cyc_List_List* _temp1218; void* _temp1220;
struct Cyc_Absyn_Tunionfield* _temp1222; struct Cyc_Absyn_Tuniondecl* _temp1224;
struct Cyc_List_List* _temp1226; struct Cyc_Core_Opt* _temp1228; struct Cyc_Core_Opt*
_temp1230; struct Cyc_Absyn_Tunionfield* _temp1232; struct Cyc_Absyn_Tuniondecl*
_temp1234; struct Cyc_List_List* _temp1236; struct Cyc_Core_Opt* _temp1238;
struct Cyc_Core_Opt* _temp1240; struct Cyc_Absyn_Enumfield* _temp1242; struct
Cyc_Absyn_Enumdecl* _temp1244; struct _tuple0* _temp1246; void* _temp1248;
struct Cyc_Absyn_Exp* _temp1250; struct Cyc_Absyn_Stmt* _temp1252; struct Cyc_List_List*
_temp1254; struct Cyc_Core_Opt* _temp1256; struct Cyc_List_List* _temp1258;
struct _tuple1* _temp1260; struct Cyc_Absyn_Fndecl* _temp1262; struct Cyc_Absyn_Exp*
_temp1264; _LL1024: if(*(( int*) _temp1022) == Cyc_Absyn_Const_e){ _LL1103:
_temp1102=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1022)->f1; if(
_temp1102 ==( void*) Cyc_Absyn_Null_c){ goto _LL1025;} else{ goto _LL1026;}}
else{ goto _LL1026;} _LL1026: if(*(( int*) _temp1022) == Cyc_Absyn_Const_e){
_LL1105: _temp1104=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1022)->f1;
goto _LL1027;} else{ goto _LL1028;} _LL1028: if(*(( int*) _temp1022) == Cyc_Absyn_Var_e){
_LL1109: _temp1108=(( struct Cyc_Absyn_Var_e_struct*) _temp1022)->f1; goto
_LL1107; _LL1107: _temp1106=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1022)->f2;
goto _LL1029;} else{ goto _LL1030;} _LL1030: if(*(( int*) _temp1022) == Cyc_Absyn_UnknownId_e){
_LL1111: _temp1110=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1022)->f1; goto
_LL1031;} else{ goto _LL1032;} _LL1032: if(*(( int*) _temp1022) == Cyc_Absyn_Primop_e){
_LL1115: _temp1114=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1022)->f1;
goto _LL1113; _LL1113: _temp1112=(( struct Cyc_Absyn_Primop_e_struct*) _temp1022)->f2;
goto _LL1033;} else{ goto _LL1034;} _LL1034: if(*(( int*) _temp1022) == Cyc_Absyn_Increment_e){
_LL1119: _temp1118=(( struct Cyc_Absyn_Increment_e_struct*) _temp1022)->f1; goto
_LL1117; _LL1117: _temp1116=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1022)->f2; goto _LL1035;} else{ goto _LL1036;} _LL1036: if(*(( int*)
_temp1022) == Cyc_Absyn_AssignOp_e){ _LL1125: _temp1124=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1022)->f1; goto _LL1123; _LL1123: _temp1122=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1022)->f2; goto _LL1121; _LL1121: _temp1120=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1022)->f3; goto _LL1037;} else{ goto _LL1038;} _LL1038: if(*(( int*)
_temp1022) == Cyc_Absyn_Conditional_e){ _LL1131: _temp1130=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1022)->f1; goto _LL1129; _LL1129: _temp1128=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1022)->f2; goto _LL1127; _LL1127: _temp1126=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1022)->f3; goto _LL1039;} else{ goto _LL1040;} _LL1040: if(*(( int*)
_temp1022) == Cyc_Absyn_SeqExp_e){ _LL1135: _temp1134=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1022)->f1; goto _LL1133; _LL1133: _temp1132=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1022)->f2; goto _LL1041;} else{ goto _LL1042;} _LL1042: if(*(( int*)
_temp1022) == Cyc_Absyn_UnknownCall_e){ _LL1139: _temp1138=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1022)->f1; goto _LL1137; _LL1137: _temp1136=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1022)->f2; goto _LL1043;} else{ goto _LL1044;} _LL1044: if(*(( int*)
_temp1022) == Cyc_Absyn_FnCall_e){ _LL1145: _temp1144=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1022)->f1; goto _LL1143; _LL1143: _temp1142=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1022)->f2; goto _LL1141; _LL1141: _temp1140=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1022)->f3; if( _temp1140 == 0){ goto _LL1045;} else{ goto _LL1046;}} else{
goto _LL1046;} _LL1046: if(*(( int*) _temp1022) == Cyc_Absyn_FnCall_e){ _LL1158:
_temp1157=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1022)->f1; goto _LL1156;
_LL1156: _temp1155=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1022)->f2; goto
_LL1147; _LL1147: _temp1146=(( struct Cyc_Absyn_FnCall_e_struct*) _temp1022)->f3;
if( _temp1146 == 0){ goto _LL1048;} else{ _temp1148=* _temp1146; _LL1154:
_temp1153= _temp1148.num_varargs; goto _LL1152; _LL1152: _temp1151= _temp1148.injectors;
goto _LL1150; _LL1150: _temp1149= _temp1148.vai; goto _LL1047;}} else{ goto
_LL1048;} _LL1048: if(*(( int*) _temp1022) == Cyc_Absyn_Throw_e){ _LL1160:
_temp1159=(( struct Cyc_Absyn_Throw_e_struct*) _temp1022)->f1; goto _LL1049;}
else{ goto _LL1050;} _LL1050: if(*(( int*) _temp1022) == Cyc_Absyn_NoInstantiate_e){
_LL1162: _temp1161=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1022)->f1;
goto _LL1051;} else{ goto _LL1052;} _LL1052: if(*(( int*) _temp1022) == Cyc_Absyn_Instantiate_e){
_LL1166: _temp1165=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1022)->f1;
goto _LL1164; _LL1164: _temp1163=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1022)->f2; goto _LL1053;} else{ goto _LL1054;} _LL1054: if(*(( int*)
_temp1022) == Cyc_Absyn_Cast_e){ _LL1170: _temp1169=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1022)->f1; _temp1171=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1022)->f1); goto _LL1168; _LL1168: _temp1167=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1022)->f2; goto _LL1055;} else{ goto _LL1056;} _LL1056: if(*(( int*)
_temp1022) == Cyc_Absyn_Address_e){ _LL1173: _temp1172=(( struct Cyc_Absyn_Address_e_struct*)
_temp1022)->f1; goto _LL1057;} else{ goto _LL1058;} _LL1058: if(*(( int*)
_temp1022) == Cyc_Absyn_New_e){ _LL1177: _temp1176=(( struct Cyc_Absyn_New_e_struct*)
_temp1022)->f1; goto _LL1175; _LL1175: _temp1174=(( struct Cyc_Absyn_New_e_struct*)
_temp1022)->f2; goto _LL1059;} else{ goto _LL1060;} _LL1060: if(*(( int*)
_temp1022) == Cyc_Absyn_Sizeofexp_e){ _LL1179: _temp1178=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1022)->f1; goto _LL1061;} else{ goto _LL1062;} _LL1062: if(*(( int*)
_temp1022) == Cyc_Absyn_Sizeoftyp_e){ _LL1181: _temp1180=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1022)->f1; goto _LL1063;} else{ goto _LL1064;} _LL1064: if(*(( int*)
_temp1022) == Cyc_Absyn_Offsetof_e){ _LL1185: _temp1184=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1022)->f1; goto _LL1183; _LL1183: _temp1182=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1022)->f2; goto _LL1065;} else{ goto _LL1066;} _LL1066: if(*(( int*)
_temp1022) == Cyc_Absyn_Deref_e){ _LL1187: _temp1186=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1022)->f1; goto _LL1067;} else{ goto _LL1068;} _LL1068: if(*(( int*)
_temp1022) == Cyc_Absyn_StructMember_e){ _LL1191: _temp1190=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1022)->f1; goto _LL1189; _LL1189: _temp1188=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1022)->f2; goto _LL1069;} else{ goto _LL1070;} _LL1070: if(*(( int*)
_temp1022) == Cyc_Absyn_StructArrow_e){ _LL1195: _temp1194=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1022)->f1; goto _LL1193; _LL1193: _temp1192=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1022)->f2; goto _LL1071;} else{ goto _LL1072;} _LL1072: if(*(( int*)
_temp1022) == Cyc_Absyn_Subscript_e){ _LL1199: _temp1198=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1022)->f1; goto _LL1197; _LL1197: _temp1196=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1022)->f2; goto _LL1073;} else{ goto _LL1074;} _LL1074: if(*(( int*)
_temp1022) == Cyc_Absyn_Tuple_e){ _LL1201: _temp1200=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1022)->f1; goto _LL1075;} else{ goto _LL1076;} _LL1076: if(*(( int*)
_temp1022) == Cyc_Absyn_Array_e){ _LL1203: _temp1202=(( struct Cyc_Absyn_Array_e_struct*)
_temp1022)->f1; goto _LL1077;} else{ goto _LL1078;} _LL1078: if(*(( int*)
_temp1022) == Cyc_Absyn_Comprehension_e){ _LL1209: _temp1208=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1022)->f1; goto _LL1207; _LL1207: _temp1206=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1022)->f2; goto _LL1205; _LL1205: _temp1204=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1022)->f3; goto _LL1079;} else{ goto _LL1080;} _LL1080: if(*(( int*)
_temp1022) == Cyc_Absyn_Struct_e){ _LL1217: _temp1216=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1022)->f1; goto _LL1215; _LL1215: _temp1214=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1022)->f2; goto _LL1213; _LL1213: _temp1212=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1022)->f3; goto _LL1211; _LL1211: _temp1210=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1022)->f4; goto _LL1081;} else{ goto _LL1082;} _LL1082: if(*(( int*)
_temp1022) == Cyc_Absyn_AnonStruct_e){ _LL1221: _temp1220=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1022)->f1; goto _LL1219; _LL1219: _temp1218=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1022)->f2; goto _LL1083;} else{ goto _LL1084;} _LL1084: if(*(( int*)
_temp1022) == Cyc_Absyn_Tunion_e){ _LL1231: _temp1230=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1022)->f1; goto _LL1229; _LL1229: _temp1228=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1022)->f2; goto _LL1227; _LL1227: _temp1226=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1022)->f3; if( _temp1226 == 0){ goto _LL1225;} else{ goto _LL1086;} _LL1225:
_temp1224=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1022)->f4; goto _LL1223;
_LL1223: _temp1222=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1022)->f5; goto
_LL1085;} else{ goto _LL1086;} _LL1086: if(*(( int*) _temp1022) == Cyc_Absyn_Tunion_e){
_LL1241: _temp1240=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1022)->f1; goto
_LL1239; _LL1239: _temp1238=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1022)->f2;
goto _LL1237; _LL1237: _temp1236=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1022)->f3;
goto _LL1235; _LL1235: _temp1234=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1022)->f4;
goto _LL1233; _LL1233: _temp1232=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1022)->f5;
goto _LL1087;} else{ goto _LL1088;} _LL1088: if(*(( int*) _temp1022) == Cyc_Absyn_Enum_e){
_LL1247: _temp1246=(( struct Cyc_Absyn_Enum_e_struct*) _temp1022)->f1; goto
_LL1245; _LL1245: _temp1244=(( struct Cyc_Absyn_Enum_e_struct*) _temp1022)->f2;
goto _LL1243; _LL1243: _temp1242=(( struct Cyc_Absyn_Enum_e_struct*) _temp1022)->f3;
goto _LL1089;} else{ goto _LL1090;} _LL1090: if(*(( int*) _temp1022) == Cyc_Absyn_Malloc_e){
_LL1251: _temp1250=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1022)->f1; goto
_LL1249; _LL1249: _temp1248=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1022)->f2; goto _LL1091;} else{ goto _LL1092;} _LL1092: if(*(( int*)
_temp1022) == Cyc_Absyn_StmtExp_e){ _LL1253: _temp1252=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1022)->f1; goto _LL1093;} else{ goto _LL1094;} _LL1094: if(*(( int*)
_temp1022) == Cyc_Absyn_UnresolvedMem_e){ _LL1257: _temp1256=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1022)->f1; goto _LL1255; _LL1255: _temp1254=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1022)->f2; goto _LL1095;} else{ goto _LL1096;} _LL1096: if(*(( int*)
_temp1022) == Cyc_Absyn_CompoundLit_e){ _LL1261: _temp1260=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1022)->f1; goto _LL1259; _LL1259: _temp1258=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1022)->f2; goto _LL1097;} else{ goto _LL1098;} _LL1098: if(*(( int*)
_temp1022) == Cyc_Absyn_Codegen_e){ _LL1263: _temp1262=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1022)->f1; goto _LL1099;} else{ goto _LL1100;} _LL1100: if(*(( int*)
_temp1022) == Cyc_Absyn_Fill_e){ _LL1265: _temp1264=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1022)->f1; goto _LL1101;} else{ goto _LL1023;} _LL1025: { struct Cyc_Absyn_Exp*
_temp1266= Cyc_Absyn_uint_exp( 0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ if( nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr(
old_typ, _temp1266, _temp1266))->r));} else{( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1267)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1270= _temp1266; struct Cyc_Absyn_Exp* _temp1271=
_temp1266; struct Cyc_Absyn_Exp* _temp1272= _temp1266; struct Cyc_Absyn_Exp*
_temp1268[ 3u]={ _temp1270, _temp1271, _temp1272}; struct _tagged_arr _temp1269={(
void*) _temp1268,( void*) _temp1268,( void*)( _temp1268 + 3u)}; _temp1267(
_temp1269);}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL1023;} _LL1027: goto _LL1023; _LL1029:{ struct
_handler_cons _temp1273; _push_handler(& _temp1273);{ int _temp1275= 0; if(
setjmp( _temp1273.handler)){ _temp1275= 1;} if( ! _temp1275){( void*)( e->r=(
void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key)) Cyc_Dict_lookup)( nv->varmap, _temp1108))->r));; _pop_handler();}
else{ void* _temp1274=( void*) _exn_thrown; void* _temp1277= _temp1274; _LL1279:
if( _temp1277 == Cyc_Dict_Absent){ goto _LL1280;} else{ goto _LL1281;} _LL1281:
goto _LL1282; _LL1280:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1283= Cyc_Absynpp_qvar2string(
_temp1108); xprintf("Can't find %.*s in exp_to_c, Var\n", _get_arr_size(
_temp1283, 1u), _temp1283.curr);})); return; _LL1282:( void) _throw( _temp1277);
_LL1278:;}}} goto _LL1023; _LL1031:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("unknownid", sizeof( unsigned char), 10u)); goto _LL1023; _LL1033: {
struct Cyc_List_List* _temp1284=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1112); struct Cyc_List_List* _temp1285=(( struct Cyc_List_List*(*)( void*(*
f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1112);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1112);{ void* _temp1286= _temp1114; _LL1288: if( _temp1286 ==( void*)
Cyc_Absyn_Size){ goto _LL1289;} else{ goto _LL1290;} _LL1290: if( _temp1286 ==(
void*) Cyc_Absyn_Printf){ goto _LL1291;} else{ goto _LL1292;} _LL1292: if(
_temp1286 ==( void*) Cyc_Absyn_Xprintf){ goto _LL1293;} else{ goto _LL1294;}
_LL1294: if( _temp1286 ==( void*) Cyc_Absyn_Fprintf){ goto _LL1295;} else{ goto
_LL1296;} _LL1296: if( _temp1286 ==( void*) Cyc_Absyn_Sscanf){ goto _LL1297;}
else{ goto _LL1298;} _LL1298: if( _temp1286 ==( void*) Cyc_Absyn_Plus){ goto
_LL1299;} else{ goto _LL1300;} _LL1300: if( _temp1286 ==( void*) Cyc_Absyn_Minus){
goto _LL1301;} else{ goto _LL1302;} _LL1302: if( _temp1286 ==( void*) Cyc_Absyn_Eq){
goto _LL1303;} else{ goto _LL1304;} _LL1304: if( _temp1286 ==( void*) Cyc_Absyn_Neq){
goto _LL1305;} else{ goto _LL1306;} _LL1306: if( _temp1286 ==( void*) Cyc_Absyn_Gt){
goto _LL1307;} else{ goto _LL1308;} _LL1308: if( _temp1286 ==( void*) Cyc_Absyn_Gte){
goto _LL1309;} else{ goto _LL1310;} _LL1310: if( _temp1286 ==( void*) Cyc_Absyn_Lt){
goto _LL1311;} else{ goto _LL1312;} _LL1312: if( _temp1286 ==( void*) Cyc_Absyn_Lte){
goto _LL1313;} else{ goto _LL1314;} _LL1314: goto _LL1315; _LL1289: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1112))->hd;{
void* _temp1316=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1324; struct Cyc_Absyn_Tqual _temp1326; void*
_temp1328; struct Cyc_Absyn_PtrInfo _temp1330; struct Cyc_Absyn_Conref*
_temp1332; struct Cyc_Absyn_Tqual _temp1334; struct Cyc_Absyn_Conref* _temp1336;
void* _temp1338; void* _temp1340; _LL1318: if(( unsigned int) _temp1316 > 4u?*((
int*) _temp1316) == Cyc_Absyn_ArrayType: 0){ _LL1329: _temp1328=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1316)->f1; goto _LL1327; _LL1327:
_temp1326=(( struct Cyc_Absyn_ArrayType_struct*) _temp1316)->f2; goto _LL1325;
_LL1325: _temp1324=(( struct Cyc_Absyn_ArrayType_struct*) _temp1316)->f3; goto
_LL1319;} else{ goto _LL1320;} _LL1320: if(( unsigned int) _temp1316 > 4u?*((
int*) _temp1316) == Cyc_Absyn_PointerType: 0){ _LL1331: _temp1330=(( struct Cyc_Absyn_PointerType_struct*)
_temp1316)->f1; _LL1341: _temp1340=( void*) _temp1330.elt_typ; goto _LL1339;
_LL1339: _temp1338=( void*) _temp1330.rgn_typ; goto _LL1337; _LL1337: _temp1336=
_temp1330.nullable; goto _LL1335; _LL1335: _temp1334= _temp1330.tq; goto _LL1333;
_LL1333: _temp1332= _temp1330.bounds; goto _LL1321;} else{ goto _LL1322;}
_LL1322: goto _LL1323; _LL1319:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1324))->r)); goto _LL1317; _LL1321:{ void* _temp1342=(( void*(*)(
void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1332); struct Cyc_Absyn_Exp* _temp1348; _LL1344: if( _temp1342 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1345;} else{ goto _LL1346;} _LL1346: if((
unsigned int) _temp1342 > 1u?*(( int*) _temp1342) == Cyc_Absyn_Upper_b: 0){
_LL1349: _temp1348=(( struct Cyc_Absyn_Upper_b_struct*) _temp1342)->f1; goto
_LL1347;} else{ goto _LL1343;} _LL1345:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp1350)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1353=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1112))->hd; struct Cyc_Absyn_Exp* _temp1354= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( _temp1340), 0); struct Cyc_Absyn_Exp* _temp1351[ 2u]={
_temp1353, _temp1354}; struct _tagged_arr _temp1352={( void*) _temp1351,( void*)
_temp1351,( void*)( _temp1351 + 2u)}; _temp1350( _temp1352);}), 0))->r)); goto
_LL1343; _LL1347:( void*)( e->r=( void*)(( void*) _temp1348->r)); goto _LL1343;
_LL1343:;} goto _LL1317; _LL1323:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp1355= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_List_List*) _check_null( _temp1285))->hd); struct
_tagged_arr _temp1356= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); xprintf("size primop applied to non-array %.*s (%.*s)",
_get_arr_size( _temp1355, 1u), _temp1355.curr, _get_arr_size( _temp1356, 1u),
_temp1356.curr);})); return; _LL1317:;} goto _LL1287;} _LL1291:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1114, 0, _temp1112, _temp1285))->r));
goto _LL1287; _LL1293:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1114, 0, _temp1112, _temp1285))->r)); goto _LL1287; _LL1295: if( _temp1112
== 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("fprintf without arguments",
sizeof( unsigned char), 26u));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1114,({ struct Cyc_Core_Opt* _temp1357=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1357->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1112))->hd); _temp1357;}),(( struct Cyc_List_List*)
_check_null( _temp1112))->tl,(( struct Cyc_List_List*) _check_null( _temp1285))->tl))->r));
goto _LL1287; _LL1297: if( _temp1112 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("sscanf without arguments", sizeof( unsigned char), 25u));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1112))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1112))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1112))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1287; _LL1299: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1284))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1112))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1112))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1358)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1361= e1; struct Cyc_Absyn_Exp* _temp1362= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1363= e2; struct Cyc_Absyn_Exp*
_temp1359[ 3u]={ _temp1361, _temp1362, _temp1363}; struct _tagged_arr _temp1360={(
void*) _temp1359,( void*) _temp1359,( void*)( _temp1359 + 3u)}; _temp1358(
_temp1360);}), 0))->r));} goto _LL1287;} _LL1301: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1284))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1112))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1112))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1284))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1364)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1367= e1; struct Cyc_Absyn_Exp* _temp1368= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1369= Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Minus, e2, 0); struct Cyc_Absyn_Exp* _temp1365[ 3u]={ _temp1367,
_temp1368, _temp1369}; struct _tagged_arr _temp1366={( void*) _temp1365,( void*)
_temp1365,( void*)( _temp1365 + 3u)}; _temp1364( _temp1366);}), 0))->r));}} goto
_LL1287;} _LL1303: goto _LL1305; _LL1305: goto _LL1307; _LL1307: goto _LL1309;
_LL1309: goto _LL1311; _LL1311: goto _LL1313; _LL1313: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1112))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1112))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1284))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1284))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1287;} _LL1315: goto
_LL1287; _LL1287:;} goto _LL1023;} _LL1035: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1118->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1118);{ void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1370=( _temp1116 ==( void*) Cyc_Absyn_PostInc?
1: _temp1116 ==( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1371=( _temp1116 ==( void*) Cyc_Absyn_PreInc?
1: _temp1116 ==( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1370,({ struct Cyc_List_List*(* _temp1372)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1375= Cyc_Absyn_address_exp( _temp1118, 0); struct
Cyc_Absyn_Exp* _temp1376= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
struct Cyc_Absyn_Exp* _temp1377= Cyc_Absyn_signed_int_exp( _temp1371, 0); struct
Cyc_Absyn_Exp* _temp1373[ 3u]={ _temp1375, _temp1376, _temp1377}; struct
_tagged_arr _temp1374={( void*) _temp1373,( void*) _temp1373,( void*)( _temp1373
+ 3u)}; _temp1372( _temp1374);}), 0))->r));} if( elt_typ ==( void*) Cyc_Absyn_VoidType?
! Cyc_Absyn_is_lvalue( _temp1118): 0){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1118, 0, Cyc_Toc_incr_lvalue,
_temp1116);( void*)( e->r=( void*)(( void*) _temp1118->r));} goto _LL1023;}}
_LL1037: { int e1_poly= Cyc_Toc_is_poly_project( _temp1124); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1124->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1120->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1124); Cyc_Toc_exp_to_c( nv, _temp1120);{ int done= 0;
if( _temp1122 != 0){ void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* change;{ void* _temp1378=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1122))->v; _LL1380: if( _temp1378 ==(
void*) Cyc_Absyn_Plus){ goto _LL1381;} else{ goto _LL1382;} _LL1382: if(
_temp1378 ==( void*) Cyc_Absyn_Minus){ goto _LL1383;} else{ goto _LL1384;}
_LL1384: goto _LL1385; _LL1381: change= _temp1120; goto _LL1379; _LL1383: change=
Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp1120, 0); goto _LL1379;
_LL1385:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("bad t ? pointer arithmetic", sizeof( unsigned char), 27u))); goto
_LL1379; _LL1379:;} done= 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_inplace_plus_e,({ struct Cyc_List_List*(* _temp1386)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1389= Cyc_Absyn_address_exp( _temp1124, 0); struct
Cyc_Absyn_Exp* _temp1390= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
struct Cyc_Absyn_Exp* _temp1391= change; struct Cyc_Absyn_Exp* _temp1387[ 3u]={
_temp1389, _temp1390, _temp1391}; struct _tagged_arr _temp1388={( void*)
_temp1387,( void*) _temp1387,( void*)( _temp1387 + 3u)}; _temp1386( _temp1388);}),
0))->r));}} if( ! done){ if( e1_poly){( void*)( _temp1120->r=( void*)(( void*)(
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1120->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp1124)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, struct _tuple9*), struct _tuple9* f_env)) Cyc_Toc_lvalue_assign)(
_temp1124, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple9* _temp1392=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9) * 1); _temp1392[ 0]=({ struct
_tuple9 _temp1393; _temp1393.f1= _temp1122; _temp1393.f2= _temp1120; _temp1393;});
_temp1392;}));( void*)( e->r=( void*)(( void*) _temp1124->r));}} goto _LL1023;}}
_LL1039: Cyc_Toc_exp_to_c( nv, _temp1130); Cyc_Toc_exp_to_c( nv, _temp1128); Cyc_Toc_exp_to_c(
nv, _temp1126); goto _LL1023; _LL1041: Cyc_Toc_exp_to_c( nv, _temp1134); Cyc_Toc_exp_to_c(
nv, _temp1132); goto _LL1023; _LL1043: _temp1144= _temp1138; _temp1142=
_temp1136; goto _LL1045; _LL1045: Cyc_Toc_exp_to_c( nv, _temp1144);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1142);
goto _LL1023; _LL1047:{ struct _RegionHandle _temp1394= _new_region(); struct
_RegionHandle* r=& _temp1394; _push_region( r);{ void* _temp1395=( void*)
_temp1149->type; void* _temp1396= Cyc_Toc_typ_to_c( _temp1395); void* _temp1397=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1157->topt))->v);
struct Cyc_Absyn_Exp* _temp1398= Cyc_Absyn_uint_exp(( unsigned int) _temp1153, 0);
void* arr_type= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1447=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1447[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1448; _temp1448.tag= Cyc_Absyn_ArrayType;
_temp1448.f1=( void*) _temp1395; _temp1448.f2= Cyc_Toc_mt_tq; _temp1448.f3=(
struct Cyc_Absyn_Exp*) _temp1398; _temp1448;}); _temp1447;})); struct _tuple0*
_temp1399= Cyc_Toc_temp_var(); struct _tuple0* _temp1400= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1401= Cyc_Absyn_var_exp( _temp1400, 0); struct
_tuple0* _temp1402= Cyc_Toc_temp_var(); int _temp1403=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1155); int _temp1404= _temp1403 - _temp1153; struct
_tagged_arr vs=({ unsigned int _temp1443=( unsigned int) _temp1153; struct
_tuple0** _temp1444=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1443); struct _tagged_arr _temp1446= _tag_arr( _temp1444,
sizeof( struct _tuple0*),( unsigned int) _temp1153);{ unsigned int _temp1445=
_temp1443; unsigned int i; for( i= 0; i < _temp1445; i ++){ _temp1444[ i]= Cyc_Toc_temp_var();}};
_temp1446;}); struct _tagged_arr xs=({ unsigned int _temp1439=( unsigned int)
_temp1404; struct _tuple0** _temp1440=( struct _tuple0**) _region_malloc( r,
sizeof( struct _tuple0*) * _temp1439); struct _tagged_arr _temp1442= _tag_arr(
_temp1440, sizeof( struct _tuple0*),( unsigned int) _temp1404);{ unsigned int
_temp1441= _temp1439; unsigned int i; for( i= 0; i < _temp1441; i ++){ _temp1440[
i]= Cyc_Toc_temp_var();}}; _temp1442;}); struct Cyc_List_List* xexps=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,(
struct Cyc_Position_Segment*) 0,(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, xs));
xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List* _temp1405=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1405->hd=( void*) Cyc_Absyn_var_exp(
_temp1402, 0); _temp1405->tl= 0; _temp1405;}));{ struct Cyc_Absyn_Stmt*
_temp1406= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
_temp1399, 0), xexps, 0), 0); struct Cyc_Absyn_Exp* _temp1407= Cyc_Absyn_add_exp(
_temp1401, _temp1398, 0); struct Cyc_List_List* _temp1408=({ struct Cyc_List_List*(*
_temp1430)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct _tuple4* _temp1433=({ struct _tuple4* _temp1438=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1438->f1= 0; _temp1438->f2=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1401, 0); _temp1438;});
struct _tuple4* _temp1434=({ struct _tuple4* _temp1437=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1437->f1= 0; _temp1437->f2= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1401, 0); _temp1437;}); struct _tuple4*
_temp1435=({ struct _tuple4* _temp1436=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1436->f1= 0; _temp1436->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1407, 0); _temp1436;}); struct _tuple4* _temp1431[ 3u]={ _temp1433,
_temp1434, _temp1435}; struct _tagged_arr _temp1432={( void*) _temp1431,( void*)
_temp1431,( void*)( _temp1431 + 3u)}; _temp1430( _temp1432);}); _temp1406= Cyc_Absyn_declare_stmt(
_temp1402, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp1395,( void*) _temp1149->rgn,
Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1408, 0), _temp1406, 0);{ struct Cyc_List_List* _temp1409=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, vs));
struct Cyc_List_List* _temp1410=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_make_dle,
_temp1409); _temp1406= Cyc_Absyn_declare_stmt( _temp1400, arr_type,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp( 0, _temp1410, 0), _temp1406, 0);{ struct Cyc_List_List*
_temp1411=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, _temp1155); struct Cyc_List_List* _temp1412=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1151);{
int i= _temp1153 - 1; for( 0; i >= 0;( i --, _temp1411=(( struct Cyc_List_List*)
_check_null( _temp1411))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1411))->hd; void* arg_type=( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c( nv, arg);
if( _temp1149->inject){ void* _temp1415; struct Cyc_Position_Segment* _temp1417;
struct Cyc_List_List* _temp1419; struct Cyc_List_List* _temp1421; struct _tuple0*
_temp1423; struct Cyc_Absyn_Tunionfield _temp1413=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1412))->hd); _LL1424: _temp1423=
_temp1413.name; goto _LL1422; _LL1422: _temp1421= _temp1413.tvs; goto _LL1420;
_LL1420: _temp1419= _temp1413.typs; goto _LL1418; _LL1418: _temp1417= _temp1413.loc;
goto _LL1416; _LL1416: _temp1415=( void*) _temp1413.sc; goto _LL1414; _LL1414:
_temp1412=(( struct Cyc_List_List*) _check_null( _temp1412))->tl;{ struct
_tuple0* _temp1425= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1426= Cyc_Absyn_var_exp(
_temp1425, 0); void* _temp1427= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp1423, _tag_arr("_struct", sizeof( unsigned char), 8u))); _temp1406= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1396,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1396, Cyc_Absyn_address_exp(
_temp1426, 0), 0), _temp1406, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple3*)(( struct Cyc_List_List*) _check_null( _temp1419))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1428= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1426, Cyc_Toc_fieldname(
1), 0), arg, 0); _temp1406= Cyc_Absyn_seq_stmt( _temp1428, _temp1406, 0);{
struct Cyc_Absyn_Stmt* _temp1429= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1426, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1423, 0), 0); _temp1406=
Cyc_Absyn_seq_stmt( _temp1429, _temp1406, 0); _temp1406= Cyc_Absyn_declare_stmt(
_temp1425, _temp1427, 0, _temp1406, 0);}}}}} else{ _temp1406= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1396,( struct Cyc_Absyn_Exp*) arg, _temp1406, 0);}}}{ int i= _temp1404 - 1;
for( 0; i >= 0;( i --, _temp1411=(( struct Cyc_List_List*) _check_null(
_temp1411))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1411))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1406= Cyc_Absyn_declare_stmt(*(( struct _tuple0**)
_check_unknown_subscript( xs, sizeof( struct _tuple0*), i)), argtype,( struct
Cyc_Absyn_Exp*) arg, _temp1406, 0);}} Cyc_Toc_exp_to_c( nv, _temp1157);
_temp1406= Cyc_Absyn_declare_stmt( _temp1399, _temp1397,( struct Cyc_Absyn_Exp*)
_temp1157, _temp1406, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1406, 0))->r));}}}}; _pop_region( r);} goto _LL1023; _LL1049: Cyc_Toc_exp_to_c(
nv, _temp1159);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1159), 0))->r)); goto _LL1023; _LL1051: Cyc_Toc_exp_to_c(
nv, _temp1161); goto _LL1023; _LL1053: Cyc_Toc_exp_to_c( nv, _temp1165);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
for( 0; _temp1163 != 0; _temp1163=(( struct Cyc_List_List*) _check_null(
_temp1163))->tl){ void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*)
_check_null( _temp1163))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*)
Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1165, 0))->r)); break;}} goto _LL1023;} _LL1055: { void* old_t2=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1167->topt))->v; void* new_typ=*
_temp1171;* _temp1171= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,
_temp1167);{ struct _tuple10 _temp1450=({ struct _tuple10 _temp1449; _temp1449.f1=
Cyc_Tcutil_compress( old_t2); _temp1449.f2= Cyc_Tcutil_compress( new_typ);
_temp1449;}); void* _temp1458; struct Cyc_Absyn_PtrInfo _temp1460; void*
_temp1462; struct Cyc_Absyn_PtrInfo _temp1464; void* _temp1466; void* _temp1468;
void* _temp1470; void* _temp1472; struct Cyc_Absyn_PtrInfo _temp1474; _LL1452:
_LL1463: _temp1462= _temp1450.f1; if(( unsigned int) _temp1462 > 4u?*(( int*)
_temp1462) == Cyc_Absyn_PointerType: 0){ _LL1465: _temp1464=(( struct Cyc_Absyn_PointerType_struct*)
_temp1462)->f1; goto _LL1459;} else{ goto _LL1454;} _LL1459: _temp1458=
_temp1450.f2; if(( unsigned int) _temp1458 > 4u?*(( int*) _temp1458) == Cyc_Absyn_PointerType:
0){ _LL1461: _temp1460=(( struct Cyc_Absyn_PointerType_struct*) _temp1458)->f1;
goto _LL1453;} else{ goto _LL1454;} _LL1454: _LL1473: _temp1472= _temp1450.f1;
if(( unsigned int) _temp1472 > 4u?*(( int*) _temp1472) == Cyc_Absyn_PointerType:
0){ _LL1475: _temp1474=(( struct Cyc_Absyn_PointerType_struct*) _temp1472)->f1;
goto _LL1467;} else{ goto _LL1456;} _LL1467: _temp1466= _temp1450.f2; if((
unsigned int) _temp1466 > 4u?*(( int*) _temp1466) == Cyc_Absyn_IntType: 0){
_LL1471: _temp1470=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1466)->f1;
goto _LL1469; _LL1469: _temp1468=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1466)->f2; goto _LL1455;} else{ goto _LL1456;} _LL1456: goto _LL1457;
_LL1453: { int _temp1476=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1464.nullable); int _temp1477=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1460.nullable); void* _temp1478=(( void*(*)( void*,
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1464.bounds); void* _temp1479=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1460.bounds);{ struct
_tuple10 _temp1481=({ struct _tuple10 _temp1480; _temp1480.f1= _temp1478;
_temp1480.f2= _temp1479; _temp1480;}); void* _temp1491; struct Cyc_Absyn_Exp*
_temp1493; void* _temp1495; struct Cyc_Absyn_Exp* _temp1497; void* _temp1499;
void* _temp1501; struct Cyc_Absyn_Exp* _temp1503; void* _temp1505; struct Cyc_Absyn_Exp*
_temp1507; void* _temp1509; _LL1483: _LL1496: _temp1495= _temp1481.f1; if((
unsigned int) _temp1495 > 1u?*(( int*) _temp1495) == Cyc_Absyn_Upper_b: 0){
_LL1498: _temp1497=(( struct Cyc_Absyn_Upper_b_struct*) _temp1495)->f1; goto
_LL1492;} else{ goto _LL1485;} _LL1492: _temp1491= _temp1481.f2; if((
unsigned int) _temp1491 > 1u?*(( int*) _temp1491) == Cyc_Absyn_Upper_b: 0){
_LL1494: _temp1493=(( struct Cyc_Absyn_Upper_b_struct*) _temp1491)->f1; goto
_LL1484;} else{ goto _LL1485;} _LL1485: _LL1502: _temp1501= _temp1481.f1; if((
unsigned int) _temp1501 > 1u?*(( int*) _temp1501) == Cyc_Absyn_Upper_b: 0){
_LL1504: _temp1503=(( struct Cyc_Absyn_Upper_b_struct*) _temp1501)->f1; goto
_LL1500;} else{ goto _LL1487;} _LL1500: _temp1499= _temp1481.f2; if( _temp1499
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1486;} else{ goto _LL1487;} _LL1487:
_LL1510: _temp1509= _temp1481.f1; if( _temp1509 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1506;} else{ goto _LL1489;} _LL1506: _temp1505= _temp1481.f2; if((
unsigned int) _temp1505 > 1u?*(( int*) _temp1505) == Cyc_Absyn_Upper_b: 0){
_LL1508: _temp1507=(( struct Cyc_Absyn_Upper_b_struct*) _temp1505)->f1; goto
_LL1488;} else{ goto _LL1489;} _LL1489: goto _LL1490; _LL1484: if( _temp1476? !
_temp1477: 0){ if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't do null-check conversion at top-level", sizeof( unsigned char),
44u));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp1171, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1511=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1511->hd=( void*) _temp1167;
_temp1511->tl= 0; _temp1511;}), 0), 0))->r));} goto _LL1482; _LL1486: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1503, _temp1167))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1512)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1515= _temp1167; struct Cyc_Absyn_Exp* _temp1516= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1460.elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1517= _temp1503; struct Cyc_Absyn_Exp* _temp1513[ 3u]={ _temp1515,
_temp1516, _temp1517}; struct _tagged_arr _temp1514={( void*) _temp1513,( void*)
_temp1513,( void*)( _temp1513 + 3u)}; _temp1512( _temp1514);}), 0))->r));} goto
_LL1482; _LL1488: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u));}{ struct Cyc_Absyn_Exp* _temp1518= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({
struct Cyc_List_List*(* _temp1520)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1523= _temp1167;
struct Cyc_Absyn_Exp* _temp1524= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c((
void*) _temp1464.elt_typ), 0); struct Cyc_Absyn_Exp* _temp1525= _temp1507;
struct Cyc_Absyn_Exp* _temp1521[ 3u]={ _temp1523, _temp1524, _temp1525}; struct
_tagged_arr _temp1522={( void*) _temp1521,( void*) _temp1521,( void*)( _temp1521
+ 3u)}; _temp1520( _temp1522);}), 0); if( _temp1477){( void*)( _temp1518->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({ struct Cyc_List_List*
_temp1519=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1519->hd=( void*) Cyc_Absyn_copy_exp( _temp1518); _temp1519->tl= 0;
_temp1519;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp1171, _temp1518, 0))->r)); goto _LL1482;} _LL1490: goto _LL1482; _LL1482:;}
goto _LL1451;} _LL1455:{ void* _temp1526=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1474.bounds); _LL1528: if(
_temp1526 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1529;} else{ goto _LL1530;}
_LL1530: goto _LL1531; _LL1529:( void*)( _temp1167->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1167->r, _temp1167->loc), Cyc_Toc_curr_sp, 0))->r));
goto _LL1527; _LL1531: goto _LL1527; _LL1527:;} goto _LL1451; _LL1457: goto
_LL1451; _LL1451:;} goto _LL1023;} _LL1057:{ void* _temp1532=( void*) _temp1172->r;
struct Cyc_Absyn_Structdecl* _temp1540; struct Cyc_List_List* _temp1542; struct
Cyc_Core_Opt* _temp1544; struct _tuple0* _temp1546; struct Cyc_List_List*
_temp1548; _LL1534: if(*(( int*) _temp1532) == Cyc_Absyn_Struct_e){ _LL1547:
_temp1546=(( struct Cyc_Absyn_Struct_e_struct*) _temp1532)->f1; goto _LL1545;
_LL1545: _temp1544=(( struct Cyc_Absyn_Struct_e_struct*) _temp1532)->f2; goto
_LL1543; _LL1543: _temp1542=(( struct Cyc_Absyn_Struct_e_struct*) _temp1532)->f3;
goto _LL1541; _LL1541: _temp1540=(( struct Cyc_Absyn_Struct_e_struct*) _temp1532)->f4;
goto _LL1535;} else{ goto _LL1536;} _LL1536: if(*(( int*) _temp1532) == Cyc_Absyn_Tuple_e){
_LL1549: _temp1548=(( struct Cyc_Absyn_Tuple_e_struct*) _temp1532)->f1; goto
_LL1537;} else{ goto _LL1538;} _LL1538: goto _LL1539; _LL1535: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr _temp1550=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1172->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_get_arr_size( _temp1550, 1u), _temp1550.curr);}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1172->topt))->v, 1, 0, _temp1542, _temp1546))->r)); goto _LL1533; _LL1537:
if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct
_tagged_arr)({ struct _tagged_arr _temp1551=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1172->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_get_arr_size( _temp1551, 1u), _temp1551.curr);}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1548))->r)); goto _LL1533; _LL1539:
Cyc_Toc_exp_to_c( nv, _temp1172); if( ! Cyc_Absyn_is_lvalue( _temp1172)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1172, 0,
Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1172, 0))->r));} goto _LL1533; _LL1533:;} goto _LL1023; _LL1059: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr _temp1552=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1174->loc); xprintf("%.*s: new at top-level", _get_arr_size( _temp1552, 1u),
_temp1552.curr);}));}{ void* _temp1553=( void*) _temp1174->r; struct Cyc_List_List*
_temp1565; struct Cyc_Absyn_Exp* _temp1567; struct Cyc_Absyn_Exp* _temp1569;
struct Cyc_Absyn_Vardecl* _temp1571; struct Cyc_Absyn_Structdecl* _temp1573;
struct Cyc_List_List* _temp1575; struct Cyc_Core_Opt* _temp1577; struct _tuple0*
_temp1579; struct Cyc_List_List* _temp1581; _LL1555: if(*(( int*) _temp1553) ==
Cyc_Absyn_Array_e){ _LL1566: _temp1565=(( struct Cyc_Absyn_Array_e_struct*)
_temp1553)->f1; goto _LL1556;} else{ goto _LL1557;} _LL1557: if(*(( int*)
_temp1553) == Cyc_Absyn_Comprehension_e){ _LL1572: _temp1571=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1553)->f1; goto _LL1570; _LL1570: _temp1569=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1553)->f2; goto _LL1568; _LL1568: _temp1567=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1553)->f3; goto _LL1558;} else{ goto _LL1559;} _LL1559: if(*(( int*)
_temp1553) == Cyc_Absyn_Struct_e){ _LL1580: _temp1579=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1553)->f1; goto _LL1578; _LL1578: _temp1577=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1553)->f2; goto _LL1576; _LL1576: _temp1575=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1553)->f3; goto _LL1574; _LL1574: _temp1573=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1553)->f4; goto _LL1560;} else{ goto _LL1561;} _LL1561: if(*(( int*)
_temp1553) == Cyc_Absyn_Tuple_e){ _LL1582: _temp1581=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1553)->f1; goto _LL1562;} else{ goto _LL1563;} _LL1563: goto _LL1564;
_LL1556: { struct _tuple0* _temp1583= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1584= Cyc_Absyn_var_exp( _temp1583, 0); struct Cyc_Absyn_Stmt* _temp1585=
Cyc_Toc_init_array( nv, _temp1584, _temp1565, Cyc_Absyn_exp_stmt( _temp1584, 0));
void* old_elt_typ;{ void* _temp1586= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1592; struct Cyc_Absyn_Conref* _temp1594; struct Cyc_Absyn_Tqual _temp1596;
struct Cyc_Absyn_Conref* _temp1598; void* _temp1600; void* _temp1602; _LL1588:
if(( unsigned int) _temp1586 > 4u?*(( int*) _temp1586) == Cyc_Absyn_PointerType:
0){ _LL1593: _temp1592=(( struct Cyc_Absyn_PointerType_struct*) _temp1586)->f1;
_LL1603: _temp1602=( void*) _temp1592.elt_typ; goto _LL1601; _LL1601: _temp1600=(
void*) _temp1592.rgn_typ; goto _LL1599; _LL1599: _temp1598= _temp1592.nullable;
goto _LL1597; _LL1597: _temp1596= _temp1592.tq; goto _LL1595; _LL1595: _temp1594=
_temp1592.bounds; goto _LL1589;} else{ goto _LL1590;} _LL1590: goto _LL1591;
_LL1589: old_elt_typ= _temp1602; goto _LL1587; _LL1591: old_elt_typ=(( void*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u)); goto _LL1587; _LL1587:;}{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* _temp1604= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* _temp1605= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1565), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1176 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1605);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1176); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1605);}{ struct Cyc_Absyn_Exp* _temp1606= Cyc_Absyn_cast_exp( _temp1604, e1,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1583, _temp1604,( struct Cyc_Absyn_Exp*) _temp1606, _temp1585, 0), 0))->r));
goto _LL1554;}}} _LL1558: { int is_tagged_ptr= 0;{ void* _temp1607= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1613; struct Cyc_Absyn_Conref* _temp1615;
struct Cyc_Absyn_Tqual _temp1617; struct Cyc_Absyn_Conref* _temp1619; void*
_temp1621; void* _temp1623; _LL1609: if(( unsigned int) _temp1607 > 4u?*(( int*)
_temp1607) == Cyc_Absyn_PointerType: 0){ _LL1614: _temp1613=(( struct Cyc_Absyn_PointerType_struct*)
_temp1607)->f1; _LL1624: _temp1623=( void*) _temp1613.elt_typ; goto _LL1622;
_LL1622: _temp1621=( void*) _temp1613.rgn_typ; goto _LL1620; _LL1620: _temp1619=
_temp1613.nullable; goto _LL1618; _LL1618: _temp1617= _temp1613.tq; goto _LL1616;
_LL1616: _temp1615= _temp1613.bounds; goto _LL1610;} else{ goto _LL1611;}
_LL1611: goto _LL1612; _LL1610: is_tagged_ptr=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1615) ==( void*) Cyc_Absyn_Unknown_b;
goto _LL1608; _LL1612:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: comprehension not an array type", sizeof( unsigned char), 42u));
goto _LL1608; _LL1608:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct
_tuple0* a= Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1567->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1569);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1571, Cyc_Absyn_var_exp( max, 0), _temp1567, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1625= _new_region(); struct _RegionHandle* r=&
_temp1625; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1648=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1648->hd=( void*)({ struct _tuple8* _temp1649=( struct _tuple8*)
_region_malloc( r, sizeof( struct _tuple8)); _temp1649->f1= max; _temp1649->f2=
Cyc_Absyn_uint_t; _temp1649->f3=( struct Cyc_Absyn_Exp*) _temp1569; _temp1649;});
_temp1648->tl= 0; _temp1648;}); struct Cyc_Absyn_Exp* ai; if( _temp1176 == 0){
ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( _temp1176); Cyc_Toc_exp_to_c( nv, r); ai=
Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0),
Cyc_Absyn_var_exp( max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp(
ptr_typ, ai, 0); decls=({ struct Cyc_List_List* _temp1626=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1626->hd=( void*)({
struct _tuple8* _temp1627=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1627->f1= a; _temp1627->f2= ptr_typ; _temp1627->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1627;}); _temp1626->tl= decls; _temp1626;}); if( is_tagged_ptr){
struct _tuple0* _temp1628= Cyc_Toc_temp_var(); void* _temp1629= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Exp* _temp1630= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_List_List*(* _temp1633)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1636= Cyc_Absyn_var_exp(
a, 0); struct Cyc_Absyn_Exp* _temp1637= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
struct Cyc_Absyn_Exp* _temp1638= _temp1569; struct Cyc_Absyn_Exp* _temp1634[ 3u]={
_temp1636, _temp1637, _temp1638}; struct _tagged_arr _temp1635={( void*)
_temp1634,( void*) _temp1634,( void*)( _temp1634 + 3u)}; _temp1633( _temp1635);}),
0); decls=({ struct Cyc_List_List* _temp1631=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1631->hd=( void*)({
struct _tuple8* _temp1632=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1632->f1= _temp1628; _temp1632->f2= _temp1629; _temp1632->f3=(
struct Cyc_Absyn_Exp*) _temp1630; _temp1632;}); _temp1631->tl= decls; _temp1631;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1628, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1639= decls; for( 0; _temp1639 != 0;
_temp1639=(( struct Cyc_List_List*) _check_null( _temp1639))->tl){ struct Cyc_Absyn_Exp*
_temp1642; void* _temp1644; struct _tuple0* _temp1646; struct _tuple8 _temp1640=*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1639))->hd); _LL1647:
_temp1646= _temp1640.f1; goto _LL1645; _LL1645: _temp1644= _temp1640.f2; goto
_LL1643; _LL1643: _temp1642= _temp1640.f3; goto _LL1641; _LL1641: s= Cyc_Absyn_declare_stmt(
_temp1646, _temp1644, _temp1642, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL1554;}}} _LL1560:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1174->topt))->v, 1, _temp1176, _temp1575, _temp1579))->r)); goto _LL1554;
_LL1562:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1176,
_temp1581))->r)); goto _LL1554; _LL1564: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1174->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp1176 == 0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1176); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1650=( void*)
_temp1174->r; struct Cyc_Absyn_Exp* _temp1656; void* _temp1658; _LL1652: if(*((
int*) _temp1650) == Cyc_Absyn_Cast_e){ _LL1659: _temp1658=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1650)->f1; goto _LL1657; _LL1657: _temp1656=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1650)->f2; goto _LL1653;} else{ goto _LL1654;} _LL1654: goto _LL1655;
_LL1653:{ struct _tuple10 _temp1661=({ struct _tuple10 _temp1660; _temp1660.f1=
Cyc_Tcutil_compress( _temp1658); _temp1660.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1656->topt))->v); _temp1660;}); void*
_temp1667; struct Cyc_Absyn_PtrInfo _temp1669; struct Cyc_Absyn_Conref*
_temp1671; struct Cyc_Absyn_Tqual _temp1673; struct Cyc_Absyn_Conref* _temp1675;
void* _temp1677; void* _temp1679; void* _temp1681; struct Cyc_Absyn_PtrInfo
_temp1683; struct Cyc_Absyn_Conref* _temp1685; struct Cyc_Absyn_Tqual _temp1687;
struct Cyc_Absyn_Conref* _temp1689; void* _temp1691; void* _temp1693; _LL1663:
_LL1682: _temp1681= _temp1661.f1; if(( unsigned int) _temp1681 > 4u?*(( int*)
_temp1681) == Cyc_Absyn_PointerType: 0){ _LL1684: _temp1683=(( struct Cyc_Absyn_PointerType_struct*)
_temp1681)->f1; _LL1694: _temp1693=( void*) _temp1683.elt_typ; goto _LL1692;
_LL1692: _temp1691=( void*) _temp1683.rgn_typ; goto _LL1690; _LL1690: _temp1689=
_temp1683.nullable; goto _LL1688; _LL1688: _temp1687= _temp1683.tq; goto _LL1686;
_LL1686: _temp1685= _temp1683.bounds; goto _LL1668;} else{ goto _LL1665;}
_LL1668: _temp1667= _temp1661.f2; if(( unsigned int) _temp1667 > 4u?*(( int*)
_temp1667) == Cyc_Absyn_PointerType: 0){ _LL1670: _temp1669=(( struct Cyc_Absyn_PointerType_struct*)
_temp1667)->f1; _LL1680: _temp1679=( void*) _temp1669.elt_typ; goto _LL1678;
_LL1678: _temp1677=( void*) _temp1669.rgn_typ; goto _LL1676; _LL1676: _temp1675=
_temp1669.nullable; goto _LL1674; _LL1674: _temp1673= _temp1669.tq; goto _LL1672;
_LL1672: _temp1671= _temp1669.bounds; goto _LL1664;} else{ goto _LL1665;}
_LL1665: goto _LL1666; _LL1664:{ struct _tuple10 _temp1696=({ struct _tuple10
_temp1695; _temp1695.f1=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1685); _temp1695.f2=(( void*(*)( void*, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one, _temp1671); _temp1695;}); void*
_temp1702; struct Cyc_Absyn_Exp* _temp1704; void* _temp1706; _LL1698: _LL1707:
_temp1706= _temp1696.f1; if( _temp1706 ==( void*) Cyc_Absyn_Unknown_b){ goto
_LL1703;} else{ goto _LL1700;} _LL1703: _temp1702= _temp1696.f2; if((
unsigned int) _temp1702 > 1u?*(( int*) _temp1702) == Cyc_Absyn_Upper_b: 0){
_LL1705: _temp1704=(( struct Cyc_Absyn_Upper_b_struct*) _temp1702)->f1; goto
_LL1699;} else{ goto _LL1700;} _LL1700: goto _LL1701; _LL1699: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_List_List*(* _temp1708)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1711= mexp;
struct Cyc_Absyn_Exp* _temp1712= _temp1656; struct Cyc_Absyn_Exp* _temp1713= Cyc_Absyn_sizeoftyp_exp(
_temp1693, 0); struct Cyc_Absyn_Exp* _temp1714= _temp1704; struct Cyc_Absyn_Exp*
_temp1709[ 4u]={ _temp1711, _temp1712, _temp1713, _temp1714}; struct _tagged_arr
_temp1710={( void*) _temp1709,( void*) _temp1709,( void*)( _temp1709 + 4u)};
_temp1708( _temp1710);}), 0))->r)); goto _LL1697; _LL1701: goto _LL1697; _LL1697:;}
goto _LL1662; _LL1666: goto _LL1662; _LL1662:;} goto _LL1651; _LL1655: goto
_LL1651; _LL1651:;} if( ! done){ struct _tuple0* _temp1715= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* _temp1716= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1715, 0), 0); struct Cyc_Absyn_Exp* _temp1717= Cyc_Absyn_signed_int_exp( 0,
0); Cyc_Toc_exp_to_c( nv, _temp1174); _temp1716= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1715, 0), _temp1717, 0),
_temp1174, 0), _temp1716, 0);{ void* _temp1718= Cyc_Absyn_cstar_typ( elt_typ,
Cyc_Toc_mt_tq);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1715, _temp1718,( struct Cyc_Absyn_Exp*) mexp, _temp1716, 0), 0))->r));}}
goto _LL1554;}} _LL1554:;} goto _LL1023; _LL1061: Cyc_Toc_exp_to_c( nv,
_temp1178); goto _LL1023; _LL1063:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1719=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1719[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1720; _temp1720.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1720.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1180);
_temp1720;}); _temp1719;}))); goto _LL1023; _LL1065:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp1721=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1721[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1722; _temp1722.tag= Cyc_Absyn_Offsetof_e;
_temp1722.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1184); _temp1722.f2=
_temp1182; _temp1722;}); _temp1721;}))); goto _LL1023; _LL1067: { void*
_temp1723= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1186->topt))->v);{ void* _temp1724= _temp1723; struct Cyc_Absyn_PtrInfo
_temp1730; struct Cyc_Absyn_Conref* _temp1732; struct Cyc_Absyn_Tqual _temp1734;
struct Cyc_Absyn_Conref* _temp1736; void* _temp1738; void* _temp1740; _LL1726:
if(( unsigned int) _temp1724 > 4u?*(( int*) _temp1724) == Cyc_Absyn_PointerType:
0){ _LL1731: _temp1730=(( struct Cyc_Absyn_PointerType_struct*) _temp1724)->f1;
_LL1741: _temp1740=( void*) _temp1730.elt_typ; goto _LL1739; _LL1739: _temp1738=(
void*) _temp1730.rgn_typ; goto _LL1737; _LL1737: _temp1736= _temp1730.nullable;
goto _LL1735; _LL1735: _temp1734= _temp1730.tq; goto _LL1733; _LL1733: _temp1732=
_temp1730.bounds; goto _LL1727;} else{ goto _LL1728;} _LL1728: goto _LL1729;
_LL1727:{ void* _temp1742=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1732); struct Cyc_Absyn_Exp* _temp1748; _LL1744: if((
unsigned int) _temp1742 > 1u?*(( int*) _temp1742) == Cyc_Absyn_Upper_b: 0){
_LL1749: _temp1748=(( struct Cyc_Absyn_Upper_b_struct*) _temp1742)->f1; goto
_LL1745;} else{ goto _LL1746;} _LL1746: if( _temp1742 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1747;} else{ goto _LL1743;} _LL1745: Cyc_Toc_exp_to_c( nv, _temp1186);
if( Cyc_Toc_is_nullable( _temp1723)){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1186->topt))->v), Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1750=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1750->hd=( void*) _temp1186; _temp1750->tl= 0;
_temp1750;}), 0), 0), 0))->r));} goto _LL1743; _LL1747: { struct Cyc_Absyn_Exp*
_temp1751= Cyc_Absyn_uint_exp( 0, 0); _temp1751->topt=({ struct Cyc_Core_Opt*
_temp1752=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1752->v=( void*) Cyc_Absyn_uint_t; _temp1752;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_subscript_exp( _temp1186, _temp1751, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1743;} _LL1743:;} goto _LL1725; _LL1729:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Deref: non-pointer",
sizeof( unsigned char), 29u)); goto _LL1725; _LL1725:;} goto _LL1023;} _LL1069:
Cyc_Toc_exp_to_c( nv, _temp1190); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL1023; _LL1071: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1194->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1194);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1755; struct Cyc_Absyn_Tqual _temp1757; struct Cyc_Absyn_Conref* _temp1759;
void* _temp1761; void* _temp1763; struct Cyc_Absyn_PtrInfo _temp1753= Cyc_Toc_get_ptr_type(
e1typ); _LL1764: _temp1763=( void*) _temp1753.elt_typ; goto _LL1762; _LL1762:
_temp1761=( void*) _temp1753.rgn_typ; goto _LL1760; _LL1760: _temp1759=
_temp1753.nullable; goto _LL1758; _LL1758: _temp1757= _temp1753.tq; goto _LL1756;
_LL1756: _temp1755= _temp1753.bounds; goto _LL1754; _LL1754:{ void* _temp1765=((
void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( Cyc_Absyn_bounds_one,
_temp1755); struct Cyc_Absyn_Exp* _temp1771; _LL1767: if(( unsigned int)
_temp1765 > 1u?*(( int*) _temp1765) == Cyc_Absyn_Upper_b: 0){ _LL1772: _temp1771=((
struct Cyc_Absyn_Upper_b_struct*) _temp1765)->f1; goto _LL1768;} else{ goto
_LL1769;} _LL1769: if( _temp1765 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1770;}
else{ goto _LL1766;} _LL1768: if( Cyc_Evexp_eval_const_uint_exp( _temp1771) < 1){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof( unsigned char), 46u));}{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( 0, _temp1759); if( Cyc_Toc_is_nullable(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1194->topt))->v)){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1194->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1773=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1773->hd=( void*) _temp1194;
_temp1773->tl= 0; _temp1773;}), 0), 0), _temp1192, 0))->r));} goto _LL1766;}
_LL1770: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1763);( void*)( _temp1194->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1757), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_List_List*(* _temp1774)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1777= Cyc_Absyn_copy_exp( _temp1194); struct Cyc_Absyn_Exp*
_temp1778= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp* _temp1779=
Cyc_Absyn_uint_exp( 0, 0); struct Cyc_Absyn_Exp* _temp1775[ 3u]={ _temp1777,
_temp1778, _temp1779}; struct _tagged_arr _temp1776={( void*) _temp1775,( void*)
_temp1775,( void*)( _temp1775 + 3u)}; _temp1774( _temp1776);}), 0), 0))->r));
goto _LL1766;} _LL1766:;} if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1023;}} _LL1073: { void*
_temp1780= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1198->topt))->v); Cyc_Toc_exp_to_c( nv, _temp1198); Cyc_Toc_exp_to_c( nv,
_temp1196);{ void* _temp1781= _temp1780; struct Cyc_List_List* _temp1789; struct
Cyc_Absyn_PtrInfo _temp1791; struct Cyc_Absyn_Conref* _temp1793; struct Cyc_Absyn_Tqual
_temp1795; struct Cyc_Absyn_Conref* _temp1797; void* _temp1799; void* _temp1801;
_LL1783: if(( unsigned int) _temp1781 > 4u?*(( int*) _temp1781) == Cyc_Absyn_TupleType:
0){ _LL1790: _temp1789=(( struct Cyc_Absyn_TupleType_struct*) _temp1781)->f1;
goto _LL1784;} else{ goto _LL1785;} _LL1785: if(( unsigned int) _temp1781 > 4u?*((
int*) _temp1781) == Cyc_Absyn_PointerType: 0){ _LL1792: _temp1791=(( struct Cyc_Absyn_PointerType_struct*)
_temp1781)->f1; _LL1802: _temp1801=( void*) _temp1791.elt_typ; goto _LL1800;
_LL1800: _temp1799=( void*) _temp1791.rgn_typ; goto _LL1798; _LL1798: _temp1797=
_temp1791.nullable; goto _LL1796; _LL1796: _temp1795= _temp1791.tq; goto _LL1794;
_LL1794: _temp1793= _temp1791.bounds; goto _LL1786;} else{ goto _LL1787;}
_LL1787: goto _LL1788; _LL1784: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1196) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1198, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL1782;} _LL1786:{ void*
_temp1803=(( void*(*)( void*, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one, _temp1793); struct Cyc_Absyn_Exp* _temp1809; _LL1805: if((
unsigned int) _temp1803 > 1u?*(( int*) _temp1803) == Cyc_Absyn_Upper_b: 0){
_LL1810: _temp1809=(( struct Cyc_Absyn_Upper_b_struct*) _temp1803)->f1; goto
_LL1806;} else{ goto _LL1807;} _LL1807: if( _temp1803 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1808;} else{ goto _LL1804;} _LL1806: _temp1809= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1809), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1797); void* ta1= Cyc_Toc_typ_to_c( _temp1801);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1795); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_List_List*(* _temp1811)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1814= _temp1198; struct Cyc_Absyn_Exp* _temp1815=
_temp1809; struct Cyc_Absyn_Exp* _temp1816= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
struct Cyc_Absyn_Exp* _temp1817= _temp1196; struct Cyc_Absyn_Exp* _temp1812[ 4u]={
_temp1814, _temp1815, _temp1816, _temp1817}; struct _tagged_arr _temp1813={(
void*) _temp1812,( void*) _temp1812,( void*)( _temp1812 + 4u)}; _temp1811(
_temp1813);}), 0), 0), 0))->r));} else{( void*)( _temp1196->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_List_List*(*
_temp1818)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct Cyc_Absyn_Exp* _temp1821= _temp1809; struct Cyc_Absyn_Exp*
_temp1822= Cyc_Absyn_copy_exp( _temp1196); struct Cyc_Absyn_Exp* _temp1819[ 2u]={
_temp1821, _temp1822}; struct _tagged_arr _temp1820={( void*) _temp1819,( void*)
_temp1819,( void*)( _temp1819 + 2u)}; _temp1818( _temp1820);}), 0))->r));} goto
_LL1804;} _LL1808: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1801);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
ta1, _temp1795), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_List_List*(* _temp1823)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1826= _temp1198;
struct Cyc_Absyn_Exp* _temp1827= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp1828= _temp1196; struct Cyc_Absyn_Exp* _temp1824[ 3u]={ _temp1826,
_temp1827, _temp1828}; struct _tagged_arr _temp1825={( void*) _temp1824,( void*)
_temp1824,( void*)( _temp1824 + 3u)}; _temp1823( _temp1825);}), 0), 0), 0))->r));
goto _LL1804;} _LL1804:;} goto _LL1782; _LL1788:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u)); goto _LL1782; _LL1782:;} goto _LL1023;} _LL1075:
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 0,
0, _temp1200))->r));} else{ struct Cyc_List_List* _temp1829=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, _temp1200); struct _tagged_arr* _temp1830= Cyc_Toc_add_tuple_type(
_temp1829); struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1200 != 0;(
_temp1200=(( struct Cyc_List_List*) _check_null( _temp1200))->tl, i ++)){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1200))->hd);
dles=({ struct Cyc_List_List* _temp1831=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1831->hd=( void*)({ struct _tuple4*
_temp1832=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1832->f1=
0; _temp1832->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1200))->hd; _temp1832;}); _temp1831->tl= dles; _temp1831;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL1023; _LL1077:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1202, 0))->r));{ struct Cyc_List_List* _temp1833= _temp1202; for( 0;
_temp1833 != 0; _temp1833=(( struct Cyc_List_List*) _check_null( _temp1833))->tl){
struct _tuple4 _temp1836; struct Cyc_Absyn_Exp* _temp1837; struct Cyc_List_List*
_temp1839; struct _tuple4* _temp1834=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1833))->hd; _temp1836=* _temp1834; _LL1840: _temp1839=
_temp1836.f1; goto _LL1838; _LL1838: _temp1837= _temp1836.f2; goto _LL1835;
_LL1835: Cyc_Toc_exp_to_c( nv, _temp1837);}} goto _LL1023; _LL1079: {
unsigned int _temp1841= Cyc_Evexp_eval_const_uint_exp( _temp1206); Cyc_Toc_exp_to_c(
nv, _temp1204);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1204)){
unsigned int i= 0; for( 0; i < _temp1841; i ++){ es=({ struct Cyc_List_List*
_temp1842=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1842->hd=( void*)({ struct _tuple4* _temp1843=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1843->f1= 0; _temp1843->f2= _temp1204; _temp1843;});
_temp1842->tl= es; _temp1842;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL1023;}} _LL1081: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1212, _temp1216))->r));}
else{ if( _temp1210 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Struct_e: missing structdecl pointer", sizeof( unsigned char), 37u));}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp1210); struct _RegionHandle _temp1844= _new_region(); struct _RegionHandle*
rgn=& _temp1844; _push_region( rgn);{ struct Cyc_List_List* _temp1845=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1212,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1846= 0;{ struct Cyc_List_List*
_temp1847=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1847 != 0; _temp1847=(( struct Cyc_List_List*) _check_null(
_temp1847))->tl){ struct Cyc_List_List* _temp1848= _temp1845; for( 0; _temp1848
!= 0; _temp1848=(( struct Cyc_List_List*) _check_null( _temp1848))->tl){ if((*((
struct _tuple11*)(( struct Cyc_List_List*) _check_null( _temp1848))->hd)).f1 ==(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1847))->hd){
struct _tuple11 _temp1851; struct Cyc_Absyn_Exp* _temp1852; struct Cyc_Absyn_Structfield*
_temp1854; struct _tuple11* _temp1849=( struct _tuple11*)(( struct Cyc_List_List*)
_check_null( _temp1848))->hd; _temp1851=* _temp1849; _LL1855: _temp1854=
_temp1851.f1; goto _LL1853; _LL1853: _temp1852= _temp1851.f2; goto _LL1850;
_LL1850: { void* _temp1856=( void*) _temp1854->type; Cyc_Toc_exp_to_c( nv,
_temp1852); if( Cyc_Toc_is_void_star( _temp1856)){( void*)( _temp1852->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1852->r, 0), 0))->r));} _temp1846=({ struct Cyc_List_List* _temp1857=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1857->hd=(
void*)({ struct _tuple4* _temp1858=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1858->f1= 0; _temp1858->f2= _temp1852; _temp1858;}); _temp1857->tl=
_temp1846; _temp1857;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1846), 0))->r));}; _pop_region( rgn);}} goto _LL1023; _LL1083: { struct Cyc_List_List*
fs;{ void* _temp1859= Cyc_Tcutil_compress( _temp1220); struct Cyc_List_List*
_temp1865; _LL1861: if(( unsigned int) _temp1859 > 4u?*(( int*) _temp1859) ==
Cyc_Absyn_AnonStructType: 0){ _LL1866: _temp1865=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1859)->f1; goto _LL1862;} else{ goto _LL1863;} _LL1863: goto _LL1864;
_LL1862: fs= _temp1865; goto _LL1860; _LL1864: fs=(( struct Cyc_List_List*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr _temp1867= Cyc_Absynpp_typ2string( _temp1220); xprintf("anon struct has type %.*s",
_get_arr_size( _temp1867, 1u), _temp1867.curr);})); goto _LL1860; _LL1860:;}{
struct _RegionHandle _temp1868= _new_region(); struct _RegionHandle* rgn=&
_temp1868; _push_region( rgn);{ struct Cyc_List_List* _temp1869=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1218, fs); for( 0; _temp1869 != 0; _temp1869=(( struct Cyc_List_List*)
_check_null( _temp1869))->tl){ struct _tuple11 _temp1872; struct Cyc_Absyn_Exp*
_temp1873; struct Cyc_Absyn_Structfield* _temp1875; struct _tuple11* _temp1870=(
struct _tuple11*)(( struct Cyc_List_List*) _check_null( _temp1869))->hd;
_temp1872=* _temp1870; _LL1876: _temp1875= _temp1872.f1; goto _LL1874; _LL1874:
_temp1873= _temp1872.f2; goto _LL1871; _LL1871: { void* _temp1877=( void*)
_temp1875->type; Cyc_Toc_exp_to_c( nv, _temp1873); if( Cyc_Toc_is_void_star(
_temp1877)){( void*)( _temp1873->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1873->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1218, 0))->r));}; _pop_region( rgn);}
goto _LL1023;} _LL1085: { struct _tuple0* qv= _temp1222->name; if( _temp1224->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1878= _temp1224->fields == 0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1224->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1878))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1878))->hd)->typs == 0){ tag_count ++;}
_temp1878=(( struct Cyc_List_List*) _check_null( _temp1878))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL1023;} _LL1087: { struct _tuple0* _temp1879= _temp1232->name; struct Cyc_List_List*
_temp1880= _temp1232->typs; struct _tuple0* _temp1881= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1882= Cyc_Absyn_var_exp( _temp1881, 0); void*
_temp1883= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1879, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1234->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1879, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1884= _temp1234->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1234->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1879,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1884))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1884))->hd)->typs != 0){ tag_count ++;}
_temp1884=(( struct Cyc_List_List*) _check_null( _temp1884))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp1885= 0; for( 0;
_temp1236 != 0;( _temp1236=(( struct Cyc_List_List*) _check_null( _temp1236))->tl,
_temp1880=(( struct Cyc_List_List*) _check_null( _temp1880))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1236))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1880))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} _temp1885=({
struct Cyc_List_List* _temp1886=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1886[ 0]=({ struct Cyc_List_List _temp1887;
_temp1887.hd=( void*)({ struct _tuple4* _temp1888=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4) * 1); _temp1888[ 0]=({ struct _tuple4 _temp1889;
_temp1889.f1= 0; _temp1889.f2= cur_e; _temp1889;}); _temp1888;}); _temp1887.tl=
_temp1885; _temp1887;}); _temp1886;});} _temp1885=({ struct Cyc_List_List*
_temp1890=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1890[ 0]=({ struct Cyc_List_List _temp1891; _temp1891.hd=( void*)({ struct
_tuple4* _temp1892=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) * 1);
_temp1892[ 0]=({ struct _tuple4 _temp1893; _temp1893.f1= 0; _temp1893.f2=
tag_exp; _temp1893;}); _temp1892;}); _temp1891.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1885); _temp1891;}); _temp1890;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1894=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1894->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1879, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp1894;}), _temp1885, 0))->r));}} else{ struct
Cyc_List_List* _temp1895=({ struct Cyc_List_List* _temp1901=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1901->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1882, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1879, 0), 0); _temp1901->tl= 0; _temp1901;});{ int i= 1; for( 0; _temp1236
!= 0;((( _temp1236=(( struct Cyc_List_List*) _check_null( _temp1236))->tl, i ++)),
_temp1880=(( struct Cyc_List_List*) _check_null( _temp1880))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1236))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1880))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp1896= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1882, Cyc_Toc_fieldname(
i), 0), cur_e, 0); _temp1895=({ struct Cyc_List_List* _temp1897=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1897->hd=( void*) _temp1896;
_temp1897->tl= _temp1895; _temp1897;});}}}{ struct Cyc_Absyn_Stmt* _temp1898=
Cyc_Absyn_exp_stmt( _temp1882, 0); struct Cyc_Absyn_Stmt* _temp1899= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1900=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1900->hd=( void*) _temp1898; _temp1900->tl= _temp1895; _temp1900;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1881, _temp1883, 0, _temp1899, 0), 0))->r));}} goto _LL1023;} _LL1089: goto
_LL1023; _LL1091: { void* t_c= Cyc_Toc_typ_to_c( _temp1248); if( _temp1250 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1250); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1248, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL1023;} _LL1093: Cyc_Toc_stmt_to_c(
nv, _temp1252); goto _LL1023; _LL1095:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem", sizeof( unsigned char), 14u)); goto _LL1023; _LL1097:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u)); goto _LL1023; _LL1099:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u)); goto _LL1023;
_LL1101:(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("fill",
sizeof( unsigned char), 5u)); goto _LL1023; _LL1023:;}} static struct Cyc_Absyn_Stmt*
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
_temp1902=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp1938; struct Cyc_Absyn_Vardecl
_temp1940; struct Cyc_List_List* _temp1941; struct Cyc_Core_Opt* _temp1943;
struct Cyc_Absyn_Exp* _temp1945; void* _temp1947; struct Cyc_Absyn_Tqual
_temp1949; struct _tuple0* _temp1951; void* _temp1953; struct Cyc_Absyn_Vardecl*
_temp1955; int _temp1957; void* _temp1959; unsigned char _temp1961; struct
_tagged_arr _temp1963; struct Cyc_Absyn_Enumfield* _temp1965; struct Cyc_Absyn_Enumdecl*
_temp1967; struct Cyc_List_List* _temp1969; struct Cyc_List_List* _temp1971;
struct Cyc_Absyn_Tunionfield* _temp1973; struct Cyc_Absyn_Tuniondecl* _temp1975;
struct Cyc_Absyn_Pat* _temp1978; struct Cyc_Absyn_Pat _temp1980; struct Cyc_Position_Segment*
_temp1981; struct Cyc_Core_Opt* _temp1983; void* _temp1985; struct Cyc_List_List*
_temp1987; struct Cyc_List_List* _temp1989; struct Cyc_Absyn_Tunionfield*
_temp1991; struct Cyc_Absyn_Tuniondecl* _temp1993; struct Cyc_List_List*
_temp1995; struct Cyc_List_List* _temp1997; struct Cyc_Absyn_Tunionfield*
_temp1999; struct Cyc_Absyn_Tuniondecl* _temp2001; struct Cyc_List_List*
_temp2003; struct Cyc_List_List* _temp2005; struct Cyc_List_List* _temp2007;
struct Cyc_Core_Opt* _temp2009; struct Cyc_Absyn_Structdecl* _temp2011; struct
Cyc_Absyn_Pat* _temp2013; struct _tuple0* _temp2015; struct Cyc_List_List*
_temp2017; struct Cyc_List_List* _temp2019; struct _tuple0* _temp2021; struct
Cyc_List_List* _temp2023; struct Cyc_List_List* _temp2025; struct _tuple0*
_temp2027; _LL1904: if(( unsigned int) _temp1902 > 2u?*(( int*) _temp1902) ==
Cyc_Absyn_Var_p: 0){ _LL1939: _temp1938=(( struct Cyc_Absyn_Var_p_struct*)
_temp1902)->f1; _temp1940=* _temp1938; _LL1954: _temp1953=( void*) _temp1940.sc;
goto _LL1952; _LL1952: _temp1951= _temp1940.name; goto _LL1950; _LL1950:
_temp1949= _temp1940.tq; goto _LL1948; _LL1948: _temp1947=( void*) _temp1940.type;
goto _LL1946; _LL1946: _temp1945= _temp1940.initializer; goto _LL1944; _LL1944:
_temp1943= _temp1940.rgn; goto _LL1942; _LL1942: _temp1941= _temp1940.attributes;
goto _LL1905;} else{ goto _LL1906;} _LL1906: if( _temp1902 ==( void*) Cyc_Absyn_Wild_p){
goto _LL1907;} else{ goto _LL1908;} _LL1908: if(( unsigned int) _temp1902 > 2u?*((
int*) _temp1902) == Cyc_Absyn_Reference_p: 0){ _LL1956: _temp1955=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1902)->f1; goto _LL1909;} else{ goto _LL1910;} _LL1910: if( _temp1902 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1911;} else{ goto _LL1912;} _LL1912: if((
unsigned int) _temp1902 > 2u?*(( int*) _temp1902) == Cyc_Absyn_Int_p: 0){
_LL1960: _temp1959=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1902)->f1;
goto _LL1958; _LL1958: _temp1957=(( struct Cyc_Absyn_Int_p_struct*) _temp1902)->f2;
goto _LL1913;} else{ goto _LL1914;} _LL1914: if(( unsigned int) _temp1902 > 2u?*((
int*) _temp1902) == Cyc_Absyn_Char_p: 0){ _LL1962: _temp1961=(( struct Cyc_Absyn_Char_p_struct*)
_temp1902)->f1; goto _LL1915;} else{ goto _LL1916;} _LL1916: if(( unsigned int)
_temp1902 > 2u?*(( int*) _temp1902) == Cyc_Absyn_Float_p: 0){ _LL1964: _temp1963=((
struct Cyc_Absyn_Float_p_struct*) _temp1902)->f1; goto _LL1917;} else{ goto
_LL1918;} _LL1918: if(( unsigned int) _temp1902 > 2u?*(( int*) _temp1902) == Cyc_Absyn_Enum_p:
0){ _LL1968: _temp1967=(( struct Cyc_Absyn_Enum_p_struct*) _temp1902)->f1; goto
_LL1966; _LL1966: _temp1965=(( struct Cyc_Absyn_Enum_p_struct*) _temp1902)->f2;
goto _LL1919;} else{ goto _LL1920;} _LL1920: if(( unsigned int) _temp1902 > 2u?*((
int*) _temp1902) == Cyc_Absyn_Tunion_p: 0){ _LL1976: _temp1975=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1902)->f1; goto _LL1974; _LL1974: _temp1973=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1902)->f2; goto _LL1972; _LL1972: _temp1971=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1902)->f3; goto _LL1970; _LL1970: _temp1969=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1902)->f4; if( _temp1969 == 0){ goto _LL1921;} else{ goto _LL1922;}} else{
goto _LL1922;} _LL1922: if(( unsigned int) _temp1902 > 2u?*(( int*) _temp1902)
== Cyc_Absyn_Pointer_p: 0){ _LL1979: _temp1978=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1902)->f1; _temp1980=* _temp1978; _LL1986: _temp1985=( void*) _temp1980.r;
if(( unsigned int) _temp1985 > 2u?*(( int*) _temp1985) == Cyc_Absyn_Tunion_p: 0){
_LL1994: _temp1993=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1985)->f1; goto
_LL1992; _LL1992: _temp1991=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1985)->f2;
goto _LL1990; _LL1990: _temp1989=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1985)->f3;
goto _LL1988; _LL1988: _temp1987=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1985)->f4;
goto _LL1984;} else{ goto _LL1924;} _LL1984: _temp1983= _temp1980.topt; goto
_LL1982; _LL1982: _temp1981= _temp1980.loc; goto _LL1977;} else{ goto _LL1924;}
_LL1977: if( _temp1987 != 0){ goto _LL1923;} else{ goto _LL1924;} _LL1924: if((
unsigned int) _temp1902 > 2u?*(( int*) _temp1902) == Cyc_Absyn_Tunion_p: 0){
_LL2002: _temp2001=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1902)->f1; goto
_LL2000; _LL2000: _temp1999=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1902)->f2;
goto _LL1998; _LL1998: _temp1997=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1902)->f3;
goto _LL1996; _LL1996: _temp1995=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1902)->f4;
goto _LL1925;} else{ goto _LL1926;} _LL1926: if(( unsigned int) _temp1902 > 2u?*((
int*) _temp1902) == Cyc_Absyn_Tuple_p: 0){ _LL2004: _temp2003=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1902)->f1; goto _LL1927;} else{ goto _LL1928;} _LL1928: if(( unsigned int)
_temp1902 > 2u?*(( int*) _temp1902) == Cyc_Absyn_Struct_p: 0){ _LL2012:
_temp2011=(( struct Cyc_Absyn_Struct_p_struct*) _temp1902)->f1; goto _LL2010;
_LL2010: _temp2009=(( struct Cyc_Absyn_Struct_p_struct*) _temp1902)->f2; goto
_LL2008; _LL2008: _temp2007=(( struct Cyc_Absyn_Struct_p_struct*) _temp1902)->f3;
goto _LL2006; _LL2006: _temp2005=(( struct Cyc_Absyn_Struct_p_struct*) _temp1902)->f4;
goto _LL1929;} else{ goto _LL1930;} _LL1930: if(( unsigned int) _temp1902 > 2u?*((
int*) _temp1902) == Cyc_Absyn_Pointer_p: 0){ _LL2014: _temp2013=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1902)->f1; goto _LL1931;} else{ goto _LL1932;} _LL1932: if(( unsigned int)
_temp1902 > 2u?*(( int*) _temp1902) == Cyc_Absyn_UnknownId_p: 0){ _LL2016:
_temp2015=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1902)->f1; goto _LL1933;}
else{ goto _LL1934;} _LL1934: if(( unsigned int) _temp1902 > 2u?*(( int*)
_temp1902) == Cyc_Absyn_UnknownCall_p: 0){ _LL2022: _temp2021=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1902)->f1; goto _LL2020; _LL2020: _temp2019=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1902)->f2; goto _LL2018; _LL2018: _temp2017=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1902)->f3; goto _LL1935;} else{ goto _LL1936;} _LL1936: if(( unsigned int)
_temp1902 > 2u?*(( int*) _temp1902) == Cyc_Absyn_UnknownFields_p: 0){ _LL2028:
_temp2027=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1902)->f1; goto
_LL2026; _LL2026: _temp2025=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1902)->f2; goto _LL2024; _LL2024: _temp2023=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1902)->f3; goto _LL1937;} else{ goto _LL1903;} _LL1905: nv= Cyc_Toc_add_varmap(
nv, _temp1951, r); goto _LL1907; _LL1907: s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL1903; _LL1909: { struct _tuple0* _temp2029= Cyc_Toc_temp_var(); decls=({
struct Cyc_List_List* _temp2030=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2030->hd=( void*)({ struct _tuple13*
_temp2031=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2031->f1= _temp2029; _temp2031->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp2031;}); _temp2030->tl= decls; _temp2030;}); nv= Cyc_Toc_add_varmap(
nv, _temp1955->name, Cyc_Absyn_var_exp( _temp2029, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2029, 0), Cyc_Absyn_address_exp(
path, 0), 0), Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1903;} _LL1911: s=
Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab);
goto _LL1903; _LL1913: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp(
_temp1959, _temp1957, 0), succ_lab, fail_lab); goto _LL1903; _LL1915: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_char_exp( _temp1961, 0), succ_lab, fail_lab); goto _LL1903; _LL1917:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp( _temp1963, 0), succ_lab,
fail_lab); goto _LL1903; _LL1919: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2032=( struct Cyc_Absyn_Enum_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct)); _temp2032[ 0]=({ struct Cyc_Absyn_Enum_e_struct
_temp2033; _temp2033.tag= Cyc_Absyn_Enum_e; _temp2033.f1= _temp1965->name;
_temp2033.f2=( struct Cyc_Absyn_Enumdecl*) _temp1967; _temp2033.f3=( struct Cyc_Absyn_Enumfield*)
_temp1965; _temp2033;}); _temp2032;}), 0), succ_lab, fail_lab); goto _LL1903;
_LL1921: { struct Cyc_Absyn_Exp* cmp_exp= Cyc_Absyn_var_exp( _temp1973->name, 0);
if( ! _temp1975->is_xtunion){ cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt( r, cmp_exp, succ_lab, fail_lab); goto
_LL1903;} _LL1923: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1987); struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag(
_temp1991->name, _tag_arr("_struct", sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq),
r, 0); struct Cyc_List_List* _temp2034=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1991->typs);
struct Cyc_List_List* _temp2035=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1987); for(
0; _temp2035 != 0;((( _temp2035=(( struct Cyc_List_List*) _check_null( _temp2035))->tl,
_temp2034=(( struct Cyc_List_List*) _check_null( _temp2034))->tl)), cnt --)){
struct Cyc_Absyn_Pat* _temp2036=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2035))->hd; void* _temp2037=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2034))->hd)).f2; struct _tuple0* _temp2038= Cyc_Toc_temp_var();
void* _temp2039=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2036->topt))->v;
void* _temp2040= Cyc_Toc_typ_to_c( _temp2039); struct _tagged_arr* _temp2041=
Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp* _temp2042= Cyc_Absyn_structarrow_exp(
rcast, Cyc_Toc_fieldname( cnt), 0); if( Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c(
_temp2037))){ _temp2042= Cyc_Absyn_cast_exp( _temp2040, _temp2042, 0);} decls=({
struct Cyc_List_List* _temp2043=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2043->hd=( void*)({ struct _tuple13*
_temp2044=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2044->f1= _temp2038; _temp2044->f2= _temp2040; _temp2044;}); _temp2043->tl=
decls; _temp2043;});{ struct _tuple12 _temp2045= Cyc_Toc_xlate_pat( nv, rgn,
_temp2039, Cyc_Absyn_var_exp( _temp2038, 0), _temp2042, _temp2036, succ_lab,
fail_lab, decls); nv= _temp2045.f1; decls= _temp2045.f2;{ struct Cyc_Absyn_Stmt*
_temp2046= _temp2045.f3; struct Cyc_Absyn_Stmt* _temp2047= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp2038, 0), _temp2042, 0); struct Cyc_Absyn_Stmt*
_temp2048= Cyc_Absyn_seq_stmt( _temp2047, _temp2046, 0); ss=({ struct Cyc_List_List*
_temp2049=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2049->hd=( void*) Cyc_Absyn_label_stmt( _temp2041, _temp2048, 0); _temp2049->tl=
ss; _temp2049;}); succ_lab= _temp2041;}}}{ struct Cyc_Absyn_Exp* test_exp; if(
_temp1993->is_xtunion){ struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp(
e2, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1991->name, 0);
test_exp= Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e, 0), Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);}
else{ unsigned int max_tag= 0;{ struct Cyc_List_List* _temp2050=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1993->fields))->v; for( 0; _temp2050 !=
0; _temp2050=(( struct Cyc_List_List*) _check_null( _temp2050))->tl){ struct Cyc_Absyn_Tunionfield*
_temp2051=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp2050))->hd; if( _temp2051->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1991->name, 0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1903;}} _LL1925: _temp2003= _temp1995; goto _LL1927; _LL1927: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2003);{ struct Cyc_List_List* _temp2052=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2003); for(
0; _temp2052 != 0;( _temp2052=(( struct Cyc_List_List*) _check_null( _temp2052))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp2053=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp2052))->hd; struct _tuple0* _temp2054= Cyc_Toc_temp_var();
void* _temp2055=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2053->topt))->v;
struct _tagged_arr* _temp2056= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2057=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2057->hd=( void*)({ struct _tuple13* _temp2058=( struct _tuple13*)
_region_malloc( rgn, sizeof( struct _tuple13)); _temp2058->f1= _temp2054;
_temp2058->f2= Cyc_Toc_typ_to_c( _temp2055); _temp2058;}); _temp2057->tl= decls;
_temp2057;});{ struct _tuple12 _temp2059= Cyc_Toc_xlate_pat( nv, rgn, _temp2055,
Cyc_Absyn_var_exp( _temp2054, 0), Cyc_Absyn_structmember_exp( path, Cyc_Toc_fieldname(
cnt), 0), _temp2053, succ_lab, fail_lab, decls); nv= _temp2059.f1; decls=
_temp2059.f2;{ struct Cyc_Absyn_Stmt* _temp2060= _temp2059.f3; struct Cyc_Absyn_Stmt*
_temp2061= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp2054, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp2060, 0); ss=({ struct Cyc_List_List* _temp2062=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2062->hd=( void*) Cyc_Absyn_label_stmt(
_temp2056, _temp2061, 0); _temp2062->tl= ss; _temp2062;}); succ_lab= _temp2056;}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL1903;} _LL1929: { struct Cyc_List_List*
ss= 0;{ struct Cyc_List_List* _temp2063=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp2005); for(
0; _temp2063 != 0; _temp2063=(( struct Cyc_List_List*) _check_null( _temp2063))->tl){
struct _tuple14* _temp2064=( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp2063))->hd; struct Cyc_Absyn_Pat* _temp2065=(* _temp2064).f2;
struct _tagged_arr* f;{ void* _temp2066=( void*)(( struct Cyc_List_List*)
_check_null((* _temp2064).f1))->hd; struct _tagged_arr* _temp2072; _LL2068: if(*((
int*) _temp2066) == Cyc_Absyn_FieldName){ _LL2073: _temp2072=(( struct Cyc_Absyn_FieldName_struct*)
_temp2066)->f1; goto _LL2069;} else{ goto _LL2070;} _LL2070: goto _LL2071;
_LL2069: f= _temp2072; goto _LL2067; _LL2071:( int) _throw(( void*) Cyc_Toc_Match_error);
_LL2067:;}{ struct _tuple0* _temp2074= Cyc_Toc_temp_var(); void* _temp2075=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp2065->topt))->v; void*
_temp2076= Cyc_Toc_typ_to_c( _temp2075); struct _tagged_arr* _temp2077= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp2078=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2078->hd=( void*)({ struct _tuple13*
_temp2079=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp2079->f1= _temp2074; _temp2079->f2= _temp2076; _temp2079;}); _temp2078->tl=
decls; _temp2078;});{ struct _tuple12 _temp2080= Cyc_Toc_xlate_pat( nv, rgn,
_temp2075, Cyc_Absyn_var_exp( _temp2074, 0), Cyc_Absyn_structmember_exp( path, f,
0), _temp2065, succ_lab, fail_lab, decls); nv= _temp2080.f1; decls= _temp2080.f2;{
struct Cyc_Absyn_Exp* _temp2081= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2011->fields))->v,
f)))->type)){ _temp2081= Cyc_Absyn_cast_exp( _temp2076, _temp2081, 0);}{ struct
Cyc_Absyn_Stmt* _temp2082= _temp2080.f3; struct Cyc_Absyn_Stmt* _temp2083= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2074, 0), _temp2081, 0),
_temp2082, 0); ss=({ struct Cyc_List_List* _temp2084=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2084->hd=( void*) Cyc_Absyn_label_stmt(
_temp2077, _temp2083, 0); _temp2084->tl= ss; _temp2084;}); succ_lab= _temp2077;}}}}}}
s= Cyc_Absyn_seq_stmts( ss, 0); goto _LL1903;} _LL1931: { struct _tuple0*
_temp2085= Cyc_Toc_temp_var(); void* _temp2086=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2013->topt))->v; decls=({ struct Cyc_List_List* _temp2087=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2087->hd=( void*)({ struct _tuple13* _temp2088=( struct _tuple13*)
_region_malloc( rgn, sizeof( struct _tuple13)); _temp2088->f1= _temp2085;
_temp2088->f2= Cyc_Toc_typ_to_c( _temp2086); _temp2088;}); _temp2087->tl= decls;
_temp2087;});{ struct _tuple12 _temp2089= Cyc_Toc_xlate_pat( nv, rgn, _temp2086,
Cyc_Absyn_var_exp( _temp2085, 0), Cyc_Absyn_deref_exp( path, 0), _temp2013,
succ_lab, fail_lab, decls); nv= _temp2089.f1; decls= _temp2089.f2;{ struct Cyc_Absyn_Stmt*
_temp2090= _temp2089.f3; struct Cyc_Absyn_Stmt* _temp2091= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp2085, 0), Cyc_Absyn_deref_exp( r,
0), 0), _temp2090, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp2091, 0);} else{ s= _temp2091;} goto _LL1903;}}} _LL1933: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid",
sizeof( unsigned char), 10u)); goto _LL1903; _LL1935: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall", sizeof(
unsigned char), 12u)); goto _LL1903; _LL1937: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields", sizeof(
unsigned char), 14u)); goto _LL1903; _LL1903:;} return({ struct _tuple12
_temp2092; _temp2092.f1= nv; _temp2092.f2= decls; _temp2092.f3= s; _temp2092;});}
struct _tuple15{ struct _tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple15* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple15* _temp2093=(
struct _tuple15*) _region_malloc( r, sizeof( struct _tuple15)); _temp2093->f1=
Cyc_Toc_fresh_label(); _temp2093->f2= Cyc_Toc_fresh_label(); _temp2093->f3= sc;
_temp2093;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp2094=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; int leave_as_switch;{ void* _temp2095= Cyc_Tcutil_compress(
_temp2094); void* _temp2101; void* _temp2103; _LL2097: if(( unsigned int)
_temp2095 > 4u?*(( int*) _temp2095) == Cyc_Absyn_IntType: 0){ _LL2104: _temp2103=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2095)->f1; goto _LL2102; _LL2102:
_temp2101=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2095)->f2; goto
_LL2098;} else{ goto _LL2099;} _LL2099: goto _LL2100; _LL2098: leave_as_switch=
1; goto _LL2096; _LL2100: leave_as_switch= 0; goto _LL2096; _LL2096:;}{ struct
Cyc_List_List* _temp2105= scs; for( 0; _temp2105 != 0; _temp2105=(( struct Cyc_List_List*)
_check_null( _temp2105))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2105))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp2105))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp2106= scs; for( 0; _temp2106
!= 0; _temp2106=(( struct Cyc_List_List*) _check_null( _temp2106))->tl){ struct
Cyc_Absyn_Stmt* _temp2107=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp2106))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp2106))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp2107, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp2107);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp2108= _new_region(); struct _RegionHandle* rgn=&
_temp2108; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple15*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp2109= lscs; for( 0;
_temp2109 != 0; _temp2109=(( struct Cyc_List_List*) _check_null( _temp2109))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp2109))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp2109))->tl == 0? end_l:(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp2109))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp2109))->hd)).f2; if( sc->where_clause == 0){ struct _tuple12
_temp2110= Cyc_Toc_xlate_pat( nv, rgn, _temp2094, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp2111=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2111->hd=( void*)
_temp2110.f1; _temp2111->tl= nvs; _temp2111;}); decls= _temp2110.f2; test_stmts=({
struct Cyc_List_List* _temp2112=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2112->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2109))->hd)).f1,
_temp2110.f3, 0); _temp2112->tl= test_stmts; _temp2112;});} else{ struct Cyc_Absyn_Exp*
_temp2113=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp2114= Cyc_Toc_fresh_label(); struct _tuple12 _temp2115= Cyc_Toc_xlate_pat(
nv, rgn, _temp2094, r, path, sc->pattern, _temp2114, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp2115.f1, _temp2113); nvs=({ struct Cyc_List_List* _temp2116=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2116->hd=( void*)
_temp2115.f1; _temp2116->tl= nvs; _temp2116;}); decls= _temp2115.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp2113, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp2114, s2,
0); test_stmts=({ struct Cyc_List_List* _temp2117=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2117->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2109))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp2115.f3, s3, 0), 0); _temp2117->tl= test_stmts;
_temp2117;});}}}}{ struct Cyc_Absyn_Stmt* _temp2118= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl != 0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2119=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp2119, end_l,(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp2120=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp2120, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp2118, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls != 0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple13 _temp2123; void* _temp2124; struct
_tuple0* _temp2126; struct _tuple13* _temp2121=( struct _tuple13*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp2123=* _temp2121; _LL2127: _temp2126= _temp2123.f1;
goto _LL2125; _LL2125: _temp2124= _temp2123.f2; goto _LL2122; _LL2122: res= Cyc_Absyn_declare_stmt(
_temp2126, _temp2124, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp2128=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2128[ 0]=({ struct Cyc_List_List
_temp2129; _temp2129.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp2129.tl= 0; _temp2129;}); _temp2128;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2130=( void*) s->r; struct Cyc_Absyn_Exp*
_temp2172; struct Cyc_Absyn_Stmt* _temp2174; struct Cyc_Absyn_Stmt* _temp2176;
struct Cyc_Absyn_Exp* _temp2178; struct Cyc_Absyn_Stmt* _temp2180; struct Cyc_Absyn_Stmt*
_temp2182; struct Cyc_Absyn_Exp* _temp2184; struct Cyc_Absyn_Stmt* _temp2186;
struct _tuple2 _temp2188; struct Cyc_Absyn_Stmt* _temp2190; struct Cyc_Absyn_Exp*
_temp2192; struct Cyc_Absyn_Stmt* _temp2194; struct Cyc_Absyn_Stmt* _temp2196;
struct Cyc_Absyn_Stmt* _temp2198; struct _tagged_arr* _temp2200; struct Cyc_Absyn_Stmt*
_temp2202; struct _tuple2 _temp2204; struct Cyc_Absyn_Stmt* _temp2206; struct
Cyc_Absyn_Exp* _temp2208; struct _tuple2 _temp2210; struct Cyc_Absyn_Stmt*
_temp2212; struct Cyc_Absyn_Exp* _temp2214; struct Cyc_Absyn_Exp* _temp2216;
struct Cyc_List_List* _temp2218; struct Cyc_Absyn_Exp* _temp2220; struct Cyc_Absyn_Switch_clause**
_temp2222; struct Cyc_List_List* _temp2224; struct Cyc_Absyn_Stmt* _temp2226;
struct Cyc_Absyn_Decl* _temp2228; struct Cyc_Absyn_Stmt* _temp2230; struct
_tagged_arr* _temp2232; struct _tuple2 _temp2234; struct Cyc_Absyn_Stmt*
_temp2236; struct Cyc_Absyn_Exp* _temp2238; struct Cyc_Absyn_Stmt* _temp2240;
struct Cyc_List_List* _temp2242; struct Cyc_Absyn_Stmt* _temp2244; struct Cyc_Absyn_Stmt*
_temp2246; struct Cyc_Absyn_Vardecl* _temp2248; struct Cyc_Absyn_Tvar* _temp2250;
struct Cyc_List_List* _temp2252; struct Cyc_Absyn_Exp* _temp2254; struct Cyc_Absyn_Stmt*
_temp2256; struct Cyc_Absyn_Stmt* _temp2258; _LL2132: if( _temp2130 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2133;} else{ goto _LL2134;} _LL2134: if((
unsigned int) _temp2130 > 1u?*(( int*) _temp2130) == Cyc_Absyn_Exp_s: 0){
_LL2173: _temp2172=(( struct Cyc_Absyn_Exp_s_struct*) _temp2130)->f1; goto
_LL2135;} else{ goto _LL2136;} _LL2136: if(( unsigned int) _temp2130 > 1u?*((
int*) _temp2130) == Cyc_Absyn_Seq_s: 0){ _LL2177: _temp2176=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2130)->f1; goto _LL2175; _LL2175: _temp2174=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2130)->f2; goto _LL2137;} else{ goto _LL2138;} _LL2138: if(( unsigned int)
_temp2130 > 1u?*(( int*) _temp2130) == Cyc_Absyn_Return_s: 0){ _LL2179:
_temp2178=(( struct Cyc_Absyn_Return_s_struct*) _temp2130)->f1; goto _LL2139;}
else{ goto _LL2140;} _LL2140: if(( unsigned int) _temp2130 > 1u?*(( int*)
_temp2130) == Cyc_Absyn_IfThenElse_s: 0){ _LL2185: _temp2184=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2130)->f1; goto _LL2183; _LL2183: _temp2182=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2130)->f2; goto _LL2181; _LL2181: _temp2180=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2130)->f3; goto _LL2141;} else{ goto _LL2142;} _LL2142: if(( unsigned int)
_temp2130 > 1u?*(( int*) _temp2130) == Cyc_Absyn_While_s: 0){ _LL2189: _temp2188=((
struct Cyc_Absyn_While_s_struct*) _temp2130)->f1; _LL2193: _temp2192= _temp2188.f1;
goto _LL2191; _LL2191: _temp2190= _temp2188.f2; goto _LL2187; _LL2187: _temp2186=((
struct Cyc_Absyn_While_s_struct*) _temp2130)->f2; goto _LL2143;} else{ goto
_LL2144;} _LL2144: if(( unsigned int) _temp2130 > 1u?*(( int*) _temp2130) == Cyc_Absyn_Break_s:
0){ _LL2195: _temp2194=(( struct Cyc_Absyn_Break_s_struct*) _temp2130)->f1; goto
_LL2145;} else{ goto _LL2146;} _LL2146: if(( unsigned int) _temp2130 > 1u?*((
int*) _temp2130) == Cyc_Absyn_Continue_s: 0){ _LL2197: _temp2196=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2130)->f1; goto _LL2147;} else{ goto _LL2148;} _LL2148: if(( unsigned int)
_temp2130 > 1u?*(( int*) _temp2130) == Cyc_Absyn_Goto_s: 0){ _LL2201: _temp2200=((
struct Cyc_Absyn_Goto_s_struct*) _temp2130)->f1; goto _LL2199; _LL2199:
_temp2198=(( struct Cyc_Absyn_Goto_s_struct*) _temp2130)->f2; goto _LL2149;}
else{ goto _LL2150;} _LL2150: if(( unsigned int) _temp2130 > 1u?*(( int*)
_temp2130) == Cyc_Absyn_For_s: 0){ _LL2217: _temp2216=(( struct Cyc_Absyn_For_s_struct*)
_temp2130)->f1; goto _LL2211; _LL2211: _temp2210=(( struct Cyc_Absyn_For_s_struct*)
_temp2130)->f2; _LL2215: _temp2214= _temp2210.f1; goto _LL2213; _LL2213:
_temp2212= _temp2210.f2; goto _LL2205; _LL2205: _temp2204=(( struct Cyc_Absyn_For_s_struct*)
_temp2130)->f3; _LL2209: _temp2208= _temp2204.f1; goto _LL2207; _LL2207:
_temp2206= _temp2204.f2; goto _LL2203; _LL2203: _temp2202=(( struct Cyc_Absyn_For_s_struct*)
_temp2130)->f4; goto _LL2151;} else{ goto _LL2152;} _LL2152: if(( unsigned int)
_temp2130 > 1u?*(( int*) _temp2130) == Cyc_Absyn_Switch_s: 0){ _LL2221:
_temp2220=(( struct Cyc_Absyn_Switch_s_struct*) _temp2130)->f1; goto _LL2219;
_LL2219: _temp2218=(( struct Cyc_Absyn_Switch_s_struct*) _temp2130)->f2; goto
_LL2153;} else{ goto _LL2154;} _LL2154: if(( unsigned int) _temp2130 > 1u?*((
int*) _temp2130) == Cyc_Absyn_Fallthru_s: 0){ _LL2225: _temp2224=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2130)->f1; goto _LL2223; _LL2223: _temp2222=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2130)->f2; goto _LL2155;} else{ goto _LL2156;} _LL2156: if(( unsigned int)
_temp2130 > 1u?*(( int*) _temp2130) == Cyc_Absyn_Decl_s: 0){ _LL2229: _temp2228=((
struct Cyc_Absyn_Decl_s_struct*) _temp2130)->f1; goto _LL2227; _LL2227:
_temp2226=(( struct Cyc_Absyn_Decl_s_struct*) _temp2130)->f2; goto _LL2157;}
else{ goto _LL2158;} _LL2158: if(( unsigned int) _temp2130 > 1u?*(( int*)
_temp2130) == Cyc_Absyn_Label_s: 0){ _LL2233: _temp2232=(( struct Cyc_Absyn_Label_s_struct*)
_temp2130)->f1; goto _LL2231; _LL2231: _temp2230=(( struct Cyc_Absyn_Label_s_struct*)
_temp2130)->f2; goto _LL2159;} else{ goto _LL2160;} _LL2160: if(( unsigned int)
_temp2130 > 1u?*(( int*) _temp2130) == Cyc_Absyn_Do_s: 0){ _LL2241: _temp2240=((
struct Cyc_Absyn_Do_s_struct*) _temp2130)->f1; goto _LL2235; _LL2235: _temp2234=((
struct Cyc_Absyn_Do_s_struct*) _temp2130)->f2; _LL2239: _temp2238= _temp2234.f1;
goto _LL2237; _LL2237: _temp2236= _temp2234.f2; goto _LL2161;} else{ goto
_LL2162;} _LL2162: if(( unsigned int) _temp2130 > 1u?*(( int*) _temp2130) == Cyc_Absyn_TryCatch_s:
0){ _LL2245: _temp2244=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2130)->f1;
goto _LL2243; _LL2243: _temp2242=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2130)->f2; goto _LL2163;} else{ goto _LL2164;} _LL2164: if(( unsigned int)
_temp2130 > 1u?*(( int*) _temp2130) == Cyc_Absyn_Region_s: 0){ _LL2251:
_temp2250=(( struct Cyc_Absyn_Region_s_struct*) _temp2130)->f1; goto _LL2249;
_LL2249: _temp2248=(( struct Cyc_Absyn_Region_s_struct*) _temp2130)->f2; goto
_LL2247; _LL2247: _temp2246=(( struct Cyc_Absyn_Region_s_struct*) _temp2130)->f3;
goto _LL2165;} else{ goto _LL2166;} _LL2166: if(( unsigned int) _temp2130 > 1u?*((
int*) _temp2130) == Cyc_Absyn_SwitchC_s: 0){ _LL2255: _temp2254=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2130)->f1; goto _LL2253; _LL2253: _temp2252=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2130)->f2; goto _LL2167;} else{ goto _LL2168;} _LL2168: if(( unsigned int)
_temp2130 > 1u?*(( int*) _temp2130) == Cyc_Absyn_Cut_s: 0){ _LL2257: _temp2256=((
struct Cyc_Absyn_Cut_s_struct*) _temp2130)->f1; goto _LL2169;} else{ goto
_LL2170;} _LL2170: if(( unsigned int) _temp2130 > 1u?*(( int*) _temp2130) == Cyc_Absyn_Splice_s:
0){ _LL2259: _temp2258=(( struct Cyc_Absyn_Splice_s_struct*) _temp2130)->f1;
goto _LL2171;} else{ goto _LL2131;} _LL2133: return; _LL2135: Cyc_Toc_exp_to_c(
nv, _temp2172); return; _LL2137: Cyc_Toc_stmt_to_c( nv, _temp2176); s= _temp2174;
continue; _LL2139: { struct Cyc_Core_Opt* topt= 0; if( _temp2178 != 0){ topt=({
struct Cyc_Core_Opt* _temp2260=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2260->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2178))->topt))->v);
_temp2260;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2178));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2261=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp2262= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2261, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp2261,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp2178, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp2262, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL2141: Cyc_Toc_exp_to_c( nv, _temp2184); Cyc_Toc_stmt_to_c( nv,
_temp2182); s= _temp2180; continue; _LL2143: Cyc_Toc_exp_to_c( nv, _temp2192);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2186); return; _LL2145: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{ int
dest_depth= _temp2194 == 0? 0:(( struct Cyc_Absyn_Stmt*) _check_null( _temp2194))->try_depth;
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL2147: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp2198=
_temp2196; goto _LL2149; _LL2149: Cyc_Toc_do_npop_before( s->try_depth -((
struct Cyc_Absyn_Stmt*) _check_null( _temp2198))->try_depth, s); return; _LL2151:
Cyc_Toc_exp_to_c( nv, _temp2216); Cyc_Toc_exp_to_c( nv, _temp2214); Cyc_Toc_exp_to_c(
nv, _temp2208); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2202); return;
_LL2153: Cyc_Toc_xlate_switch( nv, s, _temp2220, _temp2218); return; _LL2155:
if( nv->fallthru_info == 0){( int) _throw((( void*(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("fallthru in unexpected place", sizeof(
unsigned char), 29u)));}{ struct _tuple6 _temp2265; struct Cyc_Dict_Dict*
_temp2266; struct Cyc_List_List* _temp2268; struct _tagged_arr* _temp2270;
struct _tuple6* _temp2263=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp2265=* _temp2263; _LL2271: _temp2270= _temp2265.f1;
goto _LL2269; _LL2269: _temp2268= _temp2265.f2; goto _LL2267; _LL2267: _temp2266=
_temp2265.f3; goto _LL2264; _LL2264: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp2270, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp2222)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2272=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2268); struct Cyc_List_List* _temp2273=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp2224); for( 0; _temp2272 != 0;( _temp2272=((
struct Cyc_List_List*) _check_null( _temp2272))->tl, _temp2273=(( struct Cyc_List_List*)
_check_null( _temp2273))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp2273))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp2266,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp2272))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp2273))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}} _LL2157:{ void* _temp2274=( void*) _temp2228->r;
struct Cyc_Absyn_Vardecl* _temp2284; int _temp2286; struct Cyc_Absyn_Exp*
_temp2288; struct Cyc_Core_Opt* _temp2290; struct Cyc_Core_Opt* _temp2292;
struct Cyc_Absyn_Pat* _temp2294; struct Cyc_List_List* _temp2296; _LL2276: if(*((
int*) _temp2274) == Cyc_Absyn_Var_d){ _LL2285: _temp2284=(( struct Cyc_Absyn_Var_d_struct*)
_temp2274)->f1; goto _LL2277;} else{ goto _LL2278;} _LL2278: if(*(( int*)
_temp2274) == Cyc_Absyn_Let_d){ _LL2295: _temp2294=(( struct Cyc_Absyn_Let_d_struct*)
_temp2274)->f1; goto _LL2293; _LL2293: _temp2292=(( struct Cyc_Absyn_Let_d_struct*)
_temp2274)->f2; goto _LL2291; _LL2291: _temp2290=(( struct Cyc_Absyn_Let_d_struct*)
_temp2274)->f3; goto _LL2289; _LL2289: _temp2288=(( struct Cyc_Absyn_Let_d_struct*)
_temp2274)->f4; goto _LL2287; _LL2287: _temp2286=(( struct Cyc_Absyn_Let_d_struct*)
_temp2274)->f5; goto _LL2279;} else{ goto _LL2280;} _LL2280: if(*(( int*)
_temp2274) == Cyc_Absyn_Letv_d){ _LL2297: _temp2296=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2274)->f1; goto _LL2281;} else{ goto _LL2282;} _LL2282: goto _LL2283;
_LL2277: { struct Cyc_Toc_Env* _temp2298= Cyc_Toc_add_varmap( nv, _temp2284->name,
Cyc_Absyn_varb_exp( _temp2284->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2299=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2299[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2300; _temp2300.tag= Cyc_Absyn_Local_b;
_temp2300.f1= _temp2284; _temp2300;}); _temp2299;}), 0)); Cyc_Toc_local_decl_to_c(
_temp2298, _temp2298, _temp2284, _temp2226); goto _LL2275;} _LL2279:{ void*
_temp2301=( void*) _temp2294->r; struct Cyc_Absyn_Vardecl* _temp2307; _LL2303:
if(( unsigned int) _temp2301 > 2u?*(( int*) _temp2301) == Cyc_Absyn_Var_p: 0){
_LL2308: _temp2307=(( struct Cyc_Absyn_Var_p_struct*) _temp2301)->f1; goto
_LL2304;} else{ goto _LL2305;} _LL2305: goto _LL2306; _LL2304: { struct _tuple0*
old_name= _temp2307->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp2307->name= new_name; _temp2307->initializer=( struct Cyc_Absyn_Exp*)
_temp2288;( void*)( _temp2228->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2309=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2309[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2310; _temp2310.tag= Cyc_Absyn_Var_d;
_temp2310.f1= _temp2307; _temp2310;}); _temp2309;})));{ struct Cyc_Toc_Env*
_temp2311= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2312=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2312[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2313; _temp2313.tag= Cyc_Absyn_Local_b; _temp2313.f1=
_temp2307; _temp2313;}); _temp2312;}), 0)); Cyc_Toc_local_decl_to_c( _temp2311,
nv, _temp2307, _temp2226); goto _LL2302;}} _LL2306:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp2294, _temp2290, _temp2288, _temp2286,
_temp2226))->r)); goto _LL2302; _LL2302:;} goto _LL2275; _LL2281: { struct Cyc_List_List*
_temp2314=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2296); if( _temp2314 == 0){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("empty Letv_d", sizeof( unsigned char), 13u));}( void*)( _temp2228->r=(
void*)(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2315=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2315[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2316; _temp2316.tag= Cyc_Absyn_Var_d; _temp2316.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2314))->hd; _temp2316;}); _temp2315;})));
_temp2314=(( struct Cyc_List_List*) _check_null( _temp2314))->tl; for( 0;
_temp2314 != 0; _temp2314=(( struct Cyc_List_List*) _check_null( _temp2314))->tl){
struct Cyc_Absyn_Decl* _temp2317= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2318=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2318[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2319; _temp2319.tag= Cyc_Absyn_Var_d;
_temp2319.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2314))->hd; _temp2319;}); _temp2318;}), 0);( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_decl_stmt( _temp2317, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));}
Cyc_Toc_stmt_to_c( nv, s); goto _LL2275;} _LL2283:(( int(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u)); goto _LL2275; _LL2275:;} return; _LL2159: s= _temp2230;
continue; _LL2161: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2240); Cyc_Toc_exp_to_c(
nv, _temp2238); return; _LL2163: { struct _tuple0* h_var= Cyc_Toc_temp_var();
struct _tuple0* e_var= Cyc_Toc_temp_var(); struct _tuple0* was_thrown_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp2320=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2320->v=( void*) e_typ; _temp2320;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2244);{
struct Cyc_Absyn_Stmt* _temp2321= Cyc_Absyn_seq_stmt( _temp2244, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp2322= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp2323= Cyc_Absyn_var_exp(
_temp2322, 0); struct Cyc_Absyn_Vardecl* _temp2324= Cyc_Absyn_new_vardecl(
_temp2322, Cyc_Absyn_exn_typ, 0); _temp2323->topt=({ struct Cyc_Core_Opt*
_temp2325=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2325->v=( void*) Cyc_Absyn_exn_typ; _temp2325;});{ struct Cyc_Absyn_Pat*
_temp2326=({ struct Cyc_Absyn_Pat* _temp2342=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2342->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2344=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2344[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2345; _temp2345.tag= Cyc_Absyn_Var_p;
_temp2345.f1= _temp2324; _temp2345;}); _temp2344;})); _temp2342->topt=({ struct
Cyc_Core_Opt* _temp2343=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2343->v=( void*) Cyc_Absyn_exn_typ; _temp2343;}); _temp2342->loc= 0;
_temp2342;}); struct Cyc_Absyn_Exp* _temp2327= Cyc_Absyn_throw_exp( _temp2323, 0);
_temp2327->topt=({ struct Cyc_Core_Opt* _temp2328=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2328->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp2328;});{ struct Cyc_Absyn_Stmt* _temp2329= Cyc_Absyn_exp_stmt( _temp2327,
0); struct Cyc_Absyn_Switch_clause* _temp2330=({ struct Cyc_Absyn_Switch_clause*
_temp2339=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2339->pattern= _temp2326; _temp2339->pat_vars=({ struct Cyc_Core_Opt*
_temp2340=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2340->v=( void*)({ struct Cyc_List_List* _temp2341=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2341->hd=( void*) _temp2324;
_temp2341->tl= 0; _temp2341;}); _temp2340;}); _temp2339->where_clause= 0;
_temp2339->body= _temp2329; _temp2339->loc= 0; _temp2339;}); struct Cyc_Absyn_Stmt*
_temp2331= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2242,({ struct Cyc_List_List*
_temp2338=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2338->hd=( void*) _temp2330; _temp2338->tl= 0; _temp2338;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp2331);{ struct Cyc_Absyn_Exp* _temp2332= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp2337=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2337->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2337->tl= 0; _temp2337;}), 0); struct Cyc_Absyn_Stmt*
_temp2333= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp2336=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2336->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp2336->tl= 0; _temp2336;}), 0), 0); struct Cyc_Absyn_Exp* _temp2334= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp2335= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2333, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2334, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2332, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2335, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2321, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp2331, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL2165: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp2248->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp2246);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_List_List*(* _temp2346)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp2349= x_exp; struct Cyc_Absyn_Exp* _temp2347[ 1u]={ _temp2349}; struct
_tagged_arr _temp2348={( void*) _temp2347,( void*) _temp2347,( void*)( _temp2347
+ 1u)}; _temp2346( _temp2348);}), 0), 0), Cyc_Absyn_seq_stmt( _temp2246, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({ struct Cyc_List_List*(* _temp2350)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp2353= x_exp; struct Cyc_Absyn_Exp* _temp2351[ 1u]={
_temp2353}; struct _tagged_arr _temp2352={( void*) _temp2351,( void*) _temp2351,(
void*)( _temp2351 + 1u)}; _temp2350( _temp2352);}), 0), 0), 0), 0), 0), 0))->r));
return;} _LL2167:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("switchC_s",
sizeof( unsigned char), 10u)); return; _LL2169:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("cut", sizeof( unsigned char), 4u)); return; _LL2171:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof(
unsigned char), 7u)); return; _LL2131:;}} struct _tuple16{ struct _tagged_arr*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0; f->rgn_po=
0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* _temp2354= f->args; for( 0; _temp2354 != 0; _temp2354=((
struct Cyc_List_List*) _check_null( _temp2354))->tl){(*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( _temp2354))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2354))->hd)).f3);{
struct _tuple0* _temp2355=({ struct _tuple0* _temp2356=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2356->f1=( void*) Cyc_Absyn_Loc_n;
_temp2356->f2=(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2354))->hd)).f1; _temp2356;}); nv= Cyc_Toc_add_varmap( nv, _temp2355, Cyc_Absyn_var_exp(
_temp2355, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name != 0: 0){ int _temp2359; void* _temp2361;
void* _temp2363; struct Cyc_Absyn_Tqual _temp2365; struct Cyc_Core_Opt*
_temp2367; struct Cyc_Absyn_VarargInfo _temp2357=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs)); _LL2368: _temp2367= _temp2357.name; goto _LL2366;
_LL2366: _temp2365= _temp2357.tq; goto _LL2364; _LL2364: _temp2363=( void*)
_temp2357.type; goto _LL2362; _LL2362: _temp2361=( void*) _temp2357.rgn; goto
_LL2360; _LL2360: _temp2359= _temp2357.inject; goto _LL2358; _LL2358: { void*
_temp2369= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp2363, _temp2361,
_temp2365)); struct _tuple0* _temp2370=({ struct _tuple0* _temp2373=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2373->f1=( void*) Cyc_Absyn_Loc_n;
_temp2373->f2=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
_temp2367))->v; _temp2373;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2371=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2371->hd=( void*)({ struct _tuple16* _temp2372=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp2372->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp2367))->v; _temp2372->f2= _temp2365;
_temp2372->f3= _temp2369; _temp2372;}); _temp2371->tl= 0; _temp2371;})); nv= Cyc_Toc_add_varmap(
nv, _temp2370, Cyc_Absyn_var_exp( _temp2370, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp2374=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp2374 != 0; _temp2374=(( struct
Cyc_List_List*) _check_null( _temp2374))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2374))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2374))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2375= s; _LL2377: if(
_temp2375 ==( void*) Cyc_Absyn_Abstract){ goto _LL2378;} else{ goto _LL2379;}
_LL2379: if( _temp2375 ==( void*) Cyc_Absyn_ExternC){ goto _LL2380;} else{ goto
_LL2381;} _LL2381: goto _LL2382; _LL2378: return( void*) Cyc_Absyn_Public;
_LL2380: return( void*) Cyc_Absyn_Extern; _LL2382: return s; _LL2376:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2383=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 0){
Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2384=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2384->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2384;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2385=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2383).f2); if( _temp2385 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2383).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2385))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2383).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2386=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2386->v=(
void*) _temp2383; _temp2386;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2387=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp2387 != 0; _temp2387=(( struct Cyc_List_List*)
_check_null( _temp2387))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2387))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2387))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp2388=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far == 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp2389=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2389->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2389;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2390=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2388).f2); if( _temp2390 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2388).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2390))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp2388).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2391=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2391->v=(
void*) _temp2388; _temp2391;}); if( u->fields != 0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp2392=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2392 != 0; _temp2392=(( struct Cyc_List_List*)
_check_null( _temp2392))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2392))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2392))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2393=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2393->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2393;});}{ struct
_tuple0* _temp2394= tud->name; if( tud->fields == 0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp2394)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2394));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp2395=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp2395 != 0; _temp2395=(( struct Cyc_List_List*) _check_null(
_temp2395))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2395))->hd; if( f->typs == 0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2396; _temp2396.q_const= 1;
_temp2396.q_volatile= 0; _temp2396.q_restrict= 0; _temp2396;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2397=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2397->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2398=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2398[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2399; _temp2399.tag= Cyc_Absyn_Var_d; _temp2399.f1= vd; _temp2399;});
_temp2398;}), 0); _temp2397->tl= Cyc_Toc_result_decls; _temp2397;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2400; _temp2400.q_const=
1; _temp2400.q_volatile= 0; _temp2400.q_restrict= 0; _temp2400;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2401=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2401->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2402=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2402[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2403; _temp2403.tag= Cyc_Absyn_Var_d; _temp2403.f1= vd; _temp2403;});
_temp2402;}), 0); _temp2401->tl= Cyc_Toc_result_decls; _temp2401;});{ struct Cyc_List_List*
_temp2404= 0; int i= 1;{ struct Cyc_List_List* _temp2405= f->typs; for( 0;
_temp2405 != 0;( _temp2405=(( struct Cyc_List_List*) _check_null( _temp2405))->tl,
i ++)){ struct _tagged_arr* _temp2406= Cyc_Toc_fieldname( i); struct Cyc_Absyn_Structfield*
_temp2407=({ struct Cyc_Absyn_Structfield* _temp2409=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2409->name= _temp2406;
_temp2409->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2405))->hd)).f1; _temp2409->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2405))->hd)).f2);
_temp2409->width= 0; _temp2409->attributes= 0; _temp2409;}); _temp2404=({ struct
Cyc_List_List* _temp2408=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2408->hd=( void*) _temp2407; _temp2408->tl= _temp2404; _temp2408;});}}
_temp2404=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2404); _temp2404=({ struct Cyc_List_List* _temp2410=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2410->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2411=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2411->name= Cyc_Toc_tag_sp; _temp2411->tq= Cyc_Toc_mt_tq; _temp2411->type=(
void*) Cyc_Absyn_sint_t; _temp2411->width= 0; _temp2411->attributes= 0;
_temp2411;}); _temp2410->tl= _temp2404; _temp2410;});{ struct Cyc_Absyn_Structdecl*
_temp2412=({ struct Cyc_Absyn_Structdecl* _temp2416=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2416->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2416->name=({ struct Cyc_Core_Opt* _temp2418=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2418->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2418;}); _temp2416->tvs= 0; _temp2416->fields=({ struct Cyc_Core_Opt*
_temp2417=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2417->v=( void*) _temp2404; _temp2417;}); _temp2416->attributes= 0;
_temp2416;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2413=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2413->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2414=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2414[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2415; _temp2415.tag= Cyc_Absyn_Struct_d;
_temp2415.f1= _temp2412; _temp2415;}); _temp2414;}), 0); _temp2413->tl= Cyc_Toc_result_decls;
_temp2413;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2419=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2419->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2419;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2420= xd->name; struct Cyc_List_List*
_temp2421=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2421 != 0; _temp2421=(( struct Cyc_List_List*) _check_null(
_temp2421))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2421))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2422= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) + 4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2423=({ struct
Cyc_Absyn_ArrayType_struct* _temp2469=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2469[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2470; _temp2470.tag= Cyc_Absyn_ArrayType;
_temp2470.f1=( void*) Cyc_Absyn_uchar_t; _temp2470.f2= Cyc_Toc_mt_tq; _temp2470.f3=(
struct Cyc_Absyn_Exp*) _temp2422; _temp2470;}); _temp2469;}); struct Cyc_Core_Opt*
_temp2424=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2432; int _temp2433; _LL2426: if( _temp2424 == 0){ goto _LL2427;} else{
goto _LL2428;} _LL2428: if( _temp2424 == 0){ goto _LL2430;} else{ _temp2432=*
_temp2424; _LL2434: _temp2433=( int) _temp2432.v; if( _temp2433 == 0){ goto
_LL2429;} else{ goto _LL2430;}} _LL2430: goto _LL2431; _LL2427: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp(( struct _tagged_arr)({ int _temp2435= 0; int _temp2436= 0;
int _temp2437= 0; int _temp2438= 0; struct _tagged_arr _temp2439=* fn; xprintf("%c%c%c%c%.*s",
_temp2435, _temp2436, _temp2437, _temp2438, _get_arr_size( _temp2439, 1u),
_temp2439.curr);}), 0);}{ struct Cyc_Absyn_Vardecl* _temp2440= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2423, initopt);( void*)( _temp2440->sc=( void*)(( void*) f->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2441=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2441->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2442=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2442[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2443; _temp2443.tag= Cyc_Absyn_Var_d; _temp2443.f1= _temp2440; _temp2443;});
_temp2442;}), 0); _temp2441->tl= Cyc_Toc_result_decls; _temp2441;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*) Cyc_Absyn_Extern));
if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2444= f->typs; for( 0; _temp2444 != 0;( _temp2444=(( struct Cyc_List_List*)
_check_null( _temp2444))->tl, i ++)){ struct _tagged_arr* _temp2445=({ struct
_tagged_arr* _temp2449=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2449[ 0]=( struct _tagged_arr) xprintf("f%d", i); _temp2449;});
struct Cyc_Absyn_Structfield* _temp2446=({ struct Cyc_Absyn_Structfield*
_temp2448=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2448->name= _temp2445; _temp2448->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2444))->hd)).f1; _temp2448->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2444))->hd)).f2);
_temp2448->width= 0; _temp2448->attributes= 0; _temp2448;}); fields=({ struct
Cyc_List_List* _temp2447=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2447->hd=( void*) _temp2446; _temp2447->tl= fields; _temp2447;});}} fields=({
struct Cyc_List_List* _temp2450=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2450->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2451=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2451->name= Cyc_Toc_tag_sp; _temp2451->tq= Cyc_Toc_mt_tq; _temp2451->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2451->width=
0; _temp2451->attributes= 0; _temp2451;}); _temp2450->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2450;});{ struct Cyc_Absyn_Structdecl*
_temp2452=({ struct Cyc_Absyn_Structdecl* _temp2456=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2456->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2456->name=({ struct Cyc_Core_Opt* _temp2458=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2458->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2458;}); _temp2456->tvs= 0; _temp2456->fields=({ struct Cyc_Core_Opt*
_temp2457=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2457->v=( void*) fields; _temp2457;}); _temp2456->attributes= 0; _temp2456;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2453=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2453->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2454=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2454[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2455; _temp2455.tag= Cyc_Absyn_Struct_d;
_temp2455.f1= _temp2452; _temp2455;}); _temp2454;}), 0); _temp2453->tl= Cyc_Toc_result_decls;
_temp2453;});}} goto _LL2425;}} _LL2429: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2459= Cyc_Absyn_string_exp(( struct _tagged_arr)({
int _temp2464= 0; int _temp2465= 0; int _temp2466= 0; int _temp2467= 0; struct
_tagged_arr _temp2468=* fn; xprintf("%c%c%c%c%.*s", _temp2464, _temp2465,
_temp2466, _temp2467, _get_arr_size( _temp2468, 1u), _temp2468.curr);}), 0);
struct Cyc_Absyn_Vardecl* _temp2460= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp2423,( struct Cyc_Absyn_Exp*) _temp2459);( void*)( _temp2460->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2461=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2461->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2462=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2462[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2463; _temp2463.tag= Cyc_Absyn_Var_d;
_temp2463.f1= _temp2460; _temp2463;}); _temp2462;}), 0); _temp2461->tl= Cyc_Toc_result_decls;
_temp2461;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2425;
_LL2431: goto _LL2425; _LL2425:;}}} static void Cyc_Toc_enumdecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2471= ed->name;(
void*)( ed->sc=( void*)(( void*) Cyc_Absyn_Public)); if( ed->fields != 0){
struct Cyc_List_List* _temp2472=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; _temp2472 != 0; _temp2472=(( struct Cyc_List_List*)
_check_null( _temp2472))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2472))->hd)->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2472))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2472))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer != 0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2473=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2479; struct Cyc_Absyn_Exp* _temp2481; struct Cyc_Absyn_Vardecl* _temp2483;
_LL2475: if(*(( int*) _temp2473) == Cyc_Absyn_Comprehension_e){ _LL2484:
_temp2483=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2473)->f1; goto
_LL2482; _LL2482: _temp2481=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2473)->f2; goto _LL2480; _LL2480: _temp2479=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2473)->f3; goto _LL2476;} else{ goto _LL2477;} _LL2477: goto _LL2478;
_LL2476: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2483, _temp2481, _temp2479, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2474; _LL2478: Cyc_Toc_exp_to_c( init_nv,
init); goto _LL2474; _LL2474:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2485= _new_region(); struct _RegionHandle* prgn=& _temp2485;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2488; struct Cyc_List_List*
_temp2490; struct Cyc_Toc_Env* _temp2492; struct _tuple12 _temp2486= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2493: _temp2492= _temp2486.f1; goto _LL2491; _LL2491: _temp2490=
_temp2486.f2; goto _LL2489; _LL2489: _temp2488= _temp2486.f3; goto _LL2487;
_LL2487: Cyc_Toc_stmt_to_c( _temp2492, s);{ struct Cyc_Absyn_Stmt* _temp2494=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2488, _temp2494, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2495= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2496= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2495, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2488,
Cyc_Absyn_seq_stmt( _temp2496, _temp2494, 0), 0), 0);} for( 0; _temp2490 != 0;
_temp2490=(( struct Cyc_List_List*) _check_null( _temp2490))->tl){ struct
_tuple13 _temp2499; void* _temp2500; struct _tuple0* _temp2502; struct _tuple13*
_temp2497=( struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp2490))->hd;
_temp2499=* _temp2497; _LL2503: _temp2502= _temp2499.f1; goto _LL2501; _LL2501:
_temp2500= _temp2499.f2; goto _LL2498; _LL2498: s= Cyc_Absyn_declare_stmt(
_temp2502, _temp2500, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u));}{ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2504=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2530; struct Cyc_Absyn_Fndecl* _temp2532; int
_temp2534; struct Cyc_Absyn_Exp* _temp2536; struct Cyc_Core_Opt* _temp2538;
struct Cyc_Core_Opt* _temp2540; struct Cyc_Absyn_Pat* _temp2542; struct Cyc_List_List*
_temp2544; struct Cyc_Absyn_Structdecl* _temp2546; struct Cyc_Absyn_Uniondecl*
_temp2548; struct Cyc_Absyn_Tuniondecl* _temp2550; struct Cyc_Absyn_Enumdecl*
_temp2552; struct Cyc_Absyn_Typedefdecl* _temp2554; struct Cyc_List_List*
_temp2556; struct _tagged_arr* _temp2558; struct Cyc_List_List* _temp2560;
struct _tuple0* _temp2562; struct Cyc_List_List* _temp2564; _LL2506: if(*(( int*)
_temp2504) == Cyc_Absyn_Var_d){ _LL2531: _temp2530=(( struct Cyc_Absyn_Var_d_struct*)
_temp2504)->f1; goto _LL2507;} else{ goto _LL2508;} _LL2508: if(*(( int*)
_temp2504) == Cyc_Absyn_Fn_d){ _LL2533: _temp2532=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2504)->f1; goto _LL2509;} else{ goto _LL2510;} _LL2510: if(*(( int*)
_temp2504) == Cyc_Absyn_Let_d){ _LL2543: _temp2542=(( struct Cyc_Absyn_Let_d_struct*)
_temp2504)->f1; goto _LL2541; _LL2541: _temp2540=(( struct Cyc_Absyn_Let_d_struct*)
_temp2504)->f2; goto _LL2539; _LL2539: _temp2538=(( struct Cyc_Absyn_Let_d_struct*)
_temp2504)->f3; goto _LL2537; _LL2537: _temp2536=(( struct Cyc_Absyn_Let_d_struct*)
_temp2504)->f4; goto _LL2535; _LL2535: _temp2534=(( struct Cyc_Absyn_Let_d_struct*)
_temp2504)->f5; goto _LL2511;} else{ goto _LL2512;} _LL2512: if(*(( int*)
_temp2504) == Cyc_Absyn_Letv_d){ _LL2545: _temp2544=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2504)->f1; goto _LL2513;} else{ goto _LL2514;} _LL2514: if(*(( int*)
_temp2504) == Cyc_Absyn_Struct_d){ _LL2547: _temp2546=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2504)->f1; goto _LL2515;} else{ goto _LL2516;} _LL2516: if(*(( int*)
_temp2504) == Cyc_Absyn_Union_d){ _LL2549: _temp2548=(( struct Cyc_Absyn_Union_d_struct*)
_temp2504)->f1; goto _LL2517;} else{ goto _LL2518;} _LL2518: if(*(( int*)
_temp2504) == Cyc_Absyn_Tunion_d){ _LL2551: _temp2550=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp2504)->f1; goto _LL2519;} else{ goto _LL2520;} _LL2520: if(*(( int*)
_temp2504) == Cyc_Absyn_Enum_d){ _LL2553: _temp2552=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2504)->f1; goto _LL2521;} else{ goto _LL2522;} _LL2522: if(*(( int*)
_temp2504) == Cyc_Absyn_Typedef_d){ _LL2555: _temp2554=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2504)->f1; goto _LL2523;} else{ goto _LL2524;} _LL2524: if(*(( int*)
_temp2504) == Cyc_Absyn_Namespace_d){ _LL2559: _temp2558=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2504)->f1; goto _LL2557; _LL2557: _temp2556=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2504)->f2; goto _LL2525;} else{ goto _LL2526;} _LL2526: if(*(( int*)
_temp2504) == Cyc_Absyn_Using_d){ _LL2563: _temp2562=(( struct Cyc_Absyn_Using_d_struct*)
_temp2504)->f1; goto _LL2561; _LL2561: _temp2560=(( struct Cyc_Absyn_Using_d_struct*)
_temp2504)->f2; goto _LL2527;} else{ goto _LL2528;} _LL2528: if(*(( int*)
_temp2504) == Cyc_Absyn_ExternC_d){ _LL2565: _temp2564=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2504)->f1; goto _LL2529;} else{ goto _LL2505;} _LL2507: { struct _tuple0*
_temp2566= _temp2530->name; if(( void*) _temp2530->sc ==( void*) Cyc_Absyn_ExternC){
_temp2566=({ struct _tuple0* _temp2567=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2567->f1= Cyc_Toc_rel_ns; _temp2567->f2=(* _temp2566).f2;
_temp2567;});} if( _temp2530->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*) _check_null( _temp2530->initializer));} nv= Cyc_Toc_add_varmap(
nv, _temp2530->name, Cyc_Absyn_varb_exp( _temp2566,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2568=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2568[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2569; _temp2569.tag= Cyc_Absyn_Global_b;
_temp2569.f1= _temp2530; _temp2569;}); _temp2568;}), 0)); _temp2530->name=
_temp2566;( void*)( _temp2530->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2530->sc));(
void*)( _temp2530->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2530->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2570=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2570->hd=( void*) d; _temp2570->tl=
Cyc_Toc_result_decls; _temp2570;}); goto _LL2505;} _LL2509: { struct _tuple0*
_temp2571= _temp2532->name; if(( void*) _temp2532->sc ==( void*) Cyc_Absyn_ExternC){
_temp2571=({ struct _tuple0* _temp2572=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2572->f1= Cyc_Toc_rel_ns; _temp2572->f2=(* _temp2571).f2;
_temp2572;});} nv= Cyc_Toc_add_varmap( nv, _temp2532->name, Cyc_Absyn_var_exp(
_temp2571, 0)); _temp2532->name= _temp2571; Cyc_Toc_fndecl_to_c( nv, _temp2532);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2573=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2573->hd=( void*) d; _temp2573->tl=
Cyc_Toc_result_decls; _temp2573;}); goto _LL2505;} _LL2511: goto _LL2513;
_LL2513:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel",
sizeof( unsigned char), 20u)); goto _LL2505; _LL2515: Cyc_Toc_structdecl_to_c(
_temp2546); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2574=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2574->hd=( void*) d; _temp2574->tl=
Cyc_Toc_result_decls; _temp2574;}); goto _LL2505; _LL2517: Cyc_Toc_uniondecl_to_c(
_temp2548); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2575=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2575->hd=( void*) d; _temp2575->tl=
Cyc_Toc_result_decls; _temp2575;}); goto _LL2505; _LL2519: if( _temp2550->is_xtunion){
Cyc_Toc_xtuniondecl_to_c( _temp2550);} else{ Cyc_Toc_tuniondecl_to_c( _temp2550);}
goto _LL2505; _LL2521: Cyc_Toc_enumdecl_to_c( nv, _temp2552); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2576=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2576->hd=( void*) d; _temp2576->tl= Cyc_Toc_result_decls;
_temp2576;}); goto _LL2505; _LL2523: _temp2554->name= _temp2554->name; _temp2554->tvs=
0;( void*)( _temp2554->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2554->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2577=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2577->hd=( void*) d; _temp2577->tl=
Cyc_Toc_result_decls; _temp2577;}); goto _LL2505; _LL2525: _temp2560= _temp2556;
goto _LL2527; _LL2527: _temp2564= _temp2560; goto _LL2529; _LL2529: nv= Cyc_Toc_decls_to_c(
nv, _temp2564, top); goto _LL2505; _LL2505:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}