/* This file is part of the Cyclone Library.
   Copyright (C) 2006 Greg Morrisett, AT&T

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

#ifndef BASE64_H
#define BASE64_H
namespace Base64 {

  /*** \subsection{\texttt{<base64.h>}} */
  /*** Defines namespace Base64, implementing conversions to and from
       base 64 encoding, as described in the Internet Engineering Task
       Force RFC-1521.

       There is a subtlety in encoding string constants in Cyclone.
       A string constant has type

           const char ?@zeroterm;

       This can be cast to (const char ?@nozeroterm), because of the
       const.  Hence it can be passed to either encoding function.
       The difference will be that the final zero will be encoded in
       the first case, and not the second.  And note, the decode
       function produces a zeroterm output, so

           adecode(aencode("Hello"));

       will produce a string with numelts 7: there will be two zeros
       at the end.

           adecode(aencode_zeroterm("Hello"));

       will produce a string with numelts 6, only one zero at the end
       of Hello.
  */

unsigned int encoding_length(unsigned int len);
  /** [encoding_length(len)] returns the number of characters needed
      to encode [len] characters in base 64. */

int encode(const char ?@nozeroterm src,
           char ?dest,
           unsigned int src_len);
  /** [encode(src,dest,src_len)] puts the encoding of the first
      [src_len] characters of [src] into [dest].  It returns 1 on
      error and 0 on success.*/

char ?aencode(const char ?@nozeroterm src);
  /** [aencode(src)] returns a heap allocated string with the base 64
      encoding of [src].  [src] may contain zero characters. */
char ?aencode_zeroterm(const char ?src);
  /** [aencode_zeroterm(src)] returns a heap allocated string with the
      base 64 encoding of [src].  [src] is assumed to be zero
      terminated. */

unsigned int decoding_length(const char ?src);
  /** [dencoding_length(len)] returns the number of characters needed
      for the decoding of [src]. */
int decode(const char ?src, char?@nozeroterm dest);
  /** [decode(src,dest)] puts the base 64 decoding of [src] into
      [dest].  It returns 1 on error and 0 on success. */
char ?@nozeroterm adecode(const char ?src);
  /** [adecode(src)] returns a heap allocated string containing the
      base 64 decoding of [src].  It returns NULL on error. */

}
#endif
