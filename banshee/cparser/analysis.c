/* This file is part of banshee-pta.
   Copyright (C) 2000-2001 The Regents of the University of California.

banshee-pta is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

banshee-pta is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with banshee-pta; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include <stdarg.h>

#include "parser.h"
#include "c-parse.h"

#include "unparse.h"
#include "semantics.h"
#include "constants.h"
#include "AST_utils.h"
#include "analysis.h"
//#include "debug.h"
#include "pta.h" 
#include "utils.h"
#include "expr.h"
#include "env.h"
#include "hash.h"

#define INIT_HASH_SIZE 128
#define MAX_STR 1024

#define BOTTOM_INT (expr_type){pta_bottom(),unsigned_int_type}

#define PUSH_ENV ({++acnt.scope; local_env = new_env(env_rgn,local_env);}) 
#define POP_ENV ({local_env = env_parent(local_env);})

DECLARE_LIST(hash_table_list, hash_table);
DEFINE_LIST(hash_table_list, hash_table);

DECLARE_LIST(int_list, int);
DEFINE_NONPTR_LIST(int_list, int);

extern region banshee_nonptr_region;

region var_info_region;

struct counts {
  int scope;
  int collection_count;
  int string_count;
  int next_alloc;
};

struct persistence_state {
  hash_table_list global_var_envs;
  hash_table_list collection_envs;
  int_list scopes;
  int_list collection_counts;
  int_list string_counts;
  int_list next_allocs;
  int_list banshee_times;
};

static struct persistence_state state;

static struct counts acnt = {0,0,0,0};

static char *alloc_name = "alloc_";

static function_decl current_fun_decl = NULL;

// For alpha renaming
/* static int scope = 0; */

/* TODO -- must serialize this!!! */
/* static env global_var_env; */
static hash_table global_var_hash;


static env file_env;
static env local_env;
static env struct_env;
static hash_table collection_hash;

static char *alloc_names[] = {"malloc","calloc","realloc","valloc",
                              "xmalloc","__builtin_alloca","alloca",
			      "kmalloc","__rc_typed_ralloc",
			      "rc_typed_rarrayalloc",
			      "__rc_ralloc_small0",
			      "__rc_rstralloc",
			      "rc_rstralloc0",
			      "rstralloc",
			      "typed_rarrayalloc",
			      "typed_ralloc",
			      NULL};

int flag_field_based = 0;

typedef enum var_kind // Scope information
{
  vk_local,   // local scope
  vk_static,  // file scope
  vk_global,  // global scope
} var_kind;

typedef struct expr_type
{
  T t_type;
  type c_type;
} expr_type;

typedef struct var_info
{
  T t_type;
  type c_type;
  var_kind kind;
  char *name;
  int visible;
  int scope;
} *var_info;

static region analysis_rgn;
static region env_rgn;
extern region parse_region;

extern int flag_print_empty;
extern int flag_print_vars;
extern int flag_model_strings;

void banshee_backtrack(int);
int banshee_get_time();		 /* banshee.c */
bool is_void_parms(declaration); /* semantics.c */
static expr_type analyze_expression(expression) deletes;
static expr_type analyze_binary_expression(binary) deletes;
static expr_type analyze_unary_expression(unary) deletes;
static void analyze_declaration(declaration) deletes;
static void analyze_statement(statement) deletes;
static type return_type(type t);
static var_kind get_kind(data_declaration ddecl);

// Return true if a name has been defined globally
/* static bool seen_global(const char *name) */
/* { */
/*   if (( env_lookup(global_var_env,name,FALSE)) ) */
/*     return TRUE; */
/*   return FALSE; */
/* } */

static bool seen_global(const char *name)
{
  return hash_table_lookup(global_var_hash,(hash_key)name, NULL);
}


static bool var_info_insert(var_info v_info)
{
  char buf[MAX_STR];

  /* This extra renaming step is here because of a slight difference
      in hash table semantics-- in dhash (which env uses), a scan will
      see hidden bindings, but not so w/ Jeff's hash implementation
  */
  snprintf(buf, MAX_STR, "%s::%d", v_info->name, acnt.collection_count++);

  if(v_info->visible)
    hash_table_insert(collection_hash,(hash_key)rstrdup(banshee_nonptr_region, buf),
		      (hash_data)v_info->t_type);
  switch (v_info->kind)
    {
    case vk_local :
      {
	env_add(local_env, v_info->name, v_info);
	return TRUE;
      }
      break;
    case vk_static :
      {
	env_add(file_env, v_info->name, v_info);
	return TRUE;
      }
      break;
    case vk_global :
      {
	assert(!seen_global(v_info->name));
	//env_add(global_var_env, v_info->name, v_info);
	hash_table_insert(global_var_hash, (hash_key)v_info->name, 
			  (hash_data)v_info);
	return FALSE;
      }
      break;
    }
  assert(0);
  return FALSE;
}

static data_declaration make_ret_decl(char *name,type t )
{
  data_declaration ddecl = ralloc(parse_region,struct data_declaration);
  ddecl->type = t;
  ddecl->name = name;
  ddecl->kind = decl_variable;
  ddecl->isexternalscope = FALSE;
  ddecl->islimbo = FALSE;
  ddecl->islocal = TRUE;
  ddecl->initialiser = NULL;
  ddecl->isparameter = FALSE;
  ddecl->vtype = variable_normal;
  return ddecl;
}


static bool var_info_lookup(const char *name,var_info *v)
{
  if ( ( (*v) = env_lookup(local_env,name, FALSE)) )
    return TRUE;
  if ( ( (*v)  = env_lookup(file_env,name, FALSE)) )
    return TRUE;
/*   else if (( (*v) = env_lookup(global_var_env,name,FALSE)) ) */
  else if (hash_table_lookup(global_var_hash, (hash_key)name, (hash_data *)v))
    return TRUE;
  return FALSE;
}

static void collect_state();

static void var_info_init(void)
{
  env_rgn = newregion();
  local_env = new_env(env_rgn,NULL);
  file_env = new_env(env_rgn,NULL);
  struct_env = new_env(env_rgn,NULL);
}

static void var_info_clear(void) deletes
{
  deleteregion(env_rgn);
}

