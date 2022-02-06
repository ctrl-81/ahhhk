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
#include <errno.h>

#include "parser.h"
#include "arch.h"
#include "cg.h"

const char help_str[] =
  "Usage: %s [options] <input>\n\n"
  "Options:\n"
  "--help    Display this message\n"
  "-o <o>    Set the output file\n"
  "-t <t>    Set the target\n"
  "-S        Compile only, don't assemble or link\n"
  "-c        Compile and assemble only, don't link\n"
  "\nAvailable Targets:\n"
  "  nt-x86-64, linux-x86-64\n\n";

char *get_contents(const char *file_name, unsigned long *bufsize) {
  FILE *fp = fopen(file_name, "rb");
  char *buf;
  if (fp == NULL) {
    fprintf(stderr, "fatal error: failed to open ``%s'': %s\n",
	    file_name, strerror(errno));
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  *bufsize = ftell(fp);
  rewind(fp);
  buf = calloc(*bufsize, 1);
  fread(buf, *bufsize, 1, fp);

  fclose(fp);
  return buf;
}

int main(int argc, char **argv) {
  const char *in_file = NULL;
  const char *out_file = NULL;
  target_t target = NO_TARGET;
  int compile_only = 0;
  int assemble_only = 0;
  char fmt_buf[4096];
  
  // parse arguments
  for (char **p = argv + 1; *p != NULL; ++p) {
    if (**p == '-') {
      if (!strcmp(*p, "-o")) {
	if (out_file != NULL) {
	  fprintf(stderr, "fatal error: an output file is already given\n");
	  return -1;
	}
	out_file = *++p;
	if (out_file == NULL) {
	  fprintf(stderr, "fatal error: expected output path\n");
	  return -1;
	}
      } else if (!strcmp(*p, "-t")) {
	if (target != NO_TARGET) {
	  fprintf(stderr, "fatal error: multiple targets given\n");
	  return -1;
	}
	++p;
	if (*p == NULL) {
	  fprintf(stderr, "fatal error: no target given\n");
	  return -1;
	} else if (!strcmp(*p, "nt-x86-64")) {
	  target = NT_AMD64;
	} else if (!strcmp(*p, "linux-x86-64")) {
	  target = LINUX_AMD64;
	} else {
	  fprintf(stderr, "fatal error: unrecognised target ``%s''\n", *p);
	  return -1;
	}
      } else if (!strcmp(*p, "-S")) {
	compile_only = 1;
      } else if (!strcmp(*p, "-c")) {
	assemble_only = 1;
      } else if (!strcmp(*p, "--help")) {
	fprintf(stderr, help_str, argv[0]);
	return 0;
      } else {
	fprintf(stderr,
		"fatal error: unrecognised command line argument\n");
	return -1;
      }
    } else {
      if (in_file != NULL) {
	fprintf(stderr, "fatal error: an input file is already given\n");
	return -1;
      }
      in_file = *p;
      if (in_file == NULL) {
	fprintf(stderr, "fatal error: expected input path\n");
	return -1;
      }
    }
  }

  if (in_file == NULL) {
    fprintf(stderr, "fatal error: no input\n");
    return -1;
  }

  if (target == NO_TARGET) {
    target = DEFAULT_TARGET;
  }

  // read input
  unsigned long bufsize;
  char *buf = get_contents(in_file, &bufsize);
  if (buf == NULL) {
    return -1;
  }

  // parse input
  ast_t ast = parse_aaah(buf, bufsize);
  free(buf);

  // open assembly file
  FILE *fp;
  if (assemble_only) {
    if (out_file == NULL) {
      fp = stdout;
    } else {
      fp = fopen(out_file, "wb");
      if (fp == NULL) {
	fprintf(stderr, "fatal error: failed to open output\n");
	return -1;
      }
    }
  } else {
    fp = fopen("a.asm", "wb");
    if (fp == NULL) {
      fprintf(stderr, "fatal error: failed to open temporary file\n");
      return -1;
    }
  }

  compile_ast(fp, 1000000, &ast, target);
  fclose(fp);
  free_ast(&ast);

  // assemble with nasm
  
  // TODO: ensure everything is escaped correctly
  if (compile_only) {
    if (target == NT_AMD64) {
      snprintf(fmt_buf, sizeof(fmt_buf),
	       "nasm -o \"%s\" -fwin64 a.asm",
	       out_file == NULL ? "a.o" : out_file);
    } else if (target == LINUX_AMD64) {
      snprintf(fmt_buf, sizeof(fmt_buf),
	       "nasm -o \"%s\" -felf64 a.asm",
	       out_file == NULL ? "a.o" : out_file);
    }
    system(fmt_buf);
    return 0;
  } else {
    if (target == NT_AMD64)
      system("nasm -o a.o -fwin64 a.asm");
    else if (target == LINUX_AMD64)
      system("nasm -o a.o -felf64 a.asm");
  }

  // link with ld (TODO: support other linkers)
  if (target == NT_AMD64) {
    snprintf(fmt_buf, sizeof(fmt_buf),
	     "ld a.o -o \"%s\" -nostdlib -e WinMain "
	     "C:\\Windows\\System32\\msvcrt.dll",
	     out_file == NULL ? "a.exe" : out_file);
  } else if (target == LINUX_AMD64) {
    snprintf(fmt_buf, sizeof(fmt_buf),
	     "ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -lc "
	     "a.o -o \"%s\" -e _start",
	     out_file == NULL ? "a.out" : out_file);
  }
  system(fmt_buf);

  // TODO: figure out linker args for Mac OS X

  remove("a.o");
  remove("a.asm");
  
  return 0;
}
