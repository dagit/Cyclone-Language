/* parse.y:  Copyright (c) 2000, Greg Morrisett & Trevor Jim,
 * all rights reserved.
 *
 * An adaptation of the OSI C grammar for Cyclone.  This grammar
 * is adapted from the proposed C9X standard, but the productions are
 * arranged as in Kernighan and Ritchie's "The C Programming
 * Language (ANSI C)", Second Edition, pages 234-239.
 *
 * The grammar has 1 shift-reduce conflict, due to the "dangling else"
 * problem, which is properly resolved by the Cyclone port of Bison.  */

%{
#define YYDEBUG 0 // 1 to debug, 0 otherwise
#define YYPRINT yyprint

#if YYDEBUG==1
extern xenum YYSTYPE;
extern void yyprint(int i, xenum YYSTYPE v);
#endif


#include "core.h"
#include "lexing.h"
#include "list.h"
#include "string.h"
#include "set.h"
#include "position.h"
#include "absyn.h"
#include "pp.h"
#include "fn.h"
using Core;
using Lexing;
using List;
using Absyn;
using Position;
using Fn;
using String;

/* Typedef processing must be split between the parser and lexer.
   These functions are called by the parser to communicate typedefs
   to the lexer, so that the lexer can distinguish typedef names from
   identifiers. */
namespace Lex {
  extern void register_typedef(qvar s);
  extern void enter_namespace(string);
  extern void leave_namespace();
  extern void enter_using(qvar);
  extern void leave_using();
}

#define LOC(s,e) Position::segment_of_abs(s.first_line,e.last_line)
#define DUMMYLOC null

namespace Parse {
enum Struct_or_union {
  Struct_su, Union_su;
};
typedef enum Struct_or_union struct_or_union_t;

enum Blockitem {
  TopDecls_bl(list<decl>);
  Stmt_bl(stmt);
  FnDecl_bl(fndecl);
};
typedef enum Blockitem blockitem_t;

enum Type_specifier {
  Signed_spec(segment);
  Unsigned_spec(segment);
  Short_spec(segment,bool);  /* true -> boxed */
  Long_spec(segment,bool);   /* true -> boxed */
  Type_spec(typ,segment);    /* int, `a, list<`a>, etc. */
  Decl_spec(decl);
};
typedef enum Type_specifier type_specifier_t;

enum Storage_class {
  Typedef_sc, Extern_sc, Static_sc, Auto_sc, Register_sc, Abstract_sc;
};
typedef enum Storage_class storage_class_t;

struct Declaration_spec {
  Opt_t<storage_class_t> sc;
  tqual                  tq;
  list<type_specifier_t>   type_specs;
  bool                   is_inline;
};
typedef struct Declaration_spec @decl_spec_t;

struct Declarator {
  qvar                id;
  list<type_modifier> tms;
};
typedef struct Declarator @declarator_t;

struct Abstractdeclarator {
  list<type_modifier> tms;
};
typedef struct Abstractdeclarator @abstractdeclarator_t;
// declare prototypes for functions
extern void err(string msg, segment sg);
extern `a abort<`a>(string msg, segment sg);
extern `a unimp<`a>(string msg, segment sg);
extern void unimp2(string msg, segment sg);
extern void warn(string msg, segment sg);
extern $(var,tqual,typ)@
  make_struct_field(segment loc,$(qvar,tqual,typ,list<tvar>)@ field);
extern $(Opt_t<var>,tqual,typ)@ make_param(segment loc,
					   $(Opt_t<qvar>,tqual,typ,
					     list<tvar>)@ field);
extern type_specifier_t type_spec(typ t,segment loc);
extern $(tqual,typ)@ get_tqual_typ(segment loc,$(Opt_t<var>,tqual,typ)@ t);
extern $(var,tqual,typ)@ make_fn_args(segment loc,$(Opt_t<qvar>,tqual,typ)@ t);
extern void only_vardecl(list<string> params,decl x);
extern $(Opt_t<var>,tqual,typ)@ get_param_type($(list<decl>,segment)@ env,
					       string x);
extern bool is_typeparam(type_modifier tm);
extern list<type_modifier> oldstyle2newstyle(list<type_modifier> tms,
					     list<decl> tds, segment loc);
extern fndecl make_function(Opt_t<decl_spec_t> dso, declarator_t d,
			    list<decl> tds, stmt body, segment loc);

extern $(var,tqual,typ)@ fnargspec_to_arg(segment loc,
					   $(Opt_t<var>,tqual,typ)@ t);
extern typ change_signed(bool s,typ t,segment seg);
extern typ change_size(size_of sz, bool is_boxed, typ t,segment seg);
extern typ new_box_evar(tvar v);
extern $(typ,Opt_t<decl>)@
  collapse_type_specifiers (list<type_specifier_t> ts, segment loc);
extern list<$(qvar,tqual,typ,list<tvar>)@>
  apply_tmss(tqual tq,typ t,list<declarator_t> ds);
extern $(tqual,typ,list<tvar>)@
  apply_tms(tqual tq,typ t,list<type_modifier> tms);
extern var typ2tvar(segment loc, typ t);
extern typ tvar2typ(var v);
extern stmt flatten_decl(decl d,stmt s);
extern stmt flatten_declarations(list<decl> ds, stmt s);
extern list<decl> make_declarations(Opt_t<decl_spec_t> dso,
				    list<$(declarator_t,Opt_t<exp>)@> ids,
				    segment loc);
extern decl v_typ_to_typedef(segment loc, $(qvar,tqual,typ,list<tvar>)@ t);
Opt_t<Lexbuf<Function_lexbuf_state<FILE@>>> lbuf = null;
xenum exn {ParseError};
static list<decl> parse_result = null;

/**************** Definitions and Helper Functions ****************/
/* Error functions */
static void err(string msg, segment sg) {
  post_error(mk_err_parse(sg,msg));
}
static `a abort<`a>(string msg,segment sg) {
  err(msg,sg);
  throw Exit;
}
static `a unimp<`a>(string msg,segment sg) {
  return abort(xprintf("%s unimplemented",msg),sg);
}
static void unimp2(string msg,segment sg) {
  fprintf(cyc_stderr,"%s: Warning: Cyclone does not yet support %s\n",
	  string_of_segment(sg),msg);
  return;
}

static void warn(string msg,segment sg) {
  fprintf(cyc_stderr,"%s: Warning: %s\n",string_of_segment(sg),msg);
  return;
}

static $(var,tqual,typ)@
make_struct_field(segment loc,$(qvar,tqual,typ,list<tvar>)@ field) {
  if (field[3] != null)
    err("bad type params in struct field",loc);
  let qid = field[0];
  if (qid[0] != null)
    err("struct field cannot be qualified with a module name",loc);
  return &$(qid[1],field[1],field[2]);
}

static $(Opt_t<var>,tqual,typ)@
  make_param(segment loc,$(Opt_t<qvar>,tqual,typ,list<tvar>)@ field) {
  let &$(qv_opt,tq,t,tvs) = field;
  let idopt = null;
  if (qv_opt != null) {
    idopt = &Opt(qv_opt->v[1]);
    if (qv_opt->v[0] != null)
      err("parameter cannot be qualified with a module name",loc);
  }
  if (tvs != null)
    abort("parameter should have no type parameters",loc);
  return &$(idopt,tq,t);
}

/* Functions for creating abstract syntax */
static type_specifier_t type_spec(typ t,segment loc) {
  return Type_spec(t,loc);
}

/* Functions for converting strange C-like declarative constructs into
 * abstract syntax.  */

/* given an optional variable, tqual, type, and list of type
 * variables, return the tqual and type and check that the type
 * variables are null -- used when we have a tuple type specification.  */
static $(tqual,typ)@ get_tqual_typ(segment loc,$(Opt_t<var>,tqual,typ) @t) {
  return &$(t[1],t[2]);
}

/* Given a variable option, tqual, type, and type var list, make sure
 * the variable is present and the type var list is empty and return
 * the variable, tqual and type.  Used in producing function
 * arguments. */
static $(var,tqual,typ)@ make_fn_args(segment loc,$(Opt_t<qvar>,tqual,typ)@ t) {
  if (t[0] == null)
    return abort("missing variable in function prototype",loc);
  if (t[0]->v[0] != null)
    err("function arguments cannot be qualified with a module name",loc);
  return &$(t[0]->v[1],t[1],t[2]);
}

static void only_vardecl(list<string> params,decl x) {
  string decl_kind;
  switch (x->r) {
  case Var_d(vd):
    if (vd->initializer!=null)
      abort("initializers are not allowed in parameter declarations",x->loc);
    if (vd->name[0] != null)
      err("module names not allowed on parameter declarations",x->loc);
    // for sanity-checking of old-style parameter declarations
    bool found = false;
    for(; params != null; params = params->tl)
      if(zstrcmp(vd->name[1], params->hd)==0) {
	found = true;
	break;
      }
    if (!found)
      abort(xprintf("%s is not listed as a parameter",vd->name[1]),x->loc);
    return;
  case Let_d(_,_,_,_):   decl_kind = "let declaration";        break;
  case Fn_d(_):          decl_kind = "function declaration";   break;
  case Struct_d(_):      decl_kind = "struct declaration";     break;
  case Union_d:          decl_kind = "union declaration";      break;
  case Enum_d(_):        decl_kind = "enum declaration";       break;
  case Typedef_d(_):     decl_kind = "typedef";                break;
  case Xenum_d(_):       decl_kind = "xenum declaration";      break;
  case Namespace_d(_,_): decl_kind = "namespace declaration";  break;
  case Using_d(_,_):     decl_kind = "using declaration";      break;
  }
  abort(xprintf("%s appears in parameter type", decl_kind), x->loc);
  return;
}

// For old-style function definitions,
// get a parameter type from a list of declarations
static $(Opt_t<var>,tqual,typ)@ get_param_type($(list<decl>,segment)@ env,
					       string x) {
  let &$(tdl,loc) = env;
  if (tdl==null)
    return(abort(xprintf("missing type for parameter %s",x),loc));
  switch (tdl->hd->r) {
  case Var_d(vd):
    if (vd->name[0] != null)
      err("module name not allowed on parameter",loc);
    if (zstrcmp(vd->name[1],x)==0)
      // Fix: cast needed here
      return &$((Opt_t<var>)&Opt(vd->name[1]),vd->tq,vd->type);
    else return get_param_type(&$(tdl->tl,loc),x);
  default:
    // This should never happen, because we use only_vardecl first
    return(abort("non-variable declaration",tdl->hd->loc));
  }
}

static bool is_typeparam(type_modifier tm) {
  switch (tm) {
  case TypeParams_mod(_,_): return true;
  default: return false;
  }
}

// Convert an old-style function into a new-style function
static list<type_modifier>
oldstyle2newstyle(list<type_modifier> tms, list<decl> tds, segment loc) {

  // Not an old-style function
  if (tds==null) return tms;

  // If no function is found, or the function is not innermost, then
  // this is not a function definition; it is an error.  But, we
  // return silently.  The error will be caught by make_function,
  // below.
  if (tms==null) return null;

  switch (tms->hd) {
    case Function_mod(args): {
      // Is this the innermost function??
      if (tms->tl==null ||
          (is_typeparam(tms->tl->hd) && tms->tl->tl==null)) {
        // Yes
        switch (args) {
        case WithTypes(_,_):
          warn("function declaration with both new- and old-style parameter"
	       "declarations; ignoring old-style",loc);
          return tms;
        case NoTypes(ids,_):
          List::iter_c(only_vardecl,ids,tds);
          let env = &$(tds,loc);
          return
            &cons(Function_mod(WithTypes(List::map_c(get_param_type,env,ids),
					 false)),null);
        }
      } else
        // No, keep looking for the innermost function
        return &cons(tms->hd,oldstyle2newstyle(tms->tl,tds,loc));
    }
    default:
      return &cons(tms->hd,oldstyle2newstyle(tms->tl,tds,loc));
  }
}

/* make a top-level function declaration out of a
 * declaration-specifier (return type, etc.),
 * a declarator (the function name and args),
 * a declaration list (for old-style function definitions),
 * and a statement */
static fndecl make_function(Opt_t<decl_spec_t> dso, declarator_t d,
			    list<decl> tds, stmt body, segment loc) {
  // Handle old-style parameter declarations
  if (tds!=null) 
    d = &Declarator(d->id,oldstyle2newstyle(d->tms,tds,loc));

  scope sc = Public;
  list<type_specifier_t> tss = null;
  tqual tq = empty_tqual();
  bool is_inline = false;

  if (dso != null) {
    tss = dso->v->type_specs;
    tq = dso->v->tq;
    is_inline = dso->v->is_inline;
    /* Examine storage class; like C, we allow both static and extern */
    if (dso->v->sc != null)
      switch (dso->v->sc->v) {
      case Extern_sc: sc = Extern; break;
      case Static_sc: sc = Static; break;
      default: err("bad storage class on function",loc); break;
      }
  }
  let &$(t,decl_opt)         = collapse_type_specifiers(tss,loc);
  let &$(fn_tqual,fn_type,x) = apply_tms(tq,t,d->tms);
  if (decl_opt != null)
    warn("nested type declaration within function prototype",loc);
  if (x != null)
    // Example:   `a f<`b><`a>(`a x) {...}
    // Here info[2] will be the list `b.
    warn("bad type params, ignoring",loc);
  // fn_type had better be a FnType
  switch (fn_type) {
    case FnType(tvs,ret_type,args,varargs):
      let args2 = List::map_c(fnargspec_to_arg,loc,args);
      return &Fndecl {.sc=sc,.name=d->id,.tvs=tvs,
                      .is_inline=is_inline,.ret_type=ret_type,
                      .args=args2,.varargs=varargs,.body=body};
    default:
      return abort("declarator is not a function prototype",loc);
  }
}

static $(var,tqual,typ)@
fnargspec_to_arg(segment loc,$(Opt_t<var>,tqual,typ)@ t) {
  if (t[0] == null) {
    err("missing argument variable in function prototype",loc);
    return &$((var)"?",t[1],t[2]);     // Fix: explicit cast needed
  } else
    return &$(t[0]->v,t[1],t[2]);
}

/* given a numeric type convert the sign to s */
static typ change_signed(bool s,typ t,segment seg) {
  switch (t) {
  case IntType(sn,b,x): return IntType(s ? Signed : Unsigned, b, x);
  default: return abort("parse:make_sign_s:",seg);
  }
}

/* given a numeric type, sz (B[1|2|4|8]), and a boxed-ness, check
 * that the type is also a boxed integral type and change the size to
 * match sz */
static typ change_size(size_of sz, bool is_boxed, typ t,segment seg) {
  switch (t) {
  case IntType(sn,szi,b):
    if (b == Boxed && !is_boxed)
      return abort("unboxed size qualifier on boxed integral type",seg);
    else if (b == Unboxed && is_boxed)
      return abort("boxed size qualifier on unboxed integral type",seg);
    else
      return IntType(sn,sz,b);
  default:
    return abort("size qualifier on non-integral type",seg);
  }
}

/* create a new (boxed) unification variable */
static typ new_box_evar(tvar v) {
  return new_evar(BoxKind);
}

/* Given a type-specifier list, determines the type and any declared
 * structs, unions, or enums.  Most of this is just collapsing
 * combinations of [un]signed, short, long, int, char, etc.  We're
 * probably more permissive than is strictly legal here.  For
 * instance, one can write "unsigned const int" instead of "const
 * unsigned int" and so forth.  I don't think anyone will care...
 * (famous last words.)  */
static $(typ,Opt_t<decl>)@
collapse_type_specifiers (list<type_specifier_t> ts, segment loc) {

  /* flags that record what we've seen in the qualifiers */
  Opt_t<bool> signopt = null;      /* true->signed, false->unsigned */
  Opt_t<typ> topt = null;        /* the underlying type (if any) */
  Opt_t<decl> declopt = null;      /* any hidden declarations */
  bool is_short = false;         /* short or Short */
  bool is_long = false;          /* long or Long */
  bool is_longlong = false;      /* long long or Long Long */
  bool is_boxed = false;         /* e.g., is_short -> Short, etc. */

  typ the_type;                  /* the final type */

  segment last_loc = loc;
  string msg1 = "at most one type may appear within a type specifier";
  string msg2 =
    "const or volatile may only appear once within a type specifier";
  string msg3 = "type specifier includes more than one declaration";
  string msg4 = "sign specifier may only appear once within a type specifier";

  while (ts != null) {
    switch (ts->hd) {
    case Signed_spec(loc2):
      last_loc = loc2;
      if (signopt != null) err(msg4,loc2);
      if (topt != null) err("signed qualifier must come before type",loc2);
      signopt = &Opt(true);
      break;
    case Unsigned_spec(loc2):
      last_loc = loc2;
      if (signopt != null) err(msg4,loc2);
      if (topt != null) err("signed qualifier must come before type",loc2);
      signopt = &Opt(false);
      break;
    case Short_spec(loc2,box):
      last_loc = loc2;
      if (is_short || is_long || is_longlong)
        err("integral size may only appear once within a type specifier",loc2);
      else if (topt != null)
        err("short modifier must come before base type",loc2);
      else is_short = true;
      is_boxed = box;
      break;
    case Long_spec(loc2,box):
      last_loc = loc2;
      if (is_short || is_longlong)
        err("extra long in type specifier",loc2);
      else if (topt != null)
        err("long modifier must come before base type",loc2);
      if (is_long) {
        /* if we've already seen long once, then we have long long (B8) */
        if (box != is_boxed)
          err("must use long long or LONG LONG",loc2);
        is_longlong = true;
        is_long = false;
      } else {
        /* otherwise, it's just a long (B4) */
        is_long = true;
        is_boxed = box;
      }
      break;
    case Type_spec(t,loc2):
      last_loc = loc2;
      if (topt == null) topt = &Opt(t); else err(msg1,loc2);
      break;
    case Decl_spec(d):
      // we've got a struct or enum declaration embedded in here -- return
      // the declaration as well as a copy of the type -- this allows
      // us to declare structs as a side effect inside typedefs
      last_loc = d->loc;
      if (declopt == null && topt == null) {
        switch (d->r) {
        case Struct_d(sd):
          let args = List::map(tvar2typ,sd->tvs);
          topt = &Opt(StructType(sd->name,args,null));
          if (sd->fields!=null) declopt = &Opt(d);
	  break;
        case Enum_d(ed):
          let args = List::map(tvar2typ,ed->tvs);
          topt = &Opt(EnumType(ed->name,args,null));
          if (ed->fields!=null) declopt = &Opt(d);
	  break;
        case Xenum_d(xed):
          topt = &Opt(XenumType(xed->name,null));
          if (xed->fields != null) declopt = &Opt(d);
	  break;
        case Union_d:
          // FIX: TEMPORARY SO WE CAN TEST THE PARSER
          topt = &Opt(UnionType);
	  break;
        default:
          abort("bad declaration within type specifier",d->loc);
	  break;
        }
      } else err(msg3,d->loc);
      break;
    }
    ts = ts->tl;
  }
  /* sanity check */
  if ((is_short && (is_long || is_longlong)) || (is_long && is_longlong))
    abort("parse.y:collapse_type_specifiers:sanity check failed!",last_loc);
  /* it's okay to not have an explicit type as long as we have some
   * combination of signed, unsigned, short, long, or longlong */
  if (topt == null) {
    sign s = (signopt == null || (bool)signopt->v) ? Signed : Unsigned;
    boxed box = (is_boxed ? Boxed : Unboxed);
    if (is_short) {
      the_type = IntType(s, B2, box);
    } else if (is_long) {
      the_type = IntType(s, B4, box);
    } else if (is_longlong) {
      the_type = IntType(s, B8, box);
    } else if (signopt != null) {
      the_type = IntType(s, B4, box);
    } else {
      the_type = abort("missing type within specifier",last_loc);
    };
  } else {
    the_type = topt->v;
    if (signopt != null)
      the_type = change_signed((bool)signopt->v,the_type,last_loc);
    if (is_short)
      the_type = change_size(B2,is_boxed,the_type,last_loc);
    else if (is_long)
      the_type = change_size(B4,is_boxed,the_type,last_loc);
    else if (is_longlong)
      the_type = change_size(B8,is_boxed,the_type,last_loc);
  }
  return &$(the_type, declopt);
}

static list<$(qvar,tqual,typ,list<tvar>)@>
apply_tmss(tqual tq,typ t,list<declarator_t> ds) {
  if (ds==null) return null;
  let d = ds->hd;
  let q = d->id;
  $(tqual,typ,list<tvar>) @p = apply_tms(tq,t,d->tms);
  return &cons(&$(q,p[0],p[1],p[2]),apply_tmss(tq,t,ds->tl));
}

static $(tqual,typ,list<tvar>)@
apply_tms(tqual tq,typ t,list<type_modifier> tms)
{
  if (tms==null) return &$(tq,t,null);
  switch (tms->hd) {
    case Carray_mod:
      return apply_tms(empty_tqual(),ArrayType(t,tq,UntaggedArray),tms->tl);
    case Array_mod:
      return apply_tms(empty_tqual(),ArrayType(t,tq,TaggedArray),tms->tl);
    case ConstArray_mod(e):
      return apply_tms(empty_tqual(),ArrayType(t,tq,FixedArray(e)),tms->tl);
    case Function_mod(args): {
      switch (args) {
      case WithTypes(args2,vararg):
        list<tvar> typvars = null;
        // functions consume type parameters
        if (tms->tl != null) {
          switch (tms->tl->hd) {
          case TypeParams_mod(ts,_):
            typvars = ts;
            tms=tms->tl; // skip TypeParams on call of apply_tms below
	    break;
          default:
            break;
          }
        }
        // special case where the parameters are void, e.g., int f(void)
        if (!vararg                // not vararg function
            && args2 != null      // not empty arg list
            && args2->tl == null   // not >1 arg
            && args2->hd[0] == null // not f(void x)
            && args2->hd[2] == VoidType) {
	  args2 = null;
	  vararg = false;
	}
        // Note, we throw away the tqual argument.  An example where
        // this comes up is "const int f(char c)"; it doesn't really
        // make sense to think of the function as returning a const
        // (or volatile, or restrict).  The result will be copied
        // anyway.  TODO: maybe we should issue a warning.  But right
        // now we don't have a loc so the warning will be confusing.
        return apply_tms(empty_tqual(),
			 function_typ(typvars,t,args2,vararg),tms->tl);
      case NoTypes(_,loc):
        return(abort("function declaration without parameter types",loc));
      }
      return(abort("can't get here 1",DUMMYLOC)); // Fix
    }
    case TypeParams_mod(ts,loc): {
      // If we are the last type modifier, this could be the list of
      // type parameters to a typedef:
      // typedef struct foo<`a,int> foo_t<`a>
      if (tms->tl==null)
        return &$(tq,t,ts);
      // Otherwise, it is an error in the program if we get here;
      // TypeParams should already have been consumed by an outer
      // Function (see last case).
      return(abort("type parameters must appear before function arguments "
		   "in declarator",loc));
    }
    case Pointer_mod(nullable,rgn,tq2): {
      return apply_tms(tq2,PointerType(t,rgn,new_conref(nullable),tq),
		       tms->tl);
    }
  }
  return(abort("can't get here 2",DUMMYLOC)); // Fix
}

/* convert a list of types to a list of typevars -- the parser can't
 * tell lists of types apart from lists of typevars easily so we parse
 * them as types and then convert them back to typevars.  See
 * productions "struct_or_union_specifier" and "enum_specifier"; */
static var typ2tvar(segment loc, typ t) {
  switch (t) {
  case VarType(x): return x;
  default: return abort("expecting a list of type variables, not types",loc);
  }
}

static typ tvar2typ(var v) {
  return VarType(v);
}

/* given a local declaration and a statement produce a decl statement */
static stmt flatten_decl(decl d,stmt s) {
  return new_stmt(Decl_s(d,s),segment_join(d->loc,s->loc));
}
/* given a list of local declarations and a statement, produce a big
   decl statement. */
static stmt flatten_declarations(list<decl> ds, stmt s){
  return List::fold_right(flatten_decl,ds,s);
}

/* Given a declaration specifier list (a combination of storage class
   [typedef, extern, static, etc.] and type specifiers (signed, int,
   `a, const, etc.), and a list of declarators and initializers,
   produce a list of top-level declarations.  By far, this is the most
   involved function and thus I expect a number of subtle errors. */
static list<decl> make_declarations(Opt_t<decl_spec_t> dso,
				    list<$(declarator_t,Opt_t<exp>)@> ids,
				    segment loc) {
  list<type_specifier_t> tss       = null;
  Opt_t<scope>           scopeopt  = null; 
  bool                   istypedef = false;       
  tqual                  tq        = empty_tqual();

  if (dso != null) {
    tss = dso->v->type_specs;
    tq  = dso->v->tq;

    if (dso->v->is_inline)
      err("inline is only allowed on function definitions",loc);

    // figure out the scope and what kind of declaration applies
    // to each declarator.
    if (dso->v->sc != null)
      switch (dso->v->sc->v) {
      case Typedef_sc:
        if (istypedef)
          err("extra typedef keyword in declaration",loc);
        istypedef = true;
	break;
      case Extern_sc:
        if (scopeopt != null) err("extra extern keyword in declaration",loc);
        scopeopt = &Opt(Extern);
	break;
      case Static_sc:
        if (scopeopt != null) err("extra static keyword in declaration",loc);
        scopeopt = &Opt(Static);
	break;
      case Auto_sc:
        if (scopeopt != null) err("extra auto keyword in declaration",loc);
        scopeopt = &Opt(Public);
	break;
      case Register_sc:
        if (scopeopt != null) err("extra register keyword in declaration",loc);
        scopeopt = &Opt(Public);
	break;
      case Abstract_sc:
        if (scopeopt != null) err("extra abstract keyword in declaration",loc);
        scopeopt = &Opt(Abstract);
	break;
      }
  }
  scope s = (scopeopt == null ? Public : scopeopt->v);

  // separate the declarators from their initializers
  let $(declarators, exprs) = List::split(ids);
  // check to see if there are no initializers -- useful later on
  bool exps_empty = true;
  for (list<Opt_t<exp>> es = exprs; es != null; es = es->tl)
    if (es->hd != null) {
      exps_empty = false;
      break;
    }

  if (tss == null) {
    err("missing type specifiers in declaration",loc);
    return null;
  }
  /* Collapse the type specifiers to get the base type and any
   * optional nested declarations */
  let ts_info = collapse_type_specifiers(tss,loc);
  if (declarators == null) {
    /* here we have a type declaration -- either a struct, union,
       enum, or xenum as in: "struct Foo { ... };" */
    typ t = ts_info[0];
    Opt_t<decl> declopt = ts_info[1];
    if (declopt != null) {
      decl d = declopt->v;
      switch (d->r) {
      case Struct_d(sd): sd->sc = s; break;
      case Enum_d(ed)  : ed->sc = s; break;
      case Xenum_d(ed) : ed->sc = s; break;
      default: err("bad declaration",loc); return null;
      }
      return &cons(d,null);
    } else {
      switch (t) {
      case StructType(n,ts,_):
        let ts2 = List::map_c(typ2tvar,loc,ts);
        let sd  = &Structdecl{.sc = s, .name = n, .tvs = ts2, .fields = null};
        return &cons(&Decl(Struct_d(sd),loc),null);
      case EnumType(n,ts,_):
        let ts2 = List::map_c(typ2tvar,loc,ts);
        let ed  = &Enumdecl{.sc = s, .name = n, .tvs = ts2, .fields = null};
        return &cons(&Decl(Enum_d(ed),loc),null);
      case XenumType(n,_):
        let ed = &Xenumdecl{.sc=s, .name=n, .fields=null};
        return &cons(&Decl(Xenum_d(ed),loc),null);
      case UnionType:
        /* FIX: TEMPORARY SO WE CAN TEST THE PARSER */
        return &cons(&Decl(Union_d,loc),null);
      default: err("missing declarator",loc); return null;
      }
    }
  } else {
    /* declarators != null */
    typ t = ts_info[0];
    list<$(qvar,tqual,typ,list<tvar>)@>
       fields = apply_tmss(tq,t,declarators);
    if (istypedef) {
      /* we can have a nested struct, union, enum, or xenum
         declaration within the typedef as in:
         typedef struct Foo {...} t; */
      if (!exps_empty) err("initializer in typedef declaration",loc);
      list<decl> decls = List::map_c(v_typ_to_typedef,loc,fields);
      if (ts_info[1] != null) {
        decl d = ts_info[1]->v;
        switch (d->r) {
        case Struct_d(sd): sd->sc = s; break;
        case Enum_d(ed)  : ed->sc = s; break;
        case Xenum_d(ed) : ed->sc = s; break;
        default:
          err("declaration within typedef is not a struct, enum, or xenum",
              loc);
	  break;
        }
        decls = &cons(d,decls);
      }
      return decls;
    } else {
      /* here, we have a bunch of variable declarations */
      if (ts_info[1] != null)
        unimp2("nested type declaration within declarator",loc);
      list<decl> decls = null;
      list<$(qvar,tqual,typ,list<tvar>)@> ds;
      for (ds = fields; ds != null; ds = ds->tl) {
	let &$(x,tq2,t2,tvs2) = ds->hd;
        if (tvs2 != null)
          warn("bad type params, ignoring",loc);
        if (exprs == null)
          abort("unexpected null in parse!",loc);
        let eopt = exprs->hd;
        exprs = exprs->tl;
        let vd = new_vardecl(x, t2, eopt);
	vd->tq = tq2;
	vd->sc = s;
        let d = &Decl(Var_d(vd),loc);
        decls = &cons(d,decls);
      }
      return (List::imp_rev(decls));
    }
  }
}

/* convert an (optional) variable, tqual, type, and type
   parameters to a typedef declaration.  As a side effect, register
   the typedef with the lexer.  */
/* TJ: the tqual should make it into the typedef as well,
   e.g., typedef const int CI; */
static decl v_typ_to_typedef(segment loc, $(qvar,tqual,typ,list<tvar>)@ t) {
  qvar x = t[0];
  // tell the lexer that x is a typedef identifier
  Lex::register_typedef(x);
  let td = &Typedefdecl {.name = x, .tvs = t[3], .defn = t[2]};
  return new_decl(Typedef_d(td),loc);
}
} // end namespace Parse
using Parse;
%}

