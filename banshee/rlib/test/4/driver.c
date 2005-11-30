#include "../../regions.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct pair {
  int i;
  struct pair *next;
};


struct triple {
  int i;
  int j;
  struct triple *next;
};

struct quint {
  int i;
  int j;
  int k;
  int l;
  struct quint *next;
};

int update_pair(translation t, void *m) {
  update_pointer(t, (void **) &((struct pair *) m)->next);
  return(sizeof(struct pair));
}

int update_triple(translation t, void *m) {
  update_pointer(t, (void **) &((struct triple *) m)->next);
  return(sizeof(struct triple));
}

int update_quint(translation t, void *m) {
  update_pointer(t, (void **) &((struct quint *) m)->next);
  return(sizeof(struct quint));
}

struct pair *new_pair(region r, int i, struct pair *prev) {
  struct pair *p = (struct pair *) rstralloc(r, sizeof(struct pair));
  p->i = i;
  p->next = prev;
  return p;
}

struct triple *new_triple(region r, int i, struct triple *prev) {
  struct triple *p = (struct triple *) rstralloc(r, sizeof(struct triple));
  p->i = i;
  p->j = i;
  p->next = prev;
  return p;
}

struct quint *new_quint(region r, int i, struct quint *prev) {
  struct quint *p = (struct quint *) rstralloc(r, sizeof(struct quint));
  p->i = i;
  p->next = prev;
  return p;
}

#define NUM 50
int region_main(int argc, char *argv[]) {
  region r[4];
  Updater u[3];
  region quint = newregion();
  region temp = newregion();
  int i = 0;
  struct quint *q = NULL, *q2;
  translation tr;

  r[0] = quint; r[1] = NULL;
  u[0] = update_quint;

  for(i = 0; i < NUM; i++) {
    q = new_quint(quint, i, q);
  }

  serialize(r,"data","offsets");
  tr = deserialize("data", "offsets", u, temp); 
  q2 = (struct quint *) translate_pointer(tr, (void *) q);
  
  for(i = 0; i < NUM; i++) {
    if ((q == q2) || (q->i != q2->i)) {
      printf("FAIL triple\n");
      exit(1);
    }
    q = q->next;
    q2 = q2->next;
  }

  printf("OK\n");
  return 0;
}
