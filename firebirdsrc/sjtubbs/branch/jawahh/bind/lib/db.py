import utils
import conf
from os import path
import os
from bsddb.db import DBNoSuchFileError

class DBException(Exception):
	pass

dbs = {}

def init_db(domain):
	db_dir = path.join(conf.get(domain)['dir'],conf.DB_DIR)
	k = path.join(db_dir,conf.KEY_DB)
	d = path.join(db_dir,conf.DATA_DB)
	i = path.join(db_dir,conf.IP_DB)
	kdb = utils.opendb(k,True)
	ddb = utils.opendb(d,True)
	idb = utils.opendb(i,True)
	dbs[domain] = (ddb,kdb,idb)

def open_db(domain):
	if dbs.has_key(domain): return
	db_dir = path.join(conf.get(domain)['dir'],conf.DB_DIR)
	k = path.join(db_dir,conf.KEY_DB)
	d = path.join(db_dir,conf.DATA_DB)
	i = path.join(db_dir,conf.IP_DB)
	try:
		kdb = utils.opendb(k)
		ddb = utils.opendb(d)
		idb = utils.opendb(i)
	except DBNoSuchFileError:
		raise DBException('can not open db for '+domain+'. initialize first!')
	dbs[domain] = (ddb,kdb,idb)

def get_ddb(domain):
	open_db(domain)
	return dbs[domain][0]

def get_kdb(domain):
	open_db(domain)
	return dbs[domain][1]

def get_idb(domain):
	open_db(domain)
	return dbs[domain][2]

def close_db(domain):
	ddb,kdb,idb = dbs[domain]
	del dbs[domain]
	kdb.close()
	ddb.close()
	idb.close()

def close_alldb():
	global dbs
	for k in dbs:
		dbs[k][0].close()
		dbs[k][1].close()
		dbs[k][2].close()
	dbs = {}
