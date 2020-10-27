/* Generated by Proteus Visual Designer for Arduino
   Modified by Kenneth Barnett and Raj Madisetti
   ECE 3872 Electronic Sculpture Project Group 4B
*/


// Peripheral Configuration Code (Do Not Edit)
//---CONFIG_BEGIN---
#pragma GCC push_options
#pragma GCC optimize ("Os")

//#include <core.h> // Required by cpu
//#include <cpu.h>
//#include <Generic.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
// #include <Adafruit_MCP23008.h> // Required by IOX1
#include <Wire.h> // Required by IOX1
#include <Servo.h> // Required by M1
//#include <Generic/GenericServo.h>

#include <ezButton.h>

#pragma GCC pop_options

// Peripheral Constructors
//CPU &cpu = Cpu;
ezButton SW1(1);
ezButton SW2(2);
ezButton SW3(3);
ezButton SW4(4);
ezButton BTN1(5);
ezButton BTN2(7);
ezButton BTN3(8);
ezButton BTN4(9);
ezButton BTN5(14);
ezButton BTN6(15);
ezButton BTN7(16);
ezButton BTN8(17);
Adafruit_MCP23017 mcp;
Servo myServo;

void peripheral_setup () {
  mcp.begin();      // use default address 0
  mcp.pinMode(0, OUTPUT);
  myServo.attach(6);
  myServo.write(0);
}

void peripheral_loop() {
  SW1.loop();
  SW2.loop();
  SW3.loop();
  SW4.loop();
  BTN1.loop();
  BTN2.loop();
  BTN3.loop();
  BTN4.loop();
  BTN5.loop();
  BTN6.loop();
  BTN7.loop();
  BTN8.loop();
}
//---CONFIG_END---
// Flowchart Variables

bool var_main;

//  State variable declarations
bool var_live;
bool var_play;
bool var_record;
bool var_reset;

//Variable to store current button status
long var_ButtonStatus;

// Variable to keep track of the amount of valid samples in the buffer
long var_SampleCount;

// Variable to keep track of the play cursor's location in the buffer
long var_cur_samp_index;

// Variable to count various loops
long var_loop_count;

// Input buffer that can hold 30 seconds of button samples (2Hz)
long var_sample_array[60];

// Flowchart Routines
void chart_SETUP() {
  var_main = true, var_SampleCount = 0;

  // Initialize the IO Expander
  chart_InitIO();
  // Entry point to the sculpture state machine
  while (var_main) {
    while (!(SW1.getState())) {
      delay(100);
      // This is how Proteus defaulted to doing an if/ else if/ else
      // Make sure to change this in the final software
      if (!(SW2.getState())) goto l17;
      // Entry point for Play Live state
      var_live = true;
      // Stay in Play live state until the switch is opened or rreset is tripped
      while (var_live && !SW4.getState()) {
        // In Play Live mode, read the input and play the corresponding tone
        chart_ReadButtons();
        chart_PlayTone();
        // Trip the state back to idle if the switch is toggled
        if (!(SW2.getState() && !SW4.getState())) {
          var_live = false;
        }
      }
      goto l59;
l17:;
      delay(100);
      if (!(SW3.getState())) goto l35;
      // Entry point for Record state
      var_record = true;
      while (var_record && !SW4.getState()) {
        // Only record when there is room in the buffer
        if (var_SampleCount != 60) {
          // In record mode, read the button input, play the tone, and save the button input to the buffer
          chart_ReadButtons();
          chart_PlayTone();
          var_sample_array[var_SampleCount] = var_ButtonStatus;
          // Increase the sample count to move the write cursor forward
          var_SampleCount = var_SampleCount + 1;
          delay(500);
        }
        // Trip the state back to idle if the switch is toggled
        if (!(SW3.getState() && !SW4.getState())) {
          var_record = false;
        }
      }
      goto l59;
l35:;
      delay(100);
      if (!(SW4.getState())) goto l46;
      // Entry point for the Reset state
      var_loop_count = 0, var_reset = true;
      while (SW4.getState()) {
        delay(100);
        // Sample the reset button once a millisecond, keeping a counter that resets whenever reset is released
        // If the counter isn't reset after 3 seconds, overwrite the recording by setting the write cursor back to zero
        var_loop_count = var_loop_count + 1;
        if (var_loop_count == 30) {
          var_SampleCount = 0;
        }
      }
      goto l59;
l46:;
      delay(100);
    }
    // Entry point for Play Recording state
    var_cur_samp_index = 0;
    var_play = true;
    while (var_play && !SW4.getState()) {
      // Load in input from the play cursor loacation in the buffer and play the corresponding sound
      chart_PlaySoundFromRec();
      delay(500);
      if (!(SW1.getState() && !SW4.getState())) {
        var_play = false;
      }
    }
l59:;
    // Entry point to the idle state, reset servo to default position and stop playing any sounds
    myServo.write(0);
    mcp.writeGPIOAB(0);
  }
}

// Function to read in the user input and store the results as a byte in the buffer
// Each button is represented by a single bit.
void chart_ReadButtons() {
  var_ButtonStatus = 0;
  if (BTN1.getState()) {
    var_ButtonStatus = var_ButtonStatus + 1;
  }
  if (BTN2.getState()) {
    var_ButtonStatus = var_ButtonStatus + 2;
  }
  if (BTN3.getState()) {
    var_ButtonStatus = var_ButtonStatus + 4;
  }
  if (BTN4.getState()) {
    var_ButtonStatus = var_ButtonStatus + 8;
  }
  if (BTN5.getState()) {
    var_ButtonStatus = var_ButtonStatus + 16;
  }
  if (BTN6.getState()) {
    var_ButtonStatus = var_ButtonStatus + 32;
  }
  if (BTN7.getState()) {
    var_ButtonStatus = var_ButtonStatus + 64;
  }
  if (BTN8.getState()) {
    var_ButtonStatus = var_ButtonStatus + 128;
  }
}

// Function to play a sound from the recorded buffer
void chart_PlaySoundFromRec() {
  // Only play tones if there is something valid in the buffer
  if (var_SampleCount != 0) {
    // Load the button input corresponding to the location of the play cursor and play the tone
    var_ButtonStatus = var_sample_array[var_cur_samp_index];
    chart_PlayTone();
    // Increment the play cursor
    var_cur_samp_index = var_cur_samp_index + 1;
    if (!(var_cur_samp_index != var_SampleCount)) {
      // If the play cursor reaches the end of the valid recording buffer, restart the play cursor at the beginning of the buffer
      var_cur_samp_index = 0;
    }
  }
}

// Function that plays whatever tone is defined by the button status variable
void chart_PlayTone() {
  // Move the servo according to the tone played.
  myServo.write((var_ButtonStatus * 180) / 255);
  // Write the button input to te I/O expander to activate the buzzer that plays the tone
  mcp.writeGPIOAB(var_ButtonStatus);
  delay(500);
}

// Function to set all IO expander pins to outputs
void chart_InitIO() {
  for (var_loop_count = 0; var_loop_count <= 8; var_loop_count += 1) {
    mcp.pinMode(var_loop_count, 1);
  }
}

// Entry Points and Interrupt Handlers
void setup () {
  peripheral_setup();
  chart_SETUP();
}
void loop () {
  peripheral_loop();
}
