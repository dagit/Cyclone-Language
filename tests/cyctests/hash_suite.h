

/**** hash test suite  ****/
CT_SUITE_BEGIN(Hash)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


int hash_int(int i) { return i; }
void hashtable_test() {
  table_t<int,int> x = Hashtable::create(17,int_cmp,hash_int);
  Hashtable::insert(x,1,2);
  Hashtable::insert(x,-4,445);
  Hashtable::insert(x,300,2828);
  CT_ASSERT(Hashtable::lookup(x,1)==2);
  CT_ASSERT(Hashtable::lookup(x,-4)==445);
  CT_ASSERT(Hashtable::lookup(x,300)==2828);

  table_t<stringptr_t<`H,`H>,stringptr_t<`H,`H>> x2 = 
    Hashtable::create(17,strptrcmp,hash_stringptr);
  Hashtable::insert(x2,allocstr("foo"),allocstr("bar"));
  Hashtable::insert(x2,allocstr("baz"),allocstr("rogue"));
  CT_ASSERT(strptrcmp(Hashtable::lookup(x2,allocstr("foo")),
			   allocstr("bar"))==0);
  CT_ASSERT(strptrcmp(Hashtable::lookup(x2,allocstr("baz")),
			   allocstr("rogue"))==0);

  ;
}
CT_SUITE_INIT
CT_ADD_TEST(hashtable_test)
CT_SUITE_END




