#include "ProjectorApp.hpp"
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
    settings.title = "control window";
    auto mainWindow = ofCreateWindow(settings);

    // create display window
	  settings.setSize(600, 600);
	  settings.setPosition(glm::vec2(1020,0));
	  settings.resizable = true;
    settings.title = "public display window";
	  // uncomment next line to share main's OpenGL resources with gui
	  //settings.shareContextWith = mainWindow;
	  auto displayWindow = ofCreateWindow(settings);
	  //settingsWindow->setVerticalSync(false);
    
    // create display window
	  settings.setSize(600, 600);
	  settings.setPosition(glm::vec2(1020,600));
	  settings.resizable = true;
    settings.title = "projector window";
	  auto projectorWindow = ofCreateWindow(settings);
    
    // seperate applications (needed for some openframeworks stuff)
    auto manager = make_shared<AppManager>();
    auto displayApp = make_shared<DisplayApp>(600, 600);
    auto projectorApp = make_shared<ProjectorApp>(600, 600);

    // let the windows see each other
    displayApp->mainApp = manager;
    manager->displayWindow = displayWindow;
    projectorApp->mainApp = manager;
    manager->projectorWindow = projectorWindow;
    manager->cam = &displayApp->cam;

    ofRunApp(mainWindow, manager);
    ofRunApp(displayWindow, displayApp);
    ofRunApp(projectorWindow, projectorApp);
	  ofRunMainLoop();

}
