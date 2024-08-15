//
//  InteractiveWave.cpp
//  neoForm
//
//  Created by Liam May on 6/24/24.
//

#include "InteractiveWave.hpp"
#include "ofxXmlSettings.h"
#include <opencv2/imgproc.hpp>

#include <algorithm>

InteractiveWave::InteractiveWave(SerialShapeIOManager *theSerialShapeIOManager, KinectManager *theKinectManager) : Application(theSerialShapeIOManager) {
    m_kinectManager = theKinectManager;
    setup();
}


void InteractiveWave::setup(){
    
    cols = (m_CustomShapeDisplayManager)->shapeDisplaySizeX;
    rows = (m_CustomShapeDisplayManager)->shapeDisplaySizeY;
    m_IntWavePixels.allocate(cols, rows, OF_IMAGE_GRAYSCALE);
    m_IntWavePixels.set(0);
    
    timeControl = 0;
    
    density = new float*[cols];
    velocity = new float*[cols];
    wallMask = new bool*[cols];
    previousWallMask = new bool*[cols];
    
    for (int x = 0; x < cols; x++){
        density[x] = new float[rows];
        velocity[x] = new float[rows];
        wallMask[x] = new bool[rows];
        previousWallMask[x] = new bool[rows];
    }
    
    friction = 0.802;
    
    // gives initial impulses to the water, and sets wallMask to all False
    for (int x = 0; x < cols; ++x) {
        for (int y = 0; y < rows; ++y) {
            double val = 0;
            bool wall = false;
            if (x == cols / 4 || x == 3 * cols / 4 || x == cols / 2) {
                val = 500 * std::sin(1.5 * y * 2 * M_PI / rows);
            }
            if (y == rows / 4 || y == 3 * rows / 4 || y == rows / 2) {
                val = 500 * std::sin(1.5 * x * 2 * M_PI / cols);
            }
            //if setting up an initial wall, put logic here:
            
            density[x][y] = 0;
            velocity[x][y] = val;
            wallMask[x][y] = wall;
            previousWallMask[x][y] = wall;
        }
    }
    
    ProjectorHeightMapPixels.allocate(cols, rows, OF_IMAGE_COLOR);
    ProjectorHeightMapPixels.setColor(ofColor::black);
}


//----------------------------------------------------
// Put the functions for the wave operations here:
//----------------------------------------------------

float InteractiveWave::getAdjacencyDensitySum(int x, int y){
    float sum = 0;
    if (x - 1 >= 0 and not wallMask[x - 1][y]){sum += density[x - 1][y];}
    if (x + 1 < cols and not wallMask[x + 1][y]){sum += density[x + 1][y];}
    if (y - 1 >= 0 and not wallMask[x][y - 1]){sum += density[x][y - 1];}
    if (y + 1 < rows and not wallMask[x][y + 1]){sum += density[x][y + 1];}
    return sum;
}

void InteractiveWave::solveFluid(){
    for (int x = 0; x < cols; x++){
        for (int y = 0; y < rows; y++){
//        if (wallMask[x][y]){continue;}
        velocity[x][y] = friction * velocity[x][y] + (getAdjacencyDensitySum(x, y) - density[x][y] * 4.0) * 0.1;
        density[x][y] = density[x][y] + velocity[x][y];
        }
    }
    
//    ofxCvGrayscaleImage blurredDepthImg = getBlurredDepthImg();
}



//----------------------------------------------------
// Put the functions for the Kinect input --> wall mask here:
//----------------------------------------------------

void InteractiveWave::update(float dt){
    
    m_kinectManager->update();
    updateMask();
    solveFluid();
    updateHeights();
    updatePreviousWallMask();
    timeControl++;
    
}

void InteractiveWave::updateMask(){
    ofPixels maskPixels;
    m_kinectManager->getDepthPixels(maskPixels);
    ofImage img;
    img.setFromPixels(maskPixels);
    img.resize((m_CustomShapeDisplayManager)->shapeDisplaySizeX, (m_CustomShapeDisplayManager)->shapeDisplaySizeY);
    maskPixels = img.getPixels();
    
    int counter = 0;

    if (timeControl%6 == 0) {
        for (int x = 0; x < cols; x++){
            for (int y = 0; y < rows; y++){
                ofColor color = maskPixels.getColor(x, y);
                if (color.getBrightness() > ofColor(127, 127, 127).getBrightness()) {
                    wallMask[x][y] = true;
                    if (!previousWallMask[x][y]) { // Only apply ripple if it's a new mask pixel
                        if (counter % 20 == 0) {applyRippleEffect(x, y);}
                        counter++;
                    }
                } else {
                    wallMask[x][y] = false;
                }
            }
        }
    }
    if (timeControl%100 <= 5){
        applyRippleEffect(cols/2, rows/2);
        std::cout <<"x";
    }
}

void InteractiveWave::applyRippleEffect(int x, int y) {
    int strength = 100;
    int radius = 3;

    for (int i = -radius; i <= radius; ++i) {
        for (int j = -radius; j <= radius; ++j) {
            int nx = x + i;
            int ny = y + j;

            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows) {
                float distance = sqrt(i*i + j*j);
                if (distance <= (float)radius) {
                    velocity[nx][ny] += (int)(strength * cos(distance * M_PI / (0.5 * (float)radius)));
                }
            }
        }
    }
}

void InteractiveWave::updatePreviousWallMask() {
    for (int x = 0; x < cols; ++x) {
        for (int y = 0; y < rows; ++y) {
            previousWallMask[x][y] = wallMask[x][y];
        }
    }
}