static void collect_state(void)
{ 
#ifdef BANSHEE_ROLLBACK
  hash_table_list_append_tail(hash_table_copy(NULL, collection_hash), state.collection_envs);  
  hash_table_list_append_tail(hash_table_copy(NULL, global_var_hash), state.global_var_envs);
  int_list_append_tail(acnt.scope, state.scopes);
  int_list_append_tail(acnt.collection_count, state.collection_counts);
  int_list_append_tail(acnt.string_count, state.string_counts); 
  int_list_append_tail(acnt.next_alloc, state.next_allocs);
  int_list_append_tail(banshee_get_time(), state.banshee_times);
#endif /* BANSHEE_ROLLBACK */
}

void analyze(declaration program) deletes
{
  declaration d;
  
  unparse_start(stdout);
  AST_set_parents(CAST(node, program));

  var_info_init();
  acnt.scope++;

  scan_declaration(d, program) 
    analyze_declaration(d);

  collect_state();
  var_info_clear();
}

static bool is_alloc_fun(const char *name)
{
  int i;
  for (i = 0; alloc_names[i]; i++)
    {
      if (! strcmp(name,alloc_names[i]))
	return TRUE;
    }
  return FALSE;
}

static var_info new_var(const char *name,type c_type,
			var_kind kind, bool is_visible)
{
  char str[MAX_STR];

  var_info v_info = ralloc(var_info_region, struct var_info);
  
  v_info->name = rstrdup(banshee_nonptr_region,name);
  v_info->c_type = c_type;
  v_info->kind = kind;
  v_info->visible = (int)is_visible;

  if ( (kind == vk_global) )
    v_info->scope = 0;
  else v_info->scope = acnt.scope;

  snprintf(str,MAX_STR,"%s@%d",name,v_info->scope);
  
  if (flag_print_vars)
    printf("%s \n",str);

  v_info->t_type = pta_make_ref(str);

  var_info_insert(v_info);


  return v_info;
} 

// For each type, map "name" to a T kind
static T get_field_ref(const char *name, type c_type,
		       type field_type)
{
  T result;
  env field_env = env_lookup(struct_env,type_tag(c_type)->name,FALSE);

  // Struct hasn't been seen yet, make an env for its fields and add it
  if (field_env == NULL) {
    field_env = new_env(analysis_rgn,NULL);
    env_add(struct_env,type_tag(c_type)->name,field_env);
  }
  assert(field_env);
  assert(env_lookup(struct_env,type_tag(c_type)->name,FALSE));

  // Now check to see if that particular field has a type yet
  result = env_lookup(field_env,name,FALSE);
  
  if (result == NULL) {
    var_info info;
    char distname[MAX_STR];
    snprintf(distname,MAX_STR,"%s->%s",type_tag(c_type)->name,name);
    // T result = pta_make_ref(distname);
    info = new_var(distname,field_type,vk_global,TRUE);
    env_add(field_env,name,info->t_type);
    result = info->t_type;
  }

    return result;
}

static T get_var_ref(const char *name, type c_type, 
		     var_kind kind, bool is_visible)
{
  var_info v_info;
  // if ( hash_table_lookup(id_map,(hash_key)name,(hash_data*)&v_info) )
  if (var_info_lookup(name,&v_info))
    return v_info->t_type;
  else 
    return new_var(name,c_type,kind,is_visible)->t_type;
}




static expression make_identifier_expression(location loc,cstring id,bool imp,
					     data_declaration decl)
{
  identifier result = new_identifier(analysis_rgn,loc,id,NULL);
  
  result->type = decl->type;
  result->lvalue = decl->kind == decl_variable ||
    decl->kind == decl_magic_string;
  result->cst = fold_identifier(CAST(expression, result), decl);
  result->isregister = decl->kind == decl_variable &&
    decl->vtype == variable_register;
  result->static_address = 
    foldaddress_identifier(CAST(expression, result), decl);
  result->ddecl = decl;
  result->ddecl->isused = TRUE;

  return CAST(expression, result);
}

static void make_temporary(const char *name)
{
  var_info v_info = NULL;
  // hash_table_lookup(id_map,(hash_key)name,(hash_data*)&v_info);
  var_info_lookup(name,&v_info);

  assert(v_info);
  v_info->visible = FALSE;
}

/*
  Generate constraints for functions
*/

static T_list compute_param_terms(declaration arg_list)
{
  
  declaration arg;
  T_list t_args = new_T_list(analysis_rgn);
  
  if ( is_void_parms(arg_list) )
    return t_args;

  //  PUSH_ENV;
  scan_declaration(arg,arg_list)
    {
      data_declaration arg_decl;
      if (is_ellipsis_decl(arg))
	break;
      
      if (arg->kind == kind_data_decl)
	{
	  variable_decl vdecl = 
	    CAST(variable_decl,CAST(data_decl,arg)->decls);
	  arg_decl = vdecl->ddecl;
	}
      
      else
	{
	  oldidentifier_decl odecl = 
	    CAST(oldidentifier_decl,arg);
	  arg_decl = odecl->ddecl;
	}
      
      assert(arg_decl);

      T_list_cons(new_var(arg_decl->name,arg_decl->type,vk_local,TRUE)->t_type,t_args);
	       //   get_var_ref(arg_decl->name,arg_decl->type,vk_local,TRUE) );
      
    }
  //POP_ENV;
  return t_args;
}


// CHECK 
static void fun_type(function_decl fdecl,char *ret_name)
{
  // need to walk to root??
  data_declaration ddecl = fdecl->ddecl; 
  char str[MAX_STR];
  T fun_ref,ret,f_type;
  T_list param_terms;

  fun_ref = get_var_ref(ddecl->name,ddecl->type,get_kind(ddecl),FALSE);

  param_terms = compute_param_terms(fdecl->fdeclarator->parms); 

  snprintf(str,MAX_STR,"%s@%d",ddecl->name,acnt.scope);

  ret = get_var_ref(ret_name,return_type(ddecl->type),vk_local,FALSE); 
  f_type = pta_make_fun(str,ret,param_terms);

  make_temporary(ddecl->name);
  make_temporary(ret_name);
  
  pta_assignment(fun_ref,f_type);
}

