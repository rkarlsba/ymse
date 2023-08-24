#!/bin/env python3
import warnings
warnings.filterwarnings(action='ignore',message='Python 3.6 is no longer supported')
# /usr/local/lib/python3.6/site-packages/jwt/utils.py:7: CryptographyDeprecationWarning: Python 3.6 is no longer supported by the Python core team. Therefore, support for it is deprecated in cryptography. The next release of cryptography (40.0) will be the last to support Python 3.6.
#  from cryptography.hazmat.primitives.asymmetric.ec import EllipticCurve

import adal # pip3 install adal
import json
import requests
import pandas as pd  # pip3 install pandas
from pandas import json_normalize
import sys, os


# tid
import time
import dateutil.parser as dparser
import datetime

# Application.Read.All
# Directory.Read.All

tenant = 'fec81f12-6286-4550-8911-f446fcdafa1f'
resource_URL ='https://graph.microsoft.com'
authority_url = 'https://login.microsoftonline.com/%s' %(tenant)

warndays = 20  # default 20 dager
if len(sys.argv)>1:
    warndays = int(sys.argv[1])

appdates = []
warnlist = []
critlist = []
expirelist = []
lange_timespans = []

context = adal.AuthenticationContext(authority_url)

def session_token(resource_URL, app_id, app_secret):
    return context.acquire_token_with_client_credentials(resource_URL, app_id, app_secret)

def session_token_login():
    app_id = 'ad3b6e2e-2048-4e67-8585-46aad42b1b61'
    if os.path.isfile('.graph'):
        app_secret = open(".graph").read()
    else:
        app_secret = open("/var/lib/zabbix/.graph").read()
    return session_token(resource_URL, app_id, app_secret)

def get_access_token(token):
    if token is not None:
        return token['accessToken']
    token = session_token(resource_URL, app_id, app_secret)

def get_request_headers(token):
    return {'Authorization': 'bearer %s'%(token['accessToken'])}

def make_request(url, token):
    return requests.get(url, headers=get_request_headers(token))

def timespan(a, b):
    if a and b:
        at = get_unixtime(a)
        bt = get_unixtime(b)
        return calc_sec_diff(at,bt)

def remainingseconds(s):
    if s:
        a = int(time.time())
        b = get_unixtime(s)
        return calc_sec_diff(a,b)

def sec2days(s):
    if s:
        return s/(60*60*24)

def remainingdays(s):
    if s:
        return remainingseconds(s)/(60*60*24)

def get_unixtime(s):
    if s:
        dt = dparser.parse(s, fuzzy=True)
        return int(time.mktime(dt.timetuple()))

def calc_sec_diff(a,b):
    if a and b:
        return b - a

def getdate(s):
    return get_unixtime(s)

def fetch_applications():
    ret = []
    token = session_token_login()
    request_headers = get_request_headers(token)
    url = resource_URL + '/v1.0/applications'
    while True:
        if not url:
            break
        response = make_request(url, token)
        if response.status_code == 200:
            json_data = json.loads(response.text)
            url = None
            if json_data.get("@odata.nextLink", None) is not None: 
                url = json_data["@odata.nextLink"] # Fetch next link
            yield json_data['value']
            #ret.append(json_data['value'])
    #return ret

        

antall_apps = 0
antall_credentials = 0
antall_allecredentials = 0
appliste = [row for row in fetch_applications()]

#with open('appliste3.json', 'w') as f:
#	    json.dump(appliste, f)

# appliste = json.loads(open("applist2.txt").read())
# appliste = json.loads(open("appliste3.json").read())
for page in appliste:
    for app in page:
        antall_apps += 1
        appId = app.get("appId")
        displayName = app.get("displayName")
        createdDateTime = app.get("createdDateTime")
        deletedDateTime = app.get("deletedDateTime")
        for field_name in ["keyCredentials", "passwordCredentials"]:
            field = app.get(field_name)
            for val in field:
                antall_allecredentials += 1
                endDateTime = val.get("endDateTime")
                startDateTime = val.get("startDateTime")
                daysleft = remainingdays(endDateTime)
                appinfo = {"timestamp": getdate(endDateTime), "displayName": displayName, "appId": appId, "type": field_name, "endDateTime": endDateTime, "startDateTime": startDateTime, "createdDateTime": createdDateTime, "deletedDateTime": deletedDateTime, "daysleft": daysleft, "timespan": sec2days(timespan(startDateTime,endDateTime)) }
                antall_credentials += 1
                appdates.append(appinfo)
                if daysleft <= warndays and daysleft > -10:
                    warnlist.append(appinfo)
                if daysleft < 2:
                    critlist.append(appinfo)
                if daysleft > 0:
                    expirelist.append(appinfo)
                if sec2days(timespan(startDateTime,endDateTime)) > 740:
                    lange_timespans.append(appinfo)
            

nextexpire=min([int(remainingdays(d.get("endDateTime"))) for d in expirelist])
status = f"apps={antall_apps} allecredentials={antall_allecredentials} nextexpire={nextexpire}"
warnlist = sorted( warnlist, key=lambda k: (k['timestamp'], k['displayName']) )

flere = ""
info = "WARNING"
if len(warnlist) > 1:
    flere = "s"
if len(critlist) > 0:
    liste = [{d.get("displayName"): d.get("endDateTime")} for d in critlist]
    # print(f"CRITICAL: {len(critlist)} expires/d: {liste}")
    info = f"CRITICAL: {len(critlist)} expires/d: {liste}"
if len(warnlist) > 0 or len(critlist) > 0:
    liste = [{d.get("displayName"): d.get("endDateTime")} for d in warnlist]
    print(f"{info} - {nextexpire} days left - {len(warnlist)} azure application{flere} access will expire within {warndays} days, first in {nextexpire} days: {liste} status {status}")
if len(warnlist) == 0:
    print(f"OK: no azure applications expires in {warndays} days. Next will expire in {nextexpire} days. status {status}")

