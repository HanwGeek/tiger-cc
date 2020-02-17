/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Data structures and function prototypes for coloring algorithm to determine register allocation
 * @Date: 2020-02-17 19:54:18
 * @Last Modified: 2020-02-17 19:55:40
 */

#ifndef T_COLOR_H_
#define T_COLOT_H_
#include "temp.h"
#include "graph.h"

struct COL_result {Temp_map coloring; Temp_tempList spills;};
struct COL_result COL_color(G_graph ig, Temp_map initial, Temp_tempList regs);

#endif

