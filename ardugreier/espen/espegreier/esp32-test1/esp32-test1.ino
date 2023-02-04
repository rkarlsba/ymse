
#include <time.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

void setup() {
  Serial.begin(115200);
}

uint8_t temp_farenheit;
float temp_celsius;
char strftime_buf[64];
time_t now = 0;
struct tm timeinfo;
char buf[256];

float temp_celsius_direct;

void loop() {
  int measurement = 0;
  
  measurement = hallRead();
  
  Serial.print("Hall sensor measurement: ");
  Serial.println(measurement); 
  
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  sprintf(buf,"scan start %02d:%02d:%02d ",timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
  Serial.print (buf);

  temp_farenheit = temprature_sens_read();
  temp_celsius = ( temp_farenheit - 32 ) / 1.8;

  temp_celsius_direct = temperatureRead();
  
  Serial.print("Temp onBoard ");
  Serial.print(temp_farenheit);
  Serial.print("°F ");
  Serial.print(temp_celsius);
  Serial.print("°C and ");
  Serial.print(temp_celsius_direct);
  Serial.println("°C");
  delay(1000);
  now++;
}
