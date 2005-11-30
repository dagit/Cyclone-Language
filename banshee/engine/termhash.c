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
#include <string.h>
#include "termhash.h"
#include "hash.h"
#include "termhash.h"
#include "utils.h"
#include "banshee_persist_kinds.h"
#include "banshee_region_persist_kinds.h"
#define UB(n) ((1<<n)-1)   
#define CAP(n) (1<<n)      
#define INITIAL_TABLE_SIZE 8 /* the initial table size is 2^8 */
  
/*  An individual entry in the table consists of an array of stamps  */ 
/*  (same arity as the expr's constructor) in addition to the expr */ 
/*  itself. */ 
typedef struct hash_entry *hash_entry;

/*  A term_bucket is a list of entries (an list of exprs that have  */ 
/*  collided after hashing) */ 
typedef struct term_bucket *term_bucket;

bool flag_hash_cons = TRUE;

struct hash_entry
{
  int length;
  stamp *stamps;
  gen_e e;
};

struct term_bucket
{
  hash_entry entry;
  struct term_bucket *next;
};

#define scan_term_bucket(b,var) for(var = b; var; var = var->next)

/*  size: initial_table_size + number of rehashes */ 
/*  capacity: 2^size (for array size) */ 
/*  ub: 2^size-1 (for array indexing) */ 
/*  inserts: num of elements inserted into the array */ 
struct term_hash_
{
  term_bucket * term_buckets;
  region rgn;
  int ub;
  int size;
  int capacity;
  int inserts;
};

/* TODO -- this will currently leak on persistent termhashes when rehashing */
/* Need to switch to internal regions */
region hash_entry_region = NULL;
region term_bucket_region = NULL;
region term_hash_region = NULL;
region term_bucketptr_region = NULL;

static int hash(int ub, stamp stamps[], int len);
static void post_insert(term_hash tab) deletes;
static void rehash(term_hash tab) deletes;
static void reinsert(term_hash tab, term_bucket b);
static void insert(term_hash tab, gen_e e, stamp * stamps, int len);
static void insert_entry(term_hash tab, struct hash_entry *entry);
static gen_e walk(term_bucket b, stamp * stamps, int len);

static const int primes[] =
  { 83, 1789, 5189, 5449, 5659, 6703, 7517, 7699, 8287, 8807, 9067, 9587,
    10627, 10939, 11239};
/*
static const int prime_1 = 83;
static const int prime_2 = 1789;
*/
static const int initial_table_size = INITIAL_TABLE_SIZE;

term_hash make_term_hash(region rgn)
{
  int ub, n;
  int i;

  region r = NULL;
  
  term_hash tab = ralloc(rgn ? rgn : term_hash_region, struct term_hash_);

  ub = UB(initial_table_size);
  n = CAP(initial_table_size);
  
  
  tab->term_buckets = rarrayalloc(rgn ? r = newregion() : 
				  term_bucketptr_region, n, 
				  term_bucket);
  
  for (i = 0; i < n; i++)
    {
      tab->term_buckets[i] = NULL;
    }

  
  if (rgn) {
    tab->rgn = r;
  }
  else {
    tab->rgn = NULL;
  }

  tab->ub = ub;
  tab->size = initial_table_size;
  tab->capacity = n;
  tab->inserts = 0;
  return tab;
}

void term_hash_delete(term_hash tab) deletes
{
  if (tab->rgn) deleteregion(tab->rgn);
}

gen_e term_hash_find(term_hash tab, stamp stamps[], int len)
{
  int hash_val;
  term_bucket b;

  if (!flag_hash_cons) return NULL;

  hash_val = hash(tab->ub, stamps, len);
  assert(hash_val < tab->capacity);
  b = tab->term_buckets[hash_val];
  return walk(b, stamps, len);
}

static gen_e walk(term_bucket b, stamp stamps[], int len)
{
  term_bucket cur;
  scan_term_bucket(b,cur)
    {
      if (len == cur->entry->length 
	  && (memcmp(stamps, cur->entry->stamps, sizeof(int)*len) == 0) )
	return cur->entry->e;
    }
  return NULL;
}

/*  Should call t_hash_find to see if a gen_e with the given stamp  */ 
/*  signature is already in the table. If so, insert should return */ 
/*  true and do nothing. */ 
bool term_hash_insert(term_hash tab, gen_e e, stamp * stamps, int len) deletes
{
  if (!flag_hash_cons) return FALSE;

  if (term_hash_find(tab, stamps, len) != NULL)
    {
      return TRUE;
    }
  insert(tab, e, stamps, len);
  post_insert(tab);
  return FALSE;
}


/*  Insert an expression e represented by the given stamp array into */ 
/*  the hash table. */ 
static void insert(term_hash tab, gen_e e, stamp stamps[], int len)
{
  hash_entry entry;
  stamp * stamp_cpy;
  int i;

  entry = ralloc(tab->rgn ? tab->rgn : hash_entry_region, struct hash_entry);

  stamp_cpy = rarrayalloc(tab->rgn ? tab->rgn : banshee_nonptr_region, len, stamp);
  for (i = 0; i < len; i++)
    {
      stamp_cpy[i] = stamps[i];
    }

  entry->length = len;
  entry->stamps = stamp_cpy;
  entry->e = e;
  insert_entry(tab, entry);
}

