
/**** extern c test suite ****/
CT_SUITE_BEGIN(ExternC)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


// check the functionality of extern "C"
extern "C" double cos(double);

extern "C" {
  double sin(double);
  extern double sinh(double);  // should get a warning here
}

namespace CMath {
  extern "C" double atan(double);
}

namespace MyMath {
  double atan(double x) {
    return CMath::atan(x);
  }
}

void dummy_test() {
    // does nothing
}

CT_SUITE_INIT
CT_ADD_TEST(dummy_test)
CT_SUITE_END

