//
//  InteractiveWave.hpp
//  neoForm
//
//  Created by Liam May on 6/24/24.
//

#ifndef InteractiveWave_hpp
#define InteractiveWave_hpp

#include <stdio.h>
#include "Application.hpp"
#include "KinectManager.hpp"


//#include "WavyShapeObject.hpp"
//#include "WavyShapeObject.cpp"

#include "ofxXmlSettings.h"

class InteractiveWave : public Application{
public:
    
    InteractiveWave(KinectManager* kinectManager);
    InteractiveWave(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManager *theKinectManager);
    
    void setup();
    void update(float dt);
    
    int cols;
    int rows;
    float friction;
    
    int timeControl;
    
    int highResCols;
    int highResRows;
    int highResFactor;
    
    float **velocity;
    float **density;
    bool **wallMask;
    bool **previousWallMask;
    
    float **highResDensity;
    float **highResVelocity;
    
    float getAdjacencyDensitySum(int x, int y);
    void solveFluid();
    
    void solveHighResFluid();
    void updatePreviousWallMask();
    void applyRippleEffect(int x, int y);
    
    void drawContoursOnColoredMap();
    
    void updateMask();
    
    void drawContours();
    
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    void drawSectionPreviewFrameBuffer(int x, int y, int width, int height);
    
    void keyPressed(int key);
    
    std::tuple<int, int, int> heightPixelToMapColor(int Height);
    
    
private:
    KinectManager* m_kinectManager;
    
    void updateHeights();
    
    ofPixels rawSurfaceDepth;
    ofPixels m_IntWavePixels;
    ofImage m_IntWaveImage;
    ofPixels ProjectorHeightMapPixels;
    
    ofxCvGrayscaleImage getBlurredDepthImg();
    
    void drawPreviewMaskRectangle();
    void drawPreviewActuatedSections();
    
};





#endif /* InteractiveWave_hpp */
