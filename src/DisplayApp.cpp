#include "DisplayApp.hpp"

//--------------------------------------------------------------
void DisplayApp::setup(){
}

//--------------------------------------------------------------
void DisplayApp::update(){
}

//--------------------------------------------------------------
void DisplayApp::draw(){
    ofBackground(0,0,0);
    ofSetColor(255);

    mainApp->currentApplication->drawGraphicsForPublicDisplay(0, 0, width, height);
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
