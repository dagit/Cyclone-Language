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

/* Test persistence functionality */

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

#define DEFAULT_FILENAME "persist-test.out"

#define NODE_PERSIST_KIND 3
#define HASH_PERSIST_KIND 4

/* Types */
struct node_ {
  int data;
  struct node_ *left;
  struct node_ *right;
};

typedef struct node_ *node;

/* Globals */
static region test_rgn;

/* Persistence support */
bool node_serialize(FILE *f, void *obj)
{
  node n = (node) obj;
  
  assert(f);
  assert(obj);

  fwrite((void *)&n->data, sizeof(int), 1, f);
  fwrite((void *)&n->left, sizeof(void *), 1, f);
  fwrite((void *)&n->right, sizeof(void *), 1, f);

  serialize_object(n->left, NODE_PERSIST_KIND);
  serialize_object(n->right, NODE_PERSIST_KIND);


  return TRUE;
}

void *node_deserialize(FILE *f)
{
  node n = ralloc(test_rgn, struct node_);
  
  fread((void *)&n->data, sizeof(int), 1, f);
  fread((void *)&n->left, sizeof(void *), 1, f);
  fread((void *)&n->right, sizeof(void *), 1, f);

  return n;
}

bool node_set_fields(void *obj)
{
  node n = (node) obj;

  deserialize_set_obj((void **)&n->left);
  deserialize_set_obj((void **)&n->right);

  return TRUE;
}


/* Main */
static void show_usage(char *progname)
{
  printf("Usage: %s [--out --in]\n", progname);
}

static void pt_serialize()
{
  node n1;
  hash_table table;
  /* Build a small graph and a hash table */
  { 				
    node n2,n3,n4;
    
    table = make_persistent_string_hash_table(4, 
					      0);

    n1 = ralloc(test_rgn, struct node_);
    n2 = ralloc(test_rgn, struct node_);
    n3 = ralloc(test_rgn, struct node_);
    n4 = ralloc(test_rgn, struct node_);
    
    n1->data = 1;
    n2->data = 2;
    n3->data = 3;
    n4->data = 4;
    
    n1->left = n2;
    n1->right = n3;
    
    n2->left = n4;
    n2->right = n4;
    
    n3->left = n4;
    n3->right = n4;
    
    n4->left = n1;
    n4->right = n1;

    hash_table_insert(table, (hash_key)"n1", (hash_data)n1->data);
    hash_table_insert(table, (hash_key)"n2", (hash_data)n2->data);
    hash_table_insert(table, (hash_key)"n3", (hash_data)n3->data);
    hash_table_insert(table, (hash_key)"n4", (hash_data)n4->data);
  }

  /* Serialize it */
  {  
    FILE *outfile;
    serialize_fn_ptr serialize_fns[5] = {nonptr_data_serialize, 
					 funptr_data_serialize, 
					 string_data_serialize,
					 node_serialize,
					 hash_table_serialize};

    outfile = fopen(DEFAULT_FILENAME, "wb");

    /* Write down the old address of n1 */
    fwrite((void *)&n1, sizeof(void *), 1, outfile);
    
    /* Write down the old address of table */
    fwrite((void *)&table, sizeof(void *), 1, outfile);
    
    serialize_start(outfile, serialize_fns, 5);
    serialize_object(n1, NODE_PERSIST_KIND);
    serialize_object(table,HASH_PERSIST_KIND);
    serialize_end();

    fclose(outfile);
  }
}

static void pt_deserialize()
{
  node n = NULL;
  FILE *infile = NULL;
  hash_table table = NULL;
  deserialize_fn_ptr deserialize_fns[5] = {nonptr_data_deserialize,
					   funptr_data_deserialize,
					   string_data_deserialize,
					   node_deserialize,
					   hash_table_deserialize};
  set_fields_fn_ptr set_fields_fns[5] = {nonptr_data_set_fields,
					 funptr_data_set_fields,
					 string_data_set_fields,
					 node_set_fields,
					 hash_table_set_fields};

  infile = fopen(DEFAULT_FILENAME, "rb");
  
  if (infile == NULL) {
    printf("Couldn't open %s\n", DEFAULT_FILENAME);
    exit(1);
  }

  /* Read in the old address of n1 and store it as n */
  fread((void *)&n, sizeof(void *), 1, infile);

  /* Read in the old address of table and store it as table */
  fread((void *)&table, sizeof(void *), 1, infile);

  deserialize_all(infile, deserialize_fns, set_fields_fns, 5);
  
  /* Now update n with its new address */
  deserialize_set_obj((void **)&n);

  /* Update table with its new address */
  deserialize_set_obj((void **)&table);

  deserialize_end();

  /* Close the infile */
  fclose(infile);

  /* Check well-formedness of the graph */
  if (n->data != 1) {
    fail("n1's data was not deserialized correctly: %d\n",n->data);
  }
  if (n->left->data != 2) {
    fail("n2 was not deserialized correctly.\n");
  }
  if (n->right->data != 3) {
    fail("n3 was not deserialized correctly.\n");
  }
  if (n->left->right != n->left->left) {
    fail("n2's children were not deserialized correctly.\n");
  }
  if (n->right->left != n->right->left) {
    fail("n3's children were not deserialized correctly.\n");
  }
  if (n->left->left != n->right->left) {
    fail("n2 and n3 do not have the same children.\n");
  }
  if (n->left->left->data != 4) {
    fail("n4's data was not deserialized correctly.\n");
  }
  if (n->left->left->left != n || n->left->left->right != n) {
    fail("n4's children were not deserialized correctly.\n");
  }
  
  /* Check well-formedness of the hash table */
  {
    hash_table_scanner scan;
    hash_key next_key;
    hash_data next_data;

    hash_table_scan(table,&scan);

    while(hash_table_next(&scan, &next_key, &next_data)) {
      if ((int)next_data == 1) {
	if (strcmp("n1",next_key)) {
	  fail("hash table doesn't map n1 correctly.\n");
	}
      }
      else if ((int)next_data == 2) {
	if (strcmp("n2",next_key)) {
	  fail("hash table doesn't map n2 correctly.\n");
	}
      }
      else if ((int)next_data == 3) {
	if (strcmp("n3",next_key)) {
	  fail("hash table doesn't map n3 correctly.\n");
	}
      }
      else if ((int)next_data == 4) {
	if (strcmp("n4",next_key)) {
	  fail("hash toble doesn't map n4 correctly.\n");
	}
      }
      else {
	fail("There is an unknown mapping in the hash table.\n");
      }
    }
    
  }
}

void seed_fn_ptr_table(region r);

int main(int argc, char **argv)
{ 
  assert(argc > 0);

  region_init();
  list_init();
  hash_table_init();
  banshee_region_persistence_init();
  seed_fn_ptr_table(permanent);

  test_rgn = newregion();

  if (argc == 1) {
    pt_serialize();
    pt_deserialize();
    unlink(DEFAULT_FILENAME);
  }
  else if (argc == 2) {
    /* Serialize */
    if (!strcmp(argv[1], "--out")) {
      pt_serialize();
    }
    /* Deserialize */
    else if (!strcmp(argv[1], "--in")) {
      pt_deserialize();
    }
    else {
      show_usage(argv[0]);
    }
  }
  else {
    show_usage(argv[0]);
  }

  return 0;
}
