#ifndef CG_H
#define CG_H

#include <stdio.h>

typedef enum {
  R1, R2
} reg_t;

typedef struct {
  int n;
  void (*label)(FILE*, unsigned long);
  void (*move_ptr)(FILE*, int);
  void (*print_int)(FILE *);
  void (*copy_ifz_or_set)(FILE*, reg_t, unsigned long);
  void (*add_mem_to_reg)(FILE*, reg_t);
  void (*ascii_io)(FILE *);
  void (*add_cell)(FILE *, int);
  void (*read_int)(FILE*);
  void (*zero_cell)(FILE*);
  void (*double_cell)(FILE*);
  void (*square_cell)(FILE*);
  void (*loop)(FILE*, unsigned long);
  void (*jmp)(FILE*, unsigned long);
  void (*bye)(FILE*);
  void (*dbg_break)(FILE*);
} cg_t;

#if defined(ARCH_H) && defined(PARSER_H)
void compile_ast(FILE *fp, unsigned long memsize, ast_t *a, target_t target);
#endif

#endif // CG_H
