import sys,web
import passwd,functions,PERM

from includes import *

class bbsuser:
    def GET(self,userid,reqtype="json"):
        users = passwd.getAllUserAllInfo()
        if userid not in users.keys():
            return renderrouter({"message":errors["wrong_id"]})
        else:
            return renderrouter({"type":reqtype,"message":users[userid]})
