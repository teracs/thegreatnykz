#!/usr/bin/python

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
import bbslogin

urls = (
	'/bbsuser/(.*)\.(.*)','bbsuser',
	'/bbsuser/(.*)','bbsuser',

	'/bbsallusers\.(.*)','bbsallusers',
	'/bbsallusers','bbsallusers',

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

