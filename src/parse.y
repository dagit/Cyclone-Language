// parse.y:  Copyright (c) 2000, Greg Morrisett & Trevor Jim,
// all rights reserved.
//
// An adaptation of the ISO C grammar for Cyclone.  This grammar
// is adapted from the proposed C9X standard, but the productions are
// arranged as in Kernighan and Ritchie's "The C Programming
// Language (ANSI C)", Second Edition, pages 234-239.
//
// The grammar has 1 shift-reduce conflict, due to the "dangling else"
// problem, which is properly resolved by the Cyclone port of Bison.

%{
#define YYDEBUG 0 // 1 to debug, 0 otherwise
#define YYPRINT yyprint
#define YYERROR_VERBOSE

#if YYDEBUG==1
extern xtunion YYSTYPE;
extern void yyprint(int i, xtunion YYSTYPE v);
#endif


#include "core.h"
#include "stdio.h"
#include "lexing.h"
#include "list.h"
#include "string.h"
#include "set.h"
#include "position.h"
#include "absyn.h"
#include "pp.h"
using Core;
using Stdio;
using Lexing;
using List;
using Absyn;
using Position;
using String;

// Typedef processing must be split between the parser and lexer.
// These functions are called by the parser to communicate typedefs
// to the lexer, so that the lexer can distinguish typedef names from
// identifiers.
namespace Lex {
  extern void register_typedef(qvar_t s);
  extern void enter_namespace(var_t);
  extern void leave_namespace();
  extern void enter_using(qvar_t);
  extern void leave_using();
}

#define LOC(s,e) Position::segment_of_abs(s.first_line,e.last_line)
#define DUMMYLOC null

namespace Parse {

// Type definitions only needed during parsing.

tunion Struct_or_union { Struct_su, Union_su; };
typedef tunion Struct_or_union struct_or_union_t;

tunion Blockitem {
  TopDecls_bl(list_t<decl_t>);
  Stmt_bl(stmt_t);
  FnDecl_bl(fndecl_t);
};
typedef tunion Blockitem blockitem_t;

tunion Type_specifier {
  Signed_spec(seg_t);
  Unsigned_spec(seg_t);
  Short_spec(seg_t);
  Long_spec(seg_t);
  Type_spec(type_t,seg_t);    // int, `a, list_t<`a>, etc.
  Decl_spec(decl_t);
};
typedef tunion Type_specifier type_specifier_t;

tunion Storage_class {
  Typedef_sc, Extern_sc, ExternC_sc, Static_sc, Auto_sc,
  Register_sc, Abstract_sc;
};
typedef tunion Storage_class storage_class_t;

struct Declaration_spec {
  opt_t<storage_class_t>   sc;
  tqual_t                  tq;
  list_t<type_specifier_t> type_specs;
  bool                     is_inline;
  list_t<attribute_t>      attributes;
};
typedef struct Declaration_spec @decl_spec_t;

struct Declarator {
  qvar_t                  id;
  list_t<type_modifier_t> tms;
};
typedef struct Declarator @declarator_t;

struct Abstractdeclarator {
  list_t<type_modifier_t> tms;
};
typedef struct Abstractdeclarator @abstractdeclarator_t;

// forward references
static $(var_t,tqual_t,type_t)@
  fnargspec_to_arg(seg_t loc,$(opt_t<var_t>,tqual_t,type_t)@ t);
static $(type_t,opt_t<decl_t>)
  collapse_type_specifiers(list_t<type_specifier_t> ts, seg_t loc);
static $(tqual_t,type_t,list_t<tvar_t>,list_t<attribute_t>)
  apply_tms(tqual_t,type_t,list_t<attribute_t>,list_t<type_modifier_t>);
static decl_t v_typ_to_typedef(seg_t loc, $(qvar_t,tqual_t,type_t,
                                            list_t<tvar_t>,
                                            list_t<attribute_t>)@ t);

// global state (we're not re-entrant)
opt_t<Lexbuf<Function_lexbuf_state<FILE@>>> lbuf = null;
static list_t<decl_t> parse_result = null;

// Definitions and Helper Functions

  // Could be static variables in the attribute production, but we don't
  // compile static variables correctly
tunion Attribute.Aligned_att att_aligned = Aligned_att(-1);
$(string,tunion Attribute) att_map[] = {
  $("stdcall", Stdcall_att),
  $("cdecl", Cdecl_att),
  $("fastcall", Fastcall_att),
  $("noreturn", Noreturn_att),
  $("const", Const_att), // a keyword (see grammar), but __const__ possible
  $("aligned", (tunion Attribute)&att_aligned), // WARNING: sharing!
  $("packed", Packed_att),
  $("shared", Shared_att),
  $("unused", Unused_att),
  $("weak", Weak_att),
  $("dllimport", Dllimport_att),
  $("dllexport", Dllexport_att),
  $("no_instrument_function", No_instrument_function_att),
  $("constructor", Constructor_att),
  $("destructor", Destructor_att),
  $("no_check_memory_usage", No_check_memory_usage_att)
};

// Error functions
static void err(string msg, seg_t sg) {
  post_error(mk_err_parse(sg,msg));
}
static `a abort(string msg,seg_t sg) {
  err(msg,sg);
  throw Exit;
}
static void warn(string msg,seg_t sg) {
  fprintf(stderr,"%s: Warning: %s\n",string_of_segment(sg),msg);
  return;
}
static `a unimp(string msg,seg_t sg) {
  return abort(xprintf("%s unimplemented",msg),sg);
}
static void unimp2(string msg,seg_t sg) {
  fprintf(stderr,"%s: Warning: Cyclone does not yet support %s\n",
	  string_of_segment(sg),msg);
  return;
}

// Functions for creating abstract syntax
static structfield_t
make_struct_field(seg_t loc,
                  $($(qvar_t,tqual_t,type_t,list_t<tvar_t>,
                      list_t<attribute_t>)@,
                    opt_t<exp_t>)@ field_info) {
  let $(field,expopt) = *field_info;
  if ((*field)[3] != null)
    err("bad type params in struct field",loc);
  let qid = (*field)[0];
  switch ((*qid)[0]) {
  case &Rel_n(null): break;
  case &Abs_n(null): break;
  case Loc_n: break;
  default:
    err("struct field cannot be qualified with a module name",loc);
    break;
  }
  let atts = (*field)[4];
  return new Structfield{.name = (*qid)[1], .tq = (*field)[1],
                            .type = (*field)[2],
                            .width = expopt,
                            .attributes = atts};
}

static $(opt_t<var_t>,tqual_t,type_t)@
make_param(seg_t loc, $(opt_t<qvar_t>,tqual_t,type_t,list_t<tvar_t>)@ field){
  let &$(qv_opt,tq,t,tvs) = field;
  let idopt = null;
  if (qv_opt != null) {
    idopt = new Opt((*qv_opt->v)[1]);
    switch ((*qv_opt->v)[0]) {
    case &Rel_n(null): break;
    case &Abs_n(null): break;
    case Loc_n: break;
    default:
      err("parameter cannot be qualified with a module name",loc);
      break;
    }
  }
  if (tvs != null)
    abort("parameter should have no type parameters",loc);
  return new $(idopt,tq,t);
}

static type_specifier_t type_spec(type_t t,seg_t loc) {
  return new Type_spec(t,loc);
}

// convert any array types to pointer types
static type_t array2ptr(type_t t) {
  switch (t) {
  case &ArrayType(t1,tq,eopt):
    return starb_typ(t1,HeapRgn,tq,
                     eopt == null ? Unknown_b : new Upper_b((exp_t)eopt));
  default: return t;
  }
}

// convert an argument's type from arrays to pointers
static void arg_array2ptr($(opt_t<var_t>,tqual_t,type_t) @x) {
  (*x)[2] = array2ptr((*x)[2]);
}

// given an optional variable, tqual, type, and list of type
// variables, return the tqual and type and check that the type
// variables are null -- used when we have a tuple type specification.
static $(tqual_t,type_t)@
  get_tqual_typ(seg_t loc,$(opt_t<var_t>,tqual_t,type_t) @t) {
  return new $((*t)[1],(*t)[2]);
}

static void only_vardecl(list_t<stringptr> params,decl_t x) {
  string decl_kind;
  switch (x->r) {
  case &Var_d(vd):
    if (vd->initializer != null)
      abort("initializers are not allowed in parameter declarations",x->loc);
    switch ((*vd->name)[0]) {
    case Loc_n: break;
    case &Rel_n(null): break;
    case &Abs_n(null): break;
    default:
      err("module names not allowed on parameter declarations",x->loc);
      break;
    }
    // for sanity-checking of old-style parameter declarations
    bool found = false;
    for(; params != null; params = params->tl)
      if(zstrptrcmp((*vd->name)[1], params->hd)==0) {
	found = true;
	break;
      }
    if (!found)
      abort(xprintf("%s is not listed as a parameter",
		    *((*vd->name)[1])),x->loc);
    return;
  case &Let_d(_,_,_,_,_): decl_kind = "let declaration";        break;
  case &Letv_d(_):        decl_kind = "let declaration";        break;
  case &Fn_d(_):          decl_kind = "function declaration";   break;
  case &Struct_d(_):      decl_kind = "struct declaration";     break;
  case &Union_d(_):       decl_kind = "union declaration";      break;
  case &Tunion_d(_):      decl_kind = "tunion declaration";     break;
  case &Typedef_d(_):     decl_kind = "typedef";                break;
  case &Enum_d(_):        decl_kind = "enum declaration";       break;
  case &Namespace_d(_,_): decl_kind = "namespace declaration";  break;
  case &Using_d(_,_):     decl_kind = "using declaration";      break;
  case &ExternC_d(_):     decl_kind = "extern C declaration";   break;
  }
  abort(xprintf("%s appears in parameter type", decl_kind), x->loc);
  return;
}

// For old-style function definitions,
// get a parameter type from a list of declarations
static $(opt_t<var_t>,tqual_t,type_t)@
  get_param_type($(list_t<decl_t>,seg_t)@ env, stringptr x) {
  let &$(tdl,loc) = env;
  if (tdl==null)
    return(abort(xprintf("missing type for parameter %s",*x),loc));
  switch (tdl->hd->r) {
  case &Var_d(vd):
    switch ((*vd->name)[0]) {
    case Loc_n: break;
    case &Rel_n(null): break;
    case &Abs_n(null): break;
    default:
      err("module name not allowed on parameter",loc);
      break;
    }
    if (zstrptrcmp((*vd->name)[1],x)==0)
      return new {$(new{Opt((*vd->name)[1])},vd->tq,vd->type)};
    else
      return get_param_type(new $(tdl->tl,loc),x);
  default:
    // This should never happen, because we use only_vardecl first
    return(abort("non-variable declaration",tdl->hd->loc));
  }
}

static bool is_typeparam(type_modifier_t tm) {
  switch (tm) {
  case &TypeParams_mod(_,_,_): return true;
  default: return false;
  }
}

// convert an identifier to a type -- if it's the special identifier
// `H then return HeapRgn, otherwise, return a type variable.
static type_t id2type(string s, conref_t<kind_t> k) {
  if (zstrcmp(s,"`H") == 0)
    return HeapRgn;
  else
    return new VarType(new Tvar(new {s},k));
}

// convert a list of types to a list of typevars -- the parser can't
// tell lists of types apart from lists of typevars easily so we parse
// them as types and then convert them back to typevars.  See
// productions "struct_or_union_specifier" and "tunion_specifier";
static tvar_t typ2tvar(seg_t loc, type_t t) {
  switch (t) {
  case &VarType(pr): return pr;
  default: return abort("expecting a list of type variables, not types",loc);
  }
}
static type_t tvar2typ(tvar_t pr) {
  return new VarType(pr);
}

// Convert an old-style function into a new-style function
static list_t<type_modifier_t>
  oldstyle2newstyle(list_t<type_modifier_t> tms,
                    list_t<decl_t> tds, seg_t loc) {
  // Not an old-style function
  if (tds==null) return tms;

  // If no function is found, or the function is not innermost, then
  // this is not a function definition; it is an error.  But, we
  // return silently.  The error will be caught by make_function below.
  if (tms==null) return null;

  switch (tms->hd) {
    case &Function_mod(args): {
      // Is this the innermost function??
      if (tms->tl==null ||
          (is_typeparam(tms->tl->hd) && tms->tl->tl==null)) {
        // Yes
        switch (args) {
        case &WithTypes(_,_,_,_):
          warn("function declaration with both new- and old-style parameter"
	       "declarations; ignoring old-style",loc);
          return tms;
        case &NoTypes(ids,_):
          List::iter_c(only_vardecl,ids,tds);
          let env = new $(tds,loc);
          return
            new List(new Function_mod(new WithTypes(List::map_c(get_param_type,
                                                                env,ids),
                                                    false,null,null)),null);
        }
      } else
        // No, keep looking for the innermost function
        return new List(tms->hd,oldstyle2newstyle(tms->tl,tds,loc));
    }
    default: return new List(tms->hd,oldstyle2newstyle(tms->tl,tds,loc));
  }
}

// make a top-level function declaration out of a declaration-specifier
// (return type, etc.), a declarator (the function name and args),
// a declaration list (for old-style function definitions), and a statement.
static fndecl_t make_function(opt_t<decl_spec_t> dso, declarator_t d,
                              list_t<decl_t> tds, stmt_t body, seg_t loc) {
  // Handle old-style parameter declarations
  if (tds!=null)
    d = new Declarator(d->id,oldstyle2newstyle(d->tms,tds,loc));

  scope_t sc = Public;
  list_t<type_specifier_t> tss = null;
  tqual_t tq = empty_tqual();
  bool is_inline = false;
  list_t<attribute_t> atts = null;

  if (dso != null) {
    tss = dso->v->type_specs;
    tq  = dso->v->tq;
    is_inline = dso->v->is_inline;
    atts = dso->v->attributes;
    // Examine storage class; like C, we allow both static and extern
    if (dso->v->sc != null)
      switch (dso->v->sc->v) {
      case Extern_sc: sc = Extern; break;
      case ExternC_sc: sc = ExternC; break;
      case Static_sc: sc = Static; break;
      default: err("bad storage class on function",loc); break;
      }
  }
  let $(t,decl_opt) = collapse_type_specifiers(tss,loc);
  let $(fn_tqual,fn_type,x,out_atts) = apply_tms(tq,t,atts,d->tms);
  // what to do with the left-over attributes out_atts?  I'm just
  // going to append them to the function declaration and let the
  // type-checker deal with it.
  if (decl_opt != null)
    warn("nested type declaration within function prototype",loc);
  if (x != null)
    // Example:   `a f<`b><`a>(`a x) {...}
    // Here info[2] will be the list `b.
    warn("bad type params, ignoring",loc);
  // fn_type had better be a FnType
  switch (fn_type) {
    case &FnType(FnInfo{tvs,eff,ret_type,args,c_varargs,cyc_varargs,
                          attributes}):
      let args2 = List::map_c(fnargspec_to_arg,loc,args);
      // We don't fill in the cached type here because we may need
      // to figure out the bound type variables and the effect.
      return new Fndecl {.sc=sc,.name=d->id,.tvs=tvs,
                            .is_inline=is_inline,.effect=eff,
                            .ret_type=ret_type,.args=args2,
                            .c_varargs=c_varargs,
                            .cyc_varargs=cyc_varargs,
                            .body=body,.cached_typ=null,
                            .param_vardecls=null,
                            .attributes = append(attributes,out_atts)};
    default:
      return abort("declarator is not a function prototype",loc);
  }
}

static $(var_t,tqual_t,type_t)@
  fnargspec_to_arg(seg_t loc,$(opt_t<var_t>,tqual_t,type_t)@ t) {
  if ((*t)[0] == null) {
    err("missing argument variable in function prototype",loc);
    return new $(new{(string)"?"},(*t)[1],(*t)[2]);
  } else
    return new $((*t)[0]->v,(*t)[1],(*t)[2]);
}

// Given a type-specifier list, determines the type and any declared
// structs, unions, or [x]tunions.  Most of this is just collapsing
// combinations of [un]signed, short, long, int, char, etc.  We're
// probably more permissive than is strictly legal here.  For
// instance, one can write "unsigned const int" instead of "const
// unsigned int" and so forth.  I don't think anyone will care...
// (famous last words.)
static string msg1 = "at most one type may appear within a type specifier";
static string msg2 =
  "const or volatile may appear only once within a type specifier";
static string msg3 = "type specifier includes more than one declaration";
static string msg4 =
  "sign specifier may appear only once within a type specifier";

static $(type_t,opt_t<decl_t>)
  collapse_type_specifiers(list_t<type_specifier_t> ts, seg_t loc) {

  opt_t<decl_t> declopt = null;    // any hidden declarations

  bool      seen_type = false;
  bool      seen_sign = false;
  bool      seen_size = false;
  type_t    t         = VoidType;
  size_of_t sz        = B4;
  sign_t    sgn       = Signed;

  seg_t last_loc = loc;

  for(; ts != null; ts = ts->tl)
    switch (ts->hd) {
    case &Type_spec(t2,loc2):
      if(seen_type) err(msg1,loc2);
      last_loc  = loc2;
      seen_type = true;
      t         = t2;
      break;
    case &Signed_spec(loc2):
      if(seen_sign) err(msg4,loc2);
      if(seen_type) err("signed qualifier must come before type",loc2);
      last_loc  = loc2;
      seen_sign = true;
      sgn       = Signed;
      break;
    case &Unsigned_spec(loc2):
      if(seen_sign) err(msg4,loc2);
      if(seen_type) err("signed qualifier must come before type",loc2);
      last_loc  = loc2;
      seen_sign = true;
      sgn       = Unsigned;
      break;
    case &Short_spec(loc2):
      if(seen_size)
        err("integral size may appear only once within a type specifier",loc2);
      if(seen_type) err("short modifier must come before base type",loc2);
      last_loc  = loc2;
      seen_size = true;
      sz        = B2;
      break;
    case &Long_spec(loc2):
      if(seen_type) err("long modifier must come before base type",loc2);
      // okay to have seen a size so long as it was long (means long long)
      // That happens when we've seen a size yet we're B4
      if(seen_size)
	switch (sz) {
	case B4:
	  sz = B8;
	  break;
	default: err("extra long in type specifier",loc2); break;
	}
      last_loc = loc2;
      // the rest is is necessary if it's long and harmless if long long
      seen_size = true;
      break;
    case &Decl_spec(d):
      // we've got a struct or [xt]union declaration embedded in here -- return
      // the declaration as well as a copy of the type -- this allows
      // us to declare structs as a side effect inside typedefs
      // Note: Leave the last fields null so check_valid_type knows the type
      //       has not been checked.
      last_loc = d->loc;
      if (declopt == null && !seen_type) {
	seen_type = true;
        switch (d->r) {
        case &Struct_d(sd):
          let args = List::map(tvar2typ,sd->tvs);
          t = new StructType(sd->name->v,args,null);
          if (sd->fields!=null) declopt = new Opt(d);
	  break;
        case &Tunion_d(tud):
          let args = List::map(tvar2typ,tud->tvs);
          t = new TunionType(TunionInfo(new KnownTunion(tud), args, HeapRgn));
	  if(tud->fields != null) declopt = new Opt(d);
	  break;
        case &Union_d(ud):
          let args = List::map(tvar2typ,ud->tvs);
          t = new UnionType(ud->name->v,args,null);
          if (ud->fields!=null) declopt = new Opt(d);
	  break;
        case &Enum_d(ed):
          t = new EnumType(ed->name,null);
          declopt = new Opt(d);
          break;
        default:
	  abort("bad declaration within type specifier",d->loc);
	  break;
        }
      } else err(msg3,d->loc);
      break;
    }

  // it's okay to not have an explicit type as long as we have some
  // combination of signed, unsigned, short, long, or longlong
  if (!seen_type) {
    if(!seen_sign && !seen_size) err("missing type withing specifier",last_loc);
    t = new IntType(sgn, sz);
  } else {
    if(seen_sign)
      switch (t) {
      case &IntType(_,sz2): t = new IntType(sgn,sz2); break;
      default: err("sign specification on non-integral type",last_loc); break;
      }
    if(seen_size)
      switch (t) {
      case &IntType(sgn2,_):
	t = new IntType(sgn2,sz);
	break;
      default: err("size qualifier on non-integral type",last_loc); break;
      }
  }
  return $(t, declopt);
}

static list_t<$(qvar_t,tqual_t,type_t,list_t<tvar_t>,list_t<attribute_t>)@>
  apply_tmss(tqual_t tq, type_t t,list_t<declarator_t> ds,
             attributes_t shared_atts)
{
  if (ds==null) return null;
  let d = ds->hd;
  let q = d->id;
  let $(tq,new_typ,tvs,atts) = apply_tms(tq,t,shared_atts,d->tms);
  return new List(new $(q,tq,new_typ,tvs,atts),
                  apply_tmss(tq,t,ds->tl,shared_atts));
}

static $(tqual_t,type_t,list_t<tvar_t>,list_t<attribute_t>)
  apply_tms(tqual_t tq, type_t t, list_t<attribute_t> atts,
            list_t<type_modifier_t> tms) {
  if (tms==null) return $(tq,t,null,atts);
  switch (tms->hd) {
    case Carray_mod:
      return apply_tms(empty_tqual(),new ArrayType(t,tq,null),atts,tms->tl);
    case &ConstArray_mod(e):
      return apply_tms(empty_tqual(),new ArrayType(t,tq,e),atts,tms->tl);
    case &Function_mod(args): {
      switch (args) {
      case &WithTypes(args2,c_vararg,cyc_vararg,eff):
        list_t<tvar_t> typvars = null;
        // any function type attributes seen thus far get put in the function
        // type
        attributes_t fn_atts = null, new_atts = null;
        for (_ as = atts; as != null; as = as->tl) {
          if (fntype_att(as->hd))
            fn_atts = new List(as->hd,fn_atts);
          else
            new_atts = new List(as->hd,new_atts);
        }
        // functions consume type parameters
        if (tms->tl != null) {
          switch (tms->tl->hd) {
          case &TypeParams_mod(ts,_,_):
            typvars = ts;
            tms=tms->tl; // skip TypeParams on call of apply_tms below
	    break;
          default:
            break;
          }
        }
        // special case where the parameters are void, e.g., int f(void)
        if (!c_vararg && cyc_vararg == null // not vararg function
            && args2 != null      // not empty arg list
            && args2->tl == null   // not >1 arg
            && (*args2->hd)[0] == null // not f(void x)
            && (*args2->hd)[2] == VoidType) {
	  args2 = null;
	}
	// convert result type from array to pointer result
	t = array2ptr(t);
	// convert any array arguments to pointer arguments
	List::iter(arg_array2ptr,args2);
        // Note, we throw away the tqual argument.  An example where
        // this comes up is "const int f(char c)"; it doesn't really
        // make sense to think of the function as returning a const
        // (or volatile, or restrict).  The result will be copied
        // anyway.  TODO: maybe we should issue a warning.  But right
        // now we don't have a loc so the warning will be confusing.
        return apply_tms(empty_tqual(),
			 function_typ(typvars,eff,t,args2,
                                      c_vararg,cyc_vararg,fn_atts),
                         new_atts,
                         tms->tl);
      case &NoTypes(_,loc):
        throw abort("function declaration without parameter types",loc);
      }
    }
    case &TypeParams_mod(ts,loc,_): {
      // If we are the last type modifier, this could be the list of
      // type parameters to a typedef:
      // typedef struct foo<`a,int> foo_t<`a>
      if (tms->tl==null)
        return $(tq,t,ts,atts);
      // Otherwise, it is an error in the program if we get here;
      // TypeParams should already have been consumed by an outer
      // Function (see last case).
      throw abort("type parameters must appear before function arguments "
		  "in declarator",loc);
    }
    case &Pointer_mod(ps,rgntyp,tq2): {
      switch (ps) {
      case &NonNullable_ps(ue):
	return apply_tms(tq2,atb_typ(t,rgntyp,tq,
                                     new Upper_b(ue)),atts,tms->tl);
      case &Nullable_ps(ue):
	return apply_tms(tq2,starb_typ(t,rgntyp,tq,
                                       new Upper_b(ue)),atts,tms->tl);
      case TaggedArray_ps:
	return apply_tms(tq2,tagged_typ(t,rgntyp,tq),atts,tms->tl);
      }
    }
    case &Attributes_mod(loc,atts2):
      // FIX: get this in line with GCC
      // attributes get attached to function types -- I doubt that this
      // is GCC's behavior but what else to do?
      return apply_tms(tq,t,List::append(atts,atts2),tms->tl);
  }
}

// given a specifier-qualifier list, warn and ignore about any nested type
// definitions and return the collapsed type.
// FIX: We should somehow deal with the nested type definitions.
type_t speclist2typ(list_t<type_specifier_t> tss, seg_t loc) {
  let $(t,decls_opt) = collapse_type_specifiers(tss,loc);
  if(decls_opt != null)
    warn("ignoring nested type declaration(s) in specifier list",loc);
  return t;
}


// given a local declaration and a statement produce a decl statement
static stmt_t flatten_decl(decl_t d,stmt_t s) {
  return new_stmt(new Decl_s(d,s),segment_join(d->loc,s->loc));
}

// given a list of local declarations and a statement, produce a big
// decl statement.
static stmt_t flatten_declarations(list_t<decl_t> ds, stmt_t s){
  return List::fold_right(flatten_decl,ds,s);
}

// Given a declaration specifier list (a combination of storage class
// [typedef, extern, static, etc.] and type specifiers (signed, int,
// `a, const, etc.), and a list of declarators and initializers,
// produce a list of top-level declarations.  By far, this is the most
// involved function and thus I expect a number of subtle errors.
static list_t<decl_t> make_declarations(decl_spec_t ds,
                                        list_t<$(declarator_t,exp_opt_t)@> ids,
                                        seg_t loc) {
  list_t<type_specifier_t> tss       = ds->type_specs;
  tqual_t                tq        = ds->tq;
  bool                   istypedef = false;  // updated below if necessary
  scope_t                s         = Public; // updated below if necessary
  list_t<attribute_t>    atts      = ds->attributes;

  if (ds->is_inline)
    err("inline is only allowed on function definitions",loc);
  if (tss == null) {
    err("missing type specifiers in declaration",loc);
    return null;
  }

  if (ds->sc != null)
    switch (ds->sc->v) {
    case Typedef_sc:  istypedef = true; break;
    case Extern_sc:   s = Extern;   break;
    case ExternC_sc:  s = ExternC;  break;
    case Static_sc:   s = Static;   break;
    case Auto_sc:     s = Public;   break;
    case Register_sc: s = Public;   break;
    case Abstract_sc: s = Abstract; break;
    }

  // separate the declarators from their initializers
  let $(declarators, exprs) = List::split(ids);
  // check to see if there are no initializers -- useful later on
  bool exps_empty = true;
  for (list_t<exp_opt_t> es = exprs; es != null; es = es->tl)
    if (es->hd != null) {
      exps_empty = false;
      break;
    }

  // Collapse the type specifiers to get the base type and any
  // optional nested declarations
  let ts_info = collapse_type_specifiers(tss,loc);
  if (declarators == null) {
    // here we have a type declaration -- either a struct, union,
    // tunion, or xtunion as in: "struct Foo { ... };"
    let $(t,declopt) = ts_info;
    if (declopt != null) {
      decl_t d = declopt->v;
      switch (d->r) {
      case &Enum_d(ed):
        ed->sc = s;
        if (atts != null) err("bad attributes on enum",loc); break;
      case &Struct_d(sd): sd->sc = s; sd->attributes = atts; break;
      case &Union_d(ud): ud->sc = s; ud->attributes = atts; break;
      case &Tunion_d(tud):
        tud->sc = s;
        if (atts != null) err("bad attributes on tunion",loc); break;
      default: err("bad declaration",loc); return null;
      }
      return new List(d,null);
    } else {
      switch (t) {
      case &StructType(n,ts,_):
        let ts2 = List::map_c(typ2tvar,loc,ts);
        let sd = new Structdecl{s,new Opt((typedef_name_t)n),ts2,null,null};
        if (atts != null) err("bad attributes on struct",loc);
        return new List(new_decl(new Struct_d(sd),loc),null);
      case &TunionType(TunionInfo(&KnownTunion(tud),_,_)):
	if(atts != null) err("bad attributes on tunion", loc);
	return new List(new_decl(new Tunion_d(tud),loc),null);
      case &TunionType(TunionInfo(&UnknownTunion(UnknownTunionInfo(n,isx)),ts,_)):
        let ts2 = List::map_c(typ2tvar,loc,ts);
        let tud = tunion_decl(s, n, ts2, null, isx, loc);
        if (atts != null) err("bad attributes on tunion",loc);
        return new List(tud,null);
      case &UnionType(n,ts,_):
        let ts2 = List::map_c(typ2tvar,loc,ts);
        let ud = new Uniondecl{s,new Opt((typedef_name_t)n),ts2,null,null};
        if (atts != null) err("bad attributes on union",loc);
        return new List(new Decl(new Union_d(ud),loc),null);
      case &EnumType(n,_):
        let ed = new Enumdecl{s,n,null};
        if (atts != null) err("bad attributes on enum",loc);
        return new List(new Decl(new Enum_d(ed),loc),null);
      default: err("missing declarator",loc); return null;
      }
    }
  } else {
    // declarators != null
    type_t t      = ts_info[0];
    let fields = apply_tmss(tq,t,declarators,atts);
    if (istypedef) {
      // we can have a nested struct, union, tunion, or xtunion
      // declaration within the typedef as in:
      // typedef struct Foo {...} t;
      if (!exps_empty)
	err("initializer in typedef declaration",loc);
      list_t<decl_t> decls = List::map_c(v_typ_to_typedef,loc,fields);
      if (ts_info[1] != null) {
        decl_t d = ts_info[1]->v;
        switch (d->r) {
        case &Struct_d(sd):
          sd->sc = s; sd->attributes = atts; atts = null;
          break;
        case &Tunion_d(tud)  : tud->sc = s; break;
        case &Union_d(ud)    : ud->sc = s; break;
        case &Enum_d(ed)     : ed->sc = s; break;
        default:
          err("declaration within typedef is not a struct, union, tunion, "
              "or xtunion",loc);
	  break;
        }
        decls = new List(d,decls);
      }
      if (atts != null)
        err(xprintf("bad attribute %s in typedef",attribute2string(atts->hd)),
            loc);
      return decls;
    } else {
      // here, we have a bunch of variable declarations
      if (ts_info[1] != null)
        unimp2("nested type declaration within declarator",loc);
      list_t<decl_t> decls = null;
      for (let ds = fields; ds != null; ds = ds->tl, exprs = exprs->tl) {
	let &$(x,tq2,t2,tvs2,atts2) = ds->hd;
        if (tvs2 != null)
          warn("bad type params, ignoring",loc);
        if (exprs == null)
          abort("unexpected null in parse!",loc);
        let eopt = exprs->hd;
        let vd   = new_vardecl(x, t2, eopt);
	vd->tq = tq2;
	vd->sc = s;
        vd->attributes = atts2;
        let d = new Decl(new Var_d(vd),loc);
        decls = new List(d,decls);
      }
      return List::imp_rev(decls);
    }
  }
}

// Convert an identifier to a kind
static kind_t id_to_kind(string s, seg_t loc) {
  if (strlen(s) != 1) {
    err(xprintf("bad kind: %s",s), loc);
    return BoxKind;
  } else {
    switch (s[0]) {
    case 'A': return AnyKind;
    case 'M': return MemKind;
    case 'B': return BoxKind;
    case 'R': return RgnKind;
    case 'E': return EffKind;
    default:
      err(xprintf("bad kind: %s",s), loc);
      return BoxKind;
    }
  }
}

// Turn an optional list of attributes into an Attribute_mod
static list_t<type_modifier_t> attopt_to_tms(seg_t loc,
                                             attributes_t atts,
                                             list_t<type_modifier_t> tms) {
  if (atts == null) return tms;
  else return new List {new Attributes_mod(loc,atts),tms};
}


// convert an (optional) variable, tqual, type, and type
// parameters to a typedef declaration.  As a side effect, register
// the typedef with the lexer.
// TJ: the tqual should make it into the typedef as well,
// e.g., typedef const int CI;
static decl_t v_typ_to_typedef(seg_t loc, $(qvar_t,tqual_t,type_t,list_t<tvar_t>,list_t<attribute_t>)@ t) {
  qvar_t x = (*t)[0];
  // tell the lexer that x is a typedef identifier
  Lex::register_typedef(x);
  if ((*t)[4] != null)
    err(xprintf("bad attribute %s within typedef",
                attribute2string((*t)[4]->hd)),loc);
  return new_decl(new Typedef_d(new Typedefdecl{.name=x, .tvs=(*t)[3],
                                                   .defn=(*t)[2]}),loc);
}
} // end namespace Parse
using Parse;
%}

