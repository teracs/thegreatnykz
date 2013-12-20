import sys
import passwd,board,boardAllow,boardDIR,functions,json2,post
import web

from includes import *

class bbspost:
  def GET(self):
    boardName = web.input()['boardName']
    filename = web.input()['filename']

    postRaw = post.getPost(boardName,filename)

    return renderrouter({"message":postRaw,"type":'json'})
  def POST(self,reqtype):
    data= web.input(reid=False)
    # print data
    ret = boardDIR.createPost(data.boardName,data.user,data.title,data.content,data.reid)
    # boardName,user,title,content,reid=False
    return renderrouter({"message":ret,"type":reqtype})
