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
    m_kinectManager->setupTransformedPixelMap();
    
    cout << "hello there hand wavy";
}

void KinectHandWavy::setupDepthFloorMap() {
    //set all pins to 0
    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {

        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
            
            // This takes the 2 dimensional coordinates and turns them into a one dimensional index for the flattened array.
            int flattenedIndex = heightsForShapeDisplay.getPixelIndex(x, y);
            
            // This takes the 1 dimensional index for the pin, and grabs the corresponding index from the uncorrected video pixel array.
            heightsForShapeDisplay[flattenedIndex] = 0;
        }
    }
    
    // Record pins
    ofPixels m_videoPixels = m_kinectManager->getCroppedPixels(m_kinectManager->depthPixels);
    
    // Extract the current live pixels and save them to the rawSurfaceDepth
    rawSurfaceDepth = m_CustomShapeDisplayManager->getActuatedPixelsFromFullTransformSurface(m_videoPixels, m_kinectManager->m_mask);
}

void KinectHandWavy::update(float dt) {
    cout << "I'm updating!!!!!!";
    
    m_kinectManager->update();
    updateHeights();
    
}

void KinectHandWavy::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    
}

void KinectHandWavy::updateHeights() {
    if (m_kinectManager->useMask == false) {
        return;
    }

    // Temporary adaptations, but we need to actually crop out the pixels from the mask
    // Use OpenCV to crop the depth image to the mask dimensions
    IplImage* iplImg = m_kinectManager->depthImg.getCvImage();
    cv::Mat cvMatData = cv::cvarrToMat(iplImg);
    cv::Rect roi(65, 254, 490, 110); // Define your ROI (Region of Interest)
    cv::Mat croppedDepthMat = cvMatData(roi);

    //croppedDepthMat.convertTo(croppedDepthMat, CV_8U);
    
    ofxCvGrayscaleImage croppedDepthImg;
    croppedDepthImg.allocate(croppedDepthMat.cols, croppedDepthMat.rows);

if (croppedDepthMat.isContinuous()) {
    croppedDepthImg.setFromPixels(croppedDepthMat.data, croppedDepthMat.cols, croppedDepthMat.rows);
} else {
    std::vector<unsigned char> buffer(croppedDepthMat.begin<unsigned char>(), croppedDepthMat.end<unsigned char>());
    croppedDepthImg.setFromPixels(&buffer[0], croppedDepthMat.cols, croppedDepthMat.rows);
}
    
    
    // Add blur to the depth image.
    ofxCvGrayscaleImage blurredDepthImg = croppedDepthImg;
    blurredDepthImg.blurGaussian(41);
    
    // Scale blurredDepthImg to 490 by 100 <- hardcoded only out of sheer desperation.
    //blurredDepthImg.resize(490, 100);
    
    // Pass the current depth image to the shape display manager to get the actuated pixels.
    ofPixels livePixels = m_CustomShapeDisplayManager->getActuatedPixelsFromFullTransformSurface( blurredDepthImg.getPixels(), m_kinectManager->m_mask );
    
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

void KinectHandWavy::keyPressed(int Key) {
    
}
