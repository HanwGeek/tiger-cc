/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Process flowgraph to generate confict map and node-pair table
 * @Date: 2020-01-31 16:31:54
 * @Last Modified: 2020-01-31 21:02:54
 */
#ifndef T_LIVENESS_H_
#define T_LIVENESS_H_
#include "graph.h"
#include "temp.h"

typedef struct Live_moveList_ *Live_moveList;
struct Live_moveList_ {
  G_node src, dst;
  Live_moveList tail;
};

Live_moveList Live_MoveList (G_node src, G_node dst, Live_moveList tail);

struct Live_graph {G_graph graph; Live_moveList moves;};
Temp_temp Live_gtemp(G_node n);

struct Live_graph Live_liveness(G_graph flow);

#endif