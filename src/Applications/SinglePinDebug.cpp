//
//  SinglePinDebug.cpp
//  emptyExample
//
//  Created by Charles Reischer on 10/13/24.
//

#include "SinglePinDebug.hpp"
#include "ofEvents.h"
#include "utils.hpp"

void SinglePinDebug::update(float dt) {
    heightsForShapeDisplay.setColor(0);
    if (graphicsHeight <= 0 || graphicsWidth <= 0) return;

    int sizeX = m_CustomShapeDisplayManager->shapeDisplaySizeX;
    int sizeY = m_CustomShapeDisplayManager->shapeDisplaySizeY;

    // find pixel position of mouse
    int pixelX = ((ofGetMouseX() - graphicsX) * sizeX)/graphicsWidth;
    int pixelY = ((ofGetMouseY() - graphicsY) * sizeY)/graphicsHeight;
    
    if (pixelX >= 0 && pixelX < sizeX && pixelY >= 0 && pixelY < sizeY) {
        int xy = heightsForShapeDisplay.getPixelIndex(pixelX, pixelY);
        heightsForShapeDisplay[xy] = 255;
    }
}

void SinglePinDebug::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    int sizeX = m_CustomShapeDisplayManager->shapeDisplaySizeX;
    int sizeY = m_CustomShapeDisplayManager->shapeDisplaySizeY;

    ofImage image = ofImage(heightsForShapeDisplay); 
    setImageNotBlurry(image);
    image.draw(x, y, width, height);
    
    // draw grid on top of image
    for (int i = 1; i < sizeX; i++) {
        int xPos = float(i) * float(width)/sizeX + x;
        ofDrawLine(xPos, y, xPos, y + height);
    }

    for (int i = 1; i < sizeY; i++) {
        int yPos = float(i) * float(height)/sizeY + y;
        ofDrawLine(x, yPos, x + width, yPos);
    }
}
