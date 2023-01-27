/*
 * vim:ts=2:sw=2:sts=2:ai:ft=c:tw=80:wrap
 *
 * bitraf-printerroom-sensor.ino
 *
 * Basert på bitraf-printerrom-sensor. Stort sett det samme - denne leser fra analog
 * input og sender MQTT. På grunn av flere analoge inputs, er den skrevet for ESP32.
 * 
 * Released under AGPL - see LICENSE for more info.
 * 
 * Changelog
 * 
 * 2021-09-06: Skrive skiten basert på nevnte prosjekt.
 * 
 */

#include <WiFi.h>
#include "time.h"
#include <MQTT.h>
#include "esp32pins.h"

#define MAXSTRLEN 1024

const char* mqtt_id = "plantheaven";

const char* mqtt_server = "mqtt.karlsbakk.net";
const unsigned short mqtt_port = 1883;
const unsigned short mqtt_max_retries = 5;

const char* wifi_ssid = "Get-C83968";
const char* wifi_password  = "emmlvwn3kj";

const char* ntp_server = "no.pool.ntp.org";
const long  gmt_offset_sec = 3600;
const int   daylight_offset_sec = 3600;

/*
 * Global variables - measurements
 * This should be a class soem day…
 */
String http_header;
const int sensorer = 8;

WiFiClient wifi_client;
MQTTClient mqtt_client;

unsigned long last_millies;

const char* mqtt_topic_base = "/Karlsbakk/Plantemekka/Jordfukt" ;

int glob_pval = analogRead(ADC2_CH5);

void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup() {
  char topic[MAXSTRLEN];
  char payload[MAXSTRLEN];

  delay(3000); // Fordiatte!
  Serial.begin(115200);

  Serial.println(F("Testing new code from 2021-09-06"));

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  
  // Disable AP
  WiFi.softAPdisconnect (true);

  // Station stuff
  Serial.println(F("Connecting to WiFi"));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println(".");
  }

  configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  mqtt_client.begin(mqtt_server, wifi_client);

  while (!mqtt_client.connect(mqtt_id))
  {
    Serial.print(".");
    delay(1000);
  }

  snprintf(topic, MAXSTRLEN, "%s/hello", mqtt_topic_base);
  snprintf(payload, MAXSTRLEN, "Starting up at %d", time);
  mqtt_client.publish(topic, payload);
}
int count = 0;

void loop() {
  char topic[MAXSTRLEN];
  char payload[MAXSTRLEN];

  int planter = 3;

  Serial.println(count++);
  delay(1000);
  if ((count % 10) == 0) {
    Serial.printf("Count is %d\n", count);
    //for (int p=1;p<=planter;p++) {
      int p = 1; // hjelpe litt
      int pval = analogRead(ADC1_CH5);
      snprintf(topic, MAXSTRLEN, "%s/Plante/%d", mqtt_topic_base, p);
      snprintf(payload, MAXSTRLEN, "%d", fuktprosent);
      mqtt_client.publish(topic, payload);
      Serial.printf("Sending topic '%s' with payload '%d'\n", topic, payload);
    //}
  }
}
