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
#include "hash.h"
#include "regions.h"
#include "utils.h"
#include "persist.h"
#include "banshee_region_persist_kinds.h"

struct bucket_
{
  hash_key key;
  hash_data data;
  struct bucket_ *next;
};

#define scan_bucket(b, var) for (var = b; var; var = var->next)

#define BUCKETPTR_REGION bucketptr_region
/* #define BUCKETPTR_REGION banshee_ptr_region */



/* TODO -- switch this back */
/* #define bucket_region(t) bucket_region */

extern const int num_fn_ptrs;

struct Hash_table
{
  region r;         		/* Region for this table */
  hash_fn hash;     		/* Function for hashing keys */
  keyeq_fn cmp;     		/* Function for comparing keys */

  unsigned long size;		/* Number of buckets */
  unsigned long log2size;	/* log2 of size */
  unsigned long used;		/* Number of elements */

  int key_persist_kind; 	/* Kind for persistent keys */
  int data_persist_kind;	/* Kind for persistent data */
  int hash_fn_id;		
  int keyeq_fn_id; 

  bucket *table;		/* Array of (size) buckets */
};

region bucket_region = NULL;
region table_region = NULL;
region strbucket_region = NULL;
region bucketptr_region = NULL;
region keystrbucket_region = NULL;

/* #define bucket_region(t) t->r ? t->r : (t->data_persist_kind ? bucket_region : strbucket_region) */

/* #define bucket_region(t) t->r ? t->r : (t->data_persist_kind && t->key_persist_kind ? bucket_region : (t->data_persist_kind ? keystrbucket_region : (t->key_persist_kind ? strbucket_region : NULL))) */

static region get_bucket_region(hash_table t)
{
  return (t->r ? t->r : (t->data_persist_kind && t->key_persist_kind ? bucket_region : (t->data_persist_kind ? keystrbucket_region : (t->key_persist_kind ? strbucket_region : NULL))));
}


static void rehash(hash_table ht);

extern hash_table fn_ptr_table;

static hash_table make_hash_table_int(region r, unsigned long size, 
				      hash_fn hash, keyeq_fn cmp,
				      int key_persist_kind,
				      int data_persist_kind)
{
  hash_table result;

  assert(size > 0);
  assert(table_region);
  assert(bucket_region);
  assert(bucketptr_region);

  result = ralloc(r? r : table_region, 
		  struct Hash_table);
  result->r = r;
  result->hash = hash;
  result->cmp = cmp;
  result->size = 1;
  result->log2size = 0;

  if (fn_ptr_table) {
    hash_table_lookup(fn_ptr_table, (hash_key)hash, 
		      (hash_data*)&result->hash_fn_id);
    hash_table_lookup(fn_ptr_table, (hash_key)cmp,
		      (hash_data*)&result->keyeq_fn_id);
  }

    result->key_persist_kind = key_persist_kind;
    result->data_persist_kind = data_persist_kind;

  /* Force size to a power of 2 */
  while (result->size < size)
    {
      result->size *= 2;
      result->log2size++;
    }
  result->used = 0;
  result->table = rarrayalloc(result->r ? r : BUCKETPTR_REGION, result->size, bucket);

  return result;
}


hash_table make_persistent_hash_table(unsigned long size, 
				      hash_fn hash, keyeq_fn cmp,
				      int key_persist_kind,
				      int data_persist_kind)
{
  return make_hash_table_int(NULL, size, hash, cmp, key_persist_kind, 
			     data_persist_kind);
}

/* Make a new hash table, with size buckets initially.  Hash table
   elements are allocated in region rhash. */
hash_table make_hash_table(region r, unsigned long size, hash_fn hash,
			   keyeq_fn cmp)
{
  return make_hash_table_int(r, size, hash, cmp, -1, -1);
}

hash_table make_persistent_string_hash_table(unsigned long size,
					     int data_persist_kind)
{
  return make_hash_table_int(NULL, size, (hash_fn)string_hash,
			     (keyeq_fn)string_eq, 
			     STRING_PERSIST_KIND, data_persist_kind);
}
					     

/* Make a hash table for strings. */
hash_table make_string_hash_table(region rhash, unsigned long size)
{
  return make_hash_table(rhash, size, (hash_fn) string_hash,
			 (keyeq_fn) string_eq);
}

/* Zero out ht.  Doesn't reclaim bucket space. */
void hash_table_reset(hash_table ht)
{
  unsigned long i;

  ht->used = 0;
  for (i = 0; i < ht->size; i++)
    ht->table[i] = NULL;
}

/* Return the number of entries in ht */
unsigned long hash_table_size(hash_table ht)
{
  return ht->used;
}

