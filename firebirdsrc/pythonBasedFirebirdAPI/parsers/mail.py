import struct
import sys
import os
import functions
import passwd
import json
import time
def getUnreadCount(user):
	try:
		f = open("/home/bbs/bbshome/tmp/"+user+".mailsize","rb")
		cnt = int(f.read())
		f.close()
	except:
		return -1
	return cnt

def setUnreadCount(user,cnt):
	try:
		f = open("/home/bbs/bbshome/tmp/"+user+".mailsize","wb")
		f.write(str(cnt))
		f.close()
	except:
		return -1
	return cnt

DIRstruct 	=	["80s",		"76s",		"I",	"80s",		"I",		"4s",		"I",	"I"		]
order		=	["filename",	"owner",	"pnum",	"title",	"level",	"accessed",	"id",	"reid"	]


def getMailList(id):
	return _getFileHeaders(id)

def sendMail(idfrom,idto,title,content):
	fileid = int(time.time())
	while "M.%d.A"%fileid in getMailList(idto):
		fileid +=1
	f = open("/home/bbs/bbshome/mail/%s/%s/%s/%s" \
		%(idto[0].upper(),idto[1].upper(),idto,"M.%d.A"%fileid), "wb")
	f.write(content)
	f.close()
	f = open("/home/bbs/bbshome/mail/%s/%s/%s/.DIR"%(idto[0].upper(),idto[1].upper(),idto),"ab")
	ldata = ["M.%d.A"%fileid,"%s (%s)"%(idfrom,idfrom),0,title,0,"",fileid,fileid]
	data = functions.packStruct(ldata,DIRstruct,256)
	f.write(data)
	f.close()
	cnt = getUnreadCount(idto)
	setUnreadCount(idto,cnt+1)
	return dict(zip(order,ldata))

def getMail(id,filename):
	f = open("/home/bbs/bbshome/mail/%s/%s/%s/%s" \
		%(id[0].upper(),id[1].upper(),id,filename), "rb")
	content = f.read()
	f.close()
	return functions.GBK2UTF(content)	

def _getFileHeaders(id):
	try:
		f = open("/home/bbs/bbshome/mail/%s/%s/%s/.DIR"%(id[0].upper(),id[1].upper(),id),"rb")
	#print boardFolder + boardName+ os.path.sep +".DIR"
	except:
		return []
	data = f.read()
	data = functions.parseStruct(DIRstruct,order,256,data)
	f.close()
	for thePost in data:
		endflag = thePost["filename"].find("\0")
		if endflag != -1:
			thePost["filename"] = thePost["filename"][0:endflag]
	return data	

if __name__ == "__main__":
	#print setUnreadCount("FirebirdAPI",2)
	#print getUnreadCount("FirebirdAPI")
	#print json.dumps(getMailList("scaret"))
	mail = sendMail("FirebirdAPI","scaret","HIHI","PUPU")
	print mail
	print getMail("scaret",mail["filename"])
