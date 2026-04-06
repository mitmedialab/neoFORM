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
    VideoPlayerApp(SerialShapeIOManager *theCustomShapeDisplayManager, string mode);
    void setup();
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    void drawSectionPreviewFrameBuffer(int x, int y, int width, int height);

    string appInstructionsText();

    string getName();
    
    // Lifecycle hooks
    void onBecameActive() override;
    void onBecameInactive() override;

private:
    void updateHeights();
    
    string mode;  // "escher" or "machine"
    string displayName;  // "Escher Mode" or "Machine Mode"
    string videoFilename;
    bool needsCropping;
    
    ofVideoPlayer video;
    ofPixels m_videoPixels;
};

#endif /* VideoPlayerApp_hpp */
