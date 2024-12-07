//
//  KinectManagerSimple.cpp
//  neoForm
//
//  Created by Charles Reischer on 12/7/24.
//

#ifndef KinectManagerSimple_cpp
#define KinectManagerSimple_cpp

#include "KinectManagerSimple.hpp"
#include "ofxXmlSettings.h"

KinectManager::KinectManager(short nearClip, short farClip) {
    if (kinect.numAvailableDevices()>0){
        kinect.setRegistration(true); // enable depth->video image calibration
        kinect.init();
        kinect.open();
		is_connected = true;
    } else {
		is_connected = false;
		// dummy values to prevent errors/crash when kinect is not connected
    	colorPixels.allocate(1, 1, OF_IMAGE_COLOR);
    	depthPixels.allocate(1, 1, OF_IMAGE_GRAYSCALE);
		colorPixels.setColor({0, 0, 0, 1});
		depthPixels.setColor({0});
        mask.set(0, 0, 1, 1);
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
    
    int imageWidth = kinect.width;
    int imageHeight = kinect.height;

    colorPixels.allocate(imageWidth, imageHeight, OF_IMAGE_COLOR);
    depthPixels.allocate(imageWidth, imageHeight, OF_IMAGE_GRAYSCALE);

    // determine if we use mask
	ofxXmlSettings settings;
    bool settingsExist = settings.load("settings.xml");
    cout << "Awaiting Configuration of Mask \n";
    if (settingsExist){
        int width  = settings.getValue("width", 0);

        if (width > 2){
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
    } else {
        // Otherwise set the mask to the native kinect image dimensions (effectively no mask).
        mask.set(0, 0, kinect.width, kinect.height);
    }
}

void KinectManager::update() {
	if (!is_connected || !kinect.isConnected()) {
		return;
	}
    kinect.update();

    if (kinect.isFrameNew()) {
        
        //NORMAL UPDATE CODE FOR GENERAL KINECT STUFFS
        colorPixels = kinect.getPixels();
        depthPixels = kinect.getDepthPixels();
    }
}

ofShortPixels KinectManager::getDepthPixels() {
	return depthPixels;
}

ofPixels KinectManager::getColorPixels() {
	return colorPixels;
}

void KinectManager::thresholdInterp(ofShortPixels &pix, unsigned short lowThresh, unsigned short highThresh, unsigned short lowValue, unsigned short highValue) {
	for (unsigned short &pixel : pix) {
		if (pixel < lowThresh) {
			pixel = lowValue;
		} else if (pixel > highThresh) {
			pixel = highValue;
		} else {
			// linear interpolation
			double l = (pixel - lowThresh) / double(highThresh - lowThresh);
			pixel = l * highValue + (1.0 - l) * lowValue;
		}
	}
}

void KinectManager::setDepthClipping(short near, short far) {
    if (near < 500 || far > 4000) {
        cout << "[notice ] KinectManager: depth clipping values outside the " <<
                "range 500 - 4000 are likely to produce noisy data" << endl;
    }

    kinect.setDepthClipping(near, far);
}

void KinectManager::crop(ofImage &image) {
	image.crop(mask.x, mask.y, mask.width, mask.width);
}

void KinectManager::crop(ofShortImage &image) {
	image.crop(mask.x, mask.y, mask.width, mask.width);
}

KinectManager::~KinectManager() {
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();
}

#endif //KinectManagerSimple_cpp
