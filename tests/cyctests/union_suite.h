
/**** union test suite ****/
CT_SUITE_BEGIN(TaggedUnions)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}

@tagged union foo { int w; int z; };
@tagged union bar { union foo @f; int x; };
void escape_it(`a x) { }
void tagged_union_test() {
  union foo fx = {.z = 1 };
  union foo @ f = new fx;
  f->z = 1;

  if (tagcheck(f->w)) {
    printf("woo hoo %d\n",f->w); // not reached
  }
  escape_it(f);
  CT_ASSERT (tagcheck(f->w) == false); // failed tagcheck
  CT_ASSERT (f->z == 1); // correct lookup
  try {
    int z = f->w;
    CT_FAILM("Missing match exception");
  } catch {
  default: 
    CT_OK(); break;
  }

  // make sure swap works
  fx.z = 2;
  int z = 0;
  fx.z :=: z;
  CT_ASSERT (fx.z == 0);
  CT_ASSERT (z == 2);
  escape_it(f);

  // make sure that we don't partially swap something on a match failure
  try {
    f->w :=: z;
    CT_FAILM("Missing match exception (2)");
  } catch {
  default:
    CT_ASSERT (f->z == 1);
    CT_ASSERT (z == 2);
    break;
  }

  // swap through multiple levels of union
  union bar @ b = new bar{.f = f};
  CT_ASSERT (b->f->z == 1);
  z :=: b->f->z;
  CT_ASSERT (b->f->z == 2);
  CT_ASSERT (z == 1);
}

CT_SUITE_INIT
CT_ADD_TEST(tagged_union_test)
CT_SUITE_END


