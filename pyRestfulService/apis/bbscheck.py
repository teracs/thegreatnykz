import sys,web
import passwd,functions,PERM



class bbscheck:
    def GET(self):
        if checkPasswd() != True:
            return {"message":errors["wrong_passwd"]}
        else:
            userid = web.input()["user"]
            return {"message":{
		"userid":userid,
		"username":passwd.getAttr(userid,"username"),
		"gender":passwd.getAttr(userid,"gender"),
		"birthmonth":passwd.getAttr(userid,"birthmonth"),
		"birthday":passwd.getAttr(userid,"birthday"),
		"firstlogin":passwd.getAttr(userid,"firstlogin"),
		"lasthost":passwd.getAttr(userid,"lasthost"),
		"numlogins":passwd.getAttr(userid,"numlogins"),
		"money":passwd.getAttr(userid,"money"),
		"lastlogin":passwd.getAttr(userid,"lastlogin"),
		"lastlogout":passwd.getAttr(userid,"lastlogout"),
		"stay":passwd.getAttr(userid,"stay"),
		"level":PERM.levels(passwd.getAttr(userid,"userlevel"))

}}
