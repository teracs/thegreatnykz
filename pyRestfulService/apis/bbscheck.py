import sys,web
import passwd,functions,PERM
from includes import *


class bbscheck:
    def GET(self,reqtype="json"):
        if checkPasswd() != True:
            return {"message":errors["wrong_passwd"]}
        else:
            userid = web.input()["user"]
            user =  passwd.getAllUserAllInfo()[userid]
            return renderrouter({"message":user,"type":reqtype})
