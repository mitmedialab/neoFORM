//
//  WaveModeContours.cpp
//  neoForm
//
//  Created by Liam May on 8/9/24.
//

#include "WaveModeContours.hpp"
#include "ofxXmlSettings.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <vector>

#include <algorithm>

WaveModeContours::WaveModeContours(SerialShapeIOManager *theSerialShapeIOManager, KinectManager *theKinectManager) : Application(theSerialShapeIOManager) {
    m_kinectManager = theKinectManager;
    setup();
}


void WaveModeContours::setup(){
    
    cols = (m_CustomShapeDisplayManager)->shapeDisplaySizeX;
    rows = (m_CustomShapeDisplayManager)->shapeDisplaySizeY;
    m_IntWavePixels.allocate(cols, rows, OF_IMAGE_GRAYSCALE);
    m_IntWavePixels.set(0);
    
    timeControl = 0;
    
    density = new float*[cols];
    velocity = new float*[cols];
    wallMask = new bool*[cols];
    previousWallMask = new bool*[cols];
    
    contourFinder;
    lastContourCentroids;
    
    for (int x = 0; x < cols; x++){
        density[x] = new float[rows];
        velocity[x] = new float[rows];
        wallMask[x] = new bool[rows];
        previousWallMask[x] = new bool[rows];
    }
    
    friction = 0.8;
    
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

float WaveModeContours::getAdjacencyDensitySum(int x, int y){
    float sum = 0;
    if (x - 1 >= 0 and not wallMask[x - 1][y]){sum += density[x - 1][y];}
    if (x + 1 < cols and not wallMask[x + 1][y]){sum += density[x + 1][y];}
    if (y - 1 >= 0 and not wallMask[x][y - 1]){sum += density[x][y - 1];}
    if (y + 1 < rows and not wallMask[x][y + 1]){sum += density[x][y + 1];}
    return sum;
}

void WaveModeContours::solveFluid(){
    for (int x = 0; x < cols; x++){
        for (int y = 0; y < rows; y++){
//        if (wallMask[x][y]){continue;}
        velocity[x][y] = friction * velocity[x][y] + (getAdjacencyDensitySum(x, y) - density[x][y] * 4.0) * 0.1;
        density[x][y] = density[x][y] + velocity[x][y];
        }
    }
}


void WaveModeContours::update(float dt){
    
    m_kinectManager->update();
    updateMask();
    solveFluid();
    updateHeights();
    updatePreviousWallMask();
    timeControl++;
    
}

void WaveModeContours::updateMask(){
    depthImg = m_kinectManager->croppedDepthImg;
    depthImg.blurGaussian(1);
    contourFinder.findContours(depthImg, 100, (m_kinectManager->imageWidth)*(m_kinectManager->imageHeight) / 2, 1, false);
    
    maskPixels = depthImg.getPixels();
    std::vector<ofPoint> currentCentroids;
    
    float dist;
    int numBlobs = contourFinder.nBlobs;
    for (int i = 0; i < numBlobs; i++) {
        ofxCvBlob blob = contourFinder.blobs[i];
        ofPoint centroid = blob.centroid;
        currentCentroids.push_back(centroid);
        
        int center_x = centroid.x;
        int center_y = centroid.y;
        float area = blob.area;
        float length = blob.length;

        int maskWidth = maskPixels.getWidth();
        for (int index = 0; index < maskPixels.size(); index ++) {
            int x = index % maskWidth;
            int y = index / maskWidth;
            dist = sqrt((x - center_x)*(x - center_x) + (y - center_y)*(y - center_y));
            if (dist == 0) {
                maskPixels.setColor(x, y, 255);
            }
        }
    }
    
    ofImage img;
    img.setFromPixels(maskPixels);
    img.resize((m_CustomShapeDisplayManager)->shapeDisplaySizeX, (m_CustomShapeDisplayManager)->shapeDisplaySizeY);
    maskPixels = img.getPixels();
    
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            ofColor color = maskPixels.getColor(x, y);
            if (color.getBrightness() > ofColor(0, 0, 0).getBrightness()) {
                wallMask[x][y] = true;
                if (not previousWallMask[x][y]) {
                    bool newPositionDetected = false;
                    for (const auto& lastCentroid : lastContourCentroids) {
                        float distance = ofDist(x, y, lastCentroid.x, lastCentroid.y);
                        if (distance > 20) {
                            newPositionDetected = true;
                            break;
                        }
                    }
                    
                    if (newPositionDetected) {
                        handInteraction(x, y);
                    }
                }
            } else { wallMask[x][y] = false; }
        }
    }
    
    if (timeControl % 100 <= 5) {
        applyRippleEffect(cols / 2, rows / 2);
    }
    updatePreviousWallMask();
    lastContourCentroids = currentCentroids;
}

