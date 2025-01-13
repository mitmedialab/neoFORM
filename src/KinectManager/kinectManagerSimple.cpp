//
//  KinectManagerSimpleSimple.cpp
//  neoForm
//
//  Created by Charles Reischer on 12/7/24.
//

#ifndef KinectManagerSimple_cpp
#define KinectManagerSimple_cpp

#include "KinectManagerSimple.hpp"
#include "ofxXmlSettings.h"

KinectManagerSimple::KinectManagerSimple(short nearClip, short farClip) {
	// Start at "no movement"
	totalMovement = 0.0;

    if (kinect.numAvailableDevices()>0){
        kinect.setRegistration(true); // enable depth->video image calibration
        kinect.init();
        kinect.open();
		kinectInitialized = true;
    } else {
		kinectInitialized = false;
		// dummy values to prevent errors/crash when kinect is not connected
    	colorPixels.allocate(2, 2, OF_IMAGE_COLOR);
    	depthPixels.allocate(2, 2, OF_IMAGE_GRAYSCALE);
		colorPixels.setColor({0, 0, 0, 1});
		depthPixels.setColor({0});

		for (int i = 0; i < totalStoredFrames; i++) {
    		previousDepthPixelsFrames[i].allocate(2, 2, OF_IMAGE_GRAYSCALE);
			previousDepthPixelsFrames[i].setColor({0});
    		previousDepthPixelsFramesInMasked[i].allocate(2, 2, OF_IMAGE_GRAYSCALE);
			previousDepthPixelsFramesInMasked[i].setColor({0});
		}
		storedFrameWidth = 2;
		storedFrameHeight = 2;

        mask.set(0, 0, 2, 2);
		return;
	}
	
    // print the intrinsic IR sensor values
    ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
    ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
    ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
    ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";

	setDepthClipping(nearClip, farClip);
    
    //colorPixels.allocate(kinect.width, kinect.height, OF_IMAGE_COLOR);
    //depthPixels.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);

	storedFrameWidth = kinect.width / storedFrameScaleFactor;
	storedFrameHeight = kinect.height / storedFrameScaleFactor;

    // determine if we use mask
	ofxXmlSettings settings;
    bool settingsExist = settings.load("settings.xml");
    cout << "Awaiting Configuration of Mask \n";
    if (settingsExist && settings.getValue("width", 0) > 2){
        // If the settings exist and are viable, use them to set the mask.
        mask.set(
           (float) settings.getValue("x_pos", 0),
           (float) settings.getValue("y_pos", 0),
           (float) settings.getValue("width", 0),
           (float) settings.getValue("height", 0)
        );
    } else {
        // Otherwise set the mask to the native kinect image dimensions (effectively no mask).
        mask.set(0, 0, kinect.width, kinect.height);
    }

	// initiallize with starting image
    previousDepthPixelsFrames[0] = kinect.getDepthPixels();
	previousDepthPixelsFramesInMasked[0] = previousDepthPixelsFrames[0];

	previousDepthPixelsFrames[0].resize(storedFrameWidth, storedFrameHeight);
	previousDepthPixelsFramesInMasked[0].crop(mask.x, mask.y, mask.width, mask.height);
	previousDepthPixelsFramesInMasked[0].resize(storedFrameWidth, storedFrameHeight);

	for (int i = 1; i < totalStoredFrames; i++) {
		previousDepthPixelsFrames[i] = previousDepthPixelsFrames[0];
		previousDepthPixelsFramesInMasked[i] = previousDepthPixelsFramesInMasked[0];
	}

	update();
}

void KinectManagerSimple::update() {
	if (!kinectInitialized || !kinect.isConnected()) {
		return;
	}
    kinect.update();

    if (kinect.isFrameNew()) {
		// Each frame takes the spot of the one after it
		previousDepthPixelsFrames.shiftBack(1);
		previousDepthPixelsFramesInMasked.shiftBack(1);

		// uses temp variables because "resize" applies to the current object
		ofPixels temp = depthPixels;
		ofPixels masked = temp;
		temp.resize(storedFrameWidth, storedFrameHeight);
		masked.crop(mask.x, mask.y, mask.width, mask.height);
		masked.resize(storedFrameWidth, storedFrameHeight);

		previousDepthPixelsFrames[0] = temp;
		previousDepthPixelsFramesInMasked[0] = masked;
        
        //NORMAL UPDATE CODE FOR GENERAL KINECT STUFFS
        colorPixels = kinect.getPixels();
        depthPixels = kinect.getDepthPixels();

		updateTotalMovement();
    }
}

