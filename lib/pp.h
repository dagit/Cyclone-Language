#ifndef PP_H
#define PP_H

#include "core.h"
#include "list.h"

namespace PP {
using List;
extern struct ppstate;
typedef struct ppstate @Ppstate;
extern struct out;
typedef struct out @Out;
extern struct doc;
typedef struct doc @Doc;
extern void file_of_doc(Doc d, int w, FILE @f);
extern string string_of_doc(Doc d, int w);
extern $(string,list<$(int,int,int,string)@>) @string_and_links(Doc d, int w);
extern Doc text(string s);
extern Doc nil_doc();
extern Doc blank_doc();
extern Doc hyperlink(string shrt, string full);
extern Doc line_doc();
extern Doc nest(int k, Doc d);
extern Doc concat(Doc d1, Doc d2);
extern Doc concats(list<Doc> doclist);
extern Doc concats_arr(Doc docs[?]);
extern Doc doc_union(Doc d, Doc d2);
extern Doc oline_doc();
extern Doc tab(Doc d);
extern Doc ppseq<`a>(Doc pp(`a), string sep, list<`a> l0);
extern Doc seq(string sep, list<Doc> l0);
extern Doc ppseql<`a>(Doc pp(`a), string sep, list<`a> l0);
extern Doc seql(string sep, list<Doc> l0);
}
#endif
