#ifndef SYNTAX_H
#define SYNTAX_H

#ifdef SYNTAX_CYC
#define SYNTAX_EXTERN_DEFINITION
#else
#define SYNTAX_EXTERN_DEFINITION extern
#endif

#include "list.h"

namespace Syntax {
using List {
  
SYNTAX_EXTERN_DEFINITION struct Location {
  int start_pos;
  int end_pos;
  int start_line;
  int start_col;
};
typedef struct Location @ location_t;

SYNTAX_EXTERN_DEFINITION tunion Regular_expression {
  Epsilon;
  Characters(list_t<int>);
  Sequence(tunion Regular_expression, tunion Regular_expression);
  Alternative(tunion Regular_expression, tunion Regular_expression);
  Repetition(tunion Regular_expression);
};
typedef tunion Regular_expression regular_expression_t;

typedef $(regular_expression_t,location_t) @ acase_t;
typedef $(string_t,list_t<acase_t>) @ entrypoint_t;

SYNTAX_EXTERN_DEFINITION struct Lexer_definition {
  location_t         header;
  list_t<entrypoint_t> entrypoints;
  location_t         trailer;
};
typedef struct Lexer_definition * lexer_definition_t;

}}

#endif
