//
//  AxisCheckerApp.cpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#include "AxisCheckerApp.hpp"

AxisCheckerApp::AxisCheckerApp(SerialShapeIOManager *theSerialShapeIOManager) : Application(theSerialShapeIOManager) {
    cout << "AxisCheckerApp constructor" << endl;
}

void AxisCheckerApp::update(float dt) {
    normalizedPhase += dt * 0.5;
    tally++;
    tally %= m_CustomShapeDisplayManager->shapeDisplaySizeX + m_CustomShapeDisplayManager->shapeDisplaySizeY;
    updateHeights();
}

void AxisCheckerApp::updateHeights() {
    if (checkerboard) {
        for (int x = 0; x < m_CustomShapeDisplayManager->shapeDisplaySizeX; x++) {
            for (int y = 0; y < m_CustomShapeDisplayManager->shapeDisplaySizeY; y++) {
                int height;
                if (x < m_CustomShapeDisplayManager->shapeDisplaySizeX / 2 && y < m_CustomShapeDisplayManager->shapeDisplaySizeY / 2) {
                    height = 40;
                } else if (x < m_CustomShapeDisplayManager->shapeDisplaySizeX / 2) {
                    height = 250;
                } else if (y < m_CustomShapeDisplayManager->shapeDisplaySizeY / 2) {
                    height = 110;
                } else {
                    height = 180;
                }
                int xy = heightsForShapeDisplay.getPixelIndex(x, y);
                heightsForShapeDisplay[xy] = height;
            }
        }
    } else {
        for (int x = 0; x < m_CustomShapeDisplayManager->shapeDisplaySizeX; x++) {
            for (int y = 0; y < m_CustomShapeDisplayManager->shapeDisplaySizeY; y++) {
                int height;
                if (tally < m_CustomShapeDisplayManager->shapeDisplaySizeX) {
                    height = x == tally ? 255 : 0;
                } else {
                    height = m_CustomShapeDisplayManager->shapeDisplaySizeX + y == tally ? 255 : 0;
                }
                int xy = heightsForShapeDisplay.getPixelIndex(x, y);
                heightsForShapeDisplay[xy] = height;
            }
        }
    }
}

void AxisCheckerApp::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    color.setHsb(fmod(normalizedPhase * 180, 180), 255, 255);
    ofSetColor(color);
    ofImage(heightsForShapeDisplay).draw(x, y, width, height);
}

string AxisCheckerApp::appInstructionsText() {
    string instructions = (string) "" +
        "This can be used to identify axis orientation. There are two modes,\n" +
        "scan lines and checkerboard.\n" +
        "\n" +
        "  'a' : switch to " + (checkerboard ? "scan lines" : "checkerboard") + " mode" +
        "";

    return instructions;
}

void AxisCheckerApp::keyPressed(int key) {
    if (key == 'a') {
        checkerboard = !checkerboard;
        tally = 0;
    }
}