// ANSI C keywords
%token AUTO REGISTER STATIC EXTERN TYPEDEF VOID CHAR SHORT INT LONG FLOAT
%token DOUBLE SIGNED UNSIGNED CONST VOLATILE RESTRICT
%token STRUCT UNION CASE DEFAULT INLINE
%token IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN SIZEOF ENUM
// Cyc:  CYCLONE additional keywords
%token NULL_kw LET THROW TRY CATCH
%token NEW ABSTRACT FALLTHRU USING NAMESPACE TUNION XTUNION
%token FILL CODEGEN CUT SPLICE
%token PRINTF FPRINTF XPRINTF SCANF FSCANF SSCANF MALLOC
%token REGION_T REGION RNEW RMALLOC
// double and triple-character tokens
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN ELLIPSIS LEFT_RIGHT
// Cyc:  added COLON_COLON double token
%token COLON_COLON
// identifiers and constants
%token IDENTIFIER INTEGER_CONSTANT STRING
%token CHARACTER_CONSTANT FLOATING_CONSTANT
// Cyc: type variables, qualified identifers and typedef names
%token TYPE_VAR QUAL_IDENTIFIER QUAL_TYPEDEF_NAME
// Cyc: added __attribute__ keyword
%token ATTRIBUTE
// the union type for all productions -- for now a placeholder
%union {
  Okay_tok;
  Bool_tok(bool);
  Int_tok($(sign_t,int)@);
  Char_tok(char);
  Pointer_Sort_tok(tunion Pointer_Sort);
  Short_tok(short);
  String_tok(string);
  Stringopt_tok(opt_t<stringptr>);
  Type_tok(type_t);
  TypeList_tok(list_t<type_t>);
  Exp_tok(exp_t);
  ExpList_tok(list_t<exp_t>);
  Primop_tok(primop_t);
  Primopopt_tok(opt_t<primop_t>);
  QualId_tok(qvar_t);
  Stmt_tok(stmt_t);
  SwitchClauseList_tok(list_t<switch_clause_t>);
  Pattern_tok(pat_t);
  PatternList_tok(list_t<pat_t>);
  FnDecl_tok(fndecl_t);
  DeclList_tok(list_t<decl_t>);
  DeclSpec_tok(decl_spec_t);
  InitDecl_tok($(declarator_t,exp_opt_t)@);
  InitDeclList_tok(list_t<$(declarator_t,exp_opt_t)@>);
  StorageClass_tok(storage_class_t);
  TypeSpecifier_tok(type_specifier_t);
  QualSpecList_tok($(tqual_t,list_t<type_specifier_t>,attributes_t)@);
  TypeQual_tok(tqual_t);
  StructFieldDeclList_tok(list_t<structfield_t>);
  StructFieldDeclListList_tok(list_t<list_t<structfield_t>>);
  Declarator_tok(declarator_t);
  DeclaratorExpopt_tok($(declarator_t,opt_t<exp_t>)@);
  DeclaratorExpoptList_tok(list_t<$(declarator_t,opt_t<exp_t>)@>);
  AbstractDeclarator_tok(abstractdeclarator_t);
  TunionField_tok(tunionfield_t);
  TunionFieldList_tok(list_t<tunionfield_t>);
  ParamDecl_tok($(opt_t<var_t>,tqual_t,type_t)@);
  ParamDeclList_tok(list_t<$(opt_t<var_t>,tqual_t,type_t)@>);
  ParamDeclListBool_tok($(list_t<$(opt_t<var_t>,tqual_t,type_t)@>,
                          bool,vararg_info_t *,opt_t<type_t>)@);
  StructOrUnion_tok(struct_or_union_t);
  IdList_tok(list_t<var_t>);
  Designator_tok(designator_t);
  DesignatorList_tok(list_t<designator_t>);
  TypeModifierList_tok(list_t<type_modifier_t>);
  Rgn_tok(type_t);
  InitializerList_tok(list_t<$(list_t<designator_t>,exp_t)@>);
  FieldPattern_tok($(list_t<designator_t>,pat_t)@);
  FieldPatternList_tok(list_t<$(list_t<designator_t>,pat_t)@>);
  BlockItem_tok(blockitem_t);
  Kind_tok(kind_t);
  Attribute_tok(attribute_t);
  AttributeList_tok(list_t<attribute_t>);
  Enumfield_tok(enumfield_t);
  EnumfieldList_tok(list_t<enumfield_t>);
  Scope_tok(scope_t);
  TypeOpt_tok(opt_t<type_t>)
}
/* types for productions */
%type <Int_tok> INTEGER_CONSTANT
%type <String_tok> FLOATING_CONSTANT
%type <Char_tok> CHARACTER_CONSTANT
%type <Pointer_Sort_tok> pointer_char
%type <String_tok> IDENTIFIER TYPE_VAR STRING
%type <String_tok> namespace_action
%type <Exp_tok> primary_expression postfix_expression unary_expression
%type <Exp_tok> cast_expression constant multiplicative_expression
%type <Exp_tok> additive_expression shift_expression relational_expression
%type <Exp_tok> equality_expression and_expression exclusive_or_expression
%type <Exp_tok> inclusive_or_expression logical_and_expression
%type <Exp_tok> logical_or_expression conditional_expression
%type <Exp_tok> assignment_expression expression constant_expression
%type <Exp_tok> initializer array_initializer
%type <ExpList_tok> argument_expression_list argument_expression_list0
%type <InitializerList_tok> initializer_list
%type <Primop_tok> unary_operator format_primop
%type <Primopopt_tok> assignment_operator
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
%type <DeclList_tok> translation_unit external_declaration
%type <DeclSpec_tok> declaration_specifiers
%type <InitDecl_tok> init_declarator
%type <InitDeclList_tok> init_declarator_list init_declarator_list0
%type <StorageClass_tok> storage_class_specifier
%type <TypeSpecifier_tok> type_specifier enum_specifier
%type <TypeSpecifier_tok> struct_or_union_specifier tunion_specifier
%type <StructOrUnion_tok> struct_or_union
%type <TypeQual_tok> type_qualifier type_qualifier_list
%type <StructFieldDeclList_tok> struct_declaration_list struct_declaration
%type <StructFieldDeclListList_tok> struct_declaration_list0
%type <TypeModifierList_tok> pointer
%type <Rgn_tok> rgn_opt rgn
%type <Declarator_tok> declarator direct_declarator
%type <DeclaratorExpopt_tok> struct_declarator
%type <DeclaratorExpoptList_tok> struct_declarator_list struct_declarator_list0
%type <AbstractDeclarator_tok> abstract_declarator direct_abstract_declarator
%type <Bool_tok> tunion_or_xtunion optional_inject
%type <Scope_tok> tunionfield_scope
%type <TunionField_tok> tunionfield
%type <TunionFieldList_tok> tunionfield_list
%type <QualSpecList_tok> specifier_qualifier_list
%type <IdList_tok> identifier_list identifier_list0
%type <ParamDecl_tok> parameter_declaration type_name
%type <ParamDeclList_tok> parameter_list
%type <ParamDeclListBool_tok> parameter_type_list
%type <TypeList_tok> type_name_list type_params_opt effect_set region_set
%type <TypeList_tok> atomic_effect
%type <DesignatorList_tok> designation designator_list
%type <Designator_tok> designator
%type <Kind_tok> kind
%type <Type_tok> any_type_name
%type <AttributeList_tok> attributes_opt attributes attribute_list
%type <Attribute_tok> attribute
%type <Enumfield_tok> enum_field
%type <EnumfieldList_tok> enum_declaration_list
%type <TypeOpt_tok> optional_effect
/* start production */
%start prog
%%

