#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
# See README.md for details

# Libraries {{{

import argparse
import ldap
import logging
import os
import re
import sys
import traceback

# }}}
# Functions {{{

def list_of_strings(arg):
    return arg.split(',')

# }}}
# Globals {{{

# User globals
base_dn = None
user_dn = None
nodes = ()
cluster = None
debug = False
port = 636
uri = 'ldaps://'
username = None
password = None
password_file = None
node_max_attempts = 1
node_attempts = 0
node_successes = 0
cluster_max_attempts = 5
cluster_attempts = 0
cluster_successes = 0
cluster_delay = 1       # Second(s)
ldap_nodes = ()
#ldap_nodes = (
#        'openldap-prod01.oslomet.no',
#        'openldap-prod02.oslomet.no'
#)
#ldap_cluster = 'ldap.oslomet.no'
#base_dn = 'ou=tilsatt,ou=oslomet,dc=oslomet,dc=no'

builtinhelp = False

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
def probe_cluster(cluster, cluster_max_attempts):
    ldap.set_option(ldap.OPT_X_TLS_REQUIRE_CERT, ldap.OPT_X_TLS_NEVER)
    user_dn = f"uid={username},{base_dn}"
    for i in range(1, cluster_max_attempts):
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
            print("Can't connect to server - someone yanked the plug?")
            pass

        except Exception as e:
            logging.error(traceback.format_exc())
            pass

# }}}
# error(errstr) {{{

def error(errstr):
    print(f"[ERROR] {errstr}", file=sys.stderr)
    logging.error(f"[INFO] Internal error - check the logs for the zabbix-check-ldap-auth.py check: {errstr}")
    print(f"[INFO] Internal error - check the logs for the zabbix-check-ldap-auth.py check")
    exit(1)

# }}}

# }}}
# Main {{{

if __name__ == "__main__":
    # Først argparse
    argparser = argparse.ArgumentParser(add_help=builtinhelp)

    argparser.add_argument("-B", "--base-DN", type=str, help="BaseDN")
    argparser.add_argument("-s", "--user-DN", type=str, help="UserDN if not the same as BaseDN")
    argparser.add_argument("-n", "--nodes", type=list_of_strings, help="Add one or more node for checking, typically --nodes node1,node2,node3 etc, with IPs or node names")
    argparser.add_argument("-c", "--cluster", type=str, help="Add cluster for checking (only one)")
    argparser.add_argument("-d", "--debug", type=bool, help=f"Enable debugging (default {debug})")
    argparser.add_argument("-p", "--port", type=int, help=f"Typically 636 for LDAPS or 389 for LDAP, default {port}")
    argparser.add_argument("-u", "--uri", type=str, help=f"Typically LDAPS or LDAP, default {uri}")
    argparser.add_argument("-U", "--user", type=str, help="Username")
    argparser.add_argument("-P", "--password", type=str, help="Passsword")
    argparser.add_argument("-F", "--password-file", type=str, help="Path to passsword file with user:s3Cr3t")

    if not builtinhelp:
        argparser.add_argument("-h", "--help", action="store_true", help="Show this help message and exit")

    args = argparser.parse_args()

    if args.base_DN:
        base_dn = args.base_DN
    if args.user_DN:
        user_dn = args.user_DN
    else:
        user_dn = base_dn
    if args.nodes:
        nodes = args.nodes
    if args.cluster:
        cluster = args.cluster
    if args.password:
        password = args.password
        print(f'Password is {password}')
    if args.password_file:
        error("FIXME: Har ikke laga ferdig denne ennå")


    # We're going to check for *either* the status of the nodes in a cluster or
    # the status of the cluster, not boths - that'll be two different checks!
    if len(nodes) > 0 and cluster is not None:
        error(f"It seems you have defined both nodes (to {nodes}) and a cluster name (to {cluster}). This is not how we do things with this plugin. Either you define a cluster name to check the cluster, or you define the nodes in a cluster. The reason is simply that you want separate triggers for the two. If a node dies, it's bad news, right, but it's not a big deal so far as not all the nodes stop answering.")

    if not builtinhelp and args.help:
        argparser.print_help()
        sys.exit(0)

    # Så resten
    # username,password = getpwtok()
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
