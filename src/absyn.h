/* Abstract syntax.
   Copyright (C) 2001 Greg Morrisett, AT&T
   This file is part of the Cyclone compiler.

   The Cyclone compiler is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The Cyclone compiler is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the Cyclone compiler; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */
#ifndef _ABSYN_H_
#define _ABSYN_H_

// These macros are useful for declaring globally shared data constructors.
#define extern_datacon(T,C) extern datatype T.C C##_val
#define datacon(T,C) datatype T.C C##_val = C

// This file defines the abstract syntax used by the Cyclone compiler
// and related tools.  It is the crucial set of data structures that
// pervade the compiler.

// The abstract syntax is updated imperatively through many stages.
// * See cyclone.cyc for the order of the stages.
// * The invariants about abstract syntax change from stage to stage --
//   see calls to Warn::impos* in subsequent stages.

// A cute hack to avoid defining the abstract syntax twice:
#ifdef ABSYN_CYC
#define EXTERN_ABSYN
#else
#define EXTERN_ABSYN extern
#endif

#include <list.h>
#include <position.h>

namespace Relations {
  struct Reln; // see relations-ap.h
  typedef struct Reln@`r reln_t<`r>;
  typedef List::list_t<reln_t<`r>,`r> relns_t<`r>;
}
namespace AssnDef { // see assndef.h
  extern struct ExistAssnFn;
  typedef struct ExistAssnFn *existassnfn_opt_t;
}
namespace Tcpat {
  extern datatype Decision;  // see tcpat.h
  typedef datatype Decision *decision_opt_t;
}

namespace Absyn {
  using Core;
  using List;

  typedef Position::seg_t seg_t;

  typedef stringptr_t field_name_t; // field names (for structs, etc.)
  typedef stringptr_t var_t;        // variables are string pointers
  typedef stringptr_t tvarname_t;   // type variables
  typedef string_t   *var_opt_t;    // variable options

  // Name spaces
  EXTERN_ABSYN @tagged union Nmspace {
    list_t<var_t> Rel_n; // Relative name
    list_t<var_t> Abs_n; // Absolute name in Cyclone namespace
    list_t<var_t> C_n;   // Absolute name in C namespace
    int Loc_n;  // Local name, int is not used
  };
  typedef union Nmspace nmspace_t;

  // Qualified variables
  typedef $(nmspace_t,var_t) @qvar_t, *qvar_opt_t;
  typedef qvar_t typedef_name_t;
  typedef qvar_opt_t typedef_name_opt_t;

  // typedefs -- in general, we define foo_t to be struct Foo@ or datatype Foo@
  typedef enum Scope scope_t;
  typedef struct Tqual tqual_t; // not a pointer
  typedef enum Size_of size_of_t;
  typedef struct Kind @kind_t;
  typedef datatype KindBound @kindbound_t;
  typedef struct Tvar @tvar_t;
  typedef enum Sign sign_t;
  typedef enum AggrKind aggr_kind_t;
  typedef struct PtrAtts ptr_atts_t;
  typedef struct PtrInfo ptr_info_t;
  typedef struct VarargInfo vararg_info_t;
  typedef struct FnInfo fn_info_t;
  typedef union DatatypeInfo datatype_info_t;
  typedef union DatatypeFieldInfo datatype_field_info_t;
  typedef union AggrInfo aggr_info_t;
  typedef struct ArrayInfo array_info_t;
  typedef datatype Type @type_t, @rgntype_t, @booltype_t, @ptrbound_t, @aqualtype_t, @efftype_t;
  typedef datatype Type *type_opt_t;
  typedef list_t<type_t,`H> types_t;
  typedef list_t<$(type_t,type_t)@> aqualbnds_t;
  typedef union Cnst cnst_t;
  typedef enum Primop primop_t;
  typedef enum Incrementor incrementor_t;
  typedef struct VarargCallInfo vararg_call_info_t;
  typedef datatype Raw_exp @raw_exp_t;
  typedef struct Exp @exp_t, *exp_opt_t;
  typedef datatype Raw_stmt @raw_stmt_t;
  typedef struct Stmt @stmt_t, *stmt_opt_t;
  typedef datatype Raw_pat @raw_pat_t;
  typedef struct Pat @pat_t;
  typedef datatype Binding @binding_t;
  typedef struct Switch_clause @switch_clause_t;
  typedef struct Fndecl @fndecl_t;
  typedef struct Aggrdecl @aggrdecl_t;
  typedef struct Datatypefield @datatypefield_t;
  typedef struct Datatypedecl @datatypedecl_t;
  typedef struct Typedefdecl @typedefdecl_t;
  typedef struct Enumfield @enumfield_t;
  typedef struct Enumdecl @enumdecl_t;
  typedef struct Vardecl @vardecl_t, *vardecl_opt_t;
  typedef datatype Raw_decl @raw_decl_t;
  typedef struct Decl @decl_t;
  typedef datatype Designator @designator_t;
  typedef @extensible datatype AbsynAnnot @absyn_annot_t;
  typedef datatype Attribute @attribute_t;
  typedef list_t<attribute_t> attributes_t;
  typedef struct Aggrfield @aggrfield_t;
  typedef datatype OffsetofField @offsetof_field_t;
  typedef enum MallocKind malloc_kind_t;
  typedef struct MallocInfo malloc_info_t;
  typedef enum Coercion coercion_t;
  typedef struct PtrLoc *ptrloc_t;
  typedef datatype EffConstraint @effconstr_t, *effconstr_opt_t;

  // scopes for declarations
  EXTERN_ABSYN enum Scope {
    Static,    // definition is local to the compilation unit
    Abstract,  // definition is exported only abstractly (structs only)
    Public,    // definition is exported in full glory (most things)
    Extern,    // definition is provided elsewhere
    ExternC,   // definition is provided elsewhere by C code
    Register   // value may be stored in a register
  };

  // type qualifiers -- const, volatile, and restrict
  //   the print fields are printed but the real fields control the
  //   attributes with respect to type-checking.  After type-well-formedness,
  //   it should be that print_const implies real_const
  EXTERN_ABSYN struct Tqual {
    bool print_const :1;
    bool q_volatile  :1;
    bool q_restrict  :1;
    bool real_const  :1;
    seg_t loc; // only present when porting C code
  };

  EXTERN_ABSYN enum Size_of { Char_sz, Short_sz, Int_sz, Long_sz, LongLong_sz };
  EXTERN_ABSYN enum Sign     { Signed, Unsigned, None }; // for integral types
  EXTERN_ABSYN enum AggrKind { StructA, UnionA };

  EXTERN_ABSYN enum AliasQualVal {
    Aliasable_qual,  // for types that can be aliased
    Unique_qual,     // for types that cannot be aliased
    Refcnt_qual,     // for reference counted types
    Restricted_qual  // any of the above
  };
  typedef enum AliasQualVal alias_qual_val_t;

  //   // Used to classify kinds: Aliasable <= Top, Unique <= Top
  //   EXTERN_ABSYN enum AliasQual {
  //     Aliasable, // for types that can be aliased
  //     Unique,    // for types that cannot be aliased
  //     Top        // any of the above
  //   };

