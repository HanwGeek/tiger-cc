/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: function prototype from regalloc.c
 * @Date: 2020-02-17 19:56:11
 * @Last Modified: 2020-02-17 19:57:03
 */

#ifndef T_REGALLOC_H_
#define T_REGALLOC_H_
#include "temp.h"
#include "assem.h"
#include "frame.h"

struct RA_result {Temp_map coloring; AS_instrList il;};
struct RA_result RA_regAlloc(F_frame f, AS_instrList il);

#endif
