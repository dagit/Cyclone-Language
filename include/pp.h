/* This file is part of the Cyclone Library.
   Copyright (C) 2001 AT&T

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

#ifndef _PP_H_
#define _PP_H_

#include <core.h>
#include <cycboot.h>
#include <list.h>

namespace PP {

  /*** \subsection{\texttt{<pp.h>}} */
  /*** Defines a namespace PP that has functions for implementing
       pretty printers.  Internally, PP is an implementation of
       Kamin's version of Wadler's pretty printing combinators, with
       some extensions for doing hyperlinks in Tk text widgets.

       All of the internal data structures used by PP are allocated on
       the heap. */

using Core;
using List;
extern int tex_output;
extern struct Ppstate;
typedef struct Ppstate @ppstate_t;
extern struct Out;
typedef struct Out @out_t;
extern struct Doc;
typedef struct Doc @doc_t;
  /** A value of type [doc_t] is a ``document'' that can be combined
      with other documents, formatted at different widths, converted
      to strings or files. */

extern void file_of_doc(doc_t d, int w, FILE @f);
  /** [file_of_doc(d,w,f)] formats [d] to width [w], and prints the
      formatted output to [f]. */
extern string_t string_of_doc(doc_t d, int w);
  /** [string_of_doc(d,w)] formats [d] to width [w], and returns the
      formatted output in a heap-allocated string. */
extern $(string_t,list_t<$(int,int,int,string_t)@>)@string_and_links(doc_t d, int w);
  /** [string_and_links(d,w)] formats [d] to width [w], returns the
      formatted output in a heap-allocated string, and returns in
      addition a list of hyperlinks.  Each hyperlink has the form
      [\$(line,char,length,contents)], where [line] and [char] give
      the line and character in the formatted output where the
      hyperlink starts, [length] gives the number of characters of the
      hyperlink, and [contents] is a string that the hyperlink should
      point to.  The [line], [char], and [length] are exactly what is
      needed to create a hyperlink in a Tk text widget. */

extern doc_t nil_doc();
  /** [nil_doc()] returns an empty document. */
extern doc_t blank_doc();
  /** [blank_doc()] returns a document consisting of a single space
      character. */
extern doc_t line_doc();
  /** [line_doc()] returns a document consisting of a single line break. */
extern doc_t oline_doc();
  /** [oline_doc()] returns a document consisting of an optional line
      break; when the document is formatted, the pretty printer will
      decide whether to break the line. */
extern doc_t text(string_t<`H> s);
  /** [text(s)] returns a document containing exactly the string [s]. */
extern doc_t textptr(stringptr_t<`H> p);
  /** [textptr(p)] returns a documents containing exactly the string
      pointed to by [p]. */
extern doc_t text_width(string_t<`H> s, int w);
  /** [text_width(s,w)] returns a document containing exactly the string
      [s], which is assumed to have [w] characters.  This is useful when
      [s] contains markup character that don't take up space when printed,
      e.g., instructions for making text bold. */
extern doc_t hyperlink(string_t<`H> shrt, string_t<`H> full);
  /** [hyperlink(shrt,full)] returns a document that will be formatted
      as the string [shrt] linked to the string [full]. */
extern doc_t nest(int k, doc_t d);
  /** [nest(k,d)] returns a document that will be formatted like
      document [d], but indented by [k] spaces. */
extern doc_t cat(... doc_t);
  /** [cat(d1, d2, ..., dn)] returns a document consisting of
      document [d1] followed by [d2], and so on up to [dn].  */
extern doc_t cats(list_t<doc_t,`H> doclist);
  /** [cats(l)] returns a document containing all of the documents in
      list [l], in order. */
extern doc_t cats_arr(doc_t ?`H docs);
  /** [cats_arr(a)] returns a document containing all of the documents in
      array [a], in order. */
extern doc_t doc_union(doc_t d1, doc_t d2);
  /** [doc_union(d1,d2)] does ?? FIX. */
extern doc_t tab(doc_t d);
  /** [tab(d)] returns a document formatted like [d] but indented by a
      tab stop. */
extern doc_t seq(string_t<`H> sep, list_t<doc_t,`H> l);
  /** [seq(sep,l)] returns a document consisting of each document of
      [l], in sequence, with string [sep] between each adjacent
      element of [l]. */
extern doc_t ppseq(doc_t pp(`a), string_t<`H> sep, list_t<`a> l);
  /** [ppseq] is a more general form of [seq]: in [ppseq(pp,sep,l)],
      [l] is a list of values to pretty print in sequence, [pp] is a
      function that knows how to pretty print a value, and [sep] is a
      string to print between each value. */
extern doc_t seql(string_t<`H> sep, list_t<doc_t,`H> l0);
  /** [seql] is like [seq], except that the resulting document has
      line breaks after each separator. */
extern doc_t ppseql(doc_t pp(`a), string_t<`H> sep, list_t<`a> l);
  /** [ppseql] is like [ppseq], except that the resulting document has
      line breaks after each separator. */
extern doc_t group(string_t<`H> start, string_t<`H> stop, string_t<`H> sep, list_t<doc_t,`H> l);
  /** [group(start,stop,sep,l)] is like [cat(text(start), seq(sep,l),
      text(stop))]. */
extern doc_t groupl(string_t<`H> start, string_t<`H> stop, string_t<`H> sep, list_t<doc_t,`H> l);
  /** [groupl] is like [group] but a line break is inserted after each
      separator. */
extern doc_t egroup(string_t<`H> start, string_t<`H> stop, string_t<`H> sep, list_t<doc_t,`H> l);
  /** [egroup] is like [group], except that the empty document is
      returned if the list is empty. */
}
#endif /* _PP_H_ */
