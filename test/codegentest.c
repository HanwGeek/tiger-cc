/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Codegen test script.
 * @Date: 2020-01-27 11:39:52
 * @Last Modified: 2020-02-28 15:50:05
 */
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "temp.h" 
#include "tree.h" 
#include "assem.h"
#include "frame.h" 
#include "semant.h" 
#include "canon.h"
#include "prabsyn.h"
#include "printtree.h"
#include "escape.h"
#include "parse.h"
#include "codegen.h"

static void doProc(FILE* out, F_frame frame, T_stm body) {
  AS_proc proc;
  // struct RA_result allocation;
  
  T_stmList stmList = C_traceSchedule(C_basicBlocks(C_linearize(body)));
  AS_instrList iList = F_codegen(frame, stmList);
  
  fprintf(out, "BEGIN %s\n", Temp_labelstring(F_name(frame)));
  AS_printInstrList(out, iList, Temp_layerMap(F_tempMap, Temp_name()));
  fprintf(out, "END %s\n\n", Temp_labelstring(F_name(frame)));
}

int main(int argc, char* argv[]) {
  if (argc < 2) {printf("Please give the file to parse!\n"); exit(0);}
  A_exp pr_ret = parse(argv[1]);
  
  FILE *out = stdout;
  // pr_exp(out, pr_ret, 0);
  
  F_fragList frags = SEM_transProg(pr_ret);
  for (; frags; frags = frags->tail) {
    if (frags->head->kind == F_procFrag) {
      doProc(out, frags->head->u.proc.frame, frags->head->u.proc.body);
    } else if (frags->head->kind == F_stringFrag) {
      fprintf(out, "%s\n", frags->head->u.stringg.str);
    }
  }
  
  fclose(out);
  return 0;
}