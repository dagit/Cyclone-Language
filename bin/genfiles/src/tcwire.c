#include "cyc_include.h"

struct _tuple0 {
  void *f1;
  void *f2;
};
struct _tuple1 {
  void *f1;
  void *f2;
  void *f3;
};
struct _tuple2 {
  struct List_cons *f1;
  struct List_cons *f2;
};
struct _tuple3 {
  struct List_cons *f1;
  struct List_cons *f2;
  struct List_cons *f3;
};
struct _tuple4 {
  struct List_cons *f1;
  struct _tagged_string *f2;
};
struct _tuple5 {
  struct Core_Opt *f1;
  struct Absyn_Tqual *f2;
  void *f3;
};
struct _tuple6 {
  struct _tagged_string *f1;
  struct Absyn_Tqual *f2;
  void *f3;
};
struct _tagged_array0 {
  unsigned int sz;
  struct _tagged_string **contents;
};
struct _tagged_array1 {
  unsigned int sz;
  void **contents;
};
struct __sFILE;
typedef struct __sFILE FILE;
extern struct __sFILE *cyc_stdout;
extern struct __sFILE *cyc_stdin;
extern struct __sFILE *cyc_stderr;
typedef unsigned int uint;
typedef char *Cstring;
typedef struct _tagged_string *string;
extern struct _tagged_string *new_string(int);
extern char *string_to_Cstring(struct _tagged_string *);
extern int system(char *);
extern int f_string_read(struct __sFILE *,struct _tagged_string *,int,int);
extern int f_string_write(struct __sFILE *,struct _tagged_string *,int,int);
extern int f_seek(struct __sFILE *,int);
extern int fflush(struct __sFILE *);
extern int fgetc(struct __sFILE *);
extern char Core__FileOpenError__tag[19];
struct Core__FileOpenError__struct {
  char *tag;
  struct _tagged_string *f1;
};
extern char Core__FileCloseError__tag[20];
struct Core__FileCloseError__struct {
  char *tag;
};
extern struct __sFILE *Core_file_open(struct _tagged_string *fname,struct _tagged_string *mode);
extern void Core_file_close(struct __sFILE *);
extern int Core_file_delete(struct _tagged_string *);
extern int Core_file_length(struct _tagged_string *);
extern struct _tagged_string *Core_get_env(struct _tagged_string *);
extern struct _tagged_array0 *Core_std_args();
struct Core_Opt {
  void *v;
};
typedef struct Core_Opt *Core_Opt_t;
extern struct Core_Opt *Core_opt_map(void *(*f)(void *),struct Core_Opt *x);
extern bool Core_true_f(void *);
extern bool Core_false_f(void *);
extern void *Core_fst(struct _tuple0 *);
extern void *Core_snd(struct _tuple0 *);
extern void *Core_third(struct _tuple1 *);
extern void *Core_identity(void *);
extern int Core_intcmp(int,int);
extern int Core_charcmp(char,char);
extern char Core__InvalidArg__tag[16];
struct Core__InvalidArg__struct {
  char *tag;
  struct _tagged_string *f1;
};
extern char Core__Failure__tag[13];
struct Core__Failure__struct {
  char *tag;
  struct _tagged_string *f1;
};
extern char Core__Impossible__tag[16];
struct Core__Impossible__struct {
  char *tag;
  struct _tagged_string *f1;
};
extern char Core__Not_found__tag[15];
struct Core__Not_found__struct {
  char *tag;
};
extern char Core__Unreachable__tag[17];
struct Core__Unreachable__struct {
  char *tag;
  struct _tagged_string *f1;
};
extern bool Core_is_space(char);
extern int Core_int_of_string(struct _tagged_string *);
extern struct _tagged_string *Core_string_of_int(int);
extern struct _tagged_string *Core_string_of_uint(unsigned int);
extern struct _tagged_string *Core_string_of_char(char);
extern Bool Core_box_bool(bool);
extern Char Core_box_char(char);
extern Short Core_box_short(short);
extern _LongLong Core_box_long_long(long long);
extern Float Core_box_float(float);
extern Double Core_box_double(double);
extern bool Core_unbox_bool(Bool);
extern char Core_unbox_char(Char);
extern short Core_unbox_short(Short);
extern long long Core_unbox_long_long(_LongLong);
extern float Core_unbox_float(Float);
extern double Core_unbox_double(Double);
typedef unsigned int Core_size_t;
struct List_cons {
  void *hd;
  struct List_cons *tl;
};
typedef struct List_cons *List_list;
typedef struct List_cons *List_List;
extern int List_length(struct List_cons *x);
extern char List__List_empty__tag[16];
struct List__List_empty__struct {
  char *tag;
};
extern void *List_hd(struct List_cons *x);
extern struct List_cons *List_tl(struct List_cons *x);
extern struct List_cons *List_copy(struct List_cons *src);
extern struct List_cons *List_map(void *(*f)(void *),struct List_cons *x);
extern struct List_cons *List_map_c(void *(*f)(void *,void *),void *env,struct List_cons *x);
extern char List__List_mismatch__tag[19];
struct List__List_mismatch__struct {
  char *tag;
};
extern struct List_cons *List_map2(void *(*f)(void *,void *),struct List_cons *x,
				   struct List_cons *y);
extern void List_app(void *(*f)(void *),struct List_cons *x);
extern void List_app_c(void *(*f)(void *,void *),void *,struct List_cons *x);
extern void List_app2(void *(*f)(void *,void *),struct List_cons *x,struct List_cons *y);
extern void List_app2_c(void *(*f)(void *,void *,void *),void *env,struct List_cons *x,
			struct List_cons *y);
