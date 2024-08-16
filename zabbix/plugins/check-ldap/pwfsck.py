#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# -*- coding: utf-8 -*-

import ldap
import logging
import re
import sys
import traceback
from ldap.controls import SimplePagedResultsControl

# Globals
password_file = ".ldap_passwd"
debug = 1

# This one returns the first token in the format username:password
def getpwtok():
    try:
        f=open(password_file,"r")
        lines=f.readlines()
        for line in lines:
            line = re.sub(r"[\r\n]", "", line)
            auth_token = line.split(':')
            break
        f.close()
        return auth_token

    except Exception as e:
        logging.error(traceback.format_exc())

getpwtok()
