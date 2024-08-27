#!/usr/bin/env python3
# ldapsearch -y $HOME/x123 -z 10000 -o ldif-wrap=no -x -W -H ldaps://openldap-prod01.oslomet.no -b "ou=tilsatt,ou=oslomet,dc=oslomet,dc=no" -D "uid=roysk,ou=tilsatt,ou=oslomet,dc=oslomet,dc=no"
# Globals
ldap_uri="ldaps://openldap-prod01.oslomet.no"
ldap_bindDN="uid=roysk,ou=tilsatt,ou=oslomet,dc=oslomet,dc=no",
ldap_bindPW="your_password",
ldap_baseDN="OU=folder,OU=folder,DC=domain,DC=local",
ldap_filterstr="&(objectClass=user)(!(objectClass=computer))(memberOf:1.2.840.113556.1.4.1941:=CN=cool_group,OU=folder,OU=folder,DC=domain,DC=local)",
ldap_attrlist=["userPrincipalName", "givenName"],   # None to fetch all attributes
ldap_timeout=-1,                                    # wait indefinitely
ldap_pagesize=1000                                  # this is an internal parameter that says how many records do you want to fetch per request
                                                   # it doesn't change the result, since pages are fetched as needed
                                                   # by default, the max number of records allowed by AD is 1000
                                                   # so if you request more than 1000 records, it will return 1000
