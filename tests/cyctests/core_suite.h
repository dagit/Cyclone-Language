
/**** core test suite ****/
CT_SUITE_BEGIN(Core)
void begin() { }
void end() { }

void mkthin_test() {
  int ?dyn = new { for i < 10 : (int)i };

  let Core::ThinRes{buf,x} = Core::mkthin(dyn,sizeof(int));
  CT_ASSERT(x==10);
  if (buf != NULL) {
    for (unsigned int i = 0; i<x; i++)
      buf[i] = 1;
  }
  for (int i = 0; i<numelts(dyn); i++)
    CT_ASSERT(dyn[i] == 1);

  int ?dyn2 = NULL;
  let Core::ThinRes{buf2,x2} = Core::mkthin(dyn2,sizeof(int));
  CT_ASSERT(x2 == 0);
}
CT_SUITE_INIT
CT_ADD_TEST(mkthin_test)
CT_SUITE_END



