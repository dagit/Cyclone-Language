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

#include <stdio.h>
#include "banshee_region_persist_kinds.h"
#include "persist.h"
#include "hash.h"

#define UNKNOWN_ID2 -2
#define UNKNOWN_ID -1

/* A region containing pointers. The update function will call
   update_pointer on adjacent word values */
region banshee_ptr_region = NULL;

/* A region containing non pointers. The update function won't touch
   any of these values */
region banshee_nonptr_region = NULL;

/* Table containing any extra regions that have been registered as
   persistent-- keys are regions, values are Updaters */
hash_table extra_regions = NULL;

extern hash_table fn_ptr_table;
Updater extra_update_fn = NULL;
Updater extra_update_fn2 = NULL;

void banshee_region_persistence_init()
{
  region_init();

  banshee_ptr_region = newregion();
  banshee_nonptr_region = newregion();

  /* TODO -- make this persistent */
  extra_regions = make_hash_table(permanent, 32, ptr_hash, ptr_eq);
}

int update_nonptr_data(translation t, void *m)
{
  return sizeof(void *);
}

int update_ptr_data(translation t, void *m)
{
  update_pointer(t, m);
  return sizeof(void *);
}

void write_extra_info(const char *filename, unsigned long num_regions)
{
  int count = 0;
  hash_table_scanner scan;
  Updater next_updater = NULL;
  region next_region = NULL;
  FILE *f = fopen(filename, "wb");
  int unknown_id = UNKNOWN_ID;
  int unknown_id2 = UNKNOWN_ID2;

  assert(f);

  fwrite((void *)&num_regions, sizeof(unsigned long), 1, f);

  hash_table_scan(extra_regions, &scan);
  assert(fn_ptr_table);
  while(hash_table_next(&scan, (hash_key *)&next_region, 
			(hash_data *)&next_updater)) {
    int id = 0;
    bool success = hash_table_lookup(fn_ptr_table, (hash_key)next_updater, (hash_data *)&id);
    if (!success) { 
      if (next_updater == extra_update_fn)
	fwrite((void *)&unknown_id, sizeof(int), 1, f);
      else if (next_updater == extra_update_fn2)
	fwrite((void *)&unknown_id2, sizeof(int), 1, f);
      else 
	fail("Error: couldn't figure out what function %d corresponds to\n", id);
    }
    else {
      fwrite((void *)&id, sizeof(int), 1 , f);
    }
    count++;
  }
  assert(count == num_regions);
}

Updater *read_extra_info(const char *filename)
{
  unsigned long num_extra_regions = 0, i = 0;
  int next_id = 0;
  FILE *f  = fopen(filename, "rb");
  Updater *result;
  assert(f);

  fread((void *)&num_extra_regions, sizeof(unsigned long), 1, f);

  result = rarrayalloc(permanent,num_extra_regions + NUM_REGIONS, 
		       Updater);

  for (i = NUM_REGIONS; i < NUM_REGIONS + num_extra_regions; i++) {
    fread((void *)&next_id, sizeof(int), 1, f); 
    if (next_id == UNKNOWN_ID) {
      assert(extra_update_fn);
      result[i] = extra_update_fn;
    }
    else if (next_id == UNKNOWN_ID2) {
      assert(extra_update_fn2);
      result[i] = extra_update_fn2;
    }
    else {
      result[i] = update_funptr_data(next_id);
    }
  }

  return result;
}

