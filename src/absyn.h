#ifndef _ABSYN_H_
#define _ABSYN_H_

// The abstract syntax has (at last count) three different "stages":
// A. Result of parsing
// B. Result of type-checking
// C. Result of translation to C
// Each stage obeys different invariants of which the compiler hacker
// (that's you) must be aware.  For example, here is an incomplete list
// of things that must be true after type-checking:
//  B1. All qvars have the correct namespace list in them
//  B2. All "destination fields" and non_local_preds fields in stmt objects
//      must be correct.
//  B3. Every expression must have a non-null and correct type field.
//  B4. None of the "unresolved" variants are used.  (There may be 
//      underconstrained Evars for types.)
//  B5. The pat_vars field of switch_caluse is non-null and correct.
// Note that translation to C willfully violates B2 and B3, but that's okay.

// A cute hack to avoid defining the abstract syntax twice.
#ifdef ABSYN_CYC
#define EXTERN_DEFINITION
#else
#define EXTERN_DEFINITION extern
#endif

#include "core.h"
#include "list.h"
#include "position.h"

namespace Absyn {
  using Core;
  using List;
  using Position;

  typedef stringptr field_name;
  typedef stringptr var;
  typedef stringptr tvarname_t;
  typedef $(List::list<var>,var)@ qvar;
  typedef qvar typedef_name_t;
  
  // forward declarations
  extern enum Scope;
  extern struct Tqual;
  extern enum Size_of;
  extern enum Kind;
  extern enum Array_kind;
  extern enum Sign;
  extern struct Conref<`a>;
  extern enum Constraint<`a>;
  extern enum Bounds;
  extern struct PtrInfo;
  extern enum Typ;
  extern enum Funcparams;
  extern enum Type_modifier;
  extern enum Cnst;
  extern enum Primop;
  extern enum Incrementor;
  extern enum Raw_exp;
  extern struct Exp;
  extern enum Raw_stmt;
  extern struct Stmt;
  extern enum Raw_pat; 
  extern struct Pat;
  extern enum Binding;
  extern struct Switch_clause;
  extern struct Fndecl;
  extern struct Structdecl;
  extern struct Enumfield;
  extern struct Enumdecl;
  extern struct Xenumdecl;
  extern struct Typedefdecl;
  extern struct Vardecl;
  extern enum Raw_decl;
  extern struct Decl;
  extern enum Designator;
  extern xenum StmtAnnot;

  typedef enum Scope scope;
  typedef struct Tqual @tqual;
  typedef enum Size_of size_of_t;
  typedef enum Kind kind_t;
  typedef $(tvarname_t,kind_t)@ tvar; // resolved kind via in-place mutation
  typedef enum Sign sign;
  typedef struct Conref<`a> @conref<`a>;
  typedef enum Constraint<`a> constraint_t<`a>;
  typedef enum Bounds bounds_t;
  typedef struct PtrInfo ptr_info_t;
  typedef enum Typ typ;
  typedef enum Funcparams funcparams_t;
  typedef enum Type_modifier type_modifier;
  typedef enum Cnst cnst_t;
  typedef enum Primop primop;
  typedef enum Incrementor incrementor_t;
  typedef enum Raw_exp raw_exp_t;
  typedef struct Exp @exp;
  typedef enum Raw_stmt raw_stmt_t;
  typedef struct Stmt @stmt;
  typedef enum Raw_pat raw_pat_t;
  typedef struct Pat @pat;
  typedef enum Binding binding_t;
  typedef struct Switch_clause @switch_clause;
  typedef struct Fndecl @fndecl;
  typedef struct Structdecl @structdecl;
  typedef struct Enumfield @enumfield;
  typedef struct Enumdecl @enumdecl;
  typedef struct Xenumdecl @xenumdecl;
  typedef struct Typedefdecl @typedefdecl;
  typedef struct Vardecl @vardecl;
  typedef enum Raw_decl raw_decl;
  typedef struct Decl @decl;
  typedef enum Designator designator;
  typedef xenum StmtAnnot stmt_annot_t;

