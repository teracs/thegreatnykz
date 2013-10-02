import web,sys

import passwd

try:
    import simplejson as json
except ImportError:
    import json


def render_json(message):
	web.header('Content-Type', 'application/json')
	return json.dumps(message)
def render_html(message):
	web.header('Content-Type', 'text/html')
	def listed(msg):
		if type(msg) == dict:
			out = "<ul>"
			for i in msg:
				out += '<li><font color="blue">%s</font> : %s</li>'%(str(i),str(listed(msg[i])))
			out +="</ul>"
		elif type(msg) == list:
			out = "<ul>"
			for i in msg:
				out += '<li>%s</li>'%str(listed(i))
			out +="</ul>"	
		elif type(msg) == str:
			out = "\"%s\""%msg
		elif type(msg) in (int,float,long):
			out = "<font color=\"green\">%s</font>"%str(msg)
		else:
			out = str(msg)
		return out
	out = "<!DOCTYPE html><html><body>%s</body></html>"%listed(message)
	return out

#render_html = render_json

def renderrouter(d):
	if "type" in d:
		if d["type"] == "json":
			return render_json(d)
		else:
			return render_html(d)
	else:
		return render_html(d)



errors = {
	"no_error"	:{"no":0,"error":""},

	"wrong_passwd"	:{"no":1001,"error":"invalid username or passwd"},
	"wrong_id"	:{"no":1002,"error":"invalid user"},

	"require_id":{"no":2001,"error":"id is required"},
}

def checkPasswd():
	web.header('content-type','text/html;charset=utf-8',unique=True)
	data=web.input()
	if "user" in data and "pw" in data:
		return passwd.checkPasswd(data.user,data.pw)
	else:
		return False
