/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Liveness module
 * @Date: 2020-01-31 21:04:25
 * @Last Modified: 2020-02-19 17:56:29
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
static bool allNodesFixed(G_graph g, G_table t, G_table prev);
static G_node inCollGraph(G_graph g, Temp_temp temp);
static void addCollEdge(G_graph g, Temp_temp from, Temp_temp to);


static void enterLiveMap(G_table t, G_node flowNode, Temp_tempList temps) {
  G_enter(t, flowNode, temps);
}

static Temp_tempList lookupLiveMap(G_table t, G_node flowNode) {
  return (Temp_tempList)G_look(t, flowNode);
}

static bool allNodesFixed(G_graph g, G_table t, G_table pret) {
  for (G_nodeList nodes = G_nodes(g); nodes; nodes = nodes->tail) 
    if (!isequalList(lookupLiveMap(t, nodes->head), lookupLiveMap(pret, nodes->head)))
      return FALSE;
  return TRUE;
}

static G_node inCollGraph(G_graph g, Temp_temp temp) {
  for (G_nodeList nodes = G_nodes(g); nodes; nodes = nodes->tail)
    if (Live_gtemp(nodes->head) == temp)
      return nodes->head;
  return NULL;
}

static void addCollEdge(G_graph g, Temp_temp from, Temp_temp to) {
  G_node fromNode = inCollGraph(g, from), toNode = inCollGraph(g, to);
  if (!fromNode) fromNode = G_Node(g, from);
  if (!toNode) toNode = G_Node(g, to);
  G_addEdge(fromNode, toNode);
} 

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail) {
  Live_moveList list = checked_malloc(sizeof(*list));
  list->src = src; list->dst = dst;
  list->tail = tail;
  return list;
}

Temp_temp Live_gtemp(G_node n) {
  return (Temp_temp)G_nodeInfo(n);
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
      
      assignList(preInList, inList); Temp_assignList(preOutList, outList);
      assignList(inList, Temp_mergeList(FG_use(node), Temp_substractList(outList, FG_def(node))));

      Temp_tempList inSuccsList = NULL;
      for (G_nodeList succs = G_succ(node); succs; succs = succs->tail) 
        inSuccsList = Temp_mergeList(inSuccsList, lookupLiveMap(inLiveMap, succs->head));
    }
  } while (!allNodesFixed(flow, inLiveMap, preInLiveMap) && !allNodesFixed(flow, outLiveMap, preOutLiveMap));
  
  G_graph collision = G_Graph();       
  for (G_nodeList nodes = G_nodes(flow); nodes; nodes = nodes->tail) {
    G_node node = nodes->head;
    if (!FG_isMove(node)) {
      Temp_temp defVar = FG_use(node);
      if (defVar) {
        Temp_tempList liveVars = lookupLiveMap(outLiveMap, node);
        for (; liveVars; liveVars = liveVars->tail)
          addCollEdge(collision, defVar, liveVars->head);
      }
    } else if (FG_isMove(node)) {
      Temp_tempList liveVars = lookupLiveMap(outLiveMap, node);
      for (; liveVars; liveVars = liveVars->tail) {
        Temp_temp liveVar = liveVars->head;
        Temp_temp moveSrc = ((AS_instr)G_nodeInfo(node))->u.MOVE.src;
        Temp_temp moveDst = ((AS_instr)G_nodeInfo(node))->u.MOVE.dst;
        if (moveSrc != liveVar) {
          addCollEdge(collision, moveDst, liveVar);
        }
      }
    }
  }
  struct Live_graph lg = {collision, NULL};
  return lg;
}