bool is_fun_ret(const char *name)
{
  return ((*name) == '@');
}

bool is_global_name(const char *name)
{
  int i;
  char *suffixes[] = {"@0","@0[]","@0$",NULL};
  
  for (i = 0; suffixes[i]; i++)
    {
      char* found = strstr(name,suffixes[i]);
      if (found)
	return TRUE;
    }
  return FALSE;
}

/*
  Generate constraints for expressions
*/
static char *get_fun_ret(const char *name)
{
  char str[MAX_STR];
  char *ret_name;
  snprintf(str,MAX_STR,"@%s_return",name);
  ret_name = rstrdup(banshee_nonptr_region,str);
  return ret_name;
}

bool is_global_scope()
{
  return (env_parent(local_env) == NULL);
}

static var_kind get_kind(data_declaration ddecl)
{
  if (ddecl->isexternalscope)
    return vk_global;
  else if (ddecl->islocal 
	   || (current_fun_decl && ddecl->vtype == variable_static) )
    return vk_local;
  // CHECK -- is this set for function scope static vars???
  else if ( ddecl->isfilescoperef 
	    || ddecl->vtype == variable_static )
    return vk_static;
  else return vk_global;
}


// make a term type for an allocation function
static T get_next_alloc_fun() deletes
{
  char str[MAX_STR];
  char *v_name,*ret_name;
  T alloc_var, alloc_ret, alloc_fun;

  region scratch_rgn = newregion();

  // snprintf(str,MAX_STR,"%s%d",alloc_name,next_alloc++);
  snprintf(str,MAX_STR,"%s%d",alloc_name,acnt.next_alloc++);
  v_name = rstrdup(scratch_rgn,str);
  snprintf(str,MAX_STR,"%s%s",v_name,"_ret");
  ret_name = rstrdup(scratch_rgn,str);

  // CHECK allocs local or global?
  alloc_var = get_var_ref(v_name,char_type,vk_local,TRUE);
  alloc_ret = get_var_ref(ret_name,ptr_void_type,vk_local,FALSE);
  
  alloc_fun = pta_make_fun(v_name,alloc_ret,new_T_list(scratch_rgn));

  deleteregion(scratch_rgn);

  pta_assignment(alloc_ret,pta_rvalue(pta_address(alloc_var)));
  return pta_address(alloc_fun);
}

// Given a binary operator, return the type of the resulting expression
// if the arg types are t1 and t2
static type binary_oper_type(ast_kind kind,type t1,type t2)
{
  switch (kind)
    {
    case kind_plus:
    case kind_minus:
      {
	bool b1 = type_pointer(t1) || type_array(t1);
	bool b2 = type_pointer(t2) || type_array(t2);
	
	if (b1 && b2) return int_type;
	if (b1 && !b2) return t1;
	if (!b1 && b2) return t2;
	if (!b1 && !b2) return t1;
      }
    case kind_lt:
    case kind_gt:
    case kind_geq:
    case kind_leq:
    case kind_ne:
    case kind_eq:
    case kind_andand:
    case kind_oror:
    case kind_times:
    case kind_divide:
    case kind_modulo:
      {
	return int_type;
      }
      break;
    case kind_bitand:
    case kind_bitor:
    case kind_bitxor:
    case kind_rshift:
    case kind_lshift:
      {
	return t1;
      }
      break;
    default:
      assert(0);
      return ptr_void_type;
      break;
    }

  assert(0);
  return ptr_void_type;
}

// Given a unary operator, return the type of the resulting expression
// if the arg type is t1
static type unary_oper_type(ast_kind kind,type t)
{
  switch (kind)
    {
    case kind_dereference:
      {
	if (type_pointer(t))
	  return type_points_to(t);
	else if (type_array(t))
	  return type_array_of(t);
      }
      break;
    case kind_address_of:
      {
	return make_pointer_type(t);
      }
      break;
    case kind_preincrement:
    case kind_postincrement:
    case kind_predecrement:
    case kind_postdecrement:
    case kind_bitnot:
    case kind_unary_plus:
      {
	return t;
      }
    case kind_not:
    case kind_unary_minus:
      {
	return int_type;
      }
      break;
    case kind_label_address:
      {
	return ptr_void_type;
      }
      break;
    default:
      assert(0);
      break;
    }

  assert(0);
  return ptr_void_type;
}

bool will_generate_assignment(expression lhs, type lhs_type, 
				     expression e)
{
  if (type_array(lhs_type) && type_char(type_array_of(lhs_type)) 
      && e)
    return TRUE;
  else return FALSE;
}

// Generate an assignment for a variable declaration with a non-null 
// initializer. The initializer could potentially be an initializer list. 
static void generate_assignment(expression lhs,type lhs_type, expression init)
deletes
{
  init->lvalue = TRUE; // hack for default_conversion problem
  init->cst = NULL;

  assert(lhs_type);

  if (is_init_list(init))
    {
      init_list list = CAST(init_list,init);

      if (type_array(lhs_type)) // nested array inits: x[]...[] = {{},...,{}}
	{
	  expression e,array_ref;
	  expression c_bot = CAST(expression,make_unknown_cst(int_type));
       
	
	  array_ref = make_array_ref(lhs->location,lhs,c_bot);
	  
	  scan_expression(e,list->args)
	    {
	      generate_assignment(array_ref,
				  unary_oper_type(kind_dereference,lhs_type),
				  e);
	    }
	  return;
	}
      
      // hopefully, the fields are in order
      if (type_struct(lhs_type) || type_union(lhs_type) ) 
	{
	  expression e;
	  field_declaration field = type_tag(lhs_type)->fieldlist;
	  
	  scan_expression(e,list->args)
	    {
	      expression assign_field;
	      if (!field) 
		break;

	      assign_field = 
		make_field_ref(lhs->location,lhs,
			       str2cstring(analysis_rgn,field->name));

	      generate_assignment(assign_field,field->type,e);

	      field = field->next;
	    }
	  return;
	}

      else // should not have an initializer list with any other type
	assert(0);
    }
  else if (type_array(lhs_type) && type_char(type_array_of(lhs_type)))
    return;
  else
    {
      /* TODO-- this is a HACK! */
      if (!init->type)
	return;
      else {
	expression assign = 
	  make_assign(init->location,kind_assign,lhs,init);
	analyze_expression (assign);
	return;
      }
    }
  
}


