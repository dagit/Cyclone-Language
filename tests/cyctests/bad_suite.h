
/**** bad test suite ****/
CT_SUITE_BEGIN(bad_suite)


void begin() {
    // do nothing
}

void end() {
    // do nothing
}

// THIS DOESN'T WORK!!
string_t bad_strrchr(string_t<`H> s, char c) {
  int len = strlen(s);

  for (int i = len-1; i>=0; i--) {
    if (s[i] == c)
      return &s[i];
  }
  return NULL;
}
void bad_test() {
  let s2 = "fobaar";
  printf("\nDa  IN string is %s\n",s2+1);
  printf("Da OUT string is %s\n",bad_strrchr(s2+1,'a'));
  CT_ASSERT(strcmp(bad_strrchr(s2+1,'a'),"ar")==0);
  
}



CT_SUITE_INIT
CT_ADD_TEST(bad_test)
CT_SUITE_END

