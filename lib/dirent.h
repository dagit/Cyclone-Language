#ifndef _DIRENT_H
#define _DIRENT_H

#include <sys/types.h>

namespace Dirent {
  typedef void DIR;
  DIR *opendir(string_t name);
  extern "C" struct dirent *readdir(DIR *dir);
  extern "C" int closedir(DIR *dir);
}

#endif
