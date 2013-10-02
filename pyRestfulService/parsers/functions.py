import struct
import re
def parseStruct(lstruct,lorder,structsize,strin):
	block = "*#06#"
	itr = 0
	l = []
	while len(strin) > itr:
		block = strin[itr:itr + structsize]
		info = struct.unpack("".join(lstruct), block)
		j = 0
		d = {}
		for i in info:
			if isinstance(i, str):
				try:
					if len(i) > 10:
						i = GBK2UTF(i)
				except:
					pass
				while len(i)>0 and i[-1] == "\0" or len(i)>1 and i[-2] == "\0" :
					i = i[0:-1]
			d[ lorder[j] ] = i
			j +=1
		l.append(d)
		itr += structsize
	return l

def packStruct(ldata,lorder,structsize):
	for i in range(len(ldata)):
		if type(ldata[i]) == str:
			try:
				ldata[i] = ldata[i].decode("utf8")
			except:
				pass
			try:
				ldata[i] = ldata[i].encode("GBK")
			except:
				pass
	order = "".join(lorder)
	datastr = struct.pack(order,*ldata)
	return datastr


def filterCT(strin):
	pattern = re.compile('\\033\[[0-9;]*[a-zA-Z]')
	clean = pattern.sub('',strin)
	return clean

def GBK2UTF(strin):
	return strin.decode("GBK").encode("utf8")

def GBK2UnicodeEscape(strin):
	return strin.decode("GBK").encode("unicode_escape")

def prettyPrint(l,prefix=0,firstWithPrefix=False,linewidth=78,end=False):
	ret = ""
	if isinstance(l,dict):
		ret += "\n"
		end=False
		for key in l:
			ret +=" "*prefix+prettyPrint(key,end=False)+" : "+prettyPrint(l[key],end=False) + "\n"
	elif isinstance(l,list):
		for i in l:
			ret +=" "*prefix + prettyPrint(i,end=False)
	else:
		l = str(l)
		ret += " "*prefix + l + " "*(13-len(l)%13)
	if not firstWithPrefix:
		while len(ret) and ret[0] == ' ':
			ret = ret[1:]
	if end == True:
		i = 0
		ret2 =""
		while i < len(ret):
			ret2 += ret[i:i+linewidth] + "\n"
			i+=linewidth
		return ret2[:-1]
	else:
		return ret



