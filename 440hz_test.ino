
/* Test the IO Expander + DAC, should output 440Hz sine wave
 *
 * Created:    Sat Oct 20, 2020
 * Author:     Kenneth Adam Barnett
 * Board:      Arduino Pro Micro (ATMEGA32U4)
 * 
 */

#include <SPI.h>

unsigned char inSPI;
unsigned char outSPI;
long sample = 0;

void setup () {
  
   pinMode(10, OUTPUT);
   SPI.begin();
   SPI.setBitOrder(MSBFIRST);
   
   // Enable Hardware addressing and disable sequential accessing

   digitalWrite(10, LOW);
   SPI.transfer(0x40);
   SPI.transfer(0x0A);
   SPI.transfer(0x28);
   digitalWrite(10, HIGH);
   delay(100);
   
   // Set Port A GPIO0-7 to input
   digitalWrite(10, LOW);
   SPI.transfer(0x40);
   SPI.transfer(0x00);
   SPI.transfer(0xFF);
   digitalWrite(10, HIGH);
   delay(100);
   
   // Set Port B GPIO0-7 to output
   digitalWrite(10, LOW);
   SPI.transfer(0x40);
   SPI.transfer(0x01);
   SPI.transfer(0x00);
   digitalWrite(10, HIGH);
   delay(100);

   // Using Timer/Counter3 for timer interrupt
   // Disable interrupts
   cli();
   //Initialize the entire Timer/Counter Control Register A to 0
   TCCR3A = 0; 
   // InitializeTimer/Counter Control B to the binary value:
   // '0000 1001'
   TCCR3B = 0x09;
   // This sets the timer to the following settings:
   // Waveform Generation Mode (WGM[3:0])
   // '0100'   -   Clear Timer on Compare (CTC)
   // Clock Select (CS[2:0])
   // '001'     -   16MHz Clock, not prescaled
   
   // Initialize Timer/Counter value to 0
   TCNT3 = 0;
   
   // Set Output Compare Register A to ensure 1024Hz
   // 16MHz/1024= 15625 Clock Edges per interrupt
   // Minus 1 because it executes on the next clock
   OCR3A = 15624; 
   //  Set the Timer Interrupt Mask Register to mark
   // Output Compare A Match Interrupt Enable bit high
   // 'XXXX XX1X'
   TIMSK3 |= 0x02;
   // Enable interrupts
   sei();

}

void loop() {
}

ISR(TIMER3_COMPA_vect){
   // sin(2*pi*440/1024*n)
   outSPI = (unsigned char)(255.5+255.0*sin(2.6998*(double)sample));
   writeSPI();
   ++sample;
}

void readSPI() {
   digitalWrite(10, LOW);
   SPI.transfer(0x41);
   SPI.transfer(0x12);
   inSPI = SPI.transfer(0x00);
   digitalWrite(10, HIGH);
}

void writeSPI() {
   digitalWrite(10, LOW);
   SPI.transfer(0x40);
   SPI.transfer(0x13);
   SPI.transfer(outSPI);
   digitalWrite(10, HIGH);
}
