//
//  AxisCheckerApp.cpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#include "AxisCheckerApp.hpp"


void AxisCheckerApp::update(float dt) {
    normalizedPhase += dt * 0.5;
    tally++;
    tally %= SHAPE_DISPLAY_SIZE_X + SHAPE_DISPLAY_SIZE_Y;
    updateHeights();
}

void AxisCheckerApp::updateHeights() {
    if (checkerboard) {
        for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {
            for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
                int height;
                if (x < SHAPE_DISPLAY_SIZE_X / 2 && y < SHAPE_DISPLAY_SIZE_Y / 2) {
                    height = 40;
                } else if (x < SHAPE_DISPLAY_SIZE_X / 2) {
                    height = 250;
                } else if (y < SHAPE_DISPLAY_SIZE_Y / 2) {
                    height = 110;
                } else {
                    height = 180;
                }
                int xy = heightsForShapeDisplay.getPixelIndex(x, y);
                heightsForShapeDisplay[xy] = height;
            }
        }
    } else {
        for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {
            for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
                int height;
                if (tally < SHAPE_DISPLAY_SIZE_X) {
                    height = x == tally ? 255 : 0;
                } else {
                    height = SHAPE_DISPLAY_SIZE_X + y == tally ? 255 : 0;
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
