#ifndef TCFORMATSTR_H
#define TCFORMATSTR_H

#include "core.h"
#include "list.h"
#include "position.h"
#include "absyn.h"

namespace Formatstr {

// Returns the list of types corresponding to the specifiers in the string.
// Does all consistency checking w.r.t. the conversion specifiers
extern List::list<Absyn::typ> get_format_typs(string, Position::segment);

// Same as above, but the rules and types are different for scanf
extern List::list<Absyn::typ> get_scanf_typs(string, Position::segment);

// used internally by get_format_typs, but also by the translation to C.
// A cleaner interface would probably be a "format_to_c" function in
// Tcformatstr that Toc could call rather than passing back all the gunk.
extern Core::Opt_t<$(List::list<Char>, List::list<Char>, List::list<Char>, 
		     List::list<Char>, char, int)@>
parse_conversionspecification(string s, int i);

}

#endif