void WaveModeContours::applyRippleEffect(int x, int y) {
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

void WaveModeContours::handInteraction(int x, int y) {
    int radius = 3;
    float strength = 10.0;

    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
            int nx = x + i;
            int ny = y + j;
            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows) {
                float distance = sqrt(i*i + j*j);
                if (distance <= (float)radius) {
                    float influence = exp(-distance * distance / (2 * radius * radius));
                    velocity[nx][ny] += strength * influence;
                }
            }
        }
    }
}


void WaveModeContours::updatePreviousWallMask() {
    for (int x = 0; x < cols; ++x) {
        for (int y = 0; y < rows; ++y) {
            previousWallMask[x][y] = wallMask[x][y];
        }
    }
}

ofxCvGrayscaleImage WaveModeContours::getBlurredDepthImg() {
    ofxCvGrayscaleImage blurredDepthImg = m_kinectManager->depthImg;
    blurredDepthImg.blurGaussian(1);

    return blurredDepthImg;
}

void WaveModeContours::updateHeights(){

    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            m_IntWavePixels.setColor(x, y, ofColor(ofClamp(127 + velocity[x][y], 0, 255)));
        }
    }
    
   
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
    heightsForShapeDisplay.rotate90(2);     // this may not be necessary in museum depending on which way kinect is installed
}


std::tuple<int, int, int> WaveModeContours::heightPixelToMapColor(int Height) {
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
    
void WaveModeContours::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    //*** Draw the color pixels for reference.
//    m_kinectManager->colorImg.draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());

    //*** Overlay the depth image on top of the color image.
//    Set the color to white with 50% opacity
//    ofSetColor(255, 255, 255, 127);

    // Draw the depth image
    m_kinectManager->depthImg.draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());

    // Reset the color to fully opaque white
    ofSetColor(255, 255, 255, 127);

//    //*** Draw the mask rectangle
    drawPreviewMaskRectangle();
//
//    //*** Preview shape display pixels
//    ofxCvGrayscaleImage blurredDepthImg = getBlurredDepthImg();
//    blurredDepthImg.draw(2, 400, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());
//
//    //*** Contours are disabled, but maybe they will be useful in the future.
    contourFinder.draw(0, 0, 640, 480);
    ofColor c(255, 255, 255);
    ofSetLineWidth(3);
    ofNoFill();
    
    for (int i = 0; i < contourFinder.nBlobs; i++) {
        ofRectangle r = contourFinder.blobs.at(i).boundingRect;
        c.setHsb(i * 64, 255, 255);
        ofSetColor(c);
        ofDrawRectangle(r);
        if (m_contoursRecordedFlag < 1) {
            m_capturedContours.push_back(r);
        }
    }
    m_contoursRecordedFlag = 1;
//
//    //*** Draw preview of the actuated pixel regions (sections).
    drawPreviewActuatedSections();

//    ofImage(heightsForShapeDisplay).draw(30, 300, width, height);
//    ofImage(ProjectorHeightMapPixels).draw(1,1,600,800);
//    NOTE: uncomment the above line to get the projector heatmap back
}

void WaveModeContours::drawSectionPreviewFrameBuffer(int x, int y, int width, int height){
    
}

void WaveModeContours::drawPreviewMaskRectangle() {
    //     Set the drawing parameters
        ofSetColor(0, 0, 255);
        ofNoFill();
        ofSetLineWidth(5);

        // Draw the rectangle with the dimensions of the mask.
        ofDrawRectangle(
                        m_kinectManager->m_mask.getX(),
                        m_kinectManager->m_mask.getY(),
                        m_kinectManager->m_mask.getWidth(),
                        m_kinectManager->m_mask.getHeight()
        );

        // Unset color and fill for future drawing operations
        ofSetColor(255, 255, 255);
        ofFill();
}

void WaveModeContours::drawPreviewActuatedSections() {
    // Get the width in inches of the the full transform surface (need to cast shape display manager object first).
    float transformWidth = ((TransformIOManager*)m_CustomShapeDisplayManager)->m_Transform_W;

    // Get the actuated section dimensions from the CustomShapeDisplayManager
    float pixelsPerInch = m_kinectManager->m_mask.getWidth() / transformWidth;

    std::vector<ofRectangle> sections = m_CustomShapeDisplayManager->createSections(pixelsPerInch);

    // Create a frame buffer with the same dimensions as the cropped signal.
    ofFbo fbo;
    fbo.allocate(m_kinectManager->m_mask.getWidth(), m_kinectManager->m_mask.getHeight(), GL_RGBA); // GL_RGBA for transparency

    // Begin drawing into the frame buffer
    fbo.begin();
    ofClear(0, 0, 0, 0); // Clear the buffer with transparent black

    // Draw each of the section rectangles into the frame buffer
    for (const auto& section : sections) {
        ofSetColor(100,100,255,200);
        ofDrawRectangle(section);
    }

    // End drawing into the frame buffer
    fbo.end();
    fbo.draw(m_kinectManager->m_mask);
}

void WaveModeContours::keyPressed(int Key) {
    
}
