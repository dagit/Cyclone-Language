#ifndef _XARRAY_H_
#define _XARRAY_H_

// Re-sizing arrays.  Ported with some modification by Dan from xarray.ml, 
// which was written by Steve

// Note these arrays never shrink in size.

#include "core.h"
namespace Xarray {

using Core;

struct xarray<`a::B> {
  `a ?elmts;
  int num_elmts;
};
typedef struct xarray<`a> @Xarray<`a>;

extern int length(Xarray<`a>);

extern `a get(Xarray<`a>, int);

extern void set(Xarray<`a>, int, `a);

extern Xarray<`a> create(int, `a);

extern Xarray<`a> create_empty();

extern Xarray<`a> singleton(int, `a);

extern void add(Xarray<`a>, `a);

extern int add_ind(Xarray<`a>, `a);

extern `a ?to_array(Xarray<`a>);

extern Xarray<`a> from_array(`a ?arr);

extern Xarray<`a> append(Xarray<`a>, Xarray<`a>); // functional

extern void imp_append(Xarray<`a>, Xarray<`a>); // imperative

extern void app(`b f(`a), Xarray<`a>);

extern void app_c(`b f(`c,`a), `c, Xarray<`a>);

extern void iter(void f(`a), Xarray<`a>);

extern void iter_c(void f(`b,`a), `b, Xarray<`a>);

extern Xarray<`b> map(`b f(`a), Xarray<`a>);

extern Xarray<`b> map_c(`b f(`c,`a), `c, Xarray<`a>);

extern void reuse(Xarray<`a> xarr);

// Delete the last num elements.
extern void delete(Xarray<`a> xarr, int num);

// Remove the element at position i.
// Slide all higher elements down one position.
extern void remove(Xarray<`a> xarr, int i); 

}

#endif
