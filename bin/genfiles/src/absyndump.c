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
extern struct _tagged_string Cyc_Core_string_of_int(int);
extern char *string_to_Cstring(struct _tagged_string);
extern char *underlying_Cstring(struct _tagged_string);
extern struct _tagged_string Cstring_to_string(char *);
extern int system(char *);
struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
extern struct Cyc_Stdio___sFILE *Cyc_Stdio_stdout;
typedef int Cyc_Stdio_fpos_t;
extern int Cyc_Stdio_fputc(int,struct Cyc_Stdio___sFILE *);
extern char Cyc_Stdio_FileOpenError[18];
struct Cyc_Stdio_FileOpenError_struct {
  char *tag;
  struct _tagged_string f1;
};
extern char Cyc_Stdio_FileCloseError[19];
extern int Cyc_Stdio_file_string_write(struct Cyc_Stdio___sFILE *fd,struct _tagged_string src,
				       int src_offset,int max_count);
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
extern char Cyc_List_List_mismatch[18];
extern struct Cyc_List_List *Cyc_List_imp_rev(struct Cyc_List_List *x);
extern struct Cyc_List_List *Cyc_List_imp_append(struct Cyc_List_List *x,
						 struct Cyc_List_List *y);
extern char Cyc_List_Nth[8];
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
struct _tuple0 {
  void *f1;
  struct _tagged_string *f2;
};
typedef struct _tuple0 *Cyc_Absyn_qvar_t;
typedef struct _tuple0 *Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0 *Cyc_Absyn_typedef_name_t;
typedef struct _tuple0 *Cyc_Absyn_typedef_name_opt_t;
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
  struct _tuple0 *name;
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
  struct _tuple0 *tunion_name;
  struct _tuple0 *field_name;
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
  struct _tuple0 *f1;
  struct Cyc_List_List *f2;
  struct Cyc_Absyn_Structdecl **f3;
};
static const int Cyc_Absyn_UnionType = 10;
struct Cyc_Absyn_UnionType_struct {
  int tag;
  struct _tuple0 *f1;
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
  struct _tuple0 *f1;
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
  struct _tuple0 *f1;
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
  struct _tuple0 *f1;
  void *f2;
};
static const int Cyc_Absyn_UnknownId_e = 2;
struct Cyc_Absyn_UnknownId_e_struct {
  int tag;
  struct _tuple0 *f1;
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
struct _tuple1 {
  struct Cyc_Core_Opt *f1;
  struct Cyc_Absyn_Tqual f2;
  void *f3;
};
struct Cyc_Absyn_CompoundLit_e_struct {
  int tag;
  struct _tuple1 *f1;
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
  struct _tuple0 *f1;
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
  struct _tuple0 *f1;
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
struct _tuple2 {
  struct Cyc_Absyn_Exp *f1;
  struct Cyc_Absyn_Stmt *f2;
};
struct Cyc_Absyn_While_s_struct {
  int tag;
  struct _tuple2 f1;
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
  struct _tuple2 f2;
  struct _tuple2 f3;
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
  struct _tuple2 f2;
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
  struct _tuple0 *f1;
};
static const int Cyc_Absyn_UnknownCall_p = 11;
struct Cyc_Absyn_UnknownCall_p_struct {
  int tag;
  struct _tuple0 *f1;
  struct Cyc_List_List *f2;
  struct Cyc_List_List *f3;
};
static const int Cyc_Absyn_UnknownFields_p = 12;
struct Cyc_Absyn_UnknownFields_p_struct {
  int tag;
  struct _tuple0 *f1;
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
  struct _tuple0 *name;
  struct Cyc_Absyn_Tqual tq;
  void *type;
  struct Cyc_Absyn_Exp *initializer;
  struct Cyc_Core_Opt *rgn;
  struct Cyc_List_List *attributes;
};
struct Cyc_Absyn_Fndecl {
  void *sc;
  int is_inline;
  struct _tuple0 *name;
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
  struct _tuple0 *name;
  struct Cyc_List_List *tvs;
  struct Cyc_List_List *typs;
  struct Cyc_Position_Segment *loc;
};
struct Cyc_Absyn_Tuniondecl {
  void *sc;
  struct _tuple0 *name;
  struct Cyc_List_List *tvs;
  struct Cyc_Core_Opt *fields;
  int is_xtunion;
};
struct Cyc_Absyn_Enumfield {
  struct _tuple0 *name;
  struct Cyc_Absyn_Exp *tag;
  struct Cyc_Position_Segment *loc;
};
struct Cyc_Absyn_Enumdecl {
  void *sc;
  struct _tuple0 *name;
  struct Cyc_List_List *fields;
};
struct Cyc_Absyn_Typedefdecl {
  struct _tuple0 *name;
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
  struct _tuple0 *f1;
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
extern struct Cyc_Absyn_Conref *Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref *x);
extern int Cyc_Absyn_is_format_prim(void *p);
extern struct _tagged_string Cyc_Absyn_attribute2string(void *);
struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate *Cyc_PP_ppstate_t;
struct Cyc_PP_Out;
typedef struct Cyc_PP_Out *Cyc_PP_out_t;
struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc *Cyc_PP_doc_t;
extern int Cyc_Absynpp_qvar_to_Cids;
extern int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp *);
extern struct _tagged_string Cyc_Absynpp_char_escape(char);
extern struct _tagged_string Cyc_Absynpp_string_escape(struct _tagged_string);
extern struct _tagged_string Cyc_Absynpp_prim2str(void *p);
extern int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt *s);
struct _tuple3 {
  struct _tagged_string *f1;
  struct Cyc_Absyn_Tqual f2;
  void *f3;
};
extern struct _tuple1 *Cyc_Absynpp_arg_mk_opt(struct _tuple3 *arg);
struct _tuple4 {
  struct Cyc_Absyn_Tqual f1;
  void *f2;
  struct Cyc_List_List *f3;
};
extern struct _tuple4 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void *t);
extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp *e);
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
extern char Cyc_Tcutil_TypeErr[12];
extern void *Cyc_Tcutil_compress(void *t);
extern void Cyc_Absyndump_dumptyp(void *);
extern void Cyc_Absyndump_dumpntyp(void *t);
extern void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp *);
extern void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp *);
extern void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat *);
extern void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt *);
extern void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl *);
extern void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void *,void (*f)(void *),
				   void *);
