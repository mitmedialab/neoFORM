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
    sendBufferAvailible = true;
    sendBufferWithFeedbackAvailible = true;
    receiveBufferAvailible = true;
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
    ofSleepMillis(50);

    unsigned char messageContent[MSG_SIZE_SEND];
    unsigned char longMessageContent[MSG_SIZE_SEND_AND_RECEIVE];
    MessageReceive newReceivedMessage;

    int currentReceivedByte = 0;
    int receiveCounter = 0;

    while (isThreadRunning() != 0) {
		size_t numToCopy;
        // send the messages in the short message buffer
        bool sendShortMessage = false;

		// waits for sendBufferMessageWithFeedback to become available
        while(sendBufferAvailible.exchange(false) == false) {
            timespec smallTimespan[] = {{0, 50}};
            nanosleep(smallTimespan, NULL);
		}
        if (sendBuffer.size() > 0) { // if there is an element in my buffer
            for (int i = 0; i < MSG_SIZE_SEND; i++) { // copy the content
                messageContent[i] = sendBuffer.front().messageContent[i];
            }
            sendBuffer.erase(sendBuffer.begin());
            sendShortMessage = true;
        }
		sendBufferAvailible = true;
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

		// waits for sendBufferMessageWithFeedback to become available
        while(sendBufferWithFeedbackAvailible.exchange(false) == false) {
            timespec smallTimespan[] = {{0, 50}};
            nanosleep(smallTimespan, NULL);
		}
        if (sendBufferMessageWithFeedback.size() > 0) { // if there is an element in my buffer
            for (int i = 0; i < MSG_SIZE_SEND_AND_RECEIVE; i++) { // copy the content
                longMessageContent[i] = sendBufferMessageWithFeedback.front().messageContent[i];
            }
            sendBufferMessageWithFeedback.erase(sendBufferMessageWithFeedback.begin());
            sendLongMessage = true;
        }
		sendBufferWithFeedbackAvailible = true;
        if (sendLongMessage) {
            serial.writeBytes(longMessageContent, MSG_SIZE_SEND_AND_RECEIVE);
        } else {
            // pause is dependent on the speed of the machine right now, so that is a problem.

            //            usleep(500);
            // usleep symbol isn't found
            // and according to http://stackoverflow.com/questions/14340485/getting-stuck-in-usleep1
            // usleep is deprecated
            // so instead use nanosleep
            timespec twentieth_of_second[] = {{0, 50000000}};
            nanosleep(twentieth_of_second, NULL);
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
                    //lock();
        			while(receiveBufferAvailible.exchange(false) == false) {
        			    timespec smallTimespan[] = {{0, 50}};
        			    nanosleep(smallTimespan, NULL);
					}
                    receiveBuffer.push_back(newReceivedMessage);
					receiveBufferAvailible = true;
                    //unlock();
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
        //lock();
        while(sendBufferAvailible.exchange(false) == false) {
            timespec smallTimespan[] = {{0, 50}};
            nanosleep(smallTimespan, NULL);
		}
        sendBuffer.push_back(newMessage);
        //unlock();
		sendBufferAvailible = true;
    }
}

void SerialShapeIO::writeMessageRequestFeedback(unsigned char messageContent[MSG_SIZE_SEND_AND_RECEIVE]) {
    if (isThreadRunning()) {
        MessageSendAndReceive newMessage;
        for (int i = 0; i < MSG_SIZE_SEND_AND_RECEIVE; i++) {
            newMessage.messageContent[i] = messageContent[i];
        }
        //lock();
        while(sendBufferWithFeedbackAvailible.exchange(false) == false) {
            timespec smallTimespan[] = {{0, 50}};
            nanosleep(smallTimespan, NULL);
		}
        sendBufferMessageWithFeedback.push_back(newMessage);
        //unlock();
		sendBufferWithFeedbackAvailible = true;
    }
}

bool SerialShapeIO::hasNewMessage() {
    bool newMessage = false;
    if (isThreadRunning()) {
        //lock();
        while(receiveBufferAvailible.exchange(false) == false) {
            timespec smallTimespan[] = {{0, 50}};
            nanosleep(smallTimespan, NULL);
		}
        if (receiveBuffer.size() > 0) {
            newMessage = true;
        }
        //unlock();
		receiveBufferAvailible = true;
    }
    return newMessage;
}

bool SerialShapeIO::readMessage(unsigned char messageContent[MSG_SIZE_RECEIVE]) {
    bool newMessageReceived = false;
    if (isThreadRunning()) {
        //lock();
        while(receiveBufferAvailible.exchange(false) == false) {
            timespec smallTimespan[] = {{0, 50}};
            nanosleep(smallTimespan, NULL);
		}
        if (receiveBuffer.size() > 0) { // if there is an element in my buffer
            for (int i = 0; i < MSG_SIZE_RECEIVE; i++) { // copy the content
                messageContent[i] = receiveBuffer.front().messageContent[i];
            }
            receiveBuffer.erase(receiveBuffer.begin());
            newMessageReceived = true;
        }
        //unlock();
		receiveBufferAvailible = true;
    }
    return newMessageReceived;
}

