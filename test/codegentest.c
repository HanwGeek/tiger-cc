/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Codegen test script.
 * @Date: 2020-01-27 11:39:52
 * @Last Modified: 2020-01-27 11:40:02
 */
#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "temp.h" /* needed by translate.h */
#include "tree.h" /* needed by frame.h */
#include "assem.h"
#include "frame.h" /* needed by translate.h and printfrags prototype */
#include "semant.h" /* function prototype for transProg */
#include "canon.h"
#include "prabsyn.h"
#include "printtree.h"
#include "escape.h"
#include "parse.h"
#include "codegen.h"