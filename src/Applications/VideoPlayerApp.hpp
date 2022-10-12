//
//  VideoPlayerApp.hpp
//  neoForm
//
//  Created by Jonathan Williams on 8/17/22.
//

#ifndef VideoPlayerApp_hpp
#define VideoPlayerApp_hpp

#include <stdio.h>
#include "Application.hpp"

class VideoPlayerApp : public Application {
public:
    void setup();
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    string appInstructionsText();
    void keyPressed(int key);

    string getName() {return "Video Player";}

private:
    void updateHeights();
    ofVideoPlayer video;
    int calculateBlockNumber(int x_pixel_coord);
    int calculateWithinBlockX(int blockNumber, int x_pixel_coord);
    void setupTransformedPixelMap();
    int getBlockWidth();
    
    int m_blockWidth = 16;
    int m_videoToTransformIndicies[1152];
    int m_videoPixelSize = 2448;
    
    unsigned char * m_videoPixels;
    
    ofPixels plz;
    
    
};

#endif /* VideoPlayerApp_hpp */
