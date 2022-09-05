[//]: # vim:ts=4:sw=4:sts=4:et:ai:tw=80

# Local, unencrypted password store

I know this is a terrible idea, but I can't find a practical way to do this, so
I'll give it a shot this way.

To access the Zabbix API, we'll need an API user and a password. We'll also need
a URL at which we can access the API. Usually this is at
`/zabbx/api_jsonrpc.php` as seen below, but you'll never know, so we'll leave it
all open in a variable.

Create a new file, `local_passwords.py` and will, with that name, be ignored by
git by default as stated in .gitignore. We really don't want authentication data
in a versioning system like git or similar, for obvious reasons.

## `local_passwords.py`
```
api_user = api_src_user = 'zabbixapiuser'
api_password = api_src_password = 'supersecretpassword'
api_url = api_src_url = 'https://zabbix.asdf.com/zabbix/api_jsonrpc.php';

api_dst_user = 'zabbixapiuser'
api_dst_password = 'supersecretpassword'
api_dst_url = 'https://zabbix.asdf.com/zabbix/api_jsonrpc.php';
```

Then import it with something like this:

```
try:
    from local_passwords import *
except:
    print("File local_passwords.py does not exists. Please refer to the README file", file=sys.stderr)
    print("and create the named file before running this again.", file=sys.stderr)
    exit(1)
```

***[roy](mailto:roy@karlsbakk.net)***

