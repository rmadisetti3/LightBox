
/* Test the IO Expander
 *
 * Created:    Sat Oct 20, 2020
 * Author:     Kenneth Adam Barnett
 * Board:      Arduino Pro Micro (ATMEGA32U4)
 * 
 */

#include <SPI.h>


char inSPI;
char outSPI;

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

}

void loop() {

   readSPI();
   outSPI = inSPI;
   writeSPI();
   delay(100);
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