extern void Cyc_Absyndump_dumpstructfields(struct Cyc_List_List *fields);
typedef struct _tagged_string Cyc_Absyndump_dump_string_t;
struct Cyc_Stdio___sFILE **Cyc_Absyndump_dump_file = &Cyc_Stdio_stdout;
void Cyc_Absyndump_ignore(void *x) {
  return;
}
static unsigned int Cyc_Absyndump_pos = 0;
void Cyc_Absyndump_dump(struct _tagged_string s) {
  int sz = (int)({ struct _tagged_string _temp3 = s;
    (unsigned int)(_temp3.last_plus_one - _temp3.curr); });
  if (! ((int)({ struct _tagged_string _temp0 = s;
    char *_temp2 = _temp0.curr + (sz - 1);
    if (_temp0.base == 0 ? 1 : (_temp2 < _temp0.base ? 1 : _temp2 >= _temp0.last_plus_one)) {
      _throw(Null_Exception);
    }
    *_temp2; }))) {
    --sz;
  }
  Cyc_Absyndump_pos += sz + 1;
  if (Cyc_Absyndump_pos > 80) {
    Cyc_Absyndump_pos = (unsigned int)sz;
    Cyc_Stdio_fputc((int)'\n',*Cyc_Absyndump_dump_file);
  }
  else {
    Cyc_Stdio_fputc((int)' ',*Cyc_Absyndump_dump_file);
  }
  Cyc_Stdio_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
}
void Cyc_Absyndump_dump_nospace(struct _tagged_string s) {
  int sz = (int)({ struct _tagged_string _temp7 = s;
    (unsigned int)(_temp7.last_plus_one - _temp7.curr); });
  if (! ((int)({ struct _tagged_string _temp4 = s;
    char *_temp6 = _temp4.curr + (sz - 1);
    if (_temp4.base == 0 ? 1 : (_temp6 < _temp4.base ? 1 : _temp6 >= _temp4.last_plus_one)) {
      _throw(Null_Exception);
    }
    *_temp6; }))) {
    --sz;
  }
  Cyc_Absyndump_pos += sz;
  Cyc_Stdio_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
}
void Cyc_Absyndump_dump_char(int c) {
  ++Cyc_Absyndump_pos;
  Cyc_Stdio_fputc(c,*Cyc_Absyndump_dump_file);
}
void Cyc_Absyndump_dump_str(struct _tagged_string *s) {
  Cyc_Absyndump_dump(*s);
}
void Cyc_Absyndump_dump_semi() {
  Cyc_Absyndump_dump_char((int)';');
}
void Cyc_Absyndump_dump_sep(void (*f)(void *),struct Cyc_List_List *l,struct _tagged_string sep) {
  if (l == 0) {
    return;
  }
  for(0; ({ struct Cyc_List_List *_temp8 = l;
    if (_temp8 == 0) {
      _throw(Null_Exception);
    }
    _temp8->tl; }) != 0; l = ({ struct Cyc_List_List *_temp9 = l;
    if (_temp9 == 0) {
      _throw(Null_Exception);
    }
    _temp9->tl; })) {
    f((void *)({ struct Cyc_List_List *_temp10 = l;
	if (_temp10 == 0) {
	  _throw(Null_Exception);
	}
	_temp10->hd; }));
    Cyc_Absyndump_dump_nospace(sep);
  }
  f((void *)({ struct Cyc_List_List *_temp11 = l;
      if (_temp11 == 0) {
	_throw(Null_Exception);
      }
      _temp11->hd; }));
}
void Cyc_Absyndump_dump_sep_c(void (*f)(void *,void *),void *env,struct Cyc_List_List *l,
			      struct _tagged_string sep) {
  if (l == 0) {
    return;
  }
  for(0; ({ struct Cyc_List_List *_temp12 = l;
    if (_temp12 == 0) {
      _throw(Null_Exception);
    }
    _temp12->tl; }) != 0; l = ({ struct Cyc_List_List *_temp13 = l;
    if (_temp13 == 0) {
      _throw(Null_Exception);
    }
    _temp13->tl; })) {
    f(env,(void *)({ struct Cyc_List_List *_temp14 = l;
	if (_temp14 == 0) {
	  _throw(Null_Exception);
	}
	_temp14->hd; }));
    Cyc_Absyndump_dump_nospace(sep);
  }
  f(env,(void *)({ struct Cyc_List_List *_temp15 = l;
      if (_temp15 == 0) {
	_throw(Null_Exception);
      }
      _temp15->hd; }));
}
void Cyc_Absyndump_group(void (*f)(void *),struct Cyc_List_List *l,struct _tagged_string start,
			 struct _tagged_string end,struct _tagged_string sep) {
  Cyc_Absyndump_dump_nospace(start);
  ((void (*)(void (*f)(void *),struct Cyc_List_List *l,struct _tagged_string sep))Cyc_Absyndump_dump_sep)(f,
													  l,
													  sep);
  Cyc_Absyndump_dump_nospace(end);
}
void Cyc_Absyndump_group_c(void (*f)(void *,void *),void *env,struct Cyc_List_List *l,
			   struct _tagged_string start,struct _tagged_string end,
			   struct _tagged_string sep) {
  Cyc_Absyndump_dump_nospace(start);
  ((void (*)(void (*f)(void *,void *),void *env,struct Cyc_List_List *l,struct _tagged_string sep))Cyc_Absyndump_dump_sep_c)(f,
															     env,
															     l,
															     sep);
  Cyc_Absyndump_dump_nospace(end);
}
void Cyc_Absyndump_egroup(void (*f)(void *),struct Cyc_List_List *l,struct _tagged_string start,
			  struct _tagged_string end,struct _tagged_string sep) {
  if (l != 0) {
    ((void (*)(void (*f)(void *),struct Cyc_List_List *l,struct _tagged_string start,
	       struct _tagged_string end,struct _tagged_string sep))Cyc_Absyndump_group)(f,
											 l,
											 start,
											 end,
											 sep);
  }
}
void Cyc_Absyndump_dumpqvar(struct _tuple0 *v) {
  struct Cyc_List_List *nsl = 0;
  { void *_temp16 = (*v).f1;
    struct Cyc_List_List *_temp28;
    struct Cyc_List_List *_temp30;
    struct Cyc_List_List *_temp32;
    struct Cyc_List_List *_temp34;
    _LL18: if (_temp16 == (void *)Cyc_Absyn_Loc_n) {
      goto _LL19;
    }
    else {
      goto _LL20;
    }
    _LL20: if ((unsigned int)_temp16 > 1 ? *((int *)_temp16) == Cyc_Absyn_Rel_n : 0) {
      _LL29: _temp28 = (struct Cyc_List_List *)((struct Cyc_Absyn_Rel_n_struct *)_temp16)->f1;
	     if (_temp28 == 0) {
	       goto _LL21;
	     }
	     else {
	       goto _LL22;
	     }
    }
    else {
      goto _LL22;
    }
    _LL22: if ((unsigned int)_temp16 > 1 ? *((int *)_temp16) == Cyc_Absyn_Abs_n : 0) {
      _LL31: _temp30 = (struct Cyc_List_List *)((struct Cyc_Absyn_Abs_n_struct *)_temp16)->f1;
	     if (_temp30 == 0) {
	       goto _LL23;
	     }
	     else {
	       goto _LL24;
	     }
    }
    else {
      goto _LL24;
    }
    _LL24: if ((unsigned int)_temp16 > 1 ? *((int *)_temp16) == Cyc_Absyn_Rel_n : 0) {
      _LL33: _temp32 = (struct Cyc_List_List *)((struct Cyc_Absyn_Rel_n_struct *)_temp16)->f1;
	     goto _LL25;
    }
    else {
      goto _LL26;
    }
    _LL26: if ((unsigned int)_temp16 > 1 ? *((int *)_temp16) == Cyc_Absyn_Abs_n : 0) {
      _LL35: _temp34 = (struct Cyc_List_List *)((struct Cyc_Absyn_Abs_n_struct *)_temp16)->f1;
	     goto _LL27;
    }
    else {
      goto _LL17;
    }
    _LL19: goto _LL21;
    _LL21: goto _LL23;
    _LL23: Cyc_Absyndump_dump_str((*v).f2);
	   return;
    _LL25: _temp34 = _temp32;
	   goto _LL27;
    _LL27: nsl = _temp34;
	   goto _LL17;
    _LL17: ;
  }
  Cyc_Absyndump_dump_str((struct _tagged_string *)({ struct Cyc_List_List *_temp36 = nsl;
			   if (_temp36 == 0) {
			     _throw(Null_Exception);
			   }
			   _temp36->hd; }));
  for(nsl = ({ struct Cyc_List_List *_temp37 = nsl;
    if (_temp37 == 0) {
      _throw(Null_Exception);
    }
    _temp37->tl; }); nsl != 0; nsl = ({ struct Cyc_List_List *_temp38 = nsl;
    if (_temp38 == 0) {
      _throw(Null_Exception);
    }
    _temp38->tl; })) {
    if (Cyc_Absynpp_qvar_to_Cids) {
      Cyc_Absyndump_dump_char((int)'_');
    }
    else {
      Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp39 = (char *)"::";
				   struct _tagged_string _temp40;
				   _temp40.curr = _temp39;
				   _temp40.base = _temp39;
				   _temp40.last_plus_one = _temp39 + 3;
				   _temp40; }));
    }
    Cyc_Absyndump_dump_nospace(*((struct _tagged_string *)({ struct Cyc_List_List *_temp41 = nsl;
				 if (_temp41 == 0) {
				   _throw(Null_Exception);
				 }
				 _temp41->hd; })));
  }
  if (Cyc_Absynpp_qvar_to_Cids) {
    Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp42 = (char *)"_";
				 struct _tagged_string _temp43;
				 _temp43.curr = _temp42;
				 _temp43.base = _temp42;
				 _temp43.last_plus_one = _temp42 + 2;
				 _temp43; }));
  }
  else {
    Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp44 = (char *)"::";
				 struct _tagged_string _temp45;
				 _temp45.curr = _temp44;
				 _temp45.base = _temp44;
				 _temp45.last_plus_one = _temp44 + 3;
				 _temp45; }));
  }
  Cyc_Absyndump_dump_nospace(*(*v).f2);
}
void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq) {
  if (tq.q_restrict) {
    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp46 = (char *)"restrict";
			 struct _tagged_string _temp47;
			 _temp47.curr = _temp46;
			 _temp47.base = _temp46;
			 _temp47.last_plus_one = _temp46 + 9;
			 _temp47; }));
  }
  if (tq.q_volatile) {
    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp48 = (char *)"volatile";
			 struct _tagged_string _temp49;
			 _temp49.curr = _temp48;
			 _temp49.base = _temp48;
			 _temp49.last_plus_one = _temp48 + 9;
			 _temp49; }));
  }
  if (tq.q_const) {
    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp50 = (char *)"const";
			 struct _tagged_string _temp51;
			 _temp51.curr = _temp50;
			 _temp51.base = _temp50;
			 _temp51.last_plus_one = _temp50 + 6;
			 _temp51; }));
  }
}
void Cyc_Absyndump_dumpscope(void *sc) {
  void *_temp52 = sc;
  _LL54: if (_temp52 == (void *)Cyc_Absyn_Static) {
    goto _LL55;
  }
  else {
    goto _LL56;
  }
  _LL56: if (_temp52 == (void *)Cyc_Absyn_Public) {
    goto _LL57;
  }
  else {
    goto _LL58;
  }
  _LL58: if (_temp52 == (void *)Cyc_Absyn_Extern) {
    goto _LL59;
  }
  else {
    goto _LL60;
  }
  _LL60: if (_temp52 == (void *)Cyc_Absyn_ExternC) {
    goto _LL61;
  }
  else {
    goto _LL62;
  }
  _LL62: if (_temp52 == (void *)Cyc_Absyn_Abstract) {
    goto _LL63;
  }
  else {
    goto _LL53;
  }
  _LL55: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp64 = (char *)"static";
			      struct _tagged_string _temp65;
			      _temp65.curr = _temp64;
			      _temp65.base = _temp64;
			      _temp65.last_plus_one = _temp64 + 7;
			      _temp65; }));
	 return;
  _LL57: return;
  _LL59: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp66 = (char *)"extern";
			      struct _tagged_string _temp67;
			      _temp67.curr = _temp66;
			      _temp67.base = _temp66;
			      _temp67.last_plus_one = _temp66 + 7;
			      _temp67; }));
	 return;
  _LL61: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp68 = (char *)"extern \"C\"";
			      struct _tagged_string _temp69;
			      _temp69.curr = _temp68;
			      _temp69.base = _temp68;
			      _temp69.last_plus_one = _temp68 + 11;
			      _temp69; }));
	 return;
  _LL63: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp70 = (char *)"abstract";
			      struct _tagged_string _temp71;
			      _temp71.curr = _temp70;
			      _temp71.base = _temp70;
			      _temp71.last_plus_one = _temp70 + 9;
			      _temp71; }));
	 return;
  _LL53: ;
}
void Cyc_Absyndump_dumpkind(void *k) {
  void *_temp72 = k;
  _LL74: if (_temp72 == (void *)Cyc_Absyn_AnyKind) {
    goto _LL75;
  }
  else {
    goto _LL76;
  }
  _LL76: if (_temp72 == (void *)Cyc_Absyn_MemKind) {
    goto _LL77;
  }
  else {
    goto _LL78;
  }
  _LL78: if (_temp72 == (void *)Cyc_Absyn_BoxKind) {
    goto _LL79;
  }
  else {
    goto _LL80;
  }
  _LL80: if (_temp72 == (void *)Cyc_Absyn_RgnKind) {
    goto _LL81;
  }
  else {
    goto _LL82;
  }
  _LL82: if (_temp72 == (void *)Cyc_Absyn_EffKind) {
    goto _LL83;
  }
  else {
    goto _LL73;
  }
  _LL75: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp84 = (char *)"A";
			      struct _tagged_string _temp85;
			      _temp85.curr = _temp84;
			      _temp85.base = _temp84;
			      _temp85.last_plus_one = _temp84 + 2;
			      _temp85; }));
	 return;
  _LL77: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp86 = (char *)"M";
			      struct _tagged_string _temp87;
			      _temp87.curr = _temp86;
			      _temp87.base = _temp86;
			      _temp87.last_plus_one = _temp86 + 2;
			      _temp87; }));
	 return;
  _LL79: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp88 = (char *)"B";
			      struct _tagged_string _temp89;
			      _temp89.curr = _temp88;
			      _temp89.base = _temp88;
			      _temp89.last_plus_one = _temp88 + 2;
			      _temp89; }));
	 return;
  _LL81: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp90 = (char *)"R";
			      struct _tagged_string _temp91;
			      _temp91.curr = _temp90;
			      _temp91.base = _temp90;
			      _temp91.last_plus_one = _temp90 + 2;
			      _temp91; }));
	 return;
  _LL83: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp92 = (char *)"E";
			      struct _tagged_string _temp93;
			      _temp93.curr = _temp92;
			      _temp93.base = _temp92;
			      _temp93.last_plus_one = _temp92 + 2;
			      _temp93; }));
	 return;
  _LL73: ;
}
void Cyc_Absyndump_dumptps(struct Cyc_List_List *ts) {
  ((void (*)(void (*f)(void *),struct Cyc_List_List *l,struct _tagged_string start,
	     struct _tagged_string end,struct _tagged_string sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,
											ts,
											(struct _tagged_string)({ char *_temp94 = (char *)"<";
											  struct _tagged_string _temp95;
											  _temp95.curr = _temp94;
											  _temp95.base = _temp94;
											  _temp95.last_plus_one = _temp94 + 2;
											  _temp95; }),
											(struct _tagged_string)({ char *_temp96 = (char *)">";
											  struct _tagged_string _temp97;
											  _temp97.curr = _temp96;
											  _temp97.base = _temp96;
											  _temp97.last_plus_one = _temp96 + 2;
											  _temp97; }),
											(struct _tagged_string)({ char *_temp98 = (char *)",";
											  struct _tagged_string _temp99;
											  _temp99.curr = _temp98;
											  _temp99.base = _temp98;
											  _temp99.last_plus_one = _temp98 + 2;
											  _temp99; }));
}
void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar *tv) {
  Cyc_Absyndump_dump_str(tv->name);
}
void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar *tv) {
  Cyc_Absyndump_dump_str(tv->name);
  { void *_temp100 = (void *)(((struct Cyc_Absyn_Conref *(*)(struct Cyc_Absyn_Conref *x))Cyc_Absyn_compress_conref)(tv->kind))->v;
    void *_temp108;
    void *_temp110;
    _LL102: if ((unsigned int)_temp100 > 1 ? *((int *)_temp100) == Cyc_Absyn_Eq_constr : 0) {
      _LL109: _temp108 = (void *)((struct Cyc_Absyn_Eq_constr_struct *)_temp100)->f1;
	      if (_temp108 == (void *)Cyc_Absyn_BoxKind) {
		goto _LL103;
	      }
	      else {
		goto _LL104;
	      }
    }
    else {
      goto _LL104;
    }
    _LL104: if ((unsigned int)_temp100 > 1 ? *((int *)_temp100) == Cyc_Absyn_Eq_constr : 0) {
      _LL111: _temp110 = (void *)((struct Cyc_Absyn_Eq_constr_struct *)_temp100)->f1;
	      goto _LL105;
    }
    else {
      goto _LL106;
    }
    _LL106: goto _LL107;
    _LL103: goto _LL101;
    _LL105: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp112 = (char *)"::";
				 struct _tagged_string _temp113;
				 _temp113.curr = _temp112;
				 _temp113.base = _temp112;
				 _temp113.last_plus_one = _temp112 + 3;
				 _temp113; }));
	    Cyc_Absyndump_dumpkind(_temp110);
	    goto _LL101;
    _LL107: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp114 = (char *)"::?";
				 struct _tagged_string _temp115;
				 _temp115.curr = _temp114;
				 _temp115.base = _temp114;
				 _temp115.last_plus_one = _temp114 + 4;
				 _temp115; }));
	    goto _LL101;
    _LL101: ;}
  
}
void Cyc_Absyndump_dumptvars(struct Cyc_List_List *tvs) {
  ((void (*)(void (*f)(struct Cyc_Absyn_Tvar *),struct Cyc_List_List *l,struct _tagged_string start,
	     struct _tagged_string end,struct _tagged_string sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
											tvs,
											(struct _tagged_string)({ char *_temp116 = (char *)"<";
											  struct _tagged_string _temp117;
											  _temp117.curr = _temp116;
											  _temp117.base = _temp116;
											  _temp117.last_plus_one = _temp116 + 2;
											  _temp117; }),
											(struct _tagged_string)({ char *_temp118 = (char *)">";
											  struct _tagged_string _temp119;
											  _temp119.curr = _temp118;
											  _temp119.base = _temp118;
											  _temp119.last_plus_one = _temp118 + 2;
											  _temp119; }),
											(struct _tagged_string)({ char *_temp120 = (char *)",";
											  struct _tagged_string _temp121;
											  _temp121.curr = _temp120;
											  _temp121.base = _temp120;
											  _temp121.last_plus_one = _temp120 + 2;
											  _temp121; }));
}
void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List *tvs) {
  ((void (*)(void (*f)(struct Cyc_Absyn_Tvar *),struct Cyc_List_List *l,struct _tagged_string start,
	     struct _tagged_string end,struct _tagged_string sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,
											tvs,
											(struct _tagged_string)({ char *_temp122 = (char *)"<";
											  struct _tagged_string _temp123;
											  _temp123.curr = _temp122;
											  _temp123.base = _temp122;
											  _temp123.last_plus_one = _temp122 + 2;
											  _temp123; }),
											(struct _tagged_string)({ char *_temp124 = (char *)">";
											  struct _tagged_string _temp125;
											  _temp125.curr = _temp124;
											  _temp125.base = _temp124;
											  _temp125.last_plus_one = _temp124 + 2;
											  _temp125; }),
											(struct _tagged_string)({ char *_temp126 = (char *)",";
											  struct _tagged_string _temp127;
											  _temp127.curr = _temp126;
											  _temp127.base = _temp126;
											  _temp127.last_plus_one = _temp126 + 2;
											  _temp127; }));
}
struct _tuple5 {
  struct Cyc_Absyn_Tqual f1;
  void *f2;
};
void Cyc_Absyndump_dumparg(struct _tuple5 *pr) {
  ((void (*)(struct Cyc_Absyn_Tqual,void *,void (*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,
										       (*pr).f2,
										       (void (*)(int x))Cyc_Absyndump_ignore,
										       0);
}
void Cyc_Absyndump_dumpargs(struct Cyc_List_List *ts) {
  ((void (*)(void (*f)(struct _tuple5 *),struct Cyc_List_List *l,struct _tagged_string start,
	     struct _tagged_string end,struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,
										       ts,
										       (struct _tagged_string)({ char *_temp128 = (char *)"(";
											 struct _tagged_string _temp129;
											 _temp129.curr = _temp128;
											 _temp129.base = _temp128;
											 _temp129.last_plus_one = _temp128 + 2;
											 _temp129; }),
										       (struct _tagged_string)({ char *_temp130 = (char *)")";
											 struct _tagged_string _temp131;
											 _temp131.curr = _temp130;
											 _temp131.base = _temp130;
											 _temp131.last_plus_one = _temp130 + 2;
											 _temp131; }),
										       (struct _tagged_string)({ char *_temp132 = (char *)",";
											 struct _tagged_string _temp133;
											 _temp133.curr = _temp132;
											 _temp133.base = _temp132;
											 _temp133.last_plus_one = _temp132 + 2;
											 _temp133; }));
}
void Cyc_Absyndump_dumpatts(struct Cyc_List_List *atts) {
  if (atts == 0) {
    return;
  }
  Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp134 = (char *)" __attribute__((";
		       struct _tagged_string _temp135;
		       _temp135.curr = _temp134;
		       _temp135.base = _temp134;
		       _temp135.last_plus_one = _temp134 + 17;
		       _temp135; }));
  for(0; atts != 0; atts = ({ struct Cyc_List_List *_temp136 = atts;
    if (_temp136 == 0) {
      _throw(Null_Exception);
    }
    _temp136->tl; })) {
    Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void *)({ struct Cyc_List_List *_temp137 = atts;
						    if (_temp137 == 0) {
						      _throw(Null_Exception);
						    }
						    _temp137->hd; })));
    if (({ struct Cyc_List_List *_temp138 = atts;
      if (_temp138 == 0) {
	_throw(Null_Exception);
      }
      _temp138->tl; }) != 0) {
      Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp139 = (char *)",";
			   struct _tagged_string _temp140;
			   _temp140.curr = _temp139;
			   _temp140.base = _temp139;
			   _temp140.last_plus_one = _temp139 + 2;
			   _temp140; }));
    }
  }
  Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp141 = (char *)")) ";
		       struct _tagged_string _temp142;
		       _temp142.curr = _temp141;
		       _temp142.base = _temp141;
		       _temp142.last_plus_one = _temp141 + 4;
		       _temp142; }));
}
int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List *tms) {
  if (tms == 0) {
    return 0;
  }
  { void *_temp144 = (void *)({ struct Cyc_List_List *_temp143 = tms;
      if (_temp143 == 0) {
	_throw(Null_Exception);
      }
      _temp143->hd; });
    struct Cyc_Absyn_Tqual _temp150;
    void *_temp152;
    void *_temp154;
    _LL146: if ((unsigned int)_temp144 > 1 ? *((int *)_temp144) == Cyc_Absyn_Pointer_mod : 0) {
      _LL155: _temp154 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp144)->f1;
	      goto _LL153;
      _LL153: _temp152 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp144)->f2;
	      goto _LL151;
      _LL151: _temp150 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_Pointer_mod_struct *)_temp144)->f3;
	      goto _LL147;
    }
    else {
      goto _LL148;
    }
    _LL148: goto _LL149;
    _LL147: return 1;
    _LL149: return 0;
    _LL145: ;}
  
}
static void Cyc_Absyndump_dumprgn(void *t) {
  void *_temp156 = Cyc_Tcutil_compress(t);
  _LL158: if (_temp156 == (void *)Cyc_Absyn_HeapRgn) {
    goto _LL159;
  }
  else {
    goto _LL160;
  }
  _LL160: goto _LL161;
  _LL159: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp162 = (char *)"`H";
			       struct _tagged_string _temp163;
			       _temp163.curr = _temp162;
			       _temp163.base = _temp162;
			       _temp163.last_plus_one = _temp162 + 3;
			       _temp163; }));
	  goto _LL157;
  _LL161: Cyc_Absyndump_dumpntyp(t);
	  goto _LL157;
  _LL157: ;
}
struct _tuple6 {
  struct Cyc_List_List *f1;
  struct Cyc_List_List *f2;
};
static struct _tuple6 Cyc_Absyndump_effects_split(void *t) {
  struct Cyc_List_List *regions = 0;
  struct Cyc_List_List *effects = 0;
  { void *_temp164 = Cyc_Tcutil_compress(t);
    void *_temp172;
    struct Cyc_List_List *_temp174;
    _LL166: if ((unsigned int)_temp164 > 4 ? *((int *)_temp164) == Cyc_Absyn_AccessEff : 0) {
      _LL173: _temp172 = (void *)((struct Cyc_Absyn_AccessEff_struct *)_temp164)->f1;
	      goto _LL167;
    }
    else {
      goto _LL168;
    }
    _LL168: if ((unsigned int)_temp164 > 4 ? *((int *)_temp164) == Cyc_Absyn_JoinEff : 0) {
      _LL175: _temp174 = (struct Cyc_List_List *)((struct Cyc_Absyn_JoinEff_struct *)_temp164)->f1;
	      goto _LL169;
    }
    else {
      goto _LL170;
    }
    _LL170: goto _LL171;
    _LL167: regions = ({ struct Cyc_List_List *_temp176 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
	      _temp176->hd = (void *)_temp172;
	      _temp176->tl = regions;
	      _temp176; });
	    goto _LL165;
    _LL169: for(0; _temp174 != 0; _temp174 = ({ struct Cyc_List_List *_temp177 = _temp174;
	      if (_temp177 == 0) {
		_throw(Null_Exception);
	      }
	      _temp177->tl; })) {
	      struct Cyc_List_List *_temp181;
	      struct Cyc_List_List *_temp183;
	      struct _tuple6 _temp179 = Cyc_Absyndump_effects_split((void *)({ struct Cyc_List_List *_temp178 = _temp174;
								      if (_temp178 == 0) {
									_throw(Null_Exception);
								      }
								      _temp178->hd; }));
	      _LL184: _temp183 = _temp179.f1;
		      goto _LL182;
	      _LL182: _temp181 = _temp179.f2;
		      goto _LL180;
	      _LL180: regions = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
							    struct Cyc_List_List *y))Cyc_List_imp_append)(_temp183,
													  regions);
		      effects = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
							    struct Cyc_List_List *y))Cyc_List_imp_append)(_temp181,
													  effects);
	    }
	    goto _LL165;
    _LL171: effects = ({ struct Cyc_List_List *_temp185 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
	      _temp185->hd = (void *)t;
	      _temp185->tl = effects;
	      _temp185; });
	    goto _LL165;
    _LL165: ;
  }
  return ({ struct _tuple6 _temp186;
    _temp186.f1 = regions;
    _temp186.f2 = effects;
    _temp186; });
}
static void Cyc_Absyndump_dumpeff(void *t) {
  struct Cyc_List_List *_temp189;
  struct Cyc_List_List *_temp191;
  struct _tuple6 _temp187 = Cyc_Absyndump_effects_split(t);
  _LL192: _temp191 = _temp187.f1;
	  goto _LL190;
  _LL190: _temp189 = _temp187.f2;
	  goto _LL188;
  _LL188: _temp191 = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x))Cyc_List_imp_rev)(_temp191);
	  _temp189 = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x))Cyc_List_imp_rev)(_temp189);
	  for(0; _temp189 != 0; _temp189 = ({ struct Cyc_List_List *_temp193 = _temp189;
	    if (_temp193 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp193->tl; })) {
	    Cyc_Absyndump_dumpntyp((void *)({ struct Cyc_List_List *_temp194 = _temp189;
				     if (_temp194 == 0) {
				       _throw(Null_Exception);
				     }
				     _temp194->hd; }));
	    Cyc_Absyndump_dump_char((int)'+');
	  }
	  Cyc_Absyndump_dump_char((int)'{');
	  for(0; _temp191 != 0; _temp191 = ({ struct Cyc_List_List *_temp195 = _temp191;
	    if (_temp195 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp195->tl; })) {
	    Cyc_Absyndump_dumprgn((void *)({ struct Cyc_List_List *_temp196 = _temp191;
				    if (_temp196 == 0) {
				      _throw(Null_Exception);
				    }
				    _temp196->hd; }));
	    if (({ struct Cyc_List_List *_temp197 = _temp191;
	      if (_temp197 == 0) {
		_throw(Null_Exception);
	      }
	      _temp197->tl; }) != 0) {
	      Cyc_Absyndump_dump_char((int)',');
	    }
	  }
	  Cyc_Absyndump_dump_char((int)'}');
}
void Cyc_Absyndump_dumpntyp(void *t) {
  void *_temp198 = t;
  struct Cyc_Absyn_Exp *_temp264;
  struct Cyc_Absyn_Tqual _temp266;
  void *_temp268;
  struct Cyc_Absyn_FnInfo _temp270;
  struct Cyc_Absyn_PtrInfo _temp272;
  struct Cyc_Absyn_Tvar *_temp274;
  int _temp276;
  struct Cyc_Core_Opt *_temp278;
  void *_temp280;
  int _temp282;
  struct Cyc_Core_Opt *_temp284;
  struct Cyc_Core_Opt _temp286;
  void *_temp287;
  void *_temp289;
  struct Cyc_Absyn_TunionInfo _temp291;
  void *_temp293;
  struct Cyc_List_List *_temp295;
  void *_temp297;
  struct Cyc_Absyn_TunionFieldInfo _temp299;
  struct Cyc_List_List *_temp301;
  void *_temp303;
  struct Cyc_Absyn_Enumdecl *_temp305;
  struct _tuple0 *_temp307;
  void *_temp309;
  void *_temp311;
  void *_temp313;
  void *_temp315;
  void *_temp317;
  void *_temp319;
  void *_temp321;
  void *_temp323;
  void *_temp325;
  void *_temp327;
  void *_temp329;
  void *_temp331;
  void *_temp333;
  void *_temp335;
  void *_temp337;
  void *_temp339;
  struct Cyc_List_List *_temp341;
  struct Cyc_Absyn_Structdecl **_temp343;
  struct Cyc_List_List *_temp345;
  struct _tuple0 *_temp347;
  struct Cyc_Absyn_Structdecl **_temp349;
  struct Cyc_List_List *_temp351;
  struct _tuple0 *_temp353;
  struct Cyc_Absyn_Uniondecl **_temp355;
  struct Cyc_List_List *_temp357;
  struct _tuple0 *_temp359;
  struct Cyc_Absyn_Uniondecl **_temp361;
  struct Cyc_List_List *_temp363;
  struct _tuple0 *_temp365;
  struct Cyc_List_List *_temp367;
  struct Cyc_List_List *_temp369;
  struct Cyc_Core_Opt *_temp371;
  struct Cyc_List_List *_temp373;
  struct _tuple0 *_temp375;
  void *_temp377;
  void *_temp379;
  struct Cyc_List_List *_temp381;
  _LL200: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_ArrayType : 0) {
    _LL269: _temp268 = (void *)((struct Cyc_Absyn_ArrayType_struct *)_temp198)->f1;
	    goto _LL267;
    _LL267: _temp266 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_ArrayType_struct *)_temp198)->f2;
	    goto _LL265;
    _LL265: _temp264 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayType_struct *)_temp198)->f3;
	    goto _LL201;
  }
  else {
    goto _LL202;
  }
  _LL202: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_FnType : 0) {
    _LL271: _temp270 = (struct Cyc_Absyn_FnInfo)((struct Cyc_Absyn_FnType_struct *)_temp198)->f1;
	    goto _LL203;
  }
  else {
    goto _LL204;
  }
  _LL204: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_PointerType : 0) {
    _LL273: _temp272 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp198)->f1;
	    goto _LL205;
  }
  else {
    goto _LL206;
  }
  _LL206: if (_temp198 == (void *)Cyc_Absyn_VoidType) {
    goto _LL207;
  }
  else {
    goto _LL208;
  }
  _LL208: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_VarType : 0) {
    _LL275: _temp274 = (struct Cyc_Absyn_Tvar *)((struct Cyc_Absyn_VarType_struct *)_temp198)->f1;
	    goto _LL209;
  }
  else {
    goto _LL210;
  }
  _LL210: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_Evar : 0) {
    _LL281: _temp280 = (void *)((struct Cyc_Absyn_Evar_struct *)_temp198)->f1;
	    goto _LL279;
    _LL279: _temp278 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Evar_struct *)_temp198)->f2;
	    if (_temp278 == 0) {
	      goto _LL277;
	    }
	    else {
	      goto _LL212;
	    }
    _LL277: _temp276 = (int)((struct Cyc_Absyn_Evar_struct *)_temp198)->f3;
	    goto _LL211;
  }
  else {
    goto _LL212;
  }
  _LL212: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_Evar : 0) {
    _LL290: _temp289 = (void *)((struct Cyc_Absyn_Evar_struct *)_temp198)->f1;
	    goto _LL285;
    _LL285: _temp284 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Evar_struct *)_temp198)->f2;
	    if (_temp284 == 0) {
	      goto _LL214;
	    }
	    else {
	      _temp286 = *_temp284;
	      _LL288: _temp287 = (void *)_temp286.v;
		      goto _LL283;
	    }
    _LL283: _temp282 = (int)((struct Cyc_Absyn_Evar_struct *)_temp198)->f3;
	    goto _LL213;
  }
  else {
    goto _LL214;
  }
  _LL214: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_TunionType : 0) {
    _LL292: _temp291 = (struct Cyc_Absyn_TunionInfo)((struct Cyc_Absyn_TunionType_struct *)_temp198)->f1;
	    _LL298: _temp297 = (void *)_temp291.tunion_info;
		    goto _LL296;
	    _LL296: _temp295 = (struct Cyc_List_List *)_temp291.targs;
		    goto _LL294;
	    _LL294: _temp293 = (void *)_temp291.rgn;
		    goto _LL215;
  }
  else {
    goto _LL216;
  }
  _LL216: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_TunionFieldType : 0) {
    _LL300: _temp299 = (struct Cyc_Absyn_TunionFieldInfo)((struct Cyc_Absyn_TunionFieldType_struct *)_temp198)->f1;
	    _LL304: _temp303 = (void *)_temp299.field_info;
		    goto _LL302;
	    _LL302: _temp301 = (struct Cyc_List_List *)_temp299.targs;
		    goto _LL217;
  }
  else {
    goto _LL218;
  }
  _LL218: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_EnumType : 0) {
    _LL308: _temp307 = (struct _tuple0 *)((struct Cyc_Absyn_EnumType_struct *)_temp198)->f1;
	    goto _LL306;
    _LL306: _temp305 = (struct Cyc_Absyn_Enumdecl *)((struct Cyc_Absyn_EnumType_struct *)_temp198)->f2;
	    goto _LL219;
  }
  else {
    goto _LL220;
  }
  _LL220: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_IntType : 0) {
    _LL312: _temp311 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f1;
	    if (_temp311 == (void *)Cyc_Absyn_Signed) {
	      goto _LL310;
	    }
	    else {
	      goto _LL222;
	    }
    _LL310: _temp309 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f2;
	    if (_temp309 == (void *)Cyc_Absyn_B1) {
	      goto _LL221;
	    }
	    else {
	      goto _LL222;
	    }
  }
  else {
    goto _LL222;
  }
  _LL222: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_IntType : 0) {
    _LL316: _temp315 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f1;
	    if (_temp315 == (void *)Cyc_Absyn_Unsigned) {
	      goto _LL314;
	    }
	    else {
	      goto _LL224;
	    }
    _LL314: _temp313 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f2;
	    if (_temp313 == (void *)Cyc_Absyn_B1) {
	      goto _LL223;
	    }
	    else {
	      goto _LL224;
	    }
  }
  else {
    goto _LL224;
  }
  _LL224: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_IntType : 0) {
    _LL320: _temp319 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f1;
	    if (_temp319 == (void *)Cyc_Absyn_Signed) {
	      goto _LL318;
	    }
	    else {
	      goto _LL226;
	    }
    _LL318: _temp317 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f2;
	    if (_temp317 == (void *)Cyc_Absyn_B2) {
	      goto _LL225;
	    }
	    else {
	      goto _LL226;
	    }
  }
  else {
    goto _LL226;
  }
  _LL226: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_IntType : 0) {
    _LL324: _temp323 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f1;
	    if (_temp323 == (void *)Cyc_Absyn_Unsigned) {
	      goto _LL322;
	    }
	    else {
	      goto _LL228;
	    }
    _LL322: _temp321 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f2;
	    if (_temp321 == (void *)Cyc_Absyn_B2) {
	      goto _LL227;
	    }
	    else {
	      goto _LL228;
	    }
  }
  else {
    goto _LL228;
  }
  _LL228: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_IntType : 0) {
    _LL328: _temp327 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f1;
	    if (_temp327 == (void *)Cyc_Absyn_Signed) {
	      goto _LL326;
	    }
	    else {
	      goto _LL230;
	    }
    _LL326: _temp325 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f2;
	    if (_temp325 == (void *)Cyc_Absyn_B4) {
	      goto _LL229;
	    }
	    else {
	      goto _LL230;
	    }
  }
  else {
    goto _LL230;
  }
  _LL230: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_IntType : 0) {
    _LL332: _temp331 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f1;
	    if (_temp331 == (void *)Cyc_Absyn_Unsigned) {
	      goto _LL330;
	    }
	    else {
	      goto _LL232;
	    }
    _LL330: _temp329 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f2;
	    if (_temp329 == (void *)Cyc_Absyn_B4) {
	      goto _LL231;
	    }
	    else {
	      goto _LL232;
	    }
  }
  else {
    goto _LL232;
  }
  _LL232: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_IntType : 0) {
    _LL336: _temp335 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f1;
	    if (_temp335 == (void *)Cyc_Absyn_Signed) {
	      goto _LL334;
	    }
	    else {
	      goto _LL234;
	    }
    _LL334: _temp333 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f2;
	    if (_temp333 == (void *)Cyc_Absyn_B8) {
	      goto _LL233;
	    }
	    else {
	      goto _LL234;
	    }
  }
  else {
    goto _LL234;
  }
  _LL234: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_IntType : 0) {
    _LL340: _temp339 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f1;
	    if (_temp339 == (void *)Cyc_Absyn_Unsigned) {
	      goto _LL338;
	    }
	    else {
	      goto _LL236;
	    }
    _LL338: _temp337 = (void *)((struct Cyc_Absyn_IntType_struct *)_temp198)->f2;
	    if (_temp337 == (void *)Cyc_Absyn_B8) {
	      goto _LL235;
	    }
	    else {
	      goto _LL236;
	    }
  }
  else {
    goto _LL236;
  }
  _LL236: if (_temp198 == (void *)Cyc_Absyn_FloatType) {
    goto _LL237;
  }
  else {
    goto _LL238;
  }
  _LL238: if (_temp198 == (void *)Cyc_Absyn_DoubleType) {
    goto _LL239;
  }
  else {
    goto _LL240;
  }
  _LL240: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_TupleType : 0) {
    _LL342: _temp341 = (struct Cyc_List_List *)((struct Cyc_Absyn_TupleType_struct *)_temp198)->f1;
	    goto _LL241;
  }
  else {
    goto _LL242;
  }
  _LL242: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_StructType : 0) {
    _LL348: _temp347 = (struct _tuple0 *)((struct Cyc_Absyn_StructType_struct *)_temp198)->f1;
	    if (_temp347 == 0) {
	      goto _LL346;
	    }
	    else {
	      goto _LL244;
	    }
    _LL346: _temp345 = (struct Cyc_List_List *)((struct Cyc_Absyn_StructType_struct *)_temp198)->f2;
	    goto _LL344;
    _LL344: _temp343 = (struct Cyc_Absyn_Structdecl **)((struct Cyc_Absyn_StructType_struct *)_temp198)->f3;
	    goto _LL243;
  }
  else {
    goto _LL244;
  }
  _LL244: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_StructType : 0) {
    _LL354: _temp353 = (struct _tuple0 *)((struct Cyc_Absyn_StructType_struct *)_temp198)->f1;
	    goto _LL352;
    _LL352: _temp351 = (struct Cyc_List_List *)((struct Cyc_Absyn_StructType_struct *)_temp198)->f2;
	    goto _LL350;
    _LL350: _temp349 = (struct Cyc_Absyn_Structdecl **)((struct Cyc_Absyn_StructType_struct *)_temp198)->f3;
	    goto _LL245;
  }
  else {
    goto _LL246;
  }
  _LL246: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_UnionType : 0) {
    _LL360: _temp359 = (struct _tuple0 *)((struct Cyc_Absyn_UnionType_struct *)_temp198)->f1;
	    if (_temp359 == 0) {
	      goto _LL358;
	    }
	    else {
	      goto _LL248;
	    }
    _LL358: _temp357 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnionType_struct *)_temp198)->f2;
	    goto _LL356;
    _LL356: _temp355 = (struct Cyc_Absyn_Uniondecl **)((struct Cyc_Absyn_UnionType_struct *)_temp198)->f3;
	    goto _LL247;
  }
  else {
    goto _LL248;
  }
  _LL248: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_UnionType : 0) {
    _LL366: _temp365 = (struct _tuple0 *)((struct Cyc_Absyn_UnionType_struct *)_temp198)->f1;
	    goto _LL364;
    _LL364: _temp363 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnionType_struct *)_temp198)->f2;
	    goto _LL362;
    _LL362: _temp361 = (struct Cyc_Absyn_Uniondecl **)((struct Cyc_Absyn_UnionType_struct *)_temp198)->f3;
	    goto _LL249;
  }
  else {
    goto _LL250;
  }
  _LL250: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_AnonStructType : 0) {
    _LL368: _temp367 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonStructType_struct *)_temp198)->f1;
	    goto _LL251;
  }
  else {
    goto _LL252;
  }
  _LL252: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_AnonUnionType : 0) {
    _LL370: _temp369 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonUnionType_struct *)_temp198)->f1;
	    goto _LL253;
  }
  else {
    goto _LL254;
  }
  _LL254: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_TypedefType : 0) {
    _LL376: _temp375 = (struct _tuple0 *)((struct Cyc_Absyn_TypedefType_struct *)_temp198)->f1;
	    goto _LL374;
    _LL374: _temp373 = (struct Cyc_List_List *)((struct Cyc_Absyn_TypedefType_struct *)_temp198)->f2;
	    goto _LL372;
    _LL372: _temp371 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_TypedefType_struct *)_temp198)->f3;
	    goto _LL255;
  }
  else {
    goto _LL256;
  }
  _LL256: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_RgnHandleType : 0) {
    _LL378: _temp377 = (void *)((struct Cyc_Absyn_RgnHandleType_struct *)_temp198)->f1;
	    goto _LL257;
  }
  else {
    goto _LL258;
  }
  _LL258: if (_temp198 == (void *)Cyc_Absyn_HeapRgn) {
    goto _LL259;
  }
  else {
    goto _LL260;
  }
  _LL260: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_AccessEff : 0) {
    _LL380: _temp379 = (void *)((struct Cyc_Absyn_AccessEff_struct *)_temp198)->f1;
	    goto _LL261;
  }
  else {
    goto _LL262;
  }
  _LL262: if ((unsigned int)_temp198 > 4 ? *((int *)_temp198) == Cyc_Absyn_JoinEff : 0) {
    _LL382: _temp381 = (struct Cyc_List_List *)((struct Cyc_Absyn_JoinEff_struct *)_temp198)->f1;
	    goto _LL263;
  }
  else {
    goto _LL199;
  }
  _LL201: return;
  _LL203: return;
  _LL205: return;
  _LL207: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp383 = (char *)"void";
			       struct _tagged_string _temp384;
			       _temp384.curr = _temp383;
			       _temp384.base = _temp383;
			       _temp384.last_plus_one = _temp383 + 5;
			       _temp384; }));
	  return;
  _LL209: Cyc_Absyndump_dump_str(_temp274->name);
	  return;
  _LL211: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp385 = (char *)"%";
			       struct _tagged_string _temp386;
			       _temp386.curr = _temp385;
			       _temp386.base = _temp385;
			       _temp386.last_plus_one = _temp385 + 2;
			       _temp386; }));
	  Cyc_Absyndump_dumpkind(_temp280);
	  Cyc_Absyndump_dump(xprintf("(%d)",_temp276));
	  return;
  _LL213: Cyc_Absyndump_dumpntyp(_temp287);
	  return;
  _LL215: { void *_temp387 = _temp297;
    struct Cyc_Absyn_UnknownTunionInfo _temp393;
    int _temp395;
    struct _tuple0 *_temp397;
    struct Cyc_Absyn_Tuniondecl *_temp399;
    struct Cyc_Absyn_Tuniondecl _temp401;
    int _temp402;
    struct Cyc_Core_Opt *_temp404;
    struct Cyc_List_List *_temp406;
    struct _tuple0 *_temp408;
    void *_temp410;
    _LL389: if (*((int *)_temp387) == Cyc_Absyn_UnknownTunion) {
      _LL394: _temp393 = (struct Cyc_Absyn_UnknownTunionInfo)((struct Cyc_Absyn_UnknownTunion_struct *)_temp387)->f1;
	      _LL398: _temp397 = (struct _tuple0 *)_temp393.name;
		      goto _LL396;
	      _LL396: _temp395 = (int)_temp393.is_xtunion;
		      goto _LL390;
    }
    else {
      goto _LL391;
    }
    _LL391: if (*((int *)_temp387) == Cyc_Absyn_KnownTunion) {
      _LL400: _temp399 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_KnownTunion_struct *)_temp387)->f1;
	      _temp401 = *_temp399;
	      _LL411: _temp410 = (void *)_temp401.sc;
		      goto _LL409;
	      _LL409: _temp408 = (struct _tuple0 *)_temp401.name;
		      goto _LL407;
	      _LL407: _temp406 = (struct Cyc_List_List *)_temp401.tvs;
		      goto _LL405;
	      _LL405: _temp404 = (struct Cyc_Core_Opt *)_temp401.fields;
		      goto _LL403;
	      _LL403: _temp402 = (int)_temp401.is_xtunion;
		      goto _LL392;
    }
    else {
      goto _LL388;
    }
    _LL390: _temp408 = _temp397;
	    _temp402 = _temp395;
	    goto _LL392;
    _LL392: if (_temp402) {
	      Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp412 = (char *)"xtunion ";
				   struct _tagged_string _temp413;
				   _temp413.curr = _temp412;
				   _temp413.base = _temp412;
				   _temp413.last_plus_one = _temp412 + 9;
				   _temp413; }));
	    }
	    else {
	      Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp414 = (char *)"tunion ";
				   struct _tagged_string _temp415;
				   _temp415.curr = _temp414;
				   _temp415.base = _temp414;
				   _temp415.last_plus_one = _temp414 + 8;
				   _temp415; }));
	    }
	    { void *_temp416 = Cyc_Tcutil_compress(_temp293);
	      _LL418: if (_temp416 == (void *)Cyc_Absyn_HeapRgn) {
		goto _LL419;
	      }
	      else {
		goto _LL420;
	      }
	      _LL420: goto _LL421;
	      _LL419: goto _LL417;
	      _LL421: Cyc_Absyndump_dumptyp(_temp293);
		      Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp422 = (char *)" ";
					   struct _tagged_string _temp423;
					   _temp423.curr = _temp422;
					   _temp423.base = _temp422;
					   _temp423.last_plus_one = _temp422 + 2;
					   _temp423; }));
		      goto _LL417;
	      _LL417: ;
	    }
	    Cyc_Absyndump_dumpqvar(_temp408);
	    Cyc_Absyndump_dumptps(_temp295);
	    goto _LL388;
    _LL388: ;
  }
  goto _LL199;
  _LL217: { void *_temp424 = _temp303;
    struct Cyc_Absyn_UnknownTunionFieldInfo _temp430;
    int _temp432;
    struct _tuple0 *_temp434;
    struct _tuple0 *_temp436;
    struct Cyc_Absyn_Tunionfield *_temp438;
    struct Cyc_Absyn_Tunionfield _temp440;
    struct Cyc_Position_Segment *_temp441;
    struct Cyc_List_List *_temp443;
    struct Cyc_List_List *_temp445;
    struct _tuple0 *_temp447;
    struct Cyc_Absyn_Tuniondecl *_temp449;
    struct Cyc_Absyn_Tuniondecl _temp451;
    int _temp452;
    struct Cyc_Core_Opt *_temp454;
    struct Cyc_List_List *_temp456;
    struct _tuple0 *_temp458;
    void *_temp460;
    _LL426: if (*((int *)_temp424) == Cyc_Absyn_UnknownTunionfield) {
      _LL431: _temp430 = (struct Cyc_Absyn_UnknownTunionFieldInfo)((struct Cyc_Absyn_UnknownTunionfield_struct *)_temp424)->f1;
	      _LL437: _temp436 = (struct _tuple0 *)_temp430.tunion_name;
		      goto _LL435;
	      _LL435: _temp434 = (struct _tuple0 *)_temp430.field_name;
		      goto _LL433;
	      _LL433: _temp432 = (int)_temp430.is_xtunion;
		      goto _LL427;
    }
    else {
      goto _LL428;
    }
    _LL428: if (*((int *)_temp424) == Cyc_Absyn_KnownTunionfield) {
      _LL450: _temp449 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_KnownTunionfield_struct *)_temp424)->f1;
	      _temp451 = *_temp449;
	      _LL461: _temp460 = (void *)_temp451.sc;
		      goto _LL459;
	      _LL459: _temp458 = (struct _tuple0 *)_temp451.name;
		      goto _LL457;
	      _LL457: _temp456 = (struct Cyc_List_List *)_temp451.tvs;
		      goto _LL455;
	      _LL455: _temp454 = (struct Cyc_Core_Opt *)_temp451.fields;
		      goto _LL453;
	      _LL453: _temp452 = (int)_temp451.is_xtunion;
		      goto _LL439;
      _LL439: _temp438 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Absyn_KnownTunionfield_struct *)_temp424)->f2;
	      _temp440 = *_temp438;
	      _LL448: _temp447 = (struct _tuple0 *)_temp440.name;
		      goto _LL446;
	      _LL446: _temp445 = (struct Cyc_List_List *)_temp440.tvs;
		      goto _LL444;
	      _LL444: _temp443 = (struct Cyc_List_List *)_temp440.typs;
		      goto _LL442;
	      _LL442: _temp441 = (struct Cyc_Position_Segment *)_temp440.loc;
		      goto _LL429;
    }
    else {
      goto _LL425;
    }
    _LL427: _temp458 = _temp436;
	    _temp452 = _temp432;
	    _temp447 = _temp434;
	    goto _LL429;
    _LL429: if (_temp452) {
	      Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp462 = (char *)"xtunion ";
				   struct _tagged_string _temp463;
				   _temp463.curr = _temp462;
				   _temp463.base = _temp462;
				   _temp463.last_plus_one = _temp462 + 9;
				   _temp463; }));
	    }
	    else {
	      Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp464 = (char *)"tunion ";
				   struct _tagged_string _temp465;
				   _temp465.curr = _temp464;
				   _temp465.base = _temp464;
				   _temp465.last_plus_one = _temp464 + 8;
				   _temp465; }));
	    }
	    Cyc_Absyndump_dumpqvar(_temp458);
	    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp466 = (char *)".";
				 struct _tagged_string _temp467;
				 _temp467.curr = _temp466;
				 _temp467.base = _temp466;
				 _temp467.last_plus_one = _temp466 + 2;
				 _temp467; }));
	    Cyc_Absyndump_dumpqvar(_temp447);
	    Cyc_Absyndump_dumptps(_temp301);
	    goto _LL425;
    _LL425: ;
  }
  goto _LL199;
  _LL219: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp468 = (char *)"enum ";
			       struct _tagged_string _temp469;
			       _temp469.curr = _temp468;
			       _temp469.base = _temp468;
			       _temp469.last_plus_one = _temp468 + 6;
			       _temp469; }));
	  Cyc_Absyndump_dumpqvar(_temp307);
	  return;
  _LL221: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp470 = (char *)"signed char";
			       struct _tagged_string _temp471;
			       _temp471.curr = _temp470;
			       _temp471.base = _temp470;
			       _temp471.last_plus_one = _temp470 + 12;
			       _temp471; }));
	  return;
  _LL223: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp472 = (char *)"char";
			       struct _tagged_string _temp473;
			       _temp473.curr = _temp472;
			       _temp473.base = _temp472;
			       _temp473.last_plus_one = _temp472 + 5;
			       _temp473; }));
	  return;
  _LL225: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp474 = (char *)"short";
			       struct _tagged_string _temp475;
			       _temp475.curr = _temp474;
			       _temp475.base = _temp474;
			       _temp475.last_plus_one = _temp474 + 6;
			       _temp475; }));
	  return;
  _LL227: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp476 = (char *)"unsigned short";
			       struct _tagged_string _temp477;
			       _temp477.curr = _temp476;
			       _temp477.base = _temp476;
			       _temp477.last_plus_one = _temp476 + 15;
			       _temp477; }));
	  return;
  _LL229: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp478 = (char *)"int";
			       struct _tagged_string _temp479;
			       _temp479.curr = _temp478;
			       _temp479.base = _temp478;
			       _temp479.last_plus_one = _temp478 + 4;
			       _temp479; }));
	  return;
  _LL231: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp480 = (char *)"unsigned int";
			       struct _tagged_string _temp481;
			       _temp481.curr = _temp480;
			       _temp481.base = _temp480;
			       _temp481.last_plus_one = _temp480 + 13;
			       _temp481; }));
	  return;
  _LL233: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp482 = (char *)"long long";
			       struct _tagged_string _temp483;
			       _temp483.curr = _temp482;
			       _temp483.base = _temp482;
			       _temp483.last_plus_one = _temp482 + 10;
			       _temp483; }));
	  return;
  _LL235: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp484 = (char *)"unsigned long long";
			       struct _tagged_string _temp485;
			       _temp485.curr = _temp484;
			       _temp485.base = _temp484;
			       _temp485.last_plus_one = _temp484 + 19;
			       _temp485; }));
	  return;
  _LL237: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp486 = (char *)"float";
			       struct _tagged_string _temp487;
			       _temp487.curr = _temp486;
			       _temp487.base = _temp486;
			       _temp487.last_plus_one = _temp486 + 6;
			       _temp487; }));
	  return;
  _LL239: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp488 = (char *)"double";
			       struct _tagged_string _temp489;
			       _temp489.curr = _temp488;
			       _temp489.base = _temp488;
			       _temp489.last_plus_one = _temp488 + 7;
			       _temp489; }));
	  return;
  _LL241: Cyc_Absyndump_dump_char((int)'$');
	  Cyc_Absyndump_dumpargs(_temp341);
	  return;
  _LL243: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp490 = (char *)"struct";
			       struct _tagged_string _temp491;
			       _temp491.curr = _temp490;
			       _temp491.base = _temp490;
			       _temp491.last_plus_one = _temp490 + 7;
			       _temp491; }));
	  Cyc_Absyndump_dumptps(_temp345);
	  return;
  _LL245: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp492 = (char *)"struct";
			       struct _tagged_string _temp493;
			       _temp493.curr = _temp492;
			       _temp493.base = _temp492;
			       _temp493.last_plus_one = _temp492 + 7;
			       _temp493; }));
	  Cyc_Absyndump_dumpqvar((struct _tuple0 *)({ struct _tuple0 *_temp494 = _temp353;
				   if (_temp494 == 0) {
				     _throw(Null_Exception);
				   }
				   _temp494; }));
	  Cyc_Absyndump_dumptps(_temp351);
	  return;
  _LL247: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp495 = (char *)"union";
			       struct _tagged_string _temp496;
			       _temp496.curr = _temp495;
			       _temp496.base = _temp495;
			       _temp496.last_plus_one = _temp495 + 6;
			       _temp496; }));
	  Cyc_Absyndump_dumptps(_temp357);
	  return;
  _LL249: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp497 = (char *)"union";
			       struct _tagged_string _temp498;
			       _temp498.curr = _temp497;
			       _temp498.base = _temp497;
			       _temp498.last_plus_one = _temp497 + 6;
			       _temp498; }));
	  Cyc_Absyndump_dumpqvar((struct _tuple0 *)({ struct _tuple0 *_temp499 = _temp365;
				   if (_temp499 == 0) {
				     _throw(Null_Exception);
				   }
				   _temp499; }));
	  Cyc_Absyndump_dumptps(_temp363);
	  return;
  _LL251: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp500 = (char *)"struct {";
			       struct _tagged_string _temp501;
			       _temp501.curr = _temp500;
			       _temp501.base = _temp500;
			       _temp501.last_plus_one = _temp500 + 9;
			       _temp501; }));
	  Cyc_Absyndump_dumpstructfields(_temp367);
	  Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp502 = (char *)"}";
			       struct _tagged_string _temp503;
			       _temp503.curr = _temp502;
			       _temp503.base = _temp502;
			       _temp503.last_plus_one = _temp502 + 2;
			       _temp503; }));
	  return;
  _LL253: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp504 = (char *)"union {";
			       struct _tagged_string _temp505;
			       _temp505.curr = _temp504;
			       _temp505.base = _temp504;
			       _temp505.last_plus_one = _temp504 + 8;
			       _temp505; }));
	  Cyc_Absyndump_dumpstructfields(_temp369);
	  Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp506 = (char *)"}";
			       struct _tagged_string _temp507;
			       _temp507.curr = _temp506;
			       _temp507.base = _temp506;
			       _temp507.last_plus_one = _temp506 + 2;
			       _temp507; }));
	  return;
  _LL255: Cyc_Absyndump_dumpqvar(_temp375), Cyc_Absyndump_dumptps(_temp373);
	  return;
  _LL257: Cyc_Absyndump_dumprgn(_temp377);
	  return;
  _LL259: return;
  _LL261: return;
  _LL263: return;
  _LL199: ;
}
void Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt *vo) {
  if (vo != 0) {
    Cyc_Absyndump_dump_str((struct _tagged_string *)({ struct Cyc_Core_Opt *_temp508 = vo;
			     if (_temp508 == 0) {
			       _throw(Null_Exception);
			     }
			     _temp508->v; }));
  }
}
void Cyc_Absyndump_dumpfunarg(struct _tuple1 *t) {
  ((void (*)(struct Cyc_Absyn_Tqual,void *,void (*f)(struct Cyc_Core_Opt *),
	     struct Cyc_Core_Opt *))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,
							    Cyc_Absyndump_dumpvaropt,
							    (*t).f1);
}
void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List *args,int varargs,struct Cyc_Core_Opt *effopt) {
  Cyc_Absyndump_dump_char((int)'(');
  for(0; args != 0; args = ({ struct Cyc_List_List *_temp509 = args;
    if (_temp509 == 0) {
      _throw(Null_Exception);
    }
    _temp509->tl; })) {
    Cyc_Absyndump_dumpfunarg((struct _tuple1 *)({ struct Cyc_List_List *_temp510 = args;
			       if (_temp510 == 0) {
				 _throw(Null_Exception);
			       }
			       _temp510->hd; }));
    if (({ struct Cyc_List_List *_temp511 = args;
      if (_temp511 == 0) {
	_throw(Null_Exception);
      }
      _temp511->tl; }) != 0 ? 1 : varargs) {
      Cyc_Absyndump_dump_char((int)',');
    }
  }
  if (varargs) {
    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp512 = (char *)"...";
			 struct _tagged_string _temp513;
			 _temp513.curr = _temp512;
			 _temp513.base = _temp512;
			 _temp513.last_plus_one = _temp512 + 4;
			 _temp513; }));
  }
  if (effopt != 0) {
    Cyc_Absyndump_dump_semi();
    Cyc_Absyndump_dumpeff((void *)({ struct Cyc_Core_Opt *_temp514 = effopt;
			    if (_temp514 == 0) {
			      _throw(Null_Exception);
			    }
			    _temp514->v; }));
  }
  Cyc_Absyndump_dump_char((int)')');
}
void Cyc_Absyndump_dumptyp(void *t) {
  ((void (*)(struct Cyc_Absyn_Tqual,void *,void (*f)(int),int))Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual _temp515;
											 _temp515.q_const = 0;
											 _temp515.q_volatile = 0;
											 _temp515.q_restrict = 0;
											 _temp515; }),
										       t,
										       (void (*)(int x))Cyc_Absyndump_ignore,
										       0);
}
void Cyc_Absyndump_dumpdesignator(void *d) {
  void *_temp516 = d;
  struct Cyc_Absyn_Exp *_temp522;
  struct _tagged_string *_temp524;
  _LL518: if (*((int *)_temp516) == Cyc_Absyn_ArrayElement) {
    _LL523: _temp522 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ArrayElement_struct *)_temp516)->f1;
	    goto _LL519;
  }
  else {
    goto _LL520;
  }
  _LL520: if (*((int *)_temp516) == Cyc_Absyn_FieldName) {
    _LL525: _temp524 = (struct _tagged_string *)((struct Cyc_Absyn_FieldName_struct *)_temp516)->f1;
	    goto _LL521;
  }
  else {
    goto _LL517;
  }
  _LL519: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp526 = (char *)".[";
			       struct _tagged_string _temp527;
			       _temp527.curr = _temp526;
			       _temp527.base = _temp526;
			       _temp527.last_plus_one = _temp526 + 3;
			       _temp527; }));
	  Cyc_Absyndump_dumpexp(_temp522);
	  Cyc_Absyndump_dump_char((int)']');
	  goto _LL517;
  _LL521: Cyc_Absyndump_dump_char((int)'.');
	  Cyc_Absyndump_dump_nospace(*_temp524);
	  goto _LL517;
  _LL517: ;
}
struct _tuple7 {
  struct Cyc_List_List *f1;
  struct Cyc_Absyn_Exp *f2;
};
void Cyc_Absyndump_dumpde(struct _tuple7 *de) {
  ((void (*)(void (*f)(void *),struct Cyc_List_List *l,struct _tagged_string start,
	     struct _tagged_string end,struct _tagged_string sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,
											(*de).f1,
											(struct _tagged_string)({ char *_temp528 = (char *)"";
											  struct _tagged_string _temp529;
											  _temp529.curr = _temp528;
											  _temp529.base = _temp528;
											  _temp529.last_plus_one = _temp528 + 1;
											  _temp529; }),
											(struct _tagged_string)({ char *_temp530 = (char *)"=";
											  struct _tagged_string _temp531;
											  _temp531.curr = _temp530;
											  _temp531.base = _temp530;
											  _temp531.last_plus_one = _temp530 + 2;
											  _temp531; }),
											(struct _tagged_string)({ char *_temp532 = (char *)"=";
											  struct _tagged_string _temp533;
											  _temp533.curr = _temp532;
											  _temp533.base = _temp532;
											  _temp533.last_plus_one = _temp532 + 2;
											  _temp533; }));
  Cyc_Absyndump_dumpexp((*de).f2);
}
void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List *es) {
  ((void (*)(void (*f)(int,struct Cyc_Absyn_Exp *),int env,struct Cyc_List_List *l,
	     struct _tagged_string start,struct _tagged_string end,struct _tagged_string sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,
														     inprec,
														     es,
														     (struct _tagged_string)({ char *_temp534 = (char *)"";
														       struct _tagged_string _temp535;
														       _temp535.curr = _temp534;
														       _temp535.base = _temp534;
														       _temp535.last_plus_one = _temp534 + 1;
														       _temp535; }),
														     (struct _tagged_string)({ char *_temp536 = (char *)"";
														       struct _tagged_string _temp537;
														       _temp537.curr = _temp536;
														       _temp537.base = _temp536;
														       _temp537.last_plus_one = _temp536 + 1;
														       _temp537; }),
														     (struct _tagged_string)({ char *_temp538 = (char *)",";
														       struct _tagged_string _temp539;
														       _temp539.curr = _temp538;
														       _temp539.base = _temp538;
														       _temp539.last_plus_one = _temp538 + 2;
														       _temp539; }));
}
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp *e) {
  int myprec = Cyc_Absynpp_exp_prec(e);
  if (inprec >= myprec) {
    Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp540 = (char *)"(";
				 struct _tagged_string _temp541;
				 _temp541.curr = _temp540;
				 _temp541.base = _temp540;
				 _temp541.last_plus_one = _temp540 + 2;
				 _temp541; }));
  }
  { void *_temp542 = (void *)e->r;
    void *_temp634;
    char _temp636;
    void *_temp638;
    void *_temp640;
    short _temp642;
    void *_temp644;
    void *_temp646;
    int _temp648;
    void *_temp650;
    void *_temp652;
    int _temp654;
    void *_temp656;
    void *_temp658;
    long long _temp660;
    void *_temp662;
    void *_temp664;
    struct _tagged_string _temp666;
    void *_temp668;
    void *_temp670;
    struct _tagged_string _temp672;
    struct _tuple0 *_temp674;
    void *_temp676;
    struct _tuple0 *_temp678;
    struct Cyc_List_List *_temp680;
    void *_temp682;
    struct Cyc_Absyn_Exp *_temp684;
    struct Cyc_Core_Opt *_temp686;
    struct Cyc_Absyn_Exp *_temp688;
    void *_temp690;
    struct Cyc_Absyn_Exp *_temp692;
    void *_temp694;
    struct Cyc_Absyn_Exp *_temp696;
    void *_temp698;
    struct Cyc_Absyn_Exp *_temp700;
    void *_temp702;
    struct Cyc_Absyn_Exp *_temp704;
    struct Cyc_Absyn_Exp *_temp706;
    struct Cyc_Absyn_Exp *_temp708;
    struct Cyc_Absyn_Exp *_temp710;
    struct Cyc_Absyn_Exp *_temp712;
    struct Cyc_Absyn_Exp *_temp714;
    struct Cyc_List_List *_temp716;
    struct Cyc_Absyn_Exp *_temp718;
    struct Cyc_List_List *_temp720;
    struct Cyc_Absyn_Exp *_temp722;
    struct Cyc_Absyn_Exp *_temp724;
    struct Cyc_Absyn_Exp *_temp726;
    struct Cyc_List_List *_temp728;
    struct Cyc_Absyn_Exp *_temp730;
    struct Cyc_Absyn_Exp *_temp732;
    void *_temp734;
    struct Cyc_Absyn_Exp *_temp736;
    struct Cyc_Absyn_Exp *_temp738;
    struct Cyc_Absyn_Exp *_temp740;
    void *_temp742;
    struct Cyc_Absyn_Exp *_temp744;
    struct Cyc_Absyn_Exp *_temp746;
    struct _tagged_string *_temp748;
    struct Cyc_Absyn_Exp *_temp750;
    struct _tagged_string *_temp752;
    struct Cyc_Absyn_Exp *_temp754;
    struct Cyc_Absyn_Exp *_temp756;
    struct Cyc_Absyn_Exp *_temp758;
    struct Cyc_List_List *_temp760;
    struct Cyc_List_List *_temp762;
    struct _tuple1 *_temp764;
    struct Cyc_List_List *_temp766;
    struct Cyc_Absyn_Exp *_temp768;
    struct Cyc_Absyn_Exp *_temp770;
    struct Cyc_Absyn_Vardecl *_temp772;
    struct Cyc_Absyn_Structdecl *_temp774;
    struct Cyc_List_List *_temp776;
    struct Cyc_Core_Opt *_temp778;
    struct _tuple0 *_temp780;
    struct Cyc_List_List *_temp782;
    void *_temp784;
    struct Cyc_Absyn_Tunionfield *_temp786;
    struct Cyc_Absyn_Tuniondecl *_temp788;
    struct Cyc_List_List *_temp790;
    struct Cyc_Core_Opt *_temp792;
    struct Cyc_Core_Opt *_temp794;
    struct Cyc_Absyn_Enumfield *_temp796;
    struct Cyc_Absyn_Enumdecl *_temp798;
    struct _tuple0 *_temp800;
    void *_temp802;
    struct Cyc_Absyn_Exp *_temp804;
    struct Cyc_List_List *_temp806;
    struct Cyc_Core_Opt *_temp808;
    struct Cyc_Absyn_Stmt *_temp810;
    struct Cyc_Absyn_Fndecl *_temp812;
    struct Cyc_Absyn_Exp *_temp814;
    _LL544: if (*((int *)_temp542) == Cyc_Absyn_Const_e) {
      _LL635: _temp634 = (void *)((struct Cyc_Absyn_Const_e_struct *)_temp542)->f1;
	      if ((unsigned int)_temp634 > 1 ? *((int *)_temp634) == Cyc_Absyn_Char_c : 0) {
		_LL639: _temp638 = (void *)((struct Cyc_Absyn_Char_c_struct *)_temp634)->f1;
			goto _LL637;
		_LL637: _temp636 = (char)((struct Cyc_Absyn_Char_c_struct *)_temp634)->f2;
			goto _LL545;
	      }
	      else {
		goto _LL546;
	      }
    }
    else {
      goto _LL546;
    }
    _LL546: if (*((int *)_temp542) == Cyc_Absyn_Const_e) {
      _LL641: _temp640 = (void *)((struct Cyc_Absyn_Const_e_struct *)_temp542)->f1;
	      if ((unsigned int)_temp640 > 1 ? *((int *)_temp640) == Cyc_Absyn_Short_c : 0) {
		_LL645: _temp644 = (void *)((struct Cyc_Absyn_Short_c_struct *)_temp640)->f1;
			goto _LL643;
		_LL643: _temp642 = (short)((struct Cyc_Absyn_Short_c_struct *)_temp640)->f2;
			goto _LL547;
	      }
	      else {
		goto _LL548;
	      }
    }
    else {
      goto _LL548;
    }
    _LL548: if (*((int *)_temp542) == Cyc_Absyn_Const_e) {
      _LL647: _temp646 = (void *)((struct Cyc_Absyn_Const_e_struct *)_temp542)->f1;
	      if ((unsigned int)_temp646 > 1 ? *((int *)_temp646) == Cyc_Absyn_Int_c : 0) {
		_LL651: _temp650 = (void *)((struct Cyc_Absyn_Int_c_struct *)_temp646)->f1;
			if (_temp650 == (void *)Cyc_Absyn_Signed) {
			  goto _LL649;
			}
			else {
			  goto _LL550;
			}
		_LL649: _temp648 = (int)((struct Cyc_Absyn_Int_c_struct *)_temp646)->f2;
			goto _LL549;
	      }
	      else {
		goto _LL550;
	      }
    }
    else {
      goto _LL550;
    }
    _LL550: if (*((int *)_temp542) == Cyc_Absyn_Const_e) {
      _LL653: _temp652 = (void *)((struct Cyc_Absyn_Const_e_struct *)_temp542)->f1;
	      if ((unsigned int)_temp652 > 1 ? *((int *)_temp652) == Cyc_Absyn_Int_c : 0) {
		_LL657: _temp656 = (void *)((struct Cyc_Absyn_Int_c_struct *)_temp652)->f1;
			if (_temp656 == (void *)Cyc_Absyn_Unsigned) {
			  goto _LL655;
			}
			else {
			  goto _LL552;
			}
		_LL655: _temp654 = (int)((struct Cyc_Absyn_Int_c_struct *)_temp652)->f2;
			goto _LL551;
	      }
	      else {
		goto _LL552;
	      }
    }
    else {
      goto _LL552;
    }
    _LL552: if (*((int *)_temp542) == Cyc_Absyn_Const_e) {
      _LL659: _temp658 = (void *)((struct Cyc_Absyn_Const_e_struct *)_temp542)->f1;
	      if ((unsigned int)_temp658 > 1 ? *((int *)_temp658) == Cyc_Absyn_LongLong_c : 0) {
		_LL663: _temp662 = (void *)((struct Cyc_Absyn_LongLong_c_struct *)_temp658)->f1;
			goto _LL661;
		_LL661: _temp660 = (long long)((struct Cyc_Absyn_LongLong_c_struct *)_temp658)->f2;
			goto _LL553;
	      }
	      else {
		goto _LL554;
	      }
    }
    else {
      goto _LL554;
    }
    _LL554: if (*((int *)_temp542) == Cyc_Absyn_Const_e) {
      _LL665: _temp664 = (void *)((struct Cyc_Absyn_Const_e_struct *)_temp542)->f1;
	      if ((unsigned int)_temp664 > 1 ? *((int *)_temp664) == Cyc_Absyn_Float_c : 0) {
		_LL667: _temp666 = (struct _tagged_string)((struct Cyc_Absyn_Float_c_struct *)_temp664)->f1;
			goto _LL555;
	      }
	      else {
		goto _LL556;
	      }
    }
    else {
      goto _LL556;
    }
    _LL556: if (*((int *)_temp542) == Cyc_Absyn_Const_e) {
      _LL669: _temp668 = (void *)((struct Cyc_Absyn_Const_e_struct *)_temp542)->f1;
	      if (_temp668 == (void *)Cyc_Absyn_Null_c) {
		goto _LL557;
	      }
	      else {
		goto _LL558;
	      }
    }
    else {
      goto _LL558;
    }
    _LL558: if (*((int *)_temp542) == Cyc_Absyn_Const_e) {
      _LL671: _temp670 = (void *)((struct Cyc_Absyn_Const_e_struct *)_temp542)->f1;
	      if ((unsigned int)_temp670 > 1 ? *((int *)_temp670) == Cyc_Absyn_String_c : 0) {
		_LL673: _temp672 = (struct _tagged_string)((struct Cyc_Absyn_String_c_struct *)_temp670)->f1;
			goto _LL559;
	      }
	      else {
		goto _LL560;
	      }
    }
    else {
      goto _LL560;
    }
    _LL560: if (*((int *)_temp542) == Cyc_Absyn_UnknownId_e) {
      _LL675: _temp674 = (struct _tuple0 *)((struct Cyc_Absyn_UnknownId_e_struct *)_temp542)->f1;
	      goto _LL561;
    }
    else {
      goto _LL562;
    }
    _LL562: if (*((int *)_temp542) == Cyc_Absyn_Var_e) {
      _LL679: _temp678 = (struct _tuple0 *)((struct Cyc_Absyn_Var_e_struct *)_temp542)->f1;
	      goto _LL677;
      _LL677: _temp676 = (void *)((struct Cyc_Absyn_Var_e_struct *)_temp542)->f2;
	      goto _LL563;
    }
    else {
      goto _LL564;
    }
    _LL564: if (*((int *)_temp542) == Cyc_Absyn_Primop_e) {
      _LL683: _temp682 = (void *)((struct Cyc_Absyn_Primop_e_struct *)_temp542)->f1;
	      goto _LL681;
      _LL681: _temp680 = (struct Cyc_List_List *)((struct Cyc_Absyn_Primop_e_struct *)_temp542)->f2;
	      goto _LL565;
    }
    else {
      goto _LL566;
    }
    _LL566: if (*((int *)_temp542) == Cyc_Absyn_AssignOp_e) {
      _LL689: _temp688 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_AssignOp_e_struct *)_temp542)->f1;
	      goto _LL687;
      _LL687: _temp686 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_AssignOp_e_struct *)_temp542)->f2;
	      goto _LL685;
      _LL685: _temp684 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_AssignOp_e_struct *)_temp542)->f3;
	      goto _LL567;
    }
    else {
      goto _LL568;
    }
    _LL568: if (*((int *)_temp542) == Cyc_Absyn_Increment_e) {
      _LL693: _temp692 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Increment_e_struct *)_temp542)->f1;
	      goto _LL691;
      _LL691: _temp690 = (void *)((struct Cyc_Absyn_Increment_e_struct *)_temp542)->f2;
	      if (_temp690 == (void *)Cyc_Absyn_PreInc) {
		goto _LL569;
	      }
	      else {
		goto _LL570;
	      }
    }
    else {
      goto _LL570;
    }
    _LL570: if (*((int *)_temp542) == Cyc_Absyn_Increment_e) {
      _LL697: _temp696 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Increment_e_struct *)_temp542)->f1;
	      goto _LL695;
      _LL695: _temp694 = (void *)((struct Cyc_Absyn_Increment_e_struct *)_temp542)->f2;
	      if (_temp694 == (void *)Cyc_Absyn_PreDec) {
		goto _LL571;
	      }
	      else {
		goto _LL572;
	      }
    }
    else {
      goto _LL572;
    }
    _LL572: if (*((int *)_temp542) == Cyc_Absyn_Increment_e) {
      _LL701: _temp700 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Increment_e_struct *)_temp542)->f1;
	      goto _LL699;
      _LL699: _temp698 = (void *)((struct Cyc_Absyn_Increment_e_struct *)_temp542)->f2;
	      if (_temp698 == (void *)Cyc_Absyn_PostInc) {
		goto _LL573;
	      }
	      else {
		goto _LL574;
	      }
    }
    else {
      goto _LL574;
    }
    _LL574: if (*((int *)_temp542) == Cyc_Absyn_Increment_e) {
      _LL705: _temp704 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Increment_e_struct *)_temp542)->f1;
	      goto _LL703;
      _LL703: _temp702 = (void *)((struct Cyc_Absyn_Increment_e_struct *)_temp542)->f2;
	      if (_temp702 == (void *)Cyc_Absyn_PostDec) {
		goto _LL575;
	      }
	      else {
		goto _LL576;
	      }
    }
    else {
      goto _LL576;
    }
    _LL576: if (*((int *)_temp542) == Cyc_Absyn_Conditional_e) {
      _LL711: _temp710 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Conditional_e_struct *)_temp542)->f1;
	      goto _LL709;
      _LL709: _temp708 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Conditional_e_struct *)_temp542)->f2;
	      goto _LL707;
      _LL707: _temp706 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Conditional_e_struct *)_temp542)->f3;
	      goto _LL577;
    }
    else {
      goto _LL578;
    }
    _LL578: if (*((int *)_temp542) == Cyc_Absyn_SeqExp_e) {
      _LL715: _temp714 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_SeqExp_e_struct *)_temp542)->f1;
	      goto _LL713;
      _LL713: _temp712 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_SeqExp_e_struct *)_temp542)->f2;
	      goto _LL579;
    }
    else {
      goto _LL580;
    }
    _LL580: if (*((int *)_temp542) == Cyc_Absyn_UnknownCall_e) {
      _LL719: _temp718 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_UnknownCall_e_struct *)_temp542)->f1;
	      goto _LL717;
      _LL717: _temp716 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownCall_e_struct *)_temp542)->f2;
	      goto _LL581;
    }
    else {
      goto _LL582;
    }
    _LL582: if (*((int *)_temp542) == Cyc_Absyn_FnCall_e) {
      _LL723: _temp722 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_FnCall_e_struct *)_temp542)->f1;
	      goto _LL721;
      _LL721: _temp720 = (struct Cyc_List_List *)((struct Cyc_Absyn_FnCall_e_struct *)_temp542)->f2;
	      goto _LL583;
    }
    else {
      goto _LL584;
    }
    _LL584: if (*((int *)_temp542) == Cyc_Absyn_Throw_e) {
      _LL725: _temp724 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Throw_e_struct *)_temp542)->f1;
	      goto _LL585;
    }
    else {
      goto _LL586;
    }
    _LL586: if (*((int *)_temp542) == Cyc_Absyn_NoInstantiate_e) {
      _LL727: _temp726 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_NoInstantiate_e_struct *)_temp542)->f1;
	      goto _LL587;
    }
    else {
      goto _LL588;
    }
    _LL588: if (*((int *)_temp542) == Cyc_Absyn_Instantiate_e) {
      _LL731: _temp730 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Instantiate_e_struct *)_temp542)->f1;
	      goto _LL729;
      _LL729: _temp728 = (struct Cyc_List_List *)((struct Cyc_Absyn_Instantiate_e_struct *)_temp542)->f2;
	      goto _LL589;
    }
    else {
      goto _LL590;
    }
    _LL590: if (*((int *)_temp542) == Cyc_Absyn_Cast_e) {
      _LL735: _temp734 = (void *)((struct Cyc_Absyn_Cast_e_struct *)_temp542)->f1;
	      goto _LL733;
      _LL733: _temp732 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Cast_e_struct *)_temp542)->f2;
	      goto _LL591;
    }
    else {
      goto _LL592;
    }
    _LL592: if (*((int *)_temp542) == Cyc_Absyn_Address_e) {
      _LL737: _temp736 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Address_e_struct *)_temp542)->f1;
	      goto _LL593;
    }
    else {
      goto _LL594;
    }
    _LL594: if (*((int *)_temp542) == Cyc_Absyn_New_e) {
      _LL741: _temp740 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_New_e_struct *)_temp542)->f1;
	      goto _LL739;
      _LL739: _temp738 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_New_e_struct *)_temp542)->f2;
	      goto _LL595;
    }
    else {
      goto _LL596;
    }
    _LL596: if (*((int *)_temp542) == Cyc_Absyn_Sizeoftyp_e) {
      _LL743: _temp742 = (void *)((struct Cyc_Absyn_Sizeoftyp_e_struct *)_temp542)->f1;
	      goto _LL597;
    }
    else {
      goto _LL598;
    }
    _LL598: if (*((int *)_temp542) == Cyc_Absyn_Sizeofexp_e) {
      _LL745: _temp744 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Sizeofexp_e_struct *)_temp542)->f1;
	      goto _LL599;
    }
    else {
      goto _LL600;
    }
    _LL600: if (*((int *)_temp542) == Cyc_Absyn_Deref_e) {
      _LL747: _temp746 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Deref_e_struct *)_temp542)->f1;
	      goto _LL601;
    }
    else {
      goto _LL602;
    }
    _LL602: if (*((int *)_temp542) == Cyc_Absyn_StructMember_e) {
      _LL751: _temp750 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_StructMember_e_struct *)_temp542)->f1;
	      goto _LL749;
      _LL749: _temp748 = (struct _tagged_string *)((struct Cyc_Absyn_StructMember_e_struct *)_temp542)->f2;
	      goto _LL603;
    }
    else {
      goto _LL604;
    }
    _LL604: if (*((int *)_temp542) == Cyc_Absyn_StructArrow_e) {
      _LL755: _temp754 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_StructArrow_e_struct *)_temp542)->f1;
	      goto _LL753;
      _LL753: _temp752 = (struct _tagged_string *)((struct Cyc_Absyn_StructArrow_e_struct *)_temp542)->f2;
	      goto _LL605;
    }
    else {
      goto _LL606;
    }
    _LL606: if (*((int *)_temp542) == Cyc_Absyn_Subscript_e) {
      _LL759: _temp758 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Subscript_e_struct *)_temp542)->f1;
	      goto _LL757;
      _LL757: _temp756 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Subscript_e_struct *)_temp542)->f2;
	      goto _LL607;
    }
    else {
      goto _LL608;
    }
    _LL608: if (*((int *)_temp542) == Cyc_Absyn_Tuple_e) {
      _LL761: _temp760 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tuple_e_struct *)_temp542)->f1;
	      goto _LL609;
    }
    else {
      goto _LL610;
    }
    _LL610: if (*((int *)_temp542) == Cyc_Absyn_CompoundLit_e) {
      _LL765: _temp764 = (struct _tuple1 *)((struct Cyc_Absyn_CompoundLit_e_struct *)_temp542)->f1;
	      goto _LL763;
      _LL763: _temp762 = (struct Cyc_List_List *)((struct Cyc_Absyn_CompoundLit_e_struct *)_temp542)->f2;
	      goto _LL611;
    }
    else {
      goto _LL612;
    }
    _LL612: if (*((int *)_temp542) == Cyc_Absyn_Array_e) {
      _LL767: _temp766 = (struct Cyc_List_List *)((struct Cyc_Absyn_Array_e_struct *)_temp542)->f1;
	      goto _LL613;
    }
    else {
      goto _LL614;
    }
    _LL614: if (*((int *)_temp542) == Cyc_Absyn_Comprehension_e) {
      _LL773: _temp772 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Comprehension_e_struct *)_temp542)->f1;
	      goto _LL771;
      _LL771: _temp770 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Comprehension_e_struct *)_temp542)->f2;
	      goto _LL769;
      _LL769: _temp768 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Comprehension_e_struct *)_temp542)->f3;
	      goto _LL615;
    }
    else {
      goto _LL616;
    }
    _LL616: if (*((int *)_temp542) == Cyc_Absyn_Struct_e) {
      _LL781: _temp780 = (struct _tuple0 *)((struct Cyc_Absyn_Struct_e_struct *)_temp542)->f1;
	      goto _LL779;
      _LL779: _temp778 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Struct_e_struct *)_temp542)->f2;
	      goto _LL777;
      _LL777: _temp776 = (struct Cyc_List_List *)((struct Cyc_Absyn_Struct_e_struct *)_temp542)->f3;
	      goto _LL775;
      _LL775: _temp774 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Absyn_Struct_e_struct *)_temp542)->f4;
	      goto _LL617;
    }
    else {
      goto _LL618;
    }
    _LL618: if (*((int *)_temp542) == Cyc_Absyn_AnonStruct_e) {
      _LL785: _temp784 = (void *)((struct Cyc_Absyn_AnonStruct_e_struct *)_temp542)->f1;
	      goto _LL783;
      _LL783: _temp782 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonStruct_e_struct *)_temp542)->f2;
	      goto _LL619;
    }
    else {
      goto _LL620;
    }
    _LL620: if (*((int *)_temp542) == Cyc_Absyn_Tunion_e) {
      _LL795: _temp794 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Tunion_e_struct *)_temp542)->f1;
	      goto _LL793;
      _LL793: _temp792 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Tunion_e_struct *)_temp542)->f2;
	      goto _LL791;
      _LL791: _temp790 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_e_struct *)_temp542)->f3;
	      goto _LL789;
      _LL789: _temp788 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_Tunion_e_struct *)_temp542)->f4;
	      goto _LL787;
      _LL787: _temp786 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Absyn_Tunion_e_struct *)_temp542)->f5;
	      goto _LL621;
    }
    else {
      goto _LL622;
    }
    _LL622: if (*((int *)_temp542) == Cyc_Absyn_Enum_e) {
      _LL801: _temp800 = (struct _tuple0 *)((struct Cyc_Absyn_Enum_e_struct *)_temp542)->f1;
	      goto _LL799;
      _LL799: _temp798 = (struct Cyc_Absyn_Enumdecl *)((struct Cyc_Absyn_Enum_e_struct *)_temp542)->f2;
	      goto _LL797;
      _LL797: _temp796 = (struct Cyc_Absyn_Enumfield *)((struct Cyc_Absyn_Enum_e_struct *)_temp542)->f3;
	      goto _LL623;
    }
    else {
      goto _LL624;
    }
    _LL624: if (*((int *)_temp542) == Cyc_Absyn_Malloc_e) {
      _LL805: _temp804 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Malloc_e_struct *)_temp542)->f1;
	      goto _LL803;
      _LL803: _temp802 = (void *)((struct Cyc_Absyn_Malloc_e_struct *)_temp542)->f2;
	      goto _LL625;
    }
    else {
      goto _LL626;
    }
    _LL626: if (*((int *)_temp542) == Cyc_Absyn_UnresolvedMem_e) {
      _LL809: _temp808 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_UnresolvedMem_e_struct *)_temp542)->f1;
	      goto _LL807;
      _LL807: _temp806 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnresolvedMem_e_struct *)_temp542)->f2;
	      goto _LL627;
    }
    else {
      goto _LL628;
    }
    _LL628: if (*((int *)_temp542) == Cyc_Absyn_StmtExp_e) {
      _LL811: _temp810 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_StmtExp_e_struct *)_temp542)->f1;
	      goto _LL629;
    }
    else {
      goto _LL630;
    }
    _LL630: if (*((int *)_temp542) == Cyc_Absyn_Codegen_e) {
      _LL813: _temp812 = (struct Cyc_Absyn_Fndecl *)((struct Cyc_Absyn_Codegen_e_struct *)_temp542)->f1;
	      goto _LL631;
    }
    else {
      goto _LL632;
    }
    _LL632: if (*((int *)_temp542) == Cyc_Absyn_Fill_e) {
      _LL815: _temp814 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Fill_e_struct *)_temp542)->f1;
	      goto _LL633;
    }
    else {
      goto _LL543;
    }
    _LL545: Cyc_Absyndump_dump_char((int)'\'');
	    Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_temp636));
	    Cyc_Absyndump_dump_char((int)'\'');
	    goto _LL543;
    _LL547: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp816 = (char *)"(short)";
				 struct _tagged_string _temp817;
				 _temp817.curr = _temp816;
				 _temp817.base = _temp816;
				 _temp817.last_plus_one = _temp816 + 8;
				 _temp817; }));
	    Cyc_Absyndump_dump_nospace(Cyc_Core_string_of_int((int)_temp642));
	    goto _LL543;
    _LL549: Cyc_Absyndump_dump(Cyc_Core_string_of_int(_temp648));
	    goto _LL543;
    _LL551: Cyc_Absyndump_dump(Cyc_Core_string_of_int(_temp654));
	    Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp818 = (char *)"u";
					 struct _tagged_string _temp819;
					 _temp819.curr = _temp818;
					 _temp819.base = _temp818;
					 _temp819.last_plus_one = _temp818 + 2;
					 _temp819; }));
	    goto _LL543;
    _LL553: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp820 = (char *)"<<FIX LONG LONG CONSTANT>>";
				 struct _tagged_string _temp821;
				 _temp821.curr = _temp820;
				 _temp821.base = _temp820;
				 _temp821.last_plus_one = _temp820 + 27;
				 _temp821; }));
	    goto _LL543;
    _LL555: Cyc_Absyndump_dump(_temp666);
	    goto _LL543;
    _LL557: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp822 = (char *)"null";
				 struct _tagged_string _temp823;
				 _temp823.curr = _temp822;
				 _temp823.base = _temp822;
				 _temp823.last_plus_one = _temp822 + 5;
				 _temp823; }));
	    goto _LL543;
    _LL559: Cyc_Absyndump_dump_char((int)'"');
	    Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_temp672));
	    Cyc_Absyndump_dump_char((int)'"');
	    goto _LL543;
    _LL561: _temp678 = _temp674;
	    goto _LL563;
    _LL563: Cyc_Absyndump_dumpqvar(_temp678);
	    goto _LL543;
    _LL565: {
    struct _tagged_string pstr = Cyc_Absynpp_prim2str(_temp682);
      if (! Cyc_Absyn_is_format_prim(_temp682)) {
	switch (((int (*)(struct Cyc_List_List *x))Cyc_List_length)(_temp680)) {
	case 1: 
	  _LL824: if (_temp682 == (void *)Cyc_Absyn_Size) {
		    Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp826 = _temp680;
						 if (_temp826 == 0) {
						   _throw(Null_Exception);
						 }
						 _temp826->hd; }));
		    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp827 = (char *)".size";
					 struct _tagged_string _temp828;
					 _temp828.curr = _temp827;
					 _temp828.base = _temp827;
					 _temp828.last_plus_one = _temp827 + 6;
					 _temp828; }));
		  }
		  else {
		    Cyc_Absyndump_dump(pstr);
		    Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp829 = _temp680;
						 if (_temp829 == 0) {
						   _throw(Null_Exception);
						 }
						 _temp829->hd; }));
		  }
		  break;
	case 2: 
	  _LL825: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp831 = _temp680;
					       if (_temp831 == 0) {
						 _throw(Null_Exception);
					       }
					       _temp831->hd; }));
		  Cyc_Absyndump_dump(pstr);
		  Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp *)({ struct Cyc_List_List *_temp833 = ({ struct Cyc_List_List *_temp832 = _temp680;
						 if (_temp832 == 0) {
						   _throw(Null_Exception);
						 }
						 _temp832->tl; });
					       if (_temp833 == 0) {
						 _throw(Null_Exception);
					       }
					       _temp833->hd; }));
		  break;
	default: 
	  _LL830: (void)_throw((void *)({ struct Cyc_Core_Failure_struct *_temp835 = (struct Cyc_Core_Failure_struct *)GC_malloc(sizeof(struct Cyc_Core_Failure_struct));
				 _temp835[0] = ({ struct Cyc_Core_Failure_struct _temp836;
				   _temp836.tag = Cyc_Core_Failure;
				   _temp836.f1 = (struct _tagged_string)({ char *_temp837 = (char *)"Absyndump -- Bad number of arguments to primop";
				     struct _tagged_string _temp838;
				     _temp838.curr = _temp837;
				     _temp838.base = _temp837;
				     _temp838.last_plus_one = _temp837 + 47;
				     _temp838; });
				   _temp836; });
				 _temp835; }));
	}
      }
      else {
	Cyc_Absyndump_dump(pstr);
	Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp839 = (char *)"(";
				     struct _tagged_string _temp840;
				     _temp840.curr = _temp839;
				     _temp840.base = _temp839;
				     _temp840.last_plus_one = _temp839 + 2;
				     _temp840; }));
	Cyc_Absyndump_dumpexps_prec(20,_temp680);
	Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp841 = (char *)")";
				     struct _tagged_string _temp842;
				     _temp842.curr = _temp841;
				     _temp842.base = _temp841;
				     _temp842.last_plus_one = _temp841 + 2;
				     _temp842; }));
      }
      goto _LL543;
    }
    _LL567: Cyc_Absyndump_dumpexp_prec(myprec,_temp688);
	    if (_temp686 != 0) {
	      Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((void *)({ struct Cyc_Core_Opt *_temp843 = _temp686;
							if (_temp843 == 0) {
							  _throw(Null_Exception);
							}
							_temp843->v; })));
	    }
	    Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp844 = (char *)"=";
					 struct _tagged_string _temp845;
					 _temp845.curr = _temp844;
					 _temp845.base = _temp844;
					 _temp845.last_plus_one = _temp844 + 2;
					 _temp845; }));
	    Cyc_Absyndump_dumpexp_prec(myprec,_temp684);
	    goto _LL543;
    _LL569: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp846 = (char *)"++";
				 struct _tagged_string _temp847;
				 _temp847.curr = _temp846;
				 _temp847.base = _temp846;
				 _temp847.last_plus_one = _temp846 + 3;
				 _temp847; }));
	    Cyc_Absyndump_dumpexp_prec(myprec,_temp692);
	    goto _LL543;
    _LL571: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp848 = (char *)"--";
				 struct _tagged_string _temp849;
				 _temp849.curr = _temp848;
				 _temp849.base = _temp848;
				 _temp849.last_plus_one = _temp848 + 3;
				 _temp849; }));
	    Cyc_Absyndump_dumpexp_prec(myprec,_temp696);
	    goto _LL543;
    _LL573: Cyc_Absyndump_dumpexp_prec(myprec,_temp700);
	    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp850 = (char *)"++";
				 struct _tagged_string _temp851;
				 _temp851.curr = _temp850;
				 _temp851.base = _temp850;
				 _temp851.last_plus_one = _temp850 + 3;
				 _temp851; }));
	    goto _LL543;
    _LL575: Cyc_Absyndump_dumpexp_prec(myprec,_temp704);
	    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp852 = (char *)"--";
				 struct _tagged_string _temp853;
				 _temp853.curr = _temp852;
				 _temp853.base = _temp852;
				 _temp853.last_plus_one = _temp852 + 3;
				 _temp853; }));
	    goto _LL543;
    _LL577: Cyc_Absyndump_dumpexp_prec(myprec,_temp710);
	    Cyc_Absyndump_dump_char((int)'?');
	    Cyc_Absyndump_dumpexp_prec(0,_temp708);
	    Cyc_Absyndump_dump_char((int)':');
	    Cyc_Absyndump_dumpexp_prec(myprec,_temp706);
	    goto _LL543;
    _LL579: Cyc_Absyndump_dumpexp_prec(myprec,_temp714);
	    Cyc_Absyndump_dump_char((int)',');
	    Cyc_Absyndump_dumpexp_prec(myprec,_temp712);
	    goto _LL543;
    _LL581: _temp722 = _temp718;
	    _temp720 = _temp716;
	    goto _LL583;
    _LL583: Cyc_Absyndump_dumpexp_prec(myprec,_temp722);
	    Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp854 = (char *)"(";
					 struct _tagged_string _temp855;
					 _temp855.curr = _temp854;
					 _temp855.base = _temp854;
					 _temp855.last_plus_one = _temp854 + 2;
					 _temp855; }));
	    Cyc_Absyndump_dumpexps_prec(20,_temp720);
	    Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp856 = (char *)")";
					 struct _tagged_string _temp857;
					 _temp857.curr = _temp856;
					 _temp857.base = _temp856;
					 _temp857.last_plus_one = _temp856 + 2;
					 _temp857; }));
	    goto _LL543;
    _LL585: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp858 = (char *)"throw";
				 struct _tagged_string _temp859;
				 _temp859.curr = _temp858;
				 _temp859.base = _temp858;
				 _temp859.last_plus_one = _temp858 + 6;
				 _temp859; }));
	    Cyc_Absyndump_dumpexp_prec(myprec,_temp724);
	    goto _LL543;
    _LL587: _temp730 = _temp726;
	    goto _LL589;
    _LL589: Cyc_Absyndump_dumpexp_prec(inprec,_temp730);
	    goto _LL543;
    _LL591: Cyc_Absyndump_dump_char((int)'(');
	    Cyc_Absyndump_dumptyp(_temp734);
	    Cyc_Absyndump_dump_char((int)')');
	    Cyc_Absyndump_dumpexp_prec(myprec,_temp732);
	    goto _LL543;
    _LL593: Cyc_Absyndump_dump_char((int)'&');
	    Cyc_Absyndump_dumpexp_prec(myprec,_temp736);
	    goto _LL543;
    _LL595: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp860 = (char *)"new ";
				 struct _tagged_string _temp861;
				 _temp861.curr = _temp860;
				 _temp861.base = _temp860;
				 _temp861.last_plus_one = _temp860 + 5;
				 _temp861; }));
	    Cyc_Absyndump_dumpexp_prec(myprec,_temp738);
	    goto _LL543;
    _LL597: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp862 = (char *)"sizeof(";
				 struct _tagged_string _temp863;
				 _temp863.curr = _temp862;
				 _temp863.base = _temp862;
				 _temp863.last_plus_one = _temp862 + 8;
				 _temp863; }));
	    Cyc_Absyndump_dumptyp(_temp742);
	    Cyc_Absyndump_dump_char((int)')');
	    goto _LL543;
    _LL599: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp864 = (char *)"sizeof(";
				 struct _tagged_string _temp865;
				 _temp865.curr = _temp864;
				 _temp865.base = _temp864;
				 _temp865.last_plus_one = _temp864 + 8;
				 _temp865; }));
	    Cyc_Absyndump_dumpexp_prec(myprec,_temp744);
	    Cyc_Absyndump_dump_char((int)')');
	    goto _LL543;
    _LL601: Cyc_Absyndump_dump_char((int)'*');
	    Cyc_Absyndump_dumpexp_prec(myprec,_temp746);
	    goto _LL543;
    _LL603: Cyc_Absyndump_dumpexp_prec(myprec,_temp750);
	    Cyc_Absyndump_dump_char((int)'.');
	    Cyc_Absyndump_dump_nospace(*_temp748);
	    goto _LL543;
    _LL605: Cyc_Absyndump_dumpexp_prec(myprec,_temp754);
	    Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp866 = (char *)"->";
					 struct _tagged_string _temp867;
					 _temp867.curr = _temp866;
					 _temp867.base = _temp866;
					 _temp867.last_plus_one = _temp866 + 3;
					 _temp867; }));
	    Cyc_Absyndump_dump_nospace(*_temp752);
	    goto _LL543;
    _LL607: Cyc_Absyndump_dumpexp_prec(myprec,_temp758);
	    Cyc_Absyndump_dump_char((int)'[');
	    Cyc_Absyndump_dumpexp(_temp756);
	    Cyc_Absyndump_dump_char((int)']');
	    goto _LL543;
    _LL609: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp868 = (char *)"$(";
				 struct _tagged_string _temp869;
				 _temp869.curr = _temp868;
				 _temp869.base = _temp868;
				 _temp869.last_plus_one = _temp868 + 3;
				 _temp869; }));
	    Cyc_Absyndump_dumpexps_prec(20,_temp760);
	    Cyc_Absyndump_dump_char((int)')');
	    goto _LL543;
    _LL611: Cyc_Absyndump_dump_char((int)'(');
	    Cyc_Absyndump_dumptyp((*_temp764).f3);
	    Cyc_Absyndump_dump_char((int)')');
	    ((void (*)(void (*f)(struct _tuple7 *),struct Cyc_List_List *l,
		       struct _tagged_string start,struct _tagged_string end,
		       struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
								       _temp762,
								       (struct _tagged_string)({ char *_temp870 = (char *)"{";
									 struct _tagged_string _temp871;
									 _temp871.curr = _temp870;
									 _temp871.base = _temp870;
									 _temp871.last_plus_one = _temp870 + 2;
									 _temp871; }),
								       (struct _tagged_string)({ char *_temp872 = (char *)"}";
									 struct _tagged_string _temp873;
									 _temp873.curr = _temp872;
									 _temp873.base = _temp872;
									 _temp873.last_plus_one = _temp872 + 2;
									 _temp873; }),
								       (struct _tagged_string)({ char *_temp874 = (char *)",";
									 struct _tagged_string _temp875;
									 _temp875.curr = _temp874;
									 _temp875.base = _temp874;
									 _temp875.last_plus_one = _temp874 + 2;
									 _temp875; }));
	    goto _LL543;
    _LL613: ((void (*)(void (*f)(struct _tuple7 *),struct Cyc_List_List *l,
		       struct _tagged_string start,struct _tagged_string end,
		       struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
								       _temp766,
								       (struct _tagged_string)({ char *_temp876 = (char *)"{";
									 struct _tagged_string _temp877;
									 _temp877.curr = _temp876;
									 _temp877.base = _temp876;
									 _temp877.last_plus_one = _temp876 + 2;
									 _temp877; }),
								       (struct _tagged_string)({ char *_temp878 = (char *)"}";
									 struct _tagged_string _temp879;
									 _temp879.curr = _temp878;
									 _temp879.base = _temp878;
									 _temp879.last_plus_one = _temp878 + 2;
									 _temp879; }),
								       (struct _tagged_string)({ char *_temp880 = (char *)",";
									 struct _tagged_string _temp881;
									 _temp881.curr = _temp880;
									 _temp881.base = _temp880;
									 _temp881.last_plus_one = _temp880 + 2;
									 _temp881; }));
	    goto _LL543;
    _LL615: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp882 = (char *)"new {for";
				 struct _tagged_string _temp883;
				 _temp883.curr = _temp882;
				 _temp883.base = _temp882;
				 _temp883.last_plus_one = _temp882 + 9;
				 _temp883; }));
	    Cyc_Absyndump_dump_str((*_temp772->name).f2);
	    Cyc_Absyndump_dump_char((int)'<');
	    Cyc_Absyndump_dumpexp(_temp770);
	    Cyc_Absyndump_dump_char((int)':');
	    Cyc_Absyndump_dumpexp(_temp768);
	    Cyc_Absyndump_dump_char((int)'}');
	    goto _LL543;
    _LL617: Cyc_Absyndump_dumpqvar(_temp780);
	    ((void (*)(void (*f)(struct _tuple7 *),struct Cyc_List_List *l,
		       struct _tagged_string start,struct _tagged_string end,
		       struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
								       _temp776,
								       (struct _tagged_string)({ char *_temp884 = (char *)"{";
									 struct _tagged_string _temp885;
									 _temp885.curr = _temp884;
									 _temp885.base = _temp884;
									 _temp885.last_plus_one = _temp884 + 2;
									 _temp885; }),
								       (struct _tagged_string)({ char *_temp886 = (char *)"}";
									 struct _tagged_string _temp887;
									 _temp887.curr = _temp886;
									 _temp887.base = _temp886;
									 _temp887.last_plus_one = _temp886 + 2;
									 _temp887; }),
								       (struct _tagged_string)({ char *_temp888 = (char *)",";
									 struct _tagged_string _temp889;
									 _temp889.curr = _temp888;
									 _temp889.base = _temp888;
									 _temp889.last_plus_one = _temp888 + 2;
									 _temp889; }));
	    goto _LL543;
    _LL619: ((void (*)(void (*f)(struct _tuple7 *),struct Cyc_List_List *l,
		       struct _tagged_string start,struct _tagged_string end,
		       struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
								       _temp782,
								       (struct _tagged_string)({ char *_temp890 = (char *)"{";
									 struct _tagged_string _temp891;
									 _temp891.curr = _temp890;
									 _temp891.base = _temp890;
									 _temp891.last_plus_one = _temp890 + 2;
									 _temp891; }),
								       (struct _tagged_string)({ char *_temp892 = (char *)"}";
									 struct _tagged_string _temp893;
									 _temp893.curr = _temp892;
									 _temp893.base = _temp892;
									 _temp893.last_plus_one = _temp892 + 2;
									 _temp893; }),
								       (struct _tagged_string)({ char *_temp894 = (char *)",";
									 struct _tagged_string _temp895;
									 _temp895.curr = _temp894;
									 _temp895.base = _temp894;
									 _temp895.last_plus_one = _temp894 + 2;
									 _temp895; }));
	    goto _LL543;
    _LL621: Cyc_Absyndump_dumpqvar(_temp786->name);
	    if (_temp790 != 0) {
	      ((void (*)(void (*f)(struct Cyc_Absyn_Exp *),struct Cyc_List_List *l,
			 struct _tagged_string start,struct _tagged_string end,
			 struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,
									 _temp790,
									 (struct _tagged_string)({ char *_temp896 = (char *)"(";
									   struct _tagged_string _temp897;
									   _temp897.curr = _temp896;
									   _temp897.base = _temp896;
									   _temp897.last_plus_one = _temp896 + 2;
									   _temp897; }),
									 (struct _tagged_string)({ char *_temp898 = (char *)")";
									   struct _tagged_string _temp899;
									   _temp899.curr = _temp898;
									   _temp899.base = _temp898;
									   _temp899.last_plus_one = _temp898 + 2;
									   _temp899; }),
									 (struct _tagged_string)({ char *_temp900 = (char *)",";
									   struct _tagged_string _temp901;
									   _temp901.curr = _temp900;
									   _temp901.base = _temp900;
									   _temp901.last_plus_one = _temp900 + 2;
									   _temp901; }));
	    }
	    goto _LL543;
    _LL623: Cyc_Absyndump_dumpqvar(_temp800);
	    goto _LL543;
    _LL625: if (_temp804 != 0) {
	      Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp902 = (char *)"rmalloc(";
				   struct _tagged_string _temp903;
				   _temp903.curr = _temp902;
				   _temp903.base = _temp902;
				   _temp903.last_plus_one = _temp902 + 9;
				   _temp903; }));
	      Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp *)({ struct Cyc_Absyn_Exp *_temp904 = _temp804;
				      if (_temp904 == 0) {
					_throw(Null_Exception);
				      }
				      _temp904; }));
	      Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp905 = (char *)",";
				   struct _tagged_string _temp906;
				   _temp906.curr = _temp905;
				   _temp906.base = _temp905;
				   _temp906.last_plus_one = _temp905 + 2;
				   _temp906; }));
	    }
	    else {
	      Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp907 = (char *)"malloc(";
				   struct _tagged_string _temp908;
				   _temp908.curr = _temp907;
				   _temp908.base = _temp907;
				   _temp908.last_plus_one = _temp907 + 8;
				   _temp908; }));
	    }
	    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp909 = (char *)"sizeof(";
				 struct _tagged_string _temp910;
				 _temp910.curr = _temp909;
				 _temp910.base = _temp909;
				 _temp910.last_plus_one = _temp909 + 8;
				 _temp910; }));
	    Cyc_Absyndump_dumptyp(_temp802);
	    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp911 = (char *)"))";
				 struct _tagged_string _temp912;
				 _temp912.curr = _temp911;
				 _temp912.base = _temp911;
				 _temp912.last_plus_one = _temp911 + 3;
				 _temp912; }));
	    goto _LL543;
    _LL627: ((void (*)(void (*f)(struct _tuple7 *),struct Cyc_List_List *l,
		       struct _tagged_string start,struct _tagged_string end,
		       struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
								       _temp806,
								       (struct _tagged_string)({ char *_temp913 = (char *)"{";
									 struct _tagged_string _temp914;
									 _temp914.curr = _temp913;
									 _temp914.base = _temp913;
									 _temp914.last_plus_one = _temp913 + 2;
									 _temp914; }),
								       (struct _tagged_string)({ char *_temp915 = (char *)"}";
									 struct _tagged_string _temp916;
									 _temp916.curr = _temp915;
									 _temp916.base = _temp915;
									 _temp916.last_plus_one = _temp915 + 2;
									 _temp916; }),
								       (struct _tagged_string)({ char *_temp917 = (char *)",";
									 struct _tagged_string _temp918;
									 _temp918.curr = _temp917;
									 _temp918.base = _temp917;
									 _temp918.last_plus_one = _temp917 + 2;
									 _temp918; }));
	    goto _LL543;
    _LL629: Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp919 = (char *)"({";
					 struct _tagged_string _temp920;
					 _temp920.curr = _temp919;
					 _temp920.base = _temp919;
					 _temp920.last_plus_one = _temp919 + 3;
					 _temp920; }));
	    Cyc_Absyndump_dumpstmt(_temp810);
	    Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp921 = (char *)"})";
					 struct _tagged_string _temp922;
					 _temp922.curr = _temp921;
					 _temp922.base = _temp921;
					 _temp922.last_plus_one = _temp921 + 3;
					 _temp922; }));
	    goto _LL543;
    _LL631: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp923 = (char *)"codegen(";
				 struct _tagged_string _temp924;
				 _temp924.curr = _temp923;
				 _temp924.base = _temp923;
				 _temp924.last_plus_one = _temp923 + 9;
				 _temp924; }));
	    Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl *_temp925 = (struct Cyc_Absyn_Decl *)GC_malloc(sizeof(struct Cyc_Absyn_Decl));
				     _temp925->r = (void *)((void *)({ struct Cyc_Absyn_Fn_d_struct *_temp926 = (struct Cyc_Absyn_Fn_d_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Fn_d_struct));
				       _temp926[0] = ({ struct Cyc_Absyn_Fn_d_struct _temp927;
					 _temp927.tag = Cyc_Absyn_Fn_d;
					 _temp927.f1 = _temp812;
					 _temp927; });
				       _temp926; }));
				     _temp925->loc = e->loc;
				     _temp925; }));
	    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp928 = (char *)")";
				 struct _tagged_string _temp929;
				 _temp929.curr = _temp928;
				 _temp929.base = _temp928;
				 _temp929.last_plus_one = _temp928 + 2;
				 _temp929; }));
	    goto _LL543;
    _LL633: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp930 = (char *)"fill(";
				 struct _tagged_string _temp931;
				 _temp931.curr = _temp930;
				 _temp931.base = _temp930;
				 _temp931.last_plus_one = _temp930 + 6;
				 _temp931; }));
	    Cyc_Absyndump_dumpexp(_temp814);
	    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp932 = (char *)")";
				 struct _tagged_string _temp933;
				 _temp933.curr = _temp932;
				 _temp933.base = _temp932;
				 _temp933.last_plus_one = _temp932 + 2;
				 _temp933; }));
	    goto _LL543;
    _LL543: ;
  }
  if (inprec >= myprec) {
    Cyc_Absyndump_dump_char((int)')');
  }
}
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp *e) {
  Cyc_Absyndump_dumpexp_prec(0,e);
}
void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List *scs) {
  for(0; scs != 0; scs = ({ struct Cyc_List_List *_temp934 = scs;
    if (_temp934 == 0) {
      _throw(Null_Exception);
    }
    _temp934->tl; })) {
    struct Cyc_Absyn_Switch_clause *c = (struct Cyc_Absyn_Switch_clause *)({ struct Cyc_List_List *_temp944 = scs;
      if (_temp944 == 0) {
	_throw(Null_Exception);
      }
      _temp944->hd; });
    if (c->where_clause == 0 ? (void *)(c->pattern)->r == (void *)Cyc_Absyn_Wild_p : 0) {
      Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp935 = (char *)"default:";
			   struct _tagged_string _temp936;
			   _temp936.curr = _temp935;
			   _temp936.base = _temp935;
			   _temp936.last_plus_one = _temp935 + 9;
			   _temp936; }));
    }
    else {
      Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp937 = (char *)"case";
			   struct _tagged_string _temp938;
			   _temp938.curr = _temp937;
			   _temp938.base = _temp937;
			   _temp938.last_plus_one = _temp937 + 5;
			   _temp938; }));
      Cyc_Absyndump_dumppat(c->pattern);
      if (c->where_clause != 0) {
	Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp939 = (char *)"&&";
			     struct _tagged_string _temp940;
			     _temp940.curr = _temp939;
			     _temp940.base = _temp939;
			     _temp940.last_plus_one = _temp939 + 3;
			     _temp940; }));
	Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp *)({ struct Cyc_Absyn_Exp *_temp941 = c->where_clause;
				if (_temp941 == 0) {
				  _throw(Null_Exception);
				}
				_temp941; }));
      }
      Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp942 = (char *)":";
				   struct _tagged_string _temp943;
				   _temp943.curr = _temp942;
				   _temp943.base = _temp942;
				   _temp943.last_plus_one = _temp942 + 2;
				   _temp943; }));
    }
    Cyc_Absyndump_dumpstmt(c->body);
  }
}
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt *s) {
  void *_temp945 = (void *)s->r;
  struct Cyc_Absyn_Exp *_temp989;
  struct Cyc_Absyn_Stmt *_temp991;
  struct Cyc_Absyn_Stmt *_temp993;
  struct Cyc_Absyn_Exp *_temp995;
  struct Cyc_Absyn_Exp *_temp997;
  struct Cyc_Absyn_Stmt *_temp999;
  struct Cyc_Absyn_Stmt *_temp1001;
  struct Cyc_Absyn_Exp *_temp1003;
  struct Cyc_Absyn_Stmt *_temp1005;
  struct _tuple2 _temp1007;
  struct Cyc_Absyn_Stmt *_temp1009;
  struct Cyc_Absyn_Exp *_temp1011;
  struct Cyc_Absyn_Stmt *_temp1013;
  struct Cyc_Absyn_Stmt *_temp1015;
  struct Cyc_Absyn_Stmt *_temp1017;
  struct _tagged_string *_temp1019;
  struct Cyc_Absyn_Switch_clause **_temp1021;
  struct Cyc_List_List *_temp1023;
  struct Cyc_Absyn_Switch_clause **_temp1025;
  struct Cyc_List_List *_temp1027;
  struct Cyc_Absyn_Stmt *_temp1029;
  struct _tuple2 _temp1031;
  struct Cyc_Absyn_Stmt *_temp1033;
  struct Cyc_Absyn_Exp *_temp1035;
  struct _tuple2 _temp1037;
  struct Cyc_Absyn_Stmt *_temp1039;
  struct Cyc_Absyn_Exp *_temp1041;
  struct Cyc_Absyn_Exp *_temp1043;
  struct Cyc_List_List *_temp1045;
  struct Cyc_Absyn_Exp *_temp1047;
  struct Cyc_Absyn_Stmt *_temp1049;
  struct Cyc_Absyn_Decl *_temp1051;
  struct Cyc_Absyn_Stmt *_temp1053;
  struct Cyc_Absyn_Stmt *_temp1055;
  struct Cyc_Absyn_Stmt *_temp1057;
  struct _tagged_string *_temp1059;
  struct _tuple2 _temp1061;
  struct Cyc_Absyn_Stmt *_temp1063;
  struct Cyc_Absyn_Exp *_temp1065;
  struct Cyc_Absyn_Stmt *_temp1067;
  struct Cyc_List_List *_temp1069;
  struct Cyc_Absyn_Stmt *_temp1071;
  struct Cyc_Absyn_Stmt *_temp1073;
  struct Cyc_Absyn_Vardecl *_temp1075;
  struct Cyc_Absyn_Tvar *_temp1077;
  _LL947: if (_temp945 == (void *)Cyc_Absyn_Skip_s) {
    goto _LL948;
  }
  else {
    goto _LL949;
  }
  _LL949: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Exp_s : 0) {
    _LL990: _temp989 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Exp_s_struct *)_temp945)->f1;
	    goto _LL950;
  }
  else {
    goto _LL951;
  }
  _LL951: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Seq_s : 0) {
    _LL994: _temp993 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Seq_s_struct *)_temp945)->f1;
	    goto _LL992;
    _LL992: _temp991 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Seq_s_struct *)_temp945)->f2;
	    goto _LL952;
  }
  else {
    goto _LL953;
  }
  _LL953: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Return_s : 0) {
    _LL996: _temp995 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Return_s_struct *)_temp945)->f1;
	    if (_temp995 == 0) {
	      goto _LL954;
	    }
	    else {
	      goto _LL955;
	    }
  }
  else {
    goto _LL955;
  }
  _LL955: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Return_s : 0) {
    _LL998: _temp997 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Return_s_struct *)_temp945)->f1;
	    goto _LL956;
  }
  else {
    goto _LL957;
  }
  _LL957: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_IfThenElse_s : 0) {
    _LL1004: _temp1003 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_IfThenElse_s_struct *)_temp945)->f1;
	     goto _LL1002;
    _LL1002: _temp1001 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_IfThenElse_s_struct *)_temp945)->f2;
	     goto _LL1000;
    _LL1000: _temp999 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_IfThenElse_s_struct *)_temp945)->f3;
	     goto _LL958;
  }
  else {
    goto _LL959;
  }
  _LL959: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_While_s : 0) {
    _LL1008: _temp1007 = (struct _tuple2)((struct Cyc_Absyn_While_s_struct *)_temp945)->f1;
	     _LL1012: _temp1011 = _temp1007.f1;
		      goto _LL1010;
	     _LL1010: _temp1009 = _temp1007.f2;
		      goto _LL1006;
    _LL1006: _temp1005 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_While_s_struct *)_temp945)->f2;
	     goto _LL960;
  }
  else {
    goto _LL961;
  }
  _LL961: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Break_s : 0) {
    _LL1014: _temp1013 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Break_s_struct *)_temp945)->f1;
	     goto _LL962;
  }
  else {
    goto _LL963;
  }
  _LL963: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Continue_s : 0) {
    _LL1016: _temp1015 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Continue_s_struct *)_temp945)->f1;
	     goto _LL964;
  }
  else {
    goto _LL965;
  }
  _LL965: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Goto_s : 0) {
    _LL1020: _temp1019 = (struct _tagged_string *)((struct Cyc_Absyn_Goto_s_struct *)_temp945)->f1;
	     goto _LL1018;
    _LL1018: _temp1017 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Goto_s_struct *)_temp945)->f2;
	     goto _LL966;
  }
  else {
    goto _LL967;
  }
  _LL967: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Fallthru_s : 0) {
    _LL1024: _temp1023 = (struct Cyc_List_List *)((struct Cyc_Absyn_Fallthru_s_struct *)_temp945)->f1;
	     if (_temp1023 == 0) {
	       goto _LL1022;
	     }
	     else {
	       goto _LL969;
	     }
    _LL1022: _temp1021 = (struct Cyc_Absyn_Switch_clause **)((struct Cyc_Absyn_Fallthru_s_struct *)_temp945)->f2;
	     goto _LL968;
  }
  else {
    goto _LL969;
  }
  _LL969: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Fallthru_s : 0) {
    _LL1028: _temp1027 = (struct Cyc_List_List *)((struct Cyc_Absyn_Fallthru_s_struct *)_temp945)->f1;
	     goto _LL1026;
    _LL1026: _temp1025 = (struct Cyc_Absyn_Switch_clause **)((struct Cyc_Absyn_Fallthru_s_struct *)_temp945)->f2;
	     goto _LL970;
  }
  else {
    goto _LL971;
  }
  _LL971: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_For_s : 0) {
    _LL1044: _temp1043 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_For_s_struct *)_temp945)->f1;
	     goto _LL1038;
    _LL1038: _temp1037 = (struct _tuple2)((struct Cyc_Absyn_For_s_struct *)_temp945)->f2;
	     _LL1042: _temp1041 = _temp1037.f1;
		      goto _LL1040;
	     _LL1040: _temp1039 = _temp1037.f2;
		      goto _LL1032;
    _LL1032: _temp1031 = (struct _tuple2)((struct Cyc_Absyn_For_s_struct *)_temp945)->f3;
	     _LL1036: _temp1035 = _temp1031.f1;
		      goto _LL1034;
	     _LL1034: _temp1033 = _temp1031.f2;
		      goto _LL1030;
    _LL1030: _temp1029 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_For_s_struct *)_temp945)->f4;
	     goto _LL972;
  }
  else {
    goto _LL973;
  }
  _LL973: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Switch_s : 0) {
    _LL1048: _temp1047 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Switch_s_struct *)_temp945)->f1;
	     goto _LL1046;
    _LL1046: _temp1045 = (struct Cyc_List_List *)((struct Cyc_Absyn_Switch_s_struct *)_temp945)->f2;
	     goto _LL974;
  }
  else {
    goto _LL975;
  }
  _LL975: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Decl_s : 0) {
    _LL1052: _temp1051 = (struct Cyc_Absyn_Decl *)((struct Cyc_Absyn_Decl_s_struct *)_temp945)->f1;
	     goto _LL1050;
    _LL1050: _temp1049 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Decl_s_struct *)_temp945)->f2;
	     goto _LL976;
  }
  else {
    goto _LL977;
  }
  _LL977: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Cut_s : 0) {
    _LL1054: _temp1053 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Cut_s_struct *)_temp945)->f1;
	     goto _LL978;
  }
  else {
    goto _LL979;
  }
  _LL979: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Splice_s : 0) {
    _LL1056: _temp1055 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Splice_s_struct *)_temp945)->f1;
	     goto _LL980;
  }
  else {
    goto _LL981;
  }
  _LL981: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Label_s : 0) {
    _LL1060: _temp1059 = (struct _tagged_string *)((struct Cyc_Absyn_Label_s_struct *)_temp945)->f1;
	     goto _LL1058;
    _LL1058: _temp1057 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Label_s_struct *)_temp945)->f2;
	     goto _LL982;
  }
  else {
    goto _LL983;
  }
  _LL983: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Do_s : 0) {
    _LL1068: _temp1067 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Do_s_struct *)_temp945)->f1;
	     goto _LL1062;
    _LL1062: _temp1061 = (struct _tuple2)((struct Cyc_Absyn_Do_s_struct *)_temp945)->f2;
	     _LL1066: _temp1065 = _temp1061.f1;
		      goto _LL1064;
	     _LL1064: _temp1063 = _temp1061.f2;
		      goto _LL984;
  }
  else {
    goto _LL985;
  }
  _LL985: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_TryCatch_s : 0) {
    _LL1072: _temp1071 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_TryCatch_s_struct *)_temp945)->f1;
	     goto _LL1070;
    _LL1070: _temp1069 = (struct Cyc_List_List *)((struct Cyc_Absyn_TryCatch_s_struct *)_temp945)->f2;
	     goto _LL986;
  }
  else {
    goto _LL987;
  }
  _LL987: if ((unsigned int)_temp945 > 1 ? *((int *)_temp945) == Cyc_Absyn_Region_s : 0) {
    _LL1078: _temp1077 = (struct Cyc_Absyn_Tvar *)((struct Cyc_Absyn_Region_s_struct *)_temp945)->f1;
	     goto _LL1076;
    _LL1076: _temp1075 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Region_s_struct *)_temp945)->f2;
	     goto _LL1074;
    _LL1074: _temp1073 = (struct Cyc_Absyn_Stmt *)((struct Cyc_Absyn_Region_s_struct *)_temp945)->f3;
	     goto _LL988;
  }
  else {
    goto _LL946;
  }
  _LL948: Cyc_Absyndump_dump_semi();
	  goto _LL946;
  _LL950: Cyc_Absyndump_dumpexp(_temp989);
	  Cyc_Absyndump_dump_semi();
	  goto _LL946;
  _LL952: if (Cyc_Absynpp_is_declaration(_temp993)) {
	    Cyc_Absyndump_dump_char((int)'{');
	    Cyc_Absyndump_dumpstmt(_temp993);
	    Cyc_Absyndump_dump_char((int)'}');
	  }
	  else {
	    Cyc_Absyndump_dumpstmt(_temp993);
	  }
	  if (Cyc_Absynpp_is_declaration(_temp991)) {
	    Cyc_Absyndump_dump_char((int)'{');
	    Cyc_Absyndump_dumpstmt(_temp991);
	    Cyc_Absyndump_dump_char((int)'}');
	  }
	  else {
	    Cyc_Absyndump_dumpstmt(_temp991);
	  }
	  goto _LL946;
  _LL954: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1079 = (char *)"return;";
			       struct _tagged_string _temp1080;
			       _temp1080.curr = _temp1079;
			       _temp1080.base = _temp1079;
			       _temp1080.last_plus_one = _temp1079 + 8;
			       _temp1080; }));
	  goto _LL946;
  _LL956: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1081 = (char *)"return";
			       struct _tagged_string _temp1082;
			       _temp1082.curr = _temp1081;
			       _temp1082.base = _temp1081;
			       _temp1082.last_plus_one = _temp1081 + 7;
			       _temp1082; }));
	  Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp *)({ struct Cyc_Absyn_Exp *_temp1083 = _temp997;
				  if (_temp1083 == 0) {
				    _throw(Null_Exception);
				  }
				  _temp1083; }));
	  Cyc_Absyndump_dump_semi();
	  goto _LL946;
  _LL958: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1084 = (char *)"if(";
			       struct _tagged_string _temp1085;
			       _temp1085.curr = _temp1084;
			       _temp1085.base = _temp1084;
			       _temp1085.last_plus_one = _temp1084 + 4;
			       _temp1085; }));
	  Cyc_Absyndump_dumpexp(_temp1003);
	  Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp1086 = (char *)"){";
				       struct _tagged_string _temp1087;
				       _temp1087.curr = _temp1086;
				       _temp1087.base = _temp1086;
				       _temp1087.last_plus_one = _temp1086 + 3;
				       _temp1087; }));
	  Cyc_Absyndump_dumpstmt(_temp1001);
	  Cyc_Absyndump_dump_char((int)'}');
	  { void *_temp1088 = (void *)_temp999->r;
	    _LL1090: if (_temp1088 == (void *)Cyc_Absyn_Skip_s) {
	      goto _LL1091;
	    }
	    else {
	      goto _LL1092;
	    }
	    _LL1092: goto _LL1093;
	    _LL1091: goto _LL1089;
	    _LL1093: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1094 = (char *)"else{";
					  struct _tagged_string _temp1095;
					  _temp1095.curr = _temp1094;
					  _temp1095.base = _temp1094;
					  _temp1095.last_plus_one = _temp1094 + 6;
					  _temp1095; }));
		     Cyc_Absyndump_dumpstmt(_temp999);
		     Cyc_Absyndump_dump_char((int)'}');
		     goto _LL1089;
	    _LL1089: ;
	  }
	  goto _LL946;
  _LL960: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1096 = (char *)"while(";
			       struct _tagged_string _temp1097;
			       _temp1097.curr = _temp1096;
			       _temp1097.base = _temp1096;
			       _temp1097.last_plus_one = _temp1096 + 7;
			       _temp1097; }));
	  Cyc_Absyndump_dumpexp(_temp1011);
	  Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp1098 = (char *)") {";
				       struct _tagged_string _temp1099;
				       _temp1099.curr = _temp1098;
				       _temp1099.base = _temp1098;
				       _temp1099.last_plus_one = _temp1098 + 4;
				       _temp1099; }));
	  Cyc_Absyndump_dumpstmt(_temp1005);
	  Cyc_Absyndump_dump_char((int)'}');
	  goto _LL946;
  _LL962: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1100 = (char *)"break;";
			       struct _tagged_string _temp1101;
			       _temp1101.curr = _temp1100;
			       _temp1101.base = _temp1100;
			       _temp1101.last_plus_one = _temp1100 + 7;
			       _temp1101; }));
	  goto _LL946;
  _LL964: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1102 = (char *)"continue;";
			       struct _tagged_string _temp1103;
			       _temp1103.curr = _temp1102;
			       _temp1103.base = _temp1102;
			       _temp1103.last_plus_one = _temp1102 + 10;
			       _temp1103; }));
	  goto _LL946;
  _LL966: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1104 = (char *)"goto";
			       struct _tagged_string _temp1105;
			       _temp1105.curr = _temp1104;
			       _temp1105.base = _temp1104;
			       _temp1105.last_plus_one = _temp1104 + 5;
			       _temp1105; }));
	  Cyc_Absyndump_dump_str(_temp1019);
	  Cyc_Absyndump_dump_semi();
	  goto _LL946;
  _LL968: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1106 = (char *)"fallthru;";
			       struct _tagged_string _temp1107;
			       _temp1107.curr = _temp1106;
			       _temp1107.base = _temp1106;
			       _temp1107.last_plus_one = _temp1106 + 10;
			       _temp1107; }));
	  goto _LL946;
  _LL970: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1108 = (char *)"fallthru(";
			       struct _tagged_string _temp1109;
			       _temp1109.curr = _temp1108;
			       _temp1109.base = _temp1108;
			       _temp1109.last_plus_one = _temp1108 + 10;
			       _temp1109; }));
	  Cyc_Absyndump_dumpexps_prec(20,_temp1027);
	  Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp1110 = (char *)");";
				       struct _tagged_string _temp1111;
				       _temp1111.curr = _temp1110;
				       _temp1111.base = _temp1110;
				       _temp1111.last_plus_one = _temp1110 + 3;
				       _temp1111; }));
	  goto _LL946;
  _LL972: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1112 = (char *)"for(";
			       struct _tagged_string _temp1113;
			       _temp1113.curr = _temp1112;
			       _temp1113.base = _temp1112;
			       _temp1113.last_plus_one = _temp1112 + 5;
			       _temp1113; }));
	  Cyc_Absyndump_dumpexp(_temp1043);
	  Cyc_Absyndump_dump_semi();
	  Cyc_Absyndump_dumpexp(_temp1041);
	  Cyc_Absyndump_dump_semi();
	  Cyc_Absyndump_dumpexp(_temp1035);
	  Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp1114 = (char *)"){";
				       struct _tagged_string _temp1115;
				       _temp1115.curr = _temp1114;
				       _temp1115.base = _temp1114;
				       _temp1115.last_plus_one = _temp1114 + 3;
				       _temp1115; }));
	  Cyc_Absyndump_dumpstmt(_temp1029);
	  Cyc_Absyndump_dump_char((int)'}');
	  goto _LL946;
  _LL974: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1116 = (char *)"switch(";
			       struct _tagged_string _temp1117;
			       _temp1117.curr = _temp1116;
			       _temp1117.base = _temp1116;
			       _temp1117.last_plus_one = _temp1116 + 8;
			       _temp1117; }));
	  Cyc_Absyndump_dumpexp(_temp1047);
	  Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp1118 = (char *)"){";
				       struct _tagged_string _temp1119;
				       _temp1119.curr = _temp1118;
				       _temp1119.base = _temp1118;
				       _temp1119.last_plus_one = _temp1118 + 3;
				       _temp1119; }));
	  Cyc_Absyndump_dumpswitchclauses(_temp1045);
	  Cyc_Absyndump_dump_char((int)'}');
	  goto _LL946;
  _LL976: Cyc_Absyndump_dumpdecl(_temp1051);
	  Cyc_Absyndump_dumpstmt(_temp1049);
	  goto _LL946;
  _LL978: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1120 = (char *)"cut";
			       struct _tagged_string _temp1121;
			       _temp1121.curr = _temp1120;
			       _temp1121.base = _temp1120;
			       _temp1121.last_plus_one = _temp1120 + 4;
			       _temp1121; }));
	  Cyc_Absyndump_dumpstmt(_temp1053);
	  goto _LL946;
  _LL980: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1122 = (char *)"splice";
			       struct _tagged_string _temp1123;
			       _temp1123.curr = _temp1122;
			       _temp1123.base = _temp1122;
			       _temp1123.last_plus_one = _temp1122 + 7;
			       _temp1123; }));
	  Cyc_Absyndump_dumpstmt(_temp1055);
	  goto _LL946;
  _LL982: if (Cyc_Absynpp_is_declaration(_temp1057)) {
	    Cyc_Absyndump_dump_str(_temp1059);
	    Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp1124 = (char *)": {";
					 struct _tagged_string _temp1125;
					 _temp1125.curr = _temp1124;
					 _temp1125.base = _temp1124;
					 _temp1125.last_plus_one = _temp1124 + 4;
					 _temp1125; }));
	    Cyc_Absyndump_dumpstmt(_temp1057);
	    Cyc_Absyndump_dump_char((int)'}');
	  }
	  else {
	    Cyc_Absyndump_dump_str(_temp1059);
	    Cyc_Absyndump_dump_char((int)':');
	    Cyc_Absyndump_dumpstmt(_temp1057);
	  }
	  goto _LL946;
  _LL984: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1126 = (char *)"do {";
			       struct _tagged_string _temp1127;
			       _temp1127.curr = _temp1126;
			       _temp1127.base = _temp1126;
			       _temp1127.last_plus_one = _temp1126 + 5;
			       _temp1127; }));
	  Cyc_Absyndump_dumpstmt(_temp1067);
	  Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp1128 = (char *)"} while (";
				       struct _tagged_string _temp1129;
				       _temp1129.curr = _temp1128;
				       _temp1129.base = _temp1128;
				       _temp1129.last_plus_one = _temp1128 + 10;
				       _temp1129; }));
	  Cyc_Absyndump_dumpexp(_temp1065);
	  Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp1130 = (char *)");";
				       struct _tagged_string _temp1131;
				       _temp1131.curr = _temp1130;
				       _temp1131.base = _temp1130;
				       _temp1131.last_plus_one = _temp1130 + 3;
				       _temp1131; }));
	  goto _LL946;
  _LL986: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1132 = (char *)"try";
			       struct _tagged_string _temp1133;
			       _temp1133.curr = _temp1132;
			       _temp1133.base = _temp1132;
			       _temp1133.last_plus_one = _temp1132 + 4;
			       _temp1133; }));
	  Cyc_Absyndump_dumpstmt(_temp1071);
	  Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1134 = (char *)"catch {";
			       struct _tagged_string _temp1135;
			       _temp1135.curr = _temp1134;
			       _temp1135.base = _temp1134;
			       _temp1135.last_plus_one = _temp1134 + 8;
			       _temp1135; }));
	  Cyc_Absyndump_dumpswitchclauses(_temp1069);
	  Cyc_Absyndump_dump_char((int)'}');
	  goto _LL946;
  _LL988: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1136 = (char *)"region<";
			       struct _tagged_string _temp1137;
			       _temp1137.curr = _temp1136;
			       _temp1137.base = _temp1136;
			       _temp1137.last_plus_one = _temp1136 + 8;
			       _temp1137; }));
	  Cyc_Absyndump_dumptvar(_temp1077);
	  Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1138 = (char *)"> ";
			       struct _tagged_string _temp1139;
			       _temp1139.curr = _temp1138;
			       _temp1139.base = _temp1138;
			       _temp1139.last_plus_one = _temp1138 + 3;
			       _temp1139; }));
	  Cyc_Absyndump_dumpqvar(_temp1075->name);
	  Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1140 = (char *)"{";
			       struct _tagged_string _temp1141;
			       _temp1141.curr = _temp1140;
			       _temp1141.base = _temp1140;
			       _temp1141.last_plus_one = _temp1140 + 2;
			       _temp1141; }));
	  Cyc_Absyndump_dumpstmt(_temp1073);
	  Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1142 = (char *)"}";
			       struct _tagged_string _temp1143;
			       _temp1143.curr = _temp1142;
			       _temp1143.base = _temp1142;
			       _temp1143.last_plus_one = _temp1142 + 2;
			       _temp1143; }));
	  goto _LL946;
  _LL946: ;
}
struct _tuple8 {
  struct Cyc_List_List *f1;
  struct Cyc_Absyn_Pat *f2;
};
void Cyc_Absyndump_dumpdp(struct _tuple8 *dp) {
  ((void (*)(void (*f)(void *),struct Cyc_List_List *l,struct _tagged_string start,
	     struct _tagged_string end,struct _tagged_string sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,
											(*dp).f1,
											(struct _tagged_string)({ char *_temp1144 = (char *)"";
											  struct _tagged_string _temp1145;
											  _temp1145.curr = _temp1144;
											  _temp1145.base = _temp1144;
											  _temp1145.last_plus_one = _temp1144 + 1;
											  _temp1145; }),
											(struct _tagged_string)({ char *_temp1146 = (char *)"=";
											  struct _tagged_string _temp1147;
											  _temp1147.curr = _temp1146;
											  _temp1147.base = _temp1146;
											  _temp1147.last_plus_one = _temp1146 + 2;
											  _temp1147; }),
											(struct _tagged_string)({ char *_temp1148 = (char *)"=";
											  struct _tagged_string _temp1149;
											  _temp1149.curr = _temp1148;
											  _temp1149.base = _temp1148;
											  _temp1149.last_plus_one = _temp1148 + 2;
											  _temp1149; }));
  Cyc_Absyndump_dumppat((*dp).f2);
}
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat *p) {
  void *_temp1150 = (void *)p->r;
  int _temp1184;
  void *_temp1186;
  int _temp1188;
  void *_temp1190;
  char _temp1192;
  struct _tagged_string _temp1194;
  struct Cyc_Absyn_Vardecl *_temp1196;
  struct Cyc_List_List *_temp1198;
  struct Cyc_Absyn_Pat *_temp1200;
  struct Cyc_Absyn_Vardecl *_temp1202;
  struct _tuple0 *_temp1204;
  struct Cyc_List_List *_temp1206;
  struct Cyc_List_List *_temp1208;
  struct _tuple0 *_temp1210;
  struct Cyc_List_List *_temp1212;
  struct Cyc_List_List *_temp1214;
  struct _tuple0 *_temp1216;
  struct Cyc_List_List *_temp1218;
  struct Cyc_List_List *_temp1220;
  struct Cyc_Core_Opt *_temp1222;
  struct Cyc_Absyn_Structdecl *_temp1224;
  struct Cyc_List_List *_temp1226;
  struct Cyc_List_List *_temp1228;
  struct Cyc_Absyn_Tunionfield *_temp1230;
  struct Cyc_Absyn_Tuniondecl *_temp1232;
  struct Cyc_Absyn_Enumfield *_temp1234;
  struct Cyc_Absyn_Enumdecl *_temp1236;
  _LL1152: if (_temp1150 == (void *)Cyc_Absyn_Wild_p) {
    goto _LL1153;
  }
  else {
    goto _LL1154;
  }
  _LL1154: if (_temp1150 == (void *)Cyc_Absyn_Null_p) {
    goto _LL1155;
  }
  else {
    goto _LL1156;
  }
  _LL1156: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_Int_p : 0) {
    _LL1187: _temp1186 = (void *)((struct Cyc_Absyn_Int_p_struct *)_temp1150)->f1;
	     if (_temp1186 == (void *)Cyc_Absyn_Signed) {
	       goto _LL1185;
	     }
	     else {
	       goto _LL1158;
	     }
    _LL1185: _temp1184 = (int)((struct Cyc_Absyn_Int_p_struct *)_temp1150)->f2;
	     goto _LL1157;
  }
  else {
    goto _LL1158;
  }
  _LL1158: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_Int_p : 0) {
    _LL1191: _temp1190 = (void *)((struct Cyc_Absyn_Int_p_struct *)_temp1150)->f1;
	     if (_temp1190 == (void *)Cyc_Absyn_Unsigned) {
	       goto _LL1189;
	     }
	     else {
	       goto _LL1160;
	     }
    _LL1189: _temp1188 = (int)((struct Cyc_Absyn_Int_p_struct *)_temp1150)->f2;
	     goto _LL1159;
  }
  else {
    goto _LL1160;
  }
  _LL1160: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_Char_p : 0) {
    _LL1193: _temp1192 = (char)((struct Cyc_Absyn_Char_p_struct *)_temp1150)->f1;
	     goto _LL1161;
  }
  else {
    goto _LL1162;
  }
  _LL1162: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_Float_p : 0) {
    _LL1195: _temp1194 = (struct _tagged_string)((struct Cyc_Absyn_Float_p_struct *)_temp1150)->f1;
	     goto _LL1163;
  }
  else {
    goto _LL1164;
  }
  _LL1164: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_Var_p : 0) {
    _LL1197: _temp1196 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Var_p_struct *)_temp1150)->f1;
	     goto _LL1165;
  }
  else {
    goto _LL1166;
  }
  _LL1166: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_Tuple_p : 0) {
    _LL1199: _temp1198 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tuple_p_struct *)_temp1150)->f1;
	     goto _LL1167;
  }
  else {
    goto _LL1168;
  }
  _LL1168: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_Pointer_p : 0) {
    _LL1201: _temp1200 = (struct Cyc_Absyn_Pat *)((struct Cyc_Absyn_Pointer_p_struct *)_temp1150)->f1;
	     goto _LL1169;
  }
  else {
    goto _LL1170;
  }
  _LL1170: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_Reference_p : 0) {
    _LL1203: _temp1202 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Reference_p_struct *)_temp1150)->f1;
	     goto _LL1171;
  }
  else {
    goto _LL1172;
  }
  _LL1172: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_UnknownId_p : 0) {
    _LL1205: _temp1204 = (struct _tuple0 *)((struct Cyc_Absyn_UnknownId_p_struct *)_temp1150)->f1;
	     goto _LL1173;
  }
  else {
    goto _LL1174;
  }
  _LL1174: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_UnknownCall_p : 0) {
    _LL1211: _temp1210 = (struct _tuple0 *)((struct Cyc_Absyn_UnknownCall_p_struct *)_temp1150)->f1;
	     goto _LL1209;
    _LL1209: _temp1208 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownCall_p_struct *)_temp1150)->f2;
	     goto _LL1207;
    _LL1207: _temp1206 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownCall_p_struct *)_temp1150)->f3;
	     goto _LL1175;
  }
  else {
    goto _LL1176;
  }
  _LL1176: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_UnknownFields_p : 0) {
    _LL1217: _temp1216 = (struct _tuple0 *)((struct Cyc_Absyn_UnknownFields_p_struct *)_temp1150)->f1;
	     goto _LL1215;
    _LL1215: _temp1214 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownFields_p_struct *)_temp1150)->f2;
	     goto _LL1213;
    _LL1213: _temp1212 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownFields_p_struct *)_temp1150)->f3;
	     goto _LL1177;
  }
  else {
    goto _LL1178;
  }
  _LL1178: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_Struct_p : 0) {
    _LL1225: _temp1224 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Absyn_Struct_p_struct *)_temp1150)->f1;
	     goto _LL1223;
    _LL1223: _temp1222 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Struct_p_struct *)_temp1150)->f2;
	     goto _LL1221;
    _LL1221: _temp1220 = (struct Cyc_List_List *)((struct Cyc_Absyn_Struct_p_struct *)_temp1150)->f3;
	     goto _LL1219;
    _LL1219: _temp1218 = (struct Cyc_List_List *)((struct Cyc_Absyn_Struct_p_struct *)_temp1150)->f4;
	     goto _LL1179;
  }
  else {
    goto _LL1180;
  }
  _LL1180: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_Tunion_p : 0) {
    _LL1233: _temp1232 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_Tunion_p_struct *)_temp1150)->f1;
	     goto _LL1231;
    _LL1231: _temp1230 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Absyn_Tunion_p_struct *)_temp1150)->f2;
	     goto _LL1229;
    _LL1229: _temp1228 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_p_struct *)_temp1150)->f3;
	     goto _LL1227;
    _LL1227: _temp1226 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_p_struct *)_temp1150)->f4;
	     goto _LL1181;
  }
  else {
    goto _LL1182;
  }
  _LL1182: if ((unsigned int)_temp1150 > 2 ? *((int *)_temp1150) == Cyc_Absyn_Enum_p : 0) {
    _LL1237: _temp1236 = (struct Cyc_Absyn_Enumdecl *)((struct Cyc_Absyn_Enum_p_struct *)_temp1150)->f1;
	     goto _LL1235;
    _LL1235: _temp1234 = (struct Cyc_Absyn_Enumfield *)((struct Cyc_Absyn_Enum_p_struct *)_temp1150)->f2;
	     goto _LL1183;
  }
  else {
    goto _LL1151;
  }
  _LL1153: Cyc_Absyndump_dump_char((int)'_');
	   goto _LL1151;
  _LL1155: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1238 = (char *)"null";
				struct _tagged_string _temp1239;
				_temp1239.curr = _temp1238;
				_temp1239.base = _temp1238;
				_temp1239.last_plus_one = _temp1238 + 5;
				_temp1239; }));
	   goto _LL1151;
  _LL1157: Cyc_Absyndump_dump(xprintf("%d",_temp1184));
	   goto _LL1151;
  _LL1159: Cyc_Absyndump_dump(xprintf("%u",(unsigned int)_temp1188));
	   goto _LL1151;
  _LL1161: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1240 = (char *)"'";
				struct _tagged_string _temp1241;
				_temp1241.curr = _temp1240;
				_temp1241.base = _temp1240;
				_temp1241.last_plus_one = _temp1240 + 2;
				_temp1241; }));
	   Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_temp1192));
	   Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp1242 = (char *)"'";
					struct _tagged_string _temp1243;
					_temp1243.curr = _temp1242;
					_temp1243.base = _temp1242;
					_temp1243.last_plus_one = _temp1242 + 2;
					_temp1243; }));
	   goto _LL1151;
  _LL1163: Cyc_Absyndump_dump(_temp1194);
	   goto _LL1151;
  _LL1165: Cyc_Absyndump_dumpqvar(_temp1196->name);
	   goto _LL1151;
  _LL1167: ((void (*)(void (*f)(struct Cyc_Absyn_Pat *),struct Cyc_List_List *l,
		      struct _tagged_string start,struct _tagged_string end,
		      struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
								      _temp1198,
								      (struct _tagged_string)({ char *_temp1244 = (char *)"$(";
									struct _tagged_string _temp1245;
									_temp1245.curr = _temp1244;
									_temp1245.base = _temp1244;
									_temp1245.last_plus_one = _temp1244 + 3;
									_temp1245; }),
								      (struct _tagged_string)({ char *_temp1246 = (char *)")";
									struct _tagged_string _temp1247;
									_temp1247.curr = _temp1246;
									_temp1247.base = _temp1246;
									_temp1247.last_plus_one = _temp1246 + 2;
									_temp1247; }),
								      (struct _tagged_string)({ char *_temp1248 = (char *)",";
									struct _tagged_string _temp1249;
									_temp1249.curr = _temp1248;
									_temp1249.base = _temp1248;
									_temp1249.last_plus_one = _temp1248 + 2;
									_temp1249; }));
	   goto _LL1151;
  _LL1169: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1250 = (char *)"&";
				struct _tagged_string _temp1251;
				_temp1251.curr = _temp1250;
				_temp1251.base = _temp1250;
				_temp1251.last_plus_one = _temp1250 + 2;
				_temp1251; }));
	   Cyc_Absyndump_dumppat(_temp1200);
	   goto _LL1151;
  _LL1171: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1252 = (char *)"*";
				struct _tagged_string _temp1253;
				_temp1253.curr = _temp1252;
				_temp1253.base = _temp1252;
				_temp1253.last_plus_one = _temp1252 + 2;
				_temp1253; }));
	   Cyc_Absyndump_dumpqvar(_temp1202->name);
	   goto _LL1151;
  _LL1173: Cyc_Absyndump_dumpqvar(_temp1204);
	   goto _LL1151;
  _LL1175: Cyc_Absyndump_dumpqvar(_temp1210);
	   Cyc_Absyndump_dumptvars(_temp1208);
	   ((void (*)(void (*f)(struct Cyc_Absyn_Pat *),struct Cyc_List_List *l,
		      struct _tagged_string start,struct _tagged_string end,
		      struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
								      _temp1206,
								      (struct _tagged_string)({ char *_temp1254 = (char *)"(";
									struct _tagged_string _temp1255;
									_temp1255.curr = _temp1254;
									_temp1255.base = _temp1254;
									_temp1255.last_plus_one = _temp1254 + 2;
									_temp1255; }),
								      (struct _tagged_string)({ char *_temp1256 = (char *)")";
									struct _tagged_string _temp1257;
									_temp1257.curr = _temp1256;
									_temp1257.base = _temp1256;
									_temp1257.last_plus_one = _temp1256 + 2;
									_temp1257; }),
								      (struct _tagged_string)({ char *_temp1258 = (char *)",";
									struct _tagged_string _temp1259;
									_temp1259.curr = _temp1258;
									_temp1259.base = _temp1258;
									_temp1259.last_plus_one = _temp1258 + 2;
									_temp1259; }));
	   goto _LL1151;
  _LL1177: Cyc_Absyndump_dumpqvar(_temp1216);
	   Cyc_Absyndump_dumptvars(_temp1214);
	   ((void (*)(void (*f)(struct _tuple8 *),struct Cyc_List_List *l,
		      struct _tagged_string start,struct _tagged_string end,
		      struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
								      _temp1212,
								      (struct _tagged_string)({ char *_temp1260 = (char *)"{";
									struct _tagged_string _temp1261;
									_temp1261.curr = _temp1260;
									_temp1261.base = _temp1260;
									_temp1261.last_plus_one = _temp1260 + 2;
									_temp1261; }),
								      (struct _tagged_string)({ char *_temp1262 = (char *)"}";
									struct _tagged_string _temp1263;
									_temp1263.curr = _temp1262;
									_temp1263.base = _temp1262;
									_temp1263.last_plus_one = _temp1262 + 2;
									_temp1263; }),
								      (struct _tagged_string)({ char *_temp1264 = (char *)",";
									struct _tagged_string _temp1265;
									_temp1265.curr = _temp1264;
									_temp1265.base = _temp1264;
									_temp1265.last_plus_one = _temp1264 + 2;
									_temp1265; }));
	   goto _LL1151;
  _LL1179: if (_temp1224->name != 0) {
	     Cyc_Absyndump_dumpqvar((struct _tuple0 *)({ struct Cyc_Core_Opt *_temp1266 = _temp1224->name;
				      if (_temp1266 == 0) {
					_throw(Null_Exception);
				      }
				      _temp1266->v; }));
	   }
	   Cyc_Absyndump_dumptvars(_temp1220);
	   ((void (*)(void (*f)(struct _tuple8 *),struct Cyc_List_List *l,
		      struct _tagged_string start,struct _tagged_string end,
		      struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
								      _temp1218,
								      (struct _tagged_string)({ char *_temp1267 = (char *)"{";
									struct _tagged_string _temp1268;
									_temp1268.curr = _temp1267;
									_temp1268.base = _temp1267;
									_temp1268.last_plus_one = _temp1267 + 2;
									_temp1268; }),
								      (struct _tagged_string)({ char *_temp1269 = (char *)"}";
									struct _tagged_string _temp1270;
									_temp1270.curr = _temp1269;
									_temp1270.base = _temp1269;
									_temp1270.last_plus_one = _temp1269 + 2;
									_temp1270; }),
								      (struct _tagged_string)({ char *_temp1271 = (char *)",";
									struct _tagged_string _temp1272;
									_temp1272.curr = _temp1271;
									_temp1272.base = _temp1271;
									_temp1272.last_plus_one = _temp1271 + 2;
									_temp1272; }));
	   goto _LL1151;
  _LL1181: Cyc_Absyndump_dumpqvar(_temp1230->name);
	   Cyc_Absyndump_dumptvars(_temp1228);
	   if (_temp1226 != 0) {
	     ((void (*)(void (*f)(struct Cyc_Absyn_Pat *),struct Cyc_List_List *l,
			struct _tagged_string start,struct _tagged_string end,
			struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
									_temp1226,
									(struct _tagged_string)({ char *_temp1273 = (char *)"(";
									  struct _tagged_string _temp1274;
									  _temp1274.curr = _temp1273;
									  _temp1274.base = _temp1273;
									  _temp1274.last_plus_one = _temp1273 + 2;
									  _temp1274; }),
									(struct _tagged_string)({ char *_temp1275 = (char *)")";
									  struct _tagged_string _temp1276;
									  _temp1276.curr = _temp1275;
									  _temp1276.base = _temp1275;
									  _temp1276.last_plus_one = _temp1275 + 2;
									  _temp1276; }),
									(struct _tagged_string)({ char *_temp1277 = (char *)",";
									  struct _tagged_string _temp1278;
									  _temp1278.curr = _temp1277;
									  _temp1278.base = _temp1277;
									  _temp1278.last_plus_one = _temp1277 + 2;
									  _temp1278; }));
	   }
	   goto _LL1151;
  _LL1183: Cyc_Absyndump_dumpqvar(_temp1234->name);
	   goto _LL1151;
  _LL1151: ;
}
void Cyc_Absyndump_dumptunionfield(struct Cyc_Absyn_Tunionfield *ef) {
  Cyc_Absyndump_dumpqvar(ef->name);
  if (ef->typs != 0) {
    Cyc_Absyndump_dumpargs(ef->typs);
  }
}
void Cyc_Absyndump_dumptunionfields(struct Cyc_List_List *fields) {
  ((void (*)(void (*f)(struct Cyc_Absyn_Tunionfield *),struct Cyc_List_List *l,
	     struct _tagged_string sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumptunionfield,
								fields,(struct _tagged_string)({ char *_temp1279 = (char *)",";
								  struct _tagged_string _temp1280;
								  _temp1280.curr = _temp1279;
								  _temp1280.base = _temp1279;
								  _temp1280.last_plus_one = _temp1279 + 2;
								  _temp1280; }));
}
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield *ef) {
  Cyc_Absyndump_dumpqvar(ef->name);
  if (ef->tag != 0) {
    Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1281 = (char *)" = ";
			 struct _tagged_string _temp1282;
			 _temp1282.curr = _temp1281;
			 _temp1282.base = _temp1281;
			 _temp1282.last_plus_one = _temp1281 + 4;
			 _temp1282; }));
    Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp *)({ struct Cyc_Absyn_Exp *_temp1283 = ef->tag;
			    if (_temp1283 == 0) {
			      _throw(Null_Exception);
			    }
			    _temp1283; }));
  }
}
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List *fields) {
  ((void (*)(void (*f)(struct Cyc_Absyn_Enumfield *),struct Cyc_List_List *l,
	     struct _tagged_string sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
								fields,(struct _tagged_string)({ char *_temp1284 = (char *)",";
								  struct _tagged_string _temp1285;
								  _temp1285.curr = _temp1284;
								  _temp1285.base = _temp1284;
								  _temp1285.last_plus_one = _temp1284 + 2;
								  _temp1285; }));
}
void Cyc_Absyndump_dumpstructfields(struct Cyc_List_List *fields) {
  for(0; fields != 0; fields = ({ struct Cyc_List_List *_temp1286 = fields;
    if (_temp1286 == 0) {
      _throw(Null_Exception);
    }
    _temp1286->tl; })) {
    struct Cyc_Absyn_Structfield _temp1290;
    struct Cyc_List_List *_temp1291;
    struct Cyc_Core_Opt *_temp1293;
    void *_temp1295;
    struct Cyc_Absyn_Tqual _temp1297;
    struct _tagged_string *_temp1299;
    struct Cyc_Absyn_Structfield *_temp1288 = (struct Cyc_Absyn_Structfield *)({ struct Cyc_List_List *_temp1287 = fields;
      if (_temp1287 == 0) {
	_throw(Null_Exception);
      }
      _temp1287->hd; });
    _temp1290 = *_temp1288;
    _LL1300: _temp1299 = (struct _tagged_string *)_temp1290.name;
	     goto _LL1298;
    _LL1298: _temp1297 = (struct Cyc_Absyn_Tqual)_temp1290.tq;
	     goto _LL1296;
    _LL1296: _temp1295 = (void *)_temp1290.type;
	     goto _LL1294;
    _LL1294: _temp1293 = (struct Cyc_Core_Opt *)_temp1290.width;
	     goto _LL1292;
    _LL1292: _temp1291 = (struct Cyc_List_List *)_temp1290.attributes;
	     goto _LL1289;
    _LL1289: ((void (*)(struct Cyc_Absyn_Tqual,void *,void (*f)(struct _tagged_string *),
			struct _tagged_string *))Cyc_Absyndump_dumptqtd)(_temp1297,
									 _temp1295,
									 Cyc_Absyndump_dump_str,
									 _temp1299);
	     Cyc_Absyndump_dumpatts(_temp1291);
	     if (_temp1293 != 0) {
	       Cyc_Absyndump_dump_char((int)':');
	       Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp *)({ struct Cyc_Core_Opt *_temp1301 = _temp1293;
				       if (_temp1301 == 0) {
					 _throw(Null_Exception);
				       }
				       _temp1301->v; }));
	     }
	     Cyc_Absyndump_dump_semi();
  }
}
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl *td) {
  Cyc_Absyndump_dumpqvar(td->name);
  Cyc_Absyndump_dumptvars(td->tvs);
}
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl *fd) {
  Cyc_Absyndump_dumpatts(fd->attributes);
  Cyc_Absyndump_dumpqvar(fd->name);
}
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl *d) {
  void *_temp1302 = (void *)d->r;
  struct Cyc_Absyn_Fndecl *_temp1326;
  struct Cyc_Absyn_Structdecl *_temp1328;
  struct Cyc_Absyn_Uniondecl *_temp1330;
  struct Cyc_Absyn_Vardecl *_temp1332;
  struct Cyc_Absyn_Vardecl _temp1334;
  struct Cyc_List_List *_temp1335;
  struct Cyc_Core_Opt *_temp1337;
  struct Cyc_Absyn_Exp *_temp1339;
  void *_temp1341;
  struct Cyc_Absyn_Tqual _temp1343;
  struct _tuple0 *_temp1345;
  void *_temp1347;
  struct Cyc_Absyn_Tuniondecl *_temp1349;
  struct Cyc_Absyn_Tuniondecl _temp1351;
  int _temp1352;
  struct Cyc_Core_Opt *_temp1354;
  struct Cyc_List_List *_temp1356;
  struct _tuple0 *_temp1358;
  void *_temp1360;
  struct Cyc_Absyn_Enumdecl *_temp1362;
  struct Cyc_Absyn_Enumdecl _temp1364;
  struct Cyc_List_List *_temp1365;
  struct _tuple0 *_temp1367;
  void *_temp1369;
  int _temp1371;
  struct Cyc_Absyn_Exp *_temp1373;
  struct Cyc_Core_Opt *_temp1375;
  struct Cyc_Core_Opt *_temp1377;
  struct Cyc_Absyn_Pat *_temp1379;
  struct Cyc_Absyn_Typedefdecl *_temp1381;
  struct Cyc_List_List *_temp1383;
  struct _tagged_string *_temp1385;
  struct Cyc_List_List *_temp1387;
  struct _tuple0 *_temp1389;
  struct Cyc_List_List *_temp1391;
  _LL1304: if (*((int *)_temp1302) == Cyc_Absyn_Fn_d) {
    _LL1327: _temp1326 = (struct Cyc_Absyn_Fndecl *)((struct Cyc_Absyn_Fn_d_struct *)_temp1302)->f1;
	     goto _LL1305;
  }
  else {
    goto _LL1306;
  }
  _LL1306: if (*((int *)_temp1302) == Cyc_Absyn_Struct_d) {
    _LL1329: _temp1328 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Absyn_Struct_d_struct *)_temp1302)->f1;
	     goto _LL1307;
  }
  else {
    goto _LL1308;
  }
  _LL1308: if (*((int *)_temp1302) == Cyc_Absyn_Union_d) {
    _LL1331: _temp1330 = (struct Cyc_Absyn_Uniondecl *)((struct Cyc_Absyn_Union_d_struct *)_temp1302)->f1;
	     goto _LL1309;
  }
  else {
    goto _LL1310;
  }
  _LL1310: if (*((int *)_temp1302) == Cyc_Absyn_Var_d) {
    _LL1333: _temp1332 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Var_d_struct *)_temp1302)->f1;
	     _temp1334 = *_temp1332;
	     _LL1348: _temp1347 = (void *)_temp1334.sc;
		      goto _LL1346;
	     _LL1346: _temp1345 = (struct _tuple0 *)_temp1334.name;
		      goto _LL1344;
	     _LL1344: _temp1343 = (struct Cyc_Absyn_Tqual)_temp1334.tq;
		      goto _LL1342;
	     _LL1342: _temp1341 = (void *)_temp1334.type;
		      goto _LL1340;
	     _LL1340: _temp1339 = (struct Cyc_Absyn_Exp *)_temp1334.initializer;
		      goto _LL1338;
	     _LL1338: _temp1337 = (struct Cyc_Core_Opt *)_temp1334.rgn;
		      goto _LL1336;
	     _LL1336: _temp1335 = (struct Cyc_List_List *)_temp1334.attributes;
		      goto _LL1311;
  }
  else {
    goto _LL1312;
  }
  _LL1312: if (*((int *)_temp1302) == Cyc_Absyn_Tunion_d) {
    _LL1350: _temp1349 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_Tunion_d_struct *)_temp1302)->f1;
	     _temp1351 = *_temp1349;
	     _LL1361: _temp1360 = (void *)_temp1351.sc;
		      goto _LL1359;
	     _LL1359: _temp1358 = (struct _tuple0 *)_temp1351.name;
		      goto _LL1357;
	     _LL1357: _temp1356 = (struct Cyc_List_List *)_temp1351.tvs;
		      goto _LL1355;
	     _LL1355: _temp1354 = (struct Cyc_Core_Opt *)_temp1351.fields;
		      goto _LL1353;
	     _LL1353: _temp1352 = (int)_temp1351.is_xtunion;
		      goto _LL1313;
  }
  else {
    goto _LL1314;
  }
  _LL1314: if (*((int *)_temp1302) == Cyc_Absyn_Enum_d) {
    _LL1363: _temp1362 = (struct Cyc_Absyn_Enumdecl *)((struct Cyc_Absyn_Enum_d_struct *)_temp1302)->f1;
	     _temp1364 = *_temp1362;
	     _LL1370: _temp1369 = (void *)_temp1364.sc;
		      goto _LL1368;
	     _LL1368: _temp1367 = (struct _tuple0 *)_temp1364.name;
		      goto _LL1366;
	     _LL1366: _temp1365 = (struct Cyc_List_List *)_temp1364.fields;
		      goto _LL1315;
  }
  else {
    goto _LL1316;
  }
  _LL1316: if (*((int *)_temp1302) == Cyc_Absyn_Let_d) {
    _LL1380: _temp1379 = (struct Cyc_Absyn_Pat *)((struct Cyc_Absyn_Let_d_struct *)_temp1302)->f1;
	     goto _LL1378;
    _LL1378: _temp1377 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Let_d_struct *)_temp1302)->f2;
	     goto _LL1376;
    _LL1376: _temp1375 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Let_d_struct *)_temp1302)->f3;
	     goto _LL1374;
    _LL1374: _temp1373 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Let_d_struct *)_temp1302)->f4;
	     goto _LL1372;
    _LL1372: _temp1371 = (int)((struct Cyc_Absyn_Let_d_struct *)_temp1302)->f5;
	     goto _LL1317;
  }
  else {
    goto _LL1318;
  }
  _LL1318: if (*((int *)_temp1302) == Cyc_Absyn_Typedef_d) {
    _LL1382: _temp1381 = (struct Cyc_Absyn_Typedefdecl *)((struct Cyc_Absyn_Typedef_d_struct *)_temp1302)->f1;
	     goto _LL1319;
  }
  else {
    goto _LL1320;
  }
  _LL1320: if (*((int *)_temp1302) == Cyc_Absyn_Namespace_d) {
    _LL1386: _temp1385 = (struct _tagged_string *)((struct Cyc_Absyn_Namespace_d_struct *)_temp1302)->f1;
	     goto _LL1384;
    _LL1384: _temp1383 = (struct Cyc_List_List *)((struct Cyc_Absyn_Namespace_d_struct *)_temp1302)->f2;
	     goto _LL1321;
  }
  else {
    goto _LL1322;
  }
  _LL1322: if (*((int *)_temp1302) == Cyc_Absyn_Using_d) {
    _LL1390: _temp1389 = (struct _tuple0 *)((struct Cyc_Absyn_Using_d_struct *)_temp1302)->f1;
	     goto _LL1388;
    _LL1388: _temp1387 = (struct Cyc_List_List *)((struct Cyc_Absyn_Using_d_struct *)_temp1302)->f2;
	     goto _LL1323;
  }
  else {
    goto _LL1324;
  }
  _LL1324: if (*((int *)_temp1302) == Cyc_Absyn_ExternC_d) {
    _LL1392: _temp1391 = (struct Cyc_List_List *)((struct Cyc_Absyn_ExternC_d_struct *)_temp1302)->f1;
	     goto _LL1325;
  }
  else {
    goto _LL1303;
  }
  _LL1305: if (_temp1326->is_inline) {
	     Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1393 = (char *)"inline";
				  struct _tagged_string _temp1394;
				  _temp1394.curr = _temp1393;
				  _temp1394.base = _temp1393;
				  _temp1394.last_plus_one = _temp1393 + 7;
				  _temp1394; }));
	   }
	   Cyc_Absyndump_dumpscope((void *)_temp1326->sc);
	   { void *t = (void *)({ struct Cyc_Absyn_FnType_struct *_temp1396 = (struct Cyc_Absyn_FnType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_FnType_struct));
	       _temp1396[0] = ({ struct Cyc_Absyn_FnType_struct _temp1397;
		 _temp1397.tag = Cyc_Absyn_FnType;
		 _temp1397.f1 = ({ struct Cyc_Absyn_FnInfo _temp1398;
		   _temp1398.tvars = _temp1326->tvs;
		   _temp1398.effect = _temp1326->effect;
		   _temp1398.ret_typ = (void *)((void *)_temp1326->ret_type);
		   _temp1398.args = ((struct Cyc_List_List *(*)(struct _tuple1 *(*f)(struct _tuple3 *),
								struct Cyc_List_List *x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
												       _temp1326->args);
		   _temp1398.varargs = _temp1326->varargs;
		   _temp1398.attributes = 0;
		   _temp1398; });
		 _temp1397; });
	       _temp1396; });
	     ((void (*)(struct Cyc_Absyn_Tqual,void *,void (*f)(struct Cyc_Absyn_Fndecl *),
			struct Cyc_Absyn_Fndecl *))Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual _temp1395;
									     _temp1395.q_const = 0;
									     _temp1395.q_volatile = 0;
									     _temp1395.q_restrict = 0;
									     _temp1395; }),
									   t,
									   Cyc_Absyndump_dump_atts_qvar,
									   _temp1326);
	     Cyc_Absyndump_dump_char((int)'{');
	     Cyc_Absyndump_dumpstmt(_temp1326->body);
	     Cyc_Absyndump_dump_char((int)'}');
	     goto _LL1303;}
	   
  _LL1307: Cyc_Absyndump_dumpscope((void *)_temp1328->sc);
	   Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1399 = (char *)"struct";
				struct _tagged_string _temp1400;
				_temp1400.curr = _temp1399;
				_temp1400.base = _temp1399;
				_temp1400.last_plus_one = _temp1399 + 7;
				_temp1400; }));
	   if (_temp1328->name != 0) {
	     Cyc_Absyndump_dumpqvar((struct _tuple0 *)({ struct Cyc_Core_Opt *_temp1401 = _temp1328->name;
				      if (_temp1401 == 0) {
					_throw(Null_Exception);
				      }
				      _temp1401->v; }));
	   }
	   Cyc_Absyndump_dumptvars(_temp1328->tvs);
	   if (_temp1328->fields == 0) {
	     Cyc_Absyndump_dump_semi();
	   }
	   else {
	     Cyc_Absyndump_dump_char((int)'{');
	     Cyc_Absyndump_dumpstructfields((struct Cyc_List_List *)({ struct Cyc_Core_Opt *_temp1402 = _temp1328->fields;
					      if (_temp1402 == 0) {
						_throw(Null_Exception);
					      }
					      _temp1402->v; }));
	     Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1403 = (char *)"}";
				  struct _tagged_string _temp1404;
				  _temp1404.curr = _temp1403;
				  _temp1404.base = _temp1403;
				  _temp1404.last_plus_one = _temp1403 + 2;
				  _temp1404; }));
	     Cyc_Absyndump_dumpatts(_temp1328->attributes);
	     Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1405 = (char *)";";
				  struct _tagged_string _temp1406;
				  _temp1406.curr = _temp1405;
				  _temp1406.base = _temp1405;
				  _temp1406.last_plus_one = _temp1405 + 2;
				  _temp1406; }));
	   }
	   goto _LL1303;
  _LL1309: Cyc_Absyndump_dumpscope((void *)_temp1330->sc);
	   Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1407 = (char *)"union";
				struct _tagged_string _temp1408;
				_temp1408.curr = _temp1407;
				_temp1408.base = _temp1407;
				_temp1408.last_plus_one = _temp1407 + 6;
				_temp1408; }));
	   if (_temp1330->name != 0) {
	     Cyc_Absyndump_dumpqvar((struct _tuple0 *)({ struct Cyc_Core_Opt *_temp1409 = _temp1330->name;
				      if (_temp1409 == 0) {
					_throw(Null_Exception);
				      }
				      _temp1409->v; }));
	   }
	   Cyc_Absyndump_dumptvars(_temp1330->tvs);
	   if (_temp1330->fields == 0) {
	     Cyc_Absyndump_dump_semi();
	   }
	   else {
	     Cyc_Absyndump_dump_char((int)'{');
	     Cyc_Absyndump_dumpstructfields((struct Cyc_List_List *)({ struct Cyc_Core_Opt *_temp1410 = _temp1330->fields;
					      if (_temp1410 == 0) {
						_throw(Null_Exception);
					      }
					      _temp1410->v; }));
	     Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1411 = (char *)"}";
				  struct _tagged_string _temp1412;
				  _temp1412.curr = _temp1411;
				  _temp1412.base = _temp1411;
				  _temp1412.last_plus_one = _temp1411 + 2;
				  _temp1412; }));
	     Cyc_Absyndump_dumpatts(_temp1330->attributes);
	     Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1413 = (char *)";";
				  struct _tagged_string _temp1414;
				  _temp1414.curr = _temp1413;
				  _temp1414.base = _temp1413;
				  _temp1414.last_plus_one = _temp1413 + 2;
				  _temp1414; }));
	   }
	   goto _LL1303;
  _LL1311: Cyc_Absyndump_dumpscope(_temp1347);
	   ((void (*)(struct Cyc_Absyn_Tqual,void *,void (*f)(struct _tuple0 *),
		      struct _tuple0 *))Cyc_Absyndump_dumptqtd)(_temp1343,
								_temp1341,
								Cyc_Absyndump_dumpqvar,
								_temp1345);
	   Cyc_Absyndump_dumpatts(_temp1335);
	   if (_temp1339 != 0) {
	     Cyc_Absyndump_dump_char((int)'=');
	     Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp *)({ struct Cyc_Absyn_Exp *_temp1415 = _temp1339;
				     if (_temp1415 == 0) {
				       _throw(Null_Exception);
				     }
				     _temp1415; }));
	   }
	   Cyc_Absyndump_dump_semi();
	   goto _LL1303;
  _LL1313: Cyc_Absyndump_dumpscope(_temp1360);
	   if (_temp1352) {
	     Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1416 = (char *)"xtunion ";
				  struct _tagged_string _temp1417;
				  _temp1417.curr = _temp1416;
				  _temp1417.base = _temp1416;
				  _temp1417.last_plus_one = _temp1416 + 9;
				  _temp1417; }));
	   }
	   else {
	     Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1418 = (char *)"tunion ";
				  struct _tagged_string _temp1419;
				  _temp1419.curr = _temp1418;
				  _temp1419.base = _temp1418;
				  _temp1419.last_plus_one = _temp1418 + 8;
				  _temp1419; }));
	   }
	   Cyc_Absyndump_dumpqvar(_temp1358);
	   Cyc_Absyndump_dumptvars(_temp1356);
	   if (_temp1354 == 0) {
	     Cyc_Absyndump_dump_semi();
	   }
	   else {
	     Cyc_Absyndump_dump_char((int)'{');
	     Cyc_Absyndump_dumptunionfields((struct Cyc_List_List *)({ struct Cyc_Core_Opt *_temp1420 = _temp1354;
					      if (_temp1420 == 0) {
						_throw(Null_Exception);
					      }
					      _temp1420->v; }));
	     Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp1421 = (char *)"};";
					  struct _tagged_string _temp1422;
					  _temp1422.curr = _temp1421;
					  _temp1422.base = _temp1421;
					  _temp1422.last_plus_one = _temp1421 + 3;
					  _temp1422; }));
	   }
	   goto _LL1303;
  _LL1315: Cyc_Absyndump_dumpscope(_temp1369);
	   Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1423 = (char *)"enum ";
				struct _tagged_string _temp1424;
				_temp1424.curr = _temp1423;
				_temp1424.base = _temp1423;
				_temp1424.last_plus_one = _temp1423 + 6;
				_temp1424; }));
	   Cyc_Absyndump_dumpqvar(_temp1367);
	   Cyc_Absyndump_dump_char((int)'{');
	   Cyc_Absyndump_dumpenumfields(_temp1365);
	   Cyc_Absyndump_dump_nospace((struct _tagged_string)({ char *_temp1425 = (char *)"};";
					struct _tagged_string _temp1426;
					_temp1426.curr = _temp1425;
					_temp1426.base = _temp1425;
					_temp1426.last_plus_one = _temp1425 + 3;
					_temp1426; }));
	   return;
  _LL1317: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1427 = (char *)"let";
				struct _tagged_string _temp1428;
				_temp1428.curr = _temp1427;
				_temp1428.base = _temp1427;
				_temp1428.last_plus_one = _temp1427 + 4;
				_temp1428; }));
	   Cyc_Absyndump_dumppat(_temp1379);
	   Cyc_Absyndump_dump_char((int)'=');
	   Cyc_Absyndump_dumpexp(_temp1373);
	   Cyc_Absyndump_dump_semi();
	   goto _LL1303;
  _LL1319: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1429 = (char *)"typedef";
				struct _tagged_string _temp1430;
				_temp1430.curr = _temp1429;
				_temp1430.base = _temp1429;
				_temp1430.last_plus_one = _temp1429 + 8;
				_temp1430; }));
	   ((void (*)(struct Cyc_Absyn_Tqual,void *,void (*f)(struct Cyc_Absyn_Typedefdecl *),
		      struct Cyc_Absyn_Typedefdecl *))Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual _temp1431;
										_temp1431.q_const = 0;
										_temp1431.q_volatile = 0;
										_temp1431.q_restrict = 0;
										_temp1431; }),
									      (void *)_temp1381->defn,
									      Cyc_Absyndump_dumptypedefname,
									      _temp1381);
	   Cyc_Absyndump_dump_semi();
	   goto _LL1303;
  _LL1321: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1432 = (char *)"namespace";
				struct _tagged_string _temp1433;
				_temp1433.curr = _temp1432;
				_temp1433.base = _temp1432;
				_temp1433.last_plus_one = _temp1432 + 10;
				_temp1433; }));
	   Cyc_Absyndump_dump_str(_temp1385);
	   Cyc_Absyndump_dump_char((int)'{');
	   for(0; _temp1383 != 0; _temp1383 = ({ struct Cyc_List_List *_temp1434 = _temp1383;
	     if (_temp1434 == 0) {
	       _throw(Null_Exception);
	     }
	     _temp1434->tl; })) {
	     Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl *)({ struct Cyc_List_List *_temp1435 = _temp1383;
				      if (_temp1435 == 0) {
					_throw(Null_Exception);
				      }
				      _temp1435->hd; }));
	   }
	   Cyc_Absyndump_dump_char((int)'}');
	   goto _LL1303;
  _LL1323: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1436 = (char *)"using";
				struct _tagged_string _temp1437;
				_temp1437.curr = _temp1436;
				_temp1437.base = _temp1436;
				_temp1437.last_plus_one = _temp1436 + 6;
				_temp1437; }));
	   Cyc_Absyndump_dumpqvar(_temp1389);
	   Cyc_Absyndump_dump_char((int)'{');
	   for(0; _temp1387 != 0; _temp1387 = ({ struct Cyc_List_List *_temp1438 = _temp1387;
	     if (_temp1438 == 0) {
	       _throw(Null_Exception);
	     }
	     _temp1438->tl; })) {
	     Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl *)({ struct Cyc_List_List *_temp1439 = _temp1387;
				      if (_temp1439 == 0) {
					_throw(Null_Exception);
				      }
				      _temp1439->hd; }));
	   }
	   Cyc_Absyndump_dump_char((int)'}');
	   goto _LL1303;
  _LL1325: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1440 = (char *)"extern \"C\" {";
				struct _tagged_string _temp1441;
				_temp1441.curr = _temp1440;
				_temp1441.base = _temp1440;
				_temp1441.last_plus_one = _temp1440 + 13;
				_temp1441; }));
	   for(0; _temp1391 != 0; _temp1391 = ({ struct Cyc_List_List *_temp1442 = _temp1391;
	     if (_temp1442 == 0) {
	       _throw(Null_Exception);
	     }
	     _temp1442->tl; })) {
	     Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl *)({ struct Cyc_List_List *_temp1443 = _temp1391;
				      if (_temp1443 == 0) {
					_throw(Null_Exception);
				      }
				      _temp1443->hd; }));
	   }
	   Cyc_Absyndump_dump_char((int)'}');
	   goto _LL1303;
  _LL1303: ;
}
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp *e) {
  unsigned int i = Cyc_Evexp_eval_const_uint_exp(e);
  if (i != 1) {
    Cyc_Absyndump_dump_char((int)'{');
    Cyc_Absyndump_dumpexp(e);
    Cyc_Absyndump_dump_char((int)'}');
  }
}
void Cyc_Absyndump_dumptms(struct Cyc_List_List *tms,void (*f)(void *),void *a) {
  if (tms == 0) {
    f(a);
    return;
  }
  { void *_temp1445 = (void *)({ struct Cyc_List_List *_temp1444 = tms;
      if (_temp1444 == 0) {
	_throw(Null_Exception);
      }
      _temp1444->hd; });
    struct Cyc_Absyn_Tqual _temp1463;
    void *_temp1465;
    void *_temp1467;
    struct Cyc_Absyn_Exp *_temp1469;
    struct Cyc_Absyn_Tqual _temp1471;
    void *_temp1473;
    void *_temp1475;
    struct Cyc_Absyn_Exp *_temp1477;
    struct Cyc_Absyn_Tqual _temp1479;
    void *_temp1481;
    void *_temp1483;
    struct Cyc_Absyn_Tqual _temp1485;
    void *_temp1487;
    struct Cyc_Absyn_Tvar *_temp1489;
    void *_temp1491;
    struct Cyc_Absyn_Exp *_temp1493;
    struct Cyc_Absyn_Tqual _temp1495;
    void *_temp1497;
    struct Cyc_Absyn_Tvar *_temp1499;
    void *_temp1501;
    struct Cyc_Absyn_Exp *_temp1503;
    struct Cyc_Absyn_Tqual _temp1505;
    void *_temp1507;
    struct Cyc_Absyn_Tvar *_temp1509;
    void *_temp1511;
    struct Cyc_Absyn_Tqual _temp1513;
    void *_temp1515;
    void *_temp1517;
    _LL1447: if ((unsigned int)_temp1445 > 1 ? *((int *)_temp1445) == Cyc_Absyn_Pointer_mod : 0) {
      _LL1468: _temp1467 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f1;
	       if ((unsigned int)_temp1467 > 1 ? *((int *)_temp1467) == Cyc_Absyn_Nullable_ps : 0) {
		 _LL1470: _temp1469 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Nullable_ps_struct *)_temp1467)->f1;
			  goto _LL1466;
	       }
	       else {
		 goto _LL1449;
	       }
      _LL1466: _temp1465 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f2;
	       if (_temp1465 == (void *)Cyc_Absyn_HeapRgn) {
		 goto _LL1464;
	       }
	       else {
		 goto _LL1449;
	       }
      _LL1464: _temp1463 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f3;
	       goto _LL1448;
    }
    else {
      goto _LL1449;
    }
    _LL1449: if ((unsigned int)_temp1445 > 1 ? *((int *)_temp1445) == Cyc_Absyn_Pointer_mod : 0) {
      _LL1476: _temp1475 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f1;
	       if ((unsigned int)_temp1475 > 1 ? *((int *)_temp1475) == Cyc_Absyn_NonNullable_ps : 0) {
		 _LL1478: _temp1477 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_NonNullable_ps_struct *)_temp1475)->f1;
			  goto _LL1474;
	       }
	       else {
		 goto _LL1451;
	       }
      _LL1474: _temp1473 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f2;
	       if (_temp1473 == (void *)Cyc_Absyn_HeapRgn) {
		 goto _LL1472;
	       }
	       else {
		 goto _LL1451;
	       }
      _LL1472: _temp1471 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f3;
	       goto _LL1450;
    }
    else {
      goto _LL1451;
    }
    _LL1451: if ((unsigned int)_temp1445 > 1 ? *((int *)_temp1445) == Cyc_Absyn_Pointer_mod : 0) {
      _LL1484: _temp1483 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f1;
	       if (_temp1483 == (void *)Cyc_Absyn_TaggedArray_ps) {
		 goto _LL1482;
	       }
	       else {
		 goto _LL1453;
	       }
      _LL1482: _temp1481 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f2;
	       if (_temp1481 == (void *)Cyc_Absyn_HeapRgn) {
		 goto _LL1480;
	       }
	       else {
		 goto _LL1453;
	       }
      _LL1480: _temp1479 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f3;
	       goto _LL1452;
    }
    else {
      goto _LL1453;
    }
    _LL1453: if ((unsigned int)_temp1445 > 1 ? *((int *)_temp1445) == Cyc_Absyn_Pointer_mod : 0) {
      _LL1492: _temp1491 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f1;
	       if ((unsigned int)_temp1491 > 1 ? *((int *)_temp1491) == Cyc_Absyn_Nullable_ps : 0) {
		 _LL1494: _temp1493 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_Nullable_ps_struct *)_temp1491)->f1;
			  goto _LL1488;
	       }
	       else {
		 goto _LL1455;
	       }
      _LL1488: _temp1487 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f2;
	       if ((unsigned int)_temp1487 > 4 ? *((int *)_temp1487) == Cyc_Absyn_VarType : 0) {
		 _LL1490: _temp1489 = (struct Cyc_Absyn_Tvar *)((struct Cyc_Absyn_VarType_struct *)_temp1487)->f1;
			  goto _LL1486;
	       }
	       else {
		 goto _LL1455;
	       }
      _LL1486: _temp1485 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f3;
	       goto _LL1454;
    }
    else {
      goto _LL1455;
    }
    _LL1455: if ((unsigned int)_temp1445 > 1 ? *((int *)_temp1445) == Cyc_Absyn_Pointer_mod : 0) {
      _LL1502: _temp1501 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f1;
	       if ((unsigned int)_temp1501 > 1 ? *((int *)_temp1501) == Cyc_Absyn_NonNullable_ps : 0) {
		 _LL1504: _temp1503 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_NonNullable_ps_struct *)_temp1501)->f1;
			  goto _LL1498;
	       }
	       else {
		 goto _LL1457;
	       }
      _LL1498: _temp1497 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f2;
	       if ((unsigned int)_temp1497 > 4 ? *((int *)_temp1497) == Cyc_Absyn_VarType : 0) {
		 _LL1500: _temp1499 = (struct Cyc_Absyn_Tvar *)((struct Cyc_Absyn_VarType_struct *)_temp1497)->f1;
			  goto _LL1496;
	       }
	       else {
		 goto _LL1457;
	       }
      _LL1496: _temp1495 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f3;
	       goto _LL1456;
    }
    else {
      goto _LL1457;
    }
    _LL1457: if ((unsigned int)_temp1445 > 1 ? *((int *)_temp1445) == Cyc_Absyn_Pointer_mod : 0) {
      _LL1512: _temp1511 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f1;
	       if (_temp1511 == (void *)Cyc_Absyn_TaggedArray_ps) {
		 goto _LL1508;
	       }
	       else {
		 goto _LL1459;
	       }
      _LL1508: _temp1507 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f2;
	       if ((unsigned int)_temp1507 > 4 ? *((int *)_temp1507) == Cyc_Absyn_VarType : 0) {
		 _LL1510: _temp1509 = (struct Cyc_Absyn_Tvar *)((struct Cyc_Absyn_VarType_struct *)_temp1507)->f1;
			  goto _LL1506;
	       }
	       else {
		 goto _LL1459;
	       }
      _LL1506: _temp1505 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f3;
	       goto _LL1458;
    }
    else {
      goto _LL1459;
    }
    _LL1459: if ((unsigned int)_temp1445 > 1 ? *((int *)_temp1445) == Cyc_Absyn_Pointer_mod : 0) {
      _LL1518: _temp1517 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f1;
	       goto _LL1516;
      _LL1516: _temp1515 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f2;
	       goto _LL1514;
      _LL1514: _temp1513 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1445)->f3;
	       goto _LL1460;
    }
    else {
      goto _LL1461;
    }
    _LL1461: goto _LL1462;
    _LL1448: Cyc_Absyndump_dump_char((int)'*');
	     Cyc_Absyndump_dump_upperbound(_temp1469);
	     ((void (*)(struct Cyc_List_List *tms,void (*f)(void *),void *a))Cyc_Absyndump_dumptms)(({ struct Cyc_List_List *_temp1519 = tms;
												      if (_temp1519 == 0) {
													_throw(Null_Exception);
												      }
												      _temp1519->tl; }),
												    f,
												    a);
	     return;
    _LL1450: Cyc_Absyndump_dump_char((int)'@');
	     Cyc_Absyndump_dump_upperbound(_temp1477);
	     ((void (*)(struct Cyc_List_List *tms,void (*f)(void *),void *a))Cyc_Absyndump_dumptms)(({ struct Cyc_List_List *_temp1520 = tms;
												      if (_temp1520 == 0) {
													_throw(Null_Exception);
												      }
												      _temp1520->tl; }),
												    f,
												    a);
	     return;
    _LL1452: Cyc_Absyndump_dump_char((int)'?');
	     ((void (*)(struct Cyc_List_List *tms,void (*f)(void *),void *a))Cyc_Absyndump_dumptms)(({ struct Cyc_List_List *_temp1521 = tms;
												      if (_temp1521 == 0) {
													_throw(Null_Exception);
												      }
												      _temp1521->tl; }),
												    f,
												    a);
	     return;
    _LL1454: Cyc_Absyndump_dump_char((int)'*');
	     Cyc_Absyndump_dump_upperbound(_temp1493);
	     Cyc_Absyndump_dump_str(_temp1489->name);
	     ((void (*)(struct Cyc_List_List *tms,void (*f)(void *),void *a))Cyc_Absyndump_dumptms)(({ struct Cyc_List_List *_temp1522 = tms;
												      if (_temp1522 == 0) {
													_throw(Null_Exception);
												      }
												      _temp1522->tl; }),
												    f,
												    a);
	     return;
    _LL1456: Cyc_Absyndump_dump_char((int)'@');
	     Cyc_Absyndump_dump_upperbound(_temp1503);
	     Cyc_Absyndump_dump_str(_temp1499->name);
	     ((void (*)(struct Cyc_List_List *tms,void (*f)(void *),void *a))Cyc_Absyndump_dumptms)(({ struct Cyc_List_List *_temp1523 = tms;
												      if (_temp1523 == 0) {
													_throw(Null_Exception);
												      }
												      _temp1523->tl; }),
												    f,
												    a);
	     return;
    _LL1458: Cyc_Absyndump_dump_char((int)'?');
	     Cyc_Absyndump_dump_str(_temp1509->name);
	     ((void (*)(struct Cyc_List_List *tms,void (*f)(void *),void *a))Cyc_Absyndump_dumptms)(({ struct Cyc_List_List *_temp1524 = tms;
												      if (_temp1524 == 0) {
													_throw(Null_Exception);
												      }
												      _temp1524->tl; }),
												    f,
												    a);
	     return;
    _LL1460: (void)_throw((void *)({ struct Cyc_Core_Impossible_struct *_temp1525 = (struct Cyc_Core_Impossible_struct *)GC_malloc(sizeof(struct Cyc_Core_Impossible_struct));
			    _temp1525[0] = ({ struct Cyc_Core_Impossible_struct _temp1526;
			      _temp1526.tag = Cyc_Core_Impossible;
			      _temp1526.f1 = (struct _tagged_string)({ char *_temp1527 = (char *)"dumptms: bad Pointer_mod";
				struct _tagged_string _temp1528;
				_temp1528.curr = _temp1527;
				_temp1528.base = _temp1527;
				_temp1528.last_plus_one = _temp1527 + 25;
				_temp1528; });
			      _temp1526; });
			    _temp1525; }));
    _LL1462: {
    int next_is_pointer = 0;
      if (({ struct Cyc_List_List *_temp1529 = tms;
	if (_temp1529 == 0) {
	  _throw(Null_Exception);
	}
	_temp1529->tl; }) != 0) {
	void *_temp1532 = (void *)({ struct Cyc_List_List *_temp1531 = ({ struct Cyc_List_List *_temp1530 = tms;
	    if (_temp1530 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp1530->tl; });
	  if (_temp1531 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp1531->hd; });
	struct Cyc_Absyn_Tqual _temp1538;
	void *_temp1540;
	void *_temp1542;
	_LL1534: if ((unsigned int)_temp1532 > 1 ? *((int *)_temp1532) == Cyc_Absyn_Pointer_mod : 0) {
	  _LL1543: _temp1542 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1532)->f1;
		   goto _LL1541;
	  _LL1541: _temp1540 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1532)->f2;
		   goto _LL1539;
	  _LL1539: _temp1538 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1532)->f3;
		   goto _LL1535;
	}
	else {
	  goto _LL1536;
	}
	_LL1536: goto _LL1537;
	_LL1535: next_is_pointer = 1;
		 goto _LL1533;
	_LL1537: goto _LL1533;
	_LL1533: ;
      }
      if (next_is_pointer) {
	Cyc_Absyndump_dump_char((int)'(');
      }
      ((void (*)(struct Cyc_List_List *tms,void (*f)(void *),void *a))Cyc_Absyndump_dumptms)(({ struct Cyc_List_List *_temp1544 = tms;
											       if (_temp1544 == 0) {
												 _throw(Null_Exception);
											       }
											       _temp1544->tl; }),
											     f,
											     a);
      if (next_is_pointer) {
	Cyc_Absyndump_dump_char((int)')');
      }
      { void *_temp1546 = (void *)({ struct Cyc_List_List *_temp1545 = tms;
	  if (_temp1545 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp1545->hd; });
	struct Cyc_Absyn_Exp *_temp1562;
	void *_temp1564;
	struct Cyc_Core_Opt *_temp1566;
	int _temp1568;
	struct Cyc_List_List *_temp1570;
	void *_temp1572;
	struct Cyc_Position_Segment *_temp1574;
	struct Cyc_List_List *_temp1576;
	int _temp1578;
	struct Cyc_Position_Segment *_temp1580;
	struct Cyc_List_List *_temp1582;
	struct Cyc_List_List *_temp1584;
	struct Cyc_Position_Segment *_temp1586;
	struct Cyc_Absyn_Tqual _temp1588;
	void *_temp1590;
	void *_temp1592;
	_LL1548: if (_temp1546 == (void *)Cyc_Absyn_Carray_mod) {
	  goto _LL1549;
	}
	else {
	  goto _LL1550;
	}
	_LL1550: if ((unsigned int)_temp1546 > 1 ? *((int *)_temp1546) == Cyc_Absyn_ConstArray_mod : 0) {
	  _LL1563: _temp1562 = (struct Cyc_Absyn_Exp *)((struct Cyc_Absyn_ConstArray_mod_struct *)_temp1546)->f1;
		   goto _LL1551;
	}
	else {
	  goto _LL1552;
	}
	_LL1552: if ((unsigned int)_temp1546 > 1 ? *((int *)_temp1546) == Cyc_Absyn_Function_mod : 0) {
	  _LL1565: _temp1564 = (void *)((struct Cyc_Absyn_Function_mod_struct *)_temp1546)->f1;
		   if (*((int *)_temp1564) == Cyc_Absyn_WithTypes) {
		     _LL1571: _temp1570 = (struct Cyc_List_List *)((struct Cyc_Absyn_WithTypes_struct *)_temp1564)->f1;
			      goto _LL1569;
		     _LL1569: _temp1568 = (int)((struct Cyc_Absyn_WithTypes_struct *)_temp1564)->f2;
			      goto _LL1567;
		     _LL1567: _temp1566 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_WithTypes_struct *)_temp1564)->f3;
			      goto _LL1553;
		   }
		   else {
		     goto _LL1554;
		   }
	}
	else {
	  goto _LL1554;
	}
	_LL1554: if ((unsigned int)_temp1546 > 1 ? *((int *)_temp1546) == Cyc_Absyn_Function_mod : 0) {
	  _LL1573: _temp1572 = (void *)((struct Cyc_Absyn_Function_mod_struct *)_temp1546)->f1;
		   if (*((int *)_temp1572) == Cyc_Absyn_NoTypes) {
		     _LL1577: _temp1576 = (struct Cyc_List_List *)((struct Cyc_Absyn_NoTypes_struct *)_temp1572)->f1;
			      goto _LL1575;
		     _LL1575: _temp1574 = (struct Cyc_Position_Segment *)((struct Cyc_Absyn_NoTypes_struct *)_temp1572)->f2;
			      goto _LL1555;
		   }
		   else {
		     goto _LL1556;
		   }
	}
	else {
	  goto _LL1556;
	}
	_LL1556: if ((unsigned int)_temp1546 > 1 ? *((int *)_temp1546) == Cyc_Absyn_TypeParams_mod : 0) {
	  _LL1583: _temp1582 = (struct Cyc_List_List *)((struct Cyc_Absyn_TypeParams_mod_struct *)_temp1546)->f1;
		   goto _LL1581;
	  _LL1581: _temp1580 = (struct Cyc_Position_Segment *)((struct Cyc_Absyn_TypeParams_mod_struct *)_temp1546)->f2;
		   goto _LL1579;
	  _LL1579: _temp1578 = (int)((struct Cyc_Absyn_TypeParams_mod_struct *)_temp1546)->f3;
		   goto _LL1557;
	}
	else {
	  goto _LL1558;
	}
	_LL1558: if ((unsigned int)_temp1546 > 1 ? *((int *)_temp1546) == Cyc_Absyn_Attributes_mod : 0) {
	  _LL1587: _temp1586 = (struct Cyc_Position_Segment *)((struct Cyc_Absyn_Attributes_mod_struct *)_temp1546)->f1;
		   goto _LL1585;
	  _LL1585: _temp1584 = (struct Cyc_List_List *)((struct Cyc_Absyn_Attributes_mod_struct *)_temp1546)->f2;
		   goto _LL1559;
	}
	else {
	  goto _LL1560;
	}
	_LL1560: if ((unsigned int)_temp1546 > 1 ? *((int *)_temp1546) == Cyc_Absyn_Pointer_mod : 0) {
	  _LL1593: _temp1592 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1546)->f1;
		   goto _LL1591;
	  _LL1591: _temp1590 = (void *)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1546)->f2;
		   goto _LL1589;
	  _LL1589: _temp1588 = (struct Cyc_Absyn_Tqual)((struct Cyc_Absyn_Pointer_mod_struct *)_temp1546)->f3;
		   goto _LL1561;
	}
	else {
	  goto _LL1547;
	}
	_LL1549: Cyc_Absyndump_dump((struct _tagged_string)({ char *_temp1594 = (char *)"[]";
				      struct _tagged_string _temp1595;
				      _temp1595.curr = _temp1594;
				      _temp1595.base = _temp1594;
				      _temp1595.last_plus_one = _temp1594 + 3;
				      _temp1595; }));
		 goto _LL1547;
	_LL1551: Cyc_Absyndump_dump_char((int)'[');
		 Cyc_Absyndump_dumpexp(_temp1562);
		 Cyc_Absyndump_dump_char((int)']');
		 goto _LL1547;
	_LL1553: Cyc_Absyndump_dumpfunargs(_temp1570,_temp1568,_temp1566);
		 goto _LL1547;
	_LL1555: ((void (*)(void (*f)(struct _tagged_string *),struct Cyc_List_List *l,
			    struct _tagged_string start,struct _tagged_string end,
			    struct _tagged_string sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,
									    _temp1576,
									    (struct _tagged_string)({ char *_temp1596 = (char *)"(";
									      struct _tagged_string _temp1597;
									      _temp1597.curr = _temp1596;
									      _temp1597.base = _temp1596;
									      _temp1597.last_plus_one = _temp1596 + 2;
									      _temp1597; }),
									    (struct _tagged_string)({ char *_temp1598 = (char *)")";
									      struct _tagged_string _temp1599;
									      _temp1599.curr = _temp1598;
									      _temp1599.base = _temp1598;
									      _temp1599.last_plus_one = _temp1598 + 2;
									      _temp1599; }),
									    (struct _tagged_string)({ char *_temp1600 = (char *)",";
									      struct _tagged_string _temp1601;
									      _temp1601.curr = _temp1600;
									      _temp1601.base = _temp1600;
									      _temp1601.last_plus_one = _temp1600 + 2;
									      _temp1601; }));
		 goto _LL1547;
	_LL1557: if (_temp1578) {
		   Cyc_Absyndump_dumpkindedtvars(_temp1582);
		 }
		 else {
		   Cyc_Absyndump_dumptvars(_temp1582);
		 }
		 goto _LL1547;
	_LL1559: Cyc_Absyndump_dumpatts(_temp1584);
		 goto _LL1547;
	_LL1561: (void)_throw((void *)({ struct Cyc_Core_Impossible_struct *_temp1602 = (struct Cyc_Core_Impossible_struct *)GC_malloc(sizeof(struct Cyc_Core_Impossible_struct));
				_temp1602[0] = ({ struct Cyc_Core_Impossible_struct _temp1603;
				  _temp1603.tag = Cyc_Core_Impossible;
				  _temp1603.f1 = (struct _tagged_string)({ char *_temp1604 = (char *)"dumptms";
				    struct _tagged_string _temp1605;
				    _temp1605.curr = _temp1604;
				    _temp1605.base = _temp1604;
				    _temp1605.last_plus_one = _temp1604 + 8;
				    _temp1605; });
				  _temp1603; });
				_temp1602; }));
	_LL1547: ;
      }
      return;
    }
    _LL1446: ;}
  
}
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void *t,void (*f)(void *),
			    void *a) {
  struct Cyc_List_List *_temp1608;
  void *_temp1610;
  struct Cyc_Absyn_Tqual _temp1612;
  struct _tuple4 _temp1606 = Cyc_Absynpp_to_tms((struct Cyc_Absyn_Tqual)tq,
						t);
  _LL1613: _temp1612 = _temp1606.f1;
	   goto _LL1611;
  _LL1611: _temp1610 = _temp1606.f2;
	   goto _LL1609;
  _LL1609: _temp1608 = _temp1606.f3;
	   goto _LL1607;
  _LL1607: Cyc_Absyndump_dumptq(_temp1612);
	   Cyc_Absyndump_dumpntyp(_temp1610);
	   ((void (*)(struct Cyc_List_List *tms,void (*f)(void *),void *a))Cyc_Absyndump_dumptms)(((struct Cyc_List_List *(*)(struct Cyc_List_List *x))Cyc_List_imp_rev)(_temp1608),
												  f,
												  a);
}
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List *tdl,struct Cyc_Stdio___sFILE *f) {
  Cyc_Absyndump_pos = 0;
  *Cyc_Absyndump_dump_file = f;
  for(0; tdl != 0; tdl = ({ struct Cyc_List_List *_temp1614 = tdl;
    if (_temp1614 == 0) {
      _throw(Null_Exception);
    }
    _temp1614->tl; })) {
    Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl *)({ struct Cyc_List_List *_temp1615 = tdl;
			     if (_temp1615 == 0) {
			       _throw(Null_Exception);
			     }
			     _temp1615->hd; }));
  }
}
