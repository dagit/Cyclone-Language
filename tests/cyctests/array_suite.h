
/**** array test suite ****/
CT_SUITE_BEGIN(Array)

void begin() {
    // do nothing
}

void end() {
    // do nothing
}

void array_test() {
  // could definitely use more tests
  unsigned int ?x = new { for i < 6 : 2*i }; // Note, not int ?x
  unsigned int ?xp = x+3;
  CT_ASSERT(Core::arr_prevsize(xp,sizeof(*xp)) == 3);
  unsigned char ?c = new { for i < 8 : '\0' };
  unsigned char ?cp = c + 7;
  CT_ASSERT(Core::arr_prevsize((unsigned char ? @nozeroterm)cp,sizeof(*cp)) == 7);
  for (int i=0; i<6; i++)
    CT_ASSERT(x[i]==2*i);
  for (int i=0; i<6; i+=2) {
    let temp = x[i];
    x[i] = x[i+1];
    x[i+1] = temp;
  }
  for (int i=0; i<6; i+=2) {
    CT_ASSERT(x[i] == 2*(i+1));
    CT_ASSERT(x[i+1] == 2*i);
  }
  int ?z1 = new { for i < 10 : (int)(10-i) }; // 10 to 1
  int ?z2 = Array::copy(z1);
  Array::imp_rev(z2);
  int ?z3 = Array::rev_copy(Array::rev_copy(z1));
  // z2 is sorted, z1 and z3 are backwards
  CT_ASSERT(0==array_cmp(int_cmp,z1,z3));
  Array::qsort(intptr_cmp,z1,10);
  CT_ASSERT(0==array_cmp(int_cmp,z1,z2));
  Array::msort(intptr_cmp,z3,10);
  CT_ASSERT(0==array_cmp(int_cmp,z3,z2));
  int x = 4;
  int ?z4 = Array::map(add2,Array::extract(z2,2,&x));
  Array::imp_map(add2,z4);
  CT_ASSERT(0==array_cmp(int_cmp,z4,Array::extract(z2,6,NULL)));
  $(int,int) ?z5 = new {for i < 10 : $((int)(i),(int)(i)) };
  CT_ASSERT(10==numelts(z5));
  $(int ?,int ?) pair = Array::split(z5);
  CT_ASSERT(0==array_cmp(int_cmp,pair[0],pair[1]));
  
}


CT_SUITE_INIT
CT_ADD_TEST(array_test)
CT_SUITE_END

