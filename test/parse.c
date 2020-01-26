/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Parse test module.
 * @Date: 2019-10-21 19:09:51
 * @Last Modified: 2020-01-26 15:41:54
 */
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "prabsyn.h"
#include "errormsg.h"
#include "parse.h"
#include <stdio.h>

extern int yyparse(void);
extern A_exp absyn_root;

A_exp parse(string fname) {
  EM_reset(fname);
  if (yyparse() == 0) {
    // printf("Parsing successfully!\n");
    return absyn_root;
  }
  else return NULL;
}