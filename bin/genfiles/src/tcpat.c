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
extern struct Cyc_List_List *Cyc_List_rmap(struct _RegionHandle *,void *(*f)(void *),
					   struct Cyc_List_List *x);
extern struct Cyc_List_List *Cyc_List_map_c(void *(*f)(void *,void *),void *env,
					    struct Cyc_List_List *x);
extern struct Cyc_List_List *Cyc_List_rmap_c(struct _RegionHandle *,void *(*f)(void *,
									       void *),
					     void *env,struct Cyc_List_List *x);
extern char Cyc_List_List_mismatch[18];
extern struct Cyc_List_List *Cyc_List_rev(struct Cyc_List_List *x);
extern struct Cyc_List_List *Cyc_List_imp_rev(struct Cyc_List_List *x);
extern struct Cyc_List_List *Cyc_List_append(struct Cyc_List_List *x,struct Cyc_List_List *y);
extern struct Cyc_List_List *Cyc_List_rappend(struct _RegionHandle *,struct Cyc_List_List *x,
					      struct Cyc_List_List *y);
extern char Cyc_List_Nth[8];
extern struct Cyc_List_List *Cyc_List_rzip(struct _RegionHandle *r3,struct _RegionHandle *r4,
					   struct Cyc_List_List *x,struct Cyc_List_List *y);
extern struct Cyc_List_List *Cyc_List_tabulate_c(int n,void *(*f)(void *,
								  int),void *env);
struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict *Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict *Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];
struct Cyc_Set_Set;
typedef struct Cyc_Set_Set *Cyc_Set_gset_t;
typedef struct Cyc_Set_Set *Cyc_Set_hset_t;
typedef struct Cyc_Set_Set *Cyc_Set_set_t;
extern struct Cyc_Set_Set *Cyc_Set_empty(int (*comp)(void *,void *));
extern struct Cyc_Set_Set *Cyc_Set_insert(struct Cyc_Set_Set *s,void *elt);
extern int Cyc_Set_cardinality(struct Cyc_Set_Set *s);
extern int Cyc_Set_member(struct Cyc_Set_Set *s,void *elt);
extern char Cyc_Set_Absent[11];
struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError[18];
struct Cyc_Stdio_FileOpenError_struct {
  char *tag;
  struct _tagged_string f1;
};
extern char Cyc_Stdio_FileCloseError[19];
extern int Cyc_String_strcmp(struct _tagged_string s1,struct _tagged_string s2);
extern int Cyc_String_zstrcmp(struct _tagged_string,struct _tagged_string);
extern int Cyc_String_zstrptrcmp(struct _tagged_string *,struct _tagged_string *);
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
extern struct Cyc_Absyn_Conref *Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref *x);
extern void *Cyc_Absyn_new_evar(void *);
extern void *Cyc_Absyn_wildtyp();
extern void *Cyc_Absyn_uchar_t;
extern void *Cyc_Absyn_uint_t;
extern void *Cyc_Absyn_sint_t;
extern void *Cyc_Absyn_float_t;
extern struct Cyc_Absyn_Vardecl *Cyc_Absyn_new_vardecl(struct _tuple1 *x,
						       void *t,struct Cyc_Absyn_Exp *init);
struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate *Cyc_PP_ppstate_t;
struct Cyc_PP_Out;
typedef struct Cyc_PP_Out *Cyc_PP_out_t;
struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc *Cyc_PP_doc_t;
extern struct _tagged_string Cyc_Absynpp_typ2string(void *);
extern struct _tagged_string Cyc_Absynpp_kind2string(void *);
extern struct _tagged_string Cyc_Absynpp_qvar2string(struct _tuple1 *);
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
extern struct Cyc_Tcenv_Tenv *Cyc_Tcenv_add_type_vars(struct Cyc_Position_Segment *,
						      struct Cyc_Tcenv_Tenv *,
						      struct Cyc_List_List *);
extern void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv *,struct Cyc_Position_Segment *,
					   void *rgn);
extern char Cyc_Tcutil_TypeErr[12];
extern void *Cyc_Tcutil_impos(struct _tagged_string);
extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment *,struct _tagged_string);
extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment *,struct _tagged_string);
extern void *Cyc_Tcutil_compress(void *t);
extern int Cyc_Tcutil_coerceable(void *);
extern int Cyc_Tcutil_unify(void *,void *);
extern void *Cyc_Tcutil_rsubstitute(struct _RegionHandle *,struct Cyc_List_List *,
				    void *);
struct _tuple4 {
  struct Cyc_Absyn_Tvar *f1;
  void *f2;
};
extern struct _tuple4 *Cyc_Tcutil_r_make_inst_var(struct _RegionHandle *,
						  struct Cyc_Absyn_Tvar *);
extern void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List *vs,struct Cyc_Position_Segment *loc,
					 struct _tagged_string err_msg);
extern struct Cyc_List_List *Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle *rgn,
								   struct Cyc_Position_Segment *loc,
								   struct Cyc_List_List *des,
								   struct Cyc_List_List *fields);
struct _tuple5 {
  struct Cyc_List_List *f1;
  struct Cyc_List_List *f2;
};
extern struct _tuple5 Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Pat *p,
				      void **topt,void **region_opt);
extern void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Pat *p);
extern void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment *,
					      struct Cyc_List_List *);
extern int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment *,
					      struct Cyc_Absyn_Pat *p);
