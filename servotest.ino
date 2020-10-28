/*
Servotest.ino
Created by Raj Madisetti
*/

#include <Servo.h> // Required by M1
#include <Stepper.h> 
Servo myServo;
//Stepper myStepper = Stepper(100, 5, 6);

void setup() {
  myServo.attach(9);
  myServo.write(0);
}

void loop() {
  myServo.write(180);
  delay(100);
  myServo.write(360);
  //myStepper.step(500);
}
