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

#define ASIZE 1000000
int region_main(int argc, char *argv[]) {
  region r[2];
  Updater u[1];
  region temp = newregion();
  int i = 0;
  struct pair *p = NULL, *p2;
  translation t;

  r[0] = newregion();
  r[1] = NULL;
  u[0] = update_pair;
  p = (struct pair *) rarrayalloc(r[0], ASIZE, struct pair);
  for(i = 0; i < ASIZE; i++) {
    (p+i)->i = i;
    (p+i)->next = p+i;
  }
  
  serialize(r,"data","offsets");
  t = deserialize("data", "offsets", u, temp); 
  p2 = (struct pair *) translate_pointer(t, (void *) p);
  
  for(i = 0; i < ASIZE; i++) {
    if ((p == p2) || ((p+i)->i != (p2+i)->i) || (p2->next - p->next) != ((p2+i)->next - (p+i)->next)) {
      printf("FAIL: iteration %d: %d %d %d \n", i, p == p2, ((p+i)->i != (p2+i)->i), (p2->next - p->next) != ((p2+i)->next - (p+i)->next));
      printf("(p+i)->i %d    (p2+i)->i %d\n",(p+i)->i, (p2+i)->i); 
      printf("(p2+i)->next - p->next %x    (p2+i)->next - (p+i)->next %x\n",(p2->next - p->next),((p2+i)->next - (p+i)->next));
      exit(1);
    }
  }
  printf("OK\n");
  return 0;
}
