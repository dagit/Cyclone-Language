/* This file is part of the MediaNet Project.
   Copyright (C) 2002-2004 Michael Hicks

   MediaNet is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   MediaNet is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

#ifndef __STREAMBUFF_H__
#define __STREAMBUFF_H__

#include <core.h>

/** Defines **/

#define MAX_STB_PRIORITY 10


/** Types **/

struct DataBuff<`q::Q\T> {
  unsigned int ofs;
  char ? @aqual(`q) @nozeroterm buf;
};
typedef struct DataBuff<`q> databuf_t<`q>;

/* streambuffers are either packets, which are sent with a length, or
   they are just buffers */
enum StreamType { CharBuffer, Packet };

struct StreamBuff {
  <`i>
  enum StreamType typ;          // type of the data
  unsigned int priority;        // used to help with queueing decisions
  const char ?`H device;	// what device it came from
  tag_t<`i> numbufs;
  databuf_t<\RC> bufs[numbufs];
};
typedef struct StreamBuff @@aqual(`q) `r streambuff_t<`r,`q>;
typedef struct StreamBuff *@aqual(`q) `r streambuff_opt_t<`r,`q>;

/** Functions **/

extern unsigned int stb_size(streambuff_t stb);
extern string_t stb_info(streambuff_t stb);
extern void stb_free(streambuff_t stb);
extern void stb_free_unique(streambuff_t<`H,\U> stb) __attribute((consume(1)));
  // stb_free frees the underlying buffers if unique; caller must free header
extern bool stb_noaliases(streambuff_t stb);
extern streambuff_t<`r,`q> alloc_stb_with(region_t<`r> rgnhdr,
					  aqual_t<`q\T> q,
					  enum StreamType typ,
					  const char ?`H device,
					  unsigned int priority,
					  databuf_t<\RC> data);
extern streambuff_t<`rhdr,`q> alloc_stb(region_t<`rhdr::R> rgnhdr,
					aqual_t<`q\T> q,
					enum StreamType typ,
					const char ?`H device,
					unsigned int priority,
					const char ? @nozeroterm buf);
extern streambuff_t<`r,`q> stb_copy(region_t<`r::R> rgnhdr, aqual_t<`q\T> q,
				 int ofs, streambuff_t stb);
extern streambuff_t<`r,`q> stb_clone(region_t<`r::R> rgn, aqual_t<`q\T> q, streambuff_t stb);
// pulls out the first buffer from the given streambuffer and
// invokes the given function with it.  Takes care of swapping, etc.

extern streambuff_t<`H,\U> stb_rest(int ofs, streambuff_t<`H,\U> stb) __attribute((consume(2)));
// we assume `U streambuffs for stb_rest, so that refcounts are
// properly updated

extern streambuff_t<`r,`q> stb_prepend(region_t<`r::R> rgn,aqual_t<`q\T> q, streambuff_t stb,
				       char ?@aqual(\RC) @nozeroterm `H nbuf)  __attribute((consume(4)));
// prepends the given buffer to the streambuffer.  Creates aliases to
// the existing databuf buffers.

extern int stb_num_bufs(streambuff_t stb);
extern int stb_with_buffer(streambuff_t stb, int idx,
			   int (@f)<`r::R>(`a env, char ? @nozeroterm `r),
			   `a env);


/*** for performance evaluation ***/

#endif
