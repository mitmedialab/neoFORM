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
        mask.set(0, 0, 2, 2);
		return;
	}
	
    // print the intrinsic IR sensor values
    if (kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }

	setDepthClipping(nearClip, farClip);
    
    colorPixels.allocate(kinect.width, kinect.height, OF_IMAGE_COLOR);
    depthPixels.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);

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
}

void KinectManagerSimple::update() {
	if (!kinectInitialized || !kinect.isConnected()) {
		return;
	}
    kinect.update();

    if (kinect.isFrameNew()) {
        
        //NORMAL UPDATE CODE FOR GENERAL KINECT STUFFS
        colorPixels = kinect.getPixels();
        depthPixels = kinect.getDepthPixels();
    }
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
void KinectManagerSimple::thresholdInterp(ofShortPixels &pixels, unsigned short lowThresh, unsigned short highThresh, unsigned short lowValue, unsigned short highValue) {
	for (unsigned short &pixel : pixels) {
		if (pixel < lowThresh) {
            // If the pixel value is below the low threshold, set it to the low value
			pixel = lowValue;
		} else if (pixel > highThresh) {
            // If the pixel value is above the high threshold, set it to the high value
			pixel = highValue;
		} else {
			// If the pixel value is within the threshold range, apply linear interpolation
			float l = (pixel - lowThresh) / float(highThresh - lowThresh);
			pixel = l * highValue + (1.0f - l) * lowValue;
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
