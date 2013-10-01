import sys,web
import passwd,functions,PERM
from includes import *
from entrance import session

class bbslogin:
    def GET(self,reqtype="html"):
        return web.template.render('template/', cache=False).login('/bbslogin.'+reqtype)
    def POST(self,reqtype="html"):
        username = web.input()["id"]
        pw = web.input()["pw"]
        if passwd.checkPasswd(username,pw) == True:
            global session
            session.id = username
            session.pw = pw
            return renderrouter({"message":{
		"login":"success",
		"id":session.id},"type":reqtype})
        else:
            return renderrouter({"message":errors["wrong_passwd"],"type":reqtype})
