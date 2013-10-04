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
from bbscheck import bbscheck
from bbspostindex import bbspostindex
from bbspost import bbspost
import bbslogin
from bbsboard import *
urls = (
	'/bbsuser/(.*)\.(.*)','bbsuser',
	'/bbsuser/(.*)','bbsuser',

  '/bbsallusers\.(.*)','bbsallusers',
  '/bbsallusers','bbsallusers',

  '/bbsboard\.(.*)','bbsboard',
  '/bbsboard','bbsboard',

  '/bbspostindex\.(.*)','bbspostindex',
  '/bbspostindex','bbspostindex',

  '/bbspost\.(.*)','bbspost',
  '/bbspost','bbspost',

	'/bbslogin\.(.*)','bbslogin.bbslogin',
	'/bbslogin','bbslogin.bbslogin',

	'/bbscheck.*','bbscheck',
	'(.*)', 'greet'
)
app = web.application(urls, globals())
session = web.session.Session(app, web.session.DiskStore('sessions'), initializer={'id':'guest'})
class greet:
    def GET(self, path):
        return web.template.render('template/', cache=False).index(path)

if __name__ == "__main__":
    app.run()

