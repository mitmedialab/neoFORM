#include "ofMain.h"
#include "ofApp.h"
#include "AppManager.hpp"
#include <memory>

//========================================================================
int main( ){
    // create main window with specific location
	  ofGLFWWindowSettings settings;
	  settings.setSize(1216, 1068);
	  settings.setPosition(glm::vec2(400,0));
	  settings.resizable = true;
    auto mainWindow = ofCreateWindow(settings);

    // create settings window
	  settings.setSize(400, 1000);
	  settings.setPosition(glm::vec2(0,0));
	  settings.resizable = false;
	  // uncomment next line to share main's OpenGL resources with gui
	  //settings.shareContextWith = mainWindow;
	  auto settingsWindow = ofCreateWindow(settings);
	  settingsWindow->setVerticalSync(false);
    
    auto manager = make_shared<AppManager>();
    manager->setupSettingsWindow();
    ofAddListener(settingsWindow->events().draw, manager.get(), &AppManager::drawSettingsWindow);

    ofRunApp(mainWindow, manager);
	  ofRunMainLoop();

}