extern void List_iter(void (*f)(void *),struct List_cons *x);
extern void List_iter_c(void (*f)(void *,void *),void *env,struct List_cons *x);
extern void List_iter2(void (*f)(void *,void *),struct List_cons *x,struct List_cons *y);
extern void List_iter2_c(void (*f)(void *,void *,void *),void *env,struct List_cons *x,
			 struct List_cons *y);
extern void *List_fold_left(void *(*f)(void *,void *),void *accum,struct List_cons *x);
extern void *List_fold_left_c(void *(*f)(void *,void *,void *),void *,void *accum,
			      struct List_cons *x);
extern void *List_fold_right(void *(*f)(void *,void *),struct List_cons *x,
			     void *accum);
extern void *List_fold_right_c(void *(*f)(void *,void *,void *),void *,struct List_cons *x,
			       void *accum);
extern struct List_cons *List_revappend(struct List_cons *x,struct List_cons *y);
extern struct List_cons *List_rev(struct List_cons *x);
extern struct List_cons *List_imp_rev(struct List_cons *x);
extern struct List_cons *List_append(struct List_cons *x,struct List_cons *y);
extern struct List_cons *List_imp_append(struct List_cons *x,struct List_cons *y);
extern struct List_cons *List_flatten(struct List_cons *x);
extern struct List_cons *List_merge_sort(int (*less_eq)(void *,void *),struct List_cons *x);
extern struct List_cons *List_merge(int (*less_eq)(void *,void *),struct List_cons *a,
				    struct List_cons *b);
extern char List__Nth__tag[9];
struct List__Nth__struct {
  char *tag;
};
extern void *List_nth(struct List_cons *x,int i);
extern bool List_forall(bool (*pred)(void *),struct List_cons *x);
extern bool List_forall_c(bool (*pred)(void *,void *),void *env,struct List_cons *x);
extern bool List_exists(bool (*pred)(void *),struct List_cons *x);
extern struct List_cons *List_zip(struct List_cons *x,struct List_cons *y);
extern struct _tuple2 List_split(struct List_cons *x);
extern struct _tuple3 List_split3(struct List_cons *x);
extern bool List_memq(struct List_cons *l,void *x);
extern bool List_mem(int (*compare)(void *,void *),struct List_cons *l,void *x);
extern void *List_assoc(struct List_cons *l,void *x);
extern void *List_assoc_cmp(int (*compare)(void *,void *),struct List_cons *l,
			    void *x);
extern bool List_mem_assoc(struct List_cons *l,void *x);
extern struct Core_Opt *List_check_unique(int (*compare)(void *,void *),struct List_cons *x);
extern struct _tagged_array1 *List_to_array(struct List_cons *x);
extern struct List_cons *List_from_array(struct _tagged_array1 *arr);
extern struct List_cons *List_tabulate(int n,void *(*f)(int));
extern struct List_cons *List_tabulate_c(int n,void *(*f)(void *,int),void *env);
struct Lineno_pos {
  struct _tagged_string *logical_file;
  struct _tagged_string *line;
  int line_no;
  int col;
};
typedef struct Lineno_pos *Lineno_Pos;
extern struct Lineno_pos *Lineno_pos_of_abs(struct _tagged_string *,int);
extern void Lineno_poss_of_abss(struct _tagged_string *filename,struct List_cons *places);
extern char Position__Exit__tag[14];
struct Position__Exit__struct {
  char *tag;
};
extern void Position_reset_position(struct _tagged_string *);
extern void Position_set_position_file(struct _tagged_string *);
extern struct _tagged_string *Position_get_position_file();
struct Position_Segment;
typedef struct Position_Segment *Position_segment;
extern struct Position_Segment *Position_segment_of_abs(int,int);
extern struct Position_Segment *Position_segment_join(struct Position_Segment *,
						      struct Position_Segment *);
extern struct _tagged_string *Position_string_of_loc(int);
extern struct _tagged_string *Position_string_of_segment(struct Position_Segment *);
extern struct List_cons *Position_strings_of_segments(struct List_cons *);
typedef void *Position_Error_kind;
extern void *Position_Lex;
extern void *Position_Parse;
extern void *Position_Elab;
typedef void *Position_error_kind;
struct Position_Error {
  struct _tagged_string *source;
  struct Position_Segment *seg;
  void *kind;
  struct _tagged_string *desc;
};
typedef struct Position_Error *Position_error;
extern struct Position_Error *Position_mk_err_lex(struct Position_Segment *,
						  struct _tagged_string *);
extern struct Position_Error *Position_mk_err_parse(struct Position_Segment *,
						    struct _tagged_string *);
extern struct Position_Error *Position_mk_err_elab(struct Position_Segment *,
						   struct _tagged_string *);
