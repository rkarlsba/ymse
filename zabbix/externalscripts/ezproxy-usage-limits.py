#!/bin/env python3
import requests
import datetime
import sys
import re
from bs4 import BeautifulSoup
s = requests.Session()

# Configurable:


now = datetime.datetime.now()
today = f"{now:%F}"

error_msg = ""
is_warning = False
is_critical = False
ez_user = "ezzabbix"
ez_pw = ".-.SAPP[I1lL]0O0oOER,!#,ZCSD//"

if len(sys.argv) < 2:
	print("ERROR - Missing hostname as parameter")
	sys.exit(1)
server = sys.argv[1]


loginurl = f"https://{server}/login"
logouturl = f"https://{server}/logout"
postdata = {"url": "^U", "user": ez_user, "pass": ez_pw}

auditurl = f"https://{server}/audit?date=today&event=UsageLimit"
usageurl = f"https://{server}/usagelimits"
# audit?date=today&event=UsageLimit

r_login = None
try:
    r_login = s.post(url=loginurl, data=postdata)
    if r_login.status_code != 200:
        is_critical = True
        error_msg = f"Fikk ikke status code 200 ved login {loginurl}"
except Exception as e:
    is_critical = True
    error_msg += f"Fikk ikke hentet {loginurl}. "

usagepage = ""

# print(r_login.status_code)
auditpage = ""

if r_login:
    try:
        r_usage = s.get(url=usageurl)
        if r_usage.status_code != 200:
            is_critical = True
            error_msg += f"Fikk ikke status code 200 ved audit url {usageurl}"
        else:
            usagepage = r_audit.text
    except Exception as e:
        is_critical = True
        error_msg += f"Fikk ikke hentet {usageurl}. "



if r_login:
    try:
        r_audit = s.get(url=auditurl)
        if r_audit.status_code != 200:
            is_critical = True
            error_msg += f"Fikk ikke status code 200 ved audit url {auditurl}"
        else:
            auditpage = r_audit.text
    except Exception as e:
        is_critical = True
        error_msg += f"Fikk ikke hentet {auditurl}. "


usage = []
users = {}

dag = "N/A"
soup = BeautifulSoup(auditpage, 'html.parser')
# ul = usage limit
ul_type_ptn = r'<h2>Usage limit ([a-zA-Z0-9-]+)</h2>'

for ul_typepart in soup.find_all(['table', 'h2']):
    ul_typematch = re.findall(ul_type_ptn, str(ul_partpart))
    if ul_typematch:
        ultype = typematch[0]
    else:
        # usage_ptn = r'<td>Login\.(?P<event>[^\<]+)<\/td><td>(?P<ip>[^\<]+)</td><td>(?P<username>[^\<]+)</td><td>(?P<session>[^\<]+)</td><td>(?P<user>[^\<]+)<'
        usage_ptn = r"<tr[^>]*><td>&nbsp;</td><td><label for='clear-1'>aaser@oslomet.no</label></td><td>17</td><td>1.35</td><td>&nbsp;</td><td>&nbsp</td></tr>"
        usage_ptn = r"<tr[^>]*><td>[^<]*</td><td><label[^>]*>([^<]*)</label></td><td>([^<]*)</td><td>([^<]*)</td><td>([^<]*)</td><td>([^<]*)</td></tr>"
	# re.findall(usage_ptn, str(ul_typepart))
	# [('aaser@oslomet.no', '17', '1.35', '&nbsp;', '&nbsp')]
	# <tr><th scope='col'>Clear</th><th scope='col'>User</th><th scope='col' class='sortnumeric'>Transfers</th><th scope='col' class='sortnumeric'>Megabytes</th><th>Suspended</th><th scope='col'>Expires</th></tr>

