//
//  AppManager.cpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#include "AppManager.hpp"
#include "AmbientWave.hpp"
#include "Application.hpp"
#include "PropagationWave.hpp"
#include "KinectMaskMaker.hpp"
#include "TransitionApp.hpp"
#include "ofEvents.h"
#include "ofGraphics.h"
#include "ofxXmlSettings.h"
#include "utils.hpp"
#include <iterator>
#include "PinDisabler.hpp"

void AppManager::setup() {
	
	ofSetFrameRate(30);
	
	printf("Setting up Shape Display\n");
	
	// initialize shape display and set up helper objects
	setupShapeDisplayManagement();
	
	// setup external devices (e.g., kinect)
	// kinectManager = new KinectManager();
	// Depth thresholds for the kinect are set here.
	
	kinectManager = new KinectManagerSimple();
	
	// zero timeOfLastUpdate tracker
	timeOfLastUpdate = elapsedTimeInSeconds();
	
	// set up applications
	mqttApp = new MqttTransmissionApp(m_serialShapeIOManager);
	
	kinectMaskMaker = new KinectMaskMaker(m_serialShapeIOManager, kinectManager);
	
	videoPlayerApp = new VideoPlayerApp(m_serialShapeIOManager);
	videoPlayerApp->setup();
	
	// set up debugging application
	// and the debugging apps, too
	axisCheckerApp = new AxisCheckerApp(m_serialShapeIOManager);
	
	// breaks on newer openframeworks (and is no longer needed)
	// kinectDebugApp = new KinectDebugApp(m_serialShapeIOManager, kinectManager);
	// applications["kinectDebug"] = kinectDebugApp;
	
    // DepthDebug is not currently active.
	//depthDebugApp = new DepthDebugApp(m_serialShapeIOManager);
	//applications["depthDebug"] = depthDebugApp;
	
	kinectHandWavy = new KinectHandWavy(m_serialShapeIOManager, kinectManager);
    
    // Telepresence mode takes 16 bit thresholding values as parameters here.
    telepresence = new Telepresence(m_serialShapeIOManager, kinectManager,
                                 255 * 256, 140 * 256, cam);
	
	equationMode = new EquationMode(m_serialShapeIOManager);
	
	waveModeContours = new WaveModeContours(m_serialShapeIOManager, kinectManager);

	propagationWave = new PropagationWave(m_serialShapeIOManager);

	// not in applications list
	transitionApp = new TransitionApp(m_serialShapeIOManager);
	
	ambientWave = new AmbientWave(m_serialShapeIOManager);

	pinDisabler = new PinDisabler(m_serialShapeIOManager, 400, 356, 600, 600);

	if (m_serialShapeIOManager->getShapeDisplayName() == "inFORM") {
		applications.push_back(equationMode);
		applications.push_back(telepresence);
        applications.push_back(kinectHandWavy);
	} else if (m_serialShapeIOManager->getShapeDisplayName() == "TRANSFORM") {
		applications.push_back(videoPlayerApp);
		applications.push_back(waveModeContours);
		applications.push_back(ambientWave);
        applications.push_back(kinectHandWavy);
	}

	setCurrentApplication(mqttApp);

	debugApplications.push_back(pinDisabler);
	debugApplications.push_back(kinectMaskMaker);

	options.push_back(&autoTransition);
	optionNames.push_back("auto transition");

	// innitialize GUI
	debugGui.setup("debug modes:");
	debugGui.setPosition(5, 60 + 65 * (applications.size() + options.size()));
	
	// IMPORTANT: ofxGui uses raw pointers to ofxButton, so an automatic resize
	// of modeButtons will invalidate all existing pointers stored in gui.
	// DO NOT .push_back MORE THAN applications.size()!!!!
	debugModeButtons.reserve(debugApplications.size());

	int appIndex = 1; // Initialize an index for iteration

    // Iterate over the applicationOrder vector and add the corresponding app to the GUI
    for (Application* app : debugApplications) {
        debugModeButtons.push_back(ofxButton());
        
        // Construct the new button name with the index prepended
        std::string buttonName = std::to_string(appIndex) + ": " + app->getName();
        auto p_button = debugModeButtons.back().setup(buttonName);
        debugGui.add(p_button);
        
        appIndex++;
    }

	// Collapse the GUI panel for now to make room for the new graphical buttons.
	debugGui.minimize();
	
	// *** Rectangular button setup ***
    // Load a font for the button text.
    ofTrueTypeFont::setGlobalDpi(72);
    displayFont20.load("SourceSans3-Regular.ttf", 20);
    
    // Set up the buttons by creating an ofRectangle for each application
    for (int i = 0; i < applications.size(); i++){
        ofRectangle button;
        button.set(20, 60 + 65*i, 240, 50);
        applicationButtons.push_back(button);
    }

	// Set up the buttons based on options
	for (int i = 0; i < options.size(); i++) {
		ofRectangle button;
		int yOffset = 60 + 65 * (i + applications.size());
		button.set(20, yOffset, 240, 50);
		optionButtons.push_back(button);
	}

}

