/* $Id: ap.h,v 1.1 2004-07-06 17:49:20 gwprice Exp $ */
#ifndef AP_INCLUDED
#define AP_INCLUDED
#include <stdarg.h>
#define T AP_T
typedef struct T *T;
extern T AP_zero;
extern T AP_one;
extern T AP_new    (long int n);
extern T AP_fromint(long int x);
extern T AP_fromstr(const char *str, int base);
extern long int AP_toint(T x);
extern char    *AP_tostr(T x, int base);
extern T AP_neg(T x);
extern T AP_abs(T x);
extern T AP_add(T x, T y);
extern T AP_sub(T x, T y);
extern T AP_mul(T x, T y);
extern T AP_div(T x, T y);
extern T AP_mod(T x, T y);
extern T AP_pow(T x, T y, T p);
extern T    AP_addi(T x, long int y);
extern T    AP_subi(T x, long int y);
extern T    AP_muli(T x, long int y);
extern T    AP_divi(T x, long int y);
extern long AP_modi(T x, long int y);
extern T AP_lshift(T x, int s);
extern T AP_rshift(T x, int s);
extern T AP_and(T x, T y);
extern T AP_or(T x, T y);
extern T AP_xor(T x, T y);
extern int AP_cmp (T x, T y);
extern int AP_cmpi(T x, long int y);
extern T AP_gcd(T x, T y);
extern T AP_lcm(T x, T y);
#undef T
#endif
