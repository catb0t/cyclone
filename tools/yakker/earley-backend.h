#ifndef EARLEYBACKEND_H_
#define EARLEYBACKEND_H_

#include <list.h>
#include "semiring.h"
#include "earley-parsetree.h"
#include "dfa.h"
#include "ykbuf.h"

namespace EarleyAnyBackend {
extern $(List::list_t<$(EarleyParsetree::parse_tree_t,Semiring::weight_t)@>,int,int)
parse(EarleyAnyBackend::DFA::edfa_t dfa_rep, const char? input);

extern $(List::list_t<$(EarleyParsetree::parse_tree_t,Semiring::weight_t)@>,int,int)
ykb_parse(EarleyAnyBackend::DFA::edfa_t dfa_rep, ykbuf_t@ ykb, int parse_until_eof);
}

#endif /*EARLEYBACKEND_H_*/
