

/**** dict test suite ****/
CT_SUITE_BEGIN(Dict)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


void pr_binding(int i, int j) {
  printf("%d -> %d; ",i,j);
}

void dict_test() {
  dict_t<int,int> x = Dict::empty(int_cmp);
  x = Dict::insert(x,34,68);
  x = Dict::insert(x,1,2);
  x = Dict::insert(x,5,10);
  x = Dict::insert(x,-2,-4);
  CT_ASSERT(Dict::lookup(x,1)==2);
  CT_ASSERT(Dict::lookup(x,5)==10);
  CT_ASSERT(Dict::lookup(x,34)==68);
  CT_ASSERT(Dict::lookup(x,-2)==-4);
  ;
}

CT_SUITE_INIT
CT_ADD_TEST(dict_test)
CT_SUITE_END



