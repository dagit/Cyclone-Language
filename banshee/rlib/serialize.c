#include "regions.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


/*
  The region allocator keeps pages in two lists: REGULAR single page
  units and BIG multi-page units.
*/ 
enum page_kinds {
  REGULAR,
  BIG
};

/*
  A vestige of RC, each region has two allocators, one for NORMAL data
that can hold pointers and one for ATOMIC data that cannot.
*/ 
enum allocator_kinds {
  NORMAL,
  ATOMIC
};

/*
  The serialization routines write out two files: a dump of the
contents of each page, and metadata about the state of each page.
This state file consists of a sequence of page_state records, one for
each page in the order they appear in the page data file.
*/
struct page_state {
  int first_page;             /* is this the first page of the region? */
  int last_page;              /* is this the last page of the region? */
  enum allocator_kinds akind; /* normal or atomic page? A holdover
				 from RC, the allocator provides
				 faster routines for data without
				 pointers (atomic data). */
  enum page_kinds pkind;      /* regular or big page? */
  int size;                   /* page size in bytes */
  struct page *old_address;   /* the address of the page before serialization */
  int actual_size;            /* the number of bytes allocated on the page when the page was serialized. */
};

struct page_list {
  struct page *pg;
  struct page_list *next;
};

/*
  Makes a list of all the pages of a region in the reverse order
  listed in the region itself.  Reversing the list guarantees that the
  page containing the region object is first.
*/
struct page_list *make_page_list(struct page *p, region temp) {
  struct page_list *pgs = NULL, *n;
  for(; p != NULL; p = p->next) {
    n = (struct page_list *) rstralloc(temp, sizeof(struct page_list));
    n->next = pgs; 
    n->pg = p;
    pgs = n;
  }
  return pgs;
}
    
/*
  This routine serializes all the pages in a list of pages, writing
both the data file (a raw dump of the bits on the page) and the state
file (see description of page_state).

  The arguments are:
    A list of pages (pgs)

    The data and offset files to which the serailized pages are written

    A bit first_page: Is the first page in the list also the first
    page in the region?  The first page of a region contains the
     region object.

    A bit last_page: Is the last page in the list also the last page
    in the region?  We mark the last page so that we know when the
    next region is about to begin.
    
    Whether this page is a normal page or not (the alternative is an
    atomic page).
 
    What kind of page this page is: regular or big?
     
    The size of the page in bytes. 
*/
void serialize_pages(struct page_list *pgs, int data, int state, int first_page, int last_page, enum allocator_kinds akind, enum page_kinds pkind, int size, struct allocator *a) {
  struct page_state ps;
  int numbytes;
#ifndef NMEMDEBUG
  unsigned int used = 0, total = 0;
#endif
  for (; pgs != NULL; pgs = pgs->next) {
    ps.old_address = pgs->pg;
    ps.first_page = first_page; 
    first_page = 0;              /* only the first page in the list can possibly be the first page of the region */
    ps.last_page = (pgs->next == NULL) && last_page;
    ps.akind = akind;
    ps.pkind = pkind;
    ps.size = size  * ((pkind == BIG) ? pgs->pg->pagecount : 1);

    ps.actual_size = ps.size;

    if (a != NULL) {
      if ((a->page.base == (char *) pgs->pg) && (a->page.allocfrom < ((char *) pgs->pg) + ps.size))
	ps.actual_size = ((char *) a->page.allocfrom) - ((char *) pgs->pg);
      if ((a->superpage.base == (char *) pgs->pg) && (a->superpage.allocfrom < ((char *) pgs->pg) + ps.size))
	ps.actual_size = ((char *) a->superpage.allocfrom) - ((char *) pgs->pg);
      if ((a->hyperpage.base == (char *) pgs->pg) && (a->hyperpage.allocfrom < ((char *) pgs->pg) + ps.size))
	ps.actual_size = ((char *) a->hyperpage.allocfrom) - ((char *) pgs->pg);
    }

#ifndef NMEMDEBUG
   used += ((unsigned int) pgs->pg->available) - ((unsigned int) pgs->pg);
   total += ps.size;
#endif
    numbytes = write(data, pgs->pg, ps.size);
    if (numbytes != ps.size) {
      fprintf(stderr,"Serialization failed: Could not write full page.\n");
      exit(1);
    }
    numbytes = write(state, &ps, sizeof(struct page_state));
    if (numbytes != sizeof(struct page_state)) {
      fprintf(stderr,"Serialization failed: Could not write page state information.\n");
      exit(1);
    }
  }
#ifndef NMEMDEBUG
  if (total != 0)
    fprintf(stderr,"\tPage list of size %d bytes uses %d bytes = %0.2f\n", total, used, (float) used / (float) total);
#endif
}

