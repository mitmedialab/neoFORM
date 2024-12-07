//
//  KinectManagerSimple.hpp
//  neoForm
//
//  Created by Charles Reischer on 12/7/24.
//

#ifndef KinectManagerSimple_hpp
#define KinectManagerSimple_hpp

#include <stdio.h>
#include "ofxCvContourFinder.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"

class KinectManagerSimple {
public:
	KinectManagerSimple(short nearClip = 500, short farClip = 3800);
    ~KinectManagerSimple();
    void update();
	ofShortPixels getDepthPixels();
	ofShortPixels getContourPixels();
	ofPixels getColorPixels();
	void crop(ofPixels &pix);
	void crop(ofShortPixels &pix);
	void setDepthClipping(short nearClip, short farClip);
	void thresholdInterp(ofShortPixels &pix, unsigned short lowThresh, unsigned short highThresh, unsigned short lowValue, unsigned short highValue);

public:
	ofRectangle mask;

protected:
	ofxKinect kinect;

	bool isConnected;
	ofShortPixels depthPixels;
	ofShortPixels contourPixels;
	ofPixels colorPixels;
};

#endif //KinectManagerSimple_hpp
