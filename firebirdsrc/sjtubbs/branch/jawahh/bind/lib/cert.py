import M2Crypto.X509 as x509
import M2Crypto.EVP as evp
import M2Crypto.RSA as rsa
import M2Crypto.ASN1 as asn1

import time
import os.path as path
import conf

RSA_E = 0x10001

def time2asn1(t=None):
	if t is None: t = time.time()
	at = asn1.ASN1_UTCTIME()
	at.set_time(int(t))
	return at

def asn12time(asn1):
	ts = time.strptime(str(asn1),"%b %d %H:%M:%S %Y %Z")
	return time.mktime(ts) - time.timezone


def gen_pkey(bits=1024):
	pk = evp.PKey()
	k = rsa.gen_key(bits,RSA_E)
	pk.assign_rsa(k)
	return pk

def get_trusted_crt(domain):
	dc = conf.get(domain)
	return x509.load_cert(path.join(dc['dir'],dc['trusted_x509_certfile']))
	