#define MAGIC 2*0.6180339987

#define LLMAGIC ((unsigned long long)(MAGIC * (1ULL << (8 * sizeof(unsigned long) - 1))))

/* Return the bucket corresponding to k in ht */
static inline bucket *find_bucket(hash_table ht, hash_key k)
{
  unsigned long hash;

  hash = ht->hash(k);
  hash = hash * LLMAGIC;
  hash = hash >> (8 * sizeof(unsigned long) - ht->log2size);
  if (ht->size == 1)
    hash = 0;
  /* hash = hash % ht->size; */
  assert(hash < ht->size);
  return &ht->table[hash];
}

/* Given a comparison function which agrees with our hash_function,
   search for the given element. */
bool hash_table_hash_search(hash_table ht, keyeq_fn cmp, 
			    hash_key k, hash_data *d)
{
  bucket cur;

  cur = *find_bucket(ht, k);
  while (cur)
    {
      if (cmp(k, cur->key))
	{
	  if (d)
	    *d = cur->data;
	  return TRUE;
	}
      cur = cur->next;
    }
  return FALSE;
}

/* Lookup k in ht.  Returns corresponding data in *d, and function
   result is TRUE if the k was in ht, false otherwise. */
bool hash_table_lookup(hash_table ht, hash_key k, hash_data *d)
{
  return hash_table_hash_search(ht, ht->cmp, k, d);
}

/* Add k:d to ht.  If k was already in ht, replace old entry by k:d.
   Rehash if necessary.  Returns TRUE if k was not already in ht. */
bool hash_table_insert(hash_table ht, hash_key k, hash_data d)
{
  bucket *cur;

  if (ht->used > 3 * ht->size / 4)
    rehash(ht);
  cur = find_bucket(ht, k);
  while (*cur)
    {
      if (ht->cmp(k, (*cur)->key))
	{
	  (*cur)->data = d;
	  return FALSE; /* Replace */
	}
      cur = &(*cur)->next;
    }
  *cur = ralloc(get_bucket_region(ht), struct bucket_);
  (*cur)->key = k;
  (*cur)->data = d;
  (*cur)->next = NULL;
  ht->used++;
  return TRUE; /* New key */
}

/* Remove mapping for k in ht.  Returns TRUE if k was in ht. */
bool hash_table_remove(hash_table ht, hash_key k) 
{
  bucket *cur;
  bucket prev = NULL;

  cur = find_bucket(ht, k);
  while (*cur)
    {
      if (ht->cmp(k, (*cur)->key))
	{
	  if (prev)
	    prev->next = (*cur)->next;
	  else
	    *cur = (*cur)->next;
	  ht->used--;
	  return TRUE;
	}
      prev = *cur;
      cur = &(*cur)->next;
    }
  return FALSE;
}

/* Return a copy of ht */
hash_table hash_table_copy(region r, hash_table ht)
{
  unsigned long i;
  hash_table result;
  bucket cur, newbucket, *prev;

  result = make_hash_table_int(r, ht->size, ht->hash, ht->cmp,
			       ht->key_persist_kind,
			       ht->data_persist_kind);
  result->used = ht->used;

  for (i = 0; i < ht->size; i++)
    {
      prev = &result->table[i];
      scan_bucket(ht->table[i], cur)
	{
	  newbucket = ralloc(get_bucket_region(ht), struct bucket_);
	  newbucket->key = cur->key;
	  newbucket->data = cur->data;
	  newbucket->next = NULL;
	  assert(!*prev);
	  *prev = newbucket;
	  prev = &newbucket->next;
	}
    }
  return result;
}

/* Increase size of ht (double it) and reinsert all the elements */
static void rehash(hash_table ht)
{
  unsigned long old_table_size, i;
  bucket *old_table, cur;

#ifdef DEBUG
  printf("Rehash table size=%ld, used=%ld\n", ht->size, ht->used);
#endif

  old_table_size = ht->size;
  old_table = ht->table;

  ht->size = ht->size*2;
  ht->log2size = ht->log2size + 1;
  ht->used = 0;
  ht->table = rarrayalloc(ht->r ? ht->r : BUCKETPTR_REGION, ht->size, bucket);

  for (i = 0; i < old_table_size; i++)
    scan_bucket(old_table[i], cur)
      hash_table_insert(ht, cur->key, cur->data);
}

/* Begin scanning ht */
void hash_table_scan(hash_table ht, hash_table_scanner *hts)
{
  hts->ht = ht;
  hts->i = 0;
  hts->cur = hts->ht->table[0];
}

