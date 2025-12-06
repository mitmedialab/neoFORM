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
    VideoPlayerApp(SerialShapeIOManager *theCustomShapeDisplayManager);
    void setup();
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    void drawSectionPreviewFrameBuffer(int x, int y, int width, int height);

    string appInstructionsText();
    void keyPressed(int key);
	void switchFile(std::string filename);
	pair<vector<bool*>, vector<string>> getOptions();

    string getName();

private:
	std::string current_name = "";
	bool cut_video = true;
	bool is_esher = true;
	bool last_is_esher = true;

    void updateHeights();
    ofVideoPlayer video;

    ofPixels m_videoPixels;
};

#endif /* VideoPlayerApp_hpp */
