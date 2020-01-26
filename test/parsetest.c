/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Parse test script.
 * @Date: 2020-01-26 11:28:51
 * @Last Modified: 2020-01-26 11:47:39
 */
#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "prabsyn.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {printf("Please give the file to parse!\n"); exit(0);}
  A_exp pr_ret = parse(argv[1]);
  pr_exp(stdout, pr_ret, 0);
  return 0;
}