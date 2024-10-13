//
//  AppManager.hpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#ifndef AppManager_hpp
#define AppManager_hpp

#include <stdio.h>

// basics
#include "ofMain.h"
#include "constants.h"
#include "ofParameter.h"
#include "utils.hpp"
#include "ofxGui.h"

// shape display managers
#include "PinConfigs.h"
#include "ShapeIOManager.hpp"
#include "TransformIOManager.hpp"

// External Device Managers
#include "KinectManager.hpp"

// major classes
#include "Application.hpp"

// debugging applications
#include "AxisCheckerApp.hpp"

#include "KinectDebugApp.hpp"

// mqtt application
#include "MqttTransmissionApp.hpp"

// static applications
#include "VideoPlayerApp.hpp"

// Debugging app for the frame buffer and image slicing
#include "DepthDebugApp.hpp"

#include "KinectHandWavy.hpp"

class AppManager : public ofBaseApp {
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    
private:
    void setupShapeDisplayManagement();
    void setCurrentApplication(string appName);
    void updateDepthInputBoundaries();

    // interfaces to the peripherals
    SerialShapeIOManager *m_serialShapeIOManager;
    
    // external devices
    KinectManager *kinectManager;
    
    // applications
    map<string, Application *> applications;
    Application *currentApplication;
    
    // debugging applications
    AxisCheckerApp *axisCheckerApp;
    //KinectDebugApp *kinectDebugApp;
    DepthDebugApp *depthDebugApp;
    
    // mqtt application
    MqttTransmissionApp *mqttApp;
    
    // static applications
    VideoPlayerApp *videoPlayerApp;
    
    // hand wavy application
    KinectHandWavy *kinectHandWavy;
    
    // program state
    bool paused = false;
    double timeOfLastUpdate = -1;
    double timeOfLastPinConfigsUpdate = -1;

    // new gui
    ofxPanel gui;
    vector<ofxButton> modeButtons;
    vector<string> modeNames;

    // gui state
    bool showGlobalGuiInstructions = false;
    bool showDebugGui = true;
    
    // I/O data buffers

    // maybe rename to serialHeightOutput()
    std::vector<std::vector<unsigned char>> heightsForShapeDisplay;
    // maybe rename to serialHeightInput()
    std::vector<std::vector<unsigned char>> heightsFromShapeDisplay;
    ofPixels heightPixelsForShapeDisplay;

    std::vector<std::vector<PinConfigs>> pinConfigsForShapeDisplay;
    ofFbo graphicsForShapeDisplay;
    ofPixels colorPixels;
    //ofPixels depthPixels;
    
    ofPixels convertHeightsToPixels(const std::vector<std::vector<unsigned char>>& heights);
};


#endif /* AppManager_hpp */
