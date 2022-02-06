#ifndef PARSER_H
#define PARSER_H

typedef unsigned char cmd_t;

#define C_hhhh 0
#define C_hhhH 1
#define C_hhHh 2
#define C_hhHH 3
#define C_hHhh 4
#define C_hHhH 5
#define C_hHHh 6
#define C_hHHH 7
#define C_Hhhh 8
#define C_HhhH 9
#define C_HhHh 10
#define C_HhHH 11
#define C_HHhh 12
#define C_HHhH 13
#define C_HHHh 14
#define C_HHHH 15
#define C_AHHH 16
#define C_NEWLINE 17
#define C_BREAKPOINT 0xff 

typedef struct ast {
  unsigned long n;
  cmd_t *c;
} ast_t;

ast_t parse_aaah (char *buf, unsigned long bufsize);
void free_ast(ast_t *a);

#endif // PARSER_H
