#!/bin/env python3
import requests, json
import datetime
import sys
import re
from bs4 import BeautifulSoup
s = requests.Session()

mon = 0
error_msg = ""
is_warning = False
is_critical = False
# server = "ezproxy.oslomet.no"
#server = "ezproxy-dev.oslomet.no"ez
if len(sys.argv) < 2:
    print("ERROR - Missing hostname as parameter")
    sys.exit(1)

server = sys.argv[1]

ez_user = "ezzabbix"
ez_pw = ".-.SAPP[I1lL]0O0oOER,!#,ZCSD//"

loginurl = f"https://{server}/login"
postdata = {"url": "^U", "user": ez_user, "pass": ez_pw}
logouturl = f"https://{server}/logout"

statusurl = f"https://{server}/status"
intrusionurl = f"https://{server}/intrusion"

r_intrusion = None
intrusionpage = None


def hentside(url, postdata=None, expect_status_code = 200):
    global error_msg, is_critical, is_warning
    r = None
    rtext = ""
    try:
        if postdata is not None:
            r = s.post(url=url, data=postdata)
        else:
            r = s.get(url=url)
        if r:
            if r.status_code == expect_status_code:
                rtext = r.text
            if r.status_code != expect_status_code:
                is_critical = True
                error_msg += f"got status_code {r.status_code} ved {url}. "
        else:
            is_critical = True
            error_msg += f"{url} feilet. "
    except Exception as e:
        is_critical = True
        error_msg += f"Fikk ikke hentet {loginurl} (exception). "
    return r, rtext


(r_login, statuspage) = hentside(url=loginurl, postdata=postdata)

if r_login:
    (r_status, statuspage) = hentside(url=statusurl)
    (r_intrusion, intrusionpage) = hentside(url=intrusionurl)


(sess_active, sess_limit, vh_peak, vh_limit, xfer_active, xfer_limit) = (0, 0, 0, 0, 0, 0)
(ant_loggedin_user, ant_loggedin_sessions, ant_loggedin_ansatt, ant_loggedin_student, ant_loggedin_monitoring, ant_loggedin_monitoring2, ekstra_sessions) = (0, 0, 0, 0, 0, 0, 0)
ekstra_sessions_list = []

intr_ptn = "There are no intrusion attempts at this time."


if intrusionpage:
    intr_matches = re.findall(intr_ptn, intrusionpage)

    if not intr_matches:
        error_msg += f"Intrusion Attempts"

        intr2_ptn = "checkbox"
        intr2_matches = re.findall(intr2_ptn, intrusionpage)
        error_msg += f" {len(intr2_matches)} "

        intr3_ptn = "<td>\&nbsp;</td>"
        intr3_matches = re.findall(intr3_ptn, intrusionpage)
        error_msg += f" {len(intr3_matches)} "

        try:
            int(len(intr2_matches))
            # int()
            # int()
            # minus hverandre ser antall
        except Exception as e:
            print(f"exception {e}")

vh_ptn = r'Peak virtual hosts/limit: (\d+)/(\d+)'

