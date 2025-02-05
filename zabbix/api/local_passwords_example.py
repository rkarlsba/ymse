# Sample password file

# Merk at under er det definert tre sett med bruker+pass+url. api_user,
# api_src_user og api_dst_user og tilsvarende for password og url. Dette er for
# å kunne sette kilde- og målbruker og sånt. Per i dag, har bare skriptet henta
# ut data og bruker bare api_user etc, ikke api_src_ eller api_dst_

# Common user for normal use
api_user = 'zabbix-api-user'
api_password = 'supersecret'
api_url = 'https://zabbix.my.tld/zabbix/api_jsonrpc.php';

# User used for migration - source part
api_src_user = 'zabbix-api-user'
api_src_password = 'supersecret'
api_src_url = 'https://zabbix.my.tld/zabbix/api_jsonrpc.php';

# User used for migration - destination part
api_dst_user = 'zabbix-api-user'
api_dst_password = 'supersecret'
api_dst_url = 'https://zabbix.my.tld/zabbix/api_jsonrpc.php';
