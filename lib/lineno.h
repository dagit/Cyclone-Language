#ifndef LINENO_H
#define LINENO_H

namespace Lineno {

extern struct pos {
  string logical_file;
  string line;
  int    line_no;
  int    col;
};
typedef struct pos @Pos;
extern Pos pos_of_abs(string,int);

}
#endif
