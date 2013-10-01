import sys,web
import passwd,functions,PERM

from includes import *

class bbsallusers:
    def GET(self,reqtype="html"):
        users = []
        userList = passwd.getUsers()
        for u in userList:
            users.append({
                "userid":passwd.getAttr(u,"userid"),
                "username":passwd.getAttr(u,"username"),
                "gender":passwd.getAttr(u,"gender"),
                "lastlogin":passwd.getAttr(u,"lastlogin"),
                "lastlogout":passwd.getAttr(u,"lastlogout")})
        return renderrouter({"message":users,"type":reqtype})
