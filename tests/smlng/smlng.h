#ifndef _SMLNG_H_
#define _SMLNG_H_
#include "buffer.h"
#include "list.h"

extern int is_change;

/*
enum Tag { Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
           B_tag, EM_tag, I_tag, PL_tag, S_tag, TT_tag, U_tag, r_tag,
           g_tag, b_tag, c_tag, m_tag, y_tag, k_tag, w_tag
};
typedef enum Tag tag_t;
*/
typedef short tag_t;
#define Zero 0
#define One 1
#define Two 2
#define Three 3
#define Four 4
#define Five 5
#define Six 6
#define Seven 7
#define Eight 8
#define Nine 9
#define B_tag 10
#define EM_tag 11
#define I_tag 12
#define PL_tag 13
#define S_tag 14
#define TT_tag 15
#define U_tag 16
#define r_tag 17
#define g_tag 18
#define b_tag 19
#define c_tag 20
#define m_tag 21
#define y_tag 22
#define k_tag 23
#define w_tag 24

extern tunion Document;
typedef tunion Document doc_t;
extern tunion Document {
  Seq(List::list_t<doc_t>); // bound because we split arrays in place
  Text(unsigned int);
  Spc(unsigned int);
  Tagged(tag_t,doc_t);
  Mt;
};
#define DONT_KNOW 255
void dump(Buffer::buf_t b,doc_t d);
extern struct Context {
  unsigned int B :1;
  unsigned int EM :1;
  unsigned int I :1;
  unsigned int S :1;
  unsigned int TT :1;
  char U :2;    // [0-3]
  char size;    // [0-9]
  char color;   // [r_tag-w_tag]
};
typedef struct Context context_t;
context_t init_context;
extern bool is_white(char c);
#endif _SMLNG_H_