extern void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment *,struct Cyc_List_List *);
struct _tuple6 {
  struct Cyc_List_List *f1;
  struct Cyc_Absyn_Pat *f2;
};
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Pat *p) {
  void *_temp0 = (void *)p->r;
  struct _tuple1 *_temp10;
  struct Cyc_List_List *_temp12;
  struct Cyc_List_List *_temp14;
  struct _tuple1 *_temp16;
  struct Cyc_List_List *_temp18;
  struct Cyc_List_List *_temp20;
  struct _tuple1 *_temp22;
  _LL2: if ((unsigned int)_temp0 > 2 ? *((int *)_temp0) == Cyc_Absyn_UnknownId_p : 0) {
    _LL11: _temp10 = (struct _tuple1 *)((struct Cyc_Absyn_UnknownId_p_struct *)_temp0)->f1;
	   goto _LL3;
  }
  else {
    goto _LL4;
  }
  _LL4: if ((unsigned int)_temp0 > 2 ? *((int *)_temp0) == Cyc_Absyn_UnknownCall_p : 0) {
    _LL17: _temp16 = (struct _tuple1 *)((struct Cyc_Absyn_UnknownCall_p_struct *)_temp0)->f1;
	   goto _LL15;
    _LL15: _temp14 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownCall_p_struct *)_temp0)->f2;
	   goto _LL13;
    _LL13: _temp12 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownCall_p_struct *)_temp0)->f3;
	   goto _LL5;
  }
  else {
    goto _LL6;
  }
  _LL6: if ((unsigned int)_temp0 > 2 ? *((int *)_temp0) == Cyc_Absyn_UnknownFields_p : 0) {
    _LL23: _temp22 = (struct _tuple1 *)((struct Cyc_Absyn_UnknownFields_p_struct *)_temp0)->f1;
	   goto _LL21;
    _LL21: _temp20 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownFields_p_struct *)_temp0)->f2;
	   goto _LL19;
    _LL19: _temp18 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownFields_p_struct *)_temp0)->f3;
	   goto _LL7;
  }
  else {
    goto _LL8;
  }
  _LL8: goto _LL9;
  _LL3: { struct _handler_cons _temp24;
    _push_handler(&_temp24);
    { void *_temp25 = (void *)setjmp(_temp24.handler);
      if (! _temp25) {
	{ void *_temp26 = Cyc_Tcenv_lookup_ordinary(te,p->loc,_temp10);
	  struct Cyc_Absyn_Structdecl *_temp36;
	  struct Cyc_Absyn_Tunionfield *_temp38;
	  struct Cyc_Absyn_Tuniondecl *_temp40;
	  struct Cyc_Absyn_Enumfield *_temp42;
	  struct Cyc_Absyn_Enumdecl *_temp44;
	  void *_temp46;
	  _LL28: if (*((int *)_temp26) == Cyc_Tcenv_StructRes) {
	    _LL37: _temp36 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Tcenv_StructRes_struct *)_temp26)->f1;
		   goto _LL29;
	  }
	  else {
	    goto _LL30;
	  }
	  _LL30: if (*((int *)_temp26) == Cyc_Tcenv_TunionRes) {
	    _LL41: _temp40 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Tcenv_TunionRes_struct *)_temp26)->f1;
		   goto _LL39;
	    _LL39: _temp38 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Tcenv_TunionRes_struct *)_temp26)->f2;
		   goto _LL31;
	  }
	  else {
	    goto _LL32;
	  }
	  _LL32: if (*((int *)_temp26) == Cyc_Tcenv_EnumRes) {
	    _LL45: _temp44 = (struct Cyc_Absyn_Enumdecl *)((struct Cyc_Tcenv_EnumRes_struct *)_temp26)->f1;
		   goto _LL43;
	    _LL43: _temp42 = (struct Cyc_Absyn_Enumfield *)((struct Cyc_Tcenv_EnumRes_struct *)_temp26)->f2;
		   goto _LL33;
	  }
	  else {
	    goto _LL34;
	  }
	  _LL34: if (*((int *)_temp26) == Cyc_Tcenv_VarRes) {
	    _LL47: _temp46 = (void *)((struct Cyc_Tcenv_VarRes_struct *)_temp26)->f1;
		   goto _LL35;
	  }
	  else {
	    goto _LL27;
	  }
	  _LL29: Cyc_Tcutil_terr(p->loc,(struct _tagged_string)({ char *_temp48 = (char *)"struct tag used without arguments in pattern";
				   struct _tagged_string _temp49;
				   _temp49.curr = _temp48;
				   _temp49.base = _temp48;
				   _temp49.last_plus_one = _temp48 + 45;
				   _temp49; }));
		 _npop_handler(0);
		 return;
	  _LL31: (void *)(p->r = (void *)((void *)({ struct Cyc_Absyn_Tunion_p_struct *_temp50 = (struct Cyc_Absyn_Tunion_p_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Tunion_p_struct));
		   _temp50[0] = ({ struct Cyc_Absyn_Tunion_p_struct _temp51;
		     _temp51.tag = Cyc_Absyn_Tunion_p;
		     _temp51.f1 = _temp40;
		     _temp51.f2 = _temp38;
		     _temp51.f3 = 0;
		     _temp51.f4 = 0;
		     _temp51; });
		   _temp50; })));
		 _npop_handler(0);
		 return;
	  _LL33: (void *)(p->r = (void *)((void *)({ struct Cyc_Absyn_Enum_p_struct *_temp52 = (struct Cyc_Absyn_Enum_p_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Enum_p_struct));
		   _temp52[0] = ({ struct Cyc_Absyn_Enum_p_struct _temp53;
		     _temp53.tag = Cyc_Absyn_Enum_p;
		     _temp53.f1 = _temp44;
		     _temp53.f2 = _temp42;
		     _temp53; });
		   _temp52; })));
		 _npop_handler(0);
		 return;
	  _LL35: goto _LL27;
	  _LL27: ;
	}
	;
	_pop_handler();
      }
      else {
	void *_temp55 = _temp25;
	_LL57: if (_temp55 == Cyc_Dict_Absent) {
	  goto _LL58;
	}
	else {
	  goto _LL59;
	}
	_LL59: goto _LL60;
	_LL58: goto _LL56;
	_LL60: (void)_throw(_temp55);
	_LL56: ;
      }}
    
  }
  { void *_temp61 = (*_temp10).f1;
    struct Cyc_List_List *_temp69;
    _LL63: if (_temp61 == (void *)Cyc_Absyn_Loc_n) {
      goto _LL64;
    }
    else {
      goto _LL65;
    }
    _LL65: if ((unsigned int)_temp61 > 1 ? *((int *)_temp61) == Cyc_Absyn_Rel_n : 0) {
      _LL70: _temp69 = (struct Cyc_List_List *)((struct Cyc_Absyn_Rel_n_struct *)_temp61)->f1;
	     if (_temp69 == 0) {
	       goto _LL66;
	     }
	     else {
	       goto _LL67;
	     }
    }
    else {
      goto _LL67;
    }
    _LL67: goto _LL68;
    _LL64: goto _LL66;
    _LL66: (*_temp10).f1 = (void *)Cyc_Absyn_Loc_n;
	   (void *)(p->r = (void *)((void *)({ struct Cyc_Absyn_Var_p_struct *_temp71 = (struct Cyc_Absyn_Var_p_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Var_p_struct));
	     _temp71[0] = ({ struct Cyc_Absyn_Var_p_struct _temp72;
	       _temp72.tag = Cyc_Absyn_Var_p;
	       _temp72.f1 = Cyc_Absyn_new_vardecl(_temp10,(void *)Cyc_Absyn_VoidType,
						  0);
	       _temp72; });
	     _temp71; })));
	   return;
    _LL68: Cyc_Tcutil_terr(p->loc,(struct _tagged_string)({ char *_temp73 = (char *)"qualified variable in pattern";
			     struct _tagged_string _temp74;
			     _temp74.curr = _temp73;
			     _temp74.base = _temp73;
			     _temp74.last_plus_one = _temp73 + 30;
			     _temp74; }));
	   return;
    _LL62: ;}
  
  _LL5: { struct _handler_cons _temp75;
    _push_handler(&_temp75);
    { void *_temp76 = (void *)setjmp(_temp75.handler);
      if (! _temp76) {
	{ void *_temp77 = Cyc_Tcenv_lookup_ordinary(te,p->loc,_temp16);
	  struct Cyc_Absyn_Structdecl *_temp87;
	  struct Cyc_Absyn_Tunionfield *_temp89;
	  struct Cyc_Absyn_Tuniondecl *_temp91;
	  struct Cyc_Absyn_Enumfield *_temp93;
	  struct Cyc_Absyn_Enumdecl *_temp95;
	  void *_temp97;
	  _LL79: if (*((int *)_temp77) == Cyc_Tcenv_StructRes) {
	    _LL88: _temp87 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Tcenv_StructRes_struct *)_temp77)->f1;
		   goto _LL80;
	  }
	  else {
	    goto _LL81;
	  }
	  _LL81: if (*((int *)_temp77) == Cyc_Tcenv_TunionRes) {
	    _LL92: _temp91 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Tcenv_TunionRes_struct *)_temp77)->f1;
		   goto _LL90;
	    _LL90: _temp89 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Tcenv_TunionRes_struct *)_temp77)->f2;
		   goto _LL82;
	  }
	  else {
	    goto _LL83;
	  }
	  _LL83: if (*((int *)_temp77) == Cyc_Tcenv_EnumRes) {
	    _LL96: _temp95 = (struct Cyc_Absyn_Enumdecl *)((struct Cyc_Tcenv_EnumRes_struct *)_temp77)->f1;
		   goto _LL94;
	    _LL94: _temp93 = (struct Cyc_Absyn_Enumfield *)((struct Cyc_Tcenv_EnumRes_struct *)_temp77)->f2;
		   goto _LL84;
	  }
	  else {
	    goto _LL85;
	  }
	  _LL85: if (*((int *)_temp77) == Cyc_Tcenv_VarRes) {
	    _LL98: _temp97 = (void *)((struct Cyc_Tcenv_VarRes_struct *)_temp77)->f1;
		   goto _LL86;
	  }
	  else {
	    goto _LL78;
	  }
	  _LL80: {
	  struct Cyc_List_List *new_ps = 0;
	    for(0; _temp12 != 0; _temp12 = ({ struct Cyc_List_List *_temp99 = _temp12;
	      if (_temp99 == 0) {
		_throw(Null_Exception);
	      }
	      _temp99->tl; })) {
	      new_ps = ({ struct Cyc_List_List *_temp100 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
		_temp100->hd = (void *)({ struct _tuple6 *_temp101 = (struct _tuple6 *)GC_malloc(sizeof(struct _tuple6));
		  _temp101->f1 = 0;
		  _temp101->f2 = (struct Cyc_Absyn_Pat *)({ struct Cyc_List_List *_temp102 = _temp12;
		    if (_temp102 == 0) {
		      _throw(Null_Exception);
		    }
		    _temp102->hd; });
		  _temp101; });
		_temp100->tl = new_ps;
		_temp100; });
	    }
	    (void *)(p->r = (void *)((void *)({ struct Cyc_Absyn_Struct_p_struct *_temp103 = (struct Cyc_Absyn_Struct_p_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Struct_p_struct));
	      _temp103[0] = ({ struct Cyc_Absyn_Struct_p_struct _temp104;
		_temp104.tag = Cyc_Absyn_Struct_p;
		_temp104.f1 = _temp87;
		_temp104.f2 = 0;
		_temp104.f3 = _temp14;
		_temp104.f4 = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x))Cyc_List_imp_rev)(new_ps);
		_temp104; });
	      _temp103; })));
	    _npop_handler(0);
	    return;
	  }
	  _LL82: (void *)(p->r = (void *)((void *)({ struct Cyc_Absyn_Tunion_p_struct *_temp105 = (struct Cyc_Absyn_Tunion_p_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Tunion_p_struct));
		   _temp105[0] = ({ struct Cyc_Absyn_Tunion_p_struct _temp106;
		     _temp106.tag = Cyc_Absyn_Tunion_p;
		     _temp106.f1 = _temp91;
		     _temp106.f2 = _temp89;
		     _temp106.f3 = _temp14;
		     _temp106.f4 = _temp12;
		     _temp106; });
		   _temp105; })));
		 _npop_handler(0);
		 return;
	  _LL84: Cyc_Tcutil_terr(p->loc,({ struct _tagged_string _temp107 = Cyc_Absynpp_qvar2string(_temp16);
				   xprintf("bad enum constructor %.*s in pattern",
					   _temp107.last_plus_one - _temp107.curr,
					   _temp107.curr); }));
		 _npop_handler(0);
		 return;
	  _LL86: goto _LL78;
	  _LL78: ;
	}
	;
	_pop_handler();
      }
      else {
	void *_temp109 = _temp76;
	_LL111: if (_temp109 == Cyc_Dict_Absent) {
	  goto _LL112;
	}
	else {
	  goto _LL113;
	}
	_LL113: goto _LL114;
	_LL112: goto _LL110;
	_LL114: (void)_throw(_temp109);
	_LL110: ;
      }}
    
  }
  Cyc_Tcutil_terr(p->loc,({ struct _tagged_string _temp115 = Cyc_Absynpp_qvar2string(_temp16);
		    xprintf("%.*s is not a constructor in pattern",_temp115.last_plus_one - _temp115.curr,
			    _temp115.curr); }));
  return;
  _LL7: { struct _handler_cons _temp116;
    _push_handler(&_temp116);
    { void *_temp117 = (void *)setjmp(_temp116.handler);
      if (! _temp117) {
	{ void *_temp118 = Cyc_Tcenv_lookup_ordinary(te,p->loc,_temp22);
	  struct Cyc_Absyn_Structdecl *_temp124;
	  _LL120: if (*((int *)_temp118) == Cyc_Tcenv_StructRes) {
	    _LL125: _temp124 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Tcenv_StructRes_struct *)_temp118)->f1;
		    goto _LL121;
	  }
	  else {
	    goto _LL122;
	  }
	  _LL122: goto _LL123;
	  _LL121: (void *)(p->r = (void *)((void *)({ struct Cyc_Absyn_Struct_p_struct *_temp126 = (struct Cyc_Absyn_Struct_p_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Struct_p_struct));
		    _temp126[0] = ({ struct Cyc_Absyn_Struct_p_struct _temp127;
		      _temp127.tag = Cyc_Absyn_Struct_p;
		      _temp127.f1 = _temp124;
		      _temp127.f2 = 0;
		      _temp127.f3 = _temp20;
		      _temp127.f4 = _temp18;
		      _temp127; });
		    _temp126; })));
		  _npop_handler(0);
		  return;
	  _LL123: goto _LL119;
	  _LL119: ;
	}
	;
	_pop_handler();
      }
      else {
	void *_temp129 = _temp117;
	_LL131: if (_temp129 == Cyc_Dict_Absent) {
	  goto _LL132;
	}
	else {
	  goto _LL133;
	}
	_LL133: goto _LL134;
	_LL132: goto _LL130;
	_LL134: (void)_throw(_temp129);
	_LL130: ;
      }}
    
  }
  Cyc_Tcutil_terr(p->loc,({ struct _tagged_string _temp135 = Cyc_Absynpp_qvar2string(_temp22);
		    xprintf("%.*s is not a struct name, but pattern has designators",
			    _temp135.last_plus_one - _temp135.curr,_temp135.curr); }));
  return;
  _LL9: return;
  _LL1: ;
}
static struct _tagged_string *Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl *vd) {
  return (*vd->name).f2;
}
static void *Cyc_Tcpat_any_type(void **topt) {
  if (topt != 0) {
    return ({ void **_temp136 = topt;
      if (_temp136 == 0) {
	_throw(Null_Exception);
      }
      *_temp136; });
  }
  return Cyc_Absyn_new_evar((void *)Cyc_Absyn_MemKind);
}
static void *Cyc_Tcpat_num_type(void **topt,void *numt) {
  if (topt != 0 ? Cyc_Tcutil_coerceable(({ void **_temp137 = topt;
					  if (_temp137 == 0) {
					    _throw(Null_Exception);
					  }
					  *_temp137; })) : 0) {
    return ({ void **_temp138 = topt;
      if (_temp138 == 0) {
	_throw(Null_Exception);
      }
      *_temp138; });
  }
  return numt;
}
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl *vd,struct Cyc_List_List **v_result_ptr,
			     void *t) {
  (void *)(vd->type = (void *)t);
  vd->tq = Cyc_Absyn_empty_tqual();
  *v_result_ptr = ({ struct Cyc_List_List *_temp139 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
    _temp139->hd = (void *)vd;
    _temp139->tl = *v_result_ptr;
    _temp139; });
}
struct _tuple7 {
  struct Cyc_Absyn_Tqual f1;
  void *f2;
};
struct _tuple8 {
  struct Cyc_Absyn_Structfield *f1;
  struct Cyc_Absyn_Pat *f2;
};
struct _tuple5 Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Pat *p,
				  void **topt,void **region_opt) {
  Cyc_Tcpat_resolve_pat(te,p);
  { void *t;
    struct Cyc_List_List *tv_result = 0;
    struct Cyc_List_List *v_result = 0;
    { void *_temp140 = (void *)p->r;
      struct Cyc_Absyn_Vardecl *_temp174;
      struct Cyc_Absyn_Vardecl *_temp176;
      int _temp178;
      void *_temp180;
      int _temp182;
      void *_temp184;
      char _temp186;
      struct _tagged_string _temp188;
      struct Cyc_Absyn_Enumfield *_temp190;
      struct Cyc_Absyn_Enumdecl *_temp192;
      struct Cyc_Absyn_Pat *_temp194;
      struct Cyc_List_List *_temp196;
      struct Cyc_List_List *_temp198;
      struct Cyc_List_List *_temp200;
      struct Cyc_Core_Opt *_temp202;
      struct Cyc_Core_Opt **_temp204;
      struct Cyc_Absyn_Structdecl *_temp205;
      struct Cyc_List_List *_temp207;
      struct Cyc_List_List *_temp209;
      struct Cyc_Absyn_Tunionfield *_temp211;
      struct Cyc_Absyn_Tuniondecl *_temp213;
      struct _tuple1 *_temp215;
      struct Cyc_List_List *_temp217;
      struct Cyc_List_List *_temp219;
      struct _tuple1 *_temp221;
      struct Cyc_List_List *_temp223;
      struct Cyc_List_List *_temp225;
      struct _tuple1 *_temp227;
      _LL142: if (_temp140 == (void *)Cyc_Absyn_Wild_p) {
	goto _LL143;
      }
      else {
	goto _LL144;
      }
      _LL144: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_Var_p : 0) {
	_LL175: _temp174 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Var_p_struct *)_temp140)->f1;
		goto _LL145;
      }
      else {
	goto _LL146;
      }
      _LL146: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_Reference_p : 0) {
	_LL177: _temp176 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Reference_p_struct *)_temp140)->f1;
		goto _LL147;
      }
      else {
	goto _LL148;
      }
      _LL148: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_Int_p : 0) {
	_LL181: _temp180 = (void *)((struct Cyc_Absyn_Int_p_struct *)_temp140)->f1;
		if (_temp180 == (void *)Cyc_Absyn_Unsigned) {
		  goto _LL179;
		}
		else {
		  goto _LL150;
		}
	_LL179: _temp178 = (int)((struct Cyc_Absyn_Int_p_struct *)_temp140)->f2;
		goto _LL149;
      }
      else {
	goto _LL150;
      }
      _LL150: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_Int_p : 0) {
	_LL185: _temp184 = (void *)((struct Cyc_Absyn_Int_p_struct *)_temp140)->f1;
		if (_temp184 == (void *)Cyc_Absyn_Signed) {
		  goto _LL183;
		}
		else {
		  goto _LL152;
		}
	_LL183: _temp182 = (int)((struct Cyc_Absyn_Int_p_struct *)_temp140)->f2;
		goto _LL151;
      }
      else {
	goto _LL152;
      }
      _LL152: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_Char_p : 0) {
	_LL187: _temp186 = (char)((struct Cyc_Absyn_Char_p_struct *)_temp140)->f1;
		goto _LL153;
      }
      else {
	goto _LL154;
      }
      _LL154: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_Float_p : 0) {
	_LL189: _temp188 = (struct _tagged_string)((struct Cyc_Absyn_Float_p_struct *)_temp140)->f1;
		goto _LL155;
      }
      else {
	goto _LL156;
      }
      _LL156: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_Enum_p : 0) {
	_LL193: _temp192 = (struct Cyc_Absyn_Enumdecl *)((struct Cyc_Absyn_Enum_p_struct *)_temp140)->f1;
		goto _LL191;
	_LL191: _temp190 = (struct Cyc_Absyn_Enumfield *)((struct Cyc_Absyn_Enum_p_struct *)_temp140)->f2;
		goto _LL157;
      }
      else {
	goto _LL158;
      }
      _LL158: if (_temp140 == (void *)Cyc_Absyn_Null_p) {
	goto _LL159;
      }
      else {
	goto _LL160;
      }
      _LL160: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_Pointer_p : 0) {
	_LL195: _temp194 = (struct Cyc_Absyn_Pat *)((struct Cyc_Absyn_Pointer_p_struct *)_temp140)->f1;
		goto _LL161;
      }
      else {
	goto _LL162;
      }
      _LL162: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_Tuple_p : 0) {
	_LL197: _temp196 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tuple_p_struct *)_temp140)->f1;
		goto _LL163;
      }
      else {
	goto _LL164;
      }
      _LL164: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_Struct_p : 0) {
	_LL206: _temp205 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Absyn_Struct_p_struct *)_temp140)->f1;
		goto _LL203;
	_LL203: _temp202 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Struct_p_struct *)_temp140)->f2;
		_temp204 = &((struct Cyc_Absyn_Struct_p_struct *)_temp140)->f2;
		goto _LL201;
	_LL201: _temp200 = (struct Cyc_List_List *)((struct Cyc_Absyn_Struct_p_struct *)_temp140)->f3;
		goto _LL199;
	_LL199: _temp198 = (struct Cyc_List_List *)((struct Cyc_Absyn_Struct_p_struct *)_temp140)->f4;
		goto _LL165;
      }
      else {
	goto _LL166;
      }
      _LL166: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_Tunion_p : 0) {
	_LL214: _temp213 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_Tunion_p_struct *)_temp140)->f1;
		goto _LL212;
	_LL212: _temp211 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Absyn_Tunion_p_struct *)_temp140)->f2;
		goto _LL210;
	_LL210: _temp209 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_p_struct *)_temp140)->f3;
		goto _LL208;
	_LL208: _temp207 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_p_struct *)_temp140)->f4;
		goto _LL167;
      }
      else {
	goto _LL168;
      }
      _LL168: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_UnknownId_p : 0) {
	_LL216: _temp215 = (struct _tuple1 *)((struct Cyc_Absyn_UnknownId_p_struct *)_temp140)->f1;
		goto _LL169;
      }
      else {
	goto _LL170;
      }
      _LL170: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_UnknownCall_p : 0) {
	_LL222: _temp221 = (struct _tuple1 *)((struct Cyc_Absyn_UnknownCall_p_struct *)_temp140)->f1;
		goto _LL220;
	_LL220: _temp219 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownCall_p_struct *)_temp140)->f2;
		goto _LL218;
	_LL218: _temp217 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownCall_p_struct *)_temp140)->f3;
		goto _LL171;
      }
      else {
	goto _LL172;
      }
      _LL172: if ((unsigned int)_temp140 > 2 ? *((int *)_temp140) == Cyc_Absyn_UnknownFields_p : 0) {
	_LL228: _temp227 = (struct _tuple1 *)((struct Cyc_Absyn_UnknownFields_p_struct *)_temp140)->f1;
		goto _LL226;
	_LL226: _temp225 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownFields_p_struct *)_temp140)->f2;
		goto _LL224;
	_LL224: _temp223 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownFields_p_struct *)_temp140)->f3;
		goto _LL173;
      }
      else {
	goto _LL141;
      }
      _LL143: t = Cyc_Tcpat_any_type(topt);
	      goto _LL141;
      _LL145: t = Cyc_Tcpat_any_type(topt);
	      Cyc_Tcpat_set_vd(_temp174,&v_result,t);
	      goto _LL141;
      _LL147: t = Cyc_Tcpat_any_type(topt);
	      if (region_opt == 0) {
		Cyc_Tcutil_terr(p->loc,(struct _tagged_string)({ char *_temp229 = (char *)"* pattern would point into an unknown region";
				  struct _tagged_string _temp230;
				  _temp230.curr = _temp229;
				  _temp230.base = _temp229;
				  _temp230.last_plus_one = _temp229 + 45;
				  _temp230; }));
		goto _LL141;
	      }
	      Cyc_Tcpat_set_vd(_temp176,&v_result,(void *)({ struct Cyc_Absyn_PointerType_struct *_temp231 = (struct Cyc_Absyn_PointerType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_PointerType_struct));
				 _temp231[0] = ({ struct Cyc_Absyn_PointerType_struct _temp232;
				   _temp232.tag = Cyc_Absyn_PointerType;
				   _temp232.f1 = ({ struct Cyc_Absyn_PtrInfo _temp233;
				     _temp233.elt_typ = (void *)t;
				     _temp233.rgn_typ = (void *)({ void **_temp234 = region_opt;
				       if (_temp234 == 0) {
					 _throw(Null_Exception);
				       }
				       *_temp234; });
				     _temp233.nullable = ((struct Cyc_Absyn_Conref *(*)(int x))Cyc_Absyn_new_conref)(0);
				     _temp233.tq = Cyc_Absyn_empty_tqual();
				     _temp233.bounds = ((struct Cyc_Absyn_Conref *(*)())Cyc_Absyn_empty_conref)();
				     _temp233; });
				   _temp232; });
				 _temp231; }));
	      goto _LL141;
      _LL149: t = Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_t);
	      goto _LL141;
      _LL151: t = Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_t);
	      goto _LL141;
      _LL153: t = Cyc_Tcpat_num_type(topt,Cyc_Absyn_uchar_t);
	      goto _LL141;
      _LL155: t = Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_t);
	      goto _LL141;
      _LL157: t = (void *)({ struct Cyc_Absyn_EnumType_struct *_temp235 = (struct Cyc_Absyn_EnumType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_EnumType_struct));
		_temp235[0] = ({ struct Cyc_Absyn_EnumType_struct _temp236;
		  _temp236.tag = Cyc_Absyn_EnumType;
		  _temp236.f1 = _temp192->name;
		  _temp236.f2 = (struct Cyc_Absyn_Enumdecl *)_temp192;
		  _temp236; });
		_temp235; });
	      goto _LL141;
      _LL159: if (topt != 0) {
		void *_temp238 = Cyc_Tcutil_compress(({ void **_temp237 = topt;
						       if (_temp237 == 0) {
							 _throw(Null_Exception);
						       }
						       *_temp237; }));
		struct Cyc_Absyn_PtrInfo _temp244;
		_LL240: if ((unsigned int)_temp238 > 4 ? *((int *)_temp238) == Cyc_Absyn_PointerType : 0) {
		  _LL245: _temp244 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp238)->f1;
			  goto _LL241;
		}
		else {
		  goto _LL242;
		}
		_LL242: goto _LL243;
		_LL241: t = ({ void **_temp246 = topt;
			  if (_temp246 == 0) {
			    _throw(Null_Exception);
			  }
			  *_temp246; });
			goto tcpat_end;
		_LL243: goto _LL239;
		_LL239: ;
	      }
	      t = (void *)({ struct Cyc_Absyn_PointerType_struct *_temp247 = (struct Cyc_Absyn_PointerType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_PointerType_struct));
		_temp247[0] = ({ struct Cyc_Absyn_PointerType_struct _temp248;
		  _temp248.tag = Cyc_Absyn_PointerType;
		  _temp248.f1 = ({ struct Cyc_Absyn_PtrInfo _temp249;
		    _temp249.elt_typ = (void *)Cyc_Absyn_new_evar((void *)Cyc_Absyn_AnyKind);
		    _temp249.rgn_typ = (void *)Cyc_Absyn_new_evar((void *)Cyc_Absyn_RgnKind);
		    _temp249.nullable = ((struct Cyc_Absyn_Conref *(*)(int x))Cyc_Absyn_new_conref)(1);
		    _temp249.tq = Cyc_Absyn_empty_tqual();
		    _temp249.bounds = ((struct Cyc_Absyn_Conref *(*)())Cyc_Absyn_empty_conref)();
		    _temp249; });
		  _temp248; });
		_temp247; });
	      goto _LL141;
      _LL161: {
      void *inner_typ = (void *)Cyc_Absyn_VoidType;
	void **inner_topt = 0;
	if (topt != 0) {
	  void *_temp251 = Cyc_Tcutil_compress(({ void **_temp250 = topt;
						 if (_temp250 == 0) {
						   _throw(Null_Exception);
						 }
						 *_temp250; }));
	  struct Cyc_Absyn_PtrInfo _temp257;
	  struct Cyc_Absyn_Conref *_temp259;
	  struct Cyc_Absyn_Tqual _temp261;
	  struct Cyc_Absyn_Conref *_temp263;
	  void *_temp265;
	  void *_temp267;
	  _LL253: if ((unsigned int)_temp251 > 4 ? *((int *)_temp251) == Cyc_Absyn_PointerType : 0) {
	    _LL258: _temp257 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp251)->f1;
		    _LL268: _temp267 = (void *)_temp257.elt_typ;
			    goto _LL266;
		    _LL266: _temp265 = (void *)_temp257.rgn_typ;
			    goto _LL264;
		    _LL264: _temp263 = (struct Cyc_Absyn_Conref *)_temp257.nullable;
			    goto _LL262;
		    _LL262: _temp261 = (struct Cyc_Absyn_Tqual)_temp257.tq;
			    goto _LL260;
		    _LL260: _temp259 = (struct Cyc_Absyn_Conref *)_temp257.bounds;
			    goto _LL254;
	  }
	  else {
	    goto _LL255;
	  }
	  _LL255: goto _LL256;
	  _LL254: inner_typ = _temp267;
		  inner_topt = (void **)&inner_typ;
		  goto _LL252;
	  _LL256: goto _LL252;
	  _LL252: ;
	}
	{ void *ptr_rgn = Cyc_Absyn_new_evar((void *)Cyc_Absyn_RgnKind);
	  struct Cyc_List_List *_temp271;
	  struct Cyc_List_List *_temp273;
	  struct _tuple5 _temp269 = Cyc_Tcpat_tcPatRec(te,_temp194,inner_topt,
						       (void **)&ptr_rgn);
	  _LL274: _temp273 = _temp269.f1;
		  goto _LL272;
	  _LL272: _temp271 = _temp269.f2;
		  goto _LL270;
	  _LL270: tv_result = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
							  struct Cyc_List_List *y))Cyc_List_append)(tv_result,
												    _temp273);
		  v_result = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
							 struct Cyc_List_List *y))Cyc_List_append)(v_result,
												   _temp271);
		  if (inner_topt == 0) {
		    void *_temp276 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp275 = _temp194->topt;
							   if (_temp275 == 0) {
							     _throw(Null_Exception);
							   }
							   _temp275->v; }));
		    struct Cyc_Absyn_TunionFieldInfo _temp282;
		    struct Cyc_List_List *_temp284;
		    void *_temp286;
		    struct Cyc_Absyn_Tunionfield *_temp288;
		    struct Cyc_Absyn_Tuniondecl *_temp290;
		    _LL278: if ((unsigned int)_temp276 > 4 ? *((int *)_temp276) == Cyc_Absyn_TunionFieldType : 0) {
		      _LL283: _temp282 = (struct Cyc_Absyn_TunionFieldInfo)((struct Cyc_Absyn_TunionFieldType_struct *)_temp276)->f1;
			      _LL287: _temp286 = (void *)_temp282.field_info;
				      if (*((int *)_temp286) == Cyc_Absyn_KnownTunionfield) {
					_LL291: _temp290 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_KnownTunionfield_struct *)_temp286)->f1;
						goto _LL289;
					_LL289: _temp288 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Absyn_KnownTunionfield_struct *)_temp286)->f2;
						goto _LL285;
				      }
				      else {
					goto _LL280;
				      }
			      _LL285: _temp284 = (struct Cyc_List_List *)_temp282.targs;
				      goto _LL279;
		    }
		    else {
		      goto _LL280;
		    }
		    _LL280: goto _LL281;
		    _LL279: t = (void *)({ struct Cyc_Absyn_TunionType_struct *_temp292 = (struct Cyc_Absyn_TunionType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_TunionType_struct));
			      _temp292[0] = ({ struct Cyc_Absyn_TunionType_struct _temp293;
				_temp293.tag = Cyc_Absyn_TunionType;
				_temp293.f1 = ({ struct Cyc_Absyn_TunionInfo _temp294;
				  _temp294.tunion_info = (void *)((void *)({ struct Cyc_Absyn_KnownTunion_struct *_temp295 = (struct Cyc_Absyn_KnownTunion_struct *)GC_malloc(sizeof(struct Cyc_Absyn_KnownTunion_struct));
				    _temp295[0] = ({ struct Cyc_Absyn_KnownTunion_struct _temp296;
				      _temp296.tag = Cyc_Absyn_KnownTunion;
				      _temp296.f1 = _temp290;
				      _temp296; });
				    _temp295; }));
				  _temp294.targs = _temp284;
				  _temp294.rgn = (void *)ptr_rgn;
				  _temp294; });
				_temp293; });
			      _temp292; });
			    goto _LL277;
		    _LL281: t = (void *)({ struct Cyc_Absyn_PointerType_struct *_temp297 = (struct Cyc_Absyn_PointerType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_PointerType_struct));
			      _temp297[0] = ({ struct Cyc_Absyn_PointerType_struct _temp298;
				_temp298.tag = Cyc_Absyn_PointerType;
				_temp298.f1 = ({ struct Cyc_Absyn_PtrInfo _temp299;
				  _temp299.elt_typ = (void *)((void *)({ struct Cyc_Core_Opt *_temp300 = _temp194->topt;
				    if (_temp300 == 0) {
				      _throw(Null_Exception);
				    }
				    _temp300->v; }));
				  _temp299.rgn_typ = (void *)ptr_rgn;
				  _temp299.nullable = ((struct Cyc_Absyn_Conref *(*)())Cyc_Absyn_empty_conref)();
				  _temp299.tq = Cyc_Absyn_empty_tqual();
				  _temp299.bounds = ((struct Cyc_Absyn_Conref *(*)())Cyc_Absyn_empty_conref)();
				  _temp299; });
				_temp298; });
			      _temp297; });
			    goto _LL277;
		    _LL277: ;
		  }
		  else {
		    t = (void *)({ struct Cyc_Absyn_PointerType_struct *_temp301 = (struct Cyc_Absyn_PointerType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_PointerType_struct));
		      _temp301[0] = ({ struct Cyc_Absyn_PointerType_struct _temp302;
			_temp302.tag = Cyc_Absyn_PointerType;
			_temp302.f1 = ({ struct Cyc_Absyn_PtrInfo _temp303;
			  _temp303.elt_typ = (void *)((void *)({ struct Cyc_Core_Opt *_temp304 = _temp194->topt;
			    if (_temp304 == 0) {
			      _throw(Null_Exception);
			    }
			    _temp304->v; }));
			  _temp303.rgn_typ = (void *)ptr_rgn;
			  _temp303.nullable = ((struct Cyc_Absyn_Conref *(*)())Cyc_Absyn_empty_conref)();
			  _temp303.tq = Cyc_Absyn_empty_tqual();
			  _temp303.bounds = ((struct Cyc_Absyn_Conref *(*)())Cyc_Absyn_empty_conref)();
			  _temp303; });
			_temp302; });
		      _temp301; });
		  }
		  goto _LL141;}
	
      }
      _LL163: {
      struct Cyc_List_List *pat_ts = 0;
	struct Cyc_List_List *topt_ts = 0;
	if (topt != 0) {
	  void *_temp306 = Cyc_Tcutil_compress(({ void **_temp305 = topt;
						 if (_temp305 == 0) {
						   _throw(Null_Exception);
						 }
						 *_temp305; }));
	  struct Cyc_List_List *_temp312;
	  _LL308: if ((unsigned int)_temp306 > 4 ? *((int *)_temp306) == Cyc_Absyn_TupleType : 0) {
	    _LL313: _temp312 = (struct Cyc_List_List *)((struct Cyc_Absyn_TupleType_struct *)_temp306)->f1;
		    goto _LL309;
	  }
	  else {
	    goto _LL310;
	  }
	  _LL310: goto _LL311;
	  _LL309: topt_ts = _temp312;
		  goto _LL307;
	  _LL311: goto _LL307;
	  _LL307: ;
	}
	for(0; _temp196 != 0; _temp196 = ({ struct Cyc_List_List *_temp314 = _temp196;
	  if (_temp314 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp314->tl; })) {
	  void **inner_topt = 0;
	  if (topt_ts != 0) {
	    inner_topt = (void **)&(*((struct _tuple7 *)({ struct Cyc_List_List *_temp315 = topt_ts;
	      if (_temp315 == 0) {
		_throw(Null_Exception);
	      }
	      _temp315->hd; }))).f2;
	    topt_ts = ({ struct Cyc_List_List *_temp316 = topt_ts;
	      if (_temp316 == 0) {
		_throw(Null_Exception);
	      }
	      _temp316->tl; });
	  }
	  { struct Cyc_List_List *_temp320;
	    struct Cyc_List_List *_temp322;
	    struct _tuple5 _temp318 = Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat *)({ struct Cyc_List_List *_temp317 = _temp196;
							   if (_temp317 == 0) {
							     _throw(Null_Exception);
							   }
							   _temp317->hd; }),
							 inner_topt,region_opt);
	    _LL323: _temp322 = _temp318.f1;
		    goto _LL321;
	    _LL321: _temp320 = _temp318.f2;
		    goto _LL319;
	    _LL319: tv_result = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
							    struct Cyc_List_List *y))Cyc_List_append)(tv_result,
												      _temp322);
		    v_result = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
							   struct Cyc_List_List *y))Cyc_List_append)(v_result,
												     _temp320);
		    pat_ts = ({ struct Cyc_List_List *_temp324 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
		      _temp324->hd = (void *)({ struct _tuple7 *_temp325 = (struct _tuple7 *)GC_malloc(sizeof(struct _tuple7));
			_temp325->f1 = Cyc_Absyn_empty_tqual();
			_temp325->f2 = (void *)({ struct Cyc_Core_Opt *_temp327 = ((struct Cyc_Absyn_Pat *)({ struct Cyc_List_List *_temp326 = _temp196;
			    if (_temp326 == 0) {
			      _throw(Null_Exception);
			    }
			    _temp326->hd; }))->topt;
			  if (_temp327 == 0) {
			    _throw(Null_Exception);
			  }
			  _temp327->v; });
			_temp325; });
		      _temp324->tl = pat_ts;
		      _temp324; });}
	  
	}
	t = (void *)({ struct Cyc_Absyn_TupleType_struct *_temp328 = (struct Cyc_Absyn_TupleType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_TupleType_struct));
	  _temp328[0] = ({ struct Cyc_Absyn_TupleType_struct _temp329;
	    _temp329.tag = Cyc_Absyn_TupleType;
	    _temp329.f1 = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x))Cyc_List_imp_rev)(pat_ts);
	    _temp329; });
	  _temp328; });
	goto _LL141;
      }
      _LL165: if (_temp200 != 0) {
		(void)_throw(((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp330 = (char *)"tcPat: struct<...> not implemented";
										    struct _tagged_string _temp331;
										    _temp331.curr = _temp330;
										    _temp331.base = _temp330;
										    _temp331.last_plus_one = _temp330 + 35;
										    _temp331; })));
	      }
	      { struct _RegionHandle _temp332 = _new_region();
		struct _RegionHandle *rgn = &_temp332;
		_push_region(rgn);
		{ struct Cyc_List_List *inst = ((struct Cyc_List_List *(*)(struct _RegionHandle *,
									   struct _tuple4 *(*f)(struct _RegionHandle *,
												struct Cyc_Absyn_Tvar *),
									   struct _RegionHandle *env,
									   struct Cyc_List_List *x))Cyc_List_rmap_c)(rgn,
														     Cyc_Tcutil_r_make_inst_var,
														     rgn,
														     _temp205->tvs);
		  struct Cyc_List_List *all_typs = ((struct Cyc_List_List *(*)(void *(*f)(struct _tuple4 *),
									       struct Cyc_List_List *x))Cyc_List_map)((void *(*)(struct _tuple4 *))Cyc_Core_snd,
														      inst);
		  t = (void *)({ struct Cyc_Absyn_StructType_struct *_temp333 = (struct Cyc_Absyn_StructType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_StructType_struct));
		    _temp333[0] = ({ struct Cyc_Absyn_StructType_struct _temp334;
		      _temp334.tag = Cyc_Absyn_StructType;
		      _temp334.f1 = (struct _tuple1 *)((struct _tuple1 *)({ struct Cyc_Core_Opt *_temp335 = _temp205->name;
			if (_temp335 == 0) {
			  _throw(Null_Exception);
			}
			_temp335->v; }));
		      _temp334.f2 = all_typs;
		      _temp334.f3 = ({ struct Cyc_Absyn_Structdecl **_temp336 = (struct Cyc_Absyn_Structdecl **)GC_malloc(sizeof(struct Cyc_Absyn_Structdecl *));
			_temp336[0] = _temp205;
			_temp336; });
		      _temp334; });
		    _temp333; });
		  if (topt != 0) {
		    Cyc_Tcutil_unify(t,({ void **_temp337 = topt;
				       if (_temp337 == 0) {
					 _throw(Null_Exception);
				       }
				       *_temp337; }));
		  }
		  *_temp204 = ({ struct Cyc_Core_Opt *_temp338 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
		    _temp338->v = (void *)all_typs;
		    _temp338; });
		  if (_temp205->fields == 0) {
		    Cyc_Tcutil_terr(p->loc,(struct _tagged_string)({ char *_temp339 = (char *)"can't destructure an abstract struct";
				      struct _tagged_string _temp340;
				      _temp340.curr = _temp339;
				      _temp340.base = _temp339;
				      _temp340.last_plus_one = _temp339 + 37;
				      _temp340; }));
		    _npop_handler(0);
		    goto _LL141;
		  }
		  { struct Cyc_List_List *fields = ((struct Cyc_List_List *(*)(struct _RegionHandle *rgn,
									       struct Cyc_Position_Segment *loc,
									       struct Cyc_List_List *des,
									       struct Cyc_List_List *fields))Cyc_Tcutil_resolve_struct_designators)(rgn,
																		    p->loc,
																		    _temp198,
																		    (struct Cyc_List_List *)({ struct Cyc_Core_Opt *_temp361 = _temp205->fields;
																		      if (_temp361 == 0) {
																			_throw(Null_Exception);
																		      }
																		      _temp361->v; }));
		    for(0; fields != 0; fields = ({ struct Cyc_List_List *_temp341 = fields;
		      if (_temp341 == 0) {
			_throw(Null_Exception);
		      }
		      _temp341->tl; })) {
		      struct _tuple8 _temp345;
		      struct Cyc_Absyn_Pat *_temp346;
		      struct Cyc_Absyn_Structfield *_temp348;
		      struct _tuple8 *_temp343 = (struct _tuple8 *)({ struct Cyc_List_List *_temp342 = fields;
			if (_temp342 == 0) {
			  _throw(Null_Exception);
			}
			_temp342->hd; });
		      _temp345 = *_temp343;
		      _LL349: _temp348 = _temp345.f1;
			      goto _LL347;
		      _LL347: _temp346 = _temp345.f2;
			      goto _LL344;
		      _LL344: {
		      void *inst_fieldtyp = Cyc_Tcutil_rsubstitute(rgn,inst,
								   (void *)_temp348->type);
			struct Cyc_List_List *_temp352;
			struct Cyc_List_List *_temp354;
			struct _tuple5 _temp350 = Cyc_Tcpat_tcPatRec(te,_temp346,
								     (void **)&inst_fieldtyp,
								     region_opt);
			_LL355: _temp354 = _temp350.f1;
				goto _LL353;
			_LL353: _temp352 = _temp350.f2;
				goto _LL351;
			_LL351: tv_result = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
									struct Cyc_List_List *y))Cyc_List_append)(tv_result,
														  _temp354);
				v_result = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
								       struct Cyc_List_List *y))Cyc_List_append)(v_result,
														 _temp352);
				if (! Cyc_Tcutil_unify((void *)({ struct Cyc_Core_Opt *_temp356 = _temp346->topt;
							 if (_temp356 == 0) {
							   _throw(Null_Exception);
							 }
							 _temp356->v; }),
						       inst_fieldtyp)) {
				  Cyc_Tcutil_terr(p->loc,({ struct _tagged_string _temp358 = *_temp348->name;
						    struct _tagged_string _temp359 = Cyc_Absynpp_typ2string(inst_fieldtyp);
						    struct _tagged_string _temp360 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp357 = _temp346->topt;
													      if (_temp357 == 0) {
														_throw(Null_Exception);
													      }
													      _temp357->v; }));
						    xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
							    _temp358.last_plus_one - _temp358.curr,
							    _temp358.curr,
							    _temp359.last_plus_one - _temp359.curr,
							    _temp359.curr,
							    _temp360.last_plus_one - _temp360.curr,
							    _temp360.curr); }));
				}
		      }
		    }}
		  
		}
		;
		_pop_region(&_temp332);
	      }
	      goto _LL141;
      _LL167: if (((int (*)(struct Cyc_List_List *x))Cyc_List_length)(_temp209) != ((int (*)(struct Cyc_List_List *x))Cyc_List_length)(_temp211->tvs)) {
		Cyc_Tcutil_terr(p->loc,(struct _tagged_string)({ char *_temp362 = (char *)"wrong number of existential type variables in pattern";
				  struct _tagged_string _temp363;
				  _temp363.curr = _temp362;
				  _temp363.base = _temp362;
				  _temp363.last_plus_one = _temp362 + 54;
				  _temp363; }));
	      }
	      { struct _RegionHandle _temp364 = _new_region();
		struct _RegionHandle *rgn = &_temp364;
		_push_region(rgn);
		{ struct Cyc_List_List *var_tvs = 0;
		  { struct Cyc_List_List *t = _temp209;
		    for(0; t != 0; t = ({ struct Cyc_List_List *_temp365 = t;
		      if (_temp365 == 0) {
			_throw(Null_Exception);
		      }
		      _temp365->tl; })) {
		      struct Cyc_Absyn_Tvar *tv = (struct Cyc_Absyn_Tvar *)({ struct Cyc_List_List *_temp392 = t;
			if (_temp392 == 0) {
			  _throw(Null_Exception);
			}
			_temp392->hd; });
		      { void *_temp366 = (void *)(((struct Cyc_Absyn_Conref *(*)(struct Cyc_Absyn_Conref *x))Cyc_Absyn_compress_conref)(tv->kind))->v;
			void *_temp376;
			void *_temp378;
			struct Cyc_Absyn_Conref *_temp380;
			_LL368: if ((unsigned int)_temp366 > 1 ? *((int *)_temp366) == Cyc_Absyn_Eq_constr : 0) {
			  _LL377: _temp376 = (void *)((struct Cyc_Absyn_Eq_constr_struct *)_temp366)->f1;
				  if (_temp376 == (void *)Cyc_Absyn_BoxKind) {
				    goto _LL369;
				  }
				  else {
				    goto _LL370;
				  }
			}
			else {
			  goto _LL370;
			}
			_LL370: if ((unsigned int)_temp366 > 1 ? *((int *)_temp366) == Cyc_Absyn_Eq_constr : 0) {
			  _LL379: _temp378 = (void *)((struct Cyc_Absyn_Eq_constr_struct *)_temp366)->f1;
				  goto _LL371;
			}
			else {
			  goto _LL372;
			}
			_LL372: if (_temp366 == (void *)Cyc_Absyn_No_constr) {
			  goto _LL373;
			}
			else {
			  goto _LL374;
			}
			_LL374: if ((unsigned int)_temp366 > 1 ? *((int *)_temp366) == Cyc_Absyn_Forward_constr : 0) {
			  _LL381: _temp380 = (struct Cyc_Absyn_Conref *)((struct Cyc_Absyn_Forward_constr_struct *)_temp366)->f1;
				  goto _LL375;
			}
			else {
			  goto _LL367;
			}
			_LL369: goto _LL367;
			_LL371: Cyc_Tcutil_terr(p->loc,({ struct _tagged_string _temp382 = Cyc_Absynpp_qvar2string(_temp213->name);
						  struct _tagged_string _temp383 = *tv->name;
						  struct _tagged_string _temp384 = Cyc_Absynpp_kind2string(_temp378);
						  xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
							  _temp382.last_plus_one - _temp382.curr,
							  _temp382.curr,_temp383.last_plus_one - _temp383.curr,
							  _temp383.curr,_temp384.last_plus_one - _temp384.curr,
							  _temp384.curr); }));
				goto _LL367;
			_LL373: (void *)((tv->kind)->v = (void *)((void *)({ struct Cyc_Absyn_Eq_constr_struct *_temp385 = (struct Cyc_Absyn_Eq_constr_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Eq_constr_struct));
				  _temp385[0] = ({ struct Cyc_Absyn_Eq_constr_struct _temp386;
				    _temp386.tag = Cyc_Absyn_Eq_constr;
				    _temp386.f1 = (void *)((void *)Cyc_Absyn_BoxKind);
				    _temp386; });
				  _temp385; })));
				goto _LL367;
			_LL375: (void)_throw(((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp387 = (char *)"forward after compress in conref";
												    struct _tagged_string _temp388;
												    _temp388.curr = _temp387;
												    _temp388.base = _temp387;
												    _temp388.last_plus_one = _temp387 + 33;
												    _temp388; })));
			_LL367: ;
		      }
		      var_tvs = ({ struct Cyc_List_List *_temp389 = (struct Cyc_List_List *)_region_malloc(rgn,
													   sizeof(struct Cyc_List_List));
			_temp389->hd = (void *)((void *)({ struct Cyc_Absyn_VarType_struct *_temp390 = (struct Cyc_Absyn_VarType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_VarType_struct));
			  _temp390[0] = ({ struct Cyc_Absyn_VarType_struct _temp391;
			    _temp391.tag = Cyc_Absyn_VarType;
			    _temp391.f1 = tv;
			    _temp391; });
			  _temp390; }));
			_temp389->tl = var_tvs;
			_temp389; });
		    }
		  }
		  var_tvs = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x))Cyc_List_imp_rev)(var_tvs);
		  tv_result = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
							  struct Cyc_List_List *y))Cyc_List_append)(tv_result,
												    _temp209);
		  { struct Cyc_Tcenv_Tenv *te2 = Cyc_Tcenv_add_type_vars(p->loc,
									 te,
									 _temp209);
		    struct Cyc_List_List *tqts = _temp211->typs;
		    struct Cyc_List_List *all_inst = ((struct Cyc_List_List *(*)(struct _RegionHandle *,
										 struct _tuple4 *(*f)(struct _RegionHandle *,
												      struct Cyc_Absyn_Tvar *),
										 struct _RegionHandle *env,
										 struct Cyc_List_List *x))Cyc_List_rmap_c)(rgn,
															   Cyc_Tcutil_r_make_inst_var,
															   rgn,
															   _temp213->tvs);
		    struct Cyc_List_List *exist_inst = ((struct Cyc_List_List *(*)(struct _RegionHandle *r3,
										   struct _RegionHandle *r4,
										   struct Cyc_List_List *x,
										   struct Cyc_List_List *y))Cyc_List_rzip)(rgn,
															   rgn,
															   _temp211->tvs,
															   var_tvs);
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
		    if (tqts == 0) {
		      t = (void *)({ struct Cyc_Absyn_TunionType_struct *_temp393 = (struct Cyc_Absyn_TunionType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_TunionType_struct));
			_temp393[0] = ({ struct Cyc_Absyn_TunionType_struct _temp394;
			  _temp394.tag = Cyc_Absyn_TunionType;
			  _temp394.f1 = ({ struct Cyc_Absyn_TunionInfo _temp395;
			    _temp395.tunion_info = (void *)((void *)({ struct Cyc_Absyn_KnownTunion_struct *_temp396 = (struct Cyc_Absyn_KnownTunion_struct *)GC_malloc(sizeof(struct Cyc_Absyn_KnownTunion_struct));
			      _temp396[0] = ({ struct Cyc_Absyn_KnownTunion_struct _temp397;
				_temp397.tag = Cyc_Absyn_KnownTunion;
				_temp397.f1 = _temp213;
				_temp397; });
			      _temp396; }));
			    _temp395.targs = all_typs;
			    _temp395.rgn = (void *)Cyc_Absyn_new_evar((void *)Cyc_Absyn_RgnKind);
			    _temp395; });
			  _temp394; });
			_temp393; });
		    }
		    else {
		      t = (void *)({ struct Cyc_Absyn_TunionFieldType_struct *_temp398 = (struct Cyc_Absyn_TunionFieldType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_TunionFieldType_struct));
			_temp398[0] = ({ struct Cyc_Absyn_TunionFieldType_struct _temp399;
			  _temp399.tag = Cyc_Absyn_TunionFieldType;
			  _temp399.f1 = ({ struct Cyc_Absyn_TunionFieldInfo _temp400;
			    _temp400.field_info = (void *)((void *)({ struct Cyc_Absyn_KnownTunionfield_struct *_temp401 = (struct Cyc_Absyn_KnownTunionfield_struct *)GC_malloc(sizeof(struct Cyc_Absyn_KnownTunionfield_struct));
			      _temp401[0] = ({ struct Cyc_Absyn_KnownTunionfield_struct _temp402;
				_temp402.tag = Cyc_Absyn_KnownTunionfield;
				_temp402.f1 = _temp213;
				_temp402.f2 = _temp211;
				_temp402; });
			      _temp401; }));
			    _temp400.targs = all_typs;
			    _temp400; });
			  _temp399; });
			_temp398; });
		    }
		    if (topt != 0 ? tqts == 0 : 0) {
		      void *_temp404 = Cyc_Tcutil_compress(({ void **_temp403 = topt;
							     if (_temp403 == 0) {
							       _throw(Null_Exception);
							     }
							     *_temp403; }));
		      struct Cyc_Absyn_TunionFieldInfo _temp412;
		      struct Cyc_Absyn_TunionInfo _temp414;
		      void *_temp416;
		      struct Cyc_List_List *_temp418;
		      void *_temp420;
		      _LL406: if ((unsigned int)_temp404 > 4 ? *((int *)_temp404) == Cyc_Absyn_TunionFieldType : 0) {
			_LL413: _temp412 = (struct Cyc_Absyn_TunionFieldInfo)((struct Cyc_Absyn_TunionFieldType_struct *)_temp404)->f1;
				goto _LL407;
		      }
		      else {
			goto _LL408;
		      }
		      _LL408: if ((unsigned int)_temp404 > 4 ? *((int *)_temp404) == Cyc_Absyn_TunionType : 0) {
			_LL415: _temp414 = (struct Cyc_Absyn_TunionInfo)((struct Cyc_Absyn_TunionType_struct *)_temp404)->f1;
				_LL421: _temp420 = (void *)_temp414.tunion_info;
					goto _LL419;
				_LL419: _temp418 = (struct Cyc_List_List *)_temp414.targs;
					goto _LL417;
				_LL417: _temp416 = (void *)_temp414.rgn;
					goto _LL409;
		      }
		      else {
			goto _LL410;
		      }
		      _LL410: goto _LL411;
		      _LL407: t = (void *)({ struct Cyc_Absyn_TunionFieldType_struct *_temp422 = (struct Cyc_Absyn_TunionFieldType_struct *)GC_malloc(sizeof(struct Cyc_Absyn_TunionFieldType_struct));
				_temp422[0] = ({ struct Cyc_Absyn_TunionFieldType_struct _temp423;
				  _temp423.tag = Cyc_Absyn_TunionFieldType;
				  _temp423.f1 = ({ struct Cyc_Absyn_TunionFieldInfo _temp424;
				    _temp424.field_info = (void *)((void *)({ struct Cyc_Absyn_KnownTunionfield_struct *_temp425 = (struct Cyc_Absyn_KnownTunionfield_struct *)GC_malloc(sizeof(struct Cyc_Absyn_KnownTunionfield_struct));
				      _temp425[0] = ({ struct Cyc_Absyn_KnownTunionfield_struct _temp426;
					_temp426.tag = Cyc_Absyn_KnownTunionfield;
					_temp426.f1 = _temp213;
					_temp426.f2 = _temp211;
					_temp426; });
				      _temp425; }));
				    _temp424.targs = all_typs;
				    _temp424; });
				  _temp423; });
				_temp422; });
			      goto _LL405;
		      _LL409: {
		      struct Cyc_List_List *a = all_typs;
			for(0; a != 0 ? _temp418 != 0 : 0; a = ({ struct Cyc_List_List *_temp427 = a;
			  if (_temp427 == 0) {
			    _throw(Null_Exception);
			  }
			  _temp427->tl; }), _temp418 = ({ struct Cyc_List_List *_temp428 = _temp418;
			  if (_temp428 == 0) {
			    _throw(Null_Exception);
			  }
			  _temp428->tl; })) {
			  Cyc_Tcutil_unify((void *)({ struct Cyc_List_List *_temp429 = a;
					     if (_temp429 == 0) {
					       _throw(Null_Exception);
					     }
					     _temp429->hd; }),(void *)({ struct Cyc_List_List *_temp430 = _temp418;
					     if (_temp430 == 0) {
					       _throw(Null_Exception);
					     }
					     _temp430->hd; }));
			}
			goto _LL405;
		      }
		      _LL411: goto _LL405;
		      _LL405: ;
		    }
		    for(0; _temp207 != 0 ? tqts != 0 : 0; _temp207 = ({ struct Cyc_List_List *_temp431 = _temp207;
		      if (_temp431 == 0) {
			_throw(Null_Exception);
		      }
		      _temp431->tl; }), tqts = ({ struct Cyc_List_List *_temp432 = tqts;
		      if (_temp432 == 0) {
			_throw(Null_Exception);
		      }
		      _temp432->tl; })) {
		      struct Cyc_Absyn_Pat *p2 = (struct Cyc_Absyn_Pat *)({ struct Cyc_List_List *_temp445 = _temp207;
			if (_temp445 == 0) {
			  _throw(Null_Exception);
			}
			_temp445->hd; });
		      void *field_typ = Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple7 *)({ struct Cyc_List_List *_temp444 = tqts;
								 if (_temp444 == 0) {
								   _throw(Null_Exception);
								 }
								 _temp444->hd; }))).f2);
		      struct Cyc_List_List *_temp435;
		      struct Cyc_List_List *_temp437;
		      struct _tuple5 _temp433 = Cyc_Tcpat_tcPatRec(te2,p2,
								   (void **)&field_typ,
								   region_opt);
		      _LL438: _temp437 = _temp433.f1;
			      goto _LL436;
		      _LL436: _temp435 = _temp433.f2;
			      goto _LL434;
		      _LL434: tv_result = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
								      struct Cyc_List_List *y))Cyc_List_append)(tv_result,
														_temp437);
			      v_result = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
								     struct Cyc_List_List *y))Cyc_List_append)(v_result,
													       _temp435);
			      if (! Cyc_Tcutil_unify((void *)({ struct Cyc_Core_Opt *_temp439 = p2->topt;
						       if (_temp439 == 0) {
							 _throw(Null_Exception);
						       }
						       _temp439->v; }),field_typ)) {
				Cyc_Tcutil_terr(p2->loc,({ struct _tagged_string _temp441 = Cyc_Absynpp_qvar2string(_temp213->name);
						  struct _tagged_string _temp442 = Cyc_Absynpp_typ2string(field_typ);
						  struct _tagged_string _temp443 = Cyc_Absynpp_typ2string((void *)({ struct Cyc_Core_Opt *_temp440 = p2->topt;
													    if (_temp440 == 0) {
													      _throw(Null_Exception);
													    }
													    _temp440->v; }));
						  xprintf("%.*s expects argument type %.*s, not %.*s",
							  _temp441.last_plus_one - _temp441.curr,
							  _temp441.curr,_temp442.last_plus_one - _temp442.curr,
							  _temp442.curr,_temp443.last_plus_one - _temp443.curr,
							  _temp443.curr); }));
			      }
		    }
		    if (_temp207 != 0) {
		      Cyc_Tcutil_terr(p->loc,({ struct _tagged_string _temp446 = Cyc_Absynpp_qvar2string(_temp213->name);
					xprintf("too many arguments for tunion constructor %.*s",
						_temp446.last_plus_one - _temp446.curr,
						_temp446.curr); }));
		    }
		    if (tqts != 0) {
		      Cyc_Tcutil_terr(p->loc,({ struct _tagged_string _temp447 = Cyc_Absynpp_qvar2string(_temp213->name);
					xprintf("too few arguments for tunion constructor %.*s",
						_temp447.last_plus_one - _temp447.curr,
						_temp447.curr); }));
		    }}
		  
		}
		;
		_pop_region(&_temp364);
	      }
	      goto _LL141;
      _LL169: goto _LL171;
      _LL171: goto _LL173;
      _LL173: t = Cyc_Absyn_wildtyp();
	      goto _LL141;
      _LL141: ;
    }
    tcpat_end: p->topt = ({ struct Cyc_Core_Opt *_temp448 = (struct Cyc_Core_Opt *)GC_malloc(sizeof(struct Cyc_Core_Opt));
      _temp448->v = (void *)t;
      _temp448; });
    return ({ struct _tuple5 _temp449;
      _temp449.f1 = tv_result;
      _temp449.f2 = v_result;
      _temp449; });}
  
}
struct _tuple5 Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Pat *p,
			       void **topt,void **region_opt) {
  struct _tuple5 ans = Cyc_Tcpat_tcPatRec(te,p,topt,region_opt);
  { struct _RegionHandle _temp450 = _new_region();
    struct _RegionHandle *r = &_temp450;
    _push_region(r);
    Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List *(*)(struct _RegionHandle *,
							     struct _tagged_string *(*f)(struct Cyc_Absyn_Vardecl *),
							     struct Cyc_List_List *x))Cyc_List_rmap)(r,
												     Cyc_Tcpat_get_name,
												     ans.f2),
				 p->loc,(struct _tagged_string)({ char *_temp451 = (char *)"pattern contains a repeated variable";
				   struct _tagged_string _temp452;
				   _temp452.curr = _temp451;
				   _temp452.base = _temp451;
				   _temp452.last_plus_one = _temp451 + 37;
				   _temp452; }));
    ;
    _pop_region(&_temp450);
  }
  return ans;
}
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv *te,struct Cyc_Absyn_Pat *p) {
  void *_temp453 = (void *)p->r;
  struct Cyc_Absyn_Pat *_temp465;
  struct Cyc_List_List *_temp467;
  struct Cyc_List_List *_temp469;
  struct Cyc_Absyn_Tunionfield *_temp471;
  struct Cyc_Absyn_Tuniondecl *_temp473;
  struct Cyc_List_List *_temp475;
  struct Cyc_List_List *_temp477;
  struct Cyc_Core_Opt *_temp479;
  struct Cyc_Absyn_Structdecl *_temp481;
  struct Cyc_List_List *_temp483;
  _LL455: if ((unsigned int)_temp453 > 2 ? *((int *)_temp453) == Cyc_Absyn_Pointer_p : 0) {
    _LL466: _temp465 = (struct Cyc_Absyn_Pat *)((struct Cyc_Absyn_Pointer_p_struct *)_temp453)->f1;
	    goto _LL456;
  }
  else {
    goto _LL457;
  }
  _LL457: if ((unsigned int)_temp453 > 2 ? *((int *)_temp453) == Cyc_Absyn_Tunion_p : 0) {
    _LL474: _temp473 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_Tunion_p_struct *)_temp453)->f1;
	    goto _LL472;
    _LL472: _temp471 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Absyn_Tunion_p_struct *)_temp453)->f2;
	    goto _LL470;
    _LL470: _temp469 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_p_struct *)_temp453)->f3;
	    goto _LL468;
    _LL468: _temp467 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_p_struct *)_temp453)->f4;
	    goto _LL458;
  }
  else {
    goto _LL459;
  }
  _LL459: if ((unsigned int)_temp453 > 2 ? *((int *)_temp453) == Cyc_Absyn_Struct_p : 0) {
    _LL482: _temp481 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Absyn_Struct_p_struct *)_temp453)->f1;
	    goto _LL480;
    _LL480: _temp479 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Struct_p_struct *)_temp453)->f2;
	    goto _LL478;
    _LL478: _temp477 = (struct Cyc_List_List *)((struct Cyc_Absyn_Struct_p_struct *)_temp453)->f3;
	    goto _LL476;
    _LL476: _temp475 = (struct Cyc_List_List *)((struct Cyc_Absyn_Struct_p_struct *)_temp453)->f4;
	    goto _LL460;
  }
  else {
    goto _LL461;
  }
  _LL461: if ((unsigned int)_temp453 > 2 ? *((int *)_temp453) == Cyc_Absyn_Tuple_p : 0) {
    _LL484: _temp483 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tuple_p_struct *)_temp453)->f1;
	    goto _LL462;
  }
  else {
    goto _LL463;
  }
  _LL463: goto _LL464;
  _LL456: Cyc_Tcpat_check_pat_regions(te,_temp465);
  { void *_temp486 = (void *)({ struct Cyc_Core_Opt *_temp485 = p->topt;
      if (_temp485 == 0) {
	_throw(Null_Exception);
      }
      _temp485->v; });
    struct Cyc_Absyn_PtrInfo _temp494;
    struct Cyc_Absyn_Conref *_temp496;
    struct Cyc_Absyn_Tqual _temp498;
    struct Cyc_Absyn_Conref *_temp500;
    void *_temp502;
    void *_temp504;
    struct Cyc_Absyn_TunionInfo _temp506;
    void *_temp508;
    struct Cyc_List_List *_temp510;
    void *_temp512;
    _LL488: if ((unsigned int)_temp486 > 4 ? *((int *)_temp486) == Cyc_Absyn_PointerType : 0) {
      _LL495: _temp494 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp486)->f1;
	      _LL505: _temp504 = (void *)_temp494.elt_typ;
		      goto _LL503;
	      _LL503: _temp502 = (void *)_temp494.rgn_typ;
		      goto _LL501;
	      _LL501: _temp500 = (struct Cyc_Absyn_Conref *)_temp494.nullable;
		      goto _LL499;
	      _LL499: _temp498 = (struct Cyc_Absyn_Tqual)_temp494.tq;
		      goto _LL497;
	      _LL497: _temp496 = (struct Cyc_Absyn_Conref *)_temp494.bounds;
		      goto _LL489;
    }
    else {
      goto _LL490;
    }
    _LL490: if ((unsigned int)_temp486 > 4 ? *((int *)_temp486) == Cyc_Absyn_TunionType : 0) {
      _LL507: _temp506 = (struct Cyc_Absyn_TunionInfo)((struct Cyc_Absyn_TunionType_struct *)_temp486)->f1;
	      _LL513: _temp512 = (void *)_temp506.tunion_info;
		      goto _LL511;
	      _LL511: _temp510 = (struct Cyc_List_List *)_temp506.targs;
		      goto _LL509;
	      _LL509: _temp508 = (void *)_temp506.rgn;
		      goto _LL491;
    }
    else {
      goto _LL492;
    }
    _LL492: goto _LL493;
    _LL489: _temp508 = _temp502;
	    goto _LL491;
    _LL491: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_temp508);
	    return;
    _LL493: (void)_throw((void *)({ struct Cyc_Core_Impossible_struct *_temp514 = (struct Cyc_Core_Impossible_struct *)GC_malloc(sizeof(struct Cyc_Core_Impossible_struct));
			   _temp514[0] = ({ struct Cyc_Core_Impossible_struct _temp515;
			     _temp515.tag = Cyc_Core_Impossible;
			     _temp515.f1 = (struct _tagged_string)({ char *_temp516 = (char *)"check_pat_regions: bad pointer type";
			       struct _tagged_string _temp517;
			       _temp517.curr = _temp516;
			       _temp517.base = _temp516;
			       _temp517.last_plus_one = _temp516 + 36;
			       _temp517; });
			     _temp515; });
			   _temp514; }));
    _LL487: ;}
  
  _LL458: for(0; _temp467 != 0; _temp467 = ({ struct Cyc_List_List *_temp518 = _temp467;
    if (_temp518 == 0) {
      _throw(Null_Exception);
    }
    _temp518->tl; })) {
    Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat *)({ struct Cyc_List_List *_temp519 = _temp467;
				  if (_temp519 == 0) {
				    _throw(Null_Exception);
				  }
				  _temp519->hd; }));
  }
  { void *_temp521 = (void *)({ struct Cyc_Core_Opt *_temp520 = p->topt;
      if (_temp520 == 0) {
	_throw(Null_Exception);
      }
      _temp520->v; });
    struct Cyc_Absyn_TunionInfo _temp529;
    void *_temp531;
    struct Cyc_List_List *_temp533;
    void *_temp535;
    struct Cyc_Absyn_TunionFieldInfo _temp537;
    _LL523: if ((unsigned int)_temp521 > 4 ? *((int *)_temp521) == Cyc_Absyn_TunionType : 0) {
      _LL530: _temp529 = (struct Cyc_Absyn_TunionInfo)((struct Cyc_Absyn_TunionType_struct *)_temp521)->f1;
	      _LL536: _temp535 = (void *)_temp529.tunion_info;
		      goto _LL534;
	      _LL534: _temp533 = (struct Cyc_List_List *)_temp529.targs;
		      goto _LL532;
	      _LL532: _temp531 = (void *)_temp529.rgn;
		      goto _LL524;
    }
    else {
      goto _LL525;
    }
    _LL525: if ((unsigned int)_temp521 > 4 ? *((int *)_temp521) == Cyc_Absyn_TunionFieldType : 0) {
      _LL538: _temp537 = (struct Cyc_Absyn_TunionFieldInfo)((struct Cyc_Absyn_TunionFieldType_struct *)_temp521)->f1;
	      goto _LL526;
    }
    else {
      goto _LL527;
    }
    _LL527: goto _LL528;
    _LL524: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_temp531);
	    return;
    _LL526: return;
    _LL528: (void)_throw((void *)({ struct Cyc_Core_Impossible_struct *_temp539 = (struct Cyc_Core_Impossible_struct *)GC_malloc(sizeof(struct Cyc_Core_Impossible_struct));
			   _temp539[0] = ({ struct Cyc_Core_Impossible_struct _temp540;
			     _temp540.tag = Cyc_Core_Impossible;
			     _temp540.f1 = (struct _tagged_string)({ char *_temp541 = (char *)"check_pat_regions: bad tunion type";
			       struct _tagged_string _temp542;
			       _temp542.curr = _temp541;
			       _temp542.base = _temp541;
			       _temp542.last_plus_one = _temp541 + 35;
			       _temp542; });
			     _temp540; });
			   _temp539; }));
    _LL522: ;}
  
  _LL460: _temp483 = ((struct Cyc_List_List *(*)(struct Cyc_Absyn_Pat *(*f)(struct _tuple6 *),
						 struct Cyc_List_List *x))Cyc_List_map)((struct Cyc_Absyn_Pat *(*)(struct _tuple6 *))Cyc_Core_snd,
											_temp475);
	  goto _LL462;
  _LL462: for(0; _temp483 != 0; _temp483 = ({ struct Cyc_List_List *_temp543 = _temp483;
	    if (_temp543 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp543->tl; })) {
	    Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat *)({ struct Cyc_List_List *_temp544 = _temp483;
					  if (_temp544 == 0) {
					    _throw(Null_Exception);
					  }
					  _temp544->hd; }));
	  }
	  return;
  _LL464: return;
  _LL454: ;
}
static const int Cyc_Tcpat_Name_v = 0;
struct Cyc_Tcpat_Name_v_struct {
  int tag;
  struct _tagged_string f1;
};
static const int Cyc_Tcpat_Int_v = 1;
struct Cyc_Tcpat_Int_v_struct {
  int tag;
  int f1;
};
typedef void *Cyc_Tcpat_name_value_t;
struct Cyc_Tcpat_Con_s {
  void *name;
  int arity;
  struct Cyc_Core_Opt *span;
};
typedef struct Cyc_Tcpat_Con_s *Cyc_Tcpat_con_t;
static const int Cyc_Tcpat_Any = 0;
static const int Cyc_Tcpat_Con = 0;
struct Cyc_Tcpat_Con_struct {
  int tag;
  struct Cyc_Tcpat_Con_s *f1;
  struct Cyc_List_List *f2;
};
typedef void *Cyc_Tcpat_simple_pat_t;
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s *c1,struct Cyc_Tcpat_Con_s *c2) {
  void *_temp545 = (void *)c1->name;
  struct _tagged_string _temp551;
  int _temp553;
  _LL547: if (*((int *)_temp545) == Cyc_Tcpat_Name_v) {
    _LL552: _temp551 = (struct _tagged_string)((struct Cyc_Tcpat_Name_v_struct *)_temp545)->f1;
	    goto _LL548;
  }
  else {
    goto _LL549;
  }
  _LL549: if (*((int *)_temp545) == Cyc_Tcpat_Int_v) {
    _LL554: _temp553 = (int)((struct Cyc_Tcpat_Int_v_struct *)_temp545)->f1;
	    goto _LL550;
  }
  else {
    goto _LL546;
  }
  _LL548: {
  void *_temp555 = (void *)c2->name;
    struct _tagged_string _temp561;
    int _temp563;
    _LL557: if (*((int *)_temp555) == Cyc_Tcpat_Name_v) {
      _LL562: _temp561 = (struct _tagged_string)((struct Cyc_Tcpat_Name_v_struct *)_temp555)->f1;
	      goto _LL558;
    }
    else {
      goto _LL559;
    }
    _LL559: if (*((int *)_temp555) == Cyc_Tcpat_Int_v) {
      _LL564: _temp563 = (int)((struct Cyc_Tcpat_Int_v_struct *)_temp555)->f1;
	      goto _LL560;
    }
    else {
      goto _LL556;
    }
    _LL558: return Cyc_String_zstrcmp(_temp551,_temp561);
    _LL560: return - 1;
    _LL556: ;
  }
  _LL550: {
  void *_temp565 = (void *)c2->name;
    struct _tagged_string _temp571;
    int _temp573;
    _LL567: if (*((int *)_temp565) == Cyc_Tcpat_Name_v) {
      _LL572: _temp571 = (struct _tagged_string)((struct Cyc_Tcpat_Name_v_struct *)_temp565)->f1;
	      goto _LL568;
    }
    else {
      goto _LL569;
    }
    _LL569: if (*((int *)_temp565) == Cyc_Tcpat_Int_v) {
      _LL574: _temp573 = (int)((struct Cyc_Tcpat_Int_v_struct *)_temp565)->f1;
	      goto _LL570;
    }
    else {
      goto _LL566;
    }
    _LL568: return 1;
    _LL570: return _temp553 - _temp573;
    _LL566: ;
  }
  _LL546: ;
}
static struct Cyc_Set_Set *Cyc_Tcpat_empty_con_set() {
  return ((struct Cyc_Set_Set *(*)(int (*comp)(struct Cyc_Tcpat_Con_s *,struct Cyc_Tcpat_Con_s *)))Cyc_Set_empty)(Cyc_Tcpat_compare_con);
}
static struct Cyc_Core_Opt Cyc_Tcpat_one_opt = (struct Cyc_Core_Opt){.v=(void *)1};
static struct Cyc_Core_Opt Cyc_Tcpat_two_opt = (struct Cyc_Core_Opt){.v=(void *)2};
static struct Cyc_Core_Opt *Cyc_Tcpat_one_opt_ptr = (struct Cyc_Core_Opt *)&Cyc_Tcpat_one_opt;
static char _temp578[5] = "null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value = {0,(struct _tagged_string){_temp578,
											     _temp578,
											     _temp578 + 5}};
