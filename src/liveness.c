/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Liveness module
 * @Date: 2020-01-31 21:04:25
 * @Last Modified: 2020-02-14 18:44:00
 */
#include "liveness.h"
#include "graph.h"
#include "flowgraph.h"
#include "temp.h"
#include "assem.h"
#include "util.h"

typedef struct Live_lists_ *Live_lists;
struct Live_lists_ {
  Temp_tempList in, out;
};
static Live_lists Live_Lists(Temp_tempList in, Temp_tempList out);

static G_table liveMap = NULL, preliveMap = NULL;
static void enterLiveMap(G_table t, G_node flowNode, Temp_tempList temps);
static Temp_tempList lookupLiveMap(G_table t, G_node flowNode);
//* Enter temp into an ordered list
static void enterList(Temp_tempList list, Temp_temp t);
//* Assign values of list to the other
static void assignList(Temp_tempList list1, Temp_tempList list2);
//* Merge two ordered temp list
static Temp_tempList mergeList(Temp_tempList list1, Temp_tempList list2);
//* List1 substract elem from list2
static Temp_tempList substractList(Temp_tempList list1, Temp_tempList list2);
//* Return if it is equal between two ordered temp list
static bool isequalList(Temp_tempList list1, Temp_tempList list2);
static bool allNodesFixed(G_graph g, G_table t, G_table prev);


static Live_lists Live_Lists(Temp_tempList in, Temp_tempList out) {
  Live_lists l = checked_malloc(sizeof(*l));
  l->in = in; l->out = out;
  return l;
}

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
    if (!isequalList(lookupLiveMap(liveMap, nodes->head), lookupLiveMap(preliveMap, nodes->head)))
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
  if (!liveMap) liveMap = G_empty();
  if (!preliveMap) preliveMap = G_empty();

  for (G_nodeList nodes = G_nodes(flow); nodes; nodes = nodes->tail) 
    enterLiveMap(liveMap, nodes->head, Live_Lists(NULL, NULL));
  
  //* in[n] = user[n] + (out[n] - def[n])
  //* out[n] = in(succs[n])
  do {
    for (G_nodeList nodes = G_nodes(flow); nodes; nodes = nodes->tail) { 
      G_node node = nodes->head;
      Live_lists nList = lookupLiveMap(liveMap, node);
      Live_lists nPrevList = lookupLiveMap(preliveMap, node);
      if (!nList || !nPrevList) assert(0);

      assignList(nPrevList->in, nList->in);
      assignList(nPrevList->out, nList->out);
      assignList(nList->in, mergeList(FG_use(node), substractList(nList->out, FG_def(node))));
      
      Temp_tempList outList = NULL;
      for (G_nodeList succs = G_succ(node); succs; succs = succs->tail) {
        Temp_tempList succInList = (Live_lists)lookupLiveMap(liveMap, succs->head);
        if (!outList) outList = Temp_TempList(succInList)
      }
    }
  } while (!allNodesFixed(flow, liveMap, preliveMap));
  
  struct Live_graph lg = {flow, NULL};
  return lg;
}

