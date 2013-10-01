from ConfigParser import SafeConfigParser
from os import path
import sys

CONF_FILENAME = '/usr/local/dnssec/dnssec.ini'

KEYSET_DIR = 'keyset'
KEY_DIR = 'key'
DB_DIR = 'db'
LOCK_FILE = 'zone.lock'

KEY_DB = 'keys'
DATA_DB = 'data'
IP_DB = 'ips'

class ConfException(Exception):
	pass

_conf = None

def _readconf(fname=None):
	global _conf
	if not fname:
		fname = CONF_FILENAME
	_conf = SafeConfigParser()
	_conf.read([fname])


def get(domain=None):
	if not _conf:
		_readconf()
	if not domain:
		domain = 'DEFAULT'
	return dict(_conf.items(domain))

def getdomains():
	if not _conf:
		_readconf()
	return _conf.sections()
