#ifndef _SMLNG_H_
#define _SMLNG_H_
extern enum Tag { Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
           B_tag, EM_tag, I_tag, PL_tag, S_tag, TT_tag, U_tag, r_tag,
           g_tag, b_tag, c_tag, m_tag, y_tag, k_tag, w_tag
};
typedef enum Tag tag_t;
extern tunion Document;
typedef tunion Document doc_t;
extern tunion Document {
  Seq(doc_t ?);
  Text(char ?);
  Tagged(tag_t,doc_t);
};
extern struct Context {
  bool B :1;
  bool EM :1;
  bool I :1;
  bool S :1;
  bool TT :1;
  char U :2;    // [0-3]
  char size;    // [0-9]
  char color;   // [r_tag-w_tag]
};
typedef struct Context context_t;

extern bool is_white(char c);

#define DONT_KNOW 255
void dump(doc_t d);
#endif _SMLNG_H_
