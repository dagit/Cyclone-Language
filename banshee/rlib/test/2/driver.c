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

#define NUM 6000000

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

  for(i = 0; i < NUM; i++)
    p = new_pair(r[0], i, p);

  serialize(r,"data","offsets");
  if (argc > 1)
    printf("Use the following pointer for old p in the timer: %d\n", (unsigned int) p);
  t = deserialize("data", "offsets", u, temp); 
  p2 = (struct pair *) translate_pointer(t, (void *) p);
  
  for(i = 0; i < NUM; i++) {
    if ((p == p2) || (p->i != p2->i)) {
      printf("FAIL\n");
      exit(1);
    }
    p = p->next;
    p2 = p2->next;
  }
  printf("OK\n");
  return 0;
}
