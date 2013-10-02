#!/usr/bin/python
import sys,re
sys.path.append("./parser")
import boardDIR,board,post

def test_getPost():
	for boardName in board.getBoards():
		l = boardDIR.getPostsList( boardName )
		for thePost in l:
			filename = thePost["filename"]
			content = post.getPost(boardName,filename)

			assert type(content) == str

def test_getPostNoCT():
	for boardName in board.getBoards():
		l = boardDIR.getPostsList( boardName )
		for thePost in l:
			filename = thePost["filename"]
			content = post.getPostNoCT(boardName,filename)

			assert not re.search('\\033\[[0-9;]*[a-zA-Z]',content)
			assert type(content) == str

def test_getPostEscape():
	for boardName in board.getBoards():
		l = boardDIR.getPostsList( boardName )
		for thePost in l:
			filename = thePost["filename"]
			content = post.getPostEscape(boardName,filename)

			for i in content:
				assert ord(i) < 128
				assert ord(i) >= 32


def test_getPostNoCTEscape():
	for boardName in board.getBoards():
		l = boardDIR.getPostsList( boardName )
		for thePost in l:
			filename = thePost["filename"]
			content = post.getPostNoCTEscape(boardName,filename)
			assert not re.search('\\\\x1b\[[0-9;]*[a-zA-Z]',content)
			for i in content:
				assert ord(i) < 128
				assert ord(i) >= 32

