#!/usr/bin/env python3
# ldapsearch -y $HOME/.pwfile -z 10000 -o ldif-wrap=no -x -W -H ldaps://ldap.my.tld \
#       -b "ou=something,dc=my,dc=tld" \
#       -D "uid=myuser,ou=something,dc=my,dc=tld"
# Globals
ldap_uri="ldaps://ldap.my.tld"
ldap_bindDN="uid=myuser,ou=something,dc=my,dc=tld"
ldap_bindPW="mysecret"
ldap_baseDN="OU=something,DC=my,DC=tld"
ldap_filterstr="&(objectClass=user)(!(objectClass=computer))(memberOf:1.2.840.113556.1.4.1941:=CN=mygroup,OU=something,DC=my,DC=tld)"

# Set this to None to fetch all attributes
ldap_attrlist=["userPrincipalName", "givenName"]

# Wait indefinitely
ldap_timeout=-1

# This is an internal parameter that says how many records do you want to fetch per request. It
# doesn't change the result, since pages are fetched as needed by default, the max number of records
# allowed by AD is 1000. So if you request more than 1000 records, it will return 1000.
ldap_pagesize=1000
