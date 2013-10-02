#coding: utf-8
#
# getPostRaw
# getPostNoCT
# getPostInTag (TBD)
#
# scaret Email:  i @ scaret.in
# This api don't offer function like "getPostInDict" because it's a low-level
#     api, and a file string is regarded as the basic element.

import struct
import sys
import os
import functions
import re

boardFolder	=	"/home/bbs/bbshome/boards/"

def getPost(boardName,filename):
	content = _getPostRaw(boardName,filename)
	return functions.GBK2UTF(content)

def getPostNoCT(boardName,filename):
	c = getPost(boardName,filename)
	return functions.filterCT(c)

def getPostNoCTEscape(boardName,filename):
	c = _getPostRaw(boardName,filename)
	return functions.GBK2UnicodeEscape(functions.filterCT(c))

def getPostEscape(boardName,filename):
	content = _getPostRaw(boardName,filename)
	return functions.GBK2UnicodeEscape(content)

def _getPostRaw(boardName,filename):
	filename = filename[:14]
	f = open(boardFolder + boardName + os.path.sep + str(_getFileMapNum(filename)) + os.path.sep + filename)
	content = f.read()
	f.close()
	return content

def getPostNoCT(boardName,filename):
	filename = filename[:14]
	c = _getPostRaw(boardName,filename)
	return functions.filterCT(c)


def _getFileMapNum(filename):
	return int(filename[-5]) < 5 and filename[-5:-2] or str(int(filename[-5])-5)+filename[-4:-2]

	
if __name__ == "__main__":
	print getPostEscape("comment","M.1348294481.A")
