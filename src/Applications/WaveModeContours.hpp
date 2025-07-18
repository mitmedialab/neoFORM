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

    // Raindrop ripple effect parameters
    float timeControl;
    float rainDropsPerSecond; // Variable to control the number of raindrops per second
    float lastRippleTime; // Timer to track the last ripple effect time
	float currentRainDropInterval; // Randomized Period

    // velocity and density are 2d arrays that represent the state of the fluid simulation.
    std::vector<std::vector<float>> velocity;
    std::vector<std::vector<float>> density;

    // The wall masks are 2d arrays that mark the position of walls (detected obstacles) in the fluid simulation.
    // A new wall mask at a given position can trigger a ripple effect in the fluid simulation.
    std::vector<std::vector<bool>> wallMask;
    std::vector<std::vector<bool>> previousWallMask;

    float getAdjacencyDensitySum(int x, int y);
    void solveFluid(double progressAmount);

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

    string appInstructionsText();

private:
    KinectManagerSimple* m_kinectManager;

    void updateHeights();
    void applyKinectInput();
    
    // Water simulation parameters - adjustable for different effects
    struct WaterSimParams {
        float maxDensityChange = 130.0f;      // Maximum allowed density change per frame
        float responseCurveStrength = 0.6f;   // Controls sigmoid curve sharpness (higher = sharper)
        float temporalSmoothingFactor = 0.5f; // Previous frame influence (0-1)
        float inputAmplification = 50.0f;     // Amplifies input changes for more pronounced effects
    };
    
    // Applies the water simulation effects using depth data
    void applyWaterSimulation(const ofPixels& currentDepthFrame);
    
    // Simulation parameters - can be exposed to UI if needed
    WaterSimParams waterParams;

    ofPixels rawSurfaceDepth;
    ofPixels m_IntWavePixels;
    ofImage m_IntWaveImage;
    ofPixels ProjectorHeightMapPixels;

    ofxCvGrayscaleImage getBlurredDepthImg();

    void drawPreviewMaskRectangle();
    void drawPreviewActuatedSections();

    ofxCvContourFinder  contourFinder;
    ofxCvGrayscaleImage depthImg;
	ofPixels prevKinectDepth;
    ofPixels maskPixels;

	void recalculateRainInterval();

    int m_contoursRecordedFlag = 0;
    std::vector<ofRectangle> m_capturedContours;
};




#endif /* WaveModeContours_hpp */
