/*
 * stdlib.h
 *
 * Definitions for common types, variables, and functions.
 */
#ifndef _STDLIB_H_
#include <config.h>
//#include <sys/reent.h> // __sFILE conflicts with the one in stdio.h

namespace Stdlib {
extern "C" {
#define _STDLIB_H_

typedef struct _Div 
{
  int quot; /* quotient */
  int rem; /* remainder */
} div_t;

typedef struct _Ldiv
{
  long quot; /* quotient */
  long rem; /* remainder */
} ldiv_t;

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX __RAND_MAX

extern int __mb_cur_max;

#define MB_CUR_MAX __mb_cur_max

/* CYCLONE: now defined in core.h
void	_EXFUN(abort,(void) _ATTRIBUTE ((noreturn)));
*/
int	_EXFUN(abs,(int));
int	_EXFUN(atexit,(void (*__func)(void)));
/* wrapped
double	_EXFUN(atof,(const char *__nptr));
*/
#ifndef __STRICT_ANSI__
/* wrapped
float	_EXFUN(atoff,(const char *__nptr));
*/
#endif
/* wrapped
int	_EXFUN(atoi,(const char *__nptr));
long	_EXFUN(atol,(const char *__nptr));
*/
/* unsafe
void *	_EXFUN(bsearch,(const void * __key,
		       const void * __base,
		       size_t __nmemb,
		       size_t __size,
		       int _compar(const void *, const void *)));
void *	_EXFUN(calloc,(size_t __nmemb, size_t __size));
*/
div_t	_EXFUN(div,(int __numer, int __denom));
/* now in core.h
void	_EXFUN(exit,(int __status) _ATTRIBUTE ((noreturn)));
*/
/* unsafe
void	_EXFUN(free,(void *));
*/
/* wrapped
char *  _EXFUN(getenv,(const char *__string));
char *	_EXFUN(_getenv_r,(struct _reent *, const char *__string));
char *	_EXFUN(_findenv,(const char *, int *));
char *	_EXFUN(_findenv_r,(struct _reent *, const char *, int *));
*/
/* JGM: I'm commenting this out as we get a confliciting types warning here
long	_EXFUN(labs,(long));
*/
ldiv_t	_EXFUN(ldiv,(long __numer, long __denom));
/*
void *	_EXFUN(malloc,(size_t __size));
*/
/* FIX:  need to be wrapped
int	_EXFUN(mblen,(const char *, size_t));
int	_EXFUN(_mblen_r,(struct _reent *, const char *, size_t, int *));
int	_EXFUN(mbtowc,(wchar_t *, const char *, size_t));
int	_EXFUN(_mbtowc_r,(struct _reent *, wchar_t *, const char *, size_t, int *));
int	_EXFUN(wctomb,(char *, wchar_t));
int	_EXFUN(_wctomb_r,(struct _reent *, char *, wchar_t, int *));
size_t	_EXFUN(mbstowcs,(wchar_t *, const char *, size_t));
size_t	_EXFUN(_mbstowcs_r,(struct _reent *, wchar_t *, const char *, size_t, int *));
size_t	_EXFUN(wcstombs,(char *, const wchar_t *, size_t));
size_t	_EXFUN(_wcstombs_r,(struct _reent *, char *, const wchar_t *, size_t, int *));
*/
#ifndef __STRICT_ANSI__
#ifndef _REENT_ONLY
/* FIX:  need to be wrapped
int     _EXFUN(mkstemp,(char *));
char *  _EXFUN(mktemp,(char *));
*/
#endif
#endif
/* unsafe
void	_EXFUN(qsort,(void * __base, size_t __nmemb, size_t __size, int(*_compar)(const void *, const void *)));
*/
int	_EXFUN(rand,(void));
/* unsafe
void *	_EXFUN(realloc,(void * __r, size_t __size));
*/
void	_EXFUN(srand,(unsigned __seed));
/* FIX: needs to be wrapped
double	_EXFUN(strtod,(const char *__n, char **_end_PTR));
*/
#ifndef __STRICT_ANSI__
/* FIX:  needs to be wrapped
float	_EXFUN(strtodf,(const char *__n, char **_end_PTR));
*/
#endif
/* FIX:  needs to be wrapped
long	_EXFUN(strtol,(const char *__n, char **_end_PTR, int __base));
unsigned long _EXFUN(strtoul,(const char *_n_PTR, char **_end_PTR, int __base));
unsigned long _EXFUN(_strtoul_r,(struct _reent *,const char *_n_PTR, char **_end_PTR, int __base));
int	_EXFUN(system,(const char *__string));
*/

#ifndef __STRICT_ANSI__
/* wrapped
int	_EXFUN(putenv,(const char *__string));
int	_EXFUN(_putenv_r,(struct _reent *, const char *__string));
int	_EXFUN(setenv,(const char *__string, const char *__value, int __overwrite));
int	_EXFUN(_setenv_r,(struct _reent *, const char *__string, const char *__value, int __overwrite));

char *	_EXFUN(gcvt,(double,int,char *));
char *	_EXFUN(gcvtf,(float,int,char *));
char *	_EXFUN(fcvt,(double,int,int *,int *));
char *	_EXFUN(fcvtf,(float,int,int *,int *));
char *	_EXFUN(ecvt,(double,int,int *,int *));
char *	_EXFUN(ecvtbuf,(double, int, int*, int*, char *));
char *	_EXFUN(fcvtbuf,(double, int, int*, int*, char *));
char *	_EXFUN(ecvtf,(float,int,int *,int *));
char *	_EXFUN(dtoa,(double, int, int, int *, int*, char**));
*/
int	_EXFUN(rand_r,(unsigned *__seed));

#ifndef __CYGWIN__
/* unsafe 
void	_EXFUN(cfree,(void *));
*/
#else
/* wrapped
char *	_EXFUN(realpath,(const char *, char *));
void	_EXFUN(unsetenv,(const char *__string));
void	_EXFUN(_unsetenv_r,(struct _reent *, const char *__string));
*/
int	_EXFUN(random,(void));
long	_EXFUN(srandom,(unsigned __seed));
/* wrapped
char *  _EXFUN(ptsname, (int));
*/
int     _EXFUN(grantpt, (int));
int     _EXFUN(unlockpt,(int));
#endif

#endif /* ! __STRICT_ANSI__ */

/* wrapped
char *	_EXFUN(_dtoa_r,(struct _reent *, double, int, int, int *, int*, char**));
*/
/* unsafe
void *	_EXFUN(_malloc_r,(struct _reent *, size_t));
void *	_EXFUN(_calloc_r,(struct _reent *, size_t, size_t));
void	_EXFUN(_free_r,(struct _reent *, void *));
void *	_EXFUN(_realloc_r,(struct _reent *, void *, size_t));
*/
/* wrapped
void	_EXFUN(_mstats_r,(struct _reent *, char *));
int	_EXFUN(_system_r,(struct _reent *, const char *));

void	_EXFUN(__eprintf,(const char *, const char *, unsigned int, const char *));
*/
}
extern double atof(const char ?`r);
/*
extern float atoff(string);
*/
extern int atoi(const char ?`r);
extern long atol(const char ?`r);
extern mstring_t getenv(string_t);
extern double strtod(const char ?`r n, const char ?`r *`r2 end);
extern long strtol(const char ?`r n, const char ?`r *`r2 end, int base);
extern unsigned long strtoul(const char ?`r n,const char ?`r *`r2 end, int base);

/*
extern string _getenv_r(struct _reent @,string);
*/
}
#endif /* _STDLIB_H_ */
