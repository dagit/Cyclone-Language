#ifndef _LINENO_H_
#define _LINENO_H_

#include "list.h"

namespace Lineno {

extern struct Pos {
  string_t logical_file;
  mstring_t line;
  int    line_no;
  int    col;
};
typedef struct Pos @pos_t;
extern pos_t  pos_of_abs(string_t,int);
extern void poss_of_abss(string_t filename, List::list_t<$(int,pos_t)@> places);

}
#endif
