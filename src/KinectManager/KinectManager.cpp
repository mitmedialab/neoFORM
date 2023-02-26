//
//  KinectManager.cpp
//  neoForm
//
//  Created by admin on 8/15/22.
//

#include "KinectManager.hpp"

KinectManager::KinectManager(){
    baseSetup();
    contourTrackingOn = false;

}

//Special Contour tracking constructor
KinectManager::KinectManager(int pNearThreshold, int pFarThreshold, int pContourMinimumSize){
    baseSetup();
    
    contourTrackingOn = true;

    
    colorImg.allocate(kinect.width, kinect.height);
    depthImg.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    depthThreshed.allocate(kinect.width, kinect.height);
    lastDepthThreshed.allocate(kinect.width, kinect.height);
    depthThreshedDiff.allocate(kinect.width, kinect.height);
    fbo.allocate(kinect.width*2, kinect.height, GL_RGB);
    recordingImage.allocate(kinect.width*2, kinect.height, OF_IMAGE_COLOR);
    playingImage.allocate(kinect.width*2, kinect.height, OF_IMAGE_COLOR);
    
    mNearThreshold = pNearThreshold;
    mFarThreshold = pFarThreshold;
    mContourMinimumSize = pContourMinimumSize;
    
    isCurrentlyRecording = false;
    playFromRecording = false;
    
    loadAlphaMaskAndPrepForCvProcessing();
}

void KinectManager::loadAlphaMaskAndPrepForCvProcessing(){
    // type is OF_IMAGE_COLOR_ALPHA
    mask.loadImage("mask.png");
    
    // simple way to convert to differe image type,
    // changing the transparent areas to white
    ofImage image;
    image.setFromPixels(mask.getPixelsRef());
    image.setImageType(OF_IMAGE_COLOR);
    maskColorCv.setFromPixels(image.getPixels());
    maskCv = maskColorCv;
}

void KinectManager::baseSetup(){
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
        
        //NORMAL UPDATE CODE FOR GENERAL KINECT STUFFS
        //colorPixels.setFromPixels(kinect.getPixels(), kinect.width, kinect.height, OF_IMAGE_COLOR);
        //depthPixels.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
        colorPixels = kinect.getPixels();
        depthPixels = kinect.getDepthPixels();

        orientInputImages();
        
        
        if (contourTrackingOn){
        // SPECIAL UPDATE CODE FOR CONTOUR STUFFS
        colorImg.setFromPixels(kinect.getPixels());//, kinect.width, kinect.height);
        depthImg.setFromPixels(kinect.getDepthPixels()); //, kinect.width, kinect.height);
        
        lastDepthThreshed.setFromPixels(depthThreshed.getPixels());//, kinect.width, kinect.height);
        // always update the depth image
        depthThreshed.setFromPixels(depthImg.getPixels());//, kinect.width, kinect.height);
        
        // subtract mask which is png alpha image called "mask.png"
        if(useMask) subtractMask();
                
        // threshold calcutations convery depth map into black and white images
        calculateThresholdsAndModifyImages();
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // if find holes is set to true, we will get interior contours as well.
        contourFinder.findContours(depthImg, mContourMinimumSize, (kinect.width*kinect.height)/2, 10, false);
        
        depthThreshedDiff.absDiff(lastDepthThreshed, depthThreshed);
        }
    }
}

void KinectManager::subtractMask(){
    cvAnd(depthImg.getCvImage(), maskCv.getCvImage(), depthImg.getCvImage(), NULL);
}

void KinectManager::calculateThresholdsAndModifyImages(){
    depthImg.erode_3x3();
    depthImg.dilate_3x3();
    
    // we do two thresholds - one for the far plane and one for the near plane
    // we then do a cvAnd to get the pixels which are a union of the two thresholds
    grayThreshNear = depthImg;
    grayThreshFar = depthImg;
    grayThreshNear.threshold(mNearThreshold, true);
    grayThreshFar.threshold(mFarThreshold);
    cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), depthImg.getCvImage(), NULL);
    
    // find depth map excluding thresholded data
    // this causes the 10 finger effect and could be related to our discussion
    // today about dynamic thresholding
    //
    // if we threshold with the near value, and the user moves the hand just past the near point
    // and thus out of range
    // their hand will be black (since black is used for out of range areas)
    // however since their hands shadow is also black this will cause the 10 finger effect.
    //
    //cvAnd(grayThreshNear.getCvImage(), depthThreshed.getCvImage(), depthThreshed.getCvImage(), NULL);
    cvAnd(grayThreshFar.getCvImage(), depthThreshed.getCvImage(), depthThreshed.getCvImage(), NULL);
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

void KinectManager::drawContours(){
    contourFinder.draw(0,0,200,100);
}
