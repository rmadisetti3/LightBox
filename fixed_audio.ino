/* Integrated Audio and State control using IO expander
   Created:    Mon Nov 2, 2020
   Author:     Kenneth Adam Barnett
   Board:      Arduino Pro Micro (ATMEGA32U4)
*/

#include <SPI.h>
//#include <Servo.h> // Required by M1

#define IDLE 0
#define PLAYLIVE 1
#define PLAYRECORD 2
#define RECORD 3


// SPI variables
unsigned char inSPI;
unsigned char outSPI;

// Audio sampling variables
double tSine = 0.0;
const float omega[9] = {0.0f, 0.67495f, 0.75761f, 0.80266f, 0.90095f, 1.01129f, 1.07142f, 1.20263f, 1.27414f};
volatile unsigned int currentNote = 0;

// Lock variable for the SPI read/write operation
volatile unsigned int tsLock = 0;

// State variables
unsigned char state = IDLE;
// Input buffer that can hold 30 seconds of button samples (2Hz)
unsigned char sampleArray[60];
unsigned int recCursor = 0;
unsigned int playCursor = 0;

// Variables to store reset button and potentiometer readings
bool reset = 0;
int statePot;

//Servo myServo1;
//Servo myServo2;

void setup () {
  pinMode(10, OUTPUT);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  //myServo1.attach(6);
  //myServo1.write(0);
  //myServo2.attach(9);
  //myServo2.write(0);


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
  /// TODO: Read the state potentiometer and reset button from the correct pins here
  statePot = analogRead(A1);

  if (reset) {
    // If Reset is pressed, return the state to idle and
    // continue sampling reset button. If reset button is held
    // for 3 seconds, reset the record cursor the the beginning
    state = IDLE;
    // Start a counter, if it reaches the three second mark, reset the record cursor
    unsigned int resetCount = 0;
    while (reset) {
      if (resetCount == 30) {
        recCursor = 0;
      }
      delay(100);
      ++resetCount;
    }
  }
  else if (statePot < 256) {
    // Idle state (off, but receiving power)
    state = IDLE;
    delay(500);

  }
  else if (statePot < 512) {
    // Play Live
    state = PLAYLIVE;
    while (state == PLAYLIVE) {
      // Read the user input
      readSPI();
      // Set the tone played by the interrupt routine
      setTone();

      /// TODO: Replace the delay here and add the controls
      // for the motors here with delay timers. The function should
      // read button inputs stored in the inSPI variable and produce
      // a unique motor routine that takes roughly half a second
      //moveServos();
      delay(500);

      /// TODO: Read the state potentiometer and reset button from the correct pins here
      statePot = analogRead(A1);
       
      // If the state has been changed or the reset button has been hit,
      // reset the state to idle to end the loop
      if (statePot < 256 || statePot > 512 || reset) {
        state = IDLE;
        // Stop the note from playing
        inSPI = 0;
        setTone();

        /// TODO: If the motors need to be reset to a default position, do so here.
        //myServo1.write(0);
        //myServo2.write(0);
      }
    }

  }
  else if (statePot < 768) {
    // Play Record
    state = PLAYRECORD;
    playCursor = 0;
    while (state == PLAYRECORD) {
      // Only play from the recording if there are inputs stored
      if (recCursor > 0) {
        // Play the tone from the recording at the play cursor and advance the cursor
        inSPI = sampleArray[playCursor++];
        setTone();

        /// TODO: Replace the delay here and add the controls
        // for the motors here with delay timers. The function should
        // read button inputs stored in the inSPI variable and produce
        // a unique motor routine that takes roughly half a second
        //moveServos();
        delay(500);
      }
      else {

        delay(500);
      }

      /// TODO: Read the state potentiometer and reset button from the correct pins here
      statePot = analogRead(A1);
       
      // If the state has been changed or the reset button has been hit,
      // reset the state to idle to end the loop
      if (statePot < 512 || statePot > 768 || reset) {
        state = IDLE;
        // Stop the note from playing
        inSPI = 0;
        setTone();

        /// TODO: If the motors need to be reset to a default position, do so here.
        //myServo1.write(0);
        //myServo2.write(0);
      }
    }
  }
  else {
    // Record
    state = RECORD;
    while (state == RECORD) {
      // Only record if there is room in the array
      if (recCursor < 60) {
        // Read the user input
        readSPI();
        // Save the user input at the current record cursor and advance the cursor
        sampleArray[recCursor++] = inSPI;

        // Set the tone played by the interrupt routine
        setTone();
      }

      delay(500);

      /// TODO: Read the state potentiometer and reset button from the correct pins here
      statePot = analogRead(A1);
       
      // If the state has been changed or the reset button has been hit,
      // reset the state to idle to end the loop
      if (statePot < 768 || reset) {
        state = IDLE;
        // Stop the note from playing
        inSPI = 0;
        setTone();

      }
    }
  }



}

ISR(TIMER1_COMPA_vect) {
  // sin(2*pi*f/1024*n)
  if (currentNote) {
      
      // Calculate the current sample
      tSine += sin(omega[currentNote]);

      // Anti-aliasing, keep tSine bounded by 2pi
      if(tSine >= 6.283185) {
        tSine -= 6.283185;
      }

      outSPI = (unsigned char)(127.5 + 127.0 * sin(tSine));
  }
  else {
      outSPI = 0;
      tSine = 0.0;
  }
  writeSPI();

}

void readSPI() {
  // If write has the lock, spin until the lock is available
  while (tsLock);
  tsLock = 1;
  digitalWrite(10, LOW);
  SPI.transfer(0x41);
  SPI.transfer(0x12);
  inSPI = SPI.transfer(0x00);
  digitalWrite(10, HIGH);
  tsLock = 0;
}

void writeSPI() {
  // If read has the lock, skip the write (drops an audio sample)
  if (tsLock == 0) {
    tsLock = 1;
    digitalWrite(10, LOW);
    SPI.transfer(0x40);
    SPI.transfer(0x13);
    SPI.transfer(outSPI);
    digitalWrite(10, HIGH);
    tsLock = 0;
  }
}

void setTone() {

  int noteToSet;

  // If there is no input, set the note to 0
  if (inSPI == 0) {
    noteToSet = 0;
  }
  // If there is input, scan each bit and play the highest note that is held
  else {
    unsigned char bitScan = inSPI;
    for (int i = 1; i < 9; +i) {
      if (bitScan & 1) {
        noteToSet = i;
      }
      bitScan = bitScan >> 1;
    }

  }

  currentNote = noteToSet;
}
/*
void moveServos() {
    myServo1.write(((inSPI * 180) / 255));
    myServo2.write((360 - (inSPI * 180) / 255));
}
*/