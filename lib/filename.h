#ifndef _FILENAME_H_
#define _FILENAME_H_
#include "core.h"

namespace Filename {
  //extern string current_dir_name;
extern mstring_t concat(string_t, string_t);
extern mstring_t chop_extension(string_t);
extern mstring_t dirname(string_t);
extern mstring_t basename(string_t);
extern bool check_suffix(string_t, string_t);
extern mstring_t gnuify(string_t);
}

#endif