prog:
  translation_unit
    { $$ = $!1;
      parse_result = $1;
    }

translation_unit:
  external_declaration translation_unit
    { $$=^$(List::imp_append($1,$2)); }
/* Cyc: added using and namespace */
/* NB: using_action calls Lex::enter_using */
| using_action ';' translation_unit
    { $$=^$(new List(new Decl(new Using_d($1,$3),LOC(@1,@3)),null));
      Lex::leave_using();
    }
| using_action '{' translation_unit unusing_action translation_unit
    { $$=^$(new List(new Decl(new Using_d($1,$3),LOC(@1,@4)),$5));
    }
/* NB: namespace_action calls Lex::enter_namespace */
| namespace_action ';' translation_unit
    { $$=^$(new List(new Decl(new Namespace_d(new {$1},$3),LOC(@1,@3)),null));
      Lex::leave_namespace();
    }
| namespace_action '{' translation_unit unnamespace_action translation_unit
    { $$=^$(new List(new Decl(new Namespace_d(new {$1},$3),LOC(@1,@4)),$5));
    }
| EXTERN STRING '{' translation_unit '}' translation_unit
    { if (String::strcmp($2,"C") != 0)
        err("only extern \"C\" { ... } is allowed",LOC(@1,@2));
      $$=^$(new List(new Decl(new ExternC_d($4),LOC(@1,@5)),$6));
    }
