#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
# See README.md for details

# Libraries {{{

import ldap
import logging
import os
import re
import sys
import traceback

# }}}
# Globals {{{

# User globals
password_file = "~/.ldap_passwd"
username = ''
password = ''
debug = False
node_attempts = 0
node_successes = 0
cluster_attempts = 0
cluster_successes = 0
cluster_delay = 1       # Second(s)
uri = 'ldaps://'
port = 636
ldap_nodes = (
        'openldap-prod01.oslomet.no',
        'openldap-prod02.oslomet.no'
)
ldap_cluster = 'ldap.oslomet.no'
base_dn = 'ou=tilsatt,ou=oslomet,dc=oslomet,dc=no'

# }}}
# Functions {{{

# getpwtok() {{{

def getpwtok():
    try:
        f=open(os.path.expanduser(password_file),"r")
        lines=f.readlines()
        if (len(lines) > 1):
            print("More than one line in {_password_file}, only one considered", file=sys.stderr)
        for line in lines:
            line = re.sub(r"[\r\n]", "", line)
            auth_token = line.split(':')
            break
        f.close()
        return auth_token

    except Exception as e:
        logging.error(traceback.format_exc())

# }}}
# probe_node(server) {{{

# Sjekke om node svarer
def probe_node(node):
    ldap.set_option(ldap.OPT_X_TLS_REQUIRE_CERT, ldap.OPT_X_TLS_NEVER)
    user_dn = f"uid={username},{base_dn}"
    try:
        server_uri = f"{uri}{node}:{port}"
        if (debug):
            print(server_uri)
        l = ldap.initialize(server_uri)
        l.protocol_version = ldap.VERSION3
        l.simple_bind_s(user_dn, password)
        if (debug):
            print(f"Connected to {server_uri}");
        l.unbind()
        if (debug):
            print(f"Disconnected from {server_uri}");
        return True

    except ldap.INVALID_CREDENTIALS:
        print("Invalid credentials")
        return False

    except ldap.SERVER_DOWN:
        print("Can't connect to server - someone plugged the plug?")
        return False

    except Exception as e:
        logging.error(traceback.format_exc())
        return False

# }}}
# probe_cluster(cluster) {{{

# Sjekke om node svarer
def probe_cluster(cluster, attempts = 5):
    ldap.set_option(ldap.OPT_X_TLS_REQUIRE_CERT, ldap.OPT_X_TLS_NEVER)
    user_dn = f"uid={username},{base_dn}"
    for i in range(1, attempts):
        try:
            cluster_attempts += 1
            server_uri = f"{uri}{ldap_cluster}:{port}"
            if (debug):
                print(server_uri)
            l = ldap.initialize(server_uri)
            l.protocol_version = ldap.VERSION3
            l.simple_bind_s(user_dn, password)
            if (debug):
                print(f"Connected to {server_uri}");
            l.unbind()
            if (debug):
                print(f"Disconnected from {server_uri}");
            cluster_successes += 1
            pass

        except ldap.INVALID_CREDENTIALS:
            print("Invalid credentials")
            pass

        except ldap.SERVER_DOWN:
            print("Can't connect to server - someone plugged the plug?")
            pass

        except Exception as e:
            logging.error(traceback.format_exc())
            pass

# }}}

# }}}
# Main {{{

if __name__ == "__main__":
    username,password = getpwtok()
    node_successes = 0
    node_count = len(ldap_nodes)
    for node in ldap_nodes:
        try:
            node_attempts += 1
            if probe_node(node):
                node_successes += 1
                pass
        except Exception as e:
            logging.error(traceback.format_exc())
            pass

    probe_cluster(ldap_cluster);

    if (node_attempts == 1 and node_successes == 1):
        print("[INFO] \"All\" node_attempts succeeded, but, well, as there were only a single attempt, this must producee a notice - we want reundandancy!)")
    elif (node_attempts > 1):
        if (node_successes == node_attempts):
            print(f"[OK] Successes so far: {node_successes}, with attemtpts at {node_attempts}, making it a jolly nice day")
        elif (node_successes > 0 and node_successes < node_attempts ):
            print(f"[WARN] Successes so far: {node_successes}, but node_attempts at {node_attempts}, meaning we're missing {node_attempts-node_successes}")
        elif (node_successes == 0):
            print(f"[AVERAGE] No answers from LDAP service, which is not a good thing. Better check it ASAP!")
        else:
            print(f"[INFO] Headache v1.1: node_successes == {node_successes} and node_attempts == {node_attempts} and I don't understand - fuck!")
    else:
        print(f"[INFO] Headache v1.2: node_successes == {node_successes} and node_attempts == {node_attempts} and I don't understand - damn!")
    
# }}}
