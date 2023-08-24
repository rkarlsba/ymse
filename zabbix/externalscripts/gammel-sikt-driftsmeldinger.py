#!/usr/bin/env python
import re
import requests
from bs4 import BeautifulSoup
import sys
url = 'https://sikt.no/driftsmeldinger'
html_text = requests.get(url).text
soup = BeautifulSoup(html_text, 'html.parser')
ok = True

if __name__ == '__main__':
    attr = {
        'class': re.compile(r'message__[a-zA-Z]*|views-row'),
    }
    title,pubdate,desc,start,end,solved = "","","",0,0,0 
    for tag in soup.find_all(['div','h2'], attrs=attr):
        if ('views-row' in tag["class"]):
            if (not solved or not end) and (title and start ) and not (title in "FileSender - Problemer med treghet" and "08.04.2022 08:00" in start):
	        print(u' '.join(("ERROR sikt.no/driftsmeldinger unsolved:", title, start)).encode('utf-8').strip())
                ok = False
                #print(" ".join(("ERROR sikt.no/driftsmeldinger unsolved", title, start)))
            title,pubdate,desc,start,end,solved = "","","",0,0,0 
        if ('service-message__description' in tag["class"]):
            desc = tag.text
        if ('service-message__title' in tag["class"]):
            title = tag.text
        if ('service-message__start' in tag["class"]):
            start = tag.text
            start = start.replace("Start", "")
        if ('service-message__end' in tag["class"]):
            end = tag.text
        if ('service-message__solved' in tag["class"]):
            solved = tag.text
   # if (not solved or not end) and (title and start):
    if (not solved or not end) and (title and start ) and not (title in "FileSender - Problemer med treghet" and "Start 08.04.2022 08:00" in start):
	print(u' '.join(("ERROR sikt.no/driftsmeldinger unsolved:", title, start)).encode('utf-8').strip())
        ok = False
#        print(" ".join(("ERROR sikt.no/driftsmeldinger unsolved:", title, start)))

if ok:
    print("OK - ingen nye driftsmeldinger")
    sys.exit(0)
else:
    sys.exit(2)