  EXTERN_DEFINITION enum Scope { Static, Abstract, Public, Extern, ExternC };
  EXTERN_DEFINITION struct Tqual { 
    bool q_const; bool q_volatile; bool q_restrict; 
  };
  EXTERN_DEFINITION enum Size_of { B1, B2, B4, B8 };
  // Note: RgnKind is incomparable to any other kind.
  // Note: Perhaps unpackable should be the default since existentials are
  //       rare??
  // FIX: There is no way to create tyvars of unpackable kinds in the 
  //      concrete syntax.
  // FIX: There is no concrete syntax for abstract types take a type of
  //      some kind other than BoxPKind (which should remain the default).
  // FIX: There's no reason data types couldn't be packable polymorphic,
  //      but it's not at all clear what sane syntax would look like.
  //      Until then, you won't be able to create a list of region handles.
  // FIX: Is there any reason to distinguish Reg and Mem so long as we're
  //      translating to C which allows mem assignment and param passing?
  EXTERN_DEFINITION enum Kind { 
    MemUKind,
    BoxUKind,
    MemPKind,
    BoxPKind,
    RgnKind,
    UnresolvedKind
  };
  EXTERN_DEFINITION enum Sign { Signed, Unsigned };
  EXTERN_DEFINITION struct Conref<`a> { constraint_t<`a> v; };
  EXTERN_DEFINITION enum Constraint<`a> { 
    Eq_constr(`a), Forward_constr(conref<`a>), No_constr 
  };

  EXTERN_DEFINITION enum Bounds {
    Unknown_b;    // t?
    Upper_b(exp); // t*{x:x>=0 && x < e} and t@{x:x>=0 && x < e}
  };

  EXTERN_DEFINITION struct PtrInfo {
    typ              elt_typ;   // typ of value to which pointer points
    typ              rgn_typ;   // region of value to which pointer points
    conref<bool>     nullable;  // * or @
    tqual            tq;
    conref<bounds_t> bounds;    // legal bounds for pointer indexing
  };

  // Note: The last fields of EnumType, XenumType, StructType and TypedefType
  // are all set by check_valid_type which most of the compiler assumes
  // has been called.  Doing so avoids the need for some code to have and use
  // a type-name environment just like variable-binding fields avoid the need
  // for some code to have a variable environment.
  // Note: Having HeapRgnType and VarType's of RgnKind is a sort of kludge 
  //       so we can use one unification, substitution, etc. 
  //       Certain typ fields must have RgnKind and
  //       another must not have RgnKind.  Wouldn't refinement types be nice?
  // FIX: Change a lot of the abstract-syntaxes options to nullable pointers.
  //      For example, the last field of TypedefType
  // FIX: May want to make this raw_typ and store the kinds with the types.
  EXTERN_DEFINITION enum Typ {
    VoidType;
    Evar(kind_t,Opt_t<typ>,int);
    VarType(tvar); // kind induced by tvar
    EnumType(Opt_t<typedef_name_t>,list<typ>,enumdecl *);
    XenumType(typedef_name_t,xenumdecl *);
    PointerType(ptr_info_t);
    IntType(sign,size_of_t);
    FloatType;
    DoubleType;
    ArrayType(typ/* element typ*/,tqual,exp /* size */);
    FnType(list<tvar>,typ,list<$(Opt_t<var>,tqual,typ)@>,bool);
    TupleType(list<$(tqual,typ)@>);
    StructType(Opt_t<typedef_name_t>,list<typ>,structdecl *);
    TypedefType(typedef_name_t,list<typ>,Opt_t<typ>);
    HeapRgnType; // has RgnKind
    RgnHandleType(typ);//singleton for deep alloc, has BoxUKind, typ has RgnKind
    UnionType;
  };

  EXTERN_DEFINITION enum Funcparams {
    NoTypes(list<var>,segment);
    WithTypes(list<$(Opt_t<var>,tqual,typ)@>,bool);
  };

  EXTERN_DEFINITION enum Pointer_Sort {
    NonNullable_ps(exp), // exp is upper bound
    Nullable_ps(exp),    // exp is upper bound
    TaggedArray_ps
  };

  EXTERN_DEFINITION enum Type_modifier {
    Carray_mod; 
    ConstArray_mod(exp);
    // for Pointer_mod, the typ has RgnKind, default is RgnType(HeapRgn)
    Pointer_mod(enum Pointer_Sort,typ,tqual); 
    Function_mod(funcparams_t);
    TypeParams_mod(list<tvar>,segment);
  };

  EXTERN_DEFINITION enum Cnst {
    Char_c(sign,char);
    Short_c(sign,short);
    Int_c(sign,int);
    LongLong_c(sign,long long);
    Float_c(string);
    String_c(bool,string); // bool is true when heap allocate
    Null_c;
  };

  EXTERN_DEFINITION enum Primop {
    Plus, Times, Minus, Div, Mod, Eq, Neq, Gt, Lt, Gte, Lte, Not,
    Bitnot, Bitand, Bitor, Bitxor, Bitlshift, Bitlrshift, Bitarshift,
    Size, Printf, Fprintf, Xprintf, Scanf, Fscanf, Sscanf
  };

  EXTERN_DEFINITION enum Incrementor { PreInc, PostInc, PreDec, PostDec };

  EXTERN_DEFINITION enum Raw_exp {
    Const_e(cnst_t);
    Var_e(qvar,binding_t); 
    UnknownId_e(qvar);
    Primop_e(primop,list<exp>);
    AssignOp_e(exp,Opt_t<primop>,exp);
    Increment_e(exp,incrementor_t);
    Conditional_e(exp,exp,exp);
    SeqExp_e(exp,exp);
    UnknownCall_e(exp,list<exp>);
    FnCall_e(exp,list<exp>);
    Throw_e(exp);
    NoInstantiate_e(exp);
    Instantiate_e(exp,Opt_t<list<typ>>);
    Cast_e(typ,exp);
    Address_e(exp);
    Sizeof_e(typ);
    Deref_e(exp);
    StructMember_e(exp,field_name);
    StructArrow_e(exp,field_name);
    Subscript_e(exp,exp);
    Tuple_e(list<exp>);
    CompoundLit_e($(Opt_t<var>,tqual,typ)@,list<$(list<designator>,exp)@>);
    Array_e(bool,list<$(list<designator>,exp)@>);//true == came with "new"
    Comprehension_e(vardecl,exp,exp); // much of vardecl is known
    Struct_e(typedef_name_t,Opt_t<list<typ>>,list<$(list<designator>,exp)@>,
	     struct Structdecl *);
    Enum_e(Opt_t<list<typ>>,Opt_t<list<typ>>,list<exp>,enumdecl,enumfield);
    Xenum_e(Opt_t<list<typ>>,list<exp>,xenumdecl,enumfield);
    UnresolvedMem_e(Opt_t<typedef_name_t>,list<$(list<designator>,exp)@>);
    StmtExp_e(stmt);
    Codegen_e(fndecl);
    Fill_e(exp);
  };

  EXTERN_DEFINITION struct Exp {
    Opt_t<typ> topt;
    raw_exp_t  r;
    segment    loc;
  };

  // The $(exp,stmt) in loops are just a hack for holding the
  // non-local predecessors of the exp.  The stmt should always be Skip_s
  // and only the non_local_preds field is interesting.
  EXTERN_DEFINITION enum Raw_stmt {
    Skip_s;
    Exp_s(exp);
    Seq_s(stmt,stmt);
    Return_s(Opt_t<exp>);
    IfThenElse_s(exp,stmt,stmt);
    While_s($(exp,stmt),stmt);
    Break_s(Opt_t<stmt>);    // stmt is dest, set by type-checking
    Continue_s(Opt_t<stmt>); // stmt is dest, set by type-checking
    Goto_s(var,Opt_t<stmt>); // stmt is dest, set by type-checking
    For_s(exp,$(exp,stmt),$(exp,stmt),stmt); 
    Switch_s(exp,list<switch_clause>); 
    Fallthru_s(list<exp>,Opt_t<stmt>); // stmt is dest, set by type-checking
    Decl_s(decl,stmt);
    Cut_s(stmt);
    Splice_s(stmt);
    Label_s(var,stmt); 
    Do_s(stmt,$(exp,stmt));
    TryCatch_s(stmt,list<switch_clause>);
  };

  EXTERN_DEFINITION struct Stmt {
    raw_stmt_t   r;
    segment      loc;
    list<stmt>   non_local_preds; // set by type-checking
    stmt_annot_t annot;
  };

  EXTERN_DEFINITION enum Raw_pat {
    Wild_p;
    Var_p(vardecl); // only name field is right until tcPat is called
    Null_p;
    Int_p(sign,int);
    Char_p(char);
    Float_p(string);
    Tuple_p(list<pat>);
    Pointer_p(pat);
    Reference_p(vardecl); // only name field is right until tcpat is called
    Struct_p(structdecl,Opt_t<list<typ>>,list<tvar>,
	     list<$(list<designator>,pat)@>);
    Enum_p(qvar,Opt_t<list<typ>>,list<tvar>,list<pat>,enumdecl,enumfield);
    Xenum_p(qvar,list<tvar>,list<pat>,xenumdecl,enumfield);
    UnknownId_p(qvar);
    UnknownCall_p(qvar,list<tvar>,list<pat>);
    UnknownFields_p(qvar,list<tvar>,list<$(list<designator>,pat)@>);
  };

  EXTERN_DEFINITION struct Pat {
    raw_pat_t   r;
    Opt_t<typ>  topt;
    segment     loc;
  };

  EXTERN_DEFINITION struct Switch_clause {
    pat               pattern;
    Opt_t<list<qvar>> pat_vars; // set by type-checker, used by translation to C
    Opt_t<exp>        where_clause;
    stmt              body;
    segment           loc;
  };

  // only local and pat cases need to worry about shadowing
  EXTERN_DEFINITION enum Binding {
    Unresolved_b;
    Global_b(vardecl);
    Funname_b(fndecl); // good distinction between functions and function ptrs
    Param_b($(var,tqual,typ)@);
    Local_b(vardecl);
    Pat_b(vardecl); // may use a specialized type later
  };

  EXTERN_DEFINITION struct Vardecl {
    scope      sc; 
    qvar       name;
    tqual      tq;
    typ        type;
    Opt_t<exp> initializer; // ignored for pattern variables
    int        shadow; // FIX: NOT USED PROPERLY RIGHT NOW!!!
    int        block;  
  };

  EXTERN_DEFINITION struct Fndecl {
    scope                   sc;
    bool                    is_inline;
    qvar                    name;
    list<tvar>              tvs;
    typ                     ret_type;
    list<$(var,tqual,typ)@> args;
    bool                    varargs;
    stmt                    body;
  };

  EXTERN_DEFINITION struct Structdecl {
    scope                                 sc;
    Opt_t<typedef_name_t>                 name;
    list<tvar>                            tvs;
    Opt_t<list<$(field_name,tqual,typ)@>> fields;
  };

  EXTERN_DEFINITION struct Enumfield {
    qvar                name;
    Opt_t<exp>          tag;
    list<tvar>          tvs;
    list<$(tqual,typ)@> typs;
    segment             loc;
  };

  EXTERN_DEFINITION struct Enumdecl {
    scope                  sc;
    Opt_t<typedef_name_t>  name;
    list<tvar>             tvs;
    Opt_t<list<enumfield>> fields;
  };

  EXTERN_DEFINITION struct Xenumdecl {
    scope           sc;
    typedef_name_t  name;
    list<enumfield> fields;
  };

  EXTERN_DEFINITION struct Typedefdecl {
    typedef_name_t name;
    list<tvar>     tvs;
    typ            defn;
  };

  EXTERN_DEFINITION enum Raw_decl {
    Var_d(vardecl);
    Fn_d(fndecl);
    Let_d(pat,Opt_t<typ>,exp,bool); // true => exhaustive
    Struct_d(structdecl);
    Union_d;
    Enum_d(enumdecl);
    Xenum_d(xenumdecl);
    Typedef_d(typedefdecl);
    Namespace_d(var,list<decl>);
    Using_d(qvar,list<decl>);
    ExternC_d(list<decl>);
  };

  EXTERN_DEFINITION struct Decl {
    raw_decl r;
    segment  loc;
  };

  EXTERN_DEFINITION enum Designator {
    ArrayElement(exp);
    FieldName(var);
  };

  EXTERN_DEFINITION xenum StmtAnnot { EmptyAnnot; };

  // compare variables 
  extern int qvar_cmp(qvar, qvar);
  extern int varlist_cmp(list<var>, list<var>);
  extern int tvar_cmp(tvar, tvar); // WARNING: ignores the kinds

  ///////////////////////// Constructors ////////////////////////////
  extern tqual combine_tqual(tqual x,tqual y);
  extern tqual empty_tqual();
  
  //////////////////////////// Constraints /////////////////////////
  extern conref<`a> new_conref<`a>(`a x); 
  extern conref<`a> empty_conref<`a>();

  ////////////////////////////// Types //////////////////////////////
  // return a fresh type variable of the given kind 
  extern typ new_evar(kind_t);
  extern typ wildtyp();
  // unsigned types
  extern typ uchar_t, ushort_t, uint_t, ulong_t;
  // signed types
  extern typ schar_t, sshort_t, sint_t, slong_t;
  // float, double
  extern typ float_t, double_t;
  // exception name and type
  extern $(list<var>,var) @ exn_name;
  extern xenumdecl exn_xed;
  extern typ exn_typ;
  // string (char[?])
  extern typ string_typ(typ rgn);
  // FILE
  extern typ file_typ();
  // pointers
  extern typ starb_typ(typ t, typ rgn, tqual tq, bounds_t b);
  extern typ atb_typ(typ t, typ rgn, tqual tq, bounds_t b);
  extern typ star_typ(typ t, typ rgn, tqual tq); // bounds = Upper(1)
  extern typ at_typ(typ t, typ rgn, tqual tq);   // bounds = Upper(1)
  extern typ cstar_typ(typ t, tqual tq); // *t -- always heap
  extern typ tagged_typ(typ t, typ rgn, tqual tq);
  extern typ void_star_typ();
  // structs
  extern typ strct(var  name);
  extern typ strctq(qvar name);

  /////////////////////////////// Expressions ////////////////////////
  extern exp new_exp(raw_exp_t, segment);
  extern exp copy_exp(exp);
  extern exp const_exp(cnst_t, segment);
  extern exp null_exp(segment);
  extern exp bool_exp(bool, segment);
  extern exp true_exp(segment);
  extern exp false_exp(segment);
  extern exp int_exp(sign,int,segment);
  extern exp signed_int_exp(int, segment);
  extern exp uint_exp(unsigned int, segment);
  extern exp char_exp(char c, segment);
  extern exp float_exp(string f, segment);
  extern exp string_exp(bool heap_allocate, string s, segment);
  extern exp var_exp(qvar, segment);
  extern exp varb_exp(qvar, binding_t, segment);
  extern exp unknownid_exp(qvar, segment);
  extern exp primop_exp(primop, list<exp> es, segment);
  extern exp prim1_exp(primop, exp, segment);
  extern exp prim2_exp(primop, exp, exp, segment);
  extern exp add_exp(exp, exp, segment);
  extern exp subtract_exp(exp, exp, segment);
  extern exp times_exp(exp, exp, segment);
  extern exp divide_exp(exp, exp, segment);
  extern exp eq_exp(exp, exp, segment);
  extern exp neq_exp(exp, exp, segment);
  extern exp gt_exp(exp, exp, segment);
  extern exp lt_exp(exp, exp, segment);
  extern exp gte_exp(exp, exp, segment);
  extern exp lte_exp(exp, exp, segment);
  extern exp assignop_exp(exp, Opt_t<primop>, exp, segment);
  extern exp assign_exp(exp, exp, segment);
  extern exp post_inc_exp(exp, segment);
  extern exp post_dec_exp(exp, segment);
  extern exp pre_inc_exp(exp, segment);
  extern exp pre_dec_exp(exp, segment);
  extern exp conditional_exp(exp, exp, exp, segment);
  extern exp and_exp(exp, exp, segment); // &&
  extern exp or_exp(exp, exp, segment);  // ||
  extern exp seq_exp(exp, exp, segment);
  extern exp unknowncall_exp(exp, list<exp>, segment);
  extern exp fncall_exp(exp, list<exp>, segment);
  extern exp throw_exp(exp, segment);
  extern exp noinstantiate_exp(exp, segment);
  extern exp instantiate_exp(exp, Opt_t<list<typ>>, segment);
  extern exp cast_exp(typ, exp, segment);
  extern exp address_exp(exp, segment);
  extern exp sizeof_exp(typ t, segment);
  extern exp deref_exp(exp, segment);
  extern exp structmember_exp(exp, field_name, segment);
  extern exp structarrow_exp(exp, field_name, segment);
  extern exp subscript_exp(exp, exp, segment);
  extern exp tuple_exp(list<exp>, segment);
  extern exp stmt_exp(stmt, segment);
  extern exp null_pointer_exn_exp(segment);
  extern exp array_exp(bool heap_allocate, list<exp>, segment);
  extern exp unresolvedmem_exp(Opt_t<typedef_name_t>,
			       list<$(list<designator>,exp)@>,segment);
  /////////////////////////// Statements ///////////////////////////////
  extern stmt new_stmt(raw_stmt_t s, segment loc);
  extern stmt skip_stmt(segment loc);
  extern stmt exp_stmt(exp e,segment loc);
  extern stmt seq_stmt(stmt s1, stmt s2, segment loc);
  extern stmt seq_stmts(list<stmt>, segment loc);
  extern stmt return_stmt(Opt_t<exp> e,segment loc);
  extern stmt ifthenelse_stmt(exp e,stmt s1,stmt s2,segment loc);
  extern stmt while_stmt(exp e,stmt s,segment loc);
  extern stmt break_stmt(segment loc);
  extern stmt continue_stmt(segment loc);
  extern stmt for_stmt(exp e1,exp e2,exp e3,stmt s, segment loc);
  extern stmt switch_stmt(exp e, list<switch_clause>, segment loc);
  extern stmt fallthru_stmt(list<exp> el, segment loc);
  extern stmt decl_stmt(decl d, stmt s, segment loc); 
  extern stmt declare_stmt(qvar, typ, Opt_t<exp> init, stmt, segment loc);
  extern stmt cut_stmt(stmt s, segment loc);
  extern stmt splice_stmt(stmt s, segment loc);
  extern stmt label_stmt(var v, stmt s, segment loc);
  extern stmt do_stmt(stmt s, exp e, segment loc);
  extern stmt trycatch_stmt(stmt s, list<switch_clause> scs, segment loc);
  extern stmt goto_stmt(var lab, segment loc);
  extern stmt assign_stmt(exp e1, exp e2, segment loc);