/* ANSI C keywords */
%token AUTO REGISTER STATIC EXTERN TYPEDEF VOID CHAR SHORT INT LONG FLOAT
%token DOUBLE SIGNED UNSIGNED CONST VOLATILE RESTRICT
%token STRUCT UNION CASE DEFAULT INLINE
%token IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN SIZEOF ENUM
/* Cyc:  CYCLONE additional keywords */
%token BOXED_CHAR BOXED_SHORT BOXED_INT BOXED_LONG
%token BOXED_FLOAT BOXED_DOUBLE NULL LET THROW TRY CATCH
%token NEW ABSTRACT FALLTHRU USING NAMESPACE XENUM
%token FILL CODEGEN CUT SPLICE
%token PRINTF FPRINTF XPRINTF 
/* double and triple-character tokens */
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN ELLIPSIS LEFT_RIGHT
/* identifiers and constants */
%token IDENTIFIER INTEGER_CONSTANT STRING
%token CHARACTER_CONSTANT FLOATING_CONSTANT
/* Cyc: type variables, qualified identifers and typedef names */
%token TYPE_VAR QUAL_IDENTIFIER QUAL_TYPEDEF_NAME
/* the union type for all productions -- for now a placeholder */
%union {
  Okay_tok;
  Int_tok($(sign,int)@);
  Char_tok(char);
  Short_tok(short);
  String_tok(string);
  StringOpt_tok(Opt_t<string>);
  Type_tok(typ);
  TypeList_tok(list<typ>);
  Exp_tok(exp);
  ExpList_tok(list<exp>);
  Primop_tok(primop);
  PrimopOpt_tok(Opt_t<primop>);
  QualId_tok(qvar);
  Stmt_tok(stmt);
  SwitchClauseList_tok(list<switch_clause>);
  Pattern_tok(pat);
  PatternList_tok(list<pat>);
  FnDecl_tok(fndecl);
  DeclList_tok(list<decl>);
  DeclSpec_tok(decl_spec_t);
  InitDecl_tok($(declarator_t,Opt_t<exp>)@);
  InitDeclList_tok(list<$(declarator_t,Opt_t<exp>)@>);
  StorageClass_tok(storage_class_t);
  TypeSpecifier_tok(type_specifier_t);
  QualSpecList_tok($(tqual,list<type_specifier_t>)@);
  TypeQual_tok(tqual);
  StructFieldDeclList_tok(list<$(string,tqual,typ)@>);
  StructFieldDeclListList_tok(list<list<$(string,tqual,typ)@>>);
  Declarator_tok(declarator_t);
  DeclaratorList_tok(list<declarator_t>);
  AbstractDeclarator_tok(abstractdeclarator_t);
  EnumeratorField_tok(enumfield);
  EnumeratorFieldList_tok(list<enumfield>);
  ParamDecl_tok($(Opt_t<var>,tqual,typ)@);
  ParamDeclList_tok(list<$(Opt_t<var>,tqual,typ)@>);
  ParamDeclListBool_tok(list<$(Opt_t<var>,tqual,typ)@>,bool);
  StructOrUnion_tok(struct_or_union_t);
  IdList_tok(list<var>);
  Designator_tok(designator);
  DesignatorList_tok(list<designator>);
  TypeModifierList_tok(list<type_modifier>);
  Rgn_tok(rgn_t);
  InitializerList_tok(list<$(list<designator>,exp)@>);
  FieldPattern_tok($(list<designator>,pat)@);
  FieldPatternList_tok(list<$(list<designator>,pat)@>);
  BlockItem_tok(blockitem_t);
}
/* types for productions */
%type <Int_tok> INTEGER_CONSTANT
%type <String_tok> FLOATING_CONSTANT
%type <Char_tok> CHARACTER_CONSTANT
%type <String_tok> IDENTIFIER TYPE_VAR STRING
%type <String_tok> namespace_action
%type <Exp_tok> primary_expression postfix_expression unary_expression
%type <Exp_tok> cast_expression constant multiplicative_expression
%type <Exp_tok> additive_expression shift_expression relational_expression
%type <Exp_tok> equality_expression and_expression exclusive_or_expression
%type <Exp_tok> inclusive_or_expression logical_and_expression
%type <Exp_tok> logical_or_expression conditional_expression
%type <Exp_tok> assignment_expression expression constant_expression
%type <Exp_tok> initializer
%type <ExpList_tok> argument_expression_list argument_expression_list0
%type <InitializerList_tok> initializer_list
%type <Primop_tok> unary_operator format_primop
%type <PrimopOpt_tok> assignment_operator
%type <QualId_tok> QUAL_IDENTIFIER QUAL_TYPEDEF_NAME qual_opt_identifier
%type <QualId_tok> using_action
%type <Stmt_tok> statement labeled_statement
%type <Stmt_tok> compound_statement block_item_list
%type <BlockItem_tok> block_item
%type <Stmt_tok> expression_statement selection_statement iteration_statement
%type <Stmt_tok> jump_statement
%type <SwitchClauseList_tok> switch_clauses
%type <Pattern_tok> pattern
%type <PatternList_tok> tuple_pattern_list tuple_pattern_list0
%type <FieldPattern_tok> field_pattern
%type <FieldPatternList_tok> field_pattern_list field_pattern_list0
%type <FnDecl_tok> function_definition function_definition2
%type <DeclList_tok> declaration declaration_list
%type <DeclList_tok> prog
%type <DeclList_tok> translation_unit translation_unit_opt external_declaration
%type <DeclSpec_tok> declaration_specifiers
%type <InitDecl_tok> init_declarator
%type <InitDeclList_tok> init_declarator_list init_declarator_list0
%type <StorageClass_tok> storage_class_specifier
%type <TypeSpecifier_tok> type_specifier
%type <TypeSpecifier_tok> struct_or_union_specifier enum_specifier
%type <StructOrUnion_tok> struct_or_union
%type <TypeQual_tok> type_qualifier type_qualifier_list
%type <StructFieldDeclList_tok> struct_declaration_list struct_declaration
%type <StructFieldDeclListList_tok> struct_declaration_list0
%type <TypeModifierList_tok> pointer
%type <Rgn_tok> rgn
%type <Declarator_tok> declarator direct_declarator struct_declarator
%type <DeclaratorList_tok> struct_declarator_list struct_declarator_list0
%type <AbstractDeclarator_tok> abstract_declarator direct_abstract_declarator
%type <EnumeratorField_tok> enumerator
%type <EnumeratorFieldList_tok> enumerator_list
%type <QualSpecList_tok> specifier_qualifier_list
%type <IdList_tok> identifier_list identifier_list0
%type <ParamDecl_tok> parameter_declaration type_name
%type <ParamDeclList_tok> parameter_list
%type <ParamDeclListBool_tok> parameter_type_list
%type <TypeList_tok> type_name_list type_params_opt
%type <DesignatorList_tok> designation designator_list
%type <Designator_tok> designator
/* start production */
%start prog
%%

