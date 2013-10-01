#!/usr/bin/env python
# -*- coding: gb2312 -*-
"""
Created on Sun Jun 13 14:47:11 2010

@author: eaufavor
"""

import os

def maxdir(level,path): 
    now = 0
    latest_file = path
    try:
        now = os.stat(path).st_mtime
    except:
        return 0
    now1 = now
    for i in os.listdir(path): 
        if os.path.isdir(path + '/' + i):  
            c,f = maxdir(level+1, path + '/' + i)  
            if now < c:
                now = c
	        latest_file = f
    if now1 < now:
        os.system("touch "+path+" -r "+latest_file)
    return now,latest_file
														            
															             
os.chdir("../bbshome/0Announce/groups")
maxdir(0,".")
