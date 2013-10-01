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
    except mysql.MySQLError:
        logging.exception("child %d, sql error: %s",os.getpid(),line)
        try:
            conn.store_result() # clear result if any
            conn.query("SELECT 1;")
        except mysql.MySQLError:
            # mysql can not execute a simple query
            # so this is a server error which can not be handled here
            raise Exception("mysql server error")
        else:
            conn.store_result() # clear select 1 result

def finalize():
    conn.close()
