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
#include <assert.h>
#include "ufind.h"
#include "list.h"
#include "persist.h"

enum uf_kind {uf_ecr = 42,uf_link = 43};
typedef enum uf_kind uf_kind;

DECLARE_LIST(elt_stack, uf_element);
DEFINE_LIST(elt_stack, uf_element);

typedef struct ustack_elt_ {
  uf_element nonroot;
  void *old_info;
  int persist_kind;
} *ustack_elt;

DECLARE_LIST(union_stack,ustack_elt);
DEFINE_LIST(union_stack,ustack_elt);

struct uf_element_ {
  uf_kind kind;
  int rank;
  int persist_kind;
  void *info;
  struct uf_element_ *link;
  elt_stack elt_stack;
};

region uf_element_region;
region ustack_element_region;

static union_stack ustack = NULL;

struct uf_element_ *new_uf_element(region r, void *info, int persist_kind)
{
  struct uf_element_ *result;

  result = ralloc(uf_element_region, struct uf_element_);

  result->kind = uf_ecr;
  result->rank = 0;
  result->info = info;
  result->link = NULL;
  result->elt_stack = NULL;
  result->persist_kind = persist_kind;

  return result;
}

static struct uf_element_ *find(struct uf_element_ *e)
{

  if (e->kind == uf_ecr)
    return e;
  
  else if (e->link->kind == uf_link)
    {
      struct uf_element_ *temp = e->link;
	
      e->link = temp->link;
      
      assert(temp->elt_stack);
      elt_stack_cons(e,temp->elt_stack);

      return find(temp);
    }

  else
    return e->link;
}

static ustack_elt make_ustack_elt(uf_element e,void *info)
{
  ustack_elt result = ralloc(ustack_element_region, struct ustack_elt_);
  result->nonroot = e;
  result->old_info = info;
  result->persist_kind = e->persist_kind;

  return result;
}

bool uf_union(struct uf_element_ *a, struct uf_element_ *b)
{
  struct uf_element_ *e1 = find(a);
  struct uf_element_ *e2 = find(b);

  assert(ustack);

  if ( e1 == e2 )
    return FALSE;

 else if (e1->rank < e2->rank)
    {
      ustack_elt ue = make_ustack_elt(e1,e2->info);
      e1->kind = uf_link;
      e1->link = e2;

      union_stack_cons(ue, ustack);
      assert(e1->elt_stack == NULL);
      e1->elt_stack = new_persistent_elt_stack();

      return TRUE;
    }

  else if (e1->rank > e2->rank)
    {
      ustack_elt ue = make_ustack_elt(e2,e1->info);
      e2->kind = uf_link;
      e2->link = e1;

      union_stack_cons(ue, ustack);
      assert(e2->elt_stack == NULL);
      e2->elt_stack = new_persistent_elt_stack();

      return TRUE;
    }
  
  else 
    {
      ustack_elt ue = make_ustack_elt(e1,e2->info);
      e2->rank++;
      
      e1->kind = uf_link;
      e1->link = e2;

      union_stack_cons(ue, ustack);
      assert(e1->elt_stack == NULL);
      e1->elt_stack = new_persistent_elt_stack();

      return TRUE;
    }

}

bool uf_unify(combine_fn_ptr combine,
	      struct uf_element_ *a, struct uf_element_ *b)
{
  struct uf_element_ *e1 = find(a);
  struct uf_element_ *e2 = find(b);

  assert(ustack);

  if ( e1 == e2 )
    return FALSE;

  else if (e1->rank < e2->rank)
    {
      ustack_elt ue = make_ustack_elt(e1,e2->info);
      e2->info = combine(e2->info,e1->info);
      e1->kind = uf_link;
      e1->link = e2;
     
      union_stack_cons(ue, ustack);
      assert(e1->elt_stack == NULL);
      e1->elt_stack = new_persistent_elt_stack();


      return TRUE;
    }

  else if (e1->rank > e2->rank)
    {
      ustack_elt ue = make_ustack_elt(e2,e1->info);
      e1->info = combine(e1->info,e2->info);
      e2->kind = uf_link;
      e2->link = e1;

      union_stack_cons(ue, ustack);
      assert(e2->elt_stack == NULL);
      e2->elt_stack = new_persistent_elt_stack();

      return TRUE;
    }
  
  else 
    {      
      ustack_elt ue = make_ustack_elt(e1,e2->info);
      e2->info = combine(e2->info, e1->info);

      e2->rank++;
      e1->kind = uf_link;
      e1->link = e2;

      union_stack_cons(ue, ustack);
      assert(e1->elt_stack == NULL);
      e1->elt_stack = new_persistent_elt_stack();

      return TRUE;
    }
}

void *uf_get_info(struct uf_element_ *e)
{
  return find(e)->info;
}


bool uf_eq(struct uf_element_ *e1,struct uf_element_ *e2)
{
  return (find(e1) == find(e2));
}

void uf_update(struct uf_element_ *e,uf_info i)
{
  ustack_elt ue;
  e = find(e);
  ue = make_ustack_elt(e,e->info);
  union_stack_cons(ue,ustack);
  assert(e->elt_stack == NULL);

  e->info = i;
}

static void repair_elt_stack(struct uf_element_ *nonroot)
{
  elt_stack_scanner scan;
  uf_element temp;

  assert(nonroot->elt_stack);

  elt_stack_scan(nonroot->elt_stack,&scan);

  while(elt_stack_next(&scan,&temp)) {
    assert(temp->kind == uf_link);
    temp->link = nonroot;
  }
  nonroot->elt_stack = NULL;
}