static expr_type promote_to_ptr(expr_type e)
{
  if (type_array(e.c_type))
    {
      return (expr_type)
	{pta_address(e.t_type),make_pointer_type(type_array_of(e.c_type))};
    }
  else return e;
}

static type return_type(type t)
{
  if (type_function(t))
    return type_function_return_type(t);
  else if (type_pointer(t))
    return type_function_return_type(type_points_to(t));

  return t;
}

// Hate to do this, but necessary since make_identifier not working
// combined cases for identifier_expression = expression
expr_type analyze_init(data_declaration ddecl,expression init) deletes
{
  T t;
  expr_type e1,e2;
  if (is_alloc_fun(ddecl->name))
    t = get_next_alloc_fun();
  else 
    {
      var_kind kind;
      bool visible;
      /*
	if (type_function(ddecl->type))
	kind = vk_fun; 
	else if (is_global_name(ddecl->name))
	kind = vk_global;
	else 
	kind = vk_local;
      */
      kind = get_kind(ddecl);

      visible = ! type_function(ddecl->type);
      t = get_var_ref(ddecl->name,ddecl->type,kind,visible);
    }
  e1 = (expr_type){t,ddecl->type};
  
  e2 = promote_to_ptr(analyze_expression(init));
  
  pta_assignment(e1.t_type,pta_rvalue(e2.t_type));
  return e2;
}


static expr_type analyze_expression(expression e) deletes
{
  if (!e) return BOTTOM_INT;
  
  switch(e->kind)
    {
    case kind_identifier:
      {
	identifier id = CAST(identifier,e);
	T t;
	if (is_alloc_fun(id->ddecl->name))
	  t = get_next_alloc_fun();
	else 
	  {
	    var_kind kind;
	    bool visible;
	    /*
	      if (type_function(id->ddecl->type))
	      kind = vk_fun; 
	      else if (is_global_name(id->ddecl->name))
	      kind = vk_global;
	      else 
	      kind = vk_local;
	    */
	    kind = get_kind(id->ddecl);
	    
	    visible = ! type_function(id->ddecl->type);
	    t = get_var_ref(id->ddecl->name,id->ddecl->type,kind,visible);
	  }
	return (expr_type){t,id->ddecl->type};
      }
      break;

      // TODO GCC : ({statement})
    case kind_compound_expr:
      {
	
	compound_expr ce = CAST(compound_expr, e);
	compound_stmt cs = CAST(compound_stmt, ce->stmt);
	declaration d;
	statement cur_stmt;
	expr_type result;
	
	
	PUSH_ENV;

	scan_declaration(d, cs->decls) 
	  { 
	    assert(d->kind != kind_asm_decl); /*asm_decl only at toplevel */
	    analyze_declaration(d);
	  } 

	cur_stmt = cs->stmts;
            
	//analyze all but last statement
	while (cur_stmt && cur_stmt->next )
	  {
	    analyze_statement(cur_stmt);
	    cur_stmt = CAST(statement,cur_stmt->next);
	  }
	
	if (cur_stmt && is_expression_stmt(cur_stmt))
	  {
	    if (CAST(expression_stmt,cur_stmt)->arg1)
	      result =
		analyze_expression(CAST(expression_stmt,cur_stmt)->arg1);
	  } 
	else
	  result = BOTTOM_INT; 
	
	POP_ENV;
	
	return result;
      }
      break;
    case kind_sizeof_type:
    case kind_alignof_type:
    case kind_alignof_expr:
    case kind_sizeof_expr:
      /*
    case kind_signed_cst:
    case kind_unsigned_cst:
    case kind_unknown_cst:
      */
    case kind_known_cst:
    case kind_lexical_cst:
      {
	return BOTTOM_INT;
      }
    case kind_string:
      {
	string se = CAST(string, e);
	//constant s;
	string_cst s;
	T t_type;
	char str[255];
	//kind k;
	//truncate long strings (might be a bad idea)
	
	if ( is_string_cst(se->strings) )
	  s = CAST(string_cst,se->strings);
	else /* CHECK */
	  {
	    return analyze_expression(se->strings);
	  }
	//k = get_kind(se->ddecl);

	//if (k == vk_global)

	// Model strings as points-to, each string constant 
        // is a distinct source
	snprintf(str,sizeof(str),"%s_%d",
		 CAST(lexical_cst,s)->cstring.data,acnt.string_count++);
	
	if(flag_model_strings)
	  t_type = pta_address(get_var_ref(str,char_type,vk_global,TRUE));
	else
	  t_type = pta_bottom();
	return (expr_type){t_type,make_pointer_type(char_type)};
      }
      break;
    case kind_conditional:
      {
	conditional c = CAST(conditional, e);
	expr_type cond, arg1, arg2;
	type c_type;
	
	cond = promote_to_ptr(analyze_expression(c->condition));
	arg1 = promote_to_ptr(analyze_expression(c->arg1));
	arg2 = promote_to_ptr(analyze_expression(c->arg2));
	
	if (type_pointer(arg1.c_type) && type_integer(arg2.c_type))
	  c_type = arg1.c_type;
	else if (type_pointer(arg2.c_type) && type_integer(arg1.c_type))
	  c_type = arg2.c_type;
	else
	  c_type = arg1.c_type;

	return (expr_type){pta_join(arg1.t_type,arg2.t_type),c_type};
      }
      break;
    case kind_function_call:
      {
	function_call fc = CAST(function_call,e);
	expression arg;
	region scratch_rgn = newregion();
	T_list actuals = new_T_list(scratch_rgn);
	
	expr_type result,f_expr = promote_to_ptr(analyze_expression(fc->arg1));

	scan_expression(arg,fc->args) 
	  {
	    expr_type exp = 
	      promote_to_ptr(analyze_expression(arg));
	    
	    T_list_cons(pta_rvalue(exp.t_type),actuals);

	    // actuals = T_list_append(actuals,T_list_cons(NULL,pta_rvalue(exp.t_type)));
	  }

	T_list_reverse(actuals);
	
	
	result =  (expr_type)
	{
	  pta_application(pta_rvalue(f_expr.t_type),actuals),
	  return_type(f_expr.c_type)
	};

	deleteregion(scratch_rgn);
	return result;

      }
      break;
    case kind_array_ref:
      {
	array_ref ar = CAST(binary,e);
	
	expr_type a_type = promote_to_ptr(analyze_expression(ar->arg1));
	expr_type i_type = promote_to_ptr(analyze_expression(ar->arg2));
	
	T t_type = pta_join(a_type.t_type,i_type.t_type);
	type c_type;

	c_type = binary_oper_type(kind_plus,a_type.c_type,i_type.c_type);

	return 
	  (expr_type)
	  {pta_deref(t_type),unary_oper_type(kind_dereference,c_type)};
      }
      break;
    case kind_comma:
      {      
	expr_type result;
	comma c = CAST(comma, e);
	expression e2;
	scan_expression(e2, c->arg1)
	  {
	    result = promote_to_ptr(analyze_expression(e2));
	  }
	return result;
      }
      break;

      // CHECK -- make sure the type of the ast node is the correct type
    case kind_cast:
      {
	cast c = CAST(cast,e);

	expr_type result = promote_to_ptr(analyze_expression(c->arg1));

	return (expr_type){result.t_type,e->type};
      }
      break;
    case kind_cast_list:
      {
	return BOTTOM_INT;
	//assert(0);
      }
      break;
      
    case kind_field_ref: // indirects are converted to derefs by the parser
      {
	field_ref fr = CAST(field_ref,e);
	
	if (flag_field_based && type_struct(fr->arg1->type)) {
	  // I can't remember if analyze_expression can have side effects,
	  // so do it anyway
	  analyze_expression(fr->arg1); 
	  return (expr_type) {get_field_ref(fr->cstring.data,fr->arg1->type,
					    fr->type), 
				fr->type};
	}
	else {
	  expr_type result = promote_to_ptr(analyze_expression(fr->arg1));
	  return (expr_type) {result.t_type,fr->type}; // return the field type
	}
      }
      break;

    default:
      if (is_unary(e)) 
	return analyze_unary_expression(CAST(unary,e));
      else if (is_binary(e)) 
	return analyze_binary_expression(CAST(binary,e));
      else 
	{
	  unparse_start(stderr);
	  fprintf(stderr,
		  "Error : unmatched expression in analysis\n");
	  prt_expression(e,0);
	  unparse_end();
	}
      break;
    }

  assert(0);
  return BOTTOM_INT;
}

