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
#include "linkage.h"
#include "regions.h"

EXTERN_C_BEGIN

#define NUM_REGIONS 39

/*****************************************************************************
 *                                                                           *
 *   Initialization                                                          *
 *                                                                           *
 *****************************************************************************/

void banshee_region_persistence_init();

/*****************************************************************************
 *                                                                           *
 *   Interface                                                               *
 *                                                                           *
 *****************************************************************************/
/* Return an array of persistent regions, and write the information to
   filename */
region *get_persistent_regions(const char *filename);

/* Get a list of updater functions, reading the information from filename */
Updater *get_updater_functions(const char *filename);

/* Register r as a persistent region, using updater function u */
void register_persistent_region(region r, Updater u);

/* Unregister r as a persistent region */
void unregister_persistent_region(region r);

/*****************************************************************************
 *                                                                           *
 *   Region declarations                                                     *
 *                                                                           *
 *****************************************************************************/

/* A region containing non pointers. The update function won't touch
   any of these values */
extern region banshee_nonptr_region; 

/* A region containing pointers. The update function will call
   update_pointer on adjacent word values */
extern region banshee_ptr_region;

/* Regions for hash.c */
extern region bucket_region;	
extern region table_region;	
extern region bucketptr_region; 
extern region keystrbucket_region;

/* Regions for list.c */
extern region list_header_region;
extern region list_node_region;  
extern region list_strnode_region; 

/* Regions for ufind.c */
extern region uf_element_region;
extern region ustack_element_region; 

/* Regions for hash_bounds.c */
extern region bounds_region;	
extern region added_edge_info_region;

/* Regions for setif-var.c */
extern region setif_var_region;	
extern region sv_info_region;	

/* Regions for termhash.c */
extern region hash_entry_region;
extern region term_bucket_region;
extern region term_hash_region;
extern region strbucket_region;
extern region term_bucketptr_region;

/* Regions for setif-sort.c */
extern region setif_rollback_info_region;
extern region added_ub_proj_info_region; 
extern region setif_term_region;	
extern region setif_union_region;
extern region setif_inter_region;
extern region setif_constant_region;

/* Regions for term-var.c */
extern region term_var_region;	

/* Regions for flowrow_sort.c */
extern region flowrow_field_region; 
extern region flowrow_region;	    
extern region contour_region;	    
extern region flowrow_rollback_info_region;
extern region flow_var_region;		    


/* Regions for nonspec.c */
extern region constructor_region; 
extern region cons_expr_region;	  
extern region cons_group_region;  
extern region proj_pat_region;	  
extern region gproj_pat_region;	  
extern region gen_e_ptr_region;

/* Regions for term-sort.c */
extern region term_constant_region;

/*****************************************************************************
 *                                                                           *
 *   Update functions for various data types                                 *
 *                                                                           *
 *****************************************************************************/

/* Update functions for banshee_region_persist_kinds.c */
int update_nonptr_data(translation t, void *m); 
int update_ptr_data(translation t, void *m);	

/* Update functions for hash.c */
int update_hash_table(translation t, void *m);
int update_bucket(translation t, void *m);
int update_strbucket(translation t, void *m);
int update_bucketptr(translation t, void *m);
int update_keystrbucket(translation t, void *m);

/* Update functions for list.c */
int update_list_header(translation t, void *m);
int update_list_node(translation t, void *m);
int update_list_strnode(translation t, void *m);

/* Update functions for ufind.c */
int update_uf_element(translation t, void *m);
int update_ustack_element(translation t, void *m);

/* Update functions for hash_bounds.c */
int update_bounds(translation t, void *m);
int update_added_edge_info(translation t, void *m);

/* Update functions for setif-var.c */
int update_sv_info(translation t, void *m);
int update_setif_var(translation t, void *m);

/* Update functions for termhash.c  */
int update_hash_entry(translation t, void *m); 
int update_term_bucket(translation t, void *m);
int update_term_hash(translation t, void *m);
int update_term_bucketptr(translation t, void *m);

/* Update functions for setif-sort.c */
int update_setif_term(translation t, void *m);
int update_added_ub_proj_info(translation t, void *m);
int update_setif_rollback_info(translation t, void *m);
int update_setif_constant(translation t, void *m);
int update_setif_union(translation t, void *m);
int update_setif_inter(translation t, void *m);

/* Update functions for term-var.c */
int update_term_var(translation t, void *m);

/* Update functions for flowrow.c */
int update_flowrow_rollback_info(translation t, void *m);
int update_flowrow(translation t, void *m);
int update_flowrow_field(translation t, void *m);
int update_contour(translation t, void *m);
int update_flow_var(translation t, void *m);

/* Update functions for nonspec.c */
int update_proj_pat(translation t, void *m);
int update_gproj_pat(translation t, void *m);
int update_constructor(translation t, void *m);
int update_cons_group(translation t, void *m);
int update_cons_expr(translation t, void *m);
int update_gen_e_ptr(translation t, void *m);

/* Update functions for term-sort.c */
int update_term_constant(translation t, void *m);

EXTERN_C_END

/* Modules that use persistent regions */

/* banshee_region_persist_kinds.c */
/* hash.c */
/* list.c */
/* ufind.c */
/* hash_bounds.c */
/* setif-var.c */
/* termhash.c  */
/* setif-sort.c */
/* term-var.c */
/* flowrow.c */
/* nonspec.c */
/* term-sort.c */

