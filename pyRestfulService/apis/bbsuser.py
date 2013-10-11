import sys,web
import passwd,functions,PERM

from includes import *


class bbsuser:
    def GET(self,userid,reqtype="json"):
        users = passwd.getUsers([userid])
        if  not len(users):
            return renderrouter({"message":errors["wrong_id"],"type":reqtype})
        else:
            return renderrouter({"type":reqtype,"message":users})