| /* empty */ { $$=^$(null); }
;

external_declaration:
  function_definition
  { $$=^$(new List(new_decl(new Fn_d($1),LOC(@1,@1)),null)); }
| declaration         { $$=$!1; }
;

function_definition:
  declarator compound_statement
    { $$=^$(make_function(null,$1,null,$2,LOC(@1,@2))); }
| declaration_specifiers declarator compound_statement
    { $$=^$(make_function(new Opt($1),$2,null,$3,LOC(@1,@3))); }
| declarator declaration_list compound_statement
    { $$=^$(make_function(null,$1,$2,$3,LOC(@1,@3))); }
| declaration_specifiers declarator declaration_list compound_statement
    { $$=^$(make_function(new Opt($1),$2,$3,$4,LOC(@1,@4))); }
;

/* Used for nested functions; the optional declaration_specifiers
   would cause parser conflicts */
function_definition2:
  declaration_specifiers declarator compound_statement
    { $$=^$(make_function(new Opt($1),$2,null,$3,LOC(@1,@3))); }
| declaration_specifiers declarator declaration_list compound_statement
    { $$=^$(make_function(new Opt($1),$2,$3,$4,LOC(@1,@4))); }
;

using_action:
  USING qual_opt_identifier { Lex::enter_using($2); $$=$!2; }
;

unusing_action:
  '}' { Lex::leave_using(); }
;

namespace_action:
  NAMESPACE IDENTIFIER { Lex::enter_namespace(new {$2}); $$=$!2; }
;

unnamespace_action:
  '}' { Lex::leave_namespace(); }
;

/***************************** DECLARATIONS *****************************/
declaration:
  declaration_specifiers ';'
    { $$=^$(make_declarations($1,null,LOC(@1,@1))); }
| declaration_specifiers init_declarator_list ';'
    { $$=^$(make_declarations($1,$2,LOC(@1,@3))); }
/* Cyc:  let declaration */
| LET pattern '=' expression ';'
    { $$=^$(new List(let_decl($2,null,$4,LOC(@1,@5)),null)); }
| LET identifier_list ';'
    { let vds = null;
      for (let ids = $2; ids != null; ids = ids->tl) {
        let id = ids->hd;
        let qv = new $((nmspace_t)(new Rel_n(null)), id);
        let vd = new_vardecl(qv,wildtyp(),null);
        vds = new List(vd,vds);
      }
      vds = List::imp_rev(vds);
      $$=^$(new List(letv_decl(vds,LOC(@1,@3)),null));
    }
;

declaration_list:
  declaration
    { $$=$!1; }
| declaration_list declaration
    { $$=^$(List::imp_append($1,$2)); }
;

/* Cyc: added optional attributes */
declaration_specifiers:
  storage_class_specifier attributes_opt
    { $$=^$(new Declaration_spec(new Opt($1),empty_tqual(),null,false,$2)); }
| storage_class_specifier attributes_opt declaration_specifiers
    { if ($3->sc != null)
        warn("Only one storage class is allowed in a declaration",LOC(@1,@2));
      $$=^$(new Declaration_spec(new Opt($1),$3->tq,$3->type_specs,
                                 $3->is_inline,
                                 List::imp_append($2,$3->attributes)));
    }
| type_specifier attributes_opt
    { $$=^$(new Declaration_spec(null,empty_tqual(),
                                 new List($1,null),false,$2)); }
| type_specifier attributes_opt declaration_specifiers
    { $$=^$(new Declaration_spec($3->sc,$3->tq,new List($1,$3->type_specs),
                                 $3->is_inline,
                                 List::imp_append($2,$3->attributes))); }
| type_qualifier attributes_opt
    { $$=^$(new Declaration_spec(null,$1,null,false,$2)); }
| type_qualifier attributes_opt declaration_specifiers
    { $$=^$(new Declaration_spec($3->sc,combine_tqual($1,$3->tq),
                                 $3->type_specs, $3->is_inline,
                                 List::imp_append($2,$3->attributes))); }

| INLINE attributes_opt
    { $$=^$(new Declaration_spec(null,empty_tqual(),null,true,$2)); }
| INLINE attributes_opt declaration_specifiers
    { $$=^$(new Declaration_spec($3->sc,$3->tq,$3->type_specs,true,
                              List::imp_append($2,$3->attributes)));
    }
;

storage_class_specifier:
  AUTO      { $$=^$(Auto_sc); }
| REGISTER  { $$=^$(Register_sc); }
| STATIC    { $$=^$(Static_sc); }
| EXTERN    { $$=^$(Extern_sc); }
| EXTERN STRING
  { if (strcmp($2,"C") != 0)
      err("only extern or extern \"C\" is allowed",LOC(@1,@2));
    $$ = ^$(ExternC_sc);
  }
| TYPEDEF   { $$=^$(Typedef_sc); }
/* Cyc:  abstract specifier */
| ABSTRACT  { $$=^$(Abstract_sc); }
;

/* Cyc: added GCC attributes */
attributes_opt:
  /* empty */  { $$=^$(null); }
  | attributes { $$=$!1; }
;

attributes:
  ATTRIBUTE '(' '(' attribute_list ')' ')'
  { $$=^$($4); }
;

attribute_list:
  attribute { $$=^$(new List($1,null)); };
| attribute ',' attribute_list { $$=^$(new List($1,$3)); }
;

attribute:
  IDENTIFIER
  { let s = $1;
    // drop the surrounding __ in s, if it's there
    if(s.size > 4 && s[0]=='_' && s[1]=='_'
       && s[s.size-2]=='_' && s[s.size-3]=='_')
      s = substring(s,2,s.size-5);
    int i=0;
    for(; i < att_map.size; ++i)
      if(strcmp(s,att_map[i][0]) == 0) {
	$$=^$(att_map[i][1]);
	break;
      }
    if(i == att_map.size) {
      err("unrecognized attribute",LOC(@1,@1));
      $$ = ^$(Cdecl_att);
    }
  }
| CONST { $$=^$(Const_att); }
| IDENTIFIER '(' INTEGER_CONSTANT ')'
  { let s = $1;
    let &$(_,n) = $3;
    attribute_t a;
    if (zstrcmp(s,"regparm") == 0 || zstrcmp(s,"__regparm__") == 0) {
      if (n <= 0 || n > 3) err("regparm requires value between 1 and 3",
                               LOC(@3,@3));
      a = new Regparm_att(n);
    } else if (zstrcmp(s,"aligned") == 0 || zstrcmp(s,"__aligned__") == 0) {
      if (n < 0) err("aligned requires positive power of two",
                     LOC(@3,@3));
      unsigned int j = (unsigned int)n;
      for (; (j & 1) == 0; j = j >> 1);
      j = j >> 1;
      if (j != 0) err("aligned requires positive power of two",LOC(@3,@3));
      a = new Aligned_att(n);
    }
    else {
      err("unrecognized attribute",LOC(@1,@1));
      a = Cdecl_att;
    }
    $$=^$(a);
  }
| IDENTIFIER '(' STRING ')'
  { let s = $1;
    let str = $3;
    attribute_t a;
    if (zstrcmp(s,"section") == 0 || zstrcmp(s,"__section__"))
      a = new Section_att(str);
    else {
      err("unrecognized attribute",LOC(@1,@1));
      a = Cdecl_att;
    }
    $$=^$(a);
  }
;

/***************************** TYPES *****************************/
/* we could be parsing a type or a type declaration (e.g., struct)
 * so we return a type_specifier value -- either a type, a type
 * qualifier, an integral type qualifier (signed, long, etc.)
 * or a declaration.
 */
type_specifier:
  VOID      { $$=^$(type_spec(VoidType,LOC(@1,@1))); }
| '_'       { $$=^$(type_spec(new_evar(MemKind),LOC(@1,@1))); }
| CHAR      { $$=^$(type_spec(uchar_t,LOC(@1,@1))); }
| SHORT     { $$=^$(new Short_spec(LOC(@1,@1))); }
| INT       { $$=^$(type_spec(sint_t,LOC(@1,@1))); }
| LONG      { $$=^$(new Long_spec(LOC(@1,@1))); }
| FLOAT     { $$=^$(type_spec(float_t,LOC(@1,@1))); }
| DOUBLE    { $$=^$(type_spec(double_t,LOC(@1,@1))); }
| SIGNED    { $$=^$(new Signed_spec(LOC(@1,@1))); }
| UNSIGNED  { $$=^$(new Unsigned_spec(LOC(@1,@1))); }
| enum_specifier { $$=$!1; }
| struct_or_union_specifier { $$=$!1; }
/* Cyc: added [x]tunions */
| tunion_specifier { $$=$!1; }
/* Cyc: added type variables and optional type parameters to typedef'd names */
| TYPE_VAR
  { $$=^$(type_spec(id2type($1,empty_conref()), LOC(@1,@1))); }
