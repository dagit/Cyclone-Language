  
/**** let test suite ****/
CT_SUITE_BEGIN(Letv)
void begin() {
    // do nothing
}

void end() {
    // do nothing
}


void letv_test() {
    let x,y;
    y = 0;
    if (y != 0) x = 3;
    else x = 4;
    CT_ASSERT(x == 4);
    CT_ASSERT(y == 0);
    
}

CT_SUITE_INIT
CT_ADD_TEST(letv_test)
CT_SUITE_END


