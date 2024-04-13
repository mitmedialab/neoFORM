//
//  KinectHandWavy.cpp
//  neoForm
//
//  Created by admin on 2/11/24.
//

#include "KinectHandWavy.hpp"

#include "ofxXmlSettings.h"
#include <opencv2/imgproc.hpp>

KinectHandWavy::KinectHandWavy(KinectManager* kinectManager) {
    m_kinectManager = kinectManager;
}

KinectHandWavy::KinectHandWavy(SerialShapeIOManager *theSerialShapeIOManager, KinectManager *theKinectManager)
: Application(theSerialShapeIOManager) {
    
    m_kinectManager = theKinectManager;
}



void KinectHandWavy::setup() {
    cout << "hello there hand wavy";
}

void KinectHandWavy::update(float dt) {
    cout << "I'm updating!!!!!!";
    
    m_kinectManager->update();
    updateHeights();
    
}

void KinectHandWavy::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    // Draw a preview of the depthImg from the kinect manager.
    m_kinectManager->depthImg.draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
    
    //** draw a rectangle around the shape display pixels.
    ofSetColor(0, 0, 255);
    ofNoFill();
    // Set stroke weight to 2 pixels
    ofSetLineWidth(5);
    
    ofDrawRectangle(
                    m_kinectManager->m_mask.getX(),
                    m_kinectManager->m_mask.getY(),
                    m_kinectManager->m_mask.getWidth(),
                    m_kinectManager->m_mask.getHeight()
    );
    //** end draw a rectangle around the shape display pixels.

    
    //unset color and fill for future drawing operations
    ofSetColor(255, 255, 255);
    ofFill();
    
    // Try cropping the depthImg to the mask dimensions here for good luck
    //ofxCvGrayscaleImage croppedDepthImg = m_kinectManager->depthImg;
    //cv::Mat cvMatData = cv::toMat(m_kinectManager->depthImg.getCvImage());

    // Use OpenCV to crop the depth image to the mask dimensions
    ofRectangle ofRect = m_kinectManager->m_mask; // Convert the ofRectangle to a cv::Rect
    cv::Rect roi(ofRect.x, ofRect.y, ofRect.width, ofRect.height);
    ofxCvGrayscaleImage croppedDepthImg = cropCvGrayscale(m_kinectManager->depthImg, roi);


    //croppedDepthImg.draw(2, 400, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
    
    //croppedDepthImg.setROI(m_kinectManager->m_mask);
    
    
    // Preview shape display pixels
    //ofxCvGrayscaleImage blurredDepthImg = m_kinectManager->depthImg;
    ofxCvGrayscaleImage blurredDepthImg = croppedDepthImg;
    blurredDepthImg.blurGaussian(41); // <----- NOTE*!!*! THIS NEEDS TO BE 490 by 110 and is currently 640 by 480
    
    // Scale blurredDepthImg to 490 by 100
    blurredDepthImg.resize(490, 110);
    
    // Preview the bouding box around the blurredImage rectangle
    //ofDrawRectangle(2, 400, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
    
    blurredDepthImg.draw(2, 400, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
    
    //m_kinectManager->drawContours();
    

    // Color pixels for reference
    //m_kinectManager->colorImg.draw(2, 400, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
    
}

void KinectHandWavy::updateHeights() {
    // Add blur to the depth image.
    ofxCvGrayscaleImage blurredDepthImg = getBlurredDepthImg();
    
    // Pass the current depth image to the shape display manager to get the actuated pixels.
    ofPixels livePixels = m_CustomShapeDisplayManager->cropToActiveSurface( blurredDepthImg.getPixels() );
    
    // Process the inputs and updates the 'heightsForShapeDisplay' property accordingly.
    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {

        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
            
            // This takes the 2 dimensional coordinates and turns them into a one dimensional index for the flattened array.
            int flattenedIndex = heightsForShapeDisplay.getPixelIndex(x, y);
            
            // This takes the 1 dimensional index for the pin, and grabs the corresponding index from the uncorrected video pixel array.
            heightsForShapeDisplay[flattenedIndex] = livePixels[flattenedIndex];
        }
    }
}

ofxCvGrayscaleImage KinectHandWavy::getBlurredDepthImg() {
    ofxCvGrayscaleImage blurredDepthImg = m_kinectManager->croppedDepthImg;
    blurredDepthImg.blurGaussian(41);
    
    return blurredDepthImg;
}

void KinectHandWavy::keyPressed(int Key) {
    
}
