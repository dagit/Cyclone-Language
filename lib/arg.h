#ifndef _ARG_H_
#define _ARG_H_

#include "list.h"
#include "core.h"

namespace Arg {
extern xtunion exn {extern Error, extern Bad(string)};

extern tunion Spec<`e::E> {
  Unit_spec(void (@f)(;`e)),        // Call f with unit argument
  Flag_spec(void (@f)(string;`e)),  // Call f with flag argument
  FlagString_spec(void (@f)(string,string;`e)),
                                    // Call f with flag & string arguments
  Set_spec(bool@),                  // Set the reference to true
  Clear_spec(bool@),                // Set the reference to false
  String_spec(void (@f)(string;`e)),// Call f with a string argument
  Int_spec(void (@f)(int;`e)),      // Call f with an int argument
  Rest_spec(void (@f)(string;`e))   // Stop interpreting keywords and call the
                                    // function with each remaining argument
};

typedef tunion Spec<`e> gspec_t<`e>;
typedef tunion Spec<{}> spec_t;

extern void usage (List::glist_t<$(string,gspec_t<`e>,string)@`r1,`r2>,string);

extern int current;

extern void parse(List::glist_t<$(string,gspec_t<`e>,string)@`r1,`r2> specs, 
		  void anonfun(string), string errmsg, string ? args);

}

#endif