| TYPE_VAR COLON_COLON kind
   { $$=^$(type_spec(id2type($1,new_conref($3)),LOC(@1,@3))); }
| QUAL_TYPEDEF_NAME type_params_opt
    { $$=^$(type_spec(new TypedefType($1,$2,null),LOC(@1,@2))); }
/* Cyc: everything below here is an addition */
| '$' '(' parameter_list ')'
    { $$=^$(type_spec(new TupleType(List::map_c(get_tqual_typ,
                                                LOC(@3,@3),List::imp_rev($3))),
                      LOC(@1,@4))); }
| REGION_T '<' any_type_name '>'
    { $$=^$(type_spec(new RgnHandleType($3),LOC(@1,@4))); }
;

/* Cyc: new */
kind:
  IDENTIFIER { $$=^$(id_to_kind($1,LOC(@1,@1))); }
| QUAL_TYPEDEF_NAME
{ let $(nm, v) = *($1);
  if (nm != (nmspace_t)Loc_n) err("bad kind in type specifier",LOC(@1,@1));
  $$=^$(id_to_kind(*v,LOC(@1,@1)));
}
;

type_qualifier:
  CONST    { $$=^$(Tqual(true,false,false)); }
| VOLATILE { $$=^$(Tqual(false,true,false)); }
| RESTRICT { $$=^$(Tqual(false,false,true)); }
;

/* parsing of enum specifiers */
enum_specifier:
  ENUM qual_opt_identifier '{' enum_declaration_list '}'
  { $$=^$(new Decl_spec(new Decl{new Enum_d(new Enumdecl(Public,$2,new Opt($4))),
                                 LOC(@1,@5)})); }
| ENUM qual_opt_identifier
  { $$=^$(type_spec(new EnumType($2,null),LOC(@1,@2))); }
;

/* enum fields */
enum_field:
  qual_opt_identifier
  { $$ = ^$(new Enumfield($1,null,LOC(@1,@1))); }
| qual_opt_identifier '=' constant_expression
  { $$ = ^$(new Enumfield($1,$3,LOC(@1,@3))); }
;

enum_declaration_list:
  enum_field { $$ = ^$(new List($1,null)); }
| enum_field ',' enum_declaration_list { $$ = ^$(new List($1,$3)); }
;

/* parsing of struct and union specifiers. */
struct_or_union_specifier:
  struct_or_union '{' struct_declaration_list '}'
    { type_t t;
      switch ($1) {
      case Struct_su:
        t = new AnonStructType($3);
	break;
      case Union_su:
        t = new AnonUnionType($3);
	break;
      }
      $$=^$(new Type_spec(t,LOC(@1,@4)));
    }
/* Cyc:  type_params_opt are added */
| struct_or_union qual_opt_identifier type_params_opt
  '{' struct_declaration_list '}'
    { let ts = List::map_c(typ2tvar,LOC(@3,@3),$3);
      decl_t d;
      switch ($1) {
      case Struct_su:
        d = struct_decl(Public,new Opt($2),ts,new Opt($5),null,LOC(@1,@6));
	break;
      case Union_su:
        d = union_decl(Public,new Opt($2),ts,new Opt($5),null,LOC(@1,@6));
	break;
      }
      $$=^$(new Decl_spec(d));
    }
// Hack to allow struct/union names and typedef names to overlap
| struct_or_union QUAL_TYPEDEF_NAME type_params_opt
  '{' struct_declaration_list '}'
    { let ts = List::map_c(typ2tvar,LOC(@3,@3),$3);
      decl_t d;
      switch ($1) {
      case Struct_su:
        d = struct_decl(Public,new Opt($2),ts,new Opt($5),null,LOC(@1,@6));
	break;
      case Union_su:
        d = union_decl(Public,new Opt($2),ts,new Opt($5),null,LOC(@1,@6));
	break;
      }
      $$=^$(new Decl_spec(d));
    }
/* Cyc:  type_params_opt are added */
| struct_or_union qual_opt_identifier type_params_opt
    { switch ($1) {
      case Struct_su:
	$$=^$(type_spec(new StructType($2,$3,null),LOC(@1,@3)));
	break;
      case Union_su:
	$$=^$(type_spec(new UnionType($2,$3,null),LOC(@1,@3)));
	break;
      }
    }
