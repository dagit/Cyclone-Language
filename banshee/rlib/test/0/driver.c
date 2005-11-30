#include "../../regions.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

char *new_string(char *src, region r) {
  char *dest = (char *) rstralloc(r, strlen(src)+1);
  strcpy(dest, src);
  return dest;
}

int region_main(int argc, char *argv[]) {
  region r[2],temp;
  char *s1, *s2;
  translation t;
  char *test1 = "This is a test.";
  char *test2 = "This is b test.";

  r[0] = newregion();
  r[1] = NULL;
  
  s1 = new_string(test1,r[0]);
  s2 = new_string(test2,r[0]);

  serialize(r,"data","offsets");

  temp = newregion();
  t = deserialize("data", "offsets", NULL, temp);
  update_pointer(t, (void **) &s1);
  update_pointer(t, (void **) &s2);
  if ((strcmp(s1,test1) != 0) || (strcmp(s2,test2) != 0)) {
    printf("s1 = %s\ns2 = %s\n",s1,s2); 
    printf("FAIL\n");
  }
  else
    printf("OK\n");
  return 0;
}
