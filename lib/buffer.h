#ifndef _BUFFER_H_
#define _BUFFER_H_
#include "core.h"
#include "stdio.h"
#include "buffer.h"
namespace Buffer {
// Ported from the standard library of 
//(***********************************************************************)
//(*                                                                     *)
//(*                           Objective Caml                            *)
//(*                                                                     *)
//(*  Pierre Weis and Xavier Leroy, projet Cristal, INRIA Rocquencourt   *)
//(*                                                                     *)
//(*  Copyright 1999 Institut National de Recherche en Informatique et   *)
//(*  en Automatique.  Distributed only by permission.                   *)
//(*                                                                     *)
//(***********************************************************************)

//(* Module [Buffer]: extensible string buffers *)

//(* This module implements string buffers that automatically expand
//   as necessary.  It provides accumulative concatenation of strings
//   in quasi-linear time (instead of quadratic time when strings are
//   concatenated pairwise). *) 
extern struct t;
typedef struct t @T;
extern T create(uint n);
//     (* [create n] returns a fresh buffer, initially empty.
//         The [n] parameter is the initial size of the internal string
//         that holds the buffer contents.  That string is automatically
//         reallocated when more than [n] characters are stored in the buffer,
//         but shrinks back to [n] characters when [reset] is called.
//         For best performance, [n] should be of the same order of magnitude
//         as the number of characters that are expected to be stored in
//         the buffer (for instance, 80 for a buffer that holds one output
//         line).  Nothing bad will happen if the buffer grows beyond that
//         limit, however.  In doubt, take [n = 16] for instance.
//         If [n] is not between 1 and [Sys.max_string_length], it will
//         be clipped to that interval. *)
extern string contents(T);
//     (* Return a copy of the current contents of the buffer.
//        The buffer itself is unchanged. *)
extern size_t length(T);
//     (* Return the number of characters currently contained in the buffer. *)
extern void clear(T);
//     (* Empty the buffer. *)
extern void reset(T);
//     (* Empty the buffer and deallocate the internal string holding the
//        buffer contents, replacing it with the initial internal string
//        of length [n] that was allocated by [create n].
//        For long-lived buffers that may have grown a lot, [reset] allows
//        faster reclaimation of the space used by the buffer. *)
extern void add_char(T,char);
//     (* [add_char b c] appends the character [c] at the end of
//        the buffer [b]. *)
extern void add_substring(T,string,int offset,int len);
//     (* [add_substring b s ofs len] takes [len] characters from offset
//        [ofs] in string [s] and appends them at the end of the buffer [b]. *)
extern void add_string(T,string);
//     (* [add_string b s] appends the string [s] at the end of
//        the buffer [b]. *)
extern void add_buffer(T buf_dest, T buf_source);
//     (* [add_buffer b1 b2] appends the current contents of buffer [b2]
//        at the end of buffer [b1].  [b2] is not modified. *)

}
#endif