# WARNING - Could not count loggedin users in auditpage 2022-11-09. Could not count loggedin users in auditpage 2022-11-07. Could not count loggedin users in auditpage 2022-11-04. Could not count loggedin users in auditpage 2022-11-01.  LAST 10 DAYS: user login sum_success=5 sum_failed=4 sum_alfafail=2 sum_intruders=0 sum_nonsuccess=1 sum_monitoring=4 TODAY: today_success=3 today_failed=4 today_alfafail=0 today_intruders[root@zabbix.oslomet.no externalscripts]# vim ezproxy-audit-access.py 


        usermatch = re.findall(usage_ptn, str(ul_typepart))
        if not usermatch:
            error_msg += f'Could not count usage'
            is_warning = True

        if usermatch:
            usage.append({ "count": len(usermatch), 'name': ultype })
            for line in usermatch:
                user = line[0]
                u_transfer = line[1]
                u_mb = line[2]
                u_suspended = line[3]
                u_expires = line[4]
		if user:
                    info += f"username found to be {username}. "
                    username = username.split("@")[0]
                    alfakroll = True
                    alfafail += 1
                    if dag == today:
                        today_alfafail += 1
                # shibboleth gir @ i username
                if "Shibboleth" in other:
                    shibboleth += 1
                    if dag == today:
                        today_shibboleth += 1

                if "@" in username:
                    username = username.split("@")[0]

                # print(a)
                user = users.get(username, None)
                if user is None:
                    user = {"success": 0, "failed": 0, "intruder": 0, "monitoring": 0, "ips": [], "info": "", "dager": []}
                if "Success" in event:
                    if ez_user in username:
                        monitoringlogin += 1
                        user.update({"monitoring": user.get("monitoring", 0) + 1})
                        if dag == today:
                            today_monitoringlogin += 1
                    else:
                        loginsuccess += 1
                        user.update({"success": user.get("success", 0) + 1})
                        if dag == today:
                            today_loginsuccess += 1
                if "Failure" in event and not alfakroll:
                    loginfailed += 1
                    user.update({"failed": user.get("failed", 0) + 1})
                    if dag == today:
                        today_loginfailed += 1  # remove all after @ so no domain is shown.

                if "Intruder" in event:
                    intruders += 1
                    user.update({"intruder": user.get("intruder", 0) + 1})
                    if dag == today:
                        today_intruders += 1

                if ip:
                    if user.get("ips", None) is None:
                        user.update({"ips": []})
                    user.update({"ips": user.get("ips", []).append(ip)})
                if info:
                    user.update({"info": user.get("info") + info})
                dager = user.get("dager", None)
                if dager is None:
                    dager = []
                if dag not in dager:
                    dager.append(dag)
                user.update({"dager": dager})

                # if user.get("dager", None) is None:
                #     user.update({"dager": []})
                # user.update({"dager": user.get("dager", []).append(dag)})
                other = other.replace("\xa0", "")
                if other and other != "&nbsp;" and other != "":
                    user.update({"info": user.get("info") + f"{dag}: {other}. "})
                    # print(f"DAG: {dag}")
                    # print(user)
                    # print(other)

                users.update({username: user})

                # if next((sub for sub in users if sub['user'] == username), None) is None:
    #         user =

suspects = ""
# print(f"TODAY: {now:%F}")


for usr in users:
    usern = users.get(usr)
    succ = usern.get("success", 0)
    fail = usern.get("failed", 0)
    dager = usern.get("dager", "-")
    info = usern.get("info", "-")
    if succ == 0 and fail > 1:
        nonsuccess += 1
    if succ == 0 and fail > limit_nonsuccessfull_logins:
        dager = usern.get("dager")
        if f"{now:%F}" in dager:
            # suspects += usr + f":s{s}/f{f} ({info}). "
            suspects += usr + f":s{succ}/f{fail}.  "
            error_msg = suspects
            is_warning = True
            failedusers.append(usr)
            # print({usr: u})


# print(users)
# loggedin_user_ptn = r'\/status\?sessionuser\=([^\']+)'
# loggedin_user_matches = re.findall(loggedin_user_ptn, statuspage)
#     #print(loggedin_user_matches,len(loggedin_user_matches))
#
# if loggedin_user_matches:
#     try:
#         ant_loggedin_user = len(loggedin_user_matches)
#     except Exception as e:
#         print(f"exceiption {e}")
#         is_critical = True
#         error_msg += "error fetching ant_loggedin_users"


severity = "OK"
if is_warning:
    severity = "WARNING"
if is_critical:
    severity = "CRITICAL"

# if is_warning or is_critical:

# else:
performanceData = f"LAST {last_x_days} DAYS: user login sum_success={loginsuccess} sum_failed={loginfailed}" + \
    f" sum_alfafail={alfafail} sum_intruders={intruders} sum_nonsuccess={nonsuccess} sum_monitoring={monitoringlogin} sum_shibboleth={shibboleth} TODAY: today_success={today_loginsuccess}" + \
    f" today_failed={today_loginfailed} today_alfafail={today_alfafail} today_intruders={today_intruders} today_monitoring={today_monitoringlogin} today_shibboleth={today_shibboleth}" + \
    f"  failedusers={','.join(failedusers)}"
print(f"{severity} - {error_msg} {performanceData}")

try:
    r_logout = s.get(url=logouturl)
except Exception as e:
   print(f"Exception logging out {e}")
