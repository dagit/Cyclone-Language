

/**** pattern matching test suite ****/
CT_SUITE_BEGIN(Patterns)

void begin() {
    // do nothing
}

void end() {
    // do nothing
}

void pattern_match_test() {
  // char patterns
  switch ((char)('a'+2)) { // Note, cast is needed to typecheck
  case 'b': CT_FAIL(); break;
  case 'c': CT_OK(); break;
  case 'd': CT_FAIL(); break;
  default: CT_FAIL(); break;
  }
  // tuple patterns
  switch ($(1,2,3)) {
  case $(1,3,2): CT_FAIL(); break;
  case $(1,2,3): CT_OK(); break;
  case $(2,1,3): CT_FAIL(); break;
  default: CT_FAIL(); break;
  }
  // nested tuple patterns
  switch ($($(1,2),3)) {
  case $($(1,3),2): CT_FAIL(); break;
  case $($(1,2),3): CT_OK(); break;
  case $($(2,1),3): CT_FAIL(); break;
  default: CT_FAIL(); break;
  }
  // & patterns
  switch ($(new $(1,2),3)) {
  case $(&$(1,3),2): CT_FAIL(); break;
  case $(&$(1,2),3): CT_OK(); break;
  case $(&$(2,1),3): CT_FAIL(); break;
  default: CT_FAIL(); break;
  }
  // && patterns
  switch ($(1,2,3)) {
  case $(1,x,3) && x<2: CT_FAIL(); break;
  case $(1,x,3) && x==2: CT_OK(); break;
  case $(1,x,3) && x>2: CT_FAIL(); break;
  default: CT_FAIL(); break;
  }
  ;
}
CT_SUITE_INIT
CT_ADD_TEST(pattern_match_test)
CT_SUITE_END

