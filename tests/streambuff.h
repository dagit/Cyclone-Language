#ifndef __STREAMBUFF_H__
#define __STREAMBUFF_H__

#include <core.h>

/** Defines **/

#define MAX_STB_PRIORITY 10

/** Types **/

struct DataBuff<`q::Q> {
  unsigned int ofs;
  char ? @nozeroterm @aqual(`q) buf;
};
typedef struct DataBuff<`q> databuf_t<`q>;

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
  databuf_t<REFCNT> bufs[numbufs];
};
typedef struct StreamBuff @@aqual(`q) streambuff_t<`q>;
typedef struct StreamBuff *@aqual(`q) streambuff_opt_t<`q>;
typedef struct StreamBuff @`r streambuff_r_t<`r>;
/** Functions **/

extern unsigned int stb_size(streambuff_t<`q> stb : RESTRICTED >= `q);
extern string_t stb_info(streambuff_t stb);
extern void stb_free(streambuff_t stb);
extern void stb_free_unique(streambuff_t<UNIQUE> stb) __attribute__((consume(1)));
  // stb_free frees the underlying buffers if unique; caller must free header
extern bool stb_noaliases(streambuff_t<`q> stb : RESTRICTED >= `q);
extern streambuff_t<`q> alloc_stb_with(aqual_t<`q> aqhdl,//region_t<`r::TR> rgnhdr,
				       enum StreamType typ,
				       const char ?`H device,
				       unsigned int priority,
				       databuf_t<REFCNT> data : RESTRICTED >= `q);
extern streambuff_t<`q> alloc_stb(aqual_t<`q> aqhdl,//region_t<`r::TR> rgnhdr,
				  enum StreamType typ,
				  const char ?`H device,
				  unsigned int priority,
				  const char ? @nozeroterm buf : RESTRICTED >= `q);
extern streambuff_t<`q> stb_copy(aqual_t<`q> aqh, //region_t<`r::TR> rgnhdr,
				 int ofs, streambuff_t<`q2> stb : RESTRICTED >= `q, RESTRICTED >= `q2);
extern streambuff_t<`q> stb_clone(aqual_t<`q> aqh, //region_t<`r::TR> rgnhdr,
				  streambuff_t<`q2> stb : RESTRICTED >= `q, RESTRICTED >= `q2);
  // pulls out the first buffer from the given streambuffer and
  // invokes the given function with it.  Takes care of swapping, etc.

extern streambuff_t<UNIQUE> stb_rest(int ofs, streambuff_t<UNIQUE> stb) 
  __attribute__((consume(2)));
  // we assume `U streambuffs for stb_rest, so that refcounts are
  // properly updated

extern streambuff_t<`q> stb_prepend(aqual_t<`q> aqh, 
				    streambuff_t<`q2> stb,
				    char ? @nozeroterm `RC nbuf : RESTRICTED >= `q, RESTRICTED >= `q2)
  __attribute__((consume(3)));
// prepends the given buffer to the streambuffer.  Creates aliases to
  // the existing databuf buffers.

extern int stb_num_bufs(streambuff_t<`q> stb : RESTRICTED >= `q);
extern int stb_with_buffer(streambuff_t<`q> stb, int idx,
			   int (@f)(`a env, char ? @nozeroterm `r),
			   `a env : RESTRICTED >= `q);


/*** for performance evaluation ***/

#ifdef NO_FREE
#define STB_FREE_UNIQUE(__x) stb_change_refcount((__x),-1)
#else
#define STB_FREE_UNIQUE(__x) stb_free_unique(__x)
#endif

#endif
