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

// This is a setter not a getter. Yuck.
void Application::getHeightsForShapeDisplay(ofPixels &heights) {
    heights = heightsForShapeDisplay;
};

void Application::getPinConfigsForShapeDisplay(std::vector<std::vector<PinConfigs>>& configs) {
    pinConfigsForShapeDisplay = configs;
};

void Application::setHeightsFromShapeDisplayRef(const ofPixels *heights) {
    heightsFromShapeDisplay = heights;
    hasHeightsFromShapeDisplay = true;
};

void Application::setPixelsFromKinectRefs(const ofPixels *colorPixels, const ofPixels *depthPixels) {
    colorPixelsFromKinect = colorPixels;
    depthPixelsFromKinect = depthPixels;
    hasPixelsFromKinect = true;
};