  // Used to support sugar `a::B\T --> `a : RESTRICTED >= aquals(`a)
  EXTERN_ABSYN enum AliasHint {
    UniqueHint,    // for types that cannot be aliased
    RefcntHint,    // for types that cannot be aliased
    RestrictedHint,        // any of the above
    NoHint //default -- i.e aliasable
  };
  EXTERN_ABSYN enum KindQual {
    // BoxKind <= MemKind <= AnyKind
    AnyKind, // kind of all types, including abstract structs
    MemKind, // excludes abstract structs
    BoxKind, // excludes types whose values dont go in general-purpose registers
    //    RgnKind, // regions
    EffKind, // effects
    IntKind, // ints at the specification level
    BoolKind, // booleans at the specification level: true or false
    PtrBndKind,   // fat or thin(e) used for pointers
    AqualKind //for alias qualifier type variables
  };
  EXTERN_ABSYN struct Kind {
    enum KindQual  kind;
    enum AliasHint aliashint;
  };

  // kind bounds are used on tvar's to infer their kinds
  //   Eq_kb(k):  the tvar has kind k
  //   Unknown_kb(&Opt(kb)): follow the link to kb to determine bound
  //   Less_kb(&Opt(kb),k): same, but link should be less than k
  //   Unknown_kb(NULL):  the tvar's kind is completely unconstrained
  //   Less_kb(NULL,k): the tvar's kind is unknown but less than k
  EXTERN_ABSYN datatype KindBound {
    Eq_kb(kind_t);
    Unknown_kb(opt_t<kindbound_t>);
    Less_kb(opt_t<kindbound_t>, kind_t);
  };

  // type variables
  EXTERN_ABSYN struct Tvar {
    tvarname_t  name;       // the user-level name of the type variable
    int         identity;   // for alpha-conversion -- unique -- -1 is error
    kindbound_t kind;
    type_opt_t aquals_bound; //used only by the parser, tctyp strips this out and adds to qual_bnd
  };

  EXTERN_ABSYN struct PtrLoc {
    seg_t  ptr_loc;
    seg_t  rgn_loc;
    seg_t  zt_loc;
  };

  EXTERN_ABSYN struct PtrAtts {
    efftype_t eff;        // the set of regions into which the pointer may point
    booltype_t nullable;  // type admits NULL
    ptrbound_t bounds;    // legal bounds for pointer indexing
    booltype_t zero_term; // true => zero terminated array
    ptrloc_t   ptrloc;    // location info -- only present when porting C code
    booltype_t autoreleased;  // true => an autoreleased, reference-counted pointer
    aqualtype_t aqual;
  };

  // information about a pointer type
  EXTERN_ABSYN struct PtrInfo {
    type_t     elt_type;  // type of value to which pointer points
    tqual_t    elt_tq;   // qualifier **for elements**
    ptr_atts_t ptr_atts;
  };

  // information about vararg functions
  EXTERN_ABSYN struct VarargInfo {
    var_opt_t name;
    tqual_t   tq;
    type_t    type;
    bool      inject;
  };

  // information for functions
  EXTERN_ABSYN struct FnInfo {
    list_t<tvar_t>  tvars;   // abstracted type variables
    // effect describes those regions that must be live to call the fn
    type_opt_t      effect;  // null => default effect
    tqual_t         ret_tqual;   // return type qualifier
    type_t          ret_type; // return type
    // arguments are optionally named -- note that after checking well-
    // formedness of the type, these names are no longer used.  Rather,
    // the arg_vardecls are used e.g., in the requires, ensures, and
    // throws assertions.
    list_t<$(var_opt_t,tqual_t,type_t)@>  args;
    // if c_varargs is true, then cyc_varargs == null, and if
    // cyc_varargs is non-null, then c_varargs is false.
    bool                                     c_varargs;
    vararg_info_t*                           cyc_varargs;
    // partial order on region parameters
    //    list_t<$(type_t,type_t)@>                rgn_po; //NKS remove this
    // bounds on qualifier variables -- first term is a tvar, second is the bound
    list_t<$(type_t,type_t)@>                qual_bnd;
    // function type attributes can include regparm(n), noreturn, const, nothrow
    // and at most one of cdecl, stdcall, and fastcall.  See gcc info
    // for documentation on attributes.
    attributes_t                             attributes;
    // pre-condition to call function as expression
    exp_opt_t                                requires_clause;
     // pre-condition after checking well-formedness
    AssnDef::existassnfn_opt_t                      requires_assn;
    // post-condition on return from function
    exp_opt_t                                ensures_clause;
    AssnDef::existassnfn_opt_t                      ensures_assn;
    // post-condition on an escaping throw from function
    exp_opt_t                                throws_clause;
    AssnDef::existassnfn_opt_t                      throws_assn;
    // the variable used in the @ensures clause for the return value
    vardecl_opt_t                            return_value;
    // the variable declarations used in the @requires and @ensures clauses --
    // we only have vardecls corresponding to those arguments that are actually
    // named.
    list_t<vardecl_opt_t>                    arg_vardecls;
    //NEW
    list_t<effconstr_t>  effconstr;
  };

  // information for datatypes
  EXTERN_ABSYN struct UnknownDatatypeInfo {
    qvar_t name;          // name of the datatype
    bool   is_extensible; // true -> @extensible
  };
  EXTERN_ABSYN @tagged union DatatypeInfo {
    struct UnknownDatatypeInfo UnknownDatatype; // don't know definition yet
    datatypedecl_t @KnownDatatype;              // known definition
  };
  datatype_info_t UnknownDatatype(struct UnknownDatatypeInfo);
  datatype_info_t KnownDatatype(datatypedecl_t@`H);

  // information for datatype Foo.Bar
  EXTERN_ABSYN struct UnknownDatatypeFieldInfo {
    qvar_t datatype_name;   // name of the datatype
    qvar_t field_name;      // name of the datatype field
    bool   is_extensible;   // true -> @extensible
  };
  EXTERN_ABSYN @tagged union DatatypeFieldInfo {
    struct UnknownDatatypeFieldInfo UnknownDatatypefield;
    $(datatypedecl_t, datatypefield_t) KnownDatatypefield;
  };
  datatype_field_info_t UnknownDatatypefield(struct UnknownDatatypeFieldInfo);
  datatype_field_info_t KnownDatatypefield(datatypedecl_t, datatypefield_t);

  EXTERN_ABSYN @tagged union AggrInfo {
    $(aggr_kind_t,typedef_name_t,opt_t<bool> tagged) UnknownAggr;
    aggrdecl_t@ KnownAggr;
  };
  aggr_info_t UnknownAggr(aggr_kind_t,typedef_name_t,opt_t<bool,`H>);
  aggr_info_t KnownAggr(aggrdecl_t@`H);

  EXTERN_ABSYN struct ArrayInfo {
    type_t     elt_type;       // element type
    tqual_t    tq;             // qualifiers
    exp_opt_t  num_elts;       // number of elements
    booltype_t zero_term;      // true => zero-terminated
    seg_t      zt_loc;         // location of zeroterm qualifier
  };

  // could allow datatype decls in here as well
  EXTERN_ABSYN datatype Raw_typedecl {
    Aggr_td(aggrdecl_t);
    Enum_td(enumdecl_t);
    Datatype_td(datatypedecl_t);
  };
  typedef datatype Raw_typedecl @raw_type_decl_t;
  EXTERN_ABSYN struct TypeDecl {
    raw_type_decl_t r;
    seg_t           loc;
  };
  typedef struct TypeDecl @type_decl_t;

  // Type Constructors that don't require any special treatment
  EXTERN_ABSYN datatype TyCon {
    VoidCon;// MemKind
    IntCon(sign_t,size_of_t); // char, short, int.  MemKind unless B4
    FloatCon(int);  // MemKind.  0=>float, 1=>double, _=>long double
    ComplexCon; // complex ranges over intcon and floatcons only.
    RgnHandleCon; // region_t<`r> (handle for allocating).  RgnKind -> BoxKind
    TagCon;     // tag_t<t>.  IntKind -> BoxKind.
    HeapCon;    // The heap region.  EffKind // used to be RgnKind
    UniqueHeapCon;  // Short hand for the heap region with @aqual(UNIQUE). RgnKind
    RefCntHeapCon;   // Short hand for the heap region with @aqual(REFCNT). RgnKind
    //    AccessCon;  // Uses region r.  RgnKind -> EffKind //don't need this anymore REMOVE
    JoinCon;  // e1+e2.  EffKind list -> EffKind
    RgnsCon;  // regions(t).  AnyKind -> EffKind
    TrueCon;  // BoolKind
    FalseCon; // BoolKind
    ThinCon;  // IntKind -> PtrBndKind
    FatCon;   // PtrBndKind
    AqualsCon;   // aquals(`a) : BoxKind -> AqualKind (aquals(`a::B))
    AqualConstCon(alias_qual_val_t); //AqualKind (RESTRICTED, UNIQUE etc.)
    AqualVarCon; // AqualKind x AqualKind -> AqualKind  (arg1: tvar, arg2: bound)
    AqualHandleCon; // AqualKind -> BoxKind  (aqual_t<`q>)
    EnumCon(typedef_name_t,struct Enumdecl *); // MemKind
    AnonEnumCon(list_t<enumfield_t>); // MemKind
    BuiltinCon(string_t,kind_t); // e.g., __builtin_va_list
    DatatypeCon(datatype_info_t); // e.g., datatype Foo
    DatatypeFieldCon(datatype_field_info_t); // e.g. datatype Foo.Bar
    AggrCon(aggr_info_t); // e.g., union U or struct S
  };
  typedef datatype TyCon @tycon_t;

