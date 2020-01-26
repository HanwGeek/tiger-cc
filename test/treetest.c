/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Tree print test module.
 * @Date: 2020-01-13 16:16:29
 * @Last Modified: 2020-01-26 14:31:38
 */
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "temp.h" 
#include "tree.h" 
#include "frame.h" 
#include "semant.h"
#include "canon.h"
#include "prabsyn.h"
#include "printtree.h"
#include "escape.h"
#include "parse.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {printf("Please give the file to parse!\n"); exit(0);}
  A_exp pr_ret = parse(argv[1]);
  
  FILE *out = stdout;
  // pr_exp(out, pr_ret, 0);
  
  F_fragList frags = SEM_transProg(pr_ret);
  for (F_fragList f = frags; f; f = frags->tail) {
    if (f->head->kind == F_procFrag) {
      printStmList(out, C_linearize(f->head->u.proc.body));
    } else if (f->head->kind == F_stringFrag) {
      fprintf(out, "%s\n", f->head->u.stringg.str);
    }
  }
  return 0;
}