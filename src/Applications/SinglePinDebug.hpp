//
//  SinglePinDebug.hpp
//  emptyExample
//
//  Created by Charles Reischer on 10/13/24.
//

#ifndef SinglePinDebug_hpp
#define SinglePinDebug_hpp

#include "Application.hpp"

class SinglePinDebug : public Application {
public:
    SinglePinDebug(SerialShapeIOManager *theCustomShapeDisplayManager, int x, int y, int width, int height) : 
        Application(theCustomShapeDisplayManager), graphicsX(x), graphicsY(y), 
        graphicsWidth(width), graphicsHeight(height) {}
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    
    string appInstructionsText() {return "Hover over application window\nto raise pin under cursor";}
    string getName() {return "Single Pin Debug";}

    int graphicsX;
    int graphicsY;
    int graphicsWidth;
    int graphicsHeight;
};


#endif //SinglePinDebug_hpp

