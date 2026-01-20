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
#include "ofVideoPlayer.h"

class VideoPlayerApp : public Application {
public:
    VideoPlayerApp(SerialShapeIOManager *theCustomShapeDisplayManager);
    void setup();
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    void drawSectionPreviewFrameBuffer(int x, int y, int width, int height);

    string appInstructionsText();
    void keyPressed(int key);
	pair<vector<bool*>, vector<string>> getOptions();

    string getName();

private:
    enum VideoSource {
        ESCHER = 0,
        MACHINE = 1
    };

    VideoSource currentVideoIndex = ESCHER;
    VideoSource lastVideoIndex = ESCHER;

    void updateHeights();
    
    // Video resources
	ofVideoPlayer escherVideo;
	ofVideoPlayer machineVideo;
    
    // Video metadata - parallel arrays indexed by VideoSource
    vector<ofVideoPlayer*> videos;
    vector<string> videoNames;
    vector<bool> videosNeedCropping;

    ofPixels m_videoPixels;
};

#endif /* VideoPlayerApp_hpp */
