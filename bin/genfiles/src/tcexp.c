#include "cyc_include.h"

typedef int Cyc_ptrdiff_t;
typedef unsigned int Cyc_size_t;
typedef unsigned short Cyc_wchar_t;
typedef unsigned int Cyc_wint_t;
typedef char Cyc_u_char;
typedef unsigned short Cyc_u_short;
typedef unsigned int Cyc_u_int;
typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort;
typedef unsigned int Cyc_uint;
typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t;
struct Cyc_timespec {
  int tv_sec;
  int tv_nsec;
};
struct Cyc_itimerspec {
  struct Cyc_timespec it_interval;
  struct Cyc_timespec it_value;
};
typedef int Cyc_daddr_t;
typedef char *Cyc_caddr_t;
typedef unsigned int Cyc_ino_t;
typedef unsigned int Cyc_vm_offset_t;
typedef unsigned int Cyc_vm_size_t;
typedef char Cyc_int8_t;
typedef char Cyc_u_int8_t;
typedef short Cyc_int16_t;
typedef unsigned short Cyc_u_int16_t;
typedef int Cyc_int32_t;
typedef unsigned int Cyc_u_int32_t;
typedef long long Cyc_int64_t;
typedef unsigned long long Cyc_u_int64_t;
typedef int Cyc_register_t;
typedef short Cyc_dev_t;
typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t;
typedef unsigned short Cyc_gid_t;
typedef int Cyc_pid_t;
typedef int Cyc_key_t;
typedef int Cyc_ssize_t;
typedef char *Cyc_addr_t;
typedef int Cyc_mode_t;
typedef unsigned short Cyc_nlink_t;
typedef int Cyc_fd_mask;
struct Cyc__types_fd_set {
  int fds_bits[8];
};
typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char *Cyc_Cstring;
typedef struct _tagged_string Cyc_string;
typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string *Cyc_stringptr;
typedef int Cyc_bool;
extern void *exit(int);
extern void *abort();
struct Cyc_Core_Opt {
  void *v;
};
typedef struct Cyc_Core_Opt *Cyc_Core_opt_t;
struct _tuple0 {
  void *f1;
  void *f2;
};
extern void *Cyc_Core_snd(struct _tuple0 *);
extern char Cyc_Core_InvalidArg[15];
struct Cyc_Core_InvalidArg_struct {
  char *tag;
  struct _tagged_string f1;
};
extern char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct {
  char *tag;
  struct _tagged_string f1;
};
extern char Cyc_Core_Impossible[15];
struct Cyc_Core_Impossible_struct {
  char *tag;
  struct _tagged_string f1;
};
extern char Cyc_Core_Not_found[14];
extern char Cyc_Core_Unreachable[16];
struct Cyc_Core_Unreachable_struct {
  char *tag;
  struct _tagged_string f1;
};
extern char *string_to_Cstring(struct _tagged_string);
extern char *underlying_Cstring(struct _tagged_string);
extern struct _tagged_string Cstring_to_string(char *);
extern int system(char *);
struct Cyc_List_List {
  void *hd;
  struct Cyc_List_List *tl;
};
typedef struct Cyc_List_List *Cyc_List_glist_t;
typedef struct Cyc_List_List *Cyc_List_list_t;
typedef struct Cyc_List_List *Cyc_List_List_t;
extern int Cyc_List_length(struct Cyc_List_List *x);
extern char Cyc_List_List_empty[15];
extern struct Cyc_List_List *Cyc_List_map(void *(*f)(void *),struct Cyc_List_List *x);
extern struct Cyc_List_List *Cyc_List_rmap_c(struct _RegionHandle *,void *(*f)(void *,
									       void *),
					     void *env,struct Cyc_List_List *x);
extern char Cyc_List_List_mismatch[18];
extern struct Cyc_List_List *Cyc_List_imp_rev(struct Cyc_List_List *x);
extern struct Cyc_List_List *Cyc_List_rappend(struct _RegionHandle *,struct Cyc_List_List *x,
					      struct Cyc_List_List *y);
extern char Cyc_List_Nth[8];
extern void *Cyc_List_nth(struct Cyc_List_List *x,int i);
extern struct Cyc_List_List *Cyc_List_zip(struct Cyc_List_List *x,struct Cyc_List_List *y);
extern struct Cyc_List_List *Cyc_List_rzip(struct _RegionHandle *r3,struct _RegionHandle *r4,
					   struct Cyc_List_List *x,struct Cyc_List_List *y);
struct Cyc_Lineno_Pos {
  struct _tagged_string logical_file;
  struct _tagged_string line;
  int line_no;
  int col;
};
typedef struct Cyc_Lineno_Pos *Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment *Cyc_Position_seg_t;
static const int Cyc_Position_Lex = 0;
static const int Cyc_Position_Parse = 1;
static const int Cyc_Position_Elab = 2;
typedef void *Cyc_Position_error_kind_t;
struct Cyc_Position_Error {
  struct _tagged_string source;
  struct Cyc_Position_Segment *seg;
  void *kind;
  struct _tagged_string desc;
};
typedef struct Cyc_Position_Error *Cyc_Position_error_t;
extern char Cyc_Position_Nocontext[14];
typedef struct _tagged_string *Cyc_Absyn_field_name_t;
typedef struct _tagged_string *Cyc_Absyn_var_t;
typedef struct _tagged_string *Cyc_Absyn_tvarname_t;
typedef void *Cyc_Absyn_nmspace_t;
struct _tuple1 {
  void *f1;
  struct _tagged_string *f2;
};
typedef struct _tuple1 *Cyc_Absyn_qvar_t;
typedef struct _tuple1 *Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1 *Cyc_Absyn_typedef_name_t;
typedef struct _tuple1 *Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual;
struct Cyc_Absyn_Conref;
struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_FnInfo;
struct Cyc_Absyn_TunionInfo;
struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_Exp;
struct Cyc_Absyn_Stmt;
struct Cyc_Absyn_Pat;
struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl;
struct Cyc_Absyn_Structdecl;
struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl;
struct Cyc_Absyn_Tunionfield;
struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl;
struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl;
struct Cyc_Absyn_Structfield;
typedef void *Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
typedef void *Cyc_Absyn_size_of_t;
typedef void *Cyc_Absyn_kind_t;
typedef struct Cyc_Absyn_Tvar *Cyc_Absyn_tvar_t;
typedef void *Cyc_Absyn_sign_t;
typedef struct Cyc_Absyn_Conref *Cyc_Absyn_conref_t;
typedef void *Cyc_Absyn_constraint_t;
typedef void *Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t;
typedef struct Cyc_Absyn_TunionFieldInfo Cyc_Absyn_tunion_field_info_t;
typedef void *Cyc_Absyn_type_t;
typedef void *Cyc_Absyn_rgntype_t;
typedef void *Cyc_Absyn_funcparams_t;
typedef void *Cyc_Absyn_type_modifier_t;
typedef void *Cyc_Absyn_cnst_t;
typedef void *Cyc_Absyn_primop_t;
typedef void *Cyc_Absyn_incrementor_t;
typedef void *Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp *Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp *Cyc_Absyn_exp_opt_t;
typedef void *Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt *Cyc_Absyn_stmt_t;
typedef struct Cyc_Absyn_Stmt *Cyc_Absyn_stmt_opt_t;
typedef void *Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat *Cyc_Absyn_pat_t;
typedef void *Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause *Cyc_Absyn_switch_clause_t;
typedef struct Cyc_Absyn_Fndecl *Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Structdecl *Cyc_Absyn_structdecl_t;
typedef struct Cyc_Absyn_Uniondecl *Cyc_Absyn_uniondecl_t;
typedef struct Cyc_Absyn_Tunionfield *Cyc_Absyn_tunionfield_t;
typedef struct Cyc_Absyn_Tuniondecl *Cyc_Absyn_tuniondecl_t;
typedef struct Cyc_Absyn_Typedefdecl *Cyc_Absyn_typedefdecl_t;
typedef struct Cyc_Absyn_Enumfield *Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl *Cyc_Absyn_enumdecl_t;
typedef struct Cyc_Absyn_Vardecl *Cyc_Absyn_vardecl_t;
typedef void *Cyc_Absyn_raw_decl_t;
typedef struct Cyc_Absyn_Decl *Cyc_Absyn_decl_t;
typedef void *Cyc_Absyn_designator_t;
typedef void *Cyc_Absyn_stmt_annot_t;
typedef void *Cyc_Absyn_attribute_t;
typedef struct Cyc_List_List *Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield *Cyc_Absyn_structfield_t;
static const int Cyc_Absyn_Loc_n = 0;
static const int Cyc_Absyn_Rel_n = 0;
struct Cyc_Absyn_Rel_n_struct {
  int tag;
  struct Cyc_List_List *f1;
};
static const int Cyc_Absyn_Abs_n = 1;
struct Cyc_Absyn_Abs_n_struct {
  int tag;
  struct Cyc_List_List *f1;
};
static const int Cyc_Absyn_Static = 0;
static const int Cyc_Absyn_Abstract = 1;
static const int Cyc_Absyn_Public = 2;
static const int Cyc_Absyn_Extern = 3;
static const int Cyc_Absyn_ExternC = 4;
struct Cyc_Absyn_Tqual {
  int q_const:1;
  int q_volatile:1;
  int q_restrict:1;
};
static const int Cyc_Absyn_B1 = 0;
static const int Cyc_Absyn_B2 = 1;
static const int Cyc_Absyn_B4 = 2;
static const int Cyc_Absyn_B8 = 3;
static const int Cyc_Absyn_AnyKind = 0;
static const int Cyc_Absyn_MemKind = 1;
static const int Cyc_Absyn_BoxKind = 2;
static const int Cyc_Absyn_RgnKind = 3;
static const int Cyc_Absyn_EffKind = 4;
static const int Cyc_Absyn_Signed = 0;
static const int Cyc_Absyn_Unsigned = 1;
struct Cyc_Absyn_Conref {
  void *v;
};
static const int Cyc_Absyn_Eq_constr = 0;
struct Cyc_Absyn_Eq_constr_struct {
  int tag;
  void *f1;
};
static const int Cyc_Absyn_Forward_constr = 1;
struct Cyc_Absyn_Forward_constr_struct {
  int tag;
  struct Cyc_Absyn_Conref *f1;
};
static const int Cyc_Absyn_No_constr = 0;
struct Cyc_Absyn_Tvar {
  struct _tagged_string *name;
  struct Cyc_Absyn_Conref *kind;
};
static const int Cyc_Absyn_Unknown_b = 0;
static const int Cyc_Absyn_Upper_b = 0;
struct Cyc_Absyn_Upper_b_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
struct Cyc_Absyn_PtrInfo {
  void *elt_typ;
  void *rgn_typ;
  struct Cyc_Absyn_Conref *nullable;
  struct Cyc_Absyn_Tqual tq;
  struct Cyc_Absyn_Conref *bounds;
};
struct Cyc_Absyn_FnInfo {
  struct Cyc_List_List *tvars;
  struct Cyc_Core_Opt *effect;
  void *ret_typ;
  struct Cyc_List_List *args;
  int varargs;
  struct Cyc_List_List *attributes;
};
struct Cyc_Absyn_UnknownTunionInfo {
  struct _tuple1 *name;
  int is_xtunion;
};
static const int Cyc_Absyn_UnknownTunion = 0;
struct Cyc_Absyn_UnknownTunion_struct {
  int tag;
  struct Cyc_Absyn_UnknownTunionInfo f1;
};
static const int Cyc_Absyn_KnownTunion = 1;
struct Cyc_Absyn_KnownTunion_struct {
  int tag;
  struct Cyc_Absyn_Tuniondecl *f1;
};
struct Cyc_Absyn_TunionInfo {
  void *tunion_info;
  struct Cyc_List_List *targs;
  void *rgn;
};
struct Cyc_Absyn_UnknownTunionFieldInfo {
  struct _tuple1 *tunion_name;
  struct _tuple1 *field_name;
  int is_xtunion;
};
static const int Cyc_Absyn_UnknownTunionfield = 0;
struct Cyc_Absyn_UnknownTunionfield_struct {
  int tag;
  struct Cyc_Absyn_UnknownTunionFieldInfo f1;
};
static const int Cyc_Absyn_KnownTunionfield = 1;
struct Cyc_Absyn_KnownTunionfield_struct {
  int tag;
  struct Cyc_Absyn_Tuniondecl *f1;
  struct Cyc_Absyn_Tunionfield *f2;
};
struct Cyc_Absyn_TunionFieldInfo {
  void *field_info;
  struct Cyc_List_List *targs;
};
static const int Cyc_Absyn_VoidType = 0;
static const int Cyc_Absyn_Evar = 0;
struct Cyc_Absyn_Evar_struct {
  int tag;
  void *f1;
  struct Cyc_Core_Opt *f2;
  int f3;
};
static const int Cyc_Absyn_VarType = 1;
struct Cyc_Absyn_VarType_struct {
  int tag;
  struct Cyc_Absyn_Tvar *f1;
};
static const int Cyc_Absyn_TunionType = 2;
struct Cyc_Absyn_TunionType_struct {
  int tag;
  struct Cyc_Absyn_TunionInfo f1;
};
static const int Cyc_Absyn_TunionFieldType = 3;
struct Cyc_Absyn_TunionFieldType_struct {
  int tag;
  struct Cyc_Absyn_TunionFieldInfo f1;
};
static const int Cyc_Absyn_PointerType = 4;
struct Cyc_Absyn_PointerType_struct {
  int tag;
  struct Cyc_Absyn_PtrInfo f1;
};
static const int Cyc_Absyn_IntType = 5;
struct Cyc_Absyn_IntType_struct {
  int tag;
  void *f1;
  void *f2;
};
static const int Cyc_Absyn_FloatType = 1;
static const int Cyc_Absyn_DoubleType = 2;
static const int Cyc_Absyn_ArrayType = 6;
struct Cyc_Absyn_ArrayType_struct {
  int tag;
  void *f1;
  struct Cyc_Absyn_Tqual f2;
  struct Cyc_Absyn_Exp *f3;
};
static const int Cyc_Absyn_FnType = 7;
struct Cyc_Absyn_FnType_struct {
  int tag;
  struct Cyc_Absyn_FnInfo f1;
};
static const int Cyc_Absyn_TupleType = 8;
struct Cyc_Absyn_TupleType_struct {
  int tag;
  struct Cyc_List_List *f1;
};
static const int Cyc_Absyn_StructType = 9;
struct Cyc_Absyn_StructType_struct {
  int tag;
  struct _tuple1 *f1;
  struct Cyc_List_List *f2;
  struct Cyc_Absyn_Structdecl **f3;
};
static const int Cyc_Absyn_UnionType = 10;
struct Cyc_Absyn_UnionType_struct {
  int tag;
  struct _tuple1 *f1;
  struct Cyc_List_List *f2;
  struct Cyc_Absyn_Uniondecl **f3;
};
static const int Cyc_Absyn_AnonStructType = 11;
struct Cyc_Absyn_AnonStructType_struct {
  int tag;
  struct Cyc_List_List *f1;
};
static const int Cyc_Absyn_AnonUnionType = 12;
struct Cyc_Absyn_AnonUnionType_struct {
  int tag;
  struct Cyc_List_List *f1;
};
static const int Cyc_Absyn_EnumType = 13;
struct Cyc_Absyn_EnumType_struct {
  int tag;
  struct _tuple1 *f1;
  struct Cyc_Absyn_Enumdecl *f2;
};
static const int Cyc_Absyn_RgnHandleType = 14;
struct Cyc_Absyn_RgnHandleType_struct {
  int tag;
  void *f1;
};
static const int Cyc_Absyn_TypedefType = 15;
struct Cyc_Absyn_TypedefType_struct {
  int tag;
  struct _tuple1 *f1;
  struct Cyc_List_List *f2;
  struct Cyc_Core_Opt *f3;
};
static const int Cyc_Absyn_HeapRgn = 3;
static const int Cyc_Absyn_AccessEff = 16;
struct Cyc_Absyn_AccessEff_struct {
  int tag;
  void *f1;
};
static const int Cyc_Absyn_JoinEff = 17;
struct Cyc_Absyn_JoinEff_struct {
  int tag;
  struct Cyc_List_List *f1;
};
static const int Cyc_Absyn_NoTypes = 0;
struct Cyc_Absyn_NoTypes_struct {
  int tag;
  struct Cyc_List_List *f1;
  struct Cyc_Position_Segment *f2;
};
static const int Cyc_Absyn_WithTypes = 1;
struct Cyc_Absyn_WithTypes_struct {
  int tag;
  struct Cyc_List_List *f1;
  int f2;
  struct Cyc_Core_Opt *f3;
};
static const int Cyc_Absyn_NonNullable_ps = 0;
struct Cyc_Absyn_NonNullable_ps_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
static const int Cyc_Absyn_Nullable_ps = 1;
struct Cyc_Absyn_Nullable_ps_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
static const int Cyc_Absyn_TaggedArray_ps = 0;
static const int Cyc_Absyn_Regparm_att = 0;
struct Cyc_Absyn_Regparm_att_struct {
  int tag;
  int f1;
};
static const int Cyc_Absyn_Stdcall_att = 0;
static const int Cyc_Absyn_Cdecl_att = 1;
static const int Cyc_Absyn_Noreturn_att = 2;
static const int Cyc_Absyn_Const_att = 3;
static const int Cyc_Absyn_Aligned_att = 1;
struct Cyc_Absyn_Aligned_att_struct {
  int tag;
  int f1;
};
static const int Cyc_Absyn_Packed_att = 4;
static const int Cyc_Absyn_Section_att = 2;
struct Cyc_Absyn_Section_att_struct {
  int tag;
  struct _tagged_string f1;
};
static const int Cyc_Absyn_Nocommon_att = 5;
static const int Cyc_Absyn_Shared_att = 6;
static const int Cyc_Absyn_Unused_att = 7;
static const int Cyc_Absyn_Weak_att = 8;
static const int Cyc_Absyn_Dllimport_att = 9;
static const int Cyc_Absyn_Dllexport_att = 10;
static const int Cyc_Absyn_No_instrument_function_att = 11;
static const int Cyc_Absyn_Constructor_att = 12;
static const int Cyc_Absyn_Destructor_att = 13;
static const int Cyc_Absyn_No_check_memory_usage_att = 14;
static const int Cyc_Absyn_Carray_mod = 0;
static const int Cyc_Absyn_ConstArray_mod = 0;
struct Cyc_Absyn_ConstArray_mod_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
static const int Cyc_Absyn_Pointer_mod = 1;
struct Cyc_Absyn_Pointer_mod_struct {
  int tag;
  void *f1;
  void *f2;
  struct Cyc_Absyn_Tqual f3;
};
static const int Cyc_Absyn_Function_mod = 2;
struct Cyc_Absyn_Function_mod_struct {
  int tag;
  void *f1;
};
static const int Cyc_Absyn_TypeParams_mod = 3;
struct Cyc_Absyn_TypeParams_mod_struct {
  int tag;
  struct Cyc_List_List *f1;
  struct Cyc_Position_Segment *f2;
  int f3;
};
static const int Cyc_Absyn_Attributes_mod = 4;
struct Cyc_Absyn_Attributes_mod_struct {
  int tag;
  struct Cyc_Position_Segment *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Absyn_Char_c = 0;
struct Cyc_Absyn_Char_c_struct {
  int tag;
  void *f1;
  char f2;
};
static const int Cyc_Absyn_Short_c = 1;
struct Cyc_Absyn_Short_c_struct {
  int tag;
  void *f1;
  short f2;
};
static const int Cyc_Absyn_Int_c = 2;
struct Cyc_Absyn_Int_c_struct {
  int tag;
  void *f1;
  int f2;
};
static const int Cyc_Absyn_LongLong_c = 3;
struct Cyc_Absyn_LongLong_c_struct {
  int tag;
  void *f1;
  long long f2;
};
static const int Cyc_Absyn_Float_c = 4;
struct Cyc_Absyn_Float_c_struct {
  int tag;
  struct _tagged_string f1;
};
static const int Cyc_Absyn_String_c = 5;
struct Cyc_Absyn_String_c_struct {
  int tag;
  struct _tagged_string f1;
};
static const int Cyc_Absyn_Null_c = 0;
static const int Cyc_Absyn_Plus = 0;
static const int Cyc_Absyn_Times = 1;
static const int Cyc_Absyn_Minus = 2;
static const int Cyc_Absyn_Div = 3;
static const int Cyc_Absyn_Mod = 4;
static const int Cyc_Absyn_Eq = 5;
static const int Cyc_Absyn_Neq = 6;
static const int Cyc_Absyn_Gt = 7;
static const int Cyc_Absyn_Lt = 8;
static const int Cyc_Absyn_Gte = 9;
static const int Cyc_Absyn_Lte = 10;
static const int Cyc_Absyn_Not = 11;
static const int Cyc_Absyn_Bitnot = 12;
static const int Cyc_Absyn_Bitand = 13;
static const int Cyc_Absyn_Bitor = 14;
static const int Cyc_Absyn_Bitxor = 15;
static const int Cyc_Absyn_Bitlshift = 16;
static const int Cyc_Absyn_Bitlrshift = 17;
static const int Cyc_Absyn_Bitarshift = 18;
static const int Cyc_Absyn_Size = 19;
static const int Cyc_Absyn_Printf = 20;
static const int Cyc_Absyn_Fprintf = 21;
static const int Cyc_Absyn_Xprintf = 22;
static const int Cyc_Absyn_Scanf = 23;
static const int Cyc_Absyn_Fscanf = 24;
static const int Cyc_Absyn_Sscanf = 25;
static const int Cyc_Absyn_PreInc = 0;
static const int Cyc_Absyn_PostInc = 1;
static const int Cyc_Absyn_PreDec = 2;
static const int Cyc_Absyn_PostDec = 3;
static const int Cyc_Absyn_Const_e = 0;
struct Cyc_Absyn_Const_e_struct {
  int tag;
  void *f1;
};
static const int Cyc_Absyn_Var_e = 1;
struct Cyc_Absyn_Var_e_struct {
  int tag;
  struct _tuple1 *f1;
  void *f2;
};
static const int Cyc_Absyn_UnknownId_e = 2;
struct Cyc_Absyn_UnknownId_e_struct {
  int tag;
  struct _tuple1 *f1;
};
static const int Cyc_Absyn_Primop_e = 3;
struct Cyc_Absyn_Primop_e_struct {
  int tag;
  void *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Absyn_AssignOp_e = 4;
struct Cyc_Absyn_AssignOp_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct Cyc_Core_Opt *f2;
  struct Cyc_Absyn_Exp *f3;
};
static const int Cyc_Absyn_Increment_e = 5;
struct Cyc_Absyn_Increment_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  void *f2;
};
static const int Cyc_Absyn_Conditional_e = 6;
struct Cyc_Absyn_Conditional_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct Cyc_Absyn_Exp *f2;
  struct Cyc_Absyn_Exp *f3;
};
static const int Cyc_Absyn_SeqExp_e = 7;
struct Cyc_Absyn_SeqExp_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct Cyc_Absyn_Exp *f2;
};
static const int Cyc_Absyn_UnknownCall_e = 8;
struct Cyc_Absyn_UnknownCall_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Absyn_FnCall_e = 9;
struct Cyc_Absyn_FnCall_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Absyn_Throw_e = 10;
struct Cyc_Absyn_Throw_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
static const int Cyc_Absyn_NoInstantiate_e = 11;
struct Cyc_Absyn_NoInstantiate_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
static const int Cyc_Absyn_Instantiate_e = 12;
struct Cyc_Absyn_Instantiate_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Absyn_Cast_e = 13;
struct Cyc_Absyn_Cast_e_struct {
  int tag;
  void *f1;
  struct Cyc_Absyn_Exp *f2;
};
static const int Cyc_Absyn_Address_e = 14;
struct Cyc_Absyn_Address_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
static const int Cyc_Absyn_New_e = 15;
struct Cyc_Absyn_New_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct Cyc_Absyn_Exp *f2;
};
static const int Cyc_Absyn_Sizeoftyp_e = 16;
struct Cyc_Absyn_Sizeoftyp_e_struct {
  int tag;
  void *f1;
};
static const int Cyc_Absyn_Sizeofexp_e = 17;
struct Cyc_Absyn_Sizeofexp_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
static const int Cyc_Absyn_Deref_e = 18;
struct Cyc_Absyn_Deref_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
static const int Cyc_Absyn_StructMember_e = 19;
struct Cyc_Absyn_StructMember_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct _tagged_string *f2;
};
static const int Cyc_Absyn_StructArrow_e = 20;
struct Cyc_Absyn_StructArrow_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct _tagged_string *f2;
};
static const int Cyc_Absyn_Subscript_e = 21;
struct Cyc_Absyn_Subscript_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct Cyc_Absyn_Exp *f2;
};
static const int Cyc_Absyn_Tuple_e = 22;
struct Cyc_Absyn_Tuple_e_struct {
  int tag;
  struct Cyc_List_List *f1;
};
static const int Cyc_Absyn_CompoundLit_e = 23;
struct _tuple2 {
  struct Cyc_Core_Opt *f1;
  struct Cyc_Absyn_Tqual f2;
  void *f3;
};
struct Cyc_Absyn_CompoundLit_e_struct {
  int tag;
  struct _tuple2 *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Absyn_Array_e = 24;
struct Cyc_Absyn_Array_e_struct {
  int tag;
  struct Cyc_List_List *f1;
};
static const int Cyc_Absyn_Comprehension_e = 25;
struct Cyc_Absyn_Comprehension_e_struct {
  int tag;
  struct Cyc_Absyn_Vardecl *f1;
  struct Cyc_Absyn_Exp *f2;
  struct Cyc_Absyn_Exp *f3;
};
static const int Cyc_Absyn_Struct_e = 26;
struct Cyc_Absyn_Struct_e_struct {
  int tag;
  struct _tuple1 *f1;
  struct Cyc_Core_Opt *f2;
  struct Cyc_List_List *f3;
  struct Cyc_Absyn_Structdecl *f4;
};
static const int Cyc_Absyn_AnonStruct_e = 27;
struct Cyc_Absyn_AnonStruct_e_struct {
  int tag;
  void *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Absyn_Tunion_e = 28;
struct Cyc_Absyn_Tunion_e_struct {
  int tag;
  struct Cyc_Core_Opt *f1;
  struct Cyc_Core_Opt *f2;
  struct Cyc_List_List *f3;
  struct Cyc_Absyn_Tuniondecl *f4;
  struct Cyc_Absyn_Tunionfield *f5;
};
static const int Cyc_Absyn_Enum_e = 29;
struct Cyc_Absyn_Enum_e_struct {
  int tag;
  struct _tuple1 *f1;
  struct Cyc_Absyn_Enumdecl *f2;
  struct Cyc_Absyn_Enumfield *f3;
};
static const int Cyc_Absyn_Malloc_e = 30;
struct Cyc_Absyn_Malloc_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  void *f2;
};
static const int Cyc_Absyn_UnresolvedMem_e = 31;
struct Cyc_Absyn_UnresolvedMem_e_struct {
  int tag;
  struct Cyc_Core_Opt *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Absyn_StmtExp_e = 32;
struct Cyc_Absyn_StmtExp_e_struct {
  int tag;
  struct Cyc_Absyn_Stmt *f1;
};
static const int Cyc_Absyn_Codegen_e = 33;
struct Cyc_Absyn_Codegen_e_struct {
  int tag;
  struct Cyc_Absyn_Fndecl *f1;
};
static const int Cyc_Absyn_Fill_e = 34;
struct Cyc_Absyn_Fill_e_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
struct Cyc_Absyn_Exp {
  struct Cyc_Core_Opt *topt;
  void *r;
  struct Cyc_Position_Segment *loc;
};
static const int Cyc_Absyn_Skip_s = 0;
static const int Cyc_Absyn_Exp_s = 0;
struct Cyc_Absyn_Exp_s_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
static const int Cyc_Absyn_Seq_s = 1;
struct Cyc_Absyn_Seq_s_struct {
  int tag;
  struct Cyc_Absyn_Stmt *f1;
  struct Cyc_Absyn_Stmt *f2;
};
static const int Cyc_Absyn_Return_s = 2;
struct Cyc_Absyn_Return_s_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
static const int Cyc_Absyn_IfThenElse_s = 3;
struct Cyc_Absyn_IfThenElse_s_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct Cyc_Absyn_Stmt *f2;
  struct Cyc_Absyn_Stmt *f3;
};
static const int Cyc_Absyn_While_s = 4;
struct _tuple3 {
  struct Cyc_Absyn_Exp *f1;
  struct Cyc_Absyn_Stmt *f2;
};
struct Cyc_Absyn_While_s_struct {
  int tag;
  struct _tuple3 f1;
  struct Cyc_Absyn_Stmt *f2;
};
static const int Cyc_Absyn_Break_s = 5;
struct Cyc_Absyn_Break_s_struct {
  int tag;
  struct Cyc_Absyn_Stmt *f1;
};
static const int Cyc_Absyn_Continue_s = 6;
struct Cyc_Absyn_Continue_s_struct {
  int tag;
  struct Cyc_Absyn_Stmt *f1;
};
static const int Cyc_Absyn_Goto_s = 7;
struct Cyc_Absyn_Goto_s_struct {
  int tag;
  struct _tagged_string *f1;
  struct Cyc_Absyn_Stmt *f2;
};
static const int Cyc_Absyn_For_s = 8;
struct Cyc_Absyn_For_s_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct _tuple3 f2;
  struct _tuple3 f3;
  struct Cyc_Absyn_Stmt *f4;
};
static const int Cyc_Absyn_Switch_s = 9;
struct Cyc_Absyn_Switch_s_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Absyn_Fallthru_s = 10;
struct Cyc_Absyn_Fallthru_s_struct {
  int tag;
  struct Cyc_List_List *f1;
  struct Cyc_Absyn_Switch_clause **f2;
};
static const int Cyc_Absyn_Decl_s = 11;
struct Cyc_Absyn_Decl_s_struct {
  int tag;
  struct Cyc_Absyn_Decl *f1;
  struct Cyc_Absyn_Stmt *f2;
};
static const int Cyc_Absyn_Cut_s = 12;
struct Cyc_Absyn_Cut_s_struct {
  int tag;
  struct Cyc_Absyn_Stmt *f1;
};
static const int Cyc_Absyn_Splice_s = 13;
struct Cyc_Absyn_Splice_s_struct {
  int tag;
  struct Cyc_Absyn_Stmt *f1;
};
static const int Cyc_Absyn_Label_s = 14;
struct Cyc_Absyn_Label_s_struct {
  int tag;
  struct _tagged_string *f1;
  struct Cyc_Absyn_Stmt *f2;
};
static const int Cyc_Absyn_Do_s = 15;
struct Cyc_Absyn_Do_s_struct {
  int tag;
  struct Cyc_Absyn_Stmt *f1;
  struct _tuple3 f2;
};
static const int Cyc_Absyn_TryCatch_s = 16;
struct Cyc_Absyn_TryCatch_s_struct {
  int tag;
  struct Cyc_Absyn_Stmt *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Absyn_Region_s = 17;
struct Cyc_Absyn_Region_s_struct {
  int tag;
  struct Cyc_Absyn_Tvar *f1;
  struct Cyc_Absyn_Vardecl *f2;
  struct Cyc_Absyn_Stmt *f3;
};
struct Cyc_Absyn_Stmt {
  void *r;
  struct Cyc_Position_Segment *loc;
  struct Cyc_List_List *non_local_preds;
  int try_depth;
  void *annot;
};
static const int Cyc_Absyn_Wild_p = 0;
static const int Cyc_Absyn_Var_p = 0;
struct Cyc_Absyn_Var_p_struct {
  int tag;
  struct Cyc_Absyn_Vardecl *f1;
};
static const int Cyc_Absyn_Null_p = 1;
static const int Cyc_Absyn_Int_p = 1;
struct Cyc_Absyn_Int_p_struct {
  int tag;
  void *f1;
  int f2;
};
static const int Cyc_Absyn_Char_p = 2;
struct Cyc_Absyn_Char_p_struct {
  int tag;
  char f1;
};
static const int Cyc_Absyn_Float_p = 3;
struct Cyc_Absyn_Float_p_struct {
  int tag;
  struct _tagged_string f1;
};
static const int Cyc_Absyn_Tuple_p = 4;
struct Cyc_Absyn_Tuple_p_struct {
  int tag;
  struct Cyc_List_List *f1;
};
static const int Cyc_Absyn_Pointer_p = 5;
struct Cyc_Absyn_Pointer_p_struct {
  int tag;
  struct Cyc_Absyn_Pat *f1;
};
static const int Cyc_Absyn_Reference_p = 6;
struct Cyc_Absyn_Reference_p_struct {
  int tag;
  struct Cyc_Absyn_Vardecl *f1;
};
static const int Cyc_Absyn_Struct_p = 7;
struct Cyc_Absyn_Struct_p_struct {
  int tag;
  struct Cyc_Absyn_Structdecl *f1;
  struct Cyc_Core_Opt *f2;
  struct Cyc_List_List *f3;
  struct Cyc_List_List *f4;
};
static const int Cyc_Absyn_Tunion_p = 8;
struct Cyc_Absyn_Tunion_p_struct {
  int tag;
  struct Cyc_Absyn_Tuniondecl *f1;
  struct Cyc_Absyn_Tunionfield *f2;
  struct Cyc_List_List *f3;
  struct Cyc_List_List *f4;
};
static const int Cyc_Absyn_Enum_p = 9;
struct Cyc_Absyn_Enum_p_struct {
  int tag;
  struct Cyc_Absyn_Enumdecl *f1;
  struct Cyc_Absyn_Enumfield *f2;
};
static const int Cyc_Absyn_UnknownId_p = 10;
struct Cyc_Absyn_UnknownId_p_struct {
  int tag;
  struct _tuple1 *f1;
};
static const int Cyc_Absyn_UnknownCall_p = 11;
struct Cyc_Absyn_UnknownCall_p_struct {
  int tag;
  struct _tuple1 *f1;
  struct Cyc_List_List *f2;
  struct Cyc_List_List *f3;
};
static const int Cyc_Absyn_UnknownFields_p = 12;
struct Cyc_Absyn_UnknownFields_p_struct {
  int tag;
  struct _tuple1 *f1;
  struct Cyc_List_List *f2;
  struct Cyc_List_List *f3;
};
struct Cyc_Absyn_Pat {
  void *r;
  struct Cyc_Core_Opt *topt;
  struct Cyc_Position_Segment *loc;
};
struct Cyc_Absyn_Switch_clause {
  struct Cyc_Absyn_Pat *pattern;
  struct Cyc_Core_Opt *pat_vars;
  struct Cyc_Absyn_Exp *where_clause;
  struct Cyc_Absyn_Stmt *body;
  struct Cyc_Position_Segment *loc;
};
static const int Cyc_Absyn_Unresolved_b = 0;
static const int Cyc_Absyn_Global_b = 0;
struct Cyc_Absyn_Global_b_struct {
  int tag;
  struct Cyc_Absyn_Vardecl *f1;
};
static const int Cyc_Absyn_Funname_b = 1;
struct Cyc_Absyn_Funname_b_struct {
  int tag;
  struct Cyc_Absyn_Fndecl *f1;
};
static const int Cyc_Absyn_Param_b = 2;
struct Cyc_Absyn_Param_b_struct {
  int tag;
  struct Cyc_Absyn_Vardecl *f1;
};
static const int Cyc_Absyn_Local_b = 3;
struct Cyc_Absyn_Local_b_struct {
  int tag;
  struct Cyc_Absyn_Vardecl *f1;
};
static const int Cyc_Absyn_Pat_b = 4;
struct Cyc_Absyn_Pat_b_struct {
  int tag;
  struct Cyc_Absyn_Vardecl *f1;
};
struct Cyc_Absyn_Vardecl {
  void *sc;
  struct _tuple1 *name;
  struct Cyc_Absyn_Tqual tq;
  void *type;
  struct Cyc_Absyn_Exp *initializer;
  struct Cyc_Core_Opt *rgn;
  struct Cyc_List_List *attributes;
};
struct Cyc_Absyn_Fndecl {
  void *sc;
  int is_inline;
  struct _tuple1 *name;
  struct Cyc_List_List *tvs;
  struct Cyc_Core_Opt *effect;
  void *ret_type;
  struct Cyc_List_List *args;
  int varargs;
  struct Cyc_Absyn_Stmt *body;
  struct Cyc_Core_Opt *cached_typ;
  struct Cyc_Core_Opt *param_vardecls;
  struct Cyc_List_List *attributes;
};
struct Cyc_Absyn_Structfield {
  struct _tagged_string *name;
  struct Cyc_Absyn_Tqual tq;
  void *type;
  struct Cyc_Core_Opt *width;
  struct Cyc_List_List *attributes;
};
struct Cyc_Absyn_Structdecl {
  void *sc;
  struct Cyc_Core_Opt *name;
  struct Cyc_List_List *tvs;
  struct Cyc_Core_Opt *fields;
  struct Cyc_List_List *attributes;
};
struct Cyc_Absyn_Uniondecl {
  void *sc;
  struct Cyc_Core_Opt *name;
  struct Cyc_List_List *tvs;
  struct Cyc_Core_Opt *fields;
  struct Cyc_List_List *attributes;
};
struct Cyc_Absyn_Tunionfield {
  struct _tuple1 *name;
  struct Cyc_List_List *tvs;
  struct Cyc_List_List *typs;
  struct Cyc_Position_Segment *loc;
};
struct Cyc_Absyn_Tuniondecl {
  void *sc;
  struct _tuple1 *name;
  struct Cyc_List_List *tvs;
  struct Cyc_Core_Opt *fields;
  int is_xtunion;
};
struct Cyc_Absyn_Enumfield {
  struct _tuple1 *name;
  struct Cyc_Absyn_Exp *tag;
  struct Cyc_Position_Segment *loc;
};
struct Cyc_Absyn_Enumdecl {
  void *sc;
  struct _tuple1 *name;
  struct Cyc_List_List *fields;
};
struct Cyc_Absyn_Typedefdecl {
  struct _tuple1 *name;
  struct Cyc_List_List *tvs;
  void *defn;
};
static const int Cyc_Absyn_Var_d = 0;
struct Cyc_Absyn_Var_d_struct {
  int tag;
  struct Cyc_Absyn_Vardecl *f1;
};
static const int Cyc_Absyn_Fn_d = 1;
struct Cyc_Absyn_Fn_d_struct {
  int tag;
  struct Cyc_Absyn_Fndecl *f1;
};
static const int Cyc_Absyn_Let_d = 2;
struct Cyc_Absyn_Let_d_struct {
  int tag;
  struct Cyc_Absyn_Pat *f1;
  struct Cyc_Core_Opt *f2;
  struct Cyc_Core_Opt *f3;
  struct Cyc_Absyn_Exp *f4;
  int f5;
};
static const int Cyc_Absyn_Struct_d = 3;
struct Cyc_Absyn_Struct_d_struct {
  int tag;
  struct Cyc_Absyn_Structdecl *f1;
};
static const int Cyc_Absyn_Union_d = 4;
struct Cyc_Absyn_Union_d_struct {
  int tag;
  struct Cyc_Absyn_Uniondecl *f1;
};
static const int Cyc_Absyn_Tunion_d = 5;
struct Cyc_Absyn_Tunion_d_struct {
  int tag;
  struct Cyc_Absyn_Tuniondecl *f1;
};
static const int Cyc_Absyn_Enum_d = 6;
struct Cyc_Absyn_Enum_d_struct {
  int tag;
  struct Cyc_Absyn_Enumdecl *f1;
};
static const int Cyc_Absyn_Typedef_d = 7;
struct Cyc_Absyn_Typedef_d_struct {
  int tag;
  struct Cyc_Absyn_Typedefdecl *f1;
};
static const int Cyc_Absyn_Namespace_d = 8;
struct Cyc_Absyn_Namespace_d_struct {
  int tag;
  struct _tagged_string *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Absyn_Using_d = 9;
struct Cyc_Absyn_Using_d_struct {
  int tag;
  struct _tuple1 *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Absyn_ExternC_d = 10;
struct Cyc_Absyn_ExternC_d_struct {
  int tag;
  struct Cyc_List_List *f1;
};
struct Cyc_Absyn_Decl {
  void *r;
  struct Cyc_Position_Segment *loc;
};
static const int Cyc_Absyn_ArrayElement = 0;
struct Cyc_Absyn_ArrayElement_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
static const int Cyc_Absyn_FieldName = 1;
struct Cyc_Absyn_FieldName_struct {
  int tag;
  struct _tagged_string *f1;
};
extern char Cyc_Absyn_EmptyAnnot[15];
extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref *Cyc_Absyn_new_conref(void *x);
extern struct Cyc_Absyn_Conref *Cyc_Absyn_empty_conref();
extern void *Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref *x);
extern void *Cyc_Absyn_new_evar(void *);
extern void *Cyc_Absyn_wildtyp();
extern void *Cyc_Absyn_uchar_t;
extern void *Cyc_Absyn_ushort_t;
extern void *Cyc_Absyn_uint_t;
extern void *Cyc_Absyn_ulong_t;
extern void *Cyc_Absyn_schar_t;
extern void *Cyc_Absyn_sshort_t;
extern void *Cyc_Absyn_sint_t;
extern void *Cyc_Absyn_slong_t;
extern void *Cyc_Absyn_float_t;
extern void *Cyc_Absyn_exn_typ;
extern void *Cyc_Absyn_string_typ(void *rgn);
extern void *Cyc_Absyn_file_typ();
extern void *Cyc_Absyn_atb_typ(void *t,void *rgn,struct Cyc_Absyn_Tqual tq,
			       void *b);
extern void *Cyc_Absyn_star_typ(void *t,void *rgn,struct Cyc_Absyn_Tqual tq);
extern void *Cyc_Absyn_at_typ(void *t,void *rgn,struct Cyc_Absyn_Tqual tq);
extern struct Cyc_Absyn_Exp *Cyc_Absyn_new_exp(void *,struct Cyc_Position_Segment *);
extern struct Cyc_Absyn_Exp *Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp *);
extern struct Cyc_Absyn_Exp *Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment *);
extern void *Cyc_Absyn_pointer_expand(void *);
extern int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp *);
extern struct Cyc_Absyn_Structfield *Cyc_Absyn_lookup_field(struct Cyc_List_List *,
							    struct _tagged_string *);
