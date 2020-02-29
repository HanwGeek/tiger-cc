/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: function implement for regalloc module
 * @Date: 2020-02-17 20:42:54
 * @Last Modified: 2020-02-29 16:09:56
 */
#include "regalloc.h"
#include "color.h"
#include "frame.h"
#include "temp.h"
#include "table.h"
#include "graph.h"
#include "assem.h"
#include "liveness.h"
#include "flowgraph.h"

static AS_instrList rewriteProgram(AS_instrList il, Temp_tempList spills, TAB_table info);
//* Return the accesses of spill temps
static TAB_table allocInfo(F_frame f, Temp_tempList spills);
//* Insert data `mov` instr
static void insertInstr(AS_instrList instrs, Temp_tempList defs, Temp_tempList uses, TAB_table info);
//* Insert `Load` instr before origin uses
static void insertLoad(AS_instrList instrs, Temp_temp temp, F_access access);
//* Insert `Store` instr after origin defs
static void insertStore(AS_instrList instrs, Temp_temp temp, F_access access);
//* Return if `temp` in defs of `instr`
static bool inDefs(AS_instr instr, Temp_temp temp);
//* Return if `temp` in uses of `instr`
static bool inUses(AS_instr instr, Temp_temp temp);

static TAB_table allocInfo(F_frame f, Temp_tempList spills) {
  TAB_table info = TAB_empty();
  for (; spills; spills = spills->tail) {
    F_access access = F_allocLocal(f, TRUE);
    TAB_enter(info, spills->head, access);
  }
  return info;
}

static void insertInstr(AS_instrList instrs, Temp_tempList defs, Temp_tempList uses, TAB_table info) {
  for (; defs; defs = defs->tail)
    insertStore(instrs, defs->head, (F_access)TAB_look(info, defs->head));
  for (; uses; uses = uses->tail)
    insertLoad(instrs, uses->head, (F_access)TAB_look(info, uses->head));
}

static void insertLoad(AS_instrList instrs, Temp_temp temp, F_access access) {
  Temp_temp t = Temp_newtemp();
  AS_instr load = AS_Oper(String_format("mov `s0, [`s1 + %d]", F_inFrameOffset(access)),
                    Temp_TempList(t, NULL),
                      Temp_TempList(F_SP(), NULL), NULL);
  // prev -> load -> cur
  instrs->tail = AS_InstrList(load, instrs->tail);
}

static void insertStore(AS_instrList instrs, Temp_temp temp, F_access access) {
  Temp_temp t = Temp_newtemp();
  AS_instr store = AS_Oper(String_format("mov [`s0 + %d], `s1", F_inFrameOffset(access)),
                    Temp_TempList(F_SP(), NULL),
                    Temp_TempList(t, NULL), NULL);
  // prev -> cur -> store
  instrs->tail->tail = AS_InstrList(store, instrs->tail->tail);
}
static bool inDefs(AS_instr instr, Temp_temp temp) {
  Temp_tempList defs = NULL;
  switch (instr->kind) {
    case I_OPER:
      defs = instr->u.OPER.dst;
    case I_MOVE:
      defs = instr->u.MOVE.dst;
    case I_LABEL:
      break;
    default: assert(0); break; 
  }

  for (; defs; defs = defs->tail)
    if (temp == defs->head)
      return TRUE;
  return FALSE;
}

static bool inUses(AS_instr instr, Temp_temp temp) {
  Temp_tempList uses = NULL;  
  switch (instr->kind) {
    case I_OPER:
      uses = instr->u.OPER.src;
    case I_MOVE:
      uses = instr->u.MOVE.src;
    case I_LABEL:
      break;
    default:  assert(0); break;
  }

  for (; uses; uses = uses->tail)
    if (temp == uses->head)
      return TRUE;
  return FALSE;
}

static AS_instrList rewriteProgram(AS_instrList il, Temp_tempList spills, TAB_table info) {
  AS_instrList prog = AS_InstrList(NULL, NULL);
  AS_instrList cur = prog->tail, prev = prog;

  while (cur) {
    Temp_tempList defs = NULL, uses = NULL;
    for (Temp_tempList s = spills; s; s = s->tail) {
      //* Specify the `load` instr location of spills at current instr
      if (inDefs(cur->head, s->head)) defs = Temp_TempList(s->head, defs);
      if (inUses(cur->head, s->head)) uses = Temp_TempList(s->head, uses);
    }

    if (defs || uses) insertInstr(prev, defs, uses, info);
    prev = cur;
    cur = cur->tail;
  }
}

struct RA_result RA_regAlloc(F_frame f, AS_instrList il) {
  Temp_tempList spilledNodes = NULL;
  struct Live_graph liveness;
  struct COL_result colorResult;
  
  do {
    spilledNodes = NULL;

    G_graph flow = FG_AssemFlowGraph(il);
    liveness = Live_liveness(flow);
    colorResult = COL_color(liveness.graph, F_tempMap, F_registers());

    spilledNodes = colorResult.spills;

    if (spilledNodes) {
      // Spill process
      TAB_table info = allocInfo(f, spilledNodes);
      il = rewriteProgram(il, spilledNodes, info);
    }
  } while (spilledNodes);

  struct RA_result result = {colorResult.coloring, il}; 
  return result;
}
