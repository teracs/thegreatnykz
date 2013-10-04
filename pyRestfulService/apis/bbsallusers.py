import sys,web
import passwd,functions,PERM

from includes import *

class bbsallusers:
    def GET(self,reqtype="json"):
        users = passwd.getAllUserAllInfo()
        #ret = [{key:{"userid":key}} for key in users]
        return renderrouter({"message":users,"type":reqtype})
