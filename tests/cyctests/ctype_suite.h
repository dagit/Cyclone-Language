
/**** ctype test suite ****/
CT_SUITE_BEGIN(Ctype)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


void ctype_test() {
  CT_ASSERT(isupper('A'));
  CT_ASSERT(!isupper('a'));
  CT_ASSERT(!isupper('9'));
  CT_ASSERT(!isupper('\000'));
  CT_ASSERT(!islower('A'));
  CT_ASSERT(islower('a'));
  CT_ASSERT(!islower('9'));
  CT_ASSERT(!islower('\000'));
  CT_ASSERT(!isdigit('A'));
  CT_ASSERT(!isdigit('a'));
  CT_ASSERT(isdigit('9'));
  CT_ASSERT(!isdigit('\000'));
  CT_ASSERT(isxdigit('A'));
  CT_ASSERT(isxdigit('a'));
  CT_ASSERT(!isxdigit('h'));
  CT_ASSERT(isxdigit('9'));
  CT_ASSERT(!isxdigit('\000'));
  CT_ASSERT(isspace(' '));
  CT_ASSERT(!isspace('9'));
  CT_ASSERT(!isspace('\000'));
  CT_ASSERT(isalpha('A'));
  CT_ASSERT(isalpha('a'));
  CT_ASSERT(isalpha('h'));
  CT_ASSERT(!isalpha('9'));
  CT_ASSERT(!isalpha('\000'));
  CT_ASSERT(isprint(' '));
  CT_ASSERT(isprint('A'));
  CT_ASSERT(isprint('a'));
  CT_ASSERT(isprint('h'));
  CT_ASSERT(isprint('9'));
  CT_ASSERT(!isprint('\000'));
  CT_ASSERT(!isgraph(' '));
  CT_ASSERT(isgraph('A'));
  CT_ASSERT(isgraph('a'));
  CT_ASSERT(isgraph('h'));
  CT_ASSERT(isgraph('9'));
  CT_ASSERT(!isgraph('\000'));
  CT_ASSERT(toupper('c')=='C');
  CT_ASSERT(toupper('C')=='C');
  CT_ASSERT(toupper('9')=='9');
  CT_ASSERT(tolower('c')=='c');
  CT_ASSERT(tolower('C')=='c');
  CT_ASSERT(tolower('9')=='9');
  ;
}
CT_SUITE_INIT
CT_ADD_TEST(ctype_test)
CT_SUITE_END
