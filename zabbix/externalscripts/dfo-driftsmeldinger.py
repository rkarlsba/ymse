#!/usr/bin/env python3
import re
import requests
from bs4 import BeautifulSoup
import sys
url = 'https://status.dfo.no/'
html_text = requests.get(url).text
soup = BeautifulSoup(html_text, 'html.parser')
ok = True
alt = {}

# sys.exit(0)

# <div class="unresolved-incidents">
#   <div class="unresolved-incident impact-major">
#<div class="incident-title font-large">
#<a class="whitespace-pre-wrap actual-title" href="/incidents/55xx8l3q9klp">Feil ved selvbetjeningsportal og app grunnet problemer hos driftsleverandor</a>					                  
#</div>
#<div class="updates font-regular">
#<div class="update">
#<strong>Update</strong> - <span class="whitespace-pre-wrap">We are continuing to investigate this issue.</span>
#<br>
#<small><span class="ago" data-datetime-unix="1663102886000"></span>Sep <var data-var='date'>13</var>, <var data-var='year'>2022</var> - <var data-var='time'>23:01</var> CEST</small>
#</div>
#<div class="update">
#<strong>Investigating</strong> - <span class="whitespace-pre-wrap">Det jobbes med a lose problemet og fa tjenesten operativ.</span>
#<br>
#<small><span class="ago" data-datetime-unix="1663102794000"></span>Sep <var data-var='date'>13</var>, <var data-var='year'>2022</var> - <var data-var='time'>22:59</var> CEST</small>
#</div>
#</div>
#</div>


if __name__ == '__main__':
    attr = {
        'class': re.compile(r'|name|component-status|ago|unresolved-incident|incident-title'),
    }
    name,status,incident = "","",[]
    for a in soup.find_all(['a'], attrs=attr):
        if ('actual-title' in a["class"]):
              incident.append(a.text.strip(' \n\t\r').replace("\n",""))

    for tag in soup.find_all(['span','div'], attrs=attr):
        if ('name' in tag["class"]):
            status = ""
            name = tag.text.strip(' \n\t\r').replace("\n","")
        if ('component-status' in tag["class"]):
            status = tag.text.strip(' \n\t\r').replace("\n","")
            if status not in alt:
                alt[status] = []
            alt[status].append(name)
            if status != "Operational":
                ok = False

if not ok:
    print("CRITICAL status.dfo.no: ", end = "")
    #print(*incident, sep = ", ", end="")
    # print(u' '.join(*incident).encode('utf-8').strip(), sep = ", ", end = "")
    # print(' '.join(incident).encode('utf-8').strip(), sep = ", ", end = "")
    print(" ".join(incident).strip(), sep = ", ", end = "")


for status in alt:
    if status != "Operational":
        print(f" {status.upper()}: ", end = "")
        #print(*alt[status], sep = ", ", end="")
        # print(u' '.join(*alt[status]).encode('utf-8').strip(), sep = ", ", end = "")
        # print(' '.join(alt[status]).encode('utf-8').strip(), sep = ", ", end = "")
        print(' '.join(alt[status]).strip(), sep = ", ", end = "")


if ok:
    print("OK - ingen nye driftsmeldinger")
    sys.exit(0)
else:
    print()
    sys.exit(2)
