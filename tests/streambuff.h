#ifndef __STREAMBUFF_H__
#define __STREAMBUFF_H__

#include <core.h>

/** Defines **/

#define MAX_STB_PRIORITY 10

/** Types **/

struct DataBuff<`r> {
  unsigned int ofs;
  char ? @nozeroterm `r buf;
};
typedef struct DataBuff<`r> databuf_t<`r>;

// #define DATABUF_SIZE(data,idx) ((data)[idx].sz)-((data)[idx].ofs)
// #define DATABUF_VALUE(data,idx) ((data)[idx].buf + (data)[idx].ofs)

/* streambuffers are either packets, which are sent with a length, or
   they are just buffers */
enum StreamType { CharBuffer, Packet };

struct StreamBuff {
  <`i>
  enum StreamType typ;          // type of the data
  unsigned int priority;        // used to help with queueing decisions
  const char ?`H device;	// what device it came from
  tag_t<`i> numbufs;
  databuf_t<`RC> bufs[numbufs];
};
typedef struct StreamBuff @`r streambuff_t<`r>;
typedef struct StreamBuff *`r streambuff_opt_t<`r>;

/** Functions **/

extern unsigned int stb_size(streambuff_t stb);
extern string_t stb_info(streambuff_t stb);
extern void stb_free(streambuff_t stb);
extern void stb_free_unique(streambuff_t<`U> stb) __attribute__((consume(1)));
  // stb_free frees the underlying buffers if unique; caller must free header
extern bool stb_noaliases(streambuff_t stb);
extern streambuff_t<`r> alloc_stb_with(region_t<`r::TR> rgnhdr,
				       enum StreamType typ,
				       const char ?`H device,
				       unsigned int priority,
				       databuf_t<`RC> data);
extern streambuff_t<`rhdr> alloc_stb(region_t<`rhdr::TR> rgnhdr,
				     enum StreamType typ,
				     const char ?`H device,
				     unsigned int priority,
				     const char ? @nozeroterm buf);
extern streambuff_t<`r> stb_copy(region_t<`r::TR> rgnhdr,
				 int ofs, streambuff_t stb);
extern streambuff_t<`r> stb_clone(region_t<`r::TR> rgn, streambuff_t stb);
  // pulls out the first buffer from the given streambuffer and
  // invokes the given function with it.  Takes care of swapping, etc.

extern streambuff_t<`U> stb_rest(int ofs, streambuff_t<`U> stb) 
  __attribute__((consume(2)));
  // we assume `U streambuffs for stb_rest, so that refcounts are
  // properly updated

extern streambuff_t<`r> stb_prepend(region_t<`r::TR> rgn, streambuff_t stb,
				    char ? @nozeroterm `RC nbuf)
  __attribute__((consume(3)));
  // prepends the given buffer to the streambuffer.  Creates aliases to
  // the existing databuf buffers.

extern int stb_num_bufs(streambuff_t stb);
extern int stb_with_buffer(streambuff_t stb, int idx,
			   int (@f)<`r::R>(`a env, char ? @nozeroterm `r),
			   `a env);


/*** for performance evaluation ***/

#ifdef NO_FREE
#define STB_FREE_UNIQUE(__x) stb_change_refcount((__x),-1)
#else
#define STB_FREE_UNIQUE(__x) stb_free_unique(__x)
#endif

#endif
