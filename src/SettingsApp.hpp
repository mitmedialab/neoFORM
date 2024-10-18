#pragma once

#include "ofMain.h"
#include "AppManager.hpp"
#include <memory>

class SettingsApp : public ofBaseApp{
public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    // main window
    shared_ptr<AppManager> mainApp;
    
    // lets main window access settings window stuff
    friend AppManager;
protected:
    // new gui
    ofxPanel gui;
    vector<ofxButton> modeButtons;
    vector<string> modeNames;
};
