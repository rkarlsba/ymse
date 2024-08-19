#!/usr/bin/env python3

import ldap
import logging
import re
import sys
import traceback

# Globals
password_file = ".ldap_passwd"
debug = 1

# Functions
def authenticate(username, password):
    ldap.set_option(ldap.OPT_X_TLS_REQUIRE_CERT, ldap.OPT_X_TLS_NEVER)
    server = "ldaps://ldap.example.com:636"
    base_dn = "dc=example.com"
    user_dn = "uid={},{}".format(username, base_dn)
    try:
        l = ldap.initialize(server)
        l.protocol_version = ldap.VERSION3
        
        l.simple_bind_s(user_dn, password)

if __name__ == "__main__":
    # Do something fun
