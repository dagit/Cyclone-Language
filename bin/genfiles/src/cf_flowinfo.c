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
extern int Cyc_Core_intcmp(int,int);
extern int Cyc_Core_ptrcmp(void **,void **);
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
extern char Cyc_List_List_empty[15];
extern struct Cyc_List_List *Cyc_List_tl(struct Cyc_List_List *x);
extern char Cyc_List_List_mismatch[18];
extern char Cyc_List_Nth[8];
extern int Cyc_List_mem(int (*compare)(void *,void *),struct Cyc_List_List *l,
			void *x);
extern int Cyc_List_list_cmp(int (*cmp)(void *,void *),struct Cyc_List_List *l1,
			     struct Cyc_List_List *l2);
struct Cyc_Set_Set;
typedef struct Cyc_Set_Set *Cyc_Set_gset_t;
typedef struct Cyc_Set_Set *Cyc_Set_hset_t;
typedef struct Cyc_Set_Set *Cyc_Set_set_t;
extern struct Cyc_Set_Set *Cyc_Set_empty(int (*comp)(void *,void *));
extern struct Cyc_Set_Set *Cyc_Set_insert(struct Cyc_Set_Set *s,void *elt);
extern struct Cyc_Set_Set *Cyc_Set_delete(struct Cyc_Set_Set *s,void *elt);
extern int Cyc_Set_is_empty(struct Cyc_Set_Set *s);
extern char Cyc_Set_Absent[11];
extern void *Cyc_Set_choose(struct Cyc_Set_Set *s);
struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict *Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict *Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];
extern struct Cyc_Dict_Dict *Cyc_Dict_empty(int (*comp)(void *,void *));
extern struct Cyc_Dict_Dict *Cyc_Dict_insert(struct Cyc_Dict_Dict *d,void *key,
					     void *data);
extern void *Cyc_Dict_lookup(struct Cyc_Dict_Dict *d,void *key);
extern void Cyc_Dict_iter_c(void (*f)(void *,void *,void *),void *env,struct Cyc_Dict_Dict *d);
extern struct Cyc_Dict_Dict *Cyc_Dict_map_c(void *(*f)(void *,void *),void *env,
					    struct Cyc_Dict_Dict *d);
extern struct Cyc_Dict_Dict *Cyc_Dict_intersect_c(void *(*f)(void *,void *,
							     void *),void *env,
						  struct Cyc_Dict_Dict *d1,
						  struct Cyc_Dict_Dict *d2);
extern int Cyc_Dict_forall_c(int (*f)(void *,void *,void *),void *env,struct Cyc_Dict_Dict *d);
extern int Cyc_Dict_forall_intersect(int (*f)(void *,void *,void *),struct Cyc_Dict_Dict *d1,
				     struct Cyc_Dict_Dict *d2);
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
extern void *Cyc_CfFlowInfo_mkLeafPI(void *esc,void *il);
extern int Cyc_CfFlowInfo_local_root_cmp(void *,void *);
extern int Cyc_CfFlowInfo_field_cmp(void *,void *);
extern int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place *,struct Cyc_CfFlowInfo_Place *);
extern void *Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(void *t);
extern void *Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict *d,struct Cyc_CfFlowInfo_Place *place);
extern struct Cyc_Dict_Dict *Cyc_CfFlowInfo_insert_place(struct Cyc_Dict_Dict *d,
							 struct Cyc_CfFlowInfo_Place *place,
							 void *pinfo);
extern int Cyc_CfFlowInfo_isAllInit(struct Cyc_Dict_Dict *pinfo_dict,void *pinfo);
extern struct Cyc_Dict_Dict *Cyc_CfFlowInfo_escape_pointsto(void *pinfo,struct Cyc_Dict_Dict *d);
extern void *Cyc_CfFlowInfo_assign_unknown_dict(void *,void *);
extern void *Cyc_CfFlowInfo_join_flow(void *,void *);
extern int Cyc_CfFlowInfo_flow_lessthan_approx(void *f1,void *f2);
static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_noneil = {0,
										      (void *)((void *)1),
										      (void *)((void *)0)};
static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_thisil = {0,
										      (void *)((void *)1),
										      (void *)((void *)1)};
static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_allil = {0,
										     (void *)((void *)1),
										     (void *)((void *)2)};
static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_esc_noneil = {0,
										     (void *)((void *)0),
										     (void *)((void *)0)};
static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_esc_thisil = {0,
										     (void *)((void *)0),
										     (void *)((void *)1)};
