


/**** queue test suite  ****/
CT_SUITE_BEGIN(Queue)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


void queue_test() {
  Queue::queue_t<stringptr_t<`H,`H>,`H,ALIASABLE> q = Queue::create();
  CT_ASSERT(Queue::is_empty(q));
  Queue::add(q,allocstr("a"));
  CT_ASSERT(!Queue::is_empty(q));
  Queue::add(q,allocstr("b"));
  CT_ASSERT(strcmp(*(Queue::peek(q)),"a") == 0);
  CT_ASSERT(strcmp(*(Queue::take(q)),"a") == 0);
  CT_ASSERT(Queue::length(q) == 1);
  ;
}
CT_SUITE_INIT
CT_ADD_TEST(queue_test)
CT_SUITE_END




