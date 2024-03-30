//
//  KinectDebugApp.cpp
//  neoForm
//
//  Created by admin on 10/18/22.
//

#include "KinectDebugApp.hpp"

#include "ofxXmlSettings.h"

KinectDebugApp::KinectDebugApp(KinectManager* kinectManager){
    m_kinectManager = kinectManager;
    
    // TEMP FIX: PREINITIALIZE WIDTH AND HEIGHT OF MASK
    m_kinectManager->m_mask.width = 100;
    m_kinectManager->m_mask.height = 100;
    // END TEMP FIX: hope to figure this out

    setupDepthFloorMap();
    
}

void KinectDebugApp::setup() {
    setupDepthFloorMap();
}

void KinectDebugApp::setupDepthFloorMap() {
    //set all pins to 0
    for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {

        for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
            
            // This takes the 2 dimensional coordinates and turns them into a one dimensional index for the flattened array.
            int flattenedIndex = heightsForShapeDisplay.getPixelIndex(x, y);
            
            // This takes the 1 dimensional index for the pin, and grabs the corresponding index from the uncorrected video pixel array.
            heightsForShapeDisplay[flattenedIndex] = 0;
        }
    }
    
    // record all pins live pixels
    ofPixels m_videoPixels = m_kinectManager->getCroppedPixels((m_kinectManager->depthPixels));//video.getPixels();
    
    // Extact only the actuated part of the surface from the full depthmap;
    // save all pin lives pixels
    rawSurfaceDepth = getActuatedPixelsFromFullTransformSurface( m_videoPixels );

    
    cout << "setup the depth floor map w00t";
}

void KinectDebugApp::update(float dt) {
    cout << "we updatin";
    //update();
    
    m_kinectManager->update();
    
    updateHeights();
  //  if (!m_save_flag){
  //      saveShapeDisplayBoundingBox();
  //  }
  //  m_save_flag = 1;
}

// Draw the depth data into the app preview window.
void KinectDebugApp::drawGraphicsForShapeDisplay(int x, int y, int width, int height){
    cout << "they see us rollin, they hatin";
    //ofImage(m_kinectManager->depthPixels).draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
    
    m_kinectManager->drawContours();
    
    // Get the raw depth data cropped to the dimensions of the surface mask.
    
    
    cout << m_kinectManager->depthPixels.getWidth() << endl;
    ofPixels bob;
    
    m_kinectManager->depthPixels.cropTo(bob, m_kinectManager->m_mask.getX(), m_kinectManager->m_mask.getY(), m_kinectManager->m_mask.width, m_kinectManager->m_mask.height);
    
    cout << bob.getWidth() << endl;
    
    ofImage(bob).draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());

}

void KinectDebugApp::keyPressed(int key){
    
    if (key=='s'){
        m_kinectManager->saveShapeDisplayBoundingBox();
    }
    
    
}

