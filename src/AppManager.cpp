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
    
    kinectManager = new KinectManager(255, 90, 20);

    
    // zero timeOfLastUpdate tracker
    timeOfLastUpdate = elapsedTimeInSeconds();
    
    // set up applications
    mqttApp = new MqttTransmissionApp(m_serialShapeIOManager);
    applications["mqttTransmission"] = mqttApp;
    
    videoPlayerApp = new VideoPlayerApp(m_serialShapeIOManager);
    applications["videoPlayer"] = videoPlayerApp;
    videoPlayerApp->setup();
    
    // set up debugging application
    // and the debugging apps, too
    axisCheckerApp = new AxisCheckerApp(m_serialShapeIOManager);
    applications["axisChecker"] = axisCheckerApp;
    
    //breaks on newer openframeworks (and is no longer needed)
    //kinectDebugApp = new KinectDebugApp(m_serialShapeIOManager, kinectManager);
    //applications["kinectDebug"] = kinectDebugApp;
    
    depthDebugApp = new DepthDebugApp(m_serialShapeIOManager);
    applications["depthDebug"] = depthDebugApp;
    
    kinectHandWavy = new KinectHandWavy(m_serialShapeIOManager,kinectManager);
    applications["kinectHandWavy"] = kinectHandWavy;
    
    // give applications read access to input data
    for (map<string, Application *>::iterator iter = applications.begin(); iter != applications.end(); iter++) {
        Application *app = iter->second;

        /* This is deprecated and should be removed */
        /* The apps have their own reference to the shape IO manager and can get the heights from the boards themselves, they don't need the app manager to do it for them. */
        // shape display heights, if they are accessible
        //if (m_serialShapeIOManager->heightsFromShapeDisplayAvailable) {
        //    app->setHeightsFromShapeDisplayRef(&heightPixelsFromShapeDisplay);
        //}
        /* End deprecated */
        
    }
    
    // set default application
    setCurrentApplication("mqttTransmission");
}

// initialize the shape display and set up shape display helper objects
void AppManager::setupShapeDisplayManagement() {
    // initialize communication with the shape display
    // This is where the particulars of the shape display are set (i.e. TRANSFORM, inFORM, or any other physical layout).
    string shapeDisplayToUse = "inFORM";
    
    if (shapeDisplayToUse == "TRANSFORM") {
        m_serialShapeIOManager = new TransformIOManager(kinectManager);
    } else if (shapeDisplayToUse == "inFORM") {
        m_serialShapeIOManager = new InFormIOManager(kinectManager);
    } else {
        throw "unknown shape display type: " + shapeDisplayToUse;
    }
    
    printf("Setting up Shape Display Management\n");

    // Size the pin arrays correctly based on the hardware specific dimension, and initialize them with zero values
    heightsForShapeDisplay = std::vector<std::vector<unsigned char>>(m_serialShapeIOManager->shapeDisplaySizeX, std::vector<unsigned char>(m_serialShapeIOManager->shapeDisplaySizeY));
    heightsFromShapeDisplay = std::vector<std::vector<unsigned char>>(m_serialShapeIOManager->shapeDisplaySizeX, std::vector<unsigned char>(m_serialShapeIOManager->shapeDisplaySizeY));

    // initialize shape display pin configs
    PinConfigs pinConfigs;
    pinConfigs.timeOfUpdate = elapsedTimeInSeconds();
    pinConfigs.gainP = m_serialShapeIOManager->getGainP();
    pinConfigs.gainI = m_serialShapeIOManager->getGainI();
    pinConfigs.maxI  = m_serialShapeIOManager->getMaxI();
    pinConfigs.deadZone = m_serialShapeIOManager->getDeadZone();
    pinConfigs.maxSpeed = m_serialShapeIOManager->getMaxSpeed();
    m_serialShapeIOManager->setGlobalPinConfigs(pinConfigs);
    timeOfLastPinConfigsUpdate = elapsedTimeInSeconds();

    
    // Set the dimensions of the pinConfigs
    pinConfigsForShapeDisplay.resize(m_serialShapeIOManager->shapeDisplaySizeX, std::vector<PinConfigs>(m_serialShapeIOManager->shapeDisplaySizeY, pinConfigs));

    // allocate height pixels objects and clear contents
    heightPixelsForShapeDisplay.allocate(m_serialShapeIOManager->shapeDisplaySizeX, m_serialShapeIOManager->shapeDisplaySizeY, 1);
    heightPixelsForShapeDisplay.set(0);

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

    // copy heights and pin configs from application
    bool pinConfigsAreStale;
    if (!paused) {
        currentApplication->update(dt);
        currentApplication->getHeightsForShapeDisplay(heightPixelsForShapeDisplay);

        // note: manually looping over all pixels is important! the underlying
        // ofPixels char array is stored as unsigned char[y][x], while the
        // shape display heights are stored as unsigned char[x][y]
        for (int x = 0; x < m_serialShapeIOManager->shapeDisplaySizeX; x++) {
            for (int y = 0; y <  m_serialShapeIOManager->shapeDisplaySizeY; y++) {
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
    
    m_serialShapeIOManager->sendHeightsToShapeDisplay(heightsForShapeDisplay);
    if (pinConfigsAreStale) {
        m_serialShapeIOManager->setPinConfigs(pinConfigsForShapeDisplay);
        timeOfLastPinConfigsUpdate = elapsedTimeInSeconds();
    }
}

// Takes a 2D vector of heights and converts it to an ofPixels object
// ofPixels stores pixel data as a 1d flat array of chars, so the 2d vectors need to be iteratively flattened into a 1d array.
ofPixels AppManager::convertHeightsToPixels(const std::vector<std::vector<unsigned char>>& heights) {
    // Create and allocate a local ofPixels object of the right size.
    ofPixels pixels;
    pixels.allocate(m_serialShapeIOManager->shapeDisplaySizeX, m_serialShapeIOManager->shapeDisplaySizeY, OF_PIXELS_GRAY);
    
    // Loop over the 2D vector of heights.
    for (int x = 0; x < m_serialShapeIOManager->shapeDisplaySizeX; x++) {
        for (int y = 0; y < m_serialShapeIOManager->shapeDisplaySizeY; y++) {
            // ofPixels are stored as a 1D array, so we need to convert the 2D index to a 1D index.
            int index = static_cast<int>(pixels.getPixelIndex(x, y));
            
            // Set the corresponding pixel in the ofPixels object to the height value.
            pixels[index] = heights[x][y];
        }
    }

    return pixels;
}

void AppManager::draw(){
    ofBackground(0,0,0);
    ofSetColor(255);
    
    // draw shape and color I/O images

    /* Draw the height data being returned for the pin heights by the arduinos */
    ofDrawRectangle(1, 1, 302, 302);
    if (m_serialShapeIOManager->heightsFromShapeDisplayAvailable) {
        // Make a reference to the heights from the boards, this is memory safe because it doesn't copy the data.
        const auto& heightsFromBoards = m_serialShapeIOManager->getHeightsFromShapeDisplay();
        
        // Convert the heights to pixels and draw them with an ofImage
        ofPixels pixelsFromBoards = convertHeightsToPixels(heightsFromBoards);
        ofImage(pixelsFromBoards).draw(2, 2, 300, 300);
    }
    
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
    // delete m_serialShapeIOManager to shut down the shape display
    delete m_serialShapeIOManager;
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
            setCurrentApplication("kinectHandWavy");
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