static char _temp582[4] = "ptr";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value = {0,(struct _tagged_string){_temp582,
											    _temp582,
											    _temp582 + 4}};
static char _temp586[6] = "tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value = {0,(struct _tagged_string){_temp586,
											      _temp586,
											      _temp586 + 6}};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0 = (struct Cyc_Tcpat_Con_s){.name=(void *)((void *)&Cyc_Tcpat_null_name_value),
									     .arity=0,
									     .span=(struct Cyc_Core_Opt *)&Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0 = (struct Cyc_Tcpat_Con_s){.name=(void *)((void *)&Cyc_Tcpat_ptr_name_value),
										 .arity=1,
										 .span=(struct Cyc_Core_Opt *)&Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0 = (struct Cyc_Tcpat_Con_s){.name=(void *)((void *)&Cyc_Tcpat_ptr_name_value),
									    .arity=1,
									    .span=(struct Cyc_Core_Opt *)&Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s *Cyc_Tcpat_null_con = &Cyc_Tcpat_null_con0;
static struct Cyc_Tcpat_Con_s *Cyc_Tcpat_null_ptr_con = &Cyc_Tcpat_null_ptr_con0;
static struct Cyc_Tcpat_Con_s *Cyc_Tcpat_ptr_con = &Cyc_Tcpat_ptr_con0;
static struct Cyc_Tcpat_Con_s *Cyc_Tcpat_int_con(int i) {
  return ({ struct Cyc_Tcpat_Con_s *_temp587 = (struct Cyc_Tcpat_Con_s *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_s));
    _temp587->name = (void *)((void *)({ struct Cyc_Tcpat_Int_v_struct *_temp588 = (struct Cyc_Tcpat_Int_v_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Int_v_struct));
      _temp588[0] = ({ struct Cyc_Tcpat_Int_v_struct _temp589;
	_temp589.tag = Cyc_Tcpat_Int_v;
	_temp589.f1 = i;
	_temp589; });
      _temp588; }));
    _temp587->arity = 0;
    _temp587->span = 0;
    _temp587; });
}
static struct Cyc_Tcpat_Con_s *Cyc_Tcpat_float_con(struct _tagged_string f) {
  return ({ struct Cyc_Tcpat_Con_s *_temp590 = (struct Cyc_Tcpat_Con_s *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_s));
    _temp590->name = (void *)((void *)({ struct Cyc_Tcpat_Name_v_struct *_temp591 = (struct Cyc_Tcpat_Name_v_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Name_v_struct));
      _temp591[0] = ({ struct Cyc_Tcpat_Name_v_struct _temp592;
	_temp592.tag = Cyc_Tcpat_Name_v;
	_temp592.f1 = f;
	_temp592; });
      _temp591; }));
    _temp590->arity = 0;
    _temp590->span = 0;
    _temp590; });
}
static struct Cyc_Tcpat_Con_s *Cyc_Tcpat_char_con(char c) {
  return ({ struct Cyc_Tcpat_Con_s *_temp593 = (struct Cyc_Tcpat_Con_s *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_s));
    _temp593->name = (void *)((void *)({ struct Cyc_Tcpat_Int_v_struct *_temp595 = (struct Cyc_Tcpat_Int_v_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Int_v_struct));
      _temp595[0] = ({ struct Cyc_Tcpat_Int_v_struct _temp596;
	_temp596.tag = Cyc_Tcpat_Int_v;
	_temp596.f1 = (int)c;
	_temp596; });
      _temp595; }));
    _temp593->arity = 0;
    _temp593->span = ({ struct Cyc_Core_Opt *_temp594 = (struct Cyc_Core_Opt *)GC_malloc_atomic(sizeof(struct Cyc_Core_Opt));
      _temp594->v = (void *)256;
      _temp594; });
    _temp593; });
}
static struct Cyc_Tcpat_Con_s *Cyc_Tcpat_tuple_con(int i) {
  return ({ struct Cyc_Tcpat_Con_s *_temp597 = (struct Cyc_Tcpat_Con_s *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_s));
    _temp597->name = (void *)((void *)&Cyc_Tcpat_tuple_name_value);
    _temp597->arity = i;
    _temp597->span = Cyc_Tcpat_one_opt_ptr;
    _temp597; });
}
static void *Cyc_Tcpat_null_pat() {
  return (void *)({ struct Cyc_Tcpat_Con_struct *_temp598 = (struct Cyc_Tcpat_Con_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_struct));
    _temp598[0] = ({ struct Cyc_Tcpat_Con_struct _temp599;
      _temp599.tag = Cyc_Tcpat_Con;
      _temp599.f1 = Cyc_Tcpat_null_con;
      _temp599.f2 = 0;
      _temp599; });
    _temp598; });
}
static void *Cyc_Tcpat_int_pat(int i) {
  return (void *)({ struct Cyc_Tcpat_Con_struct *_temp600 = (struct Cyc_Tcpat_Con_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_struct));
    _temp600[0] = ({ struct Cyc_Tcpat_Con_struct _temp601;
      _temp601.tag = Cyc_Tcpat_Con;
      _temp601.f1 = Cyc_Tcpat_int_con(i);
      _temp601.f2 = 0;
      _temp601; });
    _temp600; });
}
static void *Cyc_Tcpat_char_pat(char c) {
  return (void *)({ struct Cyc_Tcpat_Con_struct *_temp602 = (struct Cyc_Tcpat_Con_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_struct));
    _temp602[0] = ({ struct Cyc_Tcpat_Con_struct _temp603;
      _temp603.tag = Cyc_Tcpat_Con;
      _temp603.f1 = Cyc_Tcpat_char_con(c);
      _temp603.f2 = 0;
      _temp603; });
    _temp602; });
}
static void *Cyc_Tcpat_float_pat(struct _tagged_string f) {
  return (void *)({ struct Cyc_Tcpat_Con_struct *_temp604 = (struct Cyc_Tcpat_Con_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_struct));
    _temp604[0] = ({ struct Cyc_Tcpat_Con_struct _temp605;
      _temp605.tag = Cyc_Tcpat_Con;
      _temp605.f1 = Cyc_Tcpat_float_con(f);
      _temp605.f2 = 0;
      _temp605; });
    _temp604; });
}
static void *Cyc_Tcpat_null_ptr_pat(void *p) {
  return (void *)({ struct Cyc_Tcpat_Con_struct *_temp606 = (struct Cyc_Tcpat_Con_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_struct));
    _temp606[0] = ({ struct Cyc_Tcpat_Con_struct _temp607;
      _temp607.tag = Cyc_Tcpat_Con;
      _temp607.f1 = Cyc_Tcpat_null_ptr_con;
      _temp607.f2 = ({ struct Cyc_List_List *_temp608 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
	_temp608->hd = (void *)p;
	_temp608->tl = 0;
	_temp608; });
      _temp607; });
    _temp606; });
}
static void *Cyc_Tcpat_ptr_pat(void *p) {
  return (void *)({ struct Cyc_Tcpat_Con_struct *_temp609 = (struct Cyc_Tcpat_Con_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_struct));
    _temp609[0] = ({ struct Cyc_Tcpat_Con_struct _temp610;
      _temp610.tag = Cyc_Tcpat_Con;
      _temp610.f1 = Cyc_Tcpat_ptr_con;
      _temp610.f2 = ({ struct Cyc_List_List *_temp611 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
	_temp611->hd = (void *)p;
	_temp611->tl = 0;
	_temp611; });
      _temp610; });
    _temp609; });
}
static void *Cyc_Tcpat_tuple_pat(struct Cyc_List_List *ss) {
  return (void *)({ struct Cyc_Tcpat_Con_struct *_temp612 = (struct Cyc_Tcpat_Con_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_struct));
    _temp612[0] = ({ struct Cyc_Tcpat_Con_struct _temp613;
      _temp613.tag = Cyc_Tcpat_Con;
      _temp613.f1 = Cyc_Tcpat_tuple_con(((int (*)(struct Cyc_List_List *x))Cyc_List_length)(ss));
      _temp613.f2 = ss;
      _temp613; });
    _temp612; });
}
static void *Cyc_Tcpat_con_pat(struct _tagged_string con_name,struct Cyc_Core_Opt *span,
			       struct Cyc_List_List *ps) {
  struct Cyc_Tcpat_Con_s *c = ({ struct Cyc_Tcpat_Con_s *_temp616 = (struct Cyc_Tcpat_Con_s *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_s));
    _temp616->name = (void *)((void *)({ struct Cyc_Tcpat_Name_v_struct *_temp617 = (struct Cyc_Tcpat_Name_v_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Name_v_struct));
      _temp617[0] = ({ struct Cyc_Tcpat_Name_v_struct _temp618;
	_temp618.tag = Cyc_Tcpat_Name_v;
	_temp618.f1 = con_name;
	_temp618; });
      _temp617; }));
    _temp616->arity = ((int (*)(struct Cyc_List_List *x))Cyc_List_length)(ps);
    _temp616->span = span;
    _temp616; });
  return (void *)({ struct Cyc_Tcpat_Con_struct *_temp614 = (struct Cyc_Tcpat_Con_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Con_struct));
    _temp614[0] = ({ struct Cyc_Tcpat_Con_struct _temp615;
      _temp615.tag = Cyc_Tcpat_Con;
      _temp615.f1 = c;
      _temp615.f2 = ps;
      _temp615; });
    _temp614; });
}
static void *Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat *p) {
  void *s;
  { void *_temp619 = (void *)p->r;
    struct Cyc_Absyn_Vardecl *_temp653;
    int _temp655;
    void *_temp657;
    char _temp659;
    struct _tagged_string _temp661;
    struct Cyc_Absyn_Vardecl *_temp663;
    struct Cyc_Absyn_Pat *_temp665;
    struct Cyc_List_List *_temp667;
    struct Cyc_List_List *_temp669;
    struct Cyc_Absyn_Tunionfield *_temp671;
    struct Cyc_Absyn_Tuniondecl *_temp673;
    struct Cyc_List_List *_temp675;
    struct Cyc_List_List *_temp677;
    struct Cyc_Absyn_Tunionfield *_temp679;
    struct Cyc_Absyn_Tuniondecl *_temp681;
    struct Cyc_List_List *_temp683;
    struct Cyc_List_List *_temp685;
    struct Cyc_List_List *_temp687;
    struct Cyc_Core_Opt *_temp689;
    struct Cyc_Absyn_Structdecl *_temp691;
    struct Cyc_Absyn_Enumfield *_temp693;
    struct Cyc_Absyn_Enumdecl *_temp695;
    struct _tuple1 *_temp697;
    struct Cyc_List_List *_temp699;
    struct Cyc_List_List *_temp701;
    struct _tuple1 *_temp703;
    struct Cyc_List_List *_temp705;
    struct Cyc_List_List *_temp707;
    struct _tuple1 *_temp709;
    _LL621: if (_temp619 == (void *)Cyc_Absyn_Wild_p) {
      goto _LL622;
    }
    else {
      goto _LL623;
    }
    _LL623: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_Var_p : 0) {
      _LL654: _temp653 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Var_p_struct *)_temp619)->f1;
	      goto _LL624;
    }
    else {
      goto _LL625;
    }
    _LL625: if (_temp619 == (void *)Cyc_Absyn_Null_p) {
      goto _LL626;
    }
    else {
      goto _LL627;
    }
    _LL627: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_Int_p : 0) {
      _LL658: _temp657 = (void *)((struct Cyc_Absyn_Int_p_struct *)_temp619)->f1;
	      goto _LL656;
      _LL656: _temp655 = (int)((struct Cyc_Absyn_Int_p_struct *)_temp619)->f2;
	      goto _LL628;
    }
    else {
      goto _LL629;
    }
    _LL629: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_Char_p : 0) {
      _LL660: _temp659 = (char)((struct Cyc_Absyn_Char_p_struct *)_temp619)->f1;
	      goto _LL630;
    }
    else {
      goto _LL631;
    }
    _LL631: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_Float_p : 0) {
      _LL662: _temp661 = (struct _tagged_string)((struct Cyc_Absyn_Float_p_struct *)_temp619)->f1;
	      goto _LL632;
    }
    else {
      goto _LL633;
    }
    _LL633: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_Reference_p : 0) {
      _LL664: _temp663 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_Absyn_Reference_p_struct *)_temp619)->f1;
	      goto _LL634;
    }
    else {
      goto _LL635;
    }
    _LL635: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_Pointer_p : 0) {
      _LL666: _temp665 = (struct Cyc_Absyn_Pat *)((struct Cyc_Absyn_Pointer_p_struct *)_temp619)->f1;
	      goto _LL636;
    }
    else {
      goto _LL637;
    }
    _LL637: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_Tunion_p : 0) {
      _LL674: _temp673 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_Tunion_p_struct *)_temp619)->f1;
	      goto _LL672;
      _LL672: _temp671 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Absyn_Tunion_p_struct *)_temp619)->f2;
	      goto _LL670;
      _LL670: _temp669 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_p_struct *)_temp619)->f3;
	      goto _LL668;
      _LL668: _temp667 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_p_struct *)_temp619)->f4;
	      if (_temp667 == 0) {
		goto _LL638;
	      }
	      else {
		goto _LL639;
	      }
    }
    else {
      goto _LL639;
    }
    _LL639: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_Tunion_p : 0) {
      _LL682: _temp681 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_Tunion_p_struct *)_temp619)->f1;
	      goto _LL680;
      _LL680: _temp679 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Absyn_Tunion_p_struct *)_temp619)->f2;
	      goto _LL678;
      _LL678: _temp677 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_p_struct *)_temp619)->f3;
	      goto _LL676;
      _LL676: _temp675 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_p_struct *)_temp619)->f4;
	      goto _LL640;
    }
    else {
      goto _LL641;
    }
    _LL641: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_Tuple_p : 0) {
      _LL684: _temp683 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tuple_p_struct *)_temp619)->f1;
	      goto _LL642;
    }
    else {
      goto _LL643;
    }
    _LL643: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_Struct_p : 0) {
      _LL692: _temp691 = (struct Cyc_Absyn_Structdecl *)((struct Cyc_Absyn_Struct_p_struct *)_temp619)->f1;
	      goto _LL690;
      _LL690: _temp689 = (struct Cyc_Core_Opt *)((struct Cyc_Absyn_Struct_p_struct *)_temp619)->f2;
	      goto _LL688;
      _LL688: _temp687 = (struct Cyc_List_List *)((struct Cyc_Absyn_Struct_p_struct *)_temp619)->f3;
	      goto _LL686;
      _LL686: _temp685 = (struct Cyc_List_List *)((struct Cyc_Absyn_Struct_p_struct *)_temp619)->f4;
	      goto _LL644;
    }
    else {
      goto _LL645;
    }
    _LL645: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_Enum_p : 0) {
      _LL696: _temp695 = (struct Cyc_Absyn_Enumdecl *)((struct Cyc_Absyn_Enum_p_struct *)_temp619)->f1;
	      goto _LL694;
      _LL694: _temp693 = (struct Cyc_Absyn_Enumfield *)((struct Cyc_Absyn_Enum_p_struct *)_temp619)->f2;
	      goto _LL646;
    }
    else {
      goto _LL647;
    }
    _LL647: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_UnknownId_p : 0) {
      _LL698: _temp697 = (struct _tuple1 *)((struct Cyc_Absyn_UnknownId_p_struct *)_temp619)->f1;
	      goto _LL648;
    }
    else {
      goto _LL649;
    }
    _LL649: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_UnknownCall_p : 0) {
      _LL704: _temp703 = (struct _tuple1 *)((struct Cyc_Absyn_UnknownCall_p_struct *)_temp619)->f1;
	      goto _LL702;
      _LL702: _temp701 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownCall_p_struct *)_temp619)->f2;
	      goto _LL700;
      _LL700: _temp699 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownCall_p_struct *)_temp619)->f3;
	      goto _LL650;
    }
    else {
      goto _LL651;
    }
    _LL651: if ((unsigned int)_temp619 > 2 ? *((int *)_temp619) == Cyc_Absyn_UnknownFields_p : 0) {
      _LL710: _temp709 = (struct _tuple1 *)((struct Cyc_Absyn_UnknownFields_p_struct *)_temp619)->f1;
	      goto _LL708;
      _LL708: _temp707 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownFields_p_struct *)_temp619)->f2;
	      goto _LL706;
      _LL706: _temp705 = (struct Cyc_List_List *)((struct Cyc_Absyn_UnknownFields_p_struct *)_temp619)->f3;
	      goto _LL652;
    }
    else {
      goto _LL620;
    }
    _LL622: s = (void *)Cyc_Tcpat_Any;
	    goto _LL620;
    _LL624: s = (void *)Cyc_Tcpat_Any;
	    goto _LL620;
    _LL626: s = Cyc_Tcpat_null_pat();
	    goto _LL620;
    _LL628: s = Cyc_Tcpat_int_pat(_temp655);
	    goto _LL620;
    _LL630: s = Cyc_Tcpat_char_pat(_temp659);
	    goto _LL620;
    _LL632: s = Cyc_Tcpat_float_pat(_temp661);
	    goto _LL620;
    _LL634: s = (void *)Cyc_Tcpat_Any;
	    goto _LL620;
    _LL636: { void *_temp712 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp711 = p->topt;
						     if (_temp711 == 0) {
						       _throw(Null_Exception);
						     }
						     _temp711->v; }));
      struct Cyc_Absyn_PtrInfo _temp720;
      struct Cyc_Absyn_Conref *_temp722;
      struct Cyc_Absyn_Tqual _temp724;
      struct Cyc_Absyn_Conref *_temp726;
      void *_temp728;
      void *_temp730;
      struct Cyc_Absyn_TunionInfo _temp732;
      _LL714: if ((unsigned int)_temp712 > 4 ? *((int *)_temp712) == Cyc_Absyn_PointerType : 0) {
	_LL721: _temp720 = (struct Cyc_Absyn_PtrInfo)((struct Cyc_Absyn_PointerType_struct *)_temp712)->f1;
		_LL731: _temp730 = (void *)_temp720.elt_typ;
			goto _LL729;
		_LL729: _temp728 = (void *)_temp720.rgn_typ;
			goto _LL727;
		_LL727: _temp726 = (struct Cyc_Absyn_Conref *)_temp720.nullable;
			goto _LL725;
		_LL725: _temp724 = (struct Cyc_Absyn_Tqual)_temp720.tq;
			goto _LL723;
		_LL723: _temp722 = (struct Cyc_Absyn_Conref *)_temp720.bounds;
			goto _LL715;
      }
      else {
	goto _LL716;
      }
      _LL716: if ((unsigned int)_temp712 > 4 ? *((int *)_temp712) == Cyc_Absyn_TunionType : 0) {
	_LL733: _temp732 = (struct Cyc_Absyn_TunionInfo)((struct Cyc_Absyn_TunionType_struct *)_temp712)->f1;
		goto _LL717;
      }
      else {
	goto _LL718;
      }
      _LL718: goto _LL719;
      _LL715: {
      int is_nullable = 0;
	int still_working = 1;
	while (still_working) {
	  void *_temp734 = (void *)_temp726->v;
	  struct Cyc_Absyn_Conref *_temp742;
	  int _temp744;
	  _LL736: if ((unsigned int)_temp734 > 1 ? *((int *)_temp734) == Cyc_Absyn_Forward_constr : 0) {
	    _LL743: _temp742 = (struct Cyc_Absyn_Conref *)((struct Cyc_Absyn_Forward_constr_struct *)_temp734)->f1;
		    goto _LL737;
	  }
	  else {
	    goto _LL738;
	  }
	  _LL738: if (_temp734 == (void *)Cyc_Absyn_No_constr) {
	    goto _LL739;
	  }
	  else {
	    goto _LL740;
	  }
	  _LL740: if ((unsigned int)_temp734 > 1 ? *((int *)_temp734) == Cyc_Absyn_Eq_constr : 0) {
	    _LL745: _temp744 = (int)((struct Cyc_Absyn_Eq_constr_struct *)_temp734)->f1;
		    goto _LL741;
	  }
	  else {
	    goto _LL735;
	  }
	  _LL737: (void *)(_temp726->v = (void *)((void *)_temp742->v));
		  continue;
	  _LL739: (void *)(_temp726->v = (void *)((void *)({ struct Cyc_Absyn_Eq_constr_struct *_temp746 = (struct Cyc_Absyn_Eq_constr_struct *)GC_malloc(sizeof(struct Cyc_Absyn_Eq_constr_struct));
		    _temp746[0] = ({ struct Cyc_Absyn_Eq_constr_struct _temp747;
		      _temp747.tag = Cyc_Absyn_Eq_constr;
		      _temp747.f1 = (void *)0;
		      _temp747; });
		    _temp746; })));
		  is_nullable = 0;
		  still_working = 0;
		  goto _LL735;
	  _LL741: is_nullable = (int)_temp744;
		  still_working = 0;
		  goto _LL735;
	  _LL735: ;
	}
	{ void *ss = Cyc_Tcpat_compile_pat(_temp665);
	  if (is_nullable) {
	    s = Cyc_Tcpat_null_ptr_pat(ss);
	  }
	  else {
	    s = Cyc_Tcpat_ptr_pat(ss);
	  }
	  goto _LL713;}
	
      }
      _LL717: { void *_temp748 = (void *)_temp665->r;
	struct Cyc_List_List *_temp754;
	struct Cyc_List_List *_temp756;
	struct Cyc_Absyn_Tunionfield *_temp758;
	struct Cyc_Absyn_Tuniondecl *_temp760;
	_LL750: if ((unsigned int)_temp748 > 2 ? *((int *)_temp748) == Cyc_Absyn_Tunion_p : 0) {
	  _LL761: _temp760 = (struct Cyc_Absyn_Tuniondecl *)((struct Cyc_Absyn_Tunion_p_struct *)_temp748)->f1;
		  goto _LL759;
	  _LL759: _temp758 = (struct Cyc_Absyn_Tunionfield *)((struct Cyc_Absyn_Tunion_p_struct *)_temp748)->f2;
		  goto _LL757;
	  _LL757: _temp756 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_p_struct *)_temp748)->f3;
		  goto _LL755;
	  _LL755: _temp754 = (struct Cyc_List_List *)((struct Cyc_Absyn_Tunion_p_struct *)_temp748)->f4;
		  goto _LL751;
	}
	else {
	  goto _LL752;
	}
	_LL752: goto _LL753;
	_LL751: {
	struct Cyc_Core_Opt *span;
	  if (_temp760->is_xtunion) {
	    span = 0;
	  }
	  else {
	    span = ({ struct Cyc_Core_Opt *_temp762 = (struct Cyc_Core_Opt *)GC_malloc_atomic(sizeof(struct Cyc_Core_Opt));
	      _temp762->v = (void *)((int (*)(struct Cyc_List_List *x))Cyc_List_length)((struct Cyc_List_List *)({ struct Cyc_Core_Opt *_temp763 = _temp760->fields;
											  if (_temp763 == 0) {
											    _throw(Null_Exception);
											  }
											  _temp763->v; }));
	      _temp762; });
	  }
	  s = Cyc_Tcpat_con_pat(*(*_temp758->name).f2,span,((struct Cyc_List_List *(*)(void *(*f)(struct Cyc_Absyn_Pat *),
										       struct Cyc_List_List *x))Cyc_List_map)(Cyc_Tcpat_compile_pat,
															      _temp754));
	  goto _LL749;
	}
	_LL753: s = ((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp764 = (char *)"non-[x]tunion pattern has tunion type";
									   struct _tagged_string _temp765;
									   _temp765.curr = _temp764;
									   _temp765.base = _temp764;
									   _temp765.last_plus_one = _temp764 + 38;
									   _temp765; }));
		goto _LL749;
	_LL749: ;
      }
      goto _LL713;
      _LL719: s = ((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp766 = (char *)"pointer pattern does not have pointer type!";
									 struct _tagged_string _temp767;
									 _temp767.curr = _temp766;
									 _temp767.base = _temp766;
									 _temp767.last_plus_one = _temp766 + 44;
									 _temp767; }));
	      goto _LL713;
      _LL713: ;
    }
    goto _LL620;
    _LL638: {
    struct Cyc_Core_Opt *span;
      { void *_temp769 = Cyc_Tcutil_compress((void *)({ struct Cyc_Core_Opt *_temp768 = p->topt;
					       if (_temp768 == 0) {
						 _throw(Null_Exception);
					       }
					       _temp768->v; }));
	struct Cyc_Absyn_TunionInfo _temp777;
	struct Cyc_Absyn_TunionFieldInfo _temp779;
	_LL771: if ((unsigned int)_temp769 > 4 ? *((int *)_temp769) == Cyc_Absyn_TunionType : 0) {
	  _LL778: _temp777 = (struct Cyc_Absyn_TunionInfo)((struct Cyc_Absyn_TunionType_struct *)_temp769)->f1;
		  goto _LL772;
	}
	else {
	  goto _LL773;
	}
	_LL773: if ((unsigned int)_temp769 > 4 ? *((int *)_temp769) == Cyc_Absyn_TunionFieldType : 0) {
	  _LL780: _temp779 = (struct Cyc_Absyn_TunionFieldInfo)((struct Cyc_Absyn_TunionFieldType_struct *)_temp769)->f1;
		  goto _LL774;
	}
	else {
	  goto _LL775;
	}
	_LL775: goto _LL776;
	_LL772: if (_temp673->is_xtunion) {
		  span = 0;
		}
		else {
		  span = ({ struct Cyc_Core_Opt *_temp781 = (struct Cyc_Core_Opt *)GC_malloc_atomic(sizeof(struct Cyc_Core_Opt));
		    _temp781->v = (void *)((int (*)(struct Cyc_List_List *x))Cyc_List_length)((struct Cyc_List_List *)({ struct Cyc_Core_Opt *_temp782 = _temp673->fields;
												if (_temp782 == 0) {
												  _throw(Null_Exception);
												}
												_temp782->v; }));
		    _temp781; });
		}
		goto _LL770;
	_LL774: span = ({ struct Cyc_Core_Opt *_temp783 = (struct Cyc_Core_Opt *)GC_malloc_atomic(sizeof(struct Cyc_Core_Opt));
		  _temp783->v = (void *)1;
		  _temp783; });
		goto _LL770;
	_LL776: span = ((struct Cyc_Core_Opt *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp784 = (char *)"void tunion pattern has bad type";
											     struct _tagged_string _temp785;
											     _temp785.curr = _temp784;
											     _temp785.base = _temp784;
											     _temp785.last_plus_one = _temp784 + 33;
											     _temp785; }));
		goto _LL770;
	_LL770: ;
      }
      s = Cyc_Tcpat_con_pat(*(*_temp671->name).f2,span,0);
      goto _LL620;
    }
    _LL640: _temp683 = _temp675;
	    goto _LL642;
    _LL642: s = Cyc_Tcpat_tuple_pat(((struct Cyc_List_List *(*)(void *(*f)(struct Cyc_Absyn_Pat *),
								struct Cyc_List_List *x))Cyc_List_map)(Cyc_Tcpat_compile_pat,
												       _temp683));
	    goto _LL620;
    _LL644: {
    struct Cyc_List_List *ps = 0;
      { struct Cyc_List_List *fields = (struct Cyc_List_List *)({ struct Cyc_Core_Opt *_temp818 = _temp691->fields;
	  if (_temp818 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp818->v; });
	for(0; fields != 0; fields = ({ struct Cyc_List_List *_temp786 = fields;
	  if (_temp786 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp786->tl; })) {
	  int found = Cyc_String_strcmp(*((struct Cyc_Absyn_Structfield *)({ struct Cyc_List_List *_temp815 = fields;
					  if (_temp815 == 0) {
					    _throw(Null_Exception);
					  }
					  _temp815->hd; }))->name,(struct _tagged_string)({ char *_temp816 = (char *)"";
					  struct _tagged_string _temp817;
					  _temp817.curr = _temp816;
					  _temp817.base = _temp816;
					  _temp817.last_plus_one = _temp816 + 1;
					  _temp817; })) == 0;
	  { struct Cyc_List_List *dlps0 = _temp685;
	    for(0; ! found ? dlps0 != 0 : 0; dlps0 = ({ struct Cyc_List_List *_temp787 = dlps0;
	      if (_temp787 == 0) {
		_throw(Null_Exception);
	      }
	      _temp787->tl; })) {
	      struct _tuple6 _temp791;
	      struct Cyc_Absyn_Pat *_temp792;
	      struct Cyc_List_List *_temp794;
	      struct _tuple6 *_temp789 = (struct _tuple6 *)({ struct Cyc_List_List *_temp788 = dlps0;
		if (_temp788 == 0) {
		  _throw(Null_Exception);
		}
		_temp788->hd; });
	      _temp791 = *_temp789;
	      _LL795: _temp794 = _temp791.f1;
		      goto _LL793;
	      _LL793: _temp792 = _temp791.f2;
		      goto _LL790;
	      _LL790: {
	      struct Cyc_List_List *_temp796 = _temp794;
		struct Cyc_List_List _temp802;
		struct Cyc_List_List *_temp803;
		void *_temp805;
		struct _tagged_string *_temp807;
		_LL798: if (_temp796 == 0) {
		  goto _LL800;
		}
		else {
		  _temp802 = *_temp796;
		  _LL806: _temp805 = (void *)_temp802.hd;
			  if (*((int *)_temp805) == Cyc_Absyn_FieldName) {
			    _LL808: _temp807 = (struct _tagged_string *)((struct Cyc_Absyn_FieldName_struct *)_temp805)->f1;
				    goto _LL804;
			  }
			  else {
			    goto _LL800;
			  }
		  _LL804: _temp803 = (struct Cyc_List_List *)_temp802.tl;
			  if (_temp803 == 0) {
			    goto _LL799;
			  }
			  else {
			    goto _LL800;
			  }
		}
		_LL800: goto _LL801;
		_LL799: if (Cyc_String_zstrptrcmp(_temp807,((struct Cyc_Absyn_Structfield *)({ struct Cyc_List_List *_temp809 = fields;
						    if (_temp809 == 0) {
						      _throw(Null_Exception);
						    }
						    _temp809->hd; }))->name) == 0) {
			  ps = ({ struct Cyc_List_List *_temp810 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
			    _temp810->hd = (void *)Cyc_Tcpat_compile_pat(_temp792);
			    _temp810->tl = ps;
			    _temp810; });
			  found = 1;
			}
			goto _LL797;
		_LL801: ((void (*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp811 = (char *)"bad designator(s)";
									      struct _tagged_string _temp812;
									      _temp812.curr = _temp811;
									      _temp812.base = _temp811;
									      _temp812.last_plus_one = _temp811 + 18;
									      _temp812; }));
			goto _LL797;
		_LL797: ;
	      }
	    }
	  }
	  if (! found) {
	    ((void (*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp813 = (char *)"bad designator";
								  struct _tagged_string _temp814;
								  _temp814.curr = _temp813;
								  _temp814.base = _temp813;
								  _temp814.last_plus_one = _temp813 + 15;
								  _temp814; }));
	  }
	}
      }
      s = Cyc_Tcpat_tuple_pat(ps);
      goto _LL620;
    }
    _LL646: {
    int span = ((int (*)(struct Cyc_List_List *x))Cyc_List_length)(_temp695->fields);
      s = Cyc_Tcpat_con_pat(*(*_temp693->name).f2,({ struct Cyc_Core_Opt *_temp819 = (struct Cyc_Core_Opt *)GC_malloc_atomic(sizeof(struct Cyc_Core_Opt));
			      _temp819->v = (void *)span;
			      _temp819; }),0);
      goto _LL620;
    }
    _LL648: s = ((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp820 = (char *)"compile_pat: unknownid";
								       struct _tagged_string _temp821;
								       _temp821.curr = _temp820;
								       _temp821.base = _temp820;
								       _temp821.last_plus_one = _temp820 + 23;
								       _temp821; }));
	    goto _LL620;
    _LL650: s = ((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp822 = (char *)"compile_pat: unknowncall";
								       struct _tagged_string _temp823;
								       _temp823.curr = _temp822;
								       _temp823.base = _temp822;
								       _temp823.last_plus_one = _temp822 + 25;
								       _temp823; }));
	    goto _LL620;
    _LL652: s = ((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp824 = (char *)"compile_pat: unknownfields";
								       struct _tagged_string _temp825;
								       _temp825.curr = _temp824;
								       _temp825.base = _temp824;
								       _temp825.last_plus_one = _temp824 + 27;
								       _temp825; }));
	    goto _LL620;
    _LL620: ;
  }
  return s;
}
typedef struct Cyc_List_List *Cyc_Tcpat_match_t;
static const int Cyc_Tcpat_Pos = 0;
struct Cyc_Tcpat_Pos_struct {
  int tag;
  struct Cyc_Tcpat_Con_s *f1;
  struct Cyc_List_List *f2;
};
static const int Cyc_Tcpat_Neg = 1;
struct Cyc_Tcpat_Neg_struct {
  int tag;
  struct Cyc_Set_Set *f1;
};
typedef void *Cyc_Tcpat_term_desc_t;
typedef struct Cyc_List_List *Cyc_Tcpat_context_t;
typedef struct Cyc_List_List *Cyc_Tcpat_access_t;
static const int Cyc_Tcpat_Failure = 0;
static const int Cyc_Tcpat_Success = 0;
struct Cyc_Tcpat_Success_struct {
  int tag;
  void *f1;
};
static const int Cyc_Tcpat_IfEq = 1;
struct Cyc_Tcpat_IfEq_struct {
  int tag;
  struct Cyc_List_List *f1;
  struct Cyc_Tcpat_Con_s *f2;
  void *f3;
  void *f4;
};
typedef void *Cyc_Tcpat_decision_t;
struct _tuple9 {
  struct Cyc_List_List *f1;
  struct Cyc_List_List *f2;
  struct Cyc_List_List *f3;
};
typedef struct _tuple9 *Cyc_Tcpat_work_stack_frame_t;
typedef struct Cyc_List_List *Cyc_Tcpat_work_stack_t;
static const int Cyc_Tcpat_Yes = 0;
static const int Cyc_Tcpat_No = 1;
static const int Cyc_Tcpat_Maybe = 2;
typedef void *Cyc_Tcpat_answer_t;
static void *Cyc_Tcpat_add_neg(void *td,struct Cyc_Tcpat_Con_s *c) {
  void *_temp826 = td;
  struct Cyc_Set_Set *_temp832;
  struct Cyc_List_List *_temp834;
  struct Cyc_Tcpat_Con_s *_temp836;
  _LL828: if (*((int *)_temp826) == Cyc_Tcpat_Neg) {
    _LL833: _temp832 = (struct Cyc_Set_Set *)((struct Cyc_Tcpat_Neg_struct *)_temp826)->f1;
	    goto _LL829;
  }
  else {
    goto _LL830;
  }
  _LL830: if (*((int *)_temp826) == Cyc_Tcpat_Pos) {
    _LL837: _temp836 = (struct Cyc_Tcpat_Con_s *)((struct Cyc_Tcpat_Pos_struct *)_temp826)->f1;
	    goto _LL835;
    _LL835: _temp834 = (struct Cyc_List_List *)((struct Cyc_Tcpat_Pos_struct *)_temp826)->f2;
	    goto _LL831;
  }
  else {
    goto _LL827;
  }
  _LL829: if (((int (*)(struct Cyc_Set_Set *s,struct Cyc_Tcpat_Con_s *elt))Cyc_Set_member)(_temp832,
											   c)) {
	    ((void (*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp838 = (char *)"add_neg called when constructor already in set";
								  struct _tagged_string _temp839;
								  _temp839.curr = _temp838;
								  _temp839.base = _temp838;
								  _temp839.last_plus_one = _temp838 + 47;
								  _temp839; }));
	  }
	  if (c->span != 0 ? ((int (*)(struct Cyc_Set_Set *s))Cyc_Set_cardinality)(_temp832) + 1 >= (int)({ struct Cyc_Core_Opt *_temp840 = c->span;
	    if (_temp840 == 0) {
	      _throw(Null_Exception);
	    }
	    _temp840->v; }) : 0) {
	    ((void (*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp841 = (char *)"add_neg called when |cs U {c}| >= span(c)";
								  struct _tagged_string _temp842;
								  _temp842.curr = _temp841;
								  _temp842.base = _temp841;
								  _temp842.last_plus_one = _temp841 + 42;
								  _temp842; }));
	  }
	  return (void *)({ struct Cyc_Tcpat_Neg_struct *_temp843 = (struct Cyc_Tcpat_Neg_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Neg_struct));
	    _temp843[0] = ({ struct Cyc_Tcpat_Neg_struct _temp844;
	      _temp844.tag = Cyc_Tcpat_Neg;
	      _temp844.f1 = ((struct Cyc_Set_Set *(*)(struct Cyc_Set_Set *s,
						      struct Cyc_Tcpat_Con_s *elt))Cyc_Set_insert)(_temp832,
												   c);
	      _temp844; });
	    _temp843; });
  _LL831: return ((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp845 = (char *)"add_neg called when td is Positive";
									struct _tagged_string _temp846;
									_temp846.curr = _temp845;
									_temp846.base = _temp845;
									_temp846.last_plus_one = _temp845 + 35;
									_temp846; }));
  _LL827: ;
}
static void *Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s *c,void *td) {
  void *_temp847 = td;
  struct Cyc_List_List *_temp853;
  struct Cyc_Tcpat_Con_s *_temp855;
  struct Cyc_Set_Set *_temp857;
  _LL849: if (*((int *)_temp847) == Cyc_Tcpat_Pos) {
    _LL856: _temp855 = (struct Cyc_Tcpat_Con_s *)((struct Cyc_Tcpat_Pos_struct *)_temp847)->f1;
	    goto _LL854;
    _LL854: _temp853 = (struct Cyc_List_List *)((struct Cyc_Tcpat_Pos_struct *)_temp847)->f2;
	    goto _LL850;
  }
  else {
    goto _LL851;
  }
  _LL851: if (*((int *)_temp847) == Cyc_Tcpat_Neg) {
    _LL858: _temp857 = (struct Cyc_Set_Set *)((struct Cyc_Tcpat_Neg_struct *)_temp847)->f1;
	    goto _LL852;
  }
  else {
    goto _LL848;
  }
  _LL850: if (Cyc_Tcpat_compare_con(c,_temp855) == 0) {
    return (void *)Cyc_Tcpat_Yes;
  }
  else {
    return (void *)Cyc_Tcpat_No;
  }
  _LL852: if (((int (*)(struct Cyc_Set_Set *s,struct Cyc_Tcpat_Con_s *elt))Cyc_Set_member)(_temp857,
											   c)) {
    return (void *)Cyc_Tcpat_No;
  }
  else {
    if (c->span != 0 ? (int)({ struct Cyc_Core_Opt *_temp859 = c->span;
      if (_temp859 == 0) {
	_throw(Null_Exception);
      }
      _temp859->v; }) == ((int (*)(struct Cyc_Set_Set *s))Cyc_Set_cardinality)(_temp857) + 1 : 0) {
      return (void *)Cyc_Tcpat_Yes;
    }
    else {
      return (void *)Cyc_Tcpat_Maybe;
    }
  }
  _LL848: ;
}
struct _tuple10 {
  struct Cyc_Tcpat_Con_s *f1;
  struct Cyc_List_List *f2;
};
static struct Cyc_List_List *Cyc_Tcpat_augment(struct Cyc_List_List *ctxt,
					       void *dsc) {
  struct Cyc_List_List *_temp860 = ctxt;
  struct Cyc_List_List _temp866;
  struct Cyc_List_List *_temp867;
  struct _tuple10 *_temp869;
  struct _tuple10 _temp871;
  struct Cyc_List_List *_temp872;
  struct Cyc_Tcpat_Con_s *_temp874;
  _LL862: if (_temp860 == 0) {
    goto _LL863;
  }
  else {
    goto _LL864;
  }
  _LL864: if (_temp860 == 0) {
    goto _LL861;
  }
  else {
    _temp866 = *_temp860;
    _LL870: _temp869 = (struct _tuple10 *)_temp866.hd;
	    _temp871 = *_temp869;
	    _LL875: _temp874 = _temp871.f1;
		    goto _LL873;
	    _LL873: _temp872 = _temp871.f2;
		    goto _LL868;
    _LL868: _temp867 = (struct Cyc_List_List *)_temp866.tl;
	    goto _LL865;
  }
  _LL863: return 0;
  _LL865: return ({ struct Cyc_List_List *_temp876 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
    _temp876->hd = (void *)({ struct _tuple10 *_temp877 = (struct _tuple10 *)GC_malloc(sizeof(struct _tuple10));
      _temp877->f1 = _temp874;
      _temp877->f2 = (struct Cyc_List_List *)({ struct Cyc_List_List *_temp878 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
	_temp878->hd = (void *)dsc;
	_temp878->tl = _temp872;
	_temp878; });
      _temp877; });
    _temp876->tl = _temp867;
    _temp876; });
  _LL861: ;
}
static struct Cyc_List_List *Cyc_Tcpat_norm_context(struct Cyc_List_List *ctxt) {
  struct Cyc_List_List *_temp879 = ctxt;
  struct Cyc_List_List _temp885;
  struct Cyc_List_List *_temp886;
  struct _tuple10 *_temp888;
  struct _tuple10 _temp890;
  struct Cyc_List_List *_temp891;
  struct Cyc_Tcpat_Con_s *_temp893;
  _LL881: if (_temp879 == 0) {
    goto _LL882;
  }
  else {
    goto _LL883;
  }
  _LL883: if (_temp879 == 0) {
    goto _LL880;
  }
  else {
    _temp885 = *_temp879;
    _LL889: _temp888 = (struct _tuple10 *)_temp885.hd;
	    _temp890 = *_temp888;
	    _LL894: _temp893 = _temp890.f1;
		    goto _LL892;
	    _LL892: _temp891 = _temp890.f2;
		    goto _LL887;
    _LL887: _temp886 = (struct Cyc_List_List *)_temp885.tl;
	    goto _LL884;
  }
  _LL882: return ((struct Cyc_List_List *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp895 = (char *)"norm_context: empty context";
											struct _tagged_string _temp896;
											_temp896.curr = _temp895;
											_temp896.base = _temp895;
											_temp896.last_plus_one = _temp895 + 28;
											_temp896; }));
  _LL884: return Cyc_Tcpat_augment(_temp886,(void *)({ struct Cyc_Tcpat_Pos_struct *_temp897 = (struct Cyc_Tcpat_Pos_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Pos_struct));
				     _temp897[0] = ({ struct Cyc_Tcpat_Pos_struct _temp898;
				       _temp898.tag = Cyc_Tcpat_Pos;
				       _temp898.f1 = _temp893;
				       _temp898.f2 = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x))Cyc_List_rev)(_temp891);
				       _temp898; });
				     _temp897; }));
  _LL880: ;
}
static void *Cyc_Tcpat_build_desc(struct Cyc_List_List *ctxt,void *dsc,struct Cyc_List_List *work) {
  struct _tuple5 _temp900 = ({ struct _tuple5 _temp899;
    _temp899.f1 = ctxt;
    _temp899.f2 = work;
    _temp899; });
  struct Cyc_List_List *_temp910;
  struct Cyc_List_List *_temp912;
  struct Cyc_List_List *_temp914;
  struct Cyc_List_List *_temp916;
  struct Cyc_List_List *_temp918;
  struct Cyc_List_List *_temp920;
  struct Cyc_List_List *_temp922;
  struct Cyc_List_List _temp924;
  struct Cyc_List_List *_temp925;
  struct _tuple9 *_temp927;
  struct _tuple9 _temp929;
  struct Cyc_List_List *_temp930;
  struct Cyc_List_List *_temp932;
  struct Cyc_List_List *_temp934;
  struct Cyc_List_List *_temp936;
  struct Cyc_List_List _temp938;
  struct Cyc_List_List *_temp939;
  struct _tuple10 *_temp941;
  struct _tuple10 _temp943;
  struct Cyc_List_List *_temp944;
  struct Cyc_Tcpat_Con_s *_temp946;
  _LL902: _LL913: _temp912 = _temp900.f1;
		  if (_temp912 == 0) {
		    goto _LL911;
		  }
		  else {
		    goto _LL904;
		  }
	  _LL911: _temp910 = _temp900.f2;
		  if (_temp910 == 0) {
		    goto _LL903;
		  }
		  else {
		    goto _LL904;
		  }
  _LL904: _LL917: _temp916 = _temp900.f1;
		  if (_temp916 == 0) {
		    goto _LL915;
		  }
		  else {
		    goto _LL906;
		  }
	  _LL915: _temp914 = _temp900.f2;
		  goto _LL905;
  _LL906: _LL921: _temp920 = _temp900.f1;
		  goto _LL919;
	  _LL919: _temp918 = _temp900.f2;
		  if (_temp918 == 0) {
		    goto _LL907;
		  }
		  else {
		    goto _LL908;
		  }
  _LL908: _LL937: _temp936 = _temp900.f1;
		  if (_temp936 == 0) {
		    goto _LL901;
		  }
		  else {
		    _temp938 = *_temp936;
		    _LL942: _temp941 = (struct _tuple10 *)_temp938.hd;
			    _temp943 = *_temp941;
			    _LL947: _temp946 = _temp943.f1;
				    goto _LL945;
			    _LL945: _temp944 = _temp943.f2;
				    goto _LL940;
		    _LL940: _temp939 = (struct Cyc_List_List *)_temp938.tl;
			    goto _LL923;
		  }
	  _LL923: _temp922 = _temp900.f2;
		  if (_temp922 == 0) {
		    goto _LL901;
		  }
		  else {
		    _temp924 = *_temp922;
		    _LL928: _temp927 = (struct _tuple9 *)_temp924.hd;
			    _temp929 = *_temp927;
			    _LL935: _temp934 = _temp929.f1;
				    goto _LL933;
			    _LL933: _temp932 = _temp929.f2;
				    goto _LL931;
			    _LL931: _temp930 = _temp929.f3;
				    goto _LL926;
		    _LL926: _temp925 = (struct Cyc_List_List *)_temp924.tl;
			    goto _LL909;
		  }
  _LL903: return dsc;
  _LL905: return ((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp948 = (char *)"build_desc: ctxt and work don't match";
									struct _tagged_string _temp949;
									_temp949.curr = _temp948;
									_temp949.base = _temp948;
									_temp949.last_plus_one = _temp948 + 38;
									_temp949; }));
  _LL907: return ((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp950 = (char *)"build_desc: ctxt and work don't match";
									struct _tagged_string _temp951;
									_temp951.curr = _temp950;
									_temp951.base = _temp950;
									_temp951.last_plus_one = _temp950 + 38;
									_temp951; }));
  _LL909: {
  struct Cyc_Tcpat_Pos_struct *td = ({ struct Cyc_Tcpat_Pos_struct *_temp952 = (struct Cyc_Tcpat_Pos_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Pos_struct));
      _temp952[0] = ({ struct Cyc_Tcpat_Pos_struct _temp953;
	_temp953.tag = Cyc_Tcpat_Pos;
	_temp953.f1 = _temp946;
	_temp953.f2 = ((struct Cyc_List_List *(*)(struct Cyc_List_List *x,
						  struct Cyc_List_List *y))Cyc_List_append)(((struct Cyc_List_List *(*)(struct Cyc_List_List *x))Cyc_List_rev)(_temp944),
											    ({ struct Cyc_List_List *_temp954 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
											      _temp954->hd = (void *)dsc;
											      _temp954->tl = _temp930;
											      _temp954; }));
	_temp953; });
      _temp952; });
    return Cyc_Tcpat_build_desc(_temp939,(void *)td,_temp925);
  }
  _LL901: ;
}
static void *Cyc_Tcpat_match(void *p,struct Cyc_List_List *obj,void *dsc,
			     struct Cyc_List_List *ctx,struct Cyc_List_List *work,
			     void *right_hand_side,struct Cyc_List_List *rules);
