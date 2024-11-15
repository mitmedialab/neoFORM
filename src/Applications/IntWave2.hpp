//
//  IntWave2.hpp
//  neoForm
//
//  Created by Liam May on 7/1/24.
//

#ifndef IntWave2_hpp
#define IntWave2_hpp

#include <stdio.h>
#include <stdio.h>
#include "Application.hpp"
#include "KinectManager.hpp"

#include "ofxXmlSettings.h"

class IntWave2 : public Application{
public:
    
    IntWave2(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManager *theKinectManager);
    
    void setup();
    void update(float dt);
    
    int cols;
    int rows;
    float friction;
    int maskSize;
    int numNextToWall;
    int numTwoFromWall;
    float displacedWater;
    
    int timeControl;
    
    float **velocity;
    float **density;
    bool **wallMask;
    bool **previousWallMask;
    
    float getAdjacencyDensitySum(int x, int y);
    bool isNextToWall(int x, int y);
    bool isTwoFromWall(int x, int y);
    void solveFluid();
    void applyRippleEffect(int x, int y);
    
    void updateMask();
    void updatePreviousWallMask();
    
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    
    void drawSectionPreviewFrameBuffer(int x, int y, int width, int height);
    
    void keyPressed(int key);
    
    
private:
    KinectManager* m_kinectManager;
    
    void updateHeights();
    
    ofPixels rawSurfaceDepth;
    ofPixels m_IntWavePixels;
    ofImage m_IntWaveImage;
    
    ofxCvGrayscaleImage getBlurredDepthImg();
    
    void drawPreviewMaskRectangle();
    void drawPreviewActuatedSections();
    
};


#endif /* IntWave2_hpp */