static bool pointer_destroying_kind(ast_kind k)
{
  return 
    ( k == kind_andand || k == kind_oror || k == kind_not 
      || k == kind_lt || k == kind_leq || k == kind_gt 
      || k == kind_geq || k ==  kind_eq || k ==  kind_ne
      || k ==  kind_times || k ==  kind_divide 
      || k ==  kind_lshift || k ==  kind_rshift 
      || k ==  kind_bitand || k ==  kind_bitor
      );
      
}


// Given an assignment operator kind, return the corresponding operator
static ast_kind get_bin_op(ast_kind k)
{
  switch (k)
    {
    case kind_lshift_assign:
      return kind_lshift;
    case kind_rshift_assign:
      return kind_rshift;
    case kind_bitand_assign:
      return kind_bitand;
    case kind_bitor_assign:
      return kind_bitor;
    case kind_bitxor_assign:
      return kind_bitxor;
    case kind_modulo_assign:
      return kind_modulo;
    case kind_times_assign:
      return kind_times;
    case kind_plus_assign: 
      return kind_plus;
    case kind_divide_assign:
      return kind_divide;
    case kind_minus_assign: 
      return kind_minus;
    default:
      assert(0);
      return k;
    }
  assert(0);
  return k;
}

static expr_type analyze_binary_expression(binary e) deletes
{
   assert(e);

   if ( pointer_destroying_kind(e->kind) )
    {
      type t1 = e->arg1->type; // CHECK -- arg1->type or ddecl->type
      type t2 = e->arg2->type;

      analyze_expression(e->arg1);
      analyze_expression(e->arg2);

      return (expr_type){pta_bottom(),binary_oper_type(e->kind,t1,t2)};
    }

  switch(e->kind)
    {

    case kind_bitxor:
    case kind_modulo:
    case kind_plus: 
    case kind_minus:
      {
	T t;
	expr_type e1,e2;
	type c_type;
	
	e1 = promote_to_ptr(analyze_expression(e->arg1));
	e2 = promote_to_ptr(analyze_expression(e->arg2));
	c_type= binary_oper_type(e->kind,e1.c_type,e2.c_type);
	
	t = pta_join(e1.t_type,e2.t_type);
	
	return (expr_type){t,c_type};
      }
      break;
      
    case kind_assign:
      {	
	expr_type e1,e2;
	
	e1 = analyze_expression(e->arg1);
	e2 = promote_to_ptr(analyze_expression(e->arg2));
	
	pta_assignment(e1.t_type,pta_rvalue(e2.t_type));
	return e2;
      }
      break;

      // assignment with operator
    case kind_lshift_assign:
    case kind_rshift_assign:
    case kind_bitand_assign:
    case kind_bitor_assign:
    case kind_bitxor_assign:
    case kind_modulo_assign:
    case kind_times_assign:
    case kind_divide_assign:
    case kind_plus_assign: 
    case kind_minus_assign: 
      {
	expr_type e1,e2;
	
	expression binary_exp = 
	  make_binary(e->location,get_bin_op(e->kind),e->arg1,e->arg2);
	
	e1 = analyze_expression(e->arg1);
	e2 = promote_to_ptr(analyze_expression(binary_exp));
	
        pta_assignment(e1.t_type,pta_rvalue(e2.t_type));
	
	return e2;
      }
      break;
    default:
      assert(0);
      break;
    }
  assert(0);
  return BOTTOM_INT;
}

