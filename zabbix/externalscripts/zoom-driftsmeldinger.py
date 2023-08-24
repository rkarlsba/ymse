#!/usr/bin/env python3
import re
import requests
from bs4 import BeautifulSoup
import sys
url = 'https://status.zoom.us/'
html_text = requests.get(url).text
soup = BeautifulSoup(html_text, 'html.parser')
ok = True
alt = {}


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
    print("CRITICAL status.zoom.us: ", end = "")
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
