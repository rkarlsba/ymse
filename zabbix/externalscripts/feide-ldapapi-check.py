#!/bin/env python
import requests, json
import datetime
import sys
#
# https://ssltest.feide.no/
#
API='https://ldapapi.feide.no/test'
data={ "host": "ldap.oslomet.no", "port": 636}
iplist=["2001:700:700:1::67","158.36.161.67"]
protocol=["TLS 1.2"]
cipher=["TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256"]
try:
	r = requests.post(url = API, data = data)
	j = r.json()
# 'application/json' in r.headers.get('Content-Type')
except:
        print "CRITICAL: no json reply from %s for %s" % (API,data)
	sys.exit(1)

now = datetime.datetime.now()
warn = ""
crit = ""
n_warn = 0
n_crit = 0
hosts = 0

for h in j:
	#h['ok'] = "1234512345678900"
	#h.["ip"] = False
	hosts += 1
	if h.get("ok",False) != True:
		n_crit += 1
		crit = crit + "NOT OK - expected ok as response from ldapapi.feide.no/test ,"
	if not h.get("ip") in iplist:
		n_crit += 1
		crit = crit + "%s is not in accepted testlist, " % h.get("ip")

	if h.get('certificates',None) is not None:
		for certs in h.get('certificates'):
			not_after=datetime.datetime.strptime(certs.get("not_after"),r'%Y-%m-%dT%H:%M:%SZ')
			not_after_d=datetime.datetime.strftime(not_after,'%Y-%m-%d')
			diff_a = (not_after - now).days
			not_before=datetime.datetime.strptime(certs.get("not_before"),r'%Y-%m-%dT%H:%M:%SZ')
			not_before_d=datetime.datetime.strftime(not_before,'%Y-%m-%d')
			diff_b = (now - not_before).days
			if diff_a < 4:
				n_crit += 1
				crit = crit + "%s chain expires in %s days(%s), " % (h.get("ip"), diff_a, not_after_d)
			elif diff_a < 10:
				n_warn += 1
				warn = warn + "%s chain expires in %s days(%s), " % (h.get("ip"), diff_a, not_after_d)
			if diff_b < 0:
				n_crit += 1
				crit = crit + "%s chain not valid yet, will be in %s days(%s), " % (h.get("ip"), diff_b, not_before_d)
			elif diff_b < 2:
				n_warn += 1
				warn = warn + "%s chain just became valid %s days ago(%s), " % (h.get("ip"), diff_b, not_before_d)
	
if hosts < len(iplist):
	n_crit +=1
	crit = crit + "NOT TESTING ALL HOSTS. Got %s of %s, checking ldap.oslomet.no externally from ssltest.feide.no" % (hosts,len(iplist))

if n_crit > 1 or crit<>"":
	print "CRITICAL: %s (%sc/%sw), ldap.oslomet.no tested from ssltest.feide.no" % (crit,n_crit,n_warn)
	sys.exit(1)

if n_warn > 1 or warn<>"":
	print "WARNING: %s (%sc/%sw), ldap.oslomet.no tested from ssltest.feide.no" % (warn,n_crit,n_warn)
	sys.exit(1)

if n_warn == 0 and n_crit == 0:
	print("OK: LDAP and DNS external lookup test, ldap.oslomet.no tested from ssltest.feide.no")
	sys.exit(0)

print("UNKNOWN status. should not get here? ldap.oslomet.no test via external ssltest.feide.no")
sys.exit(1)
