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
