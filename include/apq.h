#ifndef APQ_INCLUDED
#define APQ_INCLUDED
#include <ap.h>
#define T APQ_T
typedef struct T *T;
extern T APQ_fromint(int i);
extern T APQ_fromAP(AP_T n, AP_T d);
extern T APQ_fromstr(const char ? str, int base);
extern char ? APQ_tostr(T x, int base);
extern T APQ_neg(T x);
extern T APQ_abs(T x);
extern T APQ_add(T x, T y);
extern T APQ_sub(T x, T y);
extern T APQ_mul(T x, T y);
extern T APQ_div(T x, T y);
extern int APQ_cmp(T x, T y);
#undef T
#endif
