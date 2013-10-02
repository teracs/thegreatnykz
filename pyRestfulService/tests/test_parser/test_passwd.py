#!/usr/bin/python
import sys,re
sys.path.append("parser")
import passwd

def test_getUsers():
	users = passwd.getUsers()
	for u in users:
		assert re.search(r"^[a-zA-Z]{2,12}$",u)

def test_getAttr():
	users = passwd.getUsers()

	u = "NoUser!"
	f = passwd.getAttr(u,"userid")
	assert f == False

	for u in users:

		num = passwd.getAttr(u,"money")
		assert num >= 0
		assert num <= 10**6
		assert type(num) == int

		num = passwd.getAttr(u,"birthyear")
		assert num >= 70
		assert num <= 99
		assert type(num) == int

		num = passwd.getAttr(u,"firstlogin")
		assert num * 1000 >= 10 ** 12
		assert num * 1000 <= 20 ** 12
		assert type(num) == int

		s = passwd.getAttr(u,"realname")
		assert type(s) == str
		assert "\0" not in s

		num = passwd.getAttr(u,"notedate")
		assert num * 1000 >= 10 ** 12
		assert num * 1000 <= 20 ** 12
		assert type(num) == int
	
		num = passwd.getAttr(u,"medals")
		assert num >= 0
		assert type(num) == int

		num = passwd.getAttr(u,"lastlogout")
		assert num * 1000 >= 10 ** 12 or num == 0
		assert num * 1000 <= 20 ** 12
		assert type(num) == int	

		s = passwd.getAttr(u,"email")
		assert re.search(r"@[a-zA-Z]+.",s)
		assert type(s) == str
		assert "\0" not in s

		s = passwd.getAttr(u,"termtype")
		assert re.search(r"[a-zA-Z0-9_]{2,12}",s)
		assert type(s) == str
		assert "\0" not in s
		
		s = passwd.getAttr(u,"username")
		assert type(s) == str
		assert "\0" not in s	

		s = passwd.getAttr(u,"reginfo")
		assert type(s) == str
		assert "\0" not in s	

		num = passwd.getAttr(u,"banktime")
		assert num * 1000 >= 10 ** 12 or num == 0
		assert num * 1000 <= 20 ** 12
		assert type(num) == int

		s = passwd.getAttr(u,"passwd")
		assert type(s) == str
		assert "\0" in s

		num = passwd.getAttr(u,"inbank")
		assert num >= 0
		assert num <= 10**6
		assert type(num) == int		

		num = passwd.getAttr(u,"signature")
		assert num >= 0
		assert num <= 10
		assert type(num) == int		

		num = passwd.getAttr(u,"stay")
		assert num >= 0
		assert num <= 10**8
		assert type(num) == int		

		num = passwd.getAttr(u,"lastjustify")
		assert num * 1000 >= 10 ** 12 or num == 0
		assert num * 1000 <= 20 ** 12
		assert type(num) == int

		num = passwd.getAttr(u,"lastlogin")
		assert num * 1000 >= 10 ** 12 or num == 0
		assert num * 1000 <= 20 ** 12
		assert type(num) == int

		s = passwd.getAttr(u,"address")
		assert type(s) == str
		assert "\0" not in s

		num = passwd.getAttr(u,"userlevel")
		assert type(num) == int or type(num) == long
	 
		num = passwd.getAttr(u,"nummails")
		assert num >= 0
		assert type(num) == int

		s = passwd.getAttr(u,"ident")
		assert type(s) == str
		assert "\0" not in s

		s = passwd.getAttr(u,"gender")
		assert len(s) == 1
		assert s == "M" or s == "F"
		assert type(s) == str
		assert "\0" not in s

		s = passwd.getAttr(u,"userid")
		assert re.search(r"[a-zA-Z]{2,12}",s)
		assert type(s) == str
		assert "\0" not in s
		
		num = passwd.getAttr(u,"birthmonth")
		assert num >= 1
		assert num <= 12
		assert type(num) == int

		num = passwd.getAttr(u,"birthday")
		assert num >= 1
		assert num <= 31
		assert type(num) == int

		s = passwd.getAttr(u,"flags")
		assert type(s) == str

		s = passwd.getAttr(u,"lasthost")
		assert len(s) > 1
		assert type(s) == str
		assert "\0" not in s

		num = passwd.getAttr(u,"numlogins")
		assert num >= 0
		assert num <= 10**6
		assert type(num) == int

		somethingelse = passwd.getAttr(u,"somethingelse")
		assert somethingelse == False	

def test_checkPasswd():
	id = "someone not in list"
	assert not passwd.checkPasswd(id,"passwd")

	id = "FirebirdAPI"
	assert not passwd.checkPasswd(id,"incorrectpasswd")
	
	id = "FirebirdAPI"
	assert passwd.checkPasswd(id,"Firebird")
	


