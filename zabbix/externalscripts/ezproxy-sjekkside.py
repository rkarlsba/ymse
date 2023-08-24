#!/bin/env python3
import requests
import json
import datetime
import sys
import re
from bs4 import BeautifulSoup
s = requests.Session()


# server = "ezproxy-dev.oslomet.no"
# server = "ezproxy.oslomet.no"

if len(sys.argv) < 2:
    print("ERROR - Missing hostname as parameter")
    sys.exit(1)

server = sys.argv[1]

ez_user = "ezzabbix"
ez_pw = ".-.SAPP[I1lL]0O0oOER,!#,ZCSD//"

error = 0
ok = 0
msg = ""

API = f"https://{server}/login"
logouturl = f'https://{server}/logout'
data = {"url": "^U", "user": ez_user, "pass": ez_pw}

# user configurable via GET http://ezproxy.oslomet.no/auth with querystring ?force=0|1|2
# Cookie: ezproxyrequireauthenticate2   force authentication: 0=Never, 1=Always, 2= until you close all browser windows

# curl 'https://ezproxy.oslomet.no/auth?force=1'
# -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:104.0) Gecko/20100101 Firefox/104.0'
# -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8'
# -H 'Accept-Language: en-US,en;q=0.5' -H 'Accept-Encoding: gzip, deflate, br' -H 'DNT: 1'
# -H 'Connection: keep-alive'
# -H 'Referer: https://ezproxy.oslomet.no/auth?force=2'
# -H 'Cookie: ezproxyrequireauthenticate2=2'
# -H 'Upgrade-Insecure-Requests: 1'
# -H 'Sec-Fetch-Dest: document' -H 'Sec-Fetch-Mode: navigate'
# -H 'Sec-Fetch-Site: same-origin' -H 'Sec-Fetch-User: ?1'

# https://eu01.alma.exlibrisgroup.com/
# print(r)
# print(r.content)
# print(r.text)
# "OsloMet — Off Campus Access Database List"
# attr = { 'class': re.compile(r'message__[a-zA-Z]*|views-row|view-content|field'), }
# title,pubdate,desc,start,end,solved = "","","",0,0,0
# soup.find(['title']).text == "OsloMet — Off Campus Access Database List":



# API = f"https://login.{server}/login"

status = {"ok": 0, "error": 0, "loc": [], "msg": ""}


# print(f"{side}: virket")
#         if soup.find(['title']).text == "OsloMet — Off Campus Access Database List":

# response = requests.post(url, data=post_fields, timeout=timeout)
import time
timing_debug = False
tider = []
brukttid = []
def sjekkurl(apiurl, side="", postdata={}, find=[{}]):
    side_start = time.perf_counter()
    tider.append({f"start {side}": time.perf_counter()})
    r = None
    try:
        tider.append({"try": time.perf_counter()})
        r = s.post(url=apiurl, data=postdata)
        tider.append({f"post {side}": time.perf_counter()})
        timing2 = r.elapsed.total_seconds()
        tider.append({"r.elapsed.total_seconds": time.perf_counter()})
        # print(r.elapsed.total_seconds())
        soup = BeautifulSoup(r.text, 'html.parser')
        tider.append({f"soup {side}": time.perf_counter()})
        for f in find:
            tider.append({f"find_loop {side}: {f}": time.perf_counter()})
            if f.get("what", None) is None:
                if f.get("expect") in r.text:
                    status["ok"] += 1
                    tider.append({f"{side} Fant {f}": time.perf_counter()})
                    status["loc"].append({"side": side, "ok": 1, "error": 0, "info": f"virket: {f}"})
                else:
                    status["error"] += 1
                    status["msg"] += f"{side} feilet. "
                    tider.append({f"{side} Fant ikke {f}": time.perf_counter()})
                    status["loc"].append({"side": side, "ok": 0, "error": 1, "info": f"feilet. Ingen match av {f}"})
            if f.get("what", None) is not None:
                if soup.find([f.get("what")]).text == f.get("expect"):
                    status["ok"] += 1
                    tider.append({f"{side} Fant {f}": time.perf_counter()})
                    status["loc"].append({"side": side, "ok": 1, "error": 0, "info": f"virket: {f}"})
                else:
                    status["error"] += 1
                    status["msg"] += f"{side} feilet. "
                    tider.append({f"{side} Fant ikke {f}": time.perf_counter()})
                    status["loc"].append({"side": side, "ok": 0, "error": 1, "info": f"feilet. Ingen soup-match av {f}"})
        tider.append({f"{side} end_find_loop": time.perf_counter()})
    except Exception as e:
        # print(f"{e} {e.args} {e.with_traceback()}")
        # print(f"exception {e} {e.args} {sys.exc_info()[2]}")
        status["msg"] += f"{side} feilet (exception). "
        status["error"] += 1
        status["loc"].append({"side": side, "error": 1, "ok": 0, "info": "feilet", "exception": f"exception {e} {e.args} {sys.exc_info()[2]}"})
        tider.append({f"{side} exception {e}": time.perf_counter()})
    tider.append({f"{side} end try": time.perf_counter()})
    last_t = 0
    if timing_debug: 
      for tid in tider:
        for tn in tid:
             t = tid.get(tn)
             print(f"{tn}: {t - last_t:.3f}")
             last_t = t
    brukttid.append({f"{side}": time.perf_counter() - side_start})
