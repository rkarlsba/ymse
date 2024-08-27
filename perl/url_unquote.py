#!/usr/bin/env python3

from urllib.parse import unquote

url = 'https%3A%2F%2Fgithub.com%2Fearlephilhower%2Farduino-pico%2Freleases%2Fdownload%2Fglobal%2Fpackage_rp2040_index.json&v=IZKpCz6LEdg'
print(f'URL was "{url}"')
unurl = unquote(url)
print(f'URL is  "{unurl}"')
