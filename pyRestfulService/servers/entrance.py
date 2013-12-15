#!/usr/bin/python
#coding=utf-8
import web,sys

import mimerender

sys.path.append("parsers")
sys.path.append("apis")
sys.path.append("servers")
sys.path.append("../parsers")
sys.path.append("../apis")
sys.path.append("../servers")

from includes import *
from bbsuser import bbsuser
from bbsallusers import bbsallusers
from bbsuserindex import bbsuserindex
from bbscheck import bbscheck
from bbspostindex import bbspostindex
from bbspost import bbspost
import bbslogin
from bbsboard import *
from bbsboardindex import *
from bbsmail import *
urls = (
	'/bbsuser/(.*)\.(.*)','bbsuser',
	'/bbsuser/(.*)','bbsuser',

  '/bbsallusers\.(.*)','bbsallusers',
  '/bbsallusers','bbsallusers',

  '/bbsuserindex\.(.*)','bbsuserindex',
  '/bbsuserindex','bbsuserindex',

  '/bbsboard\.(.*)','bbsboard',
  '/bbsboard','bbsboard',

  '/bbsboardindex\.(.*)','bbsboardindex',
  '/bbsboardindex','bbsboardindex',

  '/bbspostindex\.(.*)','bbspostindex',
  '/bbspostindex','bbspostindex',

  '/bbspost\.(.*)','bbspost',
  '/bbspost','bbspost',

  '/bbsmailindex\.(.*)','bbsmailindex',
  '/bbsmailindex','bbsmailindex',

  '/bbsmail\.(.*)','bbsmail',
  '/bbsmail','bbsmail',

	'/bbslogin\.(.*)','bbslogin.bbslogin',
	'/bbslogin','bbslogin.bbslogin',

	'/bbscheck\.?(.+)?','bbscheck',

	'(.*)', 'greet'
)
app = web.application(urls, globals())
session = web.session.Session(app, web.session.DiskStore('sessions'), initializer={'id':'guest'})
class greet:
    def GET(self, path):
        return "Hello,World~ your path:" + path

if __name__ == "__main__":
    app.run()

