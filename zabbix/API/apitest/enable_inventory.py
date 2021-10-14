#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# curl -i -X POST -H 'Content-Type:application/json' -d'{"jsonrpc":"2.0","method":"host.create","params":{"host":"'$host'","interfaces":[{"type":1,"main":1,"useip":1,"ip":"'$hostip'","dns":"","port":"10050"}],"groups":[{"groupid":2}],"templates":[{"templateid":"'$templateid'"}],"inventory_mode":1},"auth":"'$authtoken'","id":0}' $api
content_type = 'Content-Type:application/json'

api_user = 'perlie'
api_password = 'lagemasseraregreiermedperl'
api_url = 'https://zabbix.karlsbakk.net/zabbix/api_jsonrpc.php';
api_id = 1;

# zabbix_api_login.format(id,user,password)
zabbix_api_login = {
	'jsonrpc': '2.0',
	'id' => {},
	'method': 'user.login',
	'params': {
		'user' => {},
		'password' => {},
	},
};

# zabbix_api_modify_host.format(user,password,id) {{{
# zabbix_api_modify_host = {
#     "jsonrpc": "2.0",
#     "method": "host.update",
#     "auth": {},
#     "id": {}
#     "params": {
#         "hostid": {},
#         "status": 0
#     },
# }
# }}}

# zabbix_api_host_get.format(user,password,id)
zabbix_api_host_get = {
    "jsonrpc": "2.0",
    "method": "host.get",
    "params": {
        "filter": {
            "host": [
                "Zabbix server",
                "Linux server"
            ]
        }
    },
    "auth": "038e1d7b1735c6a5436ee9eae095879e",
    "id": 1
}


# zabbix_api_enable_inventory.format(user,password,id)
enable_json = '{
    "jsonrpc":"2.0",
    "method":"host.create",
    "params":{
        "host":"{}","interfaces":[{
            "type":1,
            "main":1,
            "useip":1,
            "ip":"'$hostip'",
            "dns":"",
            "port":"10050"
        }],
        "groups":[{"groupid":2}],
        "templates":[{
            "templateid":"'$templateid'"}],
        "inventory_mode":1
    },
    "auth":"'$authtoken'","id":0}'

hostname = 'roysk.oslomet.no';
hostip = '2001:700:700:15::162';

enable_json_1 = enable_json.format($hostname, 
