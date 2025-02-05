#!/usr/bin/env python3

import sys
import ldap

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