ofPixels KinectDebugApp::getActuatedPixelsFromFullTransformSurface( ofPixels fullSurface ) {
    int xAccumPixels = 0;
    
    
    //dead
    int z0PixelsW = (int) (m_kinectManager->m_Transform_L_outer*(m_kinectManager->m_mask.width/m_kinectManager->m_Transform_W));
    //alive
    int z1PixelsW = (int) (m_kinectManager->m_Transform_block*(m_kinectManager->m_mask.width/m_kinectManager->m_Transform_W));
    //dead
    int z2PixelsW = (int) (m_kinectManager->m_Transform_L_inner*(m_kinectManager->m_mask.width/m_kinectManager->m_Transform_W));
    //alive
    int z3PixelsW = (int) (m_kinectManager->m_Transform_block*(m_kinectManager->m_mask.width/m_kinectManager->m_Transform_W));
    //dead
    int z4PixelsW = (int) (m_kinectManager->m_Transform_R_inner*(m_kinectManager->m_mask.width/m_kinectManager->m_Transform_W));
    //alive
    int z5PixelsW = (int) (m_kinectManager->m_Transform_block*(m_kinectManager->m_mask.width/m_kinectManager->m_Transform_W));
    //dead
    int z6PixelsW = (int) (m_kinectManager->m_Transform_R_outer*(m_kinectManager->m_mask.width/m_kinectManager->m_Transform_W));
    
    // allocate all zones
    ofPixels zone0, zone1, zone2, zone3, zone4, zone5, zone6;
    
    zone0.allocate((int)z0PixelsW, (int)(m_kinectManager->m_mask.height), OF_IMAGE_GRAYSCALE);
    zone1.allocate((int)z1PixelsW, (int)(m_kinectManager->m_mask.height), OF_IMAGE_GRAYSCALE);
    zone2.allocate((int)z2PixelsW, (int)(m_kinectManager->m_mask.height), OF_IMAGE_GRAYSCALE);
    zone3.allocate((int)z3PixelsW, (int)(m_kinectManager->m_mask.height), OF_IMAGE_GRAYSCALE);
    zone4.allocate((int)z4PixelsW, (int)(m_kinectManager->m_mask.height), OF_IMAGE_GRAYSCALE);
    zone5.allocate((int)z5PixelsW, (int)(m_kinectManager->m_mask.height), OF_IMAGE_GRAYSCALE);
    zone6.allocate((int)z6PixelsW, (int)(m_kinectManager->m_mask.height), OF_IMAGE_GRAYSCALE);
    
    // for video cropping, need to know x offset
    int z0start = 0;
    int z1start = z0PixelsW;
    int z2start = z1PixelsW + z1start;
    int z3start = z2PixelsW + z2start;
    int z4start = z3PixelsW + z3start;
    int z5start = z4PixelsW + z4start;
    int z6start = z5PixelsW + z5start;
    
    //croppage to video slices
    fullSurface.cropTo(zone0, z0start,0, (int)z0PixelsW, (int)m_kinectManager->m_mask.height); //dead
    fullSurface.cropTo(zone1, z1start,0, (int)z1PixelsW, (int)m_kinectManager->m_mask.height); //alive
    fullSurface.cropTo(zone2, z2start,0, (int)z2PixelsW, (int)m_kinectManager->m_mask.height); //dead
    fullSurface.cropTo(zone3, z3start,0, (int)z3PixelsW, (int)m_kinectManager->m_mask.height); //alive
    fullSurface.cropTo(zone4, z4start,0, (int)z4PixelsW, (int)m_kinectManager->m_mask.height); //dead
    fullSurface.cropTo(zone5, z5start,0, (int)z5PixelsW, (int)m_kinectManager->m_mask.height); //alive
    fullSurface.cropTo(zone6, z6start,0, (int)z6PixelsW, (int)m_kinectManager->m_mask.height); //dead
    
    //draw cropped videoslices
    //DeadandAliveBlocks.draw(zone0,0,0);
    
    // Create image objects for each of the live zones.
    ofImage imgZone1, imgZone3, imgZone5;
    
    imgZone1.allocate(z0PixelsW, (int)m_kinectManager->m_mask.height, OF_IMAGE_GRAYSCALE);
    imgZone3.allocate(z3PixelsW, (int)m_kinectManager->m_mask.height, OF_IMAGE_GRAYSCALE);
    imgZone5.allocate(z5PixelsW, (int)m_kinectManager->m_mask.height, OF_IMAGE_GRAYSCALE);
    
    imgZone1.setFromPixels(zone1);
    imgZone3.setFromPixels(zone3);
    imgZone5.setFromPixels(zone5);
    
    // Create a frame buffer to deal with the spacing of gray "dead" zones and white "alive pin zones" on the display
    // and ensure that the mapping of the cropped kinect depth pixels places their mapping in the right locations on the shape display
    ofFbo DeadandAliveBlocks;
    DeadandAliveBlocks.allocate((int) (z1PixelsW+z3PixelsW+z5PixelsW), (int) m_kinectManager->m_mask.height, GL_RGB);
    //DeadandAliveBlocks.activateAllDrawBuffers();
    DeadandAliveBlocks.begin();
    
    
    //smash them together
    // Draw the isolated live zone images into the frame buffer.
    imgZone1.draw(0,0);
    imgZone3.draw(z1start, 0);
    imgZone5.draw(z2start, 0);
  //  imgZone5.draw(z5start, 0);
    
    
    
    // Close out the frame buffer for drawing and extract the resulting image.
    DeadandAliveBlocks.end();
    
    //DeadandAliveBlocks.draw(500, 500);
    
    ofPixels liveZonesPix;
    liveZonesPix.allocate((int) (z1PixelsW+z3PixelsW+z5PixelsW), (int)m_kinectManager->m_mask.height, OF_IMAGE_GRAYSCALE);
    //liveZonesPix.setImageType(OF_IMAGE_GRAYSCALE);
        
    DeadandAliveBlocks.readToPixels(liveZonesPix, 0);
    
    DeadandAliveBlocks.clear();

    //ofImage liveZonesImg;
    
    //liveZonesImg.setFromPixels(liveZonesPix); //liveZonesPix, 48, 24, OF_IMAGE_GRAYSCALE);
    //liveZonesImg.setFromPixels(liveZonesPix, 48, 24, OF_IMAGE_GRAYSCALE);
    
    

    //rerender them and resize
    
    
    // Grayscale ensures that there is only one brightness value per pixel (instead of three channel RGB).
    //m_videoPixels.setImageType(OF_IMAGE_GRAYSCALE);
    
    //  float scaledW = 102.0/m_Transform_W;
    //float scaledH = 24.0/m_Transform_H;
    
    
    
    //m_videoPixels.resize(48,24);
    //m_videoPixels.rotate90(2);
    
    
    liveZonesPix.setImageType(OF_IMAGE_GRAYSCALE);
    liveZonesPix.resize(48,24);
    liveZonesPix.rotate90(2);
    
    return liveZonesPix;
}


