
/**** string test suite ****/
CT_SUITE_BEGIN(String)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}

void string_test() {
  // UNTESTED:
  // strpbrk
  // strspn
  // expand
  // realloc_str


  CT_ASSERT(strcmp("hello","world") < 0);
  CT_ASSERT(strcmp("hello","hello") == 0);
  CT_ASSERT(strcmp("world","hello") > 0);

  CT_ASSERT(strcmp(substring("hello",2,2),"ll")==0);
  CT_ASSERT(strcmp(replace_suffix("hello.world","world","bar"),
                        "hello.bar")==0);
  // FIX:  polymorphism requires the cast char[sz]->char?
  CT_ASSERT(strcmp(strconcat_l(new List(allocstr("hello "),
                                                  new List(allocstr("world"),NULL))),
                        "hello world")==0);
  CT_ASSERT(strcmp(str_sepstr(new List(allocstr("hello"),
                                                 new List(allocstr("world"),NULL)),
                                           "**"),
                        "hello**world")==0);
  CT_ASSERT(strcmp(implode(list('a','b','c')),"abc")==0);
  switch ((char)explode("abc")->tl->hd) {
  case 'a': CT_FAIL(); break;
  case 'b': CT_OK(); break;
  case 'c': CT_FAIL(); break;
  default: CT_FAIL(); break;
  }

  let s1 = strdup("hello");
  CT_ASSERT(strlen(s1)==5);
  CT_ASSERT(strlen((string_t)s1+2)==3);
  s1[3]='\000'; // Note, this writes over a string constant.
  CT_ASSERT(strlen(s1)==3);
  CT_ASSERT(strlen((mstring_t)s1+2)==1);
  CT_ASSERT(strlen((mstring_t)s1+3)==0);
  CT_ASSERT(strlen((mstring_t)s1+4)==1);
  mstring_t s2 = new { for i < 20 : '\000' };
  CT_ASSERT(strlen(s2)==0);
  strcat(s2,s1);
  CT_ASSERT(strcmp(s2,s1)==0);
  CT_ASSERT(strcmp(s2,"hel")==0);
  strcat(s2,s1);
  CT_ASSERT(strcmp(s2+2,"helhel"+2)==0);
  CT_ASSERT(strcmp(s2+2,"xxlhel"+2)==0);
  strcat(s2,(mstring_t)s1+4);
  CT_ASSERT(strcmp(s2,"helhelo")==0);
  strcpy(s2,"foobar");
  CT_ASSERT(strcmp(s2,"foobar")==0);
  strncpy((char ? @nozeroterm)(s2+1),"foobar"+2,3);
  CT_ASSERT(strcmp(s2,"fobaar")==0);
  CT_ASSERT(strcmp(s2,"xxxfobaar"+3)==0);
  CT_ASSERT(strcmp(strchr((mstring_t)s2+1,'a'),"aar")==0);
  CT_ASSERT(strchr((mstring_t)s2+2,'z')==NULL);
  CT_ASSERT(strcmp(strrchr((mstring_t)s2+1,'a'),"ar")==0);
  CT_ASSERT(strrchr((mstring_t)s2+2,'o')==NULL);
  CT_ASSERT(strncmp((mstring_t)s2+1,"xxxobaax"+3,4)==0);
  CT_ASSERT(strncmp((mstring_t)s2+1,"xxxobaax"+3,5)!=0);
  CT_ASSERT(strncasecmp((mstring_t)s2+1,"xxxObAax"+3,4)==0);
  let s3 = strdup(s2);
  CT_ASSERT(strcmp(s3,s2)==0);

  
}
CT_SUITE_INIT
CT_ADD_TEST(string_test)
CT_SUITE_END


