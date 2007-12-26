#ifdef USE_COMPILED_DFA
#include "dfa_cyc.h"

#elif defined(USE_FSM_DFA)
#include "dfa_fsm.h"

#elif defined(USE_EXT_DFA)
#ifndef DFA_EXT_H_
#define DFA_EXT_H_

#define EXT_DFA_NAMESPACE ExtDFA
#define EB_NAMESPACE EarleyFsmBackend
#define EB_EXT_NAMESPACE EarleyExtFsmBackend
#include "dfa_ext.h"

#endif /*DFA_EXT_H_*/

#elif defined(USE_EXT_CYC_DFA)
#ifndef DFA_EXT_CYC_H_
#define DFA_EXT_CYC_H_

#define EXT_DFA_NAMESPACE ExtCycDFA
#define EB_NAMESPACE EarleyCycBackend
#define EB_EXT_NAMESPACE EarleyExtCycBackend
#include "dfa_ext.h"

#endif /*DFA_EXT_CYC_H_*/

#else
#error "No DFA implementation specified."
#endif
