// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

Adafruit_MPU6050 mpu;

const char* mqtt_server = "192.168.10.72";
const short int mqtt_port = 1883;
const int mqtt_max_retries = 5;
const int sample_delay = 250; // ms
const char* wifi_ssid = "roysnett24";
const char* wifi_password = "ikkeveldighemmelig";

WiFiClient wificlient;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  
  Adafruit_MQTT_Client mqttclient(&wificlient, mqtt_server, mqtt_port);
  Adafruit_MQTT_Publish acc_x = Adafruit_MQTT_Publish(&mqttclient, "karlsbakk/ymse/mpu6050/acceleration/x");
  Adafruit_MQTT_Publish acc_y = Adafruit_MQTT_Publish(&mqttclient, "karlsbakk/ymse/mpu6050/acceleration/y");
  Adafruit_MQTT_Publish acc_z = Adafruit_MQTT_Publish(&mqttclient, "karlsbakk/ymse/mpu6050/acceleration/z");
  Adafruit_MQTT_Publish gyro_x = Adafruit_MQTT_Publish(&mqttclient, "karlsbakk/ymse/mpu6050/gyro/x");
  Adafruit_MQTT_Publish gyro_y = Adafruit_MQTT_Publish(&mqttclient, "karlsbakk/ymse/mpu6050/gyro/y");
  Adafruit_MQTT_Publish gyro_z = Adafruit_MQTT_Publish(&mqttclient, "karlsbakk/ymse/mpu6050/gyro/z");

  delay(100);
}

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print(a.acceleration.z);
  Serial.print(", ");
  Serial.print(g.gyro.x);
  Serial.print(",");
  Serial.print(g.gyro.y);
  Serial.print(",");
  Serial.print(g.gyro.z);
  Serial.println("");

  delay(100);
}
