/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

#ifndef LED_BUILTIN
# ifdef BUILTIN_LED
#  define LED_BUILTIN BUILTIN_LED
# else
#  define LED_BUILTIN 16
# endif
#endif

int blink_hz = 2; // blinks per second
int blink_duration = 100; // blink duration in milliseconds

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("On");
  delay(blink_hz*1000-blink_duration);
  digitalWrite(LED_BUILTIN, LOW);
  delay(blink_duration);
  Serial.println("Off");
}