static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_esc_allil = {0,
										    (void *)((void *)0),
										    (void *)((void *)2)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_none_v = {0,
									       (void *)((void *)&Cyc_CfFlowInfo_unknownis_unes_noneil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_this_v = {0,
									       (void *)((void *)&Cyc_CfFlowInfo_unknownis_unes_thisil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_all_v = {0,
									      (void *)((void *)&Cyc_CfFlowInfo_unknownis_unes_allil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_none_v = {0,
									     (void *)((void *)&Cyc_CfFlowInfo_unknownis_esc_noneil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_this_v = {0,
									     (void *)((void *)&Cyc_CfFlowInfo_unknownis_esc_thisil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_all_v = {0,
									    (void *)((void *)&Cyc_CfFlowInfo_unknownis_esc_allil)};
void *Cyc_CfFlowInfo_leaf_unesc_none = (void *)&Cyc_CfFlowInfo_leaf_unesc_none_v;
void *Cyc_CfFlowInfo_leaf_unesc_this = (void *)&Cyc_CfFlowInfo_leaf_unesc_this_v;
void *Cyc_CfFlowInfo_leaf_unesc_all = (void *)&Cyc_CfFlowInfo_leaf_unesc_all_v;
void *Cyc_CfFlowInfo_leaf_esc_none = (void *)&Cyc_CfFlowInfo_leaf_esc_none_v;
void *Cyc_CfFlowInfo_leaf_esc_this = (void *)&Cyc_CfFlowInfo_leaf_esc_this_v;
void *Cyc_CfFlowInfo_leaf_esc_all = (void *)&Cyc_CfFlowInfo_leaf_esc_all_v;
struct _tuple3 {
  void *f1;
  void *f2;
};
void *Cyc_CfFlowInfo_mkLeafPI(void *esc,void *il) {
  struct _tuple3 _temp13 = ({ struct _tuple3 _temp12;
    _temp12.f1 = esc;
    _temp12.f2 = il;
    _temp12; });
  void *_temp27;
  void *_temp29;
  void *_temp31;
  void *_temp33;
  void *_temp35;
  void *_temp37;
  void *_temp39;
  void *_temp41;
  void *_temp43;
  void *_temp45;
  void *_temp47;
  void *_temp49;
  _LL15: _LL30: _temp29 = _temp13.f1;
		if (_temp29 == (void *)Cyc_CfFlowInfo_Unesc) {
		  goto _LL28;
		}
		else {
		  goto _LL17;
		}
	 _LL28: _temp27 = _temp13.f2;
		if (_temp27 == (void *)Cyc_CfFlowInfo_NoneIL) {
		  goto _LL16;
		}
		else {
		  goto _LL17;
		}
  _LL17: _LL34: _temp33 = _temp13.f1;
		if (_temp33 == (void *)Cyc_CfFlowInfo_Unesc) {
		  goto _LL32;
		}
		else {
		  goto _LL19;
		}
	 _LL32: _temp31 = _temp13.f2;
		if (_temp31 == (void *)Cyc_CfFlowInfo_ThisIL) {
		  goto _LL18;
		}
		else {
		  goto _LL19;
		}
  _LL19: _LL38: _temp37 = _temp13.f1;
		if (_temp37 == (void *)Cyc_CfFlowInfo_Unesc) {
		  goto _LL36;
		}
		else {
		  goto _LL21;
		}
	 _LL36: _temp35 = _temp13.f2;
		if (_temp35 == (void *)Cyc_CfFlowInfo_AllIL) {
		  goto _LL20;
		}
		else {
		  goto _LL21;
		}
  _LL21: _LL42: _temp41 = _temp13.f1;
		if (_temp41 == (void *)Cyc_CfFlowInfo_Esc) {
		  goto _LL40;
		}
		else {
		  goto _LL23;
		}
	 _LL40: _temp39 = _temp13.f2;
		if (_temp39 == (void *)Cyc_CfFlowInfo_NoneIL) {
		  goto _LL22;
		}
		else {
		  goto _LL23;
		}
  _LL23: _LL46: _temp45 = _temp13.f1;
		if (_temp45 == (void *)Cyc_CfFlowInfo_Esc) {
		  goto _LL44;
		}
		else {
		  goto _LL25;
		}
	 _LL44: _temp43 = _temp13.f2;
		if (_temp43 == (void *)Cyc_CfFlowInfo_ThisIL) {
		  goto _LL24;
		}
		else {
		  goto _LL25;
		}
  _LL25: _LL50: _temp49 = _temp13.f1;
		if (_temp49 == (void *)Cyc_CfFlowInfo_Esc) {
		  goto _LL48;
		}
		else {
		  goto _LL14;
		}
	 _LL48: _temp47 = _temp13.f2;
		if (_temp47 == (void *)Cyc_CfFlowInfo_AllIL) {
		  goto _LL26;
		}
		else {
		  goto _LL14;
		}
  _LL16: return Cyc_CfFlowInfo_leaf_unesc_none;
  _LL18: return Cyc_CfFlowInfo_leaf_unesc_this;
  _LL20: return Cyc_CfFlowInfo_leaf_unesc_all;
  _LL22: return Cyc_CfFlowInfo_leaf_esc_none;
  _LL24: return Cyc_CfFlowInfo_leaf_esc_this;
  _LL26: return Cyc_CfFlowInfo_leaf_esc_all;
  _LL14: ;
}
int Cyc_CfFlowInfo_local_root_cmp(void *r1,void *r2) {
  if (r1 == r2) {
    return 0;
  }
  { struct _tuple3 _temp52 = ({ struct _tuple3 _temp51;
      _temp51.f1 = r1;
      _temp51.f2 = r2;
      _temp51; });
    void *_temp62;
    struct Cyc_Absyn_Exp *_temp64;
    void *_temp66;
    struct Cyc_Absyn_Vardecl *_temp68;
    void *_temp70;
    struct Cyc_Absyn_Vardecl *_temp72;
    void *_temp74;
    struct Cyc_Absyn_Exp *_temp76;
    void *_temp78;
    struct Cyc_Absyn_Vardecl *_temp80;
    void *_temp82;
    struct Cyc_Absyn_Vardecl *_temp84;
    void *_temp86;
    struct Cyc_Absyn_Exp *_temp88;
    void *_temp90;
    struct Cyc_Absyn_Exp *_temp92;
    _LL54: _LL67: _temp66 = _temp52.f1;
		  if (*((int *)_temp66) == Cyc_CfFlowInfo_VarRoot) {
		    _LL69: _temp68 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_CfFlowInfo_VarRoot_struct *)_temp66)->f1;
			   goto _LL63;
		  }
		  else {
		    goto _LL56;
		  }
	   _LL63: _temp62 = _temp52.f2;
		  if (*((int *)_temp62) == Cyc_CfFlowInfo_MallocPt) {
		    _LL65: _temp64 = (struct Cyc_Absyn_Exp *)((struct Cyc_CfFlowInfo_MallocPt_struct *)_temp62)->f1;
			   goto _LL55;
		  }
		  else {
		    goto _LL56;
		  }
    _LL56: _LL75: _temp74 = _temp52.f1;
		  if (*((int *)_temp74) == Cyc_CfFlowInfo_MallocPt) {
		    _LL77: _temp76 = (struct Cyc_Absyn_Exp *)((struct Cyc_CfFlowInfo_MallocPt_struct *)_temp74)->f1;
			   goto _LL71;
		  }
		  else {
		    goto _LL58;
		  }
	   _LL71: _temp70 = _temp52.f2;
		  if (*((int *)_temp70) == Cyc_CfFlowInfo_VarRoot) {
		    _LL73: _temp72 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_CfFlowInfo_VarRoot_struct *)_temp70)->f1;
			   goto _LL57;
		  }
		  else {
		    goto _LL58;
		  }
    _LL58: _LL83: _temp82 = _temp52.f1;
		  if (*((int *)_temp82) == Cyc_CfFlowInfo_VarRoot) {
		    _LL85: _temp84 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_CfFlowInfo_VarRoot_struct *)_temp82)->f1;
			   goto _LL79;
		  }
		  else {
		    goto _LL60;
		  }
	   _LL79: _temp78 = _temp52.f2;
		  if (*((int *)_temp78) == Cyc_CfFlowInfo_VarRoot) {
		    _LL81: _temp80 = (struct Cyc_Absyn_Vardecl *)((struct Cyc_CfFlowInfo_VarRoot_struct *)_temp78)->f1;
			   goto _LL59;
		  }
		  else {
		    goto _LL60;
		  }
    _LL60: _LL91: _temp90 = _temp52.f1;
		  if (*((int *)_temp90) == Cyc_CfFlowInfo_MallocPt) {
		    _LL93: _temp92 = (struct Cyc_Absyn_Exp *)((struct Cyc_CfFlowInfo_MallocPt_struct *)_temp90)->f1;
			   goto _LL87;
		  }
		  else {
		    goto _LL53;
		  }
	   _LL87: _temp86 = _temp52.f2;
		  if (*((int *)_temp86) == Cyc_CfFlowInfo_MallocPt) {
		    _LL89: _temp88 = (struct Cyc_Absyn_Exp *)((struct Cyc_CfFlowInfo_MallocPt_struct *)_temp86)->f1;
			   goto _LL61;
		  }
		  else {
		    goto _LL53;
		  }
    _LL55: return 1;
    _LL57: return - 1;
    _LL59: return ((int (*)(struct Cyc_Absyn_Vardecl *,struct Cyc_Absyn_Vardecl *))Cyc_Core_ptrcmp)(_temp84,
												    _temp80);
    _LL61: return ((int (*)(struct Cyc_Absyn_Exp *,struct Cyc_Absyn_Exp *))Cyc_Core_ptrcmp)(_temp92,
											    _temp88);
    _LL53: ;}
  
}
int Cyc_CfFlowInfo_field_cmp(void *f1,void *f2) {
  if (f1 == f2) {
    return 0;
  }
  { struct _tuple3 _temp95 = ({ struct _tuple3 _temp94;
      _temp94.f1 = f1;
      _temp94.f2 = f2;
      _temp94; });
    void *_temp105;
    int _temp107;
    void *_temp109;
    struct _tagged_string *_temp111;
    void *_temp113;
    struct _tagged_string *_temp115;
    void *_temp117;
    int _temp119;
    void *_temp121;
    struct _tagged_string *_temp123;
    void *_temp125;
    struct _tagged_string *_temp127;
    void *_temp129;
    int _temp131;
    void *_temp133;
    int _temp135;
    _LL97: _LL110: _temp109 = _temp95.f1;
		   if (*((int *)_temp109) == Cyc_CfFlowInfo_StructF) {
		     _LL112: _temp111 = (struct _tagged_string *)((struct Cyc_CfFlowInfo_StructF_struct *)_temp109)->f1;
			     goto _LL106;
		   }
		   else {
		     goto _LL99;
		   }
	   _LL106: _temp105 = _temp95.f2;
		   if (*((int *)_temp105) == Cyc_CfFlowInfo_TupleF) {
		     _LL108: _temp107 = (int)((struct Cyc_CfFlowInfo_TupleF_struct *)_temp105)->f1;
			     goto _LL98;
		   }
		   else {
		     goto _LL99;
		   }
    _LL99: _LL118: _temp117 = _temp95.f1;
		   if (*((int *)_temp117) == Cyc_CfFlowInfo_TupleF) {
		     _LL120: _temp119 = (int)((struct Cyc_CfFlowInfo_TupleF_struct *)_temp117)->f1;
			     goto _LL114;
		   }
		   else {
		     goto _LL101;
		   }
	   _LL114: _temp113 = _temp95.f2;
		   if (*((int *)_temp113) == Cyc_CfFlowInfo_StructF) {
		     _LL116: _temp115 = (struct _tagged_string *)((struct Cyc_CfFlowInfo_StructF_struct *)_temp113)->f1;
			     goto _LL100;
		   }
		   else {
		     goto _LL101;
		   }
    _LL101: _LL126: _temp125 = _temp95.f1;
		    if (*((int *)_temp125) == Cyc_CfFlowInfo_StructF) {
		      _LL128: _temp127 = (struct _tagged_string *)((struct Cyc_CfFlowInfo_StructF_struct *)_temp125)->f1;
			      goto _LL122;
		    }
		    else {
		      goto _LL103;
		    }
	    _LL122: _temp121 = _temp95.f2;
		    if (*((int *)_temp121) == Cyc_CfFlowInfo_StructF) {
		      _LL124: _temp123 = (struct _tagged_string *)((struct Cyc_CfFlowInfo_StructF_struct *)_temp121)->f1;
			      goto _LL102;
		    }
		    else {
		      goto _LL103;
		    }
    _LL103: _LL134: _temp133 = _temp95.f1;
		    if (*((int *)_temp133) == Cyc_CfFlowInfo_TupleF) {
		      _LL136: _temp135 = (int)((struct Cyc_CfFlowInfo_TupleF_struct *)_temp133)->f1;
			      goto _LL130;
		    }
		    else {
		      goto _LL96;
		    }
	    _LL130: _temp129 = _temp95.f2;
		    if (*((int *)_temp129) == Cyc_CfFlowInfo_TupleF) {
		      _LL132: _temp131 = (int)((struct Cyc_CfFlowInfo_TupleF_struct *)_temp129)->f1;
			      goto _LL104;
		    }
		    else {
		      goto _LL96;
		    }
    _LL98: return 1;
    _LL100: return - 1;
    _LL102: return ((int (*)(struct _tagged_string *,struct _tagged_string *))Cyc_Core_ptrcmp)(_temp127,
											       _temp123);
    _LL104: return _temp135 - _temp131;
    _LL96: ;}
  
}
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place *p1,struct Cyc_CfFlowInfo_Place *p2) {
  if (p1 == p2) {
    return 0;
  }
  { int i = Cyc_CfFlowInfo_local_root_cmp((void *)p1->root,(void *)p2->root);
    if (i != 0) {
      return i;
    }
    return ((int (*)(int (*cmp)(void *,void *),struct Cyc_List_List *l1,struct Cyc_List_List *l2))Cyc_List_list_cmp)(Cyc_CfFlowInfo_field_cmp,
														     p1->fields,
														     p2->fields);}
  
}
struct _tuple4 {
  struct Cyc_Absyn_Tqual f1;
  void *f2;
};
void *Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(void *t) {
  void *_temp137 = Cyc_Tcutil_compress(t);
  struct Cyc_List_List *_temp147;
  struct Cyc_List_List *_temp149;
  struct Cyc_Absyn_Structdecl **_temp151;
  struct Cyc_Absyn_Structdecl *_temp153;
  struct Cyc_Absyn_Structdecl _temp154;
  struct Cyc_List_List *_temp155;
  struct Cyc_Core_Opt *_temp157;
  struct Cyc_Core_Opt _temp159;
  struct Cyc_List_List *_temp160;
  struct Cyc_List_List *_temp162;
  struct Cyc_Core_Opt *_temp164;
  void *_temp166;
  struct Cyc_List_List *_temp168;
  struct _tuple0 *_temp170;
  _LL139: if ((unsigned int)_temp137 > 4 ? *((int *)_temp137) == Cyc_Absyn_TupleType : 0) {
    _LL148: _temp147 = (struct Cyc_List_List *)((struct Cyc_Absyn_TupleType_struct *)_temp137)->f1;
	    goto _LL140;
  }
  else {
    goto _LL141;
  }
  _LL141: if ((unsigned int)_temp137 > 4 ? *((int *)_temp137) == Cyc_Absyn_AnonStructType : 0) {
    _LL150: _temp149 = (struct Cyc_List_List *)((struct Cyc_Absyn_AnonStructType_struct *)_temp137)->f1;
	    goto _LL142;
  }
  else {
    goto _LL143;
  }
  _LL143: if ((unsigned int)_temp137 > 4 ? *((int *)_temp137) == Cyc_Absyn_StructType : 0) {
    _LL171: _temp170 = (struct _tuple0 *)((struct Cyc_Absyn_StructType_struct *)_temp137)->f1;
	    goto _LL169;
    _LL169: _temp168 = (struct Cyc_List_List *)((struct Cyc_Absyn_StructType_struct *)_temp137)->f2;
	    goto _LL152;
    _LL152: _temp151 = (struct Cyc_Absyn_Structdecl **)((struct Cyc_Absyn_StructType_struct *)_temp137)->f3;
	    if (_temp151 == 0) {
	      goto _LL145;
	    }
	    else {
	      _temp153 = *_temp151;
	      _temp154 = *_temp153;
	      _LL167: _temp166 = (void *)_temp154.sc;
		      goto _LL165;
	      _LL165: _temp164 = (struct Cyc_Core_Opt *)_temp154.name;
		      goto _LL163;
	      _LL163: _temp162 = (struct Cyc_List_List *)_temp154.tvs;
		      goto _LL158;
	      _LL158: _temp157 = (struct Cyc_Core_Opt *)_temp154.fields;
		      if (_temp157 == 0) {
			goto _LL145;
		      }
		      else {
			_temp159 = *_temp157;
			_LL161: _temp160 = (struct Cyc_List_List *)_temp159.v;
				goto _LL156;
		      }
	      _LL156: _temp155 = (struct Cyc_List_List *)_temp154.attributes;
		      goto _LL144;
	    }
  }
  else {
    goto _LL145;
  }
  _LL145: goto _LL146;
  _LL140: {
  struct Cyc_Dict_Dict *d = ((struct Cyc_Dict_Dict *(*)(int (*comp)(int,int)))Cyc_Dict_empty)(Cyc_Core_intcmp);
    { int i = 0;
      for(0; _temp147 != 0; _temp147 = ({ struct Cyc_List_List *_temp172 = _temp147;
	if (_temp172 == 0) {
	  _throw(Null_Exception);
	}
	_temp172->tl; }), ++i) {
	struct _tuple4 _temp176;
	void *_temp177;
	struct Cyc_Absyn_Tqual _temp179;
	struct _tuple4 *_temp174 = (struct _tuple4 *)({ struct Cyc_List_List *_temp173 = _temp147;
	  if (_temp173 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp173->hd; });
	_temp176 = *_temp174;
	_LL180: _temp179 = _temp176.f1;
		goto _LL178;
	_LL178: _temp177 = _temp176.f2;
		goto _LL175;
	_LL175: d = ((struct Cyc_Dict_Dict *(*)(struct Cyc_Dict_Dict *d,int key,
						void *data))Cyc_Dict_insert)(d,
									     i,
									     Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(_temp177));
      }
    }
    return (void *)({ struct Cyc_CfFlowInfo_TuplePI_struct *_temp181 = (struct Cyc_CfFlowInfo_TuplePI_struct *)GC_malloc(sizeof(struct Cyc_CfFlowInfo_TuplePI_struct));
      _temp181[0] = ({ struct Cyc_CfFlowInfo_TuplePI_struct _temp182;
	_temp182.tag = Cyc_CfFlowInfo_TuplePI;
	_temp182.f1 = d;
	_temp182; });
      _temp181; });
  }
  _LL142: _temp160 = _temp149;
	  goto _LL144;
  _LL144: {
  struct Cyc_Dict_Dict *d = ((struct Cyc_Dict_Dict *(*)(int (*comp)(struct _tagged_string *,
								    struct _tagged_string *)))Cyc_Dict_empty)(Cyc_String_zstrptrcmp);
    for(0; _temp160 != 0; _temp160 = ({ struct Cyc_List_List *_temp183 = _temp160;
      if (_temp183 == 0) {
	_throw(Null_Exception);
      }
      _temp183->tl; })) {
      struct Cyc_Absyn_Structfield _temp187;
      struct Cyc_List_List *_temp188;
      struct Cyc_Core_Opt *_temp190;
      void *_temp192;
      struct Cyc_Absyn_Tqual _temp194;
      struct _tagged_string *_temp196;
      struct Cyc_Absyn_Structfield *_temp185 = (struct Cyc_Absyn_Structfield *)({ struct Cyc_List_List *_temp184 = _temp160;
	if (_temp184 == 0) {
	  _throw(Null_Exception);
	}
	_temp184->hd; });
      _temp187 = *_temp185;
      _LL197: _temp196 = (struct _tagged_string *)_temp187.name;
	      goto _LL195;
      _LL195: _temp194 = (struct Cyc_Absyn_Tqual)_temp187.tq;
	      goto _LL193;
      _LL193: _temp192 = (void *)_temp187.type;
	      goto _LL191;
      _LL191: _temp190 = (struct Cyc_Core_Opt *)_temp187.width;
	      goto _LL189;
      _LL189: _temp188 = (struct Cyc_List_List *)_temp187.attributes;
	      goto _LL186;
      _LL186: if (Cyc_String_strcmp(*_temp196,(struct _tagged_string)({ char *_temp198 = (char *)"";
				      struct _tagged_string _temp199;
				      _temp199.curr = _temp198;
				      _temp199.base = _temp198;
				      _temp199.last_plus_one = _temp198 + 1;
				      _temp199; })) != 0) {
	d = ((struct Cyc_Dict_Dict *(*)(struct Cyc_Dict_Dict *d,struct _tagged_string *key,
					void *data))Cyc_Dict_insert)(d,_temp196,
								     Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(_temp192));
      }
    }
    return (void *)({ struct Cyc_CfFlowInfo_StructPI_struct *_temp200 = (struct Cyc_CfFlowInfo_StructPI_struct *)GC_malloc(sizeof(struct Cyc_CfFlowInfo_StructPI_struct));
      _temp200[0] = ({ struct Cyc_CfFlowInfo_StructPI_struct _temp201;
	_temp201.tag = Cyc_CfFlowInfo_StructPI;
	_temp201.f1 = d;
	_temp201; });
      _temp200; });
  }
  _LL146: return Cyc_CfFlowInfo_leaf_unesc_none;
  _LL138: ;
}
void *Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict *d,struct Cyc_CfFlowInfo_Place *place) {
  struct Cyc_CfFlowInfo_Place _temp204;
  struct Cyc_List_List *_temp205;
  void *_temp207;
  struct Cyc_CfFlowInfo_Place *_temp202 = place;
  _temp204 = *_temp202;
  _LL208: _temp207 = (void *)_temp204.root;
	  goto _LL206;
  _LL206: _temp205 = (struct Cyc_List_List *)_temp204.fields;
	  goto _LL203;
  _LL203: {
  void *ans = ((void *(*)(struct Cyc_Dict_Dict *d,void *key))Cyc_Dict_lookup)(d,
									      _temp207);
    for(0; _temp205 != 0; _temp205 = ({ struct Cyc_List_List *_temp209 = _temp205;
      if (_temp209 == 0) {
	_throw(Null_Exception);
      }
      _temp209->tl; })) {
      struct _tuple3 _temp212 = ({ struct _tuple3 _temp210;
	_temp210.f1 = ans;
	_temp210.f2 = (void *)({ struct Cyc_List_List *_temp211 = _temp205;
	  if (_temp211 == 0) {
	    _throw(Null_Exception);
	  }
	  _temp211->hd; });
	_temp210; });
      void *_temp220;
      int _temp222;
      void *_temp224;
      struct Cyc_Dict_Dict *_temp226;
      void *_temp228;
      struct _tagged_string *_temp230;
      void *_temp232;
      struct Cyc_Dict_Dict *_temp234;
      _LL214: _LL225: _temp224 = _temp212.f1;
		      if (*((int *)_temp224) == Cyc_CfFlowInfo_TuplePI) {
			_LL227: _temp226 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_TuplePI_struct *)_temp224)->f1;
				goto _LL221;
		      }
		      else {
			goto _LL216;
		      }
	      _LL221: _temp220 = _temp212.f2;
		      if (*((int *)_temp220) == Cyc_CfFlowInfo_TupleF) {
			_LL223: _temp222 = (int)((struct Cyc_CfFlowInfo_TupleF_struct *)_temp220)->f1;
				goto _LL215;
		      }
		      else {
			goto _LL216;
		      }
      _LL216: _LL233: _temp232 = _temp212.f1;
		      if (*((int *)_temp232) == Cyc_CfFlowInfo_StructPI) {
			_LL235: _temp234 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_StructPI_struct *)_temp232)->f1;
				goto _LL229;
		      }
		      else {
			goto _LL218;
		      }
	      _LL229: _temp228 = _temp212.f2;
		      if (*((int *)_temp228) == Cyc_CfFlowInfo_StructF) {
			_LL231: _temp230 = (struct _tagged_string *)((struct Cyc_CfFlowInfo_StructF_struct *)_temp228)->f1;
				goto _LL217;
		      }
		      else {
			goto _LL218;
		      }
      _LL218: goto _LL219;
      _LL215: ans = ((void *(*)(struct Cyc_Dict_Dict *d,int key))Cyc_Dict_lookup)(_temp226,
										  _temp222);
	      goto _LL213;
      _LL217: ans = ((void *(*)(struct Cyc_Dict_Dict *d,struct _tagged_string *key))Cyc_Dict_lookup)(_temp234,
												     _temp230);
	      goto _LL213;
      _LL219: (void)_throw((void *)({ struct Cyc_Core_Impossible_struct *_temp236 = (struct Cyc_Core_Impossible_struct *)GC_malloc(sizeof(struct Cyc_Core_Impossible_struct));
			     _temp236[0] = ({ struct Cyc_Core_Impossible_struct _temp237;
			       _temp237.tag = Cyc_Core_Impossible;
			       _temp237.f1 = (struct _tagged_string)({ char *_temp238 = (char *)"bad lookup_place";
				 struct _tagged_string _temp239;
				 _temp239.curr = _temp238;
				 _temp239.base = _temp238;
				 _temp239.last_plus_one = _temp238 + 17;
				 _temp239; });
			       _temp237; });
			     _temp236; }));
      _LL213: ;
    }
    return ans;
  }
}
struct _tuple5 {
  void *f1;
  struct Cyc_List_List *f2;
};
void *Cyc_CfFlowInfo_insert_place_rec(void *insert_path,struct Cyc_List_List *fields,
				      void *pinfo) {
  if (fields == 0) {
    return pinfo;
  }
  { struct _tuple5 _temp241 = ({ struct _tuple5 _temp240;
      _temp240.f1 = insert_path;
      _temp240.f2 = fields;
      _temp240; });
    struct Cyc_List_List *_temp249;
    struct Cyc_List_List _temp251;
    struct Cyc_List_List *_temp252;
    void *_temp254;
    int _temp256;
    void *_temp258;
    struct Cyc_Dict_Dict *_temp260;
    struct Cyc_List_List *_temp262;
    struct Cyc_List_List _temp264;
    struct Cyc_List_List *_temp265;
    void *_temp267;
    struct _tagged_string *_temp269;
    void *_temp271;
    struct Cyc_Dict_Dict *_temp273;
    _LL243: _LL259: _temp258 = _temp241.f1;
		    if (*((int *)_temp258) == Cyc_CfFlowInfo_TuplePI) {
		      _LL261: _temp260 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_TuplePI_struct *)_temp258)->f1;
			      goto _LL250;
		    }
		    else {
		      goto _LL245;
		    }
	    _LL250: _temp249 = _temp241.f2;
		    if (_temp249 == 0) {
		      goto _LL245;
		    }
		    else {
		      _temp251 = *_temp249;
		      _LL255: _temp254 = (void *)_temp251.hd;
			      if (*((int *)_temp254) == Cyc_CfFlowInfo_TupleF) {
				_LL257: _temp256 = (int)((struct Cyc_CfFlowInfo_TupleF_struct *)_temp254)->f1;
					goto _LL253;
			      }
			      else {
				goto _LL245;
			      }
		      _LL253: _temp252 = (struct Cyc_List_List *)_temp251.tl;
			      goto _LL244;
		    }
    _LL245: _LL272: _temp271 = _temp241.f1;
		    if (*((int *)_temp271) == Cyc_CfFlowInfo_StructPI) {
		      _LL274: _temp273 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_StructPI_struct *)_temp271)->f1;
			      goto _LL263;
		    }
		    else {
		      goto _LL247;
		    }
	    _LL263: _temp262 = _temp241.f2;
		    if (_temp262 == 0) {
		      goto _LL247;
		    }
		    else {
		      _temp264 = *_temp262;
		      _LL268: _temp267 = (void *)_temp264.hd;
			      if (*((int *)_temp267) == Cyc_CfFlowInfo_StructF) {
				_LL270: _temp269 = (struct _tagged_string *)((struct Cyc_CfFlowInfo_StructF_struct *)_temp267)->f1;
					goto _LL266;
			      }
			      else {
				goto _LL247;
			      }
		      _LL266: _temp265 = (struct Cyc_List_List *)_temp264.tl;
			      goto _LL246;
		    }
    _LL247: goto _LL248;
    _LL244: {
    void *next_path = ((void *(*)(struct Cyc_Dict_Dict *d,int key))Cyc_Dict_lookup)(_temp260,
										    _temp256);
      return (void *)({ struct Cyc_CfFlowInfo_TuplePI_struct *_temp275 = (struct Cyc_CfFlowInfo_TuplePI_struct *)GC_malloc(sizeof(struct Cyc_CfFlowInfo_TuplePI_struct));
	_temp275[0] = ({ struct Cyc_CfFlowInfo_TuplePI_struct _temp276;
	  _temp276.tag = Cyc_CfFlowInfo_TuplePI;
	  _temp276.f1 = ((struct Cyc_Dict_Dict *(*)(struct Cyc_Dict_Dict *d,
						    int key,void *data))Cyc_Dict_insert)(_temp260,
											 _temp256,
											 Cyc_CfFlowInfo_insert_place_rec(next_path,
															 _temp252,
															 pinfo));
	  _temp276; });
	_temp275; });
    }
    _LL246: {
    void *next_path = ((void *(*)(struct Cyc_Dict_Dict *d,struct _tagged_string *key))Cyc_Dict_lookup)(_temp273,
												       _temp269);
      return (void *)({ struct Cyc_CfFlowInfo_StructPI_struct *_temp277 = (struct Cyc_CfFlowInfo_StructPI_struct *)GC_malloc(sizeof(struct Cyc_CfFlowInfo_StructPI_struct));
	_temp277[0] = ({ struct Cyc_CfFlowInfo_StructPI_struct _temp278;
	  _temp278.tag = Cyc_CfFlowInfo_StructPI;
	  _temp278.f1 = ((struct Cyc_Dict_Dict *(*)(struct Cyc_Dict_Dict *d,
						    struct _tagged_string *key,
						    void *data))Cyc_Dict_insert)(_temp273,
										 _temp269,
										 Cyc_CfFlowInfo_insert_place_rec(next_path,
														 _temp265,
														 pinfo));
	  _temp278; });
	_temp277; });
    }
    _LL248: (void)_throw((void *)({ struct Cyc_Core_Impossible_struct *_temp279 = (struct Cyc_Core_Impossible_struct *)GC_malloc(sizeof(struct Cyc_Core_Impossible_struct));
			   _temp279[0] = ({ struct Cyc_Core_Impossible_struct _temp280;
			     _temp280.tag = Cyc_Core_Impossible;
			     _temp280.f1 = (struct _tagged_string)({ char *_temp281 = (char *)"bad insert_place";
			       struct _tagged_string _temp282;
			       _temp282.curr = _temp281;
			       _temp282.base = _temp281;
			       _temp282.last_plus_one = _temp281 + 17;
			       _temp282; });
			     _temp280; });
			   _temp279; }));
    _LL242: ;}
  
}
struct Cyc_Dict_Dict *Cyc_CfFlowInfo_insert_place(struct Cyc_Dict_Dict *d,
						  struct Cyc_CfFlowInfo_Place *place,
						  void *pinfo) {
  struct Cyc_CfFlowInfo_Place _temp285;
  struct Cyc_List_List *_temp286;
  void *_temp288;
  struct Cyc_CfFlowInfo_Place *_temp283 = place;
  _temp285 = *_temp283;
  _LL289: _temp288 = (void *)_temp285.root;
	  goto _LL287;
  _LL287: _temp286 = (struct Cyc_List_List *)_temp285.fields;
	  goto _LL284;
  _LL284: return ((struct Cyc_Dict_Dict *(*)(struct Cyc_Dict_Dict *d,void *key,
					     void *data))Cyc_Dict_insert)(d,
									  _temp288,
									  Cyc_CfFlowInfo_insert_place_rec(((void *(*)(struct Cyc_Dict_Dict *d,
														      void *key))Cyc_Dict_lookup)(d,
																		  _temp288),
													  _temp286,
													  pinfo));
}
struct _tuple6 {
  struct Cyc_Dict_Dict *f1;
  struct Cyc_List_List *f2;
};
int Cyc_CfFlowInfo_isAllInit_pinfo(struct _tuple6 *env,void *path);
int Cyc_CfFlowInfo_isAllInit_is(struct _tuple6 *env,void *state) {
  void *_temp290 = state;
  void *_temp298;
  void *_temp300;
  void *_temp302;
  void *_temp304;
  struct Cyc_CfFlowInfo_Place *_temp306;
  _LL292: if (*((int *)_temp290) == Cyc_CfFlowInfo_UnknownIS) {
    _LL301: _temp300 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp290)->f1;
	    goto _LL299;
    _LL299: _temp298 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp290)->f2;
	    if (_temp298 == (void *)Cyc_CfFlowInfo_AllIL) {
	      goto _LL293;
	    }
	    else {
	      goto _LL294;
	    }
  }
  else {
    goto _LL294;
  }
  _LL294: if (*((int *)_temp290) == Cyc_CfFlowInfo_UnknownIS) {
    _LL305: _temp304 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp290)->f1;
	    goto _LL303;
    _LL303: _temp302 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp290)->f2;
	    goto _LL295;
  }
  else {
    goto _LL296;
  }
  _LL296: if (*((int *)_temp290) == Cyc_CfFlowInfo_MustPointTo) {
    _LL307: _temp306 = (struct Cyc_CfFlowInfo_Place *)((struct Cyc_CfFlowInfo_MustPointTo_struct *)_temp290)->f1;
	    goto _LL297;
  }
  else {
    goto _LL291;
  }
  _LL293: return 1;
  _LL295: return 0;
  _LL297: {
  struct _tuple6 _temp310;
    struct Cyc_List_List *_temp311;
    struct Cyc_List_List **_temp313;
    struct Cyc_Dict_Dict *_temp314;
    struct _tuple6 *_temp308 = env;
    _temp310 = *_temp308;
    _LL315: _temp314 = _temp310.f1;
	    goto _LL312;
    _LL312: _temp311 = _temp310.f2;
	    _temp313 = &(*_temp308).f2;
	    goto _LL309;
    _LL309: if (((int (*)(int (*compare)(struct Cyc_CfFlowInfo_Place *,struct Cyc_CfFlowInfo_Place *),
			  struct Cyc_List_List *l,struct Cyc_CfFlowInfo_Place *x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,
												*_temp313,
												_temp306)) {
	      return 1;
	    }
	    *_temp313 = ({ struct Cyc_List_List *_temp316 = (struct Cyc_List_List *)GC_malloc(sizeof(struct Cyc_List_List) * 1);
	      _temp316[0] = ({ struct Cyc_List_List _temp317;
		_temp317.hd = (void *)_temp306;
		_temp317.tl = *_temp313;
		_temp317; });
	      _temp316; });
	    return Cyc_CfFlowInfo_isAllInit_pinfo(env,Cyc_CfFlowInfo_lookup_place(_temp314,
										  _temp306));
  }
  _LL291: ;
}
int Cyc_CfFlowInfo_isAllInit_f(struct _tuple6 *env,void *a,void *path) {
  return Cyc_CfFlowInfo_isAllInit_pinfo(env,path);
}
int Cyc_CfFlowInfo_isAllInit_pinfo(struct _tuple6 *env,void *path) {
  void *_temp318 = path;
  void *_temp326;
  struct Cyc_Dict_Dict *_temp328;
  struct Cyc_Dict_Dict *_temp330;
  _LL320: if (*((int *)_temp318) == Cyc_CfFlowInfo_LeafPI) {
    _LL327: _temp326 = (void *)((struct Cyc_CfFlowInfo_LeafPI_struct *)_temp318)->f1;
	    goto _LL321;
  }
  else {
    goto _LL322;
  }
  _LL322: if (*((int *)_temp318) == Cyc_CfFlowInfo_TuplePI) {
    _LL329: _temp328 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_TuplePI_struct *)_temp318)->f1;
	    goto _LL323;
  }
  else {
    goto _LL324;
  }
  _LL324: if (*((int *)_temp318) == Cyc_CfFlowInfo_StructPI) {
    _LL331: _temp330 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_StructPI_struct *)_temp318)->f1;
	    goto _LL325;
  }
  else {
    goto _LL319;
  }
  _LL321: return Cyc_CfFlowInfo_isAllInit_is(env,_temp326);
  _LL323: return ((int (*)(int (*f)(struct _tuple6 *,int,void *),struct _tuple6 *env,
			   struct Cyc_Dict_Dict *d))Cyc_Dict_forall_c)((int (*)(struct _tuple6 *env,
										int a,
										void *path))Cyc_CfFlowInfo_isAllInit_f,
								       env,
								       _temp328);
  _LL325: return ((int (*)(int (*f)(struct _tuple6 *,struct _tagged_string *,
				    void *),struct _tuple6 *env,struct Cyc_Dict_Dict *d))Cyc_Dict_forall_c)((int (*)(struct _tuple6 *env,
														     struct _tagged_string *a,
														     void *path))Cyc_CfFlowInfo_isAllInit_f,
													    env,
													    _temp330);
  _LL319: ;
}
int Cyc_CfFlowInfo_isAllInit(struct Cyc_Dict_Dict *pinfo_dict,void *pinfo) {
  struct _tuple6 isInitEnv = ({ struct _tuple6 _temp332;
    _temp332.f1 = pinfo_dict;
    _temp332.f2 = 0;
    _temp332; });
  return Cyc_CfFlowInfo_isAllInit_pinfo(&isInitEnv,pinfo);
}
struct _tuple7 {
  struct Cyc_Dict_Dict *f1;
  struct Cyc_Set_Set **f2;
};
void *Cyc_CfFlowInfo_pinfo_getplaces_esc(struct _tuple7 *env,void *pinfo) {
  struct _tuple7 _temp335;
  struct Cyc_Set_Set **_temp336;
  struct Cyc_Dict_Dict *_temp338;
  struct _tuple7 *_temp333 = env;
  _temp335 = *_temp333;
  _LL339: _temp338 = _temp335.f1;
	  goto _LL337;
  _LL337: _temp336 = _temp335.f2;
	  goto _LL334;
  _LL334: {
  void *_temp340 = pinfo;
    void *_temp350;
    struct Cyc_CfFlowInfo_Place *_temp352;
    void *_temp354;
    void *_temp356;
    void *_temp358;
    struct Cyc_Dict_Dict *_temp360;
    struct Cyc_Dict_Dict *_temp362;
    _LL342: if (*((int *)_temp340) == Cyc_CfFlowInfo_LeafPI) {
      _LL351: _temp350 = (void *)((struct Cyc_CfFlowInfo_LeafPI_struct *)_temp340)->f1;
	      if (*((int *)_temp350) == Cyc_CfFlowInfo_MustPointTo) {
		_LL353: _temp352 = (struct Cyc_CfFlowInfo_Place *)((struct Cyc_CfFlowInfo_MustPointTo_struct *)_temp350)->f1;
			goto _LL343;
	      }
	      else {
		goto _LL344;
	      }
    }
    else {
      goto _LL344;
    }
    _LL344: if (*((int *)_temp340) == Cyc_CfFlowInfo_LeafPI) {
      _LL355: _temp354 = (void *)((struct Cyc_CfFlowInfo_LeafPI_struct *)_temp340)->f1;
	      if (*((int *)_temp354) == Cyc_CfFlowInfo_UnknownIS) {
		_LL359: _temp358 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp354)->f1;
			goto _LL357;
		_LL357: _temp356 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp354)->f2;
			goto _LL345;
	      }
	      else {
		goto _LL346;
	      }
    }
    else {
      goto _LL346;
    }
    _LL346: if (*((int *)_temp340) == Cyc_CfFlowInfo_TuplePI) {
      _LL361: _temp360 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_TuplePI_struct *)_temp340)->f1;
	      goto _LL347;
    }
    else {
      goto _LL348;
    }
    _LL348: if (*((int *)_temp340) == Cyc_CfFlowInfo_StructPI) {
      _LL363: _temp362 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_StructPI_struct *)_temp340)->f1;
	      goto _LL349;
    }
    else {
      goto _LL341;
    }
    _LL343: *_temp336 = ((struct Cyc_Set_Set *(*)(struct Cyc_Set_Set *s,struct Cyc_CfFlowInfo_Place *elt))Cyc_Set_insert)(*_temp336,
															  _temp352);
	    if (Cyc_CfFlowInfo_isAllInit(_temp338,Cyc_CfFlowInfo_lookup_place(_temp338,
									      _temp352))) {
	      return Cyc_CfFlowInfo_mkLeafPI((void *)Cyc_CfFlowInfo_Esc,(void *)Cyc_CfFlowInfo_AllIL);
	    }
	    return Cyc_CfFlowInfo_mkLeafPI((void *)Cyc_CfFlowInfo_Esc,(void *)Cyc_CfFlowInfo_ThisIL);
    _LL345: return Cyc_CfFlowInfo_mkLeafPI((void *)Cyc_CfFlowInfo_Esc,_temp356);
    _LL347: return (void *)({ struct Cyc_CfFlowInfo_TuplePI_struct *_temp364 = (struct Cyc_CfFlowInfo_TuplePI_struct *)GC_malloc(sizeof(struct Cyc_CfFlowInfo_TuplePI_struct));
      _temp364[0] = ({ struct Cyc_CfFlowInfo_TuplePI_struct _temp365;
	_temp365.tag = Cyc_CfFlowInfo_TuplePI;
	_temp365.f1 = ((struct Cyc_Dict_Dict *(*)(void *(*f)(struct _tuple7 *,
							     void *),struct _tuple7 *env,
						  struct Cyc_Dict_Dict *d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_pinfo_getplaces_esc,
											   env,
											   _temp360);
	_temp365; });
      _temp364; });
    _LL349: return (void *)({ struct Cyc_CfFlowInfo_StructPI_struct *_temp366 = (struct Cyc_CfFlowInfo_StructPI_struct *)GC_malloc(sizeof(struct Cyc_CfFlowInfo_StructPI_struct));
      _temp366[0] = ({ struct Cyc_CfFlowInfo_StructPI_struct _temp367;
	_temp367.tag = Cyc_CfFlowInfo_StructPI;
	_temp367.f1 = ((struct Cyc_Dict_Dict *(*)(void *(*f)(struct _tuple7 *,
							     void *),struct _tuple7 *env,
						  struct Cyc_Dict_Dict *d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_pinfo_getplaces_esc,
											   env,
											   _temp362);
	_temp367; });
      _temp366; });
    _LL341: ;
  }
}
void Cyc_CfFlowInfo_pinfo_getplaces(struct Cyc_Set_Set **places_ptr,void *a,
				    void *pinfo) {
  void *_temp368 = pinfo;
  void *_temp378;
  void *_temp380;
  void *_temp382;
  void *_temp384;
  struct Cyc_CfFlowInfo_Place *_temp386;
  struct Cyc_Dict_Dict *_temp388;
  struct Cyc_Dict_Dict *_temp390;
  _LL370: if (*((int *)_temp368) == Cyc_CfFlowInfo_LeafPI) {
    _LL379: _temp378 = (void *)((struct Cyc_CfFlowInfo_LeafPI_struct *)_temp368)->f1;
	    if (*((int *)_temp378) == Cyc_CfFlowInfo_UnknownIS) {
	      _LL383: _temp382 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp378)->f1;
		      goto _LL381;
	      _LL381: _temp380 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp378)->f2;
		      goto _LL371;
	    }
	    else {
	      goto _LL372;
	    }
  }
  else {
    goto _LL372;
  }
  _LL372: if (*((int *)_temp368) == Cyc_CfFlowInfo_LeafPI) {
    _LL385: _temp384 = (void *)((struct Cyc_CfFlowInfo_LeafPI_struct *)_temp368)->f1;
	    if (*((int *)_temp384) == Cyc_CfFlowInfo_MustPointTo) {
	      _LL387: _temp386 = (struct Cyc_CfFlowInfo_Place *)((struct Cyc_CfFlowInfo_MustPointTo_struct *)_temp384)->f1;
		      goto _LL373;
	    }
	    else {
	      goto _LL374;
	    }
  }
  else {
    goto _LL374;
  }
  _LL374: if (*((int *)_temp368) == Cyc_CfFlowInfo_TuplePI) {
    _LL389: _temp388 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_TuplePI_struct *)_temp368)->f1;
	    goto _LL375;
  }
  else {
    goto _LL376;
  }
  _LL376: if (*((int *)_temp368) == Cyc_CfFlowInfo_StructPI) {
    _LL391: _temp390 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_StructPI_struct *)_temp368)->f1;
	    goto _LL377;
  }
  else {
    goto _LL369;
  }
  _LL371: return;
  _LL373: *places_ptr = ((struct Cyc_Set_Set *(*)(struct Cyc_Set_Set *s,struct Cyc_CfFlowInfo_Place *elt))Cyc_Set_insert)(*places_ptr,
															  _temp386);
	  return;
  _LL375: ((void (*)(void (*f)(struct Cyc_Set_Set **,int,void *),struct Cyc_Set_Set **env,
		     struct Cyc_Dict_Dict *d))Cyc_Dict_iter_c)((void (*)(struct Cyc_Set_Set **places_ptr,
									 int a,
									 void *pinfo))Cyc_CfFlowInfo_pinfo_getplaces,
							       places_ptr,
							       _temp388);
	  return;
  _LL377: ((void (*)(void (*f)(struct Cyc_Set_Set **,struct _tagged_string *,
			       void *),struct Cyc_Set_Set **env,struct Cyc_Dict_Dict *d))Cyc_Dict_iter_c)((void (*)(struct Cyc_Set_Set **places_ptr,
														    struct _tagged_string *a,
														    void *pinfo))Cyc_CfFlowInfo_pinfo_getplaces,
													  places_ptr,
													  _temp390);
	  return;
  _LL369: ;
}
struct Cyc_Dict_Dict *Cyc_CfFlowInfo_escape_these(struct Cyc_Set_Set *places,
						  struct Cyc_Dict_Dict *d) {
  while (! ((int (*)(struct Cyc_Set_Set *s))Cyc_Set_is_empty)(places)) {
    struct Cyc_CfFlowInfo_Place *place = ((struct Cyc_CfFlowInfo_Place *(*)(struct Cyc_Set_Set *s))Cyc_Set_choose)(places);
    places = ((struct Cyc_Set_Set *(*)(struct Cyc_Set_Set *s,struct Cyc_CfFlowInfo_Place *elt))Cyc_Set_delete)(places,
													       place);
    { void *place_old_pinfo;
      { struct _handler_cons _temp392;
	_push_handler(&_temp392);
	{ void *_temp393 = (void *)setjmp(_temp392.handler);
	  if (! _temp393) {
	    place_old_pinfo = Cyc_CfFlowInfo_lookup_place(d,place);
	    ;
	    _pop_handler();
	  }
	  else {
	    void *_temp395 = _temp393;
	    _LL397: if (_temp395 == Cyc_Dict_Absent) {
	      goto _LL398;
	    }
	    else {
	      goto _LL399;
	    }
	    _LL399: goto _LL400;
	    _LL398: continue;
	    _LL400: (void)_throw(_temp395);
	    _LL396: ;
	  }}
	
      }
      { struct _tuple7 places_env = ({ struct _tuple7 _temp401;
	  _temp401.f1 = d;
	  _temp401.f2 = &places;
	  _temp401; });
	void *place_new_pinfo = Cyc_CfFlowInfo_pinfo_getplaces_esc(&places_env,
								   place_old_pinfo);
	if (place_old_pinfo != place_new_pinfo) {
	  d = Cyc_CfFlowInfo_insert_place(d,place,place_new_pinfo);
	}}
      }
    
  }
  return d;
}
static struct Cyc_Set_Set **Cyc_CfFlowInfo_empty_place_set_ptr = 0;
struct Cyc_Dict_Dict *Cyc_CfFlowInfo_escape_pointsto(void *pinfo,struct Cyc_Dict_Dict *d) {
  if (Cyc_CfFlowInfo_empty_place_set_ptr == 0) {
    Cyc_CfFlowInfo_empty_place_set_ptr = ({ struct Cyc_Set_Set **_temp402 = (struct Cyc_Set_Set **)GC_malloc(sizeof(struct Cyc_Set_Set *) * 1);
      _temp402[0] = ((struct Cyc_Set_Set *(*)(int (*comp)(struct Cyc_CfFlowInfo_Place *,
							  struct Cyc_CfFlowInfo_Place *)))Cyc_Set_empty)(Cyc_CfFlowInfo_place_cmp);
      _temp402; });
  }
  { struct Cyc_Set_Set *places = ({ struct Cyc_Set_Set **_temp403 = Cyc_CfFlowInfo_empty_place_set_ptr;
      if (_temp403 == 0) {
	_throw(Null_Exception);
      }
      *_temp403; });
    ((void (*)(struct Cyc_Set_Set **places_ptr,int a,void *pinfo))Cyc_CfFlowInfo_pinfo_getplaces)(&places,
												  0,
												  pinfo);
    return Cyc_CfFlowInfo_escape_these(places,d);}
  
}
void *Cyc_CfFlowInfo_assign_unknown_dict(void *level,void *pinfo) {
  void *_temp404 = pinfo;
  void *_temp414;
  void *_temp416;
  void *_temp418;
  void *_temp420;
  struct Cyc_Dict_Dict *_temp422;
  struct Cyc_Dict_Dict *_temp424;
  _LL406: if (*((int *)_temp404) == Cyc_CfFlowInfo_LeafPI) {
    _LL415: _temp414 = (void *)((struct Cyc_CfFlowInfo_LeafPI_struct *)_temp404)->f1;
	    if (*((int *)_temp414) == Cyc_CfFlowInfo_UnknownIS) {
	      _LL419: _temp418 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp414)->f1;
		      if (_temp418 == (void *)Cyc_CfFlowInfo_Esc) {
			goto _LL417;
		      }
		      else {
			goto _LL408;
		      }
	      _LL417: _temp416 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp414)->f2;
		      goto _LL407;
	    }
	    else {
	      goto _LL408;
	    }
  }
  else {
    goto _LL408;
  }
  _LL408: if (*((int *)_temp404) == Cyc_CfFlowInfo_LeafPI) {
    _LL421: _temp420 = (void *)((struct Cyc_CfFlowInfo_LeafPI_struct *)_temp404)->f1;
	    goto _LL409;
  }
  else {
    goto _LL410;
  }
  _LL410: if (*((int *)_temp404) == Cyc_CfFlowInfo_TuplePI) {
    _LL423: _temp422 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_TuplePI_struct *)_temp404)->f1;
	    goto _LL411;
  }
  else {
    goto _LL412;
  }
  _LL412: if (*((int *)_temp404) == Cyc_CfFlowInfo_StructPI) {
    _LL425: _temp424 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_StructPI_struct *)_temp404)->f1;
	    goto _LL413;
  }
  else {
    goto _LL405;
  }
  _LL407: return Cyc_CfFlowInfo_mkLeafPI((void *)Cyc_CfFlowInfo_Esc,level);
  _LL409: return Cyc_CfFlowInfo_mkLeafPI((void *)Cyc_CfFlowInfo_Unesc,level);
  _LL411: return (void *)({ struct Cyc_CfFlowInfo_TuplePI_struct *_temp426 = (struct Cyc_CfFlowInfo_TuplePI_struct *)GC_malloc(sizeof(struct Cyc_CfFlowInfo_TuplePI_struct));
    _temp426[0] = ({ struct Cyc_CfFlowInfo_TuplePI_struct _temp427;
      _temp427.tag = Cyc_CfFlowInfo_TuplePI;
      _temp427.f1 = ((struct Cyc_Dict_Dict *(*)(void *(*f)(void *,void *),
						void *env,struct Cyc_Dict_Dict *d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_assign_unknown_dict,
												   level,
												   _temp422);
      _temp427; });
    _temp426; });
  _LL413: return (void *)({ struct Cyc_CfFlowInfo_StructPI_struct *_temp428 = (struct Cyc_CfFlowInfo_StructPI_struct *)GC_malloc(sizeof(struct Cyc_CfFlowInfo_StructPI_struct));
    _temp428[0] = ({ struct Cyc_CfFlowInfo_StructPI_struct _temp429;
      _temp429.tag = Cyc_CfFlowInfo_StructPI;
      _temp429.f1 = ((struct Cyc_Dict_Dict *(*)(void *(*f)(void *,void *),
						void *env,struct Cyc_Dict_Dict *d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_assign_unknown_dict,
												   level,
												   _temp424);
      _temp429; });
    _temp428; });
  _LL405: ;
}
struct Cyc_CfFlowInfo_JoinEnv {
  struct Cyc_Dict_Dict *d1;
  struct Cyc_Dict_Dict *d2;
  struct Cyc_Set_Set **places;
};
typedef struct Cyc_CfFlowInfo_JoinEnv *Cyc_CfFlowInfo_join_env_t;
void *Cyc_CfFlowInfo_join_escape(void *esc1,void *esc2) {
  struct _tuple3 _temp431 = ({ struct _tuple3 _temp430;
    _temp430.f1 = esc1;
    _temp430.f2 = esc2;
    _temp430; });
  void *_temp439;
  void *_temp441;
  void *_temp443;
  void *_temp445;
  _LL433: _LL442: _temp441 = _temp431.f1;
		  if (_temp441 == (void *)Cyc_CfFlowInfo_Esc) {
		    goto _LL440;
		  }
		  else {
		    goto _LL435;
		  }
	  _LL440: _temp439 = _temp431.f2;
		  goto _LL434;
  _LL435: _LL446: _temp445 = _temp431.f1;
		  goto _LL444;
	  _LL444: _temp443 = _temp431.f2;
		  if (_temp443 == (void *)Cyc_CfFlowInfo_Esc) {
		    goto _LL436;
		  }
		  else {
		    goto _LL437;
		  }
  _LL437: goto _LL438;
  _LL434: goto _LL436;
  _LL436: return (void *)Cyc_CfFlowInfo_Esc;
  _LL438: return (void *)Cyc_CfFlowInfo_Unesc;
  _LL432: ;
}
void *Cyc_CfFlowInfo_join_initlevel(void *il1,void *il2) {
  struct _tuple3 _temp448 = ({ struct _tuple3 _temp447;
    _temp447.f1 = il1;
    _temp447.f2 = il2;
    _temp447; });
  void *_temp460;
  void *_temp462;
  void *_temp464;
  void *_temp466;
  void *_temp468;
  void *_temp470;
  void *_temp472;
  void *_temp474;
  _LL450: _LL463: _temp462 = _temp448.f1;
		  if (_temp462 == (void *)Cyc_CfFlowInfo_NoneIL) {
		    goto _LL461;
		  }
		  else {
		    goto _LL452;
		  }
	  _LL461: _temp460 = _temp448.f2;
		  goto _LL451;
  _LL452: _LL467: _temp466 = _temp448.f1;
		  goto _LL465;
	  _LL465: _temp464 = _temp448.f2;
		  if (_temp464 == (void *)Cyc_CfFlowInfo_NoneIL) {
		    goto _LL453;
		  }
		  else {
		    goto _LL454;
		  }
  _LL454: _LL471: _temp470 = _temp448.f1;
		  if (_temp470 == (void *)Cyc_CfFlowInfo_ThisIL) {
		    goto _LL469;
		  }
		  else {
		    goto _LL456;
		  }
	  _LL469: _temp468 = _temp448.f2;
		  goto _LL455;
  _LL456: _LL475: _temp474 = _temp448.f1;
		  goto _LL473;
	  _LL473: _temp472 = _temp448.f2;
		  if (_temp472 == (void *)Cyc_CfFlowInfo_ThisIL) {
		    goto _LL457;
		  }
		  else {
		    goto _LL458;
		  }
  _LL458: goto _LL459;
  _LL451: goto _LL453;
  _LL453: return (void *)Cyc_CfFlowInfo_NoneIL;
  _LL455: goto _LL457;
  _LL457: return (void *)Cyc_CfFlowInfo_ThisIL;
  _LL459: return (void *)Cyc_CfFlowInfo_AllIL;
  _LL449: ;
}
void *Cyc_CfFlowInfo_join_initstate(struct Cyc_CfFlowInfo_JoinEnv *env,void *state1,
				    void *state2) {
  struct _tuple3 _temp477 = ({ struct _tuple3 _temp476;
    _temp476.f1 = state1;
    _temp476.f2 = state2;
    _temp476; });
  void *_temp487;
  void *_temp489;
  void *_temp491;
  void *_temp493;
  void *_temp495;
  void *_temp497;
  void *_temp499;
  struct Cyc_CfFlowInfo_Place *_temp501;
  void *_temp503;
  void *_temp505;
  void *_temp507;
  void *_temp509;
  void *_temp511;
  void *_temp513;
  void *_temp515;
  struct Cyc_CfFlowInfo_Place *_temp517;
  void *_temp519;
  struct Cyc_CfFlowInfo_Place *_temp521;
  void *_temp523;
  struct Cyc_CfFlowInfo_Place *_temp525;
  _LL479: _LL494: _temp493 = _temp477.f1;
		  if (*((int *)_temp493) == Cyc_CfFlowInfo_UnknownIS) {
		    _LL498: _temp497 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp493)->f1;
			    goto _LL496;
		    _LL496: _temp495 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp493)->f2;
			    goto _LL488;
		  }
		  else {
		    goto _LL481;
		  }
	  _LL488: _temp487 = _temp477.f2;
		  if (*((int *)_temp487) == Cyc_CfFlowInfo_UnknownIS) {
		    _LL492: _temp491 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp487)->f1;
			    goto _LL490;
		    _LL490: _temp489 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp487)->f2;
			    goto _LL480;
		  }
		  else {
		    goto _LL481;
		  }
  _LL481: _LL504: _temp503 = _temp477.f1;
		  if (*((int *)_temp503) == Cyc_CfFlowInfo_UnknownIS) {
		    _LL508: _temp507 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp503)->f1;
			    goto _LL506;
		    _LL506: _temp505 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp503)->f2;
			    goto _LL500;
		  }
		  else {
		    goto _LL483;
		  }
	  _LL500: _temp499 = _temp477.f2;
		  if (*((int *)_temp499) == Cyc_CfFlowInfo_MustPointTo) {
		    _LL502: _temp501 = (struct Cyc_CfFlowInfo_Place *)((struct Cyc_CfFlowInfo_MustPointTo_struct *)_temp499)->f1;
			    goto _LL482;
		  }
		  else {
		    goto _LL483;
		  }
  _LL483: _LL516: _temp515 = _temp477.f1;
		  if (*((int *)_temp515) == Cyc_CfFlowInfo_MustPointTo) {
		    _LL518: _temp517 = (struct Cyc_CfFlowInfo_Place *)((struct Cyc_CfFlowInfo_MustPointTo_struct *)_temp515)->f1;
			    goto _LL510;
		  }
		  else {
		    goto _LL485;
		  }
	  _LL510: _temp509 = _temp477.f2;
		  if (*((int *)_temp509) == Cyc_CfFlowInfo_UnknownIS) {
		    _LL514: _temp513 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp509)->f1;
			    goto _LL512;
		    _LL512: _temp511 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp509)->f2;
			    goto _LL484;
		  }
		  else {
		    goto _LL485;
		  }
  _LL485: _LL524: _temp523 = _temp477.f1;
		  if (*((int *)_temp523) == Cyc_CfFlowInfo_MustPointTo) {
		    _LL526: _temp525 = (struct Cyc_CfFlowInfo_Place *)((struct Cyc_CfFlowInfo_MustPointTo_struct *)_temp523)->f1;
			    goto _LL520;
		  }
		  else {
		    goto _LL478;
		  }
	  _LL520: _temp519 = _temp477.f2;
		  if (*((int *)_temp519) == Cyc_CfFlowInfo_MustPointTo) {
		    _LL522: _temp521 = (struct Cyc_CfFlowInfo_Place *)((struct Cyc_CfFlowInfo_MustPointTo_struct *)_temp519)->f1;
			    goto _LL486;
		  }
		  else {
		    goto _LL478;
		  }
  _LL480: return Cyc_CfFlowInfo_mkLeafPI(Cyc_CfFlowInfo_join_escape(_temp497,
								    _temp491),
					 Cyc_CfFlowInfo_join_initlevel(_temp495,
								       _temp489));
  _LL482: {
  struct _tuple6 isInitEnv = ({ struct _tuple6 _temp527;
      _temp527.f1 = env->d2;
      _temp527.f2 = 0;
      _temp527; });
    if (_temp505 == (void *)Cyc_CfFlowInfo_AllIL ? Cyc_CfFlowInfo_isAllInit_pinfo(&isInitEnv,
										  Cyc_CfFlowInfo_lookup_place(env->d2,
													      _temp501)) : 0) {
      *env->places = ((struct Cyc_Set_Set *(*)(struct Cyc_Set_Set *s,struct Cyc_CfFlowInfo_Place *elt))Cyc_Set_insert)(*env->places,
														       _temp501);
      return Cyc_CfFlowInfo_mkLeafPI(_temp507,_temp505);
    }
    return Cyc_CfFlowInfo_mkLeafPI(_temp507,Cyc_CfFlowInfo_join_initlevel(_temp505,
									  (void *)Cyc_CfFlowInfo_ThisIL));
  }
  _LL484: {
  struct _tuple6 isInitEnv = ({ struct _tuple6 _temp528;
      _temp528.f1 = env->d1;
      _temp528.f2 = 0;
      _temp528; });
    if (_temp511 == (void *)Cyc_CfFlowInfo_AllIL ? Cyc_CfFlowInfo_isAllInit_pinfo(&isInitEnv,
										  Cyc_CfFlowInfo_lookup_place(env->d1,
													      _temp517)) : 0) {
      *env->places = ((struct Cyc_Set_Set *(*)(struct Cyc_Set_Set *s,struct Cyc_CfFlowInfo_Place *elt))Cyc_Set_insert)(*env->places,
														       _temp517);
      return Cyc_CfFlowInfo_mkLeafPI(_temp513,_temp511);
    }
    return Cyc_CfFlowInfo_mkLeafPI(_temp513,Cyc_CfFlowInfo_join_initlevel((void *)Cyc_CfFlowInfo_ThisIL,
									  _temp511));
  }
  _LL486: if (Cyc_CfFlowInfo_place_cmp(_temp525,_temp521) == 0) {
    return (void *)({ struct Cyc_CfFlowInfo_LeafPI_struct *_temp529 = (struct Cyc_CfFlowInfo_LeafPI_struct *)GC_malloc(sizeof(struct Cyc_CfFlowInfo_LeafPI_struct));
      _temp529[0] = ({ struct Cyc_CfFlowInfo_LeafPI_struct _temp530;
	_temp530.tag = Cyc_CfFlowInfo_LeafPI;
	_temp530.f1 = (void *)state1;
	_temp530; });
      _temp529; });
  }
  { struct _tuple6 isInitEnv1 = ({ struct _tuple6 _temp532;
      _temp532.f1 = env->d1;
      _temp532.f2 = 0;
      _temp532; });
    struct _tuple6 isInitEnv2 = ({ struct _tuple6 _temp531;
      _temp531.f1 = env->d2;
      _temp531.f2 = 0;
      _temp531; });
    if (Cyc_CfFlowInfo_isAllInit_pinfo(&isInitEnv1,Cyc_CfFlowInfo_lookup_place(env->d1,
									       _temp525)) ? Cyc_CfFlowInfo_isAllInit_pinfo(&isInitEnv2,
															   Cyc_CfFlowInfo_lookup_place(env->d2,
																		       _temp521)) : 0) {
      *env->places = ((struct Cyc_Set_Set *(*)(struct Cyc_Set_Set *s,struct Cyc_CfFlowInfo_Place *elt))Cyc_Set_insert)(*env->places,
														       _temp525);
      *env->places = ((struct Cyc_Set_Set *(*)(struct Cyc_Set_Set *s,struct Cyc_CfFlowInfo_Place *elt))Cyc_Set_insert)(*env->places,
														       _temp521);
      return Cyc_CfFlowInfo_mkLeafPI((void *)Cyc_CfFlowInfo_Unesc,(void *)Cyc_CfFlowInfo_AllIL);
    }
    return Cyc_CfFlowInfo_mkLeafPI((void *)Cyc_CfFlowInfo_Unesc,(void *)Cyc_CfFlowInfo_ThisIL);}
  
  _LL478: ;
}
void *Cyc_CfFlowInfo_join_pathinfo(struct Cyc_CfFlowInfo_JoinEnv *env,void *pinfo1,
				   void *pinfo2) {
  if (pinfo1 == pinfo2) {
    return pinfo1;
  }
  { struct _tuple3 _temp534 = ({ struct _tuple3 _temp533;
      _temp533.f1 = pinfo1;
      _temp533.f2 = pinfo2;
      _temp533; });
    void *_temp544;
    void *_temp546;
    void *_temp548;
    void *_temp550;
    void *_temp552;
    struct Cyc_Dict_Dict *_temp554;
    void *_temp556;
    struct Cyc_Dict_Dict *_temp558;
    void *_temp560;
    struct Cyc_Dict_Dict *_temp562;
    void *_temp564;
    struct Cyc_Dict_Dict *_temp566;
    _LL536: _LL549: _temp548 = _temp534.f1;
		    if (*((int *)_temp548) == Cyc_CfFlowInfo_LeafPI) {
		      _LL551: _temp550 = (void *)((struct Cyc_CfFlowInfo_LeafPI_struct *)_temp548)->f1;
			      goto _LL545;
		    }
		    else {
		      goto _LL538;
		    }
	    _LL545: _temp544 = _temp534.f2;
		    if (*((int *)_temp544) == Cyc_CfFlowInfo_LeafPI) {
		      _LL547: _temp546 = (void *)((struct Cyc_CfFlowInfo_LeafPI_struct *)_temp544)->f1;
			      goto _LL537;
		    }
		    else {
		      goto _LL538;
		    }
    _LL538: _LL557: _temp556 = _temp534.f1;
		    if (*((int *)_temp556) == Cyc_CfFlowInfo_TuplePI) {
		      _LL559: _temp558 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_TuplePI_struct *)_temp556)->f1;
			      goto _LL553;
		    }
		    else {
		      goto _LL540;
		    }
	    _LL553: _temp552 = _temp534.f2;
		    if (*((int *)_temp552) == Cyc_CfFlowInfo_TuplePI) {
		      _LL555: _temp554 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_TuplePI_struct *)_temp552)->f1;
			      goto _LL539;
		    }
		    else {
		      goto _LL540;
		    }
    _LL540: _LL565: _temp564 = _temp534.f1;
		    if (*((int *)_temp564) == Cyc_CfFlowInfo_StructPI) {
		      _LL567: _temp566 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_StructPI_struct *)_temp564)->f1;
			      goto _LL561;
		    }
		    else {
		      goto _LL542;
		    }
	    _LL561: _temp560 = _temp534.f2;
		    if (*((int *)_temp560) == Cyc_CfFlowInfo_StructPI) {
		      _LL563: _temp562 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_StructPI_struct *)_temp560)->f1;
			      goto _LL541;
		    }
		    else {
		      goto _LL542;
		    }
    _LL542: goto _LL543;
    _LL537: if (_temp550 == _temp546) {
	      return pinfo1;
	    }
	    return Cyc_CfFlowInfo_join_initstate(env,_temp550,_temp546);
    _LL539: if (_temp558 == _temp554) {
	      return pinfo1;
	    }
	    return (void *)({ struct Cyc_CfFlowInfo_TuplePI_struct *_temp568 = (struct Cyc_CfFlowInfo_TuplePI_struct *)GC_malloc(sizeof(struct Cyc_CfFlowInfo_TuplePI_struct));
	      _temp568[0] = ({ struct Cyc_CfFlowInfo_TuplePI_struct _temp569;
		_temp569.tag = Cyc_CfFlowInfo_TuplePI;
		_temp569.f1 = ((struct Cyc_Dict_Dict *(*)(void *(*f)(struct Cyc_CfFlowInfo_JoinEnv *,
								     void *,
								     void *),
							  struct Cyc_CfFlowInfo_JoinEnv *env,
							  struct Cyc_Dict_Dict *d1,
							  struct Cyc_Dict_Dict *d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_pathinfo,
													  env,
													  _temp558,
													  _temp554);
		_temp569; });
	      _temp568; });
    _LL541: if (_temp566 == _temp562) {
	      return pinfo1;
	    }
	    return (void *)({ struct Cyc_CfFlowInfo_StructPI_struct *_temp570 = (struct Cyc_CfFlowInfo_StructPI_struct *)GC_malloc(sizeof(struct Cyc_CfFlowInfo_StructPI_struct));
	      _temp570[0] = ({ struct Cyc_CfFlowInfo_StructPI_struct _temp571;
		_temp571.tag = Cyc_CfFlowInfo_StructPI;
		_temp571.f1 = ((struct Cyc_Dict_Dict *(*)(void *(*f)(struct Cyc_CfFlowInfo_JoinEnv *,
								     void *,
								     void *),
							  struct Cyc_CfFlowInfo_JoinEnv *env,
							  struct Cyc_Dict_Dict *d1,
							  struct Cyc_Dict_Dict *d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_pathinfo,
													  env,
													  _temp566,
													  _temp562);
		_temp571; });
	      _temp570; });
    _LL543: (void)_throw((void *)({ struct Cyc_Core_Impossible_struct *_temp572 = (struct Cyc_Core_Impossible_struct *)GC_malloc(sizeof(struct Cyc_Core_Impossible_struct));
			   _temp572[0] = ({ struct Cyc_Core_Impossible_struct _temp573;
			     _temp573.tag = Cyc_Core_Impossible;
			     _temp573.f1 = (struct _tagged_string)({ char *_temp574 = (char *)"join_pathinfo";
			       struct _tagged_string _temp575;
			       _temp575.curr = _temp574;
			       _temp575.base = _temp574;
			       _temp575.last_plus_one = _temp574 + 14;
			       _temp575; });
			     _temp573; });
			   _temp572; }));
    _LL535: ;}
  
}
void *Cyc_CfFlowInfo_join_flow(void *f1,void *f2) {
  if (Cyc_CfFlowInfo_empty_place_set_ptr == 0) {
    Cyc_CfFlowInfo_empty_place_set_ptr = ({ struct Cyc_Set_Set **_temp576 = (struct Cyc_Set_Set **)GC_malloc(sizeof(struct Cyc_Set_Set *) * 1);
      _temp576[0] = ((struct Cyc_Set_Set *(*)(int (*comp)(struct Cyc_CfFlowInfo_Place *,
							  struct Cyc_CfFlowInfo_Place *)))Cyc_Set_empty)(Cyc_CfFlowInfo_place_cmp);
      _temp576; });
  }
  if (f1 == f2) {
    return f1;
  }
  { struct _tuple3 _temp578 = ({ struct _tuple3 _temp577;
      _temp577.f1 = f1;
      _temp577.f2 = f2;
      _temp577; });
    void *_temp586;
    void *_temp588;
    void *_temp590;
    void *_temp592;
    void *_temp594;
    struct Cyc_Dict_Dict *_temp596;
    void *_temp598;
    struct Cyc_Dict_Dict *_temp600;
    _LL580: _LL589: _temp588 = _temp578.f1;
		    if (_temp588 == (void *)Cyc_CfFlowInfo_BottomFL) {
		      goto _LL587;
		    }
		    else {
		      goto _LL582;
		    }
	    _LL587: _temp586 = _temp578.f2;
		    goto _LL581;
    _LL582: _LL593: _temp592 = _temp578.f1;
		    goto _LL591;
	    _LL591: _temp590 = _temp578.f2;
		    if (_temp590 == (void *)Cyc_CfFlowInfo_BottomFL) {
		      goto _LL583;
		    }
		    else {
		      goto _LL584;
		    }
    _LL584: _LL599: _temp598 = _temp578.f1;
		    if ((unsigned int)_temp598 > 1 ? *((int *)_temp598) == Cyc_CfFlowInfo_InitsFL : 0) {
		      _LL601: _temp600 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_InitsFL_struct *)_temp598)->f1;
			      goto _LL595;
		    }
		    else {
		      goto _LL579;
		    }
	    _LL595: _temp594 = _temp578.f2;
		    if ((unsigned int)_temp594 > 1 ? *((int *)_temp594) == Cyc_CfFlowInfo_InitsFL : 0) {
		      _LL597: _temp596 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_InitsFL_struct *)_temp594)->f1;
			      goto _LL585;
		    }
		    else {
		      goto _LL579;
		    }
    _LL581: return f2;
    _LL583: return f1;
    _LL585: if (_temp600 == _temp596) {
      return f1;
    }
    { struct Cyc_Set_Set *places = ({ struct Cyc_Set_Set **_temp605 = Cyc_CfFlowInfo_empty_place_set_ptr;
	if (_temp605 == 0) {
	  _throw(Null_Exception);
	}
	*_temp605; });
      struct Cyc_CfFlowInfo_JoinEnv joinenv = ({ struct Cyc_CfFlowInfo_JoinEnv _temp604;
	_temp604.d1 = _temp600;
	_temp604.d2 = _temp596;
	_temp604.places = &places;
	_temp604; });
      struct Cyc_Dict_Dict *outdict = ((struct Cyc_Dict_Dict *(*)(void *(*f)(struct Cyc_CfFlowInfo_JoinEnv *,
									     void *,
									     void *),
								  struct Cyc_CfFlowInfo_JoinEnv *env,
								  struct Cyc_Dict_Dict *d1,
								  struct Cyc_Dict_Dict *d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_pathinfo,
														  &joinenv,
														  _temp600,
														  _temp596);
      return (void *)({ struct Cyc_CfFlowInfo_InitsFL_struct *_temp602 = (struct Cyc_CfFlowInfo_InitsFL_struct *)GC_malloc(sizeof(struct Cyc_CfFlowInfo_InitsFL_struct));
	_temp602[0] = ({ struct Cyc_CfFlowInfo_InitsFL_struct _temp603;
	  _temp603.tag = Cyc_CfFlowInfo_InitsFL;
	  _temp603.f1 = Cyc_CfFlowInfo_escape_these(places,outdict);
	  _temp603; });
	_temp602; });}
    
    _LL579: ;}
  
}
int Cyc_CfFlowInfo_initstate_lessthan_approx(void *state1,void *state2) {
  struct _tuple3 _temp607 = ({ struct _tuple3 _temp606;
    _temp606.f1 = state1;
    _temp606.f2 = state2;
    _temp606; });
  void *_temp615;
  void *_temp617;
  void *_temp619;
  void *_temp621;
  void *_temp623;
  void *_temp625;
  void *_temp627;
  struct Cyc_CfFlowInfo_Place *_temp629;
  void *_temp631;
  struct Cyc_CfFlowInfo_Place *_temp633;
  _LL609: _LL622: _temp621 = _temp607.f1;
		  if (*((int *)_temp621) == Cyc_CfFlowInfo_UnknownIS) {
		    _LL626: _temp625 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp621)->f1;
			    goto _LL624;
		    _LL624: _temp623 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp621)->f2;
			    goto _LL616;
		  }
		  else {
		    goto _LL611;
		  }
	  _LL616: _temp615 = _temp607.f2;
		  if (*((int *)_temp615) == Cyc_CfFlowInfo_UnknownIS) {
		    _LL620: _temp619 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp615)->f1;
			    goto _LL618;
		    _LL618: _temp617 = (void *)((struct Cyc_CfFlowInfo_UnknownIS_struct *)_temp615)->f2;
			    goto _LL610;
		  }
		  else {
		    goto _LL611;
		  }
  _LL611: _LL632: _temp631 = _temp607.f1;
		  if (*((int *)_temp631) == Cyc_CfFlowInfo_MustPointTo) {
		    _LL634: _temp633 = (struct Cyc_CfFlowInfo_Place *)((struct Cyc_CfFlowInfo_MustPointTo_struct *)_temp631)->f1;
			    goto _LL628;
		  }
		  else {
		    goto _LL613;
		  }
	  _LL628: _temp627 = _temp607.f2;
		  if (*((int *)_temp627) == Cyc_CfFlowInfo_MustPointTo) {
		    _LL630: _temp629 = (struct Cyc_CfFlowInfo_Place *)((struct Cyc_CfFlowInfo_MustPointTo_struct *)_temp627)->f1;
			    goto _LL612;
		  }
		  else {
		    goto _LL613;
		  }
  _LL613: goto _LL614;
  _LL610: return Cyc_CfFlowInfo_join_escape(_temp625,_temp619) == _temp619 ? Cyc_CfFlowInfo_join_initlevel(_temp623,
													   _temp617) == _temp617 : 0;
  _LL612: return Cyc_CfFlowInfo_place_cmp(_temp633,_temp629) == 0;
  _LL614: return 0;
  _LL608: ;
}
int Cyc_CfFlowInfo_pinfo_lessthan_approx(void *ignore,void *pinfo1,void *pinfo2) {
  if (pinfo1 == pinfo2) {
    return 1;
  }
  { struct _tuple3 _temp636 = ({ struct _tuple3 _temp635;
      _temp635.f1 = pinfo1;
      _temp635.f2 = pinfo2;
      _temp635; });
    void *_temp646;
    void *_temp648;
    void *_temp650;
    void *_temp652;
    void *_temp654;
    struct Cyc_Dict_Dict *_temp656;
    void *_temp658;
    struct Cyc_Dict_Dict *_temp660;
    void *_temp662;
    struct Cyc_Dict_Dict *_temp664;
    void *_temp666;
    struct Cyc_Dict_Dict *_temp668;
    _LL638: _LL651: _temp650 = _temp636.f1;
		    if (*((int *)_temp650) == Cyc_CfFlowInfo_LeafPI) {
		      _LL653: _temp652 = (void *)((struct Cyc_CfFlowInfo_LeafPI_struct *)_temp650)->f1;
			      goto _LL647;
		    }
		    else {
		      goto _LL640;
		    }
	    _LL647: _temp646 = _temp636.f2;
		    if (*((int *)_temp646) == Cyc_CfFlowInfo_LeafPI) {
		      _LL649: _temp648 = (void *)((struct Cyc_CfFlowInfo_LeafPI_struct *)_temp646)->f1;
			      goto _LL639;
		    }
		    else {
		      goto _LL640;
		    }
    _LL640: _LL659: _temp658 = _temp636.f1;
		    if (*((int *)_temp658) == Cyc_CfFlowInfo_TuplePI) {
		      _LL661: _temp660 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_TuplePI_struct *)_temp658)->f1;
			      goto _LL655;
		    }
		    else {
		      goto _LL642;
		    }
	    _LL655: _temp654 = _temp636.f2;
		    if (*((int *)_temp654) == Cyc_CfFlowInfo_TuplePI) {
		      _LL657: _temp656 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_TuplePI_struct *)_temp654)->f1;
			      goto _LL641;
		    }
		    else {
		      goto _LL642;
		    }
    _LL642: _LL667: _temp666 = _temp636.f1;
		    if (*((int *)_temp666) == Cyc_CfFlowInfo_StructPI) {
		      _LL669: _temp668 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_StructPI_struct *)_temp666)->f1;
			      goto _LL663;
		    }
		    else {
		      goto _LL644;
		    }
	    _LL663: _temp662 = _temp636.f2;
		    if (*((int *)_temp662) == Cyc_CfFlowInfo_StructPI) {
		      _LL665: _temp664 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_StructPI_struct *)_temp662)->f1;
			      goto _LL643;
		    }
		    else {
		      goto _LL644;
		    }
    _LL644: goto _LL645;
    _LL639: return _temp652 == _temp648 ? 1 : Cyc_CfFlowInfo_initstate_lessthan_approx(_temp652,
										       _temp648);
    _LL641: return _temp660 == _temp656 ? 1 : ((int (*)(int (*f)(int,void *,
								 void *),
							struct Cyc_Dict_Dict *d1,
							struct Cyc_Dict_Dict *d2))Cyc_Dict_forall_intersect)((int (*)(int ignore,
														      void *pinfo1,
														      void *pinfo2))Cyc_CfFlowInfo_pinfo_lessthan_approx,
													     _temp660,
													     _temp656);
    _LL643: return _temp668 == _temp664 ? 1 : ((int (*)(int (*f)(struct _tagged_string *,
								 void *,void *),
							struct Cyc_Dict_Dict *d1,
							struct Cyc_Dict_Dict *d2))Cyc_Dict_forall_intersect)((int (*)(struct _tagged_string *ignore,
														      void *pinfo1,
														      void *pinfo2))Cyc_CfFlowInfo_pinfo_lessthan_approx,
													     _temp668,
													     _temp664);
    _LL645: (void)_throw((void *)({ struct Cyc_Core_Impossible_struct *_temp670 = (struct Cyc_Core_Impossible_struct *)GC_malloc(sizeof(struct Cyc_Core_Impossible_struct));
			   _temp670[0] = ({ struct Cyc_Core_Impossible_struct _temp671;
			     _temp671.tag = Cyc_Core_Impossible;
			     _temp671.f1 = (struct _tagged_string)({ char *_temp672 = (char *)"pinfo_lessthan";
			       struct _tagged_string _temp673;
			       _temp673.curr = _temp672;
			       _temp673.base = _temp672;
			       _temp673.last_plus_one = _temp672 + 15;
			       _temp673; });
			     _temp671; });
			   _temp670; }));
    _LL637: ;}
  
}
int Cyc_CfFlowInfo_flow_lessthan_approx(void *f1,void *f2) {
  if (f1 == f2) {
    return 1;
  }
  { struct _tuple3 _temp675 = ({ struct _tuple3 _temp674;
      _temp674.f1 = f1;
      _temp674.f2 = f2;
      _temp674; });
    void *_temp683;
    void *_temp685;
    void *_temp687;
    void *_temp689;
    void *_temp691;
    struct Cyc_Dict_Dict *_temp693;
    void *_temp695;
    struct Cyc_Dict_Dict *_temp697;
    _LL677: _LL686: _temp685 = _temp675.f1;
		    if (_temp685 == (void *)Cyc_CfFlowInfo_BottomFL) {
		      goto _LL684;
		    }
		    else {
		      goto _LL679;
		    }
	    _LL684: _temp683 = _temp675.f2;
		    goto _LL678;
    _LL679: _LL690: _temp689 = _temp675.f1;
		    goto _LL688;
	    _LL688: _temp687 = _temp675.f2;
		    if (_temp687 == (void *)Cyc_CfFlowInfo_BottomFL) {
		      goto _LL680;
		    }
		    else {
		      goto _LL681;
		    }
    _LL681: _LL696: _temp695 = _temp675.f1;
		    if ((unsigned int)_temp695 > 1 ? *((int *)_temp695) == Cyc_CfFlowInfo_InitsFL : 0) {
		      _LL698: _temp697 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_InitsFL_struct *)_temp695)->f1;
			      goto _LL692;
		    }
		    else {
		      goto _LL676;
		    }
	    _LL692: _temp691 = _temp675.f2;
		    if ((unsigned int)_temp691 > 1 ? *((int *)_temp691) == Cyc_CfFlowInfo_InitsFL : 0) {
		      _LL694: _temp693 = (struct Cyc_Dict_Dict *)((struct Cyc_CfFlowInfo_InitsFL_struct *)_temp691)->f1;
			      goto _LL682;
		    }
		    else {
		      goto _LL676;
		    }
    _LL678: return 1;
    _LL680: return 0;
    _LL682: if (_temp697 == _temp693) {
	      return 1;
	    }
	    return ((int (*)(int (*f)(void *,void *,void *),struct Cyc_Dict_Dict *d1,
			     struct Cyc_Dict_Dict *d2))Cyc_Dict_forall_intersect)((int (*)(void *ignore,
											   void *pinfo1,
											   void *pinfo2))Cyc_CfFlowInfo_pinfo_lessthan_approx,
										  _temp697,
										  _temp693);
    _LL676: ;}
  
}