  EXTERN_ABSYN datatype EffConstraint {
    SingleConstraint(type_t); //single(e1) used to ensure that a pointer *points into* a given region, e.g in rfree
    DisjointConstraint(type_t, type_t); //e1|e2 (useful for restrict / alias information)
    SubsetConstraint(type_t, type_t); //regions(`env)<`e, used in existential types t
  };

  // Note: The last fields of AggrType, TypedefType, and the decl
  // are set by check_valid_type which most of the compiler assumes
  // has been called.  Doing so avoids the need for some code to have and use
  // a type-name environment just like variable-binding fields avoid the need
  // for some code to have a variable environment.
  // FIX: Change a lot of the abstract-syntaxes options to nullable pointers.
  // FIX: May want to make this raw_type and store the kinds with the types.
  EXTERN_ABSYN datatype Type {
    // application of a type constructor to zero or more arguments
    AppType(tycon_t, types_t);
   // Evars are introduced for unification or via _ by the user.
    // The kind can get filled in during well-formedness checking.
    // The type can get filled in during unification.
    // The int is used as a unique identifier for printing messages.
    // The list of tvars is the set of free type variables that can
    // occur in the type to which the evar is constrained.
    Evar(opt_t<kind_t>,type_opt_t,int,opt_t<list_t<tvar_t>>);
    VarType(tvar_t); // type variables, kind induced by tvar
    PointerType(ptr_info_t); // t*, t?, t@, etc.  BoxKind when not Unknown_b
    ArrayType(array_info_t);// MemKind
    FnType(fn_info_t); // MemKind
    // We treat anonymous structs, unions, and enums slightly differently
    // than C.  In particular, we treat structurally equivalent types as
    // equal.  C requires a name for each type and uses by-name equivalence.
    //TupleType(list_t<$(tqual_t,type_t)@>); // MemKind
    AnonAggrType(aggr_kind_t,bool tuple,list_t<aggrfield_t>); // MemKind
    // An abbreviation -- the type_t* contains the definition iff any
    TypedefType(typedef_name_t,types_t,struct Typedefdecl *,type_opt_t);
    ValueofType(exp_t);      // IntKind -- exp must be a type-level expression
    // A struct, union, or enum declaration nested within a type -- we
    // enter the declaration into the environment and then only use the
    // type_t part from then on.
    TypeDeclType(type_decl_t,type_t*);
    // GCC extensions
    TypeofType(exp_t);
  };

  // Used in attributes below.
  EXTERN_ABSYN enum Format_Type { Printf_ft, Scanf_ft };

  // See GCC info pages for GCC attributes we try to support; see attributes.cyc
  EXTERN_ABSYN datatype Attribute {
    Regparm_att(int);
    Stdcall_att;
    Cdecl_att;
    Fastcall_att;
    Noreturn_att;
    Const_att;
    Aligned_att(exp_opt_t);
    Packed_att;
    Section_att(string_t);
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
    Format_att(enum Format_Type,
               int,   // format string arg
               int);  // first arg to type-check
    Initializes_att(int); // param that function initializes through
    Noliveunique_att(int); // param that has no unique pointers in it (implies consume)
    Consume_att(int); // param that will be consumed by the function
    Pure_att;
    Mode_att(string_t);
    Alias_att(string_t);
    Always_inline_att;
    No_throw_att;
  };

  // used when parsing/pretty-printing function definitions.
  EXTERN_ABSYN datatype Funcparams {
    NoTypes(list_t<var_t>,seg_t); // K&R style.
    WithTypes(list_t<$(var_opt_t,tqual_t,type_t)@>, // args and types
              bool,                                    // true ==> c_varargs
              vararg_info_t *,                         // cyc_varargs
              type_opt_t,                              // effect
	      // list_t<$(type_t,type_t)@>,               // region partial order
	      list_t<effconstr_t>,                      //effect constraints
              list_t<$(type_t,type_t)@>,               // qualifier bounds
              exp_opt_t,                               // requires clause
              exp_opt_t,                               // ensures clause
              exp_opt_t);                              // throws clause
  };