static expr_type analyze_unary_expression(unary e) deletes
{
  assert(e);
  
  if ( pointer_destroying_kind(e->kind) )
    {
      analyze_expression(e->arg1);
      return (expr_type){pta_bottom(),unary_oper_type(e->kind,e->arg1->type)};
    }
  
  switch(e->kind)
    {
    case kind_conjugate: 
    case kind_realpart: 
    case kind_imagpart: 

    case kind_unary_minus:
    case kind_unary_plus:
    case kind_bitnot: 
      {
	expr_type e1;
	type c_type;
	e1 = promote_to_ptr(analyze_expression(e->arg1));
	c_type= unary_oper_type(e->kind,e1.c_type);

	return (expr_type){e1.t_type,c_type};	
      }
      break;
    case kind_dereference:
      {
	expr_type result = promote_to_ptr(analyze_expression(e->arg1));
	
	if (type_function(type_points_to(result.c_type)))
	  return result;
	else 
	  return (expr_type)
	  {pta_deref(result.t_type),unary_oper_type(e->kind,result.c_type)};
      }
      break;
    case kind_address_of:
      {
	expr_type result = analyze_expression(e->arg1);
	
	if (type_function(result.c_type))
	    return result;
	else
	  return (expr_type)
	  {pta_address(result.t_type),unary_oper_type(e->kind,result.c_type)};
      }
      break;
    case kind_postincrement:
    case kind_preincrement:
      {
	expression assign,plus,unknown;
	
	unknown = CAST(expression,make_unknown_cst(int_type));

	plus = make_binary(e->location, kind_plus, e->arg1, unknown);
	assign = make_assign(e->location, kind_assign, e->arg1 ,plus);

	return analyze_expression(assign);
      }
      break;
    case kind_predecrement:
    case kind_postdecrement:
      {
	expression assign,minus,unknown;
	
	unknown = CAST(expression,make_unknown_cst(int_type));

	minus = make_binary(e->location, kind_minus, e->arg1, unknown);
	assign = make_assign(e->location, kind_assign, e->arg1 ,minus);

	
	return analyze_expression(assign);
      }
      break;
    case kind_extension_expr:
      {
	return analyze_expression(e->arg1);
      }
      break;

    default:
      {
	assert(0);
	return BOTTOM_INT;
      }
      break;
    }
  assert(0);
  return BOTTOM_INT;
}



static void analyze_declaration(declaration d) deletes
{
  assert(d);
  
  switch(d->kind)
    {
    case kind_asm_decl:
      break;
    case kind_extension_decl: 
      {
	extension_decl ed = CAST(extension_decl,d);
	analyze_declaration(ed->decl);
      }
      break;
    case kind_variable_decl:
      {
	variable_decl vd = CAST(variable_decl,d);
	
	// CHECK -- handle type tagged
	// CHECK -- short circuit extern decls
	// CHECK -- look at declared type??
	/*
	if (is_function_decl(vd->ddecl))
	  return;
	if (type_tagged(vd->ddecl->type))
	  {
	    bool foo = is_typename(vd->ddecl);
	    return;
	  }
	*/
	if (vd->ddecl->kind == decl_typedef ||
	    vd->ddecl->kind == decl_function ||
	    vd->ddecl->kind == decl_error )
	  return;
	
	//if (is_field_decl(vd->ddecl))
	//  return;
	else
	  {
	    bool visible;
	    //var_info *v_info;
	    
	    cstring name = str2cstring(analysis_rgn,vd->ddecl->name);
	    
	    expression var_expr =
	      make_identifier_expression(vd->location,name,FALSE,vd->ddecl);
	    
	    visible = ! type_function(vd->ddecl->type);
	    
	
	    // Make a new variable, unless the decl is global and has 
	    // already been seen. Necessary to model scoping
	    if (! ((get_kind(vd->ddecl) == vk_global) 
		   && ( seen_global(name.data)))  )
	      new_var(name.data,vd->ddecl->type,get_kind(vd->ddecl),visible);
	    
	    
	    if (vd->arg1)
	      generate_assignment(var_expr,vd->ddecl->type,vd->arg1);
	    else 
	      analyze_expression(var_expr);
	  }
      }
      break;
    case kind_data_decl: // CHECK nested data declarations (??)
      {
	data_decl dd = CAST(data_decl,d);
	declaration decl;
	scan_declaration(decl,dd->decls)
	  {
	    analyze_declaration(decl);
	  }
      }
      break;
    case kind_function_decl:
      {
	//declaration parm;
	// statement body;
	function_decl fd = CAST(function_decl,d);
	current_fun_decl = fd;
	
	//if (is_empty_stmt(fd->stmt))
	//  printf("No statement");
	
	PUSH_ENV;

	fun_type(fd,get_fun_ret(fd->ddecl->name));
       
	//scan_declaration(parm,fd->fdeclarator->parms)
	// {
	//   analyze_declaration(parm);
	//  }
	
	analyze_statement(fd->stmt);
	POP_ENV;
	
	current_fun_decl = NULL;

      }
      break;
    case kind_oldidentifier_decl:
      {
	oldidentifier_decl od = CAST(oldidentifier_decl,d);
	
	//cstring name = str2cstring(analysis_rgn,od->ddecl->name);
	
	expression var_expr =
	  make_identifier_expression(od->location,od->cstring,FALSE,od->ddecl);
	
	analyze_expression(var_expr);
      }
      break;
    case kind_ellipsis_decl: 
      break;
    default :
      assert(0); // unmatched declaration (enumerator, implicit, field ??)
      break;
    }
}

// Generate constraints for expressions within the statement

