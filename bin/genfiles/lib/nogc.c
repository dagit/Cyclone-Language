#include <stdio.h>

#ifdef CYC_REGION_PROFILE
#undef GC_malloc
#undef GC_malloc_atomic
#undef GC_free
#endif

void GC_free(void *x) {
  free(x);
}

void *GC_malloc(int x) {
  return (void*)malloc(x);
}

void *GC_malloc_atomic(int x) {
  return (void*)malloc(x);
}

unsigned int GC_gc_no = 0;

size_t GC_get_heap_size() {
  return 0;
}

size_t GC_get_free_bytes() {
  return 0;
}

size_t GC_get_total_bytes() {
  return 0;
}
