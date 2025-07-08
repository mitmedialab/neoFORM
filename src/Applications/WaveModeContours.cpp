//
//  WaveModeContours.cpp
//  neoForm
//
//  Created by Liam May on 8/9/24.
//

#include "WaveModeContours.hpp"
#include "ofGraphicsConstants.h"
#include "ofxXmlSettings.h"
#include "utils.hpp"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <vector>

#include <cstdlib> // Include for rand() and srand()
#include <ctime>   // Include for time()

#include <iomanip> // Include for std::setprecision

#include <algorithm>

WaveModeContours::WaveModeContours(SerialShapeIOManager *theSerialShapeIOManager, KinectManagerSimple *theKinectManager) : Application(theSerialShapeIOManager) {
    m_kinectManager = theKinectManager;
    setup();
}


void WaveModeContours::setup(){

    // Get the dimensions of the shape display
    cols = (m_CustomShapeDisplayManager)->shapeDisplaySizeX;
    rows = (m_CustomShapeDisplayManager)->shapeDisplaySizeY;

    // Allocate and initialize the internal wave pixels
    m_IntWavePixels.allocate(cols, rows, OF_IMAGE_GRAYSCALE);
    m_IntWavePixels.set(0);

	prevKinectDepth.allocate(cols, rows, OF_IMAGE_GRAYSCALE);

    // Set the raindrop ripple effect parameters
    timeControl = 0.0f; // Initialize timeControl
    rainDropsPerSecond = 0.2; // Example initial value, can be adjusted during runtime
    lastRippleTime = 0.0f; // Initialize the timer

    // Allocate memory for density, velocity, wallMask, and previousWallMask vectors
    density.resize(cols, std::vector<float>(rows, 0));
    velocity.resize(cols, std::vector<float>(rows, 0));
    wallMask.resize(cols, std::vector<bool>(rows, false));
    previousWallMask.resize(cols, std::vector<bool>(rows, false));

    // Initialize contour finder and last contour centroids
    contourFinder;
    lastContourCentroids;

    friction = 0.8;

    // Initialize density and velocity arrays with sinusoidal values.
    // This sets up an initial wave pattern for the fluid simulation.
    // It is optional, the fluid simulation can be started with a blank state.
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

    // Initialize a random seed for raindrop ripple position.
    srand(static_cast<unsigned int>(time(0)));

    ProjectorHeightMapPixels.allocate(cols, rows, OF_IMAGE_COLOR);
    ProjectorHeightMapPixels.setColor(ofColor::black);

    // Initialize water simulation parameters with default values
    // These can be adjusted for different water behaviors
    waterParams.maxDensityChange = 130.0f;
    waterParams.responseCurveStrength = 0.6f;
    waterParams.temporalSmoothingFactor = 0.5f;
    waterParams.inputAmplification = 12.0f;
}


//----------------------------------------------------
// Put the functions for the wave operations here:
//----------------------------------------------------

// Called by solveFluid() on each cell in the fluid grid to update the velocity and density of the simulated fluid.
// The velocity is adjusted based on friction and the difference between the sum of adjacent densities
// and the current cell's density. The density is then updated by adding the new velocity.
float WaveModeContours::getAdjacencyDensitySum(int x, int y){
    float sum = 0;
    if (x - 1 >= 0){sum += density[x - 1][y];}
    if (x + 1 < cols){sum += density[x + 1][y];}
    if (y - 1 >= 0){sum += density[x][y - 1];}
    if (y + 1 < rows){sum += density[x][y + 1];}
    return sum;
}

void WaveModeContours::solveFluid(double progressAmount){
    // Iterate over each cell in the fluid grid
	float adjustedFriction = (1.0 - progressAmount * (1.0 - friction));
    for (int x = 0; x < cols; x++){
        for (int y = 0; y < rows; y++){
        // Update the velocity of the current cell
        // The velocity is adjusted based on friction and the difference between the sum of adjacent densities
        // and the current cell's density. This simulates the fluid dynamics.
        velocity[x][y] = adjustedFriction * velocity[x][y] + (getAdjacencyDensitySum(x, y) - density[x][y] * 4.0) * 0.1;

        // Update the density of the current cell
        // The density is updated by adding the new velocity to the current density.
        density[x][y] = density[x][y] + velocity[x][y] * progressAmount;
        }
    }
}

