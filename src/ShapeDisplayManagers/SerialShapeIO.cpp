//
//  SerialShapeIO.cpp
//  FinaleForm
//
//  Created by Daniel Levine on 5/5/21.
//

#include "SerialShapeIO.hpp"

SerialShapeIO::SerialShapeIO(string portName, int baudRate, bool readable) : readable(readable) {
    serial.enumerateDevices();
    bool connectionSuccessful = serial.setup(portName, baudRate);
    if (connectionSuccessful) {
        start();
    }
}

SerialShapeIO::~SerialShapeIO() {
    stop();
    serial.close();
}

void SerialShapeIO::start() {
    startThread();   // blocking, verbose
}

void SerialShapeIO::stop() {
    waitForThread(true);
}

void SerialShapeIO::threadedFunction() {
    ofSleepMillis(500);
    
    unsigned char messageContent[MSG_SIZE_SEND];
    unsigned char longMessageContent[MSG_SIZE_SEND_AND_RECEIVE];
    MessageReceive newReceivedMessage;
    
    int currentReceivedByte = 0;
    int receiveCounter = 0;
    
    while (isThreadRunning() != 0) {
        // send the messages in the short message buffer
        bool sendShortMessage = false;
        lock();
        if (sendBuffer.size() > 0) { // if there is an element in my buffer
            for (int i = 0; i < MSG_SIZE_SEND; i++) { // copy the content
                messageContent[i] = sendBuffer.front().messageContent[i];
            }
            sendBuffer.erase(sendBuffer.begin());
            sendShortMessage = true;
        }
        unlock();
        if (sendShortMessage) {
            serial.writeBytes(messageContent, MSG_SIZE_SEND);
        }

        // if shape display is write-only, the loop body stops here
        if (!readable) {
            // make sure we're giving the serial port a break
            if (!sendShortMessage) {
                ofSleepMillis(4);
            }
            continue;
        }

        // send the messages in the long message buffer (the one where we request feedback)
        bool sendLongMessage = false;
        lock();
        if (sendBufferMessageWithFeedback.size() > 0) { // if there is an element in my buffer
            for (int i = 0; i < MSG_SIZE_SEND_AND_RECEIVE; i++) { // copy the content
                longMessageContent[i] = sendBufferMessageWithFeedback.front().messageContent[i];
            }
            sendBufferMessageWithFeedback.erase(sendBufferMessageWithFeedback.begin());
            sendLongMessage = true;
        }
        unlock();
        if (sendLongMessage) {
            serial.writeBytes(longMessageContent, MSG_SIZE_SEND_AND_RECEIVE);
        } else {
            // pause is dependent on the speed of the machine right now, so that is a problem.
            
            //            usleep(500);
            // usleep symbol isn't found
            // and according to http://stackoverflow.com/questions/14340485/getting-stuck-in-usleep1
            // usleep is deprecated
            // so instead use nanosleep
            nanosleep((struct timespec[]){{0, 50000000}}, NULL);
        }
        
        // receive messages from the table if there are any
        while (serial.available()) {
            bool receivedMessage = false;
            currentReceivedByte = serial.readByte();
            if (currentReceivedByte == TERM_ID_HEIGHT_RECEIVE) {
                receiveCounter = 1;
                newReceivedMessage.messageContent[0] = currentReceivedByte;
            } else {
                newReceivedMessage.messageContent[receiveCounter] = currentReceivedByte;
                receiveCounter++;
                if (receiveCounter >= 8) {
                    receiveCounter = 0;
                    receivedMessage = true;
                    lock();
                    receiveBuffer.push_back(newReceivedMessage);
                    unlock();
                }
            }
        }
    }
}

void SerialShapeIO::writeMessage(unsigned char messageContent[MSG_SIZE_SEND]) {
    if (isThreadRunning()) {
        MessageSend newMessage;
        for (int i = 0; i < MSG_SIZE_SEND; i++) {
            newMessage.messageContent[i] = messageContent[i];
        }
        lock();
        sendBuffer.push_back(newMessage);
        unlock();
    }
}

void SerialShapeIO::writeMessageRequestFeedback(unsigned char messageContent[MSG_SIZE_SEND_AND_RECEIVE]) {
    if (isThreadRunning()) {
        MessageSendAndReceive newMessage;
        for (int i = 0; i < MSG_SIZE_SEND_AND_RECEIVE; i++) {
            newMessage.messageContent[i] = messageContent[i];
        }
        lock();
        sendBufferMessageWithFeedback.push_back(newMessage);
        unlock();
    }
}

bool SerialShapeIO::hasNewMessage() {
    bool newMessage = false;
    if (isThreadRunning()) {
        lock();
        if (receiveBuffer.size() > 0) {
            newMessage = true;
        }
        unlock();
    }
    return newMessage;
}

bool SerialShapeIO::readMessage(unsigned char messageContent[MSG_SIZE_RECEIVE]) {
    bool newMessageReceived = false;
    if (isThreadRunning()) {
        lock();
        if (receiveBuffer.size() > 0) { // if there is an element in my buffer
            for (int i = 0; i < MSG_SIZE_RECEIVE; i++) { // copy the content
                messageContent[i] = receiveBuffer.front().messageContent[i];
            }
            receiveBuffer.erase(receiveBuffer.begin());
            newMessageReceived = true;
        }
        unlock();
    }
    return newMessageReceived;
}

