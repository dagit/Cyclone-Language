 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern int
Cyc_Core_intcmp( int, int); extern unsigned char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr f1; }
; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; }
; extern int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_map2( void*(* f)( void*, void*),
struct Cyc_List_List* x, struct Cyc_List_List* y); extern void Cyc_List_iter(
void(* f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter2( void(*
f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_exists_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y); struct
_tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; extern struct
_tuple0 Cyc_List_rsplit( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x); extern int Cyc_List_mem( int(* compare)( void*, void*),
struct Cyc_List_List* l, void* x); extern void* Cyc_List_assoc_cmp( int(*
compare)( void*, void*), struct Cyc_List_List* l, void* x); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
extern struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo;
struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt;
struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_SwitchC_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; static const int Cyc_Absyn_Loc_n=
0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Static= 0;
static const int Cyc_Absyn_Abstract= 1; static const int Cyc_Absyn_Public= 2;
static const int Cyc_Absyn_Extern= 3; static const int Cyc_Absyn_ExternC= 4;
struct Cyc_Absyn_Tqual{ int q_const: 1; int q_volatile: 1; int q_restrict: 1; }
; static const int Cyc_Absyn_B1= 0; static const int Cyc_Absyn_B2= 1; static
const int Cyc_Absyn_B4= 2; static const int Cyc_Absyn_B8= 3; static const int
Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind= 1; static const int
Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3; static const int
Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0; static const int Cyc_Absyn_Unsigned=
1; struct Cyc_Absyn_Conref{ void* v; } ; static const int Cyc_Absyn_Eq_constr= 0;
struct Cyc_Absyn_Eq_constr_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr=
1; struct Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1;
} ; static const int Cyc_Absyn_No_constr= 0; struct Cyc_Absyn_Tvar{ struct
_tagged_arr* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; void* rgn; int inject; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple1*
tunion_name; struct _tuple1* field_name; int is_xtunion; } ; static const int
Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{ int
tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
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
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
24; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
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
_tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
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
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple1* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple1* name; struct Cyc_List_List*
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
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple1*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*); extern
int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void*
Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern
void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulonglong_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void*
Cyc_Absyn_double_t; extern void* Cyc_Absyn_bounds_one; extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct _tagged_arr*); extern
struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*); struct _tuple4{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; extern struct _tuple4* Cyc_Absyn_lookup_tuple_field( struct
Cyc_List_List*, int); extern struct _tagged_arr Cyc_Absyn_attribute2string( void*);
extern int Cyc_Absyn_fntype_att( void* a); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int
qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*); extern struct _tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*, void* r1, void* r2); extern int Cyc_String_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strconcat(
struct _tagged_arr, struct _tagged_arr); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern
void* Cyc_Tcutil_impos( struct _tagged_arr); extern void Cyc_Tcutil_terr( struct
Cyc_Position_Segment*, struct _tagged_arr); extern void Cyc_Tcutil_warn( struct
Cyc_Position_Segment*, struct _tagged_arr); extern void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern void* Cyc_Tcutil_copy_type(
void* t); extern void* Cyc_Tcutil_tvar_kind( struct Cyc_Absyn_Tvar* t); extern
void* Cyc_Tcutil_typ_kind( void* t); extern void* Cyc_Tcutil_compress( void* t);
extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*);
extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerceable( void*); extern int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*, void*); extern int
Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*,
void*); extern int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*); extern int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_function_type(
void* t); extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern int
Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*,
void*); extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
void*); extern int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void*
e2); extern int Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e);
extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_Position_Segment*);
struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple5*
Cyc_Tcutil_make_inst_var( struct Cyc_List_List*, struct Cyc_Absyn_Tvar*); struct
_tuple6{ struct Cyc_List_List* f1; struct _RegionHandle* f2; } ; extern struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple6*, struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*); extern
void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*, struct
Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*); extern
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr
err_msg); extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void**
elt_typ_dest); extern void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*,
void* t, struct Cyc_Absyn_Exp* e); struct _tuple7{ int f1; void* f2; } ; extern
struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k); extern int* Cyc_Tcutil_new_tvar_id();
extern void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*); extern void
Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*); extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*);
extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*, struct Cyc_List_List*);
extern int Cyc_Tcutil_bits_only( void* t); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern void* Cyc_Tcutil_snd_tqt(
struct _tuple4*); extern int Cyc_Tcutil_supports_default( void*); extern void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
unsigned char Cyc_Tcutil_TypeErr[ 12u]="\000\000\000\000TypeErr"; extern void
Cyc_Tcutil_unify_it( void* t1, void* t2); void Cyc_Tcutil_terr( struct Cyc_Position_Segment*
loc, struct _tagged_arr s){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc, s));} void* Cyc_Tcutil_impos( struct _tagged_arr msg){({ struct _tagged_arr
_temp0= msg; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"Error: %.*s\n",
_get_arr_size( _temp0, 1u), _temp0.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Tcutil_TypeErr);} static struct
_tagged_arr Cyc_Tcutil_tvar2string( struct Cyc_Absyn_Tvar* tv){ return* tv->name;}
void Cyc_Tcutil_print_tvars( struct Cyc_List_List* tvs){ for( 0; tvs != 0; tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl){({ struct _tagged_arr _temp1= Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd); struct
_tagged_arr _temp2= Cyc_Absynpp_ckind2string((( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs))->hd)->kind); fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s::%.*s ",
_get_arr_size( _temp1, 1u), _temp1.curr, _get_arr_size( _temp2, 1u), _temp2.curr);});}
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"\n"); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);} static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0;
static struct Cyc_List_List* Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn(
struct Cyc_Position_Segment* sg, struct _tagged_arr msg){ Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp3=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp3->hd=( void*) sg; _temp3->tl= Cyc_Tcutil_warning_segs;
_temp3;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp4=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4->hd=( void*)({ struct
_tagged_arr* _temp5=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp5[ 0]= msg; _temp5;}); _temp4->tl= Cyc_Tcutil_warning_msgs; _temp4;});}
void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs == 0){ return;}
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"***Warnings***\n");{ struct Cyc_List_List*
_temp6= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs); Cyc_Tcutil_warning_segs=
0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while( Cyc_Tcutil_warning_msgs != 0) {({
struct _tagged_arr _temp7=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct
Cyc_List_List*) _check_null( _temp6))->hd); struct _tagged_arr _temp8=*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->hd);
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s\n", _get_arr_size( _temp7,
1u), _temp7.curr, _get_arr_size( _temp8, 1u), _temp8.curr);}); _temp6=(( struct
Cyc_List_List*) _check_null( _temp6))->tl; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*)
_check_null( Cyc_Tcutil_warning_msgs))->tl;} fprintf( _sfile_to_file( Cyc_Stdio_stderr),"**************\n");
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);}} struct Cyc_Core_Opt*
Cyc_Tcutil_empty_var_set= 0; static int Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar*
tv1, struct Cyc_Absyn_Tvar* tv2){ return*(( int*) _check_null( tv1->identity)) -*((
int*) _check_null( tv2->identity));} void* Cyc_Tcutil_compress( void* t){ void*
_temp9= t; struct Cyc_Core_Opt* _temp21; struct Cyc_Core_Opt* _temp23; struct
Cyc_Core_Opt** _temp25; struct Cyc_Core_Opt* _temp26; struct Cyc_Core_Opt*
_temp28; struct Cyc_Core_Opt** _temp30; _LL11: if(( unsigned int) _temp9 > 4u?*((
int*) _temp9) == Cyc_Absyn_Evar: 0){ _LL22: _temp21=(( struct Cyc_Absyn_Evar_struct*)
_temp9)->f2; if( _temp21 == 0){ goto _LL12;} else{ goto _LL13;}} else{ goto
_LL13;} _LL13: if(( unsigned int) _temp9 > 4u?*(( int*) _temp9) == Cyc_Absyn_Evar:
0){ _LL24: _temp23=(( struct Cyc_Absyn_Evar_struct*) _temp9)->f2; _temp25=&((
struct Cyc_Absyn_Evar_struct*) _temp9)->f2; goto _LL14;} else{ goto _LL15;}
_LL15: if(( unsigned int) _temp9 > 4u?*(( int*) _temp9) == Cyc_Absyn_TypedefType:
0){ _LL27: _temp26=(( struct Cyc_Absyn_TypedefType_struct*) _temp9)->f3; if(
_temp26 == 0){ goto _LL16;} else{ goto _LL17;}} else{ goto _LL17;} _LL17: if((
unsigned int) _temp9 > 4u?*(( int*) _temp9) == Cyc_Absyn_TypedefType: 0){ _LL29:
_temp28=(( struct Cyc_Absyn_TypedefType_struct*) _temp9)->f3; _temp30=&(( struct
Cyc_Absyn_TypedefType_struct*) _temp9)->f3; goto _LL18;} else{ goto _LL19;}
_LL19: goto _LL20; _LL12: return t; _LL14: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp25))->v); if( t2 !=( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp25))->v){* _temp25=({ struct Cyc_Core_Opt*
_temp31=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp31->v=( void*) t2; _temp31;});} return t2;} _LL16: return t; _LL18: { void*
t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp30))->v);
if( t2 !=( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp30))->v){* _temp30=({
struct Cyc_Core_Opt* _temp32=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp32->v=( void*) t2; _temp32;});} return t2;} _LL20: return t;
_LL10:;} void* Cyc_Tcutil_copy_type( void* t); static struct Cyc_List_List* Cyc_Tcutil_copy_types(
struct Cyc_List_List* ts){ return Cyc_List_map( Cyc_Tcutil_copy_type, ts);}
static struct Cyc_Absyn_Conref* Cyc_Tcutil_copy_conref( struct Cyc_Absyn_Conref*
c){ void* _temp33=( void*) c->v; void* _temp41; struct Cyc_Absyn_Conref* _temp43;
_LL35: if( _temp33 ==( void*) Cyc_Absyn_No_constr){ goto _LL36;} else{ goto
_LL37;} _LL37: if(( unsigned int) _temp33 > 1u?*(( int*) _temp33) == Cyc_Absyn_Eq_constr:
0){ _LL42: _temp41=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp33)->f1;
goto _LL38;} else{ goto _LL39;} _LL39: if(( unsigned int) _temp33 > 1u?*(( int*)
_temp33) == Cyc_Absyn_Forward_constr: 0){ _LL44: _temp43=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp33)->f1; goto _LL40;} else{ goto _LL34;} _LL36: return Cyc_Absyn_empty_conref();
_LL38: return Cyc_Absyn_new_conref( _temp41); _LL40: return Cyc_Tcutil_copy_conref(
_temp43); _LL34:;} static struct Cyc_Absyn_Tvar* Cyc_Tcutil_copy_tvar( struct
Cyc_Absyn_Tvar* tv){ return({ struct Cyc_Absyn_Tvar* _temp45=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp45->name= tv->name; _temp45->identity=
0; _temp45->kind= Cyc_Tcutil_copy_conref( tv->kind); _temp45;});} static struct
_tuple2* Cyc_Tcutil_copy_arg( struct _tuple2* arg){ void* _temp48; struct Cyc_Absyn_Tqual
_temp50; struct Cyc_Core_Opt* _temp52; struct _tuple2 _temp46=* arg; _LL53:
_temp52= _temp46.f1; goto _LL51; _LL51: _temp50= _temp46.f2; goto _LL49; _LL49:
_temp48= _temp46.f3; goto _LL47; _LL47: return({ struct _tuple2* _temp54=(
struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp54->f1= _temp52;
_temp54->f2= _temp50; _temp54->f3= Cyc_Tcutil_copy_type( _temp48); _temp54;});}
static struct _tuple4* Cyc_Tcutil_copy_tqt( struct _tuple4* arg){ void* _temp57;
struct Cyc_Absyn_Tqual _temp59; struct _tuple4 _temp55=* arg; _LL60: _temp59=
_temp55.f1; goto _LL58; _LL58: _temp57= _temp55.f2; goto _LL56; _LL56: return({
struct _tuple4* _temp61=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp61->f1= _temp59; _temp61->f2= Cyc_Tcutil_copy_type( _temp57); _temp61;});}
static struct Cyc_Absyn_Structfield* Cyc_Tcutil_copy_field( struct Cyc_Absyn_Structfield*
f){ return({ struct Cyc_Absyn_Structfield* _temp62=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp62->name= f->name;
_temp62->tq= f->tq; _temp62->type=( void*) Cyc_Tcutil_copy_type(( void*) f->type);
_temp62->width= f->width; _temp62->attributes= f->attributes; _temp62;});}
struct _tuple8{ void* f1; void* f2; } ; static struct _tuple8* Cyc_Tcutil_copy_rgncmp(
struct _tuple8* x){ struct _tuple8 _temp65; void* _temp66; void* _temp68; struct
_tuple8* _temp63= x; _temp65=* _temp63; _LL69: _temp68= _temp65.f1; goto _LL67;
_LL67: _temp66= _temp65.f2; goto _LL64; _LL64: return({ struct _tuple8* _temp70=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp70->f1= Cyc_Tcutil_copy_type(
_temp68); _temp70->f2= Cyc_Tcutil_copy_type( _temp66); _temp70;});} void* Cyc_Tcutil_copy_type(
void* t){ void* _temp71= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp117; struct Cyc_Absyn_TunionInfo _temp119; void* _temp121; struct Cyc_List_List*
_temp123; void* _temp125; struct Cyc_Absyn_TunionFieldInfo _temp127; struct Cyc_List_List*
_temp129; void* _temp131; struct Cyc_Absyn_PtrInfo _temp133; struct Cyc_Absyn_Conref*
_temp135; struct Cyc_Absyn_Tqual _temp137; struct Cyc_Absyn_Conref* _temp139;
void* _temp141; void* _temp143; void* _temp145; void* _temp147; struct Cyc_Absyn_Exp*
_temp149; struct Cyc_Absyn_Tqual _temp151; void* _temp153; struct Cyc_Absyn_FnInfo
_temp155; struct Cyc_List_List* _temp157; struct Cyc_List_List* _temp159; struct
Cyc_Absyn_VarargInfo* _temp161; int _temp163; struct Cyc_List_List* _temp165;
void* _temp167; struct Cyc_Core_Opt* _temp169; struct Cyc_List_List* _temp171;
struct Cyc_List_List* _temp173; struct Cyc_List_List* _temp175; struct _tuple1*
_temp177; struct Cyc_List_List* _temp179; struct _tuple1* _temp181; struct Cyc_List_List*
_temp183; struct Cyc_List_List* _temp185; struct _tuple1* _temp187; void*
_temp189; struct Cyc_List_List* _temp191; struct _tuple1* _temp193; void*
_temp195; struct Cyc_List_List* _temp197; _LL73: if( _temp71 ==( void*) Cyc_Absyn_VoidType){
goto _LL74;} else{ goto _LL75;} _LL75: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_Evar: 0){ goto _LL76;} else{ goto _LL77;} _LL77: if((
unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_VarType: 0){ _LL118:
_temp117=(( struct Cyc_Absyn_VarType_struct*) _temp71)->f1; goto _LL78;} else{
goto _LL79;} _LL79: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_TunionType:
0){ _LL120: _temp119=(( struct Cyc_Absyn_TunionType_struct*) _temp71)->f1;
_LL126: _temp125=( void*) _temp119.tunion_info; goto _LL124; _LL124: _temp123=
_temp119.targs; goto _LL122; _LL122: _temp121=( void*) _temp119.rgn; goto _LL80;}
else{ goto _LL81;} _LL81: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) ==
Cyc_Absyn_TunionFieldType: 0){ _LL128: _temp127=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp71)->f1; _LL132: _temp131=( void*) _temp127.field_info; goto _LL130; _LL130:
_temp129= _temp127.targs; goto _LL82;} else{ goto _LL83;} _LL83: if((
unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_PointerType: 0){
_LL134: _temp133=(( struct Cyc_Absyn_PointerType_struct*) _temp71)->f1; _LL144:
_temp143=( void*) _temp133.elt_typ; goto _LL142; _LL142: _temp141=( void*)
_temp133.rgn_typ; goto _LL140; _LL140: _temp139= _temp133.nullable; goto _LL138;
_LL138: _temp137= _temp133.tq; goto _LL136; _LL136: _temp135= _temp133.bounds;
goto _LL84;} else{ goto _LL85;} _LL85: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_IntType: 0){ _LL148: _temp147=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp71)->f1; goto _LL146; _LL146: _temp145=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp71)->f2; goto _LL86;} else{ goto _LL87;} _LL87: if( _temp71 ==( void*) Cyc_Absyn_FloatType){
goto _LL88;} else{ goto _LL89;} _LL89: if( _temp71 ==( void*) Cyc_Absyn_DoubleType){
goto _LL90;} else{ goto _LL91;} _LL91: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_ArrayType: 0){ _LL154: _temp153=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f1; goto _LL152; _LL152: _temp151=(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f2; goto _LL150; _LL150: _temp149=(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f3; goto _LL92;} else{ goto _LL93;} _LL93: if(( unsigned int) _temp71
> 4u?*(( int*) _temp71) == Cyc_Absyn_FnType: 0){ _LL156: _temp155=(( struct Cyc_Absyn_FnType_struct*)
_temp71)->f1; _LL172: _temp171= _temp155.tvars; goto _LL170; _LL170: _temp169=
_temp155.effect; goto _LL168; _LL168: _temp167=( void*) _temp155.ret_typ; goto
_LL166; _LL166: _temp165= _temp155.args; goto _LL164; _LL164: _temp163= _temp155.c_varargs;
goto _LL162; _LL162: _temp161= _temp155.cyc_varargs; goto _LL160; _LL160:
_temp159= _temp155.rgn_po; goto _LL158; _LL158: _temp157= _temp155.attributes;
goto _LL94;} else{ goto _LL95;} _LL95: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_TupleType: 0){ _LL174: _temp173=(( struct Cyc_Absyn_TupleType_struct*)
_temp71)->f1; goto _LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp71
> 4u?*(( int*) _temp71) == Cyc_Absyn_StructType: 0){ _LL178: _temp177=(( struct
Cyc_Absyn_StructType_struct*) _temp71)->f1; goto _LL176; _LL176: _temp175=((
struct Cyc_Absyn_StructType_struct*) _temp71)->f2; goto _LL98;} else{ goto _LL99;}
_LL99: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_UnionType:
0){ _LL182: _temp181=(( struct Cyc_Absyn_UnionType_struct*) _temp71)->f1; goto
_LL180; _LL180: _temp179=(( struct Cyc_Absyn_UnionType_struct*) _temp71)->f2;
goto _LL100;} else{ goto _LL101;} _LL101: if(( unsigned int) _temp71 > 4u?*((
int*) _temp71) == Cyc_Absyn_AnonStructType: 0){ _LL184: _temp183=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp71)->f1; goto _LL102;} else{ goto _LL103;} _LL103: if(( unsigned int)
_temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_AnonUnionType: 0){ _LL186: _temp185=((
struct Cyc_Absyn_AnonUnionType_struct*) _temp71)->f1; goto _LL104;} else{ goto
_LL105;} _LL105: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_EnumType:
0){ _LL188: _temp187=(( struct Cyc_Absyn_EnumType_struct*) _temp71)->f1; goto
_LL106;} else{ goto _LL107;} _LL107: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_RgnHandleType: 0){ _LL190: _temp189=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp71)->f1; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_TypedefType: 0){ _LL194: _temp193=((
struct Cyc_Absyn_TypedefType_struct*) _temp71)->f1; goto _LL192; _LL192:
_temp191=(( struct Cyc_Absyn_TypedefType_struct*) _temp71)->f2; goto _LL110;}
else{ goto _LL111;} _LL111: if( _temp71 ==( void*) Cyc_Absyn_HeapRgn){ goto
_LL112;} else{ goto _LL113;} _LL113: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_AccessEff: 0){ _LL196: _temp195=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp71)->f1; goto _LL114;} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_JoinEff: 0){ _LL198: _temp197=((
struct Cyc_Absyn_JoinEff_struct*) _temp71)->f1; goto _LL116;} else{ goto _LL72;}
_LL74: return t; _LL76: return t; _LL78: return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp199=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp199[ 0]=({ struct Cyc_Absyn_VarType_struct _temp200; _temp200.tag= Cyc_Absyn_VarType;
_temp200.f1= Cyc_Tcutil_copy_tvar( _temp117); _temp200;}); _temp199;}); _LL80:
return( void*)({ struct Cyc_Absyn_TunionType_struct* _temp201=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp201[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp202; _temp202.tag= Cyc_Absyn_TunionType;
_temp202.f1=({ struct Cyc_Absyn_TunionInfo _temp203; _temp203.tunion_info=( void*)
_temp125; _temp203.targs= Cyc_Tcutil_copy_types( _temp123); _temp203.rgn=( void*)
Cyc_Tcutil_copy_type( _temp121); _temp203;}); _temp202;}); _temp201;}); _LL82:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp204=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp204[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp205; _temp205.tag= Cyc_Absyn_TunionFieldType;
_temp205.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp206; _temp206.field_info=(
void*) _temp131; _temp206.targs= Cyc_Tcutil_copy_types( _temp129); _temp206;});
_temp205;}); _temp204;}); _LL84: { void* _temp207= Cyc_Tcutil_copy_type(
_temp143); void* _temp208= Cyc_Tcutil_copy_type( _temp141); struct Cyc_Absyn_Conref*
_temp209=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp139); struct Cyc_Absyn_Tqual _temp210= _temp137; struct Cyc_Absyn_Conref*
_temp211= Cyc_Tcutil_copy_conref( _temp135); return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp212=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp212[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp213; _temp213.tag= Cyc_Absyn_PointerType;
_temp213.f1=({ struct Cyc_Absyn_PtrInfo _temp214; _temp214.elt_typ=( void*)
_temp207; _temp214.rgn_typ=( void*) _temp208; _temp214.nullable= _temp209;
_temp214.tq= _temp210; _temp214.bounds= _temp211; _temp214;}); _temp213;});
_temp212;});} _LL86: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp215=(
struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp215[ 0]=({ struct Cyc_Absyn_IntType_struct _temp216; _temp216.tag= Cyc_Absyn_IntType;
_temp216.f1=( void*) _temp147; _temp216.f2=( void*) _temp145; _temp216;});
_temp215;}); _LL88: goto _LL90; _LL90: return t; _LL92: return( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp217=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp217[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp218; _temp218.tag= Cyc_Absyn_ArrayType; _temp218.f1=(
void*) Cyc_Tcutil_copy_type( _temp153); _temp218.f2= _temp151; _temp218.f3=
_temp149; _temp218;}); _temp217;}); _LL94: { struct Cyc_List_List* _temp219=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp171); struct
Cyc_Core_Opt* _temp220= _temp169 == 0? 0:({ struct Cyc_Core_Opt* _temp230=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp230->v=(
void*) Cyc_Tcutil_copy_type(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp169))->v); _temp230;}); void* _temp221= Cyc_Tcutil_copy_type( _temp167);
struct Cyc_List_List* _temp222=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_arg,
_temp165); int _temp223= _temp163; struct Cyc_Absyn_VarargInfo* cyc_varargs2= 0;
if( _temp161 != 0){ struct Cyc_Absyn_VarargInfo* cv=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp161); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp224=(
struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp224->name= cv->name; _temp224->tq= cv->tq; _temp224->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp224->rgn=( void*) Cyc_Tcutil_copy_type(( void*) cv->rgn);
_temp224->inject= cv->inject; _temp224;});}{ struct Cyc_List_List* _temp225=((
struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp159); struct Cyc_List_List*
_temp226= _temp157; return( void*)({ struct Cyc_Absyn_FnType_struct* _temp227=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp227[ 0]=({ struct Cyc_Absyn_FnType_struct _temp228; _temp228.tag= Cyc_Absyn_FnType;
_temp228.f1=({ struct Cyc_Absyn_FnInfo _temp229; _temp229.tvars= _temp219;
_temp229.effect= _temp220; _temp229.ret_typ=( void*) _temp221; _temp229.args=
_temp222; _temp229.c_varargs= _temp223; _temp229.cyc_varargs= cyc_varargs2;
_temp229.rgn_po= _temp225; _temp229.attributes= _temp226; _temp229;}); _temp228;});
_temp227;});}} _LL96: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp231=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp231[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp232; _temp232.tag= Cyc_Absyn_TupleType;
_temp232.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp173);
_temp232;}); _temp231;}); _LL98: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp233=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp233[ 0]=({ struct Cyc_Absyn_StructType_struct _temp234; _temp234.tag= Cyc_Absyn_StructType;
_temp234.f1= _temp177; _temp234.f2= Cyc_Tcutil_copy_types( _temp175); _temp234.f3=
0; _temp234;}); _temp233;}); _LL100: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp235=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp235[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp236; _temp236.tag= Cyc_Absyn_UnionType;
_temp236.f1= _temp181; _temp236.f2= Cyc_Tcutil_copy_types( _temp179); _temp236.f3=
0; _temp236;}); _temp235;}); _LL102: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp237=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp237[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp238; _temp238.tag= Cyc_Absyn_AnonStructType; _temp238.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp183); _temp238;}); _temp237;});
_LL104: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp239=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp239[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp240; _temp240.tag=
Cyc_Absyn_AnonUnionType; _temp240.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp185); _temp240;}); _temp239;}); _LL106: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp241=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp241[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp242; _temp242.tag= Cyc_Absyn_EnumType;
_temp242.f1= _temp187; _temp242.f2= 0; _temp242;}); _temp241;}); _LL108: return(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp243=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp243[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp244; _temp244.tag= Cyc_Absyn_RgnHandleType;
_temp244.f1=( void*) Cyc_Tcutil_copy_type( _temp189); _temp244;}); _temp243;});
_LL110: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp245=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp245[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp246; _temp246.tag= Cyc_Absyn_TypedefType;
_temp246.f1= _temp193; _temp246.f2= Cyc_Tcutil_copy_types( _temp191); _temp246.f3=
0; _temp246;}); _temp245;}); _LL112: return t; _LL114: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp247=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp247[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp248; _temp248.tag= Cyc_Absyn_AccessEff; _temp248.f1=(
void*) Cyc_Tcutil_copy_type( _temp195); _temp248;}); _temp247;}); _LL116: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp249=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp249[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp250; _temp250.tag= Cyc_Absyn_JoinEff; _temp250.f1= Cyc_Tcutil_copy_types(
_temp197); _temp250;}); _temp249;}); _LL72:;} static int Cyc_Tcutil_kind_leq(
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple8 _temp252=({
struct _tuple8 _temp251; _temp251.f1= k1; _temp251.f2= k2; _temp251;}); void*
_temp262; void* _temp264; void* _temp266; void* _temp268; void* _temp270; void*
_temp272; _LL254: _LL265: _temp264= _temp252.f1; if( _temp264 ==( void*) Cyc_Absyn_BoxKind){
goto _LL263;} else{ goto _LL256;} _LL263: _temp262= _temp252.f2; if( _temp262 ==(
void*) Cyc_Absyn_MemKind){ goto _LL255;} else{ goto _LL256;} _LL256: _LL269:
_temp268= _temp252.f1; if( _temp268 ==( void*) Cyc_Absyn_BoxKind){ goto _LL267;}
else{ goto _LL258;} _LL267: _temp266= _temp252.f2; if( _temp266 ==( void*) Cyc_Absyn_AnyKind){
goto _LL257;} else{ goto _LL258;} _LL258: _LL273: _temp272= _temp252.f1; if(
_temp272 ==( void*) Cyc_Absyn_MemKind){ goto _LL271;} else{ goto _LL260;} _LL271:
_temp270= _temp252.f2; if( _temp270 ==( void*) Cyc_Absyn_AnyKind){ goto _LL259;}
else{ goto _LL260;} _LL260: goto _LL261; _LL255: return 1; _LL257: return 1;
_LL259: return 1; _LL261: return 0; _LL253:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return Cyc_Absyn_conref_val( tv->kind);} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp274= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp326;
struct Cyc_Core_Opt* _temp328; struct Cyc_Absyn_Tvar* _temp330; void* _temp332;
struct Cyc_Absyn_TunionFieldInfo _temp334; void* _temp336; struct Cyc_Absyn_Tunionfield*
_temp338; struct Cyc_Absyn_TunionFieldInfo _temp340; void* _temp342; struct Cyc_Absyn_Structdecl**
_temp344; struct Cyc_Absyn_Uniondecl** _temp346; struct Cyc_Absyn_PtrInfo
_temp348; void* _temp350; struct Cyc_List_List* _temp352; struct Cyc_Core_Opt*
_temp354; _LL276: if(( unsigned int) _temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_Evar:
0){ _LL329: _temp328=(( struct Cyc_Absyn_Evar_struct*) _temp274)->f1; goto
_LL327; _LL327: _temp326=(( struct Cyc_Absyn_Evar_struct*) _temp274)->f2; goto
_LL277;} else{ goto _LL278;} _LL278: if(( unsigned int) _temp274 > 4u?*(( int*)
_temp274) == Cyc_Absyn_VarType: 0){ _LL331: _temp330=(( struct Cyc_Absyn_VarType_struct*)
_temp274)->f1; goto _LL279;} else{ goto _LL280;} _LL280: if( _temp274 ==( void*)
Cyc_Absyn_VoidType){ goto _LL281;} else{ goto _LL282;} _LL282: if(( unsigned int)
_temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_IntType: 0){ _LL333: _temp332=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp274)->f2; goto _LL283;} else{
goto _LL284;} _LL284: if( _temp274 ==( void*) Cyc_Absyn_FloatType){ goto _LL285;}
else{ goto _LL286;} _LL286: if( _temp274 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL287;} else{ goto _LL288;} _LL288: if(( unsigned int) _temp274 > 4u?*(( int*)
_temp274) == Cyc_Absyn_FnType: 0){ goto _LL289;} else{ goto _LL290;} _LL290: if((
unsigned int) _temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_RgnHandleType: 0){
goto _LL291;} else{ goto _LL292;} _LL292: if( _temp274 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL293;} else{ goto _LL294;} _LL294: if(( unsigned int) _temp274 > 4u?*((
int*) _temp274) == Cyc_Absyn_TunionType: 0){ goto _LL295;} else{ goto _LL296;}
_LL296: if(( unsigned int) _temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_TunionFieldType:
0){ _LL335: _temp334=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp274)->f1;
_LL337: _temp336=( void*) _temp334.field_info; if(*(( int*) _temp336) == Cyc_Absyn_KnownTunionfield){
_LL339: _temp338=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp336)->f2;
goto _LL297;} else{ goto _LL298;}} else{ goto _LL298;} _LL298: if(( unsigned int)
_temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_TunionFieldType: 0){ _LL341:
_temp340=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp274)->f1; _LL343:
_temp342=( void*) _temp340.field_info; if(*(( int*) _temp342) == Cyc_Absyn_UnknownTunionfield){
goto _LL299;} else{ goto _LL300;}} else{ goto _LL300;} _LL300: if(( unsigned int)
_temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_StructType: 0){ _LL345: _temp344=((
struct Cyc_Absyn_StructType_struct*) _temp274)->f3; if( _temp344 == 0){ goto
_LL301;} else{ goto _LL302;}} else{ goto _LL302;} _LL302: if(( unsigned int)
_temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_UnionType: 0){ _LL347: _temp346=((
struct Cyc_Absyn_UnionType_struct*) _temp274)->f3; if( _temp346 == 0){ goto
_LL303;} else{ goto _LL304;}} else{ goto _LL304;} _LL304: if(( unsigned int)
_temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_StructType: 0){ goto _LL305;}
else{ goto _LL306;} _LL306: if(( unsigned int) _temp274 > 4u?*(( int*) _temp274)
== Cyc_Absyn_UnionType: 0){ goto _LL307;} else{ goto _LL308;} _LL308: if((
unsigned int) _temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_AnonStructType: 0){
goto _LL309;} else{ goto _LL310;} _LL310: if(( unsigned int) _temp274 > 4u?*((
int*) _temp274) == Cyc_Absyn_AnonUnionType: 0){ goto _LL311;} else{ goto _LL312;}
_LL312: if(( unsigned int) _temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_EnumType:
0){ goto _LL313;} else{ goto _LL314;} _LL314: if(( unsigned int) _temp274 > 4u?*((
int*) _temp274) == Cyc_Absyn_PointerType: 0){ _LL349: _temp348=(( struct Cyc_Absyn_PointerType_struct*)
_temp274)->f1; goto _LL315;} else{ goto _LL316;} _LL316: if(( unsigned int)
_temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_ArrayType: 0){ _LL351: _temp350=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp274)->f1; goto _LL317;} else{
goto _LL318;} _LL318: if(( unsigned int) _temp274 > 4u?*(( int*) _temp274) ==
Cyc_Absyn_TupleType: 0){ _LL353: _temp352=(( struct Cyc_Absyn_TupleType_struct*)
_temp274)->f1; goto _LL319;} else{ goto _LL320;} _LL320: if(( unsigned int)
_temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_TypedefType: 0){ _LL355: _temp354=((
struct Cyc_Absyn_TypedefType_struct*) _temp274)->f3; goto _LL321;} else{ goto
_LL322;} _LL322: if(( unsigned int) _temp274 > 4u?*(( int*) _temp274) == Cyc_Absyn_AccessEff:
0){ goto _LL323;} else{ goto _LL324;} _LL324: if(( unsigned int) _temp274 > 4u?*((
int*) _temp274) == Cyc_Absyn_JoinEff: 0){ goto _LL325;} else{ goto _LL275;}
_LL277: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp328))->v;
_LL279: return Cyc_Absyn_conref_val( _temp330->kind); _LL281: return( void*) Cyc_Absyn_MemKind;
_LL283: return _temp332 ==( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL285: return( void*) Cyc_Absyn_MemKind; _LL287:
return( void*) Cyc_Absyn_MemKind; _LL289: return( void*) Cyc_Absyn_MemKind;
_LL291: return( void*) Cyc_Absyn_BoxKind; _LL293: return( void*) Cyc_Absyn_RgnKind;
_LL295: return( void*) Cyc_Absyn_BoxKind; _LL297: if( _temp338->typs == 0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL299: return Cyc_Tcutil_impos( _tag_arr("typ_kind: Unresolved TunionFieldType",
sizeof( unsigned char), 37u)); _LL301: return( void*) Cyc_Absyn_AnyKind; _LL303:
return( void*) Cyc_Absyn_AnyKind; _LL305: return( void*) Cyc_Absyn_MemKind;
_LL307: return( void*) Cyc_Absyn_MemKind; _LL309: return( void*) Cyc_Absyn_MemKind;
_LL311: return( void*) Cyc_Absyn_MemKind; _LL313: return( void*) Cyc_Absyn_BoxKind;
_LL315: { void* _temp356=( void*)( Cyc_Absyn_compress_conref( _temp348.bounds))->v;
void* _temp366; void* _temp368; _LL358: if(( unsigned int) _temp356 > 1u?*(( int*)
_temp356) == Cyc_Absyn_Eq_constr: 0){ _LL367: _temp366=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp356)->f1; if( _temp366 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL359;} else{
goto _LL360;}} else{ goto _LL360;} _LL360: if(( unsigned int) _temp356 > 1u?*((
int*) _temp356) == Cyc_Absyn_Eq_constr: 0){ _LL369: _temp368=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp356)->f1; if(( unsigned int) _temp368 > 1u?*((
int*) _temp368) == Cyc_Absyn_Upper_b: 0){ goto _LL361;} else{ goto _LL362;}}
else{ goto _LL362;} _LL362: if( _temp356 ==( void*) Cyc_Absyn_No_constr){ goto
_LL363;} else{ goto _LL364;} _LL364: if(( unsigned int) _temp356 > 1u?*(( int*)
_temp356) == Cyc_Absyn_Forward_constr: 0){ goto _LL365;} else{ goto _LL357;}
_LL359: return( void*) Cyc_Absyn_MemKind; _LL361: return( void*) Cyc_Absyn_BoxKind;
_LL363: return( void*) Cyc_Absyn_MemKind; _LL365: return Cyc_Tcutil_impos(
_tag_arr("typ_kind: forward constraint in ptr bounds", sizeof( unsigned char),
43u)); _LL357:;} _LL317: return( void*) Cyc_Absyn_MemKind; _LL319: return( void*)
Cyc_Absyn_MemKind; _LL321: return Cyc_Tcutil_impos(( struct _tagged_arr)({
struct _tagged_arr _temp370= Cyc_Absynpp_typ2string( t); xprintf("typ_kind: typedef found: %.*s",
_get_arr_size( _temp370, 1u), _temp370.curr);})); _LL323: return( void*) Cyc_Absyn_EffKind;
_LL325: return( void*) Cyc_Absyn_EffKind; _LL275:;} unsigned char Cyc_Tcutil_Unify[
10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify( void* t1, void* t2){ struct
_handler_cons _temp371; _push_handler(& _temp371);{ int _temp373= 0; if( setjmp(
_temp371.handler)){ _temp373= 1;} if( ! _temp373){ Cyc_Tcutil_unify_it( t1, t2);{
int _temp374= 1; _npop_handler( 0u); return _temp374;}; _pop_handler();} else{
void* _temp372=( void*) _exn_thrown; void* _temp376= _temp372; _LL378: if(
_temp376 == Cyc_Tcutil_Unify){ goto _LL379;} else{ goto _LL380;} _LL380: goto
_LL381; _LL379: return 0; _LL381:( void) _throw( _temp376); _LL377:;}}} static
void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, struct Cyc_List_List* env, void*
t){ void* _temp382= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp416;
struct Cyc_Core_Opt* _temp418; struct Cyc_Core_Opt** _temp420; struct Cyc_Core_Opt*
_temp421; struct Cyc_Absyn_PtrInfo _temp423; void* _temp425; struct Cyc_Absyn_FnInfo
_temp427; struct Cyc_List_List* _temp429; struct Cyc_List_List* _temp431; struct
Cyc_Absyn_VarargInfo* _temp433; int _temp435; struct Cyc_List_List* _temp437;
void* _temp439; struct Cyc_Core_Opt* _temp441; struct Cyc_List_List* _temp443;
struct Cyc_List_List* _temp445; void* _temp447; struct Cyc_Absyn_TunionInfo
_temp449; void* _temp451; struct Cyc_List_List* _temp453; struct Cyc_Core_Opt*
_temp455; struct Cyc_List_List* _temp457; struct Cyc_Absyn_TunionFieldInfo
_temp459; struct Cyc_List_List* _temp461; struct Cyc_List_List* _temp463; struct
Cyc_List_List* _temp465; struct Cyc_List_List* _temp467; void* _temp469; struct
Cyc_List_List* _temp471; _LL384: if(( unsigned int) _temp382 > 4u?*(( int*)
_temp382) == Cyc_Absyn_VarType: 0){ _LL417: _temp416=(( struct Cyc_Absyn_VarType_struct*)
_temp382)->f1; goto _LL385;} else{ goto _LL386;} _LL386: if(( unsigned int)
_temp382 > 4u?*(( int*) _temp382) == Cyc_Absyn_Evar: 0){ _LL422: _temp421=((
struct Cyc_Absyn_Evar_struct*) _temp382)->f2; goto _LL419; _LL419: _temp418=((
struct Cyc_Absyn_Evar_struct*) _temp382)->f4; _temp420=&(( struct Cyc_Absyn_Evar_struct*)
_temp382)->f4; goto _LL387;} else{ goto _LL388;} _LL388: if(( unsigned int)
_temp382 > 4u?*(( int*) _temp382) == Cyc_Absyn_PointerType: 0){ _LL424: _temp423=((
struct Cyc_Absyn_PointerType_struct*) _temp382)->f1; goto _LL389;} else{ goto
_LL390;} _LL390: if(( unsigned int) _temp382 > 4u?*(( int*) _temp382) == Cyc_Absyn_ArrayType:
0){ _LL426: _temp425=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp382)->f1;
goto _LL391;} else{ goto _LL392;} _LL392: if(( unsigned int) _temp382 > 4u?*((
int*) _temp382) == Cyc_Absyn_FnType: 0){ _LL428: _temp427=(( struct Cyc_Absyn_FnType_struct*)
_temp382)->f1; _LL444: _temp443= _temp427.tvars; goto _LL442; _LL442: _temp441=
_temp427.effect; goto _LL440; _LL440: _temp439=( void*) _temp427.ret_typ; goto
_LL438; _LL438: _temp437= _temp427.args; goto _LL436; _LL436: _temp435= _temp427.c_varargs;
goto _LL434; _LL434: _temp433= _temp427.cyc_varargs; goto _LL432; _LL432:
_temp431= _temp427.rgn_po; goto _LL430; _LL430: _temp429= _temp427.attributes;
goto _LL393;} else{ goto _LL394;} _LL394: if(( unsigned int) _temp382 > 4u?*((
int*) _temp382) == Cyc_Absyn_TupleType: 0){ _LL446: _temp445=(( struct Cyc_Absyn_TupleType_struct*)
_temp382)->f1; goto _LL395;} else{ goto _LL396;} _LL396: if(( unsigned int)
_temp382 > 4u?*(( int*) _temp382) == Cyc_Absyn_RgnHandleType: 0){ _LL448:
_temp447=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp382)->f1; goto
_LL397;} else{ goto _LL398;} _LL398: if(( unsigned int) _temp382 > 4u?*(( int*)
_temp382) == Cyc_Absyn_TunionType: 0){ _LL450: _temp449=(( struct Cyc_Absyn_TunionType_struct*)
_temp382)->f1; _LL454: _temp453= _temp449.targs; goto _LL452; _LL452: _temp451=(
void*) _temp449.rgn; goto _LL399;} else{ goto _LL400;} _LL400: if(( unsigned int)
_temp382 > 4u?*(( int*) _temp382) == Cyc_Absyn_TypedefType: 0){ _LL458: _temp457=((
struct Cyc_Absyn_TypedefType_struct*) _temp382)->f2; goto _LL456; _LL456:
_temp455=(( struct Cyc_Absyn_TypedefType_struct*) _temp382)->f3; goto _LL401;}
else{ goto _LL402;} _LL402: if(( unsigned int) _temp382 > 4u?*(( int*) _temp382)
== Cyc_Absyn_TunionFieldType: 0){ _LL460: _temp459=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp382)->f1; _LL462: _temp461= _temp459.targs; goto _LL403;} else{ goto _LL404;}
_LL404: if(( unsigned int) _temp382 > 4u?*(( int*) _temp382) == Cyc_Absyn_StructType:
0){ _LL464: _temp463=(( struct Cyc_Absyn_StructType_struct*) _temp382)->f2; goto
_LL405;} else{ goto _LL406;} _LL406: if(( unsigned int) _temp382 > 4u?*(( int*)
_temp382) == Cyc_Absyn_AnonStructType: 0){ _LL466: _temp465=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp382)->f1; goto _LL407;} else{ goto _LL408;} _LL408: if(( unsigned int)
_temp382 > 4u?*(( int*) _temp382) == Cyc_Absyn_AnonUnionType: 0){ _LL468:
_temp467=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp382)->f1; goto _LL409;}
else{ goto _LL410;} _LL410: if(( unsigned int) _temp382 > 4u?*(( int*) _temp382)
== Cyc_Absyn_AccessEff: 0){ _LL470: _temp469=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp382)->f1; goto _LL411;} else{ goto _LL412;} _LL412: if(( unsigned int)
_temp382 > 4u?*(( int*) _temp382) == Cyc_Absyn_JoinEff: 0){ _LL472: _temp471=((
struct Cyc_Absyn_JoinEff_struct*) _temp382)->f1; goto _LL413;} else{ goto _LL414;}
_LL414: goto _LL415; _LL385: if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env, _temp416)){( int) _throw(( void*) Cyc_Tcutil_Unify);}
goto _LL383; _LL387: if( t == evar){( int) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp421 != 0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp421))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp420))->v; for(
0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp473= 0;{ struct Cyc_List_List*
s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp420))->v;
for( 0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){
_temp473=({ struct Cyc_List_List* _temp474=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp474->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp474->tl= _temp473; _temp474;});}}}*
_temp420=({ struct Cyc_Core_Opt* _temp475=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp475->v=( void*) _temp473; _temp475;});}}}
goto _LL383; _LL389: Cyc_Tcutil_occurs( evar, env,( void*) _temp423.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp423.rgn_typ); goto _LL383; _LL391:
Cyc_Tcutil_occurs( evar, env, _temp425); goto _LL383; _LL393: if( _temp441 != 0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp441))->v);} Cyc_Tcutil_occurs( evar, env, _temp439); for( 0; _temp437 != 0;
_temp437=(( struct Cyc_List_List*) _check_null( _temp437))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp437))->hd)).f3);}
if( _temp433 != 0){ void* _temp478; void* _temp480; struct Cyc_Absyn_VarargInfo
_temp476=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp433)); _LL481:
_temp480=( void*) _temp476.type; goto _LL479; _LL479: _temp478=( void*) _temp476.rgn;
goto _LL477; _LL477: Cyc_Tcutil_occurs( evar, env, _temp480); Cyc_Tcutil_occurs(
evar, env, _temp478);} for( 0; _temp431 != 0; _temp431=(( struct Cyc_List_List*)
_check_null( _temp431))->tl){ struct _tuple8 _temp484; void* _temp485; void*
_temp487; struct _tuple8* _temp482=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp431))->hd; _temp484=* _temp482; _LL488: _temp487= _temp484.f1;
goto _LL486; _LL486: _temp485= _temp484.f2; goto _LL483; _LL483: Cyc_Tcutil_occurs(
evar, env, _temp487); Cyc_Tcutil_occurs( evar, env, _temp485);} goto _LL383;
_LL395: for( 0; _temp445 != 0; _temp445=(( struct Cyc_List_List*) _check_null(
_temp445))->tl){ Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp445))->hd)).f2);} goto _LL383; _LL397: Cyc_Tcutil_occurs( evar,
env, _temp447); goto _LL383; _LL399: Cyc_Tcutil_occurs( evar, env, _temp451);
Cyc_Tcutil_occurslist( evar, env, _temp453); goto _LL383; _LL401: _temp461=
_temp457; goto _LL403; _LL403: _temp463= _temp461; goto _LL405; _LL405: Cyc_Tcutil_occurslist(
evar, env, _temp463); goto _LL383; _LL407: _temp467= _temp465; goto _LL409;
_LL409: for( 0; _temp467 != 0; _temp467=(( struct Cyc_List_List*) _check_null(
_temp467))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp467))->hd)->type);} goto _LL383; _LL411:
Cyc_Tcutil_occurs( evar, env, _temp469); goto _LL383; _LL413: Cyc_Tcutil_occurslist(
evar, env, _temp471); goto _LL383; _LL415: goto _LL383; _LL383:;} static void
Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts){ for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs(
evar, env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static
void Cyc_Tcutil_unify_list( struct Cyc_List_List* t1, struct Cyc_List_List* t2){
struct _handler_cons _temp489; _push_handler(& _temp489);{ int _temp491= 0; if(
setjmp( _temp489.handler)){ _temp491= 1;} if( ! _temp491){ Cyc_List_iter2( Cyc_Tcutil_unify_it,
t1, t2);; _pop_handler();} else{ void* _temp490=( void*) _exn_thrown; void*
_temp493= _temp490; _LL495: if( _temp493 == Cyc_List_List_mismatch){ goto _LL496;}
else{ goto _LL497;} _LL497: goto _LL498; _LL496:( int) _throw(( void*) Cyc_Tcutil_Unify);
_LL498:( void) _throw( _temp493); _LL494:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != tq2.q_const?
1: tq1.q_volatile != tq2.q_volatile)? 1: tq1.q_restrict != tq2.q_restrict){( int)
_throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile
== tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x= Cyc_Absyn_compress_conref( x); y= Cyc_Absyn_compress_conref( y); if( x ==
y){ return;}{ void* _temp499=( void*) x->v; void* _temp507; _LL501: if( _temp499
==( void*) Cyc_Absyn_No_constr){ goto _LL502;} else{ goto _LL503;} _LL503: if((
unsigned int) _temp499 > 1u?*(( int*) _temp499) == Cyc_Absyn_Forward_constr: 0){
goto _LL504;} else{ goto _LL505;} _LL505: if(( unsigned int) _temp499 > 1u?*((
int*) _temp499) == Cyc_Absyn_Eq_constr: 0){ _LL508: _temp507=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp499)->f1; goto _LL506;} else{ goto _LL500;}
_LL502:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp509=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp509[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp510; _temp510.tag= Cyc_Absyn_Forward_constr; _temp510.f1= y; _temp510;});
_temp509;}))); return; _LL504:( int) _throw( Cyc_Tcutil_impos( _tag_arr("unify_conref: forward after compress",
sizeof( unsigned char), 37u))); _LL506: { void* _temp511=( void*) y->v; void*
_temp519; _LL513: if( _temp511 ==( void*) Cyc_Absyn_No_constr){ goto _LL514;}
else{ goto _LL515;} _LL515: if(( unsigned int) _temp511 > 1u?*(( int*) _temp511)
== Cyc_Absyn_Forward_constr: 0){ goto _LL516;} else{ goto _LL517;} _LL517: if((
unsigned int) _temp511 > 1u?*(( int*) _temp511) == Cyc_Absyn_Eq_constr: 0){
_LL520: _temp519=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp511)->f1;
goto _LL518;} else{ goto _LL512;} _LL514:( void*)( y->v=( void*)(( void*) x->v));
return; _LL516:( int) _throw( Cyc_Tcutil_impos( _tag_arr("unify_conref: forward after compress(2)",
sizeof( unsigned char), 40u))); _LL518: if( cmp( _temp507, _temp519) != 0){( int)
_throw(( void*) Cyc_Tcutil_Unify);} return; _LL512:;} _LL500:;}} static int Cyc_Tcutil_unify_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ struct _handler_cons _temp521; _push_handler(& _temp521);{ int _temp523= 0;
if( setjmp( _temp521.handler)){ _temp523= 1;} if( ! _temp523){ Cyc_Tcutil_unify_it_conrefs(
cmp, x, y);{ int _temp524= 1; _npop_handler( 0u); return _temp524;};
_pop_handler();} else{ void* _temp522=( void*) _exn_thrown; void* _temp526=
_temp522; _LL528: if( _temp526 == Cyc_Tcutil_Unify){ goto _LL529;} else{ goto
_LL530;} _LL530: goto _LL531; _LL529: return 0; _LL531:( void) _throw( _temp526);
_LL527:;}}} static int Cyc_Tcutil_boundscmp( void* b1, void* b2){ struct _tuple8
_temp533=({ struct _tuple8 _temp532; _temp532.f1= b1; _temp532.f2= b2; _temp532;});
void* _temp543; void* _temp545; void* _temp547; void* _temp549; void* _temp551;
struct Cyc_Absyn_Exp* _temp553; void* _temp555; struct Cyc_Absyn_Exp* _temp557;
_LL535: _LL546: _temp545= _temp533.f1; if( _temp545 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL544;} else{ goto _LL537;} _LL544: _temp543= _temp533.f2; if( _temp543 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL536;} else{ goto _LL537;} _LL537: _LL548:
_temp547= _temp533.f1; if( _temp547 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL538;}
else{ goto _LL539;} _LL539: _LL550: _temp549= _temp533.f2; if( _temp549 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL540;} else{ goto _LL541;} _LL541: _LL556: _temp555=
_temp533.f1; if(( unsigned int) _temp555 > 1u?*(( int*) _temp555) == Cyc_Absyn_Upper_b:
0){ _LL558: _temp557=(( struct Cyc_Absyn_Upper_b_struct*) _temp555)->f1; goto
_LL552;} else{ goto _LL534;} _LL552: _temp551= _temp533.f2; if(( unsigned int)
_temp551 > 1u?*(( int*) _temp551) == Cyc_Absyn_Upper_b: 0){ _LL554: _temp553=((
struct Cyc_Absyn_Upper_b_struct*) _temp551)->f1; goto _LL542;} else{ goto _LL534;}
_LL536: return 0; _LL538: return - 1; _LL540: return 1; _LL542: { int i1=( int)
Cyc_Evexp_eval_const_uint_exp( _temp557); int i2=( int) Cyc_Evexp_eval_const_uint_exp(
_temp553); if( i1 == i2){ return 0;} if( i1 < i2){ return - 1;} return 1;}
_LL534:;} static int Cyc_Tcutil_equal_att( void* a1, void* a2){ if( a1 == a2){
return 1;}{ struct _tuple8 _temp560=({ struct _tuple8 _temp559; _temp559.f1= a1;
_temp559.f2= a2; _temp559;}); void* _temp570; int _temp572; void* _temp574; int
_temp576; void* _temp578; int _temp580; void* _temp582; int _temp584; void*
_temp586; struct _tagged_arr _temp588; void* _temp590; struct _tagged_arr
_temp592; _LL562: _LL575: _temp574= _temp560.f1; if(( unsigned int) _temp574 >
16u?*(( int*) _temp574) == Cyc_Absyn_Regparm_att: 0){ _LL577: _temp576=(( struct
Cyc_Absyn_Regparm_att_struct*) _temp574)->f1; goto _LL571;} else{ goto _LL564;}
_LL571: _temp570= _temp560.f2; if(( unsigned int) _temp570 > 16u?*(( int*)
_temp570) == Cyc_Absyn_Regparm_att: 0){ _LL573: _temp572=(( struct Cyc_Absyn_Regparm_att_struct*)
_temp570)->f1; goto _LL563;} else{ goto _LL564;} _LL564: _LL583: _temp582=
_temp560.f1; if(( unsigned int) _temp582 > 16u?*(( int*) _temp582) == Cyc_Absyn_Aligned_att:
0){ _LL585: _temp584=(( struct Cyc_Absyn_Aligned_att_struct*) _temp582)->f1;
goto _LL579;} else{ goto _LL566;} _LL579: _temp578= _temp560.f2; if((
unsigned int) _temp578 > 16u?*(( int*) _temp578) == Cyc_Absyn_Aligned_att: 0){
_LL581: _temp580=(( struct Cyc_Absyn_Aligned_att_struct*) _temp578)->f1; goto
_LL565;} else{ goto _LL566;} _LL566: _LL591: _temp590= _temp560.f1; if((
unsigned int) _temp590 > 16u?*(( int*) _temp590) == Cyc_Absyn_Section_att: 0){
_LL593: _temp592=(( struct Cyc_Absyn_Section_att_struct*) _temp590)->f1; goto
_LL587;} else{ goto _LL568;} _LL587: _temp586= _temp560.f2; if(( unsigned int)
_temp586 > 16u?*(( int*) _temp586) == Cyc_Absyn_Section_att: 0){ _LL589:
_temp588=(( struct Cyc_Absyn_Section_att_struct*) _temp586)->f1; goto _LL567;}
else{ goto _LL568;} _LL568: goto _LL569; _LL563: _temp584= _temp576; _temp580=
_temp572; goto _LL565; _LL565: return _temp584 == _temp580; _LL567: return Cyc_String_strcmp(
_temp592, _temp588) == 0; _LL569: return 0; _LL561:;}} int Cyc_Tcutil_same_atts(
struct Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a=
a1; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( ! Cyc_List_exists_c(
Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){
return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a1)){ return 0;}}} return 1;}
static void Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp594= e; struct Cyc_List_List* _temp600; struct Cyc_List_List**
_temp602; _LL596: if(( unsigned int) _temp594 > 4u?*(( int*) _temp594) == Cyc_Absyn_JoinEff:
0){ _LL601: _temp600=(( struct Cyc_Absyn_JoinEff_struct*) _temp594)->f1;
_temp602=&(( struct Cyc_Absyn_JoinEff_struct*) _temp594)->f1; goto _LL597;}
else{ goto _LL598;} _LL598: goto _LL599; _LL597: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp602; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp603=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; Cyc_Tcutil_normalize_effect( _temp603);{ void* _temp604=
Cyc_Tcutil_compress( _temp603); _LL606: if(( unsigned int) _temp604 > 4u?*(( int*)
_temp604) == Cyc_Absyn_JoinEff: 0){ goto _LL607;} else{ goto _LL608;} _LL608:
goto _LL609; _LL607: nested_join= 1; goto _LL605; _LL609: goto _LL605; _LL605:;}}}
if( ! nested_join){ return;}{ struct Cyc_List_List* effects= 0;{ struct Cyc_List_List*
effs=* _temp602; for( 0; effs != 0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp610= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd); struct Cyc_List_List* _temp616; _LL612: if((
unsigned int) _temp610 > 4u?*(( int*) _temp610) == Cyc_Absyn_JoinEff: 0){ _LL617:
_temp616=(( struct Cyc_Absyn_JoinEff_struct*) _temp610)->f1; goto _LL613;} else{
goto _LL614;} _LL614: goto _LL615; _LL613: effects= Cyc_List_revappend( _temp616,
effects); goto _LL611; _LL615: effects=({ struct Cyc_List_List* _temp618=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp618->hd=(
void*) _temp610; _temp618->tl= effects; _temp618;}); goto _LL611; _LL611:;}}*
_temp602= Cyc_List_imp_rev( effects); goto _LL595;}} _LL599: goto _LL595; _LL595:;}}
struct _tuple9{ void* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; }
; static struct _tuple9* Cyc_Tcutil_get_effect_evar( void* t){ void* _temp619=
Cyc_Tcutil_compress( t); struct Cyc_List_List* _temp627; struct Cyc_List_List
_temp629; struct Cyc_List_List* _temp630; void* _temp632; struct Cyc_Core_Opt*
_temp634; struct Cyc_Core_Opt* _temp636; _LL621: if(( unsigned int) _temp619 > 4u?*((
int*) _temp619) == Cyc_Absyn_JoinEff: 0){ _LL628: _temp627=(( struct Cyc_Absyn_JoinEff_struct*)
_temp619)->f1; if( _temp627 == 0){ goto _LL623;} else{ _temp629=* _temp627;
_LL633: _temp632=( void*) _temp629.hd; goto _LL631; _LL631: _temp630= _temp629.tl;
goto _LL622;}} else{ goto _LL623;} _LL623: if(( unsigned int) _temp619 > 4u?*((
int*) _temp619) == Cyc_Absyn_Evar: 0){ _LL637: _temp636=(( struct Cyc_Absyn_Evar_struct*)
_temp619)->f1; goto _LL635; _LL635: _temp634=(( struct Cyc_Absyn_Evar_struct*)
_temp619)->f4; goto _LL624;} else{ goto _LL625;} _LL625: goto _LL626; _LL622: {
void* _temp638= Cyc_Tcutil_compress( _temp632); struct Cyc_Core_Opt* _temp644;
_LL640: if(( unsigned int) _temp638 > 4u?*(( int*) _temp638) == Cyc_Absyn_Evar:
0){ _LL645: _temp644=(( struct Cyc_Absyn_Evar_struct*) _temp638)->f4; goto
_LL641;} else{ goto _LL642;} _LL642: goto _LL643; _LL641: return({ struct
_tuple9* _temp646=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp646->f1= _temp632; _temp646->f2= _temp630; _temp646->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp644))->v; _temp646;}); _LL643: return 0;
_LL639:;} _LL624: if( _temp636 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp636))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));} return({ struct _tuple9* _temp647=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp647->f1= t; _temp647->f2= 0;
_temp647->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp634))->v; _temp647;}); _LL626: return 0; _LL620:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; int Cyc_Tcutil_region_in_effect( int
constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*) Cyc_Absyn_HeapRgn){
return 1;}{ void* _temp648= Cyc_Tcutil_compress( e); void* _temp658; struct Cyc_List_List*
_temp660; struct Cyc_Core_Opt* _temp662; struct Cyc_Core_Opt* _temp664; struct
Cyc_Core_Opt** _temp666; struct Cyc_Core_Opt* _temp667; _LL650: if((
unsigned int) _temp648 > 4u?*(( int*) _temp648) == Cyc_Absyn_AccessEff: 0){
_LL659: _temp658=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp648)->f1;
goto _LL651;} else{ goto _LL652;} _LL652: if(( unsigned int) _temp648 > 4u?*((
int*) _temp648) == Cyc_Absyn_JoinEff: 0){ _LL661: _temp660=(( struct Cyc_Absyn_JoinEff_struct*)
_temp648)->f1; goto _LL653;} else{ goto _LL654;} _LL654: if(( unsigned int)
_temp648 > 4u?*(( int*) _temp648) == Cyc_Absyn_Evar: 0){ _LL668: _temp667=((
struct Cyc_Absyn_Evar_struct*) _temp648)->f1; goto _LL665; _LL665: _temp664=((
struct Cyc_Absyn_Evar_struct*) _temp648)->f2; _temp666=&(( struct Cyc_Absyn_Evar_struct*)
_temp648)->f2; goto _LL663; _LL663: _temp662=(( struct Cyc_Absyn_Evar_struct*)
_temp648)->f4; goto _LL655;} else{ goto _LL656;} _LL656: goto _LL657; _LL651:
if( constrain){ return Cyc_Tcutil_unify( r, _temp658);} Cyc_Tcutil_compress(
_temp658); if( r == _temp658){ return 1;}{ struct _tuple8 _temp670=({ struct
_tuple8 _temp669; _temp669.f1= r; _temp669.f2= _temp658; _temp669;}); void*
_temp676; struct Cyc_Absyn_Tvar* _temp678; void* _temp680; struct Cyc_Absyn_Tvar*
_temp682; _LL672: _LL681: _temp680= _temp670.f1; if(( unsigned int) _temp680 > 4u?*((
int*) _temp680) == Cyc_Absyn_VarType: 0){ _LL683: _temp682=(( struct Cyc_Absyn_VarType_struct*)
_temp680)->f1; goto _LL677;} else{ goto _LL674;} _LL677: _temp676= _temp670.f2;
if(( unsigned int) _temp676 > 4u?*(( int*) _temp676) == Cyc_Absyn_VarType: 0){
_LL679: _temp678=(( struct Cyc_Absyn_VarType_struct*) _temp676)->f1; goto _LL673;}
else{ goto _LL674;} _LL674: goto _LL675; _LL673: return Cyc_Absyn_tvar_cmp(
_temp682, _temp678) == 0; _LL675: return 0; _LL671:;} _LL653: for( 0; _temp660
!= 0; _temp660=(( struct Cyc_List_List*) _check_null( _temp660))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp660))->hd)){
return 1;}} return 0; _LL655: if( _temp667 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp667))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));} if( ! constrain){ return 0;}{ void* _temp684=
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp662); Cyc_Tcutil_occurs(
_temp684,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp662))->v,
r);{ struct Cyc_Absyn_JoinEff_struct* _temp685=({ struct Cyc_Absyn_JoinEff_struct*
_temp687=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp687[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp688; _temp688.tag= Cyc_Absyn_JoinEff;
_temp688.f1=({ struct Cyc_List_List* _temp689=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp689->hd=( void*) _temp684; _temp689->tl=({
struct Cyc_List_List* _temp690=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp690->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp691=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp691[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp692; _temp692.tag= Cyc_Absyn_AccessEff;
_temp692.f1=( void*) r; _temp692;}); _temp691;})); _temp690->tl= 0; _temp690;});
_temp689;}); _temp688;}); _temp687;});* _temp666=({ struct Cyc_Core_Opt*
_temp686=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp686->v=( void*)(( void*) _temp685); _temp686;}); return 1;}} _LL657: return
0; _LL649:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp693= e;
struct Cyc_Absyn_Tvar* _temp703; struct Cyc_List_List* _temp705; struct Cyc_Core_Opt*
_temp707; struct Cyc_Core_Opt* _temp709; struct Cyc_Core_Opt** _temp711; struct
Cyc_Core_Opt* _temp712; _LL695: if(( unsigned int) _temp693 > 4u?*(( int*)
_temp693) == Cyc_Absyn_VarType: 0){ _LL704: _temp703=(( struct Cyc_Absyn_VarType_struct*)
_temp693)->f1; goto _LL696;} else{ goto _LL697;} _LL697: if(( unsigned int)
_temp693 > 4u?*(( int*) _temp693) == Cyc_Absyn_JoinEff: 0){ _LL706: _temp705=((
struct Cyc_Absyn_JoinEff_struct*) _temp693)->f1; goto _LL698;} else{ goto _LL699;}
_LL699: if(( unsigned int) _temp693 > 4u?*(( int*) _temp693) == Cyc_Absyn_Evar:
0){ _LL713: _temp712=(( struct Cyc_Absyn_Evar_struct*) _temp693)->f1; goto
_LL710; _LL710: _temp709=(( struct Cyc_Absyn_Evar_struct*) _temp693)->f2;
_temp711=&(( struct Cyc_Absyn_Evar_struct*) _temp693)->f2; goto _LL708; _LL708:
_temp707=(( struct Cyc_Absyn_Evar_struct*) _temp693)->f4; goto _LL700;} else{
goto _LL701;} _LL701: goto _LL702; _LL696: return Cyc_Absyn_tvar_cmp( v,
_temp703) == 0; _LL698: for( 0; _temp705 != 0; _temp705=(( struct Cyc_List_List*)
_check_null( _temp705))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp705))->hd)){ return 1;}}
return 0; _LL700: if( _temp712 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp712))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));}{ void* _temp714= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp707); if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp707))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp715=({ struct Cyc_Absyn_JoinEff_struct* _temp717=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp717[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp718; _temp718.tag= Cyc_Absyn_JoinEff; _temp718.f1=({ struct Cyc_List_List*
_temp719=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp719->hd=( void*) _temp714; _temp719->tl=({ struct Cyc_List_List* _temp720=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp720->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp721=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp721[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp722; _temp722.tag= Cyc_Absyn_VarType; _temp722.f1= v; _temp722;}); _temp721;}));
_temp720->tl= 0; _temp720;}); _temp719;}); _temp718;}); _temp717;});* _temp711=({
struct Cyc_Core_Opt* _temp716=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp716->v=( void*)(( void*) _temp715); _temp716;}); return 1;}}
_LL702: return 0; _LL694:;}} static int Cyc_Tcutil_evar_in_effect( void* evar,
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp723= e; struct Cyc_List_List*
_temp731; _LL725: if(( unsigned int) _temp723 > 4u?*(( int*) _temp723) == Cyc_Absyn_JoinEff:
0){ _LL732: _temp731=(( struct Cyc_Absyn_JoinEff_struct*) _temp723)->f1; goto
_LL726;} else{ goto _LL727;} _LL727: if(( unsigned int) _temp723 > 4u?*(( int*)
_temp723) == Cyc_Absyn_Evar: 0){ goto _LL728;} else{ goto _LL729;} _LL729: goto
_LL730; _LL726: for( 0; _temp731 != 0; _temp731=(( struct Cyc_List_List*)
_check_null( _temp731))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp731))->hd)){ return 1;}} return 0;
_LL728: return evar == e; _LL730: return 0; _LL724:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){{ struct _tuple9* _temp733= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple9 _temp739; struct Cyc_List_List* _temp740; struct Cyc_List_List*
_temp742; void* _temp744; _LL735: if( _temp733 == 0){ goto _LL737;} else{
_temp739=* _temp733; _LL745: _temp744= _temp739.f1; goto _LL743; _LL743:
_temp742= _temp739.f2; goto _LL741; _LL741: _temp740= _temp739.f3; goto _LL736;}
_LL737: goto _LL738; _LL736: { void* _temp746= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp752=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp752->v=( void*) _temp740; _temp752;}));
struct Cyc_Absyn_JoinEff_struct* _temp747=({ struct Cyc_Absyn_JoinEff_struct*
_temp748=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp748[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp749; _temp749.tag= Cyc_Absyn_JoinEff;
_temp749.f1=({ struct Cyc_List_List* _temp750=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp750->hd=( void*) _temp746; _temp750->tl=({
struct Cyc_List_List* _temp751=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp751->hd=( void*) e1; _temp751->tl= 0; _temp751;});
_temp750;}); _temp749;}); _temp748;}); return Cyc_Tcutil_unify( _temp744,( void*)
_temp747);} _LL738: goto _LL734; _LL734:;}{ void* _temp753= Cyc_Tcutil_compress(
e1); struct Cyc_List_List* _temp765; void* _temp767; struct Cyc_Absyn_Tvar*
_temp769; struct Cyc_Core_Opt* _temp771; struct Cyc_Core_Opt* _temp773; struct
Cyc_Core_Opt** _temp775; _LL755: if(( unsigned int) _temp753 > 4u?*(( int*)
_temp753) == Cyc_Absyn_JoinEff: 0){ _LL766: _temp765=(( struct Cyc_Absyn_JoinEff_struct*)
_temp753)->f1; goto _LL756;} else{ goto _LL757;} _LL757: if(( unsigned int)
_temp753 > 4u?*(( int*) _temp753) == Cyc_Absyn_AccessEff: 0){ _LL768: _temp767=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp753)->f1; goto _LL758;} else{
goto _LL759;} _LL759: if(( unsigned int) _temp753 > 4u?*(( int*) _temp753) ==
Cyc_Absyn_VarType: 0){ _LL770: _temp769=(( struct Cyc_Absyn_VarType_struct*)
_temp753)->f1; goto _LL760;} else{ goto _LL761;} _LL761: if(( unsigned int)
_temp753 > 4u?*(( int*) _temp753) == Cyc_Absyn_Evar: 0){ _LL774: _temp773=((
struct Cyc_Absyn_Evar_struct*) _temp753)->f2; _temp775=&(( struct Cyc_Absyn_Evar_struct*)
_temp753)->f2; goto _LL772; _LL772: _temp771=(( struct Cyc_Absyn_Evar_struct*)
_temp753)->f4; goto _LL762;} else{ goto _LL763;} _LL763: goto _LL764; _LL756:
for( 0; _temp765 != 0; _temp765=(( struct Cyc_List_List*) _check_null( _temp765))->tl){
if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*)(( struct Cyc_List_List*)
_check_null( _temp765))->hd, e2)){ return 0;}} return 1; _LL758: return( Cyc_Tcutil_region_in_effect(
0, _temp767, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp767, e2))? 1: Cyc_Tcutil_unify(
_temp767,( void*) Cyc_Absyn_HeapRgn); _LL760: return Cyc_Tcutil_variable_in_effect(
0, _temp769, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp769, e2); _LL762:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp775=({
struct Cyc_Core_Opt* _temp776=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp776->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp777=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp777[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp778; _temp778.tag= Cyc_Absyn_JoinEff;
_temp778.f1= 0; _temp778;}); _temp777;})); _temp776;});} else{ Cyc_Tcutil_occurs(
e1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp771))->v,
e2);* _temp775=({ struct Cyc_Core_Opt* _temp779=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp779->v=( void*) e2; _temp779;});}}
return 1; _LL764: return(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp780= Cyc_Absynpp_typ2string( e1);
xprintf("subset_effect: bad effect: %.*s", _get_arr_size( _temp780, 1u),
_temp780.curr);})); _LL754:;}} struct _tuple10{ struct _tuple9* f1; struct
_tuple9* f2; } ; static int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1=
Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2); Cyc_Tcutil_normalize_effect(
e1); Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp782=({ struct
_tuple10 _temp781; _temp781.f1= Cyc_Tcutil_get_effect_evar( e1); _temp781.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp781;}); struct _tuple9* _temp788; struct
_tuple9 _temp790; struct Cyc_List_List* _temp791; struct Cyc_List_List* _temp793;
void* _temp795; struct _tuple9* _temp797; struct _tuple9 _temp799; struct Cyc_List_List*
_temp800; struct Cyc_List_List* _temp802; void* _temp804; _LL784: _LL798:
_temp797= _temp782.f1; if( _temp797 == 0){ goto _LL786;} else{ _temp799=*
_temp797; _LL805: _temp804= _temp799.f1; goto _LL803; _LL803: _temp802= _temp799.f2;
goto _LL801; _LL801: _temp800= _temp799.f3; goto _LL789;} _LL789: _temp788=
_temp782.f2; if( _temp788 == 0){ goto _LL786;} else{ _temp790=* _temp788; _LL796:
_temp795= _temp790.f1; goto _LL794; _LL794: _temp793= _temp790.f2; goto _LL792;
_LL792: _temp791= _temp790.f3; goto _LL785;} _LL786: goto _LL787; _LL785: {
struct Cyc_List_List* _temp806= 0; for( 0; _temp800 != 0; _temp800=(( struct Cyc_List_List*)
_check_null( _temp800))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp791,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp800))->hd)){ _temp806=({ struct Cyc_List_List* _temp807=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp807->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp800))->hd);
_temp807->tl= _temp806; _temp807;});}}{ void* _temp808= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp813=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp813->v=( void*)
_temp806; _temp813;})); struct Cyc_Absyn_JoinEff_struct* _temp809=({ struct Cyc_Absyn_JoinEff_struct*
_temp810=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp810[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp811; _temp811.tag= Cyc_Absyn_JoinEff;
_temp811.f1=({ struct Cyc_List_List* _temp812=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp812->hd=( void*) _temp808; _temp812->tl=
Cyc_List_append( _temp802, _temp793); _temp812;}); _temp811;}); _temp810;});
return Cyc_Tcutil_unify( _temp804,( void*) _temp809)? Cyc_Tcutil_unify( _temp795,(
void*) _temp809): 0;}} _LL787: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect(
0, e2, e1): 0; _LL783:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1
!= 0; r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8
_temp816; void* _temp817; void* _temp819; struct _tuple8* _temp814=( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( r1))->hd; _temp816=* _temp814;
_LL820: _temp819= _temp816.f1; goto _LL818; _LL818: _temp817= _temp816.f2; goto
_LL815; _LL815: { int found= _temp819 ==( void*) Cyc_Absyn_HeapRgn;{ struct Cyc_List_List*
r2= rpo2; for( 0; r2 != 0? ! found: 0; r2=(( struct Cyc_List_List*) _check_null(
r2))->tl){ struct _tuple8 _temp823; void* _temp824; void* _temp826; struct
_tuple8* _temp821=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( r2))->hd;
_temp823=* _temp821; _LL827: _temp826= _temp823.f1; goto _LL825; _LL825:
_temp824= _temp823.f2; goto _LL822; _LL822: if( Cyc_Tcutil_unify( _temp819,
_temp826)? Cyc_Tcutil_unify( _temp817, _temp824): 0){ found= 1; break;}}} if( !
found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo( rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo(
rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo* f1; struct Cyc_Absyn_VarargInfo*
f2; } ; struct _tuple12{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp828= t1; struct
Cyc_Core_Opt* _temp834; struct Cyc_Core_Opt* _temp836; struct Cyc_Core_Opt**
_temp838; struct Cyc_Core_Opt* _temp839; _LL830: if(( unsigned int) _temp828 > 4u?*((
int*) _temp828) == Cyc_Absyn_Evar: 0){ _LL840: _temp839=(( struct Cyc_Absyn_Evar_struct*)
_temp828)->f1; goto _LL837; _LL837: _temp836=(( struct Cyc_Absyn_Evar_struct*)
_temp828)->f2; _temp838=&(( struct Cyc_Absyn_Evar_struct*) _temp828)->f2; goto
_LL835; _LL835: _temp834=(( struct Cyc_Absyn_Evar_struct*) _temp828)->f4; goto
_LL831;} else{ goto _LL832;} _LL832: goto _LL833; _LL831: Cyc_Tcutil_occurs( t1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp834))->v, t2);{
void* _temp841= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq( _temp841,(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp839))->v)){* _temp838=({ struct
Cyc_Core_Opt* _temp842=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp842->v=( void*) t2; _temp842;}); return;} else{{ void* _temp843= t2; struct
Cyc_Core_Opt* _temp851; struct Cyc_Core_Opt* _temp853; struct Cyc_Core_Opt**
_temp855; struct Cyc_Absyn_PtrInfo _temp857; _LL845: if(( unsigned int) _temp843
> 4u?*(( int*) _temp843) == Cyc_Absyn_Evar: 0){ _LL854: _temp853=(( struct Cyc_Absyn_Evar_struct*)
_temp843)->f2; _temp855=&(( struct Cyc_Absyn_Evar_struct*) _temp843)->f2; goto
_LL852; _LL852: _temp851=(( struct Cyc_Absyn_Evar_struct*) _temp843)->f4; goto
_LL846;} else{ goto _LL847;} _LL847: if(( unsigned int) _temp843 > 4u?*(( int*)
_temp843) == Cyc_Absyn_PointerType: 0){ _LL858: _temp857=(( struct Cyc_Absyn_PointerType_struct*)
_temp843)->f1; goto _LL856;} else{ goto _LL849;} _LL856: if(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp839))->v ==( void*) Cyc_Absyn_BoxKind){ goto
_LL848;} else{ goto _LL849;} _LL849: goto _LL850; _LL846: { struct Cyc_List_List*
_temp859=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp834))->v;{
struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp851))->v; for( 0; s2 != 0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp859,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp839))->v, _temp841)){* _temp855=({
struct Cyc_Core_Opt* _temp860=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp860->v=( void*) t1; _temp860;}); return;} goto _LL844;}
_LL848: { struct Cyc_Absyn_Conref* _temp861= Cyc_Absyn_compress_conref( _temp857.bounds);{
void* _temp862=( void*) _temp861->v; _LL864: if( _temp862 ==( void*) Cyc_Absyn_No_constr){
goto _LL865;} else{ goto _LL866;} _LL866: goto _LL867; _LL865:( void*)( _temp861->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp868=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp868[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp869; _temp869.tag= Cyc_Absyn_Eq_constr; _temp869.f1=(
void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp870=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp870[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp871; _temp871.tag= Cyc_Absyn_Upper_b; _temp871.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp871;}); _temp870;})); _temp869;}); _temp868;})));* _temp838=({
struct Cyc_Core_Opt* _temp872=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp872->v=( void*) t2; _temp872;}); return; _LL867: goto
_LL863; _LL863:;} goto _LL844;} _LL850: goto _LL844; _LL844:;}( int) _throw((
void*) Cyc_Tcutil_Unify);}} _LL833: goto _LL829; _LL829:;}{ struct _tuple8
_temp874=({ struct _tuple8 _temp873; _temp873.f1= t2; _temp873.f2= t1; _temp873;});
void* _temp924; void* _temp926; void* _temp928; void* _temp930; struct Cyc_Absyn_Tvar*
_temp932; void* _temp934; struct Cyc_Absyn_Tvar* _temp936; void* _temp938;
struct Cyc_Absyn_Structdecl** _temp940; struct Cyc_List_List* _temp942; struct
_tuple1* _temp944; void* _temp946; struct Cyc_Absyn_Structdecl** _temp948;
struct Cyc_List_List* _temp950; struct _tuple1* _temp952; void* _temp954; struct
_tuple1* _temp956; void* _temp958; struct _tuple1* _temp960; void* _temp962;
struct Cyc_Absyn_Uniondecl** _temp964; struct Cyc_List_List* _temp966; struct
_tuple1* _temp968; void* _temp970; struct Cyc_Absyn_Uniondecl** _temp972; struct
Cyc_List_List* _temp974; struct _tuple1* _temp976; void* _temp978; struct Cyc_Absyn_TunionInfo
_temp980; void* _temp982; struct Cyc_List_List* _temp984; void* _temp986; struct
Cyc_Absyn_Tuniondecl* _temp988; void* _temp990; struct Cyc_Absyn_TunionInfo
_temp992; void* _temp994; struct Cyc_List_List* _temp996; void* _temp998; struct
Cyc_Absyn_Tuniondecl* _temp1000; void* _temp1002; struct Cyc_Absyn_TunionFieldInfo
_temp1004; struct Cyc_List_List* _temp1006; void* _temp1008; struct Cyc_Absyn_Tunionfield*
_temp1010; struct Cyc_Absyn_Tuniondecl* _temp1012; void* _temp1014; struct Cyc_Absyn_TunionFieldInfo
_temp1016; struct Cyc_List_List* _temp1018; void* _temp1020; struct Cyc_Absyn_Tunionfield*
_temp1022; struct Cyc_Absyn_Tuniondecl* _temp1024; void* _temp1026; struct Cyc_Absyn_PtrInfo
_temp1028; struct Cyc_Absyn_Conref* _temp1030; struct Cyc_Absyn_Tqual _temp1032;
struct Cyc_Absyn_Conref* _temp1034; void* _temp1036; void* _temp1038; void*
_temp1040; struct Cyc_Absyn_PtrInfo _temp1042; struct Cyc_Absyn_Conref*
_temp1044; struct Cyc_Absyn_Tqual _temp1046; struct Cyc_Absyn_Conref* _temp1048;
void* _temp1050; void* _temp1052; void* _temp1054; void* _temp1056; void*
_temp1058; void* _temp1060; void* _temp1062; void* _temp1064; void* _temp1066;
void* _temp1068; void* _temp1070; void* _temp1072; void* _temp1074; struct Cyc_Absyn_Exp*
_temp1076; struct Cyc_Absyn_Tqual _temp1078; void* _temp1080; void* _temp1082;
struct Cyc_Absyn_Exp* _temp1084; struct Cyc_Absyn_Tqual _temp1086; void*
_temp1088; void* _temp1090; struct Cyc_Absyn_FnInfo _temp1092; struct Cyc_List_List*
_temp1094; struct Cyc_List_List* _temp1096; struct Cyc_Absyn_VarargInfo*
_temp1098; int _temp1100; struct Cyc_List_List* _temp1102; void* _temp1104;
struct Cyc_Core_Opt* _temp1106; struct Cyc_List_List* _temp1108; void* _temp1110;
struct Cyc_Absyn_FnInfo _temp1112; struct Cyc_List_List* _temp1114; struct Cyc_List_List*
_temp1116; struct Cyc_Absyn_VarargInfo* _temp1118; int _temp1120; struct Cyc_List_List*
_temp1122; void* _temp1124; struct Cyc_Core_Opt* _temp1126; struct Cyc_List_List*
_temp1128; void* _temp1130; struct Cyc_List_List* _temp1132; void* _temp1134;
struct Cyc_List_List* _temp1136; void* _temp1138; struct Cyc_List_List*
_temp1140; void* _temp1142; struct Cyc_List_List* _temp1144; void* _temp1146;
struct Cyc_List_List* _temp1148; void* _temp1150; struct Cyc_List_List*
_temp1152; void* _temp1154; void* _temp1156; void* _temp1158; void* _temp1160;
void* _temp1162; void* _temp1164; void* _temp1166; void* _temp1168; void*
_temp1170; void* _temp1172; _LL876: _LL925: _temp924= _temp874.f1; if((
unsigned int) _temp924 > 4u?*(( int*) _temp924) == Cyc_Absyn_Evar: 0){ goto
_LL877;} else{ goto _LL878;} _LL878: _LL929: _temp928= _temp874.f1; if( _temp928
==( void*) Cyc_Absyn_VoidType){ goto _LL927;} else{ goto _LL880;} _LL927:
_temp926= _temp874.f2; if( _temp926 ==( void*) Cyc_Absyn_VoidType){ goto _LL879;}
else{ goto _LL880;} _LL880: _LL935: _temp934= _temp874.f1; if(( unsigned int)
_temp934 > 4u?*(( int*) _temp934) == Cyc_Absyn_VarType: 0){ _LL937: _temp936=((
struct Cyc_Absyn_VarType_struct*) _temp934)->f1; goto _LL931;} else{ goto _LL882;}
_LL931: _temp930= _temp874.f2; if(( unsigned int) _temp930 > 4u?*(( int*)
_temp930) == Cyc_Absyn_VarType: 0){ _LL933: _temp932=(( struct Cyc_Absyn_VarType_struct*)
_temp930)->f1; goto _LL881;} else{ goto _LL882;} _LL882: _LL947: _temp946=
_temp874.f1; if(( unsigned int) _temp946 > 4u?*(( int*) _temp946) == Cyc_Absyn_StructType:
0){ _LL953: _temp952=(( struct Cyc_Absyn_StructType_struct*) _temp946)->f1; goto
_LL951; _LL951: _temp950=(( struct Cyc_Absyn_StructType_struct*) _temp946)->f2;
goto _LL949; _LL949: _temp948=(( struct Cyc_Absyn_StructType_struct*) _temp946)->f3;
goto _LL939;} else{ goto _LL884;} _LL939: _temp938= _temp874.f2; if((
unsigned int) _temp938 > 4u?*(( int*) _temp938) == Cyc_Absyn_StructType: 0){
_LL945: _temp944=(( struct Cyc_Absyn_StructType_struct*) _temp938)->f1; goto
_LL943; _LL943: _temp942=(( struct Cyc_Absyn_StructType_struct*) _temp938)->f2;
goto _LL941; _LL941: _temp940=(( struct Cyc_Absyn_StructType_struct*) _temp938)->f3;
goto _LL883;} else{ goto _LL884;} _LL884: _LL959: _temp958= _temp874.f1; if((
unsigned int) _temp958 > 4u?*(( int*) _temp958) == Cyc_Absyn_EnumType: 0){
_LL961: _temp960=(( struct Cyc_Absyn_EnumType_struct*) _temp958)->f1; goto
_LL955;} else{ goto _LL886;} _LL955: _temp954= _temp874.f2; if(( unsigned int)
_temp954 > 4u?*(( int*) _temp954) == Cyc_Absyn_EnumType: 0){ _LL957: _temp956=((
struct Cyc_Absyn_EnumType_struct*) _temp954)->f1; goto _LL885;} else{ goto
_LL886;} _LL886: _LL971: _temp970= _temp874.f1; if(( unsigned int) _temp970 > 4u?*((
int*) _temp970) == Cyc_Absyn_UnionType: 0){ _LL977: _temp976=(( struct Cyc_Absyn_UnionType_struct*)
_temp970)->f1; goto _LL975; _LL975: _temp974=(( struct Cyc_Absyn_UnionType_struct*)
_temp970)->f2; goto _LL973; _LL973: _temp972=(( struct Cyc_Absyn_UnionType_struct*)
_temp970)->f3; goto _LL963;} else{ goto _LL888;} _LL963: _temp962= _temp874.f2;
if(( unsigned int) _temp962 > 4u?*(( int*) _temp962) == Cyc_Absyn_UnionType: 0){
_LL969: _temp968=(( struct Cyc_Absyn_UnionType_struct*) _temp962)->f1; goto
_LL967; _LL967: _temp966=(( struct Cyc_Absyn_UnionType_struct*) _temp962)->f2;
goto _LL965; _LL965: _temp964=(( struct Cyc_Absyn_UnionType_struct*) _temp962)->f3;
goto _LL887;} else{ goto _LL888;} _LL888: _LL991: _temp990= _temp874.f1; if((
unsigned int) _temp990 > 4u?*(( int*) _temp990) == Cyc_Absyn_TunionType: 0){
_LL993: _temp992=(( struct Cyc_Absyn_TunionType_struct*) _temp990)->f1; _LL999:
_temp998=( void*) _temp992.tunion_info; if(*(( int*) _temp998) == Cyc_Absyn_KnownTunion){
_LL1001: _temp1000=(( struct Cyc_Absyn_KnownTunion_struct*) _temp998)->f1; goto
_LL997;} else{ goto _LL890;} _LL997: _temp996= _temp992.targs; goto _LL995;
_LL995: _temp994=( void*) _temp992.rgn; goto _LL979;} else{ goto _LL890;} _LL979:
_temp978= _temp874.f2; if(( unsigned int) _temp978 > 4u?*(( int*) _temp978) ==
Cyc_Absyn_TunionType: 0){ _LL981: _temp980=(( struct Cyc_Absyn_TunionType_struct*)
_temp978)->f1; _LL987: _temp986=( void*) _temp980.tunion_info; if(*(( int*)
_temp986) == Cyc_Absyn_KnownTunion){ _LL989: _temp988=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp986)->f1; goto _LL985;} else{ goto _LL890;} _LL985: _temp984= _temp980.targs;
goto _LL983; _LL983: _temp982=( void*) _temp980.rgn; goto _LL889;} else{ goto
_LL890;} _LL890: _LL1015: _temp1014= _temp874.f1; if(( unsigned int) _temp1014 >
4u?*(( int*) _temp1014) == Cyc_Absyn_TunionFieldType: 0){ _LL1017: _temp1016=((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1014)->f1; _LL1021: _temp1020=(
void*) _temp1016.field_info; if(*(( int*) _temp1020) == Cyc_Absyn_KnownTunionfield){
_LL1025: _temp1024=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1020)->f1;
goto _LL1023; _LL1023: _temp1022=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1020)->f2; goto _LL1019;} else{ goto _LL892;} _LL1019: _temp1018= _temp1016.targs;
goto _LL1003;} else{ goto _LL892;} _LL1003: _temp1002= _temp874.f2; if((
unsigned int) _temp1002 > 4u?*(( int*) _temp1002) == Cyc_Absyn_TunionFieldType:
0){ _LL1005: _temp1004=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1002)->f1;
_LL1009: _temp1008=( void*) _temp1004.field_info; if(*(( int*) _temp1008) == Cyc_Absyn_KnownTunionfield){
_LL1013: _temp1012=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1008)->f1;
goto _LL1011; _LL1011: _temp1010=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1008)->f2; goto _LL1007;} else{ goto _LL892;} _LL1007: _temp1006= _temp1004.targs;
goto _LL891;} else{ goto _LL892;} _LL892: _LL1041: _temp1040= _temp874.f1; if((
unsigned int) _temp1040 > 4u?*(( int*) _temp1040) == Cyc_Absyn_PointerType: 0){
_LL1043: _temp1042=(( struct Cyc_Absyn_PointerType_struct*) _temp1040)->f1;
_LL1053: _temp1052=( void*) _temp1042.elt_typ; goto _LL1051; _LL1051: _temp1050=(
void*) _temp1042.rgn_typ; goto _LL1049; _LL1049: _temp1048= _temp1042.nullable;
goto _LL1047; _LL1047: _temp1046= _temp1042.tq; goto _LL1045; _LL1045: _temp1044=
_temp1042.bounds; goto _LL1027;} else{ goto _LL894;} _LL1027: _temp1026=
_temp874.f2; if(( unsigned int) _temp1026 > 4u?*(( int*) _temp1026) == Cyc_Absyn_PointerType:
0){ _LL1029: _temp1028=(( struct Cyc_Absyn_PointerType_struct*) _temp1026)->f1;
_LL1039: _temp1038=( void*) _temp1028.elt_typ; goto _LL1037; _LL1037: _temp1036=(
void*) _temp1028.rgn_typ; goto _LL1035; _LL1035: _temp1034= _temp1028.nullable;
goto _LL1033; _LL1033: _temp1032= _temp1028.tq; goto _LL1031; _LL1031: _temp1030=
_temp1028.bounds; goto _LL893;} else{ goto _LL894;} _LL894: _LL1061: _temp1060=
_temp874.f1; if(( unsigned int) _temp1060 > 4u?*(( int*) _temp1060) == Cyc_Absyn_IntType:
0){ _LL1065: _temp1064=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1060)->f1;
goto _LL1063; _LL1063: _temp1062=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1060)->f2; goto _LL1055;} else{ goto _LL896;} _LL1055: _temp1054= _temp874.f2;
if(( unsigned int) _temp1054 > 4u?*(( int*) _temp1054) == Cyc_Absyn_IntType: 0){
_LL1059: _temp1058=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1054)->f1;
goto _LL1057; _LL1057: _temp1056=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1054)->f2; goto _LL895;} else{ goto _LL896;} _LL896: _LL1069: _temp1068=
_temp874.f1; if( _temp1068 ==( void*) Cyc_Absyn_FloatType){ goto _LL1067;} else{
goto _LL898;} _LL1067: _temp1066= _temp874.f2; if( _temp1066 ==( void*) Cyc_Absyn_FloatType){
goto _LL897;} else{ goto _LL898;} _LL898: _LL1073: _temp1072= _temp874.f1; if(
_temp1072 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1071;} else{ goto _LL900;}
_LL1071: _temp1070= _temp874.f2; if( _temp1070 ==( void*) Cyc_Absyn_DoubleType){
goto _LL899;} else{ goto _LL900;} _LL900: _LL1083: _temp1082= _temp874.f1; if((
unsigned int) _temp1082 > 4u?*(( int*) _temp1082) == Cyc_Absyn_ArrayType: 0){
_LL1089: _temp1088=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1082)->f1;
goto _LL1087; _LL1087: _temp1086=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1082)->f2; goto _LL1085; _LL1085: _temp1084=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1082)->f3; goto _LL1075;} else{ goto _LL902;} _LL1075: _temp1074= _temp874.f2;
if(( unsigned int) _temp1074 > 4u?*(( int*) _temp1074) == Cyc_Absyn_ArrayType: 0){
_LL1081: _temp1080=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1074)->f1;
goto _LL1079; _LL1079: _temp1078=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1074)->f2; goto _LL1077; _LL1077: _temp1076=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1074)->f3; goto _LL901;} else{ goto _LL902;} _LL902: _LL1111: _temp1110=
_temp874.f1; if(( unsigned int) _temp1110 > 4u?*(( int*) _temp1110) == Cyc_Absyn_FnType:
0){ _LL1113: _temp1112=(( struct Cyc_Absyn_FnType_struct*) _temp1110)->f1;
_LL1129: _temp1128= _temp1112.tvars; goto _LL1127; _LL1127: _temp1126= _temp1112.effect;
goto _LL1125; _LL1125: _temp1124=( void*) _temp1112.ret_typ; goto _LL1123;
_LL1123: _temp1122= _temp1112.args; goto _LL1121; _LL1121: _temp1120= _temp1112.c_varargs;
goto _LL1119; _LL1119: _temp1118= _temp1112.cyc_varargs; goto _LL1117; _LL1117:
_temp1116= _temp1112.rgn_po; goto _LL1115; _LL1115: _temp1114= _temp1112.attributes;
goto _LL1091;} else{ goto _LL904;} _LL1091: _temp1090= _temp874.f2; if((
unsigned int) _temp1090 > 4u?*(( int*) _temp1090) == Cyc_Absyn_FnType: 0){
_LL1093: _temp1092=(( struct Cyc_Absyn_FnType_struct*) _temp1090)->f1; _LL1109:
_temp1108= _temp1092.tvars; goto _LL1107; _LL1107: _temp1106= _temp1092.effect;
goto _LL1105; _LL1105: _temp1104=( void*) _temp1092.ret_typ; goto _LL1103;
_LL1103: _temp1102= _temp1092.args; goto _LL1101; _LL1101: _temp1100= _temp1092.c_varargs;
goto _LL1099; _LL1099: _temp1098= _temp1092.cyc_varargs; goto _LL1097; _LL1097:
_temp1096= _temp1092.rgn_po; goto _LL1095; _LL1095: _temp1094= _temp1092.attributes;
goto _LL903;} else{ goto _LL904;} _LL904: _LL1135: _temp1134= _temp874.f1; if((
unsigned int) _temp1134 > 4u?*(( int*) _temp1134) == Cyc_Absyn_TupleType: 0){
_LL1137: _temp1136=(( struct Cyc_Absyn_TupleType_struct*) _temp1134)->f1; goto
_LL1131;} else{ goto _LL906;} _LL1131: _temp1130= _temp874.f2; if(( unsigned int)
_temp1130 > 4u?*(( int*) _temp1130) == Cyc_Absyn_TupleType: 0){ _LL1133:
_temp1132=(( struct Cyc_Absyn_TupleType_struct*) _temp1130)->f1; goto _LL905;}
else{ goto _LL906;} _LL906: _LL1143: _temp1142= _temp874.f1; if(( unsigned int)
_temp1142 > 4u?*(( int*) _temp1142) == Cyc_Absyn_AnonStructType: 0){ _LL1145:
_temp1144=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1142)->f1; goto
_LL1139;} else{ goto _LL908;} _LL1139: _temp1138= _temp874.f2; if(( unsigned int)
_temp1138 > 4u?*(( int*) _temp1138) == Cyc_Absyn_AnonStructType: 0){ _LL1141:
_temp1140=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1138)->f1; goto
_LL907;} else{ goto _LL908;} _LL908: _LL1151: _temp1150= _temp874.f1; if((
unsigned int) _temp1150 > 4u?*(( int*) _temp1150) == Cyc_Absyn_AnonUnionType: 0){
_LL1153: _temp1152=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1150)->f1;
goto _LL1147;} else{ goto _LL910;} _LL1147: _temp1146= _temp874.f2; if((
unsigned int) _temp1146 > 4u?*(( int*) _temp1146) == Cyc_Absyn_AnonUnionType: 0){
_LL1149: _temp1148=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1146)->f1;
goto _LL909;} else{ goto _LL910;} _LL910: _LL1157: _temp1156= _temp874.f1; if(
_temp1156 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1155;} else{ goto _LL912;}
_LL1155: _temp1154= _temp874.f2; if( _temp1154 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL911;} else{ goto _LL912;} _LL912: _LL1163: _temp1162= _temp874.f1; if((
unsigned int) _temp1162 > 4u?*(( int*) _temp1162) == Cyc_Absyn_RgnHandleType: 0){
_LL1165: _temp1164=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1162)->f1;
goto _LL1159;} else{ goto _LL914;} _LL1159: _temp1158= _temp874.f2; if((
unsigned int) _temp1158 > 4u?*(( int*) _temp1158) == Cyc_Absyn_RgnHandleType: 0){
_LL1161: _temp1160=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1158)->f1;
goto _LL913;} else{ goto _LL914;} _LL914: _LL1167: _temp1166= _temp874.f1; if((
unsigned int) _temp1166 > 4u?*(( int*) _temp1166) == Cyc_Absyn_JoinEff: 0){ goto
_LL915;} else{ goto _LL916;} _LL916: _LL1169: _temp1168= _temp874.f2; if((
unsigned int) _temp1168 > 4u?*(( int*) _temp1168) == Cyc_Absyn_JoinEff: 0){ goto
_LL917;} else{ goto _LL918;} _LL918: _LL1171: _temp1170= _temp874.f1; if((
unsigned int) _temp1170 > 4u?*(( int*) _temp1170) == Cyc_Absyn_AccessEff: 0){
goto _LL919;} else{ goto _LL920;} _LL920: _LL1173: _temp1172= _temp874.f2; if((
unsigned int) _temp1172 > 4u?*(( int*) _temp1172) == Cyc_Absyn_AccessEff: 0){
goto _LL921;} else{ goto _LL922;} _LL922: goto _LL923; _LL877: Cyc_Tcutil_unify_it(
t2, t1); return; _LL879: return; _LL881: { struct _tagged_arr* _temp1174=
_temp936->name; struct _tagged_arr* _temp1175= _temp932->name; int _temp1176=*((
int*) _check_null( _temp936->identity)); int _temp1177=*(( int*) _check_null(
_temp932->identity)); void* _temp1178= Cyc_Absyn_conref_val( _temp936->kind);
void* _temp1179= Cyc_Absyn_conref_val( _temp932->kind); if( _temp1177 ==
_temp1176? Cyc_String_zstrptrcmp( _temp1174, _temp1175) == 0: 0){ if( _temp1178
!= _temp1179){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(( struct
_tagged_arr)({ struct _tagged_arr _temp1180=* _temp1174; struct _tagged_arr
_temp1181= Cyc_Absynpp_kind2string( _temp1178); struct _tagged_arr _temp1182=
Cyc_Absynpp_kind2string( _temp1179); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_get_arr_size( _temp1180, 1u), _temp1180.curr, _get_arr_size( _temp1181, 1u),
_temp1181.curr, _get_arr_size( _temp1182, 1u), _temp1182.curr);}));} return;}
goto _LL875;} _LL883: if((( _temp944 != 0? _temp952 != 0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp944),( struct _tuple1*) _check_null( _temp952))
== 0: 0)? 1:( _temp944 == 0? _temp952 == 0: 0)){ Cyc_Tcutil_unify_list( _temp942,
_temp950); return;} goto _LL875; _LL885: if( Cyc_Absyn_qvar_cmp( _temp960,
_temp956) == 0){ return;} goto _LL875; _LL887: if((( _temp968 != 0? _temp976 !=
0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp968),( struct
_tuple1*) _check_null( _temp976)) == 0: 0)? 1:( _temp968 == 0? _temp976 == 0: 0)){
Cyc_Tcutil_unify_list( _temp966, _temp974); return;} goto _LL875; _LL889: if(
_temp1000 == _temp988? 1: Cyc_Absyn_qvar_cmp( _temp1000->name, _temp988->name)
== 0){ Cyc_Tcutil_unify_it( _temp982, _temp994); Cyc_Tcutil_unify_list( _temp984,
_temp996); return;} goto _LL875; _LL891: if(( _temp1024 == _temp1012? 1: Cyc_Absyn_qvar_cmp(
_temp1024->name, _temp1012->name) == 0)? _temp1022 == _temp1010? 1: Cyc_Absyn_qvar_cmp(
_temp1022->name, _temp1010->name) == 0: 0){ Cyc_Tcutil_unify_list( _temp1006,
_temp1018); return;} goto _LL875; _LL893: Cyc_Tcutil_unify_it( _temp1038,
_temp1052); Cyc_Tcutil_unify_it( _temp1050, _temp1036); Cyc_Tcutil_unify_tqual(
_temp1032, _temp1046); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1030, _temp1044);{ void* _temp1183=( void*)( Cyc_Absyn_compress_conref(
_temp1030))->v; void* _temp1189; _LL1185: if(( unsigned int) _temp1183 > 1u?*((
int*) _temp1183) == Cyc_Absyn_Eq_constr: 0){ _LL1190: _temp1189=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1183)->f1; if( _temp1189 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1186;} else{ goto _LL1187;}} else{ goto _LL1187;} _LL1187: goto _LL1188;
_LL1186: return; _LL1188: goto _LL1184; _LL1184:;}(( void(*)( int(* cmp)( int,
int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp1034, _temp1048); return; _LL895: if( _temp1058 ==
_temp1064? _temp1056 == _temp1062: 0){ return;} goto _LL875; _LL897: return;
_LL899: return; _LL901: Cyc_Tcutil_unify_it( _temp1080, _temp1088); Cyc_Tcutil_unify_tqual(
_temp1078, _temp1086); if( _temp1084 == _temp1076){ return;} if( _temp1084 == 0?
1: _temp1076 == 0){ goto _LL875;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1084)) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1076))){ return;} goto _LL875; _LL903: { int done= 0;{ struct
_RegionHandle _temp1191= _new_region(); struct _RegionHandle* rgn=& _temp1191;
_push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1108 != 0) { if(
_temp1128 == 0){ break;} inst=({ struct Cyc_List_List* _temp1192=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1192->hd=( void*)({
struct _tuple5* _temp1193=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp1193->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1128))->hd; _temp1193->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1194=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1194[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1195; _temp1195.tag= Cyc_Absyn_VarType;
_temp1195.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp1108))->hd; _temp1195;}); _temp1194;}); _temp1193;}); _temp1192->tl= inst;
_temp1192;}); _temp1108=(( struct Cyc_List_List*) _check_null( _temp1108))->tl;
_temp1128=(( struct Cyc_List_List*) _check_null( _temp1128))->tl;} if( _temp1128
!= 0){ _npop_handler( 0u); goto _LL875;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1196=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1196[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1197; _temp1197.tag= Cyc_Absyn_FnType; _temp1197.f1=({ struct Cyc_Absyn_FnInfo
_temp1198; _temp1198.tvars= 0; _temp1198.effect= _temp1106; _temp1198.ret_typ=(
void*) _temp1104; _temp1198.args= _temp1102; _temp1198.c_varargs= _temp1100;
_temp1198.cyc_varargs= _temp1098; _temp1198.rgn_po= _temp1096; _temp1198.attributes=
_temp1094; _temp1198;}); _temp1197;}); _temp1196;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1199=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1199[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1200; _temp1200.tag= Cyc_Absyn_FnType; _temp1200.f1=({ struct Cyc_Absyn_FnInfo
_temp1201; _temp1201.tvars= 0; _temp1201.effect= _temp1126; _temp1201.ret_typ=(
void*) _temp1124; _temp1201.args= _temp1122; _temp1201.c_varargs= _temp1120;
_temp1201.cyc_varargs= _temp1118; _temp1201.rgn_po= _temp1116; _temp1201.attributes=
_temp1114; _temp1201;}); _temp1200;}); _temp1199;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1104, _temp1124); for( 0;
_temp1102 != 0? _temp1122 != 0: 0;( _temp1102=(( struct Cyc_List_List*)
_check_null( _temp1102))->tl, _temp1122=(( struct Cyc_List_List*) _check_null(
_temp1122))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1102))->hd)).f2,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1122))->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1102))->hd)).f3,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1122))->hd)).f3);} if( _temp1102 != 0?
1: _temp1122 != 0){ goto _LL875;} if( _temp1100 != _temp1120){ goto _LL875;}{
int bad_cyc_vararg= 0;{ struct _tuple11 _temp1203=({ struct _tuple11 _temp1202;
_temp1202.f1= _temp1098; _temp1202.f2= _temp1118; _temp1202;}); struct Cyc_Absyn_VarargInfo*
_temp1213; struct Cyc_Absyn_VarargInfo* _temp1215; struct Cyc_Absyn_VarargInfo*
_temp1217; struct Cyc_Absyn_VarargInfo* _temp1219; struct Cyc_Absyn_VarargInfo*
_temp1221; struct Cyc_Absyn_VarargInfo _temp1223; int _temp1224; void* _temp1226;
void* _temp1228; struct Cyc_Absyn_Tqual _temp1230; struct Cyc_Core_Opt*
_temp1232; struct Cyc_Absyn_VarargInfo* _temp1234; struct Cyc_Absyn_VarargInfo
_temp1236; int _temp1237; void* _temp1239; void* _temp1241; struct Cyc_Absyn_Tqual
_temp1243; struct Cyc_Core_Opt* _temp1245; _LL1205: _LL1216: _temp1215=
_temp1203.f1; if( _temp1215 == 0){ goto _LL1214;} else{ goto _LL1207;} _LL1214:
_temp1213= _temp1203.f2; if( _temp1213 == 0){ goto _LL1206;} else{ goto _LL1207;}
_LL1207: _LL1218: _temp1217= _temp1203.f1; if( _temp1217 == 0){ goto _LL1208;}
else{ goto _LL1209;} _LL1209: _LL1220: _temp1219= _temp1203.f2; if( _temp1219 ==
0){ goto _LL1210;} else{ goto _LL1211;} _LL1211: _LL1235: _temp1234= _temp1203.f1;
if( _temp1234 == 0){ goto _LL1204;} else{ _temp1236=* _temp1234; _LL1246:
_temp1245= _temp1236.name; goto _LL1244; _LL1244: _temp1243= _temp1236.tq; goto
_LL1242; _LL1242: _temp1241=( void*) _temp1236.type; goto _LL1240; _LL1240:
_temp1239=( void*) _temp1236.rgn; goto _LL1238; _LL1238: _temp1237= _temp1236.inject;
goto _LL1222;} _LL1222: _temp1221= _temp1203.f2; if( _temp1221 == 0){ goto
_LL1204;} else{ _temp1223=* _temp1221; _LL1233: _temp1232= _temp1223.name; goto
_LL1231; _LL1231: _temp1230= _temp1223.tq; goto _LL1229; _LL1229: _temp1228=(
void*) _temp1223.type; goto _LL1227; _LL1227: _temp1226=( void*) _temp1223.rgn;
goto _LL1225; _LL1225: _temp1224= _temp1223.inject; goto _LL1212;} _LL1206: goto
_LL1204; _LL1208: goto _LL1210; _LL1210: bad_cyc_vararg= 1; goto _LL1204;
_LL1212: Cyc_Tcutil_unify_tqual( _temp1243, _temp1230); Cyc_Tcutil_unify_it(
_temp1241, _temp1228); Cyc_Tcutil_unify_it( _temp1239, _temp1226); if( _temp1237
!= _temp1224){ bad_cyc_vararg= 1;} goto _LL1204; _LL1204:;} if( bad_cyc_vararg){
goto _LL875;}{ int bad_effect= 0;{ struct _tuple12 _temp1248=({ struct _tuple12
_temp1247; _temp1247.f1= _temp1106; _temp1247.f2= _temp1126; _temp1247;});
struct Cyc_Core_Opt* _temp1258; struct Cyc_Core_Opt* _temp1260; struct Cyc_Core_Opt*
_temp1262; struct Cyc_Core_Opt* _temp1264; _LL1250: _LL1261: _temp1260=
_temp1248.f1; if( _temp1260 == 0){ goto _LL1259;} else{ goto _LL1252;} _LL1259:
_temp1258= _temp1248.f2; if( _temp1258 == 0){ goto _LL1251;} else{ goto _LL1252;}
_LL1252: _LL1263: _temp1262= _temp1248.f1; if( _temp1262 == 0){ goto _LL1253;}
else{ goto _LL1254;} _LL1254: _LL1265: _temp1264= _temp1248.f2; if( _temp1264 ==
0){ goto _LL1255;} else{ goto _LL1256;} _LL1256: goto _LL1257; _LL1251: goto
_LL1249; _LL1253: goto _LL1255; _LL1255: bad_effect= 1; goto _LL1249; _LL1257:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1106))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1126))->v); goto _LL1249; _LL1249:;} if( bad_effect){ goto _LL875;} if( !
Cyc_Tcutil_same_atts( _temp1114, _temp1094)){ goto _LL875;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1116, _temp1096)){ goto _LL875;} return;}}} _LL905: for( 0; _temp1132 != 0?
_temp1136 != 0: 0;( _temp1132=(( struct Cyc_List_List*) _check_null( _temp1132))->tl,
_temp1136=(( struct Cyc_List_List*) _check_null( _temp1136))->tl)){ Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1132))->hd)).f1,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1136))->hd)).f1);
Cyc_Tcutil_unify_it((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp1132))->hd)).f2,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp1136))->hd)).f2);} if( _temp1132 == 0? _temp1136 == 0: 0){ return;} goto
_LL875; _LL907: _temp1152= _temp1144; _temp1148= _temp1140; goto _LL909; _LL909:
for( 0; _temp1148 != 0? _temp1152 != 0: 0;( _temp1148=(( struct Cyc_List_List*)
_check_null( _temp1148))->tl, _temp1152=(( struct Cyc_List_List*) _check_null(
_temp1152))->tl)){ struct Cyc_Absyn_Structfield* _temp1266=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1148))->hd; struct Cyc_Absyn_Structfield*
_temp1267=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1152))->hd; if( Cyc_String_zstrptrcmp( _temp1266->name, _temp1267->name) !=
0){( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual( _temp1266->tq,
_temp1267->tq); Cyc_Tcutil_unify_it(( void*) _temp1266->type,( void*) _temp1267->type);
if((( ! Cyc_Tcutil_same_atts( _temp1266->attributes, _temp1267->attributes)? 1:(
_temp1266->width != 0? _temp1267->width == 0: 0))? 1:( _temp1267->width != 0?
_temp1266->width == 0: 0))? 1:(( _temp1266->width != 0? _temp1267->width != 0: 0)?
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1266->width))->v) != Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1267->width))->v): 0)){(
int) _throw(( void*) Cyc_Tcutil_Unify);}} if( _temp1148 == 0? _temp1152 == 0: 0){
return;} goto _LL875; _LL911: return; _LL913: Cyc_Tcutil_unify_it( _temp1164,
_temp1160); return; _LL915: goto _LL917; _LL917: goto _LL919; _LL919: goto
_LL921; _LL921: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL875;} _LL923: goto _LL875; _LL875:;}( int) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1268= Cyc_Tcutil_compress(
t); _LL1270: if(( unsigned int) _temp1268 > 4u?*(( int*) _temp1268) == Cyc_Absyn_IntType:
0){ goto _LL1271;} else{ goto _LL1272;} _LL1272: if( _temp1268 ==( void*) Cyc_Absyn_FloatType){
goto _LL1273;} else{ goto _LL1274;} _LL1274: if( _temp1268 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1275;} else{ goto _LL1276;} _LL1276: if(( unsigned int) _temp1268 > 4u?*((
int*) _temp1268) == Cyc_Absyn_EnumType: 0){ goto _LL1277;} else{ goto _LL1278;}
_LL1278: goto _LL1279; _LL1271: return 1; _LL1273: return 1; _LL1275: return 1;
_LL1277: return 1; _LL1279: return 0; _LL1269:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1281=({ struct _tuple8 _temp1280; _temp1280.f1= t1;
_temp1280.f2= t2; _temp1280;}); void* _temp1303; void* _temp1305; void*
_temp1307; void* _temp1309; void* _temp1311; void* _temp1313; void* _temp1315;
void* _temp1317; void* _temp1319; void* _temp1321; void* _temp1323; void*
_temp1325; void* _temp1327; void* _temp1329; void* _temp1331; void* _temp1333;
void* _temp1335; void* _temp1337; void* _temp1339; void* _temp1341; void*
_temp1343; void* _temp1345; void* _temp1347; void* _temp1349; void* _temp1351;
void* _temp1353; void* _temp1355; _LL1283: _LL1306: _temp1305= _temp1281.f1; if(
_temp1305 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1304;} else{ goto _LL1285;}
_LL1304: _temp1303= _temp1281.f2; if( _temp1303 ==( void*) Cyc_Absyn_FloatType){
goto _LL1284;} else{ goto _LL1285;} _LL1285: _LL1310: _temp1309= _temp1281.f1;
if( _temp1309 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1308;} else{ goto
_LL1287;} _LL1308: _temp1307= _temp1281.f2; if(( unsigned int) _temp1307 > 4u?*((
int*) _temp1307) == Cyc_Absyn_IntType: 0){ goto _LL1286;} else{ goto _LL1287;}
_LL1287: _LL1314: _temp1313= _temp1281.f1; if( _temp1313 ==( void*) Cyc_Absyn_FloatType){
goto _LL1312;} else{ goto _LL1289;} _LL1312: _temp1311= _temp1281.f2; if((
unsigned int) _temp1311 > 4u?*(( int*) _temp1311) == Cyc_Absyn_IntType: 0){ goto
_LL1288;} else{ goto _LL1289;} _LL1289: _LL1320: _temp1319= _temp1281.f1; if((
unsigned int) _temp1319 > 4u?*(( int*) _temp1319) == Cyc_Absyn_IntType: 0){
_LL1322: _temp1321=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1319)->f2;
if( _temp1321 ==( void*) Cyc_Absyn_B8){ goto _LL1316;} else{ goto _LL1291;}}
else{ goto _LL1291;} _LL1316: _temp1315= _temp1281.f2; if(( unsigned int)
_temp1315 > 4u?*(( int*) _temp1315) == Cyc_Absyn_IntType: 0){ _LL1318: _temp1317=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1315)->f2; if( _temp1317 ==(
void*) Cyc_Absyn_B8){ goto _LL1290;} else{ goto _LL1291;}} else{ goto _LL1291;}
_LL1291: _LL1324: _temp1323= _temp1281.f1; if(( unsigned int) _temp1323 > 4u?*((
int*) _temp1323) == Cyc_Absyn_IntType: 0){ _LL1326: _temp1325=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1323)->f2; if( _temp1325 ==( void*) Cyc_Absyn_B8){
goto _LL1292;} else{ goto _LL1293;}} else{ goto _LL1293;} _LL1293: _LL1330:
_temp1329= _temp1281.f1; if(( unsigned int) _temp1329 > 4u?*(( int*) _temp1329)
== Cyc_Absyn_IntType: 0){ _LL1332: _temp1331=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1329)->f2; if( _temp1331 ==( void*) Cyc_Absyn_B4){ goto _LL1328;} else{
goto _LL1295;}} else{ goto _LL1295;} _LL1328: _temp1327= _temp1281.f2; if(
_temp1327 ==( void*) Cyc_Absyn_FloatType){ goto _LL1294;} else{ goto _LL1295;}
_LL1295: _LL1338: _temp1337= _temp1281.f1; if(( unsigned int) _temp1337 > 4u?*((
int*) _temp1337) == Cyc_Absyn_IntType: 0){ _LL1340: _temp1339=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1337)->f2; if( _temp1339 ==( void*) Cyc_Absyn_B4){
goto _LL1334;} else{ goto _LL1297;}} else{ goto _LL1297;} _LL1334: _temp1333=
_temp1281.f2; if(( unsigned int) _temp1333 > 4u?*(( int*) _temp1333) == Cyc_Absyn_IntType:
0){ _LL1336: _temp1335=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1333)->f2;
if( _temp1335 ==( void*) Cyc_Absyn_B2){ goto _LL1296;} else{ goto _LL1297;}}
else{ goto _LL1297;} _LL1297: _LL1346: _temp1345= _temp1281.f1; if((
unsigned int) _temp1345 > 4u?*(( int*) _temp1345) == Cyc_Absyn_IntType: 0){
_LL1348: _temp1347=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1345)->f2;
if( _temp1347 ==( void*) Cyc_Absyn_B4){ goto _LL1342;} else{ goto _LL1299;}}
else{ goto _LL1299;} _LL1342: _temp1341= _temp1281.f2; if(( unsigned int)
_temp1341 > 4u?*(( int*) _temp1341) == Cyc_Absyn_IntType: 0){ _LL1344: _temp1343=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1341)->f2; if( _temp1343 ==(
void*) Cyc_Absyn_B1){ goto _LL1298;} else{ goto _LL1299;}} else{ goto _LL1299;}
_LL1299: _LL1354: _temp1353= _temp1281.f1; if(( unsigned int) _temp1353 > 4u?*((
int*) _temp1353) == Cyc_Absyn_IntType: 0){ _LL1356: _temp1355=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1353)->f2; if( _temp1355 ==( void*) Cyc_Absyn_B2){
goto _LL1350;} else{ goto _LL1301;}} else{ goto _LL1301;} _LL1350: _temp1349=
_temp1281.f2; if(( unsigned int) _temp1349 > 4u?*(( int*) _temp1349) == Cyc_Absyn_IntType:
0){ _LL1352: _temp1351=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1349)->f2;
if( _temp1351 ==( void*) Cyc_Absyn_B1){ goto _LL1300;} else{ goto _LL1301;}}
else{ goto _LL1301;} _LL1301: goto _LL1302; _LL1284: return 1; _LL1286: return 1;
_LL1288: return 1; _LL1290: return 0; _LL1292: return 1; _LL1294: return 1;
_LL1296: return 1; _LL1298: return 1; _LL1300: return 1; _LL1302: return 0;
_LL1282:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv* te, void* t,
struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{ struct Cyc_List_List*
el= es; for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1357=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1357->v=( void*) t1; _temp1357;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
el))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr _temp1358= Cyc_Absynpp_typ2string(
t); struct _tagged_arr _temp1359= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
xprintf("type mismatch: expecting %.*s but found %.*s", _get_arr_size( _temp1358,
1u), _temp1358.curr, _get_arr_size( _temp1359, 1u), _temp1359.curr);})); return
0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp1360= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1362: if(( unsigned int) _temp1360 > 4u?*(( int*) _temp1360) == Cyc_Absyn_PointerType:
0){ goto _LL1363;} else{ goto _LL1364;} _LL1364: goto _LL1365; _LL1363: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1361; _LL1365: return 0; _LL1361:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1366= Cyc_Tcutil_compress(
t); _LL1368: if(( unsigned int) _temp1366 > 4u?*(( int*) _temp1366) == Cyc_Absyn_IntType:
0){ goto _LL1369;} else{ goto _LL1370;} _LL1370: if(( unsigned int) _temp1366 >
4u?*(( int*) _temp1366) == Cyc_Absyn_EnumType: 0){ goto _LL1371;} else{ goto
_LL1372;} _LL1372: goto _LL1373; _LL1369: return 1; _LL1371: return 1; _LL1373:
return 0; _LL1367:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){
Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u));} Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_uint_t);
return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){
Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u));} Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_sint_t);
return 1;} return 0;} int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp1375=({ struct _tuple8
_temp1374; _temp1374.f1= t1; _temp1374.f2= t2; _temp1374;}); void* _temp1387;
struct Cyc_Absyn_PtrInfo _temp1389; void* _temp1391; struct Cyc_Absyn_PtrInfo
_temp1393; void* _temp1395; struct Cyc_Absyn_Exp* _temp1397; struct Cyc_Absyn_Tqual
_temp1399; void* _temp1401; void* _temp1403; struct Cyc_Absyn_Exp* _temp1405;
struct Cyc_Absyn_Tqual _temp1407; void* _temp1409; void* _temp1411; struct Cyc_Absyn_TunionInfo
_temp1413; struct Cyc_List_List* _temp1415; void* _temp1417; struct Cyc_Absyn_Tuniondecl*
_temp1419; void* _temp1421; struct Cyc_Absyn_TunionFieldInfo _temp1423; struct
Cyc_List_List* _temp1425; void* _temp1427; struct Cyc_Absyn_Tunionfield*
_temp1429; struct Cyc_Absyn_Tuniondecl* _temp1431; void* _temp1433; struct Cyc_Absyn_TunionInfo
_temp1435; void* _temp1437; struct Cyc_List_List* _temp1439; void* _temp1441;
struct Cyc_Absyn_Tuniondecl* _temp1443; void* _temp1445; struct Cyc_Absyn_PtrInfo
_temp1447; struct Cyc_Absyn_Conref* _temp1449; struct Cyc_Absyn_Tqual _temp1451;
struct Cyc_Absyn_Conref* _temp1453; void* _temp1455; void* _temp1457; _LL1377:
_LL1392: _temp1391= _temp1375.f1; if(( unsigned int) _temp1391 > 4u?*(( int*)
_temp1391) == Cyc_Absyn_PointerType: 0){ _LL1394: _temp1393=(( struct Cyc_Absyn_PointerType_struct*)
_temp1391)->f1; goto _LL1388;} else{ goto _LL1379;} _LL1388: _temp1387=
_temp1375.f2; if(( unsigned int) _temp1387 > 4u?*(( int*) _temp1387) == Cyc_Absyn_PointerType:
0){ _LL1390: _temp1389=(( struct Cyc_Absyn_PointerType_struct*) _temp1387)->f1;
goto _LL1378;} else{ goto _LL1379;} _LL1379: _LL1404: _temp1403= _temp1375.f1;
if(( unsigned int) _temp1403 > 4u?*(( int*) _temp1403) == Cyc_Absyn_ArrayType: 0){
_LL1410: _temp1409=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1403)->f1;
goto _LL1408; _LL1408: _temp1407=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1403)->f2; goto _LL1406; _LL1406: _temp1405=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1403)->f3; goto _LL1396;} else{ goto _LL1381;} _LL1396: _temp1395=
_temp1375.f2; if(( unsigned int) _temp1395 > 4u?*(( int*) _temp1395) == Cyc_Absyn_ArrayType:
0){ _LL1402: _temp1401=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1395)->f1;
goto _LL1400; _LL1400: _temp1399=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1395)->f2; goto _LL1398; _LL1398: _temp1397=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1395)->f3; goto _LL1380;} else{ goto _LL1381;} _LL1381: _LL1422: _temp1421=
_temp1375.f1; if(( unsigned int) _temp1421 > 4u?*(( int*) _temp1421) == Cyc_Absyn_TunionFieldType:
0){ _LL1424: _temp1423=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1421)->f1;
_LL1428: _temp1427=( void*) _temp1423.field_info; if(*(( int*) _temp1427) == Cyc_Absyn_KnownTunionfield){
_LL1432: _temp1431=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1427)->f1;
goto _LL1430; _LL1430: _temp1429=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1427)->f2; goto _LL1426;} else{ goto _LL1383;} _LL1426: _temp1425=
_temp1423.targs; goto _LL1412;} else{ goto _LL1383;} _LL1412: _temp1411=
_temp1375.f2; if(( unsigned int) _temp1411 > 4u?*(( int*) _temp1411) == Cyc_Absyn_TunionType:
0){ _LL1414: _temp1413=(( struct Cyc_Absyn_TunionType_struct*) _temp1411)->f1;
_LL1418: _temp1417=( void*) _temp1413.tunion_info; if(*(( int*) _temp1417) ==
Cyc_Absyn_KnownTunion){ _LL1420: _temp1419=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1417)->f1; goto _LL1416;} else{ goto _LL1383;} _LL1416: _temp1415=
_temp1413.targs; goto _LL1382;} else{ goto _LL1383;} _LL1383: _LL1446: _temp1445=
_temp1375.f1; if(( unsigned int) _temp1445 > 4u?*(( int*) _temp1445) == Cyc_Absyn_PointerType:
0){ _LL1448: _temp1447=(( struct Cyc_Absyn_PointerType_struct*) _temp1445)->f1;
_LL1458: _temp1457=( void*) _temp1447.elt_typ; goto _LL1456; _LL1456: _temp1455=(
void*) _temp1447.rgn_typ; goto _LL1454; _LL1454: _temp1453= _temp1447.nullable;
goto _LL1452; _LL1452: _temp1451= _temp1447.tq; goto _LL1450; _LL1450: _temp1449=
_temp1447.bounds; goto _LL1434;} else{ goto _LL1385;} _LL1434: _temp1433=
_temp1375.f2; if(( unsigned int) _temp1433 > 4u?*(( int*) _temp1433) == Cyc_Absyn_TunionType:
0){ _LL1436: _temp1435=(( struct Cyc_Absyn_TunionType_struct*) _temp1433)->f1;
_LL1442: _temp1441=( void*) _temp1435.tunion_info; if(*(( int*) _temp1441) ==
Cyc_Absyn_KnownTunion){ _LL1444: _temp1443=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1441)->f1; goto _LL1440;} else{ goto _LL1385;} _LL1440: _temp1439=
_temp1435.targs; goto _LL1438; _LL1438: _temp1437=( void*) _temp1435.rgn; goto
_LL1384;} else{ goto _LL1385;} _LL1385: goto _LL1386; _LL1378: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1393.nullable, _temp1389.nullable)){
void* _temp1459=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1393.nullable))->v; int _temp1465; _LL1461:
if(( unsigned int) _temp1459 > 1u?*(( int*) _temp1459) == Cyc_Absyn_Eq_constr: 0){
_LL1466: _temp1465=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1459)->f1;
goto _LL1462;} else{ goto _LL1463;} _LL1463: goto _LL1464; _LL1462: okay= !
_temp1465; goto _LL1460; _LL1464:( int) _throw( Cyc_Tcutil_impos( _tag_arr("silent_castable conref not eq",
sizeof( unsigned char), 30u))); _LL1460:;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1393.bounds, _temp1389.bounds)){ struct _tuple8 _temp1468=({ struct _tuple8
_temp1467; _temp1467.f1=( void*)( Cyc_Absyn_compress_conref( _temp1393.bounds))->v;
_temp1467.f2=( void*)( Cyc_Absyn_compress_conref( _temp1389.bounds))->v;
_temp1467;}); void* _temp1478; void* _temp1480; void* _temp1482; void* _temp1484;
void* _temp1486; void* _temp1488; struct Cyc_Absyn_Exp* _temp1490; void*
_temp1492; void* _temp1494; struct Cyc_Absyn_Exp* _temp1496; void* _temp1498;
void* _temp1500; void* _temp1502; void* _temp1504; _LL1470: _LL1483: _temp1482=
_temp1468.f1; if(( unsigned int) _temp1482 > 1u?*(( int*) _temp1482) == Cyc_Absyn_Eq_constr:
0){ _LL1485: _temp1484=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1482)->f1;
if(( unsigned int) _temp1484 > 1u?*(( int*) _temp1484) == Cyc_Absyn_Upper_b: 0){
goto _LL1479;} else{ goto _LL1472;}} else{ goto _LL1472;} _LL1479: _temp1478=
_temp1468.f2; if(( unsigned int) _temp1478 > 1u?*(( int*) _temp1478) == Cyc_Absyn_Eq_constr:
0){ _LL1481: _temp1480=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1478)->f1;
if( _temp1480 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1471;} else{ goto _LL1472;}}
else{ goto _LL1472;} _LL1472: _LL1493: _temp1492= _temp1468.f1; if((
unsigned int) _temp1492 > 1u?*(( int*) _temp1492) == Cyc_Absyn_Eq_constr: 0){
_LL1495: _temp1494=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1492)->f1;
if(( unsigned int) _temp1494 > 1u?*(( int*) _temp1494) == Cyc_Absyn_Upper_b: 0){
_LL1497: _temp1496=(( struct Cyc_Absyn_Upper_b_struct*) _temp1494)->f1; goto
_LL1487;} else{ goto _LL1474;}} else{ goto _LL1474;} _LL1487: _temp1486=
_temp1468.f2; if(( unsigned int) _temp1486 > 1u?*(( int*) _temp1486) == Cyc_Absyn_Eq_constr:
0){ _LL1489: _temp1488=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1486)->f1;
if(( unsigned int) _temp1488 > 1u?*(( int*) _temp1488) == Cyc_Absyn_Upper_b: 0){
_LL1491: _temp1490=(( struct Cyc_Absyn_Upper_b_struct*) _temp1488)->f1; goto
_LL1473;} else{ goto _LL1474;}} else{ goto _LL1474;} _LL1474: _LL1503: _temp1502=
_temp1468.f1; if(( unsigned int) _temp1502 > 1u?*(( int*) _temp1502) == Cyc_Absyn_Eq_constr:
0){ _LL1505: _temp1504=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1502)->f1;
if( _temp1504 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1499;} else{ goto _LL1476;}}
else{ goto _LL1476;} _LL1499: _temp1498= _temp1468.f2; if(( unsigned int)
_temp1498 > 1u?*(( int*) _temp1498) == Cyc_Absyn_Eq_constr: 0){ _LL1501:
_temp1500=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1498)->f1; if(
_temp1500 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1475;} else{ goto _LL1476;}}
else{ goto _LL1476;} _LL1476: goto _LL1477; _LL1471: okay= 1; goto _LL1469;
_LL1473: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1496) >= Cyc_Evexp_eval_const_uint_exp(
_temp1490): 0; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u)); goto _LL1469; _LL1475: okay= 1; goto _LL1469;
_LL1477: okay= 0; goto _LL1469; _LL1469:;} okay= okay? Cyc_Tcutil_unify(( void*)
_temp1393.elt_typ,( void*) _temp1389.elt_typ): 0; okay= okay? Cyc_Tcutil_unify((
void*) _temp1393.rgn_typ,( void*) _temp1389.rgn_typ)? 1: Cyc_Tcenv_region_outlives(
te,( void*) _temp1393.rgn_typ,( void*) _temp1389.rgn_typ): 0; okay= okay? !(
_temp1393.tq).q_const? 1:( _temp1389.tq).q_const: 0; return okay;} _LL1380: {
int okay; okay=( _temp1405 != 0? _temp1397 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1405)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1397)): 0; return( okay? Cyc_Tcutil_unify(
_temp1409, _temp1401): 0)? ! _temp1407.q_const? 1: _temp1399.q_const: 0;}
_LL1382: if(( _temp1431 == _temp1419? 1: Cyc_Absyn_qvar_cmp( _temp1431->name,
_temp1419->name) == 0)? _temp1429->typs == 0: 0){ for( 0; _temp1425 != 0?
_temp1415 != 0: 0;( _temp1425=(( struct Cyc_List_List*) _check_null( _temp1425))->tl,
_temp1415=(( struct Cyc_List_List*) _check_null( _temp1415))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1425))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1415))->hd)){ break;}} if( _temp1425 == 0?
_temp1415 == 0: 0){ return 1;}} return 0; _LL1384:{ void* _temp1506= Cyc_Tcutil_compress(
_temp1457); struct Cyc_Absyn_TunionFieldInfo _temp1512; struct Cyc_List_List*
_temp1514; void* _temp1516; struct Cyc_Absyn_Tunionfield* _temp1518; struct Cyc_Absyn_Tuniondecl*
_temp1520; _LL1508: if(( unsigned int) _temp1506 > 4u?*(( int*) _temp1506) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1513: _temp1512=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1506)->f1; _LL1517: _temp1516=( void*) _temp1512.field_info; if(*(( int*)
_temp1516) == Cyc_Absyn_KnownTunionfield){ _LL1521: _temp1520=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1516)->f1; goto _LL1519; _LL1519: _temp1518=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1516)->f2; goto _LL1515;} else{ goto _LL1510;} _LL1515: _temp1514=
_temp1512.targs; goto _LL1509;} else{ goto _LL1510;} _LL1510: goto _LL1511;
_LL1509: if( ! Cyc_Tcutil_unify( _temp1455, _temp1437)? ! Cyc_Tcenv_region_outlives(
te, _temp1455, _temp1437): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1453,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1449,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1443->name, _temp1520->name) == 0? _temp1518->typs != 0: 0){ int okay= 1;
for( 0; _temp1514 != 0? _temp1439 != 0: 0;( _temp1514=(( struct Cyc_List_List*)
_check_null( _temp1514))->tl, _temp1439=(( struct Cyc_List_List*) _check_null(
_temp1439))->tl)){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp1514))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp1439))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1514 != 0)? 1:
_temp1439 != 0){ return 0;} return 1;} goto _LL1507; _LL1511: goto _LL1507;
_LL1507:;} return 0; _LL1386: return Cyc_Tcutil_unify( t1, t2); _LL1376:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u));} Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;}
else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1522= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp1523= Cyc_Absynpp_typ2string(
t2); xprintf("implicit cast from %.*s to %.*s", _get_arr_size( _temp1522, 1u),
_temp1522.curr, _get_arr_size( _temp1523, 1u), _temp1523.curr);})); return 1;}
else{ return 0;}}}} int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg( te, e, t);}
static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1524= old_typ; void* _temp1536;
void* _temp1538; _LL1526: if(( unsigned int) _temp1524 > 4u?*(( int*) _temp1524)
== Cyc_Absyn_EnumType: 0){ goto _LL1527;} else{ goto _LL1528;} _LL1528: if((
unsigned int) _temp1524 > 4u?*(( int*) _temp1524) == Cyc_Absyn_IntType: 0){
_LL1539: _temp1538=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1524)->f1;
goto _LL1537; _LL1537: _temp1536=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1524)->f2; goto _LL1529;} else{ goto _LL1530;} _LL1530: if( _temp1524 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1531;} else{ goto _LL1532;} _LL1532: if(
_temp1524 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1533;} else{ goto _LL1534;}
_LL1534: goto _LL1535; _LL1527: t= Cyc_Absyn_sint_t; goto _LL1525; _LL1529:{
void* _temp1540= _temp1536; _LL1542: if( _temp1540 ==( void*) Cyc_Absyn_B1){
goto _LL1543;} else{ goto _LL1544;} _LL1544: if( _temp1540 ==( void*) Cyc_Absyn_B2){
goto _LL1545;} else{ goto _LL1546;} _LL1546: if( _temp1540 ==( void*) Cyc_Absyn_B4){
goto _LL1547;} else{ goto _LL1548;} _LL1548: if( _temp1540 ==( void*) Cyc_Absyn_B8){
goto _LL1549;} else{ goto _LL1541;} _LL1543: t= Cyc_Absyn_sint_t; goto _LL1541;
_LL1545: t= Cyc_Absyn_sint_t; goto _LL1541; _LL1547: t= _temp1538 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1541; _LL1549: t= _temp1538 ==( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto _LL1541;
_LL1541:;} goto _LL1525; _LL1531: t= Cyc_Absyn_float_t; goto _LL1525; _LL1533: t=
Cyc_Absyn_double_t; goto _LL1525; _LL1535: return 0; _LL1525:;} Cyc_Tcutil_unchecked_cast(
te, e, t); return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){
return 1;} Cyc_Tcutil_any_promote( te, e); if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{
if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e,
t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1550= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp1551= Cyc_Absynpp_typ2string(
t2); xprintf("implicit cast from %.*s to %.*s", _get_arr_size( _temp1550, 1u),
_temp1550.curr, _get_arr_size( _temp1551, 1u), _temp1551.curr);})); return 1;}
else{ return 0;}}}} int Cyc_Tcutil_coerceable( void* t){ void* _temp1552= Cyc_Tcutil_compress(
t); _LL1554: if(( unsigned int) _temp1552 > 4u?*(( int*) _temp1552) == Cyc_Absyn_IntType:
0){ goto _LL1555;} else{ goto _LL1556;} _LL1556: if( _temp1552 ==( void*) Cyc_Absyn_FloatType){
goto _LL1557;} else{ goto _LL1558;} _LL1558: if( _temp1552 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1559;} else{ goto _LL1560;} _LL1560: goto _LL1561; _LL1555: return 1;
_LL1557: return 1; _LL1559: return 1; _LL1561: return 0; _LL1553:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1562=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1562->f1= x->tq; _temp1562->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1562;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1563= t1; struct Cyc_List_List* _temp1575; struct Cyc_Absyn_Structdecl**
_temp1577; struct Cyc_List_List* _temp1579; struct _tuple1* _temp1581; struct
Cyc_List_List* _temp1583; _LL1565: if( _temp1563 ==( void*) Cyc_Absyn_VoidType){
goto _LL1566;} else{ goto _LL1567;} _LL1567: if(( unsigned int) _temp1563 > 4u?*((
int*) _temp1563) == Cyc_Absyn_TupleType: 0){ _LL1576: _temp1575=(( struct Cyc_Absyn_TupleType_struct*)
_temp1563)->f1; goto _LL1568;} else{ goto _LL1569;} _LL1569: if(( unsigned int)
_temp1563 > 4u?*(( int*) _temp1563) == Cyc_Absyn_StructType: 0){ _LL1582:
_temp1581=(( struct Cyc_Absyn_StructType_struct*) _temp1563)->f1; goto _LL1580;
_LL1580: _temp1579=(( struct Cyc_Absyn_StructType_struct*) _temp1563)->f2; goto
_LL1578; _LL1578: _temp1577=(( struct Cyc_Absyn_StructType_struct*) _temp1563)->f3;
goto _LL1570;} else{ goto _LL1571;} _LL1571: if(( unsigned int) _temp1563 > 4u?*((
int*) _temp1563) == Cyc_Absyn_AnonStructType: 0){ _LL1584: _temp1583=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1563)->f1; goto _LL1572;} else{ goto
_LL1573;} _LL1573: goto _LL1574; _LL1566: return 0; _LL1568: return _temp1575;
_LL1570: if( _temp1577 == 0? 1: _temp1581 == 0){ return({ struct Cyc_List_List*
_temp1585=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1585->hd=( void*)({ struct _tuple4* _temp1586=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1586->f1= Cyc_Absyn_empty_tqual(); _temp1586->f2=
t1; _temp1586;}); _temp1585->tl= 0; _temp1585;});}{ struct Cyc_Absyn_Structdecl*
_temp1587=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1577)); struct
_tuple1 _temp1588=*(( struct _tuple1*) _check_null( _temp1581)); struct Cyc_List_List*
_temp1589=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1587->tvs, _temp1579); if( _temp1587->fields == 0){
return({ struct Cyc_List_List* _temp1590=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1590->hd=( void*)({ struct _tuple4*
_temp1591=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1591->f1=
Cyc_Absyn_empty_tqual(); _temp1591->f2= t1; _temp1591;}); _temp1590->tl= 0;
_temp1590;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1589,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1587->fields))->v);}
_LL1572: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1583); _LL1574: return({
struct Cyc_List_List* _temp1592=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1592->hd=( void*)({ struct _tuple4* _temp1593=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1593->f1= Cyc_Absyn_empty_tqual();
_temp1593->f2= t1; _temp1593;}); _temp1592->tl= 0; _temp1592;}); _LL1564:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1595=({ struct _tuple8 _temp1594; _temp1594.f1= t1; _temp1594.f2=
t2; _temp1594;}); void* _temp1601; struct Cyc_Absyn_PtrInfo _temp1603; struct
Cyc_Absyn_Conref* _temp1605; struct Cyc_Absyn_Tqual _temp1607; struct Cyc_Absyn_Conref*
_temp1609; void* _temp1611; void* _temp1613; void* _temp1615; struct Cyc_Absyn_PtrInfo
_temp1617; struct Cyc_Absyn_Conref* _temp1619; struct Cyc_Absyn_Tqual _temp1621;
struct Cyc_Absyn_Conref* _temp1623; void* _temp1625; void* _temp1627; _LL1597:
_LL1616: _temp1615= _temp1595.f1; if(( unsigned int) _temp1615 > 4u?*(( int*)
_temp1615) == Cyc_Absyn_PointerType: 0){ _LL1618: _temp1617=(( struct Cyc_Absyn_PointerType_struct*)
_temp1615)->f1; _LL1628: _temp1627=( void*) _temp1617.elt_typ; goto _LL1626;
_LL1626: _temp1625=( void*) _temp1617.rgn_typ; goto _LL1624; _LL1624: _temp1623=
_temp1617.nullable; goto _LL1622; _LL1622: _temp1621= _temp1617.tq; goto _LL1620;
_LL1620: _temp1619= _temp1617.bounds; goto _LL1602;} else{ goto _LL1599;}
_LL1602: _temp1601= _temp1595.f2; if(( unsigned int) _temp1601 > 4u?*(( int*)
_temp1601) == Cyc_Absyn_PointerType: 0){ _LL1604: _temp1603=(( struct Cyc_Absyn_PointerType_struct*)
_temp1601)->f1; _LL1614: _temp1613=( void*) _temp1603.elt_typ; goto _LL1612;
_LL1612: _temp1611=( void*) _temp1603.rgn_typ; goto _LL1610; _LL1610: _temp1609=
_temp1603.nullable; goto _LL1608; _LL1608: _temp1607= _temp1603.tq; goto _LL1606;
_LL1606: _temp1605= _temp1603.bounds; goto _LL1598;} else{ goto _LL1599;}
_LL1599: goto _LL1600; _LL1598: if( _temp1621.q_const? ! _temp1607.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1623, _temp1609)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1623): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1609): 0){ return 0;} if( ! Cyc_Tcutil_unify( _temp1625, _temp1611)? ! Cyc_Tcenv_region_outlives(
te, _temp1625, _temp1611): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1619, _temp1605)){ struct _tuple8 _temp1630=({ struct _tuple8 _temp1629;
_temp1629.f1= Cyc_Absyn_conref_val( _temp1619); _temp1629.f2= Cyc_Absyn_conref_val(
_temp1605); _temp1629;}); void* _temp1638; void* _temp1640; void* _temp1642;
struct Cyc_Absyn_Exp* _temp1644; void* _temp1646; struct Cyc_Absyn_Exp*
_temp1648; _LL1632: _LL1641: _temp1640= _temp1630.f1; if(( unsigned int)
_temp1640 > 1u?*(( int*) _temp1640) == Cyc_Absyn_Upper_b: 0){ goto _LL1639;}
else{ goto _LL1634;} _LL1639: _temp1638= _temp1630.f2; if( _temp1638 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1633;} else{ goto _LL1634;} _LL1634: _LL1647:
_temp1646= _temp1630.f1; if(( unsigned int) _temp1646 > 1u?*(( int*) _temp1646)
== Cyc_Absyn_Upper_b: 0){ _LL1649: _temp1648=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1646)->f1; goto _LL1643;} else{ goto _LL1636;} _LL1643: _temp1642=
_temp1630.f2; if(( unsigned int) _temp1642 > 1u?*(( int*) _temp1642) == Cyc_Absyn_Upper_b:
0){ _LL1645: _temp1644=(( struct Cyc_Absyn_Upper_b_struct*) _temp1642)->f1; goto
_LL1635;} else{ goto _LL1636;} _LL1636: goto _LL1637; _LL1633: goto _LL1631;
_LL1635: if( Cyc_Evexp_eval_const_uint_exp( _temp1648) < Cyc_Evexp_eval_const_uint_exp(
_temp1644)){ return 0;} goto _LL1631; _LL1637: return 0; _LL1631:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1650=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1650->hd=( void*)({ struct _tuple8*
_temp1651=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1651->f1=
t1; _temp1651->f2= t2; _temp1651;}); _temp1650->tl= assume; _temp1650;}),
_temp1627, _temp1613); _LL1600: return 0; _LL1596:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2){
struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1); struct Cyc_List_List*
tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;( tqs2=(( struct Cyc_List_List*)
_check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*) _check_null( tqs1))->tl)){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1654; void* _temp1655; struct
Cyc_Absyn_Tqual _temp1657; struct _tuple4* _temp1652=( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( tqs1))->hd; _temp1654=* _temp1652; _LL1658:
_temp1657= _temp1654.f1; goto _LL1656; _LL1656: _temp1655= _temp1654.f2; goto
_LL1653; _LL1653: { struct _tuple4 _temp1661; void* _temp1662; struct Cyc_Absyn_Tqual
_temp1664; struct _tuple4* _temp1659=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs2))->hd; _temp1661=* _temp1659; _LL1665: _temp1664= _temp1661.f1;
goto _LL1663; _LL1663: _temp1662= _temp1661.f2; goto _LL1660; _LL1660: if(
_temp1664.q_const? Cyc_Tcutil_subtype( te, assume, _temp1655, _temp1662): 0){
continue;} else{ if( Cyc_Tcutil_unify( _temp1655, _temp1662)){ continue;} else{
return 0;}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1666= t2; struct Cyc_Absyn_Uniondecl** _temp1674; struct Cyc_List_List*
_temp1676; void* _temp1678; _LL1668: if(( unsigned int) _temp1666 > 4u?*(( int*)
_temp1666) == Cyc_Absyn_UnionType: 0){ _LL1677: _temp1676=(( struct Cyc_Absyn_UnionType_struct*)
_temp1666)->f2; goto _LL1675; _LL1675: _temp1674=(( struct Cyc_Absyn_UnionType_struct*)
_temp1666)->f3; goto _LL1669;} else{ goto _LL1670;} _LL1670: if(( unsigned int)
_temp1666 > 4u?*(( int*) _temp1666) == Cyc_Absyn_IntType: 0){ _LL1679: _temp1678=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1666)->f2; if( _temp1678 ==(
void*) Cyc_Absyn_B4){ goto _LL1671;} else{ goto _LL1672;}} else{ goto _LL1672;}
_LL1672: goto _LL1673; _LL1669: if( _temp1674 == 0){ goto _LL1667;}{ struct Cyc_Absyn_Uniondecl*
_temp1680=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp1674)); if(
_temp1680->fields == 0){ goto _LL1667;}{ struct Cyc_List_List* _temp1681=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1680->tvs, _temp1676);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1680->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1681,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL1667;}} _LL1671: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1667; _LL1673: goto
_LL1667; _LL1667:;}{ void* _temp1682= t1; struct Cyc_Absyn_PtrInfo _temp1698;
struct Cyc_Absyn_Conref* _temp1700; struct Cyc_Absyn_Tqual _temp1702; struct Cyc_Absyn_Conref*
_temp1704; void* _temp1706; void* _temp1708; struct Cyc_Absyn_Exp* _temp1710;
struct Cyc_Absyn_Tqual _temp1712; void* _temp1714; struct Cyc_Absyn_Enumdecl*
_temp1716; _LL1684: if(( unsigned int) _temp1682 > 4u?*(( int*) _temp1682) ==
Cyc_Absyn_PointerType: 0){ _LL1699: _temp1698=(( struct Cyc_Absyn_PointerType_struct*)
_temp1682)->f1; _LL1709: _temp1708=( void*) _temp1698.elt_typ; goto _LL1707;
_LL1707: _temp1706=( void*) _temp1698.rgn_typ; goto _LL1705; _LL1705: _temp1704=
_temp1698.nullable; goto _LL1703; _LL1703: _temp1702= _temp1698.tq; goto _LL1701;
_LL1701: _temp1700= _temp1698.bounds; goto _LL1685;} else{ goto _LL1686;}
_LL1686: if(( unsigned int) _temp1682 > 4u?*(( int*) _temp1682) == Cyc_Absyn_ArrayType:
0){ _LL1715: _temp1714=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1682)->f1;
goto _LL1713; _LL1713: _temp1712=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1682)->f2; goto _LL1711; _LL1711: _temp1710=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1682)->f3; goto _LL1687;} else{ goto _LL1688;} _LL1688: if(( unsigned int)
_temp1682 > 4u?*(( int*) _temp1682) == Cyc_Absyn_EnumType: 0){ _LL1717:
_temp1716=(( struct Cyc_Absyn_EnumType_struct*) _temp1682)->f2; goto _LL1689;}
else{ goto _LL1690;} _LL1690: if(( unsigned int) _temp1682 > 4u?*(( int*)
_temp1682) == Cyc_Absyn_IntType: 0){ goto _LL1691;} else{ goto _LL1692;} _LL1692:
if( _temp1682 ==( void*) Cyc_Absyn_FloatType){ goto _LL1693;} else{ goto _LL1694;}
_LL1694: if( _temp1682 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1695;} else{
goto _LL1696;} _LL1696: goto _LL1697; _LL1685:{ void* _temp1718= t2; struct Cyc_Absyn_PtrInfo
_temp1724; struct Cyc_Absyn_Conref* _temp1726; struct Cyc_Absyn_Tqual _temp1728;
struct Cyc_Absyn_Conref* _temp1730; void* _temp1732; void* _temp1734; _LL1720:
if(( unsigned int) _temp1718 > 4u?*(( int*) _temp1718) == Cyc_Absyn_PointerType:
0){ _LL1725: _temp1724=(( struct Cyc_Absyn_PointerType_struct*) _temp1718)->f1;
_LL1735: _temp1734=( void*) _temp1724.elt_typ; goto _LL1733; _LL1733: _temp1732=(
void*) _temp1724.rgn_typ; goto _LL1731; _LL1731: _temp1730= _temp1724.nullable;
goto _LL1729; _LL1729: _temp1728= _temp1724.tq; goto _LL1727; _LL1727: _temp1726=
_temp1724.bounds; goto _LL1721;} else{ goto _LL1722;} _LL1722: goto _LL1723;
_LL1721: { struct Cyc_List_List* _temp1736=({ struct Cyc_List_List* _temp1753=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1753->hd=(
void*)({ struct _tuple8* _temp1754=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp1754->f1= t1; _temp1754->f2= t2; _temp1754;}); _temp1753->tl= 0;
_temp1753;}); int _temp1737= Cyc_Tcutil_ptrsubtype( te, _temp1736, _temp1708,
_temp1734)? ! _temp1702.q_const? 1: _temp1728.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp, _temp1700, _temp1726); if( ! bounds_ok){ struct _tuple8
_temp1739=({ struct _tuple8 _temp1738; _temp1738.f1= Cyc_Absyn_conref_val(
_temp1700); _temp1738.f2= Cyc_Absyn_conref_val( _temp1726); _temp1738;}); void*
_temp1745; struct Cyc_Absyn_Exp* _temp1747; void* _temp1749; struct Cyc_Absyn_Exp*
_temp1751; _LL1741: _LL1750: _temp1749= _temp1739.f1; if(( unsigned int)
_temp1749 > 1u?*(( int*) _temp1749) == Cyc_Absyn_Upper_b: 0){ _LL1752: _temp1751=((
struct Cyc_Absyn_Upper_b_struct*) _temp1749)->f1; goto _LL1746;} else{ goto
_LL1743;} _LL1746: _temp1745= _temp1739.f2; if(( unsigned int) _temp1745 > 1u?*((
int*) _temp1745) == Cyc_Absyn_Upper_b: 0){ _LL1748: _temp1747=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1745)->f1; goto _LL1742;} else{ goto _LL1743;} _LL1743: goto _LL1744;
_LL1742: if( Cyc_Evexp_eval_const_uint_exp( _temp1751) >= Cyc_Evexp_eval_const_uint_exp(
_temp1747)){ bounds_ok= 1;} goto _LL1740; _LL1744: bounds_ok= 1; goto _LL1740;
_LL1740:;} return( bounds_ok? _temp1737: 0)? Cyc_Tcutil_unify( _temp1706,
_temp1732)? 1: Cyc_Tcenv_region_outlives( te, _temp1706, _temp1732): 0;} _LL1723:
goto _LL1719; _LL1719:;} return 0; _LL1687:{ void* _temp1755= t2; struct Cyc_Absyn_Exp*
_temp1761; struct Cyc_Absyn_Tqual _temp1763; void* _temp1765; _LL1757: if((
unsigned int) _temp1755 > 4u?*(( int*) _temp1755) == Cyc_Absyn_ArrayType: 0){
_LL1766: _temp1765=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1755)->f1;
goto _LL1764; _LL1764: _temp1763=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1755)->f2; goto _LL1762; _LL1762: _temp1761=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1755)->f3; goto _LL1758;} else{ goto _LL1759;} _LL1759: goto _LL1760;
_LL1758: { int okay; okay=( _temp1710 != 0? _temp1761 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1710)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1761)): 0; return( okay? Cyc_Tcutil_unify(
_temp1714, _temp1765): 0)? ! _temp1712.q_const? 1: _temp1763.q_const: 0;}
_LL1760: return 0; _LL1756:;} return 0; _LL1689:{ void* _temp1767= t2; struct
Cyc_Absyn_Enumdecl* _temp1773; _LL1769: if(( unsigned int) _temp1767 > 4u?*((
int*) _temp1767) == Cyc_Absyn_EnumType: 0){ _LL1774: _temp1773=(( struct Cyc_Absyn_EnumType_struct*)
_temp1767)->f2; goto _LL1770;} else{ goto _LL1771;} _LL1771: goto _LL1772;
_LL1770: if(((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1716))->fields !=
0?(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1773))->fields != 0: 0)?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*) _check_null(
_temp1716))->fields))->v) >=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp1773))->fields))->v): 0){ return 1;} goto _LL1768; _LL1772:
goto _LL1768; _LL1768:;} goto _LL1691; _LL1691: goto _LL1693; _LL1693: goto
_LL1695; _LL1695: return Cyc_Tcutil_coerceable( t2); _LL1697: return 0; _LL1683:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp1775= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp1776=(
struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp1776[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp1777; _temp1777.tag= Cyc_Absyn_Cast_e;
_temp1777.f1=( void*) t; _temp1777.f2= _temp1775; _temp1777;}); _temp1776;})));
e->topt=({ struct Cyc_Core_Opt* _temp1778=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1778->v=( void*) t; _temp1778;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp1779= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1781: if(( unsigned int) _temp1779
> 4u?*(( int*) _temp1779) == Cyc_Absyn_EnumType: 0){ goto _LL1782;} else{ goto
_LL1783;} _LL1783: if(( unsigned int) _temp1779 > 4u?*(( int*) _temp1779) == Cyc_Absyn_IntType:
0){ goto _LL1784;} else{ goto _LL1785;} _LL1785: if(( unsigned int) _temp1779 >
4u?*(( int*) _temp1779) == Cyc_Absyn_Evar: 0){ goto _LL1786;} else{ goto _LL1787;}
_LL1787: goto _LL1788; _LL1782: goto _LL1784; _LL1784: return 1; _LL1786: if(
Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){
return 1;} return 0; _LL1788: return 0; _LL1780:;} int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void*
_temp1789= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1791: if( _temp1789 ==( void*) Cyc_Absyn_FloatType){ goto _LL1792;} else{
goto _LL1793;} _LL1793: if( _temp1789 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1794;} else{ goto _LL1795;} _LL1795: goto _LL1796; _LL1792: goto _LL1794;
_LL1794: return 1; _LL1796: return 0; _LL1790:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp1797= Cyc_Tcutil_compress( t); _LL1799: if(( unsigned int)
_temp1797 > 4u?*(( int*) _temp1797) == Cyc_Absyn_FnType: 0){ goto _LL1800;}
else{ goto _LL1801;} _LL1801: goto _LL1802; _LL1800: return 1; _LL1802: return 0;
_LL1798:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple8 _temp1804=({ struct _tuple8 _temp1803; _temp1803.f1= t1; _temp1803.f2=
t2; _temp1803;}); void* _temp1828; void* _temp1830; void* _temp1832; void*
_temp1834; void* _temp1836; void* _temp1838; void* _temp1840; void* _temp1842;
void* _temp1844; void* _temp1846; void* _temp1848; void* _temp1850; void*
_temp1852; void* _temp1854; void* _temp1856; void* _temp1858; void* _temp1860;
void* _temp1862; void* _temp1864; void* _temp1866; void* _temp1868; void*
_temp1870; _LL1806: _LL1829: _temp1828= _temp1804.f1; if( _temp1828 ==( void*)
Cyc_Absyn_DoubleType){ goto _LL1807;} else{ goto _LL1808;} _LL1808: _LL1831:
_temp1830= _temp1804.f2; if( _temp1830 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1809;} else{ goto _LL1810;} _LL1810: _LL1833: _temp1832= _temp1804.f1; if(
_temp1832 ==( void*) Cyc_Absyn_FloatType){ goto _LL1811;} else{ goto _LL1812;}
_LL1812: _LL1835: _temp1834= _temp1804.f2; if( _temp1834 ==( void*) Cyc_Absyn_FloatType){
goto _LL1813;} else{ goto _LL1814;} _LL1814: _LL1837: _temp1836= _temp1804.f1;
if(( unsigned int) _temp1836 > 4u?*(( int*) _temp1836) == Cyc_Absyn_IntType: 0){
_LL1841: _temp1840=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1836)->f1;
if( _temp1840 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1839;} else{ goto _LL1816;}
_LL1839: _temp1838=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1836)->f2;
if( _temp1838 ==( void*) Cyc_Absyn_B8){ goto _LL1815;} else{ goto _LL1816;}}
else{ goto _LL1816;} _LL1816: _LL1843: _temp1842= _temp1804.f2; if((
unsigned int) _temp1842 > 4u?*(( int*) _temp1842) == Cyc_Absyn_IntType: 0){
_LL1847: _temp1846=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1842)->f1;
if( _temp1846 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1845;} else{ goto _LL1818;}
_LL1845: _temp1844=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1842)->f2;
if( _temp1844 ==( void*) Cyc_Absyn_B8){ goto _LL1817;} else{ goto _LL1818;}}
else{ goto _LL1818;} _LL1818: _LL1849: _temp1848= _temp1804.f1; if((
unsigned int) _temp1848 > 4u?*(( int*) _temp1848) == Cyc_Absyn_IntType: 0){
_LL1853: _temp1852=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1848)->f1;
if( _temp1852 ==( void*) Cyc_Absyn_Signed){ goto _LL1851;} else{ goto _LL1820;}
_LL1851: _temp1850=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1848)->f2;
if( _temp1850 ==( void*) Cyc_Absyn_B8){ goto _LL1819;} else{ goto _LL1820;}}
else{ goto _LL1820;} _LL1820: _LL1855: _temp1854= _temp1804.f2; if((
unsigned int) _temp1854 > 4u?*(( int*) _temp1854) == Cyc_Absyn_IntType: 0){
_LL1859: _temp1858=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1854)->f1;
if( _temp1858 ==( void*) Cyc_Absyn_Signed){ goto _LL1857;} else{ goto _LL1822;}
_LL1857: _temp1856=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1854)->f2;
if( _temp1856 ==( void*) Cyc_Absyn_B8){ goto _LL1821;} else{ goto _LL1822;}}
else{ goto _LL1822;} _LL1822: _LL1861: _temp1860= _temp1804.f1; if((
unsigned int) _temp1860 > 4u?*(( int*) _temp1860) == Cyc_Absyn_IntType: 0){
_LL1865: _temp1864=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1860)->f1;
if( _temp1864 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1863;} else{ goto _LL1824;}
_LL1863: _temp1862=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1860)->f2;
if( _temp1862 ==( void*) Cyc_Absyn_B4){ goto _LL1823;} else{ goto _LL1824;}}
else{ goto _LL1824;} _LL1824: _LL1867: _temp1866= _temp1804.f2; if((
unsigned int) _temp1866 > 4u?*(( int*) _temp1866) == Cyc_Absyn_IntType: 0){
_LL1871: _temp1870=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1866)->f1;
if( _temp1870 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1869;} else{ goto _LL1826;}
_LL1869: _temp1868=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1866)->f2;
if( _temp1868 ==( void*) Cyc_Absyn_B4){ goto _LL1825;} else{ goto _LL1826;}}
else{ goto _LL1826;} _LL1826: goto _LL1827; _LL1807: goto _LL1809; _LL1809:
return( void*) Cyc_Absyn_DoubleType; _LL1811: goto _LL1813; _LL1813: return(
void*) Cyc_Absyn_FloatType; _LL1815: goto _LL1817; _LL1817: return Cyc_Absyn_ulonglong_t;
_LL1819: goto _LL1821; _LL1821: return Cyc_Absyn_slonglong_t; _LL1823: goto
_LL1825; _LL1825: return Cyc_Absyn_uint_t; _LL1827: return Cyc_Absyn_sint_t;
_LL1805:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp1872=( void*) e->r; struct Cyc_Core_Opt* _temp1878; _LL1874: if(*(( int*)
_temp1872) == Cyc_Absyn_AssignOp_e){ _LL1879: _temp1878=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1872)->f2; if( _temp1878 == 0){ goto _LL1875;} else{ goto _LL1876;}} else{
goto _LL1876;} _LL1876: goto _LL1877; _LL1875: Cyc_Tcutil_warn( e->loc, _tag_arr("assignment in test",
sizeof( unsigned char), 19u)); goto _LL1873; _LL1877: goto _LL1873; _LL1873:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 == c2){ return 1;} if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1880=(
struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp1880[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp1881; _temp1881.tag=
Cyc_Absyn_Forward_constr; _temp1881.f1= c2; _temp1881;}); _temp1880;}))); return
1;} else{ if(( void*) c2->v ==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1882=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1882[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1883; _temp1883.tag= Cyc_Absyn_Forward_constr;
_temp1883.f1= c1; _temp1883;}); _temp1882;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1884=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1884[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1885; _temp1885.tag= Cyc_Absyn_Forward_constr; _temp1885.f1= c1; _temp1885;});
_temp1884;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1886=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1886[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1887; _temp1887.tag= Cyc_Absyn_Forward_constr;
_temp1887.f1= c2; _temp1887;}); _temp1886;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp1888=( int*) GC_malloc_atomic( sizeof( int)); _temp1888[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp1888;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr) xprintf("#%d", i); return({ struct Cyc_Absyn_Tvar*
_temp1889=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp1889->name=({ struct _tagged_arr* _temp1890=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1890[ 0]= s; _temp1890;});
_temp1889->identity= 0; _temp1889->kind= Cyc_Absyn_new_conref( k); _temp1889;});}
int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){ struct _tagged_arr
_temp1891=* t->name; return*(( const unsigned char*) _check_unknown_subscript(
_temp1891, sizeof( unsigned char), 0)) =='#';} void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct
_tagged_arr _temp1892= Cyc_String_strconcat( _tag_arr("`", sizeof( unsigned char),
2u),* t->name);*(( unsigned char*) _check_unknown_subscript( _temp1892, sizeof(
unsigned char), 1))='t'; t->name=({ struct _tagged_arr* _temp1893=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1893[ 0]=(
struct _tagged_arr) _temp1892; _temp1893;});}} struct _tuple13{ struct
_tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static struct _tuple2*
Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({ struct _tuple2* _temp1894=(
struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp1894->f1=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp1895=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1895->v=( void*)(* x).f1; _temp1895;}); _temp1894->f2=(* x).f2;
_temp1894->f3=(* x).f3; _temp1894;});} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*
fd){ if( fd->cached_typ == 0){ struct Cyc_List_List* _temp1896= 0;{ struct Cyc_List_List*
atts= fd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ if( Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd)){ _temp1896=({ struct Cyc_List_List* _temp1897=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1897->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); _temp1897->tl=
_temp1896; _temp1897;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1898=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1898[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1899; _temp1899.tag= Cyc_Absyn_FnType;
_temp1899.f1=({ struct Cyc_Absyn_FnInfo _temp1900; _temp1900.tvars= fd->tvs;
_temp1900.effect= fd->effect; _temp1900.ret_typ=( void*)(( void*) fd->ret_type);
_temp1900.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp1900.c_varargs= fd->c_varargs; _temp1900.cyc_varargs= fd->cyc_varargs;
_temp1900.rgn_po= fd->rgn_po; _temp1900.attributes= _temp1896; _temp1900;});
_temp1899;}); _temp1898;});} return( void*)(( struct Cyc_Core_Opt*) _check_null(
fd->cached_typ))->v;} struct _tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void*
f3; } ; static void* Cyc_Tcutil_fst_fdarg( struct _tuple14* t){ return(* t).f1;}
void* Cyc_Tcutil_snd_tqt( struct _tuple4* t){ return(* t).f2;} static struct
_tuple4* Cyc_Tcutil_map2_tq( struct _tuple4* pr, void* t){ return({ struct
_tuple4* _temp1901=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1901->f1=(* pr).f1; _temp1901->f2= t; _temp1901;});} void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl*
fd){ void* t= Cyc_Tcutil_fndecl2typ( fd); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp1902= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp1908; struct Cyc_List_List* _temp1910; struct Cyc_List_List* _temp1912;
struct Cyc_Absyn_VarargInfo* _temp1914; int _temp1916; struct Cyc_List_List*
_temp1918; void* _temp1920; struct Cyc_Core_Opt* _temp1922; struct Cyc_List_List*
_temp1924; _LL1904: if(( unsigned int) _temp1902 > 4u?*(( int*) _temp1902) ==
Cyc_Absyn_FnType: 0){ _LL1909: _temp1908=(( struct Cyc_Absyn_FnType_struct*)
_temp1902)->f1; _LL1925: _temp1924= _temp1908.tvars; goto _LL1923; _LL1923:
_temp1922= _temp1908.effect; goto _LL1921; _LL1921: _temp1920=( void*) _temp1908.ret_typ;
goto _LL1919; _LL1919: _temp1918= _temp1908.args; goto _LL1917; _LL1917:
_temp1916= _temp1908.c_varargs; goto _LL1915; _LL1915: _temp1914= _temp1908.cyc_varargs;
goto _LL1913; _LL1913: _temp1912= _temp1908.rgn_po; goto _LL1911; _LL1911:
_temp1910= _temp1908.attributes; goto _LL1905;} else{ goto _LL1906;} _LL1906:
goto _LL1907; _LL1905: fd->tvs= _temp1924; fd->effect= _temp1922; goto _LL1903;
_LL1907:(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u)); return; _LL1903:;}{ struct _RegionHandle
_temp1926= _new_region(); struct _RegionHandle* r=& _temp1926; _push_region( r);
Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tagged_arr*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)(
r,( struct _tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args),
loc, _tag_arr("function declaration has repeated parameter", sizeof(
unsigned char), 44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt*
_temp1927=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1927->v=( void*) t; _temp1927;});} struct _tuple15{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Tqual f2; } ; struct _tuple16{ struct _tuple15* f1; void* f2; }
; static struct _tuple16* Cyc_Tcutil_substitute_f1( struct _RegionHandle* rgn,
struct _tuple2* y){ return({ struct _tuple16* _temp1928=( struct _tuple16*)
_region_malloc( rgn, sizeof( struct _tuple16)); _temp1928->f1=({ struct _tuple15*
_temp1929=( struct _tuple15*) _region_malloc( rgn, sizeof( struct _tuple15));
_temp1929->f1=(* y).f1; _temp1929->f2=(* y).f2; _temp1929;}); _temp1928->f2=(* y).f3;
_temp1928;});} static struct _tuple2* Cyc_Tcutil_substitute_f2( struct _tuple16*
w){ void* _temp1932; struct _tuple15* _temp1934; struct _tuple16 _temp1930=* w;
_LL1935: _temp1934= _temp1930.f1; goto _LL1933; _LL1933: _temp1932= _temp1930.f2;
goto _LL1931; _LL1931: { struct Cyc_Absyn_Tqual _temp1938; struct Cyc_Core_Opt*
_temp1940; struct _tuple15 _temp1936=* _temp1934; _LL1941: _temp1940= _temp1936.f1;
goto _LL1939; _LL1939: _temp1938= _temp1936.f2; goto _LL1937; _LL1937: return({
struct _tuple2* _temp1942=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp1942->f1= _temp1940; _temp1942->f2= _temp1938; _temp1942->f3= _temp1932;
_temp1942;});}} static void* Cyc_Tcutil_field_type( struct Cyc_Absyn_Structfield*
f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield* Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Structfield* f, void* t){ return({ struct Cyc_Absyn_Structfield*
_temp1943=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp1943->name= f->name; _temp1943->tq= f->tq; _temp1943->type=( void*) t;
_temp1943->width= f->width; _temp1943->attributes= f->attributes; _temp1943;});}
static struct Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, struct Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle* rgn, struct Cyc_List_List* inst, void* t){ void* _temp1944=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp1990; struct Cyc_Absyn_Structdecl**
_temp1992; struct Cyc_List_List* _temp1994; struct _tuple1* _temp1996; struct
Cyc_Absyn_Uniondecl** _temp1998; struct Cyc_List_List* _temp2000; struct _tuple1*
_temp2002; struct Cyc_Absyn_TunionInfo _temp2004; void* _temp2006; struct Cyc_List_List*
_temp2008; void* _temp2010; struct Cyc_Absyn_TunionFieldInfo _temp2012; struct
Cyc_List_List* _temp2014; void* _temp2016; struct Cyc_Core_Opt* _temp2018;
struct Cyc_List_List* _temp2020; struct _tuple1* _temp2022; struct Cyc_Absyn_Exp*
_temp2024; struct Cyc_Absyn_Tqual _temp2026; void* _temp2028; struct Cyc_Absyn_PtrInfo
_temp2030; struct Cyc_Absyn_Conref* _temp2032; struct Cyc_Absyn_Tqual _temp2034;
struct Cyc_Absyn_Conref* _temp2036; void* _temp2038; void* _temp2040; struct Cyc_Absyn_FnInfo
_temp2042; struct Cyc_List_List* _temp2044; struct Cyc_List_List* _temp2046;
struct Cyc_Absyn_VarargInfo* _temp2048; int _temp2050; struct Cyc_List_List*
_temp2052; void* _temp2054; struct Cyc_Core_Opt* _temp2056; struct Cyc_List_List*
_temp2058; struct Cyc_List_List* _temp2060; struct Cyc_List_List* _temp2062;
struct Cyc_List_List* _temp2064; struct Cyc_Core_Opt* _temp2066; void* _temp2068;
void* _temp2070; struct Cyc_List_List* _temp2072; _LL1946: if(( unsigned int)
_temp1944 > 4u?*(( int*) _temp1944) == Cyc_Absyn_VarType: 0){ _LL1991: _temp1990=((
struct Cyc_Absyn_VarType_struct*) _temp1944)->f1; goto _LL1947;} else{ goto
_LL1948;} _LL1948: if(( unsigned int) _temp1944 > 4u?*(( int*) _temp1944) == Cyc_Absyn_StructType:
0){ _LL1997: _temp1996=(( struct Cyc_Absyn_StructType_struct*) _temp1944)->f1;
goto _LL1995; _LL1995: _temp1994=(( struct Cyc_Absyn_StructType_struct*)
_temp1944)->f2; goto _LL1993; _LL1993: _temp1992=(( struct Cyc_Absyn_StructType_struct*)
_temp1944)->f3; goto _LL1949;} else{ goto _LL1950;} _LL1950: if(( unsigned int)
_temp1944 > 4u?*(( int*) _temp1944) == Cyc_Absyn_UnionType: 0){ _LL2003:
_temp2002=(( struct Cyc_Absyn_UnionType_struct*) _temp1944)->f1; goto _LL2001;
_LL2001: _temp2000=(( struct Cyc_Absyn_UnionType_struct*) _temp1944)->f2; goto
_LL1999; _LL1999: _temp1998=(( struct Cyc_Absyn_UnionType_struct*) _temp1944)->f3;
goto _LL1951;} else{ goto _LL1952;} _LL1952: if(( unsigned int) _temp1944 > 4u?*((
int*) _temp1944) == Cyc_Absyn_TunionType: 0){ _LL2005: _temp2004=(( struct Cyc_Absyn_TunionType_struct*)
_temp1944)->f1; _LL2011: _temp2010=( void*) _temp2004.tunion_info; goto _LL2009;
_LL2009: _temp2008= _temp2004.targs; goto _LL2007; _LL2007: _temp2006=( void*)
_temp2004.rgn; goto _LL1953;} else{ goto _LL1954;} _LL1954: if(( unsigned int)
_temp1944 > 4u?*(( int*) _temp1944) == Cyc_Absyn_TunionFieldType: 0){ _LL2013:
_temp2012=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1944)->f1; _LL2017:
_temp2016=( void*) _temp2012.field_info; goto _LL2015; _LL2015: _temp2014=
_temp2012.targs; goto _LL1955;} else{ goto _LL1956;} _LL1956: if(( unsigned int)
_temp1944 > 4u?*(( int*) _temp1944) == Cyc_Absyn_TypedefType: 0){ _LL2023:
_temp2022=(( struct Cyc_Absyn_TypedefType_struct*) _temp1944)->f1; goto _LL2021;
_LL2021: _temp2020=(( struct Cyc_Absyn_TypedefType_struct*) _temp1944)->f2; goto
_LL2019; _LL2019: _temp2018=(( struct Cyc_Absyn_TypedefType_struct*) _temp1944)->f3;
goto _LL1957;} else{ goto _LL1958;} _LL1958: if(( unsigned int) _temp1944 > 4u?*((
int*) _temp1944) == Cyc_Absyn_ArrayType: 0){ _LL2029: _temp2028=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1944)->f1; goto _LL2027; _LL2027:
_temp2026=(( struct Cyc_Absyn_ArrayType_struct*) _temp1944)->f2; goto _LL2025;
_LL2025: _temp2024=(( struct Cyc_Absyn_ArrayType_struct*) _temp1944)->f3; goto
_LL1959;} else{ goto _LL1960;} _LL1960: if(( unsigned int) _temp1944 > 4u?*((
int*) _temp1944) == Cyc_Absyn_PointerType: 0){ _LL2031: _temp2030=(( struct Cyc_Absyn_PointerType_struct*)
_temp1944)->f1; _LL2041: _temp2040=( void*) _temp2030.elt_typ; goto _LL2039;
_LL2039: _temp2038=( void*) _temp2030.rgn_typ; goto _LL2037; _LL2037: _temp2036=
_temp2030.nullable; goto _LL2035; _LL2035: _temp2034= _temp2030.tq; goto _LL2033;
_LL2033: _temp2032= _temp2030.bounds; goto _LL1961;} else{ goto _LL1962;}
_LL1962: if(( unsigned int) _temp1944 > 4u?*(( int*) _temp1944) == Cyc_Absyn_FnType:
0){ _LL2043: _temp2042=(( struct Cyc_Absyn_FnType_struct*) _temp1944)->f1;
_LL2059: _temp2058= _temp2042.tvars; goto _LL2057; _LL2057: _temp2056= _temp2042.effect;
goto _LL2055; _LL2055: _temp2054=( void*) _temp2042.ret_typ; goto _LL2053;
_LL2053: _temp2052= _temp2042.args; goto _LL2051; _LL2051: _temp2050= _temp2042.c_varargs;
goto _LL2049; _LL2049: _temp2048= _temp2042.cyc_varargs; goto _LL2047; _LL2047:
_temp2046= _temp2042.rgn_po; goto _LL2045; _LL2045: _temp2044= _temp2042.attributes;
goto _LL1963;} else{ goto _LL1964;} _LL1964: if(( unsigned int) _temp1944 > 4u?*((
int*) _temp1944) == Cyc_Absyn_TupleType: 0){ _LL2061: _temp2060=(( struct Cyc_Absyn_TupleType_struct*)
_temp1944)->f1; goto _LL1965;} else{ goto _LL1966;} _LL1966: if(( unsigned int)
_temp1944 > 4u?*(( int*) _temp1944) == Cyc_Absyn_AnonStructType: 0){ _LL2063:
_temp2062=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1944)->f1; goto
_LL1967;} else{ goto _LL1968;} _LL1968: if(( unsigned int) _temp1944 > 4u?*((
int*) _temp1944) == Cyc_Absyn_AnonUnionType: 0){ _LL2065: _temp2064=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp1944)->f1; goto _LL1969;} else{ goto
_LL1970;} _LL1970: if(( unsigned int) _temp1944 > 4u?*(( int*) _temp1944) == Cyc_Absyn_Evar:
0){ _LL2067: _temp2066=(( struct Cyc_Absyn_Evar_struct*) _temp1944)->f2; goto
_LL1971;} else{ goto _LL1972;} _LL1972: if(( unsigned int) _temp1944 > 4u?*((
int*) _temp1944) == Cyc_Absyn_RgnHandleType: 0){ _LL2069: _temp2068=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp1944)->f1; goto _LL1973;} else{
goto _LL1974;} _LL1974: if(( unsigned int) _temp1944 > 4u?*(( int*) _temp1944)
== Cyc_Absyn_EnumType: 0){ goto _LL1975;} else{ goto _LL1976;} _LL1976: if(
_temp1944 ==( void*) Cyc_Absyn_VoidType){ goto _LL1977;} else{ goto _LL1978;}
_LL1978: if(( unsigned int) _temp1944 > 4u?*(( int*) _temp1944) == Cyc_Absyn_IntType:
0){ goto _LL1979;} else{ goto _LL1980;} _LL1980: if( _temp1944 ==( void*) Cyc_Absyn_FloatType){
goto _LL1981;} else{ goto _LL1982;} _LL1982: if( _temp1944 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1983;} else{ goto _LL1984;} _LL1984: if( _temp1944 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL1985;} else{ goto _LL1986;} _LL1986: if(( unsigned int) _temp1944 > 4u?*((
int*) _temp1944) == Cyc_Absyn_AccessEff: 0){ _LL2071: _temp2070=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp1944)->f1; goto _LL1987;} else{ goto
_LL1988;} _LL1988: if(( unsigned int) _temp1944 > 4u?*(( int*) _temp1944) == Cyc_Absyn_JoinEff:
0){ _LL2073: _temp2072=(( struct Cyc_Absyn_JoinEff_struct*) _temp1944)->f1; goto
_LL1989;} else{ goto _LL1945;} _LL1947: { struct _handler_cons _temp2074;
_push_handler(& _temp2074);{ int _temp2076= 0; if( setjmp( _temp2074.handler)){
_temp2076= 1;} if( ! _temp2076){{ void* _temp2077=(( void*(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp, inst, _temp1990);
_npop_handler( 0u); return _temp2077;}; _pop_handler();} else{ void* _temp2075=(
void*) _exn_thrown; void* _temp2079= _temp2075; _LL2081: if( _temp2079 == Cyc_Core_Not_found){
goto _LL2082;} else{ goto _LL2083;} _LL2083: goto _LL2084; _LL2082: return t;
_LL2084:( void) _throw( _temp2079); _LL2080:;}}} _LL1949: { struct Cyc_List_List*
_temp2085= Cyc_Tcutil_substs( rgn, inst, _temp1994); return _temp2085 ==
_temp1994? t:( void*)({ struct Cyc_Absyn_StructType_struct* _temp2086=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2086[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2087; _temp2087.tag=
Cyc_Absyn_StructType; _temp2087.f1= _temp1996; _temp2087.f2= _temp2085;
_temp2087.f3= _temp1992; _temp2087;}); _temp2086;});} _LL1951: { struct Cyc_List_List*
_temp2088= Cyc_Tcutil_substs( rgn, inst, _temp2000); return _temp2088 ==
_temp2000? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2089=( struct
Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2089[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2090; _temp2090.tag= Cyc_Absyn_UnionType;
_temp2090.f1= _temp2002; _temp2090.f2= _temp2088; _temp2090.f3= _temp1998;
_temp2090;}); _temp2089;});} _LL1953: { struct Cyc_List_List* _temp2091= Cyc_Tcutil_substs(
rgn, inst, _temp2008); void* _temp2092= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2006); return( _temp2091 == _temp2008? _temp2092 == _temp2006: 0)? t:( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2093=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2093[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2094; _temp2094.tag= Cyc_Absyn_TunionType;
_temp2094.f1=({ struct Cyc_Absyn_TunionInfo _temp2095; _temp2095.tunion_info=(
void*) _temp2010; _temp2095.targs= _temp2091; _temp2095.rgn=( void*) _temp2092;
_temp2095;}); _temp2094;}); _temp2093;});} _LL1955: { struct Cyc_List_List*
_temp2096= Cyc_Tcutil_substs( rgn, inst, _temp2014); return _temp2096 ==
_temp2014? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2097=(
struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2097[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2098; _temp2098.tag=
Cyc_Absyn_TunionFieldType; _temp2098.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2099; _temp2099.field_info=( void*) _temp2016; _temp2099.targs= _temp2096;
_temp2099;}); _temp2098;}); _temp2097;});} _LL1957: { struct Cyc_List_List*
_temp2100= Cyc_Tcutil_substs( rgn, inst, _temp2020); return _temp2100 ==
_temp2020? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2101=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2101[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2102; _temp2102.tag=
Cyc_Absyn_TypedefType; _temp2102.f1= _temp2022; _temp2102.f2= _temp2100;
_temp2102.f3= _temp2018; _temp2102;}); _temp2101;});} _LL1959: { void* _temp2103=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2028); return _temp2103 == _temp2028? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2104=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2104[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2105; _temp2105.tag= Cyc_Absyn_ArrayType;
_temp2105.f1=( void*) _temp2103; _temp2105.f2= _temp2026; _temp2105.f3=
_temp2024; _temp2105;}); _temp2104;});} _LL1961: { void* _temp2106= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2040); void* _temp2107= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2038); if( _temp2106 == _temp2040? _temp2107 == _temp2038: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2108=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2108[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2109; _temp2109.tag= Cyc_Absyn_PointerType;
_temp2109.f1=({ struct Cyc_Absyn_PtrInfo _temp2110; _temp2110.elt_typ=( void*)
_temp2106; _temp2110.rgn_typ=( void*) _temp2107; _temp2110.nullable= _temp2036;
_temp2110.tq= _temp2034; _temp2110.bounds= _temp2032; _temp2110;}); _temp2109;});
_temp2108;});} _LL1963:{ struct Cyc_List_List* _temp2111= _temp2058; for( 0;
_temp2111 != 0; _temp2111=(( struct Cyc_List_List*) _check_null( _temp2111))->tl){
inst=({ struct Cyc_List_List* _temp2112=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2112->hd=( void*)({ struct _tuple5*
_temp2113=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2113->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2111))->hd; _temp2113->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2114=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2114[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2115; _temp2115.tag= Cyc_Absyn_VarType;
_temp2115.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2111))->hd; _temp2115;}); _temp2114;}); _temp2113;}); _temp2112->tl= inst;
_temp2112;});}}{ struct Cyc_List_List* _temp2118; struct Cyc_List_List*
_temp2120; struct _tuple0 _temp2116=(( struct _tuple0(*)( struct _RegionHandle*
r3, struct _RegionHandle* r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2052)); _LL2121:
_temp2120= _temp2116.f1; goto _LL2119; _LL2119: _temp2118= _temp2116.f2; goto
_LL2117; _LL2117: { struct Cyc_List_List* _temp2122= Cyc_Tcutil_substs( rgn,
inst, _temp2118); struct Cyc_List_List* _temp2123=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2120, _temp2122)); struct Cyc_Core_Opt* eff2; if( _temp2056 == 0){
eff2= 0;} else{ void* _temp2124= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2056))->v); if( _temp2124 ==( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2056))->v){ eff2= _temp2056;} else{ eff2=({
struct Cyc_Core_Opt* _temp2125=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2125->v=( void*) _temp2124; _temp2125;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2048 == 0){ cyc_varargs2= 0;} else{ int _temp2128; void*
_temp2130; void* _temp2132; struct Cyc_Absyn_Tqual _temp2134; struct Cyc_Core_Opt*
_temp2136; struct Cyc_Absyn_VarargInfo _temp2126=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2048)); _LL2137: _temp2136= _temp2126.name; goto _LL2135;
_LL2135: _temp2134= _temp2126.tq; goto _LL2133; _LL2133: _temp2132=( void*)
_temp2126.type; goto _LL2131; _LL2131: _temp2130=( void*) _temp2126.rgn; goto
_LL2129; _LL2129: _temp2128= _temp2126.inject; goto _LL2127; _LL2127: { void*
_temp2138= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2132); void* _temp2139= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2130); if( _temp2138 == _temp2132? _temp2130 == _temp2139: 0){
cyc_varargs2= _temp2048;} else{ cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp2140=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2140->name= _temp2136; _temp2140->tq= _temp2134; _temp2140->type=( void*)
_temp2138; _temp2140->rgn=( void*) _temp2139; _temp2140->inject= _temp2128;
_temp2140;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2143;
struct Cyc_List_List* _temp2145; struct _tuple0 _temp2141= Cyc_List_rsplit( rgn,
rgn, _temp2046); _LL2146: _temp2145= _temp2141.f1; goto _LL2144; _LL2144:
_temp2143= _temp2141.f2; goto _LL2142; _LL2142: { struct Cyc_List_List*
_temp2147= Cyc_Tcutil_substs( rgn, inst, _temp2145); struct Cyc_List_List*
_temp2148= Cyc_Tcutil_substs( rgn, inst, _temp2143); if( _temp2147 == _temp2145?
_temp2148 == _temp2143: 0){ rgn_po2= _temp2046;} else{ rgn_po2= Cyc_List_zip(
_temp2147, _temp2148);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2149=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2149[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2150; _temp2150.tag= Cyc_Absyn_FnType;
_temp2150.f1=({ struct Cyc_Absyn_FnInfo _temp2151; _temp2151.tvars= _temp2058;
_temp2151.effect= eff2; _temp2151.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2054); _temp2151.args= _temp2123; _temp2151.c_varargs= _temp2050;
_temp2151.cyc_varargs= cyc_varargs2; _temp2151.rgn_po= rgn_po2; _temp2151.attributes=
_temp2044; _temp2151;}); _temp2150;}); _temp2149;});}}}}} _LL1965: { struct Cyc_List_List*
_temp2152=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2060); struct Cyc_List_List* _temp2153= Cyc_Tcutil_substs( rgn, inst,
_temp2152); if( _temp2153 == _temp2152){ return t;}{ struct Cyc_List_List*
_temp2154=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2060, _temp2153); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2155=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2155[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2156; _temp2156.tag= Cyc_Absyn_TupleType;
_temp2156.f1= _temp2154; _temp2156;}); _temp2155;});}} _LL1967: { struct Cyc_List_List*
_temp2157=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2062); struct Cyc_List_List* _temp2158= Cyc_Tcutil_substs( rgn, inst,
_temp2157); if( _temp2158 == _temp2157){ return t;}{ struct Cyc_List_List*
_temp2159=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2062, _temp2158); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2160=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2160[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2161; _temp2161.tag= Cyc_Absyn_AnonStructType;
_temp2161.f1= _temp2159; _temp2161;}); _temp2160;});}} _LL1969: { struct Cyc_List_List*
_temp2162=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2064); struct Cyc_List_List* _temp2163= Cyc_Tcutil_substs( rgn, inst,
_temp2162); if( _temp2163 == _temp2162){ return t;}{ struct Cyc_List_List*
_temp2164=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2064, _temp2163); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2165=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2165[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2166; _temp2166.tag= Cyc_Absyn_AnonStructType;
_temp2166.f1= _temp2164; _temp2166;}); _temp2165;});}} _LL1971: if( _temp2066 !=
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2066))->v);} else{ return t;} _LL1973: { void* _temp2167= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2068); return _temp2167 == _temp2068? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2168=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2168[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2169; _temp2169.tag= Cyc_Absyn_RgnHandleType; _temp2169.f1=( void*)
_temp2167; _temp2169;}); _temp2168;});} _LL1975: return t; _LL1977: return t;
_LL1979: return t; _LL1981: return t; _LL1983: return t; _LL1985: return t;
_LL1987: { void* _temp2170= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2070);
return _temp2170 == _temp2070? t:( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp2171=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2171[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2172; _temp2172.tag= Cyc_Absyn_AccessEff;
_temp2172.f1=( void*) _temp2170; _temp2172;}); _temp2171;});} _LL1989: { struct
Cyc_List_List* _temp2173= Cyc_Tcutil_substs( rgn, inst, _temp2072); return
_temp2173 == _temp2072? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2174=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2174[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2175; _temp2175.tag= Cyc_Absyn_JoinEff;
_temp2175.f1= _temp2173; _temp2175;}); _temp2174;});} _LL1945:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2176=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2177=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2178= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2176); struct Cyc_List_List* _temp2179= Cyc_Tcutil_substs( rgn,
inst, _temp2177); if( _temp2176 == _temp2178? _temp2177 == _temp2179: 0){ return
ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2180=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2180->hd=( void*)
_temp2178; _temp2180->tl= _temp2179; _temp2180;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2224=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2224[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2225; _temp2225.tag= Cyc_Absyn_Const_e; _temp2225.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2225;}); _temp2224;}), loc);{ void*
_temp2181= Cyc_Tcutil_compress( t); void* _temp2195; void* _temp2197; void*
_temp2199; void* _temp2201; _LL2183: if(( unsigned int) _temp2181 > 4u?*(( int*)
_temp2181) == Cyc_Absyn_PointerType: 0){ goto _LL2184;} else{ goto _LL2185;}
_LL2185: if(( unsigned int) _temp2181 > 4u?*(( int*) _temp2181) == Cyc_Absyn_IntType:
0){ _LL2198: _temp2197=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2181)->f1;
goto _LL2196; _LL2196: _temp2195=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2181)->f2; if( _temp2195 ==( void*) Cyc_Absyn_B1){ goto _LL2186;} else{
goto _LL2187;}} else{ goto _LL2187;} _LL2187: if(( unsigned int) _temp2181 > 4u?*((
int*) _temp2181) == Cyc_Absyn_IntType: 0){ _LL2202: _temp2201=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2181)->f1; goto _LL2200; _LL2200: _temp2199=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2181)->f2; goto _LL2188;} else{
goto _LL2189;} _LL2189: if( _temp2181 ==( void*) Cyc_Absyn_FloatType){ goto
_LL2190;} else{ goto _LL2191;} _LL2191: if( _temp2181 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2192;} else{ goto _LL2193;} _LL2193: goto _LL2194; _LL2184: goto _LL2182;
_LL2186:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2203=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2203[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2204; _temp2204.tag= Cyc_Absyn_Const_e;
_temp2204.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2205=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2205[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2206; _temp2206.tag= Cyc_Absyn_Char_c;
_temp2206.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2206.f2='\000'; _temp2206;});
_temp2205;})); _temp2204;}); _temp2203;}))); goto _LL2182; _LL2188:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2207=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2207[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2208; _temp2208.tag= Cyc_Absyn_Const_e; _temp2208.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2209=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2209[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2210; _temp2210.tag= Cyc_Absyn_Int_c; _temp2210.f1=( void*) _temp2201;
_temp2210.f2= 0; _temp2210;}); _temp2209;})); _temp2208;}); _temp2207;}))); if(
_temp2199 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2211=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2211[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2212; _temp2212.tag= Cyc_Absyn_Cast_e;
_temp2212.f1=( void*) t; _temp2212.f2= e; _temp2212;}); _temp2211;}), loc);}
goto _LL2182; _LL2190:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2213=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2213[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2214; _temp2214.tag= Cyc_Absyn_Const_e;
_temp2214.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2215=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2215[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2216; _temp2216.tag= Cyc_Absyn_Float_c;
_temp2216.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2216;});
_temp2215;})); _temp2214;}); _temp2213;}))); goto _LL2182; _LL2192:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2217=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2217[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2218; _temp2218.tag= Cyc_Absyn_Cast_e; _temp2218.f1=( void*) t; _temp2218.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2219=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2219[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2220; _temp2220.tag= Cyc_Absyn_Const_e;
_temp2220.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2221=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2221[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2222; _temp2222.tag= Cyc_Absyn_Float_c;
_temp2222.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2222;});
_temp2221;})); _temp2220;}); _temp2219;}), loc); _temp2218;}); _temp2217;})));
goto _LL2182; _LL2194: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp2223= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_get_arr_size( _temp2223, 1u), _temp2223.curr);})); goto _LL2182; _LL2182:;}
return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_List_List* s,
struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val( tv->kind); return({
struct _tuple5* _temp2226=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2226->f1= tv; _temp2226->f2= Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt*
_temp2227=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2227->v=( void*) k; _temp2227;}),({ struct Cyc_Core_Opt* _temp2228=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2228->v=( void*) s;
_temp2228;})); _temp2226;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct
_tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2231; struct
_RegionHandle* _temp2232; struct Cyc_List_List* _temp2234; struct _tuple6*
_temp2229= env; _temp2231=* _temp2229; _LL2235: _temp2234= _temp2231.f1; goto
_LL2233; _LL2233: _temp2232= _temp2231.f2; goto _LL2230; _LL2230: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2236=( struct _tuple5*) _region_malloc(
_temp2232, sizeof( struct _tuple5)); _temp2236->f1= tv; _temp2236->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2237=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2237->v=( void*) k; _temp2237;}),({ struct Cyc_Core_Opt*
_temp2238=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2238->v=( void*) _temp2234; _temp2238;})); _temp2236;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) == 0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp2239=* tv->name;
struct _tagged_arr _temp2240= Cyc_Absynpp_ckind2string( k1); struct _tagged_arr
_temp2241= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_get_arr_size( _temp2239, 1u), _temp2239.curr, _get_arr_size( _temp2240, 1u),
_temp2240.curr, _get_arr_size( _temp2241, 1u), _temp2241.curr);}));} if( tv->identity
== 0){ tv->identity=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity;} else{ if(*(( int*) _check_null( tv->identity))
!=*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity))){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)((
struct _tagged_arr) xprintf("same type variable has different identity!"));}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2242=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2242->hd=( void*) tv; _temp2242->tl= tvs; _temp2242;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv", sizeof( unsigned char), 39u));}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*)
_check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tvs2", sizeof( unsigned char), 41u));}
if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity)) ==*(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp2243=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2243->hd=( void*) tv; _temp2243->tl=
tvs; _temp2243;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(( struct _tagged_arr)({
struct _tagged_arr _temp2244= Cyc_Tcutil_tvar2string( tv); xprintf("bound tvar id for %.*s is null",
_get_arr_size( _temp2244, 1u), _temp2244.curr);}));} return({ struct Cyc_List_List*
_temp2245=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2245->hd=( void*) tv; _temp2245->tl= tvs; _temp2245;});} static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar( struct Cyc_List_List* es, void* e){ void* _temp2246=
Cyc_Tcutil_compress( e); int _temp2252; _LL2248: if(( unsigned int) _temp2246 >
4u?*(( int*) _temp2246) == Cyc_Absyn_Evar: 0){ _LL2253: _temp2252=(( struct Cyc_Absyn_Evar_struct*)
_temp2246)->f3; goto _LL2249;} else{ goto _LL2250;} _LL2250: goto _LL2251;
_LL2249:{ struct Cyc_List_List* es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ void* _temp2254= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( es2))->hd); int _temp2260; _LL2256: if((
unsigned int) _temp2254 > 4u?*(( int*) _temp2254) == Cyc_Absyn_Evar: 0){ _LL2261:
_temp2260=(( struct Cyc_Absyn_Evar_struct*) _temp2254)->f3; goto _LL2257;} else{
goto _LL2258;} _LL2258: goto _LL2259; _LL2257: if( _temp2252 == _temp2260){
return es;} goto _LL2255; _LL2259: goto _LL2255; _LL2255:;}} return({ struct Cyc_List_List*
_temp2262=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2262->hd=( void*) e; _temp2262->tl= es; _temp2262;}); _LL2251: return es;
_LL2247:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars( struct
Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r= 0;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ int
present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b=(( struct Cyc_List_List*)
_check_null( b))->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->identity)) ==*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2263=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2263->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);
_temp2263->tl= r; _temp2263;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; } ;
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv cvtenv, void*
expected_kind, void* t){{ void* _temp2264= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp2310; struct Cyc_Core_Opt** _temp2312; struct Cyc_Absyn_Tvar* _temp2313;
struct Cyc_Absyn_Enumdecl* _temp2315; struct Cyc_Absyn_Enumdecl** _temp2317;
struct _tuple1* _temp2318; struct Cyc_Absyn_TunionInfo _temp2320; void*
_temp2322; struct Cyc_List_List* _temp2324; void* _temp2326; void** _temp2328;
struct Cyc_Absyn_TunionFieldInfo _temp2329; struct Cyc_List_List* _temp2331;
void* _temp2333; void** _temp2335; struct Cyc_Absyn_PtrInfo _temp2336; struct
Cyc_Absyn_Conref* _temp2338; struct Cyc_Absyn_Tqual _temp2340; struct Cyc_Absyn_Conref*
_temp2342; void* _temp2344; void* _temp2346; struct Cyc_Absyn_Exp* _temp2348;
struct Cyc_Absyn_Tqual _temp2350; void* _temp2352; struct Cyc_Absyn_FnInfo
_temp2354; struct Cyc_List_List* _temp2356; struct Cyc_List_List* _temp2358;
struct Cyc_Absyn_VarargInfo* _temp2360; int _temp2362; struct Cyc_List_List*
_temp2364; void* _temp2366; struct Cyc_Core_Opt* _temp2368; struct Cyc_Core_Opt**
_temp2370; struct Cyc_List_List* _temp2371; struct Cyc_List_List** _temp2373;
struct Cyc_List_List* _temp2374; struct Cyc_List_List* _temp2376; struct Cyc_List_List*
_temp2378; struct Cyc_Absyn_Structdecl** _temp2380; struct Cyc_Absyn_Structdecl***
_temp2382; struct Cyc_List_List* _temp2383; struct _tuple1* _temp2385; struct
Cyc_Absyn_Uniondecl** _temp2387; struct Cyc_Absyn_Uniondecl*** _temp2389; struct
Cyc_List_List* _temp2390; struct _tuple1* _temp2392; struct Cyc_Core_Opt*
_temp2394; struct Cyc_Core_Opt** _temp2396; struct Cyc_List_List* _temp2397;
struct _tuple1* _temp2399; void* _temp2401; void* _temp2403; struct Cyc_List_List*
_temp2405; _LL2266: if( _temp2264 ==( void*) Cyc_Absyn_VoidType){ goto _LL2267;}
else{ goto _LL2268;} _LL2268: if(( unsigned int) _temp2264 > 4u?*(( int*)
_temp2264) == Cyc_Absyn_Evar: 0){ _LL2311: _temp2310=(( struct Cyc_Absyn_Evar_struct*)
_temp2264)->f1; _temp2312=&(( struct Cyc_Absyn_Evar_struct*) _temp2264)->f1;
goto _LL2269;} else{ goto _LL2270;} _LL2270: if(( unsigned int) _temp2264 > 4u?*((
int*) _temp2264) == Cyc_Absyn_VarType: 0){ _LL2314: _temp2313=(( struct Cyc_Absyn_VarType_struct*)
_temp2264)->f1; goto _LL2271;} else{ goto _LL2272;} _LL2272: if(( unsigned int)
_temp2264 > 4u?*(( int*) _temp2264) == Cyc_Absyn_EnumType: 0){ _LL2319:
_temp2318=(( struct Cyc_Absyn_EnumType_struct*) _temp2264)->f1; goto _LL2316;
_LL2316: _temp2315=(( struct Cyc_Absyn_EnumType_struct*) _temp2264)->f2;
_temp2317=&(( struct Cyc_Absyn_EnumType_struct*) _temp2264)->f2; goto _LL2273;}
else{ goto _LL2274;} _LL2274: if(( unsigned int) _temp2264 > 4u?*(( int*)
_temp2264) == Cyc_Absyn_TunionType: 0){ _LL2321: _temp2320=(( struct Cyc_Absyn_TunionType_struct*)
_temp2264)->f1; _LL2327: _temp2326=( void*) _temp2320.tunion_info; _temp2328=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2264)->f1).tunion_info; goto _LL2325;
_LL2325: _temp2324= _temp2320.targs; goto _LL2323; _LL2323: _temp2322=( void*)
_temp2320.rgn; goto _LL2275;} else{ goto _LL2276;} _LL2276: if(( unsigned int)
_temp2264 > 4u?*(( int*) _temp2264) == Cyc_Absyn_TunionFieldType: 0){ _LL2330:
_temp2329=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2264)->f1; _LL2334:
_temp2333=( void*) _temp2329.field_info; _temp2335=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2264)->f1).field_info; goto _LL2332; _LL2332: _temp2331= _temp2329.targs;
goto _LL2277;} else{ goto _LL2278;} _LL2278: if(( unsigned int) _temp2264 > 4u?*((
int*) _temp2264) == Cyc_Absyn_PointerType: 0){ _LL2337: _temp2336=(( struct Cyc_Absyn_PointerType_struct*)
_temp2264)->f1; _LL2347: _temp2346=( void*) _temp2336.elt_typ; goto _LL2345;
_LL2345: _temp2344=( void*) _temp2336.rgn_typ; goto _LL2343; _LL2343: _temp2342=
_temp2336.nullable; goto _LL2341; _LL2341: _temp2340= _temp2336.tq; goto _LL2339;
_LL2339: _temp2338= _temp2336.bounds; goto _LL2279;} else{ goto _LL2280;}
_LL2280: if(( unsigned int) _temp2264 > 4u?*(( int*) _temp2264) == Cyc_Absyn_IntType:
0){ goto _LL2281;} else{ goto _LL2282;} _LL2282: if( _temp2264 ==( void*) Cyc_Absyn_FloatType){
goto _LL2283;} else{ goto _LL2284;} _LL2284: if( _temp2264 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2285;} else{ goto _LL2286;} _LL2286: if(( unsigned int) _temp2264 > 4u?*((
int*) _temp2264) == Cyc_Absyn_ArrayType: 0){ _LL2353: _temp2352=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2264)->f1; goto _LL2351; _LL2351:
_temp2350=(( struct Cyc_Absyn_ArrayType_struct*) _temp2264)->f2; goto _LL2349;
_LL2349: _temp2348=(( struct Cyc_Absyn_ArrayType_struct*) _temp2264)->f3; goto
_LL2287;} else{ goto _LL2288;} _LL2288: if(( unsigned int) _temp2264 > 4u?*((
int*) _temp2264) == Cyc_Absyn_FnType: 0){ _LL2355: _temp2354=(( struct Cyc_Absyn_FnType_struct*)
_temp2264)->f1; _LL2372: _temp2371= _temp2354.tvars; _temp2373=&((( struct Cyc_Absyn_FnType_struct*)
_temp2264)->f1).tvars; goto _LL2369; _LL2369: _temp2368= _temp2354.effect;
_temp2370=&((( struct Cyc_Absyn_FnType_struct*) _temp2264)->f1).effect; goto
_LL2367; _LL2367: _temp2366=( void*) _temp2354.ret_typ; goto _LL2365; _LL2365:
_temp2364= _temp2354.args; goto _LL2363; _LL2363: _temp2362= _temp2354.c_varargs;
goto _LL2361; _LL2361: _temp2360= _temp2354.cyc_varargs; goto _LL2359; _LL2359:
_temp2358= _temp2354.rgn_po; goto _LL2357; _LL2357: _temp2356= _temp2354.attributes;
goto _LL2289;} else{ goto _LL2290;} _LL2290: if(( unsigned int) _temp2264 > 4u?*((
int*) _temp2264) == Cyc_Absyn_TupleType: 0){ _LL2375: _temp2374=(( struct Cyc_Absyn_TupleType_struct*)
_temp2264)->f1; goto _LL2291;} else{ goto _LL2292;} _LL2292: if(( unsigned int)
_temp2264 > 4u?*(( int*) _temp2264) == Cyc_Absyn_AnonStructType: 0){ _LL2377:
_temp2376=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2264)->f1; goto
_LL2293;} else{ goto _LL2294;} _LL2294: if(( unsigned int) _temp2264 > 4u?*((
int*) _temp2264) == Cyc_Absyn_AnonUnionType: 0){ _LL2379: _temp2378=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2264)->f1; goto _LL2295;} else{ goto
_LL2296;} _LL2296: if(( unsigned int) _temp2264 > 4u?*(( int*) _temp2264) == Cyc_Absyn_StructType:
0){ _LL2386: _temp2385=(( struct Cyc_Absyn_StructType_struct*) _temp2264)->f1;
goto _LL2384; _LL2384: _temp2383=(( struct Cyc_Absyn_StructType_struct*)
_temp2264)->f2; goto _LL2381; _LL2381: _temp2380=(( struct Cyc_Absyn_StructType_struct*)
_temp2264)->f3; _temp2382=&(( struct Cyc_Absyn_StructType_struct*) _temp2264)->f3;
goto _LL2297;} else{ goto _LL2298;} _LL2298: if(( unsigned int) _temp2264 > 4u?*((
int*) _temp2264) == Cyc_Absyn_UnionType: 0){ _LL2393: _temp2392=(( struct Cyc_Absyn_UnionType_struct*)
_temp2264)->f1; goto _LL2391; _LL2391: _temp2390=(( struct Cyc_Absyn_UnionType_struct*)
_temp2264)->f2; goto _LL2388; _LL2388: _temp2387=(( struct Cyc_Absyn_UnionType_struct*)
_temp2264)->f3; _temp2389=&(( struct Cyc_Absyn_UnionType_struct*) _temp2264)->f3;
goto _LL2299;} else{ goto _LL2300;} _LL2300: if(( unsigned int) _temp2264 > 4u?*((
int*) _temp2264) == Cyc_Absyn_TypedefType: 0){ _LL2400: _temp2399=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2264)->f1; goto _LL2398; _LL2398: _temp2397=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2264)->f2; goto _LL2395; _LL2395: _temp2394=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2264)->f3; _temp2396=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2264)->f3;
goto _LL2301;} else{ goto _LL2302;} _LL2302: if( _temp2264 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2303;} else{ goto _LL2304;} _LL2304: if(( unsigned int) _temp2264 > 4u?*((
int*) _temp2264) == Cyc_Absyn_RgnHandleType: 0){ _LL2402: _temp2401=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2264)->f1; goto _LL2305;} else{
goto _LL2306;} _LL2306: if(( unsigned int) _temp2264 > 4u?*(( int*) _temp2264)
== Cyc_Absyn_AccessEff: 0){ _LL2404: _temp2403=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2264)->f1; goto _LL2307;} else{ goto _LL2308;} _LL2308: if(( unsigned int)
_temp2264 > 4u?*(( int*) _temp2264) == Cyc_Absyn_JoinEff: 0){ _LL2406: _temp2405=((
struct Cyc_Absyn_JoinEff_struct*) _temp2264)->f1; goto _LL2309;} else{ goto
_LL2265;} _LL2267: goto _LL2265; _LL2269: if(* _temp2312 == 0){* _temp2312=({
struct Cyc_Core_Opt* _temp2407=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2407->v=( void*) expected_kind; _temp2407;});} cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars, t); goto _LL2265; _LL2271: { struct
Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref( _temp2313->kind); if(( void*) c->v
==( void*) Cyc_Absyn_No_constr){( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2408=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2408[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2409; _temp2409.tag= Cyc_Absyn_Eq_constr;
_temp2409.f1=( void*) expected_kind; _temp2409;}); _temp2408;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2313); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2313); goto _LL2265;}
_LL2273: if(* _temp2317 == 0){ struct _handler_cons _temp2410; _push_handler(&
_temp2410);{ int _temp2412= 0; if( setjmp( _temp2410.handler)){ _temp2412= 1;}
if( ! _temp2412){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2318);* _temp2317=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2411=( void*) _exn_thrown; void* _temp2414= _temp2411; _LL2416:
if( _temp2414 == Cyc_Dict_Absent){ goto _LL2417;} else{ goto _LL2418;} _LL2418:
goto _LL2419; _LL2417: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp2420= Cyc_Absynpp_qvar2string( _temp2318); xprintf("unbound type enum %.*s",
_get_arr_size( _temp2420, 1u), _temp2420.curr);})); return cvtenv; _LL2419:(
void) _throw( _temp2414); _LL2415:;}}}{ struct Cyc_Absyn_Enumdecl* ed=( struct
Cyc_Absyn_Enumdecl*) _check_null(* _temp2317);* _temp2318=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2265;} _LL2275:{ void*
_temp2421=* _temp2328; struct Cyc_Absyn_UnknownTunionInfo _temp2427; int
_temp2429; struct _tuple1* _temp2431; struct Cyc_Absyn_Tuniondecl* _temp2433;
_LL2423: if(*(( int*) _temp2421) == Cyc_Absyn_UnknownTunion){ _LL2428: _temp2427=((
struct Cyc_Absyn_UnknownTunion_struct*) _temp2421)->f1; _LL2432: _temp2431=
_temp2427.name; goto _LL2430; _LL2430: _temp2429= _temp2427.is_xtunion; goto
_LL2424;} else{ goto _LL2425;} _LL2425: if(*(( int*) _temp2421) == Cyc_Absyn_KnownTunion){
_LL2434: _temp2433=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2421)->f1; goto
_LL2426;} else{ goto _LL2422;} _LL2424: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2435; _push_handler(& _temp2435);{ int _temp2437= 0;
if( setjmp( _temp2435.handler)){ _temp2437= 1;} if( ! _temp2437){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2431);; _pop_handler();} else{ void* _temp2436=( void*)
_exn_thrown; void* _temp2439= _temp2436; _LL2441: if( _temp2439 == Cyc_Dict_Absent){
goto _LL2442;} else{ goto _LL2443;} _LL2443: goto _LL2444; _LL2442: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2445= Cyc_Absynpp_qvar2string(
_temp2431); xprintf("unbound type [x]tunion %.*s", _get_arr_size( _temp2445, 1u),
_temp2445.curr);})); return cvtenv; _LL2444:( void) _throw( _temp2439); _LL2440:;}}}
if( tud->is_xtunion != _temp2429){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp2446= Cyc_Absynpp_qvar2string( _temp2431); xprintf("[x]tunion is different from type declaration %.*s",
_get_arr_size( _temp2446, 1u), _temp2446.curr);}));}* _temp2328=( void*)({
struct Cyc_Absyn_KnownTunion_struct* _temp2447=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2447[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2448; _temp2448.tag= Cyc_Absyn_KnownTunion;
_temp2448.f1= tud; _temp2448;}); _temp2447;}); _temp2433= tud; goto _LL2426;}
_LL2426: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2322);{ struct Cyc_List_List* tvs= _temp2433->tvs; for( 0; _temp2324 != 0?
tvs != 0: 0;( _temp2324=(( struct Cyc_List_List*) _check_null( _temp2324))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2324))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2324
!= 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp2449=
Cyc_Absynpp_qvar2string( _temp2433->name); xprintf("too many type arguments for tunion %.*s",
_get_arr_size( _temp2449, 1u), _temp2449.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2450= Cyc_Absynpp_qvar2string(
_temp2433->name); xprintf("too few type arguments for tunion %.*s",
_get_arr_size( _temp2450, 1u), _temp2450.curr);}));} goto _LL2422;} _LL2422:;}
goto _LL2265; _LL2277:{ void* _temp2451=* _temp2335; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2457; int _temp2459; struct _tuple1* _temp2461; struct _tuple1* _temp2463;
struct Cyc_Absyn_Tunionfield* _temp2465; struct Cyc_Absyn_Tuniondecl* _temp2467;
_LL2453: if(*(( int*) _temp2451) == Cyc_Absyn_UnknownTunionfield){ _LL2458:
_temp2457=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2451)->f1;
_LL2464: _temp2463= _temp2457.tunion_name; goto _LL2462; _LL2462: _temp2461=
_temp2457.field_name; goto _LL2460; _LL2460: _temp2459= _temp2457.is_xtunion;
goto _LL2454;} else{ goto _LL2455;} _LL2455: if(*(( int*) _temp2451) == Cyc_Absyn_KnownTunionfield){
_LL2468: _temp2467=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2451)->f1;
goto _LL2466; _LL2466: _temp2465=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2451)->f2; goto _LL2456;} else{ goto _LL2452;} _LL2454: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2469;
_push_handler(& _temp2469);{ int _temp2471= 0; if( setjmp( _temp2469.handler)){
_temp2471= 1;} if( ! _temp2471){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2463);; _pop_handler();} else{ void* _temp2470=( void*) _exn_thrown; void*
_temp2473= _temp2470; _LL2475: if( _temp2473 == Cyc_Dict_Absent){ goto _LL2476;}
else{ goto _LL2477;} _LL2477: goto _LL2478; _LL2476: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp2479= Cyc_Absynpp_qvar2string(
_temp2463); xprintf("unbound type tunion %.*s", _get_arr_size( _temp2479, 1u),
_temp2479.curr);})); return cvtenv; _LL2478:( void) _throw( _temp2473); _LL2474:;}}}{
struct _handler_cons _temp2480; _push_handler(& _temp2480);{ int _temp2482= 0;
if( setjmp( _temp2480.handler)){ _temp2482= 1;} if( ! _temp2482){{ void*
_temp2483= Cyc_Tcenv_lookup_ordinary( te, loc, _temp2461); struct Cyc_Absyn_Tunionfield*
_temp2489; struct Cyc_Absyn_Tuniondecl* _temp2491; _LL2485: if(*(( int*)
_temp2483) == Cyc_Tcenv_TunionRes){ _LL2492: _temp2491=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2483)->f1; goto _LL2490; _LL2490: _temp2489=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2483)->f2; goto _LL2486;} else{ goto _LL2487;} _LL2487: goto _LL2488;
_LL2486: tuf= _temp2489; tud= _temp2491; if( tud->is_xtunion != _temp2459){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2493= Cyc_Absynpp_qvar2string(
_temp2463); xprintf("[x]tunion is different from type declaration %.*s",
_get_arr_size( _temp2493, 1u), _temp2493.curr);}));} goto _LL2484; _LL2488: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2494= Cyc_Absynpp_qvar2string(
_temp2461); struct _tagged_arr _temp2495= Cyc_Absynpp_qvar2string( _temp2463);
xprintf("unbound field %.*s in type tunion %.*s", _get_arr_size( _temp2494, 1u),
_temp2494.curr, _get_arr_size( _temp2495, 1u), _temp2495.curr);}));{ struct Cyc_Tcutil_CVTEnv
_temp2496= cvtenv; _npop_handler( 0u); return _temp2496;} _LL2484:;};
_pop_handler();} else{ void* _temp2481=( void*) _exn_thrown; void* _temp2498=
_temp2481; _LL2500: if( _temp2498 == Cyc_Dict_Absent){ goto _LL2501;} else{ goto
_LL2502;} _LL2502: goto _LL2503; _LL2501: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp2504= Cyc_Absynpp_qvar2string( _temp2461);
struct _tagged_arr _temp2505= Cyc_Absynpp_qvar2string( _temp2463); xprintf("unbound field %.*s in type tunion %.*s",
_get_arr_size( _temp2504, 1u), _temp2504.curr, _get_arr_size( _temp2505, 1u),
_temp2505.curr);})); return cvtenv; _LL2503:( void) _throw( _temp2498); _LL2499:;}}}*
_temp2335=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct* _temp2506=(
struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunionfield_struct));
_temp2506[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct _temp2507; _temp2507.tag=
Cyc_Absyn_KnownTunionfield; _temp2507.f1= tud; _temp2507.f2= tuf; _temp2507;});
_temp2506;}); _temp2467= tud; _temp2465= tuf; goto _LL2456;} _LL2456: { struct
Cyc_List_List* tvs= _temp2467->tvs; for( 0; _temp2331 != 0? tvs != 0: 0;(
_temp2331=(( struct Cyc_List_List*) _check_null( _temp2331))->tl, tvs=(( struct
Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)(( struct Cyc_List_List*)
_check_null( _temp2331))->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} if( _temp2331 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp2508= Cyc_Absynpp_qvar2string( _temp2467->name);
struct _tagged_arr _temp2509= Cyc_Absynpp_qvar2string( _temp2465->name); xprintf("too many type arguments for tunion %.*s.%.*s",
_get_arr_size( _temp2508, 1u), _temp2508.curr, _get_arr_size( _temp2509, 1u),
_temp2509.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp2510= Cyc_Absynpp_qvar2string( _temp2467->name); struct
_tagged_arr _temp2511= Cyc_Absynpp_qvar2string( _temp2465->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_get_arr_size( _temp2510, 1u), _temp2510.curr, _get_arr_size( _temp2511, 1u),
_temp2511.curr);}));} goto _LL2452;} _LL2452:;} goto _LL2265; _LL2279: cvtenv=
Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2346); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2344);{ void* _temp2512=( void*)( Cyc_Absyn_compress_conref( _temp2338))->v;
void* _temp2518; struct Cyc_Absyn_Exp* _temp2520; _LL2514: if(( unsigned int)
_temp2512 > 1u?*(( int*) _temp2512) == Cyc_Absyn_Eq_constr: 0){ _LL2519:
_temp2518=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2512)->f1; if((
unsigned int) _temp2518 > 1u?*(( int*) _temp2518) == Cyc_Absyn_Upper_b: 0){
_LL2521: _temp2520=(( struct Cyc_Absyn_Upper_b_struct*) _temp2518)->f1; goto
_LL2515;} else{ goto _LL2516;}} else{ goto _LL2516;} _LL2516: goto _LL2517;
_LL2515: if( ! Cyc_Tcutil_is_const_exp( te, _temp2520)){ Cyc_Tcutil_terr( loc,
_tag_arr("pointer bounds expression is not a constant", sizeof( unsigned char),
44u));} Cyc_Tcexp_tcExp( te, 0, _temp2520); if( ! Cyc_Tcutil_coerce_uint_typ( te,
_temp2520)){ Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u));} Cyc_Evexp_eval_const_uint_exp( _temp2520); goto
_LL2513; _LL2517: goto _LL2513; _LL2513:;} goto _LL2265; _LL2281: goto _LL2265;
_LL2283: goto _LL2265; _LL2285: goto _LL2265; _LL2287: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp2352); if( _temp2348 == 0? 1: !
Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null( _temp2348))){
Cyc_Tcutil_terr( loc, _tag_arr("array bounds expression is not constant",
sizeof( unsigned char), 40u));} Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_check_null( _temp2348)); if( ! Cyc_Tcutil_coerce_uint_typ( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2348))){ Cyc_Tcutil_terr( loc, _tag_arr("array bounds expression is not an unsigned int",
sizeof( unsigned char), 47u));} Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp2348)); goto _LL2265; _LL2289: { int num_convs= 0; int
seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; for( 0; _temp2356 != 0;
_temp2356=(( struct Cyc_List_List*) _check_null( _temp2356))->tl){ if( ! Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( _temp2356))->hd)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2522= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2356))->hd); xprintf("bad function type attribute %.*s",
_get_arr_size( _temp2522, 1u), _temp2522.curr);}));}{ void* _temp2523=( void*)((
struct Cyc_List_List*) _check_null( _temp2356))->hd; _LL2525: if( _temp2523 ==(
void*) Cyc_Absyn_Stdcall_att){ goto _LL2526;} else{ goto _LL2527;} _LL2527: if(
_temp2523 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL2528;} else{ goto _LL2529;}
_LL2529: if( _temp2523 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL2530;} else{
goto _LL2531;} _LL2531: goto _LL2532; _LL2526: if( ! seen_stdcall){ seen_stdcall=
1; ++ num_convs;} goto _LL2524; _LL2528: if( ! seen_cdecl){ seen_cdecl= 1; ++
num_convs;} goto _LL2524; _LL2530: if( ! seen_fastcall){ seen_fastcall= 1; ++
num_convs;} goto _LL2524; _LL2532: goto _LL2524; _LL2524:;}} if( num_convs > 1){
Cyc_Tcutil_terr( loc,( struct _tagged_arr) xprintf("function can't have multiple calling conventions"));}
Cyc_Tcutil_check_unique_tvars( loc,* _temp2373);{ struct Cyc_List_List* b=*
_temp2373; for( 0; b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity=
Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd);{ void*
_temp2533=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd)->kind))->v; void* _temp2539;
_LL2535: if(( unsigned int) _temp2533 > 1u?*(( int*) _temp2533) == Cyc_Absyn_Eq_constr:
0){ _LL2540: _temp2539=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2533)->f1;
if( _temp2539 ==( void*) Cyc_Absyn_MemKind){ goto _LL2536;} else{ goto _LL2537;}}
else{ goto _LL2537;} _LL2537: goto _LL2538; _LL2536: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp2541=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_get_arr_size( _temp2541, 1u), _temp2541.curr);})); goto _LL2534; _LL2538: goto
_LL2534; _LL2534:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2542=({ struct Cyc_Tcutil_CVTEnv
_temp2645; _temp2645.kind_env= cvtenv.kind_env; _temp2645.free_vars= 0;
_temp2645.free_evars= 0; _temp2645;}); _temp2542= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2542,( void*) Cyc_Absyn_MemKind, _temp2366); for( 0; _temp2364 !=
0; _temp2364=(( struct Cyc_List_List*) _check_null( _temp2364))->tl){ _temp2542=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2542,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp2364))->hd)).f3);}
if( _temp2360 != 0){ if( _temp2362){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u));}{ int
_temp2545; void* _temp2547; void* _temp2549; struct Cyc_Absyn_Tqual _temp2551;
struct Cyc_Core_Opt* _temp2553; struct Cyc_Absyn_VarargInfo _temp2543=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp2360)); _LL2554: _temp2553= _temp2543.name;
goto _LL2552; _LL2552: _temp2551= _temp2543.tq; goto _LL2550; _LL2550: _temp2549=(
void*) _temp2543.type; goto _LL2548; _LL2548: _temp2547=( void*) _temp2543.rgn;
goto _LL2546; _LL2546: _temp2545= _temp2543.inject; goto _LL2544; _LL2544:
_temp2542= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2542,( void*) Cyc_Absyn_MemKind,
_temp2549);{ void* _temp2555= Cyc_Tcutil_compress( _temp2547); _LL2557: if((
unsigned int) _temp2555 > 4u?*(( int*) _temp2555) == Cyc_Absyn_Evar: 0){ goto
_LL2558;} else{ goto _LL2559;} _LL2559: if(( unsigned int) _temp2555 > 4u?*((
int*) _temp2555) == Cyc_Absyn_VarType: 0){ goto _LL2560;} else{ goto _LL2561;}
_LL2561: if( _temp2555 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2562;} else{ goto
_LL2563;} _LL2563: goto _LL2564; _LL2558: { void* vr_tvar=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2565=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2565[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2566; _temp2566.tag= Cyc_Absyn_VarType;
_temp2566.f1= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp2566;});
_temp2565;}); Cyc_Tcutil_unify( _temp2547, vr_tvar); goto _LL2556;} _LL2560:
goto _LL2556; _LL2562: Cyc_Tcutil_terr( loc, _tag_arr("varargs should be in a region, not heap-allocated",
sizeof( unsigned char), 50u)); goto _LL2556; _LL2564: goto _LL2556; _LL2556:;}
_temp2542= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2542,( void*) Cyc_Absyn_RgnKind,
_temp2547); if( _temp2545){ void* _temp2567= Cyc_Tcutil_compress( _temp2549);
struct Cyc_Absyn_TunionInfo _temp2573; void* _temp2575; void* _temp2577; _LL2569:
if(( unsigned int) _temp2567 > 4u?*(( int*) _temp2567) == Cyc_Absyn_TunionType:
0){ _LL2574: _temp2573=(( struct Cyc_Absyn_TunionType_struct*) _temp2567)->f1;
_LL2578: _temp2577=( void*) _temp2573.tunion_info; if(*(( int*) _temp2577) ==
Cyc_Absyn_KnownTunion){ goto _LL2576;} else{ goto _LL2571;} _LL2576: _temp2575=(
void*) _temp2573.rgn; goto _LL2570;} else{ goto _LL2571;} _LL2571: goto _LL2572;
_LL2570:{ void* _temp2579= Cyc_Tcutil_compress( _temp2575); _LL2581: if((
unsigned int) _temp2579 > 4u?*(( int*) _temp2579) == Cyc_Absyn_Evar: 0){ goto
_LL2582;} else{ goto _LL2583;} _LL2583: if(( unsigned int) _temp2579 > 4u?*((
int*) _temp2579) == Cyc_Absyn_VarType: 0){ goto _LL2584;} else{ goto _LL2585;}
_LL2585: if( _temp2579 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2586;} else{ goto
_LL2587;} _LL2587: goto _LL2588; _LL2582: Cyc_Tcutil_unify( _temp2547, _temp2575);
goto _LL2580; _LL2584: goto _LL2580; _LL2586: Cyc_Tcutil_terr( loc, _tag_arr("injected vararg [x]tunion should be in a region, not heap-allocated",
sizeof( unsigned char), 68u)); goto _LL2580; _LL2588: goto _LL2580; _LL2580:;}
goto _LL2568; _LL2572: Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u)); goto _LL2568; _LL2568:;}}}{ struct Cyc_List_List*
rpo= _temp2358; for( 0; rpo != 0; rpo=(( struct Cyc_List_List*) _check_null( rpo))->tl){
struct _tuple8 _temp2591; void* _temp2592; void* _temp2594; struct _tuple8*
_temp2589=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( rpo))->hd;
_temp2591=* _temp2589; _LL2595: _temp2594= _temp2591.f1; goto _LL2593; _LL2593:
_temp2592= _temp2591.f2; goto _LL2590; _LL2590: _temp2542= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2542,( void*) Cyc_Absyn_RgnKind, _temp2594); _temp2542= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2542,( void*) Cyc_Absyn_RgnKind, _temp2592);}} if(* _temp2370 != 0){
_temp2542= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2542,( void*) Cyc_Absyn_EffKind,(
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp2370))->v);} else{ struct Cyc_Absyn_Tvar*
e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind); _temp2542.free_vars= Cyc_Tcutil_add_free_tvar(
loc, _temp2542.free_vars, e);{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= _temp2542.free_vars; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ void* _temp2596=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v;
void* _temp2604; void* _temp2606; _LL2598: if(( unsigned int) _temp2596 > 1u?*((
int*) _temp2596) == Cyc_Absyn_Eq_constr: 0){ _LL2605: _temp2604=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2596)->f1; if( _temp2604 ==( void*) Cyc_Absyn_RgnKind){
goto _LL2599;} else{ goto _LL2600;}} else{ goto _LL2600;} _LL2600: if((
unsigned int) _temp2596 > 1u?*(( int*) _temp2596) == Cyc_Absyn_Eq_constr: 0){
_LL2607: _temp2606=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2596)->f1;
if( _temp2606 ==( void*) Cyc_Absyn_EffKind){ goto _LL2601;} else{ goto _LL2602;}}
else{ goto _LL2602;} _LL2602: goto _LL2603; _LL2599: effect=({ struct Cyc_List_List*
_temp2608=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2608->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2609=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2609[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2610; _temp2610.tag= Cyc_Absyn_AccessEff;
_temp2610.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2611=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2611[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2612; _temp2612.tag= Cyc_Absyn_VarType;
_temp2612.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2612;}); _temp2611;})); _temp2610;}); _temp2609;})); _temp2608->tl= effect;
_temp2608;}); goto _LL2597; _LL2601: effect=({ struct Cyc_List_List* _temp2613=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2613->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2614=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2614[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2615; _temp2615.tag= Cyc_Absyn_VarType; _temp2615.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2615;}); _temp2614;})); _temp2613->tl= effect; _temp2613;}); goto _LL2597;
_LL2603: goto _LL2597; _LL2597:;}}{ struct Cyc_List_List* ts= _temp2542.free_evars;
for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ void*
_temp2616= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_LL2618: if( _temp2616 ==( void*) Cyc_Absyn_RgnKind){ goto _LL2619;} else{ goto
_LL2620;} _LL2620: if( _temp2616 ==( void*) Cyc_Absyn_EffKind){ goto _LL2621;}
else{ goto _LL2622;} _LL2622: goto _LL2623; _LL2619: effect=({ struct Cyc_List_List*
_temp2624=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2624->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2625=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2625[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2626; _temp2626.tag= Cyc_Absyn_AccessEff;
_temp2626.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp2626;}); _temp2625;})); _temp2624->tl= effect; _temp2624;}); goto _LL2617;
_LL2621: effect=({ struct Cyc_List_List* _temp2627=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2627->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp2627->tl= effect; _temp2627;});
goto _LL2617; _LL2623: goto _LL2617; _LL2617:;}}* _temp2370=({ struct Cyc_Core_Opt*
_temp2628=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2628->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2629=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2629[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2630; _temp2630.tag= Cyc_Absyn_JoinEff;
_temp2630.f1= Cyc_List_imp_rev( effect); _temp2630;}); _temp2629;})); _temp2628;});}}
if(* _temp2373 != 0){ struct Cyc_List_List* bs=* _temp2373; for( 0; bs != 0; bs=((
struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Conref*
_temp2631= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->kind); void* _temp2632=( void*) _temp2631->v; void*
_temp2640; _LL2634: if( _temp2632 ==( void*) Cyc_Absyn_No_constr){ goto _LL2635;}
else{ goto _LL2636;} _LL2636: if(( unsigned int) _temp2632 > 1u?*(( int*)
_temp2632) == Cyc_Absyn_Eq_constr: 0){ _LL2641: _temp2640=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2632)->f1; if( _temp2640 ==( void*) Cyc_Absyn_MemKind){ goto _LL2637;}
else{ goto _LL2638;}} else{ goto _LL2638;} _LL2638: goto _LL2639; _LL2635: Cyc_Tcutil_warn(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2642=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_get_arr_size( _temp2642, 1u), _temp2642.curr);})); goto _LL2637; _LL2637:( void*)(
_temp2631->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2643=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2643[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2644; _temp2644.tag= Cyc_Absyn_Eq_constr;
_temp2644.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2644;}); _temp2643;})));
goto _LL2633; _LL2639: goto _LL2633; _LL2633:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2542.kind_env,* _temp2373); _temp2542.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2542.free_vars,* _temp2373);{ struct Cyc_List_List* tvs= _temp2542.free_vars;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2542.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2265;}}
_LL2291: for( 0; _temp2374 != 0; _temp2374=(( struct Cyc_List_List*) _check_null(
_temp2374))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp2374))->hd)).f2);} goto _LL2265; _LL2293:{ struct _RegionHandle _temp2646=
_new_region(); struct _RegionHandle* sprev_rgn=& _temp2646; _push_region(
sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2376 != 0;
_temp2376=(( struct Cyc_List_List*) _check_null( _temp2376))->tl){ struct Cyc_Absyn_Structfield
_temp2649; struct Cyc_List_List* _temp2650; struct Cyc_Core_Opt* _temp2652; void*
_temp2654; struct Cyc_Absyn_Tqual _temp2656; struct _tagged_arr* _temp2658;
struct Cyc_Absyn_Structfield* _temp2647=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2376))->hd; _temp2649=* _temp2647;
_LL2659: _temp2658= _temp2649.name; goto _LL2657; _LL2657: _temp2656= _temp2649.tq;
goto _LL2655; _LL2655: _temp2654=( void*) _temp2649.type; goto _LL2653; _LL2653:
_temp2652= _temp2649.width; goto _LL2651; _LL2651: _temp2650= _temp2649.attributes;
goto _LL2648; _LL2648: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp2658)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp2660=* _temp2658; xprintf("duplicate field %.*s in struct",
_get_arr_size( _temp2660, 1u), _temp2660.curr);}));} if( Cyc_String_strcmp(*
_temp2658, _tag_arr("", sizeof( unsigned char), 1u)) != 0){ prev_fields=({
struct Cyc_List_List* _temp2661=( struct Cyc_List_List*) _region_malloc(
sprev_rgn, sizeof( struct Cyc_List_List)); _temp2661->hd=( void*) _temp2658;
_temp2661->tl= prev_fields; _temp2661;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp2654); if( _temp2652 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2652))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp2662=* _temp2658; xprintf("bitfield %.*s does not have constant width",
_get_arr_size( _temp2662, 1u), _temp2662.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2652))->v);}{
void* _temp2663= Cyc_Tcutil_compress( _temp2654); void* _temp2669; _LL2665: if((
unsigned int) _temp2663 > 4u?*(( int*) _temp2663) == Cyc_Absyn_IntType: 0){
_LL2670: _temp2669=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2663)->f2;
goto _LL2666;} else{ goto _LL2667;} _LL2667: goto _LL2668; _LL2666:{ void*
_temp2671= _temp2669; _LL2673: if( _temp2671 ==( void*) Cyc_Absyn_B1){ goto
_LL2674;} else{ goto _LL2675;} _LL2675: if( _temp2671 ==( void*) Cyc_Absyn_B2){
goto _LL2676;} else{ goto _LL2677;} _LL2677: if( _temp2671 ==( void*) Cyc_Absyn_B4){
goto _LL2678;} else{ goto _LL2679;} _LL2679: if( _temp2671 ==( void*) Cyc_Absyn_B8){
goto _LL2680;} else{ goto _LL2672;} _LL2674: if( w > 8){ Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL2672; _LL2676: if( w > 16){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL2672; _LL2678: if( w > 32){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL2672; _LL2680: if( w > 64){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL2672; _LL2672:;} goto _LL2664; _LL2668:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp2681=*
_temp2658; struct _tagged_arr _temp2682= Cyc_Absynpp_typ2string( _temp2654);
xprintf("bitfield %.*s must have integral type but has type %.*s", _get_arr_size(
_temp2681, 1u), _temp2681.curr, _get_arr_size( _temp2682, 1u), _temp2682.curr);}));
goto _LL2664; _LL2664:;}} for( 0; _temp2650 != 0; _temp2650=(( struct Cyc_List_List*)
_check_null( _temp2650))->tl){ void* _temp2683=( void*)(( struct Cyc_List_List*)
_check_null( _temp2650))->hd; _LL2685: if( _temp2683 ==( void*) Cyc_Absyn_Packed_att){
goto _LL2686;} else{ goto _LL2687;} _LL2687: if(( unsigned int) _temp2683 > 16u?*((
int*) _temp2683) == Cyc_Absyn_Aligned_att: 0){ goto _LL2688;} else{ goto _LL2689;}
_LL2689: goto _LL2690; _LL2686: continue; _LL2688: continue; _LL2690: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2691= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2650))->hd); struct
_tagged_arr _temp2692=* _temp2658; xprintf("bad attribute %.*s on struct member %.*s",
_get_arr_size( _temp2691, 1u), _temp2691.curr, _get_arr_size( _temp2692, 1u),
_temp2692.curr);})); goto _LL2684; _LL2684:;}}}; _pop_region( sprev_rgn);} goto
_LL2265; _LL2295:{ struct _RegionHandle _temp2693= _new_region(); struct
_RegionHandle* uprev_rgn=& _temp2693; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp2378 != 0; _temp2378=(( struct Cyc_List_List*)
_check_null( _temp2378))->tl){ struct Cyc_Absyn_Structfield _temp2696; struct
Cyc_List_List* _temp2697; struct Cyc_Core_Opt* _temp2699; void* _temp2701;
struct Cyc_Absyn_Tqual _temp2703; struct _tagged_arr* _temp2705; struct Cyc_Absyn_Structfield*
_temp2694=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2378))->hd; _temp2696=* _temp2694; _LL2706: _temp2705= _temp2696.name; goto
_LL2704; _LL2704: _temp2703= _temp2696.tq; goto _LL2702; _LL2702: _temp2701=(
void*) _temp2696.type; goto _LL2700; _LL2700: _temp2699= _temp2696.width; goto
_LL2698; _LL2698: _temp2697= _temp2696.attributes; goto _LL2695; _LL2695: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp2705)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp2707=* _temp2705; xprintf("duplicate field %.*s in struct", _get_arr_size(
_temp2707, 1u), _temp2707.curr);}));} if( Cyc_String_strcmp(* _temp2705,
_tag_arr("", sizeof( unsigned char), 1u)) != 0){ prev_fields=({ struct Cyc_List_List*
_temp2708=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp2708->hd=( void*) _temp2705; _temp2708->tl= prev_fields; _temp2708;});}
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2701); if( ! Cyc_Tcutil_bits_only( _temp2701)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp2709=* _temp2705; xprintf("%.*s has a type that is possibly incompatible with other members of union",
_get_arr_size( _temp2709, 1u), _temp2709.curr);}));} if( _temp2699 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2699))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp2710=* _temp2705; xprintf("bitfield %.*s does not have constant width",
_get_arr_size( _temp2710, 1u), _temp2710.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2699))->v);}{
void* _temp2711= Cyc_Tcutil_compress( _temp2701); void* _temp2717; _LL2713: if((
unsigned int) _temp2711 > 4u?*(( int*) _temp2711) == Cyc_Absyn_IntType: 0){
_LL2718: _temp2717=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2711)->f2;
goto _LL2714;} else{ goto _LL2715;} _LL2715: goto _LL2716; _LL2714:{ void*
_temp2719= _temp2717; _LL2721: if( _temp2719 ==( void*) Cyc_Absyn_B1){ goto
_LL2722;} else{ goto _LL2723;} _LL2723: if( _temp2719 ==( void*) Cyc_Absyn_B2){
goto _LL2724;} else{ goto _LL2725;} _LL2725: if( _temp2719 ==( void*) Cyc_Absyn_B4){
goto _LL2726;} else{ goto _LL2727;} _LL2727: if( _temp2719 ==( void*) Cyc_Absyn_B8){
goto _LL2728;} else{ goto _LL2720;} _LL2722: if( w > 8){ Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL2720; _LL2724: if( w > 16){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL2720; _LL2726: if( w > 32){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL2720; _LL2728: if( w > 64){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL2720; _LL2720:;} goto _LL2712; _LL2716:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp2729=*
_temp2705; struct _tagged_arr _temp2730= Cyc_Absynpp_typ2string( _temp2701);
xprintf("bitfield %.*s must have integral type but has type %.*s", _get_arr_size(
_temp2729, 1u), _temp2729.curr, _get_arr_size( _temp2730, 1u), _temp2730.curr);}));
goto _LL2712; _LL2712:;}} for( 0; _temp2697 != 0; _temp2697=(( struct Cyc_List_List*)
_check_null( _temp2697))->tl){ void* _temp2731=( void*)(( struct Cyc_List_List*)
_check_null( _temp2697))->hd; _LL2733: if( _temp2731 ==( void*) Cyc_Absyn_Packed_att){
goto _LL2734;} else{ goto _LL2735;} _LL2735: if(( unsigned int) _temp2731 > 16u?*((
int*) _temp2731) == Cyc_Absyn_Aligned_att: 0){ goto _LL2736;} else{ goto _LL2737;}
_LL2737: goto _LL2738; _LL2734: continue; _LL2736: continue; _LL2738: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2739= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2697))->hd); struct
_tagged_arr _temp2740=* _temp2705; xprintf("bad attribute %.*s on struct member %.*s",
_get_arr_size( _temp2739, 1u), _temp2739.curr, _get_arr_size( _temp2740, 1u),
_temp2740.curr);})); goto _LL2732; _LL2732:;}}}; _pop_region( uprev_rgn);} goto
_LL2265; _LL2297: if( _temp2385 == 0){ if(* _temp2382 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr) xprintf("bad anonymous struct")); return cvtenv;}{
struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(*
_temp2382));;}} if(* _temp2382 == 0){ struct _handler_cons _temp2741;
_push_handler(& _temp2741);{ int _temp2743= 0; if( setjmp( _temp2741.handler)){
_temp2743= 1;} if( ! _temp2743){* _temp2382=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2385));; _pop_handler();} else{
void* _temp2742=( void*) _exn_thrown; void* _temp2745= _temp2742; _LL2747: if(
_temp2745 == Cyc_Dict_Absent){ goto _LL2748;} else{ goto _LL2749;} _LL2749: goto
_LL2750; _LL2748: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp2751= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp2385)); xprintf("unbound type struct %.*s", _get_arr_size( _temp2751, 1u),
_temp2751.curr);})); return cvtenv; _LL2750:( void) _throw( _temp2745); _LL2746:;}}}{
struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(*
_temp2382)); if( sd->name != 0){*(( struct _tuple1*) _check_null( _temp2385))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp2383); if( lvs != largs){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp2752= Cyc_Absynpp_qvar2string(( struct
_tuple1*) _check_null( _temp2385)); int _temp2753= lvs; int _temp2754= largs;
xprintf("struct %.*s expects %d type arguments but was given %d", _get_arr_size(
_temp2752, 1u), _temp2752.curr, _temp2753, _temp2754);}));} for( 0; _temp2383 !=
0;( _temp2383=(( struct Cyc_List_List*) _check_null( _temp2383))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)(( struct Cyc_List_List*)
_check_null( _temp2383))->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2265;}} _LL2299: if( _temp2392 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("missing union name", sizeof( unsigned char), 19u)); return cvtenv;}
if(* _temp2389 == 0){ struct _handler_cons _temp2755; _push_handler(& _temp2755);{
int _temp2757= 0; if( setjmp( _temp2755.handler)){ _temp2757= 1;} if( !
_temp2757){* _temp2389=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*) _check_null( _temp2392));; _pop_handler();} else{
void* _temp2756=( void*) _exn_thrown; void* _temp2759= _temp2756; _LL2761: if(
_temp2759 == Cyc_Dict_Absent){ goto _LL2762;} else{ goto _LL2763;} _LL2763: goto
_LL2764; _LL2762: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp2765= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp2392)); xprintf("unbound type union %.*s", _get_arr_size( _temp2765, 1u),
_temp2765.curr);})); return cvtenv; _LL2764:( void) _throw( _temp2759); _LL2760:;}}}{
struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(*
_temp2389)); if( ud->name != 0){*(( struct _tuple1*) _check_null( _temp2392))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp2390); if( lvs != largs){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp2766= Cyc_Absynpp_qvar2string(( struct
_tuple1*) _check_null( _temp2392)); int _temp2767= lvs; int _temp2768= largs;
xprintf("union %.*s expects %d type arguments but was given %d", _get_arr_size(
_temp2766, 1u), _temp2766.curr, _temp2767, _temp2768);}));} for( 0; _temp2390 !=
0;( _temp2390=(( struct Cyc_List_List*) _check_null( _temp2390))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)(( struct Cyc_List_List*)
_check_null( _temp2390))->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2265;}} _LL2301: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp2769; _push_handler(& _temp2769);{ int _temp2771=
0; if( setjmp( _temp2769.handler)){ _temp2771= 1;} if( ! _temp2771){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2399);; _pop_handler();} else{ void* _temp2770=( void*)
_exn_thrown; void* _temp2773= _temp2770; _LL2775: if( _temp2773 == Cyc_Dict_Absent){
goto _LL2776;} else{ goto _LL2777;} _LL2777: goto _LL2778; _LL2776: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2779= Cyc_Absynpp_qvar2string(
_temp2399); xprintf("unbound typedef name %.*s", _get_arr_size( _temp2779, 1u),
_temp2779.curr);})); return cvtenv; _LL2778:( void) _throw( _temp2773); _LL2774:;}}}
_temp2399[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2397; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); inst=({ struct Cyc_List_List* _temp2780=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2780->hd=( void*)({ struct
_tuple5* _temp2781=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2781->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp2781->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp2781;}); _temp2780->tl= inst; _temp2780;});} if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2782= Cyc_Absynpp_qvar2string(
_temp2399); xprintf("typedef %.*s expects a different number of arguments",
_get_arr_size( _temp2782, 1u), _temp2782.curr);})); return cvtenv;}{ void*
new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2396=({ struct
Cyc_Core_Opt* _temp2783=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2783->v=( void*) new_typ; _temp2783;}); goto _LL2265;}}} _LL2303: goto
_LL2265; _LL2305: _temp2403= _temp2401; goto _LL2307; _LL2307: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2403); goto _LL2265; _LL2309:
for( 0; _temp2405 != 0; _temp2405=(( struct Cyc_List_List*) _check_null(
_temp2405))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*) _check_null( _temp2405))->hd);}
goto _LL2265; _LL2265:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t),
expected_kind)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp2784= Cyc_Absynpp_typ2string( t); struct _tagged_arr _temp2785= Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind( t)); struct _tagged_arr _temp2786= Cyc_Absynpp_kind2string(
expected_kind); xprintf("type %.*s has kind %.*s but as used here needs kind %.*s",
_get_arr_size( _temp2784, 1u), _temp2784.curr, _get_arr_size( _temp2785, 1u),
_temp2785.curr, _get_arr_size( _temp2786, 1u), _temp2786.curr);}));} return
cvtenv;} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* kind_env, void*
expected_kind, void* t){ struct Cyc_Tcutil_CVTEnv _temp2787= Cyc_Tcutil_i_check_valid_type(
loc, te,({ struct Cyc_Tcutil_CVTEnv _temp2802; _temp2802.kind_env= kind_env;
_temp2802.free_vars= 0; _temp2802.free_evars= 0; _temp2802;}), expected_kind, t);{
struct Cyc_List_List* vs= _temp2787.free_vars; for( 0; vs != 0; vs=(( struct Cyc_List_List*)
_check_null( vs))->tl){ Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( vs))->hd);}}{ struct Cyc_List_List* evs=
_temp2787.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ void* _temp2788= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( evs))->hd); struct Cyc_Core_Opt* _temp2794; struct
Cyc_Core_Opt** _temp2796; _LL2790: if(( unsigned int) _temp2788 > 4u?*(( int*)
_temp2788) == Cyc_Absyn_Evar: 0){ _LL2795: _temp2794=(( struct Cyc_Absyn_Evar_struct*)
_temp2788)->f4; _temp2796=&(( struct Cyc_Absyn_Evar_struct*) _temp2788)->f4;
goto _LL2791;} else{ goto _LL2792;} _LL2792: goto _LL2793; _LL2791: if(*
_temp2796 == 0){* _temp2796=({ struct Cyc_Core_Opt* _temp2797=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2797->v=( void*) kind_env;
_temp2797;});} else{ struct Cyc_List_List* _temp2798=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp2796))->v; struct Cyc_List_List*
_temp2799= 0; for( 0; _temp2798 != 0; _temp2798=(( struct Cyc_List_List*)
_check_null( _temp2798))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp2798))->hd)){ _temp2799=({ struct Cyc_List_List* _temp2800=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2800->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp2798))->hd);
_temp2800->tl= _temp2799; _temp2800;});}}* _temp2796=({ struct Cyc_Core_Opt*
_temp2801=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2801->v=( void*) _temp2799; _temp2801;});} goto _LL2789; _LL2793: goto
_LL2789; _LL2789:;}} return _temp2787.free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* _temp2803= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= _temp2803; for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( x))->hd)->kind);
void* _temp2804=( void*) c->v; void* _temp2812; _LL2806: if( _temp2804 ==( void*)
Cyc_Absyn_No_constr){ goto _LL2807;} else{ goto _LL2808;} _LL2808: if((
unsigned int) _temp2804 > 1u?*(( int*) _temp2804) == Cyc_Absyn_Eq_constr: 0){
_LL2813: _temp2812=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2804)->f1;
if( _temp2812 ==( void*) Cyc_Absyn_MemKind){ goto _LL2809;} else{ goto _LL2810;}}
else{ goto _LL2810;} _LL2810: goto _LL2811; _LL2807: goto _LL2809; _LL2809:(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2814=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2814[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2815; _temp2815.tag= Cyc_Absyn_Eq_constr;
_temp2815.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2815;}); _temp2814;})));
goto _LL2805; _LL2811: goto _LL2805; _LL2805:;}} if( _temp2803 != 0){ void*
_temp2816= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2822; struct
Cyc_List_List* _temp2824; struct Cyc_List_List* _temp2826; struct Cyc_Absyn_VarargInfo*
_temp2828; int _temp2830; struct Cyc_List_List* _temp2832; void* _temp2834;
struct Cyc_Core_Opt* _temp2836; struct Cyc_List_List* _temp2838; struct Cyc_List_List**
_temp2840; _LL2818: if(( unsigned int) _temp2816 > 4u?*(( int*) _temp2816) ==
Cyc_Absyn_FnType: 0){ _LL2823: _temp2822=(( struct Cyc_Absyn_FnType_struct*)
_temp2816)->f1; _LL2839: _temp2838= _temp2822.tvars; _temp2840=&((( struct Cyc_Absyn_FnType_struct*)
_temp2816)->f1).tvars; goto _LL2837; _LL2837: _temp2836= _temp2822.effect; goto
_LL2835; _LL2835: _temp2834=( void*) _temp2822.ret_typ; goto _LL2833; _LL2833:
_temp2832= _temp2822.args; goto _LL2831; _LL2831: _temp2830= _temp2822.c_varargs;
goto _LL2829; _LL2829: _temp2828= _temp2822.cyc_varargs; goto _LL2827; _LL2827:
_temp2826= _temp2822.rgn_po; goto _LL2825; _LL2825: _temp2824= _temp2822.attributes;
goto _LL2819;} else{ goto _LL2820;} _LL2820: goto _LL2821; _LL2819: if(*
_temp2840 == 0){* _temp2840= _temp2803;} else{ if( _temp2803 != 0){ struct
_tagged_arr _temp2841=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp2803))->hd)->name; if(*(( const unsigned char*)
_check_unknown_subscript( _temp2841, sizeof( unsigned char), 0)) =='?'){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr) xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp2842=
_temp2841; xprintf("unbound type variable %.*s", _get_arr_size( _temp2842, 1u),
_temp2842.curr);}));}}} goto _LL2817; _LL2821: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp2843=*(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( _temp2803))->hd)->name; xprintf("unbound type variable %.*s",
_get_arr_size( _temp2843, 1u), _temp2843.curr);})); return; _LL2817:;}} void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){ tv->identity= Cyc_Tcutil_new_tvar_id();}}
void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List* tvs){(( void(*)( void(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity,
tvs);} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
_temp2844= Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, t);
struct Cyc_List_List* _temp2845= Cyc_Tcutil_remove_bound_tvars( _temp2844,
bound_tvars); struct Cyc_List_List* fs= _temp2845; for( 0; fs != 0; fs=(( struct
Cyc_List_List*) _check_null( fs))->tl){ struct _tagged_arr* _temp2846=(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( fs))->hd)->name; Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2847=* _temp2846; struct
_tagged_arr _temp2848= Cyc_Absynpp_typ2string( t); xprintf("unbound type variable %.*s in type %.*s",
_get_arr_size( _temp2847, 1u), _temp2847.curr, _get_arr_size( _temp2848, 1u),
_temp2848.curr);}));}} static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)(
void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_arr(* a2string)( void*), struct _tagged_arr msg){ for( 0; vs != 0;
vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List* vs2=((
struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=(( struct
Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp2849=
msg; struct _tagged_arr _temp2850= a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd); xprintf("%.*s: %.*s", _get_arr_size( _temp2849, 1u),
_temp2849.curr, _get_arr_size( _temp2850, 1u), _temp2850.curr);}));}}}} static
struct _tagged_arr Cyc_Tcutil_strptr2string( struct _tagged_arr* s){ return* s;}
void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg){(( void(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_arr(* a2string)( struct _tagged_arr*), struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)( Cyc_String_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string,
msg);} void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs){(( void(*)( int(* cmp)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr(* a2string)( struct Cyc_Absyn_Tvar*), struct _tagged_arr
msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp, tvs, loc, Cyc_Tcutil_tvar2string,
_tag_arr("duplicate type variable", sizeof( unsigned char), 24u));} struct
_tuple17{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple18{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple19{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_String_strcmp(*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( sd_fields))->hd)->name,
_tag_arr("", sizeof( unsigned char), 1u)) != 0){ fields=({ struct Cyc_List_List*
_temp2851=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2851->hd=( void*)({ struct _tuple17* _temp2852=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp2852->f1=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( sd_fields))->hd; _temp2852->f2= 0; _temp2852;});
_temp2851->tl= fields; _temp2851;});}}} fields=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans=
0; for( 0; des != 0; des=(( struct Cyc_List_List*) _check_null( des))->tl){
struct _tuple18 _temp2855; void* _temp2856; struct Cyc_List_List* _temp2858;
struct _tuple18* _temp2853=( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd; _temp2855=* _temp2853; _LL2859: _temp2858= _temp2855.f1;
goto _LL2857; _LL2857: _temp2856= _temp2855.f2; goto _LL2854; _LL2854: if(
_temp2858 == 0){ struct Cyc_List_List* _temp2860= fields; for( 0; _temp2860 != 0;
_temp2860=(( struct Cyc_List_List*) _check_null( _temp2860))->tl){ if( !(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp2860))->hd)).f2){(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp2860))->hd)).f2= 1;(*((
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct
Cyc_List_List* _temp2861=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2861->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2862=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2862[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2863; _temp2863.tag= Cyc_Absyn_FieldName;
_temp2863.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp2860))->hd)).f1)->name; _temp2863;}); _temp2862;})); _temp2861->tl= 0;
_temp2861;}); ans=({ struct Cyc_List_List* _temp2864=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2864->hd=( void*)({
struct _tuple19* _temp2865=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp2865->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp2860))->hd)).f1; _temp2865->f2= _temp2856; _temp2865;});
_temp2864->tl= ans; _temp2864;}); break;}} if( _temp2860 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments to struct", sizeof( unsigned char), 29u));}}
else{ if((( struct Cyc_List_List*) _check_null( _temp2858))->tl != 0){ Cyc_Tcutil_terr(
loc, _tag_arr("multiple designators are not supported", sizeof( unsigned char),
39u));} else{ void* _temp2866=( void*)(( struct Cyc_List_List*) _check_null(
_temp2858))->hd; struct _tagged_arr* _temp2872; _LL2868: if(*(( int*) _temp2866)
== Cyc_Absyn_ArrayElement){ goto _LL2869;} else{ goto _LL2870;} _LL2870: if(*((
int*) _temp2866) == Cyc_Absyn_FieldName){ _LL2873: _temp2872=(( struct Cyc_Absyn_FieldName_struct*)
_temp2866)->f1; goto _LL2871;} else{ goto _LL2867;} _LL2869: Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u)); goto _LL2867; _LL2871: { struct Cyc_List_List* _temp2874=
fields; for( 0; _temp2874 != 0; _temp2874=(( struct Cyc_List_List*) _check_null(
_temp2874))->tl){ if( Cyc_String_zstrptrcmp( _temp2872,((*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp2874))->hd)).f1)->name) == 0){ if((*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp2874))->hd)).f2){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp2875=*
_temp2872; xprintf("field %.*s has already been used as an argument",
_get_arr_size( _temp2875, 1u), _temp2875.curr);}));}(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp2874))->hd)).f2= 1; ans=({ struct Cyc_List_List*
_temp2876=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2876->hd=( void*)({ struct _tuple19* _temp2877=( struct _tuple19*)
_region_malloc( rgn, sizeof( struct _tuple19)); _temp2877->f1=(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp2874))->hd)).f1; _temp2877->f2=
_temp2856; _temp2877;}); _temp2876->tl= ans; _temp2876;}); break;}} if(
_temp2874 == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp2878=* _temp2872; xprintf("bad field designator %.*s", _get_arr_size(
_temp2878, 1u), _temp2878.curr);}));} goto _LL2867;} _LL2867:;}}} for( 0; fields
!= 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ if( !(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( fields))->hd)).f2){ Cyc_Tcutil_terr(
loc, _tag_arr("too few arguments to struct", sizeof( unsigned char), 28u));
break;}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void** elt_typ_dest){
void* _temp2879= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp2885;
struct Cyc_Absyn_Conref* _temp2887; void* _temp2889; _LL2881: if(( unsigned int)
_temp2879 > 4u?*(( int*) _temp2879) == Cyc_Absyn_PointerType: 0){ _LL2886:
_temp2885=(( struct Cyc_Absyn_PointerType_struct*) _temp2879)->f1; _LL2890:
_temp2889=( void*) _temp2885.elt_typ; goto _LL2888; _LL2888: _temp2887=
_temp2885.bounds; goto _LL2882;} else{ goto _LL2883;} _LL2883: goto _LL2884;
_LL2882: { struct Cyc_Absyn_Conref* _temp2891= Cyc_Absyn_compress_conref(
_temp2887); void* _temp2892=( void*)( Cyc_Absyn_compress_conref( _temp2891))->v;
void* _temp2900; _LL2894: if(( unsigned int) _temp2892 > 1u?*(( int*) _temp2892)
== Cyc_Absyn_Eq_constr: 0){ _LL2901: _temp2900=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2892)->f1; if( _temp2900 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2895;}
else{ goto _LL2896;}} else{ goto _LL2896;} _LL2896: if( _temp2892 ==( void*) Cyc_Absyn_No_constr){
goto _LL2897;} else{ goto _LL2898;} _LL2898: goto _LL2899; _LL2895:*
elt_typ_dest= _temp2889; return 1; _LL2897:( void*)( _temp2891->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2902=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2902[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp2903; _temp2903.tag= Cyc_Absyn_Eq_constr;
_temp2903.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp2903;}); _temp2902;})));*
elt_typ_dest= _temp2889; return 1; _LL2899: return 0; _LL2893:;} _LL2884: return
0; _LL2880:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3085;
_temp3085.f1= 0; _temp3085.f2=( void*) Cyc_Absyn_HeapRgn; _temp3085;}); void*
_temp2904=( void*) e->r; void* _temp2918; struct _tuple1* _temp2920; struct
_tagged_arr* _temp2922; struct Cyc_Absyn_Exp* _temp2924; struct _tagged_arr*
_temp2926; struct Cyc_Absyn_Exp* _temp2928; struct Cyc_Absyn_Exp* _temp2930;
struct Cyc_Absyn_Exp* _temp2932; struct Cyc_Absyn_Exp* _temp2934; _LL2906: if(*((
int*) _temp2904) == Cyc_Absyn_Var_e){ _LL2921: _temp2920=(( struct Cyc_Absyn_Var_e_struct*)
_temp2904)->f1; goto _LL2919; _LL2919: _temp2918=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp2904)->f2; goto _LL2907;} else{ goto _LL2908;} _LL2908: if(*(( int*)
_temp2904) == Cyc_Absyn_StructMember_e){ _LL2925: _temp2924=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp2904)->f1; goto _LL2923; _LL2923: _temp2922=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp2904)->f2; goto _LL2909;} else{ goto _LL2910;} _LL2910: if(*(( int*)
_temp2904) == Cyc_Absyn_StructArrow_e){ _LL2929: _temp2928=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp2904)->f1; goto _LL2927; _LL2927: _temp2926=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp2904)->f2; goto _LL2911;} else{ goto _LL2912;} _LL2912: if(*(( int*)
_temp2904) == Cyc_Absyn_Deref_e){ _LL2931: _temp2930=(( struct Cyc_Absyn_Deref_e_struct*)
_temp2904)->f1; goto _LL2913;} else{ goto _LL2914;} _LL2914: if(*(( int*)
_temp2904) == Cyc_Absyn_Subscript_e){ _LL2935: _temp2934=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp2904)->f1; goto _LL2933; _LL2933: _temp2932=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp2904)->f2; goto _LL2915;} else{ goto _LL2916;} _LL2916: goto _LL2917;
_LL2907: { void* _temp2936= _temp2918; struct Cyc_Absyn_Vardecl* _temp2950;
struct Cyc_Absyn_Vardecl* _temp2952; struct Cyc_Absyn_Vardecl* _temp2954; struct
Cyc_Absyn_Vardecl* _temp2956; _LL2938: if( _temp2936 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL2939;} else{ goto _LL2940;} _LL2940: if(( unsigned int) _temp2936 > 1u?*((
int*) _temp2936) == Cyc_Absyn_Funname_b: 0){ goto _LL2941;} else{ goto _LL2942;}
_LL2942: if(( unsigned int) _temp2936 > 1u?*(( int*) _temp2936) == Cyc_Absyn_Global_b:
0){ _LL2951: _temp2950=(( struct Cyc_Absyn_Global_b_struct*) _temp2936)->f1;
goto _LL2943;} else{ goto _LL2944;} _LL2944: if(( unsigned int) _temp2936 > 1u?*((
int*) _temp2936) == Cyc_Absyn_Local_b: 0){ _LL2953: _temp2952=(( struct Cyc_Absyn_Local_b_struct*)
_temp2936)->f1; goto _LL2945;} else{ goto _LL2946;} _LL2946: if(( unsigned int)
_temp2936 > 1u?*(( int*) _temp2936) == Cyc_Absyn_Pat_b: 0){ _LL2955: _temp2954=((
struct Cyc_Absyn_Pat_b_struct*) _temp2936)->f1; goto _LL2947;} else{ goto
_LL2948;} _LL2948: if(( unsigned int) _temp2936 > 1u?*(( int*) _temp2936) == Cyc_Absyn_Param_b:
0){ _LL2957: _temp2956=(( struct Cyc_Absyn_Param_b_struct*) _temp2936)->f1; goto
_LL2949;} else{ goto _LL2937;} _LL2939: return bogus_ans; _LL2941: return({
struct _tuple7 _temp2958; _temp2958.f1= 1; _temp2958.f2=( void*) Cyc_Absyn_HeapRgn;
_temp2958;}); _LL2943: { void* _temp2959= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2961: if(( unsigned int) _temp2959
> 4u?*(( int*) _temp2959) == Cyc_Absyn_ArrayType: 0){ goto _LL2962;} else{ goto
_LL2963;} _LL2963: goto _LL2964; _LL2962: return({ struct _tuple7 _temp2965;
_temp2965.f1= 1; _temp2965.f2=( void*) Cyc_Absyn_HeapRgn; _temp2965;}); _LL2964:
return({ struct _tuple7 _temp2966; _temp2966.f1=( _temp2950->tq).q_const;
_temp2966.f2=( void*) Cyc_Absyn_HeapRgn; _temp2966;}); _LL2960:;} _LL2945: {
void* _temp2967= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL2969: if(( unsigned int) _temp2967 > 4u?*(( int*)
_temp2967) == Cyc_Absyn_ArrayType: 0){ goto _LL2970;} else{ goto _LL2971;}
_LL2971: goto _LL2972; _LL2970: return({ struct _tuple7 _temp2973; _temp2973.f1=
1; _temp2973.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2952->rgn))->v;
_temp2973;}); _LL2972: return({ struct _tuple7 _temp2974; _temp2974.f1=(
_temp2952->tq).q_const; _temp2974.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2952->rgn))->v; _temp2974;}); _LL2968:;} _LL2947: _temp2956=
_temp2954; goto _LL2949; _LL2949: return({ struct _tuple7 _temp2975; _temp2975.f1=(
_temp2956->tq).q_const; _temp2975.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2956->rgn))->v; _temp2975;}); _LL2937:;} _LL2909: { void*
_temp2976= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp2924->topt))->v); struct Cyc_List_List* _temp2988; struct Cyc_List_List*
_temp2990; struct Cyc_Absyn_Structdecl** _temp2992; struct Cyc_Absyn_Structdecl*
_temp2994; struct Cyc_Absyn_Uniondecl** _temp2995; struct Cyc_Absyn_Uniondecl*
_temp2997; _LL2978: if(( unsigned int) _temp2976 > 4u?*(( int*) _temp2976) ==
Cyc_Absyn_AnonStructType: 0){ _LL2989: _temp2988=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp2976)->f1; goto _LL2979;} else{ goto _LL2980;} _LL2980: if(( unsigned int)
_temp2976 > 4u?*(( int*) _temp2976) == Cyc_Absyn_AnonUnionType: 0){ _LL2991:
_temp2990=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2976)->f1; goto
_LL2981;} else{ goto _LL2982;} _LL2982: if(( unsigned int) _temp2976 > 4u?*((
int*) _temp2976) == Cyc_Absyn_StructType: 0){ _LL2993: _temp2992=(( struct Cyc_Absyn_StructType_struct*)
_temp2976)->f3; if( _temp2992 == 0){ goto _LL2984;} else{ _temp2994=* _temp2992;
goto _LL2983;}} else{ goto _LL2984;} _LL2984: if(( unsigned int) _temp2976 > 4u?*((
int*) _temp2976) == Cyc_Absyn_UnionType: 0){ _LL2996: _temp2995=(( struct Cyc_Absyn_UnionType_struct*)
_temp2976)->f3; if( _temp2995 == 0){ goto _LL2986;} else{ _temp2997=* _temp2995;
goto _LL2985;}} else{ goto _LL2986;} _LL2986: goto _LL2987; _LL2979: _temp2990=
_temp2988; goto _LL2981; _LL2981: { struct Cyc_Absyn_Structfield* _temp2998= Cyc_Absyn_lookup_field(
_temp2990, _temp2922); if( _temp2998 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp2998))->width != 0: 0){ return({ struct _tuple7 _temp2999;
_temp2999.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp2998))->tq).q_const;
_temp2999.f2=( Cyc_Tcutil_addressof_props( te, _temp2924)).f2; _temp2999;});}
return bogus_ans;} _LL2983: { struct Cyc_Absyn_Structfield* _temp3000= Cyc_Absyn_lookup_struct_field(
_temp2994, _temp2922); if( _temp3000 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3000))->width != 0: 0){ return({ struct _tuple7 _temp3001;
_temp3001.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3000))->tq).q_const;
_temp3001.f2=( Cyc_Tcutil_addressof_props( te, _temp2924)).f2; _temp3001;});}
return bogus_ans;} _LL2985: { struct Cyc_Absyn_Structfield* _temp3002= Cyc_Absyn_lookup_union_field(
_temp2997, _temp2922); if( _temp3002 != 0){ return({ struct _tuple7 _temp3003;
_temp3003.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3002))->tq).q_const;
_temp3003.f2=( Cyc_Tcutil_addressof_props( te, _temp2924)).f2; _temp3003;});}
goto _LL2987;} _LL2987: return bogus_ans; _LL2977:;} _LL2911: { void* _temp3004=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2928->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3010; void* _temp3012; void* _temp3014; _LL3006:
if(( unsigned int) _temp3004 > 4u?*(( int*) _temp3004) == Cyc_Absyn_PointerType:
0){ _LL3011: _temp3010=(( struct Cyc_Absyn_PointerType_struct*) _temp3004)->f1;
_LL3015: _temp3014=( void*) _temp3010.elt_typ; goto _LL3013; _LL3013: _temp3012=(
void*) _temp3010.rgn_typ; goto _LL3007;} else{ goto _LL3008;} _LL3008: goto
_LL3009; _LL3007: { void* _temp3016= Cyc_Tcutil_compress( _temp3014); struct Cyc_List_List*
_temp3028; struct Cyc_List_List* _temp3030; struct Cyc_Absyn_Structdecl**
_temp3032; struct Cyc_Absyn_Structdecl* _temp3034; struct Cyc_Absyn_Uniondecl**
_temp3035; struct Cyc_Absyn_Uniondecl* _temp3037; _LL3018: if(( unsigned int)
_temp3016 > 4u?*(( int*) _temp3016) == Cyc_Absyn_AnonStructType: 0){ _LL3029:
_temp3028=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3016)->f1; goto
_LL3019;} else{ goto _LL3020;} _LL3020: if(( unsigned int) _temp3016 > 4u?*((
int*) _temp3016) == Cyc_Absyn_AnonUnionType: 0){ _LL3031: _temp3030=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3016)->f1; goto _LL3021;} else{ goto
_LL3022;} _LL3022: if(( unsigned int) _temp3016 > 4u?*(( int*) _temp3016) == Cyc_Absyn_StructType:
0){ _LL3033: _temp3032=(( struct Cyc_Absyn_StructType_struct*) _temp3016)->f3;
if( _temp3032 == 0){ goto _LL3024;} else{ _temp3034=* _temp3032; goto _LL3023;}}
else{ goto _LL3024;} _LL3024: if(( unsigned int) _temp3016 > 4u?*(( int*)
_temp3016) == Cyc_Absyn_UnionType: 0){ _LL3036: _temp3035=(( struct Cyc_Absyn_UnionType_struct*)
_temp3016)->f3; if( _temp3035 == 0){ goto _LL3026;} else{ _temp3037=* _temp3035;
goto _LL3025;}} else{ goto _LL3026;} _LL3026: goto _LL3027; _LL3019: _temp3030=
_temp3028; goto _LL3021; _LL3021: { struct Cyc_Absyn_Structfield* _temp3038= Cyc_Absyn_lookup_field(
_temp3030, _temp2926); if( _temp3038 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3038))->width != 0: 0){ return({ struct _tuple7 _temp3039;
_temp3039.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3038))->tq).q_const;
_temp3039.f2= _temp3012; _temp3039;});} return bogus_ans;} _LL3023: { struct Cyc_Absyn_Structfield*
_temp3040= Cyc_Absyn_lookup_struct_field( _temp3034, _temp2926); if( _temp3040
!= 0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3040))->width != 0: 0){
return({ struct _tuple7 _temp3041; _temp3041.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3040))->tq).q_const; _temp3041.f2= _temp3012; _temp3041;});}
return bogus_ans;} _LL3025: { struct Cyc_Absyn_Structfield* _temp3042= Cyc_Absyn_lookup_union_field(
_temp3037, _temp2926); if( _temp3042 != 0){ return({ struct _tuple7 _temp3043;
_temp3043.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3042))->tq).q_const;
_temp3043.f2= _temp3012; _temp3043;});} return bogus_ans;} _LL3027: return
bogus_ans; _LL3017:;} _LL3009: return bogus_ans; _LL3005:;} _LL2913: { void*
_temp3044= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp2930->topt))->v); struct Cyc_Absyn_PtrInfo _temp3050; struct Cyc_Absyn_Tqual
_temp3052; void* _temp3054; _LL3046: if(( unsigned int) _temp3044 > 4u?*(( int*)
_temp3044) == Cyc_Absyn_PointerType: 0){ _LL3051: _temp3050=(( struct Cyc_Absyn_PointerType_struct*)
_temp3044)->f1; _LL3055: _temp3054=( void*) _temp3050.rgn_typ; goto _LL3053;
_LL3053: _temp3052= _temp3050.tq; goto _LL3047;} else{ goto _LL3048;} _LL3048:
goto _LL3049; _LL3047: return({ struct _tuple7 _temp3056; _temp3056.f1=
_temp3052.q_const; _temp3056.f2= _temp3054; _temp3056;}); _LL3049: return
bogus_ans; _LL3045:;} _LL2915: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp2934->topt))->v); void* _temp3057= t; struct
Cyc_Absyn_Tqual _temp3067; struct Cyc_List_List* _temp3069; struct Cyc_Absyn_PtrInfo
_temp3071; struct Cyc_Absyn_Conref* _temp3073; struct Cyc_Absyn_Tqual _temp3075;
void* _temp3077; void* _temp3079; _LL3059: if(( unsigned int) _temp3057 > 4u?*((
int*) _temp3057) == Cyc_Absyn_ArrayType: 0){ _LL3068: _temp3067=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3057)->f2; goto _LL3060;} else{ goto _LL3061;} _LL3061: if(( unsigned int)
_temp3057 > 4u?*(( int*) _temp3057) == Cyc_Absyn_TupleType: 0){ _LL3070:
_temp3069=(( struct Cyc_Absyn_TupleType_struct*) _temp3057)->f1; goto _LL3062;}
else{ goto _LL3063;} _LL3063: if(( unsigned int) _temp3057 > 4u?*(( int*)
_temp3057) == Cyc_Absyn_PointerType: 0){ _LL3072: _temp3071=(( struct Cyc_Absyn_PointerType_struct*)
_temp3057)->f1; _LL3080: _temp3079=( void*) _temp3071.elt_typ; goto _LL3078;
_LL3078: _temp3077=( void*) _temp3071.rgn_typ; goto _LL3076; _LL3076: _temp3075=
_temp3071.tq; goto _LL3074; _LL3074: _temp3073= _temp3071.bounds; goto _LL3064;}
else{ goto _LL3065;} _LL3065: goto _LL3066; _LL3060: return({ struct _tuple7
_temp3081; _temp3081.f1= _temp3067.q_const; _temp3081.f2=( Cyc_Tcutil_addressof_props(
te, _temp2934)).f2; _temp3081;}); _LL3062: { struct _tuple4* _temp3082= Cyc_Absyn_lookup_tuple_field(
_temp3069,( int) Cyc_Evexp_eval_const_uint_exp( _temp2932)); if( _temp3082 != 0){
return({ struct _tuple7 _temp3083; _temp3083.f1=((*(( struct _tuple4*)
_check_null( _temp3082))).f1).q_const; _temp3083.f2=( Cyc_Tcutil_addressof_props(
te, _temp2934)).f2; _temp3083;});} return bogus_ans;} _LL3064: return({ struct
_tuple7 _temp3084; _temp3084.f1= _temp3075.q_const; _temp3084.f2= _temp3077;
_temp3084;}); _LL3066: return bogus_ans; _LL3058:;} _LL2917: Cyc_Tcutil_terr( e->loc,
_tag_arr("unary & applied to non-lvalue", sizeof( unsigned char), 30u)); return
bogus_ans; _LL2905:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3086= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Tqual _temp3092; void* _temp3094; _LL3088: if((
unsigned int) _temp3086 > 4u?*(( int*) _temp3086) == Cyc_Absyn_ArrayType: 0){
_LL3095: _temp3094=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3086)->f1;
goto _LL3093; _LL3093: _temp3092=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3086)->f2; goto _LL3089;} else{ goto _LL3090;} _LL3090: goto _LL3091;
_LL3089: { void* _temp3098; struct _tuple7 _temp3096= Cyc_Tcutil_addressof_props(
te, e); _LL3099: _temp3098= _temp3096.f2; goto _LL3097; _LL3097: return Cyc_Absyn_atb_typ(
_temp3094, _temp3098, _temp3092,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3100=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3100[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3101; _temp3101.tag= Cyc_Absyn_Upper_b;
_temp3101.f1= e; _temp3101;}); _temp3100;}));} _LL3091: return e_typ; _LL3087:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3102=(
void*) b->v; void* _temp3110; void* _temp3112; struct Cyc_Absyn_Exp* _temp3114;
_LL3104: if(( unsigned int) _temp3102 > 1u?*(( int*) _temp3102) == Cyc_Absyn_Eq_constr:
0){ _LL3111: _temp3110=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3102)->f1;
if( _temp3110 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3105;} else{ goto _LL3106;}}
else{ goto _LL3106;} _LL3106: if(( unsigned int) _temp3102 > 1u?*(( int*)
_temp3102) == Cyc_Absyn_Eq_constr: 0){ _LL3113: _temp3112=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3102)->f1; if(( unsigned int) _temp3112 > 1u?*(( int*) _temp3112) == Cyc_Absyn_Upper_b:
0){ _LL3115: _temp3114=(( struct Cyc_Absyn_Upper_b_struct*) _temp3112)->f1; goto
_LL3107;} else{ goto _LL3108;}} else{ goto _LL3108;} _LL3108: goto _LL3109;
_LL3105: return; _LL3107: if( Cyc_Evexp_eval_const_uint_exp( _temp3114) <= i){
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u));} return; _LL3109:( void*)( b->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp3116=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3116[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3117; _temp3117.tag= Cyc_Absyn_Eq_constr;
_temp3117.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3118=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3118[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3119; _temp3119.tag= Cyc_Absyn_Upper_b;
_temp3119.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3119;}); _temp3118;}));
_temp3117;}); _temp3116;}))); return; _LL3103:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3120= Cyc_Tcutil_compress(
t); void* _temp3146; struct Cyc_List_List* _temp3148; struct Cyc_Absyn_Structdecl**
_temp3150; struct Cyc_List_List* _temp3152; struct Cyc_Absyn_Uniondecl**
_temp3154; struct Cyc_List_List* _temp3156; struct Cyc_List_List* _temp3158;
struct Cyc_List_List* _temp3160; _LL3122: if( _temp3120 ==( void*) Cyc_Absyn_VoidType){
goto _LL3123;} else{ goto _LL3124;} _LL3124: if(( unsigned int) _temp3120 > 4u?*((
int*) _temp3120) == Cyc_Absyn_IntType: 0){ goto _LL3125;} else{ goto _LL3126;}
_LL3126: if( _temp3120 ==( void*) Cyc_Absyn_FloatType){ goto _LL3127;} else{
goto _LL3128;} _LL3128: if( _temp3120 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL3129;} else{ goto _LL3130;} _LL3130: if(( unsigned int) _temp3120 > 4u?*((
int*) _temp3120) == Cyc_Absyn_EnumType: 0){ goto _LL3131;} else{ goto _LL3132;}
_LL3132: if(( unsigned int) _temp3120 > 4u?*(( int*) _temp3120) == Cyc_Absyn_ArrayType:
0){ _LL3147: _temp3146=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3120)->f1;
goto _LL3133;} else{ goto _LL3134;} _LL3134: if(( unsigned int) _temp3120 > 4u?*((
int*) _temp3120) == Cyc_Absyn_TupleType: 0){ _LL3149: _temp3148=(( struct Cyc_Absyn_TupleType_struct*)
_temp3120)->f1; goto _LL3135;} else{ goto _LL3136;} _LL3136: if(( unsigned int)
_temp3120 > 4u?*(( int*) _temp3120) == Cyc_Absyn_StructType: 0){ _LL3153:
_temp3152=(( struct Cyc_Absyn_StructType_struct*) _temp3120)->f2; goto _LL3151;
_LL3151: _temp3150=(( struct Cyc_Absyn_StructType_struct*) _temp3120)->f3; goto
_LL3137;} else{ goto _LL3138;} _LL3138: if(( unsigned int) _temp3120 > 4u?*((
int*) _temp3120) == Cyc_Absyn_UnionType: 0){ _LL3157: _temp3156=(( struct Cyc_Absyn_UnionType_struct*)
_temp3120)->f2; goto _LL3155; _LL3155: _temp3154=(( struct Cyc_Absyn_UnionType_struct*)
_temp3120)->f3; goto _LL3139;} else{ goto _LL3140;} _LL3140: if(( unsigned int)
_temp3120 > 4u?*(( int*) _temp3120) == Cyc_Absyn_AnonStructType: 0){ _LL3159:
_temp3158=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3120)->f1; goto
_LL3141;} else{ goto _LL3142;} _LL3142: if(( unsigned int) _temp3120 > 4u?*((
int*) _temp3120) == Cyc_Absyn_AnonUnionType: 0){ _LL3161: _temp3160=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3120)->f1; goto _LL3143;} else{ goto
_LL3144;} _LL3144: goto _LL3145; _LL3123: return 1; _LL3125: return 1; _LL3127:
return 1; _LL3129: return 1; _LL3131: return 0; _LL3133: return Cyc_Tcutil_bits_only(
_temp3146); _LL3135: for( 0; _temp3148 != 0; _temp3148=(( struct Cyc_List_List*)
_check_null( _temp3148))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3148))->hd)).f2)){ return 0;}} return 1;
_LL3137: if( _temp3150 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3162=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3150)); if( _temp3162->fields
== 0){ return 0;}{ struct _RegionHandle _temp3163= _new_region(); struct
_RegionHandle* rgn=& _temp3163; _push_region( rgn);{ struct Cyc_List_List*
_temp3164=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3162->tvs, _temp3152);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3162->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3164,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3165= 0; _npop_handler( 0u); return
_temp3165;}}}{ int _temp3166= 1; _npop_handler( 0u); return _temp3166;}};
_pop_region( rgn);}} _LL3139: if( _temp3154 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3167=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3154)); if(
_temp3167->fields == 0){ return 0;}{ struct _RegionHandle _temp3168= _new_region();
struct _RegionHandle* rgn=& _temp3168; _push_region( rgn);{ struct Cyc_List_List*
_temp3169=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3167->tvs, _temp3156);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3167->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3169,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3170= 0; _npop_handler( 0u); return
_temp3170;}}}{ int _temp3171= 1; _npop_handler( 0u); return _temp3171;}};
_pop_region( rgn);}} _LL3141: _temp3160= _temp3158; goto _LL3143; _LL3143: for(
0; _temp3160 != 0; _temp3160=(( struct Cyc_List_List*) _check_null( _temp3160))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3160))->hd)->type)){ return 0;}} return 1;
_LL3145: return 0; _LL3121:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3172=( void*) e->r; struct
_tuple1* _temp3212; struct Cyc_Absyn_Exp* _temp3214; struct Cyc_Absyn_Exp*
_temp3216; struct Cyc_Absyn_Exp* _temp3218; struct Cyc_Absyn_Exp* _temp3220;
struct Cyc_Absyn_Exp* _temp3222; struct Cyc_Absyn_Exp* _temp3224; struct Cyc_Absyn_Exp*
_temp3226; struct Cyc_Absyn_Exp* _temp3228; void* _temp3230; struct Cyc_Absyn_Exp*
_temp3232; struct Cyc_Absyn_Exp* _temp3234; struct Cyc_Absyn_Exp* _temp3236;
struct Cyc_List_List* _temp3238; struct Cyc_List_List* _temp3240; struct Cyc_List_List*
_temp3242; struct Cyc_List_List* _temp3244; void* _temp3246; struct Cyc_List_List*
_temp3248; struct Cyc_List_List* _temp3250; _LL3174: if(*(( int*) _temp3172) ==
Cyc_Absyn_Const_e){ goto _LL3175;} else{ goto _LL3176;} _LL3176: if(*(( int*)
_temp3172) == Cyc_Absyn_Sizeoftyp_e){ goto _LL3177;} else{ goto _LL3178;}
_LL3178: if(*(( int*) _temp3172) == Cyc_Absyn_Sizeofexp_e){ goto _LL3179;} else{
goto _LL3180;} _LL3180: if(*(( int*) _temp3172) == Cyc_Absyn_Enum_e){ goto
_LL3181;} else{ goto _LL3182;} _LL3182: if(*(( int*) _temp3172) == Cyc_Absyn_Var_e){
_LL3213: _temp3212=(( struct Cyc_Absyn_Var_e_struct*) _temp3172)->f1; goto
_LL3183;} else{ goto _LL3184;} _LL3184: if(*(( int*) _temp3172) == Cyc_Absyn_Conditional_e){
_LL3219: _temp3218=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3172)->f1;
goto _LL3217; _LL3217: _temp3216=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3172)->f2; goto _LL3215; _LL3215: _temp3214=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3172)->f3; goto _LL3185;} else{ goto _LL3186;} _LL3186: if(*(( int*)
_temp3172) == Cyc_Absyn_SeqExp_e){ _LL3223: _temp3222=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3172)->f1; goto _LL3221; _LL3221: _temp3220=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3172)->f2; goto _LL3187;} else{ goto _LL3188;} _LL3188: if(*(( int*)
_temp3172) == Cyc_Absyn_NoInstantiate_e){ _LL3225: _temp3224=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3172)->f1; goto _LL3189;} else{ goto _LL3190;} _LL3190: if(*(( int*)
_temp3172) == Cyc_Absyn_Instantiate_e){ _LL3227: _temp3226=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3172)->f1; goto _LL3191;} else{ goto _LL3192;} _LL3192: if(*(( int*)
_temp3172) == Cyc_Absyn_Cast_e){ _LL3231: _temp3230=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3172)->f1; goto _LL3229; _LL3229: _temp3228=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3172)->f2; goto _LL3193;} else{ goto _LL3194;} _LL3194: if(*(( int*)
_temp3172) == Cyc_Absyn_Address_e){ _LL3233: _temp3232=(( struct Cyc_Absyn_Address_e_struct*)
_temp3172)->f1; goto _LL3195;} else{ goto _LL3196;} _LL3196: if(*(( int*)
_temp3172) == Cyc_Absyn_Comprehension_e){ _LL3237: _temp3236=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3172)->f2; goto _LL3235; _LL3235: _temp3234=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3172)->f3; goto _LL3197;} else{ goto _LL3198;} _LL3198: if(*(( int*)
_temp3172) == Cyc_Absyn_Array_e){ _LL3239: _temp3238=(( struct Cyc_Absyn_Array_e_struct*)
_temp3172)->f1; goto _LL3199;} else{ goto _LL3200;} _LL3200: if(*(( int*)
_temp3172) == Cyc_Absyn_AnonStruct_e){ _LL3241: _temp3240=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3172)->f2; goto _LL3201;} else{ goto _LL3202;} _LL3202: if(*(( int*)
_temp3172) == Cyc_Absyn_Struct_e){ _LL3243: _temp3242=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3172)->f3; goto _LL3203;} else{ goto _LL3204;} _LL3204: if(*(( int*)
_temp3172) == Cyc_Absyn_Primop_e){ _LL3247: _temp3246=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3172)->f1; goto _LL3245; _LL3245: _temp3244=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3172)->f2; goto _LL3205;} else{ goto _LL3206;} _LL3206: if(*(( int*)
_temp3172) == Cyc_Absyn_Tuple_e){ _LL3249: _temp3248=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3172)->f1; goto _LL3207;} else{ goto _LL3208;} _LL3208: if(*(( int*)
_temp3172) == Cyc_Absyn_Tunion_e){ _LL3251: _temp3250=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3172)->f3; goto _LL3209;} else{ goto _LL3210;} _LL3210: goto _LL3211;
_LL3175: return 1; _LL3177: return 1; _LL3179: return 1; _LL3181: return 1;
_LL3183: { struct _handler_cons _temp3252; _push_handler(& _temp3252);{ int
_temp3254= 0; if( setjmp( _temp3252.handler)){ _temp3254= 1;} if( ! _temp3254){{
void* _temp3255= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3212); void*
_temp3263; void* _temp3265; struct Cyc_Absyn_Vardecl* _temp3267; _LL3257: if(*((
int*) _temp3255) == Cyc_Tcenv_VarRes){ _LL3264: _temp3263=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3255)->f1; if(( unsigned int) _temp3263 > 1u?*(( int*) _temp3263) == Cyc_Absyn_Funname_b:
0){ goto _LL3258;} else{ goto _LL3259;}} else{ goto _LL3259;} _LL3259: if(*((
int*) _temp3255) == Cyc_Tcenv_VarRes){ _LL3266: _temp3265=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3255)->f1; if(( unsigned int) _temp3265 > 1u?*(( int*) _temp3265) == Cyc_Absyn_Global_b:
0){ _LL3268: _temp3267=(( struct Cyc_Absyn_Global_b_struct*) _temp3265)->f1;
goto _LL3260;} else{ goto _LL3261;}} else{ goto _LL3261;} _LL3261: goto _LL3262;
_LL3258: { int _temp3269= 1; _npop_handler( 0u); return _temp3269;} _LL3260: {
void* _temp3270= Cyc_Tcutil_compress(( void*) _temp3267->type); _LL3272: if((
unsigned int) _temp3270 > 4u?*(( int*) _temp3270) == Cyc_Absyn_ArrayType: 0){
goto _LL3273;} else{ goto _LL3274;} _LL3274: goto _LL3275; _LL3273: { int
_temp3276= 1; _npop_handler( 0u); return _temp3276;} _LL3275: { int _temp3277=
var_okay; _npop_handler( 0u); return _temp3277;} _LL3271:;} _LL3262: { int
_temp3278= var_okay; _npop_handler( 0u); return _temp3278;} _LL3256:;};
_pop_handler();} else{ void* _temp3253=( void*) _exn_thrown; void* _temp3280=
_temp3253; _LL3282: if( _temp3280 == Cyc_Dict_Absent){ goto _LL3283;} else{ goto
_LL3284;} _LL3284: goto _LL3285; _LL3283: return 0; _LL3285:( void) _throw(
_temp3280); _LL3281:;}}} _LL3185: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3218)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3216): 0)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3214): 0; _LL3187: return Cyc_Tcutil_cnst_exp( te, 0, _temp3222)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3220): 0; _LL3189: _temp3226= _temp3224; goto _LL3191; _LL3191:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3226); _LL3193: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3228); _LL3195: return Cyc_Tcutil_cnst_exp( te, 1, _temp3232);
_LL3197: return Cyc_Tcutil_cnst_exp( te, 0, _temp3236)? Cyc_Tcutil_cnst_exp( te,
0, _temp3234): 0; _LL3199: _temp3240= _temp3238; goto _LL3201; _LL3201:
_temp3242= _temp3240; goto _LL3203; _LL3203: for( 0; _temp3242 != 0; _temp3242=((
struct Cyc_List_List*) _check_null( _temp3242))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3242))->hd)).f2)){
return 0;}} return 1; _LL3205: _temp3248= _temp3244; goto _LL3207; _LL3207:
_temp3250= _temp3248; goto _LL3209; _LL3209: for( 0; _temp3250 != 0; _temp3250=((
struct Cyc_List_List*) _check_null( _temp3250))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3250))->hd)){
return 0;}} return 1; _LL3211: return 0; _LL3173:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3286= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3324; struct Cyc_Absyn_Conref* _temp3326; struct Cyc_Absyn_Conref*
_temp3328; void* _temp3330; void* _temp3332; struct Cyc_List_List* _temp3334;
struct Cyc_Absyn_Structdecl** _temp3336; struct Cyc_List_List* _temp3338; struct
Cyc_Absyn_Uniondecl** _temp3340; struct Cyc_List_List* _temp3342; struct Cyc_List_List*
_temp3344; struct Cyc_List_List* _temp3346; _LL3288: if( _temp3286 ==( void*)
Cyc_Absyn_VoidType){ goto _LL3289;} else{ goto _LL3290;} _LL3290: if((
unsigned int) _temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_Evar: 0){ goto
_LL3291;} else{ goto _LL3292;} _LL3292: if(( unsigned int) _temp3286 > 4u?*((
int*) _temp3286) == Cyc_Absyn_VarType: 0){ goto _LL3293;} else{ goto _LL3294;}
_LL3294: if(( unsigned int) _temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_TunionType:
0){ goto _LL3295;} else{ goto _LL3296;} _LL3296: if(( unsigned int) _temp3286 >
4u?*(( int*) _temp3286) == Cyc_Absyn_TunionFieldType: 0){ goto _LL3297;} else{
goto _LL3298;} _LL3298: if(( unsigned int) _temp3286 > 4u?*(( int*) _temp3286)
== Cyc_Absyn_PointerType: 0){ _LL3325: _temp3324=(( struct Cyc_Absyn_PointerType_struct*)
_temp3286)->f1; _LL3331: _temp3330=( void*) _temp3324.elt_typ; goto _LL3329;
_LL3329: _temp3328= _temp3324.nullable; goto _LL3327; _LL3327: _temp3326=
_temp3324.bounds; goto _LL3299;} else{ goto _LL3300;} _LL3300: if(( unsigned int)
_temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_IntType: 0){ goto _LL3301;}
else{ goto _LL3302;} _LL3302: if( _temp3286 ==( void*) Cyc_Absyn_FloatType){
goto _LL3303;} else{ goto _LL3304;} _LL3304: if( _temp3286 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3305;} else{ goto _LL3306;} _LL3306: if(( unsigned int) _temp3286 > 4u?*((
int*) _temp3286) == Cyc_Absyn_ArrayType: 0){ _LL3333: _temp3332=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3286)->f1; goto _LL3307;} else{ goto
_LL3308;} _LL3308: if(( unsigned int) _temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_FnType:
0){ goto _LL3309;} else{ goto _LL3310;} _LL3310: if(( unsigned int) _temp3286 >
4u?*(( int*) _temp3286) == Cyc_Absyn_TupleType: 0){ _LL3335: _temp3334=(( struct
Cyc_Absyn_TupleType_struct*) _temp3286)->f1; goto _LL3311;} else{ goto _LL3312;}
_LL3312: if(( unsigned int) _temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_StructType:
0){ _LL3339: _temp3338=(( struct Cyc_Absyn_StructType_struct*) _temp3286)->f2;
goto _LL3337; _LL3337: _temp3336=(( struct Cyc_Absyn_StructType_struct*)
_temp3286)->f3; goto _LL3313;} else{ goto _LL3314;} _LL3314: if(( unsigned int)
_temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_UnionType: 0){ _LL3343:
_temp3342=(( struct Cyc_Absyn_UnionType_struct*) _temp3286)->f2; goto _LL3341;
_LL3341: _temp3340=(( struct Cyc_Absyn_UnionType_struct*) _temp3286)->f3; goto
_LL3315;} else{ goto _LL3316;} _LL3316: if(( unsigned int) _temp3286 > 4u?*((
int*) _temp3286) == Cyc_Absyn_AnonStructType: 0){ _LL3345: _temp3344=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3286)->f1; goto _LL3317;} else{ goto
_LL3318;} _LL3318: if(( unsigned int) _temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_AnonUnionType:
0){ _LL3347: _temp3346=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3286)->f1;
goto _LL3319;} else{ goto _LL3320;} _LL3320: if(( unsigned int) _temp3286 > 4u?*((
int*) _temp3286) == Cyc_Absyn_EnumType: 0){ goto _LL3321;} else{ goto _LL3322;}
_LL3322: goto _LL3323; _LL3289: return 1; _LL3291: return 0; _LL3293: return 0;
_LL3295: return 0; _LL3297: return 0; _LL3299: { void* _temp3348=( void*)( Cyc_Absyn_compress_conref(
_temp3326))->v; void* _temp3356; void* _temp3358; _LL3350: if(( unsigned int)
_temp3348 > 1u?*(( int*) _temp3348) == Cyc_Absyn_Eq_constr: 0){ _LL3357:
_temp3356=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3348)->f1; if(
_temp3356 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3351;} else{ goto _LL3352;}}
else{ goto _LL3352;} _LL3352: if(( unsigned int) _temp3348 > 1u?*(( int*)
_temp3348) == Cyc_Absyn_Eq_constr: 0){ _LL3359: _temp3358=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3348)->f1; if(( unsigned int) _temp3358 > 1u?*(( int*) _temp3358) == Cyc_Absyn_Upper_b:
0){ goto _LL3353;} else{ goto _LL3354;}} else{ goto _LL3354;} _LL3354: goto
_LL3355; _LL3351: return 1; _LL3353: { void* _temp3360=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3328))->v; int
_temp3366; _LL3362: if(( unsigned int) _temp3360 > 1u?*(( int*) _temp3360) ==
Cyc_Absyn_Eq_constr: 0){ _LL3367: _temp3366=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3360)->f1; goto _LL3363;} else{ goto _LL3364;} _LL3364: goto _LL3365;
_LL3363: return _temp3366; _LL3365: return 0; _LL3361:;} _LL3355: return 0;
_LL3349:;} _LL3301: return 1; _LL3303: return 1; _LL3305: return 1; _LL3307:
return Cyc_Tcutil_supports_default( _temp3332); _LL3309: return 0; _LL3311: for(
0; _temp3334 != 0; _temp3334=(( struct Cyc_List_List*) _check_null( _temp3334))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3334))->hd)).f2)){ return 0;}} return 1; _LL3313: if(
_temp3336 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3336)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3338,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3315: if( _temp3340 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3340)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3342,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3317: _temp3346= _temp3344; goto _LL3319; _LL3319: return Cyc_Tcutil_fields_support_default(
0, 0, _temp3346); _LL3321: return 1; _LL3323: return 0; _LL3287:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp3368= _new_region();
struct _RegionHandle* rgn=& _temp3368; _push_region( rgn);{ struct Cyc_List_List*
_temp3369=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null(
fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3369,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type); if( ! Cyc_Tcutil_supports_default(
t)){ int _temp3370= 0; _npop_handler( 0u); return _temp3370;}}}; _pop_region(
rgn);} return 1;}