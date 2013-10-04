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
