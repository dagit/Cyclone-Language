
/**** bitvec test suite  ****/
CT_SUITE_BEGIN(Bitvec)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


void bitvec_test() {
  bitvec_t b1 = Bitvec::new_empty(24);  // a 24-bit vector
  Bitvec::set(b1,3);                  // set fourth and fifth bits
  Bitvec::set(b1,4);
  CT_ASSERT(Bitvec::get(b1,3));
  CT_ASSERT(Bitvec::get(b1,4));
  CT_ASSERT(!Bitvec::get(b1,2));
  let l1 = Bitvec::to_sorted_list(b1,24);
  CT_ASSERT(0==test_list_cmp(int_cmp,l1,
			  new List(3,new List(4,NULL))));
  bitvec_t b2 = Bitvec::new_full(24);
  bitvec_t b3 = Bitvec::new_empty(24);
  Bitvec::intersect_two(b3,b1,b2);
  CT_ASSERT(Bitvec::compare_two(b3,b1));
  Bitvec::union_two(b3,b1,b2);
  CT_ASSERT(Bitvec::compare_two(b3,b2));
  bitvec_t b4 = Bitvec::new_empty(24);
  Bitvec::diff_two(b3,b2,b1);
  Bitvec::intersect_two(b3,b3,b1);
  CT_ASSERT(Bitvec::compare_two(b3,b4));
  Bitvec::clear(b1,3);
  Bitvec::clear(b1,4);
  CT_ASSERT(Bitvec::compare_two(b1,b4));
  
}
CT_SUITE_INIT
CT_ADD_TEST(bitvec_test)
CT_SUITE_END


