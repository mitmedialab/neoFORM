//
//  KinectDebugApp.cpp
//  neoForm
//
//  Created by admin on 10/18/22.
//

#include "KinectDebugApp.hpp"

KinectDebugApp::KinectDebugApp(KinectManager* kinectManager){
    m_kinectManager = kinectManager;
}

void KinectDebugApp::update(float dt) {
    cout << "we updatin";
    
    m_kinectManager->update();
}

void KinectDebugApp::drawGraphicsForShapeDisplay(int x, int y, int width, int height){
    cout << "they see us rollin, they hatin";
    ofImage(m_kinectManager->colorPixels).draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
    //x, y, width, height);
}

