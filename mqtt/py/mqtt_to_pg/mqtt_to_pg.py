#!/usr/bin/python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import paho.mqtt.client as mqtt
import psycopg2
from mqtt_to_pg_config import mqtt_to_pg_config

#mqtt_broker = "localhost"
mqtt_broker = "mqtt.karlsbakk.net"
mqtt_clientid = "mqtt.karlsbakk.net"
pg_connection = None

def on_connect(mqttc, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    mqttc.subscribe("#")

# mqtt=> select * from mqtt_messages;
#  id |      clientid      |    topic     | message  | is_enabled |             time
# ----+--------------------+--------------+----------+------------+-------------------------------
#   1 | mqtt.karlsbakk.net | test/kitchen | dev01,on |          1 | 2020-01-18 17:57:13.616925+01
def on_message(mqttc, userdata, msg):
#   msg.replace('"', '\\"')
#   msg.replace('\'', '\\\'')
#   msg.decode("utf-8")
#   msg.replace('\'', '\\\'')
#   msg.encode("utf-8");
    #sql = "INSERT INTO mqtt_messages(clientid, topic, message) values ('{:s}', '{:s}', '{:s}')".format(mqtt_clientid, msg.topic, msg.payload.decode('utf-8'))
    # https://stackoverflow.com/questions/1466741/parameterized-queries-with-psycopg2-python-db-api-and-postgresql
    sql = "INSERT INTO mqtt_messages(clientid, topic, message) values (%(clientid)s, %(topic)s, %(message)s)"
    try:
        pg_cursor = pg_connection.cursor()
        print("Trying", sql)
        print("  with: id", mqtt_clientid, "topic", msg.topic, "message", msg.payload.decode('utf-8'))
        pg_cursor.execute(sql, {"clientid": mqtt_clientid, "topic": msg.topic, "message": msg.payload.decode('utf-8')})
        pg_connection.commit()
        pg_cursor.close()
        print("Ran", sql)
    except (Exception, psycopg2.DatabaseError) as error:
        print("FEILFEILFEIL", error)
        exit(1)

try:
    # read connection parameters
    params = mqtt_to_pg_config()

    # connect to the PostgreSQL server
    print('Connecting to the PostgreSQL database...')
    pg_connection = psycopg2.connect(**params)
  
    # create a cursor
#   pg_cursor = pg_connection.cursor()
    
    # execute a statement
#   print('PostgreSQL database version:')
#   pg_cursor.execute('SELECT version()')

    # display the PostgreSQL database server version
#   db_version = pg_cursor.fetchone()
#   print(db_version)
   
   # close the communication with the PostgreSQL
#  pg_cursor.close()
except (Exception, psycopg2.DatabaseError) as error:
    print(error)
#   finally:
#       if pg_connection is not None:
#           pg_connection.close()
#           print('Database connection closed.')
 
mqttc = mqtt.Client()
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.connect(mqtt_broker)

mqttc.loop_forever()
     
# if __name__ == '__main__':
#   pg_connect()
#   do_mqtt()