extern struct Cyc_Absyn_Structfield *Cyc_Absyn_lookup_struct_field(struct Cyc_Absyn_Structdecl *,
								   struct _tagged_string *);
extern struct Cyc_Absyn_Structfield *Cyc_Absyn_lookup_union_field(struct Cyc_Absyn_Uniondecl *,
								  struct _tagged_string *);
struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError[18];
struct Cyc_Stdio_FileOpenError_struct {
  char *tag;
  struct _tagged_string f1;
};
extern char Cyc_Stdio_FileCloseError[19];
extern int Cyc_String_zstrcmp(struct _tagged_string,struct _tagged_string);
struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate *Cyc_PP_ppstate_t;
struct Cyc_PP_Out;
typedef struct Cyc_PP_Out *Cyc_PP_out_t;
struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc *Cyc_PP_doc_t;
extern struct _tagged_string Cyc_Absynpp_typ2string(void *);
extern struct _tagged_string Cyc_Absynpp_qvar2string(struct _tuple1 *);
struct Cyc_Set_Set;
typedef struct Cyc_Set_Set *Cyc_Set_gset_t;
typedef struct Cyc_Set_Set *Cyc_Set_hset_t;
typedef struct Cyc_Set_Set *Cyc_Set_set_t;
extern char Cyc_Set_Absent[11];
struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict *Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict *Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];
static const int Cyc_Tcenv_VarRes = 0;
struct Cyc_Tcenv_VarRes_struct {
  int tag;
  void *f1;
};
static const int Cyc_Tcenv_StructRes = 1;
struct Cyc_Tcenv_StructRes_struct {
  int tag;
  struct Cyc_Absyn_Structdecl *f1;
};
static const int Cyc_Tcenv_TunionRes = 2;
struct Cyc_Tcenv_TunionRes_struct {
  int tag;
  struct Cyc_Absyn_Tuniondecl *f1;
  struct Cyc_Absyn_Tunionfield *f2;
};
static const int Cyc_Tcenv_EnumRes = 3;
struct Cyc_Tcenv_EnumRes_struct {
  int tag;
  struct Cyc_Absyn_Enumdecl *f1;
  struct Cyc_Absyn_Enumfield *f2;
};
typedef void *Cyc_Tcenv_resolved_t;
struct Cyc_Tcenv_Genv {
  struct Cyc_Set_Set *namespaces;
  struct Cyc_Dict_Dict *structdecls;
  struct Cyc_Dict_Dict *uniondecls;
  struct Cyc_Dict_Dict *tuniondecls;
  struct Cyc_Dict_Dict *enumdecls;
  struct Cyc_Dict_Dict *typedefs;
  struct Cyc_Dict_Dict *ordinaries;
  struct Cyc_List_List *availables;
};
typedef struct Cyc_Tcenv_Genv *Cyc_Tcenv_genv_t;
struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv *Cyc_Tcenv_fenv_t;
static const int Cyc_Tcenv_NotLoop_j = 0;
static const int Cyc_Tcenv_CaseEnd_j = 1;
static const int Cyc_Tcenv_FnEnd_j = 2;
static const int Cyc_Tcenv_Stmt_j = 0;
struct Cyc_Tcenv_Stmt_j_struct {
  int tag;
  struct Cyc_Absyn_Stmt *f1;
};
typedef void *Cyc_Tcenv_jumpee_t;
static const int Cyc_Tcenv_Outermost = 0;
struct Cyc_Tcenv_Outermost_struct {
  int tag;
  void *f1;
};
static const int Cyc_Tcenv_Frame = 1;
struct Cyc_Tcenv_Frame_struct {
  int tag;
  void *f1;
  void *f2;
};
static const int Cyc_Tcenv_Hidden = 2;
struct Cyc_Tcenv_Hidden_struct {
  int tag;
  void *f1;
  void *f2;
};
typedef void *Cyc_Tcenv_frames_t;
struct Cyc_Tcenv_Tenv {
  struct Cyc_List_List *ns;
  struct Cyc_Dict_Dict *ae;
  struct Cyc_Core_Opt *le;
};
typedef struct Cyc_Tcenv_Tenv *Cyc_Tcenv_tenv_t;
extern void *Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv *,struct Cyc_Position_Segment *,
				       struct _tuple1 *);
extern struct Cyc_Absyn_Structdecl **Cyc_Tcenv_lookup_structdecl(struct Cyc_Tcenv_Tenv *,
								 struct Cyc_Position_Segment *,
								 struct _tuple1 *);
extern struct Cyc_Tcenv_Tenv *Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment *,
						      struct Cyc_Tcenv_Tenv *,
						      struct Cyc_Absyn_Vardecl *);
extern struct Cyc_List_List *Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv *);
extern struct Cyc_Absyn_Stmt *Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv *);
extern struct Cyc_Tcenv_Tenv *Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv *,
						     struct Cyc_Absyn_Stmt *);
extern struct Cyc_Tcenv_Tenv *Cyc_Tcenv_new_block(struct Cyc_Position_Segment *,
						  struct Cyc_Tcenv_Tenv *);
extern void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv *,struct Cyc_Position_Segment *,
					   void *rgn);
extern void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
					      void *eff);
extern char Cyc_Tcutil_TypeErr[12];
extern void *Cyc_Tcutil_impos(struct _tagged_string);
extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment *,struct _tagged_string);
extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment *,struct _tagged_string);
extern void *Cyc_Tcutil_compress(void *t);
extern void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv *,struct Cyc_Absyn_Exp *,
				      void *);
extern int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv *,struct Cyc_Absyn_Exp *,
				 void *);
extern int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv *,struct Cyc_Absyn_Exp *,
				    void *);
extern int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv *,struct Cyc_Absyn_Exp *);
extern int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv *,void *,struct Cyc_List_List *);
extern int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv *,struct Cyc_Absyn_Exp *);
extern int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv *,struct Cyc_Absyn_Exp *);
extern int Cyc_Tcutil_coerce_use(struct Cyc_Tcenv_Tenv *,struct Cyc_Absyn_Exp *,
				 void *);
extern int Cyc_Tcutil_coerceable(void *);
extern int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv *,struct Cyc_Position_Segment *,
				      void *,void *);
extern int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv *,struct Cyc_Position_Segment *,
			       void *,void *);
extern int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp *);
extern int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp *);
extern void *Cyc_Tcutil_max_arithmetic_type(void *,void *);
extern int Cyc_Tcutil_unify(void *,void *);
extern void *Cyc_Tcutil_substitute(struct Cyc_List_List *,void *);
extern void *Cyc_Tcutil_rsubstitute(struct _RegionHandle *,struct Cyc_List_List *,
				    void *);
extern void *Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl *);
struct _tuple4 {
  struct Cyc_Absyn_Tvar *f1;
  void *f2;
};
extern struct _tuple4 *Cyc_Tcutil_r_make_inst_var(struct _RegionHandle *,
						  struct Cyc_Absyn_Tvar *);
extern void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp *);
extern void Cyc_Tcutil_check_type(struct Cyc_Position_Segment *,struct Cyc_Tcenv_Tenv *,
				  struct Cyc_List_List *bound_tvars,void *k,
				  void *);
extern void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment *,
					   struct Cyc_Absyn_Conref *);
extern void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment *,unsigned int i,
				   struct Cyc_Absyn_Conref *);
extern struct Cyc_List_List *Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle *rgn,
								   struct Cyc_Position_Segment *loc,
								   struct Cyc_List_List *des,
								   struct Cyc_List_List *fields);
extern int Cyc_Tcutil_is_tagged_pointer_typ(void *);
struct _tuple5 {
  int f1;
  void *f2;
};
extern struct _tuple5 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv *te,
						 struct Cyc_Absyn_Exp *e);
extern int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Exp *e);
extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp *e);
extern unsigned int Cyc_Evexp_szof(void *t,struct Cyc_Position_Segment *loc);
extern void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Stmt *s,
			      int new_block);
extern struct Cyc_List_List *Cyc_Formatstr_get_format_typs(struct _tagged_string,
							   struct Cyc_Position_Segment *);
extern struct Cyc_List_List *Cyc_Formatstr_get_scanf_typs(struct _tagged_string,
							  struct Cyc_Position_Segment *);
extern void *Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv *,void **,struct Cyc_Absyn_Exp *);
extern void *Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv *,void **,struct Cyc_Absyn_Exp *);
extern void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Exp *e,
			     struct _tagged_string msg_part);
struct Cyc_CfFlowInfo_Place;
typedef void *Cyc_CfFlowInfo_local_root_t;
typedef void *Cyc_CfFlowInfo_field_t;
typedef struct Cyc_CfFlowInfo_Place *Cyc_CfFlowInfo_place_t;
typedef void *Cyc_CfFlowInfo_escaped_t;
typedef void *Cyc_CfFlowInfo_init_level_t;
typedef void *Cyc_CfFlowInfo_init_state_t;
typedef void *Cyc_CfFlowInfo_path_info_t;
typedef struct Cyc_Dict_Dict *Cyc_CfFlowInfo_pinfo_dict_t;
typedef void *Cyc_CfFlowInfo_flow_info_t;
static const int Cyc_CfFlowInfo_VarRoot = 0;
struct Cyc_CfFlowInfo_VarRoot_struct {
  int tag;
  struct Cyc_Absyn_Vardecl *f1;
};
static const int Cyc_CfFlowInfo_MallocPt = 1;
struct Cyc_CfFlowInfo_MallocPt_struct {
  int tag;
  struct Cyc_Absyn_Exp *f1;
};
static const int Cyc_CfFlowInfo_StructF = 0;
struct Cyc_CfFlowInfo_StructF_struct {
  int tag;
  struct _tagged_string *f1;
};
static const int Cyc_CfFlowInfo_TupleF = 1;
struct Cyc_CfFlowInfo_TupleF_struct {
  int tag;
  int f1;
};
struct Cyc_CfFlowInfo_Place {
  void *root;
  struct Cyc_List_List *fields;
};
static const int Cyc_CfFlowInfo_Esc = 0;
static const int Cyc_CfFlowInfo_Unesc = 1;
static const int Cyc_CfFlowInfo_NoneIL = 0;
static const int Cyc_CfFlowInfo_ThisIL = 1;
static const int Cyc_CfFlowInfo_AllIL = 2;
static const int Cyc_CfFlowInfo_UnknownIS = 0;
struct Cyc_CfFlowInfo_UnknownIS_struct {
  int tag;
  void *f1;
  void *f2;
};
static const int Cyc_CfFlowInfo_MustPointTo = 1;
struct Cyc_CfFlowInfo_MustPointTo_struct {
  int tag;
  struct Cyc_CfFlowInfo_Place *f1;
};
static const int Cyc_CfFlowInfo_LeafPI = 0;
struct Cyc_CfFlowInfo_LeafPI_struct {
  int tag;
  void *f1;
};
static const int Cyc_CfFlowInfo_TuplePI = 1;
struct Cyc_CfFlowInfo_TuplePI_struct {
  int tag;
  struct Cyc_Dict_Dict *f1;
};
static const int Cyc_CfFlowInfo_StructPI = 2;
struct Cyc_CfFlowInfo_StructPI_struct {
  int tag;
  struct Cyc_Dict_Dict *f1;
};
static const int Cyc_CfFlowInfo_BottomFL = 0;
static const int Cyc_CfFlowInfo_InitsFL = 0;
struct Cyc_CfFlowInfo_InitsFL_struct {
  int tag;
  struct Cyc_Dict_Dict *f1;
};
extern void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt *enclosee,
					    struct Cyc_Absyn_Stmt *encloser);
