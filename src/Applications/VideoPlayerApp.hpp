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
};

#endif /* VideoPlayerApp_hpp */