/* Undo the last union operation */
static bool uf_backtrack_one()
{
  ustack_elt ue = NULL;

  if (union_stack_length(ustack) == 0) return FALSE;

  /* Pop the last unioned elt off the stack */
  ue = union_stack_head(ustack);
  union_stack_tail(ustack);

  if (ue == NULL) return FALSE;
  
  /* This happens when the last operation was an update */
  if (ue->nonroot->kind == uf_ecr) {
    /* Just roll back the old ecr's info */
    ue->nonroot->info = ue->old_info;
  }
  else {
    /* Make sure it's a link */
    //printf("kind is %d, uf_link is %d, uf_ecr is %d\n",ue->nonroot->kind, uf_link, uf_ecr);
    assert(ue->nonroot->kind == uf_link);
    
    /* Roll back the old ecr's info */
    find(ue->nonroot)->info = ue->old_info;
    
    /* Deunion */
    ue->nonroot->kind = uf_ecr;
    ue->nonroot->link = NULL;
    
    /* Repair the element stack */
    repair_elt_stack(ue->nonroot);
  }
  
  return TRUE;
}

void uf_rollback()
{
  while (uf_backtrack_one());
}

void uf_backtrack()
{
  uf_backtrack_one();
}

void uf_tick()
{
  union_stack_cons(NULL,ustack);
}

void uf_init()
{
  uf_element_region = newregion();
  uf_element_region = newregion();
  ustack_element_region = newregion();
  ustack = new_persistent_union_stack();
}

void uf_reset()
{
  deleteregion(ustack_element_region);
  deleteregion(uf_element_region);

  uf_init();
}

/* Persistence */
bool uf_element_serialize(FILE *f, void *obj)
{
  uf_element elt = (uf_element)obj;

  assert(f);
  assert(obj);
  
  fwrite((void *)&elt->kind, sizeof(int), 1, f);
  fwrite((void *)&elt->rank, sizeof(int), 1, f);
  fwrite((void *)&elt->persist_kind, sizeof(int), 1, f);
  fwrite((void *)&elt->info, sizeof(void *), 1, f);
  fwrite((void *)&elt->link, sizeof(void *), 1, f);
  fwrite((void *)&elt->elt_stack, sizeof(void *), 1, f);

  serialize_object(elt->info, elt->persist_kind);
  serialize_banshee_object(elt->link, uf_element);
  serialize_banshee_object(elt->elt_stack, list);

  return TRUE;
}

void *uf_element_deserialize(FILE *f)
{
  uf_element elt = ralloc(uf_element_region, struct uf_element_);

  fread((void *)&elt->kind, sizeof(int), 1, f);
  fread((void *)&elt->rank, sizeof(int), 1, f);
  fread((void *)&elt->persist_kind, sizeof(int), 1, f);
  fread((void *)&elt->info, sizeof(void *), 1, f);
  fread((void *)&elt->link, sizeof(void *), 1, f);
  fread((void *)&elt->elt_stack, sizeof(void *), 1, f);

  return elt;
}

bool uf_element_set_fields(void *obj)
{
  uf_element elt = (uf_element)obj;

  deserialize_set_obj((void **)&elt->info);
  deserialize_set_obj((void **)&elt->link);
  deserialize_set_obj((void **)&elt->elt_stack);

  return TRUE;
}

bool ustack_elt_serialize(FILE *f, void *obj)
{
  ustack_elt elt = (ustack_elt)obj;
  
  assert(f);
  assert(elt);

  fwrite((void *)&elt->nonroot, sizeof(uf_element), 1, f);
  fwrite((void *)&elt->old_info, sizeof(void *), 1, f);
  fwrite((void *)&elt->persist_kind, sizeof(int), 1, f);

  serialize_banshee_object(elt->nonroot, uf_element);
  serialize_object(elt->old_info, elt->persist_kind);

  return TRUE;
}

void *ustack_elt_deserialize(FILE *f)
{
  ustack_elt elt = ralloc(ustack_element_region, struct ustack_elt_);
  
  fread((void *)&elt->nonroot, sizeof(uf_element), 1, f);
  fread((void *)&elt->old_info, sizeof(void *), 1, f);
  fread((void *)&elt->persist_kind, sizeof(int), 1, f);
  
  return elt;
}

bool ustack_elt_set_fields(void *obj)
{
  ustack_elt elt = (ustack_elt)obj;
  deserialize_set_obj((void **)&elt->nonroot);
  deserialize_set_obj((void **)&elt->old_info);

  return TRUE;
}

void uf_serialize(FILE *f)
{
  assert(f);
  fwrite((void *)&ustack, sizeof(union_stack), 1, f);
  serialize_banshee_object(ustack, list);
}

void uf_deserialize(FILE *f)
{
  assert(f);
  fread((void *)&ustack, sizeof(union_stack), 1, f);
}

void uf_set_fields()
{
  deserialize_set_obj((void **)&ustack);
}

void write_module_uf(FILE *f)
{
  fwrite((void *)&ustack, sizeof(union_stack), 1, f);
  return;
}

void update_module_uf(translation t, FILE *f)
{
  fread((void *)&ustack, sizeof(union_stack), 1, f);
  update_pointer(t, (void **)&ustack);
}

int update_ustack_element(translation t, void *m)
{
  update_pointer(t, (void **)&((ustack_elt)m)->nonroot);
  update_pointer(t, (void **)&((ustack_elt)m)->old_info);

  return sizeof(struct ustack_elt_);
}

int update_uf_element(translation t, void *m)
{
  uf_element e = (uf_element)m;

  if (e->persist_kind) update_pointer(t, (void **)&e->info);
  
  update_pointer(t, (void **)&e->link);
  update_pointer(t, (void **)&e->elt_stack);

  return sizeof(struct uf_element_);
}

