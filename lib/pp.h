#ifndef _PP_H_
#define _PP_H_

#include "core.h"
#include "stdio.h"
#include "list.h"

namespace PP {
using Core;
using Stdio;
using List;
extern struct Ppstate;
typedef struct Ppstate @ppstate_t;
extern struct Out;
typedef struct Out @out_t;
extern struct Doc;
typedef struct Doc @doc_t;
extern void file_of_doc(doc_t d, int w, FILE @f);
extern string string_of_doc(doc_t d, int w);
extern $(string,list_t<$(int,int,int,string)@>)@string_and_links(doc_t d, int w);
extern doc_t text(string s);
extern doc_t nil_doc();
extern doc_t blank_doc();
extern doc_t hyperlink(string shrt, string full);
extern doc_t line_doc();
extern doc_t nest(int k, doc_t d);
extern doc_t concat(doc_t d1, doc_t d2);
extern doc_t concats(list_t<doc_t> doclist);
extern doc_t concats_arr(doc_t ?docs);
extern doc_t doc_union(doc_t d, doc_t d2);
extern doc_t oline_doc();
extern doc_t tab(doc_t d);
extern doc_t ppseq(doc_t pp(`a), string sep, list_t<`a> l0);
extern doc_t seq(string sep, list_t<doc_t> l0);
extern doc_t ppseql(doc_t pp(`a), string sep, list_t<`a> l0);
extern doc_t seql(string sep, list_t<doc_t> l0);
}
#endif /* _PP_H_ */
