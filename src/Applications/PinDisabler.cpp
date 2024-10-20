//
//  PinDisabler.cpp
//  emptyExample
//
//  Created by Charles Reischer on 10/19/24.
//

#ifndef PinDisabler_cpp
#define PinDisabler_cpp

#include "PinDisabler.hpp"
#include "ofGraphicsConstants.h"
#include "ofxXmlSettings.h"
#include "utils.hpp"

PinDisabler::PinDisabler(SerialShapeIOManager *shapeDisplayManager, int x, int y, int width, int height) 
: Application(shapeDisplayManager), graphicsX(x), graphicsY(y), graphicsWidth(width), graphicsHeight(height) {
    disabledPins = shapeDisplayManager->getDisabledPins();
    int i = 0;
    for (pair<int, int> pin : disabledPins) {
        disabledMap[pin] = i;
        i++;
    }
}

void PinDisabler::update(float dt) {
    // set all to max height, to highlight disabled/not working pins
    heightsForShapeDisplay.set(255);
}

void PinDisabler::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    int sizeX = m_CustomShapeDisplayManager->shapeDisplaySizeX;
    int sizeY = m_CustomShapeDisplayManager->shapeDisplaySizeY;

    ofImage disabledPixels = ofImage();
    disabledPixels.allocate(sizeX, sizeY, OF_IMAGE_COLOR);
    // set image black
    disabledPixels.setColor({0, 0, 0, 255});

    // set disabled pins red
    for (pair<int, int> pin : disabledPins) {
        disabledPixels.setColor(pin.first, pin.second, {255, 0, 0, 255});
    }

    // set pixel under mouse yellow
    auto mouseGridPos = getMouseCoordinateInGrid(graphicsX, graphicsY, graphicsWidth, graphicsHeight, sizeX, sizeY);
    if (mouseGridPos.has_value()) {
        disabledPixels.setColor(mouseGridPos.value().first, mouseGridPos.value().second, {255, 255, 0, 255});
    }

    disabledPixels.update();

    // make image not blurry
    disabledPixels.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_NEAREST);
    disabledPixels.draw(x, y, width, height);

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

void PinDisabler::mousePressed(int x, int y, int mouse) {
    // don't do anything unless left-click
    if (mouse != 0) return;

    auto gridPos = getMouseCoordinateInGrid(graphicsX, graphicsY, graphicsWidth, graphicsHeight,
        m_CustomShapeDisplayManager->shapeDisplaySizeX, m_CustomShapeDisplayManager->shapeDisplaySizeY);

    if (!gridPos.has_value()) return;

    ofxXmlSettings settings;
    settings.load("settings.xml");
    settings.pushTag(m_CustomShapeDisplayManager->getShapeDisplayName());
    settings.pushTag("disabledPins");

    bool alreadyDisabled = disabledMap.count(gridPos.value()) > 0;
    if (alreadyDisabled) { //removed existing disabled pin
        int settingsPos = disabledMap[gridPos.value()];
        disabledMap.erase(gridPos.value());

        // swap back with pin
        if (settingsPos != disabledPins.size() - 1) {
            disabledPins[settingsPos] = disabledPins.back();
            disabledMap[disabledPins.back()] = settingsPos;

            settings.setValue("pin_" + to_string(settingsPos) + ":X", gridPos.value().first);
            settings.setValue("pin_" + to_string(settingsPos) + ":Y", gridPos.value().second);
        }
        // remove back pin
        settings.removeTag("pin_" + to_string(disabledPins.size()-1));
        disabledPins.pop_back();
        settings.setValue("num", (int)disabledPins.size());
    } else { // add a disabled pin
        settings.setValue("pin_" + to_string(disabledPins.size()) + ":X", gridPos.value().first);
        settings.setValue("pin_" + to_string(disabledPins.size()) + ":Y", gridPos.value().second);
        settings.setValue("num", (int)disabledPins.size() + 1);

        disabledMap[gridPos.value()] = disabledPins.size();
        disabledPins.push_back(gridPos.value());
    }

    settings.popTag();
    settings.popTag();
    settings.save("settings.xml");
}


#endif // PinDisabler_cpp

