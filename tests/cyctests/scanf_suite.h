
/**** scanf test suite ****/
CT_SUITE_BEGIN(Scanf)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}

// Normally commented out because sscanf is insecure.
// Sometimes uncommented for testing.
void scanf_test() {
  long l = 0;
  int i = 0;
  short s = 0;
  char c = 0;
  unsigned long ul = 0;
  unsigned int ui = 0;
  unsigned short us = 0;
  unsigned char uc = 0;
  double d = 0.0;

  sscanf("10","%d",&i);
  CT_ASSERT(i == 10);
  sscanf("+11","%d",&i);
  CT_ASSERT(i == 11);
  sscanf("-10","%d",&i);
  CT_ASSERT(i == -10);
  sscanf("10","%o",&ui);
  CT_ASSERT(ui == 010);
  sscanf("-10","%o",&ui);
  CT_ASSERT(ui == -010);
  sscanf("+11","%o",&ui);
  CT_ASSERT(ui == 011);
  sscanf("af","%x",&ui);
  CT_ASSERT(ui == 0xaf);
  sscanf("-af","%X",&ui);
  CT_ASSERT(ui == -0xaf);
  sscanf("+fe","%x",&ui);
  CT_ASSERT(ui == 0xfe);
  sscanf("AF","%x",&ui);
  CT_ASSERT(ui == 0xaf);
  sscanf("-AF","%X",&ui);
  CT_ASSERT(ui == -0xaf);
  sscanf("+FE","%x",&ui);
  CT_ASSERT(ui == 0xfe);
  sscanf("%45","%%%i",&i);
  CT_ASSERT(i == 45);

  // Note we have to use %lf etc. for &d !!
  // Printf uses %f for double, but %f means float for scanf.
  sscanf("45.300000","%lf",&d);
  CT_ASSERT(d == 45.3);
  sscanf("54.30","%lf",&d);
  CT_ASSERT(d == 54.3);
  sscanf("5.43","%lg",&d);
  CT_ASSERT(d == 5.43);
  sscanf("4.530000e+01","%le",&d);
  CT_ASSERT(d == 45.3);
  sscanf("XXX  4.53e+02","XXX%le",&d);
  CT_ASSERT(d == 453.0);
  sscanf("XXX  3.53e+01","XXX%le",&d);
  CT_ASSERT(d == 35.3);
  
}
CT_SUITE_INIT
CT_ADD_TEST(scanf_test)
CT_SUITE_END


