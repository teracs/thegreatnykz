#!/usr/bin/python
import sys,re
sys.path.append("parser")
import board


def test_getBoards():
	boards = board.getBoards()
	for boardname in boards:
		assert re.search(r"^[a-zA-Z0-9]{2,12}$",boardname)

def test_getBoardAttr():
	boards = board.getBoards()
	for b in boards:
		
		num = board.getBoardAttr(b,"group")
		assert num >= 0
		assert type(num) == int or type(num) == long

		s = board.getBoardAttr(b,"title")
		assert len(s) > 0
		assert type(s) == str

		s = board.getBoardAttr(b,"BM")
		assert len(s) >= 0
		assert re.search(r"^[a-zA-Z ]*$",s)
		assert type(s) == str

		num = board.getBoardAttr(b,"level")
		assert num >= 0
		assert type(num) == int or type(num) == long

		s = board.getBoardAttr(b,"filename")
		assert s in board.getBoards()
		assert re.search(r"^[a-zA-Z0-9]{1,14}$",s)
		assert type(s) == str

		s = board.getBoardAttr(b,"flag")
		assert type(s) == str

		s = board.getBoardAttr(b,"flag2")
		assert type(s) == str

		s = board.getBoardAttr(b,"owner")
		assert re.search(r"^[a-zA-Z ]*$",s)
		assert type(s) == str

		s = board.getBoardAttr(b,"accessed")
		assert type(s) == str

def test_getBoardsDetailed():
	boardsdetail = board.getBoardsDetailed()
	assert len(boardsdetail) == len(board.getBoards())
	for boardName in board.getBoards():
		assert boardName in [i["filename"] for i in boardsdetail]
	for item in boardsdetail:
		for key in item:
			assert item[key] == board.getBoardAttr(item["filename"],key)

def test_getBoardDetailed():
	boards = board.getBoards()
	for b in boards:
		item = board.getBoardDetailed(b)
		for key in item:
			assert item[key] == board.getBoardAttr(item["filename"],key)

	assert not board.getBoardDetailed("not exist")

