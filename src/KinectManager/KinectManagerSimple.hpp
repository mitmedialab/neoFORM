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
#include "utils.hpp"

class KinectManagerSimple {
public:
	KinectManagerSimple(short nearClip = 400, short farClip = 3800);
    ~KinectManagerSimple();
    void update();
	ofShortPixels getDepthPixels();
	ofShortPixels getContourPixels();
	ofPixels getColorPixels();
	void crop(ofPixels &pixels);
	void cropUsingMask(ofShortPixels &pixels);
	void setDepthClipping(short nearClip, short farClip);
	void thresholdInterp(ofShortPixels &pixels, unsigned short lowThresh, unsigned short highThresh, unsigned short lowValue, unsigned short highValue);

	double getMovement() {return totalMovement;}

public:
	ofRectangle mask;

protected:
	void updateTotalMovement();

protected:
	ofxKinect kinect;

	bool kinectInitialized;
	ofShortPixels depthPixels;
	ofShortPixels contourPixels;
	ofShortPixels colorPixels;

	// To counteract the effect of flickering, multiple frames are checked at once.
	// "constexpr static" is used to ensure the compiler can optimize to these values. (and circularBuffer needs it)
	constexpr static int numReferenceFrames = 8;
	constexpr static int numRecentFrames = 8;
	constexpr static int totalStoredFrames = numReferenceFrames + numRecentFrames;

	// To avoid slow-down from image processing, stored frames are scaled down.
	// e.g. a value of 4 means 4x smaller in EACH dimension
	const int storedFrameScaleFactor = 4;
	int storedFrameWidth;
	int storedFrameHeight;

	circularBuffer<ofPixels, totalStoredFrames> previousDepthPixelsFrames;
	// Larger factor will average movement over a longer time. 
	// It isn't used linearly so there isn't a direct "seconds" comparison.
	const double movementTimeFactor = 0.95;
	// A number from 0 to 1, larger the more movement there has been recently
	// Experimentally, it's around 10^-8 without movement and 10^-6 with movement
	double totalMovement;
};

#endif //KinectManagerSimple_hpp
