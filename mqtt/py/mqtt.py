#!/usr/bin/python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import paho.mqtt.client as mqtt

mqtt_broker = "localhost"

def on_connect(mqttc, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    mqttc.subscribe("#")

def on_message(mqttc, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

mqttc = mqtt.Client()
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.connect(mqtt_broker)

mqttc.loop_forever()
