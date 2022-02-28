# zabbix-ssl-cert-statistic
A shell script to grab statistic of SSL Certificates
# How to Use:

   0. Place script wherever in accessible directory
      for Zabbix user(i.e. **'/etc/zabbix/scripts'**)
      on the local Zabbix server with working zabbix agent
   1. **NOTE ->** Ensure that interface for monitored host set to zabbix agent on
   the **localhost(127.0.0.1:10050)**. Port could be different depending on your
   configuration.
   2. Grant access for Zabbix user to script directory
      and script itself
      
      > chown root:zabbix /etc/zabbix/scripts
      
      > chmod 550 -R /etc/zqbbix/scripts
   
   3. Add UserParameter in zabbix_agent.conf (you can place it as well after "UnsafeUserParameters")
   
      > UserParameter=ssl-check[*],/etc/zabbix/scripts/ssl_cert_check.sh $1 $2 $3
      
   4. Restart the Zabbix agent (according to your installation)
  
      > systemctl restart zabbix-agent
      
   5. Import **template-ssl-cert-check.xml**
   6. Fill the inherited macros field {$WEBHOST} in attached template to the host 
   and change {$PORT} if necessary (by default scripts uses 443 port)
   7. Enjoy!
