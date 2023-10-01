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
    
    // Extracts only the actuated pixel from the full TRANSFORM surface, leaving behind the dead zones.
    ofPixels getLivePixelsFromFullTransformSurface( ofPixels fullSurface );
    
    // Press a key to save the current large bounding box (hopefully for the TRANSFORM) to a text file
    void keyPressed(int key);
    
    ofImage rawSurfaceDepth;


private:
    KinectManager* m_kinectManager;
    
    int m_transformStartHeight;
    
    void updateHeights();
    
};
#endif /* KinectDebugApp_hpp */
