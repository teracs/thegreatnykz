#!/usr/bin/python
import sys,re
sys.path.append("parser")
import boardAllow

def test_isKBoard():
	assert boardAllow.isKBoard("sysop") == False
	assert boardAllow.isKBoard("boardNotExist") == False
	assert boardAllow.isKBoard("kboard") == True

def test_isInBoard():
	assert boardAllow.isInBoard("scaret","kboard") == True
	assert boardAllow.isInBoard("FireBirdAPI","kboard") == False

