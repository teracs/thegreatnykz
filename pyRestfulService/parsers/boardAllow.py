import struct
import sys
import os
import functions

boardFolder	=	"/home/bbs/bbshome/boards/"

def isKBoard(boardName):
	return _hasAllowFile(boardName)

def isInBoard(username, boardName):
	return username in _getAllowList(boardName)

	
def _hasAllowFile(boardName):
	return os.path.exists( boardFolder + boardName + "/board.allow" )
	
def _getAllowList(boardName):
	if not _hasAllowFile(boardName) or not _hasAllowFile(boardName):
		return []
	f = open(boardFolder + boardName +"/board.allow","rb")
	ul = filter(None,f.read().split("\n"))
	return ul
