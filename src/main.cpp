#include "SettingsApp.hpp"
#include "ofMain.h"
#include "ofApp.h"
#include "AppManager.hpp"
#include <memory>

//========================================================================
int main( ){
    // create main window with specific location
	  ofGLFWWindowSettings settings;
	  settings.setSize(610, 1060);
	  settings.setPosition(glm::vec2(400,0));
	  settings.resizable = true;
    auto mainWindow = ofCreateWindow(settings);

    // create settings window
	  settings.setSize(400, 600);
	  settings.setPosition(glm::vec2(0,0));
	  settings.resizable = false;
	  // uncomment next line to share main's OpenGL resources with gui
	  //settings.shareContextWith = mainWindow;
	  auto settingsWindow = ofCreateWindow(settings);
	  settingsWindow->setVerticalSync(false);
    
    // seperate applications to make ofxGui happy
    auto manager = make_shared<AppManager>();
    auto settingsApp = make_shared<SettingsApp>();

    // let the two windows see each other
    settingsApp->mainApp = manager;
    settingsApp->mainWindow = mainWindow;
    manager->settingsWindow = settingsWindow;

    ofRunApp(mainWindow, manager);
    ofRunApp(settingsWindow, settingsApp);
	  ofRunMainLoop();

}
