
/**** float test suite ****/
CT_SUITE_BEGIN(Float)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


void float_test() {
  // FIX: parser is having trouble with floating point constants
  float x;
  float y;
  x = (float)2;
  y = (float)4;
  CT_ASSERT(y==x*x);
  CT_ASSERT(y==x+x);
  CT_ASSERT(y!=3*x);
  CT_ASSERT(y!=y+x);
  CT_ASSERT(x==y-x);
  CT_ASSERT(x==y/x);
  
}
CT_SUITE_INIT
CT_ADD_TEST(float_test)
CT_SUITE_END