prog:
  translation_unit
    { $$ = $!1; parse_result = $1; }

translation_unit:
  external_declaration
    { $$=$!1; }
| external_declaration translation_unit
    { $$=^$(List::imp_append($1,$2)); }
/* Cyc: added using and namespace */
/* NB: using_action calls Lex::enter_using */
| using_action ';' translation_unit
    { $$=^$(&cons(&Decl(Using_d($1,$3),LOC(@1,@3)),null));
      Lex::leave_using();
    }
| using_action '{' translation_unit unusing_action translation_unit_opt
    { $$=^$(&cons(&Decl(Using_d($1,$3),LOC(@1,@4)),$5));
    }
/* NB: namespace_action calls Lex::enter_namespace */
| namespace_action ';' translation_unit
    { $$=^$(&cons(&Decl(Namespace_d($1,$3),LOC(@1,@3)),null));
      Lex::leave_namespace();
    }
| namespace_action '{' translation_unit unnamespace_action translation_unit_opt
    { $$=^$(&cons(&Decl(Namespace_d($1,$3),LOC(@1,@4)),$5));
    }
;

translation_unit_opt:
    /* empty */    { $$=^$(null); }
| translation_unit { $$=$!1; }

external_declaration:
  function_definition
    { let d = new_decl(Fn_d($1),LOC(@1,@1));
      $$=^$(&cons(d,null)); }
