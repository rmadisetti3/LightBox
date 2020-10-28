/*
Servotest.ino
Created by Raj Madisetti
*/

#include <Servo.h> // Required by M1
#include <Stepper.h> 
Servo myServo;
int pos = 0;
//Stepper myStepper = Stepper(100, 5, 6);

void setup() {
  myServo.attach(7);
  myServo.write(0);
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  //myStepper.step(500);
}
