import struct
import sys
import os
import functions
import time

boardFolder	=	"/home/bbs/bbshome/boards/"

DIRstruct 	=	["80s",		"76s",		"I",	"80s",		"I",		"4s",		"I",	"I"		]
order		=	["filename",	"owner",	"pnum",	"title",	"level",	"accessed",	"id",	"reid"	]


def getPostsList(boardName):
	return _getFileHeaders(boardName)

def _getFileHeaders(boardName):
	if boardName == "":
		return []
	try:
		f = open(boardFolder + boardName +"/.DIR","rb")
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

def createPost(boardName,user,title,content,reid=False):
	f = open(boardFolder + boardName +"/.DIR","ab")
	fileid = int(time.time())
	while "M.%d.A"%fileid in [i["filename"] for i in getPostsList(boardName)]:
		fileid +=1
	filepath = "%s%s/%s/%s"%(boardFolder,boardName,str(fileid%500).zfill(3),"M."+str(fileid)+".A")
	print filepath
	f = open(filepath, "wb")
	f.write(content)
	f.close()
	f = open(boardFolder + boardName +"/.DIR","ab")
	ldata = ["M.%d.A"%fileid,"%s"%(user,user),0,title,0,"",fileid,reid and reid or fileid]
	data = functions.packStruct(ldata,DIRstruct,256)
	f.write(data)
	f.close()
	return dict(zip(order,ldata))



if __name__ == "__main__":
	print createPost("110","scaret","new Post","xixi")
