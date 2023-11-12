//
//  AppManager.cpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#include "AppManager.hpp"

void AppManager::setup(){
    
    ofSetFrameRate(30);

    printf("Setting up Shape Display\n");

    
    // initialize shape display and set up helper objects
    setupShapeDisplayManagement();
    
    // setup external devices (e.g., kinect)
    //kinectManager = new KinectManager();
    // Depth thresholds for the kinect are set here.
    kinectManager = new KinectManager(230, 70, 20);

    
    // zero timeOfLastUpdate tracker
    timeOfLastUpdate = elapsedTimeInSeconds();
    
    // set up applications
    mqttApp = new MqttTransmissionApp();
    applications["mqttTransmission"] = mqttApp;
    
    videoPlayerApp = new VideoPlayerApp();
    applications["videoPlayer"] = videoPlayerApp;
    videoPlayerApp->setup();
    
    // set up debugging application
    // and the debugging apps, too
    axisCheckerApp = new AxisCheckerApp();
    applications["axisChecker"] = axisCheckerApp;
    
    kinectDebugApp = new KinectDebugApp(kinectManager);
    applications["kinectDebug"] = kinectDebugApp;
    
    depthDebugApp = new DepthDebugApp();
    applications["depthDebug"] = depthDebugApp;
    
    // give applications read access to input data
    for (map<string, Application *>::iterator iter = applications.begin(); iter != applications.end(); iter++) {
        Application *app = iter->second;

        // shape display heights, if they are accessible
        if (shapeIOManager->heightsFromShapeDisplayAvailable) {
            app->setHeightsFromShapeDisplayRef(&heightPixelsFromShapeDisplay);
        }
    }
    
    // set default application
    setCurrentApplication("mqttTransmission");
}

// initialize the shape display and set up shape display helper objects
void AppManager::setupShapeDisplayManagement() {
    // initialize communication with the shape display
    shapeIOManager = new TransformIOManager();
    
    printf("Setting up Shape Display Management\n");


    // initialize shape display pin configs
    PinConfigs pinConfigs;
    pinConfigs.timeOfUpdate = elapsedTimeInSeconds();
    pinConfigs.gainP = DEFAULT_GAIN_P;
    pinConfigs.gainI = DEFAULT_GAIN_I;
    pinConfigs.maxI = DEFAULT_MAX_I;
    pinConfigs.deadZone = DEFAULT_DEAD_ZONE;
    pinConfigs.maxSpeed = DEFAULT_MAX_SPEED;
    shapeIOManager->setGlobalPinConfigs(pinConfigs);
    timeOfLastPinConfigsUpdate = elapsedTimeInSeconds();

    // clear height and pin config buffers
    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {
        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
            heightsForShapeDisplay[x][y] = 0;
            heightsFromShapeDisplay[x][y] = 0;
            pinConfigsForShapeDisplay[x][y] = pinConfigs;
        }
    }

    // allocate height pixels objects and clear contents
    heightPixelsForShapeDisplay.allocate(SHAPE_DISPLAY_SIZE_X, SHAPE_DISPLAY_SIZE_Y, 1);
    heightPixelsForShapeDisplay.set(0);
    heightPixelsFromShapeDisplay.allocate(SHAPE_DISPLAY_SIZE_X, SHAPE_DISPLAY_SIZE_Y, 1);
    heightPixelsFromShapeDisplay.set(0);

    // allocate shape display graphics container and clear contents
   graphicsForShapeDisplay.allocate(600, 800, GL_RGBA);
    graphicsForShapeDisplay.begin();
    ofClear(0);
    graphicsForShapeDisplay.end();
}

void AppManager::update(){
    
    //cout << "Update in App Manager\n";
    
    // time elapsed since last update
    float currentTime = elapsedTimeInSeconds();
    double dt = currentTime - timeOfLastUpdate;
    timeOfLastUpdate = currentTime;

    // copy heights from shape display to pixels object
    if (shapeIOManager->heightsFromShapeDisplayAvailable) {
        shapeIOManager->getHeightsFromShapeDisplay(heightsFromShapeDisplay);

        // note: manually looping over all pixels is important! the underlying
        // ofPixels char array is stored as unsigned char[y][x], while the
        // shape display heights are stored as unsigned char[x][y]
        for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {
            for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
                int xy = heightPixelsFromShapeDisplay.getPixelIndex(x, y);
                heightPixelsFromShapeDisplay[xy] = heightsFromShapeDisplay[x][y];
            }
        }
    }

    // copy heights and pin configs from application
    bool pinConfigsAreStale;
    if (!paused) {
        currentApplication->update(dt);
        currentApplication->getHeightsForShapeDisplay(heightPixelsForShapeDisplay);

        // note: manually looping over all pixels is important! the underlying
        // ofPixels char array is stored as unsigned char[y][x], while the
        // shape display heights are stored as unsigned char[x][y]
        for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {
            for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
                int xy = heightPixelsForShapeDisplay.getPixelIndex(x, y);
                heightsForShapeDisplay[x][y] = heightPixelsForShapeDisplay[xy];
            }
        }

        pinConfigsAreStale = timeOfLastPinConfigsUpdate < currentApplication->timeOfLastPinConfigsUpdate;
        if (pinConfigsAreStale) {
            currentApplication->getPinConfigsForShapeDisplay(pinConfigsForShapeDisplay);
        }
    }

    // Render the shape preview from the app into the graphicsForShapeDisplay frame buffer.
   
    graphicsForShapeDisplay.begin();
    ofBackground(0);
    ofSetColor(255);
    currentApplication->drawGraphicsForShapeDisplay(0, 0, 600, 800);
    graphicsForShapeDisplay.end();
    
    shapeIOManager->sendHeightsToShapeDisplay(heightsForShapeDisplay);
    if (pinConfigsAreStale) {
        shapeIOManager->setPinConfigs(pinConfigsForShapeDisplay);
        timeOfLastPinConfigsUpdate = elapsedTimeInSeconds();
    }
}

