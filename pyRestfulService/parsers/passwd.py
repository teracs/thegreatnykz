#coding: utf-8

# parse the file ".PASSWDS"
# no encoding convertion
# any question contact scaret: i @ scaret.in
#
# getUsers()         return a username list
# getAttr(id,attr)      throws an error while not found
# 因为我们是高富帅，所以请直接使用getAllUserAllInfo()
#
# _getIdFromBlock(block)  False while not found
# _getBlockById(id)      False while not found
# _getUserFullInfoFromBlock(block)
#                           dict, False while not found


import struct,os
import sys,functions

from PERM import *
import config

passwdfile  =  config.dir_bbshome + "/.PASSWDS"
whereischk  =  os.path.split(os.path.realpath(__file__))[0] + "/checkPasswd"
structlen   =  512
maxidlen  =  14
order  =  [  "userid",  "firstlogin",  "lasthost",  "numlogins",  "medals",  "money",  "inbank",  "banktime",    "flags",  "passwd",  "username",  "ident",  "termtype",  "reginfo",  "userlevel",  "lastlogin",  "lastlogout",  "stay",    "realname",  "address",  "email",  "nummails",  "lastjustify",  "gender",  "birthyear",  "birthmonth",  "birthday",  "signature",  "userdefine",  "notedate",  "noteline"  ]
blockstruct=[  "14s",    "l",      "60s",    "I",      "I",    "I",    "I",    "l",      "3s",    "17s",    "40s",    "40s",    "16s",    "64s",    "I",      "l",      "l",      "l",    "40s",    "80s",    "68s",    "I",    "l",      "1s",    "B",      "B",      "B",    "i",      "i",      "l",    "i"      ]

# 高富帅函数，一次读取PASSWD文件，然后将所有用户以及所有信息返回
def getAllUserAllInfo():
  u = "*#06#"
  f = open(passwdfile,"rb")
  users = {}
  block = f.read(structlen)
  while block:
    userid = _getIdFromBlock(block)
    if userid:
      users[userid] = _getUserFullInfoFromBlock(block)
    block = f.read(structlen)
  f.close()
  return users

# 矮矬穷函数，返回所有用户
def userIndex():
  u ="*#06#"
  f = open(passwdfile,"rb")
  block = f.read(structlen)
  users = []
  while block:
    id = _getIdFromBlock(block)
    if id and id[0] != '\0':
      users.append(id)
    block = f.read(structlen)
  f.close()
  return users

# 矮帅富函数,输入为一个id的数组
# 没这个人的话，返回不会有的
def getUsers(idarr):
  ret = {}
  for id in idarr:
    block = _getBlockById(id)
    if block:
      user = _getUserFullInfoFromBlock(block)
      ret[id] = user
  return ret

# 获取某个用户的某个属性。没事少用这个函数，直接用getUser
def getAttr(id = "*#06#",attr = "*#06#"):
  b = _getBlockById(id)
  if not b:
    return False
  i = _getUserFullInfoFromBlock(b)
  if attr not in i.keys():
    return False
  return i[attr]

# 检验用户密码是否正确。外部有一个c语言的可执行文件依赖。
def checkPasswd(id="*#06#",password="*#06#"):
  if not (_getBlockById(id)):
    return False
  encrypted = getAttr(id,"passwd")[-13:]
  cmd = whereischk +" '" + password[0:8] + "' " + encrypted
  print cmd
  check = os.popen(cmd).readlines()
  return check[0] == "1"

def _getIdFromBlock(block):
  u = block[0:maxidlen]
  eof = u.find("\0",1)
  if eof == 1:
    return False
  elif eof == -1:
    return u
  else:
    return u[0:eof]

def _getBlockById(id):
  f = open(passwdfile,"rb")
  block = f.read(structlen)
  while block:
    if id == _getIdFromBlock(block):
      f.close()
      return block
    block = f.read(structlen)
  return False

def _getUserFullInfoFromBlock(block):
  info = struct.unpack("".join(blockstruct),block)
  if info[0] == "\0" * maxidlen:
    return False
  d = {}
  cnt = 0
  for i in info:
    if isinstance(i,str):
      while len(i) > 0 and i[-1] == "\0":
        i = i[0:-1]
      i=functions.GBK2UTF(i)
    d[order[cnt]] = i
    cnt += 1
  return d

if __name__ == "__main__":
  if len( sys.argv ) == 1:
    print functions.prettyPrint(sorted(getUsers())),
  elif len( sys.argv ) == 2:
    print functions.prettyPrint(_getUserFullInfo(sys.argv[1])),
  elif len( sys.argv) == 3:
    print getAttr( sys.argv[1], sys.argv[2] ),
  elif len( sys.argv) == 4 and sys.argv[2].upper() == "PASSWD":
    print checkPasswd(sys.argv[1],sys.argv[3])
