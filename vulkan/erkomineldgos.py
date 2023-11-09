#!/usr/bin/env python3

import re
import requests
from bs4 import BeautifulSoup
import sys

url = 'https://www.erkomideldgos.is/'
user_agent = { 'User-agent': 'Mozilla/5.0' }
debug = { 'verbose': sys.stderr }

html_text = requests.get(url, headers = user_agent).text
#html_text = requests.get(url).text
soup = BeautifulSoup(html_text, 'html.parser')
#ok = True
#alt = {}

print(soup.prettify())
