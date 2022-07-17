//
//  AxisCheckerApp.hpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#ifndef AxisCheckerApp_hpp
#define AxisCheckerApp_hpp

#include <stdio.h>
#include "Application.hpp"


class AxisCheckerApp : public Application {
public:
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    string appInstructionsText();
    void keyPressed(int key);

    string getName() {return "Axis Checker";};

private:
    void updateHeights();

    int tally = 0;
    bool checkerboard = false;

    float normalizedPhase = 0;
    ofColor color;
};

#endif /* AxisCheckerApp_hpp */