/////////////////////////// Patterns //////////////////////////////
  extern pat new_pat(raw_pat_t p, segment s);

  ////////////////////////// Declarations ///////////////////////////
  extern decl new_decl(raw_decl r, segment loc);
  extern decl let_decl(pat p, Opt_t<typ> t_opt, exp e, segment loc);
  extern vardecl new_vardecl(qvar x, typ t, Opt_t<exp> init);
  extern vardecl static_vardecl(qvar x, typ t, Opt_t<exp> init);
  extern decl struct_decl(scope s,Opt_t<typedef_name_t> n,list<tvar> ts,
			  Opt_t<list<$(field_name,tqual,typ)@>> fs,
			  segment loc);
  extern decl enum_decl(scope s,Opt_t<typedef_name_t> n,list<tvar> ts,
			Opt_t<list<enumfield>> fs,segment loc);
  extern decl xenum_decl(scope s,typedef_name_t n,list<enumfield> fs,
			 segment loc);

  // return true if p is printf, sprintf, fprintf, scanf, fscanf, sscanf
  extern bool is_format_prim(primop p);

  extern typ function_typ(list<tvar>,typ,list<$(Opt_t<var>,tqual,typ)@>,bool);
  extern typ pointer_expand(typ);
  // extern typ pointer_abbrev(typ);
  extern bool is_lvalue(exp);

  extern $(field_name,tqual,typ) * lookup_struct_field(structdecl,var);
  extern $(tqual,typ) * lookup_tuple_field(list<$(tqual,typ)@>,int);
}
#endif
