

/**** try catch test suite ****/
CT_SUITE_BEGIN(Exception)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}



void return_from_try_test() {
  try
    return;
  catch {
  default:
    CT_FAILM("Wrong handler!");
    return;
  }
}

void break_in_try_test() {
  try {
    for (int i = 5; i>0; i--) {
      try {
        if (i == 3) break;
      }
      catch {
      default:
        CT_FAILM("incorrect handler, break in try");
        break;
      }
    }
    throw new List::List_mismatch;
  }
  catch {
  default: return;
  }
  CT_FAILM("Failed to catch outermost exception");
}

void continue_in_try_test() {
  try {
    for (int i = 5; i>0; i--) {
      try continue;
      catch {
      default:
        CT_FAILM("INCORRECT HANDLER, continue in try");
        break;
      }
    }
    throw new List::List_mismatch;
  }
  catch {
  default:
    return;
  }
  CT_FAILM("Failed to catch outermost exception");
}


CT_SUITE_INIT
CT_ADD_TEST(break_in_try_test)
CT_ADD_TEST(continue_in_try_test)
CT_ADD_TEST(return_from_try_test)
CT_SUITE_END


