

/**** tuple test suite ****/
CT_SUITE_BEGIN(Tuple)

void begin() {
    // do nothing
}

void end() {
    // do nothing
}


void tuple_test() {
  let x = $(1,2);
  let $(y,z) = x;
  CT_ASSERT(y==1);
  CT_ASSERT(z==2);
  x[0]=-3;
  x[1]=9;
  let $(y2,z2) = x;
  CT_ASSERT(y2==-3);
  CT_ASSERT(z2==9);
  
}

CT_SUITE_INIT
CT_ADD_TEST(tuple_test)
CT_SUITE_END



