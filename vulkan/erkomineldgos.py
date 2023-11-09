#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:tw=80

# Docs {{{
#
# Check https://www.erkomideldgos.is/ to see if there's an active volcano on
# Iceland.
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
#
# Licensed under either GPL, BSD, Apache, CDDL *and* the latest Adobe license
# that will only let you purchase *and* use the latest beta for currently only
# 20% above # normal price.
#
# }}}
# Example output {{{
""" 

    🌋 Er komið eldgos?

<span class="name">íslenska</span>
<span class="short-label">is</span>
<span class="name">English</span>
<span class="short-label">en</span>
<span class="m-font-size-62 font-size-96" m-font-size-set="true" style="display: initial; color: rgb(255, 255, 255);">Er komið eldgos?</span>
<span class="font-size-52 m-font-size-36" style="display: initial; color: rgb(255, 255, 255);">NEI!</span>
<span aria-hidden="true" class="dmSocialFacebook dm-social-icons-facebook oneIcon socialHubIcon style5" data-hover-effect=""></span>
<span aria-hidden="true" class="dmSocialLinkedin icon-linkedin oneIcon socialHubIcon style5" data-hover-effect=""></span>
<span aria-hidden="true" class="dmSocialInstagram dm-social-icons-instagram oneIcon socialHubIcon style5" data-hover-effect=""></span>
<span aria-hidden="true" class="dmSocialFacebook dm-social-icons-facebook oneIcon socialHubIcon style5" data-hover-effect=""></span>
<span aria-hidden="true" class="dmSocialLinkedin icon-linkedin oneIcon socialHubIcon style5" data-hover-effect=""></span>
<span aria-hidden="true" class="dmSocialInstagram dm-social-icons-instagram oneIcon socialHubIcon style5" data-hover-effect=""></span>
<span aria-hidden="true" class="dmSocialFacebook dm-social-icons-facebook oneIcon socialHubIcon style5" data-hover-effect=""></span>
<span aria-hidden="true" class="dmSocialLinkedin icon-linkedin oneIcon socialHubIcon style5" data-hover-effect=""></span>
<span aria-hidden="true" class="dmSocialInstagram dm-social-icons-instagram oneIcon socialHubIcon style5" data-hover-effect=""></span>
<span></span>

[-] <span class="name">íslenska</span>
[-] <span class="short-label">is</span>
[-] <span class="name">English</span>
[-] <span class="short-label">en</span>
[-] <span class="m-font-size-62 font-size-96" m-font-size-set="true" style="display: initial; color: rgb(255, 255, 255);">Er komið eldgos?</span>
[-] <span class="font-size-52 m-font-size-36" style="display: initial; color: rgb(255, 255, 255);">NEI!</span>
[-] <span aria-hidden="true" class="dmSocialFacebook dm-social-icons-facebook oneIcon socialHubIcon style5" data-hover-effect=""></span>
[-] <span aria-hidden="true" class="dmSocialLinkedin icon-linkedin oneIcon socialHubIcon style5" data-hover-effect=""></span>
[-] <span aria-hidden="true" class="dmSocialInstagram dm-social-icons-instagram oneIcon socialHubIcon style5" data-hover-effect=""></span>
[-] <span aria-hidden="true" class="dmSocialFacebook dm-social-icons-facebook oneIcon socialHubIcon style5" data-hover-effect=""></span>
[-] <span aria-hidden="true" class="dmSocialLinkedin icon-linkedin oneIcon socialHubIcon style5" data-hover-effect=""></span>
[-] <span aria-hidden="true" class="dmSocialInstagram dm-social-icons-instagram oneIcon socialHubIcon style5" data-hover-effect=""></span>
[-] <span aria-hidden="true" class="dmSocialFacebook dm-social-icons-facebook oneIcon socialHubIcon style5" data-hover-effect=""></span>
[-] <span aria-hidden="true" class="dmSocialLinkedin icon-linkedin oneIcon socialHubIcon style5" data-hover-effect=""></span>
[-] <span aria-hidden="true" class="dmSocialInstagram dm-social-icons-instagram oneIcon socialHubIcon style5" data-hover-effect=""></span>
[-] <span></span>

"""
# }}}
# libs {{{

import re
import requests
from bs4 import BeautifulSoup
import sys
import json

# }}}
# Globals {{{

lang = 'is'
user_agent = { 'User-agent': 'Mozilla/5.0' }
debug = { 'verbose': sys.stderr }
url = {
    'is': 'https://www.erkomideldgos.is/',
    'en': 'https://www.erkomideldgos.is/en/'
}
needle = 'font-size-52'
found = 0

# }}}
# Main code {{{

if __name__ == '__main__':
    html_text = requests.get(url[lang], headers = user_agent).text
    soup = BeautifulSoup(html_text, 'html.parser')
    eldgos = None
    title = soup.title.string

    # span class="font-size-52 m-font-size-36
    try:
        for span in soup.find_all('span'):
            for attr in span.attrs['class']:
                if attr == needle:
                    eldgos = span.contents[0]
    except Exception:
        pass

    if eldgos is None:
        print(f"Can't parse webpage {url[lang]}")
        sys.exit(1)

    print(eldgos)
    sys.exit(0)

# }}}
