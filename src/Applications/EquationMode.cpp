//
//  EquationMode.cpp
//  neoForm
//
//  Created by Liam May on 6/25/24.
//

#include "EquationMode.hpp"
#include "ofGraphicsConstants.h"

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

	// initiallize graph, associating each vertex with a color
	// vertecies are between -1 and 1, scaled when drawing
	float scale = 2.0 / std::max(cols - 1, rows - 1);
	graph.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows; y++) {
			graph.addVertex(ofPoint(scale * (x - cols/2.0), scale * (y - rows/2.0), 0));
			float r, g, b;
			std::tie(r, g, b) = heightPixelToMapColor(0);
			graph.addColor({r, g, b});
		}
	}

	// initiallize lines between vertecies
	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows - 1; y++) {
			graph.addIndex(x * rows + y);
			graph.addIndex(x * rows + y + 1);
		}
	}

	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols - 1; x++) {
			graph.addIndex(y + x * rows);
			graph.addIndex(y + (x+1) * rows);
		}
	}

    transitioning = false;
    firstFrame = true;
    transitionFrameCount = 0;
    numFrames = 20;
}

float EquationMode::equation1(float x, float y){
    // gaussian
    float center_x = cols/2.0;
    float center_y = rows/2.0;
    float sigma = 5.0f;

    return 255.0 / 1.2 * (exp(-(((float)x - center_x) * ((float)x - center_x) + ((float)y - center_y) * ((float)y - center_y)) / (2.0 * sigma * sigma)) - exp(-(((float)cols - center_x) * ((float)cols - center_x) + ((float)rows - center_y) * ((float)rows - center_y)) / (2.0 * sigma * sigma))) / (1.0 - exp(-(((float)cols - center_x) * ((float)cols - center_x) + ((float)rows - center_y) * ((float)rows - center_y)) / (2.0 * sigma * sigma)));
}

float EquationMode::equation2(float x, float y) {
    return 127.0 / 1.5 * std::cos(sqrt((((float)x - 0.5 * (float)cols) * ((float)x - 0.5 * (float)cols)) + ((float)y - 0.5 * (float)rows) * ((float)y - 0.5 * (float)rows))) + 127.0;
}

float EquationMode::equation3(float x, float y) {
    return 127.0 + 127.0 / 1.5 * std::sin((float)x * 0.5) * std::cos((float)y * 0.5);
}

float EquationMode::equation4(float x, float y) {
    float A = 127.0;
    float B = 127.0;
    float u = (2.0 * x / cols - 1.0);
    float v = (2.0 * y / rows - 1.0);
    float value = A * (u * u - v * v);
    float normalizedValue = (value + A) * (255.0 / (2.0 * A));
    return normalizedValue;
}

float EquationMode::equation5(float x, float y) {
    float amplitude = 127.0 * 0.8;
    float frequency = 0.3;
    float offset = 127.0;
    float value = amplitude * std::sin(frequency * (x - y)) + offset;
    return std::max(0.0f, std::min(255.0f, value));
}


float EquationMode::equation6(float x, float y) {
    float frequency = 0.5;
    return 127.0 + 0.9 * 127.0 * std::sin(frequency * ((float)x + (float)y));
}




void EquationMode::update(float dt){
    timeControl++;
	graphAngle += ofGetLastFrameTime() * graphRotationSpeed;
    if (transitioning || firstFrame) {
        updateHeights();
        firstFrame = false;
    } else {
        if (timeControl % 150 == 0) {
            int curr = equationNum;
            equationNum = equationNum % 6 + 1;
            startTransition(curr, equationNum);
        }
    }
}


