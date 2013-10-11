import sys
import passwd,board,boardAllow,boardDIR,functions,json2,post
import web

from includes import *

class bbsboardindex:
  def GET(self):
    boardList = board.boardIndex()
    return renderrouter({"message":boardList,"type":'json'})
