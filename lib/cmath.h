/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, AT&T

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

#ifndef  _MATH_H_
#define  _MATH_H_
namespace std {
extern "C" {

double acos(double);
float  acosf(float);
double acosh(double);
float  acoshf(float);
double asin(double);
float  asinf(float);
double asinh(double);
float  asinhf(float);
double atan(double);
double atan2(double, double);
float  atan2f(float, float);
float  atanf(float);
double atanh(double);
float  atanhf(float);
double cbrt(double);
float  cbrtf(float);
double ceil(double);
float  ceilf(float);
double copysign(double, double);
float  copysignf(float, float);
double cos(double);
float  cosf(float);
double cosh(double);
float  coshf(float);
double drem(double, double); //Not Posix
float  dremf(float, float); //Not Posix
double erf(double);
double erfc(double);
float  erfcf(float);
float  erff(float);
double exp(double);
float  expf(float);
double expm1(double);
float  expm1f(float);
double fabs(double);
float  fabsf(float);
int    finite(double); //Not Posix
int    finitef(float); //Not Posix
double floor(double);
float  floorf(float);
double fmod(double, double);
float  fmodf(float, float);
double frexp(double, int @);
float  frexpf(float, int @);
double gamma(double); //Not Posix
float  gammaf(float); //Not Posix
double hypot(double, double);
float  hypotf(float, float);
int    ilogb(double);
int    ilogbf(float);
int    isinf(double);
int    isinff(float); //Not Posix
int    isnan(double);
int    isnanf(float); //Not Posix
double j0(double);
float  j0f(float); //Not Posix
double j1(double);
float  j1f(float); //Not Posix
double jn(int, double);
float  jnf(int, float); //Not Posix
double ldexp(double, int);
float  ldexpf(float, int);
double lgamma(double);
float  lgammaf(float);
double log(double);
float  logf(float);
double log10(double);
float  log10f(float);
double log1p(double);
float  log1pf(float);
#define log2(x)(log(x) / M_LOG2_E)
#define log2f(x)(logf(x) / (float) M_LOG2_E)
double modf(double, double @);
float  modff(float, float @);
double nextafter(double, double);
float  nextafterf(float, float);
double pow(double, double);
float  powf(float, float);
double remainder(double, double);
float  remainderf(float, float);
double rint(double);
float  rintf(float);
double scalbn(double, int);
float  scalbnf(float, int);
double sin(double);
float  sinf(float);
double sinh(double);
float  sinhf(float);
double sqrt(double);
float  sqrtf(float);
double tan(double);
float  tanf(float);
double tanh(double);
float  tanhf(float);
double y0(double);
float  y0f(float); //Not Posix
double y1(double);
float  y1f(float); //Not Posix
double yn(int, double);
float  ynf(int, float); //Not Posix


/* Useful constants.  */

#define M_E		2.7182818284590452354
#define M_LOG2E		1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN2		0.69314718055994530942
#define M_LN10		2.30258509299404568402
#define M_PI		3.14159265358979323846
#define M_TWOPI         (M_PI * 2.0)
#define M_PI_2		1.57079632679489661923
#define M_PI_4		0.78539816339744830962
#define M_3PI_4		2.3561944901923448370E0
#define M_SQRTPI        1.77245385090551602792981
#define M_1_PI		0.31830988618379067154
#define M_2_PI		0.63661977236758134308
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2		1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440
#define M_LN2LO         1.9082149292705877000E-10
#define M_LN2HI         6.9314718036912381649E-1
#define M_SQRT3   	1.73205080756887719000
#define M_IVLN10        0.43429448190325182765 /* 1 / log(10) */
#define M_LOG2_E        0.693147180559945309417
#define M_INVLN2        1.4426950408889633870E0  /* 1 / log(2) */

}
}
#endif /* _MATH_H_ */
