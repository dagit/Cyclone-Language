#ifndef _XARRAY_H_
#define _XARRAY_H_

// Re-sizing arrays.  Ported with some modification by Dan from xarray.ml, 
// which was written by Steve

// Note these arrays never shrink in size.

#include "core.h"
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

