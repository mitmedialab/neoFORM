//
//  KinectHandWavy.cpp
//  neoForm
//
//  Created by admin on 2/11/24.
//

#include "KinectHandWavy.hpp"

#include "ofxXmlSettings.h"

KinectHandWavy::KinectHandWavy(KinectManager* kinectManager) {
    m_kinectManager = kinectManager;
}

KinectHandWavy::KinectHandWavy(SerialShapeIOManager *theSerialShapeIOManager, KinectManager *theKinectManager)
: Application(theSerialShapeIOManager) {
    
    m_kinectManager = theKinectManager;
    
    m_kinectManager->setupTransformedPixelMap();
    setupDepthFloorMap();
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
    
    // Uses the opencv cropped contour rectangle to crop the kinect depth pixels to only be in the size and shape of the transform
    ofPixels m_videoPixels = m_kinectManager->getCroppedPixels(m_kinectManager->depthPixels);
    
    // Extact only the actuated part of the surface from the full depthmap;
    ofPixels livePixels = m_CustomShapeDisplayManager->getActuatedPixelsFromFullTransformSurface( m_videoPixels, m_kinectManager->m_mask );
    
    // Process the inputs and updates the 'heightsForShapeDisplay' property accordingly.
    // (Jonthan thinks this could probably be a self-contained function)
    float tempSum = 0;
   
    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {

        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
            
            // This takes the 2 dimensional coordinates and turns them into a one dimensional index for the flattened array.
            int flattenedIndex = heightsForShapeDisplay.getPixelIndex(x, y);
            
            // This takes the 1 dimensional index for the pin, and grabs the corresponding index from the uncorrected video pixel array.
            heightsForShapeDisplay[flattenedIndex] = livePixels[flattenedIndex];
            //m_kinectManager->m_kinectToTransformIndicies[flattenedIndex]];
            
            // TEMPORARY TODO THRESHOLDER FOR ZEROS
            
            if (rawSurfaceDepth[flattenedIndex] >0){
                if ((livePixels[flattenedIndex] <= rawSurfaceDepth[flattenedIndex] +9) && ((livePixels[flattenedIndex] >= rawSurfaceDepth[flattenedIndex] -9))){
                    heightsForShapeDisplay[flattenedIndex] = 0;
                }if(livePixels[flattenedIndex] >rawSurfaceDepth[flattenedIndex] +9){
                    heightsForShapeDisplay[flattenedIndex] = (int) roundf(heightsForShapeDisplay[flattenedIndex]*1.8)-rawSurfaceDepth[flattenedIndex];
                }
               // if (heightsForShapeDisplay[flattenedIndex]>(m_transformStartHeight+50)){
               //     heightsForShapeDisplay[flattenedIndex]=0;
               // }
                //temp way to remove crazy edges
                
            }
            
            tempSum = tempSum+ (float)livePixels[flattenedIndex];
            
            //heightsForShapeDisplay[flattenedIndex] = m_videoPixels[flattenedIndex];//m_kinectManager->m_kinectToTransformIndicies[flattenedIndex]];
        }
    }
    
    tempSum = tempSum/1024.0;
    m_transformStartHeight = (int)tempSum;
    
}

void KinectHandWavy::keyPressed(int Key) {
    
}
