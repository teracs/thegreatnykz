import sys
import passwd,board,functions,json2,post
import web

def checkPasswd():
	data=web.input()
	if "user" in data and "pw" in data:
		return passwd.checkPasswd(data.user,data.pw)
	else:
		return False

class bbsShowBoards:
	def GET(self):
		web.header('content-type','text/html;charset=utf-8',unique=True)
		if not checkPasswd():
			return "Not Logged In"
		boardList = board.getBoardsByUser(web.input()["user"])
		boardListDetailed = []
		boardDict = board.getBoardsDict()
		for i in boardList:
			d = boardDict[i]
			d2 = {}
			d2["board"] = d["filename"]
			d2["chineseName"] = board.getBoardChineseName(d["filename"])
			d2["bm"] = d["BM"]
			boardListDetailed.append(d2)
		return json2.write(boardListDetailed)
	def POST(self):
		return self.GET()
class bbsShowBoard:
	def GET(self):
		web.header('content-type','text/json;charset=utf-8',unique=True)
		if not checkPasswd():
			return "Not Logged In"
		start = ("start" in web.input() and int(web.input()["start"]) or 1)
		cnt = ("cnt" in web.input() and web.input()["cnt"] or 20)
		b = web.input()["board"]
		boardPostsLists = board.getPostsList(b)
		l = []
		for i in range(start-1,start+cnt-1):
			if i >= len(boardPostsLists):
				break
			t = {}
			t["filename"] = boardPostsLists[i]["filename"][:14]
			t["No"] = i+1
			t["title"] = boardPostsLists[i]["title"]
			t["owner"] = boardPostsLists[i]["owner"]
			t["id"] = boardPostsLists[i]["id"]
			t["reid"] = boardPostsLists[i]["reid"]
			t["content"] = functions.GBK2UTF(post.getPostNoCT(b,t["filename"]))
			l.append(t)
		if len(l) == 0:
			return str(len(boardPostsLists)+1)
		return json2.write(l)
			
	def POST(self):
		return self.GET()



class bbsShowUsers:
	def GET(self):
		if not checkPasswd():
			return "Not Logged in"
		users = passwd.getUsers()
		return str(users)
	def POST(self):
		return self.GET()
