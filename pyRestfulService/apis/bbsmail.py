import sys
import passwd,mail,functions,json2,post
import web

from includes import *

class bbsmailindex:
  def GET(self):
    data = web.input(startfrom1=1000000,count=20)
    user = data.user
    startfrom1 = int(data['startfrom1'])
    count = int(data['count'])

    mailindex = mail.getMailList(user,startfrom1,count)
    unread = mail.getUnreadCount(user)
    ret = {"user":user,"mails":mailindex,"unread":unread}
    return renderrouter({"message":ret,"type":'json'})
  def Post(self):
    data = web.input(unread = 0)
    user = data.user
    unread = data.unread
    ret = mail.setUnreadCount(user,unread)
    return renderrouter({"message":ret,"type":'json'})

class bbsmail:
  def GET(self):
    user = web.input()['user']
    filename = web.input()['filename']
    mailRaw = mail.getMail(user,filename)
    return renderrouter({"message":mailRaw,"type":'json'})
  def POST(self,reqtype = "json"):
    data= web.input(reid=False)
    userfrom = data.userfrom
    userto = data.userto
    title = data.title
    content = data.content
    reid = data.reid
    # print data
    ret = mail.sendMail(userfrom, userto, title, content, reid)
    return renderrouter({"message":ret,"type":reqtype})
