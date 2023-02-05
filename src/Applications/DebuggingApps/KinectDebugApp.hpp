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

class KinectDebugApp : public Application{
public:

    KinectDebugApp(KinectManager* kinectManager);
    
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    
    // near and far boundary values for depth data captured, specified in millimeters
    //pair<int, int> getDepthInputBoundaries();
    
    // FROM NEW KINECT APP THING
    
  
    int mNearThreshold; // the far threshold, closest possible value is 255, farthest possible value 0
        int mFarThreshold; // the far threshold, closest possible value is 255, farthest possible value 0
        int mContourMinimumSize; // the minimum size of a contour in pixels
        bool isCurrentlyRecording;
        bool playFromRecording;
        bool useMask = false;
        
        ofImage mask, colorMask;
        ofxCvColorImage maskColorCv;
        ofxCvGrayscaleImage maskCv;
    
    // form knect methods
    void setup(int pNearThreshold = 230, int pFarThreshold = 70, int pContourMinimumSize = 20);
        
    void update(); //kinect version of update
    void updateCurrentFrame(unsigned char * pixels, int w, int h);
    
    // draw various images coordinates and size
        void drawColorImage(int x, int y, int width, int height);
        void drawDepthImage(int x, int y, int width, int height);
        
        void drawThresholdImage(int x, int y, int width, int height);
        
        void drawDepthThreshedImage(int x, int y, int width, int height);
        void drawDepthThreshedDiff(int x, int y, int width, int height);
        
        // generate video pixels for recording kinect input
        unsigned char * getRecordingPixels();
        
        // get images as pixels
        ofPixels getColorPixels();
        ofPixels  getDepthPixels();
        ofPixels  grayImagePixels();

        ofPixels  depthThreshedPixels();
        ofPixels depthThresholdOFPixels();
        
        // get contour finder
        ofxCvContourFinder * getContourFinder();
    
    // getter pass through methods
        int     numAvailableDevices();
        bool    isFrameNew();
        bool    isConnected();

        
        ofFbo fbo;
        unsigned char * returnPixels;
        ofPixels ofPixels;
        ofImage recordingImage, playingImage;
        ofImage imageLeft;
        ofImage imageRight;
    
    ofxCvGrayscaleImage     depthThreshed; // grayscale depth image
        ofxCvGrayscaleImage     lastDepthThreshed;
        ofxCvGrayscaleImage     depthThreshedDiff;
        ofxCvColorImage         colorImg;
    
private:
    KinectManager* m_kinectManager;
    
    
    //from kinect thing
    
    ofxKinect               kinect;
    
    ofxCvGrayscaleImage     depthImg; // grayscale depth image
    ofxCvGrayscaleImage     grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage     grayThreshFar; // the far thresholded image
    //ofxCvColorImage         colorImg;
    
    
    void updateImagesFromKinect();
       void updateImagesFromRecording();
       void flagImagesAsChanged();
       void calculateThresholdsAndModifyImages();
       void subtractMask();
       void loadAlphaMaskAndPrepForCvProcessing();
    
    ofxCvContourFinder      contourFinder;
    

};
#endif /* KinectDebugApp_hpp */
