//
//  constants.h
//  FinaleForm
//
//  Created by Daniel Levine on 5/5/21.
//

// Present constants are designed to work for TRANSFORM only.

#ifndef constants_h
#define constants_h

#include <string>

using namespace std;

// Serial Communication

// baud rate
#define SERIAL_BAUD_RATE 115200

// id numbers for message types sent over serial buses
#define TERM_ID_HEIGHT_SEND (unsigned char) 246 // send height data
#define TERM_ID_HEIGHT_RECEIVE (unsigned char) 253 // receive height data from display
#define TERM_ID_HEIGHT_SEND_AND_RECEIVE (unsigned char) 254 // send height data and request response
#define TERM_ID_GAIN_P (unsigned char) 247
#define TERM_ID_GAIN_I (unsigned char) 248
#define TERM_ID_MAX_I (unsigned char) 249
#define TERM_ID_DEAD_ZONE (unsigned char) 250
#define TERM_ID_GRAVITY_COMP (unsigned char) 251
#define TERM_ID_MAX_SPEED (unsigned char) 252

// frequency (in seconds) of resending pin config values
#define PIN_CONFIGS_RESET_FREQUENCY 1.5


// Transform specific constants

#define NUM_PINS_ARDUINO 6

#define SHAPE_DISPLAY_CAN_TALK_BACK 1

#endif /* constants_h */