void AppManager::draw(){
    ofBackground(0,0,0);
    ofSetColor(255);
    
    // draw shape and color I/O images

    ofDrawRectangle(1, 1, 302, 302);
    ofImage(heightPixelsFromShapeDisplay).draw(2, 2, 300, 300);
    
    ofDrawRectangle(305, 1, 302, 302);
    ofImage(heightPixelsForShapeDisplay).draw(306, 2, 300, 300);
    
    ofDrawRectangle(609, 1, 302, 302);
    graphicsForShapeDisplay.draw(610, 2, 300, 300);
    
    ofRect(913, 1, 302, 302);
    ofImage(colorPixels).draw(914, 2, 300, 300);

    // draw this app's debugging gui, if selected
    if (showDebugGui) {
        currentApplication->drawDebugGui(1, 305);
    }

    // draw text
    int menuLeftCoordinate = 21;
    int menuHeight = 350;
    string title = currentApplication->getName() + (showDebugGui ? " - Debug" : "");
    ofDrawBitmapString(title, menuLeftCoordinate, menuHeight);
    menuHeight += 30;
    ofDrawBitmapString((string) "  '?' : " + (showGlobalGuiInstructions ? "hide" : "show") + " instructions", menuLeftCoordinate, menuHeight);
    if (showGlobalGuiInstructions) {
        menuHeight += 20;
        ofDrawBitmapString((string) "  '1' - '9' : select application", menuLeftCoordinate, menuHeight);
        menuHeight += 20;
        ofDrawBitmapString((string) "  '.' : turn debug gui " + (showDebugGui ? "off" : "on"), menuLeftCoordinate, menuHeight);
        menuHeight += 20;
        ofDrawBitmapString((string) "  ' ' : " + (paused ? "play" : "pause"), menuLeftCoordinate, menuHeight);
    }
    menuHeight += 30;

    // if there isn't already a debug gui, draw some more information
    if (!showDebugGui || currentApplication == applications["water"] || currentApplication == applications["stretchy"]) {
        ofRect(913, 305, 302, 302);
        ofImage(depthPixels).draw(914, 306, 300, 300);

        ofDrawBitmapString(currentApplication->appInstructionsText(), menuLeftCoordinate, menuHeight);
        menuHeight += 20;
    }
    
}


void AppManager::setCurrentApplication(string appName) {
    if (applications.find(appName) == applications.end()) {
        throw "no application exists with name " + appName;
    }

    currentApplication = applications[appName];
    updateDepthInputBoundaries();
}

void AppManager::updateDepthInputBoundaries() {
    pair<int, int> boundaries = currentApplication->getDepthInputBoundaries();
    int near = boundaries.first;
    int far = boundaries.second;

    //no more kinect. this function is useless
}

void AppManager::exit() {
    // delete shapeIOManager to shut down the shape display
    delete shapeIOManager;
}

// handle key presses. keys unused by app manager are forwarded to the current
// application.
void AppManager::keyPressed(int key) {
    // keys used by app manager must be registered as reserved keys
    const int reservedKeysLength = 13;
    const int reservedKeys[reservedKeysLength] = {
        '/', '?', '.', ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    };
    const int *reservedKeysEnd = reservedKeys + reservedKeysLength;

    // key press events handled by the application manager
    if (find(reservedKeys, reservedKeysEnd, key) != reservedKeysEnd) {
        if (key == '/' || key == '?') {
            showGlobalGuiInstructions = !showGlobalGuiInstructions;
        } else if (key == '.') {
            showDebugGui = !showDebugGui;
        } else if (key == ' ') {
            paused = !paused;
        } else if (key == '1') {
            setCurrentApplication("mqttTransmission");
        } else if (key == '2') {
            setCurrentApplication("axisChecker");
        } else if (key == '3') {
            setCurrentApplication("videoPlayer");
        } else if (key == '4') {
            setCurrentApplication("kinectDebug");
        } else if (key == '5') {
            setCurrentApplication("depthDebug");
        }

    // forward unreserved keys to the application
    } else {
        currentApplication->keyPressed(key);
    }
}

void AppManager::keyReleased(int key) {};
void AppManager::mouseMoved(int x, int y) {};
void AppManager::mouseDragged(int x, int y, int button) {};
void AppManager::mousePressed(int x, int y, int button) {};
void AppManager::mouseReleased(int x, int y, int button) {};
void AppManager::windowResized(int w, int h) {};
void AppManager::gotMessage(ofMessage msg) {};
void AppManager::dragEvent(ofDragInfo dragInfo) {};
void AppManager::mouseEntered(int x, int y){};
void AppManager::mouseExited(int x, int y){};
