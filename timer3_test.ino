/* Timer3 Interrupt Test
 * Description: This program uses the 16-bit Timer3
 * as an interrupt to flash the built in LED at 1Hz
 *
 * Created:   Sat Oct 17 2020
 * Processor: Arduino Leonardo
 * Compiler:  Arduino AVR (Proteus)
 */

// Peripheral Configuration Code (do not edit)
//---CONFIG_BEGIN---
#pragma GCC push_options
#pragma GCC optimize ("Os")

#include <core.h> // Required by cpu
#include <cpu.h>

#pragma GCC pop_options

// Peripheral Constructors
CPU &cpu = Cpu;

void peripheral_setup () {
}

void peripheral_loop() {
}
//---CONFIG_END---

bool toggle;

void setup () {
   peripheral_setup();
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
   TCNT3 = 0;
   
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
}

// Interrupt Service routine for Timer3 Compare Match A 
ISR(TIMER3_COMPA_vect){
   toggle = !toggle;
}

void loop() {
   peripheral_loop();
   // the built in LED should flash once a second
   cpu.setBuiltInLED(toggle);

}
