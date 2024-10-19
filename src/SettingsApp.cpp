#include "SettingsApp.hpp"

//--------------------------------------------------------------
void SettingsApp::setup(){
    // innitialize GUI
    gui.setup("modes:");

    // IMPORTANT: ofxGui uses raw pointers to ofxButton, so an automatic resize
    // of modeButtons will invalidate all existing pointers stored in gui.
    // DO NOT .push_back MORE THAN applications.size()!!!!
    modeButtons.reserve(mainApp->applications.size());
    for (map<string, Application *>::iterator iter = mainApp->applications.begin(); iter != mainApp->applications.end(); iter++) {
        Application *app = iter->second;
        
        modeButtons.push_back(ofxButton());
        modeNames.push_back(iter->first);
        auto p_button = modeButtons.back().setup(app->getName());
        gui.add(p_button);

        /* This is deprecated and should be removed */
        /* The apps have their own reference to the shape IO manager and can get the heights from the boards themselves, they don't need the app manager to do it for them. */
        // shape display heights, if they are accessible
        //if (m_serialShapeIOManager->heightsFromShapeDisplayAvailable) {
        //    app->setHeightsFromShapeDisplayRef(&heightPixelsFromShapeDisplay);
        //}
        /* End deprecated */
        
    }

}

//--------------------------------------------------------------
void SettingsApp::update(){
    //set the application based on the GUI mode buttons
    int i = 0;
    for (string name : modeNames) {
        if (modeButtons[i] && mainApp->applications[name] != mainApp->currentApplication) mainApp->setCurrentApplication(name);
        i++;
    }
}

//--------------------------------------------------------------
void SettingsApp::draw(){
    ofBackground(0,0,0);
    ofSetColor(255);

    // draw text
    int menuLeftCoordinate = 21;
    int menuHeight = 350;
    string title = mainApp->currentApplication->getName() + (mainApp->showDebugGui ? " - Debug" : "");
    ofDrawBitmapString(title, menuLeftCoordinate, menuHeight);
    menuHeight += 30;
    ofDrawBitmapString((string) "  '?' : " + (mainApp->showGlobalGuiInstructions ? "hide" : "show") + " instructions", menuLeftCoordinate, menuHeight);
    if (mainApp->showGlobalGuiInstructions) {
        menuHeight += 20;
        ofDrawBitmapString((string) "  '1' - '9' : select application", menuLeftCoordinate, menuHeight);
        menuHeight += 20;
        ofDrawBitmapString((string) "  '.' : turn debug gui " + (mainApp->showDebugGui ? "off" : "on"), menuLeftCoordinate, menuHeight);
        menuHeight += 20;
        ofDrawBitmapString((string) "  ' ' : " + (mainApp->paused ? "play" : "pause"), menuLeftCoordinate, menuHeight);
    }
    menuHeight += 30;

    // if there isn't already a debug gui, draw some more information
    if (!mainApp->showDebugGui || mainApp->currentApplication == mainApp->applications["water"] || mainApp->currentApplication == mainApp->applications["stretchy"]) {
        ofDrawBitmapString(mainApp->currentApplication->appInstructionsText(), menuLeftCoordinate, menuHeight);
        menuHeight += 20;
    }

    gui.draw();
}

void SettingsApp::exit() {
    mainApp->settingsWindow = nullptr;
}

//--------------------------------------------------------------
void SettingsApp::keyPressed(int key){
    // forward to main window
    mainApp->keyPressed(key);
}

//--------------------------------------------------------------
void SettingsApp::keyReleased(int key){

}

//--------------------------------------------------------------
void SettingsApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void SettingsApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void SettingsApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void SettingsApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void SettingsApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void SettingsApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void SettingsApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void SettingsApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void SettingsApp::dragEvent(ofDragInfo dragInfo){ 

}
