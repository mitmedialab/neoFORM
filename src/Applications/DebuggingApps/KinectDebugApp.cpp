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

void KinectDebugApp::setup() {
    m_kinectManager->setupTransformedPixelMap();
}
void KinectDebugApp::update(float dt) {
    cout << "we updatin";
    //update();
    
    m_kinectManager->update();
    
    updateHeights();
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

void KinectDebugApp::updateHeights() {
    // Get pixel values from the video and map them to pin heights here.
    // m_videoPixels is the stored pixels from the current video frame, stored in this app header.
    ofPixels m_videoPixels = m_kinectManager->getCroppedPixels((m_kinectManager->depthPixels));//video.getPixels();
    
    int xAccumPixels = 0;
    
    ofFbo DeadandAliveBlocks;
    DeadandAliveBlocks.allocate(m_kinectManager->m_mask.width, m_kinectManager->m_mask.height, OF_IMAGE_GRAYSCALE);
    DeadandAliveBlocks.begin();
    
    
    //create sets of ofImages from the separate block regions
    ofPixels zone0;
    zone0.allocate((int) (m_kinectManager->m_Transform_L_outer/m_kinectManager->m_mask.height), (int)(m_kinectManager->m_Transform_H/m_kinectManager->m_mask.width), OF_IMAGE_GRAYSCALE);
    
    xAccumPixels = m_kinectManager->m_Transform_L_outer/m_kinectManager->m_mask.height;
    
    ofPixels zone1;
    m_videoPixels.cropTo(zone1, xAccumPixels,0, (int)(m_kinectManager->m_Transform_block/m_kinectManager->m_mask.width), (int)(m_kinectManager->m_Transform_H/m_kinectManager->m_mask.height));
    
    xAccumPixels = xAccumPixels + m_kinectManager->m_Transform_block/m_kinectManager->m_mask.width;
    
    ofPixels zone2;
    zone2.allocate(m_kinectManager->m_Transform_L_inner, m_kinectManager->m_Transform_H, OF_IMAGE_GRAYSCALE);
    
    
    //smash them together
    //rerender them and resize
    
    
    // Grayscale ensures that there is only one brightness value per pixel (instead of three channel RGB).
    m_videoPixels.setImageType(OF_IMAGE_GRAYSCALE);
    
    //  float scaledW = 102.0/m_Transform_W;
    //float scaledH = 24.0/m_Transform_H;
    
    
    
    m_videoPixels.resize(48,24);
    m_videoPixels.rotate90(2);
   
    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {

        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
            
            // This takes the 2 dimensional coordinates and turns them into a one dimensional index for the flattened array.
            int flattenedIndex = heightsForShapeDisplay.getPixelIndex(x, y);
            
            // This takes the 1 dimensional index for the pin, and grabs the corresponding index from the uncorrected video pixel array.
            heightsForShapeDisplay[flattenedIndex] = m_videoPixels[flattenedIndex];//m_kinectManager->m_kinectToTransformIndicies[flattenedIndex]];
        }
    }
}



