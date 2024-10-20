//
//  PinDisabler.hpp
//  emptyExample
//
//  Created by Charles Reischer on 10/19/24.
//

#ifndef PinDisabler_hpp
#define PinDisabler_hpp

#include "Application.hpp"

class PinDisabler : public Application {
public:
    PinDisabler(SerialShapeIOManager *shapeDisplayManager, int x, int y, int width, int height);

    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    void mousePressed(int x, int y, int button);

    string appInstructionsText() {return "Click on application window to toggle pin under cursor";}
    string getName() {return "Pin Disabler";}
protected:
    // store the current state, to prevent repeatedly reading settings file
    vector<pair<int, int>> disabledPins;
    // get position in settings file of arbitrary pin
    // e.g. disabledMap[disabledPins[n]] == n
    map<pair<int, int>, int> disabledMap;

    // position of graphicsForShapeDisplay
    int graphicsX;
    int graphicsY;
    int graphicsWidth;
    int graphicsHeight;
};

# endif // PinDisabler_hpp