void WaveModeContours::applyKinectInput() {
    // Get the depth image from the Kinect manager
    ofShortPixels shortPixels = m_kinectManager->getDepthPixels();
    m_kinectManager->cropUsingMask(shortPixels);

    // Apply thresholding and interpolation directly to the 16-bit depth pixel values
	// Shouldn't be needed if nearThreshold and farThreshold are correct
    //m_kinectManager->thresholdInterp(pixels, 200*256, 220*256, 0, 65535);

    // Cast the incoming ofShortPixels data to ofCvGrayscaleImage for the contour finder.
    ofxCvGrayscaleImage grayImage;
    grayImage.allocate(shortPixels.getWidth(), shortPixels.getHeight()); // Allocate with the correct dimensions
    grayImage.setFromPixels(shortPixels);

	// blur to improve downscaling (resizing)
	int scale = std::min(grayImage.width / cols, grayImage.height / rows);
	scale = std::max(3, scale); // in case we're scaling up
    grayImage.blurGaussian(scale * 2 + 1); // makes sure value is odd
	//grayImage.resize(cols, rows);

	ofPixels activeSurface = m_CustomShapeDisplayManager->cropToActiveSurface(grayImage.getPixels());
    grayImage.allocate(activeSurface.getWidth(), activeSurface.getHeight());
    grayImage.setFromPixels(activeSurface);

    // Blur the image to improve interaction "smoothness"
	int blurRange = 1/ m_CustomShapeDisplayManager->getPinSizeInInches();
	blurRange = std::max(1, blurRange);
    //grayImage.blurGaussian(2 * blurRange + 1);
	ofPixels pix = grayImage.getPixels();

	// Apply water simulation with Kinect input
	applyWaterSimulation(pix);

	// Store current frame as previous for next frame's comparison
	prevKinectDepth = pix;
}

void WaveModeContours::update(float dt){

    m_kinectManager->update();
    //updateMask();
	applyKinectInput();
	int iterations = int(0.6 + std::sqrt(5/m_CustomShapeDisplayManager->getPinSizeInInches()));
	iterations = std::max(1, iterations);
	double progressAmount = std::sqrt(2/m_CustomShapeDisplayManager->getPinSizeInInches());
	for (int i = 0; i < iterations; i++) {
		solveFluid(progressAmount/iterations);
	}
    updateHeights();
    //updatePreviousWallMask();

    // Increment timeControl based on delta time
    timeControl += dt;

}

// Processes incoming depth data, finds the contours of detected objects, and updates the matrix of detected obstacles (called walls here).
// If a new wall is detected based on the last calculated matrix of wall positions, a ripple effect is applied to the fluid simulation at that location.
void WaveModeContours::updateMask(){
    // Get the depth image from the Kinect manager and apply a Gaussian blur.
    ofShortPixels pixels = m_kinectManager->getDepthPixels();
    m_kinectManager->cropUsingMask(pixels);

    // Apply thresholding and interpolation directly to the 16-bit depth pixel values
	// Shouldn't be needed if nearThreshold and farThreshold are correct
    //m_kinectManager->thresholdInterp(pixels, 200*256, 220*256, 0, 65535);

    // Cast the incoming ofShortPixels data to ofCvGrayscaleImage for the contour finder.
    ofxCvGrayscaleImage grayImage;
    grayImage.allocate(pixels.getWidth(), pixels.getHeight()); // Allocate with the correct dimensions
    grayImage.setFromPixels(pixels);

    // Blur the image to reduce aliasing
    grayImage.blurGaussian(1);

    // Calculate the maxArea for the contour finder based on the cropped image size.
    int maxArea = ( grayImage.getWidth() * grayImage.getHeight() ) / 2;

    // Find the contours in the cropped depth image.
    contourFinder.findContours(grayImage, 100, maxArea, 1, false);

    maskPixels = grayImage.getPixels();
    std::vector<ofPoint> currentCentroids;

    float dist;
    int numBlobs = contourFinder.nBlobs;

    // For each detected blob, the centroid is calculated and stored.
    // The mask pixels are updated based on the distance from the centroid.
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

    // Resizes the mask image to match the shape display size.
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

    // Apply a raindrop ripple effect at a random location on the grid at the specified interval.
    while (timeControl - lastRippleTime >= currentRainDropInterval) {
        int randomX = rand() % cols; // Generate a random x-coordinate within the grid
        int randomY = rand() % rows; // Generate a random y-coordinate within the grid
        applyRippleEffect(randomX, randomY);
        lastRippleTime += currentRainDropInterval; // Not a pure reset, allows very small intervals

		recalculateRainInterval();
    }

    // Updates the previous wall mask and stores the current centroids.
    updatePreviousWallMask();
    lastContourCentroids = currentCentroids;
}