| declaration
    { $$=$!1; }
;

function_definition:
  declarator compound_statement
    { $$=^$(make_function(null,$1,null,$2,LOC(@1,@2))); }
| declaration_specifiers declarator compound_statement
    { $$=^$(make_function(&Opt($1),$2,null,$3,LOC(@1,@3))); }
| declarator declaration_list compound_statement
    { $$=^$(make_function(null,$1,$2,$3,LOC(@1,@3))); }
| declaration_specifiers declarator declaration_list compound_statement
    { $$=^$(make_function(&Opt($1),$2,$3,$4,LOC(@1,@4))); }
;

/* Used for nested functions; the optional declaration_specifiers
   would cause parser conflicts */
function_definition2:
  declaration_specifiers declarator compound_statement
    { $$=^$(make_function(&Opt($1),$2,null,$3,LOC(@1,@3))); }
| declaration_specifiers declarator declaration_list compound_statement
    { $$=^$(make_function(&Opt($1),$2,$3,$4,LOC(@1,@4))); }
;

using_action:
  USING qual_opt_identifier { Lex::enter_using($2); $$=$!2; }
;

unusing_action:
  '}' { Lex::leave_using(); }
;

namespace_action:
  NAMESPACE IDENTIFIER { Lex::enter_namespace($2); $$=$!2; }
;

unnamespace_action:
  '}' { Lex::leave_namespace(); }
;

/***************************** DECLARATIONS *****************************/
declaration:
  declaration_specifiers ';'
    { $$=^$(make_declarations(&Opt($1),null,LOC(@1,@1))); }
| declaration_specifiers init_declarator_list ';'
    { $$=^$(make_declarations(&Opt($1),$2,LOC(@1,@2))); }
/* Cyc:  let declaration */
| LET pattern '=' expression ';'
    { $$=^$(&cons(let_decl($2,null,$4,LOC(@1,@5)),null)); }
;

declaration_list:
  declaration
    { $$=$!1; }
| declaration_list declaration
    { $$=^$(List::imp_append($1,$2)); }
;

declaration_specifiers:
  storage_class_specifier
    { $$=^$(&Declaration_spec(&Opt($1),empty_tqual(),null,false)); }
| storage_class_specifier declaration_specifiers
    { if ($2->sc != null)
        warn("Only one storage class is allowed in a declaration",LOC(@1,@2));
      $$=^$(&Declaration_spec(&Opt($1),$2->tq,$2->type_specs,false)); }
| type_specifier
    { $$=^$(&Declaration_spec(null,empty_tqual(),
			      &cons($1,null),false)); }
| type_specifier declaration_specifiers
    { $$=^$(&Declaration_spec($2->sc,$2->tq,&cons($1,$2->type_specs),false)); }
| type_qualifier
    { $$=^$(&Declaration_spec(null,$1,null,false)); }
| type_qualifier declaration_specifiers
    { $$=^$(&Declaration_spec($2->sc,combine_tqual($1,$2->tq),
			      $2->type_specs, false)); }
| INLINE
    { $$=^$(&Declaration_spec(null,empty_tqual(),null,true)); }
| INLINE declaration_specifiers
    { $$=^$(&Declaration_spec($2->sc,$2->tq,$2->type_specs,true)); }
;

storage_class_specifier:
  AUTO
    { $$=^$(Auto_sc); }
| REGISTER
    { $$=^$(Register_sc); }
| STATIC
    { $$=^$(Static_sc); }
| EXTERN
    { $$=^$(Extern_sc); }
| TYPEDEF
    { $$=^$(Typedef_sc); }
/* Cyc:  exception and abstract specifiers */
| ABSTRACT
    { $$=^$(Abstract_sc); }
;

/***************************** TYPES *****************************/
/* we could be parsing a type or a type declaration (e.g., struct)
 * so we return a type_specifier value -- either a type, a type
 * qualifier, an integral type qualifier (signed, long, etc.)
 * or a declaration.
 */
type_specifier:
  VOID
    { $$=^$(type_spec(VoidType,LOC(@1,@1))); }
| '_' 
    { $$=^$(type_spec(new_evar(MemKind),LOC(@1,@1))); }
| CHAR
    { $$=^$(type_spec(uchar_t,LOC(@1,@1))); }
| SHORT
    { $$=^$(Short_spec(LOC(@1,@1),false)); }
| INT
    { $$=^$(type_spec(sint_t,LOC(@1,@1))); }
| LONG
    { $$=^$(Long_spec(LOC(@1,@1),false)); }
| FLOAT
    { $$=^$(type_spec(float_t,LOC(@1,@1))); }
| DOUBLE
    { $$=^$(type_spec(double_t,LOC(@1,@1))); }
| SIGNED
    { $$=^$(Signed_spec(LOC(@1,@1))); }
| UNSIGNED
    { $$=^$(Unsigned_spec(LOC(@1,@1))); }
| struct_or_union_specifier
    { $$=$!1; }
| enum_specifier
    { $$=$!1; }
/* Cyc: added optional type parameters to typedef'd names */
| QUAL_TYPEDEF_NAME type_params_opt
    { $$=^$(type_spec(TypedefType($1,$2,null),LOC(@1,@2))); }
/* Cyc: everything below here is an addition */
| TYPE_VAR
    { $$=^$(type_spec(VarType($1),LOC(@1,@1))); }
| BOXED_CHAR
    { $$=^$(type_spec(IntType(Unsigned, B1, Boxed),LOC(@1,@1))); }
| BOXED_SHORT
    { $$=^$(Short_spec(LOC(@1,@1),true)); }
| BOXED_INT
    { $$=^$(type_spec(IntType(Signed, B4, Boxed),LOC(@1,@1))); }
| BOXED_LONG
    { $$=^$(Long_spec(LOC(@1,@1),true)); }
| BOXED_FLOAT
    { $$=^$(type_spec(FloatType(Boxed),LOC(@1,@1))); }
| BOXED_DOUBLE
    { $$=^$(type_spec(DoubleType(Boxed),LOC(@1,@1))); }
| '$' '(' parameter_list ')'
    { $$=^$(type_spec(TupleType(List::map_c(get_tqual_typ,
					    LOC(@3,@3),List::imp_rev($3))),
                      LOC(@1,@4))); }
;

type_qualifier:
  CONST
    { $$=^$(&Tqual(true,false,false)); }
| VOLATILE
    { $$=^$(&Tqual(false,true,false)); }
| RESTRICT
    { $$=^$(&Tqual(false,false,true)); }

