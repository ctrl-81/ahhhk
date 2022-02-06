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
#include <stdlib.h>
#include "cg.h"

const char *get_reg_name (reg_t r) {
  return r == R1 ? "r12" : "r13";
}

static void label (FILE *fp, unsigned long n) {
  fprintf(fp, "__loc_%lu:\n", n);
}

static void move_ptr (FILE *fp, int n) {
  // sizeof(cell) = 8
  if (n == 1) {
    fprintf(fp, "\tadd r14, 8\n");
  } else if (n == - 1) {
    fprintf(fp, "\tsub r14, 8\n");
  } else if (n < 0) {
    fprintf(fp, "\tsub r14, %d\n", -n * 8);
  } else {
    fprintf(fp, "\tadd r14, %d\n", n * 8);
  }
}

static void print_int (FILE *fp) {
  fprintf(fp, "\tcall _puti\n");
}

static void copy_ifz_or_set (FILE *fp, reg_t r, unsigned long loc) {
  /* If register r is empty, copy the current memory cell into r,
      otherwise write the value of r to the memory cell 
  */
  label(fp, loc);
  if (r == R1) {
    fprintf(fp,
	    "\ttest bh, bh\n"
	    "\tjz .no_copy\n"
	    "\tmov [r14], r12\n"
	    "\txor r12, r12\n"
	    "\txor bh, bh\n"
	    "\tjmp .done\n"
	    ".no_copy:\n"
	    "\tmov r12, [r14]\n"
	    "\tmov bh, 1\n"
	    ".done:\n");
  } else if (r == R2) {
    fprintf(fp,
	    "\ttest bl, bl\n"
	    "\tjz .no_copy\n"
	    "\tmov [r14], r13\n"
	    "\txor r13, r13\n"
	    "\txor bl, bl\n"
	    "\tjmp .done\n"
	    ".no_copy:\n"
	    "\tmov r13, [r14]\n"
	    "\tmov bl, 1\n"
	    ".done:\n");
  }
}

static void add_mem_to_reg (FILE *fp, reg_t r) {
  fprintf(fp,
	  "\tadd %s, [r14]\n",
	  get_reg_name(r));
}

static void ascii_io (FILE *fp) {
  fprintf(fp, "\tcall _ascii_io\n");
}

static void add_cell (FILE *fp, int n) {
  if (n == 1) {
    fprintf(fp, "\tinc qword [r14]\n");
  } else if (n == -1) {
    fprintf(fp, "\tdec qword [r14]\n");
  } else if (n < 0) {
    fprintf(fp, "\tsub qword [r14], %d\n", -n); 
  } else {
    fprintf(fp, "\tadd qword [r14], %d\n", n);
  }
}

static void read_int(FILE *fp) {
  fprintf(fp, "\tcall _readi\n");
}

static void zero_cell(FILE *fp) {
  fprintf(fp,
	  "\txor eax, eax\n"
	  "\tmov [r14], rax\n");
}

static void double_cell(FILE *fp) {
  fprintf(fp,
	  "\tmov rax, [r14]\n"
	  "\tadd rax, rax\n"
	  "\tmov [r14], rax\n");
}

static void square_cell(FILE *fp) {
  fprintf(fp,
	  "\tmov rax, [r14]\n"
	  "\timul rax, rax\n"
	  "\tmov [r14], rax\n");
}

static void loop(FILE *fp, unsigned long j) {
  fprintf(fp,
	  "\tmov rax, [r14]\n"
	  "\ttest rax, rax\n"
	  "\tjz __loc_%lu\n", j);
}

static void jmp(FILE *fp, unsigned long j) {
  fprintf(fp, "\tjmp __loc_%lu\n", j);
}

static void bye(FILE *fp) {
  fprintf(fp, "\tcall _bye\n");
}

static void dbg_break(FILE *fp) {
  fprintf(fp, "\tint 3\n");
}
  

cg_t cg_amd64 = {
  .label = label,
  .move_ptr = move_ptr,
  .print_int = print_int,
  .copy_ifz_or_set = copy_ifz_or_set,
  .add_mem_to_reg = add_mem_to_reg,
  .ascii_io = ascii_io,
  .add_cell = add_cell,
  .read_int = read_int,
  .zero_cell = zero_cell,
  .double_cell = double_cell,
  .square_cell = square_cell,
  .loop = loop,
  .jmp = jmp,
  .bye = bye,
  .dbg_break = dbg_break
};

cg_t *amd64_get_cg(void) {
  return &cg_amd64;
}
