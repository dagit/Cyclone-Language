#ifndef _ARG_H_
#define _ARG_H_

#include "list.h"
#include "core.h"

namespace Arg {
extern xenum exn {Error, Bad(string)};

extern enum Spec<`e> {
  Unit_spec(void (@f)(;`e)),        // Call the function with unit argument
  Set_spec(bool@),                  // Set the reference to true
  Clear_spec(bool@),                // Set the reference to false
  String_spec(void (@f)(string;`e)),// Call the function with a string argument
  Int_spec(void (@f)(int;`e)),      // Call the function with an int argument
  Rest_spec(void (@f)(string;`e))   // Stop interpreting keywords and call the
                                    // function with each remaining argument
};

typedef enum Spec<`e> gspec_t<`e>;
typedef enum Spec<{}> spec_t;

extern void usage (List::glist_t<$(string,gspec_t<`e>,string)@`r1,`r2>,string);

extern int current;

extern void parse(List::glist_t<$(string,gspec_t<`e>,string)@`r1,`r2> specs, 
		  void anonfun(string), string errmsg);

}

#endif
