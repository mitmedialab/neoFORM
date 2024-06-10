//
//  DepthDebugApp.cpp
//  neoForm
//
//  Created by admin on 8/20/23.
//

#include "DepthDebugApp.hpp"

DepthDebugApp::DepthDebugApp(SerialShapeIOManager *theCustomShapeDisplayManager) : Application(theCustomShapeDisplayManager) {
    cout << "DepthDebugApp constructor" << endl;
}

void DepthDebugApp::update(float dt) {
    cout << "hello there debug depth";
    updateHeights();
}

void DepthDebugApp::updateHeights() {
    cout << "yo 'sup";
}

void DepthDebugApp::keyPressed(int key){
    key+key;
}

string DepthDebugApp::appInstructionsText(){
    return "Thou art a potato\n";
}

void DepthDebugApp::drawGraphicsForShapeDisplay(int x, int y, int width, int height){
   // color.setHsb(fmod(normalizedPhase * 180, 180), 255, 255);
   // ofSetColor(color);
    
    
    ofImage(heightsForShapeDisplay).draw(x, y, width, height);
   
    int w1 = 320;//640; //100
    int w2 = 160;//640; //200
    int w3 = 160;//640; //340
    
    
    ofImage imgZone1, imgZone3, imgZone5;
    
    imgZone1.allocate(w1, 480, OF_IMAGE_GRAYSCALE);
    imgZone3.allocate(w2, 480, OF_IMAGE_GRAYSCALE);
    imgZone5.allocate(w3, 480, OF_IMAGE_GRAYSCALE);
    
    ofPixels zone0, zone1, zone2, zone3, zone4, zone5, zone6;
    
    zone1.allocate(w1, 480, OF_IMAGE_GRAYSCALE);
    zone3.allocate(w2, 480, OF_IMAGE_GRAYSCALE);
    zone5.allocate(w3, 480, OF_IMAGE_GRAYSCALE);
    
    
    // Fill each of the zones with a solid color
    for (int y = 0; y < zone1.getHeight(); y++) {
        for (int x = 0; x < zone1.getWidth(); x++) {
            zone1.setColor(x, y, ofColor::mistyRose);
        }
    }
    
    for (int y = 0; y < zone3.getHeight(); y++) {
        for (int x = 0; x < zone3.getWidth(); x++) {
            zone3.setColor(x, y, ofColor::seaGreen);
        }
    }
    
    for (int y = 0; y < zone5.getHeight(); y++) {
        for (int x = 0; x < zone5.getWidth(); x++) {
            zone5.setColor(x, y, ofColor::blueSteel);
        }
    }
    
    imgZone1.setFromPixels(zone1);
    imgZone3.setFromPixels(zone3);
    imgZone5.setFromPixels(zone5);
    
    ofFbo DeadandAliveBlocks;
    DeadandAliveBlocks.allocate(640, 480, GL_RGB);
    //DeadandAliveBlocks.activateAllDrawBuffers();
    DeadandAliveBlocks.begin();
    
    imgZone1.draw(0,0);
    imgZone3.draw(w1, 0);
    imgZone5.draw(w1+w2, 0);
    
    DeadandAliveBlocks.end();
    
    ofPixels liveZonesPix;
    liveZonesPix.allocate(640, 480, OF_IMAGE_COLOR);
    
    DeadandAliveBlocks.readToPixels(liveZonesPix, 0);
    
    ofImage preview;
    preview.setFromPixels(liveZonesPix);
    preview.draw(0,0);
    
    
    //DeadandAliveBlocks.draw(0,0);

    DeadandAliveBlocks.clear();
    
   
    
    //imgZone1.draw(0,0);
    //imgZone3.draw(w1, 0);
    //imgZone5.draw(w2, 0);
    
    cout << 'hey theres a frame buffer yop yop yop';
    
    
}
