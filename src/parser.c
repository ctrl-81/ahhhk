/* Compiler for the AHHH programming language
   Copyright (c) 2022, Charles T. Lewis

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "parser.h"

static void add_cmd (ast_t *a, cmd_t c) {
  unsigned long i = a->n++;
  a->c = realloc(a->c, sizeof(cmd_t) * a->n);
  a->c[i] = c;
}

ast_t parse_aaah (char *buf, unsigned long bufsize) {
  /** Parses AAAH source code, returning a (not very) abstract syntax tree.
      If something other than an AAAH command is encountered (excluding
      whitespace), then the rest of the line is skipped.
   */
  ast_t a = {0, NULL};
  int skip_line = 0;
  
  for (char *p = buf; p <= buf + bufsize - 3; ++p) {
    if (skip_line) {
      skip_line = p[0] != '\n';
    } else if (toupper(p[0]) == 'H' && toupper(p[1]) == 'H'
	&& toupper(p[2]) == 'H') {
      if (toupper(p[3]) == 'H') {
	cmd_t c = (cmd_t)(!!isupper(p[3]));
	c |= (cmd_t)(!!isupper(p[2])) << 1;
	c |= (cmd_t)(!!isupper(p[1])) << 2;
	c |= (cmd_t)(!!isupper(p[0])) << 3;
	add_cmd(&a, c);
	p += 3;
      } else if (p[3] == '!') {
	add_cmd(&a, C_NEWLINE);
	p += 3;
      } else {
	skip_line = 1;
      }
    } else if (p[0] == 'A' && p[1] == 'H' && p[2] == 'H' && p[3] == 'H') {
      add_cmd(&a, C_AHHH);
      p += 3;
    } else if (p[0] == 'i' && p[1] == 'n' && p[2] == 't' && p[3] == '3') {
      add_cmd(&a, C_BREAKPOINT);
      p += 3;
    } else if (!isspace(p[0])) {
      skip_line = 1;
    }
  }
  return a;
}

void free_ast(ast_t *a) {
  a->n = 0;
  free(a->c);
  a->c = NULL;
}
