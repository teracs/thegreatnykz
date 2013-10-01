import logging
import os
import MySQLdb as mysql

#db username
DB_USERNAME = "bbs"
#db password
DB_PASSWORD = "xxxxxxxx"
#db host
DB_HOST = "localhost"
#db name
DB_NAME = "bbs"


conn = None

def initialize():
	global conn
	conn = \
	mysql.connect(host=DB_HOST,user=DB_USERNAME,passwd=DB_PASSWORD,db=DB_NAME)
	return ('sql',)

def handle(cmd,line):
	try:
		conn.query(line)
	except mysql.ProgrammingError: 
	#this error indicates sql statement is incorrect
		logging.exception("child %d, sql error: %s",os.getpid(),line)


def finalize():
	conn.close()
