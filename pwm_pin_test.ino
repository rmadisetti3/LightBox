/* Main.ino file generated by New Project wizard
 *
 * Created:   Sat Oct 17 2020
 * Processor: Arduino Leonardo
 * Compiler:  Arduino AVR (Proteus)
 */

#include <Servo.h>

#include <SPI.h>

bool toggle;
bool toggle2;

Servo servo;
Servo servo2;

void setup () {

   // Using Timer/Counter3 for timer interrupt
   // Disable interrupts
   
   cli();
   //Initialize the entire Timer/Counter Control Register A to 0
   TCCR3A = 0; 
   // InitializeTimer/Counter Control B to the binary value:
   // '0000 1100'
   TCCR3B = 0x0C;
   // This sets the timer to the following settings:
   // Waveform Generation Mode (WGM[3:0])
   // '0100'   -   Clear Timer on Compare (CTC)
   // Clock Select (CS[2:0])
   // '100'     -   16MHz Clock, prescaled by 256
   
   // Initialize Timer/Counter value to 0
   TCNT3= 0;
   
   // Set Output Compare Register A to ensure 1Hz
   // 16MHz/256= 62500 Clock Edges per interrupt
   // Minus 1 because it executes on the next clock
   OCR3A = 62499; 
   //  Set the Timer Interrupt Mask Register to mark
   // Output Compare A Match Interrupt Enable bit high
   // 'XXXX XX1X'
   TIMSK3 |= 0x02;
   // Enable interrupts
   sei();

   servo.attach(6);
   servo2.attach(9);

   servo.write(90);
   servo2.write(90);

   pinMode(LED_BUILTIN, OUTPUT);
   pinMode(8, OUTPUT);

}

void loop() {
  toggle2 = !toggle2;
  digitalWrite(8, toggle2);
  delay(1000);
}

ISR(TIMER3_COMPA_vect){
   toggle = !toggle;
   digitalWrite(LED_BUILTIN, toggle);
}