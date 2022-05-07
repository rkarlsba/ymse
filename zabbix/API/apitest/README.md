[//]: # vim:ts=4:sw=4:sts=4:et:ai:tw=80

# Local, unencrypted password store

I know this is a terrible idea, but I can't find a practical way to do this, so
I'll give it a shot this way.

This file should be saved as `local_passwords.py` and will, with that name,
be ignored by git by default. We really don't want authentication data in a
versioning system like git or similar, for obvious reasons.

## Example
```
api_user = 'myzabbixapi'
api_password = 'supersecretpassword'
api_url = 'https://my-zabbix-box.oslometh.no/zabbix/api_jsonrpc.php';
```

***[roy](mailto:roy@karlsbak.net)***

