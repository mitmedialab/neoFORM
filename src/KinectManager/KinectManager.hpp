//
//  KinectManager.hpp
//  neoForm
//
//  Created by admin on 8/15/22.
//

#ifndef KinectManager_hpp
#define KinectManager_hpp

#include <stdio.h>
#include "ofxKinect.h"
#include "ofxOpenCv.h"

class KinectManager {
public:
    KinectManager();
    
    //detailed constructor with contour tracking parameters as arguments
    KinectManager(int pNearThreshold, int pFarThreshold, int pContourMinimumSize);
    
    ~KinectManager();
    
    void baseSetup();
    
    void update();
    
    void orientInputImages();
    
    void getColorPixels(ofPixels &pixels);
    
    void getDepthPixels(ofPixels &pixels);
    
    void setDepthClipping(int near, int far);
    
    void loadAlphaMaskAndPrepForCvProcessing();
    
    void subtractMask();
    
    void calculateThresholdsAndModifyImages();
    
    int numAvailableDevices();
    
    bool isFrameNew();
    
    bool isConnected();
    
    int getImageWidth();
    
    int getImageHeight();
    
    void drawContours();
    //General Kinect Fields
    
    ofxKinect kinect;
    
    int imageWidth;
    int imageHeight;
    
    ofPixels colorPixels;
    ofPixels depthPixels;
    
    // ***********************
    // Special Tracking Fields
    // ***********************
    
    bool contourTrackingOn = false;
    
    int mNearThreshold; // the far threshold, closest possible value is 255, farthest possible value 0
    int mFarThreshold; // the far threshold, closest possible value is 255, farthest possible value 0
    int mContourMinimumSize; // the minimum size of a contour in pixels
    bool isCurrentlyRecording;
    bool playFromRecording;
    bool useMask = false;
    
    ofImage mask, colorMask;
    ofxCvColorImage maskColorCv;
    ofxCvGrayscaleImage maskCv;
    
    ofxCvGrayscaleImage     depthThreshed; // grayscale depth image
    ofxCvGrayscaleImage     lastDepthThreshed;
    ofxCvGrayscaleImage     depthThreshedDiff;
    ofxCvColorImage         colorImg;
    
    ofxCvGrayscaleImage     depthImg; // grayscale depth image
    ofxCvGrayscaleImage     grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage     grayThreshFar; // the far thresholded image
    
    ofImage recordingImage, playingImage;
    ofFbo fbo;
    
    ofxCvContourFinder      contourFinder;
};

#endif /* KinectManager_hpp */
