#include <stdio.h>

static int gc_malloc_calls = 0;
static int gc_malloc_total = 0;
static int gc_malloc_atomic_calls = 0;
static int gc_malloc_atomic_total = 0;

void GC_free(void *x) {
  return;
}

void *GC_malloc(int x) {
  gc_malloc_calls++;
  gc_malloc_total += x;
  return (void*)malloc(x);
}

void *GC_malloc_atomic(int x) {
  gc_malloc_atomic_calls++;
  gc_malloc_atomic_total += x;
  return (void*)malloc(x);
}

static char *plural(int x) {
  if (x==1) return "";
  else return "s";
}
#define PLURAL(x) x,plural(x)

void nogc_report() {
  fprintf(stderr,"GC_malloc called %d time%s, allocated %d byte%s\n",
          PLURAL(gc_malloc_calls),
          PLURAL(gc_malloc_total));
  fprintf(stderr,"GC_malloc_atomic called %d time%s, allocated %d byte%s\n",
          PLURAL(gc_malloc_atomic_calls),
          PLURAL(gc_malloc_atomic_total));
}
