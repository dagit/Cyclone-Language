#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bget.h"

//#define VERB

#ifndef VERB
#define printf(arg...)
#endif

#define throw(i) {printf("\nTHROW %d:: error allocating", i); exit(-1);}

#define default_region_page_size 512
#define CYC_REGION_PROFILE

struct _ReapPage
{ 
  void *bget_page;
  struct _ReapPage *next;
}
; 

struct _ReapHandle {
  //  struct _RuntimeStack s;
  struct _ReapPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif
  struct _pool *released_ptrs;
  struct bget_region_key *bkey;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

enum AliasQualHandle {
  CYC_CORE_REFCNT_AQUAL,
  CYC_CORE_UNIQUE_AQUAL
};
typedef enum AliasQualHandle _AliasQualHandle_t;


void Cyc_Core_rufree(struct _ReapHandle *r, void *ptr) {
  if(ptr == NULL || r == NULL)
    return;
  brel(r->bkey, ptr);
}

static void* _acquire_reap_page(bget_rgn_key_t key, bufsize s) {
  unsigned int next_size = s+sizeof(struct _ReapPage);
  struct _ReapHandle *r = (struct _ReapHandle *)key->priv;
  if(r->curr == NULL) //no assert :(
    return 0;
  void *p =malloc(next_size);
  if(!p)
    return 0;
  //chain the new page onto this regions list of free pages
  struct _ReapPage *tmp = r->curr;
  r->curr = (struct _ReapPage*)p;
  r->curr->bget_page = (p + sizeof(struct _ReapPage));
  r->curr->next = tmp;
  return r->curr->bget_page;
}

// allocate s bytes within region r
void * _reap_malloc(struct _ReapHandle *r, _AliasQualHandle_t aq, unsigned int s) {
  void *result;
  if(r->curr == NULL) { 
    //grab a page, and hold onto a ref. We need to free it later
    void *p =malloc(default_region_page_size+sizeof(struct _ReapPage));
    if(!p)
      throw(1)
    r->curr = (struct _ReapPage*)p;
    r->curr->bget_page = (p + sizeof(struct _ReapPage));
    r->curr->next = NULL;
    //pass the page to bget
    r->bkey = bget_init_region(r->curr->bget_page, default_region_page_size);
    if(!r->bkey) 
      throw(2)
    r->bkey->priv = r;
    // direct allocation for large buffers is a pain ... revisit
    // no direct allocation means there is no need for a release function
    // no compaction, default size for acquisition request is page_size+
    bectl(r->bkey, NULL, _acquire_reap_page, NULL, default_region_page_size);
  }
  if(aq == CYC_CORE_REFCNT_AQUAL) {
    result = bgetz(r->bkey, (bufsize)(s + sizeof(int)));
    if(!result)
      throw(3)
    *((int*)result) = 1;
    result += sizeof(int);
    return (void*)result;
  }
  else {
    result = bgetz(r->bkey, (bufsize)s);
    if(!result)
      throw(4)
    return (void*)result;
  }
}


struct _ReapHandle _new_reap(const char *rgn_name) {
  struct _ReapHandle r; //don't handle dynamic reaps here yet
#ifdef CYC_REGION_PROFILE
  r.name = rgn_name;
#endif
  r.curr = NULL;
  r.bkey = NULL; //lazy allocation of region pages
#if(defined(__linux__) && defined(__KERNEL__))
  r.vpage = NULL;
#endif   
  r.released_ptrs = NULL;
  //r.offset = ((char *)p) + sizeof(struct _RegionPage);
  //r.last_plus_one = r.offset + default_region_page_size;
  return r;
}


void _free_reap(struct _ReapHandle *r) {
  if(r && r->curr) {
    //notify bget 
    bget_drop_region(r->bkey);
    //free pages
    do {
      struct _ReapPage *next = r->curr->next;
      free(r->curr);
      r->curr = next;
    }while(r->curr);
    /* free autorelease pool */
/*     struct _pool *pl = r->released_ptrs; */
/*     while (pl) { */
/*       int i; */
/*       for (i = 0; i < pl->count; i++) */
/* 	drop_refptr_base(pl->pointers[i]); */
/*       {struct _pool *next = pl->next; */
/*       GC_free(pl); */
/*       pl = next;} */
/*     } */
    r->curr = 0;
  }
}

struct ReapPtrs {
  void *ptr;
  int ix;
  unsigned int size;
  struct ReapPtrs *next;
};
struct Reap {
  int counter;
  struct _ReapHandle h;
  struct ReapPtrs *p;
};

int check_contents(void *p, unsigned int size, unsigned value, unsigned int offset) {
  if(offset >= size)
    return 0;
  unsigned char *uc = (unsigned char*)p;
  int i=0;
  for(i=offset;i<(size-offset);i++)
    if((*(uc+i)) != value) {
      printf("\nCorrupted memory at offset %d -- contents 0x%x", i,*(uc+i));
      return -1;
    }
  return 0;
}

void do_free(struct Reap *r) {
  struct ReapPtrs *p = r->p;
  struct ReapPtrs *first = NULL;
  struct ReapPtrs *prev = NULL;
  struct ReapPtrs *tmp = NULL;
  while(p) {
    if((rand() % 10) < 4) {
      printf("%s: freeing pointer %d (0x%x) of size %d ... ", r->h.name, p->ix, p->ptr, p->size);fflush(stdout);
      if(check_contents(p->ptr, p->size, 0xff,0)) {
	printf("corrupted memory\n");
	exit(-1);
      }
      Cyc_Core_rufree(&r->h, p->ptr);
      //      if(p->size > 8)
      //	*(((unsigned char*)p->ptr) + 8) = 0xff;
      if(check_contents(p->ptr, p->size, 0x55,8)) {
	printf("memory not poisoned\n");
	exit(-1);
      }
      printf("freed\n");
      if(prev) {
	prev->next = p->next;
	free(p);
	p = prev->next;
      }
      else {
	tmp = p->next;
	free(p);
	p = tmp;
      }
    }
    else {
      if(!first)
	first = p;
      prev = p;
      p = p->next;
    }
  }
  r->p = first;
}

void do_alloc(struct Reap *r) {
  int i=0;
  do_free(r);
  for(i=0;i<10;i++) {
    unsigned int size = 512;//pow(2, (rand() % 10));
    if(size < 4)  size = 4;
    void *a = _reap_malloc(&r->h, CYC_CORE_UNIQUE_AQUAL, size);
    if(!a) {
      return;
    }
    memset(a, 0xff, size);
    r->counter++;
    printf("%s: alloc'd pointer %d (0x%x) of %d bytes ...", r->h.name, r->counter, a, size);
    printf("Checking initialization...");fflush(stdout);
    if(check_contents(a, size,0xff,0))
      exit(-1);
    printf("ok\n");
    struct ReapPtrs *n = (struct ReapPtrs*)malloc(sizeof(struct ReapPtrs));
    n->ptr = a;
    n->next = r->p;
    n->ix = r->counter;
    n->size = size;
    r->p = n;
  }
}

int main() {
  struct Reap r[10];
  char names[10][10];
  int i=0, j=0;
  srand(100);
  for(i=0;i<10;i++) {
    sprintf(names[i], "reap%d", i);
    r[i].counter = 0;
    r[i].h = _new_reap(names[i]);
    r[i].p = NULL;
  }
  for(j=0;j<10;j++){
    printf("********************ROUND %d********************\n", j);
    for(i=0;i<10;i++){
      if(((rand() % 10) < 2) && r[i].counter > 20){
	printf("Freeing reap %d!!!!!!!!!!!!!!", i);
	_free_reap(&r[i].h);
	r[i].counter = -1;
      }
      if(r[i].counter >= 0)
	do_alloc(&r[i]);
    }
  }
  fprintf(stdout, "ok\n");
  return 0;
}
