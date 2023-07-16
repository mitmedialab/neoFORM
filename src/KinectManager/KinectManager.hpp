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
#include "ofxXmlSettings.h"

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
    
    vector<int> saveShapeDisplayBoundingBox();
    
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
    
    ofxXmlSettings settings;
    ofRectangle m_mask;
    
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
    
    // Dan book keeping to be able to save config file of contours of rectangular shape display bounding box
    std::vector<int> getBigBoundingRectValues(std::vector<ofRectangle>);
    
    std::vector<ofRectangle> getBlobs();
    
    //linked list of ofRectangles for blob contours
    std::vector<ofRectangle> m_capturedContours;
    int m_contoursRecordedFlag = 0;
    //ofRectangle* m_capturedContours;
    
    bool m_configConfirmed = false; // to determine whether or not to check for config
    
};

#endif /* KinectManager_hpp */
