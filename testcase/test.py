'''
@Author: HanwGeek
@Github: https://github.com/HanwGeek
@Description: Test script for compiler
@Date: 2019-10-20 19:36:09
@Last Modified: 2019-12-10 20:58:19
'''
import subprocess
import json
import sys
import os

test_file_dir = os.path.dirname(__file__)
files = ["test%d.tig" % i for i in range(1, 50)] + ["merge.tig", "queens.tig"]
result = json.load(open(os.path.dirname(os.path.abspath(__file__)) + "/test_result.json", "r"))
TOTAL_FILE_CNT = len(files)
ERR_FILE_CNT = 0
rets = []
err_cases = []

def test_one(filename, mode):
  global ERR_FILE_CNT
  cmd = "./main " + test_file_dir + "/" + filename
  prog = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  _, stderr = prog.communicate()
  stderr = str(stderr, encoding="utf-8")
  test_ret = 0 if "error" in stderr else 1

  if test_ret != result[filename.split('.')[0]][mode]:
    ERR_FILE_CNT += 1
    err_cases.append(filename)
    rets.append(stderr)

def test_all():
  os.chdir(os.getcwd())
  if len(sys.argv) <= 1:
    raise ValueError("Please specify the type of test")

  if sys.argv[1] in ["token", "syntax", "semant"]:
    for filename in files:
      test_one(filename, sys.argv[1])
  else:
    raise ValueError("Only support token, syntax and semant test now!")

  pass_cnt = TOTAL_FILE_CNT - ERR_FILE_CNT
  pass_rate = pass_cnt * 100 / TOTAL_FILE_CNT
  print("Test result: %d/%d files pass test!" % (pass_cnt, TOTAL_FILE_CNT))
  print("Pass rate: %d%%" % pass_rate)
  if len(err_cases) > 0:
    print("Cases failed to pass test are:")

    for err in err_cases:
      print('\t', err)

  with open("test.output", "w") as f:
    f.writelines(rets)

if __name__ == "__main__":
  test_all()