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
#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include "bool.h"
#include "linkage.h"
#include "utils.h"
#include "persist.h"

EXTERN_C_BEGIN

typedef void *hash_key;
typedef void *hash_data;

typedef unsigned long (*hash_fn)(hash_key k);    /* Function to hash a key */
typedef bool (*keyeq_fn)(hash_key k1, hash_key k2);
                                         /* Function returning true iff
					     k1 and k2 are equal */
typedef void (*hash_apply_fn)(hash_key k, hash_data d, void *arg);
                                          /* Function applied to
					     elts in the hash table */
typedef hash_data (*hash_map_fn)(hash_key k, hash_data d, void *arg);
                                          /* Function mapped to
					     elts in the hash table */

/* Function for serializing keys to a file */
typedef void (*keywrite_fn)(FILE *f, hash_key k); 

/* Function for deserializing keys from a file */
typedef hash_key (*keyread_fn)(FILE *f);

typedef struct Hash_table *hash_table;

/* Initialize this module */
void hash_table_init();

/* Make a new hash table, with size buckets initially.  Hash table
   elements are allocated in region rhash. */
hash_table make_hash_table(region rhash, unsigned long size, hash_fn hash,
			   keyeq_fn cmp);

hash_table make_persistent_hash_table(unsigned long size, 
				      hash_fn hash, keyeq_fn cmp,
				      int key_persist_kind,
				      int data_persist_kind);

/* Make a hash table for strings. */
hash_table make_string_hash_table(region rhash, unsigned long size);

hash_table make_persistent_string_hash_table(unsigned long size,
					     int data_persist_kind);

/* Zero out ht.  Doesn't reclaim bucket space. */
void hash_table_reset(hash_table ht);

/* Return the number of entries in ht */
unsigned long hash_table_size(hash_table ht);

/* Given an equality predicate function which agrees with our
   hash_function, search for the given element. */
bool hash_table_hash_search(hash_table ht, keyeq_fn cmp, 
			    hash_key k, hash_data *d);

/* Lookup k in ht.  If d is not NULL, returns corresponding data in *d.
   Function result is TRUE if the k was in ht, false otherwise. */
bool hash_table_lookup(hash_table ht, hash_key k, hash_data *d);

/* Add k:d to ht.  If k was already in ht, replace old entry by k:d.
   Rehash if necessary.  Returns TRUE if k was not already in ht. */
bool hash_table_insert(hash_table ht, hash_key k, hash_data d);

/* Remove mapping for k in ht.  Returns TRUE if k was in ht. */
bool hash_table_remove(hash_table ht, hash_key k);

/* Return a copy of ht, allocated in rhash */
hash_table hash_table_copy(region rhash, hash_table ht);

/* Map f to all elements on ht, creating a new hash table */
hash_table hash_table_map(region r, hash_table ht, hash_map_fn f, void *arg);

typedef struct bucket_ *bucket;
typedef struct
{
  hash_table ht;
  unsigned long i;
  bucket cur;
} hash_table_scanner; /* Opaque type!  Do not modify fields. */

/* Begin scanning ht */
void hash_table_scan(hash_table ht, hash_table_scanner *);

/* Get next elt in table, storing the elt in *k and *d if k and d are
   non-NULL, respectively.  Returns TRUE if there is a next elt, FALSE
   otherwise. */
bool hash_table_next(hash_table_scanner *, hash_key *k, hash_data *d);

/* Total order on hash table keys, only uesd for hash_table_scan_sorted */
typedef int (*keycmp_fn)(hash_key k1, hash_key k2);

struct sorted_entry
{
  hash_key k;
  hash_data d;
};

typedef struct
{
  region r;
  unsigned long i;
  unsigned long size;
  struct sorted_entry *entries;
} hash_table_scanner_sorted;

/* Begin scanning ht in sorted order according to f */
void hash_table_scan_sorted(hash_table ht, keycmp_fn f,
			    hash_table_scanner_sorted *htss);

/* Just like hash_table_next, but scans in sorted order */
bool hash_table_next_sorted(hash_table_scanner_sorted *htss, hash_key *k,
			    hash_data *d);


/* Persistence */
bool hash_table_set_fields(void *obj);
void *hash_table_deserialize(FILE *f);
bool hash_table_serialize(FILE *f, void *obj);

/* Region-based persistence */
extern region bucket_region;
extern region table_region;
extern region strbucket_region;
int update_hash_table(translation t, void *m);
int update_bucket(translation t, void *m);
int update_strbucket(translation t, void *m);

EXTERN_C_END

#endif