ofxCvGrayscaleImage InteractiveWave::getBlurredDepthImg() {
    ofxCvGrayscaleImage blurredDepthImg = m_kinectManager->depthImg;
    blurredDepthImg.blurGaussian(100);
    
    
    
    return blurredDepthImg;
}

void InteractiveWave::updateHeights(){
    
    // amplify heights under mask
//    for (int i = 0; i < cols; i++){
//        for (int j = 0; j < rows; j++){
//            if (wallMask[i][j]){
//                if (1.25 * density[i][j] <= 255){
//                    density[i][j] = 1.25 * density[i][j];
//                } else {density[i][j] = 255;}
//            }
//        }
//    }
    
    
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            m_IntWavePixels.setColor(x, y, ofColor(ofClamp(127 + velocity[x][y], 0, 255)));
        }
    }
    
    
//    ofPixels livePixels = m_CustomShapeDisplayManager->cropToActiveSurface(m_IntWavePixels);
   
    
    
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            
            int flattenedIndex = heightsForShapeDisplay.getPixelIndex(x, y);
            
            heightsForShapeDisplay[flattenedIndex] = m_IntWavePixels[flattenedIndex];
            
            std::tuple<int, int, int> projector_color = heightPixelToMapColor(m_IntWavePixels[flattenedIndex]);
            int r, g, b;
            std::tie(r, g, b) = projector_color;
            ProjectorHeightMapPixels.setColor(x, y, ofColor(r, g, b));
        }
    }
    heightsForShapeDisplay.rotate90(2);
    
}


std::tuple<int, int, int> InteractiveWave::heightPixelToMapColor(int Height) {
    std::tuple<int, int, int> heightColors[] = {
        {0,0,255},{0,255,255},{0,255,0},{255,255,0},{255,165,0},{255,0,0}
    };

    float segments[] = {0.0f, 51.0f, 102.0f, 153.0f, 204.0f, 255.0f};

    int i;
    for (i = 0; i < 5; i++) {
        if (Height < segments[i+1]) break;
    }
    float ratio = (Height - segments[i]) / (segments[i+1] - segments[i]);
    int r = std::get<0>(heightColors[i]) + ratio * (std::get<0>(heightColors[i+1]) - std::get<0>(heightColors[i]));
    int g = std::get<1>(heightColors[i]) + ratio * (std::get<1>(heightColors[i+1]) - std::get<1>(heightColors[i]));
    int b = std::get<2>(heightColors[i]) + ratio * (std::get<2>(heightColors[i+1]) - std::get<2>(heightColors[i]));

    return std::make_tuple(r, g, b);
}
    
void InteractiveWave::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
//    //*** Draw the color pixels for reference.
//    m_kinectManager->colorImg.draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
//
//    //*** Overlay the depth image on top of the color image.
////    Set the color to white with 50% opacity
//    ofSetColor(255, 255, 255, 127);
//
//    // Draw the depth image
//    m_kinectManager->depthImg.draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
//
//    // Reset the color to fully opaque white
//    ofSetColor(255, 255, 255, 127);
//
////    //*** Draw the mask rectangle
//    drawPreviewMaskRectangle();
////
////    //*** Preview shape display pixels
//    ofxCvGrayscaleImage blurredDepthImg = getBlurredDepthImg();
//    blurredDepthImg.draw(2, 400, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
////
////    //*** Contours are disabled, but maybe they will be useful in the future.
////    m_kinectManager->drawContours();
////
////    //*** Draw preview of the actuated pixel regions (sections).
//    drawPreviewActuatedSections();
//
////    ofImage(heightsForShapeDisplay).draw(30, 300, width, height);
    ofImage(ProjectorHeightMapPixels).draw(1,1,600,800);
}

void InteractiveWave::drawSectionPreviewFrameBuffer(int x, int y, int width, int height){
    
}

void InteractiveWave::drawPreviewMaskRectangle() {
//    //     Set the drawing parameters
//        ofSetColor(0, 0, 255);
//        ofNoFill();
//        ofSetLineWidth(5);
//
//        // Draw the rectangle with the dimensions of the mask.
//        ofDrawRectangle(
//                        m_kinectManager->m_mask.getX(),
//                        m_kinectManager->m_mask.getY(),
//                        m_kinectManager->m_mask.getWidth(),
//                        m_kinectManager->m_mask.getHeight()
//        );
//
//        // Unset color and fill for future drawing operations
//        ofSetColor(255, 255, 255);
//        ofFill();
}

void InteractiveWave::drawPreviewActuatedSections() {
//    // Get the width in inches of the the full transform surface (need to cast shape display manager object first).
//    float transformWidth = ((TransformIOManager*)m_CustomShapeDisplayManager)->m_Transform_W;
//
//    // Get the actuated section dimensions from the CustomShapeDisplayManager
//    float pixelsPerInch = m_kinectManager->m_mask.getWidth() / transformWidth;
//
//    std::vector<ofRectangle> sections = m_CustomShapeDisplayManager->createSections(pixelsPerInch);
//
//    // Create a frame buffer with the same dimensions as the cropped signal.
//    ofFbo fbo;
//    fbo.allocate(m_kinectManager->m_mask.getWidth(), m_kinectManager->m_mask.getHeight(), GL_RGBA); // GL_RGBA for transparency
//
//    // Begin drawing into the frame buffer
//    fbo.begin();
//    ofClear(0, 0, 0, 0); // Clear the buffer with transparent black
//
//    // Draw each of the section rectangles into the frame buffer
//    for (const auto& section : sections) {
//        ofSetColor(100,100,255,200);
//        ofDrawRectangle(section);
//    }
//
//    // End drawing into the frame buffer
//    fbo.end();
//    fbo.draw(m_kinectManager->m_mask);
}

void InteractiveWave::keyPressed(int Key) {
    
}
