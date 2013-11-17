#!-*-coding:utf-8-*-
import board

boards = board.getAllBoardAllInfo()


print "所有的板面："

for boardName in boards:
  print "\n",boardName
  for attrName in boards[boardName]:
    print attrName,"    ",boards[boardName][attrName]