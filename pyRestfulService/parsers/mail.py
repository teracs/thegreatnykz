#coding:utf8
import struct
import sys
import os
import functions
import passwd
import json
import time
import config
def getUnreadCount(user):
  try:
    f = open(config.dir_bbshome + "/tmp/"+user+".mailsize","rb")
    cnt = int(f.read())
    f.close()
  except:
    return -1
  return cnt

def setUnreadCount(user,cnt):
  try:
    f = open(config.dir_bbshome + "/tmp/"+user+".mailsize","wb")
    f.write(str(cnt))
    f.close()
  except:
    return -1
  return cnt

DIRstruct   =  ["80s",    "76s",    "I",  "80s",    "I",    "4s",    "I",  "I"    ]
order    =  ["filename",  "owner",  "pnum",  "title",  "level",  "accessed",  "id",  "reid"  ]

# 高帅富函数
def getAllMailList(id):
  return _getFileHeaders(id)

# 矮矬穷函数
def getMailList(id,startfrom1 = 99999, count = 20):
  try:
    f = open(config.dir_bbshome + "/mail/%s/%s/%s/.DIR"%(id[0].upper(),id[1].upper(),id),"rb")
  #print boardFolder + boardName+ os.path.sep +".DIR"
  except:
    return []
  if startfrom1 < 0 or startfrom1 > countMail(id):
    startfrom1 = countMail(id) - count +1
  data = f.read((startfrom1 - 1)* 256)
  data = f.read(count * 256)
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

def sendMail(idfrom,idto,title,content,reid = 0):
  fileid = int(time.time())
  while "M.%d.A"%fileid in getMailList(idto):
    fileid +=1
  f = open(config.dir_bbshome + "/mail/%s/%s/%s/%s" \
    %(idto[0].upper(),idto[1].upper(),idto,"M.%d.A"%fileid), "wb")
  f.write(content)
  f.close()
  f = open(config.dir_bbshome + "/mail/%s/%s/%s/.DIR"%(idto[0].upper(),idto[1].upper(),idto),"ab")
  if not reid:
    reid = fileid
  reid = int(reid)
  ldata = ["M.%d.A"%fileid,"%s (%s)"%(idfrom,idfrom),0,title,0,"",fileid,reid]
  data = functions.packStruct(ldata,DIRstruct,256)
  f.write(data)
  f.close()
  cnt = getUnreadCount(idto)
  setUnreadCount(idto,cnt+1)
  return dict(zip(order,ldata))

def getMail(id,filename):
  f = open(config.dir_bbshome + "/mail/%s/%s/%s/%s" \
    %(id[0].upper(),id[1].upper(),id,filename), "rb")
  content = f.read()
  f.close()
  return functions.GBK2UTF(content)

# 返回最大邮件号
def countMail(id):
  dirfilesize = os.path.getsize(config.dir_bbshome + "/mail/%s/%s/%s/.DIR"%(id[0].upper(),id[1].upper(),id))
  count = dirfilesize/256
  return count

def _getFileHeaders(id):
  try:
    f = open(config.dir_bbshome + "/mail/%s/%s/%s/.DIR"%(id[0].upper(),id[1].upper(),id),"rb")
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
  print countMail("Teracs")
  print getMailList("Teracs")
