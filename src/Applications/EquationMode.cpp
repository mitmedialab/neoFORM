//
//  EquationMode.cpp
//  neoForm
//
//  Created by Liam May on 6/25/24.
//

#include "EquationMode.hpp"
#include "ofAppRunner.h"
#include "ofConstants.h"
#include "ofGraphics.h"
#include "ofGraphicsConstants.h"
#include "ofTexture.h"

#include <iostream>
#include <sstream>
#include <cmath>
#include <stdexcept>


EquationMode::EquationMode(SerialShapeIOManager *theCustomShapeDisplayManager) : Application(theCustomShapeDisplayManager){
    setup();
}

void EquationMode::publicDisplaySetup() {
	// allows mipmaps
	ofDisableArbTex();
	for (int i = 0; i < numEquations; i++) {
		equationImages.push_back(ofImage());
		stringstream name;
		name << "EquationModeImages/Equation_" << i + 1 << ".png";
		equationImages[i].load(name.str());
		equationImages[i].getTexture().enableMipmap();
		equationImages[i].getTexture().generateMipmap();
		equationImages[i].getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	}
}

void EquationMode::setup(){
    equationIndex = 0;
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
	int graphCols = graphDetailMultple * cols;
	int graphRows = graphDetailMultple * rows;
	float scale = 2.0 / std::max(graphCols - 1, graphRows - 1);
	graph.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
	graphBacking.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
	for (int x = 0; x < graphCols; x++) {
		for (int y = 0; y < graphRows; y++) {
			graph.addVertex(ofPoint(scale * (x - graphCols/2.0), scale * (y - graphRows/2.0), 0));
			graphBacking.addVertex(ofPoint(scale * (x - graphCols/2.0), scale * (y - graphRows/2.0), -backingDistance));
			graph.addColor(heightPixelToMapColor(0));
			graphBacking.addColor(ofColor::black);
		}
	}

	// initiallize lines between vertecies
	for (int x = 0; x < graphCols; x++) {
		for (int y = 0; y < graphRows - 1; y++) {
			graph.addIndex(x * graphRows + y);
			graph.addIndex(x * graphRows + y + 1);
		}
	}

	for (int y = 0; y < graphRows; y++) {
		for (int x = 0; x < graphCols - 1; x++) {
			graph.addIndex(y + x * graphRows);
			graph.addIndex(y + (x+1) * graphRows);
		}
	}

	// initiallize triangles in backing
	for (int x = 0; x < graphCols - 1; x++) {
		for (int y = 0; y < graphRows - 1; y++) {
			//triangle 1
			graphBacking.addIndex(y + x * graphRows);
			graphBacking.addIndex((y+1) + x * graphRows);
			graphBacking.addIndex((y+1) + (x+1) * graphRows);
			//triangle 2
			graphBacking.addIndex(y + x * graphRows);
			graphBacking.addIndex(y + (x+1) * graphRows);
			graphBacking.addIndex((y+1) + (x+1) * graphRows);
		}
	}

    transitioning = false;
    firstFrame = true;
    transitionFrameCount = 0;
    numFrames = 20;
}

float EquationMode::equation1(float x, float y){
    // gaussian
	float scale = std::max(cols, rows) / 3.0;
    float eq_x = (x- cols/2.0)/scale;
    float eq_y = (y- rows/2.0)/scale;

	return 255.0 * exp(-(eq_x * eq_x + eq_y * eq_y)) / exp(0);
}

float EquationMode::equation2(float x, float y) {
	float scale = std::max(cols, rows) / 3.0;
    float eq_x = (x- cols/2.0)/scale;
    float eq_y = (y- rows/2.0)/scale;
	float radius = sqrt(eq_x * eq_x + eq_y * eq_y);

    return 127.0 * std::cos(8 * radius) + 127.0;
}

float EquationMode::equation3(float x, float y) {
    return 127.0 + 127.0 * std::sin((float)x * 0.5) * std::cos((float)y * 0.5);
}

float EquationMode::equation4(float x, float y) {
    float eq_x = (2.0 * x / cols - 1.0);
    float eq_y = (2.0 * y / rows - 1.0);
    float value = 127.0 * (eq_x * eq_x - eq_y * eq_y);
    return value + 127.0;
}

float EquationMode::equation5(float x, float y) {
	float scale = std::max(cols, rows) / 8.0;
    float eq_x = (x- cols/2.0)/scale;
    float eq_y = (y- rows/2.0)/scale;
    return 127.0 * std::sin(eq_x - eq_y) + 127.0;
}

