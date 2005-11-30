/*
 * Copyright (c) 2000-2004
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/******************************************************************************
   Interface for Banshee receiver class analysis. Some notes:

    - rca_init *MUST* be called before any other function in this interface
    - rca_reset will flush all internal state and re-initialize
    - DO NOT call any query function untill ALL constraints have been added
      Doing so will implicitly invalidate any solution returned by the query 
      function
******************************************************************************/
#include <assert.h>
#include "regions.h"
#include "rca_analyze.h"

/*****************************************************************************
 *                                                                           *
 *   Intitializing/Resetting Banshee Engine                                  *
 *                                                                           *
 *****************************************************************************/


void rca_init(void)
{
  rca_types_init();
}

void rca_reset(void)
{
  rca_types_reset();
}

/*****************************************************************************
 *                                                                           *
 *   Traversing/constructing types                                           *
 *                                                                           *
 *****************************************************************************/

/* The no-information case: the empty set */
rca rca_bottom(void) 
{
  return rca_zero();
}

/* (MAY ADD CONSTRAINTS) t1 \cup t2 */
rca rca_join(rca t1,rca t2)
{
  rca result;
  region scratch_rgn = newregion();
  rca_list list = new_rca_list(scratch_rgn);
 
  rca_list_cons(t1,list);
  rca_list_cons(t2,list);

  result = rca_union(list);
  deleteregion(scratch_rgn);

  return result;
}

/* (MAY ADD CONSTRAINTS) Strip a ref constructor off */
rca rca_deref(rca t1)
{
  return ref_proj2(t1);
}

/* (MAY ADD CONSTRAINTS) The lvalue -> rvalue conversion, same as deref */
rca rca_rvalue(rca t1) 
{
  return rca_deref(t1);
}

/* Add a ref constructor */
rca rca_address(rca t1)
{
  return ref(rlabel_t_one(),rca_one(),t1);
}

/* Make a new lvalue */
rca rca_make_lval(const char *name)
{
  rca var = rca_fresh(name);
  rlabel_t absloc = rlabel_t_constant(name);

  return ref(absloc,var,var);
}

/* Build the argument field for a lam from the list of arguments*/
static rcaArgs make_lam_args(rca_list args)
{
  region scratch = newregion();
  int counter = 0;
  
  rcaArgs result;
  
  rca_list_scanner scan;
  rca temp;
  char field_name[512];

  rcaArgs_map map = new_rcaArgs_map(scratch);
  rca_list_scan(args,&scan);

  while(rca_list_next(&scan,&temp))
    {
      snprintf(field_name,512,"%d",counter++);
      rcaArgs_map_cons(rcaArgs_make_field(field_name,temp),map);
    }

  result = rcaArgs_row(map,rcaArgs_wild());

  deleteregion(scratch);
  
  return result;
}


static rcaArgs make_mbr_args(rca thisptr, rca_list args)
{
  region scratch = newregion();
  int counter = 0;
  
  rcaArgs result;
  
  rca_list_scanner scan;
  rca temp;
  char field_name[512];

  rcaArgs_map map = new_rcaArgs_map(scratch);
  rca_list_scan(args,&scan);

  while(rca_list_next(&scan,&temp))
    {
      snprintf(field_name,512,"%d",counter++);
      rcaArgs_map_cons(rcaArgs_make_field(field_name,temp),map);
    }

  rcaArgs_map_cons(rcaArgs_make_field("this",thisptr),map);

  result = rcaArgs_row(map,rcaArgs_wild());

  deleteregion(scratch);
  
  return result;
}

/* Make a new function args --exn--> ret */
rca rca_make_lam(const char *name, rca ret, rca_list args, rca exn)
{
  return lam (rlabel_t_constant(name), make_lam_args(args), ret, exn);
}

rca rca_make_mbr(const char *name, rca thisptr,
		 rca ret, rca_list args, rca exn)
{
  return mbr (rlabel_t_constant(name), make_mbr_args(thisptr,args), ret, exn);
}


