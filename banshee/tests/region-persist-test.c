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

/* Test persistence functionality at the region level */

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "regions.h"
#include "persist.h"
#include "hash.h"
#include "utils.h"
#include "banshee_region_persist_kinds.h"
#include "list.h"

#define NUM_NODES 100000

struct node_ {
  int data;
  struct node_ *next;
};

typedef struct node_ *node;

DECLARE_LIST(node_list, node);
DEFINE_KIND_LIST(node_list, node, 1);

hash_table thetable;
node_list thelist;

int update_node(translation t, void *m) {
  update_pointer(t, (void **) &((struct node_ *) m)->next);
  return(sizeof(struct node_));
}

void update()
{
  Updater u[10];
  translation t;
  region temp = newregion();

  u[9] = update_strbucket;
  u[8] = update_ptr_data;
  u[7] = update_list_strnode;
  u[6] = update_list_node;
  u[5] = update_list_header;
  u[4] = update_nonptr_data;
  u[3] = update_bucket;
  u[2] = update_hash_table;
  u[1] = update_bucketptr;
  u[0] = update_node;

  t = deserialize("data", "offsets", u, temp);
  thetable = (hash_table) translate_pointer(t, (void *)thetable);
  thelist = (node_list) translate_pointer(t, (void *)thelist);
}

int verify()
{
  int j = 0;
  char str[512];
  node next_node;

  assert(node_list_length(thelist) == NUM_NODES);
  
  for (j = 0; j < NUM_NODES; j++) {
    snprintf(str, 512, "node(%d)", j);
    hash_table_lookup(thetable, str, (hash_data *)&next_node);
    if (next_node->data != j) {
      return 0;
    }
    if (node_list_head(thelist)->data != j) {
      return 0;
    }
    thelist = node_list_tail(thelist);
  }

  /* Try allocating something else */
  {
    node n = ralloc(newregion(), struct node_);
    n->next = NULL;
  }
  
  return 1;
}

void seed_fn_ptr_table(region r);

int main(int argc, char *argv[])
{
  region r[11];
  int i = 0;
  node n = NULL;
  region node_region;
  
  region_init();
  list_init();
  hash_table_init();
  banshee_region_persistence_init();
  seed_fn_ptr_table(newregion());

  node_region = newregion();

  r[10] = NULL;
  r[9] = strbucket_region;
  r[8] = banshee_ptr_region;
  r[7] = list_strnode_region;
  r[6] = list_node_region;
  r[5] = list_header_region;
  r[4] = banshee_nonptr_region;
  r[3] = bucket_region;
  r[2] = table_region;
  r[1] = bucketptr_region;
  r[0] = node_region;

  thetable = make_persistent_string_hash_table(8, 1);
  thelist = new_persistent_node_list();

  for(i = 0; i < NUM_NODES; i++) {
    char str[512];
    node prev = n;
    n = ralloc(node_region, struct node_);
    n->data = i;
    n->next = prev;

    snprintf(str, 512,"node(%d)", i);

    hash_table_insert(thetable, (hash_key)rstrdup(banshee_nonptr_region, str),
		      (hash_data)n);
    node_list_append_tail(n, thelist);
  }

  serialize(r, "data", "offsets");

  update();
  if (!verify()) {
    printf("Failed region persist test\n");
    exit(1);
  }
  else {
    printf("Passed region persist test\n");
    exit(0);
  }

}