struct Cyc_NewControlFlow_AnalEnv {
  struct Cyc_Dict_Dict *roots;
  int in_try;
  void *tryflow;
};
typedef struct Cyc_NewControlFlow_AnalEnv *Cyc_NewControlFlow_analenv_t;
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List *ds);
static void *Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				struct _tagged_string msg) {
  Cyc_Tcutil_terr(loc,msg);
  return Cyc_Absyn_wildtyp();
}
static void Cyc_Tcexp_resolve_unknown_id(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Exp *e,
					 struct _tuple1 *q) {
  struct _handler_cons _temp0;
  _push_handler(&_temp0);
  { void *_temp1 = (void *)setjmp(_temp0.handler);
    if (! _temp1) {
      { void *_temp2 = Cyc_Tcenv_lookup_ordinary(te,e->loc,q);
	void *_temp12;
	struct Cyc_Absyn_Enumfield *_temp14;
	struct Cyc_Absyn_Enumdecl *_temp16;
	struct Cyc_Absyn_Tunionfield *_temp18;
	struct Cyc_Absyn_Tuniondecl *_temp20;
	struct Cyc_Absyn_Structdecl *_temp22;
	_LL4: if (*((int *)_temp2) == Cyc_Tcenv_VarRes) {
	  _LL13: _temp12 = (void *)((struct Cyc_Tcenv_VarRes_struct *)_temp2)->f1;
		 goto _LL5;
	}
	else {
	  goto _LL6;
	}
	_LL6: if (*((int *)_temp2) == Cyc_Tcenv_EnumRes) {
	  _LL17: _temp16 = (struct Cyc_Absyn_Enumdecl *)((struct Cyc_Tcenv_EnumRes_struct *)_temp2)->f1;
		 goto _LL15;
	  _LL15: _temp14 = (struct Cyc_Absyn_Enumfield *)((struct Cyc_Tcenv_EnumRes_struct *)_temp2)->f2;
		 goto _LL7;
	}
	else {
	  goto _LL8;
	}
	_LL8: if (*((int *)_temp2) == Cyc_Tcenv_TunionRes) {
	  _LL21: _temp20 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Tcenv_TunionRes_struct *)_temp2)->f1;
		 goto _LL19;
	  _LL19: _temp18 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Tcenv_TunionRes_struct *)_temp2)->f2;
		 goto _LL9;
	}
	else {
	  goto _LL10;
	}
	_LL10: if (*((int *)_temp2) == Cyc_Tcenv_StructRes) {
	  _LL23: _temp22 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Tcenv_StructRes_struct *)_temp2)->f1;
		 goto _LL11;
	}
	else {
	  goto _LL3;
	}
	_LL5: (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Var_e_struct *_temp24 = (struct Cyc_Absyn_Var_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Var_e_struct));
		_temp24[0] = ({ struct Cyc_Absyn_Var_e_struct _temp25;
		  _temp25.tag = Cyc_Absyn_Var_e;
		  _temp25.f1 = q;
		  _temp25.f2 = (void *)_temp12;
		  _temp25; });
		_temp24; })));
	      goto _LL3;
	_LL7: (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Enum_e_struct *_temp26 = (struct Cyc_Absyn_Enum_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Enum_e_struct));
		_temp26[0] = ({ struct Cyc_Absyn_Enum_e_struct _temp27;
		  _temp27.tag = Cyc_Absyn_Enum_e;
		  _temp27.f1 = q;
		  _temp27.f2 = (struct Cyc_Absyn_Enumdecl *)_temp16;
		  _temp27.f3 = (struct Cyc_Absyn_Enumfield *)_temp14;
		  _temp27; });
		_temp26; })));
	      goto _LL3;
	_LL9: (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Tunion_e_struct *_temp28 = (struct Cyc_Absyn_Tunion_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Tunion_e_struct));
		_temp28[0] = ({ struct Cyc_Absyn_Tunion_e_struct _temp29;
		  _temp29.tag = Cyc_Absyn_Tunion_e;
		  _temp29.f1 = 0;
		  _temp29.f2 = 0;
		  _temp29.f3 = 0;
		  _temp29.f4 = _temp20;
		  _temp29.f5 = _temp18;
		  _temp29; });
		_temp28; })));
	      goto _LL3;
	_LL11: Cyc_Tcutil_terr(e->loc,({ struct _tagged_string _temp30 = Cyc_Absynpp_qvar2string(q);
				 xprintf("bad occurrence of struct name %.*s",
					 _temp30.last_plus_one - _temp30.curr,
					 _temp30.curr); }));
	       (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Var_e_struct *_temp31 = (struct Cyc_Absyn_Var_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Var_e_struct));
		 _temp31[0] = ({ struct Cyc_Absyn_Var_e_struct _temp32;
		   _temp32.tag = Cyc_Absyn_Var_e;
		   _temp32.f1 = q;
		   _temp32.f2 = (void *)((void *)Cyc_Absyn_Unresolved_b);
		   _temp32; });
		 _temp31; })));
	       goto _LL3;
	_LL3: ;
      }
      ;
      _pop_handler();
    }
    else {
      void *_temp34 = _temp1;
      _LL36: if (_temp34 == Cyc_Dict_Absent) {
	goto _LL37;
      }
      else {
	goto _LL38;
      }
      _LL38: goto _LL39;
      _LL37: (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Var_e_struct *_temp40 = (struct Cyc_Absyn_Var_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Var_e_struct));
	       _temp40[0] = ({ struct Cyc_Absyn_Var_e_struct _temp41;
		 _temp41.tag = Cyc_Absyn_Var_e;
		 _temp41.f1 = q;
		 _temp41.f2 = (void *)((void *)Cyc_Absyn_Unresolved_b);
		 _temp41; });
	       _temp40; })));
	     goto _LL35;
      _LL39: (void)_throw(_temp34);
      _LL35: ;
    }}
  
}
struct _tuple6 {
  struct Cyc_List_List *f1;
  struct Cyc_Absyn_Exp *f2;
};
static struct _tuple6 *Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp *e) {
  return ({ struct _tuple6 *_temp42 = (struct _tuple6 *)GC_malloc(sizeof(struct _tuple6));
    _temp42->f1 = 0;
    _temp42->f2 = e;
    _temp42; });
}
static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Exp *e,
					 struct Cyc_Absyn_Exp *e1,struct Cyc_List_List *es) {
  void *_temp43 = (void *)e1->r;
  struct _tuple1 *_temp49;
  _LL45: if (*((int *)_temp43) == Cyc_Absyn_UnknownId_e) {
    _LL50: _temp49 = (struct _tuple1 *)((struct Cyc_Absyn_UnknownId_e_struct *)_temp43)->f1;
	   goto _LL46;
  }
  else {
    goto _LL47;
  }
  _LL47: goto _LL48;
  _LL46: {
  struct _handler_cons _temp51;
    _push_handler(&_temp51);
    { void *_temp52 = (void *)setjmp(_temp51.handler);
      if (! _temp52) {
	{ void *_temp53 = Cyc_Tcenv_lookup_ordinary(te,e1->loc,_temp49);
	  void *_temp63;
	  struct Cyc_Absyn_Tunionfield *_temp65;
	  struct Cyc_Absyn_Tuniondecl *_temp67;
	  struct Cyc_Absyn_Structdecl *_temp69;
	  struct Cyc_Absyn_Enumfield *_temp71;
	  struct Cyc_Absyn_Enumdecl *_temp73;
	  _LL55: if (*((int *)_temp53) == Cyc_Tcenv_VarRes) {
	    _LL64: _temp63 = (void *)((struct Cyc_Tcenv_VarRes_struct *)_temp53)->f1;
		   goto _LL56;
	  }
	  else {
	    goto _LL57;
	  }
	  _LL57: if (*((int *)_temp53) == Cyc_Tcenv_TunionRes) {
	    _LL68: _temp67 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Tcenv_TunionRes_struct *)_temp53)->f1;
		   goto _LL66;
	    _LL66: _temp65 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Tcenv_TunionRes_struct *)_temp53)->f2;
		   goto _LL58;
	  }
	  else {
	    goto _LL59;
	  }
	  _LL59: if (*((int *)_temp53) == Cyc_Tcenv_StructRes) {
	    _LL70: _temp69 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Tcenv_StructRes_struct *)_temp53)->f1;
		   goto _LL60;
	  }
	  else {
	    goto _LL61;
	  }
	  _LL61: if (*((int *)_temp53) == Cyc_Tcenv_EnumRes) {
	    _LL74: _temp73 = (struct Cyc_Absyn_Enumdecl *)((struct Cyc_Tcenv_EnumRes_struct *)_temp53)->f1;
		   goto _LL72;
	    _LL72: _temp71 = (struct Cyc_Absyn_Enumfield *)((struct Cyc_Tcenv_EnumRes_struct *)_temp53)->f2;
		   goto _LL62;
	  }
	  else {
	    goto _LL54;
	  }
	  _LL56: (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_FnCall_e_struct *_temp75 = (struct Cyc_Absyn_FnCall_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_FnCall_e_struct));
		   _temp75[0] = ({ struct Cyc_Absyn_FnCall_e_struct _temp76;
		     _temp76.tag = Cyc_Absyn_FnCall_e;
		     _temp76.f1 = e1;
		     _temp76.f2 = es;
		     _temp76; });
		   _temp75; })));
		 _npop_handler(0);
		 return;
	  _LL58: if (_temp65->typs == 0) {
		   Cyc_Tcutil_terr(e->loc,({ struct _tagged_string _temp77 = Cyc_Absynpp_qvar2string(_temp65->name);
				     xprintf("%.*s is a constant, not a function",
					     _temp77.last_plus_one - _temp77.curr,
					     _temp77.curr); }));
		 }
		 (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Tunion_e_struct *_temp78 = (struct Cyc_Absyn_Tunion_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Tunion_e_struct));
		   _temp78[0] = ({ struct Cyc_Absyn_Tunion_e_struct _temp79;
		     _temp79.tag = Cyc_Absyn_Tunion_e;
		     _temp79.f1 = 0;
		     _temp79.f2 = 0;
		     _temp79.f3 = es;
		     _temp79.f4 = _temp67;
		     _temp79.f5 = _temp65;
		     _temp79; });
		   _temp78; })));
		 _npop_handler(0);
		 return;
		 _npop_handler(0);
		 return;
	  _LL60: {
	  struct Cyc_List_List *des = ((struct Cyc_List_List *(*)(struct _tuple6 *(*f)(struct Cyc_Absyn_Exp *),
								  struct Cyc_List_List *x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,
													 es);
	    if (_temp69->name != 0) {
	      (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Struct_e_struct *_temp80 = (struct Cyc_Absyn_Struct_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Struct_e_struct));
		_temp80[0] = ({ struct Cyc_Absyn_Struct_e_struct _temp81;
		  _temp81.tag = Cyc_Absyn_Struct_e;
		  _temp81.f1 = (struct _tuple1 *)({ struct Cyc_Core_Opt *_temp82 = _temp69->name;
		    if (_temp82 == 0) {
		      _throw(Null_Exception);
		    }
		    _temp82->v; });
		  _temp81.f2 = 0;
		  _temp81.f3 = des;
		  _temp81.f4 = (struct Cyc_Absyn_Structdecl *)_temp69;
		  _temp81; });
		_temp80; })));
	    }
	    else {
	      Cyc_Tcutil_terr(e->loc,(struct _tagged_string)({ char *_temp83 = (char *)"missing struct name";
				struct _tagged_string _temp84;
				_temp84.curr = _temp83;
				_temp84.base = _temp83;
				_temp84.last_plus_one = _temp83 + 20;
				_temp84; }));
	      (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Struct_e_struct *_temp85 = (struct Cyc_Absyn_Struct_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Struct_e_struct));
		_temp85[0] = ({ struct Cyc_Absyn_Struct_e_struct _temp86;
		  _temp86.tag = Cyc_Absyn_Struct_e;
		  _temp86.f1 = _temp49;
		  _temp86.f2 = 0;
		  _temp86.f3 = des;
		  _temp86.f4 = (struct Cyc_Absyn_Structdecl *)_temp69;
		  _temp86; });
		_temp85; })));
	    }
	    _npop_handler(0);
	    return;
	  }
	  _LL62: Cyc_Tcutil_terr(e->loc,({ struct _tagged_string _temp87 = Cyc_Absynpp_qvar2string(_temp49);
				   xprintf("%.*s is an enum constructor, not a function",
					   _temp87.last_plus_one - _temp87.curr,
					   _temp87.curr); }));
		 _npop_handler(0);
		 return;
	  _LL54: ;
	}
	;
	_pop_handler();
      }
      else {
	void *_temp89 = _temp52;
	_LL91: if (_temp89 == Cyc_Dict_Absent) {
	  goto _LL92;
	}
	else {
	  goto _LL93;
	}
	_LL93: goto _LL94;
	_LL92: Cyc_Tcutil_terr(e1->loc,({ struct _tagged_string _temp95 = Cyc_Absynpp_qvar2string(_temp49);
				 xprintf("unbound identifier %.*s",_temp95.last_plus_one - _temp95.curr,
					 _temp95.curr); }));
	       (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_FnCall_e_struct *_temp96 = (struct Cyc_Absyn_FnCall_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_FnCall_e_struct));
		 _temp96[0] = ({ struct Cyc_Absyn_FnCall_e_struct _temp97;
		   _temp97.tag = Cyc_Absyn_FnCall_e;
		   _temp97.f1 = e1;
		   _temp97.f2 = es;
		   _temp97; });
		 _temp96; })));
	       return;
	_LL94: (void)_throw(_temp89);
	_LL90: ;
      }}
    
  }
  _LL48: (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_FnCall_e_struct *_temp98 = (struct Cyc_Absyn_FnCall_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_FnCall_e_struct));
	   _temp98[0] = ({ struct Cyc_Absyn_FnCall_e_struct _temp99;
	     _temp99.tag = Cyc_Absyn_FnCall_e;
	     _temp99.f1 = e1;
	     _temp99.f2 = es;
	     _temp99; });
	   _temp98; })));
	 return;
  _LL44: ;
}
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
					     void **topt,struct Cyc_Absyn_Exp *e,
					     struct Cyc_List_List *des) {
  if (topt == 0) {
    (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Array_e_struct *_temp100 = (struct Cyc_Absyn_Array_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Array_e_struct));
      _temp100[0] = ({ struct Cyc_Absyn_Array_e_struct _temp101;
	_temp101.tag = Cyc_Absyn_Array_e;
	_temp101.f1 = des;
	_temp101; });
      _temp100; })));
    return;
  }
  { void *t = ({ void **_temp138 = topt;
      if (_temp138 == 0) {
	_throw(Null_Exception);
      }
      *_temp138; });
    void *_temp102 = Cyc_Tcutil_compress(t);
    struct Cyc_Absyn_Exp *_temp112;
    struct Cyc_Absyn_Tqual _temp114;
    void *_temp116;
    struct Cyc_Absyn_Structdecl **_temp118;
    struct Cyc_List_List *_temp120;
    struct _tuple1 *_temp122;
    struct Cyc_List_List *_temp124;
    _LL104: if ((unsigned int)_temp102 > 4 ? *((int *)_temp102) == Cyc_Absyn_ArrayType : 0) {
      _LL117: _temp116 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp102)->f1;
	      goto _LL115;
      _LL115: _temp114 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp102)->f2;
	      goto _LL113;
      _LL113: _temp112 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp102)->f3;
	      goto _LL105;
    }
    else {
      goto _LL106;
    }
    _LL106: if ((unsigned int)_temp102 > 4 ? *((int *)_temp102) == Cyc_Absyn_StructType : 0) {
      _LL123: _temp122 = (struct _tuple1 *)((struct Cyc_Absyn_StructType_struct *)_temp102)->f1;
	      goto _LL121;
      _LL121: _temp120 = (struct Cyc_List_List *)((struct Cyc_Absyn_StructType_struct *)_temp102)->f2;
	      goto _LL119;
      _LL119: _temp118 = (struct Cyc_Absyn_Structdecl **)((struct Cyc_Absyn_StructType_struct *)_temp102)->f3;
	      goto _LL107;
    }
    else {
      goto _LL108;
    }
    _LL108: if ((unsigned int)_temp102 > 4 ? *((int *)_temp102) == Cyc_Absyn_AnonStructType : 0) {
      _LL125: _temp124 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonStructType_struct *)_temp102)->f1;
	      goto _LL109;
    }
    else {
      goto _LL110;
    }
    _LL110: goto _LL111;
    _LL105: (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Array_e_struct *_temp126 = (struct Cyc_Absyn_Array_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Array_e_struct));
	      _temp126[0] = ({ struct Cyc_Absyn_Array_e_struct _temp127;
		_temp127.tag = Cyc_Absyn_Array_e;
		_temp127.f1 = des;
		_temp127; });
	      _temp126; })));
	    goto _LL103;
    _LL107: if (_temp118 == 0 ? 1 : _temp122 == 0) {
	      Cyc_Tcutil_terr(loc,(struct _tagged_string)({ char *_temp128 = (char *)"Compiler bug: struct type not properly set";
				struct _tagged_string _temp129;
				_temp129.curr = _temp128;
				_temp129.base = _temp128;
				_temp129.last_plus_one = _temp128 + 43;
				_temp129; }));
	    }
	    (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Struct_e_struct *_temp130 = (struct Cyc_Absyn_Struct_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Struct_e_struct));
	      _temp130[0] = ({ struct Cyc_Absyn_Struct_e_struct _temp131;
		_temp131.tag = Cyc_Absyn_Struct_e;
		_temp131.f1 = (struct _tuple1 *)({ struct _tuple1 *_temp132 = _temp122;
		  if (_temp132 == 0) {
		    _throw(Null_Exception);
		  }
		  _temp132; });
		_temp131.f2 = 0;
		_temp131.f3 = des;
		_temp131.f4 = (struct Cyc_Absyn_Structdecl *)({ struct Cyc_Absyn_Structdecl **_temp133 = _temp118;
		  if (_temp133 == 0) {
		    _throw(Null_Exception);
		  }
		  *_temp133; });
		_temp131; });
	      _temp130; })));
	    goto _LL103;
    _LL109: (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_AnonStruct_e_struct *_temp134 = (struct Cyc_Absyn_AnonStruct_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_AnonStruct_e_struct));
	      _temp134[0] = ({ struct Cyc_Absyn_AnonStruct_e_struct _temp135;
		_temp135.tag = Cyc_Absyn_AnonStruct_e;
		_temp135.f1 = (void *)t;
		_temp135.f2 = des;
		_temp135; });
	      _temp134; })));
	    goto _LL103;
    _LL111: (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Array_e_struct *_temp136 = (struct Cyc_Absyn_Array_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Array_e_struct));
	      _temp136[0] = ({ struct Cyc_Absyn_Array_e_struct _temp137;
		_temp137.tag = Cyc_Absyn_Array_e;
		_temp137.f1 = des;
		_temp137; });
	      _temp136; })));
	    goto _LL103;
    _LL103: ;}
  
}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv *te,void **topt,struct Cyc_Absyn_Exp *e);
static void *Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv *te,void **topt,
				      struct Cyc_Absyn_Exp *e);
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv *te,struct Cyc_List_List *es) {
  for(0; es != 0; es = ({ struct Cyc_List_List *_temp139 = es;
    if (_temp139 == 0) {
      _throw(Null_Exception);
    }
    _temp139->tl; })) {
    Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp140 = es;
		      if (_temp140 == 0) {
			_throw(Null_Exception);
		      }
		      _temp140->hd; }));
  }
}
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Exp *e,struct _tagged_string msg_part) {
  Cyc_Tcutil_check_contains_assign(e);
  Cyc_Tcexp_tcExp(te,(void **)&Cyc_Absyn_sint_t,e);
  if (! Cyc_Tcutil_coerce_to_bool(te,e)) {
    Cyc_Tcutil_terr(e->loc,({ struct _tagged_string _temp142 = msg_part;
		      struct _tagged_string _temp143 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp141 = e->topt;
										if (_temp141 == 0) {
										  _throw(Null_Exception);
										}
										_temp141->v; }));
		      xprintf("test of %.*s has type %.*s instead of integral or * type",
			      _temp142.last_plus_one - _temp142.curr,_temp142.curr,
			      _temp143.last_plus_one - _temp143.curr,_temp143.curr); }));
  }
}
static void *Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
			       void **topt,void *c,struct Cyc_Absyn_Exp *e) {
  void *t;
  { void *_temp144 = c;
    char _temp160;
    void *_temp162;
    short _temp164;
    void *_temp166;
    long long _temp168;
    void *_temp170;
    struct _tagged_string _temp172;
    int _temp174;
    void *_temp176;
    struct _tagged_string _temp178;
    _LL146: if ((unsigned int)_temp144 > 1 ? *((int *)_temp144) == Cyc_Absyn_Char_c : 0) {
      _LL163: _temp162 = (void *)((struct Cyc_Absyn_Char_c_struct *)_temp144)->f1;
	      goto _LL161;
      _LL161: _temp160 = (char)((struct Cyc_Absyn_Char_c_struct *)_temp144)->f2;
	      goto _LL147;
    }
    else {
      goto _LL148;
    }
    _LL148: if ((unsigned int)_temp144 > 1 ? *((int *)_temp144) == Cyc_Absyn_Short_c : 0) {
      _LL167: _temp166 = (void *)((struct Cyc_Absyn_Short_c_struct *)_temp144)->f1;
	      goto _LL165;
      _LL165: _temp164 = (short)((struct Cyc_Absyn_Short_c_struct *)_temp144)->f2;
	      goto _LL149;
    }
    else {
      goto _LL150;
    }
    _LL150: if ((unsigned int)_temp144 > 1 ? *((int *)_temp144) == Cyc_Absyn_LongLong_c : 0) {
      _LL171: _temp170 = (void *)((struct Cyc_Absyn_LongLong_c_struct *)_temp144)->f1;
	      goto _LL169;
      _LL169: _temp168 = (long long)((struct Cyc_Absyn_LongLong_c_struct *)_temp144)->f2;
	      goto _LL151;
    }
    else {
      goto _LL152;
    }
    _LL152: if ((unsigned int)_temp144 > 1 ? *((int *)_temp144) == Cyc_Absyn_Float_c : 0) {
      _LL173: _temp172 = (struct _tagged_string)((struct Cyc_Absyn_Float_c_struct *)_temp144)->f1;
	      goto _LL153;
    }
    else {
      goto _LL154;
    }
    _LL154: if ((unsigned int)_temp144 > 1 ? *((int *)_temp144) == Cyc_Absyn_Int_c : 0) {
      _LL177: _temp176 = (void *)((struct Cyc_Absyn_Int_c_struct *)_temp144)->f1;
	      goto _LL175;
      _LL175: _temp174 = (int)((struct Cyc_Absyn_Int_c_struct *)_temp144)->f2;
	      goto _LL155;
    }
    else {
      goto _LL156;
    }
    _LL156: if ((unsigned int)_temp144 > 1 ? *((int *)_temp144) == Cyc_Absyn_String_c : 0) {
      _LL179: _temp178 = (struct _tagged_string)((struct Cyc_Absyn_String_c_struct *)_temp144)->f1;
	      goto _LL157;
    }
    else {
      goto _LL158;
    }
    _LL158: if (_temp144 == (void *)Cyc_Absyn_Null_c) {
      goto _LL159;
    }
    else {
      goto _LL145;
    }
    _LL147: t = _temp162 == (void *)Cyc_Absyn_Unsigned ? Cyc_Absyn_uchar_t : Cyc_Absyn_schar_t;
	    goto _LL145;
    _LL149: t = _temp166 == (void *)Cyc_Absyn_Unsigned ? Cyc_Absyn_ushort_t : Cyc_Absyn_sshort_t;
	    goto _LL145;
    _LL151: t = _temp170 == (void *)Cyc_Absyn_Unsigned ? Cyc_Absyn_ulong_t : Cyc_Absyn_slong_t;
	    goto _LL145;
    _LL153: t = Cyc_Absyn_float_t;
	    goto _LL145;
    _LL155: if (topt == 0) {
	      t = _temp176 == (void *)Cyc_Absyn_Unsigned ? Cyc_Absyn_uint_t : Cyc_Absyn_sint_t;
	    }
	    else {
	      void *_temp181 = ({ void **_temp180 = topt;
		if (_temp180 == 0) {
		  _throw(Null_Exception);
		}
		*_temp180; });
	      void *_temp191;
	      void *_temp193;
	      void *_temp195;
	      void *_temp197;
	      void *_temp199;
	      void *_temp201;
	      _LL183: if ((unsigned int)_temp181 > 4 ? *((int *)_temp181) == Cyc_Absyn_IntType : 0) {
		_LL194: _temp193 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp181)->f1;
			goto _LL192;
		_LL192: _temp191 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp181)->f2;
			if (_temp191 == (void *)Cyc_Absyn_B1) {
			  goto _LL184;
			}
			else {
			  goto _LL185;
			}
	      }
	      else {
		goto _LL185;
	      }
	      _LL185: if ((unsigned int)_temp181 > 4 ? *((int *)_temp181) == Cyc_Absyn_IntType : 0) {
		_LL198: _temp197 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp181)->f1;
			goto _LL196;
		_LL196: _temp195 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp181)->f2;
			if (_temp195 == (void *)Cyc_Absyn_B2) {
			  goto _LL186;
			}
			else {
			  goto _LL187;
			}
	      }
	      else {
		goto _LL187;
	      }
	      _LL187: if ((unsigned int)_temp181 > 4 ? *((int *)_temp181) == Cyc_Absyn_IntType : 0) {
		_LL202: _temp201 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp181)->f1;
			goto _LL200;
		_LL200: _temp199 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp181)->f2;
			goto _LL188;
	      }
	      else {
		goto _LL189;
	      }
	      _LL189: goto _LL190;
	      _LL184: t = _temp193 == (void *)Cyc_Absyn_Unsigned ? Cyc_Absyn_uchar_t : Cyc_Absyn_schar_t;
		      (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Const_e_struct *_temp203 = (struct Cyc_Absyn_Const_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Const_e_struct));
			_temp203[0] = ({ struct Cyc_Absyn_Const_e_struct _temp204;
			  _temp204.tag = Cyc_Absyn_Const_e;
			  _temp204.f1 = (void *)((void *)({ struct Cyc_Absyn_Char_c_struct *_temp205 = (struct Cyc_Absyn_Char_c_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Char_c_struct));
			    _temp205[0] = ({ struct Cyc_Absyn_Char_c_struct _temp206;
			      _temp206.tag = Cyc_Absyn_Char_c;
			      _temp206.f1 = (void *)_temp193;
			      _temp206.f2 = (char)_temp174;
			      _temp206; });
			    _temp205; }));
			  _temp204; });
			_temp203; })));
		      goto _LL182;
	      _LL186: t = _temp197 == (void *)Cyc_Absyn_Unsigned ? Cyc_Absyn_ushort_t : Cyc_Absyn_sshort_t;
		      (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Const_e_struct *_temp207 = (struct Cyc_Absyn_Const_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Const_e_struct));
			_temp207[0] = ({ struct Cyc_Absyn_Const_e_struct _temp208;
			  _temp208.tag = Cyc_Absyn_Const_e;
			  _temp208.f1 = (void *)((void *)({ struct Cyc_Absyn_Short_c_struct *_temp209 = (struct Cyc_Absyn_Short_c_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Short_c_struct));
			    _temp209[0] = ({ struct Cyc_Absyn_Short_c_struct _temp210;
			      _temp210.tag = Cyc_Absyn_Short_c;
			      _temp210.f1 = (void *)_temp197;
			      _temp210.f2 = (short)_temp174;
			      _temp210; });
			    _temp209; }));
			  _temp208; });
			_temp207; })));
		      goto _LL182;
	      _LL188: t = _temp201 == (void *)Cyc_Absyn_Unsigned ? Cyc_Absyn_uint_t : Cyc_Absyn_sint_t;
		      goto _LL182;
	      _LL190: t = _temp176 == (void *)Cyc_Absyn_Unsigned ? Cyc_Absyn_uint_t : Cyc_Absyn_sint_t;
		      goto _LL182;
	      _LL182: ;
	    }
	    goto _LL145;
    _LL157: {
    int len = (int)({ struct _tagged_string _temp259 = _temp178;
	(unsigned int)(_temp259.last_plus_one - _temp259.curr); });
      struct Cyc_Absyn_Const_e_struct *r = ({ struct Cyc_Absyn_Const_e_struct *_temp255 = (struct Cyc_Absyn_Const_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Const_e_struct));
	_temp255[0] = ({ struct Cyc_Absyn_Const_e_struct _temp256;
	  _temp256.tag = Cyc_Absyn_Const_e;
	  _temp256.f1 = (void *)((void *)({ struct Cyc_Absyn_Int_c_struct *_temp257 = (struct Cyc_Absyn_Int_c_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Int_c_struct));
	    _temp257[0] = ({ struct Cyc_Absyn_Int_c_struct _temp258;
	      _temp258.tag = Cyc_Absyn_Int_c;
	      _temp258.f1 = (void *)((void *)Cyc_Absyn_Unsigned);
	      _temp258.f2 = len;
	      _temp258; });
	    _temp257; }));
	  _temp256; });
	_temp255; });
      struct Cyc_Absyn_Exp *elen = Cyc_Absyn_new_exp((void *)r,loc);
      elen->topt = ({ struct Cyc_Core_Opt *_temp211 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
	_temp211->v = (void *)Cyc_Absyn_uint_t;
	_temp211; });
      t = Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,(void *)Cyc_Absyn_HeapRgn,Cyc_Absyn_empty_tqual(),
			    (void *)({ struct Cyc_Absyn_Upper_b_struct *_temp212 = (struct Cyc_Absyn_Upper_b_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
			      _temp212[0] = ({ struct Cyc_Absyn_Upper_b_struct _temp213;
				_temp213.tag = Cyc_Absyn_Upper_b;
				_temp213.f1 = elen;
				_temp213; });
			      _temp212; }));
      if (topt != 0) {
	void *_temp215 = Cyc_Tcutil_compress(({ void **_temp214 = topt;
					       if (_temp214 == 0) {
						 _throw(Null_Exception);
					       }
					       *_temp214; }));
	struct Cyc_Absyn_Exp *_temp223;
	struct Cyc_Absyn_Tqual _temp225;
	void *_temp227;
	struct Cyc_Absyn_PtrInfo _temp229;
	struct Cyc_Absyn_Conref *_temp231;
	struct Cyc_Absyn_Tqual _temp233;
	struct Cyc_Absyn_Conref *_temp235;
	void *_temp237;
	void *_temp239;
	_LL217: if ((unsigned int)_temp215 > 4 ? *((int *)_temp215) == Cyc_Absyn_ArrayType : 0) {
	  _LL228: _temp227 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp215)->f1;
		  goto _LL226;
	  _LL226: _temp225 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp215)->f2;
		  goto _LL224;
	  _LL224: _temp223 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp215)->f3;
		  goto _LL218;
	}
	else {
	  goto _LL219;
	}
	_LL219: if ((unsigned int)_temp215 > 4 ? *((int *)_temp215) == Cyc_Absyn_PointerType : 0) {
	  _LL230: _temp229 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp215)->f1;
		  _LL240: _temp239 = (void *)_temp229.elt_typ;
			  goto _LL238;
		  _LL238: _temp237 = (void *)_temp229.rgn_typ;
			  goto _LL236;
		  _LL236: _temp235 = (struct Cyc_Absyn_Conref *)_temp229.nullable;
			  goto _LL234;
		  _LL234: _temp233 = (struct Cyc_Absyn_Tqual)_temp229.tq;
			  goto _LL232;
		  _LL232: _temp231 = (struct Cyc_Absyn_Conref *)_temp229.bounds;
			  goto _LL220;
	}
	else {
	  goto _LL221;
	}
	_LL221: goto _LL222;
	_LL218: return (void *)({ struct Cyc_Absyn_ArrayType_struct *_temp241 = (struct Cyc_Absyn_ArrayType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_ArrayType_struct));
	  _temp241[0] = ({ struct Cyc_Absyn_ArrayType_struct _temp242;
	    _temp242.tag = Cyc_Absyn_ArrayType;
	    _temp242.f1 = (void *)Cyc_Absyn_uchar_t;
	    _temp242.f2 = _temp225;
	    _temp242.f3 = (struct Cyc_Absyn_Exp *)elen;
	    _temp242; });
	  _temp241; });
	_LL220: if (! Cyc_Tcutil_unify(({ void **_temp243 = topt;
					 if (_temp243 == 0) {
					   _throw(Null_Exception);
					 }
					 *_temp243; }),t) ? Cyc_Tcutil_silent_castable(te,
										       loc,
										       t,
										       ({ void **_temp244 = topt;
											 if (_temp244 == 0) {
											   _throw(Null_Exception);
											 }
											 *_temp244; })) : 0) {
		  e->topt = ({ struct Cyc_Core_Opt *_temp245 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
		    _temp245->v = (void *)t;
		    _temp245; });
		  Cyc_Tcutil_unchecked_cast(te,e,({ void **_temp246 = topt;
					      if (_temp246 == 0) {
						_throw(Null_Exception);
					      }
					      *_temp246; }));
		  t = ({ void **_temp247 = topt;
		    if (_temp247 == 0) {
		      _throw(Null_Exception);
		    }
		    *_temp247; });
		}
		else {
		  t = Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,Cyc_Absyn_new_evar((void *)Cyc_Absyn_RgnKind),
					Cyc_Absyn_empty_tqual(),(void *)({ struct Cyc_Absyn_Upper_b_struct *_temp248 = (struct Cyc_Absyn_Upper_b_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
					  _temp248[0] = ({ struct Cyc_Absyn_Upper_b_struct _temp249;
					    _temp249.tag = Cyc_Absyn_Upper_b;
					    _temp249.f1 = elen;
					    _temp249; });
					  _temp248; }));
		  if (! Cyc_Tcutil_unify(({ void **_temp250 = topt;
					   if (_temp250 == 0) {
					     _throw(Null_Exception);
					   }
					   *_temp250; }),t) ? Cyc_Tcutil_silent_castable(te,
											 loc,
											 t,
											 ({ void **_temp251 = topt;
											   if (_temp251 == 0) {
											     _throw(Null_Exception);
											   }
											   *_temp251; })) : 0) {
		    e->topt = ({ struct Cyc_Core_Opt *_temp252 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
		      _temp252->v = (void *)t;
		      _temp252; });
		    Cyc_Tcutil_unchecked_cast(te,e,({ void **_temp253 = topt;
						if (_temp253 == 0) {
						  _throw(Null_Exception);
						}
						*_temp253; }));
		    t = ({ void **_temp254 = topt;
		      if (_temp254 == 0) {
			_throw(Null_Exception);
		      }
		      *_temp254; });
		  }
		}
		goto _LL216;
	_LL222: goto _LL216;
	_LL216: ;
      }
      return t;
    }
    _LL159: t = (void *)({ struct Cyc_Absyn_PointerType_struct *_temp260 = (struct Cyc_Absyn_PointerType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_PointerType_struct));
	      _temp260[0] = ({ struct Cyc_Absyn_PointerType_struct _temp261;
		_temp261.tag = Cyc_Absyn_PointerType;
		_temp261.f1 = ({ struct Cyc_Absyn_PtrInfo _temp262;
		  _temp262.elt_typ = (void *)Cyc_Absyn_new_evar((void *)Cyc_Absyn_MemKind);
		  _temp262.rgn_typ = (void *)Cyc_Absyn_new_evar((void *)Cyc_Absyn_RgnKind);
		  _temp262.nullable = ((struct Cyc_Absyn_Conref *(*)(int x))Cyc_Absyn_new_conref)(1);
		  _temp262.tq = Cyc_Absyn_empty_tqual();
		  _temp262.bounds = ((struct Cyc_Absyn_Conref *(*)())Cyc_Absyn_empty_conref)();
		  _temp262; });
		_temp261; });
	      _temp260; });
	    goto _LL145;
    _LL145: ;
  }
  return t;
}
static void *Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
			     struct _tuple1 *q,void *b) {
  void *_temp263 = b;
  struct Cyc_Absyn_Vardecl *_temp277;
  struct Cyc_Absyn_Fndecl *_temp279;
  struct Cyc_Absyn_Vardecl *_temp281;
  struct Cyc_Absyn_Vardecl *_temp283;
  struct Cyc_Absyn_Vardecl *_temp285;
  _LL265: if (_temp263 == (void *)Cyc_Absyn_Unresolved_b) {
    goto _LL266;
  }
  else {
    goto _LL267;
  }
  _LL267: if ((unsigned int)_temp263 > 1 ? *((int *)_temp263) == Cyc_Absyn_Global_b : 0) {
    _LL278: _temp277 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Global_b_struct *)_temp263)->f1;
	    goto _LL268;
  }
  else {
    goto _LL269;
  }
  _LL269: if ((unsigned int)_temp263 > 1 ? *((int *)_temp263) == Cyc_Absyn_Funname_b : 0) {
    _LL280: _temp279 = (struct Cyc_Absyn_Fndecl *)((struct Cyc_Absyn_Funname_b_struct *)_temp263)->f1;
	    goto _LL270;
  }
  else {
    goto _LL271;
  }
  _LL271: if ((unsigned int)_temp263 > 1 ? *((int *)_temp263) == Cyc_Absyn_Pat_b : 0) {
    _LL282: _temp281 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Pat_b_struct *)_temp263)->f1;
	    goto _LL272;
  }
  else {
    goto _LL273;
  }
  _LL273: if ((unsigned int)_temp263 > 1 ? *((int *)_temp263) == Cyc_Absyn_Local_b : 0) {
    _LL284: _temp283 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Local_b_struct *)_temp263)->f1;
	    goto _LL274;
  }
  else {
    goto _LL275;
  }
  _LL275: if ((unsigned int)_temp263 > 1 ? *((int *)_temp263) == Cyc_Absyn_Param_b : 0) {
    _LL286: _temp285 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Param_b_struct *)_temp263)->f1;
	    goto _LL276;
  }
  else {
    goto _LL264;
  }
  _LL266: return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp287 = Cyc_Absynpp_qvar2string(q);
				      xprintf("unbound identifier %.*s",_temp287.last_plus_one - _temp287.curr,
					      _temp287.curr); }));
  _LL268: ({ struct _tuple1 *_temp288 = q;
	    unsigned int _temp289 = 0;
	    if (_temp289 >= 1) {
	      _throw(Null_Exception);
	    }
	    _temp288[_temp289] = ({ struct _tuple1 *_temp290 = _temp277->name;
	      unsigned int _temp291 = 0;
	      if (_temp291 >= 1) {
		_throw(Null_Exception);
	      }
	      _temp290[_temp291]; }); });
	  return (void *)_temp277->type;
  _LL270: ({ struct _tuple1 *_temp292 = q;
	    unsigned int _temp293 = 0;
	    if (_temp293 >= 1) {
	      _throw(Null_Exception);
	    }
	    _temp292[_temp293] = ({ struct _tuple1 *_temp294 = _temp279->name;
	      unsigned int _temp295 = 0;
	      if (_temp295 >= 1) {
		_throw(Null_Exception);
	      }
	      _temp294[_temp295]; }); });
	  return Cyc_Tcutil_fndecl2typ(_temp279);
  _LL272: _temp283 = _temp281;
	  goto _LL274;
  _LL274: _temp285 = _temp283;
	  goto _LL276;
  _LL276: (*q).f1 = (void *)Cyc_Absyn_Loc_n;
	  return (void *)_temp285->type;
  _LL264: ;
}
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Exp *fmt,
					struct Cyc_List_List *args,struct Cyc_List_List *(*type_getter)(struct _tagged_string,
													struct Cyc_Position_Segment *)) {
  struct Cyc_List_List *desc_types;
  { void *_temp296 = (void *)fmt->r;
    void *_temp302;
    struct _tagged_string _temp304;
    _LL298: if (*((int *)_temp296) == Cyc_Absyn_Const_e) {
      _LL303: _temp302 = (void *)((struct Cyc_Absyn_Const_e_struct *)_temp296)->f1;
	      if ((unsigned int)_temp302 > 1 ? *((int *)_temp302) == Cyc_Absyn_String_c : 0) {
		_LL305: _temp304 = (struct _tagged_string)((struct Cyc_Absyn_String_c_struct *)_temp302)->f1;
			goto _LL299;
	      }
	      else {
		goto _LL300;
	      }
    }
    else {
      goto _LL300;
    }
    _LL300: goto _LL301;
    _LL299: desc_types = type_getter(_temp304,fmt->loc);
	    goto _LL297;
    _LL301: Cyc_Tcutil_terr(fmt->loc,(struct _tagged_string)({ char *_temp306 = (char *)"expecting a literal format string";
			      struct _tagged_string _temp307;
			      _temp307.curr = _temp306;
			      _temp307.base = _temp306;
			      _temp307.last_plus_one = _temp306 + 34;
			      _temp307; }));
	    return;
    _LL297: ;
  }
  for(0; desc_types != 0 ? args != 0 : 0; desc_types = ({ struct Cyc_List_List *_temp308 = desc_types;
    if (_temp308 == 0) {
      _throw(Null_Exception);
    }
    _temp308->tl; }), args = ({ struct Cyc_List_List *_temp309 = args;
    if (_temp309 == 0) {
      _throw(Null_Exception);
    }
    _temp309->tl; })) {
    void *t = (void *)({ struct Cyc_List_List *_temp314 = desc_types;
      if (_temp314 == 0) {
	_throw(Null_Exception);
      }
      _temp314->hd; });
    struct Cyc_Absyn_Exp *e = (struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp313 = args;
      if (_temp313 == 0) {
	_throw(Null_Exception);
      }
      _temp313->hd; });
    if (! Cyc_Tcutil_coerce_arg(te,e,t)) {
      Cyc_Tcutil_terr(e->loc,({ struct _tagged_string _temp311 = Cyc_Absynpp_typ2string(t);
			struct _tagged_string _temp312 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp310 = e->topt;
										  if (_temp310 == 0) {
										    _throw(Null_Exception);
										  }
										  _temp310->v; }));
			xprintf("descriptor has type %.*s but argument has type %.*s",
				_temp311.last_plus_one - _temp311.curr,_temp311.curr,
				_temp312.last_plus_one - _temp312.curr,_temp312.curr); }));
    }
  }
  if (desc_types != 0) {
    Cyc_Tcutil_terr(fmt->loc,(struct _tagged_string)({ char *_temp315 = (char *)"too few arguments";
		      struct _tagged_string _temp316;
		      _temp316.curr = _temp315;
		      _temp316.base = _temp315;
		      _temp316.last_plus_one = _temp315 + 18;
		      _temp316; }));
  }
  if (args != 0) {
    Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp317 = args;
		      if (_temp317 == 0) {
			_throw(Null_Exception);
		      }
		      _temp317->hd; }))->loc,(struct _tagged_string)({ char *_temp318 = (char *)"too many arguments";
		      struct _tagged_string _temp319;
		      _temp319.curr = _temp318;
		      _temp319.base = _temp318;
		      _temp319.last_plus_one = _temp318 + 19;
		      _temp319; }));
  }
}
static void *Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				  void **topt,void *p,struct Cyc_Absyn_Exp *e) {
  void *t = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp368 = e->topt;
				  if (_temp368 == 0) {
				    _throw(Null_Exception);
				  }
				  _temp368->v; }));
  void *_temp320 = p;
  _LL322: if (_temp320 == (void *)Cyc_Absyn_Plus) {
    goto _LL323;
  }
  else {
    goto _LL324;
  }
  _LL324: if (_temp320 == (void *)Cyc_Absyn_Minus) {
    goto _LL325;
  }
  else {
    goto _LL326;
  }
  _LL326: if (_temp320 == (void *)Cyc_Absyn_Not) {
    goto _LL327;
  }
  else {
    goto _LL328;
  }
  _LL328: if (_temp320 == (void *)Cyc_Absyn_Bitnot) {
    goto _LL329;
  }
  else {
    goto _LL330;
  }
  _LL330: if (_temp320 == (void *)Cyc_Absyn_Size) {
    goto _LL331;
  }
  else {
    goto _LL332;
  }
  _LL332: goto _LL333;
  _LL323: goto _LL325;
  _LL325: if (! Cyc_Tcutil_is_numeric(e)) {
	    Cyc_Tcutil_terr(loc,({ struct _tagged_string _temp334 = Cyc_Absynpp_typ2string(t);
			      xprintf("expecting arithmetic type but found %.*s",
				      _temp334.last_plus_one - _temp334.curr,
				      _temp334.curr); }));
	  }
	  return (void *)({ struct Cyc_Core_Opt *_temp335 = e->topt;
	    if (_temp335 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp335->v; });
  _LL327: Cyc_Tcutil_check_contains_assign(e);
	  if (! Cyc_Tcutil_coerce_to_bool(te,e)) {
	    Cyc_Tcutil_terr(loc,({ struct _tagged_string _temp336 = Cyc_Absynpp_typ2string(t);
			      xprintf("expecting integral or * type but found %.*s",
				      _temp336.last_plus_one - _temp336.curr,
				      _temp336.curr); }));
	  }
	  return Cyc_Absyn_sint_t;
  _LL329: if (! Cyc_Tcutil_is_integral(e)) {
	    Cyc_Tcutil_terr(loc,({ struct _tagged_string _temp337 = Cyc_Absynpp_typ2string(t);
			      xprintf("expecting integral type but found %.*s",
				      _temp337.last_plus_one - _temp337.curr,
				      _temp337.curr); }));
	  }
	  return (void *)({ struct Cyc_Core_Opt *_temp338 = e->topt;
	    if (_temp338 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp338->v; });
  _LL331: { void *_temp339 = t;
    struct Cyc_Absyn_Exp *_temp347;
    struct Cyc_Absyn_Tqual _temp349;
    void *_temp351;
    struct Cyc_Absyn_PtrInfo _temp353;
    struct Cyc_Absyn_Conref *_temp355;
    struct Cyc_Absyn_Tqual _temp357;
    struct Cyc_Absyn_Conref *_temp359;
    void *_temp361;
    void *_temp363;
    _LL341: if ((unsigned int)_temp339 > 4 ? *((int *)_temp339) == Cyc_Absyn_ArrayType : 0) {
      _LL352: _temp351 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp339)->f1;
	      goto _LL350;
      _LL350: _temp349 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp339)->f2;
	      goto _LL348;
      _LL348: _temp347 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp339)->f3;
	      goto _LL342;
    }
    else {
      goto _LL343;
    }
    _LL343: if ((unsigned int)_temp339 > 4 ? *((int *)_temp339) == Cyc_Absyn_PointerType : 0) {
      _LL354: _temp353 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp339)->f1;
	      _LL364: _temp363 = (void *)_temp353.elt_typ;
		      goto _LL362;
	      _LL362: _temp361 = (void *)_temp353.rgn_typ;
		      goto _LL360;
	      _LL360: _temp359 = (struct Cyc_Absyn_Conref *)_temp353.nullable;
		      goto _LL358;
	      _LL358: _temp357 = (struct Cyc_Absyn_Tqual)_temp353.tq;
		      goto _LL356;
	      _LL356: _temp355 = (struct Cyc_Absyn_Conref *)_temp353.bounds;
		      goto _LL344;
    }
    else {
      goto _LL345;
    }
    _LL345: goto _LL346;
    _LL342: goto _LL340;
    _LL344: goto _LL340;
    _LL346: Cyc_Tcutil_terr(loc,({ struct _tagged_string _temp365 = Cyc_Absynpp_typ2string(t);
			      xprintf("size requires pointer or array type, not %.*s",
				      _temp365.last_plus_one - _temp365.curr,
				      _temp365.curr); }));
	    goto _LL340;
    _LL340: ;
  }
  return Cyc_Absyn_uint_t;
  _LL333: return ((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp366 = (char *)"Non-unary primop";
									struct _tagged_string _temp367;
									_temp367.curr = _temp366;
									_temp367.base = _temp366;
									_temp367.last_plus_one = _temp366 + 17;
									_temp367; }));
  _LL321: ;
}
static void *Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Exp *e1,
				    struct Cyc_Absyn_Exp *e2,int (*checker)(struct Cyc_Absyn_Exp *)) {
  if (! checker(e1)) {
    Cyc_Tcutil_terr(e1->loc,({ struct _tagged_string _temp370 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp369 = e1->topt;
											 if (_temp369 == 0) {
											   _throw(Null_Exception);
											 }
											 _temp369->v; }));
		      xprintf("type %.*s cannot be used here",_temp370.last_plus_one - _temp370.curr,
			      _temp370.curr); }));
    return Cyc_Absyn_wildtyp();
  }
  if (! checker(e2)) {
    Cyc_Tcutil_terr(e2->loc,({ struct _tagged_string _temp372 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp371 = e2->topt;
											 if (_temp371 == 0) {
											   _throw(Null_Exception);
											 }
											 _temp371->v; }));
		      xprintf("type %.*s cannot be used here",_temp372.last_plus_one - _temp372.curr,
			      _temp372.curr); }));
    return Cyc_Absyn_wildtyp();
  }
  { void *t1 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp374 = e1->topt;
				     if (_temp374 == 0) {
				       _throw(Null_Exception);
				     }
				     _temp374->v; }));
    void *t2 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp373 = e2->topt;
				     if (_temp373 == 0) {
				       _throw(Null_Exception);
				     }
				     _temp373->v; }));
    return Cyc_Tcutil_max_arithmetic_type(t1,t2);}
  
}
static void *Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Exp *e1,
			      struct Cyc_Absyn_Exp *e2) {
  void *t1 = (void *)({ struct Cyc_Core_Opt *_temp377 = e1->topt;
    if (_temp377 == 0) {
      _throw(Null_Exception);
    }
    _temp377->v; });
  if (Cyc_Tcutil_is_tagged_pointer_typ(t1)) {
    if (! Cyc_Tcutil_coerce_sint_typ(te,e2)) {
      Cyc_Tcutil_terr(e2->loc,({ struct _tagged_string _temp376 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp375 = e2->topt;
											   if (_temp375 == 0) {
											     _throw(Null_Exception);
											   }
											   _temp375->v; }));
			xprintf("expecting int but found %.*s",_temp376.last_plus_one - _temp376.curr,
				_temp376.curr); }));
    }
    return t1;
  }
  else {
    return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
  }
}
static void *Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Exp *e1,
			       struct Cyc_Absyn_Exp *e2) {
  void *t1 = (void *)({ struct Cyc_Core_Opt *_temp385 = e1->topt;
    if (_temp385 == 0) {
      _throw(Null_Exception);
    }
    _temp385->v; });
  void *t2 = (void *)({ struct Cyc_Core_Opt *_temp384 = e2->topt;
    if (_temp384 == 0) {
      _throw(Null_Exception);
    }
    _temp384->v; });
  if (Cyc_Tcutil_is_tagged_pointer_typ(t1)) {
    if (Cyc_Tcutil_is_tagged_pointer_typ(t2)) {
      if (! Cyc_Tcutil_unify(t1,t2)) {
	Cyc_Tcutil_terr(e1->loc,({ struct _tagged_string _temp380 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp378 = e1->topt;
											     if (_temp378 == 0) {
											       _throw(Null_Exception);
											     }
											     _temp378->v; }));
			  struct _tagged_string _temp381 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp379 = e2->topt;
										    if (_temp379 == 0) {
										      _throw(Null_Exception);
										    }
										    _temp379->v; }));
			  xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
				  _temp380.last_plus_one - _temp380.curr,
				  _temp380.curr,_temp381.last_plus_one - _temp381.curr,
				  _temp381.curr); }));
      }
      return Cyc_Absyn_sint_t;
    }
    else {
      if (! Cyc_Tcutil_coerce_sint_typ(te,e2)) {
	Cyc_Tcutil_terr(e2->loc,({ struct _tagged_string _temp382 = Cyc_Absynpp_typ2string(t1);
			  struct _tagged_string _temp383 = Cyc_Absynpp_typ2string(t2);
			  xprintf("expecting either %.*s or int but found %.*s",
				  _temp382.last_plus_one - _temp382.curr,
				  _temp382.curr,_temp383.last_plus_one - _temp383.curr,
				  _temp383.curr); }));
      }
      return t1;
    }
  }
  else {
    return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
  }
}
static void *Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				  struct Cyc_Absyn_Exp *e1,struct Cyc_Absyn_Exp *e2) {
  int e1_is_num = Cyc_Tcutil_is_numeric(e1);
  int e2_is_num = Cyc_Tcutil_is_numeric(e2);
  void *t1 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp426 = e1->topt;
				   if (_temp426 == 0) {
				     _throw(Null_Exception);
				   }
				   _temp426->v; }));
  void *t2 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp425 = e2->topt;
				   if (_temp425 == 0) {
				     _throw(Null_Exception);
				   }
				   _temp425->v; }));
  if (e1_is_num ? e2_is_num : 0) {
    return Cyc_Absyn_sint_t;
  }
  else {
    if (Cyc_Tcutil_unify(t1,t2) ? Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((void *)Cyc_Absyn_BoxKind)) : 0) {
      return Cyc_Absyn_sint_t;
    }
    else {
      if (Cyc_Tcutil_silent_castable(te,loc,t2,t1)) {
	Cyc_Tcutil_unchecked_cast(te,e2,t1);
	return Cyc_Absyn_sint_t;
      }
      else {
	if (Cyc_Tcutil_silent_castable(te,loc,t1,t2)) {
	  Cyc_Tcutil_unchecked_cast(te,e1,t2);
	  return Cyc_Absyn_sint_t;
	}
	else {
	  void *t1 = (void *)({ struct Cyc_Core_Opt *_temp424 = e1->topt;
	    if (_temp424 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp424->v; });
	  void *t2 = (void *)({ struct Cyc_Core_Opt *_temp423 = e2->topt;
	    if (_temp423 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp423->v; });
	  { struct _tuple0 _temp387 = ({ struct _tuple0 _temp386;
	      _temp386.f1 = Cyc_Tcutil_compress(t1);
	      _temp386.f2 = Cyc_Tcutil_compress(t2);
	      _temp386; });
	    void *_temp393;
	    struct Cyc_Absyn_PtrInfo _temp395;
	    struct Cyc_Absyn_Conref *_temp397;
	    struct Cyc_Absyn_Tqual _temp399;
	    struct Cyc_Absyn_Conref *_temp401;
	    void *_temp403;
	    void *_temp405;
	    void *_temp407;
	    struct Cyc_Absyn_PtrInfo _temp409;
	    struct Cyc_Absyn_Conref *_temp411;
	    struct Cyc_Absyn_Tqual _temp413;
	    struct Cyc_Absyn_Conref *_temp415;
	    void *_temp417;
	    void *_temp419;
	    _LL389: _LL408: _temp407 = _temp387.f1;
			    if ((unsigned int)_temp407 > 4 ? *((int *)_temp407) == Cyc_Absyn_PointerType : 0) {
			      _LL410: _temp409 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp407)->f1;
				      _LL420: _temp419 = (void *)_temp409.elt_typ;
					      goto _LL418;
				      _LL418: _temp417 = (void *)_temp409.rgn_typ;
					      goto _LL416;
				      _LL416: _temp415 = (struct Cyc_Absyn_Conref *)_temp409.nullable;
					      goto _LL414;
				      _LL414: _temp413 = (struct Cyc_Absyn_Tqual)_temp409.tq;
					      goto _LL412;
				      _LL412: _temp411 = (struct Cyc_Absyn_Conref *)_temp409.bounds;
					      goto _LL394;
			    }
			    else {
			      goto _LL391;
			    }
		    _LL394: _temp393 = _temp387.f2;
			    if ((unsigned int)_temp393 > 4 ? *((int *)_temp393) == Cyc_Absyn_PointerType : 0) {
			      _LL396: _temp395 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp393)->f1;
				      _LL406: _temp405 = (void *)_temp395.elt_typ;
					      goto _LL404;
				      _LL404: _temp403 = (void *)_temp395.rgn_typ;
					      goto _LL402;
				      _LL402: _temp401 = (struct Cyc_Absyn_Conref *)_temp395.nullable;
					      goto _LL400;
				      _LL400: _temp399 = (struct Cyc_Absyn_Tqual)_temp395.tq;
					      goto _LL398;
				      _LL398: _temp397 = (struct Cyc_Absyn_Conref *)_temp395.bounds;
					      goto _LL390;
			    }
			    else {
			      goto _LL391;
			    }
	    _LL391: goto _LL392;
	    _LL390: if (Cyc_Tcutil_unify(_temp419,_temp405)) {
		      return Cyc_Absyn_sint_t;
		    }
		    goto _LL388;
	    _LL392: goto _LL388;
	    _LL388: ;
	  }
	  Cyc_Tcutil_terr(loc,({ struct _tagged_string _temp421 = Cyc_Absynpp_typ2string(t1);
			    struct _tagged_string _temp422 = Cyc_Absynpp_typ2string(t2);
			    xprintf("comparison not allowed between %.*s and %.*s",
				    _temp421.last_plus_one - _temp421.curr,
				    _temp421.curr,_temp422.last_plus_one - _temp422.curr,
				    _temp422.curr); }));
	  return Cyc_Absyn_wildtyp();
	}
      }
    }
  }
}
static void *Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				   void **topt,void *p,struct Cyc_Absyn_Exp *e1,
				   struct Cyc_Absyn_Exp *e2) {
  void *_temp427 = p;
  _LL429: if (_temp427 == (void *)Cyc_Absyn_Plus) {
    goto _LL430;
  }
  else {
    goto _LL431;
  }
  _LL431: if (_temp427 == (void *)Cyc_Absyn_Minus) {
    goto _LL432;
  }
  else {
    goto _LL433;
  }
  _LL433: if (_temp427 == (void *)Cyc_Absyn_Times) {
    goto _LL434;
  }
  else {
    goto _LL435;
  }
  _LL435: if (_temp427 == (void *)Cyc_Absyn_Div) {
    goto _LL436;
  }
  else {
    goto _LL437;
  }
  _LL437: if (_temp427 == (void *)Cyc_Absyn_Mod) {
    goto _LL438;
  }
  else {
    goto _LL439;
  }
  _LL439: if (_temp427 == (void *)Cyc_Absyn_Bitand) {
    goto _LL440;
  }
  else {
    goto _LL441;
  }
  _LL441: if (_temp427 == (void *)Cyc_Absyn_Bitor) {
    goto _LL442;
  }
  else {
    goto _LL443;
  }
  _LL443: if (_temp427 == (void *)Cyc_Absyn_Bitxor) {
    goto _LL444;
  }
  else {
    goto _LL445;
  }
  _LL445: if (_temp427 == (void *)Cyc_Absyn_Bitlshift) {
    goto _LL446;
  }
  else {
    goto _LL447;
  }
  _LL447: if (_temp427 == (void *)Cyc_Absyn_Bitlrshift) {
    goto _LL448;
  }
  else {
    goto _LL449;
  }
  _LL449: if (_temp427 == (void *)Cyc_Absyn_Bitarshift) {
    goto _LL450;
  }
  else {
    goto _LL451;
  }
  _LL451: if (_temp427 == (void *)Cyc_Absyn_Eq) {
    goto _LL452;
  }
  else {
    goto _LL453;
  }
  _LL453: if (_temp427 == (void *)Cyc_Absyn_Neq) {
    goto _LL454;
  }
  else {
    goto _LL455;
  }
  _LL455: if (_temp427 == (void *)Cyc_Absyn_Gt) {
    goto _LL456;
  }
  else {
    goto _LL457;
  }
  _LL457: if (_temp427 == (void *)Cyc_Absyn_Lt) {
    goto _LL458;
  }
  else {
    goto _LL459;
  }
  _LL459: if (_temp427 == (void *)Cyc_Absyn_Gte) {
    goto _LL460;
  }
  else {
    goto _LL461;
  }
  _LL461: if (_temp427 == (void *)Cyc_Absyn_Lte) {
    goto _LL462;
  }
  else {
    goto _LL463;
  }
  _LL463: goto _LL464;
  _LL430: return Cyc_Tcexp_tcPlus(te,e1,e2);
  _LL432: return Cyc_Tcexp_tcMinus(te,e1,e2);
  _LL434: goto _LL436;
  _LL436: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
  _LL438: goto _LL440;
  _LL440: goto _LL442;
  _LL442: goto _LL444;
  _LL444: goto _LL446;
  _LL446: goto _LL448;
  _LL448: goto _LL450;
  _LL450: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);
  _LL452: goto _LL454;
  _LL454: goto _LL456;
  _LL456: goto _LL458;
  _LL458: goto _LL460;
  _LL460: goto _LL462;
  _LL462: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);
  _LL464: (void)_throw((void *)({ struct Cyc_Core_Failure_struct *_temp465 = (struct Cyc_Core_Failure_struct *)GC_malloc(sizeof(struct Cyc_Core_Failure_struct));
			 _temp465[0] = ({ struct Cyc_Core_Failure_struct _temp466;
			   _temp466.tag = Cyc_Core_Failure;
			   _temp466.f1 = (struct _tagged_string)({ char *_temp467 = (char *)"bad binary primop";
			     struct _tagged_string _temp468;
			     _temp468.curr = _temp467;
			     _temp468.base = _temp467;
			     _temp468.last_plus_one = _temp467 + 18;
			     _temp468; });
			   _temp466; });
			 _temp465; }));
  _LL428: ;
}
static void *Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				void **topt,void *p,struct Cyc_List_List *es) {
  if (p == (void *)Cyc_Absyn_Minus ? ((int (*)(struct Cyc_List_List *x))Cyc_List_length)(es) == 1 : 0) {
    return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp469 = es;
			     if (_temp469 == 0) {
			       _throw(Null_Exception);
			     }
			     _temp469->hd; }));
  }
  Cyc_Tcexp_tcExpList(te,es);
  { void *t;
    { void *_temp470 = p;
      _LL472: if (_temp470 == (void *)Cyc_Absyn_Printf) {
	goto _LL473;
      }
      else {
	goto _LL474;
      }
      _LL474: if (_temp470 == (void *)Cyc_Absyn_Fprintf) {
	goto _LL475;
      }
      else {
	goto _LL476;
      }
      _LL476: if (_temp470 == (void *)Cyc_Absyn_Xprintf) {
	goto _LL477;
      }
      else {
	goto _LL478;
      }
      _LL478: if (_temp470 == (void *)Cyc_Absyn_Scanf) {
	goto _LL479;
      }
      else {
	goto _LL480;
      }
      _LL480: if (_temp470 == (void *)Cyc_Absyn_Fscanf) {
	goto _LL481;
      }
      else {
	goto _LL482;
      }
      _LL482: if (_temp470 == (void *)Cyc_Absyn_Sscanf) {
	goto _LL483;
      }
      else {
	goto _LL484;
      }
      _LL484: goto _LL485;
      _LL473: if (es == 0) {
		return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp486 = (char *)"missing format string in printf";
					    struct _tagged_string _temp487;
					    _temp487.curr = _temp486;
					    _temp487.base = _temp486;
					    _temp487.last_plus_one = _temp486 + 32;
					    _temp487; }));
	      }
	      Cyc_Tcexp_check_format_args(te,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp488 = es;
					    if (_temp488 == 0) {
					      _throw(Null_Exception);
					    }
					    _temp488->hd; }),({ struct Cyc_List_List *_temp489 = es;
					    if (_temp489 == 0) {
					      _throw(Null_Exception);
					    }
					    _temp489->tl; }),Cyc_Formatstr_get_format_typs);
	      t = (void *)Cyc_Absyn_VoidType;
	      goto _LL471;
      _LL475: if (es == 0 ? 1 : ({ struct Cyc_List_List *_temp490 = es;
		if (_temp490 == 0) {
		  _throw(Null_Exception);
		}
		_temp490->tl; }) == 0) {
		return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp491 = (char *)"too few arguments to fprintf";
					    struct _tagged_string _temp492;
					    _temp492.curr = _temp491;
					    _temp492.base = _temp491;
					    _temp492.last_plus_one = _temp491 + 29;
					    _temp492; }));
	      }
	      if (! Cyc_Tcutil_unify((void *)({ struct Cyc_Core_Opt *_temp494 = ((struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp493 = es;
					 if (_temp493 == 0) {
					   _throw(Null_Exception);
					 }
					 _temp493->hd; }))->topt;
				       if (_temp494 == 0) {
					 _throw(Null_Exception);
				       }
				       _temp494->v; }),Cyc_Absyn_file_typ())) {
		Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp495 = es;
				  if (_temp495 == 0) {
				    _throw(Null_Exception);
				  }
				  _temp495->hd; }))->loc,({ struct _tagged_string _temp498 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp497 = ((struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp496 = es;
															if (_temp496 == 0) {
															  _throw(Null_Exception);
															}
															_temp496->hd; }))->topt;
														      if (_temp497 == 0) {
															_throw(Null_Exception);
														      }
														      _temp497->v; }));
				  xprintf("expecting FILE @ but found %.*s",
					  _temp498.last_plus_one - _temp498.curr,
					  _temp498.curr); }));
	      }
	      Cyc_Tcexp_check_format_args(te,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp500 = ({ struct Cyc_List_List *_temp499 = es;
					      if (_temp499 == 0) {
						_throw(Null_Exception);
					      }
					      _temp499->tl; });
					    if (_temp500 == 0) {
					      _throw(Null_Exception);
					    }
					    _temp500->hd; }),({ struct Cyc_List_List *_temp502 = ({ struct Cyc_List_List *_temp501 = es;
					      if (_temp501 == 0) {
						_throw(Null_Exception);
					      }
					      _temp501->tl; });
					    if (_temp502 == 0) {
					      _throw(Null_Exception);
					    }
					    _temp502->tl; }),Cyc_Formatstr_get_format_typs);
	      t = (void *)Cyc_Absyn_VoidType;
	      goto _LL471;
      _LL477: if (es == 0) {
		return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp503 = (char *)"missing format string in xprintf";
					    struct _tagged_string _temp504;
					    _temp504.curr = _temp503;
					    _temp504.base = _temp503;
					    _temp504.last_plus_one = _temp503 + 33;
					    _temp504; }));
	      }
	      Cyc_Tcexp_check_format_args(te,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp505 = es;
					    if (_temp505 == 0) {
					      _throw(Null_Exception);
					    }
					    _temp505->hd; }),({ struct Cyc_List_List *_temp506 = es;
					    if (_temp506 == 0) {
					      _throw(Null_Exception);
					    }
					    _temp506->tl; }),Cyc_Formatstr_get_format_typs);
	      t = Cyc_Absyn_string_typ(Cyc_Absyn_new_evar((void *)Cyc_Absyn_RgnKind));
	      goto _LL471;
      _LL479: if (es == 0) {
		return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp507 = (char *)"missing format string in scanf";
					    struct _tagged_string _temp508;
					    _temp508.curr = _temp507;
					    _temp508.base = _temp507;
					    _temp508.last_plus_one = _temp507 + 31;
					    _temp508; }));
	      }
	      Cyc_Tcexp_check_format_args(te,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp509 = es;
					    if (_temp509 == 0) {
					      _throw(Null_Exception);
					    }
					    _temp509->hd; }),({ struct Cyc_List_List *_temp510 = es;
					    if (_temp510 == 0) {
					      _throw(Null_Exception);
					    }
					    _temp510->tl; }),Cyc_Formatstr_get_scanf_typs);
	      t = Cyc_Absyn_sint_t;
	      goto _LL471;
      _LL481: if (es == 0 ? 1 : ({ struct Cyc_List_List *_temp511 = es;
		if (_temp511 == 0) {
		  _throw(Null_Exception);
		}
		_temp511->tl; }) == 0) {
		return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp512 = (char *)"too few arguments to fscanf";
					    struct _tagged_string _temp513;
					    _temp513.curr = _temp512;
					    _temp513.base = _temp512;
					    _temp513.last_plus_one = _temp512 + 28;
					    _temp513; }));
	      }
	      if (! Cyc_Tcutil_unify((void *)({ struct Cyc_Core_Opt *_temp515 = ((struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp514 = es;
					 if (_temp514 == 0) {
					   _throw(Null_Exception);
					 }
					 _temp514->hd; }))->topt;
				       if (_temp515 == 0) {
					 _throw(Null_Exception);
				       }
				       _temp515->v; }),Cyc_Absyn_file_typ())) {
		Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp516 = es;
				  if (_temp516 == 0) {
				    _throw(Null_Exception);
				  }
				  _temp516->hd; }))->loc,({ struct _tagged_string _temp519 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp518 = ((struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp517 = es;
															if (_temp517 == 0) {
															  _throw(Null_Exception);
															}
															_temp517->hd; }))->topt;
														      if (_temp518 == 0) {
															_throw(Null_Exception);
														      }
														      _temp518->v; }));
				  xprintf("expecting FILE @ but found %.*s",
					  _temp519.last_plus_one - _temp519.curr,
					  _temp519.curr); }));
	      }
	      Cyc_Tcexp_check_format_args(te,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp521 = ({ struct Cyc_List_List *_temp520 = es;
					      if (_temp520 == 0) {
						_throw(Null_Exception);
					      }
					      _temp520->tl; });
					    if (_temp521 == 0) {
					      _throw(Null_Exception);
					    }
					    _temp521->hd; }),({ struct Cyc_List_List *_temp523 = ({ struct Cyc_List_List *_temp522 = es;
					      if (_temp522 == 0) {
						_throw(Null_Exception);
					      }
					      _temp522->tl; });
					    if (_temp523 == 0) {
					      _throw(Null_Exception);
					    }
					    _temp523->tl; }),Cyc_Formatstr_get_scanf_typs);
	      t = Cyc_Absyn_sint_t;
	      goto _LL471;
      _LL483: Cyc_Tcutil_terr(loc,(struct _tagged_string)({ char *_temp524 = (char *)"sscanf is not supported";
				struct _tagged_string _temp525;
				_temp525.curr = _temp524;
				_temp525.base = _temp524;
				_temp525.last_plus_one = _temp524 + 24;
				_temp525; }));
	      t = Cyc_Absyn_sint_t;
	      goto _LL471;
      _LL485: switch (((int (*)(struct Cyc_List_List *x))Cyc_List_length)(es)) {
	      case 0: 
		_LL526: return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp528 = (char *)"primitive operator has 0 arguments";
						    struct _tagged_string _temp529;
						    _temp529.curr = _temp528;
						    _temp529.base = _temp528;
						    _temp529.last_plus_one = _temp528 + 35;
						    _temp529; }));
	      case 1: 
		_LL527: t = Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp531 = es;
						   if (_temp531 == 0) {
						     _throw(Null_Exception);
						   }
						   _temp531->hd; }));
			break;
	      case 2: 
		_LL530: t = Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp533 = es;
						    if (_temp533 == 0) {
						      _throw(Null_Exception);
						    }
						    _temp533->hd; }),(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp535 = ({ struct Cyc_List_List *_temp534 = es;
						      if (_temp534 == 0) {
							_throw(Null_Exception);
						      }
						      _temp534->tl; });
						    if (_temp535 == 0) {
						      _throw(Null_Exception);
						    }
						    _temp535->hd; }));
			break;
	      default: 
		_LL532: return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp537 = (char *)"primitive operator has > 2 arguments";
						    struct _tagged_string _temp538;
						    _temp538.curr = _temp537;
						    _temp538.base = _temp537;
						    _temp538.last_plus_one = _temp537 + 37;
						    _temp538; }));
	      }
	      goto _LL471;
      _LL471: ;
    }
    return t;}
  
}
struct _tuple7 {
  struct Cyc_Absyn_Tqual f1;
  void *f2;
};
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Exp *e) {
  { void *_temp539 = (void *)e->r;
    void *_temp559;
    struct Cyc_Absyn_Vardecl *_temp561;
    struct _tuple1 *_temp563;
    void *_temp565;
    struct Cyc_Absyn_Vardecl *_temp567;
    struct _tuple1 *_temp569;
    void *_temp571;
    struct Cyc_Absyn_Vardecl *_temp573;
    struct _tuple1 *_temp575;
    void *_temp577;
    struct Cyc_Absyn_Vardecl *_temp579;
    struct _tuple1 *_temp581;
    struct Cyc_Absyn_Exp *_temp583;
    struct Cyc_Absyn_Exp *_temp585;
    struct _tagged_string *_temp587;
    struct Cyc_Absyn_Exp *_temp589;
    struct _tagged_string *_temp591;
    struct Cyc_Absyn_Exp *_temp593;
    struct Cyc_Absyn_Exp *_temp595;
    _LL541: if (*((int *)_temp539) == Cyc_Absyn_Var_e) {
      _LL564: _temp563 = (struct _tuple1 *)((struct Cyc_Absyn_Var_e_struct *)_temp539)->f1;
	      goto _LL560;
      _LL560: _temp559 = (void *)((struct Cyc_Absyn_Var_e_struct *)_temp539)->f2;
	      if ((unsigned int)_temp559 > 1 ? *((int *)_temp559) == Cyc_Absyn_Param_b : 0) {
		_LL562: _temp561 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Param_b_struct *)_temp559)->f1;
			goto _LL542;
	      }
	      else {
		goto _LL543;
	      }
    }
    else {
      goto _LL543;
    }
    _LL543: if (*((int *)_temp539) == Cyc_Absyn_Var_e) {
      _LL570: _temp569 = (struct _tuple1 *)((struct Cyc_Absyn_Var_e_struct *)_temp539)->f1;
	      goto _LL566;
      _LL566: _temp565 = (void *)((struct Cyc_Absyn_Var_e_struct *)_temp539)->f2;
	      if ((unsigned int)_temp565 > 1 ? *((int *)_temp565) == Cyc_Absyn_Local_b : 0) {
		_LL568: _temp567 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Local_b_struct *)_temp565)->f1;
			goto _LL544;
	      }
	      else {
		goto _LL545;
	      }
    }
    else {
      goto _LL545;
    }
    _LL545: if (*((int *)_temp539) == Cyc_Absyn_Var_e) {
      _LL576: _temp575 = (struct _tuple1 *)((struct Cyc_Absyn_Var_e_struct *)_temp539)->f1;
	      goto _LL572;
      _LL572: _temp571 = (void *)((struct Cyc_Absyn_Var_e_struct *)_temp539)->f2;
	      if ((unsigned int)_temp571 > 1 ? *((int *)_temp571) == Cyc_Absyn_Pat_b : 0) {
		_LL574: _temp573 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Pat_b_struct *)_temp571)->f1;
			goto _LL546;
	      }
	      else {
		goto _LL547;
	      }
    }
    else {
      goto _LL547;
    }
    _LL547: if (*((int *)_temp539) == Cyc_Absyn_Var_e) {
      _LL582: _temp581 = (struct _tuple1 *)((struct Cyc_Absyn_Var_e_struct *)_temp539)->f1;
	      goto _LL578;
      _LL578: _temp577 = (void *)((struct Cyc_Absyn_Var_e_struct *)_temp539)->f2;
	      if ((unsigned int)_temp577 > 1 ? *((int *)_temp577) == Cyc_Absyn_Global_b : 0) {
		_LL580: _temp579 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Global_b_struct *)_temp577)->f1;
			goto _LL548;
	      }
	      else {
		goto _LL549;
	      }
    }
    else {
      goto _LL549;
    }
    _LL549: if (*((int *)_temp539) == Cyc_Absyn_Subscript_e) {
      _LL586: _temp585 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Subscript_e_struct *)_temp539)->f1;
	      goto _LL584;
      _LL584: _temp583 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Subscript_e_struct *)_temp539)->f2;
	      goto _LL550;
    }
    else {
      goto _LL551;
    }
    _LL551: if (*((int *)_temp539) == Cyc_Absyn_StructMember_e) {
      _LL590: _temp589 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_StructMember_e_struct *)_temp539)->f1;
	      goto _LL588;
      _LL588: _temp587 = (struct _tagged_string *)((struct Cyc_Absyn_StructMember_e_struct *)_temp539)->f2;
	      goto _LL552;
    }
    else {
      goto _LL553;
    }
    _LL553: if (*((int *)_temp539) == Cyc_Absyn_StructArrow_e) {
      _LL594: _temp593 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_StructArrow_e_struct *)_temp539)->f1;
	      goto _LL592;
      _LL592: _temp591 = (struct _tagged_string *)((struct Cyc_Absyn_StructArrow_e_struct *)_temp539)->f2;
	      goto _LL554;
    }
    else {
      goto _LL555;
    }
    _LL555: if (*((int *)_temp539) == Cyc_Absyn_Deref_e) {
      _LL596: _temp595 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Deref_e_struct *)_temp539)->f1;
	      goto _LL556;
    }
    else {
      goto _LL557;
    }
    _LL557: goto _LL558;
    _LL542: _temp567 = _temp561;
	    goto _LL544;
    _LL544: _temp573 = _temp567;
	    goto _LL546;
    _LL546: _temp579 = _temp573;
	    goto _LL548;
    _LL548: if (! (_temp579->tq).q_const) {
	      return;
	    }
	    goto _LL540;
    _LL550: { void *_temp598 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp597 = _temp585->topt;
						     if (_temp597 == 0) {
						       _throw(Null_Exception);
						     }
						     _temp597->v; }));
      struct Cyc_Absyn_PtrInfo _temp608;
      struct Cyc_Absyn_Conref *_temp610;
      struct Cyc_Absyn_Tqual _temp612;
      struct Cyc_Absyn_Conref *_temp614;
      void *_temp616;
      void *_temp618;
      struct Cyc_Absyn_Exp *_temp620;
      struct Cyc_Absyn_Tqual _temp622;
      void *_temp624;
      struct Cyc_List_List *_temp626;
      _LL600: if ((unsigned int)_temp598 > 4 ? *((int *)_temp598) == Cyc_Absyn_PointerType : 0) {
	_LL609: _temp608 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp598)->f1;
		_LL619: _temp618 = (void *)_temp608.elt_typ;
			goto _LL617;
		_LL617: _temp616 = (void *)_temp608.rgn_typ;
			goto _LL615;
		_LL615: _temp614 = (struct Cyc_Absyn_Conref *)_temp608.nullable;
			goto _LL613;
		_LL613: _temp612 = (struct Cyc_Absyn_Tqual)_temp608.tq;
			goto _LL611;
		_LL611: _temp610 = (struct Cyc_Absyn_Conref *)_temp608.bounds;
			goto _LL601;
      }
      else {
	goto _LL602;
      }
      _LL602: if ((unsigned int)_temp598 > 4 ? *((int *)_temp598) == Cyc_Absyn_ArrayType : 0) {
	_LL625: _temp624 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp598)->f1;
		goto _LL623;
	_LL623: _temp622 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp598)->f2;
		goto _LL621;
	_LL621: _temp620 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp598)->f3;
		goto _LL603;
      }
      else {
	goto _LL604;
      }
      _LL604: if ((unsigned int)_temp598 > 4 ? *((int *)_temp598) == Cyc_Absyn_TupleType : 0) {
	_LL627: _temp626 = (struct Cyc_List_List *)((struct Cyc_Absyn_TupleType_struct *)_temp598)->f1;
		goto _LL605;
      }
      else {
	goto _LL606;
      }
      _LL606: goto _LL607;
      _LL601: _temp622 = _temp612;
	      goto _LL603;
      _LL603: if (! _temp622.q_const) {
		return;
	      }
	      goto _LL599;
      _LL605: {
      unsigned int i = Cyc_Evexp_eval_const_uint_exp(_temp583);
	{ struct _handler_cons _temp628;
	  _push_handler(&_temp628);
	  { void *_temp629 = (void *)setjmp(_temp628.handler);
	    if (! _temp629) {
	      { struct _tuple7 _temp632;
		void *_temp633;
		struct Cyc_Absyn_Tqual _temp635;
		struct _tuple7 *_temp630 = ((struct _tuple7 *(*)(struct Cyc_List_List *x,
								 int i))Cyc_List_nth)(_temp626,
										      (int)i);
		_temp632 = *_temp630;
		_LL636: _temp635 = _temp632.f1;
			goto _LL634;
		_LL634: _temp633 = _temp632.f2;
			goto _LL631;
		_LL631: if (! _temp635.q_const) {
		  _npop_handler(0);
		  return;
		}
	      }
	      ;
	      _pop_handler();
	    }
	    else {
	      void *_temp638 = _temp629;
	      _LL640: if (_temp638 == Cyc_List_Nth) {
		goto _LL641;
	      }
	      else {
		goto _LL642;
	      }
	      _LL642: goto _LL643;
	      _LL641: return;
	      _LL643: (void)_throw(_temp638);
	      _LL639: ;
	    }}
	  
	}
	goto _LL599;
      }
      _LL607: goto _LL599;
      _LL599: ;
    }
    goto _LL540;
    _LL552: { void *_temp645 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp644 = _temp589->topt;
						     if (_temp644 == 0) {
						       _throw(Null_Exception);
						     }
						     _temp644->v; }));
      struct Cyc_Absyn_Structdecl **_temp657;
      struct Cyc_List_List *_temp659;
      struct _tuple1 *_temp661;
      struct Cyc_List_List *_temp663;
      struct Cyc_List_List *_temp665;
      struct Cyc_Absyn_Uniondecl **_temp667;
      struct Cyc_List_List *_temp669;
      struct _tuple1 *_temp671;
      _LL647: if ((unsigned int)_temp645 > 4 ? *((int *)_temp645) == Cyc_Absyn_StructType : 0) {
	_LL662: _temp661 = (struct _tuple1 *)((struct Cyc_Absyn_StructType_struct *)_temp645)->f1;
		goto _LL660;
	_LL660: _temp659 = (struct Cyc_List_List *)((struct Cyc_Absyn_StructType_struct *)_temp645)->f2;
		goto _LL658;
	_LL658: _temp657 = (struct Cyc_Absyn_Structdecl **)((struct Cyc_Absyn_StructType_struct *)_temp645)->f3;
		goto _LL648;
      }
      else {
	goto _LL649;
      }
      _LL649: if ((unsigned int)_temp645 > 4 ? *((int *)_temp645) == Cyc_Absyn_AnonUnionType : 0) {
	_LL664: _temp663 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonUnionType_struct *)_temp645)->f1;
		goto _LL650;
      }
      else {
	goto _LL651;
      }
      _LL651: if ((unsigned int)_temp645 > 4 ? *((int *)_temp645) == Cyc_Absyn_AnonStructType : 0) {
	_LL666: _temp665 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonStructType_struct *)_temp645)->f1;
		goto _LL652;
      }
      else {
	goto _LL653;
      }
      _LL653: if ((unsigned int)_temp645 > 4 ? *((int *)_temp645) == Cyc_Absyn_UnionType : 0) {
	_LL672: _temp671 = (struct _tuple1 *)((struct Cyc_Absyn_UnionType_struct *)_temp645)->f1;
		goto _LL670;
	_LL670: _temp669 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnionType_struct *)_temp645)->f2;
		goto _LL668;
	_LL668: _temp667 = (struct Cyc_Absyn_Uniondecl **)((struct Cyc_Absyn_UnionType_struct *)_temp645)->f3;
		goto _LL654;
      }
      else {
	goto _LL655;
      }
      _LL655: goto _LL656;
      _LL648: {
      struct Cyc_Absyn_Structfield *sf = _temp657 == 0 ? 0 : Cyc_Absyn_lookup_struct_field((struct Cyc_Absyn_Structdecl *)({ struct Cyc_Absyn_Structdecl **_temp674 = _temp657;
											     if (_temp674 == 0) {
											       _throw(Null_Exception);
											     }
											     *_temp674; }),
											   _temp587);
	if (sf == 0 ? 1 : ! ({ struct Cyc_Absyn_Structfield *_temp673 = sf;
	  if (_temp673 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp673->tq; }).q_const) {
	  return;
	}
	goto _LL646;
      }
      _LL650: _temp665 = _temp663;
	      goto _LL652;
      _LL652: {
      struct Cyc_Absyn_Structfield *sf = Cyc_Absyn_lookup_field(_temp665,
								_temp587);
	if (sf == 0 ? 1 : ! ({ struct Cyc_Absyn_Structfield *_temp675 = sf;
	  if (_temp675 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp675->tq; }).q_const) {
	  return;
	}
	goto _LL646;
      }
      _LL654: {
      struct Cyc_Absyn_Structfield *sf = _temp667 == 0 ? 0 : Cyc_Absyn_lookup_union_field((struct Cyc_Absyn_Uniondecl *)({ struct Cyc_Absyn_Uniondecl **_temp677 = _temp667;
											    if (_temp677 == 0) {
											      _throw(Null_Exception);
											    }
											    *_temp677; }),
											  _temp587);
	if (sf == 0 ? 1 : ! ({ struct Cyc_Absyn_Structfield *_temp676 = sf;
	  if (_temp676 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp676->tq; }).q_const) {
	  return;
	}
	goto _LL646;
      }
      _LL656: goto _LL646;
      _LL646: ;
    }
    goto _LL540;
    _LL554: { void *_temp679 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp678 = _temp593->topt;
						     if (_temp678 == 0) {
						       _throw(Null_Exception);
						     }
						     _temp678->v; }));
      struct Cyc_Absyn_PtrInfo _temp685;
      struct Cyc_Absyn_Conref *_temp687;
      struct Cyc_Absyn_Tqual _temp689;
      struct Cyc_Absyn_Conref *_temp691;
      void *_temp693;
      void *_temp695;
      _LL681: if ((unsigned int)_temp679 > 4 ? *((int *)_temp679) == Cyc_Absyn_PointerType : 0) {
	_LL686: _temp685 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp679)->f1;
		_LL696: _temp695 = (void *)_temp685.elt_typ;
			goto _LL694;
		_LL694: _temp693 = (void *)_temp685.rgn_typ;
			goto _LL692;
		_LL692: _temp691 = (struct Cyc_Absyn_Conref *)_temp685.nullable;
			goto _LL690;
		_LL690: _temp689 = (struct Cyc_Absyn_Tqual)_temp685.tq;
			goto _LL688;
		_LL688: _temp687 = (struct Cyc_Absyn_Conref *)_temp685.bounds;
			goto _LL682;
      }
      else {
	goto _LL683;
      }
      _LL683: goto _LL684;
      _LL682: { void *_temp697 = Cyc_Tcutil_compress(_temp695);
	struct Cyc_Absyn_Structdecl **_temp709;
	struct Cyc_List_List *_temp711;
	struct _tuple1 *_temp713;
	struct Cyc_Absyn_Uniondecl **_temp715;
	struct Cyc_List_List *_temp717;
	struct _tuple1 *_temp719;
	struct Cyc_List_List *_temp721;
	struct Cyc_List_List *_temp723;
	_LL699: if ((unsigned int)_temp697 > 4 ? *((int *)_temp697) == Cyc_Absyn_StructType : 0) {
	  _LL714: _temp713 = (struct _tuple1 *)((struct Cyc_Absyn_StructType_struct *)_temp697)->f1;
		  goto _LL712;
	  _LL712: _temp711 = (struct Cyc_List_List *)((struct Cyc_Absyn_StructType_struct *)_temp697)->f2;
		  goto _LL710;
	  _LL710: _temp709 = (struct Cyc_Absyn_Structdecl **)((struct Cyc_Absyn_StructType_struct *)_temp697)->f3;
		  goto _LL700;
	}
	else {
	  goto _LL701;
	}
	_LL701: if ((unsigned int)_temp697 > 4 ? *((int *)_temp697) == Cyc_Absyn_UnionType : 0) {
	  _LL720: _temp719 = (struct _tuple1 *)((struct Cyc_Absyn_UnionType_struct *)_temp697)->f1;
		  goto _LL718;
	  _LL718: _temp717 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnionType_struct *)_temp697)->f2;
		  goto _LL716;
	  _LL716: _temp715 = (struct Cyc_Absyn_Uniondecl **)((struct Cyc_Absyn_UnionType_struct *)_temp697)->f3;
		  goto _LL702;
	}
	else {
	  goto _LL703;
	}
	_LL703: if ((unsigned int)_temp697 > 4 ? *((int *)_temp697) == Cyc_Absyn_AnonStructType : 0) {
	  _LL722: _temp721 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonStructType_struct *)_temp697)->f1;
		  goto _LL704;
	}
	else {
	  goto _LL705;
	}
	_LL705: if ((unsigned int)_temp697 > 4 ? *((int *)_temp697) == Cyc_Absyn_AnonUnionType : 0) {
	  _LL724: _temp723 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonUnionType_struct *)_temp697)->f1;
		  goto _LL706;
	}
	else {
	  goto _LL707;
	}
	_LL707: goto _LL708;
	_LL700: {
	struct Cyc_Absyn_Structfield *sf = _temp709 == 0 ? 0 : Cyc_Absyn_lookup_struct_field((struct Cyc_Absyn_Structdecl *)({ struct Cyc_Absyn_Structdecl **_temp726 = _temp709;
											       if (_temp726 == 0) {
												 _throw(Null_Exception);
											       }
											       *_temp726; }),
											     _temp591);
	  if (sf == 0 ? 1 : ! ({ struct Cyc_Absyn_Structfield *_temp725 = sf;
	    if (_temp725 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp725->tq; }).q_const) {
	    return;
	  }
	  goto _LL698;
	}
	_LL702: {
	struct Cyc_Absyn_Structfield *sf = _temp715 == 0 ? 0 : Cyc_Absyn_lookup_union_field((struct Cyc_Absyn_Uniondecl *)({ struct Cyc_Absyn_Uniondecl **_temp728 = _temp715;
											      if (_temp728 == 0) {
												_throw(Null_Exception);
											      }
											      *_temp728; }),
											    _temp591);
	  if (sf == 0 ? 1 : ! ({ struct Cyc_Absyn_Structfield *_temp727 = sf;
	    if (_temp727 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp727->tq; }).q_const) {
	    return;
	  }
	  goto _LL698;
	}
	_LL704: _temp723 = _temp721;
		goto _LL706;
	_LL706: {
	struct Cyc_Absyn_Structfield *sf = Cyc_Absyn_lookup_field(_temp723,
								  _temp591);
	  if (sf == 0 ? 1 : ! ({ struct Cyc_Absyn_Structfield *_temp729 = sf;
	    if (_temp729 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp729->tq; }).q_const) {
	    return;
	  }
	  goto _LL698;
	}
	_LL708: goto _LL698;
	_LL698: ;
      }
      goto _LL680;
      _LL684: goto _LL680;
      _LL680: ;
    }
    goto _LL540;
    _LL556: { void *_temp731 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp730 = _temp595->topt;
						     if (_temp730 == 0) {
						       _throw(Null_Exception);
						     }
						     _temp730->v; }));
      struct Cyc_Absyn_PtrInfo _temp739;
      struct Cyc_Absyn_Conref *_temp741;
      struct Cyc_Absyn_Tqual _temp743;
      struct Cyc_Absyn_Conref *_temp745;
      void *_temp747;
      void *_temp749;
      struct Cyc_Absyn_Exp *_temp751;
      struct Cyc_Absyn_Tqual _temp753;
      void *_temp755;
      _LL733: if ((unsigned int)_temp731 > 4 ? *((int *)_temp731) == Cyc_Absyn_PointerType : 0) {
	_LL740: _temp739 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp731)->f1;
		_LL750: _temp749 = (void *)_temp739.elt_typ;
			goto _LL748;
		_LL748: _temp747 = (void *)_temp739.rgn_typ;
			goto _LL746;
		_LL746: _temp745 = (struct Cyc_Absyn_Conref *)_temp739.nullable;
			goto _LL744;
		_LL744: _temp743 = (struct Cyc_Absyn_Tqual)_temp739.tq;
			goto _LL742;
		_LL742: _temp741 = (struct Cyc_Absyn_Conref *)_temp739.bounds;
			goto _LL734;
      }
      else {
	goto _LL735;
      }
      _LL735: if ((unsigned int)_temp731 > 4 ? *((int *)_temp731) == Cyc_Absyn_ArrayType : 0) {
	_LL756: _temp755 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp731)->f1;
		goto _LL754;
	_LL754: _temp753 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp731)->f2;
		goto _LL752;
	_LL752: _temp751 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp731)->f3;
		goto _LL736;
      }
      else {
	goto _LL737;
      }
      _LL737: goto _LL738;
      _LL734: _temp753 = _temp743;
	      goto _LL736;
      _LL736: if (! _temp753.q_const) {
		return;
	      }
	      goto _LL732;
      _LL738: goto _LL732;
      _LL732: ;
    }
    goto _LL540;
    _LL558: goto _LL540;
    _LL540: ;
  }
  Cyc_Tcutil_terr(e->loc,(struct _tagged_string)({ char *_temp757 = (char *)"attempt to write a read-only location";
		    struct _tagged_string _temp758;
		    _temp758.curr = _temp757;
		    _temp758.base = _temp757;
		    _temp758.last_plus_one = _temp757 + 38;
		    _temp758; }));
}
static void *Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				   void **topt,struct Cyc_Absyn_Exp *e,void *i) {
  Cyc_Tcexp_tcExpNoPromote(te,0,e);
  if (! Cyc_Absyn_is_lvalue(e)) {
    return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp759 = (char *)"increment/decrement of non-lvalue";
				struct _tagged_string _temp760;
				_temp760.curr = _temp759;
				_temp760.base = _temp759;
				_temp760.last_plus_one = _temp759 + 34;
				_temp760; }));
  }
  Cyc_Tcexp_check_writable(te,e);
  { void *t = (void *)({ struct Cyc_Core_Opt *_temp762 = e->topt;
      if (_temp762 == 0) {
	_throw(Null_Exception);
      }
      _temp762->v; });
    if (! Cyc_Tcutil_is_numeric(e) ? ! Cyc_Tcutil_is_tagged_pointer_typ(t) : 0) {
      Cyc_Tcutil_terr(e->loc,({ struct _tagged_string _temp761 = Cyc_Absynpp_typ2string(t);
			xprintf("expecting arithmetic or pointer type but found %.*s",
				_temp761.last_plus_one - _temp761.curr,_temp761.curr); }));
    }
    return t;}
  
}
static void *Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				     void **topt,struct Cyc_Absyn_Exp *e1,
				     struct Cyc_Absyn_Exp *e2,struct Cyc_Absyn_Exp *e3) {
  Cyc_Tcexp_tcTest(te,e1,(struct _tagged_string)({ char *_temp763 = (char *)"conditional expression";
		     struct _tagged_string _temp764;
		     _temp764.curr = _temp763;
		     _temp764.base = _temp763;
		     _temp764.last_plus_one = _temp763 + 23;
		     _temp764; }));
  Cyc_Tcexp_tcExp(te,topt,e2);
  Cyc_Tcexp_tcExp(te,topt,e3);
  { void *t = Cyc_Absyn_new_evar((void *)Cyc_Absyn_MemKind);
    struct Cyc_List_List *l1 = ({ struct Cyc_List_List *_temp770 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
      _temp770->hd = (void *)e3;
      _temp770->tl = 0;
      _temp770; });
    struct Cyc_List_List *l2 = ({ struct Cyc_List_List *_temp769 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
      _temp769->hd = (void *)e2;
      _temp769->tl = l1;
      _temp769; });
    if (! Cyc_Tcutil_coerce_list(te,t,l2)) {
      Cyc_Tcutil_terr(loc,({ struct _tagged_string _temp767 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp765 = e2->topt;
										       if (_temp765 == 0) {
											 _throw(Null_Exception);
										       }
										       _temp765->v; }));
			struct _tagged_string _temp768 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp766 = e3->topt;
										  if (_temp766 == 0) {
										    _throw(Null_Exception);
										  }
										  _temp766->v; }));
			xprintf("conditional clause types do not match: %.*s != %.*s",
				_temp767.last_plus_one - _temp767.curr,_temp767.curr,
				_temp768.last_plus_one - _temp768.curr,_temp768.curr); }));
    }
    return t;}
  
}
static void *Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				  void **topt,struct Cyc_Absyn_Exp *e1,struct Cyc_Core_Opt *po,
				  struct Cyc_Absyn_Exp *e2) {
  Cyc_Tcexp_tcExpNoPromote(te,0,e1);
  Cyc_Tcexp_tcExp(te,(void **)((void *)({ struct Cyc_Core_Opt *_temp771 = e1->topt;
		    if (_temp771 == 0) {
		      _throw(Null_Exception);
		    }
		    &_temp771->v; })),e2);
  { void *t1 = (void *)({ struct Cyc_Core_Opt *_temp794 = e1->topt;
      if (_temp794 == 0) {
	_throw(Null_Exception);
      }
      _temp794->v; });
    void *t2 = (void *)({ struct Cyc_Core_Opt *_temp793 = e2->topt;
      if (_temp793 == 0) {
	_throw(Null_Exception);
      }
      _temp793->v; });
    { void *_temp772 = Cyc_Tcutil_compress(t1);
      struct Cyc_Absyn_Exp *_temp778;
      struct Cyc_Absyn_Tqual _temp780;
      void *_temp782;
      _LL774: if ((unsigned int)_temp772 > 4 ? *((int *)_temp772) == Cyc_Absyn_ArrayType : 0) {
	_LL783: _temp782 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp772)->f1;
		goto _LL781;
	_LL781: _temp780 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp772)->f2;
		goto _LL779;
	_LL779: _temp778 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp772)->f3;
		goto _LL775;
      }
      else {
	goto _LL776;
      }
      _LL776: goto _LL777;
      _LL775: Cyc_Tcutil_terr(loc,(struct _tagged_string)({ char *_temp784 = (char *)"cannot assign to an array";
				struct _tagged_string _temp785;
				_temp785.curr = _temp784;
				_temp785.base = _temp784;
				_temp785.last_plus_one = _temp784 + 26;
				_temp785; }));
	      goto _LL773;
      _LL777: goto _LL773;
      _LL773: ;
    }
    if (! Cyc_Absyn_is_lvalue(e1)) {
      return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp786 = (char *)"assignment to non-lvalue";
				  struct _tagged_string _temp787;
				  _temp787.curr = _temp786;
				  _temp787.base = _temp786;
				  _temp787.last_plus_one = _temp786 + 25;
				  _temp787; }));
    }
    Cyc_Tcexp_check_writable(te,e1);
    if (po == 0) {
      if (! Cyc_Tcutil_coerce_assign(te,e2,t1)) {
	return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp788 = Cyc_Absynpp_typ2string(t1);
				    struct _tagged_string _temp789 = Cyc_Absynpp_typ2string(t2);
				    xprintf("type mismatch: %.*s != %.*s",
					    _temp788.last_plus_one - _temp788.curr,
					    _temp788.curr,_temp789.last_plus_one - _temp789.curr,
					    _temp789.curr); }));
      }
    }
    else {
      void *p = (void *)({ struct Cyc_Core_Opt *_temp792 = po;
	if (_temp792 == 0) {
	  _throw(Null_Exception);
	}
	_temp792->v; });
      void *t_result = Cyc_Tcexp_tcBinPrimop(te,loc,0,p,e1,e2);
      if (! (Cyc_Tcutil_unify(t_result,t1) ? 1 : Cyc_Tcutil_coerceable(t_result))) {
	return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp790 = (char *)"Cannot use this operator in an assignment";
				    struct _tagged_string _temp791;
				    _temp791.curr = _temp790;
				    _temp791.base = _temp790;
				    _temp791.last_plus_one = _temp790 + 42;
				    _temp791; }));
      }
    }
    return t2;}
  
}
static void *Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				void **topt,struct Cyc_Absyn_Exp *e1,struct Cyc_Absyn_Exp *e2) {
  Cyc_Tcexp_tcExp(te,0,e1);
  Cyc_Tcexp_tcExp(te,topt,e2);
  return (void *)({ struct Cyc_Core_Opt *_temp795 = e2->topt;
    if (_temp795 == 0) {
      _throw(Null_Exception);
    }
    _temp795->v; });
}
static void *Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				void **topt,struct Cyc_Absyn_Exp *e,struct Cyc_List_List *es) {
  Cyc_Tcexp_tcExp(te,0,e);
  { void *t = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp853 = e->topt;
				    if (_temp853 == 0) {
				      _throw(Null_Exception);
				    }
				    _temp853->v; }));
    void *_temp796 = t;
    struct Cyc_Absyn_PtrInfo _temp802;
    struct Cyc_Absyn_Conref *_temp804;
    struct Cyc_Absyn_Tqual _temp806;
    struct Cyc_Absyn_Conref *_temp808;
    void *_temp810;
    void *_temp812;
    _LL798: if ((unsigned int)_temp796 > 4 ? *((int *)_temp796) == Cyc_Absyn_PointerType : 0) {
      _LL803: _temp802 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp796)->f1;
	      _LL813: _temp812 = (void *)_temp802.elt_typ;
		      goto _LL811;
	      _LL811: _temp810 = (void *)_temp802.rgn_typ;
		      goto _LL809;
	      _LL809: _temp808 = (struct Cyc_Absyn_Conref *)_temp802.nullable;
		      goto _LL807;
	      _LL807: _temp806 = (struct Cyc_Absyn_Tqual)_temp802.tq;
		      goto _LL805;
	      _LL805: _temp804 = (struct Cyc_Absyn_Conref *)_temp802.bounds;
		      goto _LL799;
    }
    else {
      goto _LL800;
    }
    _LL800: goto _LL801;
    _LL799: Cyc_Tcenv_check_rgn_accessible(te,loc,_temp810);
	    Cyc_Tcutil_check_nonzero_bound(loc,_temp804);
	    { void *_temp814 = Cyc_Tcutil_compress(_temp812);
	      struct Cyc_Absyn_FnInfo _temp820;
	      struct Cyc_List_List *_temp822;
	      int _temp824;
	      struct Cyc_List_List *_temp826;
	      void *_temp828;
	      struct Cyc_Core_Opt *_temp830;
	      struct Cyc_List_List *_temp832;
	      _LL816: if ((unsigned int)_temp814 > 4 ? *((int *)_temp814) == Cyc_Absyn_FnType : 0) {
		_LL821: _temp820 = (struct Cyc_Absyn_FnInfo)((struct Cyc_Absyn_FnType_struct *)_temp814)->f1;
			_LL833: _temp832 = (struct Cyc_List_List *)_temp820.tvars;
				goto _LL831;
			_LL831: _temp830 = (struct Cyc_Core_Opt *)_temp820.effect;
				goto _LL829;
			_LL829: _temp828 = (void *)_temp820.ret_typ;
				goto _LL827;
			_LL827: _temp826 = (struct Cyc_List_List *)_temp820.args;
				goto _LL825;
			_LL825: _temp824 = (int)_temp820.varargs;
				goto _LL823;
			_LL823: _temp822 = (struct Cyc_List_List *)_temp820.attributes;
				goto _LL817;
	      }
	      else {
		goto _LL818;
	      }
	      _LL818: goto _LL819;
	      _LL817: if (_temp824) {
			Cyc_Tcutil_terr(loc,(struct _tagged_string)({ char *_temp834 = (char *)"vararg functions (...) not supported yet";
					  struct _tagged_string _temp835;
					  _temp835.curr = _temp834;
					  _temp835.base = _temp834;
					  _temp835.last_plus_one = _temp834 + 41;
					  _temp835; }));
		      }
		      if (topt != 0) {
			Cyc_Tcutil_unify(_temp828,({ void **_temp836 = topt;
					   if (_temp836 == 0) {
					     _throw(Null_Exception);
					   }
					   *_temp836; }));
		      }
		      while (es != 0 ? _temp826 != 0 : 0) {
			struct Cyc_Absyn_Exp *e1 = (struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp843 = es;
			  if (_temp843 == 0) {
			    _throw(Null_Exception);
			  }
			  _temp843->hd; });
			void *t2 = (*((struct _tuple2 *)({ struct Cyc_List_List *_temp842 = _temp826;
			  if (_temp842 == 0) {
			    _throw(Null_Exception);
			  }
			  _temp842->hd; }))).f3;
			Cyc_Tcexp_tcExp(te,(void **)&t2,e1);
			if (! Cyc_Tcutil_coerce_arg(te,e1,t2)) {
			  Cyc_Tcutil_terr(loc,({ struct _tagged_string _temp838 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp837 = e1->topt;
													   if (_temp837 == 0) {
													     _throw(Null_Exception);
													   }
													   _temp837->v; }));
					    struct _tagged_string _temp839 = Cyc_Absynpp_typ2string(t2);
					    xprintf("actual argument has type %.*s but formal has type %.*s",
						    _temp838.last_plus_one - _temp838.curr,
						    _temp838.curr,_temp839.last_plus_one - _temp839.curr,
						    _temp839.curr); }));
			}
			es = ({ struct Cyc_List_List *_temp840 = es;
			  if (_temp840 == 0) {
			    _throw(Null_Exception);
			  }
			  _temp840->tl; });
			_temp826 = ({ struct Cyc_List_List *_temp841 = _temp826;
			  if (_temp841 == 0) {
			    _throw(Null_Exception);
			  }
			  _temp841->tl; });
		      }
		      if (es != 0) {
			Cyc_Tcutil_terr(loc,(struct _tagged_string)({ char *_temp844 = (char *)"too many arguments for function";
					  struct _tagged_string _temp845;
					  _temp845.curr = _temp844;
					  _temp845.base = _temp844;
					  _temp845.last_plus_one = _temp844 + 32;
					  _temp845; }));
		      }
		      if (_temp826 != 0) {
			Cyc_Tcutil_terr(loc,(struct _tagged_string)({ char *_temp846 = (char *)"too few arguments for function";
					  struct _tagged_string _temp847;
					  _temp847.curr = _temp846;
					  _temp847.base = _temp846;
					  _temp847.last_plus_one = _temp846 + 31;
					  _temp847; }));
		      }
		      Cyc_Tcenv_check_effect_accessible(te,loc,(void *)({ struct Cyc_Core_Opt *_temp848 = _temp830;
							  if (_temp848 == 0) {
							    _throw(Null_Exception);
							  }
							  _temp848->v; }));
		      return _temp828;
	      _LL819: return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp849 = (char *)"expected pointer to function";
						  struct _tagged_string _temp850;
						  _temp850.curr = _temp849;
						  _temp850.base = _temp849;
						  _temp850.last_plus_one = _temp849 + 29;
						  _temp850; }));
	      _LL815: ;}
	    
    _LL801: return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp851 = (char *)"expected pointer to function";
					struct _tagged_string _temp852;
					_temp852.curr = _temp851;
					_temp852.base = _temp851;
					_temp852.last_plus_one = _temp851 + 29;
					_temp852; }));
    _LL797: ;}
  
}
static void *Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
			       void **topt,struct Cyc_Absyn_Exp *e) {
  Cyc_Tcexp_tcExp(te,(void **)&Cyc_Absyn_exn_typ,e);
  if (! Cyc_Tcutil_coerce_use(te,e,Cyc_Absyn_exn_typ)) {
    Cyc_Tcutil_terr(loc,({ struct _tagged_string _temp855 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp854 = e->topt;
										     if (_temp854 == 0) {
										       _throw(Null_Exception);
										     }
										     _temp854->v; }));
		      xprintf("expected xtunion exn but found %.*s",_temp855.last_plus_one - _temp855.curr,
			      _temp855.curr); }));
  }
  return Cyc_Absyn_wildtyp();
}
static void *Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				     void **topt,struct Cyc_Absyn_Exp *e,
				     struct Cyc_List_List *ts) {
  Cyc_Tcexp_tcExpNoInst(te,0,e);
  (void *)({ struct Cyc_Core_Opt *_temp856 = e->topt;
    if (_temp856 == 0) {
      _throw(Null_Exception);
    }
    _temp856->v = (void *)Cyc_Absyn_pointer_expand(Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp857 = e->topt;
									 if (_temp857 == 0) {
									   _throw(Null_Exception);
									 }
									 _temp857->v; }))); });
  { void *t1 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp913 = e->topt;
				     if (_temp913 == 0) {
				       _throw(Null_Exception);
				     }
				     _temp913->v; }));
    { void *_temp858 = t1;
      struct Cyc_Absyn_PtrInfo _temp864;
      struct Cyc_Absyn_Conref *_temp866;
      struct Cyc_Absyn_Tqual _temp868;
      struct Cyc_Absyn_Conref *_temp870;
      void *_temp872;
      void *_temp874;
      _LL860: if ((unsigned int)_temp858 > 4 ? *((int *)_temp858) == Cyc_Absyn_PointerType : 0) {
	_LL865: _temp864 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp858)->f1;
		_LL875: _temp874 = (void *)_temp864.elt_typ;
			goto _LL873;
		_LL873: _temp872 = (void *)_temp864.rgn_typ;
			goto _LL871;
		_LL871: _temp870 = (struct Cyc_Absyn_Conref *)_temp864.nullable;
			goto _LL869;
		_LL869: _temp868 = (struct Cyc_Absyn_Tqual)_temp864.tq;
			goto _LL867;
		_LL867: _temp866 = (struct Cyc_Absyn_Conref *)_temp864.bounds;
			goto _LL861;
      }
      else {
	goto _LL862;
      }
      _LL862: goto _LL863;
      _LL861: { void *_temp876 = Cyc_Tcutil_compress(_temp874);
	struct Cyc_Absyn_FnInfo _temp882;
	struct Cyc_List_List *_temp884;
	int _temp886;
	struct Cyc_List_List *_temp888;
	void *_temp890;
	struct Cyc_Core_Opt *_temp892;
	struct Cyc_List_List *_temp894;
	_LL878: if ((unsigned int)_temp876 > 4 ? *((int *)_temp876) == Cyc_Absyn_FnType : 0) {
	  _LL883: _temp882 = (struct Cyc_Absyn_FnInfo)((struct Cyc_Absyn_FnType_struct *)_temp876)->f1;
		  _LL895: _temp894 = (struct Cyc_List_List *)_temp882.tvars;
			  goto _LL893;
		  _LL893: _temp892 = (struct Cyc_Core_Opt *)_temp882.effect;
			  goto _LL891;
		  _LL891: _temp890 = (void *)_temp882.ret_typ;
			  goto _LL889;
		  _LL889: _temp888 = (struct Cyc_List_List *)_temp882.args;
			  goto _LL887;
		  _LL887: _temp886 = (int)_temp882.varargs;
			  goto _LL885;
		  _LL885: _temp884 = (struct Cyc_List_List *)_temp882.attributes;
			  goto _LL879;
	}
	else {
	  goto _LL880;
	}
	_LL880: goto _LL881;
	_LL879: {
	struct Cyc_List_List *instantiation = 0;
	  if (((int (*)(struct Cyc_List_List *x))Cyc_List_length)(ts) != ((int (*)(struct Cyc_List_List *x))Cyc_List_length)(_temp894)) {
	    return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp896 = (char *)"type instantiation mismatch";
					struct _tagged_string _temp897;
					_temp897.curr = _temp896;
					_temp897.base = _temp896;
					_temp897.last_plus_one = _temp896 + 28;
					_temp897; }));
	  }
	  for(0; ts != 0; ts = ({ struct Cyc_List_List *_temp898 = ts;
	    if (_temp898 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp898->tl; }), _temp894 = ({ struct Cyc_List_List *_temp899 = _temp894;
	    if (_temp899 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp899->tl; })) {
	    void *k = ((void *(*)(struct Cyc_Absyn_Conref *x))Cyc_Absyn_conref_val)(((struct Cyc_Absyn_Tvar *)({ struct Cyc_List_List *_temp905 = _temp894;
										      if (_temp905 == 0) {
											_throw(Null_Exception);
										      }
										      _temp905->hd; }))->kind);
	    Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,
				  (void *)({ struct Cyc_List_List *_temp900 = ts;
				    if (_temp900 == 0) {
				      _throw(Null_Exception);
				    }
				    _temp900->hd; }));
	    instantiation = ({ struct Cyc_List_List *_temp901 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
	      _temp901->hd = (void *)({ struct _tuple4 *_temp902 = (struct _tuple4 *)GC_malloc(sizeof(struct _tuple4));
		_temp902->f1 = (struct Cyc_Absyn_Tvar *)({ struct Cyc_List_List *_temp904 = _temp894;
		  if (_temp904 == 0) {
		    _throw(Null_Exception);
		  }
		  _temp904->hd; });
		_temp902->f2 = (void *)({ struct Cyc_List_List *_temp903 = ts;
		  if (_temp903 == 0) {
		    _throw(Null_Exception);
		  }
		  _temp903->hd; });
		_temp902; });
	      _temp901->tl = instantiation;
	      _temp901; });
	  }
	  { void *new_fn_typ = Cyc_Tcutil_substitute(instantiation,(void *)({ struct Cyc_Absyn_FnType_struct *_temp909 = (struct Cyc_Absyn_FnType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_FnType_struct));
						       _temp909[0] = ({ struct Cyc_Absyn_FnType_struct _temp910;
							 _temp910.tag = Cyc_Absyn_FnType;
							 _temp910.f1 = ({ struct Cyc_Absyn_FnInfo _temp911;
							   _temp911.tvars = 0;
							   _temp911.effect = _temp892;
							   _temp911.ret_typ = (void *)_temp890;
							   _temp911.args = _temp888;
							   _temp911.varargs = _temp886;
							   _temp911.attributes = _temp884;
							   _temp911; });
							 _temp910; });
						       _temp909; }));
	    void *new_typ = (void *)({ struct Cyc_Absyn_PointerType_struct *_temp906 = (struct Cyc_Absyn_PointerType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_PointerType_struct));
	      _temp906[0] = ({ struct Cyc_Absyn_PointerType_struct _temp907;
		_temp907.tag = Cyc_Absyn_PointerType;
		_temp907.f1 = ({ struct Cyc_Absyn_PtrInfo _temp908;
		  _temp908.elt_typ = (void *)new_fn_typ;
		  _temp908.rgn_typ = (void *)_temp872;
		  _temp908.nullable = _temp870;
		  _temp908.tq = _temp868;
		  _temp908.bounds = _temp866;
		  _temp908; });
		_temp907; });
	      _temp906; });
	    return new_typ;}
	  
	}
	_LL881: goto _LL877;
	_LL877: ;
      }
      goto _LL859;
      _LL863: goto _LL859;
      _LL859: ;
    }
    return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp912 = Cyc_Absynpp_typ2string(t1);
				xprintf("expecting polymorphic type but found %.*s",
					_temp912.last_plus_one - _temp912.curr,
					_temp912.curr); }));}
  
}
static void *Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
			      void **topt,void *t,struct Cyc_Absyn_Exp *e) {
  Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void *)Cyc_Absyn_MemKind,
			t);
  Cyc_Tcexp_tcExp(te,0,e);
  { void *t2 = (void *)({ struct Cyc_Core_Opt *_temp916 = e->topt;
      if (_temp916 == 0) {
	_throw(Null_Exception);
      }
      _temp916->v; });
    if (! Cyc_Tcutil_silent_castable(te,loc,t2,t) ? ! Cyc_Tcutil_castable(te,
									  loc,
									  t2,
									  t) : 0) {
      return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp914 = Cyc_Absynpp_typ2string(t2);
				  struct _tagged_string _temp915 = Cyc_Absynpp_typ2string(t);
				  xprintf("cannot cast %.*s to %.*s",_temp914.last_plus_one - _temp914.curr,
					  _temp914.curr,_temp915.last_plus_one - _temp915.curr,
					  _temp915.curr); }));
    }
    return t;}
  
}
static void *Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				 void **topt,struct Cyc_Absyn_Exp *e) {
  void **topt2 = 0;
  struct Cyc_Absyn_Tqual tq2 = Cyc_Absyn_empty_tqual();
  if (topt != 0) {
    void *_temp918 = Cyc_Tcutil_compress(({ void **_temp917 = topt;
					   if (_temp917 == 0) {
					     _throw(Null_Exception);
					   }
					   *_temp917; }));
    struct Cyc_Absyn_PtrInfo _temp924;
    struct Cyc_Absyn_Conref *_temp926;
    struct Cyc_Absyn_Tqual _temp928;
    struct Cyc_Absyn_Conref *_temp930;
    void *_temp932;
    void *_temp934;
    _LL920: if ((unsigned int)_temp918 > 4 ? *((int *)_temp918) == Cyc_Absyn_PointerType : 0) {
      _LL925: _temp924 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp918)->f1;
	      _LL935: _temp934 = (void *)_temp924.elt_typ;
		      goto _LL933;
	      _LL933: _temp932 = (void *)_temp924.rgn_typ;
		      goto _LL931;
	      _LL931: _temp930 = (struct Cyc_Absyn_Conref *)_temp924.nullable;
		      goto _LL929;
	      _LL929: _temp928 = (struct Cyc_Absyn_Tqual)_temp924.tq;
		      goto _LL927;
	      _LL927: _temp926 = (struct Cyc_Absyn_Conref *)_temp924.bounds;
		      goto _LL921;
    }
    else {
      goto _LL922;
    }
    _LL922: goto _LL923;
    _LL921: topt2 = ({ void **_temp936 = (void **)GC_malloc(sizeof(void *));
	      _temp936[0] = _temp934;
	      _temp936; });
	    tq2 = _temp928;
	    goto _LL919;
    _LL923: goto _LL919;
    _LL919: ;
  }
  Cyc_Tcexp_tcExpNoInst(te,topt2,e);
  { void *_temp937 = (void *)e->r;
    struct Cyc_Absyn_Structdecl *_temp945;
    struct Cyc_List_List *_temp947;
    struct Cyc_Core_Opt *_temp949;
    struct _tuple1 *_temp951;
    struct Cyc_List_List *_temp953;
    _LL939: if (*((int *)_temp937) == Cyc_Absyn_Struct_e) {
      _LL952: _temp951 = (struct _tuple1 *)((struct Cyc_Absyn_Struct_e_struct *)_temp937)->f1;
	      goto _LL950;
      _LL950: _temp949 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Struct_e_struct *)_temp937)->f2;
	      goto _LL948;
      _LL948: _temp947 = (struct Cyc_List_List *)((struct Cyc_Absyn_Struct_e_struct *)_temp937)->f3;
	      goto _LL946;
      _LL946: _temp945 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Absyn_Struct_e_struct *)_temp937)->f4;
	      goto _LL940;
    }
    else {
      goto _LL941;
    }
    _LL941: if (*((int *)_temp937) == Cyc_Absyn_Tuple_e) {
      _LL954: _temp953 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tuple_e_struct *)_temp937)->f1;
	      goto _LL942;
    }
    else {
      goto _LL943;
    }
    _LL943: goto _LL944;
    _LL940: goto _LL942;
    _LL942: Cyc_Tcutil_warn(loc,(struct _tagged_string)({ char *_temp955 = (char *)"& used to allocate";
			      struct _tagged_string _temp956;
			      _temp956.curr = _temp955;
			      _temp956.base = _temp955;
			      _temp956.last_plus_one = _temp955 + 19;
			      _temp956; }));
    { void *(*ptr_maker)(void *t,void *rgn,struct Cyc_Absyn_Tqual tq) = Cyc_Absyn_at_typ;
      if (topt != 0) {
	void *_temp958 = Cyc_Tcutil_compress(({ void **_temp957 = topt;
					       if (_temp957 == 0) {
						 _throw(Null_Exception);
					       }
					       *_temp957; }));
	struct Cyc_Absyn_PtrInfo _temp964;
	struct Cyc_Absyn_Conref *_temp966;
	struct Cyc_Absyn_Tqual _temp968;
	struct Cyc_Absyn_Conref *_temp970;
	void *_temp972;
	void *_temp974;
	_LL960: if ((unsigned int)_temp958 > 4 ? *((int *)_temp958) == Cyc_Absyn_PointerType : 0) {
	  _LL965: _temp964 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp958)->f1;
		  _LL975: _temp974 = (void *)_temp964.elt_typ;
			  goto _LL973;
		  _LL973: _temp972 = (void *)_temp964.rgn_typ;
			  goto _LL971;
		  _LL971: _temp970 = (struct Cyc_Absyn_Conref *)_temp964.nullable;
			  goto _LL969;
		  _LL969: _temp968 = (struct Cyc_Absyn_Tqual)_temp964.tq;
			  goto _LL967;
		  _LL967: _temp966 = (struct Cyc_Absyn_Conref *)_temp964.bounds;
			  goto _LL961;
	}
	else {
	  goto _LL962;
	}
	_LL962: goto _LL963;
	_LL961: if (((int (*)(struct Cyc_Absyn_Conref *x))Cyc_Absyn_conref_val)(_temp970)) {
		  ptr_maker = Cyc_Absyn_star_typ;
		}
		goto _LL959;
	_LL963: goto _LL959;
	_LL959: ;
      }
      return ptr_maker((void *)({ struct Cyc_Core_Opt *_temp976 = e->topt;
			 if (_temp976 == 0) {
			   _throw(Null_Exception);
			 }
			 _temp976->v; }),(void *)Cyc_Absyn_HeapRgn,tq2);}
    
    _LL944: goto _LL938;
    _LL938: ;
  }
  { void *_temp979;
    int _temp981;
    struct _tuple5 _temp977 = Cyc_Tcutil_addressof_props(te,e);
    _LL982: _temp981 = _temp977.f1;
	    goto _LL980;
    _LL980: _temp979 = _temp977.f2;
	    goto _LL978;
    _LL978: {
    struct Cyc_Absyn_Tqual tq = Cyc_Absyn_empty_tqual();
      if (_temp981) {
	tq.q_const = 1;
      }
      { void *t = Cyc_Absyn_at_typ((void *)({ struct Cyc_Core_Opt *_temp983 = e->topt;
				     if (_temp983 == 0) {
				       _throw(Null_Exception);
				     }
				     _temp983->v; }),_temp979,tq);
	return t;}
      
    }}
  
}
static void *Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				void **topt,void *t) {
  Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void *)Cyc_Absyn_MemKind,
			t);
  Cyc_Evexp_szof(t,loc);
  return Cyc_Absyn_uint_t;
}
static void *Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
			       void **topt,struct Cyc_Absyn_Exp *e) {
  Cyc_Tcexp_tcExp(te,0,e);
  { void *t = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp1003 = e->topt;
				    if (_temp1003 == 0) {
				      _throw(Null_Exception);
				    }
				    _temp1003->v; }));
    void *_temp984 = t;
    struct Cyc_Absyn_PtrInfo _temp990;
    struct Cyc_Absyn_Conref *_temp992;
    struct Cyc_Absyn_Tqual _temp994;
    struct Cyc_Absyn_Conref *_temp996;
    void *_temp998;
    void *_temp1000;
    _LL986: if ((unsigned int)_temp984 > 4 ? *((int *)_temp984) == Cyc_Absyn_PointerType : 0) {
      _LL991: _temp990 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp984)->f1;
	      _LL1001: _temp1000 = (void *)_temp990.elt_typ;
		       goto _LL999;
	      _LL999: _temp998 = (void *)_temp990.rgn_typ;
		      goto _LL997;
	      _LL997: _temp996 = (struct Cyc_Absyn_Conref *)_temp990.nullable;
		      goto _LL995;
	      _LL995: _temp994 = (struct Cyc_Absyn_Tqual)_temp990.tq;
		      goto _LL993;
	      _LL993: _temp992 = (struct Cyc_Absyn_Conref *)_temp990.bounds;
		      goto _LL987;
    }
    else {
      goto _LL988;
    }
    _LL988: goto _LL989;
    _LL987: Cyc_Tcenv_check_rgn_accessible(te,loc,_temp998);
	    Cyc_Tcutil_check_nonzero_bound(loc,_temp992);
	    return _temp1000;
    _LL989: return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1002 = Cyc_Absynpp_typ2string(t);
					xprintf("expecting * or @ type but found %.*s",
						_temp1002.last_plus_one - _temp1002.curr,
						_temp1002.curr); }));
    _LL985: ;}
  
}
static void *Cyc_Tcexp_tcStructMember(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				      void **topt,struct Cyc_Absyn_Exp *outer_e,
				      struct Cyc_Absyn_Exp *e,struct _tagged_string *f) {
  Cyc_Tcexp_tcExpNoPromote(te,0,e);
  { void *t = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp1079 = e->topt;
				    if (_temp1079 == 0) {
				      _throw(Null_Exception);
				    }
				    _temp1079->v; }));
    void *_temp1004 = t;
    struct Cyc_Absyn_Structdecl **_temp1020;
    struct Cyc_Absyn_Structdecl *_temp1022;
    struct Cyc_List_List *_temp1023;
    struct _tuple1 *_temp1025;
    struct Cyc_List_List *_temp1027;
    struct Cyc_List_List *_temp1029;
    struct Cyc_Absyn_Uniondecl **_temp1031;
    struct Cyc_Absyn_Uniondecl *_temp1033;
    struct Cyc_List_List *_temp1034;
    struct _tuple1 *_temp1036;
    struct Cyc_Absyn_Exp *_temp1039;
    struct Cyc_Absyn_Tqual _temp1041;
    void *_temp1043;
    struct Cyc_Absyn_PtrInfo _temp1048;
    _LL1006: if ((unsigned int)_temp1004 > 4 ? *((int *)_temp1004) == Cyc_Absyn_StructType : 0) {
      _LL1026: _temp1025 = (struct _tuple1 *)((struct Cyc_Absyn_StructType_struct *)_temp1004)->f1;
	       goto _LL1024;
      _LL1024: _temp1023 = (struct Cyc_List_List *)((struct Cyc_Absyn_StructType_struct *)_temp1004)->f2;
	       goto _LL1021;
      _LL1021: _temp1020 = (struct Cyc_Absyn_Structdecl **)((struct Cyc_Absyn_StructType_struct *)_temp1004)->f3;
	       if (_temp1020 == 0) {
		 goto _LL1008;
	       }
	       else {
		 _temp1022 = *_temp1020;
		 goto _LL1007;
	       }
    }
    else {
      goto _LL1008;
    }
    _LL1008: if ((unsigned int)_temp1004 > 4 ? *((int *)_temp1004) == Cyc_Absyn_AnonStructType : 0) {
      _LL1028: _temp1027 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonStructType_struct *)_temp1004)->f1;
	       goto _LL1009;
    }
    else {
      goto _LL1010;
    }
    _LL1010: if ((unsigned int)_temp1004 > 4 ? *((int *)_temp1004) == Cyc_Absyn_AnonUnionType : 0) {
      _LL1030: _temp1029 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonUnionType_struct *)_temp1004)->f1;
	       goto _LL1011;
    }
    else {
      goto _LL1012;
    }
    _LL1012: if ((unsigned int)_temp1004 > 4 ? *((int *)_temp1004) == Cyc_Absyn_UnionType : 0) {
      _LL1037: _temp1036 = (struct _tuple1 *)((struct Cyc_Absyn_UnionType_struct *)_temp1004)->f1;
	       goto _LL1035;
      _LL1035: _temp1034 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnionType_struct *)_temp1004)->f2;
	       goto _LL1032;
      _LL1032: _temp1031 = (struct Cyc_Absyn_Uniondecl **)((struct Cyc_Absyn_UnionType_struct *)_temp1004)->f3;
	       if (_temp1031 == 0) {
		 goto _LL1014;
	       }
	       else {
		 _temp1033 = *_temp1031;
		 goto _LL1013;
	       }
    }
    else {
      goto _LL1014;
    }
    _LL1014: if ((unsigned int)_temp1004 > 4 ? *((int *)_temp1004) == Cyc_Absyn_ArrayType : 0) {
	       _LL1044: _temp1043 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp1004)->f1;
			goto _LL1042;
	       _LL1042: _temp1041 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp1004)->f2;
			goto _LL1040;
	       _LL1040: _temp1039 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp1004)->f3;
			goto _LL1038;
	     }
	     else {
	       goto _LL1016;
	     }
	     _LL1038: if (Cyc_String_zstrcmp(*f,(struct _tagged_string)({ char *_temp1045 = (char *)"size";
					       struct _tagged_string _temp1046;
					       _temp1046.curr = _temp1045;
					       _temp1046.base = _temp1045;
					       _temp1046.last_plus_one = _temp1045 + 5;
					       _temp1046; })) == 0) {
	       goto _LL1015;
	     }
	     else {
	       goto _LL1016;
	     }
    _LL1016: if ((unsigned int)_temp1004 > 4 ? *((int *)_temp1004) == Cyc_Absyn_PointerType : 0) {
	       _LL1049: _temp1048 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp1004)->f1;
			goto _LL1047;
	     }
	     else {
	       goto _LL1018;
	     }
	     _LL1047: if (Cyc_String_zstrcmp(*f,(struct _tagged_string)({ char *_temp1050 = (char *)"size";
					       struct _tagged_string _temp1051;
					       _temp1051.curr = _temp1050;
					       _temp1051.base = _temp1050;
					       _temp1051.last_plus_one = _temp1050 + 5;
					       _temp1051; })) == 0) {
	       goto _LL1017;
	     }
	     else {
	       goto _LL1018;
	     }
    _LL1018: goto _LL1019;
    _LL1007: if (_temp1025 == 0) {
      return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp1052 = (char *)"unresolved struct type (COMPILER ERROR)";
				  struct _tagged_string _temp1053;
				  _temp1053.curr = _temp1052;
				  _temp1053.base = _temp1052;
				  _temp1053.last_plus_one = _temp1052 + 40;
				  _temp1053; }));
    }
    { struct Cyc_Absyn_Structfield *finfo = Cyc_Absyn_lookup_struct_field(_temp1022,
									  f);
      if (finfo == 0) {
	return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1055 = Cyc_Absynpp_qvar2string((struct _tuple1 *)({ struct _tuple1 *_temp1054 = _temp1025;
													if (_temp1054 == 0) {
													  _throw(Null_Exception);
													}
													_temp1054; }));
				    struct _tagged_string _temp1056 = *f;
				    xprintf("struct %.*s has no %.*s field",
					    _temp1055.last_plus_one - _temp1055.curr,
					    _temp1055.curr,_temp1056.last_plus_one - _temp1056.curr,
					    _temp1056.curr); }));
      }
      { void *t2;
	{ struct _RegionHandle _temp1057 = _new_region();
	  struct _RegionHandle *rgn = &_temp1057;
	  _push_region(rgn);
	  { struct Cyc_List_List *inst = ((struct Cyc_List_List *(*)(struct _RegionHandle *r3,
								     struct _RegionHandle *r4,
								     struct Cyc_List_List *x,
								     struct Cyc_List_List *y))Cyc_List_rzip)(rgn,
													     rgn,
													     _temp1022->tvs,
													     _temp1023);
	    t2 = Cyc_Tcutil_rsubstitute(rgn,inst,(void *)({ struct Cyc_Absyn_Structfield *_temp1058 = finfo;
					  if (_temp1058 == 0) {
					    _throw(Null_Exception);
					  }
					  _temp1058->type; }));
	  }
	  ;
	  _pop_region(&_temp1057);
	}
	return t2;}
      }
    
    _LL1009: {
    struct Cyc_Absyn_Structfield *finfo = Cyc_Absyn_lookup_field(_temp1027,
								 f);
      if (finfo == 0) {
	return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1059 = *f;
				    xprintf("struct has no %.*s field",_temp1059.last_plus_one - _temp1059.curr,
					    _temp1059.curr); }));
      }
      return (void *)({ struct Cyc_Absyn_Structfield *_temp1060 = finfo;
	if (_temp1060 == 0) {
	  _throw(Null_Exception);
	}
	_temp1060->type; });
    }
    _LL1011: {
    struct Cyc_Absyn_Structfield *finfo = Cyc_Absyn_lookup_field(_temp1029,
								 f);
      if (finfo == 0) {
	return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1061 = *f;
				    xprintf("union has no %.*s field",_temp1061.last_plus_one - _temp1061.curr,
					    _temp1061.curr); }));
      }
      return (void *)({ struct Cyc_Absyn_Structfield *_temp1062 = finfo;
	if (_temp1062 == 0) {
	  _throw(Null_Exception);
	}
	_temp1062->type; });
    }
    _LL1013: if (_temp1036 == 0) {
      return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp1063 = (char *)"unresolved union type (COMPILER ERROR)";
				  struct _tagged_string _temp1064;
				  _temp1064.curr = _temp1063;
				  _temp1064.base = _temp1063;
				  _temp1064.last_plus_one = _temp1063 + 39;
				  _temp1064; }));
    }
    { struct Cyc_Absyn_Structfield *finfo = Cyc_Absyn_lookup_union_field(_temp1033,
									 f);
      if (finfo == 0) {
	return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1066 = Cyc_Absynpp_qvar2string((struct _tuple1 *)({ struct _tuple1 *_temp1065 = _temp1036;
													if (_temp1065 == 0) {
													  _throw(Null_Exception);
													}
													_temp1065; }));
				    struct _tagged_string _temp1067 = *f;
				    xprintf("union %.*s has no %.*s field",
					    _temp1066.last_plus_one - _temp1066.curr,
					    _temp1066.curr,_temp1067.last_plus_one - _temp1067.curr,
					    _temp1067.curr); }));
      }
      { struct Cyc_List_List *inst = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
								 struct Cyc_List_List *y))Cyc_List_zip)(_temp1033->tvs,
													_temp1034);
	void *t2 = Cyc_Tcutil_substitute(inst,(void *)({ struct Cyc_Absyn_Structfield *_temp1068 = finfo;
					   if (_temp1068 == 0) {
					     _throw(Null_Exception);
					   }
					   _temp1068->type; }));
	return t2;}
      }
    
    _LL1015: (void *)(outer_e->r = (void *)((void *)({ struct Cyc_Absyn_Primop_e_struct *_temp1069 = (struct Cyc_Absyn_Primop_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Primop_e_struct));
	       _temp1069[0] = ({ struct Cyc_Absyn_Primop_e_struct _temp1070;
		 _temp1070.tag = Cyc_Absyn_Primop_e;
		 _temp1070.f1 = (void *)((void *)Cyc_Absyn_Size);
		 _temp1070.f2 = ({ struct Cyc_List_List *_temp1071 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
		   _temp1071->hd = (void *)e;
		   _temp1071->tl = 0;
		   _temp1071; });
		 _temp1070; });
	       _temp1069; })));
	     return Cyc_Absyn_uint_t;
    _LL1017: (void *)(outer_e->r = (void *)((void *)({ struct Cyc_Absyn_Primop_e_struct *_temp1072 = (struct Cyc_Absyn_Primop_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Primop_e_struct));
	       _temp1072[0] = ({ struct Cyc_Absyn_Primop_e_struct _temp1073;
		 _temp1073.tag = Cyc_Absyn_Primop_e;
		 _temp1073.f1 = (void *)((void *)Cyc_Absyn_Size);
		 _temp1073.f2 = ({ struct Cyc_List_List *_temp1074 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
		   _temp1074->hd = (void *)e;
		   _temp1074->tl = 0;
		   _temp1074; });
		 _temp1073; });
	       _temp1072; })));
	     return Cyc_Absyn_uint_t;
    _LL1019: if (Cyc_String_zstrcmp(*f,(struct _tagged_string)({ char *_temp1075 = (char *)"size";
				      struct _tagged_string _temp1076;
				      _temp1076.curr = _temp1075;
				      _temp1076.base = _temp1075;
				      _temp1076.last_plus_one = _temp1075 + 5;
				      _temp1076; })) == 0) {
      return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1077 = Cyc_Absynpp_typ2string(t);
				  xprintf("expecting struct, union or array, found %.*s",
					  _temp1077.last_plus_one - _temp1077.curr,
					  _temp1077.curr); }));
    }
    else {
      return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1078 = Cyc_Absynpp_typ2string(t);
				  xprintf("expecting struct or union, found %.*s",
					  _temp1078.last_plus_one - _temp1078.curr,
					  _temp1078.curr); }));
    }
    _LL1005: ;}
  
}
static void *Cyc_Tcexp_tcStructArrow(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				     void **topt,struct Cyc_Absyn_Exp *e,
				     struct _tagged_string *f) {
  Cyc_Tcexp_tcExp(te,0,e);
  { void *t = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp1143 = e->topt;
				    if (_temp1143 == 0) {
				      _throw(Null_Exception);
				    }
				    _temp1143->v; }));
    void *_temp1080 = t;
    struct Cyc_Absyn_PtrInfo _temp1086;
    struct Cyc_Absyn_Conref *_temp1088;
    struct Cyc_Absyn_Tqual _temp1090;
    struct Cyc_Absyn_Conref *_temp1092;
    void *_temp1094;
    void *_temp1096;
    _LL1082: if ((unsigned int)_temp1080 > 4 ? *((int *)_temp1080) == Cyc_Absyn_PointerType : 0) {
      _LL1087: _temp1086 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp1080)->f1;
	       _LL1097: _temp1096 = (void *)_temp1086.elt_typ;
			goto _LL1095;
	       _LL1095: _temp1094 = (void *)_temp1086.rgn_typ;
			goto _LL1093;
	       _LL1093: _temp1092 = (struct Cyc_Absyn_Conref *)_temp1086.nullable;
			goto _LL1091;
	       _LL1091: _temp1090 = (struct Cyc_Absyn_Tqual)_temp1086.tq;
			goto _LL1089;
	       _LL1089: _temp1088 = (struct Cyc_Absyn_Conref *)_temp1086.bounds;
			goto _LL1083;
    }
    else {
      goto _LL1084;
    }
    _LL1084: goto _LL1085;
    _LL1083: Cyc_Tcutil_check_nonzero_bound(loc,_temp1088);
    { void *_temp1098 = Cyc_Tcutil_compress(_temp1096);
      struct Cyc_Absyn_Structdecl **_temp1110;
      struct Cyc_Absyn_Structdecl *_temp1112;
      struct Cyc_List_List *_temp1113;
      struct _tuple1 *_temp1115;
      struct Cyc_Absyn_Uniondecl **_temp1117;
      struct Cyc_Absyn_Uniondecl *_temp1119;
      struct Cyc_List_List *_temp1120;
      struct _tuple1 *_temp1122;
      struct Cyc_List_List *_temp1124;
      struct Cyc_List_List *_temp1126;
      _LL1100: if ((unsigned int)_temp1098 > 4 ? *((int *)_temp1098) == Cyc_Absyn_StructType : 0) {
	_LL1116: _temp1115 = (struct _tuple1 *)((struct Cyc_Absyn_StructType_struct *)_temp1098)->f1;
		 goto _LL1114;
	_LL1114: _temp1113 = (struct Cyc_List_List *)((struct Cyc_Absyn_StructType_struct *)_temp1098)->f2;
		 goto _LL1111;
	_LL1111: _temp1110 = (struct Cyc_Absyn_Structdecl **)((struct Cyc_Absyn_StructType_struct *)_temp1098)->f3;
		 if (_temp1110 == 0) {
		   goto _LL1102;
		 }
		 else {
		   _temp1112 = *_temp1110;
		   goto _LL1101;
		 }
      }
      else {
	goto _LL1102;
      }
      _LL1102: if ((unsigned int)_temp1098 > 4 ? *((int *)_temp1098) == Cyc_Absyn_UnionType : 0) {
	_LL1123: _temp1122 = (struct _tuple1 *)((struct Cyc_Absyn_UnionType_struct *)_temp1098)->f1;
		 goto _LL1121;
	_LL1121: _temp1120 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnionType_struct *)_temp1098)->f2;
		 goto _LL1118;
	_LL1118: _temp1117 = (struct Cyc_Absyn_Uniondecl **)((struct Cyc_Absyn_UnionType_struct *)_temp1098)->f3;
		 if (_temp1117 == 0) {
		   goto _LL1104;
		 }
		 else {
		   _temp1119 = *_temp1117;
		   goto _LL1103;
		 }
      }
      else {
	goto _LL1104;
      }
      _LL1104: if ((unsigned int)_temp1098 > 4 ? *((int *)_temp1098) == Cyc_Absyn_AnonStructType : 0) {
	_LL1125: _temp1124 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonStructType_struct *)_temp1098)->f1;
		 goto _LL1105;
      }
      else {
	goto _LL1106;
      }
      _LL1106: if ((unsigned int)_temp1098 > 4 ? *((int *)_temp1098) == Cyc_Absyn_AnonUnionType : 0) {
	_LL1127: _temp1126 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonUnionType_struct *)_temp1098)->f1;
		 goto _LL1107;
      }
      else {
	goto _LL1108;
      }
      _LL1108: goto _LL1109;
      _LL1101: {
      struct Cyc_Absyn_Structfield *finfo = Cyc_Absyn_lookup_struct_field(_temp1112,
									  f);
	if (finfo == 0) {
	  return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1129 = Cyc_Absynpp_qvar2string((struct _tuple1 *)({ struct _tuple1 *_temp1128 = _temp1115;
													  if (_temp1128 == 0) {
													    _throw(Null_Exception);
													  }
													  _temp1128; }));
				      struct _tagged_string _temp1130 = *f;
				      xprintf("struct %.*s has no %.*s field",
					      _temp1129.last_plus_one - _temp1129.curr,
					      _temp1129.curr,_temp1130.last_plus_one - _temp1130.curr,
					      _temp1130.curr); }));
	}
	{ void *t3;
	  { struct _RegionHandle _temp1131 = _new_region();
	    struct _RegionHandle *rgn = &_temp1131;
	    _push_region(rgn);
	    { struct Cyc_List_List *inst = ((struct Cyc_List_List *(*)(struct _RegionHandle *r3,
								       struct _RegionHandle *r4,
								       struct Cyc_List_List *x,
								       struct Cyc_List_List *y))Cyc_List_rzip)(rgn,
													       rgn,
													       _temp1112->tvs,
													       _temp1113);
	      t3 = Cyc_Tcutil_rsubstitute(rgn,inst,(void *)({ struct Cyc_Absyn_Structfield *_temp1132 = finfo;
					    if (_temp1132 == 0) {
					      _throw(Null_Exception);
					    }
					    _temp1132->type; }));
	    }
	    ;
	    _pop_region(&_temp1131);
	  }
	  Cyc_Tcenv_check_rgn_accessible(te,loc,_temp1094);
	  return t3;}
	
      }
      _LL1103: {
      struct Cyc_Absyn_Structfield *finfo = Cyc_Absyn_lookup_union_field(_temp1119,
									 f);
	if (finfo == 0) {
	  return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1134 = Cyc_Absynpp_qvar2string((struct _tuple1 *)({ struct _tuple1 *_temp1133 = _temp1122;
													  if (_temp1133 == 0) {
													    _throw(Null_Exception);
													  }
													  _temp1133; }));
				      struct _tagged_string _temp1135 = *f;
				      xprintf("union %.*s has no %.*s field",
					      _temp1134.last_plus_one - _temp1134.curr,
					      _temp1134.curr,_temp1135.last_plus_one - _temp1135.curr,
					      _temp1135.curr); }));
	}
	{ struct Cyc_List_List *inst = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
								   struct Cyc_List_List *y))Cyc_List_zip)(_temp1119->tvs,
													  _temp1120);
	  void *t3 = Cyc_Tcutil_substitute(inst,(void *)({ struct Cyc_Absyn_Structfield *_temp1136 = finfo;
					     if (_temp1136 == 0) {
					       _throw(Null_Exception);
					     }
					     _temp1136->type; }));
	  Cyc_Tcenv_check_rgn_accessible(te,loc,_temp1094);
	  return t3;}
	
      }
      _LL1105: {
      struct Cyc_Absyn_Structfield *finfo = Cyc_Absyn_lookup_field(_temp1124,
								   f);
	if (finfo == 0) {
	  return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1137 = *f;
				      xprintf("struct has no %.*s field",
					      _temp1137.last_plus_one - _temp1137.curr,
					      _temp1137.curr); }));
	}
	return (void *)({ struct Cyc_Absyn_Structfield *_temp1138 = finfo;
	  if (_temp1138 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp1138->type; });
      }
      _LL1107: {
      struct Cyc_Absyn_Structfield *finfo = Cyc_Absyn_lookup_field(_temp1126,
								   f);
	if (finfo == 0) {
	  return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1139 = *f;
				      xprintf("union has no %.*s field",_temp1139.last_plus_one - _temp1139.curr,
					      _temp1139.curr); }));
	}
	return (void *)({ struct Cyc_Absyn_Structfield *_temp1140 = finfo;
	  if (_temp1140 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp1140->type; });
      }
      _LL1109: return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1141 = Cyc_Absynpp_typ2string(t);
					   xprintf("expecting struct pointer, found %.*s",
						   _temp1141.last_plus_one - _temp1141.curr,
						   _temp1141.curr); }));
      _LL1099: ;}
    
    _LL1085: return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1142 = Cyc_Absynpp_typ2string(t);
					 xprintf("expecting struct pointer, found %.*s",
						 _temp1142.last_plus_one - _temp1142.curr,
						 _temp1142.curr); }));
    _LL1081: ;}
  
}
static void *Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				    struct Cyc_List_List *ts,struct Cyc_Absyn_Exp *index) {
  unsigned int i = Cyc_Evexp_eval_const_uint_exp(index);
  struct _handler_cons _temp1144;
  _push_handler(&_temp1144);
  { void *_temp1145 = (void *)setjmp(_temp1144.handler);
    if (! _temp1145) {
      { void *_temp1146 = (*((struct _tuple7 *(*)(struct Cyc_List_List *x,
						  int i))Cyc_List_nth)(ts,
								       (int)i)).f2;
	_npop_handler(0);
	return _temp1146;
      }
      ;
      _pop_handler();
    }
    else {
      void *_temp1148 = _temp1145;
      _LL1150: if (_temp1148 == Cyc_List_Nth) {
	goto _LL1151;
      }
      else {
	goto _LL1152;
      }
      _LL1152: goto _LL1153;
      _LL1151: return Cyc_Tcexp_expr_err(te,loc,xprintf("index is %d but tuple has only %d fields",
							(int)i,((int (*)(struct Cyc_List_List *x))Cyc_List_length)(ts)));
      _LL1153: (void)_throw(_temp1148);
      _LL1149: ;
    }}
  
}
static void *Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				   void **topt,struct Cyc_Absyn_Exp *e1,struct Cyc_Absyn_Exp *e2) {
  Cyc_Tcexp_tcExp(te,0,e1);
  Cyc_Tcexp_tcExp(te,0,e2);
  { void *t1 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp1179 = e1->topt;
				     if (_temp1179 == 0) {
				       _throw(Null_Exception);
				     }
				     _temp1179->v; }));
    void *t2 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp1178 = e2->topt;
				     if (_temp1178 == 0) {
				       _throw(Null_Exception);
				     }
				     _temp1178->v; }));
    if (! Cyc_Tcutil_coerce_sint_typ(te,e2)) {
      return Cyc_Tcexp_expr_err(te,e2->loc,({ struct _tagged_string _temp1154 = Cyc_Absynpp_typ2string(t2);
				  xprintf("expecting int subscript, found %.*s",
					  _temp1154.last_plus_one - _temp1154.curr,
					  _temp1154.curr); }));
    }
    { void *_temp1155 = t1;
      struct Cyc_Absyn_PtrInfo _temp1163;
      struct Cyc_Absyn_Conref *_temp1165;
      struct Cyc_Absyn_Tqual _temp1167;
      struct Cyc_Absyn_Conref *_temp1169;
      void *_temp1171;
      void *_temp1173;
      struct Cyc_List_List *_temp1175;
      _LL1157: if ((unsigned int)_temp1155 > 4 ? *((int *)_temp1155) == Cyc_Absyn_PointerType : 0) {
	_LL1164: _temp1163 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp1155)->f1;
		 _LL1174: _temp1173 = (void *)_temp1163.elt_typ;
			  goto _LL1172;
		 _LL1172: _temp1171 = (void *)_temp1163.rgn_typ;
			  goto _LL1170;
		 _LL1170: _temp1169 = (struct Cyc_Absyn_Conref *)_temp1163.nullable;
			  goto _LL1168;
		 _LL1168: _temp1167 = (struct Cyc_Absyn_Tqual)_temp1163.tq;
			  goto _LL1166;
		 _LL1166: _temp1165 = (struct Cyc_Absyn_Conref *)_temp1163.bounds;
			  goto _LL1158;
      }
      else {
	goto _LL1159;
      }
      _LL1159: if ((unsigned int)_temp1155 > 4 ? *((int *)_temp1155) == Cyc_Absyn_TupleType : 0) {
	_LL1176: _temp1175 = (struct Cyc_List_List *)((struct Cyc_Absyn_TupleType_struct *)_temp1155)->f1;
		 goto _LL1160;
      }
      else {
	goto _LL1161;
      }
      _LL1161: goto _LL1162;
      _LL1158: if (Cyc_Tcutil_is_const_exp(te,e2)) {
		 Cyc_Tcutil_check_bound(loc,Cyc_Evexp_eval_const_uint_exp(e2),
					_temp1165);
	       }
	       else {
		 Cyc_Tcutil_check_nonzero_bound(loc,_temp1165);
	       }
	       Cyc_Tcenv_check_rgn_accessible(te,loc,_temp1171);
	       return _temp1173;
      _LL1160: return Cyc_Tcexp_ithTupleType(te,loc,_temp1175,e2);
      _LL1162: return Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1177 = Cyc_Absynpp_typ2string(t1);
					   xprintf("subscript applied to %.*s",
						   _temp1177.last_plus_one - _temp1177.curr,
						   _temp1177.curr); }));
      _LL1156: ;}
    }
  
}
static void *Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
			       void **topt,struct Cyc_List_List *es) {
  int done = 0;
  struct Cyc_List_List *fields = 0;
  if (topt != 0) {
    void *_temp1181 = Cyc_Tcutil_compress(({ void **_temp1180 = topt;
					    if (_temp1180 == 0) {
					      _throw(Null_Exception);
					    }
					    *_temp1180; }));
    struct Cyc_List_List *_temp1187;
    _LL1183: if ((unsigned int)_temp1181 > 4 ? *((int *)_temp1181) == Cyc_Absyn_TupleType : 0) {
      _LL1188: _temp1187 = (struct Cyc_List_List *)((struct Cyc_Absyn_TupleType_struct *)_temp1181)->f1;
	       goto _LL1184;
    }
    else {
      goto _LL1185;
    }
    _LL1185: goto _LL1186;
    _LL1184: if (((int (*)(struct Cyc_List_List *x))Cyc_List_length)(_temp1187) != ((int (*)(struct Cyc_List_List *x))Cyc_List_length)(es)) {
	       Cyc_Tcutil_terr(loc,(struct _tagged_string)({ char *_temp1189 = (char *)"tuple expression has the wrong number of fields";
				 struct _tagged_string _temp1190;
				 _temp1190.curr = _temp1189;
				 _temp1190.base = _temp1189;
				 _temp1190.last_plus_one = _temp1189 + 48;
				 _temp1190; }));
	       goto _LL1182;
	     }
	     for(0; es != 0; es = ({ struct Cyc_List_List *_temp1191 = es;
	       if (_temp1191 == 0) {
		 _throw(Null_Exception);
	       }
	       _temp1191->tl; }), _temp1187 = ({ struct Cyc_List_List *_temp1192 = _temp1187;
	       if (_temp1192 == 0) {
		 _throw(Null_Exception);
	       }
	       _temp1192->tl; })) {
	       void *topt2 = (*((struct _tuple7 *)({ struct Cyc_List_List *_temp1199 = _temp1187;
		 if (_temp1199 == 0) {
		   _throw(Null_Exception);
		 }
		 _temp1199->hd; }))).f2;
	       Cyc_Tcexp_tcExpInitializer(te,(void **)&topt2,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp1193 = es;
					    if (_temp1193 == 0) {
					      _throw(Null_Exception);
					    }
					    _temp1193->hd; }));
	       fields = ({ struct Cyc_List_List *_temp1194 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
		 _temp1194->hd = (void *)({ struct _tuple7 *_temp1195 = (struct _tuple7 *)GC_malloc(sizeof(struct _tuple7));
		   _temp1195->f1 = (*((struct _tuple7 *)({ struct Cyc_List_List *_temp1198 = _temp1187;
		     if (_temp1198 == 0) {
		       _throw(Null_Exception);
		     }
		     _temp1198->hd; }))).f1;
		   _temp1195->f2 = (void *)({ struct Cyc_Core_Opt *_temp1197 = ((struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp1196 = es;
		       if (_temp1196 == 0) {
			 _throw(Null_Exception);
		       }
		       _temp1196->hd; }))->topt;
		     if (_temp1197 == 0) {
		       _throw(Null_Exception);
		     }
		     _temp1197->v; });
		   _temp1195; });
		 _temp1194->tl = fields;
		 _temp1194; });
	     }
	     done = 1;
	     goto _LL1182;
    _LL1186: goto _LL1182;
    _LL1182: ;
  }
  if (! done) {
    for(0; es != 0; es = ({ struct Cyc_List_List *_temp1200 = es;
      if (_temp1200 == 0) {
	_throw(Null_Exception);
      }
      _temp1200->tl; })) {
      Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp1201 = es;
				   if (_temp1201 == 0) {
				     _throw(Null_Exception);
				   }
				   _temp1201->hd; }));
      fields = ({ struct Cyc_List_List *_temp1202 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
	_temp1202->hd = (void *)({ struct _tuple7 *_temp1203 = (struct _tuple7 *)GC_malloc(sizeof(struct _tuple7));
	  _temp1203->f1 = Cyc_Absyn_empty_tqual();
	  _temp1203->f2 = (void *)({ struct Cyc_Core_Opt *_temp1205 = ((struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp1204 = es;
	      if (_temp1204 == 0) {
		_throw(Null_Exception);
	      }
	      _temp1204->hd; }))->topt;
	    if (_temp1205 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp1205->v; });
	  _temp1203; });
	_temp1202->tl = fields;
	_temp1202; });
    }
  }
  return (void *)({ struct Cyc_Absyn_TupleType_struct *_temp1206 = (struct Cyc_Absyn_TupleType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_TupleType_struct));
    _temp1206[0] = ({ struct Cyc_Absyn_TupleType_struct _temp1207;
      _temp1207.tag = Cyc_Absyn_TupleType;
      _temp1207.f1 = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x))Cyc_List_imp_rev)(fields);
      _temp1207; });
    _temp1206; });
}
static void *Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				     void **topt,struct _tuple2 *t,struct Cyc_List_List *des) {
  return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp1208 = (char *)"tcCompoundLit";
			      struct _tagged_string _temp1209;
			      _temp1209.curr = _temp1208;
			      _temp1209.base = _temp1208;
			      _temp1209.last_plus_one = _temp1208 + 14;
			      _temp1209; }));
}
static void *Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
			       void **elt_topt,struct Cyc_List_List *des) {
  struct Cyc_List_List *es = ((struct Cyc_List_List *(*)(struct Cyc_Absyn_Exp *(*f)(struct _tuple6 *),
							 struct Cyc_List_List *x))Cyc_List_map)((struct Cyc_Absyn_Exp *(*)(struct _tuple6 *))Cyc_Core_snd,
												des);
  void *res = Cyc_Absyn_new_evar((void *)Cyc_Absyn_MemKind);
  struct Cyc_Absyn_Const_e_struct *sz_rexp = ({ struct Cyc_Absyn_Const_e_struct *_temp1232 = (struct Cyc_Absyn_Const_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Const_e_struct));
    _temp1232[0] = ({ struct Cyc_Absyn_Const_e_struct _temp1233;
      _temp1233.tag = Cyc_Absyn_Const_e;
      _temp1233.f1 = (void *)((void *)({ struct Cyc_Absyn_Int_c_struct *_temp1234 = (struct Cyc_Absyn_Int_c_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Int_c_struct));
	_temp1234[0] = ({ struct Cyc_Absyn_Int_c_struct _temp1235;
	  _temp1235.tag = Cyc_Absyn_Int_c;
	  _temp1235.f1 = (void *)((void *)Cyc_Absyn_Unsigned);
	  _temp1235.f2 = ((int (*)(struct Cyc_List_List *x))Cyc_List_length)(es);
	  _temp1235; });
	_temp1234; }));
      _temp1233; });
    _temp1232; });
  struct Cyc_Absyn_Exp *sz_exp = Cyc_Absyn_new_exp((void *)sz_rexp,loc);
  sz_exp->topt = ({ struct Cyc_Core_Opt *_temp1210 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
    _temp1210->v = (void *)Cyc_Absyn_uint_t;
    _temp1210; });
  { void *res_t2 = (void *)({ struct Cyc_Absyn_ArrayType_struct *_temp1230 = (struct Cyc_Absyn_ArrayType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_ArrayType_struct));
      _temp1230[0] = ({ struct Cyc_Absyn_ArrayType_struct _temp1231;
	_temp1231.tag = Cyc_Absyn_ArrayType;
	_temp1231.f1 = (void *)res;
	_temp1231.f2 = Cyc_Absyn_empty_tqual();
	_temp1231.f3 = (struct Cyc_Absyn_Exp *)sz_exp;
	_temp1231; });
      _temp1230; });
    { struct Cyc_List_List *es2 = es;
      for(0; es2 != 0; es2 = ({ struct Cyc_List_List *_temp1211 = es2;
	if (_temp1211 == 0) {
	  _throw(Null_Exception);
	}
	_temp1211->tl; })) {
	Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp1212 = es2;
				     if (_temp1212 == 0) {
				       _throw(Null_Exception);
				     }
				     _temp1212->hd; }));
      }
    }
    if (! Cyc_Tcutil_coerce_list(te,res,es)) {
      Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp1213 = es;
			if (_temp1213 == 0) {
			  _throw(Null_Exception);
			}
			_temp1213->hd; }))->loc,({ struct _tagged_string _temp1214 = Cyc_Absynpp_typ2string(res);
			xprintf("elements of array do not all have the same type (%.*s)",
				_temp1214.last_plus_one - _temp1214.curr,
				_temp1214.curr); }));
    }
    { int offset = 0;
      for(0; des != 0; offset++, des = ({ struct Cyc_List_List *_temp1215 = des;
	if (_temp1215 == 0) {
	  _throw(Null_Exception);
	}
	_temp1215->tl; })) {
	struct Cyc_List_List *ds = (*((struct _tuple6 *)({ struct Cyc_List_List *_temp1229 = des;
	  if (_temp1229 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp1229->hd; }))).f1;
	if (ds != 0) {
	  void *_temp1217 = (void *)({ struct Cyc_List_List *_temp1216 = ds;
	    if (_temp1216 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp1216->hd; });
	  struct _tagged_string *_temp1223;
	  struct Cyc_Absyn_Exp *_temp1225;
	  _LL1219: if (*((int *)_temp1217) == Cyc_Absyn_FieldName) {
	    _LL1224: _temp1223 = (struct _tagged_string *)((struct Cyc_Absyn_FieldName_struct *)_temp1217)->f1;
		     goto _LL1220;
	  }
	  else {
	    goto _LL1221;
	  }
	  _LL1221: if (*((int *)_temp1217) == Cyc_Absyn_ArrayElement) {
	    _LL1226: _temp1225 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayElement_struct *)_temp1217)->f1;
		     goto _LL1222;
	  }
	  else {
	    goto _LL1218;
	  }
	  _LL1220: Cyc_Tcutil_terr(loc,(struct _tagged_string)({ char *_temp1227 = (char *)"only array index designators are supported";
				     struct _tagged_string _temp1228;
				     _temp1228.curr = _temp1227;
				     _temp1228.base = _temp1227;
				     _temp1228.last_plus_one = _temp1227 + 43;
				     _temp1228; }));
		   goto _LL1218;
	  _LL1222: Cyc_Tcexp_tcExpInitializer(te,0,_temp1225);
	  { unsigned int i = Cyc_Evexp_eval_const_uint_exp(_temp1225);
	    if (i != offset) {
	      Cyc_Tcutil_terr(_temp1225->loc,xprintf("expecting index designator %d but found %d",
						     offset,(int)i));
	    }
	    goto _LL1218;}
	  
	  _LL1218: ;
	}
      }
    }
    return res_t2;}
  
}
static void *Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				       void **topt,struct Cyc_Absyn_Vardecl *vd,
				       struct Cyc_Absyn_Exp *bound,struct Cyc_Absyn_Exp *body) {
  Cyc_Tcexp_tcExp(te,0,bound);
  if (! Cyc_Tcutil_coerce_uint_typ(te,bound)) {
    Cyc_Tcutil_terr(bound->loc,({ struct _tagged_string _temp1237 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp1236 = bound->topt;
											     if (_temp1236 == 0) {
											       _throw(Null_Exception);
											     }
											     _temp1236->v; }));
		      xprintf("expecting unsigned int, found %.*s",_temp1237.last_plus_one - _temp1237.curr,
			      _temp1237.curr); }));
  }
  if (! (vd->tq).q_const) {
    ((void (*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp1238 = (char *)"comprehension index variable is not declared const!";
							  struct _tagged_string _temp1239;
							  _temp1239.curr = _temp1238;
							  _temp1239.base = _temp1238;
							  _temp1239.last_plus_one = _temp1238 + 52;
							  _temp1239; }));
  }
  if (te->le != 0) {
    te = Cyc_Tcenv_new_block(loc,te);
    te = Cyc_Tcenv_add_local_var(loc,te,vd);
  }
  else {
    if (! Cyc_Tcutil_is_const_exp(te,bound)) {
      Cyc_Tcutil_terr(bound->loc,(struct _tagged_string)({ char *_temp1240 = (char *)"bound is not constant";
			struct _tagged_string _temp1241;
			_temp1241.curr = _temp1240;
			_temp1241.base = _temp1240;
			_temp1241.last_plus_one = _temp1240 + 22;
			_temp1241; }));
    }
    if (! Cyc_Tcutil_is_const_exp(te,body)) {
      Cyc_Tcutil_terr(bound->loc,(struct _tagged_string)({ char *_temp1242 = (char *)"body is not constant";
			struct _tagged_string _temp1243;
			_temp1243.curr = _temp1242;
			_temp1243.base = _temp1242;
			_temp1243.last_plus_one = _temp1242 + 21;
			_temp1243; }));
    }
  }
  { struct Cyc_Absyn_PtrInfo pinfo;
    void **topt2 = 0;
    struct Cyc_Absyn_Tqual *tqopt = 0;
    if (topt != 0) {
      void *_temp1245 = Cyc_Tcutil_compress(({ void **_temp1244 = topt;
					      if (_temp1244 == 0) {
						_throw(Null_Exception);
					      }
					      *_temp1244; }));
      struct Cyc_Absyn_PtrInfo _temp1253;
      struct Cyc_Absyn_Exp *_temp1255;
      struct Cyc_Absyn_Tqual _temp1257;
      struct Cyc_Absyn_Tqual *_temp1259;
      void *_temp1260;
      void **_temp1262;
      _LL1247: if ((unsigned int)_temp1245 > 4 ? *((int *)_temp1245) == Cyc_Absyn_PointerType : 0) {
	_LL1254: _temp1253 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp1245)->f1;
		 goto _LL1248;
      }
      else {
	goto _LL1249;
      }
      _LL1249: if ((unsigned int)_temp1245 > 4 ? *((int *)_temp1245) == Cyc_Absyn_ArrayType : 0) {
	_LL1261: _temp1260 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp1245)->f1;
		 _temp1262 = &((struct Cyc_Absyn_ArrayType_struct *)_temp1245)->f1;
		 goto _LL1258;
	_LL1258: _temp1257 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp1245)->f2;
		 _temp1259 = &((struct Cyc_Absyn_ArrayType_struct *)_temp1245)->f2;
		 goto _LL1256;
	_LL1256: _temp1255 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp1245)->f3;
		 goto _LL1250;
      }
      else {
	goto _LL1251;
      }
      _LL1251: goto _LL1252;
      _LL1248: pinfo = _temp1253;
	       topt2 = (void **)((void *)&pinfo.elt_typ);
	       tqopt = (struct Cyc_Absyn_Tqual *)&pinfo.tq;
	       goto _LL1246;
      _LL1250: topt2 = (void **)_temp1262;
	       tqopt = (struct Cyc_Absyn_Tqual *)_temp1259;
	       goto _LL1246;
      _LL1252: goto _LL1246;
      _LL1246: ;
    }
    { void *t = Cyc_Tcexp_tcExp(te,topt2,body);
      return (void *)({ struct Cyc_Absyn_ArrayType_struct *_temp1263 = (struct Cyc_Absyn_ArrayType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_ArrayType_struct));
	_temp1263[0] = ({ struct Cyc_Absyn_ArrayType_struct _temp1264;
	  _temp1264.tag = Cyc_Absyn_ArrayType;
	  _temp1264.f1 = (void *)t;
	  _temp1264.f2 = tqopt == 0 ? Cyc_Absyn_empty_tqual() : ({ struct Cyc_Absyn_Tqual *_temp1265 = tqopt;
	    if (_temp1265 == 0) {
	      _throw(Null_Exception);
	    }
	    *_temp1265; });
	  _temp1264.f3 = (struct Cyc_Absyn_Exp *)bound;
	  _temp1264; });
	_temp1263; });}
    }
  
}
struct _tuple8 {
  struct Cyc_Absyn_Structfield *f1;
  struct Cyc_Absyn_Exp *f2;
};
static void *Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				void **topt,struct _tuple1 **tn,struct Cyc_Core_Opt **otyps,
				struct Cyc_List_List *args,struct Cyc_Absyn_Structdecl **sd_opt) {
  struct Cyc_Absyn_Structdecl *sd;
  if (*sd_opt != 0) {
    sd = (struct Cyc_Absyn_Structdecl *)({ struct Cyc_Absyn_Structdecl *_temp1266 = *sd_opt;
      if (_temp1266 == 0) {
	_throw(Null_Exception);
      }
      _temp1266; });
  }
  else {
    { struct _handler_cons _temp1267;
      _push_handler(&_temp1267);
      { void *_temp1268 = (void *)setjmp(_temp1267.handler);
	if (! _temp1268) {
	  sd = *Cyc_Tcenv_lookup_structdecl(te,loc,*tn);
	  ;
	  _pop_handler();
	}
	else {
	  void *_temp1270 = _temp1268;
	  _LL1272: if (_temp1270 == Cyc_Dict_Absent) {
	    goto _LL1273;
	  }
	  else {
	    goto _LL1274;
	  }
	  _LL1274: goto _LL1275;
	  _LL1273: Cyc_Tcutil_terr(loc,({ struct _tagged_string _temp1276 = Cyc_Absynpp_qvar2string(*tn);
				     xprintf("unbound struct name %.*s",_temp1276.last_plus_one - _temp1276.curr,
					     _temp1276.curr); }));
		   return topt != 0 ? ({ void **_temp1277 = topt;
		     if (_temp1277 == 0) {
		       _throw(Null_Exception);
		     }
		     *_temp1277; }) : (void *)Cyc_Absyn_VoidType;
	  _LL1275: (void)_throw(_temp1270);
	  _LL1271: ;
	}}
      
    }
    *sd_opt = (struct Cyc_Absyn_Structdecl *)sd;
    if (sd->name != 0) {
      *tn = (struct _tuple1 *)({ struct Cyc_Core_Opt *_temp1278 = sd->name;
	if (_temp1278 == 0) {
	  _throw(Null_Exception);
	}
	_temp1278->v; });
    }
  }
  { struct _RegionHandle _temp1279 = _new_region();
    struct _RegionHandle *rgn = &_temp1279;
    _push_region(rgn);
    { struct Cyc_List_List *inst = ((struct Cyc_List_List *(*)(struct _RegionHandle *,
							       struct _tuple4 *(*f)(struct _RegionHandle *,
										    struct Cyc_Absyn_Tvar *),
							       struct _RegionHandle *env,
							       struct Cyc_List_List *x))Cyc_List_rmap_c)(rgn,
													 Cyc_Tcutil_r_make_inst_var,
													 rgn,
													 sd->tvs);
      struct Cyc_List_List *typs = ((struct Cyc_List_List *(*)(void *(*f)(struct _tuple4 *),
							       struct Cyc_List_List *x))Cyc_List_map)((void *(*)(struct _tuple4 *))Cyc_Core_snd,
												      inst);
      struct Cyc_Absyn_StructType_struct *res_typ = ({ struct Cyc_Absyn_StructType_struct *_temp1301 = (struct Cyc_Absyn_StructType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_StructType_struct));
	_temp1301[0] = ({ struct Cyc_Absyn_StructType_struct _temp1302;
	  _temp1302.tag = Cyc_Absyn_StructType;
	  _temp1302.f1 = (struct _tuple1 *)*tn;
	  _temp1302.f2 = typs;
	  _temp1302.f3 = ({ struct Cyc_Absyn_Structdecl **_temp1303 = (struct Cyc_Absyn_Structdecl **)GC_malloc(sizeof(struct Cyc_Absyn_Structdecl *));
	    _temp1303[0] = sd;
	    _temp1303; });
	  _temp1302; });
	_temp1301; });
      if (topt != 0) {
	Cyc_Tcutil_unify((void *)res_typ,({ void **_temp1280 = topt;
			   if (_temp1280 == 0) {
			     _throw(Null_Exception);
			   }
			   *_temp1280; }));
      }
      *otyps = ({ struct Cyc_Core_Opt *_temp1281 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
	_temp1281->v = (void *)typs;
	_temp1281; });
      if (sd->fields == 0) {
	Cyc_Tcutil_terr(loc,(struct _tagged_string)({ char *_temp1282 = (char *)"can't build abstract struct";
			  struct _tagged_string _temp1283;
			  _temp1283.curr = _temp1282;
			  _temp1283.base = _temp1282;
			  _temp1283.last_plus_one = _temp1282 + 28;
			  _temp1283; }));
	{ void *_temp1284 = (void *)res_typ;
	  _npop_handler(0);
	  return _temp1284;}
	
      }
      { struct Cyc_List_List *fields = ((struct Cyc_List_List *(*)(struct _RegionHandle *rgn,
								   struct Cyc_Position_Segment *loc,
								   struct Cyc_List_List *des,
								   struct Cyc_List_List *fields))Cyc_Tcutil_resolve_struct_designators)(rgn,
																	loc,
																	args,
																	(struct Cyc_List_List *)({ struct Cyc_Core_Opt *_temp1300 = sd->fields;
																	  if (_temp1300 == 0) {
																	    _throw(Null_Exception);
																	  }
																	  _temp1300->v; }));
	for(0; fields != 0; fields = ({ struct Cyc_List_List *_temp1285 = fields;
	  if (_temp1285 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp1285->tl; })) {
	  struct _tuple8 _temp1289;
	  struct Cyc_Absyn_Exp *_temp1290;
	  struct Cyc_Absyn_Structfield *_temp1292;
	  struct _tuple8 *_temp1287 = (struct _tuple8 *)({ struct Cyc_List_List *_temp1286 = fields;
	    if (_temp1286 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp1286->hd; });
	  _temp1289 = *_temp1287;
	  _LL1293: _temp1292 = _temp1289.f1;
		   goto _LL1291;
	  _LL1291: _temp1290 = _temp1289.f2;
		   goto _LL1288;
	  _LL1288: {
	  void *inst_fieldtyp = Cyc_Tcutil_rsubstitute(rgn,inst,(void *)_temp1292->type);
	    Cyc_Tcexp_tcExpInitializer(te,(void **)&inst_fieldtyp,_temp1290);
	    if (! Cyc_Tcutil_coerce_arg(te,_temp1290,inst_fieldtyp)) {
	      Cyc_Tcutil_terr(_temp1290->loc,({ struct _tagged_string _temp1295 = *_temp1292->name;
				struct _tagged_string _temp1296 = Cyc_Absynpp_qvar2string(*tn);
				struct _tagged_string _temp1297 = Cyc_Absynpp_typ2string(inst_fieldtyp);
				struct _tagged_string _temp1298 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp1294 = _temp1290->topt;
											   if (_temp1294 == 0) {
											     _throw(Null_Exception);
											   }
											   _temp1294->v; }));
				xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
					_temp1295.last_plus_one - _temp1295.curr,
					_temp1295.curr,_temp1296.last_plus_one - _temp1296.curr,
					_temp1296.curr,_temp1297.last_plus_one - _temp1297.curr,
					_temp1297.curr,_temp1298.last_plus_one - _temp1298.curr,
					_temp1298.curr); }));
	    }
	  }
	}
	{ void *_temp1299 = (void *)res_typ;
	  _npop_handler(0);
	  return _temp1299;}
	}
      
    }
    ;
    _pop_region(&_temp1279);}
  
}
static void *Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				    void *ts,struct Cyc_List_List *args) {
  { struct _RegionHandle _temp1304 = _new_region();
    struct _RegionHandle *rgn = &_temp1304;
    _push_region(rgn);
    { void *_temp1305 = Cyc_Tcutil_compress(ts);
      struct Cyc_List_List *_temp1311;
      _LL1307: if ((unsigned int)_temp1305 > 4 ? *((int *)_temp1305) == Cyc_Absyn_AnonStructType : 0) {
	_LL1312: _temp1311 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonStructType_struct *)_temp1305)->f1;
		 goto _LL1308;
      }
      else {
	goto _LL1309;
      }
      _LL1309: goto _LL1310;
      _LL1308: {
      struct Cyc_List_List *fields = ((struct Cyc_List_List *(*)(struct _RegionHandle *rgn,
								 struct Cyc_Position_Segment *loc,
								 struct Cyc_List_List *des,
								 struct Cyc_List_List *fields))Cyc_Tcutil_resolve_struct_designators)(rgn,
																      loc,
																      args,
																      _temp1311);
	for(0; fields != 0; fields = ({ struct Cyc_List_List *_temp1313 = fields;
	  if (_temp1313 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp1313->tl; })) {
	  struct _tuple8 _temp1317;
	  struct Cyc_Absyn_Exp *_temp1318;
	  struct Cyc_Absyn_Structfield *_temp1320;
	  struct _tuple8 *_temp1315 = (struct _tuple8 *)({ struct Cyc_List_List *_temp1314 = fields;
	    if (_temp1314 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp1314->hd; });
	  _temp1317 = *_temp1315;
	  _LL1321: _temp1320 = _temp1317.f1;
		   goto _LL1319;
	  _LL1319: _temp1318 = _temp1317.f2;
		   goto _LL1316;
	  _LL1316: Cyc_Tcexp_tcExpInitializer(te,(void **)((void *)&_temp1320->type),
					      _temp1318);
		   if (! Cyc_Tcutil_coerce_arg(te,_temp1318,(void *)_temp1320->type)) {
		     Cyc_Tcutil_terr(_temp1318->loc,({ struct _tagged_string _temp1323 = *_temp1320->name;
				       struct _tagged_string _temp1324 = Cyc_Absynpp_typ2string((void *)_temp1320->type);
				       struct _tagged_string _temp1325 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp1322 = _temp1318->topt;
												  if (_temp1322 == 0) {
												    _throw(Null_Exception);
												  }
												  _temp1322->v; }));
				       xprintf("field %.*s of struct expects type %.*s != %.*s",
					       _temp1323.last_plus_one - _temp1323.curr,
					       _temp1323.curr,_temp1324.last_plus_one - _temp1324.curr,
					       _temp1324.curr,_temp1325.last_plus_one - _temp1325.curr,
					       _temp1325.curr); }));
		   }
	}
	goto _LL1306;
      }
      _LL1310: ((void (*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp1326 = (char *)"tcAnonStruct:  not an AnonStructType";
								     struct _tagged_string _temp1327;
								     _temp1327.curr = _temp1326;
								     _temp1327.base = _temp1326;
								     _temp1327.last_plus_one = _temp1326 + 37;
								     _temp1327; }));
	       goto _LL1306;
      _LL1306: ;
    }
    ;
    _pop_region(&_temp1304);
  }
  return ts;
}
static void *Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				void **topt,struct Cyc_Absyn_Exp *e,struct Cyc_Core_Opt **all_ref,
				struct Cyc_Core_Opt **exist_ref,struct Cyc_List_List *es,
				struct Cyc_Absyn_Tuniondecl *tud,struct Cyc_Absyn_Tunionfield *tuf) {
  struct _RegionHandle _temp1328 = _new_region();
  struct _RegionHandle *rgn = &_temp1328;
  _push_region(rgn);
  { struct Cyc_List_List *all_inst = ((struct Cyc_List_List *(*)(struct _RegionHandle *,
								 struct _tuple4 *(*f)(struct _RegionHandle *,
										      struct Cyc_Absyn_Tvar *),
								 struct _RegionHandle *env,
								 struct Cyc_List_List *x))Cyc_List_rmap_c)(rgn,
													   Cyc_Tcutil_r_make_inst_var,
													   rgn,
													   tud->tvs);
    struct Cyc_List_List *exist_inst = ((struct Cyc_List_List *(*)(struct _RegionHandle *,
								   struct _tuple4 *(*f)(struct _RegionHandle *,
											struct Cyc_Absyn_Tvar *),
								   struct _RegionHandle *env,
								   struct Cyc_List_List *x))Cyc_List_rmap_c)(rgn,
													     Cyc_Tcutil_r_make_inst_var,
													     rgn,
													     tuf->tvs);
    struct Cyc_List_List *all_typs = ((struct Cyc_List_List *(*)(void *(*f)(struct _tuple4 *),
								 struct Cyc_List_List *x))Cyc_List_map)((void *(*)(struct _tuple4 *))Cyc_Core_snd,
													all_inst);
    struct Cyc_List_List *exist_typs = ((struct Cyc_List_List *(*)(void *(*f)(struct _tuple4 *),
								   struct Cyc_List_List *x))Cyc_List_map)((void *(*)(struct _tuple4 *))Cyc_Core_snd,
													  exist_inst);
    struct Cyc_List_List *inst = ((struct Cyc_List_List *(*)(struct _RegionHandle *,
							     struct Cyc_List_List *x,
							     struct Cyc_List_List *y))Cyc_List_rappend)(rgn,
													all_inst,
													exist_inst);
    *all_ref = ({ struct Cyc_Core_Opt *_temp1329 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
      _temp1329->v = (void *)all_typs;
      _temp1329; });
    *exist_ref = ({ struct Cyc_Core_Opt *_temp1330 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
      _temp1330->v = (void *)exist_typs;
      _temp1330; });
    { void *res = (void *)({ struct Cyc_Absyn_TunionFieldType_struct *_temp1378 = (struct Cyc_Absyn_TunionFieldType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_TunionFieldType_struct));
	_temp1378[0] = ({ struct Cyc_Absyn_TunionFieldType_struct _temp1379;
	  _temp1379.tag = Cyc_Absyn_TunionFieldType;
	  _temp1379.f1 = ({ struct Cyc_Absyn_TunionFieldInfo _temp1380;
	    _temp1380.field_info = (void *)((void *)({ struct Cyc_Absyn_KnownTunionfield_struct *_temp1381 = (struct Cyc_Absyn_KnownTunionfield_struct *)GC_malloc(sizeof(struct Cyc_Absyn_KnownTunionfield_struct));
	      _temp1381[0] = ({ struct Cyc_Absyn_KnownTunionfield_struct _temp1382;
		_temp1382.tag = Cyc_Absyn_KnownTunionfield;
		_temp1382.f1 = tud;
		_temp1382.f2 = tuf;
		_temp1382; });
	      _temp1381; }));
	    _temp1380.targs = all_typs;
	    _temp1380; });
	  _temp1379; });
	_temp1378; });
      if (topt != 0) {
	void *_temp1332 = Cyc_Tcutil_compress(({ void **_temp1331 = topt;
						if (_temp1331 == 0) {
						  _throw(Null_Exception);
						}
						*_temp1331; }));
	struct Cyc_Absyn_TunionFieldInfo _temp1340;
	struct Cyc_Absyn_TunionInfo _temp1342;
	void *_temp1344;
	struct Cyc_List_List *_temp1346;
	void *_temp1348;
	_LL1334: if ((unsigned int)_temp1332 > 4 ? *((int *)_temp1332) == Cyc_Absyn_TunionFieldType : 0) {
	  _LL1341: _temp1340 = (struct Cyc_Absyn_TunionFieldInfo)((struct Cyc_Absyn_TunionFieldType_struct *)_temp1332)->f1;
		   goto _LL1335;
	}
	else {
	  goto _LL1336;
	}
	_LL1336: if ((unsigned int)_temp1332 > 4 ? *((int *)_temp1332) == Cyc_Absyn_TunionType : 0) {
	  _LL1343: _temp1342 = (struct Cyc_Absyn_TunionInfo)((struct Cyc_Absyn_TunionType_struct *)_temp1332)->f1;
		   _LL1349: _temp1348 = (void *)_temp1342.tunion_info;
			    goto _LL1347;
		   _LL1347: _temp1346 = (struct Cyc_List_List *)_temp1342.targs;
			    goto _LL1345;
		   _LL1345: _temp1344 = (void *)_temp1342.rgn;
			    goto _LL1337;
	}
	else {
	  goto _LL1338;
	}
	_LL1338: goto _LL1339;
	_LL1335: Cyc_Tcutil_unify(({ void **_temp1350 = topt;
				    if (_temp1350 == 0) {
				      _throw(Null_Exception);
				    }
				    *_temp1350; }),res);
		 goto _LL1333;
	_LL1337: { struct Cyc_List_List *a = all_typs;
	  for(0; a != 0 ? _temp1346 != 0 : 0; a = ({ struct Cyc_List_List *_temp1351 = a;
	    if (_temp1351 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp1351->tl; }), _temp1346 = ({ struct Cyc_List_List *_temp1352 = _temp1346;
	    if (_temp1352 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp1352->tl; })) {
	    Cyc_Tcutil_unify((void *)({ struct Cyc_List_List *_temp1353 = a;
			       if (_temp1353 == 0) {
				 _throw(Null_Exception);
			       }
			       _temp1353->hd; }),(void *)({ struct Cyc_List_List *_temp1354 = _temp1346;
			       if (_temp1354 == 0) {
				 _throw(Null_Exception);
			       }
			       _temp1354->hd; }));
	  }
	}
	if (tuf->typs == 0 ? es == 0 : 0) {
	  e->topt = ({ struct Cyc_Core_Opt *_temp1355 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
	    _temp1355->v = (void *)res;
	    _temp1355; });
	  res = (void *)({ struct Cyc_Absyn_TunionType_struct *_temp1356 = (struct Cyc_Absyn_TunionType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_TunionType_struct));
	    _temp1356[0] = ({ struct Cyc_Absyn_TunionType_struct _temp1357;
	      _temp1357.tag = Cyc_Absyn_TunionType;
	      _temp1357.f1 = ({ struct Cyc_Absyn_TunionInfo _temp1358;
		_temp1358.tunion_info = (void *)((void *)({ struct Cyc_Absyn_KnownTunion_struct *_temp1359 = (struct Cyc_Absyn_KnownTunion_struct *)GC_malloc(sizeof(struct Cyc_Absyn_KnownTunion_struct));
		  _temp1359[0] = ({ struct Cyc_Absyn_KnownTunion_struct _temp1360;
		    _temp1360.tag = Cyc_Absyn_KnownTunion;
		    _temp1360.f1 = tud;
		    _temp1360; });
		  _temp1359; }));
		_temp1358.targs = all_typs;
		_temp1358.rgn = (void *)_temp1344;
		_temp1358; });
	      _temp1357; });
	    _temp1356; });
	  Cyc_Tcutil_unchecked_cast(te,e,res);
	  { void *_temp1361 = res;
	    _npop_handler(0);
	    return _temp1361;}
	  
	}
	goto _LL1333;
	_LL1339: goto _LL1333;
	_LL1333: ;
      }
      { struct Cyc_List_List *ts = tuf->typs;
	for(0; es != 0 ? ts != 0 : 0; es = ({ struct Cyc_List_List *_temp1362 = es;
	  if (_temp1362 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp1362->tl; }), ts = ({ struct Cyc_List_List *_temp1363 = ts;
	  if (_temp1363 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp1363->tl; })) {
	  struct Cyc_Absyn_Exp *e = (struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp1371 = es;
	    if (_temp1371 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp1371->hd; });
	  void *t = Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple7 *)({ struct Cyc_List_List *_temp1370 = ts;
					     if (_temp1370 == 0) {
					       _throw(Null_Exception);
					     }
					     _temp1370->hd; }))).f2);
	  Cyc_Tcexp_tcExpInitializer(te,(void **)&t,e);
	  if (! Cyc_Tcutil_coerce_arg(te,e,t)) {
	    Cyc_Tcutil_terr(e->loc,({ struct _tagged_string _temp1367 = Cyc_Absynpp_qvar2string(tuf->name);
			      struct _tagged_string _temp1368 = Cyc_Absynpp_typ2string(t);
			      struct _tagged_string _temp1369 = e->topt == 0 ? (struct _tagged_string)({ char *_temp1364 = (char *)"?";
				struct _tagged_string _temp1365;
				_temp1365.curr = _temp1364;
				_temp1365.base = _temp1364;
				_temp1365.last_plus_one = _temp1364 + 2;
				_temp1365; }) : Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp1366 = e->topt;
									 if (_temp1366 == 0) {
									   _throw(Null_Exception);
									 }
									 _temp1366->v; }));
			      xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
				      _temp1367.last_plus_one - _temp1367.curr,
				      _temp1367.curr,_temp1368.last_plus_one - _temp1368.curr,
				      _temp1368.curr,_temp1369.last_plus_one - _temp1369.curr,
				      _temp1369.curr); }));
	  }
	}
	if (es != 0) {
	  void *_temp1374 = Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp1372 = es;
						 if (_temp1372 == 0) {
						   _throw(Null_Exception);
						 }
						 _temp1372->hd; }))->loc,
					       ({ struct _tagged_string _temp1373 = Cyc_Absynpp_qvar2string(tuf->name);
						 xprintf("too many arguments for tunion constructor %.*s",
							 _temp1373.last_plus_one - _temp1373.curr,
							 _temp1373.curr); }));
	  _npop_handler(0);
	  return _temp1374;
	}
	if (ts != 0) {
	  void *_temp1376 = Cyc_Tcexp_expr_err(te,loc,({ struct _tagged_string _temp1375 = Cyc_Absynpp_qvar2string(tuf->name);
						 xprintf("too few arguments for tunion constructor %.*s",
							 _temp1375.last_plus_one - _temp1375.curr,
							 _temp1375.curr); }));
	  _npop_handler(0);
	  return _temp1376;
	}
	{ void *_temp1377 = res;
	  _npop_handler(0);
	  return _temp1377;}
	}
      }
    
  }
  ;
  _pop_region(&_temp1328);
}
static void *Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				void **topt,struct Cyc_Absyn_Exp *ropt,void *t) {
  void *rgn = (void *)Cyc_Absyn_HeapRgn;
  if (ropt != 0) {
    void *expected_type = (void *)({ struct Cyc_Absyn_RgnHandleType_struct *_temp1394 = (struct Cyc_Absyn_RgnHandleType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));
      _temp1394[0] = ({ struct Cyc_Absyn_RgnHandleType_struct _temp1395;
	_temp1395.tag = Cyc_Absyn_RgnHandleType;
	_temp1395.f1 = (void *)Cyc_Absyn_new_evar((void *)Cyc_Absyn_RgnKind);
	_temp1395; });
      _temp1394; });
    void *handle_type = Cyc_Tcexp_tcExp(te,(void **)&expected_type,(struct Cyc_Absyn_Exp *)({ struct Cyc_Absyn_Exp *_temp1393 = ropt;
					  if (_temp1393 == 0) {
					    _throw(Null_Exception);
					  }
					  _temp1393; }));
    void *_temp1383 = Cyc_Tcutil_compress(handle_type);
    void *_temp1389;
    _LL1385: if ((unsigned int)_temp1383 > 4 ? *((int *)_temp1383) == Cyc_Absyn_RgnHandleType : 0) {
      _LL1390: _temp1389 = (void *)((struct Cyc_Absyn_RgnHandleType_struct *)_temp1383)->f1;
	       goto _LL1386;
    }
    else {
      goto _LL1387;
    }
    _LL1387: goto _LL1388;
    _LL1386: rgn = _temp1389;
	     Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
	     goto _LL1384;
    _LL1388: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp *_temp1391 = ropt;
			       if (_temp1391 == 0) {
				 _throw(Null_Exception);
			       }
			       _temp1391->loc; }),({ struct _tagged_string _temp1392 = Cyc_Absynpp_typ2string(handle_type);
			       xprintf("expecting region_t type but found %.*s",
				       _temp1392.last_plus_one - _temp1392.curr,
				       _temp1392.curr); }));
	     goto _LL1384;
    _LL1384: ;
  }
  Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void *)Cyc_Absyn_MemKind,
			t);
  { void *_temp1396 = t;
    struct Cyc_Absyn_TunionFieldInfo _temp1402;
    struct Cyc_List_List *_temp1404;
    void *_temp1406;
    struct Cyc_Absyn_Tunionfield *_temp1408;
    struct Cyc_Absyn_Tuniondecl *_temp1410;
    _LL1398: if ((unsigned int)_temp1396 > 4 ? *((int *)_temp1396) == Cyc_Absyn_TunionFieldType : 0) {
      _LL1403: _temp1402 = (struct Cyc_Absyn_TunionFieldInfo)((struct Cyc_Absyn_TunionFieldType_struct *)_temp1396)->f1;
	       _LL1407: _temp1406 = (void *)_temp1402.field_info;
			if (*((int *)_temp1406) == Cyc_Absyn_KnownTunionfield) {
			  _LL1411: _temp1410 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_KnownTunionfield_struct *)_temp1406)->f1;
				   goto _LL1409;
			  _LL1409: _temp1408 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Absyn_KnownTunionfield_struct *)_temp1406)->f2;
				   goto _LL1405;
			}
			else {
			  goto _LL1400;
			}
	       _LL1405: _temp1404 = (struct Cyc_List_List *)_temp1402.targs;
			goto _LL1399;
    }
    else {
      goto _LL1400;
    }
    _LL1400: goto _LL1401;
    _LL1399: if (_temp1408->tvs != 0) {
	       Cyc_Tcutil_terr(loc,(struct _tagged_string)({ char *_temp1412 = (char *)"malloc with existential types not yet implemented";
				 struct _tagged_string _temp1413;
				 _temp1413.curr = _temp1412;
				 _temp1413.base = _temp1412;
				 _temp1413.last_plus_one = _temp1412 + 50;
				 _temp1413; }));
	     }
	     goto _LL1397;
    _LL1401: goto _LL1397;
    _LL1397: ;
  }
  { void *(*ptr_maker)(void *t,void *rgn,struct Cyc_Absyn_Tqual tq) = Cyc_Absyn_at_typ;
    if (topt != 0) {
      void *_temp1415 = Cyc_Tcutil_compress(({ void **_temp1414 = topt;
					      if (_temp1414 == 0) {
						_throw(Null_Exception);
					      }
					      *_temp1414; }));
      struct Cyc_Absyn_PtrInfo _temp1421;
      struct Cyc_Absyn_Conref *_temp1423;
      struct Cyc_Absyn_Tqual _temp1425;
      struct Cyc_Absyn_Conref *_temp1427;
      void *_temp1429;
      void *_temp1431;
      _LL1417: if ((unsigned int)_temp1415 > 4 ? *((int *)_temp1415) == Cyc_Absyn_PointerType : 0) {
	_LL1422: _temp1421 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp1415)->f1;
		 _LL1432: _temp1431 = (void *)_temp1421.elt_typ;
			  goto _LL1430;
		 _LL1430: _temp1429 = (void *)_temp1421.rgn_typ;
			  goto _LL1428;
		 _LL1428: _temp1427 = (struct Cyc_Absyn_Conref *)_temp1421.nullable;
			  goto _LL1426;
		 _LL1426: _temp1425 = (struct Cyc_Absyn_Tqual)_temp1421.tq;
			  goto _LL1424;
		 _LL1424: _temp1423 = (struct Cyc_Absyn_Conref *)_temp1421.bounds;
			  goto _LL1418;
      }
      else {
	goto _LL1419;
      }
      _LL1419: goto _LL1420;
      _LL1418: if (((int (*)(struct Cyc_Absyn_Conref *x))Cyc_Absyn_conref_val)(_temp1427)) {
		 ptr_maker = Cyc_Absyn_star_typ;
	       }
	       goto _LL1416;
      _LL1420: goto _LL1416;
      _LL1416: ;
    }
    return ptr_maker(t,rgn,Cyc_Absyn_empty_tqual());}
  
}
static void *Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				 void **topt,struct Cyc_Absyn_Stmt *s) {
  Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(te,s),s,1);
  Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
  while (1) {
    void *_temp1433 = (void *)s->r;
    struct Cyc_Absyn_Exp *_temp1443;
    struct Cyc_Absyn_Stmt *_temp1445;
    struct Cyc_Absyn_Stmt *_temp1447;
    struct Cyc_Absyn_Stmt *_temp1449;
    struct Cyc_Absyn_Decl *_temp1451;
    _LL1435: if ((unsigned int)_temp1433 > 1 ? *((int *)_temp1433) == Cyc_Absyn_Exp_s : 0) {
      _LL1444: _temp1443 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Exp_s_struct *)_temp1433)->f1;
	       goto _LL1436;
    }
    else {
      goto _LL1437;
    }
    _LL1437: if ((unsigned int)_temp1433 > 1 ? *((int *)_temp1433) == Cyc_Absyn_Seq_s : 0) {
      _LL1448: _temp1447 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Seq_s_struct *)_temp1433)->f1;
	       goto _LL1446;
      _LL1446: _temp1445 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Seq_s_struct *)_temp1433)->f2;
	       goto _LL1438;
    }
    else {
      goto _LL1439;
    }
    _LL1439: if ((unsigned int)_temp1433 > 1 ? *((int *)_temp1433) == Cyc_Absyn_Decl_s : 0) {
      _LL1452: _temp1451 = (struct Cyc_Absyn_Decl *)((struct Cyc_Absyn_Decl_s_struct *)_temp1433)->f1;
	       goto _LL1450;
      _LL1450: _temp1449 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Decl_s_struct *)_temp1433)->f2;
	       goto _LL1440;
    }
    else {
      goto _LL1441;
    }
    _LL1441: goto _LL1442;
    _LL1436: return (void *)({ struct Cyc_Core_Opt *_temp1453 = _temp1443->topt;
      if (_temp1453 == 0) {
	_throw(Null_Exception);
      }
      _temp1453->v; });
    _LL1438: s = _temp1445;
	     continue;
    _LL1440: s = _temp1449;
	     continue;
    _LL1442: return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp1454 = (char *)"statement expression must end with expression";
					 struct _tagged_string _temp1455;
					 _temp1455.curr = _temp1454;
					 _temp1455.base = _temp1454;
					 _temp1455.last_plus_one = _temp1454 + 46;
					 _temp1455; }));
    _LL1434: ;
  }
}
static void *Cyc_Tcexp_tcCodegen(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
				 void **topt,struct Cyc_Absyn_Fndecl *fd) {
  return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp1456 = (char *)"tcCodegen";
			      struct _tagged_string _temp1457;
			      _temp1457.curr = _temp1456;
			      _temp1457.base = _temp1456;
			      _temp1457.last_plus_one = _temp1456 + 10;
			      _temp1457; }));
}
static void *Cyc_Tcexp_tcFill(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
			      void **topt,struct Cyc_Absyn_Exp *e) {
  return Cyc_Tcexp_expr_err(te,loc,(struct _tagged_string)({ char *_temp1458 = (char *)"tcFill";
			      struct _tagged_string _temp1459;
			      _temp1459.curr = _temp1458;
			      _temp1459.base = _temp1458;
			      _temp1459.last_plus_one = _temp1458 + 7;
			      _temp1459; }));
}
static void *Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv *te,struct Cyc_Position_Segment *loc,
			     void **topt,struct Cyc_Absyn_Exp *rgn_handle,
			     struct Cyc_Absyn_Exp *e,struct Cyc_Absyn_Exp *e1) {
  void *rgn = (void *)Cyc_Absyn_HeapRgn;
  if (rgn_handle != 0) {
    void *expected_type = (void *)({ struct Cyc_Absyn_RgnHandleType_struct *_temp1471 = (struct Cyc_Absyn_RgnHandleType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));
      _temp1471[0] = ({ struct Cyc_Absyn_RgnHandleType_struct _temp1472;
	_temp1472.tag = Cyc_Absyn_RgnHandleType;
	_temp1472.f1 = (void *)Cyc_Absyn_new_evar((void *)Cyc_Absyn_RgnKind);
	_temp1472; });
      _temp1471; });
    void *handle_type = Cyc_Tcexp_tcExp(te,(void **)&expected_type,(struct Cyc_Absyn_Exp *)({ struct Cyc_Absyn_Exp *_temp1470 = rgn_handle;
					  if (_temp1470 == 0) {
					    _throw(Null_Exception);
					  }
					  _temp1470; }));
    void *_temp1460 = Cyc_Tcutil_compress(handle_type);
    void *_temp1466;
    _LL1462: if ((unsigned int)_temp1460 > 4 ? *((int *)_temp1460) == Cyc_Absyn_RgnHandleType : 0) {
      _LL1467: _temp1466 = (void *)((struct Cyc_Absyn_RgnHandleType_struct *)_temp1460)->f1;
	       goto _LL1463;
    }
    else {
      goto _LL1464;
    }
    _LL1464: goto _LL1465;
    _LL1463: rgn = _temp1466;
	     Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
	     goto _LL1461;
    _LL1465: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp *_temp1468 = rgn_handle;
			       if (_temp1468 == 0) {
				 _throw(Null_Exception);
			       }
			       _temp1468->loc; }),({ struct _tagged_string _temp1469 = Cyc_Absynpp_typ2string(handle_type);
			       xprintf("expecting region_t type but found %.*s",
				       _temp1469.last_plus_one - _temp1469.curr,
				       _temp1469.curr); }));
	     goto _LL1461;
    _LL1461: ;
  }
  { void *_temp1473 = (void *)e1->r;
    struct Cyc_Absyn_Exp *_temp1485;
    struct Cyc_Absyn_Exp *_temp1487;
    struct Cyc_Absyn_Vardecl *_temp1489;
    struct Cyc_List_List *_temp1491;
    struct Cyc_Core_Opt *_temp1493;
    struct Cyc_List_List *_temp1495;
    void *_temp1497;
    struct _tagged_string _temp1499;
    _LL1475: if (*((int *)_temp1473) == Cyc_Absyn_Comprehension_e) {
      _LL1490: _temp1489 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Comprehension_e_struct *)_temp1473)->f1;
	       goto _LL1488;
      _LL1488: _temp1487 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Comprehension_e_struct *)_temp1473)->f2;
	       goto _LL1486;
      _LL1486: _temp1485 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Comprehension_e_struct *)_temp1473)->f3;
	       goto _LL1476;
    }
    else {
      goto _LL1477;
    }
    _LL1477: if (*((int *)_temp1473) == Cyc_Absyn_UnresolvedMem_e) {
      _LL1494: _temp1493 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_UnresolvedMem_e_struct *)_temp1473)->f1;
	       goto _LL1492;
      _LL1492: _temp1491 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnresolvedMem_e_struct *)_temp1473)->f2;
	       goto _LL1478;
    }
    else {
      goto _LL1479;
    }
    _LL1479: if (*((int *)_temp1473) == Cyc_Absyn_Array_e) {
      _LL1496: _temp1495 = (struct Cyc_List_List *)((struct Cyc_Absyn_Array_e_struct *)_temp1473)->f1;
	       goto _LL1480;
    }
    else {
      goto _LL1481;
    }
    _LL1481: if (*((int *)_temp1473) == Cyc_Absyn_Const_e) {
      _LL1498: _temp1497 = (void *)((struct Cyc_Absyn_Const_e_struct *)_temp1473)->f1;
	       if ((unsigned int)_temp1497 > 1 ? *((int *)_temp1497) == Cyc_Absyn_String_c : 0) {
		 _LL1500: _temp1499 = (struct _tagged_string)((struct Cyc_Absyn_String_c_struct *)_temp1497)->f1;
			  goto _LL1482;
	       }
	       else {
		 goto _LL1483;
	       }
    }
    else {
      goto _LL1483;
    }
    _LL1483: goto _LL1484;
    _LL1476: {
    void *t_arr = Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
      void *_temp1501 = Cyc_Tcutil_compress(t_arr);
      struct Cyc_Absyn_Exp *_temp1507;
      struct Cyc_Absyn_Tqual _temp1509;
      void *_temp1511;
      _LL1503: if ((unsigned int)_temp1501 > 4 ? *((int *)_temp1501) == Cyc_Absyn_ArrayType : 0) {
	_LL1512: _temp1511 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp1501)->f1;
		 goto _LL1510;
	_LL1510: _temp1509 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp1501)->f2;
		 goto _LL1508;
	_LL1508: _temp1507 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp1501)->f3;
		 goto _LL1504;
      }
      else {
	goto _LL1505;
      }
      _LL1505: goto _LL1506;
      _LL1504: {
      void *b = Cyc_Tcutil_is_const_exp(te,_temp1487) ? (void *)({ struct Cyc_Absyn_Upper_b_struct *_temp1521 = (struct Cyc_Absyn_Upper_b_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
	  _temp1521[0] = ({ struct Cyc_Absyn_Upper_b_struct _temp1522;
	    _temp1522.tag = Cyc_Absyn_Upper_b;
	    _temp1522.f1 = _temp1487;
	    _temp1522; });
	  _temp1521; }) : (void *)Cyc_Absyn_Unknown_b;
	void *res_typ = (void *)({ struct Cyc_Absyn_PointerType_struct *_temp1518 = (struct Cyc_Absyn_PointerType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_PointerType_struct));
	  _temp1518[0] = ({ struct Cyc_Absyn_PointerType_struct _temp1519;
	    _temp1519.tag = Cyc_Absyn_PointerType;
	    _temp1519.f1 = ({ struct Cyc_Absyn_PtrInfo _temp1520;
	      _temp1520.elt_typ = (void *)_temp1511;
	      _temp1520.rgn_typ = (void *)rgn;
	      _temp1520.nullable = ((struct Cyc_Absyn_Conref *(*)())Cyc_Absyn_empty_conref)();
	      _temp1520.tq = _temp1509;
	      _temp1520.bounds = ((struct Cyc_Absyn_Conref *(*)(void *x))Cyc_Absyn_new_conref)(b);
	      _temp1520; });
	    _temp1519; });
	  _temp1518; });
	if (topt != 0) {
	  if (! Cyc_Tcutil_unify(({ void **_temp1513 = topt;
				   if (_temp1513 == 0) {
				     _throw(Null_Exception);
				   }
				   *_temp1513; }),res_typ) ? Cyc_Tcutil_silent_castable(te,
											loc,
											res_typ,
											({ void **_temp1514 = topt;
											  if (_temp1514 == 0) {
											    _throw(Null_Exception);
											  }
											  *_temp1514; })) : 0) {
	    e->topt = ({ struct Cyc_Core_Opt *_temp1515 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
	      _temp1515->v = (void *)res_typ;
	      _temp1515; });
	    Cyc_Tcutil_unchecked_cast(te,e,({ void **_temp1516 = topt;
					if (_temp1516 == 0) {
					  _throw(Null_Exception);
					}
					*_temp1516; }));
	    res_typ = ({ void **_temp1517 = topt;
	      if (_temp1517 == 0) {
		_throw(Null_Exception);
	      }
	      *_temp1517; });
	  }
	}
	return res_typ;
      }
      _LL1506: return ((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp1523 = (char *)"tcNew: comprehension returned non-array type";
									     struct _tagged_string _temp1524;
									     _temp1524.curr = _temp1523;
									     _temp1524.base = _temp1523;
									     _temp1524.last_plus_one = _temp1523 + 45;
									     _temp1524; }));
      _LL1502: ;
    }
    _LL1478: (void *)(e1->r = (void *)((void *)({ struct Cyc_Absyn_Array_e_struct *_temp1525 = (struct Cyc_Absyn_Array_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Array_e_struct));
	       _temp1525[0] = ({ struct Cyc_Absyn_Array_e_struct _temp1526;
		 _temp1526.tag = Cyc_Absyn_Array_e;
		 _temp1526.f1 = _temp1491;
		 _temp1526; });
	       _temp1525; })));
	     _temp1495 = _temp1491;
	     goto _LL1480;
    _LL1480: {
    void **elt_typ_opt = 0;
      if (topt != 0) {
	void *_temp1528 = Cyc_Tcutil_compress(({ void **_temp1527 = topt;
						if (_temp1527 == 0) {
						  _throw(Null_Exception);
						}
						*_temp1527; }));
	struct Cyc_Absyn_PtrInfo _temp1534;
	struct Cyc_Absyn_Conref *_temp1536;
	struct Cyc_Absyn_Tqual _temp1538;
	struct Cyc_Absyn_Conref *_temp1540;
	void *_temp1542;
	void *_temp1544;
	void **_temp1546;
	_LL1530: if ((unsigned int)_temp1528 > 4 ? *((int *)_temp1528) == Cyc_Absyn_PointerType : 0) {
	  _LL1535: _temp1534 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp1528)->f1;
		   _LL1545: _temp1544 = (void *)_temp1534.elt_typ;
			    _temp1546 = &(((struct Cyc_Absyn_PointerType_struct *)_temp1528)->f1).elt_typ;
			    goto _LL1543;
		   _LL1543: _temp1542 = (void *)_temp1534.rgn_typ;
			    goto _LL1541;
		   _LL1541: _temp1540 = (struct Cyc_Absyn_Conref *)_temp1534.nullable;
			    goto _LL1539;
		   _LL1539: _temp1538 = (struct Cyc_Absyn_Tqual)_temp1534.tq;
			    goto _LL1537;
		   _LL1537: _temp1536 = (struct Cyc_Absyn_Conref *)_temp1534.bounds;
			    goto _LL1531;
	}
	else {
	  goto _LL1532;
	}
	_LL1532: goto _LL1533;
	_LL1531: elt_typ_opt = (void **)_temp1546;
		 goto _LL1529;
	_LL1533: goto _LL1529;
	_LL1529: ;
      }
      { void *t_array = Cyc_Tcexp_tcExpNoPromote(te,elt_typ_opt,e1);
	void *res_typ;
	{ void *_temp1547 = Cyc_Tcutil_compress(t_array);
	  struct Cyc_Absyn_Exp *_temp1553;
	  struct Cyc_Absyn_Tqual _temp1555;
	  void *_temp1557;
	  _LL1549: if ((unsigned int)_temp1547 > 4 ? *((int *)_temp1547) == Cyc_Absyn_ArrayType : 0) {
	    _LL1558: _temp1557 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp1547)->f1;
		     goto _LL1556;
	    _LL1556: _temp1555 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp1547)->f2;
		     goto _LL1554;
	    _LL1554: _temp1553 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp1547)->f3;
		     goto _LL1550;
	  }
	  else {
	    goto _LL1551;
	  }
	  _LL1551: goto _LL1552;
	  _LL1550: res_typ = (void *)({ struct Cyc_Absyn_PointerType_struct *_temp1559 = (struct Cyc_Absyn_PointerType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_PointerType_struct));
		     _temp1559[0] = ({ struct Cyc_Absyn_PointerType_struct _temp1560;
		       _temp1560.tag = Cyc_Absyn_PointerType;
		       _temp1560.f1 = ({ struct Cyc_Absyn_PtrInfo _temp1561;
			 _temp1561.elt_typ = (void *)_temp1557;
			 _temp1561.rgn_typ = (void *)rgn;
			 _temp1561.nullable = ((struct Cyc_Absyn_Conref *(*)())Cyc_Absyn_empty_conref)();
			 _temp1561.tq = _temp1555;
			 _temp1561.bounds = ((struct Cyc_Absyn_Conref *(*)(void *x))Cyc_Absyn_new_conref)((void *)({ struct Cyc_Absyn_Upper_b_struct *_temp1562 = (struct Cyc_Absyn_Upper_b_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
													    _temp1562[0] = ({ struct Cyc_Absyn_Upper_b_struct _temp1563;
													      _temp1563.tag = Cyc_Absyn_Upper_b;
													      _temp1563.f1 = (struct Cyc_Absyn_Exp *)({ struct Cyc_Absyn_Exp *_temp1564 = _temp1553;
														if (_temp1564 == 0) {
														  _throw(Null_Exception);
														}
														_temp1564; });
													      _temp1563; });
													    _temp1562; }));
			 _temp1561; });
		       _temp1560; });
		     _temp1559; });
		   if (topt != 0) {
		     if (! Cyc_Tcutil_unify(({ void **_temp1565 = topt;
					      if (_temp1565 == 0) {
						_throw(Null_Exception);
					      }
					      *_temp1565; }),res_typ) ? Cyc_Tcutil_silent_castable(te,
												   loc,
												   res_typ,
												   ({ void **_temp1566 = topt;
												     if (_temp1566 == 0) {
												       _throw(Null_Exception);
												     }
												     *_temp1566; })) : 0) {
		       e->topt = ({ struct Cyc_Core_Opt *_temp1567 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
			 _temp1567->v = (void *)res_typ;
			 _temp1567; });
		       Cyc_Tcutil_unchecked_cast(te,e,({ void **_temp1568 = topt;
						   if (_temp1568 == 0) {
						     _throw(Null_Exception);
						   }
						   *_temp1568; }));
		       res_typ = ({ void **_temp1569 = topt;
			 if (_temp1569 == 0) {
			   _throw(Null_Exception);
			 }
			 *_temp1569; });
		     }
		   }
		   goto _LL1548;
	  _LL1552: return ((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp1570 = (char *)"tcExpNoPromote on Array_e returned non-array type";
										 struct _tagged_string _temp1571;
										 _temp1571.curr = _temp1570;
										 _temp1571.base = _temp1570;
										 _temp1571.last_plus_one = _temp1570 + 50;
										 _temp1571; }));
	  _LL1548: ;
	}
	return res_typ;}
      
    }
    _LL1482: {
    void *topt2 = Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,rgn,Cyc_Absyn_empty_tqual(),
				    (void *)Cyc_Absyn_Unknown_b);
      void *t = Cyc_Tcexp_tcExp(te,(void **)&topt2,e1);
      return Cyc_Absyn_atb_typ(t,rgn,Cyc_Absyn_empty_tqual(),(void *)({ struct Cyc_Absyn_Upper_b_struct *_temp1572 = (struct Cyc_Absyn_Upper_b_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
				 _temp1572[0] = ({ struct Cyc_Absyn_Upper_b_struct _temp1573;
				   _temp1573.tag = Cyc_Absyn_Upper_b;
				   _temp1573.f1 = Cyc_Absyn_uint_exp(1,0);
				   _temp1573; });
				 _temp1572; }));
    }
    _LL1484: {
    void **topt2 = 0;
      if (topt != 0) {
	void *_temp1575 = Cyc_Tcutil_compress(({ void **_temp1574 = topt;
						if (_temp1574 == 0) {
						  _throw(Null_Exception);
						}
						*_temp1574; }));
	struct Cyc_Absyn_PtrInfo _temp1583;
	struct Cyc_Absyn_Conref *_temp1585;
	struct Cyc_Absyn_Tqual _temp1587;
	struct Cyc_Absyn_Conref *_temp1589;
	void *_temp1591;
	void *_temp1593;
	void **_temp1595;
	struct Cyc_Absyn_TunionInfo _temp1596;
	_LL1577: if ((unsigned int)_temp1575 > 4 ? *((int *)_temp1575) == Cyc_Absyn_PointerType : 0) {
	  _LL1584: _temp1583 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp1575)->f1;
		   _LL1594: _temp1593 = (void *)_temp1583.elt_typ;
			    _temp1595 = &(((struct Cyc_Absyn_PointerType_struct *)_temp1575)->f1).elt_typ;
			    goto _LL1592;
		   _LL1592: _temp1591 = (void *)_temp1583.rgn_typ;
			    goto _LL1590;
		   _LL1590: _temp1589 = (struct Cyc_Absyn_Conref *)_temp1583.nullable;
			    goto _LL1588;
		   _LL1588: _temp1587 = (struct Cyc_Absyn_Tqual)_temp1583.tq;
			    goto _LL1586;
		   _LL1586: _temp1585 = (struct Cyc_Absyn_Conref *)_temp1583.bounds;
			    goto _LL1578;
	}
	else {
	  goto _LL1579;
	}
	_LL1579: if ((unsigned int)_temp1575 > 4 ? *((int *)_temp1575) == Cyc_Absyn_TunionType : 0) {
	  _LL1597: _temp1596 = (struct Cyc_Absyn_TunionInfo)((struct Cyc_Absyn_TunionType_struct *)_temp1575)->f1;
		   goto _LL1580;
	}
	else {
	  goto _LL1581;
	}
	_LL1581: goto _LL1582;
	_LL1578: topt2 = (void **)_temp1595;
		 goto _LL1576;
	_LL1580: topt2 = ({ void **_temp1598 = (void **)GC_malloc(sizeof(void *));
		   _temp1598[0] = ({ void **_temp1599 = topt;
		     if (_temp1599 == 0) {
		       _throw(Null_Exception);
		     }
		     *_temp1599; });
		   _temp1598; });
		 goto _LL1576;
	_LL1582: goto _LL1576;
	_LL1576: ;
      }
      { void *telt = Cyc_Tcexp_tcExp(te,topt2,e1);
	void *res_typ = (void *)({ struct Cyc_Absyn_PointerType_struct *_temp1605 = (struct Cyc_Absyn_PointerType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_PointerType_struct));
	  _temp1605[0] = ({ struct Cyc_Absyn_PointerType_struct _temp1606;
	    _temp1606.tag = Cyc_Absyn_PointerType;
	    _temp1606.f1 = ({ struct Cyc_Absyn_PtrInfo _temp1607;
	      _temp1607.elt_typ = (void *)telt;
	      _temp1607.rgn_typ = (void *)rgn;
	      _temp1607.nullable = ((struct Cyc_Absyn_Conref *(*)())Cyc_Absyn_empty_conref)();
	      _temp1607.tq = Cyc_Absyn_empty_tqual();
	      _temp1607.bounds = ((struct Cyc_Absyn_Conref *(*)(void *x))Cyc_Absyn_new_conref)((void *)({ struct Cyc_Absyn_Upper_b_struct *_temp1608 = (struct Cyc_Absyn_Upper_b_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
												 _temp1608[0] = ({ struct Cyc_Absyn_Upper_b_struct _temp1609;
												   _temp1609.tag = Cyc_Absyn_Upper_b;
												   _temp1609.f1 = Cyc_Absyn_uint_exp(1,
																     0);
												   _temp1609; });
												 _temp1608; }));
	      _temp1607; });
	    _temp1606; });
	  _temp1605; });
	if (topt != 0) {
	  if (! Cyc_Tcutil_unify(({ void **_temp1600 = topt;
				   if (_temp1600 == 0) {
				     _throw(Null_Exception);
				   }
				   *_temp1600; }),res_typ) ? Cyc_Tcutil_silent_castable(te,
											loc,
											res_typ,
											({ void **_temp1601 = topt;
											  if (_temp1601 == 0) {
											    _throw(Null_Exception);
											  }
											  *_temp1601; })) : 0) {
	    e->topt = ({ struct Cyc_Core_Opt *_temp1602 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
	      _temp1602->v = (void *)res_typ;
	      _temp1602; });
	    Cyc_Tcutil_unchecked_cast(te,e,({ void **_temp1603 = topt;
					if (_temp1603 == 0) {
					  _throw(Null_Exception);
					}
					*_temp1603; }));
	    res_typ = ({ void **_temp1604 = topt;
	      if (_temp1604 == 0) {
		_throw(Null_Exception);
	      }
	      *_temp1604; });
	  }
	}
	return res_typ;}
      
    }
    _LL1474: ;}
  
}
void *Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv *te,void **topt,struct Cyc_Absyn_Exp *e) {
  void *t = Cyc_Tcexp_tcExpNoPromote(te,topt,e);
  t = Cyc_Tcutil_compress(t);
  { void *_temp1610 = t;
    struct Cyc_Absyn_Exp *_temp1616;
    struct Cyc_Absyn_Tqual _temp1618;
    void *_temp1620;
    _LL1612: if ((unsigned int)_temp1610 > 4 ? *((int *)_temp1610) == Cyc_Absyn_ArrayType : 0) {
      _LL1621: _temp1620 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp1610)->f1;
	       goto _LL1619;
      _LL1619: _temp1618 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp1610)->f2;
	       goto _LL1617;
      _LL1617: _temp1616 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp1610)->f3;
	       goto _LL1613;
    }
    else {
      goto _LL1614;
    }
    _LL1614: goto _LL1615;
    _LL1613: {
    void *_temp1624;
      int _temp1626;
      struct _tuple5 _temp1622 = Cyc_Tcutil_addressof_props(te,e);
      _LL1627: _temp1626 = _temp1622.f1;
	       goto _LL1625;
      _LL1625: _temp1624 = _temp1622.f2;
	       goto _LL1623;
      _LL1623: {
      void *b = _temp1616 == 0 ? (void *)Cyc_Absyn_Unknown_b : (void *)({ struct Cyc_Absyn_Upper_b_struct *_temp1628 = (struct Cyc_Absyn_Upper_b_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
	  _temp1628[0] = ({ struct Cyc_Absyn_Upper_b_struct _temp1629;
	    _temp1629.tag = Cyc_Absyn_Upper_b;
	    _temp1629.f1 = (struct Cyc_Absyn_Exp *)({ struct Cyc_Absyn_Exp *_temp1630 = _temp1616;
	      if (_temp1630 == 0) {
		_throw(Null_Exception);
	      }
	      _temp1630; });
	    _temp1629; });
	  _temp1628; });
	t = Cyc_Absyn_atb_typ(_temp1620,_temp1624,_temp1618,b);
	Cyc_Tcutil_unchecked_cast(te,e,t);
	return t;
      }
    }
    _LL1615: return t;
    _LL1611: ;}
  
}
void *Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv *te,void **topt,struct Cyc_Absyn_Exp *e) {
  void *t = Cyc_Tcexp_tcExpNoPromote(te,topt,e);
  void *_temp1631 = (void *)e->r;
  struct Cyc_List_List *_temp1641;
  struct Cyc_Absyn_Exp *_temp1643;
  struct Cyc_Absyn_Exp *_temp1645;
  struct Cyc_Absyn_Vardecl *_temp1647;
  void *_temp1649;
  struct _tagged_string _temp1651;
  _LL1633: if (*((int *)_temp1631) == Cyc_Absyn_Array_e) {
    _LL1642: _temp1641 = (struct Cyc_List_List *)((struct Cyc_Absyn_Array_e_struct *)_temp1631)->f1;
	     goto _LL1634;
  }
  else {
    goto _LL1635;
  }
  _LL1635: if (*((int *)_temp1631) == Cyc_Absyn_Comprehension_e) {
    _LL1648: _temp1647 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Comprehension_e_struct *)_temp1631)->f1;
	     goto _LL1646;
    _LL1646: _temp1645 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Comprehension_e_struct *)_temp1631)->f2;
	     goto _LL1644;
    _LL1644: _temp1643 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Comprehension_e_struct *)_temp1631)->f3;
	     goto _LL1636;
  }
  else {
    goto _LL1637;
  }
  _LL1637: if (*((int *)_temp1631) == Cyc_Absyn_Const_e) {
    _LL1650: _temp1649 = (void *)((struct Cyc_Absyn_Const_e_struct *)_temp1631)->f1;
	     if ((unsigned int)_temp1649 > 1 ? *((int *)_temp1649) == Cyc_Absyn_String_c : 0) {
	       _LL1652: _temp1651 = (struct _tagged_string)((struct Cyc_Absyn_String_c_struct *)_temp1649)->f1;
			goto _LL1638;
	     }
	     else {
	       goto _LL1639;
	     }
  }
  else {
    goto _LL1639;
  }
  _LL1639: goto _LL1640;
  _LL1634: return t;
  _LL1636: return t;
  _LL1638: return t;
  _LL1640: t = Cyc_Tcutil_compress(t);
  { void *_temp1653 = t;
    struct Cyc_Absyn_Exp *_temp1659;
    struct Cyc_Absyn_Tqual _temp1661;
    void *_temp1663;
    _LL1655: if ((unsigned int)_temp1653 > 4 ? *((int *)_temp1653) == Cyc_Absyn_ArrayType : 0) {
      _LL1664: _temp1663 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp1653)->f1;
	       goto _LL1662;
      _LL1662: _temp1661 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp1653)->f2;
	       goto _LL1660;
      _LL1660: _temp1659 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp1653)->f3;
	       goto _LL1656;
    }
    else {
      goto _LL1657;
    }
    _LL1657: goto _LL1658;
    _LL1656: {
    void *_temp1667;
      int _temp1669;
      struct _tuple5 _temp1665 = Cyc_Tcutil_addressof_props(te,e);
      _LL1670: _temp1669 = _temp1665.f1;
	       goto _LL1668;
      _LL1668: _temp1667 = _temp1665.f2;
	       goto _LL1666;
      _LL1666: {
      void *b = _temp1659 == 0 ? (void *)Cyc_Absyn_Unknown_b : (void *)({ struct Cyc_Absyn_Upper_b_struct *_temp1671 = (struct Cyc_Absyn_Upper_b_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
	  _temp1671[0] = ({ struct Cyc_Absyn_Upper_b_struct _temp1672;
	    _temp1672.tag = Cyc_Absyn_Upper_b;
	    _temp1672.f1 = (struct Cyc_Absyn_Exp *)({ struct Cyc_Absyn_Exp *_temp1673 = _temp1659;
	      if (_temp1673 == 0) {
		_throw(Null_Exception);
	      }
	      _temp1673; });
	    _temp1672; });
	  _temp1671; });
	t = Cyc_Absyn_atb_typ(_temp1663,_temp1667,_temp1661,b);
	Cyc_Tcutil_unchecked_cast(te,e,t);
	return t;
      }
    }
    _LL1658: return t;
    _LL1654: ;}
  
  _LL1632: ;
}
static void *Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv *te,void **topt,
				      struct Cyc_Absyn_Exp *e) {
  { void *_temp1674 = (void *)e->r;
    struct Cyc_Absyn_Exp *_temp1680;
    _LL1676: if (*((int *)_temp1674) == Cyc_Absyn_NoInstantiate_e) {
      _LL1681: _temp1680 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_NoInstantiate_e_struct *)_temp1674)->f1;
	       goto _LL1677;
    }
    else {
      goto _LL1678;
    }
    _LL1678: goto _LL1679;
    _LL1677: Cyc_Tcexp_tcExpNoInst(te,topt,_temp1680);
	     (void *)({ struct Cyc_Core_Opt *_temp1682 = _temp1680->topt;
	       if (_temp1682 == 0) {
		 _throw(Null_Exception);
	       }
	       _temp1682->v = (void *)Cyc_Absyn_pointer_expand(Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp1683 = _temp1680->topt;
										     if (_temp1683 == 0) {
										       _throw(Null_Exception);
										     }
										     _temp1683->v; }))); });
	     e->topt = _temp1680->topt;
	     goto _LL1675;
    _LL1679: Cyc_Tcexp_tcExpNoInst(te,topt,e);
	     (void *)({ struct Cyc_Core_Opt *_temp1684 = e->topt;
	       if (_temp1684 == 0) {
		 _throw(Null_Exception);
	       }
	       _temp1684->v = (void *)Cyc_Absyn_pointer_expand(Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp1685 = e->topt;
										     if (_temp1685 == 0) {
										       _throw(Null_Exception);
										     }
										     _temp1685->v; }))); });
	     { void *_temp1687 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp1686 = e->topt;
						       if (_temp1686 == 0) {
							 _throw(Null_Exception);
						       }
						       _temp1686->v; }));
	       struct Cyc_Absyn_PtrInfo _temp1693;
	       struct Cyc_Absyn_Conref *_temp1695;
	       struct Cyc_Absyn_Tqual _temp1697;
	       struct Cyc_Absyn_Conref *_temp1699;
	       void *_temp1701;
	       void *_temp1703;
	       _LL1689: if ((unsigned int)_temp1687 > 4 ? *((int *)_temp1687) == Cyc_Absyn_PointerType : 0) {
		 _LL1694: _temp1693 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp1687)->f1;
			  _LL1704: _temp1703 = (void *)_temp1693.elt_typ;
				   goto _LL1702;
			  _LL1702: _temp1701 = (void *)_temp1693.rgn_typ;
				   goto _LL1700;
			  _LL1700: _temp1699 = (struct Cyc_Absyn_Conref *)_temp1693.nullable;
				   goto _LL1698;
			  _LL1698: _temp1697 = (struct Cyc_Absyn_Tqual)_temp1693.tq;
				   goto _LL1696;
			  _LL1696: _temp1695 = (struct Cyc_Absyn_Conref *)_temp1693.bounds;
				   goto _LL1690;
	       }
	       else {
		 goto _LL1691;
	       }
	       _LL1691: goto _LL1692;
	       _LL1690: { void *_temp1705 = Cyc_Tcutil_compress(_temp1703);
		 struct Cyc_Absyn_FnInfo _temp1711;
		 struct Cyc_List_List *_temp1713;
		 int _temp1715;
		 struct Cyc_List_List *_temp1717;
		 void *_temp1719;
		 struct Cyc_Core_Opt *_temp1721;
		 struct Cyc_List_List *_temp1723;
		 _LL1707: if ((unsigned int)_temp1705 > 4 ? *((int *)_temp1705) == Cyc_Absyn_FnType : 0) {
		   _LL1712: _temp1711 = (struct Cyc_Absyn_FnInfo)((struct Cyc_Absyn_FnType_struct *)_temp1705)->f1;
			    _LL1724: _temp1723 = (struct Cyc_List_List *)_temp1711.tvars;
				     goto _LL1722;
			    _LL1722: _temp1721 = (struct Cyc_Core_Opt *)_temp1711.effect;
				     goto _LL1720;
			    _LL1720: _temp1719 = (void *)_temp1711.ret_typ;
				     goto _LL1718;
			    _LL1718: _temp1717 = (struct Cyc_List_List *)_temp1711.args;
				     goto _LL1716;
			    _LL1716: _temp1715 = (int)_temp1711.varargs;
				     goto _LL1714;
			    _LL1714: _temp1713 = (struct Cyc_List_List *)_temp1711.attributes;
				     goto _LL1708;
		 }
		 else {
		   goto _LL1709;
		 }
		 _LL1709: goto _LL1710;
		 _LL1708: if (_temp1723 != 0) {
			    struct _RegionHandle _temp1725 = _new_region();
			    struct _RegionHandle *rgn = &_temp1725;
			    _push_region(rgn);
			    { struct Cyc_List_List *inst = ((struct Cyc_List_List *(*)(struct _RegionHandle *,
										       struct _tuple4 *(*f)(struct _RegionHandle *,
													    struct Cyc_Absyn_Tvar *),
										       struct _RegionHandle *env,
										       struct Cyc_List_List *x))Cyc_List_rmap_c)(rgn,
																 Cyc_Tcutil_r_make_inst_var,
																 rgn,
																 _temp1723);
			      struct Cyc_List_List *ts = ((struct Cyc_List_List *(*)(void *(*f)(struct _tuple4 *),
										     struct Cyc_List_List *x))Cyc_List_map)((void *(*)(struct _tuple4 *))Cyc_Core_snd,
															    inst);
			      void *ftyp = Cyc_Tcutil_rsubstitute(rgn,inst,
								  (void *)({ struct Cyc_Absyn_FnType_struct *_temp1732 = (struct Cyc_Absyn_FnType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_FnType_struct));
								    _temp1732[0] = ({ struct Cyc_Absyn_FnType_struct _temp1733;
								      _temp1733.tag = Cyc_Absyn_FnType;
								      _temp1733.f1 = ({ struct Cyc_Absyn_FnInfo _temp1734;
									_temp1734.tvars = 0;
									_temp1734.effect = _temp1721;
									_temp1734.ret_typ = (void *)_temp1719;
									_temp1734.args = _temp1717;
									_temp1734.varargs = _temp1715;
									_temp1734.attributes = _temp1713;
									_temp1734; });
								      _temp1733; });
								    _temp1732; }));
			      void *new_typ = (void *)({ struct Cyc_Absyn_PointerType_struct *_temp1729 = (struct Cyc_Absyn_PointerType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_PointerType_struct));
				_temp1729[0] = ({ struct Cyc_Absyn_PointerType_struct _temp1730;
				  _temp1730.tag = Cyc_Absyn_PointerType;
				  _temp1730.f1 = ({ struct Cyc_Absyn_PtrInfo _temp1731;
				    _temp1731.elt_typ = (void *)ftyp;
				    _temp1731.rgn_typ = (void *)_temp1701;
				    _temp1731.nullable = _temp1699;
				    _temp1731.tq = _temp1697;
				    _temp1731.bounds = _temp1695;
				    _temp1731; });
				  _temp1730; });
				_temp1729; });
			      struct Cyc_Absyn_Exp *inner = Cyc_Absyn_copy_exp(e);
			      (void *)(e->r = (void *)((void *)({ struct Cyc_Absyn_Instantiate_e_struct *_temp1726 = (struct Cyc_Absyn_Instantiate_e_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Instantiate_e_struct));
				_temp1726[0] = ({ struct Cyc_Absyn_Instantiate_e_struct _temp1727;
				  _temp1727.tag = Cyc_Absyn_Instantiate_e;
				  _temp1727.f1 = inner;
				  _temp1727.f2 = ts;
				  _temp1727; });
				_temp1726; })));
			      e->topt = ({ struct Cyc_Core_Opt *_temp1728 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
				_temp1728->v = (void *)new_typ;
				_temp1728; });
			    }
			    ;
			    _pop_region(&_temp1725);
			  }
			  goto _LL1706;
		 _LL1710: goto _LL1706;
		 _LL1706: ;
	       }
	       goto _LL1688;
	       _LL1692: goto _LL1688;
	       _LL1688: ;
	     }
	     goto _LL1675;
    _LL1675: ;
  }
  return (void *)({ struct Cyc_Core_Opt *_temp1735 = e->topt;
    if (_temp1735 == 0) {
      _throw(Null_Exception);
    }
    _temp1735->v; });
}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv *te,void **topt,struct Cyc_Absyn_Exp *e) {
  struct Cyc_Position_Segment *loc = e->loc;
  void *t;
  { void *_temp1736 = (void *)e->r;
    struct Cyc_Absyn_Exp *_temp1808;
    struct _tuple1 *_temp1810;
    struct Cyc_List_List *_temp1812;
    struct Cyc_Absyn_Exp *_temp1814;
    struct Cyc_List_List *_temp1816;
    struct Cyc_Core_Opt *_temp1818;
    void *_temp1820;
    void *_temp1822;
    struct _tuple1 *_temp1824;
    struct Cyc_List_List *_temp1826;
    void *_temp1828;
    void *_temp1830;
    struct Cyc_Absyn_Exp *_temp1832;
    struct Cyc_Absyn_Exp *_temp1834;
    struct Cyc_Core_Opt *_temp1836;
    struct Cyc_Absyn_Exp *_temp1838;
    struct Cyc_Absyn_Exp *_temp1840;
    struct Cyc_Absyn_Exp *_temp1842;
    struct Cyc_Absyn_Exp *_temp1844;
    struct Cyc_Absyn_Exp *_temp1846;
    struct Cyc_Absyn_Exp *_temp1848;
    struct Cyc_List_List *_temp1850;
    struct Cyc_Absyn_Exp *_temp1852;
    struct Cyc_Absyn_Exp *_temp1854;
    struct Cyc_List_List *_temp1856;
    struct Cyc_Absyn_Exp *_temp1858;
    struct Cyc_Absyn_Exp *_temp1860;
    void *_temp1862;
    struct Cyc_Absyn_Exp *_temp1864;
    struct Cyc_Absyn_Exp *_temp1866;
    struct Cyc_Absyn_Exp *_temp1868;
    struct Cyc_Absyn_Exp *_temp1870;
    void *_temp1872;
    struct Cyc_Absyn_Exp *_temp1874;
    struct _tagged_string *_temp1876;
    struct Cyc_Absyn_Exp *_temp1878;
    struct _tagged_string *_temp1880;
    struct Cyc_Absyn_Exp *_temp1882;
    struct Cyc_Absyn_Exp *_temp1884;
    struct Cyc_Absyn_Exp *_temp1886;
    struct Cyc_List_List *_temp1888;
    struct Cyc_List_List *_temp1890;
    struct _tuple2 *_temp1892;
    struct Cyc_List_List *_temp1894;
    struct Cyc_Absyn_Stmt *_temp1896;
    struct Cyc_Absyn_Fndecl *_temp1898;
    struct Cyc_Absyn_Exp *_temp1900;
    struct Cyc_Absyn_Exp *_temp1902;
    struct Cyc_Absyn_Exp *_temp1904;
    struct Cyc_Absyn_Vardecl *_temp1906;
    struct Cyc_Absyn_Structdecl *_temp1908;
    struct Cyc_Absyn_Structdecl **_temp1910;
    struct Cyc_List_List *_temp1911;
    struct Cyc_Core_Opt *_temp1913;
    struct Cyc_Core_Opt **_temp1915;
    struct _tuple1 *_temp1916;
    struct _tuple1 **_temp1918;
    struct Cyc_List_List *_temp1919;
    void *_temp1921;
    struct Cyc_Absyn_Tunionfield *_temp1923;
    struct Cyc_Absyn_Tuniondecl *_temp1925;
    struct Cyc_List_List *_temp1927;
    struct Cyc_Core_Opt *_temp1929;
    struct Cyc_Core_Opt **_temp1931;
    struct Cyc_Core_Opt *_temp1932;
    struct Cyc_Core_Opt **_temp1934;
    struct Cyc_Absyn_Enumfield *_temp1935;
    struct Cyc_Absyn_Enumdecl *_temp1937;
    struct _tuple1 *_temp1939;
    void *_temp1941;
    struct Cyc_Absyn_Exp *_temp1943;
    _LL1738: if (*((int *)_temp1736) == Cyc_Absyn_NoInstantiate_e) {
      _LL1809: _temp1808 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_NoInstantiate_e_struct *)_temp1736)->f1;
	       goto _LL1739;
    }
    else {
      goto _LL1740;
    }
    _LL1740: if (*((int *)_temp1736) == Cyc_Absyn_UnknownId_e) {
      _LL1811: _temp1810 = (struct _tuple1 *)((struct Cyc_Absyn_UnknownId_e_struct *)_temp1736)->f1;
	       goto _LL1741;
    }
    else {
      goto _LL1742;
    }
    _LL1742: if (*((int *)_temp1736) == Cyc_Absyn_UnknownCall_e) {
      _LL1815: _temp1814 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_UnknownCall_e_struct *)_temp1736)->f1;
	       goto _LL1813;
      _LL1813: _temp1812 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownCall_e_struct *)_temp1736)->f2;
	       goto _LL1743;
    }
    else {
      goto _LL1744;
    }
    _LL1744: if (*((int *)_temp1736) == Cyc_Absyn_UnresolvedMem_e) {
      _LL1819: _temp1818 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_UnresolvedMem_e_struct *)_temp1736)->f1;
	       goto _LL1817;
      _LL1817: _temp1816 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnresolvedMem_e_struct *)_temp1736)->f2;
	       goto _LL1745;
    }
    else {
      goto _LL1746;
    }
    _LL1746: if (*((int *)_temp1736) == Cyc_Absyn_Const_e) {
      _LL1821: _temp1820 = (void *)((struct Cyc_Absyn_Const_e_struct *)_temp1736)->f1;
	       goto _LL1747;
    }
    else {
      goto _LL1748;
    }
    _LL1748: if (*((int *)_temp1736) == Cyc_Absyn_Var_e) {
      _LL1825: _temp1824 = (struct _tuple1 *)((struct Cyc_Absyn_Var_e_struct *)_temp1736)->f1;
	       goto _LL1823;
      _LL1823: _temp1822 = (void *)((struct Cyc_Absyn_Var_e_struct *)_temp1736)->f2;
	       goto _LL1749;
    }
    else {
      goto _LL1750;
    }
    _LL1750: if (*((int *)_temp1736) == Cyc_Absyn_Primop_e) {
      _LL1829: _temp1828 = (void *)((struct Cyc_Absyn_Primop_e_struct *)_temp1736)->f1;
	       goto _LL1827;
      _LL1827: _temp1826 = (struct Cyc_List_List *)((struct Cyc_Absyn_Primop_e_struct *)_temp1736)->f2;
	       goto _LL1751;
    }
    else {
      goto _LL1752;
    }
    _LL1752: if (*((int *)_temp1736) == Cyc_Absyn_Increment_e) {
      _LL1833: _temp1832 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Increment_e_struct *)_temp1736)->f1;
	       goto _LL1831;
      _LL1831: _temp1830 = (void *)((struct Cyc_Absyn_Increment_e_struct *)_temp1736)->f2;
	       goto _LL1753;
    }
    else {
      goto _LL1754;
    }
    _LL1754: if (*((int *)_temp1736) == Cyc_Absyn_AssignOp_e) {
      _LL1839: _temp1838 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_AssignOp_e_struct *)_temp1736)->f1;
	       goto _LL1837;
      _LL1837: _temp1836 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_AssignOp_e_struct *)_temp1736)->f2;
	       goto _LL1835;
      _LL1835: _temp1834 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_AssignOp_e_struct *)_temp1736)->f3;
	       goto _LL1755;
    }
    else {
      goto _LL1756;
    }
    _LL1756: if (*((int *)_temp1736) == Cyc_Absyn_Conditional_e) {
      _LL1845: _temp1844 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Conditional_e_struct *)_temp1736)->f1;
	       goto _LL1843;
      _LL1843: _temp1842 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Conditional_e_struct *)_temp1736)->f2;
	       goto _LL1841;
      _LL1841: _temp1840 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Conditional_e_struct *)_temp1736)->f3;
	       goto _LL1757;
    }
    else {
      goto _LL1758;
    }
    _LL1758: if (*((int *)_temp1736) == Cyc_Absyn_SeqExp_e) {
      _LL1849: _temp1848 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_SeqExp_e_struct *)_temp1736)->f1;
	       goto _LL1847;
      _LL1847: _temp1846 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_SeqExp_e_struct *)_temp1736)->f2;
	       goto _LL1759;
    }
    else {
      goto _LL1760;
    }
    _LL1760: if (*((int *)_temp1736) == Cyc_Absyn_FnCall_e) {
      _LL1853: _temp1852 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_FnCall_e_struct *)_temp1736)->f1;
	       goto _LL1851;
      _LL1851: _temp1850 = (struct Cyc_List_List *)((struct Cyc_Absyn_FnCall_e_struct *)_temp1736)->f2;
	       goto _LL1761;
    }
    else {
      goto _LL1762;
    }
    _LL1762: if (*((int *)_temp1736) == Cyc_Absyn_Throw_e) {
      _LL1855: _temp1854 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Throw_e_struct *)_temp1736)->f1;
	       goto _LL1763;
    }
    else {
      goto _LL1764;
    }
    _LL1764: if (*((int *)_temp1736) == Cyc_Absyn_Instantiate_e) {
      _LL1859: _temp1858 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Instantiate_e_struct *)_temp1736)->f1;
	       goto _LL1857;
      _LL1857: _temp1856 = (struct Cyc_List_List *)((struct Cyc_Absyn_Instantiate_e_struct *)_temp1736)->f2;
	       goto _LL1765;
    }
    else {
      goto _LL1766;
    }
    _LL1766: if (*((int *)_temp1736) == Cyc_Absyn_Cast_e) {
      _LL1863: _temp1862 = (void *)((struct Cyc_Absyn_Cast_e_struct *)_temp1736)->f1;
	       goto _LL1861;
      _LL1861: _temp1860 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Cast_e_struct *)_temp1736)->f2;
	       goto _LL1767;
    }
    else {
      goto _LL1768;
    }
    _LL1768: if (*((int *)_temp1736) == Cyc_Absyn_Address_e) {
      _LL1865: _temp1864 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Address_e_struct *)_temp1736)->f1;
	       goto _LL1769;
    }
    else {
      goto _LL1770;
    }
    _LL1770: if (*((int *)_temp1736) == Cyc_Absyn_New_e) {
      _LL1869: _temp1868 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_New_e_struct *)_temp1736)->f1;
	       goto _LL1867;
      _LL1867: _temp1866 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_New_e_struct *)_temp1736)->f2;
	       goto _LL1771;
    }
    else {
      goto _LL1772;
    }
    _LL1772: if (*((int *)_temp1736) == Cyc_Absyn_Sizeofexp_e) {
      _LL1871: _temp1870 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Sizeofexp_e_struct *)_temp1736)->f1;
	       goto _LL1773;
    }
    else {
      goto _LL1774;
    }
    _LL1774: if (*((int *)_temp1736) == Cyc_Absyn_Sizeoftyp_e) {
      _LL1873: _temp1872 = (void *)((struct Cyc_Absyn_Sizeoftyp_e_struct *)_temp1736)->f1;
	       goto _LL1775;
    }
    else {
      goto _LL1776;
    }
    _LL1776: if (*((int *)_temp1736) == Cyc_Absyn_Deref_e) {
      _LL1875: _temp1874 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Deref_e_struct *)_temp1736)->f1;
	       goto _LL1777;
    }
    else {
      goto _LL1778;
    }
    _LL1778: if (*((int *)_temp1736) == Cyc_Absyn_StructMember_e) {
      _LL1879: _temp1878 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_StructMember_e_struct *)_temp1736)->f1;
	       goto _LL1877;
      _LL1877: _temp1876 = (struct _tagged_string *)((struct Cyc_Absyn_StructMember_e_struct *)_temp1736)->f2;
	       goto _LL1779;
    }
    else {
      goto _LL1780;
    }
    _LL1780: if (*((int *)_temp1736) == Cyc_Absyn_StructArrow_e) {
      _LL1883: _temp1882 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_StructArrow_e_struct *)_temp1736)->f1;
	       goto _LL1881;
      _LL1881: _temp1880 = (struct _tagged_string *)((struct Cyc_Absyn_StructArrow_e_struct *)_temp1736)->f2;
	       goto _LL1781;
    }
    else {
      goto _LL1782;
    }
    _LL1782: if (*((int *)_temp1736) == Cyc_Absyn_Subscript_e) {
      _LL1887: _temp1886 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Subscript_e_struct *)_temp1736)->f1;
	       goto _LL1885;
      _LL1885: _temp1884 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Subscript_e_struct *)_temp1736)->f2;
	       goto _LL1783;
    }
    else {
      goto _LL1784;
    }
    _LL1784: if (*((int *)_temp1736) == Cyc_Absyn_Tuple_e) {
      _LL1889: _temp1888 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tuple_e_struct *)_temp1736)->f1;
	       goto _LL1785;
    }
    else {
      goto _LL1786;
    }
    _LL1786: if (*((int *)_temp1736) == Cyc_Absyn_CompoundLit_e) {
      _LL1893: _temp1892 = (struct _tuple2 *)((struct Cyc_Absyn_CompoundLit_e_struct *)_temp1736)->f1;
	       goto _LL1891;
      _LL1891: _temp1890 = (struct Cyc_List_List *)((struct Cyc_Absyn_CompoundLit_e_struct *)_temp1736)->f2;
	       goto _LL1787;
    }
    else {
      goto _LL1788;
    }
    _LL1788: if (*((int *)_temp1736) == Cyc_Absyn_Array_e) {
      _LL1895: _temp1894 = (struct Cyc_List_List *)((struct Cyc_Absyn_Array_e_struct *)_temp1736)->f1;
	       goto _LL1789;
    }
    else {
      goto _LL1790;
    }
    _LL1790: if (*((int *)_temp1736) == Cyc_Absyn_StmtExp_e) {
      _LL1897: _temp1896 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_StmtExp_e_struct *)_temp1736)->f1;
	       goto _LL1791;
    }
    else {
      goto _LL1792;
    }
    _LL1792: if (*((int *)_temp1736) == Cyc_Absyn_Codegen_e) {
      _LL1899: _temp1898 = (struct Cyc_Absyn_Fndecl *)((struct Cyc_Absyn_Codegen_e_struct *)_temp1736)->f1;
	       goto _LL1793;
    }
    else {
      goto _LL1794;
    }
    _LL1794: if (*((int *)_temp1736) == Cyc_Absyn_Fill_e) {
      _LL1901: _temp1900 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Fill_e_struct *)_temp1736)->f1;
	       goto _LL1795;
    }
    else {
      goto _LL1796;
    }
    _LL1796: if (*((int *)_temp1736) == Cyc_Absyn_Comprehension_e) {
      _LL1907: _temp1906 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Comprehension_e_struct *)_temp1736)->f1;
	       goto _LL1905;
      _LL1905: _temp1904 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Comprehension_e_struct *)_temp1736)->f2;
	       goto _LL1903;
      _LL1903: _temp1902 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Comprehension_e_struct *)_temp1736)->f3;
	       goto _LL1797;
    }
    else {
      goto _LL1798;
    }
    _LL1798: if (*((int *)_temp1736) == Cyc_Absyn_Struct_e) {
      _LL1917: _temp1916 = (struct _tuple1 *)((struct Cyc_Absyn_Struct_e_struct *)_temp1736)->f1;
	       _temp1918 = &((struct Cyc_Absyn_Struct_e_struct *)_temp1736)->f1;
	       goto _LL1914;
      _LL1914: _temp1913 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Struct_e_struct *)_temp1736)->f2;
	       _temp1915 = &((struct Cyc_Absyn_Struct_e_struct *)_temp1736)->f2;
	       goto _LL1912;
      _LL1912: _temp1911 = (struct Cyc_List_List *)((struct Cyc_Absyn_Struct_e_struct *)_temp1736)->f3;
	       goto _LL1909;
      _LL1909: _temp1908 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Absyn_Struct_e_struct *)_temp1736)->f4;
	       _temp1910 = &((struct Cyc_Absyn_Struct_e_struct *)_temp1736)->f4;
	       goto _LL1799;
    }
    else {
      goto _LL1800;
    }
    _LL1800: if (*((int *)_temp1736) == Cyc_Absyn_AnonStruct_e) {
      _LL1922: _temp1921 = (void *)((struct Cyc_Absyn_AnonStruct_e_struct *)_temp1736)->f1;
	       goto _LL1920;
      _LL1920: _temp1919 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonStruct_e_struct *)_temp1736)->f2;
	       goto _LL1801;
    }
    else {
      goto _LL1802;
    }
    _LL1802: if (*((int *)_temp1736) == Cyc_Absyn_Tunion_e) {
      _LL1933: _temp1932 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Tunion_e_struct *)_temp1736)->f1;
	       _temp1934 = &((struct Cyc_Absyn_Tunion_e_struct *)_temp1736)->f1;
	       goto _LL1930;
      _LL1930: _temp1929 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Tunion_e_struct *)_temp1736)->f2;
	       _temp1931 = &((struct Cyc_Absyn_Tunion_e_struct *)_temp1736)->f2;
	       goto _LL1928;
      _LL1928: _temp1927 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_e_struct *)_temp1736)->f3;
	       goto _LL1926;
      _LL1926: _temp1925 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_Tunion_e_struct *)_temp1736)->f4;
	       goto _LL1924;
      _LL1924: _temp1923 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Absyn_Tunion_e_struct *)_temp1736)->f5;
	       goto _LL1803;
    }
    else {
      goto _LL1804;
    }
    _LL1804: if (*((int *)_temp1736) == Cyc_Absyn_Enum_e) {
      _LL1940: _temp1939 = (struct _tuple1 *)((struct Cyc_Absyn_Enum_e_struct *)_temp1736)->f1;
	       goto _LL1938;
      _LL1938: _temp1937 = (struct Cyc_Absyn_Enumdecl *)((struct Cyc_Absyn_Enum_e_struct *)_temp1736)->f2;
	       goto _LL1936;
      _LL1936: _temp1935 = (struct Cyc_Absyn_Enumfield *)((struct Cyc_Absyn_Enum_e_struct *)_temp1736)->f3;
	       goto _LL1805;
    }
    else {
      goto _LL1806;
    }
    _LL1806: if (*((int *)_temp1736) == Cyc_Absyn_Malloc_e) {
      _LL1944: _temp1943 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Malloc_e_struct *)_temp1736)->f1;
	       goto _LL1942;
      _LL1942: _temp1941 = (void *)((struct Cyc_Absyn_Malloc_e_struct *)_temp1736)->f2;
	       goto _LL1807;
    }
    else {
      goto _LL1737;
    }
    _LL1739: Cyc_Tcexp_tcExpNoInst(te,0,_temp1808);
	     return;
    _LL1741: Cyc_Tcexp_resolve_unknown_id(te,e,_temp1810);
	     Cyc_Tcexp_tcExpNoInst(te,topt,e);
	     return;
    _LL1743: Cyc_Tcexp_resolve_unknown_fn(te,e,_temp1814,_temp1812);
	     Cyc_Tcexp_tcExpNoInst(te,topt,e);
	     return;
    _LL1745: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_temp1816);
	     Cyc_Tcexp_tcExpNoInst(te,topt,e);
	     return;
    _LL1747: t = Cyc_Tcexp_tcConst(te,loc,topt,_temp1820,e);
	     goto _LL1737;
    _LL1749: t = Cyc_Tcexp_tcVar(te,loc,_temp1824,_temp1822);
	     goto _LL1737;
    _LL1751: t = Cyc_Tcexp_tcPrimop(te,loc,topt,_temp1828,_temp1826);
	     goto _LL1737;
    _LL1753: t = Cyc_Tcexp_tcIncrement(te,loc,topt,_temp1832,_temp1830);
	     goto _LL1737;
    _LL1755: t = Cyc_Tcexp_tcAssignOp(te,loc,topt,_temp1838,_temp1836,_temp1834);
	     goto _LL1737;
    _LL1757: t = Cyc_Tcexp_tcConditional(te,loc,topt,_temp1844,_temp1842,
					 _temp1840);
	     goto _LL1737;
    _LL1759: t = Cyc_Tcexp_tcSeqExp(te,loc,topt,_temp1848,_temp1846);
	     goto _LL1737;
    _LL1761: t = Cyc_Tcexp_tcFnCall(te,loc,topt,_temp1852,_temp1850);
	     goto _LL1737;
    _LL1763: t = Cyc_Tcexp_tcThrow(te,loc,topt,_temp1854);
	     goto _LL1737;
    _LL1765: t = Cyc_Tcexp_tcInstantiate(te,loc,topt,_temp1858,_temp1856);
	     goto _LL1737;
    _LL1767: t = Cyc_Tcexp_tcCast(te,loc,topt,_temp1862,_temp1860);
	     goto _LL1737;
    _LL1769: t = Cyc_Tcexp_tcAddress(te,loc,topt,_temp1864);
	     goto _LL1737;
    _LL1771: t = Cyc_Tcexp_tcNew(te,loc,topt,_temp1868,e,_temp1866);
	     goto _LL1737;
    _LL1773: {
    void *t1 = Cyc_Tcexp_tcExpNoPromote(te,0,_temp1870);
      t = Cyc_Tcexp_tcSizeof(te,loc,topt,t1);
      goto _LL1737;
    }
    _LL1775: t = Cyc_Tcexp_tcSizeof(te,loc,topt,_temp1872);
	     goto _LL1737;
    _LL1777: t = Cyc_Tcexp_tcDeref(te,loc,topt,_temp1874);
	     goto _LL1737;
    _LL1779: t = Cyc_Tcexp_tcStructMember(te,loc,topt,e,_temp1878,_temp1876);
	     goto _LL1737;
    _LL1781: t = Cyc_Tcexp_tcStructArrow(te,loc,topt,_temp1882,_temp1880);
	     goto _LL1737;
    _LL1783: t = Cyc_Tcexp_tcSubscript(te,loc,topt,_temp1886,_temp1884);
	     goto _LL1737;
    _LL1785: t = Cyc_Tcexp_tcTuple(te,loc,topt,_temp1888);
	     goto _LL1737;
    _LL1787: t = Cyc_Tcexp_tcCompoundLit(te,loc,topt,_temp1892,_temp1890);
	     goto _LL1737;
    _LL1789: {
    void **elt_topt = 0;
      if (topt != 0) {
	void *_temp1946 = Cyc_Tcutil_compress(({ void **_temp1945 = topt;
						if (_temp1945 == 0) {
						  _throw(Null_Exception);
						}
						*_temp1945; }));
	struct Cyc_Absyn_Exp *_temp1952;
	struct Cyc_Absyn_Tqual _temp1954;
	void *_temp1956;
	void **_temp1958;
	_LL1948: if ((unsigned int)_temp1946 > 4 ? *((int *)_temp1946) == Cyc_Absyn_ArrayType : 0) {
	  _LL1957: _temp1956 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp1946)->f1;
		   _temp1958 = &((struct Cyc_Absyn_ArrayType_struct *)_temp1946)->f1;
		   goto _LL1955;
	  _LL1955: _temp1954 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp1946)->f2;
		   goto _LL1953;
	  _LL1953: _temp1952 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp1946)->f3;
		   goto _LL1949;
	}
	else {
	  goto _LL1950;
	}
	_LL1950: goto _LL1951;
	_LL1949: elt_topt = (void **)_temp1958;
		 goto _LL1947;
	_LL1951: goto _LL1947;
	_LL1947: ;
      }
      t = Cyc_Tcexp_tcArray(te,loc,elt_topt,_temp1894);
      goto _LL1737;
    }
    _LL1791: t = Cyc_Tcexp_tcStmtExp(te,loc,topt,_temp1896);
	     goto _LL1737;
    _LL1793: t = Cyc_Tcexp_tcCodegen(te,loc,topt,_temp1898);
	     goto _LL1737;
    _LL1795: t = Cyc_Tcexp_tcFill(te,loc,topt,_temp1900);
	     goto _LL1737;
    _LL1797: t = Cyc_Tcexp_tcComprehension(te,loc,topt,_temp1906,_temp1904,
					   _temp1902);
	     goto _LL1737;
    _LL1799: t = Cyc_Tcexp_tcStruct(te,loc,topt,_temp1918,_temp1915,_temp1911,
				    _temp1910);
	     goto _LL1737;
    _LL1801: t = Cyc_Tcexp_tcAnonStruct(te,loc,_temp1921,_temp1919);
	     goto _LL1737;
    _LL1803: t = Cyc_Tcexp_tcTunion(te,loc,topt,e,_temp1934,_temp1931,_temp1927,
				    _temp1925,_temp1923);
	     goto _LL1737;
    _LL1805: t = (void *)({ struct Cyc_Absyn_EnumType_struct *_temp1959 = (struct Cyc_Absyn_EnumType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_EnumType_struct));
	       _temp1959[0] = ({ struct Cyc_Absyn_EnumType_struct _temp1960;
		 _temp1960.tag = Cyc_Absyn_EnumType;
		 _temp1960.f1 = ({ struct Cyc_Absyn_Enumdecl *_temp1961 = _temp1937;
		   if (_temp1961 == 0) {
		     _throw(Null_Exception);
		   }
		   _temp1961->name; });
		 _temp1960.f2 = _temp1937;
		 _temp1960; });
	       _temp1959; });
	     goto _LL1737;
    _LL1807: t = Cyc_Tcexp_tcMalloc(te,loc,topt,_temp1943,_temp1941);
	     goto _LL1737;
    _LL1737: ;
  }
  e->topt = ({ struct Cyc_Core_Opt *_temp1962 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
    _temp1962->v = (void *)t;
    _temp1962; });
}
