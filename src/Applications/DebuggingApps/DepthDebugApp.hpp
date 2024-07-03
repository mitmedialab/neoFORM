//
//  DepthDebugApp.hpp
//  neoForm
//
//  Created by admin on 8/20/23.
//

#ifndef DepthDebugApp_hpp
#define DepthDebugApp_hpp

#include <stdio.h>
#include "Application.hpp"
#include "KinectManager.hpp"


class DepthDebugApp : public Application {
public:
    DepthDebugApp(SerialShapeIOManager *theCustomShapeDisplayManager);
    
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    string appInstructionsText();
    void keyPressed(int key);

    string getName() {return "Depth Debug";};

private:
    void updateHeights();
    
    int tally = 0;
    bool checkerboard = false;

    float normalizedPhase = 0;
    ofColor color;

    
    
};


#endif /* DepthDebugApp_hpp */