/*
  Serializing a region consists of serializing all of its pages.  Each
  region has two allocators, "normal" for data that possibly contains
  pointers, and "atomic" for data that cannot hold pointers.
*/
int serialize_region(region r, int data, int state) { 
  region temp = newregion();
  struct page_list *normal = make_page_list(r->normal.pages, temp);
  struct page_list *atomic = make_page_list(r->atomic.pages, temp);
  struct page_list *big_normal = make_page_list(r->normal.bigpages, temp);
  struct page_list *big_atomic = make_page_list(r->atomic.bigpages, temp);

#ifndef NMEMBDEBUG
  fprintf(stderr,"Region: serializing pages (normal pages first, then big pages)\n");
#endif
  serialize_pages(normal, data, state, 1,     (atomic == NULL) && (big_normal == NULL) && (big_atomic == NULL), NORMAL, REGULAR, RPAGESIZE, &(r->normal));
  serialize_pages(atomic, data, state, 0,     (atomic != NULL) && (big_normal == NULL) && (big_atomic == NULL), NORMAL, REGULAR, RPAGESIZE, &(r->atomic));
  serialize_pages(big_normal, data, state, 0, (big_normal != NULL) && (big_atomic == NULL), NORMAL, BIG, RPAGESIZE, NULL);
  serialize_pages(big_atomic, data, state, 0, (big_atomic != NULL), NORMAL, BIG, RPAGESIZE, NULL);
  
  deleteregion(temp);
  return 0;
}