// initialize the shape display and set up shape display helper objects
void AppManager::setupShapeDisplayManagement() {
	// initialize communication with the shape display
	
	ofxXmlSettings settings;
	settings.load("settings.xml");
	string shapeDisplayToUse = settings.getValue("name", "inFORM");
	
	if (shapeDisplayToUse == "TRANSFORM") {
		m_serialShapeIOManager = new TransformIOManager(kinectManager);
	} else if (shapeDisplayToUse == "inFORM") {
		m_serialShapeIOManager = new InFormIOManager(kinectManager);
	} else {
		throw "unknown shape display type: " + shapeDisplayToUse;
	}
	
	printf("Setting up Shape Display Management\n");
	
	// Size the pin arrays correctly based on the hardware specific dimension, and
	// initialize them with zero values
	heightsForShapeDisplay = std::vector<std::vector<unsigned char>>(m_serialShapeIOManager->shapeDisplaySizeX, std::vector<unsigned char>(m_serialShapeIOManager->shapeDisplaySizeY));
	heightsFromShapeDisplay = std::vector<std::vector<unsigned char>>(m_serialShapeIOManager->shapeDisplaySizeX, std::vector<unsigned char>(m_serialShapeIOManager->shapeDisplaySizeY));
	
	// initialize shape display pin configs
	PinConfigs pinConfigs;
	pinConfigs.timeOfUpdate = elapsedTimeInSeconds();
	pinConfigs.gainP = m_serialShapeIOManager->getGainP();
	pinConfigs.gainI = m_serialShapeIOManager->getGainI();
	pinConfigs.maxI = m_serialShapeIOManager->getMaxI();
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

void AppManager::update() {
	
	// cout << "Update in App Manager\n";
	
	// time elapsed since last update
	std::chrono::steady_clock clock;
	std::chrono::duration<double, std::ratio<1, 1>> time = clock.now().time_since_epoch();
	double currentTime = time.count();
	double dt = currentTime - timeOfLastUpdate;
	timeOfLastUpdate = currentTime;
	
	// copy heights and pin configs from application
	bool pinConfigsAreStale = false;
	if (!paused) {
		currentApplication->update(dt);
		currentApplication->getHeightsForShapeDisplay(heightPixelsForShapeDisplay);
		
		// note: manually looping over all pixels is important! the underlying
		// ofPixels char array is stored as unsigned char[y][x], while the
		// shape display heights are stored as unsigned char[x][y]
		for (int x = 0; x < m_serialShapeIOManager->shapeDisplaySizeX; x++) {
			for (int y = 0; y < m_serialShapeIOManager->shapeDisplaySizeY; y++) {
				int xy = heightPixelsForShapeDisplay.getPixelIndex(x, y);
				heightsForShapeDisplay[x][y] = heightPixelsForShapeDisplay[xy];
			}
		}
		
		pinConfigsAreStale = timeOfLastPinConfigsUpdate < currentApplication->timeOfLastPinConfigsUpdate;
		if (pinConfigsAreStale) {
			currentApplication->getPinConfigsForShapeDisplay(pinConfigsForShapeDisplay);
		}
	}
	
	// Render the shape preview from the app into the graphicsForShapeDisplay
	// frame buffer.
	
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
	
	// set the application based on the GUI mode buttons
	int i = 0;
	for (Application* app : debugApplications) {
		if (debugModeButtons[i] && app != currentApplication)
			setCurrentApplication(app);
		i++;
	}

	if (autoTransition) checkAutoTransition(dt);
}

// Takes a 2D vector of heights and converts it to an ofPixels object
// ofPixels stores pixel data as a 1d flat array of chars, so the 2d vectors
// need to be iteratively flattened into a 1d array.

ofPixels AppManager::convertHeightsToPixels(
		const std::vector<std::vector<unsigned char>> &heights) {
	// Create and allocate a local ofPixels object of the right size.
	ofPixels pixels;
	pixels.allocate(m_serialShapeIOManager->shapeDisplaySizeX,
					m_serialShapeIOManager->shapeDisplaySizeY, OF_PIXELS_GRAY);

	// Loop over the 2D vector of heights.
	for (int x = 0; x < m_serialShapeIOManager->shapeDisplaySizeX; x++) {
		for (int y = 0; y < m_serialShapeIOManager->shapeDisplaySizeY; y++) {
			// ofPixels are storeted as a 1D array, so we need to convert the 2D index
			// to a 1D index.
			int index = static_cast<int>(pixels.getPixelIndex(x, y));
	
			// Set the corresponding pixel in the ofPixels object to the height value.
			pixels[index] = heights[x][y];
		}
	}
	
	return pixels;
}

void AppManager::draw() {
	ofBackground(0, 0, 0);
	ofSetColor(255);
	
	// draw text
	int menuLeftCoordinate = 21;
	int menuHeight = 680;
	string title = currentApplication->getName() + (showDebugGui ? " - Debug" : "");
	ofDrawBitmapString(title, menuLeftCoordinate, menuHeight);
	menuHeight += 30;
	ofDrawBitmapString((string) "  '?' : " + (showGlobalGuiInstructions ? "hide" : "show") + " instructions",menuLeftCoordinate, menuHeight);
	if (showGlobalGuiInstructions) {
		menuHeight += 20;
		ofDrawBitmapString((string) "  '1' - '9' : select application",menuLeftCoordinate, menuHeight);
		menuHeight += 20;
		ofDrawBitmapString((string) "  '.' : turn debug gui " + (showDebugGui ? "off" : "on"),menuLeftCoordinate, menuHeight);
		menuHeight += 20;
		ofDrawBitmapString((string) "  ' ' : " + (paused ? "play" : "pause"),menuLeftCoordinate, menuHeight);
	}
	menuHeight += 30;
	
	// if there isn't already a debug gui, draw some more information
	if (!showDebugGui || currentApplication == waveModeContours) {
		ofDrawBitmapString(currentApplication->appInstructionsText(),menuLeftCoordinate, menuHeight);
		menuHeight += 20;
	}
	
	debugGui.draw();

    // Draw the rectangular buttons for each application.
    for (int i = 0; i < applicationButtons.size(); i++){
        if (applications[i] == currentApplication){
            // Green for the current application
            ofSetColor(ofColor::seaGreen);
        } else if (applicationSwitchBlocked && applications[i] == lastSelectedApplication){
            // Dark green for the target application during the transition, so that there is immediate button feedback.
			ofSetColor(ofColor::darkGreen);
        } else {
            // Dark blue for the unselected applications.
            ofSetColor(ofColor::midnightBlue);
		}
		// Draw a rounded rectangle for the application button with a 20 pixel radius.
        ofDrawRectRounded(applicationButtons[i], 20);
        
        // Make a stroke around the current application button
        if (applications[i] == currentApplication){
            ofSetColor(ofColor::white);
            ofNoFill();
            ofSetLineWidth(2);
            ofDrawRectRounded(applicationButtons[i], 20);
            ofFill();
        }

        // Make a string with the name of the application with the loop index plus one prepended.
        Application *app = applications[i];
        string applicationOrderString = ofToString(i+1) + ": " + app->getName();
        
        // Add label for application button
        ofSetColor(ofColor::white);
        displayFont20.drawString(applicationOrderString, applicationButtons[i].x + 25, applicationButtons[i].y + 30);
    }

	// draw the buttons for options
	for (int i = 0; i < options.size(); i++) {
		if (*options[i]) {
            ofSetColor(ofColor::lightGoldenRodYellow);
		} else {
			// dark purple
            ofSetColor(ofColor::fromHex(0x300040));
		}

		// Draw a rounded rectangle for the application button with a 20 pixel radius.
        ofDrawRectRounded(optionButtons[i], 20);

        // Add label for application button
        if (*options[i]) {
			ofSetColor(ofColor::black);
		} else {
			ofSetColor(ofColor::white);
		}
        displayFont20.drawString(optionNames[i] + (*options[i] ? " (ON)" : " (OFF)"), optionButtons[i].x + 25, optionButtons[i].y + 30);
	}
	ofSetColor(ofColor::white);
	
	/* Draw the height data being returned for the pin heights by the arduinos */
	ofDrawRectangle(400, 50, 302, 302);
	if (m_serialShapeIOManager->heightsFromShapeDisplayAvailable) {
		ofDrawBitmapString("Current Physical Heights", 400, 40);
		// Make a reference to the heights from the boards, this is memory safe
		// because it doesn't copy the data.
		const auto &heightsFromBoards = m_serialShapeIOManager->getHeightsFromShapeDisplay();
	
		// Convert the heights to pixels and draw them with an ofImage
		ofPixels pixelsFromBoards = convertHeightsToPixels(heightsFromBoards);
		ofImage imageFromBoards = ofImage(pixelsFromBoards);
		setImageNotBlurry(imageFromBoards);
		imageFromBoards.draw(401, 51, 300, 300);
	}
	
	ofDrawRectangle(705, 50, 302, 302);
	ofDrawBitmapString("Heights Being Sent", 700, 40);
	ofImage heightImageForShapeDisplay = ofImage(heightPixelsForShapeDisplay);
	setImageNotBlurry(heightImageForShapeDisplay);
	heightImageForShapeDisplay.draw(706, 51, 300, 300);
	
	// display grid position of mouse in pin height input/output
	auto mouseGridPos = getMouseCoordinateInGrid(400, 50, 302, 302, 
			m_serialShapeIOManager->shapeDisplaySizeX,m_serialShapeIOManager->shapeDisplaySizeY);
	if (!mouseGridPos.exists)
		mouseGridPos = getMouseCoordinateInGrid(705, 50, 302, 302, 
			m_serialShapeIOManager->shapeDisplaySizeX,m_serialShapeIOManager->shapeDisplaySizeY);
	if (mouseGridPos.exists) {
		auto pos = mouseGridPos.coordinate;
		ofDrawBitmapString("Pixel Row: " + to_string(pos.second) + "   Pixel Column: " + to_string(pos.first), 20, 20);
	}
	
	ofDrawRectangle(400, 355, 607, 607);
	graphicsForShapeDisplay.draw(401, 356, 605, 605);
	
	// ofDrawRectangle(913, 1, 302, 302);
	// ofImage colorImage = ofImage(colorPixels);
	// setImageNotBlurry(colorImage);
	// colorImage.draw(914, 2, 300, 300);
	
	// draw this app's debugging gui, if selected
	if (showDebugGui) {
		currentApplication->drawDebugGui(401, 305);
	}
}

void AppManager::setCurrentApplication(Application* application) {
	if (applicationSwitchBlocked) return;

	if (currentApplication == application) {
		return;
	}

	if (currentApplication == nullptr) {
		currentApplication = application;
	} else {
		applicationSwitchBlocked = true;
		transitionApp->startTransition(currentApplication, application, 0.6f, &currentApplication, &applicationSwitchBlocked);
		currentApplication = transitionApp;
	}
	
	updateDepthInputBoundaries();
}

void AppManager::checkAutoTransition(double dt) {
	// some modes don't use the kinect manager, so this ensures it is listening
	kinectManager->update();
	int i = 0;
	for (auto rule : rules) {
		// only check rule if currentApplication matches
		if (*rule.from != currentApplication) continue;

		switch (rule.condition) {
		case kinectMovementAboveThreshold:
			if (kinectManager->getMovementInMasked() <= rule.threshold) {
				timeRulesSatisfied[i] = 0.0;
				continue;
			}
			break;
		case kinectMovementBelowThreshold:
			if (kinectManager->getMovementInMasked() >= rule.threshold) {
				timeRulesSatisfied[i] = 0.0;
				continue;
			}
			break;
		}

		timeRulesSatisfied[i] += dt;

		// transitions if all conditions met
		if (timeRulesSatisfied[i] >= rule.timeRuleSatisfiedNeeded) {
			setCurrentApplication(*rule.to);
			timeRulesSatisfied[i] = 0;
		}
		i++;
	}
}

void AppManager::updateDepthInputBoundaries() {
	pair<int, int> boundaries = currentApplication->getDepthInputBoundaries();
	int near = boundaries.first;
	int far = boundaries.second;

	// no more kinect. this function is useless
}

void AppManager::exit() {
	// make other windows shut down

	if (displayWindow != nullptr) {
		displayWindow->setWindowShouldClose();
	}
	if (projectorWindow != nullptr) {
		projectorWindow->setWindowShouldClose();
	}

	// delete kinectManager to let it write to settings file
	delete kinectManager;

	// delete m_serialShapeIOManager to shut down the shape display
	delete m_serialShapeIOManager;
}

// handle key presses. keys unused by app manager are forwarded to the current
// application.
void AppManager::keyPressed(int key) {
	// keys used by app manager must be registered as reserved keys
	const int reservedKeysLength = 13;
	const int reservedKeys[reservedKeysLength] = {'/', '?', '.', ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	const int *reservedKeysEnd = reservedKeys + reservedKeysLength;

	// key press events handled by the application manager
	if (find(reservedKeys, reservedKeysEnd, key) != reservedKeysEnd) {
		if (key == '/' || key == '?') {
			showGlobalGuiInstructions = !showGlobalGuiInstructions;
		} else if (key == '.') {
			showDebugGui = !showDebugGui;
		} else if (key == ' ') {
			paused = !paused;
        } else if (key > '0' && key <= '9') {
            int num = key - '0';
            if (num > 0 && num <= applications.size()) {
                setCurrentApplication(applications[num - 1]);
                lastSelectedApplication = applications[num - 1];
            }
        }
		/*else if (key == '1') {
			setCurrentApplication("mqttTransmission");
		} else if (key == '2') {
			setCurrentApplication("axisChecker");
		} else if (key == '3') {
			setCurrentApplication("videoPlayer");
		} else if (key == '4') {
			setCurrentApplication("kinectHandWavy");
		} else if (key == '6') {
			setCurrentApplication("interactiveWave");
		} else if (key == '7') {
			setCurrentApplication("intWave2");
		} else if (key == '8') {
			setCurrentApplication("equationMode");
		} else if (key == '9') {
			setCurrentApplication("WaveModeContours");
		}*/
		
		// forward unreserved keys to the application
	} else {
		currentApplication->keyPressed(key);
	}
}

void AppManager::keyReleased(int key) {};
void AppManager::mouseMoved(int x, int y) {};
void AppManager::mouseDragged(int x, int y, int button) {};
void AppManager::mousePressed(int x, int y, int button) {
    // Check if any of the application buttons were clicked
    for (int i = 0; i < applicationButtons.size(); i++){
        if (applicationButtons[i].inside(x, y)){
			// Set the current application to the one that was clicked.
            setCurrentApplication(applications[i]);
			// Also set the last selected application, so that the button can be highlighted during the transition to the new application.
            lastSelectedApplication = applications[i];
        }
    }

	for (int i = 0; i < optionButtons.size(); i++) {
		if (optionButtons[i].inside(x, y)) {
			*options[i] = !*options[i];
		}
	}

    currentApplication->mousePressed(x, y, button);
};
void AppManager::mouseReleased(int x, int y, int button) {};
void AppManager::windowResized(int w, int h) {};
void AppManager::gotMessage(ofMessage msg) {};
void AppManager::dragEvent(ofDragInfo dragInfo) {};
void AppManager::mouseEntered(int x, int y) {};
void AppManager::mouseExited(int x, int y) {};
