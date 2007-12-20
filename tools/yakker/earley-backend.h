#ifndef EARLEYBACKEND_H_
#define EARLEYBACKEND_H_

#include <list.h>
#include "semiring.h"
#include "earley-parsetree.h"


// Note that each namespace will have a subnamespace called DFA.

#define USE_COMPILED_DFA
#include "dfa.h"
namespace EarleyCycBackend{
#include "earley-backend-priv.h"
extern DFA::edfa_t init_dfa();
}
#undef USE_COMPILED_DFA

#define USE_FSM_DFA
#include "dfa.h"
namespace EarleyFsmBackend{
#include "earley-backend-priv.h"
extern DFA::edfa_t init_dfa();
}
#undef USE_FSM_DFA

#define USE_EXT_DFA
#include "dfa.h"
namespace EarleyExtBackend {
#include "earley-backend-priv.h"
}
#undef USE_EXT_DFA


#endif /*EARLEYBACKEND_H_*/