/*****************************************************************************
 *                                                                           *
 *  Processing assignments/function calls                                    *
 *                                                                           *
 *****************************************************************************/

int flag_print_constraints = 0;

void rca_do_inclusion(rca t1, rca t2)
{
  if (flag_print_constraints)
    {
      rca_print(stdout,t1);
      fprintf(stdout," <= ");
      rca_print(stdout,t2);
      puts("");
    }
  rca_inclusion(t1,t2);
}


/* rhs <= lhs, where rhs is an rvalue */
void rca_assignment(rca lhs,rca rhs)
{
  rca_do_inclusion(lhs,ref_pat1(rhs));
}

/* t1 must contain a lam, adds actuals <= formals and returns the 
   return value paired with the exception return value
 */
struct rca_app_result rca_application(rca lam, rca_list actuals)
{
  rcaArgs args = make_lam_args(actuals);
  
  rca_do_inclusion(lam,lam_pat1(args));
  
  return (struct rca_app_result){rca_address(lam_proj2(lam)),rca_address(lam_proj3(lam))};
}

/* t1 must contain a obj, */
struct rca_app_result rca_dispatch(const char *fnName, rca obj, 
				   rca this_ptr, rca_list actuals)
{
  rcaArgs args;
  rca mbr = rca_fresh("vtbl_entry");
  dispatchT app_tbl; 
 
  region scratch = newregion();
  dispatchT_map map = new_dispatchT_map(scratch);
  

  dispatchT_map_cons(dispatchT_make_field(fnName,mbr),map);
  app_tbl = dispatchT_row(map,dispatchT_wild());
  rca_do_inclusion(obj,obj_pat1(app_tbl));
  args = 
    make_mbr_args(this_ptr,actuals);

  rca_do_inclusion(mbr,mbr_pat1(args));
  
  deleteregion(scratch);

  return (struct rca_app_result){rca_address(mbr_proj2(mbr)),rca_address(mbr_proj3(mbr))};

}

/*****************************************************************************
 *                                                                           *
 *   Querying the points-to graph                                            *
 *                                                                           *
 *****************************************************************************/

/* Get the contents of an lvalue. This does not add constraints. 
   If the argument is not an lvalue type, return NULL. */
rca rca_contents(rca lval)
{
  struct ref_decon rca_decon = ref_decon(lval);
  
  assert (rca_decon.f1);
  return rca_decon.f1;
}

/* Compute the points-to set from a pointer's contents */
rlabel_t_list rca_points_to(rca contents)
{
  rca_list_scanner scan;
  rca temp;
  region scratch_rgn = newregion();
  region results_rgn = newregion();
  
  rca_list ptset = rca_list_copy(scratch_rgn,rca_tlb(contents));
  rlabel_t_list abslocs = new_rlabel_t_list(results_rgn);

  rca_list_scan(ptset,&scan);
  
  while(rca_list_next(&scan,&temp))
    {
      struct ref_decon ref = ref_decon(temp);
      struct lam_decon lam = lam_decon(temp);

      if (ref.f0)
	rlabel_t_list_cons(ref.f0,abslocs);
      else if (lam.f0)
	rlabel_t_list_cons(lam.f0,abslocs);
    }
  deleteregion(scratch_rgn);
  
  return abslocs;
}

/* Alias query : given two contents, return true if they are aliases */
bool rca_alias_query(rca r1, rca r2)
{
  rlabel_t_list_scanner scan1,scan2;
  rlabel_t_list ptset1,ptset2;
  rlabel_t temp1,temp2;
  
  /* Return true if the contents are the same */
  if (rca_eq(r1,r2))
    return TRUE;
  
  ptset1 = rca_points_to(r1);
  ptset2 = rca_points_to(r2);
  
  rlabel_t_list_scan(ptset1,&scan1);

  /* Check the emptiness of the intersection */
  while(rlabel_t_list_next(&scan1,&temp1))
    {
      rlabel_t_list_scan(ptset2,&scan2);
      
      while (rlabel_t_list_next(&scan2,&temp2))
	{
	  if (rlabel_t_eq(temp1,temp2))
	    return TRUE;
	}
    }
  
  return FALSE;
}


