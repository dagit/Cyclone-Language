#include "../../regions.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct pair {
  int i;
  struct pair *next;
};


int update_pair(translation t, void *m) {
  UPDATEPOINTER(t, (void **) &((struct pair *) m)->next);
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
  struct pair *p;
  translation t;

  if ((argc == 1) || (strcmp(argv[1],"d") && strcmp(argv[1],"b"))) {
    printf("Usage: time timer {d | b}\nd for deserialization only; b for both deserialization and serialization\n");
    exit(1);
  }

  u[0] = update_pair;
  t = deserialize("data", "offsets", u, temp);
  if (strcmp(argv[1],"b") == 0) {
    p = (struct pair *) translate_pointer(t, (void *) 1125667648);
    r[0] = regionof((void *) p);
    r[1] = NULL;
    serialize(r, "data2", "offsets2"); 
  }
  return 0;
}
