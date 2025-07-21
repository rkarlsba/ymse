#!/usr/bin/env python3

import sys
import ldap

def get_user_pass(password_file):
    try:
        f=open(password_file,"r")
        lines=f.readlines()
        for line in lines:
            line = re.sub(r"[\r\n]", "", line)
            auth_tokens = line.split(':')
        f.close()

        print(f"{auth_token[0]}:{auth_token[0]}")

    except Exception as e:
        logging.error(traceback.format_exc())

def main(): 
    Server = "ldaps://openldap-prod01.oslomet.no/"
    DN, Secret, un = sys.argv[1:4]

    opt_basedn="ou=tilsatt,ou=oslomet,dc=oslomet,dc=no";
    Scope = ldap.SCOPE_SUBTREE
    Filter = "(&(objectClass=user)(sAMAccountName="+un+"))"
    Attrs = ["displayName"]

    l = ldap.initialize(Server)
    l.protocol_version = 3
    print l.simple_bind_s(DN, Secret)

    r = l.search(Base, Scope, Filter, Attrs)
    Type,user = l.result(r,60)
    Name,Attrs = user[0]
    if hasattr(Attrs, 'has_key') and Attrs.has_key('displayName'):
      displayName = Attrs['displayName'][0]
      print displayName

    sys.exit()

if __name__ == "__main__":
    main()
