//
//  KinectHandWavy.hpp
//  neoForm
//
//  Created by admin on 2/11/24.
//

#ifndef KinectHandWavy_hpp
#define KinectHandWavy_hpp

#include <stdio.h>
#include "Application.hpp"
#include "KinectManager.hpp"

#include "ofxXmlSettings.h"

class KinectHandWavy : public Application{
public:
    
    KinectHandWavy(KinectManager* kinectManager);
    KinectHandWavy(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManager *theKinectManager);
    
    void setup();
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    
    // Press a key to save the current large bounding box (hopefully for the TRANSFORM) to a text file
    void keyPressed(int key);

private:
    KinectManager* m_kinectManager;
    
    int m_transformStartHeight;
    
    void updateHeights();
    
    ofPixels rawSurfaceDepth;
    
    ofxCvGrayscaleImage getBlurredDepthImg();
    
};




#endif /* KinectHandWavy_hpp */
