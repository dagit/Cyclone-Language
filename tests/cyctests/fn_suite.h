
/**** fn test suite ****/
CT_SUITE_BEGIN(Fn)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}

static int addn(int env, int arg) {
  return arg + env;
}

static int addpair($(int,int)@ x) {
  return (*x)[0] + (*x)[1];
}

void fn_test() {
  fn_t<int,int> inc = make_fn(addn,1);
  fn_t<int,int> dec = make_fn(addn,-1);
  fn_t<int,int> blah = compose(inc,dec);
  fn_t<$(int,int)@,int> addp = fp2fn(addpair);
  fn_t<int,fn_t<int,int>> addc = curry(addp);
  fn_t<$(int,int)@,int> addu = uncurry(addc);
  CT_ASSERT(apply(inc,0) == 1);
  CT_ASSERT(apply(dec,0) == -1);
  CT_ASSERT(apply(blah,0) == 0);
  CT_ASSERT(apply(addp,new $(1,2)) == 3);
  CT_ASSERT(apply(apply(addc,1),2) == 3);
  CT_ASSERT(apply(addu,new $(1,2)) == 3);
  list_t<int> xs = NULL;
  for (int i = 0; i < 10; i ++)
    xs = new List(i,xs);
  xs = map_fn(apply(addc,2),xs);
  bool okay = true;
  for (int i = 11; xs != NULL; xs = xs->tl, i--) {
    if (xs->hd != i) okay = false;
  }
  CT_ASSERT(okay);
  
}

CT_SUITE_INIT
CT_ADD_TEST(fn_test)
CT_SUITE_END