void KinectDebugApp::updateHeights() {
    
    if (m_kinectManager->useMask == false){
        
    }else{
    
    
        // Get pixel values from the video and map them to pin heights here.
        // m_videoPixels is the stored pixels from the current video frame, stored in this app header.
        
        
        // Uses the opencv cropped contour rectangle to crop the kinect depth pixels to only be in the size and shape of the transform
        ofPixels m_videoPixels = m_kinectManager->getCroppedPixels((m_kinectManager->depthPixels));//video.getPixels();
        
        // Extact only the actuated part of the surface from the full depthmap;
        ofPixels livePixels = getActuatedPixelsFromFullTransformSurface( m_videoPixels );

            
        float tempSum = 0;
       
        for (int x = 0; x < SHAPE_DISPLAY_SIZE_X; x++) {

            for (int y = 0; y < SHAPE_DISPLAY_SIZE_Y; y++) {
                
                // This takes the 2 dimensional coordinates and turns them into a one dimensional index for the flattened array.
                int flattenedIndex = heightsForShapeDisplay.getPixelIndex(x, y);
                
                // This takes the 1 dimensional index for the pin, and grabs the corresponding index from the uncorrected video pixel array.
                heightsForShapeDisplay[flattenedIndex] = livePixels[flattenedIndex];
                //m_kinectManager->m_kinectToTransformIndicies[flattenedIndex]];
                
                // TEMPORARY TODO THRESHOLDER FOR ZEROS
                
                if (rawSurfaceDepth[flattenedIndex] >0){
                    if ((livePixels[flattenedIndex] <= rawSurfaceDepth[flattenedIndex] +9) && ((livePixels[flattenedIndex] >= rawSurfaceDepth[flattenedIndex] -9))){
                        heightsForShapeDisplay[flattenedIndex] = 0;
                    }if(livePixels[flattenedIndex] >rawSurfaceDepth[flattenedIndex] +9){
                        heightsForShapeDisplay[flattenedIndex] = (int) roundf(heightsForShapeDisplay[flattenedIndex]*1.8)-rawSurfaceDepth[flattenedIndex];
                    }
                   // if (heightsForShapeDisplay[flattenedIndex]>(m_transformStartHeight+50)){
                   //     heightsForShapeDisplay[flattenedIndex]=0;
                   // }
                    //temp way to remove crazy edges
                    
                }
                
                tempSum = tempSum+ (float)livePixels[flattenedIndex];
                
                //heightsForShapeDisplay[flattenedIndex] = m_videoPixels[flattenedIndex];//m_kinectManager->m_kinectToTransformIndicies[flattenedIndex]];
            }
        }
        
        tempSum = tempSum/1024.0;
        m_transformStartHeight = (int)tempSum;
        
        //cout << "lololol" << m_transformStartHeight;
    }
  


}
