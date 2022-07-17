//
//  Application.cpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#include "Application.hpp"

Application::Application() {
    heightsForShapeDisplay.allocate(SHAPE_DISPLAY_SIZE_X, SHAPE_DISPLAY_SIZE_Y, OF_IMAGE_GRAYSCALE);
    heightsForShapeDisplay.set(0);
    heightsDrawingBuffer.allocate(SHAPE_DISPLAY_SIZE_X, SHAPE_DISPLAY_SIZE_Y);
};

void Application::getHeightsForShapeDisplay(ofPixels &heights) {
    heights = heightsForShapeDisplay;
};

void Application::getPinConfigsForShapeDisplay(PinConfigs configs[SHAPE_DISPLAY_SIZE_X][SHAPE_DISPLAY_SIZE_Y]) {
    PinConfigs *src = (PinConfigs *) pinConfigsForShapeDisplay;
    copy(src, src + SHAPE_DISPLAY_SIZE_2D, (PinConfigs *) configs);
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
