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
    //ofImage(m_kinectManager->depthPixels).draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
    
    m_kinectManager->drawContours();
    
    // Get the raw depth data cropped to the dimensions of the surface mask.
    
    
    cout << m_kinectManager->depthPixels.getWidth() << endl;
    ofPixels bob;
    
    m_kinectManager->depthPixels.cropTo(bob, m_kinectManager->m_mask.getX(), m_kinectManager->m_mask.getY(), m_kinectManager->m_mask.width, m_kinectManager->m_mask.height);
    
    cout << bob.getWidth() << endl;
    
    ofImage(bob).draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());

}

void KinectDebugApp::keyPressed(int key){
    
    if (key=='s'){
        m_kinectManager->saveShapeDisplayBoundingBox();
    }
    
    
}

