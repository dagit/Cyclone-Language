#ifndef LINENO_H
#define LINENO_H

#include "list.h"

namespace Lineno {

extern struct pos {
  string logical_file;
  string line;
  int    line_no;
  int    col;
};
typedef struct pos @Pos;
extern Pos  pos_of_abs(string,int);
extern void poss_of_abss(string filename, List::list<$(int,Pos)@> places);

}
#endif
