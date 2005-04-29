#ifndef _BGET_H
#define _BGET_H
/*

    Interface definitions for bget.c, the memory management package.

*/

#define  _(x)  x		      /* gcc knows prototypes */

//
typedef long bufsize;


// void	bpool	    _((void *buffer, bufsize len));
// void   *bget	    _((bufsize size));
// void   *bgetz	    _((bufsize size));
// void   *bgetr	    _((void *buffer, bufsize newsize));
// void	brel	    _((void *buf));
// void	bectl	    _((int (*compact)(bufsize sizereq, int sequence),
// 		       void *(*acquire)(bufsize size),
// 		       void (*release)(void *buf), bufsize pool_incr));
// void	bstats	    _((bufsize *curalloc, bufsize *totfree, bufsize *maxfree,
// 		       long *nget, long *nrel));
// void	bstatse     _((bufsize *pool_incr, long *npool, long *npget,
// 		       long *nprel, long *ndget, long *ndrel));
// void	bufdump     _((void *buf));
// void	bpoold	    _((void *pool, int dumpalloc, int dumpfree));
// int	bpoolv	    _((void *pool));

struct bget_region;

struct bget_region_key {
  struct bget_region *reg;
  void *priv;
};

typedef struct bget_region_key *bget_rgn_key_t;
unsigned int bget_get_static_overhead();
bget_rgn_key_t  bget_init_region  _((void *buffer, bufsize len));
void           bget_drop_region  _((bget_rgn_key_t));
void	       bpool	    _((bget_rgn_key_t, void *buffer, bufsize len));
void           *bget	    _((bget_rgn_key_t, bufsize size));
void           *bgetz	    _((bget_rgn_key_t, bufsize size));
void           *bgetr	    _((bget_rgn_key_t, void *buffer, bufsize newsize));
void	       brel	    _((bget_rgn_key_t, void *buf));
void	       bectl	    _((bget_rgn_key_t,
			       int (*compact)(bget_rgn_key_t, bufsize sizereq, int sequence),
			       void *(*acquire)(void*, bufsize size, bufsize *incr),
			       void (*release)(bget_rgn_key_t, void *buf), bufsize pool_incr));
void	       bstats	    _((bget_rgn_key_t, bufsize *curalloc, bufsize *totfree, 
			       bufsize *maxfree, long *nget, long *nrel));
void	       bstatse     _((bget_rgn_key_t, bufsize *pool_incr, long *npool, long *npget,
			      long *nprel, long *ndget, long *ndrel));
void	       bufdump     _((bget_rgn_key_t, void *buf));
void	       bpoold	    _((bget_rgn_key_t, void *pool, int dumpalloc, int dumpfree));
int	       bpoolv	    _((bget_rgn_key_t, void *pool));
bufsize bget_size(bget_rgn_key_t regkey, void *buf);
#endif
