/* Copyright (C) 2005 Greg Morrisett, AT&T.
   This file is part of the Cyclone project.

   Cyclone is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   Cyclone is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Cyclone; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */

#ifndef PARSERGEN_H
#define PARSERGEN_H
#include "bnf.h"
extern void gen(grammar_t<`H> grm, const char ?`H symb,
                List::list_t<const char ?@>textblobs, 
		int all_start, unsigned int eof_val);

/* generate a code fragment.*/
extern void gen_frag(rule_t r);

/* Print forward definitions of parsing functions in grammar grm. */
extern void gen_header(grammar_t<`H> grm, List::list_t<const char ?@>textblobs);

extern int print_main;
extern int print_globals;
extern int lazyfill;
extern int local_lookahead;

#endif
