#include "ProjectorApp.hpp"

//--------------------------------------------------------------
void ProjectorApp::setup(){
}

//--------------------------------------------------------------
void ProjectorApp::update(){
}

//--------------------------------------------------------------
void ProjectorApp::draw(){
    ofBackground(0,0,0);
    ofSetColor(255);

    //mainApp->graphicsForShapeDisplay.draw(0, 0, width, height);
    //mainApp->currentApplication->drawGraphicsForShapeDisplay(0, 0, width, height);
    //mainApp->shapeDisplayGraphicsReady = false;
}

void ProjectorApp::exit() {
    mainApp->displayWindow = nullptr;
}

//--------------------------------------------------------------
void ProjectorApp::keyPressed(int key){
    // forward to main window
    mainApp->keyPressed(key);
}

//--------------------------------------------------------------
void ProjectorApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ProjectorApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ProjectorApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ProjectorApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ProjectorApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ProjectorApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ProjectorApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ProjectorApp::windowResized(int w, int h){
    width = w;
    height = h;
}

//--------------------------------------------------------------
void ProjectorApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ProjectorApp::dragEvent(ofDragInfo dragInfo){ 

}