static void *Cyc_Tcpat_or_match(void *dsc,struct Cyc_List_List *allmrules) {
  struct Cyc_List_List *_temp955 = allmrules;
  struct Cyc_List_List _temp961;
  struct Cyc_List_List *_temp962;
  struct _tuple0 *_temp964;
  struct _tuple0 _temp966;
  void *_temp967;
  void *_temp969;
  _LL957: if (_temp955 == 0) {
    goto _LL958;
  }
  else {
    goto _LL959;
  }
  _LL959: if (_temp955 == 0) {
    goto _LL956;
  }
  else {
    _temp961 = *_temp955;
    _LL965: _temp964 = (struct _tuple0 *)_temp961.hd;
	    _temp966 = *_temp964;
	    _LL970: _temp969 = _temp966.f1;
		    goto _LL968;
	    _LL968: _temp967 = _temp966.f2;
		    goto _LL963;
    _LL963: _temp962 = (struct Cyc_List_List *)_temp961.tl;
	    goto _LL960;
  }
  _LL958: return (void *)Cyc_Tcpat_Failure;
  _LL960: return ((void *(*)(void *p,struct Cyc_List_List *obj,void *dsc,
			     struct Cyc_List_List *ctx,struct Cyc_List_List *work,
			     void *right_hand_side,struct Cyc_List_List *rules))Cyc_Tcpat_match)(_temp969,
												 0,
												 dsc,
												 0,
												 0,
												 _temp967,
												 _temp962);
  _LL956: ;
}
static void *Cyc_Tcpat_match_compile(struct Cyc_List_List *allmrules) {
  return ((void *(*)(void *dsc,struct Cyc_List_List *allmrules))Cyc_Tcpat_or_match)((void *)({ struct Cyc_Tcpat_Neg_struct *_temp971 = (struct Cyc_Tcpat_Neg_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Neg_struct));
										      _temp971[0] = ({ struct Cyc_Tcpat_Neg_struct _temp972;
											_temp972.tag = Cyc_Tcpat_Neg;
											_temp972.f1 = Cyc_Tcpat_empty_con_set();
											_temp972; });
										      _temp971; }),
										    allmrules);
}
static void *Cyc_Tcpat_and_match(struct Cyc_List_List *ctx,struct Cyc_List_List *work,
				 void *right_hand_side,struct Cyc_List_List *rules) {
  struct Cyc_List_List *_temp973 = work;
  struct Cyc_List_List _temp981;
  struct Cyc_List_List *_temp982;
  struct _tuple9 *_temp984;
  struct _tuple9 _temp986;
  struct Cyc_List_List *_temp987;
  struct Cyc_List_List *_temp989;
  struct Cyc_List_List *_temp991;
  struct Cyc_List_List _temp993;
  struct Cyc_List_List *_temp994;
  struct _tuple9 *_temp996;
  struct _tuple9 _temp998;
  struct Cyc_List_List *_temp999;
  struct Cyc_List_List *_temp1001;
  struct Cyc_List_List *_temp1003;
  _LL975: if (_temp973 == 0) {
    goto _LL976;
  }
  else {
    goto _LL977;
  }
  _LL977: if (_temp973 == 0) {
    goto _LL979;
  }
  else {
    _temp981 = *_temp973;
    _LL985: _temp984 = (struct _tuple9 *)_temp981.hd;
	    _temp986 = *_temp984;
	    _LL992: _temp991 = _temp986.f1;
		    if (_temp991 == 0) {
		      goto _LL990;
		    }
		    else {
		      goto _LL979;
		    }
	    _LL990: _temp989 = _temp986.f2;
		    if (_temp989 == 0) {
		      goto _LL988;
		    }
		    else {
		      goto _LL979;
		    }
	    _LL988: _temp987 = _temp986.f3;
		    if (_temp987 == 0) {
		      goto _LL983;
		    }
		    else {
		      goto _LL979;
		    }
    _LL983: _temp982 = (struct Cyc_List_List *)_temp981.tl;
	    goto _LL978;
  }
  _LL979: if (_temp973 == 0) {
    goto _LL974;
  }
  else {
    _temp993 = *_temp973;
    _LL997: _temp996 = (struct _tuple9 *)_temp993.hd;
	    _temp998 = *_temp996;
	    _LL1004: _temp1003 = _temp998.f1;
		     goto _LL1002;
	    _LL1002: _temp1001 = _temp998.f2;
		     goto _LL1000;
	    _LL1000: _temp999 = _temp998.f3;
		     goto _LL995;
    _LL995: _temp994 = (struct Cyc_List_List *)_temp993.tl;
	    goto _LL980;
  }
  _LL976: return (void *)({ struct Cyc_Tcpat_Success_struct *_temp1005 = (struct Cyc_Tcpat_Success_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Success_struct));
    _temp1005[0] = ({ struct Cyc_Tcpat_Success_struct _temp1006;
      _temp1006.tag = Cyc_Tcpat_Success;
      _temp1006.f1 = (void *)right_hand_side;
      _temp1006; });
    _temp1005; });
  _LL978: return ((void *(*)(struct Cyc_List_List *ctx,struct Cyc_List_List *work,
			     void *right_hand_side,struct Cyc_List_List *rules))Cyc_Tcpat_and_match)(Cyc_Tcpat_norm_context(ctx),
												     _temp982,
												     right_hand_side,
												     rules);
  _LL980: if ((_temp1003 == 0 ? 1 : _temp1001 == 0) ? 1 : _temp999 == 0) {
    return ((void *(*)(struct _tagged_string))Cyc_Tcutil_impos)((struct _tagged_string)({ char *_temp1007 = (char *)"tcpat:and_match: malformed work frame";
								  struct _tagged_string _temp1008;
								  _temp1008.curr = _temp1007;
								  _temp1008.base = _temp1007;
								  _temp1008.last_plus_one = _temp1007 + 38;
								  _temp1008; }));
  }
  { struct Cyc_List_List _temp1012;
    struct Cyc_List_List *_temp1013;
    void *_temp1015;
    struct Cyc_List_List *_temp1010 = (struct Cyc_List_List *)({ struct Cyc_List_List *_temp1009 = _temp1003;
      if (_temp1009 == 0) {
	_throw(Null_Exception);
      }
      _temp1009; });
    _temp1012 = *_temp1010;
    _LL1016: _temp1015 = (void *)_temp1012.hd;
	     goto _LL1014;
    _LL1014: _temp1013 = (struct Cyc_List_List *)_temp1012.tl;
	     goto _LL1011;
    _LL1011: {
    struct Cyc_List_List _temp1020;
      struct Cyc_List_List *_temp1021;
      struct Cyc_List_List *_temp1023;
      struct Cyc_List_List *_temp1018 = (struct Cyc_List_List *)({ struct Cyc_List_List *_temp1017 = _temp1001;
	if (_temp1017 == 0) {
	  _throw(Null_Exception);
	}
	_temp1017; });
      _temp1020 = *_temp1018;
      _LL1024: _temp1023 = (struct Cyc_List_List *)_temp1020.hd;
	       goto _LL1022;
      _LL1022: _temp1021 = (struct Cyc_List_List *)_temp1020.tl;
	       goto _LL1019;
      _LL1019: {
      struct Cyc_List_List _temp1028;
	struct Cyc_List_List *_temp1029;
	void *_temp1031;
	struct Cyc_List_List *_temp1026 = (struct Cyc_List_List *)({ struct Cyc_List_List *_temp1025 = _temp999;
	  if (_temp1025 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp1025; });
	_temp1028 = *_temp1026;
	_LL1032: _temp1031 = (void *)_temp1028.hd;
		 goto _LL1030;
	_LL1030: _temp1029 = (struct Cyc_List_List *)_temp1028.tl;
		 goto _LL1027;
	_LL1027: {
	struct _tuple9 *wf = ({ struct _tuple9 *_temp1034 = (struct _tuple9 *)GC_malloc(sizeof(struct _tuple9));
	    _temp1034->f1 = _temp1013;
	    _temp1034->f2 = _temp1021;
	    _temp1034->f3 = _temp1029;
	    _temp1034; });
	  return ((void *(*)(void *p,struct Cyc_List_List *obj,void *dsc,
			     struct Cyc_List_List *ctx,struct Cyc_List_List *work,
			     void *right_hand_side,struct Cyc_List_List *rules))Cyc_Tcpat_match)(_temp1015,
												 _temp1023,
												 _temp1031,
												 ctx,
												 ({ struct Cyc_List_List *_temp1033 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
												   _temp1033->hd = (void *)wf;
												   _temp1033->tl = _temp994;
												   _temp1033; }),
												 right_hand_side,
												 rules);
	}
      }
    }}
  
  _LL974: ;
}
static struct Cyc_List_List *Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s *pcon,
						void *dsc) {
  void *_temp1035 = dsc;
  struct Cyc_Set_Set *_temp1041;
  struct Cyc_List_List *_temp1043;
  struct Cyc_Tcpat_Con_s *_temp1045;
  _LL1037: if (*((int *)_temp1035) == Cyc_Tcpat_Neg) {
    _LL1042: _temp1041 = (struct Cyc_Set_Set *)((struct Cyc_Tcpat_Neg_struct *)_temp1035)->f1;
	     goto _LL1038;
  }
  else {
    goto _LL1039;
  }
  _LL1039: if (*((int *)_temp1035) == Cyc_Tcpat_Pos) {
    _LL1046: _temp1045 = (struct Cyc_Tcpat_Con_s *)((struct Cyc_Tcpat_Pos_struct *)_temp1035)->f1;
	     goto _LL1044;
    _LL1044: _temp1043 = (struct Cyc_List_List *)((struct Cyc_Tcpat_Pos_struct *)_temp1035)->f2;
	     goto _LL1040;
  }
  else {
    goto _LL1036;
  }
  _LL1038: {
  void *any = (void *)({ struct Cyc_Tcpat_Neg_struct *_temp1048 = (struct Cyc_Tcpat_Neg_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_Neg_struct));
      _temp1048[0] = ({ struct Cyc_Tcpat_Neg_struct _temp1049;
	_temp1049.tag = Cyc_Tcpat_Neg;
	_temp1049.f1 = Cyc_Tcpat_empty_con_set();
	_temp1049; });
      _temp1048; });
    struct Cyc_List_List *res = 0;
    { int i = 0;
      for(0; i < pcon->arity; ++i) {
	res = ({ struct Cyc_List_List *_temp1047 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
	  _temp1047->hd = (void *)any;
	  _temp1047->tl = res;
	  _temp1047; });
      }
    }
    return res;
  }
  _LL1040: return _temp1043;
  _LL1036: ;
}
static struct Cyc_List_List *Cyc_Tcpat_getoarg(struct Cyc_List_List *obj,
					       int i) {
  return ({ struct Cyc_List_List *_temp1050 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
    _temp1050->hd = (void *)(i + 1);
    _temp1050->tl = obj;
    _temp1050; });
}
static struct Cyc_List_List *Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s *pcon,
						struct Cyc_List_List *obj) {
  return ((struct Cyc_List_List *(*)(int n,struct Cyc_List_List *(*f)(struct Cyc_List_List *,
								      int),
				     struct Cyc_List_List *env))Cyc_List_tabulate_c)(pcon->arity,
										     Cyc_Tcpat_getoarg,
										     obj);
}
static void *Cyc_Tcpat_match(void *p,struct Cyc_List_List *obj,void *dsc,
			     struct Cyc_List_List *ctx,struct Cyc_List_List *work,
			     void *right_hand_side,struct Cyc_List_List *rules) {
  void *_temp1051 = p;
  struct Cyc_List_List *_temp1057;
  struct Cyc_Tcpat_Con_s *_temp1059;
  _LL1053: if (_temp1051 == (void *)Cyc_Tcpat_Any) {
    goto _LL1054;
  }
  else {
    goto _LL1055;
  }
  _LL1055: if ((unsigned int)_temp1051 > 1 ? *((int *)_temp1051) == Cyc_Tcpat_Con : 0) {
    _LL1060: _temp1059 = (struct Cyc_Tcpat_Con_s *)((struct Cyc_Tcpat_Con_struct *)_temp1051)->f1;
	     goto _LL1058;
    _LL1058: _temp1057 = (struct Cyc_List_List *)((struct Cyc_Tcpat_Con_struct *)_temp1051)->f2;
	     goto _LL1056;
  }
  else {
    goto _LL1052;
  }
  _LL1054: return ((void *(*)(struct Cyc_List_List *ctx,struct Cyc_List_List *work,
			      void *right_hand_side,struct Cyc_List_List *rules))Cyc_Tcpat_and_match)(Cyc_Tcpat_augment(ctx,
															dsc),
												      work,
												      right_hand_side,
												      rules);
  _LL1056: {
  void *_temp1061 = Cyc_Tcpat_static_match(_temp1059,dsc);
    _LL1063: if (_temp1061 == (void *)Cyc_Tcpat_Yes) {
      goto _LL1064;
    }
    else {
      goto _LL1065;
    }
    _LL1065: if (_temp1061 == (void *)Cyc_Tcpat_No) {
      goto _LL1066;
    }
    else {
      goto _LL1067;
    }
    _LL1067: if (_temp1061 == (void *)Cyc_Tcpat_Maybe) {
      goto _LL1068;
    }
    else {
      goto _LL1062;
    }
    _LL1064: {
    struct Cyc_List_List *ctx2 = ({ struct Cyc_List_List *_temp1071 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
	_temp1071->hd = (void *)({ struct _tuple10 *_temp1072 = (struct _tuple10 *)GC_malloc(sizeof(struct _tuple10));
	  _temp1072->f1 = _temp1059;
	  _temp1072->f2 = 0;
	  _temp1072; });
	_temp1071->tl = ctx;
	_temp1071; });
      struct _tuple9 *work_frame = ({ struct _tuple9 *_temp1070 = (struct _tuple9 *)GC_malloc(sizeof(struct _tuple9));
	_temp1070->f1 = _temp1057;
	_temp1070->f2 = Cyc_Tcpat_getoargs(_temp1059,obj);
	_temp1070->f3 = Cyc_Tcpat_getdargs(_temp1059,dsc);
	_temp1070; });
      struct Cyc_List_List *work2 = ({ struct Cyc_List_List *_temp1069 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
	_temp1069->hd = (void *)work_frame;
	_temp1069->tl = work;
	_temp1069; });
      return ((void *(*)(struct Cyc_List_List *ctx,struct Cyc_List_List *work,
			 void *right_hand_side,struct Cyc_List_List *rules))Cyc_Tcpat_and_match)(ctx2,
												 work2,
												 right_hand_side,
												 rules);
    }
    _LL1066: return ((void *(*)(void *dsc,struct Cyc_List_List *allmrules))Cyc_Tcpat_or_match)(Cyc_Tcpat_build_desc(ctx,
														    dsc,
														    work),
											       rules);
    _LL1068: {
    struct Cyc_List_List *ctx2 = ({ struct Cyc_List_List *_temp1077 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
	_temp1077->hd = (void *)({ struct _tuple10 *_temp1078 = (struct _tuple10 *)GC_malloc(sizeof(struct _tuple10));
	  _temp1078->f1 = _temp1059;
	  _temp1078->f2 = 0;
	  _temp1078; });
	_temp1077->tl = ctx;
	_temp1077; });
      struct _tuple9 *work_frame = ({ struct _tuple9 *_temp1076 = (struct _tuple9 *)GC_malloc(sizeof(struct _tuple9));
	_temp1076->f1 = _temp1057;
	_temp1076->f2 = Cyc_Tcpat_getoargs(_temp1059,obj);
	_temp1076->f3 = Cyc_Tcpat_getdargs(_temp1059,dsc);
	_temp1076; });
      struct Cyc_List_List *work2 = ({ struct Cyc_List_List *_temp1075 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
	_temp1075->hd = (void *)work_frame;
	_temp1075->tl = work;
	_temp1075; });
      void *s = ((void *(*)(struct Cyc_List_List *ctx,struct Cyc_List_List *work,
			    void *right_hand_side,struct Cyc_List_List *rules))Cyc_Tcpat_and_match)(ctx2,
												    work2,
												    right_hand_side,
												    rules);
      void *f = ((void *(*)(void *dsc,struct Cyc_List_List *allmrules))Cyc_Tcpat_or_match)(Cyc_Tcpat_build_desc(ctx,
														Cyc_Tcpat_add_neg(dsc,
																  _temp1059),
														work),
											   rules);
      return (void *)({ struct Cyc_Tcpat_IfEq_struct *_temp1073 = (struct Cyc_Tcpat_IfEq_struct *)GC_malloc(sizeof(struct Cyc_Tcpat_IfEq_struct));
	_temp1073[0] = ({ struct Cyc_Tcpat_IfEq_struct _temp1074;
	  _temp1074.tag = Cyc_Tcpat_IfEq;
	  _temp1074.f1 = obj;
	  _temp1074.f2 = _temp1059;
	  _temp1074.f3 = (void *)s;
	  _temp1074.f4 = (void *)f;
	  _temp1074; });
	_temp1073; });
    }
    _LL1062: ;
  }
  _LL1052: ;
}
static void Cyc_Tcpat_check_exhaust_overlap(void *d,void (*not_exhaust)(void *),
					    void *env1,void (*rhs_appears)(void *,
									   void *),
					    void *env2) {
  void *_temp1079 = d;
  void *_temp1087;
  void *_temp1089;
  void *_temp1091;
  struct Cyc_Tcpat_Con_s *_temp1093;
  struct Cyc_List_List *_temp1095;
  _LL1081: if (_temp1079 == (void *)Cyc_Tcpat_Failure) {
    goto _LL1082;
  }
  else {
    goto _LL1083;
  }
  _LL1083: if ((unsigned int)_temp1079 > 1 ? *((int *)_temp1079) == Cyc_Tcpat_Success : 0) {
    _LL1088: _temp1087 = (void *)((struct Cyc_Tcpat_Success_struct *)_temp1079)->f1;
	     goto _LL1084;
  }
  else {
    goto _LL1085;
  }
  _LL1085: if ((unsigned int)_temp1079 > 1 ? *((int *)_temp1079) == Cyc_Tcpat_IfEq : 0) {
    _LL1096: _temp1095 = (struct Cyc_List_List *)((struct Cyc_Tcpat_IfEq_struct *)_temp1079)->f1;
	     goto _LL1094;
    _LL1094: _temp1093 = (struct Cyc_Tcpat_Con_s *)((struct Cyc_Tcpat_IfEq_struct *)_temp1079)->f2;
	     goto _LL1092;
    _LL1092: _temp1091 = (void *)((struct Cyc_Tcpat_IfEq_struct *)_temp1079)->f3;
	     goto _LL1090;
    _LL1090: _temp1089 = (void *)((struct Cyc_Tcpat_IfEq_struct *)_temp1079)->f4;
	     goto _LL1086;
  }
  else {
    goto _LL1080;
  }
  _LL1082: not_exhaust(env1);
	   goto _LL1080;
  _LL1084: rhs_appears(env2,_temp1087);
	   goto _LL1080;
  _LL1086: ((void (*)(void *d,void (*not_exhaust)(void *),void *env1,void (*rhs_appears)(void *,
											 void *),
		      void *env2))Cyc_Tcpat_check_exhaust_overlap)(_temp1091,
								   not_exhaust,
								   env1,rhs_appears,
								   env2);
	   ((void (*)(void *d,void (*not_exhaust)(void *),void *env1,void (*rhs_appears)(void *,
											 void *),
		      void *env2))Cyc_Tcpat_check_exhaust_overlap)(_temp1089,
								   not_exhaust,
								   env1,rhs_appears,
								   env2);
	   goto _LL1080;
  _LL1080: ;
}
struct _tuple11 {
  int f1;
  struct Cyc_Position_Segment *f2;
};
struct _tuple12 {
  void *f1;
  struct _tuple11 *f2;
};
static struct _tuple12 *Cyc_Tcpat_get_match(int *r,struct Cyc_Absyn_Switch_clause *swc) {
  void *sp0 = Cyc_Tcpat_compile_pat(swc->pattern);
  struct _tuple11 *rhs = ({ struct _tuple11 *_temp1102 = (struct _tuple11 *)GC_malloc(sizeof(struct _tuple11));
    _temp1102->f1 = 0;
    _temp1102->f2 = (swc->pattern)->loc;
    _temp1102; });
  void *sp;
  if (swc->where_clause != 0) {
    sp = Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List *_temp1097 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
			       _temp1097->hd = (void *)sp0;
			       _temp1097->tl = ({ struct Cyc_List_List *_temp1098 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
				 _temp1098->hd = (void *)Cyc_Tcpat_int_pat(*r);
				 _temp1098->tl = 0;
				 _temp1098; });
			       _temp1097; }));
    *r = *r + 1;
  }
  else {
    sp = Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List *_temp1099 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
			       _temp1099->hd = (void *)sp0;
			       _temp1099->tl = ({ struct Cyc_List_List *_temp1100 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
				 _temp1100->hd = (void *)((void *)Cyc_Tcpat_Any);
				 _temp1100->tl = 0;
				 _temp1100; });
			       _temp1099; }));
  }
  return ({ struct _tuple12 *_temp1101 = (struct _tuple12 *)GC_malloc(sizeof(struct _tuple12));
    _temp1101->f1 = sp;
    _temp1101->f2 = rhs;
    _temp1101; });
}
static void Cyc_Tcpat_not_exhaust_err(struct Cyc_Position_Segment *loc) {
  Cyc_Tcutil_terr(loc,(struct _tagged_string)({ char *_temp1103 = (char *)"patterns may not be exhaustive";
		    struct _tagged_string _temp1104;
		    _temp1104.curr = _temp1103;
		    _temp1104.base = _temp1103;
		    _temp1104.last_plus_one = _temp1103 + 31;
		    _temp1104; }));
}
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple11 *rhs) {
  (*rhs).f1 = 1;
}
void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment *loc,struct Cyc_List_List *swcs) {
  int where_ctr = 0;
  struct Cyc_List_List *match_rules = ((struct Cyc_List_List *(*)(struct _tuple12 *(*f)(int *,
											struct Cyc_Absyn_Switch_clause *),
								  int *env,
								  struct Cyc_List_List *x))Cyc_List_map_c)(Cyc_Tcpat_get_match,
													   &where_ctr,
													   swcs);
  void *dec_tree = ((void *(*)(struct Cyc_List_List *allmrules))Cyc_Tcpat_match_compile)(match_rules);
  ((void (*)(void *d,void (*not_exhaust)(struct Cyc_Position_Segment *),struct Cyc_Position_Segment *env1,
	     void (*rhs_appears)(int,struct _tuple11 *),int env2))Cyc_Tcpat_check_exhaust_overlap)(dec_tree,
												   Cyc_Tcpat_not_exhaust_err,
												   loc,
												   Cyc_Tcpat_rule_occurs,
												   0);
  for(0; match_rules != 0; match_rules = ({ struct Cyc_List_List *_temp1105 = match_rules;
    if (_temp1105 == 0) {
      _throw(Null_Exception);
    }
    _temp1105->tl; })) {
    struct _tuple12 _temp1109;
    struct _tuple11 *_temp1110;
    struct _tuple11 _temp1112;
    struct Cyc_Position_Segment *_temp1113;
    int _temp1115;
    void *_temp1117;
    struct _tuple12 *_temp1107 = (struct _tuple12 *)({ struct Cyc_List_List *_temp1106 = match_rules;
      if (_temp1106 == 0) {
	_throw(Null_Exception);
      }
      _temp1106->hd; });
    _temp1109 = *_temp1107;
    _LL1118: _temp1117 = _temp1109.f1;
	     goto _LL1111;
    _LL1111: _temp1110 = _temp1109.f2;
	     _temp1112 = *_temp1110;
	     _LL1116: _temp1115 = _temp1112.f1;
		      goto _LL1114;
	     _LL1114: _temp1113 = _temp1112.f2;
		      goto _LL1108;
    _LL1108: if (! _temp1115) {
      Cyc_Tcutil_terr(_temp1113,(struct _tagged_string)({ char *_temp1119 = (char *)"redundant pattern";
			struct _tagged_string _temp1120;
			_temp1120.curr = _temp1119;
			_temp1120.base = _temp1119;
			_temp1120.last_plus_one = _temp1119 + 18;
			_temp1120; }));
    }
  }
}
struct _tuple13 {
  struct Cyc_Position_Segment *f1;
  int f2;
};
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple13 *pr) {
  (*pr).f2 = 0;
  Cyc_Tcutil_warn((*pr).f1,(struct _tagged_string)({ char *_temp1121 = (char *)"pattern not exhaustive";
		    struct _tagged_string _temp1122;
		    _temp1122.curr = _temp1121;
		    _temp1122.base = _temp1121;
		    _temp1122.last_plus_one = _temp1121 + 23;
		    _temp1122; }));
}
static void Cyc_Tcpat_dummy_fn(int i,int j) {
  return;
}
struct _tuple14 {
  void *f1;
  int f2;
};
int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment *loc,struct Cyc_Absyn_Pat *p) {
  struct Cyc_List_List *match_rules = ({ struct Cyc_List_List *_temp1124 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List));
    _temp1124->hd = (void *)({ struct _tuple14 *_temp1125 = (struct _tuple14 *)GC_malloc(sizeof(struct _tuple14));
      _temp1125->f1 = Cyc_Tcpat_compile_pat(p);
      _temp1125->f2 = 0;
      _temp1125; });
    _temp1124->tl = 0;
    _temp1124; });
  void *dec_tree = ((void *(*)(struct Cyc_List_List *allmrules))Cyc_Tcpat_match_compile)(match_rules);
  struct _tuple13 *exhaust_env = ({ struct _tuple13 *_temp1123 = (struct _tuple13 *)GC_malloc(sizeof(struct _tuple13));
    _temp1123->f1 = loc;
    _temp1123->f2 = 1;
    _temp1123; });
  ((void (*)(void *d,void (*not_exhaust)(struct _tuple13 *),struct _tuple13 *env1,
	     void (*rhs_appears)(int,int),int env2))Cyc_Tcpat_check_exhaust_overlap)(dec_tree,
										     Cyc_Tcpat_not_exhaust_warn,
										     exhaust_env,
										     Cyc_Tcpat_dummy_fn,
										     0);
  return (*exhaust_env).f2;
}
static struct _tuple12 *Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause *swc) {
  void *sp0 = Cyc_Tcpat_compile_pat(swc->pattern);
  struct _tuple11 *rhs = ({ struct _tuple11 *_temp1127 = (struct _tuple11 *)GC_malloc(sizeof(struct _tuple11));
    _temp1127->f1 = 0;
    _temp1127->f2 = (swc->pattern)->loc;
    _temp1127; });
  return ({ struct _tuple12 *_temp1126 = (struct _tuple12 *)GC_malloc(sizeof(struct _tuple12));
    _temp1126->f1 = sp0;
    _temp1126->f2 = rhs;
    _temp1126; });
}
static void Cyc_Tcpat_not_exhaust_err2(struct Cyc_Position_Segment *loc) {
  ;
}
void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment *loc,struct Cyc_List_List *swcs) {
  struct Cyc_List_List *match_rules = ((struct Cyc_List_List *(*)(struct _tuple12 *(*f)(struct Cyc_Absyn_Switch_clause *),
								  struct Cyc_List_List *x))Cyc_List_map)(Cyc_Tcpat_get_match2,
													 swcs);
  void *dec_tree = ((void *(*)(struct Cyc_List_List *allmrules))Cyc_Tcpat_match_compile)(match_rules);
  ((void (*)(void *d,void (*not_exhaust)(struct Cyc_Position_Segment *),struct Cyc_Position_Segment *env1,
	     void (*rhs_appears)(int,struct _tuple11 *),int env2))Cyc_Tcpat_check_exhaust_overlap)(dec_tree,
												   Cyc_Tcpat_not_exhaust_err2,
												   loc,
												   Cyc_Tcpat_rule_occurs,
												   0);
  for(0; match_rules != 0; match_rules = ({ struct Cyc_List_List *_temp1128 = match_rules;
    if (_temp1128 == 0) {
      _throw(Null_Exception);
    }
    _temp1128->tl; })) {
    struct _tuple12 _temp1132;
    struct _tuple11 *_temp1133;
    struct _tuple11 _temp1135;
    struct Cyc_Position_Segment *_temp1136;
    int _temp1138;
    void *_temp1140;
    struct _tuple12 *_temp1130 = (struct _tuple12 *)({ struct Cyc_List_List *_temp1129 = match_rules;
      if (_temp1129 == 0) {
	_throw(Null_Exception);
      }
      _temp1129->hd; });
    _temp1132 = *_temp1130;
    _LL1141: _temp1140 = _temp1132.f1;
	     goto _LL1134;
    _LL1134: _temp1133 = _temp1132.f2;
	     _temp1135 = *_temp1133;
	     _LL1139: _temp1138 = _temp1135.f1;
		      goto _LL1137;
	     _LL1137: _temp1136 = _temp1135.f2;
		      goto _LL1131;
    _LL1131: if (! _temp1138) {
      Cyc_Tcutil_terr(_temp1136,(struct _tagged_string)({ char *_temp1142 = (char *)"redundant pattern";
			struct _tagged_string _temp1143;
			_temp1143.curr = _temp1142;
			_temp1143.base = _temp1142;
			_temp1143.last_plus_one = _temp1142 + 18;
			_temp1143; }));
    }
  }
}
