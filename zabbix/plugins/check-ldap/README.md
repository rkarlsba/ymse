[comment]: <> vim:ts=8:sw=8:sts=8:et:ai

LDAP Auth check
==============================================================================
Config is given in the client code itself - quick and dirty, but it works.

password_file is just the file containing username:password from which to connect to LDAP
debug = 1 (or = True) enables debugging - not in use atm (2024-08-19)
username/passwords are just placeholder variables so far
uri is how to access LDAP - usually LDAPS
port is usually 636 for LDAPS or 389 for LDAP
ldap_nodes are the nodes behind the 

```python
password_file = "~/.ldap_passwd"
debug = 1
username = ''
password = ''
uri = 'ldaps://'
port = 636
ldap_nodes = (
        'openldap-prod01.oslomet.no',
        'openldap-prod02.oslomet.no',
)
ldap_cluster = 'openldap.oslomet.no'
base_dn = 'ou=tilsatt,ou=oslomet,dc=oslomet,dc=no'
```

[roy](mailto:roy@karlsbakk.net)