static int set_size = 0;

static void pr_lamset_rca_elem(rca t)
{
  struct lam_decon lam = lam_decon(t);
 
  if (lam.f0) 
    {
      printf(",");
      set_size += 1;
      rlabel_t_print(stdout,lam.f0);
    }
}

static void pr_objset_rca_elem(rca t)
{
  struct obj_decon obj = obj_decon(t);
 
  if (obj.f0) 
    {
      printf(",");
      set_size += 1;
      rlabel_t_print(stdout,obj.f0);
    }
}

static void pr_ptset_rca_elem(rca t)
{
  struct ref_decon ref = ref_decon(t);
 
  if (ref.f0) 
    {
      printf(",");
      set_size += 1;
      rlabel_t_print(stdout,ref.f0);
    }
}

int rca_pr_ptset(rca contents)
{
  region scratch_rgn = newregion();
  rca_list ptset = rca_list_sort(rca_list_copy(scratch_rgn,rca_tlb(contents)),rca_cmp) ;
  bool printed = FALSE;
  set_size = 0;
  
  printf("{");

  while (! rca_list_empty(ptset) && ! printed) {
    struct ref_decon ref = ref_decon(rca_list_head(ptset));

    if (ref.f0)
      {
	set_size +=1;
	rlabel_t_print(stdout,ref.f0);
	printed = TRUE;
      }
    ptset = rca_list_tail(ptset);
  }


  rca_list_app(ptset,pr_ptset_rca_elem);
  printf("}(%d)",set_size);
  deleteregion(scratch_rgn);

  return set_size;
}

int rca_pr_objset(rca contents)
{
  region scratch_rgn = newregion();
  rca_list objset = rca_list_copy(scratch_rgn,rca_tlb(contents));
  bool printed = FALSE;
  set_size = 0;

  printf("{");

  while (!rca_list_empty(objset) && ! printed) {

    struct obj_decon obj = obj_decon(rca_list_head(objset));
    
    if (obj.f0)
      {
	set_size +=1;
	rlabel_t_print(stdout,obj.f0);
	printed = TRUE;
      }
    objset = rca_list_tail(objset);
  }
  
  rca_list_app(objset,pr_objset_rca_elem);
  printf("}(%d)",set_size);
  deleteregion(scratch_rgn);

  return set_size;
}

/* Given a contents, print the set of functions for the contents and return
   the number of elements printed
 */
int rca_pr_lamset(rca contents)
{
  region scratch_rgn = newregion();
  rca_list lamset = rca_list_copy(scratch_rgn,rca_tlb(contents));
  bool printed = FALSE;
  set_size = 0;

  printf("{");
  while (! rca_list_empty(lamset) && ! printed ){

      struct lam_decon lam = lam_decon(rca_list_head(lamset));

      if (lam.f0)
	{
	  set_size +=1;
	  rlabel_t_print(stdout,lam.f0);
	  printed = TRUE;
	}

      lamset = rca_list_tail(lamset);
    }

  rca_list_app(lamset,pr_lamset_rca_elem);
  printf("}(%d)",set_size);
  deleteregion(scratch_rgn);

  return set_size;
}


/*****************************************************************************
 *                                                                           *
 *   Extensions for RCA : modeling classes/dispatch/exceptions               *
 *                                                                           *
 *****************************************************************************/

/* Make a dispatch table containing a set of methods */
/* One dispatch table per class definition */
rca rca_make_obj(const char *name, dispatchT_map member_fns)
{
  dispatchT vtable;
  getFields get = getFields_wild();
  setFields set = setFields_wild();
  rlabel_t classtag;

  vtable = dispatchT_row(member_fns,dispatchT_wild());
  classtag = rlabel_t_constant(name); 

  return obj(classtag,vtable,set,get);
}
