#ifndef _ABSYN_H_
#define _ABSYN_H_

// The abstract syntax has (at last count) three different "stages":
// A. Result of parsing
// B. Result of type-checking
// C. Result of translation to C
// Each stage obeys different invariants of which the compiler hacker
// (that's you) must be aware.
// Here are some invariants after parsing:
//  A1. None of the following are used:
//        Var_e, FnCall_e, Struct_e, Enum_e, Xenum_e
//      Instead, there will be
//        UnknownId_e, UnknownCall_e, UnresolvedMem_e
//  A2. qvars may be Rel_n, Abs_n, or Loc_n.
//      Rel_n is used for most variable references and declarations.
//      Abs_n is used for some globals (Null_Exception, etc).
//      Loc_n is used for some locals (comprehension and pattern variables).
// Here are some invariants after type checking:
//  B1. None of the following are used:
//        UnknownId_e, UnknownCall_e, UnresolvedMem_e
//      They have all been replaced by
//        Var_e, FnCall_e, Struct_e, Enum_e, Xenum_e
//      In Var_e, Unresolved_b is not used.
//  B2. All qvars are either Loc_n or Abs_n.  Any Rel_n has been
//      replaced by Loc_n or Abs_n.
//  B3. All "destination fields" and non_local_preds fields in stmt objects
//      are correct.
//  B4. Every expression has a non-null and correct type field.
//  B5. None of the "unresolved" variants are used.  (There may be 
//      underconstrained Evars for types.)
//  B6. The pat_vars field of switch_clause is non-null and correct.
// Note that translation to C willfully violates B3 and B4, but that's okay.


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
  extern enum Nmspace;
  typedef $(enum Nmspace,var) @qvar, *qvar_opt_t;
  typedef qvar typedef_name_t;
  typedef qvar_opt_t typedef_name_opt_t;
  
  // forward declarations
  extern struct Tvar;
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
  extern struct FnInfo;
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
  extern enum Attribute;
  extern struct Structfield;
  extern enum Mallocarg;

  typedef enum Scope scope;
  typedef struct Tqual @tqual;
  typedef enum Size_of size_of_t;
  typedef enum Kind kind_t;
  typedef struct Tvar @tvar; 
  typedef enum Sign sign_t;
  typedef struct Conref<`a> @conref<`a>;
  typedef enum Constraint<`a> constraint_t<`a>;
  typedef enum Bounds bounds_t;
  typedef struct PtrInfo ptr_info_t;
  typedef struct FnInfo fn_info_t;
  typedef enum Typ typ;
  typedef enum Funcparams funcparams_t;
  typedef enum Type_modifier type_modifier;
  typedef enum Cnst cnst_t;
  typedef enum Primop primop;
  typedef enum Incrementor incrementor_t;
  typedef enum Raw_exp raw_exp_t;
  typedef struct Exp @exp, *exp_opt;
  typedef enum Raw_stmt raw_stmt_t;
  typedef struct Stmt @stmt, *stmt_opt;
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
  typedef enum Attribute attribute_t;
  typedef list_t<attribute_t> attributes_t;
  typedef struct Structfield @structfield_t;
  typedef enum Mallocarg mallocarg_t;

  EXTERN_DEFINITION enum Nmspace {
    Loc_n,                  // Local name
    Rel_n(list_t<var>),     // Relative name
    Abs_n(list_t<var>)      // Absolute name
  };
  EXTERN_DEFINITION enum Scope { Static, Abstract, Public, Extern, ExternC };
  EXTERN_DEFINITION struct Tqual { 
    bool q_const; bool q_volatile; bool q_restrict; 
  };
  EXTERN_DEFINITION enum Size_of { B1, B2, B4, B8 };

  EXTERN_DEFINITION enum Kind { 
    // BoxKind <= MemKind <= AnyKind <= UnresolvedKind
    // EffKind <== UnresolvedKind, RgnKind <= UnresolvedKind
    AnyKind,      // kind of all types, including abstract structs
    MemKind,      // same as AnyType but excludes abstract structs
    BoxKind,      // same as MemKind but excludes types whose 
                  //   values do not go in a general purpose register
    RgnKind,      // regions
    EffKind       // effects
  };
  EXTERN_DEFINITION enum Sign { Signed, Unsigned };
  EXTERN_DEFINITION struct Conref<`a> { constraint_t<`a> v; };
  EXTERN_DEFINITION enum Constraint<`a> { 
    Eq_constr(`a), Forward_constr(conref<`a>), No_constr 
  };
  EXTERN_DEFINITION struct Tvar {
    tvarname_t name;
    conref<kind_t> kind;
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

  EXTERN_DEFINITION struct FnInfo {
    list_t<tvar>                     tvars;
    opt_t<typ>                       effect; // null => default effect
    typ                              ret_typ;
    list_t<$(opt_t<var>,tqual,typ)@> args;
    bool                             varargs;
    // function type attributes can include regparm(n), stdcall xor cdecl,
    // noreturn, and const.
    attributes_t                     attributes; 

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
    VoidType;                                              // MemKind
    Evar(kind_t,opt_t<typ>,int); 
    VarType(tvar);                            // kind induced by tvar
    EnumType(typedef_name_opt_t,list_t<typ>,enumdecl *);     // BoxKind
    XenumType(typedef_name_t,xenumdecl *);                 // BoxKind
    PointerType(ptr_info_t);            // BoxKind when not Unknown_b
    IntType(sign_t,size_of_t);                     // MemKind unless B4
    FloatType;                                             // MemKind
    DoubleType;                                            // MemKind
    ArrayType(typ/* element typ*/,tqual,exp /* size */);   // MemKind
    FnType(fn_info_t);                                     // MemKind
    TupleType(list_t<$(tqual,typ)@>); // MemKind
    StructType(typedef_name_opt_t,list_t<typ>,structdecl *); // MemKind
    UnionType;            // MemKind -- currently unsupported
    RgnHandleType(typ);   // BoxKind -- mem?  
    // An abbreviation -- the opt_t<typ> contains the definition if any
    TypedefType(typedef_name_t,list_t<typ>,opt_t<typ>);
    HeapRgn;              // RgnKind
    AccessEff(typ);       // RgnKind -> EffKind
    JoinEff(list_t<typ>); // EffKind list -> EffKind
  };

  EXTERN_DEFINITION enum Funcparams {
    NoTypes(list_t<var>,seg_t);
    // bool is true when varargs, opt_t<typ> is effect
    WithTypes(list_t<$(opt_t<var>,tqual,typ)@>,bool,opt_t<typ>); 
  };

  EXTERN_DEFINITION enum Pointer_Sort {
    NonNullable_ps(exp), // exp is upper bound
    Nullable_ps(exp),    // exp is upper bound
    TaggedArray_ps
  };

  // GCC attributes that we currently try to support:  this definitions
  // is only used for parsing and pretty-printing.  
  EXTERN_DEFINITION enum Attribute {
    Regparm_att(int); 
    Stdcall_att;      
    Cdecl_att;        
    Noreturn_att;     
    Const_att;
    Aligned_att(int);
    Packed_att;
    Section_att(string);
    Nocommon_att;
    Shared_att;
    Unused_att;
    Weak_att;
    Dllimport_att;
    Dllexport_att;
    No_instrument_function_att;
    Constructor_att;
    Destructor_att;
    No_check_memory_usage_att;
  };

  EXTERN_DEFINITION enum Type_modifier {
    Carray_mod; 
    ConstArray_mod(exp);
    // for Pointer_mod, the typ has RgnKind, default is RgnType(HeapRgn)
    Pointer_mod(enum Pointer_Sort,typ,tqual); 
    Function_mod(funcparams_t);
    TypeParams_mod(list_t<tvar>,seg_t,bool); // when bool is true, print kinds
    Attributes_mod(seg_t,attributes_t);
  };

  EXTERN_DEFINITION enum Cnst {
    Char_c(sign_t,char);
    Short_c(sign_t,short);
    Int_c(sign_t,int);
    LongLong_c(sign_t,long long);
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

  EXTERN_DEFINITION enum Mallocarg { 
    Typ_m(typ);
    Unresolved_m(qvar);
    // for remaining cases, what about existential type variables???
    // wait -- this doesn't work -- how will we assign the fields!
    // need a type of the variant and then allow assign of fully init to the
    // enum type!
    Enum_m(enumdecl,  enumfield); // instantiation irrelevant, I think
    Xenum_m(xenumdecl,enumfield); // instantiation irrelevant, I think
  };

  EXTERN_DEFINITION enum Raw_exp {
    Const_e(cnst_t);
    Var_e(qvar,binding_t); 
    UnknownId_e(qvar);
    Primop_e(primop,list_t<exp>);
    AssignOp_e(exp,opt_t<primop>,exp);
    Increment_e(exp,incrementor_t);
    Conditional_e(exp,exp,exp);
    SeqExp_e(exp,exp);
    UnknownCall_e(exp,list_t<exp>);
    FnCall_e(exp,list_t<exp>);
    Throw_e(exp);
    NoInstantiate_e(exp);
    Instantiate_e(exp,list_t<typ>);
    Cast_e(typ,exp);
    Address_e(exp);
    Sizeof_e(typ);
    Deref_e(exp);
    StructMember_e(exp,field_name);
    StructArrow_e(exp,field_name);
    Subscript_e(exp,exp);
    Tuple_e(list_t<exp>);
    CompoundLit_e($(opt_t<var>,tqual,typ)@,list_t<$(list_t<designator>,exp)@>);
    Array_e(bool,list_t<$(list_t<designator>,exp)@>);//true == came with "new"
    Comprehension_e(vardecl,exp,exp); // much of vardecl is known
    Struct_e(typedef_name_t,opt_t<list_t<typ>>,
	     list_t<$(list_t<designator>,exp)@>, struct Structdecl *);
    Enum_e(opt_t<list_t<typ>>,opt_t<list_t<typ>>,list_t<exp>,
	   enumdecl,enumfield);
    Xenum_e(opt_t<list_t<typ>>,list_t<exp>,xenumdecl,enumfield);
    Malloc_e(mallocarg_t);
    UnresolvedMem_e(opt_t<typedef_name_t>,list_t<$(list_t<designator>,exp)@>);
    StmtExp_e(stmt);
    Codegen_e(fndecl);
    Fill_e(exp);
  };

  EXTERN_DEFINITION struct Exp {
    opt_t<typ> topt;
    raw_exp_t  r;
    seg_t      loc;
  };

  // The $(exp,stmt) in loops are just a hack for holding the
  // non-local predecessors of the exp.  The stmt should always be Skip_s
  // and only the non_local_preds field is interesting.
  EXTERN_DEFINITION enum Raw_stmt {
    Skip_s;
    Exp_s(exp);
    Seq_s(stmt,stmt);
    Return_s(exp_opt);
    IfThenElse_s(exp,stmt,stmt);
    While_s($(exp,stmt),stmt);
    Break_s(stmt_opt);    // stmt is dest, set by type-checking
    Continue_s(stmt_opt); // stmt is dest, set by type-checking
    Goto_s(var,stmt_opt); // stmt is dest, set by type-checking
    For_s(exp,$(exp,stmt),$(exp,stmt),stmt); 
    Switch_s(exp,list_t<switch_clause>); 
    Fallthru_s(list_t<exp>,switch_clause *); // next case set by type-checking
    Decl_s(decl,stmt);
    Cut_s(stmt);
    Splice_s(stmt);
    Label_s(var,stmt); 
    Do_s(stmt,$(exp,stmt));
    TryCatch_s(stmt,list_t<switch_clause>);
  };

  EXTERN_DEFINITION struct Stmt {
    raw_stmt_t   r;
    seg_t        loc;
    list_t<stmt> non_local_preds; // set by type-checking, should go in the
                                  // appropriate CFStmtAnnot, not here!
    int          try_depth;
    stmt_annot_t annot;
  };

  EXTERN_DEFINITION enum Raw_pat {
    Wild_p;
    Var_p(vardecl); // only name field is right until tcPat is called
    Null_p;
    Int_p(sign_t,int);
    Char_p(char);
    Float_p(string);
    Tuple_p(list_t<pat>);
    Pointer_p(pat);
    Reference_p(vardecl); // only name field is right until tcpat is called
    Struct_p(structdecl,opt_t<list_t<typ>>,list_t<tvar>,
	     list_t<$(list_t<designator>,pat)@>);
    Enum_p(qvar,opt_t<list_t<typ>>,list_t<tvar>,list_t<pat>,enumdecl,enumfield);
    Xenum_p(qvar,list_t<tvar>,list_t<pat>,xenumdecl,enumfield);
    UnknownId_p(qvar);
    UnknownCall_p(qvar,list_t<tvar>,list_t<pat>);
    UnknownFields_p(qvar,list_t<tvar>,list_t<$(list_t<designator>,pat)@>);
  };

  EXTERN_DEFINITION struct Pat {
    raw_pat_t   r;
    opt_t<typ>  topt;
    seg_t       loc;
  };

  EXTERN_DEFINITION struct Switch_clause {
    pat               pattern;
    opt_t<list_t<vardecl>> pat_vars; // set by type-checker, used downstream
    exp_opt           where_clause;
    stmt              body;
    seg_t             loc;
  };

  // only local and pat cases need to worry about shadowing
  EXTERN_DEFINITION enum Binding {
    Unresolved_b; 
    Global_b(vardecl);
    Funname_b(fndecl); // good distinction between functions and function ptrs
    Param_b(vardecl);
    Local_b(vardecl);
    Pat_b(vardecl); // may use a specialized type later
  };

  // re-factor this so different kinds of vardecls only have what they
  // need.  Makes this a struct with an enum componenent (Global, Pattern,
  // Param, Local)
  EXTERN_DEFINITION struct Vardecl {
    scope              sc; 
    qvar               name;
    tqual              tq;
    typ                type;
    exp_opt            initializer; // ignored for non-local variables
    int                shadow;      // FIX: NOT USED PROPERLY RIGHT NOW!!!
    opt_t<typ>         region;      // filled in by type-checker
    // attributes can include just about anything...but the type-checker
    // must ensure that they are properly accounted for.  For instance,
    // functions cannot be aligned or packed.  And non-functions cannot
    // have regpar(n), stdcall, cdecl, noreturn, or const attributes.
    attributes_t       attributes; 
  };

  EXTERN_DEFINITION struct Fndecl {
    scope                   sc;
    bool                    is_inline;
    qvar                    name;
    list_t<tvar>            tvs; 
    opt_t<typ>              effect;     // null => default effect
    typ                     ret_type;
    list_t<$(var,tqual,typ)@> args;
    bool                    varargs;
    stmt                    body;
    opt_t<typ>              cached_typ; // cached type of the function
    opt_t<list_t<vardecl>>  param_vardecls; // so we can use pointer equality
    // any attributes except aligned or packed
    attributes_t            attributes; 
  };

  EXTERN_DEFINITION struct Structfield {
    field_name   name;    
    tqual        tq;
    typ          type;
    attributes_t attributes; // only valid ones are aligned(i) or packed
  };

  // for structs and enums, we should really memoize the string to 
  // field-number mapping!
  EXTERN_DEFINITION struct Structdecl {
    scope                        sc;
    opt_t<typedef_name_t>        name;
    list_t<tvar>                 tvs;
    opt_t<list_t<structfield_t>> fields;
    attributes_t                 attributes;
  };

  EXTERN_DEFINITION struct Enumfield {
    qvar                  name;
    exp_opt               tag;
    list_t<tvar>          tvs;
    list_t<$(tqual,typ)@> typs;
    seg_t                 loc;
  };

  EXTERN_DEFINITION struct Enumdecl {
    scope                    sc;
    opt_t<typedef_name_t>    name;
    list_t<tvar>             tvs;
    opt_t<list_t<enumfield>> fields;
  };

  EXTERN_DEFINITION struct Xenumdecl {
    scope           sc;
    typedef_name_t  name;
    list_t<enumfield> fields;
  };

  EXTERN_DEFINITION struct Typedefdecl {
    typedef_name_t name;
    list_t<tvar>   tvs;
    typ            defn;
  };

  EXTERN_DEFINITION enum Raw_decl {
    Var_d(vardecl);
    Fn_d(fndecl);
    Let_d(pat,
          opt_t<list_t<vardecl>>, // set by type-checker, used downstream
          opt_t<typ>,
          exp,
          bool); // true => exhaustive
    Struct_d(structdecl);
    Union_d;
    Enum_d(enumdecl);
    Xenum_d(xenumdecl);
    Typedef_d(typedefdecl);
    Namespace_d(var,list_t<decl>);
    Using_d(qvar,list_t<decl>);
    ExternC_d(list_t<decl>);
  };

  EXTERN_DEFINITION struct Decl {
    raw_decl r;
    seg_t    loc;
  };

  EXTERN_DEFINITION enum Designator {
    ArrayElement(exp);
    FieldName(var);
  };

  EXTERN_DEFINITION xenum StmtAnnot { EmptyAnnot; };

  // compare variables 
  extern int qvar_cmp(qvar, qvar);
  extern int varlist_cmp(list_t<var>, list_t<var>);
  extern int tvar_cmp(tvar, tvar); // WARNING: ignores the kinds

  ///////////////////////// Qualifiers ////////////////////////////
  extern tqual combine_tqual(tqual x,tqual y);
  extern tqual empty_tqual();
  
  //////////////////////////// Constraints /////////////////////////
  extern conref<`a> new_conref(`a x); 
  extern conref<`a> empty_conref();
  extern conref<`a> compress_conref(conref<`a> x);
  extern `a conref_val(conref<`a> x);

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
  extern qvar exn_name;
  extern xenumdecl exn_xed;
  extern typ exn_typ;
  // string (char ?)
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
  extern exp new_exp(raw_exp_t, seg_t);
  extern exp copy_exp(exp);
  extern exp const_exp(cnst_t, seg_t);
  extern exp null_exp(seg_t);
  extern exp bool_exp(bool, seg_t);
  extern exp true_exp(seg_t);
  extern exp false_exp(seg_t);
  extern exp int_exp(sign_t,int,seg_t);
  extern exp signed_int_exp(int, seg_t);
  extern exp uint_exp(unsigned int, seg_t);
  extern exp char_exp(char c, seg_t);
  extern exp float_exp(string f, seg_t);
  extern exp string_exp(bool heap_allocate, string s, seg_t);
  extern exp var_exp(qvar, seg_t);
  extern exp varb_exp(qvar, binding_t, seg_t);
  extern exp unknownid_exp(qvar, seg_t);
  extern exp primop_exp(primop, list_t<exp> es, seg_t);
  extern exp prim1_exp(primop, exp, seg_t);
  extern exp prim2_exp(primop, exp, exp, seg_t);
  extern exp add_exp(exp, exp, seg_t);
  extern exp subtract_exp(exp, exp, seg_t);
  extern exp times_exp(exp, exp, seg_t);
  extern exp divide_exp(exp, exp, seg_t);
  extern exp eq_exp(exp, exp, seg_t);
  extern exp neq_exp(exp, exp, seg_t);
  extern exp gt_exp(exp, exp, seg_t);
  extern exp lt_exp(exp, exp, seg_t);
  extern exp gte_exp(exp, exp, seg_t);
  extern exp lte_exp(exp, exp, seg_t);
  extern exp assignop_exp(exp, opt_t<primop>, exp, seg_t);
  extern exp assign_exp(exp, exp, seg_t);
  extern exp post_inc_exp(exp, seg_t);
  extern exp post_dec_exp(exp, seg_t);
  extern exp pre_inc_exp(exp, seg_t);
  extern exp pre_dec_exp(exp, seg_t);
  extern exp conditional_exp(exp, exp, exp, seg_t);
  extern exp and_exp(exp, exp, seg_t); // &&
  extern exp or_exp(exp, exp, seg_t);  // ||
  extern exp seq_exp(exp, exp, seg_t);
  extern exp unknowncall_exp(exp, list_t<exp>, seg_t);
  extern exp fncall_exp(exp, list_t<exp>, seg_t);
  extern exp throw_exp(exp, seg_t);
  extern exp noinstantiate_exp(exp, seg_t);
  extern exp instantiate_exp(exp, list_t<typ>, seg_t);
  extern exp cast_exp(typ, exp, seg_t);
  extern exp address_exp(exp, seg_t);
  extern exp sizeof_exp(typ t, seg_t);
  extern exp deref_exp(exp, seg_t);
  extern exp structmember_exp(exp, field_name, seg_t);
  extern exp structarrow_exp(exp, field_name, seg_t);
  extern exp subscript_exp(exp, exp, seg_t);
  extern exp tuple_exp(list_t<exp>, seg_t);
  extern exp stmt_exp(stmt, seg_t);
  extern exp null_pointer_exn_exp(seg_t);
  extern exp array_exp(bool heap_allocate, list_t<exp>, seg_t);
  extern exp unresolvedmem_exp(opt_t<typedef_name_t>,
			       list_t<$(list_t<designator>,exp)@>,seg_t);
  /////////////////////////// Statements ///////////////////////////////
  extern stmt new_stmt(raw_stmt_t s, seg_t loc);
  extern stmt skip_stmt(seg_t loc);
  extern stmt exp_stmt(exp e, seg_t loc);
  extern stmt seq_stmt(stmt s1, stmt s2, seg_t loc);
  extern stmt seq_stmts(list_t<stmt>, seg_t loc);
  extern stmt return_stmt(exp_opt e,seg_t loc);
  extern stmt ifthenelse_stmt(exp e,stmt s1,stmt s2,seg_t loc);
  extern stmt while_stmt(exp e,stmt s,seg_t loc);
  extern stmt break_stmt(seg_t loc);
  extern stmt continue_stmt(seg_t loc);
  extern stmt for_stmt(exp e1,exp e2,exp e3,stmt s, seg_t loc);
  extern stmt switch_stmt(exp e, list_t<switch_clause>, seg_t loc);
  extern stmt fallthru_stmt(list_t<exp> el, seg_t loc);
  extern stmt decl_stmt(decl d, stmt s, seg_t loc); 
  extern stmt declare_stmt(qvar, typ, exp_opt init, stmt, seg_t loc);
  extern stmt cut_stmt(stmt s, seg_t loc);
  extern stmt splice_stmt(stmt s, seg_t loc);
  extern stmt label_stmt(var v, stmt s, seg_t loc);
  extern stmt do_stmt(stmt s, exp e, seg_t loc);
  extern stmt trycatch_stmt(stmt s, list_t<switch_clause> scs, seg_t loc);
  extern stmt goto_stmt(var lab, seg_t loc);
  extern stmt assign_stmt(exp e1, exp e2, seg_t loc);

