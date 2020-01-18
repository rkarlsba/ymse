#!/usr/bin/python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
# {{{
# import paho.mqtt.client as mqtt

# Client(client_id=””, clean_session=True, userdata=None, protocol=MQTTv311, transport=”tcp”)

# mqtt_broker ="mqtt.karlsbakk.net" 
# mqtt_client = mqtt.Client("mqtt.karlsbakk.net")
# mqtt_client.connect(mqtt_broker)
# mqtt_client.subscribe("#")
# 
# mqtt_client.mqtt_on_message=on_message        #attach function to callback
# 
# def on_mqtt_message(mqtt_client, userdata, message):
#     print("message received " ,str(message.payload.decode("utf-8")))
#     print("message topic=",message.topic)
#     print("message qos=",message.qos)
#     print("message retain flag=",message.retain)
# }}}

import paho.mqtt.client as mqtt #import the client1
import time
############
def on_message(client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))
    print("message topic=",message.topic)
    print("message qos=",message.qos)
    print("message retain flag=",message.retain)
########################################
broker_address="192.168.1.184"
#broker_address="iot.eclipse.org"
print("creating new instance")
client = mqtt.Client("P1") #create new instance
client.on_message=on_message #attach function to callback
print("connecting to broker")
client.connect(broker_address) #connect to broker
client.loop_start() #start the loop
print("Subscribing to topic","house/bulbs/bulb1")
client.subscribe("house/bulbs/bulb1")
print("Publishing message to topic","house/bulbs/bulb1")
client.publish("house/bulbs/bulb1","OFF")
time.sleep(4) # wait
client.loop_stop() #stop the loop