/*
  Serialiaze an array of regions.  Here r must be an array of region
  pointers, ending in a NULL pointer.
*/
int serialize(region *r, char *datafile, char *statefile) {
  int data = open(datafile, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
  int state = open(statefile, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
  if ((data == -1) || (state == -1)) return -1;
   
  for(; *r != NULL; r++) 
    serialize_region(*r, data, state);

  close(data);
  close(state);
  return 0;
}

/*
  Translating a serialized pointer to its new location after
deserialization consists of an array lookup using the high order bits
of the old address as the index.  Note that this implementation
requires that pages be aligned at addresses where the last RPAGELOG bits are 0's.
*/
inline void *translate_pointer(translation map, void *old_address) {
#ifndef NMEMDEBUG 
  if (old_address && (*(map->map + (((unsigned int) old_address) >> RPAGELOG)) == 0)) 
    fprintf(stderr,"Warning: The pointer %x has no translation.\n", (unsigned int) old_address);
#endif
  return (*(map->map + (((unsigned int) old_address) >> RPAGELOG))) + (((unsigned int) old_address) & (((unsigned int) 0xFFFFFFFF) >> (32 - RPAGELOG)));
}

void update_pointer(translation map, void **location) {
  *location = translate_pointer(map, *location);
}

/*
  Deserialization requires two passes.  In the first pass, implemented
by allocate_regions here, all the state records for the pages are
read, the regions reconstructed, and the pages for the regions
allocated without data.  This implementation depends on the fact that
all the pages for a region appear consecutively in the file, with the
first page containing the region object appearing first in the list.
*/

void allocate_regions(int state, translation map) {
  struct page_state ps;
  struct page *newpage;
  int num_pages;
  int numbytes;
  region r = NULL;

  for(;;) {
    numbytes = read(state, &ps, sizeof(struct page_state));
    if (numbytes == 0) return;
    if (numbytes != sizeof(struct page_state)) {
      fprintf(stderr,"Error: Could not populate page map.\n");
      exit(1);
    } 

    /* 
       There are a number of cases here.

       If the page is the first page in a new region, it will have a region object.
       Rather than allocate a new page, we allocate a new region in this case (which indirectly allocates the page).
       If the page is not the first page in a new region, we allocate it and attach it to the current region.
       The logic is slightly different depending on whether the page is normal or atomic. 
    */
    if (ps.first_page) {
	r = newregion();
	newpage = r->normal.pages;
	num_pages = 1;
    }
    else
      if (ps.akind == NORMAL)
	if (ps.pkind == REGULAR) {
	  num_pages = 1;
	  newpage = alloc_single_page(r->normal.pages);
	  r->normal.pages = newpage;
	}
	else
	  {
	    num_pages = ps.size / RPAGESIZE;
	    newpage = alloc_pages(num_pages, r->normal.bigpages);
	    r->normal.bigpages = newpage;
	  }
      else
	if (ps.pkind == REGULAR) {
	  num_pages = 1;
	  newpage = alloc_single_page(r->atomic.pages);
	  r->atomic.pages = newpage;
	}
	else
	  {
	    num_pages = ps.size / RPAGESIZE;
	    newpage = alloc_pages(num_pages, r->atomic.bigpages);
	    r->atomic.bigpages = newpage;
	  }

    set_region(newpage,num_pages,r);	  
    /* Now we record the address of the new page(s) as the translation of the address of the old page(s). */
    if (((((unsigned int) ps.old_address) >> RPAGELOG) << RPAGELOG) != (unsigned int) ps.old_address) {
      fprintf(stderr,"Pages are not aligned properly!\n");
      exit(1);
    }
    /*
    *(map->map + (((unsigned int) ps.old_address) >> RPAGELOG)) = 
      (void *) newpage;
    */
    for(; num_pages > 0; ) {
      num_pages--;
      *(map->map + ((((unsigned int) ps.old_address) + (num_pages * RPAGESIZE)) >> RPAGELOG)) = 
	(void *) (((char *) newpage) + (num_pages * RPAGESIZE));
     }

  }
}


/*
  Once the translation map for addresses is complete, the region
  objects can be updated to reflect the current addresses where
  allocation will occur.  This state is the only thing that needs to
  be update in the region objects.
*/
void update_ablock(translation map, struct ablock *a, struct ablock *old) {
  a->allocfrom = translate_pointer(map, (void *) old->allocfrom); 
}

void update_allocator(translation map, struct allocator *a, struct allocator *b) {
  update_ablock(map, &(a->page), &(b->page));
  /*  update_ablock(map, &(a->superpage), &(b->superpage));
  update_ablock(map, &(a->hyperpage), &(b->hyperpage)); */
}


/*
  Update all the pointers on a page to reflect the new locations after deserialization.
The update function is type-dependent and supplied by the user.  

The alignment to RALIGNMENT must be consistent with the way the allocator lays out data in the first
place.  This implementation is a bit simpler than what the region allocator does and may not work
for arrays if RALIGNMENT is anything other than a word boundary (4).

Also a bit of a hack here is that we get the size of the object being scanned from the update function,
but in reality we need to know the size before we perform the update in case the object would extend
past the end of the page (in which case it is not in fact an object to be updated).  However, every page
must be large enough to hold at least its first object, so this implementation is in fact correct.

*/
void update_page(char *current, char *end, translation map, Updater update) {
  int size = 0;
  for(;; ) {
    current = PALIGN(current, RALIGNMENT);
    if (current >= end) break;
    size = update(map, current);
    current += size;
  }
}

/*
  Deserializing page data is the most complex operation.  A page is a block of memory.  Serialization/deserialization
relies on a page having only one type of user data on it, but there is allocator management information stored on a page
which must also be updated and not overwritten by accident.  In particular, the start of the page always contains
a "struct page" object with all of the vital statistics of the page.  Note that the last field of a page struct can
(and is) used for user data, even though it has a name "previous" that suggests something else.  In fact this field is
only used for system bookkeeping when the page is on the free list.

If this page is the first page in a region, it will also have a region object immediately after the page object.

All of the pointer manipulation herein arises from the need to have a mix of information from the new page/region objects
and the serialized page/region objects wind up in the final page/region objects.
*/

void deserialize_pages(int data, int state, translation map, Updater *update) {
  struct page_state ps;
  struct page p, *newp, *save_previous;
  int numbytes = 0;
  char *mem, *save_available;
  struct region save_new_region, save_old_region;

  numbytes = read(state, &ps, sizeof(struct page_state));
  while (numbytes != 0) {
    if (numbytes != sizeof(struct page_state)) {
      fprintf(stderr,"Error: Could not read page state. Bytes read = %d; bytest desired = %li.\n",numbytes,sizeof(struct page_state));
      exit(1);
    }
    newp = (struct page *) translate_pointer(map, ps.old_address); 
    mem = ((char *) newp) + offsetof(struct page, previous); /* compute the first location of allocated data */

    /* save structure of the new page that should not be obliterated by reading the old page */
    p = *newp;               /* save the page structure */
    save_new_region = *((region) mem); /* save the region structure, just in case this is the first page of a region */ 
    numbytes = read(data, newp, ps.size);
    if (numbytes != ps.size) {
      fprintf(stderr,"Error: Could not read entire page.\n");
      exit(1);
    }
    /* 
       At this point we have overwritten the newly allocated page header with the old (serialized) header of the page.
       While neither the old nor new headers are completely correct at this point, the new header requires fewer
       pointer updates than the old header.  So, here we overwrite the old header with the new header again.  There are
       two wrinkles: The very last field of a page header is used only when the page is on the free list and is in fact
       the start of allocation when the page is in use.  So, we copy and restore that field from the serialized version
       so that we don't lose any data.  Also, we actually want the available field from the old page; we save it and translate it
       to show us how much of the new page has been allocated.
    */
    save_previous = newp->previous;  
    save_available = newp->available;
    *newp = p;                      /* restore the current page structure */
    newp->previous = save_previous; /* and restore any lost data */
    newp->available = translate_pointer(map, save_available);

    /* If this page has a region object, restore the region and update its allocation state. */
    if (ps.first_page) {

      /* The following line must be consistent with where the region is originally layed out in newsubregion(). */
      region r = (region) mem;
      save_old_region = *r;
      *r = save_new_region;
      /* update the allocation state of the region */
      update_allocator(map, &(r->normal), &(save_old_region.normal));
      /*      update_allocator(map, &(r->atomic), &(save_old_region.atomic)); */
      /* bump the allocation pointer past the region structure */
      mem += sizeof(struct region); 
    }

    /* 
       Now translate all pointers on the page. 
       If this is the last page in the region, move to the next update function for the next region. 
    */
    if (update != NULL) {
      /* update_page(mem, ((char *) newp) + ps.actual_size, map, *update); */
      update_page(mem, newp->available, map, *update); 
      if (ps.last_page) update++;
    }

    numbytes = read(state, &ps, sizeof(struct page_state));
  }
}
  
translation new_translation(region r) {
  struct translation *t = (struct translation *) rstralloc(r, sizeof(struct translation));
  t->reg = newregion();
  /* We rely on this map be zeroed out by the memory allocator. */
  t->map = (void **) rstralloc(t->reg, sizeof(void *) * (1 << (32 - RPAGELOG)));
  return t;
}

void delete_translation(translation t) {
  deleteregion(t->reg);
}

/*
  Deserialize a set of regions.  "update" must be an array of update functions, one for
each region to be read, in the same order as the regions appear in the data file.  There
is no need to NULL-terminate this array, as the processing stops with the last region in the
file.  However, disaster may strike if the update function array has a length other than
the number of regions in the file or the functions appear in the incorrect order.
*/
translation deserialize(char *datafile, char *statefile, Updater *update, region r) {
  struct translation *t = new_translation(r);
  int data = open(datafile, O_RDONLY);
  int state = open(statefile, O_RDONLY);
  if ((data == -1) || (state == -1)) 
    return NULL;

  if (RPAGESIZE != (1 << RPAGELOG)) {
    fprintf(stderr,"Error: Unexpected page size in deserialization\n");
  }
  
  allocate_regions(state, t);
  close(state);
  state = open(statefile, O_RDONLY);
  deserialize_pages(data, state, t, update);
  close(data);
  close(state);
  return t;
}
