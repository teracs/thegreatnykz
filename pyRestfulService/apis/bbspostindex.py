import sys
import passwd,board,boardAllow,boardDIR,functions,json2,post
import web

from includes import *

class bbspostindex:
  def GET(self):
    boardName = web.input()['boardName']
    postindex = boardDIR.getPostsList(boardName)
    allow = boardAllow.getAllowList(boardName)
    ret = {"board":boardName,"posts":postindex,"allow":allow}
    return renderrouter({"message":ret,"type":'json'})
