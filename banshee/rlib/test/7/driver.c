#include "../../regions.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct pair {
  int i;
  struct pair *next;
};

int update_pair(translation t, void *m) {
  update_pointer(t, (void **) &((struct pair *) m)->next);
  return(sizeof(struct pair));
}

struct pair *new_pair(region r, int i, struct pair *prev) {
  struct pair *p = (struct pair *) rstralloc(r, sizeof(struct pair));
  p->i = i;
  p->next = prev;
  return p;
}

#define NUM 1000
#define ASIZE 1000
int region_main(int argc, char *argv[]) {
  struct pair *ap[NUM], *ap2[NUM];
  region r[2];
  Updater u[1];
  region temp = newregion();
  int i = 0, j = 0;
  translation t;

  r[0] = newregion();
  r[1] = NULL;
  u[0] = update_pair;

  for (j = 0; j < NUM; j++) {
    ap[j] = (struct pair *) rarrayalloc(r[0], ASIZE, struct pair);
    for(i = 0; i < ASIZE; i++) {
      (ap[j]+i)->i = i;
      (ap[j]+i)->next = ap[j]+i;
    }
  }
  
  serialize(r,"data","offsets");
  t = deserialize("data", "offsets", u, temp); 
  for (j = 0; j < NUM; j++)
    ap2[j] = (struct pair *) translate_pointer(t, (void *) ap[j]);
  
  for (j = 0; j < NUM; j++)
    for(i = 0; i < ASIZE; i++) {
    if ((ap[j] == ap2[j]) || ((ap[j]+i)->i != (ap2[j]+i)->i) || (ap2[j]->next - ap[j]->next) != ((ap2[j]+i)->next - (ap[j]+i)->next)) {
      printf("FAIL: iteration %d: %d %d %d \n", i, ap[j] == ap2[j], ((ap[j]+i)->i != (ap2[j]+i)->i), (ap2[j]->next - ap[j]->next) != ((ap2[j]+i)->next - (ap[j]+i)->next));
      exit(1);
    }
  }
  printf("OK\n");
  return 0;
}