void WaveModeContours::recalculateRainInterval() {
		double randZeroToOne = double(rand()) / RAND_MAX;
		// A number between -1 and 1, the higher the more random the intervals will be.
		const double randomFactor = -1.8;
		// Random interval, with expected value 0.0 / rainDropsPerSecond
		currentRainDropInterval = (1.0 * randomFactor * randZeroToOne + 1.0 - randomFactor) / rainDropsPerSecond;
}

// Applies a raindrop-like ripple effect to the surface at the given coordinates.
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

void WaveModeContours::updateHeights(){

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            m_IntWavePixels.setColor(x, y, ofColor(ofClamp(127 + velocity[x][y], 0, 255)));
        }
    }


    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {

            int flattenedIndex = heightsForShapeDisplay.getPixelIndex(x, y);

            heightsForShapeDisplay[flattenedIndex] = m_IntWavePixels[flattenedIndex];

            std::tuple<int, int, int> projector_color = heightPixelToMapColor(m_IntWavePixels[flattenedIndex]);
            int r, g, b;
            std::tie(r, g, b) = projector_color;
            ProjectorHeightMapPixels.setColor(x, y, ofColor(r, g, b));
        }
    }
    // Do not need to flip anymore, but uncomment this if it is necessary to flip the image.
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

// This is responsible for drawing the on screen preview of the app's behavior.
void WaveModeContours::drawGraphicsForShapeDisplay(int x, int y, int width, int height) {

    //*** Draw the color pixels for reference.
    ofImage colorImg = m_kinectManager->getColorPixels();
    colorImg.draw(2, 2, colorImg.getWidth(), colorImg.getHeight());

    //*** Overlay the depth image on top of the color image.
    // Set the color to white with 50% opacity
    ofSetColor(255, 255, 255, 127);

    // Draw the depth image
    ofShortImage depthImg = m_kinectManager->getDepthPixels();
    depthImg.draw(2, 2, depthImg.getWidth(), depthImg.getHeight());

    // Reset the color to fully opaque white
    ofSetColor(255, 255, 255, 255);

    //*** Draw the mask rectangle
    drawPreviewMaskRectangle();

    //*** Preview shape display pixels
    m_kinectManager->cropUsingMask(depthImg.getPixels());
    depthImg.draw(2, 400, depthImg.getWidth(), depthImg.getHeight());

    //*** Contours are disabled, but maybe they will be useful in the future.
    //m_kinectManager->drawContours();

    if ( m_CustomShapeDisplayManager->getShapeDisplayName() == "TRANSFORM" ) {
        //*** Draw preview of the actuated pixel regions (sections).
        drawPreviewActuatedSections();
    }
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
                        m_kinectManager->mask.getX(),
                        m_kinectManager->mask.getY(),
                        m_kinectManager->mask.getWidth(),
                        m_kinectManager->mask.getHeight()
        );

        // Unset color and fill for future drawing operations
        ofSetColor(255, 255, 255);
        ofFill();
}