static void analyze_statement(statement s)
{
  assert(s);

  switch(s->kind)
    {
    case kind_return_stmt:
      {
	return_stmt rs = CAST(return_stmt,s);
	expression e = rs->arg1;

	// we must be in the context of a function
	if (! current_fun_decl) return;
	assert(current_fun_decl); 
	
	if (e)
	  {
	    cstring name = 
	      str2cstring(analysis_rgn,
			  get_fun_ret(current_fun_decl->ddecl->name));

	    // CHECK
	    data_declaration ret_dd = 
	      make_ret_decl(name.data
			    ,return_type(current_fun_decl->ddecl->type));
	    expression lhs = 
	      make_identifier_expression(e->location,name,FALSE,ret_dd);

	    e->lvalue = TRUE; // hack for default_conversion problem
	    e->cst = NULL;
  
	    analyze_expression(make_assign(e->location,kind_assign,lhs,e));
	  }
      }
      break;
    case kind_expression_stmt:
      {
	expression_stmt es = CAST(expression_stmt,s);
	analyze_expression(es->arg1);
      }
      break;
    case kind_if_stmt:
      {
	if_stmt is = CAST(if_stmt, s);

	analyze_expression(is->condition);
	if (is->stmt1)
	  analyze_statement(is->stmt1);
	if (is->stmt2)
	  analyze_statement(is->stmt2);
      }
      break;
    case kind_while_stmt:
      {
	while_stmt ws = CAST(while_stmt,s);
	analyze_expression(ws->condition);
	analyze_statement(ws->stmt);
      }
      break;
    case kind_dowhile_stmt:
      {
	dowhile_stmt dws = CAST(dowhile_stmt,s);
	analyze_expression(dws->condition);
	analyze_statement(dws->stmt);
      }
      break;
    case kind_computed_goto_stmt:
      {
	computed_goto_stmt cgs = CAST(computed_goto_stmt,s);
	analyze_expression(cgs->arg1);
      }
      break;
    case kind_for_stmt:
      {
	for_stmt fs = CAST(for_stmt,s);
	if (fs->arg1) analyze_expression(fs->arg1);
	if (fs->arg2) analyze_expression(fs->arg2);
	if (fs->arg3) analyze_expression(fs->arg3);
	if (fs->stmt) analyze_statement(fs->stmt);
      }
      break;
    case kind_switch_stmt: // cases must be constant (?)
      {
	switch_stmt ss = CAST(switch_stmt,s);
	analyze_expression(ss->condition);
	analyze_statement(ss->stmt);
      }
      break;
    case kind_labeled_stmt:
      {
	labeled_stmt ls = CAST(labeled_stmt,s);
	analyze_statement(ls->stmt);
      }
      break;
    case kind_compound_stmt:
      {
	compound_stmt cs = CAST(compound_stmt,s);
	declaration decl;
	statement stmt;
	
	PUSH_ENV;
	
	scan_declaration(decl,cs->decls)
	  {
	    analyze_declaration(decl);
	  }
	scan_statement(stmt,cs->stmts)
	  {
	    analyze_statement(stmt);
	  }
	
	POP_ENV;

      }
      break;
      // Compatibility w/ points-to-- potentially make a new global var
    case kind_goto_stmt:
      {
	goto_stmt gs = CAST(goto_stmt,s);
	
	const char *name = gs->id_label->cstring.data;
	
	// CHECK : type??
	if ( (! seen_global(name)) )
	  new_var(name,int_type,vk_global,TRUE);
      }
      break;
    case kind_empty_stmt:
     case kind_break_stmt:
    case kind_continue_stmt:
      break;
    case kind_asm_stmt:
      break;
    default:
      fprintf(stderr,"Warning, unhandled stmt kind: %d\n", s->kind );
      break;
    }
}

// Measurements

/* T get_contents(var_info v_info) */
/* { */
/*   struct ref_decon t_decon = ref_decon(v_info->t_type); */

/*   return t_decon.f1; */
/* } */

void serialize_cs(FILE *f, hash_table *entry_points, unsigned long sz);

void analysis_backtrack(int backtrack_time)
{
  int_list_scanner scan;
  int next, length = 0;
  
  int_list_scan(state.banshee_times, &scan);

  /* Search the analysis state for the corresponding time */
  while(int_list_next(&scan,&next)) {
  /* Get the list length */
    if (backtrack_time == next) break;
    length++;
  }

  /* Truncate each of the lists (TODO: truncate also the global_var_hash!) */
  state.collection_envs = 
    hash_table_list_copy_upto(NULL, state.collection_envs, length);
  state.global_var_envs = 
    hash_table_list_copy_upto(NULL, state.global_var_envs, length);
  state.scopes = 
    int_list_copy_upto(NULL, state.scopes, length);
  state.collection_counts  = 
    int_list_copy_upto(NULL, state.collection_counts, length);
  state.string_counts  = 
    int_list_copy_upto(NULL, state.string_counts, length);
  state.next_allocs = 
    int_list_copy_upto(NULL, state.next_allocs, length);
  state.banshee_times =
    int_list_copy_upto(NULL, state.banshee_times, length);

  /* Set the collection environment and acnt struct */
  collection_hash = hash_table_list_last(state.collection_envs);
  global_var_hash = hash_table_list_last(state.global_var_envs);
  acnt.scope = int_list_last(state.scopes);
  acnt.collection_count = int_list_last(state.collection_counts);
  acnt.string_count = int_list_last(state.string_counts);
  acnt.next_alloc = int_list_last(state.next_allocs);

  banshee_backtrack(backtrack_time);
}

static void write_int_list(FILE *f, int_list l)
{
  int_list_scanner scan;
  int next, length;

  assert(f);

  length = int_list_length(l);


  fwrite((void *)&length, sizeof(int), 1, f);

  int_list_scan(l, &scan);
  while(int_list_next(&scan,&next)) {
    fwrite((void *)&next, sizeof(int), 1, f);
  }
  
}

static int_list read_int_list(FILE *f)
{
  int_list result;
  int length,i,next;
  assert(f);

  result = new_int_list(permanent);

  fread((void *)&length, sizeof(int), 1, f);

  for (i = 0; i < length; i++) {
    fread((void *)&next, sizeof(int), 1, f);
    int_list_append_tail(next, result);
  }
  return result;
}

void analysis_serialize(const char *filename)
{
  hash_table *entries;
  int length;
  FILE *f = fopen(filename, "wb");
  
  assert(f);
  
  fwrite((void *)&acnt, sizeof(struct counts), 1, f);
  write_int_list(f,state.scopes);
  write_int_list(f,state.collection_counts);
  write_int_list(f,state.string_counts);
  write_int_list(f,state.next_allocs);
  write_int_list(f,state.banshee_times);
  length = int_list_length(state.scopes);
  
  entries = hash_table_list_array_from_list(permanent, state.collection_envs);
  pta_serialize(f, entries, length);
}

