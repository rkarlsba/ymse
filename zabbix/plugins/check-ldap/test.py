#!/usr/bin/env python3.11

from ldap import Server, Connection, ALL, NTLM

p_ldap_server = 'openldap-prod01.oslomet.no'
p_ldap_user='roysk'
p_ldap_password='Medarbeideravgift1'
p_auto_bind=True

#Server = "ldaps://openldap-prod01.oslomet.no/"
ldap_server = Server(p_ldap_server, get_info=ALL)
ldap_conn = Connection(ldap_server, user=p_ldap_user, password=p_ldap_pass, auto_bind=p_auto_bind)
conn.extend.standard.who_am_i()
server.info
