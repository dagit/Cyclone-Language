// This is a Cyclone include file, but the actual definitions are
// made in C code.  So, this file is used to automatically produce
// a C include file, for use with the C definitions.

#ifndef _PRECORE_H_
#define _PRECORE_H_

// Core.cyc uses some additional C routines, but they are not
// declared here because they are internal to Core, whereas the
// routines below can be used by any Cyclone program.

#ifndef uint
typedef unsigned int uint;
#endif
typedef char *{0} Cstring;
// a boxed and tagged string: struct {uint sz; Cstring *contents;}@
typedef char ? string;

typedef string @stringptr;

#ifndef bool
typedef int bool;
#endif
#ifndef false 
#define false (0)
#endif
#ifndef true
#define true (1)
#endif
extern "C" `a exit<`a>(int);
extern "C" `a abort<`a>();

#endif /* _PRECORE_H_ */
