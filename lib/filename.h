#ifndef FILENAME_H
#define FILENAME_H
#include "core.h"

namespace Filename {
  //extern string current_dir_name;
extern string concat(string, string);
extern string chop_extension(string);
extern string dirname(string);
extern string basename(string);
extern bool check_suffix(string, string);
extern string gnuify(string);
}

#endif
