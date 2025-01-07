#include "DisplayApp.hpp"


DisplayApp::~DisplayApp() {
    mainApp->cam = nullptr;
}

//--------------------------------------------------------------
void DisplayApp::setup(){
	mainApp->equationMode->publicDisplaySetup();
    cam.setup(1920, 1080);

	// innitialize GUI
	gui.setup("modes:");
	gui.setPosition(5, 20);
	
	// IMPORTANT: ofxGui uses raw pointers to ofxButton, so an automatic resize
	// of modeButtons will invalidate all existing pointers stored in gui.
	// DO NOT .push_back MORE THAN applications.size()!!!!
	modeButtons.reserve(mainApp->applications.size());

	int appIndex = 1; // Initialize an index for iteration

    // Iterate over the applicationOrder vector and add the corresponding app to the GUI
    for (const auto& appName : mainApp->applicationOrder) {
        Application *app = mainApp->applications[appName];
        
        modeButtons.push_back(ofxButton());
        modeNames.push_back(appName);
        
        // Construct the new button name with the index prepended
        std::string buttonName = std::to_string(appIndex) + ": " + app->getName();
        auto p_button = modeButtons.back().setup(buttonName);
        gui.add(p_button);
        
        appIndex++;
    }

	// Collapse the GUI panel for now to make room for the new graphical buttons.
	gui.minimize();
}

//--------------------------------------------------------------
void DisplayApp::update(){
    cam.update();
	
	// set the application based on the GUI mode buttons
	int i = 0;
	for (string name : modeNames) {
		if (modeButtons[i] && mainApp->applications[name] != mainApp->currentApplication)
			mainApp->setCurrentApplication(name);
		i++;
	}
}

//--------------------------------------------------------------
void DisplayApp::draw(){
    ofBackground(0,0,0);
    ofSetColor(255);

    mainApp->currentApplication->drawGraphicsForPublicDisplay(0, 0, width, height);

	gui.draw();
}

void DisplayApp::exit() {
    mainApp->displayWindow = nullptr;
}

//--------------------------------------------------------------
void DisplayApp::keyPressed(int key){
    // forward to main window
    mainApp->keyPressed(key);
}

//--------------------------------------------------------------
void DisplayApp::keyReleased(int key){

}

//--------------------------------------------------------------
void DisplayApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void DisplayApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void DisplayApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void DisplayApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void DisplayApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void DisplayApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void DisplayApp::windowResized(int w, int h){
    width = w;
    height = h;
}

//--------------------------------------------------------------
void DisplayApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void DisplayApp::dragEvent(ofDragInfo dragInfo){ 

}