void EquationMode::updateHeights() {
    if (transitioning) {
        float alpha = (float)transitionFrameCount / numFrames;

        for (int x = 0; x < cols; x++) {
            for (int y = 0; y < rows; y++) {
                float value1 = (transitionEq1 == 1) ? equation1(x, y) : (transitionEq1 == 2) ? equation2(x, y) : (transitionEq1 == 3) ? equation3(x, y) : (transitionEq1 == 4) ? equation4(x, y) : (transitionEq1 == 5) ? equation5(x, y) : equation6(x, y);
                float value2 = (transitionEq2 == 1) ? equation1(x, y) : (transitionEq2 == 2) ? equation2(x, y) : (transitionEq2 == 3) ? equation3(x, y) : (transitionEq2 == 4) ? equation4(x, y) : (transitionEq2 == 5) ? equation5(x, y) : equation6(x, y);
                heights[x][y] = (1 - alpha) * value1 + alpha * value2;
            }
        }

        transitionFrameCount++;
        if (transitionFrameCount >= numFrames) {
            transitioning = false;
            transitionFrameCount = 0;
            equationNum = transitionEq2;
            updateHeights();
        }
    } else {
        if (equationNum == 1) {
            for (int x = 0; x < cols; x++) {
                for (int y = 0; y < rows; y++) {
                    heights[x][y] = equation1((float)x, (float)y);
                }
            }
        } else if (equationNum == 2) {
            for (int x = 0; x < cols; x++) {
                for (int y = 0; y < rows; y++) {
                    heights[x][y] = equation2(x, y);
                }
            }
        } else if (equationNum == 3) {
            for (int x = 0; x < cols; x++) {
                for (int y = 0; y < rows; y++) {
                    heights[x][y] = equation3((float)x, (float)y);
                }
            }
        } else if (equationNum == 4) {
            for (int x = 0; x < cols; x++) {
                for (int y = 0; y < rows; y++) {
                    heights[x][y] = equation4((float)x, (float)y);
                }
            }
        } else if (equationNum == 5) {
            for (int x = 0; x < cols; x++) {
                for (int y = 0; y < rows; y++) {
                    heights[x][y] = equation5((float)x, (float)y);
                }
            }
        } else if (equationNum == 6) {
            for (int x = 0; x < cols; x++) {
                for (int y = 0; y < rows; y++) {
                    heights[x][y] = equation6(x, y);
                }
            }
        }
    }

    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            m_EquationPixels.setColor(i, j, ofColor(heights[i][j]));
        }
    }
	
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
			auto vert = graph.getVertex(x * rows + y);
			graph.setVertex(x * rows + y, {vert.x, vert.y, graphHeight * heights[x][y] / 255.0});

			float r, g, b;
			std::tie(r, g, b) = heightPixelToMapColor(heights[x][y]);
			graph.setColor(x * rows + y, {r/255.0f, g/255.0f, b/255.0f});
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
            
        }
    }
}

//--------------------------------------------------------------------------
// Everything for graphics and mapping heights to color depth map
//--------------------------------------------------------------------------

void EquationMode::drawGraphicsForPublicDisplay(int x, int y, int width, int height) {
	float scale = std::min(width/2.0, height/2.0);
	// ofPushMatrix prevents orientation functions from effecting anything outside this function
	ofPushMatrix();

	// NOTE: openFrameworks right-multiplies matrix transformations, 
	// meaning you need to specify them in REVERSE ORDER
	// (translate -> rotate -> scale)
	//
	// align to camera nicely
	ofTranslate(x + width/2.0, y + height/2.0, -scale);
	ofRotateDeg(50, 1, 0, 0);
	ofScale(scale); // translation isn't scaled by this

	// rotate graph over time
	ofRotateDeg(graphAngle, 0, 0, 1);


	graph.draw();
	ofPopMatrix();
}

std::tuple<int, int, int> EquationMode::heightPixelToMapColor(int Height) {
    std::tuple<int, int, int> heightColors[] = {
        {0,0,255},{0,255,255},{0,255,0},{255,255,0},{255,165,0},{255,0,0}
    };
    
    float segments[] = {0.0f, 51.0f, 102.0f, 153.0f, 204.0f, 255.0f};
    
    int i;
    for (i = 0; i < 4; i++) {
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

void EquationMode::startTransition(int newEq1, int newEq2) {
    transitioning = true;
    transitionEq1 = newEq1;
    transitionEq2 = newEq2;
    transitionFrameCount = 0;
}

//
