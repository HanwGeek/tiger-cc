/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Compiler main prog
 * @Date: 2020-02-24 10:54:53
 * @Last Modified: 2020-03-07 21:11:12
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regalloc.h"
#include "util.h"
#include "errormsg.h"
#include "absyn.h"
#include "semant.h"
#include "frame.h"
#include "assem.h"
#include "codegen.h"
#include "tree.h"
#include "frame.h"
#include "canon.h"
#include "flowgraph.h"
#include "liveness.h"
#include "color.h"

extern int yyparse(void);
extern A_exp absyn_root;

void compile(char *inFilename, char *outFilename) {
  printf("Compiling %s to %s ...", inFilename, outFilename);
  EM_reset(inFilename);
  if (yyparse()) {
    
  }
  F_fragList frags = SEM_transProg(absyn_root);
  
  FILE *assemFile = fopen(outFilename, "w");
  for (F_fragList f = frags; f; f = f->tail)
    if (f->head->kind == F_stringFrag) {
      fprintf(assemFile, "%s db \"%s\"", f->head->u.stringg.label, f->head->u.stringg.str);
    } else {
      
    }
}

int main(int argc, char const *argv[])
{
  
  return 0;
}