if statuspage:
    vh_matches = re.findall(vh_ptn, statuspage)
    if not vh_matches:
        # print('Could not identify virtual hosts in status page')
        is_warning = True
        error_msg += "Could not identify virtual hosts in status page."

    if vh_matches:
        try:
            vh_peak = int(vh_matches[0][0])
            vh_limit = int(vh_matches[0][1])
        except Exception as e:
            print(f"Got exception {e}")
        # print(f"{vh_limit} , {vh_peak}")

        if vh_peak == vh_limit:
            error_msg += 'Virtual hosts exceeded limit'
            is_critical = True
        if (vh_peak + 30) > vh_limit:
            error_msg += 'Virtual hosts approaching limit'
            is_warning = True

    # print(error_msg)
    # print(statuspage)
    sess_ptn = r'Peak sessions active/limit: (\d+)/(\d+)'
    sess_matches = re.findall(sess_ptn, statuspage)

    if not sess_matches:
        print('Could not identify sessions in status page')
        error_msg += "Could not identify sessions in status page. "
        is_critical = True

    if sess_matches:
        try:
            sess_active = int(sess_matches[0][0])
            sess_limit = int(sess_matches[0][1])
        except Exception as e:
            print(f"exception {e}")
            is_warning = True
            error_msg += "exception reading sessions active/limit"

        if sess_active == sess_limit:
            error_msg += 'Active sessions reached limit '
            is_warning = True

        if (sess_active + 5) > sess_limit:
            error_msg += 'Active sessions near limit'
            is_warning = True

    xfer_ptn = r'concurrent transfers active/limit: (\d+)/(\d+)'
    xfer_match = re.findall(xfer_ptn, statuspage)

    if not xfer_match:
        error_msg += 'Could not identify concurrent transfers in status page. '
        is_warning = True
        print(xfer_match, len(xfer_match))

    if xfer_match:
        try:
            xfer_active = int(xfer_match[0][0])
            xfer_limit = int(xfer_match[0][1])
        except Exception as e:
            print(f"exceiption {e}")
            is_critical = True
            error_msg += "error fetching xfer_active and xfer_limit"

        if (xfer_active + 8) > xfer_limit:
            error_msg += 'Concurrent transfers at or near limit. '
            is_warning = True

    loggedin_session_ptn = r'\/status\?session\=([^\']+)'
    loggedin_session_matches = re.findall(loggedin_session_ptn, statuspage)
    if not loggedin_session_matches:
        error_msg += 'Could not count loggedin sessions in status page. '
        is_warning = True

    if loggedin_session_matches:
        try:
            ant_loggedin_sessions = len(loggedin_session_matches)
        except Exception as e:
            print(f"exceiption {e}")
            is_critical = True
            error_msg += "error fetching ant_loggedin_sessions"

    def nameToMatch(pattern, string):
        result = dict()
        for subpattern in re.split('\|(?=\(\?P<)', pattern):
            match = re.search(subpattern, string)
            if match:
                result.update(match.groupdict())
        return result


    ekstra_sessions_list = []
    ekstra_sessions = 0
    # <td><a href='/status?sessionuser=s364219'>s364219</a></td><td align='right'>1</td><td align='right'>5</td></tr>
    loggedin_ansatt_ptn = r'\/status\?sessionuser\=([a-zA-Z][a-zA-Z]+)'
    loggedin_ansatt_matches = re.findall(loggedin_ansatt_ptn, statuspage)
    loggedin_student_ptn = r'\/status\?sessionuser\=([sS][0-9][0-9]+)'
    loggedin_student_matches = re.findall(loggedin_student_ptn, statuspage)
    loggedin_monitoring_ptn = r'\/status\?sessionuser\='+ez_user
    loggedin_monitoring_matches = re.findall(loggedin_monitoring_ptn, statuspage)
    sessionusers_ptn_old = r"\/status\?sessionuser\=([^\']+)'>([^\<]+)</a></td><[^\>]+>([0-9]+)</td>"
    sessionusers_ptn = "\/status\?sessionuser\=(?P<user>[^\']+)'>(?P<username>[^\<]+)</a></td><[^\>]+>(?P<sessions>[0-9]+)</td>"
    #  (?P<resource>\w+)/ (?P<id>\d+)'
    # sessionusers_r = nameToMatch(sessionusers_ptn, statuspage)
    sessionusers_r = re.compile(sessionusers_ptn)
    sessionusers_matches = re.finditer(sessionusers_r, statuspage)
    loggedin_user_ptn = r'\/status\?sessionuser\=([^\']+)'
    loggedin_user_matches = re.findall(loggedin_user_ptn, statuspage)
    if not loggedin_ansatt_matches:
        error_msg += 'Could not count loggedin ansatte in status page. '
        is_warning = True
    if not loggedin_student_matches:
        error_msg += 'Could not count loggedin students in status page. '
        is_warning = True
    if not loggedin_monitoring_matches:
        error_msg += 'Could not count loggedin monitoring in status page. '
        is_warning = True
    if not sessionusers_matches:
        error_msg += 'Could not count loggedin monitoring in status page. '
        is_warning = True
    if not loggedin_user_matches:
        error_msg += 'Could not count loggedin users in status page. '
        is_warning = True
        # print(loggedin_user_matches,len(loggedin_user_matches))

    if loggedin_student_matches:
        try:
            ant_loggedin_student = len(loggedin_student_matches)
        except Exception as e:
            # print(f"exceiption {e}")
            # is_critical = True
            error_msg += "error fetching ant_loggedin_student"

    if loggedin_monitoring_matches:
        try:
            ant_loggedin_monitoring = len(loggedin_monitoring_matches)
        except Exception as e:
            # print(f"exceiption {e}")
            # is_critical = True
            error_msg += "error fetching ant_loggedin_monitoring"

    if sessionusers_matches:
        # mon = []
        # for match in matches:

        for user in sessionusers_matches:
            try:
                u = user.groupdict()
                if u.get("user") == ez_user:
                    mon += int(u.get("sessions", 0))
                elif int(u.get("sessions", 0)) > 1:
                    ekstra_sessions += int(u.get("sessions", 0)) -1
                    ekstra_sessions_list.append(u)
                    # print(u)
            except Exception as e:
                print(f"exception {e}")
            # print(user)
            # print(user.groupdict())


        try:
            ant_loggedin_monitoring2 = mon
        except Exception as e:
            # print(f"exceiption {e}")
            # is_critical = True
            error_msg += "error fetching ant_loggedin_monitoring2"

    if loggedin_ansatt_matches:
        try:
            ant_loggedin_ansatt = len(loggedin_ansatt_matches)-ant_loggedin_monitoring
        except Exception as e:
            # print(f"exceiption {e}")
            # is_critical = True
            error_msg += "error fetching ant_loggedin_ansatt"

    if loggedin_user_matches:
        try:
            ant_loggedin_user = len(loggedin_user_matches)
        except Exception as e:
            print(f"exceiption {e}")
            is_critical = True
            error_msg += "error fetching ant_loggedin_user"

