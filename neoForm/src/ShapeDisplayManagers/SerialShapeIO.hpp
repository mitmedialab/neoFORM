//
//  SerialShapeIO.hpp
//  FinaleForm
//
//  Created by Daniel Levine on 5/5/21.
//

#ifndef SerialShapeIO_hpp
#define SerialShapeIO_hpp

#include <stdio.h>
#include "ofThread.h"
#include "ofSerial.h" // is this a real thing?
#include "constants.h"
#include "ofUtils.h"


// There are three types of messages: One where we only send 8 bytes to the
// boards to set a parameter unidirectionally (PID terms, target positions).
// One where we send 10 bytes, to set a target position and receive back the
// current actuator position. And one where we send 8 bytes, to read a
// parameter.
#define MSG_SIZE_SEND 8
#define MSG_SIZE_SEND_AND_RECEIVE 10
#define MSG_SIZE_RECEIVE 8


struct MessageSend {
    unsigned char messageContent[MSG_SIZE_SEND];
};

struct MessageReceive {
    unsigned char messageContent[MSG_SIZE_RECEIVE];
};

struct MessageSendAndReceive {
    unsigned char messageContent[MSG_SIZE_SEND_AND_RECEIVE];
};


class SerialShapeIO: public ofThread {
public:
    SerialShapeIO(string portName, int baudRate, bool readable);
    ~SerialShapeIO();
    
    void writeMessage(unsigned char messageContent[MSG_SIZE_SEND]);

    // these methods depend on the serial connection being readable
    bool hasNewMessage();
    bool readMessage(unsigned char messageContent[MSG_SIZE_RECEIVE]);
    void writeMessageRequestFeedback(unsigned char messageContent[MSG_SIZE_SEND_AND_RECEIVE]);

    bool readable;

private:
    void start();
    void stop();
    void threadedFunction();

    ofSerial serial;

    vector<MessageSend> sendBuffer;
    vector<MessageReceive> receiveBuffer;
    vector<MessageSendAndReceive> sendBufferMessageWithFeedback;
    
};

#endif /* SerialShapeIO_hpp */

