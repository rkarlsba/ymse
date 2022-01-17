#define SERVO 2
#define COMMSPEED 57600

#include <Servo.h>

Servo myServo;  // create a servo object

void setup() {
  myServo.attach(SERVO);    // attaches the servo on pin 9 to the servo object
  Serial.begin(COMMSPEED);  // open a serial connection to your computer
  randomSeed(analogRead(0));

  Serial.println(F("Let's go!"));
}

void loop() {
  int a;
  int sec=1000;
  Serial.println(F("Just a sec..."));
  delay(sec);

  myServo.write(1);
  delay(2*sec);
  myServo.write(1);
  delay(2*sec);
}
