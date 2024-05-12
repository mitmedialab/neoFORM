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
    // Set the Transform specific hardware parameters here.
    
    pinHeightMin = 50;
    pinHeightMax = 210;
    pinHeightRange = pinHeightMax - pinHeightMin;
    
    // Pin config values, might be abstracted into a single array.
    gainP    = 1.5;
    gainI    = 0.045;
    maxI     = 25;
    deadZone = 2;
    maxSpeed = 200;
    
    // Add serial connection strings to the vector of serial connections.
    serialPorts.push_back("/dev/tty.usbserial-A702YMNV");
    serialPorts.push_back("/dev/tty.usbserial-A702YLM2");
    serialPorts.push_back("/dev/tty.usbserial-A702YMNT");
    serialPorts.push_back("/dev/tty.usbserial-A702YLM6");
    serialPorts.push_back("/dev/tty.usbserial-A702YLM9");
    serialPorts.push_back("/dev/tty.usbserial-A30011Hp");
    

    // Connect to shape display.
    connectToDisplay();
    
    configureBoards();
}

// Secondary Constructor delegates to the primary constructor and adds the kinect reference.
TransformIOManager::TransformIOManager(KinectManager* kinectRef) : TransformIOManager() {
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

ofPixels TransformIOManager::getKinectStream(){
    m_kinectManagerRef->update();
    
    //basic test
    // Uses the opencv cropped contour rectangle to crop the kinect depth pixels to only be in the size and shape of the transform
    ofPixels m_videoPixels = m_kinectManagerRef->getCroppedPixels((m_kinectManagerRef->depthPixels));//video.getPixels();

    
    return m_videoPixels;
}

// This function takes the full surface image and removes all of the non-active zones, so only
// the active zones are left. It does this by defining rectangles for each of the active zones,
// isolating them, and then re-combining them into a single image.
ofPixels TransformIOManager::cropToActiveSurface( ofPixels fullSurface ) {
    // Get the conversion factor for going from the physical dimension of the surface
    // to the pixel dimension of the image.
    float pixelsPerInch = fullSurface.getWidth() / m_Transform_W;
    
    // Create rectangles representing the active zones in the fullSurface image.
    std::vector<ofRectangle> sections = createSections(pixelsPerInch);
    
    // Crop the full surface to just the active zones
    ofPixels combinedActiveZones = combineActiveZones(fullSurface, sections);
    
    // Scale and rotate the combined active zones
    combinedActiveZones.resize(48, 24);
    combinedActiveZones.rotate90(2);
    
    // Return the cropped and transformed image
    return combinedActiveZones;
    
}

// Takes the full surface image, and an array of rectangles representing the active zones,
// and returns a new image containing only the active zones.
ofPixels TransformIOManager::combineActiveZones(ofPixels fullSurface, std::vector<ofRectangle> sections) {
    // Calculate the width of the combined active zones
    int combinedActiveZonesWidth = 0;
    for (const ofRectangle& section : sections) {
        combinedActiveZonesWidth += static_cast<int>(round(section.width));
    }

    // Create a framebuffer with the correct size to hold the pasted zones.
    ofFbo framebuffer;
    framebuffer.allocate(combinedActiveZonesWidth, fullSurface.getHeight(), GL_RGB);

    // Start the x position at the x position of the first section
    int x = 0;

    // Begin drawing into the framebuffer
    framebuffer.begin();

    // Create an image to hold the current cropped zone
    ofImage zoneImage;

    // Loop over the sections
    for (const ofRectangle& section : sections) {
        // Crop the section directly from fullSurface
        ofPixels zone;
        fullSurface.cropTo(
            zone,
            static_cast<int>(round(section.x)),
            static_cast<int>(round(section.y)),
            static_cast<int>(round(section.width)),
            static_cast<int>(round(section.height))
        );

        // Create an image from the cropped pixels
        zoneImage.setFromPixels(zone);

        // Draw the image at the correct x position for the section into the framebuffer.
        zoneImage.draw(x, 0);
        
        // Increment the x position by the width of the section, to make sure everything is consistently spaced.
        x += static_cast<int>(round(section.width));
    }

    // End drawing into the framebuffer
    framebuffer.end();

    // Extract the pixels from the framebuffer
    ofPixels combinedActiveZones;
    framebuffer.readToPixels(combinedActiveZones);
    combinedActiveZones.setImageType(OF_IMAGE_GRAYSCALE);

    // Return the combined active zones as a grayscale ofPixels object.
    return combinedActiveZones;
}

// This function takes the current scaling factor and creates an array of rectangles
// which represent the active zones on the TRANSFORM display.
std::vector<ofRectangle> TransformIOManager::createSections(float pixelsPerInch) {
    std::vector<ofRectangle> sections;

    // Calculate the pixel widths and heights of the active regions
    float activeRegionWidth = (m_Transform_block * pixelsPerInch);
    float activeRegionHeight = (m_Transform_H * pixelsPerInch);

    // Loop over the x positions to create the active region rectangles
    for (float activeXstart : m_activeZoneXstarts) {
        float activePixelX = activeXstart * pixelsPerInch;
        ofRectangle activeRegion(activePixelX, 0, activeRegionWidth, activeRegionHeight);
        sections.push_back(activeRegion);
    }
    
    return sections;
}
