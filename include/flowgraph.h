/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Function prototypes to represent control flow graphs
 * @Date: 2020-01-30 10:13:11
 * @Last Modified: 2020-02-17 19:54:53
 */
#ifndef T_FLOWGRAPH_H_
#define T_FLOWGRAPH_H_
#include "temp.h"
#include "graph.h"
#include "assem.h"

//* Return node info of defs
Temp_tempList FG_def(G_node n);
//* Return node info of uses
Temp_tempList FG_use(G_node n);
//* Return if a node is I_MOVE
//* And delete it if src == dst
bool FG_isMove(G_node n);
//* Generate flow graph with instructions
G_graph FG_AssemFlowGraph(AS_instrList il);

#endif