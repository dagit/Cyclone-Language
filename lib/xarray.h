#ifndef _XARRAY_H_
#define _XARRAY_H_

// Re-sizing arrays.  Ported with some modification by Dan from xarray.ml, 
// which was written by Steve

// Note these arrays never shrink in size.

#include "core.h"
namespace Xarray {

using Core;

struct xarray<`a> {
  `a ?elmts;
  int num_elmts;
};
typedef struct xarray<`a> @Xarray<`a>;

extern int length<`a>(Xarray<`a>);

extern `a get<`a>(Xarray<`a>, int);

extern void set<`a>(Xarray<`a>, int, `a);

extern Xarray<`a> create<`a>(int, `a);

extern Xarray<`a> create_empty<`a>();

extern Xarray<`a> singleton<`a>(int, `a);

extern void add<`a>(Xarray<`a>, `a);

extern int add_ind<`a>(Xarray<`a>, `a);

extern `a ?to_array<`a>(Xarray<`a>);

extern Xarray<`a> from_array<`a>(`a ?arr);

extern Xarray<`a> append<`a>(Xarray<`a>, Xarray<`a>); // functional

extern void imp_append<`a>(Xarray<`a>, Xarray<`a>); // imperative

extern void app<`a,`b>(`b f(`a), Xarray<`a>);

extern void app_c<`a,`b,`c>(`b f(`c,`a), `c, Xarray<`a>);

extern void iter<`a>(void f(`a), Xarray<`a>);

extern void iter_c<`a,`b>(void f(`b,`a), `b, Xarray<`a>);

extern Xarray<`b> map<`a,`b>(`b f(`a), Xarray<`a>);

extern Xarray<`b> map_c<`a,`b,`c>(`b f(`c,`a), `c, Xarray<`a>);

extern void reuse<`a>(Xarray<`a> xarr);

// Delete the last num elements.
extern void delete<`a>(Xarray<`a> xarr, int num);

// Remove the element at position i.
// Slide all higher elements down one position.
extern void remove<`a>(Xarray<`a> xarr, int i); 

}

#endif