/* Get next elt in table, storing the elt in *k and *d if k and d are
   non-NULL, respectively.  Returns TRUE if there is a next elt, FALSE
   otherwise. */
bool hash_table_next(hash_table_scanner *hts, hash_key *k, hash_data *d)
{
  while (hts->cur == NULL)
    {
      hts->i++;
      if (hts->i < hts->ht->size)
	hts->cur = hts->ht->table[hts->i];
      else
	break;
    }

  if (hts->i == hts->ht->size)
    {
      return FALSE;
    }
  else
    {
      if (k)
	*k = hts->cur->key;
      if (d)
	*d = hts->cur->data;
      hts->cur = hts->cur->next;
    }
  return TRUE;
}

/* Apply f to all elements of ht, in some arbitrary order */
void hash_table_apply(hash_table ht, hash_apply_fn f, void *arg)
{
  unsigned long i;
  bucket cur;

  for (i = 0; i < ht->size; i++)
    scan_bucket(ht->table[i], cur)
      f(cur->key, cur->data, arg);
}

/* Map f to all elements on ht, creating a new hash table in region r */
hash_table hash_table_map(region r, hash_table ht, hash_map_fn f, void *arg)
{
  unsigned long i;
  hash_table result;
  bucket cur, newbucket, *prev;

  result = make_hash_table_int(r,ht->size, ht->hash, ht->cmp,
			       ht->key_persist_kind,
			       ht->data_persist_kind);
  result->used = ht->used;
  
  for (i = 0; i < ht->size; i++)
    {
      prev = &result->table[i];
      scan_bucket(ht->table[i], cur)
	{
	  newbucket =  ralloc(get_bucket_region(ht), struct bucket_);
	  newbucket->key = cur->key;
	  newbucket->data = f(cur->key, cur->data, arg);
	  newbucket->next = NULL;
	  assert(!*prev);
	  *prev = newbucket;
	  prev = &newbucket->next;
	}
    }
  return result;
}

static keycmp_fn cur_cmp = NULL;

int entry_cmp(const void *a, const void *b)
{
  struct sorted_entry *ae = (struct sorted_entry *) a;
  struct sorted_entry *be = (struct sorted_entry *) b;
  return cur_cmp(ae->k, be->k);
}

/* Begin scanning ht in sorted order according to f */
void hash_table_scan_sorted(hash_table ht, keycmp_fn f,
			    hash_table_scanner_sorted *htss)
{
  hash_table_scanner hts;
  unsigned long i;

  htss->r = newregion();
  htss->size = hash_table_size(ht);
  htss->entries = rarrayalloc(htss->r, htss->size, struct sorted_entry);
  htss->i = 0;

  hash_table_scan(ht, &hts);
  i = 0;
  while (hash_table_next(&hts, &htss->entries[i].k,
			 &htss->entries[i].d))
    i++;
  assert(i == htss->size);
  cur_cmp = f;
  qsort(htss->entries, htss->size, sizeof(struct sorted_entry), entry_cmp);
  cur_cmp = NULL;
}

/* Just like hash_table_next, but scans in sorted order */
bool hash_table_next_sorted(hash_table_scanner_sorted *htss, hash_key *k,
			    hash_data *d)
{
  if (htss->i < htss->size)
    {
      if (k)
	*k = htss->entries[htss->i].k;
      if (d)
	*d = htss->entries[htss->i].d;
      htss->i++;
      return TRUE;
    }
  else
    {
      deleteregion(htss->r);
      htss->r = NULL;
      return FALSE;
    }
}

static unsigned long get_num_buckets(bucket b) {
  bucket cur;
  unsigned long result = 0;
  scan_bucket(b, cur) {
    result++;
  }
  return result;
}

/* Persistence */

bool hash_table_serialize(FILE *f, void *obj)
{
  unsigned long i;
  bucket cur;
  hash_table ht = (hash_table)obj;
  assert(f);
  assert(obj);

  fwrite(&ht->hash, sizeof(void *) * 2 + sizeof(unsigned long) * 3 + 
	 sizeof(int) *4, 1, f);
  
  serialize_object(ht->hash, 1);
  serialize_object(ht->cmp, 1);

  /* Write out all the key/value pairs */
  for (i = 0; i < ht->size; i++) {
    unsigned long num_buckets = get_num_buckets(ht->table[i]);
    fwrite(&num_buckets, sizeof(unsigned long), 1, f); 
    scan_bucket(ht->table[i], cur) {
      fwrite(&cur->key, sizeof(hash_key) + sizeof(hash_data), 1, f);
      serialize_object(cur->key,ht->key_persist_kind);
      serialize_object(cur->data, ht->data_persist_kind);
    }
  }
  return TRUE;
}

