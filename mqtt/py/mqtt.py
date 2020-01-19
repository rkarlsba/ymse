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

import paho.mqtt.client as mqtt
import time

mqtt_broker ="localhost" 
topic="#"

def on_my_mqtt_message(mqtt_client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))
    print("message topic=",message.topic)
    print("message qos=",message.qos)
    print("message retain flag=",message.retain)

print("creating new instance")
mqtt_client = mqtt.Client(mqtt_broker) #create new instance
mqtt_client.on_mqtt_message=on_my_mqtt_message #attach function to callback

print("connecting to broker")
mqtt_client.connect(mqtt_broker) #connect to broker
mqtt_client.loop_start() #start the loop

print("Subscribing to topic",topic)
ret = mqtt_client.subscribe(topic, 0)
print("Return value: ",ret)
# print("Publishing message to topic",topic)
# mqtt_client.publish(topic,"OFF")

time.sleep(30) # wait
mqtt_client.loop_stop() #stop the loop
