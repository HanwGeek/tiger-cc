/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: color module implement
 * @Date: 2020-02-17 20:05:43
 * @Last Modified: 2020-02-25 15:39:14
 */
#include <stdlib.h>
#include "color.h"
#include "frame.h"
#include "graph.h"
#include "temp.h"
#include "table.h"

static const int STACK_SIZE = 256;
static const int K = 6;
typedef struct COL_stack_ *COL_stack;
typedef struct COL_node_ *COL_node;
typedef struct COL_nodeList_ *COL_nodeList;
typedef struct adjPair_ *adjPair;
typedef struct adjSet_ *adjSet;
struct COL_stack_ {
  G_node* u;
  int top, size;
};

struct COL_nodeList_ {
  G_node n;
  COL_nodeList prev, next, tail;
};

struct adjPair_ {
  G_node u, v;
};

struct adjSet_ {
  TAB_table s;
};

static COL_stack selectStack = NULL;
static COL_nodeList simplifyWorklist = NULL;
static COL_nodeList spillWorklist = NULL;
static COL_nodeList coloredNodes = NULL;
// static COL_nodeList worklistMoves = NULL;
// static COL_nodeList freezeWorklist = NULL;
static int* degree = NULL;
static int *color = NULL;
static int registers[6];
static const string regNames[] = {"eax", "ebx", "ecx", "edx", "esi", "edi"};
//* Adjacent list of graph nodes
static G_nodeList* adjList = NULL;

static COL_stack Stack(void);
static void stackPush(COL_stack s, G_node t);
static G_node stackPop(COL_stack s);
static bool stackEmpty(COL_stack s);
static void enterSet(adjSet set, adjPair edge);
static COL_node COL_SimplifyNode(G_node n);
static COL_node COL_SpillNode(G_node n);
static void addNode(COL_nodeList list, G_node n);
static COL_nodeList rmNode(COL_nodeList list, G_node n);
static void makeWorkList(Temp_map initial, G_graph g);
//* Remove nodes whose degree < K
static void Simplify(void);
static void assignColors(void);
static bool MoveRelated(G_node n);
static void DecrementDegree();
static bool colorEmpty(void);
static bool inNodeList(G_node node, COL_nodeList list);
static Temp_tempList toTempList(COL_nodeList list);
static int selectColor(void);

static COL_stack Stack(void) {
  COL_stack s = checked_malloc(sizeof(*s));
  s->u = (G_node*)malloc(STACK_SIZE * sizeof(G_node));
  s->top = 0; s->size = STACK_SIZE;
  return s;
}

static void stackPush(COL_stack s, G_node n) {
  if (s->top + 1 >= s->size) {
    s->size += s->size >> 1;
    s->u = (G_node*)realloc(s->u, s->size * sizeof(G_node));
  }
  s->u[s->top++] = n;
}

static G_node stackPop(COL_stack s) {
  assert(s->top == 0);
  return s->u[--s->top];
}

static bool stackEmpty(COL_stack s) {
  return s->top == 0;
}

static void enterSet(adjSet set, adjPair edge) {
  TAB_enter(set->s, edge, NULL);
}

static void addNode(COL_nodeList list, G_node n) {
  if (list == NULL) {
    list = checked_malloc(sizeof(*list));
    list->n = n;
    list->next = NULL;
    list->prev = list;
  } else {
    COL_nodeList p = checked_malloc(sizeof(*p));
    p->prev = list; p->next = list->next;
    if (list->next) list->next->prev = p;
    list->next = p;
  }
}

static COL_nodeList rmNode(COL_nodeList list, G_node n) {
  while (list->n != n) list = list->next;
  if (list->prev == list) return list->next;
  list->prev->next = list->next;
  return list;
}

static void makeWorkList(Temp_map initial, G_graph g) {
  Temp_temp n = NULL;
  while ((n = Temp_popMap(initial))) {
    //TODO: remove `Temp_tempnum`
    if (degree[Temp_tempnum(n)] >= K) 
      addNode(spillWorklist, G_findInNodes(Temp_look(initial, n), g));
    else if (MoveRelated(G_findInNodes(n, g))) {

    } else addNode(simplifyWorklist, G_findInNodes(Temp_look(initial, n), g));
  }
}

static void Simplify() {
  for (COL_nodeList nodes = simplifyWorklist; nodes; nodes = nodes->next) {
    simplifyWorklist = rmNode(simplifyWorklist, nodes->n);
    G_node n = nodes->n;
    stackPush(selectStack, n);
    for (G_nodeList succs = G_succ(n); succs; succs = succs->tail)
      G_rmEdge(n, succs->head);
    for (G_nodeList preds = G_pred(n); preds; preds = preds->tail)  
      G_rmEdge(preds->head, n);
  } 
}

static bool MoveRelated(G_node n) {
  
}

static void DecrementDegree(COL_node n) {
  
}

static void assignColors(void) {
  while (stackEmpty(selectStack)) {
    G_node n = stackPop(selectStack);
    for (G_nodeList nodes = adjList[Temp_tempnum(G_nodeInfo(n))]; nodes; nodes = nodes->tail)
      if (inNodeList(nodes->head, coloredNodes))
        registers[color[Temp_tempnum(G_nodeInfo(nodes->head))]] = 0;
    if (colorEmpty())
      addNode(spillWorklist, G_nodeInfo(n));
    else {
      addNode(coloredNodes, G_nodeInfo(n));
      color[Temp_tempnum(G_nodeInfo(n))] = selectColor();
    }
  }
}

static bool colorEmpty(void) {
  for (int i = 0; i < K; i++)
    if (registers[i] != 0)
      return FALSE;
  return TRUE;
}

static bool inNodeList(G_node node, COL_nodeList list) {
  while (list) {
    if (list->n == node) return TRUE;
    list = list->tail; 
  }
  return FALSE;
}

static Temp_tempList toTempList(COL_nodeList list) {
  Temp_tempList retList = NULL;
  while (list) {
    if (retList) retList = Temp_TempList(G_nodeInfo(list->n), NULL);
    else retList->tail = Temp_TempList(G_nodeInfo(list->n), NULL);
    list = list->next;
  }
  return retList;
}

static int selectColor(void) {
  for (int i = 0; i < K; i++)
    if (registers[i]) return i;
  assert(0);
  return -1;
}

struct COL_result COL_color(G_graph ig, Temp_map initial, Temp_tempList regs) {
  int nodeCnt = G_nodeCount(ig);
  if (!color) color = (int*)checked_malloc(nodeCnt * sizeof(int));
  for (int i = 0; i < K; i++) color[i] = 1;
  if (!degree) degree = (int*)checked_malloc(nodeCnt * sizeof(int));
  if (!adjList) adjList = (G_nodeList*)checked_malloc(nodeCnt * sizeof(G_nodeList));
  if (!selectStack) selectStack = Stack();
  for (G_nodeList nodes = G_nodes(ig); nodes; nodes = nodes->tail) {
    int tempNum = Temp_tempnum(G_nodeInfo(nodes->head));
    degree[tempNum] = G_degree(nodes->head);
    adjList[tempNum] = G_adj(nodes->head);
  }

  makeWorkList(initial, ig);
  while (simplifyWorklist) Simplify();

  Temp_map regMap = Temp_name();
  for (COL_nodeList nodes = coloredNodes; nodes; nodes = nodes->next) {
    Temp_temp temp = G_nodeInfo(nodes->n);
    Temp_enter(regMap, temp, regNames[color[Temp_tempnum(temp)]]);
  }
  struct COL_result result = {regMap, toTempList(spillWorklist)};
  return result;
}

