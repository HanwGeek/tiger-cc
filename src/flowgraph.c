/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Flowgraph module
 * @Date: 2020-01-31 21:12:21
 * @Last Modified: 2020-02-04 16:19:55
 */
#include "flowgraph.h"
#include "assem.h"
#include "temp.h"
#include "table.h"

static void FG_completeEgde(TL_table t, G_node n);

static void FG_completeEgde(TL_table t, G_node n) {
  Temp_tempList tempList = ((AS_instr)G_nodeInfo(n))->u.OPER.jumps->labels;
  for (; tempList; tempList = tempList->tail) {
    G_node t = TL_look(t, tempList->head);
    if (t && !G_goesTo(n, t))
      G_addEdge(n, t);
  }
}

Temp_tempList FG_def(G_node n) {
  AS_instr instr = G_nodeInfo(n);
  if (n != NULL) {
    switch (instr->kind) {
      case I_OPER: return instr->u.OPER.dst;
      case I_LABEL: return instr->u.LABEL.label;
      case I_MOVE: return instr->u.MOVE.dst;
      default: assert(0 && "Invalid instrucion kind");
    }
  }
  return NULL;
}

Temp_tempList FG_use(G_node n) {
  AS_instr instr = G_nodeInfo(n);
  if (n != NULL) {
    switch (instr->kind) {
      case I_OPER: return instr->u.OPER.src;
      case I_LABEL: return instr->u.LABEL.label;
      case I_MOVE: return instr->u.MOVE.src;
      default: assert(0 && "Invalid instruction kind");
    }
  }
  return NULL;
}

bool FG_isMove(G_node n) {
  AS_instr instr = G_nodeInfo(n);
  //TODO: Delete node if src == dst
  return (instr != NULL && instr->kind == I_MOVE);
}

G_graph FG_AssemFlowGraph(AS_instrList il) {
  G_graph g = G_Graph();
  G_nodeList nodes = NULL;
  G_node cur = NULL, prev = NULL;
  TL_table tl = Templabeltable_empty();
  for (AS_instrList instrList = il; instrList; instrList = instrList->tail) {
    AS_instr instr = instrList->head;
    cur = G_Node(g, instr);
    if (prev) G_addEdge(prev, cur);
    prev = cur;
    switch (instr->kind) {
      case I_OPER:
        nodes = G_NodeList(cur, nodes);
      case I_MOVE:
        break;
      case I_LABEL:
        TL_enter(tl, instr->u.LABEL.label, cur);
        break;
      default:
        assert(0 && "Invalid instruction kind");
        break;
    }
  }
  for (; nodes; nodes = nodes->tail) {
    FG_completeEgde(tl, nodes->head);
  }
  return g;
}