
/**** printf test suite ****/
CT_SUITE_BEGIN(Printf)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


void printf_test() {
  // test without %s...
  eqstrings("hello",aprintf("hel" "lo"));
  eqstrings("10",aprintf("%d",10));
  eqstrings("+10",aprintf("%+d",10));
  eqstrings("12",aprintf("%i",10+2));
  eqstrings("12 ",aprintf("%-3i",10+2));
  eqstrings("14",aprintf("%o",10+2));
  eqstrings(" c",aprintf("%2x",10+2));
  eqstrings(" C",aprintf("%*X",2,10+2));
  eqstrings("%",aprintf("%%"));
  eqstrings("45.300000",aprintf("%f",45.3));
  eqstrings("45.30",aprintf("%.*f",2,45.3));
  eqstrings("45.3",aprintf("%g",45.3));
  eqstrings("4.530000e+01",aprintf("%e",45.3));
  eqstrings("XXX  4.53e+01",aprintf("XXX%*.2e",10,45.3));
  eqstrings("XXX  4.53e+01",aprintf("XXX%10.*e",2,45.3));
  eqstrings("XXX  4.53e+01",aprintf("XXX%*.*e",10,2,45.3));
  // and with %s, because the code in toc follows those 2 cases
  eqstrings("hellofoo",aprintf("hel" "lo" "%s","foo"));
  eqstrings("10foo",aprintf("%d%s",10,"foo"));
  eqstrings("+10foo",aprintf("%+d%s",10,"foo"));
  eqstrings("12foo",aprintf("%i%s",10+2,"foo"));
  eqstrings("12 foo",aprintf("%-3i%s",10+2,"foo"));
  eqstrings("foo14",aprintf("%s%o","foo",10+2));
  eqstrings(" cfoo",aprintf("%2x%s",10+2,"foo"));
  eqstrings("foo C",aprintf("%s%*X","foo",2,10+2));
  eqstrings("%foo",aprintf("%%%s","foo"));
  eqstrings("foo45.300000",aprintf("%s%f","foo",45.3));
  eqstrings("foo45.30",aprintf("%s%.*f","foo",2,45.3));
  eqstrings("foo45.3",aprintf("%s%g","foo",45.3));
  eqstrings("foo4.530000e+01",aprintf("%s%e","foo",45.3));
  eqstrings("XXX  4.53e+01foo",aprintf("XXX%*.2e%s",10,45.3,"foo"));
  eqstrings("XXX  4.53e+01foo",aprintf("XXX%10.*e%s",2,45.3,"foo"));
  eqstrings("XXX  4.53e+01foo",aprintf("XXX%*.*e%s",10,2,45.3,"foo"));
  
}

CT_SUITE_INIT
CT_ADD_TEST(printf_test)
CT_SUITE_END





