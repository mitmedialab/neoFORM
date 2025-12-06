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
	std::string current_name = "";
	bool cut_video = true;
	bool is_escher = true;
	bool last_is_escher = true;

    void updateHeights();
    ofVideoPlayer* cur_video = &escher_video;
	ofVideoPlayer escher_video;
	ofVideoPlayer machine_video;

    ofPixels m_videoPixels;
};

#endif /* VideoPlayerApp_hpp */
