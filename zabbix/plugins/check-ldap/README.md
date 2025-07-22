[comment]: <> vim:ts=4:sw=4:sts=4:et:ai

LDAP Auth check
==============================================================================
Config is given in the client code itself - quick and dirty, but it works.

password\_file is just the file containing username:password from which to connect to LDAP
debug = 1 (or True) enables debugging - not in use atm (2024-08-19)
username/passwords are just placeholder variables so far
uri is how to access LDAP - usually LDAPS
port is usually 636 for LDAPS or 389 for LDAP
ldap\_nodes are the nodes behind the 

```python
ldap_pw_file = "~/.ldap_passwd"
debug = 1
username = ''
password = ''
uri = 'ldaps://'
port = 636
ldap_nodes = (
    'ldap1.my.tld',
    'ldap1.my.tld',
)
ldap_cluster = 'ldap.my.tld'
base_dn = 'ou=something,ou=something,dc=my,dc=tld'
```

[roy](mailto:roy@karlsbakk.net)
