#!/usr/bin/python

import sys,os
sys.path.append(os.getcwd() + "/servers" )
sys.path.append(os.getcwd() + "/apis" )
sys.path.append(os.getcwd() + "/parsers" )



from tests.test_parser.test_passwd import *
from tests.test_parser.test_board import *
from tests.test_parser.test_boardAllow import *
from tests.test_parser.test_boardDIR import *
from tests.test_parser.test_post import *