void analysis_region_serialize(const char *filename)
{
  FILE *f = fopen(filename, "wb");
  assert(f);
 
  fwrite((void *)&state, sizeof(struct persistence_state), 1, f);
  fwrite((void *)&acnt, sizeof(struct counts), 1, f);
  fwrite((void *)&global_var_hash, sizeof(hash_table), 1, f);

  {
    int count = 0;
    char *name;
    T ttype;
    hash_table_scanner scan;
    
    hash_table_scan(collection_hash, &scan);
    
    while(hash_table_next(&scan, (hash_key)&name,(hash_data)&ttype)) {
      count++;
    }
  }

  pta_region_serialize(f);
}

void analysis_region_deserialize(translation t, const char *filename)
{
  FILE *f = fopen(filename, "rb");
  assert(f);

  fread((void *)&state, sizeof(struct persistence_state), 1, f);
  fread((void *)&acnt, sizeof(struct counts), 1, f);
  fread((void *)&global_var_hash, sizeof(hash_table), 1, f);
  //update_pointer(t, (void **)&collection_hash);
  update_pointer(t, (void **)&state.scopes);
  update_pointer(t, (void **)&state.collection_counts);
  update_pointer(t, (void **)&state.string_counts);
  update_pointer(t, (void **)&state.next_allocs);
  update_pointer(t, (void **)&state.banshee_times);
  update_pointer(t, (void **)&state.collection_envs);
  update_pointer(t, (void **)&state.global_var_envs);
  //assert(hash_table_list_last(state.collection_envs) == collection_hash);
  collection_hash = hash_table_list_last(state.collection_envs);
  update_pointer(t, (void **)&global_var_hash);

  pta_region_deserialize(t, f);
}

void analysis_deserialize(const char *filename)
{
  int length,i;
  hash_table *result;
  hash_table_list collection_envs;
  FILE *f = fopen(filename, "rb");
  assert(f);

  
  fread((void *)&acnt, sizeof(struct counts), 1, f);
  state.scopes = read_int_list(f);
  state.collection_counts = read_int_list(f);
  state.string_counts = read_int_list(f);
  state.next_allocs = read_int_list(f);
  state.banshee_times = read_int_list(f);

  result = pta_deserialize(f);
  collection_envs = new_persistent_hash_table_list();

  length = int_list_length(state.scopes);

  collection_hash = result[length-1];

  for (i = 0; i < length; i++) {
    hash_table_list_append_tail(result[i], collection_envs);
  }
  state.collection_envs = collection_envs;
  /* This really shouldn't work... */
  //assert(hash_table_list_last(state.collection_envs) == collection_hash);
}


void print_analysis_results() deletes
{
  contents_type_list ptset_list;
  region temp_region;
  struct list *visibles;
  contents_type ptset, ttype;
  char *name;
  contents_type_list_scanner scan;
  hash_table_scanner hs;

  int things_pointed_to = 0,
    non_empty_sets = 0,
    num_vars = 0;
  temp_region = newregion();
  ptset_list = new_contents_type_list(temp_region);
  visibles = new_list(temp_region,0);
 
  hash_table_scan(collection_hash, &hs);
  while(hash_table_next(&hs, (hash_key*)&name, (hash_data *)&ttype)) {
    assert(ttype);
    assert(name);
    contents_type_list_cons(pta_get_contents(ttype),ptset_list);
    num_vars++;
  }

  contents_type_list_scan(ptset_list,&scan);

  while (contents_type_list_next(&scan,&ptset))
    {
      int size = pta_get_ptsize(ptset);
      non_empty_sets += size ? 1 : 0;
      things_pointed_to += size; 
    }
  deleteregion(temp_region);
  printf("\nNumber of things pointed to: %d",things_pointed_to);
  printf("\nNumber of non-empty sets: %d",non_empty_sets);
  printf("\nAverage size: %f",(float)things_pointed_to / non_empty_sets);
  printf("\nNumber of program variables: %d\n",num_vars);
}

void print_points_to_sets()
{
  hash_table_scanner hs;
  char *name;
  contents_type ptset;
  T ttype;
  
  printf("\n========Points-to sets========\n");
  
  hash_table_scan(collection_hash, &hs);
  
  while(hash_table_next(&hs, (hash_key)&name,(hash_data)&ttype)) {
    ptset = pta_get_contents(ttype);
    if (pta_get_ptsize(ptset) || flag_print_empty)
      {
	printf("%s --> ",name);
	pta_pr_ptset(ptset);
	puts("");
      }
  }

}

void register_persistent_region(region r, Updater u);

int update_var_info(translation t, void *m)
{
  var_info v = (var_info)m;
  update_pointer(t, (void **)&v->t_type);
  update_pointer(t, (void **)&v->c_type);
  update_pointer(t, (void **)&v->name);

  return sizeof(struct var_info);
}

void analysis_init() deletes
{
  pta_init();
  // DEBUG: 
  // pta_reset();
  analysis_rgn = newregion();
/*   global_var_env = new_env(analysis_rgn,NULL); */

  /* Not really a gen_e hash, but the exact kind doesn't matter */
  global_var_hash = 
    make_persistent_string_hash_table(128, BANSHEE_PERSIST_KIND_gen_e);
  collection_hash = 
    make_persistent_string_hash_table(128, 
				      BANSHEE_PERSIST_KIND_gen_e);

  var_info_region = newregion();
  register_persistent_region(var_info_region,update_var_info);

  state.collection_envs = new_persistent_hash_table_list();
  state.global_var_envs = new_persistent_hash_table_list();
  state.scopes = new_persistent_int_list();
  state.collection_counts = new_persistent_int_list();
  state.string_counts = new_persistent_int_list();
  state.next_allocs = new_persistent_int_list();
  state.banshee_times = new_persistent_int_list();
}


void analysis_reset() deletes
{
  pta_reset();
  
  deleteregion(analysis_rgn);
  analysis_rgn = newregion();
}

void analysis_stats(FILE *f)
{
  //andersen_terms_stats(f);
}

void analysis_print_graph(void)
{
/*   FILE *f = fopen("andersen.dot","w"); */
/*   andersen_terms_print_graph(f); */
/*   fclose(f); */
}

