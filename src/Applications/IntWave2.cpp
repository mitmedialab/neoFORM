//
//  IntWave2.cpp
//  neoForm
//
//  Created by Liam May on 7/01/24.
//

#include "IntWave2.hpp"
#include "ofxXmlSettings.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <algorithm>

IntWave2::IntWave2(SerialShapeIOManager *theSerialShapeIOManager, KinectManager *theKinectManager) : Application(theSerialShapeIOManager) {
    m_kinectManager = theKinectManager;
    setup();
}

void IntWave2::setup(){
    timeControl = 0;
    cols = (m_CustomShapeDisplayManager)->shapeDisplaySizeX;
    rows = (m_CustomShapeDisplayManager)->shapeDisplaySizeY;
    m_IntWavePixels.allocate(cols, rows, OF_IMAGE_GRAYSCALE);
    m_IntWavePixels.set(0);

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

    friction = 0.8f;

    for (int x = 0; x < cols; ++x) {
        for (int y = 0; y < rows; ++y) {
            double val = 0;
            bool wall = false;
            val = 50.0 * std::sin(sqrt(std::pow((float)x - (float)cols/2.0,2) + std::pow((float)y - (float)rows/2.0,2)));
            density[x][y] = 0;
            velocity[x][y] = val;
            wallMask[x][y] = wall;
        }
    }
}

float IntWave2::getAdjacencyDensitySum(int x, int y){
    float sum = 0;
    if (x - 1 >= 0 && !wallMask[x - 1][y]) { sum += density[x - 1][y]; }
    if (x + 1 < cols && !wallMask[x + 1][y]) { sum += density[x + 1][y]; }
    if (y - 1 >= 0 && !wallMask[x][y - 1]) { sum += density[x][y - 1]; }
    if (y + 1 < rows && !wallMask[x][y + 1]) { sum += density[x][y + 1]; }
    return sum;
}

void IntWave2::solveFluid(){
    for (int x = 0; x < cols; x++){
        for (int y = 0; y < rows; y++){
            if (wallMask[x][y]){ continue; }
            float adjSum = getAdjacencyDensitySum(x, y);
            float laplacian = adjSum - 4.0f * density[x][y];
            velocity[x][y] = friction * velocity[x][y] + laplacian * 0.2f;
        }
    }

    for (int x = 0; x < cols; x++){
        for (int y = 0; y < rows; y++){
            if (!wallMask[x][y]){
                density[x][y] += velocity[x][y];
            }
        }
    }
}

void IntWave2::update(float dt){
    m_kinectManager->update();
    updateMask();
    solveFluid();
    updateHeights();
    updatePreviousWallMask();
    if (timeControl%100==0){ applyRippleEffect(cols/2, rows/2);}
    timeControl++;
}

void IntWave2::updateMask(){
    ofPixels maskPixels;
    m_kinectManager->getDepthPixels(maskPixels);
    ofImage img;
    img.setFromPixels(maskPixels);
    img.resize((m_CustomShapeDisplayManager)->shapeDisplaySizeX, (m_CustomShapeDisplayManager)->shapeDisplaySizeY);
    maskPixels = img.getPixels();

    for (int x = 0; x < cols; x++){
        for (int y = 0; y < rows; y++){
            ofColor color = maskPixels.getColor(x, y);
            if (color.getBrightness() > ofColor(127, 127, 127).getBrightness()) {
                wallMask[x][y] = true;
                if (!previousWallMask[x][y]) { // Only apply ripple if it's a new mask pixel
                    applyRippleEffect(x, y);
                }
            } else {
                wallMask[x][y] = false;
            }
        }
    }
}

void IntWave2::applyRippleEffect(int x, int y) {
    float strength = 10.0f;
    int radius = 5;

    for (int i = -radius; i <= radius; ++i) {
        for (int j = -radius; j <= radius; ++j) {
            int nx = x + i;
            int ny = y + j;

            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows) {
                float distance = sqrt(i*i + j*j);
                if (distance <= radius) {
                    velocity[nx][ny] += strength * cos(distance * M_PI / (0.5 * (float)radius));
                }
            }
        }
    }
}

void IntWave2::updatePreviousWallMask() {
    for (int x = 0; x < cols; ++x) {
        for (int y = 0; y < rows; ++y) {
            previousWallMask[x][y] = wallMask[x][y];
        }
    }
}

ofxCvGrayscaleImage IntWave2::getBlurredDepthImg() {
    ofxCvGrayscaleImage blurredDepthImg = m_kinectManager->depthImg;
    blurredDepthImg.blurGaussian(101);
    return blurredDepthImg;
}

void IntWave2::updateHeights(){
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            m_IntWavePixels.setColor(x, y, ofColor(ofClamp(127 + density[x][y], 0, 255)));
        }
    }

    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            int flattenedIndex = heightsForShapeDisplay.getPixelIndex(x, y);
            heightsForShapeDisplay[flattenedIndex] = m_IntWavePixels[flattenedIndex];
        }
    }
}

void IntWave2::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    m_kinectManager->colorImg.draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());

    ofSetColor(255, 255, 255, 127);
    m_kinectManager->depthImg.draw(2, 2, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());

    ofSetColor(255, 255, 255, 127);

    drawPreviewMaskRectangle();

    ofxCvGrayscaleImage blurredDepthImg = getBlurredDepthImg();
    blurredDepthImg.draw(2, 400, m_kinectManager->getImageWidth(), m_kinectManager->getImageHeight());

    drawPreviewActuatedSections();
}

void IntWave2::drawSectionPreviewFrameBuffer(int x, int y, int width, int height){
}

void IntWave2::drawPreviewMaskRectangle() {
    ofSetColor(0, 0, 255);
    ofNoFill();
    ofSetLineWidth(5);

    ofDrawRectangle(
        m_kinectManager->m_mask.getX(),
        m_kinectManager->m_mask.getY(),
        m_kinectManager->m_mask.getWidth(),
        m_kinectManager->m_mask.getHeight()
    );

    ofSetColor(255, 255, 255);
    ofFill();
}

void IntWave2::drawPreviewActuatedSections() {
//    float transformWidth = ((TransformIOManager*)m_CustomShapeDisplayManager)->m_Transform_W;
//    float pixelsPerInch = m_kinectManager->m_mask.getWidth() / transformWidth;
//
//    std::vector<ofRectangle> sections = m_CustomShapeDisplayManager->createSections(pixelsPerInch);
//
//    ofFbo fbo;
//    fbo.allocate(m_kinectManager->m_mask.getWidth(), m_kinectManager->m_mask.getHeight(), GL_RGBA);
//
//    fbo.begin();
//    ofClear(0, 0, 0, 0);
//
//    for (const auto& section : sections) {
//        ofSetColor(100, 100, 255, 200);
//        ofDrawRectangle(section);
//    }
//
//    fbo.end();
//    fbo.draw(m_kinectManager->m_mask);
}

void IntWave2::keyPressed(int Key) {
}