void KinectManagerSimple::updateTotalMovement() {
	// To deal with flickering, only detect sustained low values (far) transitioning to sustained high values (close)
	// This means that someone just leaving frame won't be detected, but it's probably fine
	
	unsigned long totalChange = 0;
	unsigned long totalChangeMasked = 0;
	unsigned long maxChange = 65535 * (unsigned long)storedFrameHeight * storedFrameWidth;

	for (int i = 0; i < storedFrameHeight * storedFrameWidth; i++) {
		int recentVal = 65535;
		int recentValMasked = 65535;
		int referenceVal = 0;
		int referenceValMasked = 0;

		// recentVal is only large when all recent frames are large at this pixel
		for (int k = 0; k < numRecentFrames; k++) {
			recentVal = MIN(recentVal, previousDepthPixelsFrames[k][i]);
			recentValMasked = MIN(recentValMasked, previousDepthPixelsFramesInMasked[k][i]);
		}
		// referenceVal is only small when all reference frames are small at this pixel
		for (int k = numRecentFrames; k < totalStoredFrames; k++) {
			referenceVal = MAX(referenceVal, previousDepthPixelsFrames[k][i]);
			referenceValMasked = MAX(referenceValMasked, previousDepthPixelsFramesInMasked[k][i]);
		}

		totalChange += MAX(0, recentVal - referenceVal);
		totalChangeMasked += MAX(0, recentValMasked - referenceValMasked);
	}

	// Effectively makes totalMovement a rolling average, with exponentially less contribution from older frames
	totalMovement = movementTimeFactor * totalMovement + (1.0 - movementTimeFactor) * double(totalChange) / maxChange;
	totalMovementInMasked = movementTimeFactor * totalMovementInMasked + (1.0 - movementTimeFactor) * double(totalChangeMasked) / maxChange;
}

// uses 16 bits for more precision
// max (65535) is close (at nearclip), min (0) is far (at farclip)
ofShortPixels KinectManagerSimple::getDepthPixels() {
	return depthPixels;
}

// uses regular color camera on kinect, standard 8-bit color pixels
ofPixels KinectManagerSimple::getColorPixels() {
	return colorPixels;
}

// Takes a reference to an ofShortPixels object and applies threshold-based interpolation to the pixel values.
// Because it is a reference, the original object is modified and nothing is returned.
// The effect is to set all pixels below lowThresh to lowValue, all pixels above highThresh to highValue,
// and to linearly interpolate the values in between.  The threshold values are expressed as unsigned short, the same as the pixel values.
void KinectManagerSimple::thresholdInterp(ofShortPixels &pixels, unsigned short lesserThresh, unsigned short greaterThresh, unsigned short lesserValue, unsigned short greaterValue) {
	for (unsigned short &pixel : pixels) {
		if (pixel < lesserThresh) {
            // If the pixel value is below the low threshold, set it to the low value
			pixel = lesserValue;
		} else if (pixel > greaterThresh) {
            // If the pixel value is above the high threshold, set it to the high value
			pixel = greaterValue;
		} else {
			// If the pixel value is within the threshold range, apply linear interpolation
			float l = (pixel - lesserThresh) / float(greaterThresh - lesserThresh);
			pixel = l * greaterValue + (1.0f - l) * lesserValue;
		}
	}
}

// uses the build-in clipping done by ofKinect.
// these values are NOT scaled to full 16-bit range, while getPixels IS.
// therefor values used by thresholdInterp will not line up with these values.
void KinectManagerSimple::setDepthClipping(short near, short far) {
    if (near < 500 || far > 4000) {
        cout << "[notice ] KinectManagerSimple: depth clipping values outside the " <<
                "range 500 - 4000 are likely to produce noisy data" << endl;
    }

    kinect.setDepthClipping(near, far);
}

void KinectManagerSimple::crop(ofPixels &pixels) {
	pixels.crop(mask.x, mask.y, mask.width, mask.height);
}

void KinectManagerSimple::cropUsingMask(ofShortPixels &pixels) {
	pixels.crop(mask.x, mask.y, mask.width, mask.height);
}

KinectManagerSimple::~KinectManagerSimple() {
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();
}

#endif //KinectManagerSimpleSimple_cpp
