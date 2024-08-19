#!/usr/bin/env python3

import ldap
import logging
import re
import sys
import traceback

# Globals
password_file = ".ldap_passwd"
debug = 1
username = ''
password = ''
uri = 'ldaps://'
port = 636
servers = (
        'openldap-prod01.oslomet.no',
        'openldap-prod02.oslomet.no',
        'openldap-stage01.oslomet.no',
        'openldap-stage02.oslomet.no',
        'ldap-c-dev.oslomet.no',
        'ldap-d-dev.oslomet.no',
        'ldap-e.oslomet.no'
        'ldap-f.oslomet.no'
#       'openldap.oslomet.no',
)
base_dn = 'ou=tilsatt,ou=oslomet,dc=oslomet,dc=no'

# Functions
def getpwtok():
    try:
        f=open(password_file,"r")
        lines=f.readlines()
        if (len(lines) > 1):
            print("More than one line in {password_file}, only one considered", file=sys.stderr)
        for line in lines:
            line = re.sub(r"[\r\n]", "", line)
            auth_token = line.split(':')
            break
        f.close()
        return auth_token

    except Exception as e:
        logging.error(traceback.format_exc())

def authenticate():
    ldap.set_option(ldap.OPT_X_TLS_REQUIRE_CERT, ldap.OPT_X_TLS_NEVER)
    username,password = getpwtok()
    user_dn = f"uid={username},{base_dn}"
    successes = 0
    attempts = len(servers)
    for server_name in servers:
        try:
            server_uri = f"{uri}{server_name}:{port}"
            print(server_uri)
            l = ldap.initialize(server_uri)
            l.protocol_version = ldap.VERSION3
            l.simple_bind_s(user_dn, password)
            print(f"Connected to {server_uri}");
            successes += 1
            l.unbind()

        except ldap.INVALID_CREDENTIALS:
            print("Invalid credentials")
            pass

        except ldap.SERVER_DOWN:
            print("Can't connect to server - someone plugged the plug?")
            pass

        except Exception as e:
            logging.error(traceback.format_exc())
            return False

    if (attempts == 1 and successes == 1):
        print("[INFO] \"All\" attempts succeeded, but, well, as there were only a single attempt, this must producee a notice - we want reundandancy!)")
    elif (attempts > 1):
        if (successes == attempts):
            print(f"[OK] Successes so far: {successes}, with attemtpts at {attempts}, making it a jolly nice day")
        elif (successes > 0 and successes < attempts ):
            print(f"[WARN] Successes so far: {successes}, but attempts at {attempts}, meaning we're missing {attempts-successes}")
        elif (successes == 0):
            print(f"[AVERAGE] No answers from LDAP service, which is not a good thing. Better check it ASAP!")
        else:
            print(f"[INFO] Headache v1.1: successes == {successes} and attempts == {attempts} and I don't understand - fuck!")
    else:
        print(f"[INFO] Headache v1.2: successes == {successes} and attempts == {attempts} and I don't understand - damn!")

if __name__ == "__main__":
    authenticate()