/* parsing of struct and union specifiers. */
struct_or_union_specifier:
  struct_or_union '{' struct_declaration_list '}'
    { decl d;
      switch ($1) {
      case Struct_su:
        d = struct_decl(Public,null,null,&Opt($3),LOC(@1,@4));
	break;
      case Union_su:
        unimp2("unions",LOC(@1,@4));
        d = new_decl(Union_d,LOC(@1,@4));
	break;
      }
      $$=^$(Decl_spec(d));
      unimp2("anonymous structs/unions",LOC(@1,@4));
    }
/* Cyc:  type_params_opt are added */
| struct_or_union qual_opt_identifier type_params_opt
  '{' struct_declaration_list '}'
    { let ts = List::map_c(typ2tvar,LOC(@3,@3),$3);
      decl d;
      switch ($1) {
      case Struct_su:
        d = struct_decl(Public,&Opt($2),ts,&Opt($5),LOC(@1,@6));
	break;
      case Union_su:
        unimp2("unions",LOC(@1,@6));
        d = new_decl(Union_d,LOC(@1,@6));
	break;
      }
      $$=^$(Decl_spec(d));
    }
// Hack to allow struct/union names and typedef names to overlap
| struct_or_union QUAL_TYPEDEF_NAME type_params_opt
  '{' struct_declaration_list '}'
    { let ts = List::map_c(typ2tvar,LOC(@3,@3),$3);
      decl d;
      switch ($1) {
      case Struct_su:
        d = struct_decl(Public,&Opt($2),ts,&Opt($5),LOC(@1,@6));
	break;
      case Union_su:
        unimp2("unions",LOC(@1,@6));
        d = new_decl(Union_d,LOC(@1,@6));
	break;
      }
      $$=^$(Decl_spec(d));
    }
/* Cyc:  type_params_opt are added */
| struct_or_union qual_opt_identifier type_params_opt
    { switch ($1) {
      case Struct_su:
        $$=^$(type_spec(StructType(&Opt($2),$3,null),LOC(@1,@3)));
	break;
      case Union_su:
        unimp2("unions",LOC(@1,@3));
        $$=^$(Decl_spec(new_decl(Union_d,LOC(@1,@3))));
	break;
      }
    }
// Hack to allow struct/union names and typedef names to overlap
| struct_or_union QUAL_TYPEDEF_NAME type_params_opt
    { switch ($1) {
      case Struct_su:
        $$=^$(type_spec(StructType(&Opt($2),$3,null),LOC(@1,@3)));
	break;
      case Union_su:
        unimp2("unions",LOC(@1,@3));
        $$=^$(Decl_spec(new_decl(Union_d,LOC(@1,@3))));
	break;
      }
    }
;

type_params_opt:
  /* empty */
    { $$=^$(null); }
| '<' type_name_list '>'
    { $$=^$(List::imp_rev($2)); }
| '<' type_name_list RIGHT_OP
    { /* RIGHT_OP is >>, we've seen one char too much, step back */
      lbuf->v->lex_curr_pos -= 1;
      $$=^$(List::imp_rev($2)); }
;

struct_or_union:
  STRUCT
    { $$=^$(Struct_su); }
| UNION
    { $$=^$(Union_su); }
;

struct_declaration_list:
  struct_declaration_list0
    { $$=^$(List::flatten(List::imp_rev($1))); }
;

/* NB: returns list in reverse order */
struct_declaration_list0:
  struct_declaration
    { $$=^$(&cons($1,null)); }
| struct_declaration_list0 struct_declaration
    { $$=^$(&cons($2,$1)); }
;

init_declarator_list:
  init_declarator_list0
    { $$=^$(List::imp_rev($1)); }
;

/* NB: returns list in reverse order */
init_declarator_list0:
  init_declarator
    { $$=^$(&cons($1,null)); }
| init_declarator_list0 ',' init_declarator
    { $$=^$(&cons($3,$1)); }
;

init_declarator:
  declarator
    { $$=^$(&$($1,null)); }
| declarator '=' initializer
    { $$=^$(&$($1,(Opt_t<exp>)&Opt($3))); }  // Fix: cast needed!
;

struct_declaration:
  specifier_qualifier_list struct_declarator_list ';'
    {
      /* when we collapse the specifier_qualifier_list and
       * struct_declarator_list, we get a list of (1) optional id,
       * (2) type, and (3) in addition any nested struct, union,
       * or enum declarations.  For now, we warn about the nested
       * declarations.  We must check that each id is actually present
       * and then convert this to a list of struct fields: (1) id,
       * (2) tqual, (3) type. */
      tqual tq = $1[0];
      list<type_specifier_t> tss = $1[1];
      let ts_info = collapse_type_specifiers(tss,LOC(@1,@1));
      if (ts_info[1] != null)
        warn("struct declaration contains nested type declaration",LOC(@1,@1));
      let t = ts_info[0];
      let info = apply_tmss(tq,t,$2);
      $$=^$(List::map_c(make_struct_field,LOC(@1,@2),info));
    }
;

specifier_qualifier_list:
  type_specifier
    // Fix: casts needed
    { $$=^$(&$(empty_tqual(),(list<type_specifier_t>)(&cons($1,null)))); }
| type_specifier specifier_qualifier_list
    { $$=^$(&$($2[0],(list<type_specifier_t>)(&cons($1,$2[1])))); }
| type_qualifier
    { $$=^$(&$($1,null)); }
| type_qualifier specifier_qualifier_list
    { $$=^$(&$(combine_tqual($1,$2[0]),$2[1])); }

struct_declarator_list:
  struct_declarator_list0
    { $$=^$(List::imp_rev($1)); }
;

/* NB: returns list in reverse order */
struct_declarator_list0:
  struct_declarator
    { $$=^$(&cons($1,null)); }
| struct_declarator_list0 ',' struct_declarator
    { $$=^$(&cons($3,$1)); }
;

struct_declarator:
  declarator
    { $$=$!1; }
| ':' constant_expression
    { /* FIX: TEMPORARY TO TEST PARSING */
      unimp2("bit fields",LOC(@1,@2));
      // Fix: cast needed
      $$=^$(&Declarator(&$(null,(string)""),null)); }
| declarator ':' constant_expression
    { unimp2("bit fields",LOC(@1,@2));
      $$=$!1; }
;

enum_specifier:
  ENUM '{' enumerator_list '}'
    { decl d  = enum_decl(Public,null,null,&Opt($3),LOC(@1,@4));
      $$ = ^$(Decl_spec(d));
      unimp2("anonymous enums",LOC(@1,@4));
    }
/* Cyc: added type params */
| ENUM qual_opt_identifier type_params_opt '{' enumerator_list '}'
    { let ts = List::map_c(typ2tvar,LOC(@3,@3),$3);
      decl d  = enum_decl(Public,&Opt($2),ts,&Opt($5),LOC(@1,@6));
      $$ = ^$(Decl_spec(d));
    }
| ENUM qual_opt_identifier type_params_opt
    { $$=^$(type_spec(EnumType(&Opt($2),$3,null),LOC(@1,@3)));
    }
| XENUM qual_opt_identifier '{' enumerator_list '}'
    { decl d  = xenum_decl(Public,$2,$4,LOC(@1,@5));
      $$ = ^$(Decl_spec(d));
    }
| XENUM qual_opt_identifier
    { $$=^$(type_spec(XenumType($2,null),LOC(@1,@2)));
    }
;

enumerator_list:
  enumerator
    { $$=^$(&cons($1,null)); }
| enumerator ';'
    { $$=^$(&cons($1,null)); }
| enumerator ',' enumerator_list
    { $$=^$(&cons($1,$3)); }
| enumerator ';' enumerator_list
    { $$=^$(&cons($1,$3)); }
;

enumerator:
  qual_opt_identifier
    { $$=^$(&Enumfield($1,null,null,null,LOC(@1,@1))); }
| qual_opt_identifier '=' constant_expression
    { $$=^$(&Enumfield($1,&Opt($3),null,null,LOC(@1,@3))); }
/* Cyc: value-carrying enumerators */
| qual_opt_identifier type_params_opt '(' parameter_list ')'
    { let typs = List::map_c(get_tqual_typ,LOC(@4,@4),List::imp_rev($4));
      let tvs = List::map_c(typ2tvar,LOC(@2,@2),$2);
      $$=^$(&Enumfield($1,null,tvs,typs,LOC(@1,@5))); }

declarator:
  direct_declarator
    { $$=$!1; }
| pointer direct_declarator
    { $$=^$(&Declarator($2->id,List::imp_append($1,$2->tms))); }
;

direct_declarator:
  qual_opt_identifier
    { $$=^$(&Declarator($1,null)); }
| '(' declarator ')'
    { $$=$!2; }
| direct_declarator '[' ']'
    { $$=^$(&Declarator($1->id,&cons(Carray_mod,$1->tms))); }
| direct_declarator '[' assignment_expression ']'
    { $$=^$(&Declarator($1->id,&cons(ConstArray_mod($3),$1->tms))); }
| direct_declarator '(' parameter_type_list ')'
    { let ParamDeclListBool_tok(lis,b) = $!3;
      $$=^$(&Declarator($1->id,&cons(Function_mod(WithTypes(lis,b)),
				     $1->tms)));
    }
| direct_declarator '(' ')'
    { $$=^$(&Declarator($1->id,&cons(Function_mod(WithTypes(null,false)),
                        $1->tms)));
    }
| direct_declarator '(' identifier_list ')'
    { $$=^$(&Declarator($1->id,&cons(Function_mod(NoTypes($3,LOC(@1,@4))),$1->tms))); }
/* Cyc: new kind of array */
| direct_declarator '[' '?' ']'
    { $$=^$(&Declarator($1->id,&cons(Array_mod,$1->tms))); }
/* Cyc: added type parameters */
| direct_declarator '<' type_name_list '>'
    { let ts = List::map_c(typ2tvar,LOC(@2,@4),List::imp_rev($3));
      $$=^$(&Declarator($1->id,&cons(TypeParams_mod(ts,LOC(@1,@4)),$1->tms)));
    }
| direct_declarator '<' type_name_list RIGHT_OP
    { /* RIGHT_OP is >>, we've seen one char too much, step back */
      lbuf->v->lex_curr_pos -= 1;
      let ts = List::map_c(typ2tvar,LOC(@2,@4),List::imp_rev($3));
      $$=^$(&Declarator($1->id,&cons(TypeParams_mod(ts,LOC(@1,@4)),$1->tms)));
    }
;

/* CYC: region annotations allowed */
pointer:
  '*' rgn
    { $$=^$(&cons(Pointer_mod(true,$2,empty_tqual()),null)); }
| '*' rgn type_qualifier_list
    { $$=^$(&cons(Pointer_mod(true,$2,$3),null)); }
| '*' rgn pointer
    { $$=^$(&cons(Pointer_mod(true,$2,empty_tqual()),$3)); }
| '*' rgn type_qualifier_list pointer
    { $$=^$(&cons(Pointer_mod(true,$2,$3),$4)); }
/* CYC: pointers that cannot be null */
| '@' rgn
    { $$=^$(&cons(Pointer_mod(false,$2,empty_tqual()),null)); }
| '@' rgn type_qualifier_list
    { $$=^$(&cons(Pointer_mod(false,$2,$3),null)); }
