

/**** rope test suite ****/
CT_SUITE_BEGIN(Rope)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


void rope_test() {
  Rope::rope_t r = Rope::from_string("hello world*");
  let r2 = Rope::concat(r,r);
  let r3 = Rope::concatl(new List(Rope::from_string("xx"),
                               new List(Rope::from_string("yy"),NULL)));
  CT_ASSERT(Rope::length(r) == strlen("hello world*"));
  CT_ASSERT(Rope::length(r2) == 2*strlen("hello world*"));
  CT_ASSERT(Rope::cmp(r,r2) != 0);
  CT_ASSERT(Rope::cmp(r3,r3) == 0);
  CT_ASSERT(strcmp(Rope::to_string(r),"hello world*")==0);
  CT_ASSERT(strcmp(Rope::to_string(r2),"hello world*hello world*")==0);
  CT_ASSERT(strcmp(Rope::to_string(r3),"xxyy")==0);
  ;
}

CT_SUITE_INIT
CT_ADD_TEST(rope_test)
CT_SUITE_END


