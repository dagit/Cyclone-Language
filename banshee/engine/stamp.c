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
#include <limits.h>
#include "stamp.h"
#include "utils.h"
#include "hash.h"
#include "list.h"
#include "banshee.h"
#include "banshee_persist_kinds.h"

#define INITIAL_SIZE 32
#define INITIAL1 0
#define INITIAL2 2000
#define INITIAL3 536870911
//#define MIN -1073741824 /* -2^30 */
//#define MAX  1073741824 /* 2^30 */

static hash_table str_hash;

static int count1 = INITIAL1 + LARGEST_BUILTIN_TYPE, count2 = INITIAL2, count3 = INITIAL3;
static int bounds1 = INT_MIN, bounds2 = 536870911, bounds3 = INT_MAX;

DEFINE_NONPTR_LIST(stamp_list,stamp);

static inline stamp check1(int i)
{
  if (i <= bounds1)
    fail ("Unable to create stamp with small index\n");
  return i;
}

static inline stamp check2(int i)
{
  if (i > bounds2)
    fail ("Unable to create a stamp with regular index\n");
  return i;
}

static inline stamp check3(int i)
{
  if (i >= bounds3)
    fail ("Unable to create a stamp with large index\n");
  return i;
}

stamp stamp_fresh(void)
{
  return (check2(++count2));
}

stamp stamp_fresh_small(void)
{
  return (check1(--count1));
}

stamp stamp_fresh_large(void)
{
  return (check3(++count3));
}

stamp stamp_string(const char *str) deletes
{
  int st;
  assert(str_hash != NULL);

  if (! hash_table_lookup(str_hash,(hash_key)str, (hash_data *) &st))
    {
      st = stamp_fresh();
      (void)hash_table_insert(str_hash,(hash_key)str,(hash_data) st);
    }
  return st;
}

void stamp_reset(void) 
{
  stamp_init();
}

void stamp_init(void)
{
  count1 = INITIAL1;
  count2 = INITIAL2;
  count3 = INITIAL3;
  str_hash = make_persistent_string_hash_table(INITIAL_SIZE,
					       NONPTR_PERSIST_KIND);
}

const char *stamp_to_str(region r,stamp st)
{
  return inttostr(r,st);
}

void stamp_serialize(FILE *f)
{
  int counts[3] = {count1,count2,count3};
  int success = 0;

  success = fwrite((void *)counts,sizeof(int),3,f);
  success += fwrite((void *)&str_hash, sizeof(hash_table), 1, f);

  serialize_banshee_object(str_hash, hash_table);

  if (success != 4) fail("Failed to serialize stamp module\n");
}

void stamp_deserialize(FILE *f)
{
  int counts[3];
  int success = 0;

  success = fread((void *)counts,sizeof(int),3,f);
  success += fread((void *)&str_hash, sizeof(hash_table), 1, f);

  if (success != 4) fail("Failed to deserialize stamp module\n");

  count1 = counts[0];
  count2 = counts[1];
  count3 = counts[2];  
}

void stamp_set_fields(void)
{
  deserialize_set_obj((void **)&str_hash);
}

void write_module_stamp(FILE *f)
{
  int counts[3] = {count1,count2,count3};
  int success = 0;

  success = fwrite((void *)counts,sizeof(int),3,f);
  success += fwrite((void *)&str_hash, sizeof(hash_table), 1, f);

  if (success != 4) fail("Failed to serialize stamp module\n");

  return;
}

void update_module_stamp(translation t, FILE *f)
{
  int counts[3];
  int success = 0;

  success = fread((void *)counts,sizeof(int),3,f);
  success += fread((void *)&str_hash, sizeof(hash_table), 1, f);

  if (success != 4) fail("Failed to deserialize stamp module\n");
  count1 = counts[0];
  count2 = counts[1];
  count3 = counts[2];  

  update_pointer(t, (void **)&str_hash);
}

/* Taken from here: http://www.concentric.net/~Ttwang/tech/inthash.htm  */
unsigned long stamp_hash(hash_key key)
{
  unsigned long keyval = (unsigned long)key;
  keyval += (keyval << 12);
  keyval ^= (keyval >> 22);
  keyval += (keyval << 4);
  keyval ^= (keyval >> 9);
  keyval += (keyval << 10);
  keyval ^= (keyval >> 2);
  keyval += (keyval << 7);
  keyval ^= (keyval >> 12);
  return keyval;
}

bool stamp_eq(hash_key s1, hash_key s2)
{
  return s1 == s2;
}
