//
//  Application.cpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#include "Application.hpp"

Application::Application() {
    // Default constructor
};

Application::Application(SerialShapeIOManager *theCustomShapeDisplayManager){
    
    m_CustomShapeDisplayManager = theCustomShapeDisplayManager;
    
    heightsForShapeDisplay.allocate(theCustomShapeDisplayManager->shapeDisplaySizeX, theCustomShapeDisplayManager->shapeDisplaySizeY, OF_IMAGE_GRAYSCALE);
    heightsForShapeDisplay.set(0);
    heightsDrawingBuffer.allocate(theCustomShapeDisplayManager->shapeDisplaySizeX, theCustomShapeDisplayManager->shapeDisplaySizeY);
    
}

void Application::setRefForShapeIOManager(SerialShapeIOManager* customIOManager){
    m_CustomShapeDisplayManager = customIOManager;
}

// gets by reference, but still (probably) copies data. Yuck.
void Application::getHeightsForShapeDisplay(ofPixels &heights) {
    heights = heightsForShapeDisplay;
};

void Application::getPinConfigsForShapeDisplay(std::vector<std::vector<PinConfigs>>& configs) {
    pinConfigsForShapeDisplay = configs;
};

/* This is deprecated and should be removed */
/* The apps have their own reference to the shape IO manager and can get the heights from the boards themselves, they don't need an external object to do it for them. */
//void Application::setHeightsFromShapeDisplayRef(const ofPixels *heights) {
//    heightsFromShapeDisplay = heights;
//    hasHeightsFromShapeDisplay = true;
//};
/* End deprecated*/

void Application::setPixelsFromKinectRefs(const ofPixels *colorPixels, const ofPixels *depthPixels) {
    colorPixelsFromKinect = colorPixels;
    depthPixelsFromKinect = depthPixels;
    hasPixelsFromKinect = true;
};
