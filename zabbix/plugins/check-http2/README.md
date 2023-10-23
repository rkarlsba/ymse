# HTTP/2 scan

As HTTP/2 is declared unsafe as of October 2023, I need a Zabbix scan to find
servers supporting it to turn it off.

Syntax:
```
check_http2 {HOST.HOST}
```

To just test it:
```bash
check_http2 $hostname
```
