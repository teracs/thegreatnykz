#coding:utf8
import struct
import sys
import os
import functions
import time
import config
boardFolder  =  config.dir_bbshome + "/boards/"

DIRstruct   =  ["80s",    "76s",    "I",  "80s",    "I",    "4s",    "I",  "I"    ]
order    =  ["filename",  "owner",  "pnum",  "title",  "level",  "accessed",  "id",  "reid"  ]

# 高帅富函数
def getAllPostsList(boardName):
  return _getFileHeaders(boardName)

# 矮得比较好的函数
def getPostsList(boardName,startfrom1  = 999999,count = 20):
  if boardName == "":
    return []
  try:
    f = open(boardFolder + boardName +"/.DIR","rb")
  #print boardFolder + boardName+ os.path.sep +".DIR"
  except:
    return []
  if startfrom1 < 0 or startfrom1 > countPost(boardName):
    startfrom1 = countPost(boardName) - count +1
  data = f.read( (startfrom1 - 1) * 256 ) #放掉一些
  data = f.read( count * 256 )
  if not data:
    return []
  data = functions.parseStruct(DIRstruct,order,256,data)
  f.close()
  i = 0
  for thePost in data:
    thePost["index"] = startfrom1 + i
    i += 1
    endflag = thePost["filename"].find("\0")
    if endflag != -1:
      thePost["filename"] = thePost["filename"][0:endflag]
  return data

# 返回最大帖子号
def countPost(boardName):
  dirfilesize = os.path.getsize(boardFolder + boardName +"/.DIR")
  count = dirfilesize/256
  return count

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
  print countPost("NetHack")
  print getPostsList("NetHack")
