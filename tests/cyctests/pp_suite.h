
/**** pretty printer test suite ****/
CT_SUITE_BEGIN(PP)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}

//////////////////////////// Pretty-printer test
using PP {
datatype term {
  Var(string_t),
  App(datatype term @,datatype term @),
  Lam(string_t,datatype term @)
};
bool is_lam(datatype term @ t) {
  switch (t) {
  case &Lam(x,t1): return true;
  default: return false;
  }
}
doc_t pp_term(datatype term @ t) {
  switch (t) {
  case &Var(x):
    return text(x);
  case &App(t1,t2):
    if (is_lam(t1))
      return cat(text("("),pp_term(t1),text(")"),text("("),pp_term(t2),text(")"));
      else return cat(pp_term(t1),text("("),pp_term(t2),text(")"));
  case &Lam(x,t1):
    return cat(text("\\"),text(x),text("."),pp_term(t1));
  }
}
void pp_test() {
  let x = new Var("x");
  let y = new Var("y");
  let z = new Var("z");
  let t1 = new Lam("x",new App(x,x));
  let t2 = new App(t1,t1);
  CT_ASSERT(0==strcmp("\\x.x(x)",string_of_doc(pp_term(t1),72)));
  CT_ASSERT(0==strcmp("(\\x.x(x))(\\x.x(x))",
                           string_of_doc(pp_term(t2),72)));
  
}
}

CT_SUITE_INIT
CT_ADD_TEST(pp_test)
CT_SUITE_END


