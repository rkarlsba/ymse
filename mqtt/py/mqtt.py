#!/usr/bin/python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import paho.mqtt.client as mqtt
import psycopg2
from config import config

mqtt_broker = "localhost"

def on_connect(mqttc, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    mqttc.subscribe("#")

def on_message(mqttc, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

def pg_connect():
    """ Connect to the PostgreSQL database server """
    conn = None
    try:
        # read connection parameters
        params = config()
 
        # connect to the PostgreSQL server
        print('Connecting to the PostgreSQL database...')
        conn = psycopg2.connect(**params)
      
        # create a cursor
        cur = conn.cursor()
        
        # execute a statement
        print('PostgreSQL database version:')
        cur.execute('SELECT version()')
 
        # display the PostgreSQL database server version
        db_version = cur.fetchone()
        print(db_version)
       
       # close the communication with the PostgreSQL
        cur.close()
    except (Exception, psycopg2.DatabaseError) as error:
        print(error)
    finally:
        if conn is not None:
            conn.close()
            print('Database connection closed.')
 
def do_mqtt():
    mqttc = mqtt.Client()
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message
    mqttc.connect(mqtt_broker)

    mqttc.loop_forever()
     
if __name__ == '__main__':
    pg_connect()
    do_mqtt()

