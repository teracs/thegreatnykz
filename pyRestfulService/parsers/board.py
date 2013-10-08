#coding: utf-8
#
# parse .BOARDS
# parse /boards/<board>/.DIR
# parse /boards/<board>/board.allow
#
# getBoards()         boardname list
# getBoardDetailed(boardName)
# isBoard(boardName)    bool
# isKBoard(boardName)    bool(please make sure whether board exist)
# isInBoard(username,boardName)
#                           bool(please make sure whether board exist)
# getPostsList(boardName)  list(please make sure whether board exist)
#


import struct
import sys
import os
import functions
import re
import config
boardFolder  =  config.dir_bbshome + "/boards/"

boardFilePath = config.dir_bbshome + "/.BOARDS"
#boardFilePath = os.getcwd()+"/BOARDS"
BOARDSstruct =  ["76s",    "i",    "20s",    "56s",  "3s",    "c",    "80s",    "I",    "12s"    ]
BOARDSorder =  ["filename",  "group",  "owner",  "BM",  "flag2",  "flag",    "title",  "level",  "accessed"  ]

# 高帅富函数
# 手动去除名字里有点的板面……
def getAllBoardAllInfo():
  boardList = _getDetailedBoards()
  ret = {}
  for board in boardList:
    if re.match("^[a-zA-Z0-9_]*$",board['filename']):
      ret[board['filename']] = board
  return ret

# 矮矬穷函数。这个矮矬穷函数实际上列出了所有的板面文件夹（而非读取.BOARDS）
# 手动去除名字里有点的板面……
def boardIndex():
  boards = []
  for file in os.listdir(config.dir_bbshome + "/boards"):
    if os.path.isdir(config.dir_bbshome + "/boards/" + file) and "." not in file:
      boards.append(file)
  return boards

# 矮帅富函数。board数量就有限，就算了。。。
def getBoards(boardArr):
  boardList = _getDetailedBoards()
  ret = {}
  for board in boardList:
    if board['filename'] in boardArr:
      ret[board['filename']] = board
  return ret

# 少用。去用getBoards
def getBoardAttr(boardName,attr):
  detail = getBoardDetailed(boardName)
  if not detail:
    return False
  for k in detail:
    if k == attr:
      return detail[k]
  return False

def _getDetailedBoards():
  f = open(boardFilePath)
  data = f.read()
  dataout = functions.parseStruct(BOARDSstruct,BOARDSorder,256,data)
  f.close()
  for item in dataout:
    if len(item["BM"]) and item["BM"][0] =="\0":
      item["BM"] = ""
    if "\0" in item["filename"]:
      item["filename"] = item["filename"].split("\0")[0]
  return dataout

if __name__ == "__main__":
  boards = getAllBoardAllInfo().keys()
  boarddir = boardIndex()
  print boards
  for board in boards:
    if not (board in boarddir):
      print ",".join([i for i in board]) + " not in boarddir"
      print ",".join([str(ord(i)) for i in board]) + " not in boarddir"
  for board in boarddir:
    if not (board in boards):
      print ",".join([i for i in board]) + " not in boards"
