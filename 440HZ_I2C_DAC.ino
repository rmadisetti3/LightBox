/* Test the IO Expander + DAC, should output 440Hz sine wave
 *
 * Created:    Sat Oct 20, 2020
 * Author:     Kenneth Adam Barnett
 * Board:      Arduino Pro Micro (ATMEGA32U4)
 * 
 */

#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

uint16_t outI2C;
long sample = 0;

void setup () {
   dac.begin(0x62);

   // Using Timer/Counter3 for timer interrupt
   // Disable interrupts
   cli();
   //Initialize the entire Timer/Counter Control Register A to 0
   TCCR1A = 0; 
   // InitializeTimer/Counter Control B to the binary value:
   // '0000 1001'
   TCCR1B = 0x09;
   // This sets the timer to the following settings:
   // Waveform Generation Mode (WGM[3:0])
   // '0100'   -   Clear Timer on Compare (CTC)
   // Clock Select (CS[2:0])
   // '001'     -   16MHz Clock, not prescaled

   // Initialize Timer/Counter value to 0
   TCNT1 = 0;

   // Set Output Compare Register A to ensure 1024Hz
   // 16MHz/1024= 15625 Clock Edges per interrupt
   // Minus 1 because it executes on the next clock
   OCR1A = 15624; 
   //  Set the Timer Interrupt Mask Register to mark
   // Output Compare A Match Interrupt Enable bit high
   // 'XXXX XX1X'
   TIMSK1 |= 0x02;
   // Enable interrupts
   sei();

}

void loop() {
}

ISR(TIMER1_COMPA_vect){
   // sin(2*pi*440/1024*n)
   outI2C = (uint16_t)(2047.5 + 2047.0 * sin(2.6998 * (double)sample));
   dac.setVoltage(outI2C, false);
   ++sample;
}
