

/**** set test suite ****/
CT_SUITE_BEGIN(Set)

void begin() {
    // do nothing
}

void end() {
    // do nothing
}


list_t<`a> set_elements(set_t<`a> s) {
  let ans = NULL;
  if(Set::is_empty(s))
    return NULL;
  let a = Set::choose(s);
  Iter::iter_t<_,`set_elements> iterator = Set::make_iter(Core::heap_region,s);
  while(Iter::next(iterator,&a))
    ans = new List(a,ans);
  return List::rev(ans); // we're wrongly assuming Set's implementation!!!
}

void set_test() {
  // UNTESTED:
  // singleton
  // compare
  // fold
  // fold_c
  // app
  // iter
  // iter_c
  // choose

  set_t<int> x = Set::empty(int_cmp);
  CT_ASSERT(Set::is_empty(x));
  x = Set::insert(x,9);
  let x2 = x;
  x = Set::insert(x,11);
  x = Set::insert(x,2);
  x = Set::insert(x,-2);
  CT_ASSERT(Set::member(x,9));
  CT_ASSERT(!Set::member(Set::delete(x,9),9));
  CT_ASSERT(Set::member(x,-2));
  CT_ASSERT(Set::member(Set::delete(x,9),-2));
  CT_ASSERT(!Set::member(x,4));
  CT_ASSERT(test_list_cmp(int_cmp,set_elements(x),
		       new List(-2,new List(2,new List(9,new List(11,NULL))))) == 0);
  CT_ASSERT(Set::subset(x2,x));
  CT_ASSERT(!Set::subset(x,x2));
  CT_ASSERT(Set::subset(x,x));
  CT_ASSERT(!Set::equals(x,x2));
  CT_ASSERT(Set::equals(x,x));
  CT_ASSERT(Set::member(Set::intersect(x2,x),9));
  CT_ASSERT(!Set::member(Set::intersect(x2,x),2));
  CT_ASSERT(Set::member(Set::diff(x,x2),11));
  CT_ASSERT(!Set::member(Set::diff(x,x2),9));
  let x3 = Set::from_list(int_cmp,
                          new List(11,new List(99,NULL)));
  CT_ASSERT(!Set::subset(x3,x));
  CT_ASSERT(!Set::subset(x,x3));
  CT_ASSERT(Set::cardinality(Set::union_two(x,x3))==5);
  CT_ASSERT(Set::member(Set::union_two(x,x3),9));
  CT_ASSERT(Set::member(Set::union_two(x,x3),99));
  CT_ASSERT(Set::member(Set::union_two(x,x3),11));
  CT_ASSERT(!Set::is_empty(x));

  set_t<stringptr_t<`H,`H>,`H> s = Set::empty(strptrcmp);
  s = Set::insert(s,allocstr("hello"));
  s = Set::insert(s,allocstr("world"));
  s = Set::insert(s,allocstr("sphinx"));
  s = Set::insert(s,allocstr("quartz"));
  CT_ASSERT(test_list_cmp(strptrcmp,set_elements(s),
                  new List(allocstr("hello"),
                        new List(allocstr("quartz"),
                              new List(allocstr("sphinx"),
                                    new List(allocstr("world"),NULL))))) == 0);
  ;
}

CT_SUITE_INIT
CT_ADD_TEST(set_test)
CT_SUITE_END


