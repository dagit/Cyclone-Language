
/**** union test suite ****/
CT_SUITE_BEGIN(Swap)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}

void swap_test() {
  // ints
  int i1 = 1;
  int i2 = 2;
  i1 :=: i2;
  CT_ASSERT (i1 == 2);
  CT_ASSERT (i2 == 1);
  // thin pointers
  int *x = new 1;
  int *y = new 2;
  x :=: y;
  CT_ASSERT (*x == 2);
  CT_ASSERT (*y == 1);
  // fat pointers
  int ?w = new 1;
  int ?z = new 2;
  w :=: z;
  CT_ASSERT (*w == 2);
  CT_ASSERT (*z == 1);
  // struct pointers
  struct f { int *x; } *pf = new f(new 1);
  pf->x :=: x;
  CT_ASSERT (*x == 1);
  CT_ASSERT (*pf->x == 2);  
}

CT_SUITE_INIT
CT_ADD_TEST(swap_test)
CT_SUITE_END


