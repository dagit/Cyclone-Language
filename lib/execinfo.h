#ifndef _EXECINFO_H
#define _EXECINFO_H

namespace Execinfo {
  int backtrace(int?, int);

  // Cyclone-specific: If a backtrace is available (as under Linux),
  // print it to stdout and return 0.  Else print nothing and return
  // nonzero.
  int bt(void);
}

#endif
