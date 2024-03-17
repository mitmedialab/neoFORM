//
//  TransformIOManager.cpp
//  emptyExample
//
//  Created by Daniel Levine on 5/19/21.
//

#include "TransformIOManager.hpp"

// Create new transformIOManager instance, setting up transFORM-specific board
// configuration
TransformIOManager::TransformIOManager() {
    configureBoards();
    setupTransformedPixelMap();
}

TransformIOManager::TransformIOManager(KinectManager* kinectRef) {
    configureBoards();
    setupTransformedPixelMap();
    
    m_kinectManagerRef = kinectRef;
}

// setup transFORM-specific board configuration
void TransformIOManager::configureBoards() {
    // set up coordinates for
    for (int i = 0; i < NUM_ARDUINOS; i++) {
        // determine which serial connection each board is on:
        // every 3rd and 4th board is on the second
        if (i < 64) {
            pinBoards[i].serialConnection = ((i / 2) % 2 == 0) ? 0 : 1;
        } else if (i < 128) {
            pinBoards[i].serialConnection = ((i / 2) % 2 == 0) ? 2 : 3;
        } else {
            pinBoards[i].serialConnection = ((i / 2) % 2 == 0) ? 4 : 5;
        }
        
        // every 5th to 8th board is mounted upside down, so invert the height
        pinBoards[i].invertHeight = ((i / 4) % 2 == 0) ? false : true;
        
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            int currentRow = (int)(i / 4);
            int currentColumn = j + (i % 4 * 6);
            pinBoards[i].heights[j] = 0;
            pinBoards[i].pinCoordinates[j][0] = currentRow;
            pinBoards[i].pinCoordinates[j][1] = currentColumn;
        }
        
        if ((i / 2) % 2 == 0) {
            int pinCoordinateRows[NUM_PINS_ARDUINO];
            
            // invert pin order if the boards are mounted rotated
            for (int count = 0; count < NUM_PINS_ARDUINO; count++) {
                pinCoordinateRows[NUM_PINS_ARDUINO - count - 1] = pinBoards[i].pinCoordinates[count][1];
            }
            for (int count = 0; count < NUM_PINS_ARDUINO; count++) {
                pinBoards[i].pinCoordinates[count][1] = pinCoordinateRows[count];
            }
            
            // also invert the pin height again if they are:
            pinBoards[i].invertHeight = !pinBoards[i].invertHeight;
        }
        
        // last, orient the x-y coordinate axes to the desired external axes
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            unsigned char j0 = pinBoards[i].pinCoordinates[j][0];
            unsigned char j1 = pinBoards[i].pinCoordinates[j][1];
            pinBoards[i].pinCoordinates[j][0] = SHAPE_DISPLAY_SIZE_X - 1 - j0;
            pinBoards[i].pinCoordinates[j][1] = SHAPE_DISPLAY_SIZE_Y - 1 - j1;
        }
    }

    // flag configuration as complete
    boardsAreConfigured = true;
}

// Dead/Alive block calculations: may move to a hardware specific class.
int TransformIOManager::calculateTransformWithinBlockX(int blockNumber, int x_pixel_coord) {
    int blockDead = blockNumber%2;
    
    // Find actual location on TRANSFORM grid from video
    // NOTE TRANSFORM display is 48 pixels wide.
    // NOTE video pixels are 102 wide with 5 pixel buffer on left side. (hence 107 below)
    // right side also has a 5 pixel buffer, but doesn't matter for our caculation
    int TRANSFORM_x = (int)((48.0/107.0)*(float)(x_pixel_coord+5));
    
    if (blockDead){
        //we discard the dead block
        TRANSFORM_x = 0;
    }
    
    return TRANSFORM_x;
}

// Given an x coordinate from the video (102 pixels wide),
// We chop in into 6 segments
// Segments that are used for actuation are 2, 4, 6
// Other segments are not used by TRANSFORM
// so 102 divided into 6 segments, means each segment is 17 pixels wide
// + 5 pixels on each border
// The returned block number is the number of the block where the
// video pixel input to the function ends up.
// blocks 1,3,5, useless
// blocks 2,4,6 used in transform
int TransformIOManager::calculateTransformBlockNumber(int x_pixel_coord){
    //note: int divison returns a truncated result that looks like a floor:
    // 1/2 would return 0, 4/2.5 would return 1
    int blockNum = ((x_pixel_coord+5)/16)+1;
    
    return blockNum;
}

// Runs only once on initialization.
// Fills the m_videoToTransformIndicies array with a map between flattened TRANSFORM pin numbers
// and flattened video pixels.
// m_videoToTransformIndicies(flattened TRANSFORM pixel index) = (flattened video pixel index)
// To assign TRANSFORM pixel video pixels values in the update, do something like
// (pseudocode) ==> TRANSFORM_Pin_Height(pin1) = m_video_toTransformIndicies(pin1);
void TransformIOManager::setupTransformedPixelMap(){
    int counter = 0;
    
    // Iterate over all pixels in the video frame.
    for (int y = 0; y < 24; y++) {
        for (int x = 0; x < 102; x++) {
            int blockAliveXCoord = calculateTransformWithinBlockX(calculateTransformBlockNumber(x),x);
            
            if (blockAliveXCoord){
                m_videoToTransformIndicies[counter] = 102*y+x;
                counter++;
                cout << "one made it \n";
            }
        }
    }
}


ofPixels TransformIOManager::getPinPixelsOnly(ofPixels fullPixels){
    ofPixels lalalala;
    return lalalala;
}


