#ifndef EARLEYBACKEND_H_
#define EARLEYBACKEND_H_

#include <list.h>
#include "semiring.h"
#include "earley-parsetree.h"
#include "dfa.h"


// Note that each namespace will have a subnamespace called DFA.

//namespace EarleyCycBackend{
//#include "earley-backend-priv.h"
//extern DFA::edfa_t init_dfa();
//}

namespace EarleyFsmBackend{
#include "earley-backend-priv.h"
}

namespace EarleyExtFsmBackend {
#include "earley-backend-priv.h"
}

//namespace EarleyExtCycBackend {
//#include "earley-backend-priv.h"
//}

namespace EarleyAnyBackend {
#include "earley-backend-priv.h"
}

#endif /*EARLEYBACKEND_H_*/
