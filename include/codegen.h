/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Codegen header file
 * @Date: 2020-01-06 16:40:10
 * @Last Modified: 2020-01-28 11:43:04
 */
#ifndef T_CODEGEN_H_
#define T_CODEGEN_H_

#include "assem.h"
#include "frame.h"
#include "tree.h"

AS_instrList F_codegen(F_frame frame, T_stmList stmList);

#endif