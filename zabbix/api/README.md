# Some scripts
Here's a collection of scripts using the Zabbix API to do mass updates or
similar. They should normally work, but please read the code or at least the
docs if given.

## local\_passwords.py

This file contains the usernames and passwords to use when connecting to
zabbix. Below follows example content of this file. Please note that the file
is added to .gitignore to avoid passwords to spread on github. Please remove
it from .gitignore if you want to add it, typically for an in-house git repo.

```python
api_user = api_src_user = 'zabbix-api-user'
api_password = api_src_password = 'supersecret'
api_url = api_src_url = 'https://zabbix.my.tld/zabbix/api_jsonrpc.php';

api_src_user = 'zabbix-api-user'
api_src_password = 'supersecret'
api_src_url = 'https://zabbix.my.tld/zabbix/api_jsonrpc.php';

api_dst_user = api_src_user = 'zabbix-api-user'
api_dst_password = api_src_password = 'supersecret'
api_dst_url = api_src_url = 'https://zabbix.my.tld/zabbix/api_jsonrpc.php';
```

[roy](mailto:roy@karlsbakk.net)