| '@' rgn pointer
    { $$=^$(&cons(Pointer_mod(false,$2,empty_tqual()),$3)); }
| '@' rgn type_qualifier_list pointer
    { $$=^$(&cons(Pointer_mod(false,$2,$3),$4)); }
;

rgn:
    /* empty */
  { $$ = ^$(HeapRgn); }
| TYPE_VAR
  { $$ = ^$(VarRgn($1)); }

type_qualifier_list:
  type_qualifier
    { $$ = $!1; }
| type_qualifier_list type_qualifier
    { $$ = ^$(combine_tqual($1,$2)); }
;

parameter_type_list:
  parameter_list
    { $$=^$(List::imp_rev($1),false); }
| parameter_list ',' ELLIPSIS
    { $$=^$(List::imp_rev($1),true); }
;

/* NB: returns list in reverse order */
parameter_list:
  parameter_declaration
    { $$=^$(&cons($1,null)); }
| parameter_list ',' parameter_declaration
    { $$=^$(&cons($3,$1)); }
;

/* TODO: differs from grammar in K&R */
parameter_declaration:
  specifier_qualifier_list declarator
    { let tss = $1[1];
      let ts_info = collapse_type_specifiers(tss,LOC(@1,@1));
      if (ts_info[1] != null)
        warn("parameter contains nested type declaration",LOC(@1,@1));
      let t = ts_info[0];
      let tq = $1[0];
      let tms = $2->tms;
      let t_info = apply_tms(tq,t,tms);
      if (t_info[2] != null)
        err("parameter with bad type params",LOC(@2,@2));
      let q = $2->id;
      if (q[0] != null)
        err("parameter cannot be qualified with a module name",LOC(@1,@1));
      let idopt = (Opt_t<var>)&Opt(q[1]);
      $$=^$(&$(idopt,t_info[0],t_info[1]));
    }
| specifier_qualifier_list
    { let tss = $1[1];
      let ts_info = collapse_type_specifiers(tss,LOC(@1,@1));
      if (ts_info[1] != null)
        warn("nested type declaration, ignoring",LOC(@1,@1));
      let t = ts_info[0];
      let tq = $1[0];
      $$=^$(&$(null,tq,t));
    }
| specifier_qualifier_list abstract_declarator
    { let tss = $1[1];
      let ts_info = collapse_type_specifiers(tss,LOC(@1,@1));
      if (ts_info[1] != null)
        warn("nested type declaration, ignoring",LOC(@1,@1));
      let t = ts_info[0];
      let tq = $1[0];
      let tms = $2->tms;
      let t_info = apply_tms(tq,t,tms);
      if (t_info[2] != null)
        // Ex: int (@)<`a>
        warn("bad type params, ignoring",LOC(@2,@2));
      $$=^$(&$(null,t_info[0],t_info[1]));
    }
/*
| type_name
    { $$=$!1; }
*/
;

identifier_list:
  identifier_list0
    { $$=^$(List::imp_rev($1)); }
;

/* NB: returns list in reverse order */
identifier_list0:
  IDENTIFIER
    { $$=^$(&cons($1,null)); }
| identifier_list0 ',' IDENTIFIER
    { $$=^$(&cons($3,$1)); }
;

initializer:
  assignment_expression
    { $$=$!1; }
| '{' '}'
    { $$=^$(new_exp(UnresolvedMem_e(null,null),LOC(@1,@2))); }
| '{' initializer_list '}'
    { $$=^$(new_exp(UnresolvedMem_e(null,List::imp_rev($2)),LOC(@1,@3))); }
| '{' initializer_list ',' '}'
    { $$=^$(new_exp(UnresolvedMem_e(null,List::imp_rev($2)),LOC(@1,@4))); }
;

/* NB: returns list in reverse order */
initializer_list:
  initializer
    { $$=^$(&cons(&$(null,$1),null)); }
| designation initializer
    { $$=^$(&cons(&$($1,$2),null)); }
| initializer_list ',' initializer
    { $$=^$(&cons(&$(null,$3),$1)); }
| initializer_list ',' designation initializer
    { $$=^$(&cons(&$($3,$4),$1)); }
;

designation:
  designator_list '='
    {$$=^$(List::imp_rev($1));}
;

/* NB: returns list in reverse order */
designator_list:
  designator
    { $$=^$(&cons($1,null)); }
| designator_list designator
    { $$=^$(&cons($2,$1)); }
;

designator:
  '[' constant_expression ']'
    {$$ = ^$(ArrayElement($2));}
| '.' IDENTIFIER
    {$$ = ^$(FieldName($2));}
;

type_name:
  specifier_qualifier_list
    { let tss = $1[1];
      let ts_info = collapse_type_specifiers(tss,LOC(@1,@1));
      if (ts_info[1] != null)
        warn("nested type declaration, ignoring",LOC(@1,@1));
      let t = ts_info[0];
      let tq = $1[0];
      $$=^$(&$(null,tq,t));
    }
| specifier_qualifier_list abstract_declarator
    { let tss = $1[1];
      let ts_info = collapse_type_specifiers(tss,LOC(@1,@1));
      if (ts_info[1] != null)
        warn("nested type declaration, ignoring",LOC(@1,@1));
      let t = ts_info[0];
      let tq = $1[0];
      let tms = $2->tms;
      let t_info = apply_tms(tq,t,tms);
      if (t_info[2] != null)
        // Ex: int (@)<`a>
        warn("bad type params, ignoring",LOC(@2,@2));
      $$=^$(&$(null,t_info[0],t_info[1]));
    }
;

/* Cyc: new */
/* NB: returns list in reverse order */
type_name_list:
  type_name
    { let x = $1;
      $$=^$(&cons(x[2],null)); }
| type_name_list ',' type_name
    { let x = $3;
      $$=^$(&cons(x[2],$1)); }
;

abstract_declarator:
  pointer
    { $$=^$(&Abstractdeclarator($1)); }
| direct_abstract_declarator
    { $$=$!1; }
| pointer direct_abstract_declarator
    { $$=^$(&Abstractdeclarator(List::imp_append($1,$2->tms))); }
;

direct_abstract_declarator:
  '(' abstract_declarator ')'
    { $$=$!2; }
| '[' ']'
    { $$=^$(&Abstractdeclarator(&cons(Carray_mod,null))); }
| direct_abstract_declarator '[' ']'
    { $$=^$(&Abstractdeclarator(&cons(Carray_mod,$1->tms))); }
| '[' assignment_expression ']'
    { $$=^$(&Abstractdeclarator(&cons(ConstArray_mod($2),null))); }
| direct_abstract_declarator '[' assignment_expression ']'
    { $$=^$(&Abstractdeclarator(&cons(ConstArray_mod($3),$1->tms))); }
| '(' ')'
    { $$=^$(&Abstractdeclarator(&cons(Function_mod(WithTypes(null,false)),
				      null)));
    }
| '(' parameter_type_list ')'
    { let ParamDeclListBool_tok(lis,b) = $!2;
      $$=^$(&Abstractdeclarator(&cons(Function_mod(WithTypes(lis,b)),null)));
    }
| direct_abstract_declarator '(' ')'
    { $$=^$(&Abstractdeclarator(&cons(Function_mod(WithTypes(null,false)),
				      $1->tms)));
    }
| direct_abstract_declarator '(' parameter_type_list ')'
    { let ParamDeclListBool_tok(lis,b) = $!3;
      $$=^$(&Abstractdeclarator(&cons(Function_mod(WithTypes(lis,b)),
				      $1->tms)));
    }
/* Cyc: new */
| '[' '?' ']'
    { $$=^$(&Abstractdeclarator(&cons(Array_mod,null))); }
| direct_abstract_declarator '[' '?' ']'
    { $$=^$(&Abstractdeclarator(&cons(Array_mod,$1->tms))); }
/* Cyc: new */
| direct_abstract_declarator '<' type_name_list '>'
    { let ts = List::map_c(typ2tvar,LOC(@2,@4),List::imp_rev($3));
      $$=^$(&Abstractdeclarator(&cons(TypeParams_mod(ts,LOC(@2,@4)),$1->tms)));
    }
| direct_abstract_declarator '<' type_name_list RIGHT_OP
    { /* RIGHT_OP is >>, we've seen one char too much, step back */
      lbuf->v->lex_curr_pos -= 1;
      let ts = List::map_c(typ2tvar,LOC(@2,@4),List::imp_rev($3));
      $$=^$(&Abstractdeclarator(&cons(TypeParams_mod(ts,LOC(@2,@4)),$1->tms)));
    }
;

/***************************** STATEMENTS *****************************/
statement:
  labeled_statement
    { $$=$!1; }
| expression_statement
    { $$=$!1; }
| compound_statement
    { $$=$!1; }
| selection_statement
    { $$=$!1; }
| iteration_statement
    { $$=$!1; }
| jump_statement
    { $$=$!1; }
/* Cyc: cut and splice */
| CUT statement
    { $$=^$(new_stmt(Cut_s($2),LOC(@1,@2))); }
| SPLICE statement
    { $$=^$(new_stmt(Splice_s($2),LOC(@1,@2))); }
;

/* Cyc: Unlike C, we do not treat case and default statements as
   labeled */
labeled_statement:
  IDENTIFIER ':' statement
    { $$=^$(new_stmt(Label_s($1,$3),LOC(@1,@3))); }
;

expression_statement:
  ';'
    { $$=^$(skip_stmt(LOC(@1,@1))); }
| expression ';'
    { $$=^$(exp_stmt($1,LOC(@1,@2))); }
;

compound_statement:
  '{' '}'
     { $$=^$(skip_stmt(LOC(@1,@2))); }
| '{' block_item_list '}'
     { $$=$!2; }
;

block_item_list:
  block_item
    { switch ($1) {
      case TopDecls_bl(ds):
        $$=^$(flatten_declarations(ds,skip_stmt(LOC(@1,@1))));
	break;
      case FnDecl_bl(fd):
        $$=^$(flatten_decl(new_decl(Fn_d(fd),LOC(@1,@1)),
			   skip_stmt(LOC(@1,@1))));
	break;
      case Stmt_bl(s):
        $$=^$(s);
	break;
      }
    }
| block_item block_item_list
    { switch ($1) {
      case TopDecls_bl(ds):
        $$=^$(flatten_declarations(ds,$2));
	break;
      case FnDecl_bl(fd):
        $$=^$(flatten_decl(new_decl(Fn_d(fd),LOC(@1,@1)),$2));
	break;
      case Stmt_bl(s):
        $$=^$(seq_stmt(s,$2,LOC(@1,@2)));
	break;
      }
    }
;

block_item:
  declaration
    { $$=^$(TopDecls_bl($1)); }
| statement
    { $$=^$(Stmt_bl($1)); }
/* Cyc: nested function definitions.
   The initial (return) type is required,
   to avoid parser conflicts. */
| function_definition2
    { $$=^$(FnDecl_bl($1)); }

/* This has the standard shift-reduce conflict which is properly
   resolved. */
selection_statement:
  IF '(' expression ')' statement
    { $$=^$(ifthenelse_stmt($3,$5,skip_stmt(DUMMYLOC),LOC(@1,@5))); }
| IF '(' expression ')' statement ELSE statement
    { $$=^$(ifthenelse_stmt($3,$5,$7,LOC(@1,@7))); }
/* Cyc: the body of the switch statement cannot be an arbitrary
 * statement; it must be a list of switch_clauses */
