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

#include <core.h>
namespace Xarray {

using Core;

struct Xarray<`a::B> {
  `a ?elmts;
  int num_elmts;
};
typedef struct Xarray<`a> @xarray_t<`a>;

extern int length(xarray_t<`a>);

extern `a get(xarray_t<`a>, int);

extern void set(xarray_t<`a>, int, `a);

extern xarray_t<`a> create(int, `a);

extern xarray_t<`a> create_empty();

extern xarray_t<`a> singleton(int, `a);

extern void add(xarray_t<`a>, `a);

extern int add_ind(xarray_t<`a>, `a);

extern `a ?to_array(xarray_t<`a>);

extern xarray_t<`a> from_array(`a ?arr);

extern xarray_t<`a> append(xarray_t<`a>, xarray_t<`a>); // functional

extern void imp_append(xarray_t<`a>, xarray_t<`a>); // imperative

extern void app(`b f(`a), xarray_t<`a>);

extern void app_c(`b f(`c,`a), `c, xarray_t<`a>);

extern void iter(void f(`a), xarray_t<`a>);

extern void iter_c(void f(`b,`a), `b, xarray_t<`a>);

extern xarray_t<`b> map(`b f(`a), xarray_t<`a>);

extern xarray_t<`b> map_c(`b f(`c,`a), `c, xarray_t<`a>);

extern void reuse(xarray_t<`a> xarr);

// Delete the last num elements.
extern void delete(xarray_t<`a> xarr, int num);

// Remove the element at position i.
// Slide all higher elements down one position.
extern void remove(xarray_t<`a> xarr, int i); 

}

#endif