float EquationMode::runCurrentEq(float x, float y) {
	// safety check for future maintainers
	if (transitionEq2 >= numEquations || transitionEq1 >= numEquations || equationIndex >= numEquations) {
		cout << ("Equation Mode tried to segmentation fault!") << endl;
		return 0.0;
	}

	if (transitioning) {
        float alpha = (float)transitionFrameCount / numFrames;

		// member function pointer syntax is kinda upgly
		// refer to these stackoverflow posts for more info:
		// https://stackoverflow.com/questions/1485983/how-can-i-create-a-pointer-to-a-member-function-and-call-it
		// https://stackoverflow.com/questions/15035905/setting-a-pointer-to-a-non-static-member-function
		float value1 = (this->*equations[transitionEq1])(x, y);
		float value2 = (this->*equations[transitionEq2])(x, y);
		return (1 - alpha) * value1 + alpha * value2;
	} else {
		return (this->*equations[equationIndex])(x, y);
	}
}



void EquationMode::update(float dt){
    timeControl++;
	graphAngle += ofGetLastFrameTime() * graphRotationSpeed;
    if (transitioning || firstFrame) {
        updateHeights();
        firstFrame = false;
    } else {
        if (timeControl % 150 == 0) {
            int curr = equationIndex;
            equationIndex = (equationIndex + 1) % numEquations;
            startTransition(curr, equationIndex);
        }
    }
}


void EquationMode::updateHeights() {
    if (transitioning) {
        transitionFrameCount++;
        if (transitionFrameCount >= numFrames) {
            transitioning = false;
            transitionFrameCount = 0;
            equationIndex = transitionEq2;
        }
    }

	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows; y++) {
			heights[x][y] = runCurrentEq(x, y);
		}
	}


    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            m_EquationPixels.setColor(i, j, ofColor(heights[i][j]));
        }
    }
	
	int graphCols = graphDetailMultple * cols;
	int graphRows = graphDetailMultple * rows;
    for (int x = 0; x < graphCols; x++) {
        for (int y = 0; y < graphRows; y++) {
			float height = runCurrentEq(x / float(graphDetailMultple), y / float(graphDetailMultple));
			auto vert = graph.getVertex(x * graphRows + y);
			graph.setVertex(x * graphRows + y, {vert.x, vert.y, graphHeight * height / 255.0});
			graphBacking.setVertex(x * graphRows + y, {vert.x, vert.y, graphHeight * height / 255.0 - backingDistance});

			graph.setColor(x * graphRows + y, heightPixelToMapColor(height));
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
            ProjectorHeightMapPixels.setColor(x, y, heightPixelToMapColor(runCurrentEq(float(x)/10.0, float(y)/10.0)));
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


	// allows backing to be drawn under graph while obscuring parts of it
	ofEnableDepthTest();
	graph.draw();
	graphBacking.draw();
	ofPopMatrix();

	// allows image to be drawn over graph
	ofDisableDepthTest();
	if (transitioning) {
		int eq = transitionFrameCount > numFrames/2 ? transitionEq2 : transitionEq1;
		float imageWidth = scale * equationImages[eq].getWidth()/1000.0;
		float imageHeight = scale * equationImages[eq].getHeight()/1000.0;
		float imageX = x + width/2.0 - imageWidth/2.0;
		float imageY = y + height/8.0 - imageHeight/2.0;
		equationImages[eq].draw(imageX, imageY, imageWidth, imageHeight);

		float fadeAmount = std::abs(transitionFrameCount - numFrames/2)/float(numFrames/2);
		ofSetColor(ofColor::black, 255.0 - 255.0 * fadeAmount);	
		ofDrawRectangle(imageX, imageY, imageWidth, imageHeight);
	} else {
		float imageWidth = scale * equationImages[equationIndex].getWidth()/1000.0;
		float imageHeight = scale * equationImages[equationIndex].getHeight()/1000.0;
		float imageX = x + width/2.0 - imageWidth/2.0;
		float imageY = y + height/8.0 - imageHeight/2.0;
		equationImages[equationIndex].draw(imageX, imageY, imageWidth, imageHeight);
	}
}

ofColor EquationMode::heightPixelToMapColor(int Height) {
    ofColor heightColors[] = {
        {0,0,255},{0,255,255},{0,255,0},{255,255,0},{255,165,0},{255,0,0}
    };
    
    float segments[] = {0.0f, 51.0f, 102.0f, 153.0f, 204.0f, 255.0f};
    
    int i;
    for (i = 0; i < 4; i++) {
        if (Height < segments[i+1]) break;
    }
    float ratio = (Height - segments[i]) / (segments[i+1] - segments[i]);
	return (1.0f - ratio) * heightColors[i] + ratio * heightColors[i + 1];
}

void EquationMode::drawGraphicsForProjector(int x, int y, int width, int height) {
    ofImage(ProjectorHeightMapPixels).draw(x, y, width, height);
}

void EquationMode::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {
    // Draw the current video frame as a base; .
//    m_EquationImage.setFromPixels(m_EquationPixels);
//    m_EquationImage.draw(30, 300, 544, 128);
    //ofImage(heightsForShapeDisplay).draw(x, y, width, height);
    ofImage(ProjectorHeightMapPixels).draw(x, y, width, height);
    
    
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
