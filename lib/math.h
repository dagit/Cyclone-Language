/* math.h -- Definitions for the math floating point package.  */

#ifndef  _MATH_H_
#define  _MATH_H_
namespace Math {
extern "C" {
#ifndef _PARAMS
#define _PARAMS(x) x
#endif

#ifndef EXFUN
#define EXFUN(f,x) f x
#endif

/* Reentrant ANSI C functions.  */

#ifndef __math_68881
double atan _PARAMS((double));
double cos _PARAMS((double));
double sin _PARAMS((double));
double tan _PARAMS((double));
double tanh _PARAMS((double));
double frexp _PARAMS((double, int *));
double modf _PARAMS((double, double *));
double ceil _PARAMS((double));
double fabs _PARAMS((double));
double floor _PARAMS((double));
#endif /* ! defined (__math_68881) */

/* Non reentrant ANSI C functions.  */

#ifndef _REENT_ONLY
#ifndef __math_6881
double acos _PARAMS((double));
double asin _PARAMS((double));
double atan2 _PARAMS((double, double));
double cosh _PARAMS((double));
double sinh _PARAMS((double));
double exp _PARAMS((double));
double ldexp _PARAMS((double, int));
double log _PARAMS((double));
double log10 _PARAMS((double));
double pow _PARAMS((double, double));
double sqrt _PARAMS((double));
double fmod _PARAMS((double, double));
#endif /* ! defined (__math_68881) */
#endif /* ! defined (_REENT_ONLY) */

#ifndef __STRICT_ANSI__

/* Non ANSI double precision functions.  */

double infinity _PARAMS(());
double nan _PARAMS(());
int isnan _PARAMS((double));
int isinf _PARAMS((double));
int finite _PARAMS((double));
double copysign _PARAMS((double, double));
int ilogb _PARAMS((double));

double asinh _PARAMS((double));
double cbrt _PARAMS((double));
double nextafter _PARAMS((double, double));
double rint _PARAMS((double));
double scalbn _PARAMS((double, int));

#ifndef __math_68881
double log1p _PARAMS((double));
double expm1 _PARAMS((double));
#endif /* ! defined (__math_68881) */

#ifndef _REENT_ONLY
double acosh _PARAMS((double));
double atanh _PARAMS((double));
double remainder _PARAMS((double, double));
double gamma _PARAMS((double));
double gamma_r _PARAMS((double, int *));
double lgamma _PARAMS((double));
double lgamma_r _PARAMS((double, int *));
double erf _PARAMS((double));
double erfc _PARAMS((double));
double y0 _PARAMS((double));
double y1 _PARAMS((double));
double yn _PARAMS((int, double));
double j0 _PARAMS((double));
double j1 _PARAMS((double));
double jn _PARAMS((int, double));
#define log2(x) (log (x) / M_LOG2_E)

#ifndef __math_68881
double hypot _PARAMS((double, double));
#endif

double cabs();
double drem _PARAMS((double, double));

#endif /* ! defined (_REENT_ONLY) */

/* Single precision versions of ANSI functions.  */

float atanf _PARAMS((float));
float cosf _PARAMS((float));
float sinf _PARAMS((float));
float tanf _PARAMS((float));
float tanhf _PARAMS((float));
float frexpf _PARAMS((float, int *));
float modff _PARAMS((float, float *));
float ceilf _PARAMS((float));
float fabsf _PARAMS((float));
float floorf _PARAMS((float));

#ifndef _REENT_ONLY
float acosf _PARAMS((float));
float asinf _PARAMS((float));
float atan2f _PARAMS((float, float));
float coshf _PARAMS((float));
float sinhf _PARAMS((float));
float expf _PARAMS((float));
float ldexpf _PARAMS((float, int));
float logf _PARAMS((float));
float log10f _PARAMS((float));
float powf _PARAMS((float, float));
float sqrtf _PARAMS((float));
float fmodf _PARAMS((float, float));
#endif /* ! defined (_REENT_ONLY) */

/* Other single precision functions.  */

float infinityf _PARAMS(());
float nanf _PARAMS(());
int isnanf _PARAMS((float));
int isinff _PARAMS((float));
int finitef _PARAMS((float));
float copysignf _PARAMS((float, float));
int ilogbf _PARAMS((float));

float asinhf _PARAMS((float));
float cbrtf _PARAMS((float));
float nextafterf _PARAMS((float, float));
float rintf _PARAMS((float));
float scalbnf _PARAMS((float, int));
float log1pf _PARAMS((float));
float expm1f _PARAMS((float));

#ifndef _REENT_ONLY
float acoshf _PARAMS((float));
float atanhf _PARAMS((float));
float remainderf _PARAMS((float, float));
float gammaf _PARAMS((float));
float gammaf_r _PARAMS((float, int *));
float lgammaf _PARAMS((float));
float lgammaf_r _PARAMS((float, int *));
float erff _PARAMS((float));
float erfcf _PARAMS((float));
float y0f _PARAMS((float));
float y1f _PARAMS((float));
float ynf _PARAMS((int, float));
float j0f _PARAMS((float));
float j1f _PARAMS((float));
float jnf _PARAMS((int, float));
#define log2f(x) (logf (x) / (float) M_LOG2_E)
float hypotf _PARAMS((float, float));

float cabsf();
float dremf _PARAMS((float, float));

#endif /* ! defined (_REENT_ONLY) */

/* The gamma functions use a global variable, signgam.  */

int signgam;

/* The exception structure passed to the matherr routine.  */

struct __exception 
{
  int type;
  char *name;
  double arg1;
  double arg2;
  double retval;
  int err;
};

int matherr _PARAMS((struct __exception *e));

/* Values for the type field of struct exception.  */

#define DOMAIN 1
#define SING 2
#define OVERFLOW 3
#define UNDERFLOW 4
#define TLOSS 5
#define PLOSS 6

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

/* Global control over fdlibm error handling.  */
/*
enum __fdlibm_version
{
  __fdlibm_ieee = -1,
  __fdlibm_svid,
  __fdlibm_xopen,
  __fdlibm_posix
};

#define _LIB_VERSION_TYPE enum __fdlibm_version
#define _LIB_VERSION __fdlib_version

_CONST _LIB_VERSION_TYPE _LIB_VERSION;

#define _IEEE_  __fdlibm_ieee
#define _SVID_  __fdlibm_svid
#define _XOPEN_ __fdlibm_xopen
#define _POSIX_ __fdlibm_posix
*/

#endif /* ! defined (__STRICT_ANSI__) */

#ifdef __FAST_MATH__
/* These functions are in assembler, they really do take floats. This
   can only be used with a real ANSI compiler */

float EXFUN(fast_sinf,(float));
float EXFUN(fast_cosf,(float));
float EXFUN(fast_tanf,(float));

float EXFUN(fast_asinf,(float));
float EXFUN(fast_acosf,(float));
float EXFUN(fast_atanf,(float));

float EXFUN(fast_sinhf,(float));
float EXFUN(fast_coshf,(float));
float EXFUN(fast_tanhf,(float));

float EXFUN(fast_asinhf,(float));
float EXFUN(fast_acoshf,(float));
float EXFUN(fast_atanhf,(float));

float EXFUN(fast_absf,(float));
float EXFUN(fast_sqrtf,(float));
float EXFUN(fast_exp2f,(float));
float EXFUN(fast_exp10f,(float));
float EXFUN(fast_expef,(float));
float EXFUN(fast_log10f,(float));
float EXFUN(fast_log2f,(float));
float EXFUN(fast_logef,(float));
#define	sinf(x)		fast_sinf(x)
#define	cosf(x)		fast_cosf(x)
#define	tanf(x)		fast_tanf(x)
#define	asinf(x)	fast_asinf(x)
#define	acosf(x)	fast_acosf(x)
#define	atanf(x)	fast_atanf(x)
#define	sinhf(x)	fast_sinhf(x)
#define	coshf(x)	fast_coshf(x)
#define	tanhf(x)	fast_tanhf(x)
#define	asinhf(x)	fast_asinhf(x)
#define	acoshf(x)	fast_acoshf(x)
#define	atanhf(x)	fast_atanhf(x)
#define	absf(x)		fast_absf(x)
#define	sqrtf(x)	fast_sqrtf(x)
#define	exp2f(x)	fast_exp2f(x)
#define	exp10f(x)	fast_exp10f(x)
#define	expef(x)	fast_expef(x)
#define	log10f(x)	fast_log10f(x)
#define	log2f(x)	fast_log2f(x)
#define	logef(x)	fast_logef(x)
#endif /* __FAST_MATH__ */
}
}
#endif /* _MATH_H_ */
