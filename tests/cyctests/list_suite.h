

/**** list test suite ****/
CT_SUITE_BEGIN(List)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}



void list_test() {
  // x = list from 1 to 10, backwards
  list_t<int> x = NULL;
  for (int i=1; i<11; i++) x = new List(i,x);
  // xrev = list from 1 to 10, forwards
  list_t<int> xrev = NULL;
  for (int i=1; i<11; i++) xrev = new List(11-i,xrev);
  CT_ASSERT(0==test_list_cmp(int_cmp,List::rev(x),xrev));

  // xrev2 = [1;1;2;2; ...;10;10]
  list_t<int> xrev2 = NULL;
  for (int i=1; i<11; i++) xrev2 = new List(11-i,new List(11-i,xrev2));
  // test mergesort and append
  CT_ASSERT(0==test_list_cmp(int_cmp,xrev2,
                     List::merge_sort(int_cmp,
                                      List::append(x,x))));
  CT_ASSERT(0==test_list_cmp(int_cmp,xrev2,
                     List::merge_sort(int_cmp,
                                      List::append(List::filter(even,xrev2),
                                                   List::filter(odd,xrev2)))));

  // increment x in place
  for (list_t<int> y = x; y != NULL; y=y->tl)
    y->hd = y->hd + 1;
  // xplus = list from 2 to 11, backwards
  list_t<int> xplus = NULL;
  for (int i=1; i<11; i++) xplus = new List(i+1,xplus);
  CT_ASSERT(0==test_list_cmp(int_cmp,x,xplus));

  // FIX:  casts are required for typechecking.  Should string constants
  // have type char? or char[sz] ??
  let slist = new List(allocstr("hello"),new List(allocstr("world"),NULL));
  let rlist = List::rev(slist);
  CT_ASSERT(0==test_list_cmp(strptrcmp,slist,List::rev(rlist)));

  list_t<int> xx = NULL;
  list_t<int> yy = NULL;
  // xx = list from 1 to 10, backwards
  for (int i=1; i<11; i++) xx = new List(i,xx);
  // yy = list of 10 11's
  for (int i=1; i<11; i++) yy = new List(11,yy);
  try {
    CT_ASSERT(0==test_list_cmp(int_cmp,yy,
                       List::map2(my_add,xx,List::rev(xx))));
  }
  catch {
  case &List_mismatch:
    CT_FAIL();
    break;
  }
  try {
    List::map2(my_add,xx,List::append(xx,xx));
    CT_FAIL();
  }
  catch {
  case &List_mismatch:
    CT_OK();
    break;
  }
  
}
CT_SUITE_INIT
CT_ADD_TEST(list_test)
CT_SUITE_END



