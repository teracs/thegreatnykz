import sys,web
import passwd,functions,PERM

from includes import *

class bbsuserindex:
    def GET(self,reqtype="json"):
        users = passwd.userIndex()
        #ret = [{key:{"userid":key}} for key in users]
        return renderrouter({"message":users,"type":reqtype})
