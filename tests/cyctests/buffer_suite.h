
/**** buffer test suite  ****/
CT_SUITE_BEGIN(Buffer)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


void buffer_test() {
  Buffer::T b = Buffer::create(5);
  Buffer::add_char(b,'a');
  CT_ASSERT(strcmp(Buffer::contents(b),"a")==0);
  Buffer::add_string(b,"bc");
  CT_ASSERT(strcmp(Buffer::contents(b),"abc")==0);
  Buffer::add_string(b,"defg");
  CT_ASSERT(strcmp(Buffer::contents(b),"abcdefg")==0);
  Buffer::add_substring(b,"efghijkl",3,4);
  CT_ASSERT(strcmp(Buffer::contents(b),"abcdefghijk")==0);
  CT_ASSERT(Buffer::length(b)==11);
  ;
}

CT_SUITE_INIT
CT_ADD_TEST(buffer_test)
CT_SUITE_END
