/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Liveness module
 * @Date: 2020-01-31 21:04:25
 * @Last Modified: 2020-02-13 22:05:02
 */
#include "liveness.h"
#include "graph.h"
#include "temp.h"
#include "assem.h"
#include "util.h"

static G_graph liveMap = NULL;
static void enterLiveMap(G_table t, G_node flowNode, Temp_tempList temps);
static Temp_tempList lookupLiveMap(G_table t, G_node flowNode);
//* Enter temp into an ordered list
static void enterList(Temp_tempList list, Temp_temp t);
//* Merge two ordered temp list
static Temp_tempList mergeList(Temp_tempList list1, Temp_tempList list2);
//* Return if it is equal between two ordered temp list
static bool isequalList(Temp_tempList list1, Temp_tempList list2);

static void enterLiveMap(G_table t, G_node flowNode, Temp_tempList temps) {
  G_enter(t, flowNode, temps);
}

static Temp_tempList lookupLiveMap(G_table t, G_node flowNode) {
  return (Temp_tempList)G_look(t, flowNode);
}

static void enterList(Temp_tempList list, Temp_temp t) {
  Temp_tempList cur = list, prev = NULL;
  while (Temp_tempnum(cur->head) < Temp_tempnum(t)) {
    prev = cur;
    cur = cur->tail;
  }
  prev->tail = Temp_TempList(t, cur);
}

static Temp_tempList mergeList(Temp_tempList list1, Temp_tempList list2) { 
  Temp_tempList p = Temp_TempList(NULL, NULL), prev = p;
  while (!list1 && !list2) {
    if (Temp_tempnum(list1->head) < Temp_tempnum(list2->head)) {
      prev->tail = list1;
      list1 = list1->tail;
    } else {
      prev->tail = list2;
      list2 = list2->tail;
    }
    prev = prev->tail;
  }
  prev->tail = list1 == NULL ? list2 : list1;
  return p->tail;
}

static bool isequalList(Temp_tempList list1, Temp_tempList list2) {
  while (!list1 && !list2) {
    if (list1->head != list2->head) return FALSE;
    list1 = list1->tail; list2 = list2->tail;
  }
  if (list1 || list2) return FALSE;
  return TRUE;
}

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail) {
  Live_moveList list = checked_malloc(sizeof(*list));
  list->src = src; list->dst = dst;
  list->tail = tail;
  return list;
}

Temp_temp Live_gtemp(G_node n) {
  AS_instr instr = G_nodeInfo(n);
}

struct Live_graph Live_liveness(G_graph flow) {
  if (!liveMap) liveMap = G_Graph();

  for (G_nodeList nodes = G_nodes(flow); nodes; nodes = nodes->tail) 
    enterLiveMap(liveMap, nodes->head, NULL);

  //* in[n] = user[n] + (out[n] - def[n])
  //* out[n] = in(succs[n])
  for (G_nodeList nodes = G_nodes(flow); nodes; nodes = nodes->tail) {
    
  }

  
  struct Live_graph lg = {flow, NULL};
  return lg;
}