void WaveModeContours::drawPreviewActuatedSections() {
    // Get the width in inches of the the full transform surface (need to cast shape display manager object first).
    float transformWidth = ((TransformIOManager*)m_CustomShapeDisplayManager)->m_Transform_W;

    // Get the actuated section dimensions from the CustomShapeDisplayManager
    float pixelsPerInch = m_kinectManager->mask.getWidth() / transformWidth;

    std::vector<ofRectangle> sections = m_CustomShapeDisplayManager->createSections(pixelsPerInch);

    // Create a frame buffer with the same dimensions as the cropped signal.
    ofFbo fbo;
    fbo.allocate(m_kinectManager->mask.getWidth(), m_kinectManager->mask.getHeight(), GL_RGBA); // GL_RGBA for transparency

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
    fbo.draw(m_kinectManager->mask);
}

// Control the raindrop ripple effect with the keyboard.
// Uses 2 different scales for the rainDropsPerSecond value, depending on whether it is less than 1 or greater than 1.
// The rainDropsPerSecond value is changed by 0.2 if it is less than 1, and by 1 if it is greater than 1; this is to give more options for light rain.
void WaveModeContours::keyPressed(int Key) {

    // Look for a ']', a forward arrow key press, or a an up arrow key press to increase the rainDropsPerSecond value.
    if (Key == 93 || Key == 57358 || Key == 57357) {
        // If rainDropsPerSecond is less than 1, increment it by .2
        if (rainDropsPerSecond < 1) {
            rainDropsPerSecond += 0.2;
        } else {
            // Otherwise, increment it by 1
            rainDropsPerSecond += 1;
        }

		recalculateRainInterval();
    }

    // Look for a '[', backward arrow key press or a down arrow key to decrease the rainDropsPerSecond value.
    if (Key == 91 || Key == 57356 || Key == 57359) {
        // If rainDropsPerSecond is less than or equal to, decrement it by .2
        if (rainDropsPerSecond > 0.01 && rainDropsPerSecond <= 1) { // Don't go below zero, but use 0.01 instead of 0 to account for minor floating point errors
            rainDropsPerSecond -= 0.2;
        } else if (rainDropsPerSecond > 1) {
            // Otherwise, decrement it by 1
            rainDropsPerSecond -= 1;
        }

		recalculateRainInterval();
    }

}

string WaveModeContours::appInstructionsText() {
    string instructions = (string) "Rainfall: \n";
    instructions += "Use arrow or bracket keys to change rainfall.\n";

    // Use stringstream to format rainDropsPerSecond to one decimal place
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(1) << rainDropsPerSecond;
    instructions += "Raindrops per second is " + stream.str() + "\n";
    return instructions;
}

// Apply water simulation effects using the current depth frame
void WaveModeContours::applyWaterSimulation(const ofPixels& currentDepthFrame) {
    // This method implements a physically-based water simulation with Kinect interaction:
    // 1. Compute depth differences between current and previous frames
    // 2. Apply non-linear sigmoid response for natural falloff behavior
    // 3. Apply temporal smoothing for fluid-like transitions
    // 4. Apply the resulting forces to the density field

    // Initialize smoothedChanges array if it doesn't exist yet (persists between calls)
    // Declared as static to maintain state across multiple calls; this means we don't have to initialize it in the class constructor.
    static std::vector<std::vector<float>> smoothedChanges;
    if (smoothedChanges.empty()) {
        smoothedChanges.resize(cols, std::vector<float>(rows, 0.0f));
    }

    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            // Calculate raw change based on depth difference
            float depthDifference = currentDepthFrame.getColor(x, y).getBrightness() -
                                   prevKinectDepth.getColor(x, y).getBrightness();
            float rawChange = waterParams.inputAmplification * depthDifference;

            // Apply non-linear response curve (sigmoid/tanh) for more natural falloff
            // This creates a gentle response to small changes and saturation for large changes
            float responseScale = waterParams.maxDensityChange * 1.0f;
            float nonLinearResponse = responseScale *
                                     tanh(rawChange * waterParams.responseCurveStrength / responseScale);

            // Apply temporal smoothing by blending with previous frame's changes
            // This creates more fluid, gradual transitions like real water
            float newChange = waterParams.temporalSmoothingFactor * smoothedChanges[x][y] +
                             (1.0f - waterParams.temporalSmoothingFactor) * nonLinearResponse;

            // Store the smoothed value for the next frame
            smoothedChanges[x][y] = newChange;

            // Apply the processed change to the density field
            density[x][y] -= newChange;
        }
    }
}
