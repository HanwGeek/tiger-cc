/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Liveness module
 * @Date: 2020-01-31 21:04:25
 * @Last Modified: 2020-02-04 16:23:01
 */
#include "liveness.h"
#include "temp.h"

static void enterLiveMap(G_table t, G_node flowNode, Temp_tempList temps);
static Temp_tempList lookupLiveMap(G_table t, G_node flowNode);

static void enterLiveMap(G_table t, G_node flowNode, Temp_tempList temps) {
  G_enter(t, flowNode, temps);
}

static Temp_tempList lookupLiveMap(G_table t, G_node flowNode) {
  return (Temp_tempList)G_look(t, flowNode);
}

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail) {
  Live_moveList list = checked_malloc(sizeof(*list));
  list->src = src; list->dst = dst;
  list->tail = tail;
  return list;
}

Temp_temp Live_gtemp(G_node n) {
  //TODO:
}

struct Live_graph Live_liveness(G_graph flow) {
  //TODO:
  struct Live_graph lg = {flow, NULL};
  return lg;
}