int* TransformIOManager::getPixelsToShapeDisplayIndicies(){
    return m_videoToTransformIndicies;
}

//DAN AND JONTHAN
ofPixels SerialShapeIOManager::getPinPixelsOnly(ofPixels snobby){
    cout << "get me transformey pixels matey\n";
    return snobby;
}

ofPixels TransformIOManager::getKinectStream(){
    m_kinectManagerRef->update();
    
    //basic test
    // Uses the opencv cropped contour rectangle to crop the kinect depth pixels to only be in the size and shape of the transform
    ofPixels m_videoPixels = m_kinectManagerRef->getCroppedPixels((m_kinectManagerRef->depthPixels));//video.getPixels();

    
    return m_videoPixels;
}

ofPixels TransformIOManager::getActuatedPixelsFromFullTransformSurface( ofPixels fullSurface ) {
    int xAccumPixels = 0;
    
    
    //dead
    int z0PixelsW = (int) (m_Transform_L_outer*(fullSurface.getWidth()/m_Transform_W));
    //alive
    int z1PixelsW = (int) (m_Transform_block*(fullSurface.getWidth()/m_Transform_W));
    //dead
    int z2PixelsW = (int) (m_Transform_L_inner*(fullSurface.getWidth()/m_Transform_W));
    //alive
    int z3PixelsW = (int) (m_Transform_block*(fullSurface.getWidth()/m_Transform_W));
    //dead
    int z4PixelsW = (int) (m_Transform_R_inner*(fullSurface.getWidth()/m_Transform_W));
    //alive
    int z5PixelsW = (int) (m_Transform_block*(fullSurface.getWidth()/m_Transform_W));
    //dead
    int z6PixelsW = (int) (m_Transform_R_outer*(fullSurface.getWidth()/m_Transform_W));
    
    // allocate all zones
    ofPixels zone0, zone1, zone2, zone3, zone4, zone5, zone6;
    
    zone0.allocate((int)z0PixelsW, (int)(fullSurface.getHeight()), OF_IMAGE_GRAYSCALE);
    zone1.allocate((int)z1PixelsW, (int)(fullSurface.getHeight()), OF_IMAGE_GRAYSCALE);
    zone2.allocate((int)z2PixelsW, (int)(fullSurface.getHeight()), OF_IMAGE_GRAYSCALE);
    zone3.allocate((int)z3PixelsW, (int)(fullSurface.getHeight()), OF_IMAGE_GRAYSCALE);
    zone4.allocate((int)z4PixelsW, (int)(fullSurface.getHeight()), OF_IMAGE_GRAYSCALE);
    zone5.allocate((int)z5PixelsW, (int)(fullSurface.getHeight()), OF_IMAGE_GRAYSCALE);
    zone6.allocate((int)z6PixelsW, (int)(fullSurface.getHeight()), OF_IMAGE_GRAYSCALE);
    
    // for video cropping, need to know x offset
    int z0start = 0;
    int z1start = z0PixelsW;
    int z2start = z1PixelsW + z1start;
    int z3start = z2PixelsW + z2start;
    int z4start = z3PixelsW + z3start;
    int z5start = z4PixelsW + z4start;
    int z6start = z5PixelsW + z5start;
    
    //croppage to video slices
    fullSurface.cropTo(zone0, z0start,0, (int)z0PixelsW, (int)fullSurface.getHeight()); //dead
    fullSurface.cropTo(zone1, z1start,0, (int)z1PixelsW, (int)fullSurface.getHeight()); //alive
    fullSurface.cropTo(zone2, z2start,0, (int)z2PixelsW, (int)fullSurface.getHeight()); //dead
    fullSurface.cropTo(zone3, z3start,0, (int)z3PixelsW, (int)fullSurface.getHeight()); //alive
    fullSurface.cropTo(zone4, z4start,0, (int)z4PixelsW, (int)fullSurface.getHeight()); //dead
    fullSurface.cropTo(zone5, z5start,0, (int)z5PixelsW, (int)fullSurface.getHeight()); //alive
    fullSurface.cropTo(zone6, z6start,0, (int)z6PixelsW, (int)fullSurface.getHeight()); //dead
    
    //draw cropped videoslices
    //DeadandAliveBlocks.draw(zone0,0,0);
    
    // Create image objects for each of the live zones.
    ofImage imgZone1, imgZone3, imgZone5;
    
    imgZone1.allocate(z1PixelsW, (int)fullSurface.getHeight(), OF_IMAGE_GRAYSCALE);
    imgZone3.allocate(z3PixelsW, (int)fullSurface.getHeight(), OF_IMAGE_GRAYSCALE);
    imgZone5.allocate(z5PixelsW, (int)fullSurface.getHeight(), OF_IMAGE_GRAYSCALE);
    
    imgZone1.setFromPixels(zone1);
    imgZone3.setFromPixels(zone3);
    imgZone5.setFromPixels(zone5);
    
    // Create a frame buffer to deal with the spacing of gray "dead" zones and white "alive pin zones" on the display
    // and ensure that the mapping of the cropped kinect depth pixels places their mapping in the right locations on the shape display
    ofFbo DeadandAliveBlocks;
    DeadandAliveBlocks.allocate((int) (z1PixelsW+z3PixelsW+z5PixelsW), (int) fullSurface.getHeight(), GL_RGB);
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
    liveZonesPix.allocate((int) (z1PixelsW+z3PixelsW+z5PixelsW), (int)fullSurface.getHeight(), OF_IMAGE_GRAYSCALE);
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

