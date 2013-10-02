#!/usr/bin/python
import sys,re
sys.path.append("./parser")
import boardDIR,board

def test_getPostsList():
	for boardName in board.getBoards():
		l = boardDIR.getPostsList( boardName )

		assert boardName and len(l) > 0

		for thePost in l:
		
			num = thePost["pnum"]
			assert num >= 0
			assert type(num) == int or type(num) == long

			num = thePost["level"]
			assert num >= 0
			assert type(num) == int or type(num) == long

			s = thePost["filename"]
			assert len(s) >= 0
			assert re.search(r"^[A-Z]\.[0-9]{8,11}\.[A-Z]$",s)
			assert type(s) == str

			s = thePost["title"]
			assert len(s) >= 0
			assert type(s) == str
		
			num = thePost["reid"]
			assert num >= 10**9 or num == 0
			assert num <= 10**9*2
			assert type(num) == int or type(num) == long

			s = thePost["owner"]
			assert len(s) >= 0
			assert re.search(r"^[a-zA-Z0-9\.]{2,14}$",s)
			assert type(s) == str

			s = thePost["accessed"]
			assert len(s) >= 0
			assert type(s) == str
		
			num = thePost["id"]
			assert num >= 10**9 or num == 0
			assert num <= 10**9*2
			assert re.search(str(num),thePost["filename"]) or num == 0
			assert type(num) == int or type(num) == long
