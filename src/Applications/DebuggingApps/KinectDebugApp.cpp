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

void KinectDebugApp::keyPressed(int key){
    
    if (key=='s'){
        m_kinectManager->saveShapeDisplayBoundingBox();
    }
    
    
}

