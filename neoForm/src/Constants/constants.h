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

#define NUM_ARDUINOS 192
#define NUM_PINS_ARDUINO 6

#define NUM_SERIAL_CONNECTIONS 6
#define SERIAL_PORT_0 "/dev/tty.usbserial-A702YMNV"
#define SERIAL_PORT_1 "/dev/tty.usbserial-A702YLM2"
#define SERIAL_PORT_2 "/dev/tty.usbserial-A702YMNT"
#define SERIAL_PORT_3 "/dev/tty.usbserial-A702YLM6"
#define SERIAL_PORT_4 "/dev/tty.usbserial-A702YLM9"
#define SERIAL_PORT_5 "/dev/tty.usbserial-A30011Hp"



const string SERIAL_PORTS[NUM_SERIAL_CONNECTIONS] = {
    SERIAL_PORT_0,
    SERIAL_PORT_1,
    SERIAL_PORT_2,
    SERIAL_PORT_3,
    SERIAL_PORT_4,
    SERIAL_PORT_5
};

// Shape display table
// -------------------
#define    SHAPE_DISPLAY_SIZE_X 48
#define    SHAPE_DISPLAY_SIZE_Y 24

#define SHAPE_DISPLAY_CAN_TALK_BACK 1

#define HEIGHT_MIN 50
#define HEIGHT_MAX 210

#define PINBLOCK_0_X_OFFSET 13
#define PINBLOCK_0_WIDTH 16

#define PINBLOCK_1_X_OFFSET 43
#define PINBLOCK_1_WIDTH 16

#define PINBLOCK_2_X_OFFSET 73
#define PINBLOCK_2_WIDTH 16

// shape display derived dimensions, for convenience
#define SHAPE_DISPLAY_SIZE_2D (SHAPE_DISPLAY_SIZE_X * SHAPE_DISPLAY_SIZE_Y)
#define HEIGHT_RANGE (HEIGHT_MAX - HEIGHT_MIN)

// TRANSFORM pin config defaults
#define DEFAULT_GAIN_P 1.5
#define DEFAULT_GAIN_I 0.045
#define DEFAULT_MAX_I 25
#define DEFAULT_DEAD_ZONE 2
#define DEFAULT_MAX_SPEED 200


#endif /* constants_h */

