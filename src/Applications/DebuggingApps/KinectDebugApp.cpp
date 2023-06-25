//
//  KinectDebugApp.cpp
//  neoForm
//
//  Created by admin on 10/18/22.
//

#include "KinectDebugApp.hpp"

#include "ofxXmlSettings.h"

KinectDebugApp::KinectDebugApp(KinectManager* kinectManager){
    m_kinectManager = kinectManager;
}

void KinectDebugApp::update(float dt) {
    cout << "we updatin";
    //update();
    
    m_kinectManager->update();
  //  if (!m_save_flag){
  //      saveShapeDisplayBoundingBox();
  //  }
  //  m_save_flag = 1;
}

// Draw the depth data into the app preview window.
void KinectDebugApp::drawGraphicsForShapeDisplay(int x, int y, int width, int height){
    cout << "they see us rollin, they hatin";
    ofImage(m_kinectManager->depthPixels).draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
    
    m_kinectManager->drawContours();
    
}

// Press a key to save the current large bounding box (hopefully for the TRANSFORM) to a text file
void KinectDebugApp::saveShapeDisplayBoundingBox(){
    //m_BoundaryFile.open("shape_display_boundaries.txt",ofFile::WriteOnly);
    //m_BoundaryFile << "some text";
    
    std::vector<int> theRectDims = m_kinectManager->getBigBoundingRectValues(m_kinectManager->getBlobs());
    
    
    
    ofxXmlSettings settings;
    settings.loadFile("settings.xml");
    settings.setValue("x_pos", theRectDims.at(0));
    settings.setValue("y_pos", theRectDims.at(1));
    settings.setValue("width", theRectDims.at(2));
    settings.setValue("height", theRectDims.at(3));
    settings.saveFile("settings.xml");
}

void KinectDebugApp::keyPressed(int key){
    
    if (key=='s'){
        saveShapeDisplayBoundingBox();
    }
    
    
}