#     print(f"Timing {side}: {t_start - t_try}, {t_try - t_post}, {t_post - t_r_timing}, {t_r_timing - t_soap}")
#     print(f"{timing} Request completed in {r_post_time:.0f}ms")
    return r


side = "ezproxy_login"
data["url"] = "^U"
find = [{"what": "title", "expect": "OsloMet — Off Campus Access Database List"}]
login_r = sjekkurl(apiurl=API, side=side, postdata=data, find=find)

if (login_r):
    side = "standard.no"
    data["url"] = "https://www.standard.no/no/Brukersider/Logg-inn-ekstranett/?UID=d648669d93cca2e0e0e5e7f6f1cc4933&WPPTSMyPageMenu=1"
    find = [{"expect": "OsloMet – storbyuniversitetet"}, {"expect": "ub.digitalt@oslomet.no"}, {"expect": "Standarder"}]
    # find = [{"expect": "ub.digitalt@oslomet.no"}]
    sjekkurl(apiurl=API, side=side, postdata=data, find=find)
# print(sjekkurl(apiurl=API, side=side, postdata=data, find=find))

# side = "proquest"
# data["url"] = "https://www.proquest.com"
# find = [{"what": "script", "expect": "university"}]



# print(brukttid)
if status.get("error") > 0:
    print(f"ERROR - {status.get('error')} error: {status['msg']}")
    # print(status)
    sys.exit(1)

sider = ",".join([loc.get("side", "na") for loc in status.get("loc", {})])
# print(brukttid)
pentid = " ".join([f"{b}={a.get(b)}" for a in brukttid for b in a])
sumtid = sum([a.get(b) for a in brukttid for b in a])

# pentid = " ".join([a for a in brukttid])
# pentid = " ".join([brukttid.get(loc) for loc in status.get("loc", {})])
#       for tid in tider:
# 	              for tn in tid:
	#t = tid.get(tn)

r_logout = s.get(url=logouturl)
# if r_logout.status_code == 200 and "You are now logged out" in r_logout.text:
#    print("Logget ut")
# print(r_logout)
# print(r_logout.status_code)
# print(r_logout.text)

if status.get("ok") > 1:
    # print(status)
    print(f"OK - ezproxy testet {sider} {pentid} timing={sumtid}")
    sys.exit(0)


sys.exit(1)



# for tag in soup.find_all(['title']):
#       if tag.text
#       print(tag.text)

# for tag in soup.find_all(['title'], attrs=attr):
#       print("##",str(tag),str(tag["class"]),"##")

#    if ('views-row' in tag["class"]):
 #       if (not solved or not end) and (title and start ) and not (title in "FileSender - Problemer med treghet" and "08.04.2022 08:00" in start):




#    1010 www.proquest.com
#    1057 dl.acm.org
#    1082 www.radiologictechnology.org
#    1102 www.helsebiblioteket.no
#    1151 journals.lww.com
#    1189 ovidsp.ovid.com
#    1206 bristoluniversitypressdigital.com
#    1460 static.apa.org
#    1497 lovdata.no
#    1735 www.scopus.com
#    1835 openurl.ebsco.com
#    1877 access.clarivate.com
#    2255 ovid.visiblebody.com
#    2323 www.byggforsk.no
#    2338 search.ebscohost.com
#    2410 ieeexplore.ieee.org
#    3166 hioa.eblib.com
#    3239 ars.els-cdn.com
#    3862 www.emerald.com
#    5263 www.uptodate.com
#    5269 events.taylorandfrancis.com
#    5442 link.springer.com
#    5947 www.webofscience.com
#    6374 psycnet.apa.org
#    6434 oce.ovid.com
#    7480 api.taylorfrancis.com
#    7754 www.cambridge.org
#    8020 www.standard.no
#    9619 www.taylorfrancis.com
#    9648 www.sciencedirect.com
#   11359 web.s.ebscohost.com
#   13264 web.p.ebscohost.com
#   14582 onlinelibrary.wiley.com
#   19859 journals.sagepub.com
#   23723 www.tandfonline.com
#   26867 www.idunn.no
#   63410 www.jstor.org
#   65378 ovidsp.dc1.ovid.com
#  117977 ezproxy.oslomet.no
#  175951 ebookcentral.proquest.com


