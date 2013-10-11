import sys
import passwd,board,boardAllow,boardDIR,functions,json2,post
import web

from includes import *

class bbspostindex:
  def GET(self):
    data = web.input(startfrom1=1000000,count=20)
    boardName = data['boardName']
    startfrom1 = int(data['startfrom1'])
    count = int(data['count'])

    postindex = boardDIR.getPostsList(boardName,startfrom1,count)
    allow = boardAllow.getAllowList(boardName)
    ret = {"board":boardName,"posts":postindex,"allow":allow}
    return renderrouter({"message":ret,"type":'json'})
