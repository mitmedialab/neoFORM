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

class KinectManager {
public:
    KinectManager();
    
    ~KinectManager();
    
    void update();
    
    void orientInputImages();
    
    void getColorPixels(ofPixels &pixels);
    
    void getDepthPixels(ofPixels &pixels);
    
    void setDepthClipping(int near, int far);
    
    int numAvailableDevices();
    
    bool isFrameNew();
    
    bool isConnected();
    
    int getImageWidth();
    
    int getImageHeight();
    
private:
    ofxKinect kinect;
    
    int imageWidth;
    int imageHeight;
    
    ofPixels colorPixels;
    ofPixels depthPixels;
};

#endif /* KinectManager_hpp */
