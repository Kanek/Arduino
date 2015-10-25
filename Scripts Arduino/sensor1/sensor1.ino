#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

const uint8_t MAX_TRY = 5;

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

// Pins on the remote for buttons
const int sensor_pin = A0;
unsigned long sensor_state = 0;
unsigned long threshold = 200;
unsigned long active = 0;

const uint8_t led_pin = 3;

bool normal = true;
bool change = true;

//
// Topology
//

// Single radio pipe address for the 2 nodes to communicate.
const uint64_t send_pipe = 0xF0F0F0F0AA;
const uint64_t recieve_pipe = 0xF0F0F0F0A1;

//
// Setup
//

void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24/sensor1\n\r");

  //
  // Setup and configure rf radio
  //

  radio.begin();
  
  radio.openWritingPipe(send_pipe);

  radio.printDetails();

  pinMode(sensor_pin,INPUT);
  pinMode(led_pin,OUTPUT);
  digitalWrite(led_pin,LOW);
  
}

//
// Loop
//

void loop(void)
{
  //
  // Remote role.  If the state of any button has changed, send the whole state of
  // all buttons.
  //

  delay(20); 
  uint8_t state = analogRead(sensor_pin);
  if (abs(sensor_state - state) > 40 || change)
  {
    change = false;
    sensor_state = state;
    bool ok = false;
    uint8_t i = MAX_TRY;
    printf("Now sending...\n\r");
    while (!ok && i--)
    {
      printf("Try...\n\r");
      if (sensor_state < threshold)
      {
        active = 1;
      }
      else
      {
        active = 0;
      }
      ok = radio.write( &active, sizeof(unsigned long) );
    }
    if (ok)
    {
      printf("ok\n\r");
    }
    else
    {
      printf("fail\n\r");
    }
  }
  // Try again in a short while
  delay(20);

  radio.openReadingPipe(1,recieve_pipe);

  radio.startListening();

  delay(100);

  if ( radio.available() )
  {
      unsigned long message;
      bool done = false;
      while (!done)
      {
        done = radio.read( &message, sizeof(unsigned long) );
      }
      printf("Got order, value: %lu\n\r", message);
      switch (message)
      {
        case 0:
          digitalWrite(led_pin,HIGH);
          break;
        case 1:
          digitalWrite(led_pin,LOW);
          break;
        default:
          threshold = message;
          change = true;
          break;
      }
  }

  radio.stopListening();
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