// Hack to allow struct/union names and typedef names to overlap
| struct_or_union QUAL_TYPEDEF_NAME type_params_opt
    { switch ($1) {
      case Struct_su:
        $$=^$(type_spec(new StructType($2,$3,null),LOC(@1,@3)));
	break;
      case Union_su:
        $$=^$(type_spec(new UnionType($2,$3,null),LOC(@1,@3)));
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
  STRUCT { $$=^$(Struct_su); }
| UNION  { $$=^$(Union_su); }
;

struct_declaration_list:
  struct_declaration_list0 { $$=^$(List::flatten(List::imp_rev($1))); }
;

/* NB: returns list in reverse order */
struct_declaration_list0:
  struct_declaration
    { $$=^$(new List($1,null)); }
| struct_declaration_list0 struct_declaration
    { $$=^$(new List($2,$1)); }
;

init_declarator_list:
  init_declarator_list0 { $$=^$(List::imp_rev($1)); }
;

/* NB: returns list in reverse order */
init_declarator_list0:
  init_declarator
    { $$=^$(new List($1,null)); }
| init_declarator_list0 ',' init_declarator
    { $$=^$(new List($3,$1)); }
;

init_declarator:
  declarator
    { $$=^$(new $($1,null)); }
| declarator '=' initializer
    { $$=^$(new $($1,(exp_opt_t)$3)); } // FIX: cast needed
;

struct_declaration:
  specifier_qualifier_list struct_declarator_list ';'
    {
      /* when we collapse the specifier_qualifier_list and
       * struct_declarator_list, we get a list of (1) optional id,
       * (2) type, and (3) in addition any nested struct, union,
       * or [x]tunion declarations.  For now, we warn about the nested
       * declarations.  We must check that each id is actually present
       * and then convert this to a list of struct fields: (1) id,
       * (2) tqual, (3) type. */
      tqual_t tq = (*$1)[0];
      let atts = (*$1)[2];
      let t = speclist2typ((*$1)[1], LOC(@1,@1));
      let $(decls, widths) = List::split($2);
      let info = List::zip(apply_tmss(tq,t,decls,atts),widths);
      $$=^$(List::map_c(make_struct_field,LOC(@1,@2),info));
    }
;

specifier_qualifier_list:
  type_specifier attributes_opt
    // FIX: casts needed
    { $$=^$(new $(empty_tqual(),(list_t<type_specifier_t>)(new List($1,null)),
                  $2)); }
| type_specifier attributes_opt specifier_qualifier_list
    { $$=^$(new $((*$3)[0],(list_t<type_specifier_t>)(new List($1,(*$3)[1])),
                  List::append($2,(*$3)[2]))); }
| type_qualifier attributes_opt
    { $$=^$(new $($1,null,$2)); }
| type_qualifier attributes_opt specifier_qualifier_list
    { $$=^$(new $(combine_tqual($1,(*$3)[0]),(*$3)[1],
                  List::append($2,(*$3)[2]))); }

struct_declarator_list:
  struct_declarator_list0 { $$=^$(List::imp_rev($1)); }
;

/* NB: returns list in reverse order */
struct_declarator_list0:
  struct_declarator
    { $$=^$(new List($1,null)); }
| struct_declarator_list0 ',' struct_declarator
    { $$=^$(new List($3,$1)); }
;

struct_declarator:
  declarator
    { $$=^$(new $($1,null)); }
| ':' constant_expression
    {
      // HACK: give the field an empty name -- see elsewhere in the
      // compiler where we use this invariant
      $$=^$(new $((new Declarator(new $((nmspace_t)(new Rel_n(null)),new ""),
                                  null)),
                  new Opt($2)));
    }
| declarator ':' constant_expression
    { $$=^$(new $($1,new Opt($3))); }
;

tunion_specifier:
  tunion_or_xtunion qual_opt_identifier type_params_opt '{' tunionfield_list '}'
    { let ts = List::map_c(typ2tvar,LOC(@3,@3),$3);
      $$ = ^$(new Decl_spec(tunion_decl(Public, $2,ts,new Opt($5), $1,
					LOC(@1,@6))));
    }
| tunion_or_xtunion qual_opt_identifier type_params_opt
    {
      $$=^$(type_spec(new TunionType(TunionInfo(new
		       UnknownTunion(UnknownTunionInfo($2,$1)), $3, HeapRgn)),
		       LOC(@1,@3)));
    }
| tunion_or_xtunion rgn qual_opt_identifier type_params_opt
    {
      $$=^$(type_spec(new TunionType(TunionInfo(new
		       UnknownTunion(UnknownTunionInfo($3,$1)), $4, $2)),
		       LOC(@1,@4)));
    }
| tunion_or_xtunion qual_opt_identifier '.' qual_opt_identifier type_params_opt
    { $$=^$(type_spec(new TunionFieldType(TunionFieldInfo(new
		  UnknownTunionfield(UnknownTunionFieldInfo($2,$4,$1)),$5)),
		      LOC(@1,@5)));
    }
;

tunion_or_xtunion:
  TUNION  { $$=^$(false); }
| XTUNION { $$=^$(true);  }

tunionfield_list:
  tunionfield                      { $$=^$(new List($1,null)); }
| tunionfield ';'                  { $$=^$(new List($1,null)); }
| tunionfield ',' tunionfield_list { $$=^$(new List($1,$3)); }
| tunionfield ';' tunionfield_list { $$=^$(new List($1,$3)); }
;

tunionfield_scope:
                                   { $$=^$(Public);}
| EXTERN                           { $$=^$(Extern);}
| STATIC                           { $$=^$(Static);}

tunionfield:
  tunionfield_scope qual_opt_identifier
    { $$=^$(new Tunionfield($2,null,null,LOC(@1,@2),$1)); }
| tunionfield_scope qual_opt_identifier type_params_opt '(' parameter_list ')'
    { let typs = List::map_c(get_tqual_typ,LOC(@5,@5),List::imp_rev($5));
      let tvs  = List::map_c(typ2tvar,LOC(@3,@3),$3);
      $$=^$(new Tunionfield($2,tvs,typs,LOC(@1,@6),$1)); }
;

declarator:
  direct_declarator
    { $$=$!1; }
| pointer direct_declarator
    { $$=^$(new Declarator($2->id,List::imp_append($1,$2->tms))); }
;

direct_declarator:
  qual_opt_identifier
    { $$=^$(new Declarator($1,null)); }
| '(' declarator ')'
    { $$=$!2; }
| direct_declarator '[' ']'
    { $$=^$(new Declarator($1->id,new List(Carray_mod,$1->tms)));
    }
| direct_declarator '[' assignment_expression ']'
    { $$=^$(new Declarator($1->id,new List(new ConstArray_mod($3),$1->tms)));
    }
| direct_declarator '(' parameter_type_list ')'
    { let &$(lis,b,c,eff) = $3;
      $$=^$(new Declarator($1->id,new List(new Function_mod(new WithTypes(lis,b,c,eff)),$1->tms)));
    }
| direct_declarator '(' ')'
    { $$=^$(new Declarator($1->id,
                           new List(new Function_mod(new WithTypes(null,
                                                                   false,null,
                                                                   null)),
                                    $1->tms)));
    }
| direct_declarator '(' ';' effect_set ')'
    { $$=^$(new Declarator($1->id,
                           new List(new Function_mod(new WithTypes(null,false,null,new Opt(new JoinEff($4)))),$1->tms)));
    }
| direct_declarator '(' identifier_list ')'
    { $$=^$(new Declarator($1->id,new List(new Function_mod(new NoTypes($3,LOC(@1,@4))),$1->tms))); }
/* Cyc: added type parameters */
| direct_declarator '<' type_name_list '>'
    { let ts = List::map_c(typ2tvar,LOC(@2,@4),List::imp_rev($3));
      $$=^$(new Declarator($1->id,new List(new TypeParams_mod(ts,LOC(@1,@4),false),$1->tms)));
    }
| direct_declarator '<' type_name_list RIGHT_OP
    { /* RIGHT_OP is >>, we've seen one char too much, step back */
      lbuf->v->lex_curr_pos -= 1;
      let ts = List::map_c(typ2tvar,LOC(@2,@4),List::imp_rev($3));
      $$=^$(new Declarator($1->id,new List(new TypeParams_mod(ts,LOC(@1,@4),false),$1->tms)));
    }
| direct_declarator attributes
  {
    $$=^$(new Declarator($1->id,new List(new Attributes_mod(LOC(@2,@2),$2),
                                         $1->tms)));
  }
;

/* CYC: region annotations allowed */
pointer:
  pointer_char rgn_opt attributes_opt
    { $$=^$(new List(new Pointer_mod($1,$2,empty_tqual()),
                     attopt_to_tms(LOC(@3,@3),$3,null))); }
| pointer_char rgn_opt attributes_opt type_qualifier_list
    { $$=^$(new List(new Pointer_mod($1,$2,$4),
                     attopt_to_tms(LOC(@3,@3),$3,null))); }
| pointer_char rgn_opt attributes_opt pointer
    { $$=^$(new List(new Pointer_mod($1,$2,empty_tqual()),
                     attopt_to_tms(LOC(@3,@3),$3,$4))); }
| pointer_char rgn_opt attributes_opt type_qualifier_list pointer
    { $$=^$(new List(new Pointer_mod($1,$2,$4),
                     attopt_to_tms(LOC(@3,@3),$3,$5))); }
;

pointer_char:
  '*' { $$=^$(new Nullable_ps(signed_int_exp(1,LOC(@1,@1))));  }
/* CYC: pointers that cannot be null */
| '@' { $$=^$(new NonNullable_ps(signed_int_exp(1,LOC(@1,@1)))); }
/* possibly null, with array bound given by the expresion */
| '*' '{' assignment_expression '}' {$$=^$(new Nullable_ps($3));}
/* not null, with array bound given by the expresion */
| '@' '{' assignment_expression '}' {$$=^$(new NonNullable_ps($3));}
/* tagged pointer -- bounds maintained dynamically */
| '?' { $$=^$(TaggedArray_ps); }

rgn_opt:
    /* empty */ { $$ = ^$(HeapRgn); }
| rgn { $$ = $!1; };

rgn:
  TYPE_VAR
  { $$ = ^$(id2type($1,new_conref(RgnKind))); }
| TYPE_VAR COLON_COLON kind
  { if ($3 != (kind_t)RgnKind) err("expecting region kind\n",LOC(@3,@3));
    $$ = ^$(id2type($1,new_conref(RgnKind)));
  }
| '_' { $$ = ^$(new_evar(RgnKind)); }


type_qualifier_list:
  type_qualifier
    { $$ = $!1; }
| type_qualifier_list type_qualifier
    { $$ = ^$(combine_tqual($1,$2)); }
;

parameter_type_list:
  parameter_list optional_effect
    { $$=^$(new $(List::imp_rev($1),false,null,$2)); }
| parameter_list ',' ELLIPSIS optional_effect
    { $$=^$(new $(List::imp_rev($1),true,null,$4)); }
| ELLIPSIS rgn optional_inject parameter_declaration optional_effect
{ let $(n,tq,t) = *($4);
  let v = new VarargInfo {.name = n,.tq = tq,.type = t,.rgn = $2,.inject = $3};
  $$=^$(new $(null,false,v,$5)); 
}
| parameter_list ',' ELLIPSIS rgn optional_inject parameter_declaration
  optional_effect
{ let $(n,tq,t) = *($6);
  let v = new VarargInfo {.name = n,.tq = tq,.type = t,.rgn = $4,.inject = $5};
  $$=^$(new $(List::imp_rev($1),false,v,$7)); 
}
;

/* CYC:  new */
optional_effect:
  /* empty */
  { $$=^$(null); }
| ';' effect_set
  { $$ = ^$(new Opt((type_t)(new JoinEff($2)))); }
;


optional_inject:
  /* empty */
  {$$ = ^$(false);}
| IDENTIFIER
     { let s = $1;
       if (zstrcmp(s,"inject") != 0) 
         err("missing type in function declaration",LOC(@1,@1));
       $$ = ^$(true);
     }
   ;

effect_set:
  atomic_effect { $$=$!1; }
| atomic_effect '+' effect_set { $$=^$(List::imp_append($1,$3)); }
;

atomic_effect:
  '{' '}'                   { $$=^$(null); }
| '{' region_set '}'        { $$=$!2; }
| TYPE_VAR
  { $$=^$(new List(id2type($1,new_conref(EffKind)),null)); }
| TYPE_VAR COLON_COLON kind
  { if ($3 != (kind_t)EffKind) err("expecing effect kind (E)",LOC(@3,@3));
    $$=^$(new List(id2type($1,new_conref(EffKind)),null));
  }
;

/* CYC:  new */
region_set:
  TYPE_VAR
  { $$=^$(new List(new AccessEff(id2type($1,new_conref(RgnKind))),null)); }
| TYPE_VAR ',' region_set
  { $$=^$(new List(new AccessEff(id2type($1,new_conref(RgnKind))),$3)); }
| TYPE_VAR COLON_COLON kind
  { if ($3 != (kind_t)RgnKind) err("expecting region kind (R)", LOC(@3,@3));
    $$=^$(new List(new AccessEff(id2type($1,new_conref(RgnKind))),null)); }
| TYPE_VAR COLON_COLON kind ',' region_set
  { if ($3 != (kind_t)RgnKind) err("expecting region kind (R)", LOC(@3,@3));
    $$=^$(new List(new AccessEff(id2type($1,new_conref(RgnKind))),$5)); }
;

/* NB: returns list in reverse order */
parameter_list:
  parameter_declaration
    { $$=^$(new List($1,null)); }
| parameter_list ',' parameter_declaration
    { $$=^$(new List($3,$1)); }
;

/* TODO: differs from grammar in K&R */
parameter_declaration:
  specifier_qualifier_list declarator
   {  let t   = speclist2typ((*$1)[1], LOC(@1,@1));
      let atts = (*$1)[2];
      let tq  = (*$1)[0];
      let tms = $2->tms;
      let t_info = apply_tms(tq,t,atts,tms);
      if (t_info[2] != null)
        err("parameter with bad type params",LOC(@2,@2));
      let q = $2->id;
      switch ((*q)[0]) {
      case Loc_n: break;
      case &Rel_n(null): break;
      case &Abs_n(null): break;
      default:
        err("parameter cannot be qualified with a module name",LOC(@1,@1));
        break;
      }
      let idopt = (opt_t<var_t>)(new Opt((*q)[1]));
      if (t_info[3] != null)
        warn("extra attributes on parameter, ignoring",LOC(@1,@2));
      if (t_info[2] != null)
        warn("extra type variables on parameter, ignoring",LOC(@1,@2));
      $$=^$(new $(idopt,t_info[0],t_info[1]));
    }
| specifier_qualifier_list
    { let t  = speclist2typ((*$1)[1], LOC(@1,@1));
      let atts = (*$1)[2];
      let tq = (*$1)[0];
      if (atts != null)
        warn("bad attributes on parameter, ignoring",LOC(@1,@1));
      $$=^$(new $(null,tq,t));
    }
| specifier_qualifier_list abstract_declarator
    { let t   = speclist2typ((*$1)[1], LOC(@1,@1));
      let atts = (*$1)[2];
      let tq  = (*$1)[0];
      let tms = $2->tms;
      let t_info = apply_tms(tq,t,atts,tms);
      if (t_info[2] != null)
        // Ex: int (@)<`a>
        warn("bad type parameters on formal argument, ignoring",LOC(@1,@2));
      if (t_info[3] != null)
        warn("bad attributes on parameter, ignoring",LOC(@1,@2));
      $$=^$(new $(null,t_info[0],t_info[1]));
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
    { $$=^$(new List(new {$1},null)); }
| identifier_list0 ',' IDENTIFIER
    { $$=^$(new List(new {$3},$1)); }
;

initializer:
  assignment_expression
    { $$=$!1; }
| array_initializer
    { $$=$!1; }
;

array_initializer:
  '{' '}'
    { $$=^$(new_exp(new UnresolvedMem_e(null,null),LOC(@1,@2))); }
| '{' initializer_list '}'
    { $$=^$(new_exp(new UnresolvedMem_e(null,List::imp_rev($2)),LOC(@1,@3))); }
| '{' initializer_list ',' '}'
    { $$=^$(new_exp(new UnresolvedMem_e(null,List::imp_rev($2)),LOC(@1,@4))); }
| '{' FOR IDENTIFIER '<' expression ':' expression '}'
    { let vd = new_vardecl(new $((nmspace_t)Loc_n,new {$3}), uint_t,
                           uint_exp(0,LOC(@3,@3)));
      // make the index variable const
      vd->tq = Tqual{.q_const = true, .q_volatile = false,
		     .q_restrict = true};
      $$=^$(new_exp(new Comprehension_e(vd, $5, $7),LOC(@1,@8)));
    }

;

/* NB: returns list in reverse order */
initializer_list:
  initializer
    { $$=^$(new List(new $(null,$1),null)); }
| designation initializer
    { $$=^$(new List(new $($1,$2),null)); }
| initializer_list ',' initializer
    { $$=^$(new List(new $(null,$3),$1)); }
| initializer_list ',' designation initializer
    { $$=^$(new List(new $($3,$4),$1)); }
;

designation:
  designator_list '=' { $$=^$(List::imp_rev($1)); }
;

/* NB: returns list in reverse order */
designator_list:
  designator
    { $$=^$(new List($1,null)); }
| designator_list designator
    { $$=^$(new List($2,$1)); }
;

designator:
  '[' constant_expression ']' {$$ = ^$(new ArrayElement($2));}
| '.' IDENTIFIER              {$$ = ^$(new FieldName(new {$2}));}
;

type_name:
  specifier_qualifier_list
    { let t  = speclist2typ((*$1)[1], LOC(@1,@1));
      let atts = (*$1)[2];
      if (atts != null)
        warn("ignoring attributes in type",LOC(@1,@1));
      let tq = (*$1)[0];
      $$=^$(new{$(null,tq,t)});
    }
| specifier_qualifier_list abstract_declarator
    { let t   = speclist2typ((*$1)[1], LOC(@1,@1));
      let atts = (*$1)[2];
      let tq  = (*$1)[0];
      let tms = $2->tms;
      let t_info = apply_tms(tq,t,atts,tms);
      if (t_info[2] != null)
        // Ex: int (@)<`a>
        warn("bad type params, ignoring",LOC(@2,@2));
      if (t_info[3] != null)
        warn("bad specifiers, ignoring",LOC(@2,@2));
      $$=^$(new $(null,t_info[0],t_info[1]));
    }
;

any_type_name:
  type_name { $$ = ^$((*$1)[2]); }
| '{' '}' { $$ = ^$(new JoinEff(null)); }
| '{' region_set '}' { $$ = ^$(new JoinEff($2)); }
| any_type_name '+' atomic_effect { $$ = ^$(new JoinEff(new List($1,$3))); }
;

/* Cyc: new */
/* NB: returns list in reverse order */
type_name_list:
  any_type_name { $$=^$(new List($1,null)); }
| type_name_list ',' any_type_name {$$=^$(new List($3,$1)); }
;

abstract_declarator:
  pointer
    { $$=^$(new Abstractdeclarator($1)); }
| direct_abstract_declarator
    { $$=$!1; }
| pointer direct_abstract_declarator
    { $$=^$(new Abstractdeclarator(List::imp_append($1,$2->tms))); }
;

direct_abstract_declarator:
  '(' abstract_declarator ')'
    { $$=$!2; }
| '[' ']'
    { $$=^$(new Abstractdeclarator(new List(Carray_mod,null))); }
| direct_abstract_declarator '[' ']'
    { $$=^$(new Abstractdeclarator(new List(Carray_mod,$1->tms))); }
| '[' assignment_expression ']'
    { $$=^$(new Abstractdeclarator(new List(new ConstArray_mod($2),null))); }
| direct_abstract_declarator '[' assignment_expression ']'
    { $$=^$(new Abstractdeclarator(new List(new ConstArray_mod($3),$1->tms)));
    }
| '(' ')'
    { $$=^$(new Abstractdeclarator(new List(new Function_mod(new WithTypes(null,false,null,null)),null)));
    }
| '(' ';' effect_set ')'
    { $$=^$(new Abstractdeclarator(new List(new Function_mod(new WithTypes(null,false,null,new Opt(new JoinEff($3)))),null)));
    }
| '(' parameter_type_list ')'
    { let &$(lis,b,c,eff) = $2;
      $$=^$(new Abstractdeclarator(new List(new Function_mod(new WithTypes(lis,b,c,eff)),null)));
    }
| direct_abstract_declarator '(' ')'
    { $$=^$(new Abstractdeclarator(new List(new Function_mod(new WithTypes(null,false,null,null)),
				      $1->tms)));
    }
| direct_abstract_declarator '(' ';' effect_set ')'
   { let eff = new Opt((type_t)(new JoinEff($4)));
      $$=^$(new Abstractdeclarator(new List(new Function_mod(new WithTypes(null,false,null,eff)),$1->tms)));
    }
| direct_abstract_declarator '(' parameter_type_list ')'
    { let &$(lis,b,c,eff) = $3;
      $$=^$(new Abstractdeclarator(new List(new Function_mod(new WithTypes(lis,
                                                                           b,c,eff)),$1->tms)));
    }
/* Cyc: new */
| direct_abstract_declarator '<' type_name_list '>'
    { let ts = List::map_c(typ2tvar,LOC(@2,@4),List::imp_rev($3));
      $$=^$(new Abstractdeclarator(new List(new TypeParams_mod(ts,LOC(@2,@4),
                                                               false),
                                      $1->tms)));
    }
| direct_abstract_declarator '<' type_name_list RIGHT_OP
    { /* RIGHT_OP is >>, we've seen one char too much, step back */
      lbuf->v->lex_curr_pos -= 1;
      let ts = List::map_c(typ2tvar,LOC(@2,@4),List::imp_rev($3));
      $$=^$(new Abstractdeclarator(new List(new TypeParams_mod(ts,LOC(@2,@4),
                                                               false),
                                            $1->tms)));
    }
| direct_abstract_declarator attributes
    {
      $$=^$(new Abstractdeclarator(new List(new Attributes_mod(LOC(@2,@2),$2),
                                            $1->tms)));
    }
;

/***************************** STATEMENTS *****************************/
statement:
  labeled_statement     { $$=$!1; }
| expression_statement  { $$=$!1; }
| compound_statement    { $$=$!1; }
| selection_statement   { $$=$!1; }
| iteration_statement   { $$=$!1; }
| jump_statement        { $$=$!1; }
/* Cyc: region statements */
| REGION '<' TYPE_VAR '>' IDENTIFIER statement
  { if (zstrcmp($3,"`H") == 0)
      err("bad occurrence of heap region `H",LOC(@3,@3));
    tvar_t tv = new Tvar(new $3,new_conref(RgnKind));
    type_t t = new VarType(tv);
    $$=^$(new_stmt(new Region_s(tv,new_vardecl(new $((nmspace_t)Loc_n, new $5),
                                               new RgnHandleType(t),null),$6),
                   LOC(@1,@6)));
  }
| REGION IDENTIFIER statement
  { if (zstrcmp($2,"H") == 0)
      err("bad occurrence of heap region `H",LOC(@3,@3));
    tvar_t tv = new Tvar(new xprintf("`%s",$2),new_conref(RgnKind));
    type_t t = new VarType(tv);
    $$=^$(new_stmt(new Region_s(tv,new_vardecl(new $((nmspace_t)Loc_n, new $2),
                                               new RgnHandleType(t),null),$3),
                   LOC(@1,@3)));
  }
/* Cyc: cut and splice */
| CUT statement         { $$=^$(new_stmt(new Cut_s($2),LOC(@1,@2))); }
| SPLICE statement      { $$=^$(new_stmt(new Splice_s($2),LOC(@1,@2))); }
;

/* Cyc: Unlike C, we do not treat case and default statements as
   labeled */
labeled_statement:
  IDENTIFIER ':' statement
    { $$=^$(new_stmt(new Label_s(new {$1},$3),LOC(@1,@3))); }
;

expression_statement:
  ';'            { $$=^$(skip_stmt(LOC(@1,@1))); }
| expression ';' { $$=^$(exp_stmt($1,LOC(@1,@2))); }
;

compound_statement:
  '{' '}'                 { $$=^$(skip_stmt(LOC(@1,@2))); }
| '{' block_item_list '}' { $$=$!2; }
;

block_item_list:
  block_item
    { switch ($1) {
      case &TopDecls_bl(ds):
        $$=^$(flatten_declarations(ds,skip_stmt(LOC(@1,@1))));
	break;
      case &FnDecl_bl(fd):
        $$=^$(flatten_decl(new_decl(new Fn_d(fd),LOC(@1,@1)),
			   skip_stmt(LOC(@1,@1))));
	break;
      case &Stmt_bl(s):
        $$=^$(s);
	break;
      }
    }
| block_item block_item_list
    { switch ($1) {
      case &TopDecls_bl(ds):
        $$=^$(flatten_declarations(ds,$2));
	break;
      case &FnDecl_bl(fd):
	$$=^$(flatten_decl(new_decl(new Fn_d(fd),LOC(@1,@1)),$2));
	break;
      case &Stmt_bl(s):
        $$=^$(seq_stmt(s,$2,LOC(@1,@2)));
	break;
      }
    }
;

block_item:
  declaration { $$=^$(new TopDecls_bl($1)); }
| statement   { $$=^$(new Stmt_bl($1)); }
/* Cyc: nested function definitions.
   The initial (return) type is required,
   to avoid parser conflicts. */
| function_definition2 { $$=^$(new FnDecl_bl($1)); }

/* This has the standard shift-reduce conflict which is properly resolved. */
selection_statement:
  IF '(' expression ')' statement
    { $$=^$(ifthenelse_stmt($3,$5,skip_stmt(DUMMYLOC),LOC(@1,@5))); }
| IF '(' expression ')' statement ELSE statement
    { $$=^$(ifthenelse_stmt($3,$5,$7,LOC(@1,@7))); }
/* Cyc: the body of the switch statement cannot be an arbitrary
 * statement; it must be a list of switch_clauses */
| SWITCH '(' expression ')' '{' switch_clauses '}'
    { $$=^$(switch_stmt($3,$6,LOC(@1,@7))); }
| TRY statement CATCH '{' switch_clauses '}'
    { $$=^$(trycatch_stmt($2,$5,LOC(@1,@6))); }
;

/* Cyc: unlike C, we only allow default or case statements within
 * switches.  Also unlike C, we support a more general form of pattern
 * matching within cases. */
switch_clauses:
  /* empty */
    { $$=^$(null); }
| DEFAULT ':' block_item_list
    { $$=^$(new List(new Switch_clause(new_pat(Wild_p,LOC(@1,@1)),null,
                                       null,$3,LOC(@1,@3)),
                  null));}
| CASE pattern ':' switch_clauses
    { $$=^$(new List(new Switch_clause($2,null,null,skip_stmt(LOC(@3,@3)),
                                       LOC(@1,@4)),$4)); }
| CASE pattern ':' block_item_list switch_clauses
    { $$=^$(new List(new Switch_clause($2,null,null,$4,LOC(@1,@4)),$5)); }
| CASE pattern AND_OP expression ':' switch_clauses
    { $$=^$(new List(new Switch_clause($2,null,$4,skip_stmt(LOC(@5,@5)),
                                       LOC(@1,@6)),$6)); }
| CASE pattern AND_OP expression ':' block_item_list switch_clauses
    { $$=^$(new List(new Switch_clause($2,null,$4,$6,LOC(@1,@7)),$7)); }
;

iteration_statement:
  WHILE '(' expression ')' statement
    { $$=^$(while_stmt($3,$5,LOC(@1,@5))); }
| DO statement WHILE '(' expression ')' ';'
    { $$=^$(do_stmt($2,$5,LOC(@1,@7))); }
| FOR '(' ';' ';' ')' statement
    { $$=^$(for_stmt(false_exp(DUMMYLOC),true_exp(DUMMYLOC),false_exp(DUMMYLOC),
		     $6,LOC(@1,@6))); }
| FOR '(' ';' ';' expression ')' statement
    { $$=^$(for_stmt(false_exp(DUMMYLOC),true_exp(DUMMYLOC),$5,
		     $7,LOC(@1,@7))); }
| FOR '(' ';' expression ';' ')' statement
    { $$=^$(for_stmt(false_exp(DUMMYLOC),$4,false_exp(DUMMYLOC),
		     $7,LOC(@1,@7)));}
| FOR '(' ';' expression ';' expression ')' statement
    { $$=^$(for_stmt(false_exp(DUMMYLOC),$4,$6,
		     $8,LOC(@1,@7))); }
| FOR '(' expression ';' ';' ')' statement
    { $$=^$(for_stmt($3,true_exp(DUMMYLOC),false_exp(DUMMYLOC),
		     $7,LOC(@1,@7))); }
| FOR '(' expression ';' ';' expression ')' statement
    { $$=^$(for_stmt($3,true_exp(DUMMYLOC),$6,
		     $8,LOC(@1,@8))); }
| FOR '(' expression ';' expression ';' ')' statement
    { $$=^$(for_stmt($3,$5,false_exp(DUMMYLOC),
		     $8,LOC(@1,@8))); }
| FOR '(' expression ';' expression ';' expression ')' statement
    { $$=^$(for_stmt($3,$5,$7,
		     $9,LOC(@1,@9))); }
| FOR '(' declaration ';' ')' statement
    { let decls = $3;
      let s = for_stmt(false_exp(DUMMYLOC),true_exp(DUMMYLOC),false_exp(DUMMYLOC),
		     $6,LOC(@1,@6));
      $$=^$(flatten_declarations(decls,s));
    }
| FOR '(' declaration expression ';' ')' statement
    { let decls = $3;
      let s     = for_stmt(false_exp(DUMMYLOC),$4,false_exp(DUMMYLOC),
                           $7,LOC(@1,@7));
      $$=^$(flatten_declarations(decls,s));
    }
| FOR '(' declaration ';' expression ')' statement
    { let decls = $3;
      let s     = for_stmt(false_exp(DUMMYLOC),true_exp(DUMMYLOC),$5,
                           $7,LOC(@1,@7));
      $$=^$(flatten_declarations(decls,s));
    }
| FOR '(' declaration expression ';' expression ')' statement
    { let decls = $3;
      let s     = for_stmt(false_exp(DUMMYLOC),$4,$6,
                           $8,LOC(@1,@8));
      $$=^$(flatten_declarations(decls,s));
    }
;

jump_statement:
  GOTO IDENTIFIER ';'   { $$=^$(goto_stmt(new {$2},LOC(@1,@2))); }
| CONTINUE ';'          { $$=^$(continue_stmt(LOC(@1,@1)));}
| BREAK ';'             { $$=^$(break_stmt(LOC(@1,@1)));}
| RETURN ';'            { $$=^$(return_stmt(null,LOC(@1,@1)));}
| RETURN expression ';' { $$=^$(return_stmt($2,LOC(@1,@2)));}
/* Cyc:  explicit fallthru for switches */
| FALLTHRU ';'          { $$=^$(fallthru_stmt(null,LOC(@1,@1)));}
| FALLTHRU '(' ')' ';'  { $$=^$(fallthru_stmt(null,LOC(@1,@1)));}
| FALLTHRU '(' argument_expression_list ')' ';'
                        { $$=^$(fallthru_stmt($3,LOC(@1,@1)));}
;

/***************************** PATTERNS *****************************/
/* Cyc:  patterns */
pattern:
  '_'
    { $$=^$(new_pat(Wild_p,LOC(@1,@1)));}
| '(' pattern ')'
    { $$=$!2; }
| INTEGER_CONSTANT
    { $$=^$(new_pat(new Int_p((*$1)[0],(*$1)[1]),LOC(@1,@1))); }
| '-' INTEGER_CONSTANT
    { $$=^$(new_pat(new Int_p(Signed,-((*$2)[1])),LOC(@1,@2))); }
| FLOATING_CONSTANT
    { $$=^$(new_pat(new Float_p($1),LOC(@1,@1)));}
/* TODO: we should allow negated floating constants too */
| CHARACTER_CONSTANT
    {$$=^$(new_pat(new Char_p($1),LOC(@1,@1)));}
| NULL_kw
    {$$=^$(new_pat(Null_p,LOC(@1,@1)));}
| qual_opt_identifier
    { $$=^$(new_pat(new UnknownId_p($1),LOC(@1,@1))); }
| qual_opt_identifier type_params_opt '(' tuple_pattern_list ')'
    { let tvs = List::map_c(typ2tvar,LOC(@2,@2),$2);
      $$=^$(new_pat(new UnknownCall_p($1,tvs,$4),LOC(@1,@5)));
    }
| '$' '(' tuple_pattern_list ')'
    {$$=^$(new_pat(new Tuple_p($3),LOC(@1,@4)));}
| qual_opt_identifier type_params_opt '{' '}'
    { let tvs = List::map_c(typ2tvar,LOC(@2,@2),$2);
      $$=^$(new_pat(new UnknownFields_p($1,tvs,null),LOC(@1,@4)));
    }
| qual_opt_identifier type_params_opt '{' field_pattern_list '}'
    { let tvs = List::map_c(typ2tvar,LOC(@2,@2),$2);
      $$=^$(new_pat(new UnknownFields_p($1,tvs,$4),LOC(@1,@5)));
    }
| '&' pattern
    {$$=^$(new_pat(new Pointer_p($2),LOC(@1,@2)));}
| '*' IDENTIFIER
    {$$=^$(new_pat(new Reference_p(new_vardecl(new $((nmspace_t)Loc_n,
                                                     new {$2}),
                                               VoidType,null)),
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
    {$$=^$(new List($1,null));}
| tuple_pattern_list0 ',' pattern
    {$$=^$(new List($3,$1));}
;

field_pattern:
  pattern
    {$$=^$(new $(null,$1));}
| designation pattern
    {$$=^$(new $($1,$2));}

field_pattern_list:
  field_pattern_list0
    {$$=^$(List::imp_rev($1));}
;

field_pattern_list0:
  field_pattern
    { $$=^$(new List($1,null));}
| field_pattern_list0 ',' field_pattern
    {$$=^$(new List($3,$1)); }
;
/*
struct_pattern_list:
  struct_pattern_list0
    {$$=^$(List::imp_rev($1));}
;

struct_pattern_list0:
  struct_field_pattern
    {$$=^$(new List($1,null));}
| struct_pattern_list0 ',' struct_field_pattern
    {$$=^$(new List($3,$1)); }
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
  '='          { $$=^$(null); }
| MUL_ASSIGN   { $$=^$(new Opt(Times)); }
| DIV_ASSIGN   { $$=^$(new Opt(Div)); }
| MOD_ASSIGN   { $$=^$(new Opt(Mod)); }
| ADD_ASSIGN   { $$=^$(new Opt(Plus)); }
| SUB_ASSIGN   { $$=^$(new Opt(Minus)); }
| LEFT_ASSIGN  { $$=^$(new Opt(Bitlshift)); }
| RIGHT_ASSIGN { $$=^$(new Opt(Bitlrshift)); }
| AND_ASSIGN   { $$=^$(new Opt(Bitand)); }
| XOR_ASSIGN   { $$=^$(new Opt(Bitxor)); }
| OR_ASSIGN    { $$=^$(new Opt(Bitor)); }
;

conditional_expression:
  logical_or_expression
    { $$=$!1; }
| logical_or_expression '?' expression ':' conditional_expression
    { $$=^$(conditional_exp($1,$3,$5,LOC(@1,@5))); }
/* Cyc: throw expressions */
| THROW conditional_expression
    { $$=^$(throw_exp($2,LOC(@1,@2))); }
/* Cyc: expressions to build heap-allocated objects and arrays, throw */
| NEW array_initializer
    { $$=^$(New_exp(null,$2,LOC(@1,@3))); }
| NEW logical_or_expression
    { $$=^$(New_exp(null,$2,LOC(@1,@3))); }
| RNEW '(' expression ')' array_initializer
    { $$=^$(New_exp($3,$5,LOC(@1,@5))); }
| RNEW '(' expression ')' logical_or_expression
    { $$=^$(New_exp($3,$5,LOC(@1,@5))); }
;

constant_expression:
  conditional_expression
    { $$=$!1; }
;

logical_or_expression:
  logical_and_expression
    { $$=$!1; }
| logical_or_expression OR_OP logical_and_expression
    { $$=^$(or_exp($1,$3,LOC(@1,@3))); }
;

logical_and_expression:
  inclusive_or_expression
    { $$=$!1; }
| logical_and_expression AND_OP inclusive_or_expression
    { $$=^$(and_exp($1,$3,LOC(@1,@3))); }
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
    { $$=^$(eq_exp($1,$3,LOC(@1,@3))); }
| equality_expression NE_OP relational_expression
    { $$=^$(neq_exp($1,$3,LOC(@1,@3))); }
;

relational_expression:
  shift_expression
    { $$=$!1; }
| relational_expression '<' shift_expression
    { $$=^$(lt_exp($1,$3,LOC(@1,@3))); }
| relational_expression '>' shift_expression
    { $$=^$(gt_exp($1,$3,LOC(@1,@3))); }
| relational_expression LE_OP shift_expression
    { $$=^$(lte_exp($1,$3,LOC(@1,@3))); }
| relational_expression GE_OP shift_expression
    { $$=^$(gte_exp($1,$3,LOC(@1,@3))); }
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
    { $$=^$(cast_exp((*$2)[2],$4,LOC(@1,@4))); }
;

unary_expression:
  postfix_expression      { $$=$!1; }
| INC_OP unary_expression { $$=^$(pre_inc_exp($2,LOC(@1,@2))); }
| DEC_OP unary_expression { $$=^$(pre_dec_exp($2,LOC(@1,@2))); }
| '&' cast_expression     { $$=^$(address_exp($2,LOC(@1,@2))); }
| '*' cast_expression     { $$=^$(deref_exp  ($2,LOC(@1,@2))); }
| '+' cast_expression     { $$=$!2; }
| unary_operator cast_expression { $$=^$(prim1_exp($1,$2,LOC(@1,@2))); }
| SIZEOF '(' type_name ')'       { $$=^$(sizeoftyp_exp((*$3)[2],LOC(@1,@4))); }
| SIZEOF unary_expression        { $$=^$(sizeofexp_exp($2,LOC(@1,@2))); }
/* Cyc: throw, printf, fprintf, sprintf */
| format_primop '(' argument_expression_list ')'
    { $$=^$(primop_exp($1,$3,LOC(@1,@4))); }
| MALLOC '(' SIZEOF '(' specifier_qualifier_list ')' ')'
{ $$=^$(new_exp(new Malloc_e(null,speclist2typ((*$5)[1],LOC(@5,@5))),
                LOC(@1,@7))); }
| RMALLOC '(' assignment_expression ',' SIZEOF '(' specifier_qualifier_list ')' ')'
{ $$=^$(new_exp(new Malloc_e($3,speclist2typ((*$7)[1],LOC(@7,@7))),
                LOC(@1,@9))); }

;

format_primop:
  PRINTF  { $$=^$(Printf); }
| FPRINTF { $$=^$(Fprintf); }
| XPRINTF { $$=^$(Xprintf); }
| SCANF   { $$=^$(Scanf); }
| FSCANF  { $$=^$(Fscanf); }
| SSCANF  { $$=^$(Sscanf); }
;

unary_operator:
  '~' { $$=^$(Bitnot); }
| '!' { $$=^$(Not); }
| '-' { $$=^$(Minus); }
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
    { $$=^$(structmember_exp($1,new {$3},LOC(@1,@3))); }
// Hack to allow typedef names and field names to overlap
| postfix_expression '.' QUAL_TYPEDEF_NAME
    { qvar_t q = $3;
      switch ((*q)[0]) {
      case Loc_n: break;
      case &Rel_n(null): break;
      case &Abs_n(null): break;
      default:
        err("struct field name is qualified",LOC(@3,@3));
        break;
      }
      $$=^$(structmember_exp($1,(*q)[1],LOC(@1,@3)));
    }
| postfix_expression PTR_OP IDENTIFIER
    { $$=^$(structarrow_exp($1,new {$3},LOC(@1,@3))); }
// Hack to allow typedef names and field names to overlap
| postfix_expression PTR_OP QUAL_TYPEDEF_NAME
    { qvar_t q = $3;
      switch ((*q)[0]) {
      case Loc_n: break;
      case &Rel_n(null): break;
      case &Abs_n(null): break;
      default:
        err("struct field is qualified with module name",LOC(@3,@3));
        break;
      }
      $$=^$(structarrow_exp($1,(*q)[1],LOC(@1,@3)));
    }
| postfix_expression INC_OP
    { $$=^$(post_inc_exp($1,LOC(@1,@2))); }
| postfix_expression DEC_OP
    { $$=^$(post_dec_exp($1,LOC(@1,@2))); }
| '(' type_name ')' '{' initializer_list '}'
    { $$=^$(new_exp(new CompoundLit_e($2,List::imp_rev($5)),LOC(@1,@6))); }
| '(' type_name ')' '{' initializer_list ',' '}'
    { $$=^$(new_exp(new CompoundLit_e($2,List::imp_rev($5)),LOC(@1,@7))); }
/* Cyc: added fill and codegen */
| FILL '(' expression ')'
    { $$=^$(new_exp(new Fill_e($3),LOC(@1,@4))); }
| CODEGEN '(' function_definition ')'
    { $$=^$(new_exp(new Codegen_e($3),LOC(@1,@4))); }
;

primary_expression:
  qual_opt_identifier
    /* This could be an ordinary identifier, a struct tag, or a tunion or
       xtunion constructor */
    { $$=^$(new_exp(new UnknownId_e($1),LOC(@1,@1))); }
| constant
    { $$= $!1; }
| STRING
    { $$=^$(string_exp($1,LOC(@1,@1))); }
| '(' expression ')'
    { $$= $!2; }
/* Cyc: stop instantiation */
| qual_opt_identifier LEFT_RIGHT
    { $$=^$(noinstantiate_exp(new_exp(new UnknownId_e($1),
                                      LOC(@1,@1)),LOC(@1,@2)));}
| qual_opt_identifier '@' '<' type_name_list '>'
    { $$=^$(instantiate_exp(new_exp(new UnknownId_e($1),LOC(@1,@1)),
			    List::imp_rev($4),LOC(@1,@5))); }
/* Cyc: tuple expressions */
| '$' '(' argument_expression_list ')'
    { $$=^$(tuple_exp($3,LOC(@1,@4))); }
/* Cyc: structure expressions */
| qual_opt_identifier '{' initializer_list '}'
    { $$=^$(new_exp(new Struct_e($1,null,List::imp_rev($3),null),LOC(@1,@4)));
    }
/* Cyc: compound statement expressions, as in gcc */
| '(' '{' block_item_list '}' ')'
    { $$=^$(stmt_exp($3,LOC(@1,@5))); }
;

argument_expression_list:
  argument_expression_list0 { $$=^$(List::imp_rev($1)); }
;

/* NB: returns list in reverse order */
argument_expression_list0:
  assignment_expression
    { $$=^$(new List($1,null)); }
| argument_expression_list0 ',' assignment_expression
    { $$=^$(new List($3,$1)); }
;

/* NB: We've had to move tunioneration constants into primary_expression
   because the lexer can't tell them from ordinary identifiers */
constant:
  INTEGER_CONSTANT   { $$=^$(int_exp((*$1)[0],(*$1)[1],LOC(@1,@1))); }
| CHARACTER_CONSTANT { $$=^$(char_exp($1,        LOC(@1,@1))); }
| FLOATING_CONSTANT  { $$=^$(float_exp($1,       LOC(@1,@1))); }
/* Cyc: null */
| NULL_kw            { $$=^$(null_exp(LOC(@1,@1)));}
;

qual_opt_identifier:
  IDENTIFIER      { $$=^$(new $((nmspace_t)(new Rel_n(null)),new {$1})); }
| QUAL_IDENTIFIER { $$=$!1; }
;

%%

void yyprint(int i, xtunion YYSTYPE v) {
  switch (v) {
  case Okay_tok:           fprintf(stderr,"ok");         break;
  case &Int_tok(&$(_,i2)): fprintf(stderr,"%d",i2);      break;
  case &Char_tok(c):       fprintf(stderr,"%c",c);       break;
  case &Short_tok(s):      fprintf(stderr,"%ds",(int)s); break;
  case &String_tok(s):          fprintf(stderr,"\"%s\"",s); break;
  case &Stringopt_tok(null):    fprintf(stderr,"null");     break;
  case &Stringopt_tok(&Opt(s)): fprintf(stderr,"\"%s\"",*s); break;
  case &QualId_tok(&$(p,v2)):
    let prefix = null;
    switch (p) {
    case &Rel_n(x): prefix = x; break;
    case &Abs_n(x): prefix = x; break;
    case Loc_n: break;
    }
    for (; prefix != null; prefix = prefix->tl)
      fprintf(stderr,"%s::",*(prefix->hd));
    fprintf(stderr,"%s::",*v2);
    break;
  default: fprintf(stderr,"?"); break;
  }
}

namespace Parse{
list_t<decl_t> parse_file(FILE @f) {
  parse_result = null;
  lbuf = new Opt(from_file(f));
  yyparse();
  return parse_result;
}
}
