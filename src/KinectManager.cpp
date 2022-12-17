//
//  KinectManager.cpp
//  neoForm
//
//  Created by admin on 8/15/22.
//

#include "KinectManager.hpp"

KinectManager::KinectManager(){
    if (kinect.numAvailableDevices()>0){
        kinect.setRegistration(true); // enable depth->video image calibration
        kinect.init();
        kinect.open();
    }
    
    // print the intrinsic IR sensor values
    if (kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }

    imageWidth = kinect.width;
    imageHeight = kinect.height;

    colorPixels.allocate(imageWidth, imageHeight, OF_IMAGE_COLOR);
    depthPixels.allocate(imageWidth, imageHeight, OF_IMAGE_GRAYSCALE);
}

KinectManager::~KinectManager() {
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();
}

void KinectManager::update() {
    kinect.update();

    if (isFrameNew()) {
        colorPixels.setFromPixels(kinect.getPixels(), kinect.width, kinect.height, OF_IMAGE_COLOR);
        depthPixels.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);

        orientInputImages();
    }
}

void KinectManager::orientInputImages() {
        colorPixels.rotate90(2);
        depthPixels.rotate90(2);
}

void KinectManager::getColorPixels(ofPixels &pixels) {
    pixels = colorPixels;
}

void KinectManager::getDepthPixels(ofPixels &pixels) {
    pixels = depthPixels;
}

void KinectManager::setDepthClipping(int near, int far) {
    if (near < 500 || far > 4000) {
        cout << "[notice ] KinectManager: depth clipping values outside the " <<
                "range 500 - 4000 are likely to produce noisy data" << endl;
    }

    kinect.setDepthClipping(near, far);
}

int KinectManager::numAvailableDevices(){
    return kinect.numAvailableDevices();
}

bool KinectManager::isFrameNew() {
    return kinect.isFrameNew();
}

bool KinectManager::isConnected() {
    return kinect.isConnected();
}

int KinectManager::getImageWidth() {
    return imageWidth;
}

int KinectManager::getImageHeight() {
    return imageHeight;
}
