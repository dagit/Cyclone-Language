/* Parser.
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

/*
An adaptation of the ISO C grammar for Cyclone.  This grammar
is adapted from the proposed C9X standard, but the productions are
arranged as in Kernighan and Ritchie's "The C Programming
Language (ANSI C)", Second Edition, pages 234-239.

The grammar has 18 total shift-reduce conflicts, and 4 reduce-reduce
conflicts.  

The grammar has 1 shift-reduce conflict due to the "dangling else"
problem, which (the Cyclone port of) Bison properly resolves.

There are 11 additional shift-reduce conflicts due to ambiguities in
attribute parsing.  

There is 1 shift-reduce conflict due to the treatment of && in patterns.

There are 6 additional shift-reduce conflicts and 4 reduce-reduce conflicts
having to do with specifier-qualifier-lists and the desire to allow 
typedef names to be redeclared as identifiers.  

*/

%{
#define YYDEBUG 0 // 1 to debug, 0 otherwise
#define YYPRINT yyprint
#define YYERROR_VERBOSE

#if YYDEBUG==1
extern @tagged union YYSTYPE;
extern void yyprint(int i, union YYSTYPE v);
#endif

#include <core.h>
#include <cycboot.h>
#include <lexing.h>
#include <list.h>
#include <string.h>
#include <position.h>
#include "absyn.h"
#include "tcutil.h"
#include "absynpp.h"
using Core;
using List;
using Absyn;
using Position;

// Typedef processing must be split between the parser and lexer.
// These functions are called by the parser to communicate typedefs
// to the lexer, so the lexer can distinguish typedefs from identifiers.
namespace Lex {
  extern void register_typedef(qvar_t s);
  extern void enter_namespace(var_t);
  extern void leave_namespace();
  extern void enter_using(qvar_t);
  extern void leave_using();
}

// State that we thread through the lexer and parser.
#define YYLEX_PARAM_ARG Lexing::Lexbuf<Lexing::Function_lexbuf_state<FILE@>>
#define YYLEX_ARG yylex_buf
#define YYPARSE_PARAM_ARG Lexing::Lexbuf<Lexing::Function_lexbuf_state<FILE@>> yylex_buf

#define LOC(s,e) Position::segment_of_abs(s.first_line,e.last_line)
#define DUMMYLOC NULL

namespace Parse {

  // if true, parse register storage class as public
bool no_register = false;

////////////////////// Type definitions needed only during parsing ///////////
datatype Type_specifier {
  Signed_spec(seg_t);
  Unsigned_spec(seg_t);
  Short_spec(seg_t);
  Long_spec(seg_t);
  Type_spec(type_t,seg_t);    // int, `a, list_t<`a>, etc.
  Decl_spec(decl_t);
};
typedef datatype Type_specifier @type_specifier_t;

enum Storage_class {
 Typedef_sc, Extern_sc, ExternC_sc, Static_sc, Auto_sc, Register_sc, Abstract_sc
};
typedef enum Storage_class storage_class_t;

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

////////////////////////// forward references //////////////////////
static $(type_t,opt_t<decl_t>)
  collapse_type_specifiers(list_t<type_specifier_t> ts, seg_t loc);
static $(tqual_t,type_t,list_t<tvar_t>,list_t<attribute_t>)
  apply_tms(tqual_t,type_t,list_t<attribute_t,`H>,list_t<type_modifier_t>);

////////////////// global state (we're not re-entrant) ////////////////
  //opt_t<Lexing::Lexbuf<Lexing::Function_lexbuf_state<FILE@>>> lbuf = NULL;
static list_t<decl_t> parse_result = NULL;

//////////////////// Error functions ///////////////////////
static void err(string_t<`H> msg, seg_t sg) {
  post_error(mk_err_parse(sg,msg));
}
static `a abort(seg_t sg, string_t fmt, ... inject parg_t<`r2> ap) 
  __attribute__((format(printf,2,3), noreturn)) {
  err(vrprintf(heap_region, fmt, ap), sg);
  throw new Exit;
}
static void unimp(string_t msg,seg_t sg) {
  fprintf(stderr,"%s: Warning: Cyclone does not yet support %s\n",
	  string_of_segment(sg),msg);
  return;
}
////////////////// Collapsing pointer qualifiers ///////////////////////////
static $(conref_t<bool> nullable,conref_t<bounds_t> bound,
         conref_t<bool> zeroterm,type_t rgn) 
  collapse_pointer_quals(seg_t loc, conref_t<bool> nullable, 
                         conref_t<bounds_t> bound, type_t rgn, 
                         pointer_quals_t pqs) {
  // for now, the last qualifier wins and overrides previous ones
  conref_t<bool> zeroterm = empty_conref();
  for (; pqs != NULL; pqs = pqs->tl) {
    switch (pqs->hd) {
    case &Zeroterm_ptrqual: 
      zeroterm = true_conref; break;
    case &Nozeroterm_ptrqual:
      zeroterm = false_conref; break;
    case &Nullable_ptrqual:
      nullable = true_conref; break;
    case &Notnull_ptrqual:
      nullable = false_conref; break;
    case &Fat_ptrqual:
      bound = bounds_dyneither_conref; break;
    case &Thin_ptrqual:
      bound = bounds_one_conref; break;
    case &Numelts_ptrqual(e):
      bound = new_conref(new Upper_b(e)); break;
    case &Region_ptrqual(t):
      rgn = t; break;
    }
  }
  return $(nullable,bound,zeroterm,rgn);
}

////////////////// Functions for creating abstract syntax //////////////////

// FIX:  need to guarantee this won't conflict with a user name
qvar_t gensym_enum() {
  // a way to gensym an enum name
  static int enum_counter = 0;
  return new $(Rel_n(NULL), 
	       new (string_t)aprintf("__anonymous_enum_%d__", enum_counter++));
}

static aggrfield_t
make_aggr_field(seg_t loc,
		$($(qvar_t,tqual_t,type_t,list_t<tvar_t>,
		    list_t<attribute_t,`H>)@,exp_opt_t)@ field_info) {
  let &$(&$(qid,tq,t,tvs,atts),expopt) = field_info;
  if (tvs != NULL)
    err("bad type params in struct field",loc);
  if(is_qvar_qualified(qid))
    err("struct or union field cannot be qualified with a namespace",loc);
  return new Aggrfield{.name = (*qid)[1], .tq = tq, .type = t,
		       .width = expopt, .attributes = atts};
}

static type_specifier_t type_spec(type_t t,seg_t loc) {
  return new Type_spec(t,loc);
}

// convert any array types to pointer types
static type_t array2ptr(type_t t, bool argposn) {
  switch (t) {
    // FIX: don't lose zero-term location
  case &ArrayType(ArrayInfo{t1,tq,eopt,zeroterm,ztl}):
    return starb_typ(t1,
                     argposn ? new_evar(&Tcutil::rko, NULL) : &HeapRgn_val,
                     tq,
                     eopt == NULL ? ((bounds_t)&DynEither_b_val) : 
                                    ((bounds_t)(new Upper_b((exp_t)eopt))),
                     zeroterm);
  default: return t;
  }
}

// The next few functions are used when we have a function (or aggregate)
// where the type of one parameter appears to depend on the value of
// another.  Specifically, when we have:
//    void foo(tag_t<`i> x, int arr[x])
// we want to rewrite it to:
//    void foo(tag_t<`i> x, int arr[valueof(`i)])

// given a list of arguments, pull out those that have names and
// are given tag_t's
static list_t<$(var_t,type_t)@> get_arg_tags(list_t<$(opt_t<var_t>,tqual_t,type_t) @>x) {
  let res = NULL;
  for (; x != NULL; x = x->tl) {
    switch (x->hd) {
    case &$(&Opt(v),_,&TagType(i)): 
      switch (i) {
      case &Evar(_,*z,_,_):
        // using an evar here will mess things up since the evar will be
        // duplicated.  So, we pin the evar down to a type variable instead.
        stringptr_t nm = new aprintf("`%s",*v);
        *z = new Opt(new VarType(new Tvar{nm,-1,new Eq_kb(&Tcutil::ik)}));
        break;
      default: break;
      }
      res = new List(new $(v,i),res); break;
      // while we're at it, give any anonymous regions_t's a name that
      // corresponds to the variable.
    case &$(&Opt(v),_,&RgnHandleType(&Evar(_,*z,_,_))): 
      stringptr_t nm = new aprintf("`%s",*v);
      *z = new Opt(new VarType(new Tvar{nm,-1,new Eq_kb(&Tcutil::rk)}));
      break;
    default: break;
    }
  }
  return res;
}

// same as above, but for a list of aggregate fields
static list_t<$(var_t,type_t)@> get_argrfield_tags(list_t<aggrfield_t> x) {
  let res = NULL;
  for (; x != NULL; x = x->tl) {
    switch (x->hd->type) {
    case &TagType(i):
      res = new List(new $(x->hd->name,i), res); break;
    default: break;
    }
  }
  return res;
}

// given a mapping of variables to tags (x->`i), replace the variable x with `i
static exp_t substitute_tags_exp(list_t<$(var_t,type_t)@> tags, exp_t e) {
  switch (e->r) {
  case &Var_e(&$({.Rel_n = NULL},y),_): fallthru(y);
  case &UnknownId_e(&$({.Rel_n = NULL},y)):
    for (let ts = tags; ts != NULL; ts = ts->tl) {
      let &$(x,i) = ts->hd;
      if (strptrcmp(x,y) == 0) 
        return new_exp(new Valueof_e(Tcutil::copy_type(i)),e->loc);
    }
    break;
  default: break;
  }
  return e;
}

// we have a variable x in scope with type tag_t<`i> -- look for
// array or pointer bounds involving x and replace with valueof(`i).
static type_t substitute_tags(list_t<$(var_t,type_t)@> tags, type_t t) {
  switch (t) {
  case &ArrayType(ArrayInfo{et,tq,nelts,zt,ztloc}):
    exp_opt_t nelts2 = nelts;
    if (nelts != 0) {
      nelts2 = substitute_tags_exp(tags,nelts);
    }
    let et2 = substitute_tags(tags,et);
    if (nelts != nelts2 || et != et2)
      return new ArrayType(ArrayInfo{et2,tq,nelts2,zt,ztloc});
    break;
  case &PointerType(PtrInfo{et,tq,PtrAtts{r,n,b,zt,pl}}):
    let et2 = substitute_tags(tags,et);
    let b2 = b;
    switch (*b) {
    case {.Eq_constr = &Upper_b(e)}: 
      e = substitute_tags_exp(tags,e);
      b2 = new Constraint{.Eq_constr = new Upper_b(e)};
      break;
    default: break;
    }
    if (et2 != et || b2 != b)
      return new PointerType(PtrInfo{et2,tq,PtrAtts{r,n,b2,zt,pl}});
    break;
  default: 
    // FIX: should go into other types too
    break;
  }
  return t;
}

// eliminate dependencies on tag fields by substituting the type-level
// value for the associated field name.
static void substitute_aggrfield_tags(list_t<$(var_t,type_t)@> tags, aggrfield_t x) {
  x->type = substitute_tags(tags,x->type);
}

// given an optional variable, tqual, type, and list of type
// variables, return the tqual and type and check that the type
// variables are NULL -- used when we have a tuple type specification.
static $(tqual_t,type_t)@
  get_tqual_typ(seg_t loc,$(opt_t<var_t>,tqual_t,type_t) @t) {
  return new $((*t)[1],(*t)[2]);
}

static vardecl_t decl2vardecl(decl_t d) {
  switch (d->r) {
  case &Var_d(vd): return vd;
  default: abort(d->loc,"bad declaration in `forarray' statement");
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
static type_t id2type(string_t<`H> s, kindbound_t k) {
  if (zstrcmp(s,"`H") == 0)
    return &HeapRgn_val;
  else if (zstrcmp(s,"`U") == 0)
    return &UniqueRgn_val;
  else
    return new VarType(new Tvar(new s,-1,k));
}

static tvar_t copy_tvar(tvar_t t) {
  kindbound_t k;
  switch (compress_kb(t->kind)) {
  case &Unknown_kb(_):  k = new Unknown_kb(NULL);  break;
  case &Less_kb(_,knd): k = new Less_kb(NULL,knd); break;
  case kb: k = kb; break;
  }
  return new Tvar{.name=t->name, .identity = -1, .kind = k};
}

// convert a list of types to a list of typevars -- the parser can't
// tell lists of types apart from lists of typevars easily so we parse
// them as types and then convert them back to typevars.  See
// productions "struct_or_union_specifier" and "datatype_specifier";
static tvar_t typ2tvar(seg_t loc, type_t t) {
  switch (t) {
  case &VarType(pr): return pr;
  default: abort(loc,"expecting a list of type variables, not types");
  }
}
static type_t tvar2typ(tvar_t pr) {
  return new VarType(pr);
}

// if tvar's kind is unconstrained, set it to k
static void set_vartyp_kind(type_t t, kind_t k, bool leq) {
  switch(Tcutil::compress(t)) {
  case &VarType(&Tvar(_,_,*cptr)): 
    switch(compress_kb(*cptr)) {
    case &Unknown_kb(_): 
      if (!leq) *cptr = Tcutil::kind_to_bound(k);
      else *cptr = new Less_kb(NULL,k);
      return;
    default: return;
    }
  default: return;
  }
}

// Convert an old-style function into a new-style function
static list_t<type_modifier_t> oldstyle2newstyle(list_t<type_modifier_t<`H>,`H> tms,
						 list_t<decl_t> tds, 
						 seg_t loc) {
  // Not an old-style function
  if (tds==NULL) return tms;

  // If no function is found, or the function is not innermost, then
  // this is not a function definition; it is an error.  But, we
  // return silently.  The error will be caught by make_function below.
  if (tms==NULL) return NULL;

  switch (tms->hd) {
  case &Function_mod(args):
    // Is this the innermost function??
    if (tms->tl==NULL ||
	(is_typeparam(tms->tl->hd) && tms->tl->tl==NULL)) {
      // Yes
      switch (args) {
      case &WithTypes(_,_,_,_,_):
	Tcutil::warn(loc,"function declaration with both new- and old-style "
		     "parameter declarations; ignoring old-style");
	return tms;
      case &NoTypes(ids,_):
	if(length(ids) != length(tds))
	  abort(loc, "wrong number of parameter declarations in old-style "
		"function declaration");
	// replace each parameter with the right typed version
	let rev_new_params = NULL;
	for(; ids != NULL; ids = ids->tl) {
	  let tds2 = tds;
	  for(; tds2 != NULL; tds2 = tds2->tl) {
	    let x = tds2->hd;
	    switch(x->r) {
	    case &Var_d(vd):
	      if(zstrptrcmp((*vd->name)[1],ids->hd)!=0)
		continue;
	      if(vd->initializer != NULL)
		abort(x->loc, "initializer found in parameter declaration");
	      if(is_qvar_qualified(vd->name))
		abort(x->loc, "namespaces forbidden in parameter declarations");
	      rev_new_params =
		new List(new $(new Opt((*vd->name)[1]), vd->tq, vd->type), 
			 rev_new_params);
	      goto L;
	    default: abort(x->loc, "nonvariable declaration in parameter type");
	    }
	  }
	L: if(tds2 == NULL)
	  abort(loc,"%s is not given a type",*ids->hd);
	}
	return
	  new List(new Function_mod(new WithTypes(imp_rev(rev_new_params),
						  false,NULL,NULL,NULL)),
		   NULL);
      }
    } 
    // No, keep looking for the innermost function
    fallthru;
  default: return new List(tms->hd,oldstyle2newstyle(tms->tl,tds,loc));
  }
}

// make a top-level function declaration out of a declaration-specifier
// (return type, etc.), a declarator (the function name and args),
// a declaration list (for old-style function definitions), and a statement.
static fndecl_t make_function(opt_t<decl_spec_t> dso, declarator_t d,
                              list_t<decl_t> tds, stmt_t body, seg_t loc) {
  // Handle old-style parameter declarations
  if (tds!=NULL)
    d = new Declarator(d->id,oldstyle2newstyle(d->tms,tds,loc));

  scope_t sc = Public;
  list_t<type_specifier_t> tss = NULL;
  tqual_t tq = empty_tqual(NULL);
  bool is_inline = false;
  list_t<attribute_t> atts = NULL;

  if (dso != NULL) {
    tss = dso->v->type_specs;
    tq  = dso->v->tq;
    is_inline = dso->v->is_inline;
    atts = dso->v->attributes;
    // Examine storage class; like C, we allow both static and extern
    if (dso->v->sc != NULL)
      switch (dso->v->sc->v) {
      case Extern_sc: sc = Extern; break;
      case Static_sc: sc = Static; break;
      default: err("bad storage class on function",loc); break;
      }
  }
  let $(t,decl_opt) = collapse_type_specifiers(tss,loc);
  let $(fn_tqual,fn_type,x,out_atts) = apply_tms(tq,t,atts,d->tms);
  // what to do with the left-over attributes out_atts?  I'm just
  // going to append them to the function declaration and let the
  // type-checker deal with it.
  if (decl_opt != NULL)
    Tcutil::warn(loc,"nested type declaration within function prototype");
  if (x != NULL)
    // Example:   `a f<`b><`a>(`a x) {...}
    // Here info[2] will be the list `b.
    Tcutil::warn(loc,"bad type params, ignoring");
  // fn_type had better be a FnType
  switch (fn_type) {
    case &FnType(FnInfo{tvs,eff,ret_type,args,c_varargs,cyc_varargs,
                          rgn_po,attributes}):
      let rev_newargs = NULL;
      for(let args2 = args; args2 != NULL; args2 = args2->tl) {
	let &$(vopt,tq,t) = args2->hd;
	if(vopt == NULL) {
	  err("missing argument variable in function prototype",loc);
	  rev_newargs = new List(new $(new "?",tq,t), rev_newargs);
	} else
	  rev_newargs = new List(new $(vopt->v,tq,t), rev_newargs);
      }
      // We don't fill in the cached type here because we may need
      // to figure out the bound type variables and the effect.
      return new Fndecl {.sc=sc,.name=d->id,.tvs=tvs,
                            .is_inline=is_inline,.effect=eff,
                            .ret_type=ret_type,.args=imp_rev(rev_newargs),
                            .c_varargs=c_varargs,
                            .cyc_varargs=cyc_varargs,
                            .rgn_po = rgn_po,
                            .body=body,.cached_typ=NULL,
                            .param_vardecls=NULL,
                            .fn_vardecl = NULL,
                            .attributes = append(attributes,out_atts)};
    default: abort(loc,"declarator is not a function prototype");
  }
}

static string_t msg1 = 
  "at most one type may appear within a type specifier";
static string_t msg2 =
  "const or volatile may appear only once within a type specifier";
static string_t msg3 = 
  "type specifier includes more than one declaration";
static string_t msg4 = 
  "sign specifier may appear only once within a type specifier";
// Given a type-specifier list, determines the type and any declared
// structs, unions, enums, or datatypes.  Most of this is just collapsing
// combinations of [un]signed, short, long, int, char, etc.  We're
// probably more permissive than is strictly legal here.  For
// instance, one can write "unsigned const int" instead of "const
// unsigned int" and so forth.  I don't think anyone will care...
// (famous last words)
static $(type_t,opt_t<decl_t>)
  collapse_type_specifiers(list_t<type_specifier_t> ts, seg_t loc) {

  opt_t<decl_t> declopt = NULL;    // any hidden declarations

  bool      seen_type = false;
  bool      seen_sign = false;
  bool      seen_size = false;
  type_t    t         = &VoidType_val;
  size_of_t sz        = Int_sz; 
  sign_t    sgn       = Signed;

  seg_t last_loc = loc;

  for(; ts != NULL; ts = ts->tl)
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
      sz        = Short_sz;
      break;
    case &Long_spec(loc2):
      if(seen_type) err("long modifier must come before base type",loc2);
      // okay to have seen a size so long as it was long (means long long)
      // That happens when we've seen a size yet we're Long_sz
      if(seen_size)
	switch (sz) {
	case Long_sz:
	  sz = LongLong_sz;
	  break;
	default: err("extra long in type specifier",loc2); break;
	} 
      else 
        sz = Long_sz;
      last_loc = loc2;
      // the rest is is necessary if it's long and harmless if long long
      seen_size = true;
      break;
    case &Decl_spec(d):
      // we've got a type declaration embedded in here -- return
      // the declaration as well as a copy of the type -- this allows
      // us to declare structs as a side effect inside typedefs
      // Note: Leave the last fields NULL so check_valid_type knows the type
      //       has not been checked.
      last_loc = d->loc;
      if (declopt == NULL && !seen_type) {
	seen_type = true;
        switch (d->r) {
        case &Aggr_d(ad):
          let args = List::map(tvar2typ,List::map(copy_tvar,ad->tvs));
          t = new AggrType(AggrInfo(UnknownAggr(ad->kind,ad->name,NULL),args));
          if (ad->impl!=NULL) declopt = new Opt(d);
	  break;
        case &Datatype_d(tud):
          let args = List::map(tvar2typ,List::map(copy_tvar,tud->tvs));
          t = new DatatypeType(DatatypeInfo(KnownDatatype(new tud),args));
	  if(tud->fields != NULL) declopt = new Opt(d);
	  break;
        case &Enum_d(ed):
          t = new EnumType(ed->name,NULL);
          declopt = new Opt(d);
          break;
        default: abort(d->loc,"bad declaration within type specifier");
        }
      } else err(msg3,d->loc);
      break;
    }

  // it's okay to not have an explicit type as long as we have some
  // combination of signed, unsigned, short, long, or longlong
  if (!seen_type) {
    if(!seen_sign && !seen_size) 
      Tcutil::warn(last_loc,"missing type within specifier");
    t = int_typ(sgn,sz);
  } else {
    if(seen_sign)
      switch (t) {
      case &IntType(sgn2,sz2): 
        if (sgn2 != sgn)
          t = int_typ(sgn,sz2);
        break;
      default: err("sign specification on non-integral type",last_loc); break;
      }
    if(seen_size)
      switch (t) {
      case &IntType(sgn2,sz2): 
        if (sz2 != sz)
          t = int_typ(sgn2,sz);
        break;
        // hack -- if we've seen "long" then sz will be B8
      case &DoubleType(_): t = double_typ(true); break;
      default: err("size qualifier on non-integral type",last_loc); break;
      }
  }
  return $(t, declopt);
}

static list_t<$(qvar_t,tqual_t,type_t,list_t<tvar_t>,list_t<attribute_t>)@`r,`r>
  apply_tmss(region_t<`r> r, tqual_t tq, type_t t,list_t<declarator_t> ds,
             attributes_t shared_atts)
{
  if (ds==NULL) return NULL;
  let d = ds->hd;
  let q = d->id;
  let $(tq,new_typ,tvs,atts) = apply_tms(tq,t,shared_atts,d->tms);
  // NB: we copy the type here to avoid sharing definitions
  // but we avoid the copy when ds->tl is NULL
  if (ds->tl == NULL) 
    return rnew(r) List(rnew(r) $(q,tq,new_typ,tvs,atts),NULL);
  else
    return rnew(r) List(rnew(r) $(q,tq,new_typ,tvs,atts),
                    apply_tmss(r,tq,Tcutil::copy_type(t),ds->tl,shared_atts));
}

static $(tqual_t,type_t,list_t<tvar_t>,list_t<attribute_t>)
  apply_tms(tqual_t tq, type_t t, list_t<attribute_t,`H> atts,
            list_t<type_modifier_t> tms) {
  if (tms==NULL) return $(tq,t,NULL,atts);
  switch (tms->hd) {
  case &Carray_mod(zeroterm,ztloc):
    return apply_tms(empty_tqual(NULL),
                     array_typ(t,tq,NULL,zeroterm,ztloc),atts,tms->tl);
  case &ConstArray_mod(e,zeroterm,ztloc):
    return apply_tms(empty_tqual(NULL),
                     array_typ(t,tq,e,zeroterm,ztloc),atts,tms->tl);
  case &Function_mod(args): {
    switch (args) {
    case &WithTypes(args2,c_vararg,cyc_vararg,eff,rgn_po):
      list_t<tvar_t> typvars = NULL;
      // function type attributes seen thus far get put in the function type
      attributes_t fn_atts = NULL, new_atts = NULL;
      for (_ as = atts; as != NULL; as = as->tl) {
	if (fntype_att(as->hd))
	  fn_atts = new List(as->hd,fn_atts);
	else
	  new_atts = new List(as->hd,new_atts);
      }
      // functions consume type parameters
      if (tms->tl != NULL) {
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
      if (!c_vararg && cyc_vararg == NULL // not vararg function
	  && args2 != NULL      // not empty arg list
	  && args2->tl == NULL   // not >1 arg
	  && (*args2->hd)[0] == NULL // not f(void x)
	  && (*args2->hd)[2] == &VoidType_val) {
	args2 = NULL;
      }
      // pull out any tag_t variables and their associated tag_t types
      let tags = get_arg_tags(args2);
      // convert result type from array to pointer result
      if (tags != NULL)
        t = substitute_tags(tags,t);
      t = array2ptr(t,false);
      // convert any array arguments to suitable  pointer arguments
      // and substitute away any implicit tag arguments.  
      for (let a = args2; a != NULL; a = a->tl) {
        let &$(vopt,tq,*t) = a->hd;
        if (tags != NULL) 
          *t = substitute_tags(tags,*t);
        *t = array2ptr(*t,true);
      }
      // Note, we throw away the tqual argument.  An example where
      // this comes up is "const int f(char c)"; it doesn't really
      // make sense to think of the function as returning a const
      // (or volatile, or restrict).  The result will be copied
      // anyway.  TODO: maybe we should issue a warning.  But right
      // now we don't have a loc so the warning will be confusing.
      return apply_tms(empty_tqual(tq.loc),
		       function_typ(typvars,eff,t,args2,
				    c_vararg,cyc_vararg,rgn_po,fn_atts),
		       new_atts,
		       tms->tl);
    case &NoTypes(_,loc):
      abort(loc,"function declaration without parameter types");
    }
  }
  case &TypeParams_mod(ts,loc,_):
    // If we are the last type modifier, this could be the list of
    // type parameters to a typedef:
    // typedef struct foo<`a,int> foo_t<`a>
    if (tms->tl==NULL)
      return $(tq,t,ts,atts);
    // Otherwise, it is an error in the program if we get here;
    // TypeParams should already have been consumed by an outer
    // Function (see last case).
    abort(loc, "type parameters must appear before function arguments "
	  "in declarator");
  case &Pointer_mod(ptratts,tq2):
    return apply_tms(tq2,new PointerType(PtrInfo(t,tq,ptratts)),atts,tms->tl);
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
  if(decls_opt != NULL)
    Tcutil::warn(loc,"ignoring nested type declaration(s) in specifier list");
  return t;
}

// convert an (optional) variable, tqual, type, and type
// parameters to a typedef declaration.  As a side effect, register
// the typedef with the lexer.
// TJ: FIX the tqual should make it into the typedef as well,
// e.g., typedef const int CI;
static decl_t v_typ_to_typedef(seg_t loc, $(qvar_t,tqual_t,type_t,list_t<tvar_t,`H>,list_t<attribute_t,`H>)@ t) {
  let &$(x,tq,typ,tvs,atts) = t;
  // tell the lexer that x is a typedef identifier
  Lex::register_typedef(x);
  // if the "type" is an evar, then the typedef is abstract
  opt_t<kind_t> kind;
  opt_t<type_t> type;
  switch (typ) {
  case &Evar(kopt,_,_,_): 
    type = NULL;
    if (kopt == NULL) kind = &Tcutil::bko;
    else kind = kopt;
    break;
  default: kind = NULL; type = new Opt(typ); break;
  }
  return new_decl(new Typedef_d(new Typedefdecl{.name=x, .tvs=tvs, .kind=kind,
                                                .defn=type, .atts=atts,
                                                .tq=tq}),
		  loc);
}

// given a local declaration and a statement produce a decl statement
static stmt_t flatten_decl(decl_t d,stmt_t s) {
  return new_stmt(new Decl_s(d,s),d->loc);
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
                                        seg_t tqual_loc,
					seg_t loc) {
 region mkrgn; {
  let &Declaration_spec(_,tq,tss,_,atts) = ds;
  if (tq.loc == NULL) tq.loc = tqual_loc;
  if (ds->is_inline)
    err("inline is allowed only on function definitions",loc);
  if (tss == NULL) {
    err("missing type specifiers in declaration",loc);
    return NULL;
  }

  scope_t s = Public;
  bool istypedef = false;
  if (ds->sc != NULL)
    switch (ds->sc->v) {
    case Typedef_sc:  istypedef = true; break;
    case Extern_sc:   s = Extern;   break;
    case ExternC_sc:  s = ExternC;  break;
    case Static_sc:   s = Static;   break;
    case Auto_sc:     s = Public;   break;
    case Register_sc: if(no_register) s = Public; else s = Register;   break;
    case Abstract_sc: s = Abstract; break;
    }

  // separate the declarators from their initializers
  let $(declarators, exprs) = List::rsplit(mkrgn,mkrgn,ids);
  // check to see if there are no initializers -- useful later on
  bool exps_empty = true;
  for (list_t<exp_opt_t> es = exprs; es != NULL; es = es->tl)
    if (es->hd != NULL) {
      exps_empty = false;
      break;
    }

  // Collapse the type specifiers to get the base type and any
  // nested declarations
  let ts_info = collapse_type_specifiers(tss,loc);
  if (declarators == NULL) {
    // here we have a type declaration -- either a struct, union,
    // or datatype as in: "struct Foo { ... };"
    let $(t,declopt) = ts_info;
    if (declopt != NULL) {
      decl_t d = declopt->v;
      switch (d->r) {
      case &Enum_d(ed):
        ed->sc = s;
        if (atts != NULL) err("bad attributes on enum",loc); 
	break;
      case &Aggr_d(ad): ad->sc = s; ad->attributes = atts; break;
      case &Datatype_d(tud):
        tud->sc = s;
        if (atts != NULL) err("bad attributes on datatype",loc); 
	break;
      default: err("bad declaration",loc); return NULL;
      }
      return new List(d,NULL);
    } else {
      switch (t) {
        // FIX: we're not doing anything with the @tagged qualifier here...
      case &AggrType(AggrInfo({.UnknownAggr = $(k,n,_)},ts)):
	let ts2 = List::map_c(typ2tvar,loc,ts);
	let ad  = new Aggrdecl(k,s,n,ts2,NULL,NULL);
	if (atts != NULL) err("bad attributes on type declaration",loc);
	return new List(new_decl(new Aggr_d(ad),loc),NULL);
      case &DatatypeType(DatatypeInfo({.KnownDatatype = tudp},_)):
	if(atts != NULL) err("bad attributes on datatype", loc);
	return new List(new_decl(new Datatype_d(*tudp),loc),NULL);
      case &DatatypeType(DatatypeInfo({.UnknownDatatype = UnknownDatatypeInfo(n,isx)},ts)):
        let ts2 = List::map_c(typ2tvar,loc,ts);
        let tud = datatype_decl(s, n, ts2, NULL, isx, loc);
        if (atts != NULL) err("bad attributes on datatype",loc);
        return new List(tud,NULL);
      case &EnumType(n,_):
        let ed = new Enumdecl{s,n,NULL};
        if (atts != NULL) err("bad attributes on enum",loc);
        return new List(new Decl(new Enum_d(ed),loc),NULL);
      case &AnonEnumType(fs):
        // someone's written:  enum {A,B,C}; which is a perfectly good
        // way to declare symbolic constants A, B, and C.
        let ed = new Enumdecl{s,gensym_enum(),new Opt(fs)};
        if (atts != NULL) err("bad attributes on enum",loc);
        return new List(new Decl(new Enum_d(ed),loc),NULL);
      default: err("missing declarator",loc); return NULL;
      }
    }
  } else {
    // declarators != NULL
    type_t t = ts_info[0];
    let fields = apply_tmss(mkrgn,tq,t,declarators,atts);
    if (istypedef) {
      // we can have a nested struct, union, or datatype
      // declaration within the typedef as in:
      // typedef struct Foo {...} t;
      if (!exps_empty)
	err("initializer in typedef declaration",loc);
      list_t<decl_t> decls = List::map_c(v_typ_to_typedef,loc,fields);
      if (ts_info[1] != NULL) {
        decl_t d = ts_info[1]->v;
        switch (d->r) {
        case &Aggr_d(ad):
          ad->sc = s; ad->attributes = atts; atts = NULL; break;
        case &Datatype_d(tud): tud->sc = s; break;
        case &Enum_d(ed):    ed->sc  = s; break;
        default:
          err("declaration within typedef is not a struct, union, or datatype",
              loc);
	  break;
        }
        decls = new List(d,decls);
      }
      return decls;
    } else {
      // here, we have a bunch of variable declarations
      if (ts_info[1] != NULL)
        unimp("nested type declaration within declarator",loc);
      list_t<decl_t> decls = NULL;
      for (let ds = fields; ds != NULL; ds = ds->tl, exprs = exprs->tl) {
	let &$(x,tq2,t2,tvs2,atts2) = ds->hd;
        if (tvs2 != NULL)
          Tcutil::warn(loc,"bad type params, ignoring");
        if (exprs == NULL)
          abort(loc,"unexpected NULL in parse!");
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
}

// Convert an identifier to a kind
static kind_t id_to_kind(string_t s, seg_t loc) {
  if(strlen(s)==1 || strlen(s)==2)
    switch (s[0]) {
    case 'A': return &Tcutil::ak;
    case 'M': return &Tcutil::mk;
    case 'B': return &Tcutil::bk;
    case 'R': return &Tcutil::rk;
    case 'E': return &Tcutil::ek;
    case 'I': return &Tcutil::ik;
    case 'U': 
      switch (s[1]) {
      case 'R':	return &Tcutil::urk;
      case 'A': return &Tcutil::uak;
      case 'M': return &Tcutil::umk;
      case 'B': return &Tcutil::ubk;
      default: break;
      }
      break;
    case 'T':
      switch (s[1]) {
      case 'R':	return &Tcutil::trk;
      case 'A': return &Tcutil::tak;
      case 'M': return &Tcutil::tmk;
      case 'B': return &Tcutil::tbk;
      default: break;
      }
      break;
    default:  break;
  }
  err(aprintf("bad kind: %s; strlen=%d",s,strlen(s)), loc); 
  return &Tcutil::bk;
}

// convert a pattern back into an expression 
static exp_t pat2exp(pat_t p) {
  switch (p->r) {
  case &UnknownId_p(x): return new_exp(new UnknownId_e(x),p->loc);
  case &Reference_p(vd,&Pat{.r = &Wild_p,...}): 
    return deref_exp(new_exp(new UnknownId_e(vd->name),p->loc),p->loc);
  case &Pointer_p(p2): return address_exp(pat2exp(p2),p->loc);
  case &Null_p: return null_exp(p->loc);
  case &Int_p(s,i): return int_exp(s,i,p->loc);
  case &Char_p(c): return char_exp(c,p->loc);
  case &Float_p(s): return float_exp(s,p->loc);
  case &UnknownCall_p(x,ps,false): 
    exp_t e1 = new_exp(new UnknownId_e(x),p->loc);
    list_t<exp_t> es = List::map(pat2exp,ps);
    return unknowncall_exp(e1,es,p->loc);
  case &Exp_p(e): return e;
  default: 
    err("cannot mix patterns and expressions in case",p->loc);
    return null_exp(p->loc);
  }
}

} // end namespace Parse
using Parse;
%}

// ANSI C keywords
%token AUTO REGISTER STATIC EXTERN TYPEDEF VOID CHAR SHORT INT LONG FLOAT
%token DOUBLE SIGNED UNSIGNED CONST VOLATILE RESTRICT
%token STRUCT UNION CASE DEFAULT INLINE SIZEOF OFFSETOF
%token IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN ENUM
// Cyc:  CYCLONE additional keywords
%token NULL_kw LET THROW TRY CATCH EXPORT
%token NEW ABSTRACT FALLTHRU USING NAMESPACE DATATYPE XTUNION TUNION
%token MALLOC RMALLOC CALLOC RCALLOC SWAP
%token REGION_T TAG_T REGION RNEW REGIONS RESET_REGION
%token NOZEROTERM_QUAL ZEROTERM_QUAL REGION_QUAL PORTON PORTOFF DYNREGION_T
%token ALIAS NUMELTS VALUEOF VALUEOF_T TAGCHECK NUMELTS_QUAL THIN_QUAL
%token FAT_QUAL NOTNULL_QUAL NULLABLE_QUAL
// Cyc:  CYCLONE qualifiers (e.g., @zeroterm, @tagged)
%token NOZEROTERM_QUAL ZEROTERM_QUAL TAGGED_QUAL EXTENSIBLE_QUAL RESETABLE_QUAL
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
%token TYPE_VAR TYPEDEF_NAME QUAL_IDENTIFIER QUAL_TYPEDEF_NAME 
// Cyc: added __attribute__ keyword
%token ATTRIBUTE
// specify tagged union constructors for types of semantic values that 
// the lexer must produce.  The other constructors are generated implicitly.
%union {
  Int_tok($(sign_t,int));
  Char_tok(char);
  String_tok(string_t<`H>);
  Stringopt_tok(opt_t<stringptr_t<`H,`H>,`H>);
  QualId_tok(qvar_t);
}
/* types for productions */
%type <$(sign_t,int)> INTEGER_CONSTANT
%type <char> CHARACTER_CONSTANT
%type <string_t<`H>> FLOATING_CONSTANT namespace_action
%type <string_t<`H>> IDENTIFIER TYPEDEF_NAME TYPE_VAR STRING field_name
%type <$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H> pointer_null_and_bound
%type <conref_t<bounds_t>> pointer_bound
%type <exp_t> primary_expression postfix_expression unary_expression
%type <exp_t> cast_expression constant multiplicative_expression
%type <exp_t> additive_expression shift_expression relational_expression
%type <exp_t> equality_expression and_expression exclusive_or_expression
%type <exp_t> inclusive_or_expression logical_and_expression
%type <exp_t> logical_or_expression conditional_expression
%type <exp_t> assignment_expression expression constant_expression
%type <exp_t> initializer array_initializer
%type <exp_opt_t> open_exp_opt
%type <list_t<exp_t,`H>> argument_expression_list argument_expression_list0
%type <list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>> initializer_list
%type <primop_t> unary_operator 
%type <opt_t<primop_t,`H>> assignment_operator
%type <qvar_t> QUAL_IDENTIFIER QUAL_TYPEDEF_NAME 
%type <qvar_t> qual_opt_identifier qual_opt_typedef
%type <qvar_t> using_action struct_union_name
%type <stmt_t> statement labeled_statement
%type <stmt_t> compound_statement block_item_list
%type <stmt_t> expression_statement selection_statement iteration_statement
%type <stmt_t> jump_statement
%type <list_t<switch_clause_t,`H>> switch_clauses
%type <pat_t> pattern exp_pattern conditional_pattern logical_or_pattern
%type <pat_t> logical_and_pattern inclusive_or_pattern exclusive_or_pattern
%type <pat_t> and_pattern equality_pattern relational_pattern shift_pattern
%type <pat_t> additive_pattern multiplicative_pattern cast_pattern
%type <pat_t> unary_pattern postfix_pattern primary_pattern
%type <$(list_t<pat_t,`H>,bool)@`H> tuple_pattern_list
%type <list_t<pat_t,`H>> tuple_pattern_list0
%type <$(list_t<designator_t,`H>,pat_t)@`H> field_pattern
%type <list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>> field_pattern_list0
%type <$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H> field_pattern_list
%type <fndecl_t> function_definition function_definition2
%type <list_t<decl_t,`H>> declaration declaration_list
%type <list_t<decl_t,`H>> prog translation_unit external_declaration
%type <decl_spec_t> declaration_specifiers
%type <$(declarator_t,exp_opt_t)@`H> init_declarator
%type <list_t<$(declarator_t,exp_opt_t)@`H,`H>> init_declarator_list init_declarator_list0
%type <storage_class_t> storage_class_specifier
%type <type_specifier_t> type_specifier type_specifier_notypedef enum_specifier
%type <type_specifier_t> struct_or_union_specifier datatype_specifier
%type <aggr_kind_t> struct_or_union
%type <tqual_t> type_qualifier tqual_list
%type <list_t<aggrfield_t,`H>> struct_declaration_list struct_declaration
%type <list_t<list_t<aggrfield_t,`H>,`H>> struct_declaration_list0
%type <list_t<type_modifier_t<`H>,`H>> pointer one_pointer
%type <declarator_t> declarator direct_declarator declarator_withtypedef direct_declarator_withtypedef
%type <$(declarator_t,exp_opt_t)@`H> struct_declarator
%type <list_t<$(declarator_t,exp_opt_t)@`H,`H>> struct_declarator_list struct_declarator_list0
%type <abstractdeclarator_t> abstract_declarator direct_abstract_declarator
%type <bool> optional_inject resetable_qual_opt qual_datatype
%type <scope_t> datatypefield_scope
%type <datatypefield_t> datatypefield
%type <list_t<datatypefield_t,`H>> datatypefield_list
%type <$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H> specifier_qualifier_list notypedef_specifier_qualifier_list
%type <list_t<var_t,`H>> identifier_list identifier_list0
%type <$(opt_t<var_t,`H>,tqual_t,type_t)@`H> parameter_declaration type_name
%type <list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>> parameter_list
%type <$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H> parameter_type_list
%type <list_t<type_t,`H>> type_name_list type_params_opt effect_set region_set
%type <list_t<type_t,`H>> atomic_effect
%type <list_t<designator_t,`H>> designation designator_list
%type <designator_t> designator
%type <kind_t> kind
%type <type_t> any_type_name type_var rgn_opt
%type <list_t<attribute_t,`H>> attributes_opt attributes attribute_list
%type <attribute_t> attribute
%type <enumfield_t> enum_field
%type <list_t<enumfield_t,`H>> enum_declaration_list
%type <opt_t<type_t,`H>> optional_effect 
%type <list_t<$(type_t,type_t)@`H,`H>> optional_rgn_order rgn_order
%type <conref_t<bool>> zeroterm_qual_opt
%type <list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>> export_list export_list_values
%type <pointer_qual_t> pointer_qual
%type <pointer_quals_t> pointer_quals
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
    { list_t<decl_t> x, y;
      try {
        x = $1;
      } catch {
      case &Core::Failure(_):
        x = NULL; break;
      }
      try {
        y = $2;
      } catch {
      case &Core::Failure(_):
        y = NULL; break;
      }
      $$=^$(List::imp_append(x,y)); 
    }
/* Cyc: added using and namespace */
/* NB: using_action calls Lex::enter_using */
| using_action ';' translation_unit
    { $$=^$(new List(new Decl(new Using_d($1,$3),LOC(@1,@3)),NULL));
      Lex::leave_using();
    }
| using_action '{' translation_unit unusing_action translation_unit
    { $$=^$(new List(new Decl(new Using_d($1,$3),LOC(@1,@4)),$5)); }
/* NB: namespace_action calls Lex::enter_namespace */
| namespace_action ';' translation_unit
    { $$=^$(new List(new Decl(new Namespace_d(new $1,$3),LOC(@1,@3)),NULL));
      Lex::leave_namespace();
    }
| namespace_action '{' translation_unit unnamespace_action translation_unit
    { 
      string_t nspace;
      list_t<decl_t> x, y;
      try { nspace = $1; } catch {
      case &Core::Failure(_): nspace = ""; break;
      }
      try { x = $3; } catch {
      case &Core::Failure(_): x = NULL; break;
      }
      try { y = $5; } catch {
      case &Core::Failure(_): y = NULL; break;
      }
      $$=^$(new List(new Decl(new Namespace_d(new nspace,x),LOC(@1,@4)),y)); 
    }
| EXTERN STRING '{' translation_unit '}' translation_unit
    { if (strcmp($2,"C") == 0) {
        $$=^$(new List(new Decl(new ExternC_d($4),LOC(@1,@4)),$6));
      } else {
        if (strcmp($2,"C include") != 0) {
          err("expecting \"C\" or \"C include\"",LOC(@1,@2));
        }
        $$=^$(new List(new Decl(new ExternCinclude_d($4,NULL),LOC(@1,@5)),$6));
      }
    }
| EXTERN STRING '{' translation_unit '}' export_list translation_unit
    { if (strcmp($2,"C include") != 0) 
        err("expecting \"C include\"",LOC(@1,@2));
       list_t<$(seg_t,qvar_t,bool)@> exs = $6;
      $$=^$(new List(new Decl(new ExternCinclude_d($4,exs),LOC(@1,@6)),$7));
    }
| PORTON ';' translation_unit
  { $$=^$(new List(new Decl(&Porton_d_val,LOC(@1,@1)),$3)); }
| PORTOFF ';' translation_unit
  { $$=^$(new List(new Decl(&Portoff_d_val,LOC(@1,@1)),$3)); }
| /* empty */ { $$=^$(NULL); }
;

export_list: 
  EXPORT '{' export_list_values '}' { $$= $!3; }
| EXPORT '{' '}' { $$=^$(NULL); }
;    

export_list_values:
  struct_union_name ';' { $$=^$(new List(new $(LOC(@1,@1),$1,false),NULL)); }
| struct_union_name ',' export_list_values
  { $$=^$(new List(new $(LOC(@1,@1),$1,false),$3));}
;

external_declaration:
  function_definition {$$=^$(new List(new_decl(new Fn_d($1),LOC(@1,@1)),NULL));}
| declaration         {$$=$!1;}
| error               {$$=^$(NULL);}
;

function_definition:
  declarator compound_statement
    { $$=^$(make_function(NULL,$1,NULL,$2,LOC(@1,@2))); }
| declaration_specifiers declarator compound_statement
    { $$=^$(make_function(new Opt($1),$2,NULL,$3,LOC(@1,@3))); }
/* 2 shift-reduce conflicts come up because of the next two rules and
   the final rule of function_definition2.  A declarator can be followed
   by an attribute to become a declarator, and a declaration list can
   begin with an attribute.  The default action of shift means the
   attribute will end up attached to the declarator, which is probably
   not the right decision.  However, this only comes up with old-style
   function definitions, and, both attributes following declarators and
   attributes beginning declarations are needed; so we ignore the issue
   for now.
*/
| declarator declaration_list compound_statement
    { $$=^$(make_function(NULL,$1,$2,$3,LOC(@1,@3))); }
| declaration_specifiers declarator declaration_list compound_statement
    { $$=^$(make_function(new Opt($1),$2,$3,$4,LOC(@1,@4))); }
;

/* Used for nested functions; the optional declaration_specifiers
   would cause parser conflicts */
function_definition2:
  declaration_specifiers declarator compound_statement
    { $$=^$(make_function(new Opt($1),$2,NULL,$3,LOC(@1,@3))); }
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
  NAMESPACE IDENTIFIER { Lex::enter_namespace(new $2); $$=$!2; }
;
unnamespace_action:
  '}' { Lex::leave_namespace(); }
;

/***************************** DECLARATIONS *****************************/
declaration:
  declaration_specifiers ';'
    { $$=^$(make_declarations($1,NULL,LOC(@1,@1),LOC(@1,@1))); }
| declaration_specifiers init_declarator_list ';'
    { $$=^$(make_declarations($1,$2,LOC(@1,@1),LOC(@1,@3))); }
/* Cyc: let declaration */
| LET pattern '=' expression ';'
    { $$=^$(new List(let_decl($2,$4,LOC(@1,@5)),NULL)); }
| LET identifier_list ';'
    { let vds = NULL;
      for (let ids = $2; ids != NULL; ids = ids->tl) {
        let id = ids->hd;
        qvar_t qv = new $(Rel_n(NULL), id);
        let vd = new_vardecl(qv,wildtyp(NULL),NULL);
        vds = new List(vd,vds);
      }
      vds = List::imp_rev(vds);
      $$=^$(new List(letv_decl(vds,LOC(@1,@3)),NULL));
    }
/* Cyc: region declaration */
/* region <`r> h;  and region <`r> h @resetable; */
| resetable_qual_opt REGION '<' TYPE_VAR '>' IDENTIFIER ';'
  { if (zstrcmp($4,"`H") == 0)
      err(aprintf("bad occurrence of heap region"),LOC(@4,@4));
    if (zstrcmp($4,"`U") == 0)
      err(aprintf("bad occurrence of unique region"),LOC(@4,@4));
    tvar_t tv = new Tvar(new $4,-1,Tcutil::kind_to_bound(&Tcutil::rk));
    type_t t  = new VarType(tv);
    vardecl_t vd = new_vardecl(new $(Loc_n,new $6),new RgnHandleType(t),NULL);
    $$ = ^$(new List(region_decl(tv,vd,$1,NULL,LOC(@1,@7)),NULL));
  }
/* region h;   and  region h = open(e); */
| resetable_qual_opt REGION IDENTIFIER open_exp_opt ';'
  { if (zstrcmp($3,"H") == 0)
      err("bad occurrence of heap region `H",LOC(@3,@3));
    if (zstrcmp($3,"U") == 0)
      err("bad occurrence of unique region `U",LOC(@3,@3));
    if ($1 && ($4 != NULL))
      err("open regions cannot be @resetable",LOC(@1,@5));
    tvar_t tv = new Tvar(new (string_t)aprintf("`%s",$3), -1,
			 Tcutil::kind_to_bound(&Tcutil::rk));
    type_t t = new VarType(tv);
    vardecl_t vd = new_vardecl(new $(Loc_n,new $3),new RgnHandleType(t),NULL);

    $$ = ^$(new List(region_decl(tv,vd,$1,$4,LOC(@1,@5)),NULL));
  }
/* Cyc: alias <r>t v = e; */
| ALIAS '<' TYPE_VAR '>' IDENTIFIER '=' expression ';'
  { tvar_t tv = new Tvar(new $3,-1,new Eq_kb(&Tcutil::rk));
    vardecl_t vd = new_vardecl(new $(Loc_n, new $5),&VoidType_val,NULL);
    $$ = ^$(new List(alias_decl($7,tv,vd,LOC(@1,@8)),NULL));
  }
;

resetable_qual_opt:
  /* empty */ { $$=^$(false); }
| RESETABLE_QUAL { $$=^$(true); }
;

declaration_list:
  declaration
    { $$=$!1; }
| declaration_list declaration
    { $$=^$(List::imp_append($1,$2)); }
;

/* Cyc: added optional attributes */
declaration_specifiers:
  storage_class_specifier
    { $$=^$(new Declaration_spec(new Opt($1),empty_tqual(LOC(@1,@1)),
                                 NULL,false,NULL)); }
| storage_class_specifier declaration_specifiers
    { if ($2->sc != NULL)
        Tcutil::warn(LOC(@1,@2),
                     "Only one storage class is allowed in a declaration");
      $$=^$(new Declaration_spec(new Opt($1),$2->tq,$2->type_specs,
                                 $2->is_inline,
                                 $2->attributes));
    }
| type_specifier
    { $$=^$(new Declaration_spec(NULL,empty_tqual(LOC(@1,@1)),
                                 new List($1,NULL),false,NULL)); }
| type_specifier declaration_specifiers
    { $$=^$(new Declaration_spec($2->sc,$2->tq,new List($1,$2->type_specs),
                                 $2->is_inline,
                                 $2->attributes));
    }
| type_qualifier
    { $$=^$(new Declaration_spec(NULL,$1,NULL,false,NULL)); }
| type_qualifier declaration_specifiers
    { $$=^$(new Declaration_spec($2->sc,combine_tqual($1,$2->tq),
                                 $2->type_specs, $2->is_inline,
                                 $2->attributes));
    }
| INLINE
    { $$=^$(new Declaration_spec(NULL,empty_tqual(LOC(@1,@1)),
                                 NULL,true,NULL)); }
| INLINE declaration_specifiers
    { $$=^$(new Declaration_spec($2->sc,$2->tq,$2->type_specs,true,
                                 $2->attributes));
    }
| attributes
    { $$=^$(new Declaration_spec(NULL,empty_tqual(LOC(@1,@1)),
                                 NULL,false,$1)); }
| attributes declaration_specifiers
    { $$=^$(new Declaration_spec($2->sc,$2->tq,
                                 $2->type_specs, $2->is_inline,
                                 List::imp_append($1,$2->attributes))); }
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
  /* empty */  { $$=^$(NULL); }
  | attributes { $$=$!1; }
;

attributes:
  ATTRIBUTE '(' '(' attribute_list ')' ')'
  { $$=$!4; }
;

attribute_list:
  attribute { $$=^$(new List($1,NULL)); };
| attribute ',' attribute_list { $$=^$(new List($1,$3)); }
;

attribute:
  IDENTIFIER
  { static datatype Attribute.Aligned_att att_aligned = Aligned_att(-1);
  static $(string_t,datatype Attribute @) att_map[] = {
    $("stdcall", &Stdcall_att_val),
    $("cdecl", &Cdecl_att_val),
    $("fastcall", &Fastcall_att_val),
    $("noreturn", &Noreturn_att_val),
    $("const", &Const_att_val), // a keyword (see grammar), but __const__ possible
    $("aligned", (datatype Attribute @)&att_aligned), // WARNING: sharing!
    $("packed", &Packed_att_val),
    $("shared", &Shared_att_val),
    $("unused", &Unused_att_val),
    $("weak", &Weak_att_val),
    $("dllimport", &Dllimport_att_val),
    $("dllexport", &Dllexport_att_val),
    $("no_instrument_function", &No_instrument_function_att_val),
    $("constructor", &Constructor_att_val),
    $("destructor", &Destructor_att_val),
    $("no_check_memory_usage", &No_check_memory_usage_att_val),
    $("pure",&Pure_att_val),
  };
    let s = $1;
    // drop the surrounding __ in s, if it's there
    if(numelts(s) > 4 && s[0]=='_' && s[1]=='_'
       && s[numelts(s)-2]=='_' && s[numelts(s)-3]=='_')
      s = substring(s,2,numelts(s)-5);
    int i=0;
    for(; i < numelts(att_map); ++i)
      if(strcmp(s,att_map[i][0]) == 0) {
	$$=^$(att_map[i][1]);
	break;
      }
    if(i == numelts(att_map)) {
      err("unrecognized attribute",LOC(@1,@1));
      $$ = ^$(&Cdecl_att_val);
    }
  }
| CONST { $$=^$(&Const_att_val); }
| IDENTIFIER '(' INTEGER_CONSTANT ')'
  { let s = $1;
    let $(_,n) = $3;
    attribute_t a;
    if (zstrcmp(s,"regparm") == 0 || zstrcmp(s,"__regparm__") == 0) {
      if (n < 0 || n > 3) 
	err("regparm requires value between 0 and 3", LOC(@3,@3));
      a = new Regparm_att(n);
    } else if (zstrcmp(s,"aligned") == 0 || zstrcmp(s,"__aligned__") == 0) {
      if (n < 0) err("aligned requires positive power of two", LOC(@3,@3));
      unsigned int j = (unsigned int)n;
      for (; (j & 1) == 0; j = j >> 1)
	;
      j = j >> 1;
      if (j != 0) err("aligned requires positive power of two",LOC(@3,@3));
      a = new Aligned_att(n);
    } else if(zstrcmp(s,"initializes")==0 || zstrcmp(s,"__initializes__")==0) {
      a = new Initializes_att(n);
    } else if(zstrcmp(s,"noliveunique")==0 || zstrcmp(s,"__noliveunique__")==0){
      a = new Noliveunique_att(n);
    } else {
      err("unrecognized attribute",LOC(@1,@1));
      a = &Cdecl_att_val;
    }
    $$=^$(a);
  }
| IDENTIFIER '(' STRING ')'
  { let s = $1;
    let str = $3;
    attribute_t a;
    if (zstrcmp(s,"section") == 0 || zstrcmp(s,"__section__") == 0)
      a = new Section_att(str);
    else {
      err("unrecognized attribute",LOC(@1,@1));
      a = &Cdecl_att_val;
    }
    $$=^$(a);
  }
| IDENTIFIER '(' IDENTIFIER ')'
  { let s = $1;
    let str = $3;
    attribute_t a;
    // showed up in FreeBSD
    if (zstrcmp(s,"__mode__") == 0)
      a = new Mode_att(str);
    else {
      err("unrecognized attribute",LOC(@1,@1));
      a = &Cdecl_att_val;
    }
    $$=^$(a);
  }
| IDENTIFIER '(' IDENTIFIER ',' INTEGER_CONSTANT ',' INTEGER_CONSTANT ')'
  { let s = $1; 
    let t = $3;
    let $(_,n) = $5;
    let $(_,m) = $7;
    attribute_t a = &Cdecl_att_val;
    if (zstrcmp(s,"format") == 0 || zstrcmp(s,"__format__") == 0)
      if (zstrcmp(t,"printf") == 0)
        a = new Format_att(Printf_ft,n,m);
      else if (zstrcmp(t,"scanf") == 0)
        a = new Format_att(Scanf_ft,n,m);
      else
        err("unrecognized format type",LOC(@3,@3)); 
    else 
      err("unrecognized attribute",LOC(@1,@8)); 
    $$ = ^$(a);
  }
;

/***************************** TYPES *****************************/
/* we could be parsing a type or a type declaration (e.g., struct)
 * so we return a type_specifier value -- either a type, a type qualifier, 
 * an integral type qualifier (signed, long, etc.) or a declaration.
 */
type_specifier:
  type_specifier_notypedef
    { $$ = $!1; }
| qual_opt_typedef type_params_opt
    { $$=^$(type_spec(new TypedefType($1,$2,NULL,NULL),LOC(@1,@2))); }
;

type_specifier_notypedef:
  VOID      { $$=^$(type_spec(&VoidType_val,LOC(@1,@1))); }
| '_'       { $$=^$(type_spec(new_evar(NULL,NULL),LOC(@1,@1))); }
| '_' COLON_COLON kind 
  { $$=^$(type_spec(new_evar(Tcutil::kind_to_opt($3),NULL),LOC(@1,@3))); }
| CHAR      { $$=^$(type_spec(char_typ,LOC(@1,@1))); }
| SHORT     { $$=^$(new Short_spec(LOC(@1,@1))); }
| INT       { $$=^$(type_spec(sint_typ,LOC(@1,@1))); }
| LONG      { $$=^$(new Long_spec(LOC(@1,@1))); }
| FLOAT     { $$=^$(type_spec(float_typ,LOC(@1,@1))); }
| DOUBLE    { $$=^$(type_spec(double_typ(false),LOC(@1,@1))); }
| SIGNED    { $$=^$(new Signed_spec(LOC(@1,@1))); }
| UNSIGNED  { $$=^$(new Unsigned_spec(LOC(@1,@1))); }
| enum_specifier { $$=$!1; }
| struct_or_union_specifier { $$=$!1; }
/* Cyc: added datatypes */
| datatype_specifier { $$=$!1; }
/* Cyc: added type variables and optional type parameters to typedef'd names */
| type_var { $$=^$(type_spec($1, LOC(@1,@1))); }
/* Cyc: everything below here is an addition */
| '$' '(' parameter_list ')'
    { $$=^$(type_spec(new TupleType(List::map_c(get_tqual_typ,
                                                LOC(@3,@3),List::imp_rev($3))),
                      LOC(@1,@4))); }
| REGION_T '<' any_type_name right_angle
    { $$=^$(type_spec(new RgnHandleType($3),LOC(@1,@4))); }
| REGION_T 
    { $$=^$(type_spec(new RgnHandleType(new_evar(&Tcutil::rko, NULL)),
                      LOC(@1,@1))); }
| DYNREGION_T '<' any_type_name right_angle
  { let t2 = new_evar(&Tcutil::rko, NULL);
    $$=^$(type_spec(new DynRgnType($3,t2), LOC(@1,@4)));
  }
| DYNREGION_T '<' any_type_name ',' any_type_name right_angle
  { $$=^$(type_spec(new DynRgnType($3,$5), LOC(@1,@6))); }
| TAG_T '<' any_type_name right_angle
    { $$=^$(type_spec(new TagType($3),LOC(@1,@4))); }
| TAG_T 
  { $$=^$(type_spec(new TagType(new_evar(&Tcutil::iko, NULL)),LOC(@1,@1))); }
| VALUEOF_T '(' expression ')'
  { $$=^$(type_spec(new ValueofType($3),LOC(@1,@4))); }
;

/* Cyc: new */
kind:
  field_name { $$=^$(id_to_kind($1,LOC(@1,@1))); }
;

type_qualifier:
  CONST    { seg_t loc = (Absyn::porting_c_code) ? LOC(@1,@1) : NULL; 
             $$=^$(Tqual(true,false,false,true,loc)); }
| VOLATILE { $$=^$(Tqual(false,true,false,false,NULL)); }
| RESTRICT { $$=^$(Tqual(false,false,true,false,NULL)); }
;

/* parsing of enum specifiers */
enum_specifier:
  ENUM qual_opt_identifier '{' enum_declaration_list '}'
  { $$=^$(new Decl_spec(new Decl{new Enum_d(new Enumdecl(Public,$2,new Opt($4))),
                                 LOC(@1,@5)})); }
| ENUM qual_opt_identifier
  { $$=^$(type_spec(new EnumType($2,NULL),LOC(@1,@2))); }
| ENUM '{' enum_declaration_list '}'
  { $$=^$(new Type_spec(new AnonEnumType($3),LOC(@1,@4))); }
;

/* enum fields */
enum_field:
  qual_opt_identifier
  { $$ = ^$(new Enumfield($1,NULL,LOC(@1,@1))); }
| qual_opt_identifier '=' constant_expression
  { $$ = ^$(new Enumfield($1,$3,LOC(@1,@3))); }
;

enum_declaration_list:
  enum_field { $$ = ^$(new List($1,NULL)); }
| enum_field ',' { $$ = ^$(new List($1,NULL)); }
| enum_field ',' enum_declaration_list { $$ = ^$(new List($1,$3)); }
;

/* parsing of struct and union specifiers. */
struct_or_union_specifier:
  struct_or_union '{' struct_declaration_list '}'
  { $$=^$(type_spec(new AnonAggrType($1,$3),LOC(@1,@4))); }
/* Cyc:  TAGGED_QUAL, type_params_opt are added */
| TAGGED_QUAL struct_or_union struct_union_name type_params_opt '{' 
    type_params_opt optional_rgn_order 
  struct_declaration_list '}'
    { 
      let ts = List::map_c(typ2tvar,LOC(@4,@4),$4);
      let exist_ts = List::map_c(typ2tvar,LOC(@6,@6),$6);
      $$=^$(new Decl_spec(aggr_decl($2, Public, $3, ts,
				    aggrdecl_impl(exist_ts,$7,$8,true), NULL,
				    LOC(@1,@9))));
    }
| struct_or_union struct_union_name type_params_opt '{' 
    type_params_opt optional_rgn_order 
  struct_declaration_list '}'
    { 
      let ts = List::map_c(typ2tvar,LOC(@3,@3),$3);
      let exist_ts = List::map_c(typ2tvar,LOC(@5,@5),$5);
      $$=^$(new Decl_spec(aggr_decl($1, Public, $2, ts,
				    aggrdecl_impl(exist_ts,$6,$7,false), NULL,
				    LOC(@1,@8))));
    }
| TAGGED_QUAL struct_or_union struct_union_name type_params_opt 
    { $$=^$(type_spec(new AggrType(AggrInfo(UnknownAggr($2,$3,new Opt(true)),$4)),
		      LOC(@1,@4)));
    }
| struct_or_union struct_union_name type_params_opt 
    { $$=^$(type_spec(new AggrType(AggrInfo(UnknownAggr($1,$2,NULL),$3)),
		      LOC(@1,@3)));
    }
;

type_params_opt:
  /* empty */
    { $$=^$(NULL); }
| '<' type_name_list right_angle
    { $$=^$(List::imp_rev($2)); }
;

struct_or_union:
  STRUCT { $$=^$(StructA); }
| UNION  { $$=^$(UnionA); }
;

struct_declaration_list:
  /* empty */
  { $$=^$(NULL); }
| struct_declaration_list0 
  { let decls = List::flatten(List::imp_rev($1));
    let tags = get_argrfield_tags(decls);
    if (tags != NULL)
      List::iter_c(substitute_aggrfield_tags,tags,decls);
    $$=^$(decls);
  }
;

/* NB: returns list in reverse order */
struct_declaration_list0:
  struct_declaration
    { $$=^$(new List($1,NULL)); }
| struct_declaration_list0 struct_declaration
    { $$=^$(new List($2,$1)); }
;

init_declarator_list:
  init_declarator_list0 { $$=^$(List::imp_rev($1)); }
;

/* NB: returns list in reverse order */
init_declarator_list0:
  init_declarator
    { $$=^$(new List($1,NULL)); }
| init_declarator_list0 ',' init_declarator
    { $$=^$(new List($3,$1)); }
;

init_declarator:
  declarator
    { $$=^$(new $($1,NULL)); }
| declarator '=' initializer
    { $$=^$(new $($1,(exp_opt_t)$3)); } // FIX: cast needed
;

struct_declaration:
  specifier_qualifier_list struct_declarator_list ';'
    {
      /* when we collapse the specifier_qualifier_list and
       * struct_declarator_list, we get a list of (1) optional id,
       * (2) type, and (3) in addition any nested struct, union,
       * or datatype declarations.  For now, we warn about the nested
       * declarations.  We must check that each id is actually present
       * and then convert this to a list of struct fields: (1) id,
       * (2) tqual, (3) type. */
      region temp; {
        let &$(tq,tspecs,atts) = $1;
        if (tq.loc == NULL) tq.loc = LOC(@1,@1);
        let $(decls, widths) = List::rsplit(temp,temp,$2);
        let t = speclist2typ(tspecs, LOC(@1,@1));
        let info = List::rzip(temp,temp,
                              apply_tmss(temp,tq,t,decls,atts),widths);
        $$=^$(List::map_c(make_aggr_field,LOC(@1,@2),info));
      }
    }
;

// we do this slightly differently:  at most one typedef name can occur
// within a specifier qualifier list, and that is useful since we can
// then allow certain declarators to use typedef names as the declared
// identifier.
specifier_qualifier_list:
  type_specifier
    { $$=^$(new $(empty_tqual(LOC(@1,@1)), new List($1,NULL), NULL)); }
| type_specifier notypedef_specifier_qualifier_list
    { $$=^$(new $((*$2)[0], new List($1,(*$2)[1]), (*$2)[2]));}
| type_qualifier
    { $$=^$(new $($1,NULL,NULL)); }
| type_qualifier specifier_qualifier_list
    { $$=^$(new $(combine_tqual($1,(*$2)[0]), (*$2)[1], (*$2)[2])); }
| attributes
    { $$=^$(new $(empty_tqual(LOC(@1,@1)), NULL, $1)); }
| attributes specifier_qualifier_list
    { $$=^$(new $((*$2)[0], (*$2)[1], append($1,(*$2)[2]))); }
;

// Same as above but does not allow typedef names as specifiers
notypedef_specifier_qualifier_list:
  type_specifier_notypedef
    { $$=^$(new $(empty_tqual(LOC(@1,@1)), new List($1,NULL), NULL)); }
| type_specifier_notypedef notypedef_specifier_qualifier_list
    { $$=^$(new $((*$2)[0], new List($1,(*$2)[1]), (*$2)[2]));}
| type_qualifier
    { $$=^$(new $($1,NULL,NULL)); }
| type_qualifier notypedef_specifier_qualifier_list
    { $$=^$(new $(combine_tqual($1,(*$2)[0]), (*$2)[1], (*$2)[2])); }
| attributes
    { $$=^$(new $(empty_tqual(LOC(@1,@1)), NULL, $1)); }
| attributes notypedef_specifier_qualifier_list
    { $$=^$(new $((*$2)[0], (*$2)[1], append($1,(*$2)[2]))); }
;

struct_declarator_list:
  struct_declarator_list0 { $$=^$(List::imp_rev($1)); }
;

/* NB: returns list in reverse order */
struct_declarator_list0:
  struct_declarator
    { $$=^$(new List($1,NULL)); }
| struct_declarator_list0 ',' struct_declarator
    { $$=^$(new List($3,$1)); }
;

struct_declarator:
  declarator_withtypedef
    { $$=^$(new $($1,NULL)); }
| ':' constant_expression
    { // HACK: give the field an empty name -- see elsewhere in the
      // compiler where we use this invariant
      $$=^$(new $((new Declarator(new $(Rel_n(NULL),new ""), NULL)),
                  (exp_opt_t)$2));
    }
| declarator_withtypedef ':' constant_expression
    { $$=^$(new $($1,(exp_opt_t)$3)); }
;

// FIX: hack to have rgn_opt in 1st and 3rd cases
datatype_specifier:
  qual_datatype qual_opt_identifier type_params_opt '{' datatypefield_list '}'
    { let is_extensible = $1;
      let ts = List::map_c(typ2tvar,LOC(@3,@3),$3);
      $$ = ^$(new Decl_spec(datatype_decl(Public,$2,ts,new Opt($5), 
                                          is_extensible,LOC(@1,@6))));
    }
| qual_datatype qual_opt_identifier type_params_opt 
    { let is_extensible = $1;
      $$=^$(type_spec(new DatatypeType(DatatypeInfo(UnknownDatatype(UnknownDatatypeInfo($2,is_extensible)), $3)), LOC(@1,@3)));
    }
| qual_datatype qual_opt_identifier '.' qual_opt_identifier type_params_opt 
   {  let is_extensible = $1;
      $$=^$(type_spec(new DatatypeFieldType(DatatypeFieldInfo(
		  UnknownDatatypefield(UnknownDatatypeFieldInfo($2,$4,is_extensible)),$5)),
		      LOC(@1,@5)));
    }
;

qual_datatype:
  DATATYPE { $$ = ^$(false); }
| EXTENSIBLE_QUAL DATATYPE { $$ = ^$(true); }
;

datatypefield_list:
  datatypefield                      { $$=^$(new List($1,NULL)); }
| datatypefield ';'                  { $$=^$(new List($1,NULL)); }
| datatypefield ',' datatypefield_list { $$=^$(new List($1,$3)); }
| datatypefield ';' datatypefield_list { $$=^$(new List($1,$3)); }
;

datatypefield_scope:
         { $$=^$(Public);}
| EXTERN { $$=^$(Extern);}
| STATIC { $$=^$(Static);}

datatypefield:
  datatypefield_scope qual_opt_identifier
    { $$=^$(new Datatypefield($2,NULL,LOC(@1,@2),$1)); }
| datatypefield_scope qual_opt_identifier '(' parameter_list ')'
    { let typs = List::map_c(get_tqual_typ,LOC(@4,@4),List::imp_rev($4));
      $$=^$(new Datatypefield($2,typs,LOC(@1,@5),$1)); }
;

declarator:
  direct_declarator
    { $$=$!1; }
| pointer direct_declarator
    { $$=^$(new Declarator($2->id,List::imp_append($1,$2->tms))); }
;

// same as declarator but allows typedef names to occur as the variable
declarator_withtypedef:
  direct_declarator_withtypedef
    { $$=$!1; }
| pointer direct_declarator_withtypedef
    { $$=^$(new Declarator($2->id,List::imp_append($1,$2->tms))); }
;

direct_declarator:
  qual_opt_identifier
    { $$=^$(new Declarator($1,NULL)); }
| '(' declarator ')'
    { $$=$!2; }
/* the following rule causes a large number of shift/reduce conflicts
 * but seems necessary to parse some of the GCC header files. */
| '(' attributes declarator ')'
    { let d = $3;
      d->tms = new List(new Attributes_mod(LOC(@2,@2),$2),d->tms);
      $$=$!3;
    }
| direct_declarator '[' ']' zeroterm_qual_opt
    { $$=^$(new Declarator($1->id,new List(new Carray_mod($4,LOC(@4,@4)),$1->tms)));}
| direct_declarator '[' assignment_expression ']' zeroterm_qual_opt
    { $$=^$(new Declarator($1->id,
                           new List(new ConstArray_mod($3,$5,LOC(@5,@5)),$1->tms)));}
| direct_declarator '(' parameter_type_list ')'
    { let &$(lis,b,c,eff,po) = $3;
      $$=^$(new Declarator($1->id,new List(new Function_mod(new WithTypes(lis,b,c,eff,po)),$1->tms)));
    }
| direct_declarator '(' optional_effect optional_rgn_order ')'
    { $$=^$(new Declarator($1->id,
                           new List(new Function_mod(new WithTypes(NULL,
                                                                   false,NULL,
                                                                   $3,$4)),
                                    $1->tms)));
    }
| direct_declarator '(' identifier_list ')'
    { $$=^$(new Declarator($1->id,new List(new Function_mod(new NoTypes($3,LOC(@1,@4))),$1->tms))); }
/* Cyc: added type parameters */
| direct_declarator '<' type_name_list right_angle
    { let ts = List::map_c(typ2tvar,LOC(@2,@4),List::imp_rev($3));
      $$=^$(new Declarator($1->id,new List(new TypeParams_mod(ts,LOC(@1,@4),false),$1->tms)));
    }
| direct_declarator attributes
  { $$=^$(new Declarator($1->id,new List(new Attributes_mod(LOC(@2,@2),$2),
                                         $1->tms)));
  }
/* These two cases help to improve error messages */
//| qual_opt_identifier qual_opt_identifier
//{ err("identifier has not been declared as a typedef",LOC(@1,@1));
//  $$=^$(new Declarator($2,NULL)); }   
/* | error 
   { $$=^$(new Declarator(exn_name,NULL)); } */
;

// same as direct_declarator but allows typedef names to occur as the variable
direct_declarator_withtypedef:
  qual_opt_identifier
    { $$=^$(new Declarator($1,NULL)); }
| qual_opt_typedef
    { $$=^$(new Declarator($1,NULL)); }
| '(' declarator_withtypedef ')'
    { $$=$!2; }
/* the following rule causes a large number of shift/reduce conflicts
 * but seems necessary to parse some of the GCC header files. */
| '(' attributes declarator_withtypedef ')'
    { let d = $3;
      d->tms = new List(new Attributes_mod(LOC(@2,@2),$2),d->tms);
      $$=$!3;
    }
| direct_declarator_withtypedef '[' ']' zeroterm_qual_opt
    { $$=^$(new Declarator($1->id,new List(new Carray_mod($4,LOC(@4,@4)),$1->tms)));}
| direct_declarator_withtypedef '[' assignment_expression ']' zeroterm_qual_opt
    { $$=^$(new Declarator($1->id,
                           new List(new ConstArray_mod($3,$5,LOC(@5,@5)),$1->tms)));}
| direct_declarator_withtypedef '(' parameter_type_list ')'
    { let &$(lis,b,c,eff,po) = $3;
      $$=^$(new Declarator($1->id,new List(new Function_mod(new WithTypes(lis,b,c,eff,po)),$1->tms)));
    }
| direct_declarator_withtypedef '(' optional_effect optional_rgn_order ')'
    { $$=^$(new Declarator($1->id,
                           new List(new Function_mod(new WithTypes(NULL,
                                                                   false,NULL,
                                                                   $3,$4)),
                                    $1->tms)));
    }
| direct_declarator_withtypedef '(' identifier_list ')'
    { $$=^$(new Declarator($1->id,new List(new Function_mod(new NoTypes($3,LOC(@1,@4))),$1->tms))); }
/* Cyc: added type parameters */
| direct_declarator_withtypedef '<' type_name_list right_angle
    { let ts = List::map_c(typ2tvar,LOC(@2,@4),List::imp_rev($3));
      $$=^$(new Declarator($1->id,new List(new TypeParams_mod(ts,LOC(@1,@4),false),$1->tms)));
    }
| direct_declarator_withtypedef attributes
  { $$=^$(new Declarator($1->id,new List(new Attributes_mod(LOC(@2,@2),$2),
                                         $1->tms)));
  }
/* These two cases help to improve error messages */
//| qual_opt_identifier qual_opt_identifier
//{ err("identifier has not been declared as a typedef",LOC(@1,@1));
//  $$=^$(new Declarator($2,NULL)); }   
/* | error 
   { $$=^$(new Declarator(exn_name,NULL)); } */
;

/* CYC: region annotations allowed, as is zero-termination qualifier */
pointer:
  one_pointer { $$ = $!1; }
| one_pointer pointer { $$ = ^$(imp_append($1,$2)); }

one_pointer:
  pointer_null_and_bound pointer_quals rgn_opt attributes_opt tqual_list
  { list_t<type_modifier_t> ans = NULL;
    if($4 != NULL)
      ans = new List(new Attributes_mod(LOC(@4,@4),$4), ans);
    // don't put location info on every pointer -- too expensive
    ptrloc_t ptrloc = NULL;
    let $(ploc,nullable,bound) = *$1;
    if (Absyn::porting_c_code)
      ptrloc = new PtrLoc{.ptr_loc=ploc,.rgn_loc=LOC(@3,@3),
                          .zt_loc=LOC(@2,@2)};
    let $(nullable,bound,zeroterm,rgn_opt) = collapse_pointer_quals(ploc,nullable,bound,$3,$2);
    ans = new List(new Pointer_mod(PtrAtts(rgn_opt,nullable,bound,zeroterm,ptrloc),$5), ans);
    $$ = ^$(ans);
  }

pointer_quals:
  /* empty */ { $$=^$(NULL); }
| pointer_qual pointer_quals { $$=^$(new List($1,$2)); }
;

pointer_qual:
  NUMELTS_QUAL '(' assignment_expression ')' 
  { $$ = ^$(new Numelts_ptrqual($3)); }
| REGION_QUAL '(' any_type_name ')'
  { $$ = ^$(new Region_ptrqual($3)); }
| THIN_QUAL
  { $$ = ^$(new Thin_ptrqual); }
| FAT_QUAL
  { $$ = ^$(new Fat_ptrqual); }
| ZEROTERM_QUAL
  { $$ = ^$(new Zeroterm_ptrqual); }
| NOZEROTERM_QUAL
  { $$ = ^$(new Nozeroterm_ptrqual); }
| NOTNULL_QUAL
  { $$ = ^$(new Notnull_ptrqual); }
| NULLABLE_QUAL
  { $$ = ^$(new Nullable_ptrqual); }
;

pointer_null_and_bound:
  '*' pointer_bound 
   { // avoid putting location info on here when not porting C code
     seg_t loc = LOC(@1,@1);
     $$=^$(new $(loc,true_conref, $2)); 
   }
| '@' pointer_bound 
  {  seg_t loc = LOC(@1,@1);
     $$=^$(new $(loc, false_conref, $2)); 
  }
| '?' 
  { seg_t loc = LOC(@1,@1);
    $$=^$(new $(loc, true_conref,  bounds_dyneither_conref));  
  }

pointer_bound:
/* empty */ { $$=^$(bounds_one_conref); }
| '{' assignment_expression '}' { $$=^$(new_conref(new Upper_b($2))); }

zeroterm_qual_opt:
/* empty */       { $$ = ^$(empty_conref()); }
| ZEROTERM_QUAL   { $$ = ^$(true_conref);    }
| NOZEROTERM_QUAL { $$ = ^$(false_conref);   }
;

/* Always returns a type (possibly an evar) */
rgn_opt:
/* empty */ { $$ = ^$(new_evar(&Tcutil::trko,NULL)); }
| type_var  { set_vartyp_kind($1,&Tcutil::trk,true); $$ = $!1; }
| '_'       { $$ = ^$(new_evar(&Tcutil::trko,NULL)); }
;

tqual_list:
/* empty */ { $$ = ^$(empty_tqual(LOC(@1,@1))); }
| type_qualifier tqual_list { $$ = ^$(combine_tqual($1,$2)); }
;

parameter_type_list:
  parameter_list optional_effect optional_rgn_order
    { $$=^$(new $(List::imp_rev($1),false,NULL,$2,$3)); }
| parameter_list ',' ELLIPSIS optional_effect optional_rgn_order
    { $$=^$(new $(List::imp_rev($1),true,NULL,$4,$5)); }
| ELLIPSIS optional_inject parameter_declaration optional_effect
  optional_rgn_order
{ let &$(n,tq,t) = $3;
  let v = new VarargInfo {.name = n,.tq = tq,.type = t,.inject = $2};
  $$=^$(new $(NULL,false,v,$4,$5)); 
}
| parameter_list ',' ELLIPSIS optional_inject parameter_declaration
  optional_effect optional_rgn_order
{ let &$(n,tq,t) = $5;
  let v = new VarargInfo {.name = n,.tq = tq,.type = t,.inject = $4};
  $$=^$(new $(List::imp_rev($1),false,v,$6,$7)); 
}
;

/* CYC:  new */
type_var:
  TYPE_VAR                  { $$ = ^$(id2type($1,new Unknown_kb(NULL))); }
| TYPE_VAR COLON_COLON kind { $$ = ^$(id2type($1,Tcutil::kind_to_bound($3))); }

optional_effect:
  /* empty */    { $$=^$(NULL); }
| ';' effect_set { $$ = ^$(new Opt(new JoinEff($2))); }
;

optional_rgn_order:
  /* empty */    { $$ = ^$(NULL); }
| ':' rgn_order  { $$ = $!2; }
;

rgn_order:
  atomic_effect '>' TYPE_VAR
  { // FIX: if we replace the following with:
    // $$ = ^$(new List(new $(new JoinEff($1),id2type(id,new Less_kb(NULL,TopRgnKind))), NULL));
    // then we get a core-dump.  I think it must be the gcc bug...
    let kb = new Less_kb(NULL,&Tcutil::trk);
    let id = $3;
    let t = id2type(id,kb);
    $$ = ^$(new List(new $(new JoinEff($1),t), NULL));
  }
| atomic_effect '>' TYPE_VAR ',' rgn_order 
  { 
    let kb = new Less_kb(NULL,&Tcutil::trk);
    let id = $3;
    let t = id2type(id,kb);
    $$ = ^$(new List(new $(new JoinEff($1),t),$5)); 
  }
;

optional_inject:
  /* empty */
  {$$ = ^$(false);}
| IDENTIFIER
     { if (zstrcmp($1,"inject") != 0) 
         err("missing type in function declaration",LOC(@1,@1));
       $$ = ^$(true);
     }
;

effect_set:
  atomic_effect { $$=$!1; }
| atomic_effect '+' effect_set { $$=^$(List::imp_append($1,$3)); }
;

atomic_effect:
  '{' '}'                   { $$=^$(NULL); }
| '{' region_set '}'        { $$=$!2; }
| REGIONS '(' any_type_name ')'
  { $$=^$(new List(new RgnsEff($3), NULL)); }
| type_var
  { set_vartyp_kind($1,&Tcutil::ek,false);
    $$ = ^$(new List($1,NULL)); 
  }
;

/* CYC:  new */
region_set:
  type_var
  { set_vartyp_kind($1,&Tcutil::trk,true);
    $$=^$(new List(new AccessEff($1),NULL)); 
  }
| type_var ',' region_set
  { set_vartyp_kind($1,&Tcutil::trk,true);
    $$=^$(new List(new AccessEff($1),$3)); 
  }
;

/* NB: returns list in reverse order */
parameter_list:
  parameter_declaration
    { $$=^$(new List($1,NULL)); }
| parameter_list ',' parameter_declaration
    { $$=^$(new List($3,$1)); }
;

/* FIX: differs from grammar in K&R */
parameter_declaration:
  specifier_qualifier_list declarator_withtypedef
    { let &$(tq,tspecs,atts) = $1; 
      if (tq.loc == NULL) tq.loc = LOC(@1,@1);
      let &Declarator(qv,tms) = $2;
      let t = speclist2typ(tspecs, LOC(@1,@1));
      let $(tq2,t2,tvs,atts2) = apply_tms(tq,t,atts,tms);
      if (tvs != NULL)
        err("parameter with bad type params",LOC(@2,@2));
      if(is_qvar_qualified(qv))
        err("parameter cannot be qualified with a namespace",LOC(@1,@1));
      let idopt = (opt_t<var_t>)(new Opt((*qv)[1]));
      if (atts2 != NULL)
        Tcutil::warn(LOC(@1,@2),"extra attributes on parameter, ignoring");
      $$=^$(new $(idopt,tq2,t2));
    }
| specifier_qualifier_list
    { let &$(tq,tspecs,atts) = $1; 
      if (tq.loc == NULL) tq.loc = LOC(@1,@1);
      let t = speclist2typ(tspecs, LOC(@1,@1));
      if (atts != NULL)
        Tcutil::warn(LOC(@1,@1),"bad attributes on parameter, ignoring");
      $$=^$(new $(NULL,tq,t));
    }
| specifier_qualifier_list abstract_declarator
    { let &$(tq,tspecs,atts) = $1; 
      if (tq.loc == NULL) tq.loc = LOC(@1,@1);
      let t = speclist2typ(tspecs, LOC(@1,@1));
      let tms = $2->tms;
      let $(tq2,t2,tvs,atts2) = apply_tms(tq,t,atts,tms);
      if (tvs != NULL) // Ex: int (@)<`a>
        Tcutil::warn(LOC(@1,@2),
                     "bad type parameters on formal argument, ignoring");
      if (atts2 != NULL)
        Tcutil::warn(LOC(@1,@2),"bad attributes on parameter, ignoring");
      $$=^$(new $(NULL,tq2,t2));
    }
;

identifier_list:
  identifier_list0
    { $$=^$(List::imp_rev($1)); }
;
identifier_list0:
  IDENTIFIER
    { $$=^$(new List(new $1,NULL)); }
| identifier_list0 ',' IDENTIFIER
    { $$=^$(new List(new $3,$1)); }
;

initializer:
  assignment_expression { $$=$!1; }
| array_initializer     { $$=$!1; }
;

array_initializer:
  '{' '}'
    { $$=^$(new_exp(new UnresolvedMem_e(NULL,NULL),LOC(@1,@2))); }
| '{' initializer_list '}'
    { $$=^$(new_exp(new UnresolvedMem_e(NULL,List::imp_rev($2)),LOC(@1,@3))); }
| '{' initializer_list ',' '}'
    { $$=^$(new_exp(new UnresolvedMem_e(NULL,List::imp_rev($2)),LOC(@1,@4))); }
| '{' FOR IDENTIFIER '<' expression ':' expression '}'
    { let vd = new_vardecl(new $(Loc_n,new $3), uint_typ,
                           uint_exp(0,LOC(@3,@3)));
      // make the index variable const
      vd->tq.real_const = true;
      $$=^$(new_exp(new Comprehension_e(vd, $5, $7, false),LOC(@1,@8)));
    }
;

/* NB: returns list in reverse order */
initializer_list:
  initializer
    { $$=^$(new List(new $(NULL,$1),NULL)); }
| designation initializer
    { $$=^$(new List(new $($1,$2),NULL)); }
| initializer_list ',' initializer
    { $$=^$(new List(new $(NULL,$3),$1)); }
| initializer_list ',' designation initializer
    { $$=^$(new List(new $($3,$4),$1)); }
;

designation:
  designator_list '=' { $$=^$(List::imp_rev($1)); }
;

/* NB: returns list in reverse order */
designator_list:
  designator                 { $$=^$(new List($1,NULL)); }
| designator_list designator { $$=^$(new List($2,$1));   }
;

designator:
  '[' constant_expression ']' {$$ = ^$(new ArrayElement($2));}
| '.' field_name              {$$ = ^$(new FieldName(new $2));}
;

type_name:
  specifier_qualifier_list
    { let &$(tq,tss,atts) = $1;
      let t = speclist2typ(tss, LOC(@1,@1));
      if (atts != NULL)
        Tcutil::warn(LOC(@1,@1),"ignoring attributes in type");
      $$=^$(new $(NULL,tq,t));
    }
| specifier_qualifier_list abstract_declarator
    { let &$(tq,tss,atts) = $1;
      let t = speclist2typ(tss, LOC(@1,@1));
      let tms = $2->tms;
      let t_info = apply_tms(tq,t,atts,tms);
      if (t_info[2] != NULL)
        // Ex: int (@)<`a>
        Tcutil::warn(LOC(@2,@2),"bad type params, ignoring");
      if (t_info[3] != NULL)
        Tcutil::warn(LOC(@2,@2),"bad specifiers, ignoring");
      $$=^$(new $(NULL,t_info[0],t_info[1]));
    }
;

any_type_name:
  type_name { $$ = ^$((*$1)[2]); }
| '{' '}' { $$ = ^$(new JoinEff(NULL)); }
| '{' region_set '}' { $$ = ^$(new JoinEff($2)); }
| REGIONS '(' any_type_name ')' { $$ = ^$(new RgnsEff($3)); }
| any_type_name '+' atomic_effect { $$ = ^$(new JoinEff(new List($1,$3))); }
;

/* Cyc: new */
/* NB: returns list in reverse order */
type_name_list:
  any_type_name { $$=^$(new List($1,NULL)); }
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
| '[' ']' zeroterm_qual_opt
    { $$=^$(new Abstractdeclarator(new List(new Carray_mod($3,LOC(@3,@3)),NULL))); }
| direct_abstract_declarator '[' ']' zeroterm_qual_opt
    { $$=^$(new Abstractdeclarator(new List(new Carray_mod($4,LOC(@4,@4)),$1->tms))); }
| '[' assignment_expression ']' zeroterm_qual_opt
    { $$=^$(new Abstractdeclarator(new List(new ConstArray_mod($2,$4,LOC(@4,@4)),NULL)));}
| direct_abstract_declarator '[' assignment_expression ']' zeroterm_qual_opt
    { $$=^$(new Abstractdeclarator(new List(new ConstArray_mod($3,$5,LOC(@5,@5)),
                                            $1->tms)));
    }
| '(' optional_effect optional_rgn_order ')'
    { $$=^$(new Abstractdeclarator(new List(new Function_mod(new WithTypes(NULL,false,NULL,$2,$3)),NULL)));
    }
| '(' parameter_type_list ')'
    { let &$(lis,b,c,eff,po) = $2;
      $$=^$(new Abstractdeclarator(new List(new Function_mod(new WithTypes(lis,b,c,eff,po)),NULL)));
    }
| direct_abstract_declarator '(' optional_effect optional_rgn_order ')'
    { $$=^$(new Abstractdeclarator(new List(new Function_mod(new WithTypes(NULL,false,NULL,$3,$4)),
				      $1->tms)));
    }
| direct_abstract_declarator '(' parameter_type_list ')'
    { let &$(lis,b,c,eff,po) = $3;
      $$=^$(new Abstractdeclarator(new List(new Function_mod(new WithTypes(lis,
                                                                           b,c,eff,po)),$1->tms)));
    }
/* Cyc: new */
| direct_abstract_declarator '<' type_name_list right_angle
    { let ts = List::map_c(typ2tvar,LOC(@2,@4),List::imp_rev($3));
      $$=^$(new Abstractdeclarator(new List(new TypeParams_mod(ts,LOC(@2,@4),
                                                               false),
                                      $1->tms)));
    }
| direct_abstract_declarator attributes
    { $$=^$(new Abstractdeclarator(new List(new Attributes_mod(LOC(@2,@2),$2),
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
// Cyc: reset_region(e) statement
| RESET_REGION '(' expression ')' ';'
  { $$=^$(new_stmt(new ResetRegion_s($3),LOC(@1,@5))); }
;

open_exp_opt:
  /* empty */ { $$=^$(NULL); }
| '=' IDENTIFIER '(' expression ')' 
  { if (zstrcmp($2,"open") != 0)
      err("expecting `open'",LOC(@2,@2));
    $$=^$($4);
  }
;

/* Cyc: Unlike C, we do not treat case and default statements as labeled */
labeled_statement:
  IDENTIFIER ':' statement
    { $$=^$(new_stmt(new Label_s(new $1,$3),LOC(@1,@3))); }
;

expression_statement:
  ';'            { $$=^$(skip_stmt(LOC(@1,@1))); }
| expression ';' { $$=^$(exp_stmt($1,LOC(@1,@2))); }
//| error ';'      { $$=^$(skip_stmt(LOC(@1,@1))); } 
;

compound_statement:
  '{' '}'                 { $$=^$(skip_stmt(LOC(@1,@2))); }
| '{' block_item_list '}' { $$=$!2; }
//| '{' error '}'           { $$=^$(skip_stmt(LOC(@1,@2))); }
;

block_item_list:
  declaration { $$=^$(flatten_declarations($1,skip_stmt(DUMMYLOC))); }
| declaration block_item_list { $$=^$(flatten_declarations($1,$2)); }
| statement { $$=$!1; }
| statement block_item_list { $$=^$(seq_stmt($1,$2,LOC(@1,@2))); }
| function_definition2 { $$=^$(flatten_decl(new_decl(new Fn_d($1),LOC(@1,@1)),
					    skip_stmt(DUMMYLOC))); }
| function_definition2 block_item_list
   { $$=^$(flatten_decl(new_decl(new Fn_d($1),LOC(@1,@1)), $2)); }

/* This has the standard shift-reduce conflict which is properly resolved. */
selection_statement:
  IF '(' expression ')' statement
    { $$=^$(ifthenelse_stmt($3,$5,skip_stmt(DUMMYLOC),LOC(@1,@5))); }
| IF '(' expression ')' statement ELSE statement
    { $$=^$(ifthenelse_stmt($3,$5,$7,LOC(@1,@7))); }
//| IF error
//    { $$=^$(skip_stmt(LOC(@1,@2))); }
/* Cyc: the body of the switch statement cannot be an arbitrary
 * statement; it must be a list of switch_clauses */
| SWITCH '(' expression ')' '{' switch_clauses '}'
    { $$=^$(switch_stmt($3,$6,LOC(@1,@7))); }
/* Cyc: allow an identifier without the parens */
| SWITCH qual_opt_identifier '{' switch_clauses '}'
    { let e = new_exp(new UnknownId_e($2),LOC(@2,@2)); 
      $$=^$(switch_stmt(e,$4,LOC(@1,@5))); }
/* Cyc: allow tuples without the parens */
| SWITCH '$' '(' argument_expression_list ')' '{' switch_clauses '}'
    { let e = tuple_exp($4,LOC(@2,@5));
      $$=^$(switch_stmt(e,$7,LOC(@1,@8))); 
    }
//| SWITCH error
//    { $$=^$(skip_stmt(LOC(@1,@2))); }
| TRY statement CATCH '{' switch_clauses '}'
    { $$=^$(trycatch_stmt($2,$5,LOC(@1,@6))); }
//| TRY error
//    { $$=^$(skip_stmt(LOC(@1,@2))); }
;

/* Cyc: unlike C, we only allow default or case statements within
 * switches.  Also unlike C, we support a more general form of pattern
 * matching within cases. 
 * JGM: cases with an empty statement get an implicit fallthru.
 * Should also put in an implicit "break" for the last case but that's
 * better done by the control-flow-checking.
 */
switch_clauses:
  /* empty */
    { $$=^$(NULL); }
| DEFAULT ':' block_item_list
    { $$=^$(new List(new Switch_clause(new_pat(&Wild_p_val,LOC(@1,@1)),NULL,
                                       NULL,$3,LOC(@1,@3)),
		     NULL));}
| CASE exp_pattern ':' switch_clauses
    { $$=^$(new List(new Switch_clause($2,NULL,NULL,
                                       fallthru_stmt(NULL,LOC(@3,@3)),
                                       LOC(@1,@4)),$4)); }
| CASE exp_pattern ':' block_item_list switch_clauses
    { $$=^$(new List(new Switch_clause($2,NULL,NULL,$4,LOC(@1,@4)),$5)); }
| CASE pattern AND_OP expression ':' switch_clauses
    { $$=^$(new List(new Switch_clause($2,NULL,$4,
                                       fallthru_stmt(NULL,LOC(@5,@5)),
                                       LOC(@1,@6)),$6)); }
| CASE pattern AND_OP expression ':' block_item_list switch_clauses
    { $$=^$(new List(new Switch_clause($2,NULL,$4,$6,LOC(@1,@7)),$7)); }
//| CASE error ':' block_item_list
//   { $$=^$(NULL); }
;

iteration_statement:
  WHILE '(' expression ')' statement
    { $$=^$(while_stmt($3,$5,LOC(@1,@5))); }
//| WHILE error
//    { $$=^$(skip_stmt(LOC(@1,@2))); }
| DO statement WHILE '(' expression ')' ';'
    { $$=^$(do_stmt($2,$5,LOC(@1,@7))); }
//| DO error
//    { $$=^$(skip_stmt(LOC(@1,@2))); }
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
//| FOR error
//    { $$=^$(skip_stmt(LOC(@1,@2))); }
;

jump_statement:
  GOTO IDENTIFIER ';'   { $$=^$(goto_stmt(new $2,LOC(@1,@2))); }
| CONTINUE ';'          { $$=^$(continue_stmt(LOC(@1,@1)));}
| BREAK ';'             { $$=^$(break_stmt(LOC(@1,@1)));}
| RETURN ';'            { $$=^$(return_stmt(NULL,LOC(@1,@1)));}
| RETURN expression ';' { $$=^$(return_stmt($2,LOC(@1,@2)));}
/* Cyc:  explicit fallthru for switches */
| FALLTHRU ';'          { $$=^$(fallthru_stmt(NULL,LOC(@1,@1)));}
| FALLTHRU '(' ')' ';'  { $$=^$(fallthru_stmt(NULL,LOC(@1,@1)));}
| FALLTHRU '(' argument_expression_list ')' ';'
                        { $$=^$(fallthru_stmt($3,LOC(@1,@1)));}
;

/***************************** PATTERNS *****************************/
/* Cyc:  patterns */

/* The next few productions replicate expressions except that the
 * terminal case is for patterns. */
exp_pattern:
  conditional_pattern { $$=$!1; }
;
conditional_pattern:
  logical_or_pattern { $$=$!1; }
| logical_or_pattern '?' expression ':' conditional_expression
{ $$=^$(exp_pat(conditional_exp(pat2exp($1),$3,$5,LOC(@1,@5)))); }
;
logical_or_pattern:
  logical_and_pattern {$$=$!1; }
| logical_or_pattern OR_OP logical_and_expression
{ $$=^$(exp_pat(or_exp(pat2exp($1),$3,LOC(@1,@3)))); }
;
logical_and_pattern:
  inclusive_or_pattern { $$=$!1; }
| inclusive_or_pattern AND_OP inclusive_or_expression
{ $$=^$(exp_pat(and_exp(pat2exp($1),$3,LOC(@1,@3)))); }
;
inclusive_or_pattern:
  exclusive_or_pattern { $$=$!1; }
| exclusive_or_pattern '|' exclusive_or_expression
{ $$=^$(exp_pat(prim2_exp(Bitor,pat2exp($1),$3,LOC(@1,@3)))); }
;
exclusive_or_pattern:
  and_pattern { $$=$!1; }
| and_pattern '^' exclusive_or_expression
{ $$=^$(exp_pat(prim2_exp(Bitxor,pat2exp($1),$3,LOC(@1,@3)))); }
;
and_pattern:
  equality_pattern { $$=$!1; }
| and_pattern '&' equality_expression
{ $$=^$(exp_pat(prim2_exp(Bitand,pat2exp($1),$3,LOC(@1,@3)))); }
;
equality_pattern:
  relational_pattern { $$=$!1; }
| equality_pattern EQ_OP relational_expression
{ $$=^$(exp_pat(eq_exp(pat2exp($1),$3,LOC(@1,@3)))); }
| equality_pattern NE_OP relational_expression
{ $$=^$(exp_pat(neq_exp(pat2exp($1),$3,LOC(@1,@3)))); }
;
relational_pattern:
  shift_pattern { $$=$!1; }
| relational_pattern '<' shift_expression
{ $$=^$(exp_pat(lt_exp(pat2exp($1),$3,LOC(@1,@3)))); }
| relational_pattern '>' shift_expression
{ $$=^$(exp_pat(gt_exp(pat2exp($1),$3,LOC(@1,@3)))); }
| relational_pattern LE_OP shift_expression
{ $$=^$(exp_pat(lte_exp(pat2exp($1),$3,LOC(@1,@3)))); }
| relational_pattern GE_OP shift_expression
{ $$=^$(exp_pat(gte_exp(pat2exp($1),$3,LOC(@1,@3)))); }
;
shift_pattern:
  additive_pattern { $$=$!1; }
| shift_pattern LEFT_OP additive_expression
{$$=^$(exp_pat(prim2_exp(Bitlshift,pat2exp($1),$3,LOC(@1,@3)))); }
| shift_pattern RIGHT_OP additive_expression
{$$=^$(exp_pat(prim2_exp(Bitlrshift,pat2exp($1),$3,LOC(@1,@3))));}
;
additive_pattern:
  multiplicative_pattern { $$=$!1; }
| additive_pattern '+' multiplicative_expression
{$$=^$(exp_pat(prim2_exp(Plus,pat2exp($1),$3,LOC(@1,@3))));}
| additive_pattern '-' multiplicative_expression
{$$=^$(exp_pat(prim2_exp(Minus,pat2exp($1),$3,LOC(@1,@3))));}
;
multiplicative_pattern:
  cast_pattern { $$=$!1; }
| multiplicative_pattern '*' cast_expression
{$$=^$(exp_pat(prim2_exp(Times,pat2exp($1),$3,LOC(@1,@3))));}
| multiplicative_pattern '/' cast_expression
{$$=^$(exp_pat(prim2_exp(Div,pat2exp($1),$3,LOC(@1,@3))));}
| multiplicative_pattern '%' cast_expression
{$$=^$(exp_pat(prim2_exp(Mod,pat2exp($1),$3,LOC(@1,@3))));}
;
cast_pattern:
  unary_pattern { $$=$!1; }
| '(' type_name ')' cast_expression
{ $$=^$(exp_pat(cast_exp((*$2)[2],$4,true,Unknown_coercion,LOC(@1,@4)))); }
;

unary_pattern:
  postfix_pattern { $$=$!1; }
// disallow INC_OP and DEC_OP -- not constant expressions
// disallow & and * because we need them to mark patterns
| '+' cast_expression { $$=^$(exp_pat($2)); }
| unary_operator cast_expression 
  { $$=^$(exp_pat(prim1_exp($1,$2,LOC(@1,@2)))); } 
| SIZEOF '(' type_name ')' 
  { $$=^$(exp_pat(sizeoftyp_exp((*$3)[2],LOC(@1,@4)))); }
| SIZEOF unary_expression
  { $$=^$(exp_pat(sizeofexp_exp($2,LOC(@1,@2)))); }
| OFFSETOF '(' type_name ',' field_name ')'
   { $$=^$(exp_pat(offsetof_exp((*$3)[2],
                                new StructField(new $5),LOC(@1,@6)))); }
/* not checking sign here...*/
| OFFSETOF '(' type_name ',' INTEGER_CONSTANT ')' 
   { $$=^$(exp_pat(offsetof_exp((*$3)[2],
                                new TupleIndex($5[1]), LOC(@1,@6)))); }
// disallow malloc, rmalloc, and cmalloc -- not constant expressions
;

postfix_pattern:
  primary_pattern { $$=$!1; }
// disallow subscript -- not constant expression
// disallow function call because we need them to mark patterns
// disallow x.f, x->f, x++, x-- -- not constant expressions
// disallow struct initializers -- not valid in a case anyway
// disallow FILL and CODEGEN
;

primary_pattern:
  pattern { $$=$!1;}
;

pattern:
  '_'
    { $$=^$(new_pat(&Wild_p_val,LOC(@1,@1)));}
| '(' expression ')'
    { $$=^$(exp_pat($2)); }
| constant
  { exp_t e = $1;
    switch (e->r) {
    case &Const_e({.Char_c = $(s,i)}): 
      $$=^$(new_pat(new Char_p(i),e->loc)); break;
    case &Const_e({.Short_c = $(s,i)}):
      $$=^$(new_pat(new Int_p(s,i),e->loc)); break;
    case &Const_e({.Int_c = $(s,i)}):
      $$=^$(new_pat(new Int_p(s,i),e->loc)); break;
    case &Const_e({.Float_c = s}):
      $$=^$(new_pat(new Float_p(s),e->loc)); break;
    case &Const_e({.Null_c = _}):
      $$=^$(new_pat(&Null_p_val,e->loc)); break;
    case &Const_e({.String_c = _}): 
      err("strings cannot occur within patterns",LOC(@1,@1)); break;
    case &Const_e({.LongLong_c = _}): 
      unimp("long long's in patterns",LOC(@1,@1)); break;
    default: 
      err("bad constant in case",LOC(@1,@1));
    }
  }
| qual_opt_identifier
    { $$=^$(new_pat(new UnknownId_p($1),LOC(@1,@1))); }
| IDENTIFIER IDENTIFIER pattern
    { if (strcmp($2,"as") != 0) 
        err("expecting `as'",LOC(@2,@2));
      $$=^$(new_pat(new Var_p(new_vardecl(new $(Loc_n, new $1),&VoidType_val,NULL),
                              $3),LOC(@1,@1))); 
    }
| '$' '(' tuple_pattern_list ')'
    { let $(ps, dots) = *($3);
      $$=^$(new_pat(new Tuple_p(ps,dots),LOC(@1,@4)));
    }
| qual_opt_identifier '(' tuple_pattern_list ')'
    { let $(ps, dots) = *($3);
      $$=^$(new_pat(new UnknownCall_p($1,ps,dots),LOC(@1,@4)));
    }
| qual_opt_identifier '{' type_params_opt field_pattern_list '}'
   {  let $(fps, dots) = *($4); 
      let exist_ts = List::map_c(typ2tvar,LOC(@3,@3),$3);
      $$=^$(new_pat(new Aggr_p(new AggrInfo(UnknownAggr(StructA,$1,NULL),NULL),
			       exist_ts,fps,dots),LOC(@1,@5)));
   }
| '{' type_params_opt field_pattern_list '}'
   {  let $(fps, dots) = *($3); 
      let exist_ts = List::map_c(typ2tvar,LOC(@2,@2),$2);
      $$=^$(new_pat(new Aggr_p(NULL,exist_ts,fps,dots),LOC(@1,@4)));
   }
| '&' pattern
    { $$=^$(new_pat(new Pointer_p($2),LOC(@1,@2))); }
| AND_OP pattern /* to allow &&p */
    { $$=^$(new_pat(new Pointer_p(new_pat(new Pointer_p($2),LOC(@1,@2))),LOC(@1,@2))); }
| '*' IDENTIFIER
    { $$=^$(new_pat(new Reference_p(new_vardecl(new $(Loc_n, new $2),
						&VoidType_val,NULL),
                                    new_pat(&Wild_p_val,LOC(@2,@2))),
		    LOC(@1,@2))); }
| '*' IDENTIFIER IDENTIFIER pattern
    { if (strcmp($3,"as") != 0)
        err("expecting `as'",LOC(@3,@3));
      $$=^$(new_pat(new Reference_p(new_vardecl(new $(Loc_n, new $2),
						&VoidType_val,NULL),
                                    $4),LOC(@1,@2))); 
    }
| IDENTIFIER '<' TYPE_VAR '>' 
   { let tag = id2type($3,Tcutil::kind_to_bound(&Tcutil::ik));
     $$=^$(new_pat(new TagInt_p(typ2tvar(LOC(@3,@3),tag),
				new_vardecl(new $(Loc_n,new $1),
					    new TagType(tag),NULL)),
		   LOC(@1,@4))); }
| IDENTIFIER '<' '_' '>' 
   { let tv = Tcutil::new_tvar(Tcutil::kind_to_bound(&Tcutil::ik));
     $$=^$(new_pat(new TagInt_p(tv,
				new_vardecl(new $(Loc_n,new $1),
					    new TagType(new VarType(tv)),NULL)),
		   LOC(@1,@4))); }
;

tuple_pattern_list:
  tuple_pattern_list0 { $$=^$(new $(List::rev($1), false)); }
| tuple_pattern_list0 ',' ELLIPSIS { $$=^$(new $(List::rev($1), true)); }
| ELLIPSIS { $$=^$(new $(NULL, true)); }
;

tuple_pattern_list0:
  pattern
    {$$=^$(new List($1,NULL));}
| tuple_pattern_list0 ',' pattern
    {$$=^$(new List($3,$1));}
;

field_pattern:
  pattern
    {$$=^$(new $(NULL,$1));}
| designation pattern
    {$$=^$(new $($1,$2));}

field_pattern_list:
  field_pattern_list0 { $$=^$(new $(List::rev($1), false)); }
| field_pattern_list0 ',' ELLIPSIS { $$=^$(new $(List::rev($1), true)); }
| ELLIPSIS { $$=^$(new $(NULL, true)); }
;

field_pattern_list0:
  field_pattern
    { $$=^$(new List($1,NULL));}
| field_pattern_list0 ',' field_pattern
    {$$=^$(new List($3,$1)); }
;

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
| unary_expression SWAP assignment_expression
    { $$=^$(swap_exp($1,$3,LOC(@1,@3))); }
;

assignment_operator:
  '='          { $$=^$(NULL); }
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
/* Cyc: expressions to build heap-allocated objects and arrays */
| NEW array_initializer
    { $$=^$(New_exp(NULL,$2,LOC(@1,@2))); }
| NEW logical_or_expression
    { $$=^$(New_exp(NULL,$2,LOC(@1,@2))); }
| RNEW '(' expression ')' array_initializer
    { $$=^$(New_exp($3,$5,LOC(@1,@5))); }
| RNEW '(' expression ')' logical_or_expression
    { $$=^$(New_exp($3,$5,LOC(@1,@5))); }
;

constant_expression:
  conditional_expression { $$=$!1; }
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
    { $$=^$(cast_exp((*$2)[2],$4,true,Unknown_coercion,LOC(@1,@4))); }
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
| OFFSETOF '(' type_name ',' field_name ')' 
   { $$=^$(offsetof_exp((*$3)[2],new StructField(new $5),LOC(@1,@6))); }
/* not checking sign here...*/
| OFFSETOF '(' type_name ',' INTEGER_CONSTANT ')' 
   { $$=^$(offsetof_exp((*$3)[2],new TupleIndex($5[1]), LOC(@1,@6))); }
/* Cyc: malloc, rmalloc, numelts, swap, etc. */
| MALLOC '(' expression ')'
   { $$=^$(new_exp(new Malloc_e(MallocInfo{false,NULL,NULL,$3,false}),
                   LOC(@1,@4))); }
| RMALLOC '(' assignment_expression ',' expression ')'
   { $$=^$(new_exp(new Malloc_e(MallocInfo{false,$3,NULL,$5,false}),
                   LOC(@1,@6))); }
| CALLOC '(' assignment_expression ',' SIZEOF '(' type_name ')' ')'
   { let $(_,_,t) = *($7);
     $$=^$(new_exp(new Malloc_e(MallocInfo{true,NULL,new(t),$3,false}),
                   LOC(@1,@9))); }
| RCALLOC '(' assignment_expression ',' assignment_expression ',' 
              SIZEOF '(' type_name ')' ')'
   { let $(_,_,t) = *($9);
     $$=^$(new_exp(new Malloc_e(MallocInfo{true,$3,new(t),$5,false}),
                   LOC(@1,@11))); }
| NUMELTS '(' expression ')'
   { $$=^$(primop_exp(Numelts, list($3), LOC(@1,@4))); }
| TAGCHECK '(' postfix_expression '.' field_name ')'
   { $$=^$(new_exp(new Tagcheck_e($3,new $5),LOC(@1,@6))); }
| TAGCHECK '(' postfix_expression PTR_OP field_name ')'
   { $$=^$(new_exp(new Tagcheck_e(deref_exp($3,LOC(@3,@3)),new $5),LOC(@1,@6))); }
| VALUEOF '(' type_name ')'
   { let $(_,_,t) = *$3;
     $$=^$(valueof_exp(t, LOC(@1,@4))); }
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
    { $$=^$(unknowncall_exp($1,NULL,LOC(@1,@3))); }
| postfix_expression '(' argument_expression_list ')'
    { $$=^$(unknowncall_exp($1,$3,LOC(@1,@4))); }
| postfix_expression '.' field_name
    { $$=^$(aggrmember_exp($1,new $3,LOC(@1,@3))); }
| postfix_expression PTR_OP field_name
    { $$=^$(aggrarrow_exp($1,new $3,LOC(@1,@3))); }
| postfix_expression INC_OP
    { $$=^$(post_inc_exp($1,LOC(@1,@2))); }
| postfix_expression DEC_OP
    { $$=^$(post_dec_exp($1,LOC(@1,@2))); }
| '(' type_name ')' '{' initializer_list '}'
    { $$=^$(new_exp(new CompoundLit_e($2,List::imp_rev($5)),LOC(@1,@6))); }
| '(' type_name ')' '{' initializer_list ',' '}'
    { $$=^$(new_exp(new CompoundLit_e($2,List::imp_rev($5)),LOC(@1,@7))); }
;

primary_expression:
  qual_opt_identifier
    /* Could be an identifier, a struct tag, or an datatype constructor */
    { $$=^$(new_exp(new UnknownId_e($1),LOC(@1,@1))); }
| constant
    { $$= $!1; }
| STRING
    { $$=^$(string_exp($1,LOC(@1,@1))); }
| '(' expression ')'
    { $$= $!2; }
//| '(' error ')'
//    { $$=^$(null_exp(LOC(@2,@2))); }
/* Cyc: stop instantiation */
| primary_expression LEFT_RIGHT
    { $$=^$(noinstantiate_exp($1, LOC(@1,@2)));}
| primary_expression '@' '<' type_name_list right_angle
    { $$=^$(instantiate_exp($1, List::imp_rev($4),LOC(@1,@5))); }
/* Cyc: tuple expressions */
| '$' '(' argument_expression_list ')'
    { $$=^$(tuple_exp($3,LOC(@1,@4))); }
/* Cyc: structure expressions */
| qual_opt_identifier '{' type_params_opt initializer_list '}'
    { $$=^$(new_exp(new Aggregate_e($1,$3,List::imp_rev($4),NULL),LOC(@1,@5))); }
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
    { $$=^$(new List($1,NULL)); }
| argument_expression_list0 ',' assignment_expression
    { $$=^$(new List($3,$1)); }
;

/* NB: We've had to move enumeration constants into primary_expression
   because the lexer can't tell them from ordinary identifiers */
constant:
  INTEGER_CONSTANT   { $$=^$(int_exp($1[0],$1[1],LOC(@1,@1))); }
| CHARACTER_CONSTANT { $$=^$(char_exp($1,              LOC(@1,@1))); }
| FLOATING_CONSTANT  { $$=^$(float_exp($1,             LOC(@1,@1))); }
/* Cyc: NULL */
| NULL_kw            { $$=^$(null_exp(LOC(@1,@1)));}
;

qual_opt_identifier:
  IDENTIFIER      { $$=^$(new $(Rel_n(NULL), new $1)); }
| QUAL_IDENTIFIER { $$=$!1; }
;
qual_opt_typedef:
  TYPEDEF_NAME      { $$=^$(new $(Rel_n(NULL), new $1)); }
| QUAL_TYPEDEF_NAME { $$=$!1; }
;
// Hacks to allow typedef nanmes to overlap with struct/union names and
// field names and kinds.  Still cannot be "inject"
struct_union_name:
  qual_opt_identifier { $$=$!1; }
| qual_opt_typedef    { $$=$!1; }
;
field_name:
  IDENTIFIER   { $$=$!1; }
| TYPEDEF_NAME { $$=$!1; }
;
// Hack for parsing >> as two > when dealing with nested type-parameter lists
right_angle:
  '>' {}
| RIGHT_OP { yylex_buf->lex_curr_pos -= 1; }
%%

void yyprint(int i, union YYSTYPE v) {
  switch (v) {
  case {.Int_tok = $(_,i2)}: fprintf(stderr,"%d",i2);      break;
  case {.Char_tok = c}:       fprintf(stderr,"%c",c);       break;
  case {.String_tok = s}:          fprintf(stderr,"\"%s\"",s);  break;
  case {.QualId_tok = &$(p,v2)}:
    let prefix = NULL;
    switch (p) {
    case {.Rel_n = x}: prefix = x; break;
    case {.Abs_n = x}: prefix = x; break;
    case {.Loc_n = _}: break;
    }
    for (; prefix != NULL; prefix = prefix->tl)
      fprintf(stderr,"%s::",*(prefix->hd));
    fprintf(stderr,"%s::",*v2);
    break;
  default: fprintf(stderr,"?"); break;
  }
}

namespace Parse{
list_t<decl_t> parse_file(FILE @`H f) {
  parse_result = NULL;
  yyparse(Lexing::from_file(f));
  return parse_result;
}
}