  // Type modifiers are used for parsing/pretty-printing
  EXTERN_ABSYN datatype Type_modifier<`r> {
    Carray_mod(booltype_t,seg_t); // [], booltype controls zero-term
    ConstArray_mod(exp_t,booltype_t,seg_t); // [c], booltype controls zero-term
    Pointer_mod(ptr_atts_t,tqual_t); // qualifer for the point (**not** elts)
    Function_mod(datatype Funcparams @`r);
    TypeParams_mod(list_t<tvar_t>,seg_t,bool);// when bool is true, print kinds
    Attributes_mod(seg_t,attributes_t);
  };
  typedef datatype Type_modifier<`r> @`r type_modifier_t<`r>;

  // Constants
  EXTERN_ABSYN @tagged union Cnst {
    int Null_c; // int unused
    $(sign_t,char) Char_c;
    string_t Wchar_c;
    $(sign_t,short) Short_c;
    $(sign_t,int) Int_c;
    $(sign_t,long long) LongLong_c;
    $(string_t,int) Float_c; // 0=>float, 1=>double, _=>long double
    string_t String_c;
    string_t Wstring_c;
  };

  // Primitive operations
  EXTERN_ABSYN enum Primop {
    Plus, Times, Minus, Div, Mod, Eq, Neq, Gt, Lt, Gte, Lte, Not,
    Bitnot, Bitand, Bitor, Bitxor, Bitlshift, Bitlrshift,
    Numelts, Tagof
  };

  // ++x, x++, --x, x-- respectively
  EXTERN_ABSYN enum Incrementor { PreInc, PostInc, PreDec, PostDec };

  // information about a call to a vararg function
  EXTERN_ABSYN struct VarargCallInfo {
    int                   num_varargs;
    list_t<datatypefield_t> injectors;
    vararg_info_t        @vai;
  };

  // information for offsetof
  EXTERN_ABSYN datatype OffsetofField {
    StructField(field_name_t);
    //TupleIndex(unsigned int);
  };

  // Casts are labelled with whether or not they involve a coercion
  // of some sort.  This is used in the flow analysis to get rid of
  // spurious warnings and null checks.
  EXTERN_ABSYN enum Coercion {
    Unknown_coercion, // initially, we don't know what kind of coercion
    No_coercion,      // a cast that C supports
    Null_to_NonNull,  // t*{n+m} -> t@{n}
      // FIX: should enumerate all other coercions so that we can
      // make sure the type-checker and code-generator are in sync.
    Other_coercion    // all the other coercions (see toc.cyc)
  };

  EXTERN_ABSYN datatype Designator {
    ArrayElement(exp_t);
    FieldName(var_t);
  };

  EXTERN_ABSYN enum MallocKind { Malloc, Calloc, Vmalloc };

  // information for malloc:
  //  it's important to note that when is_calloc is false (i.e., we have
  //  a malloc or rmalloc) then we can be in one of two states depending
  //  upon whether we've only parsed the expression or type-checked it.
  //  If we've parsed it, then elt_type will be NULL and num_elts will
  //  be the entire argument to malloc.  After type-checking, the malloc
  //  argument is sizeof(*elt_type)*num_elts.
  EXTERN_ABSYN struct MallocInfo {
    malloc_kind_t mknd;     // determines whether this is a malloc or calloc or vmalloc
    exp_opt_t  rgn;      // for rmalloc and rcalloc only
    exp_opt_t  aqual;    // aqual_t may be present for allocating unique or refcnt pointers
    type_t    *elt_type; // when [r]malloc, set by type-checker.  when
                         // [r]calloc, set by parser
    exp_t      num_elts; // for [r]malloc: is the sizeof(t)*n.
                         // for [r]calloc: is just n.
    bool       fat_result; // true when result is a elt_type? -- set by tc
    bool       inline_call; // when true, call _fast_region_malloc
  };

  // "raw" expressions -- don't include location info or type
  EXTERN_ABSYN datatype Raw_exp {
    Const_e(cnst_t); // constants
    Var_e(binding_t); // variables -- binding_t gets filled in
    Pragma_e(string_t); // pragmas for compiler debugging
    Primop_e(primop_t,list_t<exp_t>); // application of primitive
    AssignOp_e(exp_t,opt_t<primop_t>,exp_t); // e1 = e2, e1 += e2, etc.
    Increment_e(exp_t,incrementor_t);  // e++, ++e, --e, e--
    Conditional_e(exp_t,exp_t,exp_t);  // e1 ? e2 : e3
    And_e(exp_t,exp_t); // e1 && e2
    Or_e(exp_t,exp_t);  // e1 || e2
    SeqExp_e(exp_t,exp_t);             // e1, e2
    // the resolved field is initially false and indicates that this
    // could be a datatype constructor or aggregate constructor expression.
    // the vararg_call_info_t is non-null only if this is a vararg call
    // and is set during type-checking and used only for code generation.
    FnCall_e(exp_t,list_t<exp_t>,vararg_call_info_t *,bool resolved); //fn call
    Throw_e(exp_t,bool preserve_lineinfo); // throw.
    NoInstantiate_e(exp_t); // e@<>
    Instantiate_e(exp_t,types_t); // instantiation of polymorphic defn
    // (t)e.
    Cast_e(type_t,exp_t,bool,coercion_t);//true bool indicates user made cast
    Address_e(exp_t); // &e
    New_e(exp_opt_t, exp_t, exp_opt_t); // first expression is region -- null is heap, 3rd is aqual_t -- null is ALIASABLE
    Sizeoftype_e(type_t); // sizeof(t)
    Sizeofexp_e(exp_t); // sizeof(e)
    Offsetof_e(type_t,list_t<offsetof_field_t>); // offsetof(t,e)
    Deref_e(exp_t); // *e
    // For the next two cases, the is_read field determines whether or not
    // the expression is in a "read" (as opposed to write) position.
    // For tagged unions in a read position, we must check that
    // that the tag agrees with the member.  The is_read flag is set
    // by the type-checker.  The is_tagged field determines whether this
    // is a projection off of a tagged union and is also set by the type-
    // checker.
    AggrMember_e(exp_t,field_name_t,bool is_tagged, bool is_read);  // e.x
    AggrArrow_e(exp_t,field_name_t,bool is_tagged, bool is_read);   // e->x
    Subscript_e(exp_t,exp_t); // e1[e2]
    //Tuple_e(list_t<exp_t>);   // $(e1,...,en)
    CompoundLit_e($(var_opt_t,tqual_t,type_t)@,
                  list_t<$(list_t<designator_t>,exp_t)@>);
    Array_e(list_t<$(list_t<designator_t>,exp_t)@>); // {.0=e1,...,.n=en}
    // {for i < e1 : e2} -- the bool tells us whether it's zero-terminated
    Comprehension_e(vardecl_t,exp_t,exp_t,bool); // much of vardecl is known
    // {for i < e1 : t} -- used for variable-sized flat arrays
    ComprehensionNoinit_e(exp_t,type_t,bool);
    // Foo{.x1=e1,...,.xn=en} (with optional witness types)
    Aggregate_e(typedef_name_t,
                types_t,//witness types, maybe fewer before type-checking
                list_t<$(list_t<designator_t>,exp_t)@>,
                struct Aggrdecl *);
    // {.x1=e1,....,.xn=en}
    AnonStruct_e(type_opt_t, bool tuple, list_t<$(list_t<designator_t>,exp_t)@>);
    // Foo(e1,...,en)
    Datatype_e(list_t<exp_t>,datatypedecl_t,datatypefield_t);
    Enum_e(enumdecl_t,enumfield_t);
    AnonEnum_e(type_t,enumfield_t);
    // malloc(e1), rmalloc(e1,e2), calloc(e1,e2), rcalloc(e1,e2,e3).
    Malloc_e(malloc_info_t);
    Swap_e(exp_t,exp_t); // swap(e1,e2)
    // will resolve into array, struct, etc.
    UnresolvedMem_e(opt_t<typedef_name_t>,
                    list_t<$(list_t<designator_t>,exp_t)@>);
    StmtExp_e(stmt_t); // ({s;e})
    Tagcheck_e(exp_t, field_name_t);
    Valueof_e(type_t); // the type must have IntKind.  Valueof_e can only
    // be used within types, and is typically used within a valueof_t so
    // that we can move between the type and expression levels
    // (e.g., valueof_t<valueof(`i)*42 + 36>).
    //  Asm_e(bool volatile_kw,string_t); // uninterpreted asm statement -- used
    // within extern "C include" -- the string is all of the gunk
    // between the parens.
    //                    V instr  : outvars                    : invars                     :   clobbers V
    Asm_e(bool, string_t, list_t<$(string_t, exp_t)@>, list_t<$(string_t, exp_t)@>, list_t<string_t@>);
    Extension_e(exp_t);
    // implements GCC's __extension__ (...) though we just pass it through.
    Assert_e(exp_t); // a static assertion to be checked by the analysis
    Assert_false_e(exp_t); // a static assertion that shouldn't be proved by the analysis
  };
  // expression with auxiliary information
  EXTERN_ABSYN struct Exp {
    type_opt_t    topt;  // type of expression -- filled in by type-checker
    raw_exp_t     r;     // the real expression
    seg_t         loc;   // the location in the source code
    absyn_annot_t annot; // used during analysis and to pass info to removeAggrs
  };

  // The $(exp,stmt) in loops are just a hack for holding the
  // non-local predecessors of the exp.  The stmt should always be Skip_s
  // and only the non_local_preds field is interesting.
  EXTERN_ABSYN datatype Raw_stmt {
    Skip_s;   // ;
    Exp_s(exp_t);  // e
    Seq_s(stmt_t,stmt_t); // s1;s2
    Return_s(exp_opt_t); // return; and return e;
    IfThenElse_s(exp_t,stmt_t,stmt_t); // if (e) then s1 else s2;
    While_s($(exp_t,stmt_t),stmt_t); // while (e) s;
    Break_s;
    Continue_s;
    Goto_s(var_t);
    For_s(exp_t,$(exp_t,stmt_t),$(exp_t,stmt_t),stmt_t);
    // switch statement
    Switch_s(exp_t,list_t<switch_clause_t>, Tcpat::decision_opt_t);
    // next case set by type-checking
    Fallthru_s(list_t<exp_t>,switch_clause_t *);
    Decl_s(decl_t,stmt_t); // declarations
    Label_s(var_t,stmt_t); // L:s
    Do_s(stmt_t,$(exp_t,stmt_t));
    TryCatch_s(stmt_t,list_t<switch_clause_t>, Tcpat::decision_opt_t);
  };
  extern_datacon(Raw_stmt,Skip_s);

  // statements with auxiliary info
  EXTERN_ABSYN struct Stmt {
    raw_stmt_t    r;               // raw statement
    seg_t         loc;             // location in source code
    absyn_annot_t annot;           // used by analysis
  };

  // raw patterns
  EXTERN_ABSYN datatype Raw_pat {
    Wild_p; // _
    // for Var_p and Reference_p only name field is right until tcPat is called
    Var_p(vardecl_t,pat_t); // x as p (x when p is Wild_p)
    AliasVar_p(tvar_t, vardecl_t);  // alias <`r> T x
    Reference_p(vardecl_t,pat_t);// *x as p (*x when p is Wild_p)
    TagInt_p(tvar_t,vardecl_t);// i<`i> (unpack an int)
    //Tuple_p(list_t<pat_t>, bool dot_dot_dot); // $(p1,...,pn)
    Pointer_p(pat_t); // &p
    Aggr_p(type_opt_t,bool tuple,list_t<tvar_t>,
           list_t<$(list_t<designator_t>,pat_t)@>,
           bool dot_dot_dot);
    Datatype_p(datatypedecl_t, datatypefield_t, list_t<pat_t>, bool dot_dot_dot);
    Null_p; // NULL
    Int_p(sign_t,int); // 3
    Char_p(char);      // 'a'
    Float_p(string_t,int); // 3.1415; int 0=>float, 1=>double, _=>long double
    Enum_p(enumdecl_t,enumfield_t);
    AnonEnum_p(type_t,enumfield_t);
    UnknownId_p(qvar_t); // resolved by binding
    UnknownCall_p(qvar_t,list_t<pat_t>, bool dot_dot_dot); //resolved by binding
    Exp_p(exp_t);        // evaluated and resolved by tcpat
  };
  extern_datacon(Raw_pat,Wild_p);
  extern_datacon(Raw_pat,Null_p);

  // patterns with auxiliary information
  EXTERN_ABSYN struct Pat {
    raw_pat_t      r;    // raw pattern
    type_opt_t     topt; // type -- filled in by tcpat
    seg_t          loc;  // location in source code
  };

  // cyclone switch clauses
  EXTERN_ABSYN struct Switch_clause {
    pat_t                    pattern;  // pattern
    opt_t<list_t<$(vardecl_t *,exp_opt_t)@>> pat_vars; // set by type-checker, used downstream
    exp_opt_t                where_clause; // case p && e: -- the e part
    stmt_t                   body;     // corresponding statement
    seg_t                    loc;      // location in source code
  };

  // only local and pat cases need to worry about shadowing
  EXTERN_ABSYN datatype Binding {
    Unresolved_b(qvar_t);// don't know -- error or uninitialized
    Global_b(vardecl_t); // global variable
    Funname_b(fndecl_t); // distinction between functions and function ptrs
    Param_b(vardecl_t);  // local function parameter
    Local_b(vardecl_t);  // local variable
    Pat_b(vardecl_t);    // pattern variable
  };

  // Variable declarations.
  // re-factor this so different kinds of vardecls only have what they
  // need.  Makes this a struct with an datatype componenent (Global, Pattern,
  // Param, Local)
  EXTERN_ABSYN struct Vardecl {
    scope_t            sc;          // static, extern, etc.
    qvar_t             name;        // variable name
    seg_t              varloc;      // declaration beginning and ending location
    tqual_t            tq;          // const, volatile, etc.
    type_t             type;        // type of variable
    exp_opt_t          initializer; // optional initializer --
                                    // ignored for non-local/global variables
    type_opt_t         rgn;         // filled in by type-checker
    // attributes can include just about anything...but the type-checker
    // must ensure that they are properly accounted for.  For instance,
    // functions cannot be aligned or packed.  And non-functions cannot
    // have regpar(n), stdcall, cdecl, noreturn, or const attributes.
    attributes_t       attributes;
    bool               escapes;     // set by type-checker -- used for simple
    // flow analyses, means that &x is taken in some way so there can be
    // aliasing. (should go away since flow analysis already does
    // a less syntactic escapes analysis)
    bool               is_proto; //used for suppression of gcc warnings ...
    //by inserting extern kw appropriately
    exp_opt_t          rename; // optional asm expression,
    // used by gcc for renaming ("Controlling Names Used in Assembler Code")
  };

  // Function declarations.
  EXTERN_ABSYN struct Fndecl {
    scope_t    sc;         // static, extern, etc.
    bool       is_inline;  // inline flag
    qvar_t     name;       // function name
    stmt_t     body;       // body of function
    fn_info_t  i;
    // type differs from i in that it may have fewer attributes and it
    // may have a different const-ness on the return qualifier
    type_opt_t cached_type; // cached type of the function
    opt_t<list_t<vardecl_t>>   param_vardecls;// so we can use pointer equality
    struct Vardecl            *fn_vardecl; // used only for inner functions
    scope_t    orig_scope; //after toc, this field is used to flag extern C functions ... ignore bodies
  };

  EXTERN_ABSYN struct Aggrfield {
    field_name_t name;     // empty-string when a bitfield used for padding
    tqual_t      tq;
    type_t       type;
    exp_opt_t    width;   // bit fields: (unsigned) int and int fields only
    attributes_t attributes; // only valid ones are aligned(i) or packed
    exp_opt_t    requires_clause; // only for union fields
  };

  EXTERN_ABSYN struct AggrdeclImpl {
    list_t<tvar_t>            exist_vars;
    list_t<$(type_t,type_t)@> qual_bnd;
    list_t<aggrfield_t>       fields;
    bool                      tagged; // only applicable for unions
    //    NEW
    list_t<effconstr_t> effconstr;
  };

  //for structs and datatypes we could memoize the string->field-number mapping
  EXTERN_ABSYN struct Aggrdecl {
    aggr_kind_t           kind;
    scope_t               sc;  // abstract possible here
    typedef_name_t        name; // struct name
    list_t<tvar_t>        tvs;  // type parameters
    struct AggrdeclImpl * impl; // NULL when abstract
    attributes_t          attributes;
    // when expected_mem_kind is true, the aggregate must eventually be
    // defined and result in a &mk definition
    bool                  expected_mem_kind;
  };

  EXTERN_ABSYN struct Datatypefield {
    qvar_t                     name;
    list_t<$(tqual_t,type_t)@> typs;
    seg_t                      loc;
    scope_t                    sc; // relevant only for extensible datatypes
  };

  //for now, tvs cannot contain any qualifier variables, so no bounds info either
  EXTERN_ABSYN struct Datatypedecl {
    scope_t                      sc;
    typedef_name_t               name;
    list_t<tvar_t>               tvs;
    opt_t<list_t<datatypefield_t>> fields;
    bool                         is_extensible;
  };

  EXTERN_ABSYN struct Enumfield {
    qvar_t    name;
    exp_opt_t tag;
    seg_t     loc;
  };

  EXTERN_ABSYN struct Enumdecl {
    scope_t                    sc;
    typedef_name_t             name;
    opt_t<list_t<enumfield_t>> fields; // NULL when abstract
  };

  EXTERN_ABSYN struct Typedefdecl {
    typedef_name_t name;
    tqual_t        tq;
    list_t<tvar_t> tvs;
    opt_t<kind_t>  kind;
    type_opt_t     defn;
    attributes_t   atts;
    bool           extern_c; // DJG: not sure what true means
  };

  // raw declarations
  EXTERN_ABSYN datatype Raw_decl {
    Var_d(vardecl_t);  // variables: t x = e
    Fn_d(fndecl_t);    // functions  t f(t1 x1,...,tn xn) { ... }
    Let_d(pat_t,       // let p = e
          opt_t<list_t<$(vardecl_t *,exp_opt_t)@>>, // set by type-checker, used downstream
          exp_t, Tcpat::decision_opt_t);
    Letv_d(list_t<vardecl_t>); // multi-let
    Region_d(tvar_t,vardecl_t,exp_opt_t); // region declaration
    // region<`r> h; or region<`r> h = open(k);
    Aggr_d(aggrdecl_t);    // [struct|union] Foo { ... }
    Datatype_d(datatypedecl_t);    // datatype Bar { ... }
    Enum_d(enumdecl_t);        // enum Baz { ... }
    Typedef_d(typedefdecl_t);  // typedef t n
    Namespace_d(var_t,list_t<decl_t>); // namespace Foo { ... }
    Using_d(qvar_t,list_t<decl_t>);  // using Foo { ... }
    ExternC_d(list_t<decl_t>); // extern "C" { ... }
    ExternCinclude_d(list_t<decl_t>,list_t<decl_t>,list_t<$(seg_t,qvar_t,bool)@>,$(seg_t, list_t<qvar_t>)@);
    Porton_d;
    Portoff_d;
    Tempeston_d;
    Tempestoff_d;
    // extern "C include" { <decls> } export { <vars> }
    // the bool in the vars is used to warn when an export doesn't appear
    // in the declarations.
  };
  extern_datacon(Raw_decl,Porton_d);
  extern_datacon(Raw_decl,Portoff_d);
  extern_datacon(Raw_decl,Tempeston_d);
  extern_datacon(Raw_decl,Tempestoff_d);

  // declarations w/ auxiliary info
  EXTERN_ABSYN struct Decl {
    raw_decl_t r;
    seg_t  loc;
  };

  EXTERN_ABSYN @extensible datatype AbsynAnnot { EXTERN_ABSYN EmptyAnnot; };
  extern_datacon(AbsynAnnot, EmptyAnnot);

  ///////////////////////////////////////////////////////////////////
  // Operations and Constructors for Abstract Syntax
  ///////////////////////////////////////////////////////////////////

  // compare variables
  int qvar_cmp(qvar_t, qvar_t);
  int hash_qvar(qvar_t);
  int varlist_cmp(list_t<var_t>, list_t<var_t>);
  int tvar_cmp(tvar_t,tvar_t); // WARNING: ignores the kinds
  int tvar_id(tvar_t);

  ///////////////////////// Namespaces ////////////////////////////
  bool is_qvar_qualified(qvar_t);
  extern nmspace_t Loc_n;
  nmspace_t Rel_n(list_t<var_t,`H>);
  // Abs_n(ns) when C_scope is false, C_n(ns) when C_scope is true
  nmspace_t Abs_n(list_t<var_t,`H>, bool C_scope);

  ///////////////////////// Qualifiers ////////////////////////////
  tqual_t const_tqual(seg_t);
  tqual_t empty_tqual(seg_t);
  tqual_t combine_tqual(tqual_t,tqual_t);
  bool equal_tqual(tqual_t,tqual_t);

  ////////////////////////////// Types //////////////////////////////

  type_t compress(type_t);

  // convert a type of kind BoolKind to a bool.  If the type
  // is not yet constrained, leaves it unconstrained and return def.
  bool type2bool(bool def, type_t);

  // returns standard Tvar for current region `C
  type_t curr_rgn_type(void);
  tvar_t curr_rgn_tvar(void);

  type_t app_type(tycon_t, ...type_t);
  // return a fresh type variable of the given kind that can be unified
  // only with types whose free type variables are drawn from tenv.
  type_t new_evar(opt_t<kind_t,`H> k,opt_t<list_t<tvar_t,`H>,`H> tenv);
  // any memory type whose free type variables are drawn from the given list
  type_t wildtyp(opt_t<list_t<tvar_t,`H>,`H>);
  type_t int_type(sign_t,size_of_t);
  // unsigned types
  extern type_t char_type, uchar_type, ushort_type, uint_type, ulong_type, ulonglong_type;
  // signed types
  extern type_t schar_type, sshort_type, sint_type, slong_type, slonglong_type;
  // float, double, long double, wchar_t
  extern type_t float_type, double_type, long_double_type, wchar_type();
  type_t gen_float_type(unsigned);
  // complex types
  extern type_t complex_type(type_t);
  // regions
  extern rgntype_t heap_rgn_type, unique_rgn_shorthand_type, refcnt_rgn_shorthand_type;
  // alias qualifiers types
  extern aqualtype_t al_qual_type, un_qual_type, rc_qual_type, rtd_qual_type;
  // empty effect
  extern type_t empty_effect;
  // bool types
  extern booltype_t true_type, false_type;
  // misc constructors
  extern type_t void_type,
    var_type(tvar_t),
    tag_type(type_t),
    rgn_handle_type(rgntype_t),
    aqual_handle_type(type_t),
    aqual_var_type(type_t,type_t),
    aqual_constant(alias_qual_val_t),
    valueof_type(exp_t),
    typeof_type(exp_t),
    //    access_eff(type_t),
    join_eff(list_t<type_t,`H>),
    regionsof_eff(type_t),
    enum_type(typedef_name_t, struct Enumdecl *`H),
    anon_enum_type(list_t<enumfield_t,`H>),
    builtin_type(string_t<`H>, kind_t),
    tuple_type(list_t<$(tqual_t,type_t)@`H,`H> tqts),
    typedef_type(typedef_name_t,list_t<type_t,`H>,struct Typedefdecl*`H ,type_opt_t);

  // usefule for avoiding allocation/creation of a tuple field name
  datatype Designator.FieldName @ tuple_field_designator(int i);
  // exception name and type
  extern qvar_t exn_name;
  datatypedecl_t exn_tud();
  type_t exn_type();
  // datatype PrintArg and datatype ScanfArg types
  qvar_t datatype_print_arg_qvar();
  qvar_t datatype_scanf_arg_qvar();
  // string (char ?)
  type_t string_type(type_t rgn, aqualtype_t aq);
  type_t const_string_type(type_t rgn, aqualtype_t aq);
  // pointer bounds
  extern ptrbound_t fat_bound_type;
  ptrbound_t thin_bounds_type(type_t);
  ptrbound_t thin_bounds_exp(exp_t);
  ptrbound_t thin_bounds_int(unsigned int);
  ptrbound_t bounds_one(); // thin_bounds_int(1) (good for sharing)
  // pointer types
  type_t pointer_type(struct PtrInfo);
  // t *{e}`r
  type_t starb_type(type_t,rgntype_t,aqualtype_t,tqual_t, ptrbound_t, booltype_t zero_term, booltype_t rel);
  // t @{e}`r
  type_t atb_type(type_t, rgntype_t, aqualtype_t, tqual_t, ptrbound_t, booltype_t zero_term, booltype_t rel);
  // t *`r (bounds = Upper(1)
  type_t star_type(type_t, rgntype_t, aqualtype_t, tqual_t, booltype_t zero_term, booltype_t rel);
  // t @`r (bounds = Upper(1)
  type_t at_type(type_t, rgntype_t, aqualtype_t,tqual_t, booltype_t zero_term, booltype_t rel);
  // t*`H
  type_t cstar_type(type_t, tqual_t);
  // t?`r
  type_t fatptr_type(type_t t, type_t rgn, aqualtype_t, tqual_t, booltype_t zt, booltype_t rel);
  // structs
  type_t strct(var_t  name);
  type_t strctq(qvar_t name);
  type_t unionq_type(qvar_t name);
  // unions
  type_t union_type(var_t name);
  // arrays
  type_t array_type(type_t elt_type, tqual_t, exp_opt_t num_elts,
		    booltype_t zero_term, seg_t ztloc);
  // datatypes, datatype fields, and aggregates
  type_t datatype_type(datatype_info_t, types_t args);
  type_t datatype_field_type(datatype_field_info_t,types_t args);
  type_t aggr_type(aggr_info_t, types_t args);
  //type for aquals(`a::B)::Q
  type_t aqualsof_type(type_t);

  /////////////////////////////// Constants //////////////////////////
  cnst_t Char_c(sign_t,char);
  cnst_t Wchar_c(string_t<`H>);
  cnst_t Short_c(sign_t,short);
  cnst_t Int_c(sign_t,int);
  cnst_t LongLong_c(sign_t,long long);
  cnst_t Float_c(string_t<`H>,int);
  cnst_t String_c(string_t<`H>);
  cnst_t Wstring_c(string_t<`H>);

  /////////////////////////////// Expressions ////////////////////////
  exp_t new_exp(raw_exp_t, seg_t);
  exp_t New_exp(exp_opt_t rgn_handle, exp_t, exp_opt_t qual_hdl, seg_t); // New_e
  exp_t copy_exp(exp_t);
  exp_t const_exp(cnst_t, seg_t);
  exp_t null_exp(seg_t);
  exp_t bool_exp(bool, seg_t);
  exp_t true_exp(seg_t);
  exp_t false_exp(seg_t);
  exp_t int_exp(sign_t,int,seg_t);
  exp_t signed_int_exp(int, seg_t);
  exp_t uint_exp(unsigned int, seg_t);
  exp_t char_exp(char, seg_t);
  exp_t wchar_exp(string_t<`H>, seg_t);
  exp_t float_exp(string_t<`H>, int, seg_t);
  exp_t string_exp(string_t<`H>, seg_t);
  exp_t wstring_exp(string_t<`H>, seg_t);
  exp_t var_exp(qvar_t, seg_t);
  exp_t varb_exp(binding_t, seg_t);
  exp_t unknownid_exp(qvar_t, seg_t);
  exp_t tagof_exp(exp_t, seg_t);
  exp_t pragma_exp(string_t<`H>,seg_t);
  exp_t primop_exp(primop_t, list_t<exp_t,`H>, seg_t);
  exp_t prim1_exp(primop_t, exp_t, seg_t);
  exp_t prim2_exp(primop_t, exp_t, exp_t, seg_t);
  exp_t swap_exp(exp_t, exp_t, seg_t);
  exp_t add_exp(exp_t, exp_t, seg_t);
  exp_t times_exp(exp_t, exp_t, seg_t);
  exp_t divide_exp(exp_t, exp_t, seg_t);
  exp_t eq_exp(exp_t, exp_t, seg_t);
  exp_t neq_exp(exp_t, exp_t, seg_t);
  exp_t gt_exp(exp_t, exp_t, seg_t);
  exp_t lt_exp(exp_t, exp_t, seg_t);
  exp_t gte_exp(exp_t, exp_t, seg_t);
  exp_t lte_exp(exp_t, exp_t, seg_t);
  exp_t assignop_exp(exp_t, opt_t<primop_t,`H>, exp_t, seg_t);
  exp_t assign_exp(exp_t, exp_t, seg_t);
  exp_t increment_exp(exp_t, incrementor_t, seg_t);
  exp_t conditional_exp(exp_t, exp_t, exp_t, seg_t);
  exp_t and_exp(exp_t, exp_t, seg_t); // &&
  exp_t or_exp(exp_t, exp_t, seg_t);  // ||
  exp_t seq_exp(exp_t, exp_t, seg_t);
  exp_t unknowncall_exp(exp_t, list_t<exp_t,`H>, seg_t);
  exp_t fncall_exp(exp_t, list_t<exp_t,`H>, seg_t);
  exp_t throw_exp(exp_t, seg_t);
  exp_t rethrow_exp(exp_t, seg_t);
  exp_t noinstantiate_exp(exp_t, seg_t);
  exp_t instantiate_exp(exp_t, list_t<type_t,`H>, seg_t);
  exp_t cast_exp(type_t, exp_t, bool user_cast, coercion_t, seg_t);
  exp_t address_exp(exp_t, seg_t);
  exp_t sizeoftype_exp(type_t t, seg_t);
  exp_t sizeofexp_exp(exp_t e, seg_t);
  exp_t offsetof_exp(type_t, list_t<offsetof_field_t,`H>, seg_t);
  exp_t deref_exp(exp_t, seg_t);
  exp_t aggrmember_exp(exp_t, field_name_t, seg_t);
  exp_t aggrarrow_exp(exp_t, field_name_t, seg_t);
  exp_t subscript_exp(exp_t, exp_t, seg_t);
  exp_t tuple_exp(list_t<exp_t,`H>, seg_t);
  exp_t stmt_exp(stmt_t, seg_t);
  exp_t array_exp(list_t<exp_t,`H>, seg_t);
  exp_t valueof_exp(type_t, seg_t);
  exp_t asm_exp(bool, string_t<`H>, list_t<$(string_t<`H>,exp_t)@`H,`H>,
		list_t<$(string_t<`H>,exp_t)@`H,`H>,
		list_t<string_t<`H>@`H,`H>, seg_t);
  exp_t extension_exp(exp_t, seg_t);
  exp_t assert_exp(exp_t,seg_t);
  exp_t assert_false_exp(exp_t,seg_t);
  exp_t unresolvedmem_exp(opt_t<typedef_name_t,`H>,
			  list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>,
			  seg_t);

  qvar_t uniqueaqual_qvar();
  exp_t uniqueaqual_exp(); // refers to the unique region in Core::

  /////////////////////////// Statements ///////////////////////////////
  stmt_t new_stmt(raw_stmt_t,seg_t);
  stmt_t skip_stmt(seg_t);
  stmt_t exp_stmt(exp_t,seg_t);
  stmt_t seq_stmt(stmt_t,stmt_t,seg_t);
  stmt_t seq_stmts(list_t<stmt_t>,seg_t);
  stmt_t return_stmt(exp_opt_t,seg_t);
  stmt_t ifthenelse_stmt(exp_t,stmt_t,stmt_t,seg_t);
  stmt_t while_stmt(exp_t,stmt_t,seg_t);
  stmt_t break_stmt(seg_t);
  stmt_t continue_stmt(seg_t);
  stmt_t for_stmt(exp_t,exp_t,exp_t,stmt_t,seg_t);
  stmt_t switch_stmt(exp_t, list_t<switch_clause_t,`H>, seg_t);
  stmt_t fallthru_stmt(list_t<exp_t,`H>, seg_t);
  stmt_t decl_stmt(decl_t, stmt_t, seg_t);
  stmt_t declare_stmt(qvar_t,type_t,exp_opt_t init,stmt_t,seg_t);
  stmt_t label_stmt(var_t,stmt_t,seg_t);
  stmt_t do_stmt(stmt_t,exp_t,seg_t);
  stmt_t goto_stmt(var_t,seg_t);
  stmt_t assign_stmt(exp_t,exp_t,seg_t);
  stmt_t trycatch_stmt(stmt_t,list_t<switch_clause_t,`H>,seg_t);

  /////////////////////////// Patterns //////////////////////////////
  pat_t new_pat(raw_pat_t,seg_t);
  pat_t exp_pat(exp_t);

  ////////////////////////// Declarations ///////////////////////////
  decl_t new_decl(raw_decl_t,seg_t);
  decl_t let_decl(pat_t,exp_t,seg_t);
  decl_t letv_decl(list_t<vardecl_t,`H>, seg_t);
  decl_t region_decl(tvar_t,vardecl_t,exp_opt_t open_exp, seg_t);
  decl_t alias_decl(tvar_t,vardecl_t,exp_t,seg_t);
  vardecl_t new_vardecl(seg_t varloc, qvar_t, type_t, exp_opt_t init, exp_opt_t rename);
  vardecl_t static_vardecl(qvar_t, type_t, exp_opt_t init);
  struct AggrdeclImpl @ aggrdecl_impl(list_t<tvar_t,`H> exists,
				      // list_t<$(type_t,type_t)@`H,`H> po,
				      list_t<effconstr_t,`H> ec,
				      list_t<$(type_t,type_t)@`H,`H> qb,
				      list_t<aggrfield_t,`H> fs,
				      bool tagged);
  decl_t aggr_decl(aggr_kind_t, scope_t, typedef_name_t,
		   list_t<tvar_t,`H>, struct AggrdeclImpl *`H,
		   attributes_t, seg_t);
  type_decl_t aggr_tdecl(aggr_kind_t, scope_t, typedef_name_t,
			 list_t<tvar_t,`H>, struct AggrdeclImpl *`H,
			 attributes_t, seg_t);
  decl_t struct_decl(scope_t, typedef_name_t, list_t<tvar_t,`H>,
		     struct AggrdeclImpl *`H, attributes_t, seg_t);
  decl_t union_decl(scope_t, typedef_name_t, list_t<tvar_t,`H>,
		    struct AggrdeclImpl *`H, attributes_t, seg_t);
  decl_t datatype_decl(scope_t, typedef_name_t, list_t<tvar_t,`H>,
		       opt_t<list_t<datatypefield_t,`H>,`H>,
		       bool is_extensible, seg_t);
  type_decl_t datatype_tdecl(scope_t, typedef_name_t,
			     list_t<tvar_t,`H>,
			     opt_t<list_t<datatypefield_t,`H>,`H>,
			     bool is_extensible, seg_t);

  type_t function_type(list_t<tvar_t,`H> tvs,type_opt_t eff_typ,
		       tqual_t ret_tqual, type_t ret_type,
		       list_t<$(var_opt_t,tqual_t,type_t)@`H,`H> args,
		       bool c_varargs, vararg_info_t *`H cyc_varargs,
		       //		       list_t<$(type_t,type_t)@`H,`H> rgn_po,
		       list_t<effconstr_t,`H> effconstr,
		       list_t<$(type_t,type_t)@`H,`H> qb,
		       attributes_t,
		       exp_opt_t requires_clause, exp_opt_t ensures_clause,
                       exp_opt_t throws_clause);
  // turn t f(t1,...,tn) into t (@f)(t1,...,tn) -- when fresh_evar is
  // true, generates a fresh evar for the region of f else uses `H
  type_t pointer_expand(type_t, bool fresh_evar);
  // returns true when the expression is a valid left-hand-side
  bool is_lvalue(exp_t);
  // returns true when the expression has no side effects
  bool no_side_effects_exp(exp_t);

  // find a field by name from a list of fields
  struct Aggrfield *lookup_field(list_t<aggrfield_t>,var_t);
  // find a struct or union field by name from a declaration
  struct Aggrfield *lookup_decl_field(aggrdecl_t,var_t);
  // find a tuple field form a list of qualifiers and types
  $(tqual_t,type_t)*lookup_tuple_field(list_t<$(tqual_t,type_t)@`H>,int);
  // find a decl by name within a list of decls.  Return NULL if not found.
  struct Decl *lookup_decl(list_t<decl_t>, stringptr_t<`H>);
  // get the name of decl; return NULL if has no name
  string_t<`H> *decl_name(decl_t);
  // int to field-name caching used by control-flow and toc
  field_name_t fieldname(int);
  // get the name and aggr_kind of an aggregate type
  $(aggr_kind_t,qvar_t) aggr_kinded_name(aggr_info_t);
  // given a checked type, get the decl
  aggrdecl_t get_known_aggrdecl(aggr_info_t);
  // ditto except rule out @tagged unions and @require unions
  bool is_nontagged_nonrequire_union_type(type_t);
  // a union (anonymous or otherwise) that has requires clauses on the fields
  bool is_require_union_type(type_t);

  qvar_t binding2qvar(binding_t);

  // call Warn::impos if list is not length 1 or holds an ArrayElement
  var_t designatorlist_to_fieldname(list_t<designator_t>);

  // for visiting terms
  void visit_stmt(bool (@)(`a,exp_t), bool (@)(`a,stmt_t), `a, stmt_t);
  void visit_exp(bool (@)(`a,exp_t), bool (@)(`a,stmt_t), `a, exp_t);

  // for recurring through exps to process statement-exps
  void do_nested_statement(exp_t, `a, void (@f)(`a, stmt_t), bool do_szeof);

  // might the var appear (free) in the expression (does _not_ recur thru types)
  bool var_may_appear_exp(qvar_t,exp_t);
}

#endif
