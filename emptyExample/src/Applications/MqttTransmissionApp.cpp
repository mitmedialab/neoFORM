//
//  MqttTransmissionApp.cpp
//  emptyExample
//
//  Created by Daniel Levine on 5/20/21.
//

#include "MqttTransmissionApp.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>


// zero matrix for reset
std::string doob = "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0";

// Helper functions for array piece

int theHeights[SHAPE_DISPLAY_SIZE_X*SHAPE_DISPLAY_SIZE_Y];
std::string theStr;
string stringArray[SHAPE_DISPLAY_SIZE_X*SHAPE_DISPLAY_SIZE_Y];

string int_array_to_string(int int_array[], int size_of_array) {
    theStr = "";
    for (int i=0;i<size_of_array; i++){
        theStr.push_back(int_array[i]+'0');
    }
    //std::cout<<str;
    return theStr;
}

std::vector<std::string> string_to_int_array(std::string text){
    istringstream iss(text);
    vector<string> tokens{istream_iterator<string>{iss},
        istream_iterator<string>{}};
    
    return tokens;
}

void updateINTHEIGHTS(std::vector<std::string> puma){
    for (int i=0; i<1152; i++){
        //printf("%s",puma.at(i).c_str());
        theHeights[i] = stoi(puma.at(i));
    }
}

void MqttTransmissionApp::updateHeights() {
    //depression = touchDetector->significantDepressionAmidstStabilityPixels();

    // here's a bunch of math to produce a nicely-proportioned wave from the
    // available parameters.
    //
    // sinc math function sin(x+k)/x where:
    //   x is normalized distance from display center plus an offset
    //   k is a time-varying phase
    //   heights are arranged to produce values within the valid output range
    //   the math has been arranged to easily tune how many wave crests you get
    // this is the line that sets a pin's height
    //touchDetector->update(<#const ofPixels &heightsForShapeDisplay#>, <#const ofPixels &heightsFromShapeDisplay#>)
    
    if (string_to_int_array(doob).size() != 1152){
        printf ("ERROR!");
        printf("%i\n",string_to_int_array(doob).size());
        for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {
            for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
                int xy = heightsForShapeDisplay.getPixelIndex(x, y);
                heightsForShapeDisplay[xy] = 30;
            }
        }
        
    }else{
         updateINTHEIGHTS(string_to_int_array(doob));
        for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {
            for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
                int xy = heightsForShapeDisplay.getPixelIndex(x, y);
                
              //  if (theHeights[xy] > 193){
              //      theHeights[xy] = 170;
              //  }
              
                heightsForShapeDisplay[xy] = theHeights[xy]+30;//heightScalar * height + heightOffset;
                
               
              
                //if {//(heightsForShapeDisplay[xy] != theHeights[xy]){//(depression.getColor(x, y).r != 0) {
                    //heightsForShapeDisplay[xy] = HEIGHT_MAX;
               // }
         //   }
        }
        
      /*  for (int o = 0; o < touchDetector->significantDepressionPixels().size(); o++){
            printf("%s\n",touchDetector->significantDepressionPixels()[o].getData());
        }*/
        
    }
        
}
    
}

// end helper functions

// Transform App Functions
void MqttTransmissionApp::update(float dt){
    if (!(are_we_setup)){
        setup();
        cout << "THIS IS IT\n";
        are_we_setup = 1;
    }else{
        //cout << "trying to update \n";
        
        // update heights on shape display
       updateHeights();
        
        // update mqtt transmission
       update();
    }
}

void MqttTransmissionApp::drawGraphicsForShapeDisplay(int x, int y, int width, int height){
    //color.setHsb(fmod(normalizedPhase * 180, 180), 255, 255);
    //ofSetColor(color);
    //ofImage(heightsForShapeDisplay).draw(x, y, width, height);
}

string MqttTransmissionApp::appInstructionsText(){
    return "YOOO";
}

//MQTT Functions
void MqttTransmissionApp::setup(){
    std::cout << "We've begin the setup\n";
    //client.begin("public.cloud.shiftr.io", 1883);
    //client.connect("hi this is dan","public", "public");
    client.begin("167.172.237.93", 1883);
    client.connect("doob");
    
    ofAddListener(client.onOnline, this, &MqttTransmissionApp::onOnline);
    ofAddListener(client.onOffline, this, &MqttTransmissionApp::onOffline);
    ofAddListener(client.onMessage, this, &MqttTransmissionApp::onMessage);
    
}

void MqttTransmissionApp::update(){
    client.update();
}

void MqttTransmissionApp::exit(){
    
}

void MqttTransmissionApp::onOnline(){
    client.subscribe("toTransform.data");
}

void MqttTransmissionApp::onOffline(){
    
}

void MqttTransmissionApp::onMessage(ofxMQTTMessage &msg){
    //ofLog() << "message: " << msg.topic << " - " << msg.payload;
    //cout << "WE GOT MAIL\n" << msg.topic << msg.payload << "\n";
    doob = msg.payload;
}

void MqttTransmissionApp::keyPressed(int key){
  client.publish("hello", "world");
}
