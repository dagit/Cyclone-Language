#ifndef _FORMATSTR_H_
#define _FORMATSTR_H_

#include "core.h"
#include "list.h"
#include "position.h"
#include "absyn.h"

namespace Formatstr {

// Returns the list of types corresponding to the specifiers in the string.
// Does all consistency checking w.r.t. the conversion specifiers
extern List::list_t<Absyn::type_t> get_format_typs(string, Position::seg_t);

// Same as above, but the rules and types are different for scanf
extern List::list_t<Absyn::type_t> get_scanf_typs(string, Position::seg_t);

// used internally by get_format_typs, but also by the translation to C.
// A cleaner interface would probably be a "format_to_c" function in
// Tcformatstr that Toc could call rather than passing back all the gunk.
extern Core::opt_t<$(List::list_t<int>, List::list_t<int>, List::list_t<int>, 
		     List::list_t<int>, char, int)@>
parse_conversionspecification(string s, int i);

}

#endif
