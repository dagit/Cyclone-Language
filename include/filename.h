/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, AT&T

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


#ifndef _FILENAME_H_
#define _FILENAME_H_
#include <core.h>

namespace Filename {

  /*** \subsection{\texttt{<filename.h>}} */
  /*** Defines a namespace Filename, which implements some useful
       operations on file names that are represented as strings. */

extern mstring_t concat(string_t, string_t);
  /** Assuming that [s1] is a directory name and [s2] is a file name,
      [concat(s1,s2)] returns a new (heap-allocated) file name for the
      child [s2] of directory [s1]. */
extern mstring_t chop_extension(string_t);
  /** [chop_extension(s)] returns a copy of [s] with any file
      extension removed.  A file extension is a period (`.') followed
      by a sequence of non-period characters.  If [s] does not have a
      file extension, [chop_extension(s)] throws
      [Core::Invalid_argument("chop_extension")]. */
extern mstring_t dirname(string_t);
  /** [dirname(s)] returns the directory part of [s].  For example, if
      [s] is ["foo/bar/baz"], [dirname(s)] returns ["foo/bar"]. */
extern mstring_t basename(string_t);
  /** [basename(s)] returns the file part of [s].  For example, if
      [s] is ["foo/bar/baz"], [basename(s)] returns ["baz"]. */
extern bool check_suffix(string_t, string_t);
  /** [check_suffix(filename,suffix)] returns true if [filename] ends
      in [suffix], and returns false otherwise. */
extern mstring_t gnuify(string_t);
  /** [gnuify(s)] forces [s] to follow Unix file name conventions: any
      Windows separator characters (backslashes) are converted to Unix
      separator characters (forward slashes). */
}

#endif
