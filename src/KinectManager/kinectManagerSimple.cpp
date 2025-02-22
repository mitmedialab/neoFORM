//
//  KinectManagerSimpleSimple.cpp
//  neoForm
//
//  Created by Charles Reischer on 12/7/24.
//

#ifndef KinectManagerSimple_cpp
#define KinectManagerSimple_cpp

#include "KinectManagerSimple.hpp"
#include "ofGLUtils.h"
#include "ofxXmlSettings.h"

KinectManagerSimple::KinectManagerSimple() {
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

	// always try loading clip values from settings
	nearClip = settings.getValue("near_clip", 800);
	farClip = settings.getValue("far_clip", 3600);
	nearThreshold = settings.getValue("near_threshold", 65535);

	setDepthClipping(nearClip, farClip, nearThreshold);
    
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
		unfilteredDepthPixels = kinect.getDepthPixels();
		thresholdInterp(depthPixels, 0, nearThreshold, 0, 65535);

		updateTotalMovement();

		reverseParallax(farClip);
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
// nearClp and farClp are NOT scaled to full 16-bit range (instead in physical mm), while getPixels & nearThresh ARE.
// therefor values used by thresholdInterp will not line up with these values.
void KinectManagerSimple::setDepthClipping(unsigned short nearClp, unsigned short farClp, unsigned short nearThresh) {
    if (nearClp < 500 || farClp > 4000) {
        cout << "[notice ] KinectManagerSimple: depth clipping values outside the " <<
                "range 500 - 4000 are likely to produce noisy data" << endl;
    }

	nearClip = nearClp;
	farClip = farClp;
	nearThreshold = nearThresh;
    kinect.setDepthClipping(nearClp, farClp);
}

void KinectManagerSimple::cropUsingMask(ofPixels &pixels) {
	pixels.crop(mask.x, mask.y, mask.width, mask.height);
}

void KinectManagerSimple::cropUsingMask(ofShortPixels &pixels) {
	pixels.crop(mask.x, mask.y, mask.width, mask.height);
}

void KinectManagerSimple::saveMaskAndClip() {
	// write the mask settings to file, in case anything modified it
	ofxXmlSettings settings;
    settings.load("settings.xml");

    settings.setValue("x_pos", mask.x);
    settings.setValue("y_pos", mask.y);
    settings.setValue("width", mask.width);
    settings.setValue("height", mask.height);
	settings.setValue("near_clip", nearClip);
	settings.setValue("far_clip", farClip);
	settings.setValue("near_threshold", nearThreshold);

	settings.save("settings.xml");
}

KinectManagerSimple::~KinectManagerSimple() {
    kinect.close();
}

std::pair<int, int> calculateUnparallaxedPixelCoord(const int x, const int y, const int imageWidth, const int imageHeight, const float dist, const float slopeParallaxPerPixel, const float targetPlaneWidth, const float targetPlaneHeight) {
	// units are:   pixels  *  [unitless slope]/pixels  *  millimeters
	// resulting unit is millimeters
	// (0.0, 0.0) is center of image, NOT top left
	double trueXPos = (x - imageWidth / 2) * slopeParallaxPerPixel * dist;
	double trueYPos = (y - imageHeight / 2) * slopeParallaxPerPixel * dist;

	// back to pixels, still has (0, 0) as center 
	int unparallaxedX = trueXPos * imageWidth / targetPlaneWidth;
	int unparallaxedY = trueYPos * imageHeight / targetPlaneHeight;

	// back to having (0, 0) as top left
	return {unparallaxedX + imageWidth / 2, unparallaxedY + imageHeight / 2};
}

bool coordinateInBox(int x, int y, int width, int height) {
	return (x >= 0 && x < width && y >= 0 && y < height);
}

ofShortPixels KinectManagerSimple::getUnfilteredDepthPixels() {
	return unfilteredDepthPixels;
}

void KinectManagerSimple::reverseParallax(float targetPlaneDepth) {
	// targetPlaneDepth is in millimeters
	//
	// pixels at targetPlaneDepth will not be moved
	// pixels farther than targetPlaneDepth will be moved outwards from the center to compinsate
	// pixels closer than targetPlaneDepth will be moved inwards.
	// pixels not moved to (i.e. blocked by something else) are left at 0 (far away)
	// overlapping pixels (i.e. one over the other, but angle prevents obscuring) take the max (close) value
	//
	// view plane is 21.5 inches wide at 17.5 inches from sensor
	// total field of view (horizontal) is therefor about 63 degrees or 1.1 radians
	// the kinect already corrects in terms of depth, only the position within the image needs to be changed
	
	// represents the slope (distance from center) at a given depth, PER PIXEL
	// (i.e. 2 pixels away from the center the slope will be 2 * slopeParallaxPerPixel) 
	double slopeParallaxPerPixel = (21.5/17.5) / depthPixels.getWidth();

	int imageWidth = depthPixels.getWidth();
	int imageHeight = depthPixels.getHeight();

	// target plane dimensions in millimeters
	double targetPlaneWidth = imageWidth * slopeParallaxPerPixel * targetPlaneDepth;
	double targetPlaneHeight = imageHeight * slopeParallaxPerPixel * targetPlaneDepth;

	// store intermediate values for each pixel, to be referenced later
	//std::vector<int> previousColumnTrueXPositions = std::vector<int>(imageHeight);
	//std::vector<int> previousColumnTrueYPositions = std::vector<int>(imageHeight);
	//std::vector<unsigned short> previousColumnDepth = std::vector<unsigned short>(imageHeight);

	ofShortPixels unparallaxed;
	ofFloatPixels distances = kinect.getDistancePixels(); // physical distances in millimeters
	// allocate and set to all 0 (far away)
	unparallaxed.allocate(imageWidth, imageHeight, ofImageType::OF_IMAGE_GRAYSCALE);
	unparallaxed.set(0);

	for (int y = 0; y < imageHeight; y++) {
		for (int x = 0; x < imageWidth; x++) {
			int trueX, trueY;
			int index = unparallaxed.getPixelIndex(x, y);

			float dist = distances[index];
			std::tie(trueX, trueY) = calculateUnparallaxedPixelCoord(x, y, imageWidth, imageHeight, dist, slopeParallaxPerPixel, 
																				targetPlaneWidth, targetPlaneHeight);
			unsigned short depth = depthPixels[depthPixels.getPixelIndex(x, y)];

			// skip if pixel would be outside image (farther than torgetPlane and towards edge)
			if (!coordinateInBox(trueX, trueY, imageWidth, imageHeight)) continue;

			int trueIndex = unparallaxed.getPixelIndex(trueX, trueY);
			unparallaxed[trueIndex] = std::max(unparallaxed[trueIndex], depth);
		}
	}

	depthPixels = unparallaxed;
}

#endif //KinectManagerSimpleSimple_cpp
