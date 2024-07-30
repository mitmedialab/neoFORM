//
//  EquationMode.cpp
//  neoForm
//
//  Created by Liam May on 6/25/24.
//

#include "EquationMode.hpp"

#include <iostream>
#include <cmath>


EquationMode::EquationMode(SerialShapeIOManager *theCustomShapeDisplayManager) : Application(theCustomShapeDisplayManager){
    setup();
}

void EquationMode::setup(){
    
    equationNum = 1;
    timeControl = 0;
    cols = (m_CustomShapeDisplayManager)->shapeDisplaySizeX;
    rows = (m_CustomShapeDisplayManager)->shapeDisplaySizeY;
    m_EquationPixels.allocate(cols, rows, OF_IMAGE_GRAYSCALE);
    m_EquationPixels.set(0);
    ProjectorHeightMapPixels.allocate(cols*10, rows*10, OF_IMAGE_COLOR);
    ProjectorHeightMapPixels.setColor(ofColor::black);
    
    x_Rot = 0.0f;
    y_Rot = 0.0f;
    x_trans = 0.0f;
    y_trans = 0.0f;
    count = 0;
    
    heights = new float*[cols];
    for (int x = 0; x < cols; x++){
        heights[x] = new float[rows];
    }
    for (int x = 0; x < cols; x++){
        for (int y = 0; y < rows; y++){
            heights[x][y] = 0;
        }
    }
    heightsCopy = new float*[cols];
    for (int x = 0; x < cols; x++){
        heightsCopy[x] = new float[rows];
    }
    for (int x = 0; x < cols; x++){
        for (int y = 0; y < rows; y++){
            heightsCopy[x][y] = 0;
        }
    }
}

float EquationMode::equation1(float x, float y){
    // guassian
    float center = cols/2.0;
    float sigma = 5.0f;
    
    return 255.0/1.5 * (exp(-(((float)x - center) * ((float)x - center) + ((float)y - center) * ((float)y - center)) / (2.0 * sigma * sigma)) - exp(-(((float)cols - center) * ((float)cols - center) + ((float)cols - center) * ((float)cols - center)) / (2.0 * sigma * sigma))) / (1.0 - exp(-(((float)cols - center) * ((float)cols - center) + ((float)cols - center) * ((float)cols - center)) / (2.0 * sigma * sigma)));
}

float EquationMode::equation2(float x, float y){
    return 127.0/1.5 * std::cos(sqrt((((float)x - 0.5 * (float)cols)*((float)x - 0.5 * (float)cols)) + ((float)y - 0.5 * (float)cols)*((float)y - 0.5 * (float)cols))) + 127.0;
}

float EquationMode::equation3(float x, float y) {
    return 127.0 + 127.0/1.5 * std::sin((float)x * 0.5) * std::cos((float)y * 0.5);
}


void EquationMode::update(float dt){
    updateHeights();
    timeControl++;
    if (timeControl % 100 == 0){
        equationNum = equationNum%3 +1;
    }
}

void EquationMode::rotate(float theta){ // note that theta is in radians
    for (int x = 0; x < cols; x++){
        for (int y = 0; y < rows; y++){
            heightsCopy[x][y] = heights[x][y];
        }
    } // dummy copy of heights
    
    for (int x = 0; x < cols; x++){
        for (int y = 0; y < rows; y++){
            
            x_trans = (float)x - 0.5 * (float)cols;
            y_trans = (float)y - 0.5 * (float)cols; // center around (0,0), then rotate
            
            x_Rot = x_trans*std::cos((float)count * theta)-y_trans*std::sin((float)count * theta); //(x,y) rotated by theta degrees
            y_Rot = x_trans*std::sin((float)count * theta)+y_trans*std::cos((float)count * theta);
            
            x_Rot += 0.5 * (float)cols; // undo translation
            y_Rot += 0.5 * (float)cols;
            
            if (equationNum ==1){heights[x][y] = equation1(x_Rot, y_Rot);} // equivalent to rotating by **negative theta
            else if (equationNum == 2){heights[x][y] = equation2(x_Rot, y_Rot);}
            else if (equationNum == 3){heights[x][y] = equation3(x_Rot, y_Rot);}
            count++;
        }
    }
}

void EquationMode::updateHeights(){
    if (equationNum == 1){
        for (int x = 0; x < cols; x++){
            for (int y = 0; y < rows; y++){
                heights[x][y] = equation1((float)x,(float)y);
            }
        }
    } else if (equationNum == 2){
        for (int x = 0; x < cols; x++){
            for (int y = 0; y < rows; y++){
                heights[x][y] = equation2(x,y);
            }
        }
    } else if (equationNum == 3){
        for (int x = 0; x < cols; x++){
            for (int y = 0; y < rows; y++){
                heights[x][y] = equation3((float)x, (float)y);
            }
        }
    }
    for (int i = 0; i < cols; i++){
        for (int j = 0; j < rows; j++){
            m_EquationPixels.setColor(i, j, ofColor(heights[i][j]));
        }
    }
    
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            
            int flattenedIndex = heightsForShapeDisplay.getPixelIndex(x, y);
            
            heightsForShapeDisplay[flattenedIndex] = m_EquationPixels[flattenedIndex];
        }
    }
    
    for (int x = 0; x < cols*10; x++) {
        for (int y = 0; y < rows*10; y++) {
            int r, g, b;
            if (equationNum == 1){
                std::tuple<int, int, int> projector_color = heightPixelToMapColor(equation1((float)x/10.0, (float)y/10.0));
                std::tie(r, g, b) = projector_color;
                ProjectorHeightMapPixels.setColor(x, y, ofColor(r, g, b));
            }
            else if (equationNum == 2){
                std::tuple<int, int, int> projector_color = heightPixelToMapColor(equation2((float)x/10.0, (float)y/10.0));
                std::tie(r, g, b) = projector_color;
                ProjectorHeightMapPixels.setColor(x, y, ofColor(r, g, b));
            }
            else if (equationNum == 3){
                std::tuple<int, int, int> projector_color = heightPixelToMapColor(equation3((float)x/10.0, (float)y/10.0));
                std::tie(r, g, b) = projector_color;
                ProjectorHeightMapPixels.setColor(x, y, ofColor(r, g, b));
            }
            
        }
    }
    
//    rotate(1.0/200000.0);
}

//--------------------------------------------------------------------------
// Everything for graphics and mapping heights to color depth map
//--------------------------------------------------------------------------

std::tuple<int, int, int> EquationMode::heightPixelToMapColor(int Height) {
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

void EquationMode::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    // Draw the current video frame as a base; .
//    m_EquationImage.setFromPixels(m_EquationPixels);
//    m_EquationImage.draw(30, 300, 544, 128);
    ofImage(heightsForShapeDisplay).draw(x, y, width, height);
    ofImage(ProjectorHeightMapPixels).draw(1,1,600,800);
    
    
}

void EquationMode::drawSectionPreviewFrameBuffer(int x, int y, int width, int height) {
    
}


//
