
/**** arg test suite (NOT RUN) ****/
CT_SUITE_BEGIN(Arg)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


using Arg {
static int u_foo_foo = 0;
void u_foo() {
  u_foo_foo++;
}
static int i_foo_foo = 0;
void i_foo(int i) {
  i_foo_foo = i;
}
void r_foo(string_t s) {
  printf("Got %s\n",s);
}
void a_foo(string_t s) {
  printf("Anon %s\n",s);
}
bool n_foo(string_t s) {
  return false;
}
typedef $(string_t,bool,string_t,spec_t,string_t) @arg_t;
void myarg_test(string_t<`H> ?`H args) {
  // NOTE, TO TEST IT GIVE IT SOME ARGS!

  if (numelts(args)<=0) 
    CT_FAIL();
  else if (numelts(args)==1) 
    CT_OK();
  else {
    arg_t u = new $("-u",false,"",(spec_t)(new Unit_spec(u_foo)),"unit");
    let s_foo = new false;
    arg_t s = new $("-s",false,"",(spec_t)(new Set_spec(s_foo)),"set");
    let c_foo = new true;
    arg_t c = new $("-c",false,"",(spec_t)(new Clear_spec(c_foo)),"clear");
    arg_t i = new $("-i",false,"",(spec_t)(new Int_spec(i_foo)),"int");
    arg_t r = new $("-r",false,"",(spec_t)(new Rest_spec(r_foo)),"rest");
    parse(new List(u,new List(s,new List(c,new List(i,new List(r,NULL))))),
          a_foo,
	  n_foo,
          "Usage:",
	  args);
    printf("u_foo_foo=%d, "
           "s_foo=%s, "
           "c_foo=%s, "
           "i_foo=%d\n",
           u_foo_foo,
           (*s_foo)?(string_t)"true":(string_t)"false",
           (*c_foo)?(string_t)"true":(string_t)"false",
           i_foo_foo);
  }

  
}
}

void arg_test() {
  myarg_test(new { "./onearg", "./twoarg"});
}

CT_SUITE_INIT
CT_ADD_TEST(arg_test)
CT_SUITE_END



