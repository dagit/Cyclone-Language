#ifndef _PWD_H
#define _PWD_H

#include <sys/types.h>

namespace Pwd {
  struct passwd {
    string_t pw_name;
    string_t pw_passwd;
    uid_t pw_uid;
    gid_t pw_gid;
    string_t pw_gecos;
    string_t pw_dir;
    string_t pw_shell;
  };

  extern struct passwd *getpwnam(string_t name);
}

#endif
