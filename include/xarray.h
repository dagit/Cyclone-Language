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

#ifndef _XARRAY_H_
#define _XARRAY_H_

// Re-sizing arrays.  Ported with some modification by Dan from xarray.ml,
// which was written by Steve

// Note these arrays never shrink in size.

namespace Xarray {

  /*** \subsection{\texttt{<xarray.h>}} */
  /*** Defines namespace Xarray, which implements a datatype of
       extensible arrays. */

struct Xarray<`a::B> {
  `a ?`U elmts;
  int num_elmts;
};

typedef struct Xarray<`a> @`r xarray_t<`a,`r>;
  /** An [xarray_t] is an extensible array. */
extern int length(xarray_t<`a>);
  /** [length(a)] returns the length of extensible array [a]. */
extern `a get(xarray_t<`a>, int);
  /** [get(a,n)] returns the [n]th element of [a], or throws
      [Invalid_argument] if [n] is out of range. */
extern void set(xarray_t<`a>, int, `a);
  /** [set(a,n,v)] sets the [n]th element of [a] to [v], or throws
      [Invalid_argument] if [n] is out of range. */
extern xarray_t<`a> create(int, `a);
  /** [create(n,v)] returns a new extensible array with starting size [n]
      and default value [v]. */
extern xarray_t<`a,`r> rcreate(region_t<`r>, int, `a);
  /** [rcreate(r,n,v)] returns a new extensible array with starting size [n]
      and default value [v] in region [r]. */
extern xarray_t<`a> create_empty();
  /** [create_empty()] returns a new extensible array with starting
      size 0. */
extern xarray_t<`a,`r> rcreate_empty(region_t<`r>);
  /** [rcreate_empty(r)] returns a new extensible array with starting
      size 0 in region [r]. */
extern xarray_t<`a> singleton(int, `a);
  /** [singleton(n,v)] returns a new extensible array with a single
      element [v]. */
extern xarray_t<`a,`r> rsingleton(region_t<`r>, int, `a);
  /** [rsingleton(r,n,v)] returns a new extensible array with a single
      element [v] in region [r]. */
extern void add(xarray_t<`a>, `a);
  /** [add(a,v)] makes the extensible array larger by adding [v] to
      the end. */
extern int add_ind(xarray_t<`a>, `a);
  /** [add_ind(a,v)] makes [a] larger by adding [v] to the end, and
      returns [v]. */
extern `a ?to_array(xarray_t<`a>);
  /** [to_array(a)] returns a normal (non-extensible) array with the
      same elements as [a]. */
extern `a ?`r rto_array(region_t<`r>, xarray_t<`a>);
  /** [rto_array(a,r)] returns a normal (non-extensible) array with the
      same elements as [a] allocated in region [r]. */
extern xarray_t<`a> from_array(`a ?arr);
  /** [from_array(a)] returns an extensible array with the same
      elements as the normal (non-extensible) array [a]. */
extern xarray_t<`a,`r> rfrom_array(region_t<`r>, `a ?arr);
  /** [rfrom_array(r,a)] returns an extensible array with the same
      elements as the normal (non-extensible) array [a], allocated 
      in region [r]. */
extern xarray_t<`a> append(xarray_t<`a>, xarray_t<`a>);
  /** [append(a1,a2)] returns a new extensible array whose elements
      are the elements of [a1] followed by [a2].  The inputs [a1] and
      [a2] are not modified. */
extern xarray_t<`a,`r> rappend(region_t<`r>, xarray_t<`a>, xarray_t<`a>);
  /** [rappend(r,a1,a2)] returns a new extensible array whose elements
      are the elements of [a1] followed by [a2], allocated in region [r].  
      The inputs [a1] and [a2] are not modified. */
extern void app(`b f(`a), xarray_t<`a>);
  /** [app(f,a)] applies [f] to each element of [a], in order
      from lowest to highest.  Note that [f] returns [`a], unlike
      with [iter]. */
extern void app_c(`b f(`c,`a), `c, xarray_t<`a>);
  /** [app_c(f,e,a)] applies [f] to [e] and each element of [a], in
      order from lowest to highest. */
extern void iter(void f(`a), xarray_t<`a>);
  /** [iter(f,a)] applies [f] to each element of [a], in order
      from lowest to highest.  Note that [f] returns [void], unlike
      with [app]. */
extern void iter_c(void f(`b,`a), `b, xarray_t<`a>);
  /** [iter_c(f,e,a)] applies [f] to [e] and each element of [a], in
      order from lowest to highest. */
extern xarray_t<`b> map(`b f(`a), xarray_t<`a>);
  /** [map(f,a)] returns a new extensible array whose elements are
      obtained by applying [f] to each element of [a]. */
extern xarray_t<`b,`r> rmap(region_t<`r>,`b f(`a), xarray_t<`a>);
  /** [rmap(r,f,a)] returns a new extensible array whose elements are
      obtained by applying [f] to each element of [a], and allocated 
      in region [r]. */
extern xarray_t<`b> map_c(`b f(`c,`a), `c, xarray_t<`a>);
  /** [map_c(f,e,a)] returns a new extensible array whose elements are
      obtained by applying [f] to [e] and each element of [a]. */
extern xarray_t<`b,`r> rmap_c(region_t<`r>, `b f(`c,`a), `c, xarray_t<`a>);
  /** [rmap_c(r,f,e,a)] returns a new extensible array whose elements are
      obtained by applying [f] to [e] and each element of [a].  The result
      is allocated in region [r]. */
extern void reuse(xarray_t<`a> xarr);
  /** [reuse(a)] sets the number of elements of [a] to zero, but
      does not free the underlying array. */
extern void delete(xarray_t<`a> xarr, int num);
  /** [delete(a,n)] deletes the last [n] elements of [a]. */
extern void remove(xarray_t<`a> xarr, int i);
  /** [remove(a,i)] removes the element at position [i] from [a];
      elements at positions greater than [i] are moved down one position. */

}

#endif
