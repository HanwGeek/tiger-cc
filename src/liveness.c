/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Liveness module
 * @Date: 2020-01-31 21:04:25
 * @Last Modified: 2020-02-14 23:33:23
 */
#include "liveness.h"
#include "graph.h"
#include "flowgraph.h"
#include "temp.h"
#include "assem.h"
#include "util.h"


static G_table inLiveMap = NULL, preInLiveMap = NULL;
static G_table outLiveMap = NULL, preOutLiveMap = NULL;
static void enterLiveMap(G_table t, G_node flowNode, Temp_tempList temps);
static Temp_tempList lookupLiveMap(G_table t, G_node flowNode);
//* Enter temp into an ordered list
static Temp_tempList enterList(Temp_tempList list, Temp_temp t);
//* Assign values of list to the other
static void assignList(Temp_tempList list1, Temp_tempList list2);
//* Merge two ordered temp list
static Temp_tempList mergeList(Temp_tempList list1, Temp_tempList list2);
//* List1 substract elem from list2
static Temp_tempList substractList(Temp_tempList list1, Temp_tempList list2);
//* Return if it is equal between two ordered temp list
static bool isequalList(Temp_tempList list1, Temp_tempList list2);
static bool allNodesFixed(G_graph g, G_table t, G_table prev);


static void enterLiveMap(G_table t, G_node flowNode, Temp_tempList temps) {
  G_enter(t, flowNode, temps);
}

static Temp_tempList lookupLiveMap(G_table t, G_node flowNode) {
  return (Temp_tempList)G_look(t, flowNode);
}

static void assignList(Temp_tempList list1, Temp_tempList list2) {
  while (!list1 && !list2) {
    list1->head = list2->head;
    list1 = list1->tail;
    list2 = list2->tail;
  }
  if (!list1) list1 = NULL;
  if (!list2) list1 = list2;
}

static Temp_tempList enterList(Temp_tempList list, Temp_temp t) {
  Temp_tempList cur = list, prev = NULL;
  while (cur && Temp_tempnum(cur->head) < Temp_tempnum(t)) {
    prev = cur;
    cur = cur->tail;
  }
  prev->tail = Temp_TempList(t, cur);
}

static Temp_tempList mergeList(Temp_tempList list1, Temp_tempList list2) { 
  Temp_tempList p = Temp_TempList(NULL, NULL), prev = p;
  while (list1 && list2) {
    int num1 = Temp_tempnum(list1->head), num2 = Temp_tempnum(list2->head);
    if (num1 < num2) {
      prev->tail = list1;
      list1 = list1->tail;
    } else if (num1 > num2) {
      prev->tail = list2;
      list2 = list2->tail;
    } else {
      prev->tail = list1;
      list1 = list1->tail;
      list2 = list2->tail;
    }
    prev = prev->tail;
  }
  prev->tail = list1 == NULL ? list2 : list1;
  return p->tail;
}

static Temp_tempList substractList(Temp_tempList list1, Temp_tempList list2) {
  Temp_tempList retList = NULL, tailList = NULL;
  while (!list1 && !list2) {
    int num1 = Temp_tempnum(list1->head), num2 = Temp_tempnum(list2->head);
    if (num1 < num2) {
      if (!retList) {
        retList = Temp_TempList(list1->head, NULL);
        tailList = retList->tail;
      }
      else {
        tailList = Temp_TempList(list1->head, NULL);
        tailList = tailList->tail;
      }
    }
    if (num1 == num2) {
      list1 = list1->tail;
      list2 = list2->tail;
    }
    if (num1 > num2) {
      list2 = list2->tail;
    }
  }
  tailList = list1;
  return retList;
}

static bool isequalList(Temp_tempList list1, Temp_tempList list2) {
  while (!list1 && !list2) {
    if (list1->head != list2->head) return FALSE;
    list1 = list1->tail; list2 = list2->tail;
  }
  if (list1 || list2) return FALSE;
  return TRUE;
}

static bool allNodesFixed(G_graph g, G_table t, G_table pret) {
  for (G_nodeList nodes = G_nodes(g); nodes; nodes = nodes->tail) 
    if (!isequalList(lookupLiveMap(t, nodes->head), lookupLiveMap(pret, nodes->head)))
      return FALSE;
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
  if (!inLiveMap) inLiveMap = G_empty();
  if (!preInLiveMap) preInLiveMap = G_empty();
  if (!outLiveMap) outLiveMap = G_empty();
  if (!preOutLiveMap) preOutLiveMap = G_empty();

  for (G_nodeList nodes = G_nodes(flow); nodes; nodes = nodes->tail) {
    enterLiveMap(inLiveMap, nodes->head, NULL);
    enterLiveMap(outLiveMap, nodes->head, NULL);
  }

  //* in[n] = user[n] + (out[n] - def[n])
  //* out[n] = in(succs[n])
  do {
    for (G_nodeList nodes = G_nodes(flow); nodes; nodes = nodes->tail) { 
      G_node node = nodes->head;
      Temp_tempList inList = lookupLiveMap(inLiveMap, node), outList = lookupLiveMap(outLiveMap, node);
      Temp_tempList preInList = lookupLiveMap(preInLiveMap, node), preOutList = lookupLiveMap(preOutLiveMap, node);

      if (!inList || !outList) assert(0);
      
      assignList(preInList, inList); assignList(preOutList, outList);
      assignList(inList, mergeList(FG_use(node), substractList(outList, FG_def(node))));

      Temp_tempList inSuccsList = NULL;
      for (G_nodeList succs = G_succ(node); succs; succs = succs->tail) 
        inSuccsList = mergeList(inSuccsList, lookupLiveMap(inLiveMap, succs->head));
    }
  } while (!allNodesFixed(flow, inLiveMap, preInLiveMap) && !allNodesFixed(flow, outLiveMap, preOutLiveMap));
  
  struct Live_graph lg = {flow, NULL};
  return lg;
}