region *get_persistent_regions(const char *filename)
{
#ifndef NONSPEC
  return NULL;
#else
  region *result;
  unsigned long num_extra_regions = hash_table_size(extra_regions);

  result = rarrayalloc(permanent, num_extra_regions + NUM_REGIONS+1, region);
  /* TODO -- swap table region and banshee_nonptr_region */
  result[3] = banshee_nonptr_region;
  result[2] = banshee_ptr_region;
  result[1] = bucket_region;
  result[35] = table_region;	
  result[4] = strbucket_region;

  result[5] = list_header_region; 
  result[6] = list_node_region;  
  result[7] = list_strnode_region;
  result[8] = uf_element_region; 
  result[9] = ustack_element_region;

  result[10] = bounds_region;
  result[11] = added_edge_info_region;
  result[12] = setif_var_region;	
  result[13] = sv_info_region;
  result[14] = hash_entry_region;

  result[15] = term_bucket_region;
  result[16] = term_hash_region;
  result[17] = setif_term_region;
  result[18] = added_ub_proj_info_region;  
  result[19] = setif_rollback_info_region; 

  result[20] = term_var_region;	
  result[21] = flowrow_rollback_info_region; 
  result[22] = flowrow_region;	   
  result[23] = flowrow_field_region;
  result[24] = contour_region;	    

  result[25] = flow_var_region;	
  result[26] = proj_pat_region;	
  result[27] = gproj_pat_region;	
  result[28] = constructor_region; 
  result[29] = cons_group_region;  

  result[30] = cons_expr_region;	
  result[31] = term_constant_region; 
  result[32] = bucketptr_region;
  result[33] = keystrbucket_region;
  result[34] = gen_e_ptr_region;
  result[0] = term_bucketptr_region;
  result[36] = setif_constant_region;
  result[37] = setif_inter_region;
  result[38] = setif_union_region;

  {
    int count = 0;
    int i = NUM_REGIONS;
    region next_region;
    hash_table_scanner scan;
    hash_table_scan(extra_regions, &scan);
    
    while(hash_table_next(&scan, (hash_key *)&next_region, 
			  NULL)) {
      result[i++] = next_region;
      count++;
    }
    assert(count == num_extra_regions);
  }

  result[num_extra_regions + NUM_REGIONS] = NULL;

  write_extra_info(filename, num_extra_regions);

  return result;
#endif	/* NONSPEC */
}

Updater *get_updater_functions(const char *filename)
{
#ifndef NONSPEC
  return NULL;
#else 
  Updater *result;

  result = read_extra_info(filename); 
    //rarrayalloc(permanent, NUM_REGIONS, Updater);

  /* TODO -- swap back update_hash_table and update_nonptr_data */
  result[3] = update_nonptr_data;
  result[2] = update_ptr_data;
  result[1] = update_bucket;
  result[35] = update_hash_table;
  result[4] = update_strbucket;
 
  result[5] = update_list_header;
  result[6] = update_list_node;
  result[7] = update_list_strnode;
  result[8] = update_uf_element;
  result[9] = update_ustack_element;
  
  result[10] = update_bounds;
  result[11] = update_added_edge_info;
  result[12] = update_setif_var;
  result[13] = update_sv_info;
  result[14] = update_hash_entry; 
  
  result[15] = update_term_bucket;
  result[16] = update_term_hash;
  result[17] = update_setif_term;
  result[18] = update_added_ub_proj_info;
  result[19] = update_setif_rollback_info;
  
  result[20] = update_term_var;
  result[21] = update_flowrow_rollback_info;
  result[22] = update_flowrow;
  result[23] = update_flowrow_field;
  result[24] = update_contour;
  
  result[25] = update_flow_var;
  result[26] = update_proj_pat;
  result[27] = update_gproj_pat;
  result[28] = update_constructor;
  result[29] = update_cons_group;

  result[30] = update_cons_expr;
  result[31] = update_term_constant;
  result[32] = update_bucketptr;
  result[33] = update_keystrbucket;
  result[34] = update_gen_e_ptr;
  result[0] = update_term_bucketptr;
  result[36] = update_setif_constant;
  result[37] = update_setif_inter;
  result[38] = update_setif_union;


  return result;
#endif  /* NONSPEC  */
}

void register_persistent_region(region r, Updater u)
{
  bool success = hash_table_insert(extra_regions, r, u);

  if (!success)
    fail("Error: attempted to register a region twice in register_persistent_region\n");
}

void unregister_persistent_region(region r)
{
  bool success = hash_table_remove(extra_regions, r);

  /* This is (sort of) a benign warning. The issue is that we haven't
     called update_pointer on these regions, so we may occasionally
     (from termhash.c) try to unregister a region that hasn't been
     updated properly */
  if (! success)
    fprintf(stderr, "Warning: attempted to unregister a nonexistent region in unregister_persistent_region\n");
}
