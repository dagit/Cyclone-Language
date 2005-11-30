/*
 * Copyright (c) 1999-2004
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#ifndef REGIONS_H
#define REGIONS_H

#include "linkage.h"

EXTERN_C_BEGIN

#if (defined(RCPAIRS_FROM) || defined(RCPAIRS_ARRAY)) && !defined(RCPAIRS)
#define RCPAIRS
#endif

#ifdef RCPAIRS
/* With RCPAIRS, we record the reference count between every pair
   of regions.
   Regions are identified by a unique small id (< MAXREGIONS).
   zeroregion (globals, malloc, stack) has id 0.
   The number of references from region rfrom to region rto
   is stored in rto->rc[rfrom->id]
   (two possible alternatives:
    - store rc(rfrom, rto) in rfrom->rc[rto->id] (RCPAIRS_FROM)
    - store rc(rfrom, rto) in rcs[rfrom->id][rto->id]) (RCPAIRS_ARRAY)
*/
#define MAXREGIONS 32
#endif

#define RPAGELOG 11

typedef struct region *region;

#if defined(RCPAIRS_ARRAY)

typedef unsigned short regionid;
extern unsigned long __rcs[MAXREGIONS][MAXREGIONS];
extern region __rcregions[];
#define __RC(fromid, toid) (__rcs[fromid][toid])
#define __RCZERO(toid) (__rcs[0][toid])

#else

typedef region regionid;
extern struct region zeroregion;

#if defined(RCPAIRS_FROM)
#define __RC(fromid, toid) ((fromid)->rc[(toid)->id])
#define __RCZERO(toid) (zeroregion.rc[(toid)->id])

#elif defined(RCPAIRS)
#define __RC(fromid, toid) ((toid)->rc[(fromid)->id])
#define __RCZERO(toid) ((toid)->rc[0])
#else

#define __RC(fromid, toid) ((toid)->rc)
#define __RCZERO(toid) ((toid)->rc)

#endif
#endif

#if defined(__GNUC__) && defined(sparc)
extern regionid __rcregionmaparray[];
register regionid *__rcregionmap asm("%g6");
#else
extern regionid __rcregionmap[];
#endif

#include <stdlib.h>

typedef unsigned long __rcintptr;

struct ablock {
  char *base, *allocfrom;
};

struct allocator {
  struct ablock page;
  struct ablock superpage;
  struct ablock hyperpage;
  struct page *pages;
  struct page *bigpages;
};

struct region {
#ifdef RCPAIRS
#ifndef RCPAIRS_ARRAY
  unsigned long rc[MAXREGIONS];
#endif
  int id;
#else
  unsigned long rc;
#endif
  struct allocator normal;
  struct allocator atomic;
  __rcintptr bytes;
#ifdef DUPLICATES
  struct region *duplicates[DUPLICATES];
  struct region *base;
#endif

  struct region *parent, *sibling, *children;
  int rid, nextid;
};

extern region permanent;
void region_init(void);

region newregion(void);
region newsubregion(region parent);

typedef size_t adjust_fn(void *x, int by);
typedef adjust_fn *type_t;

/* rctypeof hack */
#define rctypeof(type) (void *)sizeof(type)

/* Low-level alloc with dynamic type info */
void *typed_ralloc(region r, size_t size, type_t type);
void *typed_rarrayalloc(region r, size_t n, size_t size, type_t type);
void *typed_rarrayextend(region r, void *old, size_t n, size_t size, type_t type);
void typed_rarraycopy(void *to, void *from, size_t n, size_t size, type_t type);

#define ralloc(r, type) typed_ralloc((r), sizeof(type), rctypeof(type))
#define rarrayalloc(r, n, type) typed_rarrayalloc((r), (n), sizeof(type), rctypeof(type))
#define rarrayextend(r, old, n, type) typed_rarrayextend((r), (old), (n), sizeof(type), rctypeof(type))
#define rarraycopy(to, from, n, type) typed_rarraycopy((to), (from), (n), sizeof(type), rctypeof(type))

