#ifndef _CTYPE_H_
#define _CTYPE_H_
#include <config.h>
namespace Ctype {
extern "C" {
int __cdecl isalnum(int);
int __cdecl isalpha(int);
int __cdecl iscntrl(int);
int __cdecl isdigit(int);
int __cdecl isgraph(int);
int __cdecl islower(int);
int __cdecl isprint(int);
int __cdecl ispunct(int);
int __cdecl isspace(int);
int __cdecl isupper(int);
int __cdecl isxdigit(int);
int __cdecl tolower(int);
int __cdecl toupper(int);
int __cdecl isascii(int);
int __cdecl toascii(int);
int __cdecl _tolower(int);
int __cdecl _toupper(int);
}
}
#endif /* _CTYPE_H_ */