| SWITCH '(' expression ')' '{' switch_clauses '}'
    { $$=^$(new_stmt(Switch_s($3,$6),LOC(@1,@7))); }
| TRY statement CATCH '{' switch_clauses '}'
    { $$=^$(new_stmt(TryCatch_s($2,$5),LOC(@1,@6))); }
;

/* Cyc: unlike C, we only allow default or case statements within
 * switches.  Also unlike C, we support a more general form of pattern
 * matching within cases. */
switch_clauses:
  /* empty */
    { $$=^$(null); }
| DEFAULT ':' block_item_list
    { $$=^$(&cons(&Switch_clause(new_pat(Wild_p,LOC(@1,@1)),null,
                                 null,$3,LOC(@1,@3)),
                  null));}
| CASE pattern ':' switch_clauses
    { $$=^$(&cons(&Switch_clause($2,null,null,skip_stmt(LOC(@3,@3)),
                                 LOC(@1,@4)),$4)); }
| CASE pattern ':' block_item_list switch_clauses
    { $$=^$(&cons(&Switch_clause($2,null,null,$4,LOC(@1,@4)),$5)); }
| CASE pattern AND_OP expression ':' switch_clauses
    { $$=^$(&cons(&Switch_clause($2,null,&Opt($4),skip_stmt(LOC(@5,@5)),
                                 LOC(@1,@6)),$6)); }
| CASE pattern AND_OP expression ':' block_item_list switch_clauses
    { $$=^$(&cons(&Switch_clause($2,null,&Opt($4),$6,LOC(@1,@7)),$7)); }
;

iteration_statement:
  WHILE '(' expression ')' statement
    { $$=^$(while_stmt($3,$5,LOC(@1,@5))); }
| DO statement WHILE '(' expression ')' ';'
    { $$=^$(do_stmt($2,$5,LOC(@1,@7))); }
| FOR '(' ';' ';' ')' statement
    { $$=^$(for_stmt(signed_int_exp(0,DUMMYLOC),bool_exp(true,DUMMYLOC),
                     signed_int_exp(0,DUMMYLOC),$6,LOC(@1,@6))); }
| FOR '(' ';' ';' expression ')' statement
    { $$=^$(for_stmt(signed_int_exp(0,DUMMYLOC),bool_exp(true,DUMMYLOC),
                             $5,$7,LOC(@1,@7))); }
| FOR '(' ';' expression ';' ')' statement
    { $$=^$(for_stmt(signed_int_exp(0,DUMMYLOC),$4,
                     signed_int_exp(0,DUMMYLOC),$7,LOC(@1,@7)));}
| FOR '(' ';' expression ';' expression ')' statement
    { $$=^$(for_stmt(signed_int_exp(0,DUMMYLOC),$4,$6,$8,LOC(@1,@7))); }
| FOR '(' expression ';' ';' ')' statement
    { $$=^$(for_stmt($3,bool_exp(true,DUMMYLOC),
                     signed_int_exp(0,DUMMYLOC),$7,LOC(@1,@7))); }
| FOR '(' expression ';' ';' expression ')' statement
    { $$=^$(for_stmt($3,bool_exp(true,DUMMYLOC),$6,$8,LOC(@1,@8))); }
| FOR '(' expression ';' expression ';' ')' statement
    { $$=^$(for_stmt($3,$5,signed_int_exp(0,DUMMYLOC),$8,LOC(@1,@8))); }
| FOR '(' expression ';' expression ';' expression ')' statement
    { $$=^$(for_stmt($3,$5,$7,$9,LOC(@1,@9))); }
| FOR '(' declaration ';' ')' statement
    { let decls = $3;
      let s = for_stmt(signed_int_exp(0,DUMMYLOC),bool_exp(true,DUMMYLOC),
		       signed_int_exp(0,DUMMYLOC),$6,LOC(@1,@6));
      $$=^$(flatten_declarations(decls,s));
    }
| FOR '(' declaration expression ';' ')' statement
    { let decls = $3;
      let s = for_stmt(signed_int_exp(0,DUMMYLOC),$4,
		       signed_int_exp(0,DUMMYLOC),$7,LOC(@1,@7));
      $$=^$(flatten_declarations(decls,s));
    }
| FOR '(' declaration ';' expression ')' statement
    { let decls = $3;
      let s = for_stmt(signed_int_exp(0,DUMMYLOC),bool_exp(true,DUMMYLOC),
		       $5,$7,LOC(@1,@7));
      $$=^$(flatten_declarations(decls,s));
    }
| FOR '(' declaration expression ';' expression ')' statement
    { let decls = $3;
      let s = for_stmt(signed_int_exp(0,DUMMYLOC),$4,$6,$8,LOC(@1,@8));
      $$=^$(flatten_declarations(decls,s));
    }
;

jump_statement:
  GOTO IDENTIFIER ';'
    { $$=^$(goto_stmt($2,LOC(@1,@2))); }
| CONTINUE ';'
    { $$=^$(continue_stmt(LOC(@1,@1)));}
| BREAK ';'
    { $$=^$(break_stmt(LOC(@1,@1)));}
| RETURN ';'
    { $$=^$(return_stmt(null,LOC(@1,@1)));}
| RETURN expression ';'
    { $$=^$(return_stmt(&Opt($2),LOC(@1,@2)));}
/* Cyc:  explicit fallthru for switches */
| FALLTHRU ';'
    { $$=^$(fallthru_stmt(null,LOC(@1,@1)));}
| FALLTHRU '(' ')' ';'
    { $$=^$(fallthru_stmt(null,LOC(@1,@1)));}
| FALLTHRU '(' argument_expression_list ')' ';'
    { $$=^$(fallthru_stmt($3,LOC(@1,@1)));}
;

/***************************** PATTERNS *****************************/
/* Cyc:  patterns */
pattern:
  '_'
    {$$=^$(new_pat(Wild_p,LOC(@1,@1)));}
| '(' pattern ')'
    { $$=$!2; }
| INTEGER_CONSTANT
    { $$=^$(new_pat(Int_p($1[0],$1[1]),LOC(@1,@1))); }
| '-' INTEGER_CONSTANT
    { $$=^$(new_pat(Int_p(Signed,-($2[1])),LOC(@1,@2)));
    }
| FLOATING_CONSTANT
    {$$=^$(new_pat(Float_p($1),LOC(@1,@1)));}
/* TODO: we should allow negated floating constants too */
| CHARACTER_CONSTANT
    {$$=^$(new_pat(Char_p($1),LOC(@1,@1)));}
| NULL
    {$$=^$(new_pat(Null_p,LOC(@1,@1)));}
| qual_opt_identifier
    { $$=^$(new_pat(UnknownId_p($1),LOC(@1,@1))); }
| qual_opt_identifier type_params_opt '(' tuple_pattern_list ')'
    { let tvs = List::map_c(typ2tvar,LOC(@2,@2),$2);
      $$=^$(new_pat(UnknownCall_p($1,tvs,$4),LOC(@1,@5)));
    }
| '$' '(' tuple_pattern_list ')'
    {$$=^$(new_pat(Tuple_p($3),LOC(@1,@4)));}
| qual_opt_identifier type_params_opt '{' '}'
    { let tvs = List::map_c(typ2tvar,LOC(@2,@2),$2);
      $$=^$(new_pat(UnknownFields_p($1,tvs,null),LOC(@1,@4)));
    }
| qual_opt_identifier type_params_opt '{' field_pattern_list '}'
    { let tvs = List::map_c(typ2tvar,LOC(@2,@2),$2);
      $$=^$(new_pat(UnknownFields_p($1,tvs,$4),LOC(@1,@5)));
    }
| '&' pattern
    {$$=^$(new_pat(Pointer_p($2),LOC(@1,@2)));}
| '*' IDENTIFIER
    {$$=^$(new_pat(Reference_p(new_vardecl(&$(null,$2),VoidType,null)),
		   LOC(@1,@2)));}
;

tuple_pattern_list:
  /* empty */
    { $$=^$(null); }
| tuple_pattern_list0
    { $$=^$(List::imp_rev($1)); }
;

/* NB: returns list in reverse order */
tuple_pattern_list0:
  pattern
    {$$=^$(&cons($1,null));}
| tuple_pattern_list0 ',' pattern
    {$$=^$(&cons($3,$1));}
;

field_pattern:
  pattern
    {$$=^$(&$(null,$1));}
| designation pattern
    {$$=^$(&$($1,$2));}

field_pattern_list:
  field_pattern_list0
    {$$=^$(List::imp_rev($1));}
;

field_pattern_list0:
  field_pattern
    { $$=^$(&cons($1,null));}
| field_pattern_list0 ',' field_pattern
    {$$=^$(&cons($3,$1)); }
;
/*
struct_pattern_list:
  struct_pattern_list0
    {$$=^$(List::imp_rev($1));}
;

struct_pattern_list0:
  struct_field_pattern
    {$$=^$(&cons($1,null));}
| struct_pattern_list0 ',' struct_field_pattern
    {$$=^$(&cons($3,$1)); }
;

struct_field_pattern:
  IDENTIFIER
    {$$=^$(^($1,new_pat(^raw_pat.Var($1),LOC(@1,@1))));}
| IDENTIFIER '=' pattern
    {$$=^$(^($1,$3));}
;
*/

/***************************** EXPRESSIONS *****************************/
expression:
  assignment_expression
    { $$=$!1; }
| expression ',' assignment_expression
    { $$=^$(seq_exp($1,$3,LOC(@1,@3))); }
;

assignment_expression:
  conditional_expression
    { $$=$!1; }
| unary_expression assignment_operator assignment_expression
    { $$=^$(assignop_exp($1,$2,$3,LOC(@1,@3))); }
;

assignment_operator:
  '='
    { $$=^$(null); }
| MUL_ASSIGN
    { $$=^$(&Opt(Times)); }
| DIV_ASSIGN
    { $$=^$(&Opt(Div)); }
| MOD_ASSIGN
    { $$=^$(&Opt(Mod)); }
| ADD_ASSIGN
    { $$=^$(&Opt(Plus)); }
| SUB_ASSIGN
    { $$=^$(&Opt(Minus)); }
| LEFT_ASSIGN
    { $$=^$(&Opt(Bitlshift)); }
| RIGHT_ASSIGN
    { $$=^$(&Opt(Bitlrshift)); }
| AND_ASSIGN
    { $$=^$(&Opt(Bitand)); }
| XOR_ASSIGN
    { $$=^$(&Opt(Bitxor)); }
| OR_ASSIGN
    { $$=^$(&Opt(Bitor)); }
;

conditional_expression:
  logical_or_expression
    { $$=$!1; }
| logical_or_expression '?' expression ':' conditional_expression
    { $$=^$(conditional_exp($1,$3,$5,LOC(@1,@5))); }
;

constant_expression:
  conditional_expression
    { $$=$!1; }
;

logical_or_expression:
  logical_and_expression
    { $$=$!1; }
| logical_or_expression OR_OP logical_and_expression
    { $$=^$(conditional_exp($1,true_exp(DUMMYLOC),$3,LOC(@1,@3))); }
;

logical_and_expression:
  inclusive_or_expression
    { $$=$!1; }
| logical_and_expression AND_OP inclusive_or_expression
    { $$=^$(conditional_exp($1,$3,false_exp(DUMMYLOC),LOC(@1,@3))); }
;

