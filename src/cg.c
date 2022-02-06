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

#include <stdlib.h>

#include "nt_amd64.h"
#include "linux_amd64.h"

#include "parser.h"
#include "arch.h"

#include "cg.h"
#include "cg_amd64.h"


static int count_cmd(ast_t *a, int pos) {
  int n = 0;
  cmd_t c = a->c[pos];

  for (; pos < a->n && a->c[pos] == c; ++pos) {
    if (a->c[pos] == c) ++n;
    else break;
  }
  return n;
}

static unsigned long get_loop_end (ast_t *a, unsigned long pos) {
  /* This gets the index into a->c of the end of this loop.
     TODO: this is a very inefficient way to go about this: ideally
     a separate pass or worked in the parsing stage, this information
     should be collected. However, this program is of little importance
     so I shall not implement this now, but will go for the very naive
     and lazy approach.
   */
  int depth = 1;
  unsigned long i = pos + 1;
  for (; i < a->n && depth != 0; ++i) {
    if (a->c[i] == C_HHHH) --depth;
    else if (a->c[i] == C_hhhh) ++depth;
  }
  return i - 1;
}

static unsigned long get_loop_start (ast_t *a, unsigned long pos) {
  /* gets the index of the loop start (as above). This suffers the 
     same deplorable inefficiencies as get_loop_end.
   */
  int depth = 1;
  unsigned long i = pos - 1;
  for (; i > 0 && depth != 0; --i) {
    if (a->c[i] == C_HHHH) --depth;
    else if (a->c[i] == C_hhhh) ++depth;
  }
  return i + 1;
}

void compile_ast (FILE *fp, unsigned long memsize,
		 ast_t *a, target_t target) {
  // setup target correctly
  cg_t *cg = NULL;
  if (target == NT_AMD64) {
    fprintf(fp, "MEMSIZE equ %lu\n", memsize);
    fwrite(src_nt_amd64_asm, 1, src_nt_amd64_asm_len, fp);
    cg = amd64_get_cg();
  } else if (target == LINUX_AMD64) {
    fprintf(fp, "MEMSIZE equ %lu\n", memsize);
    fwrite(src_linux_amd64_asm, 1, src_linux_amd64_asm_len, fp);
    cg = amd64_get_cg();
  }

  // generate assembly
  for (unsigned long i = 0; i < a->n; ++i) {
    //printf("gen %d\n", a->c[i]);
    if (a->c[i] == C_hhhH) { // move ptr right
      int count = count_cmd(a, i);
      cg->move_ptr(fp, count);
      i += count - 1;
    } else if (a->c[i] == C_hhHh) { // move ptr left
      int count = count_cmd(a, i);
      cg->move_ptr(fp, -count);
      i += count - 1;
    } else if (a->c[i] == C_hhHH) { // print integer
      cg->print_int(fp);
    } else if (a->c[i] == C_hHhh) {
      cg->copy_ifz_or_set(fp, R1, i);
    } else if (a->c[i] == C_hHhH) {
      cg->copy_ifz_or_set(fp, R2, i);
    } else if (a->c[i] == C_hHHh) { // add memory to r1
      cg->add_mem_to_reg(fp, R1);
    } else if (a->c[i] == C_hHHH) { // add memory to r2
      cg->add_mem_to_reg(fp, R2);
    } else if (a->c[i] == C_Hhhh) { // read or write ascii character
      cg->ascii_io(fp);
    } else if (a->c[i] == C_HhhH) { // add to cell
      int count = count_cmd(a, i);
      cg->add_cell(fp, count);
      i += count - 1;
    } else if (a->c[i] == C_HhHh) { // subtract from cell
      int count = count_cmd(a, i);
      cg->add_cell(fp, -count);
      i += count - 1;
    } else if (a->c[i] == C_HhHH) { // read integer
      cg->read_int(fp);
    } else if (a->c[i] == C_HHhh) { // zero current cell
      cg->zero_cell(fp);
    } else if (a->c[i] == C_HHhH) { // double cell
      cg->double_cell(fp);
    } else if (a->c[i] == C_HHHh) { // square cell
      cg->square_cell(fp);
    } else if (a->c[i] == C_HHHH) { // start loop
      unsigned long j = get_loop_end(a, i);
      cg->label(fp, i);
      cg->loop(fp, j);
    } else if (a->c[i] == C_hhhh) { // end loop
      unsigned long j = get_loop_start(a ,i);
      cg->jmp(fp, j);
      cg->label(fp, i);
    } else if (a->c[i] == C_BREAKPOINT) {
      cg->dbg_break(fp);
    } else {
      fprintf(fp, "; ooga booga\n");
    }
  }
  cg->bye(fp);
}
