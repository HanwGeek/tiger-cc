/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Function prototypes to represent control flow graphs
 * @Date: 2020-01-30 10:13:11
 * @Last Modified: 2020-01-30 10:19:08
 */
#ifndef T_FLOWGRAPH_H_
#define T_FLOWGRAPH_H_
#include "temp.h"
#include "graph.h"
#include "assem.h"

Temp_tempList FG_def(G_node n);
Temp_tempList FG_use(G_node n);
bool FG_isMove(G_node n);
G_graph FG_AssemFlowGraph(AS_instrList il);

#endif