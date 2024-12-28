//
//  WaveModeContours.hpp
//  neoForm
//
//  Created by Liam May on 8/9/24.
//

#ifndef WaveModeContours_hpp
#define WaveModeContours_hpp

#include <stdio.h>
#include "Application.hpp"
#include "KinectManagerSimple.hpp"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"
#include <vector>

class WaveModeContours : public Application{
public:
    
    WaveModeContours(KinectManagerSimple* kinectManager);
    WaveModeContours(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManagerSimple *theKinectManager);
    
    string getName() {return "Interactive Wave";};
    
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
    void handInteraction(int x, int y);
    
    void drawContoursOnColoredMap();
    
    void updateMask();
    
    void drawContours();
    
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    void drawSectionPreviewFrameBuffer(int x, int y, int width, int height);
    
    void keyPressed(int key);
    
    std::tuple<int, int, int> heightPixelToMapColor(int Height);
    std::vector<ofPoint> lastContourCentroids;
    
    
private:
    KinectManagerSimple* m_kinectManager;
    
    void updateHeights();
    
    ofPixels rawSurfaceDepth;
    ofPixels m_IntWavePixels;
    ofImage m_IntWaveImage;
    ofPixels ProjectorHeightMapPixels;
    
    ofxCvGrayscaleImage getBlurredDepthImg();
    
    void drawPreviewMaskRectangle();
    void drawPreviewActuatedSections();
    
    ofxCvContourFinder  contourFinder;
    ofxCvGrayscaleImage depthImg;
    ofPixels maskPixels;
    
    int m_contoursRecordedFlag = 0;
    std::vector<ofRectangle> m_capturedContours;
};




#endif /* WaveModeContours_hpp */