extern char Position__Nocontext__tag[19];
struct Position__Nocontext__struct {
  char *tag;
};
extern bool Position_print_context;
extern void Position_post_error(struct Position_Error *);
extern bool Position_error_p();
typedef struct _tagged_string *Absyn_tvar;
typedef struct _tagged_string *Absyn_field_name;
typedef struct _tagged_string *Absyn_var;
typedef struct _tuple4 *Absyn_qvar;
typedef struct _tuple4 *Absyn_typedef_name;
typedef void *Absyn_Scope;
struct Absyn_Tqual;
typedef void *Absyn_Size_of;
typedef void *Absyn_Kind;
typedef void *Absyn_Array_kind;
typedef void *Absyn_Sign;
typedef void *Absyn_Box;
struct Absyn_Conref;
typedef void *Absyn_Constraint;
typedef void *Absyn_Typ;
typedef void *Absyn_Funcparams;
typedef void *Absyn_Type_modifier;
typedef void *Absyn_Cnst;
typedef void *Absyn_Primop;
typedef void *Absyn_Incrementor;
typedef void *Absyn_Raw_exp;
struct Absyn_Exp;
typedef void *Absyn_Raw_stmt;
struct Absyn_Stmt;
typedef void *Absyn_Raw_pat;
struct Absyn_Pat;
typedef void *Absyn_Binding;
struct Absyn_Switch_clause;
struct Absyn_Fndecl;
struct Absyn_Structdecl;
struct Absyn_Enumfield;
struct Absyn_Enumdecl;
struct Absyn_Xenumdecl;
struct Absyn_Typedefdecl;
struct Absyn_Vardecl;
typedef void *Absyn_Raw_decl;
struct Absyn_Decl;
typedef void *Absyn_Designator;
typedef void *Absyn_scope;
typedef struct Absyn_Tqual *Absyn_tqual;
typedef void *Absyn_size_of;
typedef void *Absyn_kind;
typedef void *Absyn_array_kind;
typedef void *Absyn_sign;
typedef void *Absyn_boxed;
typedef struct Absyn_Conref *Absyn_conref;
typedef void *Absyn_constraint;
typedef void *Absyn_typ;
typedef void *Absyn_funcparams;
typedef void *Absyn_type_modifier;
typedef void *Absyn_cnst;
typedef void *Absyn_primop;
typedef void *Absyn_incrementor;
typedef void *Absyn_raw_exp;
typedef struct Absyn_Exp *Absyn_exp;
typedef void *Absyn_raw_stmt;
typedef struct Absyn_Stmt *Absyn_stmt;
typedef void *Absyn_raw_pat;
typedef struct Absyn_Pat *Absyn_pat;
typedef void *Absyn_binding_t;
typedef struct Absyn_Switch_clause *Absyn_switch_clause;
typedef struct Absyn_Fndecl *Absyn_fndecl;
typedef struct Absyn_Structdecl *Absyn_structdecl;
typedef struct Absyn_Enumfield *Absyn_enumfield;
typedef struct Absyn_Enumdecl *Absyn_enumdecl;
typedef struct Absyn_Xenumdecl *Absyn_xenumdecl;
typedef struct Absyn_Typedefdecl *Absyn_typedefdecl;
typedef struct Absyn_Vardecl *Absyn_vardecl;
typedef void *Absyn_raw_decl;
typedef struct Absyn_Decl *Absyn_decl;
typedef void *Absyn_designator;
extern void *Absyn_Static;
extern void *Absyn_Abstract;
extern void *Absyn_Public;
extern void *Absyn_Extern;
struct Absyn_Tqual {
  bool q_const;
  bool q_volatile;
  bool q_restrict;
};
extern void *Absyn_B1;
extern void *Absyn_B2;
extern void *Absyn_B4;
extern void *Absyn_B8;
extern void *Absyn_BoxKind;
extern void *Absyn_RegKind;
extern void *Absyn_MemKind;
extern void *Absyn_UntaggedArray;
extern void *Absyn_TaggedArray;
extern const int Absyn__FixedArray__tag;
struct Absyn__FixedArray__struct {
  int tag;
  struct Absyn_Exp *f1;
};
extern void *Absyn_Signed;
extern void *Absyn_Unsigned;
extern void *Absyn_Boxed;
extern void *Absyn_Unboxed;
struct Absyn_Conref {
  void *v;
};
extern const int Absyn__Eq_constr__tag;
struct Absyn__Eq_constr__struct {
  int tag;
  void *f1;
};
extern const int Absyn__Forward_constr__tag;
struct Absyn__Forward_constr__struct {
  int tag;
  struct Absyn_Conref *f1;
};
extern void *Absyn_No_constr;
extern void *Absyn_VoidType;
extern const int Absyn__Evar__tag;
struct Absyn__Evar__struct {
  int tag;
  void *f1;
  struct Core_Opt *f2;
  int f3;
};
extern const int Absyn__VarType__tag;
struct Absyn__VarType__struct {
  int tag;
  struct _tagged_string *f1;
};
extern const int Absyn__EnumType__tag;
struct Absyn__EnumType__struct {
  int tag;
  struct Core_Opt *f1;
  struct List_cons *f2;
};
extern const int Absyn__XenumType__tag;
struct Absyn__XenumType__struct {
  int tag;
  struct _tuple4 *f1;
};
extern const int Absyn__PointerType__tag;
struct Absyn__PointerType__struct {
  int tag;
  void *f1;
  struct Absyn_Conref *f2;
  struct Absyn_Tqual *f3;
};
extern const int Absyn__IntType__tag;
struct Absyn__IntType__struct {
  int tag;
  void *f1;
  void *f2;
  void *f3;
};
extern const int Absyn__BoolType__tag;
struct Absyn__BoolType__struct {
  int tag;
  void *f1;
};
extern const int Absyn__FloatType__tag;
struct Absyn__FloatType__struct {
  int tag;
  void *f1;
};
extern const int Absyn__DoubleType__tag;
struct Absyn__DoubleType__struct {
  int tag;
  void *f1;
};
extern const int Absyn__ArrayType__tag;
struct Absyn__ArrayType__struct {
  int tag;
  void *f1;
  struct Absyn_Tqual *f2;
  void *f3;
};
extern const int Absyn__FnType__tag;
struct Absyn__FnType__struct {
  int tag;
  struct List_cons *f1;
  void *f2;
  struct List_cons *f3;
  bool f4;
};
extern const int Absyn__TupleType__tag;
struct Absyn__TupleType__struct {
  int tag;
  struct List_cons *f1;
};
extern const int Absyn__StructType__tag;
struct Absyn__StructType__struct {
  int tag;
  struct Core_Opt *f1;
  struct List_cons *f2;
};
extern const int Absyn__TypedefType__tag;
struct Absyn__TypedefType__struct {
  int tag;
  struct _tuple4 *f1;
  struct List_cons *f2;
  struct Core_Opt *f3;
};
extern void *Absyn_UnionType;
extern const int Absyn__NoTypes__tag;
struct Absyn__NoTypes__struct {
  int tag;
  struct List_cons *f1;
  struct Position_Segment *f2;
};
extern const int Absyn__WithTypes__tag;
struct Absyn__WithTypes__struct {
  int tag;
  struct List_cons *f1;
  bool f2;
};
extern void *Absyn_Carray_mod;
extern void *Absyn_Array_mod;
extern const int Absyn__ConstArray_mod__tag;
struct Absyn__ConstArray_mod__struct {
  int tag;
  struct Absyn_Exp *f1;
};
extern const int Absyn__Pointer_mod__tag;
struct Absyn__Pointer_mod__struct {
  int tag;
  bool f1;
  struct Absyn_Tqual *f2;
};
extern const int Absyn__Function_mod__tag;
struct Absyn__Function_mod__struct {
  int tag;
  void *f1;
};
extern const int Absyn__TypeParams_mod__tag;
struct Absyn__TypeParams_mod__struct {
  int tag;
  struct List_cons *f1;
  struct Position_Segment *f2;
};
extern const int Absyn__Char_c__tag;
struct Absyn__Char_c__struct {
  int tag;
  void *f1;
  char f2;
};
extern const int Absyn__Short_c__tag;
struct Absyn__Short_c__struct {
  int tag;
  void *f1;
  short f2;
};
extern const int Absyn__Int_c__tag;
struct Absyn__Int_c__struct {
  int tag;
  void *f1;
  int f2;
};
extern const int Absyn__LongLong_c__tag;
struct Absyn__LongLong_c__struct {
  int tag;
  void *f1;
  long long f2;
};
extern const int Absyn__Float_c__tag;
struct Absyn__Float_c__struct {
  int tag;
  struct _tagged_string *f1;
};
extern const int Absyn__Bool_c__tag;
struct Absyn__Bool_c__struct {
  int tag;
  bool f1;
};
extern const int Absyn__String_c__tag;
struct Absyn__String_c__struct {
  int tag;
  struct _tagged_string *f1;
};
extern void *Absyn_Null_c;
extern void *Absyn_Plus;
extern void *Absyn_Times;
extern void *Absyn_Minus;
extern void *Absyn_Div;
extern void *Absyn_Mod;
extern void *Absyn_Eq;
extern void *Absyn_Neq;
extern void *Absyn_Gt;
extern void *Absyn_Lt;
extern void *Absyn_Gte;
extern void *Absyn_Lte;
extern void *Absyn_Not;
extern void *Absyn_Bitnot;
extern void *Absyn_Bitand;
extern void *Absyn_Bitor;
extern void *Absyn_Bitxor;
extern void *Absyn_Bitlshift;
extern void *Absyn_Bitlrshift;
extern void *Absyn_Bitarshift;
extern void *Absyn_Size;
extern void *Absyn_Printf;
extern void *Absyn_Fprintf;
extern void *Absyn_Xprintf;
extern void *Absyn_PreInc;
extern void *Absyn_PostInc;
extern void *Absyn_PreDec;
extern void *Absyn_PostDec;
extern const int Absyn__Const_e__tag;
struct Absyn__Const_e__struct {
  int tag;
  void *f1;
};
extern const int Absyn__Var_e__tag;
struct Absyn__Var_e__struct {
  int tag;
  struct _tuple4 *f1;
  void *f2;
};
extern const int Absyn__UnknownId_e__tag;
struct Absyn__UnknownId_e__struct {
  int tag;
  struct _tuple4 *f1;
};
extern const int Absyn__Primop_e__tag;
struct Absyn__Primop_e__struct {
  int tag;
  void *f1;
  struct List_cons *f2;
};
extern const int Absyn__AssignOp_e__tag;
struct Absyn__AssignOp_e__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct Core_Opt *f2;
  struct Absyn_Exp *f3;
};
extern const int Absyn__Increment_e__tag;
struct Absyn__Increment_e__struct {
  int tag;
  struct Absyn_Exp *f1;
  void *f2;
};
extern const int Absyn__Conditional_e__tag;
struct Absyn__Conditional_e__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct Absyn_Exp *f2;
  struct Absyn_Exp *f3;
};
extern const int Absyn__SeqExp_e__tag;
struct Absyn__SeqExp_e__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct Absyn_Exp *f2;
};
extern const int Absyn__UnknownCall_e__tag;
struct Absyn__UnknownCall_e__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct List_cons *f2;
};
extern const int Absyn__FnCall_e__tag;
struct Absyn__FnCall_e__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct List_cons *f2;
};
extern const int Absyn__Throw_e__tag;
struct Absyn__Throw_e__struct {
  int tag;
  struct Absyn_Exp *f1;
};
extern const int Absyn__NoInstantiate_e__tag;
struct Absyn__NoInstantiate_e__struct {
  int tag;
  struct Absyn_Exp *f1;
};
extern const int Absyn__Instantiate_e__tag;
struct Absyn__Instantiate_e__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct Core_Opt *f2;
};
extern const int Absyn__Cast_e__tag;
struct Absyn__Cast_e__struct {
  int tag;
  void *f1;
  struct Absyn_Exp *f2;
};
extern const int Absyn__Address_e__tag;
struct Absyn__Address_e__struct {
  int tag;
  struct Absyn_Exp *f1;
};
extern const int Absyn__Sizeof_e__tag;
struct Absyn__Sizeof_e__struct {
  int tag;
  void *f1;
};
extern const int Absyn__Deref_e__tag;
struct Absyn__Deref_e__struct {
  int tag;
  struct Absyn_Exp *f1;
};
extern const int Absyn__StructMember_e__tag;
struct Absyn__StructMember_e__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct _tagged_string *f2;
};
extern const int Absyn__StructArrow_e__tag;
struct Absyn__StructArrow_e__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct _tagged_string *f2;
};
extern const int Absyn__Subscript_e__tag;
struct Absyn__Subscript_e__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct Absyn_Exp *f2;
};
extern const int Absyn__Tuple_e__tag;
struct Absyn__Tuple_e__struct {
  int tag;
  struct List_cons *f1;
};
extern const int Absyn__CompoundLit_e__tag;
struct Absyn__CompoundLit_e__struct {
  int tag;
  struct _tuple5 *f1;
  struct List_cons *f2;
};
extern const int Absyn__Array_e__tag;
struct Absyn__Array_e__struct {
  int tag;
  struct List_cons *f1;
};
extern const int Absyn__Comprehension_e__tag;
struct Absyn__Comprehension_e__struct {
  int tag;
  struct Absyn_Vardecl *f1;
  struct Absyn_Exp *f2;
  struct Absyn_Exp *f3;
};
extern const int Absyn__Struct_e__tag;
struct Absyn__Struct_e__struct {
  int tag;
  struct _tuple4 *f1;
  struct Core_Opt *f2;
  struct List_cons *f3;
  struct Core_Opt *f4;
};
extern const int Absyn__Enum_e__tag;
struct Absyn__Enum_e__struct {
  int tag;
  struct _tuple4 *f1;
  struct Core_Opt *f2;
  struct Core_Opt *f3;
  struct List_cons *f4;
  struct Absyn_Enumdecl *f5;
  struct Absyn_Enumfield *f6;
};
extern const int Absyn__Xenum_e__tag;
struct Absyn__Xenum_e__struct {
  int tag;
  struct _tuple4 *f1;
  struct Core_Opt *f2;
  struct List_cons *f3;
  struct Absyn_Xenumdecl *f4;
  struct Absyn_Enumfield *f5;
};
extern const int Absyn__UnresolvedMem_e__tag;
struct Absyn__UnresolvedMem_e__struct {
  int tag;
  struct Core_Opt *f1;
  struct List_cons *f2;
};
extern const int Absyn__StmtExp_e__tag;
struct Absyn__StmtExp_e__struct {
  int tag;
  struct Absyn_Stmt *f1;
};
extern const int Absyn__Codegen_e__tag;
struct Absyn__Codegen_e__struct {
  int tag;
  struct Absyn_Fndecl *f1;
};
extern const int Absyn__Fill_e__tag;
struct Absyn__Fill_e__struct {
  int tag;
  struct Absyn_Exp *f1;
};
struct Absyn_Exp {
  struct Core_Opt *topt;
  void *r;
  struct Position_Segment *loc;
};
extern void *Absyn_Skip_s;
extern const int Absyn__Exp_s__tag;
struct Absyn__Exp_s__struct {
  int tag;
  struct Absyn_Exp *f1;
};
extern const int Absyn__Seq_s__tag;
struct Absyn__Seq_s__struct {
  int tag;
  struct Absyn_Stmt *f1;
  struct Absyn_Stmt *f2;
};
extern const int Absyn__Return_s__tag;
struct Absyn__Return_s__struct {
  int tag;
  struct Core_Opt *f1;
};
extern const int Absyn__IfThenElse_s__tag;
struct Absyn__IfThenElse_s__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct Absyn_Stmt *f2;
  struct Absyn_Stmt *f3;
};
extern const int Absyn__While_s__tag;
struct Absyn__While_s__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct Absyn_Stmt *f2;
};
extern void *Absyn_Break_s;
extern void *Absyn_Continue_s;
extern const int Absyn__Goto_s__tag;
struct Absyn__Goto_s__struct {
  int tag;
  struct _tagged_string *f1;
};
extern const int Absyn__For_s__tag;
struct Absyn__For_s__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct Absyn_Exp *f2;
  struct Absyn_Exp *f3;
  struct Absyn_Stmt *f4;
};
extern const int Absyn__Switch_s__tag;
struct Absyn__Switch_s__struct {
  int tag;
  struct Absyn_Exp *f1;
  struct List_cons *f2;
};
extern const int Absyn__Fallthru_s__tag;
struct Absyn__Fallthru_s__struct {
  int tag;
  struct List_cons *f1;
};
extern const int Absyn__Decl_s__tag;
struct Absyn__Decl_s__struct {
  int tag;
  struct Absyn_Decl *f1;
  struct Absyn_Stmt *f2;
};
extern const int Absyn__Cut_s__tag;
struct Absyn__Cut_s__struct {
  int tag;
  struct Absyn_Stmt *f1;
};
extern const int Absyn__Splice_s__tag;
struct Absyn__Splice_s__struct {
  int tag;
  struct Absyn_Stmt *f1;
};
extern const int Absyn__Label_s__tag;
struct Absyn__Label_s__struct {
  int tag;
  struct _tagged_string *f1;
  struct Absyn_Stmt *f2;
};
extern const int Absyn__Do_s__tag;
struct Absyn__Do_s__struct {
  int tag;
  struct Absyn_Stmt *f1;
  struct Absyn_Exp *f2;
};
extern const int Absyn__TryCatch_s__tag;
struct Absyn__TryCatch_s__struct {
  int tag;
  struct Absyn_Stmt *f1;
  struct List_cons *f2;
};
struct Absyn_Stmt {
  void *r;
  struct Position_Segment *loc;
};
extern void *Absyn_Wild_p;
extern const int Absyn__Var_p__tag;
struct Absyn__Var_p__struct {
  int tag;
  struct Absyn_Vardecl *f1;
};
extern void *Absyn_Null_p;
extern const int Absyn__Int_p__tag;
struct Absyn__Int_p__struct {
  int tag;
  void *f1;
  int f2;
};
extern const int Absyn__Char_p__tag;
struct Absyn__Char_p__struct {
  int tag;
  char f1;
};
extern const int Absyn__Float_p__tag;
struct Absyn__Float_p__struct {
  int tag;
  struct _tagged_string *f1;
};
extern const int Absyn__Bool_p__tag;
struct Absyn__Bool_p__struct {
  int tag;
  bool f1;
};
extern const int Absyn__Tuple_p__tag;
struct Absyn__Tuple_p__struct {
  int tag;
  struct List_cons *f1;
};
extern const int Absyn__Pointer_p__tag;
struct Absyn__Pointer_p__struct {
  int tag;
  struct Absyn_Pat *f1;
};
extern const int Absyn__Reference_p__tag;
struct Absyn__Reference_p__struct {
  int tag;
  struct Absyn_Vardecl *f1;
};
extern const int Absyn__Struct_p__tag;
struct Absyn__Struct_p__struct {
  int tag;
  struct Absyn_Structdecl *f1;
  struct Core_Opt *f2;
  struct List_cons *f3;
  struct List_cons *f4;
};
extern const int Absyn__Enum_p__tag;
struct Absyn__Enum_p__struct {
  int tag;
  struct _tuple4 *f1;
  struct Core_Opt *f2;
  struct List_cons *f3;
  struct List_cons *f4;
  struct Absyn_Enumdecl *f5;
  struct Absyn_Enumfield *f6;
};
extern const int Absyn__Xenum_p__tag;
struct Absyn__Xenum_p__struct {
  int tag;
  struct _tuple4 *f1;
  struct List_cons *f2;
  struct List_cons *f3;
  struct Absyn_Xenumdecl *f4;
  struct Absyn_Enumfield *f5;
};
extern const int Absyn__UnknownId_p__tag;
struct Absyn__UnknownId_p__struct {
  int tag;
  struct _tuple4 *f1;
};
extern const int Absyn__UnknownCall_p__tag;
struct Absyn__UnknownCall_p__struct {
  int tag;
  struct _tuple4 *f1;
  struct List_cons *f2;
  struct List_cons *f3;
};
extern const int Absyn__UnknownFields_p__tag;
struct Absyn__UnknownFields_p__struct {
  int tag;
  struct _tuple4 *f1;
  struct List_cons *f2;
  struct List_cons *f3;
};
struct Absyn_Pat {
  void *r;
  struct Core_Opt *topt;
  struct Position_Segment *loc;
};
struct Absyn_Switch_clause {
  struct Absyn_Pat *pattern;
  struct Core_Opt *pat_vars;
  struct Core_Opt *where_clause;
  struct Absyn_Stmt *body;
  struct Position_Segment *loc;
};
extern void *Absyn_Unresolved_b;
extern const int Absyn__Global_b__tag;
struct Absyn__Global_b__struct {
  int tag;
  struct Absyn_Vardecl *f1;
};
extern const int Absyn__Funname_b__tag;
struct Absyn__Funname_b__struct {
  int tag;
  struct Absyn_Fndecl *f1;
};
extern const int Absyn__Param_b__tag;
struct Absyn__Param_b__struct {
  int tag;
  struct _tuple6 *f1;
};
extern const int Absyn__Local_b__tag;
struct Absyn__Local_b__struct {
  int tag;
  struct Absyn_Vardecl *f1;
};
extern const int Absyn__Pat_b__tag;
struct Absyn__Pat_b__struct {
  int tag;
  struct Absyn_Vardecl *f1;
};
struct Absyn_Vardecl {
  void *sc;
  struct _tuple4 *name;
  struct Absyn_Tqual *tq;
  void *type;
  struct Core_Opt *initializer;
  int shadow;
};
struct Absyn_Fndecl {
  void *sc;
  bool is_inline;
  struct _tuple4 *name;
  struct List_cons *tvs;
  void *ret_type;
  struct List_cons *args;
  bool varargs;
  struct Absyn_Stmt *body;
};
struct Absyn_Structdecl {
  void *sc;
  struct Core_Opt *name;
  struct List_cons *tvs;
  struct Core_Opt *fields;
};
struct Absyn_Enumfield {
  struct _tuple4 *name;
  struct Core_Opt *tag;
  struct List_cons *tvs;
  struct List_cons *typs;
  struct Position_Segment *loc;
};
struct Absyn_Enumdecl {
  void *sc;
  struct Core_Opt *name;
  struct List_cons *tvs;
  struct Core_Opt *fields;
};
struct Absyn_Xenumdecl {
  void *sc;
  struct _tuple4 *name;
  struct List_cons *fields;
};
struct Absyn_Typedefdecl {
  struct _tuple4 *name;
  struct List_cons *tvs;
  void *defn;
};
extern const int Absyn__Var_d__tag;
struct Absyn__Var_d__struct {
  int tag;
  struct Absyn_Vardecl *f1;
};
extern const int Absyn__Fn_d__tag;
struct Absyn__Fn_d__struct {
  int tag;
  struct Absyn_Fndecl *f1;
};
extern const int Absyn__Let_d__tag;
struct Absyn__Let_d__struct {
  int tag;
  struct Absyn_Pat *f1;
  struct Core_Opt *f2;
  struct Absyn_Exp *f3;
  bool f4;
};
extern const int Absyn__Struct_d__tag;
struct Absyn__Struct_d__struct {
  int tag;
  struct Absyn_Structdecl *f1;
};
extern void *Absyn_Union_d;
extern const int Absyn__Enum_d__tag;
struct Absyn__Enum_d__struct {
  int tag;
  struct Absyn_Enumdecl *f1;
};
extern const int Absyn__Xenum_d__tag;
struct Absyn__Xenum_d__struct {
  int tag;
  struct Absyn_Xenumdecl *f1;
};
extern const int Absyn__Typedef_d__tag;
struct Absyn__Typedef_d__struct {
  int tag;
  struct Absyn_Typedefdecl *f1;
};
extern const int Absyn__Namespace_d__tag;
struct Absyn__Namespace_d__struct {
  int tag;
  struct _tagged_string *f1;
  struct List_cons *f2;
};
extern const int Absyn__Using_d__tag;
struct Absyn__Using_d__struct {
  int tag;
  struct _tuple4 *f1;
  struct List_cons *f2;
};
struct Absyn_Decl {
  void *r;
  struct Position_Segment *loc;
};
extern const int Absyn__ArrayElement__tag;
struct Absyn__ArrayElement__struct {
  int tag;
  struct Absyn_Exp *f1;
};
extern const int Absyn__FieldName__tag;
struct Absyn__FieldName__struct {
  int tag;
  struct _tagged_string *f1;
};
extern int Absyn_qvar_cmp(struct _tuple4 *,struct _tuple4 *);
extern int Absyn_varlist_cmp(struct List_cons *,struct List_cons *);
extern struct Absyn_Tqual *Absyn_combine_tqual(struct Absyn_Tqual *x,struct Absyn_Tqual *y);
extern struct Absyn_Tqual *Absyn_empty_tqual();
extern void *Absyn_box_k;
extern void *Absyn_reg_k;
extern void *Absyn_mem_k;
extern struct Absyn_Conref *Absyn_new_conref(void *x);
extern struct Absyn_Conref *Absyn_empty_conref();
extern void *Absyn_new_evar(void *);
extern void *Absyn_wildtyp();
extern void *Absyn_uchar_t;
extern void *Absyn_ushort_t;
extern void *Absyn_uint_t;
extern void *Absyn_ulong_t;
extern void *Absyn_schar_t;
extern void *Absyn_sshort_t;
extern void *Absyn_sint_t;
extern void *Absyn_slong_t;
extern void *Absyn_float_t;
extern void *Absyn_double_t;
extern void *Absyn_bool_t;
extern void *Absyn_uChar_t;
extern void *Absyn_uShort_t;
extern void *Absyn_uInt_t;
extern void *Absyn_uLong_t;
extern void *Absyn_sChar_t;
extern void *Absyn_sShort_t;
extern void *Absyn_sInt_t;
extern void *Absyn_sLong_t;
extern void *Absyn_Float_t;
extern void *Absyn_Double_t;
extern void *Absyn_Bool_t;
extern void *Absyn_exn_t;
extern void *Absyn_void_t;
extern void *Absyn_string_t();
extern void *Absyn_file_t();
extern void *Absyn_nullableptr_t(void *t,struct Absyn_Tqual *tq);
extern void *Absyn_void_star_typ();
extern void *Absyn_pureptr_t(void *t,struct Absyn_Tqual *tq);
extern void *Absyn_strct(struct _tagged_string *name);
extern struct Absyn_Exp *Absyn_new_exp(void *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_copy_exp(struct Absyn_Exp *);
extern struct Absyn_Exp *Absyn_const_exp(void *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_null_exp(struct Position_Segment *);
extern struct Absyn_Exp *Absyn_bool_exp(bool,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_true_exp(struct Position_Segment *);
extern struct Absyn_Exp *Absyn_false_exp(struct Position_Segment *);
extern struct Absyn_Exp *Absyn_int_exp(void *,int,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_signed_int_exp(int,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_uint_exp(unsigned int,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_char_exp(char c,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_float_exp(struct _tagged_string *f,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_string_exp(struct _tagged_string *s,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_var_exp(struct _tuple4 *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_unknownid_exp(struct _tuple4 *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_primop_exp(void *,struct List_cons *es,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_prim1_exp(void *,struct Absyn_Exp *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_prim2_exp(void *,struct Absyn_Exp *,struct Absyn_Exp *,
					 struct Position_Segment *);
extern struct Absyn_Exp *Absyn_times_exp(struct Absyn_Exp *,struct Absyn_Exp *,
					 struct Position_Segment *);
extern struct Absyn_Exp *Absyn_eq_exp(struct Absyn_Exp *,struct Absyn_Exp *,
				      struct Position_Segment *);
extern struct Absyn_Exp *Absyn_neq_exp(struct Absyn_Exp *,struct Absyn_Exp *,
				       struct Position_Segment *);
extern struct Absyn_Exp *Absyn_gt_exp(struct Absyn_Exp *,struct Absyn_Exp *,
				      struct Position_Segment *);
extern struct Absyn_Exp *Absyn_lt_exp(struct Absyn_Exp *,struct Absyn_Exp *,
				      struct Position_Segment *);
extern struct Absyn_Exp *Absyn_gte_exp(struct Absyn_Exp *,struct Absyn_Exp *,
				       struct Position_Segment *);
extern struct Absyn_Exp *Absyn_lte_exp(struct Absyn_Exp *,struct Absyn_Exp *,
				       struct Position_Segment *);
extern struct Absyn_Exp *Absyn_assignop_exp(struct Absyn_Exp *,struct Core_Opt *,
					    struct Absyn_Exp *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_assign_exp(struct Absyn_Exp *,struct Absyn_Exp *,
					  struct Position_Segment *);
extern struct Absyn_Exp *Absyn_post_inc_exp(struct Absyn_Exp *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_post_dec_exp(struct Absyn_Exp *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_pre_inc_exp(struct Absyn_Exp *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_pre_dec_exp(struct Absyn_Exp *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_conditional_exp(struct Absyn_Exp *,struct Absyn_Exp *,
					       struct Absyn_Exp *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_and_exp(struct Absyn_Exp *,struct Absyn_Exp *,
				       struct Position_Segment *);
extern struct Absyn_Exp *Absyn_or_exp(struct Absyn_Exp *,struct Absyn_Exp *,
				      struct Position_Segment *);
extern struct Absyn_Exp *Absyn_seq_exp(struct Absyn_Exp *,struct Absyn_Exp *,
				       struct Position_Segment *);
extern struct Absyn_Exp *Absyn_unknowncall_exp(struct Absyn_Exp *,struct List_cons *,
					       struct Position_Segment *);
extern struct Absyn_Exp *Absyn_fncall_exp(struct Absyn_Exp *,struct List_cons *,
					  struct Position_Segment *);
extern struct Absyn_Exp *Absyn_throw_exp(struct Absyn_Exp *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_noinstantiate_exp(struct Absyn_Exp *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_instantiate(struct Absyn_Exp *,struct List_cons *,
					   struct Position_Segment *);
extern struct Absyn_Exp *Absyn_cast_exp(void *,struct Absyn_Exp *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_address_exp(struct Absyn_Exp *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_sizeof_exp(void *t,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_deref_exp(struct Absyn_Exp *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_structmember_exp(struct Absyn_Exp *,struct _tagged_string *,
						struct Position_Segment *);
extern struct Absyn_Exp *Absyn_structarrow_exp(struct Absyn_Exp *,struct _tagged_string *,
					       struct Position_Segment *);
extern struct Absyn_Exp *Absyn_subscript_exp(struct Absyn_Exp *,struct Absyn_Exp *,
					     struct Position_Segment *);
extern struct Absyn_Exp *Absyn_tuple_exp(struct List_cons *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_stmt_exp(struct Absyn_Stmt *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_null_pointer_exn_exp(struct Position_Segment *);
extern struct Absyn_Exp *Absyn_array_exp(struct List_cons *,struct Position_Segment *);
extern struct Absyn_Exp *Absyn_unresolvedmem_exp(struct Core_Opt *,struct List_cons *,
						 struct Position_Segment *);
extern struct Absyn_Stmt *Absyn_new_stmt(void *s,struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_skip_stmt(struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_exp_stmt(struct Absyn_Exp *e,struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_seq_stmt(struct Absyn_Stmt *s1,struct Absyn_Stmt *s2,
					 struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_seq_stmts(struct List_cons *,struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_return_stmt(struct Core_Opt *e,struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_ifthenelse_stmt(struct Absyn_Exp *e,struct Absyn_Stmt *s1,
						struct Absyn_Stmt *s2,struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_while_stmt(struct Absyn_Exp *e,struct Absyn_Stmt *s,
					   struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_break_stmt(struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_continue_stmt(struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_for_stmt(struct Absyn_Exp *e1,struct Absyn_Exp *e2,
					 struct Absyn_Exp *e3,struct Absyn_Stmt *s,
					 struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_switch_stmt(struct Absyn_Exp *e,struct List_cons *,
					    struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_fallthru_stmt(struct List_cons *el,struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_decl_stmt(struct Absyn_Decl *d,struct Absyn_Stmt *s,
					  struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_declare_stmt(struct _tuple4 *,void *,struct Core_Opt *init,
					     struct Absyn_Stmt *,struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_cut_stmt(struct Absyn_Stmt *s,struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_splice_stmt(struct Absyn_Stmt *s,struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_label_stmt(struct _tagged_string *v,struct Absyn_Stmt *s,
					   struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_do_stmt(struct Absyn_Stmt *s,struct Absyn_Exp *e,
					struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_trycatch_stmt(struct Absyn_Stmt *s,struct List_cons *scs,
					      struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_goto_stmt(struct _tagged_string *lab,struct Position_Segment *loc);
extern struct Absyn_Stmt *Absyn_assign_stmt(struct Absyn_Exp *e1,struct Absyn_Exp *e2,
					    struct Position_Segment *loc);
extern struct Absyn_Pat *Absyn_new_pat(void *p,struct Position_Segment *s);
extern struct Absyn_Decl *Absyn_new_decl(void *r,struct Position_Segment *loc);
extern struct Absyn_Decl *Absyn_let_decl(struct Absyn_Pat *p,struct Core_Opt *t_opt,
					 struct Absyn_Exp *e,struct Position_Segment *loc);
extern struct Absyn_Vardecl *Absyn_new_vardecl(struct _tuple4 *x,void *t,
					       struct Core_Opt *init);
extern struct Absyn_Vardecl *Absyn_static_vardecl(struct _tuple4 *x,void *t,
						  struct Core_Opt *init);
extern struct Absyn_Decl *Absyn_struct_decl(void *s,struct Core_Opt *n,struct List_cons *ts,
					    struct Core_Opt *fs,struct Position_Segment *loc);
extern struct Absyn_Decl *Absyn_enum_decl(void *s,struct Core_Opt *n,struct List_cons *ts,
					  struct Core_Opt *fs,struct Position_Segment *loc);
extern struct Absyn_Decl *Absyn_xenum_decl(void *s,struct _tuple4 *n,struct List_cons *fs,
					   struct Position_Segment *loc);
extern bool Absyn_is_format_prim(void *p);
extern void *Absyn_function_typ(struct List_cons *,void *,struct List_cons *,
				bool);
extern void *Absyn_pointer_expand(void *);
extern bool Absyn_is_lvalue(struct Absyn_Exp *);
void Tcwire_rewire(struct Absyn_Fndecl *fd) {
  ;
}
