
/**** filename test suite ****/
CT_SUITE_BEGIN(Filename)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


void filename_test() {
  // UNTESTED:
  // gnuify
  // NOT WORKING:
  // current_dir_name  -- because can't have string global, bad initializer

  let cwd = ".";
  CT_ASSERT(0==strcmp(Filename::concat(cwd,"foo"),
                           "./foo"));
  CT_ASSERT(0==strcmp(Filename::chop_extension("foo.bar"),
                           "foo"));
  CT_ASSERT(0==strcmp(Filename::dirname("/foo/bar"),
                           "/foo"));
  CT_ASSERT(0==strcmp(Filename::basename("/foo/bar"),
                           "bar"));
  CT_ASSERT(Filename::check_suffix("/foo/bar","bar"));
  CT_ASSERT(!Filename::check_suffix("/foo/bar","foo"));
  
}


CT_SUITE_INIT
CT_ADD_TEST(filename_test)
CT_SUITE_END