extern region persist_rgn;

void *hash_table_deserialize(FILE *f)
{
  hash_table ht = NULL;
  unsigned long i;
  bucket newbucket, *prev;

  assert(f);

  ht = ralloc(table_region, struct Hash_table);
  ht->r = permanent;
  ht->hash = NULL;
  ht->cmp = NULL;

  fread(&ht->hash, sizeof(void *) * 2 + sizeof(unsigned long) * 3 + 4 * sizeof(int), 1, f);

  /* Read all the key/value pairs into the temporary region */
  ht->table = rarrayalloc(ht->r, ht->size, bucket);
  for (i = 0; i < ht->size; i++) {
    unsigned long num_buckets,j;
    fread(&num_buckets, sizeof(unsigned long), 1, f); 
    prev = &ht->table[i];
    for (j = 0; j < num_buckets; j++) {
      newbucket = ralloc(ht->r, struct bucket_);
      fread(&newbucket->key, sizeof(hash_key) + sizeof(hash_data), 1 ,f);
      newbucket->next = NULL;
      assert(!*prev);
      *prev = newbucket;
      prev = &newbucket->next;
    }
  }
  return ht;
}

bool hash_table_set_fields(void *obj)
{
 hash_table ht = (hash_table)obj;
  
 unsigned long i;
 bucket cur;
/*   bucket *oldtable = NULL, cur; */
/*   region oldregion; */
  assert(ht);

  deserialize_set_obj((void **)&ht->hash);
  deserialize_set_obj((void **)&ht->cmp);

/*   oldtable = ht->table; */
/*   oldregion = ht->r; */
/*   ht->used = 0; */
/*   ht->r = permanent; */
/*   ht->table = rarrayalloc(ht->r, ht->size, bucket); */
  
   /* Reinsert all the key/value pairs after they have been remapped */
  if (ht->data_persist_kind) {
    for (i = 0; i < ht->size; i++) {
      scan_bucket(ht->table[i], cur) {
	//deserialize_set_obj((void **)&cur->key);
	deserialize_set_obj((void **)&cur->data);
	//hash_table_insert(ht, cur->key, cur->data);
      }
    }
  }

  return TRUE;
}

/* Update function pointers and all other pointers */
int update_hash_table(translation t, void *m)
{
  hash_table tab = (hash_table)m;
  
  assert(fn_ptr_table);
  //assert(tab->hash_fn_id < num_fn_ptrs);
  //assert(tab->keyeq_fn_id < num_fn_ptrs);
  
  if (tab->hash_fn_id < num_fn_ptrs) {
    tab->hash = update_funptr_data(tab->hash_fn_id);
    hash_table_lookup(fn_ptr_table, (hash_key)tab->hash, 
		      (hash_data*)&tab->hash_fn_id);
  }

  if (tab->keyeq_fn_id < num_fn_ptrs) {
    tab->cmp = update_funptr_data(tab->keyeq_fn_id);
    hash_table_lookup(fn_ptr_table, (hash_key)tab->cmp,
		      (hash_data*)&tab->keyeq_fn_id);
  }
      
  update_pointer(t, (void **) &tab->table);

  return (sizeof(struct Hash_table));
}

int update_bucket(translation t, void *m)
{
  struct bucket_ *b = (struct bucket_ *)m;

  update_pointer(t, (void **) &b->key);
  update_pointer(t, (void **) &b->data);
  update_pointer(t, (void **) &b->next);

/*   assert(b->key); */
/*   assert(b->data); */
  return(sizeof(struct bucket_));
}

int update_strbucket(translation t, void *m)
{
  update_pointer(t, (void **) &((struct bucket_ *) m)->key);
  update_pointer(t, (void **) &((struct bucket_ *) m)->next);
  return(sizeof(struct bucket_));
}

int update_keystrbucket(translation t, void *m)
{
  update_pointer(t, (void **) &((struct bucket_ *) m)->data);
  update_pointer(t, (void **) &((struct bucket_ *) m)->next);
  return(sizeof(struct bucket_));
}

int update_bucketptr(translation t, void *m)
{
  update_pointer(t, m);
  return(sizeof(void *));
}

void hash_table_init()
{
  table_region = newregion();
  bucket_region = newregion();
  strbucket_region = newregion();
  bucketptr_region = newregion();
  keystrbucket_region = newregion();
}

/* void hash_table_reset() */
/* { */
/*   deleteregion(table_region); */
/*   deleteregion(bucket_region); */
/*   deleteregion(BUCKETPTR_REGION); */
  
/*   hash_table_init(); */
/* } */
