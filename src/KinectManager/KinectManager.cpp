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
        
        
        if (contourTrackingOn) {
            // SPECIAL UPDATE CODE FOR CONTOUR STUFFS
            colorImg.setFromPixels(kinect.getPixels());//, kinect.width, kinect.height);
            depthImg.setFromPixels(kinect.getDepthPixels()); //, kinect.width, kinect.height);
            
            lastDepthThreshed.setFromPixels(depthThreshed.getPixels());//, kinect.width, kinect.height);
            // always update the depth image
            depthThreshed.setFromPixels(depthImg.getPixels());//, kinect.width, kinect.height);
            
                
            if (!m_configConfirmed) {
                
            // determine if we use mask
                bool settingsExist = settings.loadFile("settings.xml");
                cout << "Awaiting Configuration of Mask \n";
                if (settingsExist){
                    int width  = settings.getValue("width", 0);

                    if (width < 2){
                        useMask = false;
                    } else {
                        // If the settings exist and are viable, use them to set the mask.
                        m_mask.set(
                           (float) settings.getValue("x_pos", 0),
                           (float) settings.getValue("y_pos", 0),
                           (float) settings.getValue("width", 0),
                           (float) settings.getValue("height", 0)
                       );
                        
                        useMask = true;
                        m_configConfirmed=true;
                    }
                }
                
            }
                
            // subtract mask which is png alpha image called "mask.png"
            if(useMask) {subtractMask();}

        }
                
        // threshold calcutations convery depth map into black and white images
        calculateThresholdsAndModifyImages();
        
            
            cout << "CONTOUR FINDER NOW\n";
            
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // if find holes is set to true, we will get interior contours as well.
        contourFinder.findContours(depthImg, mContourMinimumSize, (kinect.width*kinect.height)/2, 10, false);
        
        depthThreshedDiff.absDiff(lastDepthThreshed, depthThreshed);
        }
        
        
    }
//}

void KinectManager::subtractMask(){
    //cvAnd(depthImg.getCvImage(), maskCv.getCvImage(), depthImg.getCvImage(), NULL);
    //mask.loadImage("mask.png");
    
    ofImage img;
        
    int counter = 0;
    
    int w=640;
    int h = 480;
    img.allocate(w, h, OF_IMAGE_COLOR);
    img.setColor(ofColor::white);
    for (int i = 0; i < w; i++) {
        ofColor color= ofColor(255,255,255);
        for (int j = 0; j < h; j++) {
            if ((i>= (int) m_mask.position.x) and (i<= (int) m_mask.position.x + (int) m_mask.width) and (j>= (int) m_mask.position.y) and (j<= (int) m_mask.position.y + (int) m_mask.height)){
                color= ofColor(255,255,255);
            }
            else{
                color = ofColor(0,0,0);
            }
            
            //ofColor color= ofColor(255,255,255);//(255-i%w,j%h,255);
            img.setColor(i % w, j % h, color);
        }
    }
    img.update();
    
    maskColorCv.setFromPixels(img.getPixels());
    maskCv = maskColorCv;
    
    
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
        //colorPixels.rotate90(2);
        //depthPixels.rotate90(2);
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
    // Draw a preview of the contours that opencv identified.
    contourFinder.draw(0,0,640,480);
    
    // Draw solid bounding boxes for each contour blob in the contour finder
    ofColor c(255, 255, 255);
    ofSetLineWidth(3);
    ofNoFill();
    
    for(int i = 0; i < contourFinder.nBlobs; i++) {
       ofRectangle r = contourFinder.blobs.at(i).boundingRect;
       //r.x += 320; r.y += 240;
       c.setHsb(i * 64, 255, 255);
       ofSetColor(c);
       ofDrawRectangle(r);
        if (m_contoursRecordedFlag<1){
            m_capturedContours.push_back(r);
        }
   }
    m_contoursRecordedFlag = 1;
}

std::vector<int> KinectManager::getBigBoundingRectValues(std::vector<ofRectangle> theBlobs){
    std::vector<int> output;
    
    // Iterate through all the blobs looking for the one that most closely matches the aspect ratio of the transform surface.
    
    // Declaring iterator to a vector
    vector<ofRectangle>::iterator ptr;
    
    ofRectangle chosenOne; // The closest match will be the "chosenOne".
    float chosenError;
    float transformRatio = 4.03;  // This is the ratio for TRANSFORM, other shape displays will need a different constant.
    float ratio = 99999999; // larger than life
    float error = 99999999;
    
    
    for (ptr = theBlobs.begin(); ptr < theBlobs.end(); ptr++) {
        ratio = ptr->getWidth()/ptr->getHeight();
        error = abs(transformRatio - ratio);
        
        if (error<chosenError){
            chosenOne = *ptr;
            chosenError = error;
        }
    }
    
    output.push_back((int)chosenOne.getX());
    output.push_back((int)chosenOne.getY());
    output.push_back((int)chosenOne.getWidth());
    output.push_back((int)chosenOne.getHeight());

    // Return the dimensions of the "chosenOne" blob as the one that is probably the TANSFORM surface.
    return output;
}

vector<int> KinectManager::saveShapeDisplayBoundingBox(){
    
    std::vector<int> theRectDims = getBigBoundingRectValues(getBlobs());
    
    ofxXmlSettings settings;
    settings.loadFile("settings.xml");
    settings.setValue("x_pos", theRectDims.at(0));
    settings.setValue("y_pos", theRectDims.at(1));
    settings.setValue("width", theRectDims.at(2));
    settings.setValue("height", theRectDims.at(3));
    settings.saveFile("settings.xml");
    
    return theRectDims;
    
}



std::vector<ofRectangle> KinectManager::getBlobs(){
    return m_capturedContours;
}







