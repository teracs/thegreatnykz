import sys
import passwd,board,boardAllow,boardDIR,functions,json2,post
import web

from includes import *

class bbsboard:
	def GET(self):
		boardList = board.getAllBoardAllInfo()
		for boardName in boardList:
			boardList[boardName]['allow'] = boardAllow.getAllowList(boardName)
		return renderrouter({"message":boardList,"type":'json'})
