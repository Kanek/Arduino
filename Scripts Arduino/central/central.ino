#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

const uint8_t MAX_TRY = 5;
const uint8_t PERSISTENCE = 2;

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

unsigned long sensor_active;

// Pins on the LED board for LED's
const uint8_t led_pins[] = { 3,4,5 };
const uint8_t num_led_pins = sizeof(led_pins);

//
// Topology
//

// Single radio pipe address for the 2 nodes to communicate.
const uint8_t num_pipes = 3;
const uint64_t recieve_pipes[num_pipes] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0AA,  0xF0F0F0F0BB};
const uint64_t send_pipes[num_pipes] = { 0xF0F0F0F0D2LL, 0xF0F0F0F0A1,  0xF0F0F0F0B1};


//
// Setup
//

void setup(void)
{

  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24/Central\n\r");

  //
  // Setup and configure rf radio
  //

  radio.begin();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
  
  int i = num_led_pins;
  while(i--)
  {
    pinMode(led_pins[i],OUTPUT);
    digitalWrite(led_pins[i],LOW);
  }
}

//
// Loop
//

void loop(void)
{

  int i = num_pipes;

  while (i--)
  {
    
    //
    // Open pipes to other nodes for communication
    //

    // This simple sketch opens a single pipes for these two nodes to communicate
    // back and forth.  One listens on it, the other talks to it.

    radio.openReadingPipe(1,recieve_pipes[i]);

    //
    // Start listening
    //

    radio.startListening();

    delay(20);


    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &sensor_active, sizeof(unsigned long) );

        // Spew it
        printf("Got sensor, value: %lu at: %d\n\r", sensor_active, i);

        if (sensor_active)
        {
          digitalWrite(led_pins[i],LOW);
        }
        else
        {
          digitalWrite(led_pins[i],HIGH);
        }
      }
    } 
  }
  if ( Serial.available() )
  {
    char mystring[10]= "";
    int i = 0;
    unsigned long message;
    while(Serial.available()) 
    {
      char c = toupper(Serial.read());
      if (c != '\n' && c != '\r')
      {
        mystring[i] = c;
        i++; 
        mystring[i]='\0';
      }
    }
    if (strcmp(mystring, "S0")  == 0) 
    {
      radio.stopListening();
      printf("Turning on the LED 0...\n\r");
      radio.openWritingPipe(send_pipes[0]);
      delay(120);
      message = 0;
      bool ok = false;
      uint8_t i = MAX_TRY;
      while (!ok && i--)
      {
        printf("Try...\n\r");
        ok = radio.write( &message, sizeof(unsigned long) );
      }
      if (ok)
      {
        printf("ok\n\r");
      }
      else
      {
        printf("fail\n\r");
      }
      radio.startListening();
      delay(20);
    }
    if (strcmp(mystring, "H0")  == 0) 
    {
      radio.stopListening();
      printf("Turning off the LED 0...\n\r");
      radio.openWritingPipe(send_pipes[0]);
      delay(100);
      message = 1;
      bool ok = false;
      uint8_t i = MAX_TRY;
      while (!ok && i--)
      {
        printf("Try...\n\r");
        ok = radio.write( &message, sizeof(unsigned long) );
        ok = radio.write( &message, sizeof(unsigned long) );
      }
      if (ok)
      {
        printf("ok\n\r");
      }
      else
      {
        printf("fail\n\r");
      }
      radio.startListening();
      delay(20);
    }
    if (strcmp(mystring, "T0")  == 0) 
    {
      radio.stopListening();
      printf("Configuring the sensor 0...\n\r");
      radio.openWritingPipe(send_pipes[0]);
      delay(100);
      message = 2;
      bool ok = false;
      uint8_t i = MAX_TRY;
      while (!ok && i--)
      {
        printf("Try...\n\r");
        ok = radio.write( &message, sizeof(unsigned long) );
        ok = radio.write( &message, sizeof(unsigned long) );
        ok = radio.write( &message, sizeof(unsigned long) );
      }
      if (ok)
      {
        printf("ok\n\r");
      }
      else
      {
        printf("fail\n\r");
      }
      radio.startListening();
      delay(20);
    }
    if (strcmp(mystring, "S1")  == 0) 
    {
      radio.stopListening();
      printf("Turning on the LED 1...\n\r");
      radio.openWritingPipe(send_pipes[1]);
      delay(120);
      message = 0;
      bool ok = false;
      uint8_t i = MAX_TRY;
      while (!ok && i--)
      {
        printf("Try...\n\r");
        ok = radio.write( &message, sizeof(unsigned long) );
      }
      if (ok)
      {
        printf("ok\n\r");
      }
      else
      {
        printf("fail\n\r");
      }
      radio.startListening();
      delay(20);
    }
    if (strcmp(mystring, "H1")  == 0) 
    {
      radio.stopListening();
      printf("Turning off the LED 1...\n\r");
      radio.openWritingPipe(send_pipes[1]);
      delay(100);
      message = 1;
      bool ok = false;
      uint8_t i = MAX_TRY;
      while (!ok && i--)
      {
        printf("Try...\n\r");
        ok = radio.write( &message, sizeof(unsigned long) );
        ok = radio.write( &message, sizeof(unsigned long) );
      }
      if (ok)
      {
        printf("ok\n\r");
      }
      else
      {
        printf("fail\n\r");
      }
      radio.startListening();
      delay(20);
    }
    if (mystring[0] =='T') 
    {
      String newthreshold = String(mystring);
      newthreshold = newthreshold.substring(1);
      radio.stopListening();
      printf("Altering threshod sensor 1...\n\r");
      radio.openWritingPipe(send_pipes[1]);
      delay(100);
      message = newthreshold.toInt();
      bool ok = false;
      uint8_t i = MAX_TRY;
      while (!ok && i--)
      {
        printf("Try...\n\r");
        ok = radio.write( &message, sizeof(unsigned long) );
        ok = radio.write( &message, sizeof(unsigned long) );
      }
      if (ok)
      {
        printf("ok\n\r");
      }
      else
      {
        printf("fail\n\r");
      }
      radio.startListening();
      delay(20);
    }
  }
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
