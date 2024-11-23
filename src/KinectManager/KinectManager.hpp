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

	void getRawDepthPixels(ofPixels &pixels);
    
    void setDepthClipping(int near, int far);
    
    void calculateThresholdsAndModifyImages(ofxCvGrayscaleImage& inputImage);
    
    int numAvailableDevices();
    
    bool isFrameNew();
    
    bool isConnected();
    
    int getImageWidth();
    
    int getImageHeight();
    
    vector<int> saveShapeDisplayBoundingBox();
    
    //
    //functions for spatially splicing up the bounding box
    //
    
    
    
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
    
    ofxCvColorImage maskColorCv;
    ofxCvGrayscaleImage maskCv;
    
    ofxXmlSettings settings;
    ofRectangle m_mask;
    
    ofxCvGrayscaleImage     depthThreshed; // grayscale depth image
    ofxCvGrayscaleImage     lastDepthThreshed;
    ofxCvGrayscaleImage     depthThreshedDiff;
    ofxCvColorImage         colorImg;
    
    ofxCvGrayscaleImage     depthImg; // grayscale depth image
    ofxCvGrayscaleImage     croppedDepthImg; // cropped grayscale depth image
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
    
    
    // temp, need to remove
    float m_Transform_L_outer = 13.375; //inches
    float m_Transform_L_inner = 13.9375; //inches
    float m_Transform_R_inner = 14.25; //inches
    float m_Transform_R_outer = 13.1875; //inches

    float m_Transform_W = 104.75; //inches
    float m_Transform_H = 26; //inches
    
    float m_Transform_block = 15.75; //inches
    int m_Transform_block_h_pins = 24; // # of pins
    int m_Transform_block_w_pins = 16; // # of pins
    
    ofPixels m_kinectCroppedTransformPixels;
    
    //function that maps kinect pixels to transform pins
    //takes as input depth pixels
    // finds whether depth pixels are in a dead or active block
    // within the block, if active, return affected pins and heights
    
    ofxCvGrayscaleImage cropCvGrayscale(const ofxCvGrayscaleImage& inputImage, cv::Rect roi);
    
    //return a copy of found cropped pixels
    ofPixels getCroppedPixels(ofPixels inputDepthPixels);
    
    
};

#endif /* KinectManager_hpp */
