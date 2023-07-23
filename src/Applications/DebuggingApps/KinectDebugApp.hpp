//
//  KinectDebugApp.hpp
//  neoForm
//
//  Created by admin on 10/18/22.
//

#ifndef KinectDebugApp_hpp
#define KinectDebugApp_hpp

#include <stdio.h>
#include "Application.hpp"
#include "KinectManager.hpp"

#include "ofxOpenCv.h"

#include "ofxXmlSettings.h"

class KinectDebugApp : public Application{
public:

    KinectDebugApp(KinectManager* kinectManager);
    
    void setup();
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    
    // Press a key to save the current large bounding box (hopefully for the TRANSFORM) to a text file
    void keyPressed(int key);
    
    ofImage rawSurfaceDepth;


private:
    KinectManager* m_kinectManager;
    
    void updateHeights();
    
};
#endif /* KinectDebugApp_hpp */