/////////////////////////// Patterns //////////////////////////////
  extern pat new_pat(raw_pat_t p, seg_t s);

  ////////////////////////// Declarations ///////////////////////////
  extern decl new_decl(raw_decl r, seg_t loc);
  extern decl let_decl(pat p, opt_t<typ> t_opt, exp e, seg_t loc);
  extern vardecl new_vardecl(qvar x, typ t, exp_opt init);
  extern vardecl static_vardecl(qvar x, typ t, exp_opt init);
  extern decl struct_decl(scope s,opt_t<typedef_name_t> n,list_t<tvar> ts,
			  opt_t<list_t<structfield_t>> fs, attributes_t atts,
			  seg_t loc);
  extern decl enum_decl(scope s,opt_t<typedef_name_t> n,list_t<tvar> ts,
			opt_t<list_t<enumfield>> fs,seg_t loc);
  extern decl xenum_decl(scope s,typedef_name_t n,list_t<enumfield> fs,
			 seg_t loc);

  // return true if p is printf, sprintf, fprintf, scanf, fscanf, sscanf
  extern bool is_format_prim(primop p);

  extern typ function_typ(list_t<tvar> tvs,opt_t<typ> eff_typ,typ ret_typ,
                          list_t<$(opt_t<var>,tqual,typ)@> args,bool varargs,
                          attributes_t);
  extern typ pointer_expand(typ);
  // extern typ pointer_abbrev(typ);
  extern bool is_lvalue(exp);

  extern struct Structfield * lookup_struct_field(structdecl,var);
  extern $(tqual,typ) * lookup_tuple_field(list_t<$(tqual,typ)@>,int);
  extern string attribute2string(attribute_t);
}
#endif
