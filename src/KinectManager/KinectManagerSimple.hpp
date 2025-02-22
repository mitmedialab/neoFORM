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
	KinectManagerSimple();
    ~KinectManagerSimple();
    void update();
	ofShortPixels getDepthPixels();
	ofShortPixels getContourPixels();
	ofShortPixels getUnfilteredDepthPixels();
	ofPixels getColorPixels();
	void cropUsingMask(ofPixels &pixels);
	void cropUsingMask(ofShortPixels &pixels);

	// pixels closer than nearClip are set to 0 (far), other pixels closer than nearThreshold are set to 65535 (near)
	// nearClip & farClip in millimeters.
	// nearThreshold between 0 (equivilent to farClip) and 65535 (equivilent to nearClip)
	void setDepthClipping(unsigned short nearClip, unsigned short farClip, unsigned short nearThreshold);
	void saveMaskAndClip();

	double getMovement() {return totalMovement;}
	double getMovementInMasked() {return totalMovementInMasked;}

public:
	ofRectangle mask;

protected:
	void updateTotalMovement();
	void thresholdInterp(ofShortPixels &pixels, unsigned short lowThresh, unsigned short highThresh, unsigned short lowValue, unsigned short highValue);
	void reverseParallax(float targetPlaneDepthInMillimeters);

protected:
	ofxKinect kinect;

	bool kinectInitialized;
	ofShortPixels depthPixels;
	ofShortPixels unfilteredDepthPixels;
	ofShortPixels contourPixels;
	ofShortPixels colorPixels;

	// internally only used to cache setDepthClipping() params until saveMaskAndClip().
	unsigned short nearClip;
	unsigned short farClip;
	unsigned short nearThreshold;

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
	// Masked buffer resizes to the same dimensions as non-masked, potentially upscaling
	circularBuffer<ofPixels, totalStoredFrames> previousDepthPixelsFramesInMasked;
	// Larger factor will average movement over a longer time. 
	// It isn't used linearly so there isn't a direct "seconds" comparison.
	const double movementTimeFactor = 0.95;
	// A number from 0 to 1, larger the more movement there has been recently
	// Experimentally, it's around 10^-8 without movement and 10^-6 with movement
	double totalMovement;
	double totalMovementInMasked;
};

#endif //KinectManagerSimple_hpp
