/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Parse test module.
 * @Date: 2019-10-21 19:09:51
 * @Last Modified: 2020-01-08 22:35:22
 */
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "prabsyn.h"
#include "errormsg.h"
#include "parse.h"

extern int yyparse(void);
extern A_exp absyn_root;

A_exp parse(string fname) {
  EM_reset(fname);
  if (yyparse() == 0) {
    printf("Parsing successfully!\n");
    return absyn_root;
  }
  else return NULL;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {printf("Please give the file to parse!\n"); exit(0);}
  A_exp pr_ret = parse(argv[1]);
  pr_exp(stdout, pr_ret, 0);
  return 0;
}