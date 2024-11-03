#include "DisplayApp.hpp"
#include "ofMain.h"
#include "ofApp.h"
#include "AppManager.hpp"
#include <memory>

//========================================================================
int main( ){
    // create main window with specific location
	  ofGLFWWindowSettings settings;
	  settings.setSize(1010, 1060);
	  settings.setPosition(glm::vec2(0,0));
	  settings.resizable = true;
    settings.title = "main";
    auto mainWindow = ofCreateWindow(settings);

    // create display window
	  settings.setSize(600, 600);
	  settings.setPosition(glm::vec2(1020,0));
	  settings.resizable = true;
    settings.title = "display";
	  // uncomment next line to share main's OpenGL resources with gui
	  //settings.shareContextWith = mainWindow;
	  auto displayWindow = ofCreateWindow(settings);
	  //settingsWindow->setVerticalSync(false);
    
    // seperate applications to make ofxGui happy
    auto manager = make_shared<AppManager>();
    auto displayApp = make_shared<DisplayApp>();

    // let the two windows see each other
    displayApp->mainApp = manager;
    manager->displayWindow = displayWindow;

    ofRunApp(mainWindow, manager);
    ofRunApp(displayWindow, displayApp);
	  ofRunMainLoop();

}
