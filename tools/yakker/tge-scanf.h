#ifndef TGESCANF_H_
#define TGESCANF_H_

#include <core.h>

#include "dfa.h"
#include "pm_bnf.h"
#include "earley-parsetree.h"

typedef rule_pat_t (@`H fmt_crawl_fun)<`r>(EarleyParsetree::parse_tree_t, string_t<`r> fmt_str);

int internal_scanf (EarleyAnyBackend::DFA::edfa_t dfa_rep, fmt_crawl_fun fmt_crawl,
    string_t input, string_t format_string, string_t<`H> @ ?args);

#endif /*TGESCANF_H_*/