inclusive_or_expression:
  exclusive_or_expression
    { $$=$!1; }
| inclusive_or_expression '|' exclusive_or_expression
    { $$=^$(prim2_exp(Bitor,$1,$3,LOC(@1,@3))); }
;

exclusive_or_expression:
  and_expression
    { $$=$!1; }
| exclusive_or_expression '^' and_expression
    { $$=^$(prim2_exp(Bitxor,$1,$3,LOC(@1,@3))); }
;

and_expression:
  equality_expression
    { $$=$!1; }
| and_expression '&' equality_expression
    { $$=^$(prim2_exp(Bitand,$1,$3,LOC(@1,@3))); }
;

equality_expression:
  relational_expression
    { $$=$!1; }
| equality_expression EQ_OP relational_expression
    { $$=^$(prim2_exp(Eq,$1,$3,LOC(@1,@3))); }
| equality_expression NE_OP relational_expression
    { $$=^$(prim2_exp(Neq,$1,$3,LOC(@1,@3))); }
;

relational_expression:
  shift_expression
    { $$=$!1; }
| relational_expression '<' shift_expression
    { $$=^$(prim2_exp(Lt,$1,$3,LOC(@1,@3))); }
| relational_expression '>' shift_expression
    { $$=^$(prim2_exp(Gt,$1,$3,LOC(@1,@3))); }
| relational_expression LE_OP shift_expression
    { $$=^$(prim2_exp(Lte,$1,$3,LOC(@1,@3))); }
| relational_expression GE_OP shift_expression
    { $$=^$(prim2_exp(Gte,$1,$3,LOC(@1,@3))); }
;

shift_expression:
  additive_expression
    { $$=$!1; }
| shift_expression LEFT_OP additive_expression
    { $$=^$(prim2_exp(Bitlshift,$1,$3,LOC(@1,@3))); }
| shift_expression RIGHT_OP additive_expression
  { $$=^$(prim2_exp(Bitlrshift,$1,$3,LOC(@1,@3))); }
;

additive_expression:
  multiplicative_expression
    { $$=$!1; }
| additive_expression '+' multiplicative_expression
    { $$=^$(prim2_exp(Plus,$1,$3,LOC(@1,@3))); }
| additive_expression '-' multiplicative_expression
    { $$=^$(prim2_exp(Minus,$1,$3,LOC(@1,@3))); }
;

multiplicative_expression:
  cast_expression
    { $$=$!1; }
| multiplicative_expression '*' cast_expression
    { $$=^$(prim2_exp(Times,$1,$3,LOC(@1,@3))); }
| multiplicative_expression '/' cast_expression
    { $$=^$(prim2_exp(Div,$1,$3,LOC(@1,@3))); }
| multiplicative_expression '%' cast_expression
    { $$=^$(prim2_exp(Mod,$1,$3,LOC(@1,@3))); }
;

cast_expression:
  unary_expression
    { $$=$!1; }
| '(' type_name ')' cast_expression
    { $$=^$(cast_exp($2[2],$4,LOC(@1,@4))); }
;

unary_expression:
  postfix_expression
    { $$=$!1; }
| INC_OP unary_expression
    { $$=^$(pre_inc_exp($2,LOC(@1,@2))); }
| DEC_OP unary_expression
    { $$=^$(pre_dec_exp($2,LOC(@1,@2))); }
| '&' cast_expression
    { $$=^$(address_exp($2,LOC(@1,@2))); }
| '*' cast_expression
    { $$=^$(deref_exp($2,LOC(@1,@2))); }
| '+' cast_expression
    { $$ = $!2; }
/*
| '-' cast_expression
    { $$ = ^$(prim2_exp(Minus,signed_int_exp(0,LOC(@1,@2)),
                        $2,LOC(@1,@2))); }
*/
| unary_operator cast_expression
    { $$=^$(prim1_exp($1,$2,LOC(@1,@2))); }
| SIZEOF '(' type_name ')'
    { $$=^$(new_exp(Sizeof_e($3[2]),LOC(@1,@4))); }
| SIZEOF unary_expression
    { $$=^$(prim1_exp(Size,$2,LOC(@1,@2))); }
/* Cyc: throw, printf, fprintf, sprintf */
| THROW unary_expression
    { $$=^$(new_exp(Throw_e($2),LOC(@1,@2))); }
| format_primop '(' argument_expression_list ')'
    { $$=^$(new_exp(Primop_e($1,$3),LOC(@1,@4))); }
;

format_primop:
  PRINTF { $$=^$(Printf); }
| FPRINTF { $$=^$(Fprintf); }
| XPRINTF { $$=^$(Xprintf); }
;

unary_operator:
  '~'
    { $$=^$(Bitnot); }
| '!'
    { $$=^$(Not); }
| '-'
    { $$=^$(Minus); }
;

postfix_expression:
  primary_expression
    { $$= $!1; }
| postfix_expression '[' expression ']'
    { $$=^$(subscript_exp($1,$3,LOC(@1,@4))); }
| postfix_expression '(' ')'
    { $$=^$(unknowncall_exp($1,null,LOC(@1,@3))); }
| postfix_expression '(' argument_expression_list ')'
    { $$=^$(unknowncall_exp($1,$3,LOC(@1,@4))); }
| postfix_expression '.' IDENTIFIER
    { $$=^$(structmember_exp($1,$3,LOC(@1,@3))); }
// Hack to allow typedef names and field names to overlap
| postfix_expression '.' QUAL_TYPEDEF_NAME
    { qvar q = $3;
      if (q[0] != null)
	err("struct field name is qualified",LOC(@3,@3));
      $$=^$(structmember_exp($1,q[1],LOC(@1,@3)));
    }
| postfix_expression PTR_OP IDENTIFIER
    { $$=^$(structarrow_exp($1,$3,LOC(@1,@3))); }
// Hack to allow typedef names and field names to overlap
| postfix_expression PTR_OP QUAL_TYPEDEF_NAME
    { qvar q = $3;
      if (q[0] != null)
	err("struct field is qualified with module name",LOC(@3,@3));
      $$=^$(structarrow_exp($1,q[1],LOC(@1,@3)));
    }
| postfix_expression INC_OP
    { $$=^$(post_inc_exp($1,LOC(@1,@2))); }
| postfix_expression DEC_OP
    { $$=^$(post_dec_exp($1,LOC(@1,@2))); }
| '(' type_name ')' '{' initializer_list '}'
    { $$=^$(new_exp(CompoundLit_e($2,List::imp_rev($5)),LOC(@1,@6))); }
| '(' type_name ')' '{' initializer_list ',' '}'
    { $$=^$(new_exp(CompoundLit_e($2,List::imp_rev($5)),LOC(@1,@7))); }
/* Cyc: expressions to build arrays */
| NEW '{' '}'
  /* empty arrays */
    { $$=^$(new_exp(Array_e(null),LOC(@1,@3))); }
  /* constant-sized arrays */
| NEW '{' initializer_list '}'
    { $$=^$(new_exp(Array_e(List::imp_rev($3)),LOC(@1,@3))); }
| NEW '{' initializer_list ',' '}'
    { $$=^$(new_exp(Array_e(List::imp_rev($3)),LOC(@1,@4))); }
  /* array comprehension */
| NEW '{' FOR IDENTIFIER '<' expression ':' expression '}'
    { $$=^$(new_exp(Comprehension_e(new_vardecl(&$(null,$4), uint_t,
						&Opt(uint_exp(0,LOC(@4,@4)))),
				    $6, $8),
		    LOC(@1,@9))); }
/* Cyc: added fill and codegen */
| FILL '(' expression ')'
    { $$=^$(new_exp(Fill_e($3),LOC(@1,@4))); }
| CODEGEN '(' function_definition ')'
    { $$=^$(new_exp(Codegen_e($3),LOC(@1,@4))); }
;

primary_expression:
  qual_opt_identifier
    /* This could be an ordinary identifier, a struct tag, or an enum or
       xenum constructor */
    { $$=^$(new_exp(UnknownId_e($1),LOC(@1,@1))); }
| constant
    { $$= $!1; }
| STRING
    { $$=^$(new_exp(Const_e(String_c($1)),LOC(@1,@1))); }
| '(' expression ')'
    { $$= $!2; }
/* Cyc: stop instantiation */
| qual_opt_identifier LEFT_RIGHT
    { $$=^$(new_exp(NoInstantiate_e(new_exp(UnknownId_e($1),LOC(@1,@1))),
                    LOC(@1,@2))); }
/* Cyc: tuple expressions */
| '$' '(' argument_expression_list ')'
    { $$=^$(new_exp(Tuple_e($3),LOC(@1,@4))); }
/* Cyc: structure expressions */
| qual_opt_identifier '{' initializer_list '}'
    { $$=^$(new_exp(Struct_e($1,null,List::imp_rev($3),null),LOC(@1,@4))); }
/* Cyc: compound statement expressions, as in gcc */
| '(' '{' block_item_list '}' ')'
    { $$=^$(new_exp(StmtExp_e($3),LOC(@1,@5))); }
;

argument_expression_list:
  argument_expression_list0
    { $$=^$(List::imp_rev($1)); }
;

/* NB: returns list in reverse order */
argument_expression_list0:
  assignment_expression
    { $$=^$(&cons($1,null)); }
| argument_expression_list0 ',' assignment_expression
    { $$=^$(&cons($3,$1)); }
;

/* NB: We've had to move enumeration constants into primary_expression
   because the lexer can't tell them from ordinary identifiers */
constant:
  INTEGER_CONSTANT
    { $$=^$(int_exp($1[0],$1[1],LOC(@1,@1))); }
| CHARACTER_CONSTANT
    { $$=^$(new_exp(Const_e(Char_c(Unsigned,$1)),LOC(@1,@1))); }
| FLOATING_CONSTANT
    { $$=^$(new_exp(Const_e(Float_c($1)),LOC(@1,@1))); }
/* Cyc: null */
| NULL
    { $$=^$(new_exp(Const_e(Null_c),LOC(@1,@1))); }
;

qual_opt_identifier:
  IDENTIFIER      { $$=^$(&$(null,$1)); }
| QUAL_IDENTIFIER { $$=$!1; }
;

%%


void yyprint(int i, xenum YYSTYPE v) {
  switch (v) {
  case Okay_tok:          fprintf(cyc_stderr,"ok");         break;
  case Int_tok(&$(_,i2)): fprintf(cyc_stderr,"%d",i2);      break;
  case Char_tok(c):       fprintf(cyc_stderr,"%c",c);       break;
  case Short_tok(s):      fprintf(cyc_stderr,"%ds",(int)s); break;
  case String_tok(s):          fprintf(cyc_stderr,"\"%s\"",s); break;
  case StringOpt_tok(null):    fprintf(cyc_stderr,"null");     break;
  case StringOpt_tok(&Opt(s)): fprintf(cyc_stderr,"\"%s\"",s); break;
  case QualId_tok(&$(prefix,v2)):
    for (; prefix != null; prefix = prefix->tl)
      fprintf(cyc_stderr,"%s::",prefix->hd);
    fprintf(cyc_stderr,"%s::",v2);
    break;
  default: fprintf(cyc_stderr,"?"); break;
  }
 }

namespace Parse{
list<decl> parse_file(FILE @f) {
  parse_result = null;
  lbuf = &Opt(from_file(f));
  yyparse();
  return parse_result;
}
}
