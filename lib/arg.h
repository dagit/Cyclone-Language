#ifndef ARG_H
#define ARG_H

#include "core.h"
#include "list.h"

namespace Arg {
extern xenum exn {Error, Bad(string)};

extern enum Spec {
  Unit_spec(void (@f)()),         // Call the function with unit argument
  Set_spec(bool@),                // Set the reference to true
  Clear_spec(bool@),              // Set the reference to false
  String_spec(void (@f)(string)), // Call the function with a string argument
  Int_spec(void (@f)(int)),       // Call the function with an int argument
  Rest_spec(void (@f)(string))    // Stop interpreting keywords and call the
                                  // function with each remaining argument
};

typedef enum Spec spec;

extern void usage (List::list<$(string,spec,string)@>, string);

extern int current;

extern void parse(List::list<$(string,spec,string)@> specs, 
		  void anonfun(string), string errmsg);

}

#endif
