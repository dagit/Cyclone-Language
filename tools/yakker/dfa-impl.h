#ifdef USE_COMPILED_DFA
#include "dfa_cyc.h"

#elif defined(USE_FSM_DFA)
#include "dfa_fsm.h"

#elif defined(USE_EXT_DFA)
#include "dfa_ext.h"

#else
#error "No DFA implementation specified."
#endif
