#ifndef _CTYPE_H_
#define _CTYPE_H_
#include <config.h>
namespace Ctype {
extern "C" {
int _EXFUN(isalnum, (int __c));
int _EXFUN(isalpha, (int __c));
int _EXFUN(iscntrl, (int __c));
int _EXFUN(isdigit, (int __c));
int _EXFUN(isgraph, (int __c));
int _EXFUN(islower, (int __c));
int _EXFUN(isprint, (int __c));
int _EXFUN(ispunct, (int __c));
int _EXFUN(isspace, (int __c));
int _EXFUN(isupper, (int __c));
int _EXFUN(isxdigit,(int __c));
int _EXFUN(tolower, (int __c));
int _EXFUN(toupper, (int __c));
int _EXFUN(isascii, (int __c));
int _EXFUN(toascii, (int __c));
int _EXFUN(_tolower, (int __c));
int _EXFUN(_toupper, (int __c));
}
}
#endif /* _CTYPE_H_ */
