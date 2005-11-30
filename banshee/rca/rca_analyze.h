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


#ifndef RCA_ANALYZE_H
#define RCA_ANALYZE_H


#include "rca_types.h"
#include "rca_flags.h"          /* flag_print_constraints */
#include "linkage.h"

EXTERN_C_BEGIN

/******************************************************************************
   Interface for Banshee receiver class analysis. Some notes:

    - rca_init *MUST* be called before any other function in this interface
    - rca_reset will flush all internal state and re-initialize
    - DO NOT call any query function untill ALL constraints have been added
      Doing so will implicitly invalidate any solution returned by the query 
      function
******************************************************************************/

struct rca_app_result {
  rca ret;
  rca exn;
};

/*****************************************************************************
 *                                                                           *
 *   Intitializing/Resetting Banshee Engine                                  *
 *                                                                           *
 *****************************************************************************/

void rca_init(void);

void rca_reset(void);

/*****************************************************************************
 *                                                                           *
 *   Traversing/constructing types                                           *
 *                                                                           *
 *****************************************************************************/

/* The no-information case: the empty set */
rca rca_bottom(void);

/* (MAY ADD CONSTRAINTS) t1 \cup t2 */
rca rca_join(rca,rca);

/* (MAY ADD CONSTRAINTS) Strip a ref constructor off */
rca rca_deref(rca);

/* (MAY ADD CONSTRAINTS) The lvalue -> rvalue conversion, same as deref */
rca rca_rvalue(rca);

/* Add a ref constructor */
rca rca_address(rca);

/* Make a new lvalue */
rca rca_make_lval(const char *name);

/* Make a new function args --exn--> ret */
rca rca_make_lam(const char *name,rca ret, rca_list args, rca exn);

/* Make a new member function args --exn--> ret */
rca rca_make_mbr(const char *name, rca thisptr,
		 rca ret, rca_list args, rca exn);

/*****************************************************************************
 *                                                                           *
 *  Processing assignments/function calls                                    *
 *                                                                           *
 *****************************************************************************/

/* rhs <= lhs, where rhs is an rvalue */
void rca_assignment(rca lhs,rca rhs);

/* t1 must contain a lam, adds actuals <= formals and returns a
   pair consisting of the return value and exception return value */
struct rca_app_result rca_application(rca lam, rca_list actuals);

/* t1 must contain a cls, dispatches on fnName and returns a
   pair consisting of the return value and exception return value */
struct rca_app_result rca_dispatch(const char *fnName, rca cls, 
				   rca this_ptr, rca_list actuals);

/*****************************************************************************
 *                                                                           *
 *   Querying the points-to graph                                            *
 *                                                                           *
 *****************************************************************************/

/* Get the contents of an lvalue. This does not add constraints. 
   If the argument is not an lvalue type, return NULL. */
rca rca_contents(rca lval);

/* Compute the points-to set from a pointer's contents */
rlabel_t_list rca_points_to(rca);

/* Alias query : given two contents, return true if they are aliases */
bool rca_alias_query(rca, rca);

/* Given a contents, print a ptset, and return the number of 
 elements printed */
int rca_pr_ptset(rca contents);

/* Given a contents, print the set of classes for the contents and return
   the number of elements printed
*/
int rca_pr_objset(rca contents);

/* Given a contents, print the set of functions for the contents and return
   the number of elements printed
 */
int rca_pr_lamset(rca contents);

/*****************************************************************************
 *                                                                           *
 *   Extensions for RCA : modeling classes/dispatch/exceptions               *
 *                                                                           *
 *****************************************************************************/

/* Make a new class from a set of member functions */
/* One per class definition (not instance) */
rca rca_make_obj(const char *name, dispatchT_map member_fns);

/* Debugging hook for rca_inclusion */
void rca_do_inclusion(rca t1, rca t2);

EXTERN_C_END

#endif /* RCA_ANALYZE_H */

