

/**** xarray test suite ****/
CT_SUITE_BEGIN(Xarray)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}

void xarray_test() {
  let xa = Xarray::create(3,(int)'a');
  Xarray::add(xa,'a');
  Xarray::add(xa,'b');
  Xarray::add(xa,'c');
  CT_ASSERT(Xarray::get(xa,0)=='a');
  CT_ASSERT(Xarray::get(xa,1)=='b');
  CT_ASSERT(Xarray::get(xa,2)=='c');
  try { Xarray::get(xa,3); CT_FAIL(); } catch { case &Core::Invalid_argument(x): CT_OK(); break; }
  Xarray::set(xa,1,'B');
  CT_ASSERT(Xarray::get(xa,1)=='B');
  Xarray::add(xa,'d');
  CT_ASSERT(Xarray::get(xa,3)=='d');
  try { Xarray::get(xa,4); CT_FAIL(); } catch { case &Core::Invalid_argument(x): CT_OK(); break; }
  ;
}
CT_SUITE_INIT
CT_ADD_TEST(xarray_test)
CT_SUITE_END

