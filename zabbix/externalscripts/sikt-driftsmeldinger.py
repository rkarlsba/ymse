#!/usr/bin/env python
import re
import requests
from bs4 import BeautifulSoup
import sys
ok = True


ignoretittel = ["FileSender - Problemer med treghet", "Forskningsdata: Informasjon om brukeropplevde forstyrrelser"]
ignorestart = ["08.04.2022 08:00", "25.05.2022 00:00"]

if __name__ == '__main__':
 # for server in ['sikt.no','feide.no']:
 for server in ['sikt.no']:
    url = 'https://sikt.no/driftsmeldinger'
    if server == 'feide.no':
      url = 'https://feide.no/driftsmeldinger'
    html_text = requests.get(url).text
    soup = BeautifulSoup(html_text, 'html.parser')
	
    attr = {
        'class': re.compile(r'message__[a-zA-Z]*|views-row|view-content|field'),
    }
    title,pubdate,desc,start,end,solved = "","","",0,0,0 
    for tag in soup.find_all(['div','h2','span'], attrs=attr):
#	print("##",str(start),"##")
        if ('views-row' in tag["class"]):
            if (not solved or not end) and (title and start) and not (title in ignoretittel and start in ignorestart):
                print(u" ".join(("ERROR %s/driftsmeldinger unsolved" % server, title, str(start))).encode('utf-8').strip())
                ok = False
            title,pubdate,desc,start,end,solved = "","","",0,0,0 
        if ('service-message__description' in tag["class"] or 'field--type-text-with-summary' in tag["class"]):
            desc = tag.text.strip(' \n\t')
#	print(tag["class"])
        if ('service-message__title' in tag["class"] or 'field--name-title' in tag["class"]):
            title = tag.text.strip(' \n\t')
        if ('service-message__start' in tag["class"] or 'field--name-field-incident-start' in tag["class"]):
            start = tag.text.strip(' \n\t\r').replace("\n","")
            start = start.replace("Start", "")
        if ('service-message__end' in tag["class"] or 'field--name-field-incident-end' in tag["class"]):
            end = tag.text.strip(' \n\t')
        if ('service-message__solved' in tag["class"] or 'field--name-field-incident-status' in tag["class"]):
	    solved = tag.text.strip()
    if (not solved or not end) and (title and start ) and not (title in ignoretittel and start in ignorestart):
        print(u" ".join(("ERROR %s/driftsmeldinger unsolved:" % server, title, str(start))).encode('utf-8').strip())
        ok = False

if ok:
    print("OK - ingen nye driftsmeldinger")
    sys.exit(0)
else:
    sys.exit(2)
