/* assert.h:  this is defined following the suggestions of P.J.Plaugher's
 * "The Standard C Library".  
 */
#undef assert
#ifdef NDEBUG
#define assert(test) (0)
#else
#include "core.h"
namespace Assert{
extern `a AssertFail(string_t msg);
}
#define _STR(X) _VAL(X)
#define _VAL(X) #X
#define assert(test) ((test) ? 0 : \
  Assert::AssertFail(__FILE__ ":" _STR(__LINE__) " " #test))
#endif