static void insert_entry(term_hash tab, hash_entry entry)
{
  int hash_val;

  term_bucket b, new_term_bucket;
  hash_val = hash(tab->ub, entry->stamps, entry->length);
  assert(hash_val < tab->capacity);
  b = tab->term_buckets[hash_val];
  new_term_bucket = ralloc(tab->rgn ? tab->rgn : term_bucket_region, 
			   struct term_bucket);

  new_term_bucket->entry = entry;
  new_term_bucket->next = b;
  assert(hash_val < tab->capacity);
  tab->term_buckets[hash_val] = new_term_bucket;
}

static void post_insert(term_hash tab) deletes
{
  if (tab->capacity == ++tab->inserts)
    {
      rehash(tab);
    }
}

/*  Double the size of the hash table and reinsert all of the elements. */ 
static void rehash(term_hash tab) deletes
{
  region old_rgn = NULL;
  term_bucket * old_term_buckets;
  int i;
  int old_table_size = tab->capacity;

  old_term_buckets = tab->term_buckets;
  tab->capacity *= 2;
  tab->ub = UB(++tab->size);

  if (tab->rgn) {
    old_rgn = tab->rgn;
    tab->rgn = newregion();
  }
  
  tab->term_buckets = rarrayalloc(tab->rgn ? tab->rgn : term_bucketptr_region, 
				  tab->capacity, term_bucket);
  for (i = 0; i < old_table_size; i++)
    {
      if (old_term_buckets[i] != NULL && old_term_buckets[i]->entry != NULL)
	reinsert(tab, old_term_buckets[i]);
    }

  if (tab->rgn) {
    deleteregion(old_rgn);
  }
  
}

static void reinsert(term_hash tab, term_bucket b)
{
  term_bucket cur;
  scan_term_bucket(b,cur)
    insert(tab, cur->entry->e, cur->entry->stamps, cur->entry->length);
}

static int hash(int ub, stamp stamps[], int len)
{
  int i, n;

  n = 0;
  for (i = 0; i < len; i++)
    {
      n = (n + (primes[i % 15] * abs(stamps[i]))) & ub;
    }
  return n;
}

bool term_hash_serialize(FILE *f, void *obj)
{
  int i;
  term_hash h = (term_hash)obj;
  term_bucket cur;
  assert(f);
  assert(obj);

  fwrite((void *)&h->inserts, sizeof(int), 1, f);

  for (i = 0; i < h->capacity; i++) {
    scan_term_bucket(h->term_buckets[i], cur) {
      fwrite((void *)&cur->entry->length, sizeof(int), 1, f);
      fwrite((void *)cur->entry->stamps, sizeof(stamp), cur->entry->length, f);
      fwrite((void *)&cur->entry->e, sizeof(gen_e), 1, f);
      serialize_banshee_object(cur->entry->e, gen_e);
    }
  }
  return TRUE;
}

void *term_hash_deserialize(FILE *f)
{
  int inserts,i;
  term_hash result = make_term_hash(permanent);

  fread((void *)&inserts, sizeof(int), 1, f);

  for (i = 0; i < inserts; i++) {
    int length;
    fread((void *)&length, sizeof(int), 1, f);
    {
      gen_e e;
      stamp sig[length];
      fread((void *)sig, sizeof(stamp), length, f);
      fread((void *)&e, sizeof(gen_e), 1, f);
      term_hash_insert(result, e, sig, length);
    }
  }

  assert (result->inserts == inserts);

  return result;
}

bool term_hash_set_fields(void *obj)
{
  int i;
  term_bucket cur;
  term_hash h = (term_hash)obj;
  

  for (i = 0; i < h->capacity; i++) {
    scan_term_bucket(h->term_buckets[i], cur) {
      deserialize_set_obj((void **)&cur->entry->e);
    }
  }

  return TRUE;
}

void term_hash_init()
{
  term_bucketptr_region = newregion();
  term_bucket_region = newregion();
  term_hash_region = newregion();
  hash_entry_region = newregion();
}

void term_hash_reset()
{
  deleteregion(term_bucketptr_region);
  deleteregion(term_bucket_region);
  deleteregion(term_hash_region);
  deleteregion(hash_entry_region);
  
  term_hash_init();
}

int update_hash_entry(translation t, void *m) 
{
  update_pointer(t, (void **)&((hash_entry)m)->e);
  update_pointer(t, (void **)&((hash_entry)m)->stamps);
  return sizeof(struct hash_entry);
}

int update_term_bucket(translation t, void *m)
{
  term_bucket tb = (term_bucket)m;
  
  update_pointer(t, (void **)&tb->entry);
  update_pointer(t, (void **)&tb->next);
  
  return sizeof(struct term_bucket);
}

int update_term_hash(translation t, void *m)
{
  update_pointer(t, (void **)&((term_hash)m)->term_buckets);
  return sizeof(struct term_hash_);
}

int update_term_bucketptr(translation t, void *m)
{
  update_pointer(t, m);
  return (sizeof(void *));
}
