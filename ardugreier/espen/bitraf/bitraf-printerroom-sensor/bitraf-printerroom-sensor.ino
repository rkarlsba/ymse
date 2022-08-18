/*
 * vim:ts=2:sw=2:sts=2:ai:ft=c:tw=80:wrap
 *
 * bitraf-printerroom-sensor.ino
 *
 * Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
 * Released under AGPL - see LICENSE for more info.
 * 
 * Use a DHT-22 and a CCS811 to monitor temperature, humidity, CO2 and VoC in
 * Bitraf's 3d printer room. Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
 *
 * The one installed on Bitraf, uses a nodemcu 1.0 (ESP-12E), so choose this
 * board if you need to reflash the current one.
 * 
 * Changelog
 * 
 * 2020-11-24: Removed a hang left from an example, intended to make the unit hang
 * until the WDT killed it, but not stopping the WDT feeder, effectively hanging
 * the whole unit.
 *
 * 2021-04-09: Added more info about the build and details about the one
 * currently installed at Bitraf.
 *
 */

#include <DHT.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "Adafruit_CCS811.h"

//#include <ESPAsyncTCP.h>
//#include <ESPAsyncWebServer.h>

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP);

Adafruit_CCS811 ccs;

#define DHTPIN 14            // D5 on ESP8266
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "SKB_Langtid";
const char* password = "Bymisjon2015";

#define SEC               1000
#define MQTT_SERVER       "mqtt.karlsbakk.net"
#define MQTT_PORT         1883
#define MQTT_MAX_RETRIES  5
#define SAMPLE_DELAY      10*SEC
#define CCS_TIMEOUT       10

/*
 * Global variables - measurements
 * This should be a class soem day…
 */
float env_temp;
float env_hum;
int env_eco2 = -1;
int env_tvoc = -1;

String http_header;

//AsyncWebServer server(80);

WiFiClient wificlient;

Adafruit_MQTT_Client mqttclient(&wificlient, MQTT_SERVER, MQTT_PORT);

Adafruit_MQTT_Publish mqtt_temp = Adafruit_MQTT_Publish(&mqttclient, "my/3dprinterroom/temperature");
Adafruit_MQTT_Publish mqtt_hum = Adafruit_MQTT_Publish(&mqttclient, "my/3dprinterroom/humidity");
Adafruit_MQTT_Publish mqtt_eco2 = Adafruit_MQTT_Publish(&mqttclient, "my/3dprinterroom/eco2");
Adafruit_MQTT_Publish mqtt_tvoc = Adafruit_MQTT_Publish(&mqttclient, "my/3dprinterroom/tvoc");

void MQTT_connect();

void setup() {
  delay(3000); // Fordiatte!
  Serial.begin(115200);

  Serial.println(F("Testing new code from 2021-04-09"));

  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  
  // Disable AP
  WiFi.softAPdisconnect (true);

  // Station stuff
  Serial.println(F("Connecting to WiFi"));
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  if (ccs.begin()) {
    Serial.println(F("Activating CCS811 - please wait"));
    while (!ccs.available());
    float temp = ccs.calculateTemperature();
    ccs.setTempOffset(temp - 25.0); // WTF?
  } else {
    delay(3*SEC);
    for (int i=0;;i++) {
      Serial.print(F("["));
      Serial.print(i);
      Serial.print(F("] "));
      Serial.println(F("Failed to start css811 sensor! Please check your wiring."));
      delay(1*SEC);
    }
  }

  timeClient.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  env_temp = dht.readTemperature();
  env_hum = dht.readHumidity();

  int reset_countdown = 0;
  int have_ccs_data = 0;

  timeClient.update();
  Serial.print(F("Klokka er: "));
  Serial.println(timeClient.getFormattedTime());
  
  if (ccs.available()) {  
    // We have CCS data!
    have_ccs_data=1;
    if(!ccs.readData()) {
      // "calculated" temperature from the css - ignored
      // ccs_temp = ccs.calculateTemperature();
    
      // Estimated CO2 amount
      env_eco2 = ccs.geteCO2();
    
      // Total amount of volatile organic compounds
      env_tvoc = ccs.getTVOC();
    } else {
      Serial.println(F("css.readData()"));
    }
  } else {
    Serial.println(F("No CCS data :("));
  }

  MQTT_connect();

  // Now we can publish stuff!
  if (! mqtt_temp.publish(env_temp)) {
    Serial.println(F("MQTT Temp publish failed"));
  } else {
    Serial.print(F("MQTT Temp publish OK! ("));
    Serial.print(env_temp);
    Serial.println(")");
  }

  Serial.print(F("\nSending temp/hum val "));
  Serial.print(env_temp);
  Serial.print(";");
  Serial.println(env_hum);
  
  if (! mqtt_hum.publish(env_hum)) {
    Serial.println(F("MQTT Hum publish failed"));
  } else {
    Serial.print(F("MQTT Hum publish OK! ("));
    Serial.print(env_hum);
    Serial.println(")");
  }

  if (have_ccs_data) {
    if (! mqtt_eco2.publish(env_eco2)) {
      Serial.println(F("MQTT CCS811 CO2 publish failed"));
    } else {
      Serial.print(F("MQTT CCS811 CO2 publish OK! ("));
      Serial.print(env_eco2);
      Serial.println(")");
    }
  
    if (! mqtt_tvoc.publish(env_tvoc)) {
      Serial.println(F("MQTT CCS811 total volatile organic compounds publish failed"));
    } else {
      Serial.print(F("MQTT CCS811 total volatile organic compounds publish OK! ("));
      Serial.print(env_tvoc);
      Serial.println(")");
    }
  }

  Serial.printf("time;temp;hum;eco2;tvoc\n");
  Serial.printf("%i;%f;%f;%i;%i\n", timeClient.getEpochTime(), env_temp, env_hum, env_eco2, env_tvoc);

  delay(SAMPLE_DELAY);
}


// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqttclient.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = MQTT_MAX_RETRIES;
  while ((ret = mqttclient.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqttclient.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqttclient.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      /* MQTT server doesn't want to talk to us. We're left out in the cold and there's
       * nothing more to do, so we'll just give up and disable feeding the watchdog timer
       * and wait for it to kill us, starting over again next life :(
       */
      Serial.println("MQTT timeout - Calling ESP.wdtDisable() to End This Life (tm)");
      ESP.wdtDisable();
      
      /* Waiting for the inevitable… */
      delay(60000);
    }
  }
  Serial.println("MQTT Connected!");
}
