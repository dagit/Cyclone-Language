#ifndef _GRP_H
#define _GRP_H

#include <sys/types.h>

namespace Grp {
  struct group {
    string_t gr_name;
    string_t gr_passwd;
    gid_t gr_gid;
    mstring_t ?gr_mem;
  };

  extern struct group *getgrnam(string_t name);
  extern struct group *getgrgid(uid_t uid);
}

#endif