#   11502 oup.silverchair-cdn.com
#   11626 idp.springer.com
#   11627 www.ingentaconnect.com
#   12033 dx.doi.org
#   12246 status-app.jstor.org
#   13058 largecontent.ebsco-content.com
#   13415 openurl.bibsys.no
#   13479 logon.ebsco.zone
#   13942 cdn.ncbi.nlm.nih.gov
#   14264 legehandboka.no
#   15804 www.pressreader.com
#   15953 www.helsebiblioteket.no
#   16075 doi.org
#   17967 miqv.ebsco-content.com
#   18670 rum.scopus.com
#   19053 lovdata.no
#   20566 access.clarivate.com
#   20602 app.knovel.com
#   22038 www.proquest.com
#   22792 www.cochranelibrary.com
#   23515 ovid.visiblebody.com
#   27281 ovidsp.ovid.com
#   32516 dl.acm.org
#   34703 sk.sagepub.com
#   37313 components.scopus.com
#   37489 www.emerald.com
#   38432 www.ordnett.no
#   38975 methods.sagepub.com
#   39940 static.apa.org
#   45601 bestpractice.bmj.com
#   54525 hioa.eblib.com
#   61653 oxford.universitypressscholarship.com
#   61915 www.webofscience.com
#   62421 ieeexplore.ieee.org
#   64395 search.ebscohost.com
#   69061 www.ncbi.nlm.nih.gov
#   74307 events.taylorandfrancis.com
#   83373 openurl.ebsco.com
#   87760 www.scopus.com
#   97714 www.byggforsk.no
#   99360 ars.els-cdn.com
#  108791 www.taylorfrancis.com
#  113647 api.taylorfrancis.com
#  126547 oce.ovid.com
#  152807 link.springer.com
#  178863 www.cambridge.org
#  184162 psycnet.apa.org
#  197833 www.standard.no
#  300674 web.s.ebscohost.com
#  307616 www.uptodate.com
#  322646 www.sciencedirect.com
#  341126 web.p.ebscohost.com
#  459882 onlinelibrary.wiley.com
#  609126 www.tandfonline.com
#  659661 www.idunn.no
#  674820 journals.sagepub.com
#  818581 ovidsp.dc1.ovid.com
#  890096 if.ebsco-content.com
#  982222 ovidsp.dc2.ovid.com
# 1025658 www.jstor.org
# 3018549 ebookcentral.proquest.com
# 4435264 ezproxy.oslomet.no

#
# data={ "host": "ldap.oslomet.no", "port": 636}
# iplist=["2001:700:700:1::67","158.36.161.67"]
# protocol=["TLS 1.2"]
# cipher=["TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256"]
# try:

# j = r.json()
# 'application/json' in r.headers.get('Content-Type')
# except:
#        print "CRITICAL: no json reply from %s for %s" % (API,data)
#        sys.exit(1)

now = datetime.datetime.now()
# warn = ""
# crit = ""
# n_warn = 0
# n_crit = 0
# hosts = 0

# for h in j:
# h['ok'] = "1234512345678900"
# h.["ip"] = False
# hosts += 1
# if h.get("ok",False) != True:
#        n_crit += 1
#        crit = crit + "NOT OK reported by feide, "
# if not h.get("ip") in iplist:
#        n_crit += 1
#        crit = crit + "%s is not in accepted testlist, " % h.get("ip")

#         if h.get('certificates',None) is not None:
#                 for certs in h.get('certificates'):
#                         not_after=datetime.datetime.strptime(certs.get("not_after"),r'%Y-%m-%dT%H:%M:%SZ')
#                         not_after_d=datetime.datetime.strftime(not_after,'%Y-%m-%d')
#                         diff_a = (not_after - now).days
#                         not_before=datetime.datetime.strptime(certs.get("not_before"),r'%Y-%m-%dT%H:%M:%SZ')
#                         not_before_d=datetime.datetime.strftime(not_before,'%Y-%m-%d')
#                         diff_b = (now - not_before).days
#                         if diff_a < 4:
#                                 n_crit += 1
#                                 crit = crit + "%s chain expires in %s days(%s), " % (h.get("ip"), diff_a, not_after_d)
#                         elif diff_a < 10:
#                                 n_warn += 1
#                                 warn = warn + "%s chain expires in %s days(%s), " % (h.get("ip"), diff_a, not_after_d)
#                         if diff_b < 0:
#                                 n_crit += 1
#                                 crit = crit + "%s chain not valid yet, will be in %s days(%s), " % (h.get("ip"), diff_b, not_before_d)
#                         elif diff_b < 2:
#                                 n_warn += 1
#                                 warn = warn + "%s chain just became valid %s days ago(%s), " % (h.get("ip"), diff_b, not_before_d)
#
# if hosts < len(iplist):
#         n_crit +=1
#         crit = crit + "NOT TESTING ALL HOSTS. Got %s of %s" % (hosts,len(iplist))
#
# if n_crit > 1 or crit<>"":
#         print "CRITICAL: %s (%sc/%sw)" % (crit,n_crit,n_warn)
#         sys.exit(1)
#
# if n_warn > 1 or warn<>"":
#         print "WARNING: %s (%sc/%sw)" % (warn,n_crit,n_warn)
#         sys.exit(1)
#
# if n_warn == 0 and n_crit == 0:
#         print("OK: ldap external LDAP and dns lookup test ok")
#         sys.exit(0)
#
# print "UNKNOWN status. should not get here?"
sys.exit(1)
