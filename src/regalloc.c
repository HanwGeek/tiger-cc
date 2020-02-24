/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: function implement for regalloc module
 * @Date: 2020-02-17 20:42:54
 * @Last Modified: 2020-02-24 10:54:36
 */
#include "regalloc.h"
#include "color.h"
#include "frame.h"
#include "temp.h"
#include "graph.h"
#include "liveness.h"
#include "flowgraph.h"

struct RA_result RA_regAlloc(F_frame f, AS_instrList il) {
  G_graph flow = FG_AssemFlowGraph(il);
  struct Live_graph liveness = Live_liveness(flow);
  struct COL_result colorResult = COL_color(liveness.graph, F_tempMap, F_registers());
  struct RA_result result = {colorResult.coloring, il};
  return result;
}