severity = "OK"
performanceData = "Got exception og performance data"
if is_warning:
    severity = "WARNING"
if is_critical:
    severity = "CRITICAL"

ekstra_sessions_users = ""

# if True:
try:
    if ekstra_sessions_list:
        ekstra_sessions_users = " ".join([str(u.get('user'))+"/"+str(u.get('sessions')) for u in ekstra_sessions_list])
    performanceData = f"VirHosts={vh_peak} VirHosts_Limit={vh_limit} Sessions={sess_active} " +\
                      f"Sessions_Limit={sess_limit} ConcurrentXfers={xfer_active} ConcurrentXfers_Limit={xfer_limit}" +\
                      f" users={ant_loggedin_user} sessions={ant_loggedin_sessions} ansatt={ant_loggedin_ansatt}" +\
                      f" student={ant_loggedin_student} monitoring={ant_loggedin_monitoring2} extrasessions={ekstra_sessions}  extrasession_users=[{ekstra_sessions_users}]"

except Exception as e:
    print(f" Got EXCEPTION {e} ", end="")

if is_warning or is_critical:
    print(f"{severity} - {error_msg} - {performanceData}")
else:
    print(f"{severity} - {performanceData}")


hentside(logouturl)

# VirHosts=1727/4000 Sessions=173/500 ConcurrentXfers=22/200


# r_login = None
# try:
#     r_login = s.post(url=loginurl, data=postdata)
#     if r_login.status_code != 200:
#         is_critical = True
#         error_msg += f"Fikk ikke status code 200 ved login {loginurl}. "
# except Exception as e:
#     is_critical = True
#     error_msg += f"Fikk ikke hentet {loginurl}. "
# print(f"{r.status_code}: {API}")
# print(r.text)
# statuspage = ""

# print(r_login.status_code)


# r_status = None
# statuspage = ""
# try:
#     r_status = s.get(url=statusurl)
#     if r_status.status_code != 200:
#         is_critical = True
#         error_msg += f"Fikk ikke status code 200 ved status url {statusurl}. "
#     else:
#         statuspage = r_status.text
# except Exception as e:
#     is_critical = True
#     error_msg += f"Fikk ikke hentet {statusurl}. "

# r_intrusion = None
# intrusionpage = ""
# try:
#     r_intrusion = s.get(url=intrusionurl)
#     if r_intrusion.status_code != 200:
#         is_critical = True
#         error_msg += f"Fikk ikke status code 200 ved status url {intrusionurl}. "
#     else:
#         intrusionpage = r_intrusion.text
#
# except Exception as e:
#     is_critical = True
#     error_msg += f"Fikk ikke hentet {intrusionurl}. "

# print(f"{r.status_code}: {statusurl}")
