
/**** attribute test suite ****/
CT_SUITE_BEGIN(Attributes)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


// test out attribute stuff
  int att_f(int g(int) __attribute__((stdcall)) , int i) {
    return g(i);
  }

  int __attribute__((__stdcall__)) att_foo(int i) {
    return i+1;
  }

  short __attribute__((aligned(4))) att_x = 0;

  struct AttFoo {
    short x __attribute__((aligned(4)));
    short y;
  } __attribute__((packed));

  void att_test() {
//    CT_ASSERT(_sys_nerr == __CYCLONE_SYS_NUM_ERR);
    CT_ASSERT(att_f(att_foo,3) == 4);
    
  }
CT_SUITE_INIT
CT_ADD_TEST(att_test)
CT_SUITE_END
  