char *rstralloc(region r, size_t size);
char *rstralloc0(region r, size_t size);
char *rstrdup(region r, const char *s);

/* rstrextend is used to extend an old string. The string MUST have been
   initially allocated by a call to rstrextend with old == NULL (you cannot
   initially allocate the string with rstralloc) */
char *rstrextend(region r, const char *old, size_t newsize);
char *rstrextend0(region r, const char *old, size_t newsize);

void deleteregion(region r) deletes;
void deleteregion_ptr(region *r) deletes;
void deleteregion_array(int n, region *regions) deletes;
region regionof(void *ptr);

typedef void (*nomem_handler)(void);
nomem_handler set_nomem_handler(nomem_handler newhandler);

#define regionidof(ptr) (__rcregionmap[(unsigned long)(ptr) >> RPAGELOG])

#ifdef SAFE_ADJUSTFN
#define RC_ADJUST_PREAMBLE regionid baseregion = regionof(p)

#define RC_ADJUST(x, by) \
{ \
  regionid __rcr1 = regionidof(x); \
 \
  if (__rcr1 != baseregion) \
    __RCZERO(__rcr1) += by; \
}
#else
#define RC_ADJUST_PREAMBLE
#define RC_ADJUST(x, by) (__RCZERO(regionidof(x)) += by)
#endif

#define RC_UPDATE(baseregion, oldval, newval) \
{ \
  regionid __rcr1 = regionidof(oldval), __rcr2 = regionidof(newval); \
 \
  if (__rcr1 != __rcr2) \
    { \
      if (__rcr1 != baseregion) \
        __RC(baseregion, __rcr1)--; \
      if (__rcr2 != baseregion) \
        __RC(baseregion, __rcr2)++; \
    } \
}



/* RC profiling support */
enum __rcpcat { pcat_none, pcat_null, pcat_trad, pcat_reg, pcat_same, pcat_parent };

extern unsigned long rcs_adjust_struct, rcs_adjust_array, rcs_adjust, 
  rcs_update_struct, rcs_update, rcs_update_same, rcs_update_traditional,
  rcs_global_struct, rcs_global, rcs_global_traditional, rcs_update_parent;
void rcs_profile(int pid, char *filename, int lineno,
		 int kind, enum __rcpcat cat1, enum __rcpcat cat2);
enum __rcpcat __rcptrcat(void *p);
enum __rcpcat __rcupdcat(void *p, void *c);

#if 0
register unsigned long perturb_counter asm("%g7");
#endif

void __rc_stick_adjust(void), __rc_etick_adjust(void);
void __rc_stick_update(void), __rc_etick_update(void);
void __rc_stick_global(void), __rc_etick_global(void);

/* Debugging support */

void findrefs(region r, void *from, void *to);

void findgrefs(region r);
void findrrefs(region r, region from);

/* Constant region ptr support */

#define MP(r, type) ralloc((r), type sameregion)
#define WP(x, v) (*(x) = (v))
#define RP(x) (*(x))


void check_child(region r, region of);

struct translation {
  region reg;
  void **map;
};

/*
  The interface for serialization/deserialization.
*/
typedef struct translation *translation;
typedef int (*Updater)(translation, void *);
void delete_translation(translation);
extern int serialize(region *r, char *datafile, char *statefile); 
extern translation deserialize(char *, char *, Updater *, region);
extern void update_pointer(translation, void **);
extern void *translate_pointer(translation, void *);

#define TRANSLATEPOINTER(m,a) ((*(m->map + (((unsigned int) a) >> SHIFT))) + (((unsigned int) a) & 0x00001FFF))
#define UPDATEPOINTER(map,loc) *(loc) = TRANSLATEPOINTER(map,loc)

EXTERN_C_END
#endif


