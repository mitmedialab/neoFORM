#pragma once

#include "ofMain.h"
#include "AppManager.hpp"
#include "ofxGui.h"

class SettingsApp : public ofBaseApp{
public:
		void setup();
		void update();
		void draw();
    void exit();
		
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
    shared_ptr<ofAppBaseWindow> mainWindow;
    
    // lets main window access settings window stuff
    friend AppManager;
protected:
    // new gui
    ofxGuiGroup gui;
    vector<ofxButton> modeButtons;
    vector<string> modeNames;
};
