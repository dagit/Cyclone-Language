
/**** signals test suite  ****/
CT_SUITE_BEGIN(Signals)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


static int sigcnt = 0;
void sig_handler(int sig) {
  switch (sigcnt) {
  case 0:
    CT_ASSERT(sig == SIGUSR1); break;
  case 1:
    CT_ASSERT(sig == SIGUSR2); break;
  default:
    CT_ASSERT(0 == 1);
  }
  sigcnt++;
}

void sig_test() {
#ifdef __linux__
  signal(SIGUSR1,sig_handler);
  signal(SIGUSR2,sig_handler);
  raise(SIGUSR1);
  CT_ASSERT(sigcnt == 1);
  raise(SIGUSR2);
  CT_ASSERT(sigcnt == 2);
  
#endif
}

CT_SUITE_INIT
CT_ADD_TEST(sig_test)
CT_SUITE_END



