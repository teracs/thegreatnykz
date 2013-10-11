import sys,web
import passwd,functions,PERM
from includes import *


class bbscheck:
    def GET(self,reqtype="json"):
      userid = web.input()["user"]
      pw = web.input()["pw"]
      if not passwd.checkPasswd(userid,pw):
          return renderrouter({"message":errors["wrong_passwd"],"type":reqtype})
      return renderrouter({"message":passwd.getUsers([userid]),"type":reqtype})